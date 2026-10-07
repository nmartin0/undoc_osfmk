/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * $Id: pmap.c,v 1.34 1995/02/17 03:08:48 sean Exp $
 */

/*
 *	File:	pmap.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	(These guys wrote the Vax version)
 *
 *	Physical Map management code for Intel i386, i486, and i860.
 *
 *	Manages physical address maps.
 *
 *	In addition to hardware address maps, this
 *	module is called upon to provide software-use-only
 *	maps which may or may not be stored in the same
 *	form as hardware maps.  These pseudo-maps are
 *	used to store intermediate results from copy
 *	operations to and from address spaces.
 *
 *	Since the information managed by this module is
 *	also stored by the logical address mapping module,
 *	this module may throw away valid virtual-to-physical
 *	mappings at almost any time.  However, invalidations
 *	of virtual-to-physical mappings must be done as
 *	requested.
 *
 *	In order to cope with hardware architectures which
 *	make virtual-to-physical map invalidates expensive,
 *	this module may delay invalidate or reduced protection
 *	operations until such time as they are actually
 *	necessary.  This module is given full information as
 *	to which processors are currently using which maps,
 *	and to when physical maps must be made correct.
 */

#include <mach_assert.h>
#include <cpus.h>

#include <mach/machine/vm_types.h>

#include <mach/boolean.h>
#include <kern/thread.h>
#include <kern/zalloc.h>

#include <kern/lock.h>

#include <vm/pmap.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <mach/vm_prot.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_user.h>

#include <mach/machine/vm_param.h>
#include <machine/thread.h>

#include <mach_kdb.h>
#include <machine/db_machdep.h>

#define	FROZEN_PAGES	NORMA2

#if	PARAGON860
extern	boot_mk_verbose;
#else	PARAGON860
#define	boot_mk_verbose 1
#endif	PARAGON860

#define CACHEDIRBASE PARAGON860

#if	i860
#include <mach/vm_attributes.h>
#if	iPSC860
#include <i860ipsc/nodehw.h>
#endif	iPSC860

void	set_dirbase();		/* forward */

#if	FROZEN_PAGES

/*
 * Frozen VM pages were invented to support the NORMA2 implementation. A frozen
 * VM page is much like a 'wired' page in that it remains mapped and will take
 * no translation faults; nice for DMA operations. Where a frozen pages differs
 * from a wired page is the frozen page remains on the active queue while
 * the act of wiring a page removes it from all queues. The pageout daemon
 * has been educated to stay hands-off from frozen pages (although it will
 * move a frozen page to the end of the active queue). Under no circumstances
 * will the pageout daemon attempt to pageout a frozen page. The real reason
 * for frozen pages is the low overhead in thawing them. Frozen pages can be
 * thawed from interrupt level; in our case by the message-coprocessor.
 *
 * Frozen pages are utilized by NORMA2 in the following fashion: an offnode Mach
 * message with OOL (Out-Of-Line) data is destined for a local user task.
 * The act of receiving the message will create user address space for the OOL
 * data, freeze it and inform the message-coprocess to start streaming data
 * into the frozen VM page(s). When the page has been filled, the NORMA2
 * receive engine will then thaw the VM page thus making it a canidate for 
 * pageout. All this reduces the amount of non-pageable memory associated
 * with large message buffered in kernel space (NORMA1).
 */

#if	MACH_ASSERT

boolean_t	check_me=TRUE;	/* frozen page debug sanity checks */
int frozen_stop_here = 0;
int pmap_check_frozen_fail = 0;
int pmap_check_frozen_retry_total = 0;

/** this macro moved to the call location */
/**#define CHECK_FROZEN(cpte) 	
/**/
#else	/* MACH_ASSERT */

#define CHECK_FROZEN(cpte)

#endif	/* MACH_ASSERT */

/*
 * VM page boolean vector (one byte per page) which indicates wheather a VM
 * page (comprized of 'n' physical frames) is frozen. 'frozen' imples the
 * pageout daemon will not pageout the VM page, worst it will do is move it to
 * the end of page queue (active or inactive) which it is in. See vm_pageout.c
 */ 
unsigned char   *pmap_frozen_page_list;

#define isfrozen(pte)	(((pte) & INTEL_PTE_VALID) && \
				pmap_is_vm_page_frozen(pte_to_pa((pte))))
#endif	/* FROZEN_PAGES */

vm_offset_t	phystokv();
#define	PA_TO_PTE(pa)	(pa_to_pte((pa)))
#define	iswired(pte)	((pte) & INTEL_PTE_wired)
#endif	i860

#if	i386
#define	PA_TO_PTE(pa)	(pa_to_pte((pa) - VM_MIN_KERNEL_ADDRESS))
#define	iswired(pte)	((pte) & INTEL_PTE_WIRED)
#endif	i386

#ifdef	ORC
#define	OLIVETTICACHE	1
#endif	ORC

#ifndef	OLIVETTICACHE
#define	WRITE_PTE(pte_p, pte_entry)		*(pte_p) = (pte_entry);
#define	WRITE_PTE_FAST(pte_p, pte_entry)	*(pte_p) = (pte_entry);
#else	OLIVETTICACHE

/* This gross kludgery is needed for Olivetti XP7 & XP9 boxes to get
 * around an apparent hardware bug. Other than at startup it doesn't
 * affect run-time performance very much, so we leave it in for all
 * machines.
 */
extern	unsigned	*pstart();
#define CACHE_LINE	8
#define CACHE_SIZE	512
#define CACHE_PAGE	0x1000;

#define	WRITE_PTE(pte_p, pte_entry) { write_pte(pte_p, pte_entry); }

write_pte(pte_p, pte_entry)
pt_entry_t	*pte_p, pte_entry;
{
	unsigned long count;
	volatile unsigned long hold, *addr1, *addr2;

	if ( pte_entry != *pte_p )
		*pte_p = pte_entry;
	else {
		/* This isn't necessarily the optimal algorithm */
		addr1 = (unsigned long *)pstart;
		for (count = 0; count < CACHE_SIZE; count++) {
			addr2 = addr1 + CACHE_PAGE;
			hold = *addr1;		/* clear cache bank - A - */
			hold = *addr2;		/* clear cache bank - B - */
			addr1 += CACHE_LINE;
		}
	}
}

#define	WRITE_PTE_FAST(pte_p, pte_entry)*pte_p = pte_entry;

#endif	OLIVETTICACHE

/*
 *	Private data structures.
 */

/*
 *	For each vm_page_t, there is a list of all currently
 *	valid virtual mappings of that page.  An entry is
 *	a pv_entry_t; the list is the pv_table.
 */

typedef struct pv_entry {
	struct pv_entry	*next;		/* next pv_entry */
	pmap_t		pmap;		/* pmap where mapping lies */
	vm_offset_t	va;		/* virtual address for mapping */
} *pv_entry_t;

#define PV_ENTRY_NULL	((pv_entry_t) 0)

pv_entry_t	pv_head_table;		/* array of entries, one per page */

/*
 *	pv_list entries are kept on a list that can only be accessed
 *	with the pmap system locked (at SPLVM, not in the cpus_active set).
 *	The list is refilled from the pv_list_zone if it becomes empty.
 */
pv_entry_t	pv_free_list;		/* free list at SPLVM */
decl_simple_lock_data(, pv_free_list_lock)

#define	PV_ALLOC(pv_e) { \
	SIMPLE_LOCK(&pv_free_list_lock); \
	if ((pv_e = pv_free_list) != 0) { \
	    pv_free_list = pv_e->next; \
	} \
	SIMPLE_UNLOCK(&pv_free_list_lock); \
}

#define	PV_FREE(pv_e) { \
	SIMPLE_LOCK(&pv_free_list_lock); \
	pv_e->next = pv_free_list; \
	pv_free_list = pv_e; \
	SIMPLE_UNLOCK(&pv_free_list_lock); \
}

zone_t		pv_list_zone;		/* zone of pv_entry structures */

/*
 *	Each entry in the pv_head_table is locked by a bit in the
 *	pv_lock_table.  The lock bits are accessed by the physical
 *	address of the page they lock.
 */

char	*pv_lock_table;		/* pointer to array of bits */
#define pv_lock_table_size(n)	(((n)+BYTE_SIZE-1)/BYTE_SIZE)

/*
 *	First and last physical addresses that we maintain any information
 *	for.  Initialized to zero so that pmap operations done before
 *	pmap_init won't touch any non-existent structures.
 */
vm_offset_t	vm_first_phys = (vm_offset_t) 0;
vm_offset_t	vm_last_phys  = (vm_offset_t) 0;
boolean_t	pmap_initialized = FALSE;/* Has pmap_init completed? */

/*
 *	Index into pv_head table, its lock bits, and the modify/reference
 *	bits starting at vm_first_phys.
 */

#if	PARAGON860
	/*
	 *	Memory can be in two big non-contiguous chunks
	 *	(baseboard and on an expansion card).
	 *	Use a real function to map physical pages
	 *	onto an entry in the pv list.
	 */
extern int	pa_index();
#else	/* PARAGON860 */
#define pa_index(pa)	(atop(pa - vm_first_phys))
#endif	/* PARAGON860 */

#define pai_to_pvh(pai)		(&pv_head_table[pai])
#define lock_pvh_pai(pai)	(bit_lock(pai, pv_lock_table))
#define unlock_pvh_pai(pai)	(bit_unlock(pai, pv_lock_table))

/*
 *	Array of physical page attribites for managed pages.
 *	One byte per physical page.
 */
char	*pmap_phys_attributes;

/*
 *	Physical page attributes.  Copy bits from PTE definition.
 */
#define	PHYS_MODIFIED	INTEL_PTE_MOD	/* page modified */
#define	PHYS_REFERENCED	INTEL_PTE_REF	/* page referenced */

/*
 *	Amount of virtual memory mapped by one
 *	page-directory entry.
 */
#define	PDE_MAPPED_SIZE		(pdetova(1))

/*
 *	We allocate page table pages directly from the VM system
 *	through this object.  It maps physical memory.
 */
vm_object_t	pmap_object = VM_OBJECT_NULL;

/*
 *	Locking and TLB invalidation
 */

/*
 *	Locking Protocols:
 *
 *	There are two structures in the pmap module that need locking:
 *	the pmaps themselves, and the per-page pv_lists (which are locked
 *	by locking the pv_lock_table entry that corresponds to the pv_head
 *	for the list in question.)  Most routines want to lock a pmap and
 *	then do operations in it that require pv_list locking -- however
 *	pmap_remove_all and pmap_copy_on_write operate on a physical page
 *	basis and want to do the locking in the reverse order, i.e. lock
 *	a pv_list and then go through all the pmaps referenced by that list.
 *	To protect against deadlock between these two cases, the pmap_lock
 *	is used.  There are three different locking protocols as a result:
 *
 *  1.  pmap operations only (pmap_extract, pmap_access, ...)  Lock only
 *		the pmap.
 *
 *  2.  pmap-based operations (pmap_enter, pmap_remove, ...)  Get a read
 *		lock on the pmap_lock (shared read), then lock the pmap
 *		and finally the pv_lists as needed [i.e. pmap lock before
 *		pv_list lock.]
 *
 *  3.  pv_list-based operations (pmap_remove_all, pmap_copy_on_write, ...)
 *		Get a write lock on the pmap_lock (exclusive write); this
 *		also guaranteees exclusive access to the pv_lists.  Lock the
 *		pmaps as needed.
 *
 *	At no time may any routine hold more than one pmap lock or more than
 *	one pv_list lock.  Because interrupt level routines can allocate
 *	mbufs and cause pmap_enter's, the pmap_lock and the lock on the
 *	kernel_pmap can only be held at splvm.
 */

#if	NCPUS > 1
/*
 *	We raise the interrupt level to splvm, to block interprocessor
 *	interrupts during pmap operations.  We must take the CPU out of
 *	the cpus_active set while interrupts are blocked.
 */
#define SPLVM(spl)	{ \
	spl = sploff(); \
	i_bit_clear(cpu_number(), &cpus_active); \
}

#define SPLX(spl)	{ \
	i_bit_set(cpu_number(), &cpus_active); \
	splon(spl); \
}

/*
 *	Lock on pmap system
 */
lock_data_t	pmap_system_lock;

#define PMAP_READ_LOCK(pmap, spl) { \
	SPLVM(spl); \
	lock_read(&pmap_system_lock); \
	SIMPLE_LOCK(&(pmap)->lock); \
}

#define PMAP_WRITE_LOCK(spl) { \
	SPLVM(spl); \
	lock_write(&pmap_system_lock); \
}

#define PMAP_READ_UNLOCK(pmap, spl) { \
	SIMPLE_UNLOCK(&(pmap)->lock); \
	lock_read_done(&pmap_system_lock); \
	SPLX(spl); \
}

#define PMAP_WRITE_UNLOCK(spl) { \
	lock_write_done(&pmap_system_lock); \
	SPLX(spl); \
}

#define PMAP_WRITE_TO_READ_LOCK(pmap) { \
	SIMPLE_LOCK(&(pmap)->lock); \
	lock_write_to_read(&pmap_system_lock); \
}

#define LOCK_PVH(index)		(lock_pvh_pai(index))

#define UNLOCK_PVH(index)	(unlock_pvh_pai(index))

#define PMAP_UPDATE_TLBS(pmap, s, e) if ((pmap)->private_pmap == 0) \
{ \
	cpu_set	cpu_mask = 1 << cpu_number(); \
	cpu_set	users; \
 \
	/* Since the pmap is locked, other updates are locked */ \
	/* out, and any pmap_activate has finished. */ \
 \
	/* invalidate msgp TLB if kernel pmap */ \
	if ((pmap) == kernel_pmap) { \
		call_pmap_update_interrupt(); \
	} \
	/* find other cpus using the pmap */ \
	users = (pmap)->cpus_using & ~cpu_mask; \
	if (users) { \
	    /* signal them, and wait for them to finish */ \
	    /* using the pmap */ \
	    signal_cpus(users, (pmap), (s), (e)); \
	    while ((pmap)->cpus_using & cpus_active & ~cpu_mask) continue; \
	} \
 \
	/* invalidate our own TLB if pmap is in use */ \
	if ((pmap)->cpus_using & cpu_mask) { \
	    INVALIDATE_TLB((s), (e)); \
	} \
}

#else	NCPUS > 1

#define SPLVM(spl)
#define SPLX(spl)

#define PMAP_READ_LOCK(pmap, spl)	SPLVM(spl)
#define PMAP_WRITE_LOCK(spl)		SPLVM(spl)
#define PMAP_READ_UNLOCK(pmap, spl)	SPLX(spl)
#define PMAP_WRITE_UNLOCK(spl)		SPLX(spl)
#define PMAP_WRITE_TO_READ_LOCK(pmap)

#define LOCK_PVH(index)
#define UNLOCK_PVH(index)

#if	MCMSG
#define PMAP_UPDATE_TLBS(pmap, s, e) if ((pmap)->private_pmap == 0) { \
	/* invalidate msgp TLB if kernel pmap */ \
	if ((pmap) == kernel_pmap) { \
		call_pmap_update_interrupt(); \
	} \
	/* invalidate our own TLB if pmap is in use */ \
	if ((pmap)->cpus_using) { \
	    INVALIDATE_TLB((s), (e)); \
	} \
}
#else	MCMSG
#define PMAP_UPDATE_TLBS(pmap, s, e) if ((pmap)->private_pmap == 0) { \
	/* invalidate our own TLB if pmap is in use */ \
	if ((pmap)->cpus_using) { \
	    INVALIDATE_TLB((s), (e)); \
	} \
}
#endif	MCMSG

#endif	NCPUS > 1

#define MAX_TBIS_SIZE	32		/* > this -> TBIA */ /* XXX */

