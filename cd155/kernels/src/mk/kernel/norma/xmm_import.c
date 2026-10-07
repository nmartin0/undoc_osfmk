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
 * $Log: xmm_import.c,v $
 * Revision 1.10  1994/11/18  20:56:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  21:25:04  mtm
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
 * Revision 1.7.2.1  1994/08/08  23:53:37  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.8  1994/08/08  19:33:42  andyp
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
 * Revision 1.7  1994/07/12  19:25:14  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.4.2  1994/04/05  23:25:31  stans
 *   !NORMA_IPC, compile with NORMA_IPC == 0, NORMA_{DEVICE,VM} == 1
 *
 * Revision 1.6.4.1  1994/02/25  22:41:56  andyp
 * Removed some lint with respect to some debugging routines.
 *
 * Revision 1.6  1993/09/28  18:05:33  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Added PENDING_TRACKING fields to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:25  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:35  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:08  dleslie
 * Patch release of April 23
 *
 * Revision 1.5  1993/04/17  22:39:34  SSD
 * this is the same as 1.3 and covers up up David Black hack for the
 * m_invoke panic.
 *
 * Revision 1.3  1993/04/12  17:08:43  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_import.c,v
 * Revision 1.2.4.5  1993/04/15  22:45:58  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed; added new parameters to
 * 	set_ready.  [sjs]
 * 	[1993/04/15  22:11:45  alanl]
 *
 * Revision 1.2.4.4  1993/04/08  15:20:04  mmp
 * 	_proxy_set_ready brings in a new send right for xmm_pager every
 * 	time it is called.  Save it the first time it comes in, but
 * 	release the extra send right every time thereafter so the
 * 	ports don't leak.
 * 	[1993/04/08  15:16:52  mmp]
 * 
 * Revision 1.2.4.3  1993/03/18  21:14:56  mmp
 * 	In m_import_terminate():  Always set MOBJ->terminated (not strictly
 * 	necessary but useful for debugging).  Always return KERN_SUCCESS
 * 	since the mobjs will not be torn down until there are no more
 * 	outstanding requests.
 * 	[1993/03/18  20:33:22  mmp]
 * 
 * Revision 1.2.4.2  1993/02/26  20:24:22  sjs
 * 	Decrement xmm_obj reference when cleaning up xmm_kernel port.
 * 	[93/02/26            sjs]
 * 
 * Revision 1.2  1992/11/25  01:16:13  robert
 * 	fix history
 * 	[1992/11/09  22:19:08  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:49:58  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added proxy_release to safely clean up the mobj after a
 * 	terminate.
 * 
 * 	Revision 1.1  1992/11/05  21:00:22  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.5.2.7  92/06/24  18:02:39  jeffreyh
 * 	Changes routines for new interface, changes for new reply
 * 	 structure.
 * 	[92/06/18            sjs]
 * 
 * 	Add release logic to lock completed processing.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.5.2.6  92/03/28  10:12:50  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return.
 * 	 Fixed terminate and change_completed to support m_o_change_attributes().
 * 	[92/03/20            sjs]
 * 
 * Revision 2.5.2.5  92/02/21  11:25:54  jsb
 * 	Reference mobj on port to mobj conversion; release when done.
 * 	[92/02/20  10:53:45  jsb]
 * 
 * 	Deallocate all resources upon termination.
 * 	Deallocate allocated replies.
 * 	Changed MACH_PORT_NULL uses to IP_NULL.
 * 	[92/02/18  07:56:15  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Handle m_import_terminate being called before proxy_set_ready has been.
 * 	This can happen because the vm system may terminate an object after
 * 	it has been initialized but before it is ready.
 * 	[92/02/16  15:26:31  jsb]
 * 
 * 	Don't call proxy_terminate on a null port.
 * 	[92/02/11  18:21:03  jsb]
 * 
 * 	Added ipc_kobject_set to null to break port/mobj association.
 * 	Accordingly, removed dead field and associated logic.
 * 	[92/02/11  13:22:39  jsb]
 * 
 * 	Renamed xmm_import_notify to xmm_kernel_notify.
 * 	[92/02/10  17:26:50  jsb]
 * 
 * 	Use xmm object instead of <guessed host, memory_object> pair in
 * 	proxy_init. Renamed {mobj,kobj}_port to xmm_{pager,kernel}.
 * 	[92/02/10  16:56:20  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  12:51:12  jsb]
 * 
 * Revision 2.5.2.4  92/01/21  22:22:24  jsb
 * 	18-Jan-92 David L. Black (dlb) at Open Software Foundation
 * 	Add dead field to mobj and use to synchronize termination
 * 	against other operations.
 * 
 * Revision 2.5.2.3  92/01/21  21:54:12  jsb
 * 	Added xmm_import_notify stub.
 * 	[92/01/21  18:20:54  jsb]
 * 
 * 	Use ports instead of pointers when communicating with xmm_export.c.
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:22:22  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:15:01  jsb]
 * 
 * Revision 2.5.2.2.1.1  92/01/15  12:16:26  jeffreyh
 * 	Pass memory_object_name port to proxy terminate. (dlb)
 * 
 * Revision 2.5.2.2  92/01/09  18:46:13  jsb
 * 	Use remote_host_priv() instead of norma_get_special_port().
 * 	[92/01/04  18:33:18  jsb]
 * 
 * Revision 2.5.2.1  92/01/03  16:38:50  jsb
 * 	Corrected log.
 * 	[91/12/24  14:33:29  jsb]
 * 
 * Revision 2.5  91/12/10  13:26:24  jsb
 * 	Added missing third parameter in call to proxy_terminate.
 * 	[91/12/10  12:48:52  jsb]
 * 
 * Revision 2.4  91/11/14  16:52:34  rpd
 * 	Replaced master_device_port_at_node call with calls to
 *	norma_get_special_port and norma_port_location_hint.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.3  91/07/01  08:26:12  jsb
 * 	Fixed object importation protocol. Return valid return values.
 * 	[91/06/29  15:30:10  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:18  jsb
 * 	First checkin.
 * 	[91/06/17  11:03:28  jsb]
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
 *	File:	norma/xmm_import.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer for mapping a remote object.
 */

#include <norma_ipc.h>

#include <norma/xmm_obj.h>
#include <norma/ipc_node.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <mach/notify.h>
#include <mach/proxy.h>

#include <norma/xmm_dipc.h>

#define	LOCK_REQUEST_SYNC	1

#if	LOCK_REQUEST_SYNC
unsigned long	c_lock_request_sync_block;
unsigned long	c_lock_request_sync_go;
unsigned long	c_lock_request_sync_terminate;
#endif	/* LOCK_REQUEST_SYNC */


struct mobj {
	struct xmm_obj	obj;
	ipc_port_t	xmm_object;
	ipc_port_t	xmm_pager;
	ipc_port_t	xmm_kernel;
	boolean_t	terminated;
#if	LOCK_REQUEST_SYNC
	int		sync_flying;	/* count of data_writes in flight */
	thread_t	sync_complete;	/* thread awaiting completions */
#endif	LOCK_REQUEST_SYNC
#if	PENDING_TRACKING
	int		p_seen;		/* data_writes on the way out */
	int		p_matched;	/* completeds on the way in */
	int		p_terminated;	/* lossage due to termination */
#endif
};

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_import_deallocate		m_interpose_deallocate
#define	k_import_data_unavailable	k_invalid_data_unavailable
#define	k_import_get_attributes		k_invalid_get_attributes
#define	k_import_lock_request		k_invalid_lock_request
#define	k_import_data_error		k_invalid_data_error
#define	k_import_set_ready		k_invalid_set_ready
#define	k_import_destroy		k_invalid_destroy
#define	k_import_data_supply		k_invalid_data_supply
#define	k_import_release		k_invalid_release
#define	k_import_data_write_completed	k_invalid_data_write_completed

extern m_import_db_print();
extern k_import_db_print();

xmm_decl(import, "import", sizeof(struct mobj));

kern_return_t
xmm_import_create(xmm_object, new_mobj)
	ipc_port_t xmm_object;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry2(xmm_import_create, xmm_object, new_mobj);

	kr = xmm_obj_allocate(&import_class, XMM_OBJ_NULL, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}

	MOBJ->xmm_object = xmm_object;
	MOBJ->xmm_pager = IP_NULL;
	MOBJ->xmm_kernel = IP_NULL;
	MOBJ->terminated = FALSE;
#if	LOCK_REQUEST_SYNC
	MOBJ->sync_flying = 0;
	MOBJ->sync_complete = THREAD_NULL;
#endif	/* LOCK_REQUEST_SYNC */
#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif

	*new_mobj = mobj;
	return KERN_SUCCESS;
}

m_import_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	kern_return_t kr;
	xmm_obj_t kobj = mobj;
	
	xmm_entry4(m_import_init, mobj, pagesize, internal, size);

#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint

	MOBJ->xmm_kernel = ipc_port_alloc_kernel();
	if (MOBJ->xmm_kernel == IP_NULL) {
		panic("m_import_init: allocate xmm_kernel");
	}
	xmm_obj_reference(mobj);
	ipc_kobject_set(MOBJ->xmm_kernel, (ipc_kobject_t) mobj,
			IKOT_XMM_KERNEL);

	kr = proxy_init(MOBJ->xmm_object, MOBJ->xmm_kernel, pagesize,
			internal, size);
#if 1
	if (kr) {
		panic("m_import_init: proxy_init returns %x\n", kr);
	}
#endif
	return KERN_SUCCESS;
}

