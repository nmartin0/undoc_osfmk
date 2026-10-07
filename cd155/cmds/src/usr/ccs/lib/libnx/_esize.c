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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/_esize.c,v 1.6 1994/11/19 02:28:42 mtm Exp $
 *
 * Increase the size of an extended file.
 *
 *      HISTORY:
 *      $Log: _esize.c,v $
 * Revision 1.6  1994/11/19  02:28:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/10/27  17:09:37  shala
 * Changed "whence" parameter to be of type "long" instead of "int" to match
 * documentation.
 *
 * Revision 1.4  1993/10/26  16:53:06  shala
 * Used a different type definition to make it compile with R4.5 compiler.
 *
 * Revision 1.3  1992/08/05  08:48:27  dbm
 * Added History.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include "nx.h"

/*
 * _esize.c
 *
 * Description:
 *              This function is a wrapper function that is used
 *              as the interface between the _esize() call and the
 *              __esize() system call.  This interface is due to the
 *              _esize() returning a structure instead of the standard
 *              integer return value.
 *	
 * Parameters:	
 *		fd	File descriptor.
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
_esize(int fd, esize_t offset, int whence)
{
	esize_t newsize;
	int status;

	status = __esize(fd, &offset, whence, &newsize);
	if (status == -1) {
		newsize.shigh = -1;
		newsize.slow  = -1;
	}

	return(newsize);
}
