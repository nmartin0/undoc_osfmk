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
 * Mach Operating System
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: if_cnp.c,v $
 * Revision 1.5  1994/11/18  20:37:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:29:30  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:27:52  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  18:28:26  dleslie
 * First R1_0 release
 *
 * Revision 2.4.2.1  92/01/09  18:43:58  jsb
 * 	Ethernet fixes from Intel.
 * 	[92/01/08  15:23:06  jsb]
 * 
 * Revision 2.4  91/12/11  08:49:29  jsb
 * 	Fix from Intel.
 * 	[91/12/11  08:48:27  jsb]
 * 
 * Revision 2.3  91/08/03  18:17:50  jsb
 * 	Added cnpintr_count, and code in cnpintr to set unit to zero
 * 	when cnpintr is actually called from an interrupt.
 * 	[91/07/17  13:56:50  jsb]
 * 
 * Revision 2.2  91/06/18  20:50:04  jsb
 * 	Corrected include of if_cnp.h to reflect new i386ipsc/cnp directory.
 * 	[91/06/18  19:49:46  jsb]
 * 
 * 	Moved here from i386ipsc for licensing reasons.
 * 	[91/06/18  19:44:34  jsb]
 * 
 * Revision 2.5  91/06/06  17:04:40  jsb
 * 	Use cnppoll (instead of cnptimeout) to check for packets.
 * 	[91/05/13  17:06:14  jsb]
 * 
 * Revision 2.4  91/03/16  14:47:11  rpd
 * 	Changed net_filter to net_packet.
 * 	[91/01/15            rpd]
 * 
 * Revision 2.3  91/01/08  15:12:27  rpd
 * 	Changed NET_KMSG_GET, NET_KMSG_FREE to net_kmsg_get, net_kmsg_put.
 * 	[91/01/05            rpd]
 * 
 * Revision 2.2  90/12/04  14:47:14  jsb
 * 	First checkin.
 * 	[90/12/04  10:56:30  jsb]
 * 
 */
/*
 *	File:	if_cnp.c
 *	Author:	Joseph S. Barrera III
 *
 *	Copyright (c) 1990 Joseph S. Barrera III
 *	Ethernet driver for CMC ENP-100, almost completely rewritten
 *	for MACH_KERNEL + BIA + i386.
 */
/*
 * 	Copyright (c) 1988 by Communication Machinery Corporation
 * 			All rights reserved.
 *
 * 	This software contains proprietary and confidential information
 * 	of Communication Machinery Corporation and its suppliers.  Use,
 * 	disclosure, or reproduction is prohibited without the prior express
 * 	written consent of Communication Machinery Corporation.
 */
/*
 *  Network Interface Driver for CMC ENP-100
 *  Modified from ENP-10 version 1/6/88 by John Mullen
 *  BCB chaining added 6/1/88 by John Mullen
 */
#define TRACEPKT 0

#include "cnp.h"
#if NCNP > 0

#include <kern/time_out.h>
#include <device/device_types.h>
#include <device/errno.h>
#include <device/io_req.h>
#include <device/if_hdr.h>
#include <device/if_ether.h>
#include <device/net_status.h>
#include <device/net_io.h>

#include <i386at/atbus.h>
#include <i386ipsc/bia.h>
#include <i386ipsc/cnp/if_cnp.h>

#if 1
#define	cmc_hz	hz
#else
#define	cmc_hz	1
#endif

#define	CMCWAIT		10000		/* WAG */

int     cnpprobe();
int	cnpattach();
int	cnpintr();
int	cnpinit();
int	cnpoutput();
int	cnpioctl();
int	cnptimeout();
int	cnpbroadcast();

int	(*cnpintrs[])() = {cnpintr, 0};

struct  isa_dev cnpinfo[ NCNP ];

struct  isa_driver cnpdriver =
	{cnpprobe, 0, cnpattach, "cnp", cnpinfo, 0, 0};

struct	isa_dev cnpinfo[ NCNP ] = {
        {
		&cnpdriver,	/* dev_driver */
		0,		/* dev_unit */
		0,		/* dev_ctlr */
		0,		/* dev_slave */
		0,		/* dev_alive */
		(caddr_t)0x300,	/* dev_addr */
		9,		/* dev_spl */
		13,		/* pic line for device */
		0,		/* dev_dk */
		0,		/* dev_flags */
		cnpintrs,	/* dev_intr */
		(caddr_t)0x300,	/* dev_start */
		0,		/* dev_len */
		0,		/* dev_type */
		0,		/* dev_forw */
		0,		/* dev_mi */
	}
};

