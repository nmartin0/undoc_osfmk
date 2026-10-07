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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright 1991 Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_ipc.c,v 1.33 1994/11/18 20:47:19 mtm Exp $
 */

/*
 * mcmsg_ipc.c
 *
 * Mach IPC interface
 */

#define	MCMSG_MODULE	MCMSG_MODULE_IPC

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <mach_assert.h>
#include <i860paragon/mcmsg/mcmsg_ipc.h>

/*
 * use kernel_pmap dirbase for norma transfers
 */
extern pmap_t kernel_pmap;
#define IPC_DIRBASE	kernel_pmap->dirbase

#if BIGPKTS
int ipcreq_amount_granted = 0;
/*
 * holding buffer for non-page transfers with bad alignment.
 */
double mcmsg_ipc_buf[MSG_PAGE_SIZE / sizeof(double)];
#endif BIGPKTS

/*
 * Invoked from netipc_send
 */
ipcreq_send()
{

	assert(ipcreq_send_count == 0);
	ipcreq_send_count++;

	/*
	 *	if the 3-trip protocol is disabled, just
	 *	start sending the data, otherwise get
	 *	clearance from the receiver before pushing
	 *	data onto the wire.
	 */
	if (ipcreq_rts_cts == 0) {
		mcmsg_send(0, MCTRL_IPC, 0, 0);
	} else {
		mcmsg_send(0, MCTRL_IPCQ, 0, 0);
	}
}

/*
 *	Called from mcmsg_send() (MCTRL_IPCQ)
 */

mcmsg_send_ipcq(mt)
	mcmsg_task_t	*mt;
{
	register unsigned long item;

	item = (ipsc_physnode << 16) | ipcreq_sending;
	mcmsg_trace_send(MCTRL_IPCQ, item, ipcreq_send_node, 0, 0, 0);
	send2(ipcreq_send_route, 0);
	send2eod(MCTRL_IPCQ, item);
	return 0;
}


/*
 *	MCTRL_IPCQ packet has arrived.
 *	Some node is requesting permission to send.
 */
mcmsg_recv_ipcq(hdr1, hdr2)
	unsigned long	hdr1, hdr2;
{
	struct ipc_sender_item	*rts;
	unsigned long	amount;
	unsigned long	from;

	from = hdr2 >> 16;
	amount = hdr2 & 0xFFFF;
	mcmsg_trace_recv(hdr1, hdr2, from, 1, ipcreq_recv_posted, 0);

	/*
	 *	if NORMA has posted a receive,
	 *	and if we aren't currently receiving,
	 *	and if there are no outstanding grants,
	 *	and if there are no outstanding requests-to-send,
	 *	ack now.
	 */
	if (ipcreq_recv_posted == 0) {		/* is norma waiting */
		ipcq_no_post++;
		goto do_enqueue;
	}
	if (ipcreq_receiving != 0) {		/* and not yet receiving */
		ipcq_recv_in_progress++;
		goto do_enqueue;
	}
	if (ipcreq_grants_given != 0) {		/* and no grants outstanding */
		ipcq_grants_given++;
		goto do_enqueue;
	}
	if (ipc_rts_head == 0) {		/* and/or nothing outstanding */
		assert((ipc_rts_head ? 1 : ipc_rts_count == 0));
		ipcreq_clear_to_send(from, amount);
		ipcreq_ipcq_hits++;
		return 0;
	}
	ipcq_locked_or_entries++;

do_enqueue:
	/*
	 *	if the free list is empty, we can't possibly
	 *	accept a new request-to-send; "nak" by granting 0.
	 */
	if ((rts = ipc_rts_free) == 0) {
		assert(ipc_rts_free_count == 0);
		ipcreq_clear_to_send(from, 0);
		return 0;
	}
	assert(ipc_rts_free_count > 0);
	ipc_rts_free_count--;
	ipc_rts_free = rts->ipc_next;


	/*
	 *	record the sender's information, and attach
	 *	it to the queue.  netipc_recv() will drain it.
	 */
	rts->ipc_next = 0;
	rts->ipc_node = from;
	rts->ipc_length = amount;
	if (ipc_rts_head == 0)
		ipc_rts_head = rts;
	else
		ipc_rts_tail->ipc_next = rts;

	ipc_rts_tail = rts;
	ipc_rts_count++;
	if (ipc_rts_count > ipc_rts_count_max) {
		ipc_rts_count_max = ipc_rts_count;
	}
	IPC_TRACE_DEBUG("rts queue", 3, from, amount, ipc_rts_count, 0);

	return 0;
}

/*
 *	Called from mcmsg_send() (MCTRL_IPCA)
 *
 *	"item" is actually two 16-bit values:
 *
 *		item< 0:15> is the amount to ack,
 *		item<16:31> is the node to ack.
 */

mcmsg_send_ipca(mt, ctl, item, ignored)
	mcmsg_task_t	*mt;
	unsigned long	item, ignored;
{
	register unsigned long	route, to, amount, ack;

	to = (item >> 16);
	amount = item & 0xffff;
	route = calculate_route(to);
	ack = (ipsc_physnode << 16) | amount;

	mcmsg_trace_send(MCTRL_IPCA, ack, to, 0, 0, 0);
	send2(route, 0);
	send2eod(MCTRL_IPCA, ack);
	return 0;
}

/*
 *	there's a MCTRL_IPCA packet in the fifo...
 */

