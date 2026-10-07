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
 * $Id: dipc_transit.c,v 1.6 1994/11/18 20:58:18 mtm Exp $
 * 
 * HISTORY:
 */

#include "mach_assert.h"
#include "norma_ipc.h"

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_thread.h>
#include <mach/message.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_port.h>
#include <norma2/norma_log.h>
#include <norma2/dipc_transit.h>

dipc_transit_stats_t	dipc_transit_stats;

/*
 *  A transit is the network representation of a send right and can be thought
 *  of as a distributed reference count.  Distributed reference counts are only
 *  needed for the case when no more senders notification has been requested
 *  for the port.  Assuming NMS has been requested, there are three critical
 *  invariants that govern the management of send rights with transits:
 *
 *  1. Any send right in flight within the NORMA domain must `carry' at least
 *     one tranit.
 *  2. If one or more send rights for a port exist on a node other than the
 *     port's principal node, the proxy on that node must have at least one
 *     transit.
 *  3. At all times, the sum of transits held by proxies and by send rights
 *     in flight within the NORMA domain must equal the count of transits on
 *     the principal.
 *
 *  In support of the first invariant, the following convetions are employed:
 *
 *  o  If the destination port in the message header is a send right, it will
 *     carry one transit.  This transit is `on loan' while the message is
 *     in flight within the NORMA domain and is returned to the sending node
 *     when the message is enqueued on the receiving port.
 *  o  If the reply port in the message header is a send right, it carries
 *     one transit.  This transit is retained by the receiving node.
 *  o  A send right in a message body sent from the principal node carries
 *     two transits.  These transits are retained by the receiving node.
 *  o  A send right in a message body sent from a proxy node carries one
 *     transit.  This transit is retained by the receiving node.
 *
 *  The last two cases are distinguished by the representation of the send
 *  right in the body of the message itself.
 */


/*
 *  Name:	dipc_get_transits
 *
 *  Input:	Pointer to proxy port needing transits.
 *
 *  Output:	Transits are obtained from the principal port.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	XXX  Describe locking and MP considerations.
 *
 *  Description:	This is the proxy side routine for requesting
 *			more transits.  It will send an RPC to the
 *			principal port to complete the request.
 */
#define	TRANSIT_REQ	500

void
dipc_get_transits(port)
	ipc_port_t	port;
{
	long		transit = TRANSIT_REQ;	/* requests/granted transits */
	dipc_return_t	ret = DIPC_SUCCESS;

	transit_entry1(dipc_get_transits, port);

	assert(DIPC_IS_PROXY(port));

	/*
	 *  If we are supporting NMS, we need to get transits from
	 *  the principal.
	 */
	if (port->dipc_do_nms) {
		ret = dipc_buy_transits(port->dipc_node,
					port->dipc_uid,
					&transit);
	}

	/*
	 *  If the port is dead or invalid, it doesn't matter.  Just give
	 *  some transits so we can continue.  The enqueue request will
	 *  fail later and that's when we'll clean up.
	 */
	if (ret != DIPC_SUCCESS) {
		transit = 1;
		transit_log5(0,
		  "%s: port %x uid %x buy transits failed 0x%x, alloc 1\n",
			__FUNC__, port, port->dipc_uid, ret);
	}

	/*
	 *  While we were getting the transits, the receive right could have
	 *  migrated here.
	 */
	if (DIPC_IS_PROXY(port)) {
		/*
		 *  Still a proxy, common case.
		 */
		port->dipc_transit += transit;
	} else {
		/*
		 *  Now we're the principal.  Undo effects of getting
		 *  the transits (that we don't need any more).
		 */
		port->dipc_transit -= transit;
		transit_log5(0,
			"%s: port %x uid %x became principal - trans %d\n",
			__FUNC__, port, port->dipc_uid, port->dipc_transit);
	}
}

/*
 *  Name:	dipc_give_transits
 *
 *  Input:	Pointer to port
 *
 *  Output:	Transit count is adjusted by the amount requested.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	Called with the port locked.
 *
 *  Description:	This is the server side routine for requesting
 *			more transits.  The caller (DIPC server) has
 *			validated that this is not a forwarding proxy.
 *			Grant the number of transits requested.
 */
