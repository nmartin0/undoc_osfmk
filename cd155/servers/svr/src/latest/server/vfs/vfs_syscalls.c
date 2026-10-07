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
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This source file was modified by the Center for High Performance 
 * Computing (CHPC) on behalf of OSF.
 *
 */
/*
 * HISTORY
 * $Log: vfs_syscalls.c,v $
 * Revision 1.30  1995/02/01  22:35:40  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.29  1994/12/15  18:26:08  jlitvin
 * rmdir of any remote I/O node mount point will return EINVAL instead of
 * EBUSY.  Have them return EBUSY instead.  This has the side effect of
 * having the bootnode return EBUSY for "/" instead of EINVAL.  This is
 * against UNIX convention, but not against specifications for POSIX.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11761
 *  Testing: VSX
 *  Module(s): server/vfs/vfs_syscalls.c
 *
 * Revision 1.28  1994/11/18  20:50:47  mtm
 * Copyright additions/changes
 *
 * Revision 1.27  1994/06/21  17:11:24  cfj
 * quotactl() now returns EOPNOTSUPP if quotas are not configured.
 *
 *  Reviewer:dbm
 *  Risk:L
 *  Benefit or PTS #:8400
 *  Testing:
 *  Module(s):server/vfs/vfs_syscalls.c
 *
 * Revision 1.26  1994/05/27  16:12:42  brad
 * Fixed long-standing merge botch ... PFS includes were inside a TNC #ifdef.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: server builds with PFS defined and TNC not defined.
 *  Testing: built server
 *  Module(s): server/vfs/vfs_syscalls.c
 *
 * Revision 1.25  1994/05/23  23:47:07  suri
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 9373
 *  Testing: Functional verification performed on polaris
 *  Module(s): vfs/vfs_syscalls.c (mount1())
 *             cmds_libs/src/usr/sbin/mount/mount.c
 *  Solution: Modified mount syscall so that it now returns EBUSY when the
 *            refcount on the mountpoint is > 1.
 *
 * Revision 1.24  1994/04/05  20:55:49  brad
 * Merged revision 1.19.2.3 from the R1.2 branch into R1.3.
 *
 * Revision 1.19.2.3  1994/04/05  20:39:35  brad
 * Fixed bug introduced in Rev 1.19.2.1.  Don't decrement vnode reference
 * count if we're cleaning up from a failed PFS file creation.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 8857
 *  Testing: Reproduced problem as described in bug report, verified fix.
 *     Ran PFS EATs on 64 nodes.
 *  Module(s): server/vfs/vfs_syscalls.c
 *
 * Revision 1.23  1994/03/14  02:09:20  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.22  1994/03/11  20:28:38  rlg
 * Merged the changes in the R1.2 branch into the R1.3 branch (1.19.2.2).
 *
 * Revision 1.19.2.2  1994/03/11  16:40:54  rlg
 * The utimes() function for PFS files was reimplemented following the model
 * of pfs_multi_stat(), so that the header file and all stripe files have the
 * same value set in the access and modification time fields.  The old code
 * only set these fields in the header file.
 *
 *  Reviewer:  Brad Rullman
 *  Risk: medium
 *  Benefit or PTS #:  PTS #6870
 *  Module(s):  emulator/fsvr_user_side.c
 *              emulator/pfs2_user_side.c
 *              server/uxkern/fsvr.defs
 *              server/uxkern/pfs2.defs
 *              server/uxkern/fsvr_server_side.c
 *              server/uxkern/pfs2_server_side.c
 *              server/vfs/vfs_syscalls.c
 *
 * Revision 1.21  1994/03/03  18:08:01  brad
 * Merged revision 1.19.2.1 from the R1.2 branch into R1.3.
 *
 * Revision 1.19.2.1  1994/03/03  01:16:22  brad
 * Need to bump up vnode reference count when unlinking a PFS file since
 * the unlink is multi-staged ... this prevents the vnode from being
 * freed and reallocated out from under us.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 6779
 *  Testing: Ran PTS test case on 8, 16, and 64 nodes multiple times with
 *             no failures.  Ran PFS EATs on 64 nodes.
 *  Module(s): server/vfs/vfs_syscalls.c, server/uxkern/mfs_prim.c
 *
 * Revision 1.20  1994/01/11  18:26:30  jlitvin
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
 * Revision 1.19  1993/11/15  23:52:19  dbm
 *  Reviewer:None
 *  Risk:Low
 *  Benefit or PTS #:7140. Added code to pfs_fdevstat to verify that the type
 * 			of object the file pointer is pointing to is a
 * 			vnode.  Also added code to return the file mode
 * 			so that the file type can be determined on the
 * 			client side.
 *  Testing: Verified with test case.  Tested with UFS/PFS/and NFS files
 * 	  to verify worked correctly.  Also tested with sockets and
 * 	  pty's.
 *  Module(s):vfs_syscalls.c -- function pfs_fdevstat().
 *
 * Revision 1.18  1993/09/30  01:28:09  brad
 * Fix for bug 6792: error out properly on EXDEV error if the source of a hard
 * link is a PFS file (i.e. don't leave the PFS_LOCK locked).
 *
 * Revision 1.17  1993/09/27  04:39:59  robboy
 * Iffed out functions with #if FULLSERVER, in case the functions that
 * call them are also iffed out.
 *
 * Revision 1.16  1993/09/02  02:22:45  brad
 * Fix for bug 6147: fixed a race condition in unlink() when multiple nodes
 * try to unlink the same PFS file simultaneously.
 *
 * Revision 1.15  1993/09/01  01:39:19  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.14  1993/08/25  01:27:07  brad
 * Fix for bug #6267: don't remove stripe data if the target of a rename() is
 * a PFS file and the target is identical to the source.
 *
 * Revision 1.13  1993/08/18  21:44:19  brad
 * Added PFS_UNLOCK if an error occurs during a rename() operation and the
 * source file is a PFS file.  This fixes bug #6209.
 *
 * Revision 1.12  1993/07/19  23:08:07  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.11  1993/07/15  18:23:05  brad
 * Fixed bug that caused PFS stripe data to be removed when the link count
 * on the inode was greater than 1 (shouldn't remove stripe data in this
 * case).  Also added synchronization code to unlink(), link(), and rename()
 * when the file is a PFS file to resolve potential race conditions due to
 * in-progress remote stripefile operations.
 *
 * Revision 1.10  1993/07/14  18:46:45  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.9  1993/07/09  21:01:04  brad
 * Added PFS support to rename().
 *
 * Revision 1.1.1.3  1993/07/01  21:09:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/06/16  22:29:10  brad
 * Added initialization of m_iomode in mount structure during file system
 * mounts, otherwise it never gets initialized in non-UFS/non-PFS file
 * systems.
 *
 * Revision 1.7  1993/05/06  20:32:18  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6  1993/04/03  03:13:10  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.5  1993/02/24  19:51:22  cfj
 * DEV_TAB <0>0 bug fix from OSF.
 *
 * Revision 1.1.2.1.2.8  1993/03/10  05:59:24  brad
 * Added PFS support for chmod, chown, ... etc.  VOP_PREALLOC now returns
 * an actual.
 *
 * Revision 1.1.2.1.2.7  1993/02/23  04:38:34  brad
 * Added PFS support for access() and truncate()/ftruncate().
 *
 * Revision 1.1.2.1.2.6  1993/02/16  20:08:59  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.1.2.1.2.5  1993/02/09  21:41:39  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.4  1993/01/19  17:22:06  brad
 * Fixed bug in _lsize() resulting from mf_get_size_and_offset() interface
 * change by OSF.
 *
 * Revision 1.1.2.1.2.4  1993/01/12  05:02:11  brad
 * Fixed bug in _lsize() resulting from changed mf_get_size_and_offset()
 * interface.  Added temporary check for PFS file until PFS supports lsize.
 *
 * Revision 1.1.2.1.2.3  1993/01/08  02:19:01  brad
 * Added PFS support to unlink().
 *
 * Revision 1.1.2.1.2.2  1992/12/16  06:06:06  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:07:22  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:17:30  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.2  1992/11/30  22:58:07  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:46:43  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.30  1992/10/14  17:22:28  cfj
 * NX integration.
 *
 * Revision 2.36  93/08/05  16:09:22  bolsen
 * Fixed the following bug:
 * [Bug 269/3616] unlink(2) returns wrong errno (checkin for toman).
 * 
 * Revision 2.35  93/07/13  16:14:41  slively
 *      Revision 2.36  93/06/29  16:18:53  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.34  93/06/25  11:28:09  slively
 * Backout the LITE server changes.  Remove the #if FILESERVER.
 * 
 * Revision 2.33  93/06/24  13:30:39  mjl
 * Add Locus copyright notice (again).  Change paranoid panic to an ASSERT().
 * 
 * Revision 2.32  93/06/22  20:10:38  slively
 * Support for LITE server. #if FILESERVER sections and include fileserver.h.
 * 
 * Revision 2.31  93/04/24  18:46:16  klh
 * 	Revision 2.35  93/04/06  11:57:54  rabii
 * 		Turn off NOCACHE flag for namei of target in rename
 *
 * 	Revision 2.34  93/03/30  16:12:12  roy
 * 		Use the VIO_GROW_DIRECT macro to determine if vtruncate can
 * 		use VOP_SETATTR to grow a file.
 * 		[93/03/11            roy]
 *
 * 	Revision 2.33  93/02/26  11:12:08  rabii
 * 		Have mknod pass in NONODE to lower layers so they can distingusih
 * 		between rmknod with node 0, and mknod (rabii)
 *
 * 	Revision 2.32  93/02/02  15:28:49  rabii
 * 		Re-do interface to mount, mount() now obeys single server semantics,
 * 		while OSF/1 AD type mounts must call mount_ad (rabii)
 *
 * 	Revision 2.31  92/12/08  10:48:28  durriya
 * 		1.1 based vfs synchronisation changes - changes to unmount, mount
 * 		sync.                                                        (durriya)
 *
 * Revision 2.30  92/11/23  15:57:29  klh
 * 	Revision 2.29  92/11/05  17:28:47  roy
 * 		In mount() pass the full pathname of the mount point to VFS_MOUNT.
 * 		If sync() called on root_fs_node with tag != -1 do nothing.
 * 		(durriya)
 * 
 * Revision 2.29  92/10/05  14:03:03  klh
 * 	Revision 2.28  92/09/11  09:30:21  rabii
 * 		Change calling sequence to remote_getmntid from devstat.
 * 		[92/09/10            roy]
 * 
 * 	Revision 2.27  92/08/26  12:14:41  loverso
 * 		Add support for the FASTPATH flag in mount.
 * 		Got rid of mount_scaffold code (pjg).
 * 
 * 		Fix typo in ASSERT. (loverso)
 * 
 * Revision 2.28  92/08/17  18:12:39  mjl
 * Delete extraneous setting of uth->uu_opn_filep.
 * 
 * Revision 2.27  92/08/17  13:36:45  mjl
 * Pass file pointer to low-level TNC code in copen().
 * XXX Redundant code here???
 * 
 * Revision 2.26  92/08/08  01:55:32  jdh
 * minor changes to support fifos in this transition version
 * (mjl will install real fifo support) -- jdh
 * 
 * Revision 2.25  92/07/29  08:30:27  rabii
 * 	Fix bugs in rename to deallocate tond.ni_pnbuf right after the call
 * 	to namei(tond) but before calling renameit and to correctly handle
 * 	a null pathname in the call to remote_lookup (pjg).
 * 
 * Revision 2.24  92/07/16  09:43:35  rabii
 * 	Fix bug in link to call vrele(cdir) instead of remote_vrele.
 * 	Cannot vrele the vnode proxy because the port is the cdir port so
 * 	no reference was acquired on it, therefore we cannot call
 * 	mach_port_deallocate on it (pjg).
 * 
 * Revision 2.23  92/06/30  22:48:18  loverso
 * 	Added NULL ruid and rgid parameters to credentials_set_state(). (rabii)
 * 
 * Revision 2.22  92/06/08  18:31:59  pjg
 * 	Pass node # to getmntid       (durriya)
 * 
 * Revision 2.21  92/05/31  19:00:05  loverso
 * 	Use VOP_SETATTR to extend files in vtruncate (MAPPED_FILES only so far).
 * 	[92/05/29            roy]
 * 
 * Revision 2.20  92/05/18  12:31:48  roy
 * 	fsync calls vflushbuf for MAPPED_FILES.
 * 	[92/05/12            roy]
 * 
 * Revision 2.19  92/05/12  00:07:38  loverso
 * 	Several more fixes to rename. Apply the same fixes to link, where
 * 	applicable. Get rid of ni_vp1, use ni_cdir instead (pjg).
 * 
 * Revision 2.17  92/04/07  13:47:47  pjg
 * 	In rename, add vrele(tdvp) to release the reference acquired with
 * 	PORT_TO_VNODE_LOOKUP.re
 * 
 * Revision 2.16  92/04/05  17:12:04  pjg
 * 	Fixes to rename related to the new vnode proxies (pjg).
 * 
 * 	Eliminate extra copyout in stat1. add devstat     (durriya)
 * 
 * 	Add extra (null) parameter to the call to credentials_set_state().
 * 	(roman)
 * 
 * Revision 2.15  92/03/15  14:41:41  roy
 * 	92/03/09  10:27:39  roy
 * 	Use VOP_SETSIZE to extend a file, if it's a mappable file.
 * 
 * 	92/03/03  16:55:34  roy
 * 	Changes for MAPPED_FILES.
 * 
 * 	92/03/02  17:09:06  rabii
 * 	Added new credentials interface
 * 
 * Revision 2.14  92/03/09  13:58:51  durriya
 * 	Revision 3.9  91/12/18  17:19:05  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.13  92/03/01  18:45:28  pjg
 * 	Fix typo.  (loverso)
 * 
 * 	92/02/28  16:38:46  noemi
 * 	Changed mount and unmount to use mount structure ports instead of
 * 	covered vnode and file system root vnode ports.
 * 
 * Revision 2.12  92/02/21  16:47:47  durriya
 * 	get rid of NORMA_IPC conditionals
 * 
 * Revision 2.11  92/01/16  16:12:38  roy
 * 	getfsstat fix.
 * 
 * Revision 2.10  92/01/14  10:55:04  roy
 * 	92/01/07  17:09:06  noemi
 * 	Added OSF1_ADFS version of getfsstat.
 * 
 * 	92/01/06  20:40:47  noemi
 * 	Changed statfs and getfh to work with stubs.
 * 
 * 	92/01/05  21:12:08  noemi
 * 	Added OSF1_ADFS version of sync.  Fixed utimes.
 * 
 * 	92/01/01  17:13:12  avery
 * 	AVK: link code was fixed using rename code as guide. Specifdically,
 * 	rindex call and remote_lookup code were cleaned up.
 * 
 * 	92/01/01  01:14:53  noemi
 * 	Removed syscall_on_master.
 * 
 * Revision 2.9  92/01/05  20:01:37  roy
 * 	91/12/21  07:12:32  noemi
 * 	Removed bogus code that was trying to detect renaming a mount point.
 *
 * 	91/12/19  23:50:46  noemi
 * 	Changed to use mounth as a structure.  Removed some debugging code.
 * 	Changes for new chdir, chroot, and fchdir.
 * 
 * 	91/12/12  20:27:02  pjg
 * 	Added mount scaffolding under ifdef OSF1_ADFS_DEBUG and a patchable
 * 	variable (mount_scaffold).
 * 
 * 	91/12/11  14:46:18  noemi
 * 	Updated to use norma_port_location_hint.
 * 
 * 	91/12/05  23:39:29  noemi
 * 	Added rmknod function (from rabii@osf.org).  Added mknod1 to handle
 * 	the common code between mknod and rmknod.
 *
 * 	91/11/12  19:29:13  noemi
 * 	Replaced RPC calls with calls to functions sending RPCs.
 *
 * 	91/09/23  21:53:36  noemi
 * 	Removed the referencing of the covered vnode in the mount update path
 * 	from both the OSF1/ADFS and OSF/1 portions of the code.
 *
 * 	91/09/22  18:38:04  noemi
 * 	Distributed fileserver changes: major changes to mount; minor changes
 * 	to unmount; rewrote dounmount; added new mount_update, set_unmount
 * 	and clear_unmount functions.
 *
 * 	91/08/29  18:57:59  noemi
 * 	OSF1_ADFS_DEBUG coding for kludged mount.
 *
 * Revision 2.8  91/12/16  21:08:08  roy
 * 	91/12/05  08:42:34  sp
 * 	BUG 49: move the copying of the null string terminator to the emulator 
 * 	for readlink.
 * 
 * 	91/10/30  17:43:58  bernadat
 * 	Horrible hack for AFS 3.0 binary compatibility with MACH 2.5.
 * 
 * 	91/10/23  16:39:16  condict
 * 	Remove unnecessary get_time calls.  The global time var now works 
 * 	correctly.
 * 
 * Revision 2.7  91/12/13  10:25:46  roy
 * 	91/12/04  16:33:46  roy
 * 	For mapped files, call vn_rdwr() when growing in vtruncate.
 * 
 * Revision 2.6  91/11/26  13:38:05  rabii
 * 	Removed VOP_RMKNOD and replaced ti with extended VOP_MKNOD
 * 
 * Revision 2.5  91/11/25  11:25:24  rabii
 * 	Implemented rmknod for remote devices
 * 
 * Revision 2.4  91/10/04  15:31:37  chrisp
 * Add Locus copyright notice.
 * 
 * Revision 2.3  91/09/16  16:43:12  sjs
 * integrate Locus changes
 * 
 * Revision 2.2  91/08/31  14:32:06  rabii
 *      Initial V2.0 Checkin
 *
 * Revision 3.5  91/08/01  17:04:08  sp
 * Upgrade to 1.0.2
 *
 * Revision 1.13.3.3  91/02/26  12:03:23  gmf
 *      If attempting to unmount root, return
 *      EBUSY, not EINVAL.
 *      [91/02/12  16:25:03  gmf]
 *
 * Revision 1.13.3.2  91/02/01  10:46:32  gmf
 *      Modify checks for mandatory file locking.
 *      Modify update_venf_lock to do the right thing.
 *      [90/11/15  15:01:29  gmf]
 *
 * Revision 1.13.2.2  91/02/01  10:42:09  gmf
 *      Modify checks for mandatory file locking.
 *      Modify update_venf_lock to do the right thing.
 *      [90/11/15  15:01:29  gmf]
 *
 * Revision 1.13  90/10/31  14:08:54  devrcs
 *      comment on *chflags
 *      [90/10/25  16:59:02  gmf]
 *
 *      1.  Removed new vmountread interface changes.
 *      2.  Removed unused mount update synch. code
 *          and comments.
 *      [90/10/24  14:40:17  nags]
 *
 *      Make chown follow symbolic links, per AES (and new Posix specs)
 *      [90/10/16  16:11:46  lwa]
 *
 *      Add missing error check to vtruncate().
 *      [90/10/16  09:09:56  dlb]
 *
 *      changed M_FORCE to M_FMOUNT
 *      [90/10/13  21:46:29  gmf]
 *
 *      Pass STRIPSLASH nameiop to namei for mkdir, rmdir, and
 *      rename (when renaming a directory).  This results in
 *      an extra namei operation for rename.
 *      [90/10/13  12:46:25  gmf]
 *
 *      Change truncate() and ftruncate() to extend files if the new length is
 *      longer than the current one.  Rationalized them to call a common
 *      subroutine, vtruncate(), to do most of the work.
 *      [90/10/12  15:07:20  dlb]
 *
 *      Readlink now appends a null to the character string returned if there
 *      is room for the null in the user's buffer.
 *      [90/10/11  14:40:56  morris]
 *
 *      Allow the file system specific layer to handle invalid seek offsets.
 *      Return EPERM on attempts to link to a directory instead of EISDIR.
 *      [90/10/08  17:12:48  collins]
 *
 * Revision 1.12  90/10/07  15:00:58  devrcs
 *      Return EBUSY when attempting to rename over a mount point.
 *      [90/10/01  14:51:39  collins]
 *
 *      Added EndLog Marker.
 *      [90/09/28  11:55:45  gm]
 *
 *      Return EPERM when attempting to unlink a directory instead of EISDIR.
 *      [90/09/27  17:14:22  collins]
 *
 *      Check for invalid fmode in saccess.
 *      [90/09/21  11:11:13  collins]
 *
 * Revision 1.11  90/09/23  16:01:49  devrcs
 *      Check for leaf files to be on different filesystems
 *      in rename if case when both exist.  The lack of this
 *      check caused panic in ufs_rename in rename(foo, bar)
 *      when bar was a mount point.
 *      [90/09/17  16:59:32  gmf]
 *
 *      Truncate and ftruncate now return EINVAL for all non-regular files.
 *      [90/09/12  22:00:09  morris]
 *
 *      vmountwait() returns whether or not it blocked.  vmountread() takes
 *      an atomic argument and returns without doing the MOUNT_LOOKUP for
 *      atomic cases when vmountwait() blocked.
 *      [90/09/12  15:35:52  noemi]
 *
 *      Quotactl released a reference on the wrong vnode.
 *      For lock debugging purposes, unlock the mount structure
 *      before deallocating it.
 *      [90/09/08  19:04:59  nags]
 *
 *      Added update_venf_lock() to update the vnode flag VENF_LOCK to
 *      indicate if enforcement mode file locking is enabled on a file
 *      Chmod and fchmod update the vnode flag VENF_LOCK to indicate if
 *      mandatory file locking is enabled for the file.
 *      truncate and ftruncate return EAGAIN if enforcement mode locks exist on
 *      the file.
 *      [90/09/06  14:29:47  swallace]
 *
 *      Keep a reference on the filesystem when
 *      invoking VOP_QUOTACTL.
 *      [90/09/03  22:39:02  nags]
 *
 * Revision 1.10  90/08/24  12:30:46  devrcs
 *      Changes for new system call interface.
 *      Removed include of syscontext.h
 *      Removed ustat.
 *      Make creat ocreat for binary compat.  It's now a library call to open.
 *      Remove xutimes (get rid of cmu system call table).
 *      Changes for u_file_state.
 *      [90/08/17  17:49:15  gmf]
 *
 *      Recognize clean flag override on mount.
 *      Temporary fix to getvnode so it won't return
 *      prematurely; this fix will be obsoleted by
 *      upcoming system call interface changes.
 *      [90/08/19  00:16:07  nags]
 *
 *      doc change only
 *      [90/08/14  19:51:47  hosking]
 *
 *      Added compat version of open.  Only difference is that the old
 *      version will cause a controlling tty to be implicitly assigned.
 *      Also, made the Ultrix utimes behavior the default (NULL pointer
 *      means current time).
 *      Fixed saccess() to fail if any of the requested modes are not
 *      permitted.
 *      Corrected logic error in revoke permission check (|| needed to be &&).
 *      [90/08/12  12:13:11  ers]
 *
 * Revision 1.9  90/07/27  09:10:08  devrcs
 *      Remove ofile array from getvnode interface.
 *      [90/07/20  17:10:08  nags]
 *
 *      Prohibit hard links on directories.
 *      [90/07/17  08:53:54  nags]
 *
 *      nags merge
 *
 *      Condensed history:
 *      Secureware changes.                             seiden@osf.org
 *      Parallelized for OSF/1.                         nags@encore.com
 *      Make utimes binary compatible with Ultrix.      gmf@osf.org
 *      Many file layer parallelization changes.        noemi@osf.org
 *      Locks on uarea and file structures.             noemi@osf.org
 *      Eof fix for readdir.                            gmf@osf.org
 *      Connectathon fixes for mount.                   gmf@osf.org
 *      Mount update and umount compatibility fixes.    noemi@osf.org
 *      Support for FIFOs (named pipes).                ers@osf.org
 *      Added System V ustat().                         bet@osf.org
 *      Integrated 4.4BSD filesystem changes [1/5/90].  noemi@osf.org
 *      Fixes for first snapshot.                       gm@osf.org
 *      BSD4.4 Changes.                                 gm@osf.org
 *      Locking protocols derived from Encore Mach/0.6. mach@encore.com
 *      Source from 4.4 Berkeley Standard Distribution. mkm@berkeley.edu
 *      [90/06/12  21:44:08  nags]
 *
 * Revision 1.8  90/07/17  11:43:51  devrcs
 *      Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 *      [90/07/10  22:04:53  seiden]
 *
 * Revision 1.7  90/06/22  20:56:52  devrcs
 *      Post-nags-merge bug fixes
 *      [90/06/18  09:58:49  seiden]
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
 *	@(#)vfs_syscalls.c	7.36 (Berkeley) 1/3/90
 */