mcmsg_recv_ipca(hdr1, hdr2)
	unsigned long	hdr1, hdr2;
{
	register int	method;

	mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 0, 0, 0);
	assert(ipcreq_sending > 0);
	assert((hdr2 >> 16) == ipcreq_send_request.node);

	/*
	 *	if the grant matches, start sending the IPC.
	 */
	if ((hdr2 & 0xFFFF) == ipcreq_sending) {
		mcmsg_send(0, MCTRL_IPC, 0, 0);
		return 0;
	}

	/*
	 *	presumably, we only get here if the receiver's
	 *	request-to-send queue is full.  so, retry.
	 *
	 *	XXX we should probably perform a back-off of
	 *	XXX some kind to ease the load on a slow node.
	 */
assert(0); /* how's this for backoff  prp */
	mcmsg_send(0, MCTRL_IPCQ, 0, 0);
	return 0;
}

#if !BIGPKTS
/*
 *	Inject bytes into the network.
 *
 *	Assumptions about mcmsg_fifo_out():
 *
 *		1. it needs a physical address.
 *		2. if the transfer crosses a page boundary, it is
 *		   Important to Know That.
 *		3. if addr is poorly aligned (not 8-byte aligned),
 *		   mcmsg_fifo_out() degenerates into multiple 8-byte
 *		   copies from a temporary space.
 */

int ipcreq_fifo_out(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	if ((addr & 7) != 0) {
		ipcreq_fifo_out_aligned_misses++;
		mcmsg_fifo_out(addr, n);
		return n;
	}

	ipcreq_fifo_out_aligned_hits++;
	a = (unsigned long)mcmsg_validate_real( addr,
				IPC_DIRBASE, __FILE__, __LINE__);
	assert(a != 0);

	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
	if (m >= n) {
		mcmsg_fifo_out(a, n);
	} else {
		mcmsg_fifo_out(a, m);
		a = (unsigned long)mcmsg_validate_real(addr + m,
				IPC_DIRBASE, __FILE__, __LINE__);
		assert(a != 0);
		mcmsg_fifo_out(a, n - m);
	}
	return n;
}

/*
 *	Similar to the above, but slap an eod on the end.
 */

int ipcreq_fifo_out_eod(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	if ((addr & 7) != 0) {
		ipcreq_fifo_out_eod_aligned_misses++;
		mcmsg_fifo_out_eod(addr, n);
		return n;
	}

	ipcreq_fifo_out_eod_aligned_hits++;
	a = (unsigned long)mcmsg_validate_real( addr,
			IPC_DIRBASE, __FILE__, __LINE__);
	assert(a != 0);
	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
	if (m >= n) {
		mcmsg_fifo_out_eod(a, n);
	} else {
		mcmsg_fifo_out(a, m);
		a = (unsigned long)mcmsg_validate_real( addr + m,
				IPC_DIRBASE, __FILE__, __LINE__);
		assert(a != 0);
		mcmsg_fifo_out_eod(a, n - m);
	}
	return n;
}

/*
 *	Accept bytes from the network.
 */

int ipcreq_fifo_in(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	
	if ((addr & 7) != 0) {
		mcmsg_fifo_in(addr, n);
		return n;
	}
	a = (unsigned long)mcmsg_validate_real( addr,
			IPC_DIRBASE, __FILE__, __LINE__);

	assert(a != 0);
	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);

	if (m >= n) {
		mcmsg_fifo_in(a, n);
	} else {
		mcmsg_fifo_in(a, m);
		a = (unsigned long)mcmsg_validate_real( addr + m,
				IPC_DIRBASE, __FILE__, __LINE__);
		assert(a != 0);
		mcmsg_fifo_in(a, n - m);
	}
	return n;
}
#endif !BIGPKTS

#if	IPC_CSUM

/*
 *	Compute a checksum on a buffer.
 */

unsigned long ipcreq_incremental_checksum(csum, addr, n)
	unsigned long	csum;
	unsigned long	addr;
	unsigned long	n;
{
	register unsigned long	*a;
	register unsigned long	i;

	IPC_TRACE_CKDATA("enter checksum", 3, csum, addr, n, 0);

	assert((addr & 3) == 0);
	assert((n & 3) == 0);

	a = (unsigned long *) addr;
	i = (n >> 2) + 1;
	while (--i) {
		IPC_TRACE_CKDATA("checksum", 2, csum, *a, 0, 0);
		csum += *a++;
	}

	IPC_TRACE_CKDATA("leave checksum", 1, csum, 0, n, 0);

	return csum;
}

/*
 *	Compute a checksum on a netipc vector.
 */

unsigned long ipcreq_compute_checksum(vec, cnt)
	register struct netvec	*vec;
	register int		cnt;
{
	register unsigned long	csum;
	register unsigned long	*a;
	register unsigned long	n;

	csum = 0;
	while (cnt--) {
		csum = ipcreq_incremental_checksum(csum, vec->addr, vec->size);
		vec++;
	}
	return csum;
}

/*
 *	Compare the computed checksum with the previously
 *	received checksum.
 */

void ipcreq_compare_checksums(computed, received)
	register unsigned long	computed, received;
{
	if (computed == received)
		return;

	ipcreq_ck_count++;

	if (ipcreq_ignore_bad_checksums)
		return;

	mcmsg_trace_debug("ipc checksum failure",
		3, ipcreq_recv_node, computed, received, 0);

	if (ipcreq_stop_on_csum_failure)
		assert(computed == received);
}
#endif	IPC_CSUM


