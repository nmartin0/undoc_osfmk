/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 *	Copyright 1993 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_rpc.c,v 1.9 1995/02/24 17:58:22 andyp Exp $
 */

/*
 * msgp_rpc.c
 */

#define	MCMSG_MODULE	MCMSG_MODULE_RPC

#include <mach_kdb.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/msgp/msgp_hw.h>
#include <i860paragon/mcmsg/mcmsg_rpc.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <rpc_rdma/rpc.h>
#include <rpc_rdma/i860paragon/rpc.h>


#define	MSGP_RPC_DEBUG	1
#if	MSGP_RPC_DEBUG
#define	RPC_DEBUG(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	/* MSGP_RPC_DEBUG */
#define	RPC_DEBUG(a,b,c,d,e,f)
#endif	/* MSGP_RPC_DEBUG */


#if	defined(BIGPKTS) && defined(BUMPERS)
/*
 *	+bigpkts+bumpers has rules about the size of message headers
 *	that are followed by data.
 */

#define	RPC_SEND_HEADER_PAD()	\
	{ send2_now(0,0); send2_now(0,0); send2_now(0,0); send2_now(0,0); }

#define RPC_RECV_HEADER_PAD()	\
	{ recv2dummy(); recv2dummy(); recv2dummy(); recv2dummy(); }

#else	/* defined(BIGPKTS) && defined(BUMPERS) */

/*
 *	don't need the bumper padding for B-step NIC or non-BIGPKTS.
 */
#define	RPC_SEND_HEADER_PAD()
#define	RPC_RECV_HEADER_PAD()

#endif	/* defined(BIGPKTS) && defined(BUMPERS) */


extern int	rpc_engine_payload_size;

/*
 *	Each receive-side flow control item is 16 bits/node:
 *
 *	<14..00>	client slot number last delivered.
 *	<15>		if set, dumping requests from this node.
 *
 *	Each send-side flow control item is 1 bit/node.
 */
typedef unsigned short	rpc_recvflow_t;
typedef unsigned int	rpc_sendflow_t;

#define RECV_FLOW_SHIFT		(15)
#define	RECV_FLOW_DUMPING	(1 << RECV_FLOW_SHIFT)
#define RECV_FLOW_MASK		(RECV_FLOW_DUMPING - 1)
#define	RECV_FLOW_INIT		(RECV_FLOW_MASK)
#define	SEND_FLOW_SHIFT		(5)			/* 32 bits/word */
#define	SEND_FLOW_MASK		((1 << SEND_FLOW_SHIFT)-1)


/*
 *	RPC class data structure
 */
typedef struct rpc_classdata {
	rpc_engine_t		*recv_head;	/* list of receiving engines */
	rpc_recvflow_t		*recv_flow;	/* receive flow control bits */
	rpc_engine_t		*send_head;	/* list of pending sends */
	rpc_engine_t		*send_tail;	/* list of pending sends */
	rpc_sendflow_t		*send_flow;	/* send-block bits */
	unsigned short		recv_rexmit;	/* last node told selected */
	unsigned short		recv_dumping;	/* no. of nodes being dumped */
	unsigned long		total_sends;	/* simple statistics */
	unsigned long		total_recvs;	/* simple statistics */
} rpc_classdata_t;


rpc_classdata_t		*rpc_class;		/* vector of classes */
int			mcmsg_rpc_maxnode;	/* max no. of nodes */


/*
 *	A client RPC token is a tuple:
 *
 *	bits 16-31 inclusive is the originating node number.
 *	bits  0-14 inclusive is the originating slot number.
 *	bit     15 is reserved (limited to 32768 RPC slots)
 */
#define	rpc_make_client_token(node, slot)	\
	(((node) << 16) | ((slot) & 0x7fff))

#define rpc_node_from_token(token)	(((token) >> 16) & 0xffff)
#define rpc_slot_from_token(token)	((token) & 0x7fff)


/*
 *	void msgp_rpc_init()
 *
 *	PURPOSE
 *
 *	Called during MCMSG initialization.
 */
void msgp_rpc_init()
{
}


/*
 *	rpc_return_t msgp_rpc_engine_init(nclasses)
 *
 *	PURPOSE
 *
 *	Allocate and initialize during RPC engine initialization
 *	all per-class data structures.
 *
 */
