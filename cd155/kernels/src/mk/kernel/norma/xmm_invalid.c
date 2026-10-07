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
 * $Log: xmm_invalid.c,v $
 * Revision 1.9  1994/11/18  20:56:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  21:25:07  mtm
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
 * Revision 1.6.14.1  1994/08/08  23:53:50  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.7  1994/08/08  19:33:51  andyp
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
 * Revision 1.6  1993/09/28  18:05:52  andyp
 * Update for the 1.2 release.
 *
 *
 *	Added {k,m}_db_print methods.  [alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:31  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:21  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:39  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:13  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:44:14  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_invalid.c,v
 * Revision 1.2.6.2  1993/04/15  22:46:29  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed; added parameters to set_ready.  [sjs]
 * 	[1993/04/15  22:12:06  alanl]
 *
 * Revision 1.2  1992/11/25  01:16:30  robert
 * 	fix history
 * 	[1992/11/09  22:21:56  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:50:40  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added k_invalid_release for K_RELEASE method.
 * 
 * 	Revision 1.1  1992/11/05  21:00:26  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.2.4  92/06/24  18:02:47  jeffreyh
 * 	Changed routines to for new interfaces.
 * 	[92/06/18            sjs]
 * 
 * 	Add release arguments to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.2.3  92/03/28  10:13:02  jeffreyh
 * 	Changed data_write to data_write_return, removed data_return
 * 	 method.  Change TERMINATE and CHANGE_COMPLETED methods to
 * 	 support m_o_change_attributes()
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.2.2  92/02/21  11:26:03  jsb
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	[92/02/16  14:21:46  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	[92/02/09  13:56:31  jsb]
 * 
 * Revision 2.4.2.1  92/01/21  21:54:23  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:23:35  jsb]
 * 
 * Revision 2.4  91/12/10  13:26:26  jsb
 * 	Added better debugging support to xmm_invalid_complain.
 * 	[91/12/10  12:46:26  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:20  jsb
 * 	Added declaration of invalid_mclass, for debugging use.
 * 	[91/06/29  15:32:54  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:26  jsb
 * 	First checkin.
 * 	[91/06/17  11:03:08  jsb]
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
 *	File:	norma/xmm_invalid.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Definitions for invalid instances of xmm functions.
 */

#ifdef	KERNEL
#include <norma/xmm_obj.h>
#else	KERNEL
#include <xmm_obj.h>
#endif	KERNEL

#include <norma/xmm_dipc.h>


xmm_decl(invalid, "invalid", 0);

xmm_invalid_complain(name, mobj)
	char *name;
	xmm_obj_t mobj;
{
	xmm_entry2(xmm_invalid_complain, name, mobj);
	panic("xmm_invalid_complain(%s,0x%x)", name, mobj);
#if 000
	printf("m_invalid_%s from xmm_%s\n",
	       name, mobj->k_kobj->class->c_name);
	if ((int) mobj->refcount < -1000 || (int) mobj->refcount > 1000) {
		printf("deallocated! 0x%x/0x%x=%s\n",
		       mobj->k_kobj,
		       mobj,
		       (char *) mobj->refcount);
	}
#endif
	return KERN_FAILURE;
}

xmm_invalid_complain_reply(name, reply)
	char *name;
	xmm_reply_t reply;
{
	xmm_entry2(xmm_invalid_complain_reply, name, reply);
	panic("xmm_invalid_complain(%s,0x%x)", name, reply);
	/* XXX could print more info here */
	return KERN_FAILURE;
}

m_invalid_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_entry4(m_invalid_init, mobj, pagesize, internal, size);
#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	printf("m_invalid_init from xmm_%s\n", mobj->class->c_name);
	return KERN_FAILURE;
}

m_invalid_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	xmm_entry2(m_invalid_terminate, mobj, release);
#ifdef	lint
	M_TERMINATE(mobj, release);
#endif	lint
	return xmm_invalid_complain("terminate", mobj);
}

void
m_invalid_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_invalid_deallocate, 1);
	panic("m_invalid_deallocate");
}

void
k_invalid_deallocate(kobj)
	xmm_obj_t kobj;
{
	xmm_entry1(k_invalid_deallocate, kobj);
	panic("k_invalid_deallocate");
}

m_invalid_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	xmm_entry4(m_invalid_copy, mobj, offset, length, new_mobj);
#ifdef	lint
	M_COPY(mobj, offset, length, new_mobj);
#endif	lint
	return xmm_invalid_complain("copy", mobj);
}

m_invalid_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_invalid_data_request,
		mobj, offset, length, desired_access);
