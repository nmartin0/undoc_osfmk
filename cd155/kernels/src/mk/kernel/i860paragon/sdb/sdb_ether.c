/*
 *
 * $Copyright
 * Copyright 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Copyright 1994 by Intel Corporation,
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
 * $Log: sdb_ether.c,v $
 * Revision 1.3.8.1  1995/06/11  18:33:35  kat
 * Updated copyright for R1.3 PSCP
 *
 * Revision 1.3  1995/03/28  01:18:40  jerrie
 * Removed function parameter prototypes to fix problems with native builds.
 *
 *  Reviewer:	   Vineet Kumar
 *  Risk:		   Low.
 *  Benefit or PTS #: 12827
 *  Testing:	   Developer
 *  Module(s):	   See PTS report.
 *
 * Revision 1.2  1995/03/14  23:43:47  jerrie
 *  Reviewer:         Jerrie Coffman, Vineet Kumar, Richard Griffiths
 *  Risk:             High
 *  Benefit or PTS #: Add SCSI-16 daughter board support.
 *  Testing:          Ran PFS, RAID, fileio, and tape EATs.
 *  Module(s):        Too numerous to mention.  See Jerrie for details.
 *
 */
/*
 *	File:	sdb_ether.c
 *	Author:	Jim Chorn (modified by Jerrie Coffman)
 *		Intel Corporation Supercomputer Systems Division
 *	Date:	11/94
 *
 *	SCSI-16 daughter board ethernet driver
 *
 *	Based on the MIO board if_mioe.c driver written by Roy Larsen
 */


#include <sdbe.h>
#if	NSDBE > 0


#include <sys/types.h>
#include <vm/vm_kern.h>
#include <chips/busses.h>

#include <device/errno.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>
#include <device/net_io.h>

#include <i860paragon/expansion.h>

#include <i860paragon/sdb/sdb.h>
#include <i860paragon/sdb/sdb_dev.h>
#include <i860paragon/sdb/i82596.h>
#include <i860paragon/sdb/if_sdbe.h>

extern void		outb();
extern void		outw();
extern void		outl();
extern unsigned char	inb();
extern unsigned short	inw();
extern unsigned long	inl();


/*
 * Function Definitions
 */
STATIC	int	sdbe_probe();
STATIC	int	sdbe_attach();
	int	sdbe_open();
	void	sdbe_close();
STATIC	int	sdbe_init();
STATIC	int	sdbe_hwrst();
STATIC	void	sdbe_bld_scbs();
STATIC	void	sdbe_bld_rfa();
STATIC	void	sdbe_bld_txq();
STATIC	int	sdbe_diag();
STATIC	int	sdbe_config();
STATIC	int	sdbe_set_eaddr();
STATIC	int	sdbe_q_sanity();
STATIC	void	sdbe_start_ru();
STATIC	int	sdbe_gen_cmd();
	int	sdbe_setinput();
	int	sdbe_getstat();
	int	sdbe_setstat();
	int	sdbe_output();
STATIC	void	sdbe_start();
STATIC	void	sdbe_q_tx();
STATIC	void	sdbe_xmit();
	void	sdbe_intr();
STATIC	int	sdbe_rx_i();
STATIC	int	sdbe_dumpit();
STATIC	u_short	sdbe_get_framelen();
STATIC	void	sdbe_copy_frame();
STATIC	void	sdbe_read();
STATIC	void	sdbe_freerbds();
STATIC	void	sdbe_requeue();
STATIC	void	sdbe_tx_i();
STATIC	int	sdbe_set_promisc();
STATIC	int	sdbe_rd_eaddr();
STATIC	int	sdbe_wait();
	int	sdbe_attn();
	int	sdbe_print_stats();


STATIC	caddr_t			sdbe_std[NSDBE]	   = { 0 };
STATIC	struct bus_device	*sdbe_dinfo[NSDBE] = { 0 };

/*
 * Definition of the driver for the auto-configuration program
 */
struct bus_driver		sdbe_driver = {
/*
 *	  structure		   structure    field
 *	initialization		    field    description
 */
	sdbe_probe,		/* probe  - is the i82596 installed ?	 */
	0,			/* slave  - really a probe for slaves... */
	sdbe_attach,		/* attach - setup driver after the probe */
	0,			/* go	  - start transfer		 */
	sdbe_std,		/* addr	  - device csr addresses	 */
	"sdbe",			/* dname  - device driver name		 */
	sdbe_dinfo,		/* dinfo  - bus_device backpointers	 */
	0,			/* mname  - name of controller		 */
	0,			/* minfo  - bus_ctlr backpointers	 */
	0			/* flags  - flags			 */
};

static char	sdbe_signon[] = "\
SDB Ethernet Driver Revision: V1.0\n\
Copyright (c) 1994 Intel Corporation, All Rights Reserved\n";

sdbe_softc_t	sdbe_softc;

#ifdef	TX_DUAL_RX_NODE
/*
 * RX buffers in kernel (node) memory
 */
static u_char	sdbe_rx_data[RBD_QUEUE_SIZE * SDBE_BUF_SIZE];
#endif	TX_DUAL_RX_NODE

#ifdef STAT_PROF
sdbe_stats_t	*prof_sp = &sdbe_softc.stats;
#endif


#ifdef	DEBUG
/*
 * if bit x of sdbe_log_level = 1 trace this message level
 */
int	sdbe_log_level = 0		/* for non-debug set to 0	  */
		/*	| ELOG_ENTER	/* upon function entry and exit	  */
		/*	| ELOG_HIGH	/* branches within a function	  */
		/*	| ELOG_MEDIUM	/* something between low and high */
		/*	| ELOG_LOW	/* serious condition and problems */
			;

/*
 * if bit x of sdbe_log_id = 1 trace this routine
 */
int	sdbe_log_id =	0		/* for non-debug set to 0	  */
		/*	| BIT(0)	/* sdbe_probe()			  */
		/*	| BIT(1)	/* sdbe_attach()		  */
		/*	| BIT(2)	/* sdbe_open()			  */
		/*	| BIT(3)	/* sdbe_close()			  */
		/*	| BIT(4)	/* sdbe_init()			  */
		/*	| BIT(5)	/* sdbe_hwrst()			  */
		/*	| BIT(6)	/* sdbe_bld_scbs()		  */
		/*	| BIT(7)	/* sdbe_bld_rfa()		  */
		/*	| BIT(8)	/* sdbe_bld_txq()		  */
		/*	| BIT(9)	/* sdbe_diag()			  */
		/*	| BIT(10)	/* sdbe_config()		  */
		/*	| BIT(11)	/* sdbe_set_eaddr()		  */
		/*	| BIT(12)	/* sdbe_start_ru()		  */
		/*	| BIT(13)	/* sdbe_gen_cmd()		  */
		/*	| BIT(14)	/* sdbe_setinput()		  */
		/*	| BIT(15)	/* sdbe_getstat()		  */
		/*	| BIT(16)	/* sdbe_setstat()		  */
		/*	| BIT(17)	/* sdbe_output()		  */
		/*	| BIT(18)	/* sdbe_start()			  */
		/*	| BIT(19)	/* sdbe_q_tx()			  */
		/*	| BIT(20)	/* sdbe_xmit()			  */
		/*	| BIT(21)	/* sdbe_intr()			  */
		/*	| BIT(22)	/* sdbe_rx_i()			  */
		/*	| BIT(23)	/* sdbe_read()			  */
		/*	| BIT(24)	/* sdbe_requeue()		  */
		/*	| BIT(25)	/* sdbe_tx_i()			  */
		/*	| BIT(26)	/* sdbe_set_promisc()		  */
		/*	| BIT(27)	/* sdbe_rd_eaddr()		  */
		/*	| BIT(28)	/* sdbe_wait()			  */
			;
#endif	DEBUG


/*
 * sdbe_probe:
 *
 *	If called, assume we have an SDB card attached with a Ethernet
 *	interface.
 */
/* ARGSUSED */
STATIC
sdbe_probe(vaddr, ctlr)
	caddr_t		vaddr;
	struct bus_ctlr	*ctlr;
{
	extern int	expansion_id();
	sdbe_softc_t	*sp = &sdbe_softc;

	ELOG_ENTRY(0, ("sdbe_probe(vaddr=0x%x, ctlr=0x%x)\n", vaddr, ctlr));

	/* check for a SCSI-16 expansion board type */
	if (expansion_id() != EXP_ID_SCSI)
		return 0;

	/*
	 * Say hello
	 */
	printf(sdbe_signon);

