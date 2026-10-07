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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/hctlr.h,v 1.7 1995/03/02 23:32:25 arlin Exp $
 * $Log: hctlr.h,v $
 * Revision 1.7  1995/03/02  23:32:25  arlin
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
 * Revision 1.6  1994/11/29  18:03:30  arlin
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
 * Revision 1.5  1994/11/18  20:43:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/17  15:46:17  arlin
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
 * Revision 1.3  1994/09/06  21:22:51  arlin
 *  fixed HiPPI cache-coherency problems with MCP on.
 *  PTS #9218
 *
 *  Reviewer: Greg Regnier
 *  Risk: medium
 *  Benefit or PTS #: #9218
 *  Testing: HiPPI EATs passed. SDSC application passed.
 *  Module(s): hctlr.c, hctlr.h, mcmsg_mp.c, mcmsg_config.h,
 * 	    mcmsg_config_nx.h, mcmsg_post.h, msgp_mp.c
 *
 * Revision 1.2  1994/07/08  21:39:48  arlin
 * ifdef for MACH_ASSERTS wrong, s/be MACH_ASSERT
 *
 * Revision 1.1  1994/06/08  16:58:43  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *      File:   hctlr.h
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Common Interface Definitions for the HiPPI controller.
 */

#ifndef _HCTLR_H_
#define _HCTLR_H_

#include <i860paragon/hippi/hippi_status.h>
#include <mach_assert.h>

/* testing and debug */
#if MACH_ASSERT
#define   HCTLR_DEBUG 
#endif MACH_ASSERT

#define   HCTLR_STATS  1

/* The 80960CF data structure, i960_scb, behaves like controller registers 
 * This structure is located on the controller and must be volatile.	  
 * It is shared by the i860 and i960 so it CANNOT be changed without i960 
 * F/W source changes.
 */
/*
 * WARNING: any additions to i960_scb structure must be made after ctlr_mon
 * field since the diags and flashutil are dependent on the order of the first 
 * part of structure.
 */
typedef volatile struct i960_scb {

#define	HIPPI_BAD_PARITY	0x00000001	/* parity error on controller 	*/
#define	HIPPI_INIT_FAIL		0x00000002	/* initialization failed	*/
#define	HIPPI_DST_INTC_DE	0x00000004	/* DST interconnect dropped	*/
#define	HIPPI_SRC_INTC_DE	0x00000008	/* SRC interconnect dropped	*/
  unsigned int         ctlr_err;        	/* 32bit, controller errors     */

#define HIPPI_DST_ABORT         0x00000001      /* ULP abort, HiPPI DST connect */
#define HIPPI_960_DEBUG         0x00000002      /* i960 controller debug switch */
#define HIPPI_960_XDEBUG        0x00000004      /* i960 controller extra debug  */
  unsigned int         ctlr_cmd;        	/* 32bit, controller commands   */

#define	HIPPI_INIT_PASS		0x00000001	/* initialization passed	*/
#define	HIPPI_DST_INTC_AS	0x00000002	/* DST interconnect asserted	*/
#define	HIPPI_SRC_INTC_AS	0x00000004	/* SRC interconnect asserted	*/
#define	HIPPI_DMSG_RDY		0x00000008	/* i960 message to print	*/
  unsigned int         ctlr_sts;        	/* 32bit, controller status     */

#define HIPPI_SRC_ABRT          0x00000001      /* drop the SRC connection      */
#define HIPPI_SRC_CONTINUATION  0x00000002      /* multiple buffers per packet  */
  unsigned int         src_cmd; 	       	/* 32bit, controller status     */

#define HIPPI_DST_ABRT          0x00000001      /* drop the DST connection      */
#define HIPPI_DST_CONTINUATION  0x00000002      /* multiple buffers per packet  */
  unsigned int         dst_cmd;        		/* 32bit, controller status     */

  struct src_pd        *src_pd;         	/* 32bit, ptr to source PD      */
  struct dst_pd        *dst_pd;         	/* 32bit, ptr to destination PD */
  caddr_t              debug_msg;       	/* 32bit, ptr to i960 message   */

#define HIPPI_MON_START         0x00000001	/* execute i960 debug monitor	*/
#define HIPPI_MON_MSG_IN_VALID  0x00000002	/* mon message from i960 ready	*/
#define HIPPI_MON_MSG_OUT_VALID 0x00000004	/* mon message to i960 ready	*/
#define HIPPI_MON_INSTALLED     0x00000008	/* monitor is available  	*/
#define HIPPI_MON_RDY           0x00000010	/* monitor started successfully */
#define HIPPI_HOST_RDY          0x00000020	/* driver is ready for messages */
#define HIPPI_MON_BRK           0x00000040	/* break into monitor		*/	
#define HIPPI_DIAG_RSVR1        0x00000080	/* reserved for diagnostics     */
#define HIPPI_DIAG_RSVR2        0x00000100	/* reserved for diagnostics     */
#define HIPPI_DIAG_RSVR3        0x00000200	/* reserved for diagnostics     */
  unsigned int         ctlr_mon;        	/* 32bit, cntrlr monitor regs   */

/*
 * Changes can be made from this point on...
 */
  io_sglist_t          dst_sg_ptr;      	/* 32bit, ptr to sg list DST data    */
  unsigned int         *dst_sg_cont; 		/* 32bit, ptr to more sglist ptrs    */
  unsigned int         dst_sg_cont_size; 	/* 32bit, byte size of cont ptr list */
  unsigned int         host_page_size; 		/* 32bit, host phys memory page size */
  unsigned int         src_timeout; 		/* 32bit, src timeout, 20ns ticks    */
  unsigned int         dst_timeout; 		/* 32bit, dst timeout, 20ns ticks    */
  caddr_t              mon_msg_in_ptr;  	/* 32bit, ptr to message from cntlr  */
  unsigned int         mon_msg_in_size; 	/* 32bit, size of message from cntlr */
  caddr_t              mon_msg_out_ptr; 	/* 32bit, ptr to message to cntlr    */
  unsigned int         mon_msg_out_size;	/* 32bit, size of message to cntlr   */
  caddr_t              mon_msg_in_vptr; 	/* 32bit, virtual ptr of message in  */
  caddr_t              mon_msg_out_vptr;	/* 32bit, virtual ptr of message out */
} i960_scb_t;