#define INVALIDATE_TLB(s, e)	flush_tlb()



#if	NCPUS > 1
/*
 *	Structures to keep track of pending TLB invalidations
 */

#define UPDATE_LIST_SIZE	4
/* should be 1 on i860 ? */

struct pmap_update_item {
	pmap_t		pmap;		/* pmap to invalidate */
	vm_offset_t	start;		/* start address to invalidate */
	vm_offset_t	end;		/* end address to invalidate */
} ;

typedef	struct pmap_update_item	*pmap_update_item_t;

/*
 *	List of pmap updates.  If the list overflows,
 *	the last entry is changed to invalidate all.
 */
struct pmap_update_list {
	decl_simple_lock_data(,	lock)
	int			count;
	struct pmap_update_item	item[UPDATE_LIST_SIZE];
} ;
typedef	struct pmap_update_list	*pmap_update_list_t;

struct pmap_update_list	cpu_update_list[NCPUS];

#endif	NCPUS > 1

/*
 *	Other useful macros.
 */
#define current_pmap()		(vm_map_pmap(current_thread()->task->map))
#define pmap_in_use(pmap, cpu)	(((pmap)->cpus_using & (1 << (cpu))) != 0)

struct pmap	kernel_pmap_store;
pmap_t		kernel_pmap;

struct zone	*pmap_zone;		/* zone of pmap structures */

int		pmap_debug = 0;		/* flag for debugging prints */
int		ptes_per_vm_page;	/* number of hardware ptes needed
					   to map one VM page. */
unsigned int	inuse_ptepages_count = 0;	/* debugging */

extern char end;
/*
 * Page directory for kernel.
 */
pt_entry_t	*kpde = 0;	/* set by start.s - keep out of bss */

void pmap_remove_range();	/* forward */
#if	NCPUS > 1
void signal_cpus(cpu_set, pmap_t, vm_offset_t, vm_offset_t);
#endif	NCPUS > 1

#if	i860
int	paging_enabled = 0;			/* MMU turned on */
void	pmap_bootstrap_i860_physmem();		/* map physical memory */
void	pmap_bootstrap_i860_virtmem();		/* make tables for vm */
void	pmap_bootstrap_i860_dirty();		/* pre-mark pages as dirty */
void	pmap_bootstrap_i860_uncache_tables();	/* mark page tables uncached */
void	pmap_bootstrap_i860_trap_page();	/* get a page for ttrap.s */

/*
 *	The following variable i860_global_pmap_cache_policy
 *	can be used to universally restrict cache policy at run
 *	time.  (It cannot be used to "promote" normally uncached
 *	pages to some state of cacheability).  Under normal
 *	circumstances, it is set to 0.
 *	Legal values are:
 *
 *	0			-- "normal"
 *
 *	INTEL_PTE_WTHRU		-- restrict cache to write-thru.
 *
 *	INTEL_PTE_NCACHE	-- disable all data cacheing.
 *
 *	INTEL_PTE_NCACHE|INTEL_PTE_WTHRU	-- redundant, but complete.
 *
 *	If other bits are set things will not work...
 *
 */
pt_entry_t	i860_global_pmap_cache_policy;

#if	iPSC860 || PARAGON860
void	pmap_bootstrap_i860_io();		/* map some i/o devices */
#endif	iPSC860 || PARAGON860

#if	i860XP
#define	SHARING_FAULTS	0	/* physical tags; don't need this */
#else	i860XP
#define SHARING_FAULTS	1
#endif	i860XP

#if	PARAGON860
extern boolean_t	paragon_is_dram_address();
#endif	PARAGON860

#endif	i860

#if  DEBUG_ALIAS
#define PMAP_ALIAS_MAX 32
struct pmap_alias {
        vm_offset_t rpc;
        pmap_t pmap;
        vm_offset_t va;
        int cookie;
#define PMAP_ALIAS_COOKIE 0xdeadbeef
} pmap_aliasbuf[PMAP_ALIAS_MAX];
int pmap_alias_index = 0;
extern vm_offset_t get_rpc();

#endif  /* DEBUG_ALIAS */

/*
 *	Given an offset and a map, compute the address of the
 *	pte.  If the address is invalid with respect to the map
 *	then PT_ENTRY_NULL is returned (and the map may need to grow).
 *
 *	This is only used internally.
 */
pt_entry_t *pmap_pte(pmap, addr)
	register pmap_t		pmap;
	register vm_offset_t	addr;
{
	register pt_entry_t	*ptp;
	register pt_entry_t	pte;

	if (pmap->dirbase == 0)
		return(PT_ENTRY_NULL);
	pte = pmap->dirbase[pdenum(addr)];
	if ((pte & INTEL_PTE_VALID) == 0)
		return(PT_ENTRY_NULL);
	ptp = (pt_entry_t *)ptetokv(pte);
	return(&ptp[ptenum(addr)]);

}

#define	pmap_pde(pmap, addr) (&(pmap)->dirbase[pdenum(addr)])

#define DEBUG_PTE_PAGE	0

#if	DEBUG_PTE_PAGE
void ptep_check(ptep)
	ptep_t	ptep;
{
	register pt_entry_t	*pte, *epte;
	int			ctu, ctw;

	/* check the use and wired counts */
	if (ptep == PTE_PAGE_NULL)
		return;
	pte = pmap_pte(ptep->pmap, ptep->va);
	epte = pte + INTEL_PGBYTES/sizeof(pt_entry_t);
	ctu = 0;
	ctw = 0;
	while (pte < epte) {
		if (pte->pfn != 0) {
			ctu++;
			if (pte->wired)
				ctw++;
		}
		pte += ptes_per_vm_page;
	}

	if (ctu != ptep->use_count || ctw != ptep->wired_count) {
		printf("use %d wired %d - actual use %d wired %d\n",
		    	ptep->use_count, ptep->wired_count, ctu, ctw);
		panic("pte count");
	}
}
#endif	DEBUG_PTE_PAGE

/*
 *	Map memory at initialization.  The physical addresses being
 *	mapped are not managed and are never unmapped.
 *
 *	For now, VM is already on, we only need to map the
 *	specified memory.
 */
vm_offset_t pmap_map(virt, start, end, prot)
	register vm_offset_t	virt;
	register vm_offset_t	start;
	register vm_offset_t	end;
	register int		prot;
{
	register int		ps;

	ps = PAGE_SIZE;
	while (start < end) {
		pmap_enter(kernel_pmap, virt, start, prot, FALSE);
		virt += ps;
		start += ps;
	}
	return(virt);
}

/*
 *	Back-door routine for mapping kernel VM at initialization.  
 * 	Useful for mapping memory outside the range
 *	[vm_first_phys, vm_last_phys) (i.e., devices).
 *	Otherwise like pmap_map.
#if	i860
 *      Sets no-cache, A, D.
#endif	i860
 */
vm_offset_t pmap_map_bd(virt, start, end, prot)
	register vm_offset_t	virt;
	register vm_offset_t	start;
	register vm_offset_t	end;
	vm_prot_t		prot;
{
	register pt_entry_t	template;
	register pt_entry_t	*pte;
	register int		i, ps;

	template = pa_to_pte(start)
#if	i860
		| i860_global_pmap_cache_policy
		| INTEL_PTE_NCACHE
		| INTEL_PTE_WTHRU
		| INTEL_PTE_REF
		| INTEL_PTE_MOD
#endif	i860
		| INTEL_PTE_VALID;
	if (prot & VM_PROT_WRITE)
	    template |= INTEL_PTE_WRITE;

	ps = PAGE_SIZE;
	while (start < end) {
		pte = pmap_pte(kernel_pmap, virt);
		if (pte == PT_ENTRY_NULL)
			panic("pmap_map_bd: Invalid kernel address\n");
		i = ptes_per_vm_page;
		do {
		    WRITE_PTE_FAST(pte, template)
		    pte++;
		    pte_increment_pa(template);
		} while (--i > 0);
		virt += ps;
		start += ps;
	}
	return(virt);
}

extern int		cnvmem;
extern	char		*first_avail;
extern	vm_offset_t	virtual_avail, virtual_end;
extern	vm_offset_t	avail_start, avail_end;

/*
 *	Bootstrap the system enough to run with virtual memory.
 *	Map the kernel's code and data, and allocate the system page table.
 *	Called with mapping OFF.  Page_size must already be set.
 *
 *	Parameters:
 *	load_start:	PA where kernel was loaded
 *	avail_start	PA of first available physical page -
 *			   after kernel page tables
 *	avail_end	PA of last available physical page
 *	virtual_avail	VA of first available page -
 *			   after kernel page tables
 *	virtual_end	VA of last available page -
 *			   end of kernel address space
 *
 *	&start_text	start of kernel text
 *	&etext		end of kernel text
 */

void pmap_bootstrap(load_start)
	vm_offset_t	load_start;
{
	vm_offset_t	va, tva;
	pt_entry_t	template;
	pt_entry_t	*pde, *pte, *ptend;
	vm_size_t	morevm = 96*1024*1024;	/* 96MB default kernel_map */

	/*
	 *	Set ptes_per_vm_page for general use.
	 */
	ptes_per_vm_page = page_size / INTEL_PGBYTES;

	/*
	 *	The kernel's pmap is statically allocated so we don't
	 *	have to use pmap_create, which is unlikely to work
	 *	correctly at this part of the boot sequence.
	 */

	kernel_pmap = &kernel_pmap_store;

#if	NCPUS > 1
	lock_init(&pmap_system_lock, FALSE);	/* NOT a sleep lock */
#endif	NCPUS > 1

	simple_lock_init(&kernel_pmap->lock);

	kernel_pmap->ref_count = 1;


	/*
	 * allow kernel_map size over-ride within 64MB - 128MB window.
	 */
	{
		int size = getbootint("KERNEL_MAP_SIZE", 0);

		if (size)
		{
			if ((64*1024*1024 <= size) && (size <= 128*1024*1024))
			{
				morevm = (vm_size_t) size;
			} else
			{
				printf("NOTICE: Ignored KERNEL_MAP_SIZE=%d\n",
					size);
			}
		}
	}

#if	i860

	/*
	 *	Kernel virtual address space comes from the top.
	 */
	virtual_end = trunc_page(VM_MAX_KERNEL_ADDRESS);
	virtual_avail = round_page(virtual_end - morevm + INTEL_PGBYTES);

	/*
	 *	Allocate a kernel page directory; put it's virtual
	 *	address in kpde.  Allocate enough kernel page tables
	 *	to span avail_start to avail_end.
	 *	Map all of physical ram.
	 */
	pmap_bootstrap_i860_physmem(&kpde);

	/*
	 *	Map in a page for use by the trap handler which
	 *	saves state at a negative offset from r0.
	 *
	 *	The i860 also starts executing code for all
	 *	exceptions at virtual address 0xffffff00;
	 *	plop some instructions at that address to
	 *	branch to alltraps().
	 */
	pmap_bootstrap_i860_trap_page(kpde);


#if	someday
	/*
	 *	Map in a page for use by the dcache flush routine.
	 */
	pmap_bootstrap_i860_flush_area(kpde);
#endif	someday

#if	iPSC860
	/*
	 *	Map in some essential device registers.
	 */
	pmap_bootstrap_i860_io(kpde,
		FIFO_ADDR_PH, FIFO_ADDR, FALSE);

	pmap_bootstrap_i860_io(kpde,
		FIFO_ADDR_PH + XEOD_OFF_PH, FIFO_ADDR + XEOD_OFF, FALSE);

	pmap_bootstrap_i860_io(kpde,
		CSR_ADDR_PH, CSR_ADDR, FALSE);

	pmap_bootstrap_i860_io(kpde,
		PERFCNT_ADDR_PH, PERFCNT_ADDR, TRUE);

	pmap_bootstrap_i860_io(kpde,
		UART_ADDR_PH, UART_ADDR, FALSE);
#endif	iPSC860

#if	PARAGON860

	/*
	 *	Map in all the crucial, fixed VM addressed
	 *	devices. (nic, dp, rpm, etc.)
	 */
	paragon_pmap_bootstrap_io(kpde);

#endif	PARAGON860


	/*
	 *	Allocate enough kernel page tables to
	 *	span from virtual_avail to virtual_end.
	 */
	pmap_bootstrap_i860_virtmem(kpde);

	virtual_end = trunc_page(virtual_end);

	kernel_pmap->dirbase = kpde;
	if (boot_mk_verbose) {
		printf("Kernel virtual space from 0x%x to 0x%x.\n",
				virtual_avail, virtual_end);
	}

#else	i860
	/*
	 *	The kernel page directory has been allocated;
	 *	its virtual address is in kpde.
	 *
	 *	Enough kernel page table pages have been allocated
	 *	to map low system memory, kernel text, kernel data/bss,
	 *	kdb's symbols, and the page directory and page tables.
	 *
	 *	No other physical memory has been allocated.
	 */

	/*
	 * Start mapping virtual memory to physical memory, 1-1,
	 * at end of mapped memory.
	 */
	virtual_avail = phystokv(avail_start);
	virtual_end = phystokv(avail_end);

	pde = kpde;
	pde += pdenum(virtual_avail);
	if (pte_to_pa(*pde) == 0) {
	    /* This pte has not been allocated */
	    pte = 0; ptend = 0;
	}
	else {
	    pte = (pt_entry_t *)ptetokv(*pde);
						/* first pte of page */
	    ptend = pte+NPTES;			/* last pte of page */
	    pte += ptenum(virtual_avail);	/* point to pte that
						   maps first avail VA */
	    pde++;	/* point pde to first empty slot */
	}

	template = pa_to_pte(avail_start)
		| INTEL_PTE_VALID
		| INTEL_PTE_WRITE;

	for (va = virtual_avail; va < virtual_end; va += INTEL_PGBYTES) {
	    if (pte >= ptend) {
		pte = (pt_entry_t *)virtual_avail;
		ptend = pte + NPTES;
		virtual_avail = (vm_offset_t)ptend;
		*pde = PA_TO_PTE((vm_offset_t) pte)
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
		pde++;
	    }
	    WRITE_PTE_FAST(pte, template)
	    pte++;
	    pte_increment_pa(template);
	}

	avail_start = virtual_avail - VM_MIN_KERNEL_ADDRESS;

/*
 *	startup requires additional virtual memory (for tables, buffers, 
 *	etc.).  The kd driver may also require some of that memory to
 *	access the graphics board.
 *
 */
	*(int *)&template = 0;
	virtual_end += morevm;
	for (tva = va; tva < virtual_end; tva += INTEL_PGBYTES) {
	    if (pte >= ptend) {
		pte = (pt_entry_t *)virtual_avail;
		ptend = pte + NPTES;
		virtual_avail = (vm_offset_t)ptend;
		avail_start += INTEL_PGBYTES;
		*pde = PA_TO_PTE((vm_offset_t) pte)
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
		pde++;
	    }
	    WRITE_PTE_FAST(pte, template)
	    pte++;
	}
	virtual_avail = va;
	/*
	 *	c.f. comment above
	 *
	 */
	virtual_end = va + morevm;
	while (pte < ptend)
	    *pte++ = 0;

	/*
	 * Round up avail_start to PAGE_SIZE boundary
	 */

	avail_start = (avail_start + PAGE_SIZE-1) & ~(PAGE_SIZE-1);

	/*
	 *	invalidate virtual addresses at 0
	 */
	kpde[0] = 0;
	kernel_pmap->dirbase = kpde;
	if (boot_mk_verbose) {
		printf("Kernel virtual space from 0x%x to 0x%x.\n",
				VM_MIN_KERNEL_ADDRESS, virtual_end);
	}
#endif	i860

	if (boot_mk_verbose) {
		printf("Available physical space from 0x%x to 0x%x\n",
				avail_start, avail_end);
	}

#if	i860
	/*
	 *	Ensure that all pages that are used for page
	 *	tables are marked non-cacheable.
	 *
	 *	XXX This post-pass will go away, eventually.
	 */
	pmap_bootstrap_i860_uncache_tables(kernel_pmap->dirbase);

	/*
	 *	Mark all the pages that have just been entered
	 *	as accessed and dirty.
	 *
	 *	XXX This post-pass will go away, eventually.
	 */
	pmap_bootstrap_i860_dirty(kernel_pmap->dirbase);

#if	i860XP
	/*
	 *	Force writeback of the page table data to
	 *	memory.
	 */
	flush();
#endif	i860XP

	/*
	 *	Throw the Big Switch.
	 */
	set_dirbase(kernel_pmap->dirbase);
	paging_enabled = 1;

	if (boot_mk_verbose) {
		printf("Address translation enabled.\n");
	}

#endif	i860

}

