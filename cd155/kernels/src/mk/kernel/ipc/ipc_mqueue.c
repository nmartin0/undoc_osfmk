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
 * $Log: ipc_mqueue.c,v $
 * Revision 1.13  1995/03/14  08:01:38  lenb
 * 	tidy up ASMP baton usage. (12640)
 *
 * Revision 1.12  1994/11/18  20:49:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/12  19:22:04  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.10  1994/03/19  01:18:36  lenb
 * Benefit: FORCE_MASTER cleanup
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.9.4.12  1994/06/20  17:10:26  rkl
 *  Removed BORG capability.
 *
 * Revision 1.9.4.11  1994/05/26  23:02:04  stans
 *   ipc_mqueue_send() for(;;) loop: check if the principle has changed to a
 *   proxy while the sender was blocked on a queue limit or during migration.
 *
 * Revision 1.9.4.10  1994/05/18  22:39:36  stans
 * NORMA_IPC==0 cleanup
 *
 * Revision 1.9.4.9  1994/05/11  19:12:29  stans
 *   Since kernel object kmsgs are now queued on the actual destination port,
 *   we can remove special case processing in ipc_mqueue_receive() which as a
 *   side-effect does the right thing with respect to blocked senders.
 *
 * Revision 1.9.4.8  1994/05/05  02:02:54  stans
 *   dipc_kmsg_dequeue() has a new interface.
 *
 * Revision 1.9.4.7  1994/04/21  20:21:14  andyp
 * Added dipc_borg switch.
 *
 * Revision 1.9.4.6  1994/04/19  23:29:22  andyp
 * Subverted ipc_mqueue_move() with dipc_mqueue_move() for NORMA2.
 *
 * Revision 1.9.4.5  1994/04/08  15:30:01  rkl
 *  Removed LOOPBACK_ENG related code.
 *
 * Revision 1.9.4.4  1994/03/19  19:05:58  stans
 *   Handle kernel to kernel kobj messages, bail before blocked senders check
 *   as theset port message count is NOT where the message was delivered, hence
 *   it's wrong.
 *
 * Revision 1.9.4.3  1994/03/19  03:24:57  rkl
 *  Added support for DIPC queue full logic in ipc_mqueue_receive().
 *
 * Revision 1.9.4.2  1994/03/10  23:22:27  stans
 * more NORMA2 merge
 *
 * Revision 1.9.4.1  1994/03/10  07:08:14  rkl
 *  Redirect messages sent to NORMA2 proxies to dipc_mqueue_send() and fixed
 *  PGI warning on assert(!"some string here") style asserts.
 *
 * Revision 1.9  1993/09/30  23:36:26  stans
 *    MP3: debug assert()'s.
 *
 * Revision 1.8  1993/09/28  18:25:00  andyp
 * Update for 1.2 release.
 *
 *
 *	Eliminated NORMA_PROXY_PORT_QUEUE_LIMIT ifdef.
 *	Track port queue lengths. [alanl@osf.org]
 *
 *	NORMA send-side proxy message queue throttling:  norma_ipc_send
 *	must know what the original option and timeout values were to
 *	do its job.  Note that NORMA does not correctly implement send
 *	timeouts, although receive timeouts work as promised.  The
 *	NORMA_PROXY_PORT_QUEUE_LIMIT ifdef will disappear at a future
 *	date.
 *
 *	Also:  recheck whether the port is a proxy every time in
 *	ipc_mqueue_send's loop.  The port might migrate while
 *	ipc_mqueue_send is blocked on a port queue limit.
 *	[alanl@osf.org]
 *
 * Revision 1.7  1993/07/28  01:48:14  stans
 * MP3 debug
 *
 * Revision 1.6  1993/06/30  22:42:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/21  23:53:29  andyp
 * Picked up the santioned versions of ipc_kmsg.h (bitfield unions)
 * and the hack for the vm_map_copyout_page_list() deadlock in ipc_kmsg.c
 * from dlb.  Picked up mods from OSF's tree for ipc_mqueue.c which contains
 * another counter for suspending the sending thread when transmitting.
 *
 * Revision 1.4  1993/06/09  01:36:51  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:33:11  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:18:39  dleslie
 * Patch release of April 23
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_mqueue.c,v
 * Revision 1.1.6.5  1993/06/16  19:48:29  alanl
 * 	Count number of messages that set no_wait flag in message.
 * 	[1993/06/16  19:47:32  alanl]
 *
 * Revision 1.1.6.4  1993/04/22  19:38:31  rod
 * 	Record send state for NORMA in struct kmsg.	[mmp@osf.org]]
 * 	[1993/04/20  10:44:35  rod]
 * 
 * Revision 1.1.6.3  1993/04/15  22:44:14  alanl
 * 	Paging flow control (NORMA_VM).  Added an awful
 * 	hack to ipc_mqueue_send, forcing a message unique
 * 	to NORMA_VM (memory_object_data_write_completed)
 * 	to be enqueued for delivery without ever blocking.
 * 	This hack is currently necessary but of course
 * 	highly undesirable.  [alanl]
 * 	[1993/04/15  22:03:52  alanl]
 * 
 * Revision 1.1.6.2  1993/03/10  12:52:45  sjs
 * 	Added imq_unlock/imq_lock around call to netipc_replenish.
 * 	[93/03/09            sjs]
 * 
 * Revision 1.1  1992/09/30  02:07:39  robert
 * 	Initial revision
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.17.3.7  92/09/15  17:20:54  jeffreyh
 * 	Added code to detect if a race condition was lost in the NORMA
 * 	system  and our port is now a proxy (migrated out from under the
 * 	thread), in which case we return MACH_RCV_PORT_CHANGED.
 * 	[92/07/24            sjs]
 * 
 * Revision 2.17.3.6  92/06/24  17:59:57  jeffreyh
 * 	Allow norma_ipc_send to fail.
 * 	[92/06/02            dlb]
 * 
 * Revision 2.17.3.5  92/05/27  00:44:23  jeffreyh
 * 	In ipc_mqueue_receive, check whether a kmsg got queued while we called
 * 	netipc_replenish.
 * 	[92/05/12            dlb]
 * 
 * Revision 2.17.3.4.2.1  92/04/28  16:01:59  jeffreyh
 * 	Fixed race condition in NORMA system for ipc_mqueue_send().
 * 	[92/04/16            sjs]
 * 
 * Revision 2.17.3.4  92/03/28  10:09:17  jeffreyh
 * 	In error cases call norma_ipc_error_receiving instead
 * 	of norma_ipc_finish receiving.  This may eat the kmsg.
 * 	[92/03/20		dlb]
 * 
 * Revision 2.17.3.3  92/03/03  16:18:53  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:51:40  jeffreyh]
 * 
 * Revision 2.18  92/01/03  20:13:05  dbg
 * 	Removed THREAD_SHOULD_TERMINATE.
 * 	[91/12/19            dbg]
 * 
 * Revision 2.17.3.2  92/01/21  21:50:26  jsb
 * 	Picked up hack from dlb@osf.org to call norma_ipc_finish_receiving
 * 	before ipc_kmsg_destroy. The real fix is to use uncopyout_to_network.
 * 	[92/01/17  14:35:03  jsb]
 * 
 * Revision 2.17.3.1  92/01/03  16:35:24  jsb
 * 	Removed spurious arguments to norma_ipc_send.
 * 	Options and timeout will be handled here, not by norma_ipc_send.
 * 	[91/12/26  19:51:59  jsb]
 * 
 * 	Corrected log.
 * 	[91/12/24  14:15:11  jsb]
 *
 * Revision 2.17  91/12/15  10:40:33  jsb
 * 	Added norma_ipc_finish_receiving call to support large in-line msgs.
 * 
 * Revision 2.16  91/12/14  14:27:10  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.15  91/11/14  16:56:07  rpd
 * 	Picked up mysterious norma changes.
 * 	[91/11/14            rpd]
 * 
 * Revision 2.14  91/08/28  11:13:34  jsb
 * 	Added seqno argument to ipc_mqueue_receive.
 * 	Also added seqno processing to ipc_mqueue_send, ipc_mqueue_move.
 * 	[91/08/10            rpd]
 * 	Fixed norma_ipc_handoff technology.
 * 	Renamed clport things to norma_ipc things.
 * 	[91/08/15  08:23:17  jsb]
 * 
 * Revision 2.13  91/08/03  18:18:27  jsb
 * 	Renamed replenish routine.
 * 	[91/08/01  23:00:06  jsb]
 * 
 * 	Removed obsolete include.
 * 	Added option, timeout parameters to ipc_clport_send.
 * 	[91/07/17  14:04:15  jsb]
 * 
 * Revision 2.12  91/06/25  10:27:34  rpd
 * 	Added some wait_result assertions.
 * 	[91/05/30            rpd]
 * 
 * Revision 2.11  91/06/17  15:46:18  jsb
 * 	Renamed NORMA conditionals.
 * 	[91/06/17  10:44:39  jsb]
 * 
 * Revision 2.10  91/06/06  17:06:06  jsb
 * 	Added call to ip_unlock after calling ipc_clport_send.
 * 	Added support for clport handoff.
 * 	[91/06/06  16:05:12  jsb]
 * 
 * Revision 2.9  91/05/14  16:33:58  mrt
 * 	Correcting copyright
 * 
 * Revision 2.8  91/03/16  14:48:18  rpd
 * 	Renamed ipc_thread_{go,will_wait,will_wait_with_timeout}
 * 	to thread_{go,will_wait,will_wait_with_timeout}.
 * 	Replaced ipc_thread_block with thread_block.
 * 	[91/02/17            rpd]
 * 
 * Revision 2.7  91/02/05  17:22:24  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  15:46:33  mrt]
 * 
 * Revision 2.6  91/01/08  15:14:35  rpd
 * 	Changed continuation argument to (void (*)()).
 * 	[90/12/18            rpd]
 * 	Reorganized ipc_mqueue_receive.
 * 	[90/11/22            rpd]
 * 
 * 	Minor cleanup.
 * 	[90/11/11            rpd]
 * 
 * Revision 2.5  90/12/14  11:02:32  jsb
 * 	Changed parameters in ipc_clport_send call.
 * 	[90/12/13  21:20:13  jsb]
 * 
 * Revision 2.4  90/11/05  14:29:04  rpd
 * 	Use new io_reference and io_release.
 * 	Use new ip_reference and ip_release.
 * 	[90/10/29            rpd]
 * 
 * Revision 2.3  90/09/28  16:54:58  jsb
 * 	Added NORMA_IPC support.
 * 	[90/09/28  14:03:24  jsb]
 * 
 * Revision 2.2  90/06/02  14:50:39  rpd
 * 	Created for new IPC.
 * 	[90/03/26  20:57:06  rpd]
 */
