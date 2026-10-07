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
 * $Log: nfs_vnops.c,v $
 * Revision 1.12  1994/11/18  20:37:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.11  1994/07/02  00:29:01  dbm
 * Fixed a bug from the OSF merge of MAXBSIZE/DEV_BSIZE functionality.  A
 * misplaced ) was causing an incorrect byte count for NFS files.
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #:10102
 *  Testing: Specific test case.
 *  Module(s):
 *
 * Revision 1.10  1994/06/28  23:05:07  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.9  1994/01/11  18:26:41  jlitvin
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
 * Revision 1.8  1993/10/06  17:26:03  dbm
 * Removed incorrect bug fix for bug #3597.  This was causing root not to
 * be able to access files correctly.  This fixes bugs #6834, 5939, and 4422.
 *
 * Revision 1.7  1993/09/09  15:53:58  cfj
 * Fix for PTS bug #2943.  Replaced nfs_sillyrename() with a new version
 * which expands the name space of the nfs temporary files.
 *
 * Revision 1.6  1993/07/14  18:16:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:49  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/06/16  22:55:47  brad
 * Added datain and dataout to NFS vnode ops (added to UFS vnode ops by
 * OSF).  Added support for PFS file striping into NFS file systems.
 *
 * Revision 1.4  1993/05/06  20:29:38  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:57  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/03/11  21:07:36  wunder
 * Two changes. Removal of automatic root access within nfs_access(). Adding
 * OSF changes fix busy filesystem after creating devices and FIFO files.
 *
 * Revision 2.9  1993/02/26  16:37:09  durriya
 * 	Convert ~0 before sending to mknod. Do what ufs does and drop ref after
 * 	mknod - fixes busy filesystem after creating devices & fifos.  (durriya)
 *
 * Revision 1.2  1992/11/30  22:32:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:54  cfj
 * Bump major revision number.
 *
 * Revision 2.9  1992/10/22  15:25:18  dbm
 * Updated for PFS functionality.
 *
 * Revision 2.10  93/10/20  15:26:40  dnoveck
 *      DEV_BSIZE elimination: Change use of DEV_BSIZE defines to their
 *      DISK_GRANULE-based corelates.
 *
 * Revision 2.8  1992/09/20  11:26:28  roy
 * 	Another op for OSF1_ADFS.
 * 	[92/09/15            roy]
 * 
 * Revision 2.7  92/08/26  12:11:54  loverso
 * 	Additional ops for OSF1_ADFS.
 * 	[92/08/19            roy]
 * 
 * Revision 2.6  92/03/09  12:16:14  durriya
 * 	Revision 3.3  91/12/18  17:17:45  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.5  92/01/05  20:04:17  roy
 * 	91/12/23  14:33:19  roy
 * 	Back out last change.
 * 
 * Revision 2.4  91/11/26  13:31:59  rabii
 * 	Removed rmknod entries no longer needed, also added extra
 * 	argument to nfs_mknod
 * 
 * Revision 2.3  91/11/25  11:26:25  rabii
 * 	Added entries for rmknod in tables for remote devices
 * 
 * Revision 2.2  91/08/31  13:52:29  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  18:01:07  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9.4.2  91/02/19  15:25:06  tmt
 * 	Fix leak when renaming over existing file.
 * 	[91/02/16  15:53:35  tmt]
 * 
 * Revision 1.9  90/10/31  14:02:11  devrcs
 * 	Added nfs_seek to handle seeks on regular files, call spec_seek on
 * 	devices.
 * 	[90/10/08  17:11:41  collins]
 * 
 * Revision 1.8  90/10/07  14:39:42  devrcs
 * 	Handle VSOCK creation in nfs_create.
 * 	[90/10/03  16:13:23  tmt]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:53  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:18  gm]
 * 
 * 	Change buffer lock ownership debugging code to eliminate
 * 	race conditions.
 * 	[90/09/23  19:30:23  jeffc]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:08:45  collins]
 * 
 * Revision 1.7  90/09/23  15:56:30  devrcs
 * 	Eliminated init argument to page_write. Added pager and offset args.
 * 	Also eliminated nfs_page_exists.
 * 	[90/09/11  06:53:52  ers]
 * 
 * 	Don't nfs_create a non-VREG file. Add fifo_ioctl operation to vector.
 * 	[90/09/05  17:33:28  tmt]
 * 
 * Revision 1.6  90/09/13  11:48:42  devrcs
 * 	Changed regular file select routine to be seltrue().
 * 	[90/08/28  17:04:03  coren]
 * 
 * Revision 1.5  90/08/24  12:16:14  devrcs
 * 	removed u.u_error references
 * 	[90/08/20  12:34:57  gmf]
 * 
 * 	Verify buffer is  locked before calling biodone.
 * 	[90/08/19  01:12:40  nags]
 * 
 * Revision 1.4  90/07/27  09:04:49  devrcs
 * 	Changed nfs_page_read and nfs_page_write to take a uio instead
 * 	of individual arguments.
 * 	[90/07/23  14:38:35  ers]
 * 
 * 	NFS parallelization, clone driver support.
 * 	[90/07/20  17:04:12  nags]
 * 
 * 	Make calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/18  08:36:08  seiden]
 * 
 * Revision 1.3  90/07/17  11:37:32  devrcs
 * 	More changes for gcc.
 * 	[90/07/07  22:58:53  gm]
 * 
 * 	Condensed history (reverse chronology):
 * 
 * 	Parallelizaed for OSF/1					nags@encore.com
 * 	Added support for FIFOs.				ers@osf.org
 * 	Lost the extern nfs_xdrneg1, put it back.		tmt@osf.org
 * 	Fixes for fxdr and xdr VNOVAL				tmt@osf.org
 * 	Fixed nfs_setattr - type widening			gmf@osf.org
 * 	Secureware: least privilege, MAC, DAC, auditing		seiden@osf.org
 * 	Added support for kludged nfs_mknod 			gmf@osf.org
 * 	Serialized for OSF/1.					nags@encore.com
 * 	Mods for NFS going through buffer cache.		tmt@osf.org
 * 	Fix from Guelph for nfs_mkdir "retry".			tmt@osf.org
 * 	Return actual error from nfs_lookup not ENOENT.		tmt@osf.org
 * 	Forgot a m_bsize.					gmf@osf.org
 * 	Change assoc. with removing a m_fsid, m_bsize, m_fsize.	gmf@osf.org
 * 	Eof fix; nfs_readdir; named calls zero direofcache.	gmf@osf.org
 * 	Update to osc.6 and mount hang fix.			gmf@osf.org
 * 	Connectathon fixes.					gmf,tmt@osf.org
 * 	NFS attribute cache fixes.				gmf@osf.org
 * 	Eliminate #ifdef VOP_PRINT (compatibility code).	gmf@osf.org
 * 	Changes to make async_daemon work.			gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				gm@osf.org
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
 *	@(#)nfs_vnops.c	7.22 (Berkeley) 1/4/90
 */

/*
 * vnode op calls for sun nfs version 2
 */

#include <sys/unix_defs.h>
#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif
#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/kernel.h>
#include <sys/mount.h>
#include <sys/buf.h>
#include <ufs/dir.h>
#if	MACH
#include <kern/zalloc.h>
#include <kern/assert.h>
#include <mach/memory_object.h>
#include <builtin/inode_pager.h>
#include <kern/sched_prim.h>
#include <kern/mfs.h>
#else
#include <sys/malloc.h>
#endif
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/ucred.h>
#include <sys/namei.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/conf.h>
#include <sys/vnode.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#include <ufs/inode.h>
#include <sys/map.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>
#if	!MACH
#include <nfs/nfsiom.h>
#endif

/* Global vars */
int	nfs_lookup(),
	nfs_create(),
	nfs_mknod(),
	nfs_open(),
	nfs_close(),
	nfs_access(),
	nfs_getattr(),
	nfs_setattr(),
	nfs_read(),
	nfs_write(),
	seltrue(),
	vfs_noop(),
	nfs_remove(),
	nfs_link(),
	nfs_rename(),
	nfs_mkdir(),
	nfs_rmdir(),
	nfs_symlink(),
	nfs_readdir(),
	nfs_readlink(),
	nfs_abortop(),
	nfs_bmap(),
	nfs_strategy(),
	nfs_fsync(),
	nfs_seek(),
	nfs_inactive(),
	nfs_reclaim(),
	nfs_print(),
	nfs_page_read(),
	nfs_page_write();
#ifdef	PFS
int	nfs_noop();
#endif

/* Special device vnode ops */
int	spec_lookup(),
	spec_open(),
	spec_read(),
	spec_write(),
	spec_strategy(),
	spec_bmap(),
	spec_ioctl(),
	spec_select(),
	spec_seek(),
	spec_close(),
	spec_badop(),
	nfsspec_reclaim(),
	spec_nullop();

struct vnodeops nfsv2_vnodeops = {
	nfs_lookup,		/* lookup */
	nfs_create,		/* create */
	nfs_mknod,		/* mknod */
	nfs_open,		/* open */
	nfs_close,		/* close */
	nfs_access,		/* access */
	nfs_getattr,		/* getattr */
	nfs_setattr,		/* setattr */
	nfs_read,		/* read */
	nfs_write,		/* write */
	vfs_noop,		/* ioctl */
	seltrue,		/* select */
	vfs_noop,		/* mmap */
	nfs_fsync,		/* fsync */
	nfs_seek,		/* seek */
	nfs_remove,		/* remove */
	nfs_link,		/* link */
	nfs_rename,		/* rename */
	nfs_mkdir,		/* mkdir */
	nfs_rmdir,		/* rmdir */
	nfs_symlink,		/* symlink */
	nfs_readdir,		/* readdir */
	nfs_readlink,		/* readlink */
	nfs_abortop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfs_reclaim,		/* reclaim */
	nfs_bmap,		/* bmap */
	nfs_strategy,		/* strategy */
	nfs_print,		/* print */
	nfs_page_read,		/* page in */
	nfs_page_write,		/* page out */
#ifdef	PFS
	nfs_noop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
	nfs_read,		/* data in */
	nfs_write,		/* data out */
#endif
};

struct vnodeops spec_nfsv2nodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	spec_open,		/* open */
	spec_close,		/* close */
	nfs_access,		/* access */
	nfs_getattr,		/* getattr */
	nfs_setattr,		/* setattr */
	spec_read,		/* read */
	spec_write,		/* write */
	spec_ioctl,		/* ioctl */
	spec_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfsspec_reclaim,	/* reclaim */
	spec_bmap,		/* bmap */
	spec_strategy,		/* strategy */
	nfs_print,		/* print */
	spec_badop,		/* page in */
	spec_badop,		/* page out */
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
	spec_badop,		/* data in */
	spec_badop,		/* data out */
#endif
};

