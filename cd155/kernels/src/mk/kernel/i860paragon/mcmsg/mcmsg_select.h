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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/mcmsg/mcmsg_select.h,v 1.26 1995/01/30 21:42:15 joel Exp $
 */

/*
 * mcmsg_select.h
 *
 * Declarations for selectors
 */

#ifndef MCMSG_SELECT_H
#define MCMSG_SELECT_H

/*
 * Method constants for all data structures
 *
 * These share the same number space with packet types, so data structures
 * can be labelled with packet types if needed.
 */

#define SELMETH_APP		(MCTRL_END + 1)
#define SELMETH_PID_TASK	(MCTRL_END + 2)
#define SELMETH_PID_SEL		(MCTRL_END + 3)
#define SELMETH_PID		(MCTRL_END + 4)
#define SELMETH_PTYPE		(MCTRL_END + 5)
#define SELMETH_TASK		(MCTRL_END + 6)
#define SELMETH_SEQ		(MCTRL_END + 7)
#define SELMETH_XMSG		(MCTRL_END + 8)
#define SELMETH_RECV_TYPESEL	(MCTRL_END + 9)
#define SELMETH_RECV_NODESEL	(MCTRL_END + 10)
#define SELMETH_RECV_ANY	(MCTRL_END + 11)
#define SELMETH_RECV_SRC	(MCTRL_END + 12)
#define SELMETH_RECV_TYPESET	(MCTRL_END + 13)
#define SELMETH_RECV_TYPE	(MCTRL_END + 14)
#define SELMETH_RECV_TYPESRC	(MCTRL_END + 15)
#define SELMETH_RECV_XMSG	(MCTRL_END + 16)
#define SELMETH_TASK_PTYPE	(MCTRL_END + 17)
#define SELMETH_NODE_PTYPE	(MCTRL_END + 18)
/* Remember to update mcmsg_trace */

/*
 * Selector hash function
 */

#define SELECT_HASH_LEN	64
#define SELECT_HASH_FUN(x) ((x) % SELECT_HASH_LEN)

/*
 * Selector structure
 *
 * A selector is used to organize linked data structures by key values.
 * During selection, the system can find a data structure by looking it
 * up in the selector using the appropriate key value. There is a special
 * case for zero, which is expected to occur often, and for the case where
 * no data structure is linked under the specified key value.
 *
 * The first two fields are a link to the next element of a selection path in
 * case a key value search fails, and a method integer to identify the structure
 * as a selector. These must be kept in the same positions so that selectors
 * can be mixed with other data structures in a selection path.
 *
 * Selectors contain a hash table where each entry points to a linked list
 * of select items which represent the data structures whose key values fall
 * into the same bucket. There is a separate entry for the key value zero.
 *
 * The linked lists are circular with the hash table entry pointing to the
 * tail of the list, so that both insertion and deletion are relatively fast
 * while using only one pointer in the hash table.
 *
 * Select items are data structures which contain as a minimum a link, method,
 * key value, and item pointer that points to the real data structure. When
 * possible, as for NX requests, the select item is the data structure itself
 * which contains the link method and value as its first fields. Again, the
 * link and method must be kept in order so that select items can be mixed
 * with other data structures on a linked list.
 *
 * An active selector might look like this:
 *
 *   --------------------
 * ->|     fail         -------------------> next element in selection path
 *   --------------------
 *   | SELMETH_XXX_SEL  |         select item
 *   --------------------         -------------
 *   |     zero         --------->|(null link)|
 *   --------------------         -------------
 *   |     hash         |         | method    |
 *   |     table        |         -------------
 *   |------------------|         | value=0   |
 *   |                  |         ------------- 
 *   |------------------|         | item      ---> data structure
 *   |                  |         -------------
 *   |------------------|
 *   |                  --------------------------+
 *   |------------------|                         |
 *   |                  |    head select item     |   tail select item
 *   |------------------|         -----------     +-->-----------
 *   |                  |     +-->| link    ----...-->| link    -----+
 *   |------------------|     |   -----------         -----------    |
 *   |                  |     |   | method  |         | method  |    |
 *   |------------------|     |   -----------         -----------    |
 *   |                  |     |   | value   |         | value   |    |
 *   |------------------|     |   -----------         -----------    |
 *   |                  |     |   | item    |         | item    |    |
 *   |------------------|     |   -----------         -----------    |
 *   |                  |     +--------------------------------------+
 *   --------------------
 */

