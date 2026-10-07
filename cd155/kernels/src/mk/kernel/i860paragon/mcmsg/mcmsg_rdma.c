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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_rdma.c,v 1.4 1995/03/21 21:03:23 lenb Exp $
 */

/*
 * SSD HISTORY
 * $Log: mcmsg_rdma.c,v $
 * Revision 1.4  1995/03/21  21:03:23  lenb
 *         reduce !BIGPKTS rdma_engine_packet_size to 1728 from 1792 (12737)
 *
 * Revision 1.3  1994/11/18  20:44:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/12  19:20:16  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.6.11  1994/07/06  20:15:44  andyp
 * Prototyped (and left disabled) a new RDMA function rdma_undo()
 * that will be used by the fast-path enqueue.
 *
 * Revision 1.1.6.10  1994/03/18  18:06:01  andyp
 * Added a queue for incoming readys (rather than a single ready count).
 * The single count caused problems under some conditions when two readys
 * of different sizes were recieved prior to the first send (the sender
 * could send too much data).
 *
 * Revision 1.1.6.9  1994/03/02  18:37:44  andyp
 * Big packet work from the mainline.
 *
 * Revision 1.1.6.8  1994/02/18  01:53:41  andyp
 * Added rdma_seqid_t's for two new polling routines.
 *
 * Revision 1.1.6.7  1994/02/14  18:52:56  andyp
 * Clear out the engine status word counters on disconnect.
 *
 * Revision 1.1.6.6  1994/02/11  23:51:35  andyp
 * Added flag to the engine state indicating that it is faulting.
 * Added some extra assertions.
 *
 * Revision 1.1.6.5  1994/02/11  17:42:47  andyp
 * Added a count field to the rdma_fault_info_t structure.  Transmission
 * faults can now be resumed.
 *
 * Revision 1.1.6.4  1994/02/10  00:58:39  andyp
 * Use a generice resume for both send and receive.
 *
 * Revision 1.1.6.3  1994/02/09  19:31:03  andyp
 * Added some extra debugging for the polling routines.  Polls for
 * send and receive completions were wrong (dropped two lines from
 * the protoype).
 *
 * Revision 1.1.6.2  1994/02/07  20:18:15  andyp
 * Rounded out RDMA interfaces and added user-mode bindings for testing.
 *
 * Revision 1.1.6.1  1994/02/04  07:48:22  andyp
 * RPC flow control is working; RDMA still under construction.
 *
 *
 * END SSD HISTORY
 */

#include <mach_kdb.h>
#include <mach/boolean.h>
#include <mach/machine/vm_types.h>
#include <mach/machine/vm_param.h>
#include <kern/assert.h>
#include <kern/kalloc.h>
#include <kern/lock.h>
#include <vm/vm_map.h>
#include <rpc_rdma/rdma.h>
#include <rpc_rdma/i860paragon/rdma.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>


extern void	mcmsg_post( int, ... );

rdma_engine_t		*rdma_engine;
rdma_engine_status_t	*rdma_engine_status;
int			rdma_engine_slots;

#if	BIGPKTS
int			rdma_engine_packet_size = 8192;
#else	BIGPKTS
int			rdma_engine_packet_size = 1728;
#endif	BIGPKTS


rdma_slot_t	*rdma_notify_send_ring;
int		rdma_notify_send_in, rdma_notify_send_out;

rdma_slot_t	*rdma_notify_recv_ring;
int		rdma_notify_recv_in, rdma_notify_recv_out;

rdma_slot_t	*rdma_notify_send_fault_ring;
int		rdma_notify_send_fault_in, rdma_notify_send_fault_out;

rdma_slot_t	*rdma_notify_recv_fault_ring;
int		rdma_notify_recv_fault_in, rdma_notify_recv_fault_out;

decl_simple_lock_data(, rdma_engine_notification_lock)


mcmsg_rdma_init()
{
	msgp_rdma_init();

	simple_lock_init( &rdma_engine_notification_lock );
}


void rdma_recv_fault_info(
	rdma_handle_t		handle,
	rdma_fault_info_t	*info)
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;

	rdma = &rdma_engine[handle];
	req = &rdma->rdma_recv[rdma->rdma_recv_head];
	assert(req->active);

	info->map = req->map;
	info->addr = req->buf;
	info->count = req->count;
}


void rdma_send_fault_info(
	rdma_handle_t		handle,
	rdma_fault_info_t	*info)
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;

	rdma = &rdma_engine[handle];
	req = &rdma->rdma_send[rdma->rdma_send_head];
	assert(req->active);

	info->map = req->map;
	info->addr = req->buf;
	info->count = req->count;
}


void rdma_resume_send(rdma_handle_t handle)
{
	mcmsg_post(POST_RDMARESUME, handle, 0);
}


