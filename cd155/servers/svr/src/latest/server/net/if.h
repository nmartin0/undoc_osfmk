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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: if.h,v $
 * Revision 1.10  1995/02/01  21:32:50  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.9  1994/11/18  20:33:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/07/14  18:07:02  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/27  22:34:53  hobbes
 * Changed if_mtu from short to unsigned short,
 * HIPPI will want to send 64k IP packets.
 *
 * Revision 1.6  1993/05/17  19:06:23  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.5  1993/05/06  18:59:27  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/14  15:13:22  cfj
 * Merge with T9.5
 *
 * Revision 1.2.8.2  1993/04/14  15:03:20  cfj
 * Remove defined(__i860__) where not needed.
 *
 * Revision 1.3  1993/03/25  23:22:57  cfj
 * T9 Merge.
 *
 * Revision 1.2.8.1  1993/03/24  23:40:11  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.1.1  1993/05/03  17:32:23  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.7  93/05/07  16:59:07  yazz
 * Fixed RCS comments.
 * 
 * Revision 2.6  93/05/07  15:06:16  nina
 * Fixed RCS comments.
 * 
 * Revision 2.5  93/05/05  22:19:12  mjl
 * Add IFF_INVISIBLE flag, used to hide MI interfaces.
 * Add #ifndef _NET_IF_H_ .
 * 
 * Revision 2.4  1993/04/12  15:43:26  nina
 * Include if_dvnode in ifnet block only #ifdef TNC
 *
 * Revision 2.3  93/02/25  17:48:20  nina
 * Modified to support new network interface naming scheme.  Include device
 * node in ifnet structure.
 * 
 * Revision 2.2  91/08/31  13:38:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:32:41  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10.5.2  91/03/15  17:47:27  tmt
 * 	Fix comment at if_output (4 parms not 3).
 * 	[91/03/13  19:04:35  tmt]
 * 
 * Revision 1.10  90/10/07  14:31:28  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:09:37  gm]
 * 
 * Revision 1.9  90/09/23  15:55:13  devrcs
 * 	Add IFF_SIMPLEX to IFF_CANTCHANGE.
 * 	[90/09/15  15:33:01  tmt]
 * 
 * Revision 1.8  90/07/27  08:58:37  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:24:19  tmt]
 * 
 * Revision 1.7  90/04/27  19:09:21  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:23:12  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:11  devrcs
 * 	Move declarations out, use _KERNEL
 * 	[90/04/09  16:06:14  tmt]
 * 
 * Revision 1.5  90/02/05  15:49:45  robert
 * 	Rearrange ifnet to put locks last. Hide locks from non-kernel.
 * 	Remove rawintrq (unused).
 * 	[90/01/19  14:40:24  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:12  gm
 * 	#ifdefs for user code compatability.
 * 	[90/01/16  13:41:26  tmt]
 * 
 * 	Put statistics lock under KERNEL && LOCK_NETSTATS.
 * 	Temporary compat for the ifqueue, to be fixed later.
 * 	[90/01/12  17:13:53  tmt]
 * 
 * 	Compat for including necessary header files in user code.
 * 	Make structure overlay constant regardless of config options.
 * 	Add IFF_D* flags and IFF_SNAP for RFC1042.
 * 	[90/01/08  15:49:37  tmt]
 * 
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
 * Revision 1.3  90/01/03  12:40:39  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:37:49  gm]
 * 
 * Revision 1.2  89/12/26  09:43:41  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	Base:	if.h	7.6 (Berkeley) 9/20/89
 *	Merged: if.h	7.9 (Berkeley) 6/28/90
 */

#ifndef	_NET_IF_H_
#define	_NET_IF_H_

