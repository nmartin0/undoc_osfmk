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
 * $Log: tcp.h,v $
 * Revision 1.5  1994/11/18  20:35:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/05/06  22:19:15  shala
 * Include machine/endian.h for defining BYTE_ORDER.
 *
 * Revision 1.3  1993/05/06  20:26:55  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:41  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:34  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  22:45:40  hobbes
 * added RFC_1323 extensions
 *
 * Revision 2.2  1991/08/31  13:44:05  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:30  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:35:27  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:52  gm]
 * 
 * Revision 1.6  90/07/27  09:01:41  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:36:01  tmt]
 * 
 * Revision 1.5  90/04/27  19:18:50  devrcs
 * 	Use _KERNEL, _NO_BITFIELDS.
 * 	[90/04/20  12:59:23  tmt]
 * 
 * Revision 1.4  90/01/18  08:47:28  gm
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
 * Revision 1.3  90/01/03  12:41:52  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:08  gm]
 * 
 * Revision 1.2  89/12/26  10:14:08  gm
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
 *	Base:	tcp.h	7.5 (Berkeley) 6/29/88
 *	Merged:	tcp.h	7.7 (Berkeley) 6/28/90
 */

#include <machine/endian.h>

typedef	u_long	tcp_seq;
/*
 * TCP header.
 * Per RFC 793, September, 1981.
 */
struct tcphdr {
	u_short	th_sport;		/* source port */
	u_short	th_dport;		/* destination port */
	tcp_seq	th_seq;			/* sequence number */
	tcp_seq	th_ack;			/* acknowledgement number */
#if	defined(_KERNEL) || defined(_NO_BITFIELDS)
	u_char	th_xoff;
#else
#if BYTE_ORDER == LITTLE_ENDIAN 
	u_char	th_x2:4,		/* (unused) */
		th_off:4;		/* data offset */
#endif
#if BYTE_ORDER == BIG_ENDIAN 
	u_char	th_off:4,		/* data offset */
		th_x2:4;		/* (unused) */
#endif
#endif
	u_char	th_flags;
#define	TH_FIN	0x01
#define	TH_SYN	0x02
#define	TH_RST	0x04
#define	TH_PUSH	0x08
#define	TH_ACK	0x10
#define	TH_URG	0x20
	u_short	th_win;			/* window */
	u_short	th_sum;			/* checksum */
	u_short	th_urp;			/* urgent pointer */
};

#define	TCPOPT_EOL	0
#define	TCPOPT_NOP	1
#define	TCPOPT_MAXSEG	2

#ifdef RFC_1323
#define TCPOPT_WINDOW		3	/* rfc 1323 options */
#define TCPOPT_SACK_PERMITTED	4
#define TCPOPT_SACK		5
#define TCPOPT_TIMESTAMP	8
#endif /* RFC_1323 */

/*
 * Default maximum segment size for TCP.
 * With an IP MSS of 576, this is 536,
 * but 512 is probably more convenient.
 * This should be defined as MIN(512, IP_MSS - sizeof (struct tcpiphdr)).
 */
#define	TCP_MSS	512

#define	TCP_MAXWIN	65535		/* largest value for (unscaled) window */

#ifdef RFC_1323
#define TCP_MAX_WINSHIFT	14	/* maximum window shift */
#endif /* RFC_1323 */

/*
 * User-settable options (used with setsockopt).
 */
#define	TCP_NODELAY	0x01	/* don't delay send to coalesce packets */
#define	TCP_MAXSEG	0x02	/* set maximum segment size */
