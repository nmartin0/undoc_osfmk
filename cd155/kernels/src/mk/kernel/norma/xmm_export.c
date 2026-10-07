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
 * $Log: xmm_export.c,v $
 * Revision 1.10  1994/11/18  20:56:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  21:25:02  mtm
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
 * Revision 1.7.2.1  1994/08/08  23:53:27  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.8  1994/08/08  19:33:36  andyp
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
 * Revision 1.7  1994/07/12  19:25:11  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.6.4.2  1994/04/05  23:24:33  stans
 *   !NORMA_IPC, compile with NORMA_IPC == 0, NORMA_{DEVICE,VM} == 1
 *
 * Revision 1.6.4.1  1994/02/25  22:41:52  andyp
 * Removed some lint with respect to some debugging routines.
 *
 * Revision 1.6  1993/09/28  18:05:23  andyp
 * Update for the 1.2 release.
 *
 *
 *	- Added PENDING_TRACKING fields to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:20  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:14  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:32  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:05  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:41:01  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_export.c,v
 * Revision 1.2.6.3  1993/04/15  22:45:51  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed method; changed set_ready to
 * 	include new parameters.  [sjs]
 * 	[1993/04/15  22:11:34  alanl]
 *
 * Revision 1.2.6.2  1993/04/08  15:20:01  mmp
 * 	More termination fixes: _proxy_terminate needs to do an extra
 * 	xmm_obj_release.  k_export_release needs to xmm_obj_unlink the
 * 	object after calling proxy_release.
 * 	[1993/04/08  15:12:38  mmp]
 * 
 * Revision 1.2  1992/11/25  01:16:05  robert
 * 	fix history
 * 	[1992/11/09  22:17:26  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:49:41  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added k_export_release (indicates successful terminate).
 * 
 * 	Revision 1.1  1992/11/05  21:00:20  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.2.6  92/06/24  18:02:31  jeffreyh
 * 	More XMM Framework Cleanup: Create ksvm object in _proxy_init.
 * 	  Rework release logic.  Remove xmm_pager from *_completed
 * 	  routines.  Move port cleanup to (new) m_export_deallocate
 * 	  routine.  Cleanup use of xmm_reply, and implement
 * 	  proxy_supply_completed.
 * 	[92/06/24            dlb]
 * 
 * 	Changed routines for XMM Framework Cleanup; changes for new
 * 		xmm_reply structure.
 * 	[92/06/18            sjs]
 * 
 * 	Add release logic to proxy_lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	Remove unused xmm_object_by_memory_object declaration.
 * 	Delete extra arg from M_CHANGE_COMPLETED in proxy_change_completed.
 * 	[92/06/08            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.2.5  92/05/27  00:53:54  jeffreyh
 * 	Return KERN_FAILURE if the mobj is invalid and release is set in
 * 	_proxy_change_completed - occurs if the passed port is a dead
 * 	 name.
 * 	[92/05/20            sjs]
 * 
 * Revision 2.4.2.4  92/03/28  10:12:45  jeffreyh
 * 	Change data_write to data_write_return, deleted data_return
 * 	 method, added logic to make change_completed work correctly with
 * 	 change_attributes() 
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.2.3  92/02/21  11:25:50  jsb
 * 	In _proxy_terminate, deallocate xmm_pager and release xmm_kernel.
 * 	[92/02/20  15:46:35  jsb]
 * 
 * 	Reference mobj on port to mobj conversion; release when done.
 * 	[92/02/20  10:54:05  jsb]
 * 
 * 	Changed MACH_PORT use to IP_NULL. Use m_interpose_deallocate.
 * 	[92/02/18  17:13:28  jsb]
 * 
 * 	Changed reply->mobj to reply->kobj.
 * 	[92/02/16  18:22:12  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Hide and release mobj in _proxy_terminate.
 * 	[92/02/16  15:51:50  jsb]
 * 
 * 	Renamed xmm_export_notify to xmm_pager_notify.
 * 	[92/02/10  17:27:15  jsb]
 * 
 * 	Changed proxy_init to use xmm object instead of
 * 	<guessed host, memory_object> pair.
 * 	Renamed mobj_port to xmm_pager, and xmm_control to xmm_kernel.
 * 	[92/02/10  17:01:03  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  12:51:49  jsb]
 * 
 * Revision 2.4.2.2  92/01/21  21:54:06  jsb
 * 	Added xmm_export_notify stub.
 * 	[92/01/21  18:22:48  jsb]
 * 
 * 	Use ports instead of pointers when communicating with xmm_import.c.
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:21:43  jsb]
 * 
 * 	Fixes from OSF.
 * 	[92/01/17  14:14:46  jsb]
 * 
 * Revision 2.4.2.1.1.1  92/01/15  12:15:33  jeffreyh
 * 	Deallocate memory object name port on termination. (dlb)
 * 
 * Revision 2.4.2.1  92/01/03  16:38:45  jsb
 * 	Added missing type cast.
 * 	[91/12/27  21:29:32  jsb]
 * 
 * 	Cleaned up debugging printf.
 * 	[91/12/24  14:30:28  jsb]
 * 
 * Revision 2.4  91/11/15  14:10:03  rpd
 * 	Use ipc_port_copy_send in _proxy_init for import_master.
 * 	[91/09/23  09:14:28  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:07  jsb
 * 	Fixed object importation protocol.
 * 	Corrected declaration of _proxy_lock_completed.
 * 	[91/06/29  15:28:46  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:15  jsb
 * 	First checkin.
 * 	[91/06/17  11:06:11  jsb]
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
 *	File:	norma/xmm_export.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Xmm layer for allowing remote kernels to map a local object.
 */

#include <norma_ipc.h>

#include <norma/xmm_obj.h>
#include <kern/host.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <mach/notify.h>
#include <mach/proxy.h>

#include <norma/xmm_dipc.h>

struct mobj {
	struct xmm_obj	obj;
	ipc_port_t	xmm_pager;
	ipc_port_t	xmm_kernel;
#if	PENDING_TRACKING
	int		p_seen;		/* data_writes came through */
	int		p_matched;	/* completeds that returned */
	int		p_terminated;	/* orphans */
#endif
};

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_export_init			m_invalid_init
#define	m_export_terminate		m_invalid_terminate
#define	m_export_copy			m_invalid_copy
#define	m_export_data_request		m_invalid_data_request
#define	m_export_data_unlock		m_invalid_data_unlock
#define	m_export_data_write_return	m_invalid_data_write_return
#define	m_export_lock_completed		m_invalid_lock_completed
#define	m_export_supply_completed	m_invalid_supply_completed
#define	m_export_change_completed	m_invalid_change_completed
#define	m_export_data_write_completed	m_invalid_data_write_completed

extern m_export_db_print();
extern k_export_db_print();

xmm_decl(export, "export", sizeof(struct mobj));

k_export_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_entry3(k_export_data_unavailable, kobj, offset, length);

#ifdef	lint
	K_DATA_UNAVAILABLE(kobj, offset, length);
#endif	lint
	return proxy_data_unavailable(KOBJ->xmm_kernel, offset, length);
}

k_export_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	xmm_entry4(k_export_get_attributes, kobj, object_ready, may_cache, copy_strategy);

#ifdef	lint
	K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
#endif	lint
	return proxy_get_attributes(KOBJ->xmm_kernel, object_ready, may_cache,
				    copy_strategy);
}

k_export_lock_request(kobj, offset, length, should_clean, should_flush,
		      lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	kern_return_t kr;

	xmm_entry7(k_export_lock_request,
		kobj,
		offset,
		length,
		should_clean,
		should_flush,
		lock_value,
		reply);

#ifdef	lint
	K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
		       lock_value, reply);
#endif	lint
	if (reply == XMM_REPLY_NULL) {
		return proxy_lock_request(KOBJ->xmm_kernel, offset, length,
					  should_clean, should_flush,
					  lock_value, IP_NULL);
	}
	kr = xmm_reply_allocate_proxy(reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	assert(reply != XMM_REPLY_NULL);
	reply->kobj = kobj;
	return proxy_lock_request(KOBJ->xmm_kernel, offset, length,
				  should_clean, should_flush, lock_value,
				  reply->reply_proxy);
}

k_export_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	xmm_entry4(k_export_data_error, kobj, offset, length, error_value);

#ifdef	lint
	K_DATA_ERROR(kobj, offset, length, error_value);
#endif	lint
	return proxy_data_error(KOBJ->xmm_kernel, offset, length, error_value);
}

k_export_set_ready(kobj, object_ready, may_cache, write_completions,
		   copy_strategy, cluster_size,
		   use_routine, memory_object_name, reply)
	xmm_obj_t kobj;
	boolean_t object_ready;
	boolean_t may_cache;
        boolean_t write_completions;
	memory_object_copy_strategy_t copy_strategy;
        vm_size_t cluster_size;
	int use_routine;
	ipc_port_t memory_object_name;
	xmm_reply_t reply;
{
	kern_return_t kr;

	/*xmm_entry9(...)*/
	xmm_entry7(k_export_set_ready,
		kobj,
		object_ready,
		may_cache,
		write_completions,
		copy_strategy,
		cluster_size,
		use_routine);

#ifdef	lint
	K_SET_READY(kobj, object_ready, may_cache, write_completions,
		    copy_strategy, cluster_size,
		    use_routine, memory_object_name, reply);
#endif	lint
	if (reply == XMM_REPLY_NULL) {
		return proxy_set_ready(KOBJ->xmm_kernel, KOBJ->xmm_pager,
				       object_ready, may_cache,
				       write_completions, copy_strategy,
				       cluster_size, KERN_SUCCESS, use_routine,
				       memory_object_name, IP_NULL);
	}
	kr = xmm_reply_allocate_proxy(reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	assert(reply != XMM_REPLY_NULL);
	reply->kobj = kobj;
	return proxy_set_ready(KOBJ->xmm_kernel, KOBJ->xmm_pager,
			       object_ready, may_cache, write_completions,
			       copy_strategy, cluster_size,
			       KERN_SUCCESS, use_routine,
			       memory_object_name, reply->reply_proxy);
}

k_export_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	xmm_entry2(k_export_destroy, kobj, reason);

#ifdef	lint
	K_DESTROY(kobj, reason);
#endif	lint
	return proxy_destroy(KOBJ->xmm_kernel, reason);
}

k_export_data_supply(kobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	kern_return_t kr;

	xmm_entry7(k_export_data_supply, kobj, offset, data, length, lock_value, precious, reply);
#ifdef	lint
	K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious, reply);
#endif	lint
	if (reply == XMM_REPLY_NULL) {
		return proxy_data_supply(KOBJ->xmm_kernel, offset, data, length,
					 lock_value, precious, IP_NULL);
	}
	kr = xmm_reply_allocate_proxy(reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	assert(reply != XMM_REPLY_NULL);
	reply->kobj = kobj;
	return proxy_data_supply(KOBJ->xmm_kernel, offset, data, length,
				 lock_value, precious, reply->reply_proxy);
}

k_export_data_write_completed(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{

	xmm_entry3(k_export_data_write_completed, kobj, offset, size);

#ifdef	lint
	K_DATA_WRITE_COMPLETEDY(kobj, offset, size);
#endif	lint
#if	PENDING_TRACKING
	KOBJ->p_matched++;
#endif
	return proxy_data_write_completed(KOBJ->xmm_kernel, offset, size);
}

k_export_release(kobj)
	xmm_obj_t kobj;
{
	kern_return_t kr;

	xmm_entry1(k_export_release, kobj);

	kr = proxy_release(KOBJ->xmm_kernel);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	xmm_obj_unlink(KOBJ);
	return kr;
}

xmm_obj_t
convert_xmm_pager_to_mobj(xmm_pager)
	ipc_port_t xmm_pager;
{
	xmm_obj_t mobj = XMM_OBJ_NULL;

	xmm_entry1(convert_xmm_pager_to_mobj, xmm_pager);

	if (IP_VALID(xmm_pager)) {
		ip_lock(xmm_pager);
		if (ip_active(xmm_pager) &&
		    ip_kotype(xmm_pager) == IKOT_XMM_PAGER) {
			mobj = (xmm_obj_t) xmm_pager->ip_kobject;
			xmm_obj_reference(mobj);
		}
		ip_unlock(xmm_pager);
	}
	return mobj;
}

boolean_t
xmm_pager_notify(msg)
	mach_msg_header_t *msg;
{
	xmm_entry1(xmm_pager_notify, msg);

	return FALSE;
}

_proxy_init(xmm_object, xmm_kernel, pagesize, internal, size)
	ipc_port_t xmm_object;
	ipc_port_t xmm_kernel;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry5(_proxy_init, xmm_object, xmm_kernel, pagesize, internal, size);

	/*
	 * XXX
	 * Check for multiple inits and/or reuse of memory_object.
	 * XXX
	 * Should use proxy_set_ready to return errors.
	 */
	ip_lock(xmm_object);
	if (ip_kotype(xmm_object) != IKOT_XMM_OBJECT) {
		ip_unlock(xmm_object);
		return KERN_INVALID_ARGUMENT;
	}
	mobj = (xmm_obj_t) xmm_object->ip_kobject;
	ip_unlock(xmm_object);

	kr = xmm_ksvm_create(mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		printf("_proxy_init: xmm_ksvm_create: %x\n", kr);
		return kr;
	}

	kr = xmm_obj_allocate(&export_class, mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		printf("_proxy_init: xmm_obj_allocate: %x\n", kr);
		return kr;
	}

	MOBJ->xmm_pager = ipc_port_alloc_kernel();
	if (MOBJ->xmm_pager == IP_NULL) {
		panic("m_import_init: allocate xmm_pager");
	}
	ipc_kobject_set(MOBJ->xmm_pager, (ipc_kobject_t) mobj, IKOT_XMM_PAGER);
	MOBJ->xmm_kernel = xmm_kernel;

#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif

	xmm_obj_reference(mobj);
	kr = M_INIT(mobj, pagesize, internal, size);
	xmm_obj_release(mobj);
	return kr;
}

_proxy_terminate(xmm_pager, release)
	ipc_port_t xmm_pager;
	boolean_t release;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry2(_proxy_terminate, xmm_pager, release);

	mobj = convert_xmm_pager_to_mobj(xmm_pager);
	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = M_TERMINATE(mobj, release);

	/*
	 * give up reference from convert above, then give up reference
	 * from original allocation.
	 */
	xmm_obj_release(mobj);
	xmm_obj_release(mobj);
	return kr;
}

