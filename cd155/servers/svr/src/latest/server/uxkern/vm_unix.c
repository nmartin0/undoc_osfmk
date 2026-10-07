/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: vm_unix.c,v $
 * Revision 1.8  1995/02/01  17:02:26  nandy
 * In vm_mmap_delete, NULL out the pager after deallocating it
 *
 *  Reviewer:OSF
 *  Risk: L
 *  Benefit or PTS #: 11960 ( also need fix in the ipd )
 *  Testing: IPD regression tests
 *  Module(s): vm_unix.c and IPD fixes.
 *
 * Revision 1.7  1994/11/30  01:32:32  nandy
 * Added type casting to call to mach_port_deallocate() in vm_exit
 *
 *  Reviewer: jlitvin
 *  Risk:L
 *  Benefit or PTS #: 11549
 *  Testing: PTS test case
 *  Module(s): uxkern/vm_unix.c
 *
 * Revision 1.6  1994/11/18  20:50:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/12  17:45:32  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.4  1993/07/14  18:45:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:07:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:34:35  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.13  93/06/20  18:29:38  yazz
 * [ ad1.04 merge ]
 * 	After deallocating the entire vm_mmap linked list, be sure to also
 * 	null out the proc structure pointer that was pointing to it, instead
 * 	of leaving a bogus pointer hanging around.
 * 
 * Revision 2.12  93/05/27  10:31:07  bolinger
 * 	Shrink utask (and hence proc) struct by allocating vm_mmap structs
 * 	dynamically.  Here, arrange alloc/dealloc and change array traversals
 * 	to list manipulations.  Also fix bug in vm_mmap_delete() when
 * 	deleting a prefix of an existing region.
 * 
 * Revision 2.11  93/04/08  11:44:32  loverso
 * 	Print a more detailed message for vm_mmap_add failure.
 * 	[1992/09/23  13:00:23  barbou]
 *
 * 	Merged Charles Silvers's version with 1.0.4.1b1 (extend mmap regions)
 * 	[1992/06/25  15:03:13  emcmanus]
 *
 * 	Fixed the "extend mmap regions" code to get it right. (loverso)
 *
 * Revision 2.10  92/05/31  19:00:01  loverso
 * 	For MAPPED_FILES, in vm_msync() also clean pages for 
 * 	MS_INVALIDATE option.
 * 	[92/05/27            roy]
 * 
 * Revision 2.9  92/05/24  14:01:25  pjg
 * 	92/03/07  18:00:25  sp
 * 	Delete all the mmaps and parts of mmaps when munmap is used with
 * 	addresses that don't correspond exactly to regions created by 
 * 	mmap (Bug #104)
 * 
 * 	92/03/05  12:52:27  sp
 * 	Only sync the regions it makes sense to. 
 * 
 * Revision 2.8  92/05/18  12:27:07  roy
 * 	Revision 2.5.1.1  92/04/22  10:05:31  roy
 * 	Undid change in revision 2.7.  Must hold a pager port right because of
 * 	possible need to send a vnode_pager_flush message.  This is unfortunate
 * 	because it adds expense at fork, exec, and exit time.  Implemented
 * 	vm_mmap_fork and vm_mmap_exit.  
 * 	[92/03/25            roy]
 * 
 * Revision 2.7  92/05/01  10:05:31  rabii
 * 	vm_mmap data structure no longer holds a true pager send right 
 * 	for OSF1_ADFS.
 * 	[92/04/29            roy]
 * 
 * Revision 2.6  92/03/20  11:33:34  pjg
 * 	don't deallocate the server port in vm_mmap_delete
 * 
 * Revision 2.5  92/03/15  14:33:08  roy
 * 	for osf1_adfs add keep the server_port in the vm_mmap structure too and
 * 	use it to send a remote vnode_flush_object.
 * 
 * 	Revision 3.9  92/02/26  13:22:25  sp
 * 	Fix vm_mmap_purge bug which would not copy emulator region if there was
 * 	a keep on exec region after it
 * 
 * 	Revision 3.6  91/12/17  17:19:26  jose
 * 	Changed interface to vm_mmap_add to include object
 * 	instead of vnode (for vnode_flush_object).
 * 
 * Revision 2.4  92/03/09  13:57:01  durriya
 * 	[ 92/02/18  18:57:20  jose ]
 * 	Fixed vm_mmap_lookup
 * 	possibly correcting bug 71 (Simon).
 * 
 * Revision 2.3  92/03/01  18:34:55  pjg
 * 	Call remote_vnode_flush_object in msync for osf1_adfs (durriya).
 * 
 * Revision 2.2  91/12/16  12:47:59  roy
 * 	Initial check-in.
 * 
 * Revision 3.5  91/12/02  16:59:05  sp
 * Check that a region being flushed in vm_msync is backed by the vnode
 * pager (ie has a vnode associated with it).
 * 
 * Revision 3.4  91/11/26  15:34:19  sp
 * Upgrade to 1.0.3
 * 
 * Revision 3.3  91/10/28  11:10:22  condict
 * Reimplement vm_msleep/vm_mwakeup to make Berkeley semaphores work.  The globl
 * semaphore lock is gone, as are the copyouts.  The crucial change is that
 * copyin is used to check the semaphore between assert_wait and thread_block.
 * 
 * Revision 3.2  91/10/24  09:04:36  sp
 * rewrite the mapped region add/delete code and vm_exec
 * add support for msync via vm_msync
 * 
 * Revision 3.1  91/10/23  16:39:11  condict
 * Add vm_msem_init, vm_msleep and vm_mwakeup, the implementation of Berkeley
 * semaphores.  Compiles and executes but isn't correct yet.
 * 
 * Revision 3.0  91/10/15  12:13:56  sp
 * New file containing stuff from vm/vm_unix.c
 * 
 * $EndLog$
 */
#include <mapped_files.h>

#include <sys/types.h>
#include <sys/time.h>
#include <sys/vnode.h>
#include <sys/user.h>
#include <sys/mman.h>
#include <kern/lock.h>
#include <kern/sched_prim.h>
#include <kern/kalloc.h>

#include <uxkern/import_mach.h>

/*
 *	Called to deallocate the current task's address space during exec.
 *	All regions except those marked keep_on_exec are deallocated;
 *	keep_on_exec regions are preserved.  The "ignore_koe" flag causes
 *	all regions (even those marked keep-on-exec) to be deallocated;
 *	this is used, for example, when exec'ing a setuid program.
 *	For the single server there are areas where we want to ignore the
 *	ignore_koe flag, such as for the emulator.
 */

vm_exec(ignore_koe)
boolean_t	ignore_koe;
{
	void		vm_mmap_exec_purge();
	void		vm_mmap_region_check();
	struct proc	*procp = u.uu_procp;
	task_t		my_task = procp->p_task;
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap  *mmap;
	vm_offset_t	start;
	vm_offset_t	end;
	vm_offset_t	next;

	vm_mmap_exec_purge(procp, ignore_koe);

	vm_mmap_region_check(procp);

	/*
	 * Preserve the exact semantics of this loop -- after
	 * handling last vm_mmap, call vm_deallocate() for region
	 * up to VM_MAX_ADDRESS.
	 */
	U_MMAP_READ_LOCK(utask);
	for (start = VM_MIN_ADDRESS, mmap = utask->uu_mmap;
		mmap && start < VM_MAX_ADDRESS; mmap = mmap->link) {
		end = mmap->start;
		next = mmap->start + mmap->length;
		(void)vm_deallocate(my_task, start, (vm_size_t)(end - start));
		start = next;
	}
	U_MMAP_UNLOCK(utask);
	if (start < VM_MAX_ADDRESS)
		(void)vm_deallocate(my_task, start,
			(vm_size_t)(VM_MAX_ADDRESS - start));
}

void
vm_fork(parent, child)
struct proc *parent;
struct proc *child;
{
	struct utask	*ctask, *ptask;
	struct vm_mmap  *cmmap, *pmmap, **clink;
	kern_return_t	ret;

	ctask = &child->p_utask;
	ptask = &parent->p_utask;

	/*
	 * Nobody knows about the child yet, and it's guaranteed
	 * to be single-threaded, so need lock only parent's
	 * vm_mmap list.
	 */
	U_MMAP_READ_LOCK(ptask);
	for (clink = &ctask->uu_mmap, pmmap = ptask->uu_mmap; pmmap;
		pmmap = pmmap->link, clink = &cmmap->link) {
		cmmap = (struct vm_mmap *)kalloc(sizeof (*cmmap));
		if (cmmap == NULL)
			panic("vm_fork: can't kalloc child's vm_mmap\n");
		*clink = cmmap;
		*cmmap = *pmmap;
		cmmap->link = NULL;
#ifdef	OSF1_ADFS
		if (pmmap->pager != MEMORY_OBJECT_NULL)
			if (ret = mach_port_mod_refs(mach_task_self(), 	
						     pmmap->pager,
						     MACH_PORT_RIGHT_SEND, +1))
				panic("vm_mmap_fork port 0x%x\n", ret);
#endif
	}
	U_MMAP_UNLOCK(ptask);
}

void
vm_mmap_init()
{
}

void
vm_mmap_proc_init(procp)
struct proc *procp;
{
	struct utask	*utask = &procp->p_utask;

	utask->uu_mmap = NULL;
}

void
vm_exit(procp)
struct proc *procp;
{
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap  *mmap, *nmmap;

	U_MMAP_WRITE_LOCK(utask);
	for (mmap = utask->uu_mmap; mmap; mmap = nmmap) {
#ifdef	OSF1_ADFS
		kern_return_t	ret;

		if (mmap->pager != MEMORY_OBJECT_NULL)
			if (ret = mach_port_deallocate(mach_task_self(), 
						       (mach_port_t)mmap->pager))
				panic("vm_mmap_exit port 0x%x\n", ret);
#endif
		nmmap = mmap->link;
		kfree(mmap, sizeof (*mmap));
	}
	utask->uu_mmap = NULL;	/* don't litter proc struct with bogus ptrs */
	U_MMAP_UNLOCK(utask);
}
	
void
vm_mmap_region_check(procp)
struct proc	*procp;
{
	task_t		my_task = procp->p_task;
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap  **olink, *old, *onext;
	vm_address_t	start, end;
	vm_size_t	size;
	vm_prot_t	prot;
	vm_prot_t	max_prot;
	vm_inherit_t	inherit;
	boolean_t	shared;
	memory_object_name_t name;
	vm_offset_t	offset;
	kern_return_t	ret;

	U_MMAP_WRITE_LOCK(utask);
	for (olink = &utask->uu_mmap, old = *olink; old; old = onext) {
		onext = old->link;
		if (!(old->flags & VM_MMAP_FORCE_KEEP_ON_EXEC)) {
			start = old->start;
			end = start + old->length;
			if ((vm_region(my_task, &start, &size, &prot, &max_prot,
					&inherit, &shared, &name, &offset)
					!= KERN_SUCCESS)
			   || (old->start < start)
			   || (end > (start + size))) {
#ifdef  OSF1_ADFS
			    if (old->pager != MEMORY_OBJECT_NULL)
				    if (ret = mach_port_deallocate(
					            mach_task_self(), 
						    old->pager))
					    panic("vm_mmap_region_check 0x%x\n",
						  ret);
#endif
			     kfree(old, sizeof (*old));
			     *olink = onext;
			     continue;
			}
		}
		olink = &old->link;
	}
	U_MMAP_UNLOCK(utask);
}

/*
 * Allocate and initialize a new vm_mmap entry.
 */
#ifdef 	OSF1_ADFS
struct vm_mmap *
vm_mmap_new(addr, len, pager, node, offset, flags)
#else
vm_mmap_new(addr, len, pager, offset, flags)
#endif
	caddr_t		addr;
	unsigned	len;
	memory_object_t pager;
#ifdef  OSF1_ADFS
	node_t		node;
#endif
	vm_offset_t	offset;
	unsigned	flags;
{
	struct vm_mmap	*new;

	new = (struct vm_mmap *)kalloc(sizeof (*new));
	if (new == NULL)
		panic("vm_mmap_new: can't kalloc vm_mmap\n");
	new->start = (vm_address_t)addr;
	new->length = len;
	new->pager = pager; 
#ifdef OSF1_ADFS
	new->node = node;
#endif
	new->offset = offset;
	new->flags = flags;
	new->link = NULL;
	return (new);
}

void
vm_mmap_delete(procp, addr, len)
struct proc	*procp;
caddr_t		addr;
u_int		len;
{
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap  **plink, *cur, *new, *next;
	u_int		end_len;
	vm_offset_t	new_offset;
	kern_return_t	ret;

	U_MMAP_WRITE_LOCK(utask);
	for (plink = &utask->uu_mmap, cur = *plink; cur; cur = next) {
		next = cur->link;
		/*
		 * If the user address is beyond the end of this region
		 * then go to the next one.
		 */
		if ((vm_address_t)addr >= (cur->start + cur->length)) {
			plink = &cur->link;
			continue;
		}
		/*
		 * If the user address is before the start of this region
		 * we move the address up to the start, decrementing
		 * the length accordingly. If the end of the users area is
		 * also before the start then we have nothing to do.
		 */
		if ((vm_address_t)addr < cur->start) {
			if ((vm_address_t)(addr + len) <= cur->start) {
				U_MMAP_UNLOCK(utask);
				return;
			}
			len -= cur->start - (vm_address_t)addr;
			addr = (caddr_t)cur->start;
		}

		if (cur->start == (vm_address_t)addr) {
			/*
			 * We want to delete a section from the start
			 * of this region.
			 */
			if (len < cur->length) {
				cur->start += len;
				cur->offset += len;
				cur->length -= len;
				U_MMAP_UNLOCK(utask);
				return;
			}
			/*
			 * At this point we know we want to get rid of the
			 * entire mmap entry. Adjust the user address and
			 * length and then close up the list of entries.
			 */
			len -= cur->length;
			addr += cur->length;
#ifdef  OSF1_ADFS
			if (cur->pager != MEMORY_OBJECT_NULL) {
				if (ret = mach_port_deallocate(mach_task_self(), 
						        cur->pager))
					panic("vm_mmap_delete port 0x%x\n", ret);
				cur->pager = MEMORY_OBJECT_NULL;
			}
#endif
			kfree(cur, sizeof (*cur));
			*plink = next;
			continue;
		}
		/*
		 * We want to delete a section out of this region
		 * that is not at the start. Find out how much of
		 * the region is after the user address.
		 */
		end_len = cur->length -
			((vm_address_t)addr - cur->start);
		if (len >= end_len) {
			/*
			 * We want to delete to the end. Just 
			 * mark the region as shorter.
			 */
			cur->length -= end_len;
			len -= end_len;
			addr += end_len;
		} else {
			/*
			 * The worst scenario, we want to delete
			 * a chunk out of the middle of this region.
			 * We do this by shortening this entry and
			 * creating a new entry for the end.
			 */
			end_len -= len;
			/*
			 * calculate the offset into the object for
			 * the new mmap entry
			 */
			new_offset = cur->offset + ((vm_address_t)addr -
				cur->start) + len;
			cur->length = (vm_address_t)addr - cur->start;
#ifdef  OSF1_ADFS
			/*
			 * Create a new send right to the pager port. 
			 */
			if (cur->pager != 
			    MEMORY_OBJECT_NULL)		
				if (ret = mach_port_mod_refs(mach_task_self(), 	
					      cur->pager,
					      MACH_PORT_RIGHT_SEND, +1))
					panic("vm_mmap_fork port 0x%x\n", ret);
			new = vm_mmap_new(addr + len, end_len, 
					cur->pager, cur->node, new_offset,
					cur->flags);
#else
			new = vm_mmap_new(addr + len, end_len, 
					cur->pager, new_offset, cur->flags);
#endif
			new->link = cur->link;
			cur->link = new;
			next = new;
			U_MMAP_UNLOCK(utask);
			return;
		}
		plink = &cur->link;
	}
	U_MMAP_UNLOCK(utask);
}

void
vm_mmap_exec_purge(procp, ignore_koe)
struct proc	*procp;
boolean_t	ignore_koe;
{
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap	**plink, *cur, *next;
	kern_return_t	ret;

	U_MMAP_WRITE_LOCK(utask);
	for (plink = &utask->uu_mmap, cur = *plink; cur; cur = next) {
		next = cur->link;
		if ((ignore_koe ||
			!(cur->flags & VM_MMAP_KEEP_ON_EXEC)) &&
			!(cur->flags & VM_MMAP_FORCE_KEEP_ON_EXEC)) {
#ifdef	OSF1_ADFS
	        	if (cur->pager != MEMORY_OBJECT_NULL) {
				if (ret = mach_port_deallocate(
					mach_task_self(), cur->pager))
					panic("vm_mmap_exec_perge port 0x%x\n",
						ret);
			}
#endif
			kfree(cur, sizeof (*cur));
			*plink = next;
			continue;
		}
		plink = &cur->link;
	}
	U_MMAP_UNLOCK(utask);
}

kern_return_t
#ifdef 	OSF1_ADFS
vm_mmap_add(procp, addr, len, pager, node, offset, flags)
#else
vm_mmap_add(procp, addr, len, pager, offset, flags)
#endif
	struct proc	*procp;
	caddr_t		addr;
	unsigned	len;
	memory_object_t pager;
#ifdef  OSF1_ADFS
	node_t		node;
#endif
	vm_offset_t	offset;
	unsigned	flags;
{
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap	**plink, *last, *cur, *new;

	/*
	 * We don't do anything useful with these regions just yet so
	 * just ignore them to save space.
	 */

        if ((pager == (memory_object_t)0) && (flags == 0))
        	return(KERN_SUCCESS);

	U_MMAP_WRITE_LOCK(utask);
	/*
	 * First, try to combine the requested region with an existing
	 * one, if the match is exact.
	 */ 
	for (cur = utask->uu_mmap; cur; cur = cur->link) {
		if ((vm_address_t)addr < cur->start)
			break;
		if ((vm_address_t)addr == cur->start + cur->length
			&& cur->pager == pager
			&& cur->offset + cur->length == offset
			&& cur->flags == flags) {
			cur->length += len;
			U_MMAP_UNLOCK(utask);
			return(KERN_SUCCESS);
		}
	}

	/*
	 * No match with existing regions.  Have to create new one.
	 * Put the new region just before the first existing one
	 * with an end address greater than the new one's start
	 * address.  The termination condition (cur NULL) is checked
	 * for within the loop -- if we don't find an existing
	 * region at a higher address, the new region goes at the
	 * end of the list.
	 */
	last = NULL;
	for (plink = &utask->uu_mmap, cur = *plink; ; cur = cur->link) {
		if (!cur || (vm_address_t)addr <= cur->start) {
			/*
			 * Double-check here that there's no overlap between
			 * adjacent regions.
			 */
			if (last &&
				(vm_address_t)addr < last->start + last->length ||
				cur && cur->start < (vm_address_t)(addr + len))
			{
				panic("vm_mmap_add: regions overlap\n");
			}
#ifdef OSF1_ADFS
			new = vm_mmap_new(addr, len, pager, node, offset, flags);
#else
			new = vm_mmap_new(addr, len, pager, offset, flags);
#endif
			new->link = cur;
			*plink = new;
			break;
		}
		plink = &cur->link;
		last = cur;
	}
	U_MMAP_UNLOCK(utask);
	return(KERN_SUCCESS);
}

struct vm_mmap *
vm_mmap_lookup(procp, addr, len)
struct proc	*procp;
caddr_t		addr;
unsigned	len;
{
	struct utask	*utask = &procp->p_utask;
	struct vm_mmap  *cur;

	U_MMAP_READ_LOCK(utask);
	for (cur = utask->uu_mmap; cur; cur = cur->link)
		if (((vm_address_t)addr >= cur->start) &&
			((vm_address_t)addr < (cur->start + cur->length))) {
			U_MMAP_UNLOCK(utask);
			return(cur);
		}
	U_MMAP_UNLOCK(utask);
	return((struct vm_mmap *)0);
}

/*
 *  Synchronized the data in a mapped region and file either by pushing
 *  modified pages back to the object or by invalidating pages.
 *  Note this routine currently only works with the inode pager.
 */
kern_return_t vm_msync(start_addr, len, flags)
caddr_t		start_addr;
vm_size_t	len;
int		flags;
{
	struct vm_mmap	*mmap;
	int		should_clean;
	int		should_invalid;
	int		wait;
	kern_return_t	result;
	vm_offset_t	flush_offset;
	vm_size_t	flush_len;

	while (len > 0) {
		mmap = vm_mmap_lookup(u.u_procp, start_addr, len);
		if ((mmap == (struct vm_mmap *)0) ||
		    (mmap->pager == (memory_object_t)0))
			return(KERN_INVALID_ADDRESS);

		flush_offset = mmap->offset +
				((vm_offset_t)start_addr - mmap->start);
		if (((vm_offset_t)start_addr + len) >
						(mmap->start + mmap->length))
			flush_len = (mmap->start + mmap->length) -
						(vm_offset_t)start_addr;
		else
			flush_len = len;

		should_invalid = (flags & MS_INVALIDATE ? TRUE : FALSE);
		if (mmap->flags & VM_MMAP_ANONYMOUS) {
			if (!should_invalid) {
				start_addr += flush_len;
				len -= flush_len;
				continue;
			}
			should_clean = FALSE;
			wait = FALSE;
		} else {
			should_clean = (flags & (MS_ASYNC|MS_SYNC) ? 
					TRUE : FALSE);
			wait = ((flags & MS_SYNC) ? TRUE : FALSE);
#if	MAPPED_FILES
			/*
			 * The MAPPED_FILES option supports unified caching
			 * between read/write data and mmap'd regions.  If
			 * we're msync'ing with MS_INVALIDATE then we must
			 * make sure that we don't lose modifications due
			 * to previous write()'s.  Hence, clean the pages
			 * as well as flush them.  The downside is that
			 * it's possible the dirty data isn't due to write()'s
			 * in which case we're doing extra disk writes.
			 * This could be rectified by distinguishing between
			 * different types of dirty data in the kernel.
			 */
			if (flags & MS_INVALIDATE)
				should_clean = 1;
#endif
		}

		if (should_clean && (mmap->flags&VM_MMAP_PRIVATE))
			return(KERN_INVALID_ARGUMENT);

#ifdef OSF1_ADFS
		result = remote_vnode_pager_flush(mmap->node,
                                                  mmap->pager,
                                                  flush_offset, flush_len, 
                                                  wait, should_clean, 
                                                  should_invalid,
                                                  VM_PROT_NONE);

#else
		result = vnode_pager_flush(mmap->pager, flush_offset,
				flush_len, wait, should_clean, should_invalid,
				VM_PROT_NONE);
#endif

		if (result != KERN_SUCCESS)
			return(result);

		start_addr += flush_len;
		len -= flush_len;
	}

	return(result);
}

#if 0
/*
 * This routine is unnecessary in the server, because we don't have a global
 * semaphore lock.  It is here just for code compatibility.
 */
void vm_msem_init()
{
}
#endif

/* A hash function to find a reasonably unique sleep channel for each
 * semaphore.  It XOR's the port name of the containing memory
 * object with the offset of the semaphore within the memory object.
 *
 * Parameters:
 *	usem		-- user address of a semaphore
 *	object_name	-- port name of the memory obj containing the semaphore
 *	address		-- user address of region containing the semaphore
 *	offset		-- offset within memory object of start of this region
 *
 */
#define SEM_HASH(port_name, offset, address, usem) ( \
	0xA0000000 | ((int)port_name ^ \
	((int)offset + ((caddr_t) usem - (caddr_t) address))) \
)

