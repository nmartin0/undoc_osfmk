
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
 * $Id: norma_transport.h,v 1.7 1995/03/28 16:03:55 cfleck Exp $
 *
 * HISTORY:
 */

#ifndef	_NORMA_TRANSPORT_H_
#define	_NORMA_TRANSPORT_H_

/*
 * RPC/RDMA group definitions
 */
#define	NORMA_RPC_GROUP_INITIATOR	0	/* client requests */
#define	NORMA_RPC_GROUP_ENQUEUE		1	/* enqueue a meta_kmsg */
#define	NORMA_RPC_GROUP_CONTROL		2	/* queue avail, transits */

#define	NORMA_RDMA_GROUP_PAGEOUT	0	/* used by pageout thread */
#define	NORMA_RDMA_GROUP_XMIT		1	/* ordinary sends */
#define	NORMA_RDMA_GROUP_RECV		2	/* ordinary receives */
#define	NORMA_RDMA_GROUP_EMMI_REPLY	3	/* m_o_d_w_completed recvs */
#define	NORMA_RDMA_GROUP_XMIT_PRIV	4	/* m_o_d_w_completed sends */
#define	NORMA_RDMA_GROUP_XMIT_PAGEIN	5	/* outbound pagein replies */
#define	NORMA_RDMA_GROUP_VNODE_PAGER_RX	6	/* m_o_d_w/m_o_d_r, etc recvs */
#define	NORMA_RDMA_GROUP_VNODE_PAGER_TX	7	/* m_o_? sends */
#define NORMA_RDMA_TOTAL_GROUPS		8

/*
 * RPC group allocations
 */
#define NORMA_RPC_GROUP_INITIATOR_ALLOC	32
#define NORMA_RPC_GROUP_ENQUEUE_ALLOC	 8
#define NORMA_RPC_GROUP_CONTROL_ALLOC	 3

/*
 *  RPC class definitions
 */
#define	NORMA_RPC_CLASS_ENQUEUE		((rpc_class_t) 0)
#define	NORMA_RPC_CLASS_CONTROL		((rpc_class_t) 1)

/*
 * RDMA group allocations
 */
#define NORMA_RDMA_GROUP_XMIT_ALLOC		48
#define NORMA_RDMA_GROUP_RECV_ALLOC		8
#define NORMA_RDMA_GROUP_EMMI_PRIV_ALLOC	8
#define NORMA_RDMA_GROUP_PAGEOUT_ALLOC		9
#define NORMA_RDMA_GROUP_XMIT_PRIV_ALLOC	8
#define NORMA_RDMA_GROUP_PAGEIN_ALLOC		8
#define NORMA_RDMA_GROUP_VNODE_PAGER_RX_ALLOC	8
#define NORMA_RDMA_GROUP_VNODE_PAGER_TX_ALLOC	8

#endif	/* _NORMA_TRANSPORT_H_ */




