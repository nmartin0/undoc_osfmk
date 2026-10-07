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
 * $Log: kloadcall.h,v $
 * Revision 1.4  1994/11/18  20:40:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:25:26  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:34:13  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:42:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:39:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:38:51  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/12/16  12:49:04  roy
 * 	Initial check-in.
 *
 * Revision 3.0  91/10/29  16:10:42  barbou
 * Unchanged copy from OSF/1.0.2.
 * 
 * Revision 1.3  90/10/07  14:51:30  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:41:07  gm]
 * 
 * Revision 1.2  90/06/22  20:53:15  devrcs
 * 	Initial Revision.
 * 	[90/06/06  21:29:26  kwallace]
 * 
 * $EndLog$
 */

/*
 * Operation Code for kloadcall(2)
 */

#define	KLC_VM_ALLOCATE		1
#define	KLC_VM_DEALLOCATE	2
#define	KLC_VM_READ		3
#define	KLC_VM_WRITE		4
#define	KLC_VM_PROTECT		5
#define	KLC_VM_ALLOCATE_WIRED	6

#define	KLC_CALL_FUNCTION	100
