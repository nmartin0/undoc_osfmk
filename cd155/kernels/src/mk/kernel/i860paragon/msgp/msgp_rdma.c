/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
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
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_rdma.c,v 1.6 1994/11/18 20:47:44 mtm Exp $
 */

/*
 * msgp_rdma.c
 *
 * RDMA message module
 */

#define	MCMSG_MODULE	MCMSG_MODULE_RDMA

#include <mach_kdb.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/msgp/msgp_hw.h>
#include <i860paragon/mcmsg/mcmsg_rdma.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <rpc_rdma/rdma.h>
#include <rpc_rdma/i860paragon/rdma.h>


#define MSGP_RDMA_DEBUG	1
#if	MSGP_RDMA_DEBUG
#define RDMA_DEBUG(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	/* MSGP_RDMA_DEBUG */
#define RDMA_DEBUG(a,b,c,d,e,f)
#endif	/* MSGP_RDMA_DEBUG */


#if	defined(BIGPKTS) && defined(BUMPERS)
/*
 *	+bigpkts+bumpers has rules about the size of message headers
 *	that are followed by data.
 */

#define	RDMA_SEND_HEADER_PAD()	\
	{ send2_now(0,0); send2_now(0,0); send2_now(0,0); send2_now(0,0); }

#define	RDMA_RECV_HEADER_PAD()	\
	{ recv2dummy(); recv2dummy(); recv2dummy(); recv2dummy(); }

#else	/* defined(BIGPKTS) && defined(BUMPERS) */

/*
 *	don't need the bumper padding for B-step NIC.
 */
#define	RDMA_SEND_HEADER_PAD()
#define	RDMA_RECV_HEADER_PAD()

#endif	/* defined(BIGPKTS) && defined(BUMPERS) */



mcmsg_rdma_destroy_task(mt)
{
	RDMA_DEBUG("mcsmg_rdma_destroy_task", 1, mt, 0, 0, 0);
}


msgp_rdma_init()
{
	RDMA_DEBUG("msgp_rdma_init", 0, 0, 0, 0, 0);
}


mcmsg_rdma_resume( int mt, rdma_slot_t slot, int which )
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;

	RDMA_DEBUG("mcmsg_rdma_resume", 2, slot, which, 0, 0);

	rdma = &rdma_engine[slot];

	if (which == 0) {
		req = &rdma->rdma_send[rdma->rdma_send_head];

		assert(req->active);
		assert(rdma->rdma_sending == 1);
		assert(rdma->rdma_ready_head != rdma->rdma_ready_tail);

		rdma->rdma_faulting &= ~(1 << 0);

		mcmsg_send_tail(0, MCTRL_RDMAD, rdma, req,
			&rdma_engine_status[slot]);

	} else {
		RDMA_DEBUG("rdma resume recv?", 2, slot, which, 0, 0);
	}

}


static msgp_rdma_ready_store( rdma_engine_t *rdma, unsigned long count)
{
	int	next;

	rdma->rdma_ready[(next = rdma->rdma_ready_tail)] = count;
	if (++next == RDMA_MAXREQ)
		next = 0;
	rdma->rdma_ready_tail = next;
}


static void msgp_rdma_send_readys(rdma_engine_t *rdma)
{
	rdma_engine_req_t	*req;
	int	h;

	h = rdma->rdma_recv_head;
	for (;;) {
		req = &rdma->rdma_recv[h];
		if (req->active == 0)
			return;
		if (++h == RDMA_MAXREQ)
			h = 0;
		assert(req->sent == 0);
		req->sent = 1;
		if (req->count > 0)
			mcmsg_send(0, MCTRL_RDMAR, rdma, req->count);
	}
}


mcmsg_rdma_accept( int mt, rdma_slot_t slot )
{
	rdma_engine_t	*rdma;

	RDMA_DEBUG("rdma accept", 1, slot, 0, 0, 0);

	rdma = &rdma_engine[slot];

	if (rdma->rdma_state == RDMA_STATE_DISCO) {
		RDMA_DEBUG("rdma accept -- accepting", 0, 0, 0, 0, 0);
		rdma->rdma_state = RDMA_STATE_ACCEPT;
		rdma->rdma_node = 0xffffffff;
		rdma->rdma_route = 0;
		rdma->rdma_slot = ~0;
	} else {
		assert(rdma->rdma_state == RDMA_STATE_WAIT);
		RDMA_DEBUG("rdma accept !disco", 0, 0, 0, 0, 0);
		rdma->rdma_state = RDMA_STATE_READY;
		mcmsg_send(mt, MCTRL_RDMAG, rdma);
		msgp_rdma_send_readys(rdma);
	}
}


