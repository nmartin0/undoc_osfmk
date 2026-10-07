/*
 * 
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * Kernel object server
 *	support mach messages sent to remote (not on this node) kernel objects
 *		(i.e., a remote task port).
 *
 * $Id: dipc_kserver.c,v 1.5 1994/11/18 20:57:47 mtm Exp $
 *
 * HISTORY:
 */

/*
 * Just what is this "K" server business anyway? kserver threads are the agents
 * which handle Mach messages, generally syscall/MiG stubs, sent to kernel
 * objects. Without going too far down a Mach rathole, kernel objects are
 * local kernel entities which are represented by Mach IPC ports but are
 * much more that just a port (as if that isn't complex enough). An example
 * is the port which represents a user-mode task. Conceptually, a Mach syscall
 * is a message sent to a task/thread port. If said 'port' is managed by
 * your local micro-kernel then everything is fine, the syscall is processed
 * on your local node in your (stolen) thread context. What about the case
 * where the 'port' lives on another node? A message is actually sent to the
 * remote port. Unfortunatlly, no one is home to consume the message. This is
 * where a "k" (kernel) server thread comes into play. The k server thread
 * becomes the thread context for the syscall message (just like in the local
 * case). When the message and it's optional reply have been processed the
 * kserver thread returns to a common pool to await more work to do.
 * When the specific port has no more queued messages (ip_msgcount == 0) the
 * thread returns to the kserver pool.
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <mach/vm_param.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/vm_param.h>

#include <kern/queue.h>
#include <kern/assert.h>

#include <ipc/ipc_space.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_mqueue.h>

#include <kern/ipc_kobject.h>
#include <kern/ipc_sched.h>
#include <kern/host.h>
#include <kern/sched_prim.h>

#include <norma2/dipc_uid.h>
#include <norma2/norma_log.h>
#include <norma2/dipc_kserver.h>
#include <norma2/norma_transport.h>

#include <vm/vm_kern.h>
#include <vm/vm_page.h>

#define	DIPC_KSVR_THREADS 16	/* initial # of kserver threads */

int	dipc_ksvr_threads = DIPC_KSVR_THREADS;

ipc_port_t	dipc_kobj_active_head;
ipc_port_t	dipc_kobj_active_tail;
ipc_port_t	dipc_emmi_reply_port;
ipc_port_t	dipc_emmi_reply_priv_port;

void	dipc_kobj_server_thread();
void	dipc_emmi_reply_thread();


/*
 *	Initialize the dipc (Distributed IPC) kernel object servers.
 */
void dipc_kobj_server_init()
{
	int		i;

	ksvr_entry0(dipc_kobj_server_init);

	dipc_ksvr_threads = getbootint("DIPC_KSVR_THREADS", dipc_ksvr_threads);

	dipc_kobj_active_head = IP_DEAD;
	dipc_kobj_active_tail = IP_DEAD;

	/*
	 *	create dipc kobj server thread(s).
	 */
 	for (i = 0; i < dipc_ksvr_threads; i++) {
		(void) kernel_thread( kernel_task,
				dipc_kobj_server_thread, (char *) 0);
	}


	/*
	 *	create a port and a thread to handle emmi replies (special,
	 *	but don't need VM privs).
	 */
	dipc_emmi_reply_port = ipc_port_alloc_kernel();
	assert(dipc_emmi_reply_port != IP_NULL);
	(void) kernel_thread( kernel_task,
		dipc_emmi_reply_thread, (char *) dipc_emmi_reply_port );


	/*
	 *	create a port and thread to handle emmi replies that need
	 *	to operate in a memory-starved environment (ie, needs
	 *	VM priv).
	 */
	dipc_emmi_reply_priv_port = ipc_port_alloc_kernel();
	assert(dipc_emmi_reply_priv_port != IP_NULL);
	(void) kernel_thread( kernel_task,
		dipc_emmi_reply_thread, (char *) dipc_emmi_reply_priv_port );
}


int	dipc_kobj_port_wanted;


int dipc_kobj_port_activate(ipc_port_t port)
{
	int		s, event;

	ksvr_entry1(dipc_kobj_port_activate, port);

	if (port->dipc_kobj_active_list != IP_NULL)
		return 0;

	s = sploff();
	/*simple_lock(&dipc_kobj_active_lock);*/

	port->dipc_kobj_active_list = IP_DEAD;

	if (dipc_kobj_active_tail == IP_DEAD) {
		dipc_kobj_active_head = port;
		dipc_kobj_active_tail = port;
	} else {
		assert(dipc_kobj_active_tail != IP_NULL);
		dipc_kobj_active_tail->dipc_kobj_active_list = port;
		dipc_kobj_active_tail = port;
	}

	if (dipc_kobj_port_wanted > 0) {
		dipc_kobj_port_wanted--;
		event = (int) &dipc_kobj_port_wanted;
	} else {
		event = 0;
	}

	/*simple_unlock(&dipc_kobj_active_lock);*/
	splon(s);

	return event;
}


