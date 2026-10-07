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
 * $Log: vfs_bio.c,v $
 * Revision 1.14  1994/11/18  20:50:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/08/31  22:47:58  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.12.2.1  1994/08/25  21:32:49  dbm
 * Fixed incorrect allocated of buffer in bufstats() function.
 *  Reviewer: John Litvin
 *  Risk:Low
 *  Benefit or PTS #: 10684
 *  Testing: Specific test case.
 *  Module(s):
 * 	vfs/vfs_bio.c
 *
 * Revision 1.12  1994/07/24  18:29:45  dbm
 * Changed initialization of bcache_maxbsize to use DFLT_BCBSIZE instead
 * of MIN_BCBSIZE.
 *
 *  Reviewer:None
 *  Risk:Low
 *  Benefit or PTS #: 10269
 *  Testing: Specific test case.
 *  Module(s):
 * 	sys/param.h
 * 	vfs/vfs_bio.c
 *
 * Revision 1.11  1994/07/11  20:11:52  dbm
 * Fixed BOOT_IPI3_NODE_LIST bootmagic logic to work correctly.
 *  Reviewer: None.
 *  Risk:Low
 *  Benefit or PTS #:10105
 *  Testing: Specific test case, configured IPI3 node.
 *  Module(s):
 * 	uxkern/server_init.c
 * 	vfs/vfs_bio.c
 * 	ufs/ufs_vfsops.c
 *
 * Revision 1.10  1994/06/28  23:22:28  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.9  1994/01/11  18:26:05  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: less lint complaints
 *  Testing: compiled
 *  Module(s):
 * 	nfs/nfs_vnops.c
 * 	vfs/fifo_vnops.c
 * 	vfs/vfs_cache.c
 * 	vfs/vfs_flock.c
 * 	vfs/vfs_vnops.c
 * 	vfs/vfs_bio.c
 * 	vfs/vfs_subr.c
 * 	vfs/vfs_vio.c
 * 	vfs/spec_vnops.c
 * 	vfs/vfs_syscalls.c
 * 	vfs/vfs_lookup.c
 *
 * Revision 1.8  1993/09/25  00:18:20  brad
 * Checking in a bug fix from Paul Roy at OSF.  Changes to biowait() and
 * biodone().  This fix is from OSF/1 1.3:
 *  * Revision 1.18.2.2  1991/10/15  08:54:22  jeffc
 *  *      Vnode pager clustering and clustered pagein:
 *  *      1) close a post_event() buf header flags check race (gmf, rod).
 *  *      (Bug #2558)
 *  *      [91/10/14  15:43:34  jeffc]
 *
 * Revision 1.7  1993/09/23  23:30:46  cfj
 * Merge R1.1 bug fix.
 *
 * Revision 1.6.6.1  1993/09/23  23:29:17  cfj
 * Initialize the buffer cache funnel.
 *
 * Revision 1.6  1993/07/14  18:46:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:08:49  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/27  01:59:50  brad
 * Removed temporary code that allowed PFS files to be cached in the buffer
 * cache ... PFS now uses Fast Path exclusively.
 *
 * Revision 1.4  1993/05/06  20:31:53  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:30  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.13  94/02/03  11:02:23  dnoveck
 *      Changes for per-node buffer-cache block size.
 *
 * Revision 2.11  1993/03/30  16:11:33  roy
 * 	Added code to vflushbuf and vinvalbuf for VFS_VIO conditional.
 * 	[93/03/19            roy]
 *
 * Revision 2.10  93/01/08  14:38:46  durriya
 * 	set b_devnode in buf structure.
 *
 * Revision 1.3  1993/04/03  03:12:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.3  1993/02/12  22:39:36  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.1.2.2  1993/02/09  21:41:28  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.2  1992/11/30  22:57:34  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:17:25  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/05  23:46:11  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:57:11  cfj
 * Bump major revision number.
 *
 * Revision 2.9  1992/08/26  12:14:36  loverso
 * 	Removed MAY_USE_BUFCACHE in favor of VIO_IS_BUF.
 * 	[92/08/14            roy]
 * 
 * Revision 2.8  92/05/24  14:05:24  pjg
 * 	92/03/20  15:08:57  jose
 * 	Added test of B_PRIVATE in biodone to avoid
 * 	releasing private buffers.
 * 	[92/05/20            srl]
 * 
 * Revision 2.7  92/05/18  12:31:35  roy
 * 	Revision 2.6.1.1  92/05/08  12:17:14  roy
 * 	Call mf_clean from vflushbuf for MAPPED_FILES.
 * 	[92/05/01            roy]
 * 
 * Revision 2.6  92/03/15  14:41:54  roy
 * 	92/03/03  16:53:31  roy
 * 	Add MAY_USE_BUFCACHE asserts for OSF1_ADFS.
 * 
 * Revision 2.5  92/03/09  13:58:29  durriya
 * 	Revision 3.15  92/02/27  21:08:52  jose
 * 	Made event_wait conditional in iowait to avoid
 * 	calling sleep when sync. i/o is complete.
 * 
 * 	Revision 3.14  92/02/18  18:57:24  jose
 * 	Added hook for synchronous i/o (not used yet).
 * 
 * 	Revision 3.13  91/12/27  17:26:31  jose
 * 	Changed interface to reply_hash_enter for port aliasing
 * 
 * 	Revision 3.12  91/12/18  17:18:55  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.4  91/12/16  21:21:23  roy
 * 	91/10/24  16:43:33  jose
 * 	Removed #if   MACH_LDEBUG in bwrite
 * 
 * 	91/10/21  18:48:03  emcmanus
 * 	Added an assertion.
 * 
 * 	91/10/17  18:34:31  barbou
 * 	Mark the pre-allocated buffer pool as "optimizing", meaning that whoever
 * 	uses these buffer should be aware of the memory copy optimization of
 * 	bio_read_reply().
 * 
 * Revision 2.3  91/10/14  13:26:37  sjs
 * 	91/09/13  12:51:52  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  14:30:23  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.5  91/08/27  15:40:37  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.4  91/08/01  17:02:49  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14  90/10/07  15:00:18  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:56  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:46  gm]
 * 
 * 	Remove obsolete panic in getblk when blkno too big, it breaks NFS.
 * 	[90/09/29  19:52:21  tmt]
 * 
 * 	Fix blown merge in biodone().
 * 	[90/09/26  08:33:03  jeffc]
 * 
 * 	Add assert to biowait (since the code assumes only the
 * 	owner of a buffer can wait on it) and cleaned up the
 * 	code in biowait.
 * 
 * 	Change BUF_INHERIT/BUF_GIVE_AWAY interaction with VOP_STRATEGY
 * 	in order to eliminate race conditions between strategy and biodone.
 * 	The strategy routine cannot make assertions about the ownership
 * 	of a buf lock, only that it is locked. For async I/O, the ownership
 * 	will have been given to a fake thread prior to being handed off to
 * 	strategy. This is accomplished via the BUF_GIVE_AWAY() and
 * 	BUF_ACCEPT() macros.
 * 	[90/09/25  19:04:46  jeffc]
 * 
 * 	remove vprint from vflushbuf
 * 	[90/09/21  17:14:07  gmf]
 * 
 * Revision 1.13  90/09/23  16:01:26  devrcs
 * 	Simplify biodone slightly: ensure that the event_post is
 * 	always performed prior to calling the b_iodone function,
 * 	regardless of the state of B_ASYNC. Simplify debug code.
 * 	Eliminate init_buf_done and related anachronisms.
 * 	[90/09/11  15:16:48  jeffc]
 * 
 * Revision 1.12  90/09/13  11:51:52  devrcs
 * 	Fixed a race caused by calling event_post too early in biodone
 * 	(bug 847)  Also, call BUF_GIVE_AWAY at splbio (bug 848)
 * 	[90/08/30  15:39:24  noemi]
 * 
 * Revision 1.11  90/08/24  12:30:08  devrcs
 * 	fix parameters to event_wait
 * 	[90/08/19  23:21:54  gmf]
 * 
 * 	Eliminate biodone threads in favor of handling
 * 	disk completions in interrupt context.
 * 	[90/08/18  03:53:29  nags]
 * 
 * Revision 1.10  90/08/09  13:29:52  devrcs
 * 	Removed a couple of bad assertions (nags).
 * 	[90/08/02  13:07:37  nags]
 * 
 * 	Removed bad assertion.
 * 	[90/07/25  16:23:19  nags]
 * 
 * 	Getnewbuf fixes for DELWRI buffers and b_iodone.
 * 	Added BUF_IS_LOCKED assertions.
 * 	[90/07/24  13:03:53  nags]
 * 
 * Revision 1.9  90/07/27  09:09:41  devrcs
 * 	Changes to B_CALL/b_iodone interface: callee is
 * 	now responsible for determining and achieving appropriate
 * 	thread context.
 * 	[90/07/19  15:38:59  jeffc]
 * 
 * 	Use BUFCACHE_STATS instead of BIO_STATISTICS, cleanup.
 * 	[90/07/20  17:09:04  nags]
 * 
 * 	Put BM locks around looking at clean and dirty block lists in
 * 	vinvalbuf.
 * 	[90/06/29  14:46:17  nags]
 * 
 * Revision 1.7  90/06/29  13:54:50  devrcs
 * 	Added include of <bufstats_cache.h>.
 * 	[90/06/26  11:41:21  nags]
 * 
 * Revision 1.6  90/06/22  20:56:23  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant history, reverse chronology:
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Changed vinvalbuf to invalidate indirect blocks	noemi@osf.org
 * 	Integrated 4.4BSD changes as of 1/5/90		noemi@osf.org
 * 	Added some missing splx calls in vflushbuf()	noemi@osf.org
 * 	Integrated Encore parallelization code		gmf@osf.org
 * 	Made modifications for working with Mach	noemi@osf.org
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
 *	@(#)vfs_bio.c	7.19 (Berkeley) 1/4/90
 */
#if	BUFCACHE_STATS
#include <bufcache_stats.h>
#endif
#include <mapped_files.h>
#include <vfs_vio.h>

#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/biostats.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>
#include <sys/trace.h>
#include <sys/ucred.h>
#include <kern/queue.h>
#include <kern/sched_prim.h>
#include <kern/assert.h>
#include <sys/lock_types.h>
#ifdef OSF1_SERVER
#include <sys/synch.h>
#include <uxkern/vm_param.h>
#include <uxkern/device_reply_hdlr.h>
#else
#include <kern/kalloc.h>
#endif

int	BQ_AGE_DISABLE = 1;

struct	buf *buf;		/* the buffer pool itself */
char	*buffers;
int	nbuf,			/* number of buffer headers */
	bufpages;		/* number of memory pages in the buffer pool */
extern int	bufhsz;		/* size of buffer cache hash table */
#ifdef NX
extern node_t this_node;
extern node_t  hippi_node_array[][2];           /* nodes with HIPPI hdw */
extern int     hippi_node_array_entries;        /* # of node with HIPPI */
extern node_t  ipi3_node_array[][2];            /* nodes with IPI3 hdw */
extern node_t  ipi3_node_array_entries;         /* # of nodes with IPI3 */
#endif
struct	bufhd *bufhash;		/* base of buffer cache hash table */

struct	buf bfreelist[BQUEUES];	/* heads of available lists */
#if	BUFCACHE_STATS
struct	bio_stats	bio_stats = { 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
#if	MACH_LDEBUG
char 	biodone_ldebug;
#endif

/*
 * The buffer cache now has its own limit on buffer size configurable at
 * node initialization.
 */

int	bcache_maxbsize;	/* Maximum block in buffer cache. */
int	bcache_maxbshift;	/* Associated shift. */
int	bcache_maxdgsize;	/* Maximum block in buffer cache */
				/* specified in disk granules. */
int	bcache_maxdgshift;	/* Associated shift. */
int	ipi3_bcache_maxbsize;	/* Maximum block in buffer cache for */ 
				/* ipi3 devices. */


/*
 * Locking precedence:
 *	buffer lock (blocking lock)
 *	buffer cache free list lock (spin lock)
 *	buffer cache hash chain lock (spin lock)
 *
 * We often need to search the hash chain first, and then take a buffer
 * lock when we think we have found the buffer in the cache.  We must
 * drop the hash chain lock to take the blocking buffer lock and retake
 * the hash chain lock later, if needed.
 *
 * When we want to remove a buffer from the free list, we lock the free
 * list and conditionally lock the buffer.  If we can't lock the buffer,
 * we try the next one.  When we manage to lock a buffer, we remove it
 * from the free list, while holding the buffer lock and the free list
 * lock.
 *
 * Buffers are locked pretty much from the time we find an interesting one
 * in the buffer pool until we are done with it.  This means throughout any
 * I/O as well.
 *
 */

/*
 * The buffer freelist lock covers all of the freelist queues.
 * The lock only applies to UNIX_LOCKS kernels.
 */
udecl_simple_lock_data(,bfreelist_lock);
#define	BFREE_LOCK_INIT()	usimple_lock_init(&bfreelist_lock);
#define	BFREE_LOCK()		usimple_lock(&bfreelist_lock)
#define	BFREE_UNLOCK()		usimple_unlock(&bfreelist_lock)

#define BFREE_LOCK_HOLDER()	SLOCK_HOLDER(&bfreelist_lock)

/*
 * A buffer cache hash chain is protected by an individual lock that
 * prevents buffers from being added to or removed from the chain while
 * the lock is held.  This lock is needed only by UNIX_LOCKS kernels.
 *
 * Each buffer cache hash chain contains a timestamp that is updated
 * when buffers are added to the list.  The timestamp when is not
 * incremented when buffers are removed from the hash chain.
 */
#define	BHASH_LOCK_INIT(bp)	usimple_lock_init(&(bp)->bhd_lock)
#define	BHASH_LOCK(bp)		usimple_lock(&((bufhd_t *) bp)->bhd_lock)
#define	BHASH_UNLOCK(bp)	usimple_unlock(&((bufhd_t *) bp)->bhd_lock)
#define BHASH_STAMP(bp)		((struct bufhd *)(bp))->bhd_stamp

/*
 * Read in (if necessary) the block and return a buffer pointer.
 */
bread(vp, blkno, size, cred, bpp)
	struct vnode *vp;
	daddr_t blkno;
	int size;
	struct ucred *cred;
	struct buf **bpp;
{
	register struct buf *bp;
	int error;

#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(vp));
#endif
	if (size == 0)
		panic("bread: size 0");
	*bpp = bp = getblk(vp, blkno, size);
        if (event_posted(&bp->b_iocomplete)) {
		trace(TR_BREADHIT, pack(vp, size), blkno);
		return (0);
	}
	bp->b_flags |= B_READ;
	if (bp->b_bcount > bp->b_bufsize)
		panic("bread");
	if (bp->b_rcred == NOCRED && cred != NOCRED) {
		crhold(cred);
		bp->b_rcred = cred;
	}
	event_clear(&bp->b_iocomplete);
	VOP_STRATEGY(bp, error);
	trace(TR_BREADMISS, pack(vp, size), blkno);
	u.u_ru.ru_inblock++;		/* pay for read */
	error = biowait(bp);
	LASSERT(BUF_LOCK_HOLDER(bp));
	ASSERT(bp->b_bcount == size);
	return (error);
}

/*
 * Read in the block, like bread, but also start I/O on the
 * read-ahead block (which is not allocated to the caller)
 */
breada(vp, blkno, size, rablkno, rabsize, cred, bpp)
	struct vnode *vp;
	daddr_t blkno; int size;
	daddr_t rablkno; int rabsize;
	struct ucred *cred;
	struct buf **bpp;
{
	register struct buf *bp, *rabp;
	int error;

#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(vp));
#endif
	bp = NULL;
	/*
	 * If the block isn't in core, then allocate
	 * a buffer and initiate i/o (getblk checks
	 * for a cache hit).  Note that incore is less
	 * certain on a multiprocessor.
	 */
	if (!incore(vp, blkno)) {
		*bpp = bp = getblk(vp, blkno, size);
		/*
		 * We depend on the fact that getblk() won't
		 * return until the b_iocomplete status reflects
		 * whether an I/O has already happened.  getblk()
		 * makes this guarantee by first taking the buffer
		 * lock, which will remain held while an I/O is
		 * in progress.  If we move to a scheme where the
		 * buffer lock is not held over the I/O, getblk()
		 * will have to block on the iodone event should
		 * the buffer be found in the cache.
		 */
		if (!event_posted(&bp->b_iocomplete)) {
			bp->b_flags |= B_READ;
			if (bp->b_bcount > bp->b_bufsize)
				panic("breada");
			if (bp->b_rcred == NOCRED && cred != NOCRED) {
				crhold(cred);
				bp->b_rcred = cred;
			}
			event_clear(&bp->b_iocomplete);
			VOP_STRATEGY(bp, error);
			trace(TR_BREADMISS, pack(vp, size), blkno);
			u.u_ru.ru_inblock++;		/* pay for read */
		} else
			trace(TR_BREADHIT, pack(vp, size), blkno);
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_bcount == size);
	}

	/*
	 * Start i/o on the read-ahead block.
	 */
	ASSERT(rablkno != 0 && rabsize > 0);
	if (!incore(vp, rablkno)) {
		rabp = getblk(vp, rablkno, rabsize);
		LASSERT(BUF_LOCK_HOLDER(rabp));
		ASSERT(rabp->b_bcount == rabsize);
		if (event_posted(&rabp->b_iocomplete)) {
			brelse(rabp);
			trace(TR_BREADHITRA, pack(vp, rabsize), rablkno);
		} else {
			rabp->b_flags |= B_READ|B_ASYNC;
			if (rabp->b_bcount > rabp->b_bufsize)
				panic("breadrabp");
			if (rabp->b_rcred == NOCRED && cred != NOCRED) {
				crhold(cred);
				rabp->b_rcred = cred;
			}
			event_clear(&rabp->b_iocomplete);

			BUF_GIVE_AWAY(rabp);

			VOP_STRATEGY(rabp, error);
			trace(TR_BREADMISSRA, pack(vp, rabsize), rablkno);
			u.u_ru.ru_inblock++;		/* pay in advance */
		}
	}

	/*
	 * If block was in core, let bread get it.
	 * If block wasn't in core, then the read was started
	 * above, and just wait for it.
	 */
	if (bp == NULL)
		return (bread(vp, blkno, size, cred, bpp));
	error = biowait(bp);
	LASSERT(BUF_LOCK_HOLDER(bp));
	ASSERT(bp->b_bcount == size);
	return (error);
}

/*
 * Write the buffer, waiting for completion.
 * Then release the buffer.
 */
bwrite(bp)
	register struct buf *bp;
{
	register int flag;
	register struct vnode *vp;
	int s, error = 0;

#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(bp->b_vp));	
#endif
	LASSERT(BUF_LOCK_HOLDER(bp));
	flag = bp->b_flags;
	bp->b_flags &= ~(B_READ | B_ERROR | B_DELWRI);
	/*
	 * If the write was "delayed" and is being initiated
	 * asynchronously then put the buffer on the q of blocks
	 * awaiting i/o completion status.  Do this before starting
	 * the I/O because after it is started this thread doesn't
	 * own the buffer.
	 */
	if ((flag & (B_ASYNC|B_DELWRI)) == (B_ASYNC|B_DELWRI))
		bp->b_flags |= B_AGE;
	if ((flag&B_DELWRI) == 0)
		u.u_ru.ru_oublock++;		/* noone paid yet */
	else
		reassignbuf(bp, bp->b_vp);
	trace(TR_BWRITE, pack(bp->b_vp, bp->b_bcount), bp->b_lblkno);
	if (bp->b_bcount > bp->b_bufsize)
		panic("bwrite");
	vp = bp->b_vp;
	s = splbio();
	VN_OUTPUT_LOCK(vp);
	vp->v_numoutput++;
	VN_OUTPUT_UNLOCK(vp);
	splx(s);
	event_clear(&bp->b_iocomplete);
	if (flag&B_ASYNC) {
		BUF_GIVE_AWAY(bp);
	}
	VOP_STRATEGY(bp, error);
	/*
	 * If the write was synchronous, then await i/o completion.
	 * If the write was delayed, give away ownership of the buffer.
	 */
	if ((flag&B_ASYNC) == 0) {
		error = biowait(bp);
		brelse(bp);
	}
	return (error);
}

/*
 * Release the buffer, marking it so that if it is grabbed
 * for another purpose it will be written out before being
 * given up (e.g. when writing a partial block where it is
 * assumed that another write for the same block will soon follow).
 * This can't be done for magtape, since writes must be done
 * in the same order as requested.
 */
bdwrite(bp, vp)
	register struct buf *bp;
	register struct vnode *vp;
{
	int error;

#ifdef	OSF1_ADFS
	ASSERT(VIO_IS_BUF(bp->b_vp));	
#endif
	LASSERT(BUF_LOCK_HOLDER(bp));
	if ((bp->b_flags & B_DELWRI) == 0) {
		bp->b_flags |= B_DELWRI;
		reassignbuf(bp, vp);
		u.u_ru.ru_oublock++;		/* noone paid yet */
	}
	/*
	 * If this is a tape drive, the write must be initiated.
	 */
	VOP_IOCTL(bp->b_vp, 0, B_TAPE, 0, NOCRED, error);
	if (error == 0)
		bawrite(bp);
	else {
		bp->b_flags |= B_DELWRI;
		event_post(&bp->b_iocomplete);
		brelse(bp);
	}
}

/*
 * Release the buffer, start I/O on it, but don't wait for completion.
 */
bawrite(bp)
	register struct buf *bp;
{

	LASSERT(BUF_LOCK_HOLDER(bp));
	bp->b_flags |= B_ASYNC;
	(void) bwrite(bp);
}

/*
 * Release the buffer, with no I/O implied.
 */
brelse(bp)
	register struct buf *bp;
{
	register struct buf *flist;
	register int awaken, inshead;
	long flags;
	int s;

	/*
	 * It is more than likely that the buffer was actually locked by some
	 * other thread and that we are picking up the pieces.  But we take
	 * care when we initiate an asynchronous I/O to give away ownership
	 * of the buffer.
     	 *
      	 * Furthermore, on a multiprocessor, it is not yet the time to wake up
       	 * someone waiting on this buffer, or waiting for a free buffer.
	 */
	LASSERT(BUF_LOCK_HOLDER(bp));
	trace(TR_BRELSE, pack(bp->b_vp, bp->b_bufsize), bp->b_lblkno);
	/*
	 * Retry I/O for locked buffers rather than invalidating them.
	 * We currently aren't locking any buffers. - XXX
	 */
	if ((bp->b_flags & B_ERROR) && (bp->b_flags & B_LOCKED))
		bp->b_flags &= ~B_ERROR;

	/*
	 * Disassociate buffers that are no longer valid.
	 */
	if (bp->b_flags & (B_NOCACHE|B_ERROR))
		bp->b_flags |= B_INVAL;
	if ((bp->b_bufsize <= 0) || (bp->b_flags & (B_ERROR|B_INVAL))) {
		if (bp->b_vp)
			brelvp(bp);
		bp->b_flags &= ~B_DELWRI;
	}
	/*
	 * Stick the buffer back on a free list.
	 */
	flags = bp->b_flags;
	bp->b_flags &= ~(B_ASYNC|B_AGE|B_NOCACHE|B_WANTED);
	awaken = inshead = 0;
	if (bp->b_bufsize <= 0) {
		/* block has no buffer... put at front of unused buffer list */
		flist = &bfreelist[BQ_EMPTY];
		inshead++;
	} else if (flags & (B_ERROR|B_INVAL)) {
		/* block has no info ... put at front of most free list */
		flist = &bfreelist[BQ_AGE];
		inshead++;
	} else {
		if (flags & B_LOCKED)
			flist = &bfreelist[BQ_LOCKED];
 		else if ((flags & B_AGE) && (BQ_AGE_DISABLE == 0))
			flist = &bfreelist[BQ_AGE];
		else
			flist = &bfreelist[BQ_LRU];
	}
	s = splbio();
	BFREE_LOCK();
	if (inshead)
		binsheadfree(bp, flist);
	else
		binstailfree(bp, flist);
	/*
	 * Wakeup processes waiting for a free buffer.
	 */
	if (bfreelist[0].b_flags&B_WANTFREE) {
		bfreelist[0].b_flags &= ~B_WANTFREE;
		awaken++;
	}
	BFREE_UNLOCK();
	/*
	 * Unlocking buffer with interrupts disabled can deadlock
	 * with pmap system when another thread holds the buffer
	 * lock's interlock with interrupts enabled.
	 */
	BUF_UNLOCK(bp);
	splx(s);
	if (awaken)
		thread_wakeup((int)bfreelist);
}

/*
 * See if the block is associated with some buffer
 * (mainly to avoid getting hung up on a wait in breada)
 */
incore(vp, blkno)
	struct vnode *vp;
	daddr_t blkno;
{
	register struct buf *bp;
	register struct buf *dp;
	int s;

	dp = BUFHASH(vp, blkno);
	s = splbio();
	BHASH_LOCK(dp);
	for (bp = dp->b_forw; bp != dp; bp = bp->b_forw)
		/*
		 * There is an assumption that incore won't block.
		 * So, we can't lock the buffer.  Incore is advisory
		 * at best on a multiprocessor.  The block may be
		 * incore now, but not incore when you go to read it
		 * later.  The buffer may also be valid now, but
		 * invalid later.  So we don't lock it to check
		 * the flags.
		 */
		if (bp->b_lblkno == blkno && bp->b_vp == vp &&
		    (bp->b_flags & B_INVAL) == 0) {
			BHASH_UNLOCK(dp);
			splx(s);
			return (1);
		}
	BHASH_UNLOCK(dp);
	splx(s);
	return (0);
}

/*
 * Return a block if it is in memory.
 */
#if	!MACH
baddr(vp, blkno, size, cred, bpp)
	struct vnode *vp;
	daddr_t blkno;
	int size;
	struct ucred *cred;
	struct buf **bpp;
{

	if (incore(vp, blkno))
		return (bread(vp, blkno, size, cred, bpp));
	*bpp = 0;
	return (0);
}
#endif	



/*
 * Assign a buffer for the given block.  If the appropriate
 * block is already associated, return it; otherwise search
 * for the oldest non-busy buffer and reassign it.
 *
 * This routine is never called from interrupt level and
 * especialy not during a panic because it can block.  The
 * caller is expected to wait for any I/O on the buffer to complete.
 */
struct buf *
getblk(vp, blkno, size)
	register struct vnode *vp;
	daddr_t blkno;
	int size;
{
	register struct buf *bp, *dp, *bp2;
	register int s;
	register int stamp;
	int research = 0;
	int locked;

	if (size > bcache_maxbsize)
		panic("getblk: size too big");
	/*
	 * Search the cache for the block.  If we hit, but
	 * the buffer is in use for i/o, then we wait until
	 * the i/o has completed.
	 */
	dp = BUFHASH(vp, blkno);
loop:
	s = splbio();
	BHASH_LOCK(dp);
	for (bp = dp->b_forw; bp != dp; bp = bp->b_forw) {
		/*
		 * We can check these fields without locking bp because
		 * we have the hash chain locked so these fields will
		 * not change on us.  We can't check for an invalid
		 * buffer without locking the buffer, so we must unlock
		 * the hash chain, lock the buffer, and then check the
		 * flags field.
		 */
		if (bp->b_lblkno != blkno || bp->b_vp != vp)
			continue;
		BUF_LOCK_TRY(bp, locked);
		if (!locked) {
			BHASH_UNLOCK(dp);
			splx(s);
			BUF_LOCK(bp);		/* wait for unlock to occur */
			BUF_UNLOCK(bp);
			goto loop;
		}
		/*
		 * If this buffer matches and is invalid, remove it
		 * from the hash chain, so that we don't find it again
		 * when re-searching the hash chain after calling getnewbuf.
		 */
		if (bp->b_flags & B_INVAL) {
			bremhash(bp);
			BHASH_UNLOCK(dp);
			BUF_UNLOCK(bp);
			splx(s);
			goto loop;
		}
		BHASH_UNLOCK(dp);
		BFREE_LOCK();
		bremfree(bp);
		BFREE_UNLOCK();
		splx(s);
		if (bp->b_bcount != size) {
			/* Stray b_bcount */
			bp->b_flags |= B_INVAL;
			/*
			 * How do we know this buffer is dirty?  XXX
			 * For that matter, how do we guarantee that
			 * there's no race with someone else re-creating
			 * an overlapping buffer and then issuing a read?  XXX
			 */
			bwrite(bp);
			goto loop;
		}
		bp->b_flags |= B_CACHE;
		BUF_STATS(bio_stats.getblk_hits++);
		LASSERT(BUF_LOCK_HOLDER(bp));
		ASSERT(bp->b_bcount == size);
		return (bp);
	}
	stamp = BHASH_STAMP(dp);
	BHASH_UNLOCK(dp);
	splx(s);
	BUF_STATS(bio_stats.getblk_misses++);
	bp = getnewbuf();
	bfree(bp);
	bgetvp(vp, bp);
	bp->b_lblkno = blkno;
	bp->b_blkno = blkno;
	bp->b_error = 0;
	bp->b_resid = 0;
	s = splbio();
	BHASH_LOCK(dp);
	if (stamp != BHASH_STAMP(dp)) {
		/*
		 * Someone else could have inserted an identical buffer
		 * in the hash chain while the hash chain was unlocked
		 * or while we slept in getnewbuf (if we did).
		 */
		research++;
		for (bp2 = dp->b_forw; bp2 != dp; bp2 = bp2->b_forw) {
			/*
			 * We can't check the B_INVAL flag here because
			 * we don't have bp2 locked.  So we go to the
			 * top and try again when we find a match.
			 * We mark bp invalid so that it will be added
			 * to the begining of the age list.
			 */
			if (bp2->b_lblkno == blkno && bp2->b_vp == vp) {
				BHASH_UNLOCK(dp);
				splx(s);
				bp->b_flags |= B_INVAL;
				brelse(bp);
				BUF_STATS(bio_stats.getblk_dupbuf++);
				goto loop;
			}
		}
	}
	binshash(bp, dp);
	LASSERT(valid_buf_on_chain(bp, dp));
	BHASH_UNLOCK(dp);
	splx(s);
	if (research)
		BUF_STATS(bio_stats.getblk_research += research);
	if (size != bp->b_bcount)
		allocbuf(bp, size);
	LASSERT(BUF_LOCK_HOLDER(bp));
	ASSERT(bp->b_bcount == size);
	return (bp);
}

/*
 * get an empty block,
 * not assigned to any particular device
 */
struct buf *
geteblk(size)
	int size;
{
	register struct buf *bp;

	if (size > bcache_maxbsize)
		panic("geteblk: size too big");
	bp = getnewbuf();
	bp->b_flags |= B_INVAL;
	bp->b_error = 0;
	bp->b_resid = 0;
	bfree(bp);
	if (size != bp->b_bcount)
		allocbuf(bp, size);
	LASSERT(BUF_LOCK_HOLDER(bp));
	ASSERT(bp->b_bcount == size);
	return(bp);
}

/*
 * Find a buffer which is available for use.
 * Select something from a free list.
 * Preference is to AGE list, then LRU list.
 * Removes buffer from its hash chain.
 */
struct buf *
getnewbuf()
{
	register struct buf *bp, *dp;
	register struct ucred *cred;
	register struct buf *dp2;
	int s, locked;
	int lockedbufs = 0;

	BUF_STATS(bio_stats.getnewbuf_calls++);
loop:
	s = splbio();
	BFREE_LOCK();
	for (dp = &bfreelist[BQ_AGE]; dp > bfreelist; dp--) {
		if (dp->av_forw == dp)
			continue;
		/*
		 * Walk freelist avoiding locked buffers and hash chains.
		 * On a uniprocessor, this loop will terminate on the first
		 * buffer on the freelist.
		 */
		for (bp = dp->av_forw; bp != dp; bp = bp->av_forw) {
			BUF_LOCK_TRY(bp, locked);
			if (!locked) {
				lockedbufs++;
				continue;
			}
			dp2 = bp->b_hash_chain;
			if (dp2 != BHASH_NULL)
				BHASH_LOCK(dp2);
			LASSERT(dp2==BHASH_NULL || valid_chain(dp2, 0));
			goto got_one;
		}
	}
	if (dp == bfreelist) {		/* no free blocks */
		dp->b_flags |= B_WANTFREE;
		assert_wait((int)dp, FALSE);
		BFREE_UNLOCK();
		splx(s);
		thread_block();
		goto loop;
	}
got_one:
	LASSERT(BFREE_LOCK_HOLDER());
	LASSERT(BUF_LOCK_HOLDER(bp));
	bremfree(bp);
	BFREE_UNLOCK();
	if (bp->b_flags & B_DELWRI) {
		if (dp2 != BHASH_NULL)
			BHASH_UNLOCK(dp2);
		splx(s);
		(void) bawrite(bp);
		goto loop;
	}
	if (dp2 != BHASH_NULL) {
		bremhash(bp);
		BHASH_UNLOCK(dp2);
	}
	splx(s);
	trace(TR_BRELSE, pack(bp->b_vp, bp->b_bufsize), bp->b_lblkno);
	if (lockedbufs)
		BUF_STATS(bio_stats.getnewbuf_buflocked += lockedbufs);
	if (bp->b_vp)
		brelvp(bp);
	if (bp->b_rcred != NOCRED) {
		cred = bp->b_rcred;
		bp->b_rcred = NOCRED;
		crfree(cred);
	}
	if (bp->b_wcred != NOCRED) {
		cred = bp->b_wcred;
		bp->b_wcred = NOCRED;
		crfree(cred);
	}
	bp->b_flags = B_BUSY;
	bp->b_iodone = NULL;
	event_clear(&bp->b_iocomplete);
	LASSERT(BUF_LOCK_HOLDER(bp));
	return (bp);
}

/*
 * Wait for I/O completion on the buffer; return errors
 * to the user.
 */
biowait(bp)
	register struct buf *bp;
{
	LASSERT(BUF_LOCK_HOLDER(bp));
	(void) event_wait(&bp->b_iocomplete, FALSE, 0);

	/*
	 * Pick up the device's error number and pass it to the user;
	 * if there is an error but the number is 0 set a generalized code.
	 */
	if ((bp->b_flags & B_ERROR) == 0)
		return (0);
	if (bp->b_error)
		return (bp->b_error);
	return (EIO);
}

/*
 * Mark I/O complete on a buffer.
 * If someone should be called, e.g. the pageout
 * daemon, do so.  Otherwise, wake up anyone
 * waiting for it.
 */
biodone(bp)
	register struct buf *bp;
{
	register struct vnode *vp;
	int s, wakeup = 0;

	ASSERT(bp != (struct buf *) NULL);
	if (event_posted(&bp->b_iocomplete))
		panic("dup biodone");

	if ((bp->b_flags & B_READ) == 0) {
		bp->b_dirtyoff = bp->b_dirtyend = 0;
		if (vp = bp->b_vp) {
			s = splbio();
			VN_OUTPUT_LOCK(vp);
			ASSERT(vp->v_numoutput > 0);
			vp->v_numoutput--;
			if ((vp->v_outflag & VOUTWAIT) &&
			    vp->v_numoutput <= 0) {
				vp->v_outflag &= ~VOUTWAIT;
				wakeup++;
			}
			VN_OUTPUT_UNLOCK(vp);
			splx(s);
			if (wakeup)
				thread_wakeup((int)&vp->v_numoutput);
		}
	}

	/*
	 * NOTE:  Device drivers using b_iodone must funnel, 
	 * if necessary, in their iodone routine -- and must
	 * also provide themselves a thread context, if necessary.
	 */
	if (bp->b_iodone) {
		void (*f)() = bp->b_iodone;
		bp->b_iodone = NULL;
		(*f)(bp);
		return;
	}

	/*
	 * There are two event_post calls here because an event post
	 * before checking the b_flags field can race with a thread
	 * changing that field when it awakens.  Doing a single call
	 * after brelse() would change current semantics.
	 */
	if (bp->b_flags & B_ASYNC) {
		/*
		 * This buf must have been previously given away. We
		 * accept ownership here.
		 */
		event_post(&bp->b_iocomplete);
		BUF_ACCEPT(bp);
		brelse(bp);
	} else {
		event_post(&bp->b_iocomplete);
	}
}

/*
 * Make sure all write-behind blocks associated
 * with mount point are flushed out (from sync).
 */
mntflushbuf(mountp, flags)
	struct mount *mountp;
	int flags;
{
	register struct vnode *vp;
	register struct vnode *nvp;

	MOUNT_VLIST_LOCK(mountp);
	for (vp = mountp->m_mounth; vp; vp = nvp) {
		/* 
		 * Potentially replace vp with its shadow (VBLK).
		 * shadowvnode and vget_nowait expect to receive a 
		 * locked vnode.
		 * nvp will hold the vnode we flush; vp will hold the
		 * one on the mount vnode list.
		 */
		VN_LOCK(vp);
		if (vp->v_type == VBLK) {
			if ((nvp = shadowvnode(vp)) == (struct vnode *) 0) {
		 		VN_UNLOCK(vp);
		 		nvp = vp->v_mountf;
	   	 		continue;
			}
		 	VN_UNLOCK(vp);
			VN_LOCK(nvp);
		} else 
			nvp = vp;
		if (vget_nowait(nvp)) {
		 	VN_UNLOCK(nvp);
		 	nvp = vp->v_mountf;
	   	 	continue;
		}
		VN_UNLOCK(nvp);
		MOUNT_VLIST_UNLOCK(mountp);
		vflushbuf(nvp, flags);
		vrele(nvp);
		/*
		 * We must check to see if the vnode is still on the
		 * mount vnode list.  After the above, just about anything
		 * could have happened to the vnode.  It's okay if the
		 * vnode was removed from this mount vnode list and
		 * added back to it because we insert at the beginning
		 * of the list.
		 */
		MOUNT_VLIST_LOCK(mountp);
		if (vp->v_mount == mountp)
			nvp = vp->v_mountf;
		else  {
			BUF_STATS(bio_stats.mntflushbuf_misses++);
			/*
			 * We have to start all over again.
			 */
			nvp = mountp->m_mounth;
		}
	}
	MOUNT_VLIST_UNLOCK(mountp);
}

/*
 * Flush all dirty buffers associated with a vnode.
 */
vflushbuf(vp, flags)
	register struct vnode *vp;
	int flags;
{
	register struct buf *bp;
	register struct buf *nbp;
	int locked, s;
	int lockskips = 0;

#ifdef	OSF1_ADFS
	/*
	 * See the comment in ufs_fsync to see how syncing files could
	 * be integrated in a cleaner way.
	 */
#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp)) {
		/*
		 * Clean main memory pages.  Afterwards, we must fall through
		 * because of the code below dealing with indirect blocks.
		 */
		mf_clean(vp, (flags & B_SYNC) ? TRUE : FALSE);
	}
#endif		
#if	VFS_VIO
	/*
	 * For VIO_IS_FASTPATH files, we must wait for write-behinds
	 * to complete (if B_SYNC).  The v_numoutput logic below handles 
	 * this.  Consider using a dedicated vio_fsync() routine instead.
	 */
#endif
#endif	/* OSF1_ADFS */

loop:
	s = splbio();
	VN_BUFLISTS_LOCK(vp);
	for (bp = vp->v_dirtyblkhd; bp; bp = nbp) {
		BUF_LOCK_TRY(bp, locked);
		if (!locked) {
			nbp = bp->b_blockf;
			lockskips++;
			continue;
		}
		ASSERT((bp->b_flags & B_DELWRI) != 0);
		VN_BUFLISTS_UNLOCK(vp);
		BFREE_LOCK();
		bremfree(bp);
		BFREE_UNLOCK();
		splx(s);
		/*
		 * Wait for I/O associated with indirect blocks to complete,
		 * since there is no way to quickly wait for them below.
		 * NB - This is really specific to ufs, but is done here
		 * as it is easier and quicker.
		 */
		if (bp->b_vp == vp || (flags & B_SYNC) == 0)
			(void) bawrite(bp);
		else
			(void) bwrite(bp);
		goto loop;
	}
	VN_BUFLISTS_UNLOCK(vp);
	splx(s);
	if (lockskips)
		BUF_STATS(bio_stats.vflushbuf_lockskips += lockskips);
	if ((flags & B_SYNC) == 0)
		return;

	s = splbio();
	VN_OUTPUT_LOCK(vp);
	while (vp->v_numoutput) {
		vp->v_outflag |= VOUTWAIT;
		assert_wait((int)&vp->v_numoutput, FALSE);
		VN_OUTPUT_UNLOCK(vp);
		thread_block();
		VN_OUTPUT_LOCK(vp);
	}
	VN_OUTPUT_UNLOCK(vp);
	VN_BUFLISTS_LOCK(vp);
	if (vp->v_dirtyblkhd) {
		VN_BUFLISTS_UNLOCK(vp);
		splx(s);
		/*
		 * Don't need this.  This can happen when sync'ing;
		 * especially on devvp.
		 * vprint("vflushbuf: dirty", vp);
		 */
		goto loop;
	}
	VN_BUFLISTS_UNLOCK(vp);
	splx(s);
}

