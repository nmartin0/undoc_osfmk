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
 *	Copyright 1994, 1995 Intel Corporation.
 *
 *	$Id: mcmsg_rpc.c,v 1.12 1995/02/08 21:20:34 robboy Exp $
 */

/**************************************************************************
*  RPC message types:
*    RPCQ:  Message from client requesting service
*    RPCR:  Reply from server when service is done
*    RPCN:  Request by server for resend (previous message was dumped)
*    RPCA:  Resend by client
*    RPCU:  "Undo" notification - client has nothing to resend
*
* State diagram for a sender (client):
*  ___________                _____________               _____________
*  |         |   get request  |           |               |           |
*  | Ready   |   to send      | Sending   | send RPCQ     | Reply     |
*  |         |--------------->|           |-------------->| wait      |
*  |_________|                |___________|               |___________|
*   ^    ^                                                  |     |
*   |    | notify                                           |     |
*   |    |                                                  |     |
*   |  __|________                                          |     | receive
*   |  |         |         receive RPCR                     |     | RPCN
*   |  | Done    |<-----------------------------------------|     |
*   |  |         |                                                |
*   |  |_________|                                               \/
*   |         ^               _____________               _____________
*   |         |     receive   |           |               |           |
*   |         |     RPCR      | reply     | send RPCA     | resending |
*   |         |---------------| wait      |<--------------|           |
*   |send                     |___________|               |___________|
*   |RPCU                                                      |
*  _|_________                                                 |     
*  |         |                   nothing to resend             |
*  | Undoing |<------------------------------------------------|   
*  |         |                                             
*  |_________|                                            
*
*
* State diagram for a receiver (server):
*  ___________                _____________               _____________
*  |         |   get request  |           |               |           |
*  | Idle    |   to receive   | Request   | receive RPCQ  | Servicing |
*  |         |--------------->| wait      |-------------->|           |
*  |_________|                |___________|               |___________|
*   |     ^                                                 |     ^
*   |     | Send RPCR                                       |     |
*   |     |                                                 |     |
*   |  ___|_______                                          |     |
*   |  |         |               service done               |     |
*   |  | replying|<-----------------------------------------|     |
*   |  |         |                                                |receive
*   |  |_________|                                                |RPCA
*   |                                                             |
*   |Receive                                                      |
*   |RPCQ                                                         |
*   |                                                             |
*  _\/__________                _____________               ______|______
*  |           |   get request  |           |               |           |
*  |Backlogged |   to receive   | Retransmit| send RPCN     | Request   |
*  |           | -------------->|           |-------------->| wait      |
*  |___________|                |___________|               |___________|
*
*****************************************************************************/
#include <mach_kdb.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/machine/vm_param.h>
#include <kern/assert.h>
#include <kern/lock.h>
#include <rpc_rdma/rpc.h>
#include <rpc_rdma/i860paragon/rpc.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>


extern void	mcmsg_post( int, ... );

int		rpc_classes;

int		rpc_engine_slots;
rpc_engine_t	*rpc_engine;
int		rpc_engine_payload_size = 64;

/*
 *	Notification ring buffer pointers and indices.
 */
rpc_slot_t	*rpc_notify_request_ring;
int		rpc_notify_request_in, rpc_notify_request_out;

rpc_slot_t	*rpc_notify_reply_ring;
int		rpc_notify_reply_in, rpc_notify_reply_out;

rpc_slot_t	*rpc_notify_depart_ring;
int		rpc_notify_depart_in, rpc_notify_depart_out;

decl_simple_lock_data(, rpc_engine_notification_lock)


/*
 *	void mcmsg_rpc_init()
 *
 *	PURPOSE
 *
 *	Called during MCMSG initialization.
 *
 *	NOTES
 *
 *	Essentially all of the initialization work is performed
 *	in the other direction during kernel initialization.
 */
void mcmsg_rpc_init()
{
	extern void	msgp_rpc_init( void );

	simple_lock_init( &rpc_engine_notification_lock );
	msgp_rpc_init();
}