static ringinit();
static ringempty();
static ringfull();
static ringput();
static ringget();
static ringfirst();
static delay();

/*
 * Ethernet software status per interface.
 *
 * Each interface is referenced by a network interface structure,
 * es_if, which the routing code uses to locate the interface.
 * This structure contains the output queue for the interface (struct arpcom),
 * and its address (struct ether_addr).
 *
 *
 * The arpcom struct contains a network interface struct if_net, an ethernet
 * address, and an internet address.  It is defined in /sys/netinet/if_ether.h.
 *
 * The if_net struct contains the name of the interface, the address of the
 * interface, a watchdog timer, a flag word for interface status and control,
 * major and minor device numbers, mbuf queues, function pointers, and
 * statistics counters, and additional information.  It is defined in
 * /sys/net/if.h.
 * 
 *
 * The ether_addr struct is defined in if_cnp.h.
 */

struct  cnp_softc {
	struct	ifnet	es_if;		/* generic interface header */
	u_char	es_enaddr[6];		/* Ethernet hardware address */
	struct	ether_addr es_boardaddr;/* board ethernet address */
	int	es_lostintrs;		/* count lost interrupts */
	RING256	es_free;		/* free BCB's */
	char	*es_base;
	short	es_mode;
	short	es_flags;
} cnp_softc[NCNP];


/*
 * Probe for device.
 *
 * This routine is required for all 4.3 device drivers.  The required "unit"
 * parameter is unneeded for this device.
 */
cnpprobe(reg, unit)
caddr_t reg;
int	unit;
{
	register volatile CNPDEVICE *addr = (CNPDEVICE *)reg;
	extern unsigned long export_enpaddr; /* XXX */

	if (! biaprobe())
		return (0);

	reg = (caddr_t) export_enpaddr;
	cnpinfo[unit].dev_addr = reg;
	addr = (CNPDEVICE *)reg;

	if (cnpreset(addr, unit) == 0)
		return (0);

	cnp_softc[unit].es_base = reg;

	return (CNPSIZE);
}

/*
 * Interface exists: make available by filling in network interface
 * record.  System will initialize the interface when it is ready
 * to accept packets. 
 */
cnpattach(ii)
register struct isa_dev *ii;
{
	struct cnp_softc *es = &cnp_softc[ii->dev_unit];
	register struct ifnet *ifp = &es->es_if;
	register volatile CNPDEVICE *addr = (CNPDEVICE *)ii->dev_addr;

	es->es_flags = 0;
	es->es_mode = 0;
	ifp->if_unit = ii->dev_unit;
	ifp->if_header_size = sizeof(struct ether_header);
	ifp->if_header_format = HDR_ETHERNET;
	ifp->if_address_size = 6;
	ifp->if_mtu = ETHERMTU;
	ifp->if_flags = IFF_BROADCAST;

	/*
	 * read the ethernet address off the board, one byte at a time.
	 */
	bcopy(&addr->cnp_addr.e_baseaddr, &es->es_boardaddr,
	    sizeof(es->es_boardaddr));
	bcopy(&es->es_boardaddr, es->es_enaddr, sizeof(es->es_enaddr));
	printf("cnp%d: hardware address %s\n", 
		ii->dev_unit, ether_sprintf(es->es_enaddr));

	ifp->if_address = (char *)&es->es_enaddr[0];
	if_init_queues(ifp);
}

/*
 * Reset of interface
 */
cnpreset(addr, unit)
register volatile CNPDEVICE *addr;
int unit;
{
	register int i;

	untimeout(cnptimeout, unit);	/* prevent multiple timers */

	/*
	 * reset controller and wait for it to become ready
	 */
	addr->cnp_csr = 0;
	addr->cnp_iow.hst2cnp_reset = 1;

	/*
	 * provide hardware settle time for this host environment
	 */
	delay(1000);
	for (i = 0; ; i++) {
		volatile u_short csr = ntohs(addr->cnp_csr);
		if (csr & CNPCSR_ERR) {
			printf("cnp%d: detected error on reset; csr %x\n",
			       unit, csr);
			return (0);
		}
		if (csr & CNPCSR_RDY) {
			break;
		}
		if (i == CMCWAIT) {
			printf("cnp%d: timed out waiting for reset; csr %x\n",
			       unit, csr);
			return (0);
		}
		delay(1000);
	}
	delay(100000);

	addr->cnp_mode = ntohs(E_NOTERM);

	/*
	 * let controller know where its "all_ram" region
	 * begins in our address space so it can present addresses
	 * correctly to us.
	 */
	addr->cnp_base = htonl((u_long) addr->cnp_ram);

	/*
	 * transfer control from CNP kernel to application firmware
	 */
	addr->cnp_intvector = htons(1); /* Must not be zero */
	addr->cnp_state = htons(0);
	addr->cnp_go = htons(0x8080);
	for (i = 0;
	     i < CMCWAIT && (ntohs(addr->cnp_csr) & CNPCSR_ONLINE) == 0;
	     i++) {
		delay(1000);
	}
	for (; i < CMCWAIT && ntohs(addr->cnp_state) != S_CNPRUN; i++) {
		delay(1000);
	}
	if (i == CMCWAIT) {
		printf("cnp%d: application firmware failed\n", unit);
		return (0);
	}
	return (1);
}