int	fifo_open(),
	fifo_close(),
	fifo_read(),
	fifo_write(),
	fifo_ioctl(),
	nfsfifo_getattr(),
	fifo_select();

struct vnodeops fifo_nfsv2nodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	fifo_open,		/* open */
	fifo_close,		/* close */
	nfs_access,		/* access */
	nfsfifo_getattr,	/* getattr */
	nfs_setattr,		/* setattr */
	fifo_read,		/* read */
	fifo_write,		/* write */
	fifo_ioctl,		/* ioctl */
	fifo_select,		/* select */
	spec_badop,		/* mmap */
	spec_nullop,		/* fsync */
	spec_seek,		/* seek */
	spec_badop,		/* remove */
	spec_badop,		/* link */
	spec_badop,		/* rename */
	spec_badop,		/* mkdir */
	spec_badop,		/* rmdir */
	spec_badop,		/* symlink */
	spec_badop,		/* readdir */
	spec_badop,		/* readlink */
	spec_badop,		/* abortop */
	nfs_inactive,		/* inactive */
	nfs_reclaim,		/* reclaim */
	spec_bmap,		/* bmap */
	spec_badop,		/* strategy */
	nfs_print,		/* print */
	spec_badop,		/* page in */
	spec_badop,		/* page out */
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
	spec_badop,		/* data in */
	spec_badop,		/* data out */
#endif
};

extern u_long nfs_procids[NFS_NPROCS];
extern u_long nfs_prog, nfs_vers;
extern u_long nfs_xdrneg1;

#if	!MACH
/*
 * deal with these for MACH later
 */
extern char nfsiobuf[MAXPHYS+NBPG];
struct map nfsmap[NFS_MSIZ];
#endif	/* !MACH */

enum vtype v_type[NFFIFO+1];
struct buf nfs_bqueue;		/* Queue head for nfsiod's */
int nfs_asyncdaemons = 0;
int asyncdaemon_requests = 0;
extern int nonidempotent[NFS_NPROCS];
static int nfsmap_want = 0;


/*
 * nfs null call from vfs.
 */
nfs_null(vp, cred)
	struct vnode *vp;
	struct ucred *cred;
{
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb;
	
	nfsm_reqhead(nfs_procids[NFSPROC_NULL], cred, 0);
	nfsm_request(vp, nonidempotent[NFSPROC_NULL]);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs access vnode op.
 * Essentially just get vattr and then imitate iaccess()
 */
nfs_access(vp, mode, cred)
	struct vnode *vp;
	int mode;
	register struct ucred *cred;
{
#if SEC_ARCH
	register struct vattr	*vap;
	struct vattr	vattr;
	udac_t		udac;
	int		error;

	vap = &vattr;
	if (error = nfs_getattr(vp, vap, cred))
		return (error);

	udac.uid = udac.cuid = vap->va_uid;
	udac.gid = udac.cgid = vap->va_gid;
	udac.mode = vap->va_mode & 07777;

	if (SP_ACCESS(SIP->si_tag, vp->v_mount->m_tag, mode, &udac)) {
		return EACCES;
	}
	return 0;
#else /* !SEC_ARCH */
	register struct vattr *vap;
	register gid_t *gp;
	struct vattr vattr;
	register int i;
	int error;

	/*
	 * If you're the super-user,
	 * you always get access across NFS.
	 */
#if SEC_BASE
	if (privileged(SEC_ALLOWDACACCESS, 0))
#else /* !SEC_BASE */
	if (cred->cr_uid == 0)
#endif /* !SEC_BASE */
		return (0);
	vap = &vattr;
	if (error = nfs_getattr(vp, vap, cred))
		return (error);
	/*
	 * Access check is based on only one of owner, group, public.
	 * If not owner, then check group. If not a member of the
	 * group, then check public access.
	 */
	if (cred->cr_uid != vap->va_uid) {
		mode >>= 3;
		if (vap->va_gid == cred->cr_gid)
			goto found;
		gp = cred->cr_groups;
		for (i = 0; i < cred->cr_ngroups; i++, gp++)
			if (vap->va_gid == *gp)
				goto found;
		mode >>= 3;
found:
		;
	}
	if ((vap->va_mode & mode) != 0)
		return (0);
	return (EACCES);
#endif /* !SEC_ARCH */
}

/*
 * nfs open vnode op
 * Just check to see if the type is ok
 */
/* ARGSUSED */
nfs_open(vpp, mode, cred)
	struct vnode **vpp;
	int mode;
	struct ucred *cred;
{
	register enum vtype type;
	register struct vnode *vp = *vpp;

	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type == VREG || type == VDIR || type == VLNK)
		return (0);
	else
		return (EACCES);
}

/*
 * nfs close vnode op
 * For reg files, invalidate any buffer cache entries.
 */
/* ARGSUSED */
nfs_close(vp, fflags, cred)
	register struct vnode *vp;
	int fflags;
	struct ucred *cred;
{
	register struct nfsnode *np = VTONFS(vp);
	int error = 0;
	register enum vtype vtyp;

	BM(VN_LOCK(vp));
	vtyp = vp->v_type;
	BM(VN_UNLOCK(vp));
	NP_LOCK(np);
	if (vtyp == VREG && (np->n_flag & NMODIFIED)) {
		np->n_flag &= ~NMODIFIED;
		np->n_attrstamp = 0;
		NP_UNLOCK(np);
		vinvalbuf(vp, TRUE);
		NP_LOCK(np);
		if (np->n_flag & NWRITEERR) {
			np->n_flag &= ~NWRITEERR;
			error = np->n_error;
		}
	}
	NP_UNLOCK(np);
	return (error);
}

/*
 * nfs getattr call from vfs.
 */
nfs_getattr(vp, vap, cred)
	register struct vnode *vp;
	struct vattr *vap;
	struct ucred *cred;
{
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	
	/* First look in the cache.. */
	if (nfs_getattrcache(vp, vap) == 0)
		return (0);
	NFS_STATS(nfsstats.rpccnt[NFSPROC_GETATTR]++);
	nfsm_reqhead(nfs_procids[NFSPROC_GETATTR], cred, NFSX_FH);
	nfsm_fhtom(vp);
	nfsm_request(vp, nonidempotent[NFSPROC_GETATTR]);
	nfsm_loadattr(vp, vap);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs setattr call.
 */
nfs_setattr(vp, vap, cred)
	register struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	register struct nfsv2_sattr *sp;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_SETATTR]++);
	nfsm_reqhead(nfs_procids[NFSPROC_SETATTR], cred, NFSX_FH+NFSX_SATTR);
	nfsm_fhtom(vp);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	if (vap->va_mode == (u_short)VNOVAL)
		sp->sa_mode = nfs_xdrneg1;
	else
		sp->sa_mode = vtonfs_mode(vp->v_type, vap->va_mode);
	if (vap->va_uid == (uid_t) VNOVAL)
		sp->sa_uid = nfs_xdrneg1;
	else
		sp->sa_uid = txdr_unsigned(vap->va_uid);
	if (vap->va_gid == (gid_t) VNOVAL)
		sp->sa_gid = nfs_xdrneg1;
	else
		sp->sa_gid = txdr_unsigned(vap->va_gid);
	sp->sa_size = txdr_unsigned(vap->va_size);
	sp->sa_atime.tv_sec = txdr_unsigned(vap->va_atime.tv_sec);
	sp->sa_atime.tv_usec = txdr_unsigned(vap->va_flags);
	txdr_time(&vap->va_mtime, &sp->sa_mtime);
	if (vap->va_size != VNOVAL || vap->va_mtime.tv_sec != VNOVAL ||
	    vap->va_atime.tv_sec != VNOVAL) {
		np = VTONFS(vp);
		NP_LOCK(np);
		if (np->n_flag & NMODIFIED) {
			np->n_flag &= ~NMODIFIED;
			np->n_attrstamp = 0;
			NP_UNLOCK(np);
			vinvalbuf(vp, TRUE);
#if	MACH
			inode_uncache(vp);
#endif
		} else
			NP_UNLOCK(np);
	}
	nfsm_request(vp, nonidempotent[NFSPROC_SETATTR]);
	nfsm_loadattr(vp, (struct vattr *)0);
	/* should we fill in any vap fields ?? */
	nfsm_reqdone;
	return (error);
}

