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
 * $Header: /afs/ssd/i860/CVS/mk/kernel/i860paragon/hippi/if_hippi.c,v 1.9 1994/11/18 20:43:33 mtm Exp $
 *
 * $Log: if_hippi.c,v $
 * Revision 1.9  1994/11/18  20:43:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/08/29  23:45:38  arlin
 * HiPPI driver asserts with debug kernel, line 728
 *
 *  Reviewer: Bernie Keany
 *  Risk: Low
 *  Benefit or PTS #: 10764
 *  Testing: HiPPI EATS
 *  Module(s): if_hippi.c -> hippi_src_i()
 *
 * Revision 1.7  1994/06/08  17:00:16  arlin
 * Updated for R1.3 IPI-3 support
 *
 * Revision 1.6  1993/08/26  16:31:52  arlin
 * watch dog timer was not getting
 * restarted after controller timeout
 *
 * Revision 1.5  1993/08/16  23:15:14  arlin
 * changes for FAB2 HiPPI board.
 *
 * Revision 1.4  1993/06/30  22:38:02  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/06/25  19:19:37  arlin
 * added dynamic buffer pool support
 *
 * Revision 1.2  1993/06/05  00:54:31  arlin
 * replace the hippi_bd_present() call with the
 * new generic expansion_id() function call.
 *
 * Revision 1.1  1993/05/27  21:48:56  arlin
 * Initial release of Hippi Driver
 *
 */
/*
 *      File:   if_hippi.c
 *      Author: Arlin Davis
 *              Intel Corporation Supercomputer Systems Division
 *      Date:   10/93
 *
 *      HiPPI-LE Network Device Driver.
 */

#include <hippi.h>

#if NHIPPI > 0

#include <sys/types.h>
#include <device/errno.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>
#include <device/net_io.h>
#include <chips/busses.h>
#include <device/conf.h>
#include <vm/vm_kern.h>
#include <i860paragon/expansion.h>
#include <i860paragon/hippi/hippidev.h>
#include <i860paragon/hippi/hctlr.h>
#include <i860paragon/hippi/if_hippi.h>

int  ifhip_probe(), ifhip_attach(), ifhip_init(), ifhip_output();
int  ifhip_setstat(), ifhip_getstat(), ifhip_devinfo();
int  ifhip_bld_srcq(), ifhip_bld_dstq(), ifhip_rd_ula();
int  ifhip_src_i(), ifhip_dst_i(), ifhip_stats();

void ifhip_xmit();

/* Externals */
extern  int expansion_id();
extern  int getbootint();
extern struct bus_device  *hippi_dinfo[];

caddr_t     ifhip_std  [ NHIPPI ] = { 0 };
struct bus_device *ifhip_dinfo[ NHIPPI ] = { 0 };

struct	bus_driver
  ifhip_driver = {
    ifhip_probe,    /*  *probe  */
    0,              /*  *slave  */
    ifhip_attach,   /*  *attach */
    0,              /*  *dgo    */
    ifhip_std,      /*  *addr   */
    "ifhip",        /*  *dname  */
    ifhip_dinfo,    /* **dinfo  */
    0,              /*  *mname  */
    0, 		    /* **minfo  */
    0               /*   flags  */
};

static char *ifhip_signon = "\
HiPPI Network Driver (ifhip) $Revision: 1.9 $\n\
Copyright (c) 1992,1993 Intel Corp., All Rights Reserved\n";

ifhip_softc_t 	ifhip_softc;
net_pool_t 	ifhip_pool   = NET_POOL_NULL;  /* 64KB buffer pool */
net_pool_t 	ifhip_fbpool = NET_POOL_NULL;  /*  1KB buffer pool */

/*
 * Set rhlog_id bits accordingly for debug messages:
 * (via kernel debugger, w rhlog_id 0x..)
 *
 *
 *  15 14 13 12  11 10 9 8  7 6 5 4  3 2 1 0
 *   |  |  |  |   |  | | |  | | | |  | | | |__ifhip_probe
 *   |  |  |  |   |  | | |  | | | |  | | |____ifhip_attach
 *   |  |  |  |   |  | | |  | | | |  | |______ifhip_bld_srcq
 *   |  |  |  |   |  | | |  | | | |  |________ifhip_open      
 *   |  |  |  |   |  | | |  | | | |___________ifhip_output
 *   |  |  |  |   |  | | |  | | |_____________ifhip_xmit  
 *   |  |  |  |   |  | | |  | |_______________ifhip_dst_i 
 *   |  |  |  |   |  | | |  |_________________ifhip_src_i
 *   |  |  |  |   |  | | |____________________ifhip_getstat
 *   |  |  |  |   |  | |______________________ifhip_setstat   
 *   |  |  |  |   |  |________________________ifhip_setinput    
 *   |  |  |  |   |___________________________ifhip_devinfo 
 *   |  |  |  |_______________________________ifhip_rd_ula  
 */

long iflog_id    = 0;
long iflog_level = 0x0000000f;


/*
 * ifhip_probe:
 *
 *  If called, assume we have a HIPPI controller attached
 */
