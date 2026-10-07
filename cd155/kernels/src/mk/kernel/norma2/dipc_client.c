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
 *
 * $Id: dipc_client.c,v 1.6 1994/11/18 20:57:33 mtm Exp $
 *
 * HISTORY:
 */

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/message.h>

#include <vm/vm_map.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/meta_kmsg.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_server.h>
#include <norma2/dipc_client.h>
#include <norma2/dipc_blocked.h>
#include <norma2/norma_transport.h>
#include <norma2/norma_log.h>

typedef struct {
	unsigned long	port_moved;

	unsigned long	generic_disposal;
	unsigned long	reply_spin_hits;
	unsigned long	reply_spin_misses;

	unsigned long	enqueues_sent;
	unsigned long	enqueues_ok;
	unsigned long	enqueues_invalid;
	unsigned long	enqueues_dead;
	unsigned long	enqueues_full;
	unsigned long	enqueues_moved;

	unsigned long	buy_transits;
	unsigned long	sell_transits;
	unsigned long	queue_avails;
	unsigned long	blocked_sender_status;
	unsigned long	awaken_all_senders;
	unsigned long	migration_state_reqs;
	unsigned long	notify_polymorph;
	unsigned long	notify_dead_name;
	unsigned long	request_deadname;
} dipc_client_stats_t;

dipc_client_stats_t	dipc_client_stats;

#define	DIPC_CLIENT_STATS(a) dipc_client_stats.a

extern int	node_self();
extern void	panic( char * );
extern int	splsched();
extern void	splx( int );

extern void	dipc_port_lock(ipc_port_t);
extern void	dipc_port_unlock(ipc_port_t);


/*
 *	If the server indicates that the port has moved,
 *	update the proxy and repost the request to
 *	the next guess as to where the port is.
 */
static boolean_t dipc_client_chase_port(
	rpc_handle_t	rpc,
	rpc_class_t	class,
	void		(*callback)( rpc_handle_t, rpc_notify_t ),
	rpc_notify_t	arg)
{
	dipc_header_t	*reply;
	ipc_port_t	port;

	client_entry4(dipc_client_chase_port, rpc, class, callback, arg);

	reply = (dipc_header_t *) rpc_arguments(rpc);

	if (reply->status != DIPC_DEST_MOVED) {
		client_log2(3, "dipc_client_chase_port: !moved (0x%x)\n", rpc);
		return FALSE;
	}

	port = dipc_port_lookup(reply->uid);
	assert(port != IP_NULL);
	port->dipc_node = reply->node;
	rpc_send_request(reply->node, class, rpc, callback, arg);

	client_log3(0, "dipc_client_chase_port: resent to %d, (0x%x)\n",
		port->dipc_node, rpc);

	DIPC_CLIENT_STATS(port_moved++);
	return TRUE;
}


/*
 *	Invoked from interrupt level when a callback has been
 *	installed.
 *
 *	If the port has moved, chase it.
 *
 *	Otherwise, wakeup the client thread.
 */
static void dipc_reply_arrival(rpc_handle_t rpc, rpc_notify_t arg)
{
	boolean_t	chasing;
	rpc_class_t	class;

	client_entry2(dipc_reply_arrival, rpc, arg);

	class = (rpc_class_t) arg;

	chasing = dipc_client_chase_port(rpc, class, dipc_reply_arrival, arg);
	if (chasing == FALSE) {
		client_log4(3, "dipc_reply_arrival: rpc=%x arg=%x e=0x%x\n",
			rpc, arg, rpc_arguments(rpc));
		thread_wakeup_one((int) rpc_arguments(rpc));
	}
}


/*
 *	Invoked from interrupt level when a notification RPC
 *	has completed.
 *
 *	If given a non-zero notification argument, it can
 *	post a wakeup {currently an unexploited hook}.
 */
