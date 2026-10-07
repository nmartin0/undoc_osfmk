/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: if_ether.h,v $
 * Revision 1.4  1994/11/18  20:34:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:25:35  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:24  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:18  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:42:29  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:34:48  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:33:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:13:15  gm]
 * 
 * Revision 1.8  90/07/27  09:00:23  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:25:19  tmt]
 * 
 * Revision 1.7  90/06/22  20:38:55  devrcs
 * 	Add at_enaddr compat define.
 * 	[90/06/18  10:51:08  tmt]
 * 
 * 	New table and arpcom formats for enhanced ARP (netinet/if_ether.c).
 * 	[90/06/11  11:31:49  tmt]
 * 
 * Revision 1.6  90/04/27  19:14:43  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:45:36  tmt]
 * 
 * Revision 1.5  90/04/14  00:32:59  devrcs
 * 	Delete function declarations. Use _KERNEL.
 * 	[90/04/09  16:26:54  tmt]
 * 
 * Revision 1.4  90/01/18  08:44:46  gm
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/02  20:11:22  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:49:25  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	Base:	if_ether.h	7.4 (Berkeley) 2/17/89
 *	Merged:	if_ether.h	7.5 (Berkeley) 6/28/90
 */

/*
 * Structure of a 10Mb/s Ethernet header.
 */
struct	ether_header {
	u_char	ether_dhost[6];
	u_char	ether_shost[6];
	u_short	ether_type;
};

#define	ETHERTYPE_PUP	0x0200		/* PUP protocol */
#define	ETHERTYPE_IP	0x0800		/* IP protocol */
#define ETHERTYPE_ARP	0x0806		/* Addr. resolution protocol */

/*
 * The ETHERTYPE_NTRAILER packet types starting at ETHERTYPE_TRAIL have
 * (type-ETHERTYPE_TRAIL)*512 bytes of data followed
 * by an ETHER type (as given above) and then the (variable-length) header.
 */
#define	ETHERTYPE_TRAIL		0x1000		/* Trailer packet */
#define	ETHERTYPE_NTRAILER	16

#define	ETHERMTU	1500
#define	ETHERMIN	(60-14)

/*
 * Ethernet Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  Structure below is adapted
 * to resolving internet addresses.  Field names used correspond to 
 * RFC 826.
 */
struct	ether_arp {
	struct	arphdr ea_hdr;	/* fixed-size header */
	u_char	arp_sha[6];	/* sender hardware address */
	u_char	arp_spa[4];	/* sender protocol address */
	u_char	arp_tha[6];	/* target hardware address */
	u_char	arp_tpa[4];	/* target protocol address */
};
#define	arp_hrd	ea_hdr.ar_hrd
#define	arp_pro	ea_hdr.ar_pro
#define	arp_hln	ea_hdr.ar_hln
#define	arp_pln	ea_hdr.ar_pln
#define	arp_op	ea_hdr.ar_op

/*
 * Structure shared between the ethernet driver modules and
 * the address resolution code.  For example, each ec_softc or il_softc
 * begins with this structure.
 */
struct	arpcom {
	struct 	ifnet ac_if;		/* network-visible interface */
	u_char	ac_hwaddr[14];		/* hardware address (sizeof sa_data) */
	u_short	ac_arphrd;		/* arp hardware type (net/if_arp.h) */
	u_char	*ac_bcastaddr;		/* pointer to link broadcast or 0 */
	struct in_addr ac_ipaddr;	/* copy of ip address- XXX */
};
#define ac_enaddr	ac_hwaddr	/* one size fits many */

/*
 * Internet to ethernet address resolution table.
 */
struct	arptab {
	struct	in_addr at_iaddr;	/* internet address */
	struct	ifnet *at_if;		/* interface pointer */
	u_char	at_hwaddr[14];		/* hardware address (len in at_if) */
	u_short	at_flags;		/* flags (net/if_arp.h) */
	short	at_timer;		/* ticks since last referenced */
	short	at_valid;		/* ticks since last valid */
	short	at_retry;		/* request send count */
	short	at_sent;		/* at_valid at last send */
	struct	mbuf *at_hold;		/* last packet until resolved/timeout */
};
#define at_enaddr	at_hwaddr	/* as for arpcom */

#ifdef	_KERNEL
extern CONST u_char etherbroadcastaddr[6];
#endif
