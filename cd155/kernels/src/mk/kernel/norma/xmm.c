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
 * $Id: xmm.c,v 1.11 1994/11/18 20:56:26 mtm Exp $
 */
 
/*
 * SSD HISTORY
 * $Log: xmm.c,v $
 * Revision 1.11  1994/11/18  20:56:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/08/31  21:24:57  mtm
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
 * Revision 1.8.2.1  1994/08/08  23:53:01  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.9  1994/08/08  19:33:18  andyp
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
 * Revision 1.8  1994/07/12  19:24:54  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.7  1994/03/29  13:17:59  fritz
 * Avoid consuming too many (or even all) kthreads for faulting
 * data in. Instead, in case of overflow, queue incoming requests.
 * One of the busy kthreads will later take care of queued requests.
 * This fixes PTS #6878.
 *
 * Revision 1.6.4.4  1994/06/30  17:05:18  stans
 *   Convert bootmagic BOOT_DONT_CACHE once, then use variable instead of
 *   repeated calls to getbootint().
 *
 * Revision 1.6.4.3  1994/06/27  23:01:03  andyp
 * Fix for PTS 6878:
 * 	Avoid consuming too many (or even all) kthreads for faulting
 * 	data in. Instead, in case of overflow, queue incoming requests.
 * 	One of the busy kthreads will later take care of queued
 * 	requests.
 * >>>I HAVE DISABLED THIS CODE UNTIL PROVEN TO BE REQUIRED<<<
 * >>>FOR THE NEW NORMA IMPLEMENTATION.                    <<<
 *
 * 	Revision 1.6.2.1  1994/04/18  07:38:15  fritz
 * 	Copy from main trunk to fix PTS Bug #6878.
 *
 *	Revision 1.7  1994/03/29  13:17:59  fritz
 *	Avoid consuming too many (or even all) kthreads for faulting
 *	data in. Instead, in case of overflow, queue incoming requests.
 *	One of the busy kthreads will later take care of queued requests.
 *	This fixes PTS #6878.
 *
 * Revision 1.6.4.2  1994/06/20  17:15:02  rkl
 *  Removed BORG capability.
 *
 * Revision 1.6.4.1  1994/05/18  16:58:13  stans
 *   In 'db_xmm_uid_stack()' handle dipc_borg & -norma_ipc
 *
 * Revision 1.6  1993/09/28  18:04:53  andyp
 * Update for the 1.2 release.
 *
 *
 *	Added ddb commands to print out entire XMM stacks.
 *	Should have done this long ago.  [alanl@osf.org]
 *
 * Revision 1.5  1993/07/22  02:21:12  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:05  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:23  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:58  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:33:44  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm.c,v
 * Revision 1.2  1992/11/25  01:15:37  robert
 * 	fix history
 * 	[1992/11/09  22:11:53  robert]
 *
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:48:14  robert]
 *
 * Revision 0.0  92/10/01            sjs
 * 	Properly handle clean up of send_once_notifications of type
 * 	XMM_REPLY.
 * 
 * 	Revision 1.1  1992/11/05  21:00:10  robert
 * 	Initial revision
 * 	[92/10/01            sjs]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.6.2.6  92/06/24  18:02:09  jeffreyh
 * 	XMM framework simplification: delete m_kobj, k_mobj pointers.
 * 	  Delete xmm_kobj_link and rewrite xmm_obj_unlink.
 * 	  Delete reply->kobj_held and xmm_[km]_reply routines.
 * 	  xmm_obj_reference is now a macro in xmm_obj.h
 * 	[92/06/24            dlb]
 * 
 * Revision 2.6.2.5  92/02/21  14:34:51  jsb
 * 	Fixed merge botch.
 * 
 * Revision 2.6.2.4  92/02/21  11:25:25  jsb
 * 	Disassociate and deallocate port in convert_port_to_reply.
 * 	[92/02/18  17:33:30  jsb]
 * 
 * 	Added multiple init detection code to xmm_kobj_link.
 * 	Changed MACH_PORT_NULL uses to IP_NULL.
 * 	[92/02/18  08:45:28  jsb]
 * 
 * 	Changed reply->mobj to reply->kobj.
 * 	Added reference counting to xmm_reply_{allocate,deallocate}.
 * 	[92/02/16  18:25:51  jsb]
 * 
 * 	Do real reference counting (except for xmm_replies).
 * 	[92/02/16  14:11:32  jsb]
 * 
 * 	Moved invocation routines to norma/xmm_invoke.c.
 * 	Changed reply walking routines to use kobj, not mobj.
 * 	Preparation for better reference counting on xmm objs.
 * 	[92/02/09  12:53:36  jsb]
 * 
 * Revision 2.6.2.3  92/02/18  19:17:22  jeffreyh
 * 	Changed reply walking routines to use kobj, not mobj.
 * 	Added xmm_buffer_init to norma_vm_init.
 * 	[92/02/12            jsb]
 * 
 * Revision 2.6.2.2  92/01/21  21:53:31  jsb
 * 	Added xmm_reply_notify and (nonfunctional) xmm_reply_send_once.
 * 	[92/01/21  18:23:51  jsb]
 * 
 * 	De-linted. Added xmm_reply routines. Added functional forms of
 * 	invocation routines. Added xmm_{obj,reply}_print routines.
 * 	[92/01/20  17:13:30  jsb]
 * 
 * Revision 2.6.2.1  92/01/03  16:38:38  jsb
 * 	Corrected log.
 * 	[91/12/24  14:33:43  jsb]
 * 
 * Revision 2.6  91/12/10  13:26:21  jsb
 * 	Added better debugging in xmm_obj_deallocate.
 * 	[91/12/10  11:35:06  jsb]
 * 
 * Revision 2.5  91/11/14  16:52:32  rpd
 *	Added missing argument to bcopy.
 * 	[91/11/00            jsb]
 * 
 * Revision 2.4  91/08/03  18:19:37  jsb
 * 	Renamed xmm_init() to norma_vm_init().
 * 	[91/07/24  23:25:57  jsb]
 * 
 * Revision 2.3  91/07/01  08:25:56  jsb
 * 	Removed non-KERNEL code.
 * 	Replaced Xobj_allocate with xmm_obj_allocate.
 * 	Added xmm_obj_deallocate.
 * 	Use per-class zone for obj allocation.
 * 	[91/06/29  15:21:33  jsb]
 * 
 * Revision 2.2  91/06/17  15:48:10  jsb
 * 	First checkin.
 * 	[91/06/17  10:58:38  jsb]
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
 *	File:	norma/xmm.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Common xmm support routines.
 */
