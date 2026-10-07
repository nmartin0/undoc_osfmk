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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/hctlr.c,v 1.9 1995/03/02 23:31:44 arlin Exp $
 * $Log: hctlr.c,v $
 * Revision 1.9  1995/03/02  23:31:44  arlin
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
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1994/11/29  17:52:19  arlin
 *  Raw hippi DST filtering with ulp and ports fixed.
 *
 *  Reviewer: Bernie Keany
 *  Risk: low
 *  Benefit or PTS #: 11573
 *  Testing: Test Case's port.c noport.c passed. Used kernel debugging tools
 *  	       to ensure that the correct filters are being set and reset.
 *  Module(s): hctlr.c, hippi.c, if_hippi.h, hctlr.h, rhippi.h
 *
 * Revision 1.7  1994/11/18  20:43:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/10/25  23:31:42  arlin
 *  Fixed hang problem with DST cache-line hit.
 *  add flush in hctlr_recv_buffer().
 *
 *  Reviewer: Bernie Keany
 *  Risk: low
 *  Benefit or PTS #: 11383
 *  Testing: HiPPI EATS passed
 *  Module(s): hctlr.c, hctlr_recv_buffer()
 *
 * Revision 1.5  1994/10/17  15:45:45  arlin
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
 * Revision 1.4  1994/09/06  21:22:27  arlin
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
 * Revision 1.3  1994/07/26  18:31:31  arlin
 * move a debug message to get correct information.
 *
 * Revision 1.2  1994/06/22  18:06:06  arlin
 * problem in hctlr_src_q() where requests got queued up out
 * of order during async stress testing.
 *
 * Revision 1.1  1994/06/08  16:58:34  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *      File:   hctlr.c
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Common Interface to the HiPPI controller.
 */

#include <hippi.h>
#if	NHIPPI > 0

#include <sys/types.h>
#include <chips/busses.h>
#include <vm/vm_kern.h>
#include <kern/queue.h>
#include <kern/assert.h>
#include <device/io_req.h>
#include <i860paragon/spl.h>
#include <i860paragon/expansion.h>
#include <i860paragon/hippi/hippidev.h>
#include <i860paragon/hippi/hctlr.h>

static int  hctlr_bld_srcq(), hctlr_bld_dstq(), hctlr_hwrst();
static void hctlr_src_pd(), hctlr_send(), hctlr_recv(), hctlr_watch();

int	hippi_stats();

hctlr_softc_t	hctlr_softc;

extern struct bus_device  *hippi_dinfo[];
extern struct bus_ctlr    *hippi_minfo[];
extern boolean_t  hippi_probed_okay;
extern int expansion_id();

/*
 * Set hclog_id bits accordingly for debug messages:
 * (via kernel debugger, w hclog_id 0x..)
 *
 *
 *  15 14 13 12  11 10 9 8  7 6 5 4  3 2 1 0
 *   |  |  |  |   |  | | |  | | | |  | | | |__hctlr_init 
 *   |  |  |  |   |  | | |  | | | |  | | |____hctlr_hwrst 
 *   |  |  |  |   |  | | |  | | | |  | |______hctlr_bld_srcq  
 *   |  |  |  |   |  | | |  | | | |  |________hctlr_bld_dstq
 *   |  |  |  |   |  | | |  | | | |___________hctlr_src_pd  
 *   |  |  |  |   |  | | |  | | |_____________hctlr_src_q  
 *   |  |  |  |   |  | | |  | |_______________hctlr_send   
 *   |  |  |  |   |  | | |  |_________________hctlr_recv   
 *   |  |  |  |   |  | | |____________________hctlr_recv_buffer
 *   |  |  |  |   |  | |______________________hctlr_recv_filter
 *   |  |  |  |   |  |________________________hctlr_ctr_i     
 *   |  |  |  |   |___________________________hctlr_src_i       
 *   |  |  |  |_______________________________hctlr_dst_i    
 *   |  |  |__________________________________hctlr_dst_halt 
 *   |  |_____________________________________hctlr_dst_continue
 *   |________________________________________hctlr_ioctl       
 */

#ifdef HCTLR_DEBUG
long hclog_id    = 0x0;
long hclog_level = 0xfff;
int  flush_mcp_cnt = 0;
#endif HCTLR_DEBUG

/* misc. driver variables */
caddr_t	dmsg_ptr;
char	dmsg_print[HIPPI_DMSG_MAX+1];
int 	hippi_cache_limit;
int     hctlr_timer = 7;  /* default, in seconds */


/*
 * hctlr_init()		HiPPI controller initialization
 *
 * Called by: HiPPI ULP drivers, hctlr_watch() 
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - controller does not respond or initialization failed.
 */
hctlr_init()
{
        hctlr_softc_t  *sp = &hctlr_softc;
        hctlr_src_t    *srq;
        int     oldpri = SPLHIPPI();

        HCLOG_ENTRY(0, ("hctlr_init(sp=%lx)\n", sp));

	if (sp->state & HCTLR_UP) {
        	splx(oldpri);
		return(D_SUCCESS);
	}

        /*
	 *  Sanity check....
	 *  Is controller probed and alive?
	 */
	 if (!hippi_probed_okay)
	 	return(D_IO_ERROR);

	/* driver software errors are non-recoverable */
	if (sp->state & HCTLR_SW_ERROR) {
        	splx(oldpri);
		return(D_IO_ERROR);
	}

	/* 
	 * If interface is uninitialized. set initial channel state 
	 * to closed, set i960 cache limit, bld queues, init locks.
         * hippi_cache_limit:
	 *    allow the max data transfer count that will be 
 	 *    cached by the i960 to be changed if necessary.
         *    Default is set to 1Kbytes.
         */
	if (sp->state == HCTLR_UNINITIALIZED) {
		sp->node_id = NODE_ID;
		sp->scb = (i960_scb_t *)hippi_dinfo[HIPPI_DEV_SRAM]->address;
                if (hctlr_bld_srcq(sp)) {
                	sp->state = HCTLR_SW_ERROR;
        		splx(oldpri);
                        return(D_IO_ERROR);
		}
                if (hctlr_bld_dstq(sp)) {
                	sp->state = HCTLR_SW_ERROR;
        		splx(oldpri);
                        return(D_IO_ERROR);
		}
		sp->dst_state |= HCTLR_DST_CLOSED;
		hippi_cache_limit = getbootint("HIPPI_I960_CACHE_MAX", 1024); 
		queue_init(&sp->src_reqs);
		queue_init(&sp->dst_filters);
		simple_lock_init(&sp->src_req_lock);
		simple_lock_init(&sp->dst_filter_lock);
	}

        /* 
	 * if called from watch_dog than take care of any outstanding
         * requests from all ULP drivers . Check both DST and SRC queues.
         */
        if (sp->state & HCTLR_TIMEOUT) {
                printf("WARNING: hippi board not responding, flushing queues\n");
                while(sp->spd_qhead) {
			/* callback the ULP driver with timeout error       */
			/* walk the src_req queue with and the src_pd queue */

			simple_lock(&sp->src_req_lock);
			srq = (hctlr_src_t *)dequeue_head(&sp->src_reqs);
			simple_unlock(&sp->src_req_lock);

			assert(srq != 0);

			srq->error_status = SRC_ERR_RESET;
			srq->sdone(0,srq,1);

                        printf("  callback (%x) with error (%x)\n",
					srq->sdone,SRC_ERR_RESET);
        		printf("   cci %x fb,len %x,%d ior %x sg,len %x,%d ctl %x\n",
				sp->spd_qhead->ifield, sp->spd_qhead->fb_ptr,
				sp->spd_qhead->fb_len, sp->spd_qhead->src_req->ior,
				sp->spd_qhead->sg_ptr, sp->spd_qhead->sg_len,
				sp->spd_qhead->chan_ctl);

			panic("hippi timeout");

                        /* Reset SRC PD control block */
                        sp->spd_qhead->chan_ctl = 0;
                        sp->spd_qhead->ifield   = 0;
                        sp->spd_qhead->fb_ptr   = 0;
                        sp->spd_qhead->fb_len   = 0;
                        sp->spd_qhead->sg_ptr   = 0;
                        sp->spd_qhead->chan_err = 0;
                        sp->spd_qhead->sg_len   = 0;
			sp->spd_qhead->src_req  = 0;
                        if (sp->spd_qhead == sp->spd_qtail) {
                                sp->spd_qhead = 0;
                        	HCSTATS(sp, src.qflush++);
			}
                        else {
                                sp->spd_qhead = (src_pd_t *)sp->spd_qhead->next_pdv;
			}
		}
		/*  PD queue is empty, pick up any pending requests not yet queued */
		if (sp->src_state & HCTLR_SRC_PENDING) { 
			simple_lock(&sp->src_req_lock);
			queue_iterate(&sp->src_reqs, srq, hctlr_src_t*, links)
            		{
				srq->error_status = SRC_ERR_RESET;
				srq->sdone(0,srq,1);
				/* ok to remove in loop since next ptr is left intact */
				queue_remove(&sp->src_reqs, srq, hctlr_src_t*, links);
                	}
			simple_unlock(&sp->src_req_lock);
			sp->src_state &= ~HCTLR_SRC_PENDING;
		}
                while(sp->dpd_qhead) {
                        /* Reset DST PD control block */
                        sp->dpd_qhead->chan_ctl = 0;
                        sp->dpd_qhead->xfer_len = 0;
                        sp->dpd_qhead->chan_err = 0;
                        sp->dpd_qhead->ifield   = 0;
                        sp->dpd_qhead->chan_sts = 0;
                        if (sp->dpd_qhead == sp->dpd_qtail) {
                                sp->dpd_qhead = 0;
                        	HCSTATS(sp, dst.qflush++);
			}
                        else {
                                sp->dpd_qhead = (dst_pd_t *)sp->dpd_qhead->next_pdv;
			}
			/* if DST was processing incoming data, callback with error */
			if (sp->dst_req.ddone) {
                        	printf("       calling ddone (0x%x) with error (0x%x\n",
						sp->dst_req.ddone,DST_ERR_RESET);
				sp->dst_req.xfer_len = 0;
				sp->dst_req.pkt_done = TRUE;
				sp->dst_req.error_status = DST_ERR_RESET;
				sp->dst_req.ddone(0,&sp->dst_req);
				/* reset ddone and error status */
				sp->dst_req.ddone = 0;
				sp->dst_req.error_status = 0;
			}
                }
		/* reset TIMEOUT state */
		sp->state &= ~HCTLR_TIMEOUT;
	}

        /* re-initialize the i960 SCB before reseting */
        sp->scb->ctlr_err     = 0;    /* controller error  */
        sp->scb->ctlr_cmd     = 0;    /* controller commands */
        sp->scb->ctlr_sts     = 0;    /* controller status */
        sp->scb->src_cmd      = 0;    /* SRC channel commands */
        sp->scb->dst_cmd      = 0;    /* DST channel commands */
        sp->scb->src_pd       = 0;    /* Phys addr. source PD queue */
        sp->scb->dst_pd       = 0;    /* Phys addr. destination PD queue */
        sp->scb->dst_sg_ptr   = 0;    /* Phys addr. of sglist for DST data */
        sp->scb->dst_sg_cont  = 0;    /* Phys addr. of sglist continuation */
        sp->scb->dst_sg_cont_size = 0;  /* size of continuation list of ptrs */
        sp->scb->debug_msg   = 0;   	 /* Phys addr. of i960 debug message*/
        sp->scb->ctlr_mon &= HIPPI_MON_INSTALLED; /* i960 debug monitor register */

	/* The i960 is using two hardware free-running 32-bit counters running at 50 MHz,
  	 * that is, 20ns per tick; if for example we want to count up to
    	 * 1 second then the number in the counter that is equivalent to 1 second
     	 * is 1 s / 20 ns = 50 * 10**6. The driver will expect microsecond values
	 * and convert into 20ns per tick value for the i960. default == 1 sec.
	 * set after NMI since controller reset SCB during initialization 
	 */
        sp->scb->host_page_size = PAGE_SIZE;   /* Phys page size of memory */
        sp->scb->src_timeout = 50 * getbootint("HIPPI_SRC_TIMEOUT", 1000000);   
        sp->scb->dst_timeout = 50 * getbootint("HIPPI_DST_TIMEOUT", 1000000);

        /*
         * Reset the Hippi controller
         */
        if ((hctlr_hwrst(sp)) != D_SUCCESS) {
                sp->state = HCTLR_HW_ERROR;
        	splx(oldpri);
		return(D_IO_ERROR);
	}

	/* convert timeout to seconds and times by MAX SRC chain to get the MAX time
	 * a controller interrupt will take. add 2 seconds  
	 */
	hctlr_timer = ((sp->scb->src_timeout/50000000) * HCTLR_SRC_CHAIN_MAX) + 2;

#ifdef HCTLR_DEBUG
        if (hclog_id)
        	sp->scb->ctlr_cmd     = 4;    /* controller debugging on */
#endif HCTLR_DEBUG
	/* 
         * Set interface to proper run state, give controller
	 * some DST PD resources, pick up any pending SRC requests,
	 * and start watch dog timer.
	 */
	sp->state |= HCTLR_UP;	
	sp->spd_qchain = 0;
	sp->src_timer = -1;
	sp->dst_timer = -1;
        hctlr_send(sp);
	hctlr_recv(sp);
        timeout(hctlr_watch, sp, HCTLR_POLL);

        splx(oldpri);
        return(D_SUCCESS);
}


/*
 * hctlr_hwrst(sp)	reset the HiPPI controller
 *
 * Called by: hctlr_init() 
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - controller did not respond or initialization failed.
 *
 *  This routine resets the HIPPI controller.
 *  It assumes that we are at SPLHIPPI.
 */

