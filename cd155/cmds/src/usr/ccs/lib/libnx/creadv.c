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
 * CFS compatibility stub for synchronous readv operation.
 *
 * HISTORY:
 * $Log: creadv.c,v $
 * Revision 1.2  1994/11/19  02:29:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/03/20  23:39:45  brad
 * Initial revision
 *
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <errno.h>

/*
 * readv.c
 *
 * Description:
 *	Perform a synchronous readv operation with error checking.
 *	
 * Parameters:	
 *	fildes		File descriptor.
 *
 *	iov		Pointer to the destination array of I/O vectors.
 *
 *	iovcount	Number of I/O vectors in the iov array.
 *
 * Returns:
 *	Nothing.  On success, control is returned to the calling process.  On
 *	failure, an error message is written to standard error and the calling
 *	process is terminated with a non zero exit code.
 */
void
creadv(fildes, iov, iovcount)
int		fildes;		/* file descriptor */
struct iovec	*iov;		/* array of iovec structures */
int		iovcount;	/* number of iovec structures */
{
	int requested, actual, i;

	/*
	 * Determine total request count.
	 */
	requested = 0;
	for (i = 0; i < iovcount; i++) {
		requested += iov[i].iov_len;
	}

	/*
	 * Perform the readv().
	 */
	actual = readv(fildes, iov, iovcount);
	if (actual == -1) {
		nx_perror("creadv");
		exit(1);
	}
	if (actual < requested) {
		errno = ERDEOF;
		nx_perror("creadv");
		exit(1);
	}
}
