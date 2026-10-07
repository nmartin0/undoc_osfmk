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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: execlp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:56:30 $";
#endif
/*
 * FUNCTIONS: execlp 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * execlp.c	1.2  com/lib/c/sys,3.1,8943 9/13/89 11:56:05
 */

#include <stdio.h>
#include <stdarg.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#include "rec_mutex.h"

extern struct rec_mutex	_exec_rmutex;
#endif

extern int execvp(), exec_args();

/*
 * NAME:	execlp
 *
 * FUNCTION:	exec a file, using the arguments to construct an
 *		argv for the new file, and PATH to find the file
 *		to exec.
 *
 * NOTES:	execlp(file, arg0 [, arg1, ...], (char *) 0)
 *		execs a file like 'execl', except that the
 *		PATH is searched for the file to execute.
 *
 * RETURN VALUE DESCRIPTION:	-1 if a memory allocation occurs,
 *		if too many arguments were given, or if the exec
 *		fails.  Else execlp does not return.
 */

/* VARARGS1 */
int
execlp(char *file, ...)
{
	int error;		/* error status code	*/
	char **argv;		/* argv for `program'	*/
	va_list args;		/* varargs args		*/

#ifdef _THREAD_SAFE
	if (!rec_mutex_trylock(&_exec_rmutex)) {
		seterrno(EAGAIN);
		return(-1);
	}
#endif

	/* set up the arg list...  */
	va_start(args, file);

	/* get the rest of the args into argv...  */
	error = exec_args(&argv, (char ***) NULL, args);

	/* clean up the arg list...  */
	va_end(args);

	if (error == 0)
		/* do the exec...  */
		error = execvp(file, argv);

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_exec_rmutex);
#endif
	return (error);
}