/*
 * nfs lookup call, one step at a time...
 * First look in cache
 * If not found, do the rpc
 */
nfs_lookup(vp, ndp)
	register struct vnode *vp;
	register struct nameidata *ndp;
{
	register struct vnode *vdp;
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct vnode *newvp;
	long len;
	nfsv2fh_t *fhp;
	struct nfsnode *np;
	struct mount *mp;
	int wantparent, flag, error = 0;

	ndp->ni_dvp = vp;
	ndp->ni_vp = NULL;
	BM(VN_LOCK(vp));
	if (vp->v_type != VDIR) {
		BM(VN_UNLOCK(vp));
		return (ENOTDIR);
	}
	BM(VN_UNLOCK(vp));
	flag = ndp->ni_nameiop & OPFLAG;
	wantparent = ndp->ni_nameiop & WANTPARENT;
	if ((error = cache_lookup(ndp)) && error != ENOENT) {
		struct vattr vattr;
		int vpid;

		if (vp == ndp->ni_rdir && ndp->ni_isdotdot)
			panic("nfs_lookup: .. through root");
		vdp = ndp->ni_vp;
#if     UNIX_LOCKS
		vpid = ndp->ni_vpid;
#else
		vpid = vdp->v_id;
#endif
		if (vp == vdp) {
			VREF(vdp);
			error = 0;
		} else {
			VN_LOCK(vdp);
			error = vget_nowait(vdp);
			VN_UNLOCK(vdp);
		}
		if (!error) {
			if (vpid == vdp->v_id) {
			   error = nfs_getattr(vdp, &vattr, ndp->ni_cred);
			   np = VTONFS(vdp);
			   BM(NP_LOCK(np));
			   if (!error && vattr.va_ctime.tv_sec ==
			        np->n_ctime) {
				BM(NP_UNLOCK(np));
				NFS_STATS(nfsstats.lookupcache_hits++);
				return (0);
			   } else {
				BM(NP_UNLOCK(np));
				cache_purge(vdp);
				nfs_nput(vdp);
			   }
			} else
				nfs_nput(vdp);
		}
		ndp->ni_vp = (struct vnode *)0;
	}
	error = 0;
	NFS_STATS(nfsstats.lookupcache_misses++);
	NFS_STATS(nfsstats.rpccnt[NFSPROC_LOOKUP]++);
	len = ndp->ni_namelen;
	nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], ndp->ni_cred, NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
	nfsm_fhtom(vp);
	nfsm_strtom(ndp->ni_ptr, len, NFS_MAXNAMLEN);
	nfsm_request(vp, nonidempotent[NFSPROC_LOOKUP]);
nfsmout:
	if (error)
		return (error);
	nfsm_disect(fhp,nfsv2fh_t *,NFSX_FH);
	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	np = VTONFS(vp);
	/*
	 * Handle DELETE and RENAME cases...
	 */
	if (flag == DELETE && *ndp->ni_next == 0) {
		NP_LOCK(np);
		if (!bcmp(np->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
			NP_UNLOCK(np);
			VREF(vp);
			newvp = vp;
		} else {
			NP_UNLOCK(np);
			if (error = nfs_nget(mp, fhp, &np)) {
				m_freem(mrep);
				return (error);
			}
			newvp = NFSTOV(np);
		}
		if (error =
		    nfs_loadattrcache(newvp, &md, &dpos, (struct vattr *)0)) {
			nfs_nput(newvp);
			m_freem(mrep);
			return (error);
		}
		ndp->ni_vp = newvp;
		m_freem(mrep);
		return (0);
	}

	if (flag == RENAME && wantparent && *ndp->ni_next == 0) {
		NP_LOCK(np);
		if (!bcmp(np->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
			NP_UNLOCK(np);
			m_freem(mrep);
			return (EISDIR);
		}
		NP_UNLOCK(np);
		if (error = nfs_nget(mp, fhp, &np)) {
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
		if (error =
		    nfs_loadattrcache(newvp, &md, &dpos, (struct vattr *)0)) {
			nfs_nput(newvp);
			m_freem(mrep);
			return (error);
		}
		m_freem(mrep);
		ndp->ni_vp = newvp;
		return (0);
	}

	NP_LOCK(np);
	if (!bcmp(np->n_fh.fh_bytes, (caddr_t)fhp, NFSX_FH)) {
		NP_UNLOCK(np);
		VREF(vp);
		newvp = vp;
	} else if (ndp->ni_isdotdot) {
		NP_UNLOCK(np);
		if (error = nfs_nget(mp, fhp, &np)) {
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
	} else {
		NP_UNLOCK(np);
		if (error = nfs_nget(mp, fhp, &np)) {
			m_freem(mrep);
			return (error);
		}
		newvp = NFSTOV(np);
	}
	if (error = nfs_loadattrcache(newvp, &md, &dpos, (struct vattr *)0)) {
		nfs_nput(newvp);
		m_freem(mrep);
		return (error);
	}
	m_freem(mrep);

	ndp->ni_vp = newvp;
	if (error == 0 && ndp->ni_makeentry) {
		NP_LOCK(np);
		np->n_ctime = np->n_vattr.va_ctime.tv_sec;
		NP_UNLOCK(np);
		cache_enter(ndp);
	}
	return (error);
}

/*
 * nfs read call.
 * Just call nfs_bioread() to do the work.
 */
nfs_read(vp, uiop, ioflag, cred)
	register struct vnode *vp;
	struct uio *uiop;
	int ioflag;
	struct ucred *cred;
{
	BM(VN_LOCK(vp));
	if (vp->v_type != VREG) {
		BM(VN_UNLOCK(vp));
		return (EPERM);
	}
	BM(VN_UNLOCK(vp));

#ifdef	PFS
	if (VIO_IS_FASTPATH(vp)) {
		vm_address_t	buf;
		int		amount_requested, amount_read;
		int		error;

		amount_requested = uiop->uio_resid;
		if (vm_allocate(mach_task_self(), &buf, amount_requested, 
				TRUE) != KERN_SUCCESS) {
			return(ENOMEM);
		}
		uiop->uio_iov->iov_base = (caddr_t) buf;

		error = nfs_bioread(vp, uiop, ioflag, cred);
		if (error) {
			(void) vm_deallocate(mach_task_self(), buf,
					     amount_requested);
		} else {
			/*
			 * Satisfy VOP_DATAIN semantics by resetting iov_base
			 * (modified by nfs_bioread) to point to the data being
			 * returned.
			 */
			uiop->uio_iov->iov_base = (char *) buf;
			uiop->uio_iov->iov_len = amount_requested;

			/*
			 * Deallocate any unused pages of memory.
			 */
			amount_read = amount_requested - uiop->uio_resid;
			if (uiop->uio_resid > 0) {
				vm_address_t	start;
				start = (buf + round_page(amount_read));
				(void) vm_deallocate(mach_task_self(),
						     start, uiop->uio_resid);
			}
		}
		return(error);
	}
#endif	PFS

	return (nfs_bioread(vp, uiop, ioflag, cred));
}

/*
 * nfs readlink call
 */
nfs_readlink(vp, uiop, cred)
	struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	BM(VN_LOCK(vp));
	if (vp->v_type != VLNK) {
		BM(VN_UNLOCK(vp));
		return (EPERM);
	}
	BM(VN_UNLOCK(vp));
	return (nfs_bioread(vp, uiop, 0, cred));
}

/*
 * Do a readlink rpc.
 * Called by nfs_doio() from below the buffer cache.
 */
nfs_readlinkrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *p;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	long len;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_READLINK]++);
	nfsm_reqhead(nfs_procids[NFSPROC_READLINK], cred, NFSX_FH);
	nfsm_fhtom(vp);
	nfsm_request(vp, nonidempotent[NFSPROC_READLINK]);
	nfsm_strsiz(len, NFS_MAXPATHLEN);
	nfsm_mtouio(uiop, len);
	nfsm_reqdone;
	return (error);
}

