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
 * $Log: s5ino.h,v $
 * Revision 1.5  1994/11/18  20:38:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:18:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:12:39  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:05  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:38:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:36:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:33:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:32:15  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:57:14  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/06/27  15:40:03  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3  90/10/07  14:41:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:25:48  gm]
 * 
 * Revision 1.2  90/04/27  19:21:20  devrcs
 * 	Merge for snapshot.
 * 	[90/04/20  17:32:22  morris]
 * 
 * 	Initial version from system V.2.
 * 	[90/04/02  12:44:05  morris]
 * 
 * $EndLog$
 */
/* @(#)s5ino.h	 */

#ifndef	_S5_INO_H_
#define _S5_INO_H_

/* Inode structure as it appears on a disk block. */

struct s5dinode
{
	ushort  di_mode;	/* mode and type of file */
	short	di_nlink;    	/* number of links to file */
	ushort	di_uid;      	/* owner's user id */
	ushort	di_gid;      	/* owner's group id */
	off_t	di_size;     	/* number of bytes in file */
	char  	di_addr[40];	/* disk block addresses */
	time_t	di_atime;   	/* time last accessed */
	time_t	di_mtime;   	/* time last modified */
	time_t	di_ctime;   	/* time created */
};

/*
 * the 40 address bytes:
 *	39 used; 13 addresses
 *	of 3 bytes each.
 */
#endif	_S5_INO_H
