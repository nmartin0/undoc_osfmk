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
 * Manuipulate dipc ports.
 *
 * $Id: dipc_port.c,v 1.8 1994/11/18 20:58:03 mtm Exp $
 *
 * HISTORY:
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

#include <ipc/ipc_port.h>
#include <ipc/ipc_space.h>
#include <ipc/ipc_pset.h>

#include <kern/ipc_kobject.h>

#include <norma2/norma_log.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_port.h>
#include <norma2/dipc_special.h>
#include <norma2/dipc_migrate.h>

typedef struct {
	unsigned long	proxy_creates;
	unsigned long	proxy_destroys;
	unsigned long	principal_destroys;
	unsigned long	forwarder_kept;
} dipc_port_stats_t;

dipc_port_stats_t	dipc_port_stats;
#define	PORT_STATS(a)	dipc_port_stats.a;

#ifdef	port_wellness_check
#undef	port_wellness_check
#define port_wellness_check(p)  \
assert( ( (p)->ip_references - (p)->ip_srights - \
        (p)->ip_sorights - (p)->ip_msgcount - \
        (DIPC_UID_VALID(p) ? 1 : 0)) >= 0 )
#endif	/* port_wellness_check */

#if	MACH_ASSERT

ipc_port_t trace_port=IP_NULL;
dipc_uid_t trace_uid;

#define	DIPC_TRACE_PORT(port)						\
	if ( port == trace_port || port->dipc_uid == trace_uid )	\
		DIPC_PORT_LOG(0, port);

#else	/* MACH_ASSERT */

#define DIPC_TRACE_PORT(port)

#endif	/* MACH_ASSERT */

/*
 * Create and initialize a DIPC proxy (sounds like a dumb blonde..)
 *
 * inputs:
 *	uid	uid to install in this new proxy
 *
 * outputs:
 *	IP_NULL == failure, otherwise an ipc_port_t/dipc_proxy_t pointer.
 *
 * side effects:
 *	all proxy ports exist in the kernel 'ipc_space_remote'.
 *	Node # from the 'uid' is set in the port structure as 'dipc_node'.
 */

ipc_port_t
dipc_proxy_create( dipc_uid_t uid )
{
	ipc_port_t	proxy;

	port_entry1(dipc_proxy_create, uid);

	/*
	 * proxy ports comes from the special kernel 'ipc_space_remote' space.
	 */
	if ( (proxy = ipc_port_alloc_special(ipc_space_remote)) == IP_NULL) {
		panic("dipc_proxy_create: proxy port alloc failed\n");
                return IP_NULL;
	}

	PORT_STATS(proxy_creates++);

	ip_lock( proxy );

	proxy->dipc_is_proxy = TRUE;
	proxy->dipc_transit = 0;
	proxy->ip_srights = 0;
	proxy->ip_sorights = 0;


	/*
	 * install the uid and node # (from the 'uid') in the proxy
	 */
	if ( dipc_uid_install( proxy, uid ) != DIPC_SUCCESS ) {
		ipc_port_t	installed_proxy;

		ip_unlock(proxy);

		/*
		 *  See if someone created this proxy while we were blocked
		 *  allocating memory.
		 */
		if((installed_proxy = dipc_port_lookup(uid)) != IP_NULL) {
			port_log4(0,
				"%s: proxy port %x uid %x create collision\n",
				__FUNC__,
				installed_proxy,
				uid);

			dipc_proxy_destroy(proxy);
		}
		return installed_proxy;
	}

	/*
	 * proxy ports get no-more senders notification so they will
	 * be removed in a timely fashion.  Add a SO-right plus a
	 * reference to cover the nm-senders notification send.
	 */
	proxy->ip_nsrequest = proxy; 
	proxy->ip_sorights++;
	ip_reference(proxy);

	ip_unlock( proxy );

	/*
	 * The port now holds three references:
	 *   a reference was generated during the creation of this proxy in
	 *   ipc_port_special_alloc(), installing the 'uid' generates a
	 *   reference, and the NMS hook take a reference.
	 */
	port_log5(3, "%s: proxy %x uid %x refs %d\n",
			__FUNC__,
			proxy,
			CLEAN_UID(uid),
			proxy->ip_references);

	return	proxy;
}

