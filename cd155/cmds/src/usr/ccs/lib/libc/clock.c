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
static char rcsid[] = "@(#)$RCSfile: clock.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 02:04:00 $";
#endif
/*
 * FUNCTIONS:  clock
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * clock.c	1.12  com/lib/c/gen,3.1,8943 9/8/89 08:39:13
 */

#include <sys/times.h>
#include <time.h>		/* for CLK_TCK (clock ticks per second) */

#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex	_clock_rmutex;
#endif

/*
 * TIMES computes full amount of cpu time used including user, sys, child user,
 * and child sys times...
 */
#define TIMES(B)	(B.tms_utime+B.tms_stime+B.tms_cutime+B.tms_cstime)

static clock_t first = (time_t) -1; /* cpu time used after first call	*/

/*
 * NAME:	clock
 *
 * FUNCTION:	clock - return CPU time used
 *
 * NOTES:	Clock returns the amount of CPU time used (in
 *		microseconds) since the first call to clock.
 *
 * RETURN VALUE DESCRIPTION:	mount of CPU time used (in
 *		microseconds) since the first call to clock
 */

clock_t
clock(void)
{
	struct tms buffer;
#ifdef _THREAD_SAFE
	clock_t	return_val;

	rec_mutex_lock(&_clock_rmutex);
#endif

	/* set first if first time in */
	if((times(&buffer) != (clock_t)(-1)) && (first == (clock_t)(-1)))
		first = TIMES(buffer);

	/*
	 * compute difference, convert to microseconds.
	 * struct tms elements are in CLK_TCK's...
	 */
#ifdef _THREAD_SAFE
	return_val = (TIMES(buffer) - first) * (clock_t)(1000000L/CLK_TCK);
	rec_mutex_unlock(&_clock_rmutex);
	return(return_val);
#else
	return((TIMES(buffer) - first) * (clock_t)(1000000L/CLK_TCK));
#endif
}
