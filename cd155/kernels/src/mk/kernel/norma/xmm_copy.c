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
 * SSD HISTORY
 * $Log: xmm_copy.c,v $
 * Revision 1.12  1994/11/18  20:56:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/08/31  21:25:01  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.9.2.1  1994/08/08  23:53:19  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.10  1994/08/08  19:33:28  andyp
 * PTS #:	10338, 10339
 * Mandatory?:	Yes
 * Description: Don't issue proxy_lock_completed() until all expected
 * 	proxy_data_write_completed()'s have been received.
 * 	Added XMM function entry logging to the norma log ("show norma").
 * 	Added NORMA_LOG_ONLY bootmagic to log exactly one module id.
 * 	Upped the priority of the dipc_emmi_reply_threads above
 * 	that of ordinary dipc_kobj_server_threads.
 * Reviewer(s): rkl
 * Risk:	Low (compared to getting sporadic 0's or truncated files)
 * Testing:	sats, devloper tests, test cases pass.
 * Module(s):
 * 	M intel/pmap.c
 * 	M norma/xmm.c
 * 	M norma/xmm_buffer.c
 * 	M norma/xmm_copy.c
 * 	M norma/xmm_export.c
 * 	M norma/xmm_import.c
 * 	M norma/xmm_interpose.c
 * 	M norma/xmm_invalid.c
 * 	M norma/xmm_object.c
 * 	M norma/xmm_server.c
 * 	M norma/xmm_split.c
 * 	M norma/xmm_svm.c
 * 	M norma/xmm_user.c
 * 	A norma/xmm_dipc.h
 * 	M norma2/dipc_kserver.c
 * 	M norma2/norma_log.c
 * 	M norma2/norma_log.h
 *
 * Revision 1.9  1994/07/12  19:25:04  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.8  1994/03/29  13:18:37  fritz
 * Avoid consuming too many (or even all) kthreads for faulting
 * data in. Instead, in case of overflow, queue incoming requests.
 * One of the busy kthreads will later take care of queued requests.
 * This fixes PTS #6878.
 *
 * Revision 1.7.4.2  1994/06/27  23:01:13  andyp
 * Fix for PTS 6878:
 * 	Avoid consuming too many (or even all) kthreads for faulting
 * 	data in. Instead, in case of overflow, queue incoming requests.
 * 	One of the busy kthreads will later take care of queued
 * 	requests.
 * >>>I HAVE DISABLED THIS CODE UNTIL PROVEN TO BE REQUIRED<<<
 * >>>FOR THE NEW NORMA IMPLEMENTATION.                    <<<
 *
 *	Revision 1.7.2.1  1994/04/18  07:39:14  fritz
 *	Copy from main trunk to fix PTS Bug #6878.
 *
 *	Revision 1.8  1994/03/29  13:18:37  fritz
 *	Avoid consuming too many (or even all) kthreads for faulting
 *	data in. Instead, in case of overflow, queue incoming requests.
 *	One of the busy kthreads will later take care of queued requests.
 *	This fixes PTS #6878.
 *
 * Revision 1.7.4.1  1994/02/25  22:41:49  andyp
 * Removed some lint with respect to some debugging routines.
 *
 * Revision 1.7  1993/09/28  18:05:13  andyp
 * Update for the 1.2 release.
 *
 *
 *	Added {k,m}_db_print methods.  [alanl@osf.org]
 *
 * Revision 1.6  1993/07/22  02:21:17  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:52:09  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:40:44  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:46:28  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:03  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:39:57  SSD
 * pager flow control fixes.
 *
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_copy.c,v
 * Revision 1.2.7.3  1993/04/22  19:38:56  rod
 * 	Merge with NMK13 latest.
 * 	[1993/04/22  19:06:46  rod]
 *
 * 	vm_map_copyin_page_list() takes no_zero_fill arg.
 * 	[1993/04/12  17:43:13  rod]
 *
 * 	device_read_overwrite() support:
 * 	vm_map_copyin_page_list() takes protection argument.
 * 	[1993/02/19  12:00:34  rod]
 *
 * Revision 1.2.7.2  1993/04/15  22:45:44  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed; also new parameters for
 * 	K_SET_READY.  [sjs]
 * 	[1993/04/15  22:11:25  alanl]
 * 
 * Revision 1.2  1992/11/25  01:15:53  robert
 * 	fix history
 * 	[1992/11/09  22:15:59  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:49:18  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added support for new K_RELEASE method.
 * 
 * 	Revision 1.1  1992/11/05  21:00:15  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.3.5  92/06/24  18:02:21  jeffreyh
 * 	Calling sequence changes for XMM framework cleanup.
 * 	[92/06/24            dlb]
 * 
 * 	USE_OLD_PAGEOUT_TRUE --> XMM_USE_DATA_WRITE.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.3.3.4  92/03/28  10:12:36  jeffreyh
 * 	changed data_write to data_write_return, deleted data_return method.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.3.3  92/02/21  11:25:37  jsb
 * 	Deallocate memory_object and memory_object_name ports.
 * 	[92/02/20  10:56:16  jsb]
 * 
 * 	Use newer xmm_decl macro with explicit name parameter.
 * 	[92/02/16  14:06:22  jsb]
 * 
 * 	Eliminated shadow obj creation, since we now let the vm system
 * 	handle copies for us.
 * 	[92/02/11  11:07:52  jsb]
 * 
 * 	Separate xmm_copy_create from norma_copy_create.
 * 	Replaced *_internal_memory_object garbage with call to
 * 	xmm_memory_manager_export. Use new MEMORY_OBJECT_COPY_TEMPORARY
 * 	strategy instead of MEMORY_OBJECT_COPY_DELAY, since we don't
 * 	need (or want) to see changes made to our object.
 * 	[92/02/11  11:05:04  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	Removed *_internal_memory_object routines. Removed bogus data_write
 * 	implementation. Added shadow layer creation.
 * 	[92/02/09  12:49:43  jsb]
 * 
 * 	Obtain reference to map upon creation; release on termination.
 * 	[92/01/22  10:35:58  jsb]
 * 
 * Revision 2.3.3.1  92/01/21  21:53:50  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:19:14  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:13:56  jsb]
 * 
 * Revision 2.3.1.1  92/01/15  12:13:44  jeffreyh
 * 	Deallocate memory object name port on termination. (dlb)
 * 
 * Revision 2.3  91/11/15  14:09:53  rpd
 * 	Use ipc_port_make_send in norma_copy_create for returned memory_object.
 * 	[91/09/23  09:09:25  jsb]
 * 
 * Revision 2.2  91/08/28  11:16:22  jsb
 * 	In m_copy_data_request: removed dead code, and added missing
 * 	is_continuation parameter to vm_map_copyin_page_list.
 * 	[91/08/16  14:25:04  jsb]
 * 
 * 	First checkin.
 * 	[91/08/15  13:03:06  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS 
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
 * any improvements or extensions that they make and grant Carnegie the
 * rights to redistribute these changes.
 */
/*
 */
/*
 *	File:	norma/xmm_copy.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 */

#include <norma/xmm_obj.h>
#include <norma/xmm_user_rename.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <vm/memory_object.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>

#include <norma/xmm_dipc.h>

struct mobj {
	struct xmm_obj	obj;
	vm_map_t	map;
	vm_offset_t	start;
	vm_size_t	size;
	ipc_port_t	memory_object_name;
	ipc_port_t	memory_object;
};

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_copy_copy			m_invalid_copy
#define	m_copy_data_unlock		m_invalid_data_unlock
#define	m_copy_data_write_return	m_invalid_data_write_return
#define	m_copy_lock_completed		m_invalid_lock_completed
#define	m_copy_supply_completed		m_invalid_supply_completed
#define	m_copy_change_completed		m_invalid_change_completed

#define	k_copy_data_unavailable		k_invalid_data_unavailable
#define	k_copy_get_attributes		k_invalid_get_attributes
#define	k_copy_lock_request		k_invalid_lock_request
#define	k_copy_data_error		k_invalid_data_error
#define	k_copy_set_ready		k_invalid_set_ready
#define	k_copy_destroy			k_invalid_destroy
#define	k_copy_data_supply		k_invalid_data_supply
#define	k_copy_release			k_invalid_release
#define	k_copy_data_write_completed	k_invalid_data_write_completed

extern m_copy_db_print();
extern k_copy_db_print();

xmm_decl(copy, "copy", sizeof(struct mobj));


struct data_request {
	struct data_request *next;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
};

#if	NORMA_IPC && !NORMA2
extern int kserver_thread_max;
#endif	/* NORMA_IPC && !NORMA2 */
struct data_request *data_request_queue;
decl_simple_lock_data(,data_request_queue_lock)
zone_t	data_request_zone;
int data_request_in_progress;
#if MACH_KDB
unsigned int C_data_request_overflow = 0;
#endif MACH_KDB


xmm_copy_init()
{
	xmm_entry0(xmm_copy_init);

	data_request_zone = zinit(sizeof(struct data_request), 512*1024,
					sizeof(struct data_request), FALSE,
					"xmm.copy.data_request");
	data_request_queue = (struct data_request *) 0;
        simple_lock_init(&data_request_queue_lock);
}


kern_return_t
xmm_copy_create(map, start, size, new_mobj)
	vm_map_t map;
	vm_offset_t start;
	vm_size_t size;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry4(xmm_copy_create, map, start, size, new_mobj);

	assert(page_aligned(start));
	assert(page_aligned(size));
	kr = xmm_obj_allocate(&copy_class, XMM_OBJ_NULL, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	vm_map_reference(map);
	MOBJ->map = map;
	MOBJ->start = start;
	MOBJ->size = size;
	MOBJ->memory_object_name = ipc_port_alloc_kernel();
	if (MOBJ->memory_object_name == IP_NULL) {
		panic("add_internal_memory_object: ipc_port_alloc_kernel");
	}
	MOBJ->memory_object = IP_NULL;
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

norma_copy_create(map, start, size, memory_object_p)
	vm_map_t map;
	vm_offset_t start;
	vm_size_t size;
	ipc_port_t *memory_object_p;
{
	xmm_obj_t mobj;
	kern_return_t kr;
	ipc_port_t xmm_memory_manager_export();

	xmm_entry4(norma_copy_create, map, start, size, memory_object_p);

	/*
	 * Create a read-only, xmm-internal memory manager for map.
	 */
	kr = xmm_copy_create(map, start, size, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}

	/*
	 * Create an svm stack and an xmm object, and save memory object.
	 */
	MOBJ->memory_object = xmm_memory_manager_export(mobj);

	/*
	 * Return memory object.
	 */
	*memory_object_p = MOBJ->memory_object;
	return KERN_SUCCESS;
}

m_copy_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_entry4(m_copy_init, mobj, pagesize, internal, size);

#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	return K_SET_READY(mobj, OBJECT_READY_TRUE, MAY_CACHE_FALSE,
			   TRUE, MEMORY_OBJECT_COPY_TEMPORARY, 
			   pagesize, XMM_USE_DATA_WRITE,
			   ipc_port_make_send(MOBJ->memory_object_name),
			   XMM_REPLY_NULL);
}

m_copy_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	xmm_entry2(m_copy_terminate, mobj, release);
	return KERN_SUCCESS;
}

/*
 * XXX
 * What to do, if a m_copy_deallocate() arrives while a page fault is
 * still in progess? Can this occur at all? This code assumes, no.
 */

void
m_copy_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_copy_deallocate, mobj);
	vm_map_deallocate(MOBJ->map);
	ipc_port_dealloc_kernel(MOBJ->memory_object_name);
	if (MOBJ->memory_object != IP_NULL) {
		ipc_port_dealloc_kernel(MOBJ->memory_object);
	}
}