/*
 * destroy/free the proxy port.
 *
 * implicit inputs:
 *	port is not locked
 *	port has one reference
 *
 * inputs:
 *	proxy	proxy port pointer (ipc_port_t)
 *
 * outputs:
 *	none.
 *
 * side effects:
 *
 */

void
dipc_proxy_destroy( ipc_port_t proxy )
{
	port_entry1(dipc_proxy_destroy, proxy);

	DIPC_TRACE_PORT(proxy);

	/*
	 *  If the NMS hook hasn't been clean up yet, do it now.
	 */
	if (proxy->ip_nsrequest != IP_NULL) {
		proxy->ip_nsrequest = IP_NULL; 
		proxy->ip_sorights--;
		ip_release(proxy);
	}

	ipc_port_dealloc_special(proxy, ipc_space_remote);
}


/*
 * destroy a dipc (Distributed IPC) port
 *
 * inputs:
 *	port	dipc port pointer: proxy or principal
 *
 * outputs:
 *	none.
 *
 * side effects:
 *
 */

void
dipc_port_remove( ipc_port_t port )
{
	port_entry1(dipc_port_remove, port);

	assert(DIPC_UID_VALID(port));

	/*
	 *  Don't remove special ports.
	 */
	if (port->dipc_is_special)
		return;

	/*
	 *  Don't remove ports which are currently migrating.
	 */
	if ( port->dipc_migrate_state ) {
		port_log4(0,
		  "%s: port %x uid %x in migration, not removed.\n",
			__FUNC__,
			port,
			port->dipc_uid);
		return;
	}

	if ( DIPC_IS_PROXY(port) ) {
		/*
		 * Don't remove forwarding proxies, they are forwarding
		 * pointers to migrated receive rights.  A forwarding proxy
		 * is removed separately by a dead-name notfication.
		 */
		if ( port->dipc_forward ) {
			port_log4(0,
			"%s: uid %x: forwarding proxy (node %d) not removed.\n",
				__FUNC__, port->dipc_uid,port->dipc_node);
			return;
		}

		/*
		 * say goodnight....
		 */
		dipc_proxy_destroy(port);
	}
	else {
		/* dipc principal */
		DIPC_TRACE_PORT(port);
		ip_reference(port);	/* consumed by ipc_port_destroy() */
		ipc_port_destroy(port);
	}
}

/*
 * attempt to destroy a DIPC port.
 *
 * inputs:
 *	port	ipc_port_t port pointer
 *
 * outputs:
 *
 * side effects:
 *
 */

void
dipc_port_remove_try( ipc_port_t port )
{
	port_entry1(dipc_port_remove_try, port);

	/*
	 *  The DIPC state could have already been destroyed through dead
	 *  name notification.  If the UID isnt' valid, just return.
	 */
	if ( ! DIPC_UID_VALID(port) )  {
		port_log3(0, "%s: UID for port %x is not valid\n",
							__FUNC__, port);
		return;
	}

	DIPC_TRACE_PORT(port);

	/*
	 *	This can be called on both principals and proxies.
	 *
	 *	For principals:
	 *		we want to know that there are no more rights in
	 *		the dipc system (transits are zero) and all send
	 *		and send-once are consumed.
	 *
	 *	For proxies:
	 *		we want to know that the only refernces are the ones
	 *		we hold for the proxy, UID, and possible NMS hook.
	 */
	if (DIPC_IS_PROXY(port)) {
		if ((port->ip_references == 2) ||
		    (port->ip_references == 3  &&
		    (port->ip_nsrequest == port))) {
			DIPC_PORT_LOG("releasing proxy", port);
			dipc_port_remove(port);
		} else {
			DIPC_PORT_LOG("keeping proxy", port);
		}
	}
	/*
	 *  Principal
	 */
	else {
		if ((port->ip_srights   == 0) &&
		    (port->dipc_transit == 0) &&
		    (port->ip_sorights  == 0)) {
			DIPC_PORT_LOG("releasing principal", port);
			dipc_port_remove(port);
		} else {
			DIPC_PORT_LOG("keeping principal", port);
		}
	}
}

