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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Paging flow control (NORMA_VM).  Added pager_modwc to
 * the object structure to track objects requiring
 * memory_object_data_write_completed messages.  Added
 * completions_pending to track the number of outstanding
 * completions.  Added VM_OBJECT_EVENT_COMPLETION to
 * synchronize pending completion messages.  [alanl, sjs]
 * $Log: vm_object.h,v $
 * Revision 1.5  1994/11/18  21:04:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:02:01  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:56:10  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:21:16  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:59:52  SSD
 * pager flow control fixes.
 *
 * Revision 1.2  1992/11/25  01:20:15  robert
 * 	integrate changes below for norma_14
 *
 * 	David Bernard (bernard) at Open Software Foundation
 * 	vm_object_shadow() now returns indication of whether shadow was
 * 	indeed made.
 * 	[1992/11/13  19:44:35  robert]
 *
 * Revision 1.1  1992/09/30  02:37:24  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.9.9.3  92/03/03  16:27:13  jeffreyh
 * 	Add shadowed bit to vm_object structure.
 * 	[92/02/19  14:28:59  dlb]
 * 
 * 	Add use_shared_copy bit to vm_object.  Remove
 * 	new temporary object copy strategies.
 * 	[92/01/07  11:16:53  dlb]
 * 
 * 	Add definitions for temporary object copy strategies.
 * 	[92/01/06  16:26:07  dlb]
 * 	Changes from TRUNK
 * 	[92/02/26  12:35:56  jeffreyh]
 * 
 * Revision 2.10  92/01/14  16:48:24  rpd
 * 	Added vm_object_bootstrap, vm_object_lookup_name.
 * 	[91/12/31            rpd]
 * 
 * Revision 2.9.9.2  92/02/21  11:29:04  jsb
 * 	Unswapped incorrectly swapped NORMA_VM and non-NORMA_VM cases.
 * 	[92/02/10  09:31:40  jsb]
 * 
 * 	Added declaration of pager_request_t, which is an xmm_obj_t in NORMA_VM
 * 	systems, ipc_port_t otherwise. Removed pager_mobj field.
 * 	[92/02/10  08:49:16  jsb]
 * 
 * 	NORMA_VM: add pager_mobj to vm_object structure.
 * 	[92/02/09  14:46:08  jsb]
 * 
 * Revision 2.9.9.1  92/02/18  19:26:04  jeffreyh
 * 	Fixed references to  LockHolder when VM_OBJECT_DEBUG is true
 * 	[91/09/09            bernadat]
 * 
 * Revision 2.9  91/08/28  11:18:50  jsb
 * 	single_use --> use_old_pageout in vm_object structure.
 * 	[91/07/03  14:18:59  dlb]
 * 
 * Revision 2.8  91/06/25  10:34:39  rpd
 * 	Changed mach_port_t to ipc_port_t where appropriate.
 * 	[91/05/28            rpd]
 * 
 * Revision 2.7  91/05/14  17:50:38  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/02/05  17:59:42  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:33:56  mrt]
 * 
 * Revision 2.5  90/11/05  14:34:48  rpd
 * 	Added vm_object_lock_taken.
 * 	[90/11/04            rpd]
 * 
 * Revision 2.4  90/06/02  15:11:47  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  23:17:24  rpd]
 * 
 * Revision 2.3  90/02/22  20:06:39  dbg
 * 	Changed declarations of vm_object_copy routines.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.2  90/01/11  11:48:13  dbg
 * 	Pick up some changes from mainline:
 * 		Fix vm_object_absent_assert_wait to use vm_object_assert_wait
 * 		instead of vm_object_wait.  Also expanded object lock macros
 * 		under VM_OBJECT_DEBUG to check LockHolder.
 * 		[89/12/21            dlb]
 * 
 * 		Add lock_in_progress, lock_restart fields;
 * 		VM_OBJECT_EVENT_LOCK_IN_PROGRESS event.
 * 		Remove vm_object_request_port().
 * 		[89/08/07            mwyoung]
 * 	 
 * 		Removed object_list.
 * 		[89/08/31  19:45:22  rpd]
 * 	 
 * 		Optimizations from NeXT:  changed ref_count and
 * 		resident_page_count
 * 		to be shorts.  Put LockHolder under VM_OBJECT_DEBUG.  Also,
 * 		added last_alloc field for the "deactivate-behind" optimization.
 * 		[89/08/19  23:48:21  rpd]
 * 
 * 	Coalesced some bit fields (pager_created, pager_initialized,
 * 	pager_ready) into same longword with most others.
 * 	Changes for MACH_KERNEL:
 * 	. Export vm_object_copy_slowly.
 * 	. Remove non-XP definitions.
 * 	[89/04/28            dbg]
 * 
 * Revision 2.1  89/08/03  16:45:44  rwd
 * Created.
 * 
 * Revision 2.12  89/04/18  21:26:50  mwyoung
 * 	Recent history:
 * 		Improved event mechanism.
 * 		Added absent_count, to detect/prevent memory overcommitment.
 * 
 * 	All other history has been incorporated into the documentation
 * 	in this module.  Also, see the history for the implementation
 * 	file ("vm/vm_object.c").
 * 	[89/04/18            mwyoung]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
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
 */
