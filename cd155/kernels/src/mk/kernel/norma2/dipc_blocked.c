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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1994  Intel Corporation.
 *
 *	$Id: dipc_blocked.c,v 1.4 1994/11/18 20:57:29 mtm Exp $
 */

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/message.h>

#include <vm/vm_map.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_pset.h>
#include <ipc/ipc_space.h>

#include <rpc_rdma/rpc.h>
#include <rpc_rdma/rdma.h>

#include <norma2/meta_kmsg.h>
#include <norma2/dipc_uid.h>
#include <norma2/dipc_migrate.h>
#include <norma2/dipc_server.h>
#include <norma2/norma_transport.h>


extern void	panic();

#define	DIPC_SEND_FLOW_SHIFT	(5)		/* 32 bits/word */
#define	DIPC_SEND_BYTE_SHIFT	(3)		/* 2^3 bits/byte */
#define	DIPC_SEND_FLOW_MASK	((1 << DIPC_SEND_FLOW_SHIFT)-1)

/*
 *	round_bpw(n) rounds "n" up to bits-per-word (32, usually).
 *
 *	bits2bytes(n) divides "n" by the bits-per-byte
 */
#define	round_bpw(n)		\
			(((n) + DIPC_SEND_FLOW_MASK) & ~DIPC_SEND_FLOW_MASK)
#define	bits2bytes(n)	((n) >> DIPC_SEND_BYTE_SHIFT)

typedef	unsigned long dipc_send_flow_t;

extern int	dipc_max_nodes();


/*
 *	Allocate, initialize, and attach to a port a
 *	data structure for rememebering remote blocked senders.
 */
void dipc_blocked_sender_init( ipc_port_t port )
{
	vm_size_t		size;
	int			max, bits, i;
	dipc_send_flow_t	*senders;

	max = dipc_max_nodes();
	bits = round_bpw(max);
	size = bits2bytes(bits);

	if ((senders = (dipc_send_flow_t *) kalloc(size)) == 0) {
		panic("dipc_blocked_sender_init");
	}

	for (i = 0; i < (bits >> DIPC_SEND_FLOW_SHIFT); i++)
		senders[i] = 0;

	port->dipc_blocked_senders = senders;
	port->dipc_blocked_sender_count = 0;
	port->dipc_blocked_sender_next = 0;
}


/*
 *	Free the memory used for the remote blocked senders list.
 */
void dipc_blocked_sender_destroy( ipc_port_t port )
{
	vm_size_t		size;
	int			max, bits;

	if (port->dipc_blocked_senders == 0)
		return;

	max = dipc_max_nodes();
	bits = round_bpw(max);
	size = bits2bytes(bits);

	kfree(port->dipc_blocked_senders, size);

	port->dipc_blocked_senders = 0;
	port->dipc_blocked_sender_count = 0;
	port->dipc_blocked_sender_next = 0;
}


/*
 *	Remember a remote send to a port that has a full message queue.
 */
void dipc_blocked_sender_add(
	ipc_port_t	port,
	rdma_node_t	node )
{
	dipc_send_flow_t	*send, word;
	int			bitn;

	if (port->dipc_blocked_senders == 0)
		dipc_blocked_sender_init(port);

	assert(node < dipc_max_nodes());

	send = &port->dipc_blocked_senders[node >> DIPC_SEND_FLOW_SHIFT];
	bitn = 1 << (node & DIPC_SEND_FLOW_MASK);

	if (((word = *send) & bitn) == 0) {
		*send = word | bitn;
		if (port->dipc_blocked_sender_count++ == 0)
			port->dipc_blocked_sender_next = node;
	}
}


/*
 *	Select and remove a remote sender from the blocked sender's list.
 */
boolean_t dipc_blocked_sender_extract( ipc_port_t port, rdma_node_t *nodep )
{
	int			i, max;
	rdma_node_t		n;
	dipc_send_flow_t	*send;
	int			bitn;

	if (port->dipc_blocked_senders == 0)
		return FALSE;

	if (port->dipc_blocked_sender_count == 0)
		return FALSE;

	max = dipc_max_nodes();
	n = port->dipc_blocked_sender_next;

	for (i = 0; i < max; i++) {
		send = &port->dipc_blocked_senders[n >> DIPC_SEND_FLOW_SHIFT];
		bitn = 1 << (n & DIPC_SEND_FLOW_MASK);
		if ((*send & bitn) != 0) {
			if ((port->dipc_blocked_sender_next = n + 1) == max)
				port->dipc_blocked_sender_next = 0;
			*nodep = n;
			*send &= ~bitn;
			port->dipc_blocked_sender_count--;
			return TRUE;
		}
		if (++n == max)
			n = 0;
	}

	assert(0);
	panic("dipc_blocked_sender_extract: no senders!");
	return FALSE;

}