/*
 *	Called from mcmsg_send() (MCTRL_IPC)
 */

#if BIGPKTS
mcmsg_send_ipc(mt)
	mcmsg_task_t	*mt;
{
	ipcreq_t *ipcreq;			/* Request block, if any */
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register long		route;		/* Route */
	register int		vi;		/* Next netvec index */
	register int		i;		/* Temp index */
	register unsigned long	tmp;
	register unsigned long	bp1, bp2;
	int	node;				/* Destination node number */
	struct netvec *vec;			/* Buffer vector */
	int	count;				/* Buffer vector length */

	ipcreq = &ipcreq_send_request;
	node = ipcreq->node;
	vec = ipcreq->bv;
	count = ipcreq->count;

	assert(count > 0);
	assert(count <= MAXBV);
	assert(ipcreq->total != 0);

	addr = vec[0].addr;
	size = vec[0].size;
	assert(size > 0);

	route = ipcreq_send_route;

	IPC_TRACE_DEBUG("send chunk", 4, addr, size,
		*(unsigned long *) addr,
		*(unsigned long *)(addr + size - 4));

	assert((size & 3) == 0);	/* suspect NORMA */
	assert(size > 0);		/* suspect this code */

	/*
	 *	packet header logic:
	 *
	 *	if node is a real node number {
	 *		send a 1st packet header;
	 *		set node to an unreal node;
	 *	} else {
	 *		send a continued packet header;
	 *	}
	 */
	if (node != INVALID_NODE) {
		route = ipcreq_send_route;
		tmp = (ipsc_physnode << 16) | size;
		IPC_TRACE_DEBUG("ipc send 1st",
			3, node, ipsc_physnode, route, 0);
		mcmsg_trace_send(MCTRL_IPC, tmp, node, 0, 0, 0);
		node = INVALID_NODE;
		send2_now(route, 0);
		send2_now(MCTRL_IPC, tmp);
#if BUMPERS
		send2_now(0,0);
		send2_now(0,0);
		send2_now(0,0);
		send2_now(0,0);
#endif BUMPERS
	} else {
		/*
		 *	new packet (2nd or later)
		 */
		tmp = (ipsc_physnode << 16) | size;
		IPC_TRACE_DEBUG("ipc send 2nd",
			3, node, ipsc_physnode, route, 0);
		mcmsg_trace_send(MCTRL_IPC | (1 << 16),
				tmp, ipcreq_send_node, 0, 0, 0);
		send2_now(route, 0);
		send2_now(MCTRL_IPC | (1 << 16), tmp);
#if BUMPERS
		send2_now(0,0);
		send2_now(0,0);
		send2_now(0,0);
		send2_now(0,0);
#endif BUMPERS
	} 

	/*
	 *	header has been pushed;
	 *	Send the current vector
	 */
	bp1 = mcmsg_validate_read1(addr, size, IPC_DIRBASE);
	bp2 = mcmsg_validate2();
	mcmsg_send_buf(bp1, bp2, size);
	ipcreq_sending -= size;
	count--;

	if (count == 0) {
		/*
		 *	All vectors sent.
		 */
		assert(ipcreq_sending == 0);
		ipcreq_send_count--;
		assert(ipcreq_send_count == 0);
		IPC_TRACE_DEBUG("ipc send intr", 0, 0, 0, 0, 0);
		mp_netipc_send_intr();
		return 0;
	} else {
		/*
		 *	Schedule the next vector.
		 */
		ipcreq->node = node;
		ipcreq->count = count;
		for (i=0; i<count; i++) {
			vec[i] = vec[i+1];
		}
		IPC_TRACE_DEBUG("ipc send defer", 3, node, addr, size, 0);
		mcmsg_send_tail(0, MCTRL_IPC, 0, 0);
	}
}