#if 	MACH
#include <mach_nbc.h>
#endif
#include <mach_ldebug.h>
#include <mapped_files.h>
#include <quota.h>

#include <sys/secdefines.h>
#include <mach_afs_30.h>

#if	SEC_BASE
#include <sys/security.h>
#endif
#if	SEC_ARCH
#include <sys/secpolicy.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <kern/macro_help.h>
#include <ufs/ufsmount.h>
#include <sys/lock_types.h>
#ifdef  OSF1_SERVER
#include <fullserver.h>
#include <sys/synch.h>
#endif
#if	MACH
#include <kern/zalloc.h>
#include <kern/mfs.h>
#include <builtin/inode_pager.h>
#include <kern/assert.h>
#else
#include <sys/malloc.h>
#endif

#if     UNIX_LOCKS
#include <sys/ucred.h>
#endif

#ifdef	OSF1_ADFS
#include <sys/buf.h>			
#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/sthread.h>
#endif

#ifdef	TNC
#include <sys/user.h>
#endif

#ifdef	PFS
#include <pfs/pfs.h>
#include <pfs/pfs_stat.h>
#endif

#define	TEXT_CACHE_CLEAR(mp)	/* vm_object_cache_clear() */
#define	HAS_PAGING_INFO(vp)	((vp)->v_vm_info->pager != MEMORY_OBJECT_NULL)
#define	UNCACHE_PAGING_INFO(vp)	inode_uncache(vp)

static void update_venf_lock();
static struct vnode *mount_lookupname();

/*
 * local macro
 */
#define rofs(mp)	((mp)->m_flag & M_RDONLY != 0)

#ifdef  OSF1_ADFS_DEBUG
#define VT_DEBUG        0x10
int mount_scaffold = 0;
#endif

struct mounthead mounth;		/* head of the mount list */

/*
 * Virtual File System System Calls
 */

/*
 * mount system call
 *
 * Synchronization:  mount must wait for VMOUNTING to clear on the
 * (possibly covered) vnode.  Mount then sets VMOUNTING, causing
 * new pathname and file handle translations to sleep.  Next, mount
 * checks whether there's already a filesystem mounted.  This is
 * legal for remount, which must then acquire the filesystem's
 * m_lookup_lock (causing mount to wait for active translations to
 * finish).  When mount is all done, it must clear VMOUNTING from
 * the (possibly covered) vnode and wake up waiting translators.
 */
mount(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int	type;
		char	*dir;
		int	flags;
		caddr_t	data;
#ifdef OSF1_ADFS
                char    *fdir;
#endif
	} *uap = (struct args *) args;

	register struct nameidata *ndp = &u.u_nd;

	/*
	 * Get vnode to be covered
	 */
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#ifdef	OSF1_ADFS
	/* This path is ONLY taken by afs files in OSF/1 AD */
	ASSERT(ndp->ni_cdir != NULL);
	ASSERT(uap->type == MOUNT_AFS);
#endif
#if	SEC_ARCH
	return mount1(p, args, retval, (tag_t *) 0);
#else	/* !SEC_ARCH */
	return mount1(p, args, retval);
#endif	/* !SEC_ARCH */
}

#ifdef OSF1_ADFS
mount_ad(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct nameidata *ndp = &u.u_nd;
	/*
	 * Get vnode to be covered
	 */
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ASSERT(ndp->ni_cdir != NULL);
#if	SEC_ARCH
	return mount1(p, args, retval, (tag_t *) 0);
#else	/* !SEC_ARCH */
	return mount1(p, args, retval);
#endif	/* !SEC_ARCH */
}
#endif	/* OSF1_ADFS */

mount1(p, args, retval, tags)
	struct proc *p;
	void *args;
	int *retval;
#if	SEC_ARCH
	tag_t *tags;
