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
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: net_io.h,v $
 * Revision 1.7  1994/11/18  20:31:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/06/30  22:22:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.5  1993/06/25  19:30:49  arlin
 * added net_kmsg_collect_pool()
 *
 * Revision 1.4  1993/05/27  22:30:30  arlin
 * scatter/gather i/o for Hippi Driver
 *
 * Revision 1.3  1993/04/27  20:20:51  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.2.4  1993/05/11  20:02:23  bernard
 *      Moved net_kmsg_alloc/free macros to net_io.c. 
 *      They are now routines since we need to keep track
 *      of pool counts correctly.
 *      [93/04/29            bernard]
 *
 * Revision 1.1.2.3  1993/04/14  18:25:04  bernard
 *      Fixed grow/shrink pool to return new max count.
 *      [93/03/31            bernard]
 *
 *      Added new net buf interface declerations.
 *      [93/03/05            bernard]
 *
 * Revision 1.1.2.2  1993/01/19  21:34:20  bernard
 *      Added declarations for scatter/gather list network support.
 *      [1993/01/18  16:15:23  bernard]
 * 
 * Revision 1.1.10.2  1993/04/22  18:20:41  dleslie
 * First R1_0 release
 *
 * Revision 2.8  91/05/14  15:59:55  mrt
 * 	Correcting copyright
 * 
 * Revision 2.7  91/03/16  14:43:26  rpd
 * 	Added net_packet.
 * 	[91/01/14            rpd]
 * 
 * Revision 2.6  91/02/05  17:10:03  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:30:15  mrt]
 * 
 * Revision 2.5  91/01/08  15:09:56  rpd
 * 	Replaced NET_KMSG_GET, NET_KMSG_FREE
 * 	with net_kmsg_get, net_kmsg_put, net_kmsg_collect.
 * 	[91/01/05            rpd]
 * 
 * Revision 2.4  90/06/02  14:48:20  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:58:23  rpd]
 * 
 * Revision 2.3  90/02/22  20:02:26  dbg
 * 	kmsg->queue_head becomds kmsg->chain.
 * 	[90/01/25            dbg]
 * 
 * Revision 2.2  90/01/11  11:42:26  dbg
 * 	Make run in parallel.
 * 	[89/11/27            dbg]
 * 
 */
/* 
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	ll/89
 */

#ifndef	_DEVICE_NET_IO_H_
#define	_DEVICE_NET_IO_H_

/*
 * Utilities for playing with network messages.
 */

#include <mach/machine/vm_types.h>
#include <ipc/ipc_kmsg.h>

#include <kern/macro_help.h>
#include <kern/lock.h>
#include <kern/kalloc.h>

#include <device/net_status.h>

/*
 * A network packet is wrapped in a kernel message while in
 * the kernel.
 */

#define	net_kmsg(kmsg)		((net_rcv_msg_t)&(kmsg)->ikm_header)
#define	net_kmsg_sg(kmsg)	((net_rcv_msg_sg_t)&(kmsg)->ikm_header)

/*
 * Interrupt routines may allocate and free net_kmsgs with these
 * functions.  net_kmsg_get may return IKM_NULL.
 */

extern ipc_kmsg_t net_kmsg_get();
extern void net_kmsg_put();

/*
 * Network utility routines.
 */

extern void net_packet();
extern void net_filter();
extern io_return_t net_getstat();
extern io_return_t net_write();

/*
 * Non-interrupt code may allocate and free net_kmsgs with these functions.
 */

extern ipc_kmsg_t net_kmsg_alloc();
extern void net_kmsg_free(/* ipc_kmsg_t kmsg */);

/* Buffer pool types */
typedef enum {
	NET_POOL_INLINE,	/* Inline packet data */
	NET_POOL_SG		/* Scatter/Gather list packet data */
} net_pool_type_t;

typedef void *net_pool_t;	/* Handle to active buffer pool */
#define NET_POOL_NULL	((net_pool_t)0)

/*
 * Create Pool - Create a buffer pool
 *
 * Possible errors
 *	KERN_RESOURCE_SHORTAGE - unable to allocate pool resources.
 */
kern_return_t
net_kmsg_create_pool(
	/* net_pool_type_t ptype, */	/* Pool type */
	/* vm_size_t bufsize, */	/* Total buffer size (in bytes) */
	/* int minbufs,	*/		/* # of buffers in pool (to start with) */
	/* boolean_t sharable, */	/* Does caller want to share this pool */
	/* char *name, */		/* Name - Must be static! */
	/* net_pool_t *pool_result */); /* New Pool Handle (OUT arg) */

/*
 * Grow Pool - Add buffers to pool free list
 */
kern_return_t
net_kmsg_grow_pool(
	/* net_pool_t pool, */		/* Pool Handle */
	/* int nbufs, */		/* # of buffers to add to pool */
	/* int *newmax */);		/* # of buffers in pool (OUT arg) */

/*
 * Shrink Pool - Removed buffers from pool free list
 *
 * Possible errors
 *	KERN_INVALID_ARGUMENT - removal exceeds pool total
 */
kern_return_t
net_kmsg_shrink_pool(
	/* net_pool_t pool, */		/* Pool Handle */
	/* int nbufs, */		/* # of buffers to remove from pool */
	/* int *newmax */);		/* # of buffers in pool (OUT arg) */

/*
 * Collect Pool - Collect free queue pages from a net pool.
 */
void
net_kmsg_collect_pool(/* net_pool_t pool */);	/* Pool Handle */

/*
 * Allocate/Free (possibly blocking)
 */

/*
 * Allocate Buffer - Allocate buffer from pool
 *
 * Possible errors
 *	IKM_NULL - pool free list empty and no more resources
 */
ipc_kmsg_t
net_kmsg_alloc_buf(
	/* net_pool_t pool */);		/* Pool Handle */

/*
 * Free Buffer - Free buffer to pool
 */
kern_return_t
net_kmsg_free_buf(
	/* net_pool_t pool, */		/* Pool Handle */
	/* ipc_kmsg_t kmsg */);

/*
 * Get/Put (non-blocking)
 */

/*
 * Get Buffer - Get free buffer from pool free list
 *
 * Possible errors
 *	IKM_NULL - no available buffers
 */
ipc_kmsg_t
net_kmsg_get_buf(
	/* net_pool_t pool */);		/* Pool Handle */

/*
 * Put Buffer - Put buffer back on pool free list
 */
void
net_kmsg_put_buf(
	/* net_pool_t pool, */		/* Pool Handle */
	/* ipc_kmsg_t kmsg */);

/*
 * Incoming packet.  Header has already been moved to proper place.
 * We are already at splimp.
 */
void
net_packet_pool(
	/* net_pool_t		pool, */
	/* register struct ifnet	*ifp, */
	/* register ipc_kmsg_t	kmsg, */
	/* unsigned int		count, */
	/* boolean_t		priority */);

/*
 * Set a filter for a network interface.
 *
 * We are given a naked send right for the rcv_port.
 * If we are successful, we must consume that right.
 */
io_return_t
net_set_filter_pool(
	/* net_pool_t	pool, */
	/* struct ifnet	*ifp, */
	/* ipc_port_t	rcv_port, */
	/* int		priority, */
	/* filter_t	*filter, */
	/* unsigned int	filter_count */);

#endif	_DEVICE_NET_IO_H_