#else BIGPKTS
mcmsg_send_ipc(mt)
	mcmsg_task_t	*mt;
{
	ipcreq_t *ipcreq;			/* Request block, if any */
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register long		nb;		/* Bytes left to fill */
	register long		route;		/* Route */
	register unsigned long	a;		/* tmp for misaligned xfer */
	register unsigned long	b;		/* tmp for misaligned xfer */
	register int		vi;		/* Next netvec index */
	register int		i;		/* Temp index */
	register unsigned long	tmp;
#if	IPC_CSUM
	register unsigned long	csum;
#endif	IPC_CSUM
	int	node;				/* Destination node number */
	struct netvec *vec;			/* Buffer vector */
	int	count;				/* Buffer vector length */
	int	readiness;			/* Check for one pkt only */

	ipcreq = &ipcreq_send_request;
	node = ipcreq->node;
	vec = ipcreq->bv;
	count = ipcreq->count;

	assert(count > 0);
	assert(count <= MAXBV);
	assert(ipcreq->total != 0);

	addr = vec[0].addr;
	size = vec[0].size;
	vi = 1;
	assert(size > 0);

	route = ipcreq_send_route;
	nb = IPC_PKT_SIZE;

	assert(readiness = 1);
	for (;;) {

		IPC_TRACE_DEBUG("send chunk", 4, addr, size,
			*(unsigned long *) addr,
			*(unsigned long *)(addr + size - 4));

		assert((size & 3) == 0);	/* suspect NORMA */
		assert(size > 0);		/* suspect this code */
		assert((nb & 7) == 0);		/* suspect this code */

		/*
		 *	packet header logic:
		 *
		 *	if node is a real node number {
		 *		send a 1st packet header;
		 *		set node to an unreal node;
		 *	} else {
		 *		send a continued packet header;
		 *	}
		 */
		if (node != INVALID_NODE) {

			assert(readiness-- == 1);
#if	IPC_CSUM
			csum = ipcreq_compute_checksum(vec, count);
			IPC_TRACE_DEBUG("ipc send checksum", 1, csum, 0, 0, 0);
#endif	IPC_CSUM
			route = ipcreq_send_route;
			tmp = (ipsc_physnode << 16) | ipcreq_sending;
			IPC_TRACE_DEBUG("ipc send 1st",
				3, node, ipsc_physnode, route, 0);
			mcmsg_trace_send(MCTRL_IPC, tmp, node, 0, 0, 0);
			node = INVALID_NODE;
			send2_now(route, 0);
			send2_now(MCTRL_IPC, tmp);
#if	IPC_CSUM
			send2_now(csum, IPC_CHECKSUM_MAGIC);
#endif	IPC_CSUM
		} else if (nb == IPC_PKT_SIZE) {
			/*
			 *	new packet (2nd or later)
			 */
			assert(readiness-- == 1);
			tmp = (ipsc_physnode << 16) | ipcreq_sending;
			IPC_TRACE_DEBUG("ipc send 2nd",
				3, node, ipsc_physnode, route, 0);
			mcmsg_trace_send(MCTRL_IPC | (1 << 16),
					tmp, ipcreq_send_node, 0, 0, 0);
			send2_now(route, 0);
			send2_now(MCTRL_IPC | (1 << 16), tmp);
		} else {
			/*
			 *	looping to fill in packet and have
			 *	already pushed the appropriate header.
			 */
			assert(ipcreq_sending != 0);
		}

		/*
		 *	header has been pushed; start
		 *	pushing data.
		 */

		/*
		 *	if the size remaining in the current
		 *	netvec is larger than the bytes
		 *	remaining in the current packet,
		 *	fill up the packet and continue.
		 */
		if (size >= nb) {
			IPC_TRACE_DEBUG("fifo out (a)", 3, addr, nb, size, 0);
			ipcreq_fifo_out_eod(addr, nb);
			ipcreq_sending -= nb;
			if (size == nb) {
				if (ipcreq_sending == 0) goto send_done;
				addr = vec[vi].addr;
				size = vec[vi].size;
				vi++;
				assert(size > 0);
			} else {		
				addr += nb;
				size -= nb;
			}
			nb = IPC_PKT_SIZE;
			goto save_send;
		}

		/*
		 *	the remainder of the current netvec
		 *	can fit in the current packet.
		 *	determine if this is the end of
		 *	the packet stream.  if the
		 *	remaining byte counts match, it's the
		 *	end of this packet stream.
		 */
		assert(size <= nb);
		assert(size > 0);
		if (ipcreq_sending == size) {
			/*
			 *	last netvec and last packet.
			 *	special case a dangling word.
			 */
			if ((size & 7) == 0) {
				/*
				 *	multiple of 8 bytes;
				 *	no problem.
				 */
				IPC_TRACE_DEBUG("fifo out (b)",
					2, addr, size, 0, 0);
				ipcreq_fifo_out_eod(addr, size);
				ipcreq_sending -= size;
				goto send_done;
			}
			/*
			 *	darn, have 4-bytes dangling
			 *	at the end of the last netvec.
			 *	need to:
			 *		a) push up to size - 4.
			 *		b) pick up the last word.
			 *		c) pick up a dangle magic.
			 *		d) push two words w/ eod.
			 *
			 *	XXX there's nothing special about
			 *	XXX magic dangle's -- just one more
			 *	XXX easy assertion.
			 */
			if (size > 4) {
				IPC_TRACE_DEBUG("fifo out (c)",
						2, addr, size - 4, 0, 0);
				ipcreq_fifo_out(addr, size - 4);
				ipcreq_sending -= size - 4;
				addr += size - 4;
				assert(ipcreq_sending == 4);
			}
			a = *(unsigned long *) addr;
			b = IPC_DANGLE_MAGIC;	/* dumped by recv */
			IPC_TRACE_DEBUG("ipc send2eod (a)", 2, a, b, 0, 0);
			send2eod(a, b);
			ipcreq_sending -= 4;
			goto send_done;
		}

		/*
		 *	there must be more after this vector;
		 *	push the remainder of the current vector
		 *	and move to the next vector.
		 */
		assert(ipcreq_sending > size);
		if ((size & 7) == 0) {
			/*
			 *	size is a multiple of 8 bytes;
			 *	push it and move to the next netvec.
			 */
			IPC_TRACE_DEBUG("fifo out (d)", 2, addr, size, 0, 0);
			ipcreq_fifo_out(addr, size);
			ipcreq_sending -= size;
			nb -= size;
			assert(nb != 0);
			addr = vec[vi].addr;
			size = vec[vi].size;
			vi++;
			assert(size > 0);
			continue;
		}

		/*
		 *	size remaining in the current netvec must not
		 *	be a multiple of 8; need to:
		 *
		 *		a) push up to size - 4.
		 *		b) pick up last word of current netvec.
		 *		c) pick up first word of next netvec.
		 *		d) push two words (perhaps w/ eod).
		 *
		 *	special case for the size of the next vec == 4.
		 */
		/*
		 *	push up to, but not including, the last word.
		 */
		if (size > 4) {
			IPC_TRACE_DEBUG("fifo out (e)", 2, addr, size-4, 0, 0);
			ipcreq_fifo_out(addr, size - 4);
			ipcreq_sending -= size - 4;
			addr += size - 4;
			nb -= size - 4;
			assert(nb != 0);
		}

		/*
		 *	pick up the last word at the end of the
		 *	current netvec.
		 */
		a = *(unsigned long *) addr;

		/*
		 *	move to the next netvec
		 */
		addr = vec[vi].addr;
		size = vec[vi].size;
		vi++;
		assert(size > 0);

		/*
		 *	pick up the first word at the beginning
		 *	of the next netvec.
		 */
		assert(((addr | size) & 3) == 0);
		b = *(unsigned long *) addr;	/* pick 1 word */
		addr += 4;
		size -= 4;

		/*
		 *	if word "b" wasn't the only word
		 *	in the new vec...
		 */
		if (size != 0) {
			if (nb == 8) {
				IPC_TRACE_DEBUG("ipc send2eod", 2, a, b, 0, 0);
				send2eod(a, b);
				ipcreq_sending -= 8;
				if (ipcreq_sending == 0) goto send_done;
				nb = IPC_PKT_SIZE;
				goto save_send;
			}
			IPC_TRACE_DEBUG("ipc send2", 2, a, b, 0, 0);
			send2(a, b);
			ipcreq_sending -= 8;
			nb -= 8;
			assert(nb != 0);
			continue;
		}

		/*
		 *	pathological case:
		 *	previous netvec had a 4 byte dangle,
		 *	next netvec was of size 4,
		 *	and it was the last netvec.
		 */
		if (ipcreq_sending == 8) {
			IPC_TRACE_DEBUG("ipc send2eod (b)", 2, a, b, 0, 0);
			send2eod(a, b);
			ipcreq_sending -= 8;
			nb -= 8;
			assert(nb != 0);
			goto send_done; 
		}
		/* 
		 *      previous netvec had a 4 byte dangle,
		 *      next netvec was of size 4,
		 * 	and another netvec follows
		 */
		IPC_TRACE_DEBUG("ipc send2", 2, a, b, 0, 0);
		send2(a, b);
		ipcreq_sending -= 8;
		nb -= 8;
		assert(nb != 0);
		assert(vi < count);
		addr = vec[vi].addr;
		size = vec[vi].size;
		vi++;
		assert(size > 0);
		continue;
send_done:
		assert(ipcreq_sending == 0);
		assert(readiness == 0);
		ipcreq_send_count--;
		assert(ipcreq_send_count == 0);
		IPC_TRACE_DEBUG("ipc send intr", 0, 0, 0, 0, 0);
		mp_netipc_send_intr();
		return 0;
	}

	/*
	 *	cannot send any more at this time.
	 *	reschedule the send.  store the state
	 *	of the not-yet-completed request.
	 */

save_send:
	assert(readiness == 0);
	ipcreq->node = node;
	ipcreq->count = count - vi + 1;
	ipcreq->bv[0].addr = addr;
	ipcreq->bv[0].size = size;
	assert(size > 0);
	for (i = 1; vi < count; vi++, i++) {
		ipcreq->bv[i] = vec[vi];
	}
	IPC_TRACE_DEBUG("ipc send defer", 3, node, addr, size, 0);

	/*
	 *	reschedule the send and return
	 */
	mcmsg_send_tail(0, MCTRL_IPC, 0, 0);
}
#endif BIGPKTS

