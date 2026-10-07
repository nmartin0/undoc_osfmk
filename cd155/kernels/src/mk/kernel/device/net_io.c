/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995 Intel Corporation
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
 * $Log: net_io.c,v $
 * Revision 1.15  1994/11/18  20:31:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/07/18  22:48:07  andyp
 * Correcting a merge error (from merging NORMA2).  This looks like it has
 * happened before...
 *
 *  Reviewer: andyp/hobbes
 *  Risk: low
 *  Benefit or PTS #:
 *  Testing: minimal
 *  Module(s):
 *
 * Revision 1.13  1994/07/12  19:17:56  andyp
 * Merge of the NORMA2 branch back to the mainline.
 *
 * Revision 1.12  1994/05/10  16:15:00  arlin
 *  merged back in a change to not call round_page for SG messages.
 *  R1.2 and R1.3 merged.
 *
 *  Reviewer: Bernie Keany, Arlin Davis
 *  Risk:medium
 *  Benefit or PTS #: R1.2 and R1.3 in sync.
 *  Testing: HiPPI Testing
 *  Module(s): net_io.c
 *
 * Revision 1.11  1994/01/27  21:18:58  joel
 *  Reviewer: rwd@osf.org
 *  Risk:medium
 *  Benefit or PTS #: 7865
 *  Testing: EATs
 *  Module(s): kernel/device/net_io.c
 *
 * Revision 1.10.2.2  1994/02/08  02:04:01  hobbes
 *  Reviewer: Arlin Davis
 *  Risk: Medium
 *  Benefit or PTS #: 8081
 *  Testing: HIPPI EATS
 *  Module(s): net_io.c
 *
 * merged back in a change to not call round_page for SG messages.
 *
 * Revision 1.10.2.1  1994/01/27  20:53:57  joel
 *  Reviewer: rwd@osf.org, terry prickett
 *  Risk:medium
 *  Benefit or PTS #: 7865
 *  Testing: EATS
 *  Module(s): device/net_io.c
 *
 * Revision 1.10.4.2  1994/05/18  22:35:33  stans
 * NORMA_IPC --> NORMA_VM
 *
 * Revision 1.10.4.1  1994/02/16  00:21:39  andyp
 * Updates from the mainline.
 *
 * Revision 1.11  1994/01/27  21:18:58  joel
 *  Reviewer: rwd@osf.org
 *  Risk:medium
 *  Benefit or PTS #: 7865
 *  Testing: EATs
 *  Module(s): kernel/device/net_io.c
 *
 * Revision 1.10.2.1  1994/01/27  20:53:57  joel
 *  Reviewer: rwd@osf.org, terry prickett
 *  Risk:medium
 *  Benefit or PTS #: 7865
 *  Testing: EATS
 *  Module(s): device/net_io.c
 *
 * Revision 1.10  1993/10/25  22:32:10  arlin
 * merged r1.1 fix with r1.2 branch, bug #6746
 *
 * Revision 1.9.2.1  1993/09/23  21:10:40  terry
 * an IFF_SG type device pool should NOT be increased
 * when a new filter is set. net_set_filter_pool().
 *
 * Revision 1.9  1993/08/26  18:50:24  arlin
 * do not allow duplicate filters for
 * S/G type devices in net_set_filter_pool()
 *
 * Revision 1.7  1993/06/25  19:35:00  arlin
 * added net_kmsg_collect_pool().
 * fixes to net_kmsg_shrink_pool() and
 * net_kmsg_grow_pool() routines.
 *
 * Revision 1.6  1993/06/05  00:50:41  arlin
 * clean up some debug in net_set_filter
 *
 * Revision 1.5  1993/05/27  22:30:07  arlin
 * scatter/gather i/o for Hippi Driver
 * added deadname notification in net_set_filter_pool()
 *
 * Revision 1.4  1993/05/12  16:49:09  top
 * net_write() was modified to handle asyncronous writes correctly.
 * 
 * Revision 1.3  1993/04/27  20:20:48  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.2.7  1993/06/29  15:10:33  bernard
 *      Fixed synchronization problems with net_thread, net_ast and
 *       net_kmsg_collect.  Problems were related to holding locks while
 *       blocking (net_thread_lock, net_pool_lock).
 *      [93/06/29            bernard]
 *
 * Revision 1.2.2.6  1993/06/18  14:04:49  sjs
 *      Deleted initialization of nonexistant net_free_lock.
 *      From [stans@ssd.intel.com]
 *      [93/06/18            sjs]
 *
 * Revision 1.2.2.5  1993/06/01  16:57:16  sjs
 *      Fixed net_write to wait for fully sync operations.
 *      [93/04/22            bernard]
 *      [93/06/01            sjs]
 *
 * Revision 1.2.2.4  1993/05/11  20:02:19  bernard
 *      Picked up bug fixes/debugging code from Intel SSD.
 *      Moved net_kmsg_alloc/free from include file as macros
 *       to here so we can keep track of resources allocated for
 *       the inline page pool.
 *      [93/04/29            bernard]
 *
 * Revision 1.2.2.3  1993/04/14  18:25:29  bernard
 *      Fixed grow/shrink pool to return new max count.
 *      [93/03/31            bernard]
 *
 *      Fixed shrink pool routine to not return with lock held in error case.
 *      [93/03/25            bernard]
 *
 *      Fixed SG kmsgs to not use NORMA specific field to store copy object.
 *      [93/03/22            bernard]
 *
 *      Fully integrate sg & inline packets and add new net_kmsg
 *       interfaces for kmsg pools.
 *      [93/03/18            bernard]
 *
 * Revision 1.2.2.2  1993/01/19  21:34:15  bernard
 *      Fixed up net_thread_awake synchronization with net_sg code.
 *      [1993/01/19  15:43:46  bernard]
 *
 *      Put all net scatter/gather code under NET_SG conditional.
 *      [1993/01/18  22:09:34  bernard]
 *
 *      Merged in scatter/gather list network changes.
 *      [1993/01/18  16:14:53  bernard]
 *
 * Revision 1.2  1992/11/25  01:04:50  robert
 *      integrate changes below for norma_14
 *
 *      Philippe Bernadat (bernadat) at gr.osf.org
 *      Fixed packet filtering, net_do_filter() is called with
 *      real network data starting AFTER a logical
 *      header (struct packet_header). [paire@gr.osf.org]
 *      [1992/11/13  19:23:13  robert]
 *
 * Revision 1.1.10.2  1993/04/22  18:20:38  dleslie
 * First R1_0 release
 *
 * Revision 2.20.3.3  92/03/28  10:04:52  jeffreyh
 * 	Reduced net_queue_free_min to 10 in the NORMA case.
 * 	[92/03/17            jeffreyh]
 * 
 * Revision 2.20.3.2  92/03/03  16:14:04  jeffreyh
 * 	Raised net_queue_free_min on NORMA_IPC. Starting with the Norma branch
 * 	lost packets have been seen. This should be looked at to see if it
 * 	is machine dependent, NORMA dependent, or just a bug.
 * 	[92/02/27            jeffreyh]
 * 	Pick up changes from MK68
 * 	[92/02/26  11:03:30  jeffreyh]
 * 
 * Revision 2.21  92/01/03  20:03:57  dbg
 * 	Add: NETF_PUSHHDR, NETF_PUSHSTK, NETF_PUSHIND, NETF_PUSHHDRIND.
 * 	[91/12/23            dbg]
 * 
 * Revision 2.20.3.1  92/01/21  21:49:55  jsb
 * 	Changed parameters to netipc_net_packet.
 * 	[92/01/17  18:32:12  jsb]
 * 
 * Revision 2.20  91/08/28  11:11:28  jsb
 * 	Panic if network write attempted with continuation.
 * 	[91/08/12  17:29:53  dlb]
 * 
 * Revision 2.19  91/08/24  11:55:55  af
 * 	Missing include for Spls definitions.
 * 	[91/08/02  02:45:16  af]
 * 
 * Revision 2.18  91/08/03  18:17:43  jsb
 * 	Added NORMA_ETHER support.
 * 	[91/07/24  22:54:41  jsb]
 * 
 * Revision 2.17  91/05/14  15:59:34  mrt
 * 	Correcting copyright
 * 
 * Revision 2.16  91/05/10  11:48:47  dbg
 * 	Don't forget to copy the packet size when duplicating a packet
 * 	for multiple filters in net_filter().
 * 	[91/05/09            dpj]
 * 
 * Revision 2.15  91/03/16  14:43:14  rpd
 * 	Added net_thread, net_thread_continue.
 * 	[91/02/13            rpd]
 * 	Split net_rcv_msg_queue into high and low priority queues.
 * 	Cap the total number of buffers allocated.
 * 	[91/01/14            rpd]
 * 
 * 	Added net_rcv_msg_queue_size, net_rcv_msg_queue_max.
 * 	[91/01/12            rpd]
 * 
 * Revision 2.14  91/02/14  14:37:07  mrt
 * 	Added garbage collection of dead filters.
 * 	[91/02/12  12:11:10  af]
 * 
 * Revision 2.13  91/02/05  17:09:54  mrt
 * 	Changed to new Mach copyright
 * 	[91/01/31  17:30:04  mrt]
 * 
 * Revision 2.12  91/01/08  15:09:48  rpd
 * 	Replaced NET_KMSG_GET, NET_KMSG_FREE
 * 	with net_kmsg_get, net_kmsg_put, net_kmsg_collect.
 * 	Increased net_kmsg_ilist_min to 4.
 * 	[91/01/05            rpd]
 * 	Fixed net_rcv_msg_thread to round message sizes up to an int multiple.
 * 	[90/12/07            rpd]
 * 
 * 	Fixed net_rcv_msg_thread to not set vm_privilege.
 * 	[90/11/29            rpd]
 * 
 * Revision 2.11  90/09/09  23:20:00  rpd
 * 	Zero the mapped_size stats for non mappable interfaces.
 * 	[90/08/30  17:41:00  af]
 * 
 * Revision 2.10  90/08/27  21:55:18  dbg
 * 	If multiple filters receive a packet, copy the header as well as
 * 	the body.  Fix from Dan Julin.
 * 	[90/08/27            dbg]
 * 
 * 	Fix filter check to account for literal word.
 * 	[90/07/17            dbg]
 * 
 * Revision 2.9  90/08/06  15:06:57  rwd
 * 	Fixed a bug in parse_net_filter(), that was reading the
 * 	litteral from NETF_PUSHLIT as an instruction.
 * 	[90/07/18  21:56:20  dpj]
 * 
 * Revision 2.8  90/06/02  14:48:14  rpd
 * 	Converted to new IPC.
 * 	[90/03/26  21:57:43  rpd]
 * 
 * Revision 2.7  90/02/22  20:02:21  dbg
 * 	Track changes to kmsg structure.
 * 	[90/01/31            dbg]
 * 
 * Revision 2.6  90/01/11  11:42:20  dbg
 * 	Make run in parallel.
 * 	[89/12/15            dbg]
 * 
 * Revision 2.5  89/12/08  19:52:22  rwd
 * 	Picked up changes from rfr to minimize wired down memory
 * 	[89/11/21            rwd]
 * 
 * Revision 2.4  89/09/08  11:24:35  dbg
 * 	Convert to run in kernel task.  Removed some lint.
 * 	[89/07/26            dbg]
 * 
 * Revision 2.3  89/08/11  17:55:18  rwd
 * 	Picked up change from rfr which made zone collectable and
 * 	decreased min net_kmesg to 2.
 * 	[89/08/10            rwd]
 * 
 * Revision 2.2  89/08/05  16:06:58  rwd
 * 	Changed device_map to device_task_map
 * 	[89/08/04            rwd]
 * 
 * 13-Mar-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.  
 *
 */
