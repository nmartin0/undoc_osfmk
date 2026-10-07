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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hwr2.c,v 1.11 1994/11/18 20:46:50 mtm Exp $
 */

/*
 * msgp_hwr2.c
 *
 *	Hardware support for receive, LTU in particular, including:
 *
 *		ltu_recv_start(a,n)
 *		ltu_start(a, n)
 *		ltu_recv_wait()
 *		mcmsg_enable_rx_interrupts()
 *		mcmsg_recv_ready()
 */

#include <i860paragon/msgp/msgp_hw.h>

#if BIGPKTS && BUMPERS
/*
 *	Variables to keep track of the last LTU receive in
 *	case of NIC-A lockup.
 */
unsigned long mcmsg_recv_last_addr = 0;
unsigned long mcmsg_recv_last_count = 0;
unsigned long mcmsg_recv_last_remain = 0;	/* boolean */
unsigned long mcmsg_recv_last_cancel = 0;	/* boolean */
#endif BIGPKTS && BUMPERS

/*
 * Routine:
 *	ltu_recv_start(a, n)
 *
 * Arguments:
 *	a:		Data address
 *	n:		Data length
 *
 * Purpose:
 *	Starts receive LTU (ltu 0).
 *
 * Returns:
 *	none
 */
ltu_recv_start(a, n)
unsigned long a;
unsigned long n;
{

	assert(n >= LTU_MIN);
	assert((n & (LTU_ALIGN-1)) == 0);
	assert((a & (LTU_ALIGN-1)) == 0);

	mcmsg_trace_debug("LTU recv start", 2, a, n, 0, 0);

#if    BIGPKTS && BUMPERS
	{

		/*
		 * Nic-A workaround.
		 * Do one less cache line than specified.
		 * Last line will be read by CPU in ltu_recv_wait().
		 *
		 * Keep track of the last receive LTU
		 * started in case of NIC-A lockup.
		 */
		if (n > LTU_MIN) {
			n -= LTU_ALIGN;
			mcmsg_recv_last_remain = 1;
		} else {
			mcmsg_recv_last_remain = 0;
		}
	
		mcmsg_recv_last_addr = a;
		mcmsg_recv_last_count = n;
	}
#endif BIGPKTS && BUMPERS

	ltu_start(a & 0x3fffffff,
	              BYTES_TO_LTUS(n) | (0x40000000 & a));
}

/*
 * Routine:
 *	ltu_start(a, n)
 *
 * Arguments:
 *	a:		LTU address
 *	n:		LTU data
 *
 * Purpose:
 *	Starts an LTU. The arguments must be all set because this just
 *	does the bus cycle.
 *
 * Returns:
 *	none
 */

ltu_start(a, n)
unsigned long a;
unsigned long n;
{
	asm("	stio.l	r17,r16");
}

/*
 * Routine:
 *	ltu_recv_wait()
 *
 * Purpose:
 *	Waits for the receive LTU to finish
 *
 * Returns:
 *	None.
 */

ltu_recv_wait()
{
	nic_reg	t;
	unsigned long ltu_count;
	unsigned long ltu_frozen_count = 0xffffffff;
	int frozen_timer = 0;
	int ltu_timer = 0;
	register int i = 0;
	extern void mcmsg_complete_send(int escape);

	/*
	 * Wait for LTU 0 interrupt status bit to go high
	 */

	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT)) {

#if BIGPKTS
		/*
		 * In NON-BURST mode, do concurrent sends while
		 * waiting for a receive to finish.
		 */
		{
			if (mcmsg_send_in_progress)
			{
				/*
				 * if the send LTU is done
				 */
				if ((inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT))
				{
					nic_reg t;

					t.full = NIC.status.full;
					/*
					 * if the xmit fifo is empty
					 */
					if (t.halfs.lo & NIC_STAT_TX_FIFO_EMPTY)
					{
						/*
						 * complete pending send
						 */
						mcmsg_recv_waiting = 1;
						mcmsg_complete_send(1);
						mcmsg_recv_waiting = 0;
					}
				}
			}

#if BUMPERS
			/*
			 * check for LTU lockup.
			 */
			if (ltu_timer++ > 6000) {
				ltu_timer = 0;
				ltu_count = inl(DP_LTU0_COUNT) & 0x1ff;
				if (ltu_count == ltu_frozen_count) {
					frozen_timer++;
					if (frozen_timer == 4) {
						ltu_timer = 0;
						frozen_timer = 0;
						mcmsg_lockup_recover();
					}
				} else {
					ltu_frozen_count = ltu_count;
				}
			}
#endif BUMPERS

		}
#else  BIGPKTS
		assert(i++ < 1000000);
#endif BIGPKTS
	} /* end while */

	/*
	 * Clear LTU 0 interrupt status bit
	 */

	inl(DP_LTU0_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT));