/* ARGSUSED */
ifhip_probe(virt, bus)
	caddr_t			virt;
	struct bus_device	*bus;
{
	ifhip_softc_t  *sp = &ifhip_softc;
	u_char	*p;
	int	i;

	IFLOG_ENTRY(0, ("ifhip_probe(virt=%lx, bus=%lx)\n", virt, bus));

        /*
	 * Do we have a HiPPI controller and is it alive?
	 */
	if (hctlr_init() != D_SUCCESS)
		return 0;

	/*
	 *  signon message for driver
	 */
	printf(ifhip_signon);

	/*
	 *  Clear and initialize the ifhip_softc structure.
	 *  Set up virtual address of starting SRAM from the
	 *  master ifhip device info table.
	 *  The 960 scb will always be at the start of SRAM!
	 */
	for (i = 0, p = (u_char*)sp; i < sizeof(ifhip_softc_t); i++)
		*p++ = 0;

	sp->hippi_base	= (caddr_t)bus->phys_address;
	virt	        =  hippi_dinfo[HIPPI_DEV_SRAM]->address;
	sp->scb		= (i960_scb_t *)virt; 
	sp->node_id	= NODE_ID;
	sp->state	= IFHIP_PROBED;
	sp->kmsg 	= 0;
	sp->timer	= -1;

	return(1);
}

/*
 * ifhip_attach:
 *
 *  Attach the IFHIP  interface.  Read the EPROM version of the
 *  Universal Lan Address and initialize the ifnet structure.
 */
/* ARGSUSED */
ifhip_attach(bus)
	struct bus_device	*bus;
{

	ifhip_softc_t  *sp = &ifhip_softc;
	struct	ifnet	*ifp;

	IFLOG_ENTRY(1, ("\nifhip_attach(bus=%lx)\n", bus));

	/*
	 *  Make sure we're being called in the proper sequence.
	 */
	if (sp->state != IFHIP_PROBED) {
		printf("ifhip %d:%d attach called out of sequence - state = %d\n",
			sp->node_id, sp->ds_if.if_unit, sp->state);
		sp->state = IFHIP_SW_ERROR;
		return;
	} else
		sp->state = IFHIP_ATTACHED;

	/*
	 *  Read and display the Universal LAN Address found in prom.
	 */
	ifhip_rd_ula(sp, sp->ds_addr);

	ifp = &(sp->ds_if);
	ifp->if_unit          = bus->unit;
	ifp->if_mtu           = IFHIP_IP_MTU;
	ifp->if_flags         = IFF_SG;  
	ifp->if_header_size   = 32;		/* ifield and pad */	
	ifp->if_header_format = 0;
	ifp->if_address_size  = 6;
	ifp->if_address       = (char *)&sp->ds_addr[0];

	printf("\n%d: HiPPI-LE ULA (MAC) - %02x:%02x:%02x:%02x:%02x:%02x, MTU %d\n",
		sp->node_id,
		sp->ds_addr[0], sp->ds_addr[1], sp->ds_addr[2],
		sp->ds_addr[3], sp->ds_addr[4], sp->ds_addr[5],
		ifp->if_mtu);

	/* 
 	 * Initialize HiPPI network filter.
 	 * ULP to HiPPI-LE (0x04) and callback
 	 * routine to ifhip_dst_i().
 	 */
	sp->le_filter.ulp	= 0x04;
	sp->le_filter.offset	= 0;
	sp->le_filter.bsize	= 0;
	sp->le_filter.min	= 0;
	sp->le_filter.max	= 0;
	sp->le_filter.ddone	= ifhip_dst_i;

	/*  Initialize IF_NET queues */
	if_init_queues(ifp);

	return;
}

/*
 * ifhip_bld_srcq:
 *
 *  The routine will build and initialize the network driver 
 *  transmit queue. It is a circular queue.
 */

ifhip_bld_srcq(sp)
        ifhip_softc_t  *sp;
{
        ifhip_src_t    *src_q;
        int             i;

        IFLOG_ENTRY(2, ("ifhip_bld_srcq(sp=%lx)\n", sp));

        /* Wire down the transmit queue resources. */

        if (kmem_alloc_wired(	kernel_map,
				&sp->begin_sreq,
				(sizeof(ifhip_src_t)*IFHIP_SREQ_QCNT)) != KERN_SUCCESS)
                return(D_IO_ERROR);

        src_q = (ifhip_src_t *)sp->begin_sreq;

        /*
         * Build the transmit request queue
         */

        for (i = 0; i < IFHIP_SREQ_QCNT; i++, src_q++) {
                src_q->hctlr_req.i_field  = 0;
                src_q->hctlr_req.fb_ptr   = 0;
                src_q->hctlr_req.fb_len   = 0;
                src_q->hctlr_req.ior      = 0;
                src_q->hctlr_req.sg_ptr   = 0;
                src_q->hctlr_req.chan_ctl = 0;
                src_q->hctlr_req.sdone    = ifhip_src_i;
                src_q->hctlr_req.error_status = 0;
                src_q->hctlr_req.xfer_len = 0;
                src_q->next_req  = (ifhip_src_t *)src_q + 1;
        }

        /*
         *  Make SRC queue circular
         */
        src_q--;
        src_q->next_req  = (ifhip_src_t  *)sp->begin_sreq;

        /*
         *  Set PD head and tail pointers.
         */
        sp->sreq_qhead = 0;
        sp->sreq_qtail = (ifhip_src_t *)sp->begin_sreq;

        IFLOG(IFLOG_LOW, ("ifhip_bld_srcq: SRC_Q cnt %d sz %d begin %x hd %x tl %x\n",
                        IFHIP_SREQ_QCNT,(sizeof(ifhip_src_t)*IFHIP_SREQ_QCNT),
                        sp->begin_sreq,sp->sreq_qhead,sp->sreq_qtail));

        return(D_SUCCESS);
}


