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
static char rcsid[] = "@(#)$RCSfile: popen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:22 $";
#endif
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: popen, pclose 
 *
 * ORIGINS: 3, 27 
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
 * popen.c	1.10  com/lib/c/io,3.1,8943 10/18/89 14:46:22
 */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/errno.h>

#define	tst(a,b) (*mode == 'r'? (b) : (a))
#define	RDR	0
#define	WTR	1

extern int fcntl();
static int popen_pid[_NFILE];  /* _NFILE is # of open files */

/*                                                                    
 * FUNCTION: Initiates a pipe to/from a process.
 *                                                                    
 * RETURN VALUE DESCRIPTION: FILE * to opened stream, NULL on error. 
 */  

FILE *
popen(char *cmd, char *mode)
{
	int	p[2];
	register int myside, yourside;
	pid_t pid;

	if(pipe(p) < 0)
		return(NULL);
	myside = tst(p[WTR], p[RDR]);
	yourside = tst(p[RDR], p[WTR]);
	if((pid = fork()) == 0) {
		/* myside and yourside reverse roles in child */
		int	stdio;

		stdio = tst(0, 1);
		(void) close(myside);
		(void) close(stdio);
		(void) fcntl(yourside, 0, stdio);
		(void) close(yourside);
		(void) execl("/bin/sh", "sh", "-c", cmd, (char *)0);
		_exit(1);
	}
	if(pid == -1)
		return(NULL);
	popen_pid[myside] = pid;
	(void) close(yourside);
	return(fdopen(myside, mode));
}

int
pclose(ptr)
FILE	*ptr;
{
	register int f, r;
	int save_errno;
	int status;
	sigset_t	old,new;

	f = fileno(ptr);
	(void) fclose(ptr);

	sigemptyset( &new );
	sigaddset( &new, SIGINT );
	sigaddset( &new, SIGQUIT );
	sigaddset( &new, SIGHUP );
	sigprocmask( SIG_BLOCK, &new, &old );	/* Disable SIGINT, SIGQUIT, SIGHUP */

	r = waitpid(popen_pid[f], &status, 0);
	save_errno = errno;

	if(r == -1)
		status = -1;

	sigprocmask( SIG_SETMASK, &old, NULL);
	errno = save_errno;
	return(status);
}
