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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iseof.c,v 1.4 1994/11/19 02:30:56 mtm Exp $
 *
 * Determine whether the file pointer for the specified file is at end-of-file.
 *
 *      HISTORY:
 *      $Log: iseof.c,v $
 * Revision 1.4  1994/11/19  02:30:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  11:23:29  dbm
 * Added History; Matched parameter names with EPS.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * iseof.c
 *
 * Description:
 *		This function is used to determine whether the file
 *		pointer for the file identified by the 'fd' parameter
 *		is at the end of the file.  This function will
 *		return a non negative return value on success.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non-zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 *
 * Returns:
 *		0	If the file pointer is not at end of the file.
 *
 *		1	If the file pointer is at the end of the file.
 *
 */
long
iseof(fildes)
int	fildes;	/* File descriptor. */
{
	long status;

	status = _iseof(fildes);
	if (status == -1) {
		nx_perror("iseof");
		exit(1);
	}
	return(status);
}
