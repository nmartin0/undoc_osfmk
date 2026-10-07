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
 * $Log: xmm_object.c,v $
 * Revision 1.9  1994/11/18  20:56:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/31  21:25:10  mtm
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
 * Revision 1.6.2.1  1994/08/08  23:53:58  andyp
 * Merged in from the mainline the fixes for PTS #10338, #10339, #10293.
 *
 * Revision 1.7  1994/08/08  19:33:57  andyp
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
 * Revision 1.6  1994/07/12  19:25:21  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.5.8.7  1994/06/20  17:15:12  rkl
 *  Removed BORG capability.
 *
 * Revision 1.5.8.6  1994/05/23  16:52:59  stans
 *   Cleanup NORMA_IPC == 0 ifdef's
 *
 * Revision 1.5.8.5  1994/05/18  23:10:27  stans
 * NORMA_IPC==0
 *
 * Revision 1.5.8.4  1994/04/26  15:58:19  stans
 * more NORMA_IPC == 0 work
 *
 * Revision 1.5.8.3  1994/04/22  18:46:45  andyp
 * Added dipc_borg and DIPC_IS_PROXY hooks.
 *
 * Revision 1.5.8.2  1994/04/06  17:16:14  stans
 *   Botched a NORMA2 '#if', should have been NORMA_IPC.
 *
 * Revision 1.5.8.1  1994/04/05  23:25:37  stans
 *   !NORMA_IPC, compile with NORMA_IPC == 0, NORMA_{DEVICE,VM} == 1
 *
 * Revision 1.5  1993/07/22  02:21:38  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:52:33  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:46:46  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:52:20  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: xmm_object.c,v
 * Revision 1.2.4.2  1992/12/16  23:06:48  robert
 * 	integrate changes for norma 13.17
 * 	[1992/12/16  23:05:21  robert]
 *
 * Revision 1.2  1992/11/25  01:16:54  robert
 * 	fix history
 * 	[1992/11/09  22:25:47  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:51:43  robert]
 * 
 * Revision 1.1  1992/11/05  21:00:33  robert
 * 	Initial revision
 * 
 * Revision 0.0  92/10/16            sjs
 * 	Added missing semicolon
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.5  92/09/15  17:35:23  jeffreyh
 * 	Don't take out xmm_obj references here.  Send rights
 * 	on xmm_object port are sufficient.
 * 	[92/08/19            dlb]
 * 
 * Revision 2.1.2.4  92/06/24  18:02:59  jeffreyh
 * 	Avoid creating svm and split layers for internal objects.
 * 	Also don't use port xmm_object's for internal objects.
 * 	[92/06/08            dlb]
 * 
 * Revision 2.1.2.3  92/05/28  18:22:09  jeffreyh
 * 	Add new type argument to remote_host_priv call
 * 
 * Revision 2.1.2.2  92/04/08  15:46:44  jeffreyh
 * 	Just access ip_norma_dest_node field of memory object instead
 * 	of calling norma_port_location_hint (latter now consumes a send right).
 * 	[92/04/03            dlb]
 * 
 * Revision 2.1.2.1  92/02/21  11:27:52  jsb
 * 	Use xmm_object_destroy in xmm_object_notify to release mobj and
 * 	deallocate xmm object port. Added hack to get rid of send-once
 * 	right to xmm object port before we deallocate the port, since
 * 	ipc_port_release_sonce won't do so after we deallocate it.
 * 	[92/02/20  14:00:36  jsb]
 * 
 * 	Fixed reference counting on xmm objs. A reference is now held by
 * 	xmm object, which is copied along with send right to xmm object.
 * 	[92/02/18  17:15:33  jsb]
 * 
 * 	Lots of changes. First reasonably working version.
 * 	[92/02/16  15:58:03  jsb]
 * 
 * 	Added missing line to xmm_memory_manager_export.
 * 	[92/02/12  05:58:07  jsb]
 * 
 * 	Added xmm_object_allocate routine to replace replicated xmm object
 * 	creation and initialization logic.
 * 	Added xmm_object_by_memory_object_release which disassociates
 * 	xmm object from memory object, possibly resulting in the deallocation
 * 	of mobj associated with xmm object (via no-senders).
 * 	Moved all responsibility for tearing down stack in case of xmm
 * 	object creation race to no-senders notification handler.
 * 	[92/02/11  18:38:36  jsb]
 * 
 * 	Updated explanatory text. Fixed send right management. Added
 * 	xmm_memory_manager_export routine for xmm-internal memory managers.
 * 	[92/02/11  11:24:42  jsb]
 * 
 * 	Added xmm_object_notify.
 * 	[92/02/10  17:27:44  jsb]
 * 
 * 	First checkin.
 * 	[92/02/10  17:05:02  jsb]
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
 *	File:	norma/xmm_object.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines to manage xmm object to memory object association.
 */

