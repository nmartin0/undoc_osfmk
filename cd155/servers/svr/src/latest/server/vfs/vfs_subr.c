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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was created by the Center for High Performance 
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: vfs_subr.c,v $
 * Revision 1.16  1995/02/01  22:34:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.15  1994/11/18  20:50:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.14  1994/09/29  21:41:23  jlitvin
 * Automatic merge tool used by OSF caused a critical piece of code to be
 * duplicated.  This opened a small window of opportunity where 2 stale
 * pointers could be used.  This theory was proved by adding a large
 * delay to the window of opportunity and seeing 3 ufs_sync() hangs in 3
 * attempts.  In summary, we only need to delete this vnode from the old
 * mount point vnode list one time!
 *
 *  Reviewer: suri & dnoveck@osf.org
 *  Risk: low
 *  Benefit or PTS #: 10525
 *  Testing: SCAT service partition test
 *  Module(s): server/vfs/vfs_subr.c
 *
 * Revision 1.13  1994/07/05  19:13:57  cfj
 * The getmntid() function reduces to just a return ESUCCESS if
 * FULLSERVER is not defined.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:9992
 *  Testing: test case
 *  Module(s):server/vfs/vfs_subr.c server/uxkern/fsvr_msg.c
 *
 * Revision 1.12  1994/06/28  23:23:08  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.11  1994/06/18  00:40:42  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.10  1994/01/11  18:26:10  jlitvin
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
 * Revision 1.9  1993/09/27  04:41:15  robboy
 * Delete "#if 1" which had previously been "#if FULLSERVER"
 *
 * Revision 1.8  1993/08/18  16:37:47  cfj
 * Fix for PTS bug #6098.  Turn on selected code which previously was turned off if FULLSERVER
 * was not defined to allow pipes to work with the lite server in compute paritions.
 *
 * Revision 1.7  1993/07/19  23:00:27  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.6  1993/07/14  18:46:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:09:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:32:12  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:50  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.26  93/10/20  15:32:02  dnoveck
 *      DEV_BSIZE elimination: Initialize v_bufhash_shift.
 *
 * Revision 2.25  93/07/13  16:14:18  slively
 * Added a panic if getnewvnode() is called on a non-server node.
 *      Revision 2.25  93/06/29  16:18:40  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.24  93/06/25  11:27:36  slively
 * Backout the LITE server changes.  Remove the #if FILESERVER and include.
 * 
 * Revision 2.23  93/06/24  13:29:28  mjl
 * [LCC bug 0229] Null out vp->v_tncdata after freeing the memory it pointed at.
 * 
 * Revision 2.22  93/06/22  20:09:52  slively
 * Support for LITE server.  #if FILESERVER sections and include fileserver.h.
 * 
 * 	Revision 2.23  93/04/06  11:58:04  rabii
 * 		Merged Synchronization changes for engineering bug 6874 from 1.3
 *
 * 	Revision 2.22  93/03/30  16:12:00  roy
 * 		For vfs_vio, initialize the VIO module.
 * 		[93/02/18            roy]
 *
 * Revision 1.4  1993/04/03  03:13:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:08:51  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.3  1993/01/22  18:13:02  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 2.20  92/12/30  08:15:41  chrisp
 * Correct typo to satisfy the 860 compiler.
 * 
 * Revision 2.19  92/12/29  13:14:37  chrisp
 * Vnode table now dynamically allocated. vfsinit() pre-allocates and
 * 	initializes nvnode (=NVNODE as defined in param.c) vnodes at
 * 	startup. If free vnodes are exhausted, more are zalloc()ed.
 * 	Note: vnode and vnodeNVNODE point the lowest and highest addressed
 * 	vnodes respectively.
 * In vgone(), don't attempt to move a vnode to the head of the free list
 * 	if it's the only one there.
 * 
 * Revision 1.1.2.1.2.1  1992/12/16  23:06:33  dbm
 * Added PFS token functionality.
 *
 * Revision 1.2  1992/11/30  22:58:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:46:35  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:57:41  cfj
 * Bump major revision number.
 *
 * Revision 2.19  1992/10/22  16:06:27  dbm
 * Added PFS functionality.
 *
 * Revision 2.18  1992/10/06  12:24:05  roman
 * Fix RCS comments.
 * 
 * Revision 2.17  92/10/05  14:02:44  klh
 * 	Revision 2.17  92/09/29  16:50:36  rabii
 * 		Add new routine, vflushall, to be called by reboot to flush 
 *		out vnodes for all mounted file systems.  (mmp)
 * 
 * 	Revision 2.16  92/09/11  17:27:00  rabii
 * 	Removed extra printfs
 * 
 * 	Revision 2.15  92/09/11  09:30:14  rabii
 * 		getmntid() now returns the mountid through a return variable.
 * 		[92/09/10            roy]
 * 
 * Revision 2.16  92/09/28  13:38:39  klh
 * Delete annoying debug printf (klh for mjl).
 * 
 * Revision 2.15  92/08/17  13:30:43  mjl
 * In vrele(), add code to allow TNC FIFO relocation to wait for all vnode
 * referrences other than file struct referrences to go away.
 * 
 * Revision 2.14  92/06/08  18:31:48  pjg
 * 	Set v_iomode to VIO_BUF in getnewvnode (OSF1_ADFS only) to get
 * 	the default behavior (use buffer cache). This field may be set to
 * 	other values by the FS specific code if mapped files or fast path io
 * 	are enabled (pjg).
 * 
 * 	For OSF1_ADFS modify getvfs to start with mounth rather than rootfs
 * 	add node as an argument to getmntid. Also, for ADFS in getvfs the 
 * 	locking scheme has been chaged. This is not a permanent solution and
 * 	a better one needs to be adopted (durriya).
 * 
 * Revision 2.13  92/05/24  14:05:37  pjg
 * 	92/03/10  16:06:41  condict
 * 	Add missing VN_OUTPUT_LOCK_INIT calls.
 * 	[92/05/20            srl]
 * 
 * Revision 2.12  92/05/18  12:31:23  roy
 * 	Revision 2.9.1.1  92/05/08  12:16:20  roy
 * 	Added call to vm_info_reinit() in getnewvnode() for MAPPED_FILES.
 * 	Call mf_clean() in vclean() for MAPPED_FILES.
 * 	[92/05/01            roy]
 * 
 * Revision 2.11  92/04/05  17:11:38  pjg
 * 	Change nddup and ndrele to correctly handle the new vnode proxies (pjg)
 * 
 * 	Initialize mountcont_lock and mount_count in vfsinit. Define getmntid()
 * 	(durriya).
 * 
 * Revision 2.10  92/03/20  11:38:10  pjg
 * 	Don't copy the credsport in nddup because it is not stored in
 * 	nameidata anymore.
 * 
 * Revision 2.9  92/03/15  14:42:08  roy
 * 	92/03/03  16:54:27  roy
 * 	Use mf_uncache for mapped files.
 * 
 * Revision 2.8  92/03/01  18:45:12  pjg
 * 	Initialize the message seqno to 0 (pjg).
 * 	ni_msg is no more.  (loverso)
 * 
 * Revision 2.7  92/01/16  16:14:09  roy
 * 	insmntque fix (noemi).
 * 
 * Revision 2.6  92/01/14  10:54:33  roy
 * 	92/01/10  22:12:00  noemi
 * 	Put anonymous vnodes on DEAMOUNT structure's vnode list in insmntque.
 * 
 * 	92/01/07  17:13:13  noemi
 * 	Changes to use mounth as a mounthead struct.
 * 
 * Revision 2.5  92/01/05  20:01:07  roy
 * 	1991/12/19  23:49:40  noemi
 * 	Added initialization of mounth structure.
 * 
 * 	1991/12/18  22:49:48  noemi
 * 	Added initialization of va_node to vattr_null.
 * 
 * 	1991/11/27  03:13:43  ses
 * 	Changed the initialization of the vseqno field to -1.  Added
 * 	initialization of the v_mscount field (to 0).  Changed ASSERTions
 * 	accordingly.
 * 
 * 	91/10/14  20:18:28  noemi
 * 	Initialized v_magic and v_seqno to 0 in all vnodes.  Changed nddup and
 * 	ndinit to handle new nameidata fields.
 * 
 * 	1991/09/22  22:06:09  noemi
 * 	OSF1/AD update
 * 
 * Revision 2.4  91/12/13  10:25:28  roy
 * 	91/11/18  14:12:11  roy
 * 	Remove obsolete references to vm_info's close_on_flush flag.
 * 
 * Revision 2.3  91/10/14  13:26:48  sjs
 * 	91/09/13  12:52:07  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  14:31:55  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/01  17:03:55  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.14.3.3  91/01/09  09:09:03  dwm
 * 	Fixed vfssw_del mount usage lookup, bug 1570
 * 	[91/01/07  20:27:05  dwm]
 * 
 * Revision 1.14.3.2  91/01/07  20:38:39  dwm
 * 	Fixed vfssw_del mount usage lookup, bug 1570
 * 	[91/01/07  20:27:05  dwm]
 * 
 * Revision 1.14  90/10/31  14:08:49  devrcs
 * 	1.  Removed old, bad, unused getvfs code.
 * 	2.  Fixed getvfs to unlock and release covered vnode
 * 	    in all error cases.
 * 	3.  Minor comment clean up.
 * 	[90/10/24  14:39:07  nags]
 * 
 * Revision 1.13  90/10/07  15:00:52  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:55:36  gm]
 * 
 * Revision 1.12  90/09/23  16:01:43  devrcs
 * 	Voided return value of vmountwait() in getvfs()
 * 	[90/09/12  15:29:30  noemi]
 * 
 * 	Use 4.3BSD-Reno vnode-related flags (SKIPSYSTEM,
 * 	FORCECLOSE, DOCLOSE).  Interface changes for vflush
 * 	and vclean.
 * 	[90/09/03  22:38:50  nags]
 * 
 * Revision 1.11  90/08/24  12:30:38  devrcs
 * 	Fix gaping hole in getvfs.  Remove obsolete
 * 	code to fake dynamic load of NFS.  We no longer
 * 	use VBWAIT.
 * 	[90/08/19  01:28:56  nags]
 * 
 * Revision 1.10  90/07/27  09:10:02  devrcs
 * 	Fixed removal of other-than-first vnode on vnode free list.
 * 	(Caused assertion failure in vrele, vp-v_freeb == (struct vnode **) 0).
 * 	[90/07/23  16:49:49  gmf]
 * 
 * 	Removed debug print statements.
 * 	[90/07/23  09:10:53  gmf]
 * 
 * 	Dead mount structures, dynamic filesystem loading.
 * 	[90/07/20  17:09:52  nags]
 * 
 * 	Put in some paranoia assertions and change NULL to NULLVP.
 * 	[90/06/29  14:47:05  gmf]
 * 
 * Revision 1.8  90/06/29  13:54:55  devrcs
 * 	Made vnode initialization loop in vfsinit a bit safer.
 * 	[90/06/26  11:41:45  gmf]
 * 
 * Revision 1.7  90/06/22  20:56:46  devrcs
 * 	Assert that we aren't vgone'ing rootvp or rootdir.
 * 	[90/06/18  17:14:48  gmf]
 * 
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:58:33  seiden]
 * 
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Remove pageable flag from zinit argument list.	jvs@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Use m_stat.f_fsid, ndinit interface changes.	gmf@osf.org
 * 	Hash chains were mixed up when v_type==VNON.	mkm@berkeley.edu
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	[90/06/12  21:43:58  gmf]
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
 *	@(#)vfs_subr.c	7.22 (Berkeley) 12/31/89
 */

/*
 * External virtual filesystem routines
 */

#ifdef	OSF1_ADFS
#include <fullserver.h>
#include <mapped_files.h>
#include <vfs_vio.h>
#endif

#include <sys/secdefines.h>

#include <sys/param.h>
#include <sys/mount.h>
#include <sys/time.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/namei.h>
#include <sys/ucred.h>
#include <sys/errno.h>
#include <sys/lock_types.h>
#if	MACH
#ifdef	OSF1_SERVER
#include <uxkern/vm_param.h>
#else	OSF1_SERVER
#include <mach/vm_param.h>
#endif	OSF1_SERVER
#include <kern/zalloc.h>
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/mfs.h>
#ifdef	TNC
#include <tnc/un_ff.h>
#endif	/* TNC */
#include <uxkern/mf.h>

/*
 * Zones used by the VFS code for dynamic data structures
 */
zone_t	pathname_zone;
zone_t	mount_zone;

#else	/* MACH */
#include <sys/malloc.h>
#endif	/* MACH */

/*
 * XXX Get rid of this when assertion in vgone goes away
 */
extern struct vnode *rootvp;
extern struct vnode *rootdir;

/*
 * Important global vnode table variables.  These variables are
 * initialized in machine-dependent code.
 */
struct vnode	*vnode;			/* base of table */
struct vnode	*vnodeNVNODE;		/* end of table */
int		nvnode;			/* number of vnodes in table */
int		vn_maxprivate;		/* max size of fs-specific info. */
zone_t		vnode_zone;		/* vnode table zone */

/*
 * The vnode free list lock, vn_free_lock, is only used in this file.
 */
udecl_simple_lock_data(,vn_free_lock)

#define	VN_FREE_LOCK()		usimple_lock(&vn_free_lock)
#define	VN_FREE_UNLOCK()	usimple_unlock(&vn_free_lock)
#define	VN_FREE_LOCK_INIT()	usimple_lock_init(&vn_free_lock)

/*
 * Lock for the vfssw to synchronize addition and deletion of filesystems.
 * This lock must be taken for reading during mount and unmount operations,
 * and taken for writing when adding or deleting filesystems.
 */
lock_data_t	vfssw_lock;

/*
 * Lock for linked list of mounted file systems.
 */
udecl_simple_lock_data(,mountlist_lock)

void vclean();
void vprint();

/*
 * Arg passed to wait_for_vxlock to indicate whether or not to set VXLOCK
 */
#define VX_NOLOCK	0
#define VX_LOCK		1

/*
 * clear the VXLOCK flag and wakeup sleepers.
 */
void
clear_vxlock(vp)
	register struct vnode *vp;
{
	VN_LOCK(vp);
	ASSERT(vp->v_flag & VXLOCK);
	vp->v_flag &= ~VXLOCK;
	if (vp->v_flag & VXWANT) {
		vp->v_flag &= ~VXWANT;
		VN_UNLOCK(vp);
		thread_wakeup((int)&vp->v_flag);
	} else
		VN_UNLOCK(vp);
}

/*
 * look at VXLOCK and block on VXWAIT
 * Takes locked vp, and it return locked.
 */
int
wait_for_vxlock(vp, lock)
	register struct vnode *vp;
	int lock;
{
	int slept = 0;

	LASSERT(SLOCK_HOLDER(&vp->v_lock));
	while (vp->v_flag & VXLOCK) {
		vp->v_flag |= VXWANT;
		assert_wait((int)&vp->v_flag, FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
		slept = 1;
	}
	if ((slept == 0) && (lock == VX_LOCK))
		vp->v_flag |= VXLOCK;
	return (slept);
}


/*
 * Lookup a mount point by filesystem identifier.
 *
 * getvfs returns a mount point with its unmount lock locked for 
 * reading in the normal case.  This is done so that the caller has a
 * guarantee that the filesystem will not disappear during
 * the course of subsequent operations.  The caller must
 * dispose of the mount structure by calling UNMOUNT_READ_UNLOCK()
 * Without this guarantee, it would be possible for the filesystem
 * to become unmounted at almost any time while the caller does
 * its business.
 *
 * If the file system is in the process of being unmounted, this
 * function will pend until the operation is complete, and then
 * retry (and most likely fail, assuming the unmount was successful).
 *
 */
struct mount *
getvfs(fsid)
	fsid_t *fsid;
{
	register struct mount *mp;

start:
	MOUNTLIST_LOCK();
#ifdef OSF1_ADFS
	if (mp = mounth.m_next) 
#else
        if (mp = rootfs)
#endif
          do {
                  if (mp->m_stat.f_fsid.val[0] == fsid->val[0] &&
                      mp->m_stat.f_fsid.val[1] == fsid->val[1]) {
                          /*
                           * Try to acquire the mount points UNMOUNT
                           * lock.  If we sleep waiting, retry.
                           */
                          if (!UNMOUNT_TRY_READ(mp)) {
                                  MOUNTLIST_UNLOCK();
                                  thread_block();
                                  goto start;
                          }
                          MOUNTLIST_UNLOCK();
                          return(mp);
                  }
                  mp = mp->m_next;
#ifdef OSF1_ADFS
          } while (mp != (struct mount *)&mounth);
#else
          } while (mp != rootfs);
#endif
	MOUNTLIST_UNLOCK();
	return (NULLMOUNT);
}
                        
/*
 * Set vnode attributes to VNOVAL
 *
 * MP: assume that vap is locked, if necessary.
 */
#undef vattr_null
void
vattr_null(vap)
	register struct vattr *vap;
{

	vap->va_type = VNON;
	vap->va_symlink = (char *) 0;
	vap->va_mode = vap->va_nlink = vap->va_uid = vap->va_gid =
		vap->va_fsid = vap->va_fileid = vap->va_size =
		vap->va_size_rsv = vap->va_blocksize = vap->va_rdev =
#ifdef	OSF1_ADFS
		vap->va_node = 
#endif
		vap->va_bytes = vap->va_bytes_rsv =
		vap->va_atime.tv_sec = vap->va_atime.tv_usec =
		vap->va_mtime.tv_sec = vap->va_mtime.tv_usec =
		vap->va_ctime.tv_sec = vap->va_ctime.tv_usec =
		vap->va_flags = vap->va_gen = VNOVAL;
}

/*
 * Initialize a nameidata structure
 */
#if	MACH
ndinit(ndp, utnd)
#else
ndinit(ndp)
#endif
	register struct nameidata *ndp;
#if	MACH
	register struct utask_nd *utnd;
#endif
{

	bzero((caddr_t)ndp, sizeof(struct nameidata));
	ndp->ni_iov = &ndp->ni_nd.nd_iovec;
	ndp->ni_iovcnt = 1;
	ndp->ni_base = (caddr_t)&ndp->ni_dent;
	ndp->ni_rw = UIO_WRITE;
	ndp->ni_segflg = UIO_SYSSPACE;
	/*
	 * Default the segflg of the embedded uio structure to UIO_SYSSPACE.
	 * The ni_segflg is just for the pathname itself.
	 */
	ndp->ni_uioseg = UIO_SYSSPACE;
#if	MACH
	/*
	 * NOTE:
	 * In Mach, the ni_cdir, ni_rdir, and ni_cred are all in the
	 * utask structure encapsulated in a utask_nd structure.  The
	 * nameidata structure points at it.
	 * If a utask_nd ptr was passed in, use it; otherwise, use the
	 * one in the uarea.
	 * WARNING:  this routine should not be used by anyone expecting
	 * the new nameidata structure to outlast the current process.
	 * It's for temporary use of nameidata (e.g. rename) only.
	 */
	if (!utnd)
#ifdef	OSF1_SERVER
		utnd = &(u.u_utnd); 
#else	/* OSF1_SERVER */
		utnd = &(u.utask->uu_utnd); 
#endif	/* OSF1_SERVER */
	ndp->ni_utnd = utnd;

#ifdef	OSF1_ADFS
	ndp->ni_allocbuf = 0;
#endif

#endif
}

/*
 * Duplicate a nameidata structure
 */
nddup(ndp, newndp)
	register struct nameidata *ndp, *newndp;
{

#if	MACH
	/*
	 * Callers of this routine MUST have allocated ni_utnd, and
	 * pointed the nameidata structure at it.
	 */
	ASSERT(newndp->ni_utnd != 0);
	ndinit(newndp, newndp->ni_utnd);
#else
	ndinit(newndp);
#endif
	UTND_LOCK(ndp->ni_utnd);
#ifdef	OSF1_ADFS
	newndp->ni_cdirproxy = ndp->ni_cdirproxy;
	vnode_proxy_init(&newndp->ni_cdirproxy);
	newndp->ni_rdirproxy = ndp->ni_rdirproxy;
	vnode_proxy_init(&newndp->ni_rdirproxy);
#else
	newndp->ni_cdir = ndp->ni_cdir;
	VREF(newndp->ni_cdir);
	newndp->ni_rdir = ndp->ni_rdir;
	if (newndp->ni_rdir)
		VREF(newndp->ni_rdir);
#endif
	UTND_UNLOCK(ndp->ni_utnd);
	newndp->ni_cred = ndp->ni_cred;
	crhold(newndp->ni_cred);
}

/*
 * Release a nameidata structure
 */
ndrele(ndp)
	register struct nameidata *ndp;
{

#ifdef OSF1_ADFS
	remote_vrele(&ndp->ni_cdirproxy);
	remote_vrele(&ndp->ni_rdirproxy);
	ASSERT(ndp->ni_allocbuf == 0);
#else
	vrele(ndp->ni_cdir);
	if (ndp->ni_rdir)
		vrele(ndp->ni_rdir);
#endif
	crfree(ndp->ni_cred);

}

/*
 * Routines having to do with the management of the vnode table.
 */
struct vnode *vfreeh, **vfreet;
extern struct vnodeops dead_vnodeops, spec_vnodeops;

#if	MACH
/*
 * These variables are all initialized in machine-dependent code.
 */
extern int	path_num_max;		/* pathname translation buffers */
/* int	ucred_max;			   credentials */
extern int	nmount_max;		/* mount structures */
#endif

long numvnodes;
struct vattr va_null;

struct vnode *
vnode_new()
{
	register struct vnode *vp;

	/*
	 * Allocate a new vnode from the vnode table zone.
	 * Keeping track of its limits.
	 */
	vp = (struct vnode *) zalloc(vnode_zone);
	nvnode++;
	vnode       = (struct vnode *) MIN((u_int) vp, (u_int) vnode); 
	vnodeNVNODE = (struct vnode *) MAX((u_int) vp, (u_int) vnodeNVNODE); 

	/*
	 * Initialize the vnode structure.
	 */
#ifdef	OSF1_ADFS
	vp->v_magic = 0;
	vp->v_mscount = 0;
	vp->v_seqno = 0;
#endif
	vp->v_op = &dead_vnodeops;
	vp->v_mount = DEADMOUNT;
	vp->v_type = VBAD;
	vp->v_id = 0;
	vp->v_numoutput = 0;
#if	MACH
	/*
	 * initialize vm_info structures
	 */
	vp->v_vm_info = VM_INFO_NULL;
	vm_info_init(vp);
#endif
	VN_LOCK_INIT(vp);
	VN_OUTPUT_LOCK_INIT(vp);
	VN_BUFLISTS_LOCK_INIT(vp);
#if	VFS_VIO
	VN_WIP_QUEUE_INIT(vp);
#endif

	vp->v_freef = NULLVP;
	vp->v_freeb = (struct vnode **) 0;

	return(vp);
}

/*
 * Initialize the vnode structures and initialize each file system type.
 * MP:  this routine is called at system startup with no other threads
 * active in the filesystem.
 */
vfsinit()
{
	register struct vnode *vp = vnode;
	struct vfsops **vfsp;
	extern void spec_init();
	extern struct vfsops dead_vfsops;
	struct mount *mp;
	u_int  vnode_size = (u_int)(sizeof(struct vnode) + vn_maxprivate - 4);
	u_int  nvnode_conf;
	/*
	 * Initialize zone for the vnode table and record that
	 * the table is empty until zalloc() is called (in vnode_new()).
	 */
	vnode_zone = zinit(vnode_size, 0, nvnode*vnode_size, "vnode_table");
	if (vnode_zone == NULL)
		panic("vfsinit: unable to zinit vnode zone");
	nvnode_conf = nvnode;
	nvnode = 0;
	vnode = (struct vnode *) -1;
	vnodeNVNODE = (struct vnode *) 0;

	/*
	 * Build vnode free list.
	 */
	vp = vnode_new();
	vfreeh = vp;
	vfreet = &vp->v_freef;
	vp->v_freeb = &vfreeh;
	for (numvnodes = 1; numvnodes < nvnode_conf; numvnodes++) {
		vp = vnode_new();
		*vfreet = vp;
		vp->v_freeb = vfreet;
		vfreet = &vp->v_freef;
	}

	/*
	 * Initialize global file system locks.
	 */
	VN_FREE_LOCK_INIT();
	MOUNTLIST_LOCK_INIT();
	VFSSW_LOCK_INIT();

#ifdef OSF1_ADFS
        MOUNTCOUNT_LOCK_INIT();
        mount_count = 0;

#if	VFS_VIO
	/*
	 * Initialize the VIO module.
	 */
	vio_init();		
#endif
#endif

#if FULLSERVER
	/*
	 * Initialize data structures for handling special files
	 */
	spec_init();
	/*
	 * Initialize the vnode name cache
	 */
	nchinit();
	/*
	 * Initialize the vattr structure template
	 */
	vattr_null(&va_null);

#if	MACH
	/*
 	* Initialize virtual file system data structures, primarily zones.
 	* Initially, these will be neither exhaustible nor pageable.
 	* These attributes should be examined further.  For now, we
 	* panic if they run out, which is fine for debug, but not
 	* production.
 	* The sleepable attribute is not used at this time, so is a noop.
 	*/

	/*
	 * Pathname lookup buffers.  These are pretty transient, and
	 * VERY heavily used.
	 */
	pathname_zone = zinit(MAXPATHLEN,
			      path_num_max*MAXPATHLEN,
			      4 * PAGE_SIZE,
			      "pathbufs");
	if (pathname_zone == (zone_t) NULL)
		panic("vfsinit: no zones1");

	/*
	 * Mount structures.  These tend to stick around for a while
	 * once allocated, but that doesn't happen often.
	 */
	mount_zone = zinit(sizeof(struct mount),
			      nmount_max*sizeof(struct mount),
			      PAGE_SIZE,
			      "mounts");
	if (mount_zone == (zone_t) NULL)
		panic("vfsinit: no zones2");
	/*
	 * Initialize dead_mount structure
	 */
	mp = DEADMOUNT;
#if	SER_COMPAT
	mp->m_funnel = FUNNEL_NULL;
#endif
	mp->m_op = &dead_vfsops;
	/*
	 * N.B.  m_flag field is 0.  Since "anonymous" vnodes (created
	 *	 by bdevvp) will point to the dead_mount structure, these
	 *	 flags may occasionally be examined.  0 should be safe.
	 */
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_VLIST_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);

	/*
	 * Initialize head of mount list.
	 */
	bzero((caddr_t)&mounth, sizeof(struct mounthead));
	mounth.m_next = (struct mount *)&mounth;
	mounth.m_prev = (struct mount *)&mounth;

	/*
	 * XXX -- security defaults???
	 */
#endif	/* MACH */

	/*
	 * Initialize each file system type.
	 */
	for (vfsp = &vfssw[0]; vfsp <= &vfssw[MOUNT_MAXTYPE]; vfsp++) {
		if (*vfsp == NULL)
			continue;
		(*(*vfsp)->vfs_init)();
	}
#ifndef	OSF1_SERVER
#if	1
	/*
	 * Fake dynamic loading of MFS until we have the time to do
	 * MFS for real.  (We currently load NFS.) 
	 */
	{
		extern struct vfsops mfs_vfsops;

		if (vfssw_add(MOUNT_MFS, &mfs_vfsops))
			printf("Error configuring MFS\n");
	}
#endif
#endif	/* OSF1_SERVER */
#endif  /* FULLSERVER */
}

