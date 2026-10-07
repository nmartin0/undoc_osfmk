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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/lsize.c,v 1.4 1994/11/19 02:31:09 mtm Exp $
 *
 * Increase the size of a file.
 *
 *      HISTORY:
 *      $Log: lsize.c,v $
 * Revision 1.4  1994/11/19  02:31:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  11:52:13  dbm
 * Added History; Matched parameter names and types to EPS.
 *
 *
 */
#include <stdio.h>
#include <sys/types.h>
#include <errno.h>

/*
 * lsize.c
 *
 * Description:
 *		This function is used to pre-allocate disk space for
 *		a given file.
 *	
 * Parameters:	
 *		fildes	File descriptor.
 *
 * 		offset	Increases the file size as specified by the whence 
 *			parameter.
 *
 *		whence	Specifies how offset affects the file size:
 * 
 *			0	Sets the file size to the greater of
 *				the current size or offset.
 *
 *			1	Sets the file size to the greater of
 *				the current size or the current
 *				location of the file pointer plus offset.
 *
 *			2	Set the file size to the greater of the
 *				current size or the current size plus  
 *				offset.
 *
 * Returns:
 *		The new file size if the call is successful, otherwise exits
 *		the caller.
 */
long
lsize(fildes, offset, whence)
int	fildes;	/* File descriptor. */
off_t 	offset;	/* Specifies the file size as per whence */
long	whence;	/* Specifies effect of offset. */
{
	long newsize;

	newsize = _lsize(fildes, offset, whence);
	if (newsize == -1) {
		nx_perror("lsize");
		exit(1);
	}

	return(newsize);
}
