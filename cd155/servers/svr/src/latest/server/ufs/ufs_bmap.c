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
 * $Log: ufs_bmap.c,v $
 * Revision 1.12  1994/11/18  20:45:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/06/28  23:10:24  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.10  1994/02/17  16:58:51  brad
 * Merged revision 1.9.2.1 from the R1.2 branch.
 *
 * Revision 1.9.2.1  1994/02/16  04:19:49  brad
 * Fixed flawed implementation of disk block preallocation.  Only preallocate
 * full file system blocks for simplicity.  Handle i_resfrags field in
 * the inode correctly.  Several errors in ufs_prealloc() fixed.
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
 * Revision 1.9  1993/09/23  23:35:18  cfj
 * Merge R1.1 bug fix.
 *
 * Revision 1.8.6.1  1993/09/23  23:33:50  cfj
 * Use the buffer cache funnel to restrict the number of threads contenting
 * for buffer cache blocks.
 *
 * Revision 1.8  1993/07/14  18:37:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:53:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/27  02:00:21  brad
 * Removed temporary code that allowed PFS files to be cached in the buffer
 * cache ... PFS now uses Fast Path exclusively.
 *
 * Revision 1.6  1993/05/06  20:30:37  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:49:19  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:10:32  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 2.16  93/10/20  15:30:59  dnoveck
 *      DEV_BSIZE elimination: Change interface to {vio,data}_{read,write}
 *      to be in terms of disk granules.
 *
 * Revision 2.15  93/05/13  16:46:04  roy
 * 	balloc_nbc() and balloc_extend_nbc() changed to support new
 * 	size handling logic.  Also, when reserving direct blocks always
 * 	reserve the entire block.
 * 	[93/05/04            roy]
 * 
 * Revision 2.14  93/03/30  16:10:16  roy
 * 	Added VIO_IS_FASTPATH support to balloc_nbc.
 * 	[93/03/10            roy]
 *
 * Revision 2.13  93/03/12  12:55:54  rabii
 * 	Fixed balloc_nbc so write_size is always set when new_alloc is 
 * 	TRUE (rabii)
 *
 * Revision 1.4  1992/12/11  03:03:52  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.2.2.5  1993/03/10  05:25:50  brad
 * Added bug fix from OSF ad1.0.2.
 *
 * Revision 1.1.2.2.2.4  1993/02/12  22:39:19  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.2.2.3  1993/02/09  21:43:23  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.1.2.2.2.2  1992/12/16  06:03:58  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.2.2.1  1992/11/25  23:14:29  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.3  1992/11/30  22:50:46  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:33:00  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:39:15  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.12  92/11/18  10:40:44  loverso
 * 	Remove assert.
 * 
 * Revision 2.11  92/11/17  19:52:51  loverso
 * 	Make balloc_nbc() try to extend frags at reservation time, not
 * 	pageout time.  (mmp)
 * 
 * Revision 2.10  92/11/02  16:09:11  mmp
 * 	When extending frags in balloc_nbc() and balloc_extend_nbc(),
 * 	write out the zeroes for newly-allocated space in the right
 * 	place.  (mmp)
 * 
 * Revision 2.9  92/09/20  11:25:40  roy
 * 	Fix case for re-reserving frags in balloc_nbc.
 * 	[92/09/16            roy]
 * 
 * Revision 2.8  92/09/11  09:28:07  rabii
 * 	Add debug counters under UFS_NBC_DEBUG.
 * 	[92/08/28            roy]
 * 
 * Revision 2.7  92/08/26  12:12:19  loverso
 * 	When new blocks are allocated, balloc_nbc only sets ICHG flag in
 * 	inode.  It used to also set IUPD but now modifications to file data
 * 	are tracked at a higher layer.
 * 	[92/08/21            roy]
 * 
 * 	Modify bmap to handle case of reserved daddr's (B_RESERVE).  
 * 		Involved significant mods to balloc_nbc especially.
 * 	Removed MAY_USE_BUFCACHE in favor of VIO_IS_BUF.
 * 	[92/07/22            roy]
 * 
 * Revision 2.6  92/07/14  14:53:31  rabii
 * 	Modified calling sequence to data_read and data_write.
 * 	[92/07/10            roy]
 * 
 * Revision 2.5  92/05/31  18:59:07  loverso
 * 	Implemented balloc_extend_nbc for UFS_NBC.
 * 	[92/05/27            roy]
 * 
 * Revision 2.4  92/03/15  14:41:10  roy
 * 	92/03/03  16:58:49  roy
 * 	Added MAY_USE_BUFCACHE assert.
 * 
 * Revision 2.3  91/12/10  21:29:54  roy
 * 	91/10/14  20:58:44  roy
 * 	Added balloc_nbc code.
 * 
 * Revision 2.2  91/08/31  14:19:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:00:23  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  14:58:58  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:52:45  gm]
 * 
 * Revision 1.5  90/06/22  20:55:33  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Removed two unneeded bmap parameters.		gmf@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Integrated MP code from Encore Mach/0.6:	gmf@osf.org
 * 	  Merged with MACH_NBC (don't expect it to work).
 * 	[90/06/12  21:41:24  gmf]
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
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)ufs_bmap.c	7.7 (Berkeley) 1/4/90
 */
