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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860ipsc/mcmsg/mcmsg_select.h,v 1.14 1994/11/18 20:41:57 mtm Exp $
 */

/*
 * mcmsg_select.h
 *
 * External declarations
 */

#ifndef MCMSG_SELECT_H
#define MCMSG_SELECT_H

#define SEL_MAX		1000
#define SEL_CHUNK	10

#define SELECT_HASH_LEN	64

#if	PARAGON860

#define SELMETH_APP		(SENDMETH_END + 1)
#define SELMETH_PID_TASK	(SENDMETH_END + 2)
#define SELMETH_PID_SEL		(SENDMETH_END + 3)
#define SELMETH_PID		(SENDMETH_END + 4)
#define SELMETH_PTYPE		(SENDMETH_END + 5)
#define SELMETH_TASK		(SENDMETH_END + 6)
#define SELMETH_SEQ		(SENDMETH_END + 7)
#define SELMETH_XMSG		(SENDMETH_END + 8)
#define SELMETH_RECV_TYPESEL	(SENDMETH_END + 9)
#define SELMETH_RECV_NODESEL	(SENDMETH_END + 10)
#define SELMETH_RECV_ANY	(SENDMETH_END + 11)
#define SELMETH_RECV_SRC	(SENDMETH_END + 12)
#define SELMETH_RECV_TYPESET	(SENDMETH_END + 13)
#define SELMETH_RECV_TYPE	(SENDMETH_END + 14)
#define SELMETH_RECV_TYPESRC	(SENDMETH_END + 15)
#define SELMETH_RECV_XMSG	(SENDMETH_END + 16)
#define SELMETH_TASK_PTYPE	(SENDMETH_END + 17)
#define SELMETH_NODE_PTYPE	(SENDMETH_END + 18)
/* Remember to update mcmsg_trace */

#else	PARAGON860

#define SELMETH_APP		1
#define SELMETH_PID_TASK	2
#define SELMETH_PID_SEL		3
#define SELMETH_PID		4
#define SELMETH_PTYPE		5
#define SELMETH_TASK		6
#define SELMETH_SEQ		7
#define SELMETH_XMSG		8
#define SELMETH_RECV_TYPESEL	9
#define SELMETH_RECV_NODESEL	10
#define SELMETH_RECV_ANY	11
#define SELMETH_RECV_SRC	12
#define SELMETH_RECV_TYPESET	13
#define SELMETH_RECV_TYPE	14
#define SELMETH_RECV_TYPESRC	15
#define SELMETH_RECV_XMSG	16
#define SELMETH_TASK_PTYPE	17
#define SELMETH_NODE_PTYPE	18
/* Remember to update mcmsg_trace */

#endif	PARAGON860
typedef
struct select {
	void			*fail;
	unsigned long		method;
	void			*zero;
	void			*hash[SELECT_HASH_LEN];
} select_t;

#define RETIRE_MAX		6

typedef
struct select_item {
	struct select_item	*link;
	unsigned short		method;
	unsigned short		nextmethod;
	unsigned long		value;
	void			*item;
	struct mcmsg_task	*mcmsg_task;
	union {
		struct {
			void			*request;
			xmsg_t			*xmsg;
			struct select_item	*pid_si;
			struct select_item	*seq_link;
			long			dest_ptype;
			long			source_ptype;
			unsigned short		sequence;
			short			dest_node;
			unsigned long		buf;
			unsigned long		count;
			unsigned long		take;
			long			msg_type;
			unsigned long		stop;
			unsigned long		offset;
			short			vm_ast_pending;
		} nx_request;
		struct {
			unsigned long		node;
			unsigned long		process_lock:	1;
			unsigned long		send_ready:	1;
			unsigned long		sent_pak:	1;
			long			route;
			unsigned short		send_avail;
			unsigned short		recv_give;
			unsigned long		recv_total;
			unsigned short		recv_target;
			unsigned short		recv_taken;
			struct select_item	*send_wait;
			unsigned long		rk_recv_want;
			unsigned long		rk_recv_seq;
			long			rk_recv_pid;
			long			rk_recv_type;
			long			rk_recv_ptype;
			struct select_item	*rk_recv_link;
			struct select_item	*avail_link;
			unsigned short		retire_seq_in;
			unsigned short		retire_seq_out;
			unsigned short		retire_seq[RETIRE_MAX];
		} per_pid;
	} contents;
} select_item_t;

#define nxrq	contents.nx_request
#define ppid	contents.per_pid

#define SELECT_HASH_FUN(x) ((x) % SELECT_HASH_LEN)

select_item_t *mcmsg_selector_lookup_si();
select_item_t *mcmsg_selector_detach();

#define mcmsg_alloc_select_item() \
	((select_item_t *)mcmsg_l2malloc(l2size(sizeof(select_item_t))))

#define mcmsg_free_select_item(x) \
	((select_item_t *)mcmsg_l2free((x), l2size(sizeof(select_item_t))))

#endif MCMSG_SELECT_H