/*
 *  Name:	dipc_port_destroy_state
 *
 *  Input:	port pointer
 *
 *  Side Effects:
 *		Unused transits are returned.  If this is not a forwarding
 *		proxy, destroy the dipc port state but not the port structure,
 *		our caller will handle the port structure.
 *
 *  Returns:	TRUE	continue with port destruction
 *		FALSE	forwarding proxy, retain the port.
 *
 *  MP and locking
 *  consideration:	XXX Describe locking and MP considerations.
 *
 *  Description:	Called from ipc_port_destroy.  This routine will
 *			return excess transits and free the resources
 *			associated with the port but will not remove the port
 *			structure as our caller will.  If this is a forwarding
 *			proxy, the port will remain until the principal
 *			has been destroyed.
 */
boolean_t
dipc_port_destroy_state(port)
	ipc_port_t port;
{
	dipc_uid_t	uid;

	port_entry1(dipc_port_destroy_state, port);

	/*
	 *  We must remove this port from the UID table before we making any
	 *  calls that may block.  If we don't, there is the possibility that
	 *  a message could be received containg a new right for the same
	 *  remote port that is being destroyed here.  In that case, they need
	 *  to create a new proxy.
	 */
	dipc_port_log(__FUNC__,0,port);

	/*
	 *  Remember the UID being destroyed for RPC's made below.
	 *  Do not destory the UID if this is a forwarding proxy.
	 */
	uid = port->dipc_uid;
	if ( ! port->dipc_forward ) {
		if (dipc_uid_destroy( port ) != DIPC_SUCCESS ) {
			printf("Port %x uid %x not found\n", port,
				port->dipc_uid);
			panic("dipc_port_destroy_state");
		}
	}

	/*
	 *  Handle proxy state.
	 */
	if (DIPC_IS_PROXY(port)) {
		/*
		 *  Return unused transits.
		 */
		if ((port->dipc_transit > 0) && port->dipc_do_nms) {
			(void) dipc_sell_transits( port->dipc_node,
						   uid,
						   &port->dipc_transit);
		}

		/*
		 *  Explicitly clear the transit field.
		 */
		ip_lock(port);
		port->dipc_transit = 0;
		ip_unlock(port);

		/*
		 *  Don't remove proxies that are forwarding pointers to
		 *  receive rights.  They get cleaned up separately.
		 */
		if ( port->dipc_forward ) {
			port_log4(0,
				"%s: forwarding proxy %x uid %x ! removed\n",
					__FUNC__, port, port->dipc_uid);
			PORT_STATS(forwarder_kept++);
			return FALSE;
		}

		PORT_STATS(proxy_destroys++);
	} else {
		/*
		 *  Take care of any remote send-once rights that won't
		 *  be able to find this port now that the UID has been
		 *  removed from the table.
		 */
		assert(port->ip_sorights >= port->dipc_remote_sorights);
		port->ip_sorights -= port->dipc_remote_sorights;

		assert(port->ip_references >= port->dipc_remote_sorights);
		port->ip_references -= port->dipc_remote_sorights;

		/*
		 *  Wakeup any remove blocked senders so they can find out
		 *  this port is being destroyed.
		 */
		if ( port->dipc_blocked_sender_count )
			dipc_awaken_all_remote_senders(port, uid);

		/*
		 *  Free resorces associated with blocked senders.
		 */
		dipc_blocked_sender_destroy( port );

		PORT_STATS(principal_destroys++);
	}

	return TRUE;
}

