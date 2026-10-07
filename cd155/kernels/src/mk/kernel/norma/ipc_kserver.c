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
 * $Log: ipc_kserver.c,v $
 * Revision 1.10  1994/11/18  20:55:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1993/09/28  18:03:10  andyp
 * Update for the 1.2 release.
 *
 *
 *	Remove NORMA_PROXY_PORT_QUEUE_LIMIT ifdef.  [alanl@osf.org]
 *
 *	NORMA send-side proxy message queue throttling:  norma_ipc_send
 *	must know what the original option and timeout values were to
 *	do its job.  Note that NORMA does not correctly implement send
 *	timeouts, although receive timeouts work as promised.  The
 *	NORMA_PROXY_PORT_QUEUE_LIMIT ifdef will disappear at a future
 *	date.  [alanl@osf.org]
 *
 *	Slight improvements to tr messages.  [alanl@osf.org]
 *
 *	Revamp norma_critical_message to distinguish between
 *	critical (special action required by receiver) and
 *	super-critical (special action required by sender
 *	as well as receiver) messages.  Define the NORMA
 *	norma_ipc_node_set send-side message as super-critical.
 *	Change callers to norma_critical_message.  [alanl@osf.org]
 *
 *	Log current and maximum number of messages pending on
 *	kernel_kmsg and kernel_critical_kmsg lists, number of
 *	kserver activations.  [andyp@ssd.intel.com, alanl@osf.org]
 *
 *	Use kserver_pageout_thread_priority_init rather than
 *	kserver_thread_priority_init when initializing the
 *	kserver pageout support thread.  [andyp@ssd.intel.com]
 *
 * Revision 1.8  1993/07/22  22:07:44  andyp
 * Until the vnode pager threads are running at BASE_PRI, we'll leave
 * bootmagic hooks to set the priorities of kserver threads.
 *
 * Revision 1.7  1993/07/22  02:20:18  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.6  1993/06/30  22:50:22  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/25  22:28:55  andyp
 * Merged up to the latest from OSF's tree.  Recovered OSF's log messages
 * where needed.  Added the fixes for CPU idle times.  Added some Paragon
 * instrumentation for billing time spent in ipc_wait.c:norma_ipc_kmsg_accept()
 * as idle time.
 *
 * Revision 1.4  1993/06/09  01:40:14  terry
 * source sync with OSF
 *
 * Revision 1.3  1993/04/27  20:45:40  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.3  1993/04/27  00:19:21  dleslie
 * Patch release of April 23
 *
 * Revision 1.2  1993/04/12  17:20:26  SSD
 * pager flow control fixes.
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_kserver.c,v
 * Revision 1.2.4.8  1993/07/16  17:26:31  mmp
 * 	Make the kserver_pageout_support thread fixed priority and set its
 * 	priority to BASEPRI_SYSTEM (6).
 * 	[1993/07/16  17:25:35  mmp]
 *
 * Revision 1.2.4.7  1993/07/06  15:40:25  rwd
 * 	Kserver_threads now run at priority 2.  We do this so
 * 	as not to starve the vnode pager.  This needs to
 * 	be exported so that 2 is not just magic.
 * 	[93/07/02            rwd]
 * 
 * Revision 1.2.4.6  1993/05/12  17:52:18  dwm
 * 	NORMA_IPC:  Modify kserver logic to serialize delivery of
 * 	messages to kernel objects.  While one message is being
 * 	delivered, other messages to the same port wait.  A message
 * 	is regarded as delivered as soon as the kserver thread is
 * 	ready to call into the requested kernel routine, in
 * 	ipc_kobject_server().  [dlb, alanl]
 * 	[1993/05/12  17:43:27  dwm]
 * 
 * Revision 1.2.4.5  1993/04/15  22:44:37  alanl
 * 	Paging flow control (NORMA_VM).  NORMA IPC must provide
 * 	priority handling for memory_object_data_write_completed.
 * 	This message must be accepted even when memory is low to
 * 	allow pageout to complete.
 * 	Also:  norma_ipc_node_set can be critical to the pageout
 * 	path during object termination operations.  This is true
 * 	for the originator of the message (which must be able to
 * 	receive the reply) and the recipient of the message (which
 * 	must be able to handle the request, in case the sender is
 * 	important, e.g., the default pager node).
 * 	Also:  kserver_thread shouldn't take an assertion panic if
 * 	many threads are created.  Instead, the logic now handles
 * 	running out of kserver threads -- don't allow the number
 * 	of kserver threads to grow without bound.  [alanl]
 * 	[1993/04/15  22:04:21  alanl]
 * 
 * Revision 1.2.4.4  1993/04/08  22:13:36  dwm
 * 	Pick up latest Intel changes.
 * 	More threads, no panic.
 * 	[1993/04/08  18:35:02  dwm]
 * 
 * Revision 1.2.4.3  1993/02/02  13:39:50  dwm
 * 	Moved NORMA_IPC_PULL_RECEIVE_MSG definition to norma/ipc_net.h.
 * 	Removed RESUME_QUERY processing from the  kserver_pageout_support
 * 	thread and threw it  into a separate function, so QUERYs are sent
 * 	only on timeout and not also when incidental k_p_s thread routines
 * 	are invoked.  Improved synchronization for k_p_s thread. (alanl)
 * 	[1993/02/01  22:35:55  dwm]
 * 
 * Revision 1.2.4.2  1993/01/20  02:48:25  dwm
 * 	Invoke kmsg reclamation (netipc_safe_ikm_reclaim) from a
 * 	safe place, viz., kserver_pageout_support_continue.
 * 	[1993/01/20  02:42:30  dwm]
 * 
 * Revision 1.2  1992/11/25  01:14:03  robert
 * 	integrate changes below for norma_14
 * 	[1992/11/13  19:38:11  robert]
 * 
 * 	fix history
 * 	[1992/11/09  21:42:45  robert]
 * 
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:44:55  robert]
 * 
 * Revision 0.0  92/10/28            sjs
 * 		Handle KERN_NOT_RECEIVER return from norma_ipc_send (sjs).
 * 	[92/10/28            sjs]
 * 
 * Revision 0.0  92/10/26            alan
 * 		Debugging:  watch kserver_pageout_support_continue pass
 * 		kmsgs onwards.
 * 
 * 	Revision 1.1  1992/11/05  20:59:32  robert
 * 	Initial revision
 * 	[92/10/26            alan]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.9.2.8  92/09/15  17:33:36  jeffreyh
 * 	Change order of operations in kserver_pageout_support_thread to
 * 	release memory before we use it. [With alanl]
 * 	Added missing timeout call in functionkserver_pageout_support_thread
 * 	[92/08/24            jeffreyh]
 * 
 * 	Declare type for netipc_able_continue_recv().
 * 	With jeffreyh:  made kserver_pageout_support_thread
 * 	activated by a timeout to send resume queries from
 * 	suspended nodes to their oppressors.  Perhaps there
 * 	should be a separate thread for just this purpose.
 * 	[92/06/11            alanl]
 * 
 * Revision 2.9.2.7  92/06/24  18:00:42  jeffreyh
 * 	Added another function to the kserver_pageout_support
 * 	thread to send resume messages to waiting nodes
 * 	if enough memory is available.
 * 	[92/06/10            jeffreyh]
 * 
 * Revision 2.9.2.6  92/05/26  18:22:34  jeffreyh
 * 	From time to time, it is necessary to replenish the
 * 	netipc system's pages.  This is an urgent situation,
 * 	now handled by the kserver_pageout_support thread.
 * 	[92/05/07            alanl]
 * 
 * 	Enhanced the kserver_pullrecv_thread to handled two special
 * 	cases for NORMA pageout.  Case 1 is the old pullrecv case.
 * 	Case 2 scans the netipc_safe_vm_map_copy_discard_list.  Normally,
 * 	netipc_thread scans this list but netipc_thread can't run when
 * 	memory is low.  [Alanl and Jeffreyh.]
 * 	Remove deadlock from pageout path by handling pull_receive
 * 	requests in a separate thread.  See comments in code.
 * 	Also:  consistently use spl w.r.t. kernel_kmsg_lock to
 * 	prevent TLB shootdown deadlocks on multiprocessors.
 * 	[92/04/22            sjs]
 * 
 * 	Added bounds check to the number of kservers threads possible.
 * 	[92/04/16            sjs]
 * 
 * Revision 2.9.2.5  92/04/08  15:45:30  jeffreyh
 * 	Temporary debugging logic.
 * 	[92/04/06            dlb]
 * 
 * 	Added logic to make sure we never run out of kserver threads.
 * 	 This fixes migration problems among many hops.
 * 	[92/04/02            sjs]
 * 
 * Revision 2.9.2.4  92/02/21  11:24:20  jsb
 * 	In norma_kserver_deliver, don't convert reply to network format.
 * 	[92/02/21  09:04:29  jsb]
 * 
 * Revision 2.9.2.3  92/01/21  21:51:15  jsb
 * 	De-linted.
 * 	[92/01/17  12:20:44  jsb]
 * 
 * Revision 2.9.2.2  92/01/09  18:45:24  jsb
 * 	Added kernel_kmsg_lock. Use splhigh/splx instead of sploff/splon.
 * 	[92/01/08  10:03:53  jsb]
 * 
 * Revision 2.9.2.1  92/01/03  16:37:23  jsb
 * 	Corrected log.
 * 	[91/12/24  14:34:11  jsb]
 * 
 * Revision 2.9  91/12/15  10:42:15  jsb
 * 	Added norma_ipc_finish_receiving call to support large in-line msgs.
 * 
 * Revision 2.8  91/12/14  14:34:23  jsb
 * 	Removed ipc_fields.h hack.
 * 
 * Revision 2.7  91/12/10  13:26:03  jsb
 * 	Use ipc_kmsg_copyout_to_network instead of ipc_kmsg_copyin_from_kernel.
 * 	[91/12/10  11:27:15  jsb]
 * 
 * Revision 2.6  91/11/14  16:52:24  rpd
 * 	Added ipc_fields.h hack.
 *	Use IP_NORMA_IS_PROXY macro instead of ipc_space_remote.
 *	Added missing argument to kernel_thread().
 * 	[91/11/00            jsb]
 * 
 * Revision 2.5  91/08/28  11:16:03  jsb
 * 	As a hack to avoid printfs from i860ipsc/spl.c,
 * 	defined sploff/splon as splsched/splx.
 * 	[91/08/27  21:59:34  jsb]
 * 
 * 	Renamed clport things to norma things.
 * 	[91/08/15  09:11:36  jsb]
 * 
 * Revision 2.4  91/08/03  18:19:22  jsb
 * 	Replaced spldcm/splx with sploff/splon.
 * 	[91/07/28  20:52:22  jsb]
 * 
 * 	Removed obsolete includes and vm and kmsg munging operations.
 * 	[91/07/17  14:14:11  jsb]
 * 
 * 	Moved MACH_MSGH_BITS_COMPLEX_{PORTS,DATA} to mach/message.h.
 * 	[91/07/04  13:12:09  jsb]
 * 
 * 	Use vm_map_copy_t page_lists instead of old style page_lists.
 * 	[91/07/04  10:20:35  jsb]
 * 
 * Revision 2.3  91/07/01  08:25:30  jsb
 * 	Changes for new vm_map_copy_t definition.
 * 	[91/06/29  16:38:27  jsb]
 * 
 * Revision 2.2  91/06/17  15:47:44  jsb
 * 	Moved here from ipc/ipc_clkobject.c.
 * 	[91/06/17  11:05:35  jsb]
 * 
 * Revision 2.2  91/06/06  17:05:23  jsb
 * 	First checkin.
 * 	[91/05/24  13:10:00  jsb]
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
 *	File:	norma/ipc_kserver.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Functions to support ipc between nodes in a single Mach cluster.
 */