	/*
	 * Initialize the sdbe_softc structure.
	 */
	sdbzero(sp, sizeof(sdbe_softc_t));

#ifdef	TX_DUAL_RX_NODE
	/*
	 * TX buffers in Dual-port RAM
	 */
	sp->map		= (sdbe_map_t *)ETHER_START_DPRAM;
#else	TX_DUAL_RX_NODE
	/*
	 * TX buffers in Local RAM
	 */
	sp->map		= (sdbe_map_t *)ETHER_START_SRAM;
#endif	TX_DUAL_RX_NODE
	sp->map_phys	= (sdbe_map_t *)VIRT_TO_SDBE(sp->map);
	sp->scb_p	= &sp->map->scb;
	sp->node_id	= NODE_ID;
	sp->state	= SDBE_PROBED;

#ifdef	ALL_IN_LRAM
	/*
	 * RX buffers in Local RAM
	 */
	sp->rx_data     = sp->map->rx_data;	/* Data buffer start address */
#else	ALL_IN_LRAM
	/*
	 * RX buffers in kernel (node) memory
	 */
	sp->rx_data	= sdbe_rx_data;		/* Data buffer start address */
#endif	ALL_IN_LRAM
	sp->rx_data_phys = (ram_t *)VIRT_TO_SDBE(sp->rx_data);

	ELOG(ELOG_MEDIUM,
		("sdbe_probe: SDBE Dev MAP 0x%x-0x%x, RX Buffers 0x%x-0x%x\n",
			sp->map, sp->map + 1, sp->rx_data,
			sp->rx_data + RBD_QUEUE_SIZE * SDBE_BUF_SIZE));

	/*
	 * Sanity check
	 */
	if (!sp->map || !sp->rx_data) {
		printf(
	    "sdbe_probe: buffer not allocated, map = 0x%x rx_data = 0x%x\n",
			sp->map, sp->rx_data);
		return 0;
	}

	return 1;
}


/*
 * sdbe_attach:
 *
 *	Attach the SDB Ethernet interface. Read the Serial Rom copy of the
 *	Ethernet address and initialize the ifnet structure.
 */
/* ARGSUSED */
STATIC
sdbe_attach(device)
	struct bus_device	*device;
{
	sdbe_softc_t	*sp = &sdbe_softc;
	struct	ifnet	*ifp;

	ELOG_ENTRY(1, ("sdbe_attach(device=0x%x)\n", device));

	/*
	 * Make sure we're being called in the proper sequence.
	 */
	if (sp->state != SDBE_PROBED) {
		printf("SDB Ethernet %d:%d attach called out of sequence",
				device->unit, sp->node_id);
		printf(" - state = %d\n", sp->state);
		sp->state = SDBE_SOFTWARE_INIT_ERROR;
		return 0;
	} else
		sp->state = SDBE_ATTACHED;

	/*
	 * Read and display the Ethernet address found in srom.
	 */
	if ( sdbe_rd_eaddr(sp, sp->ds_addr) ) {
		sp->state = SDBE_SOFTWARE_INIT_ERROR;
		return 0;
	}

	printf(" ethernet address - %02x:%02x:%02x:%02x:%02x:%02x",
			sp->ds_addr[0], sp->ds_addr[1], sp->ds_addr[2],
			sp->ds_addr[3], sp->ds_addr[4], sp->ds_addr[5]);

	ifp = &(sp->ds_if);
	ifp->if_unit	      = device->unit;
	ifp->if_mtu	      = ETHERMTU;
	ifp->if_flags	      = IFF_BROADCAST;
	ifp->if_header_size   = sizeof(struct ether_header);
	ifp->if_header_format = HDR_ETHERNET;
	ifp->if_address_size  = ADDR_LEN;
	ifp->if_address	      = (char *)&sp->ds_addr[0];

	/*
	 * Initialize queues
	 */
	if_init_queues(ifp);

	return 0;
}


/*
 * sdbe_open:
 *
 *	Open the SDB Ethernet driver. The driver must have been probed
 *	and attached before being opened.
 */
/*ARGSUSED*/
sdbe_open(dev, flag)
	dev_t	dev;
	int	flag;
{
	sdbe_softc_t	*sp = &sdbe_softc;

	ELOG_ENTRY(2, ("sdbe_open(dev=0x%x, flag=0x%x)\n", dev, flag));

	/*
	 * Since this driver only supports one interface,
	 * make sure that unit 0 is being opened.
	 */
	if (dev != 0)
		return ENXIO;

	if (sp->state == SDBE_OPEN)
		return 0;

	/*
	 * Make sure we are called in the proper sequence then initialize
	 * the chip.  We allow the hardware init error state so we can retry.
	 */
	if (((sp->state != SDBE_ATTACHED) &&
	     (sp->state != SDBE_HARDWARE_INIT_ERROR)) ||
	    (sdbe_init(sp) == 0)) {
		return ENXIO;
	}

	/*
	 * Allow the SDB to generate Ethernet interrupts and finish
	 * setting flags.
	 */
	sp->state = SDBE_OPEN;
	sp->mode  = 0;
	sp->ds_if.if_flags |= IFF_UP;
	LAN_INTR_ENABLE(LANMSK);	/* Ethernet interrupt */
	PAR_INTR_ENABLE(LANPAR);	/* Ethernet parity error interrupt */

	return 0;
}


/*
 * sdbe_close:
 *
 *	Close the SDB Ethernet driver. The driver must have been opened
 *	before being closed.
 *
 *	NOTE: This procedure doesn't handle closes in a 'last-close' manner.
 */
/*ARGSUSED*/
void
sdbe_close(dev)
	dev_t	dev;
{
	sdbe_softc_t	*sp = &sdbe_softc;

	ELOG_ENTRY(3, ("sdbe_close(dev=0x%x)\n", dev));

	if (sp->state == SDBE_OPEN)
		sp->state = SDBE_ATTACHED; /* Back off to the attached state */
	else
		sp->state = SDBE_PROBED;   /* Error occurred to cause reinit */

	LAN_INTR_ENABLE(0);		   /* Disable Ethernet interrupt */
	PAR_INTR_ENABLE(0);		   /* Disable Ethernet parity	 */

        SET_CONTROL_REG(LANRST);	   /* Apply chip Reset */
}


/*
 * sdbe_init:
 *
 *	Reset and initialized the 82596. This routine is called from
 *	sdbe_open().
 */	
STATIC
sdbe_init(sp)
	sdbe_softc_t	*sp;
{
	int	status;
	int	oldpri = SPLNET();

	ELOG_ENTRY(4, ("sdbe_init(sp=0x%x)\n", sp));

	/*
	 * Do a hardware reset and configuration.
	 */
	if (status = sdbe_hwrst(sp)) {
		sp->custate = SDBE_READY;
		sp->ds_if.if_flags |= IFF_RUNNING;

		/*
		 * Now that we're initialized, start any transmit requests
		 * that may be queued.
		 */
		sdbe_start(sp->ds_if.if_unit);
	} else
		sp->state = SDBE_HARDWARE_INIT_ERROR;
	
	splx(oldpri);
	return status;
}


/*
 * sdbe_hwrst:
 *
 *	This routine resets the 82596. It is called by sdbe_init() and
 *	assumes that we are at SPLNET.
 */
STATIC
sdbe_hwrst(sp)
	sdbe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;

	ELOG_ENTRY(5, ("sdbe_hwrst(sp=%x)\n", sp));
	STATS(sp, resets++);

        CLEAR_CONTROL_REG(LANRST);	/* Remove i82596 chip Reset */
	delay(DELAY_10MS);		/* Give it time to get sane */
	CHIP_RESET(sp);
	SET_LOOPBACK(sp, LOOPBACK_ON);
	
	/*
	 * Build all 82596 data structures then tell the 82596 we're ready.
	 */
	sdbzero(sp->map, SDBE_ETHER_RAM_SIZE);
	sdbe_bld_scbs(sp);
	sdbe_bld_rfa(sp);
	sdbe_bld_txq(sp);
	CHANN_ATTN(sp);
	delay(DELAY_10MS);		/* Wait for command completion */

	/*
	 * Check to ensure the chip responds properly.
	 */
	if (scb_p->scb_status != (SCB_SW_CX | SCB_SW_CNA)) {
	    printf("SDB Ethernet %d:%d 82596 will not reset! Status = 0x%x\n",
		sp->ds_if.if_unit, sp->node_id, scb_p->scb_status);
	    return 0;
	}

	sdbe_attn(sp, SCB_ACK_CX|SCB_ACK_CNA);

	/*
	 * Diagnose and configure the 82596
	 */
	if ((sdbe_diag(sp) == 0) || (sdbe_config(sp) == 0))
		return 0;

	/*
	 * Set the Ethernet address, start the Receive Unit, allow 82596
	 * interrupts and take us out of loopback mode.
	 */
	if (sdbe_set_eaddr(sp) == 0)
		return 0;

	SET_LOOPBACK(sp, LOOPBACK_OFF);
	sdbe_config(sp);	/* reconfigure loopback off */
	sdbe_start_ru(sp);

	return 1;
}


/*
 * sdbe_bld_scbs:
 *
 *	This routine builds the scb structures (scp, iscp, and scb).
 */
