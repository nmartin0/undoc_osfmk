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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_intr.c,v 1.7 1994/11/18 20:41:32 mtm Exp $
 */

/*
 * mcmsg_intr.c
 *
 * Interrupt handler for multicomputer message passing
 */

#if	PARAGON860

#include <kern/assert.h>
#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>
#include <i860ipsc/mcmsg/mcmsg_nx.h>

int mcmsg_unexpected_send_intr;
extern mcmsg_console_need_int;
extern mcmsg_int_enables;
extern mcmsg_recv_enable;

#if	!MACH_ASSERT
int	mcmsg_reentry;
#endif	MACH_ASSERT

#define DEBUG_SEND_QUEUE 0

#if    DEBUG_SEND_QUEUE
#define MCMSG_TRACE_QUEUE(s,n,a,b,c,d) mcmsg_trace_debug(s,n,a,b,c,d)
#else  DEBUG_SEND_QUEUE
#define MCMSG_TRACE_QUEUE(s,n,a,b,c,d)
#endif DEBUG_SEND_QUEUE


#define mcmsg_send_switch(m, i, s) mcmsg_send_sw[m](i, s)

/*
 * Packet.h has macros for every packet type.
 * Here we use it to make external declarations for the packet send routines.
 */

#define PACKET_DEFINE(n, c, s, name, sname, send, recv) extern int send();
#define PACKET_NULL(x)
#define PACKET_FILL(x)
#define PACKET_END(x)

#include <i860ipsc/mcmsg/mcmsg_packet.h>

mcmsg_send_sw_null(item, sequence)
	void	*item;
	unsigned long sequence;
{

	return 0;
}

mcmsg_send_sw_err(item, sequence)
	void	*item;
	unsigned long sequence;
{

	assert(0);
	return 0;
}

/*
 * Packet.h has macros for every packet type.
 * Here we use it to fill in the mcmsg_send_sw array with pointers to
 * the packet send routines.
 */

extern int mcmsg_send_rk1a();

#define PACKET_DEFINE(n, c, s, name, sname, send, recv) send,
#define PACKET_NULL(x)	mcmsg_send_sw_null,
#define PACKET_FILL(x)	mcmsg_send_sw_err,
#define PACKET_END(x)


int (*mcmsg_send_sw[])() = {

#include <i860ipsc/mcmsg/mcmsg_packet.h>

	mcmsg_send_rk1a
};


int nic_interrupt(regs)
{
	register int	s;

	s = splmsg_noi();
	mcmsg_intr();
	return s;
}

nic_interrupt_poll()
{
	register int	s;

	s = splmsg();
	mcmsg_intr();
	splx(s);
}

/*
 *	Routine:
 *		mcmsg_intr
 *
 *	Purpose:
 *		Main dispatch from NIC interrupt.
 *		
 *	Returns:
 *		None.
 */
mcmsg_intr()
{
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);

	/*
	 * Process receive interrupts.
	 */
	if ((mcmsg_int_enables & 2) && mcmsg_recv_ready()) {
		register unsigned long	hdr1;
		register unsigned long	hdr2;
		register unsigned long	junk1;
		register unsigned long	junk2;

		recv2(hdr1, hdr2);
		switch (hdr1 & 0xFFFF) {

/*
 * Packet.h has macros for every packet type.
 * Here we use it to fill in the switch cases.
 * Each one ends up looking like this example:
 *
 *	case MCTRL_FOO:
 *		mcmsg_recv_foo(hdr1, hdr2);
 *		if (MCTRL_FOO != MCTRL_IPC && MCTRL_FOO < MCTRL_BT0)
 *			assert_mcmsg_eod_last();
 *		break;
 *
 * Note that the compiler should be able to eliminate the if statement
 * after constant folding.
 */

#define PACKET_DEFINE(n, control, sendmeth, name, sendname, send, recv) \
case control: recv(hdr1, hdr2); \
	if (control != MCTRL_IPC && control < MCTRL_BT0) \
		assert_mcmsg_eod_last(); \
	break;

#define PACKET_NULL(x)
#define PACKET_FILL(x)
#define PACKET_END(x)

#include "mcmsg_packet.h"

		default:
			mcmsg_trace_drop("invalid message",
					 hdr1 & 0xFFFF);
			mcmsg_trace_debug("invalid msg data", 2,
					hdr1, hdr2, 0, 0);
			mcmsg_flush_invalid();
		}
		mcmsg_nic_status(6);
		hang_time++;
	}

	/*
	 * Process send Interrupts.
	 */
	if ((mcmsg_int_enables & 1) && mcmsg_send_ready()) {
		register unsigned char	new_out;
		register long		method;

		/*
		 * Take sends from Send Queue
		 */
		if ((new_out = mcmsg_send_store_out) != mcmsg_send_store_in ) {

			mcmsg_task = mcmsg_send_store[new_out].mcmsg_task;
			if (mcmsg_task != 0) {
				mcmsg_phys = current_task() != mcmsg_task->task;
			}

			MCMSG_TRACE_QUEUE("intr send from q", 1, new_out, 0, 0, 0);

			method = mcmsg_send_switch(
				mcmsg_send_store[new_out].method,
				mcmsg_send_store[new_out].item,
				mcmsg_send_store[new_out].sequence);

			if (method == 0) {

				MCMSG_TRACE_QUEUE("intr deq send", 1, new_out, 0, 0, 0);

				mcmsg_send_store_out = new_out + 1;
				if (mcmsg_send_store_out == mcmsg_send_store_in) {
					mcmsg_disable_tx_interrupts();
				} else {
					hang_time++;
				}
			} else {
				mcmsg_send_store[new_out].method = method;
				hang_time++;
			}
		} else {
			mcmsg_unexpected_send_intr++;
			mcmsg_disable_tx_interrupts();
		}
	}

	if (mcmsg_flush) {
		flush();
		mcmsg_flush = 0;
	}

	if (mcmsg_console_need_int) {
		mcmsg_console_ints();
	}

	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
}


