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
 * $Log: raw_cb.h,v $
 * Revision 1.5  1994/11/18  20:34:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:29:00  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:00:14  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:27:13  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:35  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:33:00  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.2  1991/08/31  13:41:24  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:34:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  14:32:55  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:49  gm]
 * 
 * Revision 1.8  90/07/27  08:59:47  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:52:10  tmt]
 * 
 * Revision 1.7  90/04/27  19:13:47  devrcs
 * 	Checkpoint.
 * 	[90/04/20  12:32:48  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:40  devrcs
 * 	Move data declarations to raw_cb.c.
 * 	[90/04/09  16:19:03  tmt]
 * 
 * Revision 1.5  90/03/27  13:21:30  gm
 * 	Change args to lock_init2() [gmf]
 * 
 * Revision 1.4  90/01/18  08:44:19  gm
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
 * Revision 1.3  90/01/03  12:41:00  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:10  gm]
 * 
 * Revision 1.2  89/12/26  09:47:18  gm
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
 * Copyright (c) 1980, 1986 Regents of the University of California.
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
 *	Base:	raw_cb.h	7.5 (Berkeley) 4/22/89
 *	Merged:	raw_cb.h	7.6 (Berkeley) 6/28/90
 */

/*
 * Raw protocol interface control block.  Used
 * to tie a socket to the generic raw interface.
 */
struct rawcb {
	struct	rawcb *rcb_next;	/* doubly linked list */
	struct	rawcb *rcb_prev;
	struct	socket *rcb_socket;	/* back pointer to socket */
	struct	sockaddr *rcb_faddr;	/* destination address */
	struct	sockaddr *rcb_laddr;	/* socket's address */
	struct	sockproto rcb_proto;	/* protocol family, protocol */
};

#define	sotorawcb(so)		((struct rawcb *)(so)->so_pcb)

/*
 * Nominal space allocated to a raw socket.
 */
#define	RAWSNDQ		8192
#define	RAWRCVQ		8192

#ifdef	_KERNEL
#if	NETSYNC_LOCK
extern lock_data_t global_raw_lock;
#define RAW_LOCKINIT()	lock_init2(&global_raw_lock, TRUE, LTYPE_RAW)
#define RAW_LOCK()	lock_write(&global_raw_lock);
#define RAW_UNLOCK()	lock_done(&global_raw_lock);
#else
#define RAW_LOCKINIT()
#define RAW_LOCK()
#define RAW_UNLOCK()
#endif

extern struct rawcb rawcb;		/* head of list */
#endif
