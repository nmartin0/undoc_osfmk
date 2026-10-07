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
 
/*++ getwdir.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnqs/getwdir.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	Get current working directory (that should work on both
 *	System V and Berkeley 4.2 systems).  Note that things
 *	certainly would have been easier, if we had just used the
 *	standard getcwd() call of System V.  The only problem is
 *	that the System V version of getcwd() is implemented by
 *	using popen() to pipe the output of the pwd command into
 *	a returned buffer space, and I got tired of waiting so
 *	long.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:26:53 $ $State: Exp $)
 * $Log: getwdir.c,v $
 * Revision 1.3  1994/11/19  02:26:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  20:16:34  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  16:49:22  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:47  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:46:15  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined SCCS
static char     sccs_id[] = "@(#)getwdir.c	1.2 (getwdir.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif

#if OSF
#include "sys_types_OSF.h"      /* in OSF sys_types_OSF.h */
#else
#include <sys/types.h>
#endif

#include <sys/stat.h>
#if	UNICOS | SGI | SYS52 | UTS | OSF
#include "dir.4.2.h"			/* Directory walker emulations */
#else
#if BSD42 | BSD43 | ULTRIX 
#include <sys/dir.h>			/* Directory walking */
#else
BAD SYSTEM TYPE
#endif
#endif
#define	MAXPATHLEN	1500		/* Maximum path length possible */
					/* for getworkdir()  */

extern int strlen();

/*** getwdir
 *
 *
 *	char getwdir():
 *
 *	Returns:
 *		A pointer to the fully qualified path name of the
 *		current working directory if successful;
 *		Otherwise, NULL is returned (an error occurred).
 *
 *		WARNING:
 *			If NULL is returned indicating that we were
 *			unsuccessful in determining the current working
 *			directory, then the current working directory of
 *			the caller may, or may NOT be the same directory
 *			as when this function was invoked!!!!!
 *
 *			In other words, if this function ever returns
 *			NULL, then the caller should bail out.
 */
char *getwdir ()
{
	static char path [MAXPATHLEN + 1];
	struct stat child;
	struct stat parent;
	struct stat walkparent;
	DIR *dir;
	struct direct *dirent;
	register char *ptrpath;
	register int i;

	ptrpath = path + MAXPATHLEN;
	*ptrpath-- = '\0';			/* Null terminate */
	if (stat (".", &child) == -1) {
		/* stat() failed */
		return (NULL);
	}
	for(;;) {
		if ((dir = opendir ("..")) == NULL) {
			/* opendir() failed */
			if (*++ptrpath != '\0') chdir (++ptrpath);
			return (NULL);
		}
		if (fstat (dir->dd_fd, &parent) == -1) {
			/* fstat() failed */
			if (*++ptrpath != '\0') chdir (++ptrpath);
			return (NULL);
		}
		if (chdir ("..") == -1) {
			/*  chdir() failed. */
			if (*++ptrpath != '\0') chdir (++ptrpath);
			return (NULL);
		}
		if (child.st_dev == parent.st_dev) {
			/*
			 *  The parent directory is on the same device as
			 *  the child directory.
			 */
			if (child.st_ino == parent.st_ino) {	/* At root */
				/*
				 *  Restore to the original working directory
				 *  and return to caller.
				 */
				if (*++ptrpath == '\0') {
					/*
					 *  We were at '/'.
					 */
					*--ptrpath = '/';
				}
				else if (chdir (ptrpath) == -1) return (NULL);
				return (ptrpath);
			}
			do {	/* Loop to locate parent's name */
				if ((dirent = readdir (dir)) == NULL) {
					/* Read error */
					if (*++ptrpath != '\0') {
						chdir (++ptrpath);
					}
					return (NULL);
				}
			} while (dirent->d_ino != child.st_ino);
		}
		else {
			/*
			 *  The parent directory is on a different device
			 *  than the child directory.
			 */
			do {
				if ((dirent = readdir (dir)) == NULL) {
					/* Read error */
					if (*++ptrpath != '\0') {
						chdir (++ptrpath);
					}
					return (NULL);
				}
				stat (dirent->d_name, &walkparent);
			} while (walkparent.st_ino != child.st_ino ||
				 walkparent.st_dev != child.st_dev);
		}
		closedir (dir);
		i = strlen (dirent->d_name);
		if (ptrpath - path < i) {
			/*
			 *  The full path name is simply too long.
			 */
			if (*++ptrpath != '\0') chdir (++ptrpath);
			return (NULL);
		}
		while (i > 0) *ptrpath-- = dirent->d_name [--i];
		*ptrpath-- = '/';
		/*
		 *  We did a chdir() to the parent directory, so now
		 *  the child should be the parent here for the next
		 *  iteration of the loop.
		 */
		child = parent;
	}
}
