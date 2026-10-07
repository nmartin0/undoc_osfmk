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
 * Mach Operating System
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: event.h,v $
 * Revision 1.5  1994/11/18  20:32:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:01:14  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:09  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:28  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/08/31  13:37:22  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/07  16:59:51  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  13:50:30  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:49:27  gm]
 * 
 * Revision 1.5  90/08/24  12:03:05  devrcs
 * 	Removed event_wait_timed (see event_wait)
 * 	[90/08/19  23:15:32  gmf]
 * 
 * 	Make event_wait return a value.
 * 	Change MACH_ASSERT to MACH_LDEBUG.
 * 	[90/08/17  17:42:33  nags]
 * 
 * Revision 1.4  90/06/29  13:36:22  devrcs
 * 	Add debug/trace code.
 * 
 * 	Compressed history (reverse chronology):
 * 	Encore 0.6:  thread id/pc tracking (shashi).	nags@encore.com
 * 	Nags merge.					nags@encore.com
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.			gm@osf.org
 * 	Early Mach Release 2.5 merge with Encore MP.	alan@encore.com
 * 	Moved select queue defs to mmax/select.h.	alan@encore.com
 * 	Define sel_queue associated with select_event.	boykin@encore.com
 * 	MMAX_MP:  created file.				alan@encore.com
 * 	[90/06/26  11:12:45  gmf]
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
 *	File:	sys/event.h
 *
 *	Event structure and definitions.
 *
 *	Events with "memory".
 */

#ifndef	_KERN_EVENT_H_
#define	_KERN_EVENT_H_

#include <mach/boolean.h>
#include <kern/lock.h>

struct event {
	boolean_t		ev_event;
#if	defined(_KERNEL) && defined(MACH_LDEBUG)
	char		*ev_thread;	/* thread posted/cleared event */
	int		ev_paddr;	/* pc of last event_post */
	int		ev_caddr;	/* pc of last event_clear */
#endif
	decl_simple_lock_data(,ev_slock)
};

typedef struct event		event_t;

void	event_clear();			/* event hasn't happened yet */
void	event_init();			/* init lock and event */
void	event_post();			/* tell world event happened */
int	event_posted();			/* has event happened yet? */
int	event_wait();			/* wait for momentous event */

#endif	/* _KERN_EVENT_H_ */
