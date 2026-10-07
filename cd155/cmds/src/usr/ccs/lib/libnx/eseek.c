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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/eseek.c,v 1.7 1994/11/19 02:29:27 mtm Exp $
 *
 * Move the read/write file pointer in an extended file.
 *
 *      HISTORY:
 *      $Log: eseek.c,v $
 * Revision 1.7  1994/11/19  02:29:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/10/27  17:09:49  shala
 * Changed "whence" parameter to be of type "long" instead of "int" to match
 * documentation.
 *
 * Revision 1.5  1993/10/26  20:50:18  shala
 * Fiexed the redefinition problem to make it compile with R4.5 compiler.
 *
 * Revision 1.4  1992/08/05  10:25:16  dbm
 * Modified check of esize_t to look for both values == -1.
 *
 * Revision 1.3  92/08/05  09:57:00  dbm
 * Added History.
 * Matched parameter names with EPS.
 * 
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "nx.h"

/*
 * eseek.c
 *
 * Description:
 *		This function is used to position the read/write file
 *		pointer in an extended file.  This function will
 *		return the new position of the file pointer on success.
 *		On failure, the error message will be written to 
 *		standard error and the calling process will be 
 *		terminated with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 * 	 	offset	Affects the file pointer as per the whence parameter.
 *		whence  Specifies how to modify the file pointer:
 *			0 	Set the file pointer to 'offset' bytes from
 * 				the beginning of the file.
 *			1	Set the file pointer to its current location
 *				plus offset bytes.
 *			2	Set the file pointer to offset bytes beyond
 *				end of the file.
 * Returns:
 *		If successful, the new file offset.
 */
esize_t
eseek(int fildes, esize_t offset, int whence)
{
	esize_t newoff;

	newoff = _eseek(fildes, offset, whence);
	if ((newoff.shigh == -1) && (newoff.slow == -1)) {
		nx_perror("eseek");
		exit(1);
	}

	return(newoff);
}