/*
 * nfs read call
 */
nfs_readrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *p;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	long len, retlen, tsiz;
	struct mount *mp;

	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	nmp = vfs_to_nfs(mp);
	tsiz = uiop->uio_resid;
	while (tsiz > 0) {
		NFS_STATS(nfsstats.rpccnt[NFSPROC_READ]++);
		len = (tsiz > nmp->nm_rsize) ? nmp->nm_rsize : tsiz;
		nfsm_reqhead(nfs_procids[NFSPROC_READ], cred, NFSX_FH+NFSX_UNSIGNED*3);
		nfsm_fhtom(vp);
		nfsm_build(p, u_long *, NFSX_UNSIGNED*3);
		*p++ = txdr_unsigned(uiop->uio_offset);
		*p++ = txdr_unsigned(len);
		*p = 0;
		nfsm_request(vp, nonidempotent[NFSPROC_READ]);
		nfsm_loadattr(vp, (struct vattr *)0);
		nfsm_strsiz(retlen, nmp->nm_rsize);
		nfsm_mtouio(uiop, retlen);
		m_freem(mrep);
		if (retlen < len)
			tsiz = 0;
		else
			tsiz -= len;
	}
nfsmout:
	return (error);
}

/*
 * nfs write call
 */
nfs_writerpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register u_long *p;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	long len, tsiz;
	struct mount *mp;

	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	nmp = vfs_to_nfs(mp);
	tsiz = uiop->uio_resid;
	while (tsiz > 0) {
		NFS_STATS(nfsstats.rpccnt[NFSPROC_WRITE]++);
		len = (tsiz > nmp->nm_wsize) ? nmp->nm_wsize : tsiz;
		nfsm_reqhead(nfs_procids[NFSPROC_WRITE], cred,
			NFSX_FH+NFSX_UNSIGNED*4);
		nfsm_fhtom(vp);
		nfsm_build(p, u_long *, NFSX_UNSIGNED*4);
		*(p+1) = txdr_unsigned(uiop->uio_offset);
		*(p+3) = txdr_unsigned(len);
		nfsm_uiotom(uiop, len);
		nfsm_request(vp, nonidempotent[NFSPROC_WRITE]);
		nfsm_loadattr(vp, (struct vattr *)0);
		m_freem(mrep);
		tsiz -= len;
	}
nfsmout:
	return (error);
}

/*
 * nfs mknod call
 * This is a kludge. Use a create rpc but with the IFMT bits of the mode
 * set to specify the file type and the size field for rdev.
 */
/* ARGSUSED */
nfs_mknod(ndp, vap, cred)
	struct nameidata *ndp;
	struct ucred *cred;
	register struct vattr *vap;
{
	register struct nfsv2_sattr *sp;
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	u_long rdev;
	enum vtype vtyp;
	struct nfsnode *np;

	vtyp = vap->va_type;
	if (vap->va_type == VCHR || vap->va_type == VBLK)
		rdev = txdr_unsigned(vap->va_rdev);
	else if (vap->va_type == VFIFO) {
		vtyp = VCHR;
		rdev = txdr_unsigned(~0);
	}
	else {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		return (EOPNOTSUPP);
	}
	NFS_STATS(nfsstats.rpccnt[NFSPROC_CREATE]++);
	nfsm_reqhead(nfs_procids[NFSPROC_CREATE], ndp->ni_cred,
	NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(vtyp, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = rdev;
	/* or should these be VNOVAL ?? */
	txdr_time(&vap->va_atime, &sp->sa_atime);
	txdr_time(&vap->va_mtime, &sp->sa_mtime);
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_CREATE]);
        nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp, vap);
	nfsm_reqdone;
	np = VTONFS(ndp->ni_dvp);
	NP_LOCK(np);
	np->n_flag |= NMODIFIED;
	NP_UNLOCK(np);
	nfs_nput(ndp->ni_dvp);
	if ((error == 0) && (ndp->ni_vp))
		nfs_nput(ndp->ni_vp);
	return (error);
}


/*
 * nfs file create call
 */
nfs_create(ndp, vap)
	register struct nameidata *ndp;
	register struct vattr *vap;
{
	register struct nfsv2_sattr *sp;
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np;

	/* Only regular files and sockets should get here. */
	if (vap->va_type != VREG && vap->va_type != VSOCK) {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		return (EOPNOTSUPP);
	}
	NFS_STATS(nfsstats.rpccnt[NFSPROC_CREATE]++);
	nfsm_reqhead(nfs_procids[NFSPROC_CREATE], ndp->ni_cred,
	  NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(vap->va_type, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = txdr_unsigned(0);
	/* or should these be VNOVAL ?? */
	txdr_time(&vap->va_atime, &sp->sa_atime);
	txdr_time(&vap->va_mtime, &sp->sa_mtime);
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_CREATE]);
	if (vap->va_type != VSOCK)	/* Only need vap for socket pointer */
		vap = 0;
	nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp, vap);
	nfsm_reqdone;
	np = VTONFS(ndp->ni_dvp);
	NP_LOCK(np);
	np->n_flag |= NMODIFIED;
	NP_UNLOCK(np);
	nfs_nput(ndp->ni_dvp);
	return (error);
}

/*
 * nfs file remove call
 * To try and make nfs semantics closer to vfs semantics, a file that has
 * other references to the vnode is renamed instead of removed and then
 * removed later on the last close.
 * If v_usecount > 1
 *	  If a rename is not already in the works
 *	     call nfs_sillyrename() to set it up
 *     else
 *	  do the remove rpc
 */
nfs_remove(ndp)
	register struct nameidata *ndp;
{
	register struct vnode *vp = ndp->ni_vp;
	register struct nfsnode *np = VTONFS(ndp->ni_vp);
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct nfsnode *dp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	BM(VN_LOCK(vp));
	if (vp->v_usecount > 1) {
		BM(VN_UNLOCK(vp));
		BM(NP_LOCK(np));
		if (!np->n_sillyrename) {
			BM(NP_UNLOCK(np));
			error = nfs_sillyrename(ndp, REMOVE);
		} else
			BM(NP_UNLOCK(np));
	} else {
		BM(VN_UNLOCK(vp));
		NFS_STATS(nfsstats.rpccnt[NFSPROC_REMOVE]++);
		nfsm_reqhead(nfs_procids[NFSPROC_REMOVE], ndp->ni_cred,
		    NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen));
		nfsm_fhtom(ndp->ni_dvp);
		nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
		nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_REMOVE]);
		nfsm_reqdone;
		dp = VTONFS(ndp->ni_dvp);
		NP_LOCK(dp);
		dp->n_flag |= NMODIFIED;
		NP_UNLOCK(dp);
		/*
		 * Kludge City: If the first reply to the remove rpc is lost..
		 *   the reply to the retransmitted request will be ENOENT
		 *   since the file was in fact removed
		 *   Therefore, we cheat and return success.
		 */
		if (error == ENOENT)
			error = 0;
	}
	/*
	 * The attributes for the file are out of date, so we
	 * mark the cached attributes as invalid.
	 */
	 NP_LOCK(np);
	 np->n_attrstamp = 0;
	 NP_UNLOCK(np);

	/*
	 * here's an interesting problem.  While we were busy renaming
	 * the file, it's possible that someone (e.g. inode_pager) has
	 * released their hold and the count has decremented.  This will
	 * cause the following vrele() to call nfs_inactive, which will
	 * try to remove the file we've just went to all that trouble to
	 * rename.
	 *
	 * It might be good to check the ref. count a couple of times
	 * during the rename work, so we can avoid some extra traffic.
	 */
	if (ndp->ni_dvp == ndp->ni_vp)
		vrele(vp);
	else
		nfs_nput(ndp->ni_dvp);
	nfs_nput(vp);
	return (error);
}

/*
 * nfs file remove rpc called from nfs_inactive
 */
nfs_removeit(ndp)
	register struct nameidata *ndp;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct nfsnode *np;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_REMOVE]++);
	nfsm_reqhead(nfs_procids[NFSPROC_REMOVE], ndp->ni_cred,
		NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen));
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_REMOVE]);
	nfsm_reqdone;
	np = VTONFS(ndp->ni_dvp);
	NP_LOCK(np);
	np->n_flag |= NMODIFIED;
	NP_UNLOCK(np);
	return (error);
}

/*
 * nfs file rename call
 */