/* CMU_ENDHIST */
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
 */
/*
 *	File:	ipc/ipc_mqueue.c
 *	Author:	Rich Draves
 *	Date:	1989
 *
 *	Functions to manipulate IPC message queues.
 */

#include <cpus.h>
#include <norma_ipc.h>
#include <norma_vm.h>
#include <mach_assert.h>

#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/counters.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_marequest.h>

#if	PARAGON860
#include <i860/cpu_master.h>
#include <i860paragon/baton.h>
#endif	PARAGON860

#if	NORMA_IPC
extern ipc_mqueue_t	norma_ipc_handoff_mqueue;
extern ipc_kmsg_t	norma_ipc_handoff_msg;
extern mach_msg_size_t	norma_ipc_handoff_max_size;
extern mach_msg_size_t	norma_ipc_handoff_msg_size;
extern ipc_kmsg_t	norma_ipc_kmsg_accept();
#endif	NORMA_IPC

/*
 *	Routine:	ipc_mqueue_init
 *	Purpose:
 *		Initialize a newly-allocated message queue.
 */

void
ipc_mqueue_init(mqueue)
	ipc_mqueue_t mqueue;
{
	imq_lock_init(mqueue);
	ipc_kmsg_queue_init(&mqueue->imq_messages);
	ipc_thread_queue_init(&mqueue->imq_threads);
}