#include <machine/machparam.h>
#include <vm/vm_kern.h>
#include <vm/vm_page.h>
#include <mach/vm_param.h>
#include <mach/port.h>
#include <mach/message.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_marequest.h>
#include <norma/tr.h>

#define	NUM_KSERVER_THREADS		4
#define NUM_KSERVER_PAGEOUT_THREADS	1

/*
 * Problems:
 *	Kserver_awake should be used but isn't.
 *	We used to replenish here; should we still?
 *	Kernel_kmsg list is ugly; we should use a queue.
 */

/*
 * Locking.
 * kernel_kmsg_lock protects the following variables:
 *	kernel_kmsg		linked list of incoming messages
 *	kernel_kmsg_count	number of kmsgs currently on the list
 *	kernel_kmsg_count_max	max number of kmsgs ever on list
 *
 *	kernel_critical_kmsg	linked list of incoming critical messages
 *	kernel_critical_kmsg_count	number of kmsgs on the critical list
 *	kernel_critical_kmsg_count_max	max number of kmsgs ever on list
 *
 *	kserver_awake
 *	kserver_thread_count	count of active kserver threads
 *
 * The kernel_kmsg lock protects data structures used from
 * interrupt level, hence the lock itself must always be acquired
 * and held with interrupts disabled (splhigh).
 */