mcmsg_rdma_connect( int mt, rdma_token_t token, rdma_slot_t slot )
{
	rdma_engine_t	*rdma;
	unsigned long	dest_node;
	unsigned long	dest_slot;

	RDMA_DEBUG("rdma connect", 2, token, slot, 0, 0);

	dest_node = (token >> 16) & 0xffff;
	dest_slot = token & 0xffff;

	rdma = &rdma_engine[slot];
	assert(rdma->rdma_state == RDMA_STATE_DISCO);

	rdma->rdma_state = RDMA_STATE_CONNECT;
	rdma->rdma_slot = dest_slot;
	rdma->rdma_node = dest_node;
	rdma->rdma_route = calculate_route(dest_node);

	rdma_engine_status[slot].send_out = 0;
	rdma_engine_status[slot].recv_out = 0;

	mcmsg_send_tail(mt, MCTRL_RDMAC, rdma);
}


mcmsg_rdma_disconnect( int mt, rdma_slot_t slot )
{
	rdma_engine_t	*rdma;
	int		i;

	RDMA_DEBUG("rdma disconnect", 1, slot, 0, 0, 0);

	rdma = &rdma_engine[slot];

	rdma->rdma_state = RDMA_STATE_DISCO;
	rdma->rdma_sending = 0;
	rdma->rdma_faulting = 0;

	rdma->rdma_node = 0xffffffff;
	rdma->rdma_route = 0;
	rdma->rdma_slot = ~0;

	rdma->rdma_send_head = 0;
	rdma->rdma_send_tail = 0;
	rdma->rdma_recv_head = 0;
	rdma->rdma_recv_tail = 0;
	rdma->rdma_ready_head = 0;
	rdma->rdma_ready_tail = 0;

	for (i = 0; i < RDMA_MAXREQ; i++) {
		rdma->rdma_send[i].active = 0;
		rdma->rdma_recv[i].active = 0;
		rdma->rdma_recv[i].sent = 0;
		rdma->rdma_ready[i] = 0;
	}
}


static void msgp_rdma_notify(
	rdma_slot_t	*ring,
	rdma_slot_t	slot,
	int		*inp,
	void		(*func)() )
{
	register int	in;

	RDMA_DEBUG("msgp rdma notify", 4, ring, slot, inp, func);

	ring[(in = *inp)] = slot;
	if (++in == rdma_engine_slots)
		in = 0;
	*inp = in;
	(*func)();
}


static rdma_engine_req_t *msgp_rdma_retire_send(
	rdma_engine_t		*rdma,
	rdma_engine_req_t	*req,
	rdma_engine_status_t	*stat)
{
	int		next;
	extern void	mp_rdma_engine_send_intr();

	RDMA_DEBUG("msgp rdma retire send", 4, rdma-rdma_engine,
		req->count, rdma->rdma_send_head, rdma->rdma_send_tail);

	if (req->active == 0)
		return req;

	req->active = 0;
	stat->send_out++;

	if ((next = rdma->rdma_send_head + 1) == RDMA_MAXREQ)
		next = 0;
	rdma->rdma_send_head = next;

	if (req->notify) {
		msgp_rdma_notify(rdma_notify_send_ring, rdma-rdma_engine,
			&rdma_notify_send_in, mp_rdma_engine_send_intr);
	}

	return &rdma->rdma_send[next];
}