/*
 * ifhip_open:
 *
 *  Open the IFHIP driver.  The driver must have been probed
 *  and attached before being opened.
 */
/*ARGSUSED*/
ifhip_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	ifhip_softc_t  *sp = &ifhip_softc;
	IFLOG_ENTRY(3, ("ifhip_open(dev=%x, flag=%x)\n", dev, flag));

	/*
	 *  Make sure we are called in the proper sequence then initialize
	 *  the controller.
	 */

	if (sp->state & IFHIP_OPEN)
		return (D_SUCCESS);

        if (sp->state != IFHIP_ATTACHED) {
		IFLOG(IFLOG_HIGH, ("ifhip_open failed, interface not attached\n"));
		return (D_IO_ERROR);
	}

        if (hctlr_init(sp) != D_SUCCESS)
		return (D_IO_ERROR);

	if (ifhip_bld_srcq(sp) != D_SUCCESS)
		return (D_IO_ERROR);

        sp->state = IFHIP_OPEN;
	sp->ds_if.if_flags |= IFF_UP;
	sp->ds_if.if_flags |= IFF_RUNNING;
	return (D_SUCCESS);
}

/*
 * ifhip_output:
 *
 *	Call kernel to get frame into memory then have it call ifhip_xmit()
 *	to get it queued for the wire.
 */
/* ARGSUSED */
ifhip_output(dev, req)
	dev_t	 dev;
	io_req_t req;
{
	ifhip_softc_t  *sp = &ifhip_softc;

	IFLOG_ENTRY(4, ("ifhip_output(dev=%x, ior=%lx)\n", dev, req));

	/*
	 *  Sanity check entry to this function.
	 */
	if ((sp->ds_if.if_flags & IFF_RUNNING) == 0) {
		IFLOG(IFLOG_HIGH, ("ifhip_output failed, interface not running\n"));
		return (D_IO_ERROR);
	}
	else {
        	IFLOG(IFLOG_LOW,("ifhip_output: req %x unt %x op %x mode %x rn %x\n",req,
			req->io_unit,req->io_op,req->io_mode,req->io_recnum));
        	IFLOG(IFLOG_LOW,("ifhip_output: data %x cnt %x total %x rport %x\n",
			req->io_data,req->io_count,req->io_total,req->io_reply_port));

		return (net_write(&ifhip_softc.ds_if, ifhip_xmit, req));
	}
}


/*
 * ifhip_xmit:
 *
 *	This function will copy scatter/gather list to link list format if
 *	there is room in the transmit queue.
 *
 *	NOTE:	It is assumed that this routine is called with the SPL
 *			set to SPLHIPPI.  net_write() calls at splimp().  Routines
 *			that call ifhip_xmit() need to set the SPL at the proper
 *			level.
 */
