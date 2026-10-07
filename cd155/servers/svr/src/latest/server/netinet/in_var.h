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
 * $Log: in_var.h,v $
 * Revision 1.4  1994/11/18  20:34:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/05/06  20:26:29  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:55  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:19  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/07/31  15:35:40  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:34:41  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:32  gm]
 * 
 * Revision 1.7  90/07/27  09:01:03  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:31:11  tmt]
 * 
 * Revision 1.6  90/04/27  19:15:48  devrcs
 * 	Use _KERNEL. Delete data, function declarations.
 * 	[90/04/20  12:53:05  tmt]
 * 
 * Revision 1.4  90/01/18  08:45:19  gm
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
 * Revision 1.2  89/12/26  10:11:44  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1985, 1986 Regents of the University of California.
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
 *	Base:	in_var.h	7.4 (Berkeley) 4/22/89
 *	Merged:	in_var.h	7.6 (Berkeley) 6/28/90
 */

/*
 * Interface address, Internet version.  One of these structures
 * is allocated for each interface with an Internet address.
 * The ifaddr structure contains the protocol-independent part
 * of the structure and is assumed to be first.
 */
struct in_ifaddr {
	struct	ifaddr ia_ifa;		/* protocol-independent info */
#define	ia_ifp		ia_ifa.ifa_ifp
#define ia_flags	ia_ifa.ifa_flags
					/* ia_{,sub}net{,mask} in host order */
	u_long	ia_net;			/* network number of interface */
	u_long	ia_netmask;		/* mask of net part */
	u_long	ia_subnet;		/* subnet number, including net */
	u_long	ia_subnetmask;		/* mask of subnet part */
	struct	in_addr ia_netbroadcast; /* to recognize net broadcasts */
	struct	in_ifaddr *ia_next;	/* next in list of internet addresses */
	struct	sockaddr_in ia_addr;	/* reserve space for interface name */
	struct	sockaddr_in ia_dstaddr; /* reserve space for broadcast addr */
#define	ia_broadaddr	ia_dstaddr
	struct	sockaddr_in ia_sockmask; /* reserve space for general netmask */
};

struct	in_aliasreq {
	char	ifra_name[IFNAMSIZ];		/* if name, e.g. "en0" */
	struct	sockaddr_in ifra_addr;
	struct	sockaddr_in ifra_broadaddr;
#define ifra_dstaddr ifra_broadaddr
	struct	sockaddr_in ifra_mask;
};
/*
 * Given a pointer to an in_ifaddr (ifaddr),
 * return a pointer to the addr as a sockaddr_in.
 */
#define	IA_SIN(ia) (&(((struct in_ifaddr *)(ia))->ia_addr))

#ifdef	_KERNEL
#if	NETSYNC_LOCK
/* These may yet become blocking locks with r/w semantics... */
extern	simple_lock_data_t	inifaddr_lock;
#define INIFADDR_LOCKINIT()	simple_lock_init(&inifaddr_lock)
#define INIFADDR_LOCK()		simple_lock(&inifaddr_lock)
#define INIFADDR_UNLOCK()	simple_unlock(&inifaddr_lock)
#else
#define INIFADDR_LOCKINIT()
#define INIFADDR_LOCK()
#define INIFADDR_UNLOCK()
#endif

extern	struct	in_ifaddr *in_ifaddr;
extern	struct	ifqueue	ipintrq;	/* ip packet input queue */
#endif
