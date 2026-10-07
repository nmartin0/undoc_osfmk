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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: vfs_vnops.c,v $
 * Revision 1.13  1995/02/01  22:37:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.12  1994/11/23  22:16:18  raysx
 *  Problem description: panic: data_read: sec bounding on HiPPI node while testing
 *           IPI-3 mounted as UFS
 *  Reviewer(s): bolsen@locus.com
 * 	      jlitvin@ssd.intel.com
 *  Risk: Low
 *  Benefit or PTS #: 10650
 *  Testing:
 *  Module(s): server/ufs/ufs_vnops.c
 * 	    server/vfs/vfs_vnops.c
 *
 * Revision 1.11  1994/11/18  20:50:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.10  1994/07/11  17:16:13  johannes
 * vn_readdir(): new function for server use
 * 	      reads a directory given by a vnode
 * 	      calls VOP_READDIR
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: L
 *  Benefit or PTS #: cleanup of core directory (PARACORE)
 *  Testing: developer tests
 *  Module(s): 	server/paracore/allocinfo.c
 * 		server/paracore/core.c
 * 		server/paracore/dump.c
 * 		server/uxkern/fsvr.defs
 * 		server/uxkern/fsvr_msg.c
 * 		server/uxkern/fsvr_server_side.c
 * 		server/vfs/vfs_vnops.c
 *
 * Revision 1.9  1994/04/08  18:10:38  cfj
 * Merged revision 1.6.4.1 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.8  1994/03/14  02:09:34  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.7  1994/01/11  18:26:01  jlitvin
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
 * Revision 1.6.4.1  1994/04/08  18:08:16  cfj
 * Move the call to PPROC_CTTY_SETUP() to later on so that it
 * is only called for character special files.
 *
 *  Reviewer:dbm,chrisp@locus.com
 *  Risk:L
 *  Benefit or PTS #:8877
 *  Testing:pccm2,VSX EAT
 *  Module(s):server/vfs/vfs_vnops.c
 *
 * Revision 1.6  1993/09/02  02:44:21  brad
 * When switching from Fast Path to Mapped File mode (PFS stripefiles only),
 * initialize Mapped File block reservation fields in the inode, otherwise
 * we might hit an assert in itrunc_reserved().  Fix for bug 6401 (also see
 * ufs_inode.c).
 *
 * Revision 1.5  1993/07/14  18:46:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:10:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  20:32:26  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:13  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.20  1993/04/24  18:48:54  klh
 * 	Revision 2.19  93/03/30  16:11:45  roy
 * 		Remove fast_path_io code from vn_read/vn_write.
 * 		[93/02/18            roy]
 *
 * 	Revision 2.18  93/01/08  14:39:19  durriya
 * 		Add paging=0 as argument to VOP_PAGEOUT             (durriya)
 *
 * 	Revision 2.17  92/12/08  10:48:45  durriya
 * 		1.1 unmount sync changes. Changed MOUNT_LOOKUP locking to
 * 		UNMOUNT_TRY_READ and UNMOUNT_READ_UNLOCK for synchronization in
 * 		vn_close() and vn_fhtovp().                              (durriya)
 *
 * Revision 1.3  1993/04/03  03:13:15  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.3  1993/02/12  22:39:45  brad
 * Added support for disallowing simultaneous access to a PFS file
 * (VIO_PFS mode) and one of its stripefiles (VIO_STRIPED mode).
 *
 * Revision 1.1.2.1.2.2  1993/02/09  21:42:10  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.2  1992/11/30  22:58:17  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/26  01:43:18  brad
 * Fixed Fast Path I/O bug.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:17:37  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.1  1992/11/05  23:46:51  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:58:00  cfj
 * Bump major revision number.
 *
 * Revision 2.19  1992/10/05  14:03:39  klh
 * 	Revision 2.16  92/09/20  11:25:51  roy
 * 		Change calling sequence to mf_get_size_and_offset.
 * 		[92/09/14            roy]
 * 
 * Revision 2.18  92/09/29  08:25:05  roman
 * Fix RCS comments.
 * 
 * Revision 2.17  92/08/17  18:13:39  mjl
 * Remove debugging macros.
 * 
 * Revision 2.16  92/08/06  16:44:25  roman
 * Fix RCS comments.
 * 
 * Revision 2.15  92/08/06  13:34:13  klh
 * 	Revision 2.15  92/07/29  09:00:57  rabii
 * 		Fixed RCS log
 * 
 * 	Revision 2.14  92/07/29  08:30:33  rabii
 * 		Access to mapped file I/O moved from vn_rdwr to underlying 
 *		VOP's. Add support for FAST_PATH_IO in vn_write (pjg).
 * 		[92/07/28            roy]
 * 
 * Revision 2.14  92/06/10  10:12:15  klh
 * 	Revision 2.13  92/06/08  18:32:10  pjg
 * 		Add suport for fast_path under FAST_PATH_IO conditional 
 *		(read only). vn_read calls VOP_PAGEIN if the iomode of the 
 *		file is VIO_FASTPATH (pjg).
 * 
 * Revision 2.13  92/06/05  14:01:18  klh
 * 	Revision 2.12  92/05/18  12:31:13  roy
 * 		Revision 2.9.2.1  92/04/22  09:55:34  roy
 * 		Call mf_get_size_and_offset for mapped_files in vn_ioctl().
 * 		[92/04/06            roy]
 * 
 * Revision 2.12  92/04/06  19:09:05  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.11  92/04/05  17:12:25  pjg
 * 	In vn_stat, set st_dev to be the mountid corresponding to the file 
 * 	system that the object lives on (durriya)
 * 
 * 	Add extra parameter to VPOP_CTTY_GETATTR(). (roman)
 * 
 * Revision 2.10  92/03/20  11:38:26  pjg
 * 	92/03/17  17:38:24  noemi
 * 	Undid vn_open change to keep vnode reference for EREMOTEPORT errors.
 * 
 * Revision 2.9  92/03/16  18:29:44  pjg
 * 	92/03/16  18:21:49  noemi
 * 	Don't release vnode reference for remote special file opens.
 * 
 * Revision 2.8  92/03/15  14:42:01  roy
 * 	92/03/03  16:56:07  roy
 * 	Changes for MAPPED_FILES.
 * 
 * Revision 2.7  92/01/05  20:02:26  roy
 * 	1991/10/14  20:21:44  noemi
 * 	Use HASPATHBUF option when calling namei.
 * 
 * Revision 2.6  91/12/13  10:26:10  roy
 * 	91/12/04  16:39:39  roy
 * 	Make sure vn_read and vn_write aren't being called for mapped files.
 * 
 * 	91/12/03  10:51:47  roy
 * 	Mapped file support for vn_rdwr().
 * 
 * Revision 2.5  91/11/22  15:21:54  rabii
 * 	Added missing PPROC_CTTY_SETUP for COMPAT43. (chrisp)
 * 
 * 	Controlling terminal vnode pointer now located in tty structure.
 * 	VPOP_CTTY_GETATTR used to obtain a pointer to the corresponding
 * 	tty structure where the vnode pointer is to be set. (chrisp)
 * 
 * Revision 2.4  91/10/04  15:31:52  chrisp
 * Add Locus copyright notice.
 * 
 * Revision 2.3  91/09/16  16:41:15  sjs
 * integrated Locus changes: use of VPOP routines
 * 
 * Revision 2.2  91/08/31  14:32:15  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/01  17:04:17  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.17.9.2  91/06/21  15:02:35  gmf
 * 	If we lose the race for file creation, try again.
 * 	EEXIST is the wrong error to return from vn_open.
 * 	[91/06/21  14:27:41  gmf]
 * 
 * Revision 1.17.3.2  91/02/01  10:46:43  gmf
 * 	Modified checks for mandatory file locks.
 * 	Deleted call to update_venf_lock in vn_open;
 * 	it's not required.
 * 	[90/11/15  15:02:17  gmf]
 * 
 * 	Changed interface to locked(), so callers changed.
 * 	[90/11/06  12:44:40  gmf]
 * 
 * Revision 1.17  90/10/31  14:09:02  devrcs
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:19:16  jvs]
 * 
 * 	Remove the check for file size limit and do it at the file system 
 *	layers once we have the inode locked. This prevents two processes 
 *	from writing what they think is the last part of the file.
 * 	[90/10/11  09:47:17  sp]
 * 
 * 	Added a VFIFO case to the vn_ioctl switch.
 * 	[90/10/05  14:07:01  collins]
 * 
 * 	Check for Xenix file locks in vn_open(), vn_read() and vn_write().
 * 	[90/10/08  14:21:36  swallace]
 * 
 * Revision 1.16  90/10/07  15:01:06  devrcs
 * 	Added test to vn_write() to avoid writing past the file size limit.
 * 	[90/10/04  16:26:22  coren]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:55:56  gm]
 * 
 * Revision 1.15  90/09/23  16:01:56  devrcs
 * 	Get rid of cleanlocks() call in vn_close.  It's done in closef().
 * 	[90/09/17  19:10:46  ers]
 * 
 * 	Get rid of unnecessary conditionals around calls to
 * 	mpsleep.
 * 	[90/09/14  13:45:16  gmf]
 * 
 * 	Got rid of code in vn_flock() which prevented flock() from placing a
 * 	lock on a file.  Bug 0242.
 * 	Also did some cleanup work: a check that is done in flock() was
 * 	repeated in vn_flock().
 * 	[90/09/12  14:08:18  swallace]
 * 
 * 	Assign (potentially new) vnode to ndp->ni_vp after VOP_OPEN call in
 * 	vn_open.  This fixes bug #893/927 -- clone open/close problem.
 * 	[90/09/07  17:08:14  gmf]
 * 
 * 	vn_read, vn_write and vn_open check for enforcement mode locks.
 * 	vn_open set flag in vnode if creating file with enforcement mode
 * 	locking enabled.
 * 	vn_close calls cleanlocks if the vnode has file locks on it.  FFLCK is
 * 	deleted from the file structure.
 * 	[90/09/06  14:31:23  swallace]
 * 
 * Revision 1.14  90/08/24  12:30:54  devrcs
 * 	Removed setjmp dependencies
 * 	[90/08/20  03:36:47  gmf]
 * 
 * 	Changes for new system call interface.
 * 	Removed reference to u.u_rval1 in vn_ioctl.
 * 	[90/08/17  17:49:35  nags]
 * 
 * 	Changed COMPAT code for controlling tty assignment.
 * 	[90/08/12  12:13:22  ers]
 * 
 * Revision 1.13  90/07/27  09:10:15  devrcs
 * 	Dead mounts, use FSYNC on write, clones.
 * 	[90/07/20  17:10:25  nags]
 * 
 * Revision 1.12  90/07/17  11:44:00  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  22:05:02  seiden]
 * 
 * Revision 1.11  90/06/29  13:55:03  devrcs
 * 	Use FFLCK to conditionally call cleanlocks().
 * 	[90/06/26  11:41:57  gmf]
 * 
 * Revision 1.10  90/06/22  20:56:58  devrcs
 * 	vn_close: can't use vnode after vrele.
 * 	[90/06/18  17:15:16  gmf]
 * 
 * 	Fix typo introduced in nags merge
 * 	[90/06/18  09:59:07  seiden]
 * 
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Secureware changes.				seiden@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	COMPAT code to set controlling tty on open	ers@osf.org
 * 	Change for new select interface			coren@osf.org
 * 	File layer || changes: spin and blocking locks 	noemi@osf.org
 * 	Added vnode reader/writer counts		noemi@osf.org
 * 	Added FIFOs 					ers@osf.org
 * 	Added call to cleanlocks in vn_close		ers@osf.org
 * 	Integrated 4.4BSD changes as of 1/5/90 		noemi@osf.org
 * 	[90/06/12  21:44:20  gmf]
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
 *	@(#)vfs_vnops.c	7.13 (Berkeley) 12/21/89
 */

#if	MACH
#include <mach_nbc.h>
#endif

#ifdef	OSF1_ADFS
#include <mapped_files.h>
#endif

#include <sys/secdefines.h>
#if	SEC_BASE
#include <sys/security.h>
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/buf.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <ufs/fs.h>
#include <ufs/quota.h>
#include <sys/ioctl.h>
#include <sys/tty.h>
#if	MACH
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>	
#include <kern/assert.h>
#include <kern/parallel.h>
#endif
#include <uxkern/mf.h>

#if	MACH
#define PAGER_UNCACHE_TRY(vp)						\
MACRO_BEGIN								\
	if (!inode_uncache_try(vp))					\
		return(ETXTBSY);					\
MACRO_END
#else
#define PAGER_UNCACHE_TRY(vp)						\
MACRO_BEGIN								\
	xrele(vp);							\
	if ((vp)->v_flag & VTEXT)					\
		return (ETXTBSY);					\
MACRO_END
#endif

int	vn_read(), vn_write(), vn_ioctl(), vn_select(), vn_close();
struct 	fileops vnops =
	{ vn_read, vn_write, vn_ioctl, vn_select, vn_close };

#define MAN_LOCK  1		/* for checking System V mandatory locking */
#ifdef i386
#define XENIX_LOCK 0
#endif

/*
 * Common code for vnode open operations.
 * Check permissions, and call the VOP_OPEN or VOP_CREATE routine.
 */
#ifdef	PFS
/*
 * Additional iomode parameter adds support for setting the mode on a per-file
 * basis, rather than just per-file system.  This support is required for
 * PFS to access stripefiles in Fast Path mode.
 */
vn_open(ndp, fmode, cmode, iomode)
#else
vn_open(ndp, fmode, cmode)
#endif
	register struct nameidata *ndp;
	int fmode, cmode;
#ifdef	PFS
	int iomode;
#endif
{
	register struct vnode *vp;
	struct vnode *newvp;
	struct vattr vat;
	struct vattr *vap = &vat;
	int error;
	enum vtype	type;
	u_long v_flag;
#ifdef COMPAT_43
	int oflag;
	extern void spec_setopen();
#endif

retry:

	if (fmode & FCREAT) {
#ifdef	OSF1_ADFS
		ndp->ni_nameiop = CREATE | WANTPARENT | HASPATHBUF;
#else
		ndp->ni_nameiop = CREATE | WANTPARENT;
#endif
		if ((fmode & FEXCL) == 0)
			ndp->ni_nameiop |= FOLLOW;
		if (error = namei(ndp))
			return (error);
		if (ndp->ni_vp == NULL) {
			vattr_null(vap);
			vap->va_type = VREG;
			vap->va_mode = cmode;
			VOP_CREATE(ndp, vap, error);
			if (error) {
				/*
				 * Could be a race for creation
				 */
				if (error == EEXIST)
					goto retry;
				else
					return (error);
			}
			fmode &= ~FTRUNC;
			vp = ndp->ni_vp;
#if	SEC_BASE
			/*
			 * Set audit event type to ET_OBJECT_CREAT
			 */
			audstub_open1();
#endif
		} else {
			if (ndp->ni_dvp == ndp->ni_vp)
				vrele(ndp->ni_dvp);
			else if (ndp->ni_dvp != NULL)
				vrele(ndp->ni_dvp);
			ndp->ni_dvp = NULL;
			vp = ndp->ni_vp;
			if (fmode & FEXCL) {
				error = EEXIST;
				goto bad;
			}
			fmode &= ~FCREAT;
		}
	} else {
#ifdef	OSF1_ADFS
		ndp->ni_nameiop = LOOKUP | FOLLOW | HASPATHBUF;
#else
		ndp->ni_nameiop = LOOKUP | FOLLOW;
#endif
		if (error = namei(ndp))
			return (error);
		vp = ndp->ni_vp;
	}
	VN_LOCK(vp);
	type = vp->v_type;
	v_flag = vp->v_flag;
#ifdef	PFS
	if (type == VREG) {
		ASSERT((iomode == VIO_BUF) || (iomode == VIO_MAPPED) ||
		       (iomode == VIO_FASTPATH) || (iomode == VIO_PFS) ||
		       (iomode == VIO_STRIPED) || (iomode == VIO_NONE));
		/*
		 * Set the I/O access mode of this file, if a change is 
		 * requested.  However, disallow simultaneous access of a PFS
		 * file in different modes.  (This prevents a PFS stripefile
		 * from being opened "directly", thereby bypassing the PFS
		 * token acquire code,  when it is already opened by PFS.)
		 */
		if (iomode == VIO_NONE)  /* use default for this file system */
			iomode = vp->v_mount->m_iomode;
		if (iomode != VIO_GETMODE(vp)) {
			if ((vp->v_wrcnt == 0) && (vp->v_rdcnt == 0)) {
				/* No one else has the file open: OK to
				   change mode */
				vp->v_iomode = iomode;
				if (VIO_IS_MAPPED(vp)) {
					/* reinitialize block reservation */
					iinit_extra(VTOI(vp));
				}
			} else {  /* file is already in use in another mode */
				VN_UNLOCK(vp);
				error = EPFSBUSY;
				goto bad;
			}
		}
	}
#endif
	VN_UNLOCK(vp);
	if (type == VSOCK) {
		error = EOPNOTSUPP;
		goto bad;
	}

	if ((fmode & FCREAT) == 0) {
		if (fmode & FREAD) {
			VOP_ACCESS(vp, VREAD, ndp->ni_cred, error);
			if (error)
				goto bad;
		}
		if (fmode & (FWRITE|FTRUNC)) {
			/*
			 * This should probably be changed, since
			 * POSIX says TRUNC on directory has no effect
			 */
			if (type == VDIR) {
				error = EISDIR;
				goto bad;
			}
			if (error = vn_writechk(vp))
				goto bad;
			VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
			if (error)
				goto bad;
		}
	}
	if ((fmode & FTRUNC) && type != VFIFO) {
	        /* truncating - check enforcement mode locks */
	        if (v_flag & VENF_LOCK) {
	                error = EAGAIN;
	                goto bad;
		}
#ifdef i386
		/* check Xenix locks */
	        if (v_flag & VXENIX) {
	                error = EAGAIN;
	                goto bad;
		}
#endif
		vattr_null(vap);
		vap->va_size = 0;
		VOP_SETATTR(vp, vap, ndp->ni_cred, error);
		if (error)
			goto bad;
#if	SEC_BASE
		/*
		 * Set audit event type to ET_OBJECT_MOD
		 */
		audstub_open2();
#endif
	}
#ifdef COMPAT_43
	if (type == VCHR) {
	        PPROC_CTTY_SETUP(u.u_procp);
		BM(unix_master());
		oflag = u.u_procp->p_flag;
		BM(unix_release());
	}
#endif
	newvp = vp;
	VOP_OPEN(&newvp, fmode, ndp->ni_cred, error);
	vp = newvp;
	ndp->ni_vp = vp;	/* in case it got cloned */
#ifdef PFS
	if (error == EFSNOTSUPP) {
		int err;
#ifdef	OSF1_ADFS
		ndp->ni_nameiop = DELETE | WANTPARENT | HASPATHBUF;
#else
		ndp->ni_nameiop = DELETE | WANTPARENT;
#endif
		(void)namei(ndp);
		VOP_REMOVE(ndp, err);
		if (err)
			return (err);
		VOP_INACTIVE(vp, err);
		if (err)
			return (err);
		VN_LOCK(vp);
		if (!wait_for_vxlock(vp, 1)) {
			extern struct vnodeops dead_vnodeops;
			VN_UNLOCK(vp);
			(void)vclean(vp, VX_DOCLOSE, &dead_vnodeops);
			(void)clear_vxlock(vp);
		}
		VN_UNLOCK(vp);
		return (error);
	}
#endif
#ifdef COMPAT_43
	/*
	 * If a controlling tty was implicitly assigned,
	 * we must set the session controlling tty vnode
	 * pointer.  See code in tty.c:ttyopen().
	 */
	unix_master();
	if (!error && type == VCHR && !(fmode&O_NOCTTY) && !(oflag&SCTTY) &&
	    (u.u_procp->p_flag&SCTTY)) {
		struct tty *ttyp;
		struct vproc *vs = LOCATE_VPROC_PID(u.u_procp->p_sid);
		(void) VPOP_CTTY_GETATTR(u.u_procp->p_vproc, vs, 0, 0, 0, &ttyp);
		VPROC_RELEASE(vs, "vn_open");
		ttyp->t_vnode = vp;
		unix_release();
		/*
		 * it's as if we're doing an open
		 */
		spec_setopen(vp);
	} else
		unix_release();
#endif /* COMPAT_43 */
	if (error)
		vrele(vp);
	else {
		VN_LOCK(vp);
		/*
		 * Don't increment reader and writer counts for fifos.
		 * fifo_open takes care of that.
		 */
		if (type != VFIFO) {
			if (fmode & FREAD)
				vp->v_rdcnt++;
			if (fmode & FWRITE)
				vp->v_wrcnt++;
		}
		VN_UNLOCK(vp);
	}
	return (error);
bad:
	vrele(vp);
	return(error);
}

/*
 * Check for write permissions on the specified vnode.
 * The read-only status of the file system is checked.
 * Also, prototype text segments cannot be written.
 */
vn_writechk(vp)
	register struct vnode *vp;
{
	enum vtype	type;

	/*
	 * Disallow write attempts on read-only file systems;
	 * unless the file is a socket or a block or character
	 * device resident on the file system.
	 */

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));

	BM(MOUNT_LOCK(vp->v_mount));
	if (vp->v_mount->m_flag & M_RDONLY) {
		BM(MOUNT_UNLOCK(vp->v_mount));
		if (type != VCHR && type != VBLK &&
		    type != VSOCK && type != VFIFO)
			return (EROFS);
	} else
		BM(MOUNT_UNLOCK(vp->v_mount));

	/*
	 * If there's shared text associated with
	 * the vnode, try to free it up once.  If
	 * we fail, we can't allow writing.
	 */

	BM(VN_LOCK(vp));
	if (vp->v_flag & VTEXT) {
		BM(VN_UNLOCK(vp));
#if	MAPPED_FILES
		if (MF_MAPPABLE(vp)) {
			/*
			 * If the object can be uncached, then the VTEXT bit 
			 * will be cleared.
			 */
			mf_uncache(vp, TRUE);
			if (vp->v_flag & VTEXT)
				return (ETXTBSY);
		} else
#endif
			PAGER_UNCACHE_TRY(vp);
	} else
		BM(VN_UNLOCK(vp));
	return (0);
}