/*
 * Return the next vnode from the free list.
 * MP notes:
 *	-- vfreeh, vfreet, v_freef, v_freeb can only be accessed under 
 *	   vnode free list lock.
 *	Algorithm:
 *		-- lock vnode free list.
 *		-- conditionally lock free vnode, skipping locked 
 *		   and VXLOCK'd vnodes.  This avoids races with vgone.
 *		-- remove from free list and unlock freelist.
 *		-- if not already vgone'd, unlock, vgone it, lock again.
 *		-- initialize and reference the vnode.
 *		   (NB: name cache was purged during vgone (vclean)).
 *		-- unlock vnode, put it on a mount list and return.
 *		note:  it is important to atomically check VXLOCK, VBAD,
 *		       and call vgone, without releasing vnode lock, to
 *		       avoid races.
 * Note to callers:
 * This function used to do insmntque to add the new vnode to its new
 * mount list.  To avoid races, and keep caller code cleaner, we don't
 * do this, but require the callers (e.g. iget) to call insmntque when
 * vnode initialization is complete.  Upon return, the vnode is not on
 * any old lists, since vgone removes them.
 */
getnewvnode(tag, vops, vpp)
	enum vtagtype tag;
	struct vnodeops *vops;
	struct vnode **vpp;
{
	register struct vnode *vp, *vq;
	register int locked;