static rdma_engine_req_t *msgp_rdma_retire_recv(
	rdma_engine_t		*rdma,
	rdma_engine_req_t	*req,
	rdma_engine_status_t	*stat)
{
	int		next;
	extern void	mp_rdma_engine_recv_intr();

	RDMA_DEBUG("msgp rdma retire recv", 4, rdma-rdma_engine,
		req, rdma->rdma_recv_head, rdma->rdma_recv_tail);

	if (req->active == 0)
		return req;

	req->active = 0;
	stat->recv_out++;

	if ((next = rdma->rdma_recv_head + 1) == RDMA_MAXREQ)
		next = 0;
	rdma->rdma_recv_head = next;

	if (req->notify) {
		msgp_rdma_notify(rdma_notify_recv_ring, rdma-rdma_engine,
			&rdma_notify_recv_in, mp_rdma_engine_recv_intr);
	}

	return &rdma->rdma_recv[next];
}


static void mcmsg_rdma_flush_slot(rdma_slot_t slot)
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;
	rdma_engine_status_t	*stat;

	RDMA_DEBUG("msgp rdma flush slot", 1, slot, 0, 0, 0);

	rdma = &rdma_engine[slot];

	if ((rdma->rdma_state == RDMA_STATE_DISCO) ||
	    (rdma->rdma_state == RDMA_STATE_FLUSH))
		return;

	stat = &rdma_engine_status[slot];
	req = &rdma->rdma_send[rdma->rdma_send_head];

	while (req->active)
		req = msgp_rdma_retire_send(rdma, req, stat);

	rdma->rdma_state = RDMA_STATE_FLUSH;
}


mcmsg_rdma_flush( int mt, rdma_slot_t slot )
{
	RDMA_DEBUG("msgp rdma flush", 1, slot, 0, 0, 0);
	mcmsg_rdma_flush_slot(slot);
}


mcmsg_rdma_flush_remote( int mt, rdma_token_t token )
{
	int	node, dest_slot;

	node = (token >> 16) & 0xffff;
	dest_slot = token & 0xffff;
	mcmsg_send_tail(mt, MCTRL_RDMAF, node, dest_slot);
}


mcmsg_rdma_recv(
	int		mt,
	rdma_slot_t	slot,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;
	int			next;

	RDMA_DEBUG("mcmsg_rdma_recv", 4, slot, buf, count, notify);

	rdma = &rdma_engine[slot];

	/*
	 *	if flushing, ignore the request
	 */
	if (rdma->rdma_state == RDMA_STATE_FLUSH) {
		RDMA_DEBUG("mcmsg_rdma_recv -- flushing", 0, 0, 0, 0, 0);
		return;
	}

	/*
	 *	allocate the next available request slot;
	 *	check for an overrun (and drop the request).
	 */
	req = &rdma->rdma_recv[(next = rdma->rdma_recv_tail)];
	if (req->active) {
		mcmsg_trace_drop("rdma recv request overrun", slot);
		mp_rdma_panic();
		return;
	}
	if (++next == RDMA_MAXREQ)
		next = 0;
	rdma->rdma_recv_tail = next;


	/*
	 *	record the parameters of the receive request.
	 */
	req->map = map;
	req->dirbase = map->pmap->dirbase;
	req->buf = buf;
	req->count = count;
	req->notify = notify;
	req->active = 1;

	/*
	 *	if a zero-length receive and it's at the front
	 *	of the request queue, retire it immediately.
	 */
	if ((count == 0) && (req == &rdma->rdma_recv[rdma->rdma_recv_head])) {
		RDMA_DEBUG("mcmsg_rdma_recv -- recv 0",
			4, rdma->rdma_recv_head, rdma->rdma_recv_tail,
			&rdma->rdma_recv[rdma->rdma_recv_head], req);
		req = msgp_rdma_retire_recv(rdma, req,
				&rdma_engine_status[slot]);
		assert(!req->active);
		return;
	}


	/*
	 *	if not ready, just walk away.
	 */
	if (rdma->rdma_state != RDMA_STATE_READY) {
		req->sent = 0;
		RDMA_DEBUG("mcmsg_rdma_recv -- not ready", 3,
			count, rdma->rdma_ready_head, rdma->rdma_ready_tail, 0);
		return;
	}

	/*
	 *	give the sender the receiver's buffer count.
	 */
	if (count > 0) {
		req->sent = 1;
		RDMA_DEBUG("rdma recv -- rdmar", 2, slot, count, 0, 0);
		mcmsg_send_tail(mt, MCTRL_RDMAR, rdma, count);
	}
}