/*
 * Vnode version of rdwri() for calls on file systems.
 */
vn_rdwr(rw, vp, base, len, offset, segflg, ioflg, cred, aresid)
	enum uio_rw rw;
	struct vnode *vp;
	caddr_t base;
	int len;
	off_t offset;
	enum uio_seg segflg;
	int ioflg;
	struct ucred *cred;
	int *aresid;
{
	int error;
	struct uio auio;
	struct iovec aiov;

	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	aiov.iov_base = base;
	aiov.iov_len = len;
	auio.uio_resid = len;
	auio.uio_offset = offset;
	auio.uio_segflg = segflg;
	auio.uio_rw = rw;
	if (rw == UIO_READ) {
		VOP_READ(vp, &auio, ioflg, cred, error);
	} else {
		VOP_WRITE(vp, &auio, ioflg, cred, error);
	}

	if (aresid)
		*aresid = auio.uio_resid;
	else
		if (auio.uio_resid && error == 0)
			error = EIO;
	return (error);
}

#ifdef PARACORE
/*
 * Vnode version of getdirentries() for calls on file systems.
 */
vn_readdir(vp, base, len, offset, cred, aresid)
	struct vnode *vp;
	caddr_t base;
	int len;
	off_t *offset;
	struct ucred *cred;
	int *aresid;
{
	int error, eofflag;
	struct uio auio;
	struct iovec aiov;
	enum vtype type;

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VDIR)
		return (EINVAL);
	
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	aiov.iov_base = base;
	aiov.iov_len = len;
	auio.uio_resid = len;
	auio.uio_offset = *offset;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
	VOP_READDIR(vp, &auio, cred, &eofflag, error);

	*offset = auio.uio_offset;
	if (aresid)
		*aresid = auio.uio_resid;
	else
		if (auio.uio_resid && error == 0)
			error = EIO;
	return (error);
}
#endif PARACORE