nfs_rename(sndp, tndp)
	register struct nameidata *sndp, *tndp;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0, err2;
	struct nfsnode *sdp = VTONFS(sndp->ni_dvp);
	struct nfsnode *tdp = VTONFS(tndp->ni_dvp);
	struct nfsnode *tnp;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_RENAME]++);
	nfsm_reqhead(nfs_procids[NFSPROC_RENAME], tndp->ni_cred,
		(NFSX_FH+NFSX_UNSIGNED)*2+nfsm_rndup(sndp->ni_dent.d_namlen)+
		nfsm_rndup(tndp->ni_dent.d_namlen)); /* or sndp->ni_cred?*/
	nfsm_fhtom(sndp->ni_dvp);
	nfsm_strtom(sndp->ni_dent.d_name,sndp->ni_dent.d_namlen,NFS_MAXNAMLEN);
	nfsm_fhtom(tndp->ni_dvp);
	nfsm_strtom(tndp->ni_dent.d_name,tndp->ni_dent.d_namlen,NFS_MAXNAMLEN);
	nfsm_request(sndp->ni_dvp, nonidempotent[NFSPROC_RENAME]);
	nfsm_reqdone;
	NP_LOCK(sdp);
	sdp->n_flag |= NMODIFIED;
	NP_UNLOCK(sdp);
	NP_LOCK(tdp);
	tdp->n_flag |= NMODIFIED;
	NP_UNLOCK(tdp);
	BM(VN_LOCK(sndp->ni_vp));
	if (sndp->ni_vp->v_type == VDIR) {
		BM(VN_UNLOCK(sndp->ni_vp));
		if (tndp->ni_vp != NULL) {
			BM(VN_LOCK(tndp->ni_vp));
			if (tndp->ni_vp->v_type == VDIR) {
				BM(VN_UNLOCK(tndp->ni_vp));
				cache_purge(tndp->ni_dvp);
			} else
				BM(VN_UNLOCK(tndp->ni_vp));
		}
		cache_purge(sndp->ni_dvp);
	} else
		BM(VN_UNLOCK(sndp->ni_vp));
	/*
	 * Invalidate cached attributes of target, if it was present.
	 */
	if (tndp->ni_vp != NULL) {
		tnp = VTONFS(tndp->ni_vp);
		NP_LOCK(tnp);
		tnp->n_attrstamp = 0;
		NP_UNLOCK(tnp);
	}
	VOP_ABORTOP(tndp, err2);
	vrele(tndp->ni_dvp);
	if (tndp->ni_vp)
		vrele(tndp->ni_vp);
	VOP_ABORTOP(sndp, err2);
	vrele(sndp->ni_dvp);
	vrele(sndp->ni_vp);

	/*
	 * Kludge: Map ENOENT --> 0 assuming that it is a reply to a
	 * retry.
	 */
	if (error == ENOENT)
		error = 0;	
	return (error);
}

/*
 * nfs file rename rpc called from above
 */
nfs_renameit(sndp, tndp)
	register struct nameidata *sndp, *tndp;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct nfsnode *sdp = VTONFS(sndp->ni_dvp);
	struct nfsnode *tdp = VTONFS(tndp->ni_dvp);
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_RENAME]++);
	nfsm_reqhead(nfs_procids[NFSPROC_RENAME], tndp->ni_cred,
		(NFSX_FH+NFSX_UNSIGNED)*2+nfsm_rndup(sndp->ni_dent.d_namlen)+
		nfsm_rndup(tndp->ni_dent.d_namlen)); /* or sndp->ni_cred?*/
	nfsm_fhtom(sndp->ni_dvp);
	nfsm_strtom(sndp->ni_dent.d_name,sndp->ni_dent.d_namlen,NFS_MAXNAMLEN);
	nfsm_fhtom(tndp->ni_dvp);
	nfsm_strtom(tndp->ni_dent.d_name,tndp->ni_dent.d_namlen,NFS_MAXNAMLEN);
	nfsm_request(sndp->ni_dvp, nonidempotent[NFSPROC_RENAME]);
	nfsm_reqdone;
	NP_LOCK(sdp);
	sdp->n_flag |= NMODIFIED;
	NP_UNLOCK(sdp);
	NP_LOCK(tdp);
	tdp->n_flag |= NMODIFIED;
	NP_UNLOCK(tdp);
	return (error);
}

/*
 * nfs hard link create call
 */
nfs_link(vp, ndp)
	register struct vnode *vp;
	register struct nameidata *ndp;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *np = VTONFS(vp);
	struct nfsnode *dp = VTONFS(ndp->ni_dvp);

	NFS_STATS(nfsstats.rpccnt[NFSPROC_LINK]++);
	nfsm_reqhead(nfs_procids[NFSPROC_LINK], ndp->ni_cred,
		NFSX_FH*2+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen));
	nfsm_fhtom(vp);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_request(vp, nonidempotent[NFSPROC_LINK]);
	nfsm_reqdone;
	/*
 	 * invalidate the cached attributes.  They're not correct
	 * anymore.  We could just ++ the v_nlink count, but let's just force
	 * a refresh on the next getattr request.
	 */
	NP_LOCK(np);
	np->n_attrstamp = 0;
	NP_UNLOCK(np);
	NP_LOCK(dp);
	dp->n_flag |= NMODIFIED;
	NP_UNLOCK(dp);
	nfs_nput(ndp->ni_dvp);
	/*
	 * Kludge: Map EEXIST --> 0 assuming that it is a reply to a
	 * retry.
	 */
	if (error == EEXIST)
		error = 0;	
	return (error);
}

/*
 * nfs symbolic link create call
 */
nfs_symlink(ndp, vap, nm)
	struct nameidata *ndp;
	struct vattr *vap;
	char *nm;		/* is this the path ?? */
{
	register struct nfsv2_sattr *sp;
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *dp = VTONFS(ndp->ni_dvp);

	NFS_STATS(nfsstats.rpccnt[NFSPROC_SYMLINK]++);
	nfsm_reqhead(nfs_procids[NFSPROC_SYMLINK], ndp->ni_cred,
	NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen)+NFSX_UNSIGNED);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_strtom(nm, strlen(nm), NFS_MAXPATHLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(VLNK, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = nfs_xdrneg1;
	txdr_time(&vap->va_atime, &sp->sa_atime);	/* or VNOVAL ?? */
	txdr_time(&vap->va_mtime, &sp->sa_mtime);	/* or VNOVAL ?? */
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_SYMLINK]);
	nfsm_reqdone;
	NP_LOCK(dp);
	dp->n_flag |= NMODIFIED;
	NP_UNLOCK(dp);
	nfs_nput(ndp->ni_dvp);
	/*
	 * Kludge: Map EEXIST --> 0 assuming that it is a reply to a
	 * retry.
	 */
	if (error == EEXIST)
		error = 0;	
	return (error);
}

/*
 * nfs make dir call
 */
nfs_mkdir(ndp, vap)
	register struct nameidata *ndp;
	struct vattr *vap;
{
	register struct nfsv2_sattr *sp;
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	register int len;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0, firsttry = 1;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *dp = VTONFS(ndp->ni_dvp);

	len = ndp->ni_dent.d_namlen;
	NFS_STATS(nfsstats.rpccnt[NFSPROC_MKDIR]++);
	nfsm_reqhead(nfs_procids[NFSPROC_MKDIR], ndp->ni_cred,
	  NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len)+NFSX_SATTR);
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, len, NFS_MAXNAMLEN);
	nfsm_build(sp, struct nfsv2_sattr *, NFSX_SATTR);
	sp->sa_mode = vtonfs_mode(VDIR, vap->va_mode);
	sp->sa_uid = txdr_unsigned(ndp->ni_cred->cr_uid);
	sp->sa_gid = txdr_unsigned(ndp->ni_cred->cr_gid);
	sp->sa_size = nfs_xdrneg1;
	txdr_time(&vap->va_atime, &sp->sa_atime);	/* or VNOVAL ?? */
	txdr_time(&vap->va_mtime, &sp->sa_mtime);	/* or VNOVAL ?? */
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_MKDIR]);
	nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp, (struct vattr *)0);
	nfsm_reqdone;
	NP_LOCK(dp);
	dp->n_flag |= NMODIFIED;
	NP_UNLOCK(dp);
	/*
	 * Kludge: Map EEXIST => 0 assuming that you have a reply to a retry
	 * if we can succeed in looking up the directory.
	 * "firsttry" is necessary since the macros may "goto nfsmout" which
	 * is above the if on errors. (Ugh)
	 */
	if (error == EEXIST && firsttry) {
		firsttry = 0;
		error = 0;
		NFS_STATS(nfsstats.rpccnt[NFSPROC_LOOKUP]++);
		ndp->ni_vp = NULL;
		nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], ndp->ni_cred,
		    NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
		nfsm_fhtom(ndp->ni_dvp);
		nfsm_strtom(ndp->ni_dent.d_name, len, NFS_MAXNAMLEN);
		nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_LOOKUP]);
		nfsm_mtofh(ndp->ni_dvp, ndp->ni_vp, (struct vattr *)0);
		BM(VN_LOCK(ndp->ni_vp));
		if (ndp->ni_vp->v_type != VDIR) {
			BM(VN_UNLOCK(ndp->ni_vp));
			vrele(ndp->ni_vp);
			error = EEXIST;
		} else
			BM(VN_UNLOCK(ndp->ni_vp));

		m_freem(mrep);
	}
	nfs_nput(ndp->ni_dvp);
	return (error);
}