#define SCP_596_MODE	(SCP_ALWAYS_ONE|SCP_LINEAR_MODE|SCP_LOW_INT)
STATIC void
sdbe_bld_scbs(sp)
	sdbe_softc_t	*sp;
{
	sdbe_map_t	*map   = sp->map;
	v_scp_t		scp_p  = &map->scp;
	v_iscp_t	iscp_p = &map->iscp;
	v_scb_t		scb_p  = sp->scb_p;

	ELOG_ENTRY(6, ("sdbe_bld_scbs(sp=0x%x)\n", sp));

	sdbzero((char *)scp_p, sizeof(scp_t));
	scp_p->scp_sysbus	= SCP_596_MODE;	/* 32 bit linear bus mode */
	scp_p->scp_iscp		= (iscp_t *)VIRT_TO_SDBE(iscp_p);

	sdbzero((char *)iscp_p, sizeof(iscp_t));
	iscp_p->iscp_busy	= 1;
	iscp_p->iscp_scb	= (scb_t *)VIRT_TO_SDBE(scb_p);

	scb_p->scb_status	= 0;
	scb_p->scb_command	= 0;
	scb_p->scb_cba		= 0;		/* this is not static */
	scb_p->scb_rfa		= (rfd_t *)VIRT_TO_SDBE(map->fd_q);
	scb_p->scb_crcerrs	= 0;
	scb_p->scb_alnerrs	= 0;
	scb_p->scb_rscerrs	= 0;
	scb_p->scb_ovrnerrs	= 0;
	scb_p->scb_rcvcdt	= 0;
	scb_p->scb_shrtfrm	= 0;
	scb_p->scb_offtimer	= 0;
	scb_p->scb_ontimer	= 0;
	LOAD_SCPADDR(VIRT_TO_SDBE(scp_p));
}


/*
 * sdbe_bld_rfa:
 *
 *	This routine will build the Receive Frame Area (RFA) queue
 *	and the Receive Buffer Descriptor (RBD) list.
 *
 *	Buffer chaining is used.
 *
 *	The RFD chain is built up with the first RFD Recieve Buffer Descriptor
 *	pointer pointing at the RBD chain. All other rfd_rbda's are initialized
 *	to 0xffffffff so that the i82596 can write the rbd it uses when the rfd
 *	becomes the current frame.
 *
 *	All packet data including ethernet headers are placed into the
 *	RBD described data buffer.
 *
 *	See figure 39 in LAN Components Users Manual(296853-001).
 */
STATIC void
sdbe_bld_rfa(sp)
	sdbe_softc_t	*sp;
{
	v_fd_t		fd_p  = sp->map->fd_q;
	v_rbd_t		rbd_p = sp->map->rbd_q;
	ram_t		*buf  = sp->rx_data;
	int		i;

	ELOG_ENTRY(7, ("sdbe_bld_rfa(sp=0x%x) fd 0x%x pkt 0x%x\n",
		sp, fd_p, buf));

	/*
	 * Build rfd queue and ensure rfd_size is zero so that
	 * the entire packet goes in the buffer.
	 */
	sp->fd_unk = 0;
	sp->fd_q = fd_p;
	for (i = 0; i < RFD_QUEUE_SIZE; i++, fd_p++) {

		sdbzero((char *)fd_p, sizeof(rfd_t));
		fd_p->rfd_cmd	= AC_CW_SF;
		fd_p->rfd_next	= (rfd_t *)VIRT_TO_SDBE(fd_p + 1);
		fd_p->rfd_rbda	= (v_rbd_t)NIL_ADDR;
	}

	/*
	 * Handle last (rfd) entry and initialize the first rfd_rbda
	 */
	fd_p--;					/* Back up to the last one */
	sp->fd_q_tail	= fd_p;			/* Track tail */
	fd_p->rfd_next	= (rfd_t *)VIRT_TO_SDBE(sp->fd_q); /* Circular list */
	fd_p->rfd_cmd	= AC_CW_EL|AC_CW_SF;	/* Mark as last rfd on list */
	/* Point to the first rbd */
	sp->fd_q->rfd_rbda = (rbd_t *)VIRT_TO_SDBE(rbd_p);

	/*
	 * Build rbd queue
	 */
	sp->rx_unk = 0;
	sp->rx_rbds = rbd_p;
	for (i = 0; i < RBD_QUEUE_SIZE; i++, rbd_p++, buf += SDBE_BUF_SIZE) {
		sdbzero((char *)rbd_p, sizeof(rbd_t));
		rbd_p->rbd_next = (rbd_t *)VIRT_TO_SDBE(rbd_p + 1);
		rbd_p->rbd_size = SDBE_BUF_SIZE;
		rbd_p->rbd_buf = (u_char *)VIRT_TO_SDBE(buf);
	}

	/*
	 * Handle last (rbd) entry
	 */
	rbd_p--;
	rbd_p->rbd_next	= (rbd_t *)VIRT_TO_SDBE(sp->rx_rbds);/* Circular list */
	rbd_p->rbd_size	|= RBD_EL;			/* Terminate the list */
	sp->rx_rbdtail = rbd_p;
#ifdef Q_DEBUG
	printf("FD 0x%x %d RBD 0x%x %d buf sz %d\n", sp->fd_q, RFD_QUEUE_SIZE,
			sp->rx_rbds, RBD_QUEUE_SIZE, SDBE_BUF_SIZE);
	printf("Last buf 0x%x, local ram 0x%x to 0x%x\n",
			buf, sp->map, sp->map + 1);
#endif
}


/*
 * sdbe_bld_txq:
 *
 *	The routine will build and initialize the transmit queue.
 *	Below is hold-over MIO comments on how the queues were built.
 *	The SCSI-16 board allows 32 bit reads/writes to LRAM.
 *	This driver still copies the transmit packet into lram and
 *	lets the 82596 take it from there.
 *
 *	This allows for fast/easy queue manipulation
 *	since we don't need to keep converting between SDB memory offsets
 *	and kernel virtual address. The structure of the queue is illustrated
 *	below. Note that all fields marked with a "~" are initialized only
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
 *   | | |                   | | |    SDB memory         | | |
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
sdbe_bld_txq(sp)
	sdbe_softc_t	*sp;
{
	v_tx_q_t	cb_q = sp->map->tx_q;
	int		i;

	ELOG_ENTRY(8, ("sdbe_bld_txq(sp=0x%x)\n", sp));

	for (i = 0; i < TX_QUEUE_SIZE; i++, cb_q++) {

		/*
		 * Set SDB command block
		 */
		sdbzero((char*)cb_q, sizeof(ac_t) + sizeof(tbd_t));
		cb_q->cmd.ac_cmd	= AC_CW_EL|AC_CW_SF;
		cb_q->cmd.ac_next	= (ac_t *)VIRT_TO_SDBE(cb_q + 1);

		/*
		 * Set SDB Transmit Buffer Descriptor
		 */
		cb_q->tbd.tbd_size	= TBD_EOF;
		cb_q->tbd.tbd_next	= (tbd_t *)NIL_ADDR;
		cb_q->tbd.tbd_buf	= 
			(u_char *)VIRT_TO_SDBE(cb_q->buf); /* Snooping??? */
	}

	/*
	 * Make queue circular
	 */
	cb_q--;
	cb_q->cmd.ac_next = (ac_t *)VIRT_TO_SDBE(sp->map->tx_q);

	/*
	 * Set head and tail pointers.
	 */
	sp->tx_qhead = 0;
	sp->tx_qtail = sp->map->tx_q;
}


/*
 * sdbe_diag:
 *
 *	This routine asks the 82596 to perform its diagnostics. Sine we
 *	don't do loop back tests, this is not a complete diagnostic test.
 */
STATIC
sdbe_diag(sp)
	sdbe_softc_t	*sp;
{
	ELOG_ENTRY(9, ("sdbe_diag(sp=0x%x)\n", sp));

	/*
	 * Give diagnose command.
	 */
	if (sdbe_gen_cmd(sp, &sp->map->gen_cmd, AC_DIAGNOSE) == 0) {
		printf("SDB Ethernet %d:%d diagnostic failed\n",
						sp->ds_if.if_unit, sp->node_id);
		return 0;
	} else	
		return 1;
}


/*
 * sdbe_config:
 *
 *	This routine does a standard config of the 82596.
 */
#define	GET_PROMISC(sp)		((sp->mode & SDBE_PROMISC ? PRM:0)|CONF_4_WORD)
#define	GET_LOOPBACK(sp)	((sp->mode & SDBE_LOOPBACK) | CONF_1_WORD)

STATIC
sdbe_config(sp)
	sdbe_softc_t	*sp;
{
	v_ac_t	cb_p = &sp->map->gen_cmd;

	ELOG_ENTRY(10, ("sdbe_config(sp=0x%x)\n", sp));

	/*
	 * This is the default board configuration, monitor mode off.
	 */
	cb_p->ac_func.cfg.cf_words[0]	= CONF_0_WORD;
	cb_p->ac_func.cfg.cf_words[1]	= GET_LOOPBACK(sp);
	cb_p->ac_func.cfg.cf_words[2]	= CONF_2_WORD;
	cb_p->ac_func.cfg.cf_words[3]	= CONF_3_WORD;
	cb_p->ac_func.cfg.cf_words[4]	= GET_PROMISC(sp);
	cb_p->ac_func.cfg.cf_words[5]	= CONF_5_WORD|MIN_FRM_LEN;
	cb_p->ac_func.cfg.cf_words[6]	= CONF_6_WORD;

#ifdef DEBUG_PRM
	if (cb_p->ac_func.cfg.cf_words[4] & PRM)
		printf("Promisc mode Enabled\n");
	else
		printf("Promisc mode Disabled\n");
#endif
	if (sdbe_gen_cmd(sp, cb_p, AC_CONFIGURE) == 0) {
		printf("SDB Ethernet %d:%d configuration failed\n",
					sp->ds_if.if_unit, sp->node_id);
		return 0;
	} else
		return 1;
}


