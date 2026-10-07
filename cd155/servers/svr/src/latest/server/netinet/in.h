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
 * $Log: in.h,v $
 * Revision 1.5  1994/11/18  20:34:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/05/06  20:25:58  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/02/27  00:34:49  dleslie
 * Added define of CONST if not defined, so this file will compile on a
 * Paragon.
 *
 * Revision 1.2  1992/11/30  22:28:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:26:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:36  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1993/02/02  15:23:07  rabii
 * 	Remove CONST from definition of protosw so afs can substitute it's
 * 	vectors (rabii)
 *
 * Revision 2.2  91/08/31  13:42:52  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:35:13  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.4.2  91/03/15  17:48:49  tmt
 * 	Update extern declaration of inetsw.
 * 	[91/03/13  19:08:46  tmt]
 * 
 * Revision 1.9  90/10/07  14:34:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:13:50  gm]
 * 
 * Revision 1.8  90/07/27  09:00:42  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:28:06  tmt]
 * 
 * Revision 1.7  90/04/27  19:15:05  devrcs
 * 	Use _SOCKADDR_LEN. Delete function declarations.
 * 	[90/04/20  12:47:54  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:01  gm
 * 	Change #ifdef controlling new sockaddr format.
 * 	[90/03/07  12:19:05  tmt]
 * 
 * Revision 1.5  90/01/18  08:44:53  gm
 * 	Make file re-entrant.
 * 	[90/01/08  16:02:20  tmt]
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
 * Revision 1.4  90/01/03  12:41:29  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:41  gm]
 * 
 * Revision 1.3  89/12/26  10:10:05  gm
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
 *	Base:	in.h	7.8 (Berkeley) 9/13/89
 *	Merged:	in.h	7.10 (Berkeley) 6/28/90
 */

#ifndef	_NETINET_IN_H_
#define	_NETINET_IN_H_

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981.
 */

/*
 * Protocols
 */
#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP		1		/* control message protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define	IPPROTO_EON		80		/* ISO cnlp */

#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256


/*
 * Local port number conventions:
 * Ports < IPPORT_RESERVED are reserved for
 * privileged processes (e.g. root).
 * Ports > IPPORT_USERRESERVED are reserved
 * for servers, not necessarily privileged.
 */
#define	IPPORT_RESERVED		1024
#define	IPPORT_USERRESERVED	5000

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr {
	u_long s_addr;
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define	IN_CLASSA(i)		(((long)(i) & 0x80000000) == 0)
#define	IN_CLASSA_NET		0xff000000
#define	IN_CLASSA_NSHIFT	24
#define	IN_CLASSA_HOST		0x00ffffff
#define	IN_CLASSA_MAX		128

#define	IN_CLASSB(i)		(((long)(i) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET		0xffff0000
#define	IN_CLASSB_NSHIFT	16
#define	IN_CLASSB_HOST		0x0000ffff
#define	IN_CLASSB_MAX		65536

#define	IN_CLASSC(i)		(((long)(i) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET		0xffffff00
#define	IN_CLASSC_NSHIFT	8
#define	IN_CLASSC_HOST		0x000000ff

#define	IN_CLASSD(i)		(((long)(i) & 0xf0000000) == 0xe0000000)
#define	IN_MULTICAST(i)		IN_CLASSD(i)

#define	IN_EXPERIMENTAL(i)	(((long)(i) & 0xe0000000) == 0xe0000000)
#define	IN_BADCLASS(i)		(((long)(i) & 0xf0000000) == 0xf0000000)

#define	INADDR_ANY		(u_long)0x00000000
#define	INADDR_BROADCAST	(u_long)0xffffffff	/* must be masked */
#ifndef _KERNEL
#define	INADDR_NONE		0xffffffff		/* -1 return */
#endif

#define	IN_LOOPBACKNET		127			/* official! */

/*
 * Socket address, internet style.
 */
#if	defined(_SOCKADDR_LEN) || defined(_KERNEL)
struct sockaddr_in {
	u_char	sin_len;
	u_char	sin_family;
	u_short	sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};
#else
struct sockaddr_in {
	u_short	sin_family;
	u_short	sin_port;
	struct	in_addr sin_addr;
	char	sin_zero[8];
};
#endif

/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */
struct ip_opts {
	struct	in_addr ip_dst;		/* first hop, 0 w/o src rt */
	char	ip_opts[40];		/* actually variable in size */
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define	IP_OPTIONS	1	/* buf/ip_opts; set/get IP per-packet options */
#define	IP_HDRINCL	2	/* int; header is included with data (raw) */
#define	IP_TOS		3	/* int; IP type of service and precedence */
#define	IP_TTL		4	/* int; IP time to live */
#define	IP_RECVOPTS	5	/* bool; receive all IP options w/datagram */
#define	IP_RECVRETOPTS	6	/* bool; receive IP options for response */
#define	IP_RECVDSTADDR	7	/* bool; receive IP dst addr w/datagram */
#define	IP_RETOPTS	8	/* ip_opts; set/get IP per-packet options */

#ifdef _KERNEL
extern	struct domain inetdomain;
extern	struct protosw inetsw[];
extern	int inetprintfs;
extern	int ipgateway, ipforwarding;
#endif
#endif
