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
 *		INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corporation
 *	and may not be copied or disclosed except in accordance
 *	with the terms of that agreement.
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/irecvx_.c,v 1.3 1994/11/19 02:30:52 mtm Exp $
 *
 * FORTRAN interface to irecvx
 */


#include <sys/errno.h>

extern int errno;

irecvx_(typesel, buf, count, nodesel, ptypesel, info, buflen, infolen)

long 	*typesel; 
char 	*buf;
long	*count;
long    *nodesel;
long    *ptypesel;
long    *info;
long 	buflen;
long 	infolen;
{
	long retval;

	if ((retval = _irecvx(*typesel, buf, *count, *nodesel, *ptypesel, info)) < 0) {
		nx_perror("irecvx");
		exit(1);
	}
	return (retval);
}