mcmsg_rdma_send(
	int		mt,
	rdma_slot_t	slot,
	vm_offset_t	buf,
	vm_size_t	count,
	boolean_t	notify,
	vm_map_t	map)
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;
	int			next;

	RDMA_DEBUG("mcmsg_rdma_send", 4, slot, buf, count, notify);

	rdma = &rdma_engine[slot];

	/*
	 *	allocate the next available request slot;
	 *	check for an overrun (and drop the request).
	 */
	req = &rdma->rdma_send[(next = rdma->rdma_send_tail)];
	if (req->active) {
		mcmsg_trace_drop("rdma send request overrun", slot);
		mp_rdma_panic();
		return;
	}
	if (++next == RDMA_MAXREQ)
		next = 0;
	rdma->rdma_send_tail = next;


	/*
	 *	record the parameters of the send request.
	 */
	req->map = map;
	req->dirbase = map->pmap->dirbase;
	req->buf = buf;
	req->count = count;
	req->notify = notify;
	req->active = 1;

	/*
	 *	if a zero-length send and it's at the front
	 *	of the request queue, retire it immediately.
	 */
	if ((count == 0) && (req == &rdma->rdma_send[rdma->rdma_send_head])) {
		RDMA_DEBUG("mcmsg_rdma_send -- send 0",
			4, rdma->rdma_send_head, rdma->rdma_send_tail,
			&rdma->rdma_send[rdma->rdma_send_head], req);
		req = msgp_rdma_retire_send(rdma, req,
				&rdma_engine_status[slot]);
		assert(!req->active);
		return;
	}

	/*
	 *	if flushing, retire the request and generate
	 *	a callback if requested.
	 *
	 *	if not ready, then don't send.
	 *
	 *	if already sending, then don't send.
	 *
	 *	if receiver has no receive posted, then don't send.
	 */
	if (rdma->rdma_state == RDMA_STATE_FLUSH) {
		RDMA_DEBUG("mcmsg_rdma_send -- flushing",
				4, slot, buf, count, notify);
		req = msgp_rdma_retire_send(rdma, req,
				&rdma_engine_status[slot]);
		assert(!req->active);
		return;
	}
	if (rdma->rdma_state != RDMA_STATE_READY) {
		assert(rdma->rdma_send_head == 0);
		RDMA_DEBUG("mcmsg_rdma_send -- not ready", 0, 0, 0, 0, 0);
		return;
	}
	if (rdma->rdma_sending) {
		RDMA_DEBUG("mcmsg_rdma_send -- already sending", 0, 0, 0, 0, 0);
		return;
	}
	if (rdma->rdma_ready_head == rdma->rdma_ready_tail) {
		RDMA_DEBUG("mcmsg_rdma_send -- no recv", 0, 0, 0, 0, 0);
		return;
	}

	/*
	 *	start pumping data.
	 */
	rdma->rdma_sending = 1;
	mcmsg_send_tail(mt, MCTRL_RDMAD, rdma,
			&rdma->rdma_send[rdma->rdma_send_head],
			&rdma_engine_status[slot]);
}


mcmsg_send_rdmac(int mt, int ctl, rdma_engine_t *rdma)
{
	unsigned long	hdr1, hdr2;

	hdr1 = MCTRL_RDMAC | (rdma->rdma_slot << 16);
	hdr2 = rdma - rdma_engine | (ipsc_physnode << 16);
	mcmsg_trace_send(hdr1, hdr2, rdma->rdma_node, 2, rdma - rdma_engine, 0);
	send2_now(rdma->rdma_route, 0);
	send2eod_now(hdr1, hdr2);
}


unsigned long	msgp_connect_won_race;