/* ARGSUSED */
void
ifhip_xmit(unit)
	int	unit;
{
	ifhip_softc_t  *sp = &ifhip_softc;
	io_req_t       ior;
	io_sglist_t    sg;
	ifhip_src_t    *new_req;
	ifhip_src_t    *cur_req;
	int 		oldpri = SPLHIPPI();

	IFLOG_ENTRY(5, ("ifhip_xmit(unit=%d)\n", unit));

	/*
	 *  While there is room in the interface transmit PD queue and requests
	 *  queued from the kernel, get the scatter/gather list from request
	 *  and queue it up with the controller interface.
	 */

        while (sp->sreq_qtail->next_req != sp->sreq_qhead) {
		IFLOG(IFLOG_LOW,("ifhip_xmit: sreq_hd %x sreq_tail %x\n",
				   sp->sreq_qhead,sp->sreq_qtail));
		IF_DEQUEUE(&sp->ds_if.if_snd, ior);
		if (ior) {
                	/* request must be IO_SGLIST type */
			if (!(ior->io_op & IO_SGLIST)) {
				ior->io_error = 1;
				iodone(ior);
				continue;
			}

			/*
         		 *  Set new and current ifhip_src queue 
	 		 *  pointers checking for empty queue
         		 *  condition (sreq_qhead == 0).
         		 */
        		new_req = cur_req = sp->sreq_qtail;
        		if (sp->sreq_qhead == 0) {
                		sp->sreq_qhead = (ifhip_src_t *)new_req;
                		IFSTATS(sp, ifsrc.qmiss++);
        		} else {
                		new_req = cur_req->next_req;
                		IFSTATS(sp, ifsrc.qhit++);
        		}

        		/*
         		 * Get the information from io request.
         		 * Store the length of packet, phys addr of sglist
         		 * and reference to io request in the controller
			 * request block. The sg_list must be adjusted for
			 * the 32 byte ifield header and filler.
         		 */

			sg = (io_sglist_t)ior->io_sgp;
        		new_req->hctlr_req.i_field  = *(int *)sg->iosg_list[0].iosge_phys;
        		new_req->hctlr_req.ior      = ior;
        		new_req->hctlr_req.xfer_len = ior->io_count-32;
        		new_req->hctlr_req.sg_ptr   = sg;
        		new_req->hctlr_req.error_status = 0;

			IFLOG(IFLOG_LOW,("ifhip_xmit: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
					sg, sg->iosg_list[0].iosge_phys,
					sg->iosg_list[0].iosge_length));

			/* adjust sg_list, minus ifield, for controller */
			sg->iosg_list[0].iosge_phys   += 32;
			sg->iosg_list[0].iosge_length -= 32;
			sg->iosg_hdr.length -= 32;

			IFLOG(IFLOG_LOW,("ifhip_xmit: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
					sg, sg->iosg_list[0].iosge_phys,
					sg->iosg_list[0].iosge_length));
        		/*
         		 *  Set up new tail pointer for ifhip sreq queue
         		 */
        		if (new_req != cur_req)
                		sp->sreq_qtail = (ifhip_src_t *)new_req;

			if (hctlr_src_q(new_req) != D_SUCCESS) {
				/* failed, mark controller SRC interface down */
				/* io_done with error, take off if_sreq queue */

				IFLOG(IFLOG_LOW,("ifhip_xmit: hctlr_src_q ERROR, req=%x\n",
							new_req));
                		IFSTATS(sp, ifsrc.req_fail++);
                		
				/*
				 *  Clear this ifhip_src block and
                 		 *  bump pointer to next command 
		 		 *  block checking for end of queue.
                 		 */
        			new_req->hctlr_req.i_field   = 0;
        			new_req->hctlr_req.ior      = 0;
        			new_req->hctlr_req.xfer_len = 0;
        			new_req->hctlr_req.sg_ptr   = 0;
				
                		if (new_req == sp->sreq_qtail)  
                        		sp->sreq_qhead = 0;
                		else
                        		sp->sreq_qhead = new_req->next_req;

				ior->io_error = 1;
				iodone(ior);
				
				/* reset controller, mark down if necessary */
				if (hctlr_init() != D_SUCCESS) {
					sp->ds_if.if_flags &= ~IFF_UP;
					sp->ds_if.if_flags &= ~IFF_RUNNING;
					break;
				}
			}
		}
		else {
			/* no more requests on if_snd list */
			IFLOG(IFLOG_LOW,("ifhip_xmit: no more ior's to process!\n"));
			break; 		
		} 

	} /* while ifhip_src queue has room and no errors */
       
	if (sp->sreq_qtail->next_req == sp->sreq_qhead) {
                IFLOG(IFLOG_LOW, ("ifhip_xmit: PD queue full\n"));
                IFSTATS(sp, ifsrc.qfull++);
        }
	splx(oldpri);
}


/*
 * ifhip_dst_i:
 *
 *	Process all received frames.
 *
 */
