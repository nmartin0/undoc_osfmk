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
 * $Log: filsys.h,v $
 * Revision 1.5  1994/11/18  20:38:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:11:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:28:23  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:37:44  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:35:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:33:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:31:18  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:55:03  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/06/27  15:36:52  sp
 * Incorporated from 1.0.2
 * 
 * Revision 1.3.4.2  91/06/21  12:51:28  garyf
 * 	merge fixes from 1.1
 * 	[91/06/21  12:47:45  garyf]
 * 
 * Revision 1.4.2.2  91/06/17  11:11:39  garyf
 * 	add locking macros
 * 	[91/06/17  11:03:27  garyf]
 * 
 * Revision 1.4  91/03/04  17:19:27  devrcs
 * 	A small step toward ansiC: commented else/endif/elif trailers.
 * 	[91/01/12  17:00:08  dwm]
 * 
 * Revision 1.3  90/10/07  14:40:28  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:24:33  gm]
 * 
 * Revision 1.2  90/04/27  19:20:38  devrcs
 * 	Merge for snapshot.
 * 	[90/04/20  17:28:16  morris]
 * 
 * 	Initial version from system V.2
 * 	[90/04/17  14:43:27  morris]
 * 
 * 	[90/04/02  12:26:26  morris]
 * 
 * $EndLog$
 */
/* @(#)filsys.h	6.1 */
#ifndef _S5_FILSYS_H_
#define _S5_FILSYS_H_

/*
 * Structure of the super-block
 */
struct	filsys
{
	ushort	s_isize;	/* size in blocks of i-list */
	daddr_t	s_fsize;	/* size in blocks of entire volume */
	short	s_nfree;	/* number of addresses in s_free */
	daddr_t	s_free[NICFREE];	/* free block list */
	short	s_ninode;	/* number of i-nodes in s_inode */
	s5ino_t	s_inode[NICINOD];	/* free i-node list */
	char	s_flock;	/* lock during free list manipulation */
	char	s_ilock;	/* lock during i-list manipulation */
	char  	s_fmod; 	/* super block modified flag */
	char	s_ronly;	/* mounted read-only flag */
	time_t	s_time; 	/* last super block update */
	short	s_dinfo[4];	/* device information */
	daddr_t	s_tfree;	/* total free blocks*/
	s5ino_t	s_tinode;	/* total free inodes */
	char	s_fname[6];	/* file system name */
	char	s_fpack[6];	/* file system pack name */
	long	s_fill[13];	/* ADJUST to make sizeof filsys be 512 */
	long	s_magic;	/* magic number to indicate new file system */
	long	s_type;		/* type of new file system */
};

#define	FsMAGIC	0xfd187e20	/* s_magic number */
#define	Fs1b	1	/* 512  byte block */
#define	Fs2b	2	/* 1024 byte block */
#define	Fs3b	3	/* 2048 byte block */

#ifdef _KERNEL
#define s5FS_FLOCK(ump)				\
	lock_write(&(ump)->um_fsflock)
#define s5FS_FUNLOCK(ump)			\
	lock_done(&(ump)->um_fsflock)
#define s5FS_ILOCK(ump)				\
	lock_write(&(ump)->um_fsilock)
#define s5FS_IUNLOCK(ump)			\
	lock_done(&(ump)->um_fsilock)
#endif /* _KERNEL */

#endif  /*_S5_FILSYS_H_*/
