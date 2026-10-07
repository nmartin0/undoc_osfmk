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
 * $Log: ipc_clean.c,v $
 * Revision 1.7  1994/11/18  20:55:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/03/19  01:28:27  lenb
 * Benefit: use positive logic on locking assertions
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.5  1993/07/22  02:20:09  andyp
 * Recovered OSF's logs.  Removed uneeded files that were in the
 * repository for some reason.  Included changes resulting
 * from rwd@osf.org's visit (correctly functioning backoff logic,
 * don't overwrite a pending CTL_ACK, first-cut at cogestion handling).
 * Reconfigured default settings for timeouts and ticks.
 *
 * Revision 1.4  1993/06/30  22:50:07  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:45:25  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:50:38  dleslie
 * First R1_0 release
 *
 * END SSD HISTORY
 */
/*
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * Log: ipc_clean.c,v
 * Revision 1.2  1992/11/25  01:13:26  robert
 * 	fix history
 * 	[1992/11/09  21:35:56  robert]
 *
 * 	integrate changes below for norma_14
 * 	[1992/11/09  16:43:33  robert]
 *
 * Revision 0.0  92/10/19            dwm
 * 	Clear ip_norma_is_zombie in norma_ipc_dead_destination (#408).
 * 
 * 	Revision 1.1  1992/11/05  20:59:20  robert
 * 	Initial revision
 * 	[92/10/19            dwm]
 * 
 * $EndLog$
 */
/* CMU_HIST */
/*
 *
 * Revision 2.1.2.9  92/09/15  17:33:17  jeffreyh
 * 	Remove forwarding proxy logic.  Now handled by dead name logic.
 * 	Add "already dead" case to norma_ipc_dead_destination.
 * 	[92/09/14            dlb]
 * 	Clear ip_norma_forward in norma_ipc_dead_destination().  Add long
 * 	XXX comment about why we can't do this on the root node (yet).
 * 	[92/08/06            dlb]
 * 
 * Revision 2.1.2.8  92/06/24  18:00:29  jeffreyh
 * 	Move test for complex kmsg to caller of ipc_kmsg_uncopyout_to_network.
 * 	[92/06/02            dlb]
 * 
 * 	Proxies now maintain sotransit.
 * 	[92/06/01            dlb]
 * 
 * Revision 2.1.2.7  92/05/28  18:20:40  jeffreyh
 * 	Add logic to unprepare a destination when uncopyout'ing a kmsg.
 * 	Convert a couple of trivial routines to macros.
 * 	[92/05/28            dlb]
 * 
 * Revision 2.1.2.6  92/05/27  00:46:52  jeffreyh
 * 	Clean mscount before destroying port in norma_ipc_dead_destination.
 * 	[92/05/25            dlb]
 * 
 * Revision 2.1.2.5  92/04/08  15:45:21  jeffreyh
 * 	Use norma_ipc_yield_transits when destroying a proxy.
 * 	Get rid of related XXX comments.
 * 	[92/04/07            dlb]
 * 
 * 	Add mousetrap to norma_ipc_error_receiving to catch use of bad
 * 	norma kmsg.
 * 	[92/04/05            dlb]
 * 
 * 	Move IP_NULL logic to norma_ipc_unsend_port from
 * 	norma_kmsg_uncopyout_to_network.  Add logic to pass IP_DEAD.
 * 	[92/04/02            dlb]
 * 
 * 		Don't norma_port_remove a dead proxy; norma_ipc_port_destroy
 * 	can cope now.
 * 	[92/04/01            dlb]
 * 
 * Revision 2.1.2.4  92/03/28  10:11:22  jeffreyh
 * 	When handling a receive error, check whether remote_port should be
 * 	removed from norma port table.
 * 	[92/03/25            dlb]
 * 	Don't unsend null ports in ipc_kmsg_uncopyout_to_network.
 * 	[92/03/20  14:12:27  dlb]
 * 
 * Revision 2.1.2.3  92/02/21  11:24:11  jsb
 * 	Moved ipc_kmsg_uncopyout_to_network here from ipc/ipc_kmsg.c.
 * 	Separated norma_ipc_uncopyout_all_kmsgs out from
 * 	norma_ipc_dead_destination (renamed from norma_ipc_destroy_proxy).
 * 	[92/02/21  10:38:49  jsb]
 * 
 * 	Eliminated norma_ipc_unsend_*_dest, since norma_ipc_send_*_dest
 * 	is no longer called until and unless message is successfully
 * 	received. Added check to ensure that destroyed proxy has queued
 * 	messages. Added call to norma_port_remove before call to
 * 	ipc_port_destroy. Removed zeroing of ip_seqno.
 * 	[92/02/18  08:44:28  jsb]
 * 
 * Revision 2.1.2.2  92/01/21  21:51:00  jsb
 * 	De-linted.
 * 	[92/01/16  21:31:56  jsb]
 * 
 * Revision 2.1.2.1  92/01/03  08:57:08  jsb
 * 	First NORMA branch checkin.
 * 
 * Revision 2.1.1.3  91/12/31  21:36:40  jsb
 * 	Changed ndproxy macros to nsproxy.
 * 
 * Revision 2.1.1.2  91/12/31  12:11:04  jsb
 * 	Changed remote_port handling in norma_ipc_destroy_proxy to account
 * 	for remote consumption of send-once right (if any) in first kmsg.
 * 	Added ipc_kmsg_uncopyout_to_network, which is given remote_port which
 * 	it in turn hands to norma_ipc_unsend{_migrating,}_dest, which now
 * 	use this port instead of trying to look it up based on uid.
 * 	Changes for IP_NORMA_REQUEST macros being renamed to ip_ndproxy{,m,p}.
 * 
 * Revision 2.1.1.1  91/12/29  21:42:51  jsb
 * 	First checkin.
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
 *	File:	norma/ipc_clean.c
 *	Author:	Joseph S. Barrera III
 *	Date:	1991
 *
 *	Routines to clean messages sent to the network.
 */