#if BIGPKTS && BUMPERS
	/*
	 * Read last cache line with CPU.
	 */
	if (mcmsg_recv_last_remain) {
		mcmsg_recv_last(mcmsg_recv_last_addr+mcmsg_recv_last_count);
	}
#endif BIGPKTS && BUMPERS

	return;
}

#if BIGPKTS && BUMPERS

/*
 * Routine:
 *	mcmsg_recv_last(addr)
 *
 * Arguments:
 *	addr:		Physical address of buffer
 *
 * Purpose:
 *	Part of the NIC-A workaround.
 *
 *	Read the last cache line of a message with the CPU.
 *	Ensure that we don't empty the receive FIFO as we pull the
 *	last cache-line piece of the message.
 *
 * Parameters:
 *	addr	address of FIFO aligned buffer.
 *
 * Returns:
 *	none
 */
mcmsg_recv_last(addr)
register double *addr;
{
	long *lp;
	nic_reg t;
	int i;

	if (mcmsg_recv_last_cancel) {
		mcmsg_recv_last_cancel = 0;
		mcmsg_trace_debug("recv last cancel", 2, addr, LTU_ALIGN, 0, 0);
		return;
	}

	mcmsg_trace_debug("recv last", 2, addr, LTU_ALIGN, 0, 0);
	assert(((unsigned long)addr & (FIFO_ALIGN-1)) == 0);

	/*
	 * Wait for NOT ALMOST EMPTY or EOD-IN then get data fast.
	 */
	for (i=0; i<1000; i++) {
		t.full = NIC.status.full;
		if (t.halfs.lo & 
			(NIC_STAT_EOD_IN_NIC|NIC_STAT_RX_FIFO_NOT_ALMOST_EMPTY)) {
#if MACH_ASSERT
			lp = (long *)addr;
			recv2_now(lp[0], lp[1]);
			recv2_now(lp[2], lp[3]);
			recv2_now(lp[4], lp[5]);
			recv2_now(lp[6], lp[7]);
#else MACH_ASSERT
			addr[0] = NIC.io.full;
			addr[1] = NIC.io.full;
			addr[2] = NIC.io.full;
			addr[3] = NIC.io.full;
#endif MACH_ASSERT
			return;
		}
	}

	/*
	 * If the FIFO is not EMPTY, there is still a chance that
	 * the FIFO is not locked. 
	 */

	lp = (long *)addr;
	recv2(lp[0], lp[1]);
	recv2(lp[2], lp[3]);
	recv2(lp[4], lp[5]);
	recv2(lp[6], lp[7]);
	return;
}



char mcmsg_lockup_buf[LTU_ALIGN * 5]; 
int  mcmsg_lockup_trace_index = 0;
int  mcmsg_lockup_recoveries = 0;

/*
 * Routine:
 *	mcmsg_lockup_recover()
 *
 * Arguments:
 *	None.
 *	Uses global variables as input:
 *		mcmsg_recv_last_addr	the address of the last LTU receive
 *		mcmsg_recv_last_count the count of the last LTU receive
 *		
 * Purpose:
 *	Recover from a NIC-A lockup. Called from ltu_recv_wait
 *	if the LTU count fails to decrement. To unlock the NIC,
 *	the following steps are taken:
 *
 *		1) do 4 reads from the receive FIFO
 *		2) start a minimum receive LTU 
 *		3) pull the remainder of the xfer out of the FIFO.
 *
 * Returns:
 *	none
 */

