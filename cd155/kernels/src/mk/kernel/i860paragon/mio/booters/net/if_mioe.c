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
 */
/*
 * HISTORY
 * $Log: if_mioe.c,v $
 * Revision 0.5  1994/11/18  20:45:39  mtm
 * Copyright additions/changes
 *
 * Revision 0.4  1993/06/30  22:40:13  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 0.3  1992/11/24  18:01:18  andyp
 * Bug fix from rkl for PTS bug #3568 (setting promisc. mode).
 *
 * Revision 0.2  1992/07/17  17:32:23  rkl
 * Brought it up to date with kernel driver.
 *
 * Revision 0.1  92/07/09  16:59:37  andyp
 * Initial checkin of Paragon files.
 *
 */
#include <mioe.h>
#if	NMIOE > 0

#include <device/errno.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>
#include <device/net_io.h>
#include <chips/busses.h>
#include <i860paragon/mio/if_mioe.h>

	int	mioe_probe(), mioe_attach(), mioe_init(), mioe_output();
STATIC	int	mioe_rcv(), mioe_xmt(), mioe_diag(), mioe_config();

STATIC	void	mioe_q_tx(), mioe_xmit(), mioe_watch(), mioe_requeue();
STATIC	void	mioe_tx_i(), mioe_bld_txq(), mioe_bld_scbs(), mioe_bld_rfa();
STATIC	void	mioe_start_ru(), mioe_read(), mioe_rd_eaddr(), mioe_start();

STATIC	caddr_t			mioe_std  [ NMIOE ] = { 0 };
STATIC	struct bus_device	*mioe_info[ NMIOE ] = { 0 };

struct	bus_driver
	mioe_driver = {
		mioe_probe,	/*  *probe  */
		0,		/*  *slave  */
		mioe_attach,	/*  *attach */
		0,		/*  *dgo    */
		mioe_std,	/*  *addr   */
		"mioe",		/*  *dname  */
		mioe_info,	/* **dinfo  */
		0,		/*  *mname  */
		0,		/* **minfo  */
		0		/*   flags  */
};

static char	*mioe_signon = "\
MIO Ethernet Driver $Revision: 0.5 $\n\
Copyright (c) 1992 Intel Corp., All Rights Reserved\n";

STATIC mioe_softc_t	mioe_softc;
STATIC xmit_q_t		tx_queue[ TX_QUEUE_SIZE ];

#ifdef	DEBUG
#define	DEBUG_LOG_LEVEL 0 /*(ELOG_ENTER | ELOG_HIGH | ELOG_MEDIUM | ELOG_LOW)*/
STATIC int elog_level =  DEBUG_LOG_LEVEL;
STATIC int elog_id    =  0;
#endif

/*
 * mioe_probe:
 *
 *	If called, assume we have an MIO card attached with a Ethernet
 *	interface.
 */
/* ARGSUSED */
STATIC
mioe_probe(virt, bus)
	caddr_t			virt;
	struct bus_device	*bus;
{
	mioe_softc_t	*sp = &mioe_softc;
	u_char		*p;
	int		i;

	ELOG_ENTRY(0, ("mioe_probe(virt=%x, bus=%x)\n", virt, bus));

	/*
	 *  Say hello
	 */
	printf(mioe_signon);

	/*
	 *  Clear the mioe_softc structure.
	 */
	for (i = 0, p = (u_char*)sp; i < sizeof(mioe_softc_t); i++)
		*p++ = 0;

	sp->map		  = (mioe_map_t*) (mio_dinfo[MIO_DEV_SRAM]->address
						+ 0x70000);
	sp->mioe_base	  = (u_short)(((u_long)bus->phys_address) >> 16);
	sp->scb_p	  = &sp->map->scb;
	sp->node_id	  = NODE_ID;
	sp->state	  = MIOE_PROBED;

	ELOG(ELOG_MEDIUM, ("mioe_probe: MIOE RAM physical = %x virtual = %x\n",
						bus->phys_address, sp->map));
	return(1);
}

/*
 * mioe_attach:
 *
 *	Attache the MIO Ethernet interface.  Read the EPROM version of the
 *	Ethernet address and initialize the ifnet structure.
 */
/* ARGSUSED */
STATIC
mioe_attach(bus)
	struct bus_device	*bus;
{
	mioe_softc_t	*sp = &mioe_softc;
	struct	ifnet	*ifp;

	ELOG_ENTRY(1, ("mioe_attach(bus=%x)\n", bus));

	/*
	 *  Make sure we're being called in the proper sequence.
	 */
	if (sp->state != MIOE_PROBED) {
		printf("MIO Ethernet %d:%d attach called out of sequence - state = %d\n", 
				bus->unit, sp->node_id, sp->state);
		sp->state = MIOE_SOFTWARE_INIT_ERROR;
		return;
	} else
		sp->state = MIOE_ATTACHED;

	/*
	 *  Read and display the Ethernet address found in prom.
	 */
	mioe_rd_eaddr(sp, sp->ds_addr);

	printf(" ethernet address - %02x:%02x:%02x:%02x:%02x:%02x  ",
			sp->ds_addr[0], sp->ds_addr[1], sp->ds_addr[2],
			sp->ds_addr[3], sp->ds_addr[4], sp->ds_addr[5]);

	ifp		      = &(sp->ds_if);
	ifp->if_unit	      = bus->unit;
	ifp->if_mtu           = ETHERMTU;
	ifp->if_flags         = IFF_BROADCAST;
	ifp->if_header_size   = sizeof(struct ether_header);
	ifp->if_header_format = HDR_ETHERNET;
	ifp->if_address_size  = 6;
	ifp->if_address       = (char *)&sp->ds_addr[0];

	/*
	 *  Initialize queues
	 */
	if_init_queues(ifp);
}

/*
 * mioe_open:
 *
 *	Open the MIO Ethernet driver.  The driver must have been probed
 *	and attached before being opened.
 */
/*ARGSUSED*/
mioe_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	mioe_softc_t	*sp = &mioe_softc;

	ELOG_ENTRY(2, ("mioe_open(dev=%x, flag=%x)\n", dev, flag));

	/*
	 *  Make sure we are called in the proper sequence then initialize
	 *  the chip.
	 */
	if ((sp->state != MIOE_ATTACHED) || (mioe_init(sp) == 0))
		return (ENXIO);

	/*
	 *  Allow the MIO to generate Ethernet interrupts and finish
	 *  setting flags.
	 */
	sp->state = MIOE_OPEN;
	sp->mode  = 0;
	sp->ds_if.if_flags |= IFF_UP;

	mio_interrupt_enable(MIO_INTENABLE_ETHER);
	return (0);
}

/*
 * mioe_init:
 *
 *	Reset and initialized the 82586.  This routine is called from
 *	mioe_open() and mioe_watch().
 */	