#endif	/* !SEC_ARCH */
{
	register struct args {
		int	type;
		char	*dir;
		int	flags;
		caddr_t	data;
#ifdef OSF1_ADFS
                char    *fdir;
#endif
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *cvp = NULLVP;
	register struct mount *mp;
	int error, flag, mount_update, clear_mount;

#ifdef  OSF1_ADFS_DEBUG
	struct mount *pseudo_mp;
#endif

#if SEC_BASE
	/*
	 * Must have the mount privilege.
	 */
	if (!privileged(SEC_MOUNT, EPERM))
		return (EPERM);
#else
	/*
	 * Must be super user
	 */
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	ndp->ni_dirp = uap->dir;
#if SEC_BASE
	/*
	 * Indicate that the following namei deals with the
	 * second pathname argument to the system call.
	 */
	audstub_path2();
#endif

	error = namei(ndp);
	if (error)
		return(error);

#if SEC_BASE
	/*
	 * Indicate that a subsequent namei will deal with the
	 * first pathname argument to the system call.
	 */
	audstub_path1();
#endif
	cvp = ndp->ni_vp;
	mount_update = 0;
	clear_mount = 0;

 	if (uap->flags & M_UPDATE) {
		mount_update++;
		if ((cvp->v_flag & VROOT) == 0) {
			vrele(cvp);
			return (EINVAL);
		}
		mp = cvp->v_mount;
		/*
		 * We rely on our reference on the root vnode to keep
		 * the filesystem intact.  Forcible unmount could
		 * affect this.  If we're racing a forcible unmount,
		 * cvp->v_mount could be DEADMOUNT, but we should only
		 * get an error, nothing worse.
		 *
		 * We allow going from read-only to read-write,
		 * but not from read-write to read-only.
		 */
		MOUNT_LOCK(mp);
		if ((mp->m_flag & M_RDONLY) == 0 &&
		    (uap->flags & M_RDONLY) != 0) {
			MOUNT_UNLOCK(mp);
			error = EOPNOTSUPP;	/* Needs translation */
			vrele(cvp);
			return(error);
		}
		flag = mp->m_flag;
		mp->m_flag |= M_UPDATE;
		goto update;
	}

        ASSERT(cvp != NULLVP);
	/* 
	 * See if we're racing another mount on this vnode
	 */
	VN_LOCK(cvp);
	if (cvp->v_flag & VMOUNTPOINT) {
		error = EBUSY;
		VN_UNLOCK(cvp);
		goto out;
	}
	cvp->v_flag |= VMOUNTPOINT;
	clear_mount++;

	if (cvp->v_type != VDIR) {
		error = ENOTDIR;
                VN_UNLOCK(cvp);
		goto out;
	}

	if (cvp->v_usecount > 1) {
		error = EBUSY;
                VN_UNLOCK(cvp);
		goto out;
	}
	VN_UNLOCK(cvp);

#if	MACH_AFS_30
	uap->type = (unsigned long)afs_check_mount(uap->type, uap->data);
#endif	MACH_AFS_30


	if ((unsigned long)uap->type > MOUNT_MAXTYPE ||
	    vfssw[uap->type] == (struct vfsops *)0) {
		error = ENODEV;
		goto out;
	}

	vinvalbuf(cvp, 1);

	/*
	 * IMPORTANT SYNCHRONIZATION NOTES:
	 * The vnode is not advertised as a mount point (v_mountedhere set)
	 * until the mount is complete.  Until that time, any translations
	 * through it will return the original vnode.  Destructive
	 * operations (rmdir, unlink, rename) will fail if they detect
	 * the VMOUNTPOINT flag, which is already set.  There is, however,
	 * a small timing window after the namei() above, but before the
	 * setting of the flag, during which a destructive operation can
	 * complete.  If this happens, we will have a mount point that is
	 * invisible to pathname translation.  Unmount will work, but it needs
	 * to detect the error from namei() and search the mount list
	 * instead, looking for a name match in the f_mntonname field.
	 *
	 * A racing mount will fail when it detects the VMOUNTPOINT flag.
	 *
	 * A racing unmount will either get the new, root vnode, and
	 * complete successfully, or get the old vnode and fail because
	 * it's not mounted yet (v_mountedhere is not set).
	 */

	/*
	 * Allocate and initialize the file system.
	 * The mount initialization can be done without lock because
	 * no one else knows about the mount structure yet;
	 */
	MOUNT_ALLOCATE(mp);
	mp->m_op = vfssw[uap->type];
	mp->m_flag = 0;
	mp->m_exroot = 0;
	mp->m_mounth = (struct vnode *)0;
#ifdef	OSF1_ADFS
	mp->m_iomode = VIO_BUF;
#endif

#if SEC_FSCHANGE
	/*
	 * If this is an lmount, copy the global tag pool into the mount
	 * structure, else mark it as a labeled filesystem.  The M_SECURE
	 * flag is checked later in the sec_ufsmountcheck hook function
	 * to be sure it is consistent with whether or not the mounted
	 * volume is actually labeled.
	 */
	if (tags)
		bcopy(tags, mp->m_tag, sizeof mp->m_tag);
	else
		mp->m_flag |= M_SECURE;
#endif /* SEC_FSCHANGE */
	mp->m_vnodecovered = cvp;

	MOUNT_VLIST_LOCK_INIT(mp);
	UNMOUNT_LOCK_INIT(mp);
	MOUNT_LOCK_INIT(mp);
	MOUNT_LOCK(mp);		/* necessary because of update */

update:
	/*
	 * Set the mount level flags.
	 */
	LASSERT(MOUNT_LOCK_HOLDER(mp));
	if (uap->flags & M_RDONLY)
		mp->m_flag |= M_RDONLY;
	else
		mp->m_flag &= ~M_RDONLY;
	if (uap->flags & M_NOSUID)
		mp->m_flag |= M_NOSUID;
	else
		mp->m_flag &= ~M_NOSUID;
	if (uap->flags & M_NOEXEC)
		mp->m_flag |= M_NOEXEC;
	else
		mp->m_flag &= ~M_NOEXEC;
	if (uap->flags & M_NODEV)
		mp->m_flag |= M_NODEV;
	else
		mp->m_flag &= ~M_NODEV;
	if (uap->flags & M_SYNCHRONOUS)
		mp->m_flag |= M_SYNCHRONOUS;
	else
		mp->m_flag &= ~M_SYNCHRONOUS;
	if (uap->flags & M_FMOUNT)
		mp->m_flag |= M_FMOUNT;
	else
		mp->m_flag &= ~M_FMOUNT;
	if (uap->flags & M_FASTPATH)
		mp->m_flag |= M_FASTPATH;
	else
		mp->m_flag &= ~M_FASTPATH;
	MOUNT_UNLOCK(mp);

	/*
	 * Mount the filesystem.
         * For ADFS pass the full pathname of the mount point, not the
         * one relative to the root of the current file system
	 */
#ifdef OSF1_ADFS
	VFS_MOUNT(mp, uap->fdir, uap->data, ndp, error);
#else
	VFS_MOUNT(mp, uap->dir, uap->data, ndp, error);
#endif
	if (mount_update) {
#ifdef	OSF1_ADFS
		ASSERT(error != EREMOTE);
#endif
		MOUNT_LOCK(mp);
		if (error) {
			mp->m_flag = flag;
                        MOUNT_UNLOCK(mp);
		} else {
			mp->m_flag &= ~M_UPDATE;
#ifdef	OSF1_ADFS
			if (mp->m_flag & M_REMOTE_DIR) {
				mach_port_t mountport;
				int mflag;

				/*
				 * We may not need to update the flags in
				 * the mount structures on both nodes,
				 * but we will do so until we verify whether
				 * or not it is necessary. - NNNN
				 */

				mflag = mp->m_flag;
				ASSERT(mp->m_remoteport != MACH_PORT_NULL);
				mountport = mp->m_remoteport;
				MOUNT_UNLOCK(mp);
				error = remote_mount_update(mountport, mflag);

				/*
				 * If the remote node failed to update its
				 * mount structure, restore the original
				 * mount flags.
				 */
				if (error) {
					MOUNT_LOCK(mp);
					mp->m_flag = flag;
                                        MOUNT_UNLOCK(mp);
				}
			} else
#endif
                        MOUNT_UNLOCK(mp);
		} /* if (error) else */
                goto out;
	}

	if (error) {
		MOUNT_DEALLOCATE(mp);
		goto out;
	}

	/*
	 * We've completed the operation successfully.
	 * Now make the mount point known by setting cvp->v_mountedhere
	 * and adding the mp to the mount list.
	 *
	 * Locking: VN_LOCK dominates MOUNTLIST_LOCK().
	 */
	VN_LOCK(cvp);
	cvp->v_mountedhere = mp;
	cache_purge(cvp);
	MOUNTLIST_LOCK();
#ifdef	OSF1_ADFS
	mp->m_next = mounth.m_next;
	mp->m_prev = (struct mount *)&mounth;
	mounth.m_next = mp;
#else
	mp->m_next = rootfs->m_next;
	mp->m_prev = rootfs;
	rootfs->m_next = mp;
#endif
	mp->m_next->m_prev = mp;
	MOUNTLIST_UNLOCK();
        VN_UNLOCK(cvp);

	VFS_START(mp, 0, error);
	return (error);

out:
	ASSERT(cvp != NULLVP);
	if (clear_mount) {
		VN_LOCK(cvp);
		cvp->v_flag &= ~VMOUNTPOINT;
		VN_UNLOCK(cvp);
	}
	vrele(cvp);
	return (error);
}

#ifdef	OSF1_ADFS
/*
 * This function handles remote mount update requests received via mount_update
 * messages.
 */
mount_update(cvp, flag)
struct vnode *cvp;
int flag;
{
	struct mount *mp;

	/*
	 * Locate the mount structure and update its flags field.
	 */
	ASSERT(cvp && cvp->v_mountedhere);
	mp = cvp->v_mountedhere;
	MOUNT_LOCK(mp);
	mp->m_flag = flag;
	MOUNT_UNLOCK(mp);
	return(0);
}
#endif


/*
 * Unmount system call.  Takes a path to the vnode mounted
 * on as argument.
 *
 * There is a very small timing window during which it is
 * possible to mount a filesystem on a file which is being
 * removed, and have both operations succeed.  If this happens,
 * there will be an "invisible" mount point.  This mount point
 * will show up in the mount(8) command, when listing all mount
 * points, however, the mount point itself will not be in the
 * name space.  In order to unmount these filesystems, we detect
 * ENOENT from the namei() call and then search the mount table
 * to try to find a name match.  If found, we can safely use
 * the mount table entry to do the unmount.
 *
 * Most of the work is done by dounmount().
 */
unmount(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*pathp;
		int	flags;
	} *uap = (struct args *) args;
	register struct vnode *vp;
	register struct nameidata *ndp = &u.u_nd;
	struct mount *mp;
	int error;

#if SEC_BASE
	/*
	 * Must have the mount privilege.
	 */
	if (!privileged(SEC_MOUNT, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif

#ifdef  OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ASSERT(ndp->ni_cdir != NULL);
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->pathp;
	error = namei(ndp);
        if (error) {
		if ((error != ENOENT) || 
		    (vp = mount_lookupname(uap->pathp)) == NULLVP)
			return (error);
        } else {
                vp = ndp->ni_vp;
        }

	/*
	 * Must be the root of the filesystem
	 */
	if ((vp->v_flag & VROOT) == 0) {
		vrele(vp);
		return (EINVAL);
	}
	mp = vp->v_mount;
	if (mp == DEADMOUNT) {
		vrele(vp);
		return(ENODEV);
	}
	return (dounmount(vp, mp, uap->flags));
}

#ifdef	OSF1_ADFS
/* 
 * First we synchronize with other unmounts and pathname translations
 * by acquiring the UNMOUNT_LOCK for writing; if we're racing 
 * another unmount() and the other one gets the lock first, it will
 * fail because of the reference we keep on the root vnode; this is
 * necessary so we can be guaranteed that when we acquire the lock the
 * mount point, and lock, are still valid.  The only other users of
 * this lock are in namei() and getvfs(), which are doing 
 * UNMOUNT_TRY_READ, which is safe because they do not touch the mount
 * structure after sleeping.
 *
 * We then synchronize with sync() by calling vfs_busy(). 
 * Then it is safe to try to unmount the filesystem.
 *
 * We need the vp passed in because it's not safe to do the
 * vrele in unmount, before locking the filesystem; if this happened,
 * an entire racing unmount() could complete before we get here, causing
 * us to reference a deallocated mount structure.
 * We vrele the vp here (if it's non-null).
 *
 * N.B.  FORCIBLE UNMOUNT: Waiting for old translations to complete may
 * break forcible unmount.  For example, if a lookup via NFS is in progess
 * to a recalcitrant NFS server, it will hang indefinitely, holding the
 * UNMOUNT_LOCK, thereby preventing the unmount from completing.
 * In order to make this work properly, we need to bypass the locking and
 * vfs_busy() call and provide a different method of synchronization for
 * competing forcible unmount calls.  One of the issues if we don't lock
 * is to make sure that namei() and getvfs() don't access a bad pointer
 * if we've deallocated the mount structure they're currently using.
 */
dounmount(vp, mp, flags)
        struct vnode *vp;
        register struct mount *mp;
        int flags;
{
        register struct vnode  *cvp;
        int error;
	int mflag;
	mach_port_t remoteport = MACH_PORT_NULL;
	register struct nameidata *ndp = &u.u_nd;

	ASSERT(mp != DEADMOUNT && mp != NULLMOUNT);

	BM(MOUNT_LOCK(mp));
	mflag = mp->m_flag;
	cvp = mp->m_vnodecovered;
	if (cvp) {
		/*
		 * If the filesystem is local and not the root filesystem,
		 * disable new pathname translations.
		 */
		ASSERT((mflag & M_REMOTE_DIR) == 0);
		BM(MOUNT_UNLOCK(mp));
                ASSERT(cvp->v_mountedhere == mp);
                ASSERT(cvp->v_flag & VMOUNTPOINT);
                UNMOUNT_WRITE_LOCK(mp);
                vfs_busy(mp,1);            /* synchronise with sync */

	} else if (mflag & M_REMOTE_DIR) {
		/*
		 * If this filesystem is remotely mounted, we are on the
		 * node servicing the filesystem.  However, the covered
		 * vnode is on the node on which the filesystem was mounted.
		 * Send a set_unmount message to that node so it can disable
		 * new pathname translations.
		 */
		ASSERT(mp->m_remoteport != MACH_PORT_NULL);
		remoteport = mp->m_remoteport;
		BM(MOUNT_UNLOCK(mp));
		ASSERT((mflag & M_REMOTE_FS) == 0);
		error = remote_unmount_set(remoteport);
		if (error) {
			vrele(vp);
			return(error);
		}
                UNMOUNT_WRITE_LOCK(mp);
                vfs_busy(mp,1);

		/*
		 * Release the additional reference on the filesystem's root 
                 * vnode acquired when the unmount operation was forwarded.
		 */
		ASSERT(ndp->ni_cdir != NULLVP);
		vrele(vp);
		ASSERT(vp == ndp->ni_cdir);
		/*
		 * Yes, zero the current directory in the dummy proc
		 * struct. The vnode is gone, anyway, so any subsequent
		 * access is bogus.
		 */
		ndp->ni_cdir = NULLVP;
	} else {
		BM(MOUNT_UNLOCK(mp));
		/*
		 * it's the root!
		 */
		vrele(vp);
		return(EBUSY);
	}
	/*
         * We have acquired the unmount lock for writing so now it is 
         * safe to release the vp (the root vnode of the FS).
	 */
	if (vp != NULLVP)
		vrele(vp);

	TEXT_CACHE_CLEAR(mp);           /* remove unused cached binaries */
	cache_purgevfs(mp);     /* remove cache entries for this file sys */
	VFS_SYNC(mp, MNT_WAIT, error);
        if (!error)
                 VFS_UNMOUNT(mp, flags, error);
	if (error) {
                vfs_unbusy(mp);
                UNMOUNT_WRITE_UNLOCK(mp);
                if (!cvp) {
			ASSERT(remoteport != MACH_PORT_NULL);
                        remote_unmount_done(remoteport, error);
                }
                return(error);
	}
        
	if (cvp) {
                unmount_done(cvp, 0);
	} else {
		/*
                 * On the remote side : remove the mount structure from 
                 * the mount list, deallocate the remote
                 * mount port, release the remote unmount_lock,
                 * cler the VMOUNTPOINT flag from the covenred vnode
                 * and remove reference of the mount ptr from the covered 
                 * vnode
                 */
		remote_unmount_done(remoteport, 0);
                /*
                 * On the local node: remove the mount structure from the
                 * mount list and deallocate it before enabling lookups.
                 */
                MOUNTLIST_LOCK();
                if (mp == rootfs)
                  panic("dounmount: unmounting root");
                mp->m_prev->m_next = mp->m_next;
                mp->m_next->m_prev = mp->m_prev;
                MOUNTLIST_UNLOCK();
		/*
		 * Deallocate the send right to the remote mount port and the
		 * receive right to the local mount port.
		 */
                mount_port_deallocate(mp);
                ZFREE(mount_zone, mp);
        }
	return (error);
}

#else	/* OSF1_ADFS */

/*
 * dounmount
 * Do most of the work of an unmount.  This function is shared by
 * ufs and mfs unmounts.
 *
 * First must disable new pathname translations (VMOUNTING) and wait for
 * old ones to complete (MOUNT_DISABLE_LOOKUPS). Then it is safe to try to
 * unmount the filesystem.
 *
 * We need the vp passed in because it's not safe to do the
 * vrele in unmount, before doing vmountset; it could create races. 
 * So we vrele vp here (if it's non-null).
 *
 * N.B.  Waiting for old translations to complete probably
 * breaks forcible unmount.
 */
dounmount(vp, mp, flags)
	struct vnode *vp;
	register struct mount *mp;
	int flags;
{
	register struct vnode  *cvp;
	int error;

	ASSERT(mp != DEADMOUNT && mp != NULLMOUNT);

	if (cvp = mp->m_vnodecovered) {
		VREF(cvp);
		(void)vmountset(cvp);
		/*
		 * could have raced with another unmount
		 */
		if (!cvp->v_mountedhere) {
			error = ENODEV;
			goto out;
		}
	} else {
		/*
		 * it's the root!
		 */
		error = EBUSY;
		goto out;
	}

	MOUNT_DISABLE_LOOKUPS(mp);
	/*
	 * vp, if set, came in referenced (the root vnode of the FS).
	 */
	if (vp != (struct vnode *) 0)
		vrele(vp);
	vrele(cvp);

	TEXT_CACHE_CLEAR(mp);		/* remove unused cached binaries */
	cache_purgevfs(mp);	/* remove cache entries for this file sys */
	VFS_SYNC(mp, MNT_WAIT, error);

	VFS_UNMOUNT(mp, flags, error);
	if (error) {
		MOUNT_ENABLE_LOOKUPS(mp);
		(void)vmountclear(cvp);
		return (error);
	}

	/*
	 * Remove all ways to find the mountpoint:  covered vnode
	 * and mount list.
	 */
	VN_LOCK(cvp);
	cvp->v_mountedhere = NULLMOUNT;
	VN_UNLOCK(cvp);
	MOUNTLIST_LOCK();
	if (mp == rootfs)
		panic("dounmount: unmounting root");
	mp->m_prev->m_next = mp->m_next;
	mp->m_next->m_prev = mp->m_prev;
	MOUNTLIST_UNLOCK();
	(void)vmountclear(cvp);
	vrele(cvp);
#if	MACH_LDEBUG
	MOUNT_ENABLE_LOOKUPS(mp);
#endif
	MOUNT_DEALLOCATE(mp);
	return (error);
out:
	if (cvp) {
		(void)vmountclear(cvp);
		vrele(cvp);
	}
	return (error);
}
#endif	/* OSF1_ADFS */

/*
 * Get file handle system call
 */
getfh(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
		fhandle_t *fhp;
	} *uap = (struct args *) args;

	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	fhandle_t fh;
	int error;

#if SEC_BASE
	/*
	 * Must have the remote privilege.
	 */
	if (!privileged(SEC_REMOTE, EPERM))
		return (EPERM);
#else
	/*
	 * Must be super user
	 */
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	bzero((caddr_t)&fh, sizeof(fh));
	fh.fh_fsid = vp->v_mount->m_stat.f_fsid;
	VFS_VPTOFH(vp, &fh.fh_fid, error);
	vrele(vp);
	if (!error)
		error = copyout((caddr_t)&fh, (caddr_t)uap->fhp, sizeof (fh));
	return (error);
}


/*
 * operate on filesystem quotas
 */
/* ARGSUSED */
quotactl(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char *path;
		int cmd;
		int uid;
		caddr_t arg;
	} *uap = (struct args *) args;
	register struct mount *mp;
	register struct nameidata *ndp = &u.u_nd;
	struct vnode *vp;
	int error;

#if	!QUOTA
	return (EOPNOTSUPP);
#else
	/*
	 * namei will pend at the filesystem's root if
	 * an unmount is in progress so a quotactl operation
	 * can't interfere with a pending unmount.  However,
	 * an unmount can begin after a quotactl operation starts,
	 * so the filesystem-specific unmount routine must handle
	 * potential unmount/quotactl races.
	 *
	 * We defer the vrele until after the quota operation
	 * so that we may hang onto our reference on the filesystem
	 * across the lower layer's operation.  Otherwise, the
	 * filesystem could become unmounted in the normal way while
	 * we do our thing.  (Forcible unmount is another case entirely.)
	 */
#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->path;
	if (error = namei(ndp))
		return (error);
	mp = ndp->ni_vp->v_mount;
	vp = ndp->ni_vp;
	VFS_QUOTACTL(mp, uap->cmd, uap->uid, uap->arg, error);
	vrele(vp);
	return (error);
#endif /* QUOTA */
}

/*
 * get filesystem statistics
 *
 * Holding the referenced vnode returned by namei prevents
 * the filesystem from becoming unmounted.
 */
statfs(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char *path;
		struct statfs *buf;
	} *uap = (struct args *) args;
	register struct mount *mp;
	struct statfs sf, *sp;
	register struct nameidata *ndp = &u.u_nd;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->path;

	if (error = namei(ndp))
		return (error);
	mp = ndp->ni_vp->v_mount;
	VFS_STATFS(mp, error);
	if (error) {
		vrele(ndp->ni_vp);
		return (error);
	}
	MOUNT_LOCK(mp);
	mp->m_stat.f_flags = mp->m_flag & M_VISFLAGMASK;
	MP_ONLY(sf = mp->m_stat);
	MOUNT_UNLOCK(mp);
	MP_ONLY(sp = &sf);
	UNI_ONLY(sp = &mp->m_stat);
	error = copyout((caddr_t)sp, (caddr_t)uap->buf, sizeof(*sp));
	vrele(ndp->ni_vp);
	return (error);
}

