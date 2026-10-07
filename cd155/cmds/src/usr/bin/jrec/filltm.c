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
 *  Copyright (c)  1991 San Diego Supercomputer Center.
 *  All rights reserved.  The SDSC Software License Agreement
 *  specifies the terms and conditions for redistribution.
 */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <ctype.h>

/* Some functions for dealing with time data structures */

/* filltm fills out a pointed-to tm structure (except for tm_wday) */ 
/* using pointers to character arrays containing dates in MM/DD/YY     */
/* format (with single digits for MM and DD, and any non-digit character */
/* as the / separator allowed) and times in HH:MM:SS format (with single */
/* digits allowed for HH and MM, and any non-digit character as the    */
/* : separator).   */ 

void filltm(dateptr,hmsptr,tmptr)
char *dateptr,*hmsptr;
struct tm *tmptr;
{
    char tmpstr[3];

    (void) tzset();

#ifdef DEBUG
    (void) fprintf(stderr, "filltm: dateptr:%s: hmsptr:%s:\n",dateptr,hmsptr);
#endif
    tmpstr[0] = *dateptr;
    dateptr++;
    if (isdigit(*dateptr)) {
        tmpstr[1] = *dateptr; 
        dateptr++;
    }
    else
        tmpstr[1] = '\0';
    tmpstr[2] = '\0';
    tmptr->tm_mon= atoi(tmpstr) - 1;
    dateptr++;
    tmpstr[0] = *dateptr;
    dateptr++;
    if (isdigit(*dateptr)) {
        tmpstr[1] = *dateptr; 
        dateptr++;
    }
    else
        tmpstr[1] = '\0';
    tmpstr[2] = '\0';
    tmptr->tm_mday= atoi(tmpstr);
    dateptr++;
    tmpstr[0] = *dateptr;
    dateptr++;
    tmpstr[1] = *dateptr;
    tmpstr[2] = '\0';
    tmptr->tm_year= atoi(tmpstr);

    tmpstr[0] = *hmsptr;
    hmsptr++;
    if (isdigit(*hmsptr)) {
        tmpstr[1] = *hmsptr; 
        hmsptr++;
    }
    else
        tmpstr[1] = '\0';
    tmpstr[2] = '\0';
    tmptr->tm_hour= atoi(tmpstr);
    hmsptr++;
    tmpstr[0] = *hmsptr;
    hmsptr++;
    if (isdigit(*hmsptr)) {
        tmpstr[1] = *hmsptr; 
        hmsptr++;
    }
    else
        tmpstr[1] = '\0';
    tmpstr[2] = '\0';
    tmptr->tm_min= atoi(tmpstr);
    hmsptr++;
    tmpstr[0] = *hmsptr;
    hmsptr++;
    tmpstr[1] = *hmsptr;
    tmpstr[2] = '\0';
    tmptr->tm_sec= atoi(tmpstr);
    tmptr->tm_isdst = -1; /* ask mktime() to determine daylight/standard time */
}