/* 
 *	Author: David B. Golub, Carnegie Mellon University
 *	Date: 	3/98
 *
 *	Packet filter code taken from vaxif/enet.c written		 
 *		CMU and Stanford. 
 */

#include <mach_kdb.h>
#include <norma_ether.h>
#include <norma_ipc.h>
#include <norma_vm.h>

#include <device/net_status.h>
#include <machine/machparam.h>		/* spl definitions */
#include <device/net_io.h>
#include <device/if_hdr.h>
#include <device/io_req.h>
#include <device/ds_routines.h>
#include <device/device_port.h>   

#include <mach/boolean.h>
#include <mach/vm_param.h>

#include <ipc/ipc_port.h>
#include <ipc/ipc_kmsg.h>
#include <ipc/ipc_mqueue.h>

#include <kern/counters.h>
#include <kern/lock.h>
#include <kern/processor.h>
#include <kern/queue.h>
#include <kern/sched_prim.h>
#include <kern/thread.h>

#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <vm/vm_user.h>
#include <vm/pmap.h>

#if	NORMA_ETHER
#include <norma/ipc_ether.h>
#endif	/*NORMA_ETHER*/

#include <machine/machparam.h>

/*
 * Network IO.
 */

/*
 * Locking:
 *
 * Each net_pool structure has a set of locks covering some elements in that structure.
 * This has been inherited from the original code.
 *
 * net_pool_lock - protects the net_pool?* fields. Held at spl0().
 *  This lock is currently not necessary since we have no manipulation
 *  of data it protects after pool creation, but is included for future
 *  interface expansion.
 *
 * net_queue_lock - protects the net_queue?* fields. Held at splimp().
 * net_queue_free_lock - protects the net_queue_free?* fields. Held at splimp().
 * net_kmsg_total_lock - protects the net_kmsg variables. Held at spl0().
 *
 * There are two global locks.
 *
 * The net_thread_lock is used to exclude the net_thread and to check/set the
 * wakeup state for that thread.  Held at splimp().
 *
 * The net_pool_lock protects additions to the net_pool_list (list of all pools).
 * Held at spl0().
 *
 */

/*
 * Each interface has a write port and a set of read ports.
 * Each read port has one or more filters to determine what packets
 * should go to that port.
 */

/*
 * Receive port for net, with packet filter.
 */
struct net_rcv_port {
	queue_chain_t	chain;		/* list of open_descriptors */
	ipc_port_t	rcv_port;	/* port to send packet to */
	int		rcv_qlimit;	/* port's qlimit */
	int		rcv_count;	/* number of packets received */
	int		priority;	/* priority for filter */
	filter_t	*filter_end;	/* pointer to end of filter */
	filter_t	filter[NET_MAX_FILTER];
					/* filter operations */
};
typedef struct net_rcv_port *net_rcv_port_t;

/*
 * Private Pool descriptor
 */
struct net_pool {
	struct net_pool	*net_pool_next;			/* next pool on list */
	int		net_pool_refcount;
	char		*net_pool_name;
	net_pool_type_t	net_pool_type;
	boolean_t	net_pool_sharable;		/* Attach possible? */
	vm_offset_t	net_pool_va;			/* VA for SG packet filter */
	vm_size_t	net_kmsg_size;			/* packet size */

	/*
	 * List of net kmsgs queued to be sent to users.
	 * Messages can be high priority or low priority.
	 * The network thread processes high priority messages first.
	 */
	decl_simple_lock_data(,net_queue_lock)
	struct ipc_kmsg_queue	net_queue_high;
	int		net_queue_high_size;
	int		net_queue_high_max;		/* for debugging */
	struct ipc_kmsg_queue	net_queue_low;
	int		net_queue_low_size;
	int		net_queue_low_max;		/* for debugging */

	/*
	 * List of net kmsgs that can be touched at interrupt level.
	 * If it is empty, we will also steal low priority messages.
	 */
	decl_simple_lock_data(,net_queue_free_lock)
	struct ipc_kmsg_queue	net_queue_free;
	int		net_queue_free_size;		/* on free list */

	/*
	 * Pool Buffer total & limit
	 */
	decl_simple_lock_data(,net_kmsg_total_lock)
	int		net_kmsg_total;			/* total allocated */
	int		net_kmsg_max;			/* max possible */
	int		net_queue_free_min;

	/*
	 * Statistics
	 */
	int		net_queue_free_max;		/* for debugging */
	int		net_queue_free_hits;		/* for debugging */
	int		net_queue_free_steals;		/* for debugging */
	int		net_queue_free_misses;		/* for debugging */

	int		net_kmsg_send_high_hits;	/* for debugging */
	int		net_kmsg_send_low_hits;		/* for debugging */
	int		net_kmsg_send_high_misses;	/* for debugging */
	int		net_kmsg_send_low_misses;	/* for debugging */
};

/*
 * In order to store the copy object with the kmsg, we need more space.
 */
struct net_rcv_msg_sg_copy {
	struct net_rcv_msg_sg	nmsg;		/* standard sg net_kmsg */
	vm_map_copy_t		copy;		/* copy object */
};

int		net_thread_awaken = 0;		/* for debugging */
int		net_ast_taken = 0;		/* for debugging */

/*
 * List of net kmsgs queued to be sent to users.
 * Messages can be high priority or low priority.
 * The network thread processes high priority messages first.
 */
decl_simple_lock_data(,net_thread_lock)
boolean_t	net_thread_awake = FALSE;

/*
 * Pool list.
 */
decl_simple_lock_data(,net_pool_lock)
struct net_pool *net_pool_list = (struct net_pool *)NET_POOL_NULL;

/*
 * Statically defined pool for compatibility with old interface.
 */
net_pool_t inline_pagepool;

/*
 * This value is critical to network performance.
 * At least this many buffers should be sitting in net_queue_free.
 * If this is set too small, we will drop network packets.
 * Even a low drop rate (<1%) can cause severe network throughput problems.
 * We add one to net_queue_free_min for every filter.
 */
#if	NORMA_IPC || NORMA2
/*
 * The cube and maybe the NORMA code seem to have problems with just 3, 
 * and the cube is known to need at least 10. Some ethernet problems
 * have also shown up on the 386's in a norma cluster.
 * This should be looked at and understood if it is an iPSC problem,
 * a norma problem, or a bug.
 */
int		net_inline_free_min = 10;
#else	/* NORMA_IPC || NORMA2 */
int		net_inline_free_min = 3;
#endif	/* NORMA_IPC || NORMA2 */

vm_size_t	net_kmsg_size;
zone_t		net_rcv_zone;

/*
 *	We want more buffers when there aren't enough in the free queue
 *	and the low priority queue.  However, we don't want to allocate
 *	more than net_kmsg_max.
 */

#define net_kmsg_want_more(p)		\
	((((p)->net_queue_free_size + (p)->net_queue_low_size) \
	  < (p)->net_queue_free_min) && \
	 ((p)->net_kmsg_total < (p)->net_kmsg_max))

