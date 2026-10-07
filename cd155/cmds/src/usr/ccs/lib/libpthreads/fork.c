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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Copyright (C) 1989 Michael B. Jones
 */

#ifndef	lint
static char rcs_id[] = "$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libpthreads/fork.c,v 1.4 1994/11/19 02:33:46 mtm Exp $";
#endif	not lint

/*
 * Implementation of the unix fork call. This is necesary to ensure the world
 * is in a known state after the fork and we don't deadlock because the other
 * threads are no longer with us.
 */

#include <mach.h>
#include <pthread.h>
#include "internal.h"
#include <syscall.h>

/*
 * Function:
 *	unix_fork
 *
 * Return value:
 *	The same as fork().
 *
 * Description:
 *	Implement a straight unix fork() call even though we overload fork().
 */
int
unix_fork()
{
	int	parent_pid;
	int	fork_return;
	int	our_pid;
	extern	forknx();

	parent_pid = getpid();
	fork_return = syscall(SYS_fork);
	our_pid = getpid();

	if (fork_return < 0)
		return(fork_return);		/* Fork error */

	if (parent_pid == our_pid) {
		/*
		 * Parent.
		 */
		return(fork_return);		/* Parent returns child pid */
	} else {
		/*
		 * Child.
		 *
		 * Reinitialize mach ports, etc. as per libmach
		 */
		(void) mach_init();
		forknx();
		/*
		 * Syscall(SYS_fork) doesn't return 0 to the child.
		 * Returning fork_return would be incorrect.
		 */
		return(0);
	}
}

/*
 * Function:
 *	fork
 *
 * Return value:
 *	0	if this is the child
 *	childpid if this is the parent
 *	-1	to the parent if the fork failed.
 *
 * Description:
 *      Fork a multi-threaded process, cleaning up the pthread
 *	state in the child. The child will come up with only one
 *	thread running.
 */
int
fork()
{
	int	fork_return;

	pthread_fork_prepare();		/* Grab pthread locks to insure a
    					   consistent state after the fork */

	fork_return = unix_fork();	/* Do an actual fork (and mach_init) */

	if (fork_return != 0) {
		/*
		 * Parent and error cases.
		 */
		pthread_fork_parent();	/* Release locks grabbed by prepare */
	} else {
		/*
		 * Child.
		 */
		pthread_fork_child();	/* Rebuild a consistent state */
	}
	return(fork_return);
}

/*
 * Function:
 *	vfork
 *
 * Description:
 *	layer vfork over our fork so this is safe too.
 */
int
vfork()
{
	return(fork());
}

