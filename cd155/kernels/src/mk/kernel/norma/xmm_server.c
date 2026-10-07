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
 * $Log: xmm_server.c,v $
 * Revision 1.13  1994/11/18  20:56:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/10/11  17:13:01  tnt
 *  PTS #: 8754
 *  Mandatory?:  Yes
 *  Description: Ignore unexpected data-write-completed notifications rather
 *               than panicking.  No longer make pageout reservations for
 *               local pageout requests.
 *  Reviewer(s): Stefan Tritscher, Andy (in review)
 *  Risk:        Medium
 *  Testing:     Developer tests, Message passing EATs, Parallel SATs.
 *  Module(s):   kernel/vm/vm_object.c
 *               kernel/vm/vm_pageout.c
 *               kernel/vm/memory_object.c
 *               kernel/norma/xmm_server.c
 *
 * Removed assertions that are no longer valid.
 *
 * Revision 1.11  1994/08/31  21:25:12  mtm
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
 * Revision 1.9.2.1  1994/08/08  23:54:07  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.10  1994/08/08  19:34:07  andyp
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
 * Revision 1.9  1994/07/12  19:25:30  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.8.4.5  1994/06/20  17:15:20  rkl
 *  Removed BORG capability.
 *
 * Revision 1.8.4.4  1994/06/07  05:11:10  rkl
 *  Print port values before asserting.
 *
 * Revision 1.8.4.3  1994/05/14  00:36:14  stans
 * -NORMA_IPC
 *
 * Revision 1.8.4.2  1994/04/22  18:46:59  andyp
 * Added dipc_borg and DIPC_IS_PROXY hooks.
 *
 * Revision 1.8.4.1  1994/02/25  22:41:59  andyp
 * Removed some lint with respect to some debugging routines.
 *
 * Revision 1.8  1993/09/28  18:06:20  andyp
 * Update for the 1.2 release.
 *
 *
 *	27-Sep-93 [andyp@ssd.intel.com]
 *	Fully synchronous object termination can deadlock in
 *	the local case.
 *
 *	23-Sep-93 [alanl@osf.org]
 *	Enable fully synchronous object termination.
 *
 *	- Added PENDING_TRACKING fields to follow data_write_completed
 *	activity through the layers of an XMM stack.
 *	- Added option for fully synchronous object termination,
 *	currently disabled.  For now, object termination is
 *	semi-synchronous.
 *	- Altered k_server_data_write_completed to handle a
 *	terminating object.
 *	- Track recent objects that had data_write_completed activity.
 *	- Added {k,m}_db_print methods.
 *	[alanl@osf.org]
 *
 * Revision 1.7  1993/07/22  02:21:45  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.6  1993/07/01  15:32:23  terry
 * source sync with nmk13.25
 *
 * Revision 1.5  1993/06/30  22:52:38  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/06/09  01:40:55  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:46:52  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:20:23  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:11:17  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_server.c,v
 * Revision 1.2.4.8  1993/05/26  18:57:48  alanl
 * 	Avoid attempting to lookup VM objects when the mobj is
 * 	already known to be terminating.  Otherwise, it is possible
 * 	to hit in the cache on a re-activated object, when the
 * 	object in question is permanent.  Then a terminating XMM
 * 	stack effectively becomes hooked to a new object.  If the
 * 	stack is terminating, just throw away requests.  [mmp, alanl]
 * 	[1993/05/26  18:06:53  alanl]
 *
 * Revision 1.2.4.7  1993/05/24  17:30:31  alanl
 * 	Terminate races require that p_count assertions be a
 * 	little less aggressive.
 * 	[1993/05/24  17:30:14  alanl]
 * 
 * Revision 1.2.4.6  1993/05/23  18:07:44  alanl
 * 	Add internal field to mobj to remember how to set release
 * 	in M_TERMINATE call; fixes leak of server and user objects
 * 	for internal objects.  [mmp]
 * 	Also add some assertions for p_count.  [alanl]
 * 	[1993/05/23  17:47:57  alanl]
 * 
 * Revision 1.2.4.5  1993/05/03  19:03:43  dwm
 * 	Add modwc field; to decide whether to maintain p_count [ mmp ]
 * 	[1993/05/03  18:17:05  dwm]
 * 
 * Revision 1.2.4.4  1993/04/15  22:47:14  alanl
 * 	Paging flow control (NORMA_VM).  Added support for
 * 	data_write_completed; added new parameters to set_ready
 * 	and set_attributes; installed temporary debugging logic
 * 	to handle terminate case on completion as well.  [sjs]
 * 	Relax overly-aggressive assertion on pending count.  [alanl]
 * 	[1993/04/15  22:12:47  alanl]
 * 
 * Revision 1.2.4.3  1993/03/18  21:15:06  mmp
 * 	Always call M_TERMINATE with release==FALSE so that the mobjs
 * 	don't get torn down until all outstanding requests are completed.
 * 	The K_RELEASE will be initiated at the svm level once the k_count
 * 	goes to zero.  In lock_request and set_ready, check the result of
 * 	the vm_object_lookup; if it's VM_OBJECT_NULL, the vm_object is
 * 	terminating, so send back a reply if it was requested and return
 * 	instead of calling m_o_lock_request or m_o_change_attributes_common.
 * 	[1993/03/18  20:45:54  mmp]
 * 
 * Revision 1.2.4.2  1993/02/26  20:24:17  sjs
 * 	Release reference in m_o_terminate routine to make
 * 	sure the mobj goes away at the right time.  Once an
 * 	mobj has been terminate, it will only maintain a single
 * 	reference.
 * 	[93/02/26            sjs]
 * 
 * 	k_server_release must unlink the xmm_obj as K_RELEASE will not.
 * 	[93/02/26            sjs]
 * 
 * Revision 1.2  1992/11/25  01:17:13  robert
 * 	fix history
 * 	[1992/11/09  22:28:21  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:52:19  robert]
 * 
 * Revision 0.0  92/10/15            sjs
 * 	Added K_RELEASE logic for final clean up of terminate object.
 * 
 * 	Revision 1.1  1992/11/05  21:00:39  robert
 * 	Initial revision
 * 	[92/10/15            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.5.1.6  92/06/24  18:03:11  jeffreyh
 * 	Create ksvm object for local XMM stacks.
 * 	[92/06/24            dlb]
 * 
 * 	Changed routines to meet new interface.
 * 	[92/06/18            sjs]
 * 
 * 	Change boolean change_in_progress to count of pending_replies
 * 	and use in lock_request/lock_completed.  Add release logic
 * 	to lock_completed.
 * 	[92/06/09            dlb]
 * 
 * 	Handle null mobj in memory_object_data_* routines; return
 * 	KERN_FAILURE and caller will clean up.
 * 	Call xmm_object_internal_mobj_create for internal objects
 * 	instead of creating port xmm_object's for them.
 * 	[92/06/08            dlb]
 * 
 * 	Change use_old_pageout boolean to meaningful values.  Implement
 * 	memory_object_data_initialize.
 * 	[92/06/04            dlb]
 * 
 * Revision 2.5.1.5  92/03/28  10:13:20  jeffreyh
 * 	Changed data_write to data_write_return and deleted data_return
 * 	 method.  Added state information to mobj to prevent terminate
 * 	 from destroying an object that is in process by
 * 	 change_completed; support for m_o_change_attributes.
 * 	[92/03/20            sjs]
 * 
 * Revision 2.5.1.4  92/03/03  16:24:06  jeffreyh
 * 	Pick up fix from dlb to add missing vm_object_dealocate to the
 * 	 object->internal case of k_server_set_ready().
 * 	[92/02/29            jeffreyh]
 * 
 * Revision 2.5.1.3  92/02/21  11:28:01  jsb
 * 	Release send right to memory object in memory_object_terminate, now
 * 	that the xmm_user layer keeps a separate send right.
 * 	[92/02/20  14:02:57  jsb]
 * 
 * 	Explicitly provide name parameter to xmm_decl macro.
 * 	Changed termination for new reference counting implementation.
 * 	[92/02/16  15:53:26  jsb]
 * 
 * 	In memory_object_terminate, don't release_send memory_object_name
 * 	if it is null. Do call xmm_object_by_memory_object_release.
 * 	[92/02/11  18:23:15  jsb]
 * 
 * 	Changed xmm_memory_object_init to use xmm_object_by_memory_object
 * 	instead of xmm_lookup. Removed xmm_lookup.
 * 	[92/02/10  17:02:39  jsb]
 * 
 * 	Instead of holding a vm_object pointer, always do a vm_object_lookup
 * 	on pager to obtain vm_object. This allows us to notice when vm_object.c
 * 	has removed (as in vm_object_terminate) or changed (vm_object_collapse)
 * 	the port to object associations. Removed now unneeded xmm_object_set.
 * 	Declare second parameter of memory_object_* calls as xmm_obj_t, thanks
 * 	to new pager_request_t declaration of object->pager_request.
 * 	[92/02/10  09:47:16  jsb]
 * 
 * 	Use new xmm_decl, and new memory_object_name and deallocation protocol.
 * 	Removed svm exceptions; this is now handled by xmm_vm_object_lookup.
 * 	Changed xmm_lookup to not use memory_object kobject to hold
 * 	both mobj and vm_object; we now use memory_object->ip_norma_xmm_object
 * 	which is migrated upon memory_object port migration.
 * 	Don't defined memory_object_{init,create}; instead, vm/vm_object.c
 * 	calls new routine xmm_memory_object_init routine which passes
 * 	internal and size parameters down the xmm layers.
 * 	[92/02/09  13:54:41  jsb]
 * 
 * Revision 2.5.1.2  92/01/21  21:54:46  jsb
 * 	De-linted. Supports new (dlb) memory object routines.
 * 	Supports arbitrary reply ports to lock_request, etc.
 * 	Converted mach_port_t (and port_t) to ipc_port_t.
 * 	[92/01/20  17:28:58  jsb]
 * 
 * Revision 2.5.1.1  92/01/03  17:13:19  jsb
 * 	MACH_PORT_NULL -> IP_NULL.
 * 
 * Revision 2.5  91/08/28  11:16:24  jsb
 * 	Added temporary definition for memory_object_change_completed.
 * 	[91/08/16  14:21:20  jsb]
 * 
 * 	Added comment to xmm_lookup about read-only pagers.
 * 	[91/08/15  10:12:12  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:40  jsb
 * 	Changed mach_port_t to ipc_port_t whereever appropriate.
 * 	[91/07/17  14:07:08  jsb]
 * 
 * Revision 2.3  91/07/01  08:26:29  jsb
 * 	Added support for memory_object_create.
 * 	Now export normal memory_object_init with standard arguments.
 * 	Improved object initialization logic.
 * 	Added garbage collection.
 * 	[91/06/29  15:39:01  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:33  jsb
 * 	First checkin.
 * 	[91/06/17  11:05:10  jsb]
 * 
 */
/* CMU_ENDHIST */
/* 
 * Mach Operating System
 * Copyright (c) 1991, 1992 Carnegie Mellon University
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
 *	File:	norma/xmm_server.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Interface between kernel and xmm system.
 */

#include <norma/xmm_obj.h>
#include <norma/xmm_server_rename.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <vm/memory_object.h>
#include <vm/vm_fault.h>
#include <vm/vm_map.h>
#include <vm/vm_object.h>
#include <vm/vm_page.h>
#include <vm/vm_pageout.h>
#include <kern/host.h>
#include <kern/ipc_kobject.h>
#include <norma/xmm_dipc.h>

#if	NORMA2
#ifdef	IP_NORMA_IS_PROXY
#undef	IP_NORMA_IS_PROXY
#endif
#define	IP_NORMA_IS_PROXY	DIPC_IS_PROXY	/* how cheap is cheap?? */
#endif	/* NORMA2 */

#define	XMM_OBJECT_CHECKS	MACH_ASSERT
#if	XMM_OBJECT_CHECKS
void	object_check();
void	xmm_server_object_check();
#else	/* XMM_OBJECT_CHECKS */
#define	object_check(obj)
#define	xmm_server_object_check(obj)
#endif	/* XMM_OBJECT_CHECKS */

#define	SYNCHRONOUS_TERMINATION	0


struct mobj {
	struct xmm_obj	obj;
	ipc_port_t	pager;
	int		pending_replies; /* Replies expected to operations
						in progress. */
	unsigned int
	/* boolean_t */	terminate:1,	/* indicate mobj is terminating */
	/* boolean_t */	modwc:1,	/* using write completions */
	/* boolean_t */	internal:1;
	int		p_count;	/* outstanding modwc operations */
#if	PENDING_TRACKING
	int		p_seen;		/* data_writes on the way out */
	int		p_matched;	/* completeds on the way in */
	int		p_terminated;	/* lossage due to termination */
#endif
#if	XMM_OBJECT_CHECKS
	vm_object_t	last_obj;
#endif
};

/*
 * set ready stores the reply in the server object, calls set
 * attributes_common - if the reply is still in the mobj,
 * then set ready may have to do the completion
 */

#undef  KOBJ
#define KOBJ    ((struct mobj *) kobj)

#define	m_server_init			m_invalid_init
#define	m_server_terminate		m_invalid_terminate
#define	m_server_copy			m_invalid_copy
#define	m_server_data_request		m_invalid_data_request
#define	m_server_data_unlock		m_invalid_data_unlock
#define	m_server_data_write_return	m_invalid_data_write_return
#define	m_server_lock_completed		m_invalid_lock_completed
#define	m_server_supply_completed	m_invalid_supply_completed
#define	m_server_change_completed	m_invalid_change_completed

extern m_server_db_print();
extern k_server_db_print();

xmm_decl(server, "server", sizeof(struct mobj));

extern ipc_port_t xmm_object_by_memory_object();
extern xmm_obj_t  xmm_object_internal_mobj_create();

k_server_data_unavailable(kobj, offset, length)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_entry3(k_server_data_unavailable, kobj, offset, length);
#ifdef	lint
	K_DATA_UNAVAILABLE(kobj, offset, length);
#endif	lint
	return	KOBJ->terminate ? KERN_SUCCESS :
		memory_object_data_unavailable(vm_object_lookup(KOBJ->pager),
					       offset, length);
}

k_server_get_attributes(kobj, object_ready, may_cache, copy_strategy)
	xmm_obj_t kobj;
	boolean_t *object_ready;
	boolean_t *may_cache;
	memory_object_copy_strategy_t *copy_strategy;
{
	xmm_entry4(k_server_get_attributes,
		kobj, object_ready, may_cache, copy_strategy);

#ifdef	lint
	K_GET_ATTRIBUTES(kobj, object_ready, may_cache, copy_strategy);
#endif	lint
	return	KOBJ->terminate ? KERN_SUCCESS :
		memory_object_get_attributes(vm_object_lookup(KOBJ->pager),
					     object_ready, may_cache,
					     copy_strategy);
}

k_server_lock_request(kobj, offset, length, should_clean, should_flush,
		      lock_value, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	boolean_t should_clean;
	boolean_t should_flush;
	vm_prot_t lock_value;
	xmm_reply_t reply;
{
	vm_object_t object;

	xmm_entry7(k_server_lock_request,
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

	if (KOBJ->terminate ||
	    (object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		if (reply != XMM_REPLY_NULL)
			M_LOCK_COMPLETED(kobj, offset, length, reply, FALSE);
		return KERN_SUCCESS;
	}

	if (reply != XMM_REPLY_NULL)
		KOBJ->pending_replies++;

	return memory_object_lock_request(object,
					  offset, length, should_clean,
					  should_flush, lock_value,
					  (ipc_port_t) reply,
					  MACH_MSG_TYPE_PORT_SEND_ONCE);
}

k_server_data_error(kobj, offset, length, error_value)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t error_value;
{
	xmm_entry4(k_server_data_error, kobj, offset, length, error_value);
#ifdef	lint
	K_DATA_ERROR(kobj, offset, length, error_value);
#endif	lint
	return	KOBJ->terminate ? KERN_SUCCESS :
		memory_object_data_error(vm_object_lookup(KOBJ->pager),
					 offset, length, error_value);
}

k_server_set_ready(kobj, object_ready, may_cache, write_completions,
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
	vm_object_t object;
	kern_return_t kr;

	/*xmm_entry9(...);*/
	xmm_entry7(k_server_set_ready,
		kobj, object_ready, may_cache, write_completions,
		copy_strategy, cluster_size, use_routine);
#ifdef	lint
	K_SET_READY(kobj, object_ready, may_cache, write_completions,
		    copy_strategy, cluster_size,
		    use_routine, memory_object_name, reply);
#endif	lint

	KOBJ->modwc = write_completions;

	if (KOBJ->terminate ||
	    (object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		if (reply != XMM_REPLY_NULL)
			M_CHANGE_COMPLETED(kobj, may_cache, copy_strategy,
								reply, FALSE);
		return KERN_SUCCESS;
	}

	/*
	 * Remember pager_name. Only keep one send right for it.
	 */
	vm_object_lock(object);
	if (object->pager_name == IP_NULL) {
		object->pager_name = memory_object_name;
	} else {

#if	MACH_ASSERT
if (object->pager_name != memory_object_name)
	printf("object->pager_name = %x memory_object_name = %x\n",
				object->pager_name, memory_object_name);
#endif
		assert(object->pager_name == memory_object_name);
		ipc_port_release_send(memory_object_name);
	}

	/*
	 * If we are internal, we don't need to call set_attributes_common.
	 */
	if (object->internal) {
		assert(object->pager_ready);
		assert(reply == XMM_REPLY_NULL);
		vm_object_unlock(object);
		vm_object_deallocate(object);
		return KERN_SUCCESS;
	}

	/*
	 * Call set_attributes_common.
	 */
	if (reply != XMM_REPLY_NULL) {
		KOBJ->pending_replies++;
	}
	vm_object_unlock(object);
	kr = memory_object_set_attributes_common(object, object_ready,
						 may_cache, write_completions,
						 copy_strategy, cluster_size,
					use_routine == XMM_USE_DATA_WRITE);

	/*
	 * Send a reply if one was requested.
	 */
	if (reply != XMM_REPLY_NULL) {
                KOBJ->pending_replies--;
		M_CHANGE_COMPLETED(kobj, may_cache, copy_strategy, reply,
				   FALSE);
	}
	return kr;
}

/*
 * terminate will have released our reference, so it is only necessary
 * to do an unlink to dispose of the kobj.
 */
k_server_release(kobj)
	xmm_obj_t kobj;
{
	xmm_entry1(k_server_release, kobj);
#ifdef	lint
	K_RELEASE(kobj);
#endif	lint
	xmm_obj_unlink(kobj);

	return(KERN_SUCCESS);
}

k_server_destroy(kobj, reason)
	xmm_obj_t kobj;
	kern_return_t reason;
{
	xmm_entry2(k_server_destroy, kobj, reason);
#ifdef	lint
	K_DESTROY(kobj, reason);
#endif	lint
	return	KOBJ->terminate ? KERN_SUCCESS :
		memory_object_destroy(vm_object_lookup(KOBJ->pager), reason);
}

k_server_data_supply(kobj, offset, data, length, lock_value, precious, reply)
	xmm_obj_t kobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	vm_prot_t lock_value;
	boolean_t precious;
	xmm_reply_t reply;
{
	xmm_entry7(k_server_data_supply,
		kobj, offset, data, length, lock_value, precious, reply);
#ifdef	lint
	K_DATA_SUPPLY(kobj, offset, data, length, lock_value, precious, reply);
#endif	lint
	return	KOBJ->terminate ? KERN_SUCCESS :
		memory_object_data_supply(vm_object_lookup(KOBJ->pager),
					  offset, (vm_map_copy_t) data,
					  length, lock_value, precious,
					  (ipc_port_t) reply,
					  MACH_MSG_TYPE_PORT_SEND_ONCE);
}


unsigned int	c_kserver_data_write_completed = 0;
unsigned int	c_ksdwc_terminate = 0;
unsigned int	c_ksdwc_null_obj = 0;
unsigned int	c_ksdwc_fading_obj = 0;
unsigned int	c_ksdwc_found_obj = 0;
unsigned int	c_ksdwc_dec_ref_count = 0;
#define	KSDWC_RECENT_HISTORY	2048
vm_object_t	ksdwc_recent_obj[KSDWC_RECENT_HISTORY];
int		ksdwc_recent_obj_index = 0;

k_server_data_write_completed(kobj, offset, size)
	xmm_obj_t	kobj;
	vm_offset_t	offset;
	vm_size_t	size;
{
	vm_object_t		object;
	boolean_t		fading = FALSE;
	kern_return_t		kr;
	extern vm_object_t	vm_object_lookup_terminating();

	xmm_entry3(k_server_data_write_completed, kobj, offset, size);

	++c_kserver_data_write_completed;
	if (KOBJ->terminate) {
		++c_ksdwc_terminate;
		return KERN_SUCCESS;
	}
	if ((object = vm_object_lookup(KOBJ->pager)) == VM_OBJECT_NULL) {
		if ((object = vm_object_lookup_terminating(KOBJ->pager))
		    == VM_OBJECT_NULL) {
			++c_ksdwc_null_obj;
			printf("xmm_server warning:  reservation loss?\n");
			return KERN_SUCCESS;
		} else {
			/*
			 *	Special case:  the object is not available
			 *	through the normal paths.  At this time,
			 *	the object is NOT in the cache.  Because
			 *	the higher layer will decrement the
			 *	reference count, we must make sure that
			 *	vm_object_deallocate does not start
			 *	re-processing this object!
			 */
			++c_ksdwc_fading_obj;
			assert(NCPUS > 1 || object->ref_count == 2);
			fading = TRUE;
		}
	} else {
		assert(object->ref_count > 0);
		++c_ksdwc_found_obj;
	}
#if	MACH_KDB
	ksdwc_recent_obj[ksdwc_recent_obj_index++] = object;
	if (ksdwc_recent_obj_index >= KSDWC_RECENT_HISTORY)
		ksdwc_recent_obj_index = 0;
#endif
	assert(KOBJ->modwc == TRUE);
	assert(KOBJ->p_count > 0);
	KOBJ->p_count--;
#if	PENDING_TRACKING
	KOBJ->p_matched++;
#endif
	kr = memory_object_data_write_completed(object, offset, size);
	/*
	 *	A fading object had its ref_count boosted by TWO.
	 *	One of these references was released by the higher
	 *	layer, which called vm_object_deallocate.  We are
	 *	left with one reference to erase.
	 */
	if (fading == TRUE) {
		++c_ksdwc_dec_ref_count;
		vm_object_adjust_reference(object, -1);
		assert(NCPUS > 1 || object->ref_count == 0);
	}
	return kr;
}

xmm_memory_object_init(object)
	vm_object_t object;
{
	ipc_port_t xmm_object;
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry1(xmm_memory_object_init, object);

	/*
	 * Find or create xmm_object corresponding to memory_object.
	 * Once created, the xmm_object for a memory_object remains
	 * the same until the memory_object port dies.  Internal
	 * objects don't need an xmm_object, just an mobj.
	 *
	 * XXX
	 * This isn't right -- what about memory_object_destroy()?
	 *
	 * Maybe at that point the xmm_object port is destroyed.
	 *
	 * XXX
	 * Check for multiple inits? Or is this handled well enough
	 * by vm_object_enter. A few asserts might be worthwhile...
	 */
	if (object->internal) {
		mobj = xmm_object_internal_mobj_create(object->pager);
		assert(mobj != XMM_OBJ_NULL);
	}
	else {
		xmm_object = xmm_object_by_memory_object(object->pager);
		assert(xmm_object != IP_NULL);	/* XXX */

		/*
		 * If xmm_object is local, then so is the svm stack, which will
		 * be stored as xmm_object's kobject, and we need a local
		 * ksvm mobj. Otherwise, we need to create an import mobj.  
		 */
		if (IP_NORMA_IS_PROXY(xmm_object)) {
			kr = xmm_import_create(xmm_object, &mobj);
			if (kr != KERN_SUCCESS) {
			    panic("xmm_memory_object_init: xmm_import_create");
			    return kr;
			}
		} else {
			assert(ip_kotype(xmm_object) == IKOT_XMM_OBJECT);
			mobj = (xmm_obj_t) xmm_object->ip_kobject;
			ipc_port_release_send(xmm_object);
			kr = xmm_ksvm_create(mobj, &mobj);
			if (kr != KERN_SUCCESS) {
			    panic("xmm_memory_object_init: xmm_ksvm_create");
			    return kr;
			}
		}
	}

	/*
	 * Create a server layer on top.
	 */
	kr = xmm_obj_allocate(&server_class, mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		printf("xmm_memory_object_init: xmm_obj_allocate: %x\n", kr);
		return kr;
	}

	/*
	 * Associate server mobj with vm object, and pager with mobj.
	 *
	 * The reason that we don't store the vm object directly in the
	 * mobj is that the vm object can change, for example as a result
	 * of vm_object_collapse.
	 */
	MOBJ->pager = object->pager;
	MOBJ->pending_replies = 0;
	MOBJ->terminate = FALSE;
	MOBJ->modwc = object->pager_modwc;
	MOBJ->internal = object->internal;
	MOBJ->p_count = 0;
#if	PENDING_TRACKING
	MOBJ->p_seen = 0;
	MOBJ->p_matched = 0;
	MOBJ->p_terminated = 0;
#endif
#if	XMM_OBJECT_CHECKS
	MOBJ->last_obj = (vm_object_t)0;
#endif
	object->pager_request = mobj;

	/*
	 * Intialize the mobj for this kernel.
	 */
	M_INIT(mobj, PAGE_SIZE, (boolean_t) object->internal,
	       object->size);
	return KERN_SUCCESS;
}


memory_object_terminate(memory_object, mobj, memory_object_name)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	ipc_port_t memory_object_name;
{
	xmm_entry3(memory_object_terminate,
		memory_object, mobj, memory_object_name);

	if (MOBJ->modwc) {
#if	PENDING_TRACKING
		MOBJ->p_terminated += MOBJ->p_count;
#endif	/* PENDING_TRACKING */
		MOBJ->p_count = 0;
	}
#if	!SYNCHRONOUS_TERMINATION
	/*
	 *	When synchronous termination is enabled, the
	 *	object either shouldn't have paging flow
	 *	control enabled on it or all pending pageouts
	 *	should have completed.
	 *
	 *	When synchronous termination is disabled,
	 *	the object termination process is semi-synchronous.
	 *	All but the last few pageouts will be handled
	 *	by paging flow control.  The last few pageouts
	 *	have their reservations cancelled, and so are
	 *	not guaranteed to complete by this time.
	 */
#else	/* !SYNCHRONOUS_TERMINATION */
	assert(MOBJ->modwc == FALSE || MOBJ->p_count == 0);
	if (MOBJ->p_count > 0)
		panic("memory_object_terminate:  outstanding pageouts");
#endif	/* !SYNCHRONOUS_TERMINATION */

	if (memory_object != IP_NULL) {
		ipc_port_release_send(memory_object);
	}
	if (memory_object_name != IP_NULL) {
		ipc_port_release_send(memory_object_name);
	}
	/*
	 * Hold onto our reference; let k_server_release dispose of it.
	 * If this was an internal object, the reference will be released
	 * by calling M_TERMINATE with release==TRUE.
	 */
	MOBJ->terminate = TRUE;
	xmm_obj_release(mobj);

	return M_TERMINATE(mobj, MOBJ->internal);
}

void
m_server_deallocate(mobj)
	xmm_obj_t mobj;
{
	xmm_entry1(m_server_deallocate, mobj);
#if 666
	/*
	 * XXX should release pager --
	 * but need to coordinate with xmm_user.c
	 */
#else
	ipc_port_release_send(MOBJ->pager);
#endif
}

/* ARGSUSED */
memory_object_copy(memory_object, mobj, offset, length, new_memory_object)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	ipc_port_t new_memory_object;
{
	xmm_entry5(memory_object_copy,
		memory_object, mobj, offset, length, new_memory_object);

	panic("xmm_server: memory_object_copy\n");
	return KERN_FAILURE;
}

memory_object_data_request(memory_object, mobj, offset, length, desired_access)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry5(memory_object_data_request,
		memory_object, mobj, offset, length, desired_access);

#ifdef	lint
	memory_object++;
#endif	lint
	if (mobj == XMM_OBJ_NULL)
		return KERN_FAILURE;

	return M_DATA_REQUEST(mobj, offset, length, desired_access);
}

memory_object_data_unlock(memory_object, mobj, offset, length, desired_access)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	vm_prot_t desired_access;
{
	xmm_entry5(memory_object_data_unlock,
		memory_object, mobj, offset, length, desired_access);
#ifdef	lint
	memory_object++;
#endif	lint
	if (mobj == XMM_OBJ_NULL)
		return KERN_FAILURE;

	return M_DATA_UNLOCK(mobj, offset, length, desired_access);
}


memory_object_data_write(memory_object, mobj, offset, data, length)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
{
	xmm_entry5(memory_object_data_write,
		memory_object, mobj, offset, data, length);
#ifdef	lint
	memory_object++;
#endif	lint
	if (mobj == XMM_OBJ_NULL)
		return KERN_FAILURE;

	if (MOBJ->modwc) {
		assert(MOBJ->p_count >= 0);
		MOBJ->p_count++;
#if	PENDING_TRACKING
		MOBJ->p_seen++;
#endif
	}
	object_check(mobj);
	return M_DATA_WRT_RTN(mobj, offset, data, length,
		     	      TRUE, FALSE, XMM_USE_DATA_WRITE);
}

#ifdef	KERNEL
/* ARGSUSED */
memory_object_data_initialize(memory_object, mobj, offset, data, length)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
{
	xmm_entry5(memory_object_data_initialize,
		memory_object, mobj, offset, data, length);
#ifdef	lint
	memory_object++;
#endif	lint
	if (mobj == XMM_OBJ_NULL)
		return KERN_FAILURE;

	if (MOBJ->modwc) {
		assert(MOBJ->p_count >= 0);
		MOBJ->p_count++;
#if	PENDING_TRACKING
		MOBJ->p_seen++;
#endif
	}
	object_check(mobj);
	return M_DATA_WRT_RTN(mobj, offset, data, length,
		     	      TRUE, FALSE, XMM_USE_DATA_INITIALIZE);
}
#endif	KERNEL

memory_object_lock_completed(reply_to, reply_to_type, mobj, offset, length)
	ipc_port_t reply_to;
	mach_msg_type_name_t reply_to_type;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
{
	xmm_reply_t reply = (xmm_reply_t) reply_to;

	xmm_entry5(memory_object_lock_completed,
		reply_to, reply_to_type, mobj, offset, length);
	assert(reply != XMM_REPLY_NULL);
	assert(reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	MOBJ->pending_replies--;

	return M_LOCK_COMPLETED(mobj, offset, length, reply, FALSE);
}

memory_object_supply_completed(reply_to, reply_to_type, mobj, offset, length,
			       result, error_offset)
	ipc_port_t reply_to;
	mach_msg_type_name_t reply_to_type;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_size_t length;
	kern_return_t result;
	vm_offset_t error_offset;
{
	xmm_reply_t reply = (xmm_reply_t) reply_to;

	xmm_entry7(memory_object_supply_completed,
		reply_to,
		reply_to_type,
		mobj,
		offset,
		length,
		result,
		error_offset);
#ifdef	lint
	mobj++;
#endif
	assert(reply != XMM_REPLY_NULL);
	assert(reply_to_type == MACH_MSG_TYPE_PORT_SEND_ONCE);
	return M_SUPPLY_COMPLETED(mobj, offset, length, result, error_offset,
				  reply, FALSE);
}

memory_object_data_return(memory_object, mobj, offset, data, length,
			  dirty, kernel_copy)
	ipc_port_t memory_object;
	xmm_obj_t mobj;
	vm_offset_t offset;
	vm_offset_t data;
	vm_size_t length;
	boolean_t	dirty;
	boolean_t	kernel_copy;
{
	xmm_entry7(memory_object_data_return,
		memory_object, mobj, offset, data, length, dirty, kernel_copy);
#ifdef	lint
	memory_object++;
#endif	lint
	if (mobj == XMM_OBJ_NULL)
		return KERN_FAILURE;

	if (MOBJ->modwc) {
		assert(MOBJ->p_count >= 0);
		MOBJ->p_count++;
#if	PENDING_TRACKING
		MOBJ->p_seen++;
#endif	/* PENDING_TRACKING */
	}
	object_check(mobj);
	return M_DATA_WRT_RTN(mobj, offset, data, length,
			      dirty, kernel_copy, XMM_USE_DATA_RETURN);
}


#if	MACH_KDB
m_server_db_print(mobj)
xmm_obj_t	mobj;
{
	extern char	*db_bool_str();

	iprintf("pager=0x%x[%d kmsgs] pending_replies=0x%x p_count=0x%x\n",
		MOBJ->pager, db_port_kmsg_count(MOBJ->pager),
		MOBJ->pending_replies, MOBJ->p_count);
	iprintf("terminate=%s modwc=%s internal=%s\n",
		db_bool_str(MOBJ->terminate),
		db_bool_str(MOBJ->modwc),
		db_bool_str(MOBJ->internal));
#if	PENDING_TRACKING
	iprintf("p_seen=0x%x p_matched=0x%x p_terminated=0x%x\n",
		MOBJ->p_seen,
		MOBJ->p_matched,
		MOBJ->p_terminated);
#endif	/* PENDING_TRACKING */
	return 0;
}

k_server_db_print(kobj)
xmm_obj_t	kobj;
{
	return m_server_db_print(kobj);
}
#else	/* MACH_KDB */
m_server_db_print(mobj) xmm_obj_t mobj; { }
k_server_db_print(kobj) xmm_obj_t kobj; { }
#endif	/* MACH_KDB */


#if	XMM_OBJECT_CHECKS
/*
 *	Verify that object state matches MOBJ state.
 *	However, when terminating an object, the pager's
 *	type might be IKOT_PAGER_TERMINATING rather
 *	than IKOT_PAGER and the pager's object might not exist.
 */
void
object_check(mobj)
	xmm_obj_t mobj;
{
	vm_object_t obj;

	xmm_entry1(object_check, mobj);

	assert(ip_kotype(MOBJ->pager) == IKOT_PAGER ||
	       ip_kotype(MOBJ->pager) == IKOT_PAGER_TERMINATING);
	obj = (vm_object_t)MOBJ->pager->ip_kobject;
#if	0
	if (MOBJ->last_obj)
		assert(MOBJ->last_obj == obj);
#endif
	/*
	 *	The number of completions pending on the object
	 *	must equal or exceed the number of completions
	 *	in the mobj.  While normally these counts should
	 *	be equal, in practice it is possible for reservations
	 *	to be acquired without being used for some time.
	 *	(The thread acquiring the reservation(s) can block.)
	 *	It is conceivable that a thread could block after
	 *	making a reservation, on its way through the XMM stack,
	 *	but again the count in the XMM stack can't exceed
	 *	that on the object itself.  A data_write_completed
	 *	travelling through the stack on the way to the
	 *	object could decrement the stack's count and then
	 *	block -- again, the stack's count couldn't exceed
	 *	that of the object.  This assertion should be valid
	 *	even in an SMP environment.
	 *
	 *	However, pending counts should be ignored for pagers
	 *	that don't participate in paging flow control.
	 */

	MOBJ->last_obj = obj;
	if (MOBJ->p_count == 0)
		MOBJ->last_obj = 0;
}


void
xmm_server_object_check(object)
	vm_object_t object;
{
	xmm_obj_t mobj;

	xmm_entry1(xmm_server_object_check, object);

	mobj = (xmm_obj_t)object->pager_request;
}
#endif	/* XMM_OBJECT_CHECKS */