mcmsg_recv_rdmac( unsigned long hdr1, unsigned long hdr2 )
{
	rdma_engine_t	*rdma;
	int		slot, from;

	slot = hdr1 >> 16;
	from = hdr2 >> 16;
	rdma = &rdma_engine[slot];
	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 2, slot, from);

	rdma->rdma_slot = hdr2 & 0xffff;
	rdma->rdma_node = from;
	rdma->rdma_route = calculate_route(from);
	if (rdma->rdma_state == RDMA_STATE_DISCO) {
		msgp_connect_won_race++;
		rdma->rdma_state = RDMA_STATE_WAIT;
	} else if (rdma->rdma_state == RDMA_STATE_ACCEPT) {
		rdma->rdma_state = RDMA_STATE_READY;
		assert(rdma->rdma_send_head == 0);
		mcmsg_send(0, MCTRL_RDMAG, rdma);
		msgp_rdma_send_readys(rdma);
	} else {
		mcmsg_trace_drop("Connect wrong state", rdma->rdma_state);
	}
}


mcmsg_rdma_send_fault(rdma_slot_t slot)
{
	extern void	mp_rdma_engine_send_fault_intr();

	RDMA_DEBUG("msgp rdma send fault",
		2, slot, rdma_engine[slot].rdma_send_head, 0, 0);
	rdma_engine[slot].rdma_faulting |= (1 << 0);
	msgp_rdma_notify(rdma_notify_send_fault_ring, slot,
		&rdma_notify_send_fault_in, mp_rdma_engine_send_fault_intr);
}


mcmsg_rdma_recv_fault(rdma_slot_t slot)
{
	extern void	mp_rdma_engine_recv_fault_intr();

	RDMA_DEBUG("msgp rdma recv fault",
		2, slot, rdma_engine[slot].rdma_recv_head, 0, 0);
	rdma_engine[slot].rdma_faulting |= (1 << 1);
	msgp_rdma_notify(rdma_notify_recv_fault_ring, slot,
		&rdma_notify_recv_fault_in, mp_rdma_engine_recv_fault_intr);
}


mcmsg_send_rdmad_finish(
	int			mt,
	int			ctl,
	rdma_engine_t		*rdma,
	rdma_engine_req_t	*req,
	rdma_engine_status_t	*stat)
{
	unsigned long	count, bp1, bp2, ready, *readyp;
	int		h;

	h = rdma->rdma_ready_head;
	ready = *(readyp = &rdma->rdma_ready[h]);

	if ((count = req->count) > 0) {

		bp1 = mcmsg_validate_read1(req->buf, count, req->dirbase);
		bp2 = mcmsg_validate2();
		if ((bp1 == 0) || (bp2 == 0)) {
			RDMA_DEBUG("rdmad finish", 4,
				req->buf, count, req->dirbase, bp2);
			mcmsg_rdma_send_fault(rdma - rdma_engine);
			return;
		}

		mcmsg_trace_send(
			MCTRL_RDMAD | (rdma->rdma_slot << 16),
			count | (ipsc_physnode << 16),
			rdma->rdma_node, 2, rdma-rdma_engine, req);
#if	BIGPKTS
		send2_now(rdma->rdma_route, 0);
		send2_now(MCTRL_RDMAD | (rdma->rdma_slot << 16),
			 count | (ipsc_physnode << 16));
		RDMA_SEND_HEADER_PAD();
		mcmsg_send_buf(bp1, bp2, count);
#else	BIGPKTS
		mcmsg_send_pkt2(mt, 0,
			bp1,
			bp2,
			count,
			rdma->rdma_route,
			MCTRL_RDMAD | (rdma->rdma_slot << 16),
			count | (ipsc_physnode << 16));
#endif	BIGPKTS
		if ((*readyp -= count) == 0) {
			if (++h == RDMA_MAXREQ)
				h = 0;
			rdma->rdma_ready_head = h;
		}
	}

	req = msgp_rdma_retire_send(rdma, req, stat);
	if (req->active) {
		mcmsg_send_tail(mt, MCTRL_RDMAD, rdma, req, stat);
		return;
	}

	rdma->rdma_sending = 0;
}