int cnp_init_done = 0;

/*
 * Initialization of interface; clear recorded pending
 * operations.
 */
cnpinit(unit)
int unit;
{
	register struct cnp_softc *es = &cnp_softc[unit];
	register struct ifnet *ifp = &es->es_if;
	int s;

	if ((ifp->if_flags & IFF_RUNNING) == 0) {
		/* 
		 * open for business
		 */
		cnp_init_done = 1;
		s = splimp();
		ifp->if_flags |= IFF_RUNNING; 
		ringinit(&es->es_free, 256);
		cnpintr(unit);		/* collect free BCB's */
		timeout(cnptimeout, unit, cmc_hz);
		splx(s);
	}
}

/*ARGSUSED*/
cnpopen(dev, flag)
	dev_t	dev;
	int	flag;
{
	register int	unit = minor(dev);

	if (unit < 0 || unit >= NCNP || cnp_softc[unit].es_base == 0)
	    return (ENXIO);

	cnp_softc[unit].es_if.if_flags |= IFF_UP;
	cnpinit(unit);
	return(0);
}

/*
 * Ethernet interface interrupt handler.
 */
cnpintr(unit)
int unit;	/* interrupting unit */
{
	register volatile CNPDEVICE *addr;
	register volatile BCB *bcbp;
	register struct isa_dev *ii;
	register struct cnp_softc *es;
	register int running;
	register did_receive = 0;
	register int found = 0;

	ack_bia_interrupt();
	outb(NBCR_PORT, NUSMINT | LBXENBL);
	outb(NBCR_PORT, NUSMINT | LBXENBL | LBXENBINT);

	/*
	 * If this gets called as a real interrupt, unit is set to the
	 * vector number, in this case 13 (see pic_ipsc.c).
	 */
	if (unit > 0) {
		unit = 0;
	}

	es = &cnp_softc[unit];
	ii = &cnpinfo[unit];
	if (ii->dev_alive == 0)
		return(found);

	addr = (CNPDEVICE *)ii->dev_addr;
	running = ((es->es_if.if_flags & (IFF_UP|IFF_RUNNING))
		   == (IFF_UP|IFF_RUNNING));
	/* process non-DMA receive ring */
	while (bcbp = (BCB *)ringget(&addr->cnp_tohost)) {
		found = 1;
		if (running)
			cnpread(es, bcbp);
		ringput(&addr->cnp_cnpfree, bcbp);
		did_receive = 1;
	}

	/* inspect DMA receive ring */
	if (! ringempty(&addr->cnp_rcvdma)) {
		panic("cnp%d: DMA receive ring not empty\n", unit);
	}
	if (!running)
		return(found);

	while ((bcbp = (BCB *)ringget(&addr->cnp_hostfree)) != 0) {
		found += 256;
		ringput(&es->es_free, bcbp);
	}

	return (found);
}

/*
 * receive packet from interface
 */