mcmsg_lockup_recover()
{
	int x, i;
	nic_reg	t;
	int bcount, wcount, ltu_count;
	long *lp;
	double *start_dp, *dp, *udp;
	unsigned long lines_read;

#if MSG_TRACE
	{
		/* debug */
		extern int mcmsg_trace_count;
		mcmsg_lockup_trace_index = mcmsg_trace_count;
	}
#endif MSG_TRACE

	/*
	 * Get the current LTU count.
	 */
	ltu_count = inl(DP_LTU0_COUNT) & 0x1ff;

	/*
	 * reset the LTU receive counter
	 */
	outl(0,0);
	outl(0,0);
	outl(0,0);
	outl(0,0);

	/*
	 * Clear LTU0 count interrupt
	 */
	inl(DP_LTU0_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT));

	/*
	 * Get LTU aligned buffer.
	 */
	start_dp = (double *)
		(((unsigned long)mcmsg_lockup_buf+(LTU_ALIGN-1)) & ~(LTU_ALIGN-1));

	/*
	 * debug 
	 */
	mcmsg_lockup_recoveries++;
	mcmsg_trace_debug("LOCKUP adr,cnt,ltu_count,n", 4, 
		mcmsg_recv_last_addr, mcmsg_recv_last_count, ltu_count, mcmsg_lockup_recoveries);

	/*
	 * Do four magic reads.
	 */

	lp = (long *)start_dp;
	recv2(lp[0], lp[1]);
	recv2(lp[2], lp[3]);
	recv2(lp[4], lp[5]);
	recv2(lp[6], lp[7]);

	lines_read = 1;

	t.full = NIC.status.full;
	assert((t.halfs.lo & NIC_STAT_ERRORS) == 0);
	
	/*
	 * Put some magic in the buffer to see if the LTU
	 * gets 1 or 2 cache lines when we specify the next
	 * minimum transfer.
	 */

	lp = (long *)start_dp;
	lp[8] = 0xfeedface;
	lp[9] = 0xdeadf00d;

	lp[16] = 0xdeadface;
	lp[17] = 0xfacef00d;

	lp[24] = 0xfacefeed;
	lp[25] = 0xdeadbeed;

	/*
	 * Start a minimum LTU receive to clear out the NIC.
	 */
	dp = &start_dp[4];
	mcmsg_trace_debug("LOCKUP start ltu", 2, dp, (2 * LTU_ALIGN), 0, 0);
	ltu_start((unsigned long)dp & 0x3fffffff,
	          BYTES_TO_LTUS(2 * LTU_ALIGN) | (0x40000000 & (unsigned long)dp));

	assert((x = 1000000) != 0);
	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT)) {
		asm("  nop");
		asm("  nop");
		asm("  nop");
		asm("  nop");
		asm("  nop");
		asm("  nop");
		asm("  nop");
		asm("  nop");
		assert(x--);
	}

	/*
	 * Inspect the buffer to see what the LTU actually
	 * transferred. Valid known possibilities are:
	 *	1) LTU xfered into second cache line
	 *	2) LTU xfered into 1st and second cache line.
	 */
	
	dp = start_dp;
	if ((lp[8] == 0xfeedface) && (lp[9] == 0xdeadf00d)) {
		/*
		 * LTU xfered one cache line into second cache line
		 * location, move it up.
		 */
		assert((lp[16] != 0xdeadface) && (lp[17] != 0xfacef00d));
		dp[4] = dp[8];
		dp[5] = dp[9];
		dp[6] = dp[10];
		dp[7] = dp[11];

		lines_read++;
		mcmsg_trace_debug("LOCKUP 1 line", 1, ltu_count, 0, 0, 0);

	} else if ((lp[16] != 0xdeadface) && (lp[17] != 0xfacef00d)) {

		/*
		 * LTU xfered two cache lines.
		 * LTU count was pre-decremented so adjust.
		 */
		lines_read += 2;
		ltu_count++;

		mcmsg_trace_debug("LOCKUP 2 lines", 1, ltu_count, 0, 0, 0);
	}

	/* more than 2 cache lines? */
	assert((lp[24] == 0xfacefeed) && (lp[25] == 0xdeadbeed));

	/*
	 * Copy the data we have read from the fifo to the user buffer.
	 */
	bcount = ltu_count * LTU_ALIGN;
	udp = (double *) (mcmsg_recv_last_addr + mcmsg_recv_last_count - bcount);
	/*
	 * copy the data back into the user buffer.
	 */
	mcmsg_trace_debug("LOCKUP copy to/n", 2, udp, lines_read*LTU_ALIGN, 0, 0);
	for (i=0; i < lines_read; i++) {
		*udp++ = *dp++;
		*udp++ = *dp++;
		*udp++ = *dp++;
		*udp++ = *dp++;
	}

	/*
	 *  If more than one cache line left, restart the LTU. 
	 *  If one cache line left, pull it out with the CPU.
	 *	If zero cache lines left just return.
	 *  If < 0 cache lines left, flag recv_last to not do anything.
	 */
	ltu_count -= lines_read;
	if (ltu_count > 1) {
		mcmsg_trace_debug("LOCKUP restart ltu", 
				2, udp, ltu_count*LTU_ALIGN, 0, 0);

		/*
		 * Clear LTU0 count interrupt
		 */
		inl(DP_LTU0_CLEAR_CNT);
		assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU0_CNT));

		ltu_start((unsigned long)udp & 0x3fffffff,
				BYTES_TO_LTUS(ltu_count*LTU_ALIGN) | 
				(0x40000000 & (unsigned long)udp));
	} else if (ltu_count == 1) {
		lp = (long *) udp;
		recv2(lp[0], lp[1]);
		recv2(lp[2], lp[3]);
		recv2(lp[4], lp[5]);
		recv2(lp[6], lp[7]);
	} else if (ltu_count == -1) {
		mcmsg_recv_last_cancel = 1;
	}
	assert (ltu_count >= -1);
	return;
}
#endif BIGPKTS && BUMPERS