/*
 *	rpc_return_t rpc_init_engine(nhandles, ngroups, nclasses)
 *
 *	PURPOSE
 *
 *	Allocate and initialize all resources required by the RPC engines.
 *
 *	NOTES
 *
 *	Must be called exactly once during system initialization.
 *
 *	RETURNS
 *
 *	RPC_OK		if resources could be allocated.
 *	RPC_SHORTAGE		if too many resources are needed.
 */
rpc_return_t rpc_init_engine( int nhandles, int ngroups, int nclasses )
{
	int		i;
	vm_offset_t	addr;
	vm_size_t	size, rem, payload;
	rpc_engine_t	*r;

	/*
	 *	allocate the RPC engine state vector
	 */
	size = nhandles * sizeof(struct rpc_engine);
	if ((addr = kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}
	rpc_engine = (rpc_engine_t *) addr;

	/*
	 *	allocate and initialize the notification
	 *	ring buffers.
	 */
	size = nhandles * sizeof(rpc_slot_t);
	if ((rpc_notify_request_ring = (rpc_slot_t *) kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}
	if ((rpc_notify_reply_ring = (rpc_slot_t *) kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}
	if ((rpc_notify_depart_ring = (rpc_slot_t *) kalloc(size)) == 0) {
		return RPC_SHORTAGE;
	}
	rpc_notify_request_in = rpc_notify_request_out = 0;
	rpc_notify_reply_in = rpc_notify_reply_out = 0;
	rpc_notify_depart_in = rpc_notify_depart_out = 0;

	/*
	 *	allocate space for all RPC payload bays, and
	 *	initialize each RPC engine state.
	 */
	payload = rpc_engine_payload_size;
	rem = 0;
	r = &rpc_engine[0];
	for (i = 0; i < nhandles; i++, r++) {

		/*
		 *	if there is no more room in the current page,
		 *	get another one and align it.
		 *
		 */
		if (rem < payload) {
			if ((addr = kalloc(PAGE_SIZE)) == 0) {
				return RPC_SHORTAGE;
			}
			rem = PAGE_SIZE;

			/*
			 *	this probably paranoia -- kalloc() of
			 *	a page size is *very* likely to return
			 *	a pointer to the beginning of a page,
			 *	and those are always favorably aligned.
			 */
			while (addr & (payload - 1))
				addr++, rem--;
		}
		assert(rem >= payload);

		/*
		 *	keep both the virtual address and the
		 *	validated physical address of the payload bay.
		 */
		r->rpc_buffer = (rpc_engine_buf_t *) addr;
		r->rpc_bp1 = mcmsg_validate_write1(addr, payload,
					     kernel_pmap->dirbase);
		r->rpc_bp2 = mcmsg_validate2();
		if ((r->rpc_bp1 == 0) || (r->rpc_bp2 == 0)) {
			panic("rpc_init_engine");
		}
		addr += payload;
		rem -= payload;

		/*
		 *	initialize the fields of the engine.
		 */
		r->rpc_prev = RPC_PAYLOAD_RECEIVED;
		r->rpc_next = RPC_PAYLOAD_RECEIVED;
		r->rpc_route = 0;
		r->rpc_node = 0x0bad;
		r->rpc_class = -1;
		r->rpc_token = 0xffffffff;
		r->rpc_state = RPC_ENGINE_IDLE;
		r->rpc_intr = 0;
		r->rpc_rearm = 0;
		r->rpc_lock_mcp = 0;
		r->rpc_lock_cpu = 0;

	}

	/*
	 *	init class vector
	 */
	if (msgp_rpc_engine_init(nclasses) != RPC_OK)
		return RPC_SHORTAGE;

	rpc_engine_slots = nhandles;

	return RPC_OK;
}


/*
 *	void rpc_engine_send_request(node, class, slot, intr)
 *
 *	PURPOSE
 *
 *	Initiate an RPC request to the class of receivers on the
 *	specified node.
 *
 *	NOTES
 *
 *	If intr is non-zero, the RPC engine will notify with an
 *	interrupt when the corresponding reply arrives.
 *
 *	[ This routine could easily be turned into a macro if needed ]
 *
 */
void rpc_engine_send_request( int node, int class, int slot, int intr )
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];

	assert(r->rpc_state == RPC_ENGINE_IDLE);
	assert(r->rpc_intr == 0);

	r->rpc_state = RPC_ENGINE_BUSY;
	r->rpc_intr = intr;

	mcmsg_post(POST_RPCSENDRQ, node, class, slot, intr);
}


/*
 *	void rpc_engine_recv_request(class, slot, intr)
 *
 *	PURPOSE
 *
 *	Initiate an RPC receive request within the supplied class.
 *
 *	NOTES
 *
 *	If intr is non-zero, the RPC engine will notify with an interrupt
 *	when an RPC request has arrived.
 *
 *	[ This routine could easily be turned into a macro if needed ]
 *
 */
void rpc_engine_recv_request( int class, int slot, int intr )
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];

	assert(r->rpc_state == RPC_ENGINE_IDLE);
	assert(r->rpc_intr == 0);

	r->rpc_state = RPC_ENGINE_BUSY;
	r->rpc_intr = intr;

	mcmsg_post(POST_RPCRECVRQ, class, slot, intr);
}


