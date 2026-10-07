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
 * HISTORY
 * $Log: vm_user.c,v $
 * Revision 1.8  1995/03/17  22:28:29  terry
 * This fixes bug number 12643.  The main problem was that many unused pages
 * in an object were not being freed up.
 *
 *  Reviewer: cfleck,stans,andyp
 *  Risk: Medium
 *  Benefit or PTS #: 12643
 *  Testing: testcase, arlin has used it and stan has used it to run
 *           code on 50 nodes that never ran on > 16 nodes. Also have
 *           run the parallel sats on 58 nodes.
 *  Module(s): vm_map.c vm_user.c norma2_init.c recv_engine.c
 *
 * Revision 1.7  1994/11/18  21:04:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/09/09  17:53:15  andyp
 * PTS #: 10760, 10618, 10660
 * Mandatory?:  10760==yes, others==no.
 * Description: The vnode pager needed to have access to a separate pool
 * 	of RDMA handles.  RPC/RDMA handle allocation has been
 * 	modified to preserve allocation request order.
 * 	Corrected a typo in kern/ast.h.
 * 	Reduced the overhead for page thawing (and removed old code).
 * 	Set ice_cube_pages=2 by default.
 * Reviewer(s): rkl
 * Risk:	     Medium
 * Testing:     byte sats, test case for 10660
 * Module(s):
 * 	M intel/pmap.c
 * 	M kern/ast.h
 * 	M norma2/dipc_client.h
 * 	M norma2/dipc_mqueue.c
 * 	M norma2/dipc_mqueue.h
 * 	M norma2/dipc_server.c
 * 	M norma2/dipc_server.h
 * 	M norma2/norma2_init.c
 * 	M norma2/norma_transport.h
 * 	M norma2/recv_engine.c
 * 	M rpc_rdma/rdma.c
 * 	M rpc_rdma/rpc.c
 * 	M vm/vm_user.c
 *
 * Revision 1.5  1994/07/12  19:27:10  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.4.8.12  1994/06/30  23:46:48  stans
 * get all of pmap_copy_noise
 *
 * Revision 1.4.8.11  1994/06/30  23:43:05  stans
 *   Remove old debug 'pmap_copy_noise' code.
 *
 * Revision 1.4.8.10  1994/06/29  23:30:58  andyp
 * Added a special purpose pmap_enter_frozen() that is a simplified
 * version of pmap_enter() -- is also freezes the page and marks it
 * referenced and modified.  Removed the old freezing routine.
 * Got a nice bandwidth boost, too.
 *
 * Revision 1.4.8.9  1994/06/27  23:08:03  andyp
 * Frozen pages should not only be marked dirty, but referenced too.
 *
 * Revision 1.4.8.8  1994/05/04  20:06:55  rkl
 *  Fixed locking bug when copying OOL data into the NORMA map
 *
 * Revision 1.4.8.7  1994/04/26  15:55:08  stans
 * removed early dipc VM debug noise
 *
 * Revision 1.4.8.6  1994/02/25  21:49:58  andyp
 * Don't declare "offset" to be a register and later take its address.
 *
 * Revision 1.4.8.5  1994/02/16  21:44:36  stans
 * fixed typo
 *
 * Revision 1.4.8.4  1994/02/16  19:25:52  stans
 * fast freeze code enabled
 *
 * Revision 1.4.8.3  1994/02/11  17:44:08  andyp
 * pmap_page_is_present() now returns a boolean.
 *
 * Revision 1.4.8.2  1994/02/07  16:49:03  rkl
 *  vm_page_freeze_range() makes no_zero_fill TRUE during vm_fault() calls.
 *
 * Revision 1.4.8.1  1994/01/26  02:19:49  stans
 *   VM page freeze/thaw support.
 *
 * Revision 1.4  1993/06/30  23:02:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:56:24  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:02:19  dleslie
 * First R1_0 release
 *
 * Revision 2.18.3.2  92/03/03  16:27:29  jeffreyh
 * 	Eliminate keep_wired argument from vm_map_copyin().
 * 	[92/02/21  10:16:59  dlb]
 * 	Changes from TRUNK
 * 	[92/02/26  12:36:17  jeffreyh]
 * 
 * Revision 2.18.3.1.2.1  92/02/25  17:00:16  jeffreyh
 * 	[David L. Black 92/02/22  17:07:18  dlb@osf.org]
 * 		Add checks for protection and inheritance arguments.
 * 
 * Revision 2.18.3.1  92/02/21  11:29:08  jsb
 * 	Removed NORMA_VM hackery. Hip hip hooray.
 * 	[92/02/11  17:42:41  jsb]
 * 
 * Revision 2.18  91/12/11  08:44:21  jsb
 * 	Fixed vm_write and vm_copy to check for a null map.
 * 	Fixed vm_write and vm_copy to not check for misalignment.
 * 	Fixed vm_copy to discard the copy if the overwrite fails.
 * 	[91/12/09            rpd]
 * 
 * Revision 2.17  91/12/10  13:27:17  jsb
 * 	Apply temporary NORMA_VM workaround to XMM problem.
 * 	This leaks objects if vm_map() fails.
 * 	[91/12/10  12:55:27  jsb]
 * 
 * Revision 2.16  91/08/28  11:19:07  jsb
 * 	Fixed vm_map to check memory_object with IP_VALID.
 * 	Changed vm_wire to use KERN_INVALID_{HOST,TASK,VALUE}
 * 	instead of a generic KERN_INVALID_ARGUMENT return code.
 * 	[91/07/12            rpd]
 * 
 * Revision 2.15  91/07/31  18:22:40  dbg
 * 	Change vm_pageable to vm_wire.  Require host_priv port to gain
 * 	wiring privileges.
 * 	[91/07/30  17:28:22  dbg]
 * 
 * Revision 2.14  91/05/14  17:51:35  mrt
 * 	Correcting copyright
 * 
 * Revision 2.13  91/03/16  15:07:13  rpd
 * 	Removed temporary extra stats.
 * 	[91/02/10            rpd]
 * 
 * Revision 2.12  91/02/05  18:00:35  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:35:00  mrt]
 * 
 * Revision 2.11  90/08/06  15:08:59  rwd
 * 	Vm_read should check that the map is non null.
 * 	[90/07/26            rwd]
 * 
 * Revision 2.10  90/06/02  15:12:07  rpd
 * 	Moved trap versions of syscalls to kern/ipc_mig.c.
 * 	Removed syscall_vm_allocate_with_pager.
 * 	[90/05/31            rpd]
 * 
 * 	Purged vm_allocate_with_pager.
 * 	[90/04/09            rpd]
 * 	Purged MACH_XP_FPD.  Use vm_map_pageable_user for vm_pageable.
 * 	Converted to new IPC kernel call semantics.
 * 	[90/03/26  23:21:55  rpd]
 * 
 * Revision 2.9  90/05/29  18:39:57  rwd
 * 	New trap versions of exported vm calls from rfr.
 * 	[90/04/20            rwd]
 * 
 * Revision 2.8  90/05/03  15:53:30  dbg
 * 	Set current protection to VM_PROT_DEFAULT in
 * 	vm_allocate_with_pager.
 * 	[90/04/12            dbg]
 * 
 * Revision 2.7  90/03/14  21:11:49  rwd
 * 	Get rfr bug fix.
 * 	[90/03/07            rwd]
 * 
 * Revision 2.6  90/02/22  20:07:02  dbg
 * 	Use new vm_object_copy routines.  Use new vm_map_copy
 * 	technology.  vm_read() no longer requires page alignment.
 * 	Change PAGE_WAKEUP to PAGE_WAKEUP_DONE to reflect the fact
 * 	that it clears the busy flag.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.5  90/01/24  14:08:30  af
 * 	Fixed bug in optimized vm_write: now that we relaxed the restriction
 * 	on the page-alignment of the size arg we must be able to cope with
 * 	e.g. one-and-a-half pages as well.
 * 	Also, by simple measures on my pmax turns out that mapping is a win
 * 	versus copyin even for a single page. IF you can map.
 * 	[90/01/24  11:37:35  af]
 * 
 * Revision 2.4  90/01/22  23:09:42  af
 * 	Go through the map module for machine attributes.
 * 	[90/01/20  17:23:35  af]
 * 
 * 	Added vm_machine_attribute(), which only invokes the
 * 	corresponding pmap operation, for now.  Just a first
 * 	shot at it, lacks proper locking and keeping the info
 * 	around, someplace.
 * 	[89/12/08            af]
 * 
 * Revision 2.3  90/01/19  14:36:22  rwd
 * 	Disable vm_write optimization on mips since it doesn't appear to
 * 	work.
 * 	[90/01/19            rwd]
 * 
 * 	Get version that works on multiprocessor from rfr
 * 	[90/01/10            rwd]
 * 	Get new user copyout code from rfr.
 * 	[90/01/05            rwd]
 * 
 * Revision 2.2  89/09/08  11:29:05  dbg
 * 	Pass keep_wired parameter to vm_map_move.
 * 	[89/07/14            dbg]
 * 
 * 28-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Changes for MACH_KERNEL:
 *	. Removed non-MACH include files and all conditionals.
 *	. Added vm_pageable, for privileged tasks only.
 *	. vm_read now uses vm_map_move to consolidate map operations.
 *	. If using FAST_PAGER_DATA, vm_write expects data to be in
 *	  current task's address space.
 *
 * Revision 2.12  89/04/18  21:30:56  mwyoung
 * 	All relevant history has been integrated into the documentation below.
 * 
 */