void pmap_virtual_space(startp, endp)
	vm_offset_t *startp;
	vm_offset_t *endp;
{
	*startp = virtual_avail;
	*endp = virtual_end;
}

/*
 *	Initialize the pmap module.
 *	Called by vm_init, to initialize any structures that the pmap
 *	system needs to map virtual memory.
 */
void pmap_init()
{
	register long		npages;
	vm_offset_t		addr;
	register vm_size_t	s;
	int			i;

	/*
	 *	Allocate memory for the pv_head_table and its lock bits,
	 *	the modify bit array, and the pte_page table.
	 */

#if	PARAGON860
	npages = paragon_total_vm_pages();
#else	PARAGON860
	npages = atop(avail_end - avail_start);
#endif	PARAGON860
	s = (vm_size_t) (sizeof(struct pv_entry) * npages
				+ pv_lock_table_size(npages)
#if	FROZEN_PAGES
				+ npages	/* frozen page vector */
#endif
				+ npages);	/* attributes vector */

	s = round_page(s);
	if (kmem_alloc_wired(kernel_map, &addr, s) != KERN_SUCCESS)
		panic("pmap_init");
	bzero((char *) addr, s);

	/*
	 *	Allocate the structures first to preserve word-alignment.
	 */
	pv_head_table = (pv_entry_t) addr;
	addr = (vm_offset_t) (pv_head_table + npages);

	pv_lock_table = (char *) addr;
	addr = (vm_offset_t) (pv_lock_table + pv_lock_table_size(npages));

	pmap_phys_attributes = (char *) addr;

#if	FROZEN_PAGES
	/*
	 * set the start of the frozen VM page byte vector
	 */
	addr = (vm_offset_t) (pmap_phys_attributes + npages);
        pmap_frozen_page_list = (unsigned char *) addr;
#endif
	/*
	 *	Create the zone of physical maps,
	 *	and of the physical-to-virtual entries.
	 */
	s = (vm_size_t) sizeof(struct pmap);
	pmap_zone = zinit(s, 400*s, 4096, FALSE, "pmap"); /* XXX */
	s = (vm_size_t) sizeof(struct pv_entry);
	pv_list_zone = zinit(s, 10000*s, 4096, FALSE, "pv_list"); /* XXX */

#if	NCPUS > 1
	/*
	 *	Set up the pmap request lists
	 */
	for (i = 0; i < NCPUS; i++) {
		pmap_update_list_t	up = &cpu_update_list[i];

		for (; up < (pmap_update_list_t)&cpu_update_list[NCPUS]; up++) {
			simple_lock_init(&up->lock);
			up->count = 0;
		}
	}
#endif	NCPUS > 1

	/*
	 *	Only now, when all of the data structures are allocated,
	 *	can we set vm_first_phys and vm_last_phys.  If we set them
	 *	too soon, the kmem_alloc_wired above will try to use these
	 *	data structures and blow up.
	 */

	vm_first_phys = avail_start;
	vm_last_phys = avail_end;
	pmap_initialized = TRUE;
}

#define valid_page(x) (pmap_initialized && pmap_valid_page(x))

boolean_t pmap_verify_free(phys)
	vm_offset_t	phys;
{
	pv_entry_t	pv_h;
	int		pai;
	int		spl;
	boolean_t	result;

	assert(phys != vm_page_fictitious_addr);
	if (!pmap_initialized)
		return(TRUE);

	if (!pmap_valid_page(phys))
		return(FALSE);

	PMAP_WRITE_LOCK(spl);

	pai = pa_index(phys);
	pv_h = pai_to_pvh(pai);

	result = (pv_h->pmap == PMAP_NULL);
	PMAP_WRITE_UNLOCK(spl);

	return(result);
}

/*
 *	Create and return a physical map.
 *
 *	If the size specified for the map
 *	is zero, the map is an actual physical
 *	map, and may be referenced by the
 *	hardware.
 *
 *	If the size specified is non-zero,
 *	the map will be used in software only, and
 *	is bounded by that size.
 */
pmap_t pmap_create(size)
	vm_size_t	size;
{
	register pmap_t			p;
	register pmap_statistics_t	stats;

	/*
	 *	A software use-only map doesn't even need a map.
	 */

	if (size != 0) {
		return(PMAP_NULL);
	}

/*
 *	Allocate a pmap struct from the pmap_zone.  Then allocate
 *	the page descriptor table from the pd_zone.
 */

	p = (pmap_t) zalloc(pmap_zone);
	if (p == PMAP_NULL)
		panic("pmap_create");

	if (kmem_alloc_wired(kernel_map,
			     (vm_offset_t *)&p->dirbase, INTEL_PGBYTES)
							!= KERN_SUCCESS)
		panic("pmap_create");

#if	i860

#if   CACHEDIRBASE
#else CACHEDIRBASE

	/*
	 *	the page holding the new dirbase needs to be uncached,
	 *	not just wired...
	 */
	{
		pt_entry_t	*ptep;

		flush();        /* force cache and memory to be coherent */
		ptep = pmap_pte(kernel_pmap, (vm_offset_t) p->dirbase);
		assert(ptep != PT_ENTRY_NULL);
		*ptep |= (INTEL_PTE_NCACHE | INTEL_PTE_WTHRU);
		flush_tlb();	/* force useage of new pte entry */
	}

#endif    CACHEDIRBASE

#endif	i860

	bcopy(kpde, p->dirbase, INTEL_PGBYTES);

#if   i860

#if   CACHEDIRBASE

	/*
	 *  the page holding the new dirbase needs to be flushed
	 */

	flush();        /* force cache and memory to be coherent */

#endif    CACHEDIRBASE

#endif    i860

	p->private_pmap = 0;
	p->ref_count = 1;

	simple_lock_init(&p->lock);
	p->cpus_using = 0;

	/*
	 *	Initialize statistics.
	 */

	stats = &p->stats;
	stats->resident_count = 0;
	stats->wired_count = 0;

	return(p);
}

/*
 *	Retire the given physical map from service.
 *	Should only be called if the map contains
 *	no valid mappings.
 */

void pmap_destroy(p)
	register pmap_t	p;
{
	register pt_entry_t	*pdep;
	register vm_offset_t	pa;
	register int		c, s;
	register vm_page_t	m;

	if (p == PMAP_NULL)
		return;

	SPLVM(s);
	SIMPLE_LOCK(&p->lock);
	c = --p->ref_count;
	SIMPLE_UNLOCK(&p->lock);
	SPLX(s);

	if (c != 0) {
	    return;	/* still in use */
	}

	/*
	 *	Free the memory maps, then the
	 *	pmap structure.
	 */
	for (pdep = p->dirbase;
	     pdep < &p->dirbase[pdenum(VM_MIN_KERNEL_ADDRESS)];
	     pdep += ptes_per_vm_page) {
	    if (*pdep & INTEL_PTE_VALID) {
		pa = pte_to_pa(*pdep);
		vm_object_lock(pmap_object);
		m = vm_page_lookup(pmap_object, pa);
		if (m == VM_PAGE_NULL)
		    panic("pmap_destroy: pte page not in object");
#if	i860
		{
			pt_entry_t	*ptep;

			/*
			 *	the physical mapping for free pages that
			 *	were used as page tables should revert
			 *	back to cacheability.
			 */
			ptep = pmap_pte(kernel_pmap, (vm_offset_t) pa);
			assert(ptep != PT_ENTRY_NULL);
			*ptep &= ~(INTEL_PTE_NCACHE|INTEL_PTE_WTHRU);
			*ptep |= i860_global_pmap_cache_policy;
			/*flush();	XXX flush below will suffice */ 
		}
#endif	i860
		vm_page_lock_queues();
		vm_page_free(m);
		inuse_ptepages_count--;
		vm_object_unlock(pmap_object);
		vm_page_unlock_queues();
	    }
	}

#if	i860
	/*
	 *	revert the physical mapping for the page used as a dirbase
	 *	back to cacheability.
	 */
	{
		pt_entry_t	*ptep;

		ptep = pmap_pte(kernel_pmap, (vm_offset_t) p->dirbase);
		assert(ptep != PT_ENTRY_NULL);
		*ptep &= ~(INTEL_PTE_NCACHE|INTEL_PTE_WTHRU);
		*ptep |= i860_global_pmap_cache_policy;
		flush();	/* force memory coherency */
	}
#endif	i860

	kmem_free(kernel_map, p->dirbase, INTEL_PGBYTES);
	zfree(pmap_zone, (vm_offset_t) p);
}

/*
 *	Add a reference to the specified pmap.
 */

void pmap_reference(p)
	register pmap_t	p;
{
	int	s;
	if (p != PMAP_NULL) {
		SPLVM(s);
		SIMPLE_LOCK(&p->lock);
		p->ref_count++;
		SIMPLE_UNLOCK(&p->lock);
		SPLX(s);
	}
}

/*
 *	Remove a range of hardware page-table entries.
 *	The entries given are the first (inclusive)
 *	and last (exclusive) entries for the VM pages.
 *	The virtual address is the va for the first pte.
 *
 *	The pmap must be locked.
 *	If the pmap is not the kernel pmap, the range must lie
 *	entirely within one pte-page.  This is NOT checked.
 *	Assumes that the pte-page exists.
 */

/* static */
void pmap_remove_range(pmap, va, spte, epte)
	pmap_t			pmap;
	vm_offset_t		va;
	pt_entry_t		*spte;
	pt_entry_t		*epte;
{
	register pt_entry_t	*cpte;
	int			num_removed, num_unwired;
	int			pai;
	vm_offset_t		pa;
	int			spl;

#if	DEBUG_PTE_PAGE
	if (pmap != kernel_pmap)
		ptep_check(get_pte_page(spte));
#endif	DEBUG_PTE_PAGE
	num_removed = 0;
	num_unwired = 0;

	for (cpte = spte; cpte < epte;
	     cpte += ptes_per_vm_page, va += PAGE_SIZE) {
pmap_remove_range_retry:
	    pa = pte_to_pa(*cpte);
	    if (pa == 0)
		continue;

	    num_removed++;
	    if (iswired(*cpte))
		num_unwired++;

	    if (!valid_page(pa)) {

		/*
		 *	Outside range of managed physical memory.
		 *	Just remove the mappings.
		 */
		register int	i = ptes_per_vm_page;
		register pt_entry_t	*lpte = cpte;
		do {
		    *lpte = 0;
		    lpte++;
		} while (--i > 0);
		continue;
	    }

#if	FROZEN_PAGES
#if 	MACH_ASSERT
/**	    CHECK_FROZEN(*cpte);
/**/
{				
	if (check_me && ((*cpte) & INTEL_PTE_VALID)) {			
		if ((pmap_is_vm_page_frozen(pte_to_pa((*cpte))))) {
			if (frozen_stop_here) {
				gimmeabreak();
			}
			++pmap_check_frozen_fail;	
			while ((pmap_is_vm_page_frozen(pte_to_pa((*cpte))))) {	
				++pmap_check_frozen_retry_total;	
        			printf(				
					"rm frozen pte 0x%x thread=0x%x\n",	
					(*cpte),
					current_thread()	
				);		
				/* sleep */
				thread_will_wait_with_timeout(		
					current_thread(),	
					10  /* msecs */	
				);		
				PMAP_READ_UNLOCK(pmap, 0xffffffff);
				thread_block(0);		
				PMAP_READ_LOCK(pmap, spl);
			}				
			goto pmap_remove_range_retry;
		}				
	} 				
}
#endif
#endif

	    pai = pa_index(pa);
	    LOCK_PVH(pai);

	    /*
	     *	Get the modify and reference bits.
	     */
	    {
		register int		i;
		register pt_entry_t	*lpte;

		i = ptes_per_vm_page;
		lpte = cpte;
		do {
		    pmap_phys_attributes[pai] |=
			*lpte & (PHYS_MODIFIED|PHYS_REFERENCED);
		    *lpte = 0;
		    lpte++;
		} while (--i > 0);
	    }

	    /*
	     *	Remove the mapping from the pvlist for
	     *	this physical page.
	     */
	    {
		register pv_entry_t	pv_h, prev, cur;

		pv_h = pai_to_pvh(pai);
		if (pv_h->pmap == PMAP_NULL) {
		    panic("pmap_remove: null pv_list!");
		}
		if (pv_h->va == va && pv_h->pmap == pmap) {
		    /*
		     * Header is the pv_entry.  Copy the next one
		     * to header and free the next one (we cannot
		     * free the header)
		     */
		    cur = pv_h->next;
		    if (cur != PV_ENTRY_NULL) {
			*pv_h = *cur;
			PV_FREE(cur);
		    }
		    else {
			pv_h->pmap = PMAP_NULL;
		    }
		}
		else {
		    cur = pv_h;
		    do {
			prev = cur;
			if ((cur = prev->next) == PV_ENTRY_NULL) {
			    panic("pmap-remove: mapping not in pv_list!");
			}
		    } while (cur->va != va || cur->pmap != pmap);
		    prev->next = cur->next;
		    PV_FREE(cur);
		}
		UNLOCK_PVH(pai);
	    }
	}

	/*
	 *	Update the counts
	 */
	pmap->stats.resident_count -= num_removed;
	pmap->stats.wired_count -= num_unwired;
}

/*
 *	Remove the given range of addresses
 *	from the specified map.
 *
 *	It is assumed that the start and end are properly
 *	rounded to the hardware page size.
 */


void pmap_remove(map, s, e)
	pmap_t		map;
	vm_offset_t	s, e;
{
	int			spl;
	register pt_entry_t	*pde;
	register pt_entry_t	*spte, *epte;
	vm_offset_t		l;

	if (map == PMAP_NULL)
		return;

	PMAP_READ_LOCK(map, spl);

	/*
	 *	Invalidate the translation buffer first
	 */
#if	i860 && !i860XP
	flush();	/* removing the mapping -- need to flush() */
#endif	i860 && !i860XP
	PMAP_UPDATE_TLBS(map, s, e);

	pde = pmap_pde(map, s);
	while (s < e) {
	    l = (s + PDE_MAPPED_SIZE) & ~(PDE_MAPPED_SIZE-1);
	    if (l > e || l < s)	/* don't wrap-around */
		l = e;
	    if (*pde & INTEL_PTE_VALID) {
		spte = (pt_entry_t *)ptetokv(*pde);
		spte = &spte[ptenum(s)];
		epte = &spte[intel_btop(l-s)];
		pmap_remove_range(map, s, spte, epte);
	    }
	    s = l;
	    pde++;
	}

	PMAP_READ_UNLOCK(map, spl);
}

/*
 *	Routine:	pmap_page_protect
 *
 *	Function:
 *		Lower the permission for all mappings to a given
 *		page.
 */
