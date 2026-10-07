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
 * $Log: event.c,v $
 * Revision 1.5  1994/11/18  20:32:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:01:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:07  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:07  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:27  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/03/09  14:35:56  durriya
 * 	[Revision 3.2  91/12/18  17:16:03  sp]
 * 	Include sys/synch.h to get spl macros
 *
 * Revision 2.2  91/08/31  13:37:17  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/07  16:59:48  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  13:50:25  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:55:31  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:49:20  gm]
 * 
 * 	Fix up header comments broken during last submit.
 * 	[90/09/20  18:41:55  gmf]
 * 
 * 	Make event_wait return a value (to avoid longjmp).
 * 	Change MACH_ASSERT to MACH_LDEBUG.
 * 	[90/08/17  17:42:28  nags]
 * 
 * Revision 1.7  90/09/23  15:51:24  devrcs
 * 	Fix event_wait to spin until event is really posted.
 * 	Mpsleep can return prematurely under certain
 * 	conditions (e.g. panic).
 * 	[90/09/20  17:19:48  gmf]
 * 
 * 	Get rid of unnecessary conditionals around calls
 * 	to mpsleep.
 * 	[90/09/14  13:44:52  gmf]
 * 
 * Revision 1.6  90/08/24  12:02:55  devrcs
 * 	Make event_wait take a timeout, and use mpsleep
 * 	[90/08/20  06:23:38  gmf]
 * 
 * 	Eliminate event_wait_timed.
 * 
 * Revision 1.5  90/08/09  13:20:26  devrcs
 * 	Fix inconsistency in ASSERT<->LDEBUG options.
 * 	[90/08/05  16:49:19  tmt]
 * 
 * Revision 1.4  90/06/29  13:36:18  devrcs
 * 	Don't use current_thread() until we're ready.
 * 	[90/06/28  13:28:01  gmf]
 * 
 * 	Fix interruptibility.  Add some debug/trace code.
 * 
 * 	Compressed history (reverse chronology):
 * 	From Encore 0.6:				nags@encore.com
 * 	    thread id and pc tracking; event_wait
 * 	    would return when interrupted by a signal,
 * 	    even when the wait was "non-interruptible".
 * 	    (All by shashi@encore.com.)
 * 	Nags merge.					nags@encore.com
 * 	Interrupt level uses events so we need spls.	boykin@encore.com
 * 	MMAX_MP:  allow signals to interrupt events.	alan@encore.com
 * 	MMAX_MP:  created file.				alan@encore.com
 * 	[90/06/26  22:30:29  gmf]
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
 *	File:	kern/event.c
 *
 *	Implementation of a simple event facility.
 */

#include <mach_assert.h>
#include <mach_ldebug.h>

#include <sys/unix_defs.h>
#include <sys/proc.h>
#include <sys/param.h>
#include <sys/synch.h>
#include <sys/user.h>
#include <kern/event.h>
#ifdef  OSF1_SERVER
#include <kern/lock.h>
#else   /* OSF1_SERVER */
#include <kern/thread.h>
#endif  /* OSF1_SERVER */
#include <kern/sched_prim.h>

#if	MACH_LDEBUG
extern int check_locks;
#endif

void
event_clear(evp)
event_t	*evp;
{
	int	s;

	s = splhigh();
	simple_lock (&evp->ev_slock);
	evp->ev_event = FALSE;
#if	MACH_ASSERT
#if	MACH_LDEBUG
	if (check_locks)
		evp->ev_thread = (char *) current_thread();
#endif
#ifdef	multimax
	evp->ev_caddr = getpc_fromec();
#endif
#endif
	simple_unlock (&evp->ev_slock);
	splx(s);
}


void
event_init(evp)
event_t	*evp;
{
	simple_lock_init (&evp->ev_slock);
#if	MACH_LDEBUG
	evp->ev_paddr = -1;
#endif
	event_clear(evp);
}

/*
 * If not interruptible, block until awakened normally.
 *
 * If interruptible, call mpsleep, with timo.  If mpsleep is interrupted,
 * it will have called issig() to check signals.  Just pass the error
 * back to the caller.
 *
 * Note that we must call mpsleep at elevated priority, so that the
 * event is caught.  If we simply lowered priority without unlocking the
 * lock, deadlock could result.
 *
 * The timo parameter is ignored if not interruptible.
 */
event_wait(evp, interruptible, timo)
event_t	*evp;
boolean_t interruptible;
int timo;
{
	int			error = 0, s;

	s = splhigh();
	simple_lock (&evp->ev_slock);
	while (evp->ev_event == FALSE) {
		if (interruptible) {
			error = mpsleep((caddr_t) &evp->ev_event, 
				(PZERO + 1) | PCATCH, "event", timo,
				(void *) simple_lock_addr(evp->ev_slock), 
				MS_LOCK_SIMPLE);
			if (error)
				break;
		} else {
			/*
			 * We won't wake up out of this easily
			 */
			(void) mpsleep((caddr_t) &evp->ev_event, 
				PZERO - 1, "event", 0,
				(void *) simple_lock_addr(evp->ev_slock), 
				MS_LOCK_SIMPLE);
		}
	} /* else -- event already posted */
	if (error == 0)
		simple_unlock (&evp->ev_slock);
	splx(s);
	return (error);
}

int
event_posted(evp)
event_t	*evp;
{
	register boolean_t	saved_event;
	int			s;

	s = splhigh();
	simple_lock(&evp->ev_slock);
	saved_event = evp->ev_event;
	simple_unlock(&evp->ev_slock);
	splx(s);
	return (int) saved_event;
}

void
event_post(evp)
event_t	*evp;
{
	int	s;

	s = splhigh();
	simple_lock (&evp->ev_slock);
	evp->ev_event = TRUE;
#if	MACH_ASSERT
#if	MACH_LDEBUG
	if (check_locks)
		evp->ev_thread = (char *) current_thread();
#endif
#ifdef	multimax
	evp->ev_paddr = getpc_fromep();
#endif
#endif
	simple_unlock(&evp->ev_slock);
	splx(s);
	thread_wakeup ((int) &evp->ev_event);
}