/*
 *	File:	vm/vm_user.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 * 
 *	User-exported virtual memory functions.
 */

#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/mach_types.h>	/* to get vm_address_t */
#include <mach/memory_object.h>
#include <mach/std_types.h>	/* to get pointer_t */
#include <mach/vm_attributes.h>
#include <mach/vm_param.h>
#include <mach/vm_statistics.h>
#include <kern/host.h>
#include <kern/task.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>

vm_statistics_data_t	vm_stat;

/*
 *	vm_allocate allocates "zero fill" memory in the specfied
 *	map.
 */
kern_return_t vm_allocate(map, addr, size, anywhere)
	register vm_map_t	map;
	register vm_offset_t	*addr;
	register vm_size_t	size;
	boolean_t		anywhere;
{
	kern_return_t	result;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);
	if (size == 0) {
		*addr = 0;
		return(KERN_SUCCESS);
	}

	if (anywhere)
		*addr = vm_map_min(map);
	else
		*addr = trunc_page(*addr);
	size = round_page(size);

	result = vm_map_enter(
			map,
			addr,
			size,
			(vm_offset_t)0,
			anywhere,
			VM_OBJECT_NULL,
			(vm_offset_t)0,
			FALSE,
			VM_PROT_DEFAULT,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT);

	return(result);
}