void pmap_page_protect(phys, prot)
	vm_offset_t	phys;
	vm_prot_t	prot;
{
	pv_entry_t		pv_h, prev;
	register pv_entry_t	pv_e;
	register pt_entry_t	*pte;
	int			pai;
	register pmap_t		pmap;
	int			spl;
	boolean_t		remove;

	assert(phys != vm_page_fictitious_addr);
	if (!valid_page(phys)) {
	    /*
	     *	Not a managed page.
	     */
	    return;
	}

	/*
	 * Determine the new protection.
	 */
	switch (prot) {
	    case VM_PROT_READ:
	    case VM_PROT_READ|VM_PROT_EXECUTE:
		remove = FALSE;
		break;
	    case VM_PROT_ALL:
		return;	/* nothing to do */
	    default:
		remove = TRUE;
		break;
	}

	/*
	 *	Lock the pmap system first, since we will be changing
	 *	several pmaps.
	 */

	PMAP_WRITE_LOCK(spl);

	pai = pa_index(phys);
	pv_h = pai_to_pvh(pai);

	/*
	 * Walk down PV list, changing or removing all mappings.
	 * We do not have to lock the pv_list because we have
	 * the entire pmap system locked.
	 */
	if (pv_h->pmap != PMAP_NULL) {

	    prev = pv_e = pv_h;
	    do {
		pmap = pv_e->pmap;
		/*
		 * Lock the pmap to block pmap_extract and similar routines.
		 */
		SIMPLE_LOCK(&pmap->lock);
		{
		    register vm_offset_t va;

		    va = pv_e->va;
		    pte = pmap_pte(pmap, va);

		    /*
		     * Consistency checks.
		     */
		    /* assert(*pte & INTEL_PTE_VALID); XXX */
		    /* assert(pte_to_phys(*pte) == phys); */

		    /*
		     * Invalidate TLBs for all CPUs using this mapping.
		     */
		    PMAP_UPDATE_TLBS(pmap, va, va + PAGE_SIZE);
		}

#if	!i860
		/*
		 * Remove the mapping if new protection is NONE
		 * or if write-protecting a kernel mapping.
		 */
		if (remove || pmap == kernel_pmap)
#else	/* !i860 */
		if ( remove )
#endif	/* !i860 */
		{
		    /*
		     * Remove the mapping, collecting any modify bits.
		     */
		    if (iswired(*pte))
			panic("pmap_remove_all removing a wired page");
#if	FROZEN_PAGES
		    if (isfrozen(*pte))
			panic("pmap_remove_all removing a frozen page");
#endif
		    {
			register int	i = ptes_per_vm_page;

			do {
			    pmap_phys_attributes[pai] |=
				*pte & (PHYS_MODIFIED|PHYS_REFERENCED);
			    *pte++ = 0;
			} while (--i > 0);
		    }

		    pmap->stats.resident_count--;

		    /*
		     * Remove the pv_entry.
		     */
		    if (pv_e == pv_h) {
			/*
			 * Fix up head later.
			 */
			pv_h->pmap = PMAP_NULL;
		    }
		    else {
			/*
			 * Delete this entry.
			 */
			prev->next = pv_e->next;
			PV_FREE(pv_e);
		    }
		}
		else {
		    /*
		     * Write-protect.
		     */
		    register int i = ptes_per_vm_page;

		    do {
			*pte &= ~INTEL_PTE_WRITE;
			pte++;
		    } while (--i > 0);

		    /*
		     * Advance prev.
		     */
		    prev = pv_e;
		}

		SIMPLE_UNLOCK(&pmap->lock);

	    } while ((pv_e = prev->next) != PV_ENTRY_NULL);

	    /*
	     * If pv_head mapping was removed, fix it up.
	     */
	    if (pv_h->pmap == PMAP_NULL) {
		pv_e = pv_h->next;
		if (pv_e != PV_ENTRY_NULL) {
		    *pv_h = *pv_e;
		    PV_FREE(pv_e);
		}
	    }
	}

	PMAP_WRITE_UNLOCK(spl);
}

/*
 *	Set the physical protection on the
 *	specified range of this map as requested.
 *	Will not increase permissions.
 */
void pmap_protect(map, s, e, prot)
	pmap_t		map;
	vm_offset_t	s, e;
	vm_prot_t	prot;
{
	register pt_entry_t	*pde;
	register pt_entry_t	*spte, *epte;
	vm_offset_t		l;
	int		spl;

	if (map == PMAP_NULL)
		return;

	/*
	 * Determine the new protection.
	 */
	switch (prot) {
	    case VM_PROT_READ:
	    case VM_PROT_READ|VM_PROT_EXECUTE:
		break;
	    case VM_PROT_READ|VM_PROT_WRITE:
	    case VM_PROT_ALL:
		return;	/* nothing to do */
	    default:
		pmap_remove(map, s, e);
		return;
	}

#if	!i860
	/*
	 * If write-protecting in the kernel pmap,
	 * remove the mappings; the i386 ignores
	 * the write-permission bit in kernel mode.
	 *
	 * XXX should be #if'd for i386
	 */
	if (map == kernel_pmap) {
	    pmap_remove(map, s, e);
	    return;
	}
#endif	/* !i860 */

	SPLVM(spl);
	SIMPLE_LOCK(&map->lock);

	/*
	 *	Invalidate the translation buffer first
	 */
	PMAP_UPDATE_TLBS(map, s, e);

	pde = pmap_pde(map, s);
	while (s < e) {
	    l = (s + PDE_MAPPED_SIZE) & ~(PDE_MAPPED_SIZE-1);
	    if (l > e || l < s)	/* don't wrap-around */
		l = e;
	    if (*pde & INTEL_PTE_VALID) {
		spte = (pt_entry_t *)ptetokv(*pde);
		spte = &spte[ptenum(s)];
		epte = &spte[intel_btop(l-s)];

		while (spte < epte) {
		    if (*spte & INTEL_PTE_VALID)
			*spte &= ~INTEL_PTE_WRITE;
		    spte++;
		}
	    }
	    s = l;
	    pde++;
	}

	SIMPLE_UNLOCK(&map->lock);
	SPLX(spl);
}

/*
 *	Insert the given physical page (p) at
 *	the specified virtual address (v) in the
 *	target physical map with the protection requested.
 *
 *	If specified, the page will be wired down, meaning
 *	that the related pte can not be reclaimed.
 *
 *	NB:  This is the only routine which MAY NOT lazy-evaluate
 *	or lose information.  That is, this routine must actually
 *	insert this page into the given map NOW.
 */
void pmap_enter(pmap, v, pa, prot, wired)
	register pmap_t		pmap;
	vm_offset_t		v;
	register vm_offset_t	pa;
	vm_prot_t		prot;
	boolean_t		wired;
{
	register pt_entry_t	*pte;
	register pv_entry_t	pv_h;
	register int		i, pai;
	pv_entry_t		pv_e;
	pt_entry_t		template;
	int			spl;
	vm_offset_t		old_pa;

	assert(pa != vm_page_fictitious_addr);
	if (pmap_debug)
		printf("pmap(%x, %x)\n", v, pa);
	if (pmap == PMAP_NULL)
		return;

#if	!i860
	if (pmap == kernel_pmap && (prot & VM_PROT_WRITE) == 0
	    && !wired /* hack for io_wire */ ) {
	    /*
	     *	Because the 386 ignores write protection in kernel mode,
	     *	we cannot enter a read-only kernel mapping, and must
	     *	remove an existing mapping if changing it.
	     *
	     *  XXX should be #if'd for i386
	     */
	    PMAP_READ_LOCK(pmap, spl);

	    pte = pmap_pte(pmap, v);
	    if (pte != PT_ENTRY_NULL && pte_to_pa(*pte) != 0) {
		/*
		 *	Invalidate the translation buffer,
		 *	then remove the mapping.
		 */
		PMAP_UPDATE_TLBS(pmap, v, v + PAGE_SIZE);
		pmap_remove_range(pmap, v, pte,
				  pte + ptes_per_vm_page);
	    }
	    PMAP_READ_UNLOCK(pmap, spl);
	    return;
	}
#endif	/* !i860 */

	/*
	 *	Must allocate a new pvlist entry while we're unlocked;
	 *	zalloc may cause pageout (which will lock the pmap system).
	 *	If we determine we need a pvlist entry, we will unlock
	 *	and allocate one.  Then we will retry, throughing away
	 *	the allocated entry later (if we no longer need it).
	 */
	pv_e = PV_ENTRY_NULL;
Retry:
	PMAP_READ_LOCK(pmap, spl);

	/*
	 *	Expand pmap to include this pte.  Assume that
	 *	pmap is always expanded to include enough hardware
	 *	pages to map one VM page.
	 */

	while ((pte = pmap_pte(pmap, v)) == PT_ENTRY_NULL) {
		/*
		 *	Must unlock to expand the pmap.
		 */
		PMAP_READ_UNLOCK(pmap, spl);

		pmap_expand(pmap, v);

		PMAP_READ_LOCK(pmap, spl);
	}
	/*
	 *	Special case if the physical page is already mapped
	 *	at this address.
	 */
	old_pa = pte_to_pa(*pte);
	if (old_pa == pa) {
	    /*
	     *	May be changing its wired attribute or protection
	     */
		
	    if (wired && !iswired(*pte))
		pmap->stats.wired_count++;
	    else if (!wired && iswired(*pte))
		pmap->stats.wired_count--;

	    template = pa_to_pte(pa)
#if	i860
		| i860_global_pmap_cache_policy
#endif	i860
		| INTEL_PTE_VALID;
	    if (pmap != kernel_pmap)
		template |= INTEL_PTE_USER;
	    if (prot & VM_PROT_WRITE)
		template |= INTEL_PTE_WRITE;
	    if (wired)
		template |= INTEL_PTE_WIRED;
#if	PARAGON860
	    if (paragon_is_dram_address(pa) == FALSE)
		template |= (INTEL_PTE_NCACHE | INTEL_PTE_WTHRU);
#endif	PARAGON860
	    PMAP_UPDATE_TLBS(pmap, v, v + PAGE_SIZE);
	    i = ptes_per_vm_page;
	    do {
		if (*pte & INTEL_PTE_MOD)
		    template |= INTEL_PTE_MOD;
		WRITE_PTE(pte, template)
		pte++;
		pte_increment_pa(template);
	    } while (--i > 0);
	}
	else {

	    /*
	     *	Remove old mapping from the PV list if necessary.
	     */
	    if (old_pa != (vm_offset_t) 0) {
		/*
		 *	Invalidate the translation buffer,
		 *	then remove the mapping.
		 */
#if	i860 && !i860XP
		flush();	/* removing the mapping -- need to flush() */
#endif	i860 && !i860XP
		PMAP_UPDATE_TLBS(pmap, v, v + PAGE_SIZE);

		/*
		 *	Don't free the pte page if removing last
		 *	mapping - we will immediately replace it.
		 */
		pmap_remove_range(pmap, v, pte,
				  pte + ptes_per_vm_page);
	    }

	    if (valid_page(pa)) {

		/*
		 *	Enter the mapping in the PV list for this
		 *	physical page.
		 */

		pai = pa_index(pa);
#if SHARING_FAULTS
RetryPvList:
		/*
		 * We can return here from the sharing fault code below
		 * in case we removed the only entry on the pv list and thus
		 * must enter the new one in the list header.
		 */
#endif
		LOCK_PVH(pai);
		pv_h = pai_to_pvh(pai);

		if (pv_h->pmap == PMAP_NULL) {
		    /*
		     *	No mappings yet
		     */
		    pv_h->va = v;
		    pv_h->pmap = pmap;
		    pv_h->next = PV_ENTRY_NULL;
		}
		else {
#if	DEBUG
		    {
			/* check that this mapping is not already there
                         * or there is no alias for this mapping in the same map
                         */
			pv_entry_t	e = pv_h;
			while (e != PV_ENTRY_NULL) {
			    if (e->pmap == pmap && e->va == v)
                              panic("pmap_enter: already in pv_list");
			    e = e->next;
			}
		    }
#endif	DEBUG
#if SHARING_FAULTS
                    {
                        /*
                         * do sharing faults.
                         * if we find an entry on this pv list in the same address space
                         * remove it.  we know there will not be more than one.
                         */
			pv_entry_t	e = pv_h;
                        pt_entry_t      *opte;

			while (e != PV_ENTRY_NULL) {
			    if (e->pmap == pmap) {
                                    /*
                                     * remove it.
                                     */
                                    UNLOCK_PVH(pai);         /* give up lock on pv list */
                                    opte = pmap_pte(pmap, e->va);

                                    /* should be there... */
                                    assert(opte != PT_ENTRY_NULL);
                                    /*
                                     *	Invalidate the translation buffer,
                                     *	then remove the mapping.
                                     */
#if	i860 && !i860XP
				    flush();	/* removing; need to flush() */
#endif	i860 && !i860XP
                                    PMAP_UPDATE_TLBS(pmap, e->va, e->va + PAGE_SIZE);
                                    pmap_remove_range(pmap, e->va, opte,
                                                      opte + ptes_per_vm_page);
				    /*
				     * We could have remove the head entry on the
				     * list so there could be no more entries
				     * and so we have to use the pv head entry.
				     * so, go back to the top and try the entry
				     * again.
				     */
                                    goto RetryPvList;
                            }
                            e = e->next;
                        }

			/*
                         * check that this mapping is not already there
                         */
			e = pv_h;
			while (e != PV_ENTRY_NULL) {
			    if (e->pmap == pmap)
                              panic("pmap_enter: alias in pv_list");
			    e = e->next;
			}
                    }
#endif SHARING_FAULTS
#if DEBUG_ALIAS
                    {
                        /*
                         * check for aliases within the same address space.
                         */
			pv_entry_t	e = pv_h;
                        vm_offset_t     rpc = get_rpc();

			while (e != PV_ENTRY_NULL) {
			    if (e->pmap == pmap) {
                                    /*
                                     * log this entry in the alias ring buffer if its not there already.
                                     */
                                    struct pmap_alias *pma;
                                    int ii, logit;

                                    logit = TRUE;
                                    for (ii = 0; ii < pmap_alias_index; ii++) {
                                            if (pmap_aliasbuf[ii].rpc == rpc) {
                                                    /* found it in the log already */
                                                    logit = FALSE;
                                                    break;
                                            }
                                    }
                                    if (logit) {
                                            pma = &pmap_aliasbuf[pmap_alias_index];
                                            pma->pmap = pmap;
                                            pma->va = v;
                                            pma->rpc = rpc;
                                            pma->cookie = PMAP_ALIAS_COOKIE;
                                            if (++pmap_alias_index >= PMAP_ALIAS_MAX)
                                              panic("pmap_enter: exhausted alias log");
                                    }
                            }
                            e = e->next;
                        }
                    }
#endif DEBUG_ALIAS
		    /*
		     *	Add new pv_entry after header.
		     */
		    if (pv_e == PV_ENTRY_NULL) {
			PV_ALLOC(pv_e);
			if (pv_e == PV_ENTRY_NULL) {
			    UNLOCK_PVH(pai);
			    PMAP_READ_UNLOCK(pmap, spl);

			    /*
			     * Refill from zone.
			     */
			    pv_e = (pv_entry_t) zalloc(pv_list_zone);
			    goto Retry;
			}
		    }
		    pv_e->va = v;
		    pv_e->pmap = pmap;
		    pv_e->next = pv_h->next;
		    pv_h->next = pv_e;
		    /*
		     *	Remember that we used the pvlist entry.
		     */
		    pv_e = PV_ENTRY_NULL;
		}
		UNLOCK_PVH(pai);
	    }

	    /*
	     *	And count the mapping.
	     */

	    pmap->stats.resident_count++;
	    if (wired)
		pmap->stats.wired_count++;

	    /*
	     *	Build a template to speed up entering -
	     *	only the pfn changes.
	     */
	    template = pa_to_pte(pa)
#if	i860
		| i860_global_pmap_cache_policy
#endif	i860
		| INTEL_PTE_VALID;
	    if (pmap != kernel_pmap)
		template |= INTEL_PTE_USER;
	    if (prot & VM_PROT_WRITE)
		template |= INTEL_PTE_WRITE;
	    if (wired)
		template |= INTEL_PTE_WIRED;
#if	PARAGON860
	    /*
	     *	don't enable cacheing on non-dram physical addresses.
	     */
	    if (paragon_is_dram_address(pa) == FALSE)
		template |= (INTEL_PTE_NCACHE | INTEL_PTE_WTHRU);
#endif	PARAGON860
	    i = ptes_per_vm_page;
	    do {
		WRITE_PTE(pte, template)
		pte++;
		pte_increment_pa(template);
	    } while (--i > 0);
	}

	if (pv_e != PV_ENTRY_NULL) {
	    PV_FREE(pv_e);
	}

	PMAP_READ_UNLOCK(pmap, spl);
}


