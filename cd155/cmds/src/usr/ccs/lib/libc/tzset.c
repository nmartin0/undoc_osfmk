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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "$RCSfile: tzset.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:18 $";
#endif
/*
 * FUNCTION: tzset 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * tzset.c
 */

/*
 * tzset() looks for an environment variable named
 * TZ. It should be in the form "[:]ESTn" or "[:]ESTnEDT",
 * where "n" represents a string of digits with an optional
 * negative sign (for locations east of Greenwich, England), or
 * "ESThh[:mm[:ss]][DST[hh[:mm[:ss]]][,start[/time],end[/time]]],
 * where "start" and "end" specify the day to start and end daylight
 * time.  The format is either "Jn" (1-based Julian), "n" (0-based Julian),
 * or "Mm.n.d" (month, week of month, day of week).
 * If the variable is present, it will set the external
 * variables "timezone", "daylight", and "tzname"
 * appropriately. It is called by localtime, and
 * may also be called explicitly by the user.
 */

#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#ifdef _THREAD_SAFE  
#  include <errno.h>
/*  
*   The thread-safe version of the code have to be synchronized.
*
*   mutex _ctime_rmutex for synchronization
*/
#include "rec_mutex.h"

extern struct rec_mutex	_ctime_rmutex;
#endif /* _THREAD_SAFE  */

extern int daylbegin, daylend, chngdy;
extern long dstbegsec, dstendsec;
extern long dstchg;
extern char *atosec();
extern char *gettzname();
extern char *getdlight();

/*
 * Built in timezone names, in case TZ isn't set
 */
static struct zone {
	int	offset;
	char	*stdzone;
	char	*dlzone;
} zonetab[] = {
	-1*60,	"MET",	"MET DST",	/* Middle European */
	-2*60,	"EET",	"EET DST",	/* Eastern European */
	4*60,	"AST",	"ADT",		/* Atlantic */
	5*60,	"EST",	"EDT",		/* Eastern */
	6*60,	"CST",	"CDT",		/* Central */
	7*60,	"MST",	"MDT",		/* Mountain */
	8*60,	"PST",	"PDT",		/* Pacific */
	0*60,	"WET",	"WET DST",	/* Western European */
	-10*60,	"EST",	"EST",		/* Aust: Eastern */
     -10*60+30,	"CST",	"CST",		/* Aust: Central */
	-8*60,	"WST",	0,		/* Aust: Western */
	-1
};

/* The tzset() function includes all the POSIX requirements */

void
tzset(void)
{
	char 	*p, *q;
	int 	n;
	int 	sign;
	struct timeval	tv;
	struct timezone	tz;
	struct zone	*zp;
	char	*getdlight();

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_ctime_rmutex);
#endif
        chngdy = 0;

	if((p = getenv ("TZ")) && *p) {
		char *gettzname(), *atosec();

		while (*p == ':')
			p++;
		p = gettzname(p,0);
		p = atosec(p, &timezone);
		p = gettzname(p,1);
		daylight = tzname[1][0];
		if (*p) {
			/* this is an extended TZ variable */
			if (*p == ',') {
				/* no dst offset */
				dstchg = 1*60*60;
				p++;
			}
			else {
				/* dst offset specified */
				p = atosec(p, &dstchg);
				dstchg = timezone - dstchg;
				if (*p) 
					p++; 
			} 
			/* get daylight start/end */
			p = getdlight(p, &daylbegin, &dstbegsec);
			getdlight(p, &daylend, &dstendsec);
		}
		else {
			dstchg = 1*60*60;
			chngdy = 7;
			daylbegin = daylend = 0;
			dstbegsec = dstendsec = 2*60*60;
		}
	}
	/*
	 * Try getting info from kernel.  This is only
	 * a last resort.
	 */
	else if (gettimeofday(&tv, &tz) == 0) {
		dstchg = 1*60*60;
		chngdy = 7;
		daylbegin = daylend = 0;
		dstbegsec = dstendsec = 2*60*60;
		timezone = tz.tz_minuteswest * 60;
		daylight = tz.tz_dsttime;
		for (zp = zonetab; zp->offset != -1; zp++) {
			if (zp->offset == tz.tz_minuteswest) {
				if (zp->stdzone)
					tzname[0] = zp->stdzone;
				if (zp->dlzone)
					tzname[1] = zp->dlzone;
				break;
			}
		}
		if (zp->offset == -1) {	/* Make one up */
			sprintf(tzname[0], "CUT%c%d:%02d",
				tz.tz_minuteswest < 0 ? '+' : '-',
				tz.tz_minuteswest / 60,
				tz.tz_minuteswest % 60);
			tzname[1][0] = '\0';
		}
	}
#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_ctime_rmutex);
#endif
}
