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
 * $Log: in_systm.h,v $
 * Revision 1.4  1994/11/18  20:34:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:26:24  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:42  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:51  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:35:37  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:34:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:26  gm]
 * 
 * 	Add include of inet.h for dynamic loading build dependency.
 * 	[90/09/22  19:36:47  tmt]
 * 
 * Revision 1.8  90/07/27  09:01:00  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:30:49  tmt]
 * 
 * Revision 1.7  90/06/22  20:39:05  devrcs
 * 	INETPRINTFS defaults to DEBUG.
 * 	[90/06/07  16:24:50  tmt]
 * 
 * Revision 1.6  90/04/27  19:15:46  devrcs
 * 	Use _KERNEL. Rearrange.
 * 	[90/04/20  12:52:21  tmt]
 * 
 * Revision 1.5  90/03/27  13:22:32  gm
 * 	Change name of proto_netinet.h include (>14 chars!).
 * 	[90/03/16  15:43:29  tmt]
 * 
 * Revision 1.4  90/01/18  08:45:16  gm
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
 * Revision 1.3  90/01/02  20:11:30  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:11:26  gm
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
 *	Base:	in_systm.h	7.3 (Berkeley) 6/29/88
 *	Merged:	in_systm.h	7.4 (Berkeley) 6/28/90
 */

/*
 * Miscellaneous internetwork
 * definitions for kernel.
 */

/*
 * Network types.
 *
 * Internally the system keeps counters in the headers with the bytes
 * in native byte order so that machine instructions will work on them.
 * It reverses the bytes if necessary before transmission at each protocol
 * level.  The n_ types represent the types with the bytes in ``high-ender''
 * (big endian) order.
 */
typedef u_short n_short;		/* short as received from the net */
typedef u_long	n_long;			/* long as received from the net */

typedef	u_long	n_time;			/* ms since 00:00 GMT, net byte order */

#ifdef _KERNEL

#include "netinet/proto_inet.h"	/* Function prototypes */
#if	MACH
#include <inet.h>	/* Dynamic config requires dependency */
#endif

/*
 * Global configuration parameters for Internet. With the exception of
 * INETPRINTFS, these control the default value of kernel variables
 * settable by the system administrator. These variables are int's with
 * the names of the #defines, in lower case.
 */

/* GATEWAY turns on IP forwarding and expands ARP tables. */
#ifdef	GATEWAY
#define IPFORWARDING	1
#define IPGATEWAY	1
#else
#define IPFORWARDING	0
#define IPGATEWAY	0
#endif

/* DIRECTED_BROADCAST makes broadcast addresses match
 * only for their proper interface. */
#ifdef	DIRECTED_BROADCAST
#define IPDIRECTED_BROADCAST	1
#else
#define IPDIRECTED_BROADCAST	0
#endif

/* SUBNETSARELOCAL makes subnets appear as directly connected
 * for the purpose of max segsize, etc. */
#ifndef	SUBNETSARELOCAL
#define SUBNETSARELOCAL	1
#endif

/* TCP_COMPAT_42 makes TCP adjust initial send sequence numbers
 * and keepalives to make 4.2-based systems happy. It also causes
 * urgent data to behave (incorrectly) the way it always did. */
#ifndef	TCP_COMPAT_42
#define TCP_COMPAT_42	1
#endif

/* IPSENDREDIRECTS enables the sending of ICMP redirects. */
#ifndef	IPSENDREDIRECTS
#define IPSENDREDIRECTS	1
#endif

/* INETPRINTFS enables printfs for various errors and/or information. */
#ifndef	INETPRINTFS
#define INETPRINTFS	DEBUG
#endif

#endif