vn_read(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
{
	register struct vnode *vp = (struct vnode *)fp->f_data;
	int count, error, flag, iolocked = 0;
	u_long v_flag;
	enum vtype type;

	VN_LOCK(vp);
	type = vp->v_type;
	v_flag = vp->v_flag;
	VN_UNLOCK(vp);
	if (type == VREG || type == VDIR || type == VBLK) {
 		FP_IO_LOCK(fp);
		iolocked++;
	}
	uio->uio_offset = fp->f_offset;
	count = uio->uio_resid;

	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if (v_flag & VENF_LOCK) {
                /* mandatory file locking enabled and locks exist on file */
	        /* was flag&FNDELAY */
	        if (error = locked(fp, flag&(FNDELAY|FNONBLOCK), uio,
			       1, MAN_LOCK)) {
		      goto out;
		}
	}
#ifdef i386
	if (v_flag & VXENIX) {
                /* Xenix file locks exist on file */
		/* was flag&FNDELAY */
	        if (error = locked(fp, flag&(FNDELAY|FNONBLOCK), uio,
			       1, XENIX_LOCK))
		      goto out;
	}
#endif
	/* was flag&FNDELAY */
	VOP_READ(vp, uio, (flag & (FNDELAY|FNONBLOCK)) ? IO_NDELAY : 0,
		 cred, error);
	FP_LOCK(fp);
	fp->f_offset += count - uio->uio_resid;
	FP_UNLOCK(fp);
out:
	if (iolocked)
		FP_IO_UNLOCK(fp);
	return (error);
}

vn_write(fp, uio, cred)
	struct file *fp;
	struct uio *uio;
	struct ucred *cred;
{
	register struct vnode *vp = (struct vnode *)fp->f_data;
	int count, flag, error, ioflag = 0, iolocked = 0;
	u_long v_flag;
	enum vtype type;

	VN_LOCK(vp);
	type = vp->v_type;
	v_flag = vp->v_flag;
	VN_UNLOCK(vp);
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if (type == VREG && (flag & FAPPEND))
		ioflag |= IO_APPEND;
	/* was (flag & FNDELAY) */
	if (flag & (FNDELAY|FNONBLOCK))
		ioflag |= IO_NDELAY;
	if (flag & FSYNC)
		ioflag |= IO_SYNC;
	if (type == VREG || type == VDIR || type == VBLK) {
		FP_IO_LOCK(fp);
		iolocked++;
	}
	uio->uio_offset = fp->f_offset;
	count = uio->uio_resid;

	if (v_flag & VENF_LOCK) {
                /* mandatory file locking enabled and locks exist on file */
	        /* was flag&FNDELAY */
	        if (error = locked(fp, flag&(FNDELAY|FNONBLOCK), uio,
			       0, MAN_LOCK)) {
		      goto out;
		}
	}
#ifdef i386
	if (v_flag & VXENIX) {
                /* Xenix file locks exist on file */
	        if (error = locked(fp, flag&FNDELAY, uio, 0, XENIX_LOCK))
		      goto out;
	}
#endif
	VOP_WRITE(vp, uio, ioflag, cred, error);
	FP_LOCK(fp);
	if (ioflag & IO_APPEND)
		fp->f_offset = uio->uio_offset;
	else
		fp->f_offset += count - uio->uio_resid;
	FP_UNLOCK(fp);
out:
	if (iolocked)
		FP_IO_UNLOCK(fp);
	return (error);
}

/*
 * Get stat info for a vnode.
 */
vn_stat(vp, sb)
	struct vnode *vp;
	register struct stat *sb;
{
	struct vattr vattr;
	register struct vattr *vap;
	int error;
	u_short mode;
	enum vtype type;

#if	SEC_ARCH
	VOP_ACCESS(vp, SP_STATACC, u.u_cred, error);
	if (error)
		return error;
#endif
	vap = &vattr;
	VOP_GETATTR(vp, vap, u.u_cred, error);
	if (error)
		return (error);
	/*
	 * Copy from vattr table
	 */
#ifdef OSF1_ADFS
	/* m_mountid's uniqueness spans nodes */
        sb->st_dev = vp->v_mount->m_mountid;
#else
	sb->st_dev = vap->va_fsid;
#endif
	sb->st_ino = vap->va_fileid;
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
		return (EBADF);
	};
	sb->st_mode = mode;
	sb->st_nlink = vap->va_nlink;
	sb->st_uid = vap->va_uid;
	sb->st_gid = vap->va_gid;
	sb->st_rdev = vap->va_rdev;
	sb->st_size = vap->va_size;
	sb->st_atime = vap->va_atime.tv_sec;
#ifdef	CHKPNT
	sb->st_spare1 = vap->va_node;
#else	CHKPNT
	sb->st_spare1 = 0;
#endif	CHKPNT
	sb->st_mtime = vap->va_mtime.tv_sec;
	sb->st_spare2 = 0;
	sb->st_ctime = vap->va_ctime.tv_sec;
	sb->st_spare3 = 0;
	sb->st_blksize = vap->va_blocksize;
	sb->st_flags = vap->va_flags;
	sb->st_gen = vap->va_gen;
	sb->st_blocks = vap->va_bytes / S_BLKSIZE;
	return (0);
}