#if	FROZEN_PAGES

/*
 *	Enter a page frozen for receives from the network.
 *	This is a simplified version of pmap_enter().
 *
 *	Much of the special case processing that pmap_enter()
 *	is required to perform is uneeded by pmap_enter_frozen().
 */
void pmap_enter_frozen(pmap, va, pa)
	register pmap_t		pmap;
	vm_offset_t		va;
	register vm_offset_t	pa;
{
	register pt_entry_t	*pte;
	register pv_entry_t	pv_h;
	register int		pai, i;
	register pt_entry_t	template;
	int			spl;
	static vm_offset_t	cached_pa;
	static pv_entry_t	cached_pvh;
	static int		cached_pai;


	assert(pa != vm_page_fictitious_addr);
	assert(pmap != PMAP_NULL);

	PMAP_READ_LOCK(pmap, spl);

	/*
	 *	Expand pmap to include this pte.  Assume that
	 *	pmap is always expanded to include enough hardware
	 *	pages to map one VM page.
	 */
	while ((pte = pmap_pte(pmap, va)) == PT_ENTRY_NULL) {
		/*
		 *	Must unlock to expand the pmap.
		 */
		PMAP_READ_UNLOCK(pmap, spl);
		pmap_expand(pmap, va);
		PMAP_READ_LOCK(pmap, spl);
	}

	/*
	 *	Count the mapping.
	 */
	pmap->stats.resident_count++;


	/*
	 *	Some sanity checks...
	 *
	 *	1. This VA must not already have a mapping.
	 *	2. The page must be a page managed by pmap.
	 *	3. The page must be main memory (vs. mapped ram from a device).
	 */
	assert(pte_to_pa(*pte) == (vm_offset_t) 0);
	assert(valid_page(pa));
#if	PARAGON860
	assert(paragon_is_dram_address(pa) == TRUE);
#endif	PARAGON860

	/*
	 *	Enter the mapping in the PV list for this
	 *	physical page.
	 *
	 *	A cache of depth 1 is maintained to avoid the
	 *	call/return overhead of pa_index() (Paragon nodes
	 *	can have two chunks of widely separated physical memory
	 *	and pa_index() performs a mapping function), and
	 *	the pai_to_pvh() conversion which requires a multiply by
	 *	12 (the size of a pv_entry_t is not a power of 2).
	 *
	 *	The cache is only useful for the case of receiving
	 *	1 page of ool data repeatedly into the same physical
	 *	page (receive 1 ool page, free it, repeat).
	 */
	if (cached_pa == pa) {
		pai = cached_pai;
		LOCK_PVH(pai);
		pv_h = cached_pvh;
	} else {
		pai = pa_index(pa);
		LOCK_PVH(pai);
		pv_h = pai_to_pvh(pai);
		cached_pa  = pa;
		cached_pai = pai;
		cached_pvh = pv_h;
	}
	assert(pv_h->pmap == PMAP_NULL);	/* must have no mappings */
	assert(pv_h->next == PV_ENTRY_NULL);	/* list must be empty */
	pv_h->va = va;
	pv_h->pmap = pmap;
	UNLOCK_PVH(pai);

#if	MACH_ASSERT
	pmap_frozen_page_list[ pai ]++;
#else
	pmap_frozen_page_list[ pai ] = TRUE;
#endif

	/*
	 *	Build a template to speed up entering -
	 *	only the pfn changes.
	 */
	template = pa_to_pte(pa)
#if	i860
			| i860_global_pmap_cache_policy
#endif	i860
			| INTEL_PTE_WRITE	/* receiving into it */
			| INTEL_PTE_REF		/* it will be touched */
			| INTEL_PTE_MOD		/* it will be modified */
			| INTEL_PTE_VALID;	/* mapping is present */
	if (pmap != kernel_pmap)
		template |= INTEL_PTE_USER;


	/*
	 *	Write entries for all physical pages
	 *	within the logical page size.
	 */
	i = ptes_per_vm_page;
	do {
		WRITE_PTE(pte, template)
		pte++;
		pte_increment_pa(template);
	} while (--i > 0);

	pmap_phys_attributes[pai] |= (PHYS_MODIFIED|PHYS_REFERENCED);

	PMAP_READ_UNLOCK(pmap, spl);
}
#endif	/* FROZEN_PAGES */


/*
 *	Routine:	pmap_change_wiring
 *	Function:	Change the wiring attribute for a map/virtual-address
 *			pair.
 *	In/out conditions:
 *			The mapping must already exist in the pmap.
 */
void pmap_change_wiring(map, v, wired)
	register pmap_t	map;
	vm_offset_t	v;
	boolean_t	wired;
{
	register pt_entry_t	*pte;
	register int		i;
	int			spl;

	/*
	 *	We must grab the pmap system lock because we may
	 *	change a pte_page queue.
	 */
	PMAP_READ_LOCK(map, spl);

	if ((pte = pmap_pte(map, v)) == PT_ENTRY_NULL)
		panic("pmap_change_wiring: pte missing");

	if (wired && !iswired(*pte)) {
	    /*
	     *	wiring down mapping
	     */
	    map->stats.wired_count++;
	    i = ptes_per_vm_page;
	    do {
		*pte++ |= INTEL_PTE_WIRED;
	    } while (--i > 0);
	}
	else if (!wired && iswired(*pte)) {
	    /*
	     *	unwiring mapping
	     */
	    map->stats.wired_count--;
	    i = ptes_per_vm_page;
	    do {
		*pte &= ~INTEL_PTE_WIRED;
		pte++;
	    } while (--i > 0);
	}

	PMAP_READ_UNLOCK(map, spl);
}

/*
 *	Routine:	pmap_extract
 *	Function:
 *		Extract the physical page address associated
 *		with the given map/virtual_address pair.
 */

vm_offset_t pmap_extract(pmap, va)
	register pmap_t	pmap;
	vm_offset_t	va;
{
	register pt_entry_t	*pte;
	register vm_offset_t	pa;
	int			spl;

	SPLVM(spl);
	SIMPLE_LOCK(&pmap->lock);
	if ((pte = pmap_pte(pmap, va)) == PT_ENTRY_NULL)
	    pa = (vm_offset_t) 0;
	else if (!(*pte & INTEL_PTE_VALID))
	    pa = (vm_offset_t) 0;
	else
	    pa = pte_to_pa(*pte) + (va & INTEL_OFFMASK);
	SIMPLE_UNLOCK(&pmap->lock);
	SPLX(spl);
	return(pa);
}

/*
 *	Routine:	pmap_expand
 *
 *	Expands a pmap to be able to map the specified virtual address.
 *
 *	Allocates new virtual memory for the P0 or P1 portion of the
 *	pmap, then re-maps the physical pages that were in the old
 *	pmap to be in the new pmap.
 *
 *	Must be called with the pmap system and the pmap unlocked,
 *	since these must be unlocked to use vm_allocate or vm_deallocate.
 *	Thus it must be called in a loop that checks whether the map
 *	has been expanded enough.
 *	(We won't loop forever, since page tables aren't shrunk.)
 */
pmap_expand(map, v)
	register pmap_t		map;
	register vm_offset_t	v;
{
	pt_entry_t		*pdp;
	register vm_page_t	m;
	register vm_offset_t	pa;
	register int		i;
	int			spl;

	check_simple_locks();

	if (map == kernel_pmap)
	    panic("pmap_expand");

	/*
	 *	We cannot allocate the pmap_object in pmap_init,
	 *	because it is called before the zone package is up.
	 *	Allocate it now if it is missing.
	 */
	if (pmap_object == VM_OBJECT_NULL)
	    pmap_object = vm_object_allocate(mem_size);

	/*
	 *	Allocate a VM page for the level 2 page table entries.
	 */
	while ((m = vm_page_grab()) == VM_PAGE_NULL)
		VM_PAGE_WAIT((void (*)()) 0);

	/*
	 *	Map the page to its physical address so that it
	 *	can be found later.
	 */
	pa = m->phys_addr;
	vm_object_lock(pmap_object);
	vm_page_insert(m, pmap_object, pa);
	vm_page_lock_queues();
	vm_page_wire(m);
	inuse_ptepages_count++;
	vm_object_unlock(pmap_object);
	vm_page_unlock_queues();

	/*
	 *	Zero the page.
	 */
	bzero(phystokv(pa), PAGE_SIZE);

	PMAP_READ_LOCK(map, spl);
	/*
	 *	See if someone else expanded us first
	 */
	if (pmap_pte(map, v) != PT_ENTRY_NULL) {
		PMAP_READ_UNLOCK(map, spl);
		vm_object_lock(pmap_object);
		vm_page_lock_queues();
		vm_page_free(m);
		inuse_ptepages_count--;
		vm_page_unlock_queues();
		vm_object_unlock(pmap_object);
		return;
	}

	/*
	 *	Set the page directory entry for this page table.
	 *	If we have allocated more than one hardware page,
	 *	set several page directory entries.
	 */

#if	i860
	/*
	 * Flush the data cache.
	 */
	flush();	/* force memory to be coherent */

        /*
         * First mark the page table page(s) non-cacheable.
         */
	i = ptes_per_vm_page;
	pdp = pmap_pte(kernel_map->pmap, pa);
	do {
	    *pdp |= (INTEL_PTE_NCACHE | INTEL_PTE_WTHRU);
	    pdp++;
	} while (--i > 0);

	flush_tlb();	/* invalidate tbl to use NOCACHE and WRITETHRU */

#endif	i860
	i = ptes_per_vm_page;
	pdp = &map->dirbase[pdenum(v) & ~(i-1)];
	do {
	    *pdp = pa_to_pte(pa)
#if	i860
		| i860_global_pmap_cache_policy
#endif	i860
		| INTEL_PTE_VALID
		| INTEL_PTE_REF
		| INTEL_PTE_USER
		| INTEL_PTE_WRITE;
	    pdp++;
	    pa += INTEL_PGBYTES;
	} while (--i > 0);

#if   i860
#if   CACHEDIRBASE

	/*
	 * Flush the data cache.
	 */
	flush();    /* force memory to be coherent */

#endif    CACHEDIRBASE
#endif    i860

	PMAP_READ_UNLOCK(map, spl);

	return;
}

/*
 *	Copy the range specified by src_addr/len
 *	from the source map to the range dst_addr/len
 *	in the destination map.
 *
 *	This routine is only advisory and need not do anything.
 */
#if	0
void pmap_copy(dst_pmap, src_pmap, dst_addr, len, src_addr)
	pmap_t		dst_pmap;
	pmap_t		src_pmap;
	vm_offset_t	dst_addr;
	vm_size_t	len;
	vm_offset_t	src_addr;
{
#ifdef	lint
	dst_pmap++; src_pmap++; dst_addr++; len++; src_addr++;
#endif	lint
}
#endif	0

int	pmap_collect_ref;
int	pmap_collect_unref;
int	pmap_collect_invoked;

/*
 *	Routine:	pmap_collect
 *	Function:
 *		Garbage collects the physical map system for
 *		pages which are no longer used.
 *		Success need not be guaranteed -- that is, there
 *		may well be pages which are not referenced, but
 *		others may be collected.
 *	Usage:
 *		Called by the pageout daemon when pages are scarce.
 */
void pmap_collect(p)
	pmap_t 		p;
{
	register pt_entry_t	*pdp, *ptp;
	pt_entry_t		*eptp;
	vm_offset_t		pa;
	int			spl, wired;

	if (p == PMAP_NULL)
		return;

	if (p == kernel_pmap)
		return;

	/*
	 *	Garbage collect map.
	 */
	pmap_collect_invoked++;
	PMAP_READ_LOCK(p, spl);
	PMAP_UPDATE_TLBS(p, VM_MIN_ADDRESS, VM_MAX_ADDRESS);

	for (pdp = p->dirbase;
	     pdp < &p->dirbase[pdenum(VM_MIN_KERNEL_ADDRESS)];
	     pdp += ptes_per_vm_page)
	{
	    if (*pdp & INTEL_PTE_VALID) 
	      if(*pdp & INTEL_PTE_REF) {
		*pdp &= ~INTEL_PTE_REF;
		pmap_collect_ref++;
	      } else {
		pmap_collect_unref++;
		pa = pte_to_pa(*pdp);
		ptp = (pt_entry_t *)phystokv(pa);
		eptp = ptp + NPTES*ptes_per_vm_page;

		/*
		 * If the pte page has any wired or frozen mappings, we cannot
		 * free it.
		 */
		wired = 0;
		{
		    register pt_entry_t *ptep;
		    for (ptep = ptp; ptep < eptp; ptep++) {
#if	FROZEN_PAGES
			if ( iswired(*ptep) || isfrozen(*ptep) )
#else	/* FROZEN_PAGES */
			if ( iswired(*ptep) )
#endif	/* FROZEN_PAGES */
			{
			    wired = 1;
			    break;
			}
		    }
		}
		if (!wired) {
		    /*
		     * Remove the virtual addresses mapped by this pte page.
		     */
		    pmap_remove_range(p,
				pdetova(pdp - p->dirbase),
				ptp,
				eptp);

		    /*
		     * Invalidate the page directory pointer.
		     */
		    {
			register int i = ptes_per_vm_page;
			register pt_entry_t *pdep = pdp;
			do {
			    *pdep++ = 0;
			} while (--i > 0);
		    }

		    PMAP_READ_UNLOCK(p, spl);

		    /*
		     * And free the pte page itself.
		     */
		    {
			register vm_page_t m;

			vm_object_lock(pmap_object);
			m = vm_page_lookup(pmap_object, pa);
			if (m == VM_PAGE_NULL)
			    panic("pmap_collect: pte page not in object");
			vm_page_lock_queues();
			vm_page_free(m);
			inuse_ptepages_count--;
			vm_page_unlock_queues();
			vm_object_unlock(pmap_object);
		    }

		    PMAP_READ_LOCK(p, spl);
		}
	    }
	}
	PMAP_READ_UNLOCK(p, spl);
	return;

}


/*
 *	Routine:	pmap_activate
 *	Function:
 *		Binds the given physical map to the given
 *		processor, and returns a hardware map description.
 */
#if	0
void pmap_activate(my_pmap, th, my_cpu)
	register pmap_t	my_pmap;
	thread_t	th;
	int		my_cpu;
{
	PMAP_ACTIVATE(my_pmap, th, my_cpu);
}
#endif	0

/*
 *	Routine:	pmap_deactivate
 *	Function:
 *		Indicates that the given physical map is no longer
 *		in use on the specified processor.  (This is a macro
 *		in pmap.h)
 */