/*
 * Routine:
 *	soft_ltu_recv(address, bytecount, nicaddr)
 *	soft_ltu_send(address, bytecount, nicaddr, eodaddr)
 *
 * Arguments:
 *	address:		Buffer address
 *	bytecount:		Byte count
 *	nicaddr:		NIC FIFO address
 *	eodaddr:		NIC FIFO address (different if EOD desired)
 *
 * Purpose:
 *	Optimized copy loops can be used instead of LTU's.
 *	Assumes cache line aligned buffer address and count.
 *
 * Returns:
 *	none
 */

soft_ltu_holder()
{
asm("//	soft_ltu_recv(address, bytecount, nicaddr)		");
asm("	.align 4						");
asm("_soft_ltu_recv::						");
asm("	pfld.d	0(r18), f0	//  1st stage			");
asm("	adds	-32, r0, r22	// r22 is bla decrement		");
asm("	pfld.d	0(r18), f0	//  2nd stage			");
asm("	adds	-32, r17, r20	// r20 is bla count		");
asm("	pfld.d	0(r18), f0	//  3rd stage			");
asm("	bla	r22, r20, 0f	// prime bla			");
asm("	 adds	-16, r16, r21	// r21 is store address		");
asm("0:	bla	r22, r20, 2f	// tick off 32			");
asm("	 pfld.d	0(r18), f16	//  pipe primed and loading	");
asm("	pfld.d	16(r21), f18	//  flush pipe			");
asm("1:	pfld.d	16(r21), f20	//				");
asm("	pfld.d	16(r21), f22	//				");
asm("	fst.q	f16, 16(r21)++	// store final 32 bytes		");
asm("	bri	r1		// return			");
asm("	 fst.q	f20, 16(r21)	//				");
asm("								");
asm("2:	pfld.d	0(r18), f18	//  load next 32 bytes		");
asm("	pfld.d	0(r18), f20	//				");
asm("	pfld.d	0(r18), f22	//				");
asm("	bla	r22, r20, 4f	// tick off 32			");
asm("	 pfld.d	0(r18), f24	//				");
asm("3:	pfld.d	16(r21), f26	//  flush pipe			");
asm("	pfld.d	16(r21), f28	//				");
asm("	pfld.d	16(r21), f30	//				");
asm("	fst.q	f16, 16(r21)++	// store final 64 bytes		");
asm("	fst.q	f20, 16(r21)++	//				");
asm("	fst.q	f24, 16(r21)++	//				");
asm("	bri	r1		// return			");
asm("	 fst.q	f28, 16(r21)	//				");
asm("								");
asm("4:	pfld.d	0(r18), f26	//  load next 32 bytes		");
asm("	pfld.d	0(r18), f28	//				");
asm("	pfld.d	0(r18), f30	//				");
asm("	fst.q	f16, 16(r21)++	// store next 64 bytes		");
asm("	fst.q	f20, 16(r21)++	//				");
asm("	fst.q	f24, 16(r21)++	//				");
asm("	fst.q	f28, 16(r21)++	//				");
asm("	bla	r22, r20, 2b	// tick off 32			");
asm("	 pfld.d	0(r18), f16	//				");
asm("	br	1b		//				");
asm("	 pfld.d	16(r21), f18	//  flush pipe			");
asm("								");
asm("//	soft_ltu_send(address, bytecount, nicaddr, eodaddr)	");
asm("	.align 4						");
asm("_soft_ltu_send::						");
asm("	pfld.d	0(r16), f0	//  1st stage			");
asm("	adds	-32, r0, r22	// r22 is bla decrement		");
asm("	pfld.d	8(r16)++, f0	//  2nd stage			");
asm("	adds	-32, r17, r20	// r20 is bla count		");
asm("	pfld.d	8(r16)++, f0	//  3rd stage			");
asm("	bla	r22, r20, 0f	// prime bla			");
asm("	 nop			//				");
asm("0:	bla	r22, r20, 2f	// tick off 32			");
asm("	 pfld.d	8(r16)++, f16	//  pipe primed and loading	");
asm("	pfld.d	0(r16), f18	//  flush pipe			");
asm("1:	pfld.d	0(r16), f20	//				");
asm("	pfld.d	0(r16), f22	//				");
asm("	fst.d	f16, 0(r18)	// store final 32 bytes		");
asm("	fst.d	f18, 0(r18)	//				");
asm("	fst.d	f20, 0(r18)	//				");
asm("	bri	r1		// return			");
asm("	 fst.d	f22, 0(r19)	// send eod			");
asm("								");
asm("2:	pfld.d	8(r16)++, f18	//  load next 32 bytes		");
asm("	pfld.d	8(r16)++, f20	//				");
asm("	pfld.d	8(r16)++, f22	//				");
asm("	bla	r22, r20, 4f	// tick off 32			");
asm("	 pfld.d	8(r16)++, f24	//				");
asm("3:	pfld.d	0(r16), f26	//  flush pipe			");
asm("	pfld.d	0(r16), f28	//				");
asm("	pfld.d	0(r16), f30	//				");
asm("	fst.d	f16, 0(r18)	// store final 64 bytes		");
asm("	fst.d	f18, 0(r18)	// store final 64 bytes		");
asm("	fst.d	f20, 0(r18)	//				");
asm("	fst.d	f22, 0(r18)	//				");
asm("	fst.d	f24, 0(r18)	//				");
asm("	fst.d	f26, 0(r18)	//				");
asm("	fst.d	f28, 0(r18)	//				");
asm("	bri	r1		// return			");
asm("	 fst.d	f30, 0(r19)	// send eod			");
asm("								");
asm("4:	pfld.d	8(r16)++, f26	//  load next 32 bytes		");
asm("	pfld.d	8(r16)++, f28	//				");
asm("	pfld.d	8(r16)++, f30	//				");
asm("	fst.d	f16, 0(r18)	// store next 64 bytes		");
asm("	fst.d	f18, 0(r18)	// store next 64 bytes		");
asm("	fst.d	f20, 0(r18)	//				");
asm("	fst.d	f22, 0(r18)	//				");
asm("	fst.d	f24, 0(r18)	//				");
asm("	fst.d	f26, 0(r18)	//				");
asm("	fst.d	f28, 0(r18)	//				");
asm("	fst.d	f30, 0(r18)	//				");
asm("	bla	r22, r20, 2b	// tick off 32			");
asm("	 pfld.d	8(r16)++, f16	//				");
asm("	br	1b		//				");
asm("	 pfld.d	0(r16), f18	//  flush pipe			");
}

