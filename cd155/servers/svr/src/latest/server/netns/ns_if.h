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
 * $Log: ns_if.h,v $
 * Revision 1.6  1994/11/18  20:36:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/06/18  00:08:15  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.4  1993/07/14  18:13:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:35:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:01:53  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:28:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:12  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:34:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:46:06  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:32  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:37:32  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:18:48  gm]
 * 
 * Revision 1.4  90/07/27  09:03:16  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:10:36  tmt]
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
 *	Base:	ns_if.h	7.6 (Berkeley) 6/28/90
 */

/*
 * Interface address, xerox version.  One of these structures
 * is allocated for each interface with an internet address.
 * The ifaddr structure contains the protocol-independent part
 * of the structure and is assumed to be first.
 */

struct ns_ifaddr {
	struct	ifaddr ia_ifa;		/* protocol-independent info */
#define	ia_ifp		ia_ifa.ifa_ifp
#define	ia_flags	ia_ifa.ifa_flags
/*	union	ns_net	ia_net;		/ * network number of interface */
#define ia_net		ia_addr.sns_addr.x_net
	struct	ns_ifaddr *ia_next;	/* next in list of xerox addresses */
	struct	sockaddr_ns ia_addr;	/* reserve space for my address */
	struct	sockaddr_ns ia_dstaddr;	/* space for my broadcast address */
#define ia_broadaddr	ia_dstaddr
	struct	sockaddr_ns ia_netmask;	/* space for my network mask */
};

struct	ns_aliasreq {
	char	ifra_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	sockaddr_ns ifra_addr;
	struct	sockaddr_ns ifra_broadaddr;
#define ifra_dstaddr ifra_broadaddr
};
/*
 * Given a pointer to an ns_ifaddr (ifaddr),
 * return a pointer to the addr as a sockadd_ns.
 */

#define	IA_SNS(ia) (&(((struct ns_ifaddr *)(ia))->ia_addr))

/* This is not the right place for this but where is? */
#define	ETHERTYPE_NS	0x0600

#ifdef	NSIP
struct nsip_req {
	struct sockaddr rq_ns;	/* must be ns format destination */
	struct sockaddr rq_ip;	/* must be ip format gateway */
	short rq_flags;
};
#endif

#ifdef	_KERNEL
extern	struct	ns_ifaddr *ns_ifaddr;
extern	struct	ifqueue	nsintrq;	/* XNS input packet queue */
#endif
