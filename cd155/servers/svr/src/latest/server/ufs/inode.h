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
 * This source file was modified by the Center for High Performance 
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: inode.h,v $
 * Revision 1.6  1994/11/18  20:45:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/06/28  22:57:37  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.4  1993/07/14  18:37:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:26  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:59  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.12  93/05/13  16:45:56  roy
 * 	Remove i_ressize, add i_writesize to inode.  Added ISIZ flag and
 * 	modified IUPDAT to check it.
 * 	[93/05/04            roy]
 * 
 * Revision 2.11  93/03/23  11:57:45  condict
 * 	Eliminate references to read_count field of locks. Use LOCK_READERS
 * 	macro defined in kern/lock.h instead.
 *
 * Revision 1.2  1992/11/30  22:50:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:53  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:26  cfj
 * Bump major revision number.
 *
 * Revision 2.13  94/02/03  11:01:38  dnoveck
 *      Externals to support per-ndoe buffer-cache block size logic:
 *      ufs_bcmax_din{count,shift} and temp_fs_zone.
 *
 *
 * Revision 2.10  1992/09/11  09:28:04  rabii
 * 	RESERVE, DADDR, IS_RESERVED macros moved to dinode.h.
 * 	[92/08/26            roy]
 * 
 * Revision 2.9  92/08/26  12:12:10  loverso
 * 	Added RESERVE, DADDR, IS_RESERVED macros to support reserved daddrs.
 * 	Remove include of sys/file_info.h and related junk from inode.
 * 	Added i_resfrags, i_ressize and i_truesize to struct inode.
 * 	ISETFLAGS no longer needed (functionality moved to ufs_update).
 * 	[92/07/20            roy]
 * 
 * Revision 2.8  92/06/08  18:22:35  pjg
 * 	Implemented ISETFLAGS for OSF1_ADFS. (roy)
 * 
 * Revision 2.7  92/03/15  14:41:26  roy
 * 	Add extern for 'this_node' for GETNODE_FROM_INODE, etc. macros.
 * 
 * Revision 2.6  92/02/21  16:26:24  durriya
 * 	add GETNODE_FROM_INODE and PUTNODE_FROM_INODE macros (sjs)
 * 
 * Revision 2.5  92/01/05  19:27:51  roy
 * 	1991/11/12  19:38:01  noemi
 * 	Change i_rnode to i_node.
 * 
 * Revision 2.4  91/11/26  13:35:03  rabii
 * 	Modified to use a spare field for rnode
 * 
 * Revision 2.3  91/11/25  11:27:45  rabii
 * 	Added support for remote devices
 * 
 * Revision 2.2  91/08/31  14:18:19  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/01  16:59:47  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10  90/10/07  14:58:10  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:31  gm]
 * 
 * Revision 1.9  90/09/23  16:00:33  devrcs
 * 	For the few occasions we need to construct dummy
 * 	inodes, define the size of such a beast.
 * 	[90/09/08  19:03:47  nags]
 * 
 * 	Inode quota flags.
 * 	[90/09/03  22:36:31  nags]
 * 
 * Revision 1.8  90/08/24  12:28:20  devrcs
 * 	Place include <quota.h> inside of _KERNEL ifdef.
 * 	[90/08/19  20:17:07  gm]
 * 
 * 	Replaced old, useless quota code with (unparallelized)
 * 	4.3-Reno quota code.
 * 	[90/08/19  01:22:52  nags]
 * 
 * Revision 1.7  90/06/22  20:55:07  devrcs
 * 	Added INACTWAIT flag for inode creation/deletion races.
 * 	[90/06/18  17:10:39  nags]
 * 
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:57:07  seiden]
 * 
 * 	nags merge
 * 	[90/06/12  21:40:39  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:47:04  seiden]
 * 
 * 	Condensed ancient history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	FIFOs force IFTOVT to use all file type bits.	ers@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Merged 4.4BSD and Mach X115, Encore 0.6:	gm@osf.org
 * 	  Encore's fast symbolic link support in inode (MACH_FASTLINK).
 * 	  Encore parallelization support.
 * 	[90/06/10  02:08:45  seiden]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1982, 1989 The Regents of the University of California.
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
 *	@(#)inode.h	7.10 (Berkeley) 1/2/90
 */

#ifndef	_UFS_INODE_H_
#define _UFS_INODE_H_

#ifdef	_KERNEL
#include <quota.h>
#endif

#include <sys/secdefines.h>
#include <ufs/dinode.h>
#include <sys/types.h>
#include <ufs/quota.h>
#include <machine/endian.h>
#ifdef	_KERNEL
#include <sys/unix_defs.h>
#include <kern/zalloc.h>
#include <kern/event.h>
#include <norma_ipc.h>
#include <mapped_files.h>
#endif

#if	SEC_FSCHANGE
#include <sys/security.h>
#endif
#if	QUOTA
#include <ufs/quota.h>
#endif

/*
 * The I node is the focus of all file activity in UNIX.
 * There is a unique inode allocated for each active file,
 * each current directory, each mounted-on file, text file, and the root.
 * An inode is 'named' by its dev/inumber pair. (iget/iget.c)
 * Data in `struct dinode' is read in from permanent inode on volume.
 */

struct inode {
	struct	inode *i_chain[2]; /* hash chain, MUST be first */
	struct	vnode *i_vnode;	/* vnode associated with this inode */
	struct	vnode *i_devvp;	/* vnode for block I/O */
	u_long	i_flag;		/* see below */
	dev_t	i_dev;		/* device where inode resides */
	ino_t	i_number;	/* i number, 1-to-1 with device address */
	struct	fs *i_fs;	/* file sys associated with this inode */
	long	i_diroff;	/* offset in dir, where we found last entry */
	off_t	i_endoff;	/* end of useful stuff in directory */
	short	i_dirstamp;	/* optimize directory insertions */
	struct	dinode i_din;	/* the on-disk inode */
	struct	dquot *i_dquot[MAXQUOTAS]; /* pointer to dquot structures */
#if	SEC_FSCHANGE
	struct	dinode_sec i_disec;	/* security extension */
#endif
#ifdef	_KERNEL
#if	MAPPED_FILES
	/*
	 * Support for block reservation:  track how many frags in each
	 * direct block are reserved.
	 */
	short		i_resfrags[NDADDR];	/* number of frags reserved */

	/*
	 * Track the file size as updated via ufs_update.  It is different
	 * from the file size in the dinode in that it may be greater than the
	 * actual amount of valid data on disk.
	 */
	int		i_truesize;		/* file size */

	/*
	 * Track the amount of data that's been written back from the
	 * memory object cache to disk.  It's used to ensure that i_size
	 * isn't updated before the data is written back when a file is grown.
	 */
	int		i_writesize;		
#endif
	lock_data_t	i_io_lock;	/* reading/writing file's contents */
	event_t		i_iodone;	/* inode is in transit from disk */
	udecl_simple_lock_data(,i_incore_lock)
#endif
};

typedef	struct inode	inode_t;

#define	i_mode		i_din.di_mode
#define	i_nlink		i_din.di_nlink
#define	i_uid		i_din.di_uid
#define	i_gid		i_din.di_gid
#if	BYTE_ORDER == LITTLE_ENDIAN
#define	i_size		i_din.di_qsize.val[0]
#else
#define	i_size		i_din.di_qsize.val[1]
#endif
#define	i_db		i_din.di_db
#define	i_ib		i_din.di_ib
#define i_symlink	i_din.di_symlink
#define	i_atime		i_din.di_atime
#define	i_mtime		i_din.di_mtime
#define	i_ctime		i_din.di_ctime
#define i_blocks	i_din.di_blocks
#define	i_rdev		i_din.di_db[0]
#define i_flags		i_din.di_flags
#define i_gen		i_din.di_gen
#define	i_forw		i_chain[0]
#define	i_back		i_chain[1]
#ifdef	OSF1_ADFS
#define	i_node		i_din.di_node
#endif
#if SEC_FSCHANGE
#define i_gpriv		i_disec.di_gpriv
#define i_ppriv		i_disec.di_ppriv
#define i_tag		i_disec.di_tag
#define i_parent	i_disec.di_parent
#define i_type_flags	i_disec.di_type_flags
#endif

/* flags */
#define	IRENAME		0x0004		/* inode is being renamed */
#define	ISIZ		0x0008		/* file size has been modified */
#define	IUPD		0x0010		/* file has been modified */
#define	IACC		0x0020		/* inode access time to be updated */
#define	ICHG		0x0040		/* inode has been changed */
#define	IMOD		0x0080		/* inode has been modified */
#define	ISHLOCK		0x0100		/* file has shared lock */
#define	IEXLOCK		0x0200		/* file has exclusive lock */
#define	ILWAIT		0x0400		/* someone waiting on file lock */
#define	INACTIVATING	0x0800		/* inode is being inactivated */
#define	INACTWAIT	0x1000		/* waiting for inode inactivation */
#define	IREADERROR	0x2000		/* disk read for inode failed */
#define	IQUOTA		0x4000		/* inode has quotas attached */
#define	IQUOTING	0x8000		/* quota attach in progress */
#define	IQUOTWAIT	0x10000		/* waiting while attach progresses */

#ifdef	_KERNEL
#include <kern/macro_help.h>

/*
 * Convert between inode pointers and vnode pointers
 */
#define VTOI(vp)	((struct inode *)(vp)->v_data)
#define ITOV(ip)	((ip)->i_vnode)

/*
 * Convert between vnode types and inode formats
 */
extern enum vtype	iftovt_tab[];
extern int		vttoif_tab[];
#define IFTOVT(mode)	(iftovt_tab[((mode) & IFMT) >> 12])
#define VTTOIF(indx)	(vttoif_tab[(int)(indx)])

#define MAKEIMODE(indx, mode)	(int)(VTTOIF(indx) | (mode))

extern ino_t	dirpref();
extern u_long	get_nextgen();

/*
 * Occasionally a fake inode must be built ``by-hand''.
 */
#define	FAKE_INODE_SIZE	(sizeof(struct vnode)+sizeof(struct inode))

/*
 * Inode locking.			XXX sadly out of date XXX
 *
 * The phrase "read-only [after in-core inode creation]" mean that as long
 * as the code examining the field holds a reference to the inode, the inode
 * won't be deallocated -- and as long as the inode is allocated, the
 * contents of the field never change.
 *
 * File I/O is handled as follows.  For IFREG files, file reads and writes
 * take the inode I/O lock for reading or writing, as appropriate.  For IFDIR
 * files, the I/O lock is taken for writing when atomic operations are desired
 * (e.g., examining then updating a directory.)  The I/O lock is taken for
 * reading when the only operation to be done is examining the contents of the
 * directory.
 *
 * For IFIFO files.
 *
 * For IFBLK files and IFCHR files, no inode lock whatsoever is taken across
 * an I/O operation.  All locking is at the discretion of the lower-level
 * driver.
 *
 * Inode locking requirements per field:
 *
 *	i_chain[2]		protected by inode hash chain lock
 *	i_vnode			internal vnode locks, cf vnode.h
 *	i_devvp			incore lock
 *	i_flag			incore lock
 *	i_dev			read-only
 *	i_number		read-only
 *	i_diroff		incore lock
 *	i_fs			read-only
 *	i_dquot			governed by IQUOTA and ufsmount's quota lock
 *	di_mode			incore lock (no lock needed to check IFMT)
 *	di_nlink		incore lock
 *	di_uid			incore lock
 *	di_gid			incore lock
 *	di_size			incore lock (only modified under i/o writelock)
 *	di_atime		incore lock
 *	di_atspare		n/a
 *	di_mtime		incore lock
 *	di_mtspare		n/a
 *	di_ctime		incore lock
 *	di_ctspare		n/a
 *	union
 *		di_db & di_ib	inode I/O lock (usually held for writing)
 *		di_symlink	read-only
 *	di_flags		IC_FASTLINK is read-only
 *	di_blocks		inode I/O lock
 *	di_gen			read-only
 *	di_spare[]		n/a
#if SEC_FSCHANGE
 *	i_gpriv			TBD
 *	i_ppriv			TBD
 *	i_tag			TBD
 *	i_parent		TBD
 *	i_type_flags		TBD
#endif
#if	QUOTA
 * Special considerations apply to the inode's i_dquot array.  Refer to
 * ufs/ufs_quota.c for a description of the locking protocol.
#endif
 */

#define	IN_READ_LOCK(ip)	lock_read(&(ip)->i_io_lock)
#define	IN_WRITE_LOCK(ip)	lock_write(&(ip)->i_io_lock)
#define	IN_WRITE_LOCK_TRY(ip)	lock_try_write(&(ip)->i_io_lock)
#define	IN_READ_UNLOCK(ip)	lock_read_done(&(ip)->i_io_lock)
#define	IN_WRITE_UNLOCK(ip)	lock_write_done(&(ip)->i_io_lock)
#define	IN_SET_RECURSIVE(ip)	lock_set_recursive(&(ip)->i_io_lock)
#define IN_CLEAR_RECURSIVE(ip)	lock_clear_recursive(&(ip)->i_io_lock)
#define	IN_IO_LOCK_INIT(ip)	lock_init2(&(ip)->i_io_lock, TRUE, \
					   LTYPE_INODE_IO)

