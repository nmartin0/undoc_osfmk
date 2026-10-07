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
 *      This software is supplied under the terms of a license
 *      agreement or nondisclosure agreement with Intel Corporation
 *      and may not be copied or disclosed except in accordance
 *      with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_load.c,v 1.5 1994/11/19 02:28:54 mtm Exp $
 *
 * Under score version of load
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <nx/h.h>

/***********************  _load  ******************************
 *	
 *	Description:
 *	      User interface to load a process.
 *	
 *	Calling Sequence:
 *	      _load("filename", node, pid);
 *	
 *	Parameters:
 *	      "filename":	Pathname of the file to load
 *	      node:	Node number
 *	      pid:	PID of new process
 *	
 *	Returns:
 *	      -1 if error
 *	
 *	Called by:
 *	      User
 */

_load(file, node, pid)
	char	*file;
	long	node;
	long	pid;
{
	long nn;
	node_t node_array[128];
	pid_t pid_array[128];
	long i;
	long node_count;

	
	if ((nn = _numnodes()) < 0 )
		return (-1);
	else
		/* if our partition is larger than 128 restrict the call to
		 * only 128 nodes, since this is an ipsc860 compatible call
		*/
		if (nn > 128) 
			nn = 128;

	if (node == -1) {
		for (i = 0; i < nn; i++)
			node_array[i] = i;
		node_count = nn;
	} else {
		node_array[0] = node;
		node_count = 1;
	}

	if (nx_loadve(node_array, node_count, pid,  pid_array, file, 0, 0) < 0) {
		nx_perror("load");
		return (-1);
	}
	return (0);
}
