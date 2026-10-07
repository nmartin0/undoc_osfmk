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
 * Migrate a Mach IPC receive right between nodes in a DIPC (Distributed IPC)
 * system/cluster.
 *
 * $Id: dipc_migrate.c,v 1.5 1994/11/18 20:57:52 mtm Exp $
 *
 * HISTORY:
 *
 */

/*
 * When we notice a receive-right in a message we (the client) request the
 * existing principal's port state.  This call may block to allocate an RPC
 * handle and while waiting for an RPC reply.  This call sends a request to
 * the dipc enqueue server (for serialization).  Upon receiving this request,
 * the enqueue server calls: dipc_get_port_migration_state(uid, state, from).
 * The reply is sent back to the originating client when the call to
 * dipc_get_port_migration_state() returns. The principal state is injected
 * into a new principal port structure. To finishing migration the client
 * calls dipc_notify_polymorph_principal(port).  This call may block allocating
 * an RPC handle but does not wait for an RPC reply (it's of the
 * "compose-and-dispose" variety) and is disposed of in the background when
 * the reply does arrive.  "dipc_notify_polymorph_principal()" call sends a
 * request to the dipc control server (perhaps in the future it will be
 * directed to a new dipc migration thread).  Upon receiving this request,
 * the control server calls: dipc_port_migrate_messages(port)' to send enqueued
 * messages to the new principal.  The RPC reply to the client (for which the
 * client doesn't care about) is sent after the above call returns. Once the
 * enqueued messages are transfered the old-principal transforms itself into
 * a forwarding proxy, hence all messages from this point on will be redirected
 * to the new principal.
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>

#include <kern/queue.h>
#include <kern/assert.h>
#include <kern/host.h>

#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_thread.h>

#include <kern/ipc_kobject.h>

#include <rpc_rdma/rdma.h>

#include <norma2/dipc_uid.h>
#include <norma2/dipc_port.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_mqueue.h>
#include <norma2/meta_kmsg.h>
#include <norma2/norma_log.h>

/*
 * Convert a principal to a forwarding proxy.
 *
 * implicit inputs:
 *	'port' is already marked as a proxy due to dipc_mqueue_send()
 *	restrictions.
 *	queue-limit is set to zero
 *
 * inputs:
 *	port	port which is turned into a forwarding proxy.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	queue limit reinstated, all remote & local blocked senders are wakened.
 */

void
polymorph_principal( ipc_port_t port )
{
	ipc_port_t	dest;

	migrate_entry1(polymorph_principal, port);

	ip_lock( port );

	assert(port->ip_references > 0);

	assert((ip_kotype(port) == IKOT_NONE) ||
		(ip_kotype(port) == IKOT_PAGER) ||
		(ip_kotype(port) == IKOT_PAGER_TERMINATING));

	/*
	 * remove reference taken on the destination port of the kmsg
	 * which held this receive right in it's body.
	 * ---> WARNING <----
	 *	ip_destination == ip_receiver ala union in ipc_port_t struct.
	 */
	if ( (dest = port->ip_destination) != IP_NULL )
		ipc_port_release(dest);

	/*
	 * set receiver and bogus name that is not IP_DEAD/IP_NULL
	 */
	port->ip_receiver = ipc_space_remote;
	port->ip_receiver_name = 1; /* name used by ipc_port_alloc_special */

	/*
	 * the receive right is no longer here.
	 */
	ipc_port_clear_receiver(port);

	/*
	 * forwarding-proxy send-rights and transits have been set 
	 * accordingly in dipc_get_port_migration_state().
	 *
	 * forwarding proxy's get removed by a fake no-more senders
	 * notification. When the owning task is going away the no-more
	 * senders logic kicks in. 'dipc_no_more_sender()' is called for a
	 * proxy which consumes the send-once right, clobbers the nm_senders
	 * request and most importantly calls dipc_port_remove_try() which
	 * then removes the proxy.
	 */
	if ( port->ip_nsrequest == IP_NULL ) {
		port->ip_sorights++;	/* for ipc_port_release_sonce() */
		ip_reference(port);
	}
        port->ip_nsrequest = port;

        /* port->ip_dnrequests unchanged */
        port->ip_pdrequest = IP_NULL;
        port->ip_pset = IPS_NULL;

	assert( port->dipc_is_proxy == TRUE);	/* set in message migrate */
	assert( port->dipc_forward == TRUE);	/* ditto.. */

	port->dipc_migrate_state = DIPC_MS_AT_REST;

	ip_unlock( port );

	DIPC_PORT_LOG("EXIT as a forwarding proxy",port);
}

