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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gopf_.c,v 1.3 1994/11/19 02:30:00 mtm Exp $
 *
 *
 * This module contains the application interface to the gopf_ for FORTRAN.
 * 
 */



extern long	_gopf();


/***********************  gopf_  ******************************
 *
 * Global vector operation of a function.
 * 
 * Communication utilities use its reserved message types.
 * They range from 2000000001 to 2000000007.  They are the same for
 * all participating processes.  There must be no other messages of
 * these types in the system.  All participating processes must have the
 * same PID and the number of nodes in the allocated cube must be a
 * power of 2.
 *
 * input..
 * 
 *   x         the input vector to be used in the operation.  
 *   len       the length of the vector x in bytes.
 *   work      an array which is used to receive other
 *             contributions.  The length of the array must be at least len.
 *   f         the function to be called.
 *             It is called as f(n, work).
 *             f(n, work) sets x to whatever function of x and work
 *             is desired.
 * 
 * output..
 * 
 *   x         x contains the desired result.
 * 
 * workspace
 * 
 * 
 * error conditions
 * 
 *       none
 * 
 * calls:  _gopf_, nx_perror, exit
 *
 */

gopf_(x, len, work, f)
	long x[];
	long *len;
	long work[];
	long (*f)();
{
	if (_gopf_(x, len, work, f)) {
		nx_perror("gopf");
		exit(1);
	}
}