/*
 *	void rpc_engine_send_reply(slot, intr)
 *
 *	PURPOSE
 *
 *	Initiate an RPC reply.
 *
 *	NOTES
 *
 *	If intr is non-zero, the RPC engine will notify with an interrupt
 *	when the RPC reply has departed.
 *
 *	[ This routine could easily be turned into a macro if needed ]
 *
 */
void rpc_engine_send_reply( int slot, int intr )
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];

	assert(r->rpc_state == RPC_ENGINE_IDLE);
	assert(r->rpc_intr == 0);

	r->rpc_state = RPC_ENGINE_BUSY;
	r->rpc_intr = intr;

	mcmsg_post(POST_RPCSENDRP, slot, intr);
}


/*
 *	void rpc_engine_send_reply_recv(class, slot, intr)
 *
 *	PURPOSE
 *
 *	Initiate an RPC reply.  After the reply departs, initiate
 *	an RPC receive request within the supplied class.
 *
 *	NOTES
 *
 *	If intr is non-zero, the RPC engine will notify with an interrupt
 *	when an RPC request has arrived.  It is impossible with this
 *	interface to request a reply departure interrupt.
 *
 *	[ This routine could easily be turned into a macro if needed ]
 *
 */
void rpc_engine_send_reply_recv( int class, int slot, int intr )
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];

	assert(r->rpc_state == RPC_ENGINE_IDLE);
	assert(r->rpc_intr == 0);

	r->rpc_state = RPC_ENGINE_BUSY;
	r->rpc_intr = 0;	/* must be 0 */

	mcmsg_post(POST_RPCREPLYRECV, class, slot, intr);
}


/*
 *	boolean_t rpc_engine_status(slot)
 *
 *	PURPOSE
 *
 *	Sample an RPC engine status (busy, not busy).
 *
 */
boolean_t rpc_engine_status( int slot )
{
	return rpc_engine[slot].rpc_state;
}


/*
 *	boolean_t rpc_engine_set_notify(slot)
 *
 *	PURPOSE
 *
 *	Conditionally enable notification on a slot which did
 *	not ask for notification when posting the operation.
 *
 *	RETURNS
 *
 *	TRUE		if the operation has already finished.
 *	FALSE		if the notification was installed.
 */
