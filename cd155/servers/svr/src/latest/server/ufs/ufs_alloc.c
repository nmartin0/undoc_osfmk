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
 * $Log: ufs_alloc.c,v $
 * Revision 1.13  1994/11/18  20:45:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/06/28  23:09:48  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.11  1994/03/25  18:32:29  brad
 * Merged revision 1.8.4.2 from the R1.2 branch.
 *
 * Revision 1.8.4.2  1994/03/25  18:26:01  brad
 * Fixed improper calculation of read size if fragment being reallocated
 * at the end of a disk partition in realloccg_nbc().  Possible cause of
 * vio_device_read_synchronous panics.  Also changed
 * vio_device_read_synchronous panic in vfs_vio.c to a less severe warning,
 * and fixed kernel/driver error->errno mapping.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 8426
 *  Testing: Ran fileio/PFS EATs
 *  Module(s): server/ufs/ufs_alloc.c server/vfs/vfs_vio.c
 *
 * Revision 1.10  1994/02/17  17:00:54  brad
 * Merged revision 1.8.4.1 from the R1.2 branch.
 *
 * Revision 1.8.4.1  1994/02/16  04:19:45  brad
 * Fixed flawed implementation of disk block preallocation.  Only preallocate
 * full file system blocks for simplicity.  Handle i_resfrags field in
 * the inode correctly.  Several errors in ufs_prealloc() fixed.
 *
 *  Reviewer: Bob Godley
 *  Risk: Med
 *  Benefit or PTS #: 6318
 *  Testing: Ran PTS test.  Ran ORNL climate modelling code from bug #7266
 *     and verified lsize working now.  Ran PFS EATs and fileio EATs on
 *     64 nodes.  unmounted and force-ran fsck many times to ensure file
 *     systems clean.
 *  Module(s): server/ufs/{ufs_alloc,ufs_bmap,ufs_inode,ufs_vnops}.c
 *             server/sys/buf.h
 *
 * Revision 1.9  1994/01/14  01:18:41  jlitvin
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
 * Revision 1.8  1993/07/14  18:37:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/06/28  23:34:23  wunder
 * Added check to allow truncation of preallocated disk blocks in blkfree().
 *
 * Revision 1.6  1993/05/27  02:00:07  brad
 * Removed temporary code that allowed PFS files to be cached in the buffer
 * cache ... PFS now uses Fast Path exclusively.
 *
 * Revision 1.5  1993/05/06  20:30:34  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  94/02/03  11:01:41  dnoveck
 *	Major revision of realloccg for per-node bufefr-cache block
 *	size since the mapping between blocks and buffers is no
 *	longer one-to-one.
 *
 * Revision 2.13  93/10/20  15:30:54  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE-based defines
 *      to their DISK_GRANULE-based corelates.  Change interface to
 *      {vio,data}_{read,write} to be in terms of disk granules.
 *
 *
 * Revision 2.12  93/05/13  16:46:44  roy
 * 	Fix assert in realloccg_nbc to allow VIO_IS_PAGING files.
 * 	[93/05/11            roy]
 * 
 * Revision 2.11  93/03/30  16:10:02  roy
 * 	Added VIO_IS_FASTPATH support to realloccg_nbc.
 * 	[93/03/10            roy]
 *
 * Revision 1.4  1993/04/03  03:10:29  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.3  1992/12/11  03:03:44  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.5  1993/02/12  22:39:07  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.1.2.4  1993/02/09  21:43:14  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.1.2.1.2.3  1992/12/16  06:03:52  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.2  1992/12/14  23:22:31  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.2  1992/11/30  22:50:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/18  20:13:05  brad
 * OSF fix in ialloccg() for the problem that caused buffer cache buffers to
 * remain locked, causing the system to hang when all free buffers are 
 * depleted.  (This problem was discovered with various 'tar xvf' hangs, and 
 * hangs installing NQS, both of which were causing many directories to be 
 * created.)
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:14:22  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/05  23:39:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.10  1992/10/22  15:42:07  dbm
 * Updated for PFS functionality.
 *
 * Revision 2.10  92/11/17  19:52:39  loverso
 * 	do not mark buffers as B_LOCKED in ialloccg. The locked list is 
 * 	a black hole                                          - durriya
 * 
 * Revision 2.9  92/09/24  16:50:30  rabii
 * 	Remove bad assert related to iomode in ialloc().
 * 	[92/09/23            roy]
 * 
 * Revision 2.8  92/08/26  12:12:13  loverso
 * 	Modify realloccg_nbc and blkpref to handle reserved blocks.
 * 	Removed MAY_USE_BUFCACHE in favor of VIO_IS_BUF.
 * 	[92/07/22            roy]
 * 
 * Revision 2.7  92/07/14  14:53:27  rabii
 * 	Modified calling sequence to data_read and data_write.
 * 	[92/07/10            roy]
 * 
 * Revision 2.6  92/03/15  14:40:48  roy
 * 	92/03/03  16:58:15  roy
 * 	Changes for MAPPED_FILES (don't call inode_uncache).
 * 
 * Revision 2.5  92/03/09  12:49:00  durriya
 * 	Revision 3.7  91/12/18  17:18:10  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.4  91/12/17  09:11:56  roy
 * 	91/11/26  15:33:58  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/10/23  16:38:29  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.3  91/12/10  21:32:41  roy
 * 	91/10/14  20:57:52  roy
 * 	Added realloc_nbc code.
 * 
 * Revision 2.2  91/08/31  14:18:57  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:00:17  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5.5.2  91/09/13  10:04:34  sue
 *	Fixed bugs 2683, 2671, 2091.
 *	[91/09/13  10:02:31  sue]
 *
 * Revision 1.11  90/10/31  14:07:37  devrcs
 * 	Changes to mount update to refresh filesystem information.
 * 	Changed calls to iget().
 * 	[90/10/03  09:27:21  gmf]
 * 
 * Revision 1.10  90/10/07  14:58:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:52:36  gm]
 * 
 * Revision 1.9  90/09/23  16:00:45  devrcs
 * 	Credit user's quota when disk block allocation
 * 	fails (unlike original 4.3BSD-Reno).  Eliminate
 * 	unnecessary use of FS_LOCK.
 * 	[90/09/03  22:36:47  nags]
 * 
 * Revision 1.8  90/08/24  12:28:44  devrcs
 * 	Replaced old, useless quota code with (unparallelized)
 * 	4.3-Reno quota code.
 * 	[90/08/19  01:26:42  nags]
 * 
 * Revision 1.7  90/07/27  09:08:56  devrcs
 * 	Remove unwanted ifree assertion.
 * 	[90/07/17  08:50:19  nags]
 * 
 * Revision 1.6  90/07/17  11:43:07  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  22:04:01  seiden]
 * 
 * Revision 1.5  90/06/22  20:55:29  devrcs
 * 	Added an assertion in ialloc on vnode ops.
 * 	[90/06/18  17:11:50  nags]
 * 
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:57:13  seiden]
 * 
 * 	nags merge
 * 	[90/06/12  21:41:15  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:47:32  seiden]
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Integrated various pieces:			gmf@osf.org
 * 		4.4BSD, Mach X115, Encore fast symlinks,
 * 		Encore parallelization from Encore Mach/0.6.
 * 	[90/06/10  02:09:25  seiden]
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
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)ufs_alloc.c	7.15 (Berkeley) 12/30/89
 */
#if	MACH
#include <quota.h>
#endif
#include <mapped_files.h>
#include <ufs_nbc.h>

#include <sys/secdefines.h>
#if	SEC_BASE
#include <sys/security.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/kernel.h>
#include <sys/syslog.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#if	QUOTA
#include <ufs/quota.h>
#endif
#include <ufs/inode.h>
#include <ufs/fs.h>
#if	MACH
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/mfs.h>
#endif

extern u_long		hashalloc();
extern ino_t		ialloccg();
extern daddr_t		alloccg();
extern daddr_t		alloccgblk();
extern daddr_t		fragextend();
extern daddr_t		blkpref();
extern daddr_t		mapsearch();
extern int		inside[], around[];
extern unsigned char	*fragtbl[];

#if	MACH_ASSERT
extern struct vnodeops ufs_vnodeops;
#ifdef	PFS
extern struct vnodeops pfs_vnodeops;
#endif	PFS
#endif

/*
 * Allocate a block in the file system.
 *
 * The size of the requested block is given, which must be some
 * multiple of fs_fsize and <= fs_bsize.
 * A preference may be optionally specified. If a preference is given
 * the following hierarchy is used to allocate a block:
 *   1) allocate the requested block.
 *   2) allocate a rotationally optimal block in the same cylinder.
 *   3) allocate a block in the same cylinder group.
 *   4) quadradically rehash into other cylinder groups, until an
 *      available block is located.
 * If no block preference is given the following heirarchy is used
 * to allocate a block:
 *   1) allocate a block in the cylinder group that contains the
 *      inode for the file.
 *   2) quadradically rehash into other cylinder groups, until an
 *      available block is located.
 *
 * Caller holds inode I/O lock for writing.
 */