rpc_return_t msgp_rpc_engine_init( int nclasses )
{
	int			i, j, size, bits;
	rpc_classdata_t		*c;

	mcmsg_rpc_maxnode = paragon_mesh_x * paragon_mesh_y;
	assert(mcmsg_rpc_maxnode > 0);

	/*
	 *	Allocate all class descriptors
	 */
	size = nclasses * sizeof(rpc_classdata_t);
	if ((rpc_class = (rpc_classdata_t *) kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}

	/*
	 *	Initialize each class
	 */
	for (i = 0; i < nclasses; i++) {

		c = &rpc_class[i];
		c->recv_head = 0;
		c->recv_rexmit = 0;
		c->recv_dumping = 0;
		c->send_head = 0;
		c->send_tail = 0;

		/*
		 *	Allocate and initialize the receive-side
		 *	flow control table.
		 */
		size = mcmsg_rpc_maxnode * sizeof(rpc_recvflow_t);
		if ((c->recv_flow = (rpc_recvflow_t *) kalloc(size)) == 0) {
			return RPC_SHORTAGE;
		}
		for (j = 0; j < mcmsg_rpc_maxnode; j++)
			c->recv_flow[j] = RECV_FLOW_INIT;

		/*
		 *	Allocate and initialize the send-side
		 *	flow control bit vector.
		 *
		 *	"bits" is the number of bits needed, rounded
	 	 *	up to the next 32 bit boundary.
		 */
		bits = (mcmsg_rpc_maxnode + SEND_FLOW_MASK) & ~SEND_FLOW_MASK;
		size = bits >> 3;
		if ((c->send_flow = (rpc_sendflow_t *) kalloc(size)) == 0) {
			return RPC_SHORTAGE;
		}
		for (j = 0; j < (bits >> SEND_FLOW_SHIFT); j++)
			c->send_flow[j] = 0;

	}

	return RPC_OK;
}


/*
 *	static rpc_engine_t *msgp_rpc_prune_one(c, r)
 *
 *	PURPOSE
 *
 *	Remove a known-to-be received transmission from the
 *	retransmission list.
 *
 *	RETURNS
 *
 *	Returns the pruned engine.
 *
 */
static rpc_engine_t *msgp_rpc_prune_one(rpc_classdata_t *c, rpc_engine_t *r)
{
	rpc_engine_t	*prev, *next;

	RPC_DEBUG("msgp rpc prune one", 2, c-rpc_class, r-rpc_engine, 0, 0);

	/*
	 *	Get the Left and right neighbors of the transmission
	 *	to be pruned.
	 *
	 *	If at the front or back of the list,
	 *	update the head and/or tail pointers.
	 *
	 *	Update adjacent links, if needed.
	 *
	 *	Mark the engine as having received the payload.
	 *
	 *	Return the pruned engine.
	 */
	assert(r != 0);
	prev = r->rpc_prev;
	next = r->rpc_next;
	assert(prev != r);
	assert(prev != RPC_PAYLOAD_RECEIVED);
	assert(next != r);

	if (c->send_head == r)
		c->send_head = next;
	if (c->send_tail == r)
		c->send_tail = prev;

	if (prev)
		prev->rpc_next = next;
	if (next)
		next->rpc_prev = prev;

	r->rpc_prev = RPC_PAYLOAD_RECEIVED;
	return r;

}


/*
 *	static rpc_engine_t *msgp_rpc_find_1st(r, node)
 *
 *	PURPOSE
 *
 *	Find the first pending send to the supplied node.
 *
 *	RETURNS
 *
 *	((rpc_engine_t *) 0)	if no sends are pending to that node.
 *	an rpc_engine_t *	the first send pending to the node.
 */
static rpc_engine_t *msgp_rpc_find_1st(rpc_engine_t *r, int node)
{
	int	slot;

	RPC_DEBUG("msgp rpc find 1st", 2, r, node, 0, 0);
	if (r == 0)
		return 0;

	slot = r - rpc_engine;
	assert(slot >= 0);
	assert(slot < rpc_engine_slots);
	RPC_DEBUG("msgp rpc find 1st okay", 3, r, node, slot, 0);

	while (r != ((rpc_engine_t *) 0)) {
		if (r->rpc_node == node)
			return r;
		r = r->rpc_next;
	}

	return r;
}


/*
 *	static rpc_engine_t *msgp_rpc_prune_class(class, node, slot)
 *
 *	PURPOSE
 *
 *	Prune pending sends from the retransmission list that
 *	are known to have been delivered.
 *
 *	RETURNS
 *
 *	The last engine pruned.
 *
 */
static rpc_engine_t *msgp_rpc_prune_class(int class, int node, int slot)
{
	rpc_engine_t		*first, *last = &rpc_engine[slot];
	rpc_classdata_t		*c = &rpc_class[class];

	assert(last->rpc_node == node);
	assert(slot >= 0);
	assert(slot < rpc_engine_slots);

	/*
	 *	If the engine has already been pruned,
	 *	do nothing.
	 */
	if (last->rpc_prev == RPC_PAYLOAD_RECEIVED)
		return 0;

	RPC_DEBUG("msgp rpc prune class ", 3, class, node, slot, 0);

	/*
	 *	Scan the retransmission list looking for
	 *	the first pending send to the target node.
	 *
	 *	Prune matching sends up to (but excluding)
	 *	the last-delivered send.
	 *
	 *	Prune the last known delivered send.
	 */
	first = msgp_rpc_find_1st(c->send_head, node);
	while (first != last) {
		 first = msgp_rpc_find_1st(
		 	    msgp_rpc_prune_one(c, first)->rpc_next, node);
	}

	return msgp_rpc_prune_one(c, last);
}


/*
 *	void mcmsg_rpc_send_request(mt, node, class, slot, intr)
 *
 *	PURPOSE
 *
 *	Service a posted a RPC send request operation and conditionally
 *	post an MCTRL_RPCQ ("RPC send request") message for transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	rpc_engine_send_request().
 */
void mcmsg_rpc_send_request( int mt, int node, int class, int slot, int intr )
{
	rpc_classdata_t		*c = &rpc_class[class];
	rpc_engine_t		*r = &rpc_engine[slot];
	rpc_sendflow_t		*wordp;
	int			bitn;

	/*
	 *	Record the parameters of the send request.
	 */
	r->rpc_node = node;
	r->rpc_class = class;
	r->rpc_route = calculate_route(node);
	r->rpc_token = rpc_make_client_token(ipsc_physnode, slot);

	/*
	 *	Append the slot to the retransmission list
	 *	for the destination class.
	 */
	r->rpc_next = 0;
	if ((r->rpc_prev = c->send_tail) != 0)
		r->rpc_prev->rpc_next = r;
	else
		c->send_head = r;
	c->send_tail = r;
	c->total_sends++;


	/*
	 *	If the destination node hasn't yet indicated that
	 *	it is dropping requests, schedule the request
	 *	for transmission.
	 */
	wordp = &c->send_flow[node >> SEND_FLOW_SHIFT];
	bitn = node & SEND_FLOW_MASK;
	if ((*wordp & (1 << bitn)) == 0) {
		mcmsg_send_tail(mt, MCTRL_RPCQ, slot);
	}
}


/*
 *	static int msgp_rpc_select_sender(c)
 *
 *	PURPOSE
 *
 *	Internal function to select a send-blocked node
 *	within a class.
 *
 */
static int msgp_rpc_select_sender(rpc_classdata_t *c)
{
	int	i, r;

	/*
	 *	Advance past the last node selected.
	 *
	 *	Scan the vector, looking for a node that is
	 *	being dumped.  When one is found:
	 *
	 *	- advance the round-robin selector index,
	 *
	 *	- and return the node of the selected sender.
	 *
	 */
	r = c->recv_rexmit;
	for (i = 0; i < mcmsg_rpc_maxnode; i++) {
		if (c->recv_flow[r] & RECV_FLOW_DUMPING) {
			if ((c->recv_rexmit = r + 1) == mcmsg_rpc_maxnode)
				c->recv_rexmit = 0;
			return r;
		}
		if (++r == mcmsg_rpc_maxnode)
			r = 0;
	}

	/*
	 *	This is bad...the count of nodes being dumped must be
	 *	non-zero, but there are no entries in the table.
	 */
	mcmsg_trace_drop("rpc select sender", c->recv_dumping);
	assert(0);

	return ipsc_physnode;
}


/*
 *	void mcmsg_rpc_recv_request(mt, class, slot)
 *
 *	PURPOSE
 *
 *	Service a posted RPC receive request operation and conditionally
 *	post an MCTRL_RPCN ("RPC retransmit request") message for
 *	transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	rpc_engine_recv_request().
 *
 */
void mcmsg_rpc_recv_request( int mt, int class, int slot )
{
	rpc_classdata_t		*c = &rpc_class[class];
	rpc_engine_t		*r = &rpc_engine[slot];

	/*
	 *	Record the parameters of the receive request.
	 */
	r->rpc_node = -1;	/* recv "any" */
	r->rpc_class = class;

	c->total_recvs++;

	/*
	 *	If the class is backlogged, drain the backlog first.
	 */
	if (c->recv_dumping > 0) {
		r->rpc_node = msgp_rpc_select_sender(c);
		mcmsg_send_tail(mt, MCTRL_RPCN, slot);
		return;
	}


	/*
	 *	No remote backlog in this class, so just add this slot
	 *	to the list of waiting receivers.
	 */
	r->rpc_next = c->recv_head;
	c->recv_head = r;
}


/*
 *	void mcmsg_rpc_send_reply(mt, slot, intr)
 *
 *	PURPOSE
 *
 *	Service a posted RPC send reply operation and post
 *	an MCTRL_RPCR ("RPC reply") message for transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	rpc_engine_send_reply().
 *
 */
void mcmsg_rpc_send_reply( int mt, int slot, int intr )
{
	rpc_engine_t	*r = &rpc_engine[slot];

	/*
	 *	Record the parameters of the reply request.
	 *
	 *	Schedule the reply for transmission.
	 */
	assert(r->rpc_rearm == 0);
	r->rpc_route = calculate_route(r->rpc_node);
	mcmsg_send_tail(mt, MCTRL_RPCR, slot);
}


/*
 *	void mcmsg_rpc_reply_recv(mt, class, slot, intr)
 *
 *	PURPOSE
 *
 *	Service a posted RPC send reply operation (send post a
 *	MCTRL_RPCR), and after the reply departs, generate
 *	the equivalent of another rpc_engine_receive_request().
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	rpc_engine_send_reply_recv().  It is impossible to
 *	generate reply departure notifications through this
 *	interface.
 */
void mcmsg_rpc_reply_recv( int mt, int class, int slot, int intr )
{
	rpc_engine_t		*r = &rpc_engine[slot];

	assert(class == r->rpc_class);
	assert(r->rpc_intr == 0);
	r->rpc_rearm = (intr << 1) | 1;
	r->rpc_route = calculate_route(r->rpc_node);
	mcmsg_send_tail(mt, MCTRL_RPCR, slot);
}


/*
 *	static void msgp_rpc_send_payload(mt, method, slot, hdr1, hdr2)
 *
 *	PURPOSE
 *
 *	Common internal routine for sending a payload.
 */
static void msgp_rpc_send_payload(int mt, int method, int slot, unsigned int hdr1, unsigned int hdr2)
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];
	mcmsg_trace_send(method, r->rpc_class, r->rpc_node, 2, hdr1, hdr2);