/*
 * Vnode ioctl call
 */
vn_ioctl(fp, com, data)
	struct file *fp;
	int com;
	caddr_t data;
{
	register struct vnode *vp = ((struct vnode *)fp->f_data);
	struct vattr vattr;
	int error, flag, size;
	off_t offset;
	enum vtype type;
	extern void spec_setopen();

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	switch (type) {

	case VREG:
	case VDIR:
		if (com == FIONREAD) {
#if	SEC_ARCH
			/*
			 * If a read access check was not performed at open
			 * time, perform a SP_STATACC access check now.
			 */
			BM(FP_LOCK(fp));
			flag = fp->f_flag;
			BM(FP_UNLOCK(fp));
			if ((flag & FREAD) == 0) {
				VOP_ACCESS(vp, SP_STATACC, u.u_cred, error);
				if (error)
					return error;
			}
#endif

#if	MAPPED_FILES
			if (MF_MAPPABLE(vp)) {
				mf_get_size_and_offset(fp, &size, &offset);
				*(off_t *)data = size - offset;
			} else
#endif
			{	
				FP_IO_LOCK(fp);
				VOP_GETATTR(vp, &vattr, u.u_cred, error);
				if (error) {
					FP_IO_UNLOCK(fp);
					return (error);
				}
				*(off_t *)data = vattr.va_size - fp->f_offset;
				FP_IO_UNLOCK(fp);
			}
			return (0);
		}
		if (com == FIONBIO || com == FIOASYNC)	/* XXX */
			return (0);			/* XXX */
		/* fall into ... */

	default:
		return (ENOTTY);

	case VFIFO:
	case VCHR:
	case VBLK:
#if	SEC_ARCH
		/*
		 * If read and write access checks were not performed at open
		 * time, perform a SP_IOCTLACC access check now.
		 */
		BM(FP_LOCK(fp));	/* possibly combine with op below? */
		flag = fp->f_flag;
		BM(FP_UNLOCK(fp));
		if ((flag & (FREAD|FWRITE)) != (FREAD|FWRITE)) {
			VOP_ACCESS(vp, SP_IOCTLACC, u.u_cred, error);
			if (error)
				return error;
		}
#endif
		BM(FP_LOCK(fp));
		flag = fp->f_flag;
		BM(FP_UNLOCK(fp));
		VOP_IOCTL(vp, com, data, flag, u.u_cred, error);
		if (error == 0 && com == TIOCSCTTY) {
			struct tty *ttyp;
			struct vproc *vs;
			unix_master();	/* sessions, sigh */
			vs = LOCATE_VPROC_PID(u.u_procp->p_sid);
			(void) VPOP_CTTY_GETATTR(u.u_procp->p_vproc, vs,
						 0, 0, 0, &ttyp);
			VPROC_RELEASE(vs, "vn_ioctl");
			ttyp->t_vnode = vp;
			unix_release();
			/*
			 * it's as if we're doing an open
			 */
			spec_setopen(vp);
		}
		return (error);
	}
}