#if	!NORMA2
/*
 *	Routine:	ipc_mqueue_move
 *	Purpose:
 *		Move messages from one queue (source) to another (dest).
 *		Only moves messages sent to the specified port.
 *	Conditions:
 *		Both queues must be locked.
 *		(This is sufficient to manipulate port->ip_seqno.)
 */

void
ipc_mqueue_move(dest, source, port)
	ipc_mqueue_t dest;
	ipc_mqueue_t source;
	ipc_port_t port;
{
	ipc_kmsg_queue_t oldq, newq;
	ipc_thread_queue_t blockedq;
	ipc_kmsg_t kmsg, next;
	ipc_thread_t th;

	oldq = &source->imq_messages;
	newq = &dest->imq_messages;
	blockedq = &dest->imq_threads;

	for (kmsg = ipc_kmsg_queue_first(oldq);
	     kmsg != IKM_NULL; kmsg = next) {
		next = ipc_kmsg_queue_next(oldq, kmsg);

		/* only move messages sent to port */

		if (kmsg->ikm_header.msgh_remote_port != (mach_port_t) port)
			continue;

		ipc_kmsg_rmqueue(oldq, kmsg);

		/* before adding kmsg to newq, check for a blocked receiver */

		while ((th = ipc_thread_dequeue(blockedq)) != ITH_NULL) {
			assert(ipc_kmsg_queue_empty(newq));

			thread_go(th);

			/* check if the receiver can handle the message */

			if (kmsg->ikm_header.msgh_size <= th->ith_msize) {
				th->ith_state = MACH_MSG_SUCCESS;
				th->ith_kmsg = kmsg;
				th->ith_seqno = port->ip_seqno++;

				goto next_kmsg;
			}

			th->ith_state = MACH_RCV_TOO_LARGE;
			th->ith_msize = kmsg->ikm_header.msgh_size;
		}

		/* didn't find a receiver to handle the message */

		ipc_kmsg_enqueue(newq, kmsg);
	    next_kmsg:;
	}
}
#endif	/* !NORMA2 */