decl_simple_lock_data(,kernel_kmsg_lock)
ipc_kmsg_t kernel_kmsg = (ipc_kmsg_t) 0;
int kernel_kmsg_count;
int kernel_kmsg_count_max;

boolean_t kserver_awake = FALSE;
int kserver_awaken = 0;
int kserver_thread_count;
int kserver_thread_total;	/* count of kserver threads (debug) */
int kserver_thread_max = 64;	/* Max allowable kserver threads (debug) */

decl_simple_lock_data(,kserver_pageout_support_lock)
int kserver_pageout_support_needed;
ipc_kmsg_t kernel_critical_kmsg = (ipc_kmsg_t) 0; /* pageout critical msgs */
int kernel_critical_kmsg_count;
int kernel_critical_kmsg_count_max;
int c_norma_drain_critical = 0;

void			kserver_thread();
void			kserver_pageout_support_thread();
extern void		netipc_replenish_fallbacks();
extern boolean_t	netipc_able_continue_recv();
extern void		netipc_safe_ikm_reclaim();
extern boolean_t	norma_critical_message();

/*
 * Service request, perhaps blocking; send reply, if any.
 */
norma_kserver_deliver(kmsg)
	ipc_kmsg_t kmsg;
{
	ipc_port_t port;

	norma_ipc_finish_receiving(&kmsg);
	kmsg = ipc_kobject_server(kmsg);
	if (kmsg != IKM_NULL) {
		port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		ip_lock(port);
		if (! IP_NORMA_IS_PROXY(port)) {
			ip_unlock(port);
			ipc_mqueue_send_always(kmsg);
		} else if (norma_ipc_send(kmsg, MACH_SEND_ALWAYS, 0)
			   == KERN_NOT_RECEIVER) {
			ip_unlock(port);
			ipc_mqueue_send_always(kmsg);
		} else {
			ip_unlock(port);
		}
	}
}