void rdma_resume_recv(rdma_handle_t handle)
{
	mcmsg_post(POST_RDMARESUME, handle, 1);
}


rdma_return_t rdma_init_engine(int nhandles, int ngroups)
{
	int		i;
	vm_offset_t	addr;
	vm_size_t	size;

	/*
	 *	allocate and initialize each RDMA engine
	 */
	size = nhandles * sizeof(struct rdma_engine);
	if ((addr = kalloc(size)) == 0) {
		return RDMA_SHORTAGE;
	}
	rdma_engine = (rdma_engine_t *) addr;


	/*
	 *	allocate and initialize the notification
	 *	ring buffers.
	 */
	size = nhandles * sizeof(rdma_slot_t);
	if ((addr = kalloc(size)) == 0)
		return RDMA_SHORTAGE;
	rdma_notify_send_ring = (rdma_slot_t *) addr;
	rdma_notify_send_in = 0;
	rdma_notify_send_out = 0;

	if ((addr = kalloc(size)) == 0)
		return RDMA_SHORTAGE;
	rdma_notify_recv_ring = (rdma_slot_t *) addr;
	rdma_notify_recv_in = 0;
	rdma_notify_recv_out = 0;

	if ((addr = kalloc(size)) == 0)
		return RDMA_SHORTAGE;
	rdma_notify_send_fault_ring = (rdma_slot_t *) addr;
	rdma_notify_send_fault_in = 0;
	rdma_notify_send_fault_out = 0;

	if ((addr = kalloc(size)) == 0)
		return RDMA_SHORTAGE;
	rdma_notify_recv_fault_ring = (rdma_slot_t *) addr;
	rdma_notify_recv_fault_in = 0;
	rdma_notify_recv_fault_out = 0;

	/*
	 *	allocate and initialize the send/recv count
	 *	status vector.
	 */
	size = nhandles * sizeof(struct rdma_engine_status);
	if ((addr = kalloc(size)) == 0)
		return RDMA_SHORTAGE;
	rdma_engine_status = (rdma_engine_status_t *) addr;
	for (i = 0; i < nhandles; i++) {
		rdma_engine_status[i].send_in = 0;
		rdma_engine_status[i].recv_in = 0;
		rdma_engine_status[i].send_out = 0;
		rdma_engine_status[i].recv_out = 0;
	}

	for (i = 0; i < nhandles; i++)
		mcmsg_rdma_disconnect(0, i);

	rdma_engine_slots = nhandles;

	return RDMA_SUCCESS;
}


/*
 *	rdma_token_t rdma_engine_token(slot)
 *
 *	PURPOSE
 *
 *	Construct a token encapsulating node and slot information.
 *
 *	NOTES
 *
 *	This will probably become a macro.
 *
 *	RETURNS
 *
 *	An RDMA token.
 */
rdma_token_t rdma_engine_token( rdma_slot_t slot )
{
	return slot | ipsc_physnode << 16;
}


/*
 *	rdma_node_t rdma_engine_crack_token(token)
 *
 *	PURPOSE
 *
 *	Return the node information contained within the token.
 *
 *	NOTES
 *
 *	This will probably become a macro.
 *
 *	RETURNS
 *
 *	An RDMA node id.
 */
rdma_node_t rdma_engine_crack_token( rdma_token_t token )
{
	return (token >> 16) & 0xffff;
}


/*
 *	void rdma_engine_accept(slot)
 *
 *	PURPOSE
 *
 *	Post a request to the engine that will place "slot" in
 *	the <accepting> state.
 *
 */
void rdma_engine_accept( rdma_slot_t slot )
{
	mcmsg_post(POST_RDMAACCEPT, slot);
}


/*
 *	void rdma_engine_connect(token, slot)
 *
 *	PURPOSE
 *
 *	Post a request to the engine that will attach the RDMA engine
 *	identified by "token" to the local engine identified by "slot."
 *
 *	NOTES
 *
 *	The engine will resolve time-order races between accept/connect
 *	calls -- from the interface level, either can occur "first"
 *	in a global sense.
 */
void rdma_engine_connect( rdma_token_t token, rdma_slot_t slot )
{
	mcmsg_post(POST_RDMACONNECT, token, slot);
}


/*
 *	void rdma_engine_disconnect(slot)
 *
 *	PURPOSE
 *
 *	Post a request to the engine that will disconnect the
 *	local RDMA engine from the remote endpoint.
 *
 *	NOTES
 *
 *	The remote endpoint must also disconnect as it is a
 *	local event.
 */