STATIC
mioe_init(sp)
	mioe_softc_t	*sp;
{
	int	status;
	int	oldpri = SPLNET();

	ELOG_ENTRY(3, ("mioe_init(sp=%x)\n", sp));

	/*
	 *  Do a hardware reset and configuration.
	 */
	if (status = mioe_hwrst(sp)) {
		/*
		 *  The 82586 has been known to wedge up so start a watchdog
		 *  timer to monitor command completion.  A running timer is
		 *  preferred to setting a timeout for each command given to
		 *  the 82586.
		 */
		timeout(mioe_watch, sp, MIOE_SLOW_POLL);

		sp->timer = -1;
		sp->ds_if.if_flags |= IFF_RUNNING;

		/*
		 *  Now that we're initialized, start any transmit requests
		 *  that may be queued.
		 */
		mioe_start(sp->ds_if.if_unit);
	} else
		sp->state = MIOE_HARDWARE_INIT_ERROR;
	
	splx(oldpri);
	return(status);
}

/*
 * mioe_hwrst:
 *
 *	This routine resets the 82586.  It is called by mioe_init() and
 *	assumes that we are at SPLNET.
 */
STATIC
mioe_hwrst(sp)
	mioe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;

	ELOG_ENTRY(4, ("mioe_hwrst(sp=%x)\n", sp));
	STATS(sp, resets++);

	/*
	 *  Some 82C501AD parts don't like coming up with loop back enabled.
	 *  The fix is to toggle the esi loop back mode.  We will force it out
	 *  of loopback here and then go into loop back after resetting the
	 *  82568;
	 */
	SET_LOOPBACK(sp, LOOPBACK_OFF);
	CHIP_RESET(sp);
	SET_LOOPBACK(sp, LOOPBACK_ON);
	
	/*
	 *  Build all 82568 data structures then tell the 82586 we're ready.
	 */
	miozero(sp->map, MIO_ETHER_RAM_SIZE);
	mioe_bld_scbs(sp);
	mioe_bld_rfa(sp);
	mioe_bld_txq(sp);
	CHANN_ATTN(sp);

	/*
	 *  Give the 82586 10 msec to respond.
	 */
	delay(10000);
	if (scb_p->scb_status != (SCB_SW_CX | SCB_SW_CNA)) {
		printf("MIO Ethernet %d:%d 82586 will not reset! Status = %x\n",
			sp->ds_if.if_unit, sp->node_id, scb_p->scb_status);
		return (0);
	}

	scb_p->scb_command = (SCB_ACK_CX | SCB_ACK_CNA);
	CHANN_ATTN(sp);

	/*
	 *  Diagnose and configure the 82586
	 */
	if ((mioe_diag(sp) == 0) || (mioe_config(sp) == 0))
		return (0);

	/*
	 *  Set the Ethernet address, start the Receive Unit, allow 82586
	 *  interrupts and take us out of loopback mode.
	 */
	if (mioe_set_eaddr(sp) == 0)
		return(0);

	mioe_start_ru(sp);
	SET_LOOPBACK(sp, LOOPBACK_OFF);
	return(1);
}

/*
 * mioe_bld_scbs:
 *
 *	This routine builds the scb structures (scp, iscp, an scb).
 */
STATIC void
mioe_bld_scbs(sp)
	mioe_softc_t	*sp;
{
	mioe_map_t	*map   = sp->map;
	v_scp_t		scp_p  = &map->scp;
	v_iscp_t	iscp_p = &map->iscp;
	v_scb_t		scb_p  = &map->scb;

	ELOG_ENTRY(5, ("mioe_bld_scbs(sp=%x)\n", sp));

	scp_p->scp_sysbus	= 0;			/* 16 bit bus mode */
	scp_p->scp_iscp		= VIRT_TO_MIOE(sp, iscp_p);
	scp_p->scp_iscp_base	= sp->mioe_base;

	iscp_p->iscp_busy	= 1;
	iscp_p->iscp_scb_offset	= VIRT_TO_MIOE(sp, scb_p);
	iscp_p->iscp_scb	= VIRT_TO_MIOE(sp, map);
	iscp_p->iscp_scb_base	= sp->mioe_base;

	scb_p->scb_status	= 0;
	scb_p->scb_command	= 0;
	scb_p->scb_cbl_offset	= 0;		/* this is not static */
	scb_p->scb_rfa_offset	= VIRT_TO_MIOE(sp, map->fd_q);
	scb_p->scb_crcerrs	= 0;
	scb_p->scb_alnerrs	= 0;
	scb_p->scb_rscerrs	= 0;
	scb_p->scb_ovrnerrs	= 0;
}

/*
 * mioe_bld_rfa:
 *
 *	This routine will build the Receive Frame Area (RFA).  Due to errata
 *	on the 82586, we do not use receive buffer chaining.  Each receive
 *	buffer is large enough to hold an entire (valid) Ethernet frame of
 *	1514 bytes.  There is one FD associate with each receive buffer.
 *	Unfortunately we can not assume the original associations will hold
 *	because short and long frames may be received that will cause no
 *	receive buffers or more than one to become associated with an FD.
 *	For this reason we must still deal with the RFA as if we were using
 *	receive buffer chaining when a frame is received.
 */
STATIC void
mioe_bld_rfa(sp)
	mioe_softc_t	*sp;
{
	v_fd_t		fd_p  = sp->map->fd_q;
	v_rx_buf_t	rbd_p = sp->map->rx_buf_q;
	int		i;

	ELOG_ENTRY(6, ("mioe_bld_rfa(sp=%x)\n", sp));

	sp->begin_fd = fd_p;

	for(i = 0; i < RX_QUEUE_SIZE; i++, fd_p++, rbd_p++) {

		fd_p->status      = 0;
		fd_p->command     = 0;
		fd_p->link_offset = VIRT_TO_MIOE(sp, fd_p + 1);
		fd_p->rbd_offset  = 0xffff;

		rbd_p->rbd.status	   = 0;
		rbd_p->rbd.next_rbd_offset = VIRT_TO_MIOE(sp, rbd_p + 1);
		rbd_p->rbd.buffer_addr	   = VIRT_TO_MIOE(sp, rbd_p->buf);
		rbd_p->rbd.buffer_base	   = sp->mioe_base;
		rbd_p->rbd.size		   = MIOE_BUF_SIZE;
	}

	fd_p--;
	sp->end_fd	  = fd_p;
	fd_p->link_offset = 0xffff;
	fd_p->command	  = AC_CW_EL;

	sp->map->fd_q[0].rbd_offset = VIRT_TO_MIOE(sp, &sp->map->rx_buf_q[0]);

	rbd_p--;
	sp->end_rbd		   = rbd_p;
	rbd_p->rbd.next_rbd_offset = 0xffff;
	rbd_p->rbd.size		  |= AC_CW_EL;
}