mcmsg_send_rdmad(
	int			mt,
	int			ctl,
	rdma_engine_t		*rdma,
	rdma_engine_req_t	*req,
	rdma_engine_status_t	*stat)
{
	unsigned long	count, bp1, bp2, ready, *readyp;
	int		h;

	/*
	 *	if there is no send posted, quit sending.
	 */
	if (req->active == 0) {
		rdma->rdma_sending = 0;
		return;
	}

	/*
	 *	if the current send request is zero-length,
	 *	retire it, and move to the next request.
	 */
	if (req->count == 0) {
		req = msgp_rdma_retire_send(rdma, req, stat);
		mcmsg_send_tail(mt, MCTRL_RDMAD, rdma, req, stat);
		return;
	}

	/*
	 *	if there are no receives posted from the receiver,
	 *	quit sending.
	 */
	if ((h = rdma->rdma_ready_head) == rdma->rdma_ready_tail) {
		rdma->rdma_sending = 0;
		return;
	}

	/*
	 *	compute the max that the receiver is willing
	 *	to receive -- it's the smaller of the packet
	 *	size and the receiver's ready count.
	 */
	ready = *(readyp = &rdma->rdma_ready[h]);
	if ((count = rdma_engine_packet_size) > ready)
		count = ready;

	/*
	 *	if the number of bytes that the receiver can
	 *	receive is less than the number of bytes posted
	 *	for a send, send as much as the receiver can take.
	 */
	if (count < req->count) {

		assert(count > 0);
		bp1 = mcmsg_validate_read1(req->buf, count, req->dirbase);
		bp2 = mcmsg_validate2();
		if ((bp1 == 0) || (bp2 == 0)) {
			RDMA_DEBUG("rdmad", 4,
				req->buf, count, req->dirbase, bp2);
			mcmsg_rdma_send_fault(rdma - rdma_engine);
			return;
		}

		mcmsg_trace_send(
			MCTRL_RDMAD | (rdma->rdma_slot << 16),
			count | (ipsc_physnode << 16),
			rdma->rdma_node, 2, rdma - rdma_engine, req);
#if	BIGPKTS
		send2_now(rdma->rdma_route, 0);
		send2_now(MCTRL_RDMAD | (rdma->rdma_slot << 16),
			 count | (ipsc_physnode << 16));
		RDMA_SEND_HEADER_PAD();
		mcmsg_send_buf(bp1, bp2, count);
#else	BIGPKTS
		mcmsg_send_pkt2(mt, 0,
			bp1,
			bp2,
			count,
			rdma->rdma_route,
			MCTRL_RDMAD | (rdma->rdma_slot << 16),
			count | (ipsc_physnode << 16));
#endif	BIGPKTS
		req->buf += count;
		req->count -= count;

		/*
		 *	if the receiver's ready count has expired,
		 *	retire it and advance to the next.
		 *
		 *	if there are no more readys, quit sending.
		 *	otherwise, update the ready count.
		 */
		if ((ready -= count) == 0) {
			if (++h == RDMA_MAXREQ)
				h = 0;
			rdma->rdma_ready_head = h;
			if (h == rdma->rdma_ready_tail) {
				rdma->rdma_sending = 0;
				return;
			}
			ready = *(readyp = &rdma->rdma_ready[h]);
		}

		/*
		 *	if the receiver can take more, keep sending.
		 */
		if ((*readyp = ready) > 0) {
			mcmsg_send_tail(mt, MCTRL_RDMAD, rdma, req, stat);
			return;
		}
	}

	/*
	 *	if the receiver can take more, and this send will expire,
	 *	call the finish routine that will generate notifications.
	 */
	if (ready > 0) {
		mcmsg_send_rdmad_finish(mt, MCTRL_RDMAD, rdma, req, stat);
		return;
	}

	rdma->rdma_sending = 0;
}


