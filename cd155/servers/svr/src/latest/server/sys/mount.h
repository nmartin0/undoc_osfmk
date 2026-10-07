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
 * This file was modified and extended by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: mount.h,v $
 * Revision 1.9  1994/11/18  20:40:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1993/11/03  19:15:50  cfj
 * Include mach/port.h so user applications including sys/mount.h do not have to also
 * include port.h.
 *
 * Risk: Low
 * Reviewers: shala, nandy, rkl
 * Benefit: User will not have to know to include mach/port.h which is not documented.
 *
 * Revision 1.7  1993/09/10  16:53:21  shala
 * Added prototype for statfs() and fstatfs(). Fixed bug 5392.
 *
 * Revision 1.6  1993/07/14  18:26:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.5  1993/07/09  20:23:10  brad
 * Added #define PFS for compiling applications.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:19:33  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/04/03  03:07:38  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.2.1.2.2  1993/03/11  00:56:06  dbm
 * Added warning message so that the statfs and estatfs structure will
 * hopefully remain consistant.
 *
 * Revision 1.2  1992/11/30  22:42:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:13:27  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/05  22:40:26  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:21  cfj
 * Bump major revision number.
 *
 * Revision 2.11  1992/10/22  15:35:05  dbm
 * Added changes for PFS.
 *
 * Revision 2.10  1992/08/26  12:12:01  loverso
 * Revision 2.11  1992/12/08  10:45:26  durriya
 * 	1.1 unmount sync changes  - Eliminated MOUNT_LOOKUP* locking and replaced
 * 	with UNMOUNT* locks.	The synchronization is mostly with unmount.
 * 	added a couple of M_* f;ags and rearranged value of some of them (durriya)
 *
 * Revision 2.10  92/08/26  12:12:01  loverso
 * 	Add flag M_FASTPATH to the mount command and field m_iomode to the
 * 	mount structure (pjg).
 * 
 * Revision 2.9  92/06/08  18:22:08  pjg
 * 	Define NODE_2_FSID1 and FSID1_2_NODE  macro for OSF1_ADFS.
 * 	Change MAXFIDSIZ to 20 from 16 since file handles are 32 bytes, 
 * 	not 28. (durriya)
 * 
 * Revision 2.8  92/05/24  14:37:50  pjg
 * 	Revision 3.7  92/03/23  18:03:53  condict
 * 	Allow NCPUS == 1 in the server, to compile optimally for a uni-processor.
 * 
 * Revision 2.7  92/04/05  16:59:23  pjg
 * 	Add m_mountid in mount structure. Define macros and variables used in
 * 	setting/generating unique mountid's.                      (durriya)
 * 
 * Revision 2.6  92/03/01  18:38:43  pjg
 * 	92/02/28  16:32:33  noemi
 * 	Added mount structure support.
 * 
 * Revision 2.5  92/01/14  10:47:27  roy
 * 	92/01/07  17:14:58  noemi
 * 	Added mounthead struct and M_GETFSS_LOCK and M_GETFSS_WAIT defines.
 * 
 * Revision 2.4  92/01/05  20:08:03  roy
 * 	1991/09/21  20:15:16  noemi
 * 	Added m_remoteport field to mount structure.  Also added M_REMOTE_DIR
 * 	and M_REMOTE_FS defines.  Moved MOUNT_ALLOCATE and MOUNT_DEALLOCATE
 * 	here from vfs_syscalls.c.
 * 
 * Revision 2.3  91/12/16  18:32:01  roy
 * 	91/10/30  17:43:19  bernadat
 * 	Added AFS mount type.
 * 
 * Revision 2.2  91/08/31  14:08:41  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.5  91/07/31  15:43:52  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/31  14:06:42  devrcs
 * 	Typedef s5fs_args to ufs_args since they have identical fields.
 * 	This allows mountd to be able to treat them identically.
 * 	[90/10/25  14:03:53  morris]
 * 
 * 	Cleaned up a little.  Changed M_FORCE to M_FMOUNT
 * 	to avoid future conflicts with MNT_FORCE.
 * 	Put non-visible flags in proper place.
 * 	[90/10/13  21:41:15  gmf]
 * 
 * Revision 1.11  90/10/07  14:52:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:25  gm]
 * 
 * 	Changed M_VISFLAGMASK so that M_LOCAL and M_QUOTA are visible
 * 	to user-level programs.
 * 	[90/09/28  12:38:53  nags]
 * 
 * Revision 1.10  90/09/23  16:00:01  devrcs
 * 	Added flag for vflush option to skip vnodes being
 * 	used by kernel.
 * 	[90/09/03  22:35:54  nags]
 * 
 * Revision 1.9  90/08/24  12:26:36  devrcs
 * 	Added forcible mount option for dirty filesystems.
 * 	Added quotactl operation to vfsops, quota flags.
 * 	[90/08/19  01:17:47  nags]
 * 
 * Revision 1.8  90/07/27  09:07:24  devrcs
 * 	VFS switch locking, dead mount structures.
 * 	[90/07/20  17:06:36  nags]
 * 
 * Revision 1.7  90/06/22  20:53:26  devrcs
 * 	nags merge
 * 
 * 	Condensed history:
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Secureware changes.					seiden@osf.org
 * 	Added support for system V filesystem.			morris@osf.org
 * 	Added exflags to ufs_args (export filesystems).		gmf@osf.org
 * 	Fixed up struct statfs, added it to struct mount.	gmf@osf.org
 * 	Add NFSMNT_NOCONN.					tmt@osf.org
 * 	Integrated 4.4BSD filesystem changes [1/5/90].		noemi@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Added ifdef KERNEL around a couple of includes.		gm@osf.org
 * 	Parallelized, merged with preliminary Mach 2.5.		alan@encore.com
 * 	[90/06/12  21:38:47  gmf]
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
 *	@(#)mount.h	7.10 (Berkeley) 10/31/89
 */
#ifndef	_SYS_MOUNT_H_
#define _SYS_MOUNT_H_

#ifndef	_KERNEL
#ifndef	PFS
#define	PFS
#endif
#endif

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#include <kern/zalloc.h>
#ifdef	OSF1_ADFS
#include <uxkern/port_hash.h>
#endif
#endif

#include <sys/types.h>
#ifdef	PFS
#include <pfs/pfs.h>
#endif

#include <sys/secdefines.h>
#if	SEC_ARCH
#include <sys/security.h>
#endif 

#include <mach/port.h>


typedef quad fsid_t;			/* file system id type */

/*
 * File identifier.
 * These are unique per filesystem on a single machine.
 */
#define	MAXFIDSZ	20

struct fid {
	u_short		fid_len;		/* length of data in bytes */
	u_short		fid_reserved;		/* force longword alignment */
	char		fid_data[MAXFIDSZ];	/* data (variable length) */
};

/*
 * file system statistics
 */

/*
 * FOR OSF1_ADFS SYSTEMS:  ANY CHANGES TO THIS STRUCTURE MUST BE
 * REFLECTED IN uxkern/fsvr_types.defs.
 */

#define MNAMELEN 90	/* length of buffer for returned name */

/*
 * *** WARNING ***   Must change the estatfs structure (in pfs/pfs.h) 
 *		     to reflect any changes in the statfs structure.
 */

struct statfs {
	short	f_type;			/* type of filesystem (see below) */
	short	f_flags;		/* copy of mount flags */
	long	f_fsize;		/* fundamental filesystem block size */
	long	f_bsize;		/* optimal transfer block size */
	long	f_blocks;		/* total data blocks in file system */
	long	f_bfree;		/* free blocks in fs */
	long	f_bavail;		/* free blocks avail to non-su */
	long	f_files;		/* total file nodes in file system */
	long	f_ffree;		/* free file nodes in fs */
	fsid_t	f_fsid;			/* file system id */
	long	f_spare[9];		/* spare for later */
	char	f_mntonname[MNAMELEN];	/* directory on which mounted */
	char	f_mntfromname[MNAMELEN];/* mounted filesystem */
};

#ifdef OSF1_ADFS
/*
 * This macro is used to set f_fsid[1] for AD. Note that for AD, the rdev field
 * does not uniquely identify the file system, We also need node #, where the
 * physical disk is attached to. Since fsid[1] is set to the mount type which 
 * is only 3 bits right now, we still the upper 27 bits for the node #.
 */
#define NODE_2_FSID1(node,mnttype)      (((node) << 5) | mnttype)
#define FSID1_2_NODE(fsid)              (((fsid).val[1]) >> 5)
#endif


/*
 * Structure per mounted file system.
 * Each mounted file system has an array of
 * operations and an instance record.
 * The file systems are put on a doubly linked list.
 */
struct mount {
	struct mount	*m_next;		/* next in mount list */
	struct mount	*m_prev;		/* prev in mount list */
	struct vfsops	*m_op;			/* operations on fs */
	struct vnode	*m_vnodecovered;	/* vnode we mounted on */
	struct vnode	*m_mounth;		/* list of vnodes this mount */
	int		m_flag;			/* flags */
	uid_t		m_exroot;		/* exported mapping for uid 0 */
	struct statfs	m_stat;			/* cache of fs stats */
	qaddr_t		m_data;			/* private data */
#if	SEC_ARCH
	tag_t		m_tag[SEC_TAG_COUNT];	/* for unlabeled filesystems */
#endif
#ifdef	OSF1_ADFS
	mach_port_t	m_remoteport;		/* remote mount port */
	mach_port_t	m_mountport;		/* mount port for this struct */
        ulong_t         m_mountid;              /* a unique mountid across 
                                                 * multiple nodes of  AD server
                                                 * This set by file-sys dep. 
                                                 * code at mount using the
                                                 * macro SET_MOUNTID
                                                 */
	u_long		m_iomode;		/* I/O mode (see vnode.h) */
#endif
#ifdef	PFS
	struct statpfs	*m_statpfs;		/* cache of stripe attributes */
#endif
#ifdef	_KERNEL
	lock_data_t	m_unmount_lock;		/* pathname/filehandle synch */
	udecl_funnel_data(,m_funnel)		/* serial code compatibility */
	udecl_simple_lock_data(,m_lock)		/* multiprocessor exclusion */
	udecl_simple_lock_data(,m_vlist_lock)	/* protect vnode list */
#endif
};


#ifdef	OSF1_ADFS
/*
 * Head of the mount list.  mounthead is protected by the mountlist lock.
 */
struct mounthead {
	struct mount	*m_next;		/* next in mount list */
	struct mount	*m_prev;		/* prev in mount list */
	u_long		m_syncid;		/* id set by sync */
	u_long		m_getfssid;		/* id set by getfsstat */
	int		m_flag;			/* flags */
};


/*
 * Macros to handle hash table for mount structure ports.   
 */
#define MOUNT_PORT_HASHSZ 	63

#define MOUNT_PORT_ENTER(port, mp)					\
	mpport_hash_enter(mount_port_hash_table, (port), (char *)(mp))

#define MOUNT_PORT_REMOVE(port)						\
	mpport_hash_remove(mount_port_hash_table, (port))

#define MOUNT_PORT_LOOKUP(port, mp)					\
	mp = ((struct mount *)(mpport_hash_lookup(mount_port_hash_table, (port))))

#define	MOUNT_TO_VNODE(mp, vp, error)					\
MACRO_BEGIN								\
	if ((mp) == ((struct mount *)0))				\
		(vp) = ((struct vnode *)0);				\
	else if (((mp)->m_flag) & M_REMOTE_FS) {			\
		ASSERT((mp)->m_vnodecovered != (struct vnode *)0); 	\
		(vp) = (mp)->m_vnodecovered;				\
		VREF((vp));						\
	} else if (((mp)->m_flag) & M_REMOTE_DIR) {			\
		VFS_ROOT((mp), &(vp), (error));				\
		ASSERT(((vp) != (struct vnode *)0) && ((error) == 0));	\
	} else								\
		(vp) = ((struct vnode *)0);				\
MACRO_END
#endif	/* OSF1_ADFS */


/*
 * Mount flags.
 */
#define	M_RDONLY	0x00000001	/* read only filesystem */
#define	M_SYNCHRONOUS	0x00000002	/* file system written synchronously */
#define	M_NOEXEC	0x00000004	/* can't exec from filesystem */
#define	M_NOSUID	0x00000008	/* don't honor setuid bits on fs */
#define	M_NODEV		0x00000010	/* don't interpret special files */
#define	M_FASTPATH	0x00000020	/* fast path filesystem */
/*
 * exported mount flags.
 */
#define	M_EXPORTED	0x00000100	/* file system is exported */
#define	M_EXRDONLY	0x00000200	/* exported read only */
#if	SEC_ARCH
#define	M_SECURE	0x00000800	/* file system is labeled */
#endif
/*
 * Flags set by internal operations.
 */
#define	M_LOCAL		0x00001000	/* filesystem is stored locally */
#define	M_QUOTA		0x00002000	/* quotas are enabled on filesystem */

#ifdef	OSF1_ADFS
#define	M_REMOTE_DIR	0x00004000	/* filesystem mounted on remote dir */
#define	M_REMOTE_FS	0x00008000	/* filesystem is remote */
#endif
/*
 * Mask of flags that are visible to statfs()
 */
#define	M_VISFLAGMASK	0x0000ffff

/*
 * filesystem control flags.
 */
#define	M_UPDATE	0x00010000	/* not a real mount, just an update */
#define	M_MPBUSY	0x00020000	/* sync/unmount in progress */
#define M_FMOUNT	0x00040000	/* forcibly mount, even if not clean */
#define M_MPWANT	0x00080000	/* someone waiting for busy vfs */
#if	MACH
/*
 * MACH swap info
 */
#define	M_SWAP_PREFER	0x00400000	/* prefer to swap here */
#define	M_SWAP_NEVER	0x00800000   	/* never swap here */
#endif

#ifdef OSF1_ADFS
#define M_GETFSS_LOCK	0x01000000	/* serializes getfsstat calls */
#define M_GETFSS_WAIT	0x02000000	/* wait for getfsstat serialization */
#endif

/*
 * Operations supported on mounted file system.
 */
struct vfsops {
	int	(*vfs_mount)(	/* mp, path, data, ndp */ );
	int	(*vfs_start)(	/* mp, flags */ );
	int	(*vfs_unmount)(	/* mp, forcibly */ );
	int	(*vfs_root)(	/* mp, vpp */ );
	int	(*vfs_quotactl)(/* mp, cmd, uid, arg */ );
	int	(*vfs_statfs)(	/* mp */ );
	int	(*vfs_sync)(	/* mp, waitfor */ );
	int	(*vfs_fhtovp)(	/* mp, fidp, vpp */ );
	int	(*vfs_vptofh)(	/* vp, fidp */ );
	int	(*vfs_init)(	/* */ );
};

#ifdef	OSF1_ADFS
#define MOUNT_ALLOCATE(mp)      ZALLOC(mount_zone, mp, struct mount *);
#define MOUNT_DEALLOCATE(mp)    ZFREE(mount_zone, (mp));
#endif

#define	MOUNT_FUNNEL(m)		FUNNEL((m)->m_funnel)
#define	MOUNT_UNFUNNEL(m)	UNFUNNEL((m)->m_funnel)

#define _VFSOP_(f,mp,args,ret)						\
MACRO_BEGIN								\
	MOUNT_FUNNEL(mp);						\
	(ret) = (*(mp)->m_op->f)args;					\
	MOUNT_UNFUNNEL(mp);						\
MACRO_END

#define	_VFSOPV_(f,vp,args,ret)						\
MACRO_BEGIN								\
	MOUNT_FUNNEL((vp)->v_mount);					\
	(ret) = (*((vp)->v_mount->m_op->f))args;			\
	MOUNT_UNFUNNEL((vp)->v_mount);					\
MACRO_END

#define VFS_MOUNT(M,P,D,N,r)		_VFSOP_(vfs_mount,(M),((M),(P),(D),(N)),(r))
#define	VFS_START(MP,FL,r)		_VFSOP_(vfs_start,(MP),((MP),(FL)),(r))
#define	VFS_UNMOUNT(MP,F,r)		_VFSOP_(vfs_unmount,(MP),((MP),(F)),(r))
#define	VFS_ROOT(MP,VPP,r)		_VFSOP_(vfs_root,(MP),((MP),(VPP)),(r))
#define	VFS_QUOTACTL(MP,C,U,A,r)	_VFSOP_(vfs_quotactl,(MP),((MP),(C),(U),(A)),r)
#define	VFS_STATFS(MP,r)		_VFSOP_(vfs_statfs,(MP),(MP),(r))
#define	VFS_SYNC(MP,WF,r)		_VFSOP_(vfs_sync,(MP),((MP),(WF)),(r))
#define	VFS_FHTOVP(MP,FP,VP,r)		_VFSOP_(vfs_fhtovp,(MP),((MP),(FP),(VP)),(r))
#define	VFS_VPTOFH(VP,FIDP,r)		_VFSOPV_(vfs_vptofh,(VP),((VP),(FIDP)),(r))


/*
 * forcibly flags for vfs_umount().
 * waitfor flags to vfs_sync() and getfsstat()
 */
#define MNT_FORCE	0x1
#define MNT_NOFORCE	0x2
#define	MNT_SKIPSYSTEM	0x4
#define MNT_WAIT	0x1
#define MNT_NOWAIT	0x2

/*
 * File system types.
 */
#define	MOUNT_NONE	0
#define	MOUNT_UFS	1
#define	MOUNT_NFS	2
#define MOUNT_MFS	3
#define	MOUNT_PC	4
#define MOUNT_S5FS	5
#define	MOUNT_AFS	6
#ifndef	PFS
#define	MOUNT_MAXTYPE	6
#else
#define	MOUNT_PFS	7
#define	MOUNT_MAXTYPE	7
#endif	PFS

/*
 * Generic file handle
 * FOR OSF1_ADFS SYSTEMS:  ANY CHANGES TO THIS STRUCTURE MUST BE
 * REFLECTED IN uxkern/fsvr_types.defs.
 */
struct fhandle {
	fsid_t	fh_fsid;	/* File system id of mount point */
	struct	fid fh_fid;	/* Id of file */
};
typedef struct fhandle	fhandle_t;

/*
 * Arguments to mount UFS
 */
struct ufs_args {
	char	*fspec;		/* block special device to mount */
	int	exflags;	/* export related flags */
	uid_t	exroot;		/* mapping for root uid */
};

/*
 * Arguments to mount System V file system
 */
typedef struct ufs_args s5fs_args;

#ifdef	PFS
/*
 * Arguments to mount PFS (Parallel File System).
 */
struct pfs_args {
	struct ufs_args	fs_args;	/* passed to underlying physical FS */
	struct statpfs	stripe_attr;	/* stripe attributes of the PFS */
};
#endif	PFS

/*
 * Arguments to mount MFS
 */
struct mfs_args {
	char	*name;		/* name to export for statfs */
	caddr_t	base;		/* base address of file system in memory */
	u_long size;		/* size of file system */
};

/*
 * File Handle (32 bytes for version 2), variable up to 1024 for version 3
 */
union nfsv2fh {
	fhandle_t	fh_generic;
	u_char		fh_bytes[32];
};
typedef union nfsv2fh nfsv2fh_t;

/*
 * Arguments to mount NFS
 */
struct nfs_args {
	struct sockaddr_in	*addr;		/* file server address */
	nfsv2fh_t		*fh;		/* File handle to be mounted */
	int			flags;		/* flags */
	int			wsize;		/* write size in bytes */
	int			rsize;		/* read size in bytes */
	int			timeo;		/* initial timeout in .1 secs */
	int			retrans;	/* times to retry send */
	char			*hostname;	/* server's name */
};
/*
 * NFS mount option flags
 */
#define	NFSMNT_SOFT	0x001	/* soft mount (hard is default) */
#define	NFSMNT_WSIZE	0x002	/* set write size */
#define	NFSMNT_RSIZE	0x004	/* set read size */
#define	NFSMNT_TIMEO	0x008	/* set initial timeout */
#define	NFSMNT_RETRANS	0x010	/* set number of request retrys */
#define	NFSMNT_HOSTNAME	0x020	/* set hostname for error printf */
#define	NFSMNT_INT	0x040	/* allow interrupts on hard mount */
#define	NFSMNT_NOCONN	0x080	/* no connect on mount - any responder */

#ifdef	_KERNEL
/*
 * Mount structure locking constraints.
 *	Field		Constraint
 *	-----		----------
 *	m_next		mountlist_lock
 *	m_prev		mountlist_lock
 *	m_op		m_lock
 *	m_vnodecovered	read-only?	XXX
 *	m_mounth	m_vlist_lock
 *	m_flag		m_lock
 *	m_exroot	m_lock
 *	m_stat		m_lock
 *	m_data		read-only
 *
 * Additional mount synchronization constraints.  [XXX]
 */

/*
 * The mount lock protects the contents of the mount structure from
 * other processors.  Only needed for NCPUS > 1.
 */
#define	MOUNT_LOCK(mp)		usimple_lock(&(mp)->m_lock)
#define	MOUNT_UNLOCK(mp)	usimple_unlock(&(mp)->m_lock)
#define	MOUNT_LOCK_INIT(mp)	usimple_lock_init(&(mp)->m_lock)
#define	MOUNT_LOCK_HOLDER(mp)	SLOCK_HOLDER(&(mp)->m_lock)

/*
 * The mount vnode list lock guards the list of vnodes associated
 * with a mount point.  Only needed for NCPUS > 1.
 */
#define	MOUNT_VLIST_LOCK(mp)	usimple_lock(&(mp)->m_vlist_lock)
#define	MOUNT_VLIST_UNLOCK(mp)	usimple_unlock(&(mp)->m_vlist_lock)
#define	MOUNT_VLIST_LOCK_INIT(mp) usimple_lock_init(&(mp)->m_vlist_lock)

/*
 * The unmount lock synchronizes pathname translation (crossing
 * of mount points) with filesystem unmounting.  It is acquired for
 * reading by namei() and getvfs(), and acquired for writing by
 * unmount().
 * NOTE: these could be optimized for the root FS (to do nothing)
 *	 since the root cannot be unmounted.
 */
#define	UNMOUNT_TRY_READ(mp)      lock_try_read_assert(&(mp)->m_unmount_lock)
#define	UNMOUNT_TRY_READ_NOSLP(mp) lock_try_read(&(mp)->m_unmount_lock)
#define	UNMOUNT_WRITE_LOCK(mp)	  lock_write(&(mp)->m_unmount_lock)
#define	UNMOUNT_WRITE_UNLOCK(mp)  lock_write_done(&(mp)->m_unmount_lock)
#define	UNMOUNT_READ_UNLOCK(mp)	  lock_read_done(&(mp)->m_unmount_lock)
#define	UNMOUNT_LOCK_INIT(mp)	  lock_init2(&(mp)->m_unmount_lock, TRUE, \
					LTYPE_UNMOUNT)