unsigned int	c_kserver_activations = 0;
unsigned int	c_kserver_disappointed = 0;

void
kserver_continue()
{
	int s;
	ipc_kmsg_t	kmsg;
	ipc_kmsg_t	*prev_ptr;
	ipc_port_t	dest_port;
	int		no_work = 1;

	for (;;) {
		kserver_awaken++;
		++c_kserver_activations;

		s = splhigh();
		simple_lock(&kernel_kmsg_lock);
		while (kernel_kmsg) {
			/*
			 * If we manage to exhaust all of the threads,
			 * allocate another.
			 */
			if (--kserver_thread_count <= 0 &&
			    kserver_thread_total < kserver_thread_max) {
				++kserver_thread_total;
				/*
				 *	Pretend that this thread and
				 *	the one it is creating are
				 *	already available for service.
				 *	Otherwise, we create threads
				 *	faster than we need to.
				 */
			        kserver_thread_count += 2;
				simple_unlock(&kernel_kmsg_lock);
				splx(s);
				(void) kernel_thread(kernel_task,
						     kserver_thread,
						     (char *) 0);
				s = splhigh();
				simple_lock(&kernel_kmsg_lock);
				continue;
			}

			/*
			 *	Maintain message ordering.  Incoming,
			 *	kernel-bound messages are still ordered
			 *	by the kernel_kmsg list but once taken
			 *	off the list by a kserver thread ordering
			 *	becomes trickier.  A kserver thread can
			 *	block, especially due to memory allocation.
			 *	In such a case, a subsequent message bound
			 *	for the same kernel object can race ahead
			 *	if, for whatever reason, its kserver thread
			 *	doesn't also block.  We prevent this delivery
			 *	race by setting a flag on the destination
			 *	port for the message.  If we see this flag
			 *	set, we skip over any messages associated
			 *	with the port.  The kserver thread delivering
			 *	a message to that port will clear the flag
			 *	in ipc_kobject_server, immediately before
			 *	calling into the requested kernel operation.
			 *
			 *	There are still two exceptions:
			 *		- critical messages, processed by
			 *		the kserver_pageout_support_thread,
			 *		can race ahead of non-critical messages
			 *		- under certain load conditions,
			 *		messages relating to paging flow
			 *		control can be accelerated ahead of
			 *		normal messages
			 */
			kmsg = kernel_kmsg;
			prev_ptr = &kernel_kmsg;

			do {
				dest_port = (ipc_port_t)
       					kmsg->ikm_header.msgh_remote_port;
				ip_lock(dest_port);

				if (!dest_port->ip_norma_kserver_active) {
					dest_port->ip_norma_kserver_active =
						TRUE;
					ip_unlock(dest_port);
					break;
				}

				/*
				 *	kserver already active on this port;
				 *	skip this kmsg.
				 */
				prev_ptr = &kmsg->ikm_next;
				kmsg = kmsg->ikm_next;
				ip_unlock(dest_port);

			} while (kmsg != IKM_NULL);

			/*
			 *	If we skipped to the end of the queue,
			 *	get out of this loop.
			 */
			if (kmsg == IKM_NULL)
				break;

			*prev_ptr = kmsg->ikm_next;
			kmsg->ikm_next = IKM_BOGUS; /* XXX */
			--kernel_kmsg_count;
			simple_unlock(&kernel_kmsg_lock);
			splx(s);

			no_work = 0;
			norma_kserver_deliver(kmsg);

			s = splhigh();
			simple_lock(&kernel_kmsg_lock);
			kserver_thread_count++;
		}

		kserver_awake = FALSE;
		if (no_work)
			++c_kserver_disappointed;
		assert_wait((int) &kserver_awake, FALSE);
		simple_unlock(&kernel_kmsg_lock);
		(void) splx(s);
		thread_block(kserver_continue);
	}
}


