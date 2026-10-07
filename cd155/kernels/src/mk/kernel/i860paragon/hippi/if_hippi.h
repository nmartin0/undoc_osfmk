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
 * Copyright 1992 by Intel Corporation,
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
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/if_hippi.h,v 1.10 1994/11/29 18:02:08 arlin Exp $
 *
 * $Log: if_hippi.h,v $
 * Revision 1.10  1994/11/29  18:02:08  arlin
 *   Raw hippi DST filtering with ulp and ports fixed.
 *   Also include assert.h for proper MACH_ASSERT checking.
 *
 *   Reviewer: Bernie Keany
 *   Risk: low
 *   Benefit or PTS #: 11573
 *   Testing: Test Case's port.c noport.c passed. Used kernel debugging tools
 *              to ensure that the correct filters are being set and reset.
 *   Module(s): hctlr.c, hippi.c, if_hippi.h, hctlr.h, rhippi.h
 *
 * Revision 1.9  1994/11/18  20:43:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/10/17  15:46:31  arlin
 * driver can drop good DST packets.
 * added flow control.
 *
 *  Reviewer: Jerrie Coffman
 *  Risk: medium
 *  Benefit or PTS #: 10759
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *   Also developed special application (hippi_test.c)
 *   that forced flow control condition on both large
 *   packets.
 *  Module(s): hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 * 	    if_hippi.h, hippi_status.h
 *
 * Revision 1.7  1994/07/08  21:39:29  arlin
 * ifdef for MACH_ASSERTS wrong, s/be MACH_ASSERT
 *
 * Revision 1.6  1994/06/08  17:00:22  arlin
 * Updated for R1.3 IPI-3 support
 *
 */
/*
 *      File:   if_hippi.h
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      HiPPI-LE Network Device Driver Definitions.
 */
 
#include <mach_assert.h>
/* testing and debug */
#if MACH_ASSERT
#define   IFHIP_DEBUG  
#endif MACH_ASSERT

#define   IFHIP_STATS

/* HiPPI Network Driver Specific - ifhip  */

/*
 *  Macros
 */

#ifdef IFHIP_STATS
#define	IFSTATS(p, f)	p->stats.f
#else
#define	IFSTATS(p, f)
#endif IFHIP_STATS

#ifdef IFHIP_DEBUG		/* DEBUG */
#ifndef BIT
#define	BIT(x)	(1 << x)
#endif  BIT
#define	IFLOG_ENTER	BIT(0)
#define	IFLOG_HIGH	BIT(1)
#define	IFLOG_MEDIUM	BIT(2)
#define	IFLOG_LOW	BIT(3)

#define	IFLOG(lev, args) \
	if((iflog_level & lev) && (iflog_id & _this_id)) printf args

#define IFLOG_ENTRY(id, args)     \
	long  _this_id = BIT(id);  \
	IFLOG(IFLOG_ENTER, args)

#else				/* else NO DEBUG */

#define	IFLOG_ENTRY(i, a)
#define	IFLOG(l, a)

#endif				/* endif DEBUG */

/* IFHIP Driver States */
#define	IFHIP_UNINITIALIZED	0
#define	IFHIP_PROBED		1
#define	IFHIP_ATTACHED		2
#define	IFHIP_OPEN		4
#define	IFHIP_HW_ERROR		-1
#define	IFHIP_SW_ERROR		-2

/* default HiPPI_LE MTU size for receive buffers */
#define IFHIP_IP_MTU		((64*1024)-1)		 
#define IFHIP_SREQ_QCNT		20 

/* Transmit Queue Structure
 *   The network driver uses a linked list of data structures
 *   in GP node memory to describe the data to be transfered.
 */
typedef struct ifhip_src {
  hctlr_src_t		   hctlr_req;	 /* controller interface request structure */ 
  struct ifhip_src         *next_req;    /* ptr to next ifhip transmit descriptor  */
} ifhip_src_t;

typedef struct ifhip_softc{
	struct 	ifnet	ds_if;  	/* generic interface header */
	u_char		ds_addr[6]; 	/* ULA hardware address */
	i960_scb_t	*scb;		/* phys ptr to the controller SCB */
	caddr_t		hippi_base;	/* phys addr of the controller */
	ifhip_src_t	*begin_sreq;	/* start of xmit request queue */
	ifhip_src_t	*sreq_qhead;	/* head pointer of xmit request queue */
	ifhip_src_t	*sreq_qtail;	/* tail pointer of xmit request queue */
	hctlr_filter_t	le_filter;	/* HiPPI-LE filter for DST channel */
	short		state;		/* state of driver */
	int		node_id;	/* node number for instance of hippi */
	int		timer;		/* timer for watchdog */
	int		pool_size;	/* current buffer pool size */
	ipc_kmsg_t 	kmsg;		/* kmsg in process for DST channel */
#ifdef IFHIP_STATS
	ifhip_stats_t	stats;		/* driver statistics */
#endif      /* IFHIP_STATS */
} ifhip_softc_t;

