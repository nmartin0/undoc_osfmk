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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gland_.c,v 1.3 1994/11/19 02:29:56 mtm Exp $
 *
 *
 * This module contains the application interface to the gland_ for FORTRAN.
 * 
 */



extern long	_gland();


/***********************  gland_  ******************************
 *
 * Global vector logical AND operation.
 * 
 * Communication utilities use their reserved message types.
 * They ranges from 2000000001 to 2000000007.  They are the same for
 * all participating processes.  There must be no other messages of
 * these types in the system.  All participating processes must have
 * the same PID and the number of nodes in the allocated cube must be
 * a power of 2.
 *
 * input..
 * 
 *   x         the input vector to be used in the operation.  
 *   n         the length of the vector x.
 * 
 * output..
 * 
 *   x         x contains the desired result.
 * 
 * workspace
 * 
 *   work      an array which is used to receive the other
 *             contributions.  The length of the array must be at least n.
 * 
 * errors conditions
 * 
 *       If a message longer than n elements is received, an error message
 *       is sent to stderr, and the process is terminated.
 * 
 *       If a message shorter than n elements is received, an error message
 *	 is sent to stderr and the process is terminated.
 * 
 * calls:  _gland, nx_perror, exit
 *
 */

gland_(x, n, work)
	long x[];
	long *n;
	long work[];
{
	if (_gland(x, *n, work)) {
		nx_perror("gland");
		exit(1);
	}
}