/*
 * Holding the referenced vnode returned by getvnode prevents
 * the filesystem from becoming unmounted in the normal case.
 */
fstatfs(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		struct statfs *buf;
	} *uap = (struct args *) args;
	struct mount *mp;
	struct statfs sf, *sp;
	int error;

	if (error = getvnode(fp))
		return (error);
	mp = ((struct vnode *)fp->f_data)->v_mount;
	VFS_STATFS(mp, error);
	if (error)
		goto out;
	MOUNT_LOCK(mp);
	mp->m_stat.f_flags = mp->m_flag & M_VISFLAGMASK;
	MP_ONLY(sf = mp->m_stat);
	sf = mp->m_stat;
	MOUNT_UNLOCK(mp);
	MP_ONLY(sp = &sf);
	UNI_ONLY(sp = &mp->m_stat);
	error = copyout((caddr_t)sp, (caddr_t)uap->buf, sizeof(*sp));
out:
	FP_UNREF(fp);
	return(error);
}



#ifdef	OSF1_ADFS

/*
 * get statistics on all filesystems
 *
 * The Unmount read lock protects filesystem from being unmounted
 * while VFS_STATFS does its work.
 */
getfsstat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		struct statfs *buf;
		long bufsize;
		int flags;
		int	tag;
	} *uap = (struct args *) args;
	register struct mount *mp;
	register struct statfs *sp;
	struct statfs sf;
	caddr_t sfsp;
	long count, maxcount, tmpcount, error = 0;
	extern	node_t	root_fs_node, this_node;
	u_long getfssid;
	int	flag;

	*retval = 0;
	maxcount = uap->bufsize / sizeof(struct statfs);
	sfsp = (caddr_t)uap->buf;
	if (sfsp && maxcount == 0)
		return (0);
	count = 0;
	MOUNTLIST_LOCK();

	if (this_node == root_fs_node) 	{
		/*
		 * Remote filesystems mounted on the root fileserver are
		 * handled by the local scan of the mount list.
		 */
		if (uap->tag != -1) {
			MOUNTLIST_UNLOCK();
			return(0);
		}

		/*
		 * Serialize racing getfsstats on the root fileserver.
		 */
		while (mounth.m_flag & M_GETFSS_LOCK) {
			mounth.m_flag |= M_GETFSS_WAIT;
			assert_wait((int)&mounth.m_flag, FALSE);
			MOUNTLIST_UNLOCK();
			thread_block();
			MOUNTLIST_LOCK();
		}
		mounth.m_getfssid++;
		/*
		 * Don't use -1 or 0.
		 */
		if (mounth.m_getfssid == (u_long)-1)
			mounth.m_getfssid = 1;
	} else {
		/*
		 * If the tag is equal to the current sync tag, this
		 * is a duplicate getfsstat request generated from
		 * different mount structure entries.  Duplicate getfsstat
		 * requests must be ignored.
		 */
		ASSERT(uap->tag >= mounth.m_getfssid);
		if (uap->tag == mounth.m_getfssid) {
			MOUNTLIST_UNLOCK();
			return (0);
		}
			mounth.m_getfssid = uap->tag;
	}

	getfssid = mounth.m_getfssid;
	mp = mounth.m_prev;
	if (!sfsp) {
		/*
		 * Simply count up the number of mount structures in the
		 * system to obtain the number of mounted filesystems.
		 *
		 */
		do {
			flag = mp->m_flag;
			if (flag & M_REMOTE_FS) {
				/*
				 * If the filesystem is remote,
				 * call remote_getfsstat_count
				 * to count up the mounted 
				 * filesystems on the remote node.
				 */
				MOUNTLIST_UNLOCK();
				error = remote_getfsstat_count(mp->m_remoteport,
					getfssid, &tmpcount);
				if (error) {
					MOUNTLIST_LOCK();
					goto out;
				}
				count += tmpcount;
				MOUNTLIST_LOCK();
			} else
				count++;
			mp = mp->m_prev;
		} while (mp != (struct mount *)&mounth);
	} else {
		do {
			if (!UNMOUNT_TRY_READ_NOSLP(mp)) {
                                /* don't wait for lock */
				mp = mp->m_prev;
				continue;
			}
			flag = mp->m_flag;
			if (flag & M_REMOTE_FS) {

				/*
				 * If the filesystem is remote
				 * call remote_getfsstat to return
				 * statfs structures for the mounted
				 * filesystems on the remote node.
				 */
				MOUNTLIST_UNLOCK();
				error = remote_getfsstat(mp->m_remoteport,
					getfssid, &sfsp, maxcount - count,
					uap->flags, &tmpcount);
				if (error) {
					UNMOUNT_READ_UNLOCK(mp);
					MOUNTLIST_LOCK();
					goto out;
				}
				count += tmpcount;
				goto next;
			}
			MOUNTLIST_UNLOCK();

			/*
			 * If MNT_NOWAIT is specified, do not refresh the
			 * fsstat cache. MNT_WAIT overrides MNT_NOWAIT.
			 */
			if ((uap->flags & MNT_WAIT) != 0) {
				VFS_STATFS(mp, error);
				if (error)
					goto next;
			}
			MOUNT_LOCK(mp);
			mp->m_stat.f_flags = mp->m_flag & M_VISFLAGMASK;
			MP_ONLY(sf = mp->m_stat);
			MOUNT_UNLOCK(mp);
			MP_ONLY(sp = &sf);
			UNI_ONLY(sp = &mp->m_stat);
			if (error = copyout((caddr_t)sp, sfsp, sizeof(*sp))) {
				UNMOUNT_READ_UNLOCK(mp);
				MOUNTLIST_LOCK();
				goto out;
			}
			sfsp += sizeof(*sp);
			count++;
next:			MOUNTLIST_LOCK();
			UNMOUNT_READ_UNLOCK(mp);
			mp = mp->m_prev;
		} while (mp != (struct mount *)&mounth && count < maxcount);
	}
	*retval = count;
out:

	if (this_node == root_fs_node) {
		int wakeup = (mounth.m_flag & M_GETFSS_WAIT);

		mounth.m_flag &= ~(M_GETFSS_WAIT|M_GETFSS_LOCK);
		MOUNTLIST_UNLOCK();
		if (wakeup)
			thread_wakeup((int)&mounth.m_flag);
	} else
                MOUNTLIST_UNLOCK();

	return (error);
}

#else	/* OSF1_ADFS */

/*
 * get statistics on all filesystems
 *
 * the Unmount read lock protects filesystem from being unmounted
 * while VFS_STATFS does its work.
 */
getfsstat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		struct statfs *buf;
		long bufsize;
		int flags;
	} *uap = (struct args *) args;
	register struct mount *mp;
	register struct statfs *sp;
	struct statfs sf;
	caddr_t sfsp;
	long count, maxcount, error;

	maxcount = uap->bufsize / sizeof(struct statfs);
	sfsp = (caddr_t)uap->buf;
	if (sfsp && maxcount == 0) {
		*retval = 0;
		return (0);
	}
	count = 0;
	MOUNTLIST_LOCK();
	mp = rootfs;
	if (!sfsp) {
		do {
			count++;
			mp = mp->m_prev;
		} while (mp != rootfs);
	} else {
		do {
			if (!UNMOUNT_TRY_READ_NOSLP(mp)) {
				mp = mp->m_prev;
				continue;
			}
			MOUNTLIST_UNLOCK();
			/*
			 * If MNT_NOWAIT is specified, do not refresh the
			 * fsstat cache. MNT_WAIT overrides MNT_NOWAIT.
			 */
			if ((uap->flags & MNT_WAIT) != 0) {
				VFS_STATFS(mp, error);
				if (error)
					goto next;
			}
			MOUNT_LOCK(mp);
			mp->m_stat.f_flags = mp->m_flag & M_VISFLAGMASK;
			MP_ONLY(sf = mp->m_stat);
			MOUNT_UNLOCK(mp);
			MP_ONLY(sp = &sf);
			UNI_ONLY(sp = &mp->m_stat);
			if (error = copyout((caddr_t)sp, sfsp, sizeof(*sp))) {
				UNMOUNT_READ_UNLOCK(mp);
				return (error);
			}
			sfsp += sizeof(*sp);
			count++;
next:			MOUNTLIST_LOCK();
			UNMOUNT_READ_UNLOCK(mp);
			mp = mp->m_prev;
		} while (mp != rootfs && count < maxcount);

	}
	MOUNTLIST_UNLOCK();
	*retval = count;
	return (0);
}
#endif	/* OSF1_ADFS */


/*
 * Change current working directory to a given file descriptor.
 */
fchdir(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct vnode *vp;
	register int error = 0;
	enum vtype type;
#ifndef	OSF1_ADFS
	struct vnode *ocdir;
#endif

	if (error = getvnode(fp))
		return (error);
	vp = (struct vnode *)fp->f_data;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VDIR)
		error = ENOTDIR;
	else
		VOP_ACCESS(vp, VEXEC, u.u_cred, error);
	if (error)
		goto out;
	VREF(vp);
#ifndef	OSF1_ADFS
	U_HANDY_LOCK();
	ocdir = u.u_cdir;
	u.u_cdir = vp;
	U_HANDY_UNLOCK();
	vrele(ocdir);
#endif
out:
	FP_UNREF(fp);
	return (error);
}

/*
 * Change current working directory (``.'').
 */
chdir(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	int error;
#ifndef	OSF1_ADFS
	struct vnode *ocdir;
#endif

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = chdirec(ndp))
		return (error);
#ifndef	OSF1_ADFS
	U_HANDY_LOCK();
	ocdir = u.u_cdir;
	u.u_cdir = ndp->ni_vp;
	U_HANDY_UNLOCK();
	vrele(ocdir);
#endif
	return (0);
}

/*
 * Change notion of root (``/'') directory.
 */
chroot(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	int error;
	struct vnode *ordir;

#if SEC_BASE
	if (!privileged(SEC_CHROOT, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = chdirec(ndp))
		return (error);
	ordir = NULL;
#ifndef	OSF1_ADFS
	U_FD_LOCK(ndp->ni_fd);
	if (u.u_rdir != NULL)
		ordir = u.u_rdir;
	u.u_rdir = ndp->ni_vp;
	U_FD_UNLOCK(ndp->ni_fd);
	if (ordir != NULL)
		vrele(ordir);
#endif
	return (0);
}

/*
 * Common routine for chroot and chdir.
 */
chdirec(ndp)
	register struct nameidata *ndp;
{
	struct vnode *vp;
	int error;
	enum vtype type;

	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VDIR)
		error = ENOTDIR;
	else
		VOP_ACCESS(vp, VEXEC, ndp->ni_cred, error);
	if (error)
		vrele(vp);
	return (error);
}

#if	FULLSERVER
/* The following functions must exist in order to link the server, but are
	only executed on file server nodes */

#ifdef	OSF1_ADFS

/*
 * Sync system call.
 * Sync each mounted filesystem.
 * Note: This code really needs to be optimized - NNN
 */
/* ARGSUSED */
sync(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int tag;
	} *uap = (struct args *) args;

	register struct mount *mp;
	int error = 0;
	extern	node_t	root_fs_node, this_node;


	MOUNTLIST_LOCK();

	if (this_node == root_fs_node) 
	{
                /* 
                 * If this requested is coming from another fileserver
                 * just ignore it. All sync calls originate at the 
                 * root_fs_node. They may go remote and come back to the 
                 * root file server in which case we just ignore them -
                 * we took care of the sync the first time around.
                 */
                if (uap->tag != -1) {
                        MOUNTLIST_UNLOCK();
                        return(0);
                }
		 mounth.m_syncid++;
		 /*
		  * Don't use -1 or 0.
		  */
		 if (mounth.m_syncid == (u_long)-1) 
			mounth.m_syncid = 1;
	} else {
		/*
		 * If the tag is less than the current sync tag, this
		 * sync raced with another sync and lost, so just return.
		 * We need to check for wrap-around here.
		 */
		 ASSERT(uap->tag >= 1);
		 if (uap->tag <= mounth.m_syncid) {
			MOUNTLIST_UNLOCK();
			return(0);
		 } else
			mounth.m_syncid = uap->tag;
	}

	mp = mounth.m_next;
        do {
                if (!(mp->m_flag & M_RDONLY)) {
                        if (vfs_busy(mp,0)) {
                                /* just skip this one */
                                goto next;
                        } 
                        /*
                         * vfs_busy() will set the mount point as busy, 
                         * preventing unmounts while we work.  If an 
                         * unmount, or another sync() is in progress, 
                         * vfs_busy() will return, and we'll skip that 
                         * filesystem.
                         */

                        if (mp->m_flag & M_REMOTE_FS) {
                                u_long syncid = mounth.m_syncid;
                                
                                /*
                                 * This filesystem is remote.  Send a sync
                                 * message to the remote filesystem root port 
                                 * found in the m_remoteport field.
                                 */
                                MOUNTLIST_UNLOCK();
                                vfs_unbusy(mp);
                                error = remote_sync(mp->m_remoteport, syncid);
                                if (error) 
                                  printf("sync:error returned from remote sync (%x)\n",
                                         error);
                                MOUNTLIST_LOCK();
                        } else  {
                                MOUNTLIST_UNLOCK();
                                VFS_SYNC(mp, MNT_NOWAIT, error);
                                /*
                                 * Unbusy the filesystem, but first
                                 * take the MOUNTLIST_LOCK to 
                                 * guarantee the list.
                                 */
                                MOUNTLIST_LOCK();
                                vfs_unbusy(mp);
                        }
                }
		/*
		 * Mount list may have changed but:
		 *	1.  This filesystem is still mounted;
		 *	2.  Currently hold mount list lock;
		 * so m_next is still valid.
		 */
next:		mp = mp->m_next;
	} while (mp != (struct mount *)&mounth);
        MOUNTLIST_UNLOCK();
        return(error);
}

#else	/* OSF1_ADFS */

/*
 * Sync system call.
 * Sync each mounted filesystem.
 */
/* ARGSUSED */
sync(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct mount *mp;
	int error = 0;

	MOUNTLIST_LOCK();
	if (mp = rootfs) do {
		/*
		 * vfs_busy() will set the mount point as busy, preventing
		 * unmounts while we work.  If an unmount, or another
		 * sync() is in progress, vfs_busy() will return, and we'll
		 * skip that filesystem.
		 */
		if (!(mp->m_flag & M_RDONLY) && !vfs_busy(mp, 0)) {
			MOUNTLIST_UNLOCK();
			VFS_SYNC(mp, MNT_NOWAIT, error);
			/*
		 	 * Unbusy the filesystem, but first take 
			 * the MOUNTLIST_LOCK to guarantee the list.
		 	 */
			MOUNTLIST_LOCK();
			vfs_unbusy(mp);
		}
		/*
		 * Mount list may have changed but:
		 *	1.  This filesystem is still mounted;
		 *	2.  Currently hold mount list lock;
		 * so m_next is still valid.
		 */
		mp = mp->m_next;
	} while (mp != rootfs);
	MOUNTLIST_UNLOCK();
	return (error);
}

#endif	/* OSF1_ADFS */

/*
 * Open system call.
 *
 * Check permissions, allocate an open file structure,
 * and call the device open routine if any.
 * The open system call always passes O_NOCTTY flag, because controlling
 * tty must now be explicitly assigned.  Binary compatibility is maintained
 * with the oopen() call, which results in not passing the O_NOCTTY flag.
 *
 * The new creat() call is a library which calls open.
 */
#ifdef	COMPAT_43
/*
 * Creat system call.
 * Now a library routine that calls open.
 */
ocreat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char		*fname;
		int		fmode;
		struct file 	**fpp;
	} *uap = (struct args *) args;

	struct args1 {
		char		*fname;
		int		mode;
		int		crtmode;
		struct file 	**fpp;
#ifdef	PFS
		int		iomode;
#endif
	} openuap;

	openuap.fname = uap->fname;
	openuap.crtmode = uap->fmode;
	openuap.mode = O_WRONLY | O_CREAT | O_TRUNC;
	openuap.fpp = uap->fpp;
#ifdef	PFS
	openuap.iomode = VIO_NONE;
#endif
	return (copen(p, &openuap, retval, 1));
}

/*
 * Open system call.  Compatibility version.
 */
oopen(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (copen(p, args, retval, 1));
}

/*
 * Open system call.  The real thing.
 */
open(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (copen(p, args, retval, 0));
}

copen(p, args, retval, compat)
	int compat;
#else
open(p, args, retval)
#endif	/* COMPAT_43 */
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char		*fname;
		int		mode;
		int		crtmode;
		struct file 	**fpp;
#ifdef	PFS
		int		iomode;
#endif
	} *uap = (struct args *)  args;
	struct nameidata *ndp = &u.u_nd;
	register struct file *fp;
	int fmode, cmode;
	int error;
	extern struct fileops vnops;
#ifdef	TNC
	struct uthread *uth = current_thread();