/*
 * mioe_bld_txq:
 *
 *	The routine will build and initialize the transmit queue.  Since the
 *	MIO memory does not support long word read/writes, a shadow queue
 *      of kernel virtual pointers to the 82586 transmit command queue is
 *	kept in driver memory.  This allows for fast/easy queue manipulation
 *	since we don't need to keep converting between MIO memory offsets
 *	and kernel virtual address.  The structure of the queue is illustrated
 *	below.  Note that all fields marked with a "~" are initialized only
 *	once.
 *
 *              +---------------------------------  n  ------------------+
 *              |                                                        |
 *              V                                                        |
 *           +------+                +-----+                     +-----+ |
 *           |~next |--------------->|     |------  n  --------->|     |-+
 *       +---|~cmd_p|            +---|     |                 +---|     |
 *     +-|---|~tbd_p|          +-|---|     |               +-|---|     |
 *   +-|-|---|~buf_p|        +-|-|---|     |             +-|-|---|     |
 *   | | |   +------+        | | |   +-----+             | | |   +-----+
 *   | | |    tx_q_t         | | |    tx_q_t             | | |    tx_q_t
 *   | | |                   | | |                       | | |
 *   | | |                   | | |   driver memory       | | |
 * ..|.|.|...................|.|.|.......................|.|.|................
 *   | | |                   | | |    MIO memory         | | |
 *   | | |                   | | |                       | | |
 *   | | |       +-----------|-|-|----------------  n  --|-|-|--------------+
 *   | | |       |           | | |                       | | |              |
 *   | | |       V           | | |                       | | |              |
 *   | | |  +---------+      | | |  +--------+           | | |  +--------+  |
 *   | | +->| status  |  +---|-|-+->|        |  +-  n  --|-|-+->|        |--+
 *   | |    | command |  |   | |    |        |  |        | |    |        |
 *   | |    |~next off|--+   | |    |        |--+        | |    |        |
 *   | |    |~TBD off |--+   | |    |        |           | |    |        |--+
 *   | |    | dst addr|  |   | |    |        |--+        | |    |        |  |
 *   | |    | length  |  |   | |    |        |  |        | |    |        |  |
 *   | |    +---------+  |   | |    +--------+  |        | |    +--------+  |
 *   | |       ac_t      |   | |      ac_t      |        | |      ac_t      |
 *   | |                 |   | |                |        | |                |
 *   | |    +---------+  |   | |    +--------+  |        | |    +--------+  |
 *   | +--->| count   |<-+   | +--->|        |<-+        | +--->|        |<-+
 *   |      |~next off|      |      |        |           |      |        |
 *   |      |~buf addr|--+   |      |        |--+        |      |        |--+
 *   |      +---------+  |   |      +--------+  |        |      +--------+  |
 *   |        tbd_t      |   |        tbd_t     |        |        tbd_t     |
 *   |                   |   |                  |        |                  |
 *   |      +---------+  |   |      +--------+  |        |      +--------+  |
 *   +----->|         |<-+   +----->|        |<-+        +----->|        |<-+
 *          | transmit|             |        |                  |        |
 *          | buffer  |             |        |                  |        |
 *          |         |             |        |                  |        |
 *          +---------+             +--------+                  +--------+
 *            char[]                  char[]                      char[]
 *
 */
STATIC void
mioe_bld_txq(sp)
	mioe_softc_t	*sp;
{
	xmit_q_t	*dvr_q = tx_queue;
	v_tx_q_t	mio_q  = sp->map->tx_cmd_q;
	int		i;

	ELOG_ENTRY(7, ("mioe_bld_txq(sp=%x)\n", sp));

	for (i = 0; i < TX_QUEUE_SIZE; i++, dvr_q++, mio_q++) {

		/*
		 *  Set driver side queue structure
		 */
		dvr_q->next  = dvr_q + 1;
		dvr_q->cmd_p = &mio_q->cmd;
		dvr_q->tbd_p = &mio_q->tbd;
		dvr_q->buf_p = &mio_q->buf[0];

		/*
		 *  Set MIO command block
		 */
		mio_q->cmd.command     = AC_CW_EL;
		mio_q->cmd.link_offset = VIRT_TO_MIOE(sp, mio_q + 1);
		mio_q->cmd.tbd_offset  = VIRT_TO_MIOE(sp, dvr_q->tbd_p);

		/*
		 *  Set MIO Transmit Buffer Descriptor
		 */
		mio_q->tbd.act_count       = TBD_SW_EOF;
		mio_q->tbd.next_tbd_offset = 0xffff;
		mio_q->tbd.buffer_addr	   = VIRT_TO_MIOE(sp, dvr_q->buf_p);
		mio_q->tbd.buffer_base	   = sp->mioe_base;
	}

	/*
	 *  Make queue circular
	 */
	dvr_q--;
	dvr_q->next = tx_queue;
	mio_q--;
	mio_q->cmd.link_offset = VIRT_TO_MIOE(sp, tx_queue[0].cmd_p);

	/*
	 *  Set head and tail pointers.
	 */
	sp->tx_qhead = 0;
	sp->tx_qtail = tx_queue;
}

/*
 * mioe_diag:
 *
 *	This routine asks the 82586 to perform its diagnostics.  Sine we
 *	don't do loop back tests, this is not a complete diagnostic test.
 */
STATIC
mioe_diag(sp)
	mioe_softc_t	*sp;
{
	ELOG_ENTRY(8, ("mioe_diag(sp=%x)\n", sp));

	/*
	 *  Give diagnose command.
	 */
	if (mioe_gen_cmd(sp, &sp->map->gen_cmd, AC_DIAGNOSE) == 0) {
		printf("MIO Ethernet %d:%d diagnostic failed\n",
						sp->ds_if.if_unit, sp->node_id);
		return(0);
	} else	
		return (1);
}

/*
 * mioe_config:
 *
 *	This routine does a standard config of the 82586.
 */
#define	SET_PROMISC(sp)		(sp->mode & MIOE_PROMISC ? 0x0001 : 0x0000)

STATIC
mioe_config(sp)
	mioe_softc_t	*sp;
{
	ac_t	*cb_p = &sp->map->gen_cmd;

	ELOG_ENTRY(9, ("mioe_config(sp=%x)\n", sp));

	/*
	 * This is the default board configuration from p2-28 from 82586 book.
	 */
	cb_p->cmd.configure.fifolim_bytecnt	= 0x080c;
	cb_p->cmd.configure.addrlen_mode	= 0x2600;
	cb_p->cmd.configure.linprio_interframe	= 0x6000;
	cb_p->cmd.configure.slot_time		= 0xf200;
	cb_p->cmd.configure.hardware		= SET_PROMISC(sp);
	cb_p->cmd.configure.min_frame_len	= 0x0040;

	if (mioe_gen_cmd(sp, cb_p, AC_CONFIGURE) == 0) {
		printf("MIO Ethernet %d:%d configuration failed\n",
						sp->ds_if.if_unit, sp->node_id);
		return(0);
	} else
		return (1);
}

