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
 * $Log: ns.h,v $
 * Revision 1.5  1994/11/18  20:36:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:12:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:34:59  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:01:34  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:30:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:47  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:22:50  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:45:37  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:37:08  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:18:13  gm]
 * 
 * 	Add include of ns.h for dynamic loading build dependency.
 * 	[90/09/22  19:37:11  tmt]
 * 
 * Revision 1.8  90/09/13  11:48:23  devrcs
 * 	Fix INET configuration option test.
 * 	[90/08/28  11:23:33  tmt]
 * 
 * Revision 1.7  90/08/24  12:15:05  devrcs
 * 	Test INET option from file, not make line.
 * 	[90/08/19  14:18:35  tmt]
 * 
 * Revision 1.6  90/07/27  09:03:01  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:08:21  tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1984, 1985, 1986, 1987 Regents of the University of California.
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
 *	Base:	ns.h	7.7 (Berkeley) 6/28/90
 */

/*
 * Constants and Structures defined by the Xerox Network Software
 * per "Internet Transport Protocols", XSIS 028112, December 1981
 */

/*
 * Protocols
 */
#define NSPROTO_RI	1		/* Routing Information */
#define NSPROTO_ECHO	2		/* Echo Protocol */
#define NSPROTO_ERROR	3		/* Error Protocol */
#define NSPROTO_PE	4		/* Packet Exchange */
#define NSPROTO_SPP	5		/* Sequenced Packet */
#define NSPROTO_RAW	255		/* Placemarker*/
#define NSPROTO_MAX	256		/* Placemarker*/


/*
 * Port/Socket numbers: network standard functions
 */

#define NSPORT_RI	1		/* Routing Information */
#define NSPORT_ECHO	2		/* Echo */
#define NSPORT_RE	3		/* Router Error */

/*
 * Ports < NSPORT_RESERVED are reserved for priveleged
 * processes (e.g. root).
 */
#define NSPORT_RESERVED		3000

/* flags passed to ns_output as last parameter */

#define	NS_FORWARDING		0x1	/* most of idp header exists */
#define	NS_ROUTETOIF		0x10	/* same as SO_DONTROUTE */
#define	NS_ALLOWBROADCAST	SO_BROADCAST	/* can send broadcast packets */

#define NS_MAXHOPS		15

/* flags passed to get/set socket option */
#define	SO_HEADERS_ON_INPUT	1
#define	SO_HEADERS_ON_OUTPUT	2
#define	SO_DEFAULT_HEADERS	3
#define	SO_LAST_HEADER		4
#define	SO_NSIP_ROUTE		5
#define SO_SEQNO		6
#define	SO_ALL_PACKETS		7
#define SO_MTU			8


/*
 * NS addressing
 */
union ns_host {
	u_char	c_host[6];
	u_short	s_host[3];
};

union ns_net {
	u_char	c_net[4];
	u_short	s_net[2];
};

union ns_net_u {
	union ns_net	net_e;
	u_long		long_e;
};

struct ns_addr {
	union ns_net	x_net;
	union ns_host	x_host;
	u_short	x_port;
};

/*
 * Socket address, Xerox style
 */
struct sockaddr_ns {
#if	defined(_SOCKADDR_LEN) || defined(_KERNEL)
	u_char		sns_len;
	u_char		sns_family;
#else
	u_short		sns_family;
#endif
	struct ns_addr	sns_addr;
	char		sns_zero[2];
};
#define sns_port sns_addr.x_port

#ifdef vax
#define ns_netof(a) (*(long *) & ((a).x_net)) /* XXX - not needed */
#endif
#define ns_neteqnn(a,b) (((a).s_net[0]==(b).s_net[0]) && \
					((a).s_net[1]==(b).s_net[1]))
#define ns_neteq(a,b) ns_neteqnn((a).x_net, (b).x_net)
#define satons_addr(sa)	(((struct sockaddr_ns *)&(sa))->sns_addr)
#define ns_hosteqnh(s,t) ((s).s_host[0] == (t).s_host[0] && \
	(s).s_host[1] == (t).s_host[1] && (s).s_host[2] == (t).s_host[2])
#define ns_hosteq(s,t) (ns_hosteqnh((s).x_host,(t).x_host))
#define ns_nullhost(x) (((x).x_host.s_host[0]==0) && \
	((x).x_host.s_host[1]==0) && ((x).x_host.s_host[2]==0))

#ifdef _KERNEL
#include "netns/proto_ns.h"	/* Function prototypes */
#if	MACH
#include <ns.h>			/* Dynamic config requires dependency */
#endif
#ifndef	INET			/* Attach to INET is crude... */
#include <inet.h>
#endif
#if	INET > 0
#define NSIP	1		/* for now */
#else
#undef	NSIP
#endif

extern struct domain nsdomain;
extern union ns_host ns_thishost;
extern union ns_host ns_zerohost;
extern union ns_host ns_broadhost;
extern union ns_net ns_zeronet;
extern union ns_net ns_broadnet;
#else
#ifdef __STDC__
extern struct ns_addr ns_addr(const char *);
extern char *ns_ntoa(struct ns_addr);
#else
extern struct ns_addr ns_addr();
extern char *ns_ntoa();
#endif
#endif
