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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_ipc.c,v 1.22 1994/11/18 20:41:34 mtm Exp $
 */

/*
 * mcmsg_ipc.c
 *
 * Mach IPC interface
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>
#include <mach_assert.h>

#if	PARAGON860

#define IPC_COPY_SEND	1	/* XXX REMOVE WHEN NORMA IS FIXED */
#define IPC_CSUM	MACH_ASSERT
#define IPC_DEBUG	0
#define CKDATA		0	/* extra debug -- needs IPC_CSUM */

#if	IPC_DEBUG
#define	IPC_TRACE_DEBUG(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	IPC_DEBUG
#define	IPC_TRACE_DEBUG(a,b,c,d,e,f)
#endif	IPC_DEBUG

#if	CKDATA
#define	IPC_TRACE_CKDATA(a,b,c,d,e,f)	mcmsg_trace_debug(a,b,c,d,e,f)
#else	CKDATA
#define	IPC_TRACE_CKDATA(a,b,c,d,e,f)
#endif	CKDATA

#define	IS_LOCKED_ONTO(n)	((n) != -1)
#define	FROM_NODE(n)		((n) >> 16)
#define	FIRST_PACKET(h)		(((h) >> 16) == 0)

/*
 *	packetized NORMA ipc support
 */
ipcreq_t	ipcreq_send_request;
ipcreq_t	ipcreq_recv_request;
long	ipcreq_packet_size = 1792;	/* 56 lines * 32 bytes each */
long	ipcreq_recv_node;
long	ipcreq_length_remaining;
long	ipcreq_send_route;
long	ipcreq_send_node;
long	ipcreq_sending;
int	ipcreq_recv_posted = 0;
#define IPC_PKT_SIZE	ipcreq_packet_size

/*
 *	3-trip protocol support (RTS, CTS, SEND).
 */
#define MAX_RTS_REQS	576	/* XXX 64x9 */
struct ipc_sender_item {
	unsigned short		ipc_node;	/* remote node */
	unsigned short		ipc_length;	/* length of message */
	struct ipc_sender_item	*ipc_next;	/* next node */
};
struct ipc_sender_item *ipc_rts_head;		/* head of rts queue */
struct ipc_sender_item *ipc_rts_tail;		/* tail of rts queue */
struct ipc_sender_item *ipc_rts_free;		/* free list of queue items */
struct ipc_sender_item ipc_rts_cts_table[MAX_RTS_REQS];	/* XXX */
int	ipcreq_grants_given = 0;
int	ipc_rts_count, ipc_rts_count_max;
int	ipc_rts_free_count;
int	ipcreq_rts_cts = 1;			/* use the 3-trip */

/*
 *	some alignment and transfer statistics
 */
unsigned long	ipcreq_best_alignment_hits;
unsigned long	ipcreq_poor_alignment_hits;
unsigned long	ipcreq_cross_alignment_hits;
unsigned long	ipcreq_dump_alignment_hits;
unsigned long	ipcreq_fifo_out_aligned_hits;
unsigned long	ipcreq_fifo_out_aligned_misses;
unsigned long	ipcreq_fifo_out_eod_aligned_hits;
unsigned long	ipcreq_fifo_out_eod_aligned_misses;

/*
 *	some flushing statistics
 */
unsigned long	ipcreq_fifo_flush_count = 0;
unsigned long	ipcreq_flushed_console_stopped;
unsigned long	ipcreq_flushed_already_receiving;
unsigned long	ipcreq_flushed_other_node;

#if	IPC_CSUM
/*
 *	perform checksums? (debugging)
 */
#define	IPC_CHECKSUM_MAGIC	0xfeed1ace

int		ipcreq_ck_count;		/* checksum failures */
int ipcreq_ignore_bad_checksums = 0;
int ipcreq_stop_on_csum_failure = 1;

#define	IPC_INCREMENTAL_CSUM(csum, addr, n)	\
		(csum) = ipcreq_incremental_checksum((csum), (addr), (n))
#else	IPC_CSUM
/*
 *	Don't do checksumming...
 */
#define	IPC_INCREMENTAL_CSUM(csum, addr, n)
#endif	IPC_CSUM


#if	IPC_COPY_SEND
/*
 *	copy data to a private buffer before sending?
 *	(just like ipc_ether.c)
 *
 *	XXX SERIOUS NORMA WORKAROUND GOING ON HERE XXX
 */
#endif	IPC_COPY_SEND
/* even if compiled in, can turn off the copy code */
int	ipcreq_copy_send_enabled = 0;


/*
 *	for transfers that are multiples of 4 bytes,
 *	this is the pad word used.
 */
#define	IPC_DANGLE_MAGIC	0xc0da1963


/*
 *	make IPC traces runtime settable.
 */
int _mcmsg_trace_ipc = 0;


/*
 *	additional switches...
 */
int ipcreq_apply_back_pressure = 0;	/* disable rx interrupts? */


/*
 *	Initialization for NORMA IPC to mcmsg interfaces.
 */
mcmsg_ipc_init()
{
	ipcreq_receiving = 0;
	ipcreq_recv_count = 0;
	ipcreq_send_count = 0;
	ipcreq_recv_node = -1;
	ipcreq_rtsqueue_init();
	ipcreq_bootmagic_init();
}


ipcreq_rtsqueue_init()
{
	struct ipc_sender_item	*rts;
	int	i;

	/*
	 *	initialize the free list
	 */
	ipc_rts_free = 0;
	rts = &ipc_rts_cts_table[0];
	for (i = 0; i < MAX_RTS_REQS; i++) {
		rts->ipc_next = ipc_rts_free;
		ipc_rts_free = rts++;
		ipc_rts_free_count++;
	}
}


/*
 *	Override compile-time settings with bootmagic.
 */
ipcreq_bootmagic_init()
{
	/*
	 *	debug tracing?
	 */
	_mcmsg_trace_ipc = getbootint("MCMSG_TRACE_IPC", _mcmsg_trace_ipc);


	/*
	 *	packet size?
	 */
	ipcreq_packet_size = getbootint("IPC_PACKET_SIZE", ipcreq_packet_size);

	/*
	 *	disable interrupts while waiting for NORMA to
	 *	post another receive?
	 */
	ipcreq_apply_back_pressure = getbootint("IPC_APPLY_BACK_PRESSURE",
					ipcreq_apply_back_pressure);


	/*
	 *	copy what NORMA wants transmitted to a private buffer
	 *	and then transmit the copy?
	 */
	ipcreq_copy_send_enabled = getbootint("IPC_COPY_SEND",
					ipcreq_copy_send_enabled);

	/*
	 *	use the 3-trip protocol?  (probably an overall lose
	 *	on small systems, perhaps a small win on large systems)
	 */
	ipcreq_rts_cts = getbootint("IPC_RTS_CTS", ipcreq_rts_cts);

#if	MACH_ASSERT

	/*
	 *	ignore bad checksums?
	 */
	ipcreq_ignore_bad_checksums = getbootint("IPC_IGNORE_BAD_CHECKSUMS",
						ipcreq_ignore_bad_checksums);

	/*
	 *	fail an assertion if there is a bad checksum?
	 */
	ipcreq_stop_on_csum_failure = getbootint("IPC_CSUM_ASSERTS",
						ipcreq_stop_on_csum_failure);
#endif	MACH_ASSERT

}