#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <norma/ipc_node.h>

#include <mach/vm_param.h>

extern ipc_port_t norma_port_lookup();
extern ipc_port_t norma_ipc_receive_soright();
extern ipc_port_t norma_ipc_receive_sright();
extern void norma_ipc_unprepare_dest();

extern ipc_port_t norma_ipc_unsend_port();
extern ipc_port_t norma_ipc_unsend_rright();

#define	norma_ipc_unsend_soright(uid)	norma_ipc_receive_soright(uid)
#define norma_ipc_unsend_sright(uid)	norma_ipc_receive_sright(uid)

ipc_kmsg_t	norma_kmsg_receive_error;

/*
 * These unsend routines should undo the effects of the send routines above.
 */

ipc_port_t
norma_ipc_unsend_port(uid, type_name)
	unsigned long uid;
	mach_msg_type_name_t type_name;
{
	if (uid == IP_NORMA_NULL)
		return IP_NULL;
	if (uid == IP_NORMA_DEAD)
		return IP_DEAD;

	if (type_name == MACH_MSG_TYPE_PORT_SEND_ONCE) {
		return norma_ipc_unsend_soright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_SEND) {
		return norma_ipc_unsend_sright(uid);
	} else if (type_name == MACH_MSG_TYPE_PORT_RECEIVE) {
		return norma_ipc_unsend_rright(uid);
	} else {
		panic("norma_ipc_unsend_port: bad type %d\n", type_name);
		/* NOTREACHED */
	}
}

ipc_port_t
norma_ipc_unsend_rright(uid)
	unsigned long uid;
{
	ipc_port_t port;

	port = norma_port_lookup(uid);
	if (port == IP_NULL) {
		return IP_NULL;
	}
	if (! ip_active(port)) {
		return IP_DEAD;
	}
	assert(! port->ip_norma_is_proxy);
	ip_reference(port);
	return port;
}

void
ipc_kmsg_uncopyout_header_to_network(msgh, dest)
	mach_msg_header_t *msgh;
	ipc_port_t dest;
{
	/*
	 * Uncopy local port
	 */
	if (msgh->msgh_local_port) {
		register mach_msg_type_name_t type;

		type = MACH_MSGH_BITS_LOCAL(msgh->msgh_bits);
		type = ipc_object_copyin_type(type);
		msgh->msgh_local_port = (mach_port_t)
		    norma_ipc_unsend_port((unsigned long)msgh->msgh_local_port,
					  type);
	}

	/*
	 * Uncopy remote port, and undo effects of preparing dest.
	 */
	assert((msgh->msgh_bits & MACH_MSGH_BITS_MIGRATED) == 0);
	assert(msgh->msgh_remote_port == (mach_port_t) dest->ip_norma_uid);
	msgh->msgh_remote_port = (mach_port_t) dest;
	norma_ipc_unprepare_dest(dest, MACH_MSGH_BITS_REMOTE(msgh->msgh_bits));
}

