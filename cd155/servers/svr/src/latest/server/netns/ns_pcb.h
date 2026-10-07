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
 * $Log: ns_pcb.h,v $
 * Revision 1.5  1994/11/18  20:36:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:13:45  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:36:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:02:19  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:31:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:23:30  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:46:40  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:38:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:37:55  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:19:23  gm]
 * 
 * Revision 1.4  90/07/27  09:03:34  devrcs
 * 	XNS protocols, passthrough from BSD Reno release.
 * 	Operating under uniprocessor compat in OSF/1.
 * 	[90/07/19  17:14:20  tmt]
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
 *	Base:	ns_pcb.h	7.4 (Berkeley) 6/28/90
 */

/*
 * Ns protocol interface control block.
 */
struct nspcb {
	struct	nspcb *nsp_next;	/* doubly linked list */
	struct	nspcb *nsp_prev;
	struct	nspcb *nsp_head;
	struct	socket *nsp_socket;	/* back pointer to socket */
	struct	ns_addr nsp_faddr;	/* destination address */
	struct	ns_addr nsp_laddr;	/* socket's address */
	caddr_t	nsp_pcb;		/* protocol specific stuff */
	struct	route nsp_route;	/* routing information */
	struct	ns_addr nsp_lastdst;	/* validate cached route for dg socks*/
	long	nsp_notify_param;	/* extra info passed via ns_pcbnotify*/
	short	nsp_flags;
	u_char	nsp_dpt;		/* default packet type for idp_output*/
	u_char	nsp_rpt;		/* last received packet type by
								idp_input() */
};

/* possible flags */

#define NSP_IN_ABORT	0x1		/* calling abort through socket */
#define NSP_RAWIN	0x2		/* show headers on input */
#define NSP_RAWOUT	0x4		/* show header on output */
#define NSP_ALL_PACKETS	0x8		/* Turn off higher proto processing */

#define	NS_WILDCARD	1

#define nsp_lport nsp_laddr.x_port
#define nsp_fport nsp_faddr.x_port

#define	sotonspcb(so)		((struct nspcb *)((so)->so_pcb))

/*
 * Nominal space allocated to a ns socket.
 */
#define	NSSNDQ		2048
#define	NSRCVQ		2048


#ifdef _KERNEL
extern	struct	nspcb nspcb;			/* head of list */
#endif
