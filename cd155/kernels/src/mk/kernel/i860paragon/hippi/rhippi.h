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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/rhippi.h,v 1.7 1995/03/02 23:33:09 arlin Exp $ 
 * $Log: rhippi.h,v $
 * Revision 1.7  1995/03/02  23:33:09  arlin
 *  Added Multiple Packet per Connection support
 *  and CONTinuation support for multiple I/O
 *  per packet and connection. F/W is R1.4
 *
 *  Reviewer: Jerrie Coffman, Bernie Keany
 *  Risk: medium
 *  Benefit or PTS #: 12411
 *  Testing: HiPPI EATs: Raw, TCP/IP, and IPI-3.
 *     Also developed special applications to test
 *     new MPC and CONT modes.
 *  Module(s):
 *     /i860paragon/hippi/
 *      hctlr.c, hctlr.h, rhippi.h, rhippi.c,
 *      hippi_status.h, hdc.c
 *     /ipi/ipi_misc.c ipi_defs.h, ipi.c,
 *     /device/ds_routines.c
 *
 * Revision 1.6  1994/11/29  18:04:11  arlin
 *    Raw hippi DST filtering with ulp and ports fixed.
 *    Also include assert.h for proper MACH_ASSERT checking.
 *
 *    Reviewer: Bernie Keany
 *    Risk: low
 *    Benefit or PTS #: 11573
 *    Testing: Test Case's port.c noport.c passed. Used kernel debugging tools
 *               to ensure that the correct filters are being set and reset.
 *    Module(s): hctlr.c, hippi.c, if_hippi.h, hctlr.h, rhippi.h
 *
 * Revision 1.5  1994/11/18  20:43:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/17  15:47:00  arlin
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
 * Revision 1.3  1994/07/08  21:45:03  arlin
 * missing header and revision comments
 *
 *
 */
/*
 *      File:   rhippi.h
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Raw HiPPI Device Driver Definitions.
 *
 */

#include <mach_assert.h>
/* testing and debug */
#if MACH_ASSERT
#define   RHIPPI_DEBUG  
#endif MACH_ASSERT

#define   RHIPPI_STATS  

#ifdef RHIPPI_DEBUG		/* DEBUG */
#ifndef BIT
#define BIT(x)  (1 << x)
#endif  BIT
#define	RHLOG_ENTER	BIT(0)
#define	RHLOG_HIGH	BIT(1)
#define	RHLOG_MEDIUM	BIT(2)
#define	RHLOG_LOW	BIT(3)
/* macros */
#define	RHLOG(lev, args) \
	if((rhlog_level & lev) && (rhlog_id & _this_id)) printf args
#define RHLOG_ENTRY(id, args)     \
	long  _this_id = BIT(id);  \
	RHLOG(RHLOG_ENTER, args)
#else				/* else NO RHIPPI_DEBUG */
#define	RHLOG_ENTRY(i, a)
#define	RHLOG(l, a)
#endif				/* endif RHIPPI_DEBUG */

#ifdef RHIPPI_STATS
#define RHSTATS(p, f)   p->stats.f
#else                           /* else NO RHIPPI_STATS */
#define RHSTATS(p, f)
#endif                          /* endif RHIPPI_STATS */

/* RAW HIPPI Driver States */
#define	RHIPPI_UNINITIALIZED	0
#define	RHIPPI_PROBED		0x0001
#define	RHIPPI_ATTACHED		0x0002
#define	RHIPPI_UP		0x0004
#define	RHIPPI_DOWN		0x0008
#define	RHIPPI_OPEN		0x0010
#define	RHIPPI_CLOSE		0x0020	
#define	RHIPPI_DST_PENDING      0x0040
#define	RHIPPI_DST_ABORTED      0x0080
#define	RHIPPI_PKT_PENDING      0x0100
#define	RHIPPI_EXCL             0x0200
#define	RHIPPI_PROM             0x0400
#define	RHIPPI_MPPC             0x0800
#define	RHIPPI_CONT             0x1000
#define	RHIPPI_HW_ERROR		-1
#define	RHIPPI_SW_ERROR		-2