static void dipc_generic_rpc_handle_disposal(
	rpc_handle_t	rpc,
	rpc_notify_t	arg )
{
	client_entry2(dipc_generic_rpc_handle_disposal, rpc, arg);

	if (arg != 0)
		thread_wakeup((int) arg);
	rpc_handle_free(rpc);

	DIPC_CLIENT_STATS(generic_disposal++);
}


/*
 *	Wait for a previous request to complete.
 *
 *	Note that if a callback has been installed, all redirect
 *	processing will be handled from interrupt level.
 */
int	dipc_reply_spin_count = 36;
int	dipc_reply_spin_bucket[50];
int db_reply_spin(int reset)
{
	int	i, c;

	c = 0;
	if (reset) {
		for (i = 0; i < dipc_reply_spin_count; i++)
			dipc_reply_spin_bucket[i] = 0;
		return 0;
	}

	for (i = 0; i < dipc_reply_spin_count; i++) {
		c += dipc_reply_spin_bucket[i];
		db_printf("%3dus %6d\n", i * 5, dipc_reply_spin_bucket[i]);
	}
	return c;
}


static void dipc_client_await_reply(rpc_handle_t rpc, rpc_class_t class)
{
	boolean_t	already, chasing;
	int		s, spinout, bucket;

	client_entry2(dipc_client_await_reply, rpc, class);

	spinout = dipc_reply_spin_count;
	bucket = 0;
	while (!rpc_send_ready(rpc) && (spinout-- > 0))
		delay(5), bucket++;

	do {
		while (!rpc_send_ready(rpc)) {
			client_log2(3,
	"dipc_client_await_reply: reply not waiting for handle 0x%x\n", rpc);

			/*
			 *	Conditionally install a callback.
			 *
			 *	The callback will not be installed
			 *	if the reply has already arrived.
			 */
			s = splsched();
			already = rpc_set_callback(rpc,
					dipc_reply_arrival,
					(rpc_notify_t) class);

			if (already == FALSE) {
				/*
				 *	A callback was installed, so all
				 *	redirect processing associated with
				 *	this port will be handled from
				 *	interrupt level.
				 */
				client_log2(3,
	"dipc_client_await_reply: callback installed for handle 0x%x\n", rpc);

				assert_wait((int) rpc_arguments(rpc), FALSE);
				DIPC_CLIENT_STATS(reply_spin_misses++);

				splx(s);
				thread_block((void (*)()) 0);
				client_log2(3,
	"dipc_client_await_reply: awake, rpc=0x%x\n", rpc);
				return;
			}

			/*
			 *	Reply has already arrived...cancel the
			 *	the wait.
			 */
			client_log2(2,
	"dipc_client_await_reply: reply won race on handle 0x%x\n", rpc);
			splx(s);
		}

		DIPC_CLIENT_STATS(reply_spin_hits++);
		dipc_reply_spin_bucket[bucket]++;

		/*
		 *	if the port has since been moved, follow it
		 *	and resubmit the request.
		 *
		 *	otherwise, the reply has arrived.
		 */
		chasing = dipc_client_chase_port(rpc, class,
				dipc_reply_arrival, (rpc_notify_t) class);
	} while (chasing);
}


/*
 *	Attempt to enqueue a message on a remote port.
 */