	VN_FREE_LOCK();	
	vp = vfreeh;
	/*
	 * Try to find an unlocked vnode with VXLOCK clear.
	 */
	while (vp) {
		if (((locked = VN_LOCK_TRY(vp)) == 0) || 
		    (vp->v_flag & VXLOCK)) {
			if (locked)
				VN_UNLOCK(vp);
			vp = vp->v_freef;
		} else {
			/*
	 		 * We have locked vnode now, without VXLOCK set.
	 		 * Remove it from freelist. 
			 */
			if (vp->v_usecount)
				panic("free vnode isn't");
			/*
			 * Note: New vnode may not be first on the list
			 */
			if (vq = vp->v_freef)
				vq->v_freeb = vp->v_freeb;
			else
				vfreet = vp->v_freeb;
			*vp->v_freeb = vq;
			vp->v_freef = NULLVP;
			vp->v_freeb = (struct vnode **) 0;
			break;
		}
	}
	if (vp == NULLVP) {
		vp = vnode_new();
		VN_LOCK(vp);
	} else
		numvnodes--;
	/*
	 * Increment usecount here so any racing reactivations will not
	 * try to remove it from the free list (again).  The only known
	 * racer that will do this is vgetm(), if it sneaks in the hole
	 * during which the vnode is unlocked at the end of vgone(), after
	 * clear_vxlock(), but before re-locking the vnode.
	 * If this race it hit, the vgetm() caller will fail later due to
	 * mismatched v_id (can only happen on name cache hits).
	 */
	vp->v_usecount++;
	VN_FREE_UNLOCK();
	if (vp->v_type != VBAD) {
		/*
		 * VXLOCK is clear, don't sleep (doesn't really matter)
		 * Set VX_INACTIVE to keep vgone from mistaking the
		 * reference we just took for a "real" reference.
		 */
		(void) vgone(vp, (VX_NOSLEEP|VX_INACTIVE),
						(struct vnodeops *) 0);
	}
	/*
	 * vnode is locked.
	 * Note:  This vnode CANNOT be found on ANY lists at this
	 *	time.  vgone makes sure of that.  There is no need
	 *	for any further locking, although we hold the vnode
	 *	lock anyway.  The "exception" is the name cache, but
	 *	only the v_id field is accessed, and we don't
	 *	change that.
	 */
#ifdef  OSF1_ADFS
	ASSERT(vp->v_magic == 0);
	ASSERT(vp->v_seqno == 0);
	ASSERT(vp->v_mscount == 0);
#if	VFS_VIO
	ASSERT(queue_empty(&vp->v_wip_list));
#endif
#ifdef	TNC
	vp->v_tncdata = 0;
#endif

#if	MAPPED_FILES | PFS
	vm_info_reinit(vp);
#endif	
#endif	/* OSF1_ADFS */

#if	MACH
	vp->v_vm_info->pager = MEMORY_OBJECT_NULL;
#endif
	vp->v_type = VNON;
	vp->v_flag = 0;
	vp->v_shlockc = 0;
	vp->v_exlockc = 0;
	vp->v_lastr = 0;
	vp->v_socket = 0;
	vp->v_rdcnt = 0;
	vp->v_wrcnt = 0;
	vp->v_tag = tag;
	vp->v_op = vops;
	vp->v_bufhash_shift = 0;
#if	SEC_FSCHANGE
	vp->v_secop = 0;
#endif
#ifdef  OSF1_ADFS
	vp->v_iomode = VIO_BUF;
#endif
	*vpp = vp;
	VN_UNLOCK(vp);
	return (0);
}