/*
 * Collect free queue pages from all net pools.
 */
void net_kmsg_collect()
{
	register struct net_pool *listp, *p;

        /*
         * Get current list start.
	 */

	simple_lock(&net_pool_lock);
	listp = net_pool_list;
	simple_unlock(&net_pool_lock);

	for (p = listp; p != NET_POOL_NULL; p = p->net_pool_next) {
		net_kmsg_collect_pool(p);
	}
}

void net_kmsg_more(pool)
	net_pool_t pool;
{
	register struct net_pool *p = (struct net_pool *)pool;
	register ipc_kmsg_t kmsg;

	/*
	 * Replenish net kmsg pool if low.  We don't have the locks
	 * necessary to look at these variables, but that's OK because
	 * misread values aren't critical.  The danger in this code is
	 * that while we allocate buffers, interrupts are happening
	 * which take buffers out of the free list.  If we are not
	 * careful, we will sit in the loop and allocate a zillion
	 * buffers while a burst of packets arrives.  So we count
	 * buffers in the low priority queue as available, because
	 * net_kmsg_get will make use of them, and we cap the total
	 * number of buffers we are willing to allocate.
	 */

	while (net_kmsg_want_more(p)) {
		kmsg = net_kmsg_alloc_buf((net_pool_t)p);
		if (kmsg == IKM_NULL)
			break;
		net_kmsg_put_buf((net_pool_t)p, kmsg);
	}
}

/*
 *	ethernet_priority:
 *
 *	This function properly belongs in the ethernet interfaces;
 *	it should not be called by this module.  (We get packet
 *	priorities as an argument to net_filter.)  It is here
 *	to avoid massive code duplication.
 *
 *	Returns TRUE for high-priority packets.
 */

boolean_t ethernet_priority(kmsg)
	ipc_kmsg_t kmsg;
{
	register unsigned char *addr =
		(unsigned char *) net_kmsg(kmsg)->header;

	/*
	 *	A simplistic check for broadcast packets.
	 */

	if ((addr[0] == 0xff) && (addr[1] == 0xff) &&
	    (addr[2] == 0xff) && (addr[3] == 0xff) &&
	    (addr[4] == 0xff) && (addr[5] == 0xff))
	    return FALSE;
	else
	    return TRUE;
}

mach_msg_type_t header_type = {
	MACH_MSG_TYPE_BYTE,
	8,
	NET_HDW_HDR_MAX,
	TRUE,
	FALSE,
	FALSE,
	0
};

mach_msg_type_t packet_type = {
	MACH_MSG_TYPE_BYTE,	/* name */
	8,			/* size */
	0,			/* number */
	TRUE,			/* inline */
	FALSE,			/* longform */
	FALSE			/* deallocate */
};


mach_msg_type_long_t data_type_long = {
	{ 0,			/* name,   0 for longform */
	  0,			/* size,   0 for longform */
	  0,			/* number, 0 for longform */
	  FALSE,		/* inline */
	  TRUE,			/* longform */
	  FALSE },		/* deallocate */

	MACH_MSG_TYPE_BYTE,	/* name */
	8,			/* size */
	0,			/* number */
};

boolean_t
net_do_filter(infp, data, data_count, header)
	net_rcv_port_t	infp;
	char *		data;
	unsigned int	data_count;
	char *		header;
{
	int		stack[NET_FILTER_STACK_DEPTH+1];
	register int	*sp;
	register filter_t	*fp, *fpe;
	register unsigned int	op, arg;

	/*
	 * The filter accesses the header and data
	 * as unsigned short words.
	 */
	data_count /= sizeof(unsigned short);

#define	data_word	((unsigned short *)data)
#define	header_word	((unsigned short *)header)

	sp = &stack[NET_FILTER_STACK_DEPTH];
	fp = &infp->filter[0];
	fpe = infp->filter_end;

	*sp = TRUE;

	while (fp < fpe) {
	    arg = *fp++;
	    op = NETF_OP(arg);
	    arg = NETF_ARG(arg);

	    switch (arg) {
		case NETF_NOPUSH:
		    arg = *sp++;
		    break;
		case NETF_PUSHZERO:
		    arg = 0;
		    break;
		case NETF_PUSHLIT:
		    arg = *fp++;
		    break;
		case NETF_PUSHIND:
		    arg = *sp++;
		    if (arg >= data_count)
			return FALSE;
		    arg = data_word[arg];
		    break;
		case NETF_PUSHHDRIND:
		    arg = *sp++;
		    if (arg >= NET_HDW_HDR_MAX/sizeof(unsigned short))
			return FALSE;
		    arg = header_word[arg];
		    break;
		default:
		    if (arg >= NETF_PUSHSTK) {
			arg = sp[arg - NETF_PUSHSTK];
		    }
		    else if (arg >= NETF_PUSHHDR) {
			arg = header_word[arg - NETF_PUSHHDR];
		    }
		    else {
			arg -= NETF_PUSHWORD;
			if (arg >= data_count)
			    return FALSE;
			arg = data_word[arg];
		    }
		    break;

	    }
	    switch (op) {
		case NETF_OP(NETF_NOP):
		    *--sp = arg;
		    break;
		case NETF_OP(NETF_AND):
		    *sp &= arg;
		    break;
		case NETF_OP(NETF_OR):
		    *sp |= arg;
		    break;
		case NETF_OP(NETF_XOR):
		    *sp ^= arg;
		    break;
		case NETF_OP(NETF_EQ):
		    *sp = (*sp == arg);
		    break;
		case NETF_OP(NETF_NEQ):
		    *sp = (*sp != arg);
		    break;
		case NETF_OP(NETF_LT):
		    *sp = (*sp < arg);
		    break;
		case NETF_OP(NETF_LE):
		    *sp = (*sp <= arg);
		    break;
		case NETF_OP(NETF_GT):
		    *sp = (*sp > arg);
		    break;
		case NETF_OP(NETF_GE):
		    *sp = (*sp >= arg);
		    break;
		case NETF_OP(NETF_COR):
		    if (*sp++ == arg)
			return (TRUE);
		    break;
		case NETF_OP(NETF_CAND):
		    if (*sp++ != arg)
			return (FALSE);
		    break;
		case NETF_OP(NETF_CNOR):
		    if (*sp++ == arg)
			return (FALSE);
		    break;
		case NETF_OP(NETF_CNAND):
		    if (*sp++ != arg)
			return (TRUE);
		    break;
		case NETF_OP(NETF_LSH):
		    *sp <<= arg;
		    break;
		case NETF_OP(NETF_RSH):
		    *sp >>= arg;
		    break;
		case NETF_OP(NETF_ADD):
		    *sp += arg;
		    break;
		case NETF_OP(NETF_SUB):
		    *sp -= arg;
		    break;
	    }
	}
	return ((*sp) ? TRUE : FALSE);

#undef	data_word
#undef	header_word

}

void
reorder_queue(first, last)
	register queue_t	first, last;
{
	register queue_entry_t	prev, next;

	prev = first->prev;
	next = last->next;

	prev->next = last;
	next->prev = first;

	last->prev = prev;
	last->next = first;

	first->next = next;
	first->prev = last;
}

/*
 * Run a packet through the filters, returning a list of messages.
 * We are *not* called at interrupt level.
 */
