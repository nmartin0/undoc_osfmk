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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_load.c,v 1.6 1994/11/19 02:31:43 mtm Exp $
 *
 */

#include <sys/types.h>
#include <nx/h.h>
#include <nx/defines.h>

/***************************** nx_load ***********************
 *
 *      Calling Sequence:
 *             nx_loadve(node_array, node_count, ptype, pid_array, path)
 *
 *      Description:
 *		nx_load() executes the specified file on the specified
 *		set of nodes. First a process is created on all nodes
 *		and then each process execs on all the rest of the nodes.
 *
 *      Parameters:
 *              node_array : an array of node_ts 
 *		node_count : number of nodes in node_array
 *		ptype      : The process type for each child process
 *		pid_array  : OUT array for pids
 *		path       : The relative or absolute pathname of the file
 *
 *      Returns:
 *              number of child processes or -1
 *
 *
 */

long 
nx_load(node_array, node_count, ptype, pid_array, path) 
node_t	*node_array;		/* Array of logical node number */
long	node_count;		/* Number of nodes */
long	ptype;			/* Ptype */
pid_t	*pid_array;		/* Pointer to the Out array for pids */
char 	*path;			/* executable file */
{
	return(nx_loadve(node_array, node_count, ptype, pid_array, path,
	                 NULL, (char *) 0));
}