/*
 * Vnode select call
 */
vn_select(fp, events, revents, scanning)
	struct file *fp;
	short *events, *revents;
	int scanning;
{
	int error;

	VOP_SELECT(((struct vnode *)fp->f_data), events, revents, scanning, u.u_cred, error);
	return(error);
}

/*
 * Vnode close call
 */
vn_close(fp)
	register struct file *fp;
{
	struct vnode *vp = ((struct vnode *)fp->f_data);
	int error, flag;

#if	UNIX_LOCKS
	/*
	 * Last close on a file structure; no other outstanding
	 * references.  Normal locking considerations do not apply:
	 * flags won't change unexpectedly, etc.
	 */
	ASSERT(fp->f_count == 1);
#endif
	BM(FP_LOCK(fp));
	flag = fp->f_flag;
	BM(FP_UNLOCK(fp));
	if (flag & (FSHLOCK|FEXLOCK))
		vn_funlock(fp, FSHLOCK|FEXLOCK);
	/*
	 * Must delete vnode reference from this file entry
	 * before VOP_CLOSE, so that only other references
	 * will prevent close.
	 */
	/*
	 * Shouldn't have to do this at all.  Who else checks f_data?  XXX
	 */
	FP_LOCK(fp);
	fp->f_data = (caddr_t) 0;
	FP_UNLOCK(fp);
	VOP_CLOSE(vp, flag, u.u_cred, error);
	VN_LOCK(vp);
	/*
	 * Don't decrement reader and writer counts for fifos.
	 * fifo_close takes care of that.
	 */
	if (vp->v_type != VFIFO) {
		if (flag & FREAD)
			vp->v_rdcnt--;
		if (flag & FWRITE)
			vp->v_wrcnt--;
	}
	VN_UNLOCK(vp);
	/*
	 * Unmount can race close as follows:
	 *	- unmount flushes the buffer cache
	 *	+ active vnode writer writes into buf cache
	 *	+ active vnode writer closes vnode
	 *	- unmount verifies no active vnodes on fs
	 *	- unmount allows fs to be unmounted, leaving
	 *	bogus buffer in memory.
	 * By taking the associated mount structure's unmount
	 * lock, the close will wait for the unmount to find
	 * this active vnode. This will cause the unmount to
	 * fail, but that's life at the races.
	 * N.B.  Only do this for vnodes attached to filesystems.
	 */
	while (vp->v_mount != DEADMOUNT) {
		struct mount *mp = vp->v_mount;

		if (!UNMOUNT_TRY_READ(mp)) {
			/* 
			 * couldn't get the lock, so sleep until
			 * the unmount is done.
			 */
			thread_block();
			continue;
		}
		vrele(vp);
		UNMOUNT_READ_UNLOCK(mp);
		return (error);
	}
	vrele(vp);
	return (error);
}