void
dipc_give_transits(port, count)
	ipc_port_t	port;
	int		count;
{
	transit_entry2(dipc_give_transits, port, count);

	assert( ! DIPC_IS_PROXY(port) );
	port->dipc_transit += count;
	port->ip_srights   += count;
}

/*
 *  Name:	dipc_reclaim_transits
 *
 *  Input:	Pointer to principal port and number of transits to reclaim.
 *
 *  Output:	The transit count of the principal is decremented by the
 *		specified amount.  If no-more-senders notification may kick in.
 *
 *  Returns:		void
 *
 *  MP and locking
 *  consideration:	Called with the port locked.
 *
 *  Description:	This is the server side (principal) routine where
 *			proxy transits are returned.  The caller (DIPC server)
 *			has validated that this is not a forwarding proxy.
 *			This also doubles as an indication that there are
 *			no-more-senders on the proxy so if this is the last
 *			of the senders to be checking in, generate an internal
 *			NMS notification if needed.
 */
void
dipc_reclaim_transits(port, count)
	ipc_port_t	port;
	int		count;
{
	transit_entry2(dipc_reclaim_transits, port, count);

	assert( ! DIPC_IS_PROXY(port) );

	/* overkill for now, could be just the above assert when we believe the
	 * system REALLY works!!.
	 */
	if ( DIPC_IS_PROXY(port) )
		panic("dipc_reclaim_transits port %x is Proxy.",port);

	port->dipc_transit -= count;

	port->ip_srights -= count;
	assert(port->ip_srights >= 0);

	DIPC_PORT_LOG("after transit reclaim",port);
	assert(port->dipc_transit >= 0);

	/*
	 *  Provide no-more-senders notification if appropriate.
	 */
	if (port->ip_srights == 0) {
		ipc_port_t nsrequest = port->ip_nsrequest;

		if (nsrequest != IP_NULL) {
			port->ip_nsrequest = IP_NULL;
			transit_log4(2,
				"%s: notifying on port %x nsrequest port %x\n",
				__FUNC__,
				port,
				nsrequest);
			ipc_notify_no_senders(nsrequest, port->ip_mscount);
		}

		/*
		 *  If the receive right is dead too, destroy
		 *  the principal.
		 */
		if (!ip_active(port)) {
			dipc_port_remove_try(port);
		}
	}
}


/*
 *  Name:	dipc_export_port
 *
 *  Input:	Pointer to the port being exported.
 *
 *  Output:	Allocates a uid and (conditionally) attaches a
 *		remote blocked sender's bitvector to the port.
 *
 *  Returns:		DIPC_SUCCESS on success.
 *
 *  MP and locking
 *  consideration:	Port must be locked on entry.
 *
 *  Description:	Called whenever a port is being exported.
 *
 */
boolean_t	dipc_export_with_blocked_sender_init = TRUE;

dipc_return_t
dipc_export_port(port)
	ipc_port_t	port;
{
	extern void	dipc_blocked_sender_init();

	transit_entry1(dipc_export_port, port);

	if (dipc_uid_allocate(port) != DIPC_SUCCESS)
		panic("dipc_export_port: uid allocation error");

	if (dipc_export_with_blocked_sender_init == TRUE)
		dipc_blocked_sender_init(port);

	return DIPC_SUCCESS;
}


/*
 *  Name:	dipc_send_sright
 *
 *  Input:	Pointer to the port we are giving the send right for.
 *
 *  Output:	XXX Describe outputs
 *
 *  Returns:		The UID for the port passed in.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description:	Called whenever a send right is sent to another node.
 *			The table below specifies how many transits accompany
 *			the send right.
 *
 *                                        | Principal |  Proxy  |
 *                      ------------------+-----------+---------+
 *                      destination port  |      x    |    1    |
 *                      ------------------+-----------+---------+
 *                      reply port        |      1    |    1    |
 *                      ------------------+-----------+---------+
 *                      message body port |      2    |    1    |
 *                      ------------------+-----------+---------+
 */
