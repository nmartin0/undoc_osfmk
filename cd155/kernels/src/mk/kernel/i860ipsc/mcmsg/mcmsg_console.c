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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_console.c,v 1.11 1994/11/18 20:41:11 mtm Exp $
 */
#define TRACE_CONSOLE_CLNT 0
#define TRACE_CONSOLE_SVR  0
#define LOOPBACK_CONSOLE iPSC860

/*
 * mcmsg_console.c
 *
 * console message support
 */

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>
#include <mach/kern_return.h>

/*
polling:

	boolean_t mcmsg_console_tx_rdy()

	Returns TRUE when the message passing system's console
	transmit buffer is not full.


	boolean_t mcmsg_console_rx_rdy()

	Returns TRUE when the message passing system's console
	receive buffer is not empty.


interrupts:

	void mcmsg_console_tx_int()

	Upcall made by the message passing system if console transmit
	interrupts are enabled and the transmit buffer is not full.


	void mcmsg_console_rx_int()

	Upcall made by the message passing system if console receive
	interrupts are enabled and the receive buffer is not empty.

control:

	void mcmsg_console_tx_enb()

	Enables message passing console transmit interrupts.


	void mcmsg_console_tx_dis()

	Disables message passing console transmit interrupts.


	#if	0
	void mcmsg_console_rx_enb()

	Enables message passing console receive interrupts.


	void mcmsg_console_rx_dis()

	Disables message passing console receive interrupts.
	#endif	0

I/O:

	dont_look_t mcmsg_console_put(c)
	int c;

	Transmit character c.  It is an error to call mcmsg_console_put()
	when a call to mcmsg_console_tx_rdy() would return FALSE.


	int mcmsg_console_get()

	Receive a character.  It is an error to call mcmsg_console_get()
	when a call to mcmsg_console_rx_rdy() would return FALSE.

*/


/*
 * Client side
 */

#define NUM_CONSOLE_CHAN	4
#define CONSOLE_BUF_SIZE	8

struct console_chan {
	task_t		task;
	unsigned long	give;
	unsigned long	route;
	long		tx_flag;
	long		tx_in;
	long		tx_out;
	char		tx_buf[CONSOLE_BUF_SIZE];
	long		rx_in;
	long		rx_out;
	char		rx_buf[CONSOLE_BUF_SIZE];
} mcmsg_console_chan[NUM_CONSOLE_CHAN];

int mcmsg_console_ready;
int mcmsg_console_stopped;
int mcmsg_console_need_int;

syscall_mcmsg_console_open(node, chanp)
	long		node;
	unsigned long	*chanp;
{
	int		i;
	int		x;
	task_t		task;

	task = current_task();
	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	for (i = 0; i < NUM_CONSOLE_CHAN; i++) {
		if (mcmsg_console_chan[i].task == 0) {
			mcmsg_console_chan[i].task = task;
			mcmsg_console_chan[i].route = calculate_route(node);
			mcmsg_console_chan[i].tx_flag = 0;
			mcmsg_console_chan[i].tx_in = 0;
			mcmsg_console_chan[i].tx_out = 0;
			mcmsg_console_chan[i].rx_in = 0;
			mcmsg_console_chan[i].rx_out = 0;
			mcmsg_console_chan[i].give = 1;
			mcmsg_send(SENDMETH_CONW, 0, i);
			assert(mcmsg_reentry--);
			RED_OFF(RED_MSG);
#if	TRACE_CONSOLE_CLNT
			mcmsg_trace_debug("console open", 1, i, 0, 0, 0);
#endif	TRACE_CONSOLE_CLNT
			splx(x);
			*chanp = i;
			return KERN_SUCCESS;
		}
	}
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return KERN_FAILURE;
}

syscall_mcmsg_console_close(chan)
	unsigned long	chan;
{

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != current_task()) {
		return KERN_NO_ACCESS;
	}
#if	TRACE_CONSOLE_CLNT
	mcmsg_trace_debug("console close", 1, chan, 0, 0, 0);