#if	IPC_COPY_SEND
/*
 *	copy NORMA's data out of the way before
 *	we transmit and transmit the secret copy.
 *
 *	XXX ONLY CORRECT FOR 4K AND 8K VM PAGE SIZES
 */
#define	MAX_PAGE_SIZE	8192
#define	MAX_ADDITIONAL	sizeof(struct netipc_hdr)
#define	MAX_INSURANCE	128				/* LTU alignment */
#define	MAX_IPC_SEND	(MAX_PAGE_SIZE + MAX_ADDITIONAL + MAX_INSURANCE)

static char ipcreq_copy_send_buffer[MAX_IPC_SEND];

#define	ALIGN32(p)	(((unsigned long) (p) + 32) & ~(32 - 1))


/*
 *	this routine has an interesting side effect in that it
 *	re-packetizes the original request, and LTU-favorable
 *	alignment falls out for free...
 */
ipcreq_copy_send(ipcreq)
	register ipcreq_t	*ipcreq;
{
	register char	*bufp;
	register unsigned long	size, addr;
	register struct netvec	*nv;
	register int	vi, vc;
	register char	*head;

	assert(ipcreq_sending == 0);

	vc = ipcreq->count;
	nv = ipcreq->bv;
	bufp = (char *) ALIGN32(&ipcreq_copy_send_buffer[0]);
	head = bufp;

	for (vi = 0; vi < vc; vi++) {
		addr = nv[vi].addr;
		size = nv[vi].size;
		IPC_TRACE_DEBUG("ipc bcopy", 3, addr, bufp, size, 0);
		if ((size >= 4096) &&
		    (((addr | (unsigned long) bufp) & 0xf) == 0)) {
			do {
				/*
				 *	use the slightly lower-cost page copy
				 */
				piped_page_copy((char *) addr, bufp);
				size -= 4096;
				addr += 4096;
				bufp += 4096;
			} while (size >= 4096);
		}
		if (size > 0) {
			bcopy((char *) addr, bufp, size);
			bufp += size;
		}
	}
	assert((bufp - head) < MAX_IPC_SEND);


	/*
	 *	re-write the request as just 1 big netvec and
	 *	handle the packetizing logic in icpreq_send().
	 */
	ipcreq->count = 1;
	ipcreq->bv[0].addr = (unsigned long) head;
	ipcreq->bv[0].size = ipcreq->total;
	IPC_TRACE_DEBUG("ipc bcopy done", 2, head, ipcreq->total, 0, 0);
}
#endif	IPC_COPY_SEND


/*
 *	record NORMA's netvec in our request structure.
 */
ipcreq_record_send(ipcreq, node, vec, count)
	ipcreq_t	*ipcreq;
	int		node;
	struct netvec	*vec;
	int		count;
{
	int	i, vi;
	unsigned long	bytes;

	bytes = 0;
	ipcreq->node = node;
	for (i = 0, vi = 0; i < count; i++) {
		/*
		 *	sometimes NORMA hands us a 0 length
		 *	vector; detect and avert it here.
		 */
		if (vec[i].size > 0) {
			ipcreq->bv[vi].addr = vec[i].addr;
			ipcreq->bv[vi].size = vec[i].size;
			bytes += vec[i].size;
			IPC_TRACE_DEBUG("send setup",
				4, node, vec[i].addr, vec[i].size, i);
			vi++;
		}
	}
	assert(bytes > 0);
	ipcreq->count = vi;
	ipcreq->total = bytes;

#if	IPC_COPY_SEND
	/*
	 *	XXX COPY DATA TO A PRIVATE BUFFER BEFORE NORMA
	 *	XXX GIVES SOMETHING ELSE A CHANCE TO SCRIBBLE
	 *	XXX ON THE PAGE WE'RE TRYING TO TRANSMIT.
	 */
	if (ipcreq_copy_send_enabled) {
		ipcreq_copy_send(ipcreq);
	}
#endif	IPC_COPY_SEND

	ipcreq_send_node = node;
	ipcreq_send_route = calculate_route(node);
	ipcreq_sending = bytes;
	IPC_TRACE_DEBUG("ipc record", 3,
		node, ipcreq_send_route, ipcreq_sending, 0);
}


/*
 *	NORMA IPC interface.
 */
netipc_send(remote, vec, count)
	int	remote;				/* Destination node number */
	register struct netvec *vec;		/* Buffer vector */
	int	count;				/* Buffer vector length */
{
	ipcreq_t	*ipcreq;
	int		method, x;
	unsigned long	item;

	x = spldcm();

	IPC_TRACE_DEBUG("netipc_send", 3, remote, vec, count, 0);

	assert(ipcreq_send_count == 0);
	assert(ipcreq_sending == 0);
	ipcreq_send_count++;

	ipcreq = &ipcreq_send_request;

	/*
	 *	store the netvecs of this request.
	 */
	ipcreq_record_send(ipcreq, remote, vec, count);

	/*
	 *	if the 3-trip protocol is disabled, just
	 *	start sending the data, otherwise get
	 *	clearance from the receiver before pushing
	 *	data onto the wire.
	 */
	if (ipcreq_rts_cts == 0) {
		method = SENDMETH_IPC;
		item = (unsigned long) ipcreq;
	} else {
		method = SENDMETH_IPCQ;
		item = (ipsc_physnode << 16) | ipcreq_sending;
	}
	mcmsg_send(method, item, 0);

	splx(x);
}


/*
 *	Called from mcmsg_send() (SENDMETH_IPC)
 */
ipcreq_send_intr(item, ignored)
	register unsigned long item, ignored;
{
	IPC_TRACE_DEBUG("ipcreq_send_intr", 1, item, 0, 0, 0);
	assert(item == (unsigned long) &ipcreq_send_request);
	ipcreq_send((ipcreq_t *) item);
	return 0;
}


/*
 *	Called from mcmsg_send() (SENDMETH_IPCQ)
 */
mcmsg_send_ipcq(item, ignored)
	register unsigned long item, ignored;
{
	if (_mcmsg_trace_ipc) {
		mcmsg_trace_send(MCTRL_IPCQ, item, ipcreq_send_node, 0, 0, 0);
	}
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
	if (_mcmsg_trace_ipc) {
		mcmsg_trace_recv(hdr1, hdr2, from, 1, ipcreq_recv_posted, 0);
	}

	/*
	 *	if NORMA has posted a receive,
	 *	and if we aren't currently receiving,
	 *	and if there are no outstanding grants,
	 *	and if there are no outstanding requests-to-send,
	 *	ack now.
	 */
	if ((ipcreq_recv_posted != 0) &&	/* norma is waiting */
	    (ipcreq_receiving == 0) &&		/* and not yet receiving */
	    (ipcreq_grants_given == 0) &&	/* and no grants outstanding */
	    (ipc_rts_head == 0)) {		/* and nothing outstanding */
		assert(ipc_rts_count == 0);
		ipcreq_clear_to_send(from, amount);
		return 0;
	}

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
	if (ipc_rts_head == 0) {
		ipc_rts_head = rts;
	} else {
		ipc_rts_tail->ipc_next = rts;
	}
	ipc_rts_tail = rts;
	ipc_rts_count++;
	if (ipc_rts_count > ipc_rts_count_max) {
		ipc_rts_count_max = ipc_rts_count;
	}
	IPC_TRACE_DEBUG("rts queue", 3, from, amount, ipc_rts_count, 0);

	return 0;
}


