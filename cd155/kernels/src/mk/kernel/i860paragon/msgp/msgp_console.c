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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_console.c,v 1.13 1994/11/18 20:46:37 mtm Exp $
 */

#define	MCMSG_MODULE	MCMSG_MODULE_CONSOLE

/*
 * mcmsg_console.c
 *
 * console message support
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_console.h>
#include <mach/kern_return.h>

/*
 * Client side
 */

mcmsg_console_open(mt, node, task)
	mcmsg_task_t	*mt;
	long		node;
	task_t		task;
{
	int		i;

	for (i = 0; i < NUM_CONSOLE_CHAN; i++) {
		if (mcmsg_console_chan[i].task == 0) {
			mcmsg_console_chan[i].task = task;
			mcmsg_console_chan[i].node = node;
			mcmsg_console_chan[i].route = calculate_route(node);
			mcmsg_console_chan[i].rx_flag = 0;
			mcmsg_send(mt, MCTRL_CONW, i);
			return i;
		}
	}
	return -1;
}

mcmsg_console_close(mt, chan, task)
	mcmsg_task_t	*mt;
	unsigned long	chan;
	task_t		task;
{

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != task) {
		return KERN_NO_ACCESS;
	}
	mcmsg_console_chan[chan].task = 0;
	return KERN_SUCCESS;
}

mcmsg_console_write(mt, chan, c, task)
	mcmsg_task_t	*mt;
	unsigned long	chan;
	char		c;
	task_t		task;
{
	long		in;
	struct console_chan *cp;

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != task) {
		return KERN_NO_ACCESS;
	}
	cp = &mcmsg_console_chan[chan];
	mcmsg_send(mt, MCTRL_CONR, c, chan);
	return KERN_SUCCESS;
}

mcmsg_send_conr(mt, ctl, c, chan)
	mcmsg_task_t	*mt;
	register unsigned long	c;
	register unsigned long	chan;
{
	register unsigned long	hdr1;
	register unsigned long	route;
	register struct console_chan *cp;

	cp = &mcmsg_console_chan[chan];
	route = cp->route;
	hdr1 = MCTRL_CONR;
	mcmsg_trace_send(hdr1, c, 0, 2, chan, route);
	send2(route, 0);
	send2eod(hdr1, c);
}


mcmsg_console_read(mt, chan, task)
	mcmsg_task_t	*mt;
	unsigned long	chan;
	task_t		task;
{
	struct console_chan *cp;
	int		c;

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != task) {
		return -2;
	}
	cp = &mcmsg_console_chan[chan];
	if (!cp->rx_flag) {
		return -1;
	}
	c = cp->rx_char;
	cp->rx_flag = 0;
	mcmsg_send(mt, MCTRL_CONW, chan);
	return c;
}

mcmsg_send_conw(mt, ctl, chan)
	mcmsg_task_t	*mt;
	register unsigned long	chan;
{
	register unsigned long	hdr1;
	register unsigned long	route;
	register unsigned long	node;
	register task_t		task;

	route = mcmsg_console_chan[chan].route;
	hdr1 = MCTRL_CONW;
	node = ipsc_physnode;
	task = mcmsg_console_chan[chan].task;
	mcmsg_trace_send(hdr1, chan, 0, 2, chan, route);
	send2(route, 0);
	send2(hdr1, chan);
	send2eod(node, (long)(task));
}

mcmsg_recv_cont(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register struct console_chan *cp;

	mcmsg_trace_recv(hdr1, hdr2, 1, hdr2 >> 16, 0, 0);

	cp = &mcmsg_console_chan[hdr2 & 0xFFFF];
	if (cp->node == (hdr1 >> 16)) {
		cp->rx_char = hdr2 >> 16;
		cp->rx_flag = 1;
	} else {
		mcmsg_trace_debug("drop console character, bad node", 2,
			cp->node, hdr1 >> 16, 0, 0);
	}
}

/*
 * Server side
 */



mcmsg_console_do_put(mt)
	mcmsg_task_t	*mt;
{

	mcmsg_send(mt, MCTRL_CONT);
}

mcmsg_recv_conw(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	node;
	register unsigned long	id;

	recv2(node, id);
	mcmsg_trace_recv(hdr1, hdr2, 0, 2, node, id);

	if (mcmsg_console_state.tx_id == 0) {
		mcmsg_console_state.tx_node = node-1;
	}
	if (mcmsg_console_state.tx_node != node) {
		mcmsg_console_state.tx_node = node;
		mcmsg_console_state.tx_route = calculate_route(node);
	}
	mcmsg_console_state.tx_id = id;
	mcmsg_console_state.tx_chan = hdr2;
	mcmsg_console_state.tx_flag = 1;
	mcmsg_console_need_int = 1;
#if	LOOPBACK_CONSOLE
	if (mcmsg_console_state.rx_flag) {
		if (mcmsg_mp_enable) {
			mcmsg_console_state.tx_char =
				mcmsg_console_state.rx_char;
			mcmsg_console_state.tx_flag = 0;
			mcmsg_console_state.rx_flag = 0;
			mcmsg_send(0, MCTRL_CONT);
		} else {
			assert(--mcmsg_reentry == 0);
			mcmsg_console_put(mcmsg_console_get());
			assert(mcmsg_reentry++ == 0);
		}
	}
#endif	LOOPBACK_CONSOLE
}

mcmsg_send_cont(mt, ctl)
	mcmsg_task_t	*mt;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;

	route = mcmsg_console_state.tx_route;
	hdr1 = MCTRL_CONT | ipsc_physnode << 16;
	hdr2 = mcmsg_console_state.tx_chan | mcmsg_console_state.tx_char << 16;
	mcmsg_trace_send(hdr1, hdr2, 0, 1, route, 0);
	send2(route, 0);
	send2eod(hdr1, hdr2);
}

mcmsg_recv_conr(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	node;
	register unsigned long	dummy;
	register unsigned long	route;

	mcmsg_trace_recv(hdr1, hdr2, 0, 0, 0, 0);

	mcmsg_console_state.rx_flag = 1;
	mcmsg_console_state.rx_char = hdr2;
	mcmsg_console_need_int = 1;
#if	LOOPBACK_CONSOLE
	if (mcmsg_console_state.tx_flag) {
		if (mcmsg_mp_enable) {
			mcmsg_console_state.tx_char =
				mcmsg_console_state.rx_char;
			mcmsg_console_state.tx_flag = 0;
			mcmsg_console_state.rx_flag = 0;
			mcmsg_send(0, MCTRL_CONT);
		} else {
			assert(--mcmsg_reentry == 0);
			mcmsg_console_put(mcmsg_console_get());
			assert(mcmsg_reentry++ == 0);
		}
	}
#endif	LOOPBACK_CONSOLE

	return;
}