/*
 * Called by the migration server thread (currently dipc_control thread) to
 * complete a port migration. Port 'state' has been previously transfered to
 * the new principal. Now send any enqueued messages, finally mark the port
 * as a forwarding proxy.
 *
 * inputs:
 *	port	port pointer which is still a principal, although is marked as
 *		in the process of migration.
 *
 * implicit inputs:
 *	port is unlocked.
 *
 * outputs:
 *	none.
 *
 * implicit outputs:
 *	port is unlocked.
 *
 * side effects:
 *	Any enqueued messages are tranfered to the new principal port.
 *	This 'old-principal' port transforms into a forwarding proxy.
 */

void
dipc_port_migrate_messages( ipc_port_t port )
{
	kern_return_t		kr;
	ipc_mqueue_t		mqueue;
	ipc_kmsg_queue_t	kmsg_q;
	ipc_kmsg_t		kmsg, next;
	int			migrate_msgs=0;
	char			*debug_type;

	migrate_entry1(dipc_port_migrate_messages, port);

	assert( port != IP_NULL );
	assert( ! DIPC_IS_PROXY(port) );

	/*
	 * lock message queue and port to secure privacy....
	 * Set this migrating princicple to a forwarding proxy so
	 * dipc_mqueue_send() will migrate/send messages to the new principal
	 * and not inadvertently destroy the port.
	 */
	ip_lock( port );
	mqueue = &port->ip_messages;
	imq_lock( mqueue );
	port->dipc_is_proxy = TRUE;	/* for dipc_mqueue_send() */
	port->dipc_forward = TRUE;
	ip_unlock( port );

	kmsg_q = &mqueue->imq_messages;

	/*
	 * transfer the kmsgs to the new principal.
	 */
	kmsg = ipc_kmsg_queue_first( kmsg_q );
	for (; kmsg != IKM_NULL; kmsg = next) {

		assert( port->ip_msgcount > 0 );

		next = ipc_kmsg_queue_next( kmsg_q, kmsg );

		ipc_kmsg_rmqueue( kmsg_q, kmsg );

		assert( kmsg != IKM_NULL );

		assert( (kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_META
			|| kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_NET
			|| kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL) );

		if ( kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_META ) {
			/*
			 * A port reference was taken during the meta-kmsg
			 * enqueue operation; lose it here.
			 */
			ip_release(port);
		}
		else {
			mach_msg_bits_t		bits;
			mach_msg_type_name_t	type_name;

			bits = kmsg->ikm_header.msgh_bits;
			type_name = MACH_MSGH_BITS_REMOTE(bits);

			/* Preserve a send right by taging the kmsg as a
			 * migrating message. Local messages increment the
			 * send{-once} right count and take a reference during
			 * the enqueue act. The act of sending the message will
			 * consume a send{-once} right and the reference.
			 * Make sure we have a {bogus} transit to cover this
			 * send as we have already marked the port as a proxy.
			 */
			kmsg->ikm_header.msgh_bits |= MACH_MSGH_BITS_MIGRATED;
		}

		/*
		 * send the kmsg.
		 */
		kr = dipc_mqueue_send( port,
					kmsg,
					MACH_SEND_ALWAYS,
					MACH_MSG_TIMEOUT_NONE);

		/* one less, bell to answer.....one less, message to fry.... */
		port->ip_msgcount--;

		/*
		 * if success, then the kmsg has been disposed of.
		 * Otherwise, we do the right thing.
		 */
		if ( kr == KERN_SUCCESS ) {
			migrate_msgs++;
			continue;
		}

		migrate_log3(0, "%s: dipc_mqueue_send failed, kr %d\n",
					__FUNC__, kr);

		switch ( kmsg->ikm_kmsg_type ) {
		  case IKM_KMSG_TYPE_META:
			debug_type = "meta-kmsg";
			rdma_flush_endpoint(
					((meta_kmsg_t)kmsg)->mkm_rdma_token );
			(void)meta_kmsg_free( (meta_kmsg_t)kmsg );
			break;

		  case IKM_KMSG_TYPE_NET:
			debug_type = "network-kmsg";
			(void)ikm_free( kmsg );
			break;

		  case IKM_KMSG_TYPE_LOCAL:
			debug_type = "local-kmsg";

			/* following line from ipc_port_destroy() */
			kmsg->ikm_header.msgh_remote_port = MACH_PORT_NULL;

			ipc_kmsg_destroy( kmsg );
			break;

		  default:
			panic("dipc_port_migrate_messages: bad msg port 0x%x\n",port);
			break;
		}
		migrate_log5(0,
			"%s: port 0x%x unable to migrate %s @ kmsg 0x%x\n",
			__FUNC__, port, debug_type, kmsg);
		printf("MM: port 0x%x unable to migrate %s @ kmsg 0x%x\n",
			port, debug_type, kmsg);
	}

	assert( port->ip_msgcount == 0 );

	migrate_log4(2, "%s: uid %x sent %d msgs\n",
			__FUNC__, port->dipc_uid, migrate_msgs);
	DIPC_PORT_LOG("after-send",port);

	imq_unlock( mqueue );

	/*
	 * Messages have been migrated, now complete the metamorphisis...
	 * change principal --> proxy
	 */
	polymorph_principal( port );

	/*
	 * release any attempted sends which happened while the port was
	 * migrating. Reinstate the original queue limit.
	 */
	ip_lock(port);
	port->ip_qlimit = port->dipc_qlimit;

	/* do local senders */
	{
		ipc_thread_t	th;

		while( (th=ipc_thread_dequeue(&port->ip_blocked)) != ITH_NULL) {
			th->ith_state = MACH_MSG_SUCCESS;
			migrate_log5(1,
				"%s: port %x uid %x released blk sender %x\n",
				__FUNC__,port,port->dipc_uid,th);
			thread_go(th);
		}
	}
	ip_unlock(port);

	/* now do remote senders */
	if ( port->dipc_blocked_sender_count > 0 )
		dipc_awaken_all_remote_senders( port, port->dipc_uid );

	migrate_log3(2, "%s: uid %x exit\n", __FUNC__, port->dipc_uid);
}