m_import_terminate(mobj, release)
	xmm_obj_t	mobj;
	boolean_t	release;
{
	kern_return_t kr;

	xmm_entry2(m_import_terminate, mobj, release);

#ifdef	lint
	M_TERMINATE(mobj, release);
#endif	lint
	assert(MOBJ->xmm_kernel != IP_NULL);
	MOBJ->terminated = TRUE;
	if (MOBJ->xmm_pager == IP_NULL) {
		/*
		 * This can happen because the vm system only waits
		 * until the object is initialized before terminating
		 * it; it does not wait until the object is ready.
		 * We must therefore wait for _proxy_set_ready ourselves
		 * before terminating the object. We don't even have the
		 * option of calling proxy_terminate on xmm_object, since
		 * we no longer have send rights for xmm_object.
		 *
		 * We need to retain the xmm_kernel to mobj association
		 * so that we can process _proxy_set_ready. Fortunately,
		 * there is no need to break this association, since the
		 * only call that we should receive is _proxy_set_ready.
		 * Any spurious calls to anything else will be caught
		 * at the xmm_server level.
		 */
		printf("m_import_terminate on unready object 0x%x\n", mobj);
		return KERN_SUCCESS;
	}
#if	LOCK_REQUEST_SYNC
	if (--(MOBJ->sync_flying) > 0) {
		thread_t	th;

		if ((th = MOBJ->sync_complete) != THREAD_NULL) {
			MOBJ->sync_complete = THREAD_NULL;
			c_lock_request_sync_terminate++;
			th->ith_state = MACH_SEND_INTERRUPTED;
			thread_go(th);
		}
	}
#endif	/* LOCK_REQUEST_SYNC */
	kr = proxy_terminate(MOBJ->xmm_pager, release);
	assert(kr == KERN_SUCCESS);

	return KERN_SUCCESS;
}

