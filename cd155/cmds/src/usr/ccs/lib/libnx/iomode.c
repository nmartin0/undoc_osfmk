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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/iomode.c,v 1.4 1994/11/19 02:30:38 mtm Exp $
 *
 * Return the I/O mode of a file.
 *
 *      HISTORY:
 *      $Log: iomode.c,v $
 * Revision 1.4  1994/11/19  02:30:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  10:49:10  dbm
 * Added History; Made parameter names and types match EPS.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * iomode.c
 *
 * Description:
 *		This function is used to determine the current I/O 
 *		mode of the file identified by the 'fildes' parameter.
 *		This function will will return an non negative value 
 *		on success. On failure, the error message will be written
 *		to standard error and the calling process will be terminated
 *		with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 *
 * Returns:
 *		Value of the current I/O mode.
 */
long
iomode(fildes)
int	fildes;	/* File descriptor. */
{
	long mode;

	mode = _iomode(fildes);
	if (mode == -1) {
		nx_perror("iomode");
		exit(1);
	}

	return(mode);
}