/*
 *  Macros
 */
#define	NODE_ID   node_self()

#ifdef HCTLR_STATS
#define	HCSTATS(p, f)	p->stats.f
#else
#define	HCSTATS(p, f)
#endif /* HCTLR_STATS */

#ifdef HCTLR_DEBUG		/* DEBUG */
#ifndef BIT
#define	BIT(x)	(1 << x)
#endif  BIT
#define	HCLOG_ENTER	BIT(0)
#define	HCLOG_HIGH	BIT(1)
#define	HCLOG_MEDIUM	BIT(2)
#define	HCLOG_LOW	BIT(3)

#define	HCLOG(lev, args) \
	if((hclog_level & lev) && (hclog_id & _this_id)) printf args

#define HCLOG_ENTRY(id, args)     \
	long  _this_id = BIT(id);  \
	HCLOG(HCLOG_ENTER, args)

#else				/* else NO HCTLR_DEBUG */

#define	HCLOG_ENTRY(i, a)
#define	HCLOG(l, a)

#endif				/* endif HCTLR_DEBUG */

/* Watchdog poll rate, every second */
#define	HCTLR_POLL		(1 * hz)

/*
 * Transmit Q resources
 * Controller only supports 5 at this time
 * since the PD's are copied into the i960 
 * local memory, which is only 1KBytes.
 */
#define SRC_PD_QCNT   	10 
#define SRC_PD_QSIZE    round_page(SRC_PD_QCNT * sizeof(src_pd_t))
/* Pre-allocated space for SG list continuation support */
#define SRC_PTR_SIZE	1024
#define SRC_PTR_QSIZE   round_page(SRC_PD_QCNT * SRC_PTR_SIZE)
#define HCTLR_SRC_CHAIN_MAX 5	

/*
 * Receive Q resources
 * Controller has no limit on DST PD's however 
 * the driver splits up the queue resources 
 * to try and balance the amount of resources
 * per chain.
 */
#define DST_PD_QCNT     10 
#define DST_PD_QSIZE     round_page(DST_PD_QCNT * sizeof(dst_pd_t))
/* Pre-allocated space for SG list continuation support */
#define DST_PTR_SIZE	1024
#define DST_PTR_QSIZE   round_page(DST_PD_QCNT * DST_PTR_SIZE)
#define HCTLR_RECV_CHAIN_MAX 	(DST_PD_QCNT/2)
#define HIPPI_BURST_SIZE	1024

