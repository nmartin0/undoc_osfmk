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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_hwi.c,v 1.16 1994/11/18 20:46:48 mtm Exp $
 */

/*
 * msgp_hwi.c
 *
 * Hardware support routines for multicomputer message passing
 */

#define MCMSG_HW_EXT
#include <i860paragon/msgp/msgp_hw.h>

#if	NIC_TRACE
char *mcmsg_nic_trace_name[] = {
	"?              ",	
	"send_ready     ",
	"recv_ready 1   ",
	"recv_bump      ",
	"eod_last       ",
	"flush_invalid  ",
	"end recv       ",
	"sendeod_       ",
	"init           ",
	"check          ",
	"intr_recv      ",
	"boot_send2eod  ",
	"eagersendpkt2  ",
	"bumper out     ",
	"?              ",
};
#endif	NIC_TRACE

/*
 * Routine:
 *	mcmsg_nic_status(place)
 *
 * Arguments:
 *	place:	code position identifier for debugging
 *
 * Purpose:
 *	Obtain NIC status
 *
 * Returns:
 *	NIC status register contents
 */

double
mcmsg_nic_status(place)
	unsigned long	place;
{
	nic_reg	t;

	/*
	 * Read NIC status
	 */

	t.full = NIC.status.full;

	/*
	 * Trace NIC status calls
	 */

#if	NIC_TRACE
	mcmsg_nic_trace_count++;
	mcmsg_nic_trace_index++;
	if (mcmsg_nic_trace_index >= NIC_TRACE_SIZE) {
		mcmsg_nic_trace_index = 0;
	}
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].status = t;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].rcount = 0;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].scount = 0;
	mcmsg_nic_trace_buf[mcmsg_nic_trace_index].place = place;
	assert((t.halfs.lo & NIC_STAT_ERRORS) == 0);
#endif	NIC_TRACE

	return t.full;
}

/*
 * Routine:
 *	mcmsg_hw_init()
 *
 * Purpose:
 *	Initialize message passing hardware.
 *
 * Returns:
 *	none
 */