/*
 * nfs remove directory call
 */
nfs_rmdir(ndp)
	register struct nameidata *ndp;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsnode *dp = VTONFS(ndp->ni_dvp);

	if (ndp->ni_dvp == ndp->ni_vp) {
		vrele(ndp->ni_dvp);
		nfs_nput(ndp->ni_dvp);
		return (EINVAL);
	}
	NFS_STATS(nfsstats.rpccnt[NFSPROC_RMDIR]++);
	nfsm_reqhead(nfs_procids[NFSPROC_RMDIR], ndp->ni_cred,
		NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(ndp->ni_dent.d_namlen));
	nfsm_fhtom(ndp->ni_dvp);
	nfsm_strtom(ndp->ni_dent.d_name, ndp->ni_dent.d_namlen, NFS_MAXNAMLEN);
	nfsm_request(ndp->ni_dvp, nonidempotent[NFSPROC_RMDIR]);
	nfsm_reqdone;
	NP_LOCK(dp);
	dp->n_flag |= NMODIFIED;
	NP_UNLOCK(dp);
	cache_purge(ndp->ni_dvp);
	cache_purge(ndp->ni_vp);
	nfs_reclaim(ndp->ni_vp);
	nfs_nput(ndp->ni_vp);
	nfs_nput(ndp->ni_dvp);

	/*
	 * Kludge: Map ENOENT --> 0 assuming that it is a reply to a
	 * retry.
	 */
	if (error == ENOENT)
		error = 0;	
	return (error);
}

/*
 * nfs readdir call
 * Although cookie is defined as opaque, I translate it to/from net byte
 * order so that it looks more sensible. This appears consistent with the
 * Ultrix implementation of NFS.
 */
nfs_readdir(vp, uiop, cred, eofflagp)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
	int *eofflagp;
{
	register struct nfsnode *np = VTONFS(vp);
	int tresid, error;
	struct vattr vattr;
	u_long direofoffset;
	long flag;
	time_t mtime;

	BM(VN_LOCK(vp));
	if (vp->v_type != VDIR) {
		BM(VN_UNLOCK(vp));
		return (EPERM);
	}
	BM(VN_UNLOCK(vp));
	/*
	 * First, check for hit on the EOF offset cache
	 */
	if (uiop->uio_offset != 0) {
		BM(NP_LOCK(np));
		direofoffset = np->n_direofoffset;
		flag = np->n_flag;
		BM(NP_UNLOCK(np));
		if (uiop->uio_offset == direofoffset &&
		    (flag & NMODIFIED) == 0) {
			if (nfs_getattr(vp, &vattr, cred) == 0) {
				BM(NP_LOCK(np));
				mtime = np->n_mtime;
				BM(NP_UNLOCK(np));
				if (mtime == vattr.va_mtime.tv_sec) {
					*eofflagp = 1;
					NFS_STATS(nfsstats.direofcache_hits++);
					return (0);
				}
			}
		}
	}

	/*
	 * Call nfs_bioread() to do the real work.
	 */
	tresid = uiop->uio_resid;
	error = nfs_bioread(vp, uiop, 0, cred);

	if (!error && uiop->uio_resid == tresid) {
		*eofflagp = 1;
		NFS_STATS(nfsstats.direofcache_misses++);
	} else
		*eofflagp = 0;
	return (error);
}

/*
 * Readdir rpc call.
 * Called from below the buffer cache by nfs_doio().
 */
nfs_readdirrpc(vp, uiop, cred)
	register struct vnode *vp;
	struct uio *uiop;
	struct ucred *cred;
{
	register long len;
	register struct dirent *dp;
	register u_long *p;
	register caddr_t cp;
	register long t1;
	long tlen, lastlen;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct mbuf *md2;
	caddr_t dpos2;
	int siz;
	int more_dirs = 1;
	off_t off, savoff;
	struct dirent *savdp;
	struct nfsmount *nmp;
	long tresid;
	struct nfsnode *np = VTONFS(vp);
	struct mount *mp;

	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	nmp = vfs_to_nfs(mp);
	tresid = uiop->uio_resid;
	/*
	 * Loop around doing readdir rpc's of size uio_resid or nm_rsize,
	 * whichever is smaller, truncated to a multiple of DIRBLKSIZ.
	 * The stopping criteria is EOF or buffer full.
	 */
	while (more_dirs && uiop->uio_resid >= DIRBLKSIZ) {
		NFS_STATS(nfsstats.rpccnt[NFSPROC_READDIR]++);
		nfsm_reqhead(nfs_procids[NFSPROC_READDIR], cred, xid);
		nfsm_fhtom(vp);
		nfsm_build(p, u_long *, 2*NFSX_UNSIGNED);
		*p++ = txdr_unsigned(uiop->uio_offset);
		*p = txdr_unsigned(((uiop->uio_resid > nmp->nm_rsize) ?
			nmp->nm_rsize : uiop->uio_resid) & ~(DIRBLKSIZ-1));
		nfsm_request(vp, nonidempotent[NFSPROC_READDIR]);
		siz = 0;
		nfsm_disect(p, u_long *, NFSX_UNSIGNED);
		more_dirs = fxdr_unsigned(int, *p);
	
		/* Save the position so that we can do nfsm_mtouio() later */
		dpos2 = dpos;
		md2 = md;
	
		/* loop thru the dir entries, doctoring them to 4bsd form */
		off = uiop->uio_offset;
		while (more_dirs && siz < uiop->uio_resid) {
			savoff = off;		/* Hold onto offset and dp */
			savdp = dp;
			nfsm_disecton(p, u_long *, 2*NFSX_UNSIGNED);
			dp = (struct dirent *)p;
			dp->d_fileno = fxdr_unsigned(u_long, *p);
			++p;
			len = fxdr_unsigned(int, *p);
			if (len <= 0 || len > NFS_MAXNAMLEN) {
				error = EBADRPC;
				m_freem(mrep);
				goto nfsmout;
			}
			dp->d_namlen = (u_short)len;
			nfsm_adv(len);		/* Point past name */
			tlen = nfsm_rndup(len);
			/*
			 * This should not be necessary, but some servers have
			 * broken XDR such that these bytes are not null filled.
			 */
			if (tlen != len) {
				*dpos = '\0';	/* Null-terminate */
				nfsm_adv(tlen - len);
				len = tlen;
			}
			nfsm_disecton(p, u_long *, 2*NFSX_UNSIGNED);
			off = fxdr_unsigned(off_t, *p);
			*p++ = 0;	/* Ensures null termination of name */
			more_dirs = fxdr_unsigned(int, *p);
			dp->d_reclen = len+4*NFSX_UNSIGNED;
			siz += dp->d_reclen;
		}
		/*
		 * If at end of rpc data, get the eof boolean
		 */
		if (!more_dirs) {
			nfsm_disecton(p, u_long *, NFSX_UNSIGNED);
			more_dirs = (fxdr_unsigned(int, *p) == 0);

			/*
			 * If at EOF, cache directory offset
			 */
			if (!more_dirs) {
				NP_LOCK(np);
				np->n_direofoffset = off;
				NP_UNLOCK(np);
			}
		}
		/*
		 * If there is too much to fit in the data buffer, use savoff and
		 * savdp to trim off the last record.
		 * --> we are not at eof
		 */
		if (siz > uiop->uio_resid) {
			off = savoff;
			siz -= dp->d_reclen;
			dp = savdp;
			more_dirs = 0;	/* Paranoia */
		}
		if (siz > 0) {
			lastlen = dp->d_reclen;
			md = md2;
			dpos = dpos2;
			nfsm_mtouio(uiop, siz);
			uiop->uio_offset = off;
		} else
			more_dirs = 0;	/* Ugh, never happens, but in case.. */
		m_freem(mrep);
	}
	/*
	 * Fill last record, iff any, out to a multiple of DIRBLKSIZ
	 * by increasing d_reclen for the last record.
	 */
	if (uiop->uio_resid < tresid) {
		len = uiop->uio_resid & (DIRBLKSIZ - 1);
		if (len > 0) {
			dp = (struct dirent *)
				(uiop->uio_iov->iov_base - lastlen);
			dp->d_reclen += len;
			uiop->uio_iov->iov_base += len;
			uiop->uio_iov->iov_len -= len;
			uiop->uio_resid -= len;
		}
	}
nfsmout:
	return (error);
}

/*
 * nfs statfs call
 * (Actually a vfsop, not a vnode op)
 */
nfs_statfs(mp)
	struct mount *mp;
{
	register struct vnode *vp;
	register struct statfs *sbp;
	register struct nfsv2_statfs *sfp;
	register caddr_t cp;
	register long t1;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	struct nfsmount *nmp;
	struct ucred *cred;
	struct nfsnode *np;

