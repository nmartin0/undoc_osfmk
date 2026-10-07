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
static char rcsid[] = "@(#)$RCSfile: system.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:50 $";
#endif
/* @(#)system.c	1.14  com/lib/c/io,3.1,8943 10/18/89 15:14:51 */
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: system 
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
 */

/*LINTLIBRARY*/
#include	<signal.h>
#include	<sys/types.h>
#include	<unistd.h>	/* for access */
#include	<sys/wait.h>

#define USRSHELL	"/usr/bin/sh"
#define SINGLESHELL	"/sbin/sh"

int 	
system(const char *string)
{
  	sigset_t old,new;
	int	status, w;
	pid_t	pid;

	if((pid = fork()) == 0) 
	{
		/* 
		 * if the terminal is in a trusted state then 
		 * maintain trustedness by running the trusted shell 
		 */
		if (access(USRSHELL, X_OK) == 0)
			(void) execl(USRSHELL, "sh", "-c", string, 0);
		else
			(void) execl(SINGLESHELL, "sh", "-c", string, 0);

		_exit(127);
	}

	sigemptyset( &new );
	sigaddset( &new, SIGCHLD);
	sigaddset( &new, SIGINT);
	sigaddset( &new, SIGQUIT);
	sigprocmask( SIG_BLOCK, &new, &old);

	w = waitpid( pid, &status, 0);

	sigprocmask( SIG_SETMASK, &old, NULL);

	return((w == -1)? w: status);
}