/*
 * Called by the msem_lock library function when the semaphore is found to
 * be already locked.  It sleeps on a channel that is hashed from information
 * about the memory object containing the semaphore (this is independent of
 * which user address was supplied, and is the same for all users of the
 * semaphore) after first making sure that a wakeup call didn't already arrive.
 */
kern_return_t vm_msleep(usem)
msemaphore *usem;
{
	msemaphore	sem;
	uthread_t	my_thread = &u;
	task_t		my_task = my_thread->uu_procp->p_task;
				/* OUT params of vm_region: */
	vm_address_t	address = (vm_address_t) usem;
	vm_size_t	size;
	vm_prot_t	protection, max_protection;
	vm_inherit_t	inheritance;
	boolean_t	shared;
	memory_object_name_t
			object_name;
	vm_offset_t 	offset;
	kern_return_t	result;

	/* Find the port name for the memory_object containing the semaphore,
	 * and the offset of the semaphore within the memory object:
	 */
	if ((result = vm_region(my_task, &address, &size,
				&protection, &max_protection, &inheritance,
				&shared, &object_name, &offset))
	    != KERN_SUCCESS)
	{
		
		return(result);
	}
	/* If it found a region at a higher address, this address is not in any
	 * region:
	 */
	if (address > (vm_address_t) usem)
		return KERN_INVALID_ADDRESS;

	/* Calculate a channel from the object name and offset and prepare to
	 * sleep on that channel.  From this point on, a wakeup on this channel
	 * will cause us not to block, below:
	 */
	assert_wait(SEM_HASH(object_name, offset, address, usem), TRUE);

	/* If someone unlocked the semaphore before we set the msem_wanted
	 * field, they will never do a wakeup.  Or if they did a wakeup
	 * on this semaphore before we got to the assert_wait then we also
	 * must avoid blocking, since we missed the wakeup call.  In the
	 * latter case, they also cleared the msem_wanted field, so we can
	 * check for this:
	 */
	if (my_thread->uu_reply_msg == NULL)	/* Hack to make copyin use */
						/* vm_read instead of bcopy: */
		my_thread->uu_reply_msg = (mach_msg_header_t *)0x100;
        if (copyin((caddr_t) usem, (caddr_t) &sem, sizeof(sem)))
                return(KERN_INVALID_ADDRESS);
	if (my_thread->uu_reply_msg == (mach_msg_header_t *)0x100)
		my_thread->uu_reply_msg = NULL;
        if (sem.msem_state != 1 || sem.msem_wanted != 1) {
		/* DEBUG:
		 * printf(
		 *     "MSLEEP: not sleeping on channel %x, already awakened\n",
		 *	  SEM_HASH(object_name, offset, address, usem));
		 */
                clear_wait(current_thread(), THREAD_AWAKENED, FALSE);
                return(KERN_SUCCESS);
        }

	/* DEBUG:
	 * printf("MSLEEP: sleeping on channel %x\n",
	 *	  SEM_HASH(object_name, offset, address, usem));
	 */

	/* Wait for the semaphore to (possibly) free up. */
	thread_block();

	if (current_thread()->uu_wait_result != THREAD_AWAKENED)
		return(THREAD_INTERRUPTED);

	return(KERN_SUCCESS);
}


