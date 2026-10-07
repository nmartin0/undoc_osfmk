/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_reg.c,v 1.2 1994/11/18 20:47:46 mtm Exp $

/*
 * msgp_reg.c
 *
 * REG message module
 */

#define	MCMSG_MODULE	MCMSG_MODULE_REG

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_mp.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

#define	REG_SLOTS	10
typedef
struct reg_slot {
	post_page_t	*page;
	unsigned long	*route;
	unsigned long	in;
	unsigned long	node;
} reg_slot_t;

reg_slot_t	mcmsg_reg_slot[REG_SLOTS];

int	mcmsg_reg_active_slot;

mcmsg_reg_init()
{
	int	i;

	for (i = 0; i < REG_SLOTS; i++) {
		mcmsg_reg_slot[i].page = 0;
	}
}


mcmsg_reg_open(mt, slot, page)
	mcmsg_task_t		*mt;
	unsigned long		slot;
	unsigned long		page;
{
	post_page_t	*pp;
	unsigned long	*rp;
	int		nodecount;
	int		node;
	int		nlsize;

	if (slot >= REG_SLOTS) {
		return -1;
	}

	if (mcmsg_reg_slot[slot].page != 0) {
		return -2;
	}
	
	/*
	 * Convert post page pointer to physical address
	 * Error out if not mapped or not on page boundary
	 */

	if ((pp = (post_page_t *)mcmsg_validate_real(
		page, mt->dirbase, __FILE__, __LINE__)) == 0 ||
	    ((unsigned long)pp & (INTEL_PGBYTES-1)) != 0) {
		return -3;
	}
	mcmsg_reg_slot[slot].page = pp;
	mcmsg_reg_slot[slot].in = 0;

	nodecount = mt->task_ptype_list->numnodes + 1;
	nlsize = l2size(nodecount*sizeof(unsigned long));
	rp = (unsigned long *)mcmsg_l2malloc(nlsize);
	if (rp == 0) {
		return -4;
	}
	for (node = 0; node < nodecount; node++) {
	    *rp = calculate_route(mt->task_ptype_list->phys_node_list[node]);
		if (mt->task_ptype_list->phys_node_list[node] ==
		    ipsc_physnode) {
			mcmsg_reg_slot[slot].node = node << 16;
		}
	}
	mcmsg_reg_slot[slot].route = rp;

	mcmsg_reg_active_slot = slot;
}

mcmsg_reg_send(mt, node, a0, a1, a2, a3, a4, a5, a6, a7)
	mcmsg_task_t	*mt;
	unsigned long	node;
	long		a0;
	long		a1;
	long		a2;
	long		a3;
	long		a4;
	long		a5;
	long		a6;
	long		a7;
{

	mcmsg_send_tail(mt, MCTRL_REG, a0, a1, a2, a3, a4, a5, a6, a7, node);
}

mcmsg_send_reg(mt, ctl, a0, a1, a2, a3, a4, a5, a6, a7, node)
	mcmsg_task_t	*mt;
{
	register reg_slot_t	*rs;
	register		hdr2;

	hdr2 = mcmsg_reg_active_slot;
	rs = &mcmsg_reg_slot[hdr2];
	hdr2 |= rs->node;
	mcmsg_trace_send(MCTRL_REG, hdr2, node, 0, 0, 0);
	send2(rs->route[node], 0);
	send2(MCTRL_REG, hdr2);
	send2(a0, a1);
	send2(a2, a3);
	send2(a4, a5);
	send2eod(a6, a7);
}

mcmsg_recv_reg(hdr1, hdr2)
	register unsigned long	hdr1;
	register unsigned long	hdr2;
{
	register reg_slot_t	*rs;
	register post_page_t	*pp;

	mcmsg_trace_recv(hdr1, hdr2 & 0xFFFF, hdr2 >> 16, 0, 0, 0);
	rs = &mcmsg_reg_slot[hdr2 & 0xFFFF];
	pp = &rs->page[rs->in];
	if (pp != 0) {
		recv2(pp->arg1, pp->arg2);
		recv2(pp->arg3, pp->arg4);
		recv2(pp->arg5, pp->arg6);
		recv2(pp->arg7, pp->arg8);
		pp->arg0 = hdr2 >> 16;
		pp->method = 1;
		rs->in++;
		if (rs->in == POST_PAGE_SLOTS) {
			rs->in = 0;
		}
	} else {
		mcmsg_trace_drop("reg no recv page", rs);
		mcmsg_fifo_flush(32);
	}
}
