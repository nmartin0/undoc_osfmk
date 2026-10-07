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
static char rcsid[] = "@(#)$RCSfile: utime.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:25 $";
#endif

/*
 * COMPONENT_NAME: LIBCFS - C Library File System Interfaces
 *
 * FUNCTIONS: utime
 *
 * ORIGINS: 27, 3, 26
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
 * utime.c	1.8  com/lib/c/fs,3.1,8943A 11/8/89 01:03:16
 */

#include <sys/time.h>
#include <unistd.h>
#include <utime.h>

/*
 * utime()
 *
 * changes the modification and access times on a file
 * (this is a compatibility routine that invokes the utimes system call)
 */

utime(char *fname, struct utimbuf *times)
{
	struct timeval tvp[2];

	if (times)
	{
		/* intialize the variables */
		tvp[0].tv_sec  = times->actime;
		tvp[0].tv_usec = 0;

		tvp[1].tv_sec  = times->modtime;
		tvp[1].tv_usec = 0;
	}

	return(utimes(fname, times ? tvp : (struct timeval *)NULL));
}
