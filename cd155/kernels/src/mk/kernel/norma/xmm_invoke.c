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
 * $Log: xmm_invoke.c,v $
 * Revision 1.10  1994/11/18  20:56:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/08/31  21:25:08  mtm
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
 * Revision 1.7  1994/03/29  13:31:30  fritz
 * Add trace statements for XMM trace generation (controlled by
 * bootmagic BOOT_XMM_TRACE). Also, add optional suppression
 * of memory object caching (controlled by bootmagic BOOT_DONT_CACHE).
 *
 * Revision 1.6  1993/09/28  18:05:58  andyp
 * Update for the 1.2 release.
 *
 *
 *	Added {K,M}_DB_PRINT invocations.  [alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:34  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:24  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:41  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:15  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:46:31  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_invoke.c,v
 * Revision 1.2.6.3  1993/04/15  22:46:36  alanl
 * 	Paging flow control (NORMA_VM).  Added new method,
 * 	K_DATA_WRITE_COMPLETED.  Change K_SET_READY for new
 * 	set_attributes parameters.  [sjs]
 * 	[1993/04/15  22:12:17  alanl]
 *
 * Revision 1.2.6.2  1993/03/18  21:15:03  mmp
 * 	Remove test for kr != KERN_ABORTED in M_TERMINATE.
 * 	[1993/03/18  20:26:39  mmp]
 * 
 * Revision 1.2  1992/11/25  01:16:38  robert
 * 	fix history
 * 	[1992/11/09  22:23:32  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:51:03  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added K_RELEASE to clean up objs after a terminate.
 * 
 * 	Revision 1.1  1992/11/05  21:00:28  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.4  92/06/24  18:02:52  jeffreyh
 * 	No more R_ invocations (reply as first arg); all _completed
 * 	routines are now M_ invocations.  No longer pass kobj for
 * 	M_ invocations.  Minor change to xmm_obj_unlink and release
 * 	logic.
 * 	[92/06/24            dlb]
 * 
 * 	Add release logic to M_LOCK_COMPLETED.
 * 	[92/06/09            dlb]
 * 
 * 	use_old_pageout --> use_routine
 * 	[92/06/04            dlb]
 * 
 * Revision 2.1.2.3  92/05/27  00:54:00  jeffreyh
 * 	M_CHANGE_COMPLETED now handles invalid mobjs properly - changed
 * 	macros to make it all work.
 * 	[92/05/20            sjs]
 * 
 * Revision 2.1.2.2  92/03/28  10:13:07  jeffreyh
 * 	Put back in deleted line, some cleanup.
 * 	[92/03/25            sjs]
 * 	Changed data_write to data_write_return and deleted data_return
 * 	 method.  Changed TERMINATE to conditionalize obj_unlink call,
 * 	 and added same logic to CHANGE_COMPLETED to support
 * 	 m_o_change_attributes(). 
 * 	[92/03/20            sjs]
 * 
 * Revision 2.1.2.1  92/02/21  11:27:27  jsb
 * 	Removed reference counting from {M,K}_{REFERENCE,RELEASE} macros.
 * 	Added check for null obj in {M,K}_CALL macros.
 * 	Added call to xmm_obj_unlink to M_TERMINATE.
 * 	[92/02/16  14:17:12  jsb]
 * 
 * 	First checkin.
 * 	[92/02/09  14:17:05  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1992 Carnegie Mellon University
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
 *	File:	norma/xmm_invoke.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1992
 *
 *	Xmm invocation routines.
 */

#include <norma/xmm_obj.h>

#if	!XMM_USE_MACROS

/*
 * I've considered automatically (i.e., in these macros) grabbing and
 * release obj or class locks, but I don't see a good protocol for
 * doing so.
 */

#define	M_REFERENCE\
	kern_return_t kr;\
	register xmm_obj_t m_mobj;\
\
	m_mobj = mobj->m_mobj;\
	if (m_mobj == XMM_OBJ_NULL) {\
		panic("m_invoke: null obj");\
		return KERN_FAILURE;\
	}

#define	M_RELEASE\
	return kr;

#define	K_REFERENCE\
	kern_return_t kr;\
	register xmm_obj_t k_kobj;\
\
	k_kobj = kobj->k_kobj;\
	if (k_kobj == XMM_OBJ_NULL) {\
		panic("k_invoke: null obj");\
		return KERN_FAILURE;\
	}

#define	K_RELEASE\
	return kr;

#if	__STDC__
#define	M_CALL(method)	kr = (*m_mobj->class->m_ ## method)
#define	K_CALL(method)	kr = (*k_kobj->class->k_ ## method)
#else	__STDC__
#define	M_CALL(method)	kr = (*m_mobj->class->m_/**/method)
#define	K_CALL(method)	kr = (*k_kobj->class->k_/**/method)
#endif	__STDC__


#include <norma/xmm_ktrace.h>

#if MACH_KDB
#define M_TRACE_1(method, p1) kt_printf("%x -> m_%s_%s(%x, %x)\n", \
	mobj, m_mobj->class->c_name, method, m_mobj, p1)
#define M_TRACE_3(method, p1, p2, p3) \
	kt_printf("%x -> m_%s_%s(%x, %x, %x, %x)\n", \
	mobj, m_mobj->class->c_name, method, m_mobj, p1, p2, p3)
#define M_TRACE_4(method, p1, p2, p3, p4) \
	kt_printf("%x -> m_%s_%s(%x, %x, %x, %x, %x)\n", \
	mobj, m_mobj->class->c_name, method, m_mobj, p1, p2, p3, p4)
#define M_TRACE_6(method, p1, p2, p3, p4, p5, p6) \
	kt_printf("%x -> m_%s_%s(%x, %x, %x, %x, %x, %x, %x)\n", \
	mobj, m_mobj->class->c_name, method, m_mobj, p1, p2, p3, p4, p5, p6)
#define K_TRACE_0(method) kt_printf("%x -> k_%s_%s(%x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj)
#define K_TRACE_1(method, p1) kt_printf("%x -> k_%s_%s(%x, %x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj, p1)
#define K_TRACE_2(method, p1, p2) \
	kt_printf("%x -> k_%s_%s(%x, %x, %x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj, p1, p2)
#define K_TRACE_3(method, p1, p2, p3) \
	kt_printf("%x -> k_%s_%s(%x, %x, %x, %x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj, p1, p2, p3)
#define K_TRACE_6(method, p1, p2, p3, p4, p5, p6) \
	kt_printf("%x -> k_%s_%s(%x, %x, %x, %x, %x, %x, %x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj, p1, p2, p3, p4, p5, p6)
#define K_TRACE_8(method, p1, p2, p3, p4, p5, p6, p7, p8) \
	kt_printf("%x -> k_%s_%s(%x, %x, %x, %x, %x, %x, %x, %x, %x)\n", \
	kobj, k_kobj->class->c_name, method, k_kobj, p1, p2, p3, p4, p5, p6, p7, p8)
#else
#define M_TRACE_1(method, p1)
#define M_TRACE_3(method, p1, p2, p3)
#define M_TRACE_4(method, p1, p2, p3, p4)
#define M_TRACE_6(method, p1, p2, p3, p4, p5, p6)
#define K_TRACE_0(method)
#define K_TRACE_1(method, p1)
#define K_TRACE_2(method, p1, p2)
#define K_TRACE_3(method, p1, p2, p3) 
#define K_TRACE_6(method, p1, p2, p3, p4, p5, p6) 
#define K_TRACE_8(method, p1, p2, p3, p4, p5, p6, p7, p8) 
#endif
	
M_INIT(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	M_REFERENCE
	M_TRACE_3("init", pagesize, internal, size);
	M_CALL(init)(m_mobj, pagesize, internal, size);
	M_RELEASE
}

kern_return_t
M_TERMINATE(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	M_REFERENCE
	M_TRACE_1("terminate", release);
	M_CALL(terminate)(m_mobj, release);
	if (release)
		xmm_obj_unlink(mobj);
	M_RELEASE
}

kern_return_t
M_COPY(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	M_REFERENCE
	M_TRACE_3("copy", offset, length, new_mobj);
	M_CALL(copy)(m_mobj, offset, length, new_mobj);
	M_RELEASE
}

kern_return_t
M_DATA_REQUEST(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	M_REFERENCE
	M_TRACE_3("data_request", offset, length, desired_access);
	M_CALL(data_request)(m_mobj, offset, length, desired_access);
	M_RELEASE
}

kern_return_t
M_DATA_UNLOCK(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	M_REFERENCE
	M_TRACE_3("data_unlock", offset, length, desired_access);
	M_CALL(data_unlock)(m_mobj, offset, length, desired_access);
	M_RELEASE
}

kern_return_t
M_DATA_WRT_RTN(mobj, offset, data, length, dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	M_REFERENCE
	M_TRACE_6("data_write_return", offset, data, length, dirty, kernel_copy, use_routine);
	M_CALL(data_write_return)(m_mobj, offset, data, length, dirty,
				  kernel_copy, use_routine);
	M_RELEASE
}

kern_return_t
M_LOCK_COMPLETED(mobj, offset, length, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_reply_t reply;
	boolean_t release;
{
	M_REFERENCE
        M_TRACE_4("lock_completed", offset, length, reply, release);
	M_CALL(lock_completed)(m_mobj, offset, length, reply, release);
	if (release)
		xmm_obj_unlink(mobj);
	M_RELEASE
}

kern_return_t
M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset, reply, release)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
	xmm_reply_t reply;
	boolean_t release;
{
	M_REFERENCE
	M_TRACE_6("supply_completed", offset, length, result, error_offset, reply, release);
	M_CALL(supply_completed)(m_mobj, offset, length, result, error_offset,
				 reply, release);
	if (release)
		xmm_obj_unlink(mobj);
	M_RELEASE
}

kern_return_t
M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t mobj;
	boolean_t may_cache;
	memory_object_copy_strategy_t copy_strategy;
	boolean_t release;
	xmm_reply_t reply;
{
	M_REFERENCE
	M_TRACE_4("change_completed", may_cache, copy_strategy, reply, release);
	M_CALL(change_completed)(m_mobj, may_cache, copy_strategy,
				 reply, release);
	if (release)
		xmm_obj_unlink(mobj);
	M_RELEASE
}

kern_return_t
K_DATA_UNAVAILABLE(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	K_REFERENCE
	K_TRACE_2("data_unavailable", offset, length);
	K_CALL(data_unavailable)(k_kobj, offset, length);
	K_RELEASE
}

kern_return_t
K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	K_REFERENCE
	K_TRACE_3("get_attributes", object_ready, may_cache, copy_strategy);
	K_CALL(get_attributes)(k_kobj, object_ready, may_cache, copy_strategy);
	K_RELEASE
}

kern_return_t
K_LOCK_REQUEST(kobj, offset, length, should_clean, should_flush, lock_value,
	       reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	K_REFERENCE
	K_TRACE_6("lock_request", offset, length, should_clean, should_flush, lock_value, reply);
	K_CALL(lock_request)(k_kobj, offset, length, should_clean,
			     should_flush, lock_value, reply);
	K_RELEASE
}

kern_return_t
K_DATA_ERROR(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	K_REFERENCE
	K_TRACE_3("data_error", offset, length, error_value);
	K_CALL(data_error)(k_kobj, offset, length, error_value);
	K_RELEASE
}

kern_return_t
K_SET_READY(kobj, object_ready, may_cache, write_completions,
	    copy_strategy, cluster_size, use_routine,
	    memory_object_name, reply)
	xmm_obj_t	kobj;
	boolean_t	object_ready;
	boolean_t	may_cache;
        boolean_t	write_completions;
	memory_object_copy_strategy_t copy_strategy;
        vm_size_t	cluster_size;
	int		use_routine;
	ipc_port_t	memory_object_name;
	xmm_reply_t	reply;
{
	K_REFERENCE
	K_TRACE_8("set_ready", object_ready, may_cache, write_completions, copy_strategy, cluster_size, use_routine, memory_object_name, reply);
	K_CALL(set_ready)(k_kobj, object_ready, may_cache, write_completions,
			  copy_strategy, cluster_size, use_routine,
			  memory_object_name, reply);
	K_RELEASE
}

kern_return_t
K_DESTROY(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	K_REFERENCE
	K_TRACE_1("data_error", reason);
	K_CALL(destroy)(k_kobj, reason);
	K_RELEASE
}

kern_return_t
K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	K_REFERENCE
	K_TRACE_6("data_supply", offset, data, length, lock_value, precious, reply);
	K_CALL(data_supply)(k_kobj, offset, data, length, lock_value, precious,
			    reply);
	K_RELEASE
}

kern_return_t
_K_DESTROY(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	return (*kobj->class->k_destroy)(kobj, reason);
}

kern_return_t
K_DATA_WRITE_COMPLETED(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{
	K_REFERENCE
	K_TRACE_2("data_write_completed", offset, size);
	K_CALL(data_write_completed)(k_kobj, offset, size);
	K_RELEASE
}



#if	MACH_KDB
kern_return_t
M_DB_PRINT(mobj)
xmm_obj_t mobj;
{
	M_REFERENCE
	M_CALL(db_print)(m_mobj);
	M_RELEASE
}


kern_return_t
K_DB_PRINT(kobj)
xmm_obj_t	kobj;
{
	K_REFERENCE
	K_CALL(db_print)(k_kobj);
	K_RELEASE
}
#endif	/* MACH_KDB */

#undef K_RELEASE
kern_return_t
K_RELEASE(kobj)
	xmm_obj_t kobj;
{
	K_REFERENCE
	K_TRACE_0("release");
	K_CALL(release)(k_kobj);
	xmm_obj_unlink(kobj);
	return(kr);
}

#else	!XMM_USE_MACROS
#endif	!XMM_USE_MACROS
