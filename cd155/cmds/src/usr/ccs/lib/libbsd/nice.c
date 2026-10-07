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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)nice.c	1.4  com/lib/bsd,3.1,8943 10/5/89 09:23:07";
/*
 * COMPONENT_NAME: (LIBBSD)  Berkeley Compatibility Library
 *
 * FUNCTIONS: nice
 *
 * ORIGINS: 26 27
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
 */

#include <sys/time.h>
#include <sys/param.h>
#include <sys/resource.h>
#include <sys/errno.h>

/*
 * NAME: nice
 *                                                                    
 * FUNCTION:  Returns nice value in range [-20..20]
 *                                                                    
 * NOTES:  get/setpriority are the kernel (libc) system call stubs.
 *
 * RETURNS:  Returns the nice value; returns -1 if an error occurs and
 *	     sets errno to the approrpriate error code.
 */

nice(incr)
	int incr;
{
	int prio;
	int saverr;

	saverr = errno;
	errno = 0;
	prio = getpriority(PRIO_PROCESS, 0);
	if (prio == -1 && errno)
		return (-1);

	errno = saverr;
	if (setpriority(PRIO_PROCESS, 0, prio + incr) < 0)
		return (-1);
	else
		return (getpriority(PRIO_PROCESS, 0));
}