/*
 *	send the sender a clear-to-send.
 */

ipcreq_clear_to_send(node, amount)
	long	node, amount;
{
	if (amount > 0) {
		assert(ipcreq_grants_given == 0);
		ipcreq_grants_given++;
		ipcreq_recently_granted = node;
#if    BIGPKTS
		ipcreq_amount_granted = amount;
#endif BIGPKTS
	}
	mcmsg_send(0, MCTRL_IPCA, (node << 16) | amount, 0);
}

/*
 * Invoked from netipc_recv
 */

ipcreq_recv(mt, vec, count)
	mcmsg_task_t	*mt;
	register struct netvec *vec;		/* Provided buffer vector */
	int count;				/* Buffer vector length */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register int		i;		/* Temp index */
	register int		bytes;
	struct ipc_sender_item	*rts;

	assert(count <= MAXBV);
	assert(count >= 2);

	assert(ipcreq_recv_count == 0);
	ipcreq_recv_count++;
	ipcreq = &ipcreq_recv_request;

	bytes = 0;
	ipcreq->count = count;
	for (i = 0; i < count; i++) {
		ipcreq->bv[i] = vec[i];
		bytes += vec[i].size;
	}
	ipcreq->total = bytes;
	ipcreq_recv_posted = 1;

	/*
	 *	if the 3-trip is enabled, and
	 *	the request-to-send queue is not empty,
	 *	remove the send request from the queue,
	 *	and return it to the free list.
	 *	give permission to the sender to send.
	 */
	if (ipcreq_rts_cts != 0) {
		if ((rts = ipc_rts_head) != 0) {
			ipc_rts_count--;
			if ((ipc_rts_head = rts->ipc_next) == 0) {
				assert(ipc_rts_count == 0);
				ipc_rts_tail = 0;
			}
			rts->ipc_next = ipc_rts_free;
			ipc_rts_free = rts;
			ipc_rts_free_count++;
			ipcreq_clear_to_send(rts->ipc_node, rts->ipc_length);
		}
	}
	mcmsg_enable_rx_interrupts();
}

