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
 *
 * Do an asynchronous read operation.
 *
 * HISTORY:
 * $Log: iread.c,v $
 * Revision 1.5  1994/11/19  02:30:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/04/02  22:42:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3.2.1  1993/03/20  23:40:58  brad
 * Removed $Header.
 *
 * Revision 1.3  1992/08/05  11:05:44  dbm
 * Added History; Matched parameter names with EPS.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * iread.c
 *
 * Description:
 *		This function is used to perform an asynchronous read
 *		operation with error checking.  This function will
 *		return a non-negative I/O ID on success.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 * 		buffer	Pointer to the destination buffer.
 *		nbytes	Number of bytes to read. 
 *
 * Returns:
 *		A non-negative I/O ID.
 */
long
iread(fildes, buffer, nbytes)
int		fildes;		/* File descriptor. */
char  		*buffer;	/* Pointer to the destination buffer. */
unsigned int	nbytes;		/* Number of bytes to read. */
{
	long mid;	/* I/O ID. */

	mid = _iread(fildes, buffer, nbytes);
	if (mid == -1) {
		nx_perror("iread");
		exit(1);
	}

	return(mid);
}
