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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iodone.c,v 1.4 1994/11/19 02:30:36 mtm Exp $
 *
 * Determine whether an asynchronous read or write operation is complete.
 *
 *      HISTORY:
 *      $Log: iodone.c,v $
 * Revision 1.4  1994/11/19  02:30:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  10:40:00  dbm
 * Added History; Corrected return parameter type to return long.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * iodone.c
 *
 * Description:
 *		This function is used to determine whether the 
 *		iread() or iwrite() operation identified by 
 *		'id' is complete. This function will return 
 *		a non negative value on success. On failure,
 * 		the error message will be written to standard
 *		error and the calling process will be terminated
 *		with a non zero exit code.
 *	
 * Parameters:	
 *		id	I/O ID returned by iread() or write().
 *
 * Returns:
 *
 *		0	If the iread or iwrite is not yet complete.
 *		1	if the iread or iwrite is complete.
 */
long
iodone(id)
long	id;	/* I/O ID. */
{
	long status;	/* Status of the i/o */

	status = _iodone(id);
	if (status == -1) {
		nx_perror("iodone");
		exit(1);
	}

	return(status);
}