/*
 * Mount list lock operations.
 */
#define	MOUNTLIST_LOCK()		usimple_lock(&mountlist_lock)
#define	MOUNTLIST_UNLOCK()		usimple_unlock(&mountlist_lock)
#define	MOUNTLIST_LOCK_INIT()		usimple_lock_init(&mountlist_lock)

/*
 * Lock operations on the vfssw.  Used for addition/deletion of
 * file systems.
 */
#define VFSSW_READ_LOCK()	lock_read(&vfssw_lock)
#define VFSSW_WRITE_LOCK()	lock_write(&vfssw_lock)
#define VFSSW_WRITE_UNLOCK()	lock_done(&vfssw_lock)
#define VFSSW_READ_UNLOCK()	lock_done(&vfssw_lock)
#define VFSSW_LOCK_INIT()	lock_init2(&vfssw_lock, TRUE, LTYPE_VFSSW)

#define NULLMOUNT	((struct mount *) 0)
#define DEADMOUNT	(&dead_mount)

/*
 * exported vnode operations and globals
 */
extern void	vfs_remove();		/* remove a vfs from mount list */
extern struct	mount *getvfs();	/* return vfs given fsid */
extern int	vfs_busy();     	/* mark filesystem busy */
extern void	vfs_unbusy();	        /* mark filesystem not busy */
extern struct	mount *rootfs;		/* ptr to root mount structure */
extern struct	mount dead_mount; 	/* dead mount structure */
extern struct	vfsops *vfssw[];	/* mount filesystem type switch */
extern lock_data_t vfssw_lock;		/* guard vfssw */
udecl_simple_lock_data(extern,mountlist_lock)	/* guard mount list */