/*
 * count the send rights associated with local kmsgs enqueued on a 
 * specific port. When local IPC enqueues a message a real copyin is performed.
 * The copyin incrememnts the destination port's reference and send{-once}
 * right count. When DIPC does a remote enqueue we do NOT have the destination
 * port bits from the message header so we ONLY take a port reference. This
 * routine is called to count the send{-once} rights associated with locally
 * enqueued kmsgs. The send right count figures into the transit accounting
 * during migration.
 *
 * inputs:
 *	port	port of interest.
 *
 * output:
 *	# of kmsgs which are of the type 'local'.
 */

void
count_queued_send_rights(
		ipc_port_t	port,
	 	int		*srights,
		int		*sorights )
{
	ipc_mqueue_t		mqueue;
	ipc_kmsg_queue_t	kmsg_q;
	register ipc_kmsg_t	kmsg, next;
	int			sr=0, sor=0;

	assert( port != IP_NULL );

	*srights = 0;
	*sorights = 0;

	ip_lock( port );

	if( port->ip_msgcount <= 0 ) {
		ip_unlock( port );
		return;
	}

	mqueue = &port->ip_messages;
	imq_lock( mqueue );
	ip_unlock( port );

	kmsg_q = &mqueue->imq_messages;

	/*
	 * count local kmsgs
	 */
	kmsg = ipc_kmsg_queue_first( kmsg_q );
	for (; kmsg != IKM_NULL; kmsg = next) {
		register mach_msg_bits_t	bits;
		register mach_msg_type_name_t	name;

		next = ipc_kmsg_queue_next( kmsg_q, kmsg );

		assert( (kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_META
			|| kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_NET
			|| kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL) );

		if ( kmsg->ikm_kmsg_type == IKM_KMSG_TYPE_LOCAL ) {
			/*
			 * create bogus send right & it's reference to
			 * compensate for for the correct behavior of
			 * dipc_mqueue_send()
			 */
			bits = kmsg->ikm_header.msgh_bits;
			name = MACH_MSGH_BITS_REMOTE(bits);

			assert( (name == MACH_MSG_TYPE_PORT_SEND_ONCE ||
				name == MACH_MSG_TYPE_PORT_SEND) );

			/*
			 * a send-right will be consumed during the mqueue_send.
			 * Manufacture one here.
			 */
			if ( name == MACH_MSG_TYPE_PORT_SEND_ONCE)
				sor++;
			else
				sr++;
		}
	}

	*srights = sr;
	*sorights = sor;
}