/*
 * Free an unused, but referenced vnode and put it at the front 
 * of the vnode free list (for makealias).
 * Takes a vnode with v_usecount of 1.
 */
void
vfree(vp)
	register struct vnode *vp;
{
	VN_LOCK(vp);
	ASSERT(vp->v_usecount == 1);
	--vp->v_usecount;
	VN_FREE_LOCK();
	vp->v_freef = vfreeh;
	vp->v_freeb = &vfreeh;
	if (vfreeh)
		vfreeh->v_freeb = &vp->v_freef;
	else
		vfreet = &vp->v_freef;
	vfreeh = vp;
	numvnodes++;
	VN_FREE_UNLOCK();
	VN_UNLOCK(vp);
}

/*
 * Move a vnode from one mount queue to another.
 * MP:
 *	-- VXLOCK is set on vp (except for call from file system specific
 *	   functions (e.g. iget)), at which time the vnode is unfindable.
 *	-- lock order:  mount vlist lock, then vnode lock. 
 *	-- Vnode is NOT locked.
 *	-- mp cannot be unmounted.
 *	-- v_mount cannot change -- accessed under VXLOCK only.
 *	-- v_mountb, v_mountf under control of mount list lock. 
 */
insmntque(vp, mp)
	register struct vnode *vp;
	register struct mount *mp;
{
	struct vnode *vq;
	register struct mount *svmp;

	ASSERT(vp->v_mount != NULLMOUNT);
#ifdef	OSF1_ADFS
	svmp = vp->v_mount;
#else
	if ((svmp = vp->v_mount) != DEADMOUNT) {
#endif
		MOUNT_VLIST_LOCK(svmp);
		/*
	 	 * Delete from old mount point vnode list, if on one.
		 */
		if (vp->v_mountb) {
			if (vq = vp->v_mountf)
				vq->v_mountb = vp->v_mountb;
			*vp->v_mountb = vq;
		}
		MOUNT_VLIST_UNLOCK(svmp);
#ifndef	OSF1_ADFS
	}
#endif
	/*
	 * Insert into list of vnodes for the new mount point, if available.
	 * MP:  don't need to lock; this is only done here.
	 */
	VN_LOCK(vp);
	vp->v_mount = mp;
	VN_UNLOCK(vp);
#ifndef OSF1_ADFS
	if (mp == DEADMOUNT) {
		vp->v_mountf = NULLVP;
		vp->v_mountb = (struct vnode **) 0;
		return;
	} 
#endif
	MOUNT_VLIST_LOCK(mp);
	if (mp->m_mounth) {
		vp->v_mountf = mp->m_mounth;
		vp->v_mountb = &mp->m_mounth;
		mp->m_mounth->v_mountb = &vp->v_mountf;
		mp->m_mounth = vp;
	} else {
		mp->m_mounth = vp;
		vp->v_mountb = &mp->m_mounth;
		vp->v_mountf = NULLVP;
	}
	MOUNT_VLIST_UNLOCK(mp);
}

