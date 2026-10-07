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
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license 
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance with
 *	the terms of that agreement.
 *	Copyright (c) 1991,1992,1993  Intel Corporation.
 *
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/rhippi.c,v 1.5 1995/03/02 23:32:58 arlin Exp $
 * $Log: rhippi.c,v $
 * Revision 1.5  1995/03/02  23:32:58  arlin
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
 * Revision 1.4  1994/11/18  20:43:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1994/10/17  15:46:44  arlin
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
 * Revision 1.2  1994/07/26  18:29:31  arlin
 * Fix the 1056 size packet problem on DST and bad assert in rhippi_watch.
 *
 * Revision 1.1  1994/06/08  16:58:52  arlin
 * Initial Checkin for R1.3
 *
 */
/*
 *      File:   rhippi.c
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      Raw HiPPI Device Driver.
 */

#include <hippi.h>

#if NHIPPI > 0

#include <sys/types.h>
#include <chips/busses.h>
#include <device/io_req.h>
#include <device/conf.h>
#include <device/device_port.h>
#include <vm/vm_kern.h>
#include <kern/queue.h>
#include <kern/assert.h>
#include <ipc/ipc_port.h>
#include <i860paragon/expansion.h>
#include <i860paragon/hippi/hippidev.h>
#include <i860paragon/hippi/hctlr.h>
#include <i860paragon/hippi/rhippi.h>


int  rhippi_probe(), rhippi_attach(), rhippi_open(), rhippi_close(); 
int  rhippi_read(), rhippi_write(), rhippi_setstat(), rhippi_getstat(); 
int  rhippi_devinfo(), rhippi_bld_srcq(), rhippi_src_i(), rhippi_dst_i(); 
int  rhippi_set_filter(), rhippi_unset_filter(), rhippi_bld_filter();
int  rhippi_setinput(), rhippi_bld_dstq();
void rhippi_xmit(), rhippi_watch(), rhippi_read_cleanup();

extern  int expansion_id();
extern  int getbootint();
extern  int hctlr_timer;
extern  struct bus_ctlr  *hippi_minfo[NHIPPI]; 

caddr_t rhippi_std[NHIPPI] = { 0 };
struct  bus_device *rhippi_dinfo[NHIPPI] = { 0 };
struct	bus_driver
  rhippi_driver = {
    rhippi_probe,   /*  *probe  */
    0,              /*  *slave  */
    rhippi_attach,  /*  *attach */
    0,              /*  *dgo    */
    rhippi_std,     /*  *addr   */
    "rhippi",       /*  *dname  */
    rhippi_dinfo,   /* **dinfo  */
    "hippi",        /*  *mname  */
    hippi_minfo,    /* **minfo  */
    BUS_INTR_B4_PROBE    /*   flags  */
};

static char *rhippi_signon = "\
HiPPI Raw Driver (rhippi) $Revision: 1.5 $\n\
Copyright (c) 1992,1993 Intel Corp., All Rights Reserved\n";

rhippi_softc_t 	rhippi_softc;

/* 
 * Set rhlog_id bits accordingly for debug messages:
 * (via kernel debugger, w rhlog_id 0x..)
 *
 *
 *  19 18 17 16  15 14 13 12  11 10 9 8  7 6 5 4  3 2 1 0
 *   |  |  |  |   |  |  |  |   |  | | |  | | | |  | | | |__rhippi_probe
 *   |  |  |  |   |  |  |  |   |  | | |  | | | |  | | |____rhippi_attach
 *   |  |  |  |   |  |  |  |   |  | | |  | | | |  | |______rhippi_bld_srcq    
 *   |  |  |  |   |  |  |  |   |  | | |  | | | |  |________rhippi_bld_dstq    
 *   |  |  |  |   |  |  |  |   |  | | |  | | | |___________rhippi_bld_filter
 *   |  |  |  |   |  |  |  |   |  | | |  | | |_____________rhippi_open    
 *   |  |  |  |   |  |  |  |   |  | | |  | |_______________rhippi_close   
 *   |  |  |  |   |  |  |  |   |  | | |  |_________________rhippi_write   
 *   |  |  |  |   |  |  |  |   |  | | |____________________rhippi_xmit    
 *   |  |  |  |   |  |  |  |   |  | |______________________rhippi_read    
 *   |  |  |  |   |  |  |  |   |  |________________________rhippi_set_filter
 *   |  |  |  |   |  |  |  |   |___________________________rhippi_unset_filter
 *   |  |  |  |   |  |  |  |_______________________________rhippi_getstat     
 *   |  |  |  |   |  |  |__________________________________rhippi_setinput    
 *   |  |  |  |   |  |_____________________________________rhippi_setstat     
 *   |  |  |  |   |________________________________________rhippi_devinfo     
 *   |  |  |  |____________________________________________rhippi_dst_i       
 *   |  |  |_______________________________________________rhippi_src_i       
 *   |  |__________________________________________________rhippi_watch       
 *   |_____________________________________________________rhippi_read_cleanup       
 */
long rhlog_id    = 0;
long rhlog_level = 0xfff;


/*
 * Locate and lock next available DST command/buffer 
 */
rhippi_dst_t *
rhippi_get_dq(sp)
        rhippi_softc_t  *sp;
{
	register int    i;
	rhippi_dst_t    *dq = sp->dst_qstart;

	for (i = 0; i < DST_PD_QCNT; i++, dq++) {
		if (dq->busy == FALSE) {
			dq->busy = TRUE;
			return dq;
		}
	}
	return NULL;
}


/*
 * rhippi_probe:
 *
 *  If called, assume we have a HIPPI controller attached
 */
rhippi_probe(virt, bus)
	caddr_t			virt;
	struct bus_device	*bus;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	u_char	*p;
	int	i;

	RHLOG_ENTRY(0, ("rhippi_probe(unit=%x, virt=%x, bus=%x)\n",
					bus->unit,virt,bus));

        /*
	 * Do we have a HiPPI controller and is it alive?
	 */
	if (hctlr_init() != D_SUCCESS)
		return 0;

	/*
	 *  signon message for driver. 
	 */
	printf(rhippi_signon);

	/*
	 *  Clear and initialize the rhippi_softc structure.
	 *  Set the max request count to 64 for the default
	 *  and allow this to be overriden by bootmagic string.
         *  Set excl_key with some unique address. Will be used 
         *  when accessing the hippi interface in exclusive mode.
	 */
	for (i = 0, p = (u_char*)sp; i < sizeof(rhippi_softc_t); i++)
		*p++ = 0;

	sp->node_id	= NODE_ID;
	sp->state	= RHIPPI_PROBED;
        sp->excl_key    = (u_int)&rhippi_softc;  
        sp->first_write = TRUE;  
	sp->max_flt_cnt = getbootint("HIPPI_RAW_FILTER_CNT", RHIPPI_FILTER_QSIZE);
	return(1);
}


/*
 * rhippi_attach:
 *
 *  Attach the RAW HIPPI driver interface.  
 *
 */
rhippi_attach(bus)
	struct bus_device	*bus;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	RHLOG_ENTRY(1, ("\nrhippi_attach(unit=%x,bus=%x)\n",bus->unit,bus));

	/*
	 *  Make sure we're being called in the proper sequence.
	 */
	if (sp->state != RHIPPI_PROBED) {
		printf("rhippi driver attach out of sequence, state=%d\n",
			sp->state);
		return;
	} else {
		sp->state = RHIPPI_ATTACHED;
	}
	return;
}

/*
 * rhippi_bld_srcq:
 *
 *  The routine will build and initialize the raw hippi driver
 *  transmit request queue. It is a circular queue and holds all
 *  outstanding requests for the SRC channel. If this queue is full
 *  all new requests will be queued up on the rhippi_softc->src_q
 *  chain.
 */

rhippi_bld_srcq(sp)
        rhippi_softc_t  *sp;
{
        rhippi_src_t    *src_q;
        int             i;

        RHLOG_ENTRY(2, ("rhippi_bld_srcq(sp=%lx)\n", sp));

        /* Wire down the transmit queue resources. */

        if (kmem_alloc_wired(   kernel_map,
                                &sp->src_qstart,
                                (sizeof(rhippi_src_t)*RHIPPI_SRC_QSIZE)) != KERN_SUCCESS)
                return(D_IO_ERROR);

	/* starting address of src command queue */
        src_q = (rhippi_src_t *)sp->src_qstart;

        /*
         * Build the transmit request queue
         */
        for (i = 0; i < RHIPPI_SRC_QSIZE; i++, src_q++) {
                src_q->hreq.i_field  = 0;
                src_q->hreq.fb_ptr   = 0;
                src_q->hreq.fb_len   = 0;
                src_q->hreq.ior      = 0;
                src_q->hreq.sg_ptr   = 0;
                src_q->hreq.chan_ctl = 0;
                src_q->hreq.excl_key = sp->excl_key;
                src_q->hreq.sdone    = rhippi_src_i;
                src_q->hreq.error_status = 0;
                src_q->hreq.xfer_len = 0;
                src_q->next_req  = (rhippi_src_t *)src_q + 1;
        }

        /*
         *  Make SRC queue circular
         */
        src_q--;
        src_q->next_req  = (rhippi_src_t  *)sp->src_qstart;

        /*
         *  Set PD head and tail pointers.
         */
        sp->src_qhead = 0;
        sp->src_qtail = (rhippi_src_t *)sp->src_qstart;

        RHLOG(RHLOG_LOW, ("rhippi_bld_srcq: SRC_Q cnt %d sz %d begin %x hd %x tl %x\n",
				RHIPPI_SRC_QSIZE,(sizeof(rhippi_src_t)*RHIPPI_SRC_QSIZE),
                        sp->src_qstart,sp->src_qhead,sp->src_qtail));

        return(D_SUCCESS);
}

/*
 * rhippi_bld_dstq:
 *
 *  The routine will build and initialize the raw hippi driver
 *  DST data pending queue. It is a circular queue and holds all
 *  pending DST data waiting for a read request.
 */

rhippi_bld_dstq(sp)
        rhippi_softc_t  *sp;
{
        rhippi_dst_t    *dst_q;
	u_char          *fb_q;
        int             i;

        RHLOG_ENTRY(3, ("rhippi_bld_dstq(sp=%lx)\n", sp));

        /* Wire down the DST data pending resources */
        if (kmem_alloc_wired(   kernel_map, &sp->dst_qstart,
                                (sizeof(rhippi_dst_t)*DST_PD_QCNT)) != KERN_SUCCESS)
                return(D_IO_ERROR);

        if (kmem_alloc_wired(	kernel_map, &sp->dst_fbstart,
				((HCTLR_BURST_SIZE)*DST_PD_QCNT)) != KERN_SUCCESS)
		return(D_IO_ERROR);

	/* starting address of DST data pending queue */
        dst_q = (rhippi_dst_t *)sp->dst_qstart;
	fb_q  = sp->dst_fbstart;

        /*
         * Build the DST pending queue
         */
        for (i = 0; i < DST_PD_QCNT; i++, dst_q++) {
                dst_q->fb_ptr   = fb_q;
                dst_q->xfer_len = 0;
                dst_q->ulp      = 0;
                dst_q->filter_data = 0;
                dst_q->ifield   = 0;
                dst_q->busy     = FALSE;  /* free */
		fb_q += HCTLR_BURST_SIZE;
        }

        RHLOG(RHLOG_LOW, ("rhippi_bld_dstq: DST_Q cnt %d sz %d strt %x fbstrt %x\n",
			DST_PD_QCNT,(sizeof(rhippi_dst_t)*DST_PD_QCNT),
                        sp->dst_qstart,sp->dst_fbstart));

        return(D_SUCCESS);
}