/*
 * Invalidate in core blocks belonging to closed or umounted filesystem
 *
 * Go through the list of vnodes associated with the file system;
 * for each vnode invalidate any buffers that it holds. Normally
 * this routine is preceeded by a mntflushbuf call, so that on a quiescent
 * filesystem there will be no dirty buffers when we are done. Binval
 * returns the count of dirty buffers when it is finished.
 *
 * If the vnode represents a block special file, we use its shadow vnode
 * instead, since all buffered io will have been done using it, not the
 * "real" vnode.
 */
mntinvalbuf(mountp)
	struct mount *mountp;
{
	register struct vnode *vp;
	register struct vnode *nvp;
	int dirty = 0;

	MOUNT_VLIST_LOCK(mountp);
	for (vp = mountp->m_mounth; vp; vp = nvp) {
		/* 
		 * Potentially replace vp with its shadow (VBLK).
		 * shadowvnode and vget_nowait expect to receive a 
		 * locked vnode.
		 * nvp will hold the vnode we flush; vp will hold the
		 * one on the mount vnode list.
		 */
		VN_LOCK(vp);
		if (vp->v_type == VBLK) {
			if ((nvp = shadowvnode(vp)) == (struct vnode *) 0) {
		 		VN_UNLOCK(vp);
		 		nvp = vp->v_mountf;
	   	 		continue;
			}
		 	VN_UNLOCK(vp);
			VN_LOCK(nvp);
		} else 
			nvp = vp;
		if (vget_nowait(nvp)) {
		 	VN_UNLOCK(nvp);
		 	nvp = vp->v_mountf;
	   	 	continue;
		}
		VN_UNLOCK(nvp);
	   	MOUNT_VLIST_UNLOCK(mountp);
		dirty += vinvalbuf(nvp, 1);
		vrele(nvp);
		/*
		 * We must check to see if the vnode is still on the
		 * mount vnode list.  After the above, just about anything
		 * could have happened to the vnode.  It's okay if the
		 * vnode was removed from this mount vnode list and
		 * added back to it because we insert at the beginning
		 * of the list.
		 */
		MOUNT_VLIST_LOCK(mountp);
		if (vp->v_mount == mountp)
			nvp = vp->v_mountf;
		else  {
			BUF_STATS(bio_stats.mntinvalbuf_misses++);
			/*
			 * We have to start all over again.
			 */
			nvp = mountp->m_mounth;
		}
	}
	MOUNT_VLIST_UNLOCK(mountp);
	return (dirty);
}