/* The driver has to give the controller a first burst resource
 * with an extra 32 bytes (extra cache-line amount) in order to get 
 * the end of packet tag with one full first burst of DST data.
 * This will insure that we don't take an extra interrupt for 
 * more data only to get a end of packet tag and no data. This will
 * still occur on 1056 size packets but will be much less frequent 
 * than the 1024 size.
 */
#define HCTLR_BURST_SIZE	(HIPPI_BURST_SIZE+32)	
#define HIPPI_DMSG_MAX		79   	/* max size of i960 debug msg   */

/*
 * The hctlr_src_t typedef defines the data structure used by HiPPI controller 
 * interface and ULP drivers to transmit a packet on the HiPPI source channel.
 */
typedef struct hctlr_src{
        queue_chain_t   links;            /* queue for requests       */
        unsigned int    i_field;          /* connection control       */
        unsigned char   *fb_ptr;          /* 1st burst, physical      */
        unsigned int    fb_len;           /* first burst length       */
        io_req_t        ior;              /* what we are doing        */
        io_sglist_t     sg_ptr;           /* sg pointer, physical     */
        unsigned int    chan_ctl;         /* SRC channel control      */
        int             (*sdone)();       /* call when done           */
        unsigned int    error_status;     /* SRC and ctlr errors      */
        unsigned int    xfer_len;         /* actual data transfer     */
        unsigned int    excl_key;         /* ULP key, exclusive use   */
} hctlr_src_t;

/* Packet Descriptor Structures
 *   The HIPPI controller uses a linked list of data structures
 *   in GP node memory to describe DST and SRC data transfered.
 *   These structures are shared by the i860 and i960 and CANNOT 
 *   be changed without i960 F/W source changes. 
 *
 *       packet descriptors (PD's):  defines the HIPPI packet boundaries
 *       scatter/gather page lists:  data to be transfered (DMA) by controller
 */

typedef struct src_pd {
  unsigned int		   chan_ctl;     /* source channel control     */
/*  set by the host driver */
#define	SRC_CTL_LAST	0x00000001      /* last packet in a chain                */
#define	SRC_CTL_NDSC	0x00000002      /* do not disconnect at end of packet    */
#define	SRC_CTL_CACHE	0x00000004      /* tell i960 to make data cache coherent */
/*  set by the controller  */
#define	SRC_CTL_ORUN	0x00000008      /* tell driver about FIFO overruns       */
/* reserved to be used by driver interface */
#define	SRC_CTL_PENDING	0x00001000      /* req needs queued up for controller    */
#define	SRC_CTL_BUSY	0x00002000      /* request is in process                 */
#define	SRC_CTL_FLMCP	0x00004000      /* flush message co-processor            */
  unsigned int             ifield;       /* ifield for connect request */
  u_char                   *fb_ptr;      /* physical ptr to 1K buffer  */
  unsigned int             fb_len;       /* first burst xfer length    */
  io_sglist_t              sg_ptr;       /* physical ptr to sglist     */
  unsigned int             *sg_cont;     /* phys ptr to more sg ptrs   */
  unsigned int             sg_cont_size; /* phys ptr to more sg ptrs   */
  struct src_pd            *next_pd;     /* physical ptr to next PD    */
  unsigned int             chan_err;     /* source channel errors      */
/* error definitions in hippi_status.h */
  unsigned int             sg_len;       /* rest of data xfer length   */
  /* end of i960 ordered fields */
  struct src_pd            *next_pdv;    /* virt ptr to next PD        */
  unsigned int             *sg_cont_ptrv; /* virt ptr to more sg ptrs  */
  hctlr_src_t		   *src_req;     /* reference to ULP request   */
  unsigned int             filler[3];    /* filler for cache-line      */
} src_pd_t;