hctlr_hwrst(sp)
        hctlr_softc_t  *sp;
{
        int retry_count,intr_status,ctlr_status,ctlr_error,i;
        u_int           dmsg_check;

        HCLOG_ENTRY(1, ("hctlr_hwrst(sp=%lx)\n", sp));
        HCSTATS(sp, resets++);

	/* reset i960 and controller with NMI */
        outl(HIPPI_INTR_STATUS,0);
        outl(HIPPI_I960_NMI,1);

	/* give board time to respond */
	delay(5000000);

        retry_count = 0;

        while (TRUE) {
		/* check status of controller */
                intr_status = inb(HIPPI_INTR_STATUS);
                ctlr_status = sp->scb->ctlr_sts;
                ctlr_error  = sp->scb->ctlr_err;
                /* go ahead and take debug messages at this time */
                if (ctlr_status & HIPPI_DMSG_RDY) {
#ifdef HCTLR_DEBUG
                        /* check to see if i960 gave me a good address  */
                        dmsg_check = (u_int)(sp->scb->debug_msg);
                        if ((sp->scb->debug_msg == 0) ||
                                ((dmsg_check & 0x82000000) != 0x82000000)) {
                                printf("%d: hctlr_hwrst: debug msg, invalid addr\n",
                                                 sp->node_id);
                        } else {
                                /* change the brd phys to virtual offset */
                                dmsg_check &= 0x0000ffff;
                                dmsg_check += (u_int) (sp->scb);
                                dmsg_ptr = (caddr_t) dmsg_check;
                                for (i=0;i<HIPPI_DMSG_MAX,(*dmsg_ptr!=0);i++,dmsg_ptr++)
                                        dmsg_print[i] = *dmsg_ptr;
                                dmsg_print[i] = 0;
                                printf("%s",dmsg_print);
                                sp->scb->debug_msg = 0;
                                HCSTATS(sp, dmsg_int++);
                        }
#endif HCTLR_DEBUG
			/* clear debug message bit and interrupt */
                        sp->scb->ctlr_sts &= ~HIPPI_DMSG_RDY;
        		outl(HIPPI_INTR_STATUS,0);
			delay(5000000);  	/* give board 10ms to respond */
			continue; 	
                }
                /* check for contoller interrrupt and pass status */
                if ((!(intr_status & HIPPI_CTLR_INTR)) ||
                        (!(ctlr_status & HIPPI_INIT_PASS))) {
                        if (retry_count == 5) {
                                printf("%d: hctlr_hwrst: HiPPI board did not reset!\n",sp->node_id);
                                printf("%d: hctlr_hwrst: Intr=%x Sts=%x Err=%x Retrys=%d\n",
                                        sp->node_id,intr_status,ctlr_status,
                                        ctlr_error,retry_count);
				/* retry exhausted */
                                return (D_IO_ERROR); 
                        }
                        retry_count++;	/* bad status from controller  */
                        printf("%d: hctlr_hwrst: ERROR Intr=%x Sts=%x Err=%x Retrys=%d\n",
                                 sp->node_id,intr_status,ctlr_status,
                                 ctlr_error,retry_count);
			/* reset controller with NMI, give it time to respond */
        		outl(HIPPI_INTR_STATUS, 0); 
        		outl(HIPPI_I960_NMI,1); 
			delay(5000000);  	/* a little more time to respond */
                } else {
                        break;   	/* good status from controller */
                }
        }
        return(D_SUCCESS);
}


/* STATIC
 *
 * hctlr_bld_srcq(sp)	 build the SRC command queues
 *
 * Called by: hctlr_init() 
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - no memory for kmem_alloc_wired.
 *
 *  The routine will build and initialize the transmit SRC queue.
 *  The structure of the queue is illustrated below.
 *  Note that all fields marked with a "~" are initialized only once.
 *  The PD (packet descriptors) is a circular queue and the scatter/
 *  gather list and the data is provided by the ULP drivers.
 *
 *   .----------------------------------------------- n -------------.
 *   |                                                               |
 *   +-->+---------+  .-->+------+  .-->+------+  .-- n -->+------+  |
 *       | chan_ctl|  |   |      |  |   |      |  |        |      |  |
 *       |~next_pd |--+   |      |--+   |      |--+        |      |--+
 *   +---| sg_ptr  |  +---|      |  +---|      |       +---|      |
 *   | +-| fb_ptr  |  | +-|      |  | +-|      |       | +-|      |
 *   | | | fb_len  |  | | |      |  | | |      |       | | |      |
 *   | | +---------+  | | +------+  | | +------+       | | +------+
 *   | | src_pd       | | src_pd    | | src_pd         | | src_pd
 *   | |              | |           | |                | |
 *   | +>+---------+  | +>+------+  | +>+------+       | +>+------+
 *   |   | Burst   |  |   |      |  |   |      |       |   |      |
 *   |   | First   |  |   |      |  |   |      |       |   |      |
 *   |   +---------+  |   +------+  |   +------+       |   +------+
 *   |                |             |                  |
 *   +-->+---------+  +-->+------+  +-->+------+       +-->+------+
 *       | SG size |      |      |      |      |           |      |
 *       +---------+      +------+      +------+           +------+
 *       | entries |      |      |      |      |           |      |
 *       +---------+      +------+      +------+           +------+
 *   +---| buf_addr|  +---|      |  +---|      |       +---|      |
 *   |   | buf_len |  |   |      |  |   |      |       |   |      |
 *   |   +---------+  |   +------+  |   +------+       |   +------+
 *   | +-| buf_addr|  | +-|      |  | +-|      |       | +-|      |
 *   | | | buf_len |  | | |      |  | | |      |       | | |      |
 *   | | +---------+  | | +------+  | | +------+       | | +------+
 *   | | io_sglist    | | io_sglist | | io_sglist      | | io_sglist
 *   | |              | |           | |                | |
 *   | +>+---------+  | +>+------+  | +>+------+       | +>+------+
 *   +   | buffer  |  |   |      |  |   |      |       |   |      |
 *   |   |PAGE_SIZE|  |   |      |  |   |      |       |   |      |
 *   |   +---------+  |   +------+  |   +------+       |   +------+
 *   |                |             |                  |
 *   +-->+---------+  +-->+------+  +-->+------+       +-->+------+
 *       | buffer  |      |      |      |      |           |      |
 *       |PAGE_SIZE|      |      |      |      |           |      |
 *       +---------+      +------+      +------+           +------+
 *
 *  NOTE: The kernel wires down the memory at device_write time and   
 *        dynamically builds the scatter/gather list for each request.
 *
 */
hctlr_bld_srcq(sp)
        hctlr_softc_t  *sp;
{
        src_pd_t        *pd_q;
	u_char		*sg_q;
        int             i;

        HCLOG_ENTRY(2, ("hctlr_bld_srcq(sp=%lx)\n", sp));

        /*      Wire down the PD resources.
         *      HiPPI board and driver interface will share this memory
         */
        if (kmem_alloc_wired(kernel_map,&sp->begin_spd,SRC_PD_QSIZE)!=KERN_SUCCESS)
                return(D_IO_ERROR);

        if (kmem_alloc_wired(kernel_map,&sp->begin_sptrs,SRC_PTR_QSIZE)!=KERN_SUCCESS)
                return(D_IO_ERROR);

        pd_q = sp->begin_spd;
	sg_q = (u_char *)sp->begin_sptrs;

        /*
         * Build the SOURCE PD (packet descriptor) Queue
         */
        for (i = 0; i < SRC_PD_QCNT; i++, pd_q++, sg_q += SRC_PTR_SIZE) {
                pd_q->chan_ctl = 0;
                pd_q->ifield   = 0;
                pd_q->fb_ptr   = 0;
                pd_q->fb_len   = 0;
                pd_q->sg_ptr   = 0;
                pd_q->sg_cont  = 0;
                pd_q->sg_cont_size = 0;
                pd_q->next_pd  = (struct src_pd *)kvtophys(pd_q + 1);
                pd_q->chan_err = 0;
                pd_q->sg_len   = 0;
                pd_q->next_pdv = (struct src_pd *)pd_q + 1;
                pd_q->sg_cont_ptrv = (u_int *)sg_q;
        }

        /*
         *  Make PD queue circular
         */
        pd_q--;
        pd_q->next_pdv  = (struct src_pd *)sp->begin_spd;
        pd_q->next_pd   = (struct src_pd *) kvtophys(sp->begin_spd);

        /*
         *  Set PD head and tail pointers.
         */
        sp->spd_qhead = 0;
        sp->spd_qtail = (src_pd_t *)sp->begin_spd;

        HCLOG(HCLOG_LOW, ("hctlr_bld_srcq: PD_Q count %d size %d begin 0x%x\n",
                        	SRC_PD_QCNT,SRC_PD_QSIZE,sp->begin_spd));
	HCLOG(HCLOG_LOW, ("hctlr_bld_srcq: head 0x%x tail 0x%x end 0x%x\n",
                        	sp->spd_qhead,sp->spd_qtail,pd_q));
        return(D_SUCCESS);
}


/* STATIC
 *
 * hctlr_bld_dstq(sp)	 build the DST command queues
 *
 * Called by: hctlr_init() 
 *  
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - no memory for kmem_alloc_wired.
 *
 *  The routine will build and initialize the receive DST queue.
 *  The structure of the queue is illustrated below.
 *  Note that all fields marked with a "~" are initialized only once.
 *  The PD (packet descriptors) is a circular queue.
 *
 *   .----------------------------------------------- n -------------.
 *   |                                                               |
 *   +-->+---------+  .-->+------+  .-->+------+  .-- n -->+------+  |
 *       | chan_ctl|  |   |      |  |   |      |  |        |      |  |
 *       |~next_pd |--+   |      |--+   |      |--+        |      |--+
 *     +-| fb_ptr  |    +-|      |    +-|      |         +-|      |
 *     | | fb_len  |    | |      |    | |      |         | |      |
 *     | +---------+    | +------+    | +------+         | +------+
 *     | dst_pd         | dst_pd      | dst_pd           | dst_pd
 *     |                |             |                  |
 *     +>+---------+    +>+------+    +>+------+         +>+------+
 *       | First   |      |      |      |      |           |      |
 *       | Burst   |      |      |      |      |           |      |
 *       +---------+      +------+      +------+           +------+
 *
 *  NOTE: The interface driver will maintain enough receive buffer 
 *	  resources so that the i960 will always have a place for
 *        the first burst coming in from the HiPPI DST channel. 
 *	  HiPPI ULP drivers are responsible for giving the i960
 * 	  additional receive buffers via the hctlr_recv_buffer()
 * 	  routine.
 */
hctlr_bld_dstq(sp)
        hctlr_softc_t  *sp;
{
        dst_pd_t        *pd_q;
        u_char		*fb_q;
        u_char		*sg_q;
        int             i;

        HCLOG_ENTRY(3, ("hctlr_bld_dstq(sp=%lx)\n", sp));

        /* Wire down the DST PD_Q and First Burst resource buffers. 
	 * The First Burst resources are set to 2Kb sizes so that
	 * there is room for the end of packet tag when the packet
	 * ends with exactly one full burst (1024).
         * HiPPI board and driver will share this memory
         */
        if (kmem_alloc_wired(kernel_map, &sp->begin_dpd, DST_PD_QSIZE) != KERN_SUCCESS)
                return(D_IO_ERROR);
        if (kmem_alloc_wired(kernel_map, &sp->begin_fb, 
				((2*HIPPI_BURST_SIZE)*DST_PD_QCNT)) != KERN_SUCCESS)
                return(D_IO_ERROR);
        if (kmem_alloc_wired(kernel_map,&sp->begin_dptrs,DST_PTR_QSIZE)!=KERN_SUCCESS)
                return(D_IO_ERROR);

        pd_q = sp->begin_dpd;
	fb_q = sp->begin_fb;
	sg_q = (u_char *)sp->begin_dptrs;

        /*
         * Build the DESTINATION PD (packet descriptor) Queue
	 * Initialize the fb_len to 1024 plus one cache line (32)
	 * so that the first Burst DMA is set up for just enough 
	 * buffer to get the end of packet tag out of the DST
	 * fifo. It will stop at the next cache-line so that if
	 * don't get an end of packet tag the rest of the data
	 * will begin on a cache-line for optimal performance.
         */

        for (i = 0; i < DST_PD_QCNT; i++, pd_q++) {
                pd_q->chan_ctl = 0;
                pd_q->next_pd  = (struct dst_pd *)kvtophys(pd_q + 1);
                pd_q->fb_ptr   = (u_char *)kvtophys(fb_q);
                pd_q->fb_len   = HCTLR_BURST_SIZE;
                pd_q->xfer_len = 0;
                pd_q->chan_err = 0;
                pd_q->ifield   = 0;
                pd_q->chan_sts = 0;
                pd_q->fb_ptrv  = (u_char *)fb_q;
                pd_q->next_pdv = (struct dst_pd *)pd_q + 1;
                pd_q->sg_cont_ptrv = (u_int *)sg_q;
		fb_q += (2*HIPPI_BURST_SIZE);
		sg_q += DST_PTR_SIZE;
        }

        /*
         *  Make PD queue circular
         */
        pd_q--;
        pd_q->next_pdv  = (struct dst_pd *)sp->begin_dpd;
        pd_q->next_pd   = (struct dst_pd *)kvtophys(sp->begin_dpd);

        /*
         *  Set PD head and tail pointers.
         */
        sp->dpd_qhead = 0;
        sp->dpd_qtail = (dst_pd_t *)sp->begin_dpd;

        HCLOG(HCLOG_LOW, ("hctlr_bld_dstq: PD_Q count %d size %d begin 0x%x\n",
                        	DST_PD_QCNT,DST_PD_QSIZE,sp->begin_dpd));
	HCLOG(HCLOG_LOW, ("hctlr_bld_dstq: head 0x%x tail 0x%x end 0x%x\n",
                        	sp->dpd_qhead,sp->dpd_qtail,pd_q));
        return(D_SUCCESS);
}


