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
 * Functions to support dipc special ports
 *
 * $Id: dipc_special.c,v 1.6 1995/03/28 15:58:15 cfleck Exp $
 *
 * HISTORY:
 */

#include "cpus.h"
#include "norma_ipc.h"
#include "mach_kdb.h"
#include "mach_assert.h"
#include "console_logging.h"

#include <mach/norma_special_ports.h>

#include <mach/mach_types.h>
#include <mach/boolean.h>
#include <mach/kern_return.h>
#include <mach/port.h>
#include <mach/message.h>
#include <mach/vm_param.h>

#include <kern/queue.h>
#include <kern/assert.h>
#include <kern/host.h>
#include <kern/sched_prim.h>
#include <kern/ipc_sched.h>
#include <kern/ipc_kobject.h>
#include <kern/zalloc.h>

#include <ipc/ipc_space.h>
#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_mqueue.h>
#include <ipc/ipc_thread.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_marequest.h>

#include <kern/ipc_kobject.h>

#include <device/device_port.h>

#include <norma2/dipc_uid.h>
#include <norma2/dipc_special.h>
#include <norma2/dipc_port.h>
#include <norma2/norma_transport.h>

#include <vm/vm_kern.h>

#if MACH_ASSERT
int dipc_vnode_pager_regflag=0;
#endif MACH_ASSERT

#define	dipc_node_valid(n)	(norma_node_status_op(0, (n)))

static ipc_port_t	dipc_host_special_port[DIPC_MAX_SPECIAL_ID];


static ipc_port_t dipc_make_special(
	ipc_port_t	port,
	unsigned long	id )
{
	ipc_port_t	old;

	assert(id >= 0);
	assert(id < DIPC_MAX_SPECIAL_ID);

	old = dipc_host_special_port[id];
	if (port != IP_NULL) {
		port->dipc_is_special = TRUE;
		/*
		 * If the port is a 'proxy' then we received the port in a
		 * message and the special UID has already been installed.
		 * Verify the need to install the uid.
		 */
		if ( !DIPC_UID_VALID(port) )
			(void) dipc_uid_install(port, MK_UID(node_self(), id));
	} else {
		db_printf("warning: dipc_make_special(port=0x%x, id=%d)\n",
			port, id);
	}
	dipc_host_special_port[id] = port;
	return old;
}

	
/*
 * Initialize the DIPC (Distributed Inter Process Communication) Facility.
 * Called once from kern/startup.c
 */

void dipc_ipc_init()
{
	(void) dipc_make_special(master_device_port, NORMA_DEVICE_PORT);
	(void) dipc_make_special(realhost.host_self, NORMA_HOST_PORT);
	(void) dipc_make_special(realhost.host_priv_self, NORMA_HOST_PRIV_PORT);
	(void) dipc_make_special(realhost.host_paging_self, NORMA_HOST_PAGING_PORT);

#if	MCMSG
	mcmsg_mp_init();
	mcmsg_enable_rx_interrupts();
#endif	/* MCMSG */
}


/*ARGSUSED*/
kern_return_t
dipc_port_location_hint(
	task_t		task,
	ipc_port_t	port,
	unsigned long	*nodep )
{
	if (!IP_VALID(port))
		return KERN_INVALID_ARGUMENT;

	if (DIPC_IS_PROXY(port))
		*nodep = port->dipc_node;
	else
		*nodep = node_self();

	ipc_port_release_send(port);
	return KERN_SUCCESS;
}

/*
 * MiG compatibility stub:
 *
 *      Return likely location to find port's receiver.
 */

kern_return_t
norma_port_location_hint(task, port, node)
	task_t		task;
	ipc_port_t	port;
	unsigned long	*node;
{
	return dipc_port_location_hint(task, port, node);
}

/*
 *
 * inputs:
 *
 *
 * outputs:
 *
 *
 * side effects:
 *
 * implicit inputs:
 *
 */

