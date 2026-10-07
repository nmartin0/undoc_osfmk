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
 * Add extended number with integer.
 *
 * HISTORY:
 * $Log: eadd1.c,v $
 * Revision 1.3  1994/11/19  02:19:59  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/01/07  02:19:24  brad
 * Merged libesize modifications from R1.2 branch.
 *
 * Revision 1.1.2.1  1994/01/06  18:52:45  brad
 * Checking in results of an extended math code review by Greg Tensa.
 *
 *  Reviewer: Greg Tensa, Brad Rullman.
 *  Risk: Low.
 *  Benefit or PTS #: 7150,5958,5213,5215,5216,1632.
 *  Testing: Ran PFS EATs.
 *  Module(s): libesize/*.c
 *
 * Revision 1.1  1993/09/28  00:51:41  brad
 * Initial revision
 *
 * Revision 1.5  1993/04/02  22:41:55  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/01/21  17:25:33  brad
 * Added some real comments. mul() and div() are now mulx() and divx()
 * to avoid naming clashes with OSF/1 libc functions.
 *
 * Revision 1.3.2.1  1993/01/21  17:21:22  brad
 * Removed extern declaration of _eadd1() - this is now in nx.h.
 *
 * Revision 1.3  1992/08/05  09:36:03  dbm
 * Added History.
 *
 */

#include <errno.h>
#include <sys/types.h>
#include "nx.h"


esize_t
eadd1(e, n)
esize_t	e;
long	n;
{
	esize_t	sum;

	errno = ESUCCESS;

	sum = _eadd1(e, n);
	if ((sum.shigh == -1) && (sum.slow == -1) && (errno != ESUCCESS)) {
		nx_perror("eadd1");
		exit(1);
	}

	return(sum);
}