#if	0
void pmap_deactivate(pmap, th, which_cpu)
	pmap_t		pmap;
	thread_t	th;
	int		which_cpu;
{
#ifdef	lint
	pmap++; th++; which_cpu++;
#endif	lint
	PMAP_DEACTIVATE(pmap, th, which_cpu);
}
#endif	0

/*
 *	Routine:	pmap_kernel
 *	Function:
 *		Returns the physical map handle for the kernel.
 */
#if	0
pmap_t pmap_kernel()
{
    	return (kernel_pmap);
}
#endif	0

/*
 *	pmap_zero_page zeros the specified (machine independent) page.
 *	See machine/phys.c or machine/phys.s for implementation.
 */
#if	0
pmap_zero_page(phys)
	register vm_offset_t	phys;
{
	register int	i;

	assert(phys != vm_page_fictitious_addr);
	i = PAGE_SIZE / INTEL_PGBYTES;
	phys = intel_pfn(phys);

	while (i--)
		zero_phys(phys++);
}
#endif	0

/*
 *	pmap_copy_page copies the specified (machine independent) page.
 *	See machine/phys.c or machine/phys.s for implementation.
 */
#if	0
pmap_copy_page(src, dst)
	vm_offset_t	src, dst;
{
	int	i;

	assert(src != vm_page_fictitious_addr);
	assert(dst != vm_page_fictitious_addr);
	i = PAGE_SIZE / INTEL_PGBYTES;

	while (i--) {
		copy_phys(intel_pfn(src), intel_pfn(dst));
		src += INTEL_PGBYTES;
		dst += INTEL_PGBYTES;
	}
}
#endif	0

/*
 *	Routine:	pmap_pageable
 *	Function:
 *		Make the specified pages (by pmap, offset)
 *		pageable (or not) as requested.
 *
 *		A page which is not pageable may not take
 *		a fault; therefore, its page table entry
 *		must remain valid for the duration.
 *
 *		This routine is merely advisory; pmap_enter
 *		will specify that these pages are to be wired
 *		down (or not) as appropriate.
 */
pmap_pageable(pmap, start, end, pageable)
	pmap_t		pmap;
	vm_offset_t	start;
	vm_offset_t	end;
	boolean_t	pageable;
{
#ifdef	lint
	pmap++; start++; end++; pageable++;
#endif	lint
}

/*
 *	Clear specified attribute bits.
 */
void
phys_attribute_clear(phys, bits)
	vm_offset_t	phys;
	int		bits;
{
	pv_entry_t		pv_h;
	register pv_entry_t	pv_e;
	register pt_entry_t	*pte;
	int			pai;
	register vm_offset_t	va;
	register pmap_t		pmap;
	int			spl;

	assert(phys != vm_page_fictitious_addr);
	if (!valid_page(phys)) {
		/*
		 *	Not a managed page.
		 */
		return;
	}

	/*
	 *	Lock the pmap system first, since we will be changing
	 *	several pmaps.
	 */

	PMAP_WRITE_LOCK(spl);

	pai = pa_index(phys);
	pv_h = pai_to_pvh(pai);

	/*
	 *	Walk down PV list, clearing all modify or reference bits.
	 *	We do not have to lock the pv_list because we have
	 *	the entire pmap system locked.
	 */
	if (pv_h->pmap != PMAP_NULL) {
		for (pv_e = pv_h; pv_e != PV_ENTRY_NULL; pv_e = pv_e->next) {

			pmap = pv_e->pmap;

			SIMPLE_LOCK(&pmap->lock);
			va = pv_e->va;
			pte = pmap_pte(pmap, va);

			PMAP_UPDATE_TLBS(pmap, va, va + PAGE_SIZE);
			{
				register int	i = ptes_per_vm_page;

				do {
					*pte &= ~bits;
					pte++;
				} while (--i > 0);
			}

			SIMPLE_UNLOCK(&pmap->lock);
		}
	}

	pmap_phys_attributes[pai] &= ~bits;

	PMAP_WRITE_UNLOCK(spl);
}

/*
 *	Check specified attribute bits.
 */
boolean_t
phys_attribute_test(phys, bits)
	vm_offset_t	phys;
	int		bits;
{
	pv_entry_t		pv_h;
	register pv_entry_t	pv_e;
	register pt_entry_t	*pte;
	int			pai;
	register pmap_t		pmap;
	int			spl;
	int			r;

	assert(phys != vm_page_fictitious_addr);
	if (!valid_page(phys)) {
		/*
		 *	Not a managed page.
		 */
		return (FALSE);
	}

	/*
	 *	Lock the pmap system first, since we will be checking
	 *	several pmaps.
	 */

	PMAP_WRITE_LOCK(spl);

	pai = pa_index(phys);
	pv_h = pai_to_pvh(pai);

	if (pmap_phys_attributes[pai] & bits) {
		PMAP_WRITE_UNLOCK(spl);
		return (TRUE);
	}

	/*
	 *	Walk down PV list, checking any mapping of "phys".
	 *	We do not have to lock the pv_list because we have
	 *	the entire pmap system locked.
	 */
	if (pv_h->pmap != PMAP_NULL) {
		for (pv_e = pv_h; pv_e != PV_ENTRY_NULL; pv_e = pv_e->next) {

			pmap = pv_e->pmap;

			SIMPLE_LOCK(&pmap->lock);

			pte = pmap_pte(pmap, pv_e->va);
			{
				register int	i = ptes_per_vm_page;

				do {
					if ((r = *pte++ & bits) != 0) {
						pmap_phys_attributes[pai] |= r;
						SIMPLE_UNLOCK(&pmap->lock);
						PMAP_WRITE_UNLOCK(spl);
						return (TRUE);
					}
				} while (--i > 0);
			}
			SIMPLE_UNLOCK(&pmap->lock);
		}
	}

	PMAP_WRITE_UNLOCK(spl);
	return (FALSE);
}

/*
 *	Clear the modify bits on the specified physical page.
 */

void pmap_clear_modify(phys)
	register vm_offset_t	phys;
{
	phys_attribute_clear(phys, PHYS_MODIFIED);
}

/*
 *	pmap_is_modified:
 *
 *	Return whether or not the specified physical page is modified
 *	by any physical maps.
 */

boolean_t pmap_is_modified(phys)
	register vm_offset_t	phys;
{
	return (phys_attribute_test(phys, PHYS_MODIFIED));
}

/*
 *	pmap_clear_reference:
 *
 *	Clear the reference bit on the specified physical page.
 */

void pmap_clear_reference(phys)
	vm_offset_t	phys;
{
	phys_attribute_clear(phys, PHYS_REFERENCED);
}

/*
 *	pmap_is_referenced:
 *
 *	Return whether or not the specified physical page is referenced
 *	by any physical maps.
 */

boolean_t pmap_is_referenced(phys)
	vm_offset_t	phys;
{
	return (phys_attribute_test(phys, PHYS_REFERENCED));
}

#if	NCPUS > 1
/*
*	    TLB Coherence Code (TLB "shootdown" code)
* 
* Threads that belong to the same task share the same address space and
* hence share a pmap.  However, they  may run on distinct cpus and thus
* have distinct TLBs that cache page table entries. In order to guarantee
* the TLBs are consistent, whenever a pmap is changed, all threads that
* are active in that pmap must have their TLB updated. To keep track of
* this information, the set of cpus that are currently using a pmap is
* maintained within each pmap structure (cpus_using). PMAP_ACTIVATE_KERNEL()
* and PMAP_ACTIVATE_USER() add a cpu from this set.  PMAP_DEACTIVATE_KERNEL()
* and PMAP_DEACTIVATE_USER() remove a cpu from this set.
* Since the TLBs are not addressable over the bus, each processor must
* flush its own TLB; a processor that needs to invalidate another TLB
* needs to interrupt the processor that owns that TLB to signal the
* update.
* 
* Whenever a pmap is updated, the lock on that pmap is locked, and all
* cpus using the pmap are signaled to invalidate. All threads that need
* to activate a pmap must wait for the lock to clear to await any updates
* in progress before using the pmap. They must ACQUIRE the lock to add
* their cpu to the cpus_using set. An implicit assumption made
* throughout the TLB code is that all kernel code that runs at or higher
* than splvm blocks out update interrupts, and that such code does not
* touch pageable pages.
* 
* A shootdown interrupt serves another function besides signaling a
* processor to invalidate. The interrupt routine (pmap_update_interrupt)
* waits for the both the pmap lock (and the kernel pmap lock) to clear,
* preventing user code from making implicit pmap updates while the
* sending processor is performing its update. (This could happen via a
* user data write reference that turns on the modify bit in the page
* table). It must wait for any kernel updates that may have started
* concurrently with a user pmap update because the IPC code
* changes mappings.
* Spinning on the VALUES of the locks is sufficient (rather than
* having to acquire the locks) because any updates that occur subsequent
* to finding the lock unlocked will be signaled via another interrupt.
* (This assumes the interrupt is cleared before the low level interrupt code 
* calls pmap_update_interrupt()). 
* 
* The signaling processor must wait for any implicit updates in progress
* to terminate before continuing with its update. Thus it must wait for an
* acknowledgement of the interrupt from each processor for which such
* references could be made. For maintaining this information, a set
* cpus_active is used. A cpu is in this set if and only if it can 
* use a pmap. When pmap_update_interrupt() is entered, a cpu is removed from
* this set; when all such cpus are removed, it is safe to update.
* 
* Before attempting to acquire the update lock on a pmap, a cpu (A) must
* be at least at the priority of the interprocessor interrupt
* (splip<=splvm). Otherwise, A could grab a lock and be interrupted by a
* kernel update; it would spin forever in pmap_update_interrupt() trying
* to acquire the user pmap lock it had already acquired. Furthermore A
* must remove itself from cpus_active.  Otherwise, another cpu holding
* the lock (B) could be in the process of sending an update signal to A,
* and thus be waiting for A to remove itself from cpus_active. If A is
* spinning on the lock at priority this will never happen and a deadlock
* will result.
*/

/*
 *	Signal another CPU that it must flush its TLB
 */
void    signal_cpus(use_list, pmap, start, end)
	cpu_set		use_list;
	pmap_t		pmap;
	vm_offset_t	start, end;
{
	register int		which_cpu, j;
	register pmap_update_list_t	update_list_p;

	while ((which_cpu = ffs(use_list)) != 0) {
	    which_cpu -= 1;	/* convert to 0 origin */

	    update_list_p = &cpu_update_list[which_cpu];
	    SIMPLE_LOCK(&update_list_p->lock);

	    j = update_list_p->count;
	    if (j >= UPDATE_LIST_SIZE) {
		/*
		 *	list overflowed.  Change last item to
		 *	indicate overflow.
		 */
		update_list_p->item[UPDATE_LIST_SIZE-1].pmap  = kernel_pmap;
		update_list_p->item[UPDATE_LIST_SIZE-1].start = VM_MIN_ADDRESS;
		update_list_p->item[UPDATE_LIST_SIZE-1].end   = VM_MAX_KERNEL_ADDRESS;
	    }
	    else {
#ifdef	i860
		/*
		 * Only need one entry as the i860 cache mgmt. pulls the
		 * BIG chain and dumps the entire cache on the floor.
		 */
		if ( j == 0 ) {
			update_list_p->item[j].pmap  = pmap;
			update_list_p->item[j].start = start;
			update_list_p->item[j].end   = end;
			update_list_p->count = j+1;
		}
#else	/* i860 */
		update_list_p->item[j].pmap  = pmap;
		update_list_p->item[j].start = start;
		update_list_p->item[j].end   = end;
		update_list_p->count = j+1;
#endif	/* i860 */
	    }
	    cpu_update_needed[which_cpu] = TRUE;
	    SIMPLE_UNLOCK(&update_list_p->lock);

	    /* if its the kernel pmap, ignore cpus_idle */
	    if (((cpus_idle & (1 << which_cpu)) == 0) ||
		(pmap == kernel_pmap))
	      {
		interrupt_processor(which_cpu);
	      }
	    use_list &= ~(1 << which_cpu);
	}
}

void process_pmap_updates(my_pmap)
	register pmap_t		my_pmap;
{
	register int		my_cpu = cpu_number();
	register pmap_update_list_t	update_list_p;
	register int		j;
	register pmap_t		pmap;

	update_list_p = &cpu_update_list[my_cpu];
	SIMPLE_LOCK(&update_list_p->lock);

	for (j = 0; j < update_list_p->count; j++) {
	    pmap = update_list_p->item[j].pmap;
	    if (pmap == my_pmap ||
		pmap == kernel_pmap) {

		INVALIDATE_TLB(update_list_p->item[j].start,
				update_list_p->item[j].end);
#ifdef	i860
		/*
		 * since we call flush() which pulls the big chain (dumps
		 * entire cache on the floor). We really only want to do this
		 * once!
		 */
		j = update_list_p->count;
#endif
	    }
	}
	update_list_p->count = 0;
	cpu_update_needed[my_cpu] = FALSE;
	SIMPLE_UNLOCK(&update_list_p->lock);
}

/*
 *	Interrupt routine for TBIA requested from other processor.
 *	This routine can also be called at all interrupts time if
 *	the cpu was idle. Some driver interrupt routines might access
 *	newly allocated vm. (This is the case for hd)
 */
void pmap_update_interrupt()
{
	register int		my_cpu;
	register pmap_t		my_pmap;
	int			s;

	my_cpu = cpu_number();

	if (current_thread() == THREAD_NULL)
	    my_pmap = kernel_pmap;
	else {
	    my_pmap = current_pmap();
	    if (!pmap_in_use(my_pmap, my_cpu))
		my_pmap = kernel_pmap;
	}

	/*
	 *	Raise spl to splvm (above splip) to block out pmap_extract
	 *	from IO code (which would put this cpu back in the active
	 *	set).
	 */
	s = splvm();

	do {

	    /*
	     *	Indicate that we're not using either user or kernel
	     *	pmap.
	     */
	    i_bit_clear(my_cpu, &cpus_active);

	    /*
	     *	Wait for any pmap updates in progress, on either user
	     *	or kernel pmap.
	     */
	    while (*(volatile int *)&my_pmap->lock.lock_data ||
		   *(volatile int *)&kernel_pmap->lock.lock_data)
		continue;

	    process_pmap_updates(my_pmap);

	    i_bit_set(my_cpu, &cpus_active);

	} while (cpu_update_needed[my_cpu]);
	
	splx(s);
}
#else	NCPUS > 1
/*
 *	Dummy routine to satisfy external reference.
 */
void pmap_update_interrupt()
{
	/* should never be called. */
}
#endif	NCPUS > 1

/*ARGSUSED*/
void
pmap_remove_attributes(pmap, start, end)
	pmap_t	pmap;
	vm_offset_t	start, end;
{
}

#if	i860	/* akp */

/*
 *	pmap_attribute:
 *
 *	Set/Get special memory attributes
 *
 */
