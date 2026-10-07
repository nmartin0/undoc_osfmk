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
static char rcsid[] = "@(#)$RCSfile: opendir.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:07:20 $";
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
 * opendir.c	5.3 (Berkeley) 6/18/88
 */


#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <errno.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"
#endif

/*
 * open a directory.
 */
DIR *
opendir(const char *name)
{
	register DIR *dirp;
	register int fd;
	struct stat st;
	int pid;

	if ((fd = open((char *) name, O_RDONLY)) == -1)
		return(NULL);
	(void) fcntl(fd, F_SETFD, FD_CLOEXEC);

	if (fstat(fd, &st) < 0 || (st.st_mode&S_IFMT) != S_IFDIR) {
		close(fd);
#ifdef _THREAD_SAFE
		seterrno(ENOTDIR);
#else
		errno = ENOTDIR;
#endif
		return(NULL);
	}
	if ((dirp = (DIR *)malloc(sizeof(DIR))) == NULL) {
		close(fd);
		return(NULL);
	}
	dirp->dd_bufsiz = MAX(st.st_blksize, MAXDIRBUF);
	if ((dirp->dd_buf = (char *)malloc(dirp->dd_bufsiz)) == NULL) {
		(void)free((char *)dirp);
		close(fd);
		return(NULL);
	}
#ifdef _THREAD_SAFE
	if (rec_mutex_alloc((rec_mutex_t *)&dirp->dd_lock) < 0) {
		(void)free((char *)dirp->dd_buf);
		(void)free((char *)dirp);
		close(fd);
		return(NULL);
	}
#endif
	dirp->dd_fd = fd;
	dirp->dd_loc = 0;
	return(dirp);
}