alloc(ip, lbn, bpref, size, bnp)
	register struct inode *ip;
	daddr_t lbn, bpref;
	int size;
	daddr_t *bnp;
{
	daddr_t bno;
	register struct fs *fs;
#if	QUOTA
	int error;
#endif
	int cg;
	struct ucred *cred = u.u_cred;		/* XXX */
	
	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	/*
	 * It is entirely possible that we will "see" free space
	 * when we enter this routine but that the subsequent
	 * allocation will fail.  Rather than try to lock the
	 * filesystem during this entire process, we simply let
	 * nature take its course -- as the original code did.
	 */
	*bnp = 0;
	fs = ip->i_fs;
	if ((unsigned)size > fs->fs_bsize || fragoff(fs, size) != 0) {
		printf("dev = 0x%x, bsize = %d, size = %d, fs = %s\n",
		    ip->i_dev, fs->fs_bsize, size, fs->fs_fsmnt);
		panic("alloc: bad size");
	}
	FS_LOCK(fs);
	if (size == fs->fs_bsize && fs->fs_cstotal.cs_nbfree == 0)
		goto nospace;
#if	SEC_BASE
	if (freespace(fs, fs->fs_minfree) <= 0 && !privileged(SEC_LIMIT, 0))
#else
	if (cred->cr_uid != 0 && freespace(fs, fs->fs_minfree) <= 0)
#endif
		goto nospace;
	FS_UNLOCK(fs);
#if	QUOTA
	if (error = chkdq(ip, (long)btodg(size), cred, 0))
		return (error);
#endif
	if (bpref >= fs->fs_size)
		bpref = 0;
	if (bpref == 0)
		cg = itog(fs, ip->i_number);
	else
		cg = dtog(fs, bpref);
	bno = (daddr_t)hashalloc(ip, cg, (long)bpref, size,
		(u_long (*)())alloccg);
	if (bno > 0) {
		ip->i_blocks += btodg(size);
		IN_LOCK(ip);
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		*bnp = bno;
		return (0);
	}
#if	QUOTA
	/*
	 * Restore user's disk quota because allocation failed
	 * after we put it on his tab.
	 */
	(void) chkdq(ip, (long)-btodg(size), cred, 0);
#endif
	goto out;
nospace:
	FS_UNLOCK(fs);
out:
	fserr(fs, "file system full");
	uprintf("\n%s: write failed, file system is full\n", fs->fs_fsmnt);
	return (ENOSPC);
}


/*
 * Reallocate a fragment to a bigger size
 *
 * The number and size of the old block is given, and a preference
 * and new size is also specified. The allocator attempts to extend
 * the original block. Failing that, the regular block allocator is
 * invoked to get an appropriate block.
 *
 * Caller holds inode I/O lock for writing.
 */
realloccg(ip, lbprev, bpref, osize, nsize, bpp)
	register struct inode *ip;
	off_t lbprev;
	daddr_t bpref;
	int osize, nsize;
	struct buf **bpp;
{
	register struct fs *fs;
	int cg, request;
	daddr_t bprev, bno;
	int error;
	long freesp;
	struct ucred *cred = u.u_cred;		/* XXX */
#if	QUOTA
	int quota_updated;
#endif
#if	!MACH
	daddr_t bn;
	int count;
#endif
	
#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(ITOV(ip)));
#endif
	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	*bpp = 0;
	fs = ip->i_fs;
	if ((unsigned)osize > fs->fs_bsize || fragoff(fs, osize) != 0 ||
	    (unsigned)nsize > fs->fs_bsize || fragoff(fs, nsize) != 0) {
		printf("dev = 0x%x,bsize = %d,osize = %d,nsize = %d,fs = %s\n",
		    ip->i_dev, fs->fs_bsize, osize, nsize, fs->fs_fsmnt);
		panic("realloccg: bad size");
	}
#if	QUOTA
	quota_updated = 0;
#endif
#if	SEC_BASE
	BM(FS_LOCK(fs));
	freesp = freespace(fs, fs->fs_minfree);
	BM(FS_UNLOCK(fs));
	if (freesp <= 0 && !privileged(SEC_LIMIT, 0))
		goto nospace;
#else
	if (cred->cr_uid != 0) {
		BM(FS_LOCK(fs));
		freesp = freespace(fs, fs->fs_minfree);
		BM(FS_UNLOCK(fs));
		if (freesp <= 0)
			goto nospace;
	}
#endif
	if ((bprev = ip->i_db[lbprev]) == 0) {
		printf("dev = 0x%x, bsize = %d, bprev = %d, fs = %s\n",
		    ip->i_dev, fs->fs_bsize, bprev, fs->fs_fsmnt);
		panic("realloccg: bad bprev");
	}
#if	QUOTA
	if (error = chkdq(ip, (long)btodg(nsize - osize), cred, 0))
		return (error);
	quota_updated++;
#endif
	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(fs));
	/*
	 * Check for extension in the existing location.
	 */
	cg = dtog(fs, bprev);
	if (bno = fragextend(ip, cg, (long)bprev, osize, nsize)) {
		if (error = realloc_extendbuf(ip, lbprev, osize, nsize, bpp)) {
			blkfree(ip, bno + numfrags(fs, osize),
				fragroundup(fs, nsize)-fragroundup(fs, osize));
			goto realloccg_error;
		}
		ip->i_blocks += btodg(nsize - osize);
		IN_LOCK(ip);
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		return (0);
	}
	/*
	 * Allocate a new disk location.
	 */
	if (bpref >= fs->fs_size)
		bpref = 0;
	FS_LOCK(fs);
	switch ((int)fs->fs_optim) {
	case FS_OPTSPACE:
		/*
		 * Allocate an exact sized fragment. Although this makes
		 * best use of space, we will waste time relocating it if
		 * the file continues to grow. If the fragmentation is
		 * less than half of the minimum free reserve, we choose
		 * to begin optimizing for time.
		 */
		request = nsize;
		if (fs->fs_minfree < 5 ||
		    fs->fs_cstotal.cs_nffree >
		    fs->fs_dsize * fs->fs_minfree / (2 * 100)) {
			FS_UNLOCK(fs);
			break;
		}
		fs->fs_optim = FS_OPTTIME;
		FS_UNLOCK(fs);
		log(LOG_NOTICE,
		    "%s: optimization changed from SPACE to TIME\n",
		    fs->fs_fsmnt);
		break;
	case FS_OPTTIME:
		/*
		 * At this point we have discovered a file that is trying
		 * to grow a small fragment to a larger fragment. To save
		 * time, we allocate a full sized block, then free the
		 * unused portion. If the file continues to grow, the
		 * `fragextend' call above will be able to grow it in place
		 * without further copying. If aberrant programs cause
		 * disk fragmentation to grow within 2% of the free reserve,
		 * we choose to begin optimizing for space.
		 */
		request = fs->fs_bsize;
		if (fs->fs_cstotal.cs_nffree <
		    fs->fs_dsize * (fs->fs_minfree - 2) / 100) {
			FS_UNLOCK(fs);
			break;
		}
		fs->fs_optim = FS_OPTSPACE;
		FS_UNLOCK(fs);
		log(LOG_NOTICE,
		    "%s: optimization changed from TIME to SPACE\n",
		    fs->fs_fsmnt);
		break;
	default:
		/*
		 *	File system has some bogus value in this field.
		 *	Fix it.
		 */
		fs->fs_optim = FS_OPTSPACE;
		FS_UNLOCK(fs);
		request = nsize;
		break;
	}
	LASSERT(!FS_LOCK_HOLDER(fs));
	bno = (daddr_t)hashalloc(ip, cg, (long)bpref, request,
		(u_long (*)())alloccg);
	LASSERT(IN_WRITE_HOLDER(ip));
	if (bno > 0) {
#if	MACH 
		if (ITOV(ip)->v_vm_info->pager != MEMORY_OBJECT_NULL)
			inode_uncache(ITOV(ip));
#else
		count = howmany(osize, CLBYTES);
		for (i = 0; i < count; i++)
			munhash(ip->i_devvp, bn + i * CLBYTES / DEV_BSIZE);
#endif
		if (error = realloc_movedata(ip, lbprev, bprev, bno,
					     osize, nsize, bpp)) {
			blkfree(ip, bno, (off_t) nsize);
			goto realloccg_error;
		}
		blkfree(ip, bprev, (off_t)osize);
		if (nsize < request)
			blkfree(ip, bno + numfrags(fs, nsize),
				(off_t)(request - nsize));
		ip->i_blocks += btodg(nsize - osize);
		IN_LOCK(ip);
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		LASSERT(IN_WRITE_HOLDER(ip));
		LASSERT(!FS_LOCK_HOLDER(fs));
		return (0);
	}