dipc_enqueue_id_t dipc_kmsg_enqueue_request(
	ipc_port_t		port,
	ipc_kmsg_t		kmsg,
	mach_msg_option_t	options,
	rdma_token_t		rdma)
{
	extern	int		dipc_fastpath_max;

	dipc_enqueue_op_t	req;
	rpc_handle_t		rpc;
	rpc_node_t		node;

	client_entry4(dipc_kmsg_enqueue_request, port, kmsg, options, rdma);

	/*
	 *  Lock the port until the enqueue reply is processed.
	 */
	dipc_port_lock(port);

	/*
	 *	At some point, this routine will decide if it
	 *	should pack the kmsg into the payload bay if the
	 *	kmsg is not complex and can fit in the remainder
	 *	of the space...until then, always go for a
	 *	meta-kmsg enqueue...
	 */
	rpc = rpc_handle_alloc(NORMA_RPC_GROUP_INITIATOR, TRUE,
			sizeof(dipc_enqueue_op_t));

	req = (dipc_enqueue_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_REQUEST_ENQUEUE;
	req->request.status = ~0;
	req->request.node = (node = port->dipc_node);
	req->request.uid = port->dipc_uid;
	req->options = options;
	req->token = rdma;
	req->kmsg_size = ikm_plus_overhead(kmsg->ikm_header.msgh_size);

	/*
	 *  If this is a candidate for a fastpath enqueue, copy the kmsg
	 *  and set the type.
	 */
	if (   (kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_NET) &&
	     ! (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) &&
	       (kmsg->ikm_header.msgh_size <= dipc_fastpath_max)) {
		bcopy(&kmsg->ikm_header, req + 1, kmsg->ikm_header.msgh_size);
		req->kmsg_type = IKM_KMSG_TYPE_NET;
	} else {
		req->kmsg_type = IKM_KMSG_TYPE_META;
	}

	port->dipc_enqueue_in_flight++;

	rpc_send_request(node, NORMA_RPC_CLASS_ENQUEUE, rpc, 0, 0);
	DIPC_CLIENT_STATS(enqueues_sent++);

	return rpc;
}


/*
 *	Collect the reply from an enqueue request.
 */
int dipc_kmsg_enqueue_await(
	dipc_enqueue_id_t	id,
	ipc_port_t		port )
{
	rpc_handle_t		rpc;
	dipc_enqueue_op_t	req;
	int			cc;

	client_entry2(dipc_kmsg_enqueue_await, id, port);

	rpc = (rpc_handle_t) id;
	req = (dipc_enqueue_op_t) rpc_arguments(rpc);

	dipc_client_await_reply(rpc, NORMA_RPC_CLASS_ENQUEUE);

	port->dipc_enqueue_in_flight--;

	/*
	 *  Now that we have the enqueue reply, unlock the port.
	 */
	dipc_port_unlock(port);

	switch (req->request.status) {
	case DIPC_OK:
		client_log1(3, "dipc_kmsg_enqueue_await: DIPC_OK\n");
		cc = (req->kmsg_type == IKM_KMSG_TYPE_NET)
			? DIPC_QUEUE_KMSG
			: DIPC_QUEUE_MKMSG;
		DIPC_CLIENT_STATS(enqueues_ok++);
		break;

	case DIPC_DEST_INVALID:
		client_log2(1, "dipc_kmsg_enqueue_await: port 0x%x DIPC_DEST_INVALID\n",port);
		cc = DIPC_QUEUE_INVALID;
		DIPC_CLIENT_STATS(enqueues_invalid++);
		break;

	case DIPC_DEST_DEAD:
		client_log2(1, "dipc_kmsg_enqueue_await: port 0x%x DIPC_DEST_DEAD\n",port);
		cc = DIPC_QUEUE_DEAD;
		DIPC_CLIENT_STATS(enqueues_dead++);
		break;

	case DIPC_DEST_FULL:
		/*
		 *	Getting a DIPC_DEST_FULL doesn't mean too much
		 *	if queue avail notification has been serviced
		 *	ahead of servicing this reply (and it can
		 *	on the whim of the scheduler...).
		 *
		 */
		client_log1(1, "dipc_kmsg_enqueue_await: DIPC_DEST_FULL\n");
		if (port->dipc_early_queue_avail > 0) {
			if (port->dipc_enqueue_in_flight == 0)
				port->dipc_early_queue_avail = 0;
			else
				port->dipc_early_queue_avail--;
			cc = DIPC_QUEUE_WAS_FULL;
		} else {
			cc = DIPC_QUEUE_FULL;
		}
		DIPC_CLIENT_STATS(enqueues_full++);
		break;

	case DIPC_DEST_MOVED:
		client_log1(0, "dipc_kmsg_enqueue_await: DIPC_DEST_MOVED\n");
		assert(0);
		panic("dipc_kmsg_enqueue_await: DIPC_DEST_MOVED?");
		cc = DIPC_QUEUE_DEAD;
		DIPC_CLIENT_STATS(enqueues_moved++);
		break;

	default:
		client_log2(0, "dipc_kmsg_enqueue_await: unknown status %x\n",
					req->request.status);
		assert(0);
		panic("dipc_kmsg_enqueue_await: unknown reply status");
		cc = DIPC_QUEUE_DEAD;
		break;
	}

	rpc_handle_free(rpc);
	return cc;
}


/*
 *	Internal common routine for buying and selling transits.
 */
static dipc_return_t dipc_transit_request(
	rpc_node_t	node,
	dipc_uid_t	uid,
	long		*transitp,
	int		type,
	boolean_t	dispose )
{
	dipc_transit_op_t	req;
	rpc_handle_t		rpc;
	dipc_return_t		cc;

	client_entry5(dipc_transit_request, node, uid, *transitp, type,dispose);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_transit_request));

	req = (dipc_transit_op_t) rpc_arguments(rpc);

	req->request.type = type;
	req->request.status = ~0;
	req->request.node = node;
	req->request.uid = uid;
	req->transits = *transitp;

	if (dispose) {
		cc = DIPC_OK;
		rpc_send_request(node,
			NORMA_RPC_CLASS_CONTROL,
			rpc,
			dipc_generic_rpc_handle_disposal, 0);
	} else {
		rpc_send_request(node, NORMA_RPC_CLASS_CONTROL, rpc, 0, 0);
		dipc_client_await_reply(rpc, NORMA_RPC_CLASS_CONTROL);

		cc = req->request.status;
		*transitp = req->transits;

		rpc_handle_free(rpc);
	}
	return cc;
}