#include <norma_ipc.h>

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
#include <mach/notify.h>

#include <norma/xmm_dipc.h>

extern void xmm_svm_destroy();

#if	NORMA2
#ifdef	IP_NORMA_IS_PROXY
#undef	IP_NORMA_IS_PROXY
#endif
#define	IP_NORMA_IS_PROXY	DIPC_IS_PROXY	/* how cheap is cheap?? */
#define	ip_norma_dest_node	dipc_node	
#endif	/* NORMA2 */

/*
 * The structure here is:
 *
 *	memory object		-> xmm object		[ send right ]
 *	xmm object		-> top of mobj stack	[ xmm obj ref ]
 *	bottom of mobj stack	-> memory object	[ send right ]
 *
 * The xmm object and mobj stack are colocated. They are originally created
 * on the same node as the memory object, so that we can atomically set
 * memory_object->ip_norma_xmm_object on principal port for memory object,
 * which is the central synchronizing point for creating and finding an
 * mobj stack for a memory object.
 * 
 * After the stack is created, the memory object may migrated away from
 * the stack. The port migration mechanism is responsible for maintaining
 * the association between the memory object port and the xmm object port.
 * (In the current implementation, this means moving the send right to
 * xmm object and setting it as ip_norma_xmm_object in the new principal
 * for memory object.)
 *
 * This doesn't seem right... of course the real basic problem is
 * we are designing around a couple unclean things:
 *
 *	1. pointers from ports to objects
 *	2. transparent port interposition
 *
 * So I guess it's natural that if an object moves, and we've associated
 * data with that object, and that object doesn't know about it, then
 * we have to migrate that data ourselves. I guess a netmsgserver could
 * export a kobject (port data) interface, in which case it would know
 * to migrate the data when the port was migrated.
 *
 * Right now the policy is to create the layer at the current home of the
 * memory object. We could create it at the home of the first mapper.
 * This might make sense if we expect to often not need to talk to the
 * pager from the svm layer, for example in shadowing cases.
 * We might even want to migrate the layer. There's a lot of flexibility
 * here now that memory object has a port pointing to the svm layer.
 *
 * We could get rid of ip_norma_xmm_object and replace it with a hash table
 * (and a set of routines to manipulate it). The advantage would be the
 * space savings of eliminating the field, which for most ports will be
 * unused. Note that port migration must still migrate xmm object assocication.
 */

#define	XMM_INTERNAL_XMM_OBJECT	((ipc_port_t) 0x55555555)

xmm_object_set(memory_object, xmm_object, make_copy)
	ipc_port_t memory_object;
	ipc_port_t xmm_object;
	boolean_t make_copy;
{
	xmm_entry3(xmm_object_set, memory_object, xmm_object, make_copy);

	assert(! IP_NORMA_IS_PROXY(xmm_object));
	assert(memory_object->ip_norma_xmm_object != XMM_INTERNAL_XMM_OBJECT);
	memory_object->ip_norma_xmm_object = ipc_port_make_send(xmm_object);

	if (make_copy) {
		assert(ip_kotype(xmm_object) == IKOT_XMM_OBJECT);
		memory_object->ip_norma_xmm_object_refs = 1;
		ipc_port_copy_send(xmm_object);
	} else {
		memory_object->ip_norma_xmm_object_refs = 0;
	}
}

