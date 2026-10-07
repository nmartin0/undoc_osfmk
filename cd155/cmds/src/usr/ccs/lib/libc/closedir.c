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
static char rcsid[] = "@(#)$RCSfile: closedir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:00 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: closedir 
 *
 * ORIGINS: 26, 27 
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
 * sccsid[] = "closedir.c        1.12  com/lib/c/io,3.1,9021 4/24/90 15:55:09";
 */

#include <sys/limits.h>
#include <sys/types.h>
#include <dirent.h>	
#include <errno.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"
#endif

/*
 * close a directory.
 */
int
closedir(DIR *dirp)
{
#ifdef _THREAD_SAFE
        rec_mutex_t     lock;

        rec_mutex_lock(dirp->dd_lock);
#endif
        if ((dirp->dd_fd >= 0) && close(dirp->dd_fd) == 0) {
		dirp->dd_fd = -1;
		dirp->dd_loc = 0;
		dirp->dd_size = 0; /* so readdir will fail */
		free(dirp->dd_buf);
#ifdef _THREAD_SAFE
                lock = (rec_mutex_t)dirp->dd_lock;
#endif
		free(dirp);
#ifdef _THREAD_SAFE
                rec_mutex_unlock(lock);
                rec_mutex_free(lock);
#endif
		return(0);
	}
	else {	/* dirp does not refer to an open directory stream */
#ifdef _THREAD_SAFE
                rec_mutex_unlock(dirp->dd_lock);
		seterrno(EBADF);
#else
		errno = EBADF;
#endif
		return(-1);
	}
}