nospace:
	/*
	 * no space available
	 */
	fserr(fs, "file system full");
	uprintf("\n%s: write failed, file system is full\n", fs->fs_fsmnt);
	error = ENOSPC;
realloccg_error:
#if	QUOTA
	if (quota_updated)
		(void) chkdq(ip, (long)-btodg(nsize - osize), cred, 0);
#endif
	return (error);
}

/*
 * Extend buffer(s) to deal with block extension
 *
 * This routine extends buffers and clears buffers to reflect the 
 * extension of a logical block to a larger number of fragments.
 * Because a single logical block may span multiple buffers when
 * the buffer cache block size is smaller than the file system 
 * logical block size, this routine may need to deal with multiple
 * cache blocks, extending and creating blocks as necessary.
 *
 * The caller may provide us with the current last buffer of the
 * block and we return to him the new last buffer in the block.
 * Buffers that were extended, created, or passed to us and are no 
 * longer the last one are synchronously written.
 */
int
realloc_extendbuf(ip, lbn, osize, nsize, bpp)
	struct inode *ip;
	off_t lbn;
	int osize;
        int nsize;
	struct buf **bpp;
{
	int off;
	int ios;
	struct fs *fs = ip->i_fs;
	struct buf *bp = *bpp;
	int error = 0;
	int limit;
	int csize = osize;

	ios = iosection(fs, osize);
	off = iosectosize(fs, ios);

        /*
         * Extend the current last buffer cache block.
         */
	if (off < osize) {
		if (bp == NULL) {
		        error = bread(ITOV(ip), blktoiosec(fs, lbn) + ios,
				      osize - off, NOCRED, &bp);
			if (error) {
				brelse(bp);
				return (error);
			}
		}
		limit = MIN(nsize - off, ioseclen(fs));
		allocbuf(bp, limit);
		bzero(bp->b_un.b_addr + osize - off, 
		      (unsigned)limit - (osize-off));
		csize = off + limit;
		ios++;
		off += ioseclen(fs);
	}

        /*
         * Add new clear buffer cache blocks.  
	 */
	while (off < nsize) {
	        if (bp != NULL) {
			error = bwrite(bp);
			bp = NULL;
			if (error)
			        break;
		}
		limit = MIN(nsize - off, ioseclen(fs));
		bp = getblk(ITOV(ip), blktoiosec(fs, lbn) + ios, limit);
		bzero(bp->b_un.b_addr, (unsigned) limit);
		csize += limit;
		ios++;
		off += ioseclen(fs);
        }

        /*
         * Undo things if we has a write error.
	 */
	if (error) {
	        if (bp != NULL) 
		        brelse(bp);
		ios = iosection(fs, osize);
		off = iosectosize(fs, ios);
		while (off < csize) {
		        limit = MIN(csize - off, ioseclen(fs));
			bp = getblk(ITOV(ip), blktoiosec(fs, lbn) + ios, 
				    limit);
			if (off < osize && event_posted(&bp->b_iocomplete)) {
			        allocbuf(bp, osize-off);
				bp->b_blkno = bp->b_lblkno;
			}
			else
			        bp->b_flags |= B_INVAL;
			brelse(bp);
			ios++;
			off += ioseclen(fs);
		}
		bp = NULL;
	}
	*bpp = bp;
	return (error);
}

/* 
 * Move data to deal with a reallocation
 *
 * This routine moves data from one physical location to another to
 * reflect a reallocation made necessary by the extension of a logical
 * block to grater number of fragments.  Because the buffer cache block
 * size may be smaller than the file system logical block size, this
 * may involve reading and writing multiple buffers.
 *
 * Because the caller is going to be likely to write into the last 
 * buffer as part of file extension, this buffer is not written but 
 * returned to the caller.  Other buffers are written synchronously
 * to the new disk location.
 */
int
realloc_movedata(ip, lbn, oldbn, newbn, osize, nsize, bpp)
	struct inode *ip;
	off_t lbn;
        daddr_t oldbn;
        daddr_t newbn;
	int osize;
        int nsize;
	struct buf **bpp;
{
	int off;
	int ios;
	int iosmod = 0;
	struct fs *fs = ip->i_fs;
	struct buf *bp;
	char *temp;
	int error = 0;
	int limit;

	ZALLOC(temp_fs_zone, temp, char *);

        /*
         * Read each of the blocks from the old location and copy to a
         * temporary area.
	 */
	for (off = 0, ios = 0; off < osize; off += ioseclen(fs), ios++) {
		limit = MIN(osize - off, ioseclen(fs));
		error = bread(ITOV(ip), blktoiosec(fs, lbn) + ios,
				      limit, NOCRED, &bp);
		bcopy(bp->b_un.b_addr, temp+off, limit);
		brelse(bp);
		if (error)
		        goto finish;
	}

        /*
         * Update the disk location.
	 */
	bp = NULL;
	IN_LOCK(ip);
	ip->i_db[lbn] = newbn;
	ip->i_flag |= IUPD|ICHG;
	IN_UNLOCK(ip);

        /*
         * Get each of the buffers and copy the data into them.  All but
         * the last is written synchronously.
	 */
	for (off = 0, ios = 0; off < osize; off += ioseclen(fs), ios++) {
	        if (bp != NULL) {
			error = bwrite(bp);
			if (error) 
				goto finish;
		}
		limit = MIN(osize - off, ioseclen(fs));
		bp = getblk(ITOV(ip), blktoiosec(fs, lbn) + ios, limit);
		bp->b_blkno = fsbtodb(fs, newbn) + iosecdisp(fs, ios);
		bcopy(temp+off, bp->b_un.b_addr, limit);
		iosmod++;
	}

        /*
         * Now tack on the additional zero area.
	 */
	error = realloc_extendbuf(ip, lbn, osize, nsize, &bp);

        /*
         * All done.  Clean up.
	 */
finish:
	if (temp != NULL)
	        ZFREE(temp_fs_zone, temp);

        /*
	 * If we had an error, restore the old disk address and invalidate
         * the data.  It will get read from the old disk location when
         * needed.
	 */
	if (error) {
	        IN_LOCK(ip);
		ip->i_db[lbn] = oldbn;
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
	        for (ios = 0;  ios < iosmod; ios++) {
		        limit = MIN(osize - off, ioseclen(fs));
			bp = NULL;
			(void) bread(ITOV(ip), blktoiosec(fs, lbn) + ios,
				     limit, NOCRED, &bp);
			if (bp != NULL) {
			      bp->b_blkno = bp->b_lblkno;
                              bp->b_flags |= B_INVAL;
			}
			brelse(bp);
		}
		bp = NULL;
	}
	*bpp = bp;
	return(error);
}			
			
#if	UFS_NBC

/* statistics */
int			realloccg_calls = 0;
int			realloccg_extend_in_place = 0;
int			realloccg_extend_move = 0;
int			realloccg_extend_at_end = 0;

/*
 * Reallocate a fragment to a bigger size
 *
 * The number and size of the old block is given, and a preference
 * and new size is also specified. The allocator attempts to extend
 * the original block. Failing that, the regular block allocator is
 * invoked to get an appropriate block.
 *
 * Caller holds inode I/O lock for writing.
 *
 * realloccg_nbc differs from realloccg in that it returns a physical 
 * block number and buffer instead of a struct buf.  The buffer returned
 * is only valid if the frag could not be extended in place, in which case
 * it has nsize bytes of valid data (osize bytes from the old frag and 
 * nsize-osize bytes of zeroes at the end), but the actual size of the
 * buffer is round_page(nsize).
 *
 * The 'synchronize' parameter specifies whether it's necessary to synchronize
 * with other I/O's in progress.  A value of FALSE indicates that the caller
 * has taken care of synchronization.
 */