static ipc_port_t dipc_kobj_port_grab()
{
	ipc_port_t	port;
	int		s, w;

	s = sploff();
	/*simple_lock(&dipc_kobj_active_lock);*/

	if ((port = dipc_kobj_active_head) != IP_DEAD) {
		dipc_kobj_active_head = port->dipc_kobj_active_list;
		if (dipc_kobj_active_tail == port) {
			dipc_kobj_active_tail = IP_DEAD;
			assert(dipc_kobj_active_head == IP_DEAD);
		}
		w = 0;
	} else {
		dipc_kobj_port_wanted++;
		w = 1;
	}

	/*simple_unlock(&dipc_kobj_active_lock);*/
	splon(s);

	if (w) {
		assert_wait(&dipc_kobj_port_wanted, FALSE);
	}

	return port;
}


ipc_port_t dipc_kobj_port_select()
{
	ipc_port_t	port;

	ksvr_entry0(dipc_kobj_port_select);

	while ((port = dipc_kobj_port_grab()) == IP_DEAD) {
		thread_block((void (*)()) 0);
	}

	assert(port != IP_DEAD);
	assert(port->dipc_ksvr_id == ITH_NULL);

	port->dipc_kobj_active_list = IP_DEAD;
	port->dipc_ksvr_id = current_thread();

	/*
	 *	Take a reference to make sure that this
	 *	port doesn't go away until the message
	 *	queue is empty and this kobj_server thread
	 *	deactivates it.
	 */
	ip_reference(port);

	return port;
}


void dipc_kobj_port_deactivate(ipc_port_t port)
{
	ksvr_entry1(dipc_kobj_port_deactivate, port);

	assert(port != IP_NULL);
	assert(port->dipc_kobj_active_list != IP_NULL);
	assert(port->dipc_ksvr_id == current_thread());

	port->dipc_ksvr_id = 0;
	port->dipc_kobj_active_list = IP_NULL;

	/*
	 *	Release the reference taken when the port
	 *	was first selected by this kobj_server thread.
	 *
	 *	ipc_port_release() is used rather than ip_release()
	 *	in case the message(s) processed cause the destruction
	 *	of the port.
	 */
	ipc_port_release(port);
}


#if	MACH_KDB
int db_kobj_active(int verbose)
{
	ipc_port_t	port;
	int		count;

	count = 0;
	port = dipc_kobj_active_head;
	while (port != IP_DEAD) {
		if (verbose) {
			ipc_port_print(port);
		} else {
			db_printf("%2d port=0x%x uid=0x%x pset=0x%x\n",
				count, port, port->dipc_uid, port->ip_pset);
		}
		count++;
		port = port->dipc_kobj_active_list;
	}

	return count;
}
#endif	/* MACH_KDB */


int	dipc_kobj_server_thread_pri = 6;
int	dipc_kobj_server_thread_policy = POLICY_FIXEDPRI;
int	dipc_kobj_server_thread_quantum = 1;

