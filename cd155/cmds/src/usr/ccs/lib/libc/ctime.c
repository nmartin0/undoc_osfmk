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
static char rcsid[] = "$RCSfile: ctime.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:04:09 $";
#endif
/*
 * FUNCTIONS: ctime, localtime, gmtime, asctime
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
 * ctime.c	1.23  com/lib/c/time,3.1,9013 1/27/90 14:27:28
 */

/*
 * This module converts time as follows:
 * The epoch is 0000 Jan 1 1970 UTC (Coordinated Universal Time).
 * The argument time is in seconds since then.
 * The localtime(time) entry returns a pointer to an array
 * containing
 *  seconds (0-59)
 *  minutes (0-59)
 *  hours (0-23)
 *  day of month (1-31)
 *  month (0-11)
 *  year-1970
 *  weekday (0-6, Sun is 0)
 *  day of the year
 *  daylight savings flag
 *
 * The routine corrects for daylight saving
 * time and will work in any time zone provided
 * "timezone" is adjusted to the difference between
 * Universal and local standard time (measured in seconds).
 * In places like Michigan "daylight" must
 * be initialized to 0 to prevent the conversion
 * to daylight time.
 * There is a table which accounts for the peculiarities
 * undergone by daylight time in 1974-1975.
 *
 * The routine does not work
 * in Saudi Arabia which runs on Solar time.
 *
 * asctime(tvec)
 * where tvec is produced by localtime
 * returns a ptr to a character string
 * that has the ascii time in the form
 *	Thu Jan 01 00:00:00 1970
 *	01234567890123456789012345
 *	0	  1	    2
 *
 * ctime(timer) just calls localtime, then asctime.
 *
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

/* Define the latest possible julian date of the last Sunday in April (LSA),
 * the last Sunday in October (LSO), and the first Sunday in April (FSA).
 */
#define LSA  119
#define LSO  303
#define FSA   96

extern int dmsize_[];

extern struct { int	daylb; int	dayle; }
daytab[];

extern int Jjulian,Mtimezone,Ltimezone;
extern int daylbegin, daylend, chngdy;
extern long dstbegsec, dstendsec;
extern long dstchg;

extern time_t globaltime;

extern char *atosec();
extern char *gettzname();
extern char *getdlight();
extern char *ct_numb();
extern int weekday();

static char cbuf[26];

/*
*	Thread-safe routines.
*	The thread-safe routines do not return pointers
*	to static data the information is now passed in.
*	Locks are used in localtime and tzset to avoid
*	corruption of global data.
*/
/* Thread-safe ctime */