cnpread(es, bcbp)
	struct cnp_softc *es;
	register volatile BCB *bcbp;
{
	int len;
	char *buf;
	struct ifnet *ifp = &es->es_if;
	ipc_kmsg_t new_kmsg;
	struct ether_header *ehp;
	struct packet_header *pkt;

	ifp->if_ipackets++;

	/*
	 * Get input data length.
	 * Get pointer to ethernet header (in input buffer).
	 */
	if (bcbp->b_stat & BCB_ERR) {
		ifp->if_ierrors++;
		return;
	}
	
	new_kmsg = net_kmsg_get();
	if (new_kmsg == IKM_NULL) {
		ifp->if_rcvdrops++;
		return;
	}
	len = ntohs(bcbp->b_msglen);
	buf = (char *) ntohl(bcbp->b_addr);
	ehp = (struct ether_header *)
	    (&net_kmsg(new_kmsg)->header[0]);
	pkt = (struct packet_header *)
	    (&net_kmsg(new_kmsg)->packet[0]);
	
	/*
	 * Get header
	 */
	bcopy(buf, ehp, sizeof(struct ether_header));
#if TRACEPKT
	cnp_tracepkt(0, ehp);
#endif TRACEPKT

	/*
	 * Get body
	 */
	bcopy(buf + sizeof(struct ether_header),
	      (char *)(pkt + 1),
	      len - sizeof(struct ether_header));
	pkt->type = ehp->ether_type;
	pkt->length = len - sizeof(struct ether_header)
	    + sizeof(struct packet_header);
	
	/*
	 * Hand the packet to the network module.
	 */
	net_packet(ifp, new_kmsg, pkt->length, ethernet_priority(new_kmsg));
}

cnptimeout(unit)
int unit;
{
	int s;

	s = splimp();
	if (cnpintr(unit)) {
		cnp_softc[unit].es_lostintrs++;
	}
	timeout(cnptimeout, unit, cmc_hz);
	splx(s);
}

cnppoll()
{

	return;
}

cnpstart(unit)
	int unit;
{
	io_req_t m;
	register struct cnp_softc *es = &cnp_softc[unit];
	register struct ifnet *ifp = &es->es_if;

	for (;;) {
		IF_DEQUEUE(&ifp->if_snd, m);
		if (m)
			cnpput(unit, m, es);
		else
			return;
	}

}

cnpput(unit, m, es)
	int unit;
	io_req_t m;
	struct cnp_softc *es;
{
	register volatile BCB *bcbp;
	register volatile CNPDEVICE *addr;
	struct ifnet *ifp = &es->es_if;

	addr = (CNPDEVICE *)cnpinfo[unit].dev_addr;
	if (ringempty(&es->es_free)) {
		iodone(m);
		return 1;
	}
	bcbp = (BCB *)ringget(&es->es_free);
	if (bcbp == 0) {
		iodone(m);
		return 1;
	}
	if (ntohs(bcbp->b_len) < m->io_count) {
		printf("cnp%d: bcb too small for %d\n", unit, m->io_count);
		iodone(m);
		return 1;
	}
#if TRACEPKT
	cnp_tracepkt(1, m->io_data);
#endif TRACEPKT
	bcopy(m->io_data, ntohl(bcbp->b_addr), m->io_count);
	bcbp->b_link = htonl(0);
	if (m->io_count < MINPKTSIZE) {
		bcbp->b_len = htons(MINPKTSIZE);
	} else {
		bcbp->b_len = htons(m->io_count);
	}
	ringput(&addr->cnp_tocnp, bcbp);

	ifp->if_opackets++;

	iodone(m);
	return (0);
}

cnpoutput(dev, ior)
	dev_t	dev;
	io_req_t	ior;
{
	register int	unit = minor(dev);

	if (unit < 0 || unit >= NCNP || cnp_softc[unit].es_base == 0)
	    return (ENXIO);

	return (net_write(&cnp_softc[unit].es_if, cnpstart, ior));
}

cnpsetinput(dev, receive_port, priority, filter, filter_count)
	dev_t		dev;
	mach_port_t	receive_port;
	int		priority;
	filter_t	filter[];
	unsigned int	filter_count;
{
	register int unit = minor(dev);

	if (unit < 0 || unit >= NCNP || cnp_softc[unit].es_base == 0)
	    return (ENXIO);

	return (net_set_filter(&cnp_softc[unit].es_if,
			receive_port, priority,
			filter, filter_count));
}

/*ARGSUSED*/
cnpgetstat(dev, flavor, status, count)
	dev_t	dev;
	int	flavor;
	dev_status_t	status;		/* pointer to OUT array */
	unsigned int	*count;		/* out */
{
	register int	unit = minor(dev);

	if (unit < 0 || unit >= NCNP || cnp_softc[unit].es_base == 0)
	    return (ENXIO);

	return (net_getstat(&cnp_softc[unit].es_if,
			    flavor,
			    status,
			    count));
}