#endif

	if (error = falloc(uap->fpp))
		return (error);
	fp = *(uap->fpp);
#ifdef TNC
	uth->uu_opn_filep = (mach_port_t)fp;
#endif	/* TNC */
	fmode = uap->mode - FOPEN;
#ifdef	COMPAT_43
	if (!compat)
#endif
	fmode |= O_NOCTTY;	
	U_HANDY_LOCK();
	cmode = ((uap->crtmode &~ u.u_cmask) & 07777) &~ ISVTX;
	U_HANDY_UNLOCK();
#ifdef	OSF1_ADFS
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
#ifdef	PFS
	if (error = vn_open(ndp, fmode, cmode, uap->iomode)) {
#else
	if (error = vn_open(ndp, fmode, cmode)) {
#endif
		fdealloc(fp);
		if (error == ERESTART)
			error = EINTR;
		return (error);
	}
	FP_LOCK(fp);
	fp->f_flag = fmode & FMASK;
	fp->f_type = DTYPE_VNODE;
	fp->f_ops = &vnops;
	fp->f_data = (caddr_t)ndp->ni_vp;
#ifdef	TNC
	ASSERT((((struct vnode *)fp->f_data)->v_flag & VREDIRECT) == 0);
#endif
#if     SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp);
	return (0);
}


#ifdef	OSF1_ADFS
/*
 * Mknod system call
 */

mknod(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*fname;
		int	fmode;
		int	dev;
	} *uap = (struct args *) args;

#if	0
	return(mknod1(uap->fname, uap->fmode, uap->dev, 1));
#else
	/*
	 * Use 0 for the node number.  -1 (VNOVAL) is an unitialized node
	 * number.
	 */
	return(mknod1(uap->fname, uap->fmode, uap->dev, NONODE));
#endif
}


rmknod(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*fname;
		int	fmode;
		int	dev;
		int	node;
	} *uap = (struct args *) args;
	return(mknod1(uap->fname, uap->fmode, uap->dev, uap->node));
}


mknod1(fname, fmode, dev, node)
	char 	*fname;
	int	fmode;
	int	dev;
	int	node;
{

	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error, ret_val;
	short cmask;

#if SEC_BASE
	if ((fmode & IFMT) != IFIFO &&
	    !privileged(SEC_MKNOD, EPERM))
		return (EPERM);
#else
	if ((fmode & IFMT) != IFIFO &&
	    (error = suser(u.u_cred, &u.u_acflag)))
		return (error);
#endif
	ndp->ni_nameiop = CREATE | WANTPARENT | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = fname;

	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (vp != NULL) {
		error = EEXIST;
		goto out;
	}
	vattr_null(&vattr);
	switch (fmode & IFMT) {

	case IFMT:	/* used by badsect to flag bad sectors */
		vattr.va_type = VBAD;
		break;
	case IFCHR:
		vattr.va_type = VCHR;
		vattr.va_rdev = dev;
		vattr.va_node = node;
		break;
	case IFBLK:
		vattr.va_type = VBLK;
		vattr.va_rdev = dev;
		vattr.va_node = node;
		break;
	case IFIFO:
		vattr.va_type = VFIFO;
		break;
	default:
		error = EINVAL;
		goto out;
	}
	U_HANDY_LOCK();
	cmask = u.u_cmask;
	U_HANDY_UNLOCK();
	vattr.va_mode = (fmode & 07777) &~ cmask;
out:
	if (!error)
		VOP_MKNOD(ndp, &vattr, ndp->ni_cred, error);
	else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		if (vp)
			vrele(vp);
	}
	return (error);
}


#else	/* OSF1_ADFS */
/*
 * Mknod system call
 */

mknod(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*fname;
		int	fmode;
		int	dev;
		int	node;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error, ret_val;
	short cmask;

#if SEC_BASE
	if ((uap->fmode & IFMT) != IFIFO &&
	    !privileged(SEC_MKNOD, EPERM))
		return (EPERM);
#else
	if ((uap->fmode & IFMT) != IFIFO &&
	    (error = suser(u.u_cred, &u.u_acflag)))
		return (error);
#endif
	ndp->ni_nameiop = CREATE | WANTPARENT;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->fname;

	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (vp != NULL) {
		error = EEXIST;
		goto out;
	}
	vattr_null(&vattr);
	switch (uap->fmode & IFMT) {

	case IFMT:	/* used by badsect to flag bad sectors */
		vattr.va_type = VBAD;
		break;
	case IFCHR:
		vattr.va_type = VCHR;
		vattr.va_rdev = uap->dev;
		break;
	case IFBLK:
		vattr.va_type = VBLK;
		vattr.va_rdev = uap->dev;
		break;
	case IFIFO:
		vattr.va_type = VFIFO;
		break;
	default:
		error = EINVAL;
		goto out;
	}
	U_HANDY_LOCK();
	cmask = u.u_cmask;
	U_HANDY_UNLOCK();
	vattr.va_mode = (uap->fmode & 07777) &~ cmask;
out:
	if (!error)
		VOP_MKNOD(ndp, &vattr, ndp->ni_cred, error);
	else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		if (vp)
			vrele(vp);
	}
	return (error);
}
#endif	/* OSF1_ADFS */


#ifdef	OSF1_ADFS
/*
 * link system call
 */

link(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char    *target;
		char    *linkname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	int error, ret_val;
	enum vtype type;

	int remote_linkname = 0;
	mach_port_t lport = MACH_PORT_NULL;
	char pathname[MAXPATHLEN];
	char *tmppathname;
	struct vnode *dvp;
	extern char *rindex();

	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_dirp = uap->target;
	ASSERT(ndp->ni_cdir != NULL);
	ndp->ni_segflg = UIO_SYSSPACE;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	ndp->ni_vp = NULL;
	BM(VM_LOCK(vp));
	type = vp->v_type;
	BM(VM_UNLOCK(vp));
	if (type == VDIR) {
		vrele(vp);
		return(EPERM);
	}
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
		 * If the second pathname starts on the same node as the
		 * first one, call namei to perform the pathname translation.
		 */
		vrele(ndp->ni_cdir);
		ndp->ni_cdirproxy = ndp->ni_vnode2;
		ndp->ni_cdirproxy.vpx_usecount = 0;
		remote_vref(&ndp->ni_cdirproxy);

		ndp->ni_nameiop = CREATE | WANTPARENT | HASPATHBUF;
		ndp->ni_ptr = ndp->ni_pnbuf = ndp->ni_dirp = 
						(caddr_t)uap->linkname;
		ndp->ni_pathlen = strlen(ndp->ni_pnbuf) + 1;
#if SEC_BASE
		/*
		 * Note for auditing that following namei is for second pathname
		 */
		audstub_path2();
#endif
		error = namei(ndp);
		if (error == EREMOTE)
			tmppathname = ndp->ni_ptr;
	} else {
		/*
		 * The pathname begins on a remote node.
		 */
		error = EREMOTE;
		ndp->ni_forwport = ndp->ni_vnodeport2;
		tmppathname = uap->linkname;
	}
	if (error == EREMOTE) {
		char *slash;
		dev_t 	rdev;
		node_t	rnode;

		/*
		 * Send a lookup message to the remote start port.  Note:
		 * we do not pass the original CREATE|WANTPARENT options.
		 * Instead, we lookup the parent directory and later lookup
		 * the last component.
		 */
		slash = rindex(tmppathname, '/');

		if (slash) {
			bcopy(slash+1, pathname, strlen(slash));
			*slash = '\0';
		} else 
			pathname[0] = '\0';
		error = remote_lookup(ndp, tmppathname, LOOKUP | FOLLOW, &type,
				      &rdev, &rnode, &lport);
		if (!error) {
			remote_linkname++;
			ndp->ni_ptr = pathname;
		}
	} 
	if (error)
		goto out1;

	/*
	 * If the second pathname translation remained local, let linkit
	 * finish the work.
	 */
	if (!remote_linkname)
		return(linkit(vp, ndp));
	
	/*
	 * Attempt to translate the parent directory port into its vnode.
	 * If the translation fails, the second pathname is remote.
	 */
	ASSERT(lport != MACH_PORT_NULL);
	PORT_TO_VNODE_LOOKUP(lport, dvp);
	if (dvp == NULL) {
		error = EXDEV;
		goto out1;
	}
	ASSERT(dvp->v_magic == V_MAGIC && dvp->v_usecount >= 2);
		
	/*
	 * The ports representing both files are local, but the
	 * translation of the destination went remote.  Now, lookup
	 * the last component for creation, after deallocating
	 * the vnode port for the parent directory of the destination.
	 */
	if (mach_port_deallocate(mach_task_self(), lport) !=
		KERN_SUCCESS)
		panic("link: can't deallocate linkname port");
	remote_linkname = 0;
	/*
	 * Don't need HASPATHBUF.  We know this is a local lookup.
	 * But we need to deallocate the pathname buffer, if namei
	 * allocated one.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	ndp->ni_nameiop = CREATE | WANTPARENT;
	ndp->ni_dirp = ndp->ni_ptr;

	vrele(ndp->ni_cdir);
	ndp->ni_cdir = dvp;
	ndp->ni_cdirproxy.vpx_port = MACH_PORT_NULL;
	ndp->ni_cdirproxy.vpx_usecount = 0;
	remote_vref(&ndp->ni_cdirproxy);

	error = namei(ndp);

	ASSERT(ndp->ni_dvp == dvp);
	ASSERT(dvp->v_usecount >= 2);
	if (error == EREMOTE)
		panic("link: impossible remote linkname");
	else if (error)
		goto out;
	/*
	 * Both paths are local, so let linkit finish the work.
	 */
	error = linkit(vp, ndp);
	vrele(dvp);
	return(error);
out:
	if (ndp->ni_dvp) {		/* parent of linkname */
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
	}
out1:
	if (ndp->ni_vp)			/* linkname */
		vrele(ndp->ni_vp);
	if (vp)				/* target */
		vrele(vp);
	if (remote_linkname) {		/* linkname port */
		if (mach_port_deallocate(mach_task_self(), lport) !=
			KERN_SUCCESS)
			panic("link: can't deallocate linkname port");
	}
	return(error);
}


linkit(vp, ndp)
	struct vnode *vp;
	struct nameidata *ndp;
{
	struct vnode *xp;
	int ret_val, error = 0;
	
	xp = ndp->ni_vp;
	if (xp != NULL) {
		error = EEXIST;
		goto out;
	}
	xp = ndp->ni_dvp;
	if (vp->v_mount != xp->v_mount) {
		error = EXDEV;
		goto out;
	}
#ifdef	PFS
	if (VIO_IS_PFS(vp)) {
		/*
		 * Return error if this PFS file is in the process of being
		 * removed.
		 */
		PFS_LOCK(vp);
		if (vp->v_vm_info->pfs_deleting) {
			error = ENOENT;
			PFS_UNLOCK(vp);
		}
	}
#endif
out:
	ASSERT(ndp->ni_dvp->v_usecount >= 1);
	if (!error) {
		VOP_LINK(vp, ndp, error);
#ifdef	PFS
		/*
		 * The inode link count has now been updated, so it's safe
		 * to unlock to let pending unlinks/renames occur.
		 */
		if (VIO_IS_PFS(vp))
			PFS_UNLOCK(vp);
#endif
	} else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		if (ndp->ni_vp)
			vrele(ndp->ni_vp);
	}

	vrele(vp);
	return(error);
}
#else	/* OSF1_ADFS */
	
/*
 * link system call
 */
link(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*target;
		char	*linkname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp, *xp;
	int error, ret_val;
	enum vtype type;

	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_dirp = uap->target;
	ndp->ni_segflg = UIO_USERSPACE;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type == VDIR) {
		error = EPERM;
		goto out1;
	}

	ndp->ni_nameiop = CREATE | WANTPARENT;
	ndp->ni_dirp = (caddr_t)uap->linkname;
#if SEC_BASE
	/*
	 * Note for auditing that following namei is for second pathname
	 */
	audstub_path2();
#endif
	if (error = namei(ndp))
		goto out1;
	xp = ndp->ni_vp;
	if (xp != NULL) {
		error = EEXIST;
		goto out;
	}
	xp = ndp->ni_dvp;
	if (vp->v_mount != xp->v_mount)
		error = EXDEV;
out:
	if (!error)
		VOP_LINK(vp, ndp, error);
	else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		if (ndp->ni_vp)
			vrele(ndp->ni_vp);
	}
out1:
	vrele(vp);
	return (error);
}
#endif	/* OSF1_ADFS */


/*
 * symlink -- make a symbolic link
 */
symlink(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*target;
		char	*linkname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	struct vattr vattr;
	char *target;
	int error, ret_val;
	short cmask;

#ifdef  OSF1_ADFS
	ndp->ni_nameiop = CREATE | WANTPARENT | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = CREATE | WANTPARENT;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->linkname;
#ifndef	OSF1_ADFS
	ZALLOC(pathnmae_zone, target, char *);
#endif
#if SEC_BASE
	{
		u_int	len;

#ifdef	OSF1_ADFS
		target = uap->target;
#else
		if (error = copyinstr(uap->target, target, MAXPATHLEN, &len))
			goto out;
#endif
		/*
		 * Save the link contents for auditing, and note that the next
		 * namei is for the second system call argument.
		 * MP note: audstub_pathname() is pendable.
		 */
		audstub_pathname(target, len);
		audstub_path2();
	}
#else
#ifdef	OSF1_ADFS
	target = uap->target;
#else
	if (error = copyinstr(uap->target, target, MAXPATHLEN, (u_int *)0))
		goto out;
#endif	/* OSF1_ADFS */
#endif	/* SEC_BASE */
	if (error = namei(ndp))
		goto out;
	if (ndp->ni_vp) {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(ndp->ni_vp);
		error = EEXIST;
		goto out;
	}
	vattr_null(&vattr);
	BM(U_HANDY_LOCK());
	cmask = u.u_cmask;
	BM(U_HANDY_UNLOCK());
	vattr.va_mode = 0777 &~ cmask;
	vattr.va_type = VLNK;
	VOP_SYMLINK(ndp, &vattr, target, error);
out:
#ifndef	OSF1_ADFS
	PATH_DEALLOCATE(target);
#endif
	return (error);
}

/*
 * Unlink system call.
 * Hard to avoid races here, especially
 * in unlinking directories.
 */
unlink(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	int error, ret_val;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = DELETE | WANTPARENT | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = DELETE | WANTPARENT;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;

#ifdef	PFS
#define	UNLINK_SFILES	0
#define	UNLINK_HEADER	1
#define	UNLINK_ERROR	2
	/*
	 * If the file is a PFS header file, and it has only one link, don't
	 * unlink it - we must return an indication (ESTRIPED) that it is a
	 * PFS file to the emulator so the stripefiles can be unlinked first.
	 * BUT we want to go ahead and remove the header file if one of the
	 * following conditions apply:
	 *
	 *     - the stripefiles have already been unlinked
	 *     - the file's link count is > 1, (we want the stripe data to
	 *       remain for the other links)
	 *
	 * NOTE: The check on the inode link count should really be done in
	 * VOP_REMOVE() since the inode is a UFS/PFS file system dependent
	 * struct.  But since we already know this is a PFS file, we just do it
	 * here so we don't have to somehow pass the pfs_unlink_flag (in
	 * retval) to VOP_REMOVE().  This whole mess (including 
	 * pfs_multi_unlink() in the emulator) should eventually be implemented
	 * in the server anyway, so that the double unlink() calls to the PFS
	 * header file can be avoided.  This can be done when server-initiated
	 * PFS operations are implemented.
	 *
	 * SYNCHRONIZATION: Once we exit this routine with an ESTRIPED 
	 * indication, we are committed to removing the stripe data.  This 
	 * leaves a relatively large window before we return here to finally
	 * remove the PFS header file ... we need to prevent certain other
	 * operations on the header file from completing in this window.
	 * E.g. we need to prevent a link() operation on the same vnode from
	 * completing before the stripe data and PFS header file are removed.
	 * Otherwise we might end up with an orphan link: the link() operation
	 * completed successfully, but the stripe data is gone.  PFS_LOCK is
	 * used in conjunction with pfs_deleting to prevent these kinds of
	 * races.
	 */
	if (VIO_IS_PFS(vp)) {
		int		pfs_unlink_flag	= (int)retval;
		struct inode	*ip = VTOI(vp);

		ASSERT((pfs_unlink_flag == UNLINK_SFILES) ||
		       (pfs_unlink_flag == UNLINK_HEADER) ||
		       (pfs_unlink_flag == UNLINK_ERROR));

		switch (pfs_unlink_flag) {

		case UNLINK_SFILES:
			PFS_LOCK(vp);
			/*
			 * Is a PFS unlink operation already in progress?
			 * (If so, at this point the stripe data has been
			 * successfully deleted since we've obtained the lock.)
			 */
			if (vp->v_vm_info->pfs_deleting) {
				PFS_UNLOCK(vp);
				error = ENOENT;
				goto out;
			}
			/*
			 * Stripe data must be removed if this is the last 
			 * link.  Set an error indication and return, leaving
			 * the PFS lock locked so that competing unlink's
			 * on this file block until we know whether this unlink
			 * has succeeded or not.  Note that we bump the vnode
			 * reference count so that this vnode sticks around 
			 * until we return.
			 */
			IN_LOCK(ip);
			if (ip->i_nlink == 1) {
				IN_UNLOCK(ip);
				vp->v_vm_info->pfs_deleting = TRUE;
				VREF(vp);
				error = ESTRIPED;
				goto out;	/* hold the PFS lock */
			}
			IN_UNLOCK(ip);
			PFS_UNLOCK(vp);
			break;

		case UNLINK_HEADER:
			/*
			 * Stripe data has been removed, so unlock and 
			 * decrement vnode reference count.  (Note: the only
			 * case where pfs_deleting is FALSE is if a PFS open()
			 * only partially succeeded in creating stripefiles, so
			 * we're backing out of it ... i.e. we didn't get here
			 * via an unlink() system call.  No synchronization via
			 * PFS_LOCK done/needed in this case.)
			 */
			if (vp->v_vm_info->pfs_deleting) {
				PFS_UNLOCK(vp);
				vrele(vp);
			}
			break;

		case UNLINK_ERROR:
			/*
			 * Error occurred removing stripe data; clean up.
			 */
			vp->v_vm_info->pfs_deleting = FALSE;
			PFS_UNLOCK(vp);
			error = ESTRIPED;
			goto out;
		}
	}
#endif
	/*
	 * Don't unlink a mounted file.
	 */
	BM(VN_LOCK(vp));
	if (vp->v_flag & VROOT) {
		BM(VN_UNLOCK(vp));
		error = EBUSY;
		goto out;
	}
	if (vp->v_type == VDIR) {
		BM(VN_UNLOCK(vp));
		error = EPERM;
		goto out;
	}

#if	MAPPED_FILES
	/*
	 * Mappable files code uncaches in the underlying VOP_REMOVE.
	 */
	if (VIO_IS_MAPPED(vp))
		BM(VN_UNLOCK(vp));
	else
#endif
		if (HAS_PAGING_INFO(vp)) {
			BM(VN_UNLOCK(vp));
			/* XXX Calling inode_uncache(vp) without checking
			 * for i_nlink == 0 might be brutal. But, it is not
			 * pernicious!
			 */
			UNCACHE_PAGING_INFO(vp);
		} else
			BM(VN_UNLOCK(vp));

out:
	if (!error)
		VOP_REMOVE(ndp, error);
	else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(vp);
	}
	return (error);
}

