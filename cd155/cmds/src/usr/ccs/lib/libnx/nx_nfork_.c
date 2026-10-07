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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/nx_nfork_.c,v 1.6 1994/11/19 02:31:50 mtm Exp $
 *
 */

#include <sys/types.h>

typedef	long	node_t;

long 
nx_nfork_(node_array,node_count, ptype, pid_array) 
node_t 	*node_array;		/* Array of logical node number */
int 	*node_count;		/* Number of nodes */
long 	*ptype;			/* Ptype */
pid_t 	*pid_array;		/* Out array for pids */
{
	long status;

	if((status = nx_nfork(node_array,*node_count, *ptype, pid_array)) == -1){
        }
        return status;
}
