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
#ifndef lint
static char sccsid[] = "@(#)tick.c	4.2 (Berkeley) 8/11/83";
#endif

/* time programs */
# include <stdio.h>
# include <sys/types.h>
# include <sys/time.h>
# include <sys/resource.h>
static long start, user, syst;
tick()
{
	struct rusage ru;
	struct timeval tv;
	if (getrusage(RUSAGE_SELF, &ru) < 0 || gettimeofday(&tv, 0) < 0) {
		start = 0;
		return;
	}
	user =  ru.ru_utime.tv_sec*1000+ru.ru_utime.tv_usec/1000;
	syst =  ru.ru_stime.tv_sec*1000+ru.ru_stime.tv_usec/1000;
	start = tv.tv_sec*1000+tv.tv_usec/1000;
}
tock()
{
	struct rusage ru;
	struct timeval tv;
	float lap, use, sys;
	if (start==0) return;
	if (getrusage(RUSAGE_SELF, &ru) < 0 || gettimeofday(&tv, 0) < 0)
		return;
	lap = ((tv.tv_sec*1000+tv.tv_usec/1000)-start)/1000.;
	use = ((ru.ru_utime.tv_sec*1000+ru.ru_utime.tv_usec/1000)-user)/1000.;
	sys = ((ru.ru_stime.tv_sec*1000+ru.ru_stime.tv_usec/1000)-syst)/1000.;
	printf("Elapsed %.2f CPU %.2f (user %.2f, sys %.2f)\n",
		lap, use+sys, use, sys);
}