#else	/* FULLSERVER */	/* Stubs, to link the server */

sync()
{
	return(EINVAL);
}

mknod()
{
	return(EINVAL);
}
ocreat()
{
	return(EINVAL);
}

oopen()
{
	return(EINVAL);
}

open()
{
	return(EINVAL);
}

rmknod()
{
	return(EINVAL);
}

symlink()
{
	return(EINVAL);
}

unlink()
{
	return(EINVAL);
}

link()
{
	return(EINVAL);
}



#endif	FULLSERVER

/*
 * Seek system call
 */
lseek(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		off_t	off;
		int	sbase;
	} *uap = (struct args *) args;
	struct vattr vattr;
	struct vnode *vp;
	enum vtype type;
	int error = 0;
	off_t newoffset;

	if (error = getf(fp))
		return (error);
	if ((fp->f_type != DTYPE_VNODE)) {
		error = ESPIPE;
		goto out1;
	}
	vp = (struct vnode *) fp->f_data;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type == VFIFO) {
		error = ESPIPE;
		goto out1;
	}
	FP_IO_LOCK(fp);
	switch (uap->sbase) {

	case L_INCR:
		BM(FP_LOCK(fp));
		newoffset = fp->f_offset + uap->off;
		BM(FP_UNLOCK(fp));
		break;

	case L_XTND:
		VOP_GETATTR(vp, &vattr, u.u_cred, error);
		if (error)
			goto out;
		newoffset = uap->off + vattr.va_size;
		break;

	case L_SET:
		newoffset = uap->off;
		break;

	default:
		error = EINVAL;
		goto out;
	}

	VOP_SEEK(vp, fp->f_offset, newoffset, u.u_cred, error);

	if (error == 0) {
		FP_LOCK(fp);
		fp->f_offset = newoffset;
		FP_UNLOCK(fp);
		*retval = newoffset;
	}
out:
	FP_IO_UNLOCK(fp);
out1:
	FP_UNREF(fp);
	return(error);
}

/*
 * Access system call
 */
saccess(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*fname;
		int	fmode;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct ucred *cred = ndp->ni_cred;
	register struct vnode *vp;
	uid_t	ruid;
	gid_t	rgid;
	int error, mode;
	int				swapped = 0;
#if	!MACH
	int svuid, svgid;
#endif

	/*
	 * Check the validity of the uap->fmode argument.
	 */
	if (uap->fmode && (uap->fmode & ~(R_OK|W_OK|X_OK)))
		return(EINVAL);

	/*
	 * Duplicate credentials so this thread can override some of
	 * the values without disturbing other threads in the same
	 * task that may be using the credentials.
	 */
	PROC_LOCK(p);
	ruid = p->p_ruid;
	rgid = p->p_rgid;
	PROC_UNLOCK(p);
	if (cred->cr_uid != ruid || cred->cr_gid != rgid) {
		++swapped;
#if	MACH
		ndp->ni_cred = crdup(cred);
		ndp->ni_cred->cr_uid = ruid;
		ndp->ni_cred->cr_gid = rgid;
#else
		svuid = cred->cr_uid;
		svgid = cred->cr_gid;
		cred->cr_uid = ruid;
		cred->cr_gid = rgid;
#endif
#if	SEC_ARCH
		SP_CHANGE_SUBJECT();
#endif
	}
#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		goto out;
	vp = ndp->ni_vp;
	/*
	 * fmode == 0 means only check for exist
	 */
	if (uap->fmode) {
		mode = 0;
		if (uap->fmode & R_OK)
			VOP_ACCESS(vp, VREAD, ndp->ni_cred, error);
		if (!error && (uap->fmode & W_OK) &&
						(error = vn_writechk(vp)) == 0)
			VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
		if (!error && (uap->fmode & X_OK))
			VOP_ACCESS(vp, VEXEC, ndp->ni_cred, error);
	}
#ifdef	PFS
	if ((!error) && (VIO_IS_PFS(vp))) {
		error = ESTRIPED;
	}
#endif
	vrele(vp);
out:
	if (swapped) {
#if	MACH
		crfree(ndp->ni_cred);
		ndp->ni_cred = cred;
#else
		u.u_uid = svuid;
		u.u_gid = svgid;
#endif
#if	SEC_ARCH
		SP_CHANGE_SUBJECT();
#endif
	}
	return (error);
}

/*
 * Stat system call.  This version follows links.
 */
stat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return(stat1(p, args, retval, FOLLOW));
}

/*
 * Lstat system call.  This version does not follow links.
 */
lstat(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{

	return (stat1(p, args, retval, NOFOLLOW));
}

stat1(p, args, retval, follow)
	struct proc *p;
	void *args;
	int *retval;
	int follow;
{
	register struct args {
		char	*fname;
		struct stat *ub;

	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
#ifndef OSF1_ADFS
	struct stat sb;
#endif
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | follow | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | follow;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
#ifdef OSF1_ADFS
        /* 
         * actually this conditional should be for OSF1_SERVER but until 
         * Grenoble picks up this changewe will leave it as OSF1_ADFS
         * For the server architecture, stat1 does not get a user buffer, so
         * there is no point in wasting a copyout, unlike the IK.
         */
	error = vn_stat(ndp->ni_vp, uap->ub);
#ifdef	PFS
	if ((!error) && (VIO_IS_PFS(ndp->ni_vp))) {
		error = ESTRIPED;
	}
#endif
	vrele(ndp->ni_vp);
	return (error);
#else 
	error = vn_stat(ndp->ni_vp, &sb);
	vrele(ndp->ni_vp);
	if (error)
		return (error);
	error = copyout((caddr_t)&sb, (caddr_t)uap->ub, sizeof (sb));
	return (error);
#endif
}

#ifdef OSF1_ADFS
/* 
 * return node # and mountid for char and block special files 
 */
int
devstat(p, args)
	struct proc *p;
	void *args;
{
	register struct args {
		char	*fname;
		struct devstat *devsb;

	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	struct vattr vattr, *vap;
        struct vnode *vp;
        int error = 0;

        uap->devsb->dst_node = 0;		/* default return values */
        uap->devsb->dst_mountid = -1;

	ndp->ni_nameiop = LOOKUP | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = uap->fname;

	if (error = namei(ndp))
		return (error);
        vp = ndp->ni_vp;

        vap = &vattr;
        VOP_GETATTR(vp, vap, u.u_cred, error);
	if (error) 
                goto done;

        if ((vap->va_type != VBLK) && (vap->va_type != VCHR)) {
                error = EINVAL;
                goto done;
        }

        uap->devsb->dst_node = vap->va_node;	/* node # of device */
        if (vap->va_type == VBLK) 
		error = remote_getmntid(vp, vap->va_node, vap->va_rdev, 
					&(uap->devsb->dst_mountid));

done : 
        vrele(vp);
        return(error);
}


#endif  /* OSF1_ADFS */
/*
 * Return target name of a symbolic link
 */
readlink(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*name;
		char	*buf;
		int	count;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct iovec aiov;
	struct uio auio;
	int error, count;
	enum vtype type;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->name;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VLNK) {
		error = EINVAL;
		goto out;
	}
#if	SEC_ARCH
	VOP_ACCESS(vp, SP_STATACC, ndp->ni_cred, error);
	if (error)
		goto out;
#endif
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_offset = 0;
	auio.uio_rw = UIO_READ;
#ifdef	OSF1_ADFS
	auio.uio_segflg = UIO_SYSSPACE;
#else
	auio.uio_segflg = UIO_USERSPACE;
#endif
	auio.uio_resid = uap->count;
	VOP_READLINK(vp, &auio, ndp->ni_cred, error);
out:
	vrele(vp);
	count = uap->count - auio.uio_resid;
	*retval = count;
#ifndef	OSF1_SERVER
	if (!error && (auio.uio_resid > 0))
		subyte((caddr_t)uap->buf + count, '\0');
#endif
	return (error);
}

/*
 * XXX -- we are not currently supporting chflags and fchflags
 *	  due to a conflict in bits.  We are using the low bit of
 *	  the on-disk inode for fast symbolic links, which is
 *	  in direct conflict with the ability *chflags gives the
 *	  superuser to change the low 16 bits of that field.
 *
 * This conflict needs to be resolved before we can support these
 * calls or their equivalent.
 */
/*
 * Change flags of a file given path name.
 */
chflags(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
		int	flags;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	vattr_null(&vattr);
	vattr.va_flags = uap->flags;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, ndp->ni_cred, error);
out:
	vrele(vp);
	return (error);
}

/*
 * Change flags of a file given a file descriptor.
 */
fchflags(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		int	flags;
	} *uap = (struct args *) args;
	struct vattr vattr;
	struct vnode *vp;
	int error;

	if (error = getvnode(fp))
		return (error);
	vattr_null(&vattr);
	vattr.va_flags = uap->flags;
	vp = (struct vnode *)fp->f_data;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, fp->f_cred, error);
out:
	FP_UNREF(fp);
	return (error);
}

/*
 * Change mode of a file given path name.
 */
chmod(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
		int	fmode;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	vattr_null(&vattr);
	vattr.va_mode = uap->fmode & 07777;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, ndp->ni_cred, error);
	if ((! error) && (vp->v_type == VREG)) {
	      /* enf locking on regular file only */
	      update_venf_lock(uap->fmode & 07777, vp);
	}
#ifdef	PFS
	if ((!error) && (VIO_IS_PFS(vp))) {
		error = ESTRIPED;
	}
#endif
out:
	vrele(vp);
	return (error);
}

/*
 * Change mode of a file given a file descriptor.
 */
fchmod(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		int	fmode;
	} *uap = (struct args *) args;
	struct vattr vattr;
	struct vnode *vp;
	int error;

	if (error = getvnode(fp))
		return (error);
	vattr_null(&vattr);
	vattr.va_mode = uap->fmode & 07777;
	vp = (struct vnode *)fp->f_data;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, fp->f_cred, error);
	if ((! error) && (vp->v_type == VREG)) {
	      /* enf locking on regular file only */
	      update_venf_lock(uap->fmode & 07777, vp);
	}
out:
	FP_UNREF(fp);
	return (error);
}

/*
 * Set ownership given a path name.
 */
chown(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
		int	uid;
		int	gid;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	vattr_null(&vattr);
	vattr.va_uid = uap->uid;
	vattr.va_gid = uap->gid;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, ndp->ni_cred, error);
#ifdef	PFS
	if ((!error) && (VIO_IS_PFS(vp))) {
		error = ESTRIPED;
	}
#endif
out:
	vrele(vp);
	return (error);
}

/*
 * Set ownership given a file descriptor.
 */
fchown(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		int	uid;
		int	gid;
	} *uap = (struct args *) args;
	struct vattr vattr;
	struct vnode *vp;
	int error;

	if (error = getvnode(fp))
		return (error);
	vattr_null(&vattr);
	vattr.va_uid = uap->uid;
	vattr.va_gid = uap->gid;
	vp = (struct vnode *)fp->f_data;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_SETATTR(vp, &vattr, fp->f_cred, error);
out:
	FP_UNREF(fp);
	return (error);
}


utimes(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		char	*fname;
		struct	timeval *tptr;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct timeval tv[2];
	struct vattr vattr;
	int error;


#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (rofs(vp->v_mount)) {
		error = EROFS;
		goto out;
	}
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error)
		goto out;
#if	SEC_BASE
	if (u.u_uid != vattr.va_uid && !privileged(SEC_OWNER, EPERM)) {
		if (uap->tptr != (struct timeval *)NULL) {
			error = EPERM;
			goto out;
		}
		VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
		if (error)
			goto out;
	}
#else
	if (u.u_uid != vattr.va_uid &&
	    (error = suser(u.u_cred, &u.u_acflag))) {
		if (uap->tptr != (struct timeval *)NULL)
			goto out;
		VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
		if (error)
			goto out;
	}
#endif
	if (uap->tptr == (struct timeval *)NULL) {
		int s;

		s = splhigh();
		TIME_READ_LOCK();
		tv[0] = tv[1] = time;
		TIME_READ_UNLOCK();
		splx(s);

	} else {
		error = copyin((caddr_t)uap->tptr, (caddr_t)tv, sizeof (tv));
	}
	if (!error) {
		vattr_null(&vattr);
		vattr.va_atime = tv[0];
		vattr.va_mtime = tv[1];
		VOP_SETATTR(vp, &vattr, ndp->ni_cred, error);
#ifdef	PFS
	        if (VIO_IS_PFS(vp)) {
		        error = ESTRIPED;
		}
#endif
	}
out:
	vrele(vp);
	return (error);
}


/*
 * Truncate a file given its path name.
 */
truncate(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
		off_t	length;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	int error;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	error = vtruncate(ndp->ni_vp, uap->length, ndp->ni_cred, 1);
	vrele(ndp->ni_vp);
	return (error);
}

/*
 * Truncate a file given a file descriptor.
 */
ftruncate(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		off_t	length;
	} *uap = (struct args *) args;
	int error, flag;

	if (error = getvnode(fp))
		return (error);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if ((flag & FWRITE) == 0) {
#if SEC_BASE
		audstub_ftrunc1();
#endif
		error = EINVAL;
	}
	else {
	        error = vtruncate((struct vnode *)fp->f_data, uap->length,
				  fp->f_cred, 0);
	}
	FP_UNREF(fp);
	return (error);
}

/*
 * Common code for truncate()/ftruncate().
 */
vtruncate(vp, length, cred, check_access)
	register struct vnode *vp;
	off_t length;
	register struct ucred *cred;
	int	check_access;
{
	struct vattr vattr;
	int error;
	enum vtype type;
	u_long vflag;
#ifdef	PFS
	u_long iomode;
#endif

	VN_LOCK(vp);
	type = vp->v_type;
	vflag = vp->v_flag;
#ifdef	PFS
	iomode = VIO_GETMODE(vp);
#endif
	VN_UNLOCK(vp);
	if (type != VREG)
	        return(EINVAL);

	if (vflag & VENF_LOCK)
	      /* enf locks on file */
	      return(EAGAIN);

	if (error = vn_writechk(vp))
		return(error);

	if (check_access) {
		VOP_ACCESS(vp, VWRITE, cred, error);
		if (error)
		        return(error);
	}

#ifdef	PFS
	/*
	 * If we got this far, and this is a PFS file, return the indication
	 * to the emulator so that the stripefiles can be truncated properly.
	 */
	if (iomode == VIO_PFS)
		return(ESTRIPED);
#endif

#ifdef	OSF1_ADFS
	if (VIO_GROW_DIRECT(vp)) {
		/*
		 * Change the length of the file.  This call will extend
		 * the file if necessary.  
		 */
		vattr_null(&vattr);
		vattr.va_size = length;		
		VOP_SETATTR(vp, &vattr, cred, error);
		return(error);
	} else
#endif
	{
		/*
		 * Get the length to figure out whether this truncate
		 * is actually extending the file.
		 */
		VOP_GETATTR(vp, &vattr, cred, error);
		if (error)
			return(error);

		if (length <= vattr.va_size) {
			/*
			 * Normal truncate.  Still have to do the setattr
			 * even if the file size isn't changing.
			 */
			vattr_null(&vattr);
			vattr.va_size = length;		
			VOP_SETATTR(vp, &vattr, cred, error);
			return(error);
		} else {
			/*
			 * Extending.  VOP_SETATTR won't extend on
			 * most filesystems.  Write a zero byte into
			 * the new end of file to force extension.
			 */
			int zero = 0;
			struct uio uio;
			struct iovec iov;
			iov.iov_base = (caddr_t) &zero;
			iov.iov_len = 1;
			uio.uio_iov = &iov;
			uio.uio_iovcnt = 1;
			uio.uio_offset = length - 1;
			uio.uio_segflg = UIO_SYSSPACE;
			uio.uio_rw = UIO_WRITE;
			uio.uio_resid = 1;
			VOP_WRITE(vp, &uio, 0, cred, error);
		}
		return(error);
	}
}