xmm_object_set_internal(memory_object)
	ipc_port_t memory_object;
{
	xmm_entry1(xmm_object_set_internal, memory_object);

	assert(! IP_NORMA_IS_PROXY(memory_object));
	memory_object->ip_norma_xmm_object = XMM_INTERNAL_XMM_OBJECT;
}

ipc_port_t
xmm_object_copy(memory_object)
	ipc_port_t memory_object;
{
	register ipc_port_t xmm_object;

	xmm_entry1(xmm_object_copy, memory_object);

	assert(! IP_NORMA_IS_PROXY(memory_object));
	xmm_object = memory_object->ip_norma_xmm_object;
	assert(xmm_object != XMM_INTERNAL_XMM_OBJECT);

	if (xmm_object == IP_NULL) {
		return IP_NULL;
	}
	assert(ip_kotype(xmm_object) == IKOT_XMM_OBJECT);
	memory_object->ip_norma_xmm_object_refs++;
	return ipc_port_copy_send(xmm_object);
}

void
xmm_object_release_local(memory_object)
	ipc_port_t memory_object;
{
	ipc_port_t xmm_object;

	xmm_entry1(xmm_object_release_local, memory_object);

	assert(memory_object->ip_norma_xmm_object != XMM_INTERNAL_XMM_OBJECT);
	assert(! IP_NORMA_IS_PROXY(memory_object));
	if (--memory_object->ip_norma_xmm_object_refs == 0) {
		/*
		 * We use no-senders because it's snazzier, but we could
		 * use a call that did a move_send instead. The receiver
		 * would then deallocate the send and the no-senders
		 * notification would be done locally (if at all).
		 * Using no-senders might help deal with node failure
		 */
		xmm_object = memory_object->ip_norma_xmm_object;
		memory_object->ip_norma_xmm_object = IP_NULL;
	    	ip_unlock(memory_object);
		ipc_port_release_send(xmm_object);
	} else
		ip_unlock(memory_object);
}

/*
 * Only called internally.
 * Allocate an xmm_object port with a no-senders notification request.
 * The xmm_object takes the mobj reference.
 */
ipc_port_t
xmm_object_allocate(mobj)
	xmm_obj_t mobj;
{
	ipc_port_t xmm_object;
	ipc_port_t old_nsrequest;

	xmm_entry1(xmm_object_allocate, mobj);

	/*
	 * Create an xmm object port.
	 */
	xmm_object = ipc_port_alloc_kernel();
	if (xmm_object == IP_NULL) {
		return IP_NULL;
	}

	/*
	 * Associate the xmm obj with the xmm object port.
	 * We keep the xmm obj reference returned by the creation routine.
	 */
	ipc_kobject_set(xmm_object, (ipc_kobject_t) mobj, IKOT_XMM_OBJECT);

	/*
	 * Request a no-senders notification.
	 */
	ipc_port_nsrequest(xmm_object, 1, ipc_port_make_sonce(xmm_object),
			   &old_nsrequest);
	assert(old_nsrequest == IP_NULL);

	/*
	 * Return the port.
	 */
	return xmm_object;
}

/*
 * Called when we lose a race to associate a newly created xmm object
 * with a memory object. Also called by xmm_object_notify.
 */
xmm_object_destroy(xmm_object, mobj)
	ipc_port_t xmm_object;
	xmm_obj_t mobj;
{
	extern struct xmm_class msvm_class;  /* XXX */

	xmm_entry2(xmm_object_destroy, xmm_object, mobj);

	/*
	 * Destroy xmm object port (and its no-senders notification request).
	 * ipc_port_release_sonce() has been fixed to destroy the send
	 * once right associated with the no senders request.
	 */
	ipc_port_dealloc_kernel(xmm_object);

	/*
	 * Lose reference to mobj, and explicitly destroy it.
	 */
	xmm_obj_release(mobj);
	xmm_svm_destroy(mobj);
}

