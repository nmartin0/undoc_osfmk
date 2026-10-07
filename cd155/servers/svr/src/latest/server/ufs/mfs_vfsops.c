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
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: mfs_vfsops.c,v $
 * Revision 1.6  1994/11/18  20:45:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/14  01:18:47  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	ufs/ufs_vnops.c, ufs/ufs_vfsops.c, ufs/ufs_lookup.c
 * 	ufs/ufs_inode.c, ufs/ufs_cache.c, ufs/ufs_alloc.c
 * 	ufs/mfs_vnops.c, ufs/mfs_vfsops.c
 *
 * Revision 1.4  1993/07/14  18:37:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:27  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:01  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.4  1993/01/06  10:38:08  loverso
 * 	Fix notice.
 *
 * Revision 1.2  1992/11/30  22:50:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:30  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/01/05  19:27:09  roy
 * 	1991/11/12  19:38:22  noemi
 * 	Changed parameters in call to bdevvp.
 * 
 * Revision 2.2  91/08/31  14:18:26  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  16:59:53  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/31  14:07:33  devrcs
 * 	limit number of threads to one
 * 	[90/10/24  16:06:10  gmf]
 * 
 * 	Changes to mount update to refresh filesystem information.
 * 	Changed call to mountfs.
 * 	[90/10/03  09:26:38  gmf]
 * 
 * Revision 1.8  90/10/07  14:58:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:38  gm]
 * 
 * Revision 1.7  90/09/23  16:00:36  devrcs
 * 	Fix mfs unmounts and do a little cleanup (bug #930).
 * 	[90/09/06  17:39:56  gmf]
 * 
 * Revision 1.6  90/08/24  12:28:26  devrcs
 * 	Add entry to mfs_vfsops for quotactl.
 * 	[90/08/19  01:24:39  nags]
 * 
 * Revision 1.5  90/07/27  09:08:48  devrcs
 * 	Minor cleanup.
 * 	[90/07/20  17:07:19  nags]
 * 
 * Revision 1.4  90/06/29  13:54:16  devrcs
 * 	Parallelized and multithreaded.
 * 	[90/06/26  11:30:18  gmf]
 * 
 * 	nags merge
 * 	[90/06/12  21:40:47  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/10  02:08:52  seiden]
 * 
 * Revision 1.3  90/06/22  20:55:10  devrcs
 * 	Attach root vnode to a mount structure.
 * 	[90/06/18  17:11:11  gmf]
 * 
 * Revision 2.1  90/03/19  11:39:44  gmf
 * 	Relevant condensed history:
 * 	parallelized						gmf@osf.org
 * 	submitted to osc.8					gmf@osf.org
 * 	get rid of um_mntname
 * 	Changes to compile in osf/1				gmf@osf.org
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
 *	@(#)mfs_vfsops.c	7.8 (Berkeley) 11/25/89
 */

#include <sys/secdefines.h>

#include <sys/param.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/buf.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <ufs/ufsmount.h>
#include <ufs/mfsnode.h>
#include <ufs/fs.h>

#ifndef	OSF1_SERVER
#include <kern/thread.h>
#include <kern/sched_prim.h>
#endif	/* OSF1_SERVER */

extern struct vnodeops mfs_vnodeops;

/*
 * mfs vfs operations.
 */
int mfs_mount();
int mfs_start();
int ufs_unmount();
int ufs_root();
int ufs_quotactl();
int mfs_statfs();
int ufs_sync();
int ufs_fhtovp();
int ufs_vptofh();
int mfs_nullop();

struct vfsops mfs_vfsops = {
	mfs_mount,
	mfs_start,
	ufs_unmount,
	ufs_root,
	ufs_quotactl,
	mfs_statfs,
	ufs_sync,
	ufs_fhtovp,
	ufs_vptofh,
	mfs_nullop,
};


/*
 * VFS Operations.
 *
 * mount system call
 */
/* ARGSUSED */
mfs_mount(mp, path, data, ndp)
	struct mount *mp;
	char *path;
	caddr_t data;
	struct nameidata *ndp;
{
	struct vnode *devvp;
	struct mfs_args args;
	struct ufsmount *ump;
	register struct fs *fs;
	register struct mfsnode *mfsp;
	static int mfs_minor;
	u_int size;
	int error;

	MOUNT_LOCK(mp);
	if (mp->m_flag & M_UPDATE) {
		ump = VFSTOUFS(mp);
		fs = ump->um_fs;
		if (fs->fs_ronly && (mp->m_flag & M_RDONLY) == 0)
			fs->fs_ronly = 0;
		MOUNT_UNLOCK(mp);
		return (0);
	}
#if	SER_COMPAT
	mp->m_funnel = FUNNEL_NULL;
#endif
	mp->m_flag |= M_SWAP_NEVER;	/* cannot page to memory!!! */
	MOUNT_UNLOCK(mp);
	if (error = copyin(data, (caddr_t)&args, sizeof (struct mfs_args)))
		return (error);
#ifdef	OSF1_ADFS
	error = bdevvp(makedev(255, mfs_minor++), -1, VBLK, &devvp);
#else
	error = bdevvp(makedev(255, mfs_minor++), &devvp);
#endif
	if (error)
		return (error);
	devvp->v_tag = VT_MFS;
	devvp->v_op = &mfs_vnodeops;
	mfsp = VTOMFS(devvp);
	mfsp->mfs_baseoff = args.base;
	mfsp->mfs_size = args.size;
	mfsp->mfs_vnode = devvp;
	mfsp->mfs_pid = u.u_procp->p_pid;
	mfsp->mfs_task = current_task();
	mfsp->mfs_buflist = (struct buf *)0;
	if (error = mountfs(devvp, mp, 0)) {
		if (error == EBUSY)	/* cannot happen */
			panic("mfs_mount: duplicate mount");
		vrele(devvp);
		return (error);
	}
#if	SEC_FSCHANGE
	if (error = sec_ufsmountcheck(mp)) {
		(void) ufs_unmount(mp, 0);
		return error;
	}
#endif
	ump = VFSTOUFS(mp);
	fs = ump->um_fs;
	(void) copyinstr(path, fs->fs_fsmnt, sizeof(fs->fs_fsmnt) - 1, &size);
	bzero(fs->fs_fsmnt + size, sizeof(fs->fs_fsmnt) - size);
	bcopy((caddr_t)fs->fs_fsmnt, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
	(void) copyinstr(args.name, mp->m_stat.f_mntfromname, MNAMELEN - 1, 
		&size);
	bzero(mp->m_stat.f_mntfromname + size, MNAMELEN - size);
	/*
	 * prime the struct statfs in the mount point
	 */
	(void) mfs_statfs(mp);
	return (0);
}

/*
 * Used to grab the process and keep it in the kernel to service
 * memory filesystem I/O requests.
 * Create a number of threads and start them off.
 */
mfs_start(mp, flags)
	struct mount *mp;
	int flags;
{
#ifndef	OSF1_SERVER
	register struct vnode 	*vp = VFSTOUFS(mp)->um_devvp;
	register struct mfsnode *mfsp = VTOMFS(vp);
	int 			i;
	thread_t		thread;
	int 			nmfsthreads;
	extern int 		numcpus;
	void			mfs_thread_start();
#endif	/* OSF1_SERVER */

#ifdef	OSF1_SERVER
	printf("mfs_start() NOT YET IMPLEMENTED.\n");
	Debugger();
#else	/* OSF1_SERVER */
	/*
	 * Due to a problem in vm_map_pageable, we get a deadlock
	 * on the mmax when multiple threads are used, soooo we
	 * limit it to one for now.  Should be:
	 * 	nmfsthreads = numcpus + 1;
	 */
	nmfsthreads = 1;

	ASSERT(mfsp->mfs_task == current_task());
	/*
	 * Start nmfsthreads plus one (this one).
	 * A uniprocessor gets 2.
	 * Should we do boost priority?
	 */
	for (i = 0; i < nmfsthreads; i++) {
		if (thread_create(current_task(), &thread) ==
			KERN_SUCCESS) {
			thread_start(thread, mfs_thread_start, THREAD_USERMODE);
			(void) thread_resume(thread);
			mfsp->mfs_numthreads++;
		}
	}
	/*
	 * Wait until an unmount or signal happens, at which time, we
	 * see if it was an unmount or signal that caused us to wake up.
	 * If an unmount, then simply exit; if a signal, then attempt to
	 * unmount the file system, and then exit.
	 */
	thread = current_thread();
	assert_wait((int)mfsp, TRUE);
	thread_block();
	/*
	 * If we were signalled, then we need to try to do the unmount
	 * to clean up; otherwise, we were woken up via mfs_close, 
	 * which was called from unmount.
	 */
	if (thread->wait_result != THREAD_AWAKENED)
		(void) dounmount((struct vnode *) 0, mp, MNT_NOFORCE);
	return (0);
#endif	/* OSF1_SERVER */
}

/*
 * mfs_thread_start:  the one that really does the work.
 *
 * Loop servicing I/O requests.
 * Copy the requested data into or out of the memory filesystem
 * address space.
 */
/* ARGSUSED */
void
mfs_thread_start()
{
#ifndef	OSF1_SERVER
	register struct vnode *vp;
	register struct mfsnode *mfsp;
	register struct mount *mp;
	register struct buf *bp;
	register caddr_t base;
	thread_t	th;
#endif	/* OSF1_SERVER */

#ifdef	OSF1_SERVER
	printf("mfs_thread_start() NOT YET IMPLEMENTED.\n");
	Debugger();
#else	/* OSF1_SERVER */
	/*
	 * Find our mount point, and proceed from there.
	 */
	for (mp = rootfs->m_next; mp != rootfs; mp = mp->m_next) {
		if (mp->m_stat.f_type == MOUNT_MFS) {
			vp = VFSTOUFS(mp)->um_devvp;
			mfsp = VTOMFS(vp);
			if (mfsp->mfs_task == current_task())
				break;
		}
	}
	if (mp == rootfs) {
		return;
	}
	base = mfsp->mfs_baseoff;
	th = current_thread();
	assert_wait((int)vp, TRUE);
	thread_block();
	VN_LOCK(vp);
	while (th->wait_result == THREAD_AWAKENED) {
		while (bp = mfsp->mfs_buflist) {
			mfsp->mfs_buflist = bp->av_forw;
			VN_UNLOCK(vp);
			mfs_doio(bp, base);
			VN_LOCK(vp);
		}
		assert_wait((int)vp, TRUE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}
	VN_UNLOCK(vp);
	/*
	 * We've been interrupted or unmounted; cleanup happens in
	 * mfs_start, so we terminate.
	 */
	thread_terminate(th);
	thread_halt_self();
#endif	/* OSF1_SERVER */
}

/*
 * Get file system statistics.
 */
mfs_statfs(mp)
	struct mount *mp;
{
	int error;

	error = ufs_statfs(mp);
	MOUNT_LOCK(mp);
	mp->m_stat.f_type = MOUNT_MFS;
	MOUNT_UNLOCK(mp);
	return (error);
}