#if	MACH
extern zone_t	mount_zone;		/* dynamically allocated mounts */
#endif
extern int	nmount;			/* number of ufs mount structs */
extern int	nmount_max;		/* number of entries in mount_zone */

#ifdef	OSF1_ADFS
extern struct mounthead 	mounth;
extern mpport_hash_table_t	mount_port_hash_table;
#endif

#ifdef OSF1_ADFS
u_short  mount_count;                   /* count used to generate a unique 
                                         * mountid 
                                         */
udecl_simple_lock_data(,mountcount_lock)	/* guard mount_count */
extern node_t this_node;

#define	MOUNTCOUNT_LOCK_INIT()		usimple_lock_init(&mountcount_lock)

#define SET_MOUNTID(m)  usimple_lock(&mountcount_lock); \
                        (m)->m_mountid = ((this_node << 16) | (++mount_count));\
                        usimple_unlock(&mountcount_lock);

#endif  /* OSF1_ADFS */
					/* XXX should be combined XXX */
#endif	/* _KERNEL */

#ifndef _KERNEL
#ifdef _NO_PROTO
        extern int      statfs();
        extern int      fstatfs();
#else                           /* use POSIX required prototypes */
        extern int      statfs(char *, struct statfs *);
        extern int      fstatfs(int , struct statfs *);
#endif /* _NO_PROTO */
#endif /* _KERNEL */

#endif	/* _SYS_MOUNT_H_ */