/*
 *	Called from mcmsg_send() (SENDMETH_IPCA)
 *
 *	"item" is actually two 16-bit values:
 *
 *		item< 0:15> is the amount to ack,
 *		item<16:31> is the node to ack.
 */
mcmsg_send_ipca(item, ignored)
	unsigned long	item, ignored;
{
	register unsigned long	route, to, amount, ack;

	to = (item >> 16);
	amount = item & 0xffff;
	route = calculate_route(to);
	ack = (ipsc_physnode << 16) | amount;

	if (_mcmsg_trace_ipc) {
		mcmsg_trace_send(MCTRL_IPCA, ack, to, 0, 0, 0);
	}
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

	if (_mcmsg_trace_ipc) {
		mcmsg_trace_recv(hdr1, hdr2, hdr2 >> 16, 0, 0, 0);
	}
	assert(ipcreq_sending > 0);
	assert((hdr2 >> 16) == ipcreq_send_request.node);

	/*
	 *	if the grant matches, start sending the IPC.
	 */
	if ((hdr2 & 0xFFFF) == ipcreq_sending) {
		mcmsg_send(SENDMETH_IPC, &ipcreq_send_request, 0);
		return 0;
	}


	/*
	 *	presumably, we only get here if the receiver's
	 *	request-to-send queue is full.  so, retry.
	 *
	 *	XXX we should probably perform a back-off of
	 *	XXX some kind to ease the load on a slow node.
	 */
	mcmsg_send(SENDMETH_IPCQ, (ipsc_physnode << 16) | ipcreq_sending, 0);
	return 0;
}


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
				current_task()->map->pmap->dirbase,
				__FILE__, __LINE__);
	assert(a != 0);

	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
	if (m >= n) {
		mcmsg_fifo_out(a, n);
	} else {
		mcmsg_fifo_out(a, m);
		a = (unsigned long)mcmsg_validate_real(addr + m,
				current_task()->map->pmap->dirbase,
				__FILE__, __LINE__);
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
			current_task()->map->pmap->dirbase,
			__FILE__, __LINE__);
	assert(a != 0);
	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
	if (m >= n) {
		mcmsg_fifo_out_eod(a, n);
	} else {
		mcmsg_fifo_out(a, m);
		a = (unsigned long)mcmsg_validate_real( addr + m,
				current_task()->map->pmap->dirbase,
				__FILE__, __LINE__);
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
			current_task()->map->pmap->dirbase,
			__FILE__, __LINE__);
	assert(a != 0);
	m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
	if (m >= n) {
		mcmsg_fifo_in(a, n);
	} else {
		mcmsg_fifo_in(a, m);
		a = (unsigned long)mcmsg_validate_real( addr + m,
				current_task()->map->pmap->dirbase,
				__FILE__, __LINE__);
		assert(a != 0);
		mcmsg_fifo_in(a, n - m);
	}
	return n;
}


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

	printf("ipc checksum failed: src node=%d computed=%08x received=%08x\n",
		ipcreq_recv_node, computed, received);

	if (ipcreq_stop_on_csum_failure)
		assert(computed == received);
}
#endif	IPC_CSUM


/*
 *	Called from ipcreq_send_intr().
 */
ipcreq_send(ipcreq)
	ipcreq_t *ipcreq;			/* Request block, if any */
{
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

	assert(ipcreq != 0);
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

			if (mcmsg_send_ready() == 0) {
				break;
			}
#if	IPC_CSUM
			csum = ipcreq_compute_checksum(vec, count);
			IPC_TRACE_DEBUG("ipc send checksum", 1, csum, 0, 0, 0);
#endif	IPC_CSUM
			route = ipcreq_send_route;
			tmp = (ipsc_physnode << 16) | ipcreq_sending;
			IPC_TRACE_DEBUG("ipc send 1st",
				3, node, ipsc_physnode, route, 0);
			if (_mcmsg_trace_ipc) {
				mcmsg_trace_send(MCTRL_IPC, tmp, node, 0, 0, 0);
			}
			node = INVALID_NODE;
			send2(route, 0);
			send2(MCTRL_IPC, tmp);
#if	IPC_CSUM
			send2(csum, IPC_CHECKSUM_MAGIC);
#endif	IPC_CSUM
		} else if (nb == IPC_PKT_SIZE) {
			/*
			 *	new packet (2nd or later)
			 */
			if (mcmsg_send_ready() == 0) {
				break;
			}
			tmp = (ipsc_physnode << 16) | ipcreq_sending;
			IPC_TRACE_DEBUG("ipc send 2nd",
				3, node, ipsc_physnode, route, 0);
			if (_mcmsg_trace_ipc) {
				mcmsg_trace_send(MCTRL_IPC | (1 << 16),
					tmp, ipcreq_send_node, 0, 0, 0);
			}
			send2(route, 0);
			send2(MCTRL_IPC | (1 << 16), tmp);
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
		if (size > nb) {
			IPC_TRACE_DEBUG("fifo out (a)", 3, addr, nb, size, 0);
			ipcreq_fifo_out_eod(addr, nb);
			ipcreq_sending -= nb;
			addr += nb;
			size -= nb;
			nb = IPC_PKT_SIZE;
			continue;
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
			IPC_TRACE_DEBUG("ipc send2", 2, a, b, 0, 0);
			send2(a, b);
			ipcreq_sending -= 8;
			nb -= 8;
			continue;
		}

		/*
		 *	pathological case:
		 *	previous netvec had a 4 byte dangle,
		 *	next netvec was of size 4,
		 *	and it was the last netvec.
		 */
		IPC_TRACE_DEBUG("ipc send2eod (b)", 2, a, b, 0, 0);
		send2eod(a, b);
		ipcreq_sending -= 8;
		nb -= 8;
		/*goto send_done; */
send_done:
		assert(ipcreq_sending == 0);
		ipcreq_send_count--;
		assert(ipcreq_send_count == 0);
		IPC_TRACE_DEBUG("ipc send intr", 0, 0, 0, 0, 0);
		netipc_send_intr();
		return 0;
	}

	/*
	 *	cannot send any more at this time.
	 *	reschedule the send.  store the state
	 *	of the not-yet-completed request.
	 */
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
	mcmsg_send(SENDMETH_IPC, ipcreq, 0);
	return 0;
}


/*
 *	send the sender a clear-to-send.
 */
ipcreq_clear_to_send(node, amount)
	long	node, amount;
{
	if (amount > 0) {
		assert(ipcreq_grants_given == 0);
		ipcreq_grants_given++;
	}
	mcmsg_send(SENDMETH_IPCA, (node << 16) | amount, 0);
}


