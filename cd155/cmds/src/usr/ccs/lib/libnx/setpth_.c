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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/setpth_.c,v 1.2 1994/11/19 02:32:36 mtm Exp $
 *
 * FORTRAN interface to set_provide_threshold
 */


#include <sys/errno.h>

extern int errno;

set_provide_threshold_(provide_threshold)

unsigned long	*provide_threshold;
{
restart:
	if (_set_provide_threshold(*provide_threshold) < 0) {
		if (errno == EINTR)
			goto restart;
		nx_perror("set_provide_threshold");
		exit(1);
	}
}