#endif	TRACE_CONSOLE_CLNT
	mcmsg_console_chan[chan].task = 0;
	return KERN_SUCCESS;
}

syscall_mcmsg_console_read(chan, buf)
	unsigned long	chan;
	char		*buf;
{
	struct console_chan *cp;
	char		c;
	int		x;

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != current_task()) {
		return KERN_NO_ACCESS;
	}
	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	cp = &mcmsg_console_chan[chan];
	if (cp->rx_out == cp->rx_in) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return KERN_FAILURE;
	}
	c = cp->rx_buf[cp->rx_out];
	cp->rx_out++;
	if (cp->rx_out == CONSOLE_BUF_SIZE) {
		cp->rx_out = 0;
	}
	cp->give++;
	mcmsg_send(SENDMETH_CONW, 0, chan);
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	*buf = c;
#if	TRACE_CONSOLE_CLNT
	mcmsg_trace_debug("console read", 2, chan, c, 0, 0);
#endif	TRACE_CONSOLE_CLNT
	assert(get_spl() == 0);
	return KERN_SUCCESS;
}

syscall_mcmsg_console_write(chan, c)
	unsigned long	chan;
	char		c;
{
	long		in;
	struct console_chan *cp;
	int		x;

	if (chan >= NUM_CONSOLE_CHAN ||
	    mcmsg_console_chan[chan].task != current_task()) {
		return KERN_NO_ACCESS;
	}
	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	cp = &mcmsg_console_chan[chan];
	in = cp->tx_in + 1;
	if (in == CONSOLE_BUF_SIZE) {
		in = 0;
	}
	if (in == cp->tx_out) {
		assert(mcmsg_reentry--);
		RED_OFF(RED_MSG);
		splx(x);
		return KERN_FAILURE;
	}
	cp->tx_buf[cp->tx_in] = c;
	cp->tx_in = in;
	if (cp->tx_flag == 0) {
		cp->tx_flag = 1;
		mcmsg_send(SENDMETH_CONR, 0, chan);
	}
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
	return KERN_SUCCESS;
}


mcmsg_send_conw(dummy1, chan)
	register unsigned long	dummy1;
	register unsigned long	chan;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;
	register unsigned long	node;
	register task_t		task;

	route = mcmsg_console_chan[chan].route;
	hdr1 = MCTRL_CONW | mcmsg_console_chan[chan].give << 16;
	hdr2 = 8 | chan << 16;
	node = ipsc_physnode;
	task = mcmsg_console_chan[chan].task;
	mcmsg_console_chan[chan].give = 0;
#if	TRACE_CONSOLE_CLNT
	mcmsg_trace_send(hdr1, hdr2, 0, 2, chan, route);
#endif	TRACE_CONSOLE_CLNT
	send2(route, 0);
	send2(hdr1, hdr2);
	send2eod(node, (long)(task));
	return 0;
}

mcmsg_send_conr(dummy1, chan)
	register unsigned long	dummy1;
	register unsigned long	chan;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;
	register struct console_chan *cp;
	register unsigned long	c;

	cp = &mcmsg_console_chan[chan];
	c = cp->tx_buf[cp->tx_out];
	cp->tx_out++;
	if (cp->tx_out == CONSOLE_BUF_SIZE) {
		cp->tx_out = 0;
	}

	route = cp->route;
	hdr1 = MCTRL_CONR | c << 16;
	hdr2 = chan << 16;
#if	TRACE_CONSOLE_CLNT
	mcmsg_trace_send(hdr1, hdr2, 0, 2, chan, route);
#endif	TRACE_CONSOLE_CLNT
	send2(route, 0);
	send2eod(hdr1, hdr2);

	if (cp->tx_out != cp->tx_in) {
		return SENDMETH_CONR;
	}
	cp->tx_flag = 0;
	return 0;
}

