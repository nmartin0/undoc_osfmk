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
static char rcsid[] = "$RCSfile: ctimeutils.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:10 $";
#endif
/*
 * FUNCTIONS: ct_numb(), gettzname(), weekday(), atosec()
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
 * ctimeutils.c	
 */

/*
 * dysize(A) -- calculates the number of days in a year.  The year must be
 *      the current year minus 1900 (i.e. 1990 - 1900 = 90, so 'A' should
 *      be 90).
 */
#define dysize(A) (((1900+(A)) % 4 == 0 && (1900+(A)) % 100 != 0 || (1900+(A)) % 400 == 0) ? 366:365)

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

long	timezone = 0;		/* Default timezone is CUT (old GMT) */
int	daylight = 0;		/* no default daylight saving time.  */

/*  Initialize tzname fields allowing for timezones up to length NLTZSIZE-1.
 *  This prevents strcpy() in tzset from overflowing.
 */

#define	NLTZSIZE	256
char tznamex[2][NLTZSIZE]={"CUT",""};
char	*tzname[2]={tznamex[0],tznamex[1]};

int dmsize_[12]={31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

/*
 * The following table is used for 1974 and 1975 and
 * gives the day number of the first day after the Sunday of the
 * change.
 */
struct { int	daylb; int	dayle; }
daytab[] = {
	5,	333,	/* 1974: Jan 6 - last Sun. in Nov */
	58,	303,	/* 1975: Last Sun. in Feb - last Sun in Oct */
};

/*
 * Jjulian indicates a Julian date (either 0- or 1-based)
 * Mtimezone indicates that the TZ variable is of the form:
 *      TZA0TZB,M3.2.0,M10.5.0
 * Ltimezone indicates whether tzset() has been called already
 */
int Jjulian,Mtimezone,Ltimezone;
int daylbegin, daylend, chngdy;
long dstbegsec, dstendsec;
long dstchg=3600L;

time_t globaltime;

char *
ct_numb(char *cp,int n)
{
	cp++;
	if(n >= 10)
		*cp++ = (n/10)%10 + '0';
	else
		*cp++ = ' ';
	*cp++ = n%10 + '0';
	return(cp);
}

char *
atosec(char *p,long *result)
{
	int n, sign = 0;
	long v;
	/*
	 *  Convert string of form [-]hh:mm to seconds
	 */
	if(sign = (*p == '-'))
		p++;
	n = 0;
	while(*p >= '0' && *p <= '9')
		n = (n * 10) + *p++ - '0';
	v = ((long)(n * 60)) * 60;
	if(*p == ':') {
		p++;
		n = 0;
		while(*p >= '0' && *p <= '9')
			n = (n * 10) + *p++ - '0';
		v += n * 60;
	}
	if (*p == ':') {
		p++;
		n = 0;
		while (*p >= '0' && *p <= '9')
			n = (n * 10) + *p++ - '0';
		v += n;
	}
	if(sign)
		v = -v;
	*result = v;
	return p;
}

char *
gettzname(char *p,int index)
{
	char *q;
	int n, i;

	q = tzname[index];
	n = NLTZSIZE;
	while (*p && n-- > 0) {
		if ( (*p>='0' && *p<='9') || *p == '-' || *p == ':' || *p == '+' || *p == ',')
		    break;
		*q++ = *p++;
	}
	*q = '\0';
	/* for compatibility with standard tzname, pad with blanks to  */
	/* at least three bytes */
	if ( (n = strlen(tzname[index])) < 3 ) {
	    tzname[index][0] = 0;  /* zero = no daylight savings time */
	    n++;
	    while(n<3) tzname[index][n++] = ' ';
	    tzname[index][3] = 0;
	}
	return p;
}

char *
getdlight(char *p,long *day,long *chtime)
{
	char	buf[32];
	char	*s;
	int	Jflag = 0;

        Mtimezone = 0;

	/* Set default time */
	sprintf(buf, "%s", "02:00:00");
	atosec(buf, chtime);

	switch(*p) {
	case 'J': /* Julian day (1-based) */
		Jflag++;
		p++;
	default:  /* Julian day (0-based) */
		for (s = buf; (*s = *p) && *p != ',' && *p != '/'; 
			p++, s++);
		*s = '\0';

		/* convert to 0-based Julian */
		*day = atoi(buf) - Jflag;
                if(!Jflag)
                        Jjulian++;
		break;
	case 'M':
	{
		time_t	tim;
#ifdef _THREAD_SAFE
		struct tm  temp;
#endif
		struct	tm *tmp;
		int	i, j, k;
		int 	fday, mon;

                Mtimezone++;	
		/* Get details on Jan 1 of this year */
#ifdef _THREAD_SAFE
		tmp = &temp;
		gmtime_r(&globaltime, tmp);
#else
		tmp = gmtime(&globaltime);
#endif
                tmp->tm_mon = 0;
                tmp->tm_mday = 1;
		tim = mktime(tmp);
                Mtimezone = 0;
#ifdef _THREAD_SAFE
		gmtime_r(&tim, tmp);
#else
		tmp = gmtime(&tim);
#endif
		fday = tmp->tm_wday;

		/* Figure out day number */
		for (p++, s = buf; (*s = *p) && *p != '.' && *p != '/'; 
			p++, s++);
		if (*p == '\0')
			return(p);
		*s = '\0';
		j = atoi(buf) - 1;
		for (mon = 0, *day = 0; mon < j; mon++) {
			*day += dmsize_[mon];

			/* Keep track of first day of week of month */
                        /* Make sure tmp isn't NULL, also */
                        if (mon == 1 && tmp && dysize(tmp->tm_year) == 366) {
				/* If Feb, add leap day. */
				(*day)++;
				fday = (fday + dmsize_[mon] + 1) % 7;
			}
			else
				fday = (fday + dmsize_[mon]) % 7;
		}

		/* Get week number */
                for (p++, s = buf; (*s = *p) &&
			*p != '.' && *p != '/' && *p != ',';
			p++, s++);
		if (*p == '\0')
			return(p);
		*s = '\0';
		j = atoi(buf);

		/* Get day of week */
                for (p++, s = buf; (*s = *p) &&
			*p != '.' && *p != '/' && *p != ',';
			p++, s++);
		*s = '\0';
		i = atoi(buf);

		/* Add the intervening whole weeks */
		*day += 7 * (j - 1);
		if (i < fday) {
			/* Add the days of the first partial week */
			*day += (7 - fday);
			/* Add the offset from Sunday */
			*day += i;
		}
		else {
			/* Add the days between fday and the 1-st i-th day */
			*day += (i - fday);
		}

		/* Now the date is 0-based */
		Jjulian = 1;

	}
		break;
	case '\0':
		return(p);
	}
	if (*p == '\0')
		return(p);
	if (*p == ',')
		return(++p);

	/* Get daylight time of day.  Leading + or - illegal. */
	switch(*++p) {
	case '+':
	case '-':
		p++;
	}
	for (s = buf; (*s = *p) && *p != ','; p++, s++);
	*s = '\0';

	if (s > buf)
		/* Override previously set default */
		atosec(buf, chtime);
	/* skip over trailing ',' */
	if (*p == ',')
		p++;
	return(p);
}

/*
 * The argument is a 0-origin julian day number.
 * The value returned is the day number of the first
 * Sunday on or before the day given in the t structure.
 * If chngdy is set, it is consulted to see which day-of-week daylight
 * savings time should be moved up to.  If zero, day number is not changed
 * for weekday, allowing DST to start on a specific calendar date.
 * However, day number is adjusted by one to preserve calendar date
 * on a leap year (provided Jjulian is not set).
 */

int
weekday(struct tm *t,int d)
{
        if(!Jjulian)
         if(d >= 58)
           d += dysize(t->tm_year) - 365;
	if(chngdy == 0)
		/* don't move d to nearest day-of-week */
		return d;
	return(d - (d - t->tm_yday + t->tm_wday - chngdy + 700) % 7);
}
