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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: ufs_vfsops.c,v $
 * Revision 1.18  1994/11/18  20:46:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.17  1994/08/31  22:47:22  mtm
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
 * Revision 1.16.2.1  1994/08/25  21:39:10  dbm
 * Removed the mach_port_deallocate that was added earlier for PTS 10109.
 *
 *  Reviewer: Suri
 *  Risk:Low
 *  Benefit or PTS #: 10109
 *  Testing: Tested both remote and local file system mounting after intial
 * 	  mount failed.
 *  Module(s):
 * 	ufs/ufs_vfsops.c
 *
 * Revision 1.16  1994/07/13  20:17:03  dbm
 * Fixed merge problem which caused devices to be marked busy if mount failed.
 *
 *  Reviewer: None.
 *  Risk:Low
 *  Benefit or PTS #:10156
 *  Testing:Specific test case.
 *  Module(s):
 * 	ufs_vfsops.c
 *
 * Revision 1.15  1994/07/13  18:24:45  chrisp
 * In mountfs(), for i386 systems on which device ioctl DIOMRINFO is not
 * supported, default mach_recond_size to retain compatibility.
 *
 *  Reviewer: None
 *  Risk: L
 *  Benefit or PTS #: 10183
 *  Testing: 386 compatibilty verified
 *  Module(s): server/ufs/ufs_vfsops.c, server/sys/param.h
 *
 * Revision 1.14  1994/07/11  20:18:58  dbm
 * Fixed BOOT_IPI3_NODE_LIST bootmagic logic to work correctly.
 * Changed mount to return error instead of panicing when bcache is
 * not configured correctly.
 *  Reviewer: None.
 *  Risk:Low
 *  Benefit or PTS #:10105
 *  Testing: Specific test case, configured IPI3 node.
 *  Module(s):
 *         uxkern/server_init.c
 *         vfs/vfs_bio.c
 *         ufs/ufs_vfsops.c
 *
 * Revision 1.13  1994/07/11  19:26:11  dbm
 * Fixed bug that was not closing the device when a mount operation failed.
 *  Reviewer:
 *  Risk:Low
 *  Benefit or PTS #:10109
 *  Testing: Specific test case.
 *  Module(s): ufs_vfsops.c
 *
 * Revision 1.12  1994/07/06  20:16:17  dbm
 * Fixed incorrect calculation of superblock offset when mounting and umounting
 * file systems.  This was incorrect for devices whose superblock offset is less
 * that SBSIZE.
 *
 * Revision 1.11  1994/06/28  23:14:28  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.10  1994/04/07  20:47:39  dbm
 * Mainline merge of R1.2 version 1.7.2.2
 *
 * Revision 1.9  1994/03/31  17:17:29  dbm
 * Mainline merge of R1.2 version 1.7.2.1
 *
 * Revision 1.8  1994/01/14  01:18:18  jlitvin
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
 * Revision 1.7.2.2  1994/04/07  20:44:14  dbm
 * Added missing #if MAPPED_FILES around call to cach_flush_at_unmount.
 * so that the ramdisk server built correctly.
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #:8905
 *  Testing:Built the ramdisk server.
 *  Module(s):
 * 	ufs/ufs_vfsops.c
 *
 * Revision 1.7.2.1  1994/03/31  17:13:56  dbm
 * Added call to cache_flush_at_umount to flush data caches at unmount.
 *
 *  Reviewer: Paul Roy
 *  Risk: Medium
 *  Benefit or PTS #:8351
 *  Testing: Specific test cases.
 *  Module(s):
 * 	ufs_cache.c
 * 	ufs_vfsops.c
 *
 * Revision 1.7  1993/11/19  19:11:55  bolsen
 *  Checkin done by Brent G. Olsen for Jerry Toman at LCC
 *
 *  Reviewer: Brent Olsen and Charlie Johnson
 *  Risk: low
 *  Benefit or PTS #: 3558 - mount(3) returns wrong errno for two SYSV cases.
 *  Testing: Bug 3558 testcase
 *  Module(s): server/ufs/ufs_vfsops.c
 *
 * Revision 1.6  1993/07/14  18:38:38  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:54:14  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:31:05  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:49:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.21  93/09/13  13:35:14  toman
 * Fixed bug #268: mount(3) returns wrong errno.  Allowed relative pathnames
 * by resetting search directory for second argument before calling namei().
 * 
 * Revision 2.20  93/05/13  16:46:53  roy
 * 	In ufs_sync, also check ISIZ flag for mapped files.
 * 	[93/05/05            roy]
 * 
 * Revision 2.19  93/03/30  16:10:37  roy
 * 	Remove include of fast_path_io.h.
 * 	[93/03/30            roy]
 * 
 * Revision 2.18  93/01/08  14:33:19  durriya
 * 	add node # to dev_lookup call
 * 
 * Revision 2.17  92/12/08  10:47:13  durriya
 * 	1.1 unmount sync changes - s/UNMOUNT_LOCK_INIT/MOUNT_LOOKUP_LOCK_INIT
 * 	eliminate comments about bypassvp                           (durriya)
 * 
 * Revision 1.4  1993/04/03  03:10:46  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.4  1993/03/30  02:31:33  brad
 * Added support for forwarding PFS mount operations when the disk partition
 * to be mounted is remote.
 *
 * Revision 1.1.2.1.2.3  1993/02/04  00:29:55  brad
 * Fixed problem with initializing f_type in mount structure to zero.  Don't
 * do it if the mount is an update, since in this case it won't get reset.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  06:04:03  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:03:56  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:14:34  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.2  1992/11/30  22:51:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:40:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.23  94/02/03  11:02:02  dnoveck
 *      Change mount to allow larger block size to be mounted.
 *      Interface with the per-node buffer-cache block size logic.
 *      Fix bug in which setmount state was not being cleared on an error
 *      in mountfs.
 *
 * Revision 2.22  93/10/20  15:31:25  dnoveck
 *      DEV_BSIZE elimination: Eliminate #if MACH so the code can be
 *      read.  Rewrite mounting tests to distinguish DEV_GRANULE, sector
 *      size and mach-record size.  Interact with new buffer hash logic.
 *      Use dgtob instead of dbtob.
 *
 * Revision 2.16  92/11/05  17:28:35  roy
 * 	don't deallocate devfsport in getmdev for local case. (durriya)
 * 
 * Revision 2.15  92/09/24  16:50:38  rabii
 * 	In ufs_unmount(): set ronly correctly, then, if not ronly, call
 * 	sbupdate() to write out superblock info. (mmp)
 * 
 * Revision 2.14  92/08/26  12:12:28  loverso
 * 	Set the m_iomode field in the mount structure according to the
 * 	flag in mount (pjg).
 * 	[92/08/14            roy]
 * 
 * 	Check additional requirements regarding fs_bsize in mountfs for
 * 	mapped_files.
 * 	[92/08/09            roy]
 * 
 * Revision 2.13  92/07/14  14:53:58  rabii
 * 	Added support for fs_secsize, fs_secmask, and fs_secshift, fs_devinfo.
 * 	[92/07/09            roy]
 * 
 * Revision 2.12  92/06/08  18:23:30  pjg
 * 	Set fsid[1] to contain the node number where the device is physically
 * 	located  (durriya)
 * 
 * Revision 2.11  92/05/18  12:30:53  roy
 * 	Revision 2.9.1.1  92/05/08  12:15:49  roy
 * 	Tweak mapped_files path in ufs_sync.
 * 	[92/05/01            roy]
 * 
 * Revision 2.10  92/04/05  17:04:46  pjg
 * 	Don't deallocate the devfsport in ufs_mount. Call mach_port_deallocate
 * 	instead of remote_vrele to deallocate the devport in getmdev (pjg).
 * 
 * 	Set mountid in the mount structure in mountnfs.     (durriya)
 * 
 * Revision 2.9  92/03/15  14:40:23  roy
 * 	Remove MOUNT_DEBUG code.
 * 
 * Revision 2.8  92/03/09  12:49:57  durriya
 * 	Revision 3.6  91/12/18  17:18:32  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.7  92/03/01  18:44:06  pjg
 * 	92/02/28  16:44:46  noemi
 * 	Changed UFS mount code to use mount structure ports instead of covered
 * 	vnode and file system root vnode ports.  Removed all OSF1_ADFS code
 * 	from the UFS unmount path.
 * 
 * Revision 2.6  92/01/14  10:53:32  roy
 * 	92/01/10  19:46:37  noemi
 * 	Removed debugging printfs
 * 
 * 	92/01/07  17:11:36  noemi
 * 	Changes to handle mounth as a mounthead struct.
 * 
 * Revision 2.5  92/01/05  19:22:58  roy
 * 	91/12/18  21:54:31  noemi
 * 	Several remote mount fixes.  Added debugging code.
 * 
 * 	91/12/11  14:42:21  noemi
 * 	Updated to use norma_port_location_hint.
 * 
 * 	1991/11/12  19:41:09  noemi
 * 	Changed UFS mount and unmount code to use new special files interfaces.
 * 	Replaced RPCs with calls to functions that prepare for and send RPCs.
 * 	Fixed typos (naresh).
 * 
 * 	1991/10/16  00:20:56  noemi
 * 	Changed getmdev to correctly handle remote device names and to
 * 	deallocate a pathname buffer if namei previously returned one.
 * 
 * 	1991/10/14  20:09:45  noemi
 * 	MASSIVE changes to ufs mount and unmount paths.  Major changes to 
 * 	ufs_mount.  Added new ufs_mountfs function.  Changes to mountfs.  
 * 	Rewrote getmdev function.
 * 
 * 	1991/09/22  18:27:52  noemi
 * 	Distributed fileserver changes:  Many changes to ufs_mount and mountfs;
 * 	rewrote getmdev; wrote new ufs_mountfs function; minor changes to
 * 	ufs_mountroot.
 * 
 * Revision 2.4  91/12/17  08:37:50  roy
 * 	91/11/26  15:34:11  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/10/24  16:43:28  jose
 * 	Removed local struct time
 * 
 * 	91/10/23  16:38:38  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.3  91/12/13  10:17:27  roy
 * 	91/12/03  10:49:58  roy
 * 	Added mapped file support to ufs_sync.
 * 
 * Revision 2.2  91/08/31  14:20:58  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:01:30  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.15.4.2  91/09/13  10:04:51  sue
 *	Fixed bugs 2066 and 2052.
 *	[91/09/13  10:03:25  sue]
 *
 * Revision 1.15  90/10/31  14:07:57  devrcs
 * 	Supply target vnode to be mounted on to namei
 * 	as a vnode to bypass during translation,
 * 	preventing deadlock on malicious mount pathname
 * 	combinations.
 * 	[90/10/24  14:47:30  nags]
 * 
 * 	Initialize mount locks for root filesystem.
 * 	[90/10/16  12:39:15  jeffc]
 * 
 * 	changed M_FORCE to M_FMOUNT
 * 	[90/10/13  21:46:01  gmf]
 * 
 * 	Only copy new superblock around lock if the lock is present.
 * 	[90/10/12  15:41:02  gmf]
 * 
 * 	Fix mount update for 4.2 style filesystems.  Must set
 * 	some fields in struct fs during mount update.  These
 * 	were being skipped due to an early return.
 * 	Also cleaned it up a little.
 * 	[90/10/12  14:29:40  gmf]
 * 
 * 	Changes to mount update to refresh filesystem information:
 * 	-- ufs_mount() will do work to invalidate and/or
 * 	   refresh cached file and inode information during
 * 	   mount update.
 * 	-- mountfs() takes an update argument, and uses it.
 * 	-- minor change to ufs_sync() to deal with read-only
 * 	   file systems differently.
 * 	-- added irefresh() function to re-read dinode info.
 * 	-- added a flag (updaterefresh), defaulted to on,
 * 	   which can turn it on or off.
 * 	[90/10/09  13:59:09  gmf]
 * 
 * Revision 1.14  90/10/07  14:59:39  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:53:48  gm]
 * 
 * 	Mark local filesystems for mount.  Take appropriate
 * 	locks around uses of m_flag.
 * 	[90/09/28  12:39:11  nags]
 * 
 * 	Do ufs_sync instead of sbupate for mount update.
 * 	This ensures that everything is done correctly,
 * 	and avoids duplication of code from sync.
 * 	[90/09/24  14:32:46  gmf]
 * 
 * 	Only update superblock if filesystem modified in
 * 	mount update, and use sbupdate, not ufs_sync.
 * 	[90/09/21  11:03:00  gmf]
 * 
 * Revision 1.13  90/09/23  16:01:03  devrcs
 * 	Changed comments in ufs_fhtovp()
 * 	[90/09/12  15:23:29  noemi]
 * 
 * 	Fix last fix. Superblock would fail to be sync'ed to disk
 * 	(with the dirty bit) if the mount update operation doesn't
 * 	include the device name (arg.fspec == NULL).
 * 	[90/09/10  19:34:17  jeffc]
 * 
 * 	Filesystem clean flags:
 * 	- Reject attempts to mount dirty filesystems, unless
 * 	the user forces an override.
 * 	- Sync superblock to disk to prevent filesystem from
 * 	being marked clean on the disk but dirty in memory.
 * 	Initialize all interesting ufsmount quota fields.
 * 	Moved FAKE_INODE_SIZE to inode.h.
 * 	Moved fake_inode_init to ufs_subr.c.
 * 	[90/09/08  19:04:32  nags]
 * 
 * 	Added a comment describing an existing race condition in ufs_fhtovp.
 * 	The correct fix is non-trivial.  We should fix this for OSF/1.1.
 * 	[90/09/07  13:48:50  noemi]
 * 
 * 	Must always include ufs/quota.h before ufsmount.h.
 * 	[90/09/04  08:51:27  nags]
 * 
 * 	New quota locking.  New flags for vflush.
 * 	[90/09/03  22:37:26  nags]
 * 
 * Revision 1.12  90/08/24  12:29:26  devrcs
 * 	Support filesystem clean flags.
 * 	Replaced old, useless quota code with (unparallelized)
 * 	4.3-Reno quota code.
 * 	[90/08/19  00:10:10  nags]
 * 
 * Revision 1.11  90/07/27  09:09:11  devrcs
 * 	Permit swapping on root, VOP_OPEN for clone support.
 * 	[90/07/20  17:07:52  nags]
 * 
 * 	Made mount -u safe (clear clean flag)
 * 	[90/07/09  14:57:36  jeffc]
 * 
 * 	Fix problem with mount update and too many vrele's in one case, and not
 * 	enough in another.
 * 	[90/06/29  14:45:27  nags]
 * 
 * Revision 1.9  90/06/29  13:54:37  devrcs
 * 	Added #include <bufcache_stats.h>
 * 	[90/06/26  11:36:15  nags]
 * 
 * Revision 1.8  90/06/22  20:55:56  devrcs
 * 	nags merge
 * 	[90/06/12  21:42:15  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/10  02:09:51  seiden]
 * 
 * 	In ufs_sync, be more careful about verifying that the vnode we
 * 	have still refers to a ufs node.  Fix from gmf.
 * 	[90/06/07  17:16:18  ers]
 * 
 * 	Remove bcmp in getmdev to fix mount crash for mmax. From nags.
 * 	[90/06/01  12:23:34  brezak]
 * 
 * 	Condensed relevant history:
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Always mark file system dirty on mount			jeffc@osf.org
 * 	Fix for mount hang if superblock is trashed		jvs@osf.org
 * 	clean up ifdefs;					gmf@osf.org
 * 	add function of exportfs to ufs_mount;			gmf@osf.org
 * 	Remove CMUCS_RP code.					gmf@osf.org
 * 	change kmem_alloc's and kmem_free's to kalloc/kfree.	gmf@osf.org
 * 	mount hang fix						gmf@osf.org
 * 	Integrated 4.4BSD file system changes as of 1/5/90 	noemi@osf.org
 * 	Removed call to inode_swap_preference.			gm@osf.org
 * 	Changes for smart fsck support.				gm@osf.org
 * 	Moved resource pausing to CMUCS_RP conditional.		noemi@osf.org
 * 	Added zone for superblocks for mounts;			gmf@osf.org
 * 	[90/06/01  14:00:53  brezak]
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
 *	@(#)ufs_vfsops.c	7.33 (Berkeley) 1/4/90
 */
#if	MACH
#include <quota.h>
#if       BUFCACHE_STATS
#include <bufcache_stats.h>
#endif
#ifdef	i386
#include <cputypes.h>
#endif
#endif

#include <mapped_files.h>
#include <sys/secdefines.h>
#include <norma_ipc.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/namei.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <sys/biostats.h>
#include <sys/ucred.h>
#include <sys/file.h>
#include <sys/disklabel.h>
#include <sys/ioctl.h>
#include <sys/errno.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#include <kern/zalloc.h>
#include <kern/kalloc.h>
#include <ufs/quota.h>
#if	QUOTA
#include <sys/proc.h>
#endif
#include <ufs/fs.h>
#include <ufs/ufsmount.h>
#include <ufs/inode.h>
#include <sys/lock_types.h>
#include <sys/user.h>
#include <kern/mfs.h>
#ifdef	OSF1_ADFS
#include <uxkern/syscall_subr.h>
#include <uxkern/sthread.h>
#include <uxkern/device_utils.h>
#endif
#if	EXL
#include <machine/open.h>
#endif
#include <uxkern/mf.h>

zone_t	superblock_zone;

/*
 * ufs vfs operations.
 */
int ufs_mount();
int ufs_start();
int ufs_unmount();
int ufs_root();
int ufs_quotactl();
int ufs_statfs();
int ufs_sync();
int ufs_fhtovp();
int ufs_vptofh();
int ufs_init();

struct vfsops ufs_vfsops = {
	ufs_mount,
	ufs_start,
	ufs_unmount,
	ufs_root,
	ufs_quotactl,
	ufs_statfs,
	ufs_sync,
	ufs_fhtovp,
	ufs_vptofh,
	ufs_init
};

void irefresh();

/*
 * ufs mount table.
 */
struct ufsmount *mounttab;

/*
 * Called by vfs_mountroot when ufs is going to be mounted as root.
 * Assume that nothing else can be happening in the system, so there's
 * no locking done.
 *
 * Name is updated by mount(8) after booting.
 */
#define ROOTNAME	"root_device"

ufs_mountroot()
{
	register struct mount *mp;
	extern struct vnode *rootvp;
	struct ufsmount *ump;
	register struct fs *fs;
	u_int size;
	int error;

	ZALLOC(mount_zone, mp, struct mount *);
	mp->m_op = &ufs_vfsops;
#if	SEC_FSCHANGE
	mp->m_flag = M_RDONLY | M_SECURE;
#else
	mp->m_flag = M_RDONLY;
#endif
	mp->m_exroot = 0;
	mp->m_mounth = (struct vnode *)0;
#if	SER_COMPAT
	mp->m_funnel = FUNNEL_NULL;
#endif
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_VLIST_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);
	error = mountfs(rootvp, mp, 0);
	if (error) {
		ZFREE(mount_zone, mp);
		return (error);
	}
	mp->m_next = mp;
	mp->m_prev = mp;
	mp->m_vnodecovered = (struct vnode *)0;
#if	SEC_FSCHANGE
	if (error = sec_ufsmountcheck(mp)) {
		(void) ufs_unmount(mp, 0);
		ZFREE(mount_zone, mp);
		rootfs = NULL;
		return (error);
	}
#endif
	ump = VFSTOUFS(mp);
	fs = ump->um_fs;
	bzero(fs->fs_fsmnt, sizeof(fs->fs_fsmnt));
	fs->fs_fsmnt[0] = '/';
	bcopy((caddr_t)fs->fs_fsmnt, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
	(void) copystr(ROOTNAME, mp->m_stat.f_mntfromname, MNAMELEN - 1, &size);
	bzero(mp->m_stat.f_mntfromname + size, MNAMELEN - size);
	/*
	 * To ease conversion from formats such as Intel T11+, we set
	 * fs_fsbtodb to the value we need.  The user should then run
	 * fsck -C to update the disk.  By setting fs_fsbtodb now, he
	 * can at least get far enough to run fsck.
	 */
	if (fs->fs_fsbtodb != fs->fs_fshift - DISK_GSHIFT) {
		int i = fs->fs_fsbtodb;

		printf("Root superblock has fs_fsbtodb of %d.\n", i);
		printf("Proper value on this system is %d.\n",
		       fs->fs_fshift - DISK_GSHIFT);
		printf("Superblock temporarily modifed to ease conversion.\n");
		printf("Run fsck -C on root before doing mount -u.\n");
		printf("Run fsck -C on other file systems before mounting.\n");
		fs->fs_fsbtodb = fs->fs_fshift - DISK_GSHIFT;
	}
	/*
	 * prime the struct statfs in the mount structure
	 */
	(void) ufs_statfs(mp);
	rootfs = mp;
#ifdef	OSF1_ADFS
	mp->m_next = (struct mount *)&mounth;
	mp->m_prev = (struct mount *)&mounth;
	mounth.m_next = mp;
	mounth.m_prev = mp;
#endif
	inittodr(fs->fs_time);
	return (0);
}

/*
 * VFS Operations.
 *
 * mount system call
 *
 * Synchronization assumptions:
 *	-- Mount structure could be on global list (if M_UPDATE), so
 *	   we need to lock (sigh).
 *	-- Other attempted mounts on this directory are locked out by
 *	   mount().
 * We are responsible for detecting races on mounting the device passed in.
 * N.B.  Expect ndp to contain a pointer to the vnode to be covered in ni_vp.
 */
int updaterefresh = 1;
#ifdef	PFS
extern struct vfsops pfs_vfsops;
#endif

ufs_mount(mp, path, data, ndp)
	register struct mount *mp;
	char *path;
	caddr_t data;
	struct nameidata *ndp;
{
	struct vnode *devvp;
#ifdef OSF1_ADFS
	struct ufs_args *args;
#else
	struct ufs_args args;
#endif
	struct ufsmount *ump;
	register struct fs *fs;
	u_int size;
	int error;
#ifdef	PFS
	int	type = (mp->m_op == &pfs_vfsops) ? MOUNT_PFS : MOUNT_UFS;
	struct statpfs		*stripe_args, *stripe_attr;
	int			stripe_attr_len;
	struct uio		uio;
	struct iovec		iov;
#endif

#ifdef	OSF1_ADFS
	mach_port_t remoteport = MACH_PORT_NULL, devfsport = MACH_PORT_NULL;
	mach_port_t mountport;
	dev_t	rdev;
	int flag;
#ifdef	PFS
	/*
	 * PFS hacks required since current OSF/1AD VFS mount code is not file
	 * system independent.
	 *
	 * Check if this is a PFS mount.
	 */
	if (type == MOUNT_PFS) {
		args = &((struct pfs_args *)data)->fs_args;
		stripe_args = &((struct pfs_args *)data)->stripe_attr;
		stripe_attr_len = stripe_args->p_reclen;
	} else
#endif
	args = (struct ufs_args *)data;
#else
	if (error = copyin(data, (caddr_t)&args, sizeof (struct ufs_args)))
		return (error);
#endif


	ASSERT((mp->m_vnodecovered == NULLVP) ||
		(mp->m_vnodecovered->v_flag & VMOUNTPOINT));

	/*
	 * Process export requests.
	 */
	MOUNT_LOCK(mp);
#ifdef	OSF1_ADFS
	if ((args->exflags & M_EXPORTED) || (mp->m_flag & M_EXPORTED)) {
		if (args->exflags & M_EXPORTED)
			mp->m_flag |= M_EXPORTED;
		else
			mp->m_flag &= ~M_EXPORTED;
		if (args->exflags & M_EXRDONLY)
			mp->m_flag |= M_EXRDONLY;
		else
			mp->m_flag &= ~M_EXRDONLY;
		mp->m_exroot = args->exroot;
	}
#else
	if ((args.exflags & M_EXPORTED) || (mp->m_flag & M_EXPORTED)) {
		if (args.exflags & M_EXPORTED)
			mp->m_flag |= M_EXPORTED;
		else
			mp->m_flag &= ~M_EXPORTED;
		if (args.exflags & M_EXRDONLY)
			mp->m_flag |= M_EXRDONLY;
		else
			mp->m_flag &= ~M_EXRDONLY;
		mp->m_exroot = args.exroot;
	}
#endif

	if ((mp->m_flag & M_UPDATE) == 0) {
#if	SER_COMPAT
		mp->m_funnel = FUNNEL_NULL;
#endif
#ifdef	OSF1_ADFS
		flag = mp->m_flag;
#endif
#ifdef	PFS
		/*
		 * Initialize f_type so ufs_statfs() knows whether to set it
		 * or not.
		 */
		mp->m_stat.f_type = MOUNT_NONE;
#endif
		MOUNT_UNLOCK(mp);
#ifdef OSF1_ADFS
		/*
		 * If the block device is local, obtain its special file vnode.
		 * If the device is remote, obtain a file structure port
		 * for the device on the remote node.
		 */
		error = getmdev(&devvp, args->fspec, ndp, flag, &rdev,
				&devfsport);
		if (error == EREMOTE) {
			ASSERT(devfsport != MACH_PORT_NULL);
			/*
			 * The block device is remote.  Allocate a port for
			 * this mount structure and let the remote node perform
			 * the actual mount and return the remote mount
			 * structure port.
			 */
			mount_port_allocate(mp, &mountport);
#ifdef	PFS
			if (type == MOUNT_PFS)
				error = remote_pfs_mountfs(devfsport,
							   mountport, path,
							   args->fspec, flag,
							   stripe_args,
							   &remoteport);
			else
#endif
			error = remote_ufs_mountfs(devfsport, mountport,
				   path, args->fspec, flag, &remoteport);
			if (error) {
				/*
				 * Deallocate the mount structure port.
				 */
				 mount_port_deallocate(mp);
			}
			/*
			 * The device file structure port is no longer needed,
			 * so deallocate it.
			 */
			/*
			 * XXX This is still not right. We may be leaking
			 * the port, but it can't be deallocated here (pjg).
			 *
			mach_port_deallocate(mach_task_self(), devfsport);
			 *
			 */
			if (error)
				return(error);
			ASSERT(remoteport != MACH_PORT_NULL);
			/*
			 * Mark the mount structure indicating that the
			 * filesystem is on a remote node and save the
			 * remote mount structure port.
			 */
			MOUNT_LOCK(mp);
			mp->m_flag |= M_REMOTE_FS;
			mp->m_remoteport = remoteport;
			MOUNT_UNLOCK(mp);

			/*
			 * Partially set up statfs structure in mount structure
			 * so getfsstat works right.
			 */
#ifdef	PFS
			if (type == MOUNT_PFS)
				mp->m_stat.f_type = MOUNT_PFS;
			else
#endif
			mp->m_stat.f_type = MOUNT_UFS;
			mp->m_stat.f_flags = mp->m_flag;
			(void) copystr(path,(caddr_t)mp->m_stat.f_mntonname,
				MNAMELEN, &size);
			(void) copystr(args->fspec, mp->m_stat.f_mntfromname,
				MNAMELEN, &size); 
			/*
			 * Upper layer doesn't need to know about EREMOTE.
			 */
			return(0);
		} else if (error) {
			ASSERT(devfsport == MACH_PORT_NULL);
			return(error);
		}
		ASSERT(devfsport == MACH_PORT_NULL);
		if ((error = mountfs(devvp, mp, 0)) != 0) {
			vrele(devvp);
			return(error);
		}
#ifdef	PFS
		if (type == MOUNT_PFS) {
			/*
			 * Allocate a statpfs structure.
			 */
			ASSERT(mp->m_statpfs == NULL);
			stripe_attr = (struct statpfs *)kalloc(stripe_attr_len);
			if (stripe_attr == NULL) {
				(void) ufs_unmount(mp, 0);
				return(ENOMEM);
			}
		}
#endif	PFS

#else	/* OSF1_ADFS */
		if ((error = getmdev(&devvp, args.fspec, ndp)) != 0)
			return (error);
		if ((error = mountfs(devvp, mp, 0)) != 0) {
			vrele(devvp);
			return(error);
		}
#endif 	/* OSF1_ADFS */

#if	SEC_FSCHANGE
		if (error = sec_ufsmountcheck(mp)) {
			(void) ufs_unmount(mp, 0);
			return error;
		}
#endif
		fs = (VFSTOUFS(mp))->um_fs;
	} else {
		int flag = mp->m_flag;
#ifdef	OSF1_ADFS
		ASSERT((flag & M_REMOTE_FS) == 0);
#endif
		ump = VFSTOUFS(mp);
		MOUNT_UNLOCK(mp);
		if (!ump)
			return (ENODEV);
		fs = ump->um_fs;
		FS_LOCK(fs);
		if (fs->fs_ronly) {
			int s;

			/* make it switchable behavior */
			if (!updaterefresh)
				goto norefresh;
			else
				FS_UNLOCK(fs);
			/*
			 * Things to do to update the mount:
			 * 1) invalidate all cached file data.
			 * 2) invalidate all cached meta-data (devvp).
			 *    (done in mountfs()).
			 * 3) re-read superblock and cylinder group
			 *    information from disk (mountfs).
			 * 4) invalidate all inactive vnodes.
			 * 5) re-read inode data for all active vnodes.
			 *
			 * If changing from read-only to read-write:
			 * 6) change struct fs and update to disk.
			 *
			 * Synchronization note:
			 * Since the M_RDONLY flag may have already been
			 * cleared, there could be opens happening for
			 * write while we work.  This is OK, because
			 * no data will be flushed until the fs_ronly
			 * flag is cleared (see iupdat and ufs_sync), and 
			 * all in-core data and meta-data is about to
			 * be flushed or refreshed.
			 */
			devvp = ump->um_devvp;
			if (mntinvalbuf(mp))
				printf("ufs_mount: mntinvalbuf: dirty fs");
			/*
			 * Use mountfs, with update flag, to re-read
			 * meta-data into superblock.
			 */
			if (mountfs(devvp, mp, ump))
				panic("ufs_mount: cannot re-mount fs");
			/*
			 * Flush inactive vnodes and 
			 * re-read inode data for active vnodes.
			 * Must happen before clearing fs_ronly flag.
			 */
			irefresh(mp);
			/*
	 	 	 * Save clean flag; unconditionally restored
			 * unmount so save it here.  
			 * Set fs_clean to 0 if mounting writable.
		 	 */
			FS_LOCK(fs);
norefresh:
			if ((flag & M_RDONLY) == 0) {
				fs->fs_ronly = 0;
				if (fs->fs_fmod != 0)
					panic("ufs_mount; rofs mod");
				fs->fs_flags = fs->fs_clean;
				fs->fs_clean = 0; /* mnted writable ==> dirty */
				/*
		 	 	 * Get the modified superblock written to disk.
		 	 	 */
				s = splhigh();
				TIME_READ_LOCK();
				fs->fs_time = time.tv_sec;
				TIME_READ_UNLOCK();
				splx(s);
				FS_UNLOCK(fs);
				(void) sbupdate(ump, MNT_WAIT);
				FS_LOCK(fs);
			}
		}
		FS_UNLOCK(fs);
		/*
		 * Verify that the specified device is the one that
		 * is really being used for the root file system.
		 * It's possible for an alias to be in use, so we
		 * compare the rdev fields, not the vnodes.
		 */
#ifdef	OSF1_ADFS
		if (args->fspec == 0)
			return (0);
#else
		if (args.fspec == 0)
			return (0);
#endif

#ifdef	OSF1_ADFS
		error = getmdev(&devvp, args->fspec, ndp, flag, &rdev,
				&devfsport);
		/*
		 * Mount update should take place on the node that services
		 * the device.  So getmdev shouldn't have returned a
		 * devfsport.
		 */
		ASSERT(devfsport == MACH_PORT_NULL);
		if (!error)
			ASSERT(devvp != NULLVP);
#else
		error = getmdev(&devvp, args.fspec, ndp);
#endif
		if (!error) {
			if (devvp->v_rdev != ump->um_devvp->v_rdev) {
				error = EINVAL;	/* needs translation */
			}
			vrele(devvp);
		} else if (error == EREMOTE)
			error = (rdev == ump->um_devvp->v_rdev) ? 0 : EINVAL;
		if (error)
			return (error);
#ifdef	PFS
		if (type == MOUNT_PFS) {
			/*
			 * Stripe attributes to be updated are in the mount
			 * structure.
			 */
			stripe_attr = mp->m_statpfs;
			ASSERT(stripe_attr != NULL);
		}
#endif	PFS
	}
	/*
	 * fs is set correctly
	 */
#ifdef	OSF1_ADFS
	(void) copystr(path, fs->fs_fsmnt, sizeof(fs->fs_fsmnt) - 1, &size);
#else
	(void) copyinstr(path, fs->fs_fsmnt, sizeof(fs->fs_fsmnt) - 1, &size);
#endif
	bzero(fs->fs_fsmnt + size, sizeof(fs->fs_fsmnt) - size);
	bcopy((caddr_t)fs->fs_fsmnt, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
#ifdef	OSF1_ADFS
	(void) copystr(args->fspec, mp->m_stat.f_mntfromname, MNAMELEN - 1, 
		&size);
#else
	(void) copyinstr(args.fspec, mp->m_stat.f_mntfromname, MNAMELEN - 1, 
		&size);
#endif
	bzero(mp->m_stat.f_mntfromname + size, MNAMELEN - size);
	(void) ufs_statfs(mp);
#ifdef	PFS
	if (type == MOUNT_PFS) {
		/*
		 * Continue hacking.  If only we had a choice.  Reset the FS
		 * type (set by ufs_statfs) and the I/O mode (set by mountfs).
		 */
		mp->m_stat.f_type = MOUNT_PFS;
		mp->m_iomode = VIO_PFS;

		/*
		 * Initialize (or update) the stripe attributes in the mount
		 * structure.  Stripe attributes are copied in by forging a
		 * uio structure to hand to uiomove.  We can't just use bcopy()
		 * here, because we're copying out-of-line user data that could
		 * be backed by an untrusted pager.
		 */
		iov.iov_base = (caddr_t) stripe_args;
		iov.iov_len = stripe_attr_len;
		uio.uio_iov = &iov;
		uio.uio_iovcnt = 1;
		uio.uio_offset = 0;
		uio.uio_resid = stripe_attr_len;
		uio.uio_segflg = UIO_SYSSPACE;
		uio.uio_rw = UIO_WRITE;
		error = uiomove((caddr_t)stripe_attr, stripe_attr_len, &uio);
		if (error) {
			ufs_unmount(mp, 0);
			if ((mp->m_flag & M_UPDATE) == 0)
				kfree(stripe_attr, stripe_attr_len);
			return(error);
		}
		stripe_attr->p_magic = PFS_MAGIC;
		mp->m_statpfs = stripe_attr;
	}
#endif	PFS
	return (0);
}

#ifdef	OSF1_ADFS
/*
 * This routine handles remote mount requests received.
 */
ufs_mountfs(devvp, flag, remoteport, dir, spec, mountportp)
	struct vnode 	*devvp;
	int 		flag;
	mach_port_t 	remoteport;
	char		*dir;
	char		*spec;
	mach_port_t 	*mountportp;
{
	struct mount *mp;
	register struct fs *fs;
	int	error;
	u_int 	size;

	ASSERT(devvp != NULLVP);
	ASSERT((flag & M_REMOTE_FS) == 0);
	/*
	 * Allocate and initialize a mount structure.
	 */

	MOUNT_ALLOCATE(mp);
	mp->m_op = vfssw[MOUNT_UFS];
	mp->m_flag = (flag | M_REMOTE_DIR);
	mp->m_exroot = 0;
#ifdef	PFS
	/*
	 * Initialize f_type so ufs_statfs() knows whether to set it or not.
	 */
	mp->m_stat.f_type = MOUNT_NONE;
#endif
	mp->m_mounth = (struct vnode *)0;
	mp->m_remoteport = remoteport;
	mp->m_vnodecovered = (struct vnode *)0;
#if     SER_COMPAT
	mp->m_funnel = !FUNNEL_NULL;    /* XXX */
#endif
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_VLIST_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);

	/*
	 * Let mountfs do the bulk of the work.
	 */
	if ((error = mountfs(devvp, mp, 0)) != 0) {
		/*
		 * devvp is vrele'd by the marshalling code.
		 */
		MOUNT_DEALLOCATE(mp);
		return(error);
	}

	/*
	 * Allocate a port for the mount structure.
	 */
	mount_port_allocate(mp, mountportp);

	/*
	 * Set up the statfs structure in the mount structure.
	 */
	fs = (VFSTOUFS(mp))->um_fs;
	(void) copystr(dir, fs->fs_fsmnt, sizeof(fs->fs_fsmnt) - 1, &size);
	bzero(fs->fs_fsmnt + size, sizeof(fs->fs_fsmnt) - size);
	bcopy((caddr_t)fs->fs_fsmnt, (caddr_t)mp->m_stat.f_mntonname, MNAMELEN);
	(void) copystr(spec, mp->m_stat.f_mntfromname, MNAMELEN - 1, 
		&size);
	bzero(mp->m_stat.f_mntfromname + size, MNAMELEN - size);
	(void) ufs_statfs(mp);

	/*
	 * Add the new mount structure to the mount list.
	 */
	MOUNTLIST_LOCK();
	mp->m_next = mounth.m_next;
	mp->m_prev = (struct mount *)&mounth;
	mounth.m_next = mp;
	mp->m_next->m_prev = mp;
	MOUNTLIST_UNLOCK();
	/*
	 * Enable lookups on this filesystem.
	 */
	return(0);
}
#endif	/* OSF1_ADFS */

/*
 * Common code for mount and mountroot
 *
 * Synchronization assumptions:
 *	-- mp is NOT on global list (mount update doesn't get here), so
 *	   no mount structure locking required.
 *	-- Other attempted mounts, including forcible ones on this 
 *	   directory are locked out by mount().
 *	-- No lock needed on fs->*, since it's a new mount, except for
 *	   update case.
 */
mountfs(devvp, mp, update)
	struct vnode *devvp;
	struct mount *mp;
	struct ufsmount *update;
{
	register struct ufsmount *ump = NULL;
	struct buf *bp = NULL;
	struct devinfo *info = NULL;
	register struct fs *fs;
	struct vnode *vp;
	caddr_t base, space;
	int blks;
	int error, i, size;
	int needclose = 0;
	int setmounted = 0;
	int ronly = (mp->m_flag & M_RDONLY) != 0;
	extern vm_size_t page_mask;
#       define ERROR(x)		{ error = (x); goto out; }
	int	mach_record_size;
	int	sbsize,sboff;

	/*
	 * Disallow multiple mounts of the same device.
	 * Disallow mounting of an open block device.
	 * Allow mounting of an open character device.
	 * Flush out any old buffers remaining from previous use.
	 */
	if (vinvalbuf(devvp, 1) && update)
		panic("mountfs: modified meta-data");

	if (update) {
		/*
		 * Leave FS read-only until we're ready to change it,
		 * which is after meta-data in inode cache has been
		 * refreshed.
		 */
		ronly = 1;
		ump = update;
		goto updateskip;
	}

	if (devvp != rootvp) {
		/*
	 	 * getmdev opened the device:
	 	 */
		needclose = 1;
	}

	MOUNTTAB_LOCK();
	for (ump = &mounttab[0]; ump < &mounttab[nmount]; ump++) {
		if (ump->um_fs == NULL) 
			break;
	}
	if (ump >= &mounttab[nmount]) {
		MOUNTTAB_UNLOCK();
		ump = NULL;
		ERROR(EMFILE);			/* needs translation */
	}
	ump->um_fs = (struct fs *)1;		/* just to reserve this slot */
	MOUNTTAB_UNLOCK();
	/*
	 * Opens of filesystems other than the root occurred in getmdev.
	 */
	if (devvp == rootvp) {
		VOP_OPEN(&devvp, ronly ? FREAD : FREAD|FWRITE, NOCRED, error);
		if (error) {
			ump->um_fs = NULL;
			goto out;
		}
		needclose = 1;
	}

	/*
	 * To determine if the device is already mounted or open, we call
	 * setmount(), which knows the magic.  We set the mounted flag
	 * at the same time to prevent races on mounting this device.
	 */
	if (setmount(devvp, SM_OPEN|SM_MOUNTED|SM_SETMOUNT)) 
		ERROR(EBUSY);
	setmounted = 1;
updateskip:
	/*
	 * Determine the size of the superblock prior to trying to 
	 * read it:
	 */
	VOP_IOCTL(devvp, DIOMRINFO, &mach_record_size, 0, NOCRED, error);
#ifdef	i386
	/*
	 * For 386 systems not supporting DIOMRINFO, default.
	 */
	if (error) {
		mach_record_size = DISK_GRANULE;
	}
#else
	if (error) {
		printf("Error: unable to determine device block size: %x\n",error);
		ERROR(error);
	}
#endif
	if (mach_record_size > SBSIZE) {
		sbsize = sboff = mach_record_size;
	} else {
		sbsize = SBSIZE;
		sboff = SBOFF;
	}

	/*
	 * Check to make sure buffer cache has correct size:
	 */
	if (mach_record_size > bcache_maxbsize) {
		printf("Error: bcache_maxbsize (%d) is less than the device record size (%d)\n", bcache_maxbsize, mach_record_size);
		ERROR(EINVAL);
	}

	if (error = bread(devvp, btodg(sboff), sbsize, NOCRED, &bp))  {
		goto out;
	}
	fs = bp->b_un.b_fs;
#if	SEC_FSCHANGE
        if (fs->fs_magic != FS_MAGIC && fs->fs_magic != FS_SEC_MAGIC)
#else
	if (fs->fs_magic != FS_MAGIC)
#endif
	{
	        printf("Error: fs_magic %d is not valid\n", fs->fs_magic);
		ERROR(EINVAL);
	}
	if (fs->fs_fsize > MAXFSIZE) {
		printf("Error: fragment size %d is too large. Max %d\n",
			fs->fs_fsize, MAXFSIZE);
		ERROR(EINVAL);
	}
	if (fs->fs_fsize > bcache_maxbsize) {
		printf("Error: frag size %d is too large for buffer max%d.\n",
			fs->fs_fsize, bcache_maxbsize);
		ERROR(EINVAL);
	}
        if (fs->fs_bsize < sizeof(struct fs)) {
                printf("Error: block size %d is too small.  Minimum is %d.\n",
                       fs->fs_bsize, sizeof(struct fs));
		ERROR(EINVAL);		/* XXX also needs translation */
	}
        if (fs->fs_fsize < DISK_GRANULE) {
                printf("Error: frag size %d is too small.  Minimum is %d.\n",
                       fs->fs_fsize, DISK_GRANULE);
		ERROR(EINVAL);		/* XXX also needs translation */
	}

	if (MIN(bcache_maxbshift, fs->fs_bshift) - DISK_GSHIFT >
	    devvp->v_bufhash_shift) {



		printf("Error: block size too large given buffer hash.\n");
		printf("Reboot may be necessary.\n");
		printf("bcache_maxbshift = %d\n",bcache_maxbshift);
		printf("fs->fs_bshift = %d\n",fs->fs_bshift);
		printf("DISK_GSHIFT = %d\n",DISK_GSHIFT);
		printf("devvp->v_bufhash_shift = %d\n",
			devvp->v_bufhash_shift);
		ERROR(EINVAL);          /* XXX also needs translation */
	}
#if	MAPPED_FILES
	/* Additional assumptions */
	if (vm_page_size > fs->fs_bsize || (fs->fs_bsize & page_mask) != 0) {
		printf("Error: vm_page_size=%d, fs_bsize=%d not compatible\n",
		       vm_page_size, fs->fs_bsize);
		ERROR(EINVAL);		/* XXX also needs translation */
	}
#else /* MAPPED_FILES */
	if (fs->fs_bsize > bcache_maxbsize) {
		printf("Error: block size %d is too large for buffer max%d.\n",
			fs->fs_bsize, bcache_maxbsize);
		ERROR(EINVAL);
	}
#endif /* MAPPED_FILES */
#ifdef	OSF1_ADFS		
        info = (devinfo_t *) dev_lookup(devvp->v_rdev, devvp->v_devnode,
					BLOCK_DEV);      
        if (info == NULL) {
                  printf("Error: devinfo could not be found\n");
                  ERROR(EINVAL);
	}
        if (info->mrecsize < DISK_GRANULE) {
                  printf("Error: Mach record size %d smaller than %d.\n",
			 info->mrecsize, DISK_GRANULE);
                  ERROR(EINVAL);
	}
        if (info->mrecsize > fs->fs_fsize) {
                  printf("Error: Mach record size %d larger than fragment %d.\n",
			 info->mrecsize, fs->fs_fsize);
                  ERROR(EINVAL);
	}
        if (info->mrecsize & (info->mrecsize - 1)) {
                  printf("Error: Mach record size %d not a power of rwo.\n",
                         info->mrecsize);
                  ERROR(EINVAL);
	}
	if (!update) {
                /*
                 * Move mach record info into superblock for convenience.
                 */
		fs->fs_mrecsize = info->mrecsize;
		fs->fs_mrecmask = info->mrecmask;
		fs->fs_mrecshift = info->mrecshift;
		fs->fs_devinfo = info;
        } 
#endif /* OSF1_ADFS */
	if (!update) {
		/*
	 	* superblock zone has memory of size, MAXBSIZE.
	 	*/
		ZALLOC(superblock_zone, ump->um_fs, struct fs *);
		FS_LOCK_INIT(fs);

	} else {
		kfree(ump->um_fs->fs_csp[0], fs->fs_cssize);
	}
	/*
	 * Synchronization issues:
	 * In the update case, is it possible that someone is
	 * trying to get to the struct fs, and will be actively using
	 * the lock.  We copy the new struct fs in parts, around the
	 * lock, to deal with current users.  It also eliminates the
	 * need to change all of the i_fs fields of open inodes
	 * to a new pointer.
	 * This only happens if UNIX_LOCKS is on (we have a lock
	 * in the struct fs).
	 *
	 * There is an assumption that while a filesystem is
	 * read-only, that the fs->fs_csp array WILL NOT be
	 * touched.  This is evidenced by the fact that we
	 * just freed its memory (above), and it will not
	 * be valid until after we re-read the cylinder
	 * group information below.
	 */
#if	UNIX_LOCKS
	if (update) {
		struct fs *tfs;
		u_int copysize;
		/*
		 * Copy the new information, leaving the lock in place.
		 */
		fs = ump->um_fs;
		tfs = bp->b_un.b_fs;
		FS_LOCK(fs);
		copysize = (u_int)&fs->fs_un - (u_int)&fs->fs_link;
		bcopy((caddr_t)tfs, (caddr_t)fs, copysize);
		copysize = (u_int)fs->fs_sbsize - copysize - sizeof(fs->fs_un);
		bcopy((caddr_t)&tfs->fs_qbmask, (caddr_t)&fs->fs_qbmask, 
		      copysize);
		fs->fs_ronly = ronly;
		FS_UNLOCK(fs);
	} else {
#endif
		bcopy((caddr_t)bp->b_un.b_addr, (caddr_t)ump->um_fs,
		      (u_int)fs->fs_sbsize);
		fs = ump->um_fs;
		fs->fs_ronly = ronly;
#if	UNIX_LOCKS
	}
#endif
	fs->fs_fmod = 0;	/* just in case */
	if (fs->fs_sbsize < sbsize)
		bp->b_flags |= B_INVAL;
	brelse(bp);
	bp = NULL;

	/*
	 * Save clean flag.  We unconditionally restore fs_clean on unmount
	 * so save it here unconditionally.  Only set fs_clean to 0 if we
	 * are mounting writable.
	 */
	if (devvp != rootvp && (fs->fs_clean != FS_CLEAN) && 
	    !(mp->m_flag & M_FMOUNT) && !update) 
		ERROR(EDIRTY);
	FS_LOCK(fs);
	fs->fs_flags = fs->fs_clean;
	if (ronly == 0) {
		fs->fs_fmod = 1;
		fs->fs_clean = 0;	/* mounted writable ==> dirty */
	}
	FS_UNLOCK(fs);
	blks = howmany(fs->fs_cssize, fs->fs_fsize);
	base = space = (caddr_t)kalloc(fs->fs_cssize);
	if (base == 0)
		ERROR(ENOMEM);
	for (i = 0; i < blks; i += fs->fs_frag) {
		size = fs->fs_bsize;
		if (i + fs->fs_frag > blks)
			size = (blks - i) * fs->fs_fsize;
		error = bread(devvp, fsbtodb(fs, fs->fs_csaddr + i), size,
			NOCRED, &bp);
		if (error) {
			kfree(base, fs->fs_cssize);
			goto out;
		}
		bcopy((caddr_t)bp->b_un.b_addr, space, (u_int)size);
		fs->fs_csp[fragstoblks(fs, i)] = (struct csum *)space;
		space += size;
		brelse(bp);
		bp = NULL;
	}

	/* Sanity checks for old file systems.			   XXX */
	FS_LOCK(fs);
	fs->fs_npsect = MAX(fs->fs_npsect, fs->fs_nsect);	/* XXX */
	fs->fs_interleave = MAX(fs->fs_interleave, 1);		/* XXX */
	if (fs->fs_postblformat == FS_42POSTBLFMT)		/* XXX */
		fs->fs_nrpos = 8;				/* XXX */
	FS_UNLOCK(fs);

#ifdef	OSF1_ADFS
	/*
	 * Indicate the I/O mode to use for VREG files in this filesystem.
	 */
	if (mp->m_flag & M_FASTPATH)
		mp->m_iomode = VIO_FASTPATH;
	else 
		/* XXX the following should be changed to a run-time check */
#if	MAPPED_FILES
		mp->m_iomode = VIO_MAPPED;
#else
		mp->m_iomode = VIO_BUF;
#endif
#endif

	if (update) /* we're done */
		return (0);
	mp->m_data = (qaddr_t)ump;
	mp->m_stat.f_fsid.val[0] = devvp->v_rdev;
#ifdef OSF1_ADFS
        mp->m_stat.f_fsid.val[1] = NODE_2_FSID1(devvp->v_devnode, 
                                                MOUNT_UFS);
#else
	mp->m_stat.f_fsid.val[1] = MOUNT_UFS;
#endif
	mp->m_flag |= M_LOCAL;
#ifdef OSF1_ADFS
        SET_MOUNTID(mp);
#endif
	ump->um_mountp = mp;
	ump->um_dev = devvp->v_rdev;
	ump->um_devvp = devvp;
#if	QUOTA
	for (i = 0; i < MAXQUOTAS; i++) {
		ump->um_quotas[i] = NULLVP;
		ump->um_cred[i] = NOCRED;
	}
	ump->um_qsync = 0;
	UMPQ_LOCK_INIT(ump);
	UMPQ_SYNC_LOCK_INIT(ump);
#endif
	/*
	 * Get the superblock written to disk
	 */
	(void) ufs_sync(mp, 1);
	return (0);
out:
	if (bp)
		brelse(bp);
	if (setmounted) 
		(void) setmount(devvp, SM_CLEARMOUNT);
	if (needclose) {
		VOP_CLOSE(devvp, ronly ? FREAD : FREAD|FWRITE, NOCRED, i);
	}
	if (ump && ump->um_fs) {
		if (!update) {
                        if (ump->um_fs != (struct fs *) 1)
				ZFREE(superblock_zone, ump->um_fs);
			ump->um_fs = NULL;
		}
	}
	return (error);
#undef ERROR
}

/*
 * Make a filesystem operational.
 * Nothing to do at the moment.
 */
/* ARGSUSED */
ufs_start(mp, flags)
	struct mount *mp;
	int flags;
{

	return (0);
}

/*
 * unmount system call
 *
 * Synchronization assumptions:
 *	-- other unmounts are locked out.
 *	-- mp is still on global list, and is accessible.
 */
ufs_unmount(mp, mntflags)
	struct mount *mp;
	int mntflags;
{
	register struct ufsmount *ump;
	register struct fs *fs;
	int error, ronly, flags = 0;
	struct buf *bp;
	devinfo_t *dev_info;
	int	sboff;
#if	QUOTA
	int i;
	int mflag;
#endif

	if (mntflags & MNT_FORCE)
		return (EINVAL);	/* don't allow forcible yet */
	if (mntflags & MNT_FORCE)
		flags |= FORCECLOSE;
	mntflushbuf(mp, 0);
	if (mntinvalbuf(mp))
		return (EBUSY);
	BM(MOUNT_LOCK(mp));
	ump = VFSTOUFS(mp);
#if	QUOTA
	mflag = mp->m_flag;
#endif	
	BM(MOUNT_UNLOCK(mp));
	if (!ump)
		return(ENODEV);
#if	QUOTA
	/*
	 * It is not possible for any other processor to
	 * be setting these flags while we are unmounting.
	 *
	 * Because vfs_quotactl does a namei to find the
	 * filesystem it will operate on, we can be sure
	 * that any quotactl requests arriving after the
	 * unmount operation begins will pend at our
	 * filesystem's root until this unmount request
	 * terminates.  However, we must wait for any
	 * quotactl requests in progress to terminate.
	 */
	if (mflag & M_QUOTA) {
		if (error = vflush(mp, NULLVP, SKIPSYSTEM|flags))
			return (error);
		for (i = 0; i < MAXQUOTAS; i++)
			quotaoff(mp, i);
		/*
		 * Here we fall through to vflush again to ensure
		 * that we have gotten rid of all the system vnodes.
		 */
	}
#endif
	if (error = vflush(mp, NULLVP, flags))
		return (error);
#if	MACH_NBC
	mfs_cache_clear();		/* remove cached mapped files */
#endif
	/*
	 * We need to deal with uncaching mapped files in a 
	 * synchronous manner
	 */
	fs = ump->um_fs;
	dev_info = fs->fs_devinfo;

	ronly = fs->fs_ronly;

	if (!ronly) {
		/*
		 * Use either fs->fs_fsize or sbsize, which ever is
		 * larger.
		 */
		int max_sbsize;

		max_sbsize=MAX(fs->fs_fsize, fs->fs_sbsize);

		sbupdate(ump, MNT_WAIT);
		/*
		 * Restore the file system clean flag.
		 */
		if (fs->fs_sbsize > SBSIZE) {
			/*
			 * Large block sized device, use computed
			 * superblock size.
			 */
			sboff = BBOFF + fs->fs_sbsize;
		} else {
			sboff = SBOFF;
		}
		bp = getblk(ump->um_devvp, btodg(sboff), (int)max_sbsize);
		bcopy((caddr_t)fs, bp->b_un.b_addr, fs->fs_sbsize);
		bp->b_un.b_fs->fs_clean = fs->fs_flags;
		bp->b_un.b_fs->fs_flags = 0;
		bwrite(bp);
	}
	/*
	 * use kfree for the first one
	 */
	kfree(fs->fs_csp[0], fs->fs_cssize);
	ZFREE(superblock_zone, fs);
	MOUNTTAB_LOCK();
	ump->um_fs = NULL;
	ump->um_dev = NODEV;
	MOUNTTAB_UNLOCK();
	/*
	 * Clear mounted flag from this vnode
	 */
	(void) setmount(ump->um_devvp, SM_CLEARMOUNT);
#if  MAPPED_FILES
	cache_flush_at_unmount(dev_info);
#endif
	VOP_CLOSE(ump->um_devvp, ronly ? FREAD : FREAD|FWRITE, NOCRED, error);
	vrele(ump->um_devvp);
	ump->um_devvp = (struct vnode *)0;
	MOUNT_LOCK(mp);
	mp->m_flag &= ~M_LOCAL;
	MOUNT_UNLOCK(mp);
	return (error);
}


/*
 * Return root of a filesystem
 * Synchronization assumptions:
 *	-- it's safe, and file system isn't going anywhere.
 */
ufs_root(mp, vpp)
	struct mount *mp;
	struct vnode **vpp;
{
	char fake_vnode[FAKE_INODE_SIZE];
	struct inode *nip;
	struct vnode *tvp;
	int error;

	tvp = (struct vnode *) fake_vnode;
	fake_inode_init(tvp, mp);
	error = iget(VTOI(tvp), (ino_t)ROOTINO, &nip, 0);
	if (error)
		return (error);
	*vpp = ITOV(nip);
	return (0);
}

/*
 * Do operations associated with quotas
 */
ufs_quotactl(mp, cmds, uid, arg)
	struct mount *mp;
	int cmds;
	uid_t uid;
	caddr_t arg;
{
#if	QUOTA
	register struct nameidata *ndp = &u.u_nd;
	struct ufsmount *ump = VFSTOUFS(mp);
	struct proc *p = u.u_procp;	/* XXX */
	int cmd, type, error;
	uid_t ruid;
	gid_t rgid;
#endif

#if	!QUOTA
	return (EOPNOTSUPP);
#else
	type = cmds & SUBCMDMASK;
	cmd = cmds >> SUBCMDSHIFT;

	switch(type) {
	case USRQUOTA:
		BM(PROC_LOCK(p));
		ruid = p->p_ruid;
		BM(PROC_UNLOCK(p));
		break;
	case GRPQUOTA:
		break;
	default:
		return(EINVAL);
	}

	if (uid == -1) {
		if (type == USRQUOTA)
			uid = ruid;
		if (type == GRPQUOTA)
			uid = (uid_t)rgid;
	}

	switch (cmd) {
	case Q_GETQUOTA:
	case Q_SYNC:
		if (type == USRQUOTA && uid == ruid)
			break;
		if (type == GRPQUOTA && groupmember((gid_t)uid, ndp->ni_cred))
			break;
		/* fall through */
	default:
#if	SEC_BASE
		if (!privileged(SEC_ACCT, EPERM))
			return(EPERM);
#else
		if (error = suser(ndp->ni_cred, &u.u_acflag))
			return (error);
#endif
	}

	if ((u_int)type >= MAXQUOTAS)
		return (EINVAL);

	switch (cmd) {

	case Q_QUOTAON:
		return (quotaon(ndp, mp, type, arg));

	case Q_QUOTAOFF:
		return (quotaoff(mp, type));

	case Q_SETQUOTA:
		return (setquota(mp, uid, type, arg));

	case Q_SETUSE:
		return (setuse(mp, uid, type, arg));

	case Q_GETQUOTA:
		return (getquota(mp, uid, type, arg));

	case Q_SYNC:
		return (qsync(mp));

	default:
		return (EINVAL);
	}
	/* NOTREACHED */
#endif
}

/*
 * Get file system statistics.
 *
 * Synchronization assumptions:
 *	-- File system isn't going anywhere.
 *	-- Lock order: mount structure, then struct fs.
 */
ufs_statfs(mp)
	struct mount *mp;
{
	register struct statfs *sbp;
	register struct fs *fs;

	MOUNT_LOCK(mp);
	sbp = &mp->m_stat;
	fs = (VFSTOUFS(mp))->um_fs;
#if	SEC_FSCHANGE
	if (fs->fs_magic != FS_MAGIC && fs->fs_magic != FS_SEC_MAGIC)
#else
	if (fs->fs_magic != FS_MAGIC)
#endif
		panic("ufs_statfs");
#ifdef	PFS
	/*
	 * Don't bother re-setting f_type if it's already been done (i.e.,
	 * if we're not in the process of installing a new mount struct).
	 * This prevents an f_type of MOUNT_PFS from getting blown away.
	 * We could just reset f_type back to MOUNT_PFS in a pfs_statfs(), but
	 * then we have an open hole between setting f_type here and setting it
	 * there (the mount struct is not locked between the two).
	 */
	if (sbp->f_type == MOUNT_NONE)
#endif	PFS
	sbp->f_type = MOUNT_UFS;
	sbp->f_fsize = fs->fs_fsize;
	sbp->f_bsize = fs->fs_bsize;
	sbp->f_blocks = fs->fs_dsize;
	FS_LOCK(fs);
	sbp->f_bfree = fs->fs_cstotal.cs_nbfree * fs->fs_frag +
		fs->fs_cstotal.cs_nffree;
	sbp->f_bavail = (fs->fs_dsize * (100 - fs->fs_minfree) / 100) -
		(fs->fs_dsize - sbp->f_bfree);
	sbp->f_files =  fs->fs_ncg * fs->fs_ipg - ROOTINO;
	sbp->f_ffree = fs->fs_cstotal.cs_nifree;
	FS_UNLOCK(fs);
	MOUNT_UNLOCK(mp);
	return (0);
}

int	syncprt = 0;

/*
 * Go through the disk queues to initiate sandbagged IO;
 * go through the inodes to write those that have been modified;
 * initiate the writing of the super block if it has been modified.
 */
ufs_sync(mp, waitfor)
	struct mount *mp;
	int waitfor;
{
	register struct vnode *vp;
	register struct ufsmount *ump = VFSTOUFS(mp);
	register struct fs *fs;
	register struct vnode *nvp;
	int error, allerror = 0;
	extern void bufstats();

	if (syncprt)
		bufstats();
	fs = ump->um_fs;
	/*
	 * Write back modified superblock.
	 * Consistency check that the superblock
	 * is still in the buffer cache.
	 */
	FS_LOCK(fs);
	if (fs->fs_fmod != 0) {
		int s;

		if (fs->fs_ronly != 0) {		/* XXX */
			printf("fs = %s\n", fs->fs_fsmnt);
			panic("update: rofs mod");
		}
		fs->fs_fmod = 0;
		s = splhigh();
		TIME_READ_LOCK();
		fs->fs_time = time.tv_sec;
		TIME_READ_UNLOCK();
		splx(s);
		FS_UNLOCK(fs);
		error = sbupdate(ump, waitfor);
	} else if (fs->fs_ronly != 0) {
		/* if we're racing a mount update */
		FS_UNLOCK(fs);
		return(0);
	} else
		FS_UNLOCK(fs);
	/*
	 * Write back each (modified) inode.
	 */
	MOUNT_VLIST_LOCK(mp);
	for (vp = mp->m_mounth; vp; vp = nvp) {
		register struct inode *ip;

		/*
 		 * nvp will hold the vnode pointer to the vnode we're 
		 * flushing, which could be different from vp, the one
		 * that's on the mount vnode list, in the case of VBLK.
		 */
		VN_LOCK(vp);
		if (vp->v_type == VBLK) {
			if ((nvp = shadowvnode(vp)) == NULLVP) {
			    VN_UNLOCK(vp);
			    nvp = vp->v_mountf;
			    continue;
			}
		} else
			nvp = vp;
		VN_UNLOCK(vp);
		ip = VTOI(vp);

#if	MAPPED_FILES
		/*
		 * If the file is mappable then take a different path so
		 * that we always call vflushbuf(), which in turn will
		 * clean main memory pages.  The only reason we have
		 * to special case mappable files in this routine is 
		 * because the original code attempts to optimize away
		 * the call to vflushbuf().  This should be fixed.
		 * Ditto for checking i_flag flags here - should only
		 * be done by iupdat().
		 */
 		if (VIO_IS_MAPPED(nvp)) { 
			VN_LOCK(nvp);
			if (vget_nowait(nvp)) {
				VN_UNLOCK(nvp);
				nvp = vp->v_mountf;
				continue;
			}
			VN_UNLOCK(nvp);
			MOUNT_VLIST_UNLOCK(mp);
			vflushbuf(nvp, 0);
			IN_LOCK(ip);
			if (ip->i_flag & (IMOD|IACC|IUPD|ICHG|ISIZ)) {
				IN_UNLOCK(ip);
				if (error = iupdat(ip, &time, &time, 0))
					allerror = error;
			} else
				IN_UNLOCK(ip);

		} else {
#endif	MAPPED_FILES
			/*
			 * Not a mapped file: sync the original way.
			 */
			IN_LOCK(ip);
			if ((ip->i_flag & (IMOD|IACC|IUPD|ICHG)) == 0) {
				IN_UNLOCK(ip);
				VN_LOCK(nvp);
				if (nvp->v_dirtyblkhd == NULL) {
					VN_UNLOCK(nvp);
					nvp = vp->v_mountf;
					continue;
				}
			} else {
				IN_UNLOCK(ip);
				VN_LOCK(nvp);
			}
			if (vget_nowait(nvp)) {
				VN_UNLOCK(nvp);
				nvp = vp->v_mountf;
				continue;
			}
			MOUNT_VLIST_UNLOCK(mp);

			if (nvp->v_dirtyblkhd) {
				VN_UNLOCK(nvp);
				vflushbuf(nvp, 0);
			} else
				VN_UNLOCK(nvp);
			IN_LOCK(ip);
			if (ip->i_flag & (IMOD|IACC|IUPD|ICHG)) {
				IN_UNLOCK(ip);
				if (error = iupdat(ip, &time, &time, 0))
					allerror = error;
			} else
				IN_UNLOCK(ip);
#if	MAPPED_FILES
		}
#endif
		vrele(nvp);
		MOUNT_VLIST_LOCK(mp);
		if (vp->v_mount == mp) 
			nvp = vp->v_mountf;
		else  {
			BUF_STATS(bio_stats.ufssync_misses++);
			nvp = mp->m_mounth;
		}
	}
	MOUNT_VLIST_UNLOCK(mp);
	/*
	 * Force stale file system control information to be flushed.
	 */
	vflushbuf(ump->um_devvp, waitfor == MNT_WAIT ? B_SYNC : 0);
#if	QUOTA
	qsync(mp);
#endif
	return (allerror);
}

/*
 * Write a superblock and associated information back to disk.
 */
sbupdate(mp, waitfor)
	struct ufsmount *mp;
	int waitfor;
{
	register struct fs *fs = mp->um_fs;
	register struct buf *bp;
	int blks;
	caddr_t space;
	int i, size, error = 0;
	long max_sbsize;
	int sboff;

	ASSERT(fs->fs_ronly == 0);

	max_sbsize = MAX(fs->fs_fsize, fs->fs_sbsize);
	if (fs->fs_sbsize > SBSIZE) {
		/*
		 * Large block sized device, use computed
		 * superblock size.
		 */
		sboff = BBOFF + fs->fs_sbsize;
	} else {
		sboff = SBOFF;
	}
	bp = getblk(mp->um_devvp, btodg(sboff), (int)max_sbsize);
	FS_LOCK(fs);
	bcopy((caddr_t)fs, bp->b_un.b_addr, (int)max_sbsize);
	FS_UNLOCK(fs);
	/* Restore compatibility to old file systems.		   XXX */
	if (fs->fs_postblformat == FS_42POSTBLFMT)		/* XXX */
		bp->b_un.b_fs->fs_nrpos = -1;			/* XXX */
	if (waitfor == MNT_WAIT)
		error = bwrite(bp);
	else
		bawrite(bp);
	blks = howmany(fs->fs_cssize, fs->fs_fsize);
	space = (caddr_t)fs->fs_csp[0];
	for (i = 0; i < blks; i += fs->fs_frag) {
		size = fs->fs_bsize;
		if (i + fs->fs_frag > blks)
			size = (blks - i) * fs->fs_fsize;
		bp = getblk(mp->um_devvp, fsbtodb(fs, fs->fs_csaddr + i), size);
		FS_LOCK(fs);
		bcopy(space, bp->b_un.b_addr, (u_int)size);
		FS_UNLOCK(fs);
		space += size;
		if (waitfor == MNT_WAIT)
			error = bwrite(bp);
		else
			bawrite(bp);
	}
	return (error);
}

/*
 * File handle to vnode
 *
 * Have to be really careful about stale file handles:
 * - check that the inode number is in range
 * - call iget() to get the locked inode
 * - check for an unallocated inode (i_mode == 0)
 * - check that the generation number matches
 *
 * Synchronization assumptions:
 *	-- File system isn't going anywhere.
 */
ufs_fhtovp(mp, fhp, vpp)
	register struct mount *mp;
	struct fid *fhp;
	struct vnode **vpp;
{
	char fake_vnode[FAKE_INODE_SIZE];
	register struct ufid *ufhp;
	register struct fs *fs;
	register struct inode *ip;
	struct inode *nip;
	struct vnode *tvp;
	int error;

	ufhp = (struct ufid *)fhp;
	fs = VFSTOUFS(mp)->um_fs;
	if (ufhp->ufid_ino < ROOTINO ||
	    ufhp->ufid_ino >= fs->fs_ncg * fs->fs_ipg) {
		*vpp = (struct vnode *)0;
		return (EINVAL);
	}
	tvp = (struct vnode *) fake_vnode;
	fake_inode_init(tvp, mp);
	if (error = iget(VTOI(tvp), ufhp->ufid_ino, &nip, 0)) {
		*vpp = (struct vnode *)0;
		return (error);
	}
	ip = nip;
	IN_LOCK(ip);
	/*
	 * If the file has been unlinked or the inode has been reclaimed,
	 * we return EINVAL.
	 */
	if ((ip->i_mode == 0) ||
	    (ip->i_gen != ufhp->ufid_gen)) {
		IN_UNLOCK(ip);
		iput(ip);
		*vpp = (struct vnode *)0;
		return (EINVAL);
	}
	*vpp = ITOV(ip);
	IN_UNLOCK(ip);
	return (0);
}

/*
 * Vnode pointer to File handle
 * Synchronization assumptions:
 *	-- i_gen and i_number are read-only
 */
/* ARGSUSED */
ufs_vptofh(vp, fhp)
	struct vnode *vp;
	struct fid *fhp;
{
	register struct inode *ip = VTOI(vp);
	register struct ufid *ufhp;

	ufhp = (struct ufid *)fhp;
	ufhp->ufid_len = sizeof(struct ufid);
	ufhp->ufid_ino = ip->i_number;
	ufhp->ufid_gen = ip->i_gen;
	return (0);
}

#ifdef	OSF1_ADFS
/*
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device vnode if so.
 */
getmdev(devvpp, fname, ndp, flag, rdevp, devfsportp)
	struct vnode 		**devvpp;
	caddr_t 		fname;
	register struct nameidata *ndp;
	int 			flag;
	dev_t			*rdevp;
	mach_port_t 		*devfsportp;
{
	struct vnode *vp;
	struct vnode *cdirsave;
	int error = 0;
	int update = (flag & M_UPDATE) != 0;
	int ronly = (flag & M_RDONLY) != 0;
	mach_port_t devport = MACH_PORT_NULL;
	char *pathname;
	enum vtype type;

	struct uthread  *uth = &u;
#if     NORMA_IPC
        node_t dev_node;
        extern node_t this_node;
#endif

	*devfsportp = MACH_PORT_NULL;
	*devvpp = NULLVP;
	*rdevp = -1;
	if (ndp->ni_vp2) {
		/*
		 * Before we can re-use the nameidata structure,
		 * we check if namei allocated a pathname buffer and
		 * deallocate it.
		 */
		if (ndp->ni_allocbuf) {
			ASSERT(ndp->ni_allocbuf == 1);
			PN_DEALLOCATE(ndp->ni_pnbuf);
			ndp->ni_allocbuf = 0;
		}
		/*
		 * If the path for the device starts on the local node,
		 * call namei to translate it, passing the MOUNT option.
		 * Namei avoids potential deadlocks when it receives this
		 * option by not attempting to cross into filesystems
		 * with mounts in progress. 
		 */
		ndp->ni_nameiop = LOOKUP | FOLLOW | MOUNT | HASPATHBUF;
		ndp->ni_segflg = UIO_SYSSPACE;
		ndp->ni_ptr = ndp->ni_pnbuf = ndp->ni_dirp = fname;
		ndp->ni_pathlen = strlen(ndp->ni_pnbuf) + 1;
		cdirsave = ndp->ni_cdir;
		ndp->ni_cdir = ndp->ni_vp2;
		error = namei(ndp);
		ndp->ni_cdir = cdirsave;
		if (error == EREMOTE) {
			/*
			 * The pathname for the device crosses node boundaries.
			 */
			ASSERT(update == 0);
			pathname = ndp->ni_ptr;
		} else if (error) {
			return(error == ENOENT ? ENODEV : error);
		} else {
			/*
			 * The special file for the device is local.
			 */
			vp = ndp->ni_vp;
			VN_LOCK(vp);
			type = vp->v_type;
			VN_UNLOCK(vp);
			if (type == VBLK) {
				if (major(vp->v_rdev) < nblkdev)
					*devvpp = vp;
				else
					error = ENXIO;
			} else
				error = ENOTBLK;
			if (error) {
				vrele(vp);
				return(error);
			}
			*rdevp = vp->v_rdev;
			if (update)
				return(0);

			/*
			 * Open the device.  Pretend we're doing an open
			 * system call so that spec_open will save a file
			 * structure port in the nameidata structure if
			 * the device represented by the special file is remote.
			 */
			uth->uu_syscode = SYS_open;
			VOP_OPEN(devvpp, ronly ? FREAD : FREAD|FWRITE, NOCRED,
				error);
			uth->uu_syscode = SYS_mount;
			if (error == EREMOTEPORT) {
				/*
				 * The local special file represents a 
				 * remote device. The file structure port
				 * was saved in ndp->ni_forwport.
				 */
				ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
				*devfsportp = ndp->ni_forwport;
				error = 0;
			} else
				return(error);
		}
	} else {
		/*
		 * The pathname begins on a remote machine.  Prepare to
		 * send a lookup message.
		 */
		error = EREMOTE;
		ndp->ni_forwport = ndp->ni_rdirport;
		pathname = fname;
	}
	if (error == EREMOTE) {
		dev_t	rdev;
		node_t	rnode;

		/*
		 * Send a lookup message passing the MOUNT option.   Then
		 * send a vop_open message to open the device if the mount
		 * is not being updated.  We open the device now, instead of
		 * later to locate the node servicing the device, not the
		 * node servicing the special file describing the device.
		 */

		error = remote_lookup(ndp, pathname, LOOKUP | FOLLOW,
			&type, &rdev, &rnode, &devport);
		if (!error) {
			if (type != VBLK)
				error = ENOTBLK;
			else if (major(rdev) >= nblkdev)
				error = ENXIO;
		}
		if (error)
			return(error == ENOENT ? ENODEV : error);
		ASSERT(devport != MACH_PORT_NULL);
		*rdevp = rdev;
		if (update) {
			/*
			 * The special file exists.  We already acquired
			 * a send right to dev port when we did the mount.
			 * So we deallocate it here.
			 */
			mach_port_deallocate(mach_task_self(), devport);
			return(EREMOTE);
		}

		/*
		 * Open the remote block device and receive a 
		 * file structure port for the device.
		 */
		error = remote_spec_vnode_open(devport, ronly ? O_RDONLY :
			O_RDWR, devfsportp);
		if (error == EREMOTEPORT) {
			ASSERT(*devfsportp != MACH_PORT_NULL);
			/*
			 * The open of the remote special file actually
			 * opened a block device that was on another
			 * remote node. This results in an extra
			 * reference on both the special file vnode and
			 * the pseudo block device vnode on the other
			 * node.  Release the reference to the special
			 * file vnode.  Its port already holds a
			 * reference on the vnode.
			 */
			(void) mach_port_deallocate(mach_task_self(), devport);
			error = 0;
		}
	}
#if     NORMA_IPC
	if (!error) {
		ASSERT(*devfsportp != MACH_PORT_NULL);
		if (norma_port_location_hint(mach_task_self(),
		    *devfsportp, &dev_node) != KERN_SUCCESS)
			panic("getmdev: can't get node for device");
		if (dev_node == this_node)
#endif
		{
			register struct file *fp;

			/*
			 * If the block device is serviced
			 * locally, treat this as a local mount.
			 */
			PORT_TO_FILE_LOOKUP(*devfsportp, fp);
			ASSERT(fp != (struct file *)0);
			ASSERT(fp->f_type == DTYPE_VNODE);
			vp = (struct vnode *)fp->f_data;
			ASSERT(vp);
			FP_UNREF(fp);
			*devvpp = vp;
 			/*
			 * XXX This is not right. We may be leaking
			 * the port, but it can't be deallocated here.
			 * see the comment in ufs_mount regrding devfsport 
                         * also (durriya)
			mach_port_deallocate(mach_task_self(), *devfsportp);
                        */
			mach_port_deallocate(mach_task_self(),
				devport);
			*devfsportp = MACH_PORT_NULL;
			return(0);
		}
#if 	NORMA_IPC
	}
#endif
	if (devport != MACH_PORT_NULL)
		mach_port_deallocate(mach_task_self(), devport);
	return(error ? error : EREMOTE);
}

#else	/* OSF1_ADFS */

/*
 * Check that the user's argument is a reasonable
 * thing on which to mount, and return the device vnode if so.
 *
 */
getmdev(devvpp, fname, ndp)
	struct vnode **devvpp;
	caddr_t fname;
	register struct nameidata *ndp;
{
	struct vnode *vp;
	int error, type;

	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = fname;
	if (error = namei(ndp)) {
		/* ENOENT needs translation */
		return (error == ENOENT ? ENODEV : error);
	}
	vp = ndp->ni_vp;
	VN_LOCK(vp);
	type = vp->v_type;
	VN_UNLOCK(vp);
	if (type == VBLK) {
		if (major(vp->v_rdev) < nblkdev)
			*devvpp = vp;
		else
			error = ENXIO;
	} else
		error = ENOTBLK;
	if (error)
		vrele(vp);
	return (error);
}
#endif	/* OSF1_ADFS */


/*
 * Loop through all of the vnodes associated with a mount point, and 
 * vgone those with v_usecount of 0, and refresh the in-core inode
 * data for those which are in use.  This function may only be called
 * for a filesystem that is mounted read-only.
 * This function looks a lot like vflush, with some changes.
 *
 * Synchronization:
 * Since the filesystem is read-only, the only way that vnodes will be
 * removed from the mount list is through recycling, when the system is
 * short on vnodes, so there's little risk of the list changing.  However,
 * to be safe, if it looks like it's changed, we start over at the
 * beginning, and run through the loop until it gets through clean.
 *
 * NOTE:
 * Some of what this function does implies internal knowledge of vfs_subr.c,
 * which we don't really want to do, but...
 */

void
irefresh(mp)
	struct mount *mp;
{
	register struct vnode *vp, *nvp;
	struct inode *nip;
	
	MOUNT_VLIST_LOCK(mp);
	for (vp = mp->m_mounth; vp; vp = nvp) {
		nvp = vp->v_mountf;
		MOUNT_VLIST_UNLOCK(mp);
		VN_LOCK(vp);
		if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
			VN_UNLOCK(vp);
			(void)inode_uncache_try(vp);
			VN_LOCK(vp);
		}
		if (vp->v_usecount == 0) {
			(void) vgone(vp, VX_SLEEP, (struct vnodeops *) 0);
			VN_UNLOCK(vp);
		} else {
			register struct inode *ip = VTOI(vp);
			VN_UNLOCK(vp);
			/*
			 * Setting the update parameter to iget will
			 * cause it to re-read the inode data from the
			 * disk, and release the vnode reference before
			 * returning.
			 */
			(void)iget(ip, ip->i_number, &nip, 1);
			if (nip != ip) {
				struct vnodeops *ops;
				extern struct vnodeops  dead_vnodeops,
							spec_vnodeops;
				printf("irefresh: failed iget, vp = %X\n", vp);
				VN_LOCK(vp);
				if (!wait_for_vxlock(vp, 1)) {
					if (vp->v_type == VCHR ||
					    vp->v_type == VBLK)
						ops = &spec_vnodeops;
					else
						ops = &dead_vnodeops;
					VN_UNLOCK(vp);
					(void)vclean(vp, 0, ops);
					(void)clear_vxlock(vp);
				} else
					VN_UNLOCK(vp);

			}
		}
		MOUNT_VLIST_LOCK(mp);
		/*
		 * If nvp is still non-null and on the list, use it;
		 * otherwise, restart the loop.
		 */
		if (nvp && (nvp->v_mount != mp))
			nvp = mp->m_mounth;
	}
	MOUNT_VLIST_UNLOCK(mp);
}