_proxy_copy(xmm_pager, offset, length, new_memory_object)
	ipc_port_t xmm_pager;
	vm_offset_t offset;
	vm_size_t length;
	memory_object_t new_memory_object;
{
	xmm_obj_t mobj;

	xmm_entry4(_proxy_copy, xmm_pager, offset, length, new_memory_object);

#ifdef	lint
	offset++;
	length++;
	new_memory_object++;
#endif
	mobj = convert_xmm_pager_to_mobj(xmm_pager);
	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	panic("_proxy_copy: not implemented\n");
	xmm_obj_release(mobj);
	return KERN_FAILURE;
}

_proxy_data_request(xmm_pager, offset, length, desired_access)
	ipc_port_t xmm_pager;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry4(_proxy_data_request, xmm_pager, offset, length, desired_access);
	mobj = convert_xmm_pager_to_mobj(xmm_pager);
	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = M_DATA_REQUEST(mobj, offset, length, desired_access);
	xmm_obj_release(mobj);
	return kr;
}

_proxy_data_unlock(xmm_pager, offset, length, desired_access)
	ipc_port_t xmm_pager;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry4(_proxy_data_unlock, xmm_pager, offset, length, desired_access);
	mobj = convert_xmm_pager_to_mobj(xmm_pager);
	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
	kr = M_DATA_UNLOCK(mobj, offset, length, desired_access);
	xmm_obj_release(mobj);
	return kr;
}

