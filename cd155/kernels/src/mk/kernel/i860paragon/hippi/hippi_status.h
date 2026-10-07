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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/hippi_status.h,v 1.4 1995/03/02 23:32:48 arlin Exp $
 * $Log: hippi_status.h,v $
 * Revision 1.4  1995/03/02  23:32:48  arlin
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
 * Revision 1.3  1994/11/18  20:43:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/10/17  15:46:28  arlin
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
 * Revision 1.1  1994/06/08  16:58:48  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *      File:   hippi_status.h
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Interface Definitions for the HiPPI controller.
 *	These definitions need to be exported to /usr/include/device
 *	for user applications using raw HiPPI library interface.
 */

#ifndef _HIPPI_STATUS_H_
#define _HIPPI_STATUS_H_

/* ioctl flavors for rhippi driver */
#define RHIPPI_FILTER           (('r'<<16) + 1)
#define RHIPPI_STATUS           (('r'<<16) + 2)

/* ioctl flavors for ifhip driver */
#define IFHIP_STATUS            (('h'<<16) + 2)
#define IFHIP_SET_POOL          (('h'<<16) + 4)

/* ioctl flavors for hctlr interface support, via rhippi */
#define HIPPI_STATUS            (('h'<<16) + 1)
#define HIPPI_DEV_EXCL          (('h'<<16) + 5) /* exclusive use of interface    */
#define HIPPI_DEV_MPPC          (('h'<<16) + 6) /* multiple pckts per connection */
#define HIPPI_DEV_CONT          (('h'<<16) + 7) /* continuation; don't drop pkt  */
#define HIPPI_SRC_TIMEOUT       (('h'<<16) + 8) /* set SRC channel timeout       */
#define HIPPI_DST_TIMEOUT       (('h'<<16) + 9) /* set new DST channel timeout   */
#define HIPPI_DEV_CONT_STS      (('h'<<16) + 10)/* monitor state of CONT mode    */

/* ioctl flavors for ctlr monitor, via rhippi, FUTURE */
#define HIPPI_MON_EXEC          (('m'<<16) + 1)
#define HIPPI_MON_WRITE         (('m'<<16) + 2)
#define HIPPI_MON_READ          (('m'<<16) + 3)
#define HIPPI_MON_BREAK         (('m'<<16) + 4)

/* errors returned with raw hippi_write and hippi_read library calls */
/* set by the i960 on the controller */
#define	SRC_ERR_INTC	0x00000001      /* interconnect de-asserted              */
#define	SRC_ERR_SEQS	0x00000002      /* SRC sequence error                    */
#define	SRC_ERR_SEQD	0x00000004      /* DST sequence error			 */
#define	SRC_ERR_PARI	0x00000008      /* Parity error                          */
#define	SRC_ERR_TOWR	0x00000010      /* timeout value exceeded for transfer   */
#define	SRC_ERR_ALGN	0x00000020	/* not even number of hippi words 32bits */
#define	SRC_ERR_SWRD	0x00000040	/* last word was short, not hippi size   */
#define	SRC_ERR_ABRT	0x00000080	/* host aborted connection, MPC or CNT   */
/* set by the driver interface */
#define	SRC_ERR_RESET	0x00010000	/* reset the controller, no H/W response */