void
ipc_kmsg_uncopyout_to_network(kmsg)
	ipc_kmsg_t kmsg;
{
	vm_offset_t saddr, eaddr;

	kmsg->ikm_header.msgh_bits &= ~ (MACH_MSGH_BITS_COMPLEX_DATA |
					 MACH_MSGH_BITS_COMPLEX_PORTS |
					 MACH_MSGH_BITS_MIGRATED);

	saddr = (vm_offset_t) (&kmsg->ikm_header + 1);
	eaddr = (vm_offset_t) &kmsg->ikm_header + kmsg->ikm_header.msgh_size;

	while (saddr < eaddr) {
		mach_msg_type_long_t *type;
		mach_msg_type_name_t name;
		mach_msg_type_size_t size;
		mach_msg_type_number_t number;
		boolean_t is_inline, longform, is_port;
		vm_offset_t data;
		vm_size_t length;

		type = (mach_msg_type_long_t *) saddr;
		is_inline = type->msgtl_header.msgt_inline;
		longform = type->msgtl_header.msgt_longform;
		/* type->msgtl_header.msgt_deallocate not used */
		if (longform) {
			name = type->msgtl_name;
			size = type->msgtl_size;
			number = type->msgtl_number;
			saddr += sizeof(mach_msg_type_long_t);
		} else {
			name = type->msgtl_header.msgt_name;
			size = type->msgtl_header.msgt_size;
			number = type->msgtl_header.msgt_number;
			saddr += sizeof(mach_msg_type_t);
		}

		/* calculate length of data in bytes, rounding up */

		length = ((number * size) + 7) >> 3;

		is_port = MACH_MSG_TYPE_PORT_ANY(name);

		if (is_inline) {
			/* inline data sizes round up to int boundaries */

			data = saddr;
			saddr += (length + 3) &~ 3;
		} else if (is_port) {
			/*
			 * Not a copy object, just a kernel address.
			 */
			data = * (vm_offset_t *) saddr;
			saddr += sizeof(vm_offset_t);
		} else {
			/*
			 * This is a copy object.
			 * It is okay to leave copy objects in page-list form.
			 * The netipc module is responsible for leaving them
			 * in a sane state.
			 */
			saddr += sizeof(vm_offset_t);
		}

		if (is_port) {
			ipc_port_t *ports = (ipc_port_t *) data;
			mach_msg_type_number_t i;

			/*
			 * Convert uids into ports and undo refcount action.
			 */
			for (i = 0; i < number; i++) {
			    ports[i] = norma_ipc_unsend_port(
			    	(unsigned long) ports[i], name);
		       }
		}
	}
}

/*
 * Convert all kmsgs queued on port from network to internal format.
 */
void
norma_ipc_uncopyout_all_kmsgs(port)
	ipc_port_t port;
{
	register ipc_kmsg_t kmsg;
	register ipc_kmsg_queue_t kmsgs;

	kmsgs = &port->ip_messages.imq_messages;
	kmsg = ipc_kmsg_queue_first(kmsgs);
	if (kmsg != IKM_NULL) do {
		printf1("uncopyout ksmg=0x%x msgh_id=%d; was s=%d so=%d\n",
			kmsg, kmsg->ikm_header.msgh_id,
			port->ip_srights, port->ip_sorights);

		/*
		 * Uncopy header
		 */
		ipc_kmsg_uncopyout_header_to_network(&kmsg->ikm_header, port);

		/*
		 * Uncopy body
		 */
		if (kmsg->ikm_header.msgh_bits & MACH_MSGH_BITS_COMPLEX) {
			ipc_kmsg_uncopyout_to_network(kmsg);
		}

		/*
		 * Move to next kmsg
		 */
		kmsg = kmsg->ikm_next;
	} while (kmsg != ipc_kmsg_queue_first(kmsgs));
}

/*
 * Called when a send to a remote port returns a notification that
 * the port is dead.  Caller must donate a port reference.
 */
