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
 *	$Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libesize/etos.c,v 1.3 1994/11/19 02:20:12 mtm Exp $
 *
 * Extended number to string.
 *
 *      HISTORY:
 *      $Log: etos.c,v $
 * Revision 1.3  1994/11/19  02:20:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/01/07  02:19:36  brad
 * Merged libesize modifications from R1.2 branch.
 *
 * Revision 1.1.2.1  1994/01/06  18:53:01  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): libesize/*.c
 *
 * Revision 1.1  1993/09/28  00:52:58  brad
 * Initial revision
 *
 * Revision 1.3  1992/08/05  10:32:47  dbm
 * Added History.
 *
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"

void
etos(e, s)
esize_t	 e;
char	*s;
{
	errno = ESUCCESS;

	if (_etos(e, s) < 0) {
		nx_perror("etos");
		exit(1);
	}
}