/*
 * rhippi_bld_filter:
 *
 *  This routine will build and initialize the raw hippi driver
 *  DST filter command queue. 
 */
rhippi_bld_filter(sp)
        rhippi_softc_t  *sp;
{
        rhippi_filter_t    *rh_q;
        int             i;

        RHLOG_ENTRY(4, ("rhippi_bld_filter(sp=%lx)\n", sp));

        /* Wire down the filter queue resources. */
        if (kmem_alloc_wired(   kernel_map,
                                &sp->rh_filters,
                                (sizeof(rhippi_filter_t)*sp->max_flt_cnt)) != KERN_SUCCESS)
                return(D_IO_ERROR);

	/* starting address of DST filter queue */
	rh_q = (rhippi_filter_t *)sp->rh_filters;

        /*
         * Build the DST filter queue
         */
        for (i = 0; i < sp->max_flt_cnt; i++, rh_q++) {
                rh_q->hfilter.ulp = 0;
                rh_q->hfilter.offset = 0;
                rh_q->hfilter.bsize = 0;
                rh_q->hfilter.min = 0;
                rh_q->hfilter.max = 0;
                rh_q->hfilter.excl_key = sp->excl_key;
                rh_q->hfilter.ddone = rhippi_dst_i;
                rh_q->free = TRUE;
		rh_q->port = 0;
                rh_q->next = (rhippi_filter_t *)rh_q + 1;
        }

        /*
         *  Mark end of filter queue.
         */
        rh_q--;
        rh_q->next  = 0;

        RHLOG(RHLOG_LOW, ("rhippi_bld_filter: RH_FILTER_Q cnt %d sz %d begin %x end %x\n",
                        sp->max_flt_cnt,(sizeof(rhippi_filter_t)*sp->max_flt_cnt),
                        sp->rh_filters,rh_q));

        return(D_SUCCESS);
}


/*
 * rhippi_open:
 *
 *  Open the HIPPI driver.  The driver must have been probed
 *  and attached before being opened.
 */
rhippi_open(dev, mode, ior)
	dev_t		dev;
	int		mode;
	io_req_t	ior;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	RHLOG_ENTRY(5, ("rhippi_open(dev=%x,mode=%x,ior=%x)\n",dev,mode,ior));

        /* save reference to kernel device table */
	sp->io_device = ior->io_device;

        /*
         *  Make sure we are called in the proper sequence then initialize
         *  the controller. If HiPPI controller state is down (RHIPPI_DOWN)
	 *  try to reset controller with hctlr_init() call to recover.
         */
	if (sp->state & RHIPPI_DOWN) {
        	if (hctlr_init(sp) != D_SUCCESS)
                	return (D_IO_ERROR);
		sp->state = RHIPPI_OPEN;
	}

        if (sp->state & RHIPPI_OPEN)
                return (D_SUCCESS);

        if (sp->state != RHIPPI_ATTACHED) {
		RHLOG(RHLOG_HIGH, ("rhippi_open failed, interface not attached\n"));
		return (D_IO_ERROR);
	} else {
		/* first time through only */
        	queue_init(&sp->rhsrc_q);
		queue_init(&sp->rhdst_q);
		queue_init(&sp->rhpkt_q);
		simple_lock_init(&sp->rhsrc_lock);
		simple_lock_init(&sp->rhdst_lock);
		simple_lock_init(&sp->rhpkt_lock);
	}

        if (hctlr_init(sp) != D_SUCCESS)
                return (D_IO_ERROR);

        if (rhippi_bld_srcq(sp) != D_SUCCESS)
                return (D_IO_ERROR);

        if (rhippi_bld_dstq(sp) != D_SUCCESS)
                return (D_IO_ERROR);

        if (rhippi_bld_filter(sp) != D_SUCCESS)
                return (D_IO_ERROR);

	sp->watchdog = FALSE;
        sp->state = RHIPPI_OPEN;

        return (D_SUCCESS);
}


/*
 * rhippi_close:
 *
 */
rhippi_close(dev, ior)
	dev_t		dev;
	io_req_t	ior;
{
        int i;
	rhippi_filter_t *dfp;
	rhippi_softc_t  *sp = &rhippi_softc;
	int            oldpri = SPLHIPPI();
	RHLOG_ENTRY(6, ("rhippi_close(dev=%x,ior=%x)\n",dev,ior));

	if (sp->state & RHIPPI_DOWN) {
		splx(oldpri);
		return(D_IO_ERROR);
	}

        /* error recovery, if ioctl did not reset modes */  
	/* we know this is the last close because only one open
	 * is allowed when running exclusive. clean up all remaining 
	 * states and filters.
	 */
	if (sp->state & RHIPPI_EXCL) {
		/* blow away the EXCL filter if one is set */
		if (sp->filter_cnt) {
        		dfp = sp->rh_filters;
			for (i=0; i<sp->max_flt_cnt; i++, dfp++)
			{
				if (dfp->free == TRUE)
					continue;

				if (hctlr_recv_filter(dfp,FALSE) != D_SUCCESS)
					printf("rhippi_close: failed to unset filter\n");
				/* clean up filter and mark free */
				/* reset promiscuous mode if necessary */
        			if (dfp->hfilter.ulp == 0)
					sp->state &= ~RHIPPI_PROM;
				else
					dfp->hfilter.ulp = 0;
				dfp->hfilter.offset = 0;
				dfp->hfilter.bsize = 0;
				dfp->hfilter.min = 0;
				dfp->hfilter.max = 0;
				dfp->port = 0;	
				dfp->free = TRUE;
				sp->filter_cnt--;
			        RHLOG(RHLOG_LOW,("rhippi_close: error recovery: unset filter\n"));
				break;  /* this is the only filter */
			}
		} /* filter_cnt */

		if (sp->state & RHIPPI_MPPC) {
			RHLOG(RHLOG_LOW,("rhippi_close: error recovery: reset MPPC mode\n"));
			hctlr_ioctl(HIPPI_DEV_MPPC,FALSE,sp->excl_key);
			sp->state &= ~RHIPPI_MPPC;
			sp->first_write = TRUE;
		} else if (sp->state & RHIPPI_CONT) {
			RHLOG(RHLOG_LOW,("rhippi_close: error recovery: reset CONT mode\n"));
			hctlr_ioctl(HIPPI_DEV_CONT,FALSE,sp->excl_key);
			sp->state &= ~RHIPPI_CONT;
			sp->first_write = TRUE;
		}

		RHLOG(RHLOG_LOW,("rhippi_close: error recovery: reset EXCL mode\n"));
		hctlr_ioctl(HIPPI_DEV_EXCL,FALSE,sp->excl_key);
		sp->state &= ~RHIPPI_EXCL;
		sp->state &= ~RHIPPI_DST_ABORTED;
                sp->io_device->flag &= ~D_EXCL_OPEN;

	} /* EXCL mode TRUE */

	splx(oldpri);
	return (D_SUCCESS);
}


/*
 * rhippi_write:
 */
rhippi_write(dev, ior)
	dev_t	 dev;
	io_req_t ior;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	kern_return_t  rc;
        boolean_t      wait = FALSE; 	/* no wait */
	int            oldpri = SPLHIPPI();

	RHLOG_ENTRY(7, ("rhippi_write(dev=%x, ior=%x)\n", dev, ior));

	if (sp->state & RHIPPI_DOWN) {
		ior->io_error = TRUE;
		iodone(ior);
		splx(oldpri);
		return(D_IO_ERROR);
	}

	RHLOG(RHLOG_LOW,("rhippi_write: ior(%x)->recnum=%x cnt=%d io_op=%x\n",
				ior,ior->io_recnum,ior->io_count,ior->io_op));

	/* wire down request, convert to scatter-gather */
	rc = device_write_get_sg(ior, &wait);
 
        if (rc != KERN_SUCCESS) {
		ior->io_error = TRUE;
		iodone(ior);
		splx(oldpri);
        	return (rc);
	}

	/* io request is SG type */
        ior->io_op |= IO_SGLIST;

	RHLOG(RHLOG_LOW,("rhippi_write: ior(%x)->recnum=%x cnt=%d io_op=%x\n",
				ior,ior->io_recnum,ior->io_count,ior->io_op));

        /*
         * queue the write request for the device.
	 * If xmit queue is full then rhippi_xmit 
 	 * will chain it to the sp->rhsrc_q queue
	 * and pick it up after interrupt completion.
	 * rhippi_xmit will give it to controller
	 * to send out the HiPPI SRC channel.
         */
        simple_lock(&sp->rhsrc_lock);
        if (queue_empty(&sp->rhsrc_q)) 
               	enqueue_head(&sp->rhsrc_q,(queue_entry_t)ior);
        else 
               	enqueue_tail(&sp->rhsrc_q,(queue_entry_t)ior);
        simple_unlock(&sp->rhsrc_lock);
	rhippi_xmit(sp);

        /*
         * The io is now queued.  Wait for it if needed.
	 * check io complete for H/W errors
         */
	if (wait || (ior->io_op & IO_SYNC)) {
		splx(oldpri);
                iowait(ior);
		if (!ior->io_error)  
                	return(D_SUCCESS);
		else
			return(ior->io_error);
        }
	splx(oldpri);
        return (D_IO_QUEUED);
}


/*
 * rhippi_xmit:
 *
 *      This function will prepare the request for the HiPPI
 * 	SRC channel if there is room in the transmit queue
 * 	to process it.  rhippi_xmit is called at spl = SPLHIPPI.
 */