#define BURST_ETHER(p) \
	((u_char *)p)[0], ((u_char *)p)[1], ((u_char *)p)[2], \
	((u_char *)p)[3], ((u_char *)p)[4], ((u_char *)p)[5]
/*
 * sdbe_set_eaddr:
 *
 *	This routine will set the individual Ethernet address of the 82596
 *	according to the address found in sp->ds_addr.
 */
STATIC
sdbe_set_eaddr(sp)
	sdbe_softc_t	*sp;
{
	v_ac_t	cb_p = &sp->map->gen_cmd;

	ELOG_ENTRY(11, ("sdbe_set_eaddr(sp=0x%x)\n", sp));

	sdbcopy((caddr_t)sp->ds_addr, (caddr_t)cb_p->ac_func.iasetup, ADDR_LEN);

	if (sp->ds_addr[0] != 0	   ||
	    sp->ds_addr[1] != 0xAA ||
	    sp->ds_addr[2] != 0) {
		printf("SDB Ethernet Address Invalid %x:%x:%x:%x:%x:%x\n",
			BURST_ETHER(sp->ds_addr));
	}

	if (sdbe_gen_cmd(sp, cb_p, AC_IA_SETUP) == 0) {
		printf("SDB Ethernet %d:%d address initialization failed\n",
					sp->ds_if.if_unit, sp->node_id);
		return 0;
	} else
		return 1;
}


/*
 * sdbe_q_sanity:
 *
 *	This procedure ensures that the receive queue and rbd queue head
 *	and tail pointers make some kind of sense. The procedurewas included
 *	as part of dealing with the errata to ensure that the queues don't
 *	get into wierd linking states when the chip does something funny.
 *	By not using the CU/RU suspend/abort and ensuring that there is
 *	never a CU_START while the CU is busy and by avoiding restarting
 *	the RU as much as possible the likely-hood that this procedure will
 *	ever detect anything becomes real small.
 */
STATIC
sdbe_q_sanity(sp)
	sdbe_softc_t	*sp;
{
#ifdef DEBUG_QSANITY
	v_fd_t	fd_p  = sp->fd_unk;
	v_rbd_t	rbd_p = sp->rx_unk;

	/*
	 * Check for clean queues
	 */
	if ( !( fd_p || rbd_p ) )	/* No unknown states to check out */
		return;	
	/*
	 * Check the rfd queue
	 */
	if (fd_p) {
		printf("RFD Queue ");
		if ( ! ( fd_p->rfd_stat & AC_SW_C ) ) {
			printf("reset");
			sp->fd_q_tail = fd_p;		/* Reset tail */
			fd_p->rfd_cmd |= AC_CW_EL;
			fd_p = (v_fd_t)SDBE_TO_VIRT(fd_p->rfd_next);
			sp->fd_q = fd_p;		/* Reset head */
			fd_p->rfd_cmd &= ~AC_CW_EL;
			sp->fd_unk = 0;
		}
		printf("\n");
	} else {
		fd_p = sp->fd_q;
		if ( fd_p->rfd_stat & AC_SW_C )
			printf("Error: RFD Completed head\n");
	}

	/*
	 * Check rbd queue
	 */
	if (rbd_p) {
		printf("RBD Queue ");
		if ( !( rbd_p->rbd_act_count & RBD_STAT_F) ) {
			printf("reset");
			sp->rx_rbdtail = rbd_p;		/* Reset tail */
			rbd_p->rbd_size |= RBD_EL;
			rbd_p = (v_rbd_t)SDBE_TO_VIRT(rbd_p->rbd_next);
			sp->rx_rbds = rbd_p;		/* Reset head */
			rbd_p->rbd_size &= ~RBD_EL;
			sp->rx_unk = 0;
		}
		printf("\n");
	} else {
		rbd_p = sp->rx_rbds;
		if ( rbd_p->rbd_act_count & RBD_STAT_F )
			printf("Error: RBD Full head\n");
		if ( rbd_p->rbd_size & RBD_EL ) {
			printf("Error: RBD EL head\n");
			sp->rx_rbds = (v_rbd_t)rbd_p->rbd_next;
		}
	}
#endif
}


/*
 * sdbe_start_ru:
 *
 *	This routine starts the receive unit running. Except for when the
 *	82596 is initialized, this only needs to be done when the receive
 *	queue was exhausted. This should be a very rare event for an i860!
 *	The RFA will be reconstructed when this happens.
 */
STATIC void
sdbe_start_ru(sp)
	sdbe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;

	ELOG_ENTRY(12, ("sdbe_start_ru(sp=0x%x)\n", sp));

	STATS(sp, rx.restarts++);

	sdbe_q_sanity(sp);
	scb_p->scb_rfa = (rfd_t *)VIRT_TO_SDBE(sp->fd_q);
	sp->fd_q->rfd_rbda = (rbd_t *)VIRT_TO_SDBE(sp->rx_rbds);
	sdbe_attn(sp, SCB_RU_START);
}


/*
 * sdbe_gen_cmd(sp, cmd)
 *
 *	This routine will issue a general command then poll for command
 *	completion.
 */
STATIC
sdbe_gen_cmd(sp, cb_p, cmd)
	sdbe_softc_t	*sp;
	v_ac_t		cb_p;
	int		cmd;
{
	v_scb_t	scb_p = sp->scb_p;
	int		i;

	ELOG_ENTRY(13, ("sdbe_gen_cmd(sp=0x%x, cb_p=0x%x, cmd=0x%x)\n",
							sp, cb_p, cmd));

	/*
	 * Wait for 82596 to be ready to start a new command then issue
	 * the command passed.
	 */
	if (sdbe_wait(sp, 1000))
		return 0;

	cb_p->ac_stat	= 0;
	cb_p->ac_cmd	= ((u_short)cmd | AC_CW_EL);
	scb_p->scb_cba	= (ac_t *)VIRT_TO_SDBE(cb_p);

	sdbe_attn(sp, SCB_CU_START);

	/*
	 * Wait for command to complete.
	 */
	for (i = 1000; i; i--) {
		if (cb_p->ac_stat & AC_SW_C)
			break;
		delay(10);
	}
		
	/*
	 * Check for OK status.
	 * NOTE: selftest via PORT can't be done here.
	 */
	if ((i == 0) || ((cb_p->ac_stat & AC_SW_OK) == 0)) {
	    printf("SDB Ethernet %d:%d Command %d failed with status 0x%x\n",
		sp->ds_if.if_unit, sp->node_id, cmd, cb_p->ac_stat);
	    return 0;
	}

	/*
	 * Wait for 82596 to indicate it is no longer active.
	 */
	while (scb_p->scb_status & SCB_CUS_ACTV)
		delay(1);
	/*
	 * Acknowledge completion of the command.
	 * Note: Recv interrupts are acknowledged but now processed here.
	 */
	sdbe_attn(sp, scb_p->scb_status & SCB_SW_INT);
	return 1;
}


/*
 * sdbe_setinput:
 *
 *	Call kernel to set packet filter.
 */
/* ARGSUSED */
sdbe_setinput(dev, receive_port, priority, filter, filter_count)
	dev_t		dev;
	mach_port_t	receive_port;
	int		priority;
	filter_t	filter[];
	u_int		filter_count;
{
	ELOG_ENTRY(14, 
   ("sdbe_setinput(dev=0x%x, port=0x%x, pri=0x%x, filter=0x%x, count=0x%x)\n",
			dev, receive_port, priority, filter, filter_count));

	/*
	 * Sanity check entry to this function.
	 */
	if (sdbe_softc.state != SDBE_OPEN) {
		printf("SDB Ethernet %d:%d setinput called in wrong state\n",
				sdbe_softc.ds_if.if_unit, sdbe_softc.node_id);
		return ENXIO;
	}
	else	
		return net_set_filter(&sdbe_softc.ds_if, receive_port,
					priority, filter, filter_count);
}


/*
 * sdbe_getstat:
 *
 *	Call kernel to return status.
 */
/* ARGSUSED */
sdbe_getstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	u_int		*count;
{
	ELOG_ENTRY(15,
	    ("sdbe_getstat(dev=0x%x, flavor=0x%x, status=0x%x, count=0x%x)\n",
						dev, flavor, status, count));

	/*
	 * Sanity check entry to this function.
	 */
	if (sdbe_softc.state != SDBE_OPEN) {
		printf("SDB Ethernet %d:%d getstat called in wrong state\n",
				sdbe_softc.ds_if.if_unit, sdbe_softc.node_id);
		return ENXIO;
	}
	else	
		return net_getstat(&sdbe_softc.ds_if, flavor, status, count);
}


/*
 * sdbe_setstat:
 *
 *	Say something witty here
 */