void
norma_drain_critical()
{
	ipc_kmsg_t	kmsg;
	int		s;
	TR_DECL("norma_drain_critical");

	s = splhigh();
	simple_lock(&kernel_kmsg_lock);
	while (kernel_critical_kmsg) {
		kmsg = kernel_critical_kmsg;
		kernel_critical_kmsg = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS; /* XXX */
		--kernel_critical_kmsg_count;
		simple_unlock(&kernel_kmsg_lock);
		splx(s);
		tr5("kmsg 0x%x node 0x%x remport 0x%x msgid %d",
		    kmsg, kmsg->ikm_source_node,
		    kmsg->ikm_header.msgh_remote_port,
		    kmsg->ikm_header.msgh_id);
		norma_kserver_deliver(kmsg);
		s = splhigh();
		simple_lock(&kernel_kmsg_lock);
		++c_norma_drain_critical;
	}
	simple_unlock(&kernel_kmsg_lock);
	splx(s);
}


/*
 * Continuation for NORMA pageout special cases.
 *	1.  The netipc subsystem may be running low on
 *	memory for receiving messages.  Give the subsystem
 *	a chance to pump itself back up -- this thread
 *	has vm_privilege, so only a few pages should be
 *	allocated at a time.
 *	2.  It is also necessary from time to time to
 *	scan the netipc_safe_vm_map_copy_discard_list,
 *	freeing up discarded pages.
 *	3.  Acknowledged kmsgs must be freed from a thread that
 *	can itself allocate memory -- potentially there are
 *	notifications generated while freeing up the kmsg.
 *	4.  The pull_receive message requires a separate,
 *	wired but non-VM-abusing case.
 */