/*
 * Server side of retrieving a port state. Called by dipc control thread.
 *
 * Implicit Inputs:
 *	port is locked and referenced.
 *
 * inputs:
 *	port		principal which is now migrating.
 *	pstate		port state to be conveyed to the new node.
 *	migrate_to	node # of where the port(recv-right) will migrate to.
 *
 * outputs:
 *	none
 */

void
dipc_get_port_migration_state( 
	ipc_port_t			port,
	dipc_port_migration_state_t	pstate,
	dipc_node_t			migrate_to)
{
	long	local_srights;		/* principal's local send rights */

	int	q_srights, q_sorights;

	migrate_entry3(dipc_get_port_migration_state, port, pstate, migrate_to);

	assert ( !DIPC_IS_PROXY(port) );

	/*
	 * set the port state to 'in the process of migrating'. 
	 * Most important to set the 'dipc_node' correct as this is where any
	 * enqueued messages will be sent plus further enqueues.
	 */
	assert( port->dipc_migrate_state == DIPC_MS_AT_REST );
	port->dipc_migrate_state = DIPC_MS_MIGRATING;
	assert( migrate_to != node_self() );
	port->dipc_node = migrate_to;	/* where the buffalo will go... */

	/*
	 * Return current port state to the new principal.
	 */
	pstate->seqno = port->ip_seqno;
	pstate->msgcount = port->ip_msgcount;
	pstate->resident_node = node_self();	/* where to send deadname */
	pstate->qlimit = port->ip_qlimit;

	/*
	 * force all senders to become blocked senders by reducing the queue
	 * limit to zero. Will be reset when principal changes to a proxy.
	 */
	port->dipc_qlimit = port->ip_qlimit; /* save current Queue limit */
	port->ip_qlimit = 0;

	/*
	 * Local send-rights and transits are an interesting issue... Since we
	 * will eventually become a forwarding-proxy we will need some
	 * transits to cover our existing local send rights. Our current
	 * transit count covers all outstanding send-rights (still the
	 * principal).  Therefore we send to the new principal as the transit
	 * count (our current transit count + the local send-right count).
	 * If the new-principal used to be a proxy then my transit count
	 * will cover it's send-rights and need to be adjusted by the client
	 * side of this RPC. Since the send-right count 'ip_sright' is
	 * incremented for every transit, the local send-right count is
	 * (ip_sright - dipc_transit).  The local transit count is reset to
	 * cover just the local send-rights, as they remain here
	 * (forwarding-proxy). 
	 * A cheap way of asking the principal for some transits to cover our
	 * local send-rights.
	 */
	count_queued_send_rights( port, &q_srights, &q_sorights );

#if	MACH_ASSERT
	if ( port->dipc_transit < 0 || port->ip_srights < 0 ) {
		DIPC_PORT_LOG("PS: BAD rights", port );
		assert(0);
	}
#endif
	assert ( port->dipc_transit >= 0 );
	assert ( port->ip_srights >= 0 );

	local_srights = ( port->ip_srights - q_srights ) - port->dipc_transit;

	assert( local_srights >= 0 );

	/*
	 * generate some transits to cover send rights which will remain
	 * on this new proxy.
	 */
	pstate->transit = port->dipc_transit + (local_srights * 2);
	port->dipc_transit = local_srights * 2;
	port->ip_srights = local_srights + q_srights;

	/*
	 * handle send-once rights.
	 */
	pstate->send_once = port->ip_sorights - q_sorights;

	migrate_log6(2, "%s: uid %x minus %d from refs %d sonce %d\n",
		__FUNC__,
		port->dipc_uid,
		port->dipc_remote_sorights,
		port->ip_references,
		port->ip_sorights);

	assert( port->dipc_remote_sorights >= 0 );

	/* 
	 * Remove ip_sorights and references which cover remote_sorights.
	 * Soon to be proxy has no remote sorights.
	 */
	for(; port->dipc_remote_sorights > 0; port->dipc_remote_sorights-- ) {
		port->ip_sorights--;
		ip_release(port);
	}
	assert( port->dipc_remote_sorights == 0 );
	DIPC_PORT_LOG("exit",port);
}