#if	MACH
#include <mach_nbc.h>
#endif
#include <ufs_nbc.h>
#include <ufs_nbc_debug.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <ufs/fs.h>

/*
 * Bmap defines the structure of file system storage
 * by returning the physical block number on a device
 * given the inode and the logical block number in a file.
 */
bmap(ip, bn, bnp)
	register struct inode *ip;
	register daddr_t bn;
	daddr_t	*bnp;
{
	register struct fs *fs;
	register daddr_t nb;
	struct buf *bp;
	daddr_t *bap;
	int i, j, sh;
	int error;
	long indir_size;
	long bread_size;

	if (bn < 0) 
		return (EFBIG);
	fs = ip->i_fs;
	indir_size = SZINDIR(fs);

	/*
	 * The first NDADDR blocks are direct blocks
	 */
	if (bn < NDADDR) {
		BM(IN_LOCK(ip));
		nb = ip->i_db[bn];
		BM(IN_UNLOCK(ip));
		if (nb == 0 || IS_RESERVED(nb)) 
			*bnp = (daddr_t)-1;
		else
			*bnp = fsbtodb(fs, nb);
		return (0);
	}
	/*
	 * Determine the number of levels of indirection.
	 */
	sh = 1;
	bn -= NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(fs);
		if (bn < sh)
			break;
		bn -= sh;
	}
	if (j == 0) 
		return (EFBIG);
	/*
	 * Fetch through the indirect blocks.
	 */
	bread_size = indir_size;
	BM(IN_LOCK(ip));
	nb = ip->i_ib[NIADDR - j];
	BM(IN_UNLOCK(ip));
	if (nb == 0) {
		*bnp = (daddr_t)-1;
		return (0);
	}
	for (; j <= NIADDR; j++) {

		sh /= NINDIR(fs);
		i = (bn / sh) % NINDIR(fs);

		/*
		 * We have to be careful here.  If the buffer block
		 * size is smaller than indir_size, we will have to
		 * bread the relevant segment with bcache_maxbsize.
		 */
		if (indir_size > bcache_maxbsize) {

			int section;

			bread_size = bcache_maxbsize;
			section = i >> ufs_bcmax_nishift;

			nb += section << (bcache_maxbshift - fs->fs_fshift);
			i &= (ufs_bcmax_nisize-1);
		}

		if (error = bread(ip->i_devvp, fsbtodb(fs, nb),
		    (int)bread_size, NOCRED, &bp)) {
			brelse(bp);
			return (error);
		}
		bap = bp->b_un.b_daddr;
		nb = bap[i];
		if (nb == 0) {
			*bnp = (daddr_t)-1;
			brelse(bp);
			return (0);
		}
		brelse(bp);
	}
	if (IS_RESERVED(nb)) 
		*bnp = (daddr_t)-1;   
	else
		*bnp = fsbtodb(fs, nb);
	return (0);
}

/*
 * Balloc defines the structure of file system storage
 * by allocating the physical blocks on a device given
 * the inode and the logical block number in a file.
 */
balloc(ip, bn, relios, size, bpp, flags)
	register struct inode *ip;
	register daddr_t bn;
        daddr_t relios;
#if	MACH
	u_long size;
#else
	int size;
