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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * HISTORY
 * 23-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Moved declaration of start_lock here in case LDEBUG is 1.
 *
 * $Log: mp_desc.c,v $
 * Revision 1.5  1994/11/18  20:33:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:24:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:23:12  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:22:59  dleslie
 * First R1_0 release
 *
 * Revision 2.5.4.2  92/04/30  11:50:45  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.5.4.1  92/02/18  18:48:03  jeffreyh
 * 	Adapt int_stack_high value for Corollary
 * 	[91/09/26  04:40:18  bernadat]
 * 
 * Revision 2.5  91/11/12  11:50:51  rvb
 * 	Added simple_lock_pause.
 * 	[91/11/12            rpd]
 * 
 * Revision 2.4  91/07/31  17:39:10  dbg
 * 	Move interrupt-stack allocation for multiprocessors here.
 * 	[91/07/30  16:54:10  dbg]
 * 
 * Revision 2.3  91/05/14  16:12:08  mrt
 * 	Correcting copyright
 * 
 * Revision 2.2  91/05/08  12:39:33  dbg
 * 	Created.
 * 	[91/03/21            dbg]
 * 
 */

#include <cpus.h>

#if	NCPUS > 1

#include <kern/cpu_number.h>
#include <mach/machine.h>
#include <vm/vm_kern.h>

#include <i386/mp_desc.h>
#include <i386/lock.h>

/*
 * The i386 needs an interrupt stack to keep the PCB stack from being
 * overrun by interrupts.  All interrupt stacks MUST lie at lower addresses
 * than any thread`s kernel stack.
 */

/*
 * Addresses of bottom and top of interrupt stacks.
 */
vm_offset_t	interrupt_stack[NCPUS];
vm_offset_t	int_stack_top[NCPUS];

/*
 * Barrier address.
 */
vm_offset_t	int_stack_high;

/*
 * First cpu`s interrupt stack.
 */
char		intstack[];	/* bottom */
char		eintstack[];	/* top */

/*
 * We allocate interrupt stacks from physical memory.
 */
extern
vm_offset_t	avail_start;

/*
 * Multiprocessor i386/i486 systems use a separate copy of the
 * GDT, IDT, LDT, and kernel TSS per processor.  The first three
 * are separate to avoid lock contention: the i386 uses locked
 * memory cycles to access the descriptor tables.  The TSS is
 * separate since each processor needs its own kernel stack,
 * and since using a TSS marks it busy.
 */

/*
 * Allocated descriptor tables.
 */
struct mp_desc_table	*mp_desc_table[NCPUS] = { 0 };

/*
 * Pointer to TSS for access in load_context.
 */
struct i386_tss		*mp_ktss[NCPUS] = { 0 };

/*
 * Pointer to GDT to reset the KTSS busy bit.
 */
struct fake_descriptor	*mp_gdt[NCPUS] = { 0 };

/*
 * Boot-time tables, for initialization and master processor.
 */
extern struct fake_descriptor	idt[IDTSZ];
extern struct fake_descriptor	gdt[GDTSZ];
extern struct fake_descriptor	ldt[LDTSZ];
extern struct i386_tss		ktss;

/*
 * Allocate and initialize the per-processor descriptor tables.
 */

struct fake_descriptor ldt_desc_pattern = {
	(unsigned int) 0,
	LDTSZ * sizeof(struct fake_descriptor) - 1,
	0,
	ACC_P|ACC_PL_K|ACC_LDT
};
struct fake_descriptor tss_desc_pattern = {
	(unsigned int) 0,
	sizeof(struct i386_tss),
	0,
	ACC_P|ACC_PL_K|ACC_TSS
};