/*
 * Grab a particular vnode from the free list, increment its
 * reference count and lock it. 
 *
 * Vgetm returns 0 if it was successful.
 * If the vnode has the VXLOCK flag set, vgetm will return 1 after
 * possibly waiting for the flag to clear, depending upon the wait
 * parameter. 
 *
 * The VXLOCK flag indicates that the vnode is in a state of transition
 * and will be in a different state upon its clearing.
 *
 * vgetm comes in two flavors:
 * In flavor #1 (wait == 1), the process is awakened when VXLOCK is
 * cleared, and an error returned to indicate that the vnode is no 
 * longer usable (possibly having been changed to a new file system type).
 *
 * Flavor #2 (wait == 0) will return an error immediately without sleeping,
 * if VXLOCK is set.  
 * This version was created for those conditions when the sleep/wakeup 
 * synchronization is not necessary.
 *
 * This function is called via two different macros, one which sets
 * the wait parameter to 0, and one which sets it to 1.
 * These are defined in vnode.h as vget() and vget_nowait().
 *	vget(vp) => vgetm(vp, 1)
 *	vget_nowait(vp) => vgetm(vp, 0)
 *
 * MP:  -- vget takes an unlocked vnode; vget_nowait takes a locked vnode.
 *	-- lock order for vnode and freelist locks is (vnode, then free list).
 *
 */
vgetm(vp, wait)
	register struct vnode *vp;
	int wait;
{
	register struct vnode *vq;

	if (wait) {
		VN_LOCK(vp);
		if (wait_for_vxlock(vp, VX_NOLOCK)) {
			VN_UNLOCK(vp);
			return (1);
		}
	} else {
		if (vp->v_flag & VXLOCK)
			return (1);
	}
	LASSERT(SLOCK_HOLDER(&vp->v_lock));
	if (vp->v_usecount == 0) {
		VN_FREE_LOCK();	
		if (vq = vp->v_freef)
			vq->v_freeb = vp->v_freeb;
		else
			vfreet = vp->v_freeb;
		*vp->v_freeb = vq;
		vp->v_freef = NULLVP;
		vp->v_freeb = (struct vnode **) 0;
		numvnodes--;
		VN_FREE_UNLOCK();	
#if	MACH
		vp->v_vm_info->pager = MEMORY_OBJECT_NULL;
#endif
	}
	ASSERT(vp->v_freef == NULLVP);
	ASSERT(vp->v_freeb == (struct vnode **) 0);
	vp->v_usecount++;	/* vnode is locked */
	if (wait)
		VN_UNLOCK(vp);
	return (0);
}

#if	MACH_ASSERT
/*
 * Vnode reference, just increment the count
 */
void
vref(vp)
	struct vnode *vp;
{

	VN_LOCK(vp);
	ASSERT(vp->v_freef == NULLVP);
	ASSERT(vp->v_freeb == (struct vnode **) 0);
	if (vp->v_usecount > 10000) {
		vprint("vref: usecount > 10000", vp);
		panic("vref");
	}
	vp->v_usecount++;
	VN_UNLOCK(vp);
}
#endif