m_copy_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	struct data_request *dr;

	xmm_entry4(m_copy_data_request, mobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	assert(page_aligned(offset));
	assert(page_aligned(length));
	assert(length == PAGE_SIZE);
	assert(offset + length <= MOBJ->size);

#if	NORMA_IPC && !NORMA2
	if (data_request_in_progress > kserver_thread_max/2) {

		/*
		 * Avoid consuming too many (or even all) kthreads
		 * for faulting data in. Instead, in case of overflow,
		 * queue incoming requests. One of the busy kthreads
		 * will later take care of queued requests (see below).
		 * This should fix PTS #6878.
		 */

#if MACH_KDB
		C_data_request_overflow++;
#endif MACH_KDB

		dr = (struct data_request *) zalloc(data_request_zone);

		dr->mobj           = mobj;
		dr->offset         = offset;
		dr->length         = length;
		dr->desired_access = desired_access;

		simple_lock(&data_request_queue_lock);
		dr->next = data_request_queue;
		data_request_queue = dr;

		/*
		 * Possibly we blocked in zalloc, and all kthreads are done
		 * in between. There is no one out to compute the request
		 * queue, if we don't.
		 */

		if (data_request_in_progress == 0) {
			data_request_in_progress++;
			goto must_do_it_myself;
		}
		simple_unlock(&data_request_queue_lock);

		return KERN_SUCCESS;
	}
#endif	/* NORMA_IPC && !NORMA2 */

	simple_lock(&data_request_queue_lock);
	data_request_in_progress++;
	simple_unlock(&data_request_queue_lock);

	process_copy_data_request(mobj, offset, length, desired_access);

	/*
	 * We are done with the copy_request, which woke up this kthread.
	 * As a little extra work, help out on clearing the overflow queue.
	 */

	for (;;) {
		simple_lock(&data_request_queue_lock);
must_do_it_myself:
		if (!(dr=data_request_queue)) {
			assert(data_request_in_progress > 0);
			data_request_in_progress--;
			simple_unlock(&data_request_queue_lock);
			break;
		}

		data_request_queue = dr->next;

		simple_unlock(&data_request_queue_lock);

		process_copy_data_request(dr->mobj,
					  dr->offset,
					  dr->length,
					  dr->desired_access);
		zfree(data_request_zone, dr);
	}

	return(KERN_SUCCESS);
}