void
kserver_pageout_support_continue()
{
	ipc_kmsg_t	kmsg;
	int		s;
	TR_DECL("kserver_pageout_support_continue");

	s = splhigh();
	simple_lock(&kserver_pageout_support_lock);
	tr2("active, needed 0x%x", kserver_pageout_support_needed);

	while (kserver_pageout_support_needed > 0) {
		--kserver_pageout_support_needed;
		simple_unlock(&kserver_pageout_support_lock);
		splx(s);

		netipc_replenish_fallbacks();

		netipc_output_replenish_pages();

		netipc_safe_ikm_reclaim();

		norma_drain_critical();

		s = splhigh();
		simple_lock(&kserver_pageout_support_lock);
	}

	assert(kserver_pageout_support_needed == 0);
	assert_wait((int) kserver_pageout_support_continue, FALSE);
	simple_unlock(&kserver_pageout_support_lock);
	splx(s);
	thread_block(kserver_pageout_support_continue);
	/* NOTREACHED */
}


void
kserver_pageout_support_wakeup()
{
	int	s;

	s = splhigh();
	simple_lock(&kserver_pageout_support_lock);
	assert(kserver_pageout_support_needed >= 0);
	++kserver_pageout_support_needed;
	thread_wakeup_one((int) kserver_pageout_support_continue);
	simple_unlock(&kserver_pageout_support_lock);
	splx(s);
}


void
kserver_thread()
{
	int s;
	
	thread_set_own_priority(2);	/* XXX Same as vnode pager.  Export this */

	s = splhigh();
	simple_lock(&kernel_kmsg_lock);
	kserver_awake = FALSE;
	assert_wait((int) &kserver_awake, FALSE);
	simple_unlock(&kernel_kmsg_lock);
	(void) splx(s);

	thread_block(kserver_continue);
	/*NOTREACHED*/
}

/*
 * Avoiding remote pageout deadlock.
 *
 * The kserver_pageout_support_thread exists only to avoid deadlocking
 * the kernel when paging out to a remote node.  There are two cases
 * of interest.  Case 1:
 * The message sequence looks like this:
 *	kernel2 sends kernel1 a pageout request
 *	[ possible intervening requests ]
 *	kernel1 sends kernel2 a pull_receive request
 * Handling pull_receive requires wiring the thread so a stack
 * will always be available for it -- after all, the request will
 * arrive during a memory shortage.  Wiring a thread implies
 * vm_privilege as well as stack_privilege.  Adding vm_privilege
 * to kserver_thread is a bad idea because intervening requests
 * (e.g., a vm_object_copy_slowly) can soak up all available vm
 * and hang the thread so that the pull_receive can't be processed.
 *
 * kernel_pageout_support_thread must be wired so it will always have a
 * stack but will not get in trouble with vm_privilege because it
 * doesn't handle any other kinds of requests.
 *
 * Case 2:
 * Freeing memory.  In some cases, we want to be sure that memory
 * will be freed in a timely fashion.  In others, freeing memory may
 * also require allocating memory (although less than that being freed).
 * In all cases, a wired thread is required to guarantee that we can
 * carry out these duties even in low-memory situations.
 */

