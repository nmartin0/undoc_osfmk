/*
 * 
 * $Copyright
 * Copyright 1993 , 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_trace.c,v 1.9 1994/11/18 20:48:01 mtm Exp $
 */

/*
 * mcmsg_trace.c
 *
 * Tracing support for debugging
 */

#define	MCMSG_TRACE_MODULE	1

#include <ddb/db_command.h>
#include <i860paragon/mcmsg/mcmsg_ext.h>
#if	NX
#include <i860paragon/mcmsg/mcmsg_nx.h>
#endif	NX
#include <i860paragon/mcmsg/mcmsg_hw.h>


mcmsg_hwclock()
{
asm("counter_reg	=	0xFFFF4630"); /* Works on RX or GP node */
asm("		orh	ha%counter_reg, r0, r31	// Set up register address");
asm("		fld.d	l%counter_reg(r31), f16	// Read counter into f16-17");
asm("		fst.l	f16, 0(r16)		// Store at pointer as two");
asm("		fst.l	f17, 4(r16)		//  longs to avoid traps");
}

#if MSG_TRACE
mcmsg_trace_gen(module, id, hdr1, hdr2, hdr3, extra, extra1, extra2)
	int		id;
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{
	register	p1;
	register	p2;

	if ((mcmsg_trace_module_mask & (1 << module)) == 0) {
		return;
	}
#if	NODUPS
	p1 = mcmsg_trace_index;
	if (p1 == 0) {
		p1 = NTRACE-1;
	} else {
		p1 -= 1;
	}
	if (mcmsg_trace_buf[p1].t_id == id &&
	    mcmsg_trace_buf[p1].t_hdr1 == hdr1 &&
	    mcmsg_trace_buf[p1].t_hdr2 == hdr2 &&
	    mcmsg_trace_buf[p1].t_hdr3 == hdr3) {
		p2 = p1;
		if (p2 == 0) {
			p2 = NTRACE-1;
		} else {
			p2 -= 1;
		}
		if (mcmsg_trace_buf[p2].t_id == id &&
		    mcmsg_trace_buf[p2].t_hdr1 == hdr1 &&
		    mcmsg_trace_buf[p2].t_hdr2 == hdr2 &&
		    mcmsg_trace_buf[p2].t_hdr3 == hdr3) {
			mcmsg_hwclock(mcmsg_trace_buf[p1].hwtime);
			mcmsg_trace_buf[p1].stack = msgp_stack_info();
			mcmsg_trace_buf[p1].extra = extra;
			mcmsg_trace_buf[p1].extra1 = extra1;
			mcmsg_trace_buf[p1].extra2 = extra2;
			return;
		}
	}
#endif	NODUPS
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = id;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = module;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = hdr1;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = hdr2;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = hdr3;
	mcmsg_trace_buf[mcmsg_trace_index].extra = extra;
	mcmsg_trace_buf[mcmsg_trace_index].extra1 = extra1;
	mcmsg_trace_buf[mcmsg_trace_index].extra2 = extra2;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
	msgp_stack_check();
}

mcmsg_trace_send_(module, hdr1, hdr2, hdr3, extra, extra1, extra2)
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{

	mcmsg_trace_gen(module, TRACE_SEND, hdr1, hdr2, hdr3, extra, extra1, extra2);
}

mcmsg_trace_recv_(module, hdr1, hdr2, hdr3, extra, extra1, extra2)
	unsigned long	hdr1;
	unsigned long	hdr2;
	unsigned long	hdr3;
	int		extra;
	unsigned long	extra1;
	unsigned long	extra2;
{

	mcmsg_trace_gen(module, TRACE_RECV, hdr1, hdr2, hdr3, extra, extra1, extra2);
}

#if	NX

mcmsg_trace_provide_(module, xmsg, xp)
	xmsg_t		*xmsg;
	xmsg_t		*xp;
{

	if ((mcmsg_trace_module_mask & (1 << module)) == 0) {
		return;
	}
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_PROVIDE;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = module;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)xmsg;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = (unsigned long)xp;
	if (xp != 0) {
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = xp->size;
	} else {
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = 0;
	}
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
}
#endif	NX

mcmsg_trace_drop_(module, s, v)
	char		*s;
	unsigned long	v;
{

	if ((mcmsg_trace_module_mask & (1 << module)) == 0) {
		return;
	}
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_DROP;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = module;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = (unsigned long)v;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = 0;
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_debug_(module, s, n, v1, v2, v3, v4)
	char		*s;
	unsigned long	n;
	unsigned long	v1;
	unsigned long	v2;
	unsigned long	v3;
	unsigned long	v4;
{

	if ((mcmsg_trace_module_mask & (1 << module)) == 0) {
		return;
	}
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_DEBUG;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = module;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].extra = n;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = v1;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = v2;
	mcmsg_trace_buf[mcmsg_trace_index].extra1 = v3;
	mcmsg_trace_buf[mcmsg_trace_index].extra2 = v4;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_time_(module, s)
	char		*s;
{

	if ((mcmsg_trace_module_mask & (1 << module)) == 0) {
		return;
	}
	mcmsg_hwclock(mcmsg_trace_buf[mcmsg_trace_index].hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_TIME;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = module;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = (unsigned long)s;
	mcmsg_trace_buf[mcmsg_trace_index].extra = 0;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
}

mcmsg_trace_call(method, a1, a2, a3, a4, a5, a6, a7, a8, a9, a10)
{
	extern int post_args_list[];
	extern int post_module_list[];
	unsigned long hwtime[2];

	if ((mcmsg_trace_module_mask & (1 << post_module_list[method])) == 0) {
		return;
	}
	mcmsg_hwclock(hwtime);
	mcmsg_trace_buf[mcmsg_trace_index].hwtime[0] = hwtime[0];
	mcmsg_trace_buf[mcmsg_trace_index].hwtime[1] = hwtime[1];
	mcmsg_trace_buf[mcmsg_trace_index].stack = msgp_stack_info();
	mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_CALL;
	mcmsg_trace_buf[mcmsg_trace_index].t_module = post_module_list[method];
	mcmsg_trace_buf[mcmsg_trace_index].extra = method;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = a1;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = a2;
	mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = a3;
	mcmsg_trace_buf[mcmsg_trace_index].extra1 = a4;
	mcmsg_trace_buf[mcmsg_trace_index].extra2 = a5;
	mcmsg_trace_index++;
	mcmsg_trace_count++;
	if (mcmsg_trace_index >= NTRACE) {
		mcmsg_trace_index = 0;
	}
	if (post_args_list[method] > 5) {
		mcmsg_trace_buf[mcmsg_trace_index].hwtime[0] = hwtime[0];
		mcmsg_trace_buf[mcmsg_trace_index].hwtime[1] = hwtime[1];
		mcmsg_trace_buf[mcmsg_trace_index].t_id = TRACE_CALLX;
		mcmsg_trace_buf[mcmsg_trace_index].t_module =
						post_module_list[method];
		mcmsg_trace_buf[mcmsg_trace_index].extra = method;
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr1 = a6;
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr2 = a7;
		mcmsg_trace_buf[mcmsg_trace_index].t_hdr3 = a8;
		mcmsg_trace_buf[mcmsg_trace_index].extra1 = a9;
		mcmsg_trace_buf[mcmsg_trace_index].extra2 = a10;
		mcmsg_trace_index++;
		mcmsg_trace_count++;
		if (mcmsg_trace_index >= NTRACE) {
			mcmsg_trace_index = 0;
		}
	}
	msgp_stack_check();
}

#endif	MSG_TRACE
