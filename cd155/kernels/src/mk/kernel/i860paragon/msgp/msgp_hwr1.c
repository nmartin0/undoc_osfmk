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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hwr1.c,v 1.16 1994/11/18 20:46:49 mtm Exp $
 */

/*
 * msgp_hwr1.c
 *
 * Hardware support routines for multicomputer message passing
 */

#include <i860paragon/msgp/msgp_hw.h>

/*
 * Fifo transfer code
 *
 * The following procedures copy between a user buffer and the NIC FIFOs.
 * The top level procedures (mcmsg_recv_buf, mcmsg_send_buf) have no
 * limitations on buffer size or alignment of the user buffer except that
 * the amount of data transferred in one call cannot exceed the page size
 * or (in general) the FIFO size.
 *
 * The arguments consist of two buffer pointers and a byte count.
 * The first buffer pointer is the validated physical address of the buffer.
 * The second buffer pointer is the validated physical address of the
 * last byte to be copied in the buffer.
 * If the buffer does not cross a page, bp2 = bp1 + pkt - 1.
 * Otherwise, the high bits of bp1 are the address of the first page
 * and the high bits of bp2 are the address of the second page.
 * The byte count must be non-zero.
 */

/*
 * Routine:
 *	mcmsg_recv_buf_even(bp1, bp2, pkt)
 *
 * Arguments:
 *	bp1:		Physical address of buffer
 *	bp2:		Physical address of last byte
 *	pkt:		Byte count > 0
 *
 * Purpose:
 *	Copies from the NIC receive FIFO to a buffer
 *	Optimized for LTU aligned transfers
 *
 * Returns:
 *	none
 */

#if	HANDCODE && !BIGPKTS
/* See msgp_hw.s */
#else	HANDCODE && !BIGPKTS
mcmsg_recv_buf_even(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	t1;
	register unsigned long	t2;
	register unsigned long la, ln;

mcmsg_trace_debug("recv_buf_even", 3, bp1, bp2, pkt, 0);

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & MSG_PAGE_MASK;
	t2 = bp2 & MSG_PAGE_MASK;

	if (t1 == t2) {

		/*
		 * No page crossing, copy it all at once
		 */

		MCMSG_TRACE_FIFO("in      all", 2, bp1, pkt, 0, 0);
		if (mcmsg_ltu_enable &&
		    (pkt & (LTU_ALIGN-1)) == 0 &&
		    (pkt >= LTU_MIN) && ((bp1 & (LTU_ALIGN-1)) == 0)) {

			assert((inl(DP_CONTROL1_READ) &
				(1 << DP_IMSK1_LTU0_CNT)) == 0);

			/*
			 * Start LTU0
			 */
			ltu_recv_start(bp1, pkt);
			ltu_recv_wait();

		} else {
#if BIGPKTS
			mcmsg_recv_buf(bp1, bp2, pkt);
#else BIGPKTS
			mcmsg_fifo_in(bp1, pkt);
#endif BIGPKTS
		}

	} else {

		int len1, len2;

		/*
		 * Crosses a page, copy first page then second page
		 */

		len1 = t1 + MSG_PAGE_SIZE - bp1;
		len2 = pkt - len1;

		assert(len1 < pkt);
		if ((mcmsg_ltu_enable &&
		    (pkt & (LTU_ALIGN-1)) == 0 &&
		    len1 >= LTU_MIN && pkt - len1 >= LTU_MIN &&
		    (bp1 & (LTU_ALIGN-1)) == 0)
#if BIGPKTS
			 && !mcmsg_send_waiting		/* PTS 10721 */
#endif BIGPKTS
			) {

			/*
			 * All LTU aligned,
			 */
			assert((inl(DP_CONTROL1_READ) &
				(1 << DP_IMSK1_LTU0_CNT)) == 0);

			/*
			 * Start LTU0 for first piece
			 */
			ltu_recv_start(bp1, len1);
			ltu_recv_wait();

			/*
			 * Start LTU0 for the rest
			 */
			ltu_recv_start(t2, len2);
			ltu_recv_wait();
		} else {

			mcmsg_recv_buf(bp1, bp2, pkt);
		}
	}
}
#endif	HANDCODE && !BIGPKTS

#if BIGPKTS
/*
 * Routine:
 *	mcmsg_recv_buf(bp1, bp2, pkt)
 *
 * Arguments:
 *	bp1:		Physical address of buffer
 *	bp2:		Physical address of last byte
 *	pkt:		Byte count > 0
 *
 * Purpose:
 *	Copies from the NIC receive FIFO to a buffer
 *
 * Returns:
 *	none
 */

