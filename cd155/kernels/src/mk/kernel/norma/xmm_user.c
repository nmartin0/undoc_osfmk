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
 * $Log: xmm_user.c,v $
 * Revision 1.13  1994/11/18  20:57:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/08/31  21:25:20  mtm
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
 * Revision 1.10.2.1  1994/08/08  23:54:48  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.11  1994/08/08  19:34:32  andyp
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
 * Revision 1.10  1994/07/12  19:25:50  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/03/29  13:31:57  fritz
 * Add trace statements for XMM trace generation (controlled by
 * bootmagic BOOT_XMM_TRACE). Also, add optional suppression
 * of memory object caching (controlled by bootmagic BOOT_DONT_CACHE).
 *
 * Revision 1.7  1993/09/28  18:07:26  andyp
 * Update for the 1.2 release.
 *
 *
 *	27-Sep-93 [andyp@ssd.intel.com]
 *	Fully synchronous object termination can deadlock in
 *	the local case.
 *
 *	23-Sep-93 [alanl@osf.org]
 *	Enabled fully synchronous object termination, hence no need
 *	for code tracking terminations.
 *
 *	- Added PENDING_TRACKING fields to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Added option for fully synchronous object termination,
 *	currently disabled.  For now, object termination is
 *	semi-synchronous.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.6  1993/07/22  02:22:11  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.5  1993/06/30  22:52:57  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:41:14  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:47:08  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:37  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:25:11  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_user.c,v
 * Revision 1.2.5.4  1993/05/23  18:07:46  alanl
 * 	The modwc field should be preserved rather than forced
 * 	to TRUE in memory_object_change_attributes.  Also:  when
 * 	initializing internal objects, assume that the default
 * 	pager generates data_write_completed messages.  [sjs]
 * 	Also:  memory_object_ready must propagate modwc through
 * 	the stack, rather than forcing it to TRUE.  [mmp, alanl]
 * 	[1993/05/23  17:48:03  alanl]
 *
 * Revision 1.2.5.3  1993/05/03  19:03:38  dwm
 * 	Add modwc field to decide whether to maintain p_count,
 * 	maintaining backwards compatibility with pagers that don't
 * 	use paging flow control.  [ mmp/alanl ]
 * 	[1993/05/03  18:40:45  dwm]
 * 
 * Revision 1.2.5.2  1993/04/15  22:48:11  alanl
 * 	Paging flow control (NORMA_VM).  Added new interface,
 * 	m_o_data_write_completed, changed m_o_set_attributes
 * 	and added backwards compatibility routine.  The p_count
 * 	field is temporary, for debugging object termination.  [sjs]
 * 	[1993/04/15  22:13:31  alanl]
 * 
 * Revision 1.2  1992/11/25  01:17:42  robert
 * 	fix history
 * 	[1992/11/09  22:32:58  robert]
 * 
 * 	integrate changes below for norma_14
 * 
 * 	 If an object was TEMPORARY, also change may_cache to FALSE.
 * 	 92/10/21 sjs
 * 
 * 	 Convert unsupported TEMPORARY copy strategy to NONE.
 * 	 92/10/15 sjs
 * 	[1992/11/09  16:53:08  robert]
 * 
 * Revision 1.1  1992/11/05  21:00:51  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.4.3.8  92/09/15  17:35:43  jeffreyh
 * 	Change inited field to state, add terminated state, add assertions
 * 	for things that can't happen when an object is terminated.
 * 	[92/09/11            dlb]
 * 	Bounds check error_value of m_o_data_error to avoid conflicts
 * 	with the kernel return values.
 * 	[92/07/28            sjs]
 * 
 * Revision 2.4.3.7  92/06/24  18:03:38  jeffreyh
 * 	Changed routines to meet new interfaces; changes for reply
 * 	structure change.
 * 	[92/06/18            sjs]
 * 
 * 	Add release logic to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	Support termintion of uninitialized object.  This means
 * 	that the object was never used, so don't tell the pager.
 * 	Remove k_ prefix from memory_object_create.
 * 	[92/06/08            dlb]
 * 
 * 	Convert use_old_pageout to use_routine.  Add data_initialize case
 * 	to m_user_data_write_return.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.4.3.6  92/05/27  01:01:20  jeffreyh
 * 	Add protection argument check to memory_object_lock_request,
 * 	[92/04/29            dlb]
 * 
 * Revision 2.4.3.5  92/03/28  10:13:41  jeffreyh
 * 	Change data_write to data_write_return and conditionally call
 * 	 memory_object_write or memory_object_data_return.  Added support
 * 	 fo m_o_change_attributes().
 * 	[92/03/20            sjs]
 * 
 * Revision 2.4.3.4  92/03/03  16:24:12  jeffreyh
 * 	Pick up fix from jsb to call K_SET_READY with MAY_CACHE_FALSE. This
 * 	fixes a memory leak.
 * 	[92/02/26  12:25:25  jeffreyh]
 * 
 * Revision 2.4.3.3  92/02/21  11:28:30  jsb
 * 	Copy send right to memory object, so that each kernel can release its
 * 	own send right to memory object.
 * 	[92/02/20  13:57:24  jsb]
 * 
 * 	Reference mobj on port to mobj conversion; release when done.
 * 	[92/02/20  10:54:28  jsb]
 * 
 * 	Removed initialized flag and corresponding code to detect multiple
 * 	initialization, since xmm_kobj_link now handles such detection.
 * 	[92/02/18  08:47:44  jsb]
 * 
 * 	Changed reply->mobj to reply->kobj.
 * 	[92/02/16  18:22:26  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changes for reference counting termination protocol.
 * 	[92/02/16  15:54:47  jsb]
 * 
 * 	Removed is_internal_memory_object_call; xmm internal objects now
 * 	create their own xmm stacks and objects and thus will never be
 * 	seen here. Use new MEMORY_OBJECT_COPY_TEMPORARY strategy instead
 * 	of MEMORY_OBJECT_COPY_NONE for setting internal objects ready.
 * 	Removed ipc_kobject_set of memory_object; this was a hack for when
 * 	xmm_server.c stored a pointer to the svm mobj stack in the
 * 	memory_object kobject. We now use a separate port (the xmm object
 * 	port) for this association, and break that association elsewhere.
 * 	[92/02/11  11:22:23  jsb]
 * 
 * 	Remove vm_object_lookup_by_pager.
 * 	[92/02/10  09:41:36  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	Let mig do automatic conversion of memory_control port into user obj.
 * 	Cleaned up memory_object_create support.
 * 	[92/02/09  14:01:13  jsb]
 * 
 * Revision 2.4.3.2  92/01/21  21:55:05  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:48:19  jsb]
 * 
 * Revision 2.4.3.1  92/01/03  16:39:03  jsb
 * 	Picked up temporary fix to m_user_terminate from dlb.
 * 	[91/12/24  14:31:09  jsb]
 * 
 * Revision 2.4  91/08/28  11:16:30  jsb
 * 	Added definition for xxx_memory_object_lock_request, and temporary
 * 	stubs for data_supply, object_ready, and change_attributes.
 * 	[91/08/16  14:22:37  jsb]
 * 
 * 	Added check for internal memory objects to xmm_user_create.
 * 	[91/08/15  10:14:19  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:46  jsb
 * 	Collect garbage. Support memory_object_create.
 * 	Disassociate kobj from memory_control before calling
 * 	memory_object_terminate to prevent upcalls on terminated kobj.
 * 	[91/06/29  15:51:50  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:48  jsb
 * 	Renamed xmm_vm_object_lookup.
 * 	[91/06/17  13:20:06  jsb]
 * 
 * 	First checkin.
 * 	[91/06/17  11:02:47  jsb]
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
 *	File:	norma/xmm_user.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Interface between memory managers and xmm system.
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

#if	PARAGON860
boolean_t	do_not_cache_memory_objects=FALSE;
#endif

#define	SYNCHRONOUS_TERMINATION	0

/*
 * Since we ALWAYS have an SVM module above us,
 * we NEVER have more than one memory_control per memory_object.
 * Thus we can combine mobj and kobj.
 */

struct mobj {
	struct xmm_obj	obj;
	ipc_port_t	memory_object;
	ipc_port_t	memory_control;
	ipc_port_t	memory_object_name;
	int		state;
	int		p_count;
	boolean_t	modwc;		/* using write completions */
#if	PENDING_TRACKING
	int		p_seen;		/* writes that came through */
	int		p_matched;	/* completeds that returned */
	int		p_terminated;	/* writes w/o completeds */
#endif	/* PENDING_TRACKING */
};

#define XMM_USER_STATE_CREATED		0
#define XMM_USER_STATE_INITED		1
#define XMM_USER_STATE_TERMINATED	2

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_user_deallocate		m_interpose_deallocate

#define	k_user_data_unavailable		k_invalid_data_unavailable
#define	k_user_get_attributes		k_invalid_get_attributes
#define	k_user_lock_request		k_invalid_lock_request
#define	k_user_data_error		k_invalid_data_error
#define	k_user_set_ready		k_invalid_set_ready
#define	k_user_destroy			k_invalid_destroy
#define	k_user_data_supply		k_invalid_data_supply
#define	k_user_release			k_invalid_release
#define	k_user_data_write_completed	k_invalid_data_write_completed

extern m_user_db_print();
extern k_user_db_print();

xmm_decl(user, "user", sizeof(struct mobj));

#include <norma/xmm_ktrace.h>

#if MACH_KDB
#define M_TRACE_3(obj, method,p1, p2, p3) \
	kt_printf("%x -> memory_object_%s(%x[?], %x, %x)\n",\
	obj, method, p1, p2, p3)
#define M_TRACE_4(obj, method,p1, p2, p3, p4) \
	kt_printf("%x -> memory_object_%s(%x[?], %x, %x, %x)\n",\
	obj, method, p1, p2, p3, p4)
#define M_TRACE_5(obj, method,p1, p2, p3, p4, p5) \
	kt_printf("%x -> memory_object_%s(%x[?], %x, %x, %x, %x)\n",\
	obj, method, p1, p2, p3, p4, p5)
#define M_TRACE_6(obj, method,p1, p2, p3, p4, p5, p6) \
	kt_printf("%x -> memory_object_%s(%x[?], %x, %x, %x, %x, %X)\n",\
	obj, method, p1, p2, p3, p4, p5, p6)
#define M_TRACE_7(obj, method,p1, p2, p3, p4, p5, p6, p7) \
	kt_printf("%x -> memory_object_%s(%x[?], %x, %x, %x, %x, %x, %x)\n",\
	obj, method, p1, p2, p3, p4, p5, p6, p7)
#else
#define M_TRACE_3(obj, method, p1, p2, p3)
#define M_TRACE_4(obj, method, p1, p2, p3, p4)
#define M_TRACE_5(obj, method, p1, p2, p3, p4, p5)
#define M_TRACE_6(obj, method, p1, p2, p3, p4, p5, p6)
#define M_TRACE_7(obj, method, p1, p2, p3, p4, p5, p6, p7)
#endif


/*
 * Translate from memory_control to kobj. Take a reference.
 */
xmm_obj_t
xmm_kobj_lookup(memory_control)
	ipc_port_t memory_control;
{
	register xmm_obj_t kobj;

	xmm_entry1(xmm_kobj_lookup, memory_control);

	if (memory_control == IP_NULL) {
		return XMM_OBJ_NULL;
	}
	ip_lock(memory_control);
	if (ip_kotype(memory_control) == IKOT_PAGING_REQUEST) {
		kobj = (xmm_obj_t) memory_control->ip_kobject;
		xmm_obj_reference(kobj);
	} else {
		kobj = XMM_OBJ_NULL;
	}
	ip_unlock(memory_control);
	return kobj;
}

/*
 * We create our own memory_control and memory_object_name ports.
 * This is easier and less confusing than each kernel allocating
 * its own ports, particularly for name ports, since everyone should
 * see the same name port for the same object.
 */
kern_return_t
xmm_user_create(memory_object, new_mobj)
	ipc_port_t memory_object;
	xmm_obj_t *new_mobj;
{
	ipc_port_t memory_control;
	ipc_port_t memory_object_name;
	kern_return_t kr;
	xmm_obj_t mobj;

	xmm_entry2(xmm_user_create, memory_object, new_mobj);

	/*
	 * Allocate request port.
	 */
	memory_control = ipc_port_alloc_kernel();
	if (memory_control == IP_NULL) {
		panic("xmm_user_create: memory_control");
	}

	/*
	 * Allocate name port.
	 */
	memory_object_name = ipc_port_alloc_kernel();
	if (memory_object_name == IP_NULL) {
		panic("xmm_user_create: memory_object_name");
	}

	/*
	 * Allocate mobj.
	 */
	kr = xmm_obj_allocate(&user_class, XMM_OBJ_NULL, &mobj);
	if (kr != KERN_SUCCESS) {
		return kr;
	}

	MOBJ->memory_object = ipc_port_copy_send(memory_object);
	MOBJ->memory_control = memory_control;
	MOBJ->memory_object_name = memory_object_name;
	MOBJ->state = XMM_USER_STATE_CREATED;
	MOBJ->modwc = FALSE;	/* XXX alanl XXX */
	MOBJ->p_count = 0;
#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif

	/*
	 * Grab a reference for mobj and associate it with memory_control port.
	 */
	xmm_obj_reference(mobj);
	ipc_kobject_set(memory_control, (ipc_kobject_t) mobj,
			IKOT_PAGING_REQUEST);
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

m_user_init(mobj, pagesize, internal, size)
	xmm_obj_t mobj;
	vm_size_t pagesize;
	boolean_t internal;
	vm_size_t size;
{
	xmm_entry4(m_user_init, mobj, pagesize, internal, size);

#ifdef	lint
	M_INIT(mobj, pagesize, internal, size);
#endif	lint
	assert(MOBJ->state == XMM_USER_STATE_CREATED);
	MOBJ->state = XMM_USER_STATE_INITED;

	assert(MOBJ->memory_object != IP_NULL);
	if (internal) {
		/* acquire a naked send right for the default pager */
		ipc_port_t default_pager = memory_manager_default_reference();

		M_TRACE_6(mobj, "create", default_pager, MOBJ->memory_object, size, MOBJ->memory_control, MOBJ->memory_object_name, PAGE_SIZE);
		/* consumes the naked send right for default_pager */
		(void) memory_object_create(default_pager,
					      MOBJ->memory_object, size,
					      MOBJ->memory_control,
					      MOBJ->memory_object_name,
					      PAGE_SIZE);

		/* default pager will do modwc */
		MOBJ->modwc = TRUE;

		/* call set_ready, since default pager won't */
		return K_SET_READY(mobj, OBJECT_READY_TRUE, MAY_CACHE_FALSE,
				   MOBJ->modwc, MEMORY_OBJECT_COPY_TEMPORARY,
				   PAGE_SIZE, XMM_USE_DATA_WRITE,
				   ipc_port_make_send(MOBJ->
						      memory_object_name),
				   XMM_REPLY_NULL);
	} else {
		M_TRACE_4(mobj, "init", MOBJ->memory_object, MOBJ->memory_control, MOBJ->memory_object_name, PAGE_SIZE);
		(void) memory_object_init(MOBJ->memory_object,
					  MOBJ->memory_control,
					  MOBJ->memory_object_name,
					  PAGE_SIZE);
	}
	return KERN_SUCCESS;
}

m_user_terminate(mobj, release)
	xmm_obj_t mobj;
	boolean_t release;
{
	kern_return_t kr;

	xmm_entry2(m_user_terminate, mobj, release);

#ifdef	lint
	M_TERMINATE(mobj, release);
#endif	lint
	assert(MOBJ->state != XMM_USER_STATE_TERMINATED);
#if	!SYNCHRONOUS_TERMINATION
	if (MOBJ->modwc) {
#if	PENDING_TRACKING
		MOBJ->p_terminated += MOBJ->p_count;
#endif	/* PENDING_TRACKING */
		MOBJ->p_count = 0;
	}
#else	/* !SYNCHRONOUS_TERMINATION */
	assert(!MOBJ->modwc || MOBJ->p_count == 0);
#endif	/* !SYNCHRONOUS_TERMINATION */
	ipc_kobject_set(MOBJ->memory_control, IKO_NULL, IKOT_NONE);
	if (MOBJ->state == XMM_USER_STATE_INITED) {
		MOBJ->state = XMM_USER_STATE_TERMINATED;
		M_TRACE_3(mobj, "terminate", MOBJ->memory_object, MOBJ->memory_control, MOBJ->memory_object_name);
		kr = memory_object_terminate(MOBJ->memory_object,
				     MOBJ->memory_control,
				     MOBJ->memory_object_name);
	}
	else {
		/*
		 *	Terminate without preceding init.
		 *	Pager never knew about this, so don't tell it now
		 *	Undo actions of m_user_create.
		 */
		MOBJ->state = XMM_USER_STATE_TERMINATED;
		ipc_port_release_send(MOBJ->memory_object);
		ipc_port_dealloc_kernel(MOBJ->memory_control);
		ipc_port_dealloc_kernel(MOBJ->memory_object_name);
	}

	if (release)
		xmm_obj_release(mobj);
	return kr;
}

m_user_copy(mobj, offset, length, new_mobj)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	xmm_obj_t new_mobj;
{
	xmm_entry4(m_user_copy, mobj, offset, length, new_mobj);

#ifdef	lint
	M_COPY(mobj, offset, length, new_mobj);
#endif	lint
	panic("m_user_copy");
	/* NOTREACHED */
}

m_user_data_request(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_user_data_request, mobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_REQUEST(mobj, offset, length, desired_access);
#endif	lint
	assert(MOBJ->state != XMM_USER_STATE_TERMINATED);
	M_TRACE_5(mobj, "data_request", MOBJ->memory_object, MOBJ->memory_control, offset, length, desired_access);
	return memory_object_data_request(MOBJ->memory_object,
					  MOBJ->memory_control,
					  offset,
					  length,
					  desired_access);
}

m_user_data_unlock(mobj, offset, length, desired_access)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry4(m_user_data_unlock, mobj, offset, length, desired_access);

#ifdef	lint
	M_DATA_UNLOCK(mobj, offset, length, desired_access);
#endif	lint
	assert(MOBJ->state != XMM_USER_STATE_TERMINATED);
	M_TRACE_5(mobj, "data_unlock", MOBJ->memory_object, MOBJ->memory_control, offset, length, desired_access);
	return memory_object_data_unlock(MOBJ->memory_object,
					 MOBJ->memory_control,
					 offset,
					 length,
					 desired_access);
}

m_user_data_write_return(mobj, offset, data, length,
			 dirty, kernel_copy, use_routine)
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t dirty;
	boolean_t kernel_copy;
	int use_routine;
{
	kern_return_t kr;

	xmm_entry7(m_user_data_write_return,
		mobj, offset, data, length, dirty, kernel_copy, use_routine);

#ifdef	lint
	M_DATA_WRT_RTN(mobj, offset, data, length, dirty, kernel_copy,
			use_routine);
#endif	lint
	assert(MOBJ->state != XMM_USER_STATE_TERMINATED);

	if (MOBJ->modwc) {
		assert(MOBJ->p_count >= 0);
		MOBJ->p_count++;
#if	PENDING_TRACKING
		MOBJ->p_seen++;
#endif
	}
	switch (use_routine) {
	case XMM_USE_DATA_WRITE:
		M_TRACE_5(mobj, "data_write", MOBJ->memory_object, MOBJ->memory_control, offset, data, length);
		kr = memory_object_data_write(MOBJ->memory_object,
					MOBJ->memory_control,
					offset,
					data,
					length);
		break;
	case XMM_USE_DATA_RETURN:
		M_TRACE_7(mobj, "data_return", MOBJ->memory_object, MOBJ->memory_control, offset, data, length, dirty, kernel_copy);
		kr = memory_object_data_return(MOBJ->memory_object,
					MOBJ->memory_control,
					offset,
					data,
					length,
					dirty,
					kernel_copy);
		break;
	case XMM_USE_DATA_INITIALIZE:
		M_TRACE_5(mobj, "data_initialize", MOBJ->memory_object, MOBJ->memory_control, offset, data, length);
		kr = memory_object_data_initialize(MOBJ->memory_object,
					MOBJ->memory_control,
					offset,
					data,
					length);
		break;
	default:
		panic("m_user_data_write_return: Bad routine");
	}

	return kr;
}

m_user_lock_completed(kobj, offset, length, reply, release)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	length;
	xmm_reply_t	reply;
	boolean_t	release;
{
	kern_return_t kr;

	xmm_entry5(m_user_lock_completed, kobj, offset, length, reply, release);

#ifdef	lint
	M_LOCK_COMPLETED(kobj, offset, length, reply, release);
#endif	lint
	assert(kobj->class == &user_class);
	M_TRACE_5(kobj, "data_lock_completed", reply->reply_to.port, reply->reply_to_type, KOBJ->memory_control, offset, length);
	kr = memory_object_lock_completed(reply->reply_to.port,
					  reply->reply_to_type,
					  KOBJ->memory_control,
					  offset,
					  length);
	xmm_reply_deallocate(reply);
	return kr;
}

m_user_supply_completed(kobj, offset, length, result, error_offset, reply,
			release)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	xmm_reply_t reply;
	vm_offset_t error_offset;
	boolean_t release;
{
	kern_return_t kr;

	xmm_entry7(m_user_supply_completed,
		kobj, offset, length, result, error_offset, reply, release);

#ifdef	lint
	M_SUPPLY_COMPLETED(kobj, offset, length, result, error_offset, reply);
#endif	lint
	assert(kobj->class == &user_class);
	M_TRACE_7(kobj, "supply_completed", reply->reply_to.port, reply->reply_to_type, KOBJ->memory_control, offset, length, result, error_offset);
	kr = memory_object_supply_completed(reply->reply_to.port,
					    reply->reply_to_type,
					    KOBJ->memory_control,
					    offset,
					    length,
					    result,
					    error_offset);
	if (release)
		xmm_obj_release(kobj);
	xmm_reply_deallocate(reply);
	return kr;
}

m_user_change_completed(mobj, may_cache, copy_strategy, reply, release)
	xmm_obj_t	mobj;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
	xmm_reply_t	reply;
	boolean_t	release;
{
	kern_return_t kr;

	xmm_entry5(m_user_change_completed,
		mobj, may_cache, copy_strategy, reply, release);

#ifdef	lint
	M_CHANGE_COMPLETED(mobj, may_cache, copy_strategy, reply, release);
#endif	lint
	M_TRACE_4(mobj, "change_completed", reply->reply_to.port, reply->reply_to_type, may_cache, copy_strategy);
	kr = memory_object_change_completed(reply->reply_to.port,
					    reply->reply_to_type,
					    may_cache,
					    copy_strategy);
	xmm_reply_deallocate(reply);
	/*
	 * Dont release in the case of a null mobj 
	 */
	if (release && mobj)
		xmm_obj_release(mobj);
	return kr;
}

kern_return_t
memory_object_data_provided(kobj, offset, data, length, lock_value)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	pointer_t	data;
	unsigned int	length;
	vm_prot_t	lock_value;
{
	kern_return_t kr;

	xmm_entry5(memory_object_data_provided,
		kobj, offset, data, length, lock_value);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = K_DATA_SUPPLY(kobj, offset, data, length, lock_value,
			   PRECIOUS_FALSE, XMM_REPLY_NULL);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_data_unavailable(kobj, offset, length)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	length;
{
	kern_return_t kr;

	xmm_entry3(memory_object_data_unavailable, kobj, offset, length);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = K_DATA_UNAVAILABLE(kobj, offset, length);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t	kobj;
	boolean_t	*object_ready;
	boolean_t	*may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	kern_return_t kr;

	xmm_entry4(memory_object_get_attributes,
		kobj, object_ready, may_cache, copy_strategy);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_lock_request(kobj, offset, length, should_return, should_flush,
			   prot, reply_to, reply_to_type)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	length;
	int		should_return;
	boolean_t	should_flush;
	vm_prot_t	prot;
	ipc_port_t	reply_to;
	mach_msg_type_name_t reply_to_type;
{
	xmm_reply_t reply;
	kern_return_t kr;

	/*xmm_entry8(...);*/
	xmm_entry7(memory_object_lock_request,
		kobj,
		offset,
		length,
		should_return,
		should_flush,
		prot,
		reply_to);

	if (kobj == XMM_OBJ_NULL ||
		((prot & ~VM_PROT_ALL) != 0 && prot != VM_PROT_NO_CHANGE)) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = xmm_reply_allocate(kobj, reply_to, reply_to_type, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_LOCK_REQUEST(kobj, offset, length, should_return, should_flush,
			    prot, reply);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
xxx_memory_object_lock_request(kobj, offset, size, should_clean, should_flush,
			       prot, reply_to, reply_to_type)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	prot;
	ipc_port_t	reply_to;
	mach_msg_type_name_t reply_to_type;
{
	int should_return;

	/*xmm_entry8(...);*/
	xmm_entry7(xxx_memory_object_lock_request,
		kobj,
		offset,
		size,
		should_clean,
		should_flush,
		prot,
		reply_to);
	if (should_clean) {
		should_return = MEMORY_OBJECT_RETURN_DIRTY;
	} else {
		should_return = MEMORY_OBJECT_RETURN_NONE;
	}
	return memory_object_lock_request(kobj, offset, size, should_return,
					  should_flush, prot, reply_to,
					  reply_to_type);
}

kern_return_t
memory_object_data_error(kobj, offset, length, error_value)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	length;
	kern_return_t	error_value;
{
	kern_return_t kr;

	xmm_entry4(memory_object_data_error, kobj, offset, length, error_value);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	if (error_value < KERN_RETURN_MAX)
		error_value = 0;
	kr = K_DATA_ERROR(kobj, offset, length, error_value);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
xxx_memory_object_set_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t	kobj;
	boolean_t	object_ready;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
{
	kern_return_t kr;

	xmm_entry4(xxx_memory_object_set_attributes,
		kobj, object_ready, may_cache, copy_strategy);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
        if (do_not_cache_memory_objects) 
                may_cache = FALSE;
	if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		copy_strategy = MEMORY_OBJECT_COPY_NONE;
		may_cache = FALSE;
	}
	KOBJ->modwc = FALSE;
	kr = K_SET_READY(kobj, object_ready, may_cache, KOBJ->modwc,
			 copy_strategy, PAGE_SIZE,
			 XMM_USE_DATA_WRITE,
			 ipc_port_make_send(KOBJ->memory_object_name),
			 XMM_REPLY_NULL);
	assert(KOBJ->modwc == FALSE);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_set_attributes(kobj, object_ready, may_cache, write_completions,
			     copy_strategy, cluster_size)
	xmm_obj_t	kobj;
	boolean_t	object_ready;
	boolean_t	may_cache;
        boolean_t	write_completions;
	memory_object_copy_strategy_t copy_strategy;
        vm_size_t	cluster_size;
{
	kern_return_t kr;

	xmm_entry6(memory_object_set_attributes,
		kobj,
		object_ready,
		may_cache,
		write_completions,
		copy_strategy,
		cluster_size);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	if (do_not_cache_memory_objects)
		may_cache = FALSE;
	if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		copy_strategy = MEMORY_OBJECT_COPY_NONE;
		may_cache = FALSE;
	}
	kr = K_SET_READY(kobj, object_ready, may_cache, write_completions,
			 copy_strategy, cluster_size,
			 XMM_USE_DATA_WRITE,
			 ipc_port_make_send(KOBJ->memory_object_name),
			 XMM_REPLY_NULL);
	KOBJ->modwc = write_completions;
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_destroy(kobj, reason)
	xmm_obj_t	kobj;
	kern_return_t	reason;
{
	kern_return_t kr;

	xmm_entry2(memory_object_destroy, kobj, reason);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = K_DESTROY(kobj, reason);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_data_supply(kobj, offset, data, length, lock_value, precious,
			  reply_to, reply_to_type)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	pointer_t	data;
	unsigned int	length;
	vm_prot_t	lock_value;
	boolean_t	precious;
	ipc_port_t	reply_to;
	mach_msg_type_name_t reply_to_type;
{
	xmm_reply_t reply;
	kern_return_t kr;
	
	/*xmm_entry8(...);*/
	xmm_entry7(memory_object_data_supply,
		kobj, offset, data, length, lock_value, precious, reply_to);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	kr = xmm_reply_allocate(kobj, reply_to, reply_to_type, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious,
			   reply);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_ready(kobj, may_cache, copy_strategy)
	xmm_obj_t	kobj;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
{
	kern_return_t kr;

	xmm_entry3(memory_object_ready, kobj, may_cache, copy_strategy);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
	if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		copy_strategy = MEMORY_OBJECT_COPY_NONE;
		may_cache = FALSE;
	}
	kr = K_SET_READY(kobj, OBJECT_READY_TRUE, may_cache, KOBJ->modwc,
			 copy_strategy, PAGE_SIZE,
			 XMM_USE_DATA_RETURN,
			 ipc_port_make_send(KOBJ->memory_object_name),
			 XMM_REPLY_NULL);
	xmm_obj_release(kobj);
	return kr;
}

kern_return_t
memory_object_change_attributes(kobj, may_cache, copy_strategy, reply_to,
				reply_to_type)
	xmm_obj_t	kobj;
	boolean_t	may_cache;
	memory_object_copy_strategy_t copy_strategy;
	ipc_port_t	reply_to;
	mach_msg_type_name_t reply_to_type;
{
	xmm_reply_t reply;
	kern_return_t kr;

	xmm_entry5(memory_object_change_attributes,
		kobj, may_cache, copy_strategy, reply_to, reply_to_type);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}
        if (do_not_cache_memory_objects) 
                may_cache = FALSE;
	if (copy_strategy == MEMORY_OBJECT_COPY_TEMPORARY) {
		copy_strategy = MEMORY_OBJECT_COPY_NONE;
		may_cache = FALSE;
	}
	kr = xmm_reply_allocate(kobj, reply_to, reply_to_type, &reply);
	if (kr != KERN_SUCCESS) {
		return kr;
	}
	kr = K_SET_READY(kobj, OBJECT_READY_TRUE, may_cache, KOBJ->modwc,
			 copy_strategy, PAGE_SIZE, XMM_USE_DATA_RETURN,
			 ipc_port_make_send(KOBJ->memory_object_name), reply);
	xmm_obj_release(kobj);
	return kr;
}


kern_return_t
memory_object_data_write_completed(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{
	kern_return_t kr;
	
	xmm_entry3(memory_object_data_write_completed, kobj, offset, size);

	if (kobj == XMM_OBJ_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	if (KOBJ->modwc) {
		assert(KOBJ->p_count > 0);
		KOBJ->p_count--;
#if	PENDING_TRACKING
		KOBJ->p_matched++;
#endif
	}
	kr = K_DATA_WRITE_COMPLETED(kobj, offset, size);
	xmm_obj_release(kobj);
	return kr;
}


#if	MACH_KDB
m_user_db_print(mobj)
xmm_obj_t	mobj;
{
	char		*state_name;
	extern char	*db_bool_str();

	iprintf("memory_object=0x%x [%d kmsgs]\n",
		MOBJ->memory_object, db_port_kmsg_count(MOBJ->memory_object));
	iprintf("memory_control=0x%x [%d kmsgs]\n",
		MOBJ->memory_control,
		db_port_kmsg_count(MOBJ->memory_control));
	switch (MOBJ->state) {
	    case XMM_USER_STATE_CREATED:
		state_name = "CREATED";
		break;
	    case XMM_USER_STATE_INITED:
		state_name = "INITED";
		break;
	    case XMM_USER_STATE_TERMINATED:
		state_name = "TERMINATED";
		break;
	    default:
		state_name = "**BOGUS**";
		break;
	}
	iprintf("object_name=0x%x state=%s[%d] modwc=%s p_count=0x%x\n",
		MOBJ->memory_object_name, state_name, MOBJ->state,
		db_bool_str(MOBJ->modwc),
		MOBJ->p_count);
#if	PENDING_TRACKING
	iprintf("p_seen=0x%x p_matched=0x%x p_terminated=0x%x\n",
		MOBJ->p_seen, MOBJ->p_matched, MOBJ->p_terminated);
#endif	/* PENDING_TRACKING */
	return 0;
}

k_user_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_user_db_print(kobj);
}
#else	/* MACH_KDB */
m_user_db_print(mobj) xmm_obj_t	mobj; { }
k_user_db_print(kobj) xmm_obj_t	kobj; { }
#endif	/* MACH_KDB */
