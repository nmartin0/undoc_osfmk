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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/load.c,v 1.3 1994/11/19 02:31:06 mtm Exp $
 *
 * C interface to load
 *
 */

#include <stdio.h>
#include <sys/types.h>


/***********************  load  ******************************
 *	
 *	Description:
 *	      User interface to load a process.
 *	
 *	Calling Sequence:
 *	      load("filename", node, pid);
 *	
 *	Parameters:
 *	      "filename":	Pathname of the file to load
 *	      node:	Node number
 *	      pid:	PID of new process
 *	
 *	Returns:
 *	      none
 *	
 *	Called by:
 *	      User
 */

load(file, node, pid)
	char	*file;
	long	node;
	long	pid;
{

	if (_load(file, node, pid) < 0) {
		nx_perror("load");
		exit(1);
	}
}