/*
 * Client side of retrieving a port state. We are a proxy (new or existing)
 * which has just received a receive-right. Retrieve the remote principal's
 * state for our soon to be local principal.
 *
 * inputs:
 *	port	local port which contains the uid for the remote principal and
 *		correct forwarding node # (dipc_node).
 *
 * outputs:
 *	node where current (migrating) principal resides. Where the principal
 *	used to live.
 */

dipc_node_t
dipc_pull_port_state( ipc_port_t port )
{
	dipc_port_migration_state	pstate;
	long				remote_sorights;
	kern_return_t			kr;

	migrate_entry1(dipc_pull_port_state, port);

	/*
	 * The 'uid' derrived node can be forwarding and thus return a new
	 * node which is where the principal really lives. Eventually we
	 * determine the node where the principal really lives; 'dipc_node'.
	 * Inform that node/principal to become a proxy (migrate state becomes
	 * DIPC_MS_MIGRATING) and return principal-port's right accounting info.
	 * The new (local) principal port's migrate state is set to
	 * DIPC_MS_AT_REST. Since the old principal has not yet become a
	 * forwarding proxy, no other nodes know about this new principal
	 * except where the old principal is.
	 */

	kr = (kern_return_t) dipc_migration_state_request( port, &pstate );
	if ( kr != KERN_SUCCESS )
		panic("dipc_migration_state_request() failed, %d",kr);

	/*
	 * correctly set the principal's 'rights' accounting state.
	 */
	ip_lock(port);

	/*
	 * The transit count in the principal's state ('pstate') includes any 
	 * transits held locally (remote send rights). Here we remove any local
	 * transits as they have in essence come home.
	 */
	assert( (pstate.transit - port->dipc_transit) >= 0 );
	port->ip_srights += (pstate.transit - port->dipc_transit);
	port->dipc_transit = (pstate.transit - port->dipc_transit);

	/*
	 * add send-once rights and their accompanying reference(s).
	 * Proxys always have at least one send-once right for the
	 * no-more senders notification. Exclude this NMS right from our
	 * calculations.
	 */
	remote_sorights = pstate.send_once - (port->ip_sorights - 1);

	migrate_log6(2,
	  "%s: uid %x computed rem sonce %d, pstate.so %d ip_so %d\n",
		__FUNC__,
		port->dipc_uid,
		remote_sorights,
		pstate.send_once,
		port->ip_sorights);

	assert( remote_sorights >= 0 );

	for(; remote_sorights > 0; remote_sorights-- ) {
		port->ip_sorights++;
		port->dipc_remote_sorights++;
		ip_reference(port);
	}

	/* include NMS right. */
	assert( port->ip_sorights == pstate.send_once+1 );
	port->ip_seqno = pstate.seqno;
	port->ip_qlimit = pstate.qlimit;

	ip_unlock(port);

	/*
	 * notify the old principal to start sending any queued messages. When
	 * finished xfering messages then the old-principal transforms itself
	 * into a forwarding proxy.
	 */
	dipc_notify_polymorph_principal( port );

	assert( !DIPC_IS_PROXY(port) );

	DIPC_PORT_LOG("exit",port);

	return pstate.resident_node;
}