/*
 *	File:	vm_object.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Virtual memory object module definitions.
 */

#ifndef	_VM_VM_OBJECT_H_
#define _VM_VM_OBJECT_H_

#include <mach_pagemap.h>
#include <norma_vm.h>

#include <mach/kern_return.h>
#include <mach/boolean.h>
#include <mach/memory_object.h>
#include <mach/port.h>
#include <mach/machine/vm_types.h>
#include <kern/queue.h>
#include <kern/lock.h>
#include <kern/assert.h>
#include <kern/macro_help.h>

#if	MACH_PAGEMAP
#include <vm/vm_external.h>
#endif	MACH_PAGEMAP

#if	NORMA_VM
typedef struct xmm_obj *	pager_request_t;
#else	NORMA_VM
typedef struct ipc_port *	pager_request_t;
#endif	NORMA_VM
#define	PAGER_REQUEST_NULL	((pager_request_t) 0)

/*
 *	Types defined:
 *
 *	vm_object_t		Virtual memory object.
 *
 *	We use "struct ipc_port *" instead of "ipc_port_t"
 *	to avoid include file circularities.
 */

struct vm_object {
	queue_chain_t		memq;		/* Resident memory */
	decl_simple_lock_data(,	Lock)		/* Synchronization */
#if	VM_OBJECT_DEBUG
	int			LockHolder;	/* Thread holding Lock */
#endif	VM_OBJECT_DEBUG

	vm_size_t		size;		/* Object size (only valid
						 * if internal)
						 */

	short			ref_count;	/* Number of references */
	short			resident_page_count;
						/* number of resident pages */

	struct vm_object	*copy;		/* Object that should receive
						 * a copy of my changed pages
						 */
	struct vm_object	*shadow;	/* My shadow */
	vm_offset_t		shadow_offset;	/* Offset into shadow */

	struct ipc_port		*pager;		/* Where to get data */
	vm_offset_t		paging_offset;	/* Offset into memory object */
	pager_request_t		pager_request;	/* Where data comes back */
	struct ipc_port		*pager_name;	/* How to identify region */

	memory_object_copy_strategy_t
				copy_strategy;	/* How to handle data copy */

	unsigned int
				absent_count;	/* The number of pages that
						 * have been requested but
						 * not filled.  That is, the
						 * number of pages for which
						 * the "absent" attribute is
						 * asserted.
						 */

	unsigned int /* boolean_t array */
				all_wanted;	/* Bit array of "want to be
						 * awakened" notations.  See
						 * VM_OBJECT_EVENT_* items
						 * below
						 */