/*
 * Vnode release.
 * If count will drop to zero, call inactive routine, then re-check
 * the usecount; if still 1, decrement count and return to freelist.  
 *
 * Synchronization with vgone and vclean:
 *	Two cases in vrele:
 *		1.  v_usecount > 1 => simply decrement and return.
 *		2.  v_usecount == 1 => we're going to call VOP_INACTIVE
 *		  a) If VXLOCK not set, just call through.
 *		     We are making the filesystem-specific vn_inactive
 *		     functions responsible for dealing with this race, which
 *		     means that they can be called simultaneously on the
 *		     same vnode, but ONLY from vclean.  The inactive 
 *		     function can detect this race by checking for VXLOCK.
 *		  b) If VXLOCK is set, then we sleep on it until it's done.
 *		     Even though ref. count is > 0, and we KNOW that
 *		     the inactive function will be called from vclean.  
 *		     There is a hole between the setting of VXLOCK and the
 *		     checking of the usecount in vclean.  If
 *		     we simply decrement the usecount and return, it could 
 *		     be that we are in that hole, which would mean that
 *		     the inactive function would not be called.
 *
 *	The bottom line is that the inactive functions need to synchronize
 *	themselves.
 *
 *	NOTE:  we could put VINACTIVATING back in, but that would cause
 *		forcible unmount to fail if you're hung in the inactive
 *		routine (e.g. nfs server not available).  VINACTIVATING
 *		synchronized inactive and vclean (set here, vclean slept
 *		on it).
 */
void
vrele(vp)
	register struct vnode *vp;
{
	int error;	/* For VOP_INACTIVE call */

	if (vp == NULLVP)
		panic("vrele: null vp");
	VN_LOCK(vp);
	if (vp->v_usecount > 1) {
		vp->v_usecount--;
		VN_UNLOCK(vp);
		return;
	}
	ASSERT(vp->v_usecount == 1);
#ifdef  OSF1_ADFS
#ifdef	TNC
	/*
	 *  Waiting for "extra" vrefs to be released?
	 *  Then restore the "non-extra" file references.
	 */
	if ( vp->v_flag & VXREFWAIT ) {
		ASSERT(V_FCOUNT(vp) && vp->v_magic == V_MAGIC);
		vp->v_usecount = V_FCOUNT(vp) + 1;
		VN_UNLOCK(vp);
		thread_wakeup(&V_FCOUNT(vp));
		return;
	}
	if ( vp->v_tncdata ) {
		free(vp->v_tncdata);
		vp->v_tncdata = NULL;
	}
#endif	/* TNC */
	/*
	 * If this is the last reference on the vnode, it should
	 * not have an attached port.
	 */
	ASSERT(vp->v_magic == 0);
	ASSERT(vp->v_mscount == 0);
	ASSERT(vp->v_seqno == 0);
#endif

#if	MAPPED_FILES && MACH_ASSERT
	/*
	 * If this is the last reference on the vnode, make sure the 
	 * v_vm_info structure is sane.
	 */
	vm_info_check(vp);
#endif
	if (vp->v_usecount < 1) {
		/* do we keep this paranoia in the shipped system? */
		vprint("vrele: bad ref count\n", vp);
		panic("vrele: bad ref count");
	}
	if (vp->v_flag & VXLOCK)
		wait_for_vxlock(vp, VX_NOLOCK);
	else {
		VN_UNLOCK(vp);
		VOP_INACTIVE(vp, error);
		VN_LOCK(vp);
	}
	/*
	 * Decrement and re-check use count.  
	 * Someone could have done a legitimate
	 * vget() during the VOP_INACTIVE.  If 0, add to free list.
	 */
	if (--vp->v_usecount == 0) {
		ASSERT(vp->v_freef == NULLVP);
		ASSERT(vp->v_freeb == (struct vnode **) 0);
#ifdef  OSF1_ADFS
		/*
		 * If this is the last reference on the vnode, it should
		 * not have an attached port.
		 */
		ASSERT(vp->v_magic == 0);
#endif
		VN_FREE_LOCK();	
		if (vfreeh == (struct vnode *)0) {
			/*
			 * insert into empty list
			 */
			vfreeh = vp;
			vp->v_freeb = &vfreeh;
		} else {
			/*
			 * insert at tail of list
			 */
			*vfreet = vp;
			vp->v_freeb = vfreet;
		}
		vp->v_freef = NULLVP;
		vfreet = &vp->v_freef;
		numvnodes++;
		VN_FREE_UNLOCK();	
	}
	VN_UNLOCK(vp);
}

#if	MACH_ASSERT
/*
 * Page or buffer structure gets a reference.
 */
vhold(vp)
	register struct vnode *vp;
{

	VN_LOCK(vp);
	vp->v_holdcnt++;
	VN_UNLOCK(vp);
}

/*
 * Page or buffer structure frees a reference.
 */
holdrele(vp)
	register struct vnode *vp;
{

	VN_LOCK(vp);
	if (vp->v_holdcnt <= 0) {
		vprint("holdrele: bad count", vp);
		panic("holdrele: holdcnt");
	}
	vp->v_holdcnt--;
	VN_UNLOCK(vp);
}
#endif	/* MACH_ASSERT */

/*
 * Remove any vnodes in the vnode table belonging to mount point mp.
 *
 * If FORCECLOSE is specified, there should not be any active ones,
 * return error if any are found (nb: this is a user error, not a
 * system error). If FORCECLOSE is specified, detach any active vnodes
 * that are found.
 * Assumptions:
 *	-- No path translation
 *	-- No syncs
 *	-- List can change due to close/exit, vnode_pager activity
 *	   Deal with this by "goto loop" -- not great, but it works.
 * MP:
 *	-- Lock order:  mount vlist lock, then vnode lock.
 */
int busyprt = 0;	/* patch to print out busy vnodes */

vflush(mp, skipvp, flags)
	struct mount *mp;
	struct vnode *skipvp;
	int flags;
{
	register struct vnode *vp, *nvp;
	int busy;

loop:
	busy = 0;
	MOUNT_VLIST_LOCK(mp);
	for (vp = mp->m_mounth; vp; vp = nvp) {
		nvp = vp->v_mountf;
		/*
		 * Skip over a selected vnode.
		 * Used by ufs to skip over the quota structure inode.
		 */
		if (vp == skipvp)
			continue;
		VN_LOCK(vp);
		if ((flags & SKIPSYSTEM) && (vp->v_flag & VSYSTEM)) {
			VN_UNLOCK(vp);
			continue;
		}
			
#if	MACH 
		/*
		 * XXX Should be calling mf_uncache and inode_uncache_try
		 * with assurance that the vnode won't be recycled.
		 */
#if	MAPPED_FILES
		if (MF_MAPPABLE(vp)) {
			/*
			 * Wait for the mapped file module to uncache the file.
			 */
			VN_UNLOCK(vp);
			MOUNT_VLIST_UNLOCK(mp);
			mf_uncache(vp, TRUE);
			MOUNT_VLIST_LOCK(mp);
			VN_LOCK(vp);
		} else
#endif
			/*
			 * XXX
			 * This is a problem.  The following is not a
			 * synchronous call, so the vnode will not be
			 * unreferenced until "later".  This must be
			 * fixed so we can make unmount work properly.
			 */
			if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL) {
				VN_UNLOCK(vp);
				MOUNT_VLIST_UNLOCK(mp);
				(void)inode_uncache_try(vp);
				MOUNT_VLIST_LOCK(mp);
				VN_LOCK(vp);
			}

		/*
		 * Vnode should still be around.  Our being in this code
		 * will lock any other forced unmounts out of here, but
		 * it could have been decremented.  The check on the
		 * usecount below should deal with that.  
		 */
#endif
		/*
		 * With an unused vnode, all we need to do is vgone it.
		 * We need to start over, because the mount vlist may
		 * have changed.
		 */
		if (vp->v_usecount == 0) {
			MOUNT_VLIST_UNLOCK(mp);
			(void) vgone(vp, VX_SLEEP, (struct vnodeops *) 0);
			VN_UNLOCK(vp);
			goto loop;
		}
		/*
		 * For block or character devices, revert to an
		 * anonymous device. For all other files, just kill them.
		 */
		if (flags & FORCECLOSE) {
			MOUNT_VLIST_UNLOCK(mp);
			if (vp->v_type != VBLK && vp->v_type != VCHR) {
				(void) vgone(vp, VX_SLEEP, 0);
				VN_UNLOCK(vp);
			} else {
				/* wait for flag -- race with clearalias */
				int ret = wait_for_vxlock(vp, VX_LOCK);
				VN_UNLOCK(vp);
				if (ret == 0) {
					vclean(vp, 0, &spec_vnodeops);
					/*
					 * Problem: someone may be
					 * looking at v_mount on a 
					 * referenced vnode, assuming it's
					 * valid.  To make forcible unmount
					 * really work, we need to be able
					 * to deal with a changing v_mount;
					 * currently, we assume it's
					 * read-only.
					 */
					insmntque(vp, DEADMOUNT);
					clear_vxlock(vp);
				}
			}
			goto loop;
		}
		VN_UNLOCK(vp);
		if (busyprt)
			vprint("vflush: busy vnode", vp);
		busy++;
	}
	MOUNT_VLIST_UNLOCK(mp);
	if (busy)
		return (EBUSY);
	return (0);
}