/* default driver queue sizes */
#define RHIPPI_SRC_QSIZE	64
#define RHIPPI_FILTER_QSIZE	256
/* 1 second tick for read requests */
#define RHIPPI_READ_POLL        (1 * hz)

/* Write (SRC) Queue Structure
 *   The raw hippi driver uses a linked list of data structures
 *   in GP node memory to describe the data to be transfered.
 */
typedef struct rhippi_src {
  hctlr_src_t              hreq;    	/* controller interface request structure */
  struct rhippi_src        *next_req;   /* ptr to next write request descriptor   */
} rhippi_src_t;

/* Packet (Dst) Pending Queue Structure
 *   The raw hippi driver uses a linked list of data structures
 *   in GP node memory to describe the small DST packets waiting 
 *   for read requests. 
 */
typedef struct rhippi_dst {
  queue_head_t 	        links;		/* link for chaining		*/
  u_char		*fb_ptr;	/* ptr to packet data		*/
  u_int			xfer_len;	/* xfer lenght of this DST pkt	*/
  u_char		ulp;		/* ulp data for filter		*/
  u_short		filter_data;	/* port data for filter		*/
  u_int			ifield;		/* ifield from remote SRC   	*/
  boolean_t		busy;		/* command in use flag		*/
} rhippi_dst_t;

/* DST FILTER Queue Structure
 *   The raw hippi driver uses a linked list of data structures
 *   in GP node memory to describe the filter data.
 */
typedef struct rhippi_filter {
  hctlr_filter_t	hfilter;	/* controller interface filter structure    */
  u_int    	 	free;		/* filter state, TRUE=free, FALSE=used      */	
  ipc_port_t		port;		/* port reference from calling application  */	
  struct rhippi_filter	*next;		/* ptr to next filter structure             */
} rhippi_filter_t;

/* struct device read ior->recnum info */
typedef struct rh_data {
        u_char  ulp;
        u_char  timeout;
        u_short data;
} rh_data_t;

typedef struct {             
        queue_head_t  rhsrc_q;         /* write request overflow chain   */
        decl_simple_lock_data(,rhsrc_lock) /* lock for write requests    */
        queue_head_t  rhdst_q;         /* outstanding read request chain */
        decl_simple_lock_data(,rhdst_lock) /* lock for read requests     */
        queue_head_t  rhpkt_q;        /* pending DST packet chain        */
        decl_simple_lock_data(,rhpkt_lock) /* lock for read requests     */
        rhippi_src_t  *src_qstart;    /* write command memory            */
        rhippi_src_t  *src_qhead;     /* write command head pointer      */
        rhippi_src_t  *src_qtail;     /* write command tail pointer      */
        rhippi_dst_t  *dst_qstart;    /* DST data pending memory         */
        u_char        *dst_fbstart;   /* DST data buffers, (1056 each)   */
        rhippi_filter_t *rh_filters;  /* filter command memory           */
        int           state;          /* state of driver                 */
        int           hctlr_state;    /* state of channel interface      */
        int           watchdog;       /* state watchdog                  */
        int           wr_cnt;         /* pending write request count     */
        int           rd_cnt;         /* pending read  request count     */
        int           filter_cnt;     /* pending read request count      */
        int           max_flt_cnt;    /* maximum DST filter count        */
        int           node_id;        /* node id for console messages    */
	io_req_t      dst_ior;        /* ior reference, intr in process  */
	hctlr_dst_t   *dst_req;       /* hctlr req reference,in process  */
        u_int         excl_key;       /* ULP key, exclusive use of hippi */
        device_t      io_device;      /* pointer to open-device structure */
        int           first_write;    /* first write, ifield provided    */
#ifdef RHIPPI_STATS
        rhippi_stats_t    stats;      /* driver statistics and timers    */
#endif		/*  RHIPPI_STATS */
} rhippi_softc_t;