mcmsg_hw_init()
{
	char	*s;
	extern char *getbootenv();

	/*
	 * Configure NIC operating mode
	 */

	mcmsg_nic_control.halfs.lo = 0;
	mcmsg_nic_control.halfs.hi = NIC_CNTRL_EN_iMRC | NIC_CNTRL_EN_ALMOST;

	if ((s = getbootenv("BOOT_NIC_MODE")) != 0) {
		switch (*s) {

		case '0':
		case 'i':	/* interlocked */
			break;

		case '1':
		case 's':	/* slow */
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
			break;

		case '2':
		case 'f':	/* fast */
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
			mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_SPEED;
			break;

		default:
			assert(0);
		}

	} else {	/* Default slow streaming */
		mcmsg_nic_control.halfs.hi |= NIC_CNTRL_EN_STREAM;
	}

	if ((s = getbootenv("BOOT_NIC_CONFIG")) != 0) {
		mcmsg_nic_config = atoi(s);
	} else {
#if    BIGPKTS
#if    BUMPERS
		/* NIC flags for Big Packets & BUMPERS:
		 *   128 doubles before XMIT fifo empty
		 *	  16 doubles before RECV fifo almost full
		 */
		mcmsg_nic_config = 0x0420;
#else  BUMPERS
		/* NIC flags for Big Packets & NIC-B:
		 *    16 doubles before XMIT fifo empty
		 *	  16 doubles before RECV fifo almost full
		 */
		mcmsg_nic_config = 0x043C;
#endif BUMPERS
#else  BIGPKTS
		/*
		 * Default NIC flags:
		 *     16 doubles before XMIT fifo empty
		 *      8 doubles before RECV fifo almost full
		 */
		mcmsg_nic_config = 0x023C;
#endif BIGPKTS
	}

	{
		nic_reg	t;

		/*
		 * Reset the NIC
		 */

		t.halfs.lo = 0;
		t.halfs.hi = NIC_CNTRL_RESET;
		NIC.set.full = t.full;

		/*
		 * Select the almost-full flag for receive MRC flow control
		 * (A-step NIC workaround, avoids a glitch on the default flag)
		 */

		t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
		t.halfs.hi = 0;
		NIC.reset_test.full = t.full;

		/*
		 * Set the FIFO flag offsets.
		 * Controls where in the FIFO the programmable "almost" flags
		 * will activate.
		 */

		t.halfs.lo = mcmsg_nic_config;
		t.halfs.hi = 0;
		NIC.fifo_offset.full = t.full;

		/*
		 * Set the NIC control register to
		 * configure the NIC
		 */

		NIC.set.full = mcmsg_nic_control.full;
	}

	mcmsg_hw.bump_hdr.halfs.lo = MCTRL_BUMP;
	mcmsg_hw.tx_stop.halfs.lo = NIC_TEST_DISABLE_TX | NIC_TEST_RCV_AFULL_SEL;
	mcmsg_hw.tx_stop.halfs.hi = 0;
	mcmsg_hw.tx_go.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
	mcmsg_hw.tx_go.halfs.hi = 0;
	mcmsg_hw.send_loop = getbootint("BOOT_SEND_LOOP", 10);


	/*
	 * Initialize baby bumper control variables
	 */

	mcmsg_hw.send_enable.halfs.lo = SEND_INTR_MODE;
	mcmsg_hw.send_enable.halfs.hi = 0;
	mcmsg_hw.recv_enable.halfs.lo = 0;
	mcmsg_hw.recv_enable.halfs.hi = 0;
	mcmsg_hw.recv_status = 0;
	mcmsg_hw.globtime = 0;

#if	BUMPERS
	mcmsg_recv_byte_count = 0;
	mcmsg_send_byte_count = 0;
	{
		nic_reg	t, u;

		/*
		 * Configure the NIC again (I don't know why)
		 *
		 * Reset, set flag, and configure as above.
		 */

		t.halfs.lo = 0;
		t.halfs.hi = NIC_CNTRL_RESET;
		NIC.set.full = t.full;

		t.halfs.lo = NIC_TEST_RCV_AFULL_SEL;
		t.halfs.hi = 0;
		NIC.reset_test.full = t.full;

		t.halfs.lo = mcmsg_nic_config;
		t.halfs.hi = 0;
		NIC.fifo_offset.full = t.full;

		NIC.set.full = mcmsg_nic_control.full;

		/*
		 * Trace status after configuration
		 */

		mcmsg_nic_status(8);

		/*
		 * Send a null message to ourselves to prime the bumpers.
		 */

		send2eod(0, 0);

		/*
		 * Now the NIC receive FIFO should contain just
		 * one set of bumpers
		 */

		t.full = NIC.control.full;
		u.full = NIC.status.full;
		mcmsg_trace_debug("NIC init", 4,
			t.halfs.hi, t.halfs.lo, u.halfs.hi, u.halfs.lo);
	}
#else	BUMPERS
	/*
	 * Non-BUMPERS kernel panics if it is loaded on NIC-A hardware,
	 * unless explicit bootmagic override NICB_SW_NICA_HW=1
	 */
	if (!getbootint("NICB_SW_NICA_HW", 0))
	{
		nic_reg r;
		int revision;

		r.full = mcmsg_nic_status(8);
		revision = (r.halfs.hi >> 3) & 0x7;

		if (revision == 0x1)	/* nic-A */
		{
			panic("NIC-A hardware requires BUMPERS kernel");
		}
	}
#endif	BUMPERS

	/*
	 * Obtain LTU configuration
	 */

	if ((s = getbootenv("BOOT_LTU_NODE")) != 0) {
		mcmsg_ltu_node = atoi(s);
	} else {
		mcmsg_ltu_node = ipsc_physnode;
	}

	mcmsg_ltu_enable = (ipsc_physnode == mcmsg_ltu_node);

#if BIGPKTS
	if (!mcmsg_ltu_enable) {
		panic("LTUs must be enabled with this kernel");
	}
#endif BIGPKTS

	/*
	 * Clear LTU interrupts.
	 */

	if (mcmsg_ltu_enable) {
		inl(DP_LTU0_CLEAR_CNT);
		inl(DP_LTU1_CLEAR_CNT);
		assert(!(inl(DP_STATUS_HI) & (DP_ISTAT_LTU0_CNT | DP_ISTAT_LTU1_CNT)));
	}

	/*
	 * Page-align mcmsg_pbuf_in/out
	 */

	mcmsg_pbuf_in = (long *)
		((((unsigned long)mcmsg_pbuf_space) + MSG_PAGE_SIZE-1) &
		MSG_PAGE_MASK);

	mcmsg_pbuf_out = (long *)((unsigned long)mcmsg_pbuf_in + MSG_PAGE_SIZE);
}

/*
 * Routine:
 *	mcmsg_nic_reset()
 *
 * Purpose:
 *	Reset NIC for debugging. Doesn't work very well.
 *
 * Returns:
 *	none
 */

mcmsg_nic_reset()
{
	nic_reg	t;

	t.halfs.lo = 0;
	t.halfs.hi = NIC_CNTRL_RESET;
	NIC.set.full = t.full;
	mcmsg_hw_init();
}

/*
 * Routine:
 *	do_calculate_route(physnode)
 *
 * Arguments:
 *	physnode:	Node number
 *
 * Purpose:
 *	Calculates the appropriate hardware routing header to send a message
 *	from this node to the given node.
 *
 * Returns:
 *	Routing header
 *
 * Cooments:
 *	Because of it's relative slowness this routine is no longer called
 *	from the MCMSG modules but instead used to fill a routing table at
 *	system startup time.
 *	
 */

do_calculate_route(physnode)
	register unsigned long	physnode;
{
	unsigned long	my_x, my_y;
	unsigned long	target_x, target_y;
	unsigned long	x, y;

	/*
	 * Calculate my x,y position in the mesh
	 */

	my_x = ipsc_physnode % paragon_mesh_x;
	my_y = ipsc_physnode / paragon_mesh_x;

	/*
	 * Calculate other node's x,y position in the mesh
	 */

	target_x = physnode % paragon_mesh_x;
	target_y = physnode / paragon_mesh_x;

	/*
	 * Calculate x displacement
	 */

	if (my_x > target_x) {
		x = (my_x - target_x) | NIC_WEST;
	} else {
		x = (target_x - my_x) | NIC_EAST;
	}

	/*
	 * Calculate y displacement
	 */

	if (my_y >= target_y) {
		y = (my_y - target_y) | NIC_NORTH;
	} else {
		y = (target_y - my_y) | NIC_SOUTH;
	}

	/*
	 * Assemble hardware routing header
	 */

	return (y << 16) | x;
}
