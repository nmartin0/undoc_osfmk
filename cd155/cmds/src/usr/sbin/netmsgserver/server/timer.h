/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
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
 * OSF/1 Release 1.0
 */
/*
 * timer.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/timer.h,v 1.2 1994/11/19 03:12:22 mtm Exp $
 *
 */

/*
 * Definitions for the timer package.
 */


#ifndef _TIMER_
#define _TIMER_

#include <mach/boolean.h>
#include <sys/time.h>
#include "mem.h"

/*
 * TIMEVAL_CMP
 *	Compare two timeval structures.
 *	Result is <, ==, or > 0, according to whether t1 is <, ==, or > t2.
 */
#define TIMEVAL_CMP(t1, t2)		\
	((t1).tv_sec == (t2).tv_sec ? (t1).tv_usec - (t2).tv_usec : (t1).tv_sec - (t2).tv_sec)

/*
 * Timer structure.
 */
typedef struct timer {
	struct timer	*link;		/* used by queue package */
	struct timeval	interval;	/* timer interval */
	int		(*action)();	/* action on timer expiration */
	char 		*info;		/* arbitrary client information */
	struct timeval	deadline;	/* used by timer package */
} *timer_act_t;

#define TIMER_NULL	((timer_act_t)0)


/*
 * TIMER_WAKE_UP
 *	wakes up the timer thread immediately.
 *
 */
extern void timer_wake_up();
/*
*/


/*
 * TIMER_START:
 *	If timer t is not already on the timer queue
 *	then the absolute deadline of t is computed and t is inserted in the timer queue.
 *
 *	Assumes that the timer is not on the timer queue - timer_restart should be used if it is.
 */
extern void timer_start(/*t*/);
/*
timer_act_t		t;
*/


/*
 * TIMER_STOP:
 *	If timer t is present on the queue then it is removed from the queue.
 *	Returns whether there was a timer to be removed or not.
 */
extern boolean_t timer_stop(/*t*/);
/*
timer_act_t		t;
*/


/*
 * TIMER_RESTART:
 *	Starts timer T Whether it was already queued on the timer queue or not.
 */
extern void timer_restart(/*t*/);
/*
timer_act_t		t;
*/


/*
 * TIMER_ALLOC:
 *	allocates space for a timer and initialises it.
 */
extern timer_act_t timer_alloc();


/*
 * TIMER_INIT:
 *	Initilizes the timer package.
 *	Creates a thread which waits for timers to expire.
 */
extern boolean_t timer_init();


/*
 * TIMER_KILL:
 *	Terminates the background timer thread by tricking it into suicide.
 *	Note that the timer thread may not be terminated upon return --
 *	the purpose of this routine is to do cleanup when a program is
 *	ready to terminate so that the threads package will not dump core.
 */
extern void timer_kill();


/*
 * Memory management definitions.
 */
extern mem_objrec_t		MEM_TIMER;


#endif _TIMER_