_proxy_data_write_return(xmm_pager, offset, data, length, dirty, kernel_copy,
	use_routine)
	ipc_port_t xmm_pager;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry7(_proxy_data_write_return,
		xmm_pager,
		offset,
		data,
		length,
		dirty,
		kernel_copy,
		use_routine);

	mobj = convert_xmm_pager_to_mobj(xmm_pager);
	if (mobj == XMM_OBJ_NULL) {
		return KERN_FAILURE;
	}
#if	PENDING_TRACKING
	MOBJ->p_seen++;
#endif
	kr = M_DATA_WRT_RTN(mobj, offset, data, length,
			    dirty, kernel_copy, use_routine);
	xmm_obj_release(mobj);
	return kr;
}

_proxy_lock_completed(reply_to, offset, length, release)
	ipc_port_t reply_to;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t release;
{
	xmm_reply_t reply;
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry4(_proxy_lock_completed, reply_to, offset, length, release);

	reply = convert_port_to_reply(reply_to);
	if (reply == XMM_REPLY_NULL) {
		return KERN_FAILURE;
	}

	mobj = reply->kobj;
	kr = M_LOCK_COMPLETED(mobj, offset, length, reply, release);

	if (release)
		xmm_obj_release(mobj);

	return kr;
}

_proxy_supply_completed(reply_to, offset, length, result, error_offset,
			release)
	ipc_port_t reply_to;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	boolean_t release;
{
	xmm_reply_t reply;
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry6(_proxy_supply_completed,
		reply_to,
		offset,
		length,
		result,
		error_offset,
		release);

	reply = convert_port_to_reply(reply_to);
	if (reply == XMM_REPLY_NULL) {
	        printf ("proxy_supply_completed: reply == XMM_REPLY_NULL: FAILURE\n");
		return KERN_FAILURE;
	}

	mobj = reply->kobj;
	kr = M_SUPPLY_COMPLETED(mobj, offset, length, result, 
				  error_offset, reply, release);

	if (release)
		xmm_obj_release(mobj);

	return kr;
}

