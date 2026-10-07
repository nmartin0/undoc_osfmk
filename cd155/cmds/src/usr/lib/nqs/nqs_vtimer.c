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
 
/*++ nqs_vtimer.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_vtimer.c,v $
 *
 * DESCRIPTION:
 *
 *	This module contains the two public functions of:
 *
 *		nqs_vtimer(), and
 *		nqs_valarm()
 *
 *	which respectively set virtual timers, and enable virtual
 *	timers.  Nqs_valarm() also catches the SIGALRM signals
 *	used to implement the virtual timer mechanism.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1995/03/17 18:21:21 $ $State: Exp $)
 * $Log: nqs_vtimer.c,v $
 * Revision 1.4  1995/03/17  18:21:21  kremenek
 *  Reviewer: davidl
 *  Risk: Low
 *  Benefit or PTS #: 9765
 *  Testing: Developer testing
 *  Module(s): cmds_libs/src/usr/include/nqs/buddy.h
 * 	cmds_libs/src/usr/include/nqs/buddyvar.h
 * 	cmds_libs/src/usr/lib/nqs/macs_lib.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_bsc.c
 * 	cmds_libs/src/usr/lib/nqs/nqs_vtimer.c
 * 	cmds_libs/src/usr/lib/nqs/macs_sched.c
 * 	cmds_libs/src/usr/lib/nqs/macs_rootp.c
 *
 * Revision 1.3  1994/11/19  02:53:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:26:57  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:55  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:37  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include <signal.h>
#include "nqs.h"
#include "nqsxvars.h"		/* Global vars */

#ifdef SDSC
#define MAX_ALARMS      6       /* Max number of alarms handled */
#else
#define	MAX_ALARMS	4	/* Max number of alarms handled */
				/* 1 for -a request activation; */
				/* 1 for pipe queue expiration; */
				/* 1 for pre-arrive expiration; */
				/* 1 for nqs_aboque.c; */
#endif

extern int errno;		/* System call error # */
extern unsigned alarm();	/* Set an alarm */
extern void nqs_abort();	/* Abort NQS execution */
extern time_t time();		/* Get GMT time since 0:00:00 Jan 1, 1970 */


/*
 *	Variables local to this module.
 */
static short n_vtimers = 0;	/* Number of alarms in effect */
static struct {
	time_t vtime;		/* GMT time at which fn should be called */
	void (*fn_to_call)();	/* Function to call */
} vtimer [MAX_ALARMS];


/*** nqs_vtimer
 *
 *
 *	void nqs_vtimer():
 *
 *	Set a "virtual timer" at which time the specified function
 *	is to be called with no arguments.  The timer set by this call
 *	however will NOT be enabled until an nqs_valarm() call is made.
 */
void nqs_vtimer (ptrvtime, fn_to_call)
time_t *ptrvtime;			/* GMT time at which fn should go */
void (*fn_to_call)();			/* Function to call when time is up */
{
	void nqs_valarm();		/* Virtual SIGALRM catcher function */
	register int i;			/* Index var */
	register int vtime;		/* GMT time at which fn should go */

	vtime = *ptrvtime;		/* Get time */
	i = 0;
	while (i < n_vtimers && vtimer [i].fn_to_call != fn_to_call) {
		i++;
	}
	if (i >= n_vtimers) {
		/*
		 *  The specified function to be activated is not
		 *  already listed in the call table.
		 */
		if (n_vtimers >= MAX_ALARMS) {
			/*
			 *  Cannot set alarm.  Table is full.
			 */
			printf ("F$Virtual timer table full ");
			printf ("in call to to nqs_vtimer().\n");
	 		errno = 0;		/* Not a system call error */
			nqs_abort();		/* Crash */
		}
		else {
			/*
			 *  Add this function to the table.
			 */
			vtimer [n_vtimers].vtime = vtime;
			vtimer [n_vtimers].fn_to_call = fn_to_call;
			n_vtimers++;		/* One more timer going */
		}
	}
	else {
		/*
		 *  The specified function was already in the table
		 *  to be activated at the proper time.
		 */
		if (vtimer [i].vtime > vtime) vtimer [i].vtime = vtime;
	}
}


/*** nqs_valarm
 *
 *
 *	nqs_valarm():
 *
 *	Enable ALL virtual timers as set by the nqs_vtimer() call and
 *	also catch a SIGALRM signal to implement the virtual timer schema.
 */
void nqs_valarm ()
{
	register int i;
	void (*fn)();
	register time_t timenow;	/* Current GMT time */

	signal (SIGALRM, nqs_valarm);	/* Re-instate SIGALRM handler */
	if (n_vtimers) {		/* Timer(s) were set */
		/*
		 *  Loop until all remaining timers are set to
		 *  go off in the future.
		 */
		do {
			i = 0;
			timenow = time ((time_t *)0);
			/*
			 *  Loop to service timers which have gone off.
			 */
			while (i < n_vtimers) {
				if (vtimer [i].vtime > timenow) {
					/*
					 *  This timer has not gone off.
					 */
					i++;	/* Examine next timer */
					continue;
				}
				/*
				 *  This timer has gone off.
				 */
				fn = vtimer [i].fn_to_call;
				n_vtimers--;
				if (i < n_vtimers) {
					/*
					 *  Another timer follows the
					 *  one that just went off.
					 *  Copy the succeeding timer
					 *  state into the one being
					 *  deleted.
					 */
					vtimer [i].vtime
						= vtimer [n_vtimers].vtime;
					vtimer [i].fn_to_call
						= vtimer [n_vtimers].fn_to_call;
				}
				(*fn)();/* Call the function */
			}
		} while (setalarm() == -1);
	}
}


/*** setalarm
 *
 *
 *	int setalarm():
 *	Set next closest alarm.
 *
 *	Returns:
 *		0: if an alarm was successfully set (or no timers
 *		   existed);
 *	       -1: if a timer exists which has gone off and needs to
 *		   be handled immediately.
 */
static int setalarm ()
{
	register int i;			/* Index */
	register time_t min_time;
	time_t timenow;

	if (n_vtimers) {
		/*
		 *  Now, search for the alarm that should go off the
		 *  soonest.
		 */
		time (&timenow);	/* Get current time */
		min_time = vtimer [0].vtime;
		for (i=1; i < n_vtimers; i++) {
			if (vtimer [i].vtime < min_time) {
				min_time = vtimer [i].vtime;
			}
		}
		if (min_time <= timenow) {
			/*
			 *  We have a timer that should go off NOW.
			 */
			return (-1);		/* No timer set */
		}
		min_time -= timenow;		/* Get alarm duration */
		if (min_time > 32767) {
			/*
			 *  On some machines, integers are 16-bits.  NQS
			 *  may find itself scheduling a timer that is
			 *  several days in duration.  That many seconds
			 *  cannot be represented in a 16-bit integer.
			 *  For such alarms, we set as many smaller
			 *  alarms as necessary to reach the proper
			 *  time.
			 */
			min_time = 32767;
		}
		alarm ((unsigned) min_time);	/* Set the alarm */
	}
	return (0);				/* Success */
}