#include <norma_ipc.h>
#include <norma_vm.h>

#include <norma/xmm_obj.h>
#include <mach/notify.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>

#include <norma/xmm_dipc.h>

zone_t xmm_reply_zone;

#define	OBJ_SETQ(lhs_obj, rhs_obj)\
((rhs_obj)->refcount++, (int)((lhs_obj) = (rhs_obj)))

/*
 * If caller provides old_mobj, then he donates a reference.
 * Returns a reference for new_mobj to caller.
 */
kern_return_t
xmm_obj_allocate(class, old_mobj, new_mobj)
	xmm_class_t class;
	xmm_obj_t old_mobj;
	xmm_obj_t *new_mobj;
{
	xmm_obj_t mobj;

	xmm_entry3(xmm_obj_allocate, class, old_mobj, new_mobj);

	if (class->c_zone == ZONE_NULL) {
		char *zone_name;

		zone_name = (char *)
		    kalloc((vm_size_t) (strlen(class->c_name) + 5));
		bcopy("xmm.", zone_name, 4);
		bcopy(class->c_name, zone_name + 4, strlen(class->c_name) + 1);
		class->c_zone = zinit(class->c_size, 512*1024, class->c_size,
				      FALSE, zone_name);
	}
	mobj = (xmm_obj_t) zalloc(class->c_zone);
	bzero((char *) mobj, class->c_size);
	mobj->class = class;
	mobj->refcount = 1;
	mobj->k_kobj = XMM_OBJ_NULL;
	if (old_mobj) {
		/*
		 *	mobj->m_mobj pointer consumes caller's ref
		 *	on old_mobj.  Back pointer takes out a ref
		 *	on newly created mobj (will have 2 on return).
		 */
		mobj->m_mobj = old_mobj;  
		OBJ_SETQ(old_mobj->k_kobj, mobj);	   
	} else {
		mobj->m_mobj = XMM_OBJ_NULL;
	}
	*new_mobj = mobj;
	return KERN_SUCCESS;
}