void dipc_kobj_server_thread()
{
	thread_t		self;
	ipc_port_t		port;
	ipc_mqueue_t		mqueue;
	mach_msg_return_t	mr;
	ipc_kmsg_t		kmsg;
	mach_port_seqno_t	seqno;

	ksvr_entry0(dipc_kobj_server_thread);

	self = current_thread();
	thread_set_own_priority(dipc_kobj_server_thread_pri);
	(void) thread_policy(self,
		dipc_kobj_server_thread_policy,
		dipc_kobj_server_thread_quantum);


	for (;;) {
		port = dipc_kobj_port_select();
		assert(port != IP_NULL);
		mqueue = &port->ip_messages;

		ip_lock(port);

		while (ip_active(port) && (port->ip_msgcount > 0)) {

			imq_lock(mqueue);
			ip_unlock(port);

			mr = ipc_mqueue_receive(
				mqueue,			/* message queue */
				MACH_MSG_OPTION_NONE,	/* options */
				PAGE_SIZE*8,		/* XXX: max recv size */
				MACH_MSG_TIMEOUT_NONE,	/* timeout */
				FALSE,
				(void (*)()) 0,
				(ipc_kmsg_t) &kmsg,	/* **kmsg ptr */
				(mach_port_seqno_t *) &seqno);

			imq_unlock(mqueue);

			if (mr != MACH_MSG_SUCCESS) {
				ip_lock(port);
				continue;
			}

			assert(kmsg != IKM_NULL);
			assert(kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);

			kmsg->ikm_header.msgh_seqno = seqno;
			ksvr_log5(3, "%s: recv port=%x uid=%x msgh_id=%d\n",
				__FUNC__,
				port,
				port->dipc_uid,
				kmsg->ikm_header.msgh_id);

			if ((kmsg = ipc_kobject_server(kmsg)) == IKM_NULL) {
				ip_lock(port);
				continue;
			}


			ksvr_log5(3, "%s: send port=%x uid=%x msgh_id=%d\n",
				__FUNC__,
				kmsg->ikm_header.msgh_remote_port,
				((ipc_port_t)(kmsg->ikm_header.msgh_remote_port))->dipc_uid,
				kmsg->ikm_header.msgh_id);
			mr = ipc_mqueue_send( kmsg,
					MACH_SEND_ALWAYS,
					MACH_MSG_TIMEOUT_NONE);
			if (mr != MACH_MSG_SUCCESS) {
				ksvr_log4(0, "%s: bad send: mrc=%x kmsg=%x\n",
					__FUNC__,
					mr,
					kmsg);
				ip_lock(port);
				continue;
			}

			ip_lock(port);
		}

		ip_unlock(port);
		dipc_kobj_port_deactivate(port);
	}
}


int	dipc_emmi_reply_thread_pri = 5;
int	dipc_emmi_reply_thread_policy = POLICY_FIXEDPRI;
int	dipc_emmi_reply_thread_quantum = 1;

void dipc_emmi_reply_thread()
{
	thread_t		self;
	boolean_t		priv;
	ipc_port_t		port;
	ipc_port_t		dest;
	ipc_mqueue_t		mqueue;
	mach_msg_return_t	mr;
	ipc_kmsg_t		kmsg;
	mach_port_seqno_t	seqno;

	ksvr_entry0(dipc_emmi_reply_thread);

	self = current_thread();

	(void) thread_policy(self,
		dipc_emmi_reply_thread_policy,
		dipc_emmi_reply_thread_quantum);

	port = (ipc_port_t) self->ith_other;
	mqueue = &port->ip_messages;
	if ((priv = (port == dipc_emmi_reply_priv_port)) == TRUE) {
		self->vm_privilege = TRUE;
		self->dipc_rdma_rx_group = NORMA_RDMA_GROUP_EMMI_REPLY;
		stack_privilege(self);
		assert(dipc_emmi_reply_thread_pri != 0);
		thread_set_own_priority(dipc_emmi_reply_thread_pri - 1);
	} else {
		thread_set_own_priority(dipc_emmi_reply_thread_pri);
	}

	for (;;) {

		ip_lock(port);
		imq_lock(mqueue);
		ip_unlock(port);

		mr = ipc_mqueue_receive(
			mqueue,			/* message queue */
			MACH_MSG_OPTION_NONE,	/* options */
			PAGE_SIZE*8,		/* XXX: max recv size */
			MACH_MSG_TIMEOUT_NONE,	/* timeout */
			FALSE,
			(void (*)()) 0,
			(ipc_kmsg_t) &kmsg,	/* **kmsg ptr */
			(mach_port_seqno_t *) &seqno);

		imq_unlock(mqueue);

		if (mr != MACH_MSG_SUCCESS) {
			continue;
		}

		assert(kmsg != IKM_NULL);
		assert(kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL);

		dest = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		/*ip_release(dest);*/

		kmsg->ikm_header.msgh_seqno = seqno;
		ksvr_log6(3, "%s: recv priv=%d dest=%x uid=%x msgh_id=%d\n",
				__FUNC__,
				priv,
				dest,
				dest->dipc_uid,
				kmsg->ikm_header.msgh_id);
		if ((kmsg = ipc_kobject_server(kmsg)) == IKM_NULL) {
			continue;
		}

		ksvr_log6(3, "%s: send priv=%d dest=%x uid=%x msgh_id=%d\n",
				__FUNC__,
				priv,
				kmsg->ikm_header.msgh_remote_port,
				((ipc_port_t)(kmsg->ikm_header.msgh_remote_port))->dipc_uid,
				kmsg->ikm_header.msgh_id);
		mr = ipc_mqueue_send( kmsg,
				MACH_SEND_ALWAYS,
				MACH_MSG_TIMEOUT_NONE);
		if (mr != MACH_MSG_SUCCESS) {
			ksvr_log4(0, "%s: bad send: mrc=%x kmsg=%x\n",
				__FUNC__,
				mr,
				kmsg);
			continue;
		}
	}
}
