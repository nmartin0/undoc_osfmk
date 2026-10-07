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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/gsync.c,v 1.3 1994/11/19 02:30:11 mtm Exp $
 *
 * This module contains the application interface to the gsync.
 * 
 */



extern long	_gsync();


/***********************  gsync  ******************************
 *
 * Global synchronization operation.
 *
 * Communication utilities use their reserved message types.
 * They range from 2000000001 to 2000000007.  They are the same for
 * all participating processes.  There must be no other messages of
 * these types in the system.  All participating processes must have
 * the same PID and the number of nodes in the allocated cube must
 * be a power of 2.
 * 
 * input..
 * 
 *   none.
 * 
 * output..
 * 
 *   none.
 * 
 * errors conditions
 * 
 *	 none.
 * 
 * calls:  _gsync, nx_perror, exit
 *
 */

gsync()
{
	if (_gsync()) {
		nx_perror("gsync");
		exit(1);
	}
}