/*
 *	Routine:	ipc_mqueue_changed
 *	Purpose:
 *		Wake up receivers waiting in a message queue.
 *	Conditions:
 *		The message queue is locked.
 */

void
ipc_mqueue_changed(mqueue, mr)
	ipc_mqueue_t mqueue;
	mach_msg_return_t mr;
{
	ipc_thread_t th;

	while ((th = ipc_thread_dequeue(&mqueue->imq_threads)) != ITH_NULL) {
		th->ith_state = mr;
		thread_go(th);
	}
}

/*
 *	Routine:	ipc_mqueue_send
 *	Purpose:
 *		Send a message to a port.  The message holds a reference
 *		for the destination port in the msgh_remote_port field.
 *
 *		If unsuccessful, the caller still has possession of
 *		the message and must do something with it.  If successful,
 *		the message is queued, given to a receiver, destroyed,
 *		or handled directly by the kernel via mach_msg.
 *	Conditions:
 *		Nothing locked.
 *	Returns:
 *		MACH_MSG_SUCCESS	The message was accepted.
 *		MACH_SEND_TIMED_OUT	Caller still has message.
 *		MACH_SEND_INTERRUPTED	Caller still has message.
 */

unsigned int c_ikm_no_wait = 0;

mach_msg_return_t
ipc_mqueue_send(kmsg, option, timeout)
	ipc_kmsg_t kmsg;
	mach_msg_option_t option;
	mach_msg_timeout_t timeout;
{
	ipc_port_t port;

#ifdef	FORCE_MASTER
	FORCE_MASTER_CPU();
#endif	/* FORCE_MASTER */

#if	ASMP
	ASSERT_BATON_OWNER();
#endif	/* ASMP */

	port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
	assert(IP_VALID(port));

#if	NORMA2
	if (DIPC_IS_PROXY(port))
		return (dipc_mqueue_send(port, kmsg, option, timeout));
#endif	/* NORMA2 */

	ip_lock(port);

	if (port->ip_receiver == ipc_space_kernel) {
		ipc_kmsg_t reply;

		/*
		 *	We can check ip_receiver == ipc_space_kernel
		 *	before checking that the port is active because
		 *	ipc_port_dealloc_kernel clears ip_receiver
		 *	before destroying a kernel port.
		 */

		assert(ip_active(port));
		ip_unlock(port);

		reply = ipc_kobject_server(kmsg);
		if (reply != IKM_NULL)
			ipc_mqueue_send_always(reply);

		return MACH_MSG_SUCCESS;
	}

	for (;;) {
		ipc_thread_t self;

#if	NORMA2
	/*
	 * we could have been a blocked sender and the port migrated away
	 * while we were blocked. Check if the port has become a proxy.
	 */
	if (DIPC_IS_PROXY(port)) {
		ip_unlock(port);
		return( dipc_mqueue_send(port, kmsg, option, timeout) );
	}
#endif	/* NORMA2 */

#if	NORMA_IPC
		if (IP_NORMA_IS_PROXY(port)) {
			kern_return_t kr;

			/*
			 *	This is a norma proxy, so hand the message
			 *	to the norma code to send.  In preparing
			 *	to send the message it is possible that the
			 *	port may migrate here.  In that case, 
			 *	norma_ipc_send returns KERN_NOT_RECEIVER.
			 *
			 *	The port may migrate while this thread
			 *	sleeps, below, so the status of the port
			 *	must be re-checked on each trip through
			 *	the loop.
			 *
			 *	norma_ipc_send may drop the port lock.
			 *	This is ok, as we're not depending on
			 *	any state.
			 */
			if (option & MACH_SEND_ALWAYS) {
				kmsg->ikm_no_wait = 1;
				++c_ikm_no_wait;
			}
			kr = norma_ipc_send(kmsg, option, timeout);
			if (kr != KERN_NOT_RECEIVER) {
				ip_unlock(port);
				return kr;
			}
		}
#endif	/* NORMA_IPC */

		/*
		 *	Can't deliver to a dead port.
		 *	However, we can pretend it got sent
		 *	and was then immediately destroyed.
		 */

		if (!ip_active(port)) {
			/*
			 *	We can't let ipc_kmsg_destroy deallocate
			 *	the port right, because we might end up
			 *	in an infinite loop trying to deliver
			 *	a send-once notification.
			 */

			ip_release(port);
			ip_check_unlock(port);
			kmsg->ikm_header.msgh_remote_port = MACH_PORT_NULL;
#if	NORMA_IPC
			norma_ipc_error_receiving(&kmsg);
			if (kmsg != IKM_NULL)
				ipc_kmsg_destroy(kmsg);
#else	NORMA_IPC
			ipc_kmsg_destroy(kmsg);
#endif	NORMA_IPC
			return MACH_MSG_SUCCESS;
		}

		/*
		 *  Don't block if:
		 *	1) We're under the queue limit.
		 *	2) Caller used the MACH_SEND_ALWAYS internal option.
		 *	3) Message is sent to a send-once right.
		 */

#if	MACH_ASSERT
		port_queue_length_check(port);
#endif	/* MACH_ASSERT */
		if ((port->ip_msgcount < port->ip_qlimit) ||
		    (option & MACH_SEND_ALWAYS) ||
#if	NORMA_VM	/* truly awful NORMA_VM hack */
#define	MEMORY_OBJECT_DATA_WRITE_COMPLETED_MSG	(2042)
		    (kmsg->ikm_header.msgh_id ==
		    MEMORY_OBJECT_DATA_WRITE_COMPLETED_MSG) ||
#endif	/* NORMA_VM */
		    (MACH_MSGH_BITS_REMOTE(kmsg->ikm_header.msgh_bits) ==
						MACH_MSG_TYPE_PORT_SEND_ONCE))
			break;

		/* must block waiting for queue to clear */

		self = current_thread();

		if (option & MACH_SEND_TIMEOUT) {
			if (timeout == 0) {
				ip_unlock(port);
				return MACH_SEND_TIMED_OUT;
			}

			thread_will_wait_with_timeout(self, timeout);
		} else
			thread_will_wait(self);

		ipc_thread_enqueue(&port->ip_blocked, self);
		self->ith_state = MACH_SEND_IN_PROGRESS;

	 	ip_unlock(port);
		counter(c_ipc_mqueue_send_block++);
		thread_block((void (*)()) 0);
		ip_lock(port);

		/* why did we wake up? */

		if (self->ith_state == MACH_MSG_SUCCESS)
			continue;
		assert(self->ith_state == MACH_SEND_IN_PROGRESS);

		/* take ourselves off blocked queue */

		ipc_thread_rmqueue(&port->ip_blocked, self);

		/*
		 *	Thread wakeup-reason field tells us why
		 *	the wait was interrupted.
		 */

		switch (self->ith_wait_result) {
		    case THREAD_INTERRUPTED:
			/* send was interrupted - give up */

			ip_unlock(port);
			return MACH_SEND_INTERRUPTED;

		    case THREAD_TIMED_OUT:
			/* timeout expired */

			assert(option & MACH_SEND_TIMEOUT);
			timeout = 0;
			break;

		    case THREAD_RESTART:
		    default:
			assert( 0 /*!"ipc_mqueue_send"*/);
			break;
		}
	}

	if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_CIRCULAR) {
		ip_unlock(port);

		/* don't allow the creation of a circular loop */

#if	NORMA_IPC
		norma_ipc_error_receiving(&kmsg);
		if (kmsg != IKM_NULL)
			ipc_kmsg_destroy(kmsg);
#else	NORMA_IPC
		ipc_kmsg_destroy(kmsg);
#endif	NORMA_IPC
		return MACH_MSG_SUCCESS;
	}

    {
	ipc_mqueue_t mqueue;
	ipc_pset_t pset;
	ipc_thread_t receiver;
	ipc_thread_queue_t receivers;

#if	MACH_ASSERT
	port_queue_length_check(port);
#endif	/* MACH_ASSERT */
	port->ip_msgcount++;
	assert(port->ip_msgcount > 0);

	pset = port->ip_pset;
	if (pset == IPS_NULL)
		mqueue = &port->ip_messages;
	else
		mqueue = &pset->ips_messages;

	imq_lock(mqueue);
	receivers = &mqueue->imq_threads;

	/*
	 *	Can unlock the port now that the msg queue is locked
	 *	and we know the port is active.  While the msg queue
	 *	is locked, we have control of the kmsg, so the ref in
	 *	it for the port is still good.  If the msg queue is in
	 *	a set (dead or alive), then we're OK because the port
	 *	is still a member of the set and the set won't go away
	 *	until the port is taken out, which tries to lock the
	 *	set's msg queue to remove the port's msgs.
	 */

	ip_unlock(port);

	/* check for a receiver for the message */

#if	NORMA_IPC
	if (mqueue == norma_ipc_handoff_mqueue && norma_ipc_handoff_msg == 0) {
		norma_ipc_handoff_msg = kmsg;
		if (kmsg->ikm_header.msgh_size <= norma_ipc_handoff_max_size) {
			imq_unlock(mqueue);
			return MACH_MSG_SUCCESS;
		}
		norma_ipc_handoff_msg_size = kmsg->ikm_header.msgh_size;
	}
#endif	NORMA_IPC
	for (;;) {
		receiver = ipc_thread_queue_first(receivers);
		if (receiver == ITH_NULL) {
			/* no receivers; queue kmsg */

			ipc_kmsg_enqueue_macro(&mqueue->imq_messages, kmsg);
			imq_unlock(mqueue);
			break;
		}

		ipc_thread_rmqueue_first_macro(receivers, receiver);
		assert(ipc_kmsg_queue_empty(&mqueue->imq_messages));

		if (kmsg->ikm_header.msgh_size <= receiver->ith_msize) {
			/* got a successful receiver */

			receiver->ith_state = MACH_MSG_SUCCESS;
			receiver->ith_kmsg = kmsg;
			receiver->ith_seqno = port->ip_seqno++;
			imq_unlock(mqueue);

			thread_go(receiver);
			break;
		}

		receiver->ith_state = MACH_RCV_TOO_LARGE;
		receiver->ith_msize = kmsg->ikm_header.msgh_size;
		thread_go(receiver);
	}
    }

	return MACH_MSG_SUCCESS;
}

