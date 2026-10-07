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
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/csend_.c,v 1.3 1994/11/19 02:29:19 mtm Exp $
 *
 * FORTRAN interface to csend
 */


#include <sys/errno.h>

extern int errno;

csend_(type, buf, len, node, pid, buflen)

long	*type, *len, *node, *pid;
char	*buf;
long	buflen;

{
restart:
	if (_csend(*type, buf, *len, *node, *pid) < 0) {
		if (errno == EINTR)
			goto restart;
		nx_perror("csend");
		exit(1);
	}
}