#if	BIGPKTS
	send2_now(r->rpc_route, 0);
	send2_now(hdr1, hdr2);
	RPC_SEND_HEADER_PAD();
	mcmsg_send_buf(r->rpc_bp1, r->rpc_bp2, rpc_engine_payload_size);
#else	BIGPKTS
	mcmsg_send_pkt2(mt,
		0,
		r->rpc_bp1,
		r->rpc_bp2,
		rpc_engine_payload_size,
		r->rpc_route,
		hdr1,
		hdr2);
#endif	BIGPKTS
}


/*
 *	void mcmsg_send_rpcq(mt, dummy, slot)
 *
 *	PURPOSE
 *
 *	Service a posted MCTRL_RPCQ message transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	mcmsg_rpc_send_request().
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	|   client node  |  client slot   | |  server class  |      MCTRL_RPCQ|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 *
 */
void mcmsg_send_rpcq( int mt, int dummy, int slot )
{
	unsigned long	hdr1, hdr2;

	hdr1 = (rpc_engine[slot].rpc_class << 16) | MCTRL_RPCQ;
	hdr2 = rpc_engine[slot].rpc_token;

	msgp_rpc_send_payload(mt, MCTRL_RPCQ, slot, hdr1, hdr2);
}


/*
 *	void msgp_rpc_dump_rpcq(class, slot, from)
 *
 *	PURPOSE
 *
 *	Dump the MCTRL_RPCQ in the fifo on the floor.
 *
 *	NOTES
 *
 *	This is a real routine to make it slightly easier to gather
 *	statistics at this point.
 */