/*
 * Synch an open file.
 */
fsync(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct vnode *vp;
	int error, flag;

	if (error = getvnode(fp))
		return (error);
	vp = (struct vnode *)fp->f_data;

#if 	MAPPED_FILES 
	/*
	 * Mappable files have their data cleaned at this layer
	 * (instead of within VOP_FSYNC).  Wait for the data to be 
	 * written to disk.
	 */
	if (VIO_IS_MAPPED(vp)) 
		vflushbuf(vp, B_SYNC);
#endif
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	VOP_FSYNC(vp, flag, fp->f_cred, MNT_WAIT, error);

	FP_UNREF(fp);
	return (error);
}


#ifdef	OSF1_ADFS
/*
 * Rename system call.
 *
 * Source and destination must either both be directories, or both
 * not be directories.  If target is a directory, it must be empty.
 */
rename(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char    *from;
		char    *to;
	} *uap = (struct args *) args;
	register struct vnode *tvp, *fvp, *tdvp;
	register struct nameidata *ndp = &u.u_nd;
	struct nameidata tond;
	struct utask_nd tutnd;
	int error, ret_val;
	int stripslash = 0;
	enum vtype ttype;

	int remote_to = 0;
	mach_port_t toport = MACH_PORT_NULL;

	char tpathname[MAXPATHLEN];
	char *tmppathname;
	char *slash;
	extern char *rindex();

	/*
	 * See if we're doing directories before we decide to
	 * strip trailing slashes.  This requires an extra namei
	 * operation.  This could probably be optimized, but...
	 */
	ndp->ni_nameiop = LOOKUP | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = uap->from;
	ASSERT(ndp->ni_cdir != NULL);
	if (error = namei(ndp))
		return(error);

	tvp = ndp->ni_vp;
	BM(VN_LOCK(tvp));
	if (tvp->v_type == VDIR)
		stripslash = STRIPSLASH;
	BM(VN_UNLOCK(tvp));
	vrele(tvp);
	/*
	 * Do it again.  This to be fixed. - XXX
	 * First deallocate the pathname buffer, if namei allocated one.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	ndp->ni_nameiop = DELETE | WANTPARENT | stripslash | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_ptr = ndp->ni_pnbuf = ndp->ni_dirp = uap->from;
	ndp->ni_pathlen = strlen(ndp->ni_pnbuf) + 1;
	error = namei(ndp);
	if (error)
		return(error);
	fvp = ndp->ni_vp;
	/*
	 * Deallocate the pathname buffer, if namei allocated one.
	 * This is necessary because VOP_RENAME may call namei again.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}

	tond.ni_utnd = &tutnd;
	nddup(ndp, &tond);

	if (ndp->ni_vp2) {
		/*
		 * If the second pathname starts on the same node as the
		 * first one, call namei to perform the pathname translation.
		 */

		tond.ni_nameiop = RENAME | WANTPARENT | stripslash |
			HASPATHBUF;
		tond.ni_segflg = UIO_SYSSPACE;
		tond.ni_ptr = tond.ni_pnbuf = tond.ni_dirp = uap->to;
		tond.ni_pathlen = strlen(tond.ni_pnbuf) + 1;
		remote_vrele(&tond.ni_cdirproxy);
		tond.ni_cdirproxy = ndp->ni_vnode2;
		vnode_proxy_init(&tond.ni_cdirproxy);
#if SEC_BASE
		/*
		 * Note for auditing that following namei is for second pathname.
		 */
		audstub_path2();
#endif
		error = namei(&tond);

		if (error == EREMOTE)
			tmppathname = tond.ni_ptr;
	} else {
		/*
		 * The pathname begins on a remote node.
		 */
		error = EREMOTE;
		tond.ni_forwport = ndp->ni_vnodeport2;
		tmppathname = uap->to;
	}
	if (error == EREMOTE) {
		dev_t rdev;
		node_t	rnode;

		/*
		 * Send a lookup message to the remote start port.  Note:
		 * we do not pass the original RENAME|WANTPARENT|stripslash
		 * options.  Instead, we lookup the parent directory and
		 * later lookup the last component for renaming.
		 */
		slash = rindex(tmppathname, '/');
		if (slash) {
			bcopy(slash+1, tpathname, strlen(slash));
			*slash = '\0';
		} else {
			tpathname[0] = *tmppathname ='\0';
		}
		error = remote_lookup(&tond, tmppathname, LOOKUP | FOLLOW, 
				      &ttype, &rdev, &rnode, &toport);
		if (!error) {
			remote_to++;
			tond.ni_ptr = tpathname;
		}
	}
	if (error) {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(fvp);
		goto out3;
	}
	/*
	 * Deallocate the pathname buffer, if namei allocated one.
	 * This has to be done before renameit(), which calls namei again.
	 */
	if (tond.ni_allocbuf) {
		ASSERT(tond.ni_allocbuf == 1);
		PN_DEALLOCATE(tond.ni_pnbuf);
		tond.ni_allocbuf = 0;
	}
	/*
	 * If neither pathname translation went remote, let renameit finish
	 * the work.
	 */
	 if (!remote_to) {
#ifdef	PFS
		 error = renameit(ndp, &tond, retval);
#else
		 error = renameit(ndp, &tond);
#endif
		 goto out3;
	}
	/*
	 * Both the "from" and "to" pathnames should be local, but the 
	 * translation of the "to" file may have gone remote.  Attempt
	 * to translate the parent directory port for the "to" pathname
	 * into a vnode.  If the translation fails, the directory is remote.
	 */
	ASSERT(toport != MACH_PORT_NULL);
	PORT_TO_VNODE_LOOKUP(toport, tdvp);
	if (tdvp == NULL) {
		error =  EXDEV;
		goto out;
	}
	ASSERT(tdvp->v_magic == V_MAGIC);
	/*
	 * The ports representing both files are local, but the
	 * translation of the destination went remote.   Now lookup
	 * the last component for renaming, after deallocating the
	 * vnode port.
	 */
	if (mach_port_deallocate(mach_task_self(), toport) != KERN_SUCCESS)
		panic("rename: can't deallocate to port");
	remote_to = 0;
	toport = MACH_PORT_NULL;
	/*
	 * Don't need HASPATHBUF.  We know this is a local lookup.
	 */
	tond.ni_nameiop = RENAME | WANTPARENT | NOCACHE | stripslash;
	tond.ni_dirp = tond.ni_ptr;

	remote_vrele(&tond.ni_cdirproxy);
	tond.ni_cdir = tdvp;
	tond.ni_cdirproxy.vpx_port = MACH_PORT_NULL;
	tond.ni_cdirproxy.vpx_usecount = 0;
	remote_vref(&tond.ni_cdirproxy);

	error = namei(&tond);
	ASSERT(tond.ni_dvp == tdvp);
	if (error == EREMOTE)
		panic("rename: impossible remote dest");
	else if (error)
		goto out;

	vrele (tdvp);
#ifdef	PFS
	error = renameit(ndp, &tond, retval);
#else
	error = renameit(ndp, &tond);
#endif
	goto out3;
out:
	VOP_ABORTOP(ndp, ret_val);
	vrele(ndp->ni_dvp);
	vrele(ndp->ni_vp);
out1:
	if (!remote_to) {
		VOP_ABORTOP(&tond, ret_val);
		vrele(tond.ni_dvp);
		if (tond.ni_vp)
			vrele(tond.ni_vp);
	}
out2:
	if (toport != MACH_PORT_NULL) {
		if (mach_port_deallocate(mach_task_self(), toport) !=
			KERN_SUCCESS)
			panic("rename: can't deallocate to port");
	}
out3:
	/*
	 * If namei allocated a pathname buffer for the destination,
	 * deallocate it.
	 */
	if (tond.ni_allocbuf) {
		ASSERT(tond.ni_allocbuf == 1);
		PN_DEALLOCATE(tond.ni_pnbuf);
		tond.ni_allocbuf = 0;
	}
	ndrele(&tond);
	if (error == -1)
		return(0);
	return(error);
}


#ifdef	PFS
renameit(fndp, tndp, remove_pfs)
	struct nameidata *fndp, *tndp;
	int remove_pfs;
#else
renameit(fndp, tndp)
	struct nameidata *fndp, *tndp;
#endif
{
	struct vnode *tdvp = tndp->ni_dvp;
	struct vnode *tvp = tndp->ni_vp;
	struct vnode *fvp = fndp->ni_vp;
	enum vtype ftype, ttype;
	int error, ret_val;
#ifdef	PFS
	boolean_t pfs_lock_held = FALSE;
#endif

	error = 0;
	if (tvp != NULL) {
		BM(VN_LOCK(fvp));
		ftype = fvp->v_type;
		BM(VN_UNLOCK(fvp));
		BM(VN_LOCK(tvp));
		ttype = tvp->v_type;
		BM(VN_UNLOCK(tvp));
		if (ftype == VDIR && ttype != VDIR) {
			error = ENOTDIR;
			goto out;
		} else if (ftype != VDIR && ttype == VDIR) {
			error = EISDIR;
			goto out;
		}
		if (fvp->v_mount != tvp->v_mount) {
			if (fvp->v_mount == tdvp->v_mount) /* XXX VSX tests */
				error = EBUSY;
			else
				error = EXDEV;
			goto out;
		}
#ifdef	PFS
		/*
		 * If the target is an already-exising PFS file, the target is
		 * not equivalent to the source, and the target PFS
		 * stripefiles have not yet been removed, then don't perform
		 * the rename yet.  Return an indication that the target is a
		 * PFS file to the client emulator so the stripefiles can be
		 * unlinked first.
		 */
		if (VIO_IS_PFS(tvp) && (fvp != tvp) && (!remove_pfs)) {
			error = ESTRIPED;
			goto out;
		}
#endif
	}

#ifdef	PFS
	/*
	 * Return error if the source is a PFS file and is in the process of
	 * being removed.
	 */
	if (VIO_IS_PFS(fvp)) {
		PFS_LOCK(fvp);
		if (fvp->v_vm_info->pfs_deleting) {
			error = ENOENT;
			PFS_UNLOCK(fvp);
			goto out;
		}
		pfs_lock_held = TRUE;
	}
#endif

	if (fvp->v_mount != tdvp->v_mount) {
		error = EXDEV;
		goto out;
	}
	if (fvp == tdvp)
		error = EINVAL;
	/*
	 * If source is the same as the destination,
	 * then there is nothing to do.
	 */
	if (fvp == tvp)
	error = -1;
out:
	if (!error) {
		VOP_RENAME(fndp, tndp, error);
#ifdef	PFS
		/*
		 * The rename has completed, so it's safe to unlock to let
		 * pending unlinks occur.
		 */
		if (VIO_IS_PFS(fvp))
			PFS_UNLOCK(fvp);
#endif
	} else {
		VOP_ABORTOP(tndp, ret_val);
		vrele(tdvp);
		if (tvp)
			vrele(tvp);
		ASSERT(fvp && fndp->ni_dvp);
		VOP_ABORTOP(fndp, ret_val);
		vrele(fndp->ni_dvp);
#ifdef	PFS
		if ((VIO_IS_PFS(fvp)) && (pfs_lock_held))
			PFS_UNLOCK(fvp);
#endif
		vrele(fvp);
	}
	return(error);
}

#else	/* OSF1_ADFS */
/*
 * Rename system call.
 *
 * Source and destination must either both be directories, or both
 * not be directories.  If target is a directory, it must be empty.
 */
rename(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*from;
		char	*to;
	} *uap = (struct args *) args;
	register struct vnode *tvp, *fvp, *tdvp;
	register struct nameidata *ndp = &u.u_nd;
	struct nameidata tond;
	struct utask_nd tutnd;
	int error, ret_val;
	int stripslash = 0;
	enum vtype ftype, ttype;

	/*
	 * See if we're doing directories before we decide to 
	 * strip trailing slashes.  This requires an extra namei
	 * operation.  This could probably be optimized, but...
	 */
	ndp->ni_nameiop = LOOKUP;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->from;
	if (error = namei(ndp))
		return (error);
	tvp = ndp->ni_vp;
	BM(VN_LOCK(tvp));
	if (tvp->v_type == VDIR)
		stripslash = STRIPSLASH;
	BM(VN_UNLOCK(tvp));
	vrele(tvp);
	/*
	 * Do it again...
	 */
	ndp->ni_nameiop = DELETE | WANTPARENT | stripslash;
	ndp->ni_segflg = UIO_USERSPACE;
	ndp->ni_dirp = uap->from;
	if (error = namei(ndp))
		return (error);
	fvp = ndp->ni_vp;
	tond.ni_utnd = &tutnd;
	nddup(ndp, &tond);
	tond.ni_nameiop = RENAME | WANTPARENT | NOCACHE | stripslash;
	tond.ni_segflg = UIO_USERSPACE;
	tond.ni_dirp = uap->to;
#if SEC_BASE
	/*
	 * Note for auditing that following namei is for second pathname.
	 */
	audstub_path2();
#endif
	error = namei(&tond);
	if (error) {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(fvp);
		goto out1;
	}
	tdvp = tond.ni_dvp;
	tvp = tond.ni_vp;
	if (tvp != NULL) {
		BM(VN_LOCK(fvp));
		ftype = fvp->v_type;
		BM(VN_UNLOCK(fvp));	
		BM(VN_LOCK(tvp));
		ttype = tvp->v_type;
		BM(VN_UNLOCK(tvp));	
		if (ftype == VDIR && ttype != VDIR) {
			error = ENOTDIR;
			goto out;
		} else if (ftype != VDIR && ttype == VDIR) {
			error = EISDIR;
			goto out;
		}
		if (fvp->v_mount != tvp->v_mount) {
			if (fvp->v_mount == tdvp->v_mount) /* XXX VSX tests */
				error = EBUSY;
			else
				error = EXDEV;
			goto out;
		}
	}
	if (fvp->v_mount != tdvp->v_mount) {
		error = EXDEV;
		goto out;
	}
	if (fvp == tdvp)
		error = EINVAL;
	/*
	 * If source is the same as the destination,
	 * then there is nothing to do.
	 */
	if (fvp == tvp)
		error = -1;
out:
	if (!error)
		VOP_RENAME(ndp, &tond, error);
	else {
		VOP_ABORTOP(&tond, ret_val);
		vrele(tdvp);
		if (tvp)
			vrele(tvp);
		ASSERT(fvp && ndp->ni_dvp);
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(fvp);
	}
out1:
	ndrele(&tond);
	if (error == -1)
		return (0);
	return (error);
}
#endif	/* OSF1_ADFS */

/*
 * Mkdir system call
 */
mkdir(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*name;
		int	dmode;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error, ret_val;
	short cmask;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = CREATE | WANTPARENT | STRIPSLASH | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = CREATE | WANTPARENT | STRIPSLASH;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->name;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	if (vp != NULL) {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(vp);
		return (EEXIST);
	}
	vattr_null(&vattr);
	vattr.va_type = VDIR;
	BM(U_HANDY_LOCK());
	cmask = u.u_cmask;	
	BM(U_HANDY_UNLOCK());
	vattr.va_mode = (uap->dmode & 0777) &~ cmask;
	VOP_MKDIR(ndp, &vattr, error);
	if (!error)
		vrele(ndp->ni_vp);
	return (error);
}

/*
 * Rmdir system call.
 */
rmdir(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*name;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	int error, flag, ret_val;
	enum vtype type;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = DELETE | WANTPARENT | STRIPSLASH | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = DELETE | WANTPARENT | STRIPSLASH;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->name;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	flag = vp->v_flag;
	BM(VN_UNLOCK(vp));
	if (type != VDIR) {
		error = ENOTDIR;
		goto out;
	}
	/*
	 * Don't unlink a mounted file.
	 */
	if (flag & VROOT)
		error = EBUSY;
out:
	if (!error)
		VOP_RMDIR(ndp, error);
	else {
		VOP_ABORTOP(ndp, ret_val);
		vrele(ndp->ni_dvp);
		vrele(vp);
	}
	return (error);
}

