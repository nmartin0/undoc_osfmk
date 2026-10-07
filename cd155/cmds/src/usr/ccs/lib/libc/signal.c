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
static char rcsid[] = "@(#)$RCSfile: signal.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:16 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTION: signal
 *
 * ORIGINS: 3 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1990 
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
 * signal.c	1.20  com/lib/c/gen,3.1,9013 3/8/90 13:52:45
 */

#include <signal.h>
#include <errno.h>

/*
 * NOTES: This file is included by the _signal.c file with __SIGNAL defined
 *	to create _signal().  See _signal.c for more details.
 */


/*
 *
 * FUNCTION: sets action for a signal, compatibility interface to sigaction()
 *
 * NOTES: signal() allows only a subset of the function of sigaction().
 *	  It is provided for compatibility with old source code, and
 *	  for comformance to POSIX and ANSI "C" standards.  
 *
 * RETURN VALUES: 
 *	SIGERR         => failed, errno is set to specify the cause of failure
 *      errno = EINVAL => invalid signal number
 *	      = EINVAL => attempt to ignore or catch SIGKILL,SIGSTOP,SIGCONT
 *	Any other return value implies success, and the return value is
 *	a the previous signal action handler value (see sigaction()).
 */

void
#ifdef __SIGNAL
(*_signal(int signo, void (*fun)(int)))(int)
#else
(*signal(int signo, void (*fun)(int)))(int)
#endif
{
	struct sigaction act;	/* new signal action structure */
	struct sigaction oact;  /* returned signal action structure */ 

	/*   Setup a sigaction struct */

 	act.sa_handler = fun;        /* Handler is function passed */
	sigemptyset(&(act.sa_mask)); /* No signal to mask while in handler */
	act.sa_flags = 0;
	
	/* use the sigaction() system call to set new and get old action */

	if(sigaction(signo, &act, &oact))
		/* If sigaction failed return SIG_ERR */
	    return(SIG_ERR);
	else
        	/* use the previous signal handler as a return value */
	    return(oact.sa_handler);
}