void msgp_rpc_dump_rpcq( int class, int from, int slot )
{
	RPC_DEBUG("msgp rpc dump", 3, class, from, slot, 0);
#if	BIGPKTS
	RPC_RECV_HEADER_PAD();
#endif	BIGPKTS
	mcmsg_fifo_flush(rpc_engine_payload_size);
}


/*
 *	static void msgp_rpc_notify(r, ring, slot, &in, func)
 *
 *	PURPOSE
 *
 *	Common internal routine to add a slot to a ring buffer
 *	and call a function that will post a notification interrupt.
 */
unsigned long		msgp_rpc_mcp_spins;
unsigned long		msgp_rpc_notifications_generated;

#if	0
double				strange_bug_0A4_software_ECO;
#define	STRANGE_0A4_HACK()	mcmsg_hwclock(&strange_bug_0A4_software_ECO)
#endif

static void msgp_rpc_notify(
	rpc_engine_t	*r,
	rpc_slot_t	*ring,
	rpc_slot_t	slot,
	volatile int	*inp,
	void		(*func)() )
{
	register int	in;

	r->rpc_lock_mcp = 1;

	while (r->rpc_lock_cpu)
		msgp_rpc_mcp_spins++;

	/*
	 *  Cause a panic if we are not busy.
	 */
	if (r->rpc_state != RPC_ENGINE_BUSY)
		mp_panic(0);

#if	0
	RPC_DEBUG("RPC NOTIFY", 3, slot, r->rpc_state, r->rpc_intr, 0);
	STRANGE_0A4_HACK();
#endif

	asm("lock");
	r->rpc_state = RPC_ENGINE_IDLE;
	asm("unlock");

	if (r->rpc_intr) {

		RPC_DEBUG("msgp rpc notify", 4, ring, slot, inp, func);

		ring[(in = *inp)] = slot;
		if (++in == rpc_engine_slots)
			in = 0;
		*inp = in;
		(*func)();
		msgp_rpc_notifications_generated++;
	}

	r->rpc_lock_mcp = 0;
}


/*
 *	void msgp_rpc_deliver_request(slot, token)
 *
 *	PURPOSE
 *
 *	Pull data from the network into the waiting service slot.
 *
 *	NOTES
 *
 *	This routine optionally adds the slot to the notification
 *	ring buffer and generates the notification.
 */
void msgp_rpc_deliver_request( int slot, rpc_token_t token )
{
	rpc_engine_t	*r = &rpc_engine[slot];
	extern void	mp_rpc_request_intr();

	RPC_DEBUG("msgp rpc deliver request",
		4, slot, token, r->rpc_state, r->rpc_intr);

	/*
	 *	Save the client's token for later use.
	 *
	 *	Fill the waiting payload bay with data
	 *	from the network.
	 *
	 *	Mark the engine as <idle>, and post a notification
	 *	if it had been requested.
	 */
	r->rpc_token = token;
	r->rpc_node = rpc_node_from_token(token);

#if	BIGPKTS
	RPC_RECV_HEADER_PAD();
#endif	BIGPKTS
	mcmsg_recv_buf(r->rpc_bp1, r->rpc_bp2, rpc_engine_payload_size);

	msgp_rpc_notify(r, rpc_notify_request_ring, slot,
		&rpc_notify_request_in, mp_rpc_request_intr);
}


/*
 *	static int msgp_rpc_update_send_flow(class, node, flow)
 *
 *	PURPOSE
 *
 *	Update send-side flow control information with information
 *	returned from the receiver.  The receiver's dumping bit
 *	is put into our send-block bit.
 *
 *	RETURNS
 *
 *	1 if coming out of send-block, otherwise 0.
 *
 */
static int msgp_rpc_update_send_flow(
	int		class,
	int		node,
	rpc_recvflow_t	flow )
{
	rpc_classdata_t		*c;
	rpc_sendflow_t		*wordp;
	int			bitn, was, now;

	RPC_DEBUG("msgp rpc update send flow", 3, class, node, flow, 0);
	assert((flow & RECV_FLOW_MASK) == 0);

	/*
	 *	Update send-side flow-control information using
	 *	the "i'm-dumping-you" bit sent by the receiver.
	 *
	 *	Setting this bit will send-block this node if the
	 *	receiver has started dumping, clearing this bit
	 *	will keep this node streaming.
	 *
	 *	The bit shuffling below first clears the send-block
	 *	bit, and then or's in the receiver's dumping bit
	 *	which may be either a 0 or a 1.
	 */
	c = &rpc_class[class];
	wordp = &c->send_flow[node >> SEND_FLOW_SHIFT];
	bitn = node & SEND_FLOW_MASK;

	was = (*wordp & (1 << bitn));
	*wordp = (*wordp & ~(was)) | ((flow >> RECV_FLOW_SHIFT) << bitn);
	now = (*wordp & (1 << bitn));

	return ((now == 0) && (was != 0));
}