realloccg_nbc(ip, lbprev, bpref, osize, nsize, synchronize, bnp, buf)
	register struct inode 	*ip;
	off_t 			lbprev;
	daddr_t 		bpref;
	int 			osize, nsize;
	boolean_t		synchronize;
	daddr_t			*bnp;  		/* OUT */
	vm_address_t 		*buf;		/* OUT */
{
	register struct fs 	*fs;
	daddr_t 		bprev, bno;
	int 			cg, request, error;
	long 			freesp;
	struct ucred 		*cred = u.u_cred;		/* XXX */
	vm_address_t 		newbuf, tempbuf;
	boolean_t		is_reserved;
	struct vnode		*vp = ITOV(ip);
#if	QUOTA
	int 			quota_updated;
#endif

	ASSERT(VIO_IS_MAPPED(vp) || VIO_IS_FASTPATH(vp) || VIO_IS_PAGING(vp));
	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	*bnp = 0;
	*buf = NULL;
	realloccg_calls++;  			/* statistics */
	fs = ip->i_fs;
	if ((unsigned)osize > fs->fs_bsize || fragoff(fs, osize) != 0 ||
	    (unsigned)nsize > fs->fs_bsize || fragoff(fs, nsize) != 0) {
		printf("dev = 0x%x,bsize = %d,osize = %d,nsize = %d,fs = %s\n",
		    ip->i_dev, fs->fs_bsize, osize, nsize, fs->fs_fsmnt);
		panic("realloccg_nbc: bad size");
	}
#if	QUOTA
	quota_updated = 0;
#endif
#if	SEC_BASE
	BM(FS_LOCK(fs));
	freesp = freespace(fs, fs->fs_minfree);
	BM(FS_UNLOCK(fs));
	if (freesp <= 0 && !privileged(SEC_LIMIT, 0))
		goto nospace;
#else
	if (cred->cr_uid != 0) {
		BM(FS_LOCK(fs));
		freesp = freespace(fs, fs->fs_minfree);
		BM(FS_UNLOCK(fs));
		if (freesp <= 0)
			goto nospace;
	}
#endif
	/*
	 * Remember whether the block being grown is reserved.
	 */
	is_reserved = IS_RESERVED(ip->i_db[lbprev]);
	
	if ((bprev = DADDR(ip->i_db[lbprev])) == 0) {
		printf("dev = 0x%x, bsize = %d, bprev = %d, fs = %s\n",
		    ip->i_dev, fs->fs_bsize, bprev, fs->fs_fsmnt);
		panic("realloccg_nbc: bad bprev");
	}
#if	QUOTA
	if (error = chkdq(ip, (long)btodg(nsize - osize), cred, 0))
		return (error);
	quota_updated++;
#endif

	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(fs));
	/*
	 * Check for extension in the existing location.
	 */
	cg = dtog(fs, bprev);
	if (bno = fragextend(ip, cg, (long)bprev, osize, nsize)) {
		realloccg_extend_in_place++; 		/* statistics */
		if (bprev != bno)
			panic("bad blockno");

		ip->i_blocks += btodg(nsize - osize);
		IN_LOCK(ip);
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		*bnp = is_reserved ? RESERVE(bno) : bno;
		return (0);
	}
	/*
	 * Allocate a new disk location.
	 */
	if (bpref >= fs->fs_size)
		bpref = 0;
	FS_LOCK(fs);
	switch ((int)fs->fs_optim) {
	case FS_OPTSPACE:
		/*
		 * Allocate an exact sized fragment. Although this makes
		 * best use of space, we will waste time relocating it if
		 * the file continues to grow. If the fragmentation is
		 * less than half of the minimum free reserve, we choose
		 * to begin optimizing for time.
		 */
		request = nsize;
		if (fs->fs_minfree < 5 ||
		    fs->fs_cstotal.cs_nffree >
		    fs->fs_dsize * fs->fs_minfree / (2 * 100)) {
			FS_UNLOCK(fs);
			break;
		}
		fs->fs_optim = FS_OPTTIME;
		FS_UNLOCK(fs);
		log(LOG_NOTICE,
		    "%s: optimization changed from SPACE to TIME\n",
		    fs->fs_fsmnt);
		break;
	case FS_OPTTIME:
		/*
		 * At this point we have discovered a file that is trying
		 * to grow a small fragment to a larger fragment. To save
		 * time, we allocate a full sized block, then free the
		 * unused portion. If the file continues to grow, the
		 * `fragextend' call above will be able to grow it in place
		 * without further copying. If aberrant programs cause
		 * disk fragmentation to grow within 2% of the free reserve,
		 * we choose to begin optimizing for space.
		 */
		request = fs->fs_bsize;
		if (fs->fs_cstotal.cs_nffree <
		    fs->fs_dsize * (fs->fs_minfree - 2) / 100) {
			FS_UNLOCK(fs);
			break;
		}
		fs->fs_optim = FS_OPTSPACE;
		FS_UNLOCK(fs);
		log(LOG_NOTICE,
		    "%s: optimization changed from TIME to SPACE\n",
		    fs->fs_fsmnt);
		break;
	default:
		/*
		 *	File system has some bogus value in this field.
		 *	Fix it.
		 */
		fs->fs_optim = FS_OPTSPACE;
		FS_UNLOCK(fs);
		request = nsize;
		break;
	}
	LASSERT(!FS_LOCK_HOLDER(fs));
	bno = (daddr_t)hashalloc(ip, cg, (long)bpref, request,
		(u_long (*)())alloccg);
	LASSERT(IN_WRITE_HOLDER(ip));

	if (bno > 0) {
		if (is_reserved)
			newbuf = NULL;	/* don't need to read reserved blks */
		else {
			/*
			 * Read data from the old block.  However, we must read
			 * nsize bytes which is a problem if bprev is a frag in 
			 * the last block of the partition.  Account for this.
			 */
			realloccg_extend_move++;	/* statistics */
			if (fs->fs_size - bprev >= fs->fs_frag) {
				/* things are cool */
				if (VIO_IS_FASTPATH(vp)) {
					error = vio_read(vp, fs->fs_devinfo,
						         lbprev, 1,
							 fsbtodb(fs, bprev), 
							 btodg(nsize),
							 synchronize,
							 &newbuf);
					if (error)
						goto realloccg_bad;
				        ASSERT(newbuf != NULL);
				} else {
					error = data_read(fs->fs_devinfo, 
							  fsbtodb(fs, bprev), 
							  btodg(nsize), 
							  0, 0, &newbuf);
					if (error)
						goto realloccg_bad;
				}
			} else {
				/* read data and copy into the right size buf */
				realloccg_extend_at_end++;	/* statistics */
				if (vm_allocate(mach_task_self(), &newbuf,
						nsize, TRUE) != KERN_SUCCESS) {
					error = ENOMEM;
					goto realloccg_bad;
				}
				if (VIO_IS_FASTPATH(vp)) {
					error = vio_read(vp, fs->fs_devinfo,
							 lbprev, 1, 
							 fsbtodb(fs, bprev), 
							 btodg(osize),
							 synchronize,
							 &tempbuf);
					if (error)
						goto realloccg_bad;
				} else {
					error = data_read(fs->fs_devinfo, 
							  fsbtodb(fs, bprev), 
							  btodg(osize), 
							  0, 0, &tempbuf);
					if (error)
						goto realloccg_bad;
				}

				ASSERT(tempbuf != NULL);
				bcopy((char *)tempbuf, (char *)newbuf, osize);
				(void) vm_deallocate(mach_task_self(), tempbuf,
						     osize);
			}
			/* zero the newly allocated frags */
			bzero((char *)newbuf+osize, (unsigned)nsize - osize);
		}

#if	MACH 
#if	MAPPED_FILES
		if (VIO_IS_FASTPATH(ITOV(ip)))
#endif
			if (ITOV(ip)->v_vm_info->pager != MEMORY_OBJECT_NULL)
				inode_uncache(ITOV(ip));
#endif
		blkfree(ip, bprev, (off_t)osize);
		if (nsize < request)
			blkfree(ip, bno + numfrags(fs, nsize),
				(off_t)(request - nsize));
		ip->i_blocks += btodg(nsize - osize);
		IN_LOCK(ip);
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
		LASSERT(IN_WRITE_HOLDER(ip));
		LASSERT(!FS_LOCK_HOLDER(fs));
		*bnp = is_reserved ? RESERVE(bno) : bno;
		*buf = newbuf;
		return (0);
realloccg_bad:
		blkfree(ip, bno, (off_t)request);
		goto realloccg_error;
	}
nospace:
	/*
	 * no space available
	 */
	fserr(fs, "file system full");
	uprintf("\n%s: write failed, file system is full\n", fs->fs_fsmnt);
	error = ENOSPC;
realloccg_error:
#if	QUOTA
	if (quota_updated)
		(void) chkdq(ip, (long)-btodg(nsize - osize), cred, 0);
#endif
	return (error);
}

#endif	/* UFS_NBC */


/*
 * Allocate an inode in the file system.
 *
 * A preference may be optionally specified. If a preference is given
 * the following hierarchy is used to allocate an inode:
 *   1) allocate the requested inode.
 *   2) allocate an inode in the same cylinder group.
 *   3) quadradically rehash into other cylinder groups, until an
 *      available inode is located.
 * If no inode preference is given the following heirarchy is used
 * to allocate an inode:
 *   1) allocate an inode in cylinder group 0.
 *   2) quadradically rehash into other cylinder groups, until an
 *      available inode is located.
 *
 * Caller usually holds no locks on pip, although this is not a requirement.
 */