/*
 * Process this request, and build packet descriptor for controller.
 * 
 * CALLED BY: hctlr_src_q(), hctlr_send()
 *
 * NOTE: must be called at SPLHIPPI 
 *
 * Returns on success
 *      none         
 *
 */
void
hctlr_src_pd(src_pd,src_req)
        src_pd_t     *src_pd;
	hctlr_src_t  *src_req;   /* request from a ULP driver */
{
	hctlr_softc_t   *sp = &hctlr_softc;
        int     	nentries,i,sg_pgcnt;
	u_char		*sgp;
	u_int		*sg_cont;
	u_int		length;

        HCLOG_ENTRY(4, ("hctlr_src_pd(src_pd=0x%x,src_req=0x%x)\n",src_pd,src_req));


        /*
         * Get the information from ULP request.
         * Store the length of packet, sglist
         * and reference to the controller PD block.
         */
        src_pd->src_req = src_req;
        src_pd->ifield = src_req->i_field;
        src_pd->fb_ptr = src_req->fb_ptr;
        src_pd->fb_len = src_req->fb_len;
        src_pd->chan_ctl = src_req->chan_ctl | SRC_CTL_BUSY; 
        src_pd->chan_err = 0; 

	/* MPPC or CONT support, no auto-disconnect */
	if (sp->src_state & (HCTLR_SRC_MPPC | HCTLR_SRC_CONT))
		src_pd->chan_ctl |= SRC_CTL_NDSC;


	/* Only if the device write is not local flush the MCP
	 * to prevent any possible cache-hits during controller
	 * DMA reads. PTS #9218. Check for existing ior and reply_port
	 * first since local driver generated IO may not contain
	 * ior nor reply_port.
	 */
	HCLOG(HCLOG_LOW,("hctlr_src_pd: check io_reply_port\n"));
	if (src_req->ior) {
		HCLOG(HCLOG_LOW,("hctlr_src_pd: ior=%x\n",src_req->ior));
		if ((src_req->ior->io_reply_port) && 
			(!(src_req->ior->io_op & IO_INTERNAL))) {
			HCLOG(HCLOG_LOW,("hctlr_src_pd: port=%x\n",
						src_req->ior->io_reply_port));
        		if (DIPC_IS_PROXY(src_req->ior->io_reply_port)) {
				HCLOG(HCLOG_LOW,("hctlr_src_pd: io_reply_port REMOTE!\n"));
				src_pd->chan_ctl |= SRC_CTL_FLMCP;
			}
		}
	}
	HCLOG(HCLOG_LOW,("hctlr_src_pd: check io_reply_port done!\n"));

	/* Set length and i960 cache control for this SRC PD.
	 * if total transmit request is less then cache_limit
	 * or if first burst only then tell i960 to make it 
	 * cache coherent instead of flushing
	 */
	if (src_req->sg_ptr) {
		src_pd->sg_ptr = (io_sglist_t)kvtophys(src_req->sg_ptr);
        	src_pd->sg_len = src_req->sg_ptr->iosg_hdr.length;
		length = src_pd->sg_len + src_pd->fb_len;
		if (length < hippi_cache_limit) 
                	src_pd->chan_ctl |= SRC_CTL_CACHE;
		else
			src_pd->chan_ctl &= ~SRC_CTL_CACHE;

		/* check if the list extends past a page and if so
		 * set up continuation pointers list for i960 to use
                 * to cross page boundaries. We must create a list
		 * of physical page lists for the i960 since the sglist
                 * cannot be wired down in continuous physical pages.
                 */
		sgp = (u_char *)src_req->sg_ptr; /* use virtual address */
		nentries = src_req->sg_ptr->iosg_hdr.nentries;

		if (IO_SGLIST_SIZE(nentries) > PAGE_SIZE) {
			/* more than a page, build continuation ptr list */
			HCLOG(HCLOG_LOW,(" building SG continuation ptr list\n"));
			/* one 32bit ptr per extra page */
			sg_pgcnt = (round_page(IO_SGLIST_SIZE(nentries))/PAGE_SIZE)-1;
			sg_cont = (u_int *)src_pd->sg_cont_ptrv;  
			HCLOG(HCLOG_LOW,(" sg_cont 0x%x\n",sg_cont));
			HCLOG(HCLOG_LOW,(" sg_ptr_size (bytes) %d\n",sg_pgcnt * sizeof(int)));
			/* initialize phys continuation ptr list */	
			for (i = 0;i < sg_pgcnt;i++) {
				sgp += PAGE_SIZE;
				sg_cont[i] = (u_int) kvtophys(sgp);
				HCLOG(HCLOG_LOW,(" sg_cont[%d] = 0x%x(0x%xv)\n",
							i,sg_cont[i],sgp));
			}
			src_pd->sg_cont = (u_int *)kvtophys(sg_cont);
			src_pd->sg_cont_size = sg_pgcnt * sizeof(int);
		}
	}	
        else {
		/* always cache a small 1st burst request size */
		length = src_pd->fb_len;
                src_pd->chan_ctl |= SRC_CTL_CACHE;
	}

	/* total data length to transfer */
        src_req->xfer_len = length;

	HCLOG(HCLOG_LOW,("hctlr_src_pd: exiting!\n"));

	return;
}


/*
 * Queue up data for HiPPI controller SRC channel transfer
 * CALLED BY HIPPI ULP DRIVERS
 *
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR - controller is not responding.
 */
hctlr_src_q(src_req)
	hctlr_src_t  *src_req;   /* request from a ULP driver */
{
	hctlr_softc_t   *sp = &hctlr_softc;
        src_pd_t        *curpd;
        src_pd_t        *newpd;
        int     	oldpri;
#ifdef HCTLR_DEBUG
	hctlr_src_t  	*sreq;   /* requests already on queue  */
#endif HCTLR_DEBUG

        HCLOG_ENTRY(5, ("hctlr_src_q(src_req=0x%x)\n",src_req));

        /* Sanity check of controller interface state */
        if (!(sp->state & HCTLR_UP))  {
                return (D_IO_ERROR);
        }

	/* if interface is in EXCL state, check key */
        if ((sp->state & HCTLR_EXCL) && (src_req->excl_key != sp->excl_key))
                return (D_INVALID_OPERATION);

        oldpri = SPLHIPPI();

        HCLOG(HCLOG_LOW,("hctlr_src_q: SRQ %x cci %x fb,len %x,%d ior %x sg_ptr %x ctl %x\n",
         		  src_req,htonl(src_req->i_field),src_req->fb_ptr,src_req->fb_len,
			  src_req->ior,src_req->sg_ptr,src_req->chan_ctl));

	/* put on HiPPI SRC request queue tail */
        simple_lock(&sp->src_req_lock);
	if (queue_empty(&sp->src_reqs)) {
      		HCLOG(HCLOG_HIGH, ("hctlr_src_q: empty put on head!\n"));
		enqueue_head(&sp->src_reqs,src_req);
	}
	else {
#ifdef HCTLR_DEBUG
		queue_iterate(&sp->src_reqs, sreq, hctlr_src_t*, links)
        	{
			if (sreq == src_req) {
				panic("hctlr_src_q: SRC_REQ already queued!");
			}
		}
#endif HCTLR_DEBUG
      		HCLOG(HCLOG_HIGH, ("hctlr_src_q: not empty put on tail!\n"));
		enqueue_tail(&sp->src_reqs,src_req);
	}
        simple_unlock(&sp->src_req_lock);

	/* SRC PD queue is full or other request are pending, return */
        if ((sp->spd_qtail->next_pdv == sp->spd_qhead) ||
			(sp->src_state & HCTLR_SRC_PENDING)) {
		src_req->chan_ctl |= SRC_CTL_PENDING;
		sp->src_state |= HCTLR_SRC_PENDING;	
                HCLOG(HCLOG_LOW, ("hctlr_src_q: PD queue full\n"));
                HCSTATS(sp, src.pq_full++);
		splx(oldpri);
		return (D_SUCCESS);
        }

        /*
         *  Set new and current SRC PD queue
         *  pointers checking for empty queue
         *  condition (spd_qhead == 0).
         */
        newpd = curpd = sp->spd_qtail;
        if (sp->spd_qhead == 0) {
                 sp->spd_qhead = (src_pd_t *)newpd;
                 HCSTATS(sp, src.qmiss++);
        } else {
                 newpd = curpd->next_pdv;
                 HCSTATS(sp, src.qhit++);
        }

	assert(newpd->chan_ctl == 0);
	assert(((int)src_req->sg_ptr & (PAGE_SIZE-1)) == 0);

	/* process this request, package up the packet descriptor */
	hctlr_src_pd(newpd,src_req);

        HCLOG(HCLOG_LOW,("hctlr_src_q: PDQ %x cci %x ctl %x sg %x fb %x fb_len %d sg_len %d\n",
         		  newpd,htonl(newpd->ifield),newpd->chan_ctl,newpd->sg_ptr,
			  newpd->fb_ptr,newpd->fb_len,newpd->sg_len));

        /*
         *  Set up new tail pointer for PD queue
	 *  and call hctlr_send to kick start controller
         */
        if (newpd != curpd)
                sp->spd_qtail = (src_pd_t *)newpd;

	hctlr_send(sp);

	splx(oldpri);
	return (D_SUCCESS);
}


/*
 * Check for transmit data on SRC PD and give
 * it to controller for SRC channel output. Also check
 * the hctlr_src queue for any pending io that needs
 * to be added to SRC PD list if PD list has space.
 *
 * NOTE: must be called at SPLHIPPI 
 *
 * CALLED BY: hctlr_src_i(), hctlr_init()
 *
 * Returns on success
 *      none         
 *
 * Possible errors
 *      interface is down and not responding.
 *      NOTE: watchdog timer will take care of this condition.
 */
