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
 * $Log: _creadv.c,v $
 * Revision 1.2  1994/11/19  02:28:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/03/20  23:46:36  brad
 * Initial revision
 *
 */

#include <sys/types.h>
#include <sys/uio.h>

int
_creadv(fildes, iov, iovcount)
int		fildes;		/* file descriptor */
struct iovec	*iov;		/* array of iovec structures */
int		iovcount;	/* number of iovec structures */
{
	return(readv(fildes, iov, iovcount));
}