/*
 * Structures defining a network interface, providing a packet
 * transport mechanism (ala level 0 of the PUP protocols).
 *
 * Each interface accepts output datagrams of a specified maximum
 * length, and provides higher level routines with input datagrams
 * received from its medium.
 *
 * Output occurs when the routine if_output is called, with four parameters:
 *	(*ifp->if_output)(ifp, m, dst, ro)
 * Here m is the mbuf chain to be sent and dst is the destination address.
 * The output routine encapsulates the supplied datagram if necessary,
 * and then transmits it on its medium.
 *
 * On input, each interface unwraps the data received by it, and either
 * places it on the input queue of a internetwork datagram routine
 * and posts the associated software interrupt, or passes the datagram to a raw
 * packet input routine.
 *
 * Routines exist for locating interfaces by their addresses
 * or for locating a interface on a certain network, as well as more general
 * routing and gateway routines maintaining information used to locate
 * interfaces.  These routines live in the files if.c and route.c
 */

#if	defined(_KERNEL) && !defined(_NET_GLOBALS_H_)
#include "net/net_globals.h"
#endif

#ifndef	_TIME_
#ifdef	_KERNEL
#include "sys/time.h"
#else
#include <sys/time.h>
#endif
#endif

/*
 * Structure defining a queue for a network interface.
 */

struct ifnet {
	char	*if_name;		/* name, e.g. ``en'' or ``lo'' */
	short	if_unit;		/* sub-unit for lower level driver */
	u_short	if_mtu;			/* maximum transmission unit */
	short	if_flags;		/* up/down, broadcast, etc. */
	short	if_timer;		/* time 'til if_watchdog called */
	int	if_metric;		/* routing metric (external only) */
	struct	ifaddr *if_addrlist;	/* linked list of addresses per if */
/* procedure handles */
	int	(*if_init)();		/* init routine */
	int	(*if_output)();		/* output routine (enqueue) */
	int	(*if_start)();		/* initiate output routine */
	int	(*if_done)();		/* output complete routine */
	int	(*if_ioctl)();		/* ioctl routine */
	int	(*if_reset)();		/* bus reset routine */
	int	(*if_watchdog)();	/* timer routine */
/* generic interface statistics */
	int	if_ipackets;		/* packets received on interface */
	int	if_ierrors;		/* input errors on interface */
	int	if_opackets;		/* packets sent on interface */
	int	if_oerrors;		/* output errors on interface */
	int	if_collisions;		/* collisions on csma interfaces */
/* SNMP statistics */
	struct	timeval if_lastchange;	/* last updated */
	int	if_ibytes;		/* total number of octets received */
	int	if_obytes;		/* total number of octets sent */
	int	if_imcasts;		/* packets received via multicast */
	int	if_omcasts;		/* packets sent via multicast */
	int	if_iqdrops;		/* dropped on input, this interface */
	int	if_noproto;		/* destined for unsupported protocol */
	int	if_baudrate;		/* linespeed */
/* end statistics */
	struct	ifnet *if_next;
	u_char	if_type;		/* ethernet, tokenring, etc */
	u_char	if_addrlen;		/* media address length */
	u_char	if_hdrlen;		/* media header length */
	u_char	if_index;		/* numeric abbreviation for this if  */
	struct	ifqueue {
		struct	mbuf *ifq_head;
		struct	mbuf *ifq_tail;
		int	ifq_len;
		int	ifq_maxlen;
		int	ifq_drops;
#if	!defined(_KERNEL) || !NETSYNC_LOCK
	} if_snd;			/* output queue */
#else	/* Locks go last to make structure appear to have constant size */
		simple_lock_data_t	ifq_slock;
	} if_snd;			/* output queue */
#ifdef	LOCK_NETSTATS
	simple_lock_data_t	if_slock;	/* statistics lock */
#endif
#endif
#if defined(TNC)
	node_t	if_dvnode;			/* device node */
#endif
};