ialloc(pip, ipref, mode, ipp)
	register struct inode *pip;
	ino_t ipref;
	int mode;
	struct inode **ipp;
{
	ino_t ino;
	register struct fs *fs;
	register struct inode *ip;
	int cg, error;
	long csnifree;

	LASSERT(!FS_LOCK_HOLDER(pip->i_fs));
	*ipp = 0;
	fs = pip->i_fs;
	BM(FS_LOCK(fs));
	csnifree = fs->fs_cstotal.cs_nifree;
	BM(FS_UNLOCK(fs));
	if (csnifree == 0)
		goto noinodes;
	if (ipref >= fs->fs_ncg * fs->fs_ipg)
		ipref = 0;
	cg = itog(fs, ipref);
	ino = (ino_t)hashalloc(pip, cg, (long)ipref, mode, ialloccg);
	if (ino == 0)
		goto noinodes;
	error = iget(pip, ino, ipp, 0);
	if (error) {
		ifree(pip, ino, 0);
		return (error);
	}
	ip = *ipp;
#ifdef	PFS
	ASSERT((ITOV(ip)->v_op == &ufs_vnodeops) ||
		(ITOV(ip)->v_op == &pfs_vnodeops));
#else
	ASSERT(ITOV(ip)->v_op == &ufs_vnodeops);
#endif	PFS
	/*
	 * No one else knows about this inode yet so there's no need
	 * to take any inode locks.
	 */
	if (ip->i_mode) {
		printf("mode = 0%o, inum = %d, fs = %s\n",
		    ip->i_mode, ip->i_number, fs->fs_fsmnt);
		panic("ialloc: dup alloc");
	}
	if (ip->i_blocks) {				/* XXX */
		printf("free inode %s/%d had %d blocks\n",
		    fs->fs_fsmnt, ino, ip->i_blocks);
		ip->i_blocks = 0;
	}

	/*
	 *	i_flags holds fast link bit.
	 */
	ip->i_flags = 0;
#if	SEC_FSCHANGE
	sec_ialloc(ip, pip);
#endif
	/*
	 * Set up a new generation number for this inode.
	 */
	ip->i_gen = get_nextgen();
	return (0);
noinodes:
	fserr(fs, "out of inodes");
	uprintf("\n%s: create/symlink failed, no inodes free\n", fs->fs_fsmnt);
	return (ENOSPC);
}

/*
 * Find a cylinder to place a directory.
 *
 * The policy implemented by this algorithm is to select from
 * among those cylinder groups with above the average number of
 * free inodes, the one with the smallest number of directories.
 */
ino_t
dirpref(fs)
	register struct fs *fs;
{
	int cg, minndir, mincg, avgifree;

	BM(FS_LOCK(fs));
	avgifree = fs->fs_cstotal.cs_nifree / fs->fs_ncg;
	BM(FS_UNLOCK(fs));
	minndir = fs->fs_ipg;
	mincg = 0;
	for (cg = 0; cg < fs->fs_ncg; cg++) {
		/*
		 * If there's a contention problem on the fs_lock,
		 * recoding the lock as a non-blocking read/write
		 * lock might help for loops like this.
		 */
		BM(FS_LOCK(fs));
		if (fs->fs_cs(fs, cg).cs_ndir < minndir &&
		    fs->fs_cs(fs, cg).cs_nifree >= avgifree) {
			mincg = cg;
			minndir = fs->fs_cs(fs, cg).cs_ndir;
		}
		BM(FS_UNLOCK(fs));
	}
	return ((ino_t)(fs->fs_ipg * mincg));
}

/*
 * Select the desired position for the next block in a file.  The file is
 * logically divided into sections. The first section is composed of the
 * direct blocks. Each additional section contains fs_maxbpg blocks.
 *
 * If no blocks have been allocated in the first section, the policy is to
 * request a block in the same cylinder group as the inode that describes
 * the file. If no blocks have been allocated in any other section, the
 * policy is to place the section in a cylinder group with a greater than
 * average number of free blocks.  An appropriate cylinder group is found
 * by using a rotor that sweeps the cylinder groups. When a new group of
 * blocks is needed, the sweep begins in the cylinder group following the
 * cylinder group from which the previous allocation was made. The sweep
 * continues until a cylinder group with greater than the average number
 * of free blocks is found. If the allocation is for the first block in an
 * indirect block, the information on the previous allocation is unavailable;
 * here a best guess is made based upon the logical block number being
 * allocated.
 *
 * If a section is already partially allocated, the policy is to
 * contiguously allocate fs_maxcontig blocks.  The end of one of these
 * contiguous blocks and the beginning of the next is physically separated
 * so that the disk head will be in transit between them for at least
 * fs_rotdelay milliseconds.  This is to allow time for the processor to
 * schedule another I/O transfer.
 *
 * Caller holds the inode I/O lock for writing.  Bap is presumed to point
 * to an array of daddr_t's that is either NULL; the inode's direct blocks;
 * or one of the inode's indirect blocks, all of which are protected by the
 * inode I/O lock.
 */
int	blkpref_fails = 0;
vdecl_simple_lock_data(,blkpref_handy_lock)

daddr_t
blkpref(ip, lbn, indx, bap)
	struct inode *ip;
	daddr_t lbn;
	int indx;
	daddr_t *bap;
{
	register struct fs *fs;
	register int cg;
	int avgbfree, startcg;
	daddr_t nextblk;

	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	if (indx % fs->fs_maxbpg == 0 || DADDR(bap[indx - 1]) == 0) {
		if (lbn < NDADDR) {
			cg = itog(fs, ip->i_number);
			return (fs->fs_fpg * cg + fs->fs_frag);
		}
		/*
		 * Find a cylinder with greater than average number of
		 * unused data blocks.
		 */
		if (indx == 0 || DADDR(bap[indx - 1]) == 0)
			startcg = itog(fs, ip->i_number) + lbn / fs->fs_maxbpg;
		else
			startcg = dtog(fs, DADDR(bap[indx - 1])) + 1;
		startcg %= fs->fs_ncg;
		BM(FS_LOCK(fs));
		avgbfree = fs->fs_cstotal.cs_nbfree / fs->fs_ncg;
		BM(FS_UNLOCK(fs));
		for (cg = startcg; cg < fs->fs_ncg; cg++) {
			BM(FS_LOCK(fs));
			if (fs->fs_cs(fs, cg).cs_nbfree >= avgbfree) {
				NM(FS_LOCK(fs));
				fs->fs_cgrotor = cg;
				FS_UNLOCK(fs);
				return (fs->fs_fpg * cg + fs->fs_frag);
			}
			BM(FS_UNLOCK(fs));
		}
		for (cg = 0; cg <= startcg; cg++) {
			BM(FS_LOCK(fs));
			if (fs->fs_cs(fs, cg).cs_nbfree >= avgbfree) {
				NM(FS_LOCK(fs));
				fs->fs_cgrotor = cg;
				FS_UNLOCK(fs);
				return (fs->fs_fpg * cg + fs->fs_frag);
			}
			BM(FS_UNLOCK(fs));
		}
		/*
		 * We could have done the above search under lock,
		 * but generally speaking we should be able to find
		 * a cylinder with avgbfree even while other allocations
		 * are happening.  If the search fails, we could repeat
		 * it, holding the filesystem lock the second time around;
		 * but it's not unreasonable to just return "no preference".
		 * Nevertheless, we're interested in how many times we do
		 * just that
		 */
		STATS_ACTION(&blkpref_handy_lock, ++blkpref_fails);
		LASSERT(!FS_LOCK_HOLDER(fs));
		LASSERT(IN_WRITE_HOLDER(ip));
		return (NULL);
	}
	/*
	 * One or more previous blocks have been laid out. If less
	 * than fs_maxcontig previous blocks are contiguous, the
	 * next block is requested contiguously, otherwise it is
	 * requested rotationally delayed by fs_rotdelay milliseconds.
	 */
	nextblk = DADDR(bap[indx - 1]) + fs->fs_frag;
	if (indx > fs->fs_maxcontig &&
	    DADDR(bap[indx - fs->fs_maxcontig]) + 
	    blkstofrags(fs, fs->fs_maxcontig)
	    != nextblk)
		return (nextblk);
	if (fs->fs_rotdelay != 0)
		/*
		 * Here we convert ms of delay to frags as:
		 * (frags) = (ms) * (rev/sec) * (sect/rev) /
		 *	((sect/frag) * (ms/sec))
		 * then round up to the next block.
		 */
		nextblk +=roundup(fs->fs_rotdelay * fs->fs_rps * fs->fs_nsect /
		    (NSPF(fs) * 1000), fs->fs_frag);
	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(fs));
	return (nextblk);
}