dipc_uid_t
dipc_send_sright(port, port_in_body)
	ipc_port_t	port;
	boolean_t	port_in_body;
{
	dipc_uid_t	uid;

	transit_entry2(dipc_send_sright, port, port_in_body);

	/*
	 * Port must be active.
	 */
	if (!ip_active(port))
		return (DIPC_UID_DEAD);
Retry:
	
	if (DIPC_IS_PROXY(port)) {
		/*
		 *  Only one send right is distributed from a proxy regardless
		 *  of where the port is within the message.  If granting
		 *  the single transit would leaves the local proxy with
		 *  none, we must request more transits from the principal
		 *  port before returning.
		 */
		ip_lock(port);
		assert(port->ip_srights   > 0);
		assert(port->dipc_transit > 0);

		/*
		 *  Make sure that that we have transits to cover any
		 *  remaining send rights after this one is consumed.
		 */
		if ((port->ip_srights > 1) && (port->dipc_transit == 1)) {
			/*
			 *  Have to get more stransits.
			 *  While we're doing this, the
			 *  port could migrate here and
			 *  no longer be a proxy:  START OVER
			 */
			ip_unlock(port);
			dipc_get_transits(port);
			goto Retry;
		}

		/*
		 * Get the uid and add a transit to it.
		 */
		uid = INSERT_TRANSITS(port->dipc_uid, 1);
		port->dipc_transit--;
		assert(port->dipc_transit >= 0);

		if (port->ip_srights == 1) {
			/*
			 *  Release send right.  If needed, this will
			 *  remove the proxy.
			 */
			ipc_port_release_send(port);
		} else {
			port->ip_srights--;
			ip_release(port);
		}

		ip_unlock(port);

	} else {	/* Principle port */
		/*
		 *  If this port has never been exported assign a UID.
		 */
		if (! DIPC_UID_VALID(port))
			if (dipc_export_port(port) != DIPC_SUCCESS)
				panic("dipc_send_sright: exportation error");
		assert(! DIPC_IS_PROXY(port));
		/*
		 *  The principal can always immediately increase transits.
		 *  He does not release send right here, but rather when
		 *  transit comes back via no-local-senders notification.
		 *  This allows the principal's send right count to be an
		 *  upper bound on the true number of send rights in the
		 *  system, which among other things keeps the a lack of
		 *  senders on the principal's node from triggering a
		 *  premature no-senders notification.
		 */
		assert(port->dipc_transit >= 0);

		ip_lock(port);

		if (port_in_body) {
			/*
			 *	Ordinarily, you'd see:
			 *
			 *		port->dipc_transit += 2;
			 *		port->ip_srights += 2;
			 *		<stuff>
			 *		// Consume send right taken
			 *		// during copyin.
			 *		port->ip_srights--;
			 *
			 *	so we'll just +1 the sright instead.
			 */
			port->dipc_transit += 2;
			port->ip_srights += 1;
			uid = INSERT_TRANSITS(port->dipc_uid, 2);

			assert(port->ip_srights > 0);

			/*
			 *  Consume reference taken at copyin time.
			 */
			ip_release(port);
		} else {
			/*
			 *  This is a reply port in the message header.
			 */
			port->dipc_transit += 1;
			uid = INSERT_TRANSITS(port->dipc_uid, 1);
		}
		ip_unlock(port);
	}
	assert(uid != 0);
	return (uid);
}

/*
 *  Name:	dipc_send_soright
 *
 *  Input:	Pointer to the port we are giving the send right for.
 *
 *  Output:	XXX Describe outputs
 *
 *  Returns:		The UID for the port passed in.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description: 	Called whenever a send-once right is sent to
 *			another node.
 */