/*
 * Place an advisory lock on a vnode.
 * !! THIS IMPLIES THAT ALL STATEFUL FILE SERVERS WILL USE file table entries
 */
vn_flock(fp, cmd)
	register struct file *fp;
	int cmd;
{
	int error;
	register struct vnode *vp = (struct vnode *)fp->f_data;

	FP_LOCK(fp);

	/*
	 * If there's a exclusive lock currently applied
	 * to the file, then we've gotta wait for the
	 * lock with everyone else.
	 */
again:
	VN_LOCK(vp);
	while (vp->v_flag & VEXLOCK) {
		/*
		 * If we're holding an exclusive
		 * lock, then release it.
		 */
		if (fp->f_flag & FEXLOCK) {
			VN_UNLOCK(vp);
			FP_UNLOCK(fp);
			vn_funlock(fp, FEXLOCK);
			FP_LOCK(fp);
			VN_LOCK(vp);
			continue;
		}
		if (cmd & LOCK_NB) {
			VN_UNLOCK(vp);
			FP_UNLOCK(fp);
			return (EWOULDBLOCK);
		}
		/*
		 * Since we have both fp and vp locked, and you must
		 * get the fp lock first (see vn_funlock).  It should
		 * be safe to unlock the vnode, and let mpsleep
		 * unlock the fp for us.  mpsleep leaves lock unlocked
		 * on errors.
		 */
		vp->v_flag |= VLWAIT;
		VN_UNLOCK(vp);
		if (error = mpsleep((caddr_t)&vp->v_exlockc, PZERO + 1 | PCATCH,
				   "vn_exlock", 0, 
				   (void *) simple_lock_addr(fp->f_incore_lock),
				   MS_LOCK_SIMPLE))
			return (error);
		LASSERT(FP_LOCK_HOLDER(fp));
		VN_LOCK(vp);
	}
	if ((cmd & LOCK_EX) && (vp->v_flag & VSHLOCK)) {
		/*
		 * Must wait for any shared locks to finish
		 * before we try to apply a exclusive lock.
		 *
		 * If we're holding a shared
		 * lock, then release it.
		 */
		if (fp->f_flag & FSHLOCK) {
			VN_UNLOCK(vp);
			FP_UNLOCK(fp);
			vn_funlock(fp, FSHLOCK);
			FP_LOCK(fp);
			goto again;
		}
		if (cmd & LOCK_NB) {
			VN_UNLOCK(vp);
			FP_UNLOCK(fp);
			return (EWOULDBLOCK);
		}
		/*
		 * See comment above about use of mpsleep().
		 */
		vp->v_flag |= VLWAIT;
		VN_UNLOCK(vp);
		if (error = mpsleep((caddr_t)&vp->v_shlockc, PZERO + 1 | PCATCH,
				   "vn_shlock", 0, 
				   (void *) simple_lock_addr(fp->f_incore_lock),
				   MS_LOCK_SIMPLE))
			return (error);
		LASSERT(FP_LOCK_HOLDER(fp));
		goto again;
	}
	if (fp->f_flag & FEXLOCK)
		panic("vn_flock");
	if (cmd & LOCK_EX) {
		cmd &= ~LOCK_SH;
		vp->v_exlockc++;
		vp->v_flag |= VEXLOCK;
		fp->f_flag |= FEXLOCK;
	}
	if ((cmd & LOCK_SH) && (fp->f_flag & FSHLOCK) == 0) {
		vp->v_shlockc++;
		vp->v_flag |= VSHLOCK;
		fp->f_flag |= FSHLOCK;
	}
	VN_UNLOCK(vp);
	FP_UNLOCK(fp);
	return (0);
}

