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
static char rcsid[] = "$RCSfile: mktime.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:13 $";
#endif
/*
 * FUNCTION: mktime 
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
 * mktime.c
 */

/*
 * dysize(A) -- calculates the number of days in a year.  The year must be
 *      the current year minus 1900 (i.e. 1990 - 1900 = 90, so 'A' should
 *      be 90).
 */
#define dysize(A) (((1900+(A)) % 4 == 0 && (1900+(A)) % 100 != 0 || (1900+(A)) % 400 == 0) ? 366:365)
/*
 * YR(A) -- compute the number of days since 0 A.D.  'A' must be the current
 *              year minus 1900 (i.e. 1990 - 1900 = 90, so 'A' should be 90).
 */
#define YR(A) ((1900+A)*365 +(1900+A)/4 - (1900+A)/100 + ((1900+A) - 1600)/400)

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

/*
 * The following table is used for 1974 and 1975 and
 * gives the day number of the first day after the Sunday of the
 * change.
 */
extern struct { int	daylb; int	dayle; }
daytab[];

/*
 * Jjulian indicates a Julian date (either 0- or 1-based)
 * Mtimezone indicates that the TZ variable is of the form:
 *      TZA0TZB,M3.2.0,M10.5.0
 * Ltimezone indicates whether tzset() has been called already
 */
extern int Jjulian,Mtimezone,Ltimezone;
extern  int daylbegin, daylend, chngdy;
extern long dstbegsec, dstendsec;
extern long dstchg;

extern time_t globaltime;
extern int weekday();

/*
 * ds_in_effect() -- this routine is basically a copy of localtime(), but
 *      changed to return only TRUE or FALSE, indicating whether Daylight
 *      Savings Time is currently in effect.
 */
ds_in_effect(long copyt)
{
        int dayno;
        long tm_sec;
        int p1,p2;
        struct tm *ct,dsloc;
        int ds_daylbegin=daylbegin,ds_daylend=daylend;

        dsloc = *gmtime(&copyt);
        ct = &dsloc;

        if( daylight ) {
                dayno = ct->tm_yday;
                if (ds_daylbegin==0 || ds_daylend==0)
                {
                        if (ct->tm_year < 87)
                           ds_daylbegin = LSA;     /* last Sun in Apr */
                        else
                           ds_daylbegin = FSA;     /* first Sun in Apr */
                        ds_daylend = LSO;          /* Last Sun in Oct */
                        if(ct->tm_year == 74 || ct->tm_year == 75) {
                                ds_daylbegin = daytab[ct->tm_year-74].daylb;
                                ds_daylend = daytab[ct->tm_year-74].dayle;
                        }
                }
                ds_daylbegin = weekday(ct, ds_daylbegin);
                ds_daylend = weekday(ct, ds_daylend);
                tm_sec = (ct->tm_hour*60+ct->tm_min)*60+ct->tm_sec;
                p1 = dayno>ds_daylbegin || (dayno==ds_daylbegin && tm_sec>=dstbegsec);
                p2 = dayno<ds_daylend || (dayno==ds_daylend && tm_sec<dstendsec);
                if ((ds_daylend>ds_daylbegin && p1 && p2) ||
                                 (ds_daylend<ds_daylbegin && (p1||p2)))
                {
                /* Daylend is smaller than ds_daylbegin in the southern
                 * hemisphere.
                 */
                        if (dayno == ds_daylend && tm_sec < dstendsec &&
                                tm_sec > dstendsec - dstchg) {
                                return(0);
                        } else {
                                return(1);
                        }
                }
        }
return(0);
}

/*
 * FUNCTION: 
 *   The mktime() function converts the broken-down time, expressed as
 *   local time, in the structure pointed to by timeptr, into a time
 *   since the Epoch ( 00:00:00 GMT January 1, 1970) with the same encoding
 *   as that of the values returned by the time() function.  The original 
 *   values of the tm_wday and tm_yday components of the structure are 
 *   ignored, and the original values of the other components are not 
 *   restricted to the ranges described in the <time.h> entry.  The range 
 *   [0,61] for tm_sec allows for the occasional leap second or double leap
 *   second. -- X/Open definition of mktime().
 *
 *   The tm  structure is defined  in the time.h  header file,
 *   and it contains the following members:
 *
 *        int tm_sec;   (* Seconds (0 - 61) *)
 *        int tm_min;   (* Minutes (0 - 61) *)
 *        int tm_hour;  (* Hours (0 - 23) *)
 *        int tm_mday;  (* Day of month (1 - 31) *)
 *        int tm_mon;   (* Month of year (0 - 11) *)
 *        int tm_year;  (* Year - 1900 *)
 *        int tm_wday;  (* Day of week (0 - 6) (Sunday = 0) *)
 *        int tm_yday;  (* Day of year (0 - 365) *)
 *        int tm_isdst; (* Nonzero = Daylight saving time flag *)
 *
 *
 * PARAMETERS: 
 *	     struct tm *timeptr - pointer to be converted
 *
 * NOTES: mktime() is included in this module for access to the dstchg
 *        (DST change) variable.
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the time in seconds
 *		or -1 if that time cannot be represented.
 *
 */