/*
 * Implement the cylinder overflow algorithm.
 *
 * The policy implemented by this algorithm is:
 *   1) allocate the block in its requested cylinder group.
 *   2) quadradically rehash on the cylinder group number.
 *   3) brute force search for a free block.
 *
 * Caller may or may not hold the inode's I/O lock for writing.
 * The cylinder group is always protected by its buffer's lock.
 */
/*VARARGS5*/
u_long
hashalloc(ip, cg, pref, size, allocator)
	struct inode *ip;
	int cg;
	long pref;
	int size;	/* size for data blocks, mode for inodes */
	u_long (*allocator)();
{
	register struct fs *fs;
	long result;
	int i, icg = cg;

	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	/*
	 * 1: preferred cylinder group
	 */
	result = (*allocator)(ip, cg, pref, size);
	if (result)
		return (result);
	/*
	 * 2: quadratic rehash
	 */
	for (i = 1; i < fs->fs_ncg; i *= 2) {
		cg += i;
		if (cg >= fs->fs_ncg)
			cg -= fs->fs_ncg;
		result = (*allocator)(ip, cg, 0, size);
		if (result)
			return (result);
	}
	/*
	 * 3: brute force search
	 * Note that we start at i == 2, since 0 was checked initially,
	 * and 1 is always checked in the quadratic rehash.
	 */
	cg = (icg + 2) % fs->fs_ncg;
	for (i = 2; i < fs->fs_ncg; i++) {
		result = (*allocator)(ip, cg, 0, size);
		if (result)
			return (result);
		cg++;
		if (cg == fs->fs_ncg)
			cg = 0;
	}
	return (NULL);
}

/*
 * Determine whether a fragment can be extended.
 *
 * Check to see if the necessary fragments are available, and
 * if they are, allocate them.
 *
 * Caller holds inode I/O lock for writing.  Cylinder group is
 * protected by its buffer's lock.
 */
daddr_t
fragextend(ip, cg, bprev, osize, nsize)
	struct inode *ip;
	int cg;
	long bprev;
	int osize, nsize;
{
	register struct fs *fs;
	register struct cg *cgp;
	struct buf *bp;
	long bno;
	int frags, bbase, csnffree, ofrags;
	int i, error, s;

	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	ASSERT(nsize > osize);

	fs = ip->i_fs;
	BM(FS_LOCK(fs));
	csnffree = fs->fs_cs(fs, cg).cs_nffree;
	BM(FS_UNLOCK(fs));
	if (csnffree < numfrags(fs, nsize - osize))
		return (NULL);
	frags = numfrags(fs, nsize);
	bbase = fragnum(fs, bprev);
	if (bbase > fragnum(fs, (bprev + frags - 1))) {
		/* cannot extend across a block boundary */
		return (NULL);
	}
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return (NULL);
	}
#if	UNIX_LOCKS
	/*
	 * Recheck whether there are really enough fragments
	 * available.  Now that we hold the cylinder group's
	 * buffer, this count will not change unless we change it.
	 * Why wasn't this done for the uniprocessor case?  XXX
	 */
	BM(FS_LOCK(fs));
	csnffree = fs->fs_cs(fs, cg).cs_nffree;
	BM(FS_UNLOCK(fs));
	if (csnffree < numfrags(fs, nsize - osize)) {
		brelse(bp);
		return (NULL);
	}
#endif
	cgp = bp->b_un.b_cg;
	if (!cg_chkmagic(cgp)) {
		brelse(bp);
		return (NULL);
	}
	s = splhigh();
	TIME_READ_LOCK();
	cgp->cg_time = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	bno = dtogd(fs, bprev);
	LASSERT(BUF_LOCK_HOLDER(bp));
	ofrags = numfrags(fs, osize);
	for (i = ofrags; i < frags; i++)
		if (isclr(cg_blksfree(cgp), bno + i)) {
			brelse(bp);
			return (NULL);
		}
	/*
	 * the current fragment can be extended
	 * deduct the count on fragment being extended into
	 * increase the count on the remaining fragment (if any)
	 * allocate the extended piece
	 */
	LASSERT(BUF_LOCK_HOLDER(bp));
	for (i = frags; i < fs->fs_frag - bbase; i++)
		if (isclr(cg_blksfree(cgp), bno + i))
			break;
	cgp->cg_frsum[i - ofrags]--;
	if (i != frags)
		cgp->cg_frsum[i - frags]++;
	for (i = ofrags; i < frags; i++) {
		clrbit(cg_blksfree(cgp), bno + i);
		cgp->cg_cs.cs_nffree--;
		FS_LOCK(fs);
		fs->fs_cstotal.cs_nffree--;
		fs->fs_cs(fs, cg).cs_nffree--;
		fs->fs_fmod++;
		FS_UNLOCK(fs);
	}
	bdwrite(bp, bp->b_vp);
	return (bprev);
}

/*
 * Determine whether a block can be allocated.
 *
 * Check to see if a block of the apprpriate size is available,
 * and if it is, allocate it.
 *
 * Caller holds inode I/O lock for writing.  Cylinder group protected
 * by its buffer's lock.
 */
daddr_t
alloccg(ip, cg, bpref, size)
	struct inode *ip;
	int cg;
	daddr_t bpref;
	int size;
{
	register struct fs *fs;
	register struct cg *cgp;
	struct buf *bp;
	register int i;
	int error, bno, frags, allocsiz, s;
	long csnbfree;

	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	BM(FS_LOCK(fs));
	csnbfree = fs->fs_cs(fs, cg).cs_nbfree;
	BM(FS_UNLOCK(fs));
	if (csnbfree == 0 && size == fs->fs_bsize)
		return (NULL);
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return (NULL);
	}
	cgp = bp->b_un.b_cg;
	if (!cg_chkmagic(cgp) ||
	    (cgp->cg_cs.cs_nbfree == 0 && size == fs->fs_bsize)) {
		brelse(bp);
		return (NULL);
	}
	s = splhigh();
	TIME_READ_LOCK();
	cgp->cg_time = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	if (size == fs->fs_bsize) {
		bno = alloccgblk(fs, cgp, bpref);
		bdwrite(bp, bp->b_vp);
		return (bno);
	}
	/*
	 * check to see if any fragments are already available
	 * allocsiz is the size which will be allocated, hacking
	 * it down to a smaller size if necessary
	 */
	frags = numfrags(fs, size);
	for (allocsiz = frags; allocsiz < fs->fs_frag; allocsiz++)
		if (cgp->cg_frsum[allocsiz] != 0)
			break;
	if (allocsiz == fs->fs_frag) {
		/*
		 * no fragments were available, so a block will be
		 * allocated, and hacked up
		 */
		if (cgp->cg_cs.cs_nbfree == 0) {
			brelse(bp);
			return (NULL);
		}
		bno = alloccgblk(fs, cgp, bpref);
		bpref = dtogd(fs, bno);
		for (i = frags; i < fs->fs_frag; i++)
			setbit(cg_blksfree(cgp), bpref + i);
		i = fs->fs_frag - frags;
		cgp->cg_cs.cs_nffree += i;
		FS_LOCK(fs);
		fs->fs_cstotal.cs_nffree += i;
		fs->fs_cs(fs, cg).cs_nffree += i;
		fs->fs_fmod++;
		FS_UNLOCK(fs);
		cgp->cg_frsum[i]++;
		bdwrite(bp, bp->b_vp);
		return (bno);
	}
	bno = mapsearch(fs, cgp, bpref, allocsiz);
	if (bno < 0) {
		brelse(bp);
		return (NULL);
	}
	for (i = 0; i < frags; i++)
		clrbit(cg_blksfree(cgp), bno + i);
	cgp->cg_cs.cs_nffree -= frags;
	FS_LOCK(fs);
	fs->fs_cstotal.cs_nffree -= frags;
	fs->fs_cs(fs, cg).cs_nffree -= frags;
	fs->fs_fmod++;
	FS_UNLOCK(fs);
	cgp->cg_frsum[allocsiz]--;
	if (frags != allocsiz)
		cgp->cg_frsum[allocsiz - frags]++;
	bdwrite(bp, bp->b_vp);
	return (cg * fs->fs_fpg + bno);
}

/*
 * Allocate a block in a cylinder group.
 *
 * This algorithm implements the following policy:
 *   1) allocate the requested block.
 *   2) allocate a rotationally optimal block in the same cylinder.
 *   3) allocate the next available block on the block rotor for the
 *      specified cylinder group.
 * Note that this routine only allocates fs_bsize blocks; these
 * blocks may be fragmented by the routine that allocates them.
 *
 * Cylinder group protected by its buffer's lock.
 */