ifhip_dst_i(unit,dst_req)
	u_int		unit;
	hctlr_dst_t	*dst_req;
{
	ifhip_softc_t   *sp = &ifhip_softc;
	struct  ifnet	*ifp = &sp->ds_if;
	io_sglist_t	sg;
	int		i,s;
	u_char		*sg_data;

	IFLOG_ENTRY(6, ("ifhip_dst_i(%x,0x%x)\n",unit,dst_req));
        IFLOG(IFLOG_LOW, ("ifhip_dst_i: pkt_done=%x sp->kmsg=%x\n",
				dst_req->pkt_done,sp->kmsg));

	/* check first burst and packet completion status */

	if (!dst_req->pkt_done) {
                IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst, packet not done!\n"));
		/* 
		 * First burst is done and more data is expected. 
		 * Give controller buffers for rest of the data
	 	 * on the DST channel. Try to get buffer from
		 * large (64KB) ifhip pool.
 	 	 */ 

		if ((sp->kmsg = net_kmsg_get_buf(ifhip_pool)) == IKM_NULL) {
			/* no buffers, tell controller to abort connection */
                	IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst, no lgbufs, abort!\n"));
			IFSTATS(sp, ifdst.no_lgbufs++);
			hctlr_recv_buffer(0);	
		} 
		else {
                	IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst, more data, get lgbufs!\n"));
			/* 
			 * adjust 1st page pointer and length by hippi burst plus   
			 * cache-line amount and tell controller where to get the sg_list 
			 * buffers for rest of the data. 
			 */
			assert(dst_req->xfer_len == HCTLR_BURST_SIZE);

			sg = (io_sglist_t)(net_kmsg_sg(sp->kmsg)->data);
			sg_data = (u_char *)phystokv(sg->iosg_list[0].iosge_phys);
			IFLOG(IFLOG_LOW,("ifhip_dst_i: sg_ptr 0x%x data_ptr 0x%x\n",sg,sg_data));
			sg->iosg_list[0].iosge_phys += HCTLR_BURST_SIZE; 
			sg->iosg_list[0].iosge_length -= HCTLR_BURST_SIZE; 

                	IFLOG(IFLOG_LOW,("ifhip_dst_i: 1st brst, more data (0x%x), use lgbufs\n",
						sg->iosg_list[0].iosge_phys));

			hctlr_recv_buffer(sg);

			/* copy the 1st burst+cache-line to kmsg */
                	IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst bcopy(%x,%x,%d)\n",
					   dst_req->fb_ptr,sg_data,HCTLR_BURST_SIZE));
			bcopy(  dst_req->fb_ptr,
				sg_data,	
				HCTLR_BURST_SIZE);
		}	
	} 
	else {
                IFLOG(IFLOG_LOW, ("ifhip_dst_i: packet is done!\n"));
		/* 
	 	 * Packet is complete. Either the packet fit in the
	 	 * first burst area or this is the second interrupt
	 	 * and packet processing should be completed.
	 	 * controller is not expecting anymore DST buffers. 
	 	 */
	
		if (sp->kmsg == 0) { 	
                	IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst, packet is done!\n"));
			/* 
			 * This is first burst and the packet is complete. 
			 * use the small burst size ifhip pool if packet
			 * is good. Only error that can occur here is the
			 * packet abort results since controller interface
			 * does not interrupt with a 1st burst error condition.
			 */
			if (dst_req->error_status) {
				/* error, update statistics */
                		IFLOG(IFLOG_LOW, ("ifhip_dst_i: packet was aborted!\n"));
                        	sp->ds_if.if_ierrors++;
                        	sp->ds_if.if_rcvdrops++;
				IFSTATS(sp, ifdst.errors++);
			} 
			else {
				if ((sp->kmsg = net_kmsg_get_buf(ifhip_fbpool)) == IKM_NULL) {
                			IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst no smbufs!\n"));
					IFSTATS(sp, ifdst.no_smbufs++);
				} 
				else {
                			IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst, done, use smbufs!\n"));
					/*
			 		 * small packet was GOOD, give it to kernel with byte 
			 		 * count and update statistics. set priority = TRUE.
			 		 * net_packet_pool needs SPL imp level.
			 		 */
					sg = (io_sglist_t)(net_kmsg_sg(sp->kmsg)->data);
					sg_data = (u_char *)phystokv(sg->iosg_list[0].iosge_phys);
                			IFLOG(IFLOG_LOW, ("ifhip_dst_i: 1st burst bcopy(%x,%x,%d)\n",
							   dst_req->fb_ptr,sg->iosg_list[0].iosge_phys,
							   dst_req->xfer_len));
					bcopy(  dst_req->fb_ptr,
						sg_data,
						dst_req->xfer_len);
					s = splimp();
					net_packet_pool(ifhip_fbpool,ifp,sp->kmsg,dst_req->xfer_len,TRUE);
					splx(s);
                       			sp->ds_if.if_ipackets++;
					IFSTATS(sp, ifdst.packets++);
					IFSTATS(sp, ifdst.bytes += dst_req->xfer_len);
					IFSTATS(sp,ifdst.sm_pkts++);
				}
			}
		} 
		else {
			/* 
			 * rest of DST data is complete, 2nd interrupt.
			 */
			if (dst_req->error_status) {
                		IFLOG(IFLOG_LOW, ("ifhip_dst_i: rest of data, with errors!\n"));
       		                /* re-cycle kernel message */
				net_kmsg_put_buf(ifhip_pool,sp->kmsg);
				IFSTATS(sp, ifdst.errors++);
                        	sp->ds_if.if_ierrors++;
                        	sp->ds_if.if_rcvdrops++;
			} 
			else {
                		IFLOG(IFLOG_LOW, ("ifhip_dst_i: rest of data, GOOD!\n"));

				/*
			 	 * large packet was GOOD, re-adjust sglist and 
				 * and give it to kernel with byte count.  
			 	 * Update statistics. set priority = TRUE.
			 	 * net_packet_pool needs SPL imp level.
			 	 */

                		IFLOG(IFLOG_LOW, ("ifhip_dst_i: kmsg 0x%x, total len %d!\n",
						sp->kmsg,dst_req->xfer_len+HCTLR_BURST_SIZE));

				sg = (io_sglist_t)(net_kmsg_sg(sp->kmsg)->data);
                		IFLOG(IFLOG_LOW, ("ifhip_dst_i: sg_ptr = 0x%x\n",sg));
#ifdef HCTLR_DEBUG
				sg_data = (u_char *)(sg->iosg_list[0].iosge_phys);
				IFLOG(IFLOG_LOW,("ifhip_dst_i: sg_ptr 0x%x data_ptr 0x%x\n",sg,sg_data));
				IFLOG(IFLOG_LOW,("ifhip_dst_i: 1st 16 bytes of sg_data = "));
				for (i=0;i<16;i++)
					IFLOG(IFLOG_LOW,("%02x ",(u_char *)sg_data[i]));
			   	IFLOG(HCLOG_LOW,("\n"));

				if (dst_req->xfer_len < 10000)
					IFSTATS(sp,ifdst.sm_pkts++);
				else if (dst_req->xfer_len > 60000)
					IFSTATS(sp,ifdst.bg_pkts++);
				else
					IFSTATS(sp,ifdst.md_pkts++);
#endif HCTLR_DEBUG
				sg->iosg_list[0].iosge_phys -= (HCTLR_BURST_SIZE); 
				sg->iosg_list[0].iosge_length += (HCTLR_BURST_SIZE); 

				/*  
				 * Give message to kernel, adjust size to account
				 * for the first burst that came in on the first
				 * interrupt.
				 */
				s = splimp();
				net_packet_pool(ifhip_pool,ifp,sp->kmsg,
						dst_req->xfer_len+HCTLR_BURST_SIZE,TRUE);
				splx(s);
                       		sp->ds_if.if_ipackets++;
				IFSTATS(sp, ifdst.packets++);
				IFSTATS(sp, ifdst.bytes += dst_req->xfer_len+HCTLR_BURST_SIZE);
			}
		}
		/* pkt complete, no more outstanding DST data */
		sp->kmsg = 0;	
	}
}