void
hctlr_send(sp)
	hctlr_softc_t	*sp;
{
	hctlr_src_t	*src_req;
	src_pd_t	*pdq;
	int		chain_size,flush_ap,flush_mcp;

        HCLOG_ENTRY(6, ("hctlr_send(sp=0x%x)\n",sp));

        /*
	 * check for empty queue
	 */
	if (sp->spd_qhead == 0) {
		  sp->spd_qchain = 0;
		  HCLOG(HCLOG_LOW, ("hctlr_send: queue empty, nothing to do.\n"));
		  HCSTATS(sp, src.pq_null++);
		  return;
	}

        /*  Start the controller on what's queued up. Set last bit in chain.
         *  If the controller is currently to busy for a new request (indicated
         *  by non-zero i960_scb->src_pd) just return and let the completion
         *  interrupt pickup queued requests. If the i960 is still working on
         *  the first chain (spd_qchain != 0) and it can take another PD ptr then
         *  give it the PD after the end of the existing chain (spd_chain->next_pd)
         *  if the next PD has a valid request to give.
         *  NOTE: the driver must flush i860 cache since the controller is not
         *        cache-coherent on DMA xfers. The i960 on the controller is
         *        cache-coherent and can do some cache-coherency work for smaller
         *        size requests. Currently the controller has been profiled
         *        and optimized on I/O requests up to 16KBytes. This is the default
         *        set in hippi_cache_limit. It can be change via bootmagic.
         */

	flush_mcp = flush_ap = chain_size = 0;
        if (sp->scb->src_pd == 0)  {
        	/* i960 is ready for first chain */
        	if (sp->spd_qchain == 0) {
			/* check for flushing and maximum i960 chain size */
                	for (pdq = sp->spd_qhead; pdq != sp->spd_qtail->next_pdv; 
				pdq = pdq->next_pdv) {
				chain_size++;
				/* to flush or not to flush */ 
				/* #9218, flush message co-processor */
				if (pdq->chan_ctl & SRC_CTL_FLMCP) {
					flush_mcp++;
				}
				if (!(pdq->chan_ctl & SRC_CTL_CACHE)) {
					flush_ap++;
				}
				else {
					/* flushing, don't tell i960 to make coherent */
					if (flush_ap)
						pdq->chan_ctl &= ~SRC_CTL_CACHE;
				}
				/* hit tail or maximum i960 chain size */
				if ((chain_size == HCTLR_SRC_CHAIN_MAX) ||
						(pdq == sp->spd_qtail)) {
					break;
				}
			}
#ifdef HCTLR_STATS
                	if (chain_size > 1) {
                        	HCSTATS(sp, src.chain++);
				if (chain_size > sp->stats.src.chain_max)
					HCSTATS(sp, src.chain_max = chain_size);
			}
#endif HCTLR_STATS
			/* mark end of packet chain for i960 */
			pdq->chan_ctl |= SRC_CTL_LAST;
			if (flush_ap) {
			    flush();
			}
			/* flush MCP, PTS #9218 */
			if (flush_mcp) {
#ifdef HCTLR_DEBUG
			    flush_mcp_cnt++;
#endif HCTLR_DEBUG
			    call_mcmsg_dflush_interrupt();
			}
                	sp->scb->src_pd = (struct src_pd *)kvtophys(sp->spd_qhead);
                	sp->spd_qchain = pdq;
        		sp->src_timer = hctlr_timer;
                	HCLOG(HCLOG_LOW, 
				("hctlr_send: 1st chain, %x(%x) to %x(%x)\n",
                        	sp->spd_qhead,kvtophys(sp->spd_qhead),
				pdq,kvtophys(pdq)));
		}
        	/* i960 is ready for next chain */
        	else if ((sp->spd_qchain) && (sp->spd_qchain != sp->spd_qtail)) {
			/* check for flushing and maximum i960 chain size */
                	for (pdq = sp->spd_qchain->next_pdv; pdq != sp->spd_qtail->next_pdv; 
				pdq = pdq->next_pdv) {
				chain_size++;
				/* to flush or not to flush */ 
				/* also, flush MCP, PTS #9218 */
				if (pdq->chan_ctl & SRC_CTL_FLMCP) {
					flush_mcp++;
				}
				if (!(pdq->chan_ctl & SRC_CTL_CACHE)) {
					flush_ap++;
				}
				else {
					/* flushing, don't tell i960 to make coherent */
					if (flush_ap)
						pdq->chan_ctl &= ~SRC_CTL_CACHE;
				}
				/* hit tail or maximum i960 chain size */
				if ((chain_size == HCTLR_SRC_CHAIN_MAX) ||
						(pdq == sp->spd_qtail)) {
					break;
				}
			}
#ifdef HCTLR_STATS
                	if (chain_size > 1) {
                        	HCSTATS(sp, src.chain++);
				HCSTATS(sp, src.chain_max = chain_size);
			}
#endif HCTLR_STATS
			/* mark end of packet chain for i960 */
			pdq->chan_ctl |= SRC_CTL_LAST;
			if (flush_ap) {
			    flush();
                        }
			/* flush MCP, PTS #9218 */
			if (flush_mcp) {
#ifdef HCTLR_DEBUG
			    flush_mcp_cnt++;
#endif HCTLR_DEBUG
			    call_mcmsg_dflush_interrupt();
			}
                	sp->scb->src_pd = sp->spd_qchain->next_pd;
                	HCLOG(HCLOG_LOW, 
				("hctlr_send: next chain, %x(%x) to %x(%x)\n",
                  		sp->spd_qchain->next_pdv,sp->spd_qchain->next_pd,
				pdq,kvtophys(pdq)));
                	sp->spd_qchain = pdq;
        		sp->src_timer = hctlr_timer;
        	}
		else {
			HCLOG(HCLOG_LOW,
				("hctlr_send: i960 has all resources, 2 chains!\n"));
		}
	}
	else {
                HCLOG(HCLOG_LOW, ("hctlr_send: i960 busy\n"));
                HCSTATS(sp, src.busy++);
        }

        /*
	 * Now that the i960 is busy transfering,
	 * pick up any new requests if any exist and 
	 * there is room in SRC PD link list.
         */
	if ((sp->src_state & HCTLR_SRC_PENDING) && 
			(sp->spd_qtail->next_pdv != sp->spd_qhead)) {	
		/* start at next available PD */
		pdq = sp->spd_qtail->next_pdv;
		simple_lock(&sp->src_req_lock);
		queue_iterate(&sp->src_reqs, src_req, hctlr_src_t*, links)
            	{
			if (src_req->chan_ctl & SRC_CTL_PENDING) {
				/* more pending and no PD space */
				if (pdq == sp->spd_qhead) {
					sp->src_state |= HCTLR_SRC_PENDING;
					break;
				}

				assert(pdq->chan_ctl == 0);
				assert(((int)src_req->sg_ptr & (PAGE_SIZE-1)) == 0);

				/* request no longer pending  */
				src_req->chan_ctl &= ~SRC_CTL_PENDING;

				/* process this request, build packet descriptor */
				hctlr_src_pd(pdq,src_req);

        			HCLOG(HCLOG_LOW,("hctlr_send: PD %x cci %x ctl %x \
						sg %x fb %x len 0x%x\n",
                          			pdq,htonl(pdq->ifield),pdq->chan_ctl,
						pdq->sg_ptr,pdq->fb_ptr,src_req->xfer_len));

				/* 
				 * set up new tail pointer and channel state
				 */
				sp->spd_qtail = pdq;
				pdq = pdq->next_pdv;
				sp->src_state &= ~HCTLR_SRC_PENDING;
			} 	/* if this src request is pending */
		} 	/* queue iterate, SRC pds are available */ 
		simple_unlock(&sp->src_req_lock);
	} 	/* if SRC request is pending */
	return;
}


/*
 * Give i960 DST packet descriptor resources with
 * the first burst resource buffers. If there is
 * no filters set (channel state = CLOSED) then
 * don't give the controller any resources. This
 * will prevent unnecessary DST interrupts if a
 * remote system is trying to connect without any
 * ULP async filters set.
 *
 * NOTE: must be called at SPLHIPPI 
 *
 * CALLED BY: hctlr_dst_i(), hctlr_init()
 *
 * Returns on success
 *      none         
 *
 * Possible errors
 *      interface is down and not responding.
 */
void
hctlr_recv(sp)
	hctlr_softc_t	*sp;
{
        dst_pd_t        *curpd;
        dst_pd_t        *newpd;
        int             chain_size = 0;
        HCLOG_ENTRY(7, ("hctlr_recv()\n"));

	if (sp->dst_state & HCTLR_DST_CLOSED) {
                HCLOG(HCLOG_LOW, ("hctlr_recv: no DST filters set\n"));
		return;
	}

        /*
         *  If queue is full and i960 has both chain(s) of buffers
         *  there is no need to get more, return.
         */
        if ((sp->dpd_qtail->next_pdv == sp->dpd_qhead) &&
        	(sp->dpd_qchain == sp->dpd_qtail) && 
		(sp->dpd_qtail->chan_ctl & DST_CTL_LAST)) {
               	HCLOG(HCLOG_LOW, ("hctlr_recv: i960 has all resources\n"));
        	return;
        }

        /*
         *  While there is room in the receive PD queue and the chain
         *  does not exceed the chain_max, then the first burst resources
         *  in the PD's are pre-allocated by the driver at init time.
         */
        while ((sp->dpd_qtail->next_pdv != sp->dpd_qhead) &&
                (chain_size != HCTLR_RECV_CHAIN_MAX)) {

        	/*
         	 * Build chain and set new PD queue ptrs checking
	 	 * for empty queue condition (dpd_qhead == 0).
        	 * if the queue is empty then we are starting new
		 * new PD chains. Set dpd->chain=0 to enable the
         	 * driver to give the i960 two chains to work on 
		 * since it can process up 2 lists at a time.
         	 */
        	if (sp->dpd_qhead == 0) {
                	sp->dpd_qhead = sp->dpd_qtail;
                	sp->dpd_qchain = 0;
                	HCSTATS(sp, dst.qmiss++);
        	} else {
                	sp->dpd_qtail = sp->dpd_qtail->next_pdv;
                	HCSTATS(sp, dst.qhit++);
        	}
                chain_size++;
        }

        /*
         *  Give resources to the HIPPI destination channel. Set last bit in chain.
         *  If the controller is currently to busy for a new resource (indicated
         *  by non-zero i960_scb->dst_pd) just return and let the completion
         *  interrupt pickup more resources. If the i960 is still working on
         *  the first chain (dpd_qchain != 0) and it can take another PD ptr then
         *  give it the PD after the end of the existing chain (dpd_chain->next_pd)
         *  if there is another resource available.
         */
	/* If channel state was set to halt, then do not give i960 resources
	 * This state is set and unset via ULP drivers to hold off DST traffic 
	 */
	if (sp->dst_state & HCTLR_DST_HALT) {
                HCLOG(HCLOG_LOW, ("hctlr_recv: DST channel halted\n"));
		return;
	}

        if (sp->scb->dst_pd != 0) {
                HCLOG(HCLOG_LOW, ("hctlr_recv: i960 busy\n"));
                HCSTATS(sp, dst.busy++);
                return;
        }

        /* additional chains to the i960 */
        if ((sp->dpd_qchain) && (sp->dpd_qchain != sp->dpd_qtail)) {
                sp->dpd_qtail->chan_ctl |= DST_CTL_LAST;
                sp->scb->dst_pd = sp->dpd_qchain->next_pd;
                HCLOG(HCLOG_HIGH,("hctlr_recv: another chain to i960, %x(%x)\n",
                                   sp->dpd_qchain->next_pdv,sp->dpd_qchain->next_pd));
                sp->dpd_qchain = sp->dpd_qtail;
        }
        /* first chain to the i960 */
        else if (sp->dpd_qchain == 0) {
                sp->dpd_qtail->chan_ctl |= DST_CTL_LAST;
                sp->scb->dst_pd = (struct dst_pd *)kvtophys(sp->dpd_qhead);
                HCLOG(HCLOG_HIGH, ("hctlr_recv: first chain to i960, %x(%x)\n",
                                      sp->dpd_qhead,kvtophys(sp->dpd_qhead)));
                sp->dpd_qchain = sp->dpd_qtail;
                HCLOG(HCLOG_HIGH, ("hctlr_recv: chain size = %d\n",chain_size));
	/* one BIG chain with all PD's */
        } else {
                sp->dpd_qtail->chan_ctl |= DST_CTL_LAST;
                sp->scb->dst_pd = (struct dst_pd *)kvtophys(sp->dpd_qhead);
                sp->dpd_qchain = sp->dpd_qtail;
                HCLOG(HCLOG_HIGH,("hctlr_recv: FULL chain(%d), sent to i960\n",
					DST_PD_QCNT));
                HCLOG(HCLOG_HIGH, ("hctlr_recv: PD's %x(%x) to %x(%x)\n",
                                    sp->dpd_qhead,kvtophys(sp->dpd_qhead),
				    sp->dpd_qtail,kvtophys(sp->dpd_qtail)));
        }
        return;
}


/*
 * Give the HiPPI controller a io_sglist pointer for DST data
 * CALLED BY HIPPI ULP DRIVERS
 *
 *   sg_ptr set to NULL will force controller to abort DST connection.
 *   This routine must build an extension list of physical pointers if
 *   the sglist provided extends beyond a page.
 */
void
hctlr_recv_buffer(sg_list)
	io_sglist_t  sg_list;     /* virtual pointer to sglist */
{
	hctlr_softc_t   *sp = &hctlr_softc;
	u_char		*sgp;
	u_int		*sg_cont;
	u_int		i,sg_pgcnt;
        HCLOG_ENTRY(8, ("hctlr_recv_buffer(sg_list=0x%x)\n",sg_list));
	
	if (sp->dst_state & HCTLR_DST_CLOSED)
		return;

	assert(sp->dst_req.pkt_done != TRUE);

	if (sg_list) {
		/* data must be out of i860 cache before controller DMA */
        	flush();
		HCLOG(HCLOG_LOW,("hctlr_recv_buffer: sg 0x%xv(0x%xp) len %x nentries %x, 1st 0x%x,%x\n",
					sg_list, kvtophys(sg_list), 
					sg_list->iosg_hdr.length, 
					sg_list->iosg_hdr.nentries,
					sg_list->iosg_list[0].iosge_phys,
					sg_list->iosg_list[0].iosge_length));

		/* check if the list extends past a page and if so
		 * set up continuation pointers list for i960 to use
                 * to cross page boundaries. We must create a list
		 * of physical page lists for the i960 since the sglist
                 * cannot be wired down in continuous physical pages.
                 */

		if (IO_SGLIST_SIZE(sg_list->iosg_hdr.nentries) <= PAGE_SIZE) {
			sp->scb->dst_sg_cont = 0;
			sp->scb->dst_sg_ptr = (io_sglist_t)kvtophys(sg_list);
		}
		else {
			/* more than a page, build continuation ptr list */
			HCLOG(HCLOG_LOW,(" building SG continuation ptr list\n"));

			sgp = (u_char *)sg_list;
                        /* one 32bit ptr per extra page */
			sg_pgcnt = (round_page(IO_SGLIST_SIZE(sg_list->iosg_hdr.nentries))/PAGE_SIZE)-1;
			sg_cont = (u_int *)sp->dpd_qhead->sg_cont_ptrv;

			HCLOG(HCLOG_LOW,(" sg_cont 0x%x\n",sg_cont));
			HCLOG(HCLOG_LOW,(" sg_ptr_size (bytes) %d\n",sg_pgcnt * sizeof(int)));
			
			for (i = 0;i < sg_pgcnt;i++) {
				sgp += PAGE_SIZE;
				sg_cont[i] = (u_int)(kvtophys(sgp));
				HCLOG(HCLOG_LOW,(" sg_cont[%d] = 0x%x(0x%xv)\n",
							i,sg_cont[i],sgp));
			}
			sp->scb->dst_sg_cont = (u_int *)kvtophys(sg_cont);
			sp->scb->dst_sg_cont_size = sg_pgcnt * sizeof(int);
			sp->scb->dst_sg_ptr = (io_sglist_t)kvtophys(sg_list);
		}

	}
	else {
		sp->scb->ctlr_cmd |= HIPPI_DST_ABORT;
	}

	/* timer for expected controller interrupt */
	sp->dst_timer = hctlr_timer;
	return;
}


/*
 * Set a filter for async input data on the DST channel
 * CALLED BY HIPPI ULP DRIVERS
 *
 * Returns on success
 *      D_SUCCESS
 *
 * Possible errors
 *      D_IO_ERROR          - controller is not responding.
 *      D_INVALID_OPERATION - filter is invalid or already exists
 */