/*
 *	Acquire transits.
 */
dipc_return_t dipc_buy_transits(
	rpc_node_t	node,
	dipc_uid_t	uid,
	long		*transitp )
{
	client_entry3(dipc_buy_transits, node, uid, *transitp);

	DIPC_CLIENT_STATS(buy_transits++);
	return dipc_transit_request(node, uid, transitp,
			DIPC_REQUEST_BUY_TRANSITS, FALSE);
}


/*
 *	Return transits.
 */
dipc_return_t dipc_sell_transits(
	rpc_node_t	node,
	dipc_uid_t	uid,
	long		*transitp )
{
	client_entry3(dipc_sell_transits, node, uid, *transitp);

	DIPC_CLIENT_STATS(sell_transits++);
	return dipc_transit_request(node, uid, transitp,
			DIPC_REQUEST_SELL_TRANSITS, TRUE);
}


/*
 *	Inform a node that a port's message queue
 *	is no longer full.
 */
dipc_return_t dipc_notify_queue_avail( rdma_node_t node, dipc_uid_t uid )
{
	dipc_queue_avail_op_t	req;
	rpc_handle_t		rpc;

	client_entry2(dipc_notify_queue_avail, node, uid);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_queue_avail_request));

	req = (dipc_queue_avail_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_NOTIFY_QUEUE_AVAIL;
	req->request.status = ~0;
	req->request.node = node;
	req->request.uid = uid;

	rpc_send_request(node,
		NORMA_RPC_CLASS_CONTROL,
		rpc,
		dipc_generic_rpc_handle_disposal, 0);

	DIPC_CLIENT_STATS(queue_avails++);
	return DIPC_OK;
}


/*
 *	Given a port, notify all remotely blocked senders.
 */
void dipc_awaken_all_remote_senders(ipc_port_t port, dipc_uid_t uid)
{
	rdma_node_t	node;
	dipc_return_t	cc;

	client_entry2(dipc_awaken_all_remote_senders, port, uid);

	if (port->dipc_blocked_sender_count == 0)
		return;

	if (dipc_blocked_sender_extract(port, &node) != TRUE)
		return;	/* nothing to do */

	DIPC_CLIENT_STATS(awaken_all_senders++);
	do {
		if ( dipc_notify_queue_avail(node, uid) != DIPC_OK ) {
			panic("dipc_awaken_all_remote_senders");
		}
		cc = dipc_blocked_sender_extract( port, &node );
	} while ( cc == TRUE );
}