void
net_filter_inline(p, kmsg, send_list)
	register struct net_pool	*p;
	register ipc_kmsg_t		kmsg;
	ipc_kmsg_queue_t		send_list;
{
	register struct ifnet	*ifp;
	register net_rcv_port_t	infp, nextfp;
	register ipc_kmsg_t	new_kmsg;
	net_rcv_port_t		dead = 0;
	int count = net_kmsg(kmsg)->net_rcv_msg_packet_count;

	ifp = (struct ifnet *) kmsg->ikm_header.msgh_remote_port;
	ipc_kmsg_queue_init(send_list);

	/*
	 * Unfortunately we can't allocate or deallocate memory
	 * while holding this lock.  And we can't drop the lock
	 * while examining the filter list.
	 */
	simple_lock(&ifp->if_rcv_port_list_lock);
	for (infp = (net_rcv_port_t) queue_first(&ifp->if_rcv_port_list);
	     !queue_end(&ifp->if_rcv_port_list, (queue_entry_t)infp);
	     infp = nextfp) {
	    nextfp = (net_rcv_port_t) queue_next(&infp->chain);

	    if (net_do_filter(infp,
			net_kmsg(kmsg)->packet[sizeof(struct packet_header)],
		        count, net_kmsg(kmsg)->header)) {
		register ipc_port_t dest;

		/*
		 * Make a send right for the destination.
		 */

		dest = ipc_port_copy_send(infp->rcv_port);
		if (!IP_VALID(dest)) {
		    /*
		     * This filter is dead.  We remove it from the
		     * filter list and set it aside for deallocation.
		     */

		    queue_remove(&ifp->if_rcv_port_list, infp,
				 net_rcv_port_t, chain);
		    infp->chain.next = (queue_entry_t) dead;
		    dead = infp;
		    break;
		}

		/*
		 * Deliver copy of packet to this channel.
		 */
		if (ipc_kmsg_queue_empty(send_list)) {
		    /*
		     * Only receiver, so far
		     */
		    new_kmsg = kmsg;
		} else {
		    /*
		     * Other receivers - must allocate message and copy.
		     */
		    new_kmsg = net_kmsg_get_buf((net_pool_t)p);
		    if (new_kmsg == IKM_NULL) {
			ipc_port_release_send(dest);
			break;
		    }

		    bcopy(
			net_kmsg(kmsg)->packet,
			net_kmsg(new_kmsg)->packet,
			count);
		    bcopy(
			net_kmsg(kmsg)->header,
			net_kmsg(new_kmsg)->header,
			NET_HDW_HDR_MAX);
		    net_kmsg(new_kmsg)->net_rcv_msg_packet_count = count;
		}

		new_kmsg->ikm_header.msgh_remote_port = (mach_port_t) dest;
		ipc_kmsg_enqueue(send_list, new_kmsg);

	      	{ 
		register net_rcv_port_t prevfp;
		int rcount = ++infp->rcv_count;

		/*
		 * See if ordering of filters is wrong
		 */
		 if (infp->priority >= NET_HI_PRI) {
		    prevfp = (net_rcv_port_t) queue_prev(&infp->chain);
		    /*
		     * If infp is not the first element on the queue,
		     * and the previous element is at equal priority
		     * but has a lower count, then promote infp to
		     * be in front of prevfp.
		     */
		    if ((queue_t)prevfp != &ifp->if_rcv_port_list &&
			infp->priority == prevfp->priority) {
			/*
			 * Threshold difference to prevent thrashing
			 */
			if (100 + prevfp->rcv_count < rcount)
			    reorder_queue(&prevfp->chain, &infp->chain);
		    }
		    /*
		     * High-priority filter -> no more deliveries
		     */
		    break;
		 }
	        } 
	    } 
	}  
	simple_unlock(&ifp->if_rcv_port_list_lock);

	/*
	 * Deallocate dead filters.
	 */
	for (infp = dead; infp != 0; infp = nextfp) {
	    nextfp = (net_rcv_port_t) infp->chain.next;

	    ipc_port_release_send(infp->rcv_port);
	    simple_lock(&p->net_kmsg_total_lock);
	    p->net_queue_free_min--;
	    p->net_kmsg_max -= infp->rcv_qlimit + 1;
	    simple_unlock(&p->net_kmsg_total_lock);
	    zfree(net_rcv_zone, (vm_offset_t) infp);
	}

	if (ipc_kmsg_queue_empty(send_list)) {
	    /* Not sent - recycle */
	    net_kmsg_put_buf((net_pool_t)p, kmsg);
	}
}

/*
 * Run a scatter/gather packet through the filters, returning a list of messages.
 * We are *not* called at interrupt level.
 */
void
net_filter_sg(p, kmsg, send_list, net_data)
	register struct net_pool	*p;
	register ipc_kmsg_t		kmsg;
	ipc_kmsg_queue_t		send_list;
	struct net_rcv_msg_sg_data	*net_data;
{
	register struct ifnet	*ifp;
	register net_rcv_port_t	infp, nextfp;
	register ipc_kmsg_t	new_kmsg;
	register ipc_port_t dest;
	net_rcv_port_t		dead = 0;
	int count = net_kmsg_sg(kmsg)->net_rcv_msg_sg_packet_count;

	ifp = (struct ifnet *) kmsg->ikm_header.msgh_remote_port;
	ipc_kmsg_queue_init(send_list);

	/*
	 * Unfortunately we can't allocate or deallocate memory
	 * while holding this lock.  And we can't drop the lock
	 * while examining the filter list.
	 */
	simple_lock(&ifp->if_rcv_port_list_lock);
	for (infp = (net_rcv_port_t) queue_first(&ifp->if_rcv_port_list);
	     !queue_end(&ifp->if_rcv_port_list, (queue_entry_t)infp);
	     infp = nextfp) {
	    nextfp = (net_rcv_port_t) queue_next(&infp->chain);
	
	    /*
	     * Make a send right for the destination.
	     */
	    dest = ipc_port_copy_send(infp->rcv_port);

	    if (!IP_VALID(dest)) {

	       /*
	     	* This filter is dead.  We remove it from the
	     	* filter list and set it aside for deallocation.
	     	*/
		
	    	queue_remove(&ifp->if_rcv_port_list,infp,net_rcv_port_t,chain);
		infp->chain.next = 0;
	    	dead = infp;
	        continue;
	    }

	    if (net_do_filter(infp, net_data->packet, count, net_data->header)) {

		/*
		 * Deliver copy of packet to this channel.
		 */
		if (ipc_kmsg_queue_empty(send_list)) {
		    /*
		     * Only receiver, so far
		     */
		    new_kmsg = kmsg;
		    infp->rcv_count++;
		    new_kmsg->ikm_header.msgh_remote_port = (mach_port_t) dest;
		    ipc_kmsg_enqueue(send_list, new_kmsg);
		} else {
  		    /*
		     * Other receivers?!  can't do this!
		     */
		     printf("net_filter_sg: too many receivers, msg sent to highest priority\n");
		}
	    }
	}

	simple_unlock(&ifp->if_rcv_port_list_lock);

	/*
	 * Deallocate dead filters.
	 */
	for (infp = dead; infp != 0; infp = nextfp) {
	    nextfp = (net_rcv_port_t) infp->chain.next;
	    ipc_port_release_send(infp->rcv_port);
	    zfree(net_rcv_zone, (vm_offset_t) infp);
	}

	if (ipc_kmsg_queue_empty(send_list)) {
	    /* Not sent - recycle */
	    net_kmsg_put_buf((net_pool_t)p, kmsg);
	}
}

void
net_package_inline(kmsg)
	ipc_kmsg_t kmsg;
{
	int count;

	/*
	 * Fill in the rest of the kmsg.
	 */
	count = net_kmsg(kmsg)->net_rcv_msg_packet_count;

	ikm_init_special(kmsg, IKM_SIZE_NETWORK);
	
	kmsg->ikm_header.msgh_bits =
		MACH_MSGH_BITS(MACH_MSG_TYPE_PORT_SEND, 0);
	/* remember message sizes must be rounded up */
	kmsg->ikm_header.msgh_size =
		((mach_msg_size_t) (sizeof(struct net_rcv_msg)
				    - NET_RCV_MAX + count))+3 &~ 3;
	kmsg->ikm_header.msgh_local_port = MACH_PORT_NULL;
	kmsg->ikm_header.msgh_kind = MACH_MSGH_KIND_NORMAL;
	kmsg->ikm_header.msgh_id = NET_RCV_MSG_ID;

	net_kmsg(kmsg)->header_type = header_type;
	net_kmsg(kmsg)->packet_type = packet_type;
	net_kmsg(kmsg)->net_rcv_msg_packet_count = count;
	return;
}

void
net_package_sg(kmsg)
	ipc_kmsg_t kmsg;
{
	int count;
	vm_map_copy_t copy;

	/*
	 * Fill in the rest of the kmsg.
	 */
	/*
	 *	There's been some repeated merge difficulties
	 *	with this line -- Bernie says that it should
	 *	*not* have a round_page() on the RHS of this
	 *	expression...
	 */
	count = net_kmsg_sg(kmsg)->net_rcv_msg_sg_packet_count;


	/*
	 * Remove unnecessary pages from the packet.
	 */
	copy = ((struct net_rcv_msg_sg_copy *)kmsg)->copy;
	if (kmem_io_object_trunc(copy, (vm_size_t)count) != KERN_SUCCESS)
		panic("net_package_sg: unable to truncate packet object");

	/* Must be set to actual size, not IKM_SIZE_NETWORK  */
	ikm_init_special(kmsg, kmsg->ikm_size); 

	kmsg->ikm_header.msgh_bits = MACH_MSGH_BITS_COMPLEX |
		MACH_MSGH_BITS(MACH_MSG_TYPE_PORT_SEND, 0);

	/* remember message sizes must be rounded up */
	kmsg->ikm_header.msgh_size =
		((mach_msg_size_t) (sizeof(struct net_rcv_msg_sg)));

	kmsg->ikm_header.msgh_local_port = MACH_PORT_NULL;
	kmsg->ikm_header.msgh_kind = MACH_MSGH_KIND_NORMAL;
	kmsg->ikm_header.msgh_id = NET_RCV_MSG_ID;

	net_kmsg_sg(kmsg)->data_type = data_type_long;
	net_kmsg_sg(kmsg)->net_rcv_msg_sg_packet_count = count;

	/*
	 * Free the scatter/gather list
	 */
	io_sglist_free((struct io_sglist *)(net_kmsg_sg(kmsg)->data));

	/*
	 * Pass copy object up
	 */
	net_kmsg_sg(kmsg)->data = (vm_offset_t) copy;
	return;
}

/*
 *	net_deliver:
 *
 *	Called and returns holding pool net_queue_lock, at splimp.
 *	Dequeues a message and delivers it at spl0.
 *	Returns FALSE if no messages.
 */