/*
 * Disassociate the underlying file system from a vnode.
 * Synchronization:  see comments above vrele().
 *	-- Race in inactive functions.  They must synchronize.
 * Assumptions:
 *	-- vnode is unlocked.
 *	-- VXLOCK set.  This prevents other activity while we do
 *	   the dirty work.
 */
void
vclean(vp, flags, newops)
	register struct vnode *vp;
	int flags;
	struct vnodeops *newops;
{
	struct vnodeops *origops;
	int active;

#if	MAPPED_FILES
	if (MF_MAPPABLE(vp))
		/* clean main memory pages - synchronously */
		mf_clean(vp, TRUE);
#endif		

	VN_LOCK(vp);
	ASSERT(vp->v_flag & VXLOCK);

	/*
	 * Check to see if the vnode is in use.
	 * If so we have to reference it before we clean it out
	 * so that its count cannot fall to zero and generate a
	 * race against ourselves to recycle it.
	 * If VX_INACTIVE flag is set, don't treat the vnode as active.
	 */
	if (active = (vp->v_usecount && !(flags & VX_INACTIVE)))
		vp->v_usecount++;
	if (flags & VX_DOCLOSE) {
		VN_UNLOCK(vp);
		vinvalbuf(vp, 1);
		VN_LOCK(vp);
		ASSERT(vp->v_numoutput == 0);
	}
	/*
	 * Prevent any further operations on the vnode from
	 * being passed through to the old file system.
	 */
	origops = vp->v_op;
	vp->v_op = newops;
	vp->v_tag = VT_NON;
	VN_UNLOCK(vp);
	/*
	 * If purging an active vnode, it must be closed
	 * and deactivated before being reclaimed.
	 *
	 * The vn_inactive call here won't race with any out of vrele
	 * because this would only be called with an already-referenced
	 * vnode, and vrele only calls VOP_INACTIVE for unreferenced ones.
	 *
	 * NOTE:  we don't know the mode (FREAD or FWRITE of a file 
	 * 	  being closed here, so we can't intelligently change
	 *	  the v_rdcnt and/or v_wrcnt in the vnode.  It doesn't
	 *	  really matter, since if this function is being called
	 *	  then access to the vnode is going to be an error,
	 *	  except for device files, for which the rdcnt and wrcnt
	 *	  are not used (yet...).
	 */
	if (active) {
		if (flags & VX_DOCLOSE)
			(*(origops->vn_close))(vp, 0, NOCRED);
		(*(origops->vn_inactive))(vp);
	}
	/*
	 * Reclaim the vnode.
	 */
	if ((*(origops->vn_reclaim))(vp))
		panic("vclean: cannot reclaim");
	/*
	 * vn_reclaim may have done this already, but we must be sure.
	 */
	cache_purge(vp);

	if (active)
		vrele(vp);
}


/*
 * Eliminate all activity associated with a vnode
 * in preparation for reuse.
 * Assumptions:
 *	-- Vnode is locked.
 *	-- VXLOCK was not set by caller, but it could be set (race).
 * Algorithm:
 *	-- set VXLOCK upon entering, clear it upon return.
 *	-- Return value == 0 if VXLOCK was not set upon entering.
 *	-- Return value == 1 if VXLOCK was set upon entering.
 *	-- If VXLOCK was set, and sleep == VX_SLEEP, sleep on it, then
 *	   return.
 *	-- If VXLOCK was set, and sleep == VX_NOSLEEP, return immediately.
 *	-- It is easier for some callers to avoid checking for VBAD (an
 *	   already-vgone'd vnode), so we do it here, and simply return 0.
 * MP:
 *	-- lock order:  SPECHASH_LOCK, then vnode lock.
 *			vnode lock, then vnode freelist lock.
 *
 */
#undef sleep		/* Avoid conflict with the sleep macro of param.h */
vgone(vp, flags, ops)
	register struct vnode *vp;
	int flags;
	struct vnodeops	*ops;
{
	register struct vnode *vq;

	/*
	 * Paranoia -- we better not be vgone'ing the root device vnode
	 *		or root directory.
	 */
	ASSERT((vp != rootvp) && (vp != rootdir));
	/*
	 * vnode is locked
	 */
	LASSERT(SLOCK_HOLDER(&vp->v_lock));
	
	/*
	 * If a vgone (or vclean) is already in progress,
	 * sleep until it's clear if VX_SLEEP was passed in, otherwise
	 * return now.  wait_for_vxlock returns 1 if it slept.
	 */
	if ((vp->v_flag & VXLOCK) && (!(flags & VX_SLEEP)))
		return (1);
	if (wait_for_vxlock(vp, VX_LOCK))
		return (1);

	/*
	 * There is a slim possibility that this vnode was already
	 * vgone'd, in which case, it will have v_type == VBAD.
	 */
	if (vp->v_type == VBAD) {
		VN_UNLOCK(vp);
		clear_vxlock(vp);
		VN_LOCK(vp);
		return (0);
	}

	VN_UNLOCK(vp);
	/*
	 * Clean out the filesystem specific data.
	 */
	if (ops == (struct vnodeops *) 0)
		ops = &dead_vnodeops;

	vclean(vp, (flags | VX_DOCLOSE), ops);
	/*
	 * Delete from old mount point vnode list, if on one.
	 * v_mount only changes under VXLOCK, or when the vnode is
	 * unavailable, so we can use at it here.
	 */
	if (vp->v_mount != DEADMOUNT)
		insmntque(vp, DEADMOUNT);

	VN_LOCK(vp);
	/*
	 * If it is on the freelist, move it to the head of the list.
	 * A locked vnode with VXLOCK set and v_usecount of 0 MUST
	 * be on the vnode free list.
	 */
	if (vp->v_usecount == 0) {
		VN_FREE_LOCK();
		if (vp->v_freeb && vfreeh != vp) {
			if (vq = vp->v_freef)
				vq->v_freeb = vp->v_freeb;
			else
				vfreet = vp->v_freeb;
			*vp->v_freeb = vq;
			vp->v_freef = vfreeh;
			vp->v_freeb = &vfreeh;
			vfreeh->v_freeb = &vp->v_freef;
			vfreeh = vp;
		}
		VN_FREE_UNLOCK();
	}
	vp->v_type = VBAD;
	/*
	 * wake up sleepers.  clear_vxlock() expects unlocked vnode.
	 */
	VN_UNLOCK(vp);
	clear_vxlock(vp);
	/*
	 * we return from this function with vp locked, since we were
	 * called with it locked.
	 */
	VN_LOCK(vp);
	return (0);
}

