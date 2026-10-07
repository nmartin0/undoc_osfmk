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

#ifndef	lint
static char rcs_id[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libpthreads/sched.c,v 1.2 1994/11/19 02:33:54 mtm Exp $";
#endif	not lint

/*
 * File: sched.c
 *
 * This file contains all the support for the scheduling control of pthreads
 * which is currently none. As we use kernel threads to run on, we have to get
 * them to behave before we can export this functionality.
 */

#include <pthread.h>
#include "internal.h"
#include <errno.h>

/*
 * Function:
 *	pthread_setprio
 *
 * Parameters:
 *	thread - the thread to alter
 *	prio - the new priority for the thread to run at
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_setprio(pthread_t thread, int prio)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_getprio
 *
 * Parameters:
 *	thread - the thread to inquire about
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_getprio(pthread_t thread)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_setscheduler
 *
 * Parameters:
 *	thread - the thread to alter
 *	alg - the new scheduling policy
 *	prio - the new priority for the thread to run at
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_setscheduler(pthread_t thread, int alg, int prio)
{
	set_errno(ENOSYS);
	return(-1);
}

/*
 * Function:
 *	pthread_getscheduler
 *
 * Parameters:
 *	thread - the thread to inquire about
 *
 * Return value:
 *	-1	This function is not supported (ENOSYS)
 *
 * Description:
 *	This function is not supported.
 */
int
pthread_getscheduler(thread)
pthread_t	thread;
{
	set_errno(ENOSYS);
	return(-1);
}
