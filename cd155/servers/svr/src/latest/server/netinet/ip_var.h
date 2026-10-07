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
 * $Log: ip_var.h,v $
 * Revision 1.5  1994/11/18  20:35:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/16  18:30:57  hobbes
 * Changed declarations of ih_len, ip_len and ip_of from short
 * to u_short to support 64K IP packets.
 *
 * Revision 1.3  1993/05/06  20:26:50  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:58  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:29:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:21:21  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:51  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:36:15  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:35:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:15:32  gm]
 * 
 * Revision 1.8  90/07/27  09:01:29  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:34:51  tmt]
 * 
 * Revision 1.7  90/04/27  19:16:19  devrcs
 * 	Use _KERNEL, _NO_BITFIELDS. Remove data declarations.
 * 	[90/04/20  12:58:13  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:41  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.4  90/01/18  08:47:17  gm
 * 	Put statistics lock under KERNEL && LOCK_NETSTATS.
 * 	[90/01/12  17:06:28  tmt]
 * 
 * 	Make structure overlay constant regardless of config options.
 * 	Change lock names to match function.
 * 	[90/01/08  16:08:31  tmt]
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
 * Revision 1.3  90/01/03  12:41:50  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:39:06  gm]
 * 
 * Revision 1.2  89/12/26  10:13:16  gm
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
 *	Base:	ip_var.h	7.6 (Berkeley) 9/20/89
 *	Merged:	ip_var.h	7.7 (Berkeley) 6/28/90
 */

/*
 * Overlay for ip header used by other protocols (tcp, udp).
 */
struct ipovly {
	caddr_t	ih_next, ih_prev;	/* for protocol sequence q's */
	u_char	ih_x1;			/* (unused) */
	u_char	ih_pr;			/* protocol */
	u_short	ih_len;			/* protocol length */
	struct	in_addr ih_src;		/* source internet address */
	struct	in_addr ih_dst;		/* destination internet address */
};

/*
 * Ip reassembly queue structure.  Each fragment
 * being reassembled is attached to one of these structures.
 * They are timed out after ipq_ttl drops to 0, and may also
 * be reclaimed if memory becomes tight.
 */
struct ipq {
	struct	ipq *next,*prev;	/* to other reass headers */
	u_char	ipq_ttl;		/* time for reass q to live */
	u_char	ipq_p;			/* protocol of this fragment */
	u_short	ipq_id;			/* sequence id for reassembly */
	struct	ipasfrag *ipq_next,*ipq_prev;
					/* to ip headers of fragments */
	struct	in_addr ipq_src,ipq_dst;
};

/*
 * Ip header, when holding a fragment.
 *
 * Note: ipf_next must be at same offset as ipq_next above
 */
struct	ipasfrag {
#if	defined(_KERNEL) || defined(_NO_BITFIELDS)
	u_char	ip_vhl;
#else
#if BYTE_ORDER == LITTLE_ENDIAN 
	u_char	ip_hl:4,
		ip_v:4;
#endif
#if BYTE_ORDER == BIG_ENDIAN 
	u_char	ip_v:4,
		ip_hl:4;
#endif
#endif
	u_char	ipf_mff;		/* copied from (ip_off&IP_MF) */
	u_short	ip_len;
	u_short	ip_id;
	u_short	ip_off;
	u_char	ip_ttl;
	u_char	ip_p;
	u_short	ip_sum;
	struct	ipasfrag *ipf_next;	/* next fragment */
	struct	ipasfrag *ipf_prev;	/* previous fragment */
};

/*
 * Structure stored in mbuf in inpcb.ip_options
 * and passed to ip_output when ip options are in use.
 * The actual length of the options (including ipopt_dst)
 * is in m_len.
 */
#define MAX_IPOPTLEN	40

struct ipoption {
	struct	in_addr ipopt_dst;	/* first-hop dst if source routed */
	char	ipopt_list[MAX_IPOPTLEN];	/* options proper */
};

/* Source route holding structure (moved here from ip_input.c) */
struct ip_srcrt {
	struct	in_addr dst;			/* final destination */
	char	nop;				/* one NOP to align */
	char	srcopt[IPOPT_OFFSET + 1];	/* OPTVAL, OLEN and OFFSET */
	struct	in_addr route[MAX_IPOPTLEN/sizeof(struct in_addr)];
};

struct	ipstat {
	long	ips_total;		/* total packets received */
	long	ips_badsum;		/* checksum bad */
	long	ips_tooshort;		/* packet too short */
	long	ips_toosmall;		/* not enough data */
	long	ips_badhlen;		/* ip header length < data size */
	long	ips_badlen;		/* ip length < ip header length */
	long	ips_fragments;		/* fragments received */
	long	ips_fragdropped;	/* frags dropped (dups, out of space) */
	long	ips_fragtimeout;	/* fragments timed out */
	long	ips_forward;		/* packets forwarded */
	long	ips_cantforward;	/* packets rcvd for unreachable dest */
	long	ips_redirectsent;	/* packets forwarded on same net */
	long	ips_noproto;		/* unknown or unsupported protocol */
	long	ips_delivered;		/* packets consumed here */
	long	ips_localout;		/* total ip packets generated here */
	long	ips_odropped;		/* lost packets due to nobufs, etc. */
	long	ips_reassembled;	/* total packets reassembled ok */
	long	ips_fragmented;		/* output packets fragmented ok */
	long	ips_ofragments;		/* output fragments created */
	long	ips_cantfrag;		/* don't fragment flag was set, etc. */
#if	defined(_KERNEL) && LOCK_NETSTATS
	simple_lock_data_t ips_lock;	/* statistics lock */
#endif
};

#ifdef _KERNEL
#if	NETSYNC_LOCK
extern	lock_data_t		ip_frag_lock;
extern	simple_lock_data_t	ip_misc_lock;
#define IPFRAG_LOCKINIT()	lock_init2(&ip_frag_lock, TRUE, LTYPE_IP)
#define IPFRAG_LOCK()		lock_write(&ip_frag_lock)
#define IPFRAG_UNLOCK()		lock_done(&ip_frag_lock)
#define IPMISC_LOCKINIT()	simple_lock_init(&ip_misc_lock)
#define IPMISC_LOCK()		simple_lock(&ip_misc_lock)
#define IPMISC_UNLOCK()		simple_unlock(&ip_misc_lock)
#else	/* !NETSYNC_LOCK */
#define IPMISC_LOCKINIT()
#define IPMISC_LOCK()
#define IPMISC_UNLOCK()
#define IPFRAG_LOCKINIT()
#define IPFRAG_LOCK()
#define IPFRAG_UNLOCK()
#endif

/* flags passed to ip_output as last parameter */
#define	IP_FORWARDING		0x1		/* most of ip header exists */
#define	IP_ROUTETOIF		SO_DONTROUTE	/* bypass routing tables */
#define	IP_ALLOWBROADCAST	SO_BROADCAST	/* can send broadcast packets */

extern	CONST u_char inetctlerrmap[];
extern	struct	ipstat	ipstat;
extern	struct	ipq	ipq;			/* ip reass. queue */
extern	u_short	ip_id;				/* ip packet ctr, for ids */
#endif
