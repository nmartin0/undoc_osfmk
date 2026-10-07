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
 * $Log: nfs_bio.c,v $
 * Revision 1.7  1994/11/18  20:36:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/07/02  00:26:27  dbm
 * Fixed a bug in the merge which caused incorrect read-aheads for NFS.
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #:10102
 *  Testing:
 *  Module(s):
 *
 * Revision 1.5  1994/06/28  23:04:06  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.4  1993/07/14  18:15:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:37:44  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:01  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:28  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:07  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:58  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:14  cfj
 * Bump major revision number.
 *
 * Revision 2.3  93/10/20  15:26:35  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE defines to their
 *      DISK_GRANULE-based corelates.
 *
 * Revision 2.2  1991/08/31  13:50:47  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:45  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11.4.2  91/06/20  14:58:15  tmt
 * 	Update nfsnode with new size _before_ O_APPEND mode writes.
 * 	[91/06/20  08:51:34  tmt]
 * 
 * Revision 1.11  90/10/31  14:01:55  devrcs
 * 	Fix yet another problem with the original EFBIG fix; error bailouts must
 * 	reset uio_resid to avoid confusing code above this level (that code checks
 * 	whether uio_resid has changed to figure out if anything happened).
 * 	[90/10/16  14:43:30  dlb]
 * 
 * 	Correct the correction to the EFBIG check; the old code was assuming
 * 	sequential access would hit the limit.  Also carefully reworked the
 * 	check to use unsigned arithmetic for (potentially large) file offsets.
 * 	[90/10/16  09:06:35  dlb]
 * 
 * 	Correct the check for EFBIG. Transfer data if we can and return the
 * 	resid count. If we can't then return EFBIG immediately.
 * 	[90/10/11  09:43:13  sp]
 * 
 * Revision 1.10  90/10/07  14:38:46  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:29  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:20:48  gm]
 * 
 * Revision 1.9  90/09/23  15:56:14  devrcs
 * 	Unlock nfsnode before returning if error from
 * 	VOP_GETATTR (nfs_write, append mode).
 * 	[90/09/12  20:33:43  gmf]
 * 
 * Revision 1.8  90/07/27  09:04:18  devrcs
 * 	NFS parallelization.
 * 
 * 	Condensed history (reverse chronology):
 * 	[90/07/20  17:03:02  nags]
 * 
 * 	Parallellized for OSF/1					nags@encore.com
 * 	Serialized for OSF/1.					nags@encore.com
 * 	Mods for NFS going through buffer cache.		tmt@osf.org
 * 	Added a call to inode_uncache in nfs_read.	 	gmf@osf.org
 * 	Call bwrite to avoid bogus panic in nfs_write.		noemi@osf.org
 * 	Integrated 4.4BSD file system changes as of 1/5/90.	gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				tmt@osf.org
 * 	[90/06/12  21:35:31  nags]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 *	@(#)nfs_bio.c	7.9 (Berkeley) 1/4/90
 */

#if	MACH
#include <mach_nbc.h>
#include <mach_xp.h>
#ifdef  i386
#include <cputypes.h>
#endif
#endif

#include <sys/param.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#include <sys/trace.h>
#include <sys/mount.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>

#if	MACH
#include <kern/mfs.h>
#include <mach/memory_object.h>
#include <kern/assert.h>
#include <kern/parallel.h>
#endif

extern struct nfsmount *vfs_to_nfs();
/*
 * Vnode op for read using bio
 * Any similarity to ufs_read() is purely coincidental
 */
nfs_bioread(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct nfsnode *np = VTONFS(vp);
	struct nfsmount *nmp;
	register int biosize;
	struct buf *bp;
	struct vattr vattr;
	daddr_t lbn, bn, rablock;
	int diff, error = 0;
	long n, on;
	enum vtype type;
	u_long nsize;

	if (uio->uio_rw != UIO_READ)
		panic("nfs_read mode");
	if (uio->uio_resid == 0)
		return (0);
	if (uio->uio_offset < 0 && vp->v_type != VDIR)
		return (EINVAL);
	nmp = vfs_to_nfs(vp->v_mount);
	biosize = nmp->nm_rsize;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	NP_READ_LOCK(np);
	/*
	 * If the file's modify time on the server has changed since the
	 * last read rpc or you have written to the file,
	 * you may have lost data cache consistency with the
	 * server, so flush all of the file's data out of the cache.
	 * Then force a getattr rpc to ensure that you have up to date
	 * attributes.
	 * NB: This implies that cache data can be read when up to
	 * NFS_ATTRTIMEO seconds out of date. If you find that you need current
	 * attributes this could be forced by setting n_attrstamp to 0 before
	 * the VOP_GETATTR() call.
	 */
	/*
	 * XXX -- we need to add check here for VTEXT and return an error
	 *	  if the text file has been modified since last read.
	 */
	if (type != VLNK) {
		NP_LOCK(np);
		if (np->n_flag & NMODIFIED) {
			np->n_flag &= ~NMODIFIED;
			np->n_attrstamp = 0;
			np->n_direofoffset = 0;
			NP_UNLOCK(np);
			vinvalbuf(vp, TRUE);
			VOP_GETATTR(vp, &vattr, cred, error);
			if (error) {
				NP_READ_UNLOCK(np);
				return (error);
			}
			NP_LOCK(np);
			np->n_mtime = vattr.va_mtime.tv_sec;
			NP_UNLOCK(np);
		} else {
			NP_UNLOCK(np);
			VOP_GETATTR(vp, &vattr, cred, error);
			if (error) {
				NP_READ_UNLOCK(np);
				return (error);
			}
			if (np->n_mtime != vattr.va_mtime.tv_sec) {
				NP_LOCK(np);
				np->n_direofoffset = 0;
				NP_UNLOCK(np);
				vinvalbuf(vp, TRUE);
#if	MACH
				/*
				 * Make sure that the vm system doesn't
				 * try to use this guy anymore.  We don't do
				 * in the case above (NMODIFIED) because it
				 * was done when that bit was set in nfs_write.
				 */
				inode_uncache(vp);
#endif
				NP_LOCK(np);
				np->n_mtime = vattr.va_mtime.tv_sec;
				NP_UNLOCK(np);
			}
		}
	}

	BM(NP_LOCK(np));
	nsize = np->n_size;
	BM(NP_UNLOCK(np));
	do {
	    switch (type) {
	    case VREG:
		NFS_STATS(nfsstats.biocache_reads++);
		lbn = uio->uio_offset / biosize;
		on = uio->uio_offset & (biosize-1);
		n = MIN((unsigned)(biosize - on), uio->uio_resid);
		diff = nsize - uio->uio_offset;
		if (diff <= 0) {
			NP_READ_UNLOCK(np);
			return (error);
		}
		if (diff < n)
			n = diff;
		bn = lbn*btodg(biosize);
		rablock = (lbn+1)* btodg(biosize);
		if (np->n_lastr + 1 == lbn && nsize > dgtob(rablock))
			error = breada(vp, bn, biosize, rablock, biosize,
				cred, &bp);
		else
			error = bread(vp, bn, biosize, cred, &bp);
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_resid >= 0);
		NP_LOCK(np);
		np->n_lastr = lbn;
		NP_UNLOCK(np);
		if (bp->b_resid) {
		   diff = (on >= (biosize-bp->b_resid)) ? 0 :
			(biosize-bp->b_resid-on);
		   n = MIN(n, diff);
		}
		break;
	    case VLNK:
		NFS_STATS(nfsstats.biocache_readlinks++);
		on = 0;
		error = bread(vp, (daddr_t)0, NFS_MAXPATHLEN, cred, &bp);
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_resid >= 0);
		n = MIN(uio->uio_resid, NFS_MAXPATHLEN - bp->b_resid);
		break;
	    case VDIR:
		NFS_STATS(nfsstats.biocache_readdirs++);
		on = 0;
		error = bread(vp, uio->uio_offset, DIRBLKSIZ, cred, &bp);
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_resid >= 0);
		n = MIN(uio->uio_resid, DIRBLKSIZ - bp->b_resid);
		break;
	      default:
		error = EIO;
	    };
	    if (error) {
		NP_READ_UNLOCK(np);
		brelse(bp);
		return (error);
	    }
	    if (n > 0)
		error = uiomove(bp->b_un.b_addr + on, (int)n, uio);
	    switch (type) {
	    case VREG:
		if (n+on == biosize || uio->uio_offset == nsize)
			bp->b_flags |= B_AGE;
		break;
	    case VLNK:
		n = 0;
		break;
	    case VDIR:
		uio->uio_offset = bp->b_blkno;
		break;
	      default:
		panic("nfs_bioread: type");
	    };
	    brelse(bp);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	NP_READ_UNLOCK(np);
	return (error);
}

