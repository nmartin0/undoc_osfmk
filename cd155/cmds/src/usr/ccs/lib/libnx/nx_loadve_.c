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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_loadve_.c,v 1.8 1994/11/19 02:31:45 mtm Exp $
 *
 */

static char newpath[256];

#include <nx/defines.h>

long
nx_loadve_(node_array, node_count, ptype, pid_array, path, argv, envp, pathlen, argvlen, envplen)
long 	*node_array;            /* Array of logical node number */
long    *node_count;            /* Number of nodes */
long    *ptype;                 /* Ptype */
long    *pid_array;            /* Pointer to the Out array for pids */
char	*path;			/* executable file */
char    *const argv[];          /* array of character pointers to arguments */
char 	*const envp[];          /* environment of the new process */
long	pathlen;
long	argvlen;
long	envplen;
{
	long status;
	_copyname(path, newpath, pathlen, sizeof(newpath));
 	status = nx_loadve(node_array, *node_count, *ptype, 
			   pid_array, newpath, argv, envp);
        return status;
}
