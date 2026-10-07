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
 * $Id: dipc_deadname.c,v 1.4 1994/11/18 20:57:39 mtm Exp $
 *
 * HISTORY:
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"

#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>

#include <mach/kern_return.h>

#include <norma/ipc_node.h>
#include <kern/assert.h>
#include <kern/host.h>

#include <norma2/dipc_uid.h>
#include <norma2/dipc_port.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_mqueue.h>
#include <norma2/norma_log.h>
#include <norma2/dipc_deadname.h>

extern void 	dipc_notify_dead_name_request( dipc_node_t, dipc_uid_t );

/*
 *	Functions for managing DIPC dead name requests.
 *
 *	Dead name notifications have a two-level structure: DIPC
 *	code tracks which nodes have to be notified, and the proxy
 *	structures on those nodes track the actual notifications.
 *	The existence of a dead name notification causes destruction
 *	of the corresponding proxies to be eagerly evaluated.  A special
 *	value for the notify port is used to redirect dead name
 *	notifications for remote proxies into the DIPC system.
 */

/*
 *	dipc_dead_name_request_init:
 *
 *	Called by the ipc system the first time dead name notification
 *	is requested for a proxy.
 *
 *	Caller must hold a port reference.
 */
void dipc_dead_name_request_init( ipc_port_t port )
{
	deadname_entry1(dipc_dead_name_request_init, port);

	assert(DIPC_IS_PROXY(port));

	if ( dipc_dead_name_request(port) == 0)
		return;

	ip_reference(port);
	ipc_port_destroy(port);
}

/*
 * Verify the DNR we are about to install is not already on this port's
 * DNR (DeadName Request) list.
 *
 * inputs:
 *	port	port in question
 *	name	from_node ID + DIPC magic == fake port name.
 *	right	another dipc magic cookie value.
 *
 * outputs:
 *	boolean:
 *		TRUE == DNR is already on this port's DNR list
 *		FALSE == DNR is NOT on the list
 */

boolean_t
dipc_check_duplicate_DNR( ipc_port_t port,
			mach_port_t name,
			ipc_port_t right )
{
	register ipc_port_request_index_t	index;
	ipc_port_request_t 			dnrequests;
	ipc_table_size_t			its;
	ipc_table_elems_t			size;

	assert(ip_active(port));
	assert(name != MACH_PORT_NULL);

	/* have any deadname notifications? */
	if ( (dnrequests = port->ip_dnrequests) == IPR_NULL )
		return FALSE;

	its = dnrequests->ipr_size;
	size = its->its_size;

	for (index = 1; index < size; index++) {
		register ipc_port_request_t	ipr;

		ipr = &dnrequests[index];

		if (ipr->ipr_name == MACH_PORT_NULL)
			continue;

		if ( ipr->ipr_name == name && ipr->ipr_soright == right )
			return	TRUE;
	}

	return	FALSE;
}

/*
 *	dipc_service_dead_name_request:
 *
 *	Called on principal's node by a proxy that wants to be
 *	notified when the receive right dies (so that it can process
 *	its dead name requests).
 *
 * implicit inputs:
 *	port is locked and referenced.
 *
 * implicit outputs:
 *	port is locked and referenced.
 *
 */
kern_return_t dipc_service_dead_name_request(
	dipc_node_t	from,
	ipc_port_t	port)
{
	ipc_port_request_index_t	junk;
	kern_return_t			kr;

	deadname_entry2(dipc_service_dead_name_request, from, port);

	if (from == node_self())
		return KERN_SUCCESS;

	kr = (kern_return_t) dipc_check_duplicate_DNR(
				port,
				(mach_port_t) DIPC_DNREQUEST_NAMEOF(from),
				(ipc_port_t) DIPC_IP_DNREQUEST_MAGIC_COOKIE );
	if ( kr == TRUE )
		return KERN_SUCCESS;

	for (;;) {
		kr = ipc_port_dnrequest(port,
				DIPC_DNREQUEST_NAMEOF(from),
				DIPC_IP_DNREQUEST_MAGIC_COOKIE,
				&junk);

		if (kr == KERN_SUCCESS) {
			deadname_log5(3,
			"%s installed DNR from node %d port %x uid %x\n",
				__FUNC__,
				from,
				port,
				port->dipc_uid);
			return KERN_SUCCESS;
		}

		if ((kr = ipc_port_dngrow(port)) != KERN_SUCCESS)
			panic("dipc_service_dead_name_request");

		ip_lock(port);	/* ipc_port_dngrow() return unlocked */
	}

	/*NOTREACHED*/
}


/*
 *	dipc_notify_dead_name:
 *
 *	Inform a registered node that a port has become dead.
 *
 */
void dipc_notify_dead_name(ipc_port_t port, unsigned long name)
{
	deadname_entry2(dipc_notify_dead_name, port, name);

	dipc_notify_dead_name_request(
		DIPC_DNREQUEST_NODEOF(name),
		port->dipc_uid);
}


/*
 *	dipc_service_dead_name_notify:
 *
 *	Called to handle dead name notification sent from
 *	the node that that holds/held the principal (receive right) at the
 *	time notification was requested. The proxy can be IO_DEAD and still
 * 	have a valid uid + one reference from the valid uid.
 *
 * inputs:
 *	port	address of port to be destroyed.
 *
 * implicit inputs:
 *	the port is locked and has one reference to be consumed.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	port is destroyed.
 */

void dipc_service_dead_name_notify(ipc_port_t port)
{
	deadname_entry1(dipc_service_dead_name_notify, port);

	DIPC_PORT_LOG(0,port);

	ip_release(port);	/* remove our caller's reference */

	assert( port->ip_references > 0 );
	assert( DIPC_IS_PROXY(port) );

	/* allow dipc_port_remove() succeed */
	port->dipc_forward = FALSE;

	ip_unlock(port);

	/*
	 *  Prevent returning transits to a dead port.
	 */
	port->dipc_do_nms = FALSE;

	if ( ip_active(port) )
		dipc_port_remove(port);
	else {
		/*
		 * if this proxy is DEAD then it will have only one reference
		 * from the valid uid. Add a reference to be consumed by the
		 * port release which will completely remove the port and
		 * return is to the free list.
		 */
		DIPC_PORT_LOG("forwarding proxy dies",port);
		ip_reference(port);
		assert( DIPC_UID_VALID(port) );
		if ( dipc_uid_destroy( port ) != DIPC_SUCCESS )
			panic("dead forwarding proxy with an invalid uid?");
		ipc_port_release(port);
	}
}
