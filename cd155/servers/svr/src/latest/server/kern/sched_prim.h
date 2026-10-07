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
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: sched_prim.h,v $
 * Revision 1.6  1994/11/18  20:32:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  18:02:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:21:45  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:17:18  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:31:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/03/19  14:58:22  cfj
 * Merged from T9.
 *
 * Revision 1.2.8.1  1993/03/19  01:23:59  cfj
 * Fix to new locking code from OSF.
 *
 * Revision 2.3  1993/03/22  21:14:03  yazz
 * OSF lock changes.  Add new extern, thread_yield().
 *
 * Revision 2.2  91/08/31  13:38:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/07  17:00:25  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:55:49  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:58:00  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:57:44  gm]
 * 
 * Revision 1.3  90/01/02  20:06:16  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/10/26  07:50:51  gm
 * 	MACH X115 Update
 * 
 * Revision 6.1  89/07/26  14:22:46  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.9  89/10/11  14:25:14  dlb
 * 	thread_switch --> thread_run.
 * 	[89/09/01  17:41:25  dlb]
 * 
 * Revision 2.8  89/10/03  19:26:08  rpd
 * 	Defined thread_wakeup, thread_wakeup_with_result, and
 * 	thread_wakeup_one using thread_wakeup_prim.
 * 	[89/09/01  01:30:53  rpd]
 * 
 * Revision 2.7  89/03/09  20:15:35  rpd
 * 	More cleanup.
 * 
 * Revision 2.6  89/02/26  16:18:04  mrt
 * 	changed #ifdef	SCHED_PRIM_H_ to #ifndef
 * 
 * Revision 2.5  89/02/25  18:08:14  gm0w
 * 	Kernel code cleanup.
 * 	Put entire file under #indef KERNEL.
 * 	[89/02/15            mrt]
 * 
 * Revision 2.4  89/02/07  01:04:15  mwyoung
 * 	Relocated from sys/sched_prim.h
 * 
 * Revision 2.3  88/07/17  18:57:09  mwyoung
 * 	Added thread_wakeup_with_result routine; thread_wakeup
 * 	is a special case.
 * 
 * Revision 0.0  88/05/16            mwyoung
 * 	Added thread_wakeup_with_result routine; thread_wakeup
 * 	is a special case.
 * 	[88/05/16            mwyoung]
 * 
 * Revision 0.0  88/04/16            mwyoung
 * 	Added THREAD_RESTART wait result value.
 * 	[88/04/16            mwyoung]
 * 
 * Revision 0.0  88/02/29            dlb
 * 	thread_setrun is now a real routine.
 * 	[88/02/29            dlb]
 * 
 * Revision 0.0  87/10/13            dbg
 * 	Moved thread_will_wait and thread_go to sched_prim_macros.h,
 * 	to avoid including thread.h everywhere.
 * 	[87/10/13            dbg]
 * 
 * Revision 0.0  87/10/05            dbg
 * 	Created.  Moved thread_will_wait and thread_go here from
 * 	mach_ipc.
 * 	[87/10/05            dbg]
 * 
 * $EndLog$
 */
/*
 *	File:	sched_prim.h
 *	Author:	David Golub
 *
 *	Scheduling primitive definitions file
 *
 */

#ifndef	_KERN_SCHED_PRIM_H_
#define _KERN_SCHED_PRIM_H_

/*
 *	Possible results of assert_wait - returned in
 *	current_thread()->wait_result.
 */
#define THREAD_AWAKENED		0		/* normal wakeup */
#define THREAD_TIMED_OUT	1		/* timeout expired */
#define THREAD_INTERRUPTED	2		/* interrupted by clear_wait */
#define THREAD_SHOULD_TERMINATE	3		/* thread should terminate */
#define THREAD_RESTART		4		/* restart operation entirely */

/*
 *	Exported interface to sched_prim.c 
 */

extern void	sched_init();
extern void	assert_wait();
extern void	clear_wait();
extern void	thread_sleep();
extern void	thread_wakeup();		/* for function pointers */
extern void	thread_wakeup_prim();
extern void	thread_block();
extern void	thread_yield();
extern void	thread_run();
extern void	thread_set_timeout();
extern void	thread_setrun();

/*
 *	Routines defined as macros
 */

#define thread_wakeup(x)						\
		thread_wakeup_prim((x), FALSE, THREAD_AWAKENED)
#define thread_wakeup_with_result(x, z)					\
		thread_wakeup_prim((x), FALSE, (z))
#define thread_wakeup_one(x)						\
		thread_wakeup_prim((x), TRUE, THREAD_AWAKENED)

#endif	_KERN_SCHED_PRIM_H_
