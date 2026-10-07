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
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sleep.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:22 $";
#endif
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *   	sleep.c	5.3 (Berkeley) 9/30/87
 */


#include <sys/time.h>
#include <signal.h>

#define	setvec(vec, a) \
	vec.sv_handler = a; vec.sv_mask = vec.sv_onstack = 0

static int ringring;

unsigned int
sleep(n)
	unsigned n;
{
	static void sleepx();
	long omask;
	struct itimerval itv, oitv;
	register struct itimerval *itp = &itv;
	struct sigvec vec, ovec;
	int short_timer = 0, long_timer = 0;
	unsigned int retval;
	time_t start_time, end_time;
	int time_slept;

	if (n == 0)
		return(0);
	timerclear(&itp->it_interval);
	timerclear(&itp->it_value);
	if ((retval = setitimer(ITIMER_REAL, itp, &oitv)) < 0)
		return(retval);
	itp->it_value.tv_sec = n;
	if (timerisset(&oitv.it_value)) {
		if (timercmp(&oitv.it_value, &itp->it_value, >)) {

			/* sleep is shorter than previous timer; just
			 * remember so that we can restore it later
			 */
			long_timer = 1;
		}
		else {

			/* previous timer should go off before sleep ends;
			 * sleep for shorter time, and remember this fact
			 * so we can resignal SIGALRM
			 */
			itp->it_value = oitv.it_value;
			short_timer = 1;
		}
	}
	setvec(vec, (void *)sleepx);
	(void) sigvec(SIGALRM, &vec, &ovec);
	omask = sigblock(sigmask(SIGALRM));
	ringring = 0;
	(void) time(&start_time);
	(void) setitimer(ITIMER_REAL, itp, (struct itimerval *)0);
	sigpause(omask &~ sigmask(SIGALRM));

	/* some signal (either ours or another) occurred. Did we sleep
	 * the full requested time?
	 */
	(void) time(&end_time);
	time_slept = (int) end_time - start_time;
	if (time_slept < n)
		retval = n - time_slept;
	else retval = 0;

	(void) sigvec(SIGALRM, &ovec, (struct sigvec *)0);
	(void) sigsetmask(omask);
	if (short_timer) {
		if (ringring) {
		/* this was actually caller's original timer, so don't
		 * steal the signal from him; also, if he had set an interval,
		 * set his new timer for him
		 */
			kill(getpid(), SIGALRM);
			oitv.it_value = oitv.it_interval;
		}
	
		else
			/* some other signal; let his timer run */
			oitv.it_value.tv_sec -= time_slept;
	}
	else if (long_timer)
			/* adjust outstanding timer */
		oitv.it_value.tv_sec -= time_slept;

	(void) setitimer(ITIMER_REAL, &oitv, (struct itimerval *)0);
	return(retval);
}

static void
sleepx()
{

	ringring = 1;
}
