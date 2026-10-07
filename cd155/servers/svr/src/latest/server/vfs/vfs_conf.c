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
 * $Log: vfs_conf.c,v $
 * Revision 1.6  1994/11/18  20:50:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/19  23:00:23  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.4  1993/07/14  18:46:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:09:01  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:57  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:36  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:57:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:46:19  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:57:20  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/10/22  16:06:22  dbm
 * Added PFS functionality.
 *
 * Revision 2.6  93/07/13  16:13:48  slively
 *      Revision 2.4  93/06/29  16:18:33  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.5  93/06/25  11:27:11  slively
 * Backout the LITE server changes.  Remove the #if UFS.
 * 
 * Revision 2.4  93/06/22  20:09:13  slively
 * Support for LITE server.  #if UFS and #if NFS sections.
 * 
 * Revision 2.3  91/12/16  21:02:45  roy
 * 	91/10/30  17:43:51  bernadat
 * 	Added AFS vfsops.
 *
 * Revision 2.2  91/08/31  14:31:13  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/01  17:03:11  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/07  15:00:30  devrcs
 * 	Added dynamic dependency options
 * 	[90/09/30  15:49:42  knight]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:55:03  gm]
 * 
 * Revision 1.8  90/08/24  12:30:17  devrcs
 * 	Dynamic loading of NFS (The Real Thing).
 * 	[90/08/18  03:53:36  nags]
 * 
 * Revision 1.7  90/07/27  09:09:47  devrcs
 * 	Dynamic filesystem loading -- temporary.
 * 	[90/07/20  17:09:20  nags]
 * 
 * Revision 1.6  90/06/22  20:56:31  devrcs
 * 	Added &s5fs_vfsops back to vfssw[], which was removed for security,
 * 	now conditional with SYSV_FS option.
 * 	[90/06/19  14:56:31  bet]
 * 
 * 	nags merge
 * 	[90/06/12  21:43:23  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:48:14  seiden]
 * 
 * Revision 1.5  90/04/27  19:28:44  devrcs
 * 	 Added support for system V filesystem.
 * 	[90/04/20  14:40:47  morris]
 * 
 * 	[90/04/20  14:34:51  morris]
 * 
 * Revision 1.4  90/03/27  13:29:35  gm
 * 	Changes from George at Encore
 * 	[90/03/15  19:04:25  pam]
 * 
 * Revision 1.2  90/02/13  11:43:24  gmf
 * 	added mfs_vfsops for MFS
 * 
 * Revision 1.3  90/01/02  20:23:25  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:53:16  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
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
 *	%W% (Berkeley) %G%
 */

#include <sys/param.h>
#include <sys/mount.h>
#include <ufs.h>

/*
 * This specifies the filesystem used to mount the root.
 * This specification should be done by /etc/config.
 */
#if     UFS
extern int ufs_mountroot();
int (*mountroot)() = ufs_mountroot;
#else   /* UFS */
int (*mountroot)();
#endif  /* UFS */

/*
 * These define the root filesystem and device.
 */
struct mount *rootfs;
struct vnode *rootdir;

/*
 * Set up the filesystem operations for vnodes.
 * The types are defined in mount.h.
 */
#if     UFS
extern	struct vfsops ufs_vfsops;
#endif  /* UFS */

#include <nfs.h>
#if	NFS && !NFS_DYNAMIC
extern	struct vfsops nfs_vfsops;
#endif

#include <sysv_fs.h>
#if	SYSV_FS && !SYSV_FS_DYNAMIC
extern  struct vfsops s5fs_vfsops;
#endif

#include <mach_afs.h>
#if	MACH_AFS
extern  struct vfsops afs_vfsops;
#endif

#if	PFS && !PFS_DYNAMIC
extern	struct vfsops pfs_vfsops;
#endif

struct vfsops *vfssw[] = {
	(struct vfsops *)0,	/* 0 = MOUNT_NONE */
#if     UFS
	&ufs_vfsops,		/* 1 = MOUNT_UFS */
#else   /* UFS */
        (struct vfsops *)0,
#endif  /* UFS */
#if	NFS && !NFS_DYNAMIC
	&nfs_vfsops,		/* 2 = MOUNT_NFS */
#else
	(struct vfsops *)0,
#endif
	(struct vfsops *)0,	/* 3 = MOUNT_MFS */
	(struct vfsops *)0,	/* 4 = MOUNT_PC */
#if	SYSV_FS && !SYSV_FS_DYNAMIC
        &s5fs_vfsops,           /* 5 = MOUNT_S5FS */
#else
	(struct vfsops *)0,    	/* 5 = MOUNT_S5FS */
#endif
#if	MACH_AFS
	&afs_vfsops,		/* 6 = MOUNT_AFS */
#else
	(struct vfsops *)0,	/* 6 = MOUNT_AFS */
#endif
#if	PFS && !PFS_DYNAMIC
	&pfs_vfsops,		/* 7 = MOUNT_PFS */
#else
	(struct vfsops *)0,
#endif
};
