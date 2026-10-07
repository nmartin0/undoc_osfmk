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
 * Copyright 1994 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */

/*
 * $Id: dipc_mqueue.h,v 1.5 1994/11/18 20:57:57 mtm Exp $
 *
 * HISTORY
 * $Log: dipc_mqueue.h,v $
 * Revision 1.5  1994/11/18  20:57:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/11/11  18:17:04  andyp
 * PTS #: 10982, 11355
 * Mandatory?:  10982==no, 11355==yes
 * Description:
 *     for 10982:
 *         The queue limit enforcement used a protocol that could
 *         produce unbounded microkernel-to-microkernel traffic that
 *         could result in deadlock.  The protocol was changed so that
 *         the consumer of queue avail notifications need not produce a
 *         response, eliminating the need for no-blocked-senders notifications.
 *     for 11355:
 *         A separate RDMA group is defined for transmitting pagein
 *         responses to eliminate contention with other sends queued
 *         and to eliminate blocking the vnode pager "unexpectedly."
 *         EMMI reply messages are now also enqueued with the MACH_SEND_ALWAYS
 *         option.  The size of the default RDMA transmit group was increased
 *         from 32 to 42 handles (to support 32:1 PFS ratios).
 * Reviewer(s): cfleck,flb,sean,terry,lenb
 * Risk:        low
 * Testing:     150MCAT pccm2 runs, byte sats, aim3
 * Module(s):
 *         M ./norma2/dipc_blocked.c
 *         M ./norma2/dipc_client.c
 *         M ./norma2/dipc_debug.c
 *         M ./norma2/dipc_mqueue.c
 *         M ./norma2/dipc_mqueue.h
 *         M ./norma2/dipc_server.c
 *         M ./norma2/dipc_server.h
 *         M ./norma2/norma2_init.c
 *         M ./norma2/norma_transport.h
 *
 * Revision 1.3  1994/09/09  17:52:25  andyp
 * PTS #: 10760, 10618, 10660
 * Mandatory?:  10760==yes, others==no.
 * Description: The vnode pager needed to have access to a separate pool
 * 	of RDMA handles.  RPC/RDMA handle allocation has been
 * 	modified to preserve allocation request order.
 * 	Corrected a typo in kern/ast.h.
 * 	Reduced the overhead for page thawing (and removed old code).
 * 	Set ice_cube_pages=2 by default.
 * Reviewer(s): rkl
 * Risk:	     Medium
 * Testing:     byte sats, test case for 10660
 * Module(s):
 * 	M intel/pmap.c
 * 	M kern/ast.h
 * 	M norma2/dipc_client.h
 * 	M norma2/dipc_mqueue.c
 * 	M norma2/dipc_mqueue.h
 * 	M norma2/dipc_server.c
 * 	M norma2/dipc_server.h
 * 	M norma2/norma2_init.c
 * 	M norma2/norma_transport.h
 * 	M norma2/recv_engine.c
 * 	M rpc_rdma/rdma.c
 * 	M rpc_rdma/rpc.c
 * 	M vm/vm_user.c
 *
 * Revision 1.2  1994/07/12  21:29:23  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.1.2.7  1994/07/11  16:48:03  rkl
 *  New and improved logging.
 *
 * Revision 1.1.2.6  1994/07/05  17:47:21  rkl
 *  Added support for enqueueing simple kmsg's throught the RPC payload bay.
 *
 * Revision 1.1.2.5  1994/05/25  16:06:50  stans
 *   More log args.
 *
 * Revision 1.1.2.4  1994/04/27  23:20:15  rkl
 *  Added support to undo a kmsg sent to a dead or invalid port.
 *
 * Revision 1.1.2.3  1994/03/18  01:38:02  rkl
 *  Removed "retries" entry in stats and fixed bug where we would rety after
 *  getting a PORT_DEAD/INVALID error.
 *
 * Revision 1.1.2.2  1994/03/18  00:42:36  rkl
 *  Added extra stats.
 *
 * Revision 1.1.2.1  1994/03/15  00:34:16  rkl
 *  Made changes to support new norma logging capability.
 *
 * Revision 1.1  1994/03/10  07:44:24  rkl
 * Initial revision
 *
 */

#ifndef	_DIPC_MQUEUE_H_
#define	_DIPC_MQUEUE_H_

/*
 *  Stats structure.
 */
#define MQUEUE_STATS(a) dipc_mqueue_stats.a

typedef struct {
	unsigned long	enqueue_local;
	unsigned long	enqueue_meta;
	unsigned long	enqueue_net;
	unsigned long	payload_kmsg;
	unsigned long	queue_full;
	unsigned long	queue_was_full;
	unsigned long	unsend;
	unsigned long	send_priv;
	unsigned long	send_pagein;
} dipc_mqueue_stats_t;

#define mqueue_log1	NORMA_LOG1
#define mqueue_log2	NORMA_LOG2
#define mqueue_log3	NORMA_LOG3
#define mqueue_log4	NORMA_LOG4
#define mqueue_log5	NORMA_LOG5
#define mqueue_log6	NORMA_LOG6
#define mqueue_log7	NORMA_LOG7
#define mqueue_log8	NORMA_LOG8
#define mqueue_log9	NORMA_LOG9

#define mqueue_entry0(func) \
	NORMA_ENTRY0(MQUEUE_LOG_ID, func)
#define mqueue_entry1(func, a)      \
	NORMA_ENTRY1(MQUEUE_LOG_ID, func, a)
#define mqueue_entry2(func, a, b) \
	NORMA_ENTRY2(MQUEUE_LOG_ID, func, a, b)
#define mqueue_entry3(func, a, b, c) \
	NORMA_ENTRY3(MQUEUE_LOG_ID, func, a, b, c)
#define mqueue_entry4(func, a, b, c, d) \
	NORMA_ENTRY4(MQUEUE_LOG_ID, func, a, b, c, d)
#define mqueue_entry5(func, a, b, c, d, e) \
	NORMA_ENTRY5(MQUEUE_LOG_ID, func, a, b, c, d, e)
#define mqueue_entry6(func, a, b, c, d, e, f) \
	NORMA_ENTRY6(MQUEUE_LOG_ID, func, a, b, c, d, e, f)

#if __STDC__

extern	void		dipc_kmsg_destroy( ipc_kmsg_t );
extern	dipc_return_t	dipc_convert_kmsg_header( ipc_kmsg_t );
extern	kern_return_t	dipc_mqueue_send( ipc_port_t,
					  ipc_kmsg_t,
					  mach_msg_option_t,
					  mach_msg_timeout_t );

#else

extern	void		dipc_kmsg_destroy();
extern	dipc_return_t	dipc_convert_kmsg_header();
extern	kern_return_t	dipc_mqueue_send();

#endif 	/* __STDC__ */
#endif	/* _DIPC_MQUEUE_H_ */
