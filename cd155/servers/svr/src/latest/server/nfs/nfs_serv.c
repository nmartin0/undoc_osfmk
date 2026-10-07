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
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: nfs_serv.c,v $
 * Revision 1.7  1994/11/18  20:37:01  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:16:05  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:29:21  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/01  20:12:19  cfj
 * Merge with T9.
 *
 * Revision 2.9  1993/04/14  14:26:41  durriya
 * 	add support for FAST_PATH in nfsrv_read
 * 	[93/04/14            durriya]
 *
 * Revision 2.8  93/04/05  18:04:31  durriya
 *	fix bug in nfsrv_create when file already exists. It needs to extract 
 *	the attributes from the client sent packet, before setting them for 
 *	regular files.
 * 	[93/04/01            durriya]
 * 
 * Revision 2.7  93/02/11  17:14:29  durriya
 * 	check for permissioons before truncating to size 0 in 
 * 	nfsrv_create                                       - (durriya)
 * 
 * Revision 2.6  93/01/06  10:38:14  loverso
 * 	Fix notice.
 *
 * Revision 1.2.8.2  1993/04/01  19:56:24  cfj
 * Fix bug #4665
 *
 * Revision 1.3  1993/03/11  21:09:51  wunder
 * Added OSF change to check for permissions before truncating to size 0
 * in procedure nfsrv_create().
 *
 * Revision 1.2  1992/11/30  22:32:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:28  cfj
 * Bump major revision number.
 *
 * Revision 2.5  1992/03/09  12:14:03  durriya
 * 	Revision 3.2  91/12/18  17:17:15  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.4  92/01/05  20:03:10  roy
 * 	1991/10/14  19:56:44  noemi
 * 	Changed HASBUF to HASCOMPBUF in the call to namei.
 * 
 * Revision 2.3  91/11/26  13:30:51  rabii
 * 	Added extra argument to VOP_MKNOD
 * 
 * Revision 2.2  91/08/31  13:51:27  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:40:06  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.16.4.2  91/02/19  15:24:57  tmt
 * 	Encode 4-byte rounded rpc when short, non-rounded read requested.
 * 	Fix nfsm_reply in nfs_readdir.
 * 	[91/02/16  16:07:41  tmt]
 * 
 * Revision 1.16  90/10/07  14:39:06  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:27  gm]
 * 
 * Revision 1.15  90/09/23  15:56:21  devrcs
* 	Back out nfsrv_symlink code that tries to use client modes.  It caused
 * 	symlinks to fail.
 * 	[90/09/06  17:39:15  gmf]
 * 
 * Revision 1.14  90/08/24  12:15:37  devrcs
 * 	Use client's mode information when creating a symlink.
 * 	[90/08/19  00:59:14  nags]
 * 
 * Revision 1.13  90/08/09  13:27:05  devrcs
 * 	Fix nfsrv_rename -- "if (error)" should have been "if (!error)"
 * 	[90/08/02  13:59:11  gmf]
 * 
 * Revision 1.12  90/07/27  09:04:26  devrcs
 * 	NFS parallelization, keep namei buf, nfsrv_readdir fix.
 * 	[90/07/20  17:03:18  nags]
 * 
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:55:43  seiden]
 * 
 * Revision 1.11  90/07/17  11:37:20  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:55:40  seiden]
 * 
 * 	More changes for gcc.
 * 	[90/07/07  22:58:44  gm]
 * 
 * Revision 1.10  90/07/05  23:13:37  devrcs
 * 	Fixed problem in nfsrv_readdir in which a request of exactly 8192 bytes
 * 	would be rounded up to 8192+DIRBLKSIZ (512).  This could either cause a
 * 	TLB miss or scribble over someone else's memory.
 * 	[90/06/29  17:39:34  gmf]
 * 
 * Revision 1.9  90/06/29  13:37:20  devrcs
 * 	Parallelized by nags.
 * 	[90/06/26  11:23:48  nags]
 * 
 * 	Add FIFOs to the list of file types allowed to be opened for
 * 	write on read-only file systems.
 * 	Also extended nfs_type array to map VFIFO to NFCHR.
 * 	[90/06/18  00:17:08  ers]
 * 
 * 	Move ++ side effects out of fxdr macro - for inline.
 * 	[90/06/14  11:54:46  tmt]
 * 
 * 	nags merge
 * 	[90/06/12  21:35:44  nags]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:45:27  seiden]
 * 
 * 	Compressed history (reverse chronology):
 * 	Changed abortops semantics 				gmf@osf.org
 * 	added support for special file creation			gmf@osf.org
 * 	Serialized for OSF/1.					nags@encore.com
 * 	Fix from Guelph when out of clusters in readlink.	tmt@osf.org
 * 	Fix from Guelph for readdir eofflag clear on overflow.	tmt@osf.org
 * 	Eof fix, readdir, ndinit, m_fsid ==> m_stat.f_fsid.	gmf@osf.org
 * 	Eliminate #ifdef VOP_PRINT (backwards compatibility).	gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				tmt@osf.org
 * 
 * $EndLog$
 */