norma_ipc_dead_destination(port)
	ipc_port_t port;
{
	ipc_kmsg_t kmsg;
	mach_port_rights_t sorights;

	printf1("norma_ipc_dead_destination: 0x%x:%x refs %d\n",
		port, port->ip_norma_uid, port->ip_references);

	assert(port->ip_norma_is_proxy);
	assert(port->ip_references > 0);

	/*
	 *	If the port is already dead, then the work below
	 *	has been done for us.  Just try to remove it from
	 *	the norma system.  Consume caller's reference.
	 */
	port->ip_norma_is_zombie = FALSE;
	if (!ip_active(port)) {
		norma_port_remove_try(port);
		ipc_port_release(port);
		return;
	}

	/*
	 * Convert all kmsgs from network to internal format.
	 * XXX
	 * We need to lock to prevent anyone else from
	 * queueing ports from this point.
	 */
	norma_ipc_uncopyout_all_kmsgs(port);

	/*
	 * The remote kernel counts us as having consumed one send-once
	 * right, if our destination right was a send-once right.
	 * This reduces how often we need to call norma_ipc_yield_transits.
	 *
	 * XXX Logic currently broken on remote side.  Can be put back
	 * XXX later.
	 */
/*	kmsg = ipc_kmsg_queue_first(&port->ip_messages.imq_messages);
 *	assert(kmsg != IKM_NULL);
 *	if (MACH_MSGH_BITS_REMOTE(kmsg->ikm_header.msgh_bits) ==
 *	    MACH_MSG_TYPE_PORT_SEND_ONCE) {
 *		sorights = port->ip_sorights - 1;
 *	} else {
 *		sorights = port->ip_sorights;
 *	}
 *
 *	printf1("*** That's really s=%d so=%d\n", port->ip_srights, sorights);
 */

	/*
	 *	Yield transits to principal if there are any.
	 */
	if (port->ip_norma_stransit != 0 || port->ip_norma_sotransit != 0) {
		norma_ipc_send_yield_transits(port->ip_norma_dest_node,
				      port->ip_norma_uid,
				      -(port->ip_norma_stransit),
				      -(port->ip_norma_sotransit));

		port->ip_norma_stransit = 0;
		port->ip_norma_sotransit = 0;
	}

	/*
	 *	Destroy the port.  We clear the nsrequest because
	 *	yield_transits took care of this (above).  Clearing
	 *	mscount and checking seqno are required by ipc_port_destroy().
	 */
	assert(ip_nsproxyp(port->ip_nsrequest));
	port->ip_nsrequest = IP_NULL;
	ipc_port_set_mscount(port, 0);
	assert(port->ip_seqno == 0);
	printf1("dead_destination: 0x%x:%x: about to destroy port\n",
		port, port->ip_norma_uid);
	ipc_port_destroy(port);
	printf1("dead_destination: 0x%x:%x: destroyed port\n",
		port, port->ip_norma_uid);
}

/*
 *	Error routine called when a message is received from the
 *	network and is undeliverable.
 */
void
norma_ipc_error_receiving(kmsgp)
	ipc_kmsg_t	*kmsgp;
{
	/*
	 *	Common case: it's not a norma message
	 */
	if ((*kmsgp)->ikm_size != IKM_SIZE_NORMA)
		return;

	/*
	 * XXX	Debugging check.  In addition to being sized
	 * XXX	as above, norma kmsgs are always page aligned.
	 */
	if ((vm_offset_t)*kmsgp != trunc_page((vm_offset_t) *kmsgp))
		panic("norma_ipc_error_receiving: bogus norma kmsg");

	/*
	 *	Drop this on a queue of kmsgs that need
	 *	to be destroyed due to receive errors.  The
	 *	destination (remote) port right is assumed to be in
	 *	local form (may be PORT_NULL), others in network form
	 *
	 * 	This queue is emptied by the netipc thread.
	 */
	assert(netipc_locked());
	(*kmsgp)->ikm_next = norma_kmsg_receive_error;
	(*kmsgp)->ikm_prev = IKM_BOGUS;  /* XXX Temporary */
	norma_kmsg_receive_error = *kmsgp;
	*kmsgp = IKM_NULL;

	netipc_thread_wakeup();
}

/*
 *	Routine called by the netipc thread to clean up receive errors.
 */

void
norma_ipc_clean_receive_errors()
{
	ipc_kmsg_t	kmsg;
	ipc_port_t	remote_port;

	assert(netipc_unlocked());
	netipc_thread_lock();
	while ((kmsg = norma_kmsg_receive_error) != IKM_NULL) {

		/*
		 *	Advance the queue and destroy this kmsg.
		 *	Call norma_ipc_finish_receiving to convert
		 *	all ports to internal form (from network form).
		 *	If the destination port is not null, check
		 *	to see if it should be untabled; this is because
		 *	the transit ref goes away in receive_*_dest, but
		 *	we don't want to throw the port away there.
		 */
		norma_kmsg_receive_error = kmsg->ikm_next;
		kmsg->ikm_next = IKM_BOGUS;  /* Temporary for debug */

		netipc_thread_unlock();
		norma_ipc_finish_receiving(&kmsg);
		remote_port = (ipc_port_t) kmsg->ikm_header.msgh_remote_port;
		if (remote_port != MACH_PORT_NULL)
			norma_port_remove_try(remote_port);
		ipc_kmsg_destroy(kmsg);
		netipc_thread_lock();
	}
	netipc_thread_unlock();
}