/*
 * Read a block of directory entries in a file system independent format
 */
getdirentries(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		char	*buf;
		unsigned count;
		long	*basep;
	} *uap = (struct args *) args;
	register struct vnode *vp;
	struct uio auio;
	struct iovec aiov;
	off_t off;
	int error, eofflag, flag;
	enum vtype type;

	if (error = getvnode(fp))
		return (error);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));	
	if ((flag & FREAD) == 0) {
		error = EBADF;
		goto out;
	}
	vp = (struct vnode *)fp->f_data;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VDIR) {
		error = EINVAL;
		goto out;
	}
	aiov.iov_base = uap->buf;
	aiov.iov_len = uap->count;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_rw = UIO_READ;
	auio.uio_segflg = UIO_USERSPACE;
	auio.uio_resid = uap->count;
	FP_IO_LOCK(fp);
	auio.uio_offset = off = fp->f_offset;
	VOP_READDIR(vp, &auio, fp->f_cred, &eofflag, error);
	fp->f_offset = auio.uio_offset;
	FP_IO_UNLOCK(fp);
	if (error)
		goto out;
	error = copyout((caddr_t)&off, (caddr_t)uap->basep, sizeof(long));
	*retval = uap->count - auio.uio_resid;
#if SEC_BASE
	if (*retval)
		audstub_rdwr1(UIO_READ, uap->fd);
#endif
out:
	FP_UNREF(fp);
	return (error);
}

/*
 * mode mask for creation of files
 */
umask(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int	mask;
	} *uap = (struct args *) args;

	U_HANDY_LOCK();
	*retval = u.u_cmask;
	u.u_cmask = uap->mask & 07777;
	credentials_set_state(p->p_cred, (pid_t *) NULL, (pid_t *) NULL,
			      (struct ucred *) NULL, &p->p_utask.uu_cmask,
			      (unsigned int *) NULL,
			      (unsigned int *) NULL,
			      (uid_t *) NULL,
			      (uid_t *) NULL,
			      (task_t *) NULL);
	U_HANDY_UNLOCK();
	return (0);
}

/*
 * Void all references to file by ripping underlying filesystem
 * away from vnode.
 */
revoke(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*fname;
	} *uap = (struct args *) args;
	register struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp;
	struct vattr vattr;
	int error;
	enum vtype type;

#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->fname;
	if (error = namei(ndp))
		return (error);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VCHR && type != VBLK) {
		error = EINVAL;
		goto out;
	}
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error)
		goto out;
#if	SEC_BASE
	if (u.u_uid != vattr.va_uid && !privileged(SEC_OWNER, EPERM)) {
		error = EPERM;
		goto out;
	}
#else
	if (u.u_uid != vattr.va_uid &&
	    (error = suser(u.u_cred, &u.u_acflag)))
		goto out;
#endif
	/*
	 * N.B. The clearalias call used to be conditional upon an active
	 *	(i.e. > 1) v_usecount.  This will not work if the argument
	 *	to revoke() is an alias for an open device.
	 *	Clearalias can handle unopened devices.
	 */
#ifdef	OSF1_ADFS
	clearalias(vp);
#else
	clearalias(vp, 0);
#endif
out:
	vrele(vp);
	return (error);
}

/*
 * Returns an error to the caller or a referenced
 * file structure which the caller must dispose of
 * when done with the file structure and/or vnode.
 */
getvnode(fp)
	struct file * fp;
{
	int error;

	if (error = getf(fp))
		return (error);
	if (fp->f_type != DTYPE_VNODE) {
		FP_UNREF(fp);
		return (EINVAL);
	}
	return (0);
}


/*
 * Update the VENF_LOCK flag in the vnode to indicate if the file has
 * enforcement mode file locking on based on the mode bits of the file.
 * If it's locked and the mode bits indicate enforcement mode, set
 * the flag; otherwise, clear it.
 */
static void
update_venf_lock(mode,vp)
int mode;
struct vnode *vp;
{
	VN_LOCK(vp);
	if ((mode & S_ISGID) && (!(mode & S_IXGRP))) {
		if (vp->v_flag & VLOCKS)
			vp->v_flag |= VENF_LOCK;
	} else
		vp->v_flag &= ~VENF_LOCK;
	VN_UNLOCK(vp);
}


#ifdef OSF1_ADFS
char *
rindex(sp, c)
register char *sp, c;
{
	register char *r;

	r = 0;
	do {
		if (*sp == c)
			r = sp;
	} while (*sp++);
	return(r);
}

#endif	/* OSF1_ADFS */

#ifdef	TNC
#ifdef	CHKPNT
/*
 * Re-open a file through a file handle, and re-set all
 * the fields for that file as though we are doing a 
 * regular open().
 */
openfh(fhp, mode, fpp, return_port)
	struct fhandle	*fhp;
	int		mode;
	struct file 	**fpp;
	mach_port_t	*return_port;
{
	register struct file *fp;
	int fmode, cmode;
	int error;
	extern struct fileops vnops;
	struct nameidata *ndp = &u.u_nd;
	struct uthread *uth = current_thread();

	if (error = falloc(fpp))
		return (error);
	fp = *fpp;
	uth->uu_opn_filep = (mach_port_t)fp;
	fmode = mode - FOPEN;

	fmode |= O_NOCTTY;	
#ifdef  OSF1_ADFS
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_segflg = UIO_USERSPACE;
#endif

	/* 
	 * We do not care about cmode.  Assume the file was 
	 * created successfully. Call vn_openfh with just  
	 * fmode.				          
	 */
	if (error = vn_openfh(ndp, fhp, fmode)) {
		fdealloc(fp);
		if (error == ERESTART)
			error = EINTR;
		return (error);
	}
	FP_LOCK(fp);
	fp->f_flag = fmode & FMASK;
	fp->f_type = DTYPE_VNODE;
	fp->f_ops = &vnops;
	/* Do we get vnode from the vn_open call? */
	fp->f_data = (caddr_t)ndp->ni_vp;

	ASSERT(!(((struct vnode *)fp->f_data)->v_flag & VREDIRECT));

#if     SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp);
	return (0);
}
#endif	CHKPNT
#endif	/* TNC */

/*
 * mount_lookupname():
 * Search the mount table for a name match with the user argument
 * passed in.  If one is found, reference the root vnode and return
 * it.
 */

int invisible_mounts = 0;

static struct vnode *
mount_lookupname(pathp)
        char *pathp;
{
	caddr_t pathbuf;
	struct vnode *vp;
	struct mount *mp;
	int len, error;
        int found = 0;

	ZALLOC(pathname_zone, pathbuf, caddr_t);
	error = copyinstr(pathp, pathbuf, MAXPATHLEN, &len);
	if (error) {
		ZFREE(pathname_zone, pathbuf);
		return (NULLVP);
	}
	vp = NULLVP;
restart:
	MOUNTLIST_LOCK();
#ifdef OSF1_ADFS
        mp = mounth.m_prev;
#else
	mp = rootfs;
#endif
	ASSERT(mp);
	do {
		if (!bcmp(pathbuf, mp->m_stat.f_mntonname, (unsigned) len)) {
                        found++;
			break;
                }
#ifdef OSF1_ADFS
	} while ((mp = mp->m_prev) != (struct mount *)&mounth);
#else
	} while ((mp = mp->m_prev) != rootfs);
#endif
	if (found) {
		/* got one; now try to lock the file system */
		if (!UNMOUNT_TRY_READ(mp)) {
			MOUNTLIST_UNLOCK();
			thread_block();
			goto restart;
		}
		MOUNTLIST_UNLOCK();
		VFS_ROOT(mp, &vp, error);
		if (error) {
			UNMOUNT_READ_UNLOCK(mp);
			goto out;
		}
		ASSERT(vp->v_flag & VROOT);
		/* 
		 * If there is a nested mount point, just give up
		 * It is extremely unlikely, if at all possible, that
		 * this can happen.
		 */
		if (vp->v_mountedhere) {
			vrele(vp);
			vp = NULLVP;
		} else
			invisible_mounts++;
		UNMOUNT_READ_UNLOCK(mp);
	} else
		MOUNTLIST_UNLOCK();
out:
	ZFREE(pathname_zone, pathbuf);
	return (vp);
}

#ifdef OSF1_ADFS
int
unmount_set(cvp)
struct vnode *cvp;
{
        struct mount *mp;
        int error =0;
        
        ASSERT(cvp);
        VREF(cvp);
        mp = cvp->v_mountedhere;
        ASSERT((mp != DEADMOUNT) && (mp != NULLMOUNT));
        UNMOUNT_WRITE_LOCK(mp);
        vfs_busy(mp,1);
        vrele(cvp);
        return(0);
}


int
unmount_done(cvp, error)
struct vnode *cvp;
int          error;
{
        struct mount *mp;
        
        ASSERT(cvp);
        ASSERT(cvp->v_flag & VMOUNTPOINT);
        mp = cvp->v_mountedhere;
        ASSERT((mp != DEADMOUNT) && (mp != NULLMOUNT));

	/*
	 * The UNMOUNT_WRITE_UNLOCK() is necessary to wake up anyone 
	 * waiting for the completion of the unmount(), for example, namei().
	 */
        UNMOUNT_WRITE_UNLOCK(mp);
        vfs_unbusy(mp);

        if (!error) {
                /* remove all ways to find this mountpoint */
		VN_LOCK(cvp);
		cvp->v_mountedhere = NULLMOUNT;
                cvp->v_flag &= ~VMOUNTPOINT;
		VN_UNLOCK(cvp);
		/*
		 * Call mount_port_deallocate to deallocate send rights
		 * to the remote and local mount ports, if necessary.
		 */
		mount_port_deallocate(mp);
		MOUNTLIST_LOCK();
		if (mp == rootfs)
			panic("clear_unmount: unmounting root");
		mp->m_prev->m_next = mp->m_next;
		mp->m_next->m_prev = mp->m_prev;
		MOUNTLIST_UNLOCK();

		ZFREE(mount_zone, mp);
                vrele(cvp);
	}
        return(0);
}

#endif


#ifdef	PFS
/*
 * Increase the size of a file, preallocating disk storage if necessary (if the
 * size of the file is increasing).
 */
_lsize(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		off_t	off;
		int	sbase;
	} *uap = (struct args *) args;
	struct ucred *cred;
	struct vattr vattr;
	struct vnode *vp;
	enum vtype type;
	int flag;
	u_long vflag;
	int error = 0;
	off_t cursize, newsize;
	off_t offset;

	/*
	 * Get the vnode for the file; extract current open flags and offset.
	 */
	if (error = getvnode(fp))
		return (error);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	offset = fp->f_offset;
	BM(FP_UNLOCK(fp));

	/*
	 * Ensure user has the file open for writing.
	 */
	if ((flag & FWRITE) == 0) {
		error = EINVAL;
		goto out1;
	}
	vp = (struct vnode *) fp->f_data;
	cred = fp->f_cred;

	/*
	 * Ensure file is a regular file.
	 */
	VN_LOCK(vp);
	type = vp->v_type;
	vflag = vp->v_flag;
	VN_UNLOCK(vp);
	if (type != VREG) {
		error = EINVAL;
		goto out1;
	}

	/*
	 * Ensure that there are no file locks on the file.
	 */
	if (vflag & VENF_LOCK) {
		/* enf locks on file */
		error = EAGAIN;
		goto out1;
	}

	/*
	 * Ensure that the file system is not read-only.
	 */
	if (error = vn_writechk(vp))
		goto out1;

	/*
	 * Get current file size.
	 */
#if	MAPPED_FILES
	if (VIO_IS_MAPPED(vp))
		mf_get_size_and_offset(fp, &cursize, &offset);
	else
#endif
	{
		VOP_GETATTR(vp, &vattr, cred, error);
		if (error)
			goto out;
		cursize = vattr.va_size;
	}

	/*
	 * Calculate new file size.
	 */
	FP_IO_LOCK(fp);
	switch (uap->sbase) {

	case SIZE_SET:
		newsize = MAX(cursize, uap->off);
		break;

	case SIZE_CUR:
		newsize = MAX(cursize, offset + uap->off);
		break;

	case SIZE_END:
		newsize = MAX(cursize, cursize + uap->off);
		break;

	default:
		error = EINVAL;
		goto out;
	}

	/*
	 * Call the appropriate vnode operation for this file system.
	 */
	VOP_PREALLOC(vp, cursize, &newsize, cred, error);
	if (error == 0)
		*retval = newsize;

out:
	FP_IO_UNLOCK(fp);
out1:
	FP_UNREF(fp);
	return(error);
}


pfs_fdevstat(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	register struct args {
		struct pfs_stat *psbuf;
	} *uap = (struct args *)args;
	struct pfs_stat *pst = uap->psbuf;
	struct vattr vattr;
	register struct vattr *vap;
	struct vnode *vp;
	int error = 0;
	u_short mode;
	enum vtype type;

	if (error = getf(fp))
		return(error);

	/*
	 * Make sure this is a file:
	 */
        if (fp->f_type != DTYPE_VNODE) {
                error = EINVAL;
                goto done;
        }       

	vp = (struct vnode *)fp->f_data;
	vap = &vattr;
	VOP_GETATTR(vp, vap, u.u_cred, error);
	if (error)
		goto done;

	pst->pst_node = this_node;
	pst->pst_dev = vap->va_fsid;
	pst->pst_ino = vap->va_fileid;
	pst->pst_size = vap->va_size;
	mode = vap->va_mode;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	switch (type) {
	case VREG:
		mode |= S_IFREG;
		break;
	case VDIR:
		mode |= S_IFDIR;
		break;
	case VBLK:
		mode |= S_IFBLK;
		break;
	case VCHR:
		mode |= S_IFCHR;
		break;
	case VLNK:
		mode |= S_IFLNK;
		break;
	case VSOCK:
		mode |= S_IFSOCK;
		break;
	case VFIFO:
		mode |= S_IFIFO;
		break;
	default:
		error = EBADF;
		goto done;
	};

	pst->pst_mode =  mode;

done:
	FP_UNREF(fp);
	return (error);
}


/*
 * Get default PFS file system stripe attributes (by pathname).
 *
 * Holding the referenced vnode returned by namei prevents
 * the filesystem from becoming unmounted.
 */
statpfs(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char *path;
		struct statpfs *buf;
		unsigned buflen;
	} *uap = (struct args *) args;
	register struct mount *mp;
	struct statpfs *spfs;
	register struct nameidata *ndp = &u.u_nd;
	int error;
	int copylen;

	/*
	 * Get mount structure pointed to by the vnode for the given path.
	 */
#ifdef	OSF1_ADFS
	ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
#else
	ndp->ni_nameiop = LOOKUP | FOLLOW;
	ndp->ni_segflg = UIO_USERSPACE;
#endif
	ndp->ni_dirp = uap->path;
	if (error = namei(ndp))
		return (error);
	mp = ndp->ni_vp->v_mount;

	/*
	 * If this is a PFS file system, copy the default stripe attributes
	 * from the mount table to the given buffer.  Technically, a new VFS
	 * OP should be added for statpfs (similar to statfs), but since PFS
	 * is the only file system that supports it, we just do it here.
	 */
	MOUNT_LOCK(mp);
	spfs = mp->m_statpfs;
	if (spfs != NULL) {
		copylen = MIN(uap->buflen, spfs->p_reclen);
		error = copyout((caddr_t)spfs, (caddr_t)uap->buf, copylen);
	} else {
		error = EFSNOTSUPP;
	}
	MOUNT_UNLOCK(mp);

	vrele(ndp->ni_vp);
	if (!error)
		*retval = copylen;
	return(error);
}


/*
 * Get default PFS file system stripe attributes (by file descriptor).
 *
 * Holding the referenced vnode returned by getvnode prevents
 * the filesystem from becoming unmounted in the normal case.
 */
fstatpfs(fp, args, retval)
	struct file *fp;
	void *args;
	int *retval;
{
	struct args {
		struct statpfs *buf;
		unsigned buflen;
	} *uap = (struct args *) args;
	struct mount *mp;
	struct statpfs *spfs;
	int error;
	int copylen;

	/*
	 * Get mount structure pointed to by the file descriptor's vnode.
	 */
	if (error = getvnode(fp))
		return (error);
	mp = ((struct vnode *)fp->f_data)->v_mount;

	/*
	 * If this is a PFS file system, copy the default stripe attributes
	 * from the mount table to the given buffer.  Technically, a new VFS
	 * OP should be added for statpfs and fstatpfs (similar to statfs),
	 * but since PFS is the only file system that supports it, we just 
	 * do it here.
	 */
	MOUNT_LOCK(mp);
	spfs = mp->m_statpfs;
	if (spfs != NULL) {
		copylen = MIN(uap->buflen, spfs->p_reclen);
		error = copyout((caddr_t)spfs, (caddr_t)uap->buf, copylen);
	} else {
		error = EFSNOTSUPP;
	}
	MOUNT_UNLOCK(mp);

	FP_UNREF(fp);
	if (!error)
		*retval = copylen;
	return(error);
}
#endif	PFS