/*
 *	vm_allocate_coalesce  allocates "zero fill" memory in the specfied
 *	map.  This routine is slightly different from above routine
 *  in that it optional coalescing of objects in the maps (if the
 *  objects can be coalesced.
 */
kern_return_t vm_allocate_coalesce (map, addr, size, anywhere, coalesce)
	register vm_map_t	map;
	register vm_offset_t	*addr;
	register vm_size_t	size;
	boolean_t		anywhere;
	boolean_t		coalesce;
{
	kern_return_t	result;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);
	if (size == 0) {
		*addr = 0;
		return(KERN_SUCCESS);
	}

	if (anywhere)
		*addr = vm_map_min(map);
	else
		*addr = trunc_page(*addr);
	size = round_page(size);

	result = vm_map_enter_coalesce(
			map,
			addr,
			size,
			(vm_offset_t)0,
			anywhere,
			VM_OBJECT_NULL,
			(vm_offset_t)0,
			FALSE,
			VM_PROT_DEFAULT,
			VM_PROT_ALL,
			VM_INHERIT_DEFAULT,
			coalesce);

	return(result);
}

/*
 *	vm_deallocate deallocates the specified range of addresses in the
 *	specified address map.
 */
kern_return_t vm_deallocate(map, start, size)
	register vm_map_t	map;
	vm_offset_t		start;
	vm_size_t		size;
{
	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	if (size == (vm_offset_t) 0)
		return(KERN_SUCCESS);

	return(vm_map_remove(map, trunc_page(start), round_page(start+size)));
}

/*
 *	vm_inherit sets the inheritance of the specified range in the
 *	specified map.
 */
kern_return_t vm_inherit(map, start, size, new_inheritance)
	register vm_map_t	map;
	vm_offset_t		start;
	vm_size_t		size;
	vm_inherit_t		new_inheritance;
{
	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

        switch (new_inheritance) {
        case VM_INHERIT_NONE:
        case VM_INHERIT_COPY:
        case VM_INHERIT_SHARE:
                break;
        default:
                return(KERN_INVALID_ARGUMENT);
        }

	return(vm_map_inherit(map,
			      trunc_page(start),
			      round_page(start+size),
			      new_inheritance));
}

