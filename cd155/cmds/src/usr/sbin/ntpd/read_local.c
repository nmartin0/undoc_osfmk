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
 
#ifdef	REFCLOCK
/*
 *  A dummy clock reading routine that reads the current system time.
 *  from the local host.  Its possible that this could be actually used
 *  if the system was in fact a very accurate time keeper (a true real-time
 *  system with good crystal clock or better).
 */

#include <sys/types.h>
#include <sys/time.h>

init_clock_local(file)
char *file;
{
	return getdtablesize();	/* invalid if we ever use it */
}

read_clock_local(cfd, tvp, mtvp)
int cfd;
struct timeval **tvp, **mtvp;
{
	static struct timeval realtime, mytime;

	gettimeofday(&realtime, 0);
	mytime = realtime;
	*tvp = &realtime;
	*mtvp = &mytime;
	return(0);
}
#endif