/*
 * Flush out and invalidate all buffers associated with a vnode.
 * Called with the underlying object locked.
 */
vinvalbuf(vp, save)
	register struct vnode *vp;
	int save;
{
	register struct buf *bp;
	register struct buf **blist;
	register s;
	int dirty = 0, misses = 0;

	s = splbio();
	VN_BUFLISTS_LOCK(vp);
	for (;;) {
		if (vp->v_dirtyblkhd)
			blist = &vp->v_dirtyblkhd;
		else if (vp->v_cleanblkhd)
			blist = &vp->v_cleanblkhd;
		else 
			break;
		while (bp = *blist) {
			VN_BUFLISTS_UNLOCK(vp);
			BUF_LOCK(bp);
			/*
			 * See if this buffer is still at the head of this list.
			 */
			BM(VN_BUFLISTS_LOCK(vp));
			if (bp != *blist) {
				BM(VN_BUFLISTS_UNLOCK(vp));
				BUF_UNLOCK(bp);
				VN_BUFLISTS_LOCK(vp);
				misses++;
				continue;
			}
			BM(VN_BUFLISTS_UNLOCK(vp));
			BFREE_LOCK();
			bremfree(bp);
			BFREE_UNLOCK();
			splx(s);
			if (save && (bp->b_flags & B_DELWRI)) {
				dirty++;
				bawrite(bp);
				s = splbio();
				VN_BUFLISTS_LOCK(vp);
				continue;
			}
                        if (bp->b_vp != vp) {
                                reassignbuf(bp, bp->b_vp);
			} else 
                                bp->b_flags |= B_INVAL;
			brelse(bp);
			s = splbio();
			VN_BUFLISTS_LOCK(vp);
		}
	}
	VN_BUFLISTS_UNLOCK(vp);

#if	VFS_VIO
	if (dirty || VIO_IS_FASTPATH(vp)) {
		/*
		 * For fastpath files we must wait for all previously
		 * initiated write-behinds to complete.  
		 */
#else
	if (dirty) {
#endif
		VN_OUTPUT_LOCK(vp);
		while (vp->v_numoutput) {
			vp->v_outflag |= VOUTWAIT;
			assert_wait((int)&vp->v_numoutput, FALSE);
			VN_OUTPUT_UNLOCK(vp);
			thread_block();
			VN_OUTPUT_LOCK(vp);
		}
		VN_OUTPUT_UNLOCK(vp);
	}
	splx(s);
	if (misses)
		BUF_STATS(bio_stats.vinvalbuf_misses += misses);
	return (dirty);
}

/*
 * Associate a buffer with a vnode.
 */
bgetvp(vp, bp)
	register struct vnode *vp;
	register struct buf *bp;
{
	int s;

	LASSERT(BUF_LOCK_HOLDER(bp));
	if (bp->b_vp || bp->b_rvp)
		panic("bgetvp: not free");
	VHOLD(vp);
	bp->b_rvp = bp->b_vp = vp;
	VN_LOCK(vp);
	if (vp->v_type == VBLK || vp->v_type == VCHR) {
		bp->b_dev = vp->v_rdev;
#ifdef OSF1_ADFS
                bp->b_devnode = vp->v_devnode;
#endif
	} else {
		bp->b_dev = NODEV;
#ifdef OSF1_ADFS
                bp->b_devnode = NONODE;
#endif
        }
                
	VN_UNLOCK(vp);
	/*
	 * Insert onto list for new vnode.
	 */
	s = splbio();
	VN_BUFLISTS_LOCK(vp);
	if (vp->v_cleanblkhd) {
		bp->b_blockf = vp->v_cleanblkhd;
		bp->b_blockb = &vp->v_cleanblkhd;
		vp->v_cleanblkhd->b_blockb = &bp->b_blockf;
		vp->v_cleanblkhd = bp;
	} else {
		vp->v_cleanblkhd = bp;
		bp->b_blockb = &vp->v_cleanblkhd;
		bp->b_blockf = NULL;
	}
	VN_BUFLISTS_UNLOCK(vp);
	splx(s);
}

/*
 * Disassociate a buffer from a vnode.
 */
brelvp(bp)
	register struct buf *bp;
{
	register struct vnode	*vp;
	struct buf		*bq;
	int s;

	LASSERT(BUF_LOCK_HOLDER(bp));
	vp = bp->b_rvp;
	if (vp == (struct vnode *) 0)
		panic("brelvp: NULL");
	/*
	 * Delete from old vnode list, if on one.
	 */
	s = splbio();
	VN_BUFLISTS_LOCK(vp);
	if (bp->b_blockb) {
		if (bq = bp->b_blockf)
			bq->b_blockb = bp->b_blockb;
		*bp->b_blockb = bq;
		bp->b_blockf = NULL;
		bp->b_blockb = NULL;
	}
	VN_BUFLISTS_UNLOCK(vp);
	splx(s);
	bp->b_rvp = bp->b_vp = (struct vnode *) 0;
	HOLDRELE(vp);
}

/*
 * Reassign a buffer from one vnode to another.
 * Used to assign file specific control information
 * (indirect blocks) to the vnode to which they belong.
 */
reassignbuf(bp, newvp)
	register struct buf *bp;
	register struct vnode *newvp;
{
	register struct buf	*bq, **listheadp;
	register struct vnode	*vp;
	register		 s;

	LASSERT(BUF_LOCK_HOLDER(bp));
	if (newvp == NULL)
		panic("reassignbuf: NULL");
	vp = bp->b_rvp;
	/*
	 * Delete from old vnode list, if on one.
	 */
	if (vp != (struct vnode *) 0) {
		ASSERT(bp->b_blockb != 0);
		s = splbio();
		VN_BUFLISTS_LOCK(vp);
		if (bq = bp->b_blockf)
			bq->b_blockb = bp->b_blockb;
		*bp->b_blockb = bq;
		VN_BUFLISTS_UNLOCK(vp);
		splx(s);
	}
	/*
	 * If dirty, put on list of dirty buffers;
	 * otherwise insert onto list of clean buffers.
	 */
	if (bp->b_flags & B_DELWRI)
		listheadp = &newvp->v_dirtyblkhd;
	else
		listheadp = &newvp->v_cleanblkhd;
	s = splbio();
	VN_BUFLISTS_LOCK(newvp);
	if (*listheadp) {
		bp->b_blockf = *listheadp;
		bp->b_blockb = listheadp;
		bp->b_blockf->b_blockb = &bp->b_blockf;
		*listheadp = bp;
	} else {
		*listheadp = bp;
		bp->b_blockb = listheadp;
		bp->b_blockf = NULL;
	}
	VN_BUFLISTS_UNLOCK(newvp);
	splx(s);
	bp->b_rvp = newvp;
}


/*
 * Release space associated with a buffer.  Why isn't this a macro? - XXX
 */
bfree(bp)
	struct buf *bp;
{
	bp->b_bcount = 0;
}

/*
 * Expand or contract the actual memory allocated to a buffer.
 * If no memory is available, release buffer and take error exit
 */
#ifdef	OSF1_SERVER
allocbuf(bp, size)
	register struct buf *bp;
	int size;
{
	vm_size_t	current_size, desired_size;
	vm_offset_t	new_start;

	LASSERT(BUF_LOCK_HOLDER(bp));
	current_size = bp->b_bufsize;
	desired_size = round_page(size);

	if (current_size < desired_size) {
	    /*
	     * Buffer is growing.
	     * If buffer already has data, allocate new area and copy
	     * old data to it.
	     */
	    (void) vm_allocate(mach_task_self(),
			       &new_start,
			       desired_size,
			       TRUE);
#if	0
	    (void) vm_pageable(mach_task_self(),
			       new_start,
			       desired_size,
			       VM_PROT_READ|VM_PROT_WRITE);
#endif
	    bcopy(bp->b_un.b_addr,
		  (caddr_t) new_start,
		  bp->b_bufsize);
	    (void) vm_deallocate(mach_task_self(),
				 (vm_offset_t)bp->b_un.b_addr,
				 current_size);
	    bp->b_un.b_addr = (char *)new_start;
	    bp->b_bufsize = desired_size;
	}
	bp->b_bcount = size;
	return (1);
}
#else	/* OSF1_SERVER */
allocbuf(tp, size)
	register struct buf *tp;
	int size;
{
	register struct buf *bp, *ep, *dp;
	int sizealloc, take, locked;
	int s, buflocked = 0;

	LASSERT(BUF_LOCK_HOLDER(tp));
	sizealloc = round_page(size);
	/*
	 * Buffer size is not changing
	 */
	if (sizealloc == tp->b_bufsize) {
		tp->b_bcount = size;
		return;
	}
	/*
	 * Buffer size is shrinking.  Place excess space in a
	 * buffer header taken from the BQ_EMPTY buffer list and
	 * placed on the "most free" list. If no extra buffer
	 * headers are available, leave the extra space in the
	 * present buffer.
	 */
	if (sizealloc < tp->b_bufsize) {
		s = splbio();
		BFREE_LOCK();
		/*
		 * Walk the empty list avoiding locked buffers. On a
		 * uniprocessor, this loop will terminate at the
		 * first buffer on the freelist.
		 */
		dp = &bfreelist[BQ_EMPTY];
		for (ep = dp->av_forw; ep != dp; ep = ep->av_forw) {
			BUF_LOCK_TRY(ep, locked);
			if (!locked) {
				buflocked++;
				continue;
			}
			goto got_one;
		}
		if (ep == dp) {
			BFREE_UNLOCK();
			splx(s);
			tp->b_bcount = size;
			return;
		}
got_one:
		bremfree(ep);
		BFREE_UNLOCK();
		splx(s);
		if (buflocked)
			BUF_STATS(bio_stats.allocbuf_buflocked += buflocked);
		pagemove(tp->b_un.b_addr + sizealloc, ep->b_un.b_addr,
		    (int)tp->b_bufsize - sizealloc);
		ep->b_bufsize = tp->b_bufsize - sizealloc;
		tp->b_bufsize = sizealloc;
		ep->b_flags |= B_INVAL;
		ep->b_bcount = 0;
		LASSERT(BUF_LOCK_HOLDER(ep));
		brelse(ep);
		LASSERT(BUF_LOCK_HOLDER(tp));
		tp->b_bcount = size;
		return;
	}
	/*
	 * More buffer space is needed. Get it out of buffers on
	 * the "most free" list, placing the empty headers on the
	 * BQ_EMPTY buffer header list.
	 */
	while (tp->b_bufsize < sizealloc) {
		take = sizealloc - tp->b_bufsize;
		bp = getnewbuf();
		LASSERT(BUF_LOCK_HOLDER(bp));
		if (take >= bp->b_bufsize)
			take = bp->b_bufsize;
		pagemove(&bp->b_un.b_addr[bp->b_bufsize - take],
		    &tp->b_un.b_addr[tp->b_bufsize], take);
		tp->b_bufsize += take;
		bp->b_bufsize = bp->b_bufsize - take;
		if (bp->b_bcount > bp->b_bufsize)
			bp->b_bcount = bp->b_bufsize;
		if (bp->b_bufsize <= 0) {
			bp->b_dev = (dev_t)NODEV;
#ifdef OSF1_ADFS
                        bp->b_devnode = NONODE;
#endif
			bp->b_error = 0;
			bp->b_flags |= B_INVAL;
		}
		brelse(bp);
	}
	LASSERT(BUF_LOCK_HOLDER(tp));
	tp->b_bcount = size;
}
#endif	/* OSF1_SERVER */

/*
 * Early initialization for the buffer cache.  This must be called
 * before we do the memory layout.
 */
void
bio_startup()
{
	int x;
	boolean_t bad_size = FALSE;
	int found_hippi = 0;
	int found_ipi3 = 0;
	int i;
	int node;
	/* 
	 * Determine the per-node buffer cache size.  
	 */
#ifdef NX
	/* 
	 * First see if we are an IPI-3 node, and if so use
	 * the ipi3_bcache_maxbsize for bcache_maxbsize.  IPI-3
	 * require two things, first a hippi node and second the 
	 * entry must be in the IPI3_NODE_LIST.
	 */
	for(i = 0; i < hippi_node_array_entries; i++) {
		for (node = hippi_node_array[i][0];
		     node <= hippi_node_array[i][1];
		     node++) {
			if( this_node == node ) {
				found_hippi = TRUE;
				break;
			}
		} /* inner for loop */

		if(found_hippi)
			break;

	} /* outer for loop */

	if  (found_hippi) {
		for(i = 0; i < ipi3_node_array_entries; i++) {
			for (node = ipi3_node_array[i][0];
			     node <= ipi3_node_array[i][1];
			     node++) {
				if( this_node == node ) {
					found_ipi3 = TRUE;
					break;
				}
			} /* inner for loop */

			if(found_ipi3)
				break;
		} /* outer for loop */
	}
#endif
	if (found_ipi3) {
		if (ipi3_bcache_maxbsize == 0) {
			ipi3_bcache_maxbsize = MIN_IPI3_BCBSIZE;
		} 
		bcache_maxbsize = ipi3_bcache_maxbsize;

	} else {
		if (bcache_maxbsize == 0) 
			bcache_maxbsize = DFLT_BCBSIZE;
	}

	if (bcache_maxbsize & (bcache_maxbsize - 1)) {
		printf("buffer cache block size %d is not a power of two\n",
		       bcache_maxbsize);
		bad_size = TRUE;
	}
	if (bcache_maxbsize < (found_ipi3 ? MIN_IPI3_BCBSIZE : MIN_BCBSIZE)) {
		printf("buffer cache block size %d is too small\n", 
		       bcache_maxbsize);
		printf("It must be at least %d\n", (found_ipi3 ? 
			MIN_IPI3_BCBSIZE : MIN_BCBSIZE));
		bad_size = TRUE;
	}
	if (bad_size) {
		if (found_ipi3) {
			printf("Default value (%d) will be used.\n", MIN_IPI3_BCBSIZE);
			bcache_maxbsize = MIN_IPI3_BCBSIZE;

		} else {
			printf("Default value (%d) will be used.\n", DFLT_BCBSIZE);
			bcache_maxbsize = DFLT_BCBSIZE;
		}
	}

	/*
         * Now compute secondary lengths and shifts.
	 */
        bcache_maxdgsize = bcache_maxbsize / DISK_GRANULE;
	bcache_maxbshift = 0;
	for (x = bcache_maxbsize; x > 1; x >>= 1)
		bcache_maxbshift++;
        bcache_maxdgshift = bcache_maxbshift - DISK_GSHIFT;
#       undef  DEF_MAXBSIZE  
}

/*
 * Initialize the buffer I/O system by initializing buffer cache
 * hash links, freeing all buffers and setting all device buffer
 * lists to empty.  Assumes uniprocessor mode (boot-time).
 */
void
bio_init()
{
	register struct buf *bp, *dp;
	register struct bufhd *bhp;
	register int i;
#ifndef	OSF1_SERVER
	int base, residual;
	extern task_t first_task;
#else	/* OSF1_SERVER */
	vm_offset_t     buf_addr;
	vm_size_t       buf_size;
	kern_return_t   bio_read_reply();
	kern_return_t   bio_write_reply();
#endif	/* OSF1_SERVER */
	int x;
	boolean_t bad_size = FALSE;

	for (bhp = bufhash, i = 0; i < bufhsz; i++, bhp++) {
		BHASH_LOCK_INIT(bhp);
		bhp->b_forw = bhp->b_back = (struct buf *)bhp;
		BHASH_STAMP(bhp) = 0;
	}
	BFREE_LOCK_INIT();
	for (dp = bfreelist; dp < &bfreelist[BQUEUES]; dp++) {
		dp->b_forw = dp->b_back = dp->av_forw = dp->av_back = dp;
		dp->b_flags = B_HEAD;
	}
#ifdef	OSF1_SERVER /* this comes from uxkern/block_io.c */
	/*
	 * Existing code wants buffers to have some memory.
	 */
	buf_size = round_page(bcache_maxbsize);
	(void) vm_allocate(mach_task_self(),
			   &buf_addr,
			   nbuf*buf_size,
			   TRUE);
#if	0
	(void) vm_pageable(mach_task_self(),
			   buf_addr,
			   nbuf*buf_size,
			   VM_PROT_READ|VM_PROT_WRITE);
#endif
#else	/* OSF1_SERVER */
	base = bufpages / nbuf;
	residual = bufpages % nbuf;
#endif	/* OSF1_SERVER */
	for (i = 0; i < nbuf; i++) {
		bp = &buf[i];
		event_init(&bp->b_iocomplete);
		bp->b_hash_chain = BHASH_NULL;
		bp->b_dev = NODEV;
#ifdef OSF1_ADFS
                bp->b_devnode = NONODE;
#endif
		bp->b_bcount = 0;
		bp->b_rcred = NOCRED;
		bp->b_wcred = NOCRED;
		bp->b_dirtyoff = 0;
		bp->b_dirtyend = 0;
		bp->b_iodone = NULL;
#ifdef	OSF1_SERVER
		bp->b_un.b_addr = (char *)buf_addr + i * buf_size;
		bp->b_bufsize = buf_size;

		/* allocate one reply port per buffer (the Accent way...) */
#ifdef	REPLY_PORT_ALIAS
		reply_hash_enter(&bp->b_reply_port,
				 (char *)bp,
				 bio_read_reply,
				 bio_write_reply);
		
#else
		bp->b_reply_port = mach_reply_port();
		reply_hash_enter(bp->b_reply_port,
				 (char *)bp,
				 bio_read_reply,
				 bio_write_reply);
#endif
		bp->b_optimize_mem = TRUE;
#else   /* OSF1_SERVER */
#if	EXL
/* The actual allocation is one page per buffer.	--- csy	*/
		bp->b_un.b_addr = buffers + i * PAGE_SIZE;
#else
	        bp->b_un.b_addr = buffers + i * bcache_maxbsize;
#endif
 		if (i < residual)
			bp->b_bufsize = (base + 1) * page_size;
		else
			bp->b_bufsize = base * page_size;
#endif	/* OSF1_SERVER */
		bp->b_flags = B_INVAL;
		BUF_LOCKINIT(bp);
		BUF_LOCK(bp);
		brelse(bp);
	}

	b_funnel.b_count = 0;
	b_funnel.b_waiting = FALSE;
	BFUNNEL_LOCK_INIT();
}

/*
 * Print out statistics on the current allocation of the buffer pool.
 * Can be enabled to print out on every ``sync'' by setting "syncprt"
 * above.
 */
void
bufstats()
{
	int i, j, count;
	register struct buf *bp, *dp;
	int *counts;
	static char *bname[BQUEUES] = { "LOCKED", "LRU", "AGE", "EMPTY" };
	int s;
	int size;


	size = ((bcache_maxbsize/CLBYTES) + 1) * 4;

	if ((counts = (int *)malloc(size)) == NULL) 
		panic("bufstats: malloc");

	for (bp = bfreelist, i = 0; bp < &bfreelist[BQUEUES]; bp++, i++) {
		count = 0;
		for (j = 0; j <= bcache_maxbsize/CLBYTES; j++) {
			counts[j] = 0;
		}
		s = splbio();
		BFREE_LOCK();
		for (dp = bp->av_forw; dp != bp; dp = dp->av_forw) {
			counts[dp->b_bufsize/CLBYTES]++;
			count++;
		}
		BFREE_UNLOCK();
		splx(s);
		printf("%s: total-%d", bname[i], count);
		for (j = 0; j <= bcache_maxbsize/CLBYTES; j++)
			if (counts[j] != 0)
				printf(", %d-%d", j * CLBYTES, counts[j]);
		printf("\n");
	}
	free(counts);
}

#if	MACH_LDEBUG
valid_buf_on_chain(bp, dp)
struct buf *bp;
struct buf *dp;
{
	struct bufhd *dpchk;

	if (!BUF_LOCK_HOLDER(bp)) {
		printf("corruption:  unlocked bp 0x%x\n", bp);
		return 0;
	}
	dpchk = (struct bufhd *) dp;
	if (dpchk < bufhash || dpchk >= bufhash + bufhsz) {
		printf("corruption:  dp 0x%x out of range\n", dpchk);
		return 0;
	}
	if (((int)dpchk-(int)bufhash) % sizeof(struct bufhd) != 0) {
		printf("corruption:  dp 0x%x off-center\n", dpchk);
		return 0;
	}
	if (bp < buf || bp >= buf + nbuf) {
		printf("corruption:  bp 0x%x out of range\n", bp);
		return 0;
	}
	if (((int)bp - (int)buf) % sizeof(struct buf) != 0) {
		printf("corruption:  bp 0x%x off-center\n", bp);
		return 0;
	}
	return valid_chain(dp, bp);
}


valid_chain(dp, bp)
struct buf *dp, *bp;
{
	struct buf *bpchk;
	struct buf *last_forw, *last_bpchk;
	int found_bp;

	found_bp = 0;
	last_forw = dp->b_forw;
	last_bpchk = dp;

	for (bpchk = dp->b_forw; bpchk != dp; bpchk = bpchk->b_forw) {
		if (bpchk == bp)
			++found_bp;
		if (last_forw != bpchk) {
			printf("corruption:  bp 0x%x but last_forw 0x%x\n",
			       bpchk, last_forw);
			return 0;
		}
		if (bpchk->b_back != last_bpchk) {
			printf("corruption:  bp 0x%x, b_back 0x%x, last_bpchk 0x%x\n",
			       bpchk, bpchk->b_back, last_bpchk);
			return 0;
		}
		if (bpchk->b_forw->b_back != bpchk) {
			printf("corruption:  bp 0x%x b_forw 0x%x b_forw_back 0x%x\n",
			       bpchk, bpchk->b_forw, bpchk->b_forw->b_back);
			return 0;
		}
		if (bpchk->b_back->b_forw != bpchk) {
			printf("corruption:  bp 0x%x b_back 0x%x b_back_forw 0x%x\n",
			       bpchk, bpchk->b_back, bpchk->b_back->b_forw);
			return 0;
		}
		last_forw = bpchk->b_forw;
		last_bpchk = bpchk;
	}
	if ((bp && !found_bp) || (!bp && found_bp)) {
		printf("corruption:  bp 0x%x not on chain 0x%x\n", bp, dp);
		return 0;
	}
	return 1;
}
#endif	/* MACH_LDEBUG */