cnpsetstat(dev, flavor, status, count)
	dev_t	dev;
	int	flavor;
	dev_status_t	status;
	unsigned int	count;
{
	register int	unit = minor(dev);
	register struct cnp_softc *es;

	if (unit < 0 || unit >= NCNP || cnp_softc[unit].es_base == 0)
	    return (ENXIO);

	es = &cnp_softc[unit];

	switch (flavor) {
	    case NET_STATUS:
	    {
		/*
		 * All we can change are flags, and not many of those.
		 */
		register struct net_status *ns = (struct net_status *)status;
		int	mode = 0;

		if (count < NET_STATUS_COUNT)
		    return (D_INVALID_SIZE);

		if (ns->flags & IFF_ALLMULTI)
		    mode |= MOD_ENAL;
		if (ns->flags & IFF_PROMISC)
		    mode |= MOD_PROM;

		/*
		 * Force a compilete reset if the receive mode changes
		 * so that these take effect immediately.
		 */
		if (es->es_mode != mode) {
		    es->es_mode = mode;
		    if (es->es_flags & DSF_RUNNING) {
			es->es_flags &= ~(DSF_LOCK | DSF_RUNNING);
			cnpinit(unit);
		    }
		}
		break;
	    }
	    case NET_ADDRESS:
	    {
		/*
		 * XXX The hardware manual indicates that this is doable
		 * XXX but we'll worry about it later
		 */
		return (D_INVALID_OPERATION);	/* XXX */
	    }

	    default:
		return (D_INVALID_OPERATION);
	}
	return (D_SUCCESS);
}

/* 
 * routines to synchronize CNP and host 
 */

static
ringinit(rp, size)
	register volatile RING *rp;
{
	rp->r_rdidx = rp->r_wrtidx = ntohs(0);
	rp->r_size = ntohs(size);
}

static
ringempty(rp)
	register volatile RING *rp;
{
	return (htons(rp->r_rdidx) == htons(rp->r_wrtidx));
}

static
ringfull(rp)
	register volatile RING *rp;
{
	register short idx;

	idx = (ntohs(rp->r_wrtidx) + 1) & (ntohs(rp->r_size) - 1);
	return (idx == ntohs(rp->r_rdidx));
}

static
ringput(rp, v)
	register volatile RING *rp;
	unsigned int v;
{
	register int idx;

	idx = (ntohs(rp->r_wrtidx) + 1) & (ntohs(rp->r_size) - 1);
	if (idx != ntohs(rp->r_rdidx)) {
		rp->r_slot[ntohs(rp->r_wrtidx)] = htonl(v);
		rp->r_wrtidx = htons(idx);
		if ((idx -= (short) ntohs(rp->r_rdidx)) < 0)
			idx += htons(rp->r_size);
		return idx;			/* num ring entries */
	}
	return (0);
}

static
ringget(rp)
	register volatile RING *rp;
{
	register int i = 0;

	if (ntohs(rp->r_rdidx) != ntohs(rp->r_wrtidx)) {
		i = ntohl(rp->r_slot[ntohs(rp->r_rdidx)]);
		rp->r_rdidx =
		    htons((ntohs(rp->r_rdidx) + 1) & (ntohs(rp->r_size) - 1));
	}
	return i;
}

static
ringfirst(rp)
	register volatile RING *rp;
{
	register int i;

	if (ntohs(rp->r_rdidx) != ntohs(rp->r_wrtidx)) {
		i = ntohl(rp->r_slot[ntohs(rp->r_rdidx)]);
		return i;
	} else {
		return 0;
	}
}

static int delay(time) 
	volatile long time;
{
	while (time--);
}

word16	bia_ctrl_reg;	/* Since CONTROL reg is write only, save it here */

struct bia *bia;

enable_bia_interrupt()
{
	bia_ctrl_reg |= INT_ENBL;    /* Enable PBX interrupt from BIA */
	bia->bia_control = bia_ctrl_reg;
	bia->bia_imr = IMR_NO_MASK;
}

disable_bia_interrupt()
{
	unsigned long enable_flag;
	
	enable_flag = bia_ctrl_reg & INT_ENBL;
	bia_ctrl_reg &= ~INT_ENBL;    /* Disable PBX interrupt from BIA */
	bia->bia_control = bia_ctrl_reg;
	bia->bia_imr = IMR_MASKALL;
	return(enable_flag);
}

unsigned short cnp_intvec[8];
long cnp_berr_count = 0;
long cnp_sysfail_count = 0;
long cnp_to_count = 0;

