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
 * $Log: lock.h,v $
 * Revision 1.4  1994/11/18  20:40:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:29  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:01  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/15  11:40:47  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  13:08:02  sjs
 *	Initial revision in this tree.
 *
 * Revision 3.1  91/09/16  10:07:05  condict
 * Add this file for users.  Required by AES.
 * 
 * Revision 1.3  90/10/07  14:51:48  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:34  gm]
 * 
 * Revision 1.2  90/04/27  19:26:10  devrcs
 * 	Created
 * 	[90/04/20  16:14:25  ers]
 * 
 * $EndLog$
 */
/*
 * sys/lock.h: flags for locking (wiring) process regions
 */
#define	UNLOCK	 0
#define	PROCLOCK 1
#define	TXTLOCK	 2
#define	DATLOCK	 4
