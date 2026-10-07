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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
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
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: ipc_kobject.c,v $
 * Revision 1.11  1994/11/18  20:52:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/12  19:23:19  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.9  1994/05/18  21:21:11  flb
 *  Reviewer: flb
 *  Risk: low
 *  Benefit or PTS #: improve problem detection
 *  Testing: sats, eats
 *  Module(s): ipc_kobject.c
 *  Desription:    added asserts on ikm_cache() state
 *
 * Revision 1.8.8.1  1994/03/10  07:22:32  rkl
 *  Fixed PGI warning for assert(!"some string here") syle of assert.
 *
 * Revision 1.8  1993/06/30  22:44:40  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.7  1993/06/11  00:13:37  arlin
 * added new switch case in ipc_kobject_notify()
 * to call ds_master_notify(). This moved the inspection
 * of notification to device code.
 *
 * Revision 1.6  1993/06/09  01:13:44  terry
 * source sync with OSF
 *
 * Revision 1.5  1993/05/27  22:24:09  arlin
 * added support for dn notification on master_device_port, ipc_kobject_notify().
 *
 * Revision 1.4  1993/04/27  20:35:35  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:35:24  dleslie
 * First R1_0 release
 *
 * Revision 2.10.3.4  92/05/27  00:46:06  jeffreyh
 * 	Added ifdef MCMSG for nx_get/put_task_info
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.10.3.3  92/03/03  16:20:02  jeffreyh
 * 	Add IKOT_PAGER_TERMINATING case to ipc_kobject_destroy.
 * 	[92/02/25            dlb]
 * 
 * Revision 2.11  92/01/03  20:14:32  dbg
 * 	Call <subsystem>_routine_name to find IPC stub to execute.
 * 	Build reply message header by hand. (XXX)
 * 	Simplify cleanup of request message when reply is success.
 * 	[91/12/18            dbg]
 * 
 * Revision 2.10.3.2  92/02/21  11:23:50  jsb
 * 	Shuffled xmm kobject cases in ipc_kobject_notify.
 * 	[92/02/10  17:26:24  jsb]
 * 
 * Revision 2.10.3.1  92/01/21  21:50:42  jsb
 * 	Removed debugging printf.
 * 	[92/01/21  19:33:50  jsb]
 * 
 * 	Added ipc_kobject_notify for kernel-requested notifications.
 * 	Added code to correctly release send-once kobject destinations.
 * 	[92/01/21  18:20:19  jsb]
 * 
 * Revision 2.10  91/12/13  13:42:24  jsb
 * 	Added support for norma/norma_internal.defs.
 * 
 * Revision 2.9  91/08/01  14:36:18  dbg
 * 	Call machine-dependent interface routine, under
 * 	MACH_MACHINE_ROUTINES.
 * 	[91/08/01            dbg]
 * 
 * Revision 2.8  91/06/17  15:47:02  jsb
 * 	Renamed NORMA conditionals. Added NORMA_VM support.
 * 	[91/06/17  13:46:55  jsb]
 * 
 * Revision 2.7  91/06/06  17:07:05  jsb
 * 	Added NORMA_TASK support.
 * 	[91/05/14  09:05:48  jsb]
 * 
 * Revision 2.6  91/05/18  14:31:42  rpd
 * 	Added check_simple_locks.
 * 	[91/04/01            rpd]
 * 
 * Revision 2.5  91/05/14  16:42:00  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/03/16  14:50:02  rpd
 * 	Replaced ith_saved with ikm_cache.
 * 	[91/02/16            rpd]
 * 
 * Revision 2.3  91/02/05  17:26:37  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:12:51  mrt]
 * 
 * Revision 2.2  90/06/02  14:54:08  rpd
 * 	Created for new IPC.
 * 	[90/03/26  23:46:53  rpd]
 * 
 */
/*
 *	File:	kern/ipc_kobject.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Functions for letting a port represent a kernel object.
 */

#include <mach_debug.h>
#include <mach_ipc_test.h>
#include <mach_machine_routines.h>
#include <norma_task.h>
#include <norma_vm.h>
#include <norma_ipc.h>

