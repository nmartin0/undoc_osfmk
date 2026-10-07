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
 *  Copyright 1992  Intel Corporation.
 *
 *  $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/ccs/lib/libnx/restrictvol.c,v 1.4 1994/11/19 02:32:26 mtm Exp $
 *
 * C interface to do restrictvol on a PFS file.
 *
 *      HISTORY:
 *      $Log: restrictvol.c,v $
 * Revision 1.4  1994/11/19  02:32:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1992/08/05  11:56:32  dbm
 * Added History.
 *
 *
 */

#include <errno.h>

long
restrictvol(fid, nvol, vollst)
int fid;
int nvol;
int *vollst;
{
	return(ESUCCESS);
}


long
_restrictvol(fid, nvol, vollst)
int fid;
int nvol;
int *vollst;
{
	return(ESUCCESS);
}