typedef
struct select {
	void			*fail;		/* Link to next on path */
	unsigned long		method;		/* Method, type of selector */
	struct select_item	*zero;		/* Link to zero item */
	struct select_item	*hash[SELECT_HASH_LEN]; /* Hash table */
} select_t;

/*
 * Select item structure
 *
 * Includes a variant for NX requests and a variant to represent processes
 */

#define RETIRE_MAX		6

typedef
struct select_item {
	struct select_item	*link;		/* Link to next item */
	unsigned short		method;		/* Method, type of item */
	unsigned short		nextmethod;	/* Next method (obsolete?) */
	unsigned long		value;		/* Key value */
	void			*item;		/* Pointer to data structure */
	struct mcmsg_task	*mcmsg_task;	/* Associated process */
	union {
	 struct {	/* NX request */
	 	user_pointer_t		request;	/* -> nxreq */
	 	user_pointer_t		xmsg;		/* -> xmsg */
	 	struct select_item	*pid_si;	/* other process */
	 	struct select_item	*seq_link;     /* see lookup_sequence */
		unsigned long		originating_node; /* for global sends */
	 	long			dest_ptype;	/* destination ptype */
	 	long			source_ptype;	/* source ptype */
	 	unsigned short		sequence;	/* msg sequence nbr */
	 	short			dest_node;	/* destination node */
	 	unsigned long		buf;		/* usr buffer address */
	 	unsigned long		count;		/* byte count */
	 	unsigned long		take;		/* sys buf bytes */
	 	long			msg_type;	/* type */
	 	unsigned long		stop;		/* bytes ok to send */
	 	unsigned long		offset;	      /* bytes from beginning */
	 	short			vm_ast_pending;	/* boolean taking ast */
	 } nx_request;
	 struct {	/* Per process */
	 	unsigned long		node;		/* node number */
	 	int			process_lock:	1; /* -plk flag */
	 	int			send_ready:	1; /* inquiry state */
	 	int			sent_pak:	1; /* inquiry state */
	 	int			recvd_pak:	1; /* inquiry state */
#if BIGPKTS
	 	int			sent_nat:	1; /* inquiry state */
#endif BIGPKTS
		int			in_rel_send_wait: 1;/* boolean */
	 	long			route;		/* route to node */
	 	unsigned long		send_avail;	/* bytes can send */
	 	unsigned long		recv_give;	/* bytes to give */
	 	unsigned long		recv_total;	/* bytes ready here */
	 	unsigned long		recv_target;	/* bytes ready target */
	 	unsigned long		recv_taken;	/* bytes in msgs */
	 	struct select_item	*send_wait;	/* list of sends */
	 	unsigned long		rk_recv_want;	/* used for NXS */
	 	unsigned long		rk_recv_seq;	/* used for NXS */
	 	long			rk_recv_pid;	/* used for NXS */
	 	long			rk_recv_type;	/* used for NXS */
	 	long			rk_recv_ptype;	/* used for NXS */
		unsigned long		rk_recv_originating_node;
	 	struct select_item	*rk_recv_link;	/* used for NXS */
	 	struct select_item	*avail_link;	/* list of procs */
                struct select_item      *attach_link;   /* next attached pid */
                struct select_item      *att_pend_link; /* next waiting pid */
                struct select_item      *sub_pid_si;    /* replacement pid */
                unsigned long           when_recvd;     /* relative timestamp */
	 } per_pid;
	} contents;
} select_item_t;

/*
 * Convenient macros to access NX and process variants
 */

#define nxrq	contents.nx_request
#define ppid	contents.per_pid

select_item_t *mcmsg_selector_lookup_si();
select_item_t *mcmsg_selector_detach();
select_item_t *mcmsg_select_recvtype_detach();
select_item_t *mcmsg_alloc_select_item();
select_item_t *mcmsg_lookup_remote();

#endif MCMSG_SELECT_H
