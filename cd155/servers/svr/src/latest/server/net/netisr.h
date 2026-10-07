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
 * $Log: netisr.h,v $
 * Revision 1.5  1994/11/18  20:34:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:08:20  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:28:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  18:59:54  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:26:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:25:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:19:16  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:32:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1992/03/09  14:40:43  durriya
 * 	92/01/07  23:34:35  condict
 * 	Set netisr flag to 1 to schedule a softnet interrupt.
 *
 * Revision 2.2  91/08/31  13:40:27  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/31  15:33:41  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:32:27  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:11:08  gm]
 * 
 * Revision 1.9  90/07/27  08:59:24  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/19  16:45:15  tmt]
 * 
 * Revision 1.8  90/07/05  23:12:40  devrcs
 * 	Add softnet->active to protect domain.
 * 	[90/07/03  18:47:10  tmt]
 * 
 * Revision 1.7  90/04/27  19:13:07  devrcs
 * 	Rearrange softnet_intr definition/declaration.
 * 	[90/04/20  12:30:05  tmt]
 * 
 * Revision 1.6  90/04/14  00:32:26  devrcs
 * 	Off by one on schednetisr.
 * 	[90/04/10  09:54:41  tmt]
 * 
 * 	Add softnet_intr array for dynamic netisr's, remove netisr word.
 * 	Move declarations to netisr.c. Use _KERNEL.
 * 	[90/04/09  16:16:31  tmt]
 * 
 * Revision 1.5  90/02/05  15:50:02  robert
 * 	Add NETISR_MB for getting more mbuf clusters.
 * 	[90/01/19  14:38:40  tmt]
 * 
 * Revision 1.4  90/01/18  08:43:58  gm
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
 * Revision 1.3  90/01/03  12:40:50  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:38:01  gm]
 * 
 * Revision 1.2  89/12/26  09:46:05  gm
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
 * Copyright (c) 1980, 1986, 1989 Regents of the University of California.
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
 *	Base:	netisr.h	7.5 (Berkeley) 4/22/89
 *	Merged:	netisr.h	7.6 (Berkeley) 6/28/90
 */

#ifdef	_KERNEL

#if	NETISR_THREAD
/*
 * The networking code is in separate kernel threads.
 */
#ifndef	LOCORE
#include "kern/sched_prim.h"
#define setsoftnet()	thread_wakeup_one((caddr_t)netisr_thread)
#endif

#else	NETISR_THREAD

extern int netisr;
extern void Netintr();

/*
 * The networking code runs off simulated software interrupts.
 * These interrupts will be triggered from a few "safe" places in the
 * server, including interrupt_exit, thread_block and end_server_op.  This
 * is to avoid calling them when networking code is active.
 */
#if defined(vax) || defined(tahoe)
#include "machine/mtpr.h"
#define	setsoftnet()	mtpr(SIRR, 12)
#else
#define	setsoftnet()	(netisr = 1)  /* Indicate soft interrupt is needed */
#endif

#endif	NETISR_THREAD

/*
 * Each ``pup-level-1'' input queue has a bit in a ``netisr'' status
 * word which is used to de-multiplex a single software
 * interrupt used for scheduling the network code to calls
 * on the lowest level routine of each protocol.
 */
#define	NETISR_MB	0		/* using 0 for mbufs */
#define	NETISR_ARP	1		/* using 1 for ARP */
#define	NETISR_IP	2		/* same as AF_INET */
#define	NETISR_IMP	3		/* same as AF_IMPLINK */
#define	NETISR_NS	6		/* same as AF_NS */
#define	NETISR_ISO	7		/* same as AF_ISO */

#define NETISR_MAX	8

#if	NETSYNC_LOCK
#define NETISR_LOCKINIT()	simple_lock_init(&netisr_slock)
#define NETISR_LOCK()		simple_lock(&netisr_slock)
#define NETISR_UNLOCK()		simple_unlock(&netisr_slock)
#else
#define NETISR_LOCKINIT()
#define NETISR_LOCK()
#define NETISR_UNLOCK()
#endif

/* Must be called at splimp() */
#define schednetisr(anisr) {		\
	NETISR_LOCK();			\
	++softnet_intr[anisr+1].pending;\
	NETISR_UNLOCK();		\
	setsoftnet();			\
}

#ifndef LOCORE

#if	NETSYNC_LOCK
extern simple_lock_data_t	netisr_slock;
#endif
extern struct softnet_intr {
	short	active;			/* Softnet active count */
	short	pending;		/* Interrupt pending on queue */
	void	(*isr)();		/* Isr to process input */
	struct	ifqueue *ifq;		/* Queue to receive packets, or NULL */
	struct	domain *dom;		/* Domain isr belongs to, or NULL*/
} softnet_intr[NETISR_MAX+1];		/* One extra for the wildcard */

#endif
#endif
