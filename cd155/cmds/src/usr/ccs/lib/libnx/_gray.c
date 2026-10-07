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
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_gray.c,v 1.2 1994/11/19 02:28:50 mtm Exp $
 */

/*
 *
 * gray(j) = j-th entry in gray code
 *
 * domain and range both 0..127
 *
 * this is the standard way to obtain a one-dimensional mesh
 * or ring of tasks on the hypercube.  if the j-th task is
 * carried out on node number gray(j), then each task is
 * connected directly to its predecessor and its successor
 * in the hypercube topology.
 */

long _gray(j)

long j;
{
	return(j^(j/2));
}