hctlr_recv_filter(new_dfp,flag)
        hctlr_filter_t	*new_dfp;    	/* pointer to new filter      */
        boolean_t  	flag;          	/* set or unset the filter    */
{
	hctlr_softc_t   *sp = &hctlr_softc;
	hctlr_filter_t  *dfp;
        int     	empty,match;
        int 		oldpri = SPLHIPPI();

        HCLOG_ENTRY(9, ("hctlr_recv_filter(0x%x,%x)\n",new_dfp,flag));

        /* Sanity check of controller interface state */
        if (!(sp->state & HCTLR_UP)) {
		splx(oldpri);
                return (D_IO_ERROR);
	}

	/* if interface is in EXCL state, check key */
        if ((sp->state & HCTLR_EXCL) && (new_dfp->excl_key != sp->excl_key)) {
        	HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: EXCL, wrong key\n"));
		splx(oldpri);
                return (D_INVALID_OPERATION);
	}

	/* for promiscuos mode, must be EXCL */
	if ((new_dfp->ulp == 0) && (!(sp->state & HCTLR_EXCL))) {
        	HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: ulp=0 and !EXCL\n"));
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	/* only one filter allowed in promiscuous mode */
	if ((flag) && (sp->dst_state & HCTLR_DST_PROM)) {
        	HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: set, already in PROM mode\n"));
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	/* driver is not in CONT mode and the controller is  */
        if (!(sp->dst_state & HCTLR_DST_CONT) && 
	      ((sp->scb->src_cmd & HIPPI_SRC_CONTINUATION) ||
	       (sp->scb->dst_cmd & HIPPI_DST_CONTINUATION))) {
            	printf("hctlr_recv_filter: ERROR: CTLR still in CONT mode!\n");
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	empty = TRUE;
	match = FALSE;

        HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: fp %x ulp %02x of %x bs %x mn %x mx %x\n",
				new_dfp,new_dfp->ulp,new_dfp->offset,new_dfp->bsize,
				new_dfp->min, new_dfp->max));
        HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: callback routine (ddone) -> 0x%x\n",
				new_dfp->ddone));

	/* walk the queue and look for existing filter  */
	simple_lock(&sp->dst_filter_lock);
	queue_iterate(&sp->dst_filters, dfp, hctlr_filter_t*, links)
	{
		empty = FALSE;
		if (flag) { 			/* setting new filter */
			if (new_dfp->offset == 0) {	/* ulp only */
				/* looking for ulp only filter */
				if (new_dfp->ulp == dfp->ulp) {
					match = TRUE;
					break;
				}
			} else {		/* ulp + offset data */
				/* looking for ulp + offset data filter */
				if ((new_dfp->ulp == dfp->ulp) &&
				    (new_dfp->offset == dfp->offset) &&
				    (new_dfp->bsize == dfp->bsize)) { 
					/* check data range */
					if (((new_dfp->min >= dfp->min) &&
					     (new_dfp->min <= dfp->max)) ||
					    ((new_dfp->max >= dfp->min) &&
					     (new_dfp->max <= dfp->max))) {
						match = TRUE;
						break;
					}
				}
			}
		} else {			/* removing a filter */
			if (new_dfp == dfp) {
				/* found reference to filter data */
				if ((new_dfp->ulp == dfp->ulp) &&
				    (new_dfp->offset == dfp->offset) && 	
				    (new_dfp->bsize == dfp->bsize) && 	
				    (new_dfp->min == dfp->min) && 	
				    (new_dfp->max == dfp->max)) { 	
						match = TRUE;
						break;
				} else {
					break;
				}
			}
		}		
	}			
	simple_unlock(&sp->dst_filter_lock);

	/* 
	 * check for invalid operations 
	 *	- removing filter with nothing on list
	 *      - no match found when removing filter
	 * 	- offset greater than burst size when setting
	 *	- duplicate found when setting filter
	 *      - setting ulp=0 and not empty
	 */
	if (((empty) && (!flag)) || ((!match) && (!flag)) ||
		((flag) && (new_dfp->offset > HIPPI_BURST_SIZE)) ||
		((match) && (flag)) || (((flag) && (!new_dfp->ulp)) && !empty)) {

        	HCLOG(HCLOG_HIGH,("hctlr_recv_filter: FAILED, set(%d),\
        			empty(%d),match(%d),offset(%d),ulp(%d)\n",
				flag,empty,match,new_dfp->offset,new_dfp->ulp));
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	/* set or remove filter and set proper DST channel state */ 
	simple_lock(&sp->dst_filter_lock);
	if (flag) {
        	HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: filter 0x%x added\n",new_dfp));
                HCSTATS(sp, filters++);
		if (queue_empty(&sp->dst_filters)) 
			enqueue_head(&sp->dst_filters, new_dfp);
		else
			enqueue_tail(&sp->dst_filters, new_dfp);

		/* if this is the first filter, open the channel */
		if (empty) {
        		HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: dst_state = OPEN\n"));
			sp->dst_state &= ~HCTLR_DST_CLOSED;
		}
		/* if ulp == 0 set DST channel in promiscuous mode */
		if (new_dfp->ulp == 0) 
			sp->dst_state |= HCTLR_DST_PROM;
	}
	else { 
        	HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: filter 0x%x removed\n",new_dfp));
		queue_remove(&sp->dst_filters, new_dfp, hctlr_filter_t*, links);
                HCSTATS(sp, filters--);
		if (queue_empty(&sp->dst_filters)) {
        		HCLOG(HCLOG_HIGH, ("hctlr_recv_filter: dst_state = CLOSED\n"));
			sp->dst_state |= HCTLR_DST_CLOSED;
		}

		/* if ulp == 0 reset DST channel out of promiscuous mode */
		if (new_dfp->ulp == 0) 
			sp->dst_state &= ~HCTLR_DST_PROM;
	}			
	simple_unlock(&sp->dst_filter_lock);

	hctlr_recv(sp);
	splx(oldpri);
	return (D_SUCCESS);
}


/*
 * hctlr_ctr_i(unit)
 *
 * This routine will be called for all controller type interrupts
 * including initialization, controller errors and status, etc.
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler                              
 *
 * Returns on success
 * 	nothing   
 *
 * Possible errors
 *      - SRC or DST interrupt before initialization
 */
void
hctlr_ctr_i(unit,intr)
	u_int	unit;
	unsigned short intr;
{
	hctlr_softc_t  *sp = &hctlr_softc;
        u_int           bderr,bdsts,dmsg_check,i;

     /*   HCLOG_ENTRY(10, ("hctlr_ctr_i(unit=%d,intr=%x)\n",unit,intr)); */

        /*
         * Process controller request, errors first, then status.
         * force reset via watchdog with timer if necessary.
         */

         HCSTATS(sp, ctlr_int++);
         bderr = sp->scb->ctlr_err;
         bdsts = sp->scb->ctlr_sts;

       /*  HCLOG(HCLOG_LOW,("hctlr_ctr_i: err=%x,sts=%x\n",bderr,bdsts)); */

         if (bderr) {
                 if (bderr & HIPPI_BAD_PARITY) {
                         printf("%d: HiPPI, bad parity on controller board.\n",
                                 sp->node_id);
                         sp->src_timer = 0;
                         sp->dst_timer = 0;
                 }
                 if (bderr & HIPPI_INIT_FAIL) {
                         printf("%d: HiPPI ERROR, controller init failed.\n",
                                 sp->node_id);
                         sp->src_timer = 0;
                         sp->dst_timer = 0;
                 }
                 if (bderr & HIPPI_SRC_INTC_DE) {
                         HCSTATS(sp, src.nointc++);
                 }
                 if (bderr & HIPPI_DST_INTC_DE) {
                         HCSTATS(sp, dst.nointc++);
                 }
         }
         if (bdsts) {
                 if (bdsts & HIPPI_INIT_PASS) {
                        HCSTATS(sp, init++);
                 	if (bdsts & HIPPI_SRC_INTC_AS)
                         	HCSTATS(sp, srci++);
			else
				printf("HiPPI Warning: No Cable detected on SRC Channel\n");

                 	if (bdsts & HIPPI_DST_INTC_AS)
                         	HCSTATS(sp, dsti++);
			else
				printf("HiPPI Warning: No Cable detected on DST Channel\n");
                 }

                 if (bdsts & HIPPI_DMSG_RDY) {
                         HCSTATS(sp, dmsg_int++);
                         /* check to see if i960 gave me a good address  */
                         dmsg_check = (u_int)(sp->scb->debug_msg);
                         if ((sp->scb->debug_msg == 0) ||
                                 ((dmsg_check & 0x82000000) != 0x82000000)) {
                                 printf("%d: HiPPI debug message, invalid address\n",
                                                 sp->node_id);
                         } else {
                                 /* change the brd phys to virtual offset */
                                 dmsg_check &= 0x0000ffff;
                                 dmsg_check += (u_int) (sp->scb);
                                 dmsg_ptr = (caddr_t) dmsg_check;
                                 for (i=0;i<HIPPI_DMSG_MAX,(*dmsg_ptr!=0);i++,dmsg_ptr++)
                                         dmsg_print[i] = *dmsg_ptr;
                                 dmsg_print[i] = 0;
                                 printf("%s",dmsg_print);
                                 sp->scb->debug_msg = 0;
                         }
                 }
         }

	/* clear controller status and ack interrupt */
        sp->scb->ctlr_err = 0;
        sp->scb->ctlr_sts = 0;
        outl(HIPPI_INTR_STATUS, intr & ~HIPPI_CTLR_INTR);

        return;
}


/*
 * hctlr_src_i(unit)
 *
 * This routine will be called to service SRC channel interrupts
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler                              
 *
 * Returns on success
 * 	nothing   
 *
 * Possible errors
 *      - interrupt before initialization
 */
void
hctlr_src_i(unit,intr)
	u_int	unit;
	unsigned short  intr;
{
	hctlr_softc_t  	*sp = &hctlr_softc;
        register  src_pd_t     *pdq;
        register  hctlr_src_t  *srq;
        hctlr_src_t     *hd_srq;
        int             count = 0;
#ifdef HCTLR_DEBUG
        int             i;
        u_char          *data;
#endif HCTLR_DEBUG

        HCLOG_ENTRY(11, ("hctlr_src_i(unit=%d,intr=%x)\n",unit,intr));

	HCSTATS(sp, src.intr++);

        /*  Acknowledge interrupt */
        outl(HIPPI_INTR_STATUS, intr & ~HIPPI_SRC_INTR);  

        /* Sanity check of controller interface state */
        if (!(sp->state & HCTLR_UP))  {
                return;
        }

        /*
         *  Get the SRC queue head.
         */
        pdq = sp->spd_qhead;

	/* get request from the head of hctlr_req queue */
        simple_lock(&sp->src_req_lock);
	srq = (hctlr_src_t *)dequeue_head(&sp->src_reqs);
        simple_unlock(&sp->src_req_lock);

	assert(pdq != 0);
	assert(srq != 0);

        HCLOG(HCLOG_LOW, ("hctlr_src_i: SREQ %x: prev %x next %x\n",
				srq,srq->links.prev,srq->links.next));

        /*
	 *  SRC interrupts from the i960 will always be issued after
	 *  completed chains to reduce the amount of service interrupts.
         *  Service the PD queue and the SRC_REQ queue until we hit 
	 *  the last packet in the SRC PD chain. 
         */
        while (pdq) {
                sp->src_timer = -1;                 /* xmit command longer pending */
		count++;

#ifdef HCTLR_DEBUG
		if (srq != pdq->src_req) {
			printf("hctlr_src_i: PD (%x) and REQ (%X) out of sync\n",
						pdq->src_req,srq);
			panic("hctlr_src_i: src PD and src REQ out of sync");
		}
#endif HCTLR_DEBUG

		if (count == 1) 		/* callback reference for chaining */
			hd_srq = srq;
		
                HCLOG(HCLOG_LOW,("hctlr_src_i: PD 0x%x, REQ 0x%x complete, error=%x\n",
                                    pdq,srq,pdq->chan_err));
                HCLOG(HCLOG_LOW,("hctlr_src_i: PD->sg_ptr 0x%x, REQ->sg_ptr 0x%x\n",
                                    phystokv(pdq->sg_ptr),srq->sg_ptr));

		/* update request error_status */
		srq->error_status = pdq->chan_err;

                /*
                 *  Check and keep stats on errors. Packet was not sent.
                 */
                if (!pdq->chan_err) {
                        /*
                         *  Update stats if packet was good
                         */
                        HCSTATS(sp,src.packets++);
                        HCSTATS(sp,src.bytes += (pdq->fb_len + pdq->sg_len));
                } else {
                        HCSTATS(sp, src.errors++);

                        if (pdq->chan_err & SRC_ERR_INTC) {
                                HCSTATS(sp, src.nointc++);
				printf("HiPPI Warning: No Cable detected on SRC Channel\n");
			}

                        if (pdq->chan_err & SRC_ERR_SEQS)
                                HCSTATS(sp, src.seqs++);

                        if (pdq->chan_err & SRC_ERR_SEQD)
                                HCSTATS(sp, src.seqd++);

                        if (pdq->chan_err & SRC_ERR_PARI)
                                HCSTATS(sp, src.pari++);

                        if (pdq->chan_err & SRC_ERR_TOWR)
                                HCSTATS(sp, src.towr++);

                        if (pdq->chan_err & SRC_ERR_ALGN)
                                HCSTATS(sp, src.algn++);

                        if (pdq->chan_err & SRC_ERR_SWRD)
                                HCSTATS(sp, src.swrd++);
                }
#ifdef HCTLR_DEBUG
                HCLOG(HCLOG_LOW,("hctlr_src_i: ctl %x err %x fb_ptr %x fb_len %d sg_len %d\n",
                                pdq->chan_ctl,pdq->chan_err,pdq->fb_ptr,
				pdq->fb_len,pdq->sg_len));
		if (!pdq->fb_ptr) 
                	data = (u_char *)pdq->sg_ptr->iosg_list[0].iosge_phys;
		else
			data = (u_char *)pdq->fb_ptr;

                HCLOG(HCLOG_LOW,("hctlr_src_i: First 16 bytes = "));
                for (i=0;i<16;i++)
                 	HCLOG(HCLOG_LOW,("%02x ",(u_char *)data[i]));
                HCLOG(HCLOG_LOW,("\n"));

                /* keep statistics of overruns, remote too slow */
		if (pdq->chan_ctl & SRC_CTL_ORUN)
                        HCSTATS(sp,src.overrun++);
#endif HCTLR_DEBUG
		/* update SRC REQ xfer length */
		srq->xfer_len     = pdq->fb_len + pdq->sg_len;
	
		/* clean up SRC packet descriptor */
		pdq->ifield   = 0;
		pdq->fb_ptr   = 0;
		pdq->fb_len   = 0;
		pdq->sg_ptr   = 0;
		pdq->chan_err = 0;
		pdq->sg_len   = 0;
		pdq->sg_cont = 0;
		pdq->sg_cont_size = 0;
		pdq->src_req = 0;

                /*
                 *  Bump pointer to next command block checking
		 *  for end of queue or chain. Adjust head and
		 *  tail pointers. Get next request only 
		 *  if we are not at end of PD queue or chain.
		 *  If queue is empty, driver is starting new
		 *  PD chains (spd_qchain=0). This enables 
		 *  the driver to give the i960 two chains to 
		 *  work on since it can queue up 2 at a time.
                 */
        	if (pdq == sp->spd_qtail)  {
                	HCLOG(HCLOG_LOW,("hctlr_src_i: end of chain and list\n"));
			/* end of chain and list */
                	sp->spd_qtail = pdq->next_pdv;
			sp->spd_qchain = 0;
			sp->spd_qhead = 0;
			pdq->chan_ctl = 0;
			pdq = 0;
        	} 
		else if ((pdq->chan_ctl & SRC_CTL_LAST) == 0) {
                	HCLOG(HCLOG_LOW,("hctlr_src_i: more, get next REQ and PD\n"));
			/* more to do, clear ctl field, get next PD and SRC_REQ */
			pdq->chan_ctl = 0;
                        pdq = pdq->next_pdv;
			simple_lock(&sp->src_req_lock);
			srq = (hctlr_src_t *)dequeue_head(&sp->src_reqs);
        		simple_unlock(&sp->src_req_lock);
			assert(srq != 0);
		} 
		else {
                	HCLOG(HCLOG_LOW,("hctlr_src_i: end of this chain\n"));
			/* end of list, adjust head pointer and get out */
			sp->spd_qhead = pdq->next_pdv;
			pdq->chan_ctl = 0;
			pdq = 0;
		}
                /* 
		 * Callback the ULP sdone routine if next callback 
		 * routine is different or if this is last packet
		 * in the PD chain or list. This allows us to 
		 * callback with chained request if possible.
		 */
		if ((hd_srq->sdone != srq->sdone) || (!pdq)) {
                	HCLOG(HCLOG_HIGH,("hctlr_src_i: REQ->sdone, 0x%x(0,0x%x,%d)\n",
                                    hd_srq->sdone,hd_srq,count));
                	hd_srq->sdone(0,hd_srq,count);
			count = 0;
		} 
        }

        /*
         *  Pick up any queued requests that may have
	 *  been blocked or waiting for more queue space.
	 *  We are already running at SPLHIPPI for hctlr_send().
         */
        hctlr_send(sp);

	return;
}


