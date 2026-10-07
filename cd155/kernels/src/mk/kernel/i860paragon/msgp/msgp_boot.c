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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_boot.c,v 1.22 1995/03/09 02:02:40 lenb Exp $
 */

/*
 * mcmsg_boot.c
 *
 * boot message support
 */

#define	MCMSG_MODULE	MCMSG_MODULE_BOOT

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_boot.h>
#include <i860paragon/msgp/msgp_hw.h>


#if	BOOT_DELAY
/*
 * NIC-A boot delays. [usec] to prevent HW throttling bug.
 * new bootmesh doesn't use BT2 or its delay.
 */
unsigned long mcmsg_boot_delay_bt2;
unsigned long mcmsg_boot_delay_btx;
#endif	BOOT_DELAY

mcmsg_boot_send()
{
	mcmsg_send(0, MCTRL_BT0, 0, 0);
}

mcmsg_send_boot(mt, ctl, dummy1, dummy2)
	mcmsg_task_t	*mt;
	register unsigned long	dummy1;
	register unsigned long	dummy2;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;
	register unsigned long	count;
#if	BIGPKTS
	extern int mcmsg_recv_waiting;
#endif	BIGPKTS

	hdr1 = mcmsg_boot_request.control;
	hdr2 = mcmsg_boot_request.count;
	count = hdr2 & 0xFFFF;
	route = calculate_route(mcmsg_boot_request.node);
	if (mcmsg_boot_request.flags) {
		route |= NIC_BROADCAST | (NIC_BROADCAST << 16);
	}
#if	BIGPKTS
	/*
	 * broadcast boot packets come back to our own receive FIFO
	 * but we can't concurrent receive them b/c they can be odd lengths
	 * and we can't prevent the PIO EOD at the end.
	 */
	mcmsg_recv_waiting = 1;
#endif	BIGPKTS

	mcmsg_trace_send(hdr1, hdr2, 0, 2, route, mcmsg_boot_request.node);
	send2(route, 0);
	if (count > 0) {
		send2(hdr1, hdr2);
#if	BUMPERS
		mcmsg_fifo_out(mcmsg_boot_request.bufp, count-8);
		boot_send2eod(
			*(unsigned long *)&mcmsg_boot_request.buf[count-8],
			*(unsigned long *)&mcmsg_boot_request.buf[count-4]);
#else	BUMPERS
		mcmsg_fifo_out_eod(mcmsg_boot_request.bufp, count);
#endif	BUMPERS
	} else {
#if	BUMPERS
		boot_send2eod(hdr1, hdr2);
#else	BUMPERS
		send2eod(hdr1, hdr2);
#endif	BUMPERS
	}

#if	BIGPKTS
	/* End of Protect Boot Packets from Concurrent Send/Recv */
	mcmsg_recv_waiting = 0;
#endif	BIGPKTS

#if	BOOT_DELAY
	{
		double delay;
		extern double dclock();

		if (mcmsg_boot_request.control == MCTRL_BT2) {
			delay = (double)mcmsg_boot_delay_bt2 * 1e-6; /* usec */
		} else {
			delay = (double)mcmsg_boot_delay_btx * 1e-6; /* usec */
		}

		mcmsg_boot_request.when = dclock() + delay;
	}
#endif	BOOT_DELAY

	/*
	 * if xmit was not a broadcast, then signal that we're done
	 * (for broadcast we're not done till we drained the receive)
	 */
	if (!mcmsg_boot_request.flags) {
		mcmsg_boot_request.control = 0;
	}
	return;
}


mcmsg_recv_boot(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	mcmsg_trace_recv(hdr1, hdr2, 0, 0, 0, 0);
	mcmsg_fifo_flush(hdr2 & 0xFFFF);
	mcmsg_boot_count[hdr1 & 3]++;
#if	BUMPERS
	mcmsg_recv_byte_count = 0;
	mcmsg_hw.recv_status = (NIC_STAT_RX_FIFO_ALMOST_FULL | NIC_STAT_EOD_IN_NIC);
#endif	BUMPERS
	/*
	 * Signal that we're done
	 */
	mcmsg_boot_request.control = 0;

	return 0;
}

