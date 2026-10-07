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
 * $Log: in_pcb.h,v $
 * Revision 1.5  1994/11/18  20:34:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/18  00:16:43  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.3  1993/05/06  20:26:13  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:37  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:28:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:27:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:20:45  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:43:07  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:35:26  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/07  14:34:28  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:14:11  gm]
 * 
 * Revision 1.11  90/08/09  13:26:24  devrcs
 * 	Add INPLOOKUP_USECACHE bit for in_pcblookup.
 * 	[90/07/24  14:12:49  tmt]
 * 
 * Revision 1.10  90/07/27  09:00:53  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  17:29:42  tmt]
 * 
 * Revision 1.9  90/07/05  23:13:10  devrcs
 * 	Correct a comment.
 * 	[90/07/03  18:52:07  tmt]
 * 
 * Revision 1.8  90/06/22  20:39:02  devrcs
 * 	Parallelization repairs. Socket locks, then inpcb locks.
 * 	Take inpcbhead locks directly. Do refcounting right.
 * 	[90/06/07  16:11:10  tmt]
 * 
 * Revision 1.7  90/04/27  19:15:38  devrcs
 * 	Use _KERNEL. Remove function declarations.
 * 	[90/04/20  12:51:12  tmt]
 * 
 * Revision 1.6  90/03/27  13:22:06  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.5  90/02/05  15:50:27  robert
 * 	Add macro for INPCB_ISLOCKED.
 * 	[90/01/19  15:27:13  tmt]
 * 
 * Revision 1.4  90/01/18  08:45:09  gm
 * 	#ifdefs for user code compatability.
 * 	[90/01/16  13:43:46  tmt]
 * 
 * 	New convention: the global lock chain is the inp_lock of the inp_head.
 * 	This means we don't have to pass in the global lock pointer to notify,
 * 	detach, etc and makes the code more general. It also makes
 * 	in_losing_lock possible, which is used by NFS for now.
 * 	Make structure overlay constant regardless of config options.
 * 	[90/01/08  16:03:57  tmt]
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
 * Revision 1.3  90/01/03  12:41:34  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:49  gm]
 * 
 * Revision 1.2  89/12/26  10:10:58  gm
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
 *	Base:	in_pcb.h	7.4 (Berkeley) 4/22/89
 *	Merged:	in_pcb.h	7.6 (Berkeley) 6/28/90
 */

/*
 * Common structure pcb for internet protocol implementation.
 * Here are stored pointers to local and foreign host table
 * entries, local and foreign socket numbers, and pointers
 * up (to a socket structure) and down (to a protocol-specific)
 * control block.
 */
struct inpcb {
	struct	inpcb *inp_next,*inp_prev;
					/* pointers to other pcb's */
	struct	inpcb *inp_head;	/* pointer back to chain of inpcb's
					   for this protocol */
	struct	in_addr inp_faddr;	/* foreign host table entry */
	u_short	inp_fport;		/* foreign port */
	struct	in_addr inp_laddr;	/* local host table entry */
	u_short	inp_lport;		/* local port */
	struct	socket *inp_socket;	/* back pointer to socket */
	caddr_t	inp_ppcb;		/* pointer to per-protocol pcb */
	struct	route inp_route;	/* placeholder for routing entry */
	int	inp_flags;		/* generic IP/datagram flags */
	struct	ip inp_ip;		/* header prototype; should have more */
	struct	mbuf *inp_options;	/* IP options */
	int	inp_refcnt;		/* reference count */
#if	defined(_KERNEL) && NETSYNC_LOCK
	lock_data_t inp_lock;		/* structure lock */
	simple_lock_data_t inp_rc_lock;	/* refcnt lock */
#endif
};

/* flags in inp_flags: */
#define	INP_RECVOPTS		0x01	/* receive incoming IP options */
#define	INP_RECVRETOPTS		0x02	/* receive IP options for reply */
#define	INP_RECVDSTADDR		0x04	/* receive IP dst address */
#define	INP_CONTROLOPTS		(INP_RECVOPTS|INP_RECVRETOPTS|INP_RECVDSTADDR)

#ifdef sotorawcb
/*
 * Common structure pcb for raw internet protocol access.
 * Here are internet specific extensions to the raw control block,
 * and space is allocated to the necessary sockaddrs.
 */
struct raw_inpcb {
	struct	rawcb rinp_rcb;	/* common control block prefix */
	struct	mbuf *rinp_options;	/* IP options */
	int	rinp_flags;		/* flags, e.g. raw sockopts */
#define	RINPF_HDRINCL	0x1		/* user supplies entire IP header */
	struct	sockaddr_in rinp_faddr;	/* foreign address */
	struct	sockaddr_in rinp_laddr;	/* local address */
	struct	route rinp_route;	/* placeholder for routing entry */
};
#endif