boolean_t net_deliver(p, nonblocking)
	register struct net_pool *p;
	boolean_t nonblocking;
{
	register ipc_kmsg_t kmsg;
	boolean_t high_priority;
	struct ipc_kmsg_queue send_list;
	register vm_offset_t addr;

	/*
	 * Pick up a pending network message and deliver it.
	 * Deliver high priority messages before low priority.
	 */

	if ((kmsg = ipc_kmsg_dequeue(&p->net_queue_high)) != IKM_NULL) {
	    p->net_queue_high_size--;
	    high_priority = TRUE;
	} else if ((kmsg = ipc_kmsg_dequeue(&p->net_queue_low)) != IKM_NULL) {
	    p->net_queue_low_size--;
	    high_priority = FALSE;
	} else
	    return FALSE;
	simple_unlock(&p->net_queue_lock);
	(void) spl0();

	/*
	 * Run the packet through the filters,
	 * getting back a queue of packets to send.
	 */
	switch (p->net_pool_type) {
	case NET_POOL_INLINE:
		net_filter_inline(p, kmsg, &send_list);
		break;
	case NET_POOL_SG:
		addr = p->net_pool_va;

		/*
		 * Map a vm range over the beginning of the packet so
		 * we can run the filter.
		 */
		pmap_enter(vm_map_pmap(kernel_map), addr,
			   ((struct io_sglist *)(net_kmsg_sg(kmsg)->data))->iosg_list[0].iosge_phys,
			   VM_PROT_READ, TRUE);

		/*
		 * Run the packet through the filters,
		 * getting back a queue of packets to send.
		 */
		net_filter_sg(p, kmsg, &send_list, (struct net_rcv_msg_sg_data *)addr);

		/*
		 * Remove mapping.
		 */
		pmap_remove(vm_map_pmap(kernel_map), addr, addr+PAGE_SIZE); 
		break;
	}

	if (!nonblocking) {
	    /*
	     * There is a danger of running out of available buffers
	     * because they all get moved into the high priority queue
	     * or a port queue.  In particular, we might need to
	     * allocate more buffers as we pull (previously available)
	     * buffers out of the low priority queue.  But we can only
	     * allocate if we are allowed to block.
	     */
	    net_kmsg_more((net_pool_t)p);
	}

	while ((kmsg = ipc_kmsg_dequeue(&send_list)) != IKM_NULL) {

		/*
		 * Package message
		 */

		switch (p->net_pool_type) {
		case NET_POOL_INLINE:
			net_package_inline(kmsg);
			break;
		case NET_POOL_SG:
			net_package_sg(kmsg);
			/*
			 * SG kmsg will be destroyed or delivered,
			 * so decrement the total.
			 */
			simple_lock(&p->net_kmsg_total_lock);
			p->net_kmsg_total--;
			simple_unlock(&p->net_kmsg_total_lock);
			break;
		}

		/*
		 * Send the packet to the destination port.  Drop it
		 * if the destination port is over its backlog.
		 */

		if (ipc_mqueue_send(kmsg, MACH_SEND_TIMEOUT, 0) ==
						    MACH_MSG_SUCCESS) {
			if (high_priority)
				p->net_kmsg_send_high_hits++;
			else
				p->net_kmsg_send_low_hits++;
			/* the receiver is responsible for the message now */
		} else {
			if (high_priority)
				p->net_kmsg_send_high_misses++;
			else
				p->net_kmsg_send_low_misses++;
			ipc_kmsg_destroy(kmsg);
		}
	}

	(void) splimp();
	simple_lock(&p->net_queue_lock);
	return TRUE;
}


/*
 * Incoming packet.  Header has already been moved to proper place.
 * We are already at splimp.
 */
void
net_packet_pool(pool, ifp, kmsg, count, priority)
	net_pool_t		pool;
	register struct ifnet	*ifp;
	register ipc_kmsg_t	kmsg;
	unsigned int		count;
	boolean_t		priority;
{
	register struct net_pool *p = (struct net_pool *)pool;

	kmsg->ikm_header.msgh_remote_port = (mach_port_t) ifp;
	switch (p->net_pool_type) {
	case NET_POOL_INLINE:
		net_kmsg(kmsg)->net_rcv_msg_packet_count = count;
		break;
	case NET_POOL_SG:
		net_kmsg_sg(kmsg)->net_rcv_msg_sg_packet_count = count;
		break;
	}

	simple_lock(&p->net_queue_lock);
	if (priority) {
	    ipc_kmsg_enqueue(&p->net_queue_high, kmsg);
	    if (++p->net_queue_high_size > p->net_queue_high_max)
		p->net_queue_high_max = p->net_queue_high_size;
	} else {
	    ipc_kmsg_enqueue(&p->net_queue_low, kmsg);
	    if (++p->net_queue_low_size > p->net_queue_low_max)
		p->net_queue_low_max = p->net_queue_low_size;
	}
	simple_unlock(&p->net_queue_lock);

	/*
	 *	If the network thread is awake, then we don't
	 *	need to take an AST, because the thread will
	 *	deliver the packet.
	 */
	if (!net_thread_awake) {
	    int s = splsched();
	    ast_on(cpu_number(), AST_NETWORK);
	    (void) splx(s);
	}
}

/*
 * Check filter for invalid operations or stack over/under-flow.
 */
boolean_t
parse_net_filter(filter, count)
	register filter_t	*filter;
	unsigned int		count;
{
	register int	sp;
	register filter_t	*fpe = &filter[count];
	register filter_t	op, arg;

	sp = NET_FILTER_STACK_DEPTH;

	for (; filter < fpe; filter++) {
	    op = NETF_OP(*filter);
	    arg = NETF_ARG(*filter);

	    switch (arg) {
		case NETF_NOPUSH:
		    break;
		case NETF_PUSHZERO:
		    sp--;
		    break;
		case NETF_PUSHLIT:
		    filter++;
		    if (filter >= fpe)
			return (FALSE);	/* literal value not in filter */
		    sp--;
		    break;
		case NETF_PUSHIND:
		case NETF_PUSHHDRIND:
		    break;
		default:
		    if (arg >= NETF_PUSHSTK) {
			if (arg - NETF_PUSHSTK + sp > NET_FILTER_STACK_DEPTH)
			    return FALSE;
		    }
		    else if (arg >= NETF_PUSHHDR) {
			if (arg - NETF_PUSHHDR >=
				NET_HDW_HDR_MAX/sizeof(unsigned short))
			    return FALSE;
		    }
		    /* else... cannot check for packet bounds
				without packet */
		    sp--;
		    break;
	    }
	    if (sp < 2) {
		return (FALSE);	/* stack overflow */
	    }
	    if (op == NETF_OP(NETF_NOP))
		continue;

	    /*
	     * all non-NOP operators are binary.
	     */
	    if (sp > NET_MAX_FILTER-2)
		return (FALSE);

	    sp++;
	    switch (op) {
		case NETF_OP(NETF_AND):
		case NETF_OP(NETF_OR):
		case NETF_OP(NETF_XOR):
		case NETF_OP(NETF_EQ):
		case NETF_OP(NETF_NEQ):
		case NETF_OP(NETF_LT):
		case NETF_OP(NETF_LE):
		case NETF_OP(NETF_GT):
		case NETF_OP(NETF_GE):
		case NETF_OP(NETF_COR):
		case NETF_OP(NETF_CAND):
		case NETF_OP(NETF_CNOR):
		case NETF_OP(NETF_CNAND):
		case NETF_OP(NETF_LSH):
		case NETF_OP(NETF_RSH):
		case NETF_OP(NETF_ADD):
		case NETF_OP(NETF_SUB):
		    break;
		default:
		    return (FALSE);
	    }
	}
	return (TRUE);
}

/*
 * Set a filter for a network interface.
 *
 * We are given a naked send right for the rcv_port.
 * If we are successful, we must consume that right.
 */