/* set by the i960 on the controller */
#define	DST_ERR_INTC	0x00000001      /* interconnect de-asserted              */
#define	DST_ERR_SEQS	0x00000002      /* SRC sequence error                    */
#define	DST_ERR_SEQD	0x00000004      /* DST sequence error			 */
#define	DST_ERR_PARI	0x00000008      /* parity error                          */
#define	DST_ERR_TOWB	0x00000010      /* timeout waiting for remote SRC burst  */
#define	DST_ERR_RLLE	0x00000020      /* LLRC error                            */
#define	DST_ERR_EXBF	0x00000040      /* DST data exceeded rcv buf (MTU)       */
#define	DST_ERR_ABRT	0x00000080      /* packet was aborted by host interface  */
#define	DST_ERR_ALGN	0x00000100      /* driver gave i960 unaligned sg buffer  */
#define	DST_ERR_ABRTC   0x00000200      /* abort connection,  CONT or MPPC mode  */
#define	DST_ERR_CONT    0x00000400      /* CONT mode error, aborted connection   */
/* set by the driver interface */
#define	DST_ERR_RESET	0x00010000	/* reset the controller, no H/W response */
#define	DST_ERR_DEAD 	0x00020000	/* watchdog, dead recv port, application */
#define	DST_ERR_FILT 	0x00040000	/* watchdog, no filter set for request   */
#define	DST_ERR_TOWP 	0x00080000	/* watchdog, read timout expired         */
#define	DST_ERR_CLEANUP (DST_ERR_DEAD | DST_ERR_FILT | DST_ERR_TOWP | DST_ERR_CONT) 

/* 
 * The hlib_filter_t typedef defines the data structure used by the controller 
 * interface to set a filter with an ioctl call via rhippi driver.
 */
typedef struct hlib_filter {
	u_char          ulp;		/* HiPPI ULP data            */
	u_int           offset;		/* offset in bytes to data   */
	u_char          bsize;		/* size in bytes of data     */
	u_short         data;		/* filter data for matching  */
	boolean_t       flag;		/* TRUE-FALSE, set or unset  */
} hlib_filter_t;


/*  
 * The hctlr_stats_t typedef defines the data structure used by the HiPPI controller 
 * interface for HiPPI channel and driver statistics.
 */
typedef struct {
    struct {               /* SOURCE CHANNEL                    */
        u_int  packets;    /* packets sent                      */
        u_int  bytes;      /* total bytes sent                  */
        u_int  intr;       /* interrupt for SRC channel         */
        u_int  busy;       /* controller to busy for new SRC PD */
        u_int  pq_full;    /* PD queue is full                  */
        u_int  pq_null;    /* PD queue NULL head ptr            */
        u_int  qhit;       /* something to do in PD queue       */
        u_int  qmiss;      /* nothing to do in PD queue         */
        u_int  qflush;     /* flushed queue in re-init cycle    */
        u_int  errors;     /* SRC channel errors                */
        u_int  nointc;     /* interconnect deasserted           */
        u_int  seqs;       /* source sequence error             */
        u_int  seqd;       /* destination sequence error        */
        u_int  towr;       /* timeout waiting for ready         */
        u_int  pari;       /* parity error                      */
        u_int  algn;       /* not even number of HiPPI words    */
        u_int  swrd;       /* last word short, not HiPPI size   */
        u_int  chain;      /* requests sent to i960 in chains   */
        u_int  chain_max;  /* largest chain sent to i960        */
        u_int  overrun;    /* SRC fifo full and no DST readys   */
    } src;
    struct {               /* DESTINATION CHANNEL               */
        u_int  packets;    /* packets received                  */
        u_int  bytes;      /* total bytes received              */
        u_int  intr;       /* interrupt for DST channel         */
        u_int  busy;       /* controller to busy for new DST PD */
        u_int  pq_full;    /* PD queue is full                  */
        u_int  pq_null;    /* PD queue NULL head ptr            */
        u_int  qhit;       /* something to do in PD queue       */
        u_int  qmiss;      /* nothing to do in PD queue         */
        u_int  qflush;     /* flushed queue in re-init cycle    */
        u_int  errors;     /* DST channel errors                */
        u_int  nointc;     /* interconnect deasserted           */
        u_int  seqs;       /* source sequence error             */
        u_int  seqd;       /* destination sequence error        */
        u_int  pari;       /* parity error                      */
        u_int  llrc;       /* LLRC error on burst data          */
        u_int  towb;       /* timeout waiting for burst         */
        u_int  exbf;       /* DST data exceeded buffer resource */
        u_int  abrt;       /* aborted destination connection    */
        u_int  algn;       /* buffer addr and len was unaligned */
        u_int  chain;      /* requests sent to i960 in chains   */
        u_int  underrun;   /* DST fifo empty and no bursts      */
        u_int  halt;       /* DST Channel was halted            */
        u_int  cont;       /* DST Channel was unhalted          */
        u_int  cabort;     /* CONT host aborted connection      */
        u_int  cerr;       /* CONT error, connection aborted    */
        } dst; 
    u_int  srci;           /* SRC interrconnect asserted        */
    u_int  dsti;       	   /* DST interrconnect asserted        */
    u_int  timeouts;       /* watchdog, no controller response  */
    u_int  init;           /* controller init passes            */
    u_int  resets;         /* controller reset count            */
    u_int  dmsg_int;       /* controller debug msg interrupts   */
    u_int  ctlr_int;       /* controller type interrupts        */
    u_int  filters;        /* filters on DST filter list        */
} hctlr_stats_t;