/*
 * Handle notifications. We only care about no-senders notifications.
 */
boolean_t
xmm_object_notify(msg)
	mach_msg_header_t *msg;
{
	ipc_port_t xmm_object;
	xmm_obj_t mobj;

	xmm_entry1(xmm_object_notify, msg);

	/*
	 * Only process no-senders notifications.
	 */
	if (msg->msgh_id != MACH_NOTIFY_NO_SENDERS) {
		printf("xmm_object_notify: strange notification %d\n",
		       msg->msgh_id);
		return FALSE;
	}

	/*
	 * Extract xmm_object port from notification message.
	 */
	xmm_object = (ipc_port_t) msg->msgh_remote_port;

	/*
	 * Get and disassociate mobj from xmm object port.
	 */
	assert(ip_kotype(xmm_object) == IKOT_XMM_OBJECT);
	mobj = (xmm_obj_t) xmm_object->ip_kobject;
	ipc_kobject_set(xmm_object, IKO_NULL, IKOT_NONE);

	/*
	 * Destroy xmm object port and mobj.
	 */
	xmm_object_destroy(xmm_object, mobj);
	return TRUE;
}

/*
 * Called with memory_object locked. Unlocks memory_object.
 */
ipc_port_t
xmm_object_by_memory_object_remote(memory_object)
	ipc_port_t memory_object;
{
	unsigned long node;
	kern_return_t kr;
	ipc_port_t xmm_object;
	
	xmm_entry1(xmm_object_by_memory_object_remote, memory_object);

	assert(IP_NORMA_IS_PROXY(memory_object));

	node = memory_object->ip_norma_dest_node;

	ip_unlock(memory_object);
	kr = r_norma_xmm_object_by_memory_object(remote_host_priv(node,
						      MACH_MSG_TYPE_PORT_SEND),
						 memory_object,
						 &xmm_object);
	assert(kr == KERN_SUCCESS);
	return xmm_object;
}

/*
 * Return send right for xmm object corresponding to memory object.
 * This is to be consumed when using xmm object to set up init,
 * either via move_send dest in proxy_init, or explicit deallocation
 * in local case.
 * Also returns one xmm_object ref, to be given to svm layer and
 * released there upon termination via xmm_object_release.
 * Also returns one xmm obj ref, to be consumed by xmm_obj_allocate
 * in either _proxy_init or xmm_memory_object_init.
 *
 * Create xmm object if necessary.
 * Memory object holds a send right to xmm object as well, which is released
 * when xmm object refs drop to 0. No-senders then triggers
 * svm deallocation.
 */