#ifdef _THREAD_SAFE
int
ctime_r(const time_t *timer, char *cbuf, int len)
{
	struct tm 	tm;

	if ((cbuf == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
	if (localtime_r(timer, &tm) < 0) {
		asctime_r(&tm, cbuf, len);
		return(0);
	} else
		return (-1);
}
#else /* _THREAD_SAFE */

char 	*
ctime(const time_t *timer)
{
	return(asctime(localtime(timer)));
}
#endif /* _THREAD_SAFE */


/* Thread-safe localtime */

#ifdef  _THREAD_SAFE 
int
localtime_r(const time_t *timer, struct tm *ct)
#else
struct tm *
localtime(const time_t *timer)
#endif
{
	int dayno;
	long tm_sec;
	int p1,p2;
#ifndef _THREAD_SAFE 
	struct tm *ct;
        static struct tm lcloc; /* the static structure returned to the user */
#endif /* _THREAD_SAFE */ 
	long copyt;
        int lc_daylbegin,lc_daylend;

#ifdef _THREAD_SAFE 
	if (ct == NULL) {
		seterrno(EINVAL);
		return(-1);
	}
	rec_mutex_lock(&_ctime_rmutex);
#endif /* _THREAD_SAFE */ 

        if(!Ltimezone) {
                globaltime = *timer ;
                Jjulian=0;
                tzset();
        }
        lc_daylbegin = daylbegin;
	lc_daylend = daylend;
	copyt = *timer - timezone;

#ifdef _THREAD_SAFE 
	gmtime_r(&copyt, ct);
#else /* _THREAD_SAFE */ 
        lcloc = *gmtime(&copyt);
        ct = &lcloc;
#endif /* _THREAD_SAFE */ 

	if( daylight ) {
		dayno = ct->tm_yday;

                if (lc_daylbegin==0 || lc_daylend==0)
		{
			if (ct->tm_year < 87)
                           lc_daylbegin = LSA;     /* last Sun in Apr */
			else
                           lc_daylbegin = FSA;     /* first Sun in Apr */
			lc_daylend = LSO;          /* Last Sun in Oct */
			if(ct->tm_year == 74 || ct->tm_year == 75) {
                                lc_daylbegin = daytab[ct->tm_year-74].daylb;
                                lc_daylend = daytab[ct->tm_year-74].dayle;
			}
		}
                lc_daylbegin = weekday(ct, lc_daylbegin);
                lc_daylend = weekday(ct, lc_daylend);
		tm_sec = (ct->tm_hour*60+ct->tm_min)*60+ct->tm_sec;
                p1 = dayno>lc_daylbegin || (dayno==lc_daylbegin && tm_sec>=dstbegsec);
                p2 = dayno<lc_daylend || (dayno==lc_daylend && tm_sec<dstendsec);
                if ((lc_daylend>lc_daylbegin && p1 && p2) ||
                                 (lc_daylend<lc_daylbegin && (p1||p2)))
		{
		/* Daylend is smaller than lc_daylbegin in the southern
		 * hemisphere.
		 */
                        if (dayno == lc_daylend && tm_sec < dstendsec &&
                                tm_sec > dstendsec - dstchg) {
#ifdef _THREAD_SAFE 
				gmtime_r(&copyt, ct);
#else /* _THREAD_SAFE */ 
				ct = gmtime(&copyt);
#endif /* _THREAD_SAFE */ 
			} else {
				copyt += dstchg;
#ifdef _THREAD_SAFE 
				gmtime_r(&copyt, ct);
#else /* _THREAD_SAFE */ 
				ct = gmtime(&copyt);
#endif /* _THREAD_SAFE */ 
				ct->tm_isdst++;
			}
		}
	}

#ifdef _THREAD_SAFE 
	rec_mutex_unlock(&_ctime_rmutex);
	return(0);
#else /* _THREAD_SAFE */
	return(ct);
#endif /* _THREAD_SAFE */
}


/*    Thread-safe gmtime    */

#ifdef  _THREAD_SAFE 
int
gmtime_r(const time_t *timer, struct tm *xtime)
{
	int d0, d1;
	long hms, day;

#ifdef _THREAD_SAFE
	if (xtime == NULL) {
		seterrno(EINVAL);
		return(-1);
	}
#endif
	/*
	 * break initial number into days
	 */
	hms = *timer % 86400L;
	day = *timer / 86400L;
	if(hms < 0) {
		hms += 86400L;
		day -= 1;
	}
	/*
	 * generate hours:minutes:seconds
	 */
	xtime->tm_sec = hms % 60;
	d1 = hms / 60;
	xtime->tm_min = d1 % 60;
	d1 /= 60;
	xtime->tm_hour = d1;

	/*
	 * day is the day number.
	 * generate day of the week.
	 * The addend is 4 mod 7 (1/1/1970 was Thursday)
	 */

	xtime->tm_wday = (day + 7340036L) % 7;

	/*
	 * year number
	 */
	if(day >= 0)
		for(d1=70; day >= dysize(d1); d1++)
			day -= dysize(d1);
	else
		for(d1=70; day < 0; d1--)
			day += dysize(d1-1);
	xtime->tm_year = d1;
	xtime->tm_yday = d0 = day;

	/*
	 * generate month
	 */

	if (dysize(d1) == 366)
	    dmsize_[1] = 29;
	for(d1=0; d0 >= dmsize_[d1]; d1++)
		d0 -= dmsize_[d1];
	dmsize_[1] = 28;
	xtime->tm_mday = d0+1;
	xtime->tm_mon = d1;
	xtime->tm_isdst = 0;
	return(0);
}
#else  /* _THREAD_SAFE */
struct tm *
gmtime(const time_t *timer)
{
	int d0, d1;
	long hms, day;
	static struct tm xtime;
	/*
	 * break initial number into days
	 */
	hms = *timer % 86400L;
	day = *timer / 86400L;

	if(hms < 0) {
		hms += 86400L;
		day -= 1;
	}
	/*
	 * generate hours:minutes:seconds
	 */
	xtime.tm_sec = hms % 60;
	d1 = hms / 60;
	xtime.tm_min = d1 % 60;
	d1 /= 60;
	xtime.tm_hour = d1;

	/*
	 * day is the day number.
	 * generate day of the week.
	 * The addend is 4 mod 7 (1/1/1970 was Thursday)
	 */

	xtime.tm_wday = (day + 7340036L) % 7;

	/*
	 * year number
	 */
	if(day >= 0)
		for(d1=70; day >= dysize(d1); d1++)
			day -= dysize(d1);
	else
		for(d1=70; day < 0; d1--)
			day += dysize(d1-1);
	xtime.tm_year = d1;
	xtime.tm_yday = d0 = day;

	/*
	 * generate month
	 */

	if (dysize(d1) == 366)
	    dmsize_[1] = 29;
	for(d1=0; d0 >= dmsize_[d1]; d1++)
	    d0 -= dmsize_[d1];
	dmsize_[1] = 28;
	xtime.tm_mday = d0+1;
	xtime.tm_mon = d1;
	xtime.tm_isdst = 0;
	return(&xtime);
}
#endif /* _THREAD_SAFE  */
 
 /* Thread-safe asctime */

#ifdef  _THREAD_SAFE
int
asctime_r(const struct tm *timeptr, char *cbuf, int len)
#else /* _THREAD_SAFE */
char *
asctime(const struct tm *timeptr)
#endif /* _THREAD_SAFE */
{
	char *cp, *ncp;
	int *tp;
	char	*ct_numb();

#ifdef _THREAD_SAFE
	if ((cbuf == NULL) || len < 1) {
		seterrno(EINVAL);
		return(-1);
	}
#endif
	cp = cbuf;
	for(ncp = "Day Mon 00 00:00:00 1900\n"; *cp++ = *ncp++; );
	ncp = &"SunMonTueWedThuFriSat"[3*timeptr->tm_wday];
	cp = cbuf;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp++;
	tp = (int *)&timeptr->tm_mon;
	ncp = &"JanFebMarAprMayJunJulAugSepOctNovDec"[(*tp)*3];
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	*cp++ = *ncp++;
	cp = ct_numb(cp, *--tp);
	cp = ct_numb(cp, *--tp+100);
	cp = ct_numb(cp, *--tp+100);
	cp = ct_numb(cp, *--tp+100);
	if(timeptr->tm_year >= 100) {
		cp[1] = '2';
		cp[2] = '0';
	}
	cp += 2;
	cp = ct_numb(cp, timeptr->tm_year+100);
#ifdef  _THREAD_SAFE 
	return(0);
#else /* _THREAD_SAFE  */
	return(cbuf);
#endif /* _THREAD_SAFE */
}
