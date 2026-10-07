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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_task.h,v 1.15 1994/11/18 20:41:59 mtm Exp $
 */

/*
 * mcmsg_task.h
 *
 * Message passing ptype, node, and task structures
 */

#ifndef MCMSG_TASK_H
#define MCMSG_TASK_H

#define PTYPE_ENTRIES	31

/* XXX yuk, fix */
#define node_list_t long

typedef
struct ptype_list {
	long		method;
	unsigned long	last_entry;
	long		app;
	unsigned long	numnodes;
	unsigned long	*phys_node_list;
	struct ptype_list *node_ptype_list;
	unsigned long	refcount;
	unsigned long	fill7;
	struct ptype_entry {
		long			ptype;
		void			*item;
		long			offset;
	} ptype_entry[PTYPE_ENTRIES];
} ptype_list_t;

typedef
struct mcmsg_task {
	struct mcmsg_task	*link;
	unsigned long		method;
	long			pid;
	struct task		*task;
	pt_entry_t		*dirbase;
	struct ptype_list	*task_ptype_list;
	struct select		*pid_sel;

	unsigned long		node;
	unsigned long		numnodes;

	select_item_t		*selection_path;

	xmsg_t			*xmsg_head;	/* Head of buffer list */
	xmsg_t			*xmsg_tail;	/* Tail of buffer list */
	unsigned long		wired_start;
	unsigned long		wired_end;
	unsigned long		provided;
	unsigned long		assigned;
	unsigned long		assign_target;
	unsigned long		nxport;		/* NX kernel port */
	unsigned long		masktrap;

	struct applinfo		applinfo;

	struct select_item	*avail_need;
	struct select_item	*rk_recv_need;
	struct select_item	*send_wait_unk;
} mcmsg_task_t;

#endif MCMSG_TASK_H