ipc_port_t
xmm_object_by_memory_object(memory_object)
	ipc_port_t memory_object;
{
	ipc_port_t xmm_object, old_xmm_object;
	xmm_obj_t mobj;
	kern_return_t kr;

	xmm_entry1(xmm_object_by_memory_object, memory_object);

	/*
	 * We always create the svm stack at the current location of the
	 * memory object. We may have to chase it down if it's migrating.
	 *
	 * The memory_object principal node is the one true source
	 * of knowledge about whether an svm stack exists.
	 */
	ip_lock(memory_object);
	if (IP_NORMA_IS_PROXY(memory_object)) {
		/* the following call inherits the lock */
		return xmm_object_by_memory_object_remote(memory_object);
	}

	/*
	 * If there is already an xmm_object associated with this
	 * memory_object, return it, after taking a send-right reference
	 * which will be given (moved, if necessary) to the caller.
	 */
	xmm_object = xmm_object_copy(memory_object);
	if (xmm_object != IP_NULL) {
		ip_unlock(memory_object);
		return xmm_object;
	}

	/*
	 * Check kobject type, to foil attempts to map in inappropriate
	 * kernel objects (like task ports).
	 */
	if (ip_kotype(memory_object) != IKOT_NONE &&
	    ip_kotype(memory_object) != IKOT_PAGER) {
		ip_unlock(memory_object);
		return IP_NULL;
	}

	/*
	 * No xmm object is currently associcated with memory object.
	 * Unlock memory object port, and create an xmm obj stack.
	 * Internal objects don't need svm (and associated split) layers.
	 *
	 * XXX
	 * Should deallocate things if this call fails part-way.
	 */
	ip_unlock(memory_object);
	kr = xmm_user_create(memory_object, &mobj);
	if (kr != KERN_SUCCESS) {
		panic("xmm_mo_create: xmm_user_create: %x\n", kr);
		return IP_NULL;
	}

	kr = xmm_split_create(mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		panic("xmm_mo_create: xmm_split_create: %x\n", kr);
		return IP_NULL;
	}
	kr = xmm_svm_create(mobj, memory_object, &mobj);
	if (kr != KERN_SUCCESS) {
		panic("xmm_mo_create: xmm_svm_create: %x\n", kr);
		return IP_NULL;
	}

	/*
	 * Create an xmm object and associate it with stack.
	 * It will have one send right and a no-senders notification.
	 */
	xmm_object = xmm_object_allocate(mobj);
	if (xmm_object == IP_NULL) {
		panic("xmm_mo_create: xmm_object_allocate: %x\n", kr);
		return IP_NULL;
	}

	/*
	 * Now that we have a stack to associate with the memory object,
	 * make sure we still want it. If we don't, then just release
	 * the send right, and the no-senders notification handler
	 * will take care of deallocation.
	 *
	 * First, make sure that the memory object has not migrated.
	 */
	ip_lock(memory_object);
	if (IP_NORMA_IS_PROXY(memory_object)) {
		xmm_object_destroy(xmm_object, mobj);
		/* the following call inherits the lock */
		return xmm_object_by_memory_object_remote(memory_object);
	}

	/*
	 * If we lost the race to create the stack, discard ours
	 * and use the one already created. Otherwise, associate
	 * our xmm object and stack with the memory object,
	 * by giving the memory object the send right to the xmm object.
	 */
	old_xmm_object = xmm_object_copy(memory_object);
	if (old_xmm_object != IP_NULL) {
		xmm_object_destroy(xmm_object, mobj);
		xmm_object = old_xmm_object;
	} else {
		xmm_object_set(memory_object, xmm_object, TRUE);
	}

	/*
	 * Unlock memory object and return the xmm object send right.
	 */
	ip_unlock(memory_object);
	return xmm_object;
}

xmm_obj_t
xmm_object_internal_mobj_create(memory_object)
ipc_port_t memory_object;
{
	xmm_obj_t	mobj;
	kern_return_t kr;

	xmm_entry1(xmm_object_internal_mobj_create, memory_object);

	/*
	 *	Internal (paging) objects only need a user layer
	 */

	kr = xmm_user_create(memory_object, &mobj);
	if (kr != KERN_SUCCESS) {
	  panic("xmm_object_internal_mobj_create: xmm_user_create: %x\n", kr);
		return XMM_OBJ_NULL;
	}

	/*
	 *	Call xmm_object_set_internal to catch any other use of
	 *	this object (there shouldn't be any).
	 */
	ip_lock(memory_object);
	assert(!IP_NORMA_IS_PROXY(memory_object));
	assert(memory_object->ip_norma_xmm_object == IP_NULL);
	assert(ip_kotype(memory_object) == IKOT_PAGER);
	xmm_object_set_internal(memory_object);
	ip_unlock(memory_object);

	return(mobj);
}

/*
 * Remote, protected cover routine for xmm_object_by_memory_object.
 * Requires host_priv.
 */