/*
 * Receive the destination port specified uid. 'kmsg' is in network format.
 *
 * input:
 *	uid		remote port uid.
 *	type_name	mach message header type name
 *	remote_port	address of 'kmsg->ikm_header.msgh_remote_port'
 *
 * outputs:
 *
 *	KERN_SUCCESS		success!
 *	KERN_INVALID_NAME	invalid port name
 *	KERN_NOT_RECEIVER	port has migrated elsewhere ().
 *	KERN_INVALID_RIGHT	port has died, sender must cleanup.
 *
 * side effects:
 *
 *
 */

kern_return_t
dipc_receive_dest( ipc_kmsg_t kmsg )
{
	kern_return_t		kr;
	dipc_uid_t		uid;
	ipc_port_t		port;
	mach_port_t		*dest_portp;
	mach_msg_header_t	*msgh;
	mach_msg_type_name_t	type_name;

	port_entry1(dipc_receive_dest, kmsg);

	assert( kmsg != IKM_NULL );

	dest_portp = (mach_port_t *) &kmsg->ikm_header.msgh_remote_port;
	msgh = (mach_msg_header_t *) &kmsg->ikm_header;
	type_name = MACH_MSGH_BITS_REMOTE(msgh->msgh_bits);

	/*
	 * This can only be some flavor of send or send-once right.
	 */
	assert( (type_name == MACH_MSG_TYPE_PORT_SEND) ||
	        (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) );

	uid = (dipc_uid_t) *dest_portp;

	/*
	 * is this a migrating port message?
	 */
	if (msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) {
		port_log3(1,"%s: migrated message uid %x\n", __FUNC__, uid);

		msgh->msgh_bits &= ~MACH_MSGH_BITS_MIGRATED;
		kr = dipc_receive_migrating_message(uid, type_name, dest_portp);

		return kr;
	}

	/*
	 * Handle the special port case.
	 */
	if ( DIPC_SPECIAL_UID(uid) ) {
		dipc_node_t	node = NODE_FROM_UID(uid);
		dipc_uid_seq_t	seq = SEQID_FROM_UID(uid);

		if ( node == node_self()) {
			*dest_portp = (mach_port_t)
					dipc_local_special_port(seq, type_name);
		} else {
			*dest_portp = (mach_port_t) dipc_remote_special_port(
								node,
								seq,
								type_name);
		}
		return KERN_SUCCESS;
	}

	/*
	 * Find associated port.
	 */
	if ( (port = dipc_port_lookup(uid)) == IP_NULL) {
		/*
		 * Must be an invalid uid; otherwise, we would have
		 * found something with dipc_port_lookup.
		 */
		port_log3(0, "%s: invalid uid %x!\n", __FUNC__, uid);

		*dest_portp = MACH_PORT_NULL;

		return KERN_INVALID_NAME;
	}

	/*
	 * Check to see whether we are the correct receiver for this message.
	 * If not, try to say where the correct receiver would be.
	 */
	if ( DIPC_IS_PROXY(port) ) {
		/*
		 * Tell sender to use new node.
		 */
		port_log4(0, "%s: dest migrated %d -> %d\n",
				__FUNC__,
				node_self(),
				port->dipc_node);

		printf("dipc_receive_dest principal has migrated to node %d\n",
				port->dipc_node);

		/*
		 * remove enqueue reference; port might go away.
		 */
		ip_release(port);
		dipc_port_remove_try(port);

		*dest_portp = MACH_PORT_NULL;

		return KERN_NOT_RECEIVER;
	}

	/*
	 * We have a local principal. Make sure it is active.
	 */
	if (! ip_active(port)) {
		/*
		 * If it is not active, it is a dead port, kept alive
		 * by remote send and send-once references.
		 */
		port_log4(0, "%s: dead principal 0x%x, refs %d\n",
				__FUNC__,
				uid,
				port->ip_references);
		/*
		 * remove enqueue reference; port might go away.
		 */
		ipc_port_release(port);

		/*
		 *	Sender is responsible for all cleanup.
		 */
		*dest_portp = MACH_PORT_NULL;

		return KERN_INVALID_RIGHT;
	}
	
#if	MACH_ASSERT
	/*
	 * Check queue limit.
	 */
	port_queue_length_check(port);

	if (port->ip_msgcount > port->ip_qlimit) {
		/*
		 * MACH_SEND_ALWAYS may exceed the queue limit
		 */
		if ( port->ip_msgcount >= (port->ip_qlimit * 2) )
			port_log6(1, "%s: port %x uid %x msgs %d qlimit %d\n",
					__FUNC__,
					port,
					uid,
					port->ip_msgcount,
					port->ip_qlimit);
	}
#endif	/* MACH_ASSERT */
	
	/*
	 * Return the port pointer. Simulate copyin:
	 *	Add a send{-once} right, notice no port reference is taken
	 *	here as it was previously taken during the enqueue operation.
	 */
	ip_lock( port );
	if (type_name == MACH_MSG_TYPE_PORT_SEND) {
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
		assert(type_name == MACH_MSG_TYPE_PORT_SEND_ONCE);
		ip_release(port);
		port->dipc_remote_sorights--;
	}
	ip_unlock( port );

	DIPC_PORT_LOG("exit",port);

	/*
	 * set valid IPC port pointer
	 */
	*dest_portp = (mach_port_t)port;

	return KERN_SUCCESS;
}

