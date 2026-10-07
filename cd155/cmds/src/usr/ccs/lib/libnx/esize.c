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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/esize.c,v 1.7 1994/11/19 02:29:30 mtm Exp $
 *
 * Increase the size of an extended file.
 *
 *      HISTORY:
 *      $Log: esize.c,v $
 * Revision 1.7  1994/11/19  02:29:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/10/27  17:09:52  shala
 * Changed "whence" parameter to be of type "long" instead of "int" to match
 * documentation.
 *
 * Revision 1.5  1993/10/26  16:52:26  shala
 * Used a different type definition to make it work with R4.5 compiler.
 *
 * Revision 1.4  1992/08/05  10:27:09  dbm
 * Modified check of esize_t ot look for both values == -1
 *
 * Revision 1.3  92/08/05  10:16:49  dbm
 * Added History; make parameter names same as EPS; fixed error check
 * to be consistent with other calls.
 * 
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "nx.h"

/*
 * esize.c
 *
 * Description:
 *		This function is used to increase the size of a 
 *		file.  This function will return the new size of
 *		the file on success.  On failure, the error message
 *		will be written to standard error and the calling
 *		process will be terminated with a non zero exit code.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 * 		offset	Increase the size as per the whence parameter.
 *		whence  Specifies how to increase the size:
 *
 *			0 	Set the file size to the greater of the 
 *				current size or offset.
 *
 *			1	Set the file pointer to the greater of the 
 *				current size or the current location of 
 *				the file pointer plus offset.
 *
 *			2	Set the file size to the greater of the 
 *				current size or the current size plus offset.
 *
 * Returns:
 *		The new file size, in bytes.
 */
esize_t
esize(int fildes, esize_t offset, int whence)
{
	esize_t newsize;

	newsize = _esize(fildes, offset, whence);
	if ((newsize.shigh == -1) && (newsize.slow == -1)) {
		nx_perror("esize");
		exit(1);
	}

	return(newsize);
}
