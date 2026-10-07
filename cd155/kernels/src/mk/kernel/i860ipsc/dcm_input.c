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
 * $Log: dcm_input.c,v $
 * Revision 1.4  1994/11/18  20:40:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:49  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/12/11  00:32:23  andyp
 * AFS munched on the RCS file...here is the restored version.
 * (checking out version 1.1 gets you a file that won't compile)
 *
 * Revision 1.1  1992/09/22  18:11:27  regnier
 * Initial revision
 *
 * Revision 2.7.2.5  92/05/27  00:42:57  jeffreyh
 * 	code cleanup.
 * 	[regnier@ssd.intel.com]
 * 
 * Revision 2.7.2.4  92/04/08  15:43:54  jeffreyh
 * 	Use the experimental dcm fifo code
 * 	[92/04/08            andyp]
 * 
 * Revision 2.7.2.3  92/03/28  10:08:36  jeffreyh
 * 	Comment out dcm_enable_rx_interrupts() in dcm_recv() and dcm_recv_some().
 * 	[92/03/27            andyp]
 * 	Change flush_on_dcm_recv_some from 1 to 0. We think that change
 * 	to pmap makes flush unnecessary.
 * 	[92/03/24            jeffreyh]
 * 
 * Revision 2.7.2.2  92/02/18  19:02:09  jeffreyh
 * 	[intel] mysterious dcm fixes.
 * 	[92/02/13  12:51:06  jeffreyh]
 * 
 * Revision 2.7.2.1  92/01/21  21:50:13  jsb
 * 	Use norma/ipc_netvec.h instead of norma/ipc_net.h.
 * 	[92/01/10  10:24:15  jsb]
 * 
 * Revision 2.7  91/12/14  12:03:33  jsb
 * 	Fix from andyp at Intel: flush cache after receiving page.
 * 
 * Revision 2.6  91/12/10  16:31:45  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:22  jsb]
 * 
 * Revision 2.5  91/09/04  11:28:17  jsb
 * 	Removed debugging printfs and made it work (sort of).
 * 	[91/09/04  09:08:06  jsb]
 * 
 * Revision 2.4  91/08/28  11:12:50  jsb
 * 	Updated for norma/ipc_net.c compatability.
 * 	Contains lots of debugging printfs.
 * 	[91/08/27  17:04:22  jsb]
 * 
 * Revision 2.3  91/06/18  20:52:23  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  19:01:38  jsb]
 * 
 * 	From Paul Pierce: first implementation of this module.
 * 	[91/01/07  10:49:00  jsb]
 * 
 * Revision 2.2  91/06/17  15:45:35  jsb
 * 	Removed bogus bcopy definition.
 * 	[91/06/17  15:28:56  jsb]
 * 
 * 	Updated for newer, faster internode IPC implementation.
 * 	[91/06/17  10:41:40  jsb]
 * 
 * Revision 2.2  90/12/04  14:50:05  jsb
 * 	First checkin.
 * 	[90/12/03  21:48:37  jsb]
 * 
 */
/* 
 * dcm.c Don Cameron, Joel Clark December 1989
 * 	 revised and revisited January, 1992	Andy Pfiffer
 * 
 * i860-specific DCM driver code.
 */ 

#include <mach/vm_param.h>
#include <norma/ipc_netvec.h>
#include <kern/assert.h>
#include <ipc/ipc_kmsg.h>
#include <ipsc/dcmcom.h>
#include <i860ipsc/nodehw.h>
#include <i860ipsc/ctlreg.h>
#include <i860ipsc/dcmstate.h>

#define	DCM_MAX_RECV_VEC	5

struct dcm_header	dcm_recv_header;
struct netvec		dcm_recv_vec[DCM_MAX_RECV_VEC];
struct dcm_state	dcm_recv_state;
#if	NOCACHE
int	 flush_on_dcm_recv_some = 0;
#else	NOCACHE
int	 flush_on_dcm_recv_some = 0;
#endif	NOCACHE
int	dcm_message_traffic = 0;
int	dcm_rx_trace = 0;


void	dcm_disable_rx_interrupts();
void	dcm_enable_rx_interrupts();


dcm_spl_check(str)
char	*str;
{
	int	s, im, spl;

	s = sploff();
	spl = get_spl();
	if ((spl == 0) && (s & 0x10)) {
		printf("dcm_spl_check: called from %s, spl == %d, s=%d\n", str, spl, s);
	}
	splon(s);
}


static int dcm_recv_check_header(dcm)
	struct dcm_header *dcm;
{
	if (dcm->type != DCM_HDR_TYPE_MACH) {
		printf("%s: bad type: type=0x%x,route=0x%x,size=%d\n",
			"dcm_recv_check_header",
			dcm->type, dcm->route, dcm->size);
		return 1;
	}

	if (dcm_rx_trace)
		printf("dcm rx: r=0x%x,s=%d\n", dcm->route, dcm->size);

	return 0;
}