void
rhippi_xmit(sp)
        rhippi_softc_t *sp;
{
        io_req_t       ior = NULL;
        io_sglist_t    sg;
	rhippi_src_t   *cur_req;
	rhippi_src_t   *new_req;

	RHLOG_ENTRY(8, ("rhippi_xmit(sp=0x%x)\n",sp));

        /*
         *  While there is room in the interface transmit queue and requests
         *  queued from the kernel, get the scatter/gather list from request
         *  and queue it up with the controller interface.
         */
        while (sp->src_qtail->next_req != sp->src_qhead) {
        	simple_lock(&sp->rhsrc_lock);
               	ior = (io_req_t)dequeue_head(&sp->rhsrc_q);
		simple_unlock(&sp->rhsrc_lock);
		
	        if (!ior) 
			break;

                RHLOG(RHLOG_LOW,("rhippi_xmit: src_hd %x src_tail %x ior %x\n",
                                   sp->src_qhead,sp->src_qtail,ior));
		/*
		 *  Set new and current rhippi_src queue
		 *  pointers checking for empty queue
		 *  condition (src_qhead == 0).
		 */
		new_req = cur_req = sp->src_qtail;
		if (sp->src_qhead == 0) {
			sp->src_qhead = (rhippi_src_t *)new_req;
			RHSTATS(sp, rhsrc.qmiss++);
		} else {
			new_req = cur_req->next_req;
			RHSTATS(sp, rhsrc.qhit++);
		}

		assert(new_req->hreq.ior == 0);

		/*
		 * Get the information from io request.
		 * Store the length of packet, phys addr of sglist
		 * and reference to io request in the controller
		 * request block. The sg_list must be adjusted for
		 * the 32 byte ifield header and filler. The ifield
		 * is in the 1st 32 bit word and a fb_len is the 2nd 
		 * word only if a short first burst is required.
		 * If a short burst is required then adjust the sg_list
	         * to start after the first burst in the first page.
		 * MPC or CNT mode, ifield is provided with first write
		 * only.
		 */
		sg = (io_sglist_t)ior->io_sgp;

		RHLOG(RHLOG_LOW,("rhippi_xmit: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
					sg, sg->iosg_list[0].iosge_phys,
					sg->iosg_list[0].iosge_length));

                /* only if NOT MPC or CNT mode or if exclusive and 1st write only */
		if ( !(sp->state & (RHIPPI_MPPC | RHIPPI_CONT)) ||
		     ((sp->state & (RHIPPI_MPPC | RHIPPI_CONT)) && (sp->first_write)) ) {
			new_req->hreq.i_field  = *(int *)sg->iosg_list[0].iosge_phys;
		   	sg->iosg_list[0].iosge_phys   += 32;
		   	sg->iosg_list[0].iosge_length -= 32;
		   	sg->iosg_hdr.length -= 32;
		   	new_req->hreq.xfer_len     = ior->io_count-32;
		   	sp->first_write = FALSE;
		} else {
		   	new_req->hreq.xfer_len     = ior->io_count;
		}

		new_req->hreq.ior          = ior;
		new_req->hreq.sg_ptr       = sg;
		new_req->hreq.error_status = 0;
		new_req->hreq.chan_ctl     = 0;
		sp->wr_cnt++;

		RHLOG(RHLOG_LOW,("rhippi_xmit: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
					sg, sg->iosg_list[0].iosge_phys,
					sg->iosg_list[0].iosge_length));

		/*
		 * Set up new tail pointer
		 */
		if (new_req != cur_req)
			sp->src_qtail = (rhippi_src_t *)new_req;

		if (hctlr_src_q(new_req) != D_SUCCESS) {
			/* failed, mark controller SRC interface down */
			/* io done with error, take off SRC request queue */

			RHLOG(RHLOG_LOW,("rhippi_xmit: hctlr_src_q ERROR, req=%x\n",
       			                     new_req));
			RHSTATS(sp, rhsrc.reqfail++);

			/*
			 *  Clear this rhippi_src block and
			 *  bump pointer to next command
			 *  block checking for end of queue.
			 */
			new_req->hreq.i_field  = 0;
			new_req->hreq.ior      = 0;
			new_req->hreq.xfer_len = 0;
			new_req->hreq.sg_ptr   = 0;

			if (new_req == sp->src_qtail)
				sp->src_qhead = 0;
			else
				sp->src_qhead = new_req->next_req;

			ior->io_error = D_IO_ERROR;
			iodone(ior);
			sp->wr_cnt--;

			/* try to reset controller */
			if (hctlr_init() != D_SUCCESS) {
				sp->state |= RHIPPI_DOWN;
			 	break;
			}
		} 

	} /* while room in queue and ior's to send */

	if (sp->src_qtail->next_req == sp->src_qhead) {
		RHLOG(RHLOG_LOW,("rhippi_xmit: write queue full\n"));
		RHSTATS(sp, rhsrc.qfull++);
	}
}

#define h_min(a, b)     (((a) < (b)) ? (a) : (b))


/*
 * rhippi_read:
 */
rhippi_read(dev, ior)
	dev_t	 dev;
	io_req_t ior;
{
	rhippi_softc_t	*sp = &rhippi_softc;
	rh_data_t	*rhp;
	rhippi_dst_t	*dq;
	io_sglist_t	sg;
	kern_return_t	rc;
	int		oldpri = SPLHIPPI();

	RHLOG_ENTRY(9, ("rhippi_read(dev=%x, ior=%x)\n", dev, ior));

        /* Sanity checking....
	 * if the connection has dropped in CONT mode, return error 
	 * Also, check for SYNC io, this driver does not support
	 * sync reads since there is no way to abort a pending
	 * sync iowait and since HiPPI is basically an async input
	 * device there is no good way to time this event for timeout.
         */
	if ((sp->state & RHIPPI_DOWN) || (ior->io_op & IO_SYNC) ||
	    ((sp->state & RHIPPI_CONT) && (sp->state & RHIPPI_DST_ABORTED))) {
		ior->io_error = TRUE;
		iodone(ior);
		splx(oldpri);
		return(D_IO_ERROR);
	}

	RHLOG(RHLOG_LOW,("rhippi_read: ior(%x)->recnum=%x cnt=%d io_op=%x\n",
				ior,ior->io_recnum,ior->io_count,ior->io_op));

	/* wire down request, scatter-gather type */
	rc = device_read_alloc_sg(ior, ior->io_count);

	/* io request is SG type */
        ior->io_op |= IO_SGLIST;

        if (rc != KERN_SUCCESS) {
		ior->io_error = TRUE;
		iodone(ior);
		splx(oldpri);
        	return (rc);
	}

	RHLOG(RHLOG_LOW,("rhippi_read: ior(%x)->recnum=%x cnt=%d io_op=%x\n",
				ior,ior->io_recnum,ior->io_count,ior->io_op));

        /*
         * queue the read request.
         */
        simple_lock(&sp->rhdst_lock);
        if (queue_empty(&sp->rhdst_q)) 
               	enqueue_head(&sp->rhdst_q,(queue_entry_t)ior);
        else 
               	enqueue_tail(&sp->rhdst_q,(queue_entry_t)ior);
        simple_unlock(&sp->rhdst_lock);

	sp->rd_cnt++;

	/* .................RHIPPI_PKT_PENDING.....................
	 * if DST data has already arrived (pkt_done) and is waiting 
	 * for ior and this ior matches the filter, complete the IOR.
	 * This STATE covers the case where small completed packets 
	 * arrive before the read request is posted. hctlr_dst_halt() 
	 * was called from interrupt routine to prevent queue overflow.
	 */
	if (sp->state & RHIPPI_PKT_PENDING) {
		RHLOG(RHLOG_LOW,("rhippi_read: RHIPPI_PKT_PENDING!\n"));
		rhp = (rh_data_t*)&ior->io_recnum;	
		/* search pending queue for match */
        	simple_lock(&sp->rhpkt_lock);
		assert(!queue_empty(&sp->rhpkt_q));
                queue_iterate(&sp->rhpkt_q, dq, rhippi_dst_t*, links)
                {
			if ((rhp->ulp == dq->ulp) && 
				(rhp->data == (u_short)dq->filter_data)) {
                		RHLOG(RHLOG_LOW,("rhippi_read: bcopy(%x,%x,0x%x)\n",
					   dq->fb_ptr,ior->io_data,
					   h_min(dq->xfer_len,ior->io_count)));

				/* copy data to read buffer */
				bcopy(  dq->fb_ptr,
					ior->io_data,	
					h_min(dq->xfer_len,ior->io_count));

				/* if extra data, mark error */
				if (ior->io_count < dq->xfer_len) {
					ior->io_error = DST_ERR_EXBF;
				} else {
					ior->io_count = dq->xfer_len;
				}

				/* free pending pkt command buffer */
				dq->busy = FALSE;
             	   		queue_remove(&sp->rhpkt_q, dq, rhippi_dst_t*, links);

				/* complete read request */
        			simple_lock(&sp->rhdst_lock);
             	   		queue_remove(	&sp->rhdst_q,
						(hctlr_src_t*)ior,
						hctlr_src_t*, 
						links);
        			simple_unlock(&sp->rhdst_lock);
				RHSTATS(sp, rhdst.packets++);
				RHSTATS(sp, rhdst.bytes += ior->io_count);
				sp->rd_cnt--;
				iodone(ior);
				break;
			}
		}
		/* 
		 * if pending packet queue is empty, 
		 * set state, start channel, reset head ptr 
		 */
		if (queue_empty(&sp->rhpkt_q)) {
			sp->state &= ~RHIPPI_PKT_PENDING;
			hctlr_dst_continue();
		}
        	simple_unlock(&sp->rhpkt_lock);
	}
	/* .................RHIPPI_DST_PENDING...............
	 * if ior is not already processed (PKT_PENDING), and
	 * DST data has arrived and is waiting for ior and 
	 * this ior matches the filter then give the data buffers
	 * to controller immediately and copy over 1st burst
	 */
	if ((sp->state & RHIPPI_DST_PENDING) && (!(ior->io_op & IO_DONE))) {
		RHLOG(RHLOG_LOW,("rhippi_read: RHIPPI_DST_PENDING!\n"));
		rhp = (rh_data_t*)&ior->io_recnum;	
		if ((rhp->ulp == sp->dst_req->ulp) && 
			(rhp->data == (u_short)sp->dst_req->filter_data)) {

			if (ior->io_count > HCTLR_BURST_SIZE) {
				sp->dst_ior = ior;
				sg = ior->io_sgp;
				sg->iosg_list[0].iosge_phys += HCTLR_BURST_SIZE; 
				sg->iosg_list[0].iosge_length -= HCTLR_BURST_SIZE; 

				RHLOG(RHLOG_LOW,("rhippi_read: sg 0x%x data (0x%xV),(0x%xP)\n",
						sg,ior->io_data,sg->iosg_list[0].iosge_phys));

				hctlr_recv_buffer(sg);

				/* copy the 1st burst to read buffer */
                		RHLOG(RHLOG_LOW, ("rhippi_read: FB bcopy(%x(%x),%x(%x),0x%x)\n",
					   sp->dst_req->fb_ptr,kvtophys(sp->dst_req->fb_ptr),
					   ior->io_data,kvtophys(ior->io_data),HCTLR_BURST_SIZE));

				bcopy(  sp->dst_req->fb_ptr,
					ior->io_data,	
					HCTLR_BURST_SIZE);
			}
			else { 	/* this request is to small for DST data, abort */
				RHLOG(RHLOG_LOW,("rhippi_read: ior to small for data, abort\n"));
				hctlr_recv_buffer(0);
        			simple_lock(&sp->rhdst_lock);
             	   		queue_remove(	&sp->rhdst_q,
						(hctlr_src_t*)ior,
						hctlr_src_t*, 
						links);
        			simple_unlock(&sp->rhdst_lock);
				ior->io_error = DST_ERR_EXBF;
				iodone(ior);
				sp->rd_cnt--;
			}
			sp->state &= ~RHIPPI_DST_PENDING;
		}
	}
        /* new requests, schedule watchdog if not running */
	if (!sp->watchdog) {
		RHLOG(RHLOG_LOW,("rhippi_read: start watchdog\n"));
		sp->watchdog = TRUE;
		timeout(rhippi_watch, sp, RHIPPI_READ_POLL);
	}

        /*
         * The io is now queued, return
         */

	splx(oldpri);

        return (D_IO_QUEUED);
}



/*
 * rhippi_set_filter:
 *
 *      This function will get a free filter referenc and set
 *	it with the controller interface for async input.
 * 	rhippi_set_filter is called at spl = SPLHIPPI.
 */