/*
 * receive a receive-right: To do this we must migrate the recv-right from 
 * the remote-node (the current principal) to our local node (soon to be the
 * new principal); the remote principal becomes a forwarding proxy while the
 * local proxy (possibly newly allocated) becomes the principal.
 *
 * implicit inputs:
 *	receiving user's context.
 *
 * inputs:
 *	uid	uid of remote principal port which currently holds the
 *		recv-right.
 * outputs:
 *	ipc_port_t of port which has the new receive-right.
 *
 * side effects:
 *
 */

ipc_port_t
dipc_receive_rright( dipc_uid_t	uid )
{
	ipc_port_t	principal;
	dipc_node_t	proxy_node;

	migrate_entry1(dipc_receive_rright, uid);

	/*
	* Get a real port for this uid.  If there's an existing proxy, we
	* convert it to a real principal, otherwise we allocate a new real
	* port.  We want a real port because the sender is going to turn their
	* existing principal into a proxy soon.
	*/
	assert( ! DIPC_SPECIAL_UID(uid) );

	/*
	 * Try to find the proxy. If we don't have a proxy, then we must
	 * create a proxy.
	 */
	if ( (principal = dipc_port_lookup(uid)) == IP_NULL) {
		/*
		 * unable to find an existing port with specified 'uid'.
		 * Create one or die.
		 */
		if ( (principal = dipc_proxy_create( uid )) == IP_NULL) {
			panic("dipc_receive_rright: dipc_proxy_create");
		}
		ip_lock(principal);
		DIPC_PORT_LOG("create proxy",principal);
	}
	else {
		/*
		 * We have an existing proxy.
		 */
		ip_lock(principal);

		DIPC_PORT_LOG("existing proxy",principal);

		assert( DIPC_IS_PROXY(principal) );
		assert( principal->dipc_migrate_state == DIPC_MS_AT_REST );

		principal->dipc_forward = FALSE;
	}

	/*
	 * become a principal and by virtue of the 'dipc_migrating_state' value
	 * all new enqueues will block resulting in blocked senders.
	 * 'send-always' enqueues will still happen at the old-principal until
	 * such a time as the principal transforms into a forwarding proxy.
         * 'ip_receiver' must be null because 'ipc_right_copyout()' checks this.
	 * 'principal' is locked at this point.
	 */

	principal->dipc_migrated = TRUE;
	principal->dipc_is_proxy = FALSE;
        principal->ip_receiver_name = MACH_PORT_NULL;
        principal->ip_destination = IP_NULL;

	/*
	 * There is a port reference associated with the receive right itself.
	 * Normally we would add that port reference here. Additionally we need
	 * to remove an extra proxy reference, a proxy has an additional
	 * reference (itself) that the principal does NOT have. The operations
	 * cancel each other. Looks like a NOP to me....
	 */

	ip_unlock(principal);

	DIPC_PORT_LOG("before pull-state",principal);

	/*
	 * Inform the remote migration server the principal should start
	 * migrating and return the current principal port state info.
	 */
	proxy_node = dipc_pull_port_state( principal );

	/*
	 * The correct principal state has been instantiated locally, we will
	 * now accept enqueues of the messages originally queued at the 'old'
	 * principal (soon to be a forwarding proxy). The remote dipc control
	 * thread will send all queued messages to our new principal as it's
	 * the only remote entity which knows about our new principal. Once
	 * all queued messages have been enqueued to our new principal the
	 * control thread will complete the transformation of the remote
	 * 'principal' to a 'forwarding' proxy.
	 */
        /*
         * Always request dead name notification from the new principal
	 * node so that forwarding proxy will get cleaned up. Make the
	 * call as if the node which now has a forwarding proxy made the
	 * deadname request.
         */
        (void)dipc_service_dead_name_request( proxy_node, principal );

	DIPC_PORT_LOG("exit",principal);

	/* we are a principal now */
	return	principal;
}


