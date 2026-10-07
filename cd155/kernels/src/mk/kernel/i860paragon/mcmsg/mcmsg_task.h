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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_task.h,v 1.25 1994/11/18 20:44:45 mtm Exp $
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

/*
 * The ptype list is used to look up a process by its ptype.
 */

typedef
struct ptype_list {
	long		method;
	unsigned long	last_entry;
	long		app;
	unsigned long	numnodes;
	unsigned long	*phys_node_list;
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
	struct ptype_list 	*node_ptype_list;
	struct select		*pid_sel;

	unsigned long		node;
	unsigned long		numnodes;

	select_item_t		*selection_path;

	user_pointer_t		xmsg_rdy;	/* Head of message list */
	user_pointer_t		xmsg_rdy_end;	/* Last element in buffer list
						   prior to start of EMPTY
						   buffers */
	user_pointer_t		xmsg_head;	/* Head of buffer list */
	user_pointer_t		xmsg_tail;	/* Tail of buffer list */
	unsigned long		wired_start;
	unsigned long		wired_end;
	unsigned long		provided;
	unsigned long		assigned;
	unsigned long		reserved;	/* buffers reserved for noc */
	unsigned long		nxport[2];	/* NX kernel ports */
	unsigned long		masktrap;
	unsigned long		nxport_req_in; /* Incoming AST req placed here */
	unsigned long		nxport_req_out; /* Reqs sent to nxports from here */
	mcmsg_nxport_req_t	*nxport_req_list; /* Buffer for AST reqs for task */

	struct applinfo		applinfo;

	struct select_item	*avail_need;
	struct select_item	*rk_recv_need;
	struct select_item	*send_wait_unk;
        select_item_t           *attached;         /* attach pids not > noc */
        select_item_t           *attach_wait;      /* pids waiting to send */
 	select_item_t           *attach_pend;  /* pids waiting to be attached */

#if	MCMSG_MODULE_PUMA

	unsigned long		puma_hdrp;
	unsigned long		puma_nhdr;
	unsigned long		puma_hdri;
	unsigned long		puma_bufp;
	unsigned long		puma_offset;
	unsigned long		puma_bufsize;

#endif	MCMSG_MODULE_PUMA

#if	MCMSG_MODULE_VC
	unsigned long		vc_base;
	unsigned long		vc_limit;
	unsigned long		vc_low_free;
	unsigned long		vc_stbuf;
	unsigned long		vc_tagged;
#endif	MCMSG_MODULE_VC
} mcmsg_task_t;

#endif MCMSG_TASK_H