/*
 *  mioe_set_eaddr:
 *
 *	This routine will set the individual Ethernet address of the 82586
 *	according to the address found in sp->ds_addr.
 */
STATIC
mioe_set_eaddr(sp)
	mioe_softc_t	*sp;
{
	v_ac_t	cb_p = &sp->map->gen_cmd;

	ELOG_ENTRY(10, ("mioe_set_eaddr(sp=%x)\n", sp));

	miocopy(sp->ds_addr, cb_p->cmd.iasetup, ETHER_ADD_SIZE);

	if (mioe_gen_cmd(sp, cb_p, AC_IASETUP) == 0) {
		printf("MIO Ethernet %d:%d address initialization failed\n",
					sp->ds_if.if_unit, sp->node_id);
		return(0);
	} else
		return (1);
}

/*
 * mioe_start_ru:
 *
 *	This routine starts the receive unit running.  Except for when the
 *	82586 is initialized, this only needs to be done when the receive
 *	queue was exhausted.  This should be a very rare event for an i860!
 *	The RFA will be reconstructed when this happens.
 */
STATIC void
mioe_start_ru(sp)
	mioe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;
	v_fd_t	fd_p  = sp->begin_fd;

	ELOG_ENTRY(11, ("mioe_start_ru(sp=%x)\n", sp));

	/*
	 *  Keep stats on empty receive queue.
	 */
	if (fd_p == NULL) {
		ELOG(ELOG_HIGH, ("mioe_start_ru: receive queue overrun\n"));
		STATS(sp, rx.q_empty++);
	}

	/*
	 *  If the RU just went not ready and it just completed an FD,
	 *  do not restart the RU.  This will wipe out the just completed FD.
	 *  There will be a second interrupt that will remove the FD via
	 *  mioe_rx_i() and thus call mioe_ru_start() which will then start
	 *  the RU if necessary.
	 */
	else if (fd_p->status & AC_SW_C)
		return;

	/*
	 *  If we get here, the RU is not ready so rebuild the RFA and
	 *  start the RU.
	 */
	ELOG(ELOG_HIGH,("mioe_start_ru: RU not ready - restarting\n"));
	STATS(sp, rx.restarts++);

	mioe_bld_rfa(sp);	/* sp->begin_fd set by this call */

	(void)mioe_wait(sp, 100);	/* XXX should check return */
	scb_p->scb_command    = SCB_RU_STRT;
	scb_p->scb_rfa_offset = VIRT_TO_MIOE(sp, sp->begin_fd);
	CHANN_ATTN(sp);
}

/*
 * mioe_gen_cmd(sp, cmd)
 *
 *	This routine will issue a general command then poll for command
 *	completion.
 */
STATIC
mioe_gen_cmd(sp, cb_p, cmd)
	mioe_softc_t	*sp;
	v_ac_t		cb_p;
	int		cmd;
{
	v_scb_t	scb_p = &sp->map->scb;
	int	i;

	ELOG_ENTRY(12, ("mioe_gen_cmd(sp=%x, cb_p=%x, cmd=%x)\n",
							sp, cb_p, cmd));
	/*
	 *  Wait for 82586 to be ready to start a new command then issue
	 *  the command passed.
	 */
	if (mioe_wait(sp, 1000))
		return (0);

	cb_p->ac_status	      = 0;
	cb_p->ac_command      = ((u_short)cmd | AC_CW_EL);
	scb_p->scb_cbl_offset = VIRT_TO_MIOE(sp, cb_p);
	scb_p->scb_command    = SCB_CU_STRT;

	CHANN_ATTN(sp);

	/*
	 *  Wait for command to complete.
	 */
	for(i = 1000; i; i--) {
		if (cb_p->ac_status & AC_SW_C)
			break;
		delay(10);
	}
		
	/*
	 *  Check for OK status.
	 */
	if ((i == 0) || ((cb_p->ac_status & AC_SW_OK) == 0)) {
		printf("MIO Ethernet %d:%d Command %d failed with status %x\n",
			sp->ds_if.if_unit, sp->node_id, cmd, cb_p->ac_status);
		return (0);
	}

	/*
	 *  Wait for 82586 to indicate it is no longer active.
	 */
	while (scb_p->scb_status & SCB_CUS_ACTV)
		delay(1);
	/*
	 * Acknowledge completion of the command.
	 */
	scb_p->scb_command = (scb_p->scb_status & SCB_SW_INT);
	CHANN_ATTN(sp);
	return(1);
}

/*
 * mioe_setinput:
 *
 *	Call kernel to set packet filter.
 */
/* ARGSUSED */
mioe_setinput(dev, receive_port, priority, filter, filter_count)
	dev_t		dev;
	mach_port_t	receive_port;
	int		priority;
	filter_t	filter[];
	u_int		filter_count;
{
	ELOG_ENTRY(13, 
	      ("mioe_setinput(dev=%x, port=%x, pri=%x, filter=%x, count=%x)\n",
			dev, receive_port, priority, filter, filter_count));

	/*
	 *  Sanity check entry to this function.
	 */
	if (mioe_softc.state != MIOE_OPEN) {
		printf("MIO Ethernet %d:%d setinput called in wrong state\n",
				mioe_softc.ds_if.if_unit, mioe_softc.node_id);
		return (ENXIO);
	}
	else	
		return (net_set_filter(&mioe_softc.ds_if, receive_port,
					priority, filter, filter_count));
}

/*
 * mioe_getstat:
 *
 *	Call kernel to return status.
 */
/* ARGSUSED */
mioe_getstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	u_int		*count;
{
	ELOG_ENTRY(14,("mioe_getstat(dev=%x, flavor=%x, status=%x, count=%x)\n",
						dev, flavor, status, count));

	/*
	 *  Sanity check entry to this function.
	 */
	if (mioe_softc.state != MIOE_OPEN) {
		printf("MIO Ethernet %d:%d getstat called in wrong state\n",
				mioe_softc.ds_if.if_unit, mioe_softc.node_id);
		return (ENXIO);
	}
	else	
		return (net_getstat(&mioe_softc.ds_if, flavor, status, count));
}

/*
 * mioe_setstat:
 *
 *	Say something witty here
 */
