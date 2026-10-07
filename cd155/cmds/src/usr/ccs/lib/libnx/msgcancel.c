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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/msgcancel.c,v 1.3 1994/11/19 02:31:16 mtm Exp $
 *
 * Cancels an asynchronous send or receive request.
 * 
 *
 */

extern long	_msgcancel();

/*
 * msgcancel
 *
 * Description: Cancels an asynchronous send or receive request
 *
 * Parameters:
 *	tid	Message request identifier - associates the msgcancel
 *		call with a particular request
 *
 * Returns:	None
 */
msgcancel(tid)

long	tid;
{
	if (_msgcancel(tid) == -1) {
		nx_perror("msgcancel");
		exit(1);
	}
}
