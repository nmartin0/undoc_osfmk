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
 * $Log: xmm_interpose.c,v $
 * Revision 1.9  1994/11/18  20:56:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  21:25:06  mtm
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
 * Revision 1.6.14.1  1994/08/08  23:53:44  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.7  1994/08/08  19:33:47  andyp
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
 * Revision 1.6  1993/09/28  18:05:43  andyp
 * Update for the 1.2 release.
 *
 *
 *	Add methods for {k,m}_interpose_db_print.  [alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:27  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:19  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:37  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:10  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:43:19  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_interpose.c,v
 * Revision 1.2.6.2  1993/04/15  22:46:15  alanl
 * 	Paging flow control (NORMA_VM).  Added suppport for
 * 	data_write_completed.  [sjs]
 * 	[1993/04/15  22:11:56  alanl]
 *
 * Revision 1.2  1992/11/25  01:16:21  robert
 * 	fix history
 * 	[1992/11/09  22:20:32  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:50:17  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added support for K_RELEASE method.
 * 
 * 	Revision 1.1  1992/11/05  21:00:24  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.3.2.4  92/06/24  18:02:44  jeffreyh
 * 	Calling sequence changes for XMM Framework Cleanup.  No more
 * 	xmm_kobj_link().
 * 	[92/06/24            sjs]
 * 
 * 	Pass release arguments in lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.3.2.3  92/03/28  10:12:54  jeffreyh
 * 	Changed data_write to data_write_return, deleted data_return method.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.3.2.2  92/02/21  11:26:00  jsb
 * 	Now xmm_kobj_link handles checks for multiple inits.
 * 	[92/02/18  08:00:07  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed m_interpose_terminate to simply call M_TERMINATE instead of
 * 	explicitly deallocating obj, since we now do real reference counting.
 * 	[92/02/16  14:21:04  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  12:54:10  jsb]
 * 
 * Revision 2.3.2.1  92/01/21  21:54:19  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:23:15  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:16  jsb
 * 	Collect garbage. Return valid return values.
 * 	[91/06/29  15:31:20  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:21  jsb
 * 	First checkin.
 * 	[91/06/17  11:08:05  jsb]
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
 *	File:	norma/xmm_interpose.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for null instances of xmm functions.
 */

#ifdef	KERNEL
#include <norma/xmm_obj.h>
#else	KERNEL
#include <xmm_obj.h>
#endif	KERNEL

#include <norma/xmm_dipc.h>


xmm_decl(interpose, "interpose", sizeof(struct xmm_obj));

m_interpose_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_entry4(m_interpose_init, mobj, pagesize, internal, size);
	return M_INIT(mobj, pagesize, internal, size);
}

m_interpose_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	xmm_entry2(m_interpose_terminate, mobj, release);
	return M_TERMINATE(mobj, release);
}

void
m_interpose_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_interpose_deallocate, mobj);
}

m_interpose_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	xmm_entry4(m_interpose_copy, mobj, offset, length, new_mobj);
	return M_COPY(mobj, offset, length, new_mobj);
}

m_interpose_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_interpose_data_request, mobj, offset, length, desired_access);
	return M_DATA_REQUEST(mobj, offset, length, desired_access);
}

m_interpose_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_interpose_data_unlock, mobj, offset, length, desired_access);
	return M_DATA_UNLOCK(mobj, offset, length, desired_access);
}

m_interpose_data_write_return(mobj, offset, data, length,
		dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	xmm_entry7(m_interpose_data_write_return,
		mobj,
		offset,
		data,
		length,
		dirty,
		kernel_copy,
		use_routine);

	return M_DATA_WRT_RTN(mobj, offset, data, length,
			      dirty, kernel_copy, use_routine);
}

m_interpose_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry5(m_interpose_lock_completed,
		mobj, offset, length, reply, release);
	return M_LOCK_COMPLETED(mobj, offset, length, reply, release);
}

m_interpose_supply_completed(mobj, offset, length, result, error_offset, reply,
			     release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry7(m_interpose_supply_completed,
		mobj, offset, length, result, error_offset, reply, release);
	return M_SUPPLY_COMPLETED(reply, offset, length, result, error_offset);
}

m_interpose_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry5(m_interpose_change_completed,
		mobj, may_cache, copy_strategy, reply, release);
	return M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply,
				  release);
}

k_interpose_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_entry3(k_interpose_data_unavailable, kobj, offset, length);
	return K_DATA_UNAVAILABLE(kobj, offset, length);
}

k_interpose_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	xmm_entry4(k_interpose_get_attributes,
		kobj, object_ready, may_cache, copy_strategy);
	return K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
}

k_interpose_lock_request(kobj, offset, length, should_clean, should_flush,
			 lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	xmm_entry7(k_interpose_lock_request,
		kobj,
		offset,
		length,
		should_clean,
		should_flush,
		lock_value,
		reply);
	return K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush,
			      lock_value, reply);
}

k_interpose_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	xmm_entry4(k_interpose_data_error, kobj, offset, length, error_value);
	return K_DATA_ERROR(kobj, offset, length, error_value);
}

k_interpose_set_ready(kobj, object_ready, may_cache, write_completions,
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
	/*xmm_entry9(...);*/
	xmm_entry7(k_interpose_set_ready,
		kobj,
		object_ready,
		may_cache,
		write_completions,
		copy_strategy,
		cluster_size,
		use_routine);
	return K_SET_READY(kobj, object_ready, may_cache, write_completions,
			   copy_strategy, cluster_size,
			   use_routine, memory_object_name, reply);
}

k_interpose_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	xmm_entry2(k_interpose_destroy, kobj, reason);
	return K_DESTROY(kobj, reason);
}

k_interpose_data_supply(kobj, offset, data, length, lock_value, precious,
			reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	xmm_entry7(k_interpose_data_supply,
		kobj, offset, data, length, lock_value, precious, reply);
	return K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious,
			     reply);
}

kern_return_t
k_interpose_data_write_completed(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{
	xmm_entry3(k_interpose_data_write_completed, kobj, offset, size);
	K_DATA_WRITE_COMPLETED(kobj, offset, size);
}

k_interpose_release(kobj)
	xmm_obj_t kobj;
{
	xmm_entry1(k_interpose_release, kobj);
	return K_RELEASE(kobj);
}


#if	MACH_KDB
m_interpose_db_print(mobj)
	xmm_obj_t mobj;
{
	return 0;
}


k_interpose_db_print(kobj)
	xmm_obj_t	kobj;
{
	return m_interpose_db_print(kobj);
}
#endif	/* MACH_KDB */
