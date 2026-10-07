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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/common_nx_c/_eseek.c,v 1.4 1994/11/19 01:56:21 mtm Exp $
 *
 * Move the read/write file pointer in an extended file.
 *
 *      HISTORY:
 *      $Log: _eseek.c,v $
 * Revision 1.4  1994/11/19  01:56:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/10/27  17:07:46  shala
 * Changed "whence" parameter to be of type "long" instead of "int" to match
 * documentation.
 *
 * Revision 1.2  1993/10/26  16:54:59  shala
 * Used a different type definition to make it compile with R4.5 compiler.
 *
 * Revision 1.1  1993/09/28  01:16:40  brad
 * Initial revision
 *
 * Revision 1.3  1992/08/05  08:47:39  dbm
 * Added History.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "nx.h"

/*
 * _eseek.c
 *
 * Description:
 *		This function is a wrapper function that is used
 *		as the interface between the _eseek() call and the
 *		__eseek() system call.  This interface is due to the
 *		_eseek() returning a structure instead of the standard
 *		integer return value.
 *	
 * Parameters:	
 *		fd	File descriptor.
 * 		offset	Affects the file pointer as per the whence parameter.
 *		whence  Specifies how to modify the file pointer:
 *			0 	Set the file pointer to 'offset' bytes from
 * 				the beginning of the file.
 *			1	Set the file pointer to its current location
 *				plus offset bytes.
 *			2	Set the file pointer to offset bytes beyond
 *				end of the file.
 * Returns:
 *		The new file offset.
 */
esize_t
_eseek(int fd, esize_t offset, int whence)
{
	esize_t newoff;
	int	status;

	status = __eseek(fd, &offset, whence, &newoff);
	if (status == -1) {
		newoff.slow  = -1;
		newoff.shigh = -1;
	}

	return(newoff);
}
