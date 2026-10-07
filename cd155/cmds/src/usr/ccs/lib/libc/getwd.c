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
static char rcsid[] = "@(#)$RCSfile: getwd.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:06:01 $";
#endif
/*
 * FUNCTIONS: getwd
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * getwd.c	1.8  com/lib/c/gen,3.1,8943 10/12/89 09:03:59
 */

#include <limits.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 *
 * FUNCTION: returns the pathname of the current working directory. 
 *
 * RETURN VALUE DESCRIPTION: On error an error message is copied to pathname
 *	and a NULL pointer is returned.
 */
/*LINTLIBRARY*/

#ifdef MSG
#include "libc_msg.h"
#define GETWDERR(s,n)	strcpy(pathname, NLgetamsg(MF_LIBC,MS_LIBC,n,s))
#else
#define GETWDERR(s,n)	strcpy(pathname, (s));
#endif

static char *prepend();			/* prepend dirname to pathname */

char *
getwd(char *pathname)
{
	char pathbuf[PATH_MAX];		/* temporary pathname buffer */
	char *pnptr = &pathbuf[(sizeof pathbuf)-1]; /* pathname pointer */
	char curdir[PATH_MAX];		/* current directory buffer */
	char *dptr = curdir;		/* directory pointer */
	long cdev, rdev;		/* current & root vfs id */
	ino_t cino, rino;		/* current & root inode number */
	DIR *dirp;			/* directory stream */
	struct dirent *dir;		/* directory entry struct */
	struct stat d, dd;		/* file status struct */
	int pathsize;			/* pathname length */

	pathsize = 0;
	*pnptr = '\0';
	if (stat("/", &d) < 0) {
		GETWDERR("getwd: can't stat /", M_RSTAT);
		return (NULL);
	}
	rdev = d.st_dev;
	rino = d.st_ino;
	strcpy(dptr, "./");
	dptr += 2;
	if (stat(curdir, &d) < 0) {
		GETWDERR("getwd: can't stat .", M_HSTAT);
		return (NULL);
	}

	for (;;) {
		if (d.st_ino == rino && d.st_dev == rdev)
			break;		/* reached root directory */
		cino = d.st_ino;
		cdev = d.st_dev;
		strcpy(dptr, "../");
		dptr += 3;
		if ((dirp = opendir(curdir)) == NULL) {
			GETWDERR("getwd: can't open ..", M_OPENPAR);
			return (NULL);
		}
		fstat(dirp->dd_fd, &d);
		if (cdev == d.st_dev) {
			if (cino == d.st_ino) {
				/* reached root directory */
				closedir(dirp);
				break;
			}
			do {
				if ((dir = readdir(dirp)) == NULL) {
					closedir(dirp);
					GETWDERR("getwd: read error in ..",M_READPAR);
					/* readdir() needn't set errno for EOF*/
					if (!errno)
						SETERR(ENOENT);
					return (NULL);
				}
			} while (dir->d_fileno != cino);
		} else
			do {
				if ((dir = readdir(dirp)) == NULL) {
					closedir(dirp);
					GETWDERR("getwd: read error in ..",M_READPAR);
					/* readdir() needn't set errno for EOF*/
                                        if (!errno)
                                                SETERR(ENOENT);
					return (NULL);
				}
				strcpy(dptr, dir->d_name);
				lstat(curdir, &dd);
			} while(dd.st_ino != cino || dd.st_dev != cdev);
		pnptr = prepend(dir->d_name, pnptr, &pathsize);
		pnptr = prepend("/", pnptr, &pathsize);
		closedir(dirp);
	}
	if (*pnptr == '\0')		/* current dir == root dir */
		strcpy(pathname, "/");
	else
		strcpy(pathname, pnptr);
	return (pathname);
}

/*
 * prepend() tacks a directory name onto the front of a pathname.
 */
static char *
prepend(dirname, pathname, pathsize)
	register char *dirname;
	register char *pathname;
	register int  *pathsize;
{
	register int i;			/* directory name size counter */

	for (i = 0; *dirname != '\0'; i++, dirname++)
		continue;
	if (((*pathsize) += i) < PATH_MAX)
		while (i-- > 0)
			*--pathname = *--dirname;
	return (pathname);
}