	unsigned int
				paging_in_progress:16,
						/* The memory object ports are
						 * being used (e.g., for pagein
						 * or pageout) -- don't change any
						 * of these fields (i.e., don't
						 * collapse, destroy or terminate)
						 */
	/* boolean_t */		pager_created:1,/* Has pager ever been created? */
	/* boolean_t */		pager_initialized:1,/* Are fields ready to use? */
	/* boolean_t */		pager_ready:1,	/* Will manager take requests? */

#if	NORMA_VM
	/* boolean_t */		pager_modwc:1,	/* pager uses write_completed */
#endif
	/* boolean_t */		can_persist:1,	/* The kernel may keep the data
						 * for this object (and rights to
						 * the memory object) after all
						 * address map references are
						 * deallocated?
						 */
	/* boolean_t */		internal:1,	/* Created by the kernel (and
						 * therefore, managed by the
						 * default memory manger)
						 */
	/* boolean_t */		temporary:1,	/* Permanent objects may be changed
						 * externally by the memory manager,
						 * and changes made in memory must
						 * be reflected back to the memory
						 * manager.  Temporary objects lack
						 * both of these characteristics.
						 */
	/* boolean_t */		alive:1,	/* Not yet terminated (debug) */
	/* boolean_t */		lock_in_progress : 1,
						/* Is a multi-page lock
						 * request in progress?
						 */
	/* boolean_t */		lock_restart : 1,
						/* Should lock request in
						 * progress restart search?
						 */
	/* boolean_t */		use_old_pageout : 1,
						/* Use old pageout primitives? 
						 */
	/* boolean_t */		use_shared_copy : 1,/* Use shared (i.e.,
						 * delayed) copy on write */
	/* boolean_t */		shadowed: 1;	/* Shadow may exist */

	queue_chain_t		cached_list;	/* Attachment point for the list
						 * of objects cached as a result
						 * of their can_persist value
						 */
	vm_offset_t		last_alloc;	/* last allocation offset */
#if	MACH_PAGEMAP
	vm_external_t		existence_info;
#endif	MACH_PAGEMAP
#if	NORMA_VM
	int			completions_pending;
#endif	NORMA_VM
};

typedef struct vm_object	*vm_object_t;
#define VM_OBJECT_NULL		((vm_object_t) 0)

extern
vm_object_t	kernel_object;		/* the single kernel object */

/*
 *	Declare procedures that operate on VM objects.
 */

extern void		vm_object_bootstrap();
extern void		vm_object_init();
extern void		vm_object_terminate();
extern vm_object_t	vm_object_allocate();
extern void		vm_object_reference();
extern void		vm_object_deallocate();
extern void		vm_object_pmap_protect();
extern void		vm_object_pmap_remove();
extern void		vm_object_page_remove();
extern boolean_t	vm_object_shadow();
extern void		vm_object_collapse();
extern vm_object_t	vm_object_lookup();
extern vm_object_t	vm_object_lookup_name();
extern struct ipc_port	*vm_object_name();
extern void		vm_object_remove();

extern boolean_t	vm_object_copy_temporary();
extern kern_return_t	vm_object_copy_strategically();
extern kern_return_t	vm_object_copy_slowly();

extern vm_object_t	vm_object_enter();
extern void		vm_object_pager_create();
extern void		vm_object_destroy();

extern void		vm_object_print();

extern vm_object_t	vm_object_request_object();

/*
 *	Event waiting handling
 */

#define	VM_OBJECT_EVENT_INITIALIZED		0
#define	VM_OBJECT_EVENT_PAGER_READY		1
#define	VM_OBJECT_EVENT_PAGING_IN_PROGRESS	2
#define	VM_OBJECT_EVENT_ABSENT_COUNT		3
#define	VM_OBJECT_EVENT_LOCK_IN_PROGRESS	4
#define	VM_OBJECT_EVENT_COMPLETION		5

#define	vm_object_wait(object, event, interruptible)			\
	MACRO_BEGIN							\
	(object)->all_wanted |= 1 << (event);				\
	vm_object_sleep(((int) object) + (event),			\
			(object),					\
			(interruptible));				\
	MACRO_END

#define	vm_object_assert_wait(object, event, interruptible)		\
	MACRO_BEGIN							\
	(object)->all_wanted |= 1 << (event);				\
	assert_wait(((int) object) + (event), (interruptible));		\
	MACRO_END