#endif
	struct buf **bpp;
	int flags;
{
	register struct fs *fs;
	register daddr_t nb;
	struct buf *bp, *nbp;
	struct vnode *vp = ITOV(ip);
	u_long osize, nsize;
	int i, j, sh, error;
	daddr_t newb, lbn, *bap, pref, blkpref();
	long indir_size;

#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(vp));
#endif
	LASSERT(IN_WRITE_HOLDER(ip));
	*bpp = (struct buf *)0;
	if (bn < 0)
		return (EFBIG);
	fs = ip->i_fs;
	indir_size = SZINDIR(fs);

	/*
	 * If the next write will extend the file into a new block,
	 * and the file is currently composed of a fragment
	 * this fragment has to be extended to be a full block.
	 */
	BM(IN_LOCK(ip));
	nb = lblkno(fs, ip->i_size);
	BM(IN_UNLOCK(ip));
	if (nb < NDADDR && nb < bn) {
		osize = blksize(fs, ip, nb);
		if (osize < fs->fs_bsize && osize > 0) {
			ASSERT(relios == 0);
			error = realloccg(ip, nb,
				blkpref(ip, nb, (int)nb, &ip->i_db[0]),
				osize, (int)fs->fs_bsize, &bp);
			if (error)
				return (error);
			IN_LOCK(ip);
			ip->i_size = (nb + 1) * fs->fs_bsize;
			ip->i_db[nb] = dbtofsb(fs, bp->b_blkno);
			ip->i_flag |= IUPD|ICHG;
			IN_UNLOCK(ip);
			if (flags & B_SYNC)
				bwrite(bp);
			else
				bawrite(bp);
#if	MACH_NBC
			if (flush)
				ino_flush(ip, nb * fs->fs_bsize, fs->fs_bsize);
#endif
		}
	}
	/*
	 * The first NDADDR blocks are direct blocks
	 */
	if (bn < NDADDR) {
		BM(IN_LOCK(ip));
		nb = ip->i_db[bn];
		if (nb != 0 && ip->i_size >= (bn + 1) * fs->fs_bsize) {
			BM(IN_UNLOCK(ip));
			error = bread(vp, blktoiosec(fs, bn) + relios,
				      ioseclen(fs), NOCRED, &bp);
			if (error) {
				brelse(bp);
				return (error);
			}
			*bpp = bp;
			return (0);
		} else
			BM(IN_UNLOCK(ip));
		if (nb != 0) {
			/*
			 * Consider need to reallocate a fragment.
			 */
			BM(IN_LOCK(ip));
			osize = fragroundup(fs, blkoff(fs, ip->i_size));
			BM(IN_UNLOCK(ip));
			nsize = fragroundup(fs, size) +
				iosectosize(fs, relios);
			if (nsize <= osize) {
				osize = iosecoff(fs, osize);
				if (osize == 0)
					osize = ioseclen(fs);
				error = bread(vp, blktoiosec(fs, bn) + relios,
					      osize, NOCRED, &bp);
				if (error) {
					brelse(bp);
					return (error);
				}
			} else {
				error = realloccg(ip, bn,
					blkpref(ip, bn, (int)bn, &ip->i_db[0]),
					osize, nsize, &bp);
				if (error)
					return (error);
			}
		} else {
			BM(IN_LOCK(ip));
			if (ip->i_size < (bn + 1) * fs->fs_bsize)
				nsize = fragroundup(fs, size);
			else
				nsize = fs->fs_bsize;
			BM(IN_UNLOCK(ip));
			error = alloc(ip, bn,
				blkpref(ip, bn, (int)bn, &ip->i_db[0]),
				nsize, &newb);
			if (error)
				return (error);
			nsize = iosecoff(fs, nsize);
			if (nsize == 0)
				nsize = ioseclen(fs);
			bp = getblk(vp, blktoiosec(fs, bn) + relios, nsize);
			bp->b_blkno = fsbtodb(fs, newb);
			if (flags & B_CLRBUF)
				clrbuf(bp);
			IN_LOCK(ip);
			ip->i_db[bn] = dbtofsb(fs, bp->b_blkno);
			ip->i_flag |= IUPD|ICHG;
			IN_UNLOCK(ip);
		}
		*bpp = bp;
		return (0);
	}
	/*
	 * Determine the number of levels of indirection.
	 */
	pref = 0;
	sh = 1;
	lbn = bn;
	bn -= NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(fs);
		if (bn < sh)
			break;
		bn -= sh;
	}
	if (j == 0)
		return (EFBIG);
	/*
	 * Fetch the first indirect block allocating if necessary.
	 */
	BM(IN_LOCK(ip));
	nb = ip->i_ib[NIADDR - j];
	BM(IN_UNLOCK(ip));
	if (nb == 0) {
		pref = blkpref(ip, lbn, 0, (daddr_t *)0);
		if (error = alloc(ip, lbn, pref, (int)indir_size, &newb))
			return (error);
		nb = newb;
		if (error = bwrite_indirect_block(ip, nb, indir_size)){
			blkfree(ip, nb, indir_size);
			return(error);
		}
		IN_LOCK(ip);
		ip->i_ib[NIADDR - j] = nb;
		ip->i_flag |= IUPD|ICHG;
		IN_UNLOCK(ip);
	}
	/*
	 * Fetch through the indirect blocks, allocating as necessary.
	 */
	for (; ; j++) {
		int bread_size;

		sh /= NINDIR(fs);
		i = (bn / sh) % NINDIR(fs);
		bread_size = indir_size;

		/*
		 * Indirect block size maybe smaller than the buffer
		 * block size.  Find the fragment where the bn is
		 * located if the case.
		 */
		if (indir_size > bcache_maxbsize) {

			int section;

			bread_size = bcache_maxbsize;
			section = i >> ufs_bcmax_nishift;

			nb += section << (bcache_maxbshift - fs->fs_fshift);
			i &= (ufs_bcmax_nisize-1);
		}

		error = bread(ip->i_devvp, fsbtodb(fs, nb),
		    (int)bread_size, NOCRED, &bp);
		if (error) {
			brelse(bp);
			return (error);
		}
		bap = bp->b_un.b_daddr;
		nb = bap[i];
		if (j == NIADDR)
			break;
		if (nb != 0) {
			brelse(bp);
			continue;
		}
		if (pref == 0)
			pref = blkpref(ip, lbn, 0, (daddr_t *)0);
		if (error = alloc(ip, lbn, pref, (int)indir_size, &newb)) {
			brelse(bp);
			return (error);
		}
		nb = newb;
		if (error = bwrite_indirect_block(ip, nb, indir_size)){
			blkfree(ip, nb, indir_size);
			return(error);
		}
		bap[i] = nb;
		/*
		 * If required, write synchronously, otherwise use
		 * delayed write. If this is the first instance of
		 * the delayed write, reassociate the buffer with the
		 * file so it will be written if the file is sync'ed.
		 */
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp, vp);
/*
 * what do we do here?
 */
#if	MACH_NBC
		if (flush)
			ino_flush(ip, lbn*fs->fs_bsize, fs->fs_bsize);