/*
 * Vnode op for write using bio
 */
nfs_write(vp, uio, ioflag, cred)
	register struct vnode *vp;
	register struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register int biosize;
	struct buf *bp;
	struct nfsmount *nmp;
	struct nfsnode *np = VTONFS(vp);
	struct vattr vattr;
	daddr_t lbn, bn;
	int n, on, error = 0;
	unsigned efbig;
	register unsigned file_limit;
	u_long nsize;

	if (uio->uio_rw != UIO_WRITE)
		panic("nfs_write mode");
	if (vp->v_type != VREG)
		return (EIO);
	if (uio->uio_offset < 0)
		return (EINVAL);
	if (uio->uio_resid == 0)
		return (0);
	NP_WRITE_LOCK(np);
	BM(NP_LOCK(np));
	nsize = np->n_size;
	BM(NP_UNLOCK(np));
	/* Should we try and do this ?? */
	if (ioflag & (IO_APPEND | IO_SYNC)) {
		NP_LOCK(np);
		if (np->n_flag & NMODIFIED) {
			np->n_flag &= ~NMODIFIED;
			NP_UNLOCK(np);
			vinvalbuf(vp, TRUE);
		} else
			NP_UNLOCK(np);
		if (ioflag & IO_APPEND) {
			NP_LOCK(np);
			np->n_attrstamp = 0;
			NP_UNLOCK(np);
			VOP_GETATTR(vp, &vattr, cred, error);
			if (error) {
				NP_WRITE_UNLOCK(np);
				return (error);
			}
			BM(NP_LOCK(np));
			nsize = np->n_size;
			BM(NP_UNLOCK(np));
			uio->uio_offset = nsize;
		}
		error = nfs_writerpc(vp, uio, cred);
		NP_WRITE_UNLOCK(np);
		return (error);
	}