kern_return_t pmap_attribute( pmap, address, size, attribute, value )
	pmap_t		pmap;
	vm_offset_t	address;
	vm_size_t	size;
	vm_machine_attribute_t	attribute;
	vm_machine_attribute_val_t* value;		/* IN/OUT */
{
	kern_return_t	kr;

	if (attribute != MATTR_CACHE)
		return KERN_INVALID_ARGUMENT;

	if (pmap == PMAP_NULL)
		return KERN_SUCCESS;

	switch (*value) {
	case MATTR_VAL_OFF:
	case MATTR_VAL_ON:
	case MATTR_VAL_GET:
		kr = KERN_INVALID_ARGUMENT;	/* implement later */
		break;

#if	i860XP
	case MATTR_VAL_CACHE_FLUSH:	/* flush from all caches */
		/*
		 *	we'll assume that if we're told to flush
		 *	a range from all caches, we'll simply
		 *	run the flush loop and writeback everything.
		 */
		flush();
		break;

	case MATTR_VAL_DCACHE_FLUSH:	/* flush from data cache */
	case MATTR_VAL_ICACHE_FLUSH:	/* flush from instruction cache */ 
		/*
		 *	invalidate, but don't necessarily trigger
		 *	write-backs.
		 */
		flush_tlb();
		break;
#else	i860XP
	/*
	 *	XR doesn't have physical tags, so just flush
	 *	everything.
	 */
	case MATTR_VAL_CACHE_FLUSH:	/* flush from all caches */
	case MATTR_VAL_DCACHE_FLUSH:	/* flush from data cache */
	case MATTR_VAL_ICACHE_FLUSH:	/* flush from instruction cache */ 
		flush();
		break;
#endif	i860XP
	}

	return kr;

}


/*
 *	Allocate a page for use during pmap_bootstrap().
 *
 *	Pages are stolen from the "hole_end" side.
 */
static vm_offset_t *pmap_bootstrap_i860_steal_page()
{
	pt_entry_t	*page;
	extern vm_offset_t hole_end;

	page = (pt_entry_t *) hole_end;
	hole_end += INTEL_PGBYTES;
	bzero((char *) page, INTEL_PGBYTES);

	return page;
}


/*
 *	Return TRUE if addr should be marked as write-thru.
 *
 *	In general, kernel text+data+bss+symbols+misc
 *	should *not* be marked as write-thru; physical
 *	free pages not-yet-in-use should be marked as
 *	write-thru.
 */
static boolean_t pmap_bootstrap_i860_mark_write_thru(addr)
	vm_offset_t	addr;
{
	extern	vm_offset_t	hole_start, hole_end;

#if	i860XP
	/*
	 *	Physical tags should take care of this...
	 */
	return FALSE;
#else	i860XP
	/*
	 *	Kernel text, data, bss, page dir, page tabs, etc.
	 */
	if ((addr >= hole_start) && (addr < hole_end))
		return FALSE;

	return TRUE;
#endif	i860XP
}

/*
 * Mark a range of addresses as noncacheable.
 */
void pmap_bootstrap_mark_nocache(phys_lo, phys_hi)
	vm_offset_t	phys_lo, phys_hi;
{
	pt_entry_t	*tab, *pdep, *ptep, tmp;
	pt_entry_t	*dir;
	vm_offset_t	cur, max;
	int		pagcnt;

	dir = kpde;
	cur = intel_round_page(phys_lo);
	max = intel_trunc_page(phys_hi);
	pagcnt=0;
	while (cur < max) {
		pdep = &dir[pdenum(cur)];
		if (*pdep == 0) {
			panic("pmap_bootstrap_i860_memrange: no page table");
		}
		tab = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);
		ptep = &tab[ptenum(cur)];
		tmp = pa_to_pte((vm_offset_t) cur)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_MOD
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE
			| INTEL_PTE_NCACHE;	/* Don't cache it */
		*ptep = tmp;
		cur += INTEL_PGBYTES;
		pagcnt++;
	}

	if (boot_mk_verbose) {
		printf("[ %d phys pages marked noncacheable 0x%x to 0x%x ]\n",
			pagcnt, phys_lo, phys_hi);
	}
}


/*
 *	Map a range of memory, physical:virtual as 1:1
 *
 *	Enough page tables to map phys_lo to phys_hi are
 *	allocated.
 */
void pmap_bootstrap_i860_memrange(dir, phys_lo, phys_hi)
	pt_entry_t	*dir;
	vm_offset_t	phys_lo, phys_hi;
{
	pt_entry_t	*tab, *pdep, *ptep, tmp;
	vm_offset_t	cur, max;
	int		ntables, tabcnt, pagcnt;

	tabcnt = 0;
	pagcnt = 0;
	phys_lo = intel_round_page(phys_lo);
	phys_hi = intel_trunc_page(phys_hi);
	ntables = intel_btop(phys_hi - phys_lo) / NPTES;

	/*
	 *	Allocate and initialize the page tables
	 */
	cur = phys_lo;
	while (ntables--) {
		tab = pmap_bootstrap_i860_steal_page();
		dir[pdenum(cur)] = pa_to_pte((vm_offset_t) tab)
				| i860_global_pmap_cache_policy
				| INTEL_PTE_REF
				| INTEL_PTE_VALID
				| INTEL_PTE_WRITE;
		cur += (INTEL_PGBYTES * NPTES);
		tabcnt++;
	}

	cur = phys_lo;
	max = phys_hi;
	while (cur < max) {
		pdep = &dir[pdenum(cur)];
		if (*pdep == 0) {
			panic("pmap_bootstrap_i860_memrange: no page table");
		}
		tab = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);
		ptep = &tab[ptenum(cur)];
		if (*ptep != 0) {
			panic("pmap_bootstrap_i860_memrange: address in use");
		}
		tmp = pa_to_pte((vm_offset_t) cur)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_MOD
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
		if (pmap_bootstrap_i860_mark_write_thru(cur))
			tmp |= INTEL_PTE_WTHRU;
		*ptep = tmp;
		cur += INTEL_PGBYTES;
		pagcnt++;
	}

	if (boot_mk_verbose) {
		printf("[ %d phys pages (%d tables) mapping 0x%x to 0x%x ]\n",
			pagcnt, tabcnt, phys_lo, phys_hi);
	}
}


/*
 *	Map all of physical memory 1-1.
 */
void pmap_bootstrap_i860_physmem(dirbase)
	pt_entry_t	**dirbase;	/* out */
{
	pt_entry_t	*dir;

	/*
	 *	Allocate the kernel page directory.
	 */
	dir = (pt_entry_t *) pmap_bootstrap_i860_steal_page();
	*dirbase = dir;

	/*
	 *	map avail_start to avail_end
	 */
	pmap_bootstrap_i860_memrange(dir, avail_start, avail_end);
#if	PARAGON860
	/*
	 *	On Paragon GP nodes, physical memory can be addressed
	 *	four different ways: unsnooped, snoop cpu0, snoop cpu1,
	 *	and fully snooped.
	 */
	/* unsnooped */
	pmap_bootstrap_i860_memrange(dir,
		avail_start-0x30000000, avail_end-0x30000000);

	/*
	 *	Enter physical mappings for any additional memory
	 *	that may be on an expansion card.
	 */
	paragon_pmap_bootstrap_expansion_memory(dir);
#endif	PARAGON860
}


/*
 *	Allocate (and initialize) enough kernel page tables to cover
 *	virtual_avail to virtual_end.
 */
void pmap_bootstrap_i860_virtmem(dir)
	pt_entry_t	*dir;
{
	vm_offset_t	cur, max;
	pt_entry_t	*tab, *pdep;

	cur = virtual_avail;
	max = virtual_end;
	while (cur < max) {
		pdep = &dir[pdenum(cur)];
		cur += INTEL_PGBYTES;
		if (*pdep != 0)
			continue;
		tab = (pt_entry_t *) pmap_bootstrap_i860_steal_page();
		*pdep = pa_to_pte((vm_offset_t) tab)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
	}
}


#if	iPSC860 || PARAGON860
/*
 *	Map in some crucial device registers.
 */
void pmap_bootstrap_i860_io(dir, pdev, vdev, user)
	pt_entry_t	*dir;
	vm_offset_t	pdev, vdev;
	boolean_t	user;
{
	pt_entry_t	*tab, *pdep, *ptep, bits;

	pdep = &dir[pdenum(vdev)];
	if (*pdep == 0) {
		/*
		 *	no page table...make one
		 */
		tab = (pt_entry_t *) pmap_bootstrap_i860_steal_page();
		bits = pa_to_pte((vm_offset_t) tab)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
		if (user) {
			bits |= INTEL_PTE_USER;
		}
		*pdep = bits;
	} else {
		/*
		 *	use the existing page table
		 */
		if (user && ((*pdep & INTEL_PTE_USER) == 0)) {
			*pdep |= INTEL_PTE_USER;
		}
		tab = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);
	}
	ptep = &tab[ptenum(vdev)];

	/*
	 *	Address collision?
	 */
	if (*ptep != 0) {
		panic("pmap_bootstrap_i860_io");
	}

	bits = pa_to_pte((vm_offset_t) pdev)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_MOD
			| INTEL_PTE_VALID
			| INTEL_PTE_NCACHE
			| INTEL_PTE_WTHRU
			| INTEL_PTE_WRITE;
	if (user) {
		bits |= INTEL_PTE_USER;
		bits &= ~INTEL_PTE_WRITE;
	}
	*ptep = bits;

	if (vdev < virtual_end)
		virtual_end = intel_trunc_page(vdev - INTEL_PGBYTES);

	if (boot_mk_verbose) {
		printf("[ device register phys %x mapped to virt %x. ]\n",
			pdev, vdev);
	}
}

#endif	iPSC860 || PARAGON860

/*
 *	Map in a page for use by the trap handler;
 *	it saves state at a negative offset from r0.
 *
 *	The i860 also starts executing code for all
 *	exceptions at virtual address 0xffffff00; this
 *	routine installs instructions to branch to alltraps().
 */
void pmap_bootstrap_i860_trap_page(dir)
	pt_entry_t	*dir;
{
	pt_entry_t	*pdep, *ptep, *tab;
	unsigned long	*vtrapvec, *ptrapvec;
	unsigned long	*vtrappage, *ptrappage;
	unsigned long	nop, br;
	long		broff;
	extern void	alltraps();

	vtrappage = (unsigned long *) 0xfffff000;
	if (vtrappage < (unsigned long *) virtual_end)
		virtual_end = intel_trunc_page((vm_offset_t)vtrappage-INTEL_PGBYTES);

	pdep = &dir[pdenum((vm_offset_t) vtrappage)];

	if (pte_to_pa(*pdep) == 0) {
		/*
		 *	Need to make a page table...
		 */
		tab = (pt_entry_t *) pmap_bootstrap_i860_steal_page();
		*pdep = pa_to_pte((vm_offset_t) tab)
			| i860_global_pmap_cache_policy
			| INTEL_PTE_REF
			| INTEL_PTE_VALID
			| INTEL_PTE_WRITE;
	} else {
		/*
		 *	Use the existing page table
		 */
		tab = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);
	}

	ptep = &tab[ptenum((vm_offset_t) vtrappage)];
#if	iPSC860 || PARAGON860
	/*
	 *	Most i860 designs have *some* physical memory at
	 *	0xffffff00; these two platforms have address decoding
	 *	circuitry that decodes the same memory (modulo memory
	 *	size) from some base address up to 0xffffffff.
	 *	One convenient alias is at 0xfffff000 and is the
	 *	last physical page of memory.  So, just subtract one
	 *	page from avail_end as a way to prevent us from using
	 *	the top-most page and map it one-to-one with 0xfffff000.
	 */
	ptrappage = vtrappage;
	avail_end -= INTEL_PGBYTES;
#else	iPSC860 || PARAGON860
	/*
	 *	Other i860 platforms may have to do something like this...
	 */
	ptrappage = (unsigned long *) pmap_bootstrap_i860_steal_page();
#endif	iPSC860 || PARAGON860

	*ptep = pa_to_pte((vm_offset_t) ptrappage)
		| i860_global_pmap_cache_policy
		| INTEL_PTE_REF
		| INTEL_PTE_MOD
		| INTEL_PTE_VALID
		| INTEL_PTE_WRITE;

	/*
	 * No need to do this again. It's done once at the boot time in
	 * i860/start.s. If a trap page must be written, then the following
	 * code needs to be rewritten because the trap page design has
	 * been changed. - sgd
	 */
#if 0
	nop = 0xa000; nop <<= 16;
	br  = 0x6800; br  <<= 16;
	vtrapvec = &vtrappage[960];
	ptrapvec = &ptrappage[960];
	broff = ((vm_offset_t) alltraps) - ((vm_offset_t) &vtrapvec[9]);

	ptrapvec[0] = nop;
	ptrapvec[1] = nop;
	ptrapvec[2] = nop;
	ptrapvec[3] = nop;
	ptrapvec[4] = nop;
	ptrapvec[5] = nop;
	ptrapvec[6] = nop;
	ptrapvec[7] = nop;
	ptrapvec[8] = br | ((broff >> 2) & 0x03ffffff);
	ptrapvec[9] = nop;
#endif
	if (boot_mk_verbose) {
		printf("[ trap-handler page at phys addr 0x%x. ]\n", ptrappage);
	}
}


/*
 *	This is the sloppy way to mark *all* of the initial
 *	kernel pages as accessed and dirty.
 *	No exceptions! (pun intended).
 */
void pmap_bootstrap_i860_dirty(dirbase)
	pt_entry_t	*dirbase;
{
	pt_entry_t	*ptep, *pdep, tmp;
	int		x, y, pdecnt, ptecnt;

	pdecnt = ptecnt = 0;
	pdep = &dirbase[0];
	for (y = 0; y < NPDES; y++, pdep++) {
		if (((tmp = *pdep) & INTEL_PTE_VALID) == 0) {
			continue;
		}
		*pdep = tmp
			| INTEL_PTE_REF
			;
		pdecnt++;

		ptep = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);

		for (x = 0; x < NPTES; x++, ptep++) {
			if ((tmp = *ptep) & INTEL_PTE_VALID) {
				if ((tmp & (INTEL_PTE_REF|INTEL_PTE_MOD)) == 0)
					*ptep = tmp
						| INTEL_PTE_REF
						| INTEL_PTE_MOD
						;
				ptecnt++;
			}
		}
	}
	if (boot_mk_verbose) {
     printf("[ pre-marked %d pages (%d pde's) as referenced and modified. ]\n",
			ptecnt, pdecnt);
	}
}


/*
 *	Mark all pages that are actualy page tables as uncached.
 */
void pmap_bootstrap_i860_uncache_tables(dirbase)
	pt_entry_t	*dirbase;
{
	pt_entry_t	*ptep, *pdep, *tab, tmp;
	int		i, cnt;

	assert(kernel_pmap->dirbase == dirbase);

	/*
	 *	the page directory (dirbase) should be un-cached too...
	 */
	ptep = pmap_pte(kernel_pmap, (vm_offset_t) kernel_pmap->dirbase);
	assert(ptep != PT_ENTRY_NULL);
	*ptep |= INTEL_PTE_NCACHE | INTEL_PTE_WTHRU;

	/*
	 *	scan the page directory for page table pointers;
	 *	mark the pages for *those* to be non-cacheable.
	 */
	cnt = 0;
	pdep = &dirbase[0];
	for (i = 0; i < NPDES; i++, pdep++) {
		if (((tmp = *pdep) & INTEL_PTE_VALID) == 0) {
			continue;
		}
		cnt++;
		tab = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);

		/*
		 * tab points to a page table.
		 * get the pte for it.
		 */
		ptep = pmap_pte(kernel_pmap, (vm_offset_t) tab);
		assert(ptep != PT_ENTRY_NULL);
		*ptep |= INTEL_PTE_NCACHE | INTEL_PTE_WTHRU;
	}

	if (boot_mk_verbose) {
	 printf("[ %d kernel page tables marked non-cacheable. ]\n", cnt);
	}
}


#if	MACH_KDB
#if	i860

/*
 *	show a pmap
 */