/*
 *	WARNING: always favors local senders!
 *
 *	When the "fairness" variable per port is installed,
 *	we can use it to decide whether to wake a local or a
 *	remote sender.
 */
void dipc_unblock_one_sender( ipc_port_t port )
{
	ipc_thread_queue_t	senders;
	ipc_thread_t		sender;
	rdma_node_t		node;
	dipc_return_t		cc;

	client_entry1(dipc_unblock_one_sender, port);

	senders = &port->ip_blocked;
	if ((sender = ipc_thread_queue_first(senders)) != ITH_NULL) {
		ipc_thread_rmqueue(senders, sender);
		sender->ith_state = MACH_MSG_SUCCESS;
		thread_go(sender);
		return;
	}

	if (port->dipc_blocked_senders == 0)
		return;

	if (port->dipc_blocked_sender_count == 0)
		return;

	if (dipc_blocked_sender_extract(port, &node) == TRUE) {
		cc = dipc_notify_queue_avail(node, port->dipc_uid);
		assert(cc == DIPC_OK);
		return;
	}
}


/*
 *	Get the migration state of a principal.
 */
dipc_return_t dipc_migration_state_request(
	ipc_port_t			port,
	dipc_port_migration_state	*pstate )
{
	dipc_migration_op_t	req;
	rpc_handle_t		rpc;
	rpc_node_t		node;
	dipc_return_t		cc;

	client_entry2(dipc_migration_state_request, port, pstate);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_migration_request));

	req = (dipc_migration_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_REQUEST_MIGRATION;
	req->request.status = ~0;
	req->request.node = (node = port->dipc_node);
	req->request.uid = port->dipc_uid;
	req->from = node_self();

	rpc_send_request(node, NORMA_RPC_CLASS_ENQUEUE, rpc, 0, 0);
	dipc_client_await_reply(rpc, NORMA_RPC_CLASS_ENQUEUE);

	cc = req->request.status;
	*pstate = req->state;

	rpc_handle_free(rpc);
	DIPC_CLIENT_STATS(migration_state_reqs++);
	return cc;
}


/*
 *	Polymorph a principal into a forwarding proxy.
 */
dipc_return_t dipc_notify_polymorph_principal( ipc_port_t port )
{
	dipc_polymorph_op_t	req;
	rpc_handle_t		rpc;
	rpc_node_t		node;
	dipc_return_t		cc;

	client_entry1(dipc_notify_polymorph_principal, port);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_polymorph_request));

	req = (dipc_polymorph_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_NOTIFY_POLYMORPH;
	req->request.status = ~0;
	req->request.node = (node = port->dipc_node);
	req->request.uid = port->dipc_uid;
	req->from = node_self();

	rpc_send_request(node, NORMA_RPC_CLASS_CONTROL, rpc, 0, 0);
	dipc_client_await_reply(rpc, NORMA_RPC_CLASS_CONTROL);
	cc = req->request.status;
	rpc_handle_free(rpc);

	DIPC_CLIENT_STATS(notify_polymorph++);
	return cc;
}


/*
 *	Inform a previously registered node that a right has died.
 *
 *	(A compose-and-dispose RPC).
 */
dipc_return_t dipc_notify_dead_name_request( rpc_node_t node, dipc_uid_t uid )
{
	rpc_handle_t		rpc;
	dipc_notify_dead_op_t	req;

	client_entry2(dipc_notify_dead_name_request, node, uid);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_notify_dead_name_request));

	req = (dipc_notify_dead_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_NOTIFY_DEAD_NAME;
	req->request.status = ~0;
	req->request.node = node;
	req->request.uid = uid;

	rpc_send_request(node,
		NORMA_RPC_CLASS_CONTROL,
		rpc,
		dipc_generic_rpc_handle_disposal, 0);

	DIPC_CLIENT_STATS(notify_dead_name++);
	return DIPC_OK;

}