#define	IFF_UP		0x1		/* interface is up */
#define	IFF_BROADCAST	0x2		/* broadcast address valid */
#define	IFF_DEBUG	0x4		/* turn on debugging */
#define	IFF_LOOPBACK	0x8		/* is a loopback net */
#define	IFF_POINTOPOINT	0x10		/* interface is point-to-point link */
#define	IFF_NOTRAILERS	0x20		/* avoid use of trailers */
#define	IFF_RUNNING	0x40		/* resources allocated */
#define	IFF_NOARP	0x80		/* no address resolution protocol */
#define	IFF_PROMISC	0x100		/* receive all packets */
#define	IFF_ALLMULTI	0x200		/* receive all multicast packets */
#define	IFF_OACTIVE	0x400		/* transmission in progress */
#define	IFF_SIMPLEX	0x800		/* can't hear own transmissions */
#ifdef	TNC
#define IFF_INVISIBLE	0x1000		/* can't be seen by SIOCGIFCONF */
#endif	/* TNC */

/* Device-specific flags */
#define IFF_D1		0x8000
#define IFF_D2		0x4000
#define IFF_D3		0x2000
#ifndef	TNC
#define IFF_D4		0x1000
#endif	/* ! TNC */
#define IFF_SNAP	IFF_D1		/* Ethernet driver outputs SNAP hdr */

/* flags set internally only: */
#define	CANTCHANGE	\
	(IFF_BROADCAST|IFF_POINTOPOINT|IFF_SIMPLEX|IFF_RUNNING|IFF_OACTIVE)

#ifdef	TNC
#define	IFF_CANTCHANGE	(CANTCHANGE|IFF_INVISIBLE)
#else
#define	IFF_CANTCHANGE	CANTCHANGE
#endif

#if	NETSYNC_LOCK
/*
 * Multiprocessor queue locking.
 *
 * Note that the IF_QFULL and IF_DROP macros become racy in an mp environment.
 * The exact number of ifq_drops probably isn't important; on the other hand,
 * it is possible that an unlocked ifq could grow larger than its declared
 * ifq_maxlen as processors race between IF_QFULL and IF_ENQUEUE.  However,
 * it is still ABSOLUTELY NECESSARY that modification of ifq_len be locked!
 *
 * Note also that any macros which take the IFQ_LOCK must be done at splimp.
 *
 */
#define IFQ_LOCKINIT(ifq)	simple_lock_init(&((ifq)->ifq_slock))
#define IFQ_LOCK(ifq)		simple_lock(&((ifq)->ifq_slock))
#define IFQ_UNLOCK(ifq)		simple_unlock(&((ifq)->ifq_slock))
#else
#define IFQ_LOCKINIT(ifq)
#define IFQ_LOCK(ifq)
#define IFQ_UNLOCK(ifq)
#endif

/*
 * Output queues (ifp->if_snd) and internetwork datagram level (pup level 1)
 * input routines have queues of messages stored on ifqueue structures
 * (defined above).  Entries are added to and deleted from these structures
 * by these macros, which should be called with ipl raised to splimp().
 */
#define	IF_QFULL(ifq)		((ifq)->ifq_len >= (ifq)->ifq_maxlen)
#define	IF_DROP(ifq)		((ifq)->ifq_drops++)
#define	IF_ENQUEUE_NOLOCK(ifq, m) { \
	(m)->m_nextpkt = 0; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_head = m; \
	else \
		(ifq)->ifq_tail->m_nextpkt = m; \
	(ifq)->ifq_tail = m; \
	(ifq)->ifq_len++; \
}
#define	IF_ENQUEUE(ifq, m) { \
	IFQ_LOCK(ifq); \
	IF_ENQUEUE_NOLOCK(ifq, m); \
	IFQ_UNLOCK(ifq); \
}
#define	IF_PREPEND_NOLOCK(ifq, m) { \
	(m)->m_nextpkt = (ifq)->ifq_head; \
	if ((ifq)->ifq_tail == 0) \
		(ifq)->ifq_tail = (m); \
	(ifq)->ifq_head = (m); \
	(ifq)->ifq_len++; \
}
#define	IF_PREPEND(ifq, m) { \
	IFQ_LOCK(ifq); \
	IF_PREPEND_NOLOCK(ifq, m); \
	IFQ_UNLOCK(ifq); \
}
#define	IF_DEQUEUE_NOLOCK(ifq, m) { \
	(m) = (ifq)->ifq_head; \
	if (m) { \
		if (((ifq)->ifq_head = (m)->m_nextpkt) == 0) \
			(ifq)->ifq_tail = 0; \
		(m)->m_nextpkt = 0; \
		(ifq)->ifq_len--; \
	} \
}
#define	IF_DEQUEUE(ifq, m) { \
	IFQ_LOCK(ifq); \
	IF_DEQUEUE_NOLOCK(ifq, m); \
	IFQ_UNLOCK(ifq); \
}