/*
 *	flush the current packet in the fifo.
 *	"n" is the number of bytes to flush.
 */

ipcreq_fifo_flush(n)
	register unsigned long	n;
{
	register long		nb = IPC_PKT_SIZE;

	ipcreq_fifo_flush_count++;
	if (n > nb) {
		n = nb;
	}
	mcmsg_fifo_flush(n);
}

/*
 *	Called when a NORMA IPC packet arrives in the fifo.
 */

#if BIGPKTS
/*
 *	Version for large packets.
 *	Uses the mcmsg_recv_buf() interface to allow nic-a workaround.
 *	If the length of the incoming packet is poorly aligned, 
 *	it needs to do buffering and copying.
 */
mcmsg_recv_ipc(hdr1, length)
	unsigned long	hdr1;			/* First header word */
	register int	length;		/* Remaining message bytes */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register int		count;		/* Buffer vector length */
	register int		vi, i;		/* Temp index */
	register long		n;
	register unsigned long	bp1, bp2;

#if    BUMPERS
	recv2dummy();
	recv2dummy();
	recv2dummy();
	recv2dummy();
#endif BUMPERS

	/*
	 *	1st packet of a Mach IPC?
	 */
	if (FIRST_PACKET(hdr1)) {
		mcmsg_trace_recv(hdr1, length, FROM_NODE(length),
				1, ipcreq_recv_posted, 0);

		/*	
		 *	Flush if already locked on.
		 */
		if (IS_LOCKED_ONTO(ipcreq_recv_node)) {
			/*
			 *	can't accept the packet because we've
			 *	already "locked on" to a packet stream
			 *	from another node.
			 */
			mcmsg_trace_debug("ipc flush ( 1st)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_flushed_already_receiving++;
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}

		/*
		 *	Flush if NORMA hasn't posted a receive.
		 */
		if (ipcreq_recv_posted == 0) {
			mcmsg_ipc_drop++;
			mcmsg_trace_debug("ipc flush (~req)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}

		/*
		 *	Lock on (shields up!)
		 */
		assert(ipcreq_receiving == 0);
		assert(IS_LOCKED_ONTO(ipcreq_recv_node) == 0);
		ipcreq_receiving = 1;
		ipcreq_recv_node = FROM_NODE(length);
		ipcreq_length_remaining = ipcreq_amount_granted & 0xFFFF;
		IPC_TRACE_DEBUG("ipc locked on",
			2, ipcreq_recv_node, length, 0, 0);
	} else {
		/*
		 *	2nd (or later) packet of Mach IPC
		 */
		assert((hdr1 >> 16) == 1);
		mcmsg_trace_recv(hdr1, length, FROM_NODE(length), 0, 0, 0);

		/*
		 *	Flush if we're already locked on.
		 */
		if (ipcreq_recv_node != FROM_NODE(length)) {
			/*
			 *	can't accept the packet because
			 *	we've already "locked on" to another node.
			 */
			mcmsg_trace_debug("ipc flush ( 2nd)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_flushed_other_node++;
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}
	}

	IPC_TRACE_DEBUG("ipc recv", 4,
		hdr1, length, ipcreq_recv_node, ipcreq_length_remaining);

	length &= 0xFFFF;

	assert(ipcreq_receiving != 0);
	assert(ipcreq_recv_posted != 0);
	assert(IS_LOCKED_ONTO(ipcreq_recv_node));

	ipcreq = &ipcreq_recv_request;
	count = ipcreq->count;
	addr = ipcreq->bv[0].addr;
	size = ipcreq->bv[0].size;

	/*
	 *	pull the current packet off of the network.
	 */
	assert(length > 0);
	if (size >= length) {
		/*
		 * The packet can fit into the current vector.
		 * Pull it all at once.
		 */
		bp1 = mcmsg_validate_read1(addr, length, IPC_DIRBASE);
		bp2 = mcmsg_validate2();
		mcmsg_recv_buf(bp1, bp2, length);

		/*
		 * Adjust the receive vector.
		 */
		ipcreq_length_remaining -= length;
		if (ipcreq_length_remaining > 0) {
			if (size == length) {
				for (i=0; i<count; i++) {
					ipcreq->bv[i] = ipcreq->bv[i+1];
				}
				ipcreq->count--;
			} else {
				ipcreq->bv[0].size -= length;
				ipcreq->bv[0].addr += length;
			}
		}
	} else {
		int pktlen, veclen;
		/*
		 * The packet needs to be split up into more
		 * than one vector. Pull it into a buffer and 
		 * copy it into vector buffers.
		 */
		assert(sizeof(mcmsg_ipc_buf) > length);
		bp1 = mcmsg_validate_read1(mcmsg_ipc_buf, length, IPC_DIRBASE);
		bp2 = mcmsg_validate2();
		mcmsg_recv_buf(bp1, bp2, length);

		/*
		 * Copy the data into the vectors.
		 * Adjust the buffer vectors counts on the fly.
		 */
		bp1 = (unsigned long)&mcmsg_ipc_buf;
		pktlen = length;

		for (vi=0; vi< count && pktlen > 0; vi++) {
			veclen = ipcreq->bv[vi].size;
			if (veclen > 0) {
				if (veclen >= pktlen) {
					/* data fits in buffer */
					bcopy(bp1, ipcreq->bv[vi].addr, pktlen);
					ipcreq->bv[vi].size -= pktlen;
					ipcreq->bv[vi].addr += pktlen;
					pktlen = 0;
				} else {
					/* data is longer than buffer */
					bcopy(bp1, ipcreq->bv[vi].addr, veclen);
					ipcreq->bv[vi].size = 0;
					pktlen -= veclen;
					bp1 += veclen;
				}
			}
		}
		assert(pktlen == 0);
		ipcreq_length_remaining -= length;
	}

	if (ipcreq_length_remaining > 0) {
		IPC_TRACE_DEBUG("ipc recv defer", 1, ipcreq_length_remaining, 0, 0, 0);
	} else {
		/*
		 *	message has been received; prepare to
		 *	deliver up to NORMA.
		 */
	
		/*STAND_DOWN(ipcreq_recv_node);*/
		ipcreq_recv_posted = 0;
		ipcreq_recv_node = -1;
		ipcreq_receiving = 0;
		ipcreq_recv_count--;
		assert(ipcreq_recv_count == 0);
		if (ipcreq_rts_cts) {
			ipcreq_grants_given--;
			assert(ipcreq_grants_given == 0);
		}
	
		IPC_TRACE_DEBUG("ipc deliver", 0, 0, 0, 0, 0);
	
		/* Deliver to kernel */
		mp_netipc_recv_intr();
	}
	return;
}
#else BIGPKTS

/*
 *	Complications arise when buffers aren't multiples of 8 bytes,
 *	or when a packet crosses an unfavorably aligned vector.
 */
mcmsg_recv_ipc(hdr1, length)
	unsigned long	hdr1;			/* First header word */
	register int	length;		/* Remaining message bytes */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register int		count;		/* Buffer vector length */
	register int		vi;		/* Vec index */
	register int		i;		/* Temp index */
	register long		n;
	register long		nb = IPC_PKT_SIZE;
#if	IPC_CSUM
	register unsigned long	a, b;
	static unsigned long	csum;
	static unsigned long	csumck;
#endif	IPC_CSUM

	/*
	 *	1st packet of a Mach IPC?
	 */
	if (FIRST_PACKET(hdr1)) {
		mcmsg_trace_recv(hdr1, length, FROM_NODE(length),
				1, ipcreq_recv_posted, 0);
#if	IPC_CSUM
		/*
		 *	Get the checksum off the wire...
		 */
		recv2(a, b);
		assert(b == IPC_CHECKSUM_MAGIC);
		IPC_TRACE_DEBUG("ipc recv csum", 2, a, b, 0, 0);
#endif	IPC_CSUM

		/*	
		 *	Flush if already locked on.
		 */
		if (IS_LOCKED_ONTO(ipcreq_recv_node)) {
			/*
			 *	can't accept the packet because we've
			 *	already "locked on" to a packet stream
			 *	from another node.
			 */
			mcmsg_trace_debug("ipc flush ( 1st)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_flushed_already_receiving++;
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}

		/*
		 *	Flush if NORMA hasn't posted a receive.
		 */
		if (ipcreq_recv_posted == 0) {
			mcmsg_ipc_drop++;
			mcmsg_trace_debug("ipc flush (~req)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}

		/*
		 *	Lock on (shields up!)
		 */
		assert(ipcreq_receiving == 0);
		assert(IS_LOCKED_ONTO(ipcreq_recv_node) == 0);
		ipcreq_receiving = 1;
		ipcreq_recv_node = FROM_NODE(length);
		ipcreq_length_remaining = length & 0xFFFF;
#if	IPC_CSUM
		csumck = a;	/* received */
		csum = 0;	/* computed */
		IPC_TRACE_DEBUG("ipc locked on", 4,
			ipcreq_recv_node, ipcreq_length_remaining, a, b);
#else	IPC_CSUM
		IPC_TRACE_DEBUG("ipc locked on",
			2, ipcreq_recv_node, length, 0, 0);
#endif	IPC_CSUM

	} else {
		/*
		 *	2nd (or later) packet of Mach IPC
		 */
		assert((hdr1 >> 16) == 1);
		mcmsg_trace_recv(hdr1, length, FROM_NODE(length), 0, 0, 0);

		/*
		 *	Flush if we're already locked on.
		 */
		if (ipcreq_recv_node != FROM_NODE(length)) {
			/*
			 *	can't accept the packet because
			 *	we've already "locked on" to another node.
			 */
			mcmsg_trace_debug("ipc flush ( 2nd)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_flushed_other_node++;
			ipcreq_fifo_flush(length & 0xFFFF);
			return;
		}
	}

	IPC_TRACE_DEBUG("ipc recv", 4,
		hdr1, length, ipcreq_recv_node, ipcreq_length_remaining);

	length &= 0xFFFF;

	assert(ipcreq_receiving != 0);
	assert(ipcreq_recv_posted != 0);
	assert(IS_LOCKED_ONTO(ipcreq_recv_node));
	assert(ipcreq_length_remaining == length);

	ipcreq = &ipcreq_recv_request;
	count = ipcreq->count;
	addr = ipcreq->bv[0].addr;
	size = ipcreq->bv[0].size;
	vi = 1;

	/*
	 *	pull the current packet off of the network.
	 */
	while (nb > 0) {
		assert(length > 0);

		/*
		 *	compute the number of bytes to actually
		 *	accept pull from the fifo.
		 */
		n = length;
		if (n > nb) {
			n = nb;
		}
		if (n > ((size + 7) & ~7)) {
			n = ((size + 7) & ~7);
		}

		/*
		 *	check alignments, and transfer accordingly.
		 */
		if ((n <= size) && ((n & 7) == 0)) {
			/*
			 *	most favorable alignment
			 */
			ipcreq_best_alignment_hits++;
			IPC_TRACE_DEBUG("fifo in (a)", 2, addr, n, 0, 0);
			ipcreq_fifo_in(addr, n);
			ipcreq_length_remaining -= n;
			IPC_TRACE_DEBUG("fifo in done (a)", 0, 0, 0, 0, 0);
			IPC_INCREMENTAL_CSUM(csum, addr, n);
			size -= n;
			if (size == 0) {
				/*
				 *	the current vector has been filled,
				 *	move on to the next (if needed).
				 */
				if (vi < count) {
					/*
					 *	still another vector to
					 *	receive into...
					 */
					addr = ipcreq->bv[vi].addr;
					size = ipcreq->bv[vi].size;
					vi++;
				} else {
					/*
					 *	no more vectors...does the
					 *	total length match?
					 */
					assert(n == length);
				}
			} else {
				/*
				 *	receipt of the current vector is
				 *	still incomplete. (size != 0)
				 */
				addr += n;
			}
		} else {
			unsigned long t1, t2;

			/*
			 *	unfavorable alignments.
			 */
			ipcreq_poor_alignment_hits++;
			if (n > size) {

				/*
				 *	this clause handles the case of
				 *	one 32-bit word at the end of the
				 *	current vector, and one 32-bit word
				 *	at the beginning of the next vector.
				 */
				assert(vi < count);
				assert((n & 7) == 0);
				assert(n - size == 4);

				IPC_TRACE_DEBUG("crossed", 0, 0, 0, 0, 0);
				ipcreq_cross_alignment_hits++;

				IPC_TRACE_DEBUG("fifo in (b)",
					4, addr, n - 8, size, n);
				ipcreq_fifo_in(addr, n - 8);
				ipcreq_length_remaining -= n - 8;
				IPC_TRACE_DEBUG("fifo in done (b)",
					0, 0, 0, 0, 0);
				IPC_INCREMENTAL_CSUM(csum, addr, n - 8);

				recv2(t1, t2);
				ipcreq_length_remaining -= 8;
				*(unsigned long *)(addr + n - 8) = t1;
				IPC_INCREMENTAL_CSUM(csum, (addr + n - 8), 4);
				addr = ipcreq->bv[vi].addr;
				size = ipcreq->bv[vi].size - 4;
				*(unsigned long *)addr = t2;
				IPC_INCREMENTAL_CSUM(csum, addr, 4);
				addr += 4;
				vi++;
			} else {
				/*
				 *	this clause handles the case of
				 *	a single 32-bit word that has been
				 *	padded to 64 bits.  it drops the
				 *	second 32-bit word.
				 */
				assert((n & 7) == 4);
				ipcreq_dump_alignment_hits++;

				IPC_TRACE_DEBUG("fifo in (c)",
					3, addr, n & ~7, n, 0);
				ipcreq_fifo_in(addr, n & ~7);
				IPC_TRACE_DEBUG("fifo in done (c)",
					0, 0, 0, 0, 0);
				IPC_INCREMENTAL_CSUM(csum, addr, n & ~7);

				recv2(t1, t2);
				ipcreq_length_remaining -= 4;
				*(unsigned long *)(addr + (n & ~7)) = t1;
				IPC_INCREMENTAL_CSUM(csum, (addr + (n & ~7)),4);
				IPC_TRACE_DEBUG("dumped", 1, t2, 0, 0, 0);
				assert(t2 == IPC_DANGLE_MAGIC);
				size -= n;
			}
		}
		IPC_TRACE_DEBUG("end loop", 2, n, length, 0, 0);

		nb -= n;
		length -= n;
		if (length == 0) {

			/*
			 *	message has been received; prepare to
			 *	deliver up to NORMA.
			 */
#if	IPC_CSUM
			ipcreq_compare_checksums(csum, csumck);
#endif	IPC_CSUM

			/*
			 *	Disable the lock on.
			 */
			/*STAND_DOWN(ipcreq_recv_node);*/
			ipcreq_recv_posted = 0;
			ipcreq_recv_node = -1;
			ipcreq_receiving = 0;
			ipcreq_recv_count--;
			assert(ipcreq_recv_count == 0);
			if (ipcreq_rts_cts) {
				ipcreq_grants_given--;
				assert(ipcreq_grants_given == 0);
			}

			IPC_TRACE_DEBUG("ipc deliver", 0, 0, 0, 0, 0);

			/* Deliver to kernel */
			mp_netipc_recv_intr();
			return;
		}
	}

	/*
	 *	store remaining request, and the next packet
	 *	will get here soon (perhaps).
	 */
	IPC_TRACE_DEBUG("ipc recv defer", 3, length, vi, count, 0);
	assert(count <= MAXBV);
	ipcreq->count = count - vi + 1;
	ipcreq->bv[0].addr = addr;
	ipcreq->bv[0].size = size;
	if (vi > 1) {
		/*
		 *	shuffle the remaining netvecs by one.
		 */
		for (i = 1; vi < count; vi++, i++) {
			ipcreq->bv[i] = ipcreq->bv[vi];
		}
	}
	return;
}
#endif BIGPKTS
