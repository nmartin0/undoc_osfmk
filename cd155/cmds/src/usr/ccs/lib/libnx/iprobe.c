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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iprobe.c,v 1.3 1994/11/19 02:30:42 mtm Exp $
 *
 * Allows the user to check if a message of the specified type is
 * available for receipt.
 * 
 *
 */

extern long	_iprobe();

/*
 * iprobe
 *
 * Description:	Check if a message of a specified type has arrived
 *
 * Parameters:	
 *	type	Message type 
 *
 * Returns:	 0: Message has not yet arrived
 *		 1: Message has arrived
 *		-1: Invalid type parameter
 */
long
iprobe(type)

long type;
{
	long	retval;

	if ( (retval = _iprobe(type) ) == -1) {
		nx_perror("iprobe");
		exit(1);
	}
	return(retval);
}
