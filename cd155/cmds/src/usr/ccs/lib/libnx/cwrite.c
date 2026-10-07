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
 * CFS compatibility stub for synchronous write operation.
 *
 * HISTORY:
 * $Log: cwrite.c,v $
 * Revision 1.5  1994/11/19  02:29:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/04/02  22:41:49  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3.2.1  1993/03/20  23:37:07  brad
 * Just call read()/write() directly instead of calling _cread()/_cwrite().
 *
 * Revision 1.3  1992/08/05  09:26:02  dbm
 * Added History
 * Matched parameter types and names to EPS.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <nx.h>

/*
 * cwrite.c
 *
 * Description:
 *		This function is used to perform a synchronous write
 *		operation with error checking.  Upon successful completion
 *		cwrite simply returns control to the calling process; no
 *		values are explicitly returned.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 * 		buffer	Pointer to the source buffer.
 *		nbytes	Number of bytes to written. 
 *
 * Returns:
 *		Number of bytes actually written. 
 */
void
cwrite(fildes, buffer, nbytes)
int		fildes;		/* File descriptor. */
char		*buffer;	/* Pointer to the source buffer. */
unsigned int	nbytes;		/* Number of bytes to write. */
{
	int cnt;

	cnt = write(fildes, buffer, nbytes);
	if (cnt == -1) {
		nx_perror("cwrite");
		exit(1);
	}
}
