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
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *  Copyright 1992  Intel Corporation.
 *
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iodone_.c,v 1.4 1994/11/19 02:30:37 mtm Exp $
 *
 * FORTRAN interface to check an asynchronous operation is completed.
 *
 *      HISTORY:
 *      $Log: iodone_.c,v $
 * Revision 1.4  1994/11/19  02:30:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  10:44:36  dbm
 * Added History; Added check for error conditions.
 *
 *
 */
long
iodone_(tid)
long	*tid;
{
	long status; 

	status = _iodone(*tid);
	if (status < 0) {
		nx_perror("iodone");
		exit(1);
	}
}