/*
 *	Routine:	ipc_mqueue_copyin
 *	Purpose:
 *		Convert a name in a space to a message queue.
 *	Conditions:
 *		Nothing locked.  If successful, the message queue
 *		is returned locked and caller gets a ref for the object.
 *		This ref ensures the continued existence of the queue.
 *	Returns:
 *		MACH_MSG_SUCCESS	Found a message queue.
 *		MACH_RCV_INVALID_NAME	The space is dead.
 *		MACH_RCV_INVALID_NAME	The name doesn't denote a right.
 *		MACH_RCV_INVALID_NAME
 *			The denoted right is not receive or port set.
 *		MACH_RCV_IN_SET		Receive right is a member of a set.
 */

mach_msg_return_t
ipc_mqueue_copyin(space, name, mqueuep, objectp)
	ipc_space_t space;
	mach_port_t name;
	ipc_mqueue_t *mqueuep;
	ipc_object_t *objectp;
{
	ipc_entry_t entry;
	ipc_entry_bits_t bits;
	ipc_object_t object;
	ipc_mqueue_t mqueue;

	is_read_lock(space);
	if (!space->is_active) {
		is_read_unlock(space);
		return MACH_RCV_INVALID_NAME;
	}

	entry = ipc_entry_lookup(space, name);
	if (entry == IE_NULL) {
		is_read_unlock(space);
		return MACH_RCV_INVALID_NAME;
	}

	bits = entry->ie_bits;
	object = entry->ie_object;

	if (bits & MACH_PORT_TYPE_RECEIVE) {
		ipc_port_t port;
		ipc_pset_t pset;

		port = (ipc_port_t) object;
		assert(port != IP_NULL);

		ip_lock(port);
		assert(ip_active(port));
		assert(port->ip_receiver_name == name);
		assert(port->ip_receiver == space);
		is_read_unlock(space);

		pset = port->ip_pset;
		if (pset != IPS_NULL) {
			ips_lock(pset);
			if (ips_active(pset)) {
				ips_unlock(pset);
				ip_unlock(port);
				return MACH_RCV_IN_SET;
			}

			ipc_pset_remove(pset, port);
			ips_check_unlock(pset);
			assert(port->ip_pset == IPS_NULL);
		}

		mqueue = &port->ip_messages;
	} else if (bits & MACH_PORT_TYPE_PORT_SET) {
		ipc_pset_t pset;

		pset = (ipc_pset_t) object;
		assert(pset != IPS_NULL);

		ips_lock(pset);
		assert(ips_active(pset));
		assert(pset->ips_local_name == name);
		is_read_unlock(space);

		mqueue = &pset->ips_messages;
	} else {
		is_read_unlock(space);
		return MACH_RCV_INVALID_NAME;
	}

	/*
	 *	At this point, the object is locked and active,
	 *	the space is unlocked, and mqueue is initialized.
	 */

	io_reference(object);
	imq_lock(mqueue);
	io_unlock(object);

	*objectp = object;
	*mqueuep = mqueue;
	return MACH_MSG_SUCCESS;
}

