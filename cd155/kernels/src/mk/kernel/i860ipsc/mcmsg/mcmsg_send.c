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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_send.c,v 1.21 1994/11/18 20:41:58 mtm Exp $
 */

/*
 * mcmsg_send.c
 *
 * Send interrupt handler for multicomputer message passing
 */

#if	iPSC860

#include <i860ipsc/mcmsg/mcmsg_ext.h>
#include <i860ipsc/mcmsg/mcmsg_hw.h>

int mcmsg_unexpected_send_intr;

dcm_send_intr()
{
	register unsigned char	new_out;
	register long		method;
	register int		x;
	register long		t;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	RED_ON(RED_MSG);
	assert((t = MAXLOOP) != 0);
	while (mcmsg_send_ready()) {
		if (ipcreq_send_head != 0) {
			ipcreq_send_intr();
		} else if ( (new_out = mcmsg_send_store_out) !=
			    mcmsg_send_store_in ) {
			mcmsg_task = mcmsg_send_store[new_out].mcmsg_task;
			if (mcmsg_task != 0) {
				mcmsg_phys = current_task() != mcmsg_task->task;
			}
			method = mcmsg_send_switch(
				mcmsg_send_store[new_out].method,
				mcmsg_send_store[new_out].item,
				mcmsg_send_store[new_out].sequence);
			if (method == 0) {
				mcmsg_send_store_out = new_out + 1;
			} else {
				mcmsg_send_store[new_out].method = method;
			}
#if	MACH_ASSERT
			if (t == 0) {
				mcmsg_trace_debug("loopout", 3,
					mcmsg_send_store[new_out].method,
					mcmsg_send_store[new_out].item,
					mcmsg_send_store[new_out].sequence, 0);
			}
#endif	MACH_ASSERT
		} else {
			mcmsg_unexpected_send_intr++;
			mcmsg_disable_tx_interrupts();
			break;
		}
		if (ipcreq_send_head == 0 &&
		    mcmsg_send_store_out == mcmsg_send_store_in) {
			mcmsg_disable_tx_interrupts();
			break;
		}
		assert(t-- != 0);
	}
	if (mcmsg_flush) {
		flush();
		mcmsg_flush = 0;
	}
	assert(mcmsg_reentry--);
	RED_OFF(RED_MSG);
	splx(x);
}

mcmsg_send(method, item, sequence)
	int	method;
	void	*item;
	unsigned long sequence;
{
	register unsigned char	in;
	register unsigned char	new_in;
	register long		t;

	if (ipcreq_send_head == 0 &&
	    mcmsg_send_store_in == mcmsg_send_store_out) {
		assert((t = MAXLOOP) != 0);

		/* the send queues are empty, send this one */

		while (mcmsg_send_ready()) {
			method = mcmsg_send_switch(method, item, sequence);
			if (method == 0) {
				return;
			}
if (t == 0) mcmsg_trace_debug("loopout", 3, method, item, sequence, 0);
			assert(t-- != 0);
		}
	}

	in = mcmsg_send_store_in;
	new_in = in + 1;
	if (new_in == mcmsg_send_store_out) {

		assert((t = MAXLOOP) != 0);
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

		mcmsg_send_store_out++;
	}
	mcmsg_send_store[in].method = method;
	mcmsg_send_store[in].item = item;
	mcmsg_send_store[in].sequence = sequence;
	mcmsg_send_store[in].mcmsg_task = mcmsg_task;
	mcmsg_send_store_in = new_in;
	mcmsg_enable_tx_interrupts();
	return 0;
}

mcmsg_send_switch(method, item, sequence)
	int	method;
	void	*item;
	unsigned long sequence;
{
	int	x;

	switch (method) {

	case SENDMETH_PNQ:
		return mcmsg_send_pnq(item, sequence);

	case SENDMETH_PNA:
		return mcmsg_send_pna(item, sequence);

	case SENDMETH_PAK:
		return mcmsg_send_pak(item, sequence);

	case SENDMETH_PRM:
		return mcmsg_send_prm(item, sequence);

	case SENDMETH_RKS:
		return mcmsg_send_rks(item, sequence);

	case SENDMETH_RKR:
		return mcmsg_send_rkr(item, sequence);

	case SENDMETH_RK1A:
		return mcmsg_send_rk1a(item, sequence);

	case SENDMETH_RK1:
		return mcmsg_send_rk1(item, sequence);

	case SENDMETH_RKN:
		return mcmsg_send_rkn(item, sequence);

	case SENDMETH_RKA:
		return mcmsg_send_rka(item, sequence);

	case SENDMETH_NXS:
		return mcmsg_send_nxs(item, sequence);

	case SENDMETH_NXM:
		return mcmsg_send_nxm(item, sequence);

	case SENDMETH_NXQ:
		return mcmsg_send_nxq(item, sequence);

	case SENDMETH_NXC:
		return mcmsg_send_nxc(item, sequence);

	case SENDMETH_NX1:
		return mcmsg_send_nx1(item, sequence);

	case SENDMETH_NXF:
		return mcmsg_send_nxf(item, sequence);

	case SENDMETH_NXN:
		return mcmsg_send_nxn(item, sequence);

	case SENDMETH_BOOT:
		return mcmsg_send_boot(item, sequence);

	case SENDMETH_CONW:
		return mcmsg_send_conw(item, sequence);

	case SENDMETH_CONT:
		return mcmsg_send_cont(item, sequence);

	case SENDMETH_CONR:
		return mcmsg_send_conr(item, sequence);

	case SENDMETH_NULL:
		return 0;

	default:
		mcmsg_trace_debug("invalid send method", 3,
				method, item, sequence, 0);
		assert(0);
		return 0;
	}
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

#endif	iPSC860
