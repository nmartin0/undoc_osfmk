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
static char rcsid[] = "@(#)$RCSfile: times.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:08 $";
#endif
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * times.c   5.2 (Berkeley) 3/9/86
 */


#include <sys/time.h>
#include <sys/times.h>		/* for struct tms */
#include <sys/resource.h>

/*
 * times() for X/Open/POSIX compatability.
 *
 * Returns the number of (CLK_TCK)ths of a second since the epoch.
 * The value may wrap.
 */

static
scale(tvp)
	register struct timeval *tvp;
{
	/*
	 * convert seconds and microseconds to CLK_TCKs
	 */
	return ((tvp->tv_sec * CLK_TCK) + 
		((tvp->tv_usec * CLK_TCK) / 1000000));
}

clock_t
times(tmsp)
	register struct tms *tmsp;
{
	struct rusage ru;
	struct timeval tp;

	if (getrusage(RUSAGE_SELF, &ru) < 0)
		return (-1);
	tmsp->tms_utime = scale(&ru.ru_utime);
	tmsp->tms_stime = scale(&ru.ru_stime);
	if (getrusage(RUSAGE_CHILDREN, &ru) < 0)
		return ((clock_t) -1);
	tmsp->tms_cutime = scale(&ru.ru_utime);
	tmsp->tms_cstime = scale(&ru.ru_stime);
	if (gettimeofday(&tp, (struct timezone *)0) < 0)
		return((clock_t) -1);
	return((clock_t) scale(&tp));
}
