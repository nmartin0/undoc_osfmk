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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hws1.c,v 1.21 1994/11/18 20:46:57 mtm Exp $
 */

/*
 * msgp_hws1.c
 *
 * Hardware support routines for multicomputer message passing
 */

#include <i860paragon/msgp/msgp_hw.h>

void	mcmsg_complete_send(int);	/* forward */
int	mcmsg_pkt2_abort;

#if    BIGPKTS
/*
 *  Global variables for concurrent send/recv
 *  mcmsg_send_in_progress: boolean
 *  mcmsg_send_addr_remain: address of send in progress
 *  mcmsg_send_count_remain: count of send in progress
 *
 *  mcmsg_send_waiting: boolean
 *  mcmsg_recv_waiting: boolean
 *
 *	mcmsg_local_send:	boolean (is current send to local node)
 */
int           mcmsg_send_in_progress = 0;
int           mcmsg_send_count_remain = 0;
unsigned long mcmsg_send_addr_remain = 0;

int           mcmsg_send_waiting = 0;
int           mcmsg_recv_waiting = 0;

int           mcmsg_local_send = 0;
#endif BIGPKTS


#if	HANDCODE && !BIGPKTS
/* See msgp_hw.s */
#else	HANDCODE && !BIGPKTS

/*
 * Fifo transfer code
 *
 * The following procedures copy between a user buffer and the NIC FIFOs.
 * The top level procedures (mcmsg_recv_buf, mcmsg_send_buf) have no
 * limitations on buffer size or alignment of the user buffer except that
 * the amount of data transferred in one call cannot exceed the page size.
 *
 * The arguments consist of two buffer pointers and a byte count.
 * The first buffer pointer is the validated physical address of the buffer.
 * The second buffer pointer is the validated physical address of the
 * last byte to be copied in the buffer.
 * If the buffer does not cross a page, bp2 = bp1 + pkt - 1.
 * Otherwise, the high bits of bp1 are the address of the first page
 * and the high bits of bp2 are the address of the second page.
 * The byte count must be non-zero.
 * For send, the byte count must have been rounded up to PKT_GRAN.
 */

/*
 * Routine:
 *	mcmsg_send_pkt2(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2)
 *
 * Arguments:
 *	mt:	mcmsg_task pointer
 *	dummy:	unused
 *	bp1:	Physical address of buffer
 *	bp2:	Physical address of last byte
 *	pkt:	Byte count (unrounded)
 *	route:	HW route
 *	hdr1:	Header word
 *	hdr2:	Header word
 *
 * Purpose:
 *	Copies from a buffer to the NIC transmit FIFO
 *	Assumes data is the last part of the packet so it appends EOD
 *
 * Returns:
 *	none
 */

