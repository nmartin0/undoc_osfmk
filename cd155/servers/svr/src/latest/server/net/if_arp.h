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
 * $Log: if_arp.h,v $
 * Revision 1.7  1994/11/18  20:33:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/02/01  21:21:08  jlitvin
 * Whoops!  I accidently added the lint warning I removed to the log message!
 *
 * Revision 1.5  1994/02/01  00:50:27  jlitvin
 * Fix lint warnings about "\* encountered inside a comment."
 *
 * Revision 1.4  1993/07/14  18:07:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:27:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:28  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:24  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:18:40  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:38:44  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:32:45  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:31:32  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:09:44  gm]
 * 
 * Revision 1.7  90/07/27  08:58:41  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:24:49  tmt]
 * 
 * Revision 1.6  90/06/22  20:38:24  devrcs
 * 	Add macros for indexing in to arbitrary arp headers.
 * 	Add STALE and DEAD flags for enhanced arp (netinet/if_ether).
 * 	[90/06/15  08:13:48  tmt]
 * 
 * Revision 1.5  90/04/27  19:11:46  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:23:27  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:18  gm
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
 * Revision 1.3  90/01/02  20:10:46  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  09:43:56  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1986 Regents of the University of California.
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
 *	Base:	if_arp.h	7.3 (Berkeley) 6/27/88
 *	Merged:	if_arp.h	7.4 (Berkeley) 6/28/90
 */

/*
 * Address Resolution Protocol.
 *
 * See RFC 826 for protocol description.  ARP packets are variable
 * in size; the arphdr structure defines the fixed-length portion.
 * Protocol type values are the same as those for 10 Mb/s Ethernet.
 * It is followed by the variable-sized fields ar_sha, arp_spa,
 * arp_tha and arp_tpa in that order, according to the lengths
 * specified.  Field names used correspond to RFC 826.
 */
struct	arphdr {
	u_short	ar_hrd;		/* format of hardware address */
#define ARPHRD_ETHER 	1	/* ethernet hardware address */
#define ARPHRD_802	6	/* 802 net hardware address */
	u_short	ar_pro;		/* format of protocol address */
	u_char	ar_hln;		/* length of hardware address */
	u_char	ar_pln;		/* length of protocol address */
	u_short	ar_op;		/* one of: */
#define	ARPOP_REQUEST	1	/* request to resolve address */
#define	ARPOP_REPLY	2	/* response to previous request */
/*
 * The remaining fields are variable in size,
 * according to the sizes above.
 */
/*	u_char	ar_sha[];	\* sender hardware address */
/*	u_char	ar_spa[];	\* sender protocol address */
/*	u_char	ar_tha[];	\* target hardware address */
/*	u_char	ar_tpa[];	\* target protocol address */
};

/* For indexing into arbitrary ha/pa ARP headers */
#define AR_SHA(ah)	((u_char *)((ah)+1))
#define AR_SPA(ah)	(AR_SHA(ah)+(ah)->ar_hln)
#define AR_THA(ah)	(AR_SPA(ah)+(ah)->ar_pln)
#define AR_TPA(ah)	(AR_THA(ah)+(ah)->ar_hln)

/*
 * ARP ioctl request
 */
struct arpreq {
	struct	sockaddr arp_pa;		/* protocol address */
	struct	sockaddr arp_ha;		/* hardware address */
	int	arp_flags;			/* flags */
};
/*  arp_flags and at_flags field values */
#define	ATF_INUSE	0x01	/* entry in use */
#define ATF_COM		0x02	/* completed entry (enaddr valid) */
#define	ATF_PERM	0x04	/* permanent entry */
#define	ATF_PUBL	0x08	/* publish entry (respond for other host) */
#define	ATF_USETRAILERS	0x10	/* has requested trailers */
#define	ATF_USE802	0x20	/* host using 802 framing */
#define ATF_STALE	0x0100	/* entry needs to be refreshed */
#define ATF_DEAD	0x0200	/* entry (host) failed to refresh */

#define ATF_CANTCHANGE	(0xff00|ATF_INUSE|ATF_COM)
