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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/cprobe.c,v 1.3 1994/11/19 02:29:09 mtm Exp $
 *
 *  Wait for a message of the specified type to arrive.
 * 
 *
 */

extern long	_cprobe();

extern int errno;

/*
 * cprobe
 *
 * Description:	Wait for a message of a specified type to arrive.
 *
 * Parameters:	
 *	type	Message type 
 *
 * Returns:	None
 */
cprobe(type)

long type;
{
	if (_cprobe(type) == -1) {
		nx_perror("cprobe");
		exit(1);
	}
}
