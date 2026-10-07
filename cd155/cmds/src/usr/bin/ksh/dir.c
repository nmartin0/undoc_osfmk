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
static char rcsid[] = "@(#)$RCSfile: dir.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:12 $";
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
#include <sys/errno.h>

# define FAIL NULL

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

	if ((fd = open((char *) name, 0)) == -1)
		return(NULL);
	(void) fcntl(fd, F_SETFD, FD_CLOEXEC);

	if (fstat(fd, &st) < 0 || (st.st_mode&S_IFMT) != S_IFDIR) {
		close(fd);
		return(NULL);
	}
	if ((dirp = (DIR *)malloc(sizeof(DIR))) == NULL) {
		close(fd);
		return(NULL);
	}
	dirp->dd_bufsiz = MAX(st.st_blksize, MAXBSIZE);
	if ((dirp->dd_buf = (char *)malloc(dirp->dd_bufsiz)) == NULL) {
		(void)free((char *)dirp);
		close(fd);
		return(NULL);
	}
	dirp->dd_fd = fd;
	dirp->dd_loc = 0;
	return(dirp);
}


/*
 * get next entry in a directory.
 */
struct dirent *
readdir(dirp)
	register DIR *dirp;

{
	register struct dirent *dp;
	long offset;

	if (dirp == NULL || dirp->dd_buf == NULL) {
	  	errno = EBADF;
		return (FAIL);
	}
	for (;;) {
		if (dirp->dd_loc == 0) {
			dirp->dd_size = getdirentries(dirp->dd_fd,
						      dirp->dd_buf, 
						      dirp->dd_bufsiz,
						      &offset);
			if (dirp->dd_size <= 0) {
				return(FAIL);
			}
		}
		if (dirp->dd_loc >= dirp->dd_size) {
			dirp->dd_loc = 0;
			continue;
		}
		dp = (struct dirent *)(dirp->dd_buf + dirp->dd_loc);
		if ((int)dp & 03) {	/* bogus pointer check */
			errno = EFAULT;
			return(FAIL);
		}
		if (dp->d_reclen <= 0 ||
		    dp->d_reclen > dirp->dd_bufsiz + 1 - dirp->dd_loc) {
			return(FAIL);
		}
		dirp->dd_loc += dp->d_reclen;
		if (dp->d_fileno == 0)
			continue;
		return (dp);
	}
}


/*
 * close a directory.
 */
int
closedir(DIR *dirp)
{
        if (dirp->dd_fd >= 0) {
		close(dirp->dd_fd);
		dirp->dd_fd = -1;
		dirp->dd_loc = 0;
		dirp->dd_size = 0; /* so readdir will fail */
		free(dirp->dd_buf);
		free(dirp);
		return(0);
	}
	else {	/* dirp does not refer to an open directory stream */
		errno = EBADF;
		return(-1);
	}
}
