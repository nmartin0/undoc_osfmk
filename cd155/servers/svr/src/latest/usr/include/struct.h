/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: struct.h,v $
 * Revision 1.3  1994/11/18  21:11:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:18:57  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:21  nandy
 * Initial revision
 *
 * Revision 2.3  92/04/13  14:39:41  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.3  90/10/07  20:37:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:00:18  gm]
 * 
 * Revision 2.2  90/01/02  19:15:51  gm
 * 	Fixes for first snapshot.
 * 
 * $EndLog$
 */
/*	struct.h	4.1	83/05/03	*/

/*
 * access to information relating to the fields of a structure
 */

#define	fldoff(str, fld)	((int)&(((struct str *)0)->fld))
#define	fldsiz(str, fld)	(sizeof(((struct str *)0)->fld))
#define	strbase(str, ptr, fld)	((struct str *)((char *)(ptr)-fldoff(str, fld)))