/*
 * Routine:
 *	mcmsg_fifo_flush(n)
 *
 * Arguments:
 *	n:		Byte count
 *
 * Purpose:
 *	Empties the NIC receive FIFO.
 *	The byte count must be a multiple of 8.
 *
 * Returns:
 *	none
 */

mcmsg_fifo_flush(n)
	unsigned long	n;
{
	unsigned long	x;
	unsigned long	y;
	unsigned long	t;

	n = (n+(FIFO_ALIGN-1)) & ~(FIFO_ALIGN-1);

	assert((t = 1000000) != 0);
	DATA_COUNT(mcmsg_data_flush, n);

	while (n) {
		recv2(x, y);
		n -= 8;
	}

	assert(n == 0);
	return t;
}

/*
 * Routine:
 *	mcmsg_fifo_flush_pkt(n)
 *
 * Arguments:
 *	n:		Byte count
 *
 * Purpose:
 *	Removes entire data part of packet from receive FIFO.
 *
 * Returns:
 *	none
 */

mcmsg_fifo_flush_pkt(n)
	unsigned long	n;
{
	unsigned long	x;
	unsigned long	y;
	unsigned long	t;

#if BIGPKTS
	n = (n+(LTU_ALIGN-1)) & ~(LTU_ALIGN-1);
#endif BIGPKTS

	return (mcmsg_fifo_flush(n));
}