/*
 *	vm_protect sets the protection of the specified range in the
 *	specified map.
 */

kern_return_t vm_protect(map, start, size, set_maximum, new_protection)
	register vm_map_t	map;
	vm_offset_t		start;
	vm_size_t		size;
	boolean_t		set_maximum;
	vm_prot_t		new_protection;
{
	if ((map == VM_MAP_NULL) || (new_protection & ~VM_PROT_ALL))
		return(KERN_INVALID_ARGUMENT);

	return(vm_map_protect(map,
			      trunc_page(start),
			      round_page(start+size),
			      new_protection,
			      set_maximum));
}

kern_return_t vm_statistics(map, stat)
	vm_map_t	map;
	vm_statistics_data_t	*stat;
{
	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	*stat = vm_stat;

	stat->pagesize = PAGE_SIZE;
	stat->free_count = vm_page_free_count;
	stat->active_count = vm_page_active_count;
	stat->inactive_count = vm_page_inactive_count;
	stat->wire_count = vm_page_wire_count;

	return(KERN_SUCCESS);
}

/*
 * Handle machine-specific attributes for a mapping, such
 * as cachability, migrability, etc.
 */
kern_return_t vm_machine_attribute(map, address, size, attribute, value)
	vm_map_t	map;
	vm_address_t	address;
	vm_size_t	size;
	vm_machine_attribute_t	attribute;
	vm_machine_attribute_val_t* value;		/* IN/OUT */
{
	extern kern_return_t	vm_map_machine_attribute();

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	return vm_map_machine_attribute(map, address, size, attribute, value);
}

kern_return_t vm_read(map, address, size, data, data_size)
	vm_map_t	map;
	vm_address_t	address;
	vm_size_t	size;
	pointer_t	*data;
	vm_size_t	*data_size;
{
	kern_return_t	error;
	vm_map_copy_t	ipc_address;

	if (map == VM_MAP_NULL)
		return(KERN_INVALID_ARGUMENT);

	if ((error = vm_map_copyin(map,
				address,
				size,
				FALSE,	/* src_destroy */
				&ipc_address)) == KERN_SUCCESS) {
		*data = (pointer_t) ipc_address;
		*data_size = size;
	}
	return(error);
}

kern_return_t vm_write(map, address, data, size)
	vm_map_t	map;
	vm_address_t	address;
	pointer_t	data;
	vm_size_t	size;
{
	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	return vm_map_copy_overwrite(map, address, (vm_map_copy_t) data,
				     FALSE /* interruptible XXX */);
}

kern_return_t vm_copy(map, source_address, size, dest_address)
	vm_map_t	map;
	vm_address_t	source_address;
	vm_size_t	size;
	vm_address_t	dest_address;
{
	vm_map_copy_t copy;
	kern_return_t kr;

	if (map == VM_MAP_NULL)
		return KERN_INVALID_ARGUMENT;

	kr = vm_map_copyin(map, source_address, size,
			   FALSE, &copy);
	if (kr != KERN_SUCCESS)
		return kr;

	kr = vm_map_copy_overwrite(map, dest_address, copy,
				   FALSE /* interruptible XXX */);
	if (kr != KERN_SUCCESS) {
		vm_map_copy_discard(copy);
		return kr;
	}

	return KERN_SUCCESS;
}

/*
 *	Routine:	vm_map
 */