_proxy_change_completed(reply_to, may_cache, copy_strategy, release)
	ipc_port_t reply_to;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	boolean_t release;
{
	xmm_reply_t reply;
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry4(_proxy_change_completed, reply_to, may_cache, copy_strategy, release);

	reply = convert_port_to_reply(reply_to);
	if (reply == XMM_REPLY_NULL) {
		printf ("proxy_change_completed: invalid reply\n");
		return KERN_FAILURE;
	}
	mobj = reply->kobj;
	kr = M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply,
				release);
	if (release) 
	    xmm_obj_release(mobj);

	return kr;
}

void
m_export_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_export_deallocate, mobj);

	ipc_kobject_set(MOBJ->xmm_pager, IKO_NULL, IKOT_NONE);
	ipc_port_dealloc_kernel(MOBJ->xmm_pager);
	ipc_port_release_send(MOBJ->xmm_kernel);
}


#if	MACH_KDB
m_export_db_print(mobj)
xmm_obj_t	mobj;
{
#if	NORMA_IPC
	iprintf("xmm_pager=0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_pager,
		db_port_kmsg_count(MOBJ->xmm_pager),
		MOBJ->xmm_pager->ip_norma_uid,
		MOBJ->xmm_pager->ip_norma_dest_node);
	iprintf("xmm_kernel=0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_kernel,
		db_port_kmsg_count(MOBJ->xmm_kernel),
		MOBJ->xmm_kernel->ip_norma_uid,
		MOBJ->xmm_kernel->ip_norma_dest_node);
#elif	NORMA2
	iprintf("xmm_pager=0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_pager,
		db_port_kmsg_count(MOBJ->xmm_pager),
		MOBJ->xmm_pager->dipc_uid,
		MOBJ->xmm_pager->dipc_node);
	iprintf("xmm_kernel=0x%x [%d kmsgs uid=0x%x dest=%d]\n",
		MOBJ->xmm_kernel,
		db_port_kmsg_count(MOBJ->xmm_kernel),
		MOBJ->xmm_kernel->dipc_uid,
		MOBJ->xmm_kernel->dipc_node);
#endif	/* NORMA2 */
#if	PENDING_TRACKING
	iprintf("p_seen=0x%x p_matched=0x%x p_terminated=0x%x\n",
		MOBJ->p_seen, MOBJ->p_matched, MOBJ->p_terminated);
#endif
	return 0;
}


k_export_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_export_db_print(kobj);
}
#else	/* MACH_KDB */
m_export_db_print(mobj) xmm_obj_t mobj; { }
k_export_db_print(kobj) xmm_obj_t kobj; { }
#endif	/* MACH_KDB */
