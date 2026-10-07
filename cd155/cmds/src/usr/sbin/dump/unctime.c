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
/*
 * HISTORY
 * $Log: unctime.c,v $
 * Revision 1.3  1994/11/19  03:05:43  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/12  21:46:24  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.7  90/10/07  22:08:36  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:09:51  gm]
 * 
 * Revision 2.6  90/08/25  12:29:10  devrcs
 * 	insert rcsid string;
 * 	delete old sccs id string;
 * 	[90/08/09  03:59:45  nm]
 * 
 * Revision 2.5  90/07/27  11:19:52  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:39:18  nm]
 * 
 * Revision 2.4  90/06/22  22:12:11  devrcs
 * 	integration to osc.12
 * 	[90/05/31  04:39:18  nm]
 * 
 * Revision 2.3  90/04/14  00:13:08  devrcs
 * 	integration of Jim's code
 * 	[90/04/10  08:13:30  nm]
 * 
 * 	Merged AIX3.1 and BSD4.3 to one source.
 * 	Made major changes fixing that code up. (Inserted comments too!)
 * 	Merged (BSD4.3 --> CMU) changes with what resulted.
 * 	Disabled edump (cannot quite figure out what it wants to do,
 * 		also not supported by rmt).
 * 	[90/01/11            walkerj]
 * 
 * Revision 2.2  90/01/02  18:41:37  gm
 * 	Fixes for first snapshot.
 * 	[90/01/02            gm]
 * 
 * 	Current version from CMU.
 * 	[89/12/21            gm]
 * 
 * $EndLog$
 */
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: unctime.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 03:05:43 $";
#endif

/*
 * This module contains IBM CONFIDENTIAL code. -- (IBM Confidential Restricted
 * when combined with the aggregated modules for this product) OBJECT CODE ONLY
 * SOURCE MATERIALS (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or disclosure
 * restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1980 Regents of the University of California. All
 * rights reserved.  The Berkeley software License Agreement specifies the
 * terms and conditions for redistribution.
 */

#include	<sys/types.h>
#include	<sys/time.h>
#include	<stdio.h>

/*
 * Convert a ctime(3) format string into a system format date. Return the date
 * thus calculated.
 *
 * Return -1 if the string is not in ctime format.
 */

/*
 * Offsets into the ctime string to various parts.
 */

#define	E_MONTH		4
#define	E_DAY		8
#define	E_HOUR		11
#define	E_MINUTE	14
#define	E_SECOND	17
#define	E_YEAR		20

static int		dcmp();
static int		lookup();
static time_t		emitl();

time_t
unctime(str)
	char	       *str;
{
	struct tm	then;

	if ((then.tm_mon = lookup(&str[E_MONTH])) < 0)
	{
		return(-1);
	}
	then.tm_mday = atoi(&str[E_DAY]);
	then.tm_hour = atoi(&str[E_HOUR]);
	then.tm_min = atoi(&str[E_MINUTE]);
	then.tm_sec = atoi(&str[E_SECOND]);
	then.tm_year = atoi(&str[E_YEAR]) - 1900;
	return(emitl(&then));
}

static char	months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

static int
lookup(str)
	register char  *str;
{
	register char  *cp;

	for (cp = months; *cp != '\0'; cp += 3)
	{
		if (strncmp(cp, str, 3) == 0)
		{
			return((cp - months) / 3);
		}
	}
	return(-1);
}

/*
 * Routine to convert a localtime(3) format date back into a system format
 * date.
 *
 * Use a binary search.
 */


static time_t
emitl(dp)
	struct tm      *dp;
{
	time_t		conv;
	register int	i, bit;
	struct tm      *localtime();

	conv = 0;
	for (i = 30; i >= 0; --i)
	{
		bit = 1 << i;

		/* set the bit */

		conv |= bit;

		/* if this new time, conv, is greater than the
		 * one being tested */

		if (dcmp(localtime(&conv), dp) > 0)
		{
			/* unset the bit again */

			conv &= ~bit;
		}
	}
	return(conv);
}

/*
 * Compare two localtime dates, return result.
 */

#define	DECIDE(a) \
	if (dp1->a > dp2->a)	\
	{			\
		return(1);	\
	}			\
	if (dp1->a < dp2->a)	\
	{			\
		return(-1);	\
	}

static int
dcmp(dp1, dp2)
	register struct tm     *dp1, *dp2;
{

	DECIDE(tm_year);
	DECIDE(tm_mon);
	DECIDE(tm_mday);
	DECIDE(tm_hour);
	DECIDE(tm_min);
	DECIDE(tm_sec);
	return(0);
}
