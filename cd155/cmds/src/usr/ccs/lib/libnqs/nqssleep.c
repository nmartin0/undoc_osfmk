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
 
/*++ nqssleep.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/nqssleep.c,v $
 *
 * DESCRIPTION:
 *
 *	NQS version of sleep().  The System V one has a very nasty
 *	bug such that when a non-SIGALRM signal arrives which is to
 *	be caught, AND the SIGALRM signal just happens to arrive
 *	while handling the first signal, then we never return to
 *	the original signal handler.
 *
 *	Thank you again AT&T.  The mighty mountain has labored, and
 *	once again failed to even produce a mouse correctly.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:27:27 $ $State: Exp $)
 * $Log: nqssleep.c,v $
 * Revision 1.3  1994/11/19  02:27:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:17:52  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:55:24  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:52:33  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <signal.h>

/*
 *
 *
 *	Variables local to this module.
 */
static short alarm_semaphore;		/* SIGALRM semaphore */


/*** nqssleep
 *
 *
 *	nqssleep():
 *	Sleep for N seconds.
 */
void nqssleep (timetosleep)
int timetosleep;
{
	void alarmsig();		/* Catch SIGALRM */
#if OSF
	void (*sigalrm_sav)();		/* Save old SIGALRM handler */
#else
	int (*sigalrm_sav)();		/* Save old SIGALRM handler */
#endif
	unsigned origalarm;		/* Original alarm time */

	origalarm = alarm (0);		/* Get original alarm set */
	sigalrm_sav = signal (SIGALRM, alarmsig); /* Get/Set SIGALRM handler */
	alarm_semaphore = 0;		/* Clear SIGALRM semaphore */
	alarm (timetosleep);		/* Set the alarm */
	while (alarm_semaphore == 0) pause();
	signal (SIGALRM, sigalrm_sav);	/* Restore SIGALRM state */
	alarm (origalarm);		/* Restore the original alarm */
}


/*** alarmsig
 *
 *
 *	void alarmsig():
 *
 *	This function serves as the signal catcher for SIGARLM signals
 *	occuring as the result of an alarm () call in the return_file()
 *	function above.
 *
 *	This function SETS the "alarm_semphore" flag.
 */
static void alarmsig ()
{
	signal (SIGALRM, alarmsig);
	alarm_semaphore++;		/* SIGALRM received */
}
