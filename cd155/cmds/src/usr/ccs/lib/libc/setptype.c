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
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Id: setptype.c,v 1.3 1994/11/19 02:08:08 mtm Exp $
 *
 * HISTORY
 * $Log: setptype.c,v $
 * Revision 1.3  1994/11/19  02:08:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/10/05  14:56:48  sdh
 * Added an _setptype stub routine so non-nx apps will
 * link ok with nx_setup.
 *
 *  Reviewer:
 *  Risk: low
 *  Benefit or PTS #: 11083 related
 *  Testing: manual
 *  Module(s):
 * 	setptype.c
 *
 * Revision 1.1  1993/10/25  15:07:23  shala
 * Moved it from libc/IPSC860 to libc directory, so it does not get included to
 * libc_r.a. Fixed bug #4516.
 *
 * Revision 1.1  1992/06/10  16:42:25  stans
 * Initial revision
 *
 *
 */

/*
 * This is dummy routine which will be replaced with the one
 * in libnx when linking for parallel application using -nx or -lnx switch.
 *
 */
int
setptype( ptype ) 
int	ptype;
{
	return;
}

int
_setptype( ptype )
int     ptype;
{
        return;
}

