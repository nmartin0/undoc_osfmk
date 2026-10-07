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
 * CFS compatibility stub for synchronous read operation.
 *
 * HISTORY:
 * $Log: cread.c,v $
 * Revision 1.7  1994/11/19  02:29:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/04/02  22:41:43  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5.2.1  1993/03/20  23:37:16  brad
 * Just call read()/write() directly instead of calling _cread()/_cwrite().
 *
 * Revision 1.5  1992/08/05  09:27:45  dbm
 * Matched parameter types with EPS.
 *
 * Revision 1.4  92/08/05  09:16:42  dbm
 * Changed perror to nx_perror.
 * 
 * Revision 1.3  92/08/05  09:13:28  dbm
 * Added History and also fixed up comments
 * Added check for read past eof condition to make work same as
 * previous CFS versions.
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "nx.h"

/*
 * cread.c
 *
 * Description:
 *		This function is used to perform a synchronous read
 *		operation with error checking.  Upon succesful completion
 *		This routines simply returns control to the calling 
 *		process; no values are explicitly returned.
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
 *		Nothing.
 */
void
cread(fildes, buffer, nbytes)
int		fildes;		/* File descriptor. */
char		*buffer;	/* Pointer to the destination buffer. */
unsigned int	nbytes;		/* Number of bytes to read. */
{
	int cnt;

	cnt = read(fildes, buffer, nbytes);
	if (cnt == -1) {
		nx_perror("cread");
		exit(1);
	}
	if (cnt < nbytes) {
		errno = ERDEOF;
		nx_perror("cread");
		exit(1);
	}
}