/*
 * Called to wait for a time when the semaphore might be free.  This routine
 * locks a global semaphore lock, sets the wanted flag, checks the state of
 * the semaphore and if it is not free sleeps.
 */
kern_return_t vm_mwakeup(usem)
msemaphore *usem;
{
	task_t		my_task = u.uu_procp->p_task;
				/* OUT params of vm_region: */
	vm_address_t	address = (vm_address_t) usem;
	vm_size_t	size;
	vm_prot_t	protection, max_protection;
	vm_inherit_t	inheritance;
	boolean_t	shared;
	memory_object_name_t
			object_name;
	vm_offset_t 	offset;

	kern_return_t	result;
	
	/* Find the port name for the memory_object containing the semaphore,
	 * and the offset of the semaphore within the memory object:
	 */
	if ((result = vm_region(my_task, &address, &size,
				&protection, &max_protection, &inheritance,
				&shared, &object_name, &offset))
	    != KERN_SUCCESS)
	{
		/* DEBUG:
		 * printf("MWAKEUP: vm_region failed (%d)\n", result);
		 */
		return(result);
	}
	/* If it found a region at a higher address, this address is not in any
	 * region:
	 */
	if (address > (vm_address_t) usem) {
		/* DEBUG:
		 * printf("MWAKEUP: invalid user semaphore address (%x)\n",
		 * 	  address);
		 */
		return KERN_INVALID_ADDRESS;
	}

	/* DEBUG:
	 * printf("MWAKEUP: wakeup on channel %x\n",
	 *	  SEM_HASH(object_name, offset, address, usem));
	 */
		thread_wakeup(SEM_HASH(object_name, offset, address, usem));

	return(KERN_SUCCESS);
}