kern_return_t vm_map(
		target_map,
		address, size, mask, anywhere,
		memory_object, offset,
		copy,
		cur_protection, max_protection,	inheritance)
	vm_map_t	target_map;
	vm_offset_t	*address;
	vm_size_t	size;
	vm_offset_t	mask;
	boolean_t	anywhere;
	ipc_port_t	memory_object;
	vm_offset_t	offset;
	boolean_t	copy;
	vm_prot_t	cur_protection;
	vm_prot_t	max_protection;
	vm_inherit_t	inheritance;
{
	register
	vm_object_t	object;
	register
	kern_return_t	result;

	if ((target_map == VM_MAP_NULL) ||
	    (cur_protection & ~VM_PROT_ALL) ||
	    (max_protection & ~VM_PROT_ALL))
		return(KERN_INVALID_ARGUMENT);

        switch (inheritance) {
        case VM_INHERIT_NONE:
        case VM_INHERIT_COPY:
        case VM_INHERIT_SHARE:
                break;
        default:
                return(KERN_INVALID_ARGUMENT);
        }

	*address = trunc_page(*address);
	size = round_page(size);

	if (!IP_VALID(memory_object)) {
		object = VM_OBJECT_NULL;
		offset = 0;
		copy = FALSE;
	} else if ((object = vm_object_enter(memory_object, size, FALSE))
			== VM_OBJECT_NULL)
		return(KERN_INVALID_ARGUMENT);

	/*
	 *	Perform the copy if requested
	 */

	if (copy) {
		vm_object_t	new_object;
		vm_offset_t	new_offset;

		result = vm_object_copy_strategically(object, offset, size,
				&new_object, &new_offset,
				&copy);

		/*
		 *	Throw away the reference to the
		 *	original object, as it won't be mapped.
		 */

		vm_object_deallocate(object);

		if (result != KERN_SUCCESS)
			return (result);

		object = new_object;
		offset = new_offset;
	}

	if ((result = vm_map_enter(target_map,
				address, size, mask, anywhere,
				object, offset,
				copy,
				cur_protection, max_protection, inheritance
				)) != KERN_SUCCESS)
		vm_object_deallocate(object);
	return(result);
}

/*
 *	Specify that the range of the virtual address space
 *	of the target task must not cause page faults for
 *	the indicated accesses.
 *
 *	[ To unwire the pages, specify VM_PROT_NONE. ]
 */
kern_return_t vm_wire(host, map, start, size, access)
	host_t			host;
	register vm_map_t	map;
	vm_offset_t		start;
	vm_size_t		size;
	vm_prot_t		access;
{
	if (host == HOST_NULL)
		return KERN_INVALID_HOST;

	if (map == VM_MAP_NULL)
		return KERN_INVALID_TASK;

	if (access & ~VM_PROT_ALL)
		return KERN_INVALID_ARGUMENT;

	return vm_map_pageable_user(map,
				    trunc_page(start),
				    round_page(start+size),
				    access);
}

#if	NORMA2

/*
 * Copy a range of VA's from a source pmap to the destination pmap.
 * It's assumed the VM house-keeping (vm_copyout() or suc has been performed).
 * Virtual addresses are copy by copying those hardware pte's which are
 * currently in the source pmap.
 *
 * inputs:
 *	src_map		source VM map ptr.
 *	src_va		starting VA in the source map.
 *	end_va		ending (inclusive) VA in source map.
 *	dst_map		destination VM map ptr.
 *	dst_va		starting VA in the destination map.
 *
 */

unsigned long vm_map_pmap_copy_range_lock_misses;
unsigned long vm_map_pmap_copy_range_lock_hits;

vm_map_pmap_copy_range( src_map, src_va, end_va, dst_map, dst_addr )
	vm_map_t	src_map;
	vm_offset_t	src_va;
	vm_offset_t	end_va;
	vm_map_t	dst_map;
	vm_offset_t	dst_addr;
{
	vm_offset_t	start, end, dst;
	vm_size_t	npages, misses;
	pmap_t		src_pmap, dst_pmap;

	dst = trunc_page(dst_addr);
	start = trunc_page(src_va);
	end = round_page(end_va);
	npages = (end - start) / PAGE_SIZE;

	assert( npages > 0 );

	src_pmap = vm_map_pmap(src_map);
	dst_pmap = vm_map_pmap(dst_map);

#define	vm_map_lock_try_read( map )	lock_try_read(&(map)->lock)

	if (vm_map_lock_try_read( src_map ) == FALSE) {
		vm_map_pmap_copy_range_lock_misses++;
		return;
	}

	vm_map_pmap_copy_range_lock_hits++;

	/*vm_map_lock_read( src_map );*/
	vm_map_lock( dst_map );

	misses = pmap_copy_range( src_pmap, start, npages, dst_pmap, dst );

	vm_map_unlock( dst_map );
	vm_map_unlock_read( src_map );
}