/*
 * hctlr_dst_i(unit)
 *
 * This routine will be called to service DST channel interrupts
 *
 * Called by: hippi_intr()      master HiPPI interrupt handler                              
 *
 * Returns on success
 * 	nothing   
 *
 * Possible errors
 *      - interrupt before initialization
 */
void
hctlr_dst_i(unit,intr)
	u_int	unit;
	unsigned short  intr;
{
	hctlr_softc_t  *sp = &hctlr_softc;
        register  dst_pd_t        *pdq;
        register  hctlr_filter_t  *dfp;
        hctlr_dst_t     *dst_req;
	int		status,errors,fdata;
        int             i;
        u_char          *data;
        u_char          *data2;

        HCLOG_ENTRY(12, ("hctlr_dst_i(unit=%d,intr=%x)\n",unit,intr));

	HCSTATS(sp, dst.intr++);

        /*  Acknowledge interrupt */
        outl(HIPPI_INTR_STATUS, intr & ~HIPPI_DST_INTR);  

        /* Sanity check of controller interface state */
        if (!(sp->state & HCTLR_UP))  {
                return;
        }

        /*  Get the queue head pointer 	*/
        pdq = sp->dpd_qhead;

	assert(pdq);

	errors = pdq->chan_err;
	status = pdq->chan_sts;

#ifdef HCTLR_DEBUG
        flush();
        HCLOG(HCLOG_LOW,("hctlr_dst_i: PD %x sts %x ctl %x err %x fb_len %d fb_data %x xlen %d\n",
                               	pdq,status,pdq->chan_ctl,errors,pdq->fb_len,
	     			pdq->fb_ptr,pdq->xfer_len));
	/* if first burst, print data */
	if (sp->dst_req.ddone == 0) {  
              HCLOG(HCLOG_LOW,("hctlr_dst_i: First 16 bytes = "));
              for (i=0;i<16;i++)
                   HCLOG(HCLOG_LOW,("%02x ",(u_char *)pdq->fb_ptr[i]));
              HCLOG(HCLOG_LOW,("\n"));
	}
#endif HCTLR_DEBUG

        if (!errors) {
		/*
		 * If this is the first burst and its not an
		 * interrupt from a DST abort command, run  
		 * data through the DST filter and call the
		 * registered ULP routine if a match is
		 * found. 
		 */

		if (sp->dst_req.ddone == 0) {  
			/* first burst */
                 	HCLOG(HCLOG_LOW,(" GOOD 1st burst, running through filters\n"));
                	HCSTATS(sp, dst.bytes += pdq->fb_len);
			if (!(sp->dst_state & HCTLR_DST_CLOSED)) {
				/* run through filters */
				simple_lock(&sp->dst_filter_lock);
				queue_iterate(&sp->dst_filters,dfp,hctlr_filter_t*,links)
				{
					if (sp->dst_state & HCTLR_DST_PROM) { 
                 				HCLOG(HCLOG_LOW,(" PRM mode, only 1 filter\n"));
						sp->dst_req.ddone = dfp->ddone;
						sp->dst_req.ulp   = dfp->ulp;
						break;
					}
					if (dfp->offset == 0) {	 /* ulp filter */
						if (dfp->ulp == *pdq->fb_ptrv) {
                 					HCLOG(HCLOG_LOW,(" ULP only filter\n"));
							sp->dst_req.ddone = dfp->ddone;
							sp->dst_req.ulp   = dfp->ulp;
							break;
						}
					} 
					else {		/* ulp + data filter */
						if (dfp->ulp == *pdq->fb_ptrv) {
                 					HCLOG(HCLOG_LOW,(" ULP + data filter\n"));
							/* check data at offset, byte at a time */
							data  = pdq->fb_ptrv + dfp->offset;
							data2 = (u_char*)&fdata;	
							for (i = 0; i < dfp->bsize; i++)
								*data2++ = *data++; 			
                 					HCLOG(HCLOG_LOW,(" fdata = 0x%x,%x!\n",
								pdq->fb_ptrv+dfp->offset,fdata));

							/* set mask for correct data byte size */
							fdata &= (0xffffffff >> 
								((sizeof(int) - dfp->bsize)*8));   

                 					HCLOG(HCLOG_LOW,(" mask fdata=0x%x,%x!\n",
								pdq->fb_ptrv+dfp->offset,fdata));

							if ((fdata >= dfp->min) && 
									(fdata <= dfp->max)) {
                 						HCLOG(HCLOG_LOW,
									(" ULP+data match!\n"));
								sp->dst_req.ddone = dfp->ddone;
								sp->dst_req.ulp   = dfp->ulp;
								sp->dst_req.filter_data = fdata;
								break;
							}
						}
					}
				}
				simple_unlock(&sp->dst_filter_lock);
				if (sp->dst_req.ddone) {
                 			HCLOG(HCLOG_LOW,(" 1st burst, filter MATCH!\n"));
					/* match, update request and callback ULP routine */
					sp->dst_req.fb_ptr   = pdq->fb_ptrv;
					sp->dst_req.xfer_len = pdq->fb_len;
					sp->dst_req.ifield   = pdq->ifield;
					sp->dst_req.pkt_done = 
							(status & DST_STS_COMP) ? TRUE : FALSE;
                 			HCLOG(HCLOG_LOW,(" 1st intr, calling ddone 0x%x\n",
								sp->dst_req.ddone));
					sp->dst_req.ddone(0,&sp->dst_req);
				}
				else {
                 			HCLOG(HCLOG_LOW,(" 1st burst, NO filter match!\n"));
					/* no match, if more data tell i960 to abort */
         				if (!(status & DST_STS_COMP)) {
                 				HCLOG(HCLOG_LOW,("  abort reset of data!\n"));
						sp->scb->ctlr_cmd |= HIPPI_DST_ABORT;
						sp->dst_timer = hctlr_timer;
					}
				}
			}
			else {
				/* if no filters and pkt not done, abort */
				if (!(status & DST_STS_COMP)) {
                 			HCLOG(HCLOG_LOW,(" DST closed, abort connection!\n"));
					sp->scb->ctlr_cmd |= HIPPI_DST_ABORT;
					sp->dst_timer = hctlr_timer;
				}
			}
		}
		else {
			/* pkt must be complete on 2nd interrupt */
			assert(status & DST_STS_COMP);

			/* second interrupt, process rest of data */
                	HCSTATS(sp, dst.bytes += pdq->xfer_len);
                	HCLOG(HCLOG_LOW,(" 2nd intr, process rest of data!\n"));
			/* update fields and call the callback routine again */
			sp->dst_req.xfer_len = pdq->xfer_len;
			sp->dst_req.pkt_done = TRUE;
                 	HCLOG(HCLOG_LOW,(" 2nd intr, calling ddone 0x%x\n",
						sp->dst_req.ddone));
			sp->dst_req.ddone(0,&sp->dst_req);
		}
	}
        else {	/* errors on transfer, update statistics */
                 HCSTATS(sp, dst.errors++);

                 if (errors & DST_ERR_INTC) {
                        HCSTATS(sp, dst.nointc++);
			printf("HiPPI Warning: No Cable detected on DST Channel\n");
		 }

                 if (errors & DST_ERR_SEQS)
                         HCSTATS(sp, dst.seqs++);

                 if (errors & DST_ERR_SEQD)
                         HCSTATS(sp, dst.seqd++);

                 if (errors & DST_ERR_PARI)
                         HCSTATS(sp, dst.pari++);

                 if (errors & DST_ERR_TOWB)
                         HCSTATS(sp, dst.towb++);

                 if (errors & DST_ERR_RLLE)
                                HCSTATS(sp, dst.llrc++);

                 if (errors & DST_ERR_EXBF)
                         HCSTATS(sp, dst.exbf++);

                 if (errors & DST_ERR_ABRT)
                         HCSTATS(sp, dst.abrt++);

                 if (errors & DST_ERR_ALGN)
                         HCSTATS(sp, dst.algn++);

                 if (errors & DST_ERR_ABRTC) {
                         HCSTATS(sp, dst.cabort++);
			 /* don't mark as error */
                 	 HCSTATS(sp, dst.errors--);
		 }

                 if (errors & DST_ERR_CONT)
                         HCSTATS(sp, dst.cerr++);

		/* 
		 * only if 2nd interrupt callback ddone routine.
		 * don't bother calling back on 1st burst.  
		 */
         	 if ((status & DST_STS_COMP) && (sp->dst_req.ddone)) {
                	HCLOG(HCLOG_LOW,("hctlr_dst_i: 2nd intr ERROR, call ddone 0x%x\n",
						sp->dst_req.ddone));
			sp->dst_req.error_status = errors;
			sp->dst_req.pkt_done = TRUE;
			sp->dst_req.ddone(0,&sp->dst_req);
		 }
         }

	 /* 
	  * If packet complete, clear PD fields,
	  * clear ddone field, adjust DST PD queue,
	  * clear DST timer, and get more resources.
	  * Also, set the fb_len to 1024+32 since this
	  * is always the xfer count when more data
	  * is expected.
	  */
         if (status & DST_STS_COMP) {
#ifdef HCTLR_DEBUG
		if (pdq->chan_ctl & DST_CTL_URUN)
			HCSTATS(sp, dst.underrun++);
#endif HCTLR_DEBUG
                HCLOG(HCLOG_LOW,("hctlr_dst_i: PKT DONE, free PD(%x)!\n",pdq));
                HCSTATS(sp, dst.packets++);
               	pdq->chan_sts = 0;
               	pdq->chan_ctl = 0;
               	pdq->chan_err = 0;
               	pdq->fb_len   = HCTLR_BURST_SIZE;
		sp->dst_req.ddone = 0;
		sp->dst_req.error_status = 0;
		sp->dst_req.ulp = 0;
		sp->dst_req.filter_data = 0;
		sp->dst_timer = -1;
		sp->scb->dst_sg_cont = 0;
		sp->scb->dst_sg_cont_size = 0;

		/* reached end of PD queue list */
               	if (pdq == sp->dpd_qtail) {
                       	sp->dpd_qhead = 0;
                       	sp->dpd_qtail = sp->dpd_qtail->next_pdv;
		}
               	else {
                       	sp->dpd_qhead = pdq->next_pdv;
		}

        	hctlr_recv(sp);
	}
	return;
}


