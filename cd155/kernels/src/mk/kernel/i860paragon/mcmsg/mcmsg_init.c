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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_init.c,v 1.6 1994/11/18 20:44:17 mtm Exp $
 */

/*
 * mcmsg_init.c
 *
 * Initialization for multicomputer message passing
 */

#include <i860paragon/mcmsg/mcmsg_ext.h>
#include <i860paragon/mcmsg/mcmsg_hw.h>
#include <i860paragon/mcmsg/mcmsg_appl.h>

dcm_init_recv()
{

	return;
}

dcm_init_send()
{

	return;
}

/*
 *	Routine:
 *		syscall_mcmsg_nx_op
 *
 *	Purpose:
 *		Generic system call interface.
 *		Operation performed dependent on op (first parameter).
 *
 *		Allows multiple back-door functions without changing
 *		the mach syscall files.
 *
 *	Returns:
 *		Dependent upon op.
 *
 *		Op = 1: kernel config info
 */
syscall_mcmsg_nx_op(op, a, b, c, d, e, f, g, h)
unsigned long op, a, b, c, d, e, f, g, h;
{
	unsigned long	retval;

	switch (op) {
		case 1:
			retval = 0;
			if (mcmsg_mp_enable) {
				retval |= 1;
			}
#if    BUMPERS
			retval |= 2;
#endif BUMPERS
#if    BIGPKTS
			retval |= 4;
#endif BIGPKTS
#if    ASMP
			retval |= 8;
#endif ASMP
			break;
		default:
			retval = -1;
	}
	return retval;
}


syscall_mcmsg_nodeinfo(nodeinfo)
	nodeinfo_t	*nodeinfo;
{
	task_t		task;
	unsigned long	*np;

	/*
	 * Attach to Mach task
	 */

	task = current_task();
	if (task->mcmsg_task == 0) {
		return -1;
	}

	if (task->mcmsg_task->applinfo.app == -1) {
		return -1;
	}

	nodeinfo->mynode = task->mcmsg_task->node;
	nodeinfo->numnodes = task->mcmsg_task->numnodes;
	nodeinfo->applinfo = task->mcmsg_task->applinfo;

	return 0;
}

syscall_mcmsg_wire(address, size)
	unsigned long	address;
	unsigned long	size;
{
	task_t		task;
	mcmsg_task_t	*mt;
	int		i;
	int		npages,pavail;

	/*
	 *	Make sure there is enough memory to wire without paging
	 *	out the server.
	 */
	npages = size/PAGE_SIZE;	/* convert to signed */
	pavail = vm_page_free_count + 
		 vm_page_inactive_count + 
		 vm_page_active_count;
	if ((pavail - npages) < 275) {
		return -2;
	}

	task = current_task();
	mt = task->mcmsg_task;
	if (mt == 0) {
		return -1;
	}

	i = mcmsg_wire_buffer(mt->task, address, address + size - 1);

	if (mt->wired_start == mt->wired_end) {
		mt->wired_start = address;
		mt->wired_end = address + size - 1;
	} else if (mt->wired_start > address) {
		mt->wired_start = address;
	} else if (mt->wired_end < address + size - 1) {
		mt->wired_end = address + size - 1;
	}

	return i;
}