mcmsg_recv_rdmad( unsigned long hdr1, unsigned long hdr2 )
{
	register unsigned long		count, n, bp1, bp2;
	register rdma_engine_req_t	*req;
	rdma_engine_t		*rdma;
	int			slot;

#if	BIGPKTS
	RDMA_RECV_HEADER_PAD();
#endif	BIGPKTS

	slot = hdr1 >> 16;
	count = hdr2 & 0xffff;
	rdma = &rdma_engine[slot];
	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16,
		2, hdr1 >> 16, rdma->rdma_recv_head);
	assert(count > 0);
	assert(rdma->rdma_state & RDMA_CONNECTED);

	req = &rdma->rdma_recv[rdma->rdma_recv_head];
	do {
		assert(req->active);

		if (req->count > count) {
			assert(count > 0);
			bp1 = mcmsg_validate_write1(req->buf, count, req->dirbase);
			bp2 = mcmsg_validate2();
			assert(bp1 != 0 && bp2 != 0);
#if	BIGPKTS
			mcmsg_recv_buf(bp1, bp2, count);
#else	BIGPKTS
			mcmsg_recv_buf_even(bp1, bp2, count);
#endif	BIGPKTS
			req->count -= count;
			req->buf += count;
			return;
		}

		if (req->count > 0) {
			n = req->count;
			bp1 = mcmsg_validate_write1(req->buf, n, req->dirbase);
			bp2 = mcmsg_validate2();
			assert(bp1 != 0 && bp2 != 0);
#if	BIGPKTS
			mcmsg_recv_buf(bp1, bp2, n);
#else	BIGPKTS
			mcmsg_recv_buf_even(bp1, bp2, n);
#endif	BIGPKTS
			req->count = 0;
			count -= n;
		}

		do {
			req = msgp_rdma_retire_recv(rdma, req,
					&rdma_engine_status[slot]);
		} while ((req->active) && (req->count == 0));

	} while (count > 0);

}


mcmsg_send_rdmag(int mt, int ctl, rdma_engine_t	*rdma)
{
	mcmsg_trace_send(MCTRL_RDMAG | (rdma->rdma_slot << 16), 0,
		rdma->rdma_node, 2, rdma - rdma_engine, rdma->rdma_slot);
	send2_now(rdma->rdma_route, 0);
	send2eod_now(MCTRL_RDMAG | (rdma->rdma_slot << 16), 0);
}


mcmsg_recv_rdmag( unsigned long hdr1, unsigned long unused )
{
	rdma_engine_t	*rdma;
	int		slot;
	int		h, t;

	slot = hdr1 >> 16;
	rdma = &rdma_engine[slot];
	mcmsg_trace_recv(hdr1, unused, rdma->rdma_node, 2, slot, unused);

	rdma->rdma_state = RDMA_STATE_READY;

	msgp_rdma_send_readys(rdma);
}


mcmsg_send_rdmar(int mt, int ctl, rdma_engine_t *rdma, unsigned long count)
{
	assert(count > 0);

	mcmsg_trace_send(MCTRL_RDMAR | (rdma->rdma_slot << 16), count,
		rdma->rdma_node, 2, rdma - rdma_engine, count);
	send2_now(rdma->rdma_route, 0);
	send2eod_now(MCTRL_RDMAR | (rdma->rdma_slot << 16), count);
}


unsigned long msgp_rdma_recv_rdmar_total;
unsigned long msgp_rdma_recv_rdmar_hits;

mcmsg_recv_rdmar( unsigned long hdr1, unsigned long count )
{
	rdma_engine_t		*rdma;
	rdma_engine_req_t	*req;
	int			slot;

	slot = hdr1 >> 16;
	rdma = &rdma_engine[slot];
	mcmsg_trace_recv(hdr1, count, rdma->rdma_node, 2, slot, count);

	assert(rdma->rdma_state & RDMA_CONNECTED);
	assert(count > 0);

	msgp_rdma_ready_store(rdma, count);

	msgp_rdma_recv_rdmar_total++;
	if (rdma->rdma_sending) {
		msgp_rdma_recv_rdmar_hits++;
		return;
	}

	req = &rdma->rdma_send[rdma->rdma_send_head];
	if (req->active) {
		rdma->rdma_sending = 1;
		mcmsg_send_tail(0, MCTRL_RDMAD,
			rdma, req, &rdma_engine_status[slot]);
	}
}


mcmsg_send_rdmaf(int mt, int ctl, unsigned long node, int slot)
{
	unsigned long	hdr1;

	hdr1 = MCTRL_RDMAF | (slot << 16);

	mcmsg_trace_send(hdr1, ipsc_physnode, node, 2, hdr1, slot);
	send2_now(calculate_route(node), 0);
	send2eod_now(hdr1, ipsc_physnode);
}


mcmsg_recv_rdmaf(unsigned long hdr1, unsigned long from)
{
	int	slot;

	slot = (hdr1 >> 16) & 0xffff;
	mcmsg_trace_recv(hdr1, from, slot, 0, 0, 0);
	mcmsg_rdma_flush_slot(slot);
}