/*
 * Unlock a file.
 */
vn_funlock(fp, kind)
	register struct file *fp;
	int kind;
{
	register struct vnode *vp = (struct vnode *)fp->f_data;
	int flags;

	FP_LOCK(fp);
	kind &= fp->f_flag;
	if (vp == NULL || kind == 0) {
		FP_UNLOCK(fp);
		return;
	}
	VN_LOCK(vp);
	flags = vp->v_flag;
	if (kind & FSHLOCK) {
		if ((flags & VSHLOCK) == 0)
			panic("vn_funlock: SHLOCK");
		if (--vp->v_shlockc == 0) {
			vp->v_flag &= ~VSHLOCK;
			if (flags & VLWAIT)
				thread_wakeup((int)&vp->v_shlockc);
		}
		fp->f_flag &= ~FSHLOCK;
	}
	if (kind & FEXLOCK) {
		if ((flags & VEXLOCK) == 0)
			panic("vn_funlock: EXLOCK");
		if (--vp->v_exlockc == 0) {
			vp->v_flag &= ~(VEXLOCK|VLWAIT);
			if (flags & VLWAIT)
				thread_wakeup((int)&vp->v_exlockc);
		} 
		fp->f_flag &= ~FEXLOCK;
	}
	VN_UNLOCK(vp);
	FP_UNLOCK(fp);
}