/* @(#)nfs_serv.c	2.1 16:12:49 4/20/90 SecureWare */
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
 *	@(#)nfs_serv.c	7.10 (Berkeley) 11/22/89
 */

/*
 * nfs version 2 server calls to vnode ops
 * - these routines generally have 3 phases
 *   1 - break down and validate rpc request in mbuf list
 *   2 - do the vnode ops for the request
 *       (surprisingly ?? many are very similar to syscalls in vfs_syscalls.c)
 *   3 - build the rpc reply in an mbuf list
 *   nb:
 *	- do not mix the phases, since the nfsm_?? macros can return failures
 *	  on mbuf exhaustion or similar and do not do any vrele() or vput()'s
 *
 *      - the nfsm_reply() macro generates an nfs rpc reply with the nfs
 *	error number iff error != 0 whereas
 *       nfsm_srverr simply drops the mbufs and gives up
 *       (==> nfsm_srverr implies an error here at the server, usually mbuf
 *	  exhaustion)
 */
#if     MACH
#include <mach_nbc.h>
#endif

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <sys/time.h>
#include <sys/param.h>
#include <sys/mount.h>
#if	MACH
#include <kern/zalloc.h>
#include <kern/mfs.h>
#include <builtin/inode_pager.h>
#else
#include <sys/malloc.h>
#endif
#include <sys/mbuf.h>
#include <sys/file.h>
#include <sys/user.h>
#include <ufs/dir.h>
#include <sys/vnode.h>
#include <sys/uio.h>
#include <sys/ucred.h>
#include <sys/namei.h>
#include <sys/errno.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#include <ufs/inode.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>

/* Global vars */
extern u_long nfs_procids[NFS_NPROCS];
extern u_long nfs_xdrneg1;
extern u_long nfs_false, nfs_true;
nfstype nfs_type[VFIFO+1]={ NFNON, NFREG, NFDIR, NFBLK, NFCHR, NFLNK, NFNON,
			NFCHR};
/*
 * nfs getattr service
 */
nfsrv_getattr(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct nfsv2_fattr *fp;
	struct vattr va;
	register struct vattr *vap = &va;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	VOP_GETATTR(vp, vap, cred, error);
	vrele(vp);
	nfsm_reply(NFSX_FATTR);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	nfsm_srvdone;
}

/*
 * nfs setattr service
 */
nfsrv_setattr(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct vattr va;
	register struct vattr *vap = &va;
	register struct nfsv2_sattr *sp;
	register struct nfsv2_fattr *fp;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_disect(sp, struct nfsv2_sattr *, NFSX_SATTR);
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	if (error = nfsrv_access(vp, VWRITE, cred))
		goto out;
	vattr_null(vap);
	/*
	 * Nah nah nah nah na nah
	 * There is a bug in the Sun client that puts 0xffff in the mode
	 * field of sattr when it should put in 0xffffffff. The u_short
	 * doesn't sign extend.
	 * --> check the low order 2 bytes for 0xffff
	 */
	if ((fxdr_unsigned(int, sp->sa_mode) & 0xffff) != 0xffff)
		vap->va_mode = nfstov_mode(sp->sa_mode);
	if (sp->sa_uid != nfs_xdrneg1)
		vap->va_uid = fxdr_unsigned(uid_t, sp->sa_uid);
	if (sp->sa_gid != nfs_xdrneg1)
		vap->va_gid = fxdr_unsigned(gid_t, sp->sa_gid);
	if (sp->sa_size != nfs_xdrneg1)
		vap->va_size = fxdr_unsigned(u_long, sp->sa_size);
	/*
	 * The usec field of sa_atime is overloaded with the va_flags field
	 * for 4.4BSD clients. Hopefully other clients always set both the
	 * sec and usec fields to -1 when not setting the atime.
	 */
	if (sp->sa_atime.tv_sec != nfs_xdrneg1) {
		vap->va_atime.tv_sec = fxdr_unsigned(long, sp->sa_atime.tv_sec);
		vap->va_atime.tv_usec = 0;
	}
	if (sp->sa_atime.tv_usec != nfs_xdrneg1)
		vap->va_flags = fxdr_unsigned(u_long, sp->sa_atime.tv_usec);
	if (sp->sa_mtime.tv_sec != nfs_xdrneg1)
		fxdr_time(&sp->sa_mtime, &vap->va_mtime);
	VOP_SETATTR(vp, vap, cred, error);
	if (error) {
		vrele(vp);
		nfsm_reply(0);
	}
	VOP_GETATTR(vp, vap, cred, error);
out:
	vrele(vp);
	nfsm_reply(NFSX_FATTR);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	nfsm_srvdone;
}

/*
 * nfs lookup rpc
 */
nfsrv_lookup(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct nfsv2_fattr *fp;
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	register caddr_t cp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	long len;
	struct vattr va, *vap = &va;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = LOOKUP;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		nfsm_reply(0);
	vp = ndp->ni_vp;
	bzero((caddr_t)fhp, sizeof(nfh));
	/*
	 * N.B.: there's a slim chance that this vnode has been vgone'd,
	 * in which case v_mount points to dead_mount.  If this is the
	 * case, then we can depend on an error from the VFS_VPTOFH call.
	 */
	fhp->fh_fsid = vp->v_mount->m_stat.f_fsid;
	VFS_VPTOFH(vp, &fhp->fh_fid, error);
	if (error) {
		vrele(vp);
		nfsm_reply(0);
	}
	VOP_GETATTR(vp, vap, cred, error);
	vrele(vp);
	nfsm_reply(NFSX_FH+NFSX_FATTR);
	nfsm_srvfhtom(fhp);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	nfsm_srvdone;
}

/*
 * nfs readlink service
 */
nfsrv_readlink(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct iovec iv[NFS_MAXPATHLEN/MLEN+1];
	register struct iovec *ivp = iv;
	register struct mbuf *mp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mp2, *mp3, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	struct uio io, *uiop = &io;
	int i, tlen, len;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	len = 0;
	i = 0;
	while (len < NFS_MAXPATHLEN) {
		MGET(mp, M_WAIT, MT_DATA);
		NFSMCLGET(mp, M_WAIT);
		mp->m_len = NFSMSIZ(mp);
		if (len == 0)
			mp3 = mp2 = mp;
		else {
			mp2->m_next = mp;
			mp2 = mp;
		}
		if ((len+mp->m_len) > NFS_MAXPATHLEN) {
			mp->m_len = NFS_MAXPATHLEN-len;
			len = NFS_MAXPATHLEN;
		} else
			len += mp->m_len;
		ivp->iov_base = mtod(mp, caddr_t);
		ivp->iov_len = mp->m_len;
		i++;
		ivp++;
	}
	uiop->uio_iov = iv;
	uiop->uio_iovcnt = i;
	uiop->uio_offset = 0;
	uiop->uio_resid = len;
	uiop->uio_rw = UIO_READ;
	uiop->uio_segflg = UIO_SYSSPACE;
	if (error = nfsrv_fhtovp(fhp, &vp, cred)) {
		m_freem(mp3);
		nfsm_reply(0);
	}
	BM(VN_LOCK(vp));
	if (vp->v_type == VLNK) {
		BM(VN_UNLOCK(vp));
		VOP_READLINK(vp, uiop, cred, error);
	} else {
		BM(VN_UNLOCK(vp));
		error = EINVAL;
	}
out:
	vrele(vp);
	if (error)
		m_freem(mp3);
	nfsm_reply(NFSX_UNSIGNED);
	if (uiop->uio_resid > 0) {
		len -= uiop->uio_resid;
		tlen = nfsm_rndup(len);
		nfsm_adj(mp3, NFS_MAXPATHLEN-tlen, tlen-len);
	}
	nfsm_build(p, u_long *, NFSX_UNSIGNED);
	*p = txdr_unsigned(len);
	mb->m_next = mp3;
	nfsm_srvdone;
}

/*
 * nfs read service
 */
nfsrv_read(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct iovec iv[NFS_MAXDATA/MCLBYTES+1];
	register struct iovec *ivp = iv;
	register struct mbuf *mp;
	register struct nfsv2_fattr *fp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	struct mbuf *mp2, *mp3;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	struct uio io, *uiop = &io;
	struct vattr va, *vap = &va;
	int i, tlen, cnt, len, nio, left;
	off_t off;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_disect(p, u_long *, NFSX_UNSIGNED);
	off = fxdr_unsigned(off_t, *p);
	nfsm_srvstrsiz(cnt, NFS_MAXDATA);
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	if (error = nfsrv_access(vp, VREAD | VEXEC, cred)) {
		vrele(vp);
		nfsm_reply(0);
	}
#ifdef OSF1_ADFS
	if (VIO_IS_FASTPATH(vp)) {
		extern void free_msgmbuf();
		uiop->uio_iov = ivp;
		uiop->uio_iovcnt = 1;
		uiop->uio_offset = off;
		uiop->uio_resid = cnt;
		uiop->uio_rw = UIO_READ;
		uiop->uio_segflg = UIO_SYSSPACE;
		/* 
		 * set the iov_base to NULL so that the VOP_DATAIN layer
		 * will assign a buffer into which the data is read
		 */
		ivp->iov_base  = (caddr_t)NULL;
		ivp->iov_len = cnt;
		VOP_DATAIN(vp, uiop, 0, cred, error);

		/* 
		 * set cnt to the amount that was actually read rounded upto 
		 * a page-size. The VOP_DATAIN interface gaurantees that we
		 * will receive multiples of vm_page_size with 
		 * (request_size - uio_resid) being the actual number of 
		 * valid bytes. 'cnt' is used to indicating the size of the
		 * mbuf data and 'len' is used to indicate the actual number 
		 * of valid bytes of data. Both 'cnt' and 'len' are used 
		 * later in the code in calls to nfsm_adj and setting the data
		 * len in the mbuf mb.
		 */
		len = cnt;		   /* uio_resid is subtracted later */
		cnt = round_page(cnt - uiop->uio_resid);

		/*
		 * We will make an external mbuf from the data buffer
		 * returned by VOP_DATAIN to data copy
		 */
		NFSMGETX(mp3, free_msgmbuf, ivp->iov_base, ivp->iov_base,
			 cnt, M_WAIT);
	} else 
#endif
	{
		len = left = cnt;
		nio = (cnt+MCLBYTES-1)/MCLBYTES;
		uiop->uio_iov = ivp;
		uiop->uio_iovcnt = nio;
		uiop->uio_offset = off;
		uiop->uio_resid = cnt;
		uiop->uio_rw = UIO_READ;
		uiop->uio_segflg = UIO_SYSSPACE;
		for (i = 0; i < nio; i++) {
			MGET(mp, M_WAIT, MT_DATA);
			if (left > MLEN)
			  NFSMCLGET(mp, M_WAIT);
			mp->m_len = (M_HASCL(mp)) ? MCLBYTES : MLEN;
			if (i == 0) {
				mp3 = mp2 = mp;
			} else {
				mp2->m_next = mp;
				mp2 = mp;
			}
			if (left > MLEN && !M_HASCL(mp)) {
				m_freem(mp3);
				vrele(vp);
				nfsm_srverr;
			}
			ivp->iov_base = mtod(mp, caddr_t);
			if (left > mp->m_len) {
				ivp->iov_len = mp->m_len;
				left -= mp->m_len;
			} else {
				ivp->iov_len = mp->m_len = left;
				left = 0;
			}
			ivp++;
		}
		VOP_READ(vp, uiop, 0, cred, error);
	}
	off = uiop->uio_offset;
	if (error) {
		m_freem(mp3);
		vrele(vp);
		nfsm_reply(0);
	}
	VOP_GETATTR(vp, vap, cred, error);
	if (error)
		m_freem(mp3);
	vrele(vp);
	nfsm_reply(NFSX_FATTR+NFSX_UNSIGNED);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	len -= uiop->uio_resid;
	if (len > 0) {
		tlen = nfsm_rndup(len);
		if (cnt != tlen || len != tlen)
			nfsm_adj(mp3, cnt-tlen, tlen-len);
	} else {
		m_freem(mp3);
		mp3 = (struct mbuf *)0;
	}
	nfsm_build(p, u_long *, NFSX_UNSIGNED);
	*p = txdr_unsigned(len);
	mb->m_next = mp3;
	nfsm_srvdone;
}

/*
 * nfs write service
 */
nfsrv_write(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct iovec *ivp;
	register struct mbuf *mp;
	register struct nfsv2_fattr *fp;
	struct iovec iv[NFS_MAXIOVEC];
	struct vattr va;
	register struct vattr *vap = &va;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	struct uio io, *uiop = &io;
	off_t off;
	long siz, len, xfer;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_disect(p, u_long *, 4*NFSX_UNSIGNED);
	++p;
	off = fxdr_unsigned(off_t, *p);
	p += 2;
	len = fxdr_unsigned(long, *p);
	if (len > NFS_MAXDATA || len <= 0) {
		error = EBADRPC;
		nfsm_reply(0);
	}
	if (dpos == (mtod(md, caddr_t)+md->m_len)) {
		mp = md->m_next;
		if (mp == NULL) {
			error = EBADRPC;
			nfsm_reply(0);
		}
	} else {
		mp = md;
		siz = dpos-mtod(mp, caddr_t);
		mp->m_len -= siz;
		NFSMADV(mp, siz);
	}
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	if (error = nfsrv_access(vp, VWRITE, cred)) {
		vrele(vp);
		nfsm_reply(0);
	}
	uiop->uio_resid = 0;
	uiop->uio_rw = UIO_WRITE;
	uiop->uio_segflg = UIO_SYSSPACE;
	/*
	 * Do up to NFS_MAXIOVEC mbufs of write each iteration of the
	 * loop until done.
	 */
	while (len > 0 && uiop->uio_resid == 0) {
		ivp = iv;
		siz = 0;
		uiop->uio_iov = ivp;
		uiop->uio_iovcnt = 0;
		uiop->uio_offset = off;
		while (len > 0 && uiop->uio_iovcnt < NFS_MAXIOVEC && mp != NULL) {
			ivp->iov_base = mtod(mp, caddr_t);
			if (len < mp->m_len)
				ivp->iov_len = xfer = len;
			else
				ivp->iov_len = xfer = mp->m_len;
#ifdef notdef
			/* Not Yet .. */
			if (M_HASCL(mp) && (((u_long)ivp->iov_base) & CLOFSET) == 0)
				ivp->iov_op = NULL;	/* what should it be ?? */
			else
				ivp->iov_op = NULL;
#endif
			uiop->uio_iovcnt++;
			ivp++;
			len -= xfer;
			siz += xfer;
			mp = mp->m_next;
		}
		if (len > 0 && mp == NULL) {
			error = EBADRPC;
			vrele(vp);
			nfsm_reply(0);
		}
		uiop->uio_resid = siz;
		VOP_WRITE(vp, uiop, IO_SYNC, cred, error);
		if (error) {
			vrele(vp);
			nfsm_reply(0);
		}
		off = uiop->uio_offset;
	}
	VOP_GETATTR(vp, vap, cred, error);
	vrele(vp);
	nfsm_reply(NFSX_FATTR);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	nfsm_srvdone;
}

/*
 * nfs create service
 * If doesn't exist, create it;
 * otherwise truncate to 0 length if specified.
 */
nfsrv_create(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct nfsv2_fattr *fp;
	struct vattr va;
	register struct vattr *vap = &va;
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register caddr_t cp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	long rdev;
	int error = 0, err2;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	long len;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = CREATE | WANTPARENT;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		nfsm_reply(0);
	vattr_null(vap);
	/*
	 * Iff doesn't exist, create it
	 * otherwise just truncate to 0 length
	 *   should I set the mode too ??
	 */
	if (ndp->ni_vp == NULL) {
                nfsm_disect(p, u_long *, NFSX_UNSIGNED);
		vap->va_type = IFTOVT(fxdr_unsigned(u_long, *p));
		vap->va_mode = nfstov_mode(*p);
		if (vap->va_type == VNON)
			vap->va_type = VREG;
		rdev = fxdr_unsigned(long, *(p+3));
		if (vap->va_type == VREG) {
			VOP_CREATE(ndp, vap, error);
			if (error)
				nfsm_reply(0);
		} else if (vap->va_type == VCHR || vap->va_type == VBLK ||
			vap->va_type == VFIFO) {
			if (vap->va_type == VCHR && rdev == ~0)
				vap->va_type = VFIFO;
			if (vap->va_type == VFIFO)
				;
#if     SEC_BASE
                        else if (!privileged(SEC_MKNOD, 0)) {
                                error = EPERM;
#else
                        else if (error = suser(cred, (short *)0)) {
#endif 
				VOP_ABORTOP(ndp, err2);
				vrele(ndp->ni_dvp);
				nfsm_reply(0);
			} else
				vap->va_rdev = (dev_t)rdev;
			VOP_MKNOD(ndp, vap, cred, error);
			if (error)
				nfsm_reply(0);
#ifdef	OSF1_ADFS
			ndp->ni_nameiop = LOOKUP | HASCOMPBUF;
#else
			ndp->ni_nameiop = LOOKUP | HASBUF;
#endif
			if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
				nfsm_reply(0);
		} else {
			VOP_ABORTOP(ndp, err2);
			vrele(ndp->ni_dvp);
			error = ENXIO;
			nfsm_reply(0);
		}
		vp = ndp->ni_vp;
	} else {
                struct nfsv2_sattr     *sp;
		vp = ndp->ni_vp;
		ndp->ni_vp = (struct vnode *)0;
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
                BM(VN_LOCK(vp));
                /*
                 * extract attributes from the packet and set vap to it.
                 */
                nfsm_disect(sp, struct nfsv2_sattr *, NFSX_SATTR);
                /*
                 * There is a bug in the Sun client that puts 0xffff in the
                 * mode field of sattr when it should put in 0xffffffff. The
                 * u_short doesn't sign extend.
                 * --> check the low order 2 bytes for 0xffff
                 */
                if ((fxdr_unsigned(int, sp->sa_mode) & 0xffff) != 0xffff)
                        vap->va_mode = nfstov_mode(sp->sa_mode);
                if (sp->sa_uid != nfs_xdrneg1)
                        vap->va_uid = fxdr_unsigned(uid_t, sp->sa_uid);
                if (sp->sa_gid != nfs_xdrneg1)
                        vap->va_gid = fxdr_unsigned(gid_t, sp->sa_gid);
                if (sp->sa_size != nfs_xdrneg1)
                        vap->va_size = fxdr_unsigned(u_long, sp->sa_size);

                if (vap->va_type != VNON && vap->va_type != vp->v_type) {
                        BM(VN_UNLOCK(vp));
                        error = EEXIST;
                        vrele(vp);
                        nfsm_reply(0);
                } else if (vp->v_type == VREG &&  vap->va_size == 0) {
                        BM(VN_UNLOCK(vp));
                        if ((error = nfsrv_access(vp, VWRITE, cred)) == 0) {
                                vap->va_type = VNON;
                                vap->va_mode = VNOVAL;
                                VOP_SETATTR(vp, vap, cred, error);
                        }
                        if (error) {
                                vrele(vp);
                                nfsm_reply(0);
                        }
		}
	}
	bzero((caddr_t)fhp, sizeof(nfh));
	fhp->fh_fsid = vp->v_mount->m_stat.f_fsid;
	VFS_VPTOFH(vp, &fhp->fh_fid, error);
	if (error) {
		vrele(vp);
		nfsm_reply(0);
	}
	VOP_GETATTR(vp, vap, cred, error);
	vrele(vp);
	nfsm_reply(NFSX_FH+NFSX_FATTR);
	nfsm_srvfhtom(fhp);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	return (error);
nfsmout:
	ASSERT(ndp->ni_vp && ndp->ni_dvp);
	VOP_ABORTOP(ndp, err2);
	vrele(ndp->ni_dvp);
	if (ndp->ni_vp)
		vrele(ndp->ni_vp);
	return (error);
}

/*
 * nfs remove service
 */
nfsrv_remove(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	long len;
	enum vtype type;
	int flag;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = DELETE | WANTPARENT;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		nfsm_reply(0);
#if	MACH
/*
 * NOTE:  may want to look into sharing more with unlink(), with something
 *	  like vn_remove.  The rest of this function is straight from
 *	  unlink.
 */
#endif
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	flag = vp->v_flag;
	BM(VN_UNLOCK(vp));
	if (type == VDIR) {
		error = EISDIR;
		goto out;
	}
	/*
	 * Don't unlink a mounted file.
	 */
	if (flag & VROOT) {
		error = EBUSY;
		goto out;
	}
#if     MACH
	/*
	 * XXX Calling inode_uncache(vp) without checking for i_nlink ==
	 * might be brutal. But, it is not pernicious!
	 * This same thing is done in unlink().
	 */
	if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL)
		inode_uncache(vp);
#else
	if (vp->v_flag & VTEXT)
		xrele(vp);	/* try once to free text */
#endif
out:
	if (!error)
		VOP_REMOVE(ndp, error);
	else {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		vrele(vp);
	}
	nfsm_reply(0);
	nfsm_srvdone;
}

/*
 * nfs rename service
 */
nfsrv_rename(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct nameidata *ndp;
#if	MACH
	struct utask_nd utnd;
	struct utask_nd utnd1;
#endif
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0, err2;
	char *cp2;
	struct mbuf *mb, *mreq;
	struct nameidata nami, tond;
	struct vnode *fvp, *tvp, *tdvp;
	nfsv2fh_t fnfh, tnfh;
	fhandle_t *ffhp, *tfhp;
	long len, len2;
	int rootflg = 0;

	ndp = &nami;
#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	ffhp = &fnfh.fh_generic;
	tfhp = &tnfh.fh_generic;
	nfsm_srvmtofh(ffhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	/*
	 * Remember if we are root so that we can reset cr_uid before
	 * the second nfs_namei() call
	 */
	if (cred->cr_uid == 0)
		rootflg++;
	ndp->ni_cred = cred;
	ndp->ni_nameiop = DELETE | WANTPARENT;
	if (error = nfs_namei(ndp, ffhp, len, &md, &dpos))
		nfsm_reply(0);
	fvp = ndp->ni_vp;
	nfsm_srvmtofh(tfhp);
	nfsm_srvstrsiz(len2, NFS_MAXNAMLEN);
	if (rootflg)
		cred->cr_uid = 0;
#if	MACH
	ndinit(&tond, &utnd1);
#else
	ndinit(&tond);
#endif
	crhold(cred);
	tond.ni_cred = cred;
	tond.ni_nameiop = RENAME | WANTPARENT | NOCACHE;
	error = nfs_namei(&tond, tfhp, len2, &md, &dpos);
	tdvp = tond.ni_dvp;
	tvp = tond.ni_vp;
	if (tvp != NULL) {
		enum vtype ftype;
		enum vtype ttype;
		BM(VN_LOCK(fvp));
		ftype = fvp->v_type;
		BM(VN_UNLOCK(fvp));
		BM(VN_LOCK(tvp));
		ttype = tvp->v_type;
		BM(VN_UNLOCK(tvp));

		if (ftype == VDIR && ttype != VDIR) {
			error = EISDIR;
			goto out;
		} else if (ftype != VDIR && ttype == VDIR) {
			error = ENOTDIR;
			goto out;
		}
	}
	if (error) {
		VOP_ABORTOP(ndp, err2);
		vrele(ndp->ni_dvp);
		vrele(fvp);
		goto out1;
	}
	if (fvp->v_mount != tdvp->v_mount) {
		error = EXDEV;
		goto out;
	}
	if (fvp == tdvp || fvp == tvp)
		error = EINVAL;
out:
	if (!error) {
		VREF(ndp->ni_cdir);
		VREF(tond.ni_cdir);
		VOP_RENAME(ndp, &tond, error);
		vrele(ndp->ni_cdir);
		vrele(tond.ni_cdir);
	} else {
		VOP_ABORTOP(&tond, err2);
		vrele(tond.ni_dvp);
		if (tvp)
			vrele(tvp);
		VOP_ABORTOP(ndp, err2);
		vrele(ndp->ni_dvp);
		vrele(fvp);
	}
out1:
	crfree(cred);
	nfsm_reply(0);
	return (error);
nfsmout:
	VOP_ABORTOP(ndp, err2);
	vrele(ndp->ni_dvp);
	vrele(fvp);
	return (error);
}

/*
 * nfs link service
 */
nfsrv_link(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mreq;
	struct vnode *vp, *xp;
	nfsv2fh_t nfh, dnfh;
	fhandle_t *fhp, *dfhp;
	long len;
	enum vtype type;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	dfhp = &dnfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvmtofh(dfhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
#if	SEC_BASE
	if (type == VDIR && !privileged(SEC_LINKDIR, 0)) {
		error = EPERM;
		goto out1;
	}
#else
	if (type == VDIR && (error = suser(cred, NULL)))
		goto out1;
#endif
	ndp->ni_cred = cred;
	ndp->ni_nameiop = CREATE | WANTPARENT;
	if (error = nfs_namei(ndp, dfhp, len, &md, &dpos))
		goto out1;
	xp = ndp->ni_vp;
	if (xp != NULL) {
		error = EEXIST;
		goto out;
	}
	xp = ndp->ni_dvp;
	/*
	 * N.B.:  If forcible unmount is ever working, it is possible that
	 *	  both vnodes could have been vgone'd, which would make
	 *	  their v_mount fields equivalent, but invalid.  We rely
	 *	  on an error from one of the VOP* functions to deal with
	 *	  this.
	 */
	if (vp->v_mount != xp->v_mount)
		error = EXDEV;
out:
	if (error) {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		if (ndp->ni_vp)
			vrele(ndp->ni_vp);
	} else
		VOP_LINK(vp, ndp, error);
out1:
	vrele(vp);
	nfsm_reply(0);
	nfsm_srvdone;
}

/*
 * nfs symbolic link service
 */
nfsrv_symlink(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct vattr va;
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register struct vattr *vap = &va;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0, err2;
	char *cp2;
	struct mbuf *mb, *mreq;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	long len, tlen, len2;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = CREATE | WANTPARENT;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		goto out;
	nfsm_srvstrsiz(len2, NFS_MAXPATHLEN);
	tlen = nfsm_rndup(len2);
	if (len2 == tlen) {
		nfsm_disect(cp2, caddr_t, tlen+NFSX_UNSIGNED);
		*(cp2+tlen) = '\0';
	} else {
		nfsm_disect(cp2, caddr_t, tlen);
	}
	if (ndp->ni_vp) {
		VOP_ABORTOP(ndp, err2);
		vrele(ndp->ni_dvp);
		vrele(ndp->ni_vp);
		error = EEXIST;
		goto out;
	}
	vattr_null(vap);
	vap->va_mode = 0777;
	vap->va_type = VLNK;
	VOP_SYMLINK(ndp, vap, cp2, error);
out:
	nfsm_reply(0);
	return (error);
nfsmout:
	VOP_ABORTOP(ndp, err2);
	vrele(ndp->ni_dvp);
	if (ndp->ni_vp)
		vrele(ndp->ni_vp);
	return (error);
}

/*
 * nfs mkdir service
 */
nfsrv_mkdir(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct vattr va;
	register struct vattr *vap = &va;
	register struct nfsv2_fattr *fp;
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register caddr_t cp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0, err2;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	long len;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = CREATE | WANTPARENT;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		nfsm_reply(0);
	nfsm_disect(p, u_long *, NFSX_UNSIGNED);
	vattr_null(vap);
	vap->va_type = VDIR;
	vap->va_mode = nfstov_mode(*p++);
	vp = ndp->ni_vp;
	if (vp != NULL) {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		vrele(vp);
		error = EEXIST;
		nfsm_reply(0);
	}
	VOP_MKDIR(ndp, vap, error);
	if (error)
		nfsm_reply(0);
	vp = ndp->ni_vp;
	bzero((caddr_t)fhp, sizeof(nfh));
	fhp->fh_fsid = vp->v_mount->m_stat.f_fsid;
	VFS_VPTOFH(vp, &fhp->fh_fid, error);
	if (error) {
		vrele(vp);
		nfsm_reply(0);
	}
	VOP_GETATTR(vp, vap, cred, error);
	vrele(vp);
	nfsm_reply(NFSX_FH+NFSX_FATTR);
	nfsm_srvfhtom(fhp);
	nfsm_build(fp, struct nfsv2_fattr *, NFSX_FATTR);
	nfsm_srvfillattr;
	return (error);
nfsmout:
	VOP_ABORTOP(ndp, err2);
	vrele(ndp->ni_dvp);
	if (ndp->ni_vp)
		vrele(ndp->ni_vp);
	return (error);
}

/*
 * nfs rmdir service
 */
nfsrv_rmdir(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf *mrep, *md, **mrq;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	struct nameidata nami;
	register struct nameidata *ndp = &nami;
#if	MACH
	struct utask_nd utnd;
#endif
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	long len;

#if	MACH
	ndinit(ndp, &utnd);
#else
	ndinit(ndp);
#endif
	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_srvstrsiz(len, NFS_MAXNAMLEN);
	ndp->ni_cred = cred;
	ndp->ni_nameiop = DELETE | WANTPARENT;
	if (error = nfs_namei(ndp, fhp, len, &md, &dpos))
		nfsm_reply(0);
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	if (vp->v_type != VDIR) {
		BM(VN_UNLOCK(vp));
		error = ENOTDIR;
		goto out;
	}
	BM(VN_UNLOCK(vp));
	/*
	 * No rmdir "." please.
	 */
	if (ndp->ni_dvp == vp) {
		error = EINVAL;
		goto out;
	}
	/*
	 * Don't unlink a mounted file.
	 */
	BM(VN_LOCK(vp));
	if (vp->v_flag & VROOT)
		error = EBUSY;
	BM(VN_UNLOCK(vp));
out:
	if (!error)
		VOP_RMDIR(ndp, error);
	else {
		VOP_ABORTOP(ndp, error);
		vrele(ndp->ni_dvp);
		vrele(vp);
	}
	nfsm_reply(0);
	nfsm_srvdone;
}

/*
 * nfs readdir service
 * - mallocs what it thinks is enough to read
 *	count rounded up to a multiple of DIRBLKSIZ <= NFS_MAXREADDIR
 * - calls VOP_READDIR()
 * - loops around building the reply
 *	if the output generated exceeds count break out of loop
 *	The nfsm_clget macro is used here so that the reply will be packed
 *	tightly in mbuf clusters.
 * - it only knows that it has encountered eof when the VOP_READDIR()
 *	reads nothing
 * - as such one readdir rpc will return eof false although you are there
 *	and then the next will return eof
 * - it trims out records with d_fileno == 0
 *	this doesn't matter for Unix clients, but they might confuse clients
 *	for other os'.
 * NB: It is tempting to set eof to true if the VOP_READDIR() reads less
 *	than requested, but this may not apply to all filesystems. For
 *	example, client NFS does not { although it is never remote mounted
 *	anyhow }
 * PS: The NFS protocol spec. does not clarify what the "count" byte
 *	argument is a count of.. just name strings and file id's or the
 *	entire reply rpc or ...
 *	I tried just file name and id sizes and it confused the Sun client,
 *	so I am using the full rpc size now. The "paranoia.." comment refers
 *	to including the status longwords that are not a part of the dir.
 *	"entry" structures, but are in the rpc.
 */
nfsrv_readdir(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register char *bp, *be;
	register struct mbuf *mp;
	register struct dirent *dp;
	register caddr_t cp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	char *cpos, *cend;
	int len, nlen, rem, xfer, tsiz, i;
	struct vnode *vp;
	struct mbuf *mp2, *mp3;
	nfsv2fh_t nfh;
	fhandle_t *fhp;
	struct uio io;
	struct iovec iv;
	int siz, cnt, fullsiz, eofflag;
	u_long on;
	char *rbuf;
	off_t off, toff;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	nfsm_disect(p, u_long *, 2*NFSX_UNSIGNED);
	toff = fxdr_unsigned(off_t, *p);
	++p;
	off = (toff & ~(DIRBLKSIZ-1));
	on = (toff & (DIRBLKSIZ-1));
	cnt = fxdr_unsigned(int, *p);
	siz = ((cnt+DIRBLKSIZ) & ~(DIRBLKSIZ-1));
	if (cnt >= NFS_MAXREADDIR)
		siz = NFS_MAXREADDIR;
	fullsiz = siz;
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	if (error = nfsrv_access(vp, VEXEC, cred)) {
		vrele(vp);
		nfsm_reply(0);
	}
#if	MACH
	ZALLOC(nfsreaddir_zone, rbuf, caddr_t);
#else
	MALLOC(rbuf, caddr_t, siz, M_TEMP, M_WAITOK);
#endif
again:
	iv.iov_base = rbuf;
	iv.iov_len = fullsiz;
	io.uio_iov = &iv;
	io.uio_iovcnt = 1;
	io.uio_offset = off;
	io.uio_resid = fullsiz;
	io.uio_segflg = UIO_SYSSPACE;
	io.uio_rw = UIO_READ;
	ASSERT(fullsiz <= NFS_MAXREADDIR);
	VOP_READDIR(vp, &io, cred, &eofflag, error);
	off = io.uio_offset;
	if (error) {
		vrele(vp);
#if	MACH
		ZFREE(nfsreaddir_zone, rbuf);
#else
		free((caddr_t)rbuf, M_TEMP);
#endif
		nfsm_reply(0);
	}
	if (io.uio_resid) {
		siz -= io.uio_resid;

		/*
		 * If nothing read, return eof
		 * rpc reply
		 */
		if (siz == 0) {
			vrele(vp);
			nfsm_reply(2*NFSX_UNSIGNED);
			nfsm_build(p, u_long *, 2*NFSX_UNSIGNED);
			*p++ = nfs_false;
			*p = nfs_true;
#if	MACH
			ZFREE(nfsreaddir_zone, rbuf);
#else
			FREE((caddr_t)rbuf, M_TEMP);
#endif
			return (0);
		}
	}
	/*
	 * Check for degenerate cases of nothing useful read.
	 * If so  go try again
	 */
	cpos = rbuf+on;
	cend = rbuf+siz;
	dp = (struct dirent *)cpos;
	while (cpos < cend && dp->d_fileno == 0) {
		cpos += dp->d_reclen;
		dp = (struct dirent *)cpos;
	}
	if (cpos >= cend) {
		toff = off;
		siz = fullsiz;
		on = 0;
		goto again;
	}
	cpos = rbuf+on;
	cend = rbuf+siz;
	dp = (struct dirent *)cpos;
	vrele(vp);
	len = 3*NFSX_UNSIGNED;	/* paranoia, probably can be 0 */
	bp = be = (caddr_t)0;
	mp3 = (struct mbuf *)0;
	nfsm_reply(0);

	/* Loop through the records and build reply */
	while (cpos < cend) {
		if (dp->d_fileno != 0) {
			nlen = dp->d_namlen;
			rem = nfsm_rndup(nlen)-nlen;
	
			/*
			 * As noted above, the NFS spec. is not clear about what
			 * should be included in "count" as totalled up here in
			 * "len".
			 */
			len += (4*NFSX_UNSIGNED+nlen+rem);
			if (len > cnt) {
				eofflag = 0;
				break;
			}
	
			/* Build the directory record xdr from the dirent entry */
			nfsm_clget;
			*p = nfs_true;
			bp += NFSX_UNSIGNED;
			nfsm_clget;
			*p = txdr_unsigned(dp->d_fileno);
			bp += NFSX_UNSIGNED;
			nfsm_clget;
			*p = txdr_unsigned(nlen);
			bp += NFSX_UNSIGNED;
	
			/* And loop arround copying the name */
			xfer = nlen;
			cp = dp->d_name;
			while (xfer > 0) {
				nfsm_clget;
				if ((bp+xfer) > be)
					tsiz = be-bp;
				else
					tsiz = xfer;
				bcopy(cp, bp, tsiz);
				bp += tsiz;
				xfer -= tsiz;
				if (xfer > 0)
					cp += tsiz;
			}
			/* And null pad to a long boundary */
			for (i = 0; i < rem; i++)
				*bp++ = '\0';
			nfsm_clget;
	
			/* Finish off the record */
			toff += dp->d_reclen;
			*p = txdr_unsigned(toff);
			bp += NFSX_UNSIGNED;
		} else
			toff += dp->d_reclen;
		cpos += dp->d_reclen;
		dp = (struct dirent *)cpos;
	}
	nfsm_clget;
	*p = nfs_false;
	bp += NFSX_UNSIGNED;
	nfsm_clget;
	if (eofflag)
		*p = nfs_true;
	else
		*p = nfs_false;
	bp += NFSX_UNSIGNED;
	if (bp < be)
		mp->m_len = bp-mtod(mp, caddr_t);
	mb->m_next = mp3;
#if	MACH
	ZFREE(nfsreaddir_zone, rbuf);
#else
	FREE(rbuf, M_TEMP);
#endif
	nfsm_srvdone;
}

/*
 * nfs statfs service
 */
nfsrv_statfs(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	register struct statfs *sf;
	register struct nfsv2_statfs *sfp;
	register u_long *p;
	register long t1;
	caddr_t bpos;
	struct mount *mp;
	int error = 0;
	char *cp2;
	struct mbuf *mb, *mb2, *mreq;
	struct vnode *vp;
	nfsv2fh_t nfh;
	fhandle_t *fhp;

	fhp = &nfh.fh_generic;
	nfsm_srvmtofh(fhp);
	if (error = nfsrv_fhtovp(fhp, &vp, cred))
		nfsm_reply(0);
	mp = vp->v_mount;
	sf = &mp->m_stat;
	VFS_STATFS(mp, error);
	nfsm_reply(NFSX_STATFS);
	nfsm_build(sfp, struct nfsv2_statfs *, NFSX_STATFS);
	sfp->sf_tsize = txdr_unsigned(NFS_MAXDATA);
	MOUNT_LOCK(mp);
	sfp->sf_bsize = txdr_unsigned(sf->f_fsize);
	sfp->sf_blocks = txdr_unsigned(sf->f_blocks);
	sfp->sf_bfree = txdr_unsigned(sf->f_bfree);
	sfp->sf_bavail = txdr_unsigned(sf->f_bavail);
	MOUNT_UNLOCK(mp);
	vrele(vp);
	nfsm_srvdone;
}

/*
 * Null operation, used by clients to ping server
 */
/* ARGSUSED */
nfsrv_null(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	caddr_t bpos;
	int error = 0;
	struct mbuf *mb, *mreq;

	error = VNOVAL;
	nfsm_reply(0);
	return (error);
}

/*
 * No operation, used for obsolete procedures
 */
/* ARGSUSED */
nfsrv_noop(mrep, md, dpos, cred, xid, mrq, repstat)
	struct mbuf **mrq;
	struct mbuf *mrep, *md;
	caddr_t dpos;
	struct ucred *cred;
	u_long xid;
	int *repstat;
{
	caddr_t bpos;
	int error = 0;
	struct mbuf *mb, *mreq;

	error = EPROCUNAVAIL;
	nfsm_reply(0);
	return (error);
}

/*
 * Perform access checking for vnodes obtained from file handles that would
 * refer to files already opened by a Unix client. You cannot just use
 * vn_writechk() and VOP_ACCESS() for two reasons.
 * 1 - You must check for M_EXRDONLY as well as M_RDONLY for the write case
 * 2 - The owner is to be given access irrespective of mode bits so that
 *     processes that chmod after opening a file don't break. I don't like
 *     this because it opens a security hole, but since the nfs server opens
 *     a security hole the size of a barn door anyhow, what the heck.
 */
nfsrv_access(vp, flags, cred)
	register struct vnode *vp;
	int flags;
	register struct ucred *cred;
{
	struct vattr vattr;
	int error;
	if (flags & VWRITE) {
		enum vtype type;
		int flag, mflag;
		struct mount *mp;
		BM(VN_LOCK(vp));
		type = vp->v_type;
		flag = vp->v_flag;
		BM(VN_UNLOCK(vp));
		/* Just vn_writechk() changed to check M_EXRDONLY */
		/*
		 * Disallow write attempts on read-only file systems;
		 * unless the file is a socket or a block or character
		 * device resident on the file system.
		 */
		mp = vp->v_mount;
		BM(MOUNT_LOCK(mp));
		mflag = mp->m_flag;
		BM(MOUNT_UNLOCK(mp));
		if ((mflag & (M_RDONLY | M_EXRDONLY)) &&
			type != VCHR &&
			type != VBLK &&
			type != VSOCK &&
			type != VFIFO)
				return (EROFS);
		/*
		 * If there's shared text associated with
		 * the inode, try to free it up once.  If
		 * we fail, we can't allow writing.
		 */
#if     MACH
		/*
	 	* XXX Calling inode_uncache(vp) without checking for i_nlink ==
	 	* might be brutal. But, it is not pernicious!
	 	* This same thing is done in unlink().
	 	*/
		if (vp->v_vm_info->pager != MEMORY_OBJECT_NULL)
			inode_uncache(vp);
#else
		if (vp->v_flag & VTEXT)
			xrele(vp);
#endif
		if (flag & VTEXT)
			return (ETXTBSY);
		VOP_GETATTR(vp, &vattr, cred, error);
		if (error)
			return (error);
		if (cred->cr_uid == vattr.va_uid)
			return (0);
	} else {
		VOP_GETATTR(vp, &vattr, cred, error);
		if (error)
			return (error);
		if (cred->cr_uid == vattr.va_uid)
			return (0);
	}
	VOP_ACCESS(vp, flags, cred, error);
	return (error);
}