#define	IFQ_MAXLEN	50
#define	IFNET_SLOWHZ	1		/* granularity is 1 second */

/*
 * The ifaddr structure contains information about one address
 * of an interface.  They are maintained by the different address families,
 * are allocated and attached when an address is set, and are linked
 * together so all addresses for an interface can be located.
 */
struct ifaddr {
	struct	sockaddr *ifa_addr;	/* address of interface */
	struct	sockaddr *ifa_dstaddr;	/* other end of p-to-p link */
#define	ifa_broadaddr	ifa_dstaddr	/* broadcast address interface */
	struct	sockaddr *ifa_netmask;	/* used to determine subnet */
	struct	ifnet *ifa_ifp;		/* back-pointer to interface */
	struct	ifaddr *ifa_next;	/* next address for interface */
	void	(*ifa_rtrequest)();	/* check or clean routes (+ or -)'d */
	struct	rtentry *ifa_rt;	/* ??? for ROUTETOIF */
	u_short	ifa_flags;		/* mostly rt_flags for cloning */
	u_short	ifa_llinfolen;		/* extra to malloc for link info */
};
#define IFA_ROUTE	RTF_UP		/* route installed */
/*
 * Interface request structure used for socket
 * ioctl's.  All interface ioctl's must have parameter
 * definitions which begin with ifr_name.  The
 * remainder may be interface specific.
 */
struct	ifreq {
#define	IFNAMSIZ	16
	char	ifr_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	union {
		struct	sockaddr ifru_addr;
		struct	sockaddr ifru_dstaddr;
		struct	sockaddr ifru_broadaddr;
		short	ifru_flags;
		int	ifru_metric;
		caddr_t	ifru_data;
	} ifr_ifru;
#define	ifr_addr	ifr_ifru.ifru_addr	/* address */
#define	ifr_dstaddr	ifr_ifru.ifru_dstaddr	/* other end of p-to-p link */
#define	ifr_broadaddr	ifr_ifru.ifru_broadaddr	/* broadcast address */
#define	ifr_flags	ifr_ifru.ifru_flags	/* flags */
#define	ifr_metric	ifr_ifru.ifru_metric	/* metric */
#define	ifr_data	ifr_ifru.ifru_data	/* for use by interface */
};

struct ifaliasreq {
	char	ifra_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	sockaddr ifra_addr;
	struct	sockaddr ifra_broadaddr;
	struct	sockaddr ifra_mask;
};

/*
 * Structure used in SIOCGIFCONF request.
 * Used to retrieve interface configuration
 * for machine (useful for programs which
 * must know all networks accessible).
 */
struct	ifconf {
	int	ifc_len;		/* size of associated buffer */
	union {
		caddr_t	ifcu_buf;
		struct	ifreq *ifcu_req;
	} ifc_ifcu;
#define	ifc_buf	ifc_ifcu.ifcu_buf	/* buffer address */
#define	ifc_req	ifc_ifcu.ifcu_req	/* array of structures returned */
};

#ifdef	_KERNEL
#include "net/if_arp.h"
extern struct	ifnet *ifnet;
#else
#include <net/if_arp.h>
#endif

#endif	/* ! _NET_IF_H_ */
