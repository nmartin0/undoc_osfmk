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
 
/*++ fmttime.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/fmttime.c,v $
 *
 * DESCRIPTION:
 *
 *	Format a time string.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:44 $ $State: Exp $)
 * $Log: fmttime.c,v $
 * Revision 1.3  1994/11/19  02:26:44  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:08  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:36  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:48:47  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)fmttime.c	1.2 (fmttime.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if OSF
#include "sys_types_OSF.h"      /* time_t */ /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>			/* time_t */
#endif

#if	SGI | SYS52 | UTS | UNICOS | OSF
#include <time.h>
extern char *tzname [2];
extern void tzset();			/* Set local timezone */
#else
#if	BSD42 | BSD43 | ULTRIX
#include <sys/time.h>
extern char *timezone();
#else
BAD SYSTEM TYPE
#endif
#endif

extern struct tm *localtime ();		/* Get local time */
extern char *asctime();			/* Format time */

/*** fmttime
 *
 *
 *	char *fmttime():
 *
 *	Return a pointer to a formatted time string where the time
 *	is specified as a long integer defining the number of seconds
 *	since 00:00:00 GMT, January 1, 1970.
 */
char *fmttime (timeinsecs)
time_t *timeinsecs;
{
	static char buffer [40];	/* Output buffer */
	char *tzn;			/* Time zone name */
	char *ts;			/* Time string */
	struct tm *tp;			/* Time structure pointer */

#if	SGI | SYS52 | UTS | UNICOS  | OSF
	tzset();			/* Get local timezone */
	tp = localtime (timeinsecs);
	ts = asctime (tp);
	tzn = tzname [tp->tm_isdst];
#else
#if	BSD42 | BSD43 | ULTRIX 
	struct	timeval	tv;
	struct	timezone tz;

	gettimeofday (&tv, &tz);
	tp = localtime (timeinsecs);
	ts = asctime (tp);
	tzn = timezone (tz.tz_minuteswest, tp->tm_isdst);
#else
BAD SYSTEM TYPE
#endif
#endif
	if (tzn == (char *) 0) tzn = "";
	ts [24] = '\0';				/* Zap newline character */
	sprintf (buffer, "%.20s%s%s", ts, tzn, ts+19);
	return (buffer);
}