#if	PARAGON860	/* XXX change this when the server is changed */
int	kserver_pageout_thread_priority_init = 2;
#else	PARAGON860
int	kserver_pageout_thread_priority_init = BASEPRI_SYSTEM;
#endif	PARAGON860

void
kserver_pageout_support_thread()
{
	kern_return_t	kr;
	int		s;
	
	(void) thread_policy(current_thread(), POLICY_FIXEDPRI, 1);
	thread_set_own_priority(kserver_pageout_thread_priority_init);
	kr = thread_wire (realhost.host_priv_self, current_thread(), TRUE);
	if (kr != KERN_SUCCESS) {
		printf ("kserver_thread: thread_wire fail with 0x%x\n", kr);
	}
	simple_lock_init(&kserver_pageout_support_lock);
	kserver_pageout_support_needed = 0;
	kserver_pageout_support_continue();
	/* NOTREACHED */
}

void
norma_ipc_kobject_send(kmsg)
	ipc_kmsg_t kmsg;
{
	ipc_kmsg_t	*kern_kmsg_list;
	ipc_kmsg_t	km;
	boolean_t	use_pageout_support;
	int		s, *kern_kmsg_count, *kern_kmsg_count_max;

	if (norma_critical_message(kmsg->ikm_header.msgh_id, FALSE) == TRUE) {
		kern_kmsg_list = &kernel_critical_kmsg;
		use_pageout_support = TRUE;
		kern_kmsg_count = &kernel_critical_kmsg_count;
		kern_kmsg_count_max = &kernel_critical_kmsg_count_max;
	} else {
		kern_kmsg_list = &kernel_kmsg;
		use_pageout_support = FALSE;
		kern_kmsg_count = &kernel_kmsg_count;
		kern_kmsg_count_max = &kernel_kmsg_count_max;
	}

	s = splhigh();
	simple_lock(&kernel_kmsg_lock);
	if (*kern_kmsg_list) {
		for (km = *kern_kmsg_list; km->ikm_next; )
			km = km->ikm_next;
		km->ikm_next = kmsg;
	} else {
		*kern_kmsg_list = kmsg;
	}
	if ((*kern_kmsg_count = *kern_kmsg_count + 1) > *kern_kmsg_count_max) {
		*kern_kmsg_count_max = *kern_kmsg_count;
	}
	kmsg->ikm_next = 0;
	kmsg->ikm_prev = IKM_BOGUS; /* XXX */
	if (use_pageout_support == FALSE)
		thread_wakeup_one((int) &kserver_awake);
	simple_unlock(&kernel_kmsg_lock);
	splx(s);
	if (use_pageout_support == TRUE)
		kserver_pageout_support_wakeup();
}

norma_kserver_startup()
{
	int i;

	for (i = 0; i < NUM_KSERVER_PAGEOUT_THREADS; i++) {
		(void) kernel_thread(kernel_task,
				     kserver_pageout_support_thread,
				     (char *) 0);
	}
	for (i = 0; i < NUM_KSERVER_THREADS; i++) {
		(void) kernel_thread(kernel_task, kserver_thread, (char *) 0);
	}
	kserver_thread_count = NUM_KSERVER_THREADS;
	kserver_thread_total = NUM_KSERVER_THREADS;
}
