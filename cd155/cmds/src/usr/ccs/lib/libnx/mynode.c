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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/mynode.c,v 1.3 1994/11/19 02:31:27 mtm Exp $
 *
 * Returns the node number of the calling process.
 * 
 *
 */

extern long	_mynode();

/*
 * mynode 
 *
 * Description:	Returns the node number.
 *
 * Parameters: 	None
 *
 * Returns:	Host or remote host node number
 */
long
mynode()
{
	long	retval;

	if ( (retval = _mynode() ) == -1) {
		nx_perror("mynode");
		exit(1);
	}
	return(retval);
}