#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	return xmm_invalid_complain("data_request", mobj);
}

m_invalid_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_invalid_data_unlock, mobj, offset, length, desired_access);
#ifdef	lint
	M_DATA_UNLOCK(mobj, offset, length, desired_access);
#endif	lint
	return xmm_invalid_complain("data_unlock", mobj);
}

m_invalid_data_write_return(mobj, offset, data, length,
			    dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	xmm_entry7(m_invalid_data_write_return,
		mobj, offset, data, length, dirty, kernel_copy, use_routine);
#ifdef	lint
	M_DATA_WRT_RTN(mobj, offset, data, length,
		       dirty, kernel_copy, use_routine);
#endif	lint
	return xmm_invalid_complain("data_write_return", mobj);
}

m_invalid_lock_completed(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry5(m_invalid_lock_completed,
		mobj, offset, length, reply, release);
#ifdef	lint
	M_LOCK_COMPLETED(mobj, offset, length, reply, release);
#endif	lint
	return xmm_invalid_complain_reply("lock_completed", reply);
}

m_invalid_supply_completed(mobj, offset, length, result, error_offset, reply,
			   release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry7(m_invalid_supply_completed,
		mobj, offset, length, result, error_offset, reply, release);
#ifdef	lint
	M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset, reply);
#endif	lint
	return xmm_invalid_complain_reply("supply_completed", reply);
}

m_invalid_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t reply;
	boolean_t release;
{
	xmm_entry5(m_invalid_change_completed,
		mobj, may_cache, copy_strategy, reply, release);
#ifdef	lint
	M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply, release);
#endif	lint
	return xmm_invalid_complain_reply("change_completed", reply);
}

k_invalid_release(kobj)
	xmm_obj_t kobj;
{
	xmm_entry1(k_invalid_release, kobj);
#ifdef	lint
	K_RELEASE(kobj);
#endif
	panic("k_invalid_release");
	return(KERN_FAILURE);
}

k_invalid_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_entry3(k_invalid_data_unavailable, kobj, offset, length);
#ifdef	lint
	K_DATA_UNAVAILABLE(kobj, offset, length);
#endif	lint
	return xmm_invalid_complain("data_unavailable", kobj);
}

k_invalid_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	xmm_entry4(k_invalid_get_attributes,
		kobj, object_ready, may_cache, copy_strategy);
#ifdef	lint
	K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
#endif	lint
	return xmm_invalid_complain("get_attributes", kobj);
}

k_invalid_lock_request(kobj, offset, length, should_clean, should_flush,
		       lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	xmm_entry7(k_invalid_lock_request,
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
	return xmm_invalid_complain("lock_request", kobj);
}

k_invalid_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	xmm_entry4(k_invalid_data_error, kobj, offset, length, error_value);
#ifdef	lint
	K_DATA_ERROR(kobj, offset, length, error_value);
#endif	lint
	return xmm_invalid_complain("data_error", kobj);
}

k_invalid_set_ready(kobj, object_ready, may_cache, write_completions,
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
	xmm_entry7(k_invalid_set_ready,
		kobj,
		object_ready,
		may_cache,
		write_completions,
		copy_strategy,
		cluster_size,
		use_routine);
#ifdef	lint
	K_SET_READY(kobj, object_ready, may_cache, copy_strategy,
		    use_routine, memory_object_name, reply);
#endif	lint
	return xmm_invalid_complain("set_ready", kobj);
}

k_invalid_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	xmm_entry2(k_invalid_destroy, kobj, reason);
#ifdef	lint
	K_DESTROY(kobj, reason);
#endif	lint
	return xmm_invalid_complain("destroy", kobj);
}

k_invalid_data_supply(kobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	xmm_entry7(k_invalid_data_supply,
		kobj, offset, data, length, lock_value, precious, reply);
#ifdef	lint
	K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious, reply);
#endif	lint
	return xmm_invalid_complain("data_supply", kobj);
}


kern_return_t
k_invalid_data_write_completed(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{
	xmm_entry3(k_invalid_data_write_completed, kobj, offset, size);
#ifdef	lint
	K_DATA_WRITE_COMPLETED(kobj, offset, size);
#endif	lint
	return xmm_invalid_complain("data_write_completed", kobj);
}



#if	MACH_KDB
m_invalid_db_print(mobj)
xmm_obj_t	mobj;
{
	iprintf("**INVALID CLASS** xmm_obj=0x%x\n", mobj);
	return 0;
}


k_invalid_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_invalid_db_print(kobj);
}
#endif	/* MACH_KDB */