dipc_uid_t
dipc_send_soright( port, port_in_body )
	ipc_port_t	port;
	boolean_t	port_in_body;
{
	dipc_uid_t	uid;

	transit_entry2(dipc_send_soright, port, port_in_body);

	/*
	 * Port must be active.
	 */
	if (!ip_active(port))
		return (DIPC_UID_DEAD);

	ip_lock(port);

	if (DIPC_IS_PROXY(port)) {
		/*
		 * Save uid
		 */
		uid = port->dipc_uid;

		/*
		 * A proxy releases the send-once right.
		 * This case occurs when moving a send-once right
		 * to another node.
		 */
		assert( port->ip_sorights > 0 );

		dipc_port_log(__FUNC__,0,port);

		ipc_port_release_sonce(port);
	} else {
		/*
		 * The principal does not release send-once right here, but
		 * rather when the send-once right is used as a destination
		 * (of either a reply or a send-once notification).
		 * This allows the principal's send-once right count to
		 * be an accurate count of the true number of send-once
		 * rights in the system.
		 */
		port->dipc_remote_sorights++;

		/*
		 * If this port has never been exported, assign it a uid
		 * and place it on the norma port list.
		 */
		if (! DIPC_UID_VALID(port))
			if (dipc_export_port(port) != DIPC_SUCCESS)
				panic("dipc_send_soright: exportation error");

		uid = port->dipc_uid;

		transit_log6(3, "%s: principal %x uid %x refs %d sorights %d\n",
				__FUNC__,
				port,
				uid,
				port->ip_references,
				port->ip_sorights);

		assert(! DIPC_IS_PROXY(port));
		ip_unlock(port);
	}

	assert(DIPC_UID_VALID(port));
	return (uid);
}

/*
 *  Name:	dipc_send_rright
 *
 *  Input:	Pointer to the port we are sending the receive right to.
 *
 *  Output:	XXX Describe outputs
 *
 *  Returns:		The UID for the port passed in.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description:	Called whenever a receive right is sent to another
 *			node.
 */
dipc_uid_t
dipc_send_rright( port)
	ipc_port_t	port;
{
	dipc_uid_t	uid;

	transit_entry1(dipc_send_rright, port);

	assert(! DIPC_IS_PROXY(port) );

	/*
	 *  If this port has never been exported (for either srights or
	 *  sorights), assign it a uid and place it on the UID table.
	 */
	if (! DIPC_UID_VALID(port)) {
		if (dipc_export_port(port) != DIPC_SUCCESS)
			panic("dipc_send_rright: exportation error");
		/* uid holds a port reference */
	}

	uid = port->dipc_uid;

	/*
	 *  For now, we continue to accept messages here.
	 *  We turn into a proxy only when we have a dest_node to give
	 *  senders, which will be sent by the receiver of this receive
	 *  right.  (We may not yet know where dest_node is, since the
	 *  message carrying this receive right may be indirected...)
	 */
	return uid;
}

/*
 *  Name:	dipc_send_port
 *
 *  Input:	Pointer to the port, type of capability we are sending, and
 *		where in the message this right is comming from.
 *
 *  Output:	XXX Describe outputs
 *
 *  Returns:		Returns the UID associated with the specified port.
 *
 *  MP and locking
 *  consideration:	port is unlocked at this juncture.
 *
 *  Description:	XXX Descrive function.
 *
 */
dipc_uid_t
dipc_send_port(port, type_name, port_in_body)
	ipc_port_t		port;
	mach_msg_type_name_t	type_name;
	boolean_t		port_in_body;
{
	transit_entry3(dipc_send_port, port, type_name, port_in_body);

	if (port == IP_NULL)
		return DIPC_UID_NULL;

	if (port == IP_DEAD)
		return DIPC_UID_DEAD;

	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		return (dipc_send_soright(port, port_in_body));
	}
	else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return (dipc_send_sright(port, port_in_body));
	}
	else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		return (dipc_send_rright(port));
	}
	else {
		panic("dipc_send_port: bad type %d\n", type_name);
	}

	/*NOTREACHED*/
	return (DIPC_UID_NULL);
}

/*
 *  Name:	dipc_unsend_sright
 *
 *  Input:	Pointer to the port we are undoing the send right for and
 *		it's associated UID in the message.
 *
 *  Output:	Transists, port references, and send rights are altered
 *		to undo the effects of a previous dipc_send_sright().
 *
 *  Returns:		The port associated with the UID.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description:	Try to undo the actions taken in dipc_send_sright().
 */