unsigned long	mcmsg_rpc_cpu_spins;
unsigned long	mcmsg_rpc_cpu_total;
boolean_t rpc_engine_set_notify( int slot )
{
	register rpc_engine_t	*r = &rpc_engine[slot];
	boolean_t		cc;

	if (r->rpc_state == RPC_ENGINE_IDLE)
		return TRUE;

	mcmsg_rpc_cpu_total++;

	asm("lock");
	r->rpc_lock_cpu = 1;
	asm("unlock");

	if (r->rpc_lock_mcp) {
		/*
		 *	We lost so just wait for the MCP to finish
		 *	then return TRUE.
		 */
		asm("lock");
		r->rpc_lock_cpu = 0;
		asm("unlock");

		while (r->rpc_lock_mcp)
			mcmsg_rpc_cpu_spins++;
		assert(r->rpc_state == RPC_ENGINE_IDLE);
		return TRUE;
	}

	asm("lock");
	r->rpc_intr = 1;
	if (cc = (r->rpc_state == RPC_ENGINE_IDLE))
		r->rpc_intr = 0;

	r->rpc_lock_cpu = 0;
	asm("unlock");

	return cc;
}


/*
 *	void *rpc_engine_alloc_payload(slot, argsize)
 *
 *	PURPOSE
 *
 *	Return a pointer to the payload associated with the
 *	supplied handle.
 *
 *	NOTES
 *
 *	Some implementations might actually allocate memory rather than
 *	indexing into a fixed sized buffer (and also support variable
 *	sized buffers).
 */
void *rpc_engine_alloc_payload( int slot, int argsize )
{
	assert(argsize <= rpc_engine_payload_size);
	return (void *) rpc_engine[slot].rpc_buffer;
}


/*
 *	static void mcmsg_rpc_notify(ringbuf, &out, &in, (*func)())
 *
 *	PURPOSE
 *
 *	Internal common routine to drain items from the notification
 *	ring buffer (shared with the coprocessor) and call a function.
 *
 *	NOTES
 *
 *	This routine is called from interrupt level.
 *
 *	Each item in the ring buffer is a slot number that has requested
 *	some notification; the triggering event has already occured.
 *	The coprocessor writes to one end of the ring buffer, the
 *	other CPU drains from the other (FIFO order).
 *	
 */
unsigned long mcmsg_rpc_notifications_serviced;
static void mcmsg_rpc_notify(
	rpc_slot_t	*ring,
	volatile int	*outp,
	volatile int	*inp,
	void		(*func)(int) )
{
	register int	slot, out;
	rpc_engine_t	*r;

	simple_lock( &rpc_engine_notification_lock );

	while ((out = *outp) != *inp) {

		slot = ring[out++];
		if (out == rpc_engine_slots)
			out = 0;
		*outp = out;

		r = &rpc_engine[slot];
		assert(r->rpc_intr != 0);
		r->rpc_intr = 0;

		(*func)(slot);
		mcmsg_rpc_notifications_serviced++;
	}

	simple_unlock( &rpc_engine_notification_lock );
}


/*
 *	void rpc_engine_request_intr()
 *
 *	PURPOSE
 *
 *	Dispatch RPC request-arrival notifications.
 *
 *	NOTES
 *
 *	This routine is called directly from the interrupt handler
 *	(in our case, the cpu-to-cpu interrupt handler) and at
 *	interrupt level.
 */
void rpc_engine_request_intr()
{
	extern void	rpc_request_arrival( int );

	mcmsg_rpc_notify( rpc_notify_request_ring,
		&rpc_notify_request_out, &rpc_notify_request_in,
		rpc_request_arrival );
}


/*
 *	void rpc_engine_reply_intr()
 *
 *	PURPOSE
 *
 *	Dispatch RPC reply-arrival notifications.
 *
 *	NOTES
 *
 *	This routine is called directly from the interrupt handler
 *	(in our case, the cpu-to-cpu interrupt handler) and at
 *	interrupt level.
 */
void rpc_engine_reply_intr()
{
	extern void	rpc_reply_arrival( int );

	mcmsg_rpc_notify( rpc_notify_reply_ring,
		&rpc_notify_reply_out, &rpc_notify_reply_in,
		rpc_reply_arrival );
}