	nmp = vfs_to_nfs(mp);
	if (error = nfs_nget(mp, &nmp->nm_fh, &np))
		return (error);
	vp = NFSTOV(np);
	NFS_STATS(nfsstats.rpccnt[NFSPROC_STATFS]++);
	cred = crget();
	cred->cr_ngroups = 1;
	nfsm_reqhead(nfs_procids[NFSPROC_STATFS], cred, NFSX_FH);
	nfsm_fhtom(vp);
	nfsm_request(vp, nonidempotent[NFSPROC_STATFS]);
	nfsm_disect(sfp, struct nfsv2_statfs *, NFSX_STATFS);
	MOUNT_LOCK(mp);
	sbp = &mp->m_stat;
	sbp->f_type = MOUNT_NFS;
	sbp->f_flags = nmp->nm_flag;
	sbp->f_bsize = fxdr_unsigned(long, sfp->sf_tsize);
	sbp->f_fsize = fxdr_unsigned(long, sfp->sf_bsize);
	sbp->f_blocks = fxdr_unsigned(long, sfp->sf_blocks);
	sbp->f_bfree = fxdr_unsigned(long, sfp->sf_bfree);
	sbp->f_bavail = fxdr_unsigned(long, sfp->sf_bavail);
	sbp->f_files = 0;
	sbp->f_ffree = 0;
	MOUNT_UNLOCK(mp);
	nfsm_reqdone;
	nfs_nput(vp);
	crfree(cred);
	return (error);
}

static char hextoasc[] = "0123456789abcdef";

/*
 * Silly rename. To make the NFS filesystem that is stateless look a little
 * more like the "ufs" a remove of an active vnode is translated to a rename
 * to a funny looking filename that is removed by nfs_inactive on the
 * nfsnode. There is the potential for another process on a different client
 * to create the same funny name between the nfs_lookitup() fails and the
 * nfs_rename() completes, but...
 */
nfs_sillyrename(ndp, flag)
	register struct nameidata *ndp;
	int flag;
{
	register struct nfsnode *np;
	register struct sillyrename *sp;
	register struct nameidata *tndp;
	int error;
	short pid;
	int 	counter;	/* 0-9999 for silly name construction */
	char	*insert;	/* insert point for 0-9999 in silly name */ 

	np = VTONFS(ndp->ni_dvp);
	cache_purge(ndp->ni_dvp);
#if	MACH
	ZALLOC(sillyrename_zone, sp, struct sillyrename *);
#else
	MALLOC(sp, struct sillyrename *, sizeof (struct sillyrename),
		M_TEMP, M_WAITOK);
#endif	/* MACH */
	sp->s_flag = flag;
	bcopy((caddr_t)&np->n_fh, (caddr_t)&sp->s_fh, NFSX_FH);
	np = VTONFS(ndp->ni_vp);
	tndp = &sp->s_namei;
#if	MACH
	/*
	 * This structure is required because the ni_cred field of ndp is
	 * shared and in the utask structure, and we don't want to nuke it.
	 * The fields of the ni_utnd struct other than the cred are not
	 * needed here (rdir, cdir).  We need to dynamically allocate these
	 * pieces (above) because they can hang around long after the process
	 * is gone.
	 */
	tndp->ni_utnd = &sp->s_utnd;
#endif	/* MACH */
	tndp->ni_cred = crdup(ndp->ni_cred);

	/* Fudge together a funny name */
	pid = u.u_procp->p_pid;
	bcopy(".nfsxxxx0000", tndp->ni_dent.d_name, 13);
	tndp->ni_dent.d_namlen = 12;
	tndp->ni_dent.d_name[7] = hextoasc[pid & 0xf];
	tndp->ni_dent.d_name[6] = hextoasc[(pid >> 4) & 0xf];
	tndp->ni_dent.d_name[5] = hextoasc[(pid >> 8) & 0xf];
	tndp->ni_dent.d_name[4] = hextoasc[(pid >> 12) & 0xf];
	insert = &(tndp->ni_dent.d_name[8]);
	counter = 0;

	/* Try lookitups until we get one that isn't there */
	while (nfs_lookitup(ndp->ni_dvp, tndp, (struct nfsnode *)0) == 0) {
		counter++;
		if (counter > 9999) {
			error = EINVAL;
			goto bad;
		}
		sprintf(insert, "%d", counter);
	}
	if (error = nfs_renameit(ndp, tndp))
		goto bad;
	(void) nfs_lookitup(ndp->ni_dvp, tndp, np);
	NP_LOCK(np);
	np->n_sillyrename = sp;
	NP_UNLOCK(np);
	return (0);
bad:
	crfree(tndp->ni_cred);
#if	MACH
	ZFREE(sillyrename_zone, sp);
#else
	free((caddr_t)sp, M_TEMP);
#endif	/* MACH */
	return (error);
}

/*
 * Look up a file name for silly rename stuff.
 * Just like nfs_lookup() except that it doesn't load returned values
 * into the nfsnode table.
 * If fhp != NULL it copies the returned file handle out
 */
nfs_lookitup(vp, ndp, np)
	register struct vnode *vp;
	register struct nameidata *ndp;
	struct nfsnode *np;
{
	register u_long *p;
	register caddr_t cp;
	register long t1, t2;
	caddr_t bpos, dpos, cp2;
	u_long xid;
	int error = 0;
	struct mbuf *mreq, *mrep, *md, *mb, *mb2;
	long len;

	NFS_STATS(nfsstats.rpccnt[NFSPROC_LOOKUP]++);
	ndp->ni_dvp = vp;
	ndp->ni_vp = NULL;
	len = ndp->ni_dent.d_namlen;
	nfsm_reqhead(nfs_procids[NFSPROC_LOOKUP], ndp->ni_cred, NFSX_FH+NFSX_UNSIGNED+nfsm_rndup(len));
	nfsm_fhtom(vp);
	nfsm_strtom(ndp->ni_dent.d_name, len, NFS_MAXNAMLEN);
	nfsm_request(vp, nonidempotent[NFSPROC_LOOKUP]);
	if (np != NULL) {
		nfsm_disect(cp, caddr_t, NFSX_FH);
		NP_LOCK(np);
		bcopy(cp, (caddr_t)&np->n_fh, NFSX_FH);
		NP_UNLOCK(np);
	}
	nfsm_reqdone;
	return (error);
}

/*
 * Kludge City..
 * - make nfs_bmap() essentially a no-op that does no translation
 * - do nfs_strategy() by faking physical I/O with nfs_readit/nfs_writeit
 *   after mapping the physical addresses into Kernel Virtual space in the
 *   nfsiobuf area.
 *   (Maybe I could use the process's page mapping, but I was concerned that
 *    Kernel Write might not be enabled and also figured copyout() would do
 *    a lot more work than bcopy() and also it currently happens in the
 *    context of the swapper process (2).
 */
nfs_bmap(vp, bn, vpp, bnp)
	struct vnode *vp;
	daddr_t bn;
	struct vnode **vpp;
	daddr_t *bnp;
{
	struct mount *mp;

	BM(VN_LOCK(vp));
	mp = vp->v_mount;
	BM(VN_UNLOCK(vp));
	if (vpp != NULL)
		*vpp = vp;
	if (bnp != NULL) {
		BM(MOUNT_LOCK(mp));
		*bnp = bn * btodg(mp->m_stat.f_bsize);
		BM(MOUNT_UNLOCK(mp));
	}
	return (0);
}

/*
 * Strategy routine for phys. i/o
 * If the biod's are running, queue a request
 * otherwise just call nfs_doio() to get it done
 */
nfs_strategy(bp)
	register struct buf *bp;
{
	register struct buf *dp;
	register int i;
	struct proc *rp;
	int error = 0;
	int fnd = 0;

	/*
	 * If the i/o is synchronous, we handle the request ourselves
	 * and avoid the context switch involved with running the i/o
	 * daemon.
	 */
	if ((bp->b_flags & B_ASYNC) == 0) {
		return (nfs_doio(bp));
	}
	LASSERT(BUF_IS_LOCKED(bp));
	/*
	 * If an i/o daemon is waiting, queue the request, wake it up,
	 * and wait for completion; otherwise just do it ourselves.
	 */
	ASYNCD_LOCK();
	if (asyncdaemon_requests >= nfs_asyncdaemons) {
		ASYNCD_UNLOCK();
		/*
		 * This was asynchronous, but we decided to do it
		 * ourselves. Reclaim ownership of the buf.
		 */
		BUF_ACCEPT(bp);
		return (nfs_doio(bp));
	}
	dp = &nfs_bqueue;
	if (dp->b_actf == NULL) {
		dp->b_actl = bp;
		bp->b_actf = dp;
	} else {
		dp->b_actf->b_actl = bp;
		bp->b_actf = dp->b_actf;
	}
	dp->b_actf = bp;
	bp->b_actl = dp;
	++asyncdaemon_requests;
	ASYNCD_UNLOCK();
	thread_wakeup_one((int)&asyncdaemon_requests);
	return (0);
}

