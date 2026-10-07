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
 * $Log: mfsnode.h,v $
 * Revision 1.5  1994/11/18  20:45:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:37:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:41  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:30  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:04  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:36  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:37  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:18:42  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/01  17:00:06  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/12/06  14:05:05  devrcs
 * 	Cleanup copyright and history log comments.
 * 	[90/11/15  11:12:03  gm]
 * 
 * Revision 1.5  90/10/07  14:58:25  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:52  gm]
 * 
 * Revision 1.4  90/06/29  13:54:22  devrcs
 * 	Parallelized and multithreaded.
 * 	[90/06/26  11:30:47  gmf]
 * 
 * Revision 1.3  90/06/22  20:55:17  devrcs
 * 	nags merge
 * 	[90/06/12  21:41:01  gmf]
 * 
 * Revision 2.1  90/03/19  11:39:48  gmf
 * 	Condensed relevant history:
 * 	Integrated with 4.4BSD sources from 11/30/89		noemi@osf.org
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
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
 *	@(#)mfsnode.h	7.1 (Berkeley) 10/24/89
 */

#ifndef	_UFS_MFSNODE_H_
#define _UFS_MFSNODE_H_
/*
 * This structure defines the control data for the memory
 * based file system.
 */
#ifndef	OSF1_SERVER
#include <kern/thread.h>
#endif	/* OSF1_SERVER */

struct mfsnode {
	struct	vnode *mfs_vnode;	/* vnode associated with this mfsnode */
	caddr_t	mfs_baseoff;		/* base of file system in memory */
	long	mfs_size;		/* size of memory file system */
	pid_t	mfs_pid;		/* our process id */
	struct	buf *mfs_buflist;	/* list of I/O requests */
	task_t  mfs_task;		/* our task */
	int	mfs_numthreads;		/* number of threads started */
	long	mfs_spare[4];
};

/*
 * Convert between mfsnode pointers and vnode pointers
 */
#define VTOMFS(vp)	((struct mfsnode *)(vp)->v_data)
#define MFSTOV(mfsp)	((mfsp)->mfs_vnode)
#endif	_UFS_MFSNODE_H
