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
 *	Copyright 1991  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hws2.c,v 1.8 1994/11/21 22:57:22 tnt Exp $
 */

/*
 * msgp_hw.c
 *
 * Hardware support routines for multicomputer message passing
 */

#include <i860paragon/msgp/msgp_hw.h>

/*
 * Routine:
 *	ltu_send_start()
 *
 * Purpose:
 *	Start a send LTU (LTU 1)
 *
 * Returns:
 *	none
 */
ltu_send_start(a, n)
unsigned long a, n;
{

	mcmsg_trace_debug("LTU send start", 2, a, n, 0, 0);


#if BIGPKTS
	/*
	 * Clear LTU 1 interrupt status bit
	 */

	inl(DP_LTU1_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT));
#endif BIGPKTS

	/*
	 * start LTU 1
	 */
	ltu_start((a + 4) & 0x3fffffff,
	          BYTES_TO_LTUS(n) | 0x80000000 | (0x40000000 & (a + 4)));

}

/*
 * Routine:
 *	ltu_send_start_eod()
 *
 * Purpose:
 *	Start a send LTU (LTU 1) with EOD
 *
 * Returns:
 *	none
 */
ltu_send_start_eod(a, n)
unsigned long a, n;
{

	mcmsg_trace_debug("LTU send EOD", 2, a, n, 0, 0);


#if BIGPKTS
	/*
	 * Clear LTU 1 interrupt status bit
	 */

	inl(DP_LTU1_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT));
#endif BIGPKTS

	/*
	 * start LTU 1
	 */
	ltu_start((a + 4) & 0x3fffffff,
	          BYTES_TO_LTUS(n) | (0x40000000 & (a + 4)));

}


#if BIGPKTS
/*
 * Routine:
 *	ltu_send_wait()
 *
 * Purpose:
 *	Waits for the send LTU to finish
 *
 *	Big Packet version allows us to process receive packets
 *	while waiting for a send LTU to complete.
 *
 * Returns:
 *	none
 */

ltu_send_wait()
{
	register int i = 0;
	register int recv_in_fifo = 0;

#if BUMPERS
	unsigned long ltu_count;
	unsigned long ltu_frozen_count = 0xffffffff;
	int frozen_timer = 0;
	int ltu_timer = 0;
#endif BUMPERS

	/*
	 * Wait for LTU 1 interrupt status bit to go high
	 */

	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT)) {

		{
			/*
			 * If we are waiting for a send to complete, and a receive
			 * comes in, handle it. Ensure send_waiting is non-zero
			 * to protect on-going send in the case that a receive
			 * method attempts to do yet another mcmsg_send().
			 *
			 * PTS #11502 fix: Once we've sensed something in the
			 * receive FIFO, check for the send LTU completed before
			 * handling the receive.  The packet to be received may
			 * have been sent in response to the send-in-progress,
			 * and may update state information that will not be
			 * consistent unless the post-processing of the send
			 * occurs first.  In the case of PTS #11502, this was due
			 * to the receipt of an NXC packet for the current
			 * send-in-progress before the post-processing of the send
			 * had placed the send on the send_wait queue for the
			 * remote pid.
			 */

			if (recv_in_fifo) {
				mcmsg_send_waiting = 1;
				mcmsg_intr_recv();
				mcmsg_send_waiting = 0;
			}
			recv_in_fifo = (!mcmsg_recv_waiting && mcmsg_recv_ready());
#if BUMPERS
			/*
			 * Detect LTU lockup on the send side.
			 */
			if (ltu_timer++ > 100000) {
				ltu_timer = 0;
				ltu_count = inl(DP_LTU1_COUNT) & 0x1ff;
				if (ltu_count == ltu_frozen_count) {
					frozen_timer++;
					if (frozen_timer == 8) {
						mcmsg_trace_drop("ltu send timeout", ltu_count);
						panic("ltu send timeout");
					}
				} else {
					ltu_frozen_count = ltu_count;
				}
			}
#endif BUMPERS
		}
	} /* end while */
}

#else BIGPKTS

/*
 * Routine:
 *	ltu_send_wait()
 *
 * Purpose:
 *	Waits for the send LTU to finish
 *
 * Returns:
 *	none
 */

ltu_send_wait()
{
	register int i = 0;

	/*
	 * Wait for LTU 1 interrupt status bit to go high
	 */

	while (!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT)) {
		/*
		 * In BURST mode, LTU transmits that do not fit in the NIC FIFO
		 * will not complete.  The assertion below should catch it.
		 */
		assert(i++ < 1000000);
	}

	/*
	 * Clear LTU 1 interrupt status bit
	 */

	inl(DP_LTU1_CLEAR_CNT);
	assert(!(inl(DP_STATUS_HI) & DP_ISTAT_LTU1_CNT));
}
#endif BIGPKTS

/*
 * Routine:
 *	mcmsg_send_ready()
 *
 * Purpose:
 *	Checks whether there is room for a packet in the NIC send FIFO.
 *
 * Returns:
 *	1 if NIC send FIFO ready for a packet
 *	0 if not
 */

mcmsg_send_ready()
{
	nic_reg	t;

	t.full = mcmsg_nic_status(1);

	/*
	 * Check for TX FIFO ready.
	 */

	if ((t.halfs.lo & SEND_INTR_MODE) != 0) {

		DISABLE_TX_FIFO;
		return 1;
	}
	return 0;
}

#if	BUMPERS

/*
 * Routine:
 *	send2eod_(a, b)
 *
 * Arguments:
 *	a:		Data
 *	b:		Data
 *
 * Purpose:
 *	A-step NIC workaround.
 *	Called through send2eod macro when bumpers enabled.
 *	Evens up the packet and appends baby bumpers.
 *
 * Returns:
 *	none
 */

send2eod_(a, b)
	long	a;
	long	b;
{
	nic_reg	t;
	int	i;

	SEND_WAIT;
	/*
	 * Send the data
	 */
	t.halfs.lo = a;
	t.halfs.hi = b;
	NIC.io.full = t.full;

	MCMSG_BUMPER_OUT;
	RELEASE_TX_FIFO;
}

/*
 * Routine:
 *	boot_send2eod(a, b)
 *
 * Arguments:
 *	a:		Data
 *	b:		Data
 *
 * Purpose:
 *	Replaces send2eod macro for boot sends with A-step NIC workaround.
 *	Boot messages do not use bumpers.
 *
 * Returns:
 *	none
 */

boot_send2eod(a, b)
	long	a;
	long	b;
{
	nic_reg	t;

	/*
	 * Send the data
	 */

	t.halfs.lo = a;
	t.halfs.hi = b;
	EOD.io.full = t.full;

	mcmsg_send_byte_count = 0;
	mcmsg_nic_status(11);

	RELEASE_TX_FIFO;
}

#endif	BUMPERS
