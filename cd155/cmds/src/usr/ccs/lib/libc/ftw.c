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
static char rcsid[] = "@(#)$RCSfile: ftw.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:05:17 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
static char sccsid[] = "@(#)ftw.c	1.9  com/lib/c/gen,3.1,8943 10/11/89 16:58:04";
 */ 
/*
 * FUNCTIONS: ftw
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <dirent.h>
#include <errno.h>
#include <ftw.h>
#include <string.h>

extern char *malloc();
extern long lseek();
extern int errno;

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 * NAME:	ftw
 *                                                                    
 * FUNCTION:	ftw - file tree walk
 *                                                                    
 * NOTES:
 *	Given a path name, ftw starts from the file given by that path
 *	name and visits each file and directory in the tree beneath
 *	that file.  Any symbolic links in the *inital* path will
 *	be traversed, including the final component (per the OSF/AES).
 *	Links encountered while walking the directory tree will not
 *	be traversed. If a single file has multiple (hard)links within the
 *	structure, it will be visited once for each such link.
 *	For each object visited, fn is called with three arguments.
 *	The first contains the path name of the object, the second
 *	contains a pointer to a stat buffer which will usually hold
 *	appropriate information for the object and the third will
 *	contain an integer value giving additional information about
 *
 *		FTW_F	The object is a file for which stat was
 *			successful.  It does not guarantee that the
 *			file can actually be read.
 *
 *		FTW_D	The object is a directory for which stat and
 *			open for read were both successful.
 *
 *		FTW_DNR	The object is a directory for which stat
 *			succeeded, but which cannot be read.  Because
 *			the directory cannot be read, fn will not be
 *			called for any descendants of this directory.
 *
 *		FTW_SL	The object is a symbolic link for which the
 *			lstat was successful.
 *
 *		FTW_NS	Stat failed on the object because of lack of
 *			appropriate permission.  This indication will
 *			be given, for example, for each file in a
 *			directory with read but no execute permission.
 *			Because stat failed, it is not possible to
 *			determine whether this object is a file or a
 *			directory.  The stat buffer passed to fn will
 *			contain garbage.  Stat failure for any reason
 *			other than lack of permission will be
 *			considered an error and will cause ftw to stop 
 *			and return -1 to its caller.
 *
 *	If fn returns nonzero, ftw stops and returns the same value
 *	to its caller.  If ftw gets into other trouble along the way,
 *	it returns -1 and leaves an indication of the cause in errno.
 *
 *	The third argument is no longer used, because the tree walk
 *	algorithm was changed so that at each level the directory is opened,
 *	it's contents read into a linked list, and then it's closed.
 *	This is all done *before* the routine recurses; so there is
 *	never more than one descriptor open at a time.
 *
 *	Old story:
 *	The third argument to ftw does not limit the depth to which
 *	ftw will go.  Rather, it limits the depth to which ftw will
 *	go before it starts recycling file descriptors.  In general,
 *	it is necessary to use a file descriptor for each level of the
 *	tree, but they can be recycled for deep trees by saving the
 *	position, closing, re-opening, and seeking.  It is possible
 *	to start recycling file descriptors by sensing when we have
 *	run out, but in general this will not be terribly useful if
 *	fn expects to be able to open files.  We could also figure out
 *	how many file descriptors are available and guarantee a certain
 *	number to fn, but we would not know how many to guarantee,
 *	and we do not want to impose the extra overhead on a caller who
 *	knows how many are available without having to figure it out.
 *
 *	It is possible for ftw to die with a memory fault in the event
 *	of a file system so deeply nested that the stack overflows.
 *
 * RETURN VALUE DESCRIPTION:	-1 if an error of any kind occurs (system
 *	call fails, malloc fails, etc), any non-zero value returned by
 *	a call to 'fn', else 0
 */

struct elem {
	struct elem *next;
	char *data;
};

struct list {
	struct elem *first;
	struct elem *last;
};

static
list_empty(listp)
struct list *listp;
{
	listp->first = listp->last = NULL;
}