#endif
	}
	/*
	 * Get the data block, allocating if necessary.
	 */
	if (nb == 0) {
		pref = blkpref(ip, lbn, i, &bap[0]);
		if (error = alloc(ip, lbn, pref, (int)fs->fs_bsize, &newb)) {
			brelse(bp);
			return (error);
		}
		nb = newb;
		nbp = getblk(vp, lbn, fs->fs_bsize);
		nbp->b_blkno = fsbtodb(fs, nb);
		if (flags & B_CLRBUF)
			clrbuf(nbp);
		bap[i] = nb;
		/*
		 * If required, write synchronously, otherwise use
		 * delayed write. If this is the first instance of
		 * the delayed write, reassociate the buffer with the
		 * file so it will be written if the file is sync'ed.
		 */
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp, vp);
		*bpp = nbp;
		return (0);
	}
	brelse(bp);
	if (flags & B_CLRBUF) {
		error = bread(vp, lbn, (int)fs->fs_bsize, NOCRED, &nbp);
		if (error) {
			brelse(nbp);
			return (error);
		}
	} else {
		nbp = getblk(vp, lbn, fs->fs_bsize);
		nbp->b_blkno = fsbtodb(fs, nb);
	}
	*bpp = nbp;
	return (0);
}


#if 	UFS_NBC

#if	UFS_NBC_DEBUG
#define	debug_incr_counter(x)	(x)++

int			ufs_balloc_get_zero_buf = 0;     /* stats */
int			ufs_balloc_extend = 0;
int			ufs_balloc_extend1_realloc = 0;
int			ufs_balloc_extend1_get_zero = 0;
int			ufs_balloc_extend1_data_write = 0;
int			ufs_balloc_extend2_realloc = 0;
int			ufs_balloc_extend2_alloc = 0;
int			ufs_balloc_extend2_get_zero = 0;
int			ufs_balloc_extend2_data_write = 0;
int			ufs_balloc_nbc_get_zero = 0;
int			ufs_balloc_nbc_data_write = 0;

#else	/* UFS_NBC_DEBUG */
#define	debug_incr_counter(x)	
#endif  /* UFS_NBC_DEBUG */

/*
 * Routine to allocate a zero'd buffer.
 * XXX We would like to use statically allocated buffer, and we should
 * once the data_write interface can be told not to deallocate the buffer.
 */
vm_address_t
balloc_get_zero_buf(size)
	u_long		size;
{
	vm_address_t	tempbuf;
	kern_return_t	ret;

	debug_incr_counter(ufs_balloc_get_zero_buf);
	if ((ret = vm_allocate(mach_task_self(), &tempbuf, size, TRUE)) 
	    != KERN_SUCCESS)
		panic("balloc_get_zero_buf: can't");
	bzero((char *) tempbuf, size);
	return(tempbuf);
}
	    
/*
 * Extend direct blocks as necessary.  This routine should only be called 
 * when growing a file.  It assumes that any I/O's that result from extending
 * blocks need to synchronize with other I/O's in progress.
 */
balloc_extend_nbc(ip, size, flags)
	register struct inode 	*ip;
	u_long 			size;  
	int			flags;
{
	register struct fs 	*fs;
	u_long 			osize, nsize, n2size, cursize;
	daddr_t 		nb, lastlbn, lbn, blkpref();
	vm_address_t 		buffer;
	int 			numfrag, ressize;
	struct vnode		*vp = ITOV(ip);
	int			error;

	ASSERT(VIO_IS_MAPPED(vp) || VIO_IS_FASTPATH(vp));
	LASSERT(IN_WRITE_HOLDER(ip));
	fs = ip->i_fs;

	lbn = lblkno(fs, size);
	if (lbn < 0)
		return (EFBIG);

	IN_LOCK(ip);
	if (VIO_IS_MAPPED(vp)) 
		cursize = ip->i_writesize;
	else
		cursize = ip->i_size;
	IN_UNLOCK(ip);
	ASSERT(size >= cursize);
	
	/*
	 * Determine the size of the current last block, and what the new
	 * size should be after the file is extended.  Then, grow it if nec.
	 */
 	lastlbn = lblkno(fs, cursize);
	osize = blksize2(fs, cursize, lastlbn);
	nsize = blksize2(fs, size, lastlbn);/* new size of current last block */
	n2size = blksize2(fs, size, lbn);   /* new size of new last block */

	debug_incr_counter(ufs_balloc_extend);

	if (nsize > osize && osize > 0) {
		nb = ip->i_db[lastlbn];
		ASSERT(!IS_RESERVED(nb));
		numfrag = numfrags(fs, nsize - osize);
		if (ip->i_resfrags[lastlbn] >= numfrag) {
			/* frags already reserved */
			ip->i_resfrags[lastlbn] -= numfrag;
			buffer = NULL;
		} else {
			debug_incr_counter(ufs_balloc_extend1_realloc);
			ressize = ip->i_resfrags[lastlbn] << fs->fs_fshift;
			error = realloccg_nbc(ip, lastlbn, blkpref(ip, lastlbn, 
								 (int)lastlbn,
								 &ip->i_db[0]),
					      osize+ressize, nsize, TRUE,
					      &nb, &buffer);
			if (error)
				return (error);
			ip->i_resfrags[lastlbn] = 0;
			IN_LOCK(ip);
			ip->i_db[lastlbn] = nb;
			ip->i_flag |= ICHG;
			IN_UNLOCK(ip);
		}

		nb = fsbtodb(fs, nb);	/* convert frag addr to sector */
		if (buffer == NULL) {
			debug_incr_counter(ufs_balloc_extend1_get_zero);
			/* newly allocated space must be zero'd */
			nb += btodg(osize);
			nsize = nsize - osize;
			buffer = balloc_get_zero_buf(nsize);  
		}

		debug_incr_counter(ufs_balloc_extend1_data_write);
		if (VIO_IS_FASTPATH(vp)) {
			(void) vio_write(vp, fs->fs_devinfo, buffer, nsize,
					 lastlbn, 1, nb, btodg(nsize),
					 TRUE,
					 (flags & B_SYNC) ? TRUE : FALSE);
		} else
			(void) data_write(fs->fs_devinfo, nb,
					  buffer, btodg(nsize),
					  (flags & B_SYNC) ? TRUE : FALSE);
	}

	/*
	 * If there's a new last block, and it's a direct block, then allocate
	 * space as necessary.  We have to do this because some code assumes
	 * space is allocated.
	 */
	if (lbn < NDADDR && n2size > 0 && (lbn > lastlbn ||
					   (lbn == lastlbn && osize == 0))) {
		ASSERT(ip->i_db[lbn] == 0 || IS_RESERVED(ip->i_db[lbn]));
		if (IS_RESERVED(ip->i_db[lbn])) {
			/*
			 * The block is already reserved.
			 */
			ASSERT(ip->i_resfrags[lbn] << fs->fs_fshift == 
			       fs->fs_bsize);
			numfrag = numfrags(fs, n2size);
			ip->i_resfrags[lbn] -= numfrag;
			nb = DADDR(ip->i_db[lbn]);
		} else {
			debug_incr_counter(ufs_balloc_extend2_alloc);
			error = alloc(ip, lbn,
				      blkpref(ip, lbn, (int)lbn, &ip->i_db[0]),
				      n2size, &nb);
			if (error)
				return (error);
		}
		IN_LOCK(ip);
		ip->i_db[lbn] = nb;
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
		/* newly allocated space must be zero'd */
		debug_incr_counter(ufs_balloc_extend2_get_zero);
		buffer = balloc_get_zero_buf(n2size);  

		debug_incr_counter(ufs_balloc_extend2_data_write);
		if (VIO_IS_FASTPATH(vp)) {
			(void) vio_write(vp, fs->fs_devinfo, buffer, n2size,
					 lbn, 1, 
					 fsbtodb(fs, nb), btodg(n2size),
					 TRUE, (flags & B_SYNC) ? TRUE : FALSE);
		} else
			(void) data_write(fs->fs_devinfo, fsbtodb(fs, nb),
					  buffer, btodg(n2size),
					  (flags & B_SYNC) ? TRUE : FALSE);
	}
	
	return (0);
}