daddr_t
alloccgblk(fs, cgp, bpref)
	register struct fs *fs;
	register struct cg *cgp;
	daddr_t bpref;
{
	daddr_t bno;
	int cylno, pos, delta;
	short *cylbp;
	register int i;

	/*
	 * cgp's buffer should be locked, no easy way to check.  XXX
	 */
	LASSERT(!FS_LOCK_HOLDER(fs));
	if (bpref == 0) {
		bpref = cgp->cg_rotor;
		goto norot;
	}
	bpref = blknum(fs, bpref);
	bpref = dtogd(fs, bpref);
	/*
	 * if the requested block is available, use it
	 */
	if (isblock(fs, cg_blksfree(cgp), fragstoblks(fs, bpref))) {
		bno = bpref;
		goto gotit;
	}
	/*
	 * check for a block available on the same cylinder
	 */
	cylno = cbtocylno(fs, bpref);
	if (cg_blktot(cgp)[cylno] == 0)
		goto norot;
#ifdef	multimax
	/*
	 * block layout info is not available, so just have
	 * to take any block in this cylinder.
	 */
	bpref = howmany(fs->fs_spc * cylno, NSPF(fs));
#else	/* multimax */
	if (fs->fs_cpc == 0) {
		/*
		 * block layout info is not available, so just have
		 * to take any block in this cylinder.
		 */
		bpref = howmany(fs->fs_spc * cylno, NSPF(fs));
		goto norot;
	}
	/*
	 * check the summary information to see if a block is
	 * available in the requested cylinder starting at the
	 * requested rotational position and proceeding around.
	 */
	cylbp = cg_blks(fs, cgp, cylno);
	pos = cbtorpos(fs, bpref);
	for (i = pos; i < fs->fs_nrpos; i++)
		if (cylbp[i] > 0)
			break;
	if (i == fs->fs_nrpos)
		for (i = 0; i < pos; i++)
			if (cylbp[i] > 0)
				break;
	if (cylbp[i] > 0) {
		/*
		 * found a rotational position, now find the actual
		 * block. A panic if none is actually there.
		 */
		pos = cylno % fs->fs_cpc;
		bno = (cylno - pos) * fs->fs_spc / NSPB(fs);
		if (fs_postbl(fs, pos)[i] == -1) {
			printf("pos = %d, i = %d, fs = %s\n",
			    pos, i, fs->fs_fsmnt);
			panic("alloccgblk: cyl groups corrupted");
		}
		for (i = fs_postbl(fs, pos)[i];; ) {
			if (isblock(fs, cg_blksfree(cgp), bno + i)) {
				bno = blkstofrags(fs, (bno + i));
				goto gotit;
			}
			delta = fs_rotbl(fs)[i];
			if (delta <= 0 ||
			    delta + i > fragstoblks(fs, fs->fs_fpg))
				break;
			i += delta;
		}
		printf("pos = %d, i = %d, fs = %s\n", pos, i, fs->fs_fsmnt);
		panic("alloccgblk: can't find blk in cyl");
	}
#endif	/* multimax */
norot:
	/*
	 * no blocks in the requested cylinder, so take next
	 * available one in this cylinder group.
	 */
	bno = mapsearch(fs, cgp, bpref, (int)fs->fs_frag);
	if (bno < 0)
		return (NULL);
	cgp->cg_rotor = bno;
gotit:
	clrblock(fs, cg_blksfree(cgp), (long)fragstoblks(fs, bno));
	cgp->cg_cs.cs_nbfree--;
	FS_LOCK(fs);
	fs->fs_cstotal.cs_nbfree--;
	fs->fs_cs(fs, cgp->cg_cgx).cs_nbfree--;
	fs->fs_fmod++;
	FS_UNLOCK(fs);
	cylno = cbtocylno(fs, bno);
	cg_blks(fs, cgp, cylno)[cbtorpos(fs, bno)]--;
	cg_blktot(cgp)[cylno]--;
	return (cgp->cg_cgx * fs->fs_fpg + bno);
}

/*
 * Determine whether an inode can be allocated.
 *
 * Check to see if an inode is available, and if it is,
 * allocate it using the following policy:
 *   1) allocate the requested inode.
 *   2) allocate the next available inode after the requested
 *      inode in the specified cylinder group.
 *
 * Caller may or may not hold inode I/O lock for writing.  Cylinder
 * group protected by its buffer's lock.
 */
ino_t
ialloccg(ip, cg, ipref, mode)
	struct inode *ip;
	int cg;
	daddr_t ipref;
	int mode;
{
	register struct fs *fs;
	register struct cg *cgp;
	struct buf *bp;
	int error, start, len, loc, map, i, s;
	long csnifree;

	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	BM(FS_LOCK(fs));
	csnifree = fs->fs_cs(fs, cg).cs_nifree;
	BM(FS_UNLOCK(fs));
	if (csnifree == 0)
		return (NULL);
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return (NULL);
	}
	cgp = bp->b_un.b_cg;
	if (!cg_chkmagic(cgp) || cgp->cg_cs.cs_nifree == 0) {
		brelse(bp);
		return (NULL);
	}
	s = splhigh();
	TIME_READ_LOCK();
	cgp->cg_time = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	if (ipref) {
		ipref %= fs->fs_ipg;
		if (isclr(cg_inosused(cgp), ipref))
			goto gotit;
	}
	start = cgp->cg_irotor / NBBY;
	len = howmany(fs->fs_ipg - cgp->cg_irotor, NBBY);
	loc = skpc(0xff, len, &cg_inosused(cgp)[start]);
	if (loc == 0) {
		len = start + 1;
		start = 0;
		loc = skpc(0xff, len, &cg_inosused(cgp)[0]);
		if (loc == 0) {
#if	SEC_FSCHANGE
			/*
			 * number of inodes in a cyl group may not fit
			 * exactly into the byte array.  Check last byte.
			 */
			len = fs->fs_ipg % NBBY;
			if(len) {
				loc = howmany(fs->fs_ipg, NBBY) - 1;
				map = cg_inosused(cgp)[loc];
				ipref = loc * NBBY;
				for(i=1; i < (1 << len); i <<= 1, ipref++) {
					if((map & i) == 0) {
						cgp->cg_irotor = ipref;
						goto gotit;
					}
				}
			}
#endif
			printf("cg = %s, irotor = %d, fs = %s\n",
			    cg, cgp->cg_irotor, fs->fs_fsmnt);
			panic("ialloccg: map corrupted");
			/* NOTREACHED */
		}
	}
	i = start + len - loc;
	map = cg_inosused(cgp)[i];
	ipref = i * NBBY;
	for (i = 1; i < (1 << NBBY); i <<= 1, ipref++) {
		if ((map & i) == 0) {
			cgp->cg_irotor = ipref;
			goto gotit;
		}
	}
	printf("fs = %s\n", fs->fs_fsmnt);
	panic("ialloccg: block not in map");
	/* NOTREACHED */
gotit:
	setbit(cg_inosused(cgp), ipref);
	cgp->cg_cs.cs_nifree--;
	FS_LOCK(fs);
	fs->fs_cstotal.cs_nifree--;
	fs->fs_cs(fs, cg).cs_nifree--;
	fs->fs_fmod++;
	if ((mode & IFMT) == IFDIR) {
		cgp->cg_cs.cs_ndir++;
		fs->fs_cstotal.cs_ndir++;
		fs->fs_cs(fs, cg).cs_ndir++;
	}
	FS_UNLOCK(fs);
	bdwrite(bp, bp->b_vp);
	return (cg * fs->fs_ipg + ipref);
}

/*
 * Free a block or fragment.
 *
 * The specified block or fragment is placed back in the
 * free map. If a fragment is deallocated, a possible
 * block reassembly is checked.
 *
 * Caller holds inode I/O lock for writing.
 */