/* ARGSUSED */
mioe_setstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	u_int		count;
{
	struct net_status *ns = (struct net_status *)status;
	mioe_softc_t	  *sp = &mioe_softc;

	ELOG_ENTRY(15,("mioe_setstat(dev=%x, flavor=%x, status=%x, count=%x)\n",
						dev, flavor, status, count));
	/*
	 *  Sanity check entry to this function.
	 */
	if (sp->state != MIOE_OPEN) {
		printf("MIO Ethernet %d:%d setstat called in wrong state\n",
						sp->ds_if.if_unit, sp->node_id);
		return (ENXIO);
	}

	switch (flavor) {
	case NET_STATUS:
		ELOG(ELOG_LOW, ("mioe_setstat: NET_STATUS->flags=%x\n",
								ns->flags));
		/*
		 *  Sanity check that the status structure passed is at
		 *  least as large as a net_status structure.
		 */
		if (count < NET_STATUS_COUNT) {
			ELOG(ELOG_HIGH,("mioe_setstat: Bad count (%d)\n",
								(int)count));
			return (D_INVALID_OPERATION);
		}

		/*
		 *  Set 82586 for proper promiscuous mode.
		 */
		if (mioe_set_promisc(sp, ns->flags & IFF_PROMISC) == 0)
			return (D_IO_ERROR);
		break;

	case NET_ADDRESS: {
		union ether_cvt {
			u_char	addr[6];
			u_long	lwd[2];
		} *ec = (union ether_cvt *) status;

		/*
		 *  Sanity check that the status structure passed is at
		 *  least as large as our conversion structure.
		 */
		if (count < sizeof(*ec) / sizeof(int)) {
			ELOG(ELOG_HIGH,("mioe_setstat: Bad count (%d)\n",
								(int)count));
			return (D_INVALID_SIZE);
		}

                ec->lwd[0] = ntohl(ec->lwd[0]);
		ec->lwd[1] = ntohl(ec->lwd[1]);

		ELOG(ELOG_HIGH,
			("mioe_setstat: NET_ADDRESS = %x:%x:%x:%x:%x:%x\n",
				ec->addr[0], ec->addr[1], ec->addr[2],
				ec->addr[3], ec->addr[4], ec->addr[5]));

		miocopy(ec->addr, sp->ds_addr, ETHER_ADD_SIZE);
		if (!mioe_set_eaddr(sp))
			return (D_IO_ERROR);
		break;
                }

	default:
		return (D_INVALID_OPERATION);
	}

	return (D_SUCCESS);
}

/*
 * mioe_output:
 *
 *	Call kernel to get frame into memory then have it call mioe_start()
 *	to get it queued for the wire.
 */
/* ARGSUSED */
mioe_output(dev, ior)
	dev_t	 dev;
	io_req_t ior;
{
	ELOG_ENTRY(16, ("mioe_output(dev=%x, ior=%x)\n", dev, ior));

	/*
	 *  Sanity check entry to this function.
	 */
	if (mioe_softc.state != MIOE_OPEN) {
		ELOG(ELOG_HIGH, ("mioe_output: Wrong state (%d)\n",
							mioe_softc.state));
		return (ENXIO);
	}
	else	
		return (net_write(&mioe_softc.ds_if, mioe_start, ior));
}

/*
 * mioe_start:
 *
 *	This function will copy transmit data to MIO memory if there is
 *	room in the transmit queue.  If the 82586 is not currently sending
 *	something on the wire it will start a transmit.
 *
 *	NOTE:	It is assumed that this routine is called with the SPL
 *		set to SPLNET.  Net_write() calls at splimp().  Routines
 *		that call mioe_start() need to set the SPL at the proper
 *		level.
 */
/* ARGSUSED */
void STATIC 
mioe_start(unit)
	int	unit;
{
	mioe_softc_t	*sp = &mioe_softc;
	io_req_t	req;			/* pointer value */

	ELOG_ENTRY(17, ("mioe_start(unit=%d)\n", unit));

	/*
	 *  While there is room in the transmit queue and requests queued
	 *  from the kernel, get them copied to MIO memory and linked into
	 *  the 82586 queue.
	 */
	while (sp->tx_qtail->next != sp->tx_qhead) {
		IF_DEQUEUE(&sp->ds_if.if_snd, req);
		if (req) {
			mioe_q_tx(sp, req);
			iodone(req);
		} else {
			break;
		}
	}

	/*
	 *  Check for full queue so statistics can be kept.
	 */
	if (sp->tx_qtail->next == sp->tx_qhead) {
		ELOG(ELOG_MEDIUM, ("mioe_start: tx queue full\n"));
		STATS(sp, tx.q_full++);
	}

	/*
	 *  Start the 82586 on what's queued up.
	 */
	mioe_xmit(sp);
}

/*
 * mioe_q_tx:
 *
 *	This routine will copy user data into MIO memory and queue the
 *	request for transmission.  This allows multiple transmit commands
 *	to be chained together when the system can generate transmit requests
 *	faster than the network can take them.  See mioe_bld_txq() for details
 *	on how queue is managed.
 */
STATIC void
mioe_q_tx(sp, req)
	mioe_softc_t	*sp;
	io_req_t	req;
{
	struct ether_header *eh_p = (struct ether_header *)req->io_data;
	u_short		    len;
	xmit_q_t	    *cur;
	xmit_q_t	    *new;
	v_tx_t		    cb_p;

	ELOG_ENTRY(18, ("mioe_q_tx(sp=%x, req=%x)\n", sp, req));

	/*
	 *  Set new and current queue pointers checking for empty queue
	 *  condition (tx_qhead == 0).
	 */
	new = cur = sp->tx_qtail;
	if (sp->tx_qhead == 0) {
		sp->tx_qhead = new;
		STATS(sp, tx.q_miss++);
	} else {
		new = cur->next;
		STATS(sp, tx.q_hit++);
	}

	cb_p	      = new->cmd_p;
	cb_p->status  = 0;
	cb_p->command = (AC_CW_EL | AC_TRANSMIT | AC_CW_I);
	cb_p->length  = (u_short)(eh_p->ether_type);

	new->tbd_p->act_count = MAX(req->io_count - sizeof(struct ether_header),
					 ETHERMIN) | TBD_SW_EOF;
	len = req->io_count - sizeof(struct ether_header);

	miocopy(eh_p->ether_dhost, cb_p->dest_addr,ETHER_ADD_SIZE);
	miocopy(req->io_data + sizeof(struct ether_header), new->buf_p, len);

	/*
	 *  It is important that we don't expose the new command block
	 *  (by clearing the EL bit) until we are completely done initializing
	 *  it.
	 */
	if (new != cur) {
		cur->cmd_p->command = AC_TRANSMIT;
		sp->tx_qtail = new;
	}
	
	/*
	 *  Update stats
	 */
	sp->ds_if.if_opackets++;
	STATS(sp, tx.packets++);
	STATS(sp, tx.bytes += len);
}

/*
 * mioe_xmit:
 *
 *	The routine will start the 82586 processing the beginning of the
 *	transmit command chain.
 */
