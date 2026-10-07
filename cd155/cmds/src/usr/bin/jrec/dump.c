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

void dumptm(tmptr)
struct tm *tmptr;
{
    (void) fprintf (stderr,"\ntm_sec = %02d\n",tmptr->tm_sec);
    (void) fprintf (stderr,"tm_min = %02d\n",tmptr->tm_min);
    (void) fprintf (stderr,"tm_hour = %02d\n",tmptr->tm_hour);
    (void) fprintf (stderr,"tm_mday= %02d\n",tmptr->tm_mday);
    (void) fprintf (stderr,"tm_mon = %02d\n",tmptr->tm_mon);
    (void) fprintf (stderr,"tm_year = %02d\n",tmptr->tm_year);
    (void) fprintf (stderr,"tm_wday = %02d\n",tmptr->tm_wday);
    (void) fprintf (stderr,"tm_yday = %02d\n",tmptr->tm_yday);
    (void) fprintf (stderr,"tm_isdst = %d\n\n",tmptr->tm_isdst);
}