mcmsg_recv_cont(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register struct console_chan *cp;

#if	TRACE_CONSOLE_CLNT
	mcmsg_trace_recv(hdr1, hdr2, 1, hdr2 >> 16, 0, 0);
#endif	TRACE_CONSOLE_CLNT

	cp = &mcmsg_console_chan[hdr2 >> 16];
	cp->rx_buf[cp->rx_in] = hdr1 >> 16;
	cp->rx_in++;
	if (cp->rx_in == CONSOLE_BUF_SIZE) {
		cp->rx_in = 0;
	}
	return 0;
}

/*
 * Server side
 */


struct {
	boolean_t	tx_enable;
	boolean_t	rx_enable;
	long		tx_chan;
	long		tx_route;
	long		tx_id;
	long		tx_avail;
	boolean_t	tx_flag;
	long		tx_char;
	boolean_t	rx_flag;
	long		rx_char;
} mcmsg_console_state;

mcmsg_console_init()
{

	mcmsg_console_ready = 1;
	mcmsg_console_stopped = 0;
	mcmsg_console_need_int = 0;
}

mcmsg_console_tx_enb()
{

	mcmsg_console_state.tx_enable = 1;
	mcmsg_console_stopped = 0;
}

mcmsg_console_tx_dis()
{

	mcmsg_console_state.tx_enable = 0;
	if (!mcmsg_console_state.rx_enable) {
		mcmsg_console_stopped = 1;
	}
}

mcmsg_console_rx_enb()
{

	mcmsg_console_state.rx_enable = 1;
	mcmsg_console_stopped = 0;
}

mcmsg_console_rx_dis()
{

	mcmsg_console_state.rx_enable = 0;
	if (!mcmsg_console_state.tx_enable) {
		mcmsg_console_stopped = 1;
	}
}

boolean_t
mcmsg_console_tx_rdy()
{
	boolean_t	tx_save;
	boolean_t	rx_save;
	int		reentry_save;

	if (!mcmsg_console_ready) {
		return 0;
	}

	tx_save = mcmsg_console_state.tx_enable;
	mcmsg_console_state.tx_enable = 0;
	rx_save = mcmsg_console_state.rx_enable;
	mcmsg_console_state.rx_enable = 0;
#if	MACH_ASSERT
	reentry_save = mcmsg_reentry;
	mcmsg_reentry = 0;
#endif	MACH_ASSERT

#if	PARAGON860

	nic_interrupt_poll();

#else	PARAGON860

	if (mcmsg_recv_ready()) {
		dcm_recv_intr();
	}
	if (mcmsg_send_ready()) {
		dcm_send_intr();
	}

#endif	PARAGON860

	if (mcmsg_console_state.tx_avail > 0 &&
	    !mcmsg_console_state.tx_flag) {
		mcmsg_console_state.tx_flag = 1;
		mcmsg_console_state.tx_avail--;
	}

	mcmsg_console_state.tx_enable = tx_save;
	mcmsg_console_state.rx_enable = rx_save;
#if	MACH_ASSERT
	mcmsg_reentry = reentry_save;
#endif	MACH_ASSERT

	return mcmsg_console_state.tx_flag;
}

mcmsg_console_rx_rdy()
{
	boolean_t	tx_save;
	boolean_t	rx_save;
	int		reentry_save;

	if (!mcmsg_console_ready) {
		return 0;
	}

	tx_save = mcmsg_console_state.tx_enable;
	mcmsg_console_state.tx_enable = 0;
	rx_save = mcmsg_console_state.rx_enable;
	mcmsg_console_state.rx_enable = 0;
#if	MACH_ASSERT
	reentry_save = mcmsg_reentry;
	mcmsg_reentry = 0;
#endif	MACH_ASSERT

#if	PARAGON860

	nic_interrupt_poll();

#else	PARAGON860

	if (mcmsg_recv_ready()) {
		dcm_recv_intr();
	}
	if (mcmsg_send_ready()) {
		dcm_send_intr();
	}

#endif	PARAGON860

	mcmsg_console_state.tx_enable = tx_save;
	mcmsg_console_state.rx_enable = rx_save;
#if	MACH_ASSERT
	mcmsg_reentry = reentry_save;
#endif	MACH_ASSERT

	return mcmsg_console_state.rx_flag;
}