#include <mach/port.h>
#include <mach/kern_return.h>
#include <mach/message.h>
#include <mach/mig_errors.h>
#include <mach/notify.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_object.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_thread.h>

#if	MACH_MACHINE_ROUTINES
#include <machine/machine_routines.h>
#endif

/*
 *	Routine:	ipc_kobject_server
 *	Purpose:
 *		Handle a message sent to the kernel.
 *		Generates a reply message.
 *	Conditions:
 *		Nothing locked.
 */

ipc_kmsg_t
ipc_kobject_server(request)
	ipc_kmsg_t request;
{
	mach_msg_size_t reply_size = ikm_less_overhead(8192);
	ipc_kmsg_t reply;
	kern_return_t kr;
	mig_routine_t routine;
	ipc_port_t *destp;
#if	NORMA_IPC
	ipc_port_t	dest_port;
#endif


	reply = ikm_alloc(reply_size);
	if (reply == IKM_NULL) {
		printf("ipc_kobject_server: dropping request\n");
		ipc_kmsg_destroy(request);
		return IKM_NULL;
	}
#if	NORMA_IPC
	/*
	 *	Maintaining the order of messages delivered to
	 *	kernel objects via NORMA isn't easy:  especially
	 *	when allocating memory under load, a kserver
	 *	thread can block and cause its message to be
	 *	delayed.  Message ordering is now maintained by
	 *	setting a bit on the destination port while a
	 *	kserver thread is delivering the message.  We can
	 *	clear this bit now because at this point we are
	 *	ready to invoke the actual kernel routine requested.
	 *	The message ordering guarantees at this point are
	 *	identical to those of a non-NORMA kernel.
	 *
	 *	Additional messages bound for this port will be
	 *	delivered by other active kserver threads or will
	 *	be processed by this kserver thread when it returns.
	 */
	dest_port = (ipc_port_t) request->ikm_header.msgh_remote_port;
	ip_lock(dest_port);
	if (dest_port->ip_norma_kserver_active)
		dest_port->ip_norma_kserver_active = FALSE;
	ip_unlock(dest_port);
#endif
	ikm_init(reply, reply_size);

	/*
	 * Initialize reply message.
	 */
	{
#define	InP	((mach_msg_header_t *) &request->ikm_header)
#define	OutP	((mig_reply_header_t *) &reply->ikm_header)

	    static mach_msg_type_t RetCodeType = {
		/* msgt_name = */		MACH_MSG_TYPE_INTEGER_32,
		/* msgt_size = */		32,
		/* msgt_number = */		1,
		/* msgt_inline = */		TRUE,
		/* msgt_longform = */		FALSE,
		/* msgt_unused = */		0
	    };
	    OutP->Head.msgh_bits =
		MACH_MSGH_BITS(MACH_MSGH_BITS_LOCAL(InP->msgh_bits), 0);
	    OutP->Head.msgh_size = sizeof(mig_reply_header_t);
	    OutP->Head.msgh_remote_port = InP->msgh_local_port;
	    OutP->Head.msgh_local_port  = MACH_PORT_NULL;
	    OutP->Head.msgh_seqno = 0;
	    OutP->Head.msgh_id = InP->msgh_id + 100;

	    OutP->RetCodeType = RetCodeType;

#undef	InP
#undef	OutP
	}

	/*
	 * Find the server routine to call, and call it
	 * to perform the kernel function
	 */
    {
	extern mig_routine_t	mach_server_routine(),
				mach_port_server_routine(),
				mach_host_server_routine(),
				device_server_routine(),
				device_pager_server_routine();
#if	MACH_DEBUG
	extern mig_routine_t	mach_debug_server_routine();
#endif
#if	NORMA_TASK
	extern mig_routine_t	mach_norma_server_routine();
	extern mig_routine_t	norma_internal_server_routine();
#endif
#if	NORMA_VM
	extern mig_routine_t	proxy_server_routine();
#endif

#if	MACH_MACHINE_ROUTINES
	extern mig_routine_t	MACHINE_SERVER_ROUTINE();
#endif
#if	MCMSG && (iPSC860 || PARAGON860)
	extern mig_routine_t	mcmsg_info_server_routine();
#endif	MCMSG && (iPSC860 || PARAGON860)

	check_simple_locks();
	if ((routine = mach_server_routine(&request->ikm_header)) != 0
	 || (routine = mach_port_server_routine(&request->ikm_header)) != 0
	 || (routine = mach_host_server_routine(&request->ikm_header)) != 0
	 || (routine = device_server_routine(&request->ikm_header)) != 0
	 || (routine = device_pager_server_routine(&request->ikm_header)) != 0
#if	MACH_DEBUG
	 || (routine = mach_debug_server_routine(&request->ikm_header)) != 0
#endif	MACH_DEBUG
#if	NORMA_TASK
	 || (routine = mach_norma_server_routine(&request->ikm_header)) != 0
	 || (routine = norma_internal_server_routine(&request->ikm_header)) != 0
#endif	NORMA_TASK
#if	NORMA_VM
	 || (routine = proxy_server_routine(&request->ikm_header)) != 0
#endif	NORMA_VM
#if	MACH_MACHINE_ROUTINES
	 || (routine = MACHINE_SERVER_ROUTINE(&request->ikm_header)) != 0
#endif	MACH_MACHINE_ROUTINES
#if	MCMSG && (iPSC860 || PARAGON860)
	 || (routine = mcmsg_info_server_routine(&request->ikm_header)) != 0
#endif	MCMSG && (iPSC860 || PARAGON860)
	) {
	    (*routine)(&request->ikm_header, &reply->ikm_header);
	}
	else {
	    if (!ipc_kobject_notify(&request->ikm_header, &reply->ikm_header)){
#if	MACH_IPC_TEST
		printf("ipc_kobject_server: bogus kernel message, id=%d\n",
		       request->ikm_header.msgh_id);
#endif	MACH_IPC_TEST
		((mig_reply_header_t *) &reply->ikm_header)->RetCode
			 = MIG_BAD_ID;
	    }
	}
    }
	check_simple_locks();

	/*
	 *	Destroy destination. The following code differs from
	 *	ipc_object_destroy in that we release the send-once
	 *	right instead of generating a send-once notification
	 * 	(which would bring us here again, creating a loop).
	 *	It also differs in that we only expect send or
	 *	send-once rights, never receive rights.
	 *
	 *	We set msgh_remote_port to IP_NULL so that the kmsg
	 *	destroy routines don't try to destroy the port twice.
	 */
	destp = (ipc_port_t *) &request->ikm_header.msgh_remote_port;
	switch (MACH_MSGH_BITS_REMOTE(request->ikm_header.msgh_bits)) {
		case MACH_MSG_TYPE_PORT_SEND:
		ipc_port_release_send(*destp);
		break;
		
		case MACH_MSG_TYPE_PORT_SEND_ONCE:
		ipc_port_release_sonce(*destp);
		break;
		
		default:
		assert(0 /*!"ipc_object_destroy: strange destination rights"*/);
	}
	*destp = IP_NULL;

	kr = ((mig_reply_header_t *) &reply->ikm_header)->RetCode;
	if ((kr == KERN_SUCCESS) || (kr == MIG_NO_REPLY)) {
		/*
		 *	The server function is responsible for the contents
		 *	of the message.  The reply port right is moved
		 *	to the reply message, and we have deallocated
		 *	the destination port right, so we just need
		 *	to free the kmsg.
		 */

		/* like ipc_kmsg_put, but without the copyout */

		ikm_check_initialized(request, request->ikm_size);
		if ((request->ikm_size == IKM_SAVED_KMSG_SIZE) &&
		    (ikm_cache() == IKM_NULL)) {
			assert(ikm_cache() == IKM_NULL);
			ikm_cache() = request;
		}
		else {
			assert(ikm_cache() != request);
			ikm_free(request);
		}
	} else {
		/*
		 *	The message contents of the request are intact.
		 *	Destroy everthing except the reply port right,
		 *	which is needed in the reply message.
		 */

		request->ikm_header.msgh_local_port = MACH_PORT_NULL;
		ipc_kmsg_destroy(request);
	}

	if (kr == MIG_NO_REPLY) {
		/*
		 *	The server function will send a reply message
		 *	using the reply port right, which it has saved.
		 */

		ikm_free(reply);
		return IKM_NULL;
	} else if (!IP_VALID((ipc_port_t)reply->ikm_header.msgh_remote_port)) {
		/*
		 *	Can't queue the reply message if the destination
		 *	(the reply port) isn't valid.
		 */

		ipc_kmsg_destroy(reply);
		return IKM_NULL;
	}

	return reply;
}