/*
 *	NORMA IPC interface.
 */
netipc_recv(vec, count)
	register struct netvec *vec;		/* Provided buffer vector */
	int count;				/* Buffer vector length */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register int		i;		/* Temp index */
	register int		x;
	register int		bytes;
	struct ipc_sender_item	*rts;

	x = spldcm();

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
	splx(x);
}


/*
 *	flush the current packet in the fifo.
 *	"n" is the number of bytes to flush.
 */
ipcreq_fifo_flush(n)
	register unsigned long	n;
{
	register long		nb = IPC_PKT_SIZE;

	if (ipcreq_rts_cts != 0) {
		extern			mcmsg_console_stopped;
		/*
		 *	if running the 3-trip protocol, we should
		 *	never have to flush the fifo.
		 */
		if (mcmsg_console_stopped != 0) {
			assert(0);
		}
	}
	ipcreq_fifo_flush_count++;
	if (n > nb) {
		n = nb;
	}
	mcmsg_fifo_flush(n);
	assert_mcmsg_eod_last();
}


/*
 *	Called when a NORMA IPC packet arrives in the fifo.
 *
 *	Complications arise when buffers aren't multiples of 8 bytes,
 *	or when a packet crosses an unfavorably aligned vector.
 */
ipcreq_recv_intr(hdr1, length)
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
	extern			mcmsg_console_stopped;

	/*
	 *	1st packet of a Mach IPC?
	 */
	if (FIRST_PACKET(hdr1)) {
		if (_mcmsg_trace_ipc) {
			mcmsg_trace_recv(hdr1, length, FROM_NODE(length),
				1, ipcreq_recv_posted, 0);
		}
#if	IPC_CSUM
		/*
		 *	Get the checksum off the wire...
		 */
		recv2(a, b);
		assert(b == IPC_CHECKSUM_MAGIC);
		IPC_TRACE_DEBUG("ipc recv csum", 2, a, b, 0, 0);
#endif	IPC_CSUM

		/*
		 *	Flush if the console is running.
		 */
		if (mcmsg_console_stopped) {
			mcmsg_trace_debug("ipc flush (cons)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_flushed_console_stopped++;
			ipcreq_fifo_flush(length & 0xFFFF);
			return 0;
		}


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
			return 0;
		}

		/*
		 *	Flush if NORMA hasn't posted a receive.
		 */
		if (ipcreq_recv_posted == 0) {
			mcmsg_ipc_drop++;
			mcmsg_trace_debug("ipc flush (~req)",
				2, length & 0xFFFF, FROM_NODE(length),0, 0);
			ipcreq_fifo_flush(length & 0xFFFF);
			return 0;
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
		if (_mcmsg_trace_ipc) {
			mcmsg_trace_recv(hdr1, length, FROM_NODE(length),
					0, 0, 0);
		}

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
			return 0;
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

			assert_mcmsg_eod_last();
			IPC_TRACE_DEBUG("ipc deliver", 0, 0, 0, 0, 0);

			/* Deliver to kernel */
			if (ipcreq_apply_back_pressure) {
				mcmsg_disable_rx_interrupts();
			}
			netipc_recv_intr();
			return 0;
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
	assert_mcmsg_eod_last();
	return 0;
}


#endif	PARAGON860


/*	--- hypercube implementation starts here ----	*/

#if	iPSC860

#define IPC_CSUM	PARAGON860
#define CKDATA		0
#define IPC_DEBUG	0
#define SHORT_PACKETS	PARAGON860
#define	IPC_BUF		0

#if	SHORT_PACKETS
#define IPC_PKT_SIZE	1024
long	ipcreq_recv_node;
long	ipcreq_send_route;
long	ipcreq_sending;
#endif	SHORT_PACKETS

#if	IPC_CSUM
int		ipcreq_ck_count;
ipcreq_t	ipcreq_save;
#endif	IPC_CSUM

#if	IPC_BUF

#define	IPC_BUF_CHUNK	64
#define IPC_BUF_SIZE	16384

unsigned char	*mcmsg_ipc_buf_start;
unsigned char	*mcmsg_ipc_buf_end;
unsigned char	*mcmsg_ipc_buf_in;
unsigned char	*mcmsg_ipc_buf_out;

unsigned char	mcmsg_ipc_buf_space(IPC_BUF_SIZE + IPC_BUF_CHUNK);

#endif	IPC_BUF;

/*
 * make IPC traces runtime settable.
 */
int _mcmsg_trace_ipc = 0;

mcmsg_ipc_init()
{
	int	i;

	ipcreq_receiving = 0;
	ipcreq_send_head = 0;
	ipcreq_send_tail = 0;
	ipcreq_recv_head = 0;
	ipcreq_recv_tail = 0;
	ipcreq_free_list = 0;
	for (i = 0; i < MAXIPCREQ; i++) {
		ipcreq[i].link = ipcreq_free_list;
		ipcreq_free_list = &ipcreq[i];
	}

	ipcreq_recv_count = 0;
	ipcreq_send_count = 0;
	ipcreq_send_max = 0;
#if	SHORT_PACKETS
	ipcreq_recv_node = -1;
#endif	SHORT_PACKETS
#if	IPC_BUF
	mcmsg_ipc_buf_start = (unsigned char *)
			      (((unsigned long)mcmsg_ipc_buf_space +
			        IPC_BUF_CHUNK - 1) &
			       ~(IPC_BUF_CHUNK - 1));
	mcmsg_ipc_buf_end = &mcmsg_ipc_buf_start[IPC_BUF_SIZE];
	mcmsg_ipc_buf_in = &mcmsg_ipc_buf_start[IPC_BUF_CHUNK - sizeof(long)];
	mcmsg_ipc_buf_out = mcmsg_ipc_buf_in;
#endif	IPC_BUF
}

netipc_send(remote, vec, count)
	int	remote;				/* Destination node number */
	register struct netvec *vec;		/* Buffer vector */
	int	count;				/* Buffer vector length */
{
	int	x;

#if iPSC860
flush(); /* xxx */
#endif iPSC860

	x = spldcm();
	ipcreq_send(remote, vec, count, 0);
	splx(x);
}

ipcreq_send_intr()
{
	ipcreq_t *ipcreq;			/* Request block */

#if iPSC860
flush();	/* xxx */
#endif iPSC860
	ipcreq = ipcreq_send_head;
	ipcreq_send(ipcreq->node, ipcreq->bv, ipcreq->count, ipcreq);
	return 0;
}

#if PARAGON860

ipcreq_fifo_out(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	if ((addr & 7) == 0) {

		a = (unsigned long)mcmsg_validate_real(
					addr,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

		m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
		if (m >= n) {
			mcmsg_fifo_out(a, n);
		} else {
			mcmsg_fifo_out(a, m);

			a = (unsigned long)mcmsg_validate_real(
					addr + m,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

			mcmsg_fifo_out(a, n - m);
		}

	} else {
		mcmsg_fifo_out(addr, n);
	}
}

ipcreq_fifo_out_eod(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	if ((addr & 7) == 0) {

		a = (unsigned long)mcmsg_validate_real(
					addr,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

		m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
		if (m >= n) {
			mcmsg_fifo_out_eod(a, n);
		} else {
			mcmsg_fifo_out(a, m);

			a = (unsigned long)mcmsg_validate_real(
					addr + m,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

			mcmsg_fifo_out_eod(a, n - m);
		}

	} else {
		mcmsg_fifo_out_eod(addr, n);
	}
}

ipcreq_fifo_in(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long a;
	register unsigned long m;

	if ((addr & 7) == 0) {

		a = (unsigned long)mcmsg_validate_real(
					addr,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

		m = INTEL_PGBYTES - (addr & INTEL_OFFMASK);
		if (m >= n) {
			mcmsg_fifo_in(a, n);
		} else {
			mcmsg_fifo_in(a, m);

			a = (unsigned long)mcmsg_validate_real(
					addr + m,
					current_task()->map->pmap->dirbase,
					__FILE__, __LINE__);

			mcmsg_fifo_in(a, n - m);
		}

	} else {
		mcmsg_fifo_in(addr, n);
	}
}
#endif	PARAGON860

ipcreq_send(node, vec, count, ipcreq)
	int	node;				/* Destination node number */
	struct netvec *vec;			/* Buffer vector */
	int	count;				/* Buffer vector length */
	ipcreq_t *ipcreq;			/* Request block, if any */
{
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register int		vi;		/* Vec index */
	register int		i;		/* Temp index */
	register long		r;
	register long		t;
#if	SHORT_PACKETS
	register long		nb;
#endif	SHORT_PACKETS
#if	IPC_CSUM
	register unsigned long	csum;
#endif	IPC_CSUM

	assert(count > 0);
	assert(count <= 100);
	addr = vec[0].addr;
	size = vec[0].size;
	vi = 1;
#if	SHORT_PACKETS
	r = ipcreq_send_route;
	nb = IPC_PKT_SIZE;
#endif	SHORT_PACKETS
	if (ipcreq != 0 || ipcreq_send_head == 0) {
		assert((t = 100) != 0);
#if	SHORT_PACKETS
		for (;;)
#else	SHORT_PACKETS
		while (mcmsg_send_ready())
#endif	SHORT_PACKETS
		{

#if	IPC_DEBUG
mcmsg_trace_debug("send chunk", 4, addr, size,
*(unsigned long *)addr, *(unsigned long *)(addr + size - 4));
#endif	IPC_DEBUG
			assert(t-- != 0);
			if (node != INVALID_NODE) {	/* Send header */
				register struct netvec *v;
				register int n;
				register t;

#if	SHORT_PACKETS
				if (!mcmsg_send_ready()) {
					break;
				}
#endif	SHORT_PACKETS
#if	IPC_CSUM
				csum = 0;
#endif	IPC_CSUM
				for (t = 0, v = vec, n = count;
				     n > 0;
				     v++, n--) {
					t += v->size;
#if	IPC_DEBUG
mcmsg_trace_debug("ipc send piece", 2, v->addr, v->size, 0, 0);
#endif	IPC_DEBUG
#if	IPC_CSUM
{ unsigned long *a = (unsigned long *)(v->addr); long n = v->size;
  assert((n & 3) == 0);
  while (n) {
#if CKDATA
mcmsg_trace_debug("ipc send data", 2, *a, csum, 0, 0);
#endif CKDATA
    csum += *a++;
    n -= 4;
  }
}
#endif	IPC_CSUM
				}
#if	IPC_CSUM
#if	IPC_DEBUG
				mcmsg_trace_debug("ipc send checksum", 1,
						  csum, 0, 0, 0);
#endif	IPC_DEBUG
#endif	IPC_CSUM

				r = calculate_route(node);
#if	SHORT_PACKETS
				ipcreq_sending = t;
				t |= ipsc_physnode << 16;
				ipcreq_send_route = r;
#endif	SHORT_PACKETS

#if TRACE_IPC
#if	IPC_CSUM
				if (_mcmsg_trace_ipc) {
					mcmsg_trace_send(MCTRL_IPC, t, 0, 2, r, csum);
				}
#else	IPC_CSUM
				if (_mcmsg_trace_ipc) {
					mcmsg_trace_send(MCTRL_IPC, t, 0, 2, node, r);
				}
#endif	IPC_CSUM
#endif TRACE_IPC
				send2(r, 0);
				send2(MCTRL_IPC, t);
#if	IPC_CSUM
				send2(csum, 0);
#endif	IPC_CSUM
				node = INVALID_NODE;
#if	SHORT_PACKETS
#else	SHORT_PACKETS
				continue;
#endif	SHORT_PACKETS
			}
#if	SHORT_PACKETS
			else if (nb == IPC_PKT_SIZE) {

#if	SHORT_PACKETS
				if (!mcmsg_send_ready()) {
					break;
				}
#endif	SHORT_PACKETS
				t = ipcreq_sending | (ipsc_physnode << 16);
#if	IPC_DEBUG
mcmsg_trace_debug("ipc cont", 3, node, ipsc_physnode, r, 0);
#endif	IPC_DEBUG
#if TRACE_IPC
				if (_mcmsg_trace_ipc) {
					mcmsg_trace_send(MCTRL_IPC + (1 << 16), t, 0, 0, 0, 0);
				}
#endif TRACE_IPC
				send2(r, 0);
				send2(MCTRL_IPC + (1 << 16), t);
			}
#endif	SHORT_PACKETS
#if	SHORT_PACKETS
			if (size > nb) {

#if	IPC_DEBUG
mcmsg_trace_debug("fifo out", 2, addr, nb, 0, 0);
#endif	IPC_DEBUG
				ipcreq_fifo_out_eod(addr, nb);
				addr += nb;
				size -= nb;
				ipcreq_sending -= nb;
				nb = IPC_PKT_SIZE;
				continue;
			}
#else	SHORT_PACKETS
			if (size > HALF_MSG_FIFO) {	/* Put half FIFO load */
#if	IPC_DEBUG
mcmsg_trace_debug("fifo out", 2, addr, HALF_MSG_FIFO, 0, 0);
#endif	IPC_DEBUG
#if	iPSC860
				mcmsg_fifo_out(addr, HALF_MSG_FIFO);
#else	iPSC860
				ipcreq_fifo_out(addr, HALF_MSG_FIFO);
#endif	iPSC860
				addr += HALF_MSG_FIFO;
				size -= HALF_MSG_FIFO;
				break;
			}
#endif	SHORT_PACKETS
			if (vi < count) {		/* More message */
#if	iPSC860
				mcmsg_fifo_out(addr, size);
				addr = vec[vi].addr;
				size = vec[vi].size;
				vi++;
#else	iPSC860
				if ((size & 7) == 0) {
#if	IPC_DEBUG
mcmsg_trace_debug("fifo out", 2, addr, size, 0, 0);
#endif	IPC_DEBUG
#if	SHORT_PACKETS
					if (size == nb) {
						ipcreq_fifo_out_eod(addr, size);
						nb = IPC_PKT_SIZE;
					} else if (size > 0) {
						ipcreq_fifo_out(addr, size);
						nb -= size;
					}
					ipcreq_sending -= size;
#else	SHORT_PACKETS
					ipcreq_fifo_out(addr, size);
#endif	SHORT_PACKETS
					addr = vec[vi].addr;
					size = vec[vi].size;
					vi++;
				} else {
					register unsigned long	a;
					register unsigned long	b;

#if	IPC_DEBUG
mcmsg_trace_debug("fifo out", 2, addr, size - 4, 0, 0);
#endif	IPC_DEBUG
					ipcreq_fifo_out(addr, size - 4);
#if	SHORT_PACKETS
					nb -= size + 4;
					ipcreq_sending -= size + 4;
#endif	SHORT_PACKETS
					a = *(unsigned long *)
						(addr + size - 4);

					do {
						addr = vec[vi].addr;
						size = vec[vi].size;
						vi++;
					} while (size == 0 && vi < count);
					assert(size != 0);
#if	IPC_DEBUG
mcmsg_trace_debug("split chnk", 3, addr, size, *(unsigned long *)addr, 0);
#endif	IPC_DEBUG

					b = *(unsigned long *)addr;
					addr += 4;
					size -= 4;
					if (size == 0 && vi == count) {
						send2eod(a, b);
						goto send_done;
#if	SHORT_PACKETS
					} else if (nb == 0) {
						send2eod(a, b);
						nb = IPC_PKT_SIZE;
#endif	SHORT_PACKETS
					} else {
						send2(a, b);
					}
				}
#endif	iPSC860
				continue;
			}
#if	IPC_DEBUG
mcmsg_trace_debug("fifo eod", 2, addr, (size + 7) & ~7, 0, 0);
#endif	IPC_DEBUG
#if	iPSC860
			mcmsg_fifo_out_eod(addr, size);
#else	iPSC860
			ipcreq_fifo_out_eod(addr, (size + 7) & ~7);
#endif	iPSC860
send_done:
			if (ipcreq != 0) {
				assert(ipcreq == ipcreq_send_head);

				/* Unlink from send queue */
				ipcreq_send_head = ipcreq->link;

				ipcreq_send_count--;

				/* Put on free list */
				ipcreq->link = ipcreq_free_list;
				ipcreq_free_list = ipcreq;

				/* Do next in send queue */
				if (ipcreq_send_head == 0) {
					ipcreq_send_tail = 0;

					assert(ipcreq_send_count == 0);

				} else {
					ipcreq = ipcreq_send_head;
					vec = ipcreq->bv;
					count = ipcreq->count;
					addr = vec[0].addr;
					size = vec[0].size;
					vi = 1;
					netipc_send_intr();
					continue;
				}
			}
			netipc_send_intr();
			return 0;
		}
	}
	if (ipcreq == 0) {

		/* Get from free list */
		ipcreq = ipcreq_free_list;
		assert(ipcreq != 0);
		ipcreq_free_list = ipcreq->link;
		ipcreq->link = 0;

		/* Put on send queue */
		if (ipcreq_send_head == 0) {
			ipcreq_send_head = ipcreq;
		} else {
			ipcreq_send_tail->link = ipcreq;
		}
		ipcreq_send_tail = ipcreq;

		ipcreq_send_count++;
		if (ipcreq_send_count > ipcreq_send_max)
			ipcreq_send_max = ipcreq_send_count;
	}

	/* Store this request */
	ipcreq->node = node;
	ipcreq->count = count - vi + 1;
	ipcreq->bv[0].addr = addr;
	ipcreq->bv[0].size = size;
	for (i = 1; vi < count; vi++, i++) {
		ipcreq->bv[i] = vec[vi];
	}

#if	PARAGON860
	mcmsg_send(SENDMETH_IPC, ipcreq_send_head, 0);
#else	PARAGON860
	mcmsg_enable_tx_interrupts();
#endif	PARAGON860
}


#if	SHORT_PACKETS
#if	IPC_BUF

mcmsg_ipc_buffer(length, csum)
	register unsigned long	length;
	register unsigned long	csumck;
{
	register unsigned long	avail;
	register unsigned long	remain;
	register unsigned char	*new_in;
#if	IPC_CSUM
	register unsigned long	csum = 0;
	register unsigned long	*cp;
	register unsigned long	cn;
#endif	IPC_CSUM

	if (length > IPC_PKT_SIZE) {
		return 0;
	}
	avail = mcmsg_ipc_buf_out - mcmsg_ipc_buf_in;
	if (avail > IPC_BUF_SIZE) {	/* overflow */
		avail += IPC_BUF_SIZE;
	}
	if (avail < length + sizeof(long)) {
		return 0;
	}
	*(unsigned long *)mcmsg_ipc_buf_in = length;
	remain = mcmsg_ipc_buf_end - (mcmsg_ipc_buf_in + sizeof(long));
	if (remain >= length) {
		ipcreq_fifo_in(mcmsg_ipc_buf_in + sizeof(long), length);
#if	IPC_CSUM
		cp = (unsigned long *)(mcmsg_ipc_buf_in + sizeof(long));
		cn = length;
		assert((cn & 3) == 0);
		while (cn) {
			csum += *cp++;
			cn -= sizeof(long);
		}
#endif	IPC_CSUM
		new_in = mcmsg_ipc_buf_in +
			((length + sizeof(long) + IPC_BUF_CHUNK-1) &
			 ~(IPC_BUF_CHUNK-1));
	} else {
		ipcreq_fifo_in(mcmsg_ipc_buf_in + sizeof(long), remain);
#if	IPC_CSUM
		cp = (unsigned long *)(mcmsg_ipc_buf_in + sizeof(long));
		cn = remain;
		assert((cn & 3) == 0);
		while (cn) {
			csum += *cp++;
			cn -= sizeof(long);
		}
#endif	IPC_CSUM
		ipcreq_fifo_in(mcmsg_ipc_buf_start, length - remain);
#if	IPC_CSUM
		cp = (unsigned long *)(mcmsg_ipc_buf_start);
		cn = length - remain;
		assert((cn & 3) == 0);
		while (cn) {
			csum += *cp++;
			cn -= sizeof(long);
		}
#endif	IPC_CSUM
		new_in = mcmsg_ipc_buf_in - IPC_BUF_SIZE +
			((length + sizeof(long) + IPC_BUF_CHUNK-1) &
			 ~(IPC_BUF_CHUNK-1));
	}
	assert_mcmsg_eod_last();
#if	IPC_CSUM
#if	IPC_DEBUG
mcmsg_trace_debug("ipc buf  checksum", 1, csum, 0, 0, 0);
#endif	IPC_DEBUG
		if (csum != csumck) {
			mcmsg_trace_debug("ipc buf checksum failure", 2,
				csum, csumck, 0, 0);
		 printf("ipc buf checksum failed: len %d sums %08x %08x\n",
				length, csum, csumck);
			ipcreq_ck_count++;
			assert_mcmsg_eod_last();
			return 0;
		}
			assert(csum == csumck);
#endif	IPC_CSUM
	mcmsg_ipc_buf_in = new_in;
	return 1;
}

#endif	IPC_BUF
#endif	SHORT_PACKETS

netipc_recv(vec, count)
	register struct netvec *vec;		/* Provided buffer vector */
	int count;				/* Buffer vector length */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register int		i;		/* Temp index */
	register int		x;

#if iPSC860
flush(); /* xxx */
#endif iPSC860

	x = spldcm();

	assert(count <= 100);
	assert(count >= 2);

	/* Get from free list */
	ipcreq = ipcreq_free_list;
	assert(ipcreq != 0);
	ipcreq_free_list = ipcreq->link;
	ipcreq->link = 0;
	ipcreq_recv_count++;

	/* Put on recv queue */
	if (ipcreq_recv_head == 0) {
		ipcreq_recv_head = ipcreq;
	} else {
		ipcreq_recv_tail->link = ipcreq;
	}
	ipcreq_recv_tail = ipcreq;

	ipcreq->count = count;
	for (i = 0; i < count; i++) {
		ipcreq->bv[i] = vec[i];
	}
	mcmsg_enable_rx_interrupts();
	splx(x);
}

ipcreq_recv_intr(hdr1, length)
	unsigned long	hdr1;			/* First header word */
	register int	length;			/* Remaining message bytes */
{
	register ipcreq_t	*ipcreq;	/* Request block */
	register unsigned long	addr;		/* Current buffer address */
	register unsigned long	size;		/* Current buffer size */
	register int		count;		/* Buffer vector length */
	register int		vi;		/* Vec index */
	register int		i;		/* Temp index */
	register long		t;
	register long		n;
#if	SHORT_PACKETS
	register long		nb = IPC_PKT_SIZE;
#if	IPC_CSUM
	static unsigned long	csum;
	static unsigned long	csumck;
#endif	IPC_CSUM
	extern			mcmsg_console_stopped;

	if (mcmsg_console_stopped) {
		register unsigned long a, b;
#if	IPC_CSUM
		recv2(a, b);
#endif	IPC_CSUM
#if	SHORT_PACKETS
		n = (length + 7) & 0xFFF8;
		if (n > nb) {
			n = nb;
		}
		mcmsg_fifo_flush(n);
#else	SHORT_PACKETS
		mcmsg_fifo_flush((length + 7) & 0xFFF8);
#endif	SHORT_PACKETS
		assert_mcmsg_eod_last();
		return 0;
	}
	if ((hdr1 & (1 << 16)) == 0) {
		register unsigned long a, b;
#if	IPC_CSUM
		recv2(a, b);
		if (_mcmsg_trace_ipc) {
			mcmsg_trace_recv(hdr1, length, 0, 2, ipcreq_recv_head, a);
		}
		if (ipcreq_recv_head != 0) {
			ipcreq_save = *ipcreq_recv_head;
		}
#else	IPC_CSUM
		if (_mcmsg_trace_ipc) {
			mcmsg_trace_recv(hdr1, length, 0, 1, ipcreq_recv_head, 0);
		}
#endif	IPC_CSUM

#if	SHORT_PACKETS
#if	IPC_BUF
		if (ipcreq_recv_node != -1 || ipcreq_recv_head == 0)
#else	IPC_BUF
		if (ipcreq_recv_node != -1)
#endif	IPC_BUF
#else	SHORT_PACKETS
		if (ipcreq_receiving != 0)
#endif	SHORT_PACKETS
		{

#if	IPC_BUF
			if (mcmsg_ipc_buffer(length & 0xFFFF), a) {
				return 0;
			}
			mcmsg_trace_debug("ipc flush, not ready for it", 2,
				  ipcreq_receiving, ipcreq_recv_head, 0, 0);
#else	IPC_BUF
			mcmsg_trace_debug("ipc flush, already receiving",
					  0, 0, 0, 0, 0);
#endif	IPC_BUF
#if	SHORT_PACKETS
			n = (length + 7) & 0xFFF8;
			if (n > nb) {
				n = nb;
			}
			mcmsg_fifo_flush(n);
#else	SHORT_PACKETS
			mcmsg_fifo_flush((length + 7) & 0xFFF8);
#endif	SHORT_PACKETS
			assert_mcmsg_eod_last();
			return 0;
		}
		ipcreq_recv_node = length >> 16;
#if	IPC_CSUM
		csumck = a;
		csum = 0;
#endif	IPC_CSUM
	} else {
		if (_mcmsg_trace_ipc) {
			mcmsg_trace_recv(hdr1, length, 0, 0, 0, 0);
		}
		if (ipcreq_recv_node != (length >> 16)) {
			mcmsg_trace_debug("ipc flush, other node",
					  0, 0, 0, 0, 0);
#if	SHORT_PACKETS
			n = (length + 7) & 0xFFF8;
			if (n > nb) {
				n = nb;
			}
			mcmsg_fifo_flush(n);
#else	SHORT_PACKETS
			mcmsg_fifo_flush((length + 7) & 0xFFF8);
#endif	SHORT_PACKETS
			assert_mcmsg_eod_last();
			return 0;
		}
	}
	length &= 0xFFFF;
#endif	SHORT_PACKETS

#if iPSC860
flush(); /* xxx */
#endif iPSC860

	/* Get from recv queue */
	ipcreq = ipcreq_recv_head;

	/* If no IPC request, drop it */
	if (ipcreq == 0) {
		mcmsg_ipc_drop++;
#if TRACE_IPC
		mcmsg_trace_debug("ipc flush, no request",
				  1, (length + 7) & ~7, 0, 0, 0);
#endif TRACE_IPC
#if	iPSC860
		mcmsg_fifo_flush(length);
#else	iPSC860
#if	SHORT_PACKETS
		n = (length + 7) & ~7;
		if (n > nb) {
			n = nb;
		}
		mcmsg_fifo_flush(n);
#else	SHORT_PACKETS
		mcmsg_fifo_flush((length + 7) & ~7);
#endif	SHORT_PACKETS
#endif	iPSC860
		assert_mcmsg_eod_last();
		return 0;
	}

	count = ipcreq->count;
	addr = ipcreq->bv[0].addr;
	size = ipcreq->bv[0].size;
	vi = 1;

	assert((t = MAXLOOP) != 0);
#if	SHORT_PACKETS
	while (nb > 0)
#else	SHORT_PACKETS
	while (mcmsg_recv_ready())
#endif	SHORT_PACKETS
	{

		assert(t-- != 0);
		assert(length > 0);
		n = length;
#if	SHORT_PACKETS
		if (n > nb) {
			n = nb;
		}
#else	SHORT_PACKETS
		if (n > HALF_MSG_FIFO) {
			n = HALF_MSG_FIFO;
		}
#endif	SHORT_PACKETS

#if	iPSC860
		if (n > size) {
			n = size;
		}

		mcmsg_fifo_in(addr, n);
		size -= n;
		if (size == 0 && length > n) {
			assert(vi < count);

			addr = ipcreq->bv[vi].addr;
			size = ipcreq->bv[vi].size;
			vi++;
		} else {
			addr += n;
		}
#else	iPSC860
		if (n > ((size + 7) & ~7)) {
			n = ((size + 7) & ~7);
		}

		if (n <= size && (n & 7) == 0) {
#if	IPC_DEBUG
mcmsg_trace_debug("fifo in", 2, addr, n, 0, 0);
#endif	IPC_DEBUG
			ipcreq_fifo_in(addr, n);
#if	IPC_DEBUG
mcmsg_trace_debug("fifo in done", 0, 0, 0, 0, 0);
#endif	IPC_DEBUG
			size -= n;
			if (size == 0) {

				if(vi < count) {

					addr = ipcreq->bv[vi].addr;
					size = ipcreq->bv[vi].size;
					vi++;

				} else {
					assert(n == length);
				}

			} else {
				addr += n;
			}

		} else {
			unsigned long t1, t2;

			if (n > size) {

#if	IPC_DEBUG
mcmsg_trace_debug("fifo in", 4, addr, n - 8, size, n);
#endif	IPC_DEBUG
				assert(vi < count);
				assert((n & 7) == 0);
				assert(n - size == 4);

				ipcreq_fifo_in(addr, n - 8);
#if	IPC_DEBUG
mcmsg_trace_debug("fifo in done", 0, 0, 0, 0, 0);
#endif	IPC_DEBUG
#if 0
{ unsigned long *a; int i;
  a = (unsigned long *)ipcreq->bv[0].addr;
#if 0
  assert(a[0] != 0xabcdef00 || a[1] != 0xf0);
#else
  if(a[0] == 0xabcdef00 && a[1] == 0xf0) {
    for (i = 1000000; i; i--);
  }
#endif
}
#endif
				recv2(t1, t2);
#if	IPC_DEBUG
mcmsg_trace_debug("crossed", 0, 0, 0, 0, 0);
#endif	IPC_DEBUG
				*(unsigned long *)(addr + n - 8) = t1;

				addr = ipcreq->bv[vi].addr;
				size = ipcreq->bv[vi].size - 4;
				*(unsigned long *)addr = t2;
				addr += 4;
				vi++;

			} else {

#if	IPC_DEBUG
mcmsg_trace_debug("fifo in", 3, addr, n & ~7, n, 0);
#endif	IPC_DEBUG
				assert((n & 7) == 4);
				ipcreq_fifo_in(addr, n & ~7);
#if	IPC_DEBUG
mcmsg_trace_debug("fifo in done", 0, 0, 0, 0, 0);
#endif	IPC_DEBUG
				recv2(t1, t2);
				*(unsigned long *)(addr + (n & ~7)) = t1;
#if	IPC_DEBUG
mcmsg_trace_debug("dumped", 1, t2, 0, 0, 0);
#endif	IPC_DEBUG
				size -= n;

			}
		}
#if	IPC_DEBUG
mcmsg_trace_debug("end loop", 2, n, length, 0, 0);
#endif	IPC_DEBUG

#endif	iPSC860


#if	SHORT_PACKETS
		nb -= n;
#endif	SHORT_PACKETS
		length -= n;
		if (length == 0) {
#if	IPC_CSUM
{ int i; unsigned long *a; long n;
  for (i = 0; i < vi-1; i++) {
    n = ipcreq->bv[i].size;
    a = (unsigned long *)ipcreq->bv[i].addr;
    assert((n & 3) == 0);
    while (n) {
#if CKDATA
mcmsg_trace_debug("ipc recv data", 2, *a, csum, 0, 0);
#endif CKDATA
      csum += *a++;
      n -= 4;
    }
  }
  n = ipcreq->bv[vi-1].size - size;
  a = (unsigned long *)ipcreq->bv[vi-1].addr;
  assert((n & 3) == 0);
  while (n) {
#if CKDATA
mcmsg_trace_debug("ipc recv data", 2, *a, csum, 0, 0);
#endif CKDATA
      csum += *a++;
      n -= 4;
  }
}
#if	IPC_DEBUG
mcmsg_trace_debug("ipc recv checksum", 1, csum, 0, 0, 0);
#endif	IPC_DEBUG
			if (csum != csumck) {
				mcmsg_trace_debug("ipc checksum failure", 2,
					csum, csumck, 0, 0);
			 printf("ipc checksum failed: len %d sums %08x %08x\n",
					length, csum, csumck);
				ipcreq_ck_count++;
#if 0
				*ipcreq_recv_head = ipcreq_save;
				assert_mcmsg_eod_last();
				return 0;
#endif
			}
			assert(csum == csumck);
#endif	IPC_CSUM

			/* Take from recv queue */
			ipcreq_recv_head = ipcreq->link;
			if (ipcreq->link == 0) {
				ipcreq_recv_tail = 0;
				mcmsg_disable_rx_interrupts();
			}
			ipcreq_receiving = 0;
#if	SHORT_PACKETS
			ipcreq_recv_node = -1;
#endif	SHORT_PACKETS
			ipcreq_recv_count--;

			/* if count non-zero, queue better not be empty */
			if (ipcreq_recv_count > 0 && 
			    ipcreq_recv_head == 0) {
				assert(0);
			}

			/* Put on free list */
			ipcreq->link = ipcreq_free_list;
			ipcreq_free_list = ipcreq;

			assert_mcmsg_eod_last();
#if	IPC_DEBUG
mcmsg_trace_debug("ipc deliver", 0, 0, 0, 0, 0);
#endif	IPC_DEBUG

			/* Deliver to kernel */
			netipc_recv_intr();
			return 0;
		}
	}

	/* Store remaining request */
#if	IPC_DEBUG
mcmsg_trace_debug("ipc recv store", 3, length, vi, count, 0);
#endif	IPC_DEBUG
	assert(count <= 100);
#if	SHORT_PACKETS
	ipcreq_receiving = 0;
#else	SHORT_PACKETS
	ipcreq_receiving = length;
#endif	SHORT_PACKETS
#if	IPC_CSUM
{ int i; unsigned long *a; long n;
  for (i = 0; i < vi-1; i++) {
    n = ipcreq->bv[i].size;
    a = (unsigned long *)ipcreq->bv[i].addr;
    assert((n & 3) == 0);
    while (n) {
#if CKDATA
mcmsg_trace_debug("ipc recv data", 2, *a, csum, 0, 0);
#endif CKDATA
      csum += *a++;
      n -= 4;
    }
  }
  n = ipcreq->bv[vi-1].size - size;
  a = (unsigned long *)ipcreq->bv[vi-1].addr;
  assert((n & 3) == 0);
  while (n) {
#if CKDATA
mcmsg_trace_debug("ipc recv data", 2, *a, csum, 0, 0);
#endif CKDATA
    csum += *a++;
    n -= 4;
  }
}
#endif	IPC_CSUM
	ipcreq->count = count - vi + 1;
	ipcreq->bv[0].addr = addr;
	ipcreq->bv[0].size = size;
	if (vi > 1) {
		for (i = 1; vi < count; vi++, i++) {
			ipcreq->bv[i] = ipcreq->bv[vi];
		}
	}
#if	SHORT_PACKETS
	assert_mcmsg_eod_last();
#endif	SHORT_PACKETS
	return 0;
}

#endif	iPSC860		/* end of hypercube stuff */