/*
 *	static rpc_engine_t *msgp_rpc_update_flow_info(class, node, flow)
 *
 *	PURPOSE
 *
 *	Update send-side flow control information and prune the
 *	retransmission list.
 *
 *	RETURNS
 *
 *	If the transmission list is pruned, it returns the last pruned
 *	engine.  Otherwise, ((rpc_engine_t *) 0) is returned.
 */
static rpc_engine_t *msgp_rpc_update_flow_info(
	int		class,
	int		node,
	rpc_recvflow_t	flow)
{
	RPC_DEBUG("msgp rpc update flow info", 3, class, node, flow, 0);

	/*
	 *	If there is a last-received slot number in the
	 *	receive flow data, prune the class up to that slot.
	 */
	if ((flow &= RECV_FLOW_MASK) != RECV_FLOW_INIT)
		return msgp_rpc_prune_class(class, node, flow);

	return 0;
}


unsigned long msgp_rpc_unblock_send;
unsigned long msgp_rpc_unblock_send_max;

static int msgp_rpc_unblock_resend(rpc_engine_t *r, int node)
{
	int	i = 0;

	RPC_DEBUG("msgp rpc unblock BEGIN", 3, r, r - rpc_engine, node, 0);

	while (r) {
		mcmsg_send(0, MCTRL_RPCQ, r - rpc_engine);
		msgp_rpc_unblock_send++;
		i++;
		r = msgp_rpc_find_1st(r->rpc_next, node);
	}

	if (i > msgp_rpc_unblock_send_max)
		msgp_rpc_unblock_send_max = i;

	RPC_DEBUG("msgp rpc unblock DONE", 0, 0, 0, 0, 0);
}


/*
 *	static void msgp_rpc_deliver_reply(slot, node, flow)
 *
 *	PURPOSE
 *
 *	Pull data from the network into the waiting client slot.
 *
 *	NOTES
 *
 *	Optionally add the slot to the reply arrival notification
 *	ring buffer and generate notifications.
 *
 *	Update send-side flow control information (retransmission
 *	list houskeeping).
 */
static void msgp_rpc_deliver_reply( int slot, int node, rpc_recvflow_t flow )
{
	rpc_engine_t	*r = &rpc_engine[slot];
	int		unblocking;
	rpc_class_t	class;
	extern void	mp_rpc_reply_intr();

	RPC_DEBUG("msgp rpc deliver reply",
		4, slot, node, flow, r->rpc_intr);

	/*
	 *	Deliver the payload to the requesting
	 *	client's buffer.
	 *
	 *	Update local flow control status and prune the
	 *	retransmission list.
	 *
	 *	Mark the engine as <idle>, and run
	 *	the notification loop if it had been requested.
	 */
#if	BIGPKTS
	RPC_RECV_HEADER_PAD();
#endif	BIGPKTS
	mcmsg_recv_buf(r->rpc_bp1, r->rpc_bp2, rpc_engine_payload_size);

	/*
	 *	We MUST save r->rpc_class in a temporary variable, as
	 *	r->rpc_class might be overwritten in the callback function.
	 */
	class = r->rpc_class;

	unblocking = msgp_rpc_update_send_flow(class, node,
			flow & RECV_FLOW_DUMPING);

	(void) msgp_rpc_update_flow_info(class, node, flow);

	msgp_rpc_notify(r, rpc_notify_reply_ring, slot,
		&rpc_notify_reply_in, mp_rpc_reply_intr);

	if (unblocking) {
		r = msgp_rpc_find_1st(rpc_class[class].send_head, node);
		if (r != 0) {
			msgp_rpc_unblock_resend(r, node);
		}
	}
}


/*
 *	void mcmsg_recv_rpcq(hdr1, class)
 *
 *	PURPOSE
 *
 *	Process (drain from the network) and conditionally deliver
 *	an inbound MCTRL_RPCQ message.
 *
 *	NOTES
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	|   client node  |  client slot   | |  server class  |      MCTRL_RPCQ|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 *
 */
void mcmsg_recv_rpcq( unsigned long hdr1, rpc_token_t token )
{
	rpc_classdata_t		*c;
	rpc_recvflow_t		*flow;
	rpc_engine_t		*r;
	int			class, from, slot;

	class = (hdr1 >> 16) & 0xffff;
	from  = rpc_node_from_token(token);
	slot  = rpc_slot_from_token(token);

	mcmsg_trace_recv(hdr1, token, from, 2, hdr1, token);
	assert((from >= 0) && (from < mcmsg_rpc_maxnode));

	/*
	 *	If dumping requests from the sending node,
	 *	continue to do so.
	 */
	c = &rpc_class[class];
	flow = &(c->recv_flow[from]);
	if (*flow & RECV_FLOW_DUMPING) {
		msgp_rpc_dump_rpcq(class, from, slot);
		return;
	}


	/*
	 *	If there are no waiting server slots, start dumping
	 *	requests from this node.
	 *
	 *	If this is the first node about to be dumped, a
	 *	small optimization can be had by setting the
	 *	retransmission index to the node about to
	 *	be dumped.
	 */
	if ((r = c->recv_head) == 0) {
		*flow |= RECV_FLOW_DUMPING;
		if (++c->recv_dumping == 1)
			c->recv_rexmit = from;
		msgp_rpc_dump_rpcq(class, from, slot);
		return;
	}


	/*
	 *	Have a waiting receiver -- remove the receiver
	 *	from the list of receivers, update the receive-side
	 *	flow control to indicate successful reception from
	 *	the sending slot, and deliver the payload.
	 */
	c->recv_head = r->rpc_next;
	r->rpc_next = 0;
	*flow = slot;		/* also clears the dumping bit */
	msgp_rpc_deliver_request(r - rpc_engine, token);
}