#ifdef notdef
	cnt = uio->uio_resid;
	osize = np->n_size;
#endif

	/*
	 * We know this is a regular file as we checked earler.
	 */
	file_limit = u.u_rlimit[RLIMIT_FSIZE].rlim_cur;
	if (uio->uio_offset >= file_limit) {
		NP_WRITE_UNLOCK(np);
		unix_master();
		psignal(u.u_procp, SIGXFSZ);
		unix_release();
		return (EFBIG);
	}

	efbig = uio->uio_offset + uio->uio_resid;
	if (efbig > file_limit) {
		efbig -= file_limit;
		uio->uio_resid -= efbig;
	}
	else
		efbig = 0;
	/*
	 * I use nm_rsize, not nm_wsize so that all buffer cache blocks
	 * will be the same size within a filesystem. nfs_writerpc will
	 * still use nm_wsize when sizing the rpc's.
	 */
	nmp = vfs_to_nfs(vp->v_mount);
	biosize = nmp->nm_rsize;
	NP_LOCK(np);
	np->n_flag |= NMODIFIED;
	NP_UNLOCK(np);
	do {
		NFS_STATS(nfsstats.biocache_writes++);
		lbn = uio->uio_offset / biosize;
		on = uio->uio_offset & (biosize-1);
		n = MIN((unsigned)(biosize - on), uio->uio_resid);
		NP_LOCK(np);
		if (uio->uio_offset+n > np->n_size)
			np->n_size = uio->uio_offset+n;
		NP_UNLOCK(np);
		bn = lbn * btodg(biosize);
#if	MACH
		VN_LOCK(vp);
		if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
			VN_UNLOCK(vp);
			inode_uncache(vp);
		} else
			VN_UNLOCK(vp);
#endif
again:
		bp = getblk(vp, bn, biosize);
		LASSERT(BUF_LOCK_HOLDER(bp));
		if (bp->b_wcred == NOCRED) {
			crhold(cred);
			bp->b_wcred = cred;
		}
		if (bp->b_dirtyend > 0) {
			/*
                         * If the new write will leave a contiguous dirty
                         * area, just update the b_dirtyoff and b_dirtyend,
                         * otherwise force a write rpc of the old dirty area.
			 */
			if (on <= bp->b_dirtyend && (on+n) >= bp->b_dirtyoff) {
				bp->b_dirtyoff = MIN(on, bp->b_dirtyoff);
				bp->b_dirtyend = MAX((on+n), bp->b_dirtyend);
			} else {
                                if (error = bwrite(bp)) {
					break;
				}
                                goto again;
			}
		} else {
			bp->b_dirtyoff = on;
			bp->b_dirtyend = on+n;
		}
		if (error = uiomove(bp->b_un.b_addr + on, n, uio)) {
			brelse(bp);
			break;
		}
		if ((n+on) == biosize) {
			bp->b_flags |= B_AGE;
			bawrite(bp);
		} else
			bdwrite(bp, bp->b_vp);
	} while (error == 0 && uio->uio_resid > 0 && n != 0);
	if (efbig > 0)
		uio->uio_resid += efbig;
#ifdef notdef
	/* Should we try and do this for nfs ?? */
	if (error && (ioflag & IO_UNIT)) {
		np->n_size = osize;
		uio->uio_offset -= cnt - uio->uio_resid;
		uio->uio_resid = cnt;
	}
#endif
	NP_WRITE_UNLOCK(np);
	return (error);
}