/*
 * Receive a send-once right.  Find or create a proxy for the given uid, add
 * a send-once right reference.
 *
 * implicit inputs:
 *	called when an send-once right is detected in the message body
 *
 * inputs:
 *		uid	dipc uid
 *
 * outputs:
 *		port (principal/proxy) pointer (ipc_port_t) for given uid.
 *
 */

ipc_port_t
dipc_receive_soright( dipc_uid_t uid )
{
	ipc_port_t	port;
	dipc_node_t	node = NODE_FROM_UID(uid);

	port_entry1(dipc_receive_soright, uid);

	/*
	 * Handle the special port case.
	 */
	if (DIPC_SPECIAL_UID(uid)) {
		dipc_uid_seq_t	seq = SEQID_FROM_UID(uid);

		if ( node == node_self() )
			port = dipc_local_special_port( seq,
						MACH_MSG_TYPE_PORT_SEND_ONCE );
	        else
			port = dipc_remote_special_port( node, seq,
						MACH_MSG_TYPE_PORT_SEND_ONCE );
		return port;
	}

	/*
	 * Try to find the port. If we don't have a port, then we must
	 * create a proxy.
	 */
	if ( (port = dipc_port_lookup(uid)) == IP_NULL) {

		assert( node != node_self());

		if ( (port = dipc_proxy_create( uid )) == IP_NULL) {
			panic("receive_soright: dipc_proxy_create");
		}
	}

	ip_lock( port );

	/*
	 * Is it a proxy?
	 */
	if ( DIPC_IS_PROXY(port) ) {
		/*
		 * Adjust sorights.
		 * We increment, because we are inheriting a remote count.
		 */
		assert(port->ip_nsrequest != IP_NULL);
		assert(port == port->ip_nsrequest);
		assert(ip_active(port)); /* XXX How could it be otherwise? */

		/*
		 * Transfer the send-once right ref from the message header to
		 * the port proper.  The reference will be consumed by an
		 * ensuing send-once send.
		 */
		port->ip_sorights++;
		ip_reference(port);

		ip_unlock( port );

		DIPC_PORT_LOG("proxy", port);

		return port;
	}
	else {
		port->dipc_remote_sorights--;
	}

	/*
	 * port is a principal, alive or dead?
	 */
	if ( ! ip_active(port) ) {
		/*
		 * Dead principal:
		 *	 decrement sorights because they have come home.
		 */
		port->ip_sorights--;
		ip_release(port);

		ip_unlock( port );

		assert((long) port->ip_sorights >= 0);

		DIPC_PORT_LOG("dead_principal", port);

		dipc_port_remove_try(port);

		return IP_DEAD;
	}

	/*
	 * A living principal:
	 *	We don't do anything with the send-once right.  We gained
	 *	the soright and reference in ipc_right_copyin() and we did
	 *	not consume it during dipc_send_soright().  When the right
	 *	is copied out to the user, the books will balance.
	 */
	assert((long) port->ip_sorights > 0);

	ip_unlock( port );

	return port;
}

