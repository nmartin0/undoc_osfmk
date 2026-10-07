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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_nodedim.c,v 1.2 1994/11/19 02:28:56 mtm Exp $
 *
 */


/*
 *
 * dimension = nodedim()
 *
 * Nodedim returns the dimension for the allocated cube.
 * The dimension returned is dimension for numnodes which are a power of 2.
 * E.g., if a user has allocated 7 nodes, the dimension returned is 3.
 *
 */

long _nodedim()

{

	register long i;
	long n, dim;

	if((n = _numnodes()) == -1)
		return (-1);
	i = 1;
	for (dim=0; i<n; ++dim) i <<= 1;
	return(dim);
}
