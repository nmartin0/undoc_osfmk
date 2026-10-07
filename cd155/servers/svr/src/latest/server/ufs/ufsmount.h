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
 * $Log: ufsmount.h,v $
 * Revision 1.5  1994/11/18  20:46:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:38:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:55:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:18  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:50:20  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1993/03/23  11:57:55  condict
 * 	Eliminate references to read_count field of locks. Use LOCK_READERS
 * 	macro defined in kern/lock.h instead.
 *
 * Revision 1.2  1992/11/30  22:51:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:40:17  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:49:26  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  14:21:18  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:01:58  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/07  14:59:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:07  gm]
 * 
 * 	Re-introduced quota flags for use in quotaoff.
 * 	[90/09/24  15:51:35  nags]
 * 
 * Revision 1.7  90/09/23  16:01:13  devrcs
 * 	New quota locking.
 * 	[90/09/03  22:37:55  nags]
 * 
 * Revision 1.6  90/08/24  12:29:43  devrcs
 * 	Replaced old, useless quota code with (unparallelized)
 * 	4.3-Reno quota code.
 * 	[90/08/18  03:53:16  nags]
 * 
 * Revision 1.5  90/06/22  20:56:07  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Get rid of um_mntname.				gmf@osf.org
 * 	Avoid mount hangs by using mount stat struct.	gmf@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Merged 4.4BSD and Mach changes.			noemi@osf.org
 * 	[90/06/12  21:42:35  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
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
 *	@(#)ufsmount.h	7.3 (Berkeley) 5/9/89
 */

#ifndef _UFS_UFSMOUNT_
#define	_UFS_UFSMOUNT_

#ifdef	_KERNEL
#include <kern/lock.h>
#endif

/*
 * Mount structure.
 * One allocated on every mount.
 * Used to find the super block.
 */
struct	ufsmount {
	struct	mount *um_mountp;	/* vfs structure for this filesystem */
	dev_t	um_dev;			/* device mounted */
	struct	vnode *um_devvp;	/* vnode for block device mounted */
	struct	fs *um_fs;		/* pointer to superblock */
	struct	vnode *um_quotas[MAXQUOTAS]; /* pointer to quota files */
	struct	ucred *um_cred[MAXQUOTAS]; /* cred for access to quota file */
	time_t	um_btime[MAXQUOTAS];	/* block quota time limit */
	time_t	um_itime[MAXQUOTAS];	/* inode quota time limit */
	char	um_qflags[MAXQUOTAS];	/* quota specific flags, see below */
	char	um_qsync;		/* qsync in progress */
#ifdef	_KERNEL
	decl_simple_lock_data(,um_qsync_lock) /* protect um_qsync */
	lock_data_t	um_lock;	/* quota read/write lock */
#endif
};

#define	QTF_OPENING	0x01		/* Q_QUOTAON in progress */
#define QTF_CLOSING	0x02		/* Q_QUOTAOFF in progress */

#ifdef	_KERNEL
/*
 * Convert mount ptr to ufsmount ptr.
 */
#define VFSTOUFS(mp)	((struct ufsmount *)((mp)->m_data))

/*
 * Quota locking strategy.
 *
 * Ufsmount quota lock.  Dquot lock.  Dqhead hash chain locks.
 * Dqfree lock.  IQUOTA, IQUOTING, IQUOTWAIT.			XXX
 */
#define	UMPQ_READ_LOCK(ump)	lock_read(&(ump)->um_lock)
#define	UMPQ_READ_UNLOCK(ump)	lock_read_done(&(ump)->um_lock)
#define	UMPQ_WRITE_LOCK(ump)	lock_write(&(ump)->um_lock)
#define	UMPQ_WRITE_UNLOCK(ump)	lock_write_done(&(ump)->um_lock)
#define	UMPQ_LOCK_RECURSIVE(ump) lock_set_recursive(&(ump)->um_lock)
#define	UMPQ_LOCK_UNRECURSIVE(ump) lock_clear_recursive(&(ump)->um_lock)
#define	UMPQ_LOCK_INIT(ump)	lock_init2(&(ump)->um_lock, TRUE, LTYPE_UMPQ)

#define	UMPQ_WRITE_HOLDER(ump)	LOCK_HOLDER(&(ump)->um_lock)
#define	UMPQ_READ_HOLDER(ump)	LOCK_READERS(&(ump)->um_lock)

#define	UMPQ_SYNC_LOCK(ump)	simple_lock(&(ump)->um_qsync_lock)
#define	UMPQ_SYNC_UNLOCK(ump)	simple_unlock(&(ump)->um_qsync_lock)
#define	UMPQ_SYNC_LOCK_INIT(ump) simple_lock_init(&(ump)->um_qsync_lock)

/*
 * mount table
 */
extern struct ufsmount	*mounttab;

/*
 * Unix mount table lock operations.  The only operation
 * on the mounttab requiring synchronization is allocation
 * of entries in the table for mount structures.  Thus, the
 * lock only needs to be held for a brief period of time.
 */
udecl_simple_lock_data(,mounttab_lock);
#define MOUNTTAB_LOCK()		usimple_lock(&mounttab_lock);
#define MOUNTTAB_UNLOCK()	usimple_unlock(&mounttab_lock);
#define MOUNTTAB_LOCK_INIT()	usimple_lock_init(&mounttab_lock);
#endif

#endif /* _UFS_UFSMOUNT_ */