rhippi_set_filter(ulp,offset,data,port)
        u_char		ulp;
	u_int		offset;
	u_short		data;
        ipc_port_t      port;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	rhippi_filter_t *rf;
        ipc_port_t      notify;
        kern_return_t   kr;

	RHLOG_ENTRY(10, ("rhippi_set_filter(ulp=%x,offset=%x,data=%x,port=%x)\n", 
				ulp,offset,data,port));

	if (sp->state & RHIPPI_DOWN) 
		return(D_IO_ERROR);

	/* check for full queue, limited to max request count */
	if (sp->filter_cnt >= sp->max_flt_cnt) {
		RHSTATS(sp, rhdst.fqfull++);
		return(D_IO_ERROR);
	}

	/* check for existing port reference or filter */
	for (rf = sp->rh_filters; rf->next != 0; rf++)  {
		if (rf->free == TRUE)
			continue;
		if ((rf->port == port) || 
			((rf->hfilter.ulp == ulp) &&
			(rf->hfilter.offset == offset) && 
			(rf->hfilter.min == data)))
		{
			RHLOG(RHLOG_LOW,("rhippi_set_filter: port(%x) or filter exists\n",port));
			return(D_INVALID_OPERATION);
		}
	}
		
        /*
         *  Find a free filter command, request a deadname
	 *  notification for the application reply port 
         *  and set filter with the controller interface
	 *  via hctlr_recv_filter() call.
         */
	for (rf = sp->rh_filters; rf->next != 0; rf++) {
		if (rf->free == FALSE)
			continue;

                RHLOG(RHLOG_LOW,("rhippi_set_filter: NEW filter 0x%x, filter cnt %d\n",
					rf,sp->filter_cnt));

		/*
		 * Store the ulp, offset, data and a
		 * reference to the application port to 
		 * the new filter command request block.
		 */
		rf->hfilter.ulp = ulp;
		rf->hfilter.offset = offset;
		rf->hfilter.bsize = 2;
		rf->hfilter.min = data;
		rf->hfilter.max = data;
		rf->port = port;	

		if ((hctlr_recv_filter(rf,TRUE)) == D_SUCCESS) {

			/* Request a deadname notification for all NEW
			 * application reply ports added to filter list. 
			 * Send notification to the master_device_port.
			 * Kernel will simply mark port dead.
			 */
			notify = ipc_port_make_sonce(master_device_port);
			kr = ipc_port_dnrequest(port, port, notify, &notify);
			while (kr != KERN_SUCCESS) {
				kr = ipc_port_dngrow(port);
				if (kr != KERN_SUCCESS) {
					RHSTATS(sp, rhdst.dn_errs++);
					return(D_IO_ERROR);	
				}
				kr = ipc_port_dnrequest(port, port, notify, &notify);
			}
			rf->free = FALSE;
			sp->filter_cnt++;
                	RHLOG(RHLOG_LOW,("rhippi_set_filter: SUCCESS, new filter cnt %d\n",
						sp->filter_cnt));
			break;
		} else {
                	RHLOG(RHLOG_LOW,("rhippi_set_filter: hctlr_recv_filter() FAILED\n"));
			return(D_INVALID_OPERATION);
		}

	} /* while room in filter queue */

        /* new filter, schedule watchdog if not running */
	if (!sp->watchdog) {
		RHLOG(RHLOG_LOW,("rhippi_read: start watchdog\n"));
		sp->watchdog = TRUE;
		timeout(rhippi_watch, sp, RHIPPI_READ_POLL);
	}

        if (ulp == 0)
		sp->state |= RHIPPI_PROM;

	return(D_SUCCESS);
}

/*
 * rhippi_unset_filter:
 *
 *      This function will unset an active filter 
 * 	rhippi_set_filter is called at spl = SPLHIPPI.
 */
rhippi_unset_filter(ulp,offset,data,port)
        u_char		ulp;
	u_int		offset;
	u_short		data;
        ipc_port_t      port;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	rhippi_filter_t *rf;
	rh_data_t	*rhp;
	io_req_t	ior;
	int		errors = 0;

	RHLOG_ENTRY(11, ("rhippi_unset_filter(ulp=%x,offset=%x,data=%x,port=%x)\n", 
				ulp,offset,data,port));

        /*
         *  Find the active filter 
         *  and unset filter with the controller interface
	 *  via hctlr_recv_filter() call.
         */
	for (rf = sp->rh_filters; rf->next != 0; rf++) {
		if (rf->free == TRUE)
			continue;

		/* found reference, complete any read ior's, cleanup and free filter */
		if (port == rf->port) {
                	RHLOG(RHLOG_LOW,("rhippi_unset_filter: REMOVE filter 0x%x\n",rf));
			if ((hctlr_recv_filter(rf,FALSE)) == D_SUCCESS) {
				/* clean up ior's associated with filter */
        			simple_lock(&sp->rhdst_lock);
				for (ior = (io_req_t)queue_first(&sp->rhdst_q);  
					!queue_end(&sp->rhdst_q, (queue_entry_t)ior);
					ior = ior->io_next)
               			{
        				simple_unlock(&sp->rhdst_lock);
					rhp = (rh_data_t*)&ior->io_recnum;	
					RHLOG(RHLOG_LOW,(" ior(%x), ulp(%x) in process\n",ior,rhp->ulp));
					if ((rhp->ulp == rf->hfilter.ulp) && 
						(rhp->data == (u_short)rf->hfilter.min)) {
						/* ior in process (DST intr), ignore */
						if (sp->dst_ior != ior) { 
							ior->io_error = DST_ERR_FILT;
							RHLOG(RHLOG_LOW,(" ior (0x%x) aborting\n",ior));
							RHSTATS(sp,rhdst.unset++);
							errors++;
						}
					}		
        				simple_lock(&sp->rhdst_lock);
               		 	}
        			simple_unlock(&sp->rhdst_lock);

				if (errors) 
					rhippi_read_cleanup(errors);

				/* clean up filter and mark free */
				rf->hfilter.ulp = 0;
				rf->hfilter.offset = 0;
				rf->hfilter.bsize = 0;
				rf->hfilter.min = 0;
				rf->hfilter.max = 0;
				rf->port = 0;	
				rf->free = TRUE;
				sp->filter_cnt--;
        			if (ulp == 0)
					sp->state &= ~RHIPPI_PROM;

                		RHLOG(RHLOG_LOW,("rhippi_unset_filter: SUCCESS\n",rf));
				return(D_SUCCESS);
			} else {
				/* this should never happen */
                		RHLOG(RHLOG_LOW,("rhippi_unset_filter: FAILED\n",rf));
				return(D_INVALID_OPERATION);
			}
		}

	} /* while not end of filter queue */

        RHLOG(RHLOG_LOW,("rhippi_unset_filter: filter->port(%x) not found\n",rf));
	return(D_INVALID_OPERATION);
}

/*
 * rhippi_getstat:
 *
 *      return driver status.
 */
rhippi_getstat(dev,flavor,status,count)
        dev_t           dev;
        int             flavor;
        dev_status_t    status;
        int             *count;
{
	rhippi_softc_t   *sp = &rhippi_softc;
	extern  hctlr_softc_t  hctlr_softc;

        RHLOG_ENTRY(12, ("rhippi_getstat(dev=%x,flav=%d,stat=0x%x=%x,cnt=%d)\n",
                                        dev,flavor,status,*(int *)status,*count));

        /*
         *  Sanity check entry to this function.
         */
	if (sp->state & RHIPPI_DOWN) 
		return(D_IO_ERROR);

        switch (flavor) {
#ifdef RHIPPI_STATS
        case RHIPPI_STATUS:           /* Return raw driver status */
                if (*count < sizeof(rhippi_stats_t))
                        return (D_INVALID_OPERATION);
                bcopy(&sp->stats,status,sizeof(rhippi_stats_t));
                break;
#endif
#ifdef HCTLR_STATS
        case HIPPI_STATUS:           /* Return hctlr interface status */
                if (*count < sizeof(hctlr_stats_t))
                        return (D_INVALID_OPERATION);
                bcopy(&hctlr_softc.stats,status,sizeof(hctlr_stats_t));
                break;
#endif 
        default:
                return (D_INVALID_OPERATION);
        }
        return (D_SUCCESS);
}


/*
 * rhippi_setinput:
 *      Call kernel to set DST packet filter for posted reads.
 */