kern_return_t
norma_xmm_object_by_memory_object(host, memory_object, xmm_object)
	host_t host;
	ipc_port_t memory_object;
	ipc_port_t *xmm_object;
{
	xmm_entry3(norma_xmm_object_by_memory_object,
		host, memory_object, xmm_object);

	/*
	 * Check host port validity.
	 */
	if (host == HOST_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	/*
	 * Obtain xmm_object, perhaps recursively.
	 */
	*xmm_object = xmm_object_by_memory_object(memory_object);

	/*
	 * Discard send right to memory_object given to us by our caller.
	 */
	ipc_port_release_send(memory_object);
	return KERN_SUCCESS;
}

/*
 * Called with memory_object locked. Unlocks memory_object.
 */
void
xmm_object_release_remote(memory_object)
	ipc_port_t memory_object;
{
	unsigned long node;
	kern_return_t kr;

	xmm_entry1(xmm_object_release_remote, memory_object);

	assert(IP_NORMA_IS_PROXY(memory_object));

	node = memory_object->ip_norma_dest_node;

	ip_unlock(memory_object);
	r_norma_xmm_object_release(remote_host_priv(node,
						    MACH_MSG_TYPE_PORT_SEND),
				   memory_object);
}

/*
 * If there are no real references to xmm object, then break its
 * association with memory object.
 */
void
xmm_object_release(memory_object)
	ipc_port_t memory_object;
{
	xmm_entry1(xmm_object_release, memory_object);

	/*
	 * Use local or remote form as appropriate.  Calls inherit
	 * the lock.
	 */
	ip_lock(memory_object);
	if (IP_NORMA_IS_PROXY(memory_object)) {
		xmm_object_release_remote(memory_object);
	} else {
		xmm_object_release_local(memory_object);
	}
}

/*
 * Remote, protected cover routine for xmm_object_release.
 * Requires host_priv.
 */
kern_return_t
norma_xmm_object_release(host, memory_object)
	host_t host;
	ipc_port_t memory_object;
{
	xmm_entry2(norma_xmm_object_release, host, memory_object);

	/*
	 * Check host port validity.
	 */
	if (host == HOST_NULL) {
		return KERN_INVALID_ARGUMENT;
	}

	/*
	 * Release xmm object.
	 */
	xmm_object_release(memory_object);

	/*
	 * Discard send right to memory_object given to us by our caller.
	 */
	ipc_port_release_send(memory_object);
	return KERN_SUCCESS;
}

/*
 * Create an xmm object and a stack for an xmm-internal memory manager.
 */
ipc_port_t
xmm_memory_manager_export(mobj)
	xmm_obj_t mobj;
{
	kern_return_t kr;
	ipc_port_t xmm_object;
	ipc_port_t memory_object;

	xmm_entry1(xmm_memory_manager_export, mobj);

	/*
	 * Create a memory object port for the memory manager.
	 */
	memory_object = ipc_port_alloc_kernel();
	if (memory_object == IP_NULL) {
		panic("xmm_memory_manager_export: memory_object");
		return IP_NULL;
	}

	/*
	 * Create an svm stack on top of mobj.
	 */
	kr = xmm_split_create(mobj, &mobj);
	if (kr != KERN_SUCCESS) {
		panic("xmm_memory_manager_export: xmm_split_create: %x\n", kr);
		return IP_NULL;
	}
	kr = xmm_svm_create(mobj, memory_object, &mobj);
	if (kr != KERN_SUCCESS) {
		panic("xmm_memory_manager_export: xmm_svm_create: %x\n", kr);
		return IP_NULL;
	}

	/*
	 * Create an xmm object and associate it with stack.
	 * It will have one send right and a no-senders notification request.
	 */
	xmm_object = xmm_object_allocate(mobj);
	if (xmm_object == IP_NULL) {
		panic("xmm_memory_manager_export: xmm_object_allocate");
	}

	/*
	 * Associate the xmm object with a memory object,
	 * and return a send right for the memory object.
	 */
	xmm_object_set(memory_object, xmm_object, FALSE);
	return ipc_port_make_send(memory_object);
}
