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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gcolx.c,v 1.3 1994/11/19 02:29:39 mtm Exp $
 *
 * This module contains the application interface to gcolx.
 * 
 */

/*
 ************************* gcolx ******************************************
 *
 * Globally collects(concatenates) a contribution of specified length from
 * each node in node number order.  By providing the expected length of each
 * contribution, the speed of this operation is greatly improved due to the
 * reduced overhead of calculating where each contribution belongs in the
 * output buffer.  The resulting concatenation is sensitive to the order
 * in which these contributions are received.  To insure that the
 * contributions are received in order, it is necessary to use specified
 * type numbers.  The type numbers used are reserved commutl message types.
 * They range from 2000000001 to 2000000007.  All participating processes
 * must have the same PID and the number of nodes in the allocated cube
 * must be a power of 2.
 * 
 * input..
 * 
 *   x         pointer to the input buffer to be used in the operation.  
 *   xlens     array of the lengths of each contribution in bytes.
 * 
 * output..
 * 
 *   y         pointer to the output buffer to be used in the operation.
 *             y contains the desired result.
 * 
 */

extern long	_gcolx();

gcolx(x, xlens, y)
	char x[];
	long xlens[];
	char y[];
{
	if (_gcolx(x, xlens, y)){
		nx_perror("gcolx");
		exit(1);
	}
}