/*
 *	Routine:	ipc_mqueue_receive
 *	Purpose:
 *		Receive a message from a message queue.
 *
 *		If continuation is non-zero, then we might discard
 *		our kernel stack when we block.  We will continue
 *		after unblocking by executing continuation.
 *
 *		If resume is true, then we are resuming a receive
 *		operation after a blocked receive discarded our stack.
 *	Conditions:
 *		The message queue is locked; it is unlocked.
 *
 *		Our caller must hold a reference for the port or port set
 *		to which this queue belongs, to keep the queue
 *		from being deallocated.  Furthermore, the port or set
 *		must have been active when the queue was locked.
 *
 *		The kmsg is returned with clean header fields
 *		and with the circular bit turned off.
 *	Returns:
 *		MACH_MSG_SUCCESS	Message returned in kmsgp.
 *		MACH_RCV_TOO_LARGE	Message size returned in kmsgp.
 *		MACH_RCV_TIMED_OUT	No message obtained.
 *		MACH_RCV_INTERRUPTED	No message obtained.
 *		MACH_RCV_PORT_DIED	Port/set died; no message.
 *		MACH_RCV_PORT_CHANGED	Port moved into set; no msg.
 */

mach_msg_return_t
ipc_mqueue_receive(mqueue, option, max_size, timeout,
		   resume, continuation, kmsgp, seqnop)
	ipc_mqueue_t mqueue;
	mach_msg_option_t option;
	mach_msg_size_t max_size;
	mach_msg_timeout_t timeout;
	boolean_t resume;
	void (*continuation)();
	ipc_kmsg_t *kmsgp;
	mach_port_seqno_t *seqnop;
{
	ipc_port_t port;
	ipc_kmsg_t kmsg;
	mach_port_seqno_t seqno;
#if	NORMA2
	mach_msg_return_t		rc;
	extern mach_msg_return_t	dipc_kmsg_dequeue();
#endif	/* NORMA2 */

    {
	ipc_kmsg_queue_t kmsgs = &mqueue->imq_messages;
	ipc_thread_t self = current_thread();

#ifdef	FORCE_MASTER
        MUST_BE_MASTER("ipc_mqueue_receive()",my_caller());
#endif	/* FORCE_MASTER */

#if	ASMP
	ASSERT_BATON_OWNER();
#endif	/* ASMP */
	if (resume)
		goto after_thread_block;

	for (;;) {
		kmsg = ipc_kmsg_queue_first(kmsgs);
#if	NORMA2
		/*
		 * if this is a meta/network kmsg, then convert this kmsg
		 * into a REAL kmsg which is left at the head of the message
		 * queue.
		 */
		if ( kmsg != IKM_NULL ) {
			if ( kmsg->ikm_kmsg_type != IKM_KMSG_TYPE_LOCAL ) {

				/* get a real kmsg */
				rc = dipc_kmsg_dequeue( kmsg, kmsgs, &kmsg);

				switch( rc ) {
				  case MACH_MSG_SUCCESS:
					assert( kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);
					break;
				  case MACH_RCV_PORT_DIED:
					printf("ipc_mqueue_recv: null kmsg, return MACH_RCV_PORT_DIED\n");
					imq_unlock(mqueue);
					return MACH_RCV_PORT_DIED;
					break;
				  default:
					panic("dipc_kmsg_dequeue bad return 0x%x?",rc);
					break;
				}
			}
		}
#endif	/* NORMA2 */
#if	NORMA_IPC
		/*
		 * It may be possible to make this work even when a timeout
		 * is specified.
		 *
		 * Netipc_replenish should be moved somewhere else.
		 */
		if (kmsg == IKM_NULL && ! (option & MACH_RCV_TIMEOUT)) {
		    imq_unlock(mqueue);
		    netipc_replenish(FALSE);
		    imq_lock(mqueue);
		    netipc_thread_lock();
		    kmsg = ipc_kmsg_queue_first(kmsgs);
		    if (kmsg == IKM_NULL) {
			*kmsgp = IKM_NULL;
			kmsg = norma_ipc_kmsg_accept(mqueue, max_size,
						     (mach_msg_size_t *)kmsgp);
			if (kmsg != IKM_NULL) {
				port = (ipc_port_t)
				    kmsg->ikm_header.msgh_remote_port;
				seqno = port->ip_seqno++;
				break;
			}
			if (*kmsgp) {
				imq_unlock(mqueue);
				return MACH_RCV_TOO_LARGE;
			}
		    }
		    netipc_thread_unlock();
		}
#endif	NORMA_IPC
		if (kmsg != IKM_NULL) {
			/* check space requirements */

			if (kmsg->ikm_header.msgh_size > max_size) {
				* (mach_msg_size_t *) kmsgp =
					kmsg->ikm_header.msgh_size;
				imq_unlock(mqueue);
				return MACH_RCV_TOO_LARGE;
			}

			ipc_kmsg_rmqueue_first_macro(kmsgs, kmsg);
			port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
			seqno = port->ip_seqno++;
			break;
		}

		/* must block waiting for a message */

		if (option & MACH_RCV_TIMEOUT) {
			if (timeout == 0) {
				imq_unlock(mqueue);
				return MACH_RCV_TIMED_OUT;
			}

			thread_will_wait_with_timeout(self, timeout);
		} else
			thread_will_wait(self);

		ipc_thread_enqueue_macro(&mqueue->imq_threads, self);
		self->ith_state = MACH_RCV_IN_PROGRESS;
		self->ith_msize = max_size;

		imq_unlock(mqueue);
		if (continuation != (void (*)()) 0) {
			counter(c_ipc_mqueue_receive_block_user++);
			thread_block(continuation);
		} else {
			counter(c_ipc_mqueue_receive_block_kernel++);
			thread_block((void (*)()) 0);
		}
	after_thread_block:
		imq_lock(mqueue);

		/* why did we wake up? */

		if (self->ith_state == MACH_MSG_SUCCESS) {
			/* pick up the message that was handed to us */

			kmsg = self->ith_kmsg;
			seqno = self->ith_seqno;
#if	NORMA2
			if ( kmsg->ikm_kmsg_type != IKM_KMSG_TYPE_LOCAL ) {
				/* get a real kmsg */
				rc = dipc_kmsg_dequeue(kmsg, IKMQ_NULL, &kmsg);
				switch( rc ) {
				  case MACH_MSG_SUCCESS:
					assert( kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);
					break;

				  case MACH_RCV_PORT_DIED:
					printf("ipc_mqueue_recv: null kmsg, return MACH_RCV_PORT_DIED\n");
					imq_unlock(mqueue);

					return MACH_RCV_PORT_DIED;
					break;

				  default:
					panic("dipc_kmsg_dequeue bad return 0x%x?",rc);
					break;
				}
			}
#endif	/* NORMA2 */
			port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
#if	NORMA2
			if (DIPC_IS_PROXY(port)) {
				imq_unlock(mqueue);
				return (MACH_RCV_PORT_CHANGED);
			}
#endif	/* NORMA2 */
#if	NORMA_IPC
			if (IP_NORMA_IS_PROXY(port)) {
				imq_unlock(mqueue);
				return (MACH_RCV_PORT_CHANGED);
			}
#endif	NORMA_IPC
			break;
		}

		switch (self->ith_state) {
		    case MACH_RCV_TOO_LARGE:
			/* pick up size of the too-large message */

			* (mach_msg_size_t *) kmsgp = self->ith_msize;
			/* fall-through */

		    case MACH_RCV_PORT_DIED:
		    case MACH_RCV_PORT_CHANGED:
			/* something bad happened to the port/set */

			imq_unlock(mqueue);
			return self->ith_state;

		    case MACH_RCV_IN_PROGRESS:
			/*
			 *	Awakened for other than IPC completion.
			 *	Remove ourselves from the waiting queue,
			 *	then check the wakeup cause.
			 */

			ipc_thread_rmqueue(&mqueue->imq_threads, self);

			switch (self->ith_wait_result) {
			    case THREAD_INTERRUPTED:
				/* receive was interrupted - give up */

				imq_unlock(mqueue);
				return MACH_RCV_INTERRUPTED;

			    case THREAD_TIMED_OUT:
				/* timeout expired */

				assert(option & MACH_RCV_TIMEOUT);
				timeout = 0;
				break;

			    case THREAD_RESTART:
			    default:
				assert(0 /*!"ipc_mqueue_receive"*/);
				break;
			}
			break;

		    default:
			assert(0 /*!"ipc_mqueue_receive: strange ith_state"*/);
		}
	}

	/* we have a kmsg; unlock the msg queue */

	imq_unlock(mqueue);
	assert(kmsg->ikm_header.msgh_size <= max_size);
    }

    {
	ipc_marequest_t marequest;

	marequest = kmsg->ikm_marequest;
	if (marequest != IMAR_NULL) {
		ipc_marequest_destroy(marequest);
		kmsg->ikm_marequest = IMAR_NULL;
	}
	assert((kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_CIRCULAR) == 0);

	assert(port == (ipc_port_t) kmsg->ikm_header.msgh_remote_port);
	ip_lock(port);

	if (ip_active(port)) {
		assert(port->ip_msgcount > 0);
		port->ip_msgcount--;

#if	NORMA2
		/*
		 *	Handle blocked senders (local and/or remote).
		 *
		 */
		if (port->ip_msgcount < port->ip_qlimit) {
			dipc_unblock_one_sender(port);
		} else 
#endif	/* NORMA2 */
		{
			ipc_thread_queue_t senders;
			ipc_thread_t sender;

			senders = &port->ip_blocked;
			sender = ipc_thread_queue_first(senders);

			if ((sender != ITH_NULL) &&
			    (port->ip_msgcount < port->ip_qlimit)) {
				ipc_thread_rmqueue(senders, sender);
				sender->ith_state = MACH_MSG_SUCCESS;
				thread_go(sender);
			}
		}
	}

	ip_unlock(port);
    }

#if	NORMA_IPC
	norma_ipc_finish_receiving(&kmsg);
#endif	NORMA_IPC

	*kmsgp = kmsg;
	*seqnop = seqno;

	return MACH_MSG_SUCCESS;
}