io_return_t
net_set_filter_pool(pool, ifp, rcv_port, priority, filter, filter_count)
	net_pool_t	pool;
	struct ifnet	*ifp;
	ipc_port_t	rcv_port;
	int		priority;
	filter_t	*filter;
	unsigned int	filter_count;
{
	register struct net_pool *p = (struct net_pool *)pool;
	boolean_t	found;
	register net_rcv_port_t	infp, new_infp;
	ipc_port_t      notify;
	kern_return_t   kr;


	/*
	 * Check the filter syntax.
	 */
	if (!parse_net_filter(filter, filter_count))
	    return (D_INVALID_OPERATION);

	/*
	 * Check for SG use in interface.
	 */
	if ((ifp->if_flags & IFF_SG) && (p->net_pool_type != NET_POOL_SG))
	    return (D_INVALID_OPERATION);

	/*
	 * Look for an existing filter on the same reply port.
	 * If there is one, replace it.
	 */
	new_infp = 0;
	simple_lock(&ifp->if_rcv_port_list_lock);

	while (TRUE) {
	    found = FALSE;
	    queue_iterate(&ifp->if_rcv_port_list, infp, net_rcv_port_t, chain)
	    {
	    	register int i,exists;
		/*
		 * For SG type interfaces check for an existing
		 * filter since there is no support for multiple
		 * receivers (ool messages). Will only find
		 * exact matches.
		 */
		if ((ifp->if_flags & IFF_SG)) {
		     /* check for same filter size */
		     if (filter_count == (infp->filter_end - &infp->filter[0])) {
		          /* check for active port */
			  if (io_active(&(infp->rcv_port)->ip_object)) {
			       /* check each filter element  */
			       exists = TRUE;
	    		       for (i = 0; i < filter_count; i++) {
			            if (infp->filter[i] != filter[i])
				         exists = FALSE;	
			       }
			       if (exists) {
	    		            simple_unlock(&ifp->if_rcv_port_list_lock);
	    			    return (D_INVALID_OPERATION);
		               }
			  }
		     }
		}

		if (infp->rcv_port == rcv_port) {
		    /*
		     * Remove the old filter from list, and re-use its
		     * data structure.
		     */
		    remqueue(&ifp->if_rcv_port_list, (queue_entry_t)infp);
		    found = TRUE;

		    /*
		     * We keep the existing reference for the receive
		     * port.
		     */
		    break;
		}
	    }
	    if (found)
		break;

	    if (new_infp) {
		/*
		 * No existing filter - use the new structure.
		 */
		infp = new_infp;
		new_infp = 0;
		break;
	    }

	    /*
	     * Must unlock to allocate a new filter.  If someone else
	     * has added a filter on the same port after we have
	     * allocated, release the new structure and indicate
	     * an error.
	     */
	    simple_unlock(&ifp->if_rcv_port_list_lock);

	    new_infp = (net_rcv_port_t)zalloc(net_rcv_zone);
	    new_infp->rcv_port = rcv_port;
	    new_infp->rcv_count = 0;

	    /*
	     * Re-lock and try the lookup again.
	     */
	    simple_lock(&ifp->if_rcv_port_list_lock);
	}

	/*
	 * Change filter.
	 */
	{
	    register int i;

	    infp->priority = priority;
	    for (i = 0; i < filter_count; i++)
		infp->filter[i] = filter[i];
	    infp->filter_end = &infp->filter[i];
	}

	/*
	 * Insert according to priority.
	 */
	{
	    register net_rcv_port_t	next;

	    queue_iterate(&ifp->if_rcv_port_list, next, net_rcv_port_t, chain)
		if (infp->priority > next->priority)
		    break;
	    enqueue_tail((queue_t)&next->chain, (queue_entry_t)infp);
	}

	if (found) {
	    /*
	     * We found the receive port, so we already have a reference.
	     * Deallocate the extra reference.
	     */
	    ipc_port_release_send(rcv_port);
	} else {
	    mach_port_msgcount_t qlimit = 0;

	    /*
	     * We allocated a new filter, so increase net_queue_free_min
	     * and net_kmsg_max to allow for more queued messages.
	     */

	    if (IP_VALID(rcv_port)) {
		ip_lock(rcv_port);
		if (ip_active(rcv_port))
		    qlimit = rcv_port->ip_qlimit;
		ip_unlock(rcv_port);
	    }

	    infp->rcv_qlimit = qlimit;

            /* only increase pool size if NOT a SG type devices */
	    if (!(ifp->if_flags & IFF_SG)) {
	    	simple_lock(&p->net_kmsg_total_lock);
	    	p->net_queue_free_min++;
	    	p->net_kmsg_max += qlimit + 1;
	    	simple_unlock(&p->net_kmsg_total_lock);
	    }
	}

	simple_unlock(&ifp->if_rcv_port_list_lock);

	if (new_infp) {
	    /*
	     * Allocated one filter structure too many.
	     */
	    zfree(net_rcv_zone, (vm_offset_t) new_infp);
	}

        /* Support for remote receivers.
	/* Request a dead name notification for all receive ports
         * added to filter list. Send notification to the
	 * master_device_port.    
         */

	notify = ipc_port_make_sonce(master_device_port);
	kr = ipc_port_dnrequest(rcv_port, rcv_port, notify, &notify);
        while (kr != KERN_SUCCESS) {
                kr = ipc_port_dngrow(rcv_port);
                if (kr != KERN_SUCCESS)
                    panic("net_set_filter_pool: ipc_port_dngrow failure");

		kr = ipc_port_dnrequest(rcv_port, rcv_port, notify, &notify);
	}
	return (D_SUCCESS);
}

/*
 * Other network operations
 */