/*
 *	void mcmsg_send_rpcr(mt, dummy, slot)
 *
 *	PURPOSE
 *
 *	Service a posted MCTRL_RPCR message transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	mcmsg_rpc_send_reply().
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  server node   | |  client slot   |      MCTRL_RPCR|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 */
void mcmsg_send_rpcr( int mt, int dummy, int slot )
{
	rpc_engine_t	*r = &rpc_engine[slot];
	rpc_recvflow_t	*flowp, flow;
	unsigned int	hdr1, hdr2, reply;
	extern void	mp_rpc_depart_intr();

	/*
	 *	Prep the two header words and send the payload.
	 */
	reply = rpc_slot_from_token(r->rpc_token);
	flowp = &rpc_class[r->rpc_class].recv_flow[r->rpc_node];
	flow = *flowp;

	hdr1 = (reply << 16) | MCTRL_RPCR;
	hdr2 = ( flow << 16) | ipsc_physnode;

	/*
	 *	Always go back to the init state after
	 *	providing the sender with information.
	 *	Keep the dumping bit, set the slot number
	 *	to the ground state.
	 *
	 *	(Resolves a race that can occur when
	 *	send-side slots are recycled quickly.)
	 */
	*flowp = (flow & RECV_FLOW_DUMPING) | RECV_FLOW_INIT;

	msgp_rpc_send_payload(mt, MCTRL_RPCR, slot, hdr1, hdr2);

	msgp_rpc_notify(r, rpc_notify_depart_ring, slot,
		&rpc_notify_depart_in, mp_rpc_depart_intr);

	if (r->rpc_rearm != 0) {
		assert(r->rpc_state == RPC_ENGINE_IDLE);
		asm("lock");
		r->rpc_state = RPC_ENGINE_BUSY;
		r->rpc_intr = (r->rpc_rearm >> 1);
		r->rpc_rearm = 0;
		asm("unlock");
		mcmsg_rpc_recv_request(mt, r->rpc_class, slot);
	}
}


/*
 *	void mcmsg_recv_rpcr(hdr1, hdr2)
 *
 *	PURPOSE
 *
 *	Process (drain from the network) and deliver an inbound
 *	MCTRL_RPCR message.
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  server node   | |  client slot   |      MCTRL_RPCR|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 */
void mcmsg_recv_rpcr( unsigned long hdr1, unsigned long hdr2 )
{
	int		node, class, slot;
	rpc_recvflow_t	flow;

	slot = (hdr1 >> 16) & 0xffff;
	flow = (hdr2 >> 16) & 0xffff;
	node = hdr2 & 0xffff;

	mcmsg_trace_recv(hdr1, hdr2, flow, 2, hdr1, hdr2);
	assert((node >= 0) && (node < mcmsg_rpc_maxnode));

	/*
	 *	Deliver the reply from the network.
	 */
	msgp_rpc_deliver_reply(slot, node, flow);
}


/*
 *	void mcmsg_send_rpcn(mt, dummy, token)
 *
 *	PURPOSE
 *
 *	Service a posted MCTRL_RPCN message transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as the result of calling
 *	mcmsg_rpc_recv_request() and will cause the sender
 *	to retransmit a payload that we have dumped.
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  server node   | |  server class  |      MCTRL_RPCN|
 *	+----------------+----------------+ +----------------+----------------+
 *	|0000000000000000 0000000000000000| |0000000000000000|  server token  |
 *	+----------------+----------------+ +----------------+----------------+
 *
 */
void mcmsg_send_rpcn( int mt, int dummy, unsigned int slot )
{
	rpc_engine_t	*r = &rpc_engine[slot];
	int		node, class;
	rpc_recvflow_t	*flowp, flow;
	unsigned int	hdr1, hdr2, hdr3;

	node = r->rpc_node;
	class = r->rpc_class;
	flowp = &rpc_class[class].recv_flow[node];
	flow = *flowp;

	hdr1 = (class << 16) | MCTRL_RPCN;
	hdr2 = ( flow << 16) | ipsc_physnode;
	hdr3 = slot;

	*flowp = (flow & RECV_FLOW_DUMPING) | RECV_FLOW_INIT;

	mcmsg_trace_send(MCTRL_RPCN, slot, node, 2, hdr2, hdr3);
	send2_now(calculate_route(node), 0);
	send2_now(hdr1, hdr2);
	send2eod_now(hdr3, 0);
}


/*
 *	void mcmsg_recv_rpcn(hdr1, hdr2)
 *
 *	PURPOSE
 *
 *	Process (drain from the network) and respond to a
 *	MCTRL_RPCN message.
 *
 *	NOTES
 *
 *	A MCTRL_RPCA message will be posted for transmission.
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  server node   | |  server class  |      MCTRL_RPCN|
 *	+----------------+----------------+ +----------------+----------------+
 *	|0000000000000000 0000000000000000| |0000000000000000|  server token  |
 *	+----------------+----------------+ +----------------+----------------+
 *
 *
 */