ipc_port_t
dipc_local_special_port(
	unsigned long		id,
	mach_msg_type_name_t	type )
{
	assert((int)id > 0 && id <= DIPC_MAX_SPECIAL_ID);

	if (dipc_host_special_port[id] == IP_NULL)
		return IP_NULL;

	if (type == MACH_MSG_TYPE_PORT_SEND)
		return ipc_port_make_send(dipc_host_special_port[id]);

	assert(type == MACH_MSG_TYPE_PORT_SEND_ONCE);

	return ipc_port_make_sonce(dipc_host_special_port[id]);
}


ipc_port_t dipc_remote_special_port(
	dipc_node_t		node,
	int 			id,
	mach_msg_type_name_t	type )
{
	ipc_port_t	port;
	dipc_uid_t	uid;

	assert(node != node_self());
	assert((int)id > 0 && id <= DIPC_MAX_SPECIAL_ID);

	uid = MK_UID(node, id);

	if ( ( port = dipc_port_lookup(uid)) == IP_NULL) {
		port = dipc_proxy_create(uid);
		if (port == IP_NULL)
			panic("dipc_remote_special_port: dipc_proxy_create");

		port->dipc_is_special = TRUE;
		port->dipc_transit = 0x7fffffff;
	}

	if (type == MACH_MSG_TYPE_PORT_SEND) {
		port->ip_srights++;
	} else {
		assert(type == MACH_MSG_TYPE_PORT_SEND_ONCE);
		port->ip_sorights++;
	}

	ip_reference(port);

	return port;
}


ipc_port_t dipc_remote_device(
	unsigned long		node,
	mach_msg_type_name_t	type )
{
	return dipc_remote_special_port((dipc_node_t) node,
			NORMA_DEVICE_PORT, type);
}


ipc_port_t dipc_remote_host(
	unsigned long		node,
	mach_msg_type_name_t	type )
{
	return dipc_remote_special_port((dipc_node_t) node,
			NORMA_HOST_PORT, type);
}


ipc_port_t dipc_remote_host_priv(
	unsigned long		node,
	mach_msg_type_name_t	type )
{
	return dipc_remote_special_port((dipc_node_t) node,
			NORMA_HOST_PRIV_PORT, type);
}


ipc_port_t dipc_remote_host_paging(
	unsigned long		node,
	mach_msg_type_name_t	type )
{
	return dipc_remote_special_port((dipc_node_t) node,
			NORMA_HOST_PAGING_PORT, type);
}


kern_return_t dipc_get_special_port(
	host_t		host,
	unsigned long	node,
	int		id,
	ipc_port_t	*portp )
{
	unsigned long	mynode;
	ipc_port_t	rhost;

	if (host == HOST_NULL)
		return KERN_INVALID_HOST;

	if (id <= 0)
		return KERN_INVALID_ARGUMENT;

	if (id > DIPC_MAX_SPECIAL_ID)
		return KERN_INVALID_ARGUMENT;

	if (!dipc_node_valid(node))
		return KERN_INVALID_ARGUMENT;

	mynode = node_self();
	if (id <= DIPC_MAX_SPECIAL_KERNEL_ID) {
		if (node == mynode) {
			*portp = dipc_local_special_port(
					id,
					MACH_MSG_TYPE_PORT_SEND);
		} else {
			*portp = dipc_remote_special_port(
					(dipc_node_t) node,
					(int) id,
					MACH_MSG_TYPE_PORT_SEND);
		}
		return KERN_SUCCESS;
	}

	if (node == mynode) {
		*portp = ipc_port_copy_send(dipc_host_special_port[id]);
		return KERN_SUCCESS;
	}

	rhost = dipc_remote_host_priv(node, MACH_MSG_TYPE_PORT_SEND);
	return r_norma_get_special_port(rhost, node, id, portp);
}

/*
 * MiG compatibility stub:
 *
 * The blocking behavior of this call is now somewhat funny.
 * A remote call will block until the remote node comes up.
 * However, in either local or remote case, a null port may be returned;
 * that is, we do not block for a valid port to show up.
 */