ack_bia_interrupt()
{
	long	int_level;
	unsigned long isr;

	isr = ~(bia->bia_isr);

	bia_ctrl_reg &= ~(SHFL1 | SHFL0);
	bia_ctrl_reg |= MODE_16_BIT;
	bia_ctrl_reg |= VME_IACK;
	bia->bia_control = bia_ctrl_reg;

	if (isr & ISR_BERR_) {
		printf("VME bus error\n");
		bia->bia_clr_int = 0;
		cnp_berr_count++;
	}
	if (isr & ISR_SYSFAIL_) {
		printf("VME sysfail error\n");
		bia->bia_clr_int = 0;
		cnp_sysfail_count++;
	}
	if (isr & ISR_BIA_TO_) {
		printf("VME timeout error\n");
		cnp_to_count++;
	}
	for (int_level = 1; int_level <= 8; int_level++) {
		if ( (isr & (1 << (int_level-1))) != 0 ) {
			cnp_intvec[int_level-1] = *(unsigned short *)
						(&bia->bia_vme[2*int_level]);
		}
	}

	bia_ctrl_reg &= ~(SHFL1 | SHFL0);
	bia_ctrl_reg |= MODE_ARRAY;
	bia_ctrl_reg &= ~VME_IACK;
	bia->bia_control = bia_ctrl_reg;

}

/*
 * Set bia vme window for given vme address;
 * return current kernel virtual address for the given vme address.
 */
unsigned long make_vme_pointer(phaddr)
	unsigned long phaddr;
{
	bia->bia_pag_mod &= MOD_MASK;
	bia->bia_pag_mod |= (phaddr & PAG_MASK);
	return (unsigned long) &bia->bia_vme[phaddr & ~PAG_MASK];
}

#define ENP_VADDR       0xe00000		/* ENP virtual address */
unsigned long export_enpaddr;

biaprobe()
{
	int bia_found = 0;

	outb(NBCR_PORT, NUSMINT | LBXENBL | LBXENBINT);

	bia = (struct bia *) map_phys(BIA_VME, sizeof(*bia));
	
	/*
	 * Set up the BIA CONTROL register
	 */
	bia_ctrl_reg = RESET_CTRL; /* Save CONTROL register (not readable) */
	bia->bia_control = bia_ctrl_reg;
	
	/*
	 * Set up the BIA interrupt mask register
	 */
	bia->bia_imr = IMR_MASKALL;
	
	/*
	 * Clear the BIA interrupt status register
	 */
	bia->bia_clr_int = 0;
	
	/*
	 * Clear the BIA page address register
	 * Page address set to 0, modifier is A24 supervisory data access
	 */
	bia->bia_pag_mod = STD_SUP_DATA;

	disable_bia_interrupt();

	/* TEST for presence of BIA board by reading loopback */

	bia_ctrl_reg |= (TEST_NET | MODE_32_BIT );
	bia->bia_control = bia_ctrl_reg;
		
	bia->bia_pag_mod = 0xa0a00505;	  /* a Unique pattern */
	bia_found = (bia->bia_swap_reg == 0xa0a00505);

	bia_ctrl_reg &= ~TEST_NET;
	bia->bia_control = bia_ctrl_reg;

	bia->bia_pag_mod = STD_SUP_DATA;

	if (!bia_found) {
		return 0;
	}

	enable_bia_interrupt();

	bia_ctrl_reg &= ~(SHFL1 | SHFL0);
	bia_ctrl_reg |= MODE_ARRAY;
	bia->bia_control = bia_ctrl_reg;

	export_enpaddr = (unsigned long) make_vme_pointer(ENP_VADDR);
	bia->bia_clr_int = 0;
	enable_bia_interrupt();
	return 1;
}

#if TRACEPKT

extern unsigned long hardclock_ticks;

#define TRACESIZE 32

struct traceentry {
	unsigned long	timestamp;
	char		sr;
	char		fill;
	char		eaddr[6];
	char		junk[4];
} tracebuf[TRACESIZE];

int traceidx = 0;

cnp_tracepkt(sr, pkt)
	int	sr;
	char	*pkt;
{

	tracebuf[traceidx].timestamp = hardclock_ticks;
	tracebuf[traceidx].sr = sr;
	if (sr) {
		bcopy(pkt, tracebuf[traceidx].eaddr, 6);
	} else {
		bcopy(&pkt[6], tracebuf[traceidx].eaddr, 6);
	}
	traceidx++;
	if (traceidx == TRACESIZE)
		traceidx = 0;
}
#endif TRACEPKT

#endif /* NCNP > 0 */