#define	IN_LOCK(ip)		usimple_lock(&(ip)->i_incore_lock)
#define	IN_UNLOCK(ip)		usimple_unlock(&(ip)->i_incore_lock)
#define	IN_LOCK_INIT(ip)	usimple_lock_init(&(ip)->i_incore_lock)
#define	IN_LOCK_TRY(ip)		usimple_lock_try(&(ip)->i_incore_lock)

#define	IN_WRITE_HOLDER(ip)	LOCK_HOLDER(&(ip)->i_io_lock)
#define	IN_READ_HOLDER(ip)	LOCK_READERS(&(ip)->i_io_lock)

/*
 * To supoort backward compatibility, we have a hack! If we're setting the node
 * number in the inode through the AD server interface, we set the high bit on.
 * This allows us to use use the high bit as a magic cookie to distinguish
 * between valid norma node numbers in the inode and old untouched inodes, the
 * latter case implying that the device is on the current node instead of the
 * remote node 0.
 *
 * In the non-norma_ipc case, the node number should always be this_node which
 * will prevent us from going remote.
 */

#ifdef	OSF1_ADFS
extern	node_t	this_node;

#if	NORMA_IPC
#define GETNODE_FROM_INODE(ip)  (((ip)->i_node & 0x80000000) ? ((ip)->i_node & 0x7fffffff) : (this_node))
#else
#define GETNODE_FROM_INODE(ip)  (this_node)
#endif

