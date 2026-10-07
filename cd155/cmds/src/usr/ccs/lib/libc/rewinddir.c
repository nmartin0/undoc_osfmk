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
static char rcsid[] = "@(#)$RCSfile: rewinddir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:53 $";
#endif
/*
 * FUNCTIONS: rewinddir 
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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * rewinddir.c	1.2  com/lib/c/io,3.1,8943 9/9/89 13:31:58
 */

/*
 * FUNCTION:
 *	resets the positiion of the directory stream to which 'dirp' refers
 *	to the beginning of the directory.
 *
 * RETURNS:
 *	rewinddir() does not return a value.
 *
 */

#include <sys/param.h>
#include <dirent.h>
#include <sys/file.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"
#endif

#ifdef rewinddir
#undef rewinddir
#endif

/*
 * Since seekdir is unsafe, we need an explicit routine for rewinddir
 */
void
rewinddir(DIR *dirp)

{
	long curloc, base, offset;
	struct direct *dp;
	extern long lseek();

	if (dirp == NULL || dirp->dd_buf == NULL)
		return;
#ifdef _THREAD_SAFE
	rec_mutex_lock(dirp->dd_lock);
#endif
	dirp->dd_loc = 0;
	(void) lseek(dirp->dd_fd, (off_t)0, L_SET);
#ifdef _THREAD_SAFE
	rec_mutex_unlock(dirp->dd_lock);
#endif
}
