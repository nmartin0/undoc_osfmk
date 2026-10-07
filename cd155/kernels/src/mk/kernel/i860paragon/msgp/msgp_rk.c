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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/msgp/msgp_rk.c,v 1.37 1994/11/18 20:47:47 mtm Exp $
 */

/*
 * mcmsg_rk.c
 *
 * RK messages
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/msgp/msgp.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

mcmsg_xsend(mt, xmsg, node, dest_ptype)
	mcmsg_task_t	*mt;
	xmsg_t		*xmsg;
	unsigned	node;
	int		dest_ptype;
{
	register task_t		task;
	register xmsg_t		*xp;
	register long		i;
	register unsigned long	sequence;
	register unsigned long	msg_size;
	register long		dest_pid;
	register select_item_t	*si;
	register select_item_t	*pid_si;

	xp = (xmsg_t *)mcmsg_validate_line(mt, xmsg);
	if (xp == 0) {
		return -1;
	}
	sequence = mcmsg_send_sequence;
	mcmsg_send_sequence = sequence + 1;

	dest_pid = mcmsg_remote_pid(mt, node, dest_ptype);
	if (dest_pid != -1) {
		pid_si = mcmsg_lookup_remote(mt, dest_pid);
	} else {
		if (node < 0 || node > mt->numnodes) {
			xp->state = XMSG_TRASH;
			return -1;
		}
		pid_si = 0;
	}

	si = mcmsg_alloc_select_item();
	assert(si != 0);
	si->item = (void *)si;
	si->nxrq.request = 0;
	si->value = sequence;
	si->method = 0;
	si->nextmethod = MCTRL_NXN;
	si->mcmsg_task = mt;
	si->nxrq.pid_si = pid_si;
	si->nxrq.dest_node = node;
	si->nxrq.dest_ptype = dest_ptype;
	si->nxrq.source_ptype = xp->source_ptype;
	si->nxrq.msg_type = xp->msg_type;
	/* si->nxrq.buf cannot point to a split xmsg buffer here */
	si->nxrq.buf = (unsigned long)(xmsg + 1);
	si->nxrq.count = xp->length;
	si->nxrq.offset = 0;
	si->nxrq.stop = xp->length;
	si->nxrq.take = 0;
	si->nxrq.sequence = sequence;
	si->nxrq.xmsg = (user_pointer_t)xmsg;
	si->nxrq.vm_ast_pending = 0;

	if (pid_si == 0) {
		mcmsg_inquire(mt, si);
	} else if (pid_si->ppid.send_ready == 0) {
		mcmsg_attach(mt, si);
	} else {
		mcmsg_schedule_send(mt, si);
	}
	return 0;
}

