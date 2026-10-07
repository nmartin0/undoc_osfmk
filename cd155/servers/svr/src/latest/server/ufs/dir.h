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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: dir.h,v $
 * Revision 1.8  1994/11/18  20:45:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/28  22:56:01  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.6  1993/07/14  18:37:15  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:08  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/22  18:27:06  brad
 * Modified previous revision to use new constant, MINDEV_BSIZE.
 *
 * Revision 1.4  1993/05/22  18:09:47  brad
 * Changed #define of DIRBLKSIZ to 512, instead of DEV_BSIZE.  512 bytes is
 * the smallest atomic read/write unit of any Paragon driver.
 *
 * Revision 1.3  1993/05/06  20:30:21  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:54  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:42  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:12  cfj
 * Bump major revision number.
 *
 * Revision 2.3  93/10/20  15:32:27  dnoveck
 *      For DEV_BSIZE eliminatin: Don't define DIRBLKSIZ in terms
 *      of DEV_BSIZE.
 *
 * Revision 2.2  1991/08/31  14:17:54  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/01  16:59:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:57:56  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:09  gm]
 * 
 * Revision 1.5  90/07/17  11:42:55  devrcs
 * 	Removed information defined in dirent.h and changed to include
 * 	that information directly.
 * 	[90/07/06  13:35:27  gm]
 * 
 * Revision 1.4  90/07/05  23:14:54  devrcs
 * 	Changed to get user-space definitions from <dirent.h>.
 * 	[90/07/02  17:41:22  gm]
 * 
 * Revision 1.3  90/01/02  20:23:09  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:44:34  gm
 * 	Condensed history
 * 	  Merged 4.4 version with CMU X115
 * 	  CMU history
 * 	    use dynamically allocated buffer for _dirdesc struct
 * 	    changed rewinddir from macro to routine
 * 	    added declarations for __STDC__
 * 	[89/12/25            gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)dir.h	7.6 (Berkeley) 5/9/89
 */


/****************************************************************************
*                                                                           *
*   *********************************************************************   *
*   *                                                                   *   *
*   *   WARNING: Some of the defnitions below are duplicated in the     *   *
*   *            kernel header file boot_ufs/dir.h  Until someone       *   *
*   *            fixes this, please keep them in sync.                  *   *
*   *                                                                   *   *
*   *********************************************************************   *
*                                                                           *
****************************************************************************/


#ifndef	_UFS_DIR_H_
#define _UFS_DIR_H_

#include <sys/types.h>

/*
 * A directory consists of some number of blocks of DIRBLKSIZ
 * bytes, where DIRBLKSIZ is chosen such that it can be transferred
 * to disk in a single atomic operation (e.g. 512 bytes on most machines).
 *
 * Each DIRBLKSIZ byte block contains some number of directory entry
 * structures, which are of variable length.  Each directory entry has
 * a struct direct at the front of it, containing its inode number,
 * the length of the entry, and the length of the name contained in
 * the entry.  These are followed by the name padded to a 4 byte boundary
 * with null bytes.  All names are guaranteed null terminated.
 * The maximum length of a name in a directory is MAXNAMLEN.
 *
 * The macro DIRSIZ(dp) gives the amount of space required to represent
 * a directory entry.  Free space in a directory is represented by
 * entries which have dp->d_reclen > DIRSIZ(dp).  All DIRBLKSIZ bytes
 * in a directory block are claimed by the directory entries.  This
 * usually results in the last entry in a directory having a large
 * dp->d_reclen.  When entries are deleted from a directory, the
 * space is returned to the previous entry in the same directory
 * block by increasing its dp->d_reclen.  If the first entry of
 * a directory block is free, then its dp->d_ino is set to 0.
 * Entries other than the first in a directory do not normally have
 * dp->d_ino set to 0.
 */
#define DIRBLKSIZ	512

#ifdef _KERNEL
/*
 * Template for manipulating directories.
 * Should use struct direct's, but the name field
 * is MAXNAMLEN - 1, and this just won't do.
 */
struct dirtemplate {
	u_long	dot_ino;
	short	dot_reclen;
	short	dot_namlen;
	char	dot_name[4];		/* must be multiple of 4 */
	u_long	dotdot_ino;
	short	dotdot_reclen;
	short	dotdot_namlen;
	char	dotdot_name[4];		/* ditto */
};
#endif /* _KERNEL */

#include <dirent.h>
#endif	_UFS_DIR_H_