/*
 * We are called because 'msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) == 1'
 * hence this is a migrating message.
 *
 * inputs:
 *	uid		'uid' from remote_port in kmsg header (*dest_portp)
 *	type_name	'remote_port' send right name	
 *	dest_portp	pointer to remote_port field in a kmsg Mach header.
 *
 * outputs:
 *	KERN_SUCCESS
 *	KERN_INVALID_RIGHT
 *
 * side effects:
 *	KERN_SUCCESS: 'dest_portp' port pointer is set to a real port pointer.
 *	KERN_INVALID_RIGHT: 'dest_portp' set to IP_NULL (invalid port).
 */

kern_return_t
dipc_receive_migrating_message(
		dipc_uid_t		uid,
		mach_msg_type_name_t	type_name,
		mach_port_t		*dest_portp )
{
	ipc_port_t	port;

	migrate_entry3(dipc_receive_migrating_message, uid, type_name,
		dest_portp);

	assert( ! DIPC_SPECIAL_UID(uid) );

	/*
	 * Find associated port.
	 */
	if ( (port = dipc_port_lookup(uid)) == IP_NULL) {
		/*
		 * Must be an invalid uid; otherwise, we would have
		 * found something with dipc_port_lookup.
		 */
		port_log3(0, "%s: bad uid %x\n", __FUNC__, uid);
printf("dipc_receive_migrating_dest bad destination uid %x\n",uid);

		*dest_portp = MACH_PORT_NULL;

		return KERN_INVALID_RIGHT;
	}

	/*
	 * should NOT be a proxy!
	 */
	if ( DIPC_IS_PROXY(port) ) {
printf("dipc_receive_migrating_dest port %x is proxy?\n",port);
		port_log2(0,"dipc_receive_migrating_dest port 0x%x is proxy?\n",
			port);
		*dest_portp = MACH_PORT_NULL;

		return KERN_INVALID_RIGHT;
	}

	assert(ip_active(port));

	/*
	 * adjust send/{-once} right
	 */
	if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		/*
		 * Add a send right, notice no port reference is taken
		 * here as it was previously taken during the enqueue operation.
		 */
		port->ip_srights++;
	}
	else {
		assert(type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);
		/*
		 * No need to do anything with the soright.  We did nothing
		 * when it went off-node.  The copy out process will consume
		 * the right and reference.  We must, however, consume the
		 * reference taken by the DIPC enqueue server when the message
		 * was enqueued.
		 */
		ip_release(port);
		port->dipc_remote_sorights--;
	}

	DIPC_PORT_LOG(0,port);

	/*
	 * Set ipc_port_t port pointer in message in place of uid.
	 */

	*dest_portp = (mach_port_t)port;

	return KERN_SUCCESS;
}