/*
 * Noop
 */
vfs_noop()
{

	return (ENXIO);
}

/*
 * Null op
 */
vfs_nullop()
{

	return (0);
}

#ifdef	TNC
#ifdef	CHKPNT
/*
 * vn_fhtovp() - convert a fh to a vnode ptr (optionally locked)
 * 	- look up fsid in mount list (if not found ret error)
 *	- get vp by calling VFS_FHTOVP() macro
*/
vn_fhtovp(fhp, lockflag, vpp)
	fhandle_t *fhp;
	int lockflag;
 	struct vnode **vpp;
{
 	register struct mount *mp;
 	int error;
 
 	if ((mp = getvfs(&fhp->fh_fsid)) == NULL)
 		return (ESTALE);
 	VFS_FHTOVP(mp, &fhp->fh_fid, vpp, error);
 	/* getvfs returned the mount structure locked */
 	UNMOUNT_READ_UNLOCK(mp);
 	if (error)
 		return (ESTALE);
 	return (0);
}

/*
 * Borrowed from vn_open().  First translate the file
 * handle to a vnode, and then use VOP_OPEN to re-open
 * this particular file.
 */
vn_openfh(ndp, fhp, fmode)
	register struct nameidata *ndp;
	struct fhandle *fhp;
	int fmode;
{
	struct vnode *vp;
	struct vnode *newvp;
	struct vattr vat;
	struct vattr *vap = &vat;
	int error;
	enum vtype	type;
	u_long v_flag;
#ifdef COMPAT_43
	int oflag;
	extern void spec_setopen();
	PPROC_CTTY_SETUP(u.u_procp);
#endif

	/*
	 * Call vn_fhtovp() to translate our newly constructed 
	 * file handle to a legitimate vnode pointer.
	 * It seems that the lockflag is not being used, so
	 * just 0 for now.
	 */
	error = vn_fhtovp(fhp, 0, (struct vnode **) &vp);
	
	/* 
	 * Make sure the file has not been unlinked or unmounted.
	 * When error is ESTALE, vp should be NULL.  But return. 
	 */
	if (error == ESTALE) return(error);

	VN_LOCK(vp);
	type = vp->v_type;
	v_flag = vp->v_flag;
	VN_UNLOCK(vp);
	if (type == VSOCK) {
		error = EOPNOTSUPP;
		goto bad;
	}

	/* 
	 * Since we assume that the file has already been
	 * opend, we just have to make sure it is accessible.
	 */
	if (fmode & FCREAT){
		VOP_ACCESS(vp, VREAD, ndp->ni_cred, error);
		if (error)
		goto bad;
	}

	if ((fmode & FCREAT) == 0) {
		if (fmode & FREAD) {
			VOP_ACCESS(vp, VREAD, ndp->ni_cred, error);
			if (error)
				goto bad;
		}
		if (fmode & (FWRITE|FTRUNC)) {
			/*
			 * This should probably be changed, since
			 * POSIX says TRUNC on directory has no effect
			 */
			if (type == VDIR) {
				error = EISDIR;
				goto bad;
			}
			if (error = vn_writechk(vp))
				goto bad;
			VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
			if (error)
				goto bad;
		}
	}
	if ((fmode & FTRUNC) && type != VFIFO) {
	        /* truncating - check enforcement mode locks */
	        if (v_flag & VENF_LOCK) {
	                error = EAGAIN;
	                goto bad;
		}
#ifdef i386
		/* check Xenix locks */
	        if (v_flag & VXENIX) {
	                error = EAGAIN;
	                goto bad;
		}
#endif
		vattr_null(vap);
		vap->va_size = 0;
		VOP_SETATTR(vp, vap, ndp->ni_cred, error);
		if (error)
			goto bad;
#if	SEC_BASE
		/*
		 * Set audit event type to ET_OBJECT_MOD
		 */
		audstub_open2();
#endif
	}
#ifdef COMPAT_43
	if (type == VCHR) {
		BM(unix_master());
		oflag = u.u_procp->p_flag;
		BM(unix_release());
	}
#endif
	newvp = vp;
	VOP_OPEN(&newvp, fmode, ndp->ni_cred, error);
	vp = newvp;
	ndp->ni_vp = vp;	/* in case it got cloned */
#ifdef COMPAT_43
	/*
	 * If a controlling tty was implicitly assigned,
	 * we must set the session controlling tty vnode
	 * pointer.  See code in tty.c:ttyopen().
	 */
	unix_master();
	if (!error && type == VCHR && !(fmode&O_NOCTTY) && !(oflag&SCTTY) &&
	    (u.u_procp->p_flag&SCTTY)) {
		struct tty *ttyp;
		struct vproc *vs = LOCATE_VPROC_PID(u.u_procp->p_sid);
		(void) VPOP_CTTY_GETATTR(u.u_procp->p_vproc, vs, 0, 0, 0, &ttyp);
		VPROC_RELEASE(vs, "vn_open");
		ttyp->t_vnode = vp;
		unix_release();
		/*
		 * it's as if we're doing an open
		 */
		spec_setopen(vp);
	} else
		unix_release();
#endif /* COMPAT_43 */
	if (error)
		vrele(vp);
	else {
		VN_LOCK(vp);
		/*
		 * Don't increment reader and writer counts for fifos.
		 * fifo_open takes care of that.
		 */
		if (type != VFIFO) {
			if (fmode & FREAD)
				vp->v_rdcnt++;
			if (fmode & FWRITE)
				vp->v_wrcnt++;
		}
		VN_UNLOCK(vp);
	}
	return (error);
bad:
	vrele(vp);
	return(error);
}
#endif	CHKPNT
#endif	/* TNC */
