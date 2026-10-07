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
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: dcm_output.c,v $
 * Revision 1.3  1994/11/18  20:40:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:34:51  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/09/22  18:11:38  regnier
 * Initial revision
 *
 * Revision 2.6.2.5  92/05/27  00:43:03  jeffreyh
 * 	code cleanup
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.6.2.4  92/04/08  15:43:57  jeffreyh
 * 	Use the experimental dcm fifo code
 * 	[92/04/08            andyp]
 * 
 * Revision 2.6.2.3  92/03/28  10:08:41  jeffreyh
 * 	Comment out dcm_enable_tx_interrupts() in dcm_send() and
 * 	dcm_send_some().  Strange things can happen if this is called 
 * 		too early.
 * 	[92/03/27            andyp]
 * 
 * Revision 2.6.2.2  92/02/18  19:02:33  jeffreyh
 * 	[intel] mysterious fixes to dcm code.
 * 	[92/02/13  12:48:40  jeffreyh]
 * 
 * Revision 2.6.2.1  92/01/21  21:50:18  jsb
 * 	Use norma/ipc_netvec.h instead of norma/ipc_net.h.
 * 	[92/01/10  10:23:54  jsb]
 * 
 * Revision 2.6  91/12/10  16:31:48  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:25  jsb]
 * 
 * Revision 2.5  91/09/04  11:28:20  jsb
 * 	Removed debugging printfs and made it work (sort of).
 * 	[91/09/04  09:08:42  jsb]
 * 
 * Revision 2.4  91/08/28  11:12:52  jsb
 * 	Updated for norma/ipc_net.c compatability.
 * 	Contains lots of debugging printfs.
 * 	Also contains dummy definitions for some new dbg routines.
 * 	[91/08/27  17:07:22  jsb]
 * 
 * Revision 2.3  91/06/18  20:52:25  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:01:51  jsb]
 * 
 * 	From Paul Pierce: first implementation of this module.
 * 	[91/01/07  10:49:00  jsb]
 * 
 * Revision 2.2  91/06/17  15:45:39  jsb
 * 	Updated for newer, faster internode IPC implementation.
 * 	[91/06/17  10:41:59  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:05  jsb
 * 	First checkin.
 * 	[90/12/03  21:48:37  jsb]
 * 
 */
/* 
 * dcm.c Don Cameron, Joel Clark December 1989
 *	revised and revisited January, 1992 Andy Pfiffer
 * 
 * i860-specific DCM driver code.
 */ 

#include <norma_ipc.h>
#include <norma/ipc_netvec.h>
#include <kern/assert.h>
#include <ipsc/dcmcom.h>
#include <i860ipsc/nodehw.h>
#include <i860ipsc/ctlreg.h>
#include <i860ipsc/dcmstate.h>

#define DCM_MAX_SEND_VEC	5

struct dcm_header	dcm_send_header;
struct netvec		dcm_send_vec[DCM_MAX_SEND_VEC];
struct dcm_state	dcm_send_state;
int			ipsc_route;
extern int		ipsc_physnode;
extern int		dcm_message_traffic;
int			dcm_tx_trace = 0;


void	dcm_disable_tx_interrupts();
void	dcm_enable_tx_interrupts();


dcm_init_send()
{
	soft_pic_disable(SEND_INT_MASK);
	dcm_disable_tx_interrupts();

	bzero(&dcm_send_vec[0], DCM_MAX_SEND_VEC * sizeof(struct netvec));
	bzero(&dcm_send_state, sizeof(dcm_send_state));
	bzero(&dcm_send_header, sizeof(dcm_send_header));

	ipsc_route = ipsc_physnode & 0x7f;
}


dcm_send(vecp, nvecs)
	struct netvec *vecp;
	int nvecs;
{
	if (dcm_message_traffic++ == 0)
		led_red_on();

	if (dcm_tx_trace)
		printf("dcm tx: r=0x%x, s=%d\n",
			dcm_send_header.route, dcm_send_header.size);
	dcm_send_state.dcm_vecp = vecp;
	dcm_send_state.dcm_nvecs = nvecs;
	dcm_send_state.dcm_state = DCM_READY;
#if 1   /* 1 == optimization:
         * stuff bytes NOW instead of waiting for dcm fifo interrupt.
         */
        dcm_send_some();
#else
        soft_pic_enable(SEND_INT_MASK);
#endif
}


netipc_send(remote, vec, count)
	int	remote;
	register struct netvec *vec;
	int	count;
{
	int	s;

	s = sploff();
	dcm_send_header.route = remote ^ ipsc_route;
	dcm_send_header.type = DCM_HDR_TYPE_MACH;
	dcm_send_vec[0].addr = (unsigned long) &dcm_send_header;
	dcm_send_vec[0].size = (unsigned long) sizeof(dcm_send_header);
	if (count == 2) {
		dcm_send_vec[1] = vec[0];
		dcm_send_vec[2] = vec[1];
		dcm_send_header.size = vec[0].size + vec[1].size;
	} else if (count == 3) {
		dcm_send_vec[1] = vec[0];
		dcm_send_vec[2] = vec[1];
		dcm_send_vec[3] = vec[2];
		dcm_send_header.size = vec[0].size + vec[1].size + vec[2].size;
	} else {
		panic("netipc_send: bad count=%d\n", count);
	}
	dcm_send(&dcm_send_vec[0], count + 1);
	splon(s);
}


dcm_send_intr()
{
	soft_pic_disable(SEND_INT_MASK);
	dcm_disable_tx_interrupts();

	if (dcm_send_state.dcm_state != DCM_IDLE) {
		dcm_send_some();
	}
}


dcm_send_some()
{
	struct netvec	*vec;
	unsigned long	addr, size;

	dcm_spl_check("dcm_send_some");

	if (dcm_send_state.dcm_state == DCM_READY) {
		dcm_send_state.dcm_state = DCM_BUSY;
	}

	vec = dcm_send_state.dcm_vecp;

	assert(vec->addr != 0);
	assert(dcm_send_state.dcm_nvecs > 0);

	while ((STATUS_REG & SEND_HALF) == 0) {

		addr = vec->addr;
		size = vec->size;

		if (size > HALF_MSG_FIFO) {
			/* send a chunk */
			dcm_fifo_out(addr, HALF_MSG_FIFO);
			vec->addr = addr + HALF_MSG_FIFO;
			vec->size = size - HALF_MSG_FIFO;
			continue;
		}

		if (size > 0) {
			/* send the rest of the vec */
			if (dcm_send_header.size > 0) {
				dcm_fifo_out(addr, size);
			} else {
				dcm_fifo_out_eod(addr, size);
			}
			vec->addr = addr + size;
			vec->size = 0;
		} else if (size < 0) {
			panic("dcm_send_some: can't send negative bytes!");
		} else {
			/*
			 * I'm close to convincing myself that it is
			 * okay for size to be == 0...
			 */
		}

		/* this vec has been sent -- move on */
		dcm_send_state.dcm_nvecs--;

		/* finished? */
		if ((dcm_send_state.dcm_nvecs == 0) ||
		    (dcm_send_header.size == 0)) {
			dcm_send_state.dcm_state = DCM_IDLE;
			if (--dcm_message_traffic <= 0) {
				dcm_message_traffic = 0;
				led_red_off();
			}
			if (dcm_tx_trace)
				printf("dcm tx: send complete\n");
			netipc_send_intr();
			return;
		}

		/* move to the next vec */
		vec = ++dcm_send_state.dcm_vecp;
		if (vec->size > dcm_send_header.size) {
			vec->size = dcm_send_header.size;
		}
		dcm_send_header.size -= vec->size;
	}

	soft_pic_enable(SEND_INT_MASK);
/*	dcm_enable_tx_interrupts(); */
}


dcm_fifo_out(addr, n)
	unsigned long addr;
	unsigned long n;
{
	register unsigned long *saddr = (unsigned long *) addr;
	register unsigned long *eaddr = (unsigned long *) (addr + n);

	if (dcm_tx_trace)
		printf("dcm tx: fifo_out(addr=%x, size=%d)\n", addr, n);
	_dcm_fifo_out(addr, FIFO_ADDR, n >> 2);
}


dcm_fifo_out_eod(addr, n)
	unsigned long	addr;
	unsigned long	n;
{
	register unsigned long *saddr = (unsigned long *) addr;
	register unsigned long *eaddr = (unsigned long *) (addr + n - 4);
	volatile unsigned long *fifo  = (unsigned long *) FIFO_ADDR;

	if (dcm_tx_trace)
		printf("dcm tx: fifo_out_eod(addr=%x, size=%d)\n", addr, n);
	while (saddr < eaddr) {
		*fifo = *saddr++;
	}
	SEND_EOD = *saddr;
}


/*
 *	disable the DCM transmit interrupt.
 */
void dcm_disable_tx_interrupts()
{
	int	s;

	s = sploff();
	CLR_CONTROL(SEND_INT_MASK);
	splon(s);
}


/*
 *	enable the DCM transmit interrupt.
 */
void
dcm_enable_tx_interrupts()
{
	int	s;

	s = sploff();
	SET_CONTROL(SEND_INT_MASK);
	splon(s);
}


netipc_network_init()
{
	extern int _node_self;

	_node_self = ipsc_physnode;
}


dcm_send_reset()
{
#if	MACH_KDB
	gimmeabreak();
#endif	MACH_KDB
	panic("dcm_send_reset");
}