/*
 *	Routine:	ipc_kobject_set
 *	Purpose:
 *		Make a port represent a kernel object of the given type.
 *		The caller is responsible for handling refs for the
 *		kernel object, if necessary.
 *	Conditions:
 *		Nothing locked.  The port must be active.
 */

void
ipc_kobject_set(port, kobject, type)
	ipc_port_t port;
	ipc_kobject_t kobject;
	ipc_kobject_type_t type;
{
	ip_lock(port);
	assert(ip_active(port) || (ip_kotype(port) == IKOT_PAGER_TERMINATING));
#if	NORMA_IPC && MACH_ASSERT
	port->ip_norma_spare3 = (port->ip_bits & IO_BITS_KOTYPE);
#endif
	port->ip_bits = (port->ip_bits &~ IO_BITS_KOTYPE) | type;
	port->ip_kobject = kobject;
	ip_unlock(port);
}

/*
 *	Routine:	ipc_kobject_destroy
 *	Purpose:
 *		Release any kernel object resources associated
 *		with the port, which is being destroyed.
 *
 *		This should only be needed when resources are
 *		associated with a user's port.  In the normal case,
 *		when the kernel is the receiver, the code calling
 *		ipc_port_dealloc_kernel should clean up the resources.
 *	Conditions:
 *		The port is not locked, but it is dead.
 */