kern_return_t
norma_get_special_port(host, node, id, portp)
	host_t		host;
	unsigned long	node;
	unsigned long	id;
	ipc_port_t	*portp;
{
	return dipc_get_special_port(host, node, id, portp);

}

kern_return_t dipc_set_special_port(
	host_t		host,
	int		id,
	ipc_port_t	port )
{
	ipc_port_t	old;
	boolean_t	new_entry = TRUE;

	if (host == HOST_NULL)
		return KERN_INVALID_HOST;

	if ( (id <= 0) || (id > DIPC_MAX_SPECIAL_ID) )
		return KERN_INVALID_VALUE;

	if (id <= DIPC_MAX_SPECIAL_KERNEL_ID)
		return KERN_INVALID_VALUE;

	if (!IP_VALID(port))
		return KERN_INVALID_ARGUMENT;

	old = dipc_make_special(port, id);
	if (IP_VALID(old)) {
		new_entry = FALSE;
		if ( old != port )
			ipc_port_release_send(old);
	}

#if	CONSOLE_LOGGING
	if ( id == NORMA_CONSOLE_LOG_PORT )
		conslog_thread_startup( dipc_host_special_port[id], new_entry );
#endif
	return KERN_SUCCESS;
}

/*
 * MiG compatibility routine:
 *
 *	set a NORMA special port
 */

kern_return_t
norma_set_special_port(host, id, port)
        host_t host;
        unsigned long id;
        ipc_port_t port;
{
	return dipc_set_special_port(host, id, port);
}

void dipc_unset_special_port(ipc_port_t port)
{
	int	id;

	if ( ! IP_VALID(port) )
		return;

	port->dipc_is_special = FALSE;	/* so the port can really go away */

	for (id = 1; id <= DIPC_MAX_SPECIAL_ID; id++) {
		if (dipc_host_special_port[id] == port) {
			assert(id >= DIPC_MAX_SPECIAL_KERNEL_ID);
			dipc_host_special_port[id] = IP_NULL;
			if ( ip_active(port) )
				ipc_port_release_send(port);
		}
	}
}


ipc_port_t
remote_device(node, type)
	unsigned long		node;
	mach_msg_type_name_t	type;
{
	return dipc_remote_device(node, type);
}

ipc_port_t
remote_host(node, type)
	unsigned long 		node;
	mach_msg_type_name_t	type;
{
	return dipc_remote_host(node, type);
}

ipc_port_t
remote_host_priv(node, type)
	unsigned long		node;
	mach_msg_type_name_t	type;
{
	return dipc_remote_host_priv(node, type);
}

ipc_port_t
remote_host_paging(node, type)
	unsigned long 		node;
	mach_msg_type_name_t	type;
{
	return dipc_remote_host_paging(node, type);
}

/*
 *  routine for the vnode pager threads to call on startup.
 *  This routine will assign them a special pool of RDMA resources
 *  to pull from.
 *
 */


norma_enable_vnode_pager(
        host_t          host,
        thread_t        target)
{
        /*
         * valid host port?
         */
        if  (host == HOST_NULL )
                return KERN_INVALID_ARGUMENT;

        /*
         * is it my host priv port?
         */
        if ( host->host_priv_self != realhost.host_priv_self )
                return KERN_INVALID_ARGUMENT;

        /*
         * valid thread?
         */
        if  (target == THREAD_NULL )
                return KERN_INVALID_ARGUMENT;

	/* assign special resources to pull from */

	target->dipc_rdma_rx_group = NORMA_RDMA_GROUP_VNODE_PAGER_RX;
	target->dipc_rdma_tx_group = NORMA_RDMA_GROUP_VNODE_PAGER_TX;

#if MACH_ASSERT
	dipc_vnode_pager_regflag++;
#endif MACH_ASSERT


        return KERN_SUCCESS;
}

