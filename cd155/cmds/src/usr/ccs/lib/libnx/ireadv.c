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
 * Do an asynchronous readv operation.
 *
 * HISTORY:
 * $Log: ireadv.c,v $
 * Revision 1.2  1994/11/19  02:30:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/03/20  23:44:14  brad
 * Initial revision
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <errno.h>
#include <errno.h>

/*
 * ireadv.c
 *
 * Description:
 *	Perform an asynchronous readv operation with error checking.
 *	
 * Parameters:	
 *	fildes		File descriptor.
 *
 *	iov		Pointer to the destination array of I/O vectors.
 *
 *	iovcount	Number of I/O vectors in the iov array.
 *
 * Returns:
 *	On success, this function returns a non-negative I/O ID.  On failure,
 *	an error message is written to standard error and the calling process
 *	is terminated with a non zero exit code.
 */
long
ireadv(fildes, iov, iovcount)
int		fildes;		/* file descriptor */
struct iovec	*iov;		/* array of iovec structures */
int		iovcount;	/* number of iovec structures */
{
	long mid;	/* I/O ID. */

	mid = _ireadv(fildes, iov, iovcount);
	if (mid == -1) {
		nx_perror("ireadv");
		exit(1);
	}

	return(mid);
}