STATIC void
mioe_xmit(sp)
	mioe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;

	ELOG_ENTRY(19, ("mioe_xmit(sp=%x)\n", sp));

	/*
	 *  If the 82586 is currently transmitting a request (indicated
	 *  by a positive timer value) just return and let the completion
	 *  interrupt pickup queued requests.  Otherwise, if there is
	 *  something in the transmit queue, start it.
	 */
	if (sp->timer >= 0) {
		ELOG(ELOG_MEDIUM, ("mioe_xmit: tx busy\n"));
		STATS(sp, tx.busy++);
		return;
	}

	/*
	 *  Check for non-empty queue.
	 */
	if (sp->tx_qhead == 0) {
		ELOG(ELOG_LOW, ("mioe_xmit: queue empty\n"));
		return;
	}

	if (mioe_wait(sp, 100)) {
		sp->timer = 0;		/* cause chip reset */
		return;
	}

	sp->timer = MIOE_TIMER_RETRIES;
	scb_p->scb_cbl_offset = VIRT_TO_MIOE(sp, sp->tx_qhead->cmd_p);
	scb_p->scb_command    = SCB_CU_STRT;
	CHANN_ATTN(sp);
}

/*
 * mioe_watch():
 *
 *	This routine is the watchdog timer routine for the 82586 chip.
 *	It has been know to wedge under curtain conditions.  The algorithm
 *	used here is to use a slow poll when things are going right and then
 *	drop into a faster poll when a problem is detected.
 */
/* ARGSUSED */
STATIC void
mioe_watch(sp)
	mioe_softc_t	*sp;
{
	int	oldpri = SPLNET();

	ELOG_ENTRY(20, ("mioe_watch(arg=%x)\n", sp));

	/*
	 *  If all commands have been accounted for (timer = -1) or the
	 *  interface is not up, continue slow poll.
	 */
	if ((sp->timer < 0) || ((sp->ds_if.if_flags & IFF_UP) == 0)) {

		timeout(mioe_watch, sp, MIOE_SLOW_POLL);

	} else if (sp->timer == 0) {

		/*
		 *  There is a command outstanding for which we haven't
		 *  received an interrupt.  If retries are exhausted,
		 *  try to reset the 82586.  This will cause any pending
		 *  transmit requests in our 82586 queue to be dropped.
		 *
		 *  Mioe_init() has restarted the timer and checked for
		 *  a queued I/O request if it returned success.
		 */
		if (mioe_init(sp) == 0) {

			/*
			 *  Something is really wrong, mark the interface down.
			 */

			sp->ds_if.if_flags &= ~IFF_UP;

printf("MIO Ethernet %d:%d timed out and won't reset - interface marked down\n",
						sp->ds_if.if_unit, sp->node_id);
		}
	} else {

		/*
		 *  Retries left, decrement and go to a fast poll timer.
		 */
		ELOG(ELOG_HIGH, ("mioe_watch: Command timeout\n"));
		STATS(sp, timeouts++);

		sp->timer--;
		timeout(mioe_watch, sp, MIOE_FAST_POLL);
	}

	splx(oldpri);
	return;
}

/*
 * mioe_intr:
 *
 *	This function is the interrupt handler for the MIO Ethernet
 *	interface.  This routine will be called whenever a transmit
 *	command chain is complete or a packet is received.
 */
/* ARGSUSED */
void
mioe_intr(unit)
	int unit;
{
	mioe_softc_t	*sp   = &mioe_softc;
	v_scb_t		scb_p = sp->scb_p;
	u_short		status;

	ELOG_ENTRY(21, ("mioe_intr(unit=%x)\n", unit));

	/*
	 *  Sanity check
	 */
	if (sp->state != MIOE_OPEN) { 
		printf("MIO Ethernet %d:%d interrupt before initialization\n",
						sp->ds_if.if_unit, sp->node_id);
		return;
	}

	/*
	 *  Wait for the 82586 to indicate status if valid.  If it times out,
	 *  cause a chip reset by setting the timer to 0.
	 */
	if (mioe_wait(sp, 100)) {
		sp->timer = 0;
		return;
	}

	/*
	 *  Check for spurious interrupts.
	 */
	status = scb_p->scb_status;
	if ((status & SCB_SW_INT) == 0) {
		ELOG(ELOG_LOW,	/* XXX change when using real interrupts */
			("mioe_intr: spurious interrupt - status=%x\n",status));
		STATS(sp, spurious_int++);
		return;
	}

	/*
	 *  Acknowledge the current interrupt.
	 */
	scb_p->scb_command = (status & SCB_SW_INT);
	CHANN_ATTN(sp);

	/*
	 *  Process received frames.  If we went RNR then
	 *  mioe_rx_i() will restart the RU.
	 */
	if (status & (SCB_SW_FR | SCB_SW_RNR)) {
		mioe_rx_i(sp);
	}

	/*
	 *  Process transmitted packets.
	 */
	if (status & (SCB_SW_CX | SCB_SW_CNA)) {
		sp->timer = -1;		/* interrupt no longer pending */
		mioe_tx_i(sp);
	}

	/*
	 *  Accumulate errors kept by the chip.
	 */
	STATS(sp, rx.crcerrs += scb_p->scb_crcerrs);
	STATS(sp, rx.alnerrs += scb_p->scb_alnerrs);
	STATS(sp, rx.rscerrs += scb_p->scb_rscerrs);

	scb_p->scb_crcerrs = scb_p->scb_alnerrs = scb_p->scb_rscerrs = 0;

	return;
}

/*
 * mioe_rx_i:
 *
 *	Called be the MIOE interrupt entry point mioe_intr().  Process all
 *	received frames.  If we went RNR (ran out of receive buffers), start
 *	the RU.  The routine assumes that buffer chaining has not been done
 *	due to errata on the 82586.  This simplifies/speeds up receive
 *	frame processing.  
 */
#define	ONLY_ONE(p)	(p->rbd.status & RBD_SW_EOF)

