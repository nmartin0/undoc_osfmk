/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: sync.c,v $
 * Revision 1.5  1994/11/18  21:07:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:06:20  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  21:00:29  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:07:27  dleslie
 * First R1_0 release
 *
 * Revision 2.6.2.1  92/03/28  10:36:43  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20  13:37:12  jeffreyh]
 * 
 * Revision 2.7  92/03/06  14:09:59  rpd
 * 	Replaced swtch_pri with yield.
 * 	[92/03/06            rpd]
 * 
 * Revision 2.6  91/05/14  17:59:54  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/14  14:21:38  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:41:42  mrt]
 * 
 * Revision 2.4  90/11/05  14:38:08  rpd
 * 	Fix casting.  Use new macros.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.3  90/08/07  14:31:50  rpd
 * 	Removed RCS keyword nonsense.
 * 
 * Revision 2.2  89/12/08  19:55:01  rwd
 * 	Changed spin_lock to spin_lock_solid to optimize.
 * 	[89/11/13            rwd]
 * 	Added copyright.  Move mutexes to cproc.c.  Spin locks are now
 * 	old mutexes.
 * 	[89/10/23            rwd]
 * 
 */
/*
 * sync.c
 *
 * Spin locks
 */

#include <cthreads.h>
#include "cthread_internals.h"

/*
 * Spin locks.
 * Use test and test-and-set logic on all architectures.
 */

int cthread_spin_count=0;

void
spin_lock_solid(p)
	register spin_lock_t *p;
{
	while (spin_lock_locked(p) || !spin_try_lock(p)) {
#ifdef STATISTICS
		cthread_spin_count++;
#endif
#ifdef SPIN_RESCHED
		yield();
#endif
	}
}