process_copy_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	extern zone_t vm_map_copy_zone;
	vm_map_copy_t copy;
	kern_return_t kr;

	xmm_entry4(process_copy_data_request, mobj, offset, length, desired_access);

	kr = vm_map_copyin_page_list(MOBJ->map,
				     MOBJ->start + offset,
				     PAGE_SIZE,
				     FALSE,/* src_destroy */
				     TRUE,/* steal pages */
				     &copy,
				     FALSE/* is continuation */,
				     VM_PROT_READ,
				     FALSE/* no zero fill */);
	if (kr) {
		fret("xmm_copy_data_request 0x%x 0x%x 0x%x: %x\n",
		     MOBJ->start, offset, length, kr);
		return K_DATA_ERROR(mobj, offset, length, kr);
	}
	/* XXX should only return appropriate access */
	return K_DATA_SUPPLY(mobj, offset, (vm_offset_t) copy, length,
			     VM_PROT_NONE, FALSE, XMM_REPLY_NULL);
}

#if	MACH_KDB
m_copy_db_print(mobj)
xmm_obj_t	mobj;
{
	iprintf("map=0x%x start=0x%x size=0x%x\n",
		MOBJ->map, MOBJ->start, MOBJ->size);
	iprintf("memory_object_name=0x%x memory_object=0x%x\n",
		MOBJ->memory_object_name, MOBJ->memory_object);
	return 0;
}

k_copy_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_copy_db_print(kobj);
}
#else	/* MACH_KDB */
m_copy_db_print(mobj) xmm_obj_t	mobj; { }
k_copy_db_print(kobj) xmm_obj_t	kobj; { }
#endif	/* MACH_KDB */