mcmsg_recv_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	t1;
	register unsigned long	t2;
	register unsigned long pkt_in;
	register unsigned long last_addr;

	mcmsg_trace_debug("recv_buf", 3, bp1, bp2, pkt, 0);

	/*
	 *	Always receive cache lines.
	 */
	pkt_in = (pkt + LTU_ALIGN-1) & ~(LTU_ALIGN-1);

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & MSG_PAGE_MASK;
	t2 = bp2 & MSG_PAGE_MASK;

	if (t1 == t2) {

		/*
		 * No page crossing, copy it all at once
		 */

		MCMSG_TRACE_FIFO("in      all", 2, bp1, pkt, 0, 0);

		/*
		 * if (pkt_in > pkt) then the packet was padded on the
		 * transmit side to round up to LTU alignment.
		 * We must not write the padding onto the user.
		 */
		if (pkt_in > pkt)
		{
			/*
			 * optimization for LTU aligned buffer
			 * with length large enough for 2 LTU transfers.
			 */

			if ((pkt_in >= (2 * LTU_MIN)) &&
				((bp1 & (LTU_ALIGN-1)) == 0))
			{
				int size_less_LTU_MIN = pkt_in - LTU_MIN;

				/*
				 * LTU most of packet directly to user
				 */
				mcmsg_fifo_in(bp1, size_less_LTU_MIN);
				/*
				 * LTU_MIN packet into buffer and bcopy
				 */
				mcmsg_fifo_in(mcmsg_pbuf_in, LTU_MIN);
				bcopy(mcmsg_pbuf_in, bp1 + size_less_LTU_MIN,
					LTU_MIN - (pkt_in - pkt));
			} else
			{
				mcmsg_fifo_in(mcmsg_pbuf_in, pkt_in);
				bcopy(mcmsg_pbuf_in, bp1, pkt);
			}
		} else {
			mcmsg_fifo_in(bp1, pkt_in);
		}

	} else {

		int len1, len2, diff;
		/*
		 * Crosses a page, copy first page then second page
		 */

		len1 = t1 + MSG_PAGE_SIZE - bp1;
		len2 = pkt_in - len1;
		diff = pkt_in - pkt;

		assert(len1 < pkt);
		if (((len1 & (FIFO_ALIGN-1)) == 0) && 
		    (!mcmsg_send_waiting)) {	/* PTS 10721 */

			/*
			 * Page crossing is well aligned
			 */
			/*
			 * Read first page.
			 */
			mcmsg_fifo_in(bp1, len1);

			if (diff > 0) {
				/*
				 * Avoid writing past end of user buffer.
				 */
				mcmsg_fifo_in(mcmsg_pbuf_in, len2);
				bcopy(mcmsg_pbuf_in, t2, len2 - diff);
			} else {
				mcmsg_fifo_in(t2, len2);
			}

		} else {

			/*
			 * Page crossing is poorly aligned.
			 * Copy from FIFO to temporary buffer
			 * then bcopy out.
			 */

			MCMSG_TRACE_FIFO("in     copy", 2, mcmsg_pbuf_in, pkt, 0, 0);
			/*
			 * Avoid writing past end of user buffer.
			 */
			mcmsg_fifo_in(mcmsg_pbuf_in, pkt_in);
			bcopy(mcmsg_pbuf_in, bp1, len1);
			bcopy(((unsigned long)mcmsg_pbuf_in) + len1, t2, pkt - len1);
		}
	}
}

#else BIGPKTS

/*
 * Routine:
 *	mcmsg_recv_buf(bp1, bp2, pkt)
 *
 * Arguments:
 *	bp1:		Physical address of buffer
 *	bp2:		Physical address of last byte
 *	pkt:		Byte count > 0
 *
 * Purpose:
 *	Copies from the NIC receive FIFO to a buffer
 *
 * Returns:
 *	none
 */

mcmsg_recv_buf(bp1, bp2, pkt)
	register unsigned long	bp1;
	register unsigned long	bp2;
	register unsigned long	pkt;
{
	register unsigned long	t1;
	register unsigned long	t2;

	/*
	 * Calculate page addresses
	 */

	t1 = bp1 & MSG_PAGE_MASK;
	t2 = bp2 & MSG_PAGE_MASK;

	if (t1 == t2) {

		/*
		 * No page crossing, copy it all at once
		 */

		MCMSG_TRACE_FIFO("in      all", 2, bp1, pkt, 0, 0);
		mcmsg_fifo_in(bp1, pkt);

	} else {

		/*
		 * Crosses a page, copy first page then second page
		 *
		 * t1 is the byte count for the first page
		 */

		t1 = t1 + MSG_PAGE_SIZE - bp1;
		assert(t1 < pkt);
		if ((t1 & (FIFO_ALIGN-1)) == 0) {

			/*
			 * Page crossing is well aligned
			 */

			MCMSG_TRACE_FIFO("in    first", 2, bp1, t1, 0, 0);
			mcmsg_fifo_in(bp1, t1);
			MCMSG_TRACE_FIFO("in     last", 2, t2, pkt - t1, 0, 0);
			mcmsg_fifo_in(t2, pkt - t1);
		} else {

			/*
			 * Page crossing is poorly aligned.
			 * Copy from FIFO to temporary buffer
			 * then bcopy out.
			 */

			MCMSG_TRACE_FIFO("in     copy", 2, mcmsg_pbuf_in, pkt, 0, 0);
			mcmsg_fifo_in(mcmsg_pbuf_in, pkt);
			bcopy(mcmsg_pbuf_in, bp1, t1);
			bcopy(((unsigned long)mcmsg_pbuf_in) + t1, t2, pkt - t1);
		}
	}
}