io_return_t
net_getstat(ifp, flavor, status, count)
	struct ifnet	*ifp;
	int		flavor;
	dev_status_t	status;		/* pointer to OUT array */
	unsigned int	*count;		/* OUT */
{
	switch (flavor) {
	    case NET_STATUS:
	    {
		register struct net_status *ns = (struct net_status *)status;

		ns->min_packet_size = ifp->if_header_size;
		ns->max_packet_size = ifp->if_header_size + ifp->if_mtu;
		ns->header_format   = ifp->if_header_format;
		ns->header_size	    = ifp->if_header_size;
		ns->address_size    = ifp->if_address_size;
		ns->flags	    = ifp->if_flags;
		ns->mapped_size	    = 0;

		*count = NET_STATUS_COUNT;
		break;
	    }
	    case NET_ADDRESS:
	    {
		register int	addr_byte_count;
		register int	addr_long_count;
		register int	i;

		addr_byte_count = ifp->if_address_size;
		addr_long_count = (addr_byte_count + (sizeof(long)-1))
					 / sizeof(long);
		bcopy((char *)ifp->if_address,
		      (char *)status,
		      (unsigned) addr_byte_count);
		if (addr_byte_count < addr_long_count * sizeof(long))
		    bzero((char *)status + addr_byte_count,
			  (unsigned) (addr_long_count * sizeof(long)
				      - addr_byte_count));

		for (i = 0; i < addr_long_count; i++) {
		    register long word;

		    word = status[i];
		    status[i] = htonl(word);
		}
		*count = addr_long_count;
		break;
	    }
	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

io_return_t
net_write(ifp, start, ior)
	register struct ifnet *ifp;
	int		(*start)();
	io_req_t	ior;
{
	int	s;
	kern_return_t	rc;
	boolean_t	wait;

	/*
	 * Reject the write if the interface is down.
	 */
	if ((ifp->if_flags & (IFF_UP|IFF_RUNNING)) != (IFF_UP|IFF_RUNNING))
	    return (D_DEVICE_DOWN);

	/*
	 * Reject the write if the packet is too large or too small.
	 */
	if (ior->io_count < ifp->if_header_size ||
	    ior->io_count > ifp->if_header_size + ifp->if_mtu)
	    return (D_INVALID_SIZE);

	/*
	 * Wire down the memory.
	 */

	rc = device_write_get(ior, &wait);
	if (rc != KERN_SUCCESS)
	    return (rc);

	/*
	 *	Network interfaces can't cope with VM continuations.
	 *	If wait is set, just panic.
	 */
	if (wait) {
		panic("net_write: VM continuation");
	}

	/*
	 * Queue the packet on the output queue, and
	 * start the device.
	 */
	s = splimp();
	IF_ENQUEUE(&ifp->if_snd, ior);
	(*start)(ifp->if_unit);
	splx(s);

	/* 
	 * Wait for fully synchronous operations
	 */
	if (ior->io_op & IO_SYNC) {
		iowait(ior);
		return(D_SUCCESS);
	}

	return (D_IO_QUEUED);
}

/*
 * Create Pool - Create a buffer pool
 *
 * Possible errors
 *	KERN_RESOURCE_SHORTAGE - unable to allocate pool resources.
 */
kern_return_t
net_kmsg_create_pool(ptype, bufsize, minbufs, sharable, name, pool_result)
	net_pool_type_t ptype;	/* Pool type */
	vm_size_t bufsize;	/* Total buffer size (in bytes) */
	int minbufs;		/* # of buffers in pool (to start with) */
	boolean_t sharable;	/* Does caller want to share this pool */
	char *name;		/* Name - Must be static! */
	net_pool_t *pool_result; /* New Pool Handle (OUT arg) */
{
	register struct net_pool *p;
	boolean_t found;

	*pool_result = NET_POOL_NULL;

	switch (ptype) {
	case NET_POOL_INLINE:
	case NET_POOL_SG:
		break;

	default:
		return(KERN_INVALID_ARGUMENT);
	}

	p = (struct net_pool *)kalloc(sizeof(*p));
	if (p == (struct net_pool *)NET_POOL_NULL)
		return(KERN_RESOURCE_SHORTAGE);

	bzero(p, sizeof(*p));

	/*
	 * Fill in all the good stuff
	 */
	p->net_pool_type = ptype;
	p->net_pool_name = name;
	p->net_pool_sharable = sharable;
	p->net_queue_free_min = minbufs;
	p->net_kmsg_max = minbufs;

	switch (ptype) {
	case NET_POOL_INLINE:
		/*
		 * INLINE kmsg size is kmsg wrapped around packet
		 */
		p->net_kmsg_size = round_page(ikm_plus_overhead(bufsize));
		break;

	case NET_POOL_SG:
		/*
		 * SG kmsg size is really packet size.
		 */
		p->net_kmsg_size = round_page(bufsize);
		/*
		 * For SG pools, allocate va space for packet filter
		 */
		if (vm_allocate(kernel_map, &p->net_pool_va, PAGE_SIZE, TRUE) != KERN_SUCCESS)
			panic("net_kmsg_create_pool: unable to allocate SG packet filter VA");
	}

	p->net_pool_refcount = 1;

	simple_lock_init(&p->net_queue_free_lock);
	simple_lock_init(&p->net_queue_lock);
	simple_lock_init(&p->net_kmsg_total_lock);

	ipc_kmsg_queue_init(&p->net_queue_free);
	ipc_kmsg_queue_init(&p->net_queue_high);
	ipc_kmsg_queue_init(&p->net_queue_low);

	/*
	 * Add to the list of pools.
	 * We only add to the front of the list so
	 * we can be deliver packets at the same time
	 * as pool additions.
	 */
	simple_lock(&net_pool_lock);
	p->net_pool_next = net_pool_list;
	net_pool_list = p;
	simple_unlock(&net_pool_lock);

	*pool_result = (net_pool_t)p;
	return(KERN_SUCCESS);
}

/*
 * Grow Pool - Add buffers to pool free list
 */
kern_return_t
net_kmsg_grow_pool(pool, nbufs, newmax)
	net_pool_t pool;		/* Pool Handle */
	int nbufs;			/* # of buffers to add to pool */
	int *newmax;			/* # of buffers in pool (OUT arg) */
{
	register struct net_pool *p = (struct net_pool *)pool;

	simple_lock(&p->net_kmsg_total_lock);
	p->net_kmsg_max += nbufs;	
	*newmax = p->net_kmsg_max;
	simple_unlock(&p->net_kmsg_total_lock);
	simple_lock(&p->net_queue_free_lock);
	p->net_queue_free_min += nbufs;
	simple_unlock(&p->net_queue_free_lock);
	return(KERN_SUCCESS);
}

/*
 * Shrink Pool - Removed buffers from pool free list
 *
 * Possible errors
 *	KERN_INVALID_ARGUMENT - removal exceeds pool total
 */
kern_return_t
net_kmsg_shrink_pool(pool, nbufs, newmax)
	net_pool_t pool;		/* Pool Handle */
	int nbufs;			/* # of buffers to remove from pool */
	int *newmax;			/* # of buffers in pool (OUT arg) */
{
	register struct net_pool *p = (struct net_pool *)pool;
	register kern_return_t result;
	int count;

	result = KERN_SUCCESS;
	simple_lock(&p->net_kmsg_total_lock);
	count = p->net_kmsg_max - nbufs;
	if (count < 0) {
		simple_unlock(&p->net_kmsg_total_lock);
		return(KERN_INVALID_ARGUMENT);
	}
	p->net_kmsg_max = count;
	*newmax = p->net_kmsg_max;
	simple_unlock(&p->net_kmsg_total_lock);

	simple_lock(&p->net_queue_free_lock);
	p->net_queue_free_min = count;
	simple_unlock(&p->net_queue_free_lock);
	net_kmsg_collect_pool(pool);
	
	return(KERN_SUCCESS);
}

/*
 * Collect Pool - Collect free queue pages from a net pool.
 */

void net_kmsg_collect_pool(pool)
	net_pool_t pool;		/* Pool Handle */
{
	register struct net_pool *p = (struct net_pool *)pool;
	register ipc_kmsg_t kmsg;
	int s;

	s = splimp();
	simple_lock(&p->net_queue_free_lock);
	while (p->net_queue_free_size > p->net_queue_free_min) {
		kmsg = ipc_kmsg_dequeue(&p->net_queue_free);
		p->net_queue_free_size--;
		simple_unlock(&p->net_queue_free_lock);
		(void) splx(s);
		net_kmsg_free_buf(p, kmsg);
		s = splimp();
		simple_lock(&p->net_queue_free_lock);
	}
	simple_unlock(&p->net_queue_free_lock);
	(void) splx(s);
}

/*
 * Allocate/Free (possibly blocking)
 */

/*
 * Allocate Buffer - Allocate buffer for a particular pool.
 *
 * Possible errors
 *	IKM_NULL - no more resources
 */
ipc_kmsg_t
net_kmsg_alloc_buf(pool)
	net_pool_t pool;		/* Pool Handle */
{
	register struct net_pool *p = (struct net_pool *)pool;
        register struct io_sglist *sglist;
        register ipc_kmsg_t kmsg;
        vm_map_copy_t copy;
        kern_return_t ret;
        vm_size_t size;
        register int ii;
        int nentries;

	/*
	 * Allocate and return a new one if we are allowed to.
	 */
	if (p->net_kmsg_total >= p->net_kmsg_max) {
		return(IKM_NULL);
	}

	/*
	 * Ok to allocate.
	 */
	switch (p->net_pool_type) {
	case NET_POOL_INLINE:
		kmsg = (ipc_kmsg_t) kalloc(p->net_kmsg_size);
		if (kmsg == IKM_NULL)
			return(IKM_NULL);
		break;

	case NET_POOL_SG:
		kmsg = (ipc_kmsg_t) kalloc(ikm_plus_overhead(sizeof(struct net_rcv_msg_sg_copy)));
		if (kmsg == IKM_NULL)
			return(IKM_NULL);

		kmsg->ikm_size = ikm_plus_overhead(sizeof(struct net_rcv_msg_sg_copy));
		size = p->net_kmsg_size;
		nentries = size/PAGE_SIZE;

		io_sglist_alloc(sglist, nentries);
		if (!sglist) {
			kfree(kmsg, kmsg->ikm_size);
			return(IKM_NULL);
		}
		net_kmsg_sg(kmsg)->data = (vm_offset_t)sglist;
		sglist->iosg_hdr.length = size;

		if ((ret = kmem_io_sglist_object_alloc(sglist, &copy)) !=  KERN_SUCCESS) {
			kfree(kmsg, kmsg->ikm_size);
			io_sglist_free(sglist);
			return(IKM_NULL);
		}

		/* store copy object away for message queueing */
		((struct net_rcv_msg_sg_copy *)kmsg)->copy = copy;
		break;
       	}

	/* increment the total */
	simple_lock(&p->net_kmsg_total_lock);
	p->net_kmsg_total++;
	simple_unlock(&p->net_kmsg_total_lock);
	return(kmsg);
}

/*
 * Free Buffer - Free buffer from a particular pool.
 */
kern_return_t
net_kmsg_free_buf(pool, kmsg)
	net_pool_t pool;		/* Pool Handle */
	ipc_kmsg_t kmsg;
{
	register struct net_pool *p = (struct net_pool *)pool;
        struct io_sglist *sglist; 

	if (kmsg == IKM_NULL) {
		panic("net_kmsg_free_buf: NULL kmsg");
		return;
	}

	switch (p->net_pool_type) {
	case NET_POOL_INLINE:
		kfree((vm_offset_t) (kmsg), p->net_kmsg_size);
		break;

	case NET_POOL_SG:
		/*
		 * Free the entries, the sglist and the sg format kmsg.
		 */
		sglist = (struct io_sglist *)net_kmsg_sg(kmsg)->data;
		kmem_io_object_deallocate(((struct net_rcv_msg_sg_copy *)kmsg)->copy);
		io_sglist_free(sglist);
		kfree((vm_offset_t) kmsg, kmsg->ikm_size);
		break;
	}

	/* decrement the total */
	simple_lock(&p->net_kmsg_total_lock);
	p->net_kmsg_total--;
	simple_unlock(&p->net_kmsg_total_lock);

	return(KERN_SUCCESS);
}


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
net_kmsg_get_buf(pool)
	net_pool_t pool;		/* Pool Handle */
{
	register struct net_pool *p = (struct net_pool *)pool;
	register ipc_kmsg_t kmsg;
	int s;

	/*
	 *	First check the list of free buffers.
	 */
	s = splimp();
	simple_lock(&p->net_queue_free_lock);
	kmsg = ipc_kmsg_queue_first(&p->net_queue_free);
	if (kmsg != IKM_NULL) {
	    ipc_kmsg_rmqueue_first_macro(&p->net_queue_free, kmsg);
	    p->net_queue_free_size--;
	    p->net_queue_free_hits++;
	}
	simple_unlock(&p->net_queue_free_lock);

	if (kmsg == IKM_NULL) {
	    /*
	     *	Try to steal from the low priority queue.
	     */
	    simple_lock(&p->net_queue_lock);
	    kmsg = ipc_kmsg_queue_first(&p->net_queue_low);
	    if (kmsg != IKM_NULL) {
		ipc_kmsg_rmqueue_first_macro(&p->net_queue_low, kmsg);
		p->net_queue_low_size--;
		p->net_queue_free_steals++;
	    }
	    simple_unlock(&p->net_queue_lock);
	}

	if (kmsg == IKM_NULL)
	    p->net_queue_free_misses++;
	(void) splx(s);

	if (net_kmsg_want_more(p) || (kmsg == IKM_NULL)) {
	    boolean_t awake;

	    s = splimp();
	    simple_lock(&net_thread_lock);
	    awake = net_thread_awake;
	    net_thread_awake = TRUE;
	    simple_unlock(&net_thread_lock);
	    (void) splx(s);

	    if (!awake)
		thread_wakeup((int) &net_thread_awake);
	}

	return kmsg;
}

/*
 * Put Buffer - Put buffer back on pool free list
 */
void
net_kmsg_put_buf(pool, kmsg)
	net_pool_t pool;		/* Pool Handle */
	ipc_kmsg_t kmsg;
{
	register struct net_pool *p = (struct net_pool *)pool;
	int s;

	if (kmsg == IKM_NULL)
		return;

	s = splimp();
	simple_lock(&p->net_queue_free_lock);
	ipc_kmsg_enqueue_macro(&p->net_queue_free, kmsg);
	if (++p->net_queue_free_size > p->net_queue_free_max)
	    p->net_queue_free_max = p->net_queue_free_size;
	simple_unlock(&p->net_queue_free_lock);
	(void) splx(s);
	return;
}

/*
 * COMPATIBILITY code.
 */
ipc_kmsg_t
net_kmsg_get()
{
	return(net_kmsg_get_buf(inline_pagepool));
}

void
net_kmsg_put(kmsg)
	ipc_kmsg_t	kmsg;
{
	net_kmsg_put_buf(inline_pagepool, kmsg);
}

ipc_kmsg_t
net_kmsg_alloc()
{
	return(net_kmsg_alloc_buf(inline_pagepool));
}

void
net_kmsg_free(kmsg)
	ipc_kmsg_t kmsg;
{
	net_kmsg_free_buf(inline_pagepool, kmsg);
}

/*
 * Set a filter for a network interface.
 *
 * We are given a naked send right for the rcv_port.
 * If we are successful, we must consume that right.
 */
io_return_t
net_set_filter(ifp, rcv_port, priority, filter, filter_count)
	struct ifnet	*ifp;
	ipc_port_t	rcv_port;
	int		priority;
	filter_t	*filter;
	unsigned int	filter_count;
{
	return(net_set_filter_pool(inline_pagepool, ifp, rcv_port,
			priority, filter, filter_count));
}

/*
 * Incoming packet.  Header has already been moved to proper place.
 * We are already at splimp.
 */
void
net_packet(ifp, kmsg, count, priority)
	register struct ifnet	*ifp;
	register ipc_kmsg_t	kmsg;
	unsigned int		count;
	boolean_t		priority;
{
#if	NORMA_ETHER
	if (netipc_net_packet(kmsg, count)) {
		return;
	}
#endif	NORMA_ETHER
	net_packet_pool(inline_pagepool, ifp, kmsg, count, priority);
}


/*
 *	We want to deliver packets using ASTs, so we can avoid the
 *	thread_wakeup/thread_block needed to get to the network
 *	thread.  However, we can't allocate memory in the AST handler,
 *	because memory allocation might block.  Hence we have the
 *	network thread to allocate memory.  The network thread also
 *	delivers packets, so it can be allocating and delivering for a
 *	burst.  net_thread_awake is protected by net_queue_lock
 *	(instead of net_queue_free_lock) so that net_packet and
 *	net_ast can safely determine if the network thread is running.
 *	This prevents a race that might leave a packet sitting without
 *	being delivered.  It is possible for net_kmsg_get to think
 *	the network thread is awake, and so avoid a wakeup, and then
 *	have the network thread sleep without allocating.  The next
 *	net_kmsg_get will do a wakeup.
 */

void net_ast()
{
	int s;
	register struct net_pool *listp, *p;
        boolean_t awake = TRUE;

	net_ast_taken++;

        /*
         * Get current list start.
         */

	simple_lock(&net_pool_lock);
	listp = net_pool_list;
	simple_unlock(&net_pool_lock);

        if (current_thread()==current_processor()->idle_thread) {
            /*
             * We cannot call net_deliver from the idle thread
             * since net_deliver can block in ipc_mqueue_send
             */
            s = splimp();
            simple_lock(&net_thread_lock);
            awake = net_thread_awake;
            net_thread_awake = TRUE;
            simple_unlock(&net_thread_lock);
            goto out;
        }

	/*
	 *	If the network thread is awake, then we would
	 *	rather deliver messages from it, because
	 *	it can also allocate memory.
	 */

	s = splimp();
	for (p = listp; p != NET_POOL_NULL; p = p->net_pool_next) {
		/*
		 * Start delivering.
		 * If we find the network thread awake,
		 * bail out and let it deliver the packets
		 */
		simple_lock(&p->net_queue_lock);
		while (!net_thread_awake) {
			if (!net_deliver(p, TRUE))
				break;
		}
		simple_unlock(&p->net_queue_lock);
		if (net_thread_awake)
			break;
	}

	/*
	 *	Prevent an unnecessary AST.  Either the network
	 *	thread will deliver the messages, or there are
	 *	no messages left to deliver.
	 */

out:
	(void) splsched();
	ast_off(cpu_number(), AST_NETWORK);
	(void) splx(s);
        if (!awake)
            thread_wakeup((int) &net_thread_awake);

}

void net_thread_continue()
{
	for (;;) {
		int s;
		register struct net_pool *listp, *p;

		net_thread_awaken++;

		/*
	 	 * Get current list start.
	 	 */
		simple_lock(&net_pool_lock);
                listp = net_pool_list;
                simple_unlock(&net_pool_lock);

		for (p = listp; p != NET_POOL_NULL; p = p->net_pool_next) {
			/*
			 *	First get more buffers.
			 */
			net_kmsg_more(p);
		}

		s = splimp();
		for (p = listp; p != NET_POOL_NULL; p = p->net_pool_next) {
			simple_lock(&p->net_queue_lock);
			while (net_deliver(p, FALSE))
				continue;
			simple_unlock(&p->net_queue_lock);
		}

		simple_lock(&net_thread_lock);
		net_thread_awake = FALSE;
		assert_wait((int) &net_thread_awake, FALSE);
		simple_unlock(&net_thread_lock);
		(void) splx(s);

		counter(c_net_thread_block++);
		thread_block(net_thread_continue);
	}
}

void net_thread()
{
	int s;

	/*
	 *	We should be very high priority.
	 */

	thread_set_own_priority(0);

	/*
	 *	We sleep initially, so that we don't allocate any buffers
	 *	unless the network is really in use and they are needed.
	 */

	s = splimp();
	simple_lock(&net_thread_lock);
	net_thread_awake = FALSE;
	assert_wait((int) &net_thread_awake, FALSE);
	simple_unlock(&net_thread_lock);
	(void) splx(s);
	counter(c_net_thread_block++);
	thread_block(net_thread_continue);
	net_thread_continue();
	/*NOTREACHED*/
}

/*
 * Initialize the whole package.
 */
void
net_io_init()
{
	register vm_size_t	size;

	size = sizeof(struct net_rcv_port);
	net_rcv_zone = zinit(size,
			     size * 1000,
			     PAGE_SIZE,
			     FALSE,
			     "net_rcv_port");

	simple_lock_init(&net_pool_lock);
	simple_lock_init(&net_thread_lock);

	/*
	 * Create inline pool for existing drivers not using
	 * new interface.
	 */
	if (net_kmsg_create_pool(
		 NET_POOL_INLINE,
		 sizeof(struct net_rcv_msg),
		 net_inline_free_min, FALSE, "inline-pagepool",
		 &inline_pagepool) != KERN_SUCCESS) {
		panic("net_io_init: unable to create inline pool");
	}
	net_kmsg_size = round_page(ikm_plus_overhead(sizeof(struct net_rcv_msg)));
}

#if	MACH_KDB
/*
 * Print all net_recv_port for a device
 */
void
net_recv_port_print(ifp)
	struct ifnet 	*ifp;
{
	register net_rcv_port_t infp,nextfp;
	int i;
	extern	void 	ipc_port_print();

	printf("net_rcv_port for network device, ifp =  0x%x\n",ifp);
	for (infp = (net_rcv_port_t) queue_first(&ifp->if_rcv_port_list);
	     !queue_end(&ifp->if_rcv_port_list, (queue_entry_t)infp);
	     infp = nextfp) {
	    	nextfp = (net_rcv_port_t) queue_next(&infp->chain);
		printf("infp->rcv_port   0x%x\n",infp->rcv_port);
		ipc_port_print(infp->rcv_port);
		printf("infp->rcv_qlimit   %d\n",infp->rcv_qlimit);
		printf("infp->rcv_count    %d\n",infp->rcv_count);
		printf("infp->priority     %d\n",infp->priority);
		printf("infp->filter_end 0x%x\n",infp->filter_end);
		printf("infp->filter[]   0x%x\n",&infp->filter[0]);

		for (i=0; &infp->filter[i] != infp->filter_end; i++) 
			printf(" %04x",infp->filter[i]); 	
		printf("\n");
	}		
}

/*
 * Print statistics of all net_pools
 */
void
net_pool_print()
{
	register struct net_pool *p;

     	for (p = net_pool_list; p != NET_POOL_NULL; p = p->net_pool_next) {
		printf("NET_POOL STATISTICS:\n");
		printf("net_pool_name = %s\n",p->net_pool_name);
		printf("Pool Buffer total and limits:\n");
        	printf(" net_kmsg_size=%d net_kmsg_total=%d net_msg_max=%d\n",
			p->net_kmsg_size,p->net_kmsg_total,p->net_kmsg_max);
		printf("Kmsgs queued to be sent to user:\n");
		printf(" net_queue_high_size=%d net_queue_high_max=%d\n",
			p->net_queue_high_size,p->net_queue_high_max);
		printf(" net_queue_low_size=%d  net_queue_low_max=%d\n",
			p->net_queue_low_size,p->net_queue_low_max);
		printf("Receive port backlog:\n");
		printf(" net_kmsg_send_high_hits=%d net_kmsg_send_high_misses=%d\n",
			p->net_kmsg_send_high_hits,p->net_kmsg_send_high_misses);
		printf(" net_kmsg_send_low_hits=%d net_kmsg_send_low_misses=%d\n",
			p->net_kmsg_send_low_hits,p->net_kmsg_send_low_misses);
		printf("Kmsg free list Statistics:\n");
		printf(" net_queue_free_size=%d \n",p->net_queue_free_size);
		printf(" net_queue_free_min=%d net_queue_free_max=%d\n",
			p->net_queue_free_min,p->net_queue_free_max);
		printf(" net_queue_free_hits=%d net_queue_free_misses=%d\n",
			p->net_queue_free_hits,p->net_queue_free_misses);
		printf(" net_queue_free_steals=%d\n",p->net_queue_free_steals);
     	}
}

#endif	/* MACH_KDB */