void
dipc_unsend_sright(uid, port)
	dipc_uid_t	uid;
	ipc_port_t	port;
{
	int		transits;

	transit_entry2(dipc_unsend_sright, uid, port);

	transits = EXTRACT_TRANSITS(uid);
	ip_lock(port);

	if (DIPC_IS_PROXY(port)) {
		transit_log4(1,
			"%s: undoing send right for proxy port %x UID %x\n",
			__FUNC__,
			port,
			uid);

		port->dipc_transit += transits;
	} else {
		transit_log4(1,
			"%s: undoing send right for principal port %x UID %x\n",
			__FUNC__,
			port,
			uid);

		port->dipc_transit -= transits;
		port->ip_srights   -= (transits - 1);

		if (transits > 1)
			ip_reference(port);
	}
	ip_unlock(port);
}

/*
 *  Name:	dipc_unsend_soright
 *
 *  Input:	Pointer to the port we are undoing the send once right for.
 *
 *  Output:	Port references and send rights are altered to undo the
 *		effects of a previous dipc_send_soright().
 *
 *  Returns:		The port associated with the UID.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description:	Undo the actions taken in dipc_send_soright().
 */
void
dipc_unsend_soright(port, port_in_body)
	ipc_port_t	port;
	boolean_t	port_in_body;
{
	transit_entry2(dipc_unsend_soright, port, port_in_body);

	ip_lock(port);

	if (DIPC_IS_PROXY(port)) {
		port->ip_sorights++;
		ip_reference(port);

		transit_log4(1, "%s: undoing soright for proxy %x uid %x\n",
				__FUNC__,
				port,
				port->dipc_uid);
	} else {
		port->ip_sorights--;
		ip_release(port);
		if (port_in_body)
			port->dipc_remote_sorights--;

		transit_log4(1, "%s: undoing soright for principal %x uid %x\n",
				__FUNC__,
				port,
				port->dipc_uid);
	}

	ip_unlock(port);
}

/*
 *  Name:	dipc_unsend_rright
 *
 *  Input:	Pointer to the port we are undoing the receive right for.
 *
 *  Output:	Port references and send rights are altered to undo the
 *		effects of a previous dipc_send_rright().
 *
 *  Returns:		The port associated with the UID.
 *
 *  MP and locking
 *  consideration:	Port must be unlocked on entry.
 *
 *  Description:	Undo the actions taken in dipc_send_rright().
 */
void
dipc_unsend_rright(port)
	ipc_port_t	port;
{
	transit_entry1(dipc_unsend_rright, port);
	transit_log4(1, "%s: undoing rright for port %x uid %x\n",
			__FUNC__,
			port,
			port->dipc_uid);
}

/*
 *  Name:	dipc_unsend_port
 *
 *  Input:	UID to the port right we are giving back.
 *
 *  Output:	Transits, rights, and references are altered to undo the
 *		effects of a previous dipc_send_port();
 *
 *  Returns:		The port associated with the UID.
 *
 *  MP and locking
 *  consideration:	port is unlocked at this juncture.
 *
 *  Description:	This function will be called when a remote enqueue
 *			attempt failed.  By that time, we would have already
 *			done the conversion from local to net kmsg.   So,
 *			this routine must undo the effects of the previous
 *			call to dipc_send_port().
 *
 *			It is possible that the call to dipc_send_port()
 *			caused a proxy port to be destroyed.  In this case,
 *			the port pointer returned will be IP_NULL.
 */
ipc_port_t
dipc_unsend_port(uid, type_name, port_in_body)
	dipc_uid_t		uid;
	mach_msg_type_name_t	type_name;
	boolean_t		port_in_body;
{
	ipc_port_t	port;

	transit_entry3(dipc_unsend_port, uid, type_name, port_in_body);

	if ((port = dipc_port_lookup(uid)) == IP_NULL) {
		transit_log3(0, "%s: no port for UID %x\n", __FUNC__, uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		dipc_unsend_soright(port, port_in_body);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		dipc_unsend_sright(uid, port);
	} else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		dipc_unsend_rright(port);
	} else {
		panic("dipc_unsend_port: bad type %d\n", type_name);
	}
	return (port);
}
