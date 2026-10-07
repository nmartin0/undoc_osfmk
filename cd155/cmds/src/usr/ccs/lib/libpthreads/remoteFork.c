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
 *      Copyright (c) 1992, Intel Corporation.
 *      All rights reserved.
 *
 *         INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *    This software is supplied under the terms of a license
 *    agreement or nondisclosure agreement with Intel Corpo-
 *    ration and may not be copied or disclosed except in
 *    accordance with the terms of that agreement.
 */

/*
 * Pthreads implementation of rforkmulti() and rfork() system calls. These
 * versions are necesary to ensure fork'ed children are in a known state
 * after the rfork{multi}() and we don't deadlock because the other threads
 * are no longer with us.
 *
 * HISTORY:
 * $Log: remoteFork.c,v $
 * Revision 1.3  1994/11/19  02:33:53  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/10/22  16:45:30  stans
 *    More comments.
 *
 * Revision 1.1  1993/08/05  22:35:46  stans
 *    pthread versions of rfork() and rforkmulti().
 *
 */

#include <mach.h>
#include <pthread.h>
#include "internal.h"
#include <syscall.h>

/*
 * Function:
 *   rforkmulti(count, node_array, rval_array, pid_array)
 *	int     *count;
 *	int     *node_array
 *	int     *rval_array
 *	int     *pid_array
 *
 * Description:
 *      remote fork a multi-threaded process to multiple nodes, cleaning up
 *	the pthread state in the children. The children will come up with only
 *	one thread running. Patterned after the libnx.a version.
 */

rforkmulti(count, node_array, rval_array, pid_array)
	int     *count;
	int     *node_array;
	int     *rval_array;
	int     *pid_array;
{
	int	fork_return;

	int	node;

	pthread_fork_prepare();		/* Grab pthread locks to insure a
    					   consistent state after the fork */

	/*
	 * Do an actual rforkmulti() which includes the necessary Mach interface
	 * re-initalizations (i.e., call mach_init() ).
	 */
	fork_return = pthread_rforkmulti(count,node_array,rval_array,pid_array);

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
 *   rfork( node )
 *	int     node;
 *
 * Description:
 *      remote fork a multi-threaded process to the specified node, cleaning
 *	up the pthread state in the child. The child will come up with only
 *	one thread running.
 */

rfork( node )
	int     node;
{
	int	fork_return;

	pthread_fork_prepare();		/* Grab pthread locks to insure a
    					   consistent state after the fork */

	/*
	 * Do an actual rfork(node) which includes the necessary Mach interface
	 * re-initalizations (i.e., call mach_init() ).
	 */
	fork_return = pthread_rfork( node );

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

