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
static char rcsid[] = "@(#)$RCSfile: pathconf.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:07:20 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 *
 * static char sccsid[] = "@(#)pathconf.c	1.14  com/lib/c/gen,3.1,9021 3/21/90 17:33:11";
 */
/*
 * FUNCTIONS: pathconf, fpathconf
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

#include <limits.h>
#include <unistd.h>
#include <termios.h>
#include <ulimit.h> 
#include <sys/mount.h>
#include <errno.h>
#include <sys/stat.h>

/*
 * PURPOSE:
 *	The pathconf() and fpathconf() functions provide a method for the
 *      application to determine the current value of a configurable system
 *      limit or option that is associated with a file or directory.
 *	These variables are found in <limits.h> or <unistd.h>.
 *      The 'name' argument represents the system variable to be queried
 * 	relative to that file or directory.  For pathconf(), the 'path'
 * 	arguments points to the pathname of a file or directory.  For
 * 	fpathconf(), the 'fildes' arguments is an open file descriptor.
 *
 * RETURNS:
 * 	If 'name' is an invalid value, a -1 is returned.  If the
 *	variable corresponding to the 'name' value is not defined
 * 	on the system, a -1 is returned without changing errno.  Otherwise
 *      the pathconf() and fpathconf functions will return the current
 *	variable value for the file or directory.
 *
 * ERRORS:
 *	pathconf() and fpathconf():
 *	EINVAL		The value of the 'name' argument is invalid.
 *
 *	pathconf():
 *	EACCES		Search permission is denied for a component of the 
 *			path prefix.
 *	ENAMETOOLONG	The length of the path argument exceeds PATH_MAX
 *	ENOENT		The named file does not exist or the 'path' arguments
 *			points to an empty string.
 *	ENOTDIR		A component of the path prefix is not a directory.
 *
 *	fpathconf():
 *	EBADF		The 'fildes' argument is not a valid file descriptor.
 *
 */

long 
pathconf( char *path, int name)
{

	extern int statfs();
	struct statfs statfsbuf;
	int u;

	/*
	 * If the current process does not have search permission on a
	 * component of the path prefix, return -1.  Also, retrive the
	 * the value of NAME_MAX.    
	 */
	if (statfs(path, &statfsbuf) < 0)
		return(-1);

	if (path == NULL)
	{
#ifdef	_THREAD_SAFE
		seterrno(ENOENT);
#else
		errno = ENOENT;
#endif
		return(-1);
	}

	switch (name)
	 {
/*  f_name_max is a newly added field to the statfs structure.  It contains
 *  the value of NAME_MAX for a particular filesystem.  Currently, NAME_MAX
 *  is undefined in limits.h because of multiple filesystems.
 */

		case _PC_NAME_MAX:
/*XXXXXXXX  This is just a temporary fix until kernel group does new
	    statfs system call and structure!!

			if(chk_perm(path) == -1)
				return(-1);
			else
				return(statfsbuf.f_name_max); 
*XXXXXXXXX   the next line will be deleted when the *real* fix is done */
			return(NAME_MAX);

		case _PC_LINK_MAX:
#ifndef LINK_MAX
		 	return(-1);
#else
			return(LINK_MAX);
#endif

/* It is possible that in the future MAX_CANON and MAX_INPUT may have dynamic
 * values according to the particular filesystem.  Currently these values
 * are static.
 */

		case _PC_MAX_CANON: 
#ifndef MAX_CANON
			return(-1);
#else
			return(MAX_CANON);
#endif
		case _PC_MAX_INPUT: 
#ifndef MAX_INPUT
			return(-1);
#else
			return(MAX_INPUT);
#endif


		case _PC_PATH_MAX:
#ifndef PATH_MAX
			return(-1);
#else
			return(PATH_MAX);
#endif
		case _PC_PIPE_BUF:
#ifndef PIPE_BUF
			return(-1);
#else
			return(PIPE_BUF);
#endif

		case _PC_CHOWN_RESTRICTED:
#ifdef _POSIX_CHOWN_RESTRICTED
		 	return(_POSIX_CHOWN_RESTRICTED);
#else
#error _POSIX_CHOWN_RESTRICTED CANNOT BE UNDEFINED
#endif


		case _PC_NO_TRUNC: 	
#ifdef _POSIX_NO_TRUNC
			return(_POSIX_NO_TRUNC);	
#else
#error _POSIX_NO_TRUNC CANNOT BE UNDEFINED
#endif

		case _PC_VDISABLE:
#ifdef _POSIX_VDISABLE
			return(_POSIX_VDISABLE);
#else
#error _POSIX_VDISABLE CANNOT BE UNDEFINED
#endif

 	default:
#ifdef	_THREAD_SAFE
		seterrno(EINVAL);
#else
		errno = EINVAL;
#endif
		return(-1);

	}
}
  