struct mp_desc_table *
mp_desc_init(mycpu)
	register int	mycpu;
{
	register struct mp_desc_table *mpt;

	if (mycpu == master_cpu) {
	    /*
	     * Master CPU uses the tables built at boot time.
	     * Just set the TSS and GDT pointers.
	     */
	    mp_ktss[mycpu] = &ktss;
	    mp_gdt[mycpu] = gdt;
	    return 0;
	}
	else {
	    /*
	     * Other CPUs allocate the table from the bottom of
	     * the interrupt stack.
	     */
	    mpt = (struct mp_desc_table *) interrupt_stack[mycpu];

	    mp_desc_table[mycpu] = mpt;
	    mp_ktss[mycpu] = &mpt->ktss;
	    mp_gdt[mycpu] = mpt->gdt;

	    /*
	     * Copy the tables
	     */
	    bcopy((char *)idt,
		  (char *)mpt->idt,
		  sizeof(idt));
	    bcopy((char *)gdt,
		  (char *)mpt->gdt,
		  sizeof(gdt));
	    bcopy((char *)ldt,
		  (char *)mpt->ldt,
		  sizeof(ldt));
	    bzero((char *)&mpt->ktss,
		  sizeof(struct i386_tss));

	    /*
	     * Fix up the entries in the GDT to point to
	     * this LDT and this TSS.
	     */
	    mpt->gdt[sel_idx(KERNEL_LDT)] = ldt_desc_pattern;
	    mpt->gdt[sel_idx(KERNEL_LDT)].offset =
			(unsigned int) mpt->ldt;
	    fix_desc(&mpt->gdt[sel_idx(KERNEL_LDT)], 1);

	    mpt->gdt[sel_idx(KERNEL_TSS)] = tss_desc_pattern;
	    mpt->gdt[sel_idx(KERNEL_TSS)].offset =
			(unsigned int) &mpt->ktss;
	    fix_desc(&mpt->gdt[sel_idx(KERNEL_TSS)], 1);

	    mpt->ktss.ss0 = KERNEL_DS;
	    mpt->ktss.io_bit_map_offset = 0x0FFF;	/* no IO bitmap */

	    return mpt;
	}
}


/*
 * Called after all CPUs have been found, but before the VM system
 * is running.  The machine array must show which CPUs exist.
 */
void
interrupt_stack_alloc()
{
	register int	i;
	int		cpu_count;
	vm_offset_t	stack_start;

	/*
	 * Count the number of CPUs.
	 */
	cpu_count = 0;
	for (i = 0; i < NCPUS; i++)
	    if (machine_slot[i].is_cpu)
		cpu_count++;

	/*
	 * Allocate an interrupt stack for each CPU except for
	 * the master CPU (which uses the bootstrap stack)
	 */
	stack_start = phystokv(avail_start);
	avail_start = round_page(avail_start + INTSTACK_SIZE*(cpu_count-1));

	/*
	 * Set up pointers to the top of the interrupt stack.
	 */
	for (i = 0; i < NCPUS; i++) {
	    if (i == master_cpu) {
		interrupt_stack[i] = (vm_offset_t) intstack;
		int_stack_top[i]   = (vm_offset_t) eintstack;
	    }
	    else if (machine_slot[i].is_cpu) {
		interrupt_stack[i] = stack_start;
		int_stack_top[i]   = stack_start + INTSTACK_SIZE;

		stack_start += INTSTACK_SIZE;
	    }
	}

	/*
	 * Set up the barrier address.  All thread stacks MUST
	 * be above this address.
	 */
#if	AT386
	/*
	 * intstack is at higher addess than stack_start for AT mps
	 * so int_stack_high must point at eintstack.
	 * XXX
	 * But what happens if a kernel stack gets allocated below
	 * 1 Meg ? Probably never happens, there is only 640 K available
	 * There.
	 */
	int_stack_high = (vm_offset_t) eintstack;
#else	AT386
	int_stack_high = stack_start;
#endif	AT386
}

/*
 * Start lock is declared here rather than in i386/start.s, in case
 * locks are compiled with debugging and therefore might be more than
 * one word long. Also it must be initialized to zero as BSS has not
 * been cleared at boo time
 */

struct slock start_lock = {0};		/* used in start.s */

/* XXX should be adjusted per CPU speed */
int simple_lock_pause_loop = 100;

unsigned int simple_lock_pause_count = 0;	/* debugging */

void
simple_lock_pause()
{
	static volatile int dummy;
	int i;

	simple_lock_pause_count++;

	/*
	 * Used in loops that are trying to acquire locks out-of-order.
	 */

	for (i = 0; i < simple_lock_pause_loop; i++)
	    dummy++;	/* keep the compiler from optimizing the loop away */
}

#endif /* NCPUS > 1 */