time_t 	
mktime(struct tm *timeptr)
{
        struct tm gmtptr;
        time_t secs = 0L,savsecs;
	register int i, days = 0;
	int tm_sec;
	static int mdays[]={0,31,59,90,120,151,181,212,243,273,304,334,-1,-1};
#ifdef _THREAD_SAFE
	struct tm tmp;
#endif

	/*
	 * Since we aren't allowed to look at tm_yday by X/Open, we have
	 * to compute the number of days into the year ourselves.  Note that
	 * tm_mon is 0-based, so months go from 0 - 11.  We use that as an 
	 * index into mdays[], which gives us the number of days without 
	 * accounting for leap years.  If dysize() returns 366, we simply add
	 * 1 to days (if we're in March or later).  The numbers assigned
	 * into days are the number of days that MUST have passed already if 
	 * we're in a given month (i.e. if tm_mon == 3 (April), then AT LEAST 
	 * 90 days have already passed (we add 1 later if it's a leap year, 
	 * which gives us the total number of days)).  
	 */
        /*
         * This gross kludge ensures that our month and year are within
         * acceptable limits.
         */
        if (timeptr->tm_mon < 0) {
lt_repeat:
                timeptr->tm_year--;
                timeptr->tm_mon += 12;
                if (timeptr->tm_mon < 0) goto lt_repeat;  /* Yuck! */
		days = mdays[timeptr->tm_mon];
        } else if (timeptr->tm_mon > 11) {
gt_repeat:
                timeptr->tm_year++;
                timeptr->tm_mon -= 12;
                if (timeptr->tm_mon > 11) goto gt_repeat; /* Yuck * 2 */
                days = mdays[timeptr->tm_mon];
        } else
                days = mdays[timeptr->tm_mon];

        days += (timeptr->tm_mday - 1); /* convert to 0-origin */
        /*
         * Compute the number of days since (or until) Jan. 1, 1970, using the
         * YR macro.  Number of days until the beginning of the year - number
         * of days until Jan. 1, 1970 = # of days between Jan. 1, 1970 and
         * today.
         */
        days += (YR(timeptr->tm_year)) - (YR(70));
        if (dysize(timeptr->tm_year) == 366)
                days--;
        if ((dysize(timeptr->tm_year) == 366) && (timeptr->tm_mon > 1))
                days++;

        secs = days * 86400L;   /* seconds since 1970 (until beg. of today) */
	secs += (timeptr->tm_hour*60 + timeptr->tm_min)*60 + timeptr->tm_sec;

	/*
         * X/Open says that mktime() should perform as though tzset() were
         * called.  The best way to assure that is to call tzset().  It also
         * gives us the value of timezone which we use to offset time before
         * calling localtime() (makes localtime() give us back the same thing
         * we gave it, instead of factoring in the difference between local
         * time and GMT.)
	 */
        if (!Mtimezone) { /* prevent infinite recursion */
                Jjulian=0;
                globaltime=secs;
                tzset();        /* get timezone set up for use */
        }

	/*
         * ANSI C says if tm_isdst is 0 the daylight saving is not in
         * effect.
         * If it is > 1 mktime should assume it is in effect.
         * If it is == -1 mktime should figure out if it is in effect.
	 */

        savsecs = secs;
        if(timeptr->tm_isdst)
        {
                if(timeptr->tm_isdst > 0 )
                        secs -= dstchg  ;
                else
                {
                  if(ds_in_effect(savsecs))
                        secs -= dstchg;
                }
        }

        /*
         * Rebuild the timeptr structure by calling localtime() with the
         * newly calculated number of seconds since the Epoch as the
         * argument.  Store the results back into timeptr.  Note that
         * timezone is added to secs to produce the CUT time so that the
         * return from localtime() is correct.
         */
        secs += timezone;

	if ((int)secs < 0){		/* range check for overflow */
#ifdef _THREAD_SAFE
		seterrno(EINVAL);
#endif
		return( (time_t)-1 );
	}

        Ltimezone=1;

#ifdef _THREAD_SAFE
	timeptr = &tmp;
	localtime_r(&secs, timeptr);
#else
	*timeptr = *localtime(&secs);
#endif
        Ltimezone=0;

        return(secs);
}