STATIC
mioe_rx_i(sp)
	mioe_softc_t	*sp;
{
	v_fd_t		fd_p;
	v_rx_buf_t	rbd_p;

	ELOG_ENTRY(22, ("mioe_rx_i()\n"));
	STATS(sp, rx.interrupts++);

	/*
	 *  While there are completed frames to process, do it.
	 */
	for (fd_p = sp->begin_fd;
				fd_p && (fd_p->status & AC_SW_C);
							fd_p = sp->begin_fd) {

		/*
		 *  Adjust for next FD to be used for receive and find the RBD.
		 */
		sp->begin_fd = (v_fd_t)MIOE_TO_VIRT(sp, fd_p->link_offset);
		rbd_p = (v_rx_buf_t)MIOE_TO_VIRT(sp, fd_p->rbd_offset);

		/*
		 *  Make sure frame consumed at least one buffer.
		 */
		if (rbd_p && (fd_p->status & AC_SW_OK) && ONLY_ONE(rbd_p)) {

			/*
			 *  Bring the frame into GP memory.
			 */
			mioe_read(sp, fd_p, rbd_p);
		/*
		 *  Record the error.
		 */
		} else if ((fd_p->status & AC_SW_OK) == 0) {
			sp->ds_if.if_ierrors++;

		} else if (rbd_p == 0) {
			ELOG(ELOG_MEDIUM, ("moie_rx_i: NULL RBD on FD\n"));
			STATS(sp, rx.null_rbd++);

		} else {
			ELOG(ELOG_MEDIUM, ("moie_rx_i: long frame\n"));
			STATS(sp, rx.longs++);
		}

		/*
		 *  Requeue FD and RB's
		 */
		mioe_requeue(sp, fd_p, rbd_p);
	}

	/*
	 *  If the receive unit is not ready then restart it.
	 */
	if ((sp->map->scb.scb_status & SCB_RUS_READY) == 0)
		mioe_start_ru(sp);

	return;
}

/*
 * mioe_read:
 *
 *	This routine does the actual copy of data (including Ethernet header
 *	structure) from MIO memory to an ipc_kmsg_t.  Errata on the 82586
 *	indicates it can botch receive buffer switching when the last buffer
 *	only contains two or three bytes.  The work-around is to not use
 *	buffer chaining.  Because of this, we do not do buffer walking on
 *	received buffers but assume the complete frame is contained in a
 *	single buffer.  (This is also much quicker when you have the memory
 *	to spare!)
 */
STATIC void
mioe_read(sp, fd_p, rbd_p)
	mioe_softc_t	*sp;
	v_fd_t		fd_p;
	v_rx_buf_t	rbd_p;
{
	struct ifnet		*ifp = &sp->ds_if;
	struct ether_header	*ehp;
	struct packet_header	*pkt;
	ipc_kmsg_t		new_kmsg;
	u_short			frame_len;

	ELOG_ENTRY(23, ("mioe_read(rbd_p=%x)\n", rbd_p));

	/*
	 *  If the interface has been marked down, don't bother reading
	 *  the frame into kernel memory.
	 */
	if ((ifp->if_flags & IFF_UP) == 0) 
		return;

	/*
	 *  Get kernel memory to receive incoming packet.
	 */
	if ((new_kmsg = net_kmsg_get()) == IKM_NULL) {
		ELOG(ELOG_HIGH, ("mioe_read: IKM_NULL\n"));
		STATS(sp, rx.no_buffs++);
		ifp->if_rcvdrops++;
		return;
	}

	/*
 	 *  Set pointers
	 */
	ehp = (struct ether_header  *)(&net_kmsg(new_kmsg)->header[0]);
	pkt = (struct packet_header *)(&net_kmsg(new_kmsg)->packet[0]);

	/*
	 *  Copy Ethernet header.
	 *
	 *  Since these three fields are contiguous in both structures,
	 *  would it be cheesy to just do one miocopy() for all fields
	 *  at once?
	 *
	 *  miocopy(fd_p->destination, ehp->ether_dhost, (2*ETHER_ADD_SIZE)+2);
	 */
	miocopy(fd_p->destination, ehp->ether_dhost, ETHER_ADD_SIZE);
	miocopy(fd_p->source,      ehp->ether_shost, ETHER_ADD_SIZE);
	ehp->ether_type = fd_p->length;

	/*
	 *  Copy frame.
	 */
	frame_len = (rbd_p->rbd.status & RBD_SW_COUNT);
	miocopy(rbd_p->buf, (char *)(pkt + 1), frame_len);

	/*
	 *  Build packet header.
	 */
	pkt->type   = fd_p->length;
	pkt->length = frame_len + sizeof(struct packet_header);

	ELOG(ELOG_LOW, ("mioe_read: %d bytes from %x:%x:%x:%x:%x:%x\n",
		frame_len, fd_p->source[0], fd_p->source[1], fd_p->source[2],
			   fd_p->source[3], fd_p->source[4], fd_p->source[5]));
	/*
	 * Send the packet to the network module.
	 */
	ifp->if_ipackets++;
	STATS(sp, rx.packets++);
	STATS(sp, rx.bytes += frame_len);

	net_packet(ifp, new_kmsg, pkt->length, ethernet_priority(new_kmsg));
	return;
}

/*
 * mioe_requeue:
 *
 *	This routine puts spent FD's and RBD's back onto the free receive
 *	frame area (RFA).
 */
#define	EOP(p) ((p->rbd.status & RBD_SW_EOF) || (p->rbd.size & RFD_EL))

STATIC void
mioe_requeue(sp, fd_p, rbd_p)
	mioe_softc_t	*sp;
	v_fd_t		fd_p;
	v_rx_buf_t	rbd_p;
{
	v_rx_buf_t	last;

	ELOG_ENTRY(24, ("mioe_requeue(sp=%x, fd_p=%x, rbd_p=%x)\n",
							sp, fd_p, rbd_p));

	/*
	 *  Indicate the FD is now the anchor for the end of the list and
	 *  reinitialize its fields.  Link it into the free FD list.
	 */
	fd_p->command	  = AC_CW_EL;
	fd_p->status	  = 0;
	fd_p->link_offset = 0xffff;
	fd_p->rbd_offset  = 0xffff;

	sp->end_fd->link_offset = VIRT_TO_MIOE(sp, fd_p);
	sp->end_fd->command	= 0;
	sp->end_fd		= fd_p;

	/*
	 *  Free all RB's associated with this FD.
	 */
	if (rbd_p) {
		last = rbd_p;
		while(!EOP(last)) {
			last->rbd.status = 0;
			last = (v_rx_buf_t)
				MIOE_TO_VIRT(sp, last->rbd.next_rbd_offset);
		}
		
		/*
		 *  Indicate the RBD is now the anchor for the end of the list
		 *  and link it into the free RBD list.
		 */
		last->rbd.next_rbd_offset = 0xffff;
		last->rbd.status	  = 0;
		last->rbd.size		 |= AC_CW_EL;

		sp->end_rbd->rbd.next_rbd_offset = VIRT_TO_MIOE(sp, rbd_p);
		sp->end_rbd->rbd.size		&= ~AC_CW_EL;
		sp->end_rbd			 = last;
	}
}

/*
 * mioe_tx_i:
 *
 *	This routine will process completed transmit requests.  It will
 *	adjust the transmit queue pointer and start another transmit chain
 *	if there are requests in the queue.
 */
