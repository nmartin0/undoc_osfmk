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
 * $Log: mfs_vnops.c,v $
 * Revision 1.7  1994/11/18  20:45:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/28  23:08:56  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.5  1994/01/14  01:18:45  jlitvin
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
 * Revision 1.4  1993/07/14  18:37:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:30:29  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:33  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1992/10/22  15:42:04  dbm
 * Updated for PFS functionality.
 *
 * Revision 2.3  93/10/20  15:30:50  dnoveck
 *      DEV_BSIZE elimination: Change use of DEB_BSIZE-based defines
 *      to their DISK_GRANULE-based corelates.
 *
 * Revision 2.2  1991/08/31  14:18:33  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/01  17:00:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:58:20  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:45  gm]
 * 
 * Revision 1.6  90/09/23  16:00:39  devrcs
 * 	Fixed mfs unmounts and clean up a little (bug #930).
 * 	[90/09/06  17:40:28  gmf]
 * 
 * Revision 1.5  90/07/27  09:08:51  devrcs
 * 	VOP_OPEN changes for clone driver support, dead mounts.
 * 	[90/07/20  17:07:29  nags]
 * 
 * Revision 1.4  90/06/29  13:54:19  devrcs
 * 	Parallelized and multithreaded.
 * 	[90/06/26  11:30:36  gmf]
 * 
 * Revision 1.3  90/06/22  20:55:13  devrcs
 * 	nags merge
 * 	[90/06/12  21:40:55  gmf]
 * 
 * Revision 2.1  90/03/19  11:39:47  gmf
 * 	Relevant condensed history:
 * 	parallelized						gmf@osf.org
 * 	submitted to osc.8					gmf@osf.org
 * 	fixes to compile and run in osf/1			gmf@osf.org
 * 	Integrated with 4.4BSD sources from 11/30/89		gmf@osf.org
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
 *	@(#)mfs_vnops.c	7.10 (Berkeley) 11/30/89
 */

#include <sys/param.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/user.h>
#include <sys/buf.h>
#ifndef	OSF1_SERVER
#include <sys/vmmac.h>
#endif	/* OSF1_SERVER */
#include <sys/errno.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <ufs/mfsnode.h>
#ifndef	OSF1_SERVER
#include <kern/thread.h>
#endif	/* OSF1_SERVER */
#include <kern/sched_prim.h>

/*
 * mfs vnode operations.
 */
int	mfs_open(),
	mfs_strategy(),
	mfs_bmap(),
	mfs_ioctl(),
	mfs_close(),
	mfs_inactive(),
	mfs_print(),
	mfs_badop(),
	mfs_nullop();

/*
 * use spec_reclaim to clean up aliases
 */
extern int	spec_reclaim();

struct vnodeops mfs_vnodeops = {
	mfs_badop,		/* lookup */
	mfs_badop,		/* create */
	mfs_badop,		/* mknod */
	mfs_open,		/* open */
	mfs_close,		/* close */
	mfs_badop,		/* access */
	mfs_badop,		/* getattr */
	mfs_badop,		/* setattr */
	mfs_badop,		/* read */
	mfs_badop,		/* write */
	mfs_ioctl,		/* ioctl */
	mfs_badop,		/* select */
	mfs_badop,		/* mmap */
	mfs_badop,		/* fsync */
	mfs_badop,		/* seek */
	mfs_badop,		/* remove */
	mfs_badop,		/* link */
	mfs_badop,		/* rename */
	mfs_badop,		/* mkdir */
	mfs_badop,		/* rmdir */
	mfs_badop,		/* symlink */
	mfs_badop,		/* readdir */
	mfs_badop,		/* readlink */
	mfs_badop,		/* abortop */
	mfs_inactive,		/* inactive */
	spec_reclaim,		/* reclaim */
	mfs_bmap,		/* bmap */
	mfs_strategy,		/* strategy */
	mfs_print,		/* print */
	mfs_badop,		/* page read */
	mfs_badop,		/* page write */
#ifdef	PFS
	mfs_badop,		/* preallocate and set size */
#endif

};

/*
 * Vnode Operations.
 *
 * Open called to allow memory filesystem to initialize and
 * validate before actual IO. Record our process identifier
 * so we can tell when we are doing I/O to ourself.
 */
/* ARGSUSED */
mfs_open(vpp, mode, cred)
	register struct vnode **vpp;
	int mode;
	struct ucred *cred;
{
	int error;
	register struct vnode *vp = *vpp;

	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK) {
		BM(VN_UNLOCK(vp));
		panic("mfs_open not VBLK");
		/* NOTREACHED */
	}
	BM(VN_UNLOCK(vp));
	error = makealias(vp);
	return(error);
}

/*
 * Ioctl operation.
 */
/* ARGSUSED */
mfs_ioctl(vp, com, data, fflag, cred)
	struct vnode *vp;
	int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
{

	return (-1);
}

/*
 * Pass I/O requests to the memory filesystem process.
 */
mfs_strategy(bp)
	register struct buf *bp;
{
	register struct mfsnode *mfsp;
	struct vnode *vp;

	if (vfinddev(bp->b_dev, VBLK, &vp))
		panic("mfs_strategy: bad dev");
	/*
	 * No locking here; these fields are read-only.
	 */
	mfsp = VTOMFS(vp);
	if (mfsp->mfs_pid == u.u_procp->p_pid) {
		mfs_doio(bp, mfsp->mfs_baseoff);
	} else {
		VN_LOCK(vp);
		bp->av_forw = mfsp->mfs_buflist;
		mfsp->mfs_buflist = bp;
		VN_UNLOCK(vp);
		thread_wakeup_one((int)vp);
	}
	return (0);
}

/*
 * Memory file system I/O.
 *
 * Essentially play ubasetup() and disk interrupt service routine by
 * doing the copies to or from the memfs process. If doing physio
 * (i.e. pagein), we must map the I/O through the kernel virtual
 * address space.
 */
mfs_doio(bp, base)
	register struct buf *bp;
	caddr_t base;
{
	caddr_t kernaddr, offset;

	/*
	 * For phys I/O, map the b_addr into kernel virtual space
	 */
	if ((bp->b_flags & B_PHYS) == 0) {
		kernaddr = bp->b_un.b_addr;
	} else {
		panic("mfs_doio: B_PHYS\n");
	}
	offset = base + dgtob(bp->b_blkno);
	if (bp->b_flags & B_READ)
		bp->b_error = copyin(offset, kernaddr, bp->b_bcount);
	else
		bp->b_error = copyout(kernaddr, offset, bp->b_bcount);
	if (bp->b_error)
		bp->b_flags |= B_ERROR;
	biodone(bp);
}

/*
 * This is a noop, simply returning what one has been given.
 */
mfs_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{

	if (vpp != NULL)
		*vpp = vp;
	if (bnp != NULL)
		*bnp = bn;
	return (0);
}

/*
 * Memory filesystem close routine
 */
/* ARGSUSED */
mfs_close(vp, flag, cred)
	register struct vnode *vp;
	int flag;
	struct ucred *cred;
{
	register struct mfsnode *mfsp = VTOMFS(vp);
	register struct buf *bp;
	struct specalias *sa;

	/*
	 * Finish any pending I/O requests.
	 */
	VN_LOCK(vp);
	while (bp = mfsp->mfs_buflist) {
		mfsp->mfs_buflist = bp->av_forw;
		VN_UNLOCK(vp);
		mfs_doio(bp, mfsp->mfs_baseoff);
		VN_LOCK(vp);
	}
	VN_UNLOCK(vp);
	/*
	 * On last close of a memory filesystem
	 * we must invalidate any in core blocks, so that
	 * we can, free up its vnode.
	 */
	vflushbuf(vp, 0);
	if (vinvalbuf(vp, 1))
		return (0);
	/*
	 * There should be no way to have any more uses of this
	 * vnode, so if we find any other uses, it is a panic.
	 */
	BM(VN_LOCK(vp));
	if (vp->v_usecount > 1 || mfsp->mfs_buflist) {
		vprint("mfs_close", vp);
		panic("mfs_close");
	}
	BM(VN_UNLOCK(vp));
	/*
	 * Send a request to the filesystem server to exit.
	 */
	thread_wakeup_with_result((caddr_t)vp, THREAD_INTERRUPTED);
	/*
	 * Wake up initial thread, so it can exit.
	 */
	thread_wakeup((caddr_t)mfsp);
	/*
	 * Must decrement usecount on specalias structure.  Cannot go
	 * through normal mechanism (spec_close) since it assumes a
	 * normal device.  All we need to do is decrement the usecount.
	 * No locking required -- nobody knows about these vnodes.
	 */
	sa = vp->v_specinfo->si_alias;
	ASSERT((sa != (struct specalias *) 0) && 
	       (sa->sa_usecount == 1));
	sa->sa_usecount--;
	return (0);
}

/*
 * Memory filesystem inactive routine
 */
/* ARGSUSED */
mfs_inactive(vp)
	struct vnode *vp;
{

	return (0);
}

/*
 * Print out the contents of an mfsnode.
 */
mfs_print(vp)
	struct vnode *vp;
{
	register struct mfsnode *mfsp = VTOMFS(vp);

	printf("tag VT_MFS, pid %d, base %d, size %d\n", mfsp->mfs_pid,
		mfsp->mfs_baseoff, mfsp->mfs_size);
}

/*
 * Block device bad operation
 */
mfs_badop()
{

	panic("mfs_badop called\n");
	/* NOTREACHED */
}

/*
 * Block device null operation
 */
mfs_nullop()
{

	return (0);
}
