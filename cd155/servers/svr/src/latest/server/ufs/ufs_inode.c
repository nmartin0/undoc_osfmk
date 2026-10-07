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
 * $Log: ufs_inode.c,v $
 * Revision 1.12  1994/11/18  20:45:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/13  20:32:10  dbm
 * Fixed allocation of superblock zone to use MAXBSIZE instead of superblock
 * size.  This was needed because zone shared with file truncate logic.
 *
 * Revision 1.10  1994/06/28  23:12:20  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.9  1994/02/17  17:01:33  brad
 * Merged revision 1.7.4.1 from the R1.2 branch.
 *
 * Revision 1.7.4.1  1994/02/16  04:19:51  brad
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
 * Revision 1.8  1994/01/14  01:18:33  jlitvin
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
 * Revision 1.7  1993/09/02  02:41:11  brad
 * Don't call itrunc_reserved on Fast Path files.  Fix for bug 6401 (also see
 * vfs_vnops.c) from Paul Roy's AD1.0.5 tree.
 *
 * Revision 1.6  1993/07/21  18:33:52  wunder
 * Modified itrunc_reserved to not attempt to truncate space if inode is marked
 * as having been preallocated.  Also added PFS debug statements.
 *
 * Revision 1.5  1993/07/14  18:38:08  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:53:32  cfj
 * Adding new code from vendor
 *
 * Revision 2.29  94/02/03  11:01:49  dnoveck
 *      Changes for per-node buffer-cache block size.
 *           Read/write inodes in other than logical block units.
 *           Changes to itrunc for new indirect logic.
 *           Interface changes to balloc.
 *
 * Revision 2.28  93/10/20  15:31:16  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE-based defines
 *      to their DISK_GRANULE-based corelates.  Change interface to
 *      {vio,data}_{read,write} to be in terms of disk granules.
 *
 * Revision 2.26  93/06/02  17:17:09  rabii
 * 	Add MAPPED_FILES conditional so building without them works (rabii)
 * 
 * Revision 2.25  93/05/13  16:46:15  roy
 * 	Update itrunc and itrunc_reserved to support new size handling.
 * 	Implemented iupdsiz().
 * 	[93/05/05            roy]
 * 
 * Revision 1.4  1993/05/27  03:11:24  wunder
 * Modified itrunc_reserved to not truncate reserved blocks for preallocated
 * files.
 *
 * Revision 1.3  1993/05/06  20:30:50  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:30  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.24  1993/04/06  11:57:05  rabii
 * 	Add proper synchronization between iget and ufs_inactive as to
 * 	avoid panic of i_mode = 0
 *
 * 	Fixed race inactivating and activating inodes (ufs_inactive
 * 	races with ufs_lookup).
 *
 * 	Fixed unlocking of inode write when returning from itrunc when
 * 	getinoquota fails and QUOTA is defined
 *
 * Revision 2.23  93/03/30  16:10:27  roy
 * 	Added VIO_IS_FASTPATH support to itrunc().  Also, all reserved
 * 	block support is now under UFS_NBC ifdef and invoked depending
 * 	on value of VIO_BLK_RESERVE macro.
 * 	[93/03/11            roy]
 * 
 * Revision 2.22  93/02/05  12:09:48  durriya
 * 	initiliase i_devvp in iget() fairly early since it is also used 
 * 	for matching in the inode hash chain.                     (durriya)
 * 
 * Revision 2.21  93/01/08  14:33:09  durriya
 * 	use node # also when looking for an inode match in iget().
 *
 * Revision 1.2  1992/11/30  22:51:03  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:39:32  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:48:18  cfj
 * Bump major revision number.
 *
 * Revision 2.21  1992/10/22  15:42:11  dbm
 * Updated for PFS functionality.
 *
 * Revision 2.20  1992/09/29  16:50:21  rabii
 * 	Change assert in itrunc to printf.
 * 	[92/09/28            roy]
 * 
 * Revision 2.19  92/09/24  16:50:34  rabii
 * 	Minor cleanup of itrunc_reserved.
 * 	[92/09/23            roy]
 * 
 * Revision 2.18  92/09/20  11:25:13  roy
 * 	Mods for mappable files:  added iinit_mf, itrunc calls mf_trunc, 
 * 	fix setting of i_ressize in itrunc and also set i_truesize.
 * 	[92/09/15            roy]
 * 
 * Revision 2.17  92/09/11  09:28:10  rabii
 * 	Move call of itrunc_reserved to ufs_inactive from ufs_reclaim.
 * 	Set iomode explicitly rather than using VIO_SETMODE.
 * 	Fix bug in itrunc related to signed vs. unsigned local variable.
 * 	[92/08/28            roy]
 * 
 * Revision 2.16  92/08/26  12:12:23  loverso
 * 	Modify itrunc and iget to support disk block reservation.
 * 	Call itrunc_reserved from ufs_reclaim.
 * 	Call VIO_SETMODE from iget to set the vnode's iomode.
 * 	[92/07/20            roy]
 * 
 * Revision 2.15  92/07/29  08:27:33  rabii
 * 	itrunc will return EFBIG if growing beyond rlimit.
 * 	[92/07/20            roy]
 * 
 * Revision 2.14  92/07/14  14:53:45  rabii
 * 	Modified calling sequence to data_read and data_write.
 * 	[92/07/10            roy]
 * 
 * Revision 2.13  92/06/08  18:23:09  pjg
 * 	Set v_iomode in the vnode according to the type of file. Defaults
 * 	to VIO_BUF (use the buffer cache) if neither mapped_files or
 * 	fast_path_io are enabled (pjg).
 * 
 * Revision 2.12  92/05/31  18:59:14  loverso
 * 	In itrunc(), growing the file sets the size right in the inode and
 * 	extends direct blocks as necessary, rather than calling VOP_WRITE
 * 	(MAPPED_FILES and UFS_NBC only).
 * 	[92/05/27            roy]
 * 
 * Revision 2.11  92/05/24  14:47:36  pjg
 * 	92/03/10  16:06:30  condict
 * 	Change event_clear to event_init to fix missing lock init.
 * 	[92/05/19            srl]
 * 
 * Revision 2.10  92/03/15  14:41:19  roy
 * 	92/03/03  17:01:51  roy
 * 	Changes to itrunc() for MAPPED_FILES and UFS_NBC.
 * 
 * Revision 2.9  92/03/09  12:49:27  durriya
 * 	Revision 3.8  91/12/18  17:18:21  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.8  92/02/21  16:39:07  durriya
 * 	move GET(PUT)NODE_FROM_INODE to ufs/inode.h  (sjs)
 * 
 * Revision 2.7  92/01/16  16:15:16  roy
 * 	Define and use PUTNODE_IN_INODE & GETNODE_FROM_INODE to 
 * 	add & extract the node number to/from the inode (durriya).
 * 
 * Revision 2.6  92/01/05  19:24:11  roy
 * 	1991/11/12  19:39:04  noemi
 * 	Changed parameters in call to specalloc.
 * 
 * Revision 2.5  91/12/17  08:49:22  roy
 * 	91/10/23  16:38:38  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.4  91/12/13  10:16:19  roy
 * 	91/10/14  20:59:50  roy
 * 	Call data_init() from ufs_init().
 * 
 * Revision 2.3  91/11/26  13:35:41  rabii
 * 	modified iget to set node field of a newly allocated inode to 0
 * 
 * Revision 2.2  91/08/31  14:19:23  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.6  91/08/01  17:00:37  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.17.4.2  91/02/26  12:29:06  gmf
 * 	Cannot simply do iput() if we lose the iget
 * 	race for the hash chain.  Do vgone, instead.
 * 	[91/02/25  18:20:54  gmf]
 * 
 * 	Return error consistently from ufsspec_reclaim.
 * 	[91/02/25  13:37:17  gmf]
 * 
 * Revision 1.17  90/10/31  14:07:42  devrcs
 * 	fix typo in security case
 * 	[90/10/25  09:08:21  gmf]
 * 
 * 	protect vp->v_vm_info references
 * 	[90/10/22  14:41:35  gmf]
 * 
 * 	Copy di_gen from old incore dinode to the fresh
 * 	one before doing bcmp and copying the new
 * 	dinode over the old.
 * 	[90/10/20  14:27:52  gmf]
 * 
 * 	Change itrunc to extend files if new length is longer.  NOTE: this only
 * 	happens if a setattr comes in over nfs to extend -- see corresponding
 * 	change to syscalls in vfs/vfs_syscalls.c.
 * 	[90/10/12  15:15:32  dlb]
 * 
 * 	Changes to mount update to refresh filesystem information.
 * 	-- Changed iget to take an update parameter, which tells
 * 	it to re-read the on-disk inode information for
 * 	already-cached inodes.  Print warning if iget update reads an
 * 	on-disk inode different from that in memory (i.e. fsck changed it).
 * 	-- Changed iupdat to use fs->fs_ronly flag to check
 * 	for rofs, not the M_RDONLY flag.
 * 	[90/10/03  09:28:45  gmf]
 * 
 * Revision 1.16  90/10/07  14:59:08  devrcs
 * 	Don't update mtime or ctime in itimes for a read-only filesystem.
 * 	[90/10/03  13:43:31  morris]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:53:00  gm]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:10:02  collins]
 * 
 * Revision 1.15  90/09/23  16:00:50  devrcs
 * 	Don't iupdat asynchronously in itrunc if the caller requested
 * 	synchronous writes.  Also, changed some comments in iget.
 * 	[90/09/12  15:22:39  noemi]
 * 
 * 	Added comments about quota operations, ufsmount
 * 	quota lock around i_dquot manipulations in ufs_reclaim
 * 	to close so-far hypothetical quotaon race.
 * 	[90/09/08  19:03:56  nags]
 * 
 * 	Removed assertion that failed for inodes with link counts of 0 and
 * 	non-zero modes.  This is a temporary kludge.  We should fix this
 * 	correctly for OSF/1.1
 * 	[90/09/07  13:46:57  noemi]
 * 
 * 	New quota code, based on 4.3BSD-Reno.  Eliminate
 * 	redundant zeroing of icache_stats.
 * 	[90/09/03  22:37:00  nags]
 * 
 * Revision 1.14  90/09/13  11:51:35  devrcs
 * 	In itrunc -- don't do synchronous iupdat if file is being truncated
 * 	to the same size that it already has.
 * 	[90/08/22  14:37:42  gmf]
 * 
 * Revision 1.13  90/08/24  12:28:55  devrcs
 * 	removed u.u_error references
 * 	[90/08/20  12:35:20  gmf]
 * 
 * 	fix parameters to event_wait
 * 	[90/08/19  23:21:42  gmf]
 * 
 * Revision 1.12  90/08/09  13:29:34  devrcs
 * 	If truncating zero length inode to zero, do iupdat asynchronously.
 * 	[90/08/02  13:04:54  gmf]
 * 
 * Revision 1.11  90/07/27  09:09:02  devrcs
 * 	Use INOCACHE_STATS instead of ICACHE_STATISTICS, dmnts.
 * 	[90/07/20  17:07:39  nags]
 * 
 * 	Fix indirtrunc assertion.
 * 	[90/07/17  08:51:26  nags]
 * 
 * 	Change SP_SETOBJNUM to SEC_SETOBJNUM. Also remove bogus assertion.
 * 	[90/07/11  10:39:53  seiden]
 * 
 * Revision 1.10  90/07/17  11:43:12  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  22:04:11  seiden]
 * 
 * Revision 1.9  90/06/29  13:54:26  devrcs
 * 
 * 	Condensed history (reverse chronology):
 * 	Fixed some races and added icache statistics.	nags@encore.com
 * 	Enable icache stats, clean-up debug code.	nags@encore.com
 * 	Handle errors breading/initializing inodes.	nags@encore.com
 * 	Fix iget/ufs_inactive races using INACTWAIT.	nags@encore.com
 * 	Post-nags-merge bug fixes			seiden@osf.org
 * 	nags merge.					nags@encore.com
 * 	Secureware changes.				seiden@osf.org
 * 	Remove pageable flag from zinit argument list.	jvs@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Indirect block truncation fixes.		noemi@osf.org
 * 	Iget handles VFIFO specially.			ers@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Inode_uncache_try value unmounted busy fs's.	gm@osf.org
 * 	Integrated Encore 0.6 parallelization.		gm@osf.org
 * 	  Berkeley 4.4 based with:
 * 	  fast symbolic link support
 * 	  some MACH_NBC code
 * 	  support for Mach VM, as required
 * 	[90/06/26  11:32:18  nags]
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
 *    @(#)ufs_inode.c 7.28 (Berkeley) 2/8/90
 */

#include <ufs_nbc.h>
#include <mapped_files.h>
#include <norma_ipc.h>
#if	MACH
#include <quota.h>
#include <mach_nbc.h>
#include <mach_assert.h>
#include <xpr_debug.h>
#if	INOCACHE_STATS
#include <inocache_stats.h>
#endif
#endif

#include <sys/secdefines.h>
#if	SEC_FSCHANGE
#include <sys/security.h>
#include <sys/secpolicy.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/mount.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#if	QUOTA
#include <ufs/quota.h>
#endif
#include <ufs/inode.h>
#include <ufs/fs.h>
#include <ufs/ufsmount.h>
#include <ufs/icstats.h>
#include <kern/event.h>
#include <sys/lock_types.h>
#include <sys/kernel.h>
#if	MACH
#include <kern/zalloc.h>
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/mfs.h>
#else
#include <sys/malloc.h>
#endif
#ifdef OSF1_ADFS
#include <sys/specdev.h>
#endif

int		prtactive;		/* 1 => print active vnode reclaim */
struct	ihead	*ihead;			/* inode hash chains */

#if	INOCACHE_STATS
struct icache_stats 	icache_stats;
#define	ICSTAT(clause)	STATS_ACTION(&icache_stats_lock, (clause))
vdecl_simple_lock_data(,icache_stats_lock)
#else
#define	ICSTAT(clause)
#endif

/*
 * Generation number manipulation
 */
u_long	nextgennumber;		/* next generation number to assign */
udecl_simple_lock_data(,gen_lock)

#define	GEN_LOCK()		usimple_lock(&gen_lock)
#define	GEN_UNLOCK()		usimple_unlock(&gen_lock)
#define	GEN_LOCK_INIT()		usimple_lock_init(&gen_lock)

/*
 * Conceal some Mach/Unix details.
 */
#if	MACH
#define TEMP_FS_ALLOC(s,size)	ZALLOC(temp_fs_zone, (s), daddr_t *)
#define TEMP_FS_FREE(s)		ZFREE(temp_fs_zone, (s))
#else
#define	TEMP_FS_ALLOC(s,size)	MALLOC((s), daddr_t *, (size), M_TEMP,M_WAITOK)
#define	TEMP_FS_FREE(s)		FREE((s), M_TEMP);
#endif

/*
 * Variables to allow the buffer cache block size to be smaller than the
 * file system block size.
 */
long	ufs_bcmax_nisize;	/* Number of indirect pointers in buffer */
                                /* cache block size. */
int	ufs_bcmax_nishift;	/* Associated shift. */
int	ufs_bcmax_dincount;	/* Number of disk inodes in buffer cache */
                                /* block size. */
int	ufs_bcmax_dinshift;	/* Associated shift. */

/*
 * Variables for io sectioning logic
 */
int	iosecshift[sizeof(long)*NBBY];	/* IO section shift for each logical */
					/* block size. */
long	iosecmask[sizeof(long)*NBBY];	/* Mask for IO section offset for */
					/* each logical block size. */
int	iosecbshift[sizeof(long)*NBBY];	/* Shift between IO section size and */
					/* logical block size for each log- */
					/* ical block size. */
long	iosecdmask[sizeof(long)*NBBY];	/* Mask for relative io section */
					/* number within block for each */
					/* logical block size. */
long    ioseclen[sizeof(long)*NBBY];	/* Length of io section for each */
					/* logical block size. */
/*
 * Zone for temporary copies of blocks.
 */
zone_t	temp_fs_zone;


/*
 * Initialize hash links for inodes.
 */
ufs_init()
{
	register int i;
	register struct ihead *ih = ihead;
	extern int nmount_max;
	int sb_size;
	vdecl_simple_lock_data(extern,blkpref_handy_lock)

	ASSERT((inohsz & inohsz-1) == 0);
	for (i = inohsz; --i >= 0; ih++) {
		IHASH_LOCK_INIT(ih);
		ih->ih_head[0] = ih;
		ih->ih_head[1] = ih;
		ih->ih_timestamp = 0;
	}
#if	UFS_NBC
	data_init();  /* initialize the file data machinery */
#endif

#if	MACH
	/*
	 * superblocks for mounted file systems.  These are used
	 * when mounting file systems and truncating files.
	 */
	if (bcache_maxbsize > SBSIZE) {
		sb_size = bcache_maxbsize;

	} else {
		sb_size = SBSIZE;
	}
		
	superblock_zone = zinit(sb_size,
			      nmount_max*sb_size,
			      MAXBSIZE,
			      "superblocks");
	if (superblock_zone == (zone_t) NULL)
		panic("ufs_init: no superblock zone");
	/*
	 * Create a zone for temporary fs block copies.  The old
	 * practice was to use the superblock zone.  We still do that
	 * unless the buffer cache block size is big in which case we
	 * create our own zone.
	 */
	temp_fs_zone = superblock_zone;
	if (bcache_maxbsize > sb_size) {
		temp_fs_zone = zinit(MAXBSIZE,
				     nmount*MAXBSIZE,
				     MAXBSIZE,
				     "temp ufs blocks");
		if (temp_fs_zone == (zone_t) NULL)
			panic("ufs_init: no temp-fs zone");
	}


	/*
	 * Set up variables to allow block sizes larger than buffer
	 * size.
	 */
	ufs_bcmax_nisize = bcache_maxbsize / sizeof(daddr_t);
	if (ufs_bcmax_nisize & (ufs_bcmax_nisize - 1))
		panic("ufs_init: ufs_bcmax_nisize not power of two");
	ufs_bcmax_nishift = 0;
	for (i = ufs_bcmax_nisize; i > 1; i >>= 1)
		ufs_bcmax_nishift++;
	ufs_bcmax_dincount = bcache_maxbsize / sizeof(struct dinode);
	if (ufs_bcmax_dincount & (ufs_bcmax_dincount - 1))
		panic("ufs_init: ufs_bcmax_dincount not power of two");
	ufs_bcmax_dinshift = 0;
	for (i = ufs_bcmax_dincount; i > 1; i >>= 1)
		ufs_bcmax_dinshift++;
	for (i = 0; i < sizeof(long)*NBBY; i++) {
		if (i <= bcache_maxbshift) {
			iosecshift[i] = i;
			iosecmask[i] = (1 << i) - 1;
			iosecbshift[i] = 0;
			iosecdmask[i] = 0;
			ioseclen[i] = (1 << i);
		}
		else {
			iosecshift[i] = bcache_maxbshift;
			iosecmask[i] = bcache_maxbsize - 1;
			iosecbshift[i] = i - bcache_maxbshift;
			iosecdmask[i] = (1 << iosecbshift[i]) - 1;
			ioseclen[i] = bcache_maxbsize;
		}
	}
#endif
	GEN_LOCK_INIT();
	VSTATS_LOCK_INIT(&blkpref_handy_lock);
	MOUNTTAB_LOCK_INIT();
#if	QUOTA
	dqinit();
#endif
	return (0);
}

extern struct vnodeops ufs_vnodeops, spec_inodeops, fifo_inodeops;
#if SEC_FSCHANGE
extern struct vnsecops ufs_vnsecops;
#endif
#ifdef	PFS
extern struct vnodeops pfs_vnodeops;
#endif	PFS

#if	UFS_NBC
/*
 * Initialize some extra fields in the inode.
 */
iinit_extra(ip)
	register struct inode *ip;
{
	int 		i;
	static int 	ndaddr_div_2 = NDADDR / 2;

	IN_LOCK(ip);
	if (VIO_BLK_RESERVE(ITOV(ip))) {
		/* 
		 * Zero the reserved frag counts for direct blocks.
		 * Each is a short, but we zero two at a time.
		 */
		for (i = 0; i < ndaddr_div_2; i++) 
			*((long *)ip->i_resfrags+i) = 0;
	}
	if (VIO_IS_MAPPED(ITOV(ip))) {
		/*
		 * Initialize the inode's true size field to the on-disk size.
		 * While the inode is active in memory, this field tracks the
		 * file's true size.
		 */
		ip->i_truesize = ip->i_size;

		/*
		 * Initialze the size field tracking the amount of data
		 * that's been written.
		 */
		ip->i_writesize = ip->i_size;
	}
	IN_UNLOCK(ip);
}

#endif	/* MAPPED_FILES */

/*
 * Look up an vnode/inode by device,inumber.
 * If it is in core (in the inode structure),
 * honor the locking protocol.
 * If it is not in core, read it in from the
 * specified device.
 * Callers must check for mount points!!
 * Callers guarantee that the filesystem won't
 * become unmounted; typically, xp is a referenced
 * inode on the target filesystem.
 * In all cases, a pointer to a unlocked
 * inode structure (with an incremented vnode
 * reference count) is returned.
 *
 * If update parameter set, read dinode data from disk unconditionally,
 * to refresh the information in the cache.  Leave non-dinode state
 * as it is; it won't have changed during a mount update.
 */
iget(xp, ino, ipp, update)
	struct inode *xp;
	ino_t ino;
	struct inode **ipp;
	int update;
{
	struct mount *mntp;
	struct fs *fs;
	struct inode *ip, *ip2;
	struct vnode *vp, *nvp;
	struct buf *bp;
	struct dinode *dp;
	struct	ihead *ih;
	u_long iflag;
	int error, mflag, stamp;
	dev_t dev;
#ifdef OSF1_ADFS
        node_t node;
#endif
	int i;
	long bread_size;
	daddr_t bn;
	int index;

	mntp = ITOV(xp)->v_mount;
	if (mntp == DEADMOUNT)
		return (ENODEV);
	fs = VFSTOUFS(mntp)->um_fs;
	dev = xp->i_dev;
#ifdef OSF1_ADFS
        ASSERT(xp->i_devvp != NULL);
        node = xp->i_devvp->v_devnode;
#endif

	ICSTAT(icache_stats.ic_iget_call++);
loop:
	ICSTAT(icache_stats.ic_iget_loop++);
	ih = &ihead[INOHASH(dev, ino)];
	IHASH_LOCK(ih);
	for (ip = ih->ih_chain[0]; ip != (struct inode *)ih; 
             ip = ip->i_forw) {
#ifdef OSF1_ADFS
                ASSERT(ip->i_devvp != NULL);
		if (ino != ip->i_number || dev != ip->i_dev || 
                    (node != ip->i_devvp->v_devnode))
#else
		if (ino != ip->i_number || dev != ip->i_dev)
#endif
			continue;
		IN_LOCK(ip);
		IHASH_UNLOCK(ih);
		/*
		 * Synchronize with inodes being inactivated.  This
		 * is similar to inodes being reactivated from the
		 * name cache (see ufs_lookup()).
		 */
		if (ip->i_flag & INACTIVATING) {
			ip->i_flag |= INACTWAIT;
			assert_wait((int)&ip->i_flag, FALSE);
			IN_UNLOCK(ip);
			thread_block();
			goto loop;
		}
		vp = ITOV(ip);
		/* lock order: inode, then vnode */
		VN_LOCK(vp);
		error = vget_nowait(vp);
		VN_UNLOCK(vp);
		IN_UNLOCK(ip);
		if (error) {
			/*
			 * The inode we seek is undergoing traumatic change.
			 * Wait for that change to complete before going on.
			 */
			if (vget(vp) == 0)
				vrele(vp);
			ICSTAT(icache_stats.ic_iget_vget++);
			goto loop;
		}
		(void) event_wait(&ip->i_iodone, FALSE, 0);
		BM(IN_LOCK(ip));
		iflag = ip->i_flag;
		BM(IN_UNLOCK(ip));
		if (iflag & IREADERROR) {
			iput(ip);
			return(EIO);
		}
		if (update)
			break;
		*ipp = ip;
		ICSTAT(icache_stats.ic_iget_hit++);
		ICSTAT(ip->i_mode == 0 ? icache_stats.ic_iget_reallocd++ : 0);
#ifdef	notyet
		/*
		 * We should really be checking this assertion.  However,
		 * it is currently possible for an NFS client to send us
		 * a file handle for a file that has been unlinked.  The
		 * inode for the file may not have been inactivated.  So
		 * we may find the inode in the cache with a link count
		 * of 0 and a non-zero mode.  The generation number will
		 * have changed. This guarantees that NFS servers will
		 * return an error if they find such inodes in the cache.
		 * No threads will do igets on this inode from UFS because
		 * the file has been unlinked and its vnode is no longer
		 * in the name cache.
		 */
		ASSERT(ip->i_nlink > 0 || ip->i_mode == 0);
#endif
		return(0);
	}
	if (update) {
		ASSERT(ip == xp);	/* inode to be updated is xp */
		nvp = ITOV(ip);
		event_clear(&ip->i_iodone);
		goto updateskip;
	}
	stamp = ih->ih_timestamp;
	IHASH_UNLOCK(ih);
	/*
	 * Allocate a new inode.
	 */
#ifdef	PFS
	if (mntp->m_stat.f_type == MOUNT_PFS) {
		if (error = getnewvnode(VT_UFS, &pfs_vnodeops, &nvp)) {
			*ipp = 0;
			return (error);
		}
	} else
#endif	PFS
	if (error = getnewvnode(VT_UFS, &ufs_vnodeops, &nvp)) {
		*ipp = 0;
		return (error);
	}
	ip = VTOI(nvp);
	ip->i_vnode = nvp;
	ip->i_flag = 0;
	ip->i_devvp = 0;
	ip->i_mode = 0;
	ip->i_diroff = 0;
	ip->i_dirstamp = 0;
	ip->i_forw = ip->i_back = ip;	/* in case we have to drop inode */
	/*
	 * Force other threads that find this inode in the cache to
	 * wait until initialization completes.
	 */
	event_init(&ip->i_iodone);
#if	QUOTA
	for (i = 0; i < MAXQUOTAS; i++)
		ip->i_dquot[i] = NODQUOT;
#endif
	ip->i_dev = dev;
	ip->i_number = ino;
	ip->i_devvp = VFSTOUFS(mntp)->um_devvp;
	VREF(ip->i_devvp);
	IN_LOCK_INIT(ip);
	IN_IO_LOCK_INIT(ip);
	/*
	 * Put the inode onto its hash chain so that other threads will
	 * find it but only if there's not already an identical inode
	 * in the cache.  If the timestamp on the hash chain hasn't
	 * changed, we can skip re-scanning the chain.
	 */
	IHASH_LOCK(ih);
	if (stamp != ih->ih_timestamp) {
		ICSTAT(icache_stats.ic_iget_research++);
		for (ip2 = ih->ih_chain[0]; ip2 != (struct inode *)ih;
		     ip2 = ip2->i_forw) {
#ifdef OSF1_ADFS
                        ASSERT(ip2->i_devvp != NULL);
			if (ino == ip2->i_number && dev == ip2->i_dev &&
                            (node == ip2->i_devvp->v_devnode)) {
#else
			if (ino == ip2->i_number && dev == ip2->i_dev) {
#endif
				struct vnode *tvp = ITOV(ip);
				IHASH_UNLOCK(ih);
				ip->i_flag |= IREADERROR;
				VN_LOCK(tvp);
				vgone(tvp, VX_NOSLEEP, 0);
				VN_UNLOCK(tvp);
				vrele(tvp);
				goto loop;
			}
		}
	}
	insque(ip, ih);
	ih->ih_timestamp++;
	IHASH_UNLOCK(ih);
	ICSTAT(icache_stats.ic_iget_insert++);
updateskip:
	/*
	 * Read in the disk contents for the inode.
	 *
	 * Choose the smaller of the buffer block size and file system
	 * block size.  We assume dinode is a power of two and won't
	 * exceed buffer size.  Note that the latter is not true on a
	 * labelled file system (for security) so this is not supported.
	 */
	bread_size = fs->fs_bsize;
	bn = fsbtodb(fs, itod(fs, ino));
	index = itoo(fs, ino);
	if (bread_size > bcache_maxbsize) {
		bread_size = bcache_maxbsize;
		bn += (index >> ufs_bcmax_dinshift)
			<< (bcache_maxbshift - DISK_GSHIFT);
		index &= (ufs_bcmax_dincount-1);
	}
	if (error = bread(VFSTOUFS(mntp)->um_devvp, bn,
	    (int)bread_size, NOCRED, &bp)) {
		/*
		 * Unlock and discard unneeded inode.
		 * i_mode is set to 0, which will cause anyone
		 * waiting for this inode to realize the inode
		 * is damaged.
		 */
		idrop(ip);
		brelse(bp);
		*ipp = 0;
		ICSTAT(icache_stats.ic_iget_error++);
		ASSERT(!update);	/* let's hope not! */
		return (error);
	}
	/*
	 * We don't need to lock the inode across this initialization
	 * because any other threads finding the inode in the cache sleep
	 * until this thread completes the initialization.
	 */
#if	SEC_FSCHANGE
	nvp->v_secop = &ufs_vnsecops;
	if (FsSEC(fs)) {
		struct sec_dinode       *dp;

		dp = (struct sec_dinode *) bp->b_un.b_dino + index;
		ip->i_din = dp->di_node;
		ip->i_disec = dp->di_sec;
	} else {
		dp = bp->b_un.b_dino + index;
		if (update) {
			dp->di_gen = ip->i_gen;
			if (bcmp(&(ip->i_din), dp, sizeof(struct dinode)))
				printf("WARNING: iget, dinode no. %d changed\n",
					ip->i_number);
		}
		ip->i_din = *dp;
		bzero(&ip->i_disec, sizeof ip->i_disec);
		bcopy(mntp->m_tag, ip->i_tag, sizeof ip->i_tag);
	}
#if	SEC_ILB
	SEC_SETOBJNUM(ip->i_tag, nvp - vnode);
	sp_init_obj_bits(ip->i_tag);
#endif	/* SEC_ILB */
#else	/* !SEC_FSCHANGE */
	dp = bp->b_un.b_dino + index;
	if (update) {
		dp->di_gen = ip->i_gen;
		if (bcmp(&(ip->i_din), dp, sizeof(struct dinode)))
			printf("WARNING: iget, dinode no. %d changed\n",
				ip->i_number);
	}
	ip->i_din = *dp;
#endif	/* !SEC_FSCHANGE */
	brelse(bp);
	/*
	 * If we're updating, we're done.  Finish up and return.
	 */
	if (update) {
		*ipp = ip;
		event_post(&ip->i_iodone);
		vrele(nvp);
		return (0);
	}
	/*
	 * Initialize the associated vnode.  Vnode can't be found
	 * anywhere; if found in the inode cache, callers will wait.
	 */
	vp = ITOV(ip);
	vp->v_type = IFTOVT(ip->i_mode);
#ifdef 	OSF1_ADFS
	/*
	 * For regular files, get the iomode from the mount structure.
	 * Otherwise, the default iomode is to use the buffer cache.
	 */
	vp->v_iomode = vp->v_type == VREG ? mntp->m_iomode : VIO_BUF;
#endif

	if (vp->v_type == VCHR || vp->v_type == VBLK) {
#ifdef	OSF1_ADFS
		if (error = specalloc(vp, ip->i_rdev, 
				      GETNODE_FROM_INODE(ip)))
#else
		if (error = specalloc(vp, ip->i_rdev))
#endif
		{
			/*
			 * Get rid of this bogus inode.  Anyone else
			 * finding this inode in the cache will be
			 * awoken, see that there was an error, and
			 * return EIO themselves.
			 */
			vp->v_type = VNON;
			idrop(ip);
			*ipp = 0;
			return (error);
		}
		vp->v_op = &spec_inodeops;
	} else if (vp->v_type == VFIFO)
		vp->v_op = &fifo_inodeops;
	if (ino == ROOTINO)
		vp->v_flag |= VROOT;
#if	SEC_FSCHANGE
	if (ip->i_type_flags & SEC_I_MLD)
		vp->v_flag |= VMLD;
#endif
	/*
	 * Finish inode initialization.
	 */
	ip->i_fs = fs;
	/*
	 * Set up a generation number for this inode if it does not
	 * already have one.  This should only happen on old filesystems.
	 */
	if (ip->i_gen == 0) {
		ip->i_gen = get_nextgen();
		BM(MOUNT_LOCK(mntp));
		mflag = mntp->m_flag;
		BM(MOUNT_UNLOCK(mntp));
		if ((mflag & M_RDONLY) == 0)
			ip->i_flag |= IMOD;
	}
#if	UFS_NBC
        iinit_extra(ip);	/* init some extra fields in the inode */
#endif

	insmntque(vp, mntp);		/* make it publically available */
	event_post(&ip->i_iodone);
	ASSERT(ip->i_nlink > 0 || ip->i_mode == 0);
	*ipp = ip;
	return (0);
}

/*
 * Decrement the reference count of an inode structure.
 */
iput(ip)
	register struct inode *ip;
{
	vrele(ITOV(ip));
}


/*
 * iget left an inode in a funny state, either
 * because the disk read failed or because it
 * wasn't possible to finish initializing the inode.
 *
 * It is possible that other threads found this inode
 * in the cache so we must wake them up; setting
 * IREADERROR lets them know there was a problem.
 *
 * Eventually everyone will iput/vrele the inode out
 * of existence; ufs_inactive and ufs_reclaim check
 * for IREADERROR.
 *
 * We also remove the inode from its hash chain so that
 * subsequent attempts to find the inode will try
 * all over.
 */
idrop(ip)
register struct inode *ip;
{
	struct ihead	*ih;

	IN_LOCK(ip);
	ip->i_mode = 0;
	ip->i_flag |= IREADERROR;
	IN_UNLOCK(ip);

	ih = &ihead[INOHASH(ip->i_dev, ip->i_number)];
	IHASH_LOCK(ih);
	remque(ip);
	ip->i_forw = ip;		/* make fake hash chain */
	ip->i_back = ip;
	IHASH_UNLOCK(ih);

	event_post(&ip->i_iodone);
	iput(ip);
}


/*
 * Last reference to an inode, write the inode out and if necessary,
 * truncate and deallocate the file.
 *
 * This activity is racy:  this inode could be reactivated by vget
 * through the namei cache or through the vnode mount list.  However,
 * if the link count went to zero, the only way the vnode may be
 * reactivated is through the mount vnode list (sync).
 *
 * Note that the vnode layer guarantees that inactive requests will
 * be serialized but it is still possible (albeit unlikely) to receive
 * multiple inactive requests on an inode that has a 0 link count.
 *
 * Interesting conditions:
 *	IREADERROR
 *	i_nlink <= 0
 *
 * Can't have simultaneous inactives because the count properties
 * guarantee that only one will get through at a time.  However, could
 * have sequential inactives.  (1 would go to 0, calls inactive; vget
 * sends count to 2; first guy finishes inactivating, decrements count
 * to one; second guy would send count to 0, calls inactive.)
 *
 * However, we could be racing vclean.
 *
 * Three possible ways to re-activate:
 *	1.  namei cache -- not an issue for linkcount 0.
 *	2.  file handle, goes through iget
 *	3.  mount vnode list, goes through vget
 */

ufs_inactive(vp)
	struct vnode *vp;
{
	register struct inode *ip = VTOI(vp);
	int mode, error = 0;
	int imode, mflag;
#if	UFS_NBC && MACH_ASSERT
	int i; 
#endif	/* UFS_NBC && MACH_ASSERT */
	struct mount *mp;

	LASSERT(!IN_WRITE_HOLDER(ip));
	mp = vp->v_mount;
	/*
	 * mp will be null only if there was an error in iget prior
	 * to the insmntque call.
	 */
	if (mp != DEADMOUNT) {
		BM(MOUNT_LOCK(mp));
		mflag = mp->m_flag;
		BM(MOUNT_UNLOCK(mp));
	}

	IN_LOCK(ip);
	/*
	 * Uncommon cases:  inode is being inactivated already
	 * or the inode failed to be read correctly from the disk.
	 * The setting of INACTIVATING must be done with the assurance
	 * that the vnode usecount is still 1.  The race here is with
	 * ufs_lookup for inode/vnode reactivation.
	 * Lock order is inode, then vnode.
	 */
	VN_LOCK(vp);
	if (vp->v_usecount != 1) {
		VN_UNLOCK(vp);
		IN_UNLOCK(ip);
		return(0);
	}
	if (ip->i_flag & (INACTIVATING|IREADERROR)) {
		error = ip->i_flag & IREADERROR;
		IN_UNLOCK(ip);
		if (error) {
			/* vnode is locked */
			(void) vgone(vp, VX_NOSLEEP, 0);
		}
		VN_UNLOCK(vp);
		return(0);
	}

	ip->i_flag |= INACTIVATING;
	/*
	 * Get rid of inodes related to stale file handles
	 * or that are not entirely valid (see iget).
	 * Calling vgone will result in calling back into this function.
	 * We avoid recursion with the above check on INACTIVATING.
	 */
	if (ip->i_mode == 0 || mp == DEADMOUNT) {
		IN_UNLOCK(ip);
                /* vnode is locked */
		(void) vgone(vp, VX_NOSLEEP, 0);
		VN_UNLOCK(vp);
		IN_LOCK(ip);
		if (ip->i_flag & INACTWAIT)
			thread_wakeup((int)&ip->i_flag);
		ip->i_flag &= ~(INACTIVATING|INACTWAIT);
		IN_UNLOCK(ip);
		return(0);
	}

        VN_UNLOCK(vp);  /* done with vnode for now */
	imode = ip->i_mode;
	if (ip->i_nlink <= 0 && (mflag & M_RDONLY) == 0) {
		struct inode *localip = ip;  /* for brain-dead gdb */
		IN_UNLOCK(ip);
		/*
		 * Note that this inode could still be
		 * reactivated from the mount vnode list.
		 * Assume such users are only going to do
		 * a sync and are going to "do the right thing".
		 *
		 * Inodes reactivated by file handle translation
		 * call iget, which sleeps while INACTIVATING is set.
		 * When iget finally returns, i_mode is zero, causing
		 * file handle translation to fail.
		 */
#if	QUOTA
		/*
		 * No need to hold inode I/O write lock across
		 * this chkiq call because no one else knows
		 * about this inode.
		 */
		if (!getinoquota(ip))
			(void) chkiq(ip, -1, NOCRED, 0);
#endif
		error = itrunc(ip, (u_long)0, 0);
	 	ASSERT(ip->i_blocks == 0);
		IN_LOCK(ip);
#if	SEC_FSCHANGE
		bzero((caddr_t) &ip->i_disec, sizeof ip->i_disec);
#endif
		mode = ip->i_mode;
		imode = ip->i_mode = 0;
		ip->i_rdev = 0;
		ip->i_flag |= IUPD|ICHG;
#ifdef	PFS
		if (ip->i_flags & (IC_PREALLOCATED))
			ip->i_flags &= ~IC_PREALLOCATED;
#endif	PFS
		IN_UNLOCK(ip);
		ifree(ip, ip->i_number, mode);
	} else {
		IN_UNLOCK(ip);
#if	UFS_NBC
		if (VIO_IS_MAPPED(vp) && (mflag & M_RDONLY) == 0)
			/* 
			 * Free up any extra reserved disk space associated
			 * with the last block. This is necessary because 
			 * space is reserved on a block size granularity 
			 * but the file size is on a byte boundary.
			 *
			 * At this point we know that only the caller has
			 * a vnode reference, and hence there are not any
			 * dirty pages still needing to be written out.
			 */
			itrunc_reserved(ip);
#endif	
	}

	IUPDAT(ip, &time, &time, 0);

#if	UFS_NBC && MACH_ASSERT
	if (VIO_BLK_RESERVE(vp) && (mflag & M_RDONLY) == 0) {
		/* 
     		 * Check that there are no reserved blocks.  We know that all
		 * reserved blocks should have either been written or truncated,
		 * in which case there should be no reserved blocks left.
		 */
		for (i = 0; i < NDADDR; i++) {
			struct inode	*localip = ip;  /* for brain-dead gdb */
			struct vnode	*localvp = vp;

			if (ip->i_resfrags[i] != 0)
				panic("ufs_inactive: I=%d i_resfrags[%d]=%d!=0",
				      ip->i_number, i, ip->i_resfrags[i]);
#ifdef	PFS
			if (!(ip->i_flags & IC_PREALLOCATED) &&
			    (IS_RESERVED(ip->i_db[i])))
#else
			if (IS_RESERVED(ip->i_db[i]))
#endif
				panic("ufs_inactive: I=%d i_db[%d]=%d is res\n",
				      ip->i_number, i, DADDR(ip->i_db[i]));
		}
		/*
		 * Do a quick check of the first indirect block too.
		 */
#ifdef	PFS
		if (!(ip->i_flags & IC_PREALLOCATED)) {
#else
		{
#endif
			daddr_t		bn, *bap;
			int		error;
			struct buf 	*bp;
			struct fs 	*fs = ip->i_fs;

			bn = ip->i_ib[1];
			if (bn != 0 && ip->i_devvp != NULL) {
				long    indir_size = SZINDIR(fs);
				long	bread_size = indir_size;
				int	num_of_pts = NINDIR(fs),
				        loop_count, 
				        j;

				/*
				 * Break up the bread if the buffer size is
				 * smaller than indir block size.
				 */
				if (indir_size > bcache_maxbsize) {

					bread_size = bcache_maxbsize;
					num_of_pts = ufs_bcmax_nisize;
				}
				
				while (indir_size > 0) {
					error = bread(ip->i_devvp, fsbtodb(fs,
				                bn), (int)bread_size, NOCRED, &bp);
					ASSERT(error == 0);
					bap = bp->b_un.b_daddr;
					for (i = 0; i < num_of_pts; i++)
						if (IS_RESERVED(bap[i])) {
						     printf("ufs_inactive: I=%d", 
							    ip->i_number);
						     panic(" i_ib[1]=%d has res blks\n",
							   ip->i_ib[1]);
						}
					bn += bcache_maxbsize >> fs->fs_fshift;
					indir_size -= bread_size;
				}
				brelse(bp);
			}
		}
	}
#endif	/* UFS_NBC && MACH_ASSERT */

	/*
	 * If we are done with the inode, reclaim it
	 * so that it can be reused immediately.
	 */
	IN_LOCK(ip);
	VN_LOCK(vp);
	if (ip->i_flag & INACTWAIT)
		thread_wakeup((int)&ip->i_flag);
	ip->i_flag &= ~(INACTIVATING|INACTWAIT);
	IN_UNLOCK(ip);
	if (vp->v_usecount == 1 && imode == 0) 
		(void) vgone(vp, VX_NOSLEEP, 0);
	VN_UNLOCK(vp);
	return(error);
}

int iupdnot = 0;	/* TEMP: suppress time updates if true */

/*
 * Reclaim a device inode so that it can be used for other purposes.
 *
 * There must not be anyone else who knows about this inode.
 */
ufsspec_reclaim(vp)
	register struct vnode *vp;
{
	int error;
	if (!(error = spec_reclaim(vp)))
		error = ufs_reclaim(vp);
	return(error);
}

/*
 * Reclaim an inode so that it can be used for other purposes.
 *
 * There must not be anyone else who knows about this inode.
 */
ufs_reclaim(vp)
	register struct vnode *vp;
{
	register struct inode *ip = VTOI(vp);
	struct vnode *devvp;
	dev_t dev;
	ino_t ino;
	struct ihead *ih;
#if	QUOTA
	int i;
	struct ufsmount *ump;
#endif

	dev = ip->i_dev;
	ino = ip->i_number;
	ih = &ihead[INOHASH(dev, ino)];
	IHASH_LOCK(ih);
	/*
	 * Remove the inode from its hash chain.
	 * On occasion, the inode may be on a bogus hash-
	 * chain consisting only of itself; e.g., after
	 * an error reading an inode from disk.
	 */
	remque(ip);
	ip->i_forw = ip;
	ip->i_back = ip;
	IHASH_UNLOCK(ih);
	/*
	 * Purge old data structures associated with the inode.
	 */
	cache_purge(vp);
	IN_LOCK(ip);
	if (ip->i_devvp) {
		devvp = ip->i_devvp;
		ip->i_devvp = 0;
		IN_UNLOCK(ip);
		vrele(devvp);
	} else
		IN_UNLOCK(ip);
#if	QUOTA
	/*
	 * No one else knows about this inode/vnode
	 * so there's no concern about clobbering
	 * the i_dquot array.  Note that the quota
	 * routines that walk the mount vnode list
	 * call vget, which synchronizes appropriately
	 * with reclaim.  We must hold the ufsmount
	 * quota lock for reading while disposing of the
	 * dquots to prevent races with quotaon/quotaoff.
	 */
	ump = VFSTOUFS(ITOV(ip)->v_mount);
	UMPQ_READ_LOCK(ump);
	for (i = 0; i < MAXQUOTAS; i++) {
		if (ip->i_dquot[i] != NODQUOT) {
			dqrele(vp, ip->i_dquot[i]);
			ip->i_dquot[i] = NODQUOT;
		}
	}
	UMPQ_READ_UNLOCK(ump);
#endif

	ip->i_flag = 0;
	return (0);
}

#if	MAPPED_FILES
/*
 * If the ISIZ flag is set then the file has grown.  Update the
 * i_size in the on-disk inode, but only if the data in the
 * grown region has been written out.
 *
 * Must be called with the inode locked.
 */
void
iupdsiz(ip)
	register struct inode *ip;
{
	ASSERT(ip->i_flag & ISIZ);
	ASSERT(VIO_IS_MAPPED(ITOV(ip)));
	if (ip->i_truesize <= ip->i_writesize) {
		ip->i_size = ip->i_truesize;
		ip->i_flag &= ~(ISIZ);
		ip->i_flag |= ICHG;   /* indicate inode has changed */
	}
}
#endif

/*
 * Check accessed and update flags on an inode structure.
 * If any is on, update the inode with the current time.
 * If waitfor is given, then must ensure I/O order,
 * so wait for write to complete.
 */
iupdat(ip, ta, tm, waitfor)
	register struct inode *ip;
	struct timeval *ta, *tm;
	int waitfor;
{
	struct buf *bp;
	struct dinode *dp;
	register struct fs *fs;
	int error, ronly;
	register int s, changed;
	daddr_t bn;
	int index;
	long bread_size;

	fs = ip->i_fs;
	BM(IN_LOCK(ip));
#if	MAPPED_FILES
	changed = ((ip->i_flag & (IUPD|IACC|ICHG|IMOD|ISIZ)) != 0);
#else
	changed = ((ip->i_flag & (IUPD|IACC|ICHG|IMOD)) != 0);
#endif
	BM(IN_UNLOCK(ip));
	if (!changed)
		return (0);
	BM(FS_LOCK(fs));
	ronly = fs->fs_ronly;
	BM(FS_UNLOCK(fs));
	if (ronly)
		return (0);
	/*
	 * Choose the smaller of the buffer block size and
	 * file system block size (assuming dinode won't
	 * exceed buffer size).
	 */
	bread_size = fs->fs_bsize;
	bn = fsbtodb(fs, itod(fs, ip->i_number));
	index = itoo(fs, ip->i_number);
	if (bread_size > bcache_maxbsize) {
		bread_size = bcache_maxbsize;
		bn += (index >> ufs_bcmax_dinshift)
			<< (bcache_maxbshift - DISK_GSHIFT);
		index &= (ufs_bcmax_dincount-1);
	}
	error = bread(ip->i_devvp, bn, (int)bread_size, NOCRED, &bp);
	if (error) {
		brelse(bp);
		return (error);
	}
	IN_LOCK(ip);

#if	MAPPED_FILES
	if (ip->i_flag & ISIZ)
		iupdsiz(ip); 	/* update the i_size field */
#endif
	/*
	 * To be perfectly honest, nothing says that the time
	 * being passed in is the "system" time; however, it
	 * usually is so we take the lock around the whole thing.
	 */
	s = splhigh();
	TIME_READ_LOCK();
	if (iupdnot)
		ip->i_flag &= ~(ICHG);
	if (ip->i_flag&IACC)
		ip->i_atime = ta->tv_sec;
	if (ip->i_flag&IUPD)
		ip->i_mtime = tm->tv_sec;
	if (ip->i_flag&ICHG)
		ip->i_ctime = time.tv_sec;
	TIME_READ_UNLOCK();
	splx(s);
	ip->i_flag &= ~(IUPD|IACC|ICHG|IMOD);
#if	SEC_FSCHANGE
	if (FsSEC(fs)) {
		struct sec_dinode       *dp;

		dp = (struct sec_dinode *) bp->b_un.b_dino + index;
		dp->di_node = ip->i_din;
		dp->di_sec = ip->i_disec;
	} else {
		dp = bp->b_un.b_dino + index;
		*dp = ip->i_din;
	}
#else	/* !SEC_FSCHANGE */
	dp = bp->b_un.b_dino + index;
	*dp = ip->i_din;
#endif	/* !SEC_FSCHANGE */
	IN_UNLOCK(ip);
	if (waitfor) {
		return (bwrite(bp));
	} else {
		bdwrite(bp, bp->b_vp);
		return (0);
	}
}


/*
 * We cheat on the time.  In theory, the passed
 * in pointers could refer to any timeval.  In practice,
 * the vast majority of itimes call pass pointers to
 * the system time variable.  So we assume that we are
 * using the system time and once in a while pay the
 * price of doing irrelevant locking when the timeval
 * pointers refer to something else.
 */
itimes(ip, t1, t2)
struct inode *ip;
struct timeval *t1, *t2;
{
	register int s;
	register struct fs *fsp;

	/*
	 * Don't modify times on read-only file system.
	 */
	fsp = ip->i_fs;
	if (fsp == NULL) {
		printf("Trying to set times in a bogus inode\n");
		return;
	}
	FS_LOCK(fsp);
	if (fsp->fs_ronly) {
		FS_UNLOCK(fsp);
		return;
	}
	FS_UNLOCK(fsp);
	IN_LOCK(ip);
	s = splhigh();
	TIME_READ_LOCK();
	if ((ip)->i_flag&(IUPD|IACC|ICHG)) {
		(ip)->i_flag |= IMOD;
		if ((ip)->i_flag&IACC)
			(ip)->i_atime = (t1)->tv_sec;
		if ((ip)->i_flag&IUPD)
			(ip)->i_mtime = (t2)->tv_sec;
		if ((ip)->i_flag&ICHG)
			(ip)->i_ctime = time.tv_sec;
		(ip)->i_flag &= ~(IACC|IUPD|ICHG);
	}
	TIME_READ_UNLOCK();
	splx(s);
	IN_UNLOCK(ip);
}


#define	SINGLE	0	/* index of single indirect block */
#define	DOUBLE	1	/* index of double indirect block */
#define	TRIPLE	2	/* index of triple indirect block */
/*
 * Truncate the inode ip to at most length size.  Free affected disk
 * blocks -- the blocks of the file are removed in reverse order.
 *
 * NB: triple indirect blocks are untested.
 */
itrunc(oip, length, flags)
	register struct inode *oip;
	u_long length;
	int flags;
{
	register daddr_t lastblock;
	daddr_t bn, lbn, lastiblock[NIADDR];
	register struct fs *fs;
	register struct inode *ip;
	struct vnode *vp;
	struct buf *bp;
#if	MACH
	u_long osize, size;
	int offset, level;
#else
	int offset, osize, size, level;
#endif
	long count, nblocks, blocksreleased = 0;
	register int i;
	int waitfor, aflags, error, allerror;
	struct inode tip;
#if	UFS_NBC
	vm_address_t buf;
	int resspace, numgranules;
#endif	

	vp = ITOV(oip);
	fs = oip->i_fs;

#if	MAPPED_FILES 
#if	!UFS_NBC
	itrunc has not been modified to support non-ufs_nbc w/ mapped_files;
#endif

	if (VIO_IS_MAPPED(vp) && (flags & IO_TRUNC) == 0)
		/*
		 * Truncation is handled through the mf module.
		 * 
		 * Note: mf_trunc may in turn call back into itrunc, 
		 * but in that case the IO_TRUNC flag will be set.  
		 * Avoiding such a reentry involves potential 
		 * modification to all itrunc callers (to call 
		 * mf_trunc instead), and hence has poor isolation.
		 *
		 * Upon reentry a write token for the file is held,
		 * thus guaranteeing that i_truesize represents the 
		 * maximum offset of any allocated disk space.
		 */
		return(mf_trunc(vp, length, flags));
#endif
	
	/*
	 *	Fast symbolic links have no storage.  Can truncate in place.
	 */
	if (((oip->i_mode & IFMT) == IFLNK) &&
	    ((oip->i_flags & IC_FASTLINK) != 0) &&
	    (oip->i_size > length)) {
		IN_WRITE_LOCK(oip);
		bzero(&(oip->i_symlink[length]), oip->i_size - length);
		IN_LOCK(oip);
		oip->i_size = length;
		oip->i_flag |= ICHG|IUPD;
		IN_UNLOCK(oip);
		IN_WRITE_UNLOCK(oip);
		error = iupdat(oip, &time, &time, 0);
		return (error);
	}

	/*
	 * Check whether we're growing or shrinking the file.
	 * For mapped files, the length of data on disk is i_writesize.
	 */
	IN_WRITE_LOCK(oip);
#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp)) 
		osize = oip->i_writesize;
	else
#endif
		osize = oip->i_size;

	if (length >= osize) {
#if	SEC_ILB
                /*
                 * Catch here to reset object ILs when the object
                 * is already empty
                 */
                if (osize == 0 && length == 0 && FsSEC(fs))
                        SP_EMPTY_OBJECT(oip->i_tag);
#endif

#if	UFS_NBC
		if (VIO_GROW_DIRECT(vp)) {
			boolean_t	shrink;

			/*
			 * Simply update the size in the inode.  However,
			 * first check if there's a partial direct block
			 * that needs to be extended.  And, must return
			 * EFBIG, if nec.
			 */
			if (length >= (unsigned) 
			              u.u_rlimit[RLIMIT_FSIZE].rlim_cur) {
				IN_WRITE_UNLOCK(oip);
				return(EFBIG);
			}
			if (length > osize) {
				error = balloc_extend_nbc(oip, length, 
					      (flags & IO_SYNC) ? B_SYNC : 0);
				if (error) {
					IN_WRITE_UNLOCK(oip);
					return(error);
				}
			}
			IN_LOCK(oip);
			oip->i_size = length;
			oip->i_flag |= ICHG|IUPD;

			/*
			 * Mapped files may have some reserved blocks
			 * to truncate back.  Otherwise, nothing more to do.
			 */
			if (!VIO_IS_MAPPED(vp)) {
				IN_UNLOCK(oip);
				IN_WRITE_UNLOCK(oip);
				error = iupdat(oip, &time, &time, 
					       (flags & IO_SYNC) ? 1 : 0);
				return(error);
			}

			shrink = blkroundup(fs, oip->i_truesize) > length ?
				 TRUE : FALSE;
			/*
			 * For mapped files, update i_truesize and i_writesize.
			 */
			oip->i_truesize = length;
			oip->i_writesize = length;
			osize = length;			/* needed below */
			IN_UNLOCK(oip);	       

			if (!shrink) {
				IN_UNLOCK(oip);
				IN_WRITE_UNLOCK(oip);
				error = iupdat(oip, &time, &time, 
					       (flags & IO_SYNC) ? 1 : 0);
				return(error);
			}
		} else
#endif
		{
			IN_LOCK(oip);
			if (length == osize) { 
				/*
				 * Not extending.  Don't update synchronously
				 * unless caller asked for it explicitly.
				 */
				int waitfor = 1;

				if ((flags & IO_SYNC) == 0)
					waitfor = 0;

				oip->i_flag |= ICHG|IUPD;
				IN_UNLOCK(oip);
				IN_WRITE_UNLOCK(oip);
				error = iupdat(oip, &time, &time, waitfor);
				return (error);
			} else {
				struct uio uio;
				struct iovec iov;
				int zero = 0, count;

				/*
				 * Write a zero at the end of the file to extend
				 * it.  The write logic does all the dirty work.
				 */
				IN_UNLOCK(oip);
				IN_WRITE_UNLOCK(oip);
				iov.iov_base = (caddr_t) &zero;
				iov.iov_len = 1;
				uio.uio_iov = &iov;
				uio.uio_iovcnt = 1;
				uio.uio_offset = length - 1;
				uio.uio_segflg = UIO_SYSSPACE;
				uio.uio_rw = UIO_WRITE;
				uio.uio_resid = 1;
				VOP_WRITE(vp, &uio, 0, u.u_cred, error);
			}
			return(error);
		}
	} 

	/*
	 * Calculate index into inode's block list of
	 * last direct and indirect blocks (if any)
	 * which we want to keep.  Lastblock is -1 when
	 * the file is truncated to 0.
	 */
	LASSERT(IN_WRITE_HOLDER(oip));
	lastblock = lblkno(fs, length + fs->fs_bsize - 1) - 1;
	lastiblock[SINGLE] = lastblock - NDADDR;
	lastiblock[DOUBLE] = lastiblock[SINGLE] - NINDIR(fs);
	lastiblock[TRIPLE] = lastiblock[DOUBLE] - NINDIR(fs) * NINDIR(fs);
	nblocks = btodg(SZINDIR(fs));
	/*
	 * Update the size of the file. If the file is not being
	 * truncated to a block boundry, the contents of the
	 * partial block following the end of the file must be
	 * zero'ed in case it ever become accessable again because
	 * of subsequent file growth.
	 *
	 * Don't need to zero if we're just truncating reserved blocks
	 * (indicated by length >= osize).
	 */
	offset = blkoff(fs, length);
	if (offset > 0 && length < osize) {
		lbn = lblkno(fs, length);
		aflags = B_CLRBUF;
		if (flags & IO_SYNC)
			aflags |= B_SYNC;
#if	QUOTA
		if (error = getinoquota(oip)) {
			IN_WRITE_UNLOCK(oip);
			return (error);
		}
#endif

#if	UFS_NBC
                if (VIO_GROW_DIRECT(vp)) {
			/*
			 * There is no need to do disk space allocation.  
			 * If the block exists, its size is guaranteed to be
			 * proper (based on the size of the file) because 
			 * allocation was done at file growth time.
			 */
			u_long	save_size;

			if (error = bmap(oip, lbn, &bn)) {
				IN_WRITE_UNLOCK(oip);
				return (error);
			}

			IN_LOCK(oip);
			save_size = oip->i_size;
			oip->i_size = length;	
			IN_UNLOCK(oip);
			size = blksize(fs, oip, lbn);

			if ((long)bn != -1 && size > offset) {
				ASSERT(bn != 0);
				numgranules = btodg(size);
				if (VIO_IS_MAPPED(vp)) {
					error = data_read(fs->fs_devinfo, bn, 
							  numgranules, 0, 0, 
							  &buf);
					if (error) 
						goto handle_error;

					bzero((char *)buf + offset, 
					      (unsigned)(size - offset));
					error = data_write(fs->fs_devinfo, bn, 
							   buf, numgranules, 
							   (flags & IO_SYNC) ? 
							   TRUE : FALSE);
					if (error) 
						goto handle_error;
				} else {
					ASSERT(VIO_IS_FASTPATH(vp));
					error = vio_read(vp, fs->fs_devinfo,
							 lbn, 1, bn, numgranules,
							 TRUE,
							 &buf);
					if (error) 
						goto handle_error;
					ASSERT(buf != NULL);
					bzero((char *)buf + offset, 
					      (unsigned)(size - offset));
					error = vio_write(vp, fs->fs_devinfo,
							  buf, size, lbn, 1,  
							  bn, numgranules, 
							  TRUE,
							  (flags & IO_SYNC) ? 
							  TRUE : FALSE);
					if (error) 
						goto handle_error;
				}
			handle_error:
				if (error) {
					/* restore old size */
					IN_LOCK(oip);
					oip->i_size = save_size;	
					IN_UNLOCK(oip);
					IN_WRITE_UNLOCK(oip);
					return (error);
				}
			}
		} else
#endif	
		{
			ASSERT(ioseclen(fs) <= bcache_maxbsize);
			if (error = balloc(oip, lbn, 0, offset, &bp, aflags)) {
				IN_WRITE_UNLOCK(oip);
				return (error);
			}
			bn = bp->b_blkno;

			IN_LOCK(oip);
			oip->i_size = length;	
			IN_UNLOCK(oip);
			size = blksize(fs, oip, lbn);

			BM(VN_LOCK(vp));
			if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
				BM(VN_UNLOCK(vp));
				inode_uncache(vp);
			} else
				BM(VN_UNLOCK(vp));

			bzero(bp->b_un.b_addr + offset, 
			      (unsigned)(size - offset));
			if (size != bp->b_bcount)
				allocbuf(bp, size);
			if (flags & IO_SYNC)
				bwrite(bp);
			else
				bdwrite(bp, bp->b_vp);
		}
	} 

	/*
	 * Update file and block pointers
	 * on disk before we start freeing blocks.
	 * If we crash before free'ing blocks below,
	 * the blocks will be returned to the free list.
	 * lastiblock values are also normalized to -1
	 * for calls to indirtrunc below.
	 */
	tip = *oip;
	tip.i_size = osize;		/* needed by blksize() below */
	for (level = TRIPLE; level >= SINGLE; level--)
		if (lastiblock[level] < 0) {
			oip->i_ib[level] = 0;
			lastiblock[level] = -1;
		}
	for (i = NDADDR - 1; i > lastblock; i--)
		oip->i_db[i] = 0;
	/*
	 * Update the inode with the new length.
	 */
	IN_LOCK(oip);
	oip->i_size = length;
#if	SEC_ILB
	/* XXX inode locked */
	if (oip->i_size == 0 && FsSEC(fs))
		SP_EMPTY_OBJECT(oip->i_tag);
#endif	/* SEC_ILB */
	oip->i_flag |= ICHG|IUPD;

#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp)) {
		/*
		 * For mapped files, update i_truesize and i_writesize.
		 */
		oip->i_truesize = length;
		oip->i_writesize = length;
	} 
#endif
	IN_UNLOCK(oip);

#if	UFS_NBC
	/*
	 * For files using the VIO module, truncate needs to synchronize
	 * with write-behinds in progress (and read-aheads, if any).
	 * Otherwise, it is possible for a block to be freed, reallocated 
	 * to another file/dir, written, and then a previous write-behind 
	 * completes.  vinvalbuf() does this for VIO_IS_FASTPATH files.
	 *
	 * XXX One wouldn't think this problem would exist for files using
	 * the data_read/data_write interfaces because a write to a newly
	 * allocated block will have to synchronize with write-behinds
	 * (and read-aheads) when data_write is called.  BUT, what if a
	 * freed block get reallocated to a directory, say, in which 
	 * case the subsequent write wouldn't use data_write!  
	 *
	 * However, the iupdat() below causes both a synchronous read 
	 * and a synchronous write, greatly reducing any possibility 
	 * of a write-behind still being in progress when the block
	 * is reallocated.  Also, the possibility of a truncate following
	 * on the heals of a data_write is unlikely because of the fact
	 * that mf_trunc first invalidates main memory data before calling
	 * itrunc().  Lastly, the kernel's disk_sort routine would need to
	 * reorder the disk writes for this situation to occur.  Nonetheless, 
	 * this problem will be fixed when the 'data' interfaces are retired 
	 * in favor of the vio interfaces.
         */		 
#endif

	vinvalbuf(vp, (length > 0));
	allerror = iupdat(oip, &time, &time, MNT_WAIT);

	/*
	 * Don't have to lock ip because it's totally private.  However,
	 * tip is a copy of oip and thus inherited a writelocked I/O lock.
	 *
	 * Indirect blocks first.
	 */
	ip = &tip;
	for (level = TRIPLE; level >= SINGLE; level--) {
		bn = ip->i_ib[level];
		if (bn != 0) {
			error = indirtrunc(ip, bn, lastiblock[level], level,
				&count);
			if (error)
				allerror = error;
			blocksreleased += count;
			if (lastiblock[level] < 0) {
				ip->i_ib[level] = 0;
				blkfree(ip, bn, (off_t)SZINDIR(fs));
				blocksreleased += nblocks;
			}
		}
		if (lastiblock[level] >= 0)
			goto done;
	}

	/*
	 * All whole direct blocks or frags.
	 */
	for (i = NDADDR - 1; i > lastblock; i--) {
		register off_t bsize;
		bn = ip->i_db[i];
		if (bn == 0)
			continue;
#if 	UFS_NBC
#ifdef	PFS
/*
 * There is only one bit available in a block number to indicate that the
 * block is reserved *or* preallocated.  In mapped file block reservation
 * code, if a block is marked reserved then resfrags[n] is never 0.  In block
 * preallocation code, we do not use resfrags and so in fact we can tell that
 * an individual block is preallocated vs. reserved by checking resfrags.
 */
#define IS_PREALLOCATED(ip, daddr) \
	(((ip->i_flags & IC_PREALLOCATED) && (IS_RESERVED(daddr)) && \
	  (ip->i_resfrags[i] == 0)) ? TRUE : FALSE)

		/*
		 * For truncation, preallocated blocks are treated just like
		 * normal blocks, vs. reserved blocks.
		 */
		if ((VIO_BLK_RESERVE(vp)) && (!IS_PREALLOCATED(oip, bn))) {
#else
		if (VIO_BLK_RESERVE(vp)) {
#endif
			/*
			 * Note that modifications to i_resfrags are 
			 * done with oip, not ip.
			 */
			bsize = oip->i_resfrags[i] << fs->fs_fshift;  
			oip->i_resfrags[i] = 0;
			if (!IS_RESERVED(bn)) {
				bsize += (off_t)blksize(fs, ip, i);
			}
			ASSERT(bsize <= fs->fs_bsize);
		} else
#endif
		{
			bsize = (off_t)blksize(fs, ip, i);
		}

		ip->i_db[i] = 0;
		blkfree(ip, DADDR(bn), bsize);
		blocksreleased += btodg(bsize);
	}
	if (lastblock < 0)
		goto done;

	/*
	 * Finally, look for a change in size of the
	 * last direct block; release any frags.
	 */
	bn = ip->i_db[lastblock];
	if (bn != 0) {
		off_t oldspace, newspace;
		/*
		 * Calculate amount of space we're giving
		 * back as old block size minus new block size.
		 */
#if 	UFS_NBC
#ifdef	PFS
		if ((VIO_BLK_RESERVE(vp)) && (!IS_PREALLOCATED(oip, bn))) {
#else
		if (VIO_BLK_RESERVE(vp)) {
#endif
			ASSERT(IS_RESERVED(bn) ? 
			       oip->i_resfrags[lastblock] > 0 : TRUE);
			resspace = oldspace = oip->i_resfrags[lastblock] << 
				              fs->fs_fshift;  
			if (!IS_RESERVED(bn)) {
				oldspace += (off_t)blksize(fs, ip, lastblock);
			}
			ASSERT(oldspace <= fs->fs_bsize);
		} else
#endif
		{
			oldspace = blksize(fs, ip, lastblock);
		}

		ip->i_size = length;
		newspace = blksize(fs, ip, lastblock);
		if (newspace == 0)
			panic("itrunc: newspace");
		if (oldspace - newspace > 0) {
			/*
			 * Block number of space to be free'd is
			 * the old block # plus the number of frags
			 * required for the storage we're keeping.
			 */
			bn = DADDR(bn) + numfrags(fs, newspace);
			blkfree(ip, bn, oldspace - newspace);
			blocksreleased += btodg(oldspace - newspace);
#if	UFS_NBC
#ifdef	PFS
		if ((VIO_BLK_RESERVE(vp)) && (!IS_PREALLOCATED(oip, bn))) {
#else
			if (VIO_BLK_RESERVE(vp)) {
#endif
				resspace -= (int)(oldspace - newspace);
				oip->i_resfrags[lastblock] = resspace > 0 ? 
					numfrags(fs, resspace) : 0;
			}
#endif
		}
	}
done:
/* BEGIN PARANOIA */
	for (level = SINGLE; level <= TRIPLE; level++)
		if (ip->i_ib[level] != oip->i_ib[level])
			panic("itrunc1");
	for (i = 0; i < NDADDR; i++)
		if (ip->i_db[i] != oip->i_db[i])
			panic("itrunc2");

#if 	UFS_NBC && MACH_ASSERT
	if (VIO_BLK_RESERVE(vp) && (length == 0)) {
		struct inode *localip = oip;  /* for brain-dead gdb */
		for (i = 0; i < NDADDR; i++)
			ASSERT(localip->i_resfrags[i] == 0);

		ASSERT(localip->i_blocks - blocksreleased == 0);
	}
#endif
/* END PARANOIA */
	oip->i_blocks -= blocksreleased;
/* this is also PARANOIA that needs to disappear after we find the bug */
	if (oip->i_blocks < 0)			/* sanity */
		oip->i_blocks = 0;
	IN_LOCK(oip);
	oip->i_flag |= ICHG;
	IN_UNLOCK(oip);
#if	QUOTA
	if (!getinoquota(oip))
		(void) chkdq(oip, -blocksreleased, NOCRED, 0);
#endif

	IN_WRITE_UNLOCK(oip);
	return (allerror);
}

/*
 * Release blocks associated with the inode ip and
 * stored in the indirect block bn.  Blocks are free'd
 * in LIFO order up to (but not including) lastbn.  If
 * level is greater than SINGLE, the block is an indirect
 * block and recursive calls to indirtrunc must be used to
 * cleanse other indirect blocks.
 *
 * NB: triple indirect blocks are untested.
 */
indirtrunc(ip, bn, lastbn, level, countp)
	register struct inode *ip;
	daddr_t bn, lastbn;
	int level;
	long *countp;
{
	register int i;
	struct buf *bp;
	register struct fs *fs = ip->i_fs;
	register daddr_t *bap;
	daddr_t *copy, nb, last;
	long blkcount, factor;
	int nblocks, blocksreleased = 0;
	int error, allerror = 0;
	long indir_size;

	LASSERT(IN_WRITE_HOLDER(ip));
	/*
	 * Calculate index in current block of last
	 * block to be kept.  -1 indicates the entire
	 * block so we need not calculate the index.
	 */
	factor = 1;
	for (i = SINGLE; i < level; i++)
		factor *= NINDIR(fs);
	last = lastbn;
	if (lastbn > 0)
		last /= factor;
        indir_size = SZINDIR(fs);
	nblocks = btodg(fs->fs_bsize);

	/* 
	 * If the buffer block size if smaller than the disk block
	 * size, we will have to segment the bread() and bwrite().
	 */

	if (indir_size > bcache_maxbsize) {
		int frag; 
		int temp_last = -1;
		int bread_size = bcache_maxbsize;
		int num_frag = indir_size / bread_size;

		/*
		 * First find out which fragment we truncate to.
		 * temp_last is used to index which fragment
		 * truncation begins.
		 */
		if (last > 0)
			temp_last = last / bread_size;

		TEMP_FS_ALLOC(copy, indir_size);

		/*
		 * For each one of the bread, do the following.
		 */
		for(frag=0; frag < num_frag; frag++) {
			error = bread(ip->i_devvp,fsbtodb(fs, bn), bread_size,
				NOCRED, &bp);
			if (error) {
				brelse(bp);
				*countp = 0;
				return (error);
			}
			bap = bp->b_un.b_daddr;

			/*
			 * Copy each smaller segments into the copy buffer.
			 */
			bcopy((caddr_t)bap, (caddr_t)copy+(frag*bread_size), 
			      (u_int)bread_size);

			/*
			 * If this segment is before the truncation line,
			 * just continue.
			 */
			if (frag < temp_last) {
				bn += bread_size >> fs->fs_fshift;
				continue;
			}

			/*
			 * If this is the fragment that contains the last
			 * truncation, we zero out all the pointers after
			 * the truncating line.
			 */
			if (frag == temp_last) {
				int mark;

				mark = last % bread_size;
				bzero((caddr_t)&bap[mark + 1], 
				      (u_int)(ufs_bcmax_nisize - 
				       (mark + 1)) * sizeof(daddr_t));
			}
			else
				/* 
				 * If this fragment is after truncation mark,
				 * zero out the whole fragment.
				 */
				bzero((caddr_t)&bap[0], (u_int)(bread_size));
					
			if (last == -1)
				bp->b_flags |= B_INVAL;
			error = bwrite(bp);
			if (error)
				allerror = error;
			bn += bread_size >> fs->fs_fshift;
		}
	}
	else {
		
		/*
		* Get buffer of block pointers, zero those
		* entries corresponding to blocks to be free'd,
		* and update on disk copy first.
		*/
		error = bread(ip->i_devvp, fsbtodb(fs, bn), (int)indir_size,
			NOCRED, &bp);
		if (error) {
			brelse(bp);
			*countp = 0;
			return (error);
		}
		bap = bp->b_un.b_daddr;
		TEMP_FS_ALLOC(copy, indir_size);
		bcopy((caddr_t)bap, (caddr_t)copy, (u_int)indir_size);
		bzero((caddr_t)&bap[last + 1],
	  	  (u_int)(NINDIR(fs) - (last + 1)) * sizeof (daddr_t));
		if (last == -1)
			bp->b_flags |= B_INVAL;
		error = bwrite(bp);
		if (error)
			allerror = error;
	}
	bap = copy;

	/*
	 * Recursively free totally unused blocks.
	 */
	for (i = NINDIR(fs) - 1; i > last; i--) {
		nb = DADDR(bap[i]);
		if (nb == 0)
			continue;
		if (level > SINGLE) {
			error = indirtrunc(ip, nb, (daddr_t)-1, level - 1,
				&blkcount);
			if (error)
				allerror = error;
			blocksreleased += blkcount;
		}

		/* 
		 * Free blocks here will depend on are we really
		 * freeing data blocks or indirect blocks, since
		 * one is fs_bsize while the other is fs_fsize *
		 * fs_fragindir.  If level is > SINGLE, that means
		 * we are pointing towards more indirect blocks.
		 */
		if (level > SINGLE) {
			blkfree(ip, nb, (off_t)indir_size);
			blocksreleased += btodg(indir_size);
		}
		else {
			blkfree(ip, nb, (off_t)fs->fs_bsize);
			blocksreleased += nblocks;
		}
	}

	/*
	 * Recursively free last partial block.
	 */
	if (level > SINGLE && lastbn >= 0) {
		last = lastbn % factor;
		nb = DADDR(bap[i]);
		if (nb != 0) {
			error = indirtrunc(ip, nb, last, level - 1, &blkcount);
			if (error)
				allerror = error;
			blocksreleased += blkcount;
		}
	}
	ASSERT(blocksreleased > 0 || ip->i_size > 0);
	TEMP_FS_FREE(copy);
	*countp = blocksreleased;
	return (allerror);
}

#if	UFS_NBC

/*
 * Free any reserved frags associated with the last direct block in
 * the file.  There may be more space reserved than actually used because
 * block reservation takes place on a block size boundary basis whereas
 * a file's size is on a byte boundary.  
 *
 * It's also the case that writes always take place on a page granularity.
 * Hence, there may be some frags between the true size of the file
 * and the written size that need to be freed.
 *
 * Assumption:  All dirty data associated with this inode has been 
 *		written back to the UFS.  
 */
itrunc_reserved(ip)
	register struct inode *ip;
{
	register struct fs 	*fs;
	daddr_t			lastlbn, bn;
	u_long			ressize;
	int			deltasize;

	fs = ip->i_fs;
	IN_WRITE_LOCK(ip);

	/*
	 * Free any reserved frags in the last blk.  
	 */
	IN_LOCK(ip);
	if (ip->i_flag & ISIZ) {
		/*
		 * Make sure i_size reflects the true size.
		 */
		ASSERT(ip->i_truesize <= ip->i_writesize);
		iupdsiz(ip);
	}

	lastlbn = lblkno(fs, ip->i_size);
	if (lastlbn < NDADDR) {
		/* amount of reserved space */
		ressize = ip->i_resfrags[lastlbn] << fs->fs_fshift;

		ASSERT(ip->i_writesize >= ip->i_size);
		deltasize = ip->i_writesize - fragroundup(fs, ip->i_size);
		if (deltasize > 0) {
			ASSERT(deltasize <= vm_page_size);
			ressize += deltasize;
		}

		if (ressize > 0) {
			bn = ip->i_db[lastlbn];
			ASSERT(bn != 0 && !IS_RESERVED(bn));

			if (IS_RESERVED(bn)) {
				/*
				 * This should never happen (unless perhaps the
				 * emulator got trashed).  Try to correct.
				 */
				printf("itrunc_reserved: Reserved blk found\n");
				printf("  I=%d may be bad. size=%d resfr=%d\n",
				       ip->i_number, ip->i_size, 
				       ip->i_resfrags[lastlbn]);
				bn = DADDR(bn);
				ip->i_size -= blkoff(fs, ip->i_size);
				ressize = fs->fs_bsize;
			}

			/* skip past valid frags and free reserved space */
			ASSERT(fragroundup(fs, blkoff(fs, ip->i_size))
			       + ressize <= fs->fs_bsize);
			bn += numfrags(fs, 
				       fragroundup(fs, blkoff(fs, ip->i_size)));
			ip->i_blocks -= btodg(ressize);
			ip->i_flag |= ICHG;
			ip->i_resfrags[lastlbn] = 0;
			IN_UNLOCK(ip);
			blkfree(ip, bn, ressize);	 
			IN_LOCK(ip);
		}
	} 

	/*
	 * Since we're truncating any extra reserved frags,
	 * we may reset i_writesize from i_size.  This is
	 * needed in case the inode is reactivated.
	 */
	if (VIO_IS_MAPPED(ITOV(ip))) {
		ASSERT(ip->i_writesize >= ip->i_size);
		ip->i_writesize = ip->i_size;
	}
	IN_UNLOCK(ip);

	IN_WRITE_UNLOCK(ip);
	return(0);
}

#endif	/* UFS_NBC */

/*
 * Check mode permission on inode pointer. Mode is READ, WRITE or EXEC.
 * The mode is shifted to select the owner/group/other fields. The
 * super user is granted all permissions.
 *
 * NB: Called from vnode op table. It seems this could all be done
 * using vattr's but...
 */
iaccess(ip, mode, cred)
	register struct inode *ip;
	register int mode;
	struct ucred *cred;
{
#if	SEC_ARCH
	udac_t          udac;

	udac.uid = udac.cuid = ip->i_uid;
	udac.gid = udac.cgid = ip->i_gid;
	udac.mode = ip->i_mode & 07777;

	if (SP_ACCESS(SIP->si_tag, ip->i_tag, mode, &udac)) {
		return EACCES;
	}
	return 0;
#else	/* !SEC_ARCH */
	register gid_t *gp;
	uid_t iuid;
	gid_t igid;
	u_short imode;
	int i;

	/*
	 * If you're the super-user, you always get access.
	 */
#if	SEC_BASE
	if (privileged(SEC_ALLOWDACACCESS, 0))
#else
	if (cred->cr_uid == 0)
#endif
		return (0);
	/*
	 * Access check is based on only one of owner, group, public.
	 * If not owner, then check group. If not a member of the
	 * group, then check public access.
	 */
	IN_LOCK(ip);
	iuid = ip->i_uid;
	igid = ip->i_gid;
	imode = ip->i_mode;
	IN_UNLOCK(ip);
	if (cred->cr_uid != iuid) {
		mode >>= 3;
		if (igid == cred->cr_gid)
			goto found;
		gp = cred->cr_groups;
		for (i = 0; i < cred->cr_ngroups; i++, gp++)
			if (igid == *gp)
				goto found;
		mode >>= 3;
found:
		;
	}
	if ((imode & mode) != 0)
		return (0);
	return (EACCES);
#endif	/* !SEC_ARCH */
}

u_long
get_nextgen()
{
	register u_long gen;
	int s;

	GEN_LOCK();
	s = splhigh();
	TIME_READ_LOCK();
	if (++nextgennumber < (u_long)time.tv_sec)
		nextgennumber = time.tv_sec;
	gen = nextgennumber;
	TIME_READ_UNLOCK();
	splx(s);
	GEN_UNLOCK();
	return(gen);
}