void rdma_engine_disconnect( rdma_slot_t slot )
{
	rdma_engine_status_t	*stat;

	stat = &rdma_engine_status[slot];

	assert(stat->send_in == stat->send_out);
	assert(stat->recv_in == stat->recv_out);

	stat->send_in = 0;
	stat->send_out = 0;
	stat->recv_in = 0;
	stat->recv_out = 0;

	mcmsg_post(POST_RDMADISCO, slot);
}


/*
 *	int rdma_engine_flush(slot)
 *
 *	PURPOSE
 *
 *	Conditionally drop operations posted prior to
 *	a connection being established.  If a connection
 *	has been established, the connection is not
 *	flushed.
 *
 *	NOTES
 *
 *	Actually, in this implementation, we're relying
 *	on higher-level software to make the guarantee that
 *	no remote connect request is racing towards this slot.
 *
 *	RETURNS
 *
 *	0		if flushed.
 *	1		if not flushed.
 */
int rdma_engine_flush( rdma_slot_t slot )
{
	mcmsg_post(POST_RDMAFLUSH, slot);
	return 0;
}


/*
 *	void rdma_engine_flush_endpoint(token)
 *
 *	PURPOSE
 *
 *	Flush a remote endpoint.  The endpoint will
 *	continue to generate callbacks for posted and
 *	future sends, although no data will be transmitted.
 *
 *	NOTES
 *
 *	Posted and future receives will be ignored.
 *
 */
void rdma_engine_flush_endpoint( rdma_token_t token )
{
	mcmsg_post(POST_RDMAFLUSHREMOTE, token);
}


/*
 *	PURPOSE
 *
 *	Special purpose routine to silently dump all
 *	posted operations.
 *
 *	WARNING
 *
 *	It is an error to call this routine on a
 *	slot that has a connection established.
 */
void rdma_engine_undo( rdma_slot_t slot )
{
	rdma_engine_status_t	*stat;

	stat = &rdma_engine_status[slot];
	stat->send_in = 0;
	stat->send_out = 0;
	stat->recv_in = 0;
	stat->recv_out = 0;
	mcmsg_post(POST_RDMADISCO, slot);
}


/*
 *	rdma_seqid_t rdma_send(slot, buf, count, notify, map)
 *
 *	PURPOSE
 *
 *	Post a send operation.
 */
rdma_seqid_t rdma_engine_send(
	rdma_slot_t	slot,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	rdma_seqid_t	seq;

	assert(map != 0);

	seq = ++rdma_engine_status[slot].send_in;
	mcmsg_post(POST_RDMASEND, slot, buf, count, notify, map);
	return seq;
}


/*
 *	rdma_seqid_t rdma_recv(slot, buf, count, notify, map)
 *
 *	PURPOSE
 *
 *	Post a receive operation.
 */
rdma_seqid_t rdma_engine_recv(
	rdma_slot_t	slot,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	rdma_seqid_t	seq;

	assert(map != 0);

	seq = ++rdma_engine_status[slot].recv_in;
	mcmsg_post(POST_RDMARECV, slot, buf, count, notify, map);
	return seq;
}


mcmsg_rdma_notify(
	rdma_slot_t	*ring,
	int		*outp,
	int		*inp,
	void		(*func)(int) )
{
	register int	slot, out;

	simple_lock( &rdma_engine_notification_lock );

	while ((out = *outp) != *inp) {
		slot = ring[out++];
		if (out == rdma_engine_slots)
			out = 0;
		*outp = out;
		(*func)(slot);
        }

	simple_unlock( &rdma_engine_notification_lock );
}


rdma_engine_send_intr()
{
	extern void	rdma_send_intr( int );

	mcmsg_rdma_notify( rdma_notify_send_ring,
		&rdma_notify_send_out, &rdma_notify_send_in,
		rdma_send_intr );
}


rdma_engine_recv_intr()
{
	extern void	rdma_recv_intr( int );

	mcmsg_rdma_notify( rdma_notify_recv_ring,
		&rdma_notify_recv_out, &rdma_notify_recv_in,
		rdma_recv_intr );
}


rdma_engine_send_fault_intr()
{
	extern void	rdma_send_fault_intr( int );

	mcmsg_rdma_notify( rdma_notify_send_fault_ring,
		&rdma_notify_send_fault_out, &rdma_notify_send_fault_in,
		rdma_send_fault_intr );
}


rdma_engine_recv_fault_intr()
{
	extern void	rdma_recv_fault_intr( int );

	mcmsg_rdma_notify( rdma_notify_recv_fault_ring,
		&rdma_notify_recv_fault_out, &rdma_notify_recv_fault_in,
		rdma_recv_fault_intr );
}


boolean_t rdma_engine_send_busy(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return ((s->send_in - s->send_out) >= RDMA_MAXREQ);
}


boolean_t rdma_engine_send_ready(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return ((s->send_in - s->send_out) < RDMA_MAXREQ);
}


boolean_t rdma_engine_send_done(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return (s->send_in == s->send_out);
}