/*
 *	Routine:
 *		mcmsg_send
 *
 *	Purpose:
 *		Send Message until network is busy or message done.
 *		If not done
 *			If send queue is full
 *				Force out a message
 *			Enqueue the send
 *		Endif
 *		
 *	Returns:
 *		None.
 */
mcmsg_send(method, item, sequence)
	int	method;
	void	*item;
	unsigned long sequence;
{
	register unsigned char	in;
	register unsigned char	new_in;
	register long		t;

	if (mcmsg_send_store_in == mcmsg_send_store_out) {
		assert((t = MAXLOOP) != 0);

		/* the send queues are empty, send this one */

		while (mcmsg_send_ready()) {
			method = mcmsg_send_switch(method, item, sequence);
			if (method == 0) {
				return;
			}
			assert(t-- != 0);
		}
	}

	in = mcmsg_send_store_in;
	new_in = in + 1;
	if (new_in == mcmsg_send_store_out) {

		/*
		 * Send Queue is full, force one out.
		 */
		assert((t = MAXLOOP) != 0);
		MCMSG_TRACE_QUEUE("send from q", 1, new_in, 0, 0, 0);

		do {
			register long		u;

			assert((u = 1000000) != 0);
			while (!mcmsg_send_ready()) {
				assert(u-- != 0);
			}
			method = mcmsg_send_switch(
				mcmsg_send_store[new_in].method,
				mcmsg_send_store[new_in].item,
				mcmsg_send_store[new_in].sequence);

			assert(t-- != 0);
		} while (method != 0);

		MCMSG_TRACE_QUEUE("deq send", 1, new_in, 0, 0, 0);
		mcmsg_send_store_out++;
	}

	/*
	 * Queue send.
	 */
	mcmsg_send_store[in].method = method;
	mcmsg_send_store[in].item = item;
	mcmsg_send_store[in].sequence = sequence;
	mcmsg_send_store[in].mcmsg_task = mcmsg_task;

	MCMSG_TRACE_QUEUE("queue send", 4, method, item, sequence, in);

	mcmsg_send_store_in = new_in;
	mcmsg_enable_tx_interrupts();
	return;
}

/*
 *	Routine:
 *		mcmsg_send_now
 *
 *	Purpose:
 *		Send message now, bypass send queues.
 *		
 *	Returns:
 *		1	send completed
 *		0	send not completed
 */
mcmsg_send_now(method, item, sequence)
	int	method;
	void	*item;
	unsigned long sequence;
{
	register long 	t;

	assert((t = MAXLOOP) != 0);
	while (mcmsg_send_ready()) {
		method = mcmsg_send_switch(method, item, sequence);
		if (method == 0) {
			return 1;
		}
		if (t == 0) {
			mcmsg_trace_debug("now loopout", 3, method, item, sequence, 0);
		}
		assert(t-- != 0);
	}
	return 0;
}

zap_send_store(index)
	int	index;
{

	mcmsg_send_store[index].method = SENDMETH_NULL;
}

#endif	PARAGON860