/* ARGSUSED */
sdbe_setstat(dev, flavor, status, count)
	dev_t		dev;
	int		flavor;
	dev_status_t	status;
	u_int		count;
{
	struct net_status *ns = (struct net_status *)status;
	sdbe_softc_t	  *sp = &sdbe_softc;

	ELOG_ENTRY(16,
	    ("sdbe_setstat(dev=0x%x, flavor=0x%x, status=0x%x, count=0x%x)\n",
						dev, flavor, status, count));
	/*
	 * Sanity check entry to this function.
	 */
	if (sp->state != SDBE_OPEN) {
		printf("SDB Ethernet %d:%d setstat called in wrong state\n",
						sp->ds_if.if_unit, sp->node_id);
		return ENXIO;
	}

	switch (flavor) {
	case NET_STATUS:
		ELOG(ELOG_LOW, ("sdbe_setstat: NET_STATUS->flags=0x%x\n",
								ns->flags));
		/*
		 * Sanity check that the status structure passed is at
		 * least as large as a net_status structure.
		 */
		if (count < NET_STATUS_COUNT) {
			ELOG(ELOG_HIGH,("sdbe_setstat: Bad count (%d)\n",
								(int)count));
			return D_INVALID_OPERATION;
		}

		/*
		 * Set 82596 for proper promiscuous mode.
		 */
		if (sdbe_set_promisc(sp, ns->flags & IFF_PROMISC))
			return D_IO_ERROR;
		break;

	case NET_ADDRESS: {
		union ether_cvt {
			u_char	addr[6];
			u_long	lwd[2];
		} *ec = (union ether_cvt *)status;

		/*
		 * Sanity check that the status structure passed is at
		 * least as large as our conversion structure.
		 */
		if (count < sizeof(*ec) / sizeof(int)) {
			ELOG(ELOG_HIGH,("sdbe_setstat: Bad count (%d)\n",
								(int)count));
			return D_INVALID_SIZE;
		}

		ec->lwd[0] = ntohl(ec->lwd[0]);
		ec->lwd[1] = ntohl(ec->lwd[1]);

		ELOG(ELOG_HIGH,
			("sdbe_setstat: NET_ADDRESS = %x:%x:%x:%x:%x:%x\n",
				ec->addr[0], ec->addr[1], ec->addr[2],
				ec->addr[3], ec->addr[4], ec->addr[5]));

		sdbcopy((caddr_t)ec->addr, (caddr_t)sp->ds_addr, ADDR_LEN);
		if (!sdbe_set_eaddr(sp))
			return D_IO_ERROR;
		break;
        }

	default:
		return D_INVALID_OPERATION;
	}

	return D_SUCCESS;
}


/*
 * sdbe_output:
 *
 *	Call kernel to get frame into memory then have it call sdbe_start()
 *	to get it queued for the wire.
 */
/* ARGSUSED */
sdbe_output(dev, ior)
	dev_t	 dev;
	io_req_t ior;
{
	ELOG_ENTRY(17, ("sdbe_output(dev=0x%x, ior=0x%x)\n", dev, ior));

	/*
	 * Sanity check entry to this function.
	 */
	if (sdbe_softc.state != SDBE_OPEN) {
		ELOG(ELOG_HIGH, ("sdbe_output: Wrong state (%d)\n",
							sdbe_softc.state));
		return ENXIO;
	}
	else	
		return net_write(&sdbe_softc.ds_if, sdbe_start, ior);
}


/*
 * sdbe_start:
 *
 *	This function will copy transmit data to SDB memory if there is
 *	room in the transmit queue. If the 82596 is not currently sending
 *	something on the wire it will start a transmit.
 *
 *	NOTE:	It is assumed that this routine is called with the SPL
 *		set to SPLNET. Net_write() calls at splimp(). Routines
 *		that call sdbe_start() need to set the SPL at the proper
 *		level.
 *		
 *		This version of the driver copies the data from memory
 *		to the transmit queue which isn't necessary.
 */
/* ARGSUSED */
void STATIC 
sdbe_start(unit)
	int	unit;
{
	sdbe_softc_t	*sp = &sdbe_softc;
	io_req_t	req;			/* pointer value */

	ELOG_ENTRY(18, ("sdbe_start(unit=%d)\n", unit));

	/*
	 * While there is room in the transmit queue and requests queued
	 * from the kernel, get them copied to SDB memory and linked into
	 * the 82596 queue.
	 */
	while (sp->tx_qtail->cmd.ac_next != (ac_t *)VIRT_TO_SDBE(sp->tx_qhead)){
		IF_DEQUEUE(&sp->ds_if.if_snd, req);
		if (req) {
			sdbe_q_tx(sp, req);
			iodone(req);
		} else {
			break;
		}
	}

	/*
	 * Check for full queue so statistics can be kept.
	 */
	if (sp->tx_qtail->cmd.ac_next == (ac_t *)VIRT_TO_SDBE(sp->tx_qhead)) {
		ELOG(ELOG_MEDIUM, ("sdbe_start: tx queue full\n"));
		STATS(sp, tx.q_full++);
	}

	/*
	 * Start the 82596 on what's queued up.
	 */
	sdbe_xmit(sp);
}


/*
 * sdbe_q_tx:
 *
 *	This routine will copy user data into SDB memory and queue the
 *	request for transmission. This allows multiple transmit commands
 *	to be chained together when the system can generate transmit requests
 *	faster than the network can take them. See sdbe_bld_txq() for details
 *	on how queue is managed.
 */
STATIC void
sdbe_q_tx(sp, req)
	sdbe_softc_t	*sp;
	io_req_t	req;
{
	struct ether_header *eh_p = (struct ether_header *)req->io_data;
	u_short		    len;
	v_tx_q_t	    cur;
	v_tx_q_t	    new;
	v_ac_t		    cb_p;

	ELOG_ENTRY(19, ("sdbe_q_tx(sp=0x%x, req=0x%x)\n", sp, req));

	/*
	 * Set new and current queue pointers checking for empty queue
	 * condition (tx_qhead == 0).
	 */
	new = cur = sp->tx_qtail;
	if (sp->tx_qhead == 0) {
		sp->tx_qhead = new;
		STATS(sp, tx.q_miss++);
	} else {
		new = (v_tx_q_t)SDBE_TO_VIRT(cur->cmd.ac_next);
		STATS(sp, tx.q_hit++);
	}

	/*
	 * Fill out the transmit packet information.
	 */
	cb_p = &new->cmd;
	cb_p->ac_stat = 0;
	cb_p->ac_cmd = (AC_CW_EL | AC_TRANSMIT | AC_CW_I | AC_CW_SF );
	cb_p->ac_func.xmit.tc_tbda = (tbd_t *)VIRT_TO_SDBE(&new->tbd);
	cb_p->ac_func.xmit.tc_count = ADDR_LEN + sizeof(short);
	sdbcopy((caddr_t)eh_p->ether_dhost,
		(caddr_t)cb_p->ac_func.xmit.tc_dest, ADDR_LEN);
	cb_p->ac_func.xmit.tc_length = eh_p->ether_type;

	/*
	 * Copy data into transmit buffer 
	 */
	new->tbd.tbd_buf = (u_char *)VIRT_TO_SDBE(new->buf);
	new->tbd.tbd_size = MAX(req->io_count - sizeof(struct ether_header),
				ETHERMIN) | TBD_EOF;
	len = req->io_count - sizeof(struct ether_header);
	sdbcopy(req->io_data + sizeof(struct ether_header),
		(caddr_t)new->buf, len);

	/*
	 * It is important that we don't expose the new command block
	 * (by clearing the EL bit) until we are completely done initializing
	 * it.
	 */
	if (new != cur) {
		cur->cmd.ac_cmd = AC_TRANSMIT | AC_CW_SF;
		sp->tx_qtail = new;
	}
	
	/*
	 * Update stats
	 */
	sp->ds_if.if_opackets++;
	STATS(sp, tx.packets++);
	STATS(sp, tx.bytes += len);
}


/*
 * sdbe_xmit:
 *
 *	The routine will start the 82596 processing the beginning of
 *	the transmit command chain.
 *
 *	Note: This procedure provides that the C-Unit is not started
 *	when it is already active as per 82596 Erratum: 31.0 (CU start
 *	while CU active).
 */
STATIC void
sdbe_xmit(sp)
	sdbe_softc_t	*sp;
{
	v_scb_t	scb_p = sp->scb_p;

	ELOG_ENTRY(20, ("sdbe_xmit(sp=0x%x)\n", sp));

	/*
	 * If the 82596 is currently transmitting a request just return
	 * and let the completion interrupt pickup queued requests.
	 * Otherwise, if there is something in the transmit queue, start it.
	 */
	if (sp->custate != SDBE_READY) {
		ELOG(ELOG_MEDIUM,
			("sdbe_xmit: tx busy-error %d\n",sp->custate));
		STATS(sp, tx.busy++);
		return;
	}

	/*
	 * Check for non-empty queue.
	 */
	if (sp->tx_qhead == 0) {
		ELOG(ELOG_LOW, ("sdbe_xmit: queue empty\n"));
		return;
	}

	/*
	 * Place the request on the queue and kick the device in the head.
	 */
 	sp->custate = SDBE_TXBUSY;
	scb_p->scb_cba = (ac_t *)VIRT_TO_SDBE(sp->tx_qhead);
	sdbe_attn(sp, SCB_CU_START);
}