dcm_init_recv()
{
	soft_pic_disable(RECV_INT_MASK | EOD_IN_INT_MASK);
	dcm_disable_rx_interrupts();

	bzero(&dcm_recv_vec[0], DCM_MAX_RECV_VEC * sizeof(struct netvec));
	bzero(&dcm_recv_state, sizeof(dcm_recv_state));

	dcm_recv_vec[0].addr = (unsigned long) &dcm_recv_header;
	dcm_recv_vec[0].size = (unsigned long) sizeof(dcm_recv_header);

	dcm_recv_vec[3].addr = 0;
	dcm_recv_vec[3].size = 0;
	dcm_recv_vec[4].addr = 0;
	dcm_recv_vec[4].size = 0;
}


/*
 * store some state information and enable interrupts.
 */
dcm_recv(vecp, nvecs)
	struct netvec *vecp;
	int nvecs;
{
	dcm_spl_check("dcm_recv");
	dcm_recv_state.dcm_vecp = vecp;
	dcm_recv_state.dcm_nvecs = nvecs;
	dcm_recv_state.dcm_state = DCM_READY;
	soft_pic_enable(RECV_INT_MASK | EOD_IN_INT_MASK);
}


netipc_recv(vec, count)
	register struct netvec *vec;
	int count;
{
	int	s;

	s = sploff();
	if (count != 2) {
		panic("netipc_recv");
	}

	dcm_recv_vec[0].addr = (unsigned long) &dcm_recv_header;
	dcm_recv_vec[0].size = (unsigned long) sizeof(dcm_recv_header);
	dcm_recv_vec[1] = vec[0];
	dcm_recv_vec[2] = vec[1];
	dcm_recv(&dcm_recv_vec[0], count + 1);
	if (dcm_rx_trace)
		printf("dcm rx? v=0x%x,c=%d,s=%d\n", vec, count,
			dcm_recv_header.size);

	splon(s);
}


dcm_recv_intr()
{
	soft_pic_disable(RECV_INT_MASK | EOD_IN_INT_MASK);
	dcm_disable_rx_interrupts();

	assert(dcm_recv_state.dcm_state != DCM_IDLE);

	if (dcm_recv_state.dcm_state == DCM_READY) {
		dcm_recv_state.dcm_state = DCM_BUSY;
	}

	assert(dcm_recv_state.dcm_vecp != (struct netvec *) 0);
	assert(dcm_recv_state.dcm_nvecs > 0);

	dcm_recv_some();
}


dcm_recv_some()
{
	struct netvec	*vec;
	unsigned long	addr, size;

	dcm_spl_check("dcm_recv_some");

	vec = dcm_recv_state.dcm_vecp;

	while ((STATUS_REG & (RECV_HALF|EOD_IN)) != 0) {

		addr = vec->addr;
		size = vec->size;

		/* receive as much as we can */
		if (size > HALF_MSG_FIFO) {
			dcm_fifo_in(addr, HALF_MSG_FIFO);
			vec->addr = addr + HALF_MSG_FIFO;
			vec->size = size - HALF_MSG_FIFO;
			continue;
		}

		/* can receive the rest of the vec */
		if (size > 0) {
			dcm_fifo_in(addr, size);
			vec->addr = addr + size;
			vec->size = 0;
		}

		/* if this is the first vec, check the header */
		if (vec == &dcm_recv_vec[0]) {
			if (dcm_recv_check_header(&dcm_recv_header)) {
				dcm_recv_reset();
				continue;
			}
			if (dcm_message_traffic++ == 0)
				led_red_on();
		}

		/* move to the next vector if not yet done */
		dcm_recv_state.dcm_nvecs--;

		if ((dcm_recv_state.dcm_nvecs == 0) ||
		    (dcm_recv_header.size == 0)) {
			dcm_recv_state.dcm_state = DCM_IDLE;
			if (dcm_rx_trace) printf("dcm rx: recv complete\n");
			if (--dcm_message_traffic <= 0) {
				dcm_message_traffic = 0;
				led_red_off();
			}
			if (flush_on_dcm_recv_some) flush();
			netipc_recv_intr();
			return;
		}

		vec = ++dcm_recv_state.dcm_vecp;
		if (vec->size > dcm_recv_header.size) {
			vec->size = dcm_recv_header.size;
		}
		dcm_recv_header.size -= vec->size;
	}

	soft_pic_enable(RECV_INT_MASK | EOD_IN_INT_MASK);
}


dcm_fifo_in(addr, n)
	unsigned long	*addr;
	unsigned long	n;
{
	if (dcm_rx_trace)
		printf("dcm rx: fifo_in(addr=%x, size=%d)\n", addr, n);
	_dcm_fifo_in(addr, FIFO_ADDR, n >> 2);
}


/*
 *	disable the DCM receive interrupts.
 */
void
dcm_disable_rx_interrupts()
{
	int	s;

	s = sploff();
	CLR_CONTROL(RECV_INT_MASK | EOD_IN_INT_MASK);
	splon(s);
}


/*
 *	enable the DCM receive interrupts.
 */
void
dcm_enable_rx_interrupts()
{
	int	s;

	s = sploff();
	SET_CONTROL(RECV_INT_MASK | EOD_IN_INT_MASK);
	splon(s);
}


dcm_recv_reset()
{
#if	MACH_KDB
	gimmeabreak();
#endif	MACH_KDB
	panic("dcm_recv_reset");
}