/*
 * ifhip_src_i:
 *
 *	This routine will process completed transmit requests,  
 *	adjust the transmit queue pointer and process any
 *	new or outstanding transmit requests. When count is 
 *      greater than 1 all request will be completed in order
 * 	as given to controller.
 */
ifhip_src_i(unit,if_req,count)
	u_int		unit;
	ifhip_src_t	*if_req;
	int		count;
{
	ifhip_softc_t   *sp = &ifhip_softc;
	io_sglist_t	sg;
	io_req_t	ior;

	IFLOG_ENTRY(7, ("ifhip_src_i(%x,0x%x,%d)\n",unit,if_req,count));

	assert(if_req == sp->sreq_qhead); 
	assert(if_req->hctlr_req.sg_ptr != 0); 
	assert(if_req->hctlr_req.ior != 0); 

	while (count) {
        	/*
         	 * Get the information from controller 
		 * request. The sg_list must be adjusted for
		 * the 32 byte ifield header and filler.
         	 */
		 assert(if_req != sp->sreq_qtail->next_req); 

		ior = if_req->hctlr_req.ior;
		sg  = (io_sglist_t)phystokv(if_req->hctlr_req.sg_ptr);
                        
		IFLOG(IFLOG_LOW,("ifhip_src_i: req %x cci %x ior %x xlen %d sg %x err %x\n",
                                if_req,htonl(if_req->hctlr_req.i_field),
                                if_req->hctlr_req.ior, if_req->hctlr_req.xfer_len,
                                if_req->hctlr_req.sg_ptr,if_req->hctlr_req.error_status));

		/* check for transmit errors */
		if (if_req->hctlr_req.error_status) {
			ior->io_error = if_req->hctlr_req.error_status;
 			sp->ds_if.if_oerrors++;
			IFSTATS(sp,ifsrc.errors++);
		}
		else {
                        sp->ds_if.if_opackets++;
                        IFSTATS(sp,ifsrc.packets++);
                        IFSTATS(sp,ifsrc.bytes += (if_req->hctlr_req.xfer_len));
#ifdef HCTLR_DEBUG
			if (if_req->hctlr_req.xfer_len < 10000)
				IFSTATS(sp,ifsrc.sm_pkts++);
			else if (if_req->hctlr_req.xfer_len > 60000)
				IFSTATS(sp,ifsrc.bg_pkts++);
			else
				IFSTATS(sp,ifsrc.md_pkts++);
#endif HCTLR_DEBUG
		}

		/* adjust sg_list for kernel, ifield padding */
		sg->iosg_list[0].iosge_phys   -= 32;
		sg->iosg_list[0].iosge_length += 32;
		sg->iosg_hdr.length += 32;

		IFLOG(IFLOG_LOW,("ifhip_src_i: sg_ptr 0x%x 1st phys 0x%x 1st len %d\n",
				sg, sg->iosg_list[0].iosge_phys,
				sg->iosg_list[0].iosge_length));

		/* io request complete */
		iodone(ior);

		/* goto next request and adjust count */
		if_req = if_req->next_req;
		count--;
	}

	/*
	 *  Adjust head and tail pointers and pick up any new
	 *  or queued requests that may have been blocked.
	 *  We are pointing to the req after the last one
	 *  just processed.
	 */

	if (if_req == sp->sreq_qtail->next_req) {
		sp->sreq_qtail = if_req;
		sp->sreq_qhead = 0;
	}
	else
		sp->sreq_qhead = if_req;

	ifhip_xmit(unit);  
}

/*
 * ifhip_getstat:
 *
 *	Call kernel to return status.
 */
/* ARGSUSED */