/*
 * XXX
 * REPLACE THIS COMMENT
 *
 * Termination protocol:
 * Each layer terminates layer beneath it before deallocating.
 * Bottom layers disable outside upcalls before blocking.
 * (For example, xmm_user.c does kobject_set(NULL) before
 * calling memory_object_terminate.)
 */

xmm_obj_unlink(mobj)
	xmm_obj_t mobj;
{
	register xmm_obj_t m_mobj; 

	xmm_entry1(xmm_obj_unlink, mobj);

	if ((m_mobj = mobj->m_mobj) != XMM_OBJ_NULL) {
		assert(mobj == m_mobj->k_kobj);
		mobj->m_mobj = XMM_OBJ_NULL;
		m_mobj->k_kobj = XMM_OBJ_NULL;
		xmm_obj_release(mobj);
		xmm_obj_release(m_mobj);
	}
}

xmm_obj_release(obj)
	xmm_obj_t obj;
{
	xmm_entry1(xmm_obj_release, obj);

	assert(obj->refcount > 0);
	if (--obj->refcount == 0) {
		assert(obj->m_mobj == XMM_OBJ_NULL);
		assert(obj->k_kobj == XMM_OBJ_NULL);
		obj->class->m_deallocate(obj);
		zfree(obj->class->c_zone, (vm_offset_t) obj);
	}
}

kern_return_t
xmm_reply_allocate(kobj, reply_to, reply_to_type, new_reply)
	xmm_obj_t kobj;
	ipc_port_t reply_to;
	mach_msg_type_name_t reply_to_type;
	xmm_reply_t *new_reply;
{
	register xmm_reply_t reply;

	xmm_entry5(xmm_reply_allocate,
		kobj,
		reply_to,
		(reply_to) ? reply_to->dipc_node : -1,
		reply_to_type,
		new_reply);

	if (reply_to == IP_NULL) {
		*new_reply = XMM_REPLY_NULL;
		return KERN_SUCCESS;
	}
	reply = (xmm_reply_t) zalloc(xmm_reply_zone);
	reply->kobj = kobj;
	reply->reply_to.port = reply_to;
	reply->reply_to_type = reply_to_type;
	reply->reply_proxy = IP_NULL;
	*new_reply = reply;
	return KERN_SUCCESS;
}

xmm_reply_deallocate(reply)
	xmm_reply_t reply;
{
	xmm_entry2(xmm_reply_deallocate,
		reply,
		(reply) ? reply->kobj : -1);

	zfree(xmm_reply_zone, (vm_offset_t) reply);
}

/*
 * XXX
 * The same proxy could be used over and over again. Perhaps should
 * have a pool of xmm_replies, with reply_proxies always allocated,
 * and eliminate this routine.
 */
kern_return_t
xmm_reply_allocate_proxy(reply)
	xmm_reply_t reply;
{
	xmm_entry1(xmm_reply_allocate_proxy, reply);

	if (reply == XMM_REPLY_NULL) {
		return KERN_SUCCESS;
	}
	assert(reply->reply_proxy == IP_NULL);
	reply->reply_proxy = ipc_port_alloc_kernel();
	if (reply->reply_proxy == IP_NULL) {
		panic("xmm_reply_allocate_proxy");
		return KERN_FAILURE;
	}
	ipc_kobject_set(reply->reply_proxy, (ipc_kobject_t) reply,
			IKOT_XMM_REPLY);
	return KERN_SUCCESS;
}