mcmsg_console_put(c)
	char	c;
{

	if (!mcmsg_console_ready) {
		return;
	}

	if (!mcmsg_console_state.tx_flag) {
		mcmsg_trace_drop("console put", mcmsg_console_state.tx_flag);
	}
	mcmsg_console_state.tx_flag = 0;
	mcmsg_console_state.tx_char = c;
	mcmsg_send(SENDMETH_CONT, 0, 0);
}

mcmsg_console_get()
{

	if (!mcmsg_console_state.rx_flag) {
		mcmsg_trace_drop("console get", mcmsg_console_state.rx_flag);
	}
	mcmsg_console_state.rx_flag = 0;
	return mcmsg_console_state.rx_char;
}

mcmsg_send_cont(dummy1, dummy2)
	register unsigned long	dummy1;
	register unsigned long	dummy2;
{
	register unsigned long	hdr1;
	register unsigned long	hdr2;
	register unsigned long	route;

	route = mcmsg_console_state.tx_route;
	hdr1 = MCTRL_CONT | mcmsg_console_state.tx_char << 16;
	hdr2 = mcmsg_console_state.tx_chan << 16;
#if	TRACE_CONSOLE_SVR
	mcmsg_trace_send(hdr1, hdr2, 0, 1, route, 0);
#endif	TRACE_CONSOLE_SVR
	send2(route, 0);
	send2eod(hdr1, hdr2);

	return 0;
}

mcmsg_recv_conw(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	node;
	register unsigned long	id;
	register unsigned long	route;

	recv2(node, id);
#if	TRACE_CONSOLE_SVR
	mcmsg_trace_recv(hdr1, hdr2, 0, 2, node, id);
#endif	TRACE_CONSOLE_SVR

	route = calculate_route(node);
	if (mcmsg_console_state.tx_route != route ||
	    mcmsg_console_state.tx_id != id) {
		mcmsg_console_state.tx_route = route;
		mcmsg_console_state.tx_id = id;
		mcmsg_console_state.tx_chan = hdr2 >> 16;
		mcmsg_console_state.tx_avail = 0;
	}
	mcmsg_console_state.tx_avail += hdr1 >> 16;
	if (!mcmsg_console_state.tx_flag) {
		mcmsg_console_state.tx_flag = 1;
		mcmsg_console_state.tx_avail--;
		mcmsg_console_need_int = 1;
	}
#if	LOOPBACK_CONSOLE
	if (mcmsg_console_state.rx_flag) {
		mcmsg_console_put(mcmsg_console_get());
	}
#endif	LOOPBACK_CONSOLE

	return 0;
}

mcmsg_recv_conr(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register unsigned long	node;
	register unsigned long	dummy;
	register unsigned long	route;

#if	TRACE_CONSOLE_SVR
	mcmsg_trace_recv(hdr1, hdr2, 0, 0, 0, 0);
#endif	TRACE_CONSOLE_SVR

	mcmsg_console_state.rx_flag = 1;
	mcmsg_console_state.rx_char = hdr1 >> 16;
	mcmsg_console_need_int = 1;
#if	LOOPBACK_CONSOLE
	if (mcmsg_console_state.tx_flag) {
		mcmsg_console_put(mcmsg_console_get());
	}
#endif	LOOPBACK_CONSOLE

	return 0;
}

mcmsg_console_ints()
{

	if (mcmsg_console_state.rx_flag && mcmsg_console_state.rx_enable) {
		mcmsg_console_rx_int();
	}
	if (mcmsg_console_state.tx_flag && mcmsg_console_state.tx_enable) {
		mcmsg_console_tx_int();
	}
	mcmsg_console_need_int = 0;
}

#if	LOOPBACK_CONSOLE

mcmsg_console_tx_int()
{
}

mcmsg_console_rx_int()
{
}
#endif	LOOPBACK_CONSOLE
