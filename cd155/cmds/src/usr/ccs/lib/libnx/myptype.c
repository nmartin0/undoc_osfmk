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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/myptype.c,v 1.4 1994/11/19 02:31:32 mtm Exp $
 *
 * Return ptype of caller.
 * 
 */

extern long	_myptype();

/*
 * myptype 
 *
 * Description:	Returns process type
 *
 * Parameters: 	None
 *
 * Returns: 	Process type
 */
long
myptype()
{
	long	retval;

	if ( (retval = _myptype() ) == -1) {
		nx_perror("myptype");
		exit(1);
	}
	return(retval);
}