/*
 *  Statistics and counter for RAW HIPPI driver.
 */
typedef struct {
        struct  {         /* RAW SRC REQUESTS        */
           u_int packets; /* packets sent            */
           u_long bytes;  /* total bytes sent        */
           u_int errors;  /* SRC channel errors      */
           u_int qhit;    /* write queue hit         */
           u_int qmiss;   /* write queue miss        */
           u_int qfull;   /* write queue full        */
           u_int qhigh;   /* write queue high mark   */
           u_int chain;   /* write requests chained  */
           u_int reqfail; /* controller request fail */
        } rhsrc;
        struct {          /* RAW DST REQUESTS        */
           u_int packets; /* packets received        */
           u_long bytes;  /* total bytes received    */
           u_int errors;  /* DST channel errors      */
           u_int fqfull;  /* read filter queue full  */
           u_int qhigh;   /* read queue high mark    */
           u_int noreqs;  /* intr but no request match */
           u_int smreq;   /* request buffer to small for data */
           u_int timeout; /* read req. watchdog timer pop     */
           u_int dn_errs; /* port deadname notification error */
           u_int dead;    /* application port dead, ior's aborted */
           u_int no_flt;  /* no filters, read ior's aborted */
           u_int unset;   /* filter unset, ior's requests aborted */
           u_int abrt;    /* filter match, aborted waiting for IOR */
           u_int drop;    /* packets dropped waiting for request   */
         } rhdst;
} rhippi_stats_t;


/*
 *  Statistics and counter for IFHIP driver.
 */
typedef struct {
        struct  {                     /* SOURCE CHANNEL */
                u_int packets;        /* packets sent */
                u_int bytes;          /* total bytes sent */
                u_int intr;           /* interrupt for SRC channel  */
                u_int qhit;           /* something on the ifhip_sreq queue */
                u_int qmiss;          /* empty ifhip_sreq_queue  */
                u_int qfull;          /* full ifhip_sreq_queue  */
                u_int req_fail;       /* controller SRC interface returned error */
                u_int bg_pkts;        /* total of pkts, size greater than 60000 bytes */
                u_int md_pkts;        /* total of pkts, size = 10000-60000 bytes */
                u_int sm_pkts;        /* total of pkts, size = 0-10000 bytes */
                u_int errors;         /* total of src channel errors */
        } ifsrc;
        struct {                      /* DESTINATION CHANNEL */
                u_int packets;        /* packets recieved  */
                u_int bytes;          /* total bytes recieved */
                u_int intr;           /* interrupt for DST channel */
                u_int no_smbufs;      /* no small 1KB buffer for DST */
                u_int no_lgbufs;      /* no large 64KB kernel buffer for DST */
                u_int bg_pkts;        /* total of pkts, size greater than 60000 bytes */
                u_int md_pkts;        /* total of pkts, size = 10000-60000 bytes */
                u_int sm_pkts;        /* total of pkts, size = 0-10000 bytes */
                u_int errors;         /* dst channel errors */
        } ifdst;
} ifhip_stats_t;

#endif _HIPPI_STATUS_H_