void db_show_pmap(addr, have_addr, count, modif)
	db_expr_t	addr;
	boolean_t	have_addr;
	db_expr_t	count;
	char		*modif;
{
	pmap_t		pmap = PMAP_NULL;
	pt_entry_t	pte, *ptep;
	pt_entry_t	match;
	thread_t	thread = THREAD_NULL;
	boolean_t	verbose = FALSE;
	boolean_t	debug = FALSE;
	boolean_t	and = TRUE;
	char		*fmt1 = " %c";
	char		*fmt2 = " %c%c";
	vm_offset_t	va, pa;
	int		i, c;
	extern int	indent;
	extern thread_t	db_default_thread;

	if (have_addr) {
		pmap = (pmap_t) addr;
	}

	if (db_option(modif, '_'))
		debug = TRUE;

	if (debug) {
		db_printf("a=%x ha=%d c=%d m=\"%s\" pmap=%x th=%x\n",
			addr, have_addr, count, modif, pmap, thread);
	}

	match = 0;
	while ((c = *modif++)) {
		switch (c) {
		case '.': and = FALSE; break;
		case 'v': verbose = TRUE; break;
		case 'w': match |= INTEL_PTE_WRITE; break;
		case 'u': match |= INTEL_PTE_USER; break;
		case 'T': match |= INTEL_PTE_WTHRU; break;
		case 'C': match |= INTEL_PTE_NCACHE; break;
		case 'a': match |= INTEL_PTE_REF; break;
		case 'd': match |= INTEL_PTE_MOD; break;
		case 'k': match &= ~INTEL_PTE_USER; break;
		case 'r': match &= ~INTEL_PTE_WRITE; break;
		case 't':
			pmap = PMAP_NULL;
			if (have_addr) {
				thread = (thread_t) addr;
			} else {
				if ((thread = db_default_thread)==THREAD_NULL)
					thread = current_thread();
			}
			break;
		}
	}
	if (debug)
		db_printf("match=%08x\n", match);

	if (pmap == PMAP_NULL) {
		if (thread != THREAD_NULL) {
			pmap = vm_map_pmap(thread->task->map);
		} else {
			db_printf("need a pmap or a thread...\n");
			return;
		}
	}
	if (pmap == PMAP_NULL)
		return;

	db_printf("pmap %x:\n", pmap);
	db_printf("  dirbase=%x refs=%d\n", pmap->dirbase, pmap->ref_count);
	db_printf("  private_pmap=%d\n", pmap->private_pmap);
	db_printf("  cpus_using=[");
	for (i = 0; i < 32; i++)
		if (pmap->cpus_using & (1 << i))
			db_printf(" %d", i);
	db_printf(" ]\n");
	if (verbose) {
		db_printf("  resident=%d wired=%d\n",
			pmap->stats.resident_count, pmap->stats.wired_count);
	}

	va = 0;
	for (va = 0; va <= (~0) - I860_PGBYTES; va += I860_PGBYTES) {
		if ((ptep = pmap_pte(pmap, va)) == PT_ENTRY_NULL)
			continue;
		if (((pte = *ptep) & INTEL_PTE_VALID) != INTEL_PTE_VALID)
			continue;
		if (match) {
			if (and == TRUE) {
				if ((pte & match) != match)
					continue;
			} else {
				if ((pte & match) == 0)
					continue;
			}
		}
		pa = pte_to_pa(pte);
		db_printf("    va=%08x pa=%08x pte=%x [", va, pa, pte);
		if (pte & INTEL_PTE_WRITE)
			db_printf(fmt1, 'w');
		else
			db_printf(fmt1, 'r');
		if (pte & INTEL_PTE_USER)
			db_printf(fmt1, 'u');
		else
			db_printf(fmt1, 'k');
		if (pte & INTEL_PTE_REF)
			db_printf(fmt2, ' ', 'a');
		else
			db_printf(fmt2, '!', 'a');
		if (pte & INTEL_PTE_MOD)
			db_printf(fmt2, ' ', 'd');
		else
			db_printf(fmt2, '!', 'd');
		if (pte & INTEL_PTE_WTHRU)
			db_printf(" wthru");
		if (pte & INTEL_PTE_NCACHE)
			db_printf(" !cache");
		db_printf(" ]\n");
		if (verbose) {
			db_printf("    pv_list {\n");
			indent += 6;
			(void) db_phystov(pa);
			indent -= 6;
			db_printf("    }\n");
		}

	}

}


/*
 *	print out the results of kvtophys(arg)
 */
void db_kvtophys(vaddr)
	vm_offset_t	vaddr;
{
	db_printf("0x%x", kvtophys(vaddr));
}
#endif	i860


/*
 *	Walk the pages tables.
 */
void db_show_vaddrs(dirbase)
	pt_entry_t	*dirbase;
{
	pt_entry_t	*ptep, *pdep, tmp;
	int		x, y, pdecnt, ptecnt;

	if (dirbase == 0) {
		dirbase = kernel_pmap->dirbase;
	}
	if (dirbase == 0) {
		db_printf("need a dirbase...\n");
	}
	dirbase = (pt_entry_t *) ((unsigned long) dirbase & ~INTEL_OFFMASK);

	db_printf("dirbase: 0x%x\n", dirbase);

	pdecnt = ptecnt = 0;
	pdep = &dirbase[0];
	for (y = 0; y < NPDES; y++, pdep++) {
		if (((tmp = *pdep) & INTEL_PTE_VALID) == 0) {
			continue;
		}
		pdecnt++;
		ptep = (pt_entry_t *) ((*pdep) & ~INTEL_OFFMASK);
		db_printf("dir[%4d]: 0x%x\n", y, *pdep);
		for (x = 0; x < NPTES; x++, ptep++) {
			if (((tmp = *ptep) & INTEL_PTE_VALID) == 0) {
				continue;
			}
			ptecnt++;
			db_printf("   tab[%4d]: 0x%x, va=0x%x, pa=0x%x\n",
				x,
				*ptep,
				(y << 22) | (x << 12),
				*ptep & ~INTEL_OFFMASK);
		}
	}

	db_printf("total: %d tables, %d page table entries.\n", pdecnt, ptecnt);

}


int db_phystov(pa)
	vm_offset_t	pa;
{
	pv_entry_t	pv_h;
	int		cnt;

	if (!pmap_valid_page(pa))
		return -1;

	cnt = 0;
	pv_h = pai_to_pvh(pa_index(pa));
	while (pv_h) {
		iprintf("pa=%08x va=%08x pmap=%x\n", pa, pv_h->va, pv_h->pmap);
		pv_h = pv_h->next;
		cnt++;
	}
	return cnt;
}


#endif	MACH_KDB

#if	i860XP
static int	use_i860xp_flush = 1;
#endif	i860XP

void set_dirbase(dirbase)
	register vm_offset_t	dirbase;
{
#if	i860XP
	if (use_i860xp_flush) {
		i860xp_flush_and_ctxsw(dirbase);
	} else {
		flush_and_ctxsw(dirbase);
	}
#else	i860XP
	flush_and_ctxsw(dirbase);
#endif	i860XP
}
#endif	i860

#include <mach_vm_debug.h>
#if	MACH_VM_DEBUG
int
pmap_list_resident_pages(pmap, listp, space)
	register pmap_t		pmap;
	register vm_offset_t	*listp;
	register int		space;
{
	return 0;
}
#endif	MACH_VM_DEBUG

#if	FROZEN_PAGES

/*
 * DEBUG: check for frozen pages, report those which are.
 */
check_fpl()
{
	register unsigned char	*p;
	int	fpl_size=paragon_total_vm_pages();

	for(p=pmap_frozen_page_list; p < &pmap_frozen_page_list[fpl_size]; p++)
		if ( *p )
			printf("  idx %d\n",(p-pmap_frozen_page_list));
	return 0;
}

/*
 * Verify is the specified VA is currently mapped in the physical hardware.
 *
 * implicit inputs:
 *	pmap is non-null(valid).
 *	va has been truncated to a VM page boundary.
 *
 * inputs:
 *	pmap	pointer to current pmap
 *	va	virtual-address in question.
 *
 * outputs:
 *	TRUE	VA is in the mapping hardware (pmap)
 *	FALSE	VA is NOT in the mapping hardware.
 */

boolean_t
pmap_page_is_present( pmap, va )
	pmap_t		pmap;
	vm_offset_t	va;
{
	register pt_entry_t	*pte;
	int			spl;
	
	PMAP_READ_LOCK(pmap, spl);

	/*
	 * check if VM page is resident
	 */
	if ( (pte=pmap_pte(pmap, va)) == PT_ENTRY_NULL ) {
		PMAP_READ_UNLOCK(pmap, spl);
		return FALSE;
	}

	{
		register int		i;

		i = ptes_per_vm_page;
		do {
			if ( !( (*pte) & INTEL_PTE_VALID) ) {
				PMAP_READ_UNLOCK(pmap, spl);
				return FALSE;
			}
			pte++;
		} while( --i > 0 );
	}

	PMAP_READ_UNLOCK(pmap, spl);

	return TRUE;
}

/*
 * Freeze a range of VM pages.
 *
 * implicit inputs:
 *	start address has been truncated to a VM page address.
 *	pmap is valid.
 *	VA is mapped to a valid physical address
 *
 * inputs:
 *	pmap		pmap which maps these pages
 *	va		starting VM page address
 *	pageCount	number of VM pages to freeze.
 *
 * outputs:
 *	none, panic() if problems.
 *
 */

pmap_freeze_vm_page_range( pmap, va, pageCount )
	pmap_t		pmap;
	vm_offset_t	va;
	int		pageCount;
{
	register pt_entry_t	*pte;
	register vm_offset_t	phys;
	int			spl;

	PMAP_READ_LOCK(pmap, spl);

	for(; pageCount > 0; --pageCount, va += PAGE_SIZE) {
		/*
		 * verify page is resident in hardware map
		 */
		if ( (pte=pmap_pte(pmap, va)) == PT_ENTRY_NULL ) {
			PMAP_READ_UNLOCK(pmap, spl);
			printf("pmap 0x%x va 0x%x\n",pmap,va);
			panic("pmap_freeze_page_range() null pte");
		}
	
		if ( ! ((*pte) & INTEL_PTE_VALID) ) {
			PMAP_READ_UNLOCK(pmap, spl);
			printf("pmap 0x%x va 0x%x pte 0x%x\n",pmap,va,pte);
			panic("pmap_freeze_page_range() invalid pte");
		}

		phys = pte_to_pa(*pte);	/* get physical page addr */

		if (!pmap_valid_page(phys))
			panic("pmap_freeze_vm_page_range() invalid phys page");

		assert( pmap_frozen_page_list[ pa_index(phys) ] == 0 );

#if	MACH_ASSERT
		pmap_frozen_page_list[ pa_index(phys) ]++;
#else
		pmap_frozen_page_list[ pa_index(phys) ] = TRUE;
#endif
	}
	PMAP_READ_UNLOCK(pmap, spl);
}


pmap_thaw_quickly( pmap, va, npages )
	pmap_t			pmap;
	register vm_offset_t	va;
	int			npages;
{
	register pt_entry_t	pt;
	register pt_entry_t	*dirbase;
	register vm_offset_t	pa;

	assert( pmap != PMAP_NULL );
	assert( pmap->dirbase != 0 );

	dirbase = pmap->dirbase;

	for (; npages > 0; --npages, va += PAGE_SIZE) {

		pt = dirbase[pdenum(va)];
		assert((pt & INTEL_PTE_VALID) != 0);

		pt = ((pt_entry_t *)(pt & INTEL_PTE_PFN))[ptenum(va)];
		assert((pt & INTEL_PTE_VALID) != 0);

		pa = (pt & INTEL_PTE_PFN) | (va & ~(INTEL_PTE_PFN));
		pmap_frozen_page_list[pa_index(pa)] = FALSE;
	}
}


/*
 * is the VM page frozen?
 *
 * inputs:
 *	phys	physical address of the kernel VM page.
 *
 * outputs:
 *	TRUE	VM page is frozen
 *	FALSE	page is NOT frozen (thawed).
 */

boolean_t
pmap_is_vm_page_frozen( phys )
	register vm_offset_t	phys;
{
	if ( !valid_page(phys) )
		return	FALSE;

	return( pmap_frozen_page_list[ pa_index(phys) ] ? TRUE : FALSE );
}

vm_offset_t
pmap_get_pa(pmap, va )
	pmap_t		pmap;
	vm_offset_t	va;
{

	register pt_entry_t	*pte;
	int			spl;
	vm_offset_t		pa;

	PMAP_READ_LOCK(pmap, spl);

	/*
	 * verify page is present in hardware map
	 */
	if ( (pte=pmap_pte(pmap, va)) == PT_ENTRY_NULL ) {
		PMAP_READ_UNLOCK(pmap, spl);
		return 0;
	}
	
	if ( ! ((*pte) & INTEL_PTE_VALID) ) {
		PMAP_READ_UNLOCK(pmap, spl);
		return 0;
	}

	/* extract physical page-address */
	pa = pte_to_pa(*pte);

	PMAP_READ_UNLOCK(pmap, spl);

	return pa;
}

/*
 * copy a range of VA's from a source pmap to the destination pmap.
 * Source PA's (physical address) are copied such that the destination pte
 * has write access removed.
 *
 * implicit inputs:
 *	Source & destination VM maps are locked.
 *	VM house-keeping has already been performed ala vm_copyout() or such.
 *	destination pmap will NEVER need a pmap_expand(), it's the kernel_pmap.
 *
 * inputs:
 *	src_pmap	source pmap ptr.
 *	src_va		starting VA in source pmap
 *	npages		# of VM pages to operate on.
 *	dst_pmap	destination pmap ptr.
 *	dst_va		starting VA in destination pmap.
 *
 * outputs:
 *	number of VM pages which were not copied.
 */
int pmap_copy_skips; /* XXX debug: # of times the dst VA was already mapped */
int pmap_copy_already;	/* XXX debug: # of times src & dst VA's were copies */

pmap_copy_range( src_pmap, src_va, npages, dst_pmap, dst_va )
	register pmap_t		src_pmap;
	register vm_offset_t	src_va;
	register vm_size_t	npages;
	register pmap_t		dst_pmap;
	register vm_offset_t	dst_va;
{
	register pt_entry_t	*spte, *dpte;
	register unsigned	src_pa;
	int			spl, misses=0;

	PMAP_READ_LOCK(src_pmap, spl);

	for(; npages > 0; npages--,src_va += PAGE_SIZE,dst_va += PAGE_SIZE) {
		if ( (spte = pmap_pte(src_pmap, src_va)) == PT_ENTRY_NULL )
			continue;

		if ( ! ( *spte & INTEL_PTE_VALID) ) {
			misses++;
			continue;
		}

		if ( (dpte = pmap_pte(dst_pmap, dst_va)) == PT_ENTRY_NULL )
			continue;

		/* collect the physical address */
		src_pa = pte_to_pa((unsigned)(*spte));

		/*
		 * Check if the pmap_enter() has already been accomplished;
		 * src-PA == dst-PA. Otherwise skip on to next VM page.
		 */
		if ( (*dpte & INTEL_PTE_VALID) ) {
			/* destination pte is valid, same phys page? */
			if ( src_pa == pte_to_pa((unsigned)(*dpte)) ) {
				pmap_copy_skips++;
				continue;	/* no work here, next VM page */
			}
			/*
			 *	This means that there are two different
			 *	physical pages, presumably with the exact
			 *	same contents.  vm_map_copyout() must have
			 *	done the work for us with the aggressive
			 *	enter machinery and a COW of the original
			 *	(in the source pmap) must have happened.
			 */
			pmap_copy_already++;
			continue;
#if	very_paranoid && i860
			assert(bcmp(src_pa, pte_to_pa(*dpte), PAGE_SIZE) == 0);
#endif
		}

		/* stuff the physical map */
		pmap_enter( dst_pmap, dst_va, src_pa, VM_PROT_READ, FALSE );
	}

	PMAP_READ_UNLOCK(src_pmap, spl);

	return misses;
}

#endif	/* FROZEN_PAGES */
