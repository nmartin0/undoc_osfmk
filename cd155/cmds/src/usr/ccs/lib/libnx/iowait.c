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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iowait.c,v 1.4 1994/11/19 02:30:41 mtm Exp $
 *
 * Wait for an asynchronous read or write operation to complete.
 *
 *      HISTORY:
 *      $Log: iowait.c,v $
 * Revision 1.4  1994/11/19  02:30:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  10:57:30  dbm
 * Added History; Fixed some comments.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * iowait.c
 *
 * Description:
 *		This function is used to cause a process to wait
 *		until the iread() or iwrite() operation identified
 *		by 'id' to complete. This function will
 *		return on success. On failure, the error message
 *		will be written to standard error and the calling
 *		process will be terminated with a non zero exit code.
 *	
 * Parameters:	
 *		id	I/O ID returned by iread() or write().
 *
 * Returns:
 *		Nothing.
 */
void
iowait(id)
long	id;	/* I/O ID. */
{
	int status;	/* Status of the i/o operation. */

	status = _iowait(id);
	if (status == -1) {
		nx_perror("iowait");
		exit(1);
	}
}