void
ipc_kobject_destroy(port)
	ipc_port_t port;
{
	switch (ip_kotype(port)) {
	    case IKOT_PAGER:
		vm_object_destroy(port);
		break;

	    case IKOT_PAGER_TERMINATING:
		vm_object_pager_wakeup(port);
		break;

	    default:
#if	MACH_ASSERT
		printf("ipc_kobject_destroy: port 0x%x, kobj 0x%x, type %d\n",
		       port, port->ip_kobject, ip_kotype(port));
#endif	MACH_ASSERT
		break;
	}
}

/*
 *	Routine:	ipc_kobject_notify
 *	Purpose:
 *		Deliver notifications to kobjects that care about them.
 */

boolean_t
ipc_kobject_notify(request_header, reply_header)
	mach_msg_header_t *request_header;
	mach_msg_header_t *reply_header;
{
	ipc_port_t port = (ipc_port_t) request_header->msgh_remote_port;

	((mig_reply_header_t *) reply_header)->RetCode = MIG_NO_REPLY;
	switch (request_header->msgh_id) {
		case MACH_NOTIFY_PORT_DELETED:
		case MACH_NOTIFY_MSG_ACCEPTED:
		case MACH_NOTIFY_PORT_DESTROYED:
		case MACH_NOTIFY_NO_SENDERS:
		case MACH_NOTIFY_SEND_ONCE:
		case MACH_NOTIFY_DEAD_NAME:
		break;

		default:
		return FALSE;
	}
	switch (ip_kotype(port)) {
#if	NORMA_VM
		case IKOT_XMM_OBJECT:
		return xmm_object_notify(request_header);

		case IKOT_XMM_PAGER:
		return xmm_pager_notify(request_header);

		case IKOT_XMM_KERNEL:
		return xmm_kernel_notify(request_header);

		case IKOT_XMM_REPLY:
		return xmm_reply_notify(request_header);
#endif	NORMA_VM

		case IKOT_DEVICE:
		return ds_notify(request_header);

		case IKOT_MASTER_DEVICE:
		return ds_master_notify(request_header);

		default:
		return FALSE;
	}
}