m_import_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	xmm_entry4(m_import_copy, mobj, offset, length, new_mobj);

#ifdef	lint
	M_COPY(mobj, offset, length, new_mobj);
#endif	lint
	panic("m_import_copy\n");
}

/*
 *	VM system should handle ready synchronization for everything else
 */

m_import_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_import_data_request, mobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	return proxy_data_request(MOBJ->xmm_pager, offset, length,
				  desired_access);
}

m_import_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_import_data_unlock, mobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_UNLOCK(mobj, offset, length, desired_access);
#endif	lint
	return proxy_data_unlock(MOBJ->xmm_pager, offset, length,
				 desired_access);
}

m_import_data_write_return(mobj, offset, data, length,
			   dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{

	xmm_entry7(m_import_data_write_return, mobj, offset, data, length, dirty, kernel_copy, use_routine);

#ifdef	lint
	M_DATA_WRT_RTN (mobj, offset, data, length,
			dirty, kernel_copy, use_routine);
#endif	lint
#if	PENDING_TRACKING
	MOBJ->p_seen++;
#endif
#if	LOCK_REQUEST_SYNC
	MOBJ->sync_flying++;
#endif	/* LOCK_REQUEST_SYNC */

	return proxy_data_write_return(MOBJ->xmm_pager, offset, data, length,
					dirty, kernel_copy, use_routine);
}

m_import_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	ipc_port_t reply_to = reply->reply_to.port;

	xmm_entry5(m_import_lock_completed, mobj, offset, length, reply, release);

#ifdef	lint
	M_LOCK_COMPLETED(mobj, offset, length, reply, release);
#endif	lint
	assert(reply->reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	xmm_reply_deallocate(reply);
	/*
	 * At this layer, mobj and kobj should be the same, so we only
	 * put mobj into the xmm_pager.
	 */
#if	LOCK_REQUEST_SYNC
	if (MOBJ->sync_flying > 0) {
		thread_t	self;

		assert(MOBJ->sync_complete == THREAD_NULL);
		self = current_thread();
		MOBJ->sync_complete = self;
		c_lock_request_sync_block++;
		self->ith_state = MACH_MSG_SUCCESS;
		thread_will_wait(self);
		thread_block(0);
		assert(MOBJ->sync_flying == 0);
		assert(MOBJ->sync_complete == THREAD_NULL);
	}
#endif	/* LOCK_REQUEST_SYNC */
	return proxy_lock_completed(reply_to, offset, length, release);
}

m_import_supply_completed(mobj, offset, length, result, error_offset, reply,
			  release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	xmm_reply_t reply;
	vm_offset_t error_offset;
	boolean_t release;
{
	ipc_port_t reply_to = reply->reply_to.port;

	xmm_entry7(m_import_supply_completed,
		mobj,
		offset,
		length,
		result,
		error_offset,
		reply,
		release);

#ifdef	lint
	M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset, reply);
#endif	lint
	assert(reply->reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	xmm_reply_deallocate(reply);
	return proxy_supply_completed(reply_to, offset, length, result,
				      error_offset, release);
}

m_import_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
	ipc_port_t reply_to = reply->reply_to.port;

	xmm_entry5(m_import_change_completed,
		mobj, may_cache, copy_strategy, reply, release);

#ifdef	lint
	M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply, release);
#endif	lint
	assert(reply->reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	xmm_reply_deallocate(reply);
	/*
	 * At this layer, mobj and kobj should be the same, so we only
	 * put mobj into the xmm_pager.
	 */
	return proxy_change_completed(reply_to,  may_cache, copy_strategy, 
				      release);
}

xmm_obj_t
convert_xmm_kernel_to_kobj(xmm_kernel)
	ipc_port_t xmm_kernel;
{
	xmm_obj_t kobj = XMM_OBJ_NULL;

	xmm_entry1(convert_xmm_kernel_to_kobj, xmm_kernel);

	if (IP_VALID(xmm_kernel)) {
		ip_lock(xmm_kernel);
		if (ip_active(xmm_kernel) &&
		    ip_kotype(xmm_kernel) == IKOT_XMM_KERNEL) {
			kobj = (xmm_obj_t) xmm_kernel->ip_kobject;
			xmm_obj_reference(kobj);
		}
		ip_unlock(xmm_kernel);
	}
	return kobj;
}

boolean_t
xmm_kernel_notify(msg)
	mach_msg_header_t *msg;
{
	xmm_entry1(xmm_kernel_notify, msg);

	return FALSE;
}

_proxy_release(xmm_kernel)
	ipc_port_t	xmm_kernel;
{
	kern_return_t kr;
	xmm_obj_t kobj;

	xmm_entry1(_proxy_release, xmm_kernel);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	/*
	 * Deallocate resources associated with mobj.
	 * MOBJ->xmm_object was deallocated via proxy_init.
	 * MOBJ->xmm_kernel will be deallocated via proxy_terminate.
	 * We must explicitly deallocate MOBJ->xmm_pager
	 * (to make mobj inaccessible) and then release mobj.
	 */
	ipc_kobject_set(KOBJ->xmm_kernel, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(KOBJ->xmm_kernel);
	/*
	 * release reference held by xmm_kernel port
	 */
	xmm_obj_release(kobj);

	kr = K_RELEASE(kobj);
	xmm_obj_release(kobj);

	return(KERN_SUCCESS);
}

_proxy_data_unavailable(xmm_kernel, offset, length)
	ipc_port_t	xmm_kernel;
	vm_offset_t	offset;
	vm_size_t	length;
{
	xmm_obj_t kobj;
	kern_return_t kr;

	xmm_entry3(_proxy_data_unavailable, xmm_kernel, offset, length);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = K_DATA_UNAVAILABLE(kobj, offset, length);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_get_attributes(xmm_kernel, object_ready, may_cache, copy_strategy)
	ipc_port_t	xmm_kernel;
	boolean_t	*object_ready;
	boolean_t	*may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	xmm_obj_t kobj;
	kern_return_t kr;

	xmm_entry4(_proxy_get_attributes, xmm_kernel, object_ready, may_cache, copy_strategy);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_lock_request(xmm_kernel, offset, length, should_clean, should_flush,
		    prot, reply_to)
	ipc_port_t	xmm_kernel;
	vm_offset_t	offset;
	vm_size_t	length;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	prot;
	ipc_port_t	reply_to;
{
	kern_return_t kr;
	xmm_reply_t reply;
	xmm_obj_t kobj;

	xmm_entry7(_proxy_lock_request,
		xmm_kernel,
		offset,
		length,
		should_clean,
		should_flush,
		prot,
		reply_to);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		/*
		 * XXX	What about reply message???
		 * XXX	Printf here for now.
		 * XXX  Same thing goes for other calls that ask for replies.
		 */
		printf("Rejecting proxy_lock_request on dead object\n");
		return KERN_FAILURE;
	}
	kr = xmm_reply_allocate_send_once(kobj, reply_to, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
			    prot, reply);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_data_error(xmm_kernel, offset, length, error_value)
	ipc_port_t	xmm_kernel;
	vm_offset_t	offset;
	vm_size_t	length;
	kern_return_t	error_value;
{
	xmm_obj_t kobj;
	kern_return_t kr;

	xmm_entry4(_proxy_data_error, xmm_kernel, offset, length, error_value);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = K_DATA_ERROR(kobj, offset, length, error_value);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_set_ready(xmm_kernel, xmm_pager, object_ready, may_cache, 
		 write_completions, copy_strategy, cluster_size,
		 error_value, use_routine, memory_object_name, reply_to)
	ipc_port_t	xmm_kernel;
	ipc_port_t	xmm_pager;
	boolean_t	object_ready;
	boolean_t	may_cache;
        boolean_t	write_completions;
	memory_object_copy_strategy_t copy_strategy;
        vm_size_t	cluster_size;
	kern_return_t	error_value;
	int		use_routine;
	ipc_port_t	memory_object_name;
	ipc_port_t	reply_to;
{
	xmm_obj_t kobj;
	xmm_reply_t reply;
	kern_return_t kr;

	/*xmm_entry11(...)*/
	xmm_entry7(_proxy_set_ready,
		xmm_kernel,
		xmm_pager,
		object_ready,
		may_cache,
		write_completions,
		copy_strategy,
		cluster_size);

	if (error_value) {
		/* destroy? or should export have done that? */
		printf("proxy_set_ready loses\n");
	}
	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}

	/*
	 * save the pager port if this is the first send right made,
	 * otherwise release the right.  The original send right will
	 * be sent back and released via proxy_terminate.
	 */
	if (KOBJ->xmm_pager == IP_NULL)
		KOBJ->xmm_pager = xmm_pager;
	else
		ipc_port_release_send(KOBJ->xmm_pager);

	if (KOBJ->terminated) {
		/*
		 * Now that we have xmm_pager, we can process the
		 * pending m_import_terminate.
		 *
		 * XXX what should we do with reply?
		 */
		printf("_proxy_set_ready on terminated 0x%x\n", kobj);
		kr = m_import_terminate(kobj, TRUE);
		xmm_obj_release(kobj);
		return kr;
	}
	kr = xmm_reply_allocate_send_once(kobj, reply_to, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_SET_READY(kobj, object_ready, may_cache, write_completions,
			 copy_strategy, cluster_size,
			 use_routine, memory_object_name, reply);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_destroy(xmm_kernel, reason)
	ipc_port_t	xmm_kernel;
	kern_return_t	reason;
{
	xmm_obj_t kobj;
	kern_return_t kr;

	xmm_entry2(_proxy_destroy, xmm_kernel, reason);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = K_DESTROY(kobj, reason);
	xmm_obj_release(kobj);
	return kr;
}

_proxy_data_supply(xmm_kernel, offset, data, length, lock_value, precious,
		   reply_to)
	ipc_port_t	xmm_kernel;
	vm_offset_t	offset;
	pointer_t	data;
	unsigned int	length;
	vm_prot_t	lock_value;
	ipc_port_t	reply_to;
{
	xmm_obj_t kobj;
	xmm_reply_t reply;
	kern_return_t kr;

	xmm_entry7(_proxy_data_supply,
		xmm_kernel,
		offset,
		data,
		length,
		lock_value,
		precious,
		reply_to);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = xmm_reply_allocate_send_once(kobj, reply_to, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious,
			   reply);
	xmm_obj_release(kobj);
	return kr;
}


_proxy_data_write_completed(xmm_kernel, offset, size)
	ipc_port_t	xmm_kernel;
	vm_offset_t	offset;
	vm_size_t	size;
{
	xmm_obj_t kobj;
	kern_return_t kr;

	xmm_entry3(_proxy_data_write_completed, xmm_kernel, offset, size);

	kobj = convert_xmm_kernel_to_kobj(xmm_kernel);
	if (kobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
#if	PENDING_TRACKING
	KOBJ->p_matched++;
#endif
#if	LOCK_REQUEST_SYNC
	assert(KOBJ->sync_flying >= 1);
	if (--(KOBJ->sync_flying) == 0) {
		thread_t	th;

		if ((th = KOBJ->sync_complete) != THREAD_NULL) {
			KOBJ->sync_complete = THREAD_NULL;
			c_lock_request_sync_go++;
			thread_go(th);
		}
	}
#endif	/* LOCK_REQUEST_SYNC */
	kr = K_DATA_WRITE_COMPLETED(kobj, offset, size);
	xmm_obj_release(kobj);
	return kr;
}



#if	MACH_KDB
m_import_db_print(mobj)
xmm_obj_t	mobj;
{
	extern char	*db_bool_str();

#if	NORMA_IPC
	iprintf("object\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_object,
		db_port_kmsg_count(MOBJ->xmm_object),
		MOBJ->xmm_object->ip_norma_uid,
		MOBJ->xmm_object->ip_norma_dest_node);
	iprintf("pager\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_pager,
		db_port_kmsg_count(MOBJ->xmm_pager),
		MOBJ->xmm_pager->ip_norma_uid,
		MOBJ->xmm_pager->ip_norma_dest_node);
	iprintf("kernel\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_kernel,
		db_port_kmsg_count(MOBJ->xmm_kernel),
		MOBJ->xmm_kernel->ip_norma_uid,
		MOBJ->xmm_kernel->ip_norma_dest_node);
#elif	NORMA2
	iprintf("object\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_object,
		db_port_kmsg_count(MOBJ->xmm_object),
		MOBJ->xmm_object->dipc_uid,
		MOBJ->xmm_object->dipc_node);
	iprintf("pager\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_pager,
		db_port_kmsg_count(MOBJ->xmm_pager),
		MOBJ->xmm_pager->dipc_uid,
		MOBJ->xmm_pager->dipc_node);
	iprintf("kernel\t0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_kernel,
		db_port_kmsg_count(MOBJ->xmm_kernel),
		MOBJ->xmm_kernel->dipc_uid,
		MOBJ->xmm_kernel->dipc_node);
#endif	/* NORMA2 */
	iprintf("terminated=%s\n",
		db_bool_str(MOBJ->terminated));
#if	LOCK_REQUEST_SYNC
	iprintf("sync_flying=%d sync_complete=0x%x\n",
		MOBJ->sync_flying,
		MOBJ->sync_complete);
#endif	/* LOCK_REQUEST_SYNC */
#if	PENDING_TRACKING
	iprintf("p_seen=0x%x p_matched=0x%x p_terminated=0x%x\n",
		MOBJ->p_seen,
		MOBJ->p_matched,
		MOBJ->p_terminated);
#endif
	return 0;
}

k_import_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_import_db_print(kobj);
}
#else	/* MACH_KDB */
m_import_db_print(mobj) xmm_obj_t mobj; { }
k_import_db_print(kobj) xmm_obj_t kobj; { }
#endif	/* MACH_KDB */