rhippi_setinput(dev, recv_port, priority, filter, filter_count)
	dev_t           dev;
	mach_port_t     recv_port;
	int             priority;
	hlib_filter_t   *filter;
	u_int           filter_count;
{
        rhippi_softc_t  *sp = &rhippi_softc;
	u_char		ulp,bsize;
	u_short		data;
	int		offset,flag;
        kern_return_t   rc;
	int 		oldpri = SPLHIPPI();

	RHLOG_ENTRY(13,("rhippi_setinput(dev=%x, port=%x, pri=%x, filter=%x, count=%x)\n",
				dev, recv_port, priority, filter, filter_count));

        /*
         *  Sanity check entry to this function.
         */
	if (sp->state & RHIPPI_DOWN) {
		splx(oldpri);
		return(D_IO_ERROR);
	}

	if ((!recv_port) || (filter_count < sizeof(hlib_filter_t))) {
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	flag   = filter->flag;
	ulp    = filter->ulp;
	offset = filter->offset;

        /* ulp == 0 has special meaning: DST channel promiscuous mode. 
	 * the application wants exlusive use of the DST channel 
	 * and whats ALL traffic, so no filtering will be done
	 */
	if ((ulp == 0) && ((sp->state & RHIPPI_EXCL) == 0)) {
		printf("rhippi_setinput: ERROR: ulp=0 and not EXCL mode!\n");
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}	  

	if ((flag) && ((ulp == 0) && (sp->filter_cnt))) {
		printf("rhippi_setinput: ERROR: setting with ulp=0 and filter set!\n");
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}	  

	if ((offset) && (filter->bsize != 2)) {
		RHLOG(RHLOG_LOW, ("rhippi_setinput: bad filter!\n"));
		splx(oldpri);
		return (D_INVALID_OPERATION);
	}

	if (offset) {
		data   = filter->data;
		bsize  = filter->bsize;
	} else {
		data   = 0;
		bsize  = 0;
	}

	RHLOG(RHLOG_LOW, (" ulp=%x,offset=%x,bsize=%x,data=%x,flag=%x,port=%x\n",
				ulp,offset,bsize,data,flag,recv_port));

	if (flag) 
		rc = rhippi_set_filter(ulp,offset,data,recv_port);
	else 
		rc = rhippi_unset_filter(ulp,offset,data,recv_port);


	splx(oldpri);
	return(rc);
}


/*
 * rhippi_setstat:
 *
 *      set driver status.
 */
rhippi_setstat(dev,flavor,status,count)
        dev_t           dev;
        int             flavor;
        dev_status_t    status;
        int             count;
{
        rhippi_softc_t  *sp = &rhippi_softc;
	u_char		ulp;
	u_short		data;
	int		offset,bsize,flag;
	ipc_port_t	port;
	int 		oldpri = SPLHIPPI();

        RHLOG_ENTRY(14,("rhippi_setstat(dev=%x,flav=%d,stat=0x%x=%x,cnt=%d)\n",
                                        dev,flavor,status,*(int *)status,count));
        /*
         *  Sanity check entry to this function.
         */
	if (sp->state & RHIPPI_DOWN) {
		splx(oldpri);
		return(D_IO_ERROR);
	}

        switch (flavor) {
        case HIPPI_SRC_TIMEOUT:       /* debug only, change SRC channel timeout */

#ifdef RHIPPI_DEBUG
                RHLOG(RHLOG_LOW,(" ioctl: HIPPI_SRC_TIMEOUT\n"));
                if (count != 1) {
			splx(oldpri);
                        return (D_INVALID_OPERATION);
		}
		hctlr_ioctl(HIPPI_SRC_TIMEOUT,*(u_int*)status,sp->excl_key);
#else  
		splx(oldpri);
                return (D_INVALID_OPERATION);
#endif 
                break;

        case HIPPI_DST_TIMEOUT:       /* debug only, change DST channel timeout */

#ifdef RHIPPI_DEBUG
                RHLOG(RHLOG_LOW,(" ioctl: HIPPI_DST_TIMEOUT\n"));
                if (count != 1) {
			splx(oldpri);
                        return (D_INVALID_OPERATION);
		}
		hctlr_ioctl(HIPPI_DST_TIMEOUT,*(u_int*)status,sp->excl_key);
#else  
		splx(oldpri);
                return (D_INVALID_OPERATION);
#endif 
                break;

        case HIPPI_DEV_EXCL:     /* request exclusive use of hippi interface */

                RHLOG(RHLOG_LOW,(" ioctl: HIPPI_DEV_EXCL\n"));

                /* TRUE, set exclusive mode */
                if (*(u_int*)status) {
			if ((sp->io_device->flag & D_EXCL_OPEN) ||
				(sp->io_device->open_count > 1)) {
                		RHLOG(RHLOG_LOW,(" ERROR: flag(%x),open_count(%x)\n",
					sp->io_device->flag,sp->io_device->open_count));
				splx(oldpri);
                        	return (D_INVALID_OPERATION);
			}
                        /* ask for exlcusive interface use */
                        if (sp->state & RHIPPI_EXCL) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
			/* set interface in exclusive mode */
                        if (hctlr_ioctl(HIPPI_DEV_EXCL,TRUE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
                        sp->state |= RHIPPI_EXCL;
                        sp->io_device->flag |= D_EXCL_OPEN;
                }
                /* FALSE, unset exclusive mode */
                else {
                        if ((sp->state & RHIPPI_EXCL) == 0) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
			/* set interface out of exclusive mode */
                        if (hctlr_ioctl(HIPPI_DEV_EXCL,FALSE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
                        sp->state &= ~RHIPPI_EXCL;
                        sp->io_device->flag &= ~D_EXCL_OPEN;
                }
                break;

        case HIPPI_DEV_MPPC:       /* multiple packets per connection mode */

                RHLOG(RHLOG_LOW,(" ioctl: HIPPI_DEV_MPPC\n"));

                /* check: non-exclusive, CONT mode */
                if (!(sp->state & RHIPPI_EXCL) || (sp->state & RHIPPI_CONT)) {
			splx(oldpri);
                        return (D_INVALID_OPERATION);
                }

                /* TRUE, set multiple packets per connection mode */
                if (*(u_int*)status) {
			if (sp->state & RHIPPI_MPPC) {
				splx(oldpri);
				return (D_INVALID_OPERATION);
			}
                	if (hctlr_ioctl(HIPPI_DEV_MPPC,TRUE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
			sp->first_write = TRUE;
                        sp->state |= RHIPPI_MPPC;
                }
                /* FALSE, reset multiple packets per connection mode */
                else {
			if (!(sp->state & RHIPPI_MPPC)) {
				splx(oldpri);
				return (D_INVALID_OPERATION);
			}

			/* reset interface out of MPPC mode */
                	if (hctlr_ioctl(HIPPI_DEV_MPPC,FALSE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
                        else {
				sp->state &= ~RHIPPI_MPPC;
				sp->state &= ~RHIPPI_DST_ABORTED;
				sp->first_write = TRUE;
			}
                }
                break;

        case HIPPI_DEV_CONT:       /* buffer continuation support */

                RHLOG(RHLOG_LOW,(" ioctl: HIPPI_DEV_CONT\n"));

                /* check: non-exclusive, MPPC mode */
                if (!(sp->state & RHIPPI_EXCL) || (sp->state & RHIPPI_MPPC)) {
			splx(oldpri);
                        return (D_INVALID_OPERATION);
		}

                /* TRUE, reset one connection, one packet mode */
                if (*(u_int*)status) {
			if (sp->state & RHIPPI_CONT) {
				splx(oldpri);
				return (D_INVALID_OPERATION);
			}

                	if (hctlr_ioctl(HIPPI_DEV_CONT,TRUE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}

			sp->first_write = TRUE;
                        sp->state |= RHIPPI_CONT;
                }
                /* FALSE, reset one connection, one packet mode */
                else {
			if (!(sp->state & RHIPPI_CONT)) {
				splx(oldpri);
				return (D_INVALID_OPERATION);
			}

			/* reset interface out of CONT mode */
                	if (hctlr_ioctl(HIPPI_DEV_CONT,FALSE,sp->excl_key) != D_SUCCESS) {
				splx(oldpri);
                                return (D_INVALID_OPERATION);
			}
                        else {
				sp->state &= ~RHIPPI_CONT;
				sp->state &= ~RHIPPI_DST_ABORTED;
				sp->first_write = TRUE;
			}
                }
                break;

        case HIPPI_MON_START:	/* start i960 monitor */
		printf("rhippi_setstat: i960 start monitor..\n");
		printf("rhippi_setstat: NOT SUPPORTED\n");
		break;
        case HIPPI_MON_WRITE:	/* i960 monitor write */
		printf("rhippi_setstat: i960 monitor write message interface\n");
		printf("rhippi_setstat: NOT SUPPORTED\n");
		break;
        case HIPPI_MON_READ:	/* i960 monitor read */
		printf("rhippi_setstat: i960 monitor read message interface\n");
		printf("rhippi_setstat: NOT SUPPORTED\n");
		break;
        case HIPPI_MON_BREAK:	/* i960 monitor break command */
		printf("rhippi_setstat: i960 monitor break\n");
		printf("rhippi_setstat: NOT SUPPORTED\n");
		break;
        default:
		splx(oldpri);
                return (D_INVALID_OPERATION);
        }
	splx(oldpri);
        return (D_SUCCESS);
}


/*
 * rhippi_devinfo:
 *
 *      Routine to return driver information to kernel.
 */
rhippi_devinfo(dev, flavor, info)
        int     dev;
        int     flavor;
        char    *info;
{
        rhippi_softc_t  *sp = &rhippi_softc;
        int    result;

        RHLOG_ENTRY(15, ("rhippi_devinfo: dev=%x, flavor=%x)\n",dev, flavor));

	if (sp->state & RHIPPI_DOWN) 
		return(D_IO_ERROR);

        result = D_SUCCESS;

        switch (flavor) {
        case D_INFO_SGLIST_IO:
                *((char *) info) = TRUE;
                break;
        default:
                result = D_INVALID_OPERATION;
        }
        return(result);
}


/*
 * rhippi_dst_i:
 *
 *	Process all received frames.
 *
 */
rhippi_dst_i(unit,dst_req)
	u_int		unit;
	hctlr_dst_t	*dst_req;
{
	rhippi_softc_t   *sp = &rhippi_softc;
	io_req_t	ior;
	rh_data_t	*rhp;
	rhippi_dst_t	*dq;
	io_sglist_t	sg;

	RHLOG_ENTRY(16, ("rhippi_dst_i()\n"));

	/* check first burst and packet completion status */
	if (!sp->dst_req) {
		/* ..............1ST INTERRUPT....
		 * First burst is done and it was not aborted. 
		 * Find the io request for this data and give the
		 * the controller buffers (sg_list) for the rest 
		 * of the data on the DST channel. Cast the ior 
                 * with hctlr_src_t* and links since ior does not
                 * have a field definition for chaining. Location
                 * of next and prev pointers are same so its okay. 
 	 	 */ 
		sp->dst_req = dst_req; /* save reference */
        	simple_lock(&sp->rhdst_lock);
		for ( ior = (io_req_t)queue_first(&sp->rhdst_q);  
			!queue_end(&sp->rhdst_q, (queue_entry_t)ior);  
			ior = ior->io_next )
                {
			/* if promiscuous mode, take first ior */
			if (sp->state & RHIPPI_PROM) {
                		RHLOG(RHLOG_LOW, ("rhippi_dst_i: promiscuous mode\n"));
				sp->dst_ior = ior;
				break;
			}
			rhp = (rh_data_t*)&ior->io_recnum;	
			if ((rhp->ulp == dst_req->ulp) && 
				(rhp->data == (u_short)dst_req->filter_data) &&
					(rhp->timeout != 0)) {
				sp->dst_ior = ior;
				break;
			}
                }
        	simple_unlock(&sp->rhdst_lock);

		if (!sp->dst_ior) {
#ifdef RHIPPI_DEBUG
			int	i;
			u_char		*sg_data;
			/* no request found, if more data expected, abort */
                	RHLOG(RHLOG_LOW, ("rhippi_dst_i: 1st burst, no req pending!\n"));
			if (rhlog_id) {
				sg_data = dst_req->fb_ptr;
                		printf("rhippi_dst_i: no req, 1st 16 bytes of data:\n");
				for (i=0;i<16;i++)
					printf(" %x",(u_char *)sg_data[i]);
				printf("\n\n");

                		printf("rhippi_dst_i: all pending read requests: \n");
				/* print all pending read ior's */
        			simple_lock(&sp->rhdst_lock);
				for ( ior = (io_req_t)queue_first(&sp->rhdst_q);  
					!queue_end(&sp->rhdst_q, (queue_entry_t)ior);  
					ior = ior->io_next )
                		{
					rhp = (rh_data_t*)&ior->io_recnum;	
					printf(" pending ior: ulp %x, data %x, timeout %x\n",
							rhp->ulp,rhp->data,rhp->timeout);
				}
        			simple_unlock(&sp->rhdst_lock);
			}
#endif RHIPPI_DEBUG
			RHSTATS(sp, rhdst.noreqs++);
			/* if no request is found and packet is NOT done 
			 * set state to PENDING and the rhippi_read will
			 * pick up the request and process. If the packet
			 * is done with only 1 burst we simply throw it
			 * away.
			 */ 
			if (!dst_req->pkt_done) {
                		RHLOG(RHLOG_LOW,("rhippi_dst_i: RHIPPI_DST_PENDING\n"));
				sp->state |= RHIPPI_DST_PENDING;	
			} else {
                		RHLOG(RHLOG_LOW,("rhippi_dst_i: RHIPPI_PKT_PENDING\n"));
        			simple_lock(&sp->rhpkt_lock);
				if (dq = (rhippi_dst_t *)rhippi_get_dq(sp)) {
					/* store away in pkt pending queue */ 
					dq->ulp =  dst_req->ulp;
					dq->filter_data = (u_short)dst_req->filter_data;
					dq->xfer_len = dst_req->xfer_len;
					dq->ifield = dst_req->ifield;
                			RHLOG(RHLOG_LOW, ("rhippi_dst_i: bcopy(%x,%x,0x%x)\n",
					   	dst_req->fb_ptr,dq->fb_ptr,dst_req->xfer_len));
					bcopy(dst_req->fb_ptr,dq->fb_ptr,dst_req->xfer_len);
        				if (queue_empty(&sp->rhpkt_q)) {
               					enqueue_head(&sp->rhpkt_q,(queue_entry_t)dq);
						sp->state |= RHIPPI_PKT_PENDING;
						hctlr_dst_halt();
					}
        				else {
               					enqueue_tail(&sp->rhpkt_q,(queue_entry_t)dq);
					}
				} 
#ifdef RHIPPI_STATS
				else {
					RHSTATS(sp,rhdst.drop++);
				}
#endif RHIPPI_STATS
				simple_unlock(&sp->rhpkt_lock);
				assert(dq); 
				sp->dst_req = 0; /* we are done */
			}
			return;
		}

                RHLOG(RHLOG_LOW, ("rhippi_dst_i: 1st burst, processing ior(0x%x)!\n",ior));

		if (!dst_req->pkt_done) {
			/* 
			 * We have a good ior match and more data is expected.
			 * adjust 1st page pointer and length by hippi burst  
			 * amount and tell controller where to get the sg_list 
			 * buffers for rest of the data. 
			 * If the ior count is equal to HCTLR_BURST_SIZE, it is
			 * ok to go ahead and let the i960 complete a 2nd interrupt
			 * with no data.
			 */
			assert(dst_req->xfer_len == HCTLR_BURST_SIZE);
			if (ior->io_count < HCTLR_BURST_SIZE)  {
				/* not enough buffer space for more data */
               		 	RHLOG(RHLOG_LOW, ("rhippi_dst_i: 1st brst, req to small, abort!\n"));
				RHSTATS(sp, rhdst.smreq++);
				hctlr_recv_buffer(0);	
				return;
			}
			sg = ior->io_sgp;
			RHLOG(RHLOG_LOW,("rhippi_dst_i: sg_ptr 0x%x data (0x%xV),(0x%xP)\n",
						sg,ior->io_data,sg->iosg_list[0].iosge_phys));

			sg->iosg_list[0].iosge_phys += HCTLR_BURST_SIZE; 
			sg->iosg_list[0].iosge_length -= HCTLR_BURST_SIZE; 

                	RHLOG(RHLOG_LOW,("rhippi_dst_i: 1st brst, MORE data (0x%xP)\n",
						sg->iosg_list[0].iosge_phys));

			hctlr_recv_buffer(sg);

			/* copy the 1st burst to read buffer */
                	RHLOG(RHLOG_LOW, ("rhippi_dst_i: 1st burst bcopy(%x(%x),%x(%x),0x%x)\n",
					   dst_req->fb_ptr,kvtophys(dst_req->fb_ptr),
					   ior->io_data,kvtophys(ior->io_data),HCTLR_BURST_SIZE));

			bcopy(  dst_req->fb_ptr,
				ior->io_data,	
				HCTLR_BURST_SIZE);

                	RHLOG(RHLOG_LOW,("rhippi_dst_i: after bcopy, from (0x%x)=%x to (0x%x)=%x\n",
					dst_req->fb_ptr,*(u_char*)dst_req->fb_ptr,
					ior->io_data,*(u_char*)ior->io_data));

		}
		else {
			/* 
			 * We have a good ior match and no more data is expected.
			 * bcopy the first burst data and complete ior after checking
			 * for size.
			 */
			sg  = ior->io_sgp;
			RHLOG(RHLOG_LOW,("rhippi_dst_i: sg_ptr 0x%x data_ptrv 0x%x\n",sg,ior->io_data));

			/* copy the 1st burst to read buffer */
                	RHLOG(RHLOG_LOW,("rhippi_dst_i: 1st brst,done,bcopy(%x,%x,%x)\n",
					   dst_req->fb_ptr,ior->io_data,
					   HCTLR_BURST_SIZE));
			if (ior->io_count < dst_req->xfer_len) {
				bcopy(  dst_req->fb_ptr,
					ior->io_data,	
					ior->io_count);
					RHSTATS(sp, rhdst.smreq++);
				ior->io_error = DST_ERR_EXBF;
				RHSTATS(sp, rhdst.bytes += ior->io_count);
			} else {
				bcopy(  dst_req->fb_ptr,
					ior->io_data,	
					dst_req->xfer_len);
				ior->io_count = dst_req->xfer_len;
				RHSTATS(sp, rhdst.bytes += dst_req->xfer_len);
			}

			/* clean up read request */ 
			RHSTATS(sp, rhdst.packets++);
        		simple_lock(&sp->rhdst_lock);
             	   	queue_remove(&sp->rhdst_q, (hctlr_src_t*)ior, hctlr_src_t*, links);
        		simple_unlock(&sp->rhdst_lock);
			iodone(ior);
			sp->rd_cnt--;
			sp->dst_ior = 0;	/* no more outstanding DST data */
			sp->dst_req = 0;	/* no more outstanding DST data */
		}	
	} 
	else if (sp->dst_ior) {
		/* ..............2ND INTERRUPT....
		 * rest of DST data is complete and we have an ior.
		 * get ior reference and complete the read request. 
		 */
		ior = sp->dst_ior;
		sg = sp->dst_ior->io_sgp;
                RHLOG(RHLOG_LOW,("rhippi_dst_i: 2nd intr, completing ior(0x%x)!\n",ior));

                RHLOG(RHLOG_LOW,("rhippi_dst_i: 2nd intr, from (0x%x)=%x to (0x%x)=%x\n",
				dst_req->fb_ptr,*(u_char*)dst_req->fb_ptr,
				ior->io_data,*(u_char*)ior->io_data));

		if (dst_req->error_status) {
                	RHLOG(RHLOG_LOW, ("rhippi_dst_i: rest of data, with errors!\n"));
			RHSTATS(sp, rhdst.errors++);
			RHSTATS(sp, rhdst.packets++);
			ior->io_error = dst_req->error_status;
		} 
		else {
#ifdef RHIPPI_DEBUG 
			/*
		 	 * DST packet was GOOD, re-adjust sglist and 
		 	 * Update statistics and complete ior with io done. 
		 	 */
			int	i;
			u_char		*sg_data;

                	RHLOG(RHLOG_LOW, ("rhippi_dst_i: rest of data, GOOD!\n"));
                	RHLOG(RHLOG_LOW, ("rhippi_dst_i: ior 0x%x, total len %d!\n",
						ior,dst_req->xfer_len+HCTLR_BURST_SIZE));
			sg_data = (u_char *)(sg->iosg_list[0].iosge_phys);
			RHLOG(RHLOG_LOW,("rhippi_dst_i: sg_ptr 0x%x data_ptr 0x%x\n",sg,sg_data));
			RHLOG(RHLOG_LOW,("rhippi_dst_i: 1st 16 bytes of sg_data = "));
			for (i=0;i<16;i++)
				RHLOG(RHLOG_LOW,("%02x ",(u_char *)sg_data[i]));
		   	RHLOG(RHLOG_LOW,("\n"));
#endif RHIPPI_DEBUG
			RHSTATS(sp, rhdst.packets++);
			RHSTATS(sp, rhdst.bytes += (dst_req->xfer_len+HCTLR_BURST_SIZE));

			sg->iosg_list[0].iosge_phys -= HCTLR_BURST_SIZE; 
			sg->iosg_list[0].iosge_length += HCTLR_BURST_SIZE; 
			ior->io_count = dst_req->xfer_len + HCTLR_BURST_SIZE;
		}
		/* clean up read request */ 
        	simple_lock(&sp->rhdst_lock);
                queue_remove(&sp->rhdst_q, (hctlr_src_t*)ior, hctlr_src_t*, links);
        	simple_unlock(&sp->rhdst_lock);
		iodone(ior);
		sp->rd_cnt--;
		sp->dst_ior = 0;	/* no more outstanding DST data */
		sp->dst_req = 0;	/* no more outstanding DST data */
	} else {
		/* ..............2ND INTERRUPT...NO IOR.. 
		 * 2nd intr, no ior, ABORTED connection 
		 * no more outstanding or pending DST data 
		 */
		RHLOG(RHLOG_LOW,(" 2nd interrupt, no ior, done \n"));
		RHSTATS(sp, rhdst.abrt++);
		sp->dst_req = 0;
		sp->state &= ~RHIPPI_DST_PENDING;	
	}
	return;
}


/*
 * rhippi_src_i:
 *
 *	This routine will process completed transmit requests,  
 *	adjust the transmit queue pointer and process any
 *	new or outstanding transmit requests. When count is 
 *      greater than 1 all request will be completed in order
 * 	as given to controller.
 */
rhippi_src_i(unit,rh_req,count)
	u_int		unit;
	rhippi_src_t	*rh_req;
	int		count;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	io_sglist_t	sg;
	io_req_t	ior;

	RHLOG_ENTRY(17, ("rhippi_src_i(%x,0x%x,%d)\n",unit,rh_req,count));

	assert(rh_req == sp->src_qhead); 
	assert(rh_req->hreq.sg_ptr != 0); 
	assert(rh_req->hreq.ior != 0); 
	
#ifdef RHIPPI_STATS
	if (count > 1)
		RHSTATS(sp,rhsrc.chain++);
#endif RHIPPI_STATS

	while (count) {
        	/*
         	 * Get the information from controller 
		 * request. The sg_list must be adjusted for
		 * the 32 byte ifield header and filler.
         	 */
		assert(rh_req != sp->src_qtail->next_req); 

		ior = rh_req->hreq.ior;
		sg  = (io_sglist_t)phystokv(rh_req->hreq.sg_ptr);
                        
		RHLOG(RHLOG_LOW,("rhippi_src_i: req %x cci %x ior %x xlen %d sg %x err %x\n",
                                rh_req,htonl(rh_req->hreq.i_field),
                                rh_req->hreq.ior, rh_req->hreq.xfer_len,
                                rh_req->hreq.sg_ptr,rh_req->hreq.error_status));

		/* check for transmit errors */
		if (rh_req->hreq.error_status) {
			ior->io_error = rh_req->hreq.error_status;
			RHSTATS(sp,rhsrc.errors++);
		}
		else {
                        RHSTATS(sp,rhsrc.packets++);
                        RHSTATS(sp,rhsrc.bytes += (rh_req->hreq.xfer_len));
		}

		/* re-adjust sg_list, ifield  */
                if ( !(sp->state & (RHIPPI_MPPC | RHIPPI_CONT)) ||
		     ((sp->state & (RHIPPI_MPPC | RHIPPI_CONT)) && (sp->first_write)) ) {
			sg->iosg_list[0].iosge_phys   -= 32;
			sg->iosg_list[0].iosge_length += 32;
			sg->iosg_hdr.length += 32;
		}

		RHLOG(RHLOG_LOW,("rhippi_src_i: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
				sg, sg->iosg_list[0].iosge_phys,
				sg->iosg_list[0].iosge_length));

		/* io request complete */
		iodone(ior);
		sp->wr_cnt--;

		/* cleanup request ior field */
		rh_req->hreq.ior          = 0;

		/* goto next request and adjust count */
		rh_req = rh_req->next_req;
		count--;
	}

	/*
	 *  Adjust head and tail pointers and pick up any new
	 *  or queued requests that may have been blocked.
	 */
	if (rh_req == sp->src_qtail->next_req) {
		sp->src_qtail = rh_req;
		sp->src_qhead = 0;
	}
	else {
		sp->src_qhead = rh_req;
	}

	rhippi_xmit(sp);  
	return;
}


/*
 * rhippi_watch(sp)
 *
 *      This routine is the watchdog timer routine for read requests.
 * 	Will only be scheduled when read requests are pending.
 *
 * Called by:   kernel timer
 *
 * Returns on success
 *      nothing
 *
 * Possible errors
 *      - Read (DST) request exceeds timeout value
 *      - Calling application terminated improperly
 *        and port is dead.
 */
void
rhippi_watch(sp)
        rhippi_softc_t  *sp;
{
	rh_data_t       *rhp;
	rhippi_filter_t *dfp;
	rhippi_dst_t    *dq; 
	io_req_t	ior;
	int		i,errors = 0;
	int     	oldpri = SPLHIPPI();

	RHLOG_ENTRY(18,(""));

	/* no requests pending and no filters set, exit */
	if ((queue_empty(&sp->rhdst_q)) && (!sp->filter_cnt)) {
		sp->watchdog = FALSE;
        	splx(oldpri);
		return;
	}

        /* if running CONTinuation mode check status of DST connection */
	if (sp->state & RHIPPI_CONT) {
		if (hctlr_ioctl(HIPPI_DEV_CONT_STS,0,sp->excl_key) != D_SUCCESS) {
			RHLOG(RHLOG_LOW,("watch: HiPPI DST connection aborted!\n"));
			sp->state |= RHIPPI_DST_ABORTED;
		}
	}

	/* requests pending or filters active. 
	 * check filters and mark read requests 
	 * if receiving application port is dead.
	 * application termintated improperly
	 */
	dfp = sp->rh_filters;
        for (i = 0; i < sp->max_flt_cnt; i++, dfp++) 
        {
		if (dfp->free == TRUE) 
			continue;

		/* is port still active */
		if (io_active(&(dfp->port)->ip_object)) {
			continue;
		} else {
			RHLOG(RHLOG_LOW,(" dead recv port (0x%x) found\n",dfp->port));
			/* dead, find all requests */ 
        		simple_lock(&sp->rhdst_lock);
			for (ior = (io_req_t)queue_first(&sp->rhdst_q);  
				!queue_end(&sp->rhdst_q, (queue_entry_t)ior);  
				ior = ior->io_next )
               		{
        			simple_unlock(&sp->rhdst_lock);
				rhp = (rh_data_t*)&ior->io_recnum;	
				if ((rhp->ulp == dfp->hfilter.ulp) && 
					(rhp->data == (u_short)dfp->hfilter.min)) {
						/* if ior is being serviced (DST intr), leave it alone */
						if (sp->dst_ior != ior) { 
							RHLOG(RHLOG_LOW,(" ior (0x%x) dead port\n",ior));
							RHSTATS(sp,rhdst.dead++);
							errors++;
							ior->io_error = DST_ERR_DEAD;
						}
				}		
        			simple_lock(&sp->rhdst_lock);
                	}
        		simple_unlock(&sp->rhdst_lock);

			/* if there is a matching pending request
			 * that matches the dead filter then abort  
			 */
	 		if (sp->state & RHIPPI_DST_PENDING) {
				RHLOG(RHLOG_LOW,("rhippi_watch: DST_PENDING!\n"));
				if ((dfp->hfilter.ulp == sp->dst_req->ulp) && 
					(dfp->hfilter.min == sp->dst_req->filter_data)) {
					RHLOG(RHLOG_LOW,("rhippi_watch: filter matches, abort\n"));
					hctlr_recv_buffer(0);
				}
			}
			/* if there are pending small DST packets
			 * that match the dead filter then clear
			 * proper queue entries for reuse. drop
			 * packets.
			 */
	 		if (sp->state & RHIPPI_PKT_PENDING) {
				RHLOG(RHLOG_LOW,("rhippi_watch: PKT_PENDING!\n"));
        			simple_lock(&sp->rhpkt_lock);
				assert(!queue_empty(&sp->rhpkt_q));
                		queue_iterate(&sp->rhpkt_q, dq, rhippi_dst_t*, links)
                		{
					if ((dfp->hfilter.ulp == dq->ulp) && 
						(dfp->hfilter.min == dq->filter_data)) 
					{
						RHLOG(RHLOG_LOW,("rhippi_watch: pkt freed\n"));
						RHSTATS(sp,rhdst.dead++);
						/* free pending pkt command buffer */
						dq->busy = FALSE;
						queue_remove(	&sp->rhpkt_q, dq, 
								rhippi_dst_t*, links);
					}
				}
				if (queue_empty(&sp->rhpkt_q)) {
					sp->state &= ~RHIPPI_PKT_PENDING;
					hctlr_dst_continue();
				}
        			simple_unlock(&sp->rhpkt_lock);
			}

			/* unset, clean up, and mark free */
			if (hctlr_recv_filter(dfp,FALSE) != D_SUCCESS)
				printf("rhippi_watch: failed to unset filter\n");
			if (dfp->hfilter.ulp == 0)
				sp->state &= ~RHIPPI_PROM;
			else 
				dfp->hfilter.ulp = 0;
			dfp->hfilter.offset = 0;
			dfp->hfilter.min = 0;
			dfp->hfilter.max = 0;
			dfp->port = 0;	
			dfp->free = TRUE;
			sp->filter_cnt--;
                	RHLOG(RHLOG_LOW,(" unset filter(0x%x),cnt=%d,state=0x%x\n",
						dfp,sp->filter_cnt,sp->state));

        		/* error recovery, for EXCL mode  */  
			if ((sp->state & RHIPPI_EXCL) && (sp->filter_cnt == 0)) {
				if (sp->state & RHIPPI_MPPC) {
					RHLOG(RHLOG_LOW,("rhippi_watch: reset MPPC mode\n"));
					hctlr_ioctl(HIPPI_DEV_MPPC,FALSE,sp->excl_key);
					sp->state &= ~RHIPPI_MPPC;
			        	sp->first_write = TRUE;
				} else if (sp->state & RHIPPI_CONT) {
					RHLOG(RHLOG_LOW,("rhippi_watch: reset CONT mode\n"));
					hctlr_ioctl(HIPPI_DEV_CONT,FALSE,sp->excl_key);
					sp->state &= ~RHIPPI_CONT;
			        	sp->first_write = TRUE;
				}
				RHLOG(RHLOG_LOW,("rhippi_watch: reset EXCL mode\n"));
				hctlr_ioctl(HIPPI_DEV_EXCL,FALSE,sp->excl_key);
				sp->state &= ~RHIPPI_EXCL;
				sp->state &= ~RHIPPI_DST_ABORTED;
                        	sp->io_device->flag &= ~D_EXCL_OPEN;
			}

		} /* else, port is dead */
	} /* for filters in list */

	/* check IOR's for timeouts, dead filter residue, or aborted CONT */
        simple_lock(&sp->rhdst_lock);
	for (ior = (io_req_t)queue_first(&sp->rhdst_q);  
		!queue_end(&sp->rhdst_q, (queue_entry_t)ior);  
		ior = ior->io_next)
        {
        	simple_unlock(&sp->rhdst_lock);

		/* no filters, cleanup all pending non-marked requests */
		if ((!sp->filter_cnt) && (!ior->io_error)){
			/* if ior is being serviced (DST intr), leave it alone */
			if (sp->dst_ior != ior) { 
				RHLOG(RHLOG_LOW,(" no filters, cleanup ior(0x%x)\n",ior));
				ior->io_error = DST_ERR_FILT;
				RHSTATS(sp,rhdst.no_flt++);
				errors++;
			}
        		simple_lock(&sp->rhdst_lock);
			continue;
		}

		/* DST connect aborted, cleanup all pending non-marked requests */
		if ((sp->state & RHIPPI_CONT) && (sp->state & RHIPPI_DST_ABORTED)) {
			/* if ior is being serviced (DST intr), leave it alone */
			if (sp->dst_ior != ior) { 
				RHLOG(RHLOG_LOW,(" CONT aborted, cleanup ior(0x%x)\n",ior));
				ior->io_error = DST_ERR_CONT;
				RHSTATS(sp,rhdst.abrt++);
				errors++;
			}
        		simple_lock(&sp->rhdst_lock);
			continue;
		}

		/* no timeout set or already marked because of dead filter */
		rhp = (rh_data_t*)&ior->io_recnum;	
		if ((rhp->timeout == 0xff) || (ior->io_error)) {
        		simple_lock(&sp->rhdst_lock);
			continue;
		}

		/* exhausted timeout value, and request is not in process */
		if ((rhp->timeout == 0) && (sp->dst_ior != ior)) { 
			/* if ior is being serviced (DST intr), leave it alone */
			RHLOG(RHLOG_LOW,(" ior (0x%x) timeout\n",ior));
			ior->io_error = DST_ERR_TOWP;
			RHSTATS(sp,rhdst.timeout++);
			errors++;
		}		
		else {
			/* one second tick */
			rhp->timeout--;
		}
       		simple_lock(&sp->rhdst_lock);
	}
       	simple_unlock(&sp->rhdst_lock);
		
	/*
	 * If something has timed out or filter 
	 * is marked with dead port then remove 
	 * each request and issue io done.
 	 */
	if (errors)
		rhippi_read_cleanup(errors);

	/* if requests still pending or filters set, schedule watchdog */
       	simple_lock(&sp->rhdst_lock);
	if ((!queue_empty(&sp->rhdst_q)) || (sp->filter_cnt)) {
        	simple_unlock(&sp->rhdst_lock);
		RHLOG(RHLOG_LOW,(" watchdog continues\n"));
		sp->watchdog = TRUE;
		timeout(rhippi_watch, sp, RHIPPI_READ_POLL);
	} else {
        	simple_unlock(&sp->rhdst_lock);
		sp->watchdog = FALSE;
	}
        splx(oldpri);
	return;
}



/*
 * rhippi_read_cleanup(dfp)
 *
 *      This routine is the cleanup routine for all read ior's 
 * 	that are marked with an error.
 *
 * Called by:   rhippi_unset_filter, rhippi_watch
 *              called at SPLHIPPI.
 *
 * Returns on success
 *      nothing
 *
 */
void
rhippi_read_cleanup(count)
	int count;
{
	rhippi_softc_t  *sp = &rhippi_softc;
	io_req_t	ior;

	RHLOG_ENTRY(19,("rhippi_read_cleanup\n"));

	/*
	 * If something has timed out or filter 
	 * is marked with dead port then remove 
	 * each request and issue io done.
 	 */
	while (count) 
	{ 
       		simple_lock(&sp->rhdst_lock);
		if (queue_empty(&sp->rhdst_q)) {
			simple_unlock(&sp->rhdst_lock);
			RHLOG(RHLOG_LOW,("rhippi_rd_cleanup: ior read queue now empty\n"));
			break;  
		}
		for (ior = (io_req_t)queue_first(&sp->rhdst_q);  
			!queue_end(&sp->rhdst_q, (queue_entry_t)ior);
			ior = ior->io_next)
               	{
			if (ior->io_error & DST_ERR_CLEANUP) {
				RHLOG(RHLOG_LOW,("rhippi_rd_cleanup: ior (0x%x) aborted\n",ior));
				/* map the ior next,prev ptrs with hctlr_src_t */
             	   		queue_remove(&sp->rhdst_q, (hctlr_src_t*)ior, hctlr_src_t*, links);
				iodone(ior);
				sp->rd_cnt--;
				break;
			} else {
				continue;
			}
		}
       		simple_unlock(&sp->rhdst_lock);
		count--;
	}
	assert(!count);

#ifdef RHIPPI_DEBUG
	if (count)
		printf("rhippi_read_cleanup: cleanup problems, leftover count = %d\n",count);
#endif RHIPPI_DEBUG

	return;
}


/*
 * rhippi_stats:
 *
 *  Print Raw Hippi driver statistics to console.
 */
rhippi_stats()
{
#ifdef RHIPPI_STATS
	rhippi_softc_t  *sp = &rhippi_softc;
	rhippi_stats_t  *s = &sp->stats;

        db_printf("\n%d: Raw Hippi Write (SRC) stats:\n",sp->node_id);
	db_printf("packets=%u bytes=%u KB chained=%u errors=%u\n",
		s->rhsrc.packets,s->rhsrc.bytes/1024,s->rhsrc.chain,s->rhsrc.errors);
        db_printf("qfull=%u qhigh=%u qhit=%u qmiss=%u reqfail=%u\n",
		s->rhsrc.qfull,s->rhsrc.qhigh,s->rhsrc.qhit,
		s->rhsrc.qmiss,s->rhsrc.reqfail);

        db_printf("\n%d: Raw Hippi Read (DST) stats:\n",sp->node_id);
	db_printf("packets=%u bytes=%u KB errors=%u\n",
		s->rhdst.packets,s->rhdst.bytes/1024,s->rhdst.errors);
	db_printf("Total Read Requests/Packets Aborted (%d):\n",
		(s->rhdst.timeout+s->rhdst.dead+s->rhdst.no_flt+s->rhdst.unset+s->rhdst.abrt));
	db_printf("timeouts=%u deadport=%u no_filters=%u unset_filter=%u aborted=%u\n",
		s->rhdst.timeout,s->rhdst.dead,s->rhdst.no_flt,s->rhdst.unset,s->rhdst.abrt);

        db_printf("\n%d: Other Raw Hippi stats:\n",sp->node_id);
	db_printf("wr_cnt=%u rd_cnt=%u filter_cnt=%u max_filters=%u\n",
		sp->wr_cnt,sp->rd_cnt,sp->filter_cnt,sp->max_flt_cnt);
	db_printf("filter_qfull=%u qhigh=%u dn_errs=%d no_rd_reqs=%u drop=%u\n",
		s->rhdst.fqfull,s->rhdst.qhigh,s->rhdst.dn_errs,
		s->rhdst.noreqs,s->rhdst.drop);
#else
	db_printf("RHIPPI_STATS not defined\n");
#endif
	return 0;
}



/*
 * rhippi_debug:
 *
 *  Print Hippi debug information.
 *   - all packet descriptors, i960 SCB, rhippi_softc
 */
rhippi_debug(verbose)
	int verbose;
{
        rhippi_softc_t  *sp = &rhippi_softc;
        rhippi_filter_t *dfp;
        rhippi_src_t    *srcq;
        rhippi_dst_t    *dq;
        io_req_t        ior;
        rh_data_t       *rhp;
        int             i,oldpri = SPLHIPPI();

	if (sp->state == RHIPPI_ATTACHED) {
		db_printf("\nRaw HiPPI driver not initialized, open device\n"); 
		return 0;
	}

        db_printf("\nRHIPPI_SOFTC:\n");
        db_printf(" rhippi_softc = 0x%x\n",sp);
        db_printf("   rhsrc_q.next = 0x%x\n",sp->rhsrc_q.next);
        db_printf("   rhsrc_q.prev = 0x%x\n",sp->rhsrc_q.prev);
        db_printf("   rhdst_q.next = 0x%x\n",sp->rhdst_q.next);
        db_printf("   rhdst_q.prev = 0x%x\n",sp->rhdst_q.prev);
        db_printf("   src_qstart   = 0x%x\n",sp->src_qstart);
        db_printf("   src_qhead    = 0x%x\n",sp->src_qhead);
        db_printf("   src_qtail    = 0x%x\n",sp->src_qtail);
        db_printf("   rh_filters   = 0x%x\n",sp->rh_filters);
        db_printf("   state        = 0x%x\n",sp->state);
	if (sp->state & RHIPPI_UP)
		db_printf("                  RHIPPI_UP\n");
	if (sp->state & RHIPPI_DOWN)
		db_printf("                  RHIPPI_DOWN\n");
	if (sp->state & RHIPPI_OPEN)
		db_printf("                  RHIPPI_OPEN\n");
	if (sp->state & RHIPPI_CLOSE)
		db_printf("                  RHIPPI_CLOSE\n");
	if (sp->state & RHIPPI_DST_PENDING)
        	db_printf("                  RHIPPI_DST_PENDING\n");
	if (sp->state & RHIPPI_DST_ABORTED)
        	db_printf("                  RHIPPI_DST_ABORTED\n");
	if (sp->state & RHIPPI_PKT_PENDING)
        	db_printf("                  RHIPPI_PKT_PENDING\n");
	if (sp->state & RHIPPI_EXCL)
        	db_printf("                  RHIPPI_EXCL\n");
	if (sp->state & RHIPPI_PROM)
        	db_printf("                  RHIPPI_PROM\n");
	if (sp->state & RHIPPI_MPPC)
        	db_printf("                  RHIPPI_MPPC\n");
	if (sp->state & RHIPPI_CONT)
        	db_printf("                  RHIPPI_CONT\n");
        db_printf("   wr_cnt       = 0x%x\n",sp->wr_cnt);
        db_printf("   rd_cnt       = 0x%x\n",sp->rd_cnt);
        db_printf("   filter_cnt   = 0x%x\n",sp->filter_cnt);
        db_printf("   max_flt_cnt  = 0x%x\n",sp->max_flt_cnt);
        db_printf("   node_id      = 0x%x\n",sp->node_id);
        db_printf("   dst_ior      = 0x%x\n",sp->dst_ior);
        db_printf("   dst_req      = 0x%x\n",sp->dst_req);
        db_printf("   excl_key     = 0x%x\n",sp->excl_key);
        db_printf("   io_device    = 0x%x\n",sp->io_device);

        db_printf("\nRHIPPI WRITE REQUEST IN-PROCESS QUEUE:\n");
        srcq = sp->src_qstart;
        for (i = 0; i < RHIPPI_SRC_QSIZE; i++, srcq++) 
	{
		/* if a command is in use or verbose set, print */
		if ((srcq->hreq.ior) || (verbose)) {
            		db_printf(" WRITE_REQ = 0x%x\n",srcq);
               		db_printf(" srcq->hreq.links.next   = 0x%x\n",srcq->hreq.links.next);
         		db_printf(" srcq->hreq.links.prev   = 0x%x\n",srcq->hreq.links.prev);
               		db_printf(" srcq->hreq.i_field      = 0x%x\n",srcq->hreq.i_field);
                	db_printf(" srcq->hreq.fb_ptr       = 0x%x\n",srcq->hreq.fb_ptr);
                	db_printf(" srcq->hreq.fb_len       = 0x%x\n",srcq->hreq.fb_len);
                	db_printf(" srcq->hreq.ior          = 0x%x\n",srcq->hreq.ior);
                	db_printf(" srcq->hreq.sg_ptr       = 0x%x\n",srcq->hreq.sg_ptr);
                	db_printf(" srcq->hreq.chan_ctl     = 0x%x\n",srcq->hreq.chan_ctl);
                	db_printf(" srcq->hreq.sdone()      = 0x%x\n",srcq->hreq.sdone);
                	db_printf(" srcq->hreq.error_status = 0x%x\n",srcq->hreq.error_status);
                	db_printf(" srcq->hreq.xfer_len     = 0x%x\n",srcq->hreq.xfer_len);
                	db_printf(" srcq->next_req          = 0x%x\n",srcq->next_req);
		}
	}

	db_printf("\nRHIPPI WRITE REQUEST PENDING CHAIN:\n");
        simple_lock(&sp->rhsrc_lock);
        if (!queue_empty(&sp->rhsrc_q)) {
                queue_iterate(&sp->rhsrc_q, srcq, rhippi_src_t*, hreq.links)
                {
                        db_printf(" WRITE_REQ = 0x%x\n",srcq);
                        db_printf(" srcq->hreq.links.next   = 0x%x\n",srcq->hreq.links.next);
                        db_printf(" srcq->hreq.links.prev   = 0x%x\n",srcq->hreq.links.prev);
                        db_printf(" srcq->hreq.i_field      = 0x%x\n",srcq->hreq.i_field);
                        db_printf(" srcq->hreq.fb_ptr       = 0x%x\n",srcq->hreq.fb_ptr);
                        db_printf(" srcq->hreq.fb_len       = 0x%x\n",srcq->hreq.fb_len);
                        db_printf(" srcq->hreq.ior          = 0x%x\n",srcq->hreq.ior);
                        db_printf(" srcq->hreq.sg_ptr       = 0x%x\n",srcq->hreq.sg_ptr);
                        db_printf(" srcq->hreq.sdone()      = 0x%x\n",srcq->hreq.sdone);
                        db_printf(" srcq->hreq.error_status = 0x%x\n",srcq->hreq.error_status);
                        db_printf(" srcq->hreq.xfer_len     = 0x%x\n",srcq->hreq.xfer_len);
                        db_printf(" srcq->next_req          = 0x%x\n",srcq->next_req);
                }
        }
        simple_unlock(&sp->rhsrc_lock);
        
	db_printf("\nRHIPPI DST FILTERS:\n");
	dfp = sp->rh_filters;
        for (i = 0; i < sp->max_flt_cnt; i++, dfp++) 
        {
		/* print only filter in use or if verbose is set */
		if ((dfp->free == FALSE) || (verbose)) {
                	db_printf(" FILTER = 0x%x\n",dfp);
                	db_printf(" dfp->hfilter->links.next = 0x%x\n",dfp->hfilter.links.next);
                	db_printf(" dfp->hfilter->links.prev = 0x%x\n",dfp->hfilter.links.prev);
                	db_printf(" dfp->hfilter->ulp        = 0x%x\n",dfp->hfilter.ulp);
                	db_printf(" dfp->hfilter->offset     = 0x%x\n",dfp->hfilter.offset);
                	db_printf(" dfp->hfilter->bsize      = 0x%x\n",dfp->hfilter.bsize);
                	db_printf(" dfp->hfilter->min        = 0x%x\n",dfp->hfilter.min);
                	db_printf(" dfp->hfilter->max        = 0x%x\n",dfp->hfilter.max);
                	db_printf(" dfp->hfilter->ddone()    = 0x%x\n",dfp->hfilter.ddone);
                	db_printf(" dfp->free                = 0x%x\n",dfp->free);
                	db_printf(" dfp->port                = 0x%x\n",dfp->port);
                	db_printf(" dfp->next                = 0x%x\n",dfp->next);
		}
        }

	db_printf("\nRHIPPI READ REQUESTS IN-PROCESS:\n");
        simple_lock(&sp->rhdst_lock);
        if (!queue_empty(&sp->rhdst_q)) {
                for (ior = (io_req_t)queue_first(&sp->rhdst_q);
                     !queue_end(&sp->rhdst_q, (queue_entry_t)ior);
                     ior = ior->io_next)
                {
			rhp = (rh_data_t*)&ior->io_recnum;
                        db_printf(" READ_REQ = 0x%x\n",ior);
                        db_printf(" ior->io.io_next         = 0x%x\n",ior->io_next);
                        db_printf(" ior->io.io_prev         = 0x%x\n",ior->io_prev);
                        db_printf(" ior->io_count           = 0x%x\n",ior->io_count);
                        db_printf(" ior->io_sgp             = 0x%x\n",ior->io_sgp);
                        db_printf(" ior->io_recnum          = 0x%x\n",ior->io_recnum);
                        db_printf("      io_recnum.ulp      = 0x%x\n",rhp->ulp);
                        db_printf("      io_recnum.timeout  = 0x%x\n",rhp->timeout);
                        db_printf("      io_recnum.data     = 0x%x\n",rhp->data);
                }
        }
        simple_unlock(&sp->rhdst_lock);
        
	db_printf("\nRHIPPI DST PACKETS PENDING CHAIN:\n");
        simple_lock(&sp->rhpkt_lock);
        if (!queue_empty(&sp->rhpkt_q)) {
                queue_iterate(&sp->rhpkt_q, dq, rhippi_dst_t*, links)
                {
                        db_printf(" DST_PKT = 0x%x\n",dq);
                        db_printf(" dq->links.next  = 0x%x\n",dq->links.next);
                        db_printf(" dq->links.prev  = 0x%x\n",dq->links.prev);
                        db_printf(" dq->fb_ptr      = 0x%x\n",dq->fb_ptr);
                        db_printf(" dq->xfer_len    = 0x%x\n",dq->xfer_len);
                        db_printf(" dq->ulp         = 0x%x\n",dq->ulp);
                        db_printf(" dq->filter_data = 0x%x\n",dq->filter_data);
                        db_printf(" dq->ifield      = 0x%x\n",dq->ifield);
                        db_printf(" dq->busy        = 0x%x\n",dq->busy);
                }
        }
        simple_unlock(&sp->rhpkt_lock);
        splx(oldpri);

	return 0;
}

#endif NHIPPI > 0


