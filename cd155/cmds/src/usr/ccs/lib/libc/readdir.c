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
static char rcsid[] = "@(#)$RCSfile: readdir.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:07:42 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
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
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * readdir.c	5.4 (Berkeley) 6/18/88
 */


#include <sys/param.h>
#include <dirent.h>
#include <errno.h>

#ifdef _THREAD_SAFE
#include "rec_mutex.h"
#include <memory.h>
#define SUCCESS 0
#define FAIL -1
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#define FAIL NULL
#endif /* _THREAD_SAFE */

/*
 * get next entry in a directory.
 */
#ifdef _THREAD_SAFE
int 
readdir_r(dirp, dirent)
	register DIR *dirp;
	struct dirent *dirent;

#else /* _THREAD_SAFE */

struct dirent *
readdir(dirp)
	register DIR *dirp;

#endif /* _THREAD_SAFE */
{
	register struct dirent *dp;
	long offset;

	if (dirp == NULL || dirp->dd_buf == NULL) {
	  	SETERR(EBADF);
		return (FAIL);
	}
#ifdef _THREAD_SAFE
	rec_mutex_lock(dirp->dd_lock);
#endif
	for (;;) {
		if (dirp->dd_loc == 0) {
			dirp->dd_size = getdirentries(dirp->dd_fd,
						      dirp->dd_buf, 
						      dirp->dd_bufsiz,
						      &offset);
			if (dirp->dd_size <= 0) {
#ifdef _THREAD_SAFE
				rec_mutex_unlock(dirp->dd_lock);
#endif
				return(FAIL);
			}
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = 0;
			continue;
		}
		dp = (struct dirent *)(dirp->dd_buf + dirp->dd_loc);
		if ((int)dp & 03) {	/* bogus pointer check */
#ifdef _THREAD_SAFE
			rec_mutex_unlock(dirp->dd_lock);
#endif
			SETERR(EFAULT);
			return(FAIL);
		}
		if (dp->d_reclen <= 0 ||
		    dp->d_reclen > dirp->dd_bufsiz + 1 - dirp->dd_loc) {
#ifdef _THREAD_SAFE
			rec_mutex_unlock(dirp->dd_lock);
#endif
			return(FAIL);
		}
		dirp->dd_loc += dp->d_reclen;
		if (dp->d_fileno == 0)
			continue;
#ifdef _THREAD_SAFE
		memcpy((void *)dirent, (void *)dp, sizeof(struct dirent));
		rec_mutex_unlock(dirp->dd_lock);
		return (SUCCESS);
#else
		return (dp);
#endif
	}
}