/*
 * sdbe_intr:
 *
 *	This function is the interrupt handler for the SDB Ethernet
 *	interface. This routine will be called whenever a transmit
 *	command chain is complete or a packet is received.
 */
/* ARGSUSED */
void
sdbe_intr(unit)
	int unit;
{
	sdbe_softc_t	*sp  = &sdbe_softc;
	v_scb_t		scb_p = sp->scb_p;
	u_short		status;

	ELOG_ENTRY(21,
		("sdbe_intr(unit=0x%x)=stat(0x%x)\n", unit, scb_p->scb_status));

	/*
	 * Sanity check
	 */
	if (sp->state != SDBE_OPEN) { 
		printf("SDB Ethernet %d:%d interrupt before initialization\n",
						sp->ds_if.if_unit, sp->node_id);
		return;
	}

	/*
	 * Wait for the 82596 to indicate status if valid.
	 */
	if (sdbe_wait(sp, 100)) {
		printf("sdbe_intr: Wait timeout...\n");
		sp->custate = SDBE_ERROR;
		return;
	}

	/*
	 * Check for spurious interrupts.
	 */
	status = scb_p->scb_status;
	if ((status & SCB_SW_INT) == 0) {
		/*
		 * Note: Resource Interupts need to be handled
		 * whenever thye occur.  This code is paranoia
		 * code which handles them, even if they don't
		 * deserve it.
		 */
		if (status & SCB_RUS_NORBDS) {
			sdbe_rx_i(sp, status);
			printf("Resource spurious interrupt = status 0x%x\n",
				status);
		}
#ifdef	DEBUG
		if (status == SCB_RUS_READY) {
		  ELOG(ELOG_LOW,
		    ("sdbe_intr: receive unit ready - status=0x%x\n",status));
		} else {
		  ELOG(ELOG_LOW,
		    ("sdbe_intr: spurious interrupt - status=0x%x\n",status));
		}
#endif	DEBUG
		STATS(sp, spurious_int++);
		return;
	}

	/*
	 * Acknowledge the current interrupt.
	 */
	sdbe_attn(sp, status & SCB_SW_INT);

	/*
	 * Process received frames. If we went RNR then
	 * sdbe_rx_i() will restart the RU.
	 */
	if (status & (SCB_SW_FR | SCB_SW_RNR)) {
		sdbe_rx_i(sp, status);
	}

	/*
	 * Process transmitted packets.
	 */
	if (status & (SCB_SW_CX | SCB_SW_CNA)) {
		sp->custate = SDBE_READY;
		sdbe_tx_i(sp);
	}

	/*
	 * Accumulate errors kept by the chip.
	 */
	STATS(sp, rx.crcerrs += scb_p->scb_crcerrs);
	STATS(sp, rx.alnerrs += scb_p->scb_alnerrs);
	STATS(sp, rx.rscerrs += scb_p->scb_rscerrs);

	scb_p->scb_crcerrs = scb_p->scb_alnerrs = scb_p->scb_rscerrs = 0;

	return;
}


/*
 * sdbe_rx_i:
 *
 *	Called by the interrupt entry point sdbe_intr(). Process all
 *	received frames. If we went RNR (ran out of receive buffers),
 *	restart the RU.
 *
 * Note:The routine assumes that buffer chaining has not been done
 *	due to errata on the 82586. This simplifies/speeds up receive
 *	frame processing. No such errata on the 82596 exists.
 */
#define	ONLY_ONE(p)	(p->rfd.rfd_actual_cnt & RBD_EOF)

STATIC
sdbe_rx_i(sp, scb_status)
	sdbe_softc_t	*sp;
	u_short		scb_status;
{
	v_fd_t		fd_p;
	u_short		status;

	ELOG_ENTRY(22, ("sdbe_rx_i(0x%x)\n", scb_status));
	STATS(sp, rx.interrupts++);

	/*
	 * While there are completed frames to process, do it.
	 */
	fd_p = sp->fd_q;
	while( fd_p && fd_p->rfd_rbda != (rbd_t *)NIL_ADDR ) {

		status = fd_p->rfd_stat;

		if ( status & AC_SW_B )		/* busy wait for it! */
			break;
		if ( !( status & AC_SW_C) )	/* not done, wait for it! */
			break;

		if ((status & AC_GOOD) == AC_GOOD) {
			/* Bring the frame into memory */
			sdbe_read(sp, fd_p);

			/*
			 * Requeue FD and RB's when the RFD has been completed
			 */
			sdbe_requeue(sp, fd_p);

			/*
			 * Assign the next RFD as head of the queue
			 * and handle status processing.
			 */
			sp->fd_q = (v_fd_t)SDBE_TO_VIRT(fd_p->rfd_next);
		}
		else if (status & AC_SW_C) {
			/* Record the errors. */

			sp->ds_if.if_ierrors++;

			sdbe_requeue(sp, fd_p);
			sp->fd_q = (v_fd_t)SDBE_TO_VIRT(fd_p->rfd_next);
		}
		fd_p = (v_fd_t)SDBE_TO_VIRT(fd_p->rfd_next);
	}

	/*
	 * If the receive unit is not ready then restart it.
	 * Note: The code below is equivilent to:
	 * if ((scb_status & SCB_RUS) == SCB_RUS_NORBDS  ||
	 *     (scb_status & SCB_RUS) == SCB_RUS_NOPRBDS ||
	 *     (scb_status & SCB_RUS) == SCB_RUS_NORESRC) {
	 */
	if (scb_status & SCB_RUS_NORBDS) {
		STATS(sp, rx.null_rbd++);
		sdbe_attn(sp, SCB_RU_ABORT);	/* ensure not in ready state */
		sdbe_start_ru(sp);		/* For ERRATUM 32.0 */
	}

	if ( (scb_status & SCB_RUS) == SCB_RUS_IDLE )
		sdbe_start_ru(sp);

	return;
}


STATIC
sdbe_dumpit(sp, fd_p)
	sdbe_softc_t	*sp;
	v_fd_t		fd_p;
{
	v_rbd_t	rx_p;
	u_short frame_len;
	u_short last_chunk;

#ifdef DEBUG_ALL
	rx_p = (v_rbd_t)SDBE_TO_VIRT(fd_p->rfd_rbda);

	printf("sdbe_dumpit: fd 0x%x\n", fd_p);
	printf(
	"fd 0x%x cmd 0x%x status 0x%x, next 0x%x rbd 0x%x act 0x%x sz 0x%x\n",
		fd_p, fd_p->rfd_cmd, fd_p->rfd_stat, fd_p->rfd_next,
		fd_p->rfd_rbda, fd_p->rfd_actual_cnt, fd_p->rfd_size);
	printf("eth: 0x%x 0x%x, len 0x%x\n", fd_p->rfd_dest_addr[0],
		fd_p->rfd_dest_addr[1], fd_p->rfd_length);

	if (rx_p != (v_rbd_t)NIL_ADDR) {
		do {
			printf(
		    "trbd 0x%x : ac 0x%x bo 0x%x buf 0x%x sz 0x%x -> 0x%x\n",
				rx_p, rx_p->rbd_act_count,
				rx_p->rbd_zero, rx_p->rbd_buf,
				rx_p->rbd_size, rx_p->rbd_next);
			last_chunk = rx_p->rbd_act_count & RBD_EOF;
			rx_p = (v_rbd_t)SDBE_TO_VIRT(rx_p->rbd_next);
		} while (!last_chunk);
	}
#endif 
}


/*
 * sdbe_get_framelen - get frame length for the received packet buffer chain.
 *
 *	This routine calculates the amount of data associated with the
 *	RFD by totalling the actual count fields of both RFD and RBD
 *	descriptors.  This total is the frame length.
 *
 *	returns: frame length
 */
STATIC u_short
sdbe_get_framelen(sp, fd_p)
	sdbe_softc_t	*sp;
	v_fd_t		fd_p;
{
	v_rbd_t	rx_p;
	u_short frame_len;
	u_short last_chunk;

	frame_len = 0;
	rx_p = (v_rbd_t)SDBE_TO_VIRT(fd_p->rfd_rbda);

	do {
		if (rx_p == (v_rbd_t)NIL_ADDR) {
#ifndef IGNORE_ERRATA_3_32
			printf("sdbe_get_framelen: Bad frame rx_p 0x%x\n",
				fd_p);
			sdbe_dumpit(sp, fd_p);
#endif
			return(frame_len);
		}
		if (rx_p->rbd_act_count & RBD_STAT_F) {
			frame_len += rx_p->rbd_act_count & RBD_COUNT;
			last_chunk = rx_p->rbd_act_count & RBD_EOF;
			rx_p = (v_rbd_t)SDBE_TO_VIRT(rx_p->rbd_next);
		} else {
			last_chunk = 1;
			printf(
			    "sdbe_get_framelen: Bad frame in 0x%x count 0x%x\n",
				rx_p, rx_p->rbd_act_count);
		}
	} while (!last_chunk);

	return frame_len;
}