/*
 * Finds reply corresponding to port.
 * Disassociates port from reply and deallocates port.
 */
xmm_reply_t
convert_port_to_reply(port)
	ipc_port_t port;
{
	xmm_reply_t reply = XMM_REPLY_NULL;

	xmm_entry1(convert_port_to_reply, port);

	if (IP_VALID(port)) {
		ip_lock(port);
		if (ip_active(port) && ip_kotype(port) == IKOT_XMM_REPLY) {
			reply = (xmm_reply_t) port->ip_kobject;
#ifdef	PARAGON860
			ip_unlock(port);
#endif
			ipc_kobject_set(port, IKO_NULL, IKOT_NONE);
#ifdef	PARAGON860
			ip_lock(port);
#endif
			assert(reply->reply_proxy == port);
			reply->reply_proxy = IP_NULL;
		}
		ip_unlock(port);
		ipc_port_dealloc_kernel(port);
	}
	return reply;
}

/*
 *
 * We got here because the remote kernel's port was dead, so clean up
 * the port and reply structure.
 */
xmm_reply_send_once(notification)
	mach_msg_header_t *notification;
{
	ipc_port_t port = (ipc_port_t) notification->msgh_remote_port;
	xmm_reply_t reply;

	xmm_entry1(xmm_reply_send_once, notification);

	assert(ip_kotype(port) == IKOT_XMM_REPLY);
	reply = convert_port_to_reply(port);
	if (reply != XMM_REPLY_NULL)
		xmm_reply_deallocate(reply);

	return TRUE;
}

boolean_t
xmm_reply_notify(msg)
	mach_msg_header_t *msg;
{
	xmm_entry1(xmm_reply_notify, msg);

	switch (msg->msgh_id) {
		case MACH_NOTIFY_SEND_ONCE:
		xmm_reply_send_once(msg);
		return TRUE;

		default:
		printf("ds_notify: strange notification %d\n", msg->msgh_id);
		return FALSE;
	}
}

norma_vm_init()
{
	xmm_entry0(norma_vm_init);

#if	PARAGON860
	{
		extern int	do_not_cache_memory_objects;
		extern int	getbootint();

		do_not_cache_memory_objects = getbootint("BOOT_DONT_CACHE",
						FALSE);
	}
#endif
	xmm_svm_init();
	xmm_split_init();
	xmm_buffer_init();
	xmm_copy_init();
	xmm_reply_zone = zinit(sizeof(struct xmm_reply), 512*1024,
			       sizeof(struct xmm_reply), FALSE, "xmm_reply");
}

#include <mach_kdb.h>
#if	MACH_KDB
#define	printf	kdbprintf

/*
 *	Routine:	xmm_obj_print
 *	Purpose:
 *		Pretty-print an xmm obj.
 */

void
xmm_obj_print(obj)
	xmm_obj_t obj;
{
	extern int indent;

	printf("xmm obj 0x%x\n", obj);

	indent += 2;

	iprintf("class=0x%x[%s]", obj->class, obj->class->c_name);
	printf(", refcount=%d\n", obj->refcount);

	iprintf("m_mobj=0x%x", obj->m_mobj);
	printf(", k_kobj=0x%x\n", obj->k_kobj);

	indent -=2;
}

/*
 *	Routine:	xmm_reply_print
 *	Purpose:
 *		Pretty-print an xmm reply.
 */

void
xmm_reply_print(reply)
	xmm_reply_t reply;
{
	extern int indent;

	printf("xmm reply 0x%x\n", reply);

	indent += 2;

	iprintf("reply_to=0x%x", reply->reply_to.port);
	printf(", reply_to_type=%d[", reply->reply_to_type);
	switch (reply->reply_to_type) {
		case MACH_MSG_TYPE_PORT_SEND:
		printf("send");
		break;

		case MACH_MSG_TYPE_PORT_SEND_ONCE:
		printf("send_once");
		break;

		case XMM_SVM_REPLY:
		printf("svm");
		break;

		case XMM_SPLIT_REPLY:
		printf("split");
		break;

		default:
		printf("???");
		break;
	}
	printf("]\n");

	iprintf("kobj=0x%x", reply->kobj);
	printf(", reply_proxy=0x%x\n", reply->reply_proxy);

	indent -=2;
}