/*
 * Receive a send right.  Find or create proxy for the given uid. Add a send
 * right reference.
 *
 * inputs:
 *	uid	uid from kmsg
 *
 * outputs:
 *	port pointer:
 *		IP_NULL on error, otherwise a valid port pointer.
 *
 */

ipc_port_t
dipc_receive_sright( dipc_uid_t uid )
{
	ipc_port_t	port;
	dipc_node_t	node = NODE_FROM_UID(uid);

	port_entry1(dipc_receive_sright, uid);

	/*
	 * Handle the special port case.
	 */
	if (DIPC_SPECIAL_UID(uid)) {
		dipc_uid_seq_t	seq = SEQID_FROM_UID(uid);

		if ( node == node_self() )
			port = dipc_local_special_port( seq,
						MACH_MSG_TYPE_PORT_SEND );
	        else
			port = dipc_remote_special_port( node, seq,
						MACH_MSG_TYPE_PORT_SEND );
		port_wellness_check(port);

		return port;
	}

	/*
	 * Try to find the port. If we don't have a port, then we must
	 * create a proxy.
	 */
	if ( (port = dipc_port_lookup(uid)) == IP_NULL) {

		assert( node != node_self() );
		if ( (port = dipc_proxy_create( uid )) == IP_NULL) {
			panic("receive_sright: dipc_proxy_create");
		}

		DIPC_PORT_LOG("(new proxy)",port);
	}

	ip_lock( port );
ip_reference(port);

	/*
	 * We have an existing port structure; Is it proxy or principal?
	 */
	if ( DIPC_IS_PROXY(port) ) {
/*
 * add a send right and port reference to cover it
 */
port->ip_srights++;

		/*
		 *  If ip_nsrequest == IP_NULL, this port is in the process of
		 *  being destroyed, probably through dipc_no_more_senders().
		 *  Now that we are receiving another send right, reinstall
		 *  the ip_nsrequset field.
		 */
		if (port->ip_nsrequest == IP_NULL) {
			port_log3(0, "%s: reinstall NMS %x\n", __FUNC__, port);
			ip_reference(port);
			port->ip_sorights++;
			port->ip_nsrequest = port;
		}

		assert(ip_active(port)); /* XXX How could it be otherwise? */

		/*
		 * EXTRACT_TRANSITS() knows wheather this is from the
		 * principal(2) or a proxy(1).
		 */
		port->dipc_transit += EXTRACT_TRANSITS(uid);

		ip_unlock( port );

		DIPC_PORT_LOG("(existing proxy)",port);
 
		port_wellness_check(port);

		return port;
	}

	/*
	 * It is a principal, is it dead?
	 */
	if (! ip_active(port)) {
		port_log3(0, "%s: dead port %x\n", __FUNC__, uid);

		/*
		 * Adjust transit count, since sender used up a transit in
		 * sending to us. This may enable us to free the port.
		 * Consume immediately the sright associated with transit.
		 */
		assert(port->dipc_transit > 0);
		assert(port->ip_srights > 0);

		port->dipc_transit--;	/* for the consumed remote sright */
		port->ip_srights--;	/*             "		  */
		ip_release(port);	/* for reference taken above      */

		ip_unlock( port );

		dipc_port_remove_try(port);

		return IP_DEAD;
	}

	/*
	 * 'port' is a living principal:
	 *	Decrement transit count as the previously exported send-right
	 *	(transit++) has now come back.
	 */
	assert(port->ip_srights > 0);
	assert(port->dipc_transit > 0);

	/*
	 * adjust transits for send-rights which have returned home (send
	 * rights are now local).
	 */
	port->dipc_transit -= EXTRACT_TRANSITS(uid);

	DIPC_PORT_LOG("(existing principal)",port);
 
	ip_unlock( port );

	return port;
}