/*
 * sdbe_copy_frame - gather frame chunks up into a packet.
 *
 * The only reason this procedure is here is because there
 * might be a use for a gathering count (similar to checksum'ing
 * an mbuf chain). Actually, if the buffers are managed in smaller
 * chunks than the full size (eg. >1 rbd per rfd) this procedure
 * is needed since the rbds can wrap.
 */
STATIC void
sdbe_copy_frame(sp, rx_p, offset, dest, len)
	sdbe_softc_t	*sp;
	v_rbd_t		rx_p;
	short		offset;
	caddr_t		dest;
	short		len;
{
	u_short	last_chunk;
	short	rx_len;

	do {
		if (rx_p->rbd_act_count & RBD_STAT_F) {
			rx_len = (rx_p->rbd_act_count & RBD_COUNT) - offset;
			if (rx_len > 0) {
#ifdef XFER_DEBUG
				printf("sdbe_copy_frame: 0x%x 0x%x 0x%x\n",
					rx_p->rbd_buf+offset, dest, rx_len);
#endif
				sdbcopy((caddr_t)SDBE_RX_DATA_TO_VIRT
					(rx_p->rbd_buf) + offset,
					dest, (int)rx_len);
				dest += rx_len;
			}
			offset = 0;	/* Only first bcopy gets offset */
			last_chunk = rx_p->rbd_act_count & RBD_EOF;
			rx_p = (v_rbd_t)SDBE_TO_VIRT(rx_p->rbd_next);
		} else {
			printf("sdbe_copy_frame: Missing RBD_STAT_F 0x%x\n",
				rx_p);
			break;
		}
	} while (!last_chunk);

	return;
}


/*
 * sdbe_read:
 *
 *	This routine does the actual copy of data (including Ethernet header
 *	structure) from SDB memory to an ipc_kmsg_t. Errata on the 82596
 *	indicates it can botch receive buffer switching when the last buffer
 *	only contains two or three bytes. The work-around is to not use
 *	buffer chaining. Because of this, we do not do buffer walking on
 *	received buffers but assume the complete frame is contained in a
 *	single buffer. (This is also much quicker when you have the memory
 *	to spare!)
 */
STATIC void
sdbe_read(sp, fd_p)
	sdbe_softc_t	*sp;
	v_fd_t		fd_p;
{
	struct ifnet		*ifp = &(sp->ds_if);
	struct ether_header	*ehp;
	struct packet_header	*pkt;
	ipc_kmsg_t		new_kmsg;
	short			frame_len;
	v_rbd_t			rx_p;
	int			s;

	ELOG_ENTRY(23,
		("sdbe_read(fd=0x%x,rx_p=0x%x)\n", fd_p, fd_p->rfd_rbda));

	rx_p = (v_rbd_t)SDBE_TO_VIRT(fd_p->rfd_rbda);

	/*
	 * If the interface has been marked down, don't bother reading
	 * the frame into kernel memory.
	 */
	if ((ifp->if_flags & IFF_UP) == 0)
		return;

	/*
	 * get frame lens
	 */
	frame_len  = sdbe_get_framelen(sp, fd_p);
	frame_len -= sizeof(struct ether_header);

	if (frame_len < ETHERMIN) {
		/* Paranoia, just drop this packet */
		if (frame_len + sizeof(struct ether_header) == 0)
			printf("Zero Length Packet Bug detected 0x%x\n", fd_p);
		else
			printf("Short Packet Bug detected 0x%x(0x%x)\n",
				fd_p, frame_len);
		sdbe_dumpit(sp, fd_p);
		return;
	}

	/*
	 * Get kernel memory to receive incoming packet.
	 */
	if ((new_kmsg = net_kmsg_get()) == IKM_NULL) {
		ELOG(ELOG_HIGH, ("sdbe_read: IKM_NULL\n"));
		STATS(sp, rx.no_buffs++);
		ifp->if_rcvdrops++;
		return;
	}

	/*
 	 * Set pointers
	 */
	ehp = (struct ether_header *)  net_kmsg(new_kmsg)->header;
	pkt = (struct packet_header *) net_kmsg(new_kmsg)->packet;

	/*
	 * Copy Ethernet header.
	 *
	 * Since these three fields are contiguous in both structures,
	 * one could do a single bcopy. Since the copy below is faster
	 * if the target address is copied 4 bytes at a time we do the
	 * addresses and length separate (cheese anyone? ).
	 *
	 * No longer kept in header....
	 * bcopy(fd_p->rfd_dest_addr, ehp->ether_dhost, ADDR_LEN);
	 * bcopy(fd_p->rfd_src_addr,  ehp->ether_shost, ADDR_LEN);
	 * ehp->ether_type = fd_p->rfd_length;
	 */
	sdbcopy((caddr_t)SDBE_RX_DATA_TO_VIRT(rx_p->rbd_buf),
		(caddr_t)ehp,
		 sizeof(struct ether_header));

	/*
	 * Copy frame.
	 * Just like
	 * bcopy(rx_p->rbd_buf, &net_kmsg(new_kmsg)->header[0], frame_len);
	 * but handles the gaps...
	 */
	sdbe_copy_frame(sp, rx_p, (short)sizeof(struct ether_header),
			(caddr_t)(pkt+1), frame_len);
	/*
	 * Build packet header.
	 */
	pkt->type   = ehp->ether_type;
	pkt->length = frame_len + sizeof(struct packet_header);

	ELOG(ELOG_LOW,
		("sdbe_read: stat 0x%x %d bytes from %x:%x:%x:%x:%x:%x\n",
			fd_p->rfd_stat, frame_len,
			ehp->ether_shost[0], ehp->ether_shost[1],
			ehp->ether_shost[2], ehp->ether_shost[3],
			ehp->ether_shost[4], ehp->ether_shost[5]));
	/*
	 * Send the packet to the network module.
	 */
	ifp->if_ipackets++;
	STATS(sp, rx.packets++);
	STATS(sp, rx.bytes += frame_len);

	s = splimp();	/* net_packet() assumes splimp() */
	net_packet(ifp, new_kmsg, pkt->length,
		(fd_p->rfd_stat & RFD_BROADCAST) ? FALSE : TRUE);
	splx(s);
	return;
}


/*
 * sdbe_freerbds:
 *
 * 	This routine frees a chain of RBD descriptors.
 */
STATIC void	
sdbe_freerbds(sp, rx_p)
	sdbe_softc_t	*sp;
	v_rbd_t		rx_p;
{
	v_rbd_t		lastrx_p;
	u_short		last_chunk;
	u_short		t_size;

	/*
	 * While it's not the last one and it's not prefetched and
	 * it's not the tail of the queue... ok, I'm paranoid... ;-)
	 *
	 * Reinitialize and reuse the RBD buffers.
	 */
	do {
		last_chunk = rx_p->rbd_act_count & RBD_EOF;
		rx_p->rbd_act_count = 0;
		rx_p->rbd_size = SDBE_BUF_SIZE;
		lastrx_p = rx_p;
		rx_p = (v_rbd_t)SDBE_TO_VIRT(rx_p->rbd_next);
	} while (!last_chunk
	&& rx_p != sp->rx_rbdtail
	&& !(rx_p->rbd_size & RBD_STAT_P));

	/*
	 * Ensure the last one is always terminated properly.
	 * Note: Must check both P and F bits to determine if
	 * 	 the dynamic chaining link occurs properly.
	 */
	lastrx_p->rbd_size = SDBE_BUF_SIZE|RBD_EL;
	t_size = sp->rx_rbdtail->rbd_act_count & RBD_STAT_F;
	t_size |= (sp->rx_rbdtail->rbd_size & ~RBD_EL);
	if ( !( t_size & RBD_STAT_P ) ) {	/* not too late to add */
		sp->rx_rbdtail->rbd_size = t_size;
		/*
		 * Check again to handle access race condition.
		 * If we see a Prefetch/Full bit go active then the
		 * queue starts with the rbd which was the first
		 * reinitialized rbd prior to the lack of rbd resources
		 * occurrence.
		 */
		t_size = sp->rx_rbdtail->rbd_size;
		if ( t_size & RBD_STAT_P ) {
			if (sp->rx_unk)
				printf("RX UNK Twice!\n");
			sp->rx_unk = sp->rx_rbdtail;	/* Lost race */
		}
	}
	sp->rx_rbdtail = lastrx_p;
}


/*
 * sdbe_requeue:
 *
 *	This routine puts spent FD's and RBD's back onto the free receive
 *	frame area (RFA).
 */
#define	EOP(p)	\
	((u_int)(p) == (u_int)NIL_ADDR || !((p)->rbd.rbd_act_count & RBD_EOF))

