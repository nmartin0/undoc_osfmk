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
 *      Copyright 1994  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/allocator/attributes.h,v 1.3 1994/11/19 03:04:10 mtm Exp $
 *
 */

/* History:
 *	$Log: attributes.h,v $
 * Revision 1.3  1994/11/19  03:04:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/06/08  17:16:54  mag
 * Correct core dumps when user passes bad args to showpart
 *  Reviewer: gregt
 *  Risk: Low
 *  Benefit or PTS #: 9720
 *  Testing: EATS: rmcmd
 *  Module(s): allocator/{attribute.c, attribute.h, init_appl.c, mkpart_rpc.c}
 * 	    showpart/showpart.c
 *  Related: None
 *
 * Revision 1.1  1994/06/01  20:46:31  mag
 * Initial revision
 *
 *
*/

typedef struct {
	int	m_nAttributes;		/* # of attributes for this node */
	int*	m_pnValues;		/* array of attribute values */
	char**	m_ppszAttributes;	/* array of attribute strings */
} ATTRIBUTE_TABLE_T;	


int BuildMatchingBitmap(int, char*, int, BITMAP_T** res);
