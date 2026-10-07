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
static char rcsid[] = "@(#)$RCSfile: execvp.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:56:31 $";
#endif
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCSYS) Standard C Library System Functions 
 *
 * FUNCTIONS: execvp 
 *
 * ORIGINS: 3, 26, 27 
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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * sccsid[] = "execvp.c  1.9  com/lib/c/sys,3.1,9021 2/17/90 22:27:31";
 */

#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <unistd.h>
#include <sys/errno.h>
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _exec_rmutex;
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

static char *execat(), shell[] = "/bin/sh";
extern char **exec_argv();

/*
 * times to try when we get a busy text error...
 */
#define	TXTBUSY_TRIES	5

/*
 * NAME:	execvp
 *
 * FUNCTION:	execvp - exec a file, using argv, but search PATH
 *		for the file
 *
 * NOTES:	execvp(file, argv) execs a file, using argv as the argv
 *		for the new program.  PATH is searched for the file.
 *
 * RETURN VALUE DESCRIPTION:
 *		-1 is returned if the exec fails, if too many arguments
 *		were given, or on a memory allocation error.  Else this
 *		call does not return.
 */

int
#ifdef _NO_PROTO
execvp(file, argv)
char	*file;				/* file to exec	*/
char	**argv;				/* its argv	*/
#else /* _NO_PROTO */
execvp(char *file, char *argv[])
#endif /* _NO_PROTO */
{
	char	*pathstr;		/* PATH				*/
	char    fullname[PATH_MAX+1];	/* full filename to exec	*/
	char	*fp;			/* pointer to cp 		*/
	char	**newargv;		/* new argv to use		*/
	int	argvct;			/* 'newargv' pointers allocated	*/
	int	argc;			/* counter for newargv		*/
	int	size;			/* byte count for fullname	*/
	char	*cp;			/* Current Path component	*/
	int eacces = 0;			/* seen errno == EACCES yet?	*/
	unsigned int etxtbsy = 1;	/* errno == ETXTBSY counter	*/
#ifdef _THREAD_SAFE
        int     rc;

        if (!rec_mutex_trylock(&_exec_rmutex)) {
                seterrno(EAGAIN);
                return(-1);
        }
#endif

	/*
	 * If PATH or the file argument is an empty string return with
	 * errno set to ENOENT.  For execlp and execvp need to handle
	 * empty file here.
	 */
	if( *file == '\0' ) {
#ifdef _THREAD_SAFE
                rec_mutex_unlock(&_exec_rmutex);
#endif
		SETERR(ENOENT);
		return (-1);
	}
	/* get the path */
	if ((pathstr = getenv("PATH")) == NULL)
		pathstr = ":/bin:/usr/bin";		/* default path */

	/* don't search the path if the file has a '/' in it ...	*/
	cp = strchr(file, (int) '/') != NULL ? "" : pathstr;

	do {
		/* test to see if path+file exceeds PATH_MAX */
		/* construct an absolute pathname to exec... */
                for(fp=cp,size=0;(*fp != ':') && *fp;size++,fp++);
                if( (strlen(file) + size) <= PATH_MAX)
			cp = execat(cp, file, fullname);
                else {
                        SETERR(ENAMETOOLONG);
			return(-1);
		}

	retry:
		/* try to exec it */
		(void) execv(fullname, argv);

		/* we had an error.  figure out what it was ... */
		switch(errno) {

		case ENOEXEC:		/* exists, but not executable	*/
			/* maybe it's a shell script.  try a shell	*/

			/* count number of pointers in argv */
			for (argc = 0; argv[argc] != NULL; argc++);
			argc++;		/* include the NULL	*/

			/* get some memory to work with */
			for (argvct = 0; argvct < argc + 1; )
				if ((newargv = exec_argv(&argvct)) == NULL){
#ifdef _THREAD_SAFE
                                        rec_mutex_unlock(&_exec_rmutex);
#endif
					return (-1);
			}

			/*
			 * copy the old argv in.  leave space for the shell
			 * and program name
			 */
			(void) memmove((void *)&newargv[2], (void *)&argv[1],
				(size_t)((argc - 1) * sizeof(argv[1])));
			if ((newargv[0] = strrchr(shell,(int) '/')) == NULL)
				newargv[0] = shell;
			else
				newargv[0]++;
			newargv[1] = fullname;

			/* exec it */
#ifdef _THREAD_SAFE
                        rc = execv(shell, newargv);
                        rec_mutex_unlock(&_exec_rmutex);
                        return (rc);
#else
                        return (execv(shell, newargv));
#endif

		case ETXTBSY:		/* text was busy */
			/* try again? */
			if(++etxtbsy > TXTBUSY_TRIES)
				return(-1);
			(void) sleep(etxtbsy);
			goto retry;

		case EACCES:		/* permission denied... */
			/* set the flag, and try another path element */
			++eacces;
			break;

		case ENOMEM:		/* problems with the arguments */
		case E2BIG:
#ifdef _THREAD_SAFE
                        rec_mutex_unlock(&_exec_rmutex);
#endif
			/* don't try again */	
			return(-1);

		}
	/* loop while we still have path components left... */
	} while(cp != NULL);

	if(eacces)
		/* set errno back to EACCES in case we lost it */
		SETERR(EACCES);

#ifdef _THREAD_SAFE
        rec_mutex_unlock(&_exec_rmutex);
#endif
	return(-1);
}

/*
 * NAME:	execat
 *
 * FUNCTION:	construct a full pathname to exec, using the filename
 *		and the next component of the path
 *
 * NOTES:	execat takes a path, a filename to exec and builds
 *		the full filename to exec using the next component
 *		of the path.  A pointer to the following path
 *		component is returned.
 *
 * RETURN VALUE DESCRIPTION:	NULL if we've reached the end of
 *		the path, else a pointer to the next path component
 */

static char *
execat(path, file, fullname)
char	*path;			/* path to use		*/
char	*file;			/* file to exec		*/
char	*fullname;		/* destination filename	*/
{
	char	*fp;		/* fullname pointer	*/

	fp = fullname;

	/* copy in the current component of path */
	while (*path && *path != ':')
		*fp++ = *path++;

	/* append a '/' if there was a path name to copy in */
	if (fullname != fp)
		*fp++ = '/';

	/* append the filename */
	while (*file)
		*fp++ = *file++;

	/* NULL terminate the fullname */
	*fp = '\0';

	/*
	 * return NULL if we've hit the end of the path,
	 * else return a pointer to the next component
	 */
	return (*path == '\0' ? NULL : ++path);
}