STATIC void
sdbe_requeue(sp, fd_p)
	sdbe_softc_t	*sp;
	v_fd_t		fd_p;
{
	v_rbd_t		rx_p;
	u_short		stat;

	rx_p = (v_rbd_t)SDBE_TO_VIRT(fd_p->rfd_rbda);

	{
	ELOG_ENTRY(24,
		("sdbe_requeue(sp=0x%x, fd_p=0x%x n=0x%x t 0x%x, rx_p=0x%x)\n",
				sp, fd_p, fd_p->rfd_next, sp->fd_q_tail, rx_p));
	}

	/*
	 * Check status and free all RBD descriptors.
	 */
	if (fd_p->rfd_stat & RFD_BUSY) {
		printf("sdbe_requeue() called on active buffer\n");
		return;
	}
	sdbe_freerbds(sp, rx_p);

	/*
	 * Indicate fd_p is now the anchor for the end of the list and
	 * reinitialize its fields. Link it into the free FD list.
	 */
	fd_p->rfd_cmd	= AC_CW_EL|AC_CW_SF|AC_CW_I;
	fd_p->rfd_stat	= 0;
	fd_p->rfd_actual_cnt = 0;		/* Clear 'F' full bit */
	fd_p->rfd_rbda	= (v_rbd_t)NIL_ADDR;
	fd_p->rfd_size	= 0;		/* Put the entire packet in buffer */

	/*
	 * Handle the dynamic chaining. If the link took then
	 * we have added the fd_p to the tail of the chain.
	 * If not then we stop updating the tail pointer
	 * and wait for the out of resources interrupt.
	 */
	stat = sp->fd_q_tail->rfd_stat;
	if ( sp->fd_q_tail != fd_p ) {
		if ( !( stat & (RFD_BUSY|RFD_DONE) ) ) {
			sp->fd_q_tail->rfd_cmd = AC_CW_SF|AC_CW_I;
			stat = sp->fd_q_tail->rfd_stat;
			if ( ( stat & (RFD_BUSY|RFD_DONE))) {
				if (sp->fd_unk)
					printf("FD UNK Twice!\n");
				sp->fd_unk = fd_p;
			}
		}
	}
	sp->fd_q_tail = fd_p;
}


/*
 * sdbe_tx_i:
 *
 *	This routine will process completed transmit requests. It will
 *	adjust the transmit queue pointer and start another transmit chain
 *	if there are requests in the queue.
 */
STATIC void
sdbe_tx_i(sp)
	sdbe_softc_t	*sp;
{
	v_ac_t		cb_q;
	u_short		status;

	ELOG_ENTRY(25, ("sdbe_tx_i(sp=0x%x)\n", sp));
	STATS(sp, tx.interrupts++);

	/*
	 * Get the queue head and make sure it is non-zero.
	 */
	if ((cb_q = (v_ac_t)sp->tx_qhead) == 0) {
		ELOG(ELOG_HIGH, ("sdbe_tx_i: NULL queue head ptr\n"));
		STATS(sp, tx.q_null++);
		return;
	}

	/*
	 * Walk the transmit queue until we hit the tail pointer or we
	 * find a command that has not been completed.
	 */
	while (cb_q) {
		status = cb_q->ac_stat;
		if ((status & AC_SW_C) == 0)
			break;

		ELOG(ELOG_LOW, ("sdbe_tx_i: 0x%x complete with status 0x%x\n",
						cb_q, cb_q->ac_stat));
		cb_q->ac_cmd = AC_CW_EL|AC_CW_SF;

		/*
		 * Check and keep stats on errors.
		 */
		if ((status & AC_SW_OK) == 0) {
			ELOG(ELOG_HIGH,
			    ("sdbe_tx_i: tx error - status=0x%x\n", status));
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
		"SDB Ethernet %d:%d transmission link error - check cable\n",
						sp->ds_if.if_unit, sp->node_id);
		}

		if (status & TC_DEFER)
			STATS(sp, tx.defer++);
		
		sp->ds_if.if_collisions += (status & 0x0f);

		/*
		 * Bump pointer to next command block checking for end of queue.
		 */
		if (cb_q == (v_ac_t)sp->tx_qtail)
			cb_q = 0;
		else
			cb_q = (v_ac_t)SDBE_TO_VIRT(cb_q->ac_next);
	}
	sp->tx_qhead = (v_tx_q_t)cb_q;

	/*
	 * Pick up any queued requests that may have been blocked.
	 */
	sdbe_start(sp->ds_if.if_unit);
}


/*
 * sdbe_set_promisc:
 *
 *	Set or reset 82596 promiscuous mode.
 */
STATIC
sdbe_set_promisc(sp, flag)
	sdbe_softc_t	*sp;
	int		flag;
{
	ELOG_ENTRY(26, ("sdbe_set_promisc(sp=0x%x, flag=%d)\n", sp, flag));

	if (flag) {
		sp->mode	   |= SDBE_PROMISC;
		sp->ds_if.if_flags |= IFF_PROMISC;
	} else {
		sp->mode	   &= ~SDBE_PROMISC;
		sp->ds_if.if_flags &= ~IFF_PROMISC;
	}
	return sdbe_config(sp);
}


/*
 * sdbe_rd_eaddr: Get and validate Ethernet address from the serial rom.
 *
 * Returns 0 if all is ok.
 */
STATIC
sdbe_rd_eaddr(sp, addr_p)
	sdbe_softc_t	*sp;
	u_char		*addr_p;
{
	ELOG_ENTRY(27, ("sdbe_rd_eaddr(sp=0x%x, addr_p=0x%x)\n", sp, addr_p));

	/*
	 * Get system defined Ethernet address.
	 */
	if (x24c02_read(addr_p, ADDR_LEN, 0, 0) != ADDR_LEN)
		addr_p[1] = 0;	/* Cause validation error below */

	/*
	 * Validate it (as best we can).
	 */
	if ((addr_p[0] != 0) || (addr_p[1] != 0xaa) || (addr_p[2] != 0)) {
		printf ("Warning: Ethernet address appears invalid\n");
		return 1;
	}

	return 0;
}


/*
 * sdbe_wait:
 *
 *	Wait for the SCB to be ready.
 */
STATIC
sdbe_wait(sp, time)
sdbe_softc_t	*sp;
int		time;
{
	ELOG_ENTRY(28, ("sdbe_wait(sp=0x%x, time=%d, cmd=0x%x)\n",
				sp, time, sp->scb_p->scb_command));
	STATS(sp, wait_calls++);

	while (time--) {
		if (sp->scb_p->scb_command == 0) {
			return 0;
		}

		delay(10);
		STATS(sp, wait_spins++);
	}
	
	printf("SDB Ethernet %d:%d wait timeout\n",
		sp->ds_if.if_unit, sp->node_id);
	STATS(sp, wait_timeouts++);

	return 1;
}


/*
 * sdbe_attn:
 *
 *	Grab the i82596's attention and give it a command to do.
 */
sdbe_attn(sp, start)
	sdbe_softc_t	*sp;
	u_short		start;
{
	if (sdbe_wait(sp, 1000)) {
		printf("sdbe_attn: wait timeout\n");
		return;
	}
	sp->scb_p->scb_command  = start;
	CHANN_ATTN(sp);
}


/*
 * sdbe_print_stats:
 *
 *	Debugger print routine
 */
sdbe_print_stats()
{
#ifdef SDBE_STATS
	sdbe_stats_t	*s = &sdbe_softc.stats;

printf("tx stats:\n");
printf("packets  = %08d bytes = %08d inter = %08d defer   = %08d\n",
	  s->tx.packets, s->tx.bytes, s->tx.interrupts, s->tx.defer);

printf("fatal_col = %08d busy  = %08d errors = %08d underruns = %08d\n",
	  s->tx.fatal_collis, s->tx.busy, s->tx.errors, s->tx.underruns);

printf("q_full  = %08d q_null = %08d q_hit = %08d q_miss  = %08d\n",
	  s->tx.q_full, s->tx.q_null, s->tx.q_hit, s->tx.q_miss);

printf("\nrx stats:\n");
printf("packets = %08d bytes  = %08d inter  = %08d restarts = %08d\n",
	  s->rx.packets, s->rx.bytes, s->rx.interrupts, s->rx.restarts);

printf("crcerrs = %08d alnerrs = %08d rscerrs = %08d overnerrs = %08d\n",
	  s->rx.crcerrs, s->rx.alnerrs, s->rx.rscerrs, s->rx.ovrnerrs);

printf("q_empty = %08d no_buffs = %08d longs  = %08d null_rbd = %08d\n",
	  s->rx.q_empty, s->rx.no_buffs, s->rx.longs, s->rx.null_rbd);

printf("\nother stats:\n");
printf("timeouts = %08d  spurious_int = %08d  resets = %08d\n",
	  s->timeouts, s->spurious_int, s->resets);
	
printf("wait(calls = %d spins = %d timeouts = %d)\n",
	  s->wait_calls, s->wait_spins, s->wait_timeouts);

printf("\nif_stats:\n");
printf("ipackets = %08d  ierrors  = %08d  opackets = %08d\n",
	  sdbe_softc.ds_if.if_ipackets, sdbe_softc.ds_if.if_ierrors,
	  sdbe_softc.ds_if.if_opackets);

printf("oerrors = %08d  collisions = %08d  rcvdrops = %08d\n",
	 sdbe_softc.ds_if.if_oerrors, sdbe_softc.ds_if.if_collisions,
	 sdbe_softc.ds_if.if_rcvdrops);
#else
	printf("SDBE_STATS not defined\n");
#endif
}

#endif	NSDBE > 0