/*
 *	Request dead name notification.
 */
dipc_return_t dipc_dead_name_request( ipc_port_t port )
{
	rpc_handle_t		rpc;
	dipc_request_dead_op_t	req;
	rpc_node_t		node;
	dipc_return_t		cc;

	client_entry1(dipc_dead_name_request, port);

	rpc = rpc_handle_alloc( NORMA_RPC_GROUP_INITIATOR,
			TRUE,
			sizeof(struct dipc_request_dead_name));

	req = (dipc_request_dead_op_t) rpc_arguments(rpc);

	req->request.type = DIPC_REQUEST_DEAD_NAME;
	req->request.status = ~0;
	req->request.node = (node = port->dipc_node);
	req->request.uid = port->dipc_uid;
	req->from = node_self();

	rpc_send_request(node, NORMA_RPC_CLASS_CONTROL, rpc, 0, 0);
	dipc_client_await_reply(rpc, NORMA_RPC_CLASS_CONTROL);

	cc = req->request.status;

	rpc_handle_free(rpc);
	DIPC_CLIENT_STATS(request_deadname++);
	return cc;
}


extern	int	dipc_serialize;

void dipc_port_lock(ipc_port_t port)
{
	client_entry1(dipc_port_lock, port);

	if (dipc_serialize) {
		while (port->dipc_serialize_lock) {
			client_log3(2, "%s: port %x busy, going to sleep.\n",
				__FUNC__, port);
			assert_wait((int) port, FALSE);
			port->dipc_serialize_waiters++;
			thread_block(0);
		}
		assert(port->dipc_serialize_lock == 0);
		port->dipc_serialize_lock = 1;
	}
}


void dipc_port_unlock(ipc_port_t port)
{
	client_entry1(dipc_port_unlock, port);

	if (dipc_serialize) {
		assert(port->dipc_serialize_lock == 1);

		port->dipc_serialize_lock = 0;
		if (port->dipc_serialize_waiters > 0) {
			client_log4(2, "%s: port %x NOT busy, waking. (%d)\n",
				__FUNC__, port, port->dipc_serialize_waiters);
			port->dipc_serialize_waiters--;
			thread_wakeup_one((int) port);
		}
	}
}

db_dipc_client_stats()
{
	dipc_client_stats_t	*p = &dipc_client_stats;

        db_printf("dipc_client stats:\n");
	db_printf("  port_moved             %8d",   p->port_moved);
	db_printf("  generic_disposal       %8d\n", p->generic_disposal);
	db_printf("  reply_spin_hits        %8d",   p->reply_spin_hits);
	db_printf("  reply_spin_misses      %8d\n", p->reply_spin_misses);

	db_printf("  enqueues_sent          %8d",   p->enqueues_sent);
	db_printf("  enqueues_ok            %8d\n", p->enqueues_ok);
	db_printf("  enqueues_invalid       %8d",   p->enqueues_invalid);
	db_printf("  enqueues_dead          %8d\n", p->enqueues_dead);
	db_printf("  enqueues_full          %8d",   p->enqueues_full);
	db_printf("  enqueues_moved         %8d\n", p->enqueues_moved);

	db_printf("  buy_transits           %8d",   p->buy_transits);
	db_printf("  sell_transits          %8d\n", p->sell_transits);
	db_printf("  queue_avails           %8d",   p->queue_avails);
	db_printf("  blocked_sender_status  %8d\n", p->blocked_sender_status);
	db_printf("  awaken_all_senders     %8d",   p->awaken_all_senders);
	db_printf("  migration_state_reqs   %8d\n", p->migration_state_reqs);
	db_printf("  notify_polymorph       %8d",   p->notify_polymorph);
	db_printf("  notify_dead_name       %8d\n", p->notify_dead_name);
	db_printf("  request_deadname       %8d\n", p->request_deadname);

	return 0;
}
