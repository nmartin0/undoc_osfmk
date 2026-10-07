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
static char rcsid[] = "@(#)$RCSfile: alarm.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:26 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * alarm.c	5.2 (Berkeley) 3/9/86
 */


/*
 * Backwards compatible alarm.
 */
#include <sys/time.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex	_alarm_rmutex;
#endif

alarm(secs)
	int secs;
{
	struct itimerval it, oitv;
	register struct itimerval *itp = &it;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_alarm_rmutex);
#endif

	timerclear(&itp->it_interval);
	itp->it_value.tv_sec = secs;
	itp->it_value.tv_usec = 0;
	if (setitimer(ITIMER_REAL, itp, &oitv) < 0) {
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_alarm_rmutex);
#endif
		return (-1);
	}
	if (oitv.it_value.tv_usec)
		oitv.it_value.tv_sec++;
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_alarm_rmutex);
#endif
	return (oitv.it_value.tv_sec);
}