STATIC void
mioe_tx_i(sp)
	mioe_softc_t	*sp;
{
	xmit_q_t	*q;
	u_short		status;

	ELOG_ENTRY(25, ("mioe_tx_i(sp=%x)\n", sp));
	STATS(sp, tx.interrupts++);

	/*
	 *  Get the queue head and make sure it is non-zero.
	 */
	if ((q = sp->tx_qhead) == 0) {
		ELOG(ELOG_HIGH, ("mioe_tx_i: NULL queue head ptr\n"));
		STATS(sp, tx.q_null++);
		return;
	}

	/*
	 *  Walk the transmit queue until we hit the tail pointer or we
	 *  find a command that has not been completed.
	 */
	while (q) {
		status = q->cmd_p->status;
		if ((status & AC_SW_C) == 0)
			break;

		ELOG(ELOG_LOW, ("mioe_tx_i: %x complete with status %x\n",
						q->cmd_p, q->cmd_p->status));
		q->cmd_p->command = AC_CW_EL;

		/*
		 *  Check and keep stats on errors.
		 */
		if ((status & AC_SW_OK) == 0) {
			ELOG(ELOG_HIGH,
				("mioe_tx_i: tx error - status=%x\n", status));
			STATS(sp, tx.errors++);
			sp->ds_if.if_oerrors++;

			if (status & TC_COLLISION) {
				STATS(sp, tx.fatal_collis++);
				sp->ds_if.if_collisions += 16;
			}
			if (status & TC_DMA)
				STATS(sp, tx.underruns++);

			if (status & TC_CARRIER)
				printf(
		"MIO Ethernet %d:%d transmission link error - check cable\n",
						sp->ds_if.if_unit, sp->node_id);
		}

		if (status & TC_DEFER)
			STATS(sp, tx.defer++);
		
		sp->ds_if.if_collisions += (status & 0x0f);

		/*
		 *  Bump pointer to next command block checking for end of
		 *  queue.
		 */
		if (q == sp->tx_qtail)
			q = 0;
		else
			q = q->next;
	}
	sp->tx_qhead = q;

	/*
	 *  Pick up any queued requests that may have been blocked.
	 */
	mioe_start(sp->ds_if.if_unit);
}

/*
 * mioe_set_promisc:
 *
 *	Set or reset 82586 promiscuous mode.
 */
STATIC
mioe_set_promisc(sp, flag)
	mioe_softc_t	*sp;
	int		flag;
{
	ELOG_ENTRY(26, ("mioe_set_promisc(sp=%x, flag=%d)\n", sp, flag));

	if (flag) {
		sp->mode	   |= MIOE_PROMISC;
		sp->ds_if.if_flags |= IFF_PROMISC;
	} else {
		sp->mode	   &=~MIOE_PROMISC;
		sp->ds_if.if_flags &= ~IFF_PROMISC;
	}
	return (mioe_config(sp));
}

/*
 * mioe_rd_eaddr:
 *
 *	Get and validate Ethernet address.
 */
STATIC void
mioe_rd_eaddr(sp, addr_p)
	mioe_softc_t	*sp;
	u_char		*addr_p;
{
	ELOG_ENTRY(27, ("mioe_rd_eaddr(sp=%x, addr_p=%x)\n", sp, addr_p));

	/*
	 *  Get system defined Ethernet address.
	 */
	get_ether_addr(sp->ds_if.if_unit, addr_p);

	/*
	 *  Validate it (as best we can).
	 */
	if ((addr_p[0] != 0) || (addr_p[1] != 0xaa) || (addr_p[2] != 0))
		printf ("Warning: Ethernet address appears invalid\n");
}

/*
 * mioe_wait:
 *
 *	Wait for the SCB to be ready.
 */
STATIC
mioe_wait(sp, time)
mioe_softc_t	*sp;
int		time;
{
	ELOG_ENTRY(28, ("mioe_wait(sp=%x, time=%d)\n", sp, time));
	STATS(sp, wait_calls++);

	while(time--) {
		if (sp->scb_p->scb_command == 0)
			return (0);

		delay(10);
		STATS(sp, wait_spins++);
	}
	
	printf("MIO Ethernet %d:%d wait timeout\n",
					sp->ds_if.if_unit, sp->node_id);
	STATS(sp, wait_timeouts++);
	return (1);
}

/*
 *  mioe_print_stats:
 *
 *	Debugger print routine
 */
mioe_print_stats()
{
#ifdef MIOE_STATS
	mioe_stats_t	*s = &mioe_softc.stats;

dp_printf("tx stats:\n");
dp_printf("packets   = % -8d bytes  = % -8d inter  = % -8d defer     = % -8d\n",
	   s->tx.packets, s->tx.bytes, s->tx.interrupts, s->tx.defer);

dp_printf("fatal_col = % -8d busy   = % -8d errors = % -8d underruns = % -8d\n",
	   s->tx.fatal_collis, s->tx.busy, s->tx.errors, s->tx.underruns);

dp_printf("q_full    = % -8d q_null = % -8d q_hit  = % -8d q_miss    = % -8d\n",
	   s->tx.q_full, s->tx.q_null, s->tx.q_hit, s->tx.q_miss);

dp_printf("\nrx stats:\n");
dp_printf("packets = % -8d bytes    = % -8d inter   = % -8d restarts  = % -8d\n",
	   s->rx.packets, s->rx.bytes, s->rx.interrupts, s->rx.restarts);

dp_printf("crcerrs = % -8d alnerrs  = % -8d rscerrs = % -8d overnerrs = % -8d\n",
	   s->rx.crcerrs, s->rx.alnerrs, s->rx.rscerrs, s->rx.ovrnerrs);

dp_printf("q_empty = % -8d no_buffs = % -8d longs   = % -8d null_rbd  = % -8d\n",
	   s->rx.q_empty, s->rx.no_buffs, s->rx.longs, s->rx.null_rbd);

dp_printf("\nother stats:\n");
dp_printf("timeouts = % -8d   spurious_int = % -8d   resets = % -8d\n",
	   s->timeouts, s->spurious_int, s->resets);
	
dp_printf("wait(calls = %d spins = %d timeouts = %d)\n",
	   s->wait_calls, s->wait_spins, s->wait_timeouts);

dp_printf("\nif_stats:\n");
dp_printf("ipackets = % -8d   ierrors    = % -8d   opackets = % -8d\n",
	   mioe_softc.ds_if.if_ipackets, mioe_softc.ds_if.if_ierrors,
	   mioe_softc.ds_if.if_opackets);

dp_printf("oerrors  = % -8d   collisions = % -8d   rcvdrops = % -8d\n",
	  mioe_softc.ds_if.if_oerrors, mioe_softc.ds_if.if_collisions,
	  mioe_softc.ds_if.if_rcvdrops);
#else
	dp_printf("MIOE_STATS not defined\n");
#endif
}
#endif	NMIOE > 0