/*
 * hctlr_watch(sp)
 *
 *      This routine is the watchdog timer routine for the HIPPI controller.
 *      The algorithm used here is to use a slow poll when things are
 *      going right and then drop into faster poll when a problem is detected.
 *
 * Called by: 	kernel timer                                    
 *
 * Returns on success
 * 	nothing   
 *
 * Possible errors
 *      - HiPPI board fails to re-initialize
 *	- SRC request exceeds timeout value
 */
void
hctlr_watch(sp)
	hctlr_softc_t  *sp;
{
        int     oldpri = SPLHIPPI();

        /*
         *  If all DST and SRC commands have been accounted for (timer = -1) 
	 *  or the interface is not up, continue slow poll.
         */
        if (((sp->src_timer < 0) && (sp->dst_timer < 0)) || 
				((sp->state & HCTLR_UP) == 0)) {
                timeout(hctlr_watch, sp, HCTLR_POLL);

        } else if ((sp->src_timer == 0) || (sp->dst_timer == 0)) {
                HCSTATS(sp, timeouts++);
                /*
                 *  There is a command outstanding for which we haven't
                 *  received an interrupt.  If retries are exhausted,
                 *  try to reset the i960.  This will cause any pending
                 *  transmit requests in SRC queue to be marked bad.
		 *  If reset successful hctlr_init schedules watchdog
                 */
                sp->state &= ~HCTLR_UP;
		sp->state |= HCTLR_TIMEOUT;
                if (hctlr_init(sp) != D_SUCCESS) {
                        sp->state &= ~HCTLR_UP;
                        printf("%d: HiPPI board not responding\n",sp->node_id);
                        printf("%d: HiPPI interface marked down\n",sp->node_id);
		}
        } else {
                /*
                 *  Retries left, decrement and go to a fast poll time.
                 */
#ifdef HCTLR_DEBUG
                if (hclog_id)
                        printf("hctlr_watch: retry, timers src=%d dst=%d\n",
				sp->src_timer,sp->dst_timer);
#endif HCTLR_DEBUG

		if (sp->src_timer > 0)
                	sp->src_timer--;
		if (sp->dst_timer > 0)
			sp->dst_timer--;

                /* retry, change to fast poll */
                timeout(hctlr_watch, sp, HCTLR_POLL);
        }
        splx(oldpri);
        return;
}


/*
 * hctlr_dst_halt()
 *
 *      This routine is called by the raw driver (rhippi.c) to halt 
 *	DST channel, hold off readys which forces flow control.
 *
 * Called by: 	raw (rhippi.c) driver ONLY!                                    
 *
 * Returns on success
 * 	nothing   
 */
void
hctlr_dst_halt()
{
	hctlr_softc_t  *sp = &hctlr_softc;
        int     oldpri = SPLHIPPI();

        HCLOG_ENTRY(13, ("hctlr_dst_halt()\n"));
        HCSTATS(sp, dst.halt++);
	sp->dst_state |= HCTLR_DST_HALT;
        splx(oldpri);
	return;
}


/*
 * hctlr_dst_continue()
 *
 *      This routine is called by raw driver only (rhippi.c) 
 *
 * Called by: 	raw (rhippi.c) driver ONLY!                                    
 *
 * Returns on success
 * 	nothing   
 */
void
hctlr_dst_continue()
{
	hctlr_softc_t  *sp = &hctlr_softc;
        int     oldpri = SPLHIPPI();

        HCLOG_ENTRY(14, ("hctlr_dst_continue()\n"));
        HCSTATS(sp, dst.cont++);
	sp->dst_state &= ~HCTLR_DST_HALT;
	/* make resources available again */
	hctlr_recv(sp);
        splx(oldpri);
	return;
}

/*
 * low level ioctl control for controller interface from ULP drivers.
 * must run at SPLHIPPI 
 *
 *  Valid types include:
 *   HIPPI_DST_TIMEOUT, HIPPI_SRC_TIMEOUT, HIPPI_DEV_EXCL
 *   HIPPI_DEV_MPPC, HIPPI_DEV_CONT
 *
 * Returns on success:
 *      D_SUCCESS
 *
 * errors:
 *      D_IO_ERROR - DST connetion has been aborted
 *      D_INVALID_OPERATION - invalid
 */
hctlr_ioctl(type, param, key)
       int   type;   	/* type of ioctl  */
       int   param;  	/* parameter for ioctl */
       int   key;       /* a unique value for EXCL access */
{
	hctlr_softc_t  *sp = &hctlr_softc;
	int rc,oldpri = SPLHIPPI();
        HCLOG_ENTRY(15, ("hctlr_ioctl(%x,%x,%x)\n",type,param,key));

        rc = D_SUCCESS;
        switch (type) {
        case HIPPI_SRC_TIMEOUT:       /* change the SRC channel timeout */

		if (param == 0) {
                        rc =  D_INVALID_OPERATION;
			break;
		}

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_SRC_TIMEOUT\n"));

                /* new value is passed in microseconds, convert to 20ns
                 * increments and give it to the controller
                 */
                printf("HiPPI SRC timeout changed from %d usec. to %d usec.\n",
                        sp->scb->src_timeout/50, param);

                sp->scb->src_timeout = 50 * param;
                break;

        case HIPPI_DST_TIMEOUT:       /* change the DST channel timeout */

		if (param == 0) {
                        rc =  D_INVALID_OPERATION;
			break;
		}

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_DST_TIMEOUT\n"));

                /* new value is passed in microseconds, convert to 20ns
                 * increments and give it to the controller
                 */
                printf("HiPPI DST timeout changed from %d usec. to %d usec.\n",
                        sp->scb->dst_timeout/50, param);

                sp->scb->dst_timeout = 50 * param;
                break;

        case HIPPI_DEV_EXCL:     /* request exclusive use of hippi interface */

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_DEV_EXCL\n"));

                /* TRUE, set exclusive mode */
                if (param) {
                        /* check: EXCL */
                        if ((sp->state & HCTLR_EXCL) || (!key)) {
                		HCLOG(HCLOG_LOW,("hctlr_ioctl: already EXCL or no key\n"));
				rc = D_INVALID_OPERATION;
				break;
                        }
                        /* there must be no filters and no src_reqs pending */
        		simple_lock(&sp->dst_filter_lock);
        		if (!queue_empty(&sp->dst_filters)) {
        			simple_unlock(&sp->dst_filter_lock);
                		HCLOG(HCLOG_LOW,("hctlr_ioctl: filter already set\n"));
				rc = D_INVALID_OPERATION;
				break;
			}
        		simple_unlock(&sp->dst_filter_lock);
        		simple_lock(&sp->src_req_lock);
        		if (!queue_empty(&sp->src_reqs)) {
        			simple_unlock(&sp->src_req_lock);
				rc = D_INVALID_OPERATION;
				break;
        		}
        		simple_unlock(&sp->src_req_lock);
			sp->excl_key = key;
                        sp->state |= HCTLR_EXCL;
                }
                /* FALSE, unset exclusive mode */
                else {
                	/* check: non-exclusive, and key */
                	if ((!(sp->state & HCTLR_EXCL)) || (sp->excl_key != key)) {
				rc = D_INVALID_OPERATION;
				break;
                	}
			sp->excl_key = 0;
                        sp->state &= ~HCTLR_EXCL;
                }
                break;

        case HIPPI_DEV_MPPC:       /* multiple packets per connection mode */

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_DEV_MPPC\n"));

                /* check: non-exclusive, and key */
                if ((!(sp->state & HCTLR_EXCL)) || (sp->excl_key != key)) {
			rc = D_INVALID_OPERATION;
			break;
                }

                /* TRUE, set multiple packets per connection mode */
                if (param) {
                        if ((sp->src_state & HCTLR_SRC_MPPC) || 
			     (sp->dst_state & HCTLR_DST_MPPC)) {
				rc = D_INVALID_OPERATION;
				break;
                        }
                        else {
                                /* set correct SRC state */
                                sp->src_state |= HCTLR_SRC_MPPC;
                                sp->dst_state |= HCTLR_DST_MPPC;
                        }
                }
                /* FALSE, reset multiple packets per connection mode */
                else {
                        if (((sp->src_state & HCTLR_SRC_MPPC) == 0) ||
                                ((sp->dst_state & HCTLR_DST_MPPC) == 0)) {
				rc = D_INVALID_OPERATION;
				break;
                        }
                        else {
                                /* reset MPPC state, drop connection */
                                sp->src_state &= ~HCTLR_SRC_MPPC;
                                sp->dst_state &= ~HCTLR_DST_MPPC;
                                sp->scb->src_cmd |= HIPPI_SRC_ABRT;
                                sp->scb->dst_cmd |= HIPPI_DST_ABRT;
                        }
                }
                break;

        case HIPPI_DEV_CONT:       /* buffer continuation support */

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_DEV_CONT\n"));

                /* check: non-exclusive, and key */
                if ((!(sp->state & HCTLR_EXCL)) || (sp->excl_key != key)) {
				rc = D_INVALID_OPERATION;
				break;
                }

                /* TRUE, set continuation mode */
                if (param) {
                        if ((sp->src_state & HCTLR_SRC_CONT) || 
			     (sp->dst_state & HCTLR_DST_CONT)) {
				rc = D_INVALID_OPERATION;
				break;
                        } else if ((sp->scb->src_cmd & HIPPI_SRC_CONTINUATION) ||
				   (sp->scb->dst_cmd & HIPPI_SRC_CONTINUATION)) {
                		printf("hctlr_ioctl: CTLR still in CONT mode!\n");
				rc = D_INVALID_OPERATION;
				break;
			}
                        else {
                                /* set correct channel states */
                                sp->src_state |= HCTLR_SRC_CONT;
                                sp->dst_state |= HCTLR_DST_CONT;
                                sp->scb->src_cmd |= HIPPI_SRC_CONTINUATION;
                                sp->scb->dst_cmd |= HIPPI_DST_CONTINUATION;
                        }
                }
                /* FALSE, reset continuation mode */
                else {
                        if (((sp->src_state & HCTLR_SRC_CONT) == 0) ||
                                ((sp->dst_state & HCTLR_DST_CONT) == 0)) {
				rc = D_INVALID_OPERATION;
				break;
                        }
                        else {
                                /* reset SRC state, drop connection */
                                sp->src_state &= ~HCTLR_SRC_CONT;
                                sp->dst_state &= ~HCTLR_DST_CONT;
                                sp->scb->src_cmd |= HIPPI_SRC_ABRT;
                                sp->scb->dst_cmd |= HIPPI_DST_ABRT;
                        }
                }
                break;

        case HIPPI_DEV_CONT_STS:       /* buffer continuation status */

                HCLOG(HCLOG_LOW,("hctlr_ioctl: HIPPI_DEV_CONT_STS\n"));

		if (sp->dst_state & HCTLR_DST_CONT_DROP) {
                	HCLOG(HCLOG_LOW,("hctlr_ioctl: DST connection dropped\n"));
			sp->dst_state &= ~HCTLR_DST_CONT_DROP;
			rc = D_IO_ERROR;
		}
		break;

        default:
	        rc = D_INVALID_OPERATION; 
        }

	splx(oldpri);
        return (rc);
}


/*
 * hippi_stats:
 *
 * This routine will be called to print all statistics
 * and is called from the kernel debugger.
 */
hippi_stats()
{
#ifdef HCTLR_STATS
	hctlr_softc_t  *sp = &hctlr_softc;
        hctlr_stats_t   *s = &sp->stats;

        db_printf("\n%d: Hippi SRC stats:\n",sp->node_id);
        db_printf("packets=%u bytes=%u KB intr=%u i960busy=%u chained=%u, largest chain=%u\n",
                s->src.packets,s->src.bytes/1024,s->src.intr,s->src.busy,s->src.chain,
		s->src.chain_max);
        db_printf("qfull=%u qnull=%u qhit=%u qmiss=%u qflush=%u errs=%u\n",
                s->src.pq_full,s->src.pq_null,s->src.qhit,s->src.qmiss,
                s->src.qflush, s->src.errors);
        db_printf("seqs=%u seqd=%u pari=%u nointc=%u towr=%u\n",
                s->src.seqs, s->src.seqd, s->src.pari, s->src.nointc,s->src.towr);
        db_printf("algn=%u swrd=%u overruns=%u\n",s->src.algn,s->src.swrd,s->src.overrun);

        db_printf("\n%d: Hippi DST stats:\n",sp->node_id);
        db_printf("packets=%u bytes=%u KB intr=%u i960busy=%u\n",
                s->dst.packets,s->dst.bytes/1024,s->dst.intr,s->dst.busy);
        db_printf("qfull=%u qnull=%u qhit=%u qmiss=%u qflush=%u errs=%u\n",
                s->dst.pq_full, s->dst.pq_null, s->dst.qhit, s->dst.qmiss,
                s->dst.qflush, s->dst.errors);
        db_printf("seqs=%u seqd=%u pari=%u nointc=%u towb=%u llrc=%u\n",
                s->dst.seqs,s->dst.seqd,s->dst.pari,s->dst.nointc,
		s->dst.towb,s->dst.llrc);
        db_printf("exbf=%u abrt=%u algn=%u undrun=%u cabrt=%u cerr=%u\n",
		s->dst.exbf,s->dst.abrt,s->dst.algn,s->dst.underrun,
		s->dst.cabort,s->dst.cerr);

        db_printf("\n%d: Hippi other stats:\n",sp->node_id);
        db_printf("timeout=%u dmsg=%u ctlr_i=%u srci=%u dsti=%u\n",
                s->timeouts,s->dmsg_int,s->ctlr_int,s->srci,s->dsti);
        db_printf("reset=%u init=%u DstHalt=%u DstContinue=%u\n",
		s->resets,s->init,s->dst.halt,s->dst.cont);

#else
        db_printf("HCTLR_STATS not defined\n");
#endif
        return;
}