#define	vm_object_wakeup(object, event)					\
	MACRO_BEGIN							\
	if ((object)->all_wanted & (1 << (event)))			\
		thread_wakeup(((int) object) + (event));		\
	(object)->all_wanted &= ~(1 << (event));			\
	MACRO_END

/*
 *	Routines implemented as macros
 */

#define		vm_object_paging_begin(object) 				\
	(object)->paging_in_progress++

#define		vm_object_paging_end(object) 				\
	MACRO_BEGIN							\
	assert((object)->paging_in_progress != 0);			\
	if (--(object)->paging_in_progress == 0) {			\
		vm_object_wakeup(object,				\
			VM_OBJECT_EVENT_PAGING_IN_PROGRESS);		\
	}								\
	MACRO_END

#define		vm_object_paging_wait(object, interruptible)		\
	MACRO_BEGIN							\
	while ((object)->paging_in_progress != 0) {			\
		vm_object_wait(	(object),				\
				VM_OBJECT_EVENT_PAGING_IN_PROGRESS,	\
				(interruptible));			\
		vm_object_lock(object);					\
									\
		/*XXX if ((interruptible) &&	*/			\
		    /*XXX (current_thread()->wait_result != THREAD_AWAKENED))*/ \
			/*XXX break; */					\
	}								\
	MACRO_END

#define	vm_object_absent_assert_wait(object, interruptible)		\
	MACRO_BEGIN							\
	vm_object_assert_wait(	(object),				\
			VM_OBJECT_EVENT_ABSENT_COUNT,			\
			(interruptible));				\
	MACRO_END


#define	vm_object_absent_release(object)				\
	MACRO_BEGIN							\
	(object)->absent_count--;					\
	vm_object_wakeup((object),					\
			 VM_OBJECT_EVENT_ABSENT_COUNT);			\
	MACRO_END

/*
 *	Object locking macros (with and without debugging)
 */

#if	VM_OBJECT_DEBUG
#define vm_object_lock_init(object) \
MACRO_BEGIN \
	simple_lock_init(&(object)->Lock); \
	(object)->LockHolder = 0; \
MACRO_END
#define vm_object_lock(object) \
MACRO_BEGIN \
	simple_lock(&(object)->Lock); \
	(object)->LockHolder = (int) current_thread(); \
MACRO_END
#define vm_object_unlock(object) \
MACRO_BEGIN \
	if ((object)->LockHolder != (int) current_thread()) \
	    panic("vm_object_unlock 0x%x", (object)); \
	(object)->LockHolder = 0; \
	simple_unlock(&(object)->Lock); \
MACRO_END
#define vm_object_lock_try(object) \
	(simple_lock_try(&(object)->Lock) \
	    ? ( ((object)->LockHolder = (int) current_thread()) , TRUE) \
	    : FALSE)
#define vm_object_sleep(event, object, interruptible) \
MACRO_BEGIN \
	if ((object)->LockHolder != (int) current_thread()) \
	    panic("vm_object_sleep 0x%x", (object)); \
	(object)->LockHolder = 0; \
	thread_sleep((event), simple_lock_addr((object)->Lock), \
		(interruptible)); \
MACRO_END
#define	vm_object_lock_taken(object)	\
		((object)->LockHolder == (int) current_thread())
#else	VM_OBJECT_DEBUG
#define vm_object_lock_init(object)	simple_lock_init(&(object)->Lock)
#define vm_object_lock(object)		simple_lock(&(object)->Lock)
#define vm_object_unlock(object)	simple_unlock(&(object)->Lock)
#define vm_object_lock_try(object)	simple_lock_try(&(object)->Lock)
#define vm_object_sleep(event, object, interruptible)			\
		thread_sleep((event), simple_lock_addr((object)->Lock), \
			     (interruptible))
#define	vm_object_lock_taken(object)	simple_lock_taken(&(object)->Lock)
#endif	VM_OBJECT_DEBUG

#endif	_VM_VM_OBJECT_H_