rdma_seqid_t rdma_engine_send_complete(rdma_slot_t slot)
{
	return rdma_engine_status[slot].send_out;
}


boolean_t rdma_engine_recv_busy(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return ((s->recv_in - s->recv_out) >= RDMA_MAXREQ);
}


boolean_t rdma_engine_recv_ready(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return ((s->recv_in - s->recv_out) < RDMA_MAXREQ);
}


boolean_t rdma_engine_recv_done(rdma_slot_t slot)
{
	rdma_engine_status_t	*s;

	s = &rdma_engine_status[slot];
	return (s->recv_in == s->recv_out);
}


rdma_seqid_t rdma_engine_recv_complete(rdma_slot_t slot)
{
	return rdma_engine_status[slot].recv_out;
}


#if	MACH_KDB

/*
 *	Pretty-print an RDMA request.
 */
rdma_print_engine_request(rdma_engine_req_t *req)
{
	iprintf("dir=0x%x buf=0x%x cnt=%d s=%d a=%d n=%d map=0x%x\n",
		req->dirbase,
		req->buf,
		req->count,
		req->sent,
		req->active,
		req->notify,
		req->map);
}


static char *rdma_engine_state_human( unsigned char state )
{
	switch (state) {
	case RDMA_STATE_DISCO:
		return "disco";

	case RDMA_STATE_WAIT:
		return "wait";

	case RDMA_STATE_ACCEPT:
		return "accept";

	case RDMA_STATE_CONNECT:
		return "connect";

	case RDMA_STATE_READY:
		return "ready";

	case RDMA_STATE_STOP:
		return "stop";

	case RDMA_STATE_FLUSH:
		return "flush";
	}
	return "fobbed";
}


/*
 *	Pretty-print an RDMA engine.
 */
rdma_print_engine( int slot )
{
	rdma_engine_t	*rdma;
	int		h, t, count;
	extern int	indent;

	if ((slot < 0) || (slot >= rdma_engine_slots)) {
		iprintf("warning: using ((rdma_engine_t *) 0x%x)\n", slot);
		rdma = (rdma_engine_t *) slot;
		slot = rdma - rdma_engine;
	} else {
		rdma = &rdma_engine[slot];
	}

	iprintf("rdma engine=%d (0x%x) {\n", slot, rdma);

	indent += 2;
	iprintf("node=%d, rem slot=%d, route=0x%08x,\n",
		rdma->rdma_node,
		rdma->rdma_slot,
		rdma->rdma_route);
	iprintf("state=0x%x [%s], faulting=%d, sending=%d,\n",
		rdma->rdma_state, rdma_engine_state_human(rdma->rdma_state),
		rdma->rdma_faulting,
		rdma->rdma_sending);
	iprintf("send_in=%d, send_out=%d,\n",
		rdma_engine_status[slot].send_in,
		rdma_engine_status[slot].send_out);
	iprintf("recv_in=%d, recv_out=%d,\n",
		rdma_engine_status[slot].recv_in,
		rdma_engine_status[slot].recv_out);

	iprintf("ready_head=%d, ready_tail=%d {\n",
		rdma->rdma_ready_head,
		rdma->rdma_ready_tail);
	indent += 2;
	count = 0;
	h = rdma->rdma_ready_head;
	t = rdma->rdma_ready_tail;
	while (h != t) {
		iprintf("ready=%d (0x%x)\n", rdma->rdma_ready[h]);
		if (++h == RDMA_MAXREQ)
			h = 0;
		count++;
	}
	indent -= 2;
	iprintf("} /* count=%d */,\n", count);

	iprintf("send_head=%d, send_tail=%d {\n",
		rdma->rdma_send_head,
		rdma->rdma_send_tail);
	indent += 2;
	count = 0;
	h = rdma->rdma_send_head;
	t = rdma->rdma_send_tail;
	while (h != t) {
		rdma_print_engine_request(&rdma->rdma_send[h]);
		if (++h == RDMA_MAXREQ)
			h = 0;
		count++;
	}
	indent -= 2;
	iprintf("} /* count=%d */,\n", count);

	iprintf("recv_head=%d, recv_tail=%d {\n",
		rdma->rdma_recv_head,
		rdma->rdma_recv_tail);
	indent += 2;
	count = 0;
	h = rdma->rdma_recv_head;
	t = rdma->rdma_recv_tail;
	while (h != t) {
		rdma_print_engine_request(&rdma->rdma_recv[h]);
		if (++h == RDMA_MAXREQ)
			h = 0;
		count++;
	}
	indent -= 2;
	iprintf("} /* count=%d */\n", count);

	indent -= 2;

	iprintf("}\n");
	return (int) slot;
}

#endif	/* MACH_KDB */