/*
 * hippi_debug:
 *
 * This routine will be called to print all debug information.
 * It is called from the kernel debugger.
 *  
 * Prints:  interface softc stucture, i960 SCB, SRC requests,
 *          DST filters, SRC and DST packet descriptors.
 */
hippi_debug()
{
        src_pd_t        *spd;
	dst_pd_t	*dpd;
	hctlr_filter_t	*dfp;
	hctlr_src_t	*src_req;
	hctlr_softc_t	*sp = &hctlr_softc;
	int     	i,oldpri = SPLHIPPI();

        if (sp->state == HCTLR_UNINITIALIZED) {
		db_printf("\nHiPPI interface driver not initialized\n");
		return 0;
	}

        db_printf("\nHCTLR_SOFTC:\n");
	db_printf(" sp->scb         = 0x%x\n",sp->scb);
	db_printf(" sp->hippi_base  = 0x%x\n",sp->hippi_base);
	db_printf(" sp->begin_fb    = 0x%x\n",sp->begin_fb);
	db_printf(" sp->begin_spd   = 0x%x\n",sp->begin_spd);
	db_printf(" sp->begin_sptrs = 0x%x\n",sp->begin_sptrs);
	db_printf(" sp->spd_qhead   = 0x%x\n",sp->spd_qhead);
	db_printf(" sp->spd_qchain  = 0x%x\n",sp->spd_qchain);
	db_printf(" sp->spd_qtail   = 0x%x\n",sp->spd_qtail);
	db_printf(" sp->begin_dpd   = 0x%x\n",sp->begin_dpd);
	db_printf(" sp->begin_dptrs = 0x%x\n",sp->begin_dptrs);
	db_printf(" sp->dpd_qhead   = 0x%x\n",sp->dpd_qhead);
	db_printf(" sp->dpd_qchain  = 0x%x\n",sp->dpd_qchain);
	db_printf(" sp->dpd_qtail   = 0x%x\n",sp->dpd_qtail);
	db_printf(" sp->dst_req.fb_ptr       = 0x%x\n",sp->dst_req.fb_ptr);
	db_printf(" sp->dst_req.xfer_len     = 0x%x\n",sp->dst_req.xfer_len);
	db_printf(" sp->dst_req.error_status = 0x%x\n",sp->dst_req.error_status);
	db_printf(" sp->dst_req.pkt_done     = 0x%x\n",sp->dst_req.pkt_done);
	db_printf(" sp->dst_req.ulp          = 0x%x\n",sp->dst_req.ulp);
	db_printf(" sp->dst_req.filter_data  = 0x%x\n",sp->dst_req.filter_data);
	db_printf(" sp->dst_req.ifield       = 0x%x\n",sp->dst_req.ifield);
	db_printf(" sp->dst_req.ddone        = 0x%x\n",sp->dst_req.ddone);
	db_printf(" sp->state       = 0x%x\n",sp->state);
	if (sp->state & HCTLR_UP)
		db_printf("                 = HCTLR_UP\n");
	if (sp->state & HCTLR_TIMEOUT)
		db_printf("                 = HCTLR_TIMEOUT\n");
	if (sp->state & HCTLR_EXCL)
		db_printf("                 = HCTLR_EXCL\n");
	if (sp->state & HCTLR_HW_ERROR)
		db_printf("                 = HCTLR_HW_ERROR\n");
	if (sp->state & HCTLR_SW_ERROR)
		db_printf("                 = HCTLR_SW_ERROR\n");
	db_printf(" sp->src_state    = 0x%x\n",sp->src_state);
	if (sp->src_state & HCTLR_SRC_PENDING)
		db_printf("                 = HCTLR_SRC_PENDING\n");
	if (sp->src_state & HCTLR_SRC_MPPC)
		db_printf("                 = HCTLR_SRC_MPPC\n");
	if (sp->src_state & HCTLR_SRC_CONT)
		db_printf("                 = HCTLR_SRC_CONT\n");
	db_printf(" sp->dst_state    = 0x%x\n",sp->dst_state);
	if (sp->dst_state & HCTLR_DST_CLOSED)
		db_printf("                 = HCTLR_DST_CLOSED\n");
	if (sp->dst_state & HCTLR_DST_HALT)
		db_printf("                 = HCTLR_DST_HALT\n");
	if (sp->dst_state & HCTLR_DST_MPPC)
		db_printf("                 = HCTLR_DST_MPPC\n");
	if (sp->dst_state & HCTLR_DST_CONT)
		db_printf("                 = HCTLR_DST_CONT\n");
	if (sp->dst_state & HCTLR_DST_PROM)
		db_printf("                 = HCTLR_DST_PROM\n");
	if (sp->dst_state & HCTLR_DST_CONT_DROP)
		db_printf("                 = HCTLR_DST_CONT_DROP\n");
	db_printf(" sp->excl_key    = 0x%x\n",sp->excl_key);
	db_printf(" sp->node_id     = 0x%x\n",sp->node_id);
	db_printf(" sp->src_timer   = 0x%x\n",sp->src_timer);
	db_printf(" sp->dst_timer   = 0x%x\n",sp->dst_timer);

        db_printf("\ni960 SCB:\n");
	db_printf(" i960_scb                   = 0x%x\n",sp->scb);
	db_printf(" i960_scb->ctlr_err         = 0x%x\n",sp->scb->ctlr_err);
	db_printf(" i960_scb->ctlr_cmd         = 0x%x\n",sp->scb->ctlr_cmd);
	db_printf(" i960_scb->ctlr_sts         = 0x%x\n",sp->scb->ctlr_sts);
	db_printf(" i960_scb->src_cmd          = 0x%x\n",sp->scb->src_cmd);
	if (sp->scb->src_cmd & HIPPI_SRC_ABRT)
		db_printf("                 = HIPPI_SRC_ABRT\n");
	if (sp->scb->src_cmd & HIPPI_SRC_CONTINUATION)
		db_printf("                 = HIPPI_SRC_CONTINUATION\n");
	db_printf(" i960_scb->dst_cmd          = 0x%x\n",sp->scb->dst_cmd);
	if (sp->scb->dst_cmd & HIPPI_DST_ABRT)
		db_printf("                 = HIPPI_DST_ABRT\n");
	if (sp->scb->dst_cmd & HIPPI_DST_CONTINUATION)
		db_printf("                 = HIPPI_DST_CONTINUATION\n");
	db_printf(" i960_scb->src_pd           = 0x%x\n",sp->scb->src_pd);
	db_printf(" i960_scb->dst_pd           = 0x%x\n",sp->scb->dst_pd);
	db_printf(" i960_scb->dst_sg_ptr       = 0x%x\n",sp->scb->dst_sg_ptr);
	db_printf(" i960_scb->dst_sg_cont      = 0x%x\n",sp->scb->dst_sg_cont);
	db_printf(" i960_scb->dst_sg_sont_size = 0x%x\n",sp->scb->dst_sg_cont_size);
	db_printf(" i960_scb->host_page_size   = 0x%x\n",sp->scb->host_page_size);
	db_printf(" i960_scb->src_timeout (us) = 0x%x\n",sp->scb->src_timeout/50);
	db_printf(" i960_scb->dst_timeout (us) = 0x%x\n",sp->scb->dst_timeout/50);
	db_printf(" i960_scb->debug_msg        = 0x%x\n",sp->scb->debug_msg);
	db_printf(" i960_scb->ctlr_mon         = 0x%x\n",sp->scb->ctlr_mon);

	db_printf("\nDST_FILTERS:\n");
	simple_lock(&sp->dst_filter_lock);
	if (queue_empty(&sp->dst_filters)) {
		db_printf("no filters are set\n");
	}
	else {
		queue_iterate(&sp->dst_filters,dfp,hctlr_filter_t*,links)
		{
			db_printf(" FILTER = 0x%x\n",dfp);
			db_printf(" filter->links.prev = 0x%x\n",dfp->links.prev);
			db_printf(" filter->links.next = 0x%x\n",dfp->links.next);
			db_printf(" filter->ulp        = 0x%x\n",dfp->ulp);
			db_printf(" filter->offset     = 0x%x\n",dfp->offset);
			db_printf(" filter->bsize      = 0x%x\n",dfp->bsize);
			db_printf(" filter->min        = 0x%x\n",dfp->min);
			db_printf(" filter->max        = 0x%x\n",dfp->max);
			db_printf(" filter->ddone()    = 0x%x\n",dfp->ddone);
		}
	}
	simple_unlock(&sp->dst_filter_lock);

        db_printf("\nSRC REQUEST QUEUE:\n");
        simple_lock(&sp->src_req_lock);
	if (queue_empty(&sp->src_reqs)) {
		db_printf("no outstanding SRC requests\n");
	}
	else {
		spd = sp->spd_qhead;
		queue_iterate(&sp->src_reqs, src_req, hctlr_src_t*, links)
        	{
			/* check SPD and SREQ alignment to end of SPD queue */
			if (spd->next_pdv != sp->spd_qhead) { 
				if (src_req->fb_ptr != spd->fb_ptr) 
					db_printf("\nWARNING!! src REQ and src PD out of sync!\n");
				if (spd->next_pdv != sp->spd_qhead)
					spd = spd->next_pdv;
			}
			db_printf(" SRC_REQ = 0x%x\n",src_req);
			db_printf(" src_req->links.prev   = 0x%x\n",src_req->links.prev);
			db_printf(" src_req->links.next   = 0x%x\n",src_req->links.next);
			db_printf(" src_req->i_field      = 0x%x\n",src_req->i_field);
			db_printf(" src_req->fb_ptr       = 0x%x\n",src_req->fb_ptr);
			db_printf(" src_req->fb_len       = 0x%x\n",src_req->fb_len);
			db_printf(" src_req->ior          = 0x%x\n",src_req->ior);
			db_printf(" src_req->sg_ptr       = 0x%x\n",src_req->sg_ptr);
			db_printf(" src_req->chan_ctl     = 0x%x\n",src_req->chan_ctl);
			db_printf(" src_req->sdone()      = 0x%x\n",src_req->sdone);
			db_printf(" src_req->error_status = 0x%x\n",src_req->error_status);
			db_printf(" src_req->xfer_len     = 0x%x\n",src_req->xfer_len);
			db_printf(" src_req->excl_key     = 0x%x\n",src_req->excl_key);
		}
	}
        simple_unlock(&sp->src_req_lock);

        db_printf("\nSRC PACKET DESCRIPTOR QUEUE:\n");
	spd = sp->spd_qhead;
	if (!spd) 
	   db_printf("SRC pd queue is not in use\n");
        while (spd) {
		db_printf(" SRC_PD = 0x%x\n",spd);
		db_printf(" src_pd->chan_ctl     = 0x%x\n",spd->chan_ctl);
		db_printf(" src_pd->ifield       = 0x%x\n",spd->ifield);
		db_printf(" src_pd->fb_ptr       = 0x%x\n",spd->fb_ptr);
		db_printf(" src_pd->fb_len       = 0x%x\n",spd->fb_len);
		db_printf(" src_pd->sg_ptr       = 0x%x\n",phystokv(spd->sg_ptr));
		db_printf(" src_pd->sg_cont      = 0x%x\n",spd->sg_cont);
		db_printf(" src_pd->sg_cont_size = 0x%x\n",spd->sg_cont_size);
/*		db_printf(" src_pd->next_pd      = 0x%x\n",spd->next_pd);   */
		db_printf(" src_pd->chan_err     = 0x%x\n",spd->chan_err);
		db_printf(" src_pd->sg_len       = 0x%x\n",spd->sg_len);
		db_printf(" src_pd->sg_cont_ptrv = 0x%x\n",spd->sg_cont_ptrv);
		db_printf(" src_pd->src_req      = 0x%x\n",spd->src_req);
/*		db_printf(" src_pd->next_pdv     = 0x%x\n",spd->next_pdv);  */
                if (spd == sp->spd_qtail)
			spd = 0;
		else
			spd = spd->next_pdv;
        }

        db_printf("\nDST PACKET DESCRIPTOR QUEUE:\n");
	dpd = sp->dpd_qhead;
	if (!dpd) 
	   db_printf("DST pd queue is not in use\n");
        while (dpd) {
		db_printf(" DST_PD = 0x%x\n",dpd);
		db_printf(" dst_pd->chan_ctl = 0x%x\n",dpd->chan_ctl);
/*		db_printf(" dst_pd->next_pd  = 0x%x\n",dpd->next_pd); */
		db_printf(" dst_pd->fb_ptr   = 0x%x\n",dpd->fb_ptr);
		db_printf(" dst_pd->fb_len   = 0x%x\n",dpd->fb_len);
		db_printf(" dst_pd->xfer_len = 0x%x\n",dpd->xfer_len);
		db_printf(" dst_pd->chan_err = 0x%x\n",dpd->chan_err);
		db_printf(" dst_pd->ifield   = 0x%x\n",dpd->ifield);
		db_printf(" dst_pd->chan_sts = 0x%x\n",dpd->chan_sts);
		db_printf(" dst_pd->fb_ptrv  = 0x%x\n",dpd->fb_ptrv);
		db_printf(" dst_pd->sg_cont_ptrv = 0x%x\n",dpd->sg_cont_ptrv);
/*		db_printf(" dst_pd->next_pdv = 0x%x\n",dpd->next_pdv); */
                if (dpd == sp->dpd_qtail)
			dpd = 0;
		else
			dpd = dpd->next_pdv;
	}

	splx(oldpri);
}


#endif	NHIPPI > 0
