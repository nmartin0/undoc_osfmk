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
 * @OSF_COPYRIGHT@
 */
/*-
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
/*
 * HISTORY
 * $Log: attime.c,v $
 * Revision 1.4  1994/11/19  01:34:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/05/11  17:41:07  stans
 * update to latest OSF version; version # chnage only.
 *
 * Revision 1.3  90/10/07  16:51:04  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  15:02:24  gm]
 * 
 * Revision 1.2  90/07/17  11:55:21  devrcs
 * 	New.
 * 	[90/07/09  16:47:09  brezak]
 * 
 * $EndLog$
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: attime.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:34:39 $";
#endif

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>

#if defined(NLS) || defined(KJI)
#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#define printf NLprintf
#define fprintf NLfprintf
#endif
#include <locale.h>

#ifdef MSG
#include <nl_types.h>
#include "ps_msg.h"
extern nl_catd catd;
#define MSGSTR(Num,Str) NLcatgets(catd,MS_PS,Num,Str)
#else
#define MSGSTR(Num,Str) Str
#endif
        
#define	HR	(60 * 60)
#define	DAY	(24 * HR)
#define	MON	(30 * DAY)

static time_t now;
/*
 * prttime prints a time in hours and minutes or minutes and seconds.
 * The character string tail is printed at the end, obvious
 * strings to pass are "", " ", or "am".
 */
static char *
prttime(tim, tail)
	time_t tim;
	char *tail;
{
	static char timebuf[32], *cp;

	if (tim >= 60) {
		int mins = tim % 60;
		sprintf(timebuf, "%2d:%02d", tim/60, mins);
	} else if (tim >= 0)
		sprintf(timebuf, "    %2d", tim);
	cp = timebuf;
	while (*cp)
		cp++;
	sprintf(cp, "%s", tail);

	return (timebuf);
}

static char *weekday[] = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
static char *month[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun",
		"Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

/* prtat prints a 12 hour time given a pointer to a time of day */
char *
attime(started)
	long *started;
{
	struct tm *p;
	register int hr, pm;
	static char prbuff[64];

	if (now == 0)
		time(&now);
	p = localtime(started);
	hr = p->tm_hour;
	pm = (hr > 11);
	if (hr > 11)
		hr -= 12;
	if (hr == 0)
		hr = 12;
	if (now - *started <= 18 * HR)
		return (prttime(hr * 60 + p->tm_min, pm ? "pm" : "am"));
	else if (now - *started <= 7 * DAY)
		sprintf(prbuff, "%*s%d%s", hr < 10 ? 4 : 3,
			weekday[p->tm_wday], hr, pm ? "pm" : "am");
	else
		sprintf(prbuff, "%2d%s%2d", p->tm_mday, month[p->tm_mon], 
			p->tm_year);

	return (prbuff);
}