typedef struct dst_pd {
  unsigned int             chan_ctl;    /* dest. channel control      */
/* set by the host driver */
#define	DST_CTL_LAST	0x00000001      /* last packet resource in chain */
/* set by the controller  */
#define	DST_CTL_URUN	0x00000002      /* tell driver about FIFO underruns */
  struct dst_pd            *next_pd;    /* physical ptr to next PD    */
  u_char                   *fb_ptr;     /* physical ptr to 1K buffer  */
  unsigned int             fb_len;      /* first burst xfer length    */
  unsigned int             xfer_len;    /* rest of data xfer length   */
  unsigned int             chan_err;    /* dest. channel errors       */
/* error definitions in hippi_status.h */
  unsigned int             ifield;      /* ifield used for request    */
  unsigned int             chan_sts;    /* dest. channel status       */
/* set by the controller */
#define	DST_STS_BUSY	0x00000001      /* NOT USED:                             */
#define	DST_STS_COMP	0x00000002      /* transmission complete                 */
#define	DST_STS_ERRS	0x00000004      /* error(s) detected                     */
  /* end of i960 ordered fields */
  u_char                   *fb_ptrv;    /* virtual ptr to 1K buffer   */
  struct dst_pd            *next_pdv;   /* virtual ptr to next PD     */
  unsigned int             *sg_cont_ptrv; /* virt ptr to more sg ptrs */
  unsigned int             filler[5];   /* filler for cache-line      */
} dst_pd_t;


/*
 * The hctlr_dst_t typedef defines the data structure used by HiPPI controller 
 * interface and ULP drivers to receive a packet on the HiPPI source channel.
 */
typedef struct hctlr_dst{
        unsigned char   *fb_ptr;           /* 1st burst, virtual      */
        unsigned int    xfer_len;          /* data transfer length    */
        unsigned int    error_status;      /* output errors           */
        boolean_t       pkt_done;          /* end of packet           */
        unsigned char   ulp;               /* HiPPI ULP data          */
        unsigned int    filter_data;       /* matched filter data     */
        unsigned int    ifield;            /* used for connection     */	
        int             (*ddone)(); 	   /* routine call when done  */
} hctlr_dst_t;

/*  
 * The hctlr_filter_t typedef defines the data structure used by the HiPPI controller 
 * interface to set a filter for async input data on the  HiPPI destination channel.
 */
typedef struct hctlr_filter{
        queue_chain_t        links;      /* queue for filter          */
        unsigned char        ulp;        /* HiPPI ULP data            */
        unsigned int         offset;     /* offset in bytes to data   */
        unsigned char        bsize;      /* size in bytes of data     */
        unsigned int         min;        /* minimum value of data     */
        unsigned int         max;        /* maximum value of data     */
        unsigned int         excl_key;   /* ULP key, exclusive use    */
        int                  (*ddone)(); /* routine call when done    */
} hctlr_filter_t;

typedef struct hctlr_softc{
        i960_scb_t    *scb;          /*  physical ptr to i960 scb    */
        caddr_t       hippi_base;    /*  physical ptr to ctlr. sram  */
        queue_chain_t src_reqs;      /*  src ctlr request queue      */
        decl_simple_lock_data(,src_req_lock)  /*lock,ctlr req. queue */
        u_char        *begin_sptrs;  /*  SG pointer bufs, start addr */
        u_char        *begin_fb;     /*  first burst bufs, start addr*/
        src_pd_t      *begin_spd;    /*  src PD, starting address    */
        src_pd_t      *spd_qhead;    /*  src PD queue head ptr       */
        src_pd_t      *spd_qchain;   /*  src PD queue 2nd chain ptr  */
        src_pd_t      *spd_qtail;    /*  src PD queue tail ptr       */
        dst_pd_t      *begin_dpd;    /*  dst PD, start addr.         */
        u_char        *begin_dptrs;  /*  SG pointer bufs, start addr */
        dst_pd_t      *dpd_qhead;    /*  dst PD queue head ptr       */
        dst_pd_t      *dpd_qchain;   /*  dst PD queue 2nd chain ptr  */
        dst_pd_t      *dpd_qtail;    /*  dst PD queue tail ptr       */
        hctlr_dst_t   dst_req;       /*  DST ctlr request structure  */
        queue_chain_t dst_filters;   /*  filter list for async input */
        decl_simple_lock_data(,dst_filter_lock) /*lock, filter list  */
#define HCTLR_UNINITIALIZED     0x00
#define HCTLR_UP                0x01
#define HCTLR_EXCL              0x02
#define HCTLR_TIMEOUT           0x04
#define HCTLR_HW_ERROR          0x08 
#define HCTLR_SW_ERROR          0x10  
        int            state;        /*  state of interface          */
        int            excl_key;     /*  unique ULP key, EXCL access */
#define	HCTLR_SRC_PENDING 	0x01
#define HCTLR_SRC_MPPC		0x02
#define HCTLR_SRC_CONT		0x04
        int            src_state;    /*  state of channels           */
#define	HCTLR_DST_CLOSED	0x01
#define	HCTLR_DST_HALT		0x02
#define HCTLR_DST_MPPC		0x04
#define HCTLR_DST_CONT		0x08
#define HCTLR_DST_PROM		0x10
#define HCTLR_DST_CONT_DROP	0x20
        int            dst_state;    /*  state of channels           */
        int            node_id;      /*  node id for console message */
        int            src_timer;    /*  SRC timer for watchdog      */
        int            dst_timer;    /*  DST timer for watchdog      */
#ifdef HCTLR_STATS
        hctlr_stats_t  stats;        /*  interface statistics        */
#endif      /* HCTLR_STATS */
} hctlr_softc_t;