mcmsg_send_pkt2(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2)
	mcmsg_task_t	*mt;
	register unsigned long  dummy;
	register unsigned long  bp1;
	register unsigned long  bp2;
	register unsigned long  pkt;
	register unsigned long  route;
	register unsigned long  hdr1;
	register unsigned long  hdr2;
{

	/*
	 * Send header
	 */

	send2_now(route, 0);
	send2_now(hdr1, hdr2);

#if	BIGPKTS && BUMPERS

	send2_now(0, 0);
	send2_now(0, 0);
	send2_now(0, 0);
	send2_now(0, 0);

#endif	BIGPKTS && BUMPERS

	/*
	 * Send data
	 */

	mcmsg_send_buf(bp1, bp2, (pkt + (FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1));
}

/*
 * Routine:
 *	mcmsg_send_pkt4(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2, hdr3, hdr4)
 *
 * Arguments:
 *	mt:	mcmsg_task pointer
 *	dummy:	unused
 *	bp1:	Physical address of buffer
 *	bp2:	Physical address of last byte
 *	pkt:	Byte count (unrounded)
 *	route:	HW route
 *	hdr1:	Header word
 *	hdr2:	Header word
 *	hdr3:	Header word
 *	hdr4:	Header word
 *
 * Purpose:
 *	Copies from a buffer to the NIC transmit FIFO
 *	Assumes data is the last part of the packet so it appends EOD
 *
 * Returns:
 *	none
 */

#if	BIGPKTS

mcmsg_send_pkt4(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2, hdr3, hdr4)
	mcmsg_task_t	*mt;
	register unsigned long  dummy;
	register unsigned long  bp1;
	register unsigned long  bp2;
	register unsigned long  pkt;
	register unsigned long  route;
	register unsigned long  hdr1;
	register unsigned long  hdr2;
	register unsigned long  hdr3;
	register unsigned long  hdr4;
{

	/*
	 * Send header
	 */

	send2_now(route, 0);
	send2_now(hdr1, hdr2);
	send2_now(hdr3, hdr4);
	send2_now(0, 0);

#if	BUMPERS

	send2_now(0, 0);
	send2_now(0, 0);

#endif	BUMPERS

	/*
	 * Send data
	 */

	mcmsg_send_buf(bp1, bp2, (pkt + (FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1));
}

#else	BIGPKTS

mcmsg_send_pkt4(mt, dummy, bp1, bp2, pkt, route, hdr1, hdr2, hdr3, hdr4)
	mcmsg_task_t	*mt;
	register unsigned long  dummy;
	register unsigned long  bp1;
	register unsigned long  bp2;
	register unsigned long  pkt;
	register unsigned long  route;
	register unsigned long  hdr1;
	register unsigned long  hdr2;
	register unsigned long  hdr3;
	register unsigned long  hdr4;
{
	register unsigned long  t1;
	register unsigned long  t2;
	register unsigned long  n;
	nic_reg t;
	register double x;
	register double y;
	register unsigned long la, ln;
	long pktm1;


	/*
	 * Round pkt up to 8-byte boundary
	 */

	assert((pktm1 = pkt - 1) >= 0);
	pkt = (pkt + (FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1);

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & ~(MSG_PAGE_SIZE-1);
	t2 = bp2 & ~(MSG_PAGE_SIZE-1);

	/*
	 * Send header
	 */

	t.halfs.lo = route;
	NIC.io.full = t.full;

	t.halfs.lo = hdr1;
	t.halfs.hi = hdr2;
	NIC.io.full = t.full;

	t.halfs.lo = hdr3;
	t.halfs.hi = hdr4;
	NIC.io.full = t.full;

	NIC.io.full = t.full;

	if (t1 == t2) {

		/*
		 * No page crossing, copy it all at once
		 */

		assert(bp2 == bp1 + pktm1);
		MCMSG_TRACE_FIFO("outeod  all", 2, bp1, pkt, 0, 0);
		if (mcmsg_ltu_enable &&
		    (pkt & (LTU_ALIGN-1)) == 0 &&
		    pkt >= LTU_MIN && (bp1 & (LTU_ALIGN-1)) == 0) {

			assert((inl(DP_CONTROL1_READ) &
				(1 << DP_IMSK1_LTU1_CNT)) == 0);

			/*
			 * Start LTU1
			 */

#if BUMPERS
			ltu_send_start(bp1, pkt);
#else BUMPERS
			ltu_send_start_eod(bp1, pkt);
#endif BUMPERS

			ltu_send_wait();

			MCMSG_BUMPER_OUT;
			RELEASE_TX_FIFO;


		} else {
			mcmsg_fifo_out_eod(bp1, pkt);
		}
	} else {

		/*
		 * Crosses a page, copy first page then second page
		 *
		 * t1 is the byte count for the first page
		 */

		t1 = t1 + MSG_PAGE_SIZE - bp1;
		assert(t1 < pkt);

		if (mcmsg_ltu_enable &&
		    (pkt & (LTU_ALIGN-1)) == 0 &&
		    t1 >= LTU_MIN && pkt - t1 >= LTU_MIN &&
		    (bp1 & (LTU_ALIGN-1)) == 0) {

			/*
			 * Send 1st page.
			 */

			ltu_send_start(bp1, t1);
			ltu_send_wait();
			
			/*
			 * Send 2nd page.
			 */

#if BUMPERS
			ltu_send_start(t2, pkt-t1);
#else BUMPERS
			ltu_send_start_eod(t2, pkt-t1);
#endif BUMPERS
			ltu_send_wait();

			MCMSG_BUMPER_OUT;
			RELEASE_TX_FIFO;

		} else {
			mcmsg_send_buf(bp1, bp2, pkt);
		}
	}
}
#endif	BIGPKTS
#endif	HANDCODE && !BIGPKTS

/*
 * Routine:
 *	mcmsg_send_hdr2
 *
 * Arguments:
 *	route
 *	h1 - h2		header words
 *
 * Purpose:
 *	Copies a header consisting of 2 elements to the FIFO. (no EOD) 
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr2(route, h1, h2)
unsigned long route;
unsigned long h1, h2;
{
	send2_now(route, 0);
	send2_now(h1, h2);
#if	BIGPKTS && BUMPERS
	send2_now(0, 0);
	send2_now(0, 0);
	send2_now(0, 0);
	send2_now(0, 0);
#endif	BIGPKTS && BUMPERS
}
/*
 * Routine:
 *	mcmsg_send_hdr2_eod
 *
 * Arguments:
 *	route
 *	h1 - h2		header words
 *
 * Purpose:
 *	Copies a header consisting of 2 elements to the FIFO with EOD.
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr2_eod(route, h1, h2)
unsigned long route;
unsigned long h1, h2;
{
	send2_now(route, 0);
#if	BIGPKTS && BUMPERS
	send2_now(h1, h2);
	send2_now(0, 0);
	send2_now(0, 0);
	send2_now(0, 0);
	send2eod_now(0, 0);
#else	BIGPKTS && BUMPERS
	send2eod_now(h1, h2);
#endif	BIGPKTS && BUMPERS
}
/*
 * Routine:
 *	mcmsg_send_hdr4
 *
 * Arguments:
 *	route
 *	h1 - h4		header words
 *
 * Purpose:
 *	Copies a header consisting of 4 elements to the FIFO. (no EOD) 
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr4(route, h1, h2, h3, h4)
unsigned long route;
unsigned long h1, h2, h3, h4;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(0, 0);
#if	BIGPKTS && BUMPERS
	send2_now(0, 0);
	send2_now(0, 0);
#endif	BIGPKTS && BUMPERS
}
/*
 * Routine:
 *	mcmsg_send_hdr4_eod
 *
 * Arguments:
 *	route
 *	h1 - h4		header words
 *
 * Purpose:
 *	Copies a header consisting of 4 elements to the FIFO with EOD.
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr4_eod(route, h1, h2, h3, h4)
unsigned long route;
unsigned long h1, h2, h3, h4;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
#if	BIGPKTS && BUMPERS
	send2_now(0, 0);
	send2_now(0, 0);
#endif	BIGPKTS && BUMPERS
	send2eod_now(0, 0);
}
/*
 * Routine:
 *	mcmsg_send_hdr6
 *
 * Arguments:
 *	route
 *	h1 - h6		header words
 *
 * Purpose:
 *	Copies a header consisting of 6 elements to the FIFO. (no EOD) 
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr6(route, h1, h2, h3, h4, h5, h6)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(h5, h6);
#if	BIGPKTS && BUMPERS
	send2_now(0, 0);
	send2_now(0, 0);
#endif	BIGPKTS && BUMPERS
}
/*
 * Routine:
 *	mcmsg_send_hdr6_eod
 *
 * Arguments:
 *	route
 *	h1 - h6		header words
 *
 * Purpose:
 *	Copies a header consisting of 6 elements to the FIFO with EOD.
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr6_eod(route, h1, h2, h3, h4, h5, h6)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
#if	BIGPKTS && BUMPERS
	send2_now(h5, h6);
	send2_now(0, 0);
	send2eod_now(0, 0);
#else	BIGPKTS && BUMPERS
	send2eod_now(h5, h6);
#endif	BIGPKTS && BUMPERS
}
/*
 * Routine:
 *	mcmsg_send_hdr8
 *
 * Arguments:
 *	route
 *	h1 - h8		header words
 *
 * Purpose:
 *	Copies a header consisting of 8 elements to the FIFO. (no EOD) 
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr8(route, h1, h2, h3, h4, h5, h6, h7, h8)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6, h7, h8;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(h5, h6);
	send2_now(h7, h8);
	send2_now(0, 0);
}
/*
 * Routine:
 *	mcmsg_send_hdr8_eod
 *
 * Arguments:
 *	route
 *	h1 - h8		header words
 *
 * Purpose:
 *	Copies a header consisting of 8 elements to the FIFO with EOD.
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr8_eod(route, h1, h2, h3, h4, h5, h6, h7, h8)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6, h7, h8;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(h5, h6);
	send2_now(h7, h8);
	send2eod_now(0, 0);
}

#if     HANDCODE
/* See msgp_hw.s */
#else   HANDCODE

/*
 * Routine:
 *	mcmsg_send_hdr10
 *
 * Arguments:
 *	route
 *	hdr1 - hdr10	header words
 *
 * Purpose:
 *	Copies a header consisting of 10 elements to the FIFO. (no EOD) 
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr10(route, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6, h7, h8, h9, h10;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(h5, h6);
	send2_now(h7, h8);
	send2_now(h9, h10);
}
/*
 * Routine:
 *	mcmsg_send_hdr10_eod
 *
 * Arguments:
 *	route
 *	hdr1 - hdr10	header words
 *
 * Purpose:
 *	Copies a header consisting of 10 elements to the FIFO with EOD.
 *	No data is sent.
 *
 * Returns:
 *	none
 */
mcmsg_send_hdr10_eod(route, h1, h2, h3, h4, h5, h6, h7, h8, h9, h10)
unsigned long route;
unsigned long h1, h2, h3, h4, h5, h6, h7, h8, h9, h10;
{
	send2_now(route, 0);
	send2_now(h1, h2);
	send2_now(h3, h4);
	send2_now(h5, h6);
	send2_now(h7, h8);
	send2eod_now(h9, h10);
}
#endif	HANDCODE

#if BIGPKTS

/*
 * Routine:
 *	mcmsg_send_buf(bp1, bp2, pkt)	BIGPKT version.
 *
 * Arguments:
 *	bp1:		Physical address of buffer
 *	bp2:		Physical address of last byte
 *	pkt:		Byte count
 *
 * Purpose:
 *	Copies from a buffer to the NIC transmit FIFO
 *	Assumes data is the last part of the packet so it appends EOD
 *
 *	BIGPKT version:
 *	This version deals with a packet send as at least two operations,
 *	beginning and completion. It attempts to make the length of the
 *	beginning of the packet an even multiple of LTU_ALIGN so that
 *	when possible, the LTU can be utilized. Upon return, it checks
 *	for possible asynchronous completion of the message it started.
 *
 *	NOTE: this version relies on the fact that mcmsg_fifo_out() 
 *	will do the necessary alignment (and copying) if an LTU xfer 
 *	is possible. It ensures that the last xfer done by mcmsg_fifo_out()
 *	is an LTU xfer, that is, the buffer send ends on a cache line.
 *
 * Returns:
 *	none
 */
mcmsg_send_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	pkt_out;
	register unsigned long	t1;
	register unsigned long	t2;
	register long	bcount;
	register long 	count1, count2;

mcmsg_trace_debug("send buf", 3, bp1, bp2, pkt, 0);

	pkt_out = (pkt + (LTU_ALIGN-1)) & ~(LTU_ALIGN-1);

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & MSG_PAGE_MASK;
	t2 = bp2 & MSG_PAGE_MASK;

	if (t1 == t2) {
		/*
		 * No page crossing.
		 */

		if (t1 == ((bp1 + pkt_out - 1) & MSG_PAGE_MASK)) {

			if ((bp1 & (FIFO_ALIGN-1)) == 0) {
				/*
				 * if fifo aligned, ensure that last
				 * transfer is LTU.
				 */
				count1 = ((bp1 + (LTU_ALIGN-1)) & ~(LTU_ALIGN-1)) - bp1;
				count2 = (pkt_out - count1) & ~(LTU_ALIGN-1);
				bcount = count1 + count2;
				mcmsg_send_count_remain = pkt_out - bcount;
			} else {
				/*
				 * if poorly aligned, ensure count is
				 * multiple of LTU_ALIGN, because the data
				 * will be copied into an aligned buffer.
				 */
				mcmsg_send_count_remain = pkt_out & (LTU_ALIGN-1);
				bcount = pkt_out - mcmsg_send_count_remain;
			}
		} else {
			/* Rounding up the pkt size to a cache line puts us past
			 * end-of-page.  Conditions under which this occurs :
			 *  Buffer doesn't begin on a cache line.
			 *  Buffer ends within last cache line of the page.
			 *
			 * The buffer must be copied to send multiple of cache line bytes.
			 */
			bcopy(bp1, mcmsg_pbuf_out, pkt);
			bp1 = (unsigned long) mcmsg_pbuf_out;
			bcount = pkt_out;
			mcmsg_send_count_remain = 0;
		}

		if (bcount < LTU_MIN) {
			bcount = (pkt_out + (FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1);
			mcmsg_fifo_out_eod(bp1, bcount);
		} else {
#if    BUMPERS
			mcmsg_send_addr_remain = bp1 + bcount;
			mcmsg_send_in_progress = 1;

			mcmsg_fifo_out(bp1, bcount);

			if (mcmsg_send_in_progress) {
				mcmsg_complete_send(0);
			}
#else  BUMPERS
			/*
			 * Without bumpers, complete_send() is not required.
			 */
			if (mcmsg_send_count_remain > 0) {
				mcmsg_send_addr_remain = bp1 + bcount;
				mcmsg_send_in_progress = 1;

				mcmsg_fifo_out(bp1, bcount);

				if (mcmsg_send_in_progress) {
					mcmsg_complete_send(0);
				}
			} else {
				mcmsg_fifo_out_eod(bp1, bcount);
			}
#endif BUMPERS
		}

	} else {

		/*
		 * Crosses a page.
		 *
		 * count1 is length in first page
		 */

		count1 = t1 + MSG_PAGE_SIZE - bp1;
		count2 = pkt_out - count1;

		assert(count1 < pkt_out);
		assert(count2 < pkt_out);

		if (((count1 & (FIFO_ALIGN-1)) == 0) && 
			(mcmsg_local_send == 0)) {

			/*
			 * Page crossing is well aligned
			 */

			mcmsg_send_count_remain = count2;
			mcmsg_send_addr_remain = t2;
			mcmsg_send_in_progress = 1;

			mcmsg_fifo_out(bp1, count1);

			if (mcmsg_send_in_progress) {
				mcmsg_complete_send(0);
			}

		} else {

			/*
			 * Page crossing is poorly aligned.
			 * Bcopy to temporary buffer then send.
			 */

			bcopy(bp1, mcmsg_pbuf_out, count1);
			bcopy(t2, ((unsigned long)mcmsg_pbuf_out)+count1, count2);

			mcmsg_send_count_remain = pkt_out & (LTU_ALIGN-1);
#if    BUMPERS
			bcount = pkt_out - mcmsg_send_count_remain;
			mcmsg_send_addr_remain = (unsigned long)mcmsg_pbuf_out + bcount;
			mcmsg_send_in_progress = 1;

			mcmsg_fifo_out(mcmsg_pbuf_out, bcount);

			if (mcmsg_send_in_progress) {
				mcmsg_complete_send(0);
			}
#else  BUMPERS
			/*
			 * Without bumpers, complete_send() is not required.
			 */
			if (mcmsg_send_count_remain == 0) {
				mcmsg_fifo_out_eod(mcmsg_pbuf_out, pkt_out);
			} else {
				bcount = pkt_out - mcmsg_send_count_remain;
				mcmsg_send_addr_remain = (unsigned long)mcmsg_pbuf_out + bcount;
				mcmsg_send_in_progress = 1;

				mcmsg_fifo_out(mcmsg_pbuf_out, bcount);

				if (mcmsg_send_in_progress) {
					mcmsg_complete_send(0);
				}
			}
#endif BUMPERS
		}
	}
	return;
}


/*
 * Routine:
 *	mcmsg_complete_send(int escape)
 *
 * Arguments:
 *	escape	0 -- stay here till done
 *		1 -- escape after 1 loop.
 *
 * Purpose:
 *	Completes a send that has been started.
 *	Clears mcmsg_send_in_progress global flag if able.
 *
 * Returns:
 *	none
 */

/*
 * FIFO_MAX -- XXX tune and move to mcmsg_hw.h
 * FIFO_ALIGNED, >= LTU_MIN, smaller than whatever
 * length will throttle an LTU to an empty FIFO
 */
#define	FIFO_MAX	(1024+512)	/* 1.5KB */

void
mcmsg_complete_send(int escape)
{
register unsigned int bcount;
register unsigned int baddr;
int t;

	assert((t = MAXLOOP) != 0);
	while (mcmsg_send_in_progress) {
		if (mcmsg_send_count_remain >= LTU_MIN) {
			/*
			 * Do another sizable chunk.
			 */
			if (escape && (mcmsg_send_count_remain >= FIFO_MAX))
				bcount = FIFO_MAX;
			else
				bcount = mcmsg_send_count_remain & ~(LTU_ALIGN-1);

			mcmsg_send_count_remain -= bcount;
			baddr  = mcmsg_send_addr_remain;
#if    BUMPERS
			mcmsg_send_addr_remain += bcount;
			mcmsg_trace_debug("complete send more", 2, baddr, bcount, 0, 0);
			mcmsg_fifo_out(baddr, bcount);
#else  BUMPERS
			if (mcmsg_send_count_remain == 0) {
				mcmsg_send_in_progress = 0;
				mcmsg_trace_debug("complete send end", 2, baddr, bcount, 0, 0);
				mcmsg_fifo_out_eod(baddr, bcount);
			} else {
				mcmsg_send_addr_remain += bcount;
				mcmsg_trace_debug("complete send more", 2, baddr, bcount, 0, 0);
				mcmsg_fifo_out(baddr, bcount);
			}
#endif BUMPERS
	
		} else if (mcmsg_send_count_remain > 0) {
	
			/*
			 * Last small chunk remaining.
			 */
			mcmsg_trace_debug("complete send end", 2, 
				mcmsg_send_addr_remain, mcmsg_send_count_remain, 0, 0);
	
			/*
			 * Send rest and clear send_in_progress
			 */
			bcount = (mcmsg_send_count_remain+(FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1);
			mcmsg_fifo_out_eod(mcmsg_send_addr_remain, bcount);
			mcmsg_send_in_progress = 0;
		} else {
#if    BUMPERS	
			mcmsg_trace_debug("complete send end", 1, 0, 0, 0, 0);
	
			/*
			 * Done: Clear send_in_progress.
			 */
			MCMSG_BUMPER_OUT;
			mcmsg_send_in_progress = 0;
#else  BUMPERS	
			/* Should not complete 0 length with no bumpers. */
			assert(0);
#endif BUMPERS	
		}
		assert(t-- != 0);

		if (escape)
			break;
	} /* end while */
	return;
}

#else BIGPKTS

/*
 * Routine:
 *	mcmsg_send_buf(bp1, bp2, pkt)
 *
 * Arguments:
 *	bp1:		Physical address of buffer
 *	bp2:		Physical address of last byte
 *	pkt:		Byte count
 *
 * Purpose:
 *	Copies from a buffer to the NIC transmit FIFO
 *	Assumes data is the last part of the packet so it appends EOD
 *
 * Returns:
 *	none
 */

mcmsg_send_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	pkt_out;
	register unsigned long	t1;
	register unsigned long	t2;

	pkt_out = (pkt + (FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1);

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & MSG_PAGE_MASK;
	t2 = bp2 & MSG_PAGE_MASK;
	if (t1 == t2) {

		/*
		 * No page crossing, send it all at once
		 */

		/* Rounding up the pkt size to FIFO alignment may put us past the
		 * end-of-page.  Conditions under which this occurs :
		 *  Buffer doesn't begin on FIFO alignment.
		 *  Buffer ends within the last 8 bytes of the page.
		 */

		/* In the diagram the top axis is the packet alignment.  The left
		 * axis is the buffer alignment.  The alignments are on either
		 * 8 byte (FIFO) alignments, or other (poor) alignments.  The
		 * arguments in parenthesis are to the mcmsg_fifo_out and
		 * mcmsg_fifo_out_eod functions.
		 */
/*
*	   \ pkt       FIFO                  poor
*	buf \_________________________________________________
*		|                       |                       |
* FIFO  | (bp1, pkt_out == pkt) |  (bp1, pkt_out)		|
*		|                       |                       |
*		|-----------------------------------------------|
*		|                       |                       |
* poor  | (bp1, pkt_out == pkt) |  copy(pkt)			|
*		|[copy done in fifo_out]|  (pbuf_out, pkt_out)  |
*		|_______________________|_______________________|
*/
		/* Since we must perform the copy in both instances of poor
		 * buffer alignment, we can do the copy here based on pkt.  In
		 * the case of poor alignments for both the buffer and pkt, pkt_out
		 * may point into the next (possibly non-existent) page, so
		 * we can't use it to do the copy.
		 */

		MCMSG_TRACE_FIFO("outeod  all", 2, bp1, pkt, 0, 0);
		if ((bp1 & (FIFO_ALIGN-1)) == 0) {
			mcmsg_fifo_out_eod(bp1, pkt_out);
		} else {
			bcopy(bp1, mcmsg_pbuf_out, pkt);
			mcmsg_fifo_out_eod(mcmsg_pbuf_out, pkt_out);
		}
	} else {

		/*
		 * Crosses a page, copy first page then second page
		 *
		 * t1 is the byte count for the first page
		 */

		t1 = t1 + MSG_PAGE_SIZE - bp1;
		assert(t1 < pkt_out);

		if ((t1 & (FIFO_ALIGN-1)) == 0) {

			/*
			 * Page crossing is well aligned
			 */

			MCMSG_TRACE_FIFO("out   first", 2, bp1, t1, 0, 0);
			mcmsg_fifo_out(bp1, t1);
			MCMSG_TRACE_FIFO("outeod last", 2, t2, pkt_out - t1, 0, 0);
			mcmsg_fifo_out_eod(t2, pkt_out - t1);
		} else {

			/*
			 * Page crossing is poorly aligned.
			 * Bcopy to temporary buffer
			 * then copy out
			 */

			bcopy(bp1, mcmsg_pbuf_out, t1);
			bcopy(t2, ((unsigned long)mcmsg_pbuf_out) + t1, pkt_out - t1);
			MCMSG_TRACE_FIFO("outeod copy", 2, mcmsg_pbuf_out, pkt_out, 0, 0);
			mcmsg_fifo_out_eod(mcmsg_pbuf_out, pkt_out);
		}
	}

}
#endif BIGPKTS

/*
 * Routine:
 *	mcmsg_fifo_out(addr, n)
 *
 * Arguments:
 *	addr:		Physical address of buffer
 *	n:		Byte count
 *
 * Purpose:
 *	Copies from a buffer to the NIC send FIFO.
 *	The buffer must be physically contiguous.
 * 	The byte count must be a multiple of 8.
 *	Detects the case where the LTU can be used.
 *
 * Returns:
 *	none
 */

mcmsg_fifo_out(addr, n)
	register unsigned long addr;
	register unsigned long n;
{
	register unsigned long m;

mcmsg_trace_debug("fifo_out", 2, addr, n, 0, 0);
#if BIGPKTS
	assert(n <=  DRAM_PAGE);
#else BIGPKTS
	assert(n <= FIFO_SIZE);
#endif BIGPKTS

	assert((mcmsg_mp_enable == 0) ? stack_assert_bounds() : 1);

	if ((addr & (FIFO_ALIGN-1)) != 0) {

		/*
		 * Poor alignment, copy into well aligned buffer.
		 */

		bcopy(addr, mcmsg_pbuf_out, n);
		addr = (unsigned long)mcmsg_pbuf_out;
	}

	/*
	 * 64-bit aligned, so we can copy directly
	 */

	m = (LTU_ALIGN-1) & -addr;
	if (n >= m && n - m >= LTU_MIN) {
		register unsigned long *dp = (unsigned long *) addr;

		/*
		 * Part of this transfer can be done with LTU
		 * m = number of bytes to cache line boundary
		 */

		n -= m;
		if (m) {
			DATA_COUNT(mcmsg_data_out_fifo, m);
			do {
				send2(dp[0], dp[1]);
				dp += 2;
				m -= 8;
			} while (m);
		}

		/*
		 * n = cache line aligned byte count
		 * m = remainder
		 */

		m = n & (LTU_ALIGN-1);
		n &= ~(LTU_ALIGN-1);

		DATA_COUNT(mcmsg_data_out_ltu, n);

		/*
		 * Validate the LTU alignment requirements
		 */

		assert(((unsigned long)dp & (LTU_ALIGN-1)) == 0);
		assert((n & (LTU_ALIGN-1)) == 0);
		assert(n >= LTU_MIN);
		assert(n <= DRAM_PAGE);
		assert((unsigned long)dp+n <=
			NEXT_DRAM_PAGE((unsigned long)dp));

		/*
		 * Decide whether to use real LTU or optimized copy loop
		 */

		if (mcmsg_ltu_enable) {

			assert((inl(DP_CONTROL1_READ) &
				(1 << DP_IMSK1_LTU1_CNT)) == 0);

			/*
			 * Start LTU1
			 */

			ltu_send_start(dp, n);

			/*
			 * Wait for the LTU to complete
			 */

			ltu_send_wait();

		} else {

			/*
			 * Use optimized copy loop
			 */

			soft_ltu_send(dp, n,
				      &NIC.io.full, &NIC.io.full);
		}

		if (m) {
			dp = (unsigned long *)
			 ((unsigned long)dp + n);
			DATA_COUNT(mcmsg_data_out_fifo, m);
			while (m) {
				send2(dp[0], dp[1]);
				dp += 2;
				m -= 8;
			}
		}

	} else {
		register unsigned long *dp = (unsigned long *) addr;
		register unsigned long *eaddr = (unsigned long *)
						(addr + n);

		/*
		 * Too small for LTU
		 */

		DATA_COUNT(mcmsg_data_out_fifo, n);
		while (dp < eaddr) {
			send2(dp[0], dp[1]);
			dp += 2;
		}
		assert(dp == eaddr);
	}

}


/*
 * Routine:
 *	mcmsg_fifo_out_eod(addr, n)
 *
 * Arguments:
 *	addr:		Physical address of buffer
 *	n:		Byte count
 *
 * Purpose:
 *	Copies from a buffer to the NIC send FIFO with EOD.
 *	The buffer must be physically contiguous.
 * 	The byte count must be a multiple of 8.
 *	Detects the case where the LTU can be used.
 *
 * Returns:
 *	none
 */

mcmsg_fifo_out_eod(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned long m;

mcmsg_trace_debug("fifo_out_eod", 2, addr, n, 0, 0);
#if BIGPKTS
	assert(n <=  DRAM_PAGE);
#else BIGPKTS
	assert(n <= FIFO_SIZE);
#endif BIGPKTS

	assert((mcmsg_mp_enable == 0) ? stack_assert_bounds() : 1);

	if ((addr & (FIFO_ALIGN-1)) != 0) {

		/*
		 * Poorly aligned, copy into well-aligned buffer.
		 */

		bcopy(addr, mcmsg_pbuf_out, n);
		addr = (unsigned long)mcmsg_pbuf_out;
	}

	/*
	 * 64-bit aligned, so we can copy directly
	 */

	m = (LTU_ALIGN-1) & -addr;
	if (n >= m && n - m >= LTU_MIN) {
		register unsigned long *dp = (unsigned long *) addr;

		/*
		 * Part of this transfer can be done with LTU
		 * m = number of bytes to cache line boundary
		 */

		n -= m;
		if (m) {
			DATA_COUNT(mcmsg_data_out_fifo, m);
			do {
				send2(dp[0], dp[1]);
				dp += 2;
				m -= 8;
			} while (m);
		}

		/*
		 * n = cache line aligned byte count
		 * m = remainder
		 */

		m = n & (LTU_ALIGN-1);
		n &= ~(LTU_ALIGN-1);

		DATA_COUNT(mcmsg_data_out_ltu, n);
		if (m == 0) {

			/*
			 * Validate the LTU alignment requirements
			 */

			assert(((unsigned long)dp & (LTU_ALIGN-1)) == 0);
			assert((n & (LTU_ALIGN-1)) == 0);
			assert(n >= LTU_MIN);
			assert(n <= DRAM_PAGE);
			assert((unsigned long)dp+n <=
				NEXT_DRAM_PAGE((unsigned long)dp));

			/*
			 * Decide whether to use real LTU or
			 * optimized copy loop
			 */

			if (mcmsg_ltu_enable) {
				assert((inl(DP_CONTROL1_READ) &
					(1 << DP_IMSK1_LTU1_CNT)) == 0);

				/*
				 * Start LTU1
				 */

#if BUMPERS
				ltu_send_start(dp, n);
#else BUMPERS
				ltu_send_start_eod(dp, n);
#endif BUMPERS

				/*
				 * Wait for the LTU to complete
				 */

				ltu_send_wait();

				MCMSG_BUMPER_OUT;
				RELEASE_TX_FIFO;

			} else {

				/*
				 * Use optimized copy loop
				 */

#if BUMPERS
				/* Without EOD */
				soft_ltu_send(dp, n,
					&NIC.io.full, &NIC.io.full);

#else BUMPERS
				/* With EOD */
				soft_ltu_send(dp, n,
					&NIC.io.full, &EOD.io.full);

#endif BUMPERS

				MCMSG_BUMPER_OUT;
				RELEASE_TX_FIFO;
			}
		} else {

			/*
			 * Validate the LTU alignment requirements
			 */

			assert(((unsigned long)dp & (LTU_ALIGN-1)) == 0);
			assert((n & (LTU_ALIGN-1)) == 0);
			assert(n >= LTU_MIN);
			assert(n <= DRAM_PAGE);
			assert((unsigned long)dp+n <=
				NEXT_DRAM_PAGE((unsigned long)dp));

			/*
			 * Decide whether to use real LTU or
			 * optimized copy loop
			 */

			if (mcmsg_ltu_enable) {

				assert((inl(DP_CONTROL1_READ) &
					(1 << DP_IMSK1_LTU1_CNT)) == 0);

				/*
				 * Start LTU1
				 */

				ltu_send_start(dp, n);

				/*
				 * Wait for the LTU to complete
				 */

				ltu_send_wait();

			} else {

				/*
				 * Use optimized copy loop
				 */

				soft_ltu_send(dp, n,
					&NIC.io.full, &NIC.io.full);
			}

			DATA_COUNT(mcmsg_data_out_fifo, m);
			m -= 8;
			dp = (unsigned long *)
			 ((unsigned long)dp + n);
			while (m) {
				send2(dp[0], dp[1]);
				dp += 2;
				m -= 8;
			}
			send2eod(dp[0], dp[1]);
		}

	} else {
		register unsigned long *dp = (unsigned long *) addr;
		register unsigned long *eaddr = (unsigned long *)
						(addr + n - 8);

		/*
		 * Too small for LTU
		 */

		DATA_COUNT(mcmsg_data_out_fifo, n);
		while (dp < eaddr) {
			send2(dp[0], dp[1]);
			dp += 2;
		}
		assert(dp == eaddr);
		send2eod(dp[0], dp[1]);
	}
}