/*
 *	void rpc_engine_depart_intr()
 *
 *	PURPOSE
 *
 *	Dispatch RPC reply-departure notifications.
 *
 *	NOTES
 *
 *	This routine is called directly from the interrupt handler
 *	(in our case, the cpu-to-cpu interrupt handler) and at
 *	interrupt level.
 */
void rpc_engine_depart_intr()
{
	extern void	rpc_reply_depart( int );

	mcmsg_rpc_notify( rpc_notify_depart_ring,
		&rpc_notify_depart_out, &rpc_notify_depart_in,
		rpc_reply_depart );
}


/*
 *	This curious piece of code is meant to removed
 *	after someone concludes that there is, in fact, a
 *	coherency botch tickled by rpc_engine_set_notify()
 *	and all hardware is fixed...
 *
 *	Gosh, I REALLY hate needing this routine...
 */
int	rpc_verbose_unstick = 0;
int rpc_engine_unstick()
{
	int		slot, unstuck, s;
	rpc_engine_t	*r;

	unstuck = 0;
	r = &rpc_engine[0];
	s = splmsg();
	for (slot = 0; slot < rpc_engine_slots; slot++, r++) {
		/*
		 *	if it's marked busy, then ignore it.
		 */
		if (r->rpc_state == RPC_ENGINE_BUSY)
			continue;

		/*
		 *	if it's idle, and an interrupt request
		 *	is not indicated, ignore it.
		 */
		if (r->rpc_intr == 0)
			continue;

		/*
		 *	sure looks like it's wedged...
		 *	give it a second chance or kick it
		 *	and get it going again.
		 */
		if (++r->rpc_intr == 2)
			continue;

		/*
		 *	it's stuck -- kick it and move on.
		 *
		 *	THE BIG ASSUMPTION HERE IS THAT THIS STUCK
		 *	HANDLE IS WAITING FOR AN rpc_reply_arrival()
		 *	INTERRUPT.
		 */
		if (rpc_verbose_unstick) {
			printf("rpc_engine_unstick: stuck handle %d, rpc_intr=%d\n",
				slot, r->rpc_intr);
		}
		r->rpc_intr = 0;
		rpc_reply_arrival(slot);
		unstuck++;
	}
	splx(s);

	return unstuck;

}


#if	MACH_KDB

/*
 *	Pretty print an interesting RPC engine.
 */
int rpc_print_interesting_engine( int slot )
{
	rpc_engine_t	*r;

	r = &rpc_engine[slot];
	if (r->rpc_node == 65535)
		db_printf(" -- ");
	else
		db_printf("%4d ", r->rpc_node);
	db_printf("%d %d %d %d %08x %08x",
		r->rpc_class,
		r->rpc_state,
		r->rpc_intr,
		r->rpc_rearm,
		r->rpc_prev,
		r->rpc_next);

	return slot;
}


/*
 *	Pretty print an RPC engine.
 */
int rpc_print_engine( int slot )
{
	rpc_engine_t	*r;
	extern int	indent;

	if ((slot < 0) || (slot >= rpc_engine_slots)) {
		iprintf("warning: using ((rpc_engine_t *) 0x%x)\n", slot);
		r = (rpc_engine_t *) slot;
		slot = r - rpc_engine;
	} else {
		r = &rpc_engine[slot];
	}

	iprintf("rpc engine=%d (0x%x) {\n", slot, r);

	indent += 2;
	iprintf("prev=0x%x, next=0x%x,\n",
		r->rpc_prev,
		r->rpc_next);
	iprintf("buffer=0x%x, bp1=0x%x, bp2=0x%x\n",
		r->rpc_buffer,
		r->rpc_bp1,
		r->rpc_bp2);
	iprintf("route=0x%08x, node=%d, class=%d,\n",
		r->rpc_route,
		r->rpc_node,
		r->rpc_class);
	iprintf("token=0x%x, state=%d, intr=%d, rearm=%d\n",
		r->rpc_token,
		r->rpc_state,
		r->rpc_intr,
		r->rpc_rearm);
	indent -= 2;

	iprintf("}\n");

	return (int) slot;
}
#endif	/* MACH_KDB */
