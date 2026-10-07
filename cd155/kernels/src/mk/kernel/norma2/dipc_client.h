/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
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
 *	Copyright 1994  Intel Corporation.
 *
 *	$Id: dipc_client.h,v 1.4 1994/11/18 20:57:35 mtm Exp $
 */

typedef int	dipc_enqueue_id_t;	/* for synchronizing on the reply */

/*
 *	kmsg enqueue request.
 */
extern dipc_enqueue_id_t dipc_kmsg_enqueue_request(
		ipc_port_t,
		ipc_kmsg_t,
		mach_msg_option_t,
		rdma_token_t);

/*
 *	bit definitions for the enqueue 'options' field
 */
#define	DIPC_BLOCKED_SENDERS	(0x80000000)	/* are there blocked senders? */
#define DIPC_EMMI_REPLY		(0x40000000)	/* VERY important message,
						 * ksvr will assign a thread to
						 * specifically process this
						 * message. Used to correct the
						 * brain-dead XMM interface
						 * deadlock problems.
						 */
#define DIPC_EMMI_REPLY_PRIV	(0x20000000)	/* Process NOW! (processed
						 * with VM privlege -- write
						 * completions).
						 */
#define DIPC_NOT_SPECIAL	(0x00000000)	/* NOT bit #0, just zero */



/*
 *	to collect the kmsg enqueue response.
 */
extern int dipc_kmsg_enqueue_await(dipc_enqueue_id_t, ipc_port_t);
#define	DIPC_QUEUE_MKMSG	0	/* meta-kmsg was enqueued */
#define	DIPC_QUEUE_KMSG		1	/* kmsg was enqueued */
#define	DIPC_QUEUE_FULL		2	/* port message queue was full */
#define	DIPC_QUEUE_INVALID	3	/* unknown port (uid was unknown) */
#define	DIPC_QUEUE_DEAD		4	/* inactive port (!ip_active()) */
#define	DIPC_QUEUE_WAS_FULL	5	/* early avail processing */

extern int dipc_buy_transits( int, dipc_uid_t, long *);
extern int dipc_sell_transits( int, dipc_uid_t, long *);

/*
 *  Debug logging support
 */
#define client_log1	NORMA_LOG1
#define client_log2	NORMA_LOG2
#define client_log3	NORMA_LOG3
#define client_log4	NORMA_LOG4
#define client_log5	NORMA_LOG5
#define client_log6	NORMA_LOG6

#define client_entry0(func) \
	NORMA_ENTRY0(CLIENT_LOG_ID, func)
#define client_entry1(func, a)      \
	NORMA_ENTRY1(CLIENT_LOG_ID, func, a)
#define client_entry2(func, a, b) \
	NORMA_ENTRY2(CLIENT_LOG_ID, func, a, b)
#define client_entry3(func, a, b, c) \
	NORMA_ENTRY3(CLIENT_LOG_ID, func, a, b, c)
#define client_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(CLIENT_LOG_ID, func, a, b, c, d)
#define client_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(CLIENT_LOG_ID, func, a, b, c, d, e)
#define client_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(CLIENT_LOG_ID, func, a, b, c, d, e, f)

