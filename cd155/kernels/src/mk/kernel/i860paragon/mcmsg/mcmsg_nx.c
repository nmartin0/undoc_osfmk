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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_nx.c,v 1.7 1994/11/18 20:44:27 mtm Exp $

/*
 * mcmsg_nx.c
 *
 * NX messages
 */

#define	MCMSG_MODULE	MCMSG_MODULE_NX

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_nx.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>

mcmsg_nx_init()
{

	mcmsg_memory_init();
	mcmsg_select_init();
	mcmsg_selector_init(&mcmsg_app_sel, SELMETH_APP);
	mcmsg_selector_init(&mcmsg_local_sel, SELMETH_PID_TASK);
	mcmsg_selector_init(&mcmsg_seq_sel, SELMETH_PID_TASK);
}

/*
 * schedule the async send or recieve ast
 */

syscall_mcmsg_nx_local_ast(request)
	register nxreq_t	*request;
{
	register task_t		task;
	register int		x;

	x = spldcm();
	assert(mcmsg_reentry++ == 0);
	CHECK_REENTRY("entry ast");
	RED_ON(RED_MSG);

	task = current_task();
	mcmsg_hreq_ast(task, request);

	CHECK_REENTRY("exit  ast");
	assert(--mcmsg_reentry == 0);
	RED_OFF(RED_MSG);
	splx(x);
	return 0;
}