/*
 * Fun and games with i/o
 * Essentially play ubasetup() and disk interrupt service routine by
 * mapping the data buffer into kernel virtual space and doing the
 * nfs read or write rpc's from it.
 * If the biod's are not running, this is just called from nfs_strategy(),
 * otherwise it is called by the biod's to do what would normally be
 * partially disk interrupt driven.
 */
nfs_doio(bp)
	register struct buf *bp;
{
	register struct pte *pte, *ppte;
	register caddr_t vaddr;
	register struct uio *uiop;
	register struct vnode *vp;
	struct nfsnode *np;
	struct ucred *cr;
	int npf, npf2;
	int reg;
	caddr_t vbase;
	unsigned v;
	struct proc *rp;
	int o, error;
	struct uio uio;
	struct iovec io;
	enum vtype type;

	LASSERT(BUF_LOCK_HOLDER(bp));
	vp = bp->b_vp;
	uiop = &uio;
	uiop->uio_iov = &io;
	uiop->uio_iovcnt = 1;
	uiop->uio_segflg = UIO_SYSSPACE;

	/*
	 * For phys i/o, map the b_addr into kernel virtual space using
	 * the Nfsiomap pte's
	 * Also, add a temporary b_rcred for reading using the process's uid
	 * and a guess at a group
	 */
	if (bp->b_flags & B_PHYS) {
#if	MACH
		panic("nfs_doio -- physmem");
#else	/* MACH */
		bp->b_rcred = cr = crget();
		rp = (bp->b_flags & B_DIRTY) ? &proc[2] : bp->b_proc;
		cr->cr_uid = rp->p_uid;
		cr->cr_gid = 0;		/* Anything ?? */
		cr->cr_ngroups = 1;
		o = (int)bp->b_un.b_addr & PGOFSET;
		npf2 = npf = btoc(bp->b_bcount + o);

		/*
		 * Get some mapping page table entries
		 */
		while ((reg = rmalloc(nfsmap, (long)npf)) == 0) {
			nfsmap_want++;
			sleep((caddr_t)&nfsmap_want, PZERO-1);
		}
		reg--;
		if (bp->b_flags & B_PAGET)
			pte = &Usrptmap[btokmx((struct pte *)bp->b_un.b_addr)];
		else {
			v = btop(bp->b_un.b_addr);
			if (bp->b_flags & B_UAREA)
				pte = &rp->p_addr[v];
			else
				pte = vtopte(rp, v);
		}

		/*
		 * Play vmaccess() but with the Nfsiomap page table
		 */
		ppte = &Nfsiomap[reg];
		vbase = vaddr = &nfsiobuf[reg*NBPG];
		while (npf != 0) {
			mapin(ppte, (u_int)vaddr, pte->pg_pfnum, (int)(PG_V|PG_KW));
#if defined(tahoe)
			mtpr(P1DC, vaddr);
#endif
			ppte++;
			pte++;
			vaddr += NBPG;
			--npf;
		}

		/*
		 * And do the i/o rpc
		 */
		io.iov_base = vbase+o;
		io.iov_len = uiop->uio_resid = bp->b_bcount;
		uiop->uio_offset = dgtob(bp->b_blkno);
		if (bp->b_flags & B_READ) {
			uiop->uio_rw = UIO_READ;
			NFS_STATS(nfsstats.read_physios++);
			bp->b_error = error = nfs_readrpc(vp, uiop, bp->b_rcred);
		} else {
			uiop->uio_rw = UIO_WRITE;
			NFS_STATS(nfsstats.write_physios++);
			bp->b_error = error = nfs_writerpc(vp, uiop, bp->b_wcred);
		}

		/*
		 * Finally, release pte's used by physical i/o
		 */
		crfree(cr);
		rmfree(nfsmap, (long)npf2, (long)++reg);
		if (nfsmap_want) {
			nfsmap_want = 0;
			wakeup((caddr_t)&nfsmap_want);
		}
#endif	/* MACH */
	} else {
		if (bp->b_flags & B_READ) {
			io.iov_len = uiop->uio_resid = bp->b_bcount;
			io.iov_base = bp->b_un.b_addr;
			uiop->uio_rw = UIO_READ;
			BM(VN_LOCK(vp));
			type = vp->v_type;
			BM(VN_UNLOCK(vp));
			switch (type) {
			case VREG:
				uiop->uio_offset = dgtob(bp->b_blkno);
				NFS_STATS(nfsstats.read_bios++);
				error = nfs_readrpc(vp, uiop, bp->b_rcred);
				break;
			case VLNK:
				uiop->uio_offset = 0;
				NFS_STATS(nfsstats.readlink_bios++);
				error = nfs_readlinkrpc(vp, uiop, bp->b_rcred);
				break;
			case VDIR:
				uiop->uio_offset = bp->b_lblkno;
				NFS_STATS(nfsstats.readdir_bios++);
				error = nfs_readdirrpc(vp, uiop, bp->b_rcred);
				/*
				 * Save offset cookie in b_blkno.
				 */
				bp->b_blkno = uiop->uio_offset;
				break;
			};
			bp->b_error = error;
		} else {
			io.iov_len = uiop->uio_resid = bp->b_dirtyend
				- bp->b_dirtyoff;
			uiop->uio_offset = dgtob(bp->b_blkno) + bp->b_dirtyoff;
			io.iov_base = bp->b_un.b_addr + bp->b_dirtyoff;
			uiop->uio_rw = UIO_WRITE;
			NFS_STATS(nfsstats.write_bios++);
			bp->b_error = error = nfs_writerpc(vp, uiop, bp->b_wcred);
			if (error) {
				np = VTONFS(vp);
				NP_LOCK(np);
				np->n_error = error;
				np->n_flag |= NWRITEERR;
				NP_UNLOCK(np);
			}
			bp->b_dirtyoff = bp->b_dirtyend = 0;
		}
	}
	if (error)
		bp->b_flags |= B_ERROR;
	bp->b_resid = uiop->uio_resid;
#if MACH_LDEBUG
	if (bp->b_flags&B_ASYNC) {
		/*
		 * biodone expects to take ownership of async I/O
		 * buffers, so we give it away so that biodone can
		 * take it back.
		 */
		BUF_GIVE_AWAY(bp);
	}
#endif
	biodone(bp);
	return (error);
}

/*
 * Flush all the blocks associated with a vnode.
 * 	Walk through the buffer pool and push any dirty pages
 *	associated with the vnode.
 */
/* ARGSUSED */
nfs_fsync(vp, fflags, cred, waitfor)
	register struct vnode *vp;
	int fflags;
	struct ucred *cred;
	int waitfor;
{
	register struct nfsnode *np = VTONFS(vp);
	int error = 0;

	NP_LOCK(np);
	if (np->n_flag & NMODIFIED) {
		np->n_flag &= ~NMODIFIED;
		NP_UNLOCK(np);
		vflushbuf(vp, waitfor == MNT_WAIT ? B_SYNC : 0);
	} else
		NP_UNLOCK(np);
	BM(NP_LOCK(np));
	if (!error && (np->n_flag & NWRITEERR))
		error = np->n_error;
	BM(NP_UNLOCK(np));
	return (error);
}

/*
 * Seek on a file
 *
 * Negative offsets are invalid.
 */
/* ARGSUSED */
nfs_seek(vp, oldoff, newoff, cred)
	struct vnode *vp;
	off_t oldoff, newoff;
	struct ucred *cred;
{
	if ((int) newoff < 0)
		return(EINVAL);
	else
		return(0);
}

nfs_page_read(vp, uio, cred)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
{
	int error;

 	error = nfs_read(vp, uio, 0, cred);
	if (error) {
		printf("error %d on pagein (nfs_read)\n", error);
		error = EIO;
	}
	return(error);
}

nfs_page_write(vp, uio, cred, pager, offset)
	struct vnode *vp;
	struct uio *uio;
	struct ucred *cred;
	memory_object_t pager;
	vm_offset_t offset;
{
	int error;

	error = nfs_write(vp, uio, 0, cred);
	if (error) {
		printf("error %d on pageout (nfs_write)\n", error);
		error = EIO;
	}
	return(error);
}

/*
 * getattr wrapper for fifos.
 */
nfsfifo_getattr(vp, vap, cred)
	struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	int error;

	/*
	 * Get most attributes from the nfsnode, rest
	 * from the fifo.
	 */
	if (error = nfs_getattr(vp, vap, cred))
		return(error);
	return(fifo_getattr(vp, vap, cred));
}

/*
 * Print out the contents of an nfsnode.
 */
nfs_print(vp)
	struct vnode *vp;
{
	register struct nfsnode *np = VTONFS(vp);
	long fileid, fsid;

	BM(NP_LOCK(np));
	fileid = np->n_vattr.va_fileid;
	fsid = np->n_vattr.va_fsid;
	BM(NP_UNLOCK(np));

	printf("tag VT_NFS, fileid %d fsid 0x%x\n",
		fileid, fsid);
}


#ifdef	PFS
/*
 * Unsupported operation.
 */
nfs_noop()
{
	return(EFSNOTSUPP);
}
#endif	PFS

