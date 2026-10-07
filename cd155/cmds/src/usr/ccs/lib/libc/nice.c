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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: nice.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:07:16 $";
#endif
/*
 * FUNCTIONS: nice
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * nice.c	1.6  com/lib/c/gen,3.1,8943 10/5/89 09:21:52
 */

#include <sys/time.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <errno.h>

#ifdef _THREAD_SAFE
#include "rec_mutex.h"
extern struct rec_mutex	_nice_rmutex;
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 *
 * FUNCTION: Nice adds an increment value to the nice value of the
 *	calling process.  Nice will fail and not change the nice value if
 *	the increment value is negative and the effective user ID of the
 *	calling process does not have SET_PROC&US1.PRIORITY system privilege.
 *	Nice is restricted to setting priorities in the range 0 to 39.
 *
 * PARAMETERS:
 *	incr	- an integer value by which the process's nice value
 *		  is changed.
 *
 * NOTES: The setpriority system call does the hard work of checking
 *	privilege, enforcing system limits, and changing the process's
 *	nice value.
 *
 * RETURN VALUE DESCRIPTION: Nice returns the new nice value minus NZERO.
 */                                                                   

/*
 *
 * get/setpriority deal in range [-20..+20] as does the kernel.
 */
nice(incr)
	int incr;
{
	int prio;
	int saverr;
#ifdef _THREAD_SAFE
	int rc;

	rec_mutex_lock(&_nice_rmutex);
#endif

	saverr = errno;
	SETERR(0);
	prio = getpriority(PRIO_PROCESS, 0);	/* In range of [-20..+20] */
	if (prio == -1 && errno)
		return (-1);

	SETERR(saverr);

	/*
	 * we do this test here as the kernel will effectively range
	 * check 0..40 (ie 41 nice values). This catches trying to nice
	 * to 40 and limits it to 39.
	 */
	if (prio + incr >= PRIO_MAX - 1)
		incr = PRIO_MAX - 1 - prio;
	/*
	 * Let the kernel do the rest of the range checking.
	 */
	if (setpriority(PRIO_PROCESS, 0, prio + incr) < 0)  {
		if (errno == EACCES)
			SETERR(EPERM);
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_nice_rmutex);
#endif
		return (-1);
	} else {
#ifdef _THREAD_SAFE
		rc = getpriority(PRIO_PROCESS, 0) - PRIZERO;
		rec_mutex_unlock(&_nice_rmutex);
		return(rc);
#else
		return (getpriority(PRIO_PROCESS, 0) - PRIZERO);
#endif
	}
}