/*
 * Routine:
 *	mcmsg_enable_rx_interrupts()
 *
 * Purpose:
 *	Enable the NIC receive interrupts.
 *	(Actually enabled when SPL set appropriately)
 *	This should only be called once during initialization,
 *	but its actually called when an IPC receive buffer is posted.
 *
 * Returns:
 *	none
 */
void
mcmsg_enable_rx_interrupts()
{
	int s;

	if (mcmsg_hw.recv_enable.halfs.lo == 0) {
		nic_reg t;

        mcmsg_hw.recv_status = RECV_INTR_MODE_1;

        mcmsg_hw.recv_enable.halfs.lo = mcmsg_hw.recv_status |
                        NIC_CNTRL_ERRORS;
        NIC.set.full = mcmsg_hw.recv_enable.full;
    }
}

/*
 * Routine:
 *	mcmsg_recv_ready()
 *
 * Purpose:
 *	Check whether there is a packet ready in the NIC receive FIFO.
 *
 * Returns:
 *	1 if there is a packet ready to be received
 *	0 if not
 */

mcmsg_recv_ready()
{
	nic_reg	t;
	register double	x;
	int	i = 0;

	/*
	 * Don't check until fully initialized
	 */

	if (!mcmsg_hw.recv_enable.halfs.lo)
		return 0;

#if    MACH_ASSERT
	t.full = mcmsg_nic_status(2);
#else  MACH_ASSERT
	t.full = NIC.status.full;
#endif MACH_ASSERT

	/*
	 * Check for NIC Receive Ready Status
	 */
	if ((t.halfs.lo & mcmsg_hw.recv_status) != 0) {
		return 1;
	}
	return 0;
}

/*
 * Routine:
 *	mcmsg_eod_last()
 *
 * Purpose:
 *	Called after every message.
 *	Intended to test that the last read from the NIC receive FIFO had
 *	an EOD marker, indicating that it was indeed the end of the packet.
 *
 * Returns:
 *	none
 */