static int
list_append(listp, data)
struct list *listp;
char *data;
{
	struct elem *ep;
	int len;

	ep = (struct elem *) calloc(1, sizeof(struct elem));
	if (ep == NULL) {
		SETERR(ENOMEM);
		return(-1);
	}
	ep->data = malloc(len = (strlen(data) + 1));
	if (ep->data == NULL) {
		SETERR(ENOMEM);
		return(-1);
	}
	bcopy(data, ep->data, len);
	if (listp->first == NULL)
		listp->first = ep;
	else
		listp->last->next = ep;
	listp->last = ep;
	return(0);
}

static char *
list_first(listp)
struct list *listp;
{
	struct elem *ep;
	char *data;

	if ((ep = listp->first) == NULL)
		return(NULL);
	listp->first = ep->next;
	data = ep->data;
	free((char *)ep);
	return(data);
}

static int
ftw_work(
	char *path,
	char *component,		/* root of tree walk */
	int (*fn)(char *,struct stat *,int), /* user function to call */
	int first_time)			/* is this the initial path */
{
	int rc, n;
	char *subpath;
	struct stat sb;
	DIR *dirp;
	struct dirent *entry;
	struct list list;
	char *elem;

	/*
	 * Try to get file status.
	 * If unsuccessful, errno will say why.
	 */
	if(first_time) {		/* Follow the link */
		if(stat(path, &sb) < 0)
			return(errno == EACCES ? (*fn)(path, &sb, FTW_NS) : -1);
	}
	else 
		if(lstat(path, &sb) < 0)
			return(errno == EACCES ? (*fn)(path, &sb, FTW_NS) : -1);

	/*
	 *	The lstat succeeded, so we know the object exists.
	 *	If not a directory, call the user function and return.
	 */
	if((sb.st_mode & S_IFMT) != S_IFDIR)
		return((*fn)(path, &sb, S_ISLNK(sb.st_mode) ? FTW_SL : FTW_F));

	/*
	 *	The object was a directory.
	 *
	 *	Open a file to read the directory
	 */
	dirp = opendir(path);

	/*
	 *	Call the user function, telling it whether
	 *	the directory can be read.  If it can't be read
	 *	call the user function or indicate an error,
	 *	depending on the reason it couldn't be read.
	 */
	if(dirp == NULL)
		return(errno == EACCES ? (*fn)(path, &sb, FTW_DNR) : -1);

	/* We could read the directory.  Call user function. */
	rc = (*fn)(path, &sb, FTW_D);
	if(rc != 0) {
		closedir(dirp);
		return(rc);
	}

	/* Check if we need a buffer */
	if (component == NULL) {

		/* Allocate a buffer to hold generated pathnames. */
		subpath = malloc((unsigned)PATH_MAX);
		if(subpath == NULL) {
			closedir(dirp);
			SETERR(ENOMEM);
			return(-1);
		}

		/* Create a prefix to which we will append component names */
		(void)strcpy(subpath, path);
		n = strlen(subpath);
		if(n > 0 && subpath[n-1] != '/')
			subpath[n++] = '/';
		component = &subpath[n];

	} else {

		subpath = path;
		*component++ = '/';

	}

	/*
	 *      Read the directory one component at a time.
	 *      We must ignore "." and "..", but other than that,
	 *      just create a path name and call self to check it out.
	 */
	list_empty(&list);
	while((entry = readdir(dirp)) != NULL) {
		if(strcmp(entry->d_name, ".") == 0 ||
		   strcmp(entry->d_name, "..") == 0)
		    continue;

		if(list_append(&list, entry->d_name) == -1) {
			closedir(dirp);
			return(-1);
		}
	}

	closedir(dirp);

	while ((elem = list_first(&list)) != NULL) {

		/* Append component name to the working path */
		strcpy(component, elem);
		n = strlen(elem);
		free(elem);

		/*
		 *	Do a recursive call to process the file.
		 *	(watch this, sports fans)
		 */
		rc = ftw_work(subpath, component + n, fn, 0);
		if(rc != 0) {
			if (subpath != path)
				free(subpath);
			return(rc);
		}
	}

	/*
	 *      We got out of the subdirectory loop.
	 *      Clean up and return.
	 */
	if (subpath != path)
		free(subpath);

	return(0);
}

int
ftw(
char *path,			     /* root of tree walk	*/
int (*fn)(char *,struct stat *,int), /* user function to call for each file */
int depth)			/* maximum number of file descriptors to use */
{
	return(ftw_work(path, (char *)NULL, fn, 1));
}