#endif BIGPKTS

/*
 * Routine:
 *	mcmsg_fifo_in(addr, n)
 *
 * Arguments:
 *	addr:		Physical address of buffer
 *	n:		Byte count
 *
 * Purpose:
 *	Copies from the NIC receive FIFO to a buffer.
 *	The buffer must be physically contiguous.
 *	There is no alignment restriction on the byte count,
 *	so this routine copies the last data byte-by-byte if necessary.
 *	This is necessary to avoid overrunning the user receive buffer.
 *	Detects the case where the LTU can be used.
 *
 * Returns:
 *	none
 */

mcmsg_fifo_in(addr, n)
	register unsigned long	addr;
	register unsigned long	n;
{
	register unsigned char	*p;
	register unsigned char	*q;
	register unsigned long m;
	register unsigned long *dp;
	register unsigned long addr_in;
	register int bcount_in;

mcmsg_trace_debug("fifo_in", 2, addr, n, 0, 0);
#if	BIGPKTS
	assert(n <= DRAM_PAGE);
#else	BIGPKTS
	assert(n <= FIFO_SIZE);
#endif	BIGPKTS

	assert((mcmsg_mp_enable == 0) ? stack_assert_bounds() : 1);

	/*
	 * If buffer is poorly aligned, read it into well
	 * aligned buffer and copy it later.
	 */

	if ((addr & (FIFO_ALIGN-1)) != 0) {
		addr_in = (unsigned long)mcmsg_pbuf_in;
		bcount_in = n;
	} else {
		addr_in = addr;
	}

	/*
	 * 64-bit aligned, so we can copy directly
	 */
	dp = (unsigned long *) addr_in;

	m = (LTU_ALIGN-1) & -addr_in;
	if (n >= m && n - m >= LTU_MIN) {

		/*
		 * Part of this transfer can be done with LTU
		 * m = number of bytes to cache line boundary
		 */

		n -= m;
		if (m) {
			DATA_COUNT(mcmsg_data_in_fifo, m);
			do {
				recv2(dp[0], dp[1]);
				dp += 2;
				m -= 8;
			} while (m);
		}

		/*
		 * m = cache line aligned byte count
		 * n = remainder
		 */

		m = n & ~(LTU_ALIGN-1);
		n &= (LTU_ALIGN-1);

		DATA_COUNT(mcmsg_data_in_ltu, m);

		/*
		 * Validate the LTU alignment requirements
		 */

		assert(((unsigned long)dp & (LTU_ALIGN-1)) == 0);
		assert((m & (LTU_ALIGN-1)) == 0);
		assert(m >= LTU_MIN);
		assert(m <= DRAM_PAGE);
		assert((unsigned long)dp+m <=
			NEXT_DRAM_PAGE((unsigned long)dp));

		/*
		 * Decide whether to use real LTU or
		 * optimized copy loop
		 */

		if (mcmsg_ltu_enable) {

			assert((inl(DP_CONTROL1_READ) &
				(1 << DP_IMSK1_LTU0_CNT)) == 0);

			/*
			 * Start LTU0
			 */
			ltu_recv_start(dp, m);
			ltu_recv_wait();

		} else {

			/*
			 * Use optimized copy loop
			 */

			soft_ltu_recv(dp, m, &NIC.io.full);
		}

		if (n == 0) {
			goto fifo_in_copy;
		}
		dp = (unsigned long *) ((unsigned long)dp + m);
	}

	/*
	 * Too small for LTU
	 */

	DATA_COUNT(mcmsg_data_in_fifo, n);
	while (n >= 8) {
		recv2(dp[0], dp[1]);
		dp += 2;
		n -= 8;
	}
	if (n > 0) {
		unsigned long last[2];

		recv2(last[0], last[1]);
		p = (unsigned char *)last;
		q = (unsigned char *)dp;
		while (n--) {
			*q++ = *p++;
		}
	}
fifo_in_copy:

	if (addr != addr_in) {

		/* Copy data from temporary buffer */

		bcopy (addr_in, addr, bcount_in);
	}
	return;
}