/*
 * Balloc_nbc defines the structure of file system storage
 * by allocating the physical blocks on a device given
 * the inode and the logical block number in a file.
 *
 * balloc_nbc differs from balloc in that it returns a physical
 * block number instead of a struct buf.  Also, if new_alloc is
 * returned TRUE then new disk space of size write_size was allocated.
 *
 * It's assumed that a higher layer will write write_size bytes to the
 * the new space (either in the form of real data or zeroes),
 * starting at write_indx within the logical block. If
 * newbuf != NULL (only possible if new_alloc==TRUE) then newbuf
 * represents write_size bytes of data that must be written to the
 * new space (i.e., frag reallocation is causing data to be "moved").
 * Note that the actual size of newbuf is rounded up to a page size
 * multiple.  Also, if newbuf != NULL, then write_indx is guaranteed 
 * to be 0.
 *
 * balloc_nbc also supports block reservation, indicated by B_RESERVE in
 * the flags arg.  Logical block numbers < NDADDR may be either fully or
 * partially reserved.  Blocks > NDADDR may only be fully reserved.
 *
 * The 'synchronize' parameter specifies whether it's necessary to synchronize
 * with other I/O's in progress.  A value of FALSE indicates that the caller
 * has taken care of synchronization.
 */
balloc_nbc(ip, lbn, size, synchronize, fsize,
	   bnp, new_alloc, write_indx, write_size, newbuf, flags)
	register struct inode 	*ip;
	register daddr_t 	lbn;
	u_long 			size;  
	boolean_t		synchronize;
	u_long			fsize;
	daddr_t			*bnp;		/* out */
	boolean_t 		*new_alloc;	/* out */
	u_long 			*write_indx;	/* out */
	u_long 			*write_size;	/* out */
	vm_address_t 		*newbuf;	/* out */
	int			flags;		/* out */
{
	register struct fs 	*fs;
	struct buf 		*bp, *nbp;
	struct vnode 		*vp = ITOV(ip);
	u_long 			isize, osize, nsize;
	int 			numfrag, ressize;
	int 			i, j, sh, error = ESUCCESS;
	daddr_t 		nb, lastlbn, xlbn, *bap, pref, blkpref();
	vm_address_t 		buffer;
	boolean_t		do_wakeup = FALSE;
	long			indir_size;

	ASSERT(VIO_IS_MAPPED(vp) || VIO_IS_FASTPATH(vp) || VIO_IS_PAGING(vp));
	LASSERT(IN_WRITE_HOLDER(ip));
	*bnp = (daddr_t) 0;
	if (lbn < 0)
		return (EFBIG);
	fs = ip->i_fs;
	indir_size = SZINDIR(fs);

	*new_alloc = FALSE;	/* was new disk space allocated? */
	*newbuf = NULL;		/* buffer returned that must be written */
	*write_indx = 0;	/* index in lbn to write if new_alloc==TRUE */

	/* 	 
	 * If the next write will extend the file into a new block, 	 
	 * and the file is currently composed of a fragment 	 
	 * this fragment has to be extended to be a full block.  
	 *
	 * This needs to be done at reservation time so that pageouts
	 * don't fail trying to extend the previous block.  Also, 
	 * since any I/O's occurring while extending the block were 
	 * not anticipated by the caller, the 'synchronize' argument
	 * to realloccg_nbc must be TRUE.
	 */ 	
	BM(IN_LOCK(ip));
	if (VIO_IS_MAPPED(vp)) 
		isize = ip->i_writesize;
	else
		isize = MAX(fsize, ip->i_size);
   	BM(IN_UNLOCK(ip));
	lastlbn = lblkno(fs, isize);
 	if (lastlbn < NDADDR && lastlbn < lbn) {
 		osize = fragroundup(fs, blkoff(fs, isize));
 		if (osize < fs->fs_bsize && osize > 0) {
			ASSERT(!IS_RESERVED(ip->i_db[lastlbn]));
			numfrag = numfrags(fs, fs->fs_bsize - osize);
			if (ip->i_resfrags[lastlbn] >= numfrag) {
				/* frags already reserved */
				ip->i_resfrags[lastlbn] -= numfrag;
				ASSERT(ip->i_resfrags[lastlbn] == 0);
				buffer = NULL;
				nb = ip->i_db[lastlbn];  /* needed below */
			} else {
				ressize = ip->i_resfrags[lastlbn] << 
					fs->fs_fshift;
				error = realloccg_nbc(ip, lastlbn,
						      blkpref(ip, lastlbn, 
							      (int)lastlbn,
							      &ip->i_db[0]),
						      osize+ressize, 
						      (int)fs->fs_bsize, TRUE,
						      &nb, &buffer);
				if (error)
					return (error);
				ip->i_resfrags[lastlbn] = 0;
			}
 			IN_LOCK(ip);
			isize = (lastlbn + 1) * fs->fs_bsize;
			if (VIO_IS_MAPPED(vp))
				ip->i_writesize = isize;
			else
				ip->i_size = isize;
 			ip->i_db[lastlbn] = nb;
 			ip->i_flag |= ICHG;
 			IN_UNLOCK(ip);

			nb = fsbtodb(fs, nb);  /* convert frag addr to gran */
			if (buffer == NULL) {
				debug_incr_counter(ufs_balloc_nbc_get_zero);
				/* newly allocated space must be zero'd */
				nb += btodg(osize);
				osize = fs->fs_bsize - osize;
				buffer = balloc_get_zero_buf(osize);
			} else {
				osize = fs->fs_bsize;
			}

			/* note that data_write consumes the buffer */
			debug_incr_counter(ufs_balloc_nbc_data_write);
			if (VIO_IS_FASTPATH(vp)) {
				(void) vio_write(vp, fs->fs_devinfo, buffer, 
					      osize, lastlbn, 1, nb, 
					      btodg(osize),
					      TRUE,
					      (flags & B_SYNC) ? TRUE : FALSE);
			} else
				(void) data_write(fs->fs_devinfo, nb, 
						  buffer, btodg(osize),
						  (flags & B_SYNC) ? 
						  TRUE : FALSE);
		}
 	}

 	/*
 	 * The first NDADDR blocks are direct blocks
 	 */
	if (lbn < NDADDR) {
		BM(IN_LOCK(ip));
		nb = ip->i_db[lbn];
		if (nb != 0 && isize >= (lbn + 1) * fs->fs_bsize) {
			BM(IN_UNLOCK(ip));
			if (IS_RESERVED(nb)) {
#ifdef	PFS
				ASSERT((ip->i_flags & IC_PREALLOCATED) ?
				       ((ip->i_resfrags[lbn] == 0) ||
					(ip->i_resfrags[lbn] == fs->fs_frag)) :
				       (ip->i_resfrags[lbn] == fs->fs_frag));
#else
				ASSERT(ip->i_resfrags[lbn] == fs->fs_frag);
#endif
				*new_alloc = TRUE;
#ifdef	PFS
				if ((flags & B_RESERVE) ||
				    (flags & B_PREALLOC)) {
#else
				if (flags & B_RESERVE) {
#endif
					*write_size = 0;
					*bnp = fsbtodb(fs, DADDR(nb));
					return (0);
				} else {
					ip->i_resfrags[lbn] = 0;
					*write_size = fs->fs_bsize;  
					nb = DADDR(nb);
				}
				IN_LOCK(ip);
				ip->i_db[lbn] = nb;
				ip->i_flag |= ICHG;
				IN_UNLOCK(ip);
			}
			*bnp = fsbtodb(fs, DADDR(nb));
			return (0);
		} else
			BM(IN_UNLOCK(ip));

		if (IS_RESERVED(nb)) {
#ifdef	PFS
			ASSERT((ip->i_flags & IC_PREALLOCATED) ?
			       ((ip->i_resfrags[lbn] == 0) ||
				(ip->i_resfrags[lbn] == fs->fs_frag)) :
			       (ip->i_resfrags[lbn] == fs->fs_frag));
#else
			ASSERT(ip->i_resfrags[lbn] == fs->fs_frag);
#endif
			nsize = fragroundup(fs, size);
			*new_alloc = TRUE;
#ifdef	PFS
			if ((flags & B_RESERVE) || (flags & B_PREALLOC)) {
#else
			if (flags & B_RESERVE) {
#endif
				/* 
				 * It's possible that we're trying to reserve
				 * already-reserved direct blocks.  The reason
				 * is that a previous reserve operation 
				 * would've reserved the whole block.
				 */
				*write_size = 0;
				/* setting *bnp not strictly needed */
				*bnp = fsbtodb(fs, DADDR(nb));
				return (0);
			} else {
				nb = DADDR(nb);
				ip->i_resfrags[lbn] = numfrags(fs, 
							 fs->fs_bsize - nsize);
				*write_size = nsize;
			} 
		} else if (nb != 0) {
			/*
			 * Consider need to reallocate a fragment. 
			 */
			BM(IN_LOCK(ip));
			osize = fragroundup(fs, blkoff(fs, isize));
			BM(IN_UNLOCK(ip));
			nsize = fragroundup(fs, size);

			if (nsize <= osize) {
				*bnp = fsbtodb(fs, nb);
				return (0);
			} else {
				/*
				 * Check if we have any/enough reserved blocks
				 * to fulfill the request.
				 */
				*new_alloc = TRUE;  
				numfrag = numfrags(fs, nsize - osize);
				if (ip->i_resfrags[lbn] >= numfrag) {
					/* frags already reserved */
					if (flags & B_RESERVE) 
						/*
						 * Could be re-reserving if,
						 * for instance, a page with
						 * reserved frags backing it
						 * was paged out and then paged
						 * back in.
						 */
						*write_size = 0;
					else {
						ip->i_resfrags[lbn] -= numfrag;
						*write_indx = osize;
						*write_size = nsize - osize;
					}
					*bnp = fsbtodb(fs, nb);
					return (0);
				} else {
					ressize = ip->i_resfrags[lbn] << 
						fs->fs_fshift;
					error = realloccg_nbc(ip, lbn,
							      blkpref(ip, lbn, 
								  (int)lbn,
								  &ip->i_db[0]),
							      osize+ressize, 
							      nsize, 
							      synchronize,
							      &nb, newbuf);
					if (error)
						return (error);
					if (flags & B_RESERVE) {
						ip->i_resfrags[lbn] = numfrag;
						if (*newbuf) 
							*write_size = nsize;
						else
							*write_size = 0;
					} else {
						ip->i_resfrags[lbn] = 0;
						/* 
						 * If frags didn't move to a new
						 * block, then only need to
						 * write the new frags.
						 */
						if (*newbuf) 
							*write_size = nsize;
						else {
							*write_indx = osize;
							*write_size = nsize - 
								osize;
					        }
					}
				}
			} 

		} else {  /* nb is 0 */
			BM(IN_LOCK(ip));
			/*
			 * Always reserve full blocks.
			 */
			if ((isize < (lbn + 1) * fs->fs_bsize) &&
			    !(flags & B_RESERVE))
				nsize = fragroundup(fs, size);
			else
				nsize = fs->fs_bsize;
			BM(IN_UNLOCK(ip));
			error = alloc(ip, lbn,
				blkpref(ip, lbn, (int)lbn, &ip->i_db[0]),
				nsize, &nb);
			if (error)
				return (error);
			*new_alloc = TRUE;
			if (flags & B_RESERVE) {
				ip->i_resfrags[lbn] = fs->fs_frag;
				*write_size = 0;
				nb = RESERVE(nb);
#ifdef	PFS
			} else if (flags & B_PREALLOC) {
				*write_size = 0;
				nb = RESERVE(nb);
#endif
			} else
				*write_size = nsize;  
		}
		IN_LOCK(ip);
		ip->i_db[lbn] = nb;
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
		*bnp = fsbtodb(fs, DADDR(nb));
		return (0);
	}
	/*
	 * Determine the number of levels of indirection. 
	 */
	pref = 0;
	sh = 1;
	xlbn = lbn - NDADDR;
	for (j = NIADDR; j > 0; j--) {
		sh *= NINDIR(fs);
		if (xlbn < sh)
			break;
		xlbn -= sh;
	}
	if (j == 0)
		return (EFBIG);
	/*
	 * Fetch the first indirect block allocating if necessary. 
	 */
	BM(IN_LOCK(ip));
	nb = ip->i_ib[NIADDR - j];
	BM(IN_UNLOCK(ip));
	if (nb == 0) {
		pref = blkpref(ip, lbn, 0, (daddr_t *)0);
		if (error = alloc(ip, lbn, pref, (int)indir_size, &nb))
			return (error);
		if (error = bwrite_indirect_block(ip, nb, indir_size)){
			blkfree(ip, nb, indir_size);
			return(error);
		}
		IN_LOCK(ip);
		ip->i_ib[NIADDR - j] = nb;
		ip->i_flag |= ICHG;
		IN_UNLOCK(ip);
	}
	/*
	 * Fetch through the indirect blocks, allocating as necessary.
	 *
	 * Synchronization: A thread executing this section of code may hold
	 * two buffer pool buffers simultaneously: e.g. one containing an
	 * indirect block, and another used by alloc() to contain cylinder
	 * group info.  This may potentially cause a deadlock if an unlimited
	 * number of threads are allowed to grab a first buffer, thereby
	 * emptying the free list.  At this point all threads will block
	 * forever attempting to get a second buffer.  So we limit the number
	 * of threads to nbuf-4. (Theoretically this should be 
	 * nbuf-1-<# of B_LOCKED buffers>, but B_LOCKED buffers are never used
	 * in OSF/1AD or OSF/1 1.3.  So nbuf-1 should be OK.  But let's be
	 * pessimistic and use nbuf-4 ... ).
	 */
	BFUNNEL_LOCK();
	while (b_funnel.b_count >= (nbuf-4)) {
		b_funnel.b_waiting = TRUE;
		assert_wait((int)&b_funnel.b_count, FALSE);
		BFUNNEL_UNLOCK();
		thread_block();
		BFUNNEL_LOCK();
	}
	b_funnel.b_count++;
	BFUNNEL_UNLOCK();

	for (; ; j++) {
		int bread_size;

		sh /= NINDIR(fs);
		i = (xlbn / sh) % NINDIR(fs);
		bread_size = indir_size;

		/*
		 * Indiret block size maybe smaller than the buffer
		 * block size.  Find the fragment where the bn is
		 * located if the case.
		 */
		if (indir_size > bcache_maxbsize) {
			int section;

			bread_size = bcache_maxbsize;
			section = i >> ufs_bcmax_nishift;

			nb += section << (bcache_maxbshift - fs->fs_fshift);
			i &= (ufs_bcmax_nisize-1);
		}
		error = bread(ip->i_devvp, fsbtodb(fs, nb), (int)bread_size,
			      NOCRED, &bp);

		if (error) {
			brelse(bp);
			goto out;
		}
		bap = bp->b_un.b_daddr;
		nb = bap[i];
		if (j == NIADDR)
			break;
		if (nb != 0) {
			brelse(bp);
			continue;
		}
		if (pref == 0)
			pref = blkpref(ip, lbn, 0, (daddr_t *)0);
		if (error = alloc(ip, lbn, pref, (int)indir_size, &nb)) {
			brelse(bp);
			goto out;
		}
		if (error = bwrite_indirect_block(ip, nb, indir_size)){
			blkfree(ip, nb, indir_size);
			goto out;
		}
		bap[i] = nb;
		/*
		 * If required, write synchronously, otherwise use
		 * delayed write. If this is the first instance of
		 * the delayed write, reassociate the buffer with the
		 * file so it will be written if the file is sync'ed. 
		 */
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp, vp);
	}
	/*
	 * Get the data block, allocating if necessary. 
	 */
	if (nb == 0 || IS_RESERVED(nb)) {
		*new_alloc = TRUE;
		if (nb == 0) {
			pref = blkpref(ip, lbn, i, &bap[0]);
			if (error = alloc(ip, lbn, pref, (int)fs->fs_bsize, 
					  &nb)) {
				brelse(bp);
				goto out;
			}

#ifdef	PFS
			if ((flags & B_RESERVE) || (flags & B_PREALLOC)) {
#else
			if (flags & B_RESERVE) {
#endif
				bap[i] = RESERVE(nb);
				*write_size = 0;     
			} else {
				bap[i] = nb;
				*write_size = fs->fs_bsize;  
			}
		} else {
			/*
			 * It's possible an entire block was reserved
			 * on behalf of a prior request and hence we 
			 * could be attempting to reserve already 
			 * allocated blocks.
			 */
			nb = DADDR(nb);  /* convert to real daddr */
			if (flags & B_RESERVE) {
				*write_size = 0;
				*bnp = fsbtodb(fs, nb);
				brelse(bp);
				goto out;
			}
			bap[i] = nb;	
			*write_size = fs->fs_bsize;
		}

		/*
		 * If required, write synchronously.
		 */
		if (flags & B_SYNC)
			bwrite(bp);
		else
			bdwrite(bp, vp);

	} else 
		brelse(bp);

 out:
	BFUNNEL_LOCK();
	b_funnel.b_count--;
	if (b_funnel.b_waiting) {
		b_funnel.b_waiting = FALSE;
		do_wakeup = TRUE;
	}
	BFUNNEL_UNLOCK();
	if (do_wakeup)
		thread_wakeup((int)&b_funnel.b_count);

	*bnp = fsbtodb(fs, nb);
	return(error);
}
#endif 	/* UFS_NBC */


bwrite_indirect_block(ip, nb, indir_bsize)
register struct inode *ip;
daddr_t nb;
long	indir_bsize;	
{
	register struct fs *fs;
	struct buf* bp;
	int bwrite_size = indir_bsize;
	int error;
	
	fs = ip->i_fs;

	/* 
	 * If buffer block size is smaller than the write
	 * block size, break up the write into smaller 
	 * buffers.
	 */
	if (indir_bsize > bcache_maxbsize)
		bwrite_size = bcache_maxbsize;

	/*
	 * Loop through and write one or more pieces.  Write
	 * synchronously so that indirect blocks never point
	 * at garbage.
	 */
	while (indir_bsize > 0) {
		bp = getblk(ip->i_devvp, fsbtodb(fs, nb), bwrite_size);
		clrbuf(bp);
		if (error = bwrite(bp))
			return (error);
		nb += bcache_maxbsize >> fs->fs_fshift;
		indir_bsize -= bwrite_size;
	}
	return(0);
}