mcmsg_eod_last()
{
	int x;
	nic_reg	t;

	t.full = mcmsg_nic_status(4);

#if BUMPERS
#if BIGPKTS
	/*
	 * Can't check for the presence of a bumper in MODE_1
	 * because of the loopback case. I can't send myself
	 * a bumper if I am waiting for it.
	 */
	if (mcmsg_hw.recv_status == RECV_INTR_MODE_1) {
		assert((t.halfs.lo & NIC_STAT_ERRORS) == 0);
		return 1;
	}
#else BIGPKTS
	if (mcmsg_hw.recv_status == RECV_INTR_MODE_1) {
		/*
		 * If BUMPER loaded mode, Check that BUMPER is present.
		 */
		return ((t.halfs.lo & (NIC_STAT_EOD_IN_NIC | NIC_STAT_CAN_READ_1)) ==
	          (NIC_STAT_EOD_IN_NIC | NIC_STAT_CAN_READ_1));
	}
#endif BIGPKTS
#endif BUMPERS

	return (t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0;
}

#if	BUMPERS
#if	HANDCODE
/* See msgp_dispatch.s */
#else	HANDCODE

mcmsg_recv_even(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	assert(hdr2 == 0xbabeeeee);
}

mcmsg_recv_bump(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	nic_reg	t;
	unsigned long	a, b;

	assert(hdr1 == MCTRL_BUMP);
	assert(hdr2 == 0xbabefe9d);

	/*
	 * Take the rest of the bumper out
	 */

	recv2(a, b);
	assert(a == (MCTRL_BUMP | 1 << 16) && b == 0xbabefe9d);
	recv2(a, b);
	assert(a == (MCTRL_BUMP | 2 << 16) && b == 0xbabefe9d);
	recv2(a, b);
	assert(a == (MCTRL_BUMP | 3 << 16) && b == 0xbabefe9d);

	/*
	 * Switch to bumpers not loaded mode
	 */

#if	MACH_ASSERT
	t.full = mcmsg_nic_status(3);
	assert((t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0);
	assert(mcmsg_recv_byte_count % 16 == 8);
#endif	MACH_ASSERT
	mcmsg_recv_byte_count = 0;
	mcmsg_hw.recv_status = RECV_INTR_MODE_2;
}
#endif	HANDCODE

/*
 * Routine:
 *	mcmsg_set_bumper()
 *
 * Purpose:
 *	Called after every message. Switches the receive mode.
 *
 * Returns:
 *	none
 */

mcmsg_set_bumper()
{

	assert(mcmsg_hw.recv_enable.halfs.lo);

	/*
	 * Clear the old interrupt mask
	 */

	NIC.clear.full = mcmsg_hw.recv_enable.full;

	/*
	 * Set the new interrupt mask
	 */

	mcmsg_hw.recv_enable.halfs.lo = mcmsg_hw.recv_status |
					NIC_CNTRL_ERRORS;
	NIC.set.full = mcmsg_hw.recv_enable.full;
#if	NIC_TRACE
	mcmsg_recv_count++;
#endif	NIC_TRACE
}

/*
 * Routine:
 *	mcmsg_flush_invalid()
 *
 * Purpose:
 *	Clears the NIC receive FIFO. Does not require a byte count.
 *
 * Returns:
 *	none
 */

mcmsg_flush_invalid()
{
	unsigned long a, b;
	nic_reg	t;
	long	m;

	assert(0);
	m = 1000000;
	for (;;) {

		/*
		 * Read word by word checking status before each one
		 */

		t.full = mcmsg_nic_status(5);

		/*
		 * Keep checking while the FIFO has nothing in it
		 * (Is this right? should we check NIC_STAT_CAN_READ_1?)
		 * (Maybe thats why this always seems to hang)
		 */

		if ((t.halfs.lo & NIC_STAT_RX_LOW) == 0) {
			return;
		}

		/*
		 * If the last one we read was EOD, we are done
		 */

		if ((t.halfs.lo & NIC_STAT_EOD_IN_WORD) != 0) {
			return;
		}

		/*
		 * Read in one word
		 */

		recv2(a, b);
#if 0
		mcmsg_trace_debug("invalid msg data", 2, a, b, 0, 0);
#endif
		if (--m == 0) {
			break;
		}
	}
	mcmsg_trace_debug("no EOD found", 0, 0, 0, 0, 0);
	assert_mcmsg_eod_last();
}


#else	BUMPERS

#if	HANDCODE

/* See msgp_dispatch.s */

#else HANDCODE
mcmsg_recv_even(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{

	assert(hdr2 == 0xbabeeeee);
}

mcmsg_recv_bump(hdr1, hdr2)
	unsigned long	hdr1;
	unsigned long	hdr2;
{
	assert(0);
}
#endif	HANDCODE

/*
 * Routine:
 *	mcmsg_flush_invalid()
 *
 * Purpose:
 *	Clears the NIC receive FIFO. Does not require a byte count.
 *
 * Returns:
 *	none
 */

mcmsg_flush_invalid()
{
	unsigned long a, b;

	while (!mcmsg_eod_last()) {
		recv2(a, b);
#if 0
		mcmsg_trace_debug("invalid msg data", 2, a, b, 0, 0);
#endif
	}
}

#endif	BUMPERS