blkfree(ip, bno, size)
	register struct inode *ip;
	daddr_t bno;
	off_t size;
{
	register struct fs *fs;
	register struct cg *cgp;
	struct buf *bp;
	int error, cg, blk, frags, bbase;
	register int i, s;

	LASSERT(IN_WRITE_HOLDER(ip));
	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	if ((unsigned)size > fs->fs_bsize || fragoff(fs, size) != 0) {
		printf("dev = 0x%x, bsize = %d, size = %d, fs = %s\n",
		    ip->i_dev, fs->fs_bsize, size, fs->fs_fsmnt);
		panic("blkfree: bad size");
	}
	if (IS_RESERVED(bno)) {
#ifdef PFS
		if (ip->i_flags & IC_PREALLOCATED)
			/*
			 * Clear reserved bit from block address since it is
			 * OK to deallocate preallocated blocks.
			 */
			bno = DADDR(bno);
		else
#endif PFS
			panic("blkfree: reserved daddr encountered %d",
			      DADDR(bno));
	}
	cg = dtog(fs, bno);
	if (badblock(fs, bno)) {
		printf("bad block %d, ino %d\n", bno, ip->i_number);
		return;
	}
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return;
	}
	cgp = bp->b_un.b_cg;
	if (!cg_chkmagic(cgp)) {
		brelse(bp);
		return;
	}
	s = splhigh();
	TIME_READ_LOCK();
	cgp->cg_time = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	bno = dtogd(fs, bno);
	if (size == fs->fs_bsize) {
		if (isblock(fs, cg_blksfree(cgp), fragstoblks(fs, bno))) {
			printf("dev = 0x%x, block = %d, fs = %s\n",
			    ip->i_dev, bno, fs->fs_fsmnt);
			panic("blkfree: freeing free block");
		}
		setblock(fs, cg_blksfree(cgp), fragstoblks(fs, bno));
		cgp->cg_cs.cs_nbfree++;
		FS_LOCK(fs);
		fs->fs_cstotal.cs_nbfree++;
		fs->fs_cs(fs, cg).cs_nbfree++;
		FS_UNLOCK(fs);
		i = cbtocylno(fs, bno);
		cg_blks(fs, cgp, i)[cbtorpos(fs, bno)]++;
		cg_blktot(cgp)[i]++;
	} else {
		bbase = bno - fragnum(fs, bno);
		/*
		 * decrement the counts associated with the old frags
		 */
		blk = blkmap(fs, cg_blksfree(cgp), bbase);
		fragacct(fs, blk, cgp->cg_frsum, -1);
		/*
		 * deallocate the fragment
		 */
		frags = numfrags(fs, size);
		for (i = 0; i < frags; i++) {
			if (isset(cg_blksfree(cgp), bno + i)) {
				printf("dev = 0x%x, block = %d, fs = %s\n",
				    ip->i_dev, bno + i, fs->fs_fsmnt);
#if	MACH
				printf("size = 0x%x, frags = 0x%x, i = 0x%x\n",
					size, frags, i);
#endif
				panic("blkfree: freeing free frag");
			}
			setbit(cg_blksfree(cgp), bno + i);
		}
		cgp->cg_cs.cs_nffree += i;
		FS_LOCK(fs);
		fs->fs_cstotal.cs_nffree += i;
		fs->fs_cs(fs, cg).cs_nffree += i;
		FS_UNLOCK(fs);
		/*
		 * add back in counts associated with the new frags
		 */
		blk = blkmap(fs, cg_blksfree(cgp), bbase);
		fragacct(fs, blk, cgp->cg_frsum, 1);
		/*
		 * if a complete block has been reassembled, account for it
		 */
		if (isblock(fs, cg_blksfree(cgp),
		    (daddr_t)fragstoblks(fs, bbase))) {
			cgp->cg_cs.cs_nffree -= fs->fs_frag;
			FS_LOCK(fs);
			fs->fs_cstotal.cs_nffree -= fs->fs_frag;
			fs->fs_cs(fs, cg).cs_nffree -= fs->fs_frag;
			cgp->cg_cs.cs_nbfree++;
			fs->fs_cstotal.cs_nbfree++;
			fs->fs_cs(fs, cg).cs_nbfree++;
			FS_UNLOCK(fs);
			i = cbtocylno(fs, bbase);
			cg_blks(fs, cgp, i)[cbtorpos(fs, bbase)]++;
			cg_blktot(cgp)[i]++;
		}
	}
	FS_LOCK(fs);
	fs->fs_fmod++;
	FS_UNLOCK(fs);
	bdwrite(bp, bp->b_vp);
}

/*
 * Free an inode.
 *
 * The specified inode is placed back in the free map.
 *
 * Caller need not hold any lock on the inode but may need
 * other synchronization depending on how the inode has been used.
 */
ifree(ip, ino, mode)
	struct inode *ip;
	ino_t ino;
	int mode;
{
	register struct fs *fs;
	register struct cg *cgp;
	struct buf *bp;
	int error, cg, s;

	LASSERT(!FS_LOCK_HOLDER(ip->i_fs));
	fs = ip->i_fs;
	if ((unsigned)ino >= fs->fs_ipg*fs->fs_ncg) {
		printf("dev = 0x%x, ino = %d, fs = %s\n",
		    ip->i_dev, ino, fs->fs_fsmnt);
		panic("ifree: range");
	}
	cg = itog(fs, ino);
	error = bread(ip->i_devvp, fsbtodb(fs, cgtod(fs, cg)),
		(int)fs->fs_cgsize, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return;
	}
	cgp = bp->b_un.b_cg;
	if (!cg_chkmagic(cgp)) {
		brelse(bp);
		return;
	}
	s = splhigh();
	TIME_READ_LOCK();
	cgp->cg_time = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	ino %= fs->fs_ipg;
	if (isclr(cg_inosused(cgp), ino)) {
		printf("ifree: freeing free inode\n");
		printf("dev = 0x%x, ino = %d, fs = %s\n",
		    ip->i_dev, ino, fs->fs_fsmnt);
#ifndef	multimax
		panic("ifree: freeing free inode");
#endif
		brelse(bp);
		return;
	}
	clrbit(cg_inosused(cgp), ino);
	if (ino < cgp->cg_irotor)
		cgp->cg_irotor = ino;
	cgp->cg_cs.cs_nifree++;
	FS_LOCK(fs);
	fs->fs_cstotal.cs_nifree++;
	fs->fs_cs(fs, cg).cs_nifree++;
	if ((mode & IFMT) == IFDIR) {
		cgp->cg_cs.cs_ndir--;
		fs->fs_cstotal.cs_ndir--;
		fs->fs_cs(fs, cg).cs_ndir--;
	}
	fs->fs_fmod++;
	FS_UNLOCK(fs);
	bdwrite(bp, bp->b_vp);
}

/*
 * Find a block of the specified size in the specified cylinder group.
 *
 * It is a panic if a request is made to find a block if none are
 * available.
 *
 * Caller holds cylinder group's buffer's lock.
 */
daddr_t
mapsearch(fs, cgp, bpref, allocsiz)
	register struct fs *fs;
	register struct cg *cgp;
	daddr_t bpref;
	int allocsiz;
{
	daddr_t bno;
	int start, len, loc, i;
	int blk, field, subfield, pos;

	LASSERT(!FS_LOCK_HOLDER(fs));
	/*
	 * find the fragment by searching through the free block
	 * map for an appropriate bit pattern
	 */
	if (bpref)
		start = dtogd(fs, bpref) / NBBY;
	else
		start = cgp->cg_frotor / NBBY;
	len = howmany(fs->fs_fpg, NBBY) - start;
	loc = scanc((unsigned)len, (u_char *)&cg_blksfree(cgp)[start],
		(u_char *)fragtbl[fs->fs_frag],
		(u_char)(1 << (allocsiz - 1 + (fs->fs_frag % NBBY))));
	if (loc == 0) {
		len = start + 1;
		start = 0;
		loc = scanc((unsigned)len, (u_char *)&cg_blksfree(cgp)[0],
			(u_char *)fragtbl[fs->fs_frag],
			(u_char)(1 << (allocsiz - 1 + (fs->fs_frag % NBBY))));
		if (loc == 0) {
			printf("start = %d, len = %d, fs = %s\n",
			    start, len, fs->fs_fsmnt);
			panic("alloccg: map corrupted");
			/* NOTREACHED */
		}
	}
	bno = (start + len - loc) * NBBY;
	cgp->cg_frotor = bno;
	/*
	 * found the byte in the map
	 * sift through the bits to find the selected frag
	 */
	for (i = bno + NBBY; bno < i; bno += fs->fs_frag) {
		blk = blkmap(fs, cg_blksfree(cgp), bno);
		blk <<= 1;
		field = around[allocsiz];
		subfield = inside[allocsiz];
		for (pos = 0; pos <= fs->fs_frag - allocsiz; pos++) {
			if ((blk & field) == subfield)
				return (bno + pos);
			field <<= 1;
			subfield <<= 1;
		}
	}
	printf("bno = %d, fs = %s\n", bno, fs->fs_fsmnt);
	panic("alloccg: block not in map");
	return (-1);
}

/*
 * Check that a specified block number is in range.
 */
badblock(fs, bn)
	register struct fs *fs;
	daddr_t bn;
{

	if ((unsigned)bn >= fs->fs_size) {
		printf("bad block %d, ", bn);
		fserr(fs, "bad block");
		return (1);
	}
	return (0);
}

/*
 * Fserr prints the name of a file system with an error diagnostic.
 *
 * The form of the error message is:
 *	fs: error message
 */
fserr(fs, cp)
	struct fs *fs;
	char *cp;
{

	log(LOG_ERR, "%s: %s\n", fs->fs_fsmnt, cp);
}
