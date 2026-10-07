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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_boot.c,v 1.15 1994/11/18 20:41:09 mtm Exp $
 */

/*
 * mcmsg_boot.c
 *
 * boot message support
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

#define	BOOT_DELAY	PARAGON860

#if	BOOT_DELAY

unsigned long mcmsg_boot_delay_bt2 = 100000000;
unsigned long mcmsg_boot_delay_btx = 500;

#endif	BOOT_DELAY

int	mcmsg_boot_count[4];

struct {
	unsigned long		node;
	unsigned long		flags;
	unsigned long		control;
	unsigned long		count;
#if	BOOT_DELAY
	unsigned long		when[2];
#endif	BOOT_DELAY
	unsigned char		*bufp;
	unsigned char		buf[4096];
} mcmsg_boot_request;

syscall_mcmsg_boot_send(host, node, flags, control, sequence, buf, count)
	mach_port_t		host;
	unsigned long		node;
	unsigned long		flags;
	unsigned long		control;
	unsigned long		sequence;
	unsigned long		buf;
	long			count;
{
	register int		x;
	register select_item_t	*si;

	if ((control & 0xFFFF) < MCTRL_ALLOW_LO ||
	    (control & 0xFFFF) > MCTRL_ALLOW_HI) {
		return -1;
	}
	if ((count & (PKT_GRAN-1)) != 0) {
		return -1;
	}
	if (count > sizeof(mcmsg_boot_request.buf)) {
		return -1;
	}
	if (mcmsg_boot_request.control != 0) {
		return 0;
	}
#if	BOOT_DELAY
	{
		unsigned long	now[2];

		mcmsg_hwclock(now);
		if (now[1] < mcmsg_boot_request.when[1] ||
		    (now[1] == mcmsg_boot_request.when[1] &&
		     now[0] < mcmsg_boot_request.when[0])) {
			return 0;
		}
	}
#endif	BOOT_DELAY

	mcmsg_boot_request.node = node;
	mcmsg_boot_request.flags = flags;
	mcmsg_boot_request.count = count | (sequence << 16);
	mcmsg_boot_request.bufp = (unsigned char *)
		mcmsg_validate_real(mcmsg_boot_request.buf,
				    current_task()->map->pmap->dirbase,
				    __FILE__,
				    __LINE__);
	bcopy(buf, mcmsg_boot_request.buf, count);

	mcmsg_boot_request.control = control;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	mcmsg_phys = 0;
	mcmsg_task = 0;

	mcmsg_send(SENDMETH_BOOT, 0, 0);

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return 1;
}

mcmsg_send_boot(dummy1, dummy2)
	register unsigned long	dummy1;
	register unsigned long	dummy2;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;
	register unsigned long	count;

	hdr1 = mcmsg_boot_request.control;
	hdr2 = mcmsg_boot_request.count;
	count = hdr2 & 0xFFFF;
	route = calculate_route(mcmsg_boot_request.node);
#if	PARAGON860
	if (mcmsg_boot_request.flags) {
		route |= NIC_BROADCAST | (NIC_BROADCAST << 16);
	}
#endif	PARAGON860
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

#if	BOOT_DELAY
	{
		unsigned long		when[2];
		register unsigned long	n0;

		mcmsg_hwclock(when);
		if (mcmsg_boot_request.control == MCTRL_BT2) {
			n0 = when[0] + mcmsg_boot_delay_bt2;
		} else {
			n0 = when[0] + mcmsg_boot_delay_btx;
		}
		if (n0 < when[0]) {
			when[1]++;
		}
		mcmsg_boot_request.when[0] = n0;
		mcmsg_boot_request.when[1] = when[1];
	}
#endif	BOOT_DELAY

	mcmsg_boot_request.control = 0;
	return 0;
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
#endif	BUMPERS

	return 0;
}

