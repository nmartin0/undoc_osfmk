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
 * $Log: s5mount.h,v $
 * Revision 1.5  1994/11/18  20:38:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:18:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:12:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:14  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:38:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:36:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:33:59  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:32:25  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:57:37  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/06/27  15:40:10  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3.4.2  91/06/21  12:52:28  garyf
 * 	merge fixes from 1.1
 * 	[91/06/21  12:48:29  garyf]
 * 
 * Revision 1.5.2.2  91/06/17  11:12:43  garyf
 * 	add lock for superblock
 * 	[91/06/17  11:04:30  garyf]
 * 
 * Revision 1.3  90/10/07  14:41:24  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:26:01  gm]
 * 
 * Revision 1.2  90/04/27  19:21:27  devrcs
 * 	Merge for snapshot.
 * 	[90/04/20  17:34:53  morris]
 * 
 * 	Initial version from system V.2.
 * 	[90/04/20  15:16:28  morris]
 * 
 * $EndLog$
 */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * All rights reserved.
 *
 *	@(#)s5mount.h	
 */

#ifndef _S5_MOUNT_H_
#define	_S5_MOUNT_H_

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */

struct	s5fsmount {
	struct	mount *um_mountp;	/* vfs structure for this fs */
	dev_t	um_dev;			/* device mounted */
	struct	vnode *um_devvp;	/* vnode for block device mounted */
	struct	filsys *um_fs;		/* pointer to superblock */
	lock_data_t um_fsflock;		/* lock over superblock free list */
	lock_data_t um_fsilock;		/* lock over superblock inode list */
	char	um_mntname[MNAMELEN];	/* mounted filesystem */
};
#ifdef KERNEL
/*
 * Convert mount ptr to s5fsmount ptr.
 */
#define VFSTOS5FS(mp)	((struct s5fsmount *)((mp)->m_data))

/*
 * mount table
 */
extern struct s5fsmount	s5fs_mounttab[NMOUNT];

#endif	KERNEL

#endif _S5_MOUNT_H_