boolean_t
db_xmm_object_check(obj)
xmm_obj_t	obj;
{
	/* check object against xmm object zone boundaries */
	return TRUE;
}


/*
 *	Chase mobj pointers to find the end of the XMM stack.
 *	Hack:  detect an instance of an svm kobj while traversing the
 *	stack so that we may print an entire stack given a pointer
 *	to an XMM object below svm.
 */
xmm_obj_t
chase_obj_links(obj, svm_kobjp)
xmm_obj_t	obj;
xmm_obj_t	*svm_kobjp;
{
	xmm_obj_t		topmost;
	extern struct xmm_class	ksvm_class;
	extern xmm_obj_t	ksvm_to_msvm();

	while (obj != XMM_OBJ_NULL) {
		topmost = obj;
		if (obj->class == &ksvm_class) {
			*svm_kobjp = obj;
			obj = ksvm_to_msvm(obj);
		} else
			obj = obj->m_mobj;
	}
	return topmost;
}


/*
 *	Pretty-print an XMM stack from a pager's perspective.
 *	Follow the mobj pointers from the initial object until
 *	reaching the top of the stack.  Reverse direction,
 *	using the kobj pointers to print out each stack layer.
 *	Hack:  detect an instance of an svm kobj while traversing the
 *	stack so that we may print an entire stack given a pointer
 *	to an XMM object below svm.  It is not possible to print
 *	the entire stack when beginning from above svm as we have
 *	no idea which ksvm obj should be used to make our way
 *	through the svm layer.
 */
db_xmm_stack(obj)
xmm_obj_t	obj;
{
	xmm_obj_t		top_mobj;
	xmm_obj_t		svm_kobj;
	xmm_obj_t		o;
	extern struct xmm_class	msvm_class;
	extern int		indent;

	if (db_xmm_object_check(obj) == FALSE)
		return;

	svm_kobj = XMM_OBJ_NULL;
	top_mobj = chase_obj_links(obj, &svm_kobj);

	o = top_mobj;
	while (o != XMM_OBJ_NULL) {
		iprintf("%2s%s obj 0x%x\trefcount %d\n",
			o == obj ? ">>" : "  ", o->class->c_name,
			o, o->refcount);
		indent += 4;
		(*o->class->k_db_print)(o);
		indent -= 4;
		if (o->class == &msvm_class)
			o = svm_kobj;
		else
			o = o->k_kobj;
	}
	return 0;
}


xmm_stack_print(obj)
xmm_obj_t	obj;
{
	db_xmm_stack(obj);
}


/*
 *	Given a port belonging to XMM, attempt to find
 *	and trace an associated stack.
 */
db_xmm_port_stack(port)
ipc_port_t	port;
{
}


#if	NORMA2
#define	norma_port_lookup	dipc_port_lookup
#endif	/* NORMA2 */

db_xmm_uid_stack(uid)
unsigned long	uid;
{
	ipc_port_t		port;
	xmm_obj_t		mobj;

	extern ipc_port_t	norma_port_lookup();

	if ((port = norma_port_lookup(uid)) == IP_NULL) {
		iprintf("uid 0x%x has no port\n", uid);
		return 0;
	}

	switch (ip_kotype(port)) {
	    case IKOT_XMM_PAGER:
	    case IKOT_XMM_KERNEL:
	    case IKOT_PAGER_TERMINATING:
		break;
	    case IKOT_XMM_OBJECT:
		iprintf("port 0x%x has strange type %d, good luck...\n",
			port, ip_kotype(port));
		break;
	    default:
		iprintf("port 0x%x has unsupported type %d\n",
			port, ip_kotype(port));
	}
	mobj = (xmm_obj_t) port->ip_kobject;
	return db_xmm_stack(mobj);
}

#endif	/* MACH_KDB */