long 
fpathconf(int fildes, int name)
{

	extern int fstatfs();
	struct statfs fstatfsbuf;
	int u;

	if (fstatfs(fildes, &fstatfsbuf) < 0)
		return(-1);
	switch (name)
	 {
		case _PC_NAME_MAX:
/*XXXXXXXX  This is just a temporary fix until kernel group does new
	    statfs system call and structure!!
			if (fstatfs(fildes, &fstatfsbuf) < 0)
				return(-1);

                        if (fchk_perm(fildes)== -1)
                                return(-1);
                        else
				return(fstatfsbuf.f_name_max); 
*XXXXXXXXX   the next line will be deleted when the *real* fix is done */
			return(NAME_MAX);


		case _PC_LINK_MAX:
#ifndef LINK_MAX
		 	return(-1);
#else
			return(LINK_MAX);
#endif

/* It is possible that in the future the following two values may not be 
 * set values and they may change from filesystem to filesystem.  Currently,
 * these values are set.
 */

		case _PC_MAX_CANON: 
#ifndef MAX_CANON
			return(-1);
#else
			return(MAX_CANON);
#endif
		case _PC_MAX_INPUT: 
#ifndef MAX_INPUT
			return(-1);
#else
			return(MAX_INPUT);
#endif


		case _PC_PATH_MAX:
#ifndef PATH_MAX
			return(-1);
#else
			return(PATH_MAX);
#endif
		case _PC_PIPE_BUF:
#ifndef PIPE_BUF
			return(-1);
#else
			return(PIPE_BUF);
#endif

		case _PC_CHOWN_RESTRICTED:
#ifdef _POSIX_CHOWN_RESTRICTED
		 	return(_POSIX_CHOWN_RESTRICTED);
#else
#error _POSIX_CHOWN_RESTRICTED CANNOT BE UNDEFINED
#endif


		case _PC_NO_TRUNC: 	
#ifdef _POSIX_NO_TRUNC
			return(_POSIX_NO_TRUNC);
#else
#error _POSIX_NO_TRUNC CANNOT BE UNDEFINED
#endif

		case _PC_VDISABLE:
#ifdef _POSIX_VDISABLE
			return(_POSIX_VDISABLE);
#else
#error _POSIX_VDISABLE CANNOT BE UNDEFINED
#endif

 	default:
#ifdef	_THREAD_SAFE
		seterrno(EINVAL);
#else
		errno = EINVAL;
#endif
		return(-1);

	}
}

/*
 If the current process does not have the appropriate privileges
 to query the file named by path, return a -1.
*/

chk_perm(path)
char *path;
{
	struct stat pathbuf;

	if (stat(path, &pathbuf) == -1)
		return(-1);
	if ((pathbuf.st_mode & S_IRUSR) == S_IRUSR)
		return(0);
}

fchk_perm(fildes)
int fildes;
{
	struct stat fpathbuf;

	if (fstat(fildes, &fpathbuf) == -1)
		return(-1);
	if ((fpathbuf.st_mode & S_IRUSR) == S_IRUSR)
		return(0);
}
