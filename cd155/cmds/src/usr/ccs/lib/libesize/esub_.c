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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libesize/esub_.c,v 1.3 1994/11/19 02:20:11 mtm Exp $
 *
 * FORTRAN interface for extended subtract.
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

esub_(e1, e2, e3)
esize_t	*e1;
esize_t	*e2;
esize_t	*e3;
{
	errno = ESUCCESS;

	*e3 = _esub(*e1, *e2);
	if ((e3->shigh == -1) && (e3->slow == -1) &&( errno != ESUCCESS)) {
		nx_perror("esub");
		exit(1);
	}
}