/*
 * Called when a port right of any flavor is received from another node.
 *
 * inputs:
 *	uid		uid from network kmsg
 *	type_name	port rights 'name'
 *
 * outputs:
 *	error: IP_NULL | IP_DEAD
 *	success: a valid local IPC port pointer.
 *
 */
ipc_port_t
dipc_receive_port( dipc_uid_t uid, mach_msg_type_name_t type_name )
{
	port_entry2(dipc_receive_port, uid, type_name);

	/*
	 * Catch null and dead ports.
	 */
	if (uid == DIPC_UID_NULL)
		return IP_NULL;

	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		return dipc_receive_soright( uid );
	}
	else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return dipc_receive_sright( uid );
	}
	else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		return dipc_receive_rright( uid );
	}
	else {
		panic("dipc_receive_port: bad type %d\n", type_name);
		return IP_NULL;
	}
}

#if	MACH_ASSERT

/*
 * log the status of a specific port. 'uid' is output on each line to
 * facilitate NORMA log filtering.
 *
 * inputs:
 *	from	string which identifies the caller
 *	msg	string which would display more interesting info
 *	port	port porinter.
 *
 * outputs:
 *	void
 */

void
dipc_port_log(
		char		*from,
		char		*msg,
		ipc_port_t	port )
{
	char	*empty = "";

	port_entry3(dipc_port_log, from, msg, port);

	port_log5(3,"%s %s port %x uid %x\n",
		from, 
		(msg ? msg : ""),
		port,
		port->dipc_uid
		);

	port_log8(3, "  port %x [%s%s%s%s%s%s]\n",
                        port,
                        (port->dipc_is_proxy    ? "Proxy" : "Principle"),
                        (port->dipc_is_special  ? " special" : empty),
                        (port->dipc_forward     ? " forwarding" : empty),
                        (port->dipc_migrate_state ? " migrating" : empty),
                        (port->dipc_migrated    ? " migrated" : empty),
                        (ip_active(port)	? empty : " Dead")
			);

	port_log6(3, "  port %x node %d msgcount %d qlimit %d mscount %d\n",
				port,
				port->dipc_node,
				port->ip_msgcount,
				port->ip_qlimit,
				port->ip_mscount);

	port_log7(3, "  port %x refs %d send %d trans %d sonce %d rsonce %d\n",
				port,
				port->ip_references,
				port->ip_srights,
				port->dipc_transit,
				port->ip_sorights,
				port->dipc_remote_sorights);
}

int	c_port_queue_length_found = 0;
int	c_port_queue_length_max = 0;
int	port_queue_length_warning = 513;
int	port_queue_length_enabled = 0;

port_queue_length_check(port)
	ipc_port_t	port;
{
	ipc_mqueue_t		mqueue;
	ipc_kmsg_t		kmsg;
	ipc_kmsg_queue_t	kmsgs = &mqueue->imq_messages;
	int			count;

	if (port_queue_length_enabled == 0)
		return;
	count = db_port_kmsg_count(port);
	if (count > c_port_queue_length_max)
		c_port_queue_length_max = count;
	if (count < port_queue_length_warning)
		return;
	++c_port_queue_length_found;
	assert(count < port_queue_length_warning);
}

#endif	/* MACH_ASSERT */

db_dipc_port_stats()
{
	dipc_port_stats_t	*p = &dipc_port_stats;

        db_printf("dipc_port stats:\n");
	db_printf("  proxy_creates          %8d",   p->proxy_creates);
	db_printf("  proxy_destroys         %8d\n", p->proxy_destroys);
	db_printf("  principal_destroys     %8d",   p->principal_destroys);
	db_printf("  forwarder_kept         %8d\n", p->forwarder_kept);

	return 0;
}