/* 
 * Function prototypes for functions called from externel drivers. 
 */
extern int hctlr_timer;

/*
 * HiPPI controller initialization    
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - controller did not respond or initialization failed.
 */
int 
hctlr_init(/* no arguments */);

/*
 * Queue up data for HiPPI controller SRC channel transfer     
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - controller is not responding.
 */
int 
hctlr_src_q(
	/* hctlr_src_t	 *src_req */);	/* request from a ULP driver */

/*
 * Give the HiPPI controller a io_sglist pointer for DST data
 * 
 *   sg_ptr set to NULL will force controller to abort DST connection
 */
void 
hctlr_recv_buffer(
	/* io_sglist_t  sg_ptr */);	/* physical pointer to sglist */

/*
 * Tell HiPPI controller interface to set DST channel state  
 *   to HCTLR_DST_HALT and hold off readys to force flow control.
 */
void 
hctlr_dst_halt(); 

/*
 * Tell HiPPI controller interface to reset DST channel state  
 *   from HCTLR_DST_HALT and allow DST data to continue.
 */
void 
hctlr_dst_continue(); 

/*
 * Set a filter for async input data on the DST channel      
 *
 * Returns on success
 *      D_SUCCESS
 *
 *      D_IO_ERROR - controller is not responding.
 *      D_INVALID_OPERATION - filter is invalid or already exists 
 */
int 
hctlr_recv_filter(
	/* hctlr_filter_t  *dst_filter; */     /* pointer to new filter        */
	/* boolean_t       flag;         */ ); /* true to add, false to remove */

/*
 * low level ioctl control for controller interface from ULP drivers. 
 *
 *  Valid types include:
 *   HIPPI_DST_TIMEOUT, HIPPI_SRC_TIMEOUT, HIPPI_DEV_EXCL
 *   HIPPI_DEV_MPPC, HIPPI_DEV_CONT      
 *
 * Returns on success:
 *      D_SUCCESS
 *
 * errors:
 *      D_IO_ERROR - controller is not responding.
 *      D_INVALID_OPERATION - invalid  
 */
int 
hctlr_ioctl(
	/* int   type;   */   /* type of ioctl  */
	/* int   param; */    /* parameter for ioctl */
        /* int   key;  */  ); /* unique ULP key for EXCL access */

/*
 * hctlr_ctr_i(unit)
 *
 * This routine will be called for all controller type interrupts
 * including initialization, controller errors and status, etc.
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler
 *
 * Possible errors
 *      - SRC or DST interrupt before initialization
 */
void 
hctlr_ctr_i(
	/* u_int	unit */);	/* unit number, always 0 */

/*
 * hctlr_src_i(unit)
 *
 * This routine will be called to service SRC channel interrupts
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler
 *
 * Possible errors
 *      - interrupt before initialization 
 */
void 
hctlr_src_i(
	/* u_int	unit */);	/* unit number, always 0 */

/*
 * hctlr_dst_i(unit)
 *
 * This routine will be called to service DST channel interrupts
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler
 *
 * Possible errors
 *      - interrupt before initialization 
 */
void 
hctlr_dst_i(
	/* u_int	unit */);	/* unit number, always 0 */

#endif _HCTLR_H_