void mcmsg_recv_rpcn( unsigned long hdr1, unsigned long hdr2 )
{
	int		class, node, slot, unblocking;
	rpc_recvflow_t	flow;
	rpc_token_t	token;
	rpc_engine_t	*r;
	unsigned int	null;

	/*
	 *	Extract the packet from the network.
	 */
	class = (hdr1 >> 16) & 0xffff;
	node = hdr2 & 0xffff;
	flow = (hdr2 >> 16) & 0xffff;
	recv2(token, null);
	mcmsg_trace_recv(hdr1, hdr2, token, 2, hdr1, hdr2);
	assert((node >= 0) && (node < mcmsg_rpc_maxnode));

	/*
	 *	Prune the retransmission list and update
	 *	flow control information.
	 *
	 *	If the receiver has already informed us of
	 *	the last slot received, the slot field of
	 *	the flow information contains a special
	 *	value RECV_FLOW_INIT.
	 *
	 *	If nothing was pruned away, go look for
	 *	the first pending send starting from the
	 *	front of the retransmission list.
	 *
	 */
	unblocking = msgp_rpc_update_send_flow(class, node,
		flow & RECV_FLOW_DUMPING);

	if ((r = msgp_rpc_update_flow_info(class, node, flow)) != 0) {
		r = msgp_rpc_find_1st(r->rpc_next, node);
	} else {
		r = msgp_rpc_find_1st(rpc_class[class].send_head, node);
	}

	/*
	 *	"r" now represents the engine holding the payload that
	 *	should be retransmitted.
	 *
	 *	if "r" is null, that means the node asking for a payload
	 *	is operating from a stale recv_dumping bit -- there is
	 *	an updated one on it's way already...but was not processed
	 *	before the MCTRL_RPCN was sent.
	 */
	if (r == 0) {
		RPC_DEBUG("no thanks", 2, node, token, 0, 0);
		mcmsg_send_tail(0, MCTRL_RPCU, node, token);
		return;
	}

	/*
	 *	In addition to the payload, the receiving node needs
	 *	to know if this node has more blocked senders behind
	 *	the item to be retransmitted.
	 *
	 *	Fabricate a receiver-side flow descriptor that includes
	 *	the slot for the item to be retransmitted (we're
	 *	guaranteed that it will get there), and a bit indicating
	 *	we've still got blocked senders.
	 *
	 *	The send-flow bit also needs to stay in sync with the
	 *	bit we're giving the receiver.
	 */
	slot = r - rpc_engine;
	RPC_DEBUG("msgp rpc engine selected", 4, slot, r, r->rpc_next, node);

	/*
	 *	inform the receiver of any additional sends
	 *	scheduled to the same class after the selected payload.
	 */
	flow = slot;
	if (msgp_rpc_find_1st(r->rpc_next, node) != 0) {
		flow |= RECV_FLOW_DUMPING;
	}

	/*
	 *	A reply to a retransmission request implies
	 *	removing the engine from the retransmission
	 *	list as well (and is indicated in the flow
	 *	data we're sending back).
	 */
	assert(r->rpc_prev != RPC_PAYLOAD_RECEIVED);
	(void) msgp_rpc_prune_one(&rpc_class[r->rpc_class], r);

	/*
	 *	Schedule the payload for transmission
	 */
	mcmsg_send(0, MCTRL_RPCA, slot, token, flow);

	/*
	 *	If transitioning out of send-block, resend any
	 *	send-blocked requests that arrived after having given
	 *	the receiver the indication of having no more
	 *	blocked sends.
	 */
	if (unblocking && ((flow & RECV_FLOW_DUMPING) == 0)) {
		r = msgp_rpc_find_1st(rpc_class[r->rpc_class].send_head, node);
		if (r != 0) {
			msgp_rpc_unblock_resend(r, node);
		}
	}

}


/*
 *	void mcmsg_send_rpca(mt, dummy, slot, token, flow)
 *
 *	PURPOSE
 *
 *	Service a posted MCTRL_RPCA message transmission.
 *
 *	NOTES
 *
 *	This routine is invoked as a result of calling
 *	mcmsg_recv_rpcn().
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  client node   | |  server token  |      MCTRL_RPCA|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 */
void mcmsg_send_rpca( int mt, int dummy, int slot, int token, rpc_recvflow_t flow )
{
	unsigned int	hdr1, hdr2;

	/*
	 *	Prep the header words and resend the payload.
	 */
	hdr1 = (token << 16) | MCTRL_RPCA;
	hdr2 = (flow  << 16) | ipsc_physnode;

	msgp_rpc_send_payload(mt, MCTRL_RPCA, slot, hdr1, hdr2);
}


/*
 *	void mcmsg_recv_rpca(hdr1, class)
 *
 *	PURPOSE
 *
 *	Process (drain from the network) and deliver an inbound
 *	MCTRL_RPCA message.
 *
 *	The packet as it appears on the wire:
 *
 *      63                              32  31                               0
 *	+----------------+----------------+ +----------------+----------------+
 *	| recv flow data |  client node   | |  server token  |      MCTRL_RPCA|
 *	+----------------+----------------+ +----------------+----------------+
 *	|             payload             | |             payload             |
 *	+----------------+----------------+ +----------------+----------------+
 *	.
 *	.
 *	.
 */
