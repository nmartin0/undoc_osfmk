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
static char rcsid[] = "@(#)$RCSfile: abort.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:25 $";
#endif
/*
 * FUNCTIONS: abort
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * abort.c	1.14  com/lib/c/gen,3.1,8943 10/10/89 10:21:36
 */

#include <signal.h>
#include <stdlib.h>

#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex _abort_rmutex;
#endif

/*
 * FUNCTION:	Causes abnormal program termination to occur, unless the
 *		signal SIGABRT is being caught and the signal handler does
 *		not return.
 *
 * RETURN VALUE DESCRIPTION:
 *              The abort function cannot return to its caller.
 */

static	int abortcnt;

void
abort(void)
{
	struct sigaction action;
	sigset_t mask;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_abort_rmutex);
#endif
	if (!abortcnt) {
		abortcnt++;
#ifdef _THREAD_SAFE
		rec_mutex_unlock(&_abort_rmutex);
#endif
		_cleanup();
	}
#ifdef _THREAD_SAFE
	else
		rec_mutex_unlock(&_abort_rmutex);
#endif
	(void) raise(SIGABRT);	/* Terminate the process */

	/*
	 * The signal handler for SIGABRT has returned or SIGABRT is blocked.
	 * Take additional steps to terminate the process.  If there is a
	 * pending SIGABRT then unblocking it may terminate the process.
	 */
	(void) sigemptyset(&mask);
	(void) sigaddset(&mask, SIGABRT);
 	(void) sigprocmask(SIG_UNBLOCK, &mask, (sigset_t *)NULL);

	/*
	 * Still alive?  Replace the signal handler for SIGABRT with SIG_DFL
	 * and raise SIGABRT again.  This should terminate the process.
	 */
	action.sa_handler = SIG_DFL;
	action.sa_flags = 0;
	(void) sigemptyset(&action.sa_mask);
 	(void) sigaction(SIGABRT, &action, (struct sigaction *)NULL);
	(void) raise(SIGABRT);

	exit(SIGABRT);	/* Should never ever get this far */
}