#define PUTNODE_IN_INODE(ip,n)    ((ip)->i_node = (n) | 0x80000000)

#endif	OSF1_ADFS

/*
 * Avoid locking inode here in favor of re-checking under lock
 * in iupdat.			XXX
 */
#define IUPDAT(ip, t1, t2, waitfor)		\
MACRO_BEGIN					\
	if (ip->i_flag&(IUPD|IACC|ICHG|IMOD|ISIZ))	\
		(void) iupdat(ip, t1, t2, waitfor); \
MACRO_END

#define	ITIMES(ip, t1, t2)	itimes((ip), (t1), (t2))

/*
 * This overlays the fid structure (see mount.h)
 */
struct ufid {
	u_short	ufid_len;	/* length of structure */
	u_short	ufid_pad;	/* force long alignment */
	ino_t	ufid_ino;	/* file number (ino) */
	long	ufid_gen;	/* generation number */
};


extern int	inohsz;		/* elements in inode hash table */

#define	MININOHSZ	64	/* minimum size of inode hash table */

#define INOHASH(dev,ino)	(((dev)+(ino))&(inohsz-1))

struct ihead {				/* inode LRU cache, Chris Maltby */
	union {
		struct ihead	*iu_head[2];
		struct inode	*iu_chain[2];
	} ih_un;
	int	ih_timestamp;		/* track hash chain insertions */
	udecl_simple_lock_data(,ih_lock) /* MP exclusion */
};

extern struct ihead *ihead;

#define	ih_head		ih_un.iu_head
#define	ih_chain	ih_un.iu_chain

#define	IHASH_LOCK(ih)		usimple_lock(&(ih)->ih_lock)
#define	IHASH_UNLOCK(ih)	usimple_unlock(&(ih)->ih_lock)
#define	IHASH_LOCK_TRY(ih)	usimple_lock_try(&(ih)->ih_lock)
#define	IHASH_LOCK_INIT(ih)	usimple_lock_init(&(ih)->ih_lock)

/*
 * Fast link runtime controls
 */
extern	int		create_fastlinks;


extern	zone_t	temp_fs_zone;

extern	int	ufs_bcmax_dincount;
extern	int	ufs_bcmax_dinshift;

#endif  /* _KERNEL */
#endif	/* _UFS_INODE_H_ */