void mcmsg_recv_rpca( unsigned long hdr1, unsigned long hdr2 )
{
	int		slot, from;
	rpc_recvflow_t	flow, *flowp;
	rpc_classdata_t		*c;

	/*
	 *	Extract the control information from the
	 *	message and drop a trace.
	 */
	slot = (hdr1 >> 16) & 0xffff;
	flow = (hdr2 >> 16) & 0xffff;
	from = (hdr2 & 0xffff);
	mcmsg_trace_recv(hdr1, hdr2, from, 2, hdr1, hdr2);
	assert((from >= 0) && (from < mcmsg_rpc_maxnode));

	/*
	 *	Delivery of the retransmitted payload will be
	 *	to the server slot named in the MCTRL_RPCN
	 *	sent to the client (and carried back in the MCTRL_RPCA).
	 */

	/*
	 *	Update the receive-side flow control information
	 *	using the sender-side generated values.
	 *
	 *	There isn't room in the header to send an "honest"
	 *	client-side token...but it is easy to fabricate
	 *	one using the updated flow-control information...
	 *
	 *	Update receive-side flow-control information
	 *	using information from the sender.  If the sender
	 *	has more blocked senders, mark the node as being
	 *	dumped.
	 *
	 *	Deliver the request (as if it hadn't been dumped)
	 */
	c = &rpc_class[rpc_engine[slot].rpc_class];
	flowp = &c->recv_flow[from];
	if (*flowp & RECV_FLOW_DUMPING) {
		*flowp &= ~RECV_FLOW_DUMPING;
		c->recv_dumping--;
	}
	if (flow & RECV_FLOW_DUMPING) {
		if (++c->recv_dumping == 1) {
			c->recv_rexmit = from;
		}
	}
	*flowp = flow;

	msgp_rpc_deliver_request(slot,
		rpc_make_client_token(from, flow & RECV_FLOW_MASK));
}


void mcmsg_send_rpcu(int mt, int dummy, int node, unsigned token)
{
	unsigned long	hdr1, hdr2;


	hdr1 = (         token << 16) | MCTRL_RPCU;
	hdr2 = (RECV_FLOW_INIT << 16) | ipsc_physnode;

	mcmsg_trace_send(MCTRL_RPCU, token, node, 2, hdr1, hdr2);
	send2_now(calculate_route(node), 0);
	send2eod_now(hdr1, hdr2);
}


void mcmsg_recv_rpcu( unsigned long hdr1, unsigned long hdr2 )
{
	int		slot, from;
	rpc_recvflow_t	flow;

	slot = (hdr1 >> 16) & 0xffff;
	flow = (hdr2 >> 16) & 0xffff;
	from = (hdr2 & 0xffff);
	mcmsg_trace_recv(hdr1, hdr2, from, 2, hdr1, hdr2);
	assert((from >= 0) && (from < mcmsg_rpc_maxnode));

	mcmsg_rpc_recv_request(0, rpc_engine[slot].rpc_class, slot);
}


#if	MACH_KDB

/*
 *	Pretty print an RPC class
 */
int rpc_print_engine_class( int class )
{
	rpc_classdata_t		*c;
	rpc_engine_t		*r;
	rpc_recvflow_t		rflow;
	rpc_sendflow_t		*wordp;
	int			bitn;
	int			i, count;
	extern int		indent;

	c = &rpc_class[class];

	iprintf("rpc class=%d (0x%x) {\n", class, c);
	indent += 2;

	iprintf("total_sends=%d, total_recvs=%d\n",
		c->total_sends, c->total_recvs);

	iprintf("recv_head=0x%x {\n", c->recv_head);
	indent += 2;
	for (r = c->recv_head, count = 0; r; r = r->rpc_next, count++) {
		(void) rpc_print_engine( r - rpc_engine );
	}
	indent -= 2;
	iprintf("} /* %d receivers */\n", count);

	iprintf("recv_flow=0x%x {\n", c->recv_flow);
	indent += 2;
	for (i = 0; i < mcmsg_rpc_maxnode; i++) {
		rflow = c->recv_flow[i];
		iprintf("%5d: %5d%c\n",
			i,
			rflow & RECV_FLOW_MASK,
			(rflow & RECV_FLOW_DUMPING) ? '*' : ' ');
	}
	indent -= 2;
	iprintf("}\n");

	iprintf("recv_rexmit=%d, recv_dumping=%d\n",
		c->recv_rexmit, c->recv_dumping);

	iprintf("send_head=0x%x, send_tail=0x%x {\n",
		c->send_head, c->send_tail);
	indent += 2;
	for (r = c->send_head, count = 0; r; r = r->rpc_next, count++) {
		(void) rpc_print_engine( r - rpc_engine );
	}
	indent -= 2;
	iprintf("} /* %d senders */\n", count);

	iprintf("send_flow=0x%x {\n", c->send_flow);
	indent += 2;
	for (i = 0, count = 0; i < mcmsg_rpc_maxnode; i++) {
		wordp = &c->send_flow[i >> SEND_FLOW_SHIFT];
		bitn = i & SEND_FLOW_MASK;
		if ((*wordp & (1 << bitn)) != 0) {
			count++;
			iprintf("%4d: (wordp=0x%x, bitn=%d)\n",
				i, wordp, bitn);
		}
	}
	indent -= 2;
	iprintf("} /* %d nodes dumping */\n", count);

	indent -= 2;
	iprintf("}\n");

	return class;
}

#endif	/* MACH_KDB */