/*
 * freeze specified range of VM pages.
 *
 * implicit inputs:
 *	range of VM pages have never been accessed (i.e., NO physical pages
 *	allocated). Freshly vm_allocate()'ed.
 *
 * inputs:
 *	map		vm map pointer
 *	start		start VM page address
 *	pageCount	# of pages.
 *
 * outputs:
 *	KERN_SUCCESS == success
 *	otherwise kern_return_t error code.
 */

kern_return_t
vm_page_freeze_range( map, start, pageCount )
	vm_map_t	map;
	vm_address_t	start;
	vm_size_t	pageCount;
{
	register vm_offset_t	va;
	vm_offset_t		offset;
	pmap_t			pmap;
	kern_return_t		kr;
	vm_object_t		object;
	boolean_t		save;
	extern boolean_t	vm_page_deactivate_behind;

	if ( pageCount <= 0 )
		panic("vm_page_freeze_range() pageCount <= 0");
		/*return KERN_SUCCESS; */

	if ( (pmap = map->pmap) == PMAP_NULL)
		panic("vm_page_freeze_range() null pmap?");
		/*return KERN_INVALID_ARGUMENT; */

	va = trunc_page(start);

	/*
	 * Find the VM object to speed things along.
	 * Verify the start and end pages are in the same object.
	 */
	{
		vm_prot_t	prot;
		unsigned	version, wired;

		kr = vm_map_lookup(	&map,
					va,
					VM_PROT_WRITE,
					&version,
					&object,
					&offset,
					&prot,
					&wired );

		if ( kr != KERN_SUCCESS ) {
			printf("vm_page_freeze_range() vm_map_lookup err, %d\n",
				kr);
			panic("vm_page_freeze_range() vm_map_lookup failed.");
		}

		if ( object == VM_OBJECT_NULL )
			panic("vm_page_freeze_range() null object?");
	}

	/*
	 * Allocate VM pages from the free list, stuff them into the specified
	 * object and finally into the i860 hardware (pmap_enter).
	 */
	/*
	 * make sure the preceeding VM page stays put!!
	 * see vm/vm_resident.c, vm_page_insert()
	 */
	save = vm_page_deactivate_behind;
	vm_page_deactivate_behind = FALSE;
	do {
		register vm_page_t	m;

		/*
		 * We know the VM pages will not be present in the hardware
		 * map as they are freshly vm_allocate()'d pages so we skip
		 * the hardware check for page presence.
		 */
#if	MACH_ASSERT
		if ( pmap_page_is_present(pmap,va) ) {
			printf("0x%x va is present?\n",va);
			assert( 0 );
		}
#endif
		vm_object_lock(object);

		/*
		 * Allocate a VM page 
		 */
		while ((m = vm_page_grab()) == VM_PAGE_NULL) {
			vm_object_unlock(object);
			VM_PAGE_WAIT((void (*)()) 0);
			vm_object_lock(object);
		}

		vm_page_init(m, m->phys_addr);

		vm_object_paging_begin(object);

		/* hold pageout daemon at bay */
		vm_page_lock_queues();

		vm_page_insert( m, object, offset );

		m->busy = TRUE;	/* prevent others from bothering us */

		/*
		 * we're about to receive into this page, so it needs
		 * to look like it is referenced *and* dirty.  otherwise,
		 * after thawing, the page might get stolen away by the
		 * pageout thread without writing it out to it's pager.
		 */
		m->reference = TRUE;	/* it soon will be referenced... */
		m->dirty = TRUE;	/* ...and dirty */

		vm_page_unlock_queues();

		/* must unlock for pmap_{enter/expand}() */
		vm_object_unlock(object);

		/*
		 * Enter and freeze the physical page into the physical map
		 * hardware (pmap), setting writeable, referenced,
		 *  and modified bits.
		 */
		pmap_enter_frozen(pmap, va, m->phys_addr);

		vm_object_lock(object);

		PAGE_WAKEUP_DONE(m);	/* clear busy */

		vm_page_lock_queues();

		/*
		 * make sure the page is on the active queue.
		 */
		assert(!m->active && !m->inactive);
		vm_page_activate(m);

		vm_page_unlock_queues();
		vm_object_paging_end(object);
		vm_object_unlock(object);

		va += PAGE_SIZE;	/* next VM page */
		offset += PAGE_SIZE;

	} while( --pageCount > 0 );
	vm_page_deactivate_behind = save;

	return KERN_SUCCESS;
}

#endif	/* NORMA2 */