ifhip_getstat(dev,flavor,status,count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	int		*count;
{
	ifhip_softc_t  *sp = &ifhip_softc;
	struct ifnet *ifp = &sp->ds_if;
	IFLOG_ENTRY(8,("ifhip_getstat(dev=%x, flavor=%d, status = 0x%x=%x, count=%d)\n",
					dev, flavor, status, *(int *)status, *count));
	/*
	 *  Sanity check entry to this function.
	 */
	if ((sp->ds_if.if_flags & IFF_RUNNING) == 0) {
		printf("ifhip %d:%d netstat failed, interface not running\n",
			sp->node_id, sp->ds_if.if_unit);	
		return (D_IO_ERROR);
	}
	switch (flavor) {
   	case NET_STATUS:
        {
		register struct net_status *ns = (struct net_status *)status;

		ns->min_packet_size = ifp->if_header_size;

		/* we cannot return MTU if greater than 64k-1, 
		 * because server supports only u_short, so we
		 * have to lie in order support both TCP and RAW HiPPI
		 */
		if (ifp->if_mtu > IFHIP_IP_MTU)
			ns->max_packet_size = IFHIP_IP_MTU;
		else
			ns->max_packet_size = ifp->if_mtu;
		ns->header_format   = ifp->if_header_format;
		ns->header_size     = ifp->if_header_size;
		ns->address_size    = ifp->if_address_size;
		ns->flags           = ifp->if_flags;
		ns->mapped_size     = 0;
		*count = NET_STATUS_COUNT;
		break;
	}
	case NET_ADDRESS:
	{
		register int    addr_byte_count;
		register int    addr_long_count;
		register int    i;

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
#ifdef IFHIP_STATS
        case IFHIP_STATUS:                   /* Return driver status */
		if (*count < sizeof(ifhip_stats_t))
			return (D_INVALID_OPERATION);
		bcopy(&sp->stats,status,sizeof(ifhip_stats_t));
		break;	
#endif IFHIP_STATS
	default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}


/*
 * ifhip_setstat:
 *
 */
/* ARGSUSED */
ifhip_setstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	int		count;
{
	ifhip_softc_t     *sp = &ifhip_softc;
	u_char 		  *data = (u_char *)status;
	io_req_t       	  req;
	ipc_kmsg_t        kmsg;
	int		  oldpri,i;

	IFLOG_ENTRY(9,("ifhip_setstat(dev=%x, flavor=%d, status=0x%x=%02x, count=%d)\n",
					dev, flavor, status, *(int *)status, count));
	/*
	 *  Sanity check entry to this function.
	 */
	if ((sp->ds_if.if_flags & IFF_RUNNING) == 0) {
		printf("ifhip %d:%d setstat failed, interface not running\n",
				sp->node_id,sp->ds_if.if_unit);
		return (D_IO_ERROR);
	}
	switch (flavor) {
	case NET_STATUS:
	{
		register struct net_status *ns = (struct net_status *)status;
		IFLOG(IFLOG_LOW, ("ifhip_setstat: NET_STATUS->flags=%x\n",ns->flags));

		/*
		 *  Sanity check that the status structure passed is at
		 *  least as large as a net_status structure.
		 */
		if (count < NET_STATUS_COUNT) {
			IFLOG(IFLOG_HIGH,("ifhip_setstat: Bad count (%d)\n",(int)count));
			return (D_INVALID_OPERATION);
		}
		break;
	}
	case IFHIP_SET_POOL:         /* set buffer pools for network kmsg's */
	{
		int new_pool_count, new_pool_size;

		/* only allow one set pool from server */
		if (ifhip_pool != NET_POOL_NULL) {
			printf("ifhip %d:%d WARNING, buffer pools already exist\n", 
				sp->node_id,sp->ds_if.if_unit);
			return (D_IO_ERROR);
		}

		oldpri = SPLHIPPI();
		new_pool_count = *(int*)status; 
		status++;
		new_pool_size  = *(int*)status;

		if (net_kmsg_create_pool(
		    NET_POOL_SG,
		    new_pool_size,
		    new_pool_count,
     		    FALSE, "ifhip-big",&ifhip_pool) != KERN_SUCCESS) {
			printf("ifhip %d:%d error, unable to create buf pool sz=%d, cnt=%d\n", 
				sp->node_id,sp->ds_if.if_unit,
				new_pool_size, new_pool_count);
			sp->state = IFHIP_SW_ERROR;
			splx(oldpri);
			return (D_IO_ERROR);
		} else { 
			sp->pool_size = new_pool_size;
			sp->ds_if.if_mtu = new_pool_size; 
			net_kmsg_get_buf(ifhip_pool);	/* kick start allocation */
			printf("ifhip %d:%d large buffer pool created, sz=%d, cnt=%d\n",
				sp->node_id,sp->ds_if.if_unit,
				new_pool_size, new_pool_count);
		}
		if (net_kmsg_create_pool(
		    NET_POOL_SG,
		    HCTLR_BURST_SIZE,
		    20,
     		    FALSE, "ifhip-small",&ifhip_fbpool) != KERN_SUCCESS) {
			printf("ifhip %d:%d error, unable to create buf pool sz=%d, cnt=%d\n", 
				sp->node_id,sp->ds_if.if_unit,
				new_pool_size, new_pool_count);
			sp->state = IFHIP_SW_ERROR;
			splx(oldpri);
			return (D_IO_ERROR);
		} else { 
			net_kmsg_get_buf(ifhip_fbpool);	/* kick start allocation */
			printf("ifhip %d:%d small buffer pool created, sz=%d, cnt=%d\n",
				sp->node_id,sp->ds_if.if_unit,
				HCTLR_BURST_SIZE, 20);
		}
		splx(oldpri);
		break;
	}
	default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/*
 * ifhip_setinput:
 *
 *	Call kernel to set packet filter.
 *      The driver needs just one net_filter even though 
 *      we have 2 buffer pools (ifhip_pool,ifhip_fbpool).
 */
/* ARGSUSED */
ifhip_setinput(dev, receive_port, priority, filter, filter_count)
	dev_t		dev;
	mach_port_t	receive_port;
	int		priority;
	filter_t	filter[];
	u_int		filter_count;
{
	ifhip_softc_t   *sp = &ifhip_softc;
	int		rc;
	IFLOG_ENTRY(10,
		("ifhip_setinput(dev=%x, port=%x, pri=%x, filter=%x, count=%x)\n",
			dev, receive_port, priority, filter, filter_count));
	/*
	 *  Sanity check entry to this function.
	 */
	if (ifhip_softc.state != IFHIP_OPEN) {
		printf("ifhip %d:%d setinput failed, interface not opened\n",
			sp->node_id, sp->ds_if.if_unit);
		return (D_IO_ERROR);
	}
	/* 	
	 * set filter with both the controller and kernel 
	 */
	if (ifhip_pool) {
		/* 
 		 * Set DST filter for controller.
		 * flag=TRUE for setting a new filter. 
 		 */
		if (rc = hctlr_recv_filter(&sp->le_filter,1) != D_SUCCESS) {
			IFLOG(IFLOG_HIGH,
				("ifhip_setinput: hctlr_recv_filter failed (%x)\n",rc));
			return (rc);
		}
    		return(net_set_filter_pool(ifhip_pool,&ifhip_softc.ds_if, receive_port,
					priority, filter, filter_count));
	}	
	else { 
		/* if no pool was set return error */
		return (D_IO_ERROR);
	}
}

/*
 * ifhip_devinfo:
 *
 *	Routine to return information to kernel.
 */
/* ARGSUSED */
ifhip_devinfo(dev, flavor, info)
	int	dev;
	int	flavor;
	char	*info;
{
	register int	result;

	IFLOG_ENTRY(11,("ifhip_devinfo: dev=%x, flavor=%x)\n",dev, flavor));

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
 * ifhip_rd_ula:    Get and validate ULA for HiPPI board.
 */

ifhip_rd_ula(sp, addr_p)
	ifhip_softc_t   *sp;
	u_char		*addr_p;
{
	int	tmp,i;
	u_char	*ula = ((u_char *)(HIPPI_FLASH_ULA_ADDR)); 
	
	/*
	 *  Get system defined Universal Lan Address.
	 *  Located in the controller's FLASH at 0x8301fff8.
	 *  A double read is required to get a byte at a time
	 *  from the flash. 
	 */

	IFLOG_ENTRY(12, ("ifhip_rd_ula(sp=%x, addr_p=%x, ula_p=%x)\n", sp, addr_p, ula));

	for (i=0; i<6; i++, ula++) {
		addr_p[i] = inb(ula);
		addr_p[i] = inb(0x85000000);
	}

	/*
	 *  Validate it (as best we can).
	 */
	if ((addr_p[0] != 0) || (addr_p[1] != 0xaa) || (addr_p[2] != 0))
		printf ("\nifhip %d:%d  Warning: universal lan address (ULA) appears invalid\n",
			sp->node_id,sp->ds_if.if_unit);
	return;
}


/* 
 * print ifhip statistics
 */
ifhip_stats()
{
	ifhip_softc_t  *sp = &ifhip_softc;
	ifhip_stats_t   *s = &sp->stats;

	printf("\n%d: ifhip SRC stats:\n",sp->node_id);
	printf("packets=%u bytes=%u KB intr=%u errors=%u\n",
		s->ifsrc.packets,s->ifsrc.bytes/1024,s->ifsrc.intr,s->ifsrc.errors);
	printf("large pkts=%u medium pkts=%u small pkts=%u\n",
		s->ifsrc.bg_pkts,s->ifsrc.md_pkts,s->ifsrc.sm_pkts);
	printf("qhit=%u qmiss=%u req_fail=%u\n",
		s->ifsrc.qhit,s->ifsrc.qmiss,s->ifsrc.req_fail);

        printf("\n%d: ifhip DST stats:\n",sp->node_id);
	printf("packets=%u bytes=%u KB intr=%u errors=%u\n",
		s->ifdst.packets,s->ifdst.bytes/1024,s->ifdst.intr,s->ifdst.errors);
	printf("large pkts=%u medium pkts=%u small pkts=%u\n",
		s->ifdst.bg_pkts,s->ifdst.md_pkts,s->ifdst.sm_pkts);
	printf("no_smbufs=%u no_lgbufs=%u \n",s->ifdst.no_smbufs,s->ifdst.no_lgbufs);

	return;
}


#endif NHIPPI > 0