#define	INPLOOKUP_WILDCARD	1
#define	INPLOOKUP_SETLOCAL	2
#define	INPLOOKUP_USECACHE	4

#define	sotoinpcb(so)		((struct inpcb *)(so)->so_pcb)
#define	sotorawinpcb(so)	((struct raw_inpcb *)(so)->so_pcb)

#ifdef _KERNEL

extern CONST struct in_addr	zeroin_addr;

#if	NETSYNC_LOCK
/*
 * The INPCB lock is used to protect each connection on the system.
 *  It implicitly also protects the TCPCB or UDPCB control block
 *  it points to as well.  The socket it points to is protected
 *  separately.
 *
 * Taking the INPCB lock is a pain because the structure is linked onto
 *  a global (e.g. UDP or TCP) chain.  Holding the INPCB lock doesn't
 *  mean that someone else can't dequeue themselves from the chain;
 *  i.e. the "next" and "prev" pointers are protected by the global
 *  lock.
 *
 * The lock hierarchy is socket first, inpcb second. A reference on an
 *  inpcb also protects the attached socket. Reference counts are
 *  incremented with the pcb head locked (for read or write). When
 *  the reference count goes to 0, the inpcb is deallocated, and
 *  sofree() is called. However, because sockets may live on from
 *  file table references, the socket unlock is left to the caller.
 *  
 * The chain heads are locked for write recursively to alleviate
 *  the situation in in_pcb.c and unconnected udp sends.
 */
#define	INPCB_LOCK(inp)		lock_write(&((inp)->inp_lock))
#define	INPCB_UNLOCK(inp)	lock_done(&((inp)->inp_lock))
#define	INPCB_LOCKINIT(inp)	lock_init2(&((inp)->inp_lock),TRUE,LTYPE_INPCB)
#define INPCB_ISLOCKED(inp)	lock_islocked(&((inp)->inp_lock))
#define INHEAD_READ_LOCK(head)	lock_read(&((head)->inp_lock))
#define	INHEAD_READ_UNLOCK(head) lock_done(&((head)->inp_lock))
#define INHEAD_WRITE_LOCK(head)	{ \
	lock_write(&((head)->inp_lock)); \
	lock_set_recursive(&((head)->inp_lock)); \
}
#define	INHEAD_WRITE_UNLOCK(head) { \
	lock_clear_recursive(&((head)->inp_lock)); \
	lock_done(&((head)->inp_lock)); \
}
#define INHEAD_LOCKINIT(head)	INPCB_LOCKINIT(head)

/*
 * Locks and operations on refcounts.
 */
#define	INPCBRC_LOCKINIT(inp)	simple_lock_init(&((inp)->inp_rc_lock))
#define	INPCBRC_LOCK(inp)	simple_lock(&((inp)->inp_rc_lock))
#define	INPCBRC_UNLOCK(inp)	simple_unlock(&((inp)->inp_rc_lock))

#define	INPCBRC_REF(inp) { \
	INPCBRC_LOCK(inp); \
	(inp)->inp_refcnt++; \
	INPCBRC_UNLOCK(inp); \
}

#define	INPCBRC_UNREF(inp) { \
	INPCBRC_LOCK(inp); \
	if ((inp)->inp_refcnt == 1) { \
		INPCBRC_UNLOCK(inp); \
		in_pcbfree(inp); \
	} else { \
		(inp)->inp_refcnt--; \
		INPCBRC_UNLOCK(inp); \
	} \
}

#else	/* !NETSYNC_LOCK */
#define	INPCB_LOCK(inp)
#define	INPCB_UNLOCK(inp)
#define	INPCB_LOCKINIT(inp)
#define	INPCB_ISLOCKED(inp)	1
#define INHEAD_READ_LOCK(head)
#define INHEAD_WRITE_LOCK(head)
#define	INHEAD_READ_UNLOCK(head)
#define	INHEAD_WRITE_UNLOCK(head)
#define INHEAD_LOCKINIT(head)
#define	INPCBRC_LOCKINIT(inp)
#define	INPCBRC_LOCK(inp)
#define	INPCBRC_UNLOCK(inp)
#define	INPCBRC_REF(inp)	(inp)->inp_refcnt++
#define	INPCBRC_UNREF(inp)	{if (--(inp)->inp_refcnt == 0) in_pcbfree(inp);}
#endif
#endif