/*
 * vfssw_add and vfssw_del.
 * Routines to manipulate the VFS switch structure.
 *
 * These functions are intended to be called by filesystem configuration
 * entry points to add and delete file systems from the VFS switch.
 *
 * vfssw_add(fstype, fsops)
 *	short fstype;		file system identifier (index into vfssw)
 *	struct vfsops *fsops;	pointer to VFS operations
 *
 * Algorithm:
 *	1.  Insert in VFS switch, if not there already.
 *	2.  Call file system's initialization function; if an error is
 *	    returned, then the file system is removed from the switch.
 *
 * Notes:
 * 	At this time, the implementation requires that the fstype argument
 * 	be an index into the vfssw.  As a result, a given file system will
 *	always be in the same place in the VFS switch.
 *
 * vfssw_del(fstype)
 *	short fstype;	file system identifier (index into vfssw)
 *
 * Algorithm:
 *	1.  Be sure that there are no active mounts of this file system type.
 *	2.  Remove the file system operations from the VFS switch.
 *
 * Notes:
 * 	It is assumed that if a file system has cleanup to do, it will
 *	do that after calling this function.
 *
 * Lock Synchronization:
 *	The VFSSW_READ_LOCK must be held during the mount operation.
 *	The VFSSW_WRITE_LOCK must be held during vfssw_add and vfssw_del.
 *	The VFSSW_*_LOCK has precedence over any other file system lock.
 */
vfssw_add(fstype, fsops)
	short fstype;
	struct vfsops *fsops;
{
	int error = 0;

	if (fstype < 0 || fstype > MOUNT_MAXTYPE)
		return (EINVAL);
	VFSSW_WRITE_LOCK();
	if (vfssw[fstype] == (struct vfsops *) 0) {
		vfssw[fstype] = fsops;
		if (error = (*(fsops)->vfs_init)())
			vfssw[fstype] = (struct vfsops *) 0;
	} else
		error = EBUSY;
	VFSSW_WRITE_UNLOCK();
	return (error);
}

vfssw_del(fstype)
	short fstype;
{
	register struct mount *mp;
	int error = 0;

	if (fstype < 0 || fstype > MOUNT_MAXTYPE)
		return (EINVAL);
	VFSSW_WRITE_LOCK();
	if (vfssw[fstype] != (struct vfsops *) 0) {
		MOUNTLIST_LOCK();
		mp = rootfs;
		do {
			if (mp->m_stat.f_type == fstype)
				break;
			mp = mp->m_next;
		} while (mp != rootfs);
		MOUNTLIST_UNLOCK();
		if (mp == rootfs)
			/* no active mounts */
			vfssw[fstype] = (struct vfsops *) 0;
		else
			error = EBUSY;
	} else
		error = ENODEV;
	VFSSW_WRITE_UNLOCK();
	return (error);
}

/*
 * Mark a mount point as busy.
 * Used to synchronize access and to delay unmounting (during sync()).
 * If the wait parameter is false, don't delay, return a non-zero value
 * immediately.
 */
int
vfs_busy(mp, wait)
struct mount *mp;
int wait;
{
	MOUNT_LOCK(mp);
	while (mp->m_flag & M_MPBUSY) {
		if (!wait) {
			MOUNT_UNLOCK(mp);
			return(1);
		}
		mp->m_flag |= M_MPWANT;
		assert_wait((int)&mp->m_flag, FALSE);
		MOUNT_UNLOCK(mp);
		thread_block();
		MOUNT_LOCK(mp);
	}
	mp->m_flag |= M_MPBUSY;
	MOUNT_UNLOCK(mp);
	return(0);
}

/*
 * Free a busy filesystem.
 * Used to synchronize access and to delay unmounting (during sync()).
 */
void
vfs_unbusy(mp)
struct mount *mp;
{
	int dowakeup;

	MOUNT_LOCK(mp);
	ASSERT(mp->m_flag & M_MPBUSY);
	mp->m_flag &= ~M_MPBUSY;
	if (mp->m_flag & M_MPWANT) {
		mp->m_flag &= ~M_MPWANT;
		dowakeup = 1;
	} else
		dowakeup = 0;
	MOUNT_UNLOCK(mp);
	if (dowakeup)
		thread_wakeup((int)&mp->m_flag);
}

/*
 * Print out a description of a vnode.
 * MP:  no locking here.  It's only done under debug or panic situations.
 */
static char *typename[] =
   { "VNON", "VREG", "VDIR", "VBLK", "VCHR", "VLNK", "VSOCK", "VFIFO", "VBAD" };

void
vprint(label, vp)
	char *label;
	register struct vnode *vp;
{
	int indx = (int) vp->v_type;
	int error;

	if (label != NULL)
		printf("%s: ", label);
	printf("type %s, usecount %d, refcount %d,", typename[indx],
		vp->v_usecount, vp->v_holdcnt);
	if (vp->v_flag)
		printf(" flags =0x%X", vp->v_flag);
	printf("\n\t");
	VOP_PRINT(vp, error);
}

#ifdef OSF1_ADFS
/*
 * Given a rdev value, we will return the mountid for the file system that was
 * mounted corresponding to the block special device that has that rdev.
 */
int
getmntid(devnode, rdev, mntid)
        node_t     	devnode;
        dev_t      	rdev;
	ulong_t		*mntid;		
        
{
#if FULLSERVER

	register struct mount	*mp;

	MOUNTLIST_LOCK();
        mp = mounth.m_next;
        do {
                /*
                 * HACK ALERT - We use a pretty bad hack out here to look for 
                 * the mount structure we are interested. We know that for UFS
                 * the f_fsid.val[0] is set to the rdev of the corresponding 
                 * block special device and so we use that out here. Otherwise,
                 * we would have to do this in a ufs specific call.
                 * The correct solution would be to make it a vfsop, although
                 * currently in AD this function has valid semantics only for 
                 * ufs.
                 */
#ifndef	PFS
		if ((mp->m_stat.f_type == MOUNT_UFS) &&
		    (mp->m_stat.f_fsid.val[1] == NODE_2_FSID1(devnode,
							      MOUNT_UFS)) &&
		    (mp->m_stat.f_fsid.val[0] == rdev)) {
#else
		if (((mp->m_stat.f_type == MOUNT_UFS) ||
		     (mp->m_stat.f_type == MOUNT_PFS)) &&
		    (mp->m_stat.f_fsid.val[1] == NODE_2_FSID1(devnode,
							      MOUNT_UFS)) &&
		    (mp->m_stat.f_fsid.val[0] == rdev)) {
#endif
                        /* found what we are looking for */
                        *mntid = mp->m_mountid;
                        break;
                }
                mp = mp->m_next;
        } while (mp != (struct mount *)&mounth);

        MOUNTLIST_UNLOCK();
#endif /* FULLSERVER */
        return(ESUCCESS);
}

/*
 * flush out and remove vnodes for all mounted file systems.  Called only
 * during a reboot(), primarily to sync out mapped files.
 */
void
vflushall()
{
	register struct mount *mp;

	MOUNTLIST_LOCK();
	mp = mounth.m_next;
	do {
		if (!UNMOUNT_TRY_READ_NOSLP(mp))
			goto skip;
		MOUNT_LOCK(mp);
		if ((mp->m_flag & (M_RDONLY|M_REMOTE_FS)) != 0) {
			MOUNT_UNLOCK(mp);
			goto next;
		}
		MOUNT_UNLOCK(mp);
		MOUNTLIST_UNLOCK();
		mntflushbuf(mp, 0);
		vflush(mp, NULLVP, 0);
		MOUNTLIST_LOCK();
next:
		UNMOUNT_READ_UNLOCK(mp);
skip:
		mp = mp->m_next;
	} while (mp != (struct mount *)&mounth);
	MOUNTLIST_UNLOCK();
}
#endif
