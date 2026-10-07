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
 * $Log: nfs_subs.c,v $
 * Revision 1.7  1994/11/18  20:37:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1993/07/14  18:16:29  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:27  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  20:29:31  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.4  1993/04/03  03:06:49  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 2.9  1993/01/06  10:38:21  loverso
 * 	Fix notice.
 *
 * Revision 2.8  92/12/08  10:44:52  durriya
 * 	1.1 unmount sync changes - changed MOUNT_LOOKUP_DONE to 
 * 	UNMOUNT_READ_UNLOCK                            (durriya)
 * 
 * Revision 2.7  92/11/17  19:52:19  loverso
 * 	In is_remote(), call dev_node_lookup() to get the device node. (mmp)
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:00:40  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:58:24  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:32:30  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:22  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.6  92/06/08  18:21:18  pjg
 * 	Added is_remote, extract_nfs_fh and nfsm_disct_non_destructive for
 * 	ADFS (durriya).
 * 
 * Revision 2.5  92/03/09  12:15:13  durriya
 * 	Revision 3.3  91/12/18  17:17:36  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.4  92/01/05  20:03:39  roy
 * 	1991/11/12  19:44:27  noemi
 * 	Changed parameters in call to specalloc.
 * 
 * 	1991/10/14  19:57:47  noemi
 * 	Changed to HASBUF nameiop to HASCOMPBUF.
 * 
 * Revision 2.3  91/10/14  12:36:18  sjs
 * 	91/09/13  12:51:03  sp
 * 	include uxkern/vm_param.h to find PAGE_SIZE
 * 
 * Revision 2.2  91/08/31  13:51:58  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:40:26  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/31  14:02:02  devrcs
 * 	Deal correctly with case where we find a socket we didn't
 * 	create, i.e. don't dereference NULL.
 * 	[90/10/11  07:58:26  tmt]
 * 
 * Revision 1.11  90/10/07  14:39:25  devrcs
 * 	Handle VSOCK creation in nfs_loadattrcache.
 * 	[90/10/03  16:11:42  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:53  gm]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:08:25  collins]
 * 
 * Revision 1.10  90/08/24  12:15:53  devrcs
 * 	Dynamic loading of NFS.  Can't dynamically destroy a
 * 	zone (yet) so kludge around the problem by keeping zone
 * 	pointers in permanent variables.
 * 	[90/08/18  03:45:49  nags]
 * 
 * Revision 1.9  90/07/27  09:04:35  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:03:39  nags]
 * 
 * 	Corrected calculation of va_bytes in nfs_loadattrcache.
 * 	Also added support for FIFO files.
 * 	[90/06/18  00:17:21  ers]
 * 
 * 	nags merge
 * 	[90/06/12  21:36:03  nags]
 * 
 * 	Remove pageable flag from zinit argument list.
 * 	[90/06/07  15:37:50  jvs]
 * 
 * 	Compressed history (reverse chronology):
 * 	Updated for OSF/1 parallelization.			nags@encore.com
 * 	Fix some size == 0 problems.				tmt@osf.org
 * 	Use nfs_socketinit instead of nfs_timer.		tmt@osf.org
 * 	Change m_fsid to m_stat.f_fsid.				gmf@osf.org
 * 	va_size1 ==> va_size_rsv (byte swapping).		gmf@osf.org
 * 	Do fillinhost at runtime and enable by default.		gmf/tmt@osf.org
 * 	Store file size correctly when big-endian.		noemi@osf.org
 * 	Eliminate #ifdef VOP_PRINT (backward compatibility).	gmf@osf.org
 * 	Changes to make async_daemon work.			gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				gm@osf.org
 * 	[90/06/07  16:18:33  jvs]
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
 *	@(#)nfs_subs.c	7.14 (Berkeley) 11/25/89
 */

/*
 * These functions support the macros and help fiddle mbuf chains for
 * the nfs op functions. They do things like create the rpc header and
 * copy data between mbuf chains and uio lists.
 */
#include <sys/unix_defs.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <ufs/dir.h>
#include <sys/time.h>
#include <sys/errno.h>
#include <sys/kernel.h>
#if	MACH
#include <kern/zalloc.h>
#else
#include <sys/malloc.h>
#endif
#include <sys/mbuf.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/uio.h>
#include <sys/namei.h>
#include <sys/ucred.h>
#include <sys/map.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#include <nfs/rpcv2.h>
#include <nfs/nfsv2.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <nfs/nfs.h>
#include <nfs/xdr_subs.h>
#include <nfs/nfsm_subs.h>
#if	MACH
#ifdef	OSF1_SERVER
#include <uxkern/vm_param.h>
#else	OSF1_SERVER
#include <mach/vm_param.h>
#endif	OSF1_SERVER
#else
#include <nfs/nfsiom.h>
#endif

struct nfsstats nfsstats;
vdecl_simple_lock_data(,nfs_stats_lock)

/*
 * Data items converted to xdr at startup, since they are constant
 * This is kinda hokey, but may save a little time doing byte swaps
 */
u_long nfs_procids[NFS_NPROCS];
u_long nfs_xdrneg1;
u_long rpc_call, rpc_vers, rpc_reply, rpc_msgdenied,
	rpc_mismatch, rpc_auth_unix, rpc_msgaccepted;
u_long nfs_vers, nfs_prog, nfs_true, nfs_false;

/* And other global data */
static struct unixauth {
	u_long	rpc_auth;
	u_long	size;
	u_long	hostid;
	u_long	hostnamelen;
	char	hostname[MAXHOSTNAMELEN];
} unixauth_hdr;
static u_long nfs_xid = 1;
static struct unixauth *rpc_unixauth = &unixauth_hdr;
static int unixauth_size = 0;
static int unixauth_init = 0;

extern long hostid;
extern enum vtype v_type[];
#if	!MACH
extern struct map nfsmap[NFS_MSIZ];
#endif

lock_data_t	unixauth_lock;
#define NFSAUTH_WRITE_LOCK()		lock_write(&unixauth_lock)
#define NFSAUTH_LOCK_DONE()		lock_done(&unixauth_lock)
#define NFSAUTH_LOCK_INIT()		lock_init(&unixauth_lock, TRUE)

udecl_simple_lock_data(, nfsxid_lock)
#define	NFSXID_LOCK()		usimple_lock(&nfsxid_lock)
#define	NFSXID_UNLOCK()		usimple_unlock(&nfsxid_lock)
#define	NFSXID_LOCK_INIT()	usimple_lock_init(&nfsxid_lock)

/*
 * Maximum number of groups passed through to NFS server.
 * For release 3.X systems, the maximum value is 8.
 * For release 4.X systems, the maximum value is 10.
 */
int numgrps = 8;
/*
 * Flag to fill hostname in RPC unix auth.
 */
int fillinhost = 1;

int nfsmbuf_fit = 0;
int nfsmbuf_nofit = 0;
/*
 * Create the header for an rpc request packet
 * The function nfs_unixauth() creates a unix style authorization string
 * and returns a ptr to it.
 * The hsiz is the size of the rest of the nfs request header.
 * (just used to decide if a cluster is a good idea)
 * nb: Note that the prog, vers and procid args are already in xdr byte order
 */
struct mbuf *nfsm_reqh(prog, vers, procid, cred, hsiz, bpos, mb, retxid)
	u_long prog;
	u_long vers;
	u_long procid;
	struct ucred *cred;
	int hsiz;
	caddr_t *bpos;
	struct mbuf **mb;
	u_long *retxid;
{
	register struct mbuf *mreq, *m;
	register u_long *p, *nmp;
	struct mbuf *m1;
	char *ap;
	int asiz, siz, ngr, i;
	struct nfsmount nm;	/* unixauth must be less than this size */

	if (!unixauth_init)
		nfs_unixauth_init();
	nmp = (u_long *)&nm;
	bzero(nmp, sizeof(struct nfsmount));
	NFSMGETHDR(mreq);
	ngr = ((cred->cr_ngroups > numgrps) ? numgrps : cred->cr_ngroups);
	asiz = (ngr << 2);
	if (fillinhost)
		asiz += nfsm_rndup(hostnamelen)+(9*NFSX_UNSIGNED);
	else
		asiz += 9*NFSX_UNSIGNED;

	/* If we need a lot, alloc a cluster ?? */
	if ((asiz+hsiz+RPC_SIZ) > MHLEN)
		NFSMCLGET(mreq, M_WAIT);
	mreq->m_len = NFSMSIZ(mreq);
	siz = mreq->m_len;
	m1 = mreq;
	/*
	 * Alloc enough mbufs
	 * We do it now to avoid all sleeps after the call to nfs_unixauth()
	 */
	while ((asiz+RPC_SIZ) > siz) {
		MGET(m, M_WAIT, MT_DATA);
		m1->m_next = m;
		m->m_len = MLEN;
		siz += MLEN;
		m1 = m;
	}
	p = mtod(mreq, u_long *);
	NFSXID_LOCK();
	*p++ = *retxid = txdr_unsigned(++nfs_xid);
	NFSXID_UNLOCK();
	*p++ = rpc_call;
	*p++ = rpc_vers;
	*p++ = prog;
	*p++ = vers;
	*p++ = procid;

	m = mreq;
	siz = m->m_len-RPC_SIZ;
	if (asiz <= siz)
		nmp = p;
	/*
	 * Copy the unix auth info into a local structure
	 */
	ap = (char *)nmp;
	bcopy((caddr_t)rpc_unixauth, nmp, unixauth_size);
	nmp = (u_long *) ((int)nmp + unixauth_size);
	*nmp++ = txdr_unsigned(cred->cr_uid);
	*nmp++ = txdr_unsigned(cred->cr_gid);
	*nmp++ = txdr_unsigned(ngr);
	for (i = 0; i < ngr; i++)
		*nmp++ = txdr_unsigned(cred->cr_groups[i]);
	/* And add the AUTH_NULL */
	*nmp++ = 0;
	*nmp = 0;
	i = ((caddr_t)nmp - (caddr_t)(&((struct unixauth *)ap)->hostnamelen));
	((struct unixauth *)ap)->size = txdr_unsigned(i);

	if (asiz <= siz) {
		nfsmbuf_fit++;
		m->m_len = asiz+RPC_SIZ;
	} else {
		nfsmbuf_nofit++;
		bcopy(ap, (caddr_t)p, siz);
		ap += siz;
		asiz -= siz;
		while (asiz > 0) {
			siz = (asiz > MLEN) ? MLEN : asiz;
			m = m->m_next;
			bcopy(ap, mtod(m, caddr_t), siz);
			m->m_len = siz;
			asiz -= siz;
			ap += siz;
		}
	}
	
	/* Finally, return values */
	*mb = m;
	*bpos = mtod(m, caddr_t)+m->m_len;
	return (mreq);
}

/*
 * copies mbuf chain to the uio scatter/gather list
 */
nfsm_mbuftouio(mrep, uiop, siz, dpos)
	struct mbuf **mrep;
	struct uio *uiop;
	int siz;
	caddr_t *dpos;
{
	register int xfer, left, len;
	register struct mbuf *mp;
	register char *mbufcp, *uiocp;
	long uiosiz, rem;
	int error = 0;

	mp = *mrep;
	mbufcp = *dpos;
	len = mtod(mp, caddr_t)+mp->m_len-mbufcp;
	rem = nfsm_rndup(siz)-siz;
	while (siz > 0) {
		if (uiop->uio_iovcnt <= 0 || uiop->uio_iov == NULL)
			return (EFBIG);
		left = uiop->uio_iov->iov_len;
		uiocp = uiop->uio_iov->iov_base;
		if (left > siz)
			left = siz;
		uiosiz = left;
		while (left > 0) {
			while (len == 0) {
				mp = mp->m_next;
				if (mp == NULL)
					return (EBADRPC);
				mbufcp = mtod(mp, caddr_t);
				len = mp->m_len;
			}
			xfer = (left > len) ? len : left;
#ifdef notdef
			/* Not Yet.. */
			if (uiop->uio_iov->iov_op != NULL)
				(*(uiop->uio_iov->iov_op))
				(mbufcp, uiocp, xfer);
			else
#endif
			if (uiop->uio_segflg == UIO_SYSSPACE)
				bcopy(mbufcp, uiocp, xfer);
			else
				copyout(mbufcp, uiocp, xfer);
			left -= xfer;
			len -= xfer;
			mbufcp += xfer;
			uiocp += xfer;
			uiop->uio_offset += xfer;
			uiop->uio_resid -= xfer;
		}
		if (uiop->uio_iov->iov_len <= siz) {
			uiop->uio_iovcnt--;
			uiop->uio_iov++;
		} else {
			uiop->uio_iov->iov_base += uiosiz;
			uiop->uio_iov->iov_len -= uiosiz;
		}
		siz -= uiosiz;
	}
	*dpos = mbufcp;
	*mrep = mp;
	if (rem > 0) {
		if (len < rem)
			error = nfs_adv(mrep, dpos, rem, len);
		else
			*dpos += rem;
	}
	return (0);
}

/*
 * copies a uio scatter/gather list to an mbuf chain...
 */
nfsm_uiotombuf(uiop, mq, siz, bpos)
	register struct uio *uiop;
	struct mbuf **mq;
	int siz;
	caddr_t *bpos;
{
	register struct mbuf *mp;
	struct mbuf *mp2;
	long xfer, left, uiosiz;
	int clflg;
	int rem, len;
	char *cp, *uiocp;

	if (siz > MLEN)		/* or should it >= MCLBYTES ?? */
		clflg = 1;
	else
		clflg = 0;
	rem = nfsm_rndup(siz)-siz;
	mp2 = *mq;
	while (siz > 0) {
		if (uiop->uio_iovcnt <= 0 || uiop->uio_iov == NULL)
			return (EINVAL);
		left = uiop->uio_iov->iov_len;
		uiocp = uiop->uio_iov->iov_base;
		if (left > siz)
			left = siz;
		uiosiz = left;
		while (left > 0) {
			MGET(mp, M_WAIT, MT_DATA);
			if (clflg)
				NFSMCLGET(mp, M_WAIT);
			mp->m_len = NFSMSIZ(mp);
			mp2->m_next = mp;
			mp2 = mp;
			xfer = (left > mp->m_len) ? mp->m_len : left;
#ifdef notdef
			/* Not Yet.. */
			if (uiop->uio_iov->iov_op != NULL)
				(*(uiop->uio_iov->iov_op))
				(uiocp, mtod(mp, caddr_t), xfer);
			else
#endif
			if (uiop->uio_segflg == UIO_SYSSPACE)
				bcopy(uiocp, mtod(mp, caddr_t), xfer);
			else
				copyin(uiocp, mtod(mp, caddr_t), xfer);
			len = mp->m_len;
			mp->m_len = xfer;
			left -= xfer;
			uiocp += xfer;
			uiop->uio_offset += xfer;
			uiop->uio_resid -= xfer;
		}
		if (uiop->uio_iov->iov_len <= siz) {
			uiop->uio_iovcnt--;
			uiop->uio_iov++;
		} else {
			uiop->uio_iov->iov_base += uiosiz;
			uiop->uio_iov->iov_len -= uiosiz;
		}
		siz -= uiosiz;
	}
	if (rem > 0) {
		if (rem > (len-mp->m_len)) {
			MGET(mp, M_WAIT, MT_DATA);
			mp->m_len = 0;
			mp2->m_next = mp;
		}
		cp = mtod(mp, caddr_t)+mp->m_len;
		for (left = 0; left < rem; left++)
			*cp++ = '\0';
		mp->m_len += rem;
		*bpos = cp;
	} else
		*bpos = mtod(mp, caddr_t)+mp->m_len;
	*mq = mp;
	return (0);
}

/*
 * Help break down an mbuf chain by setting the first siz bytes contiguous
 * pointed to by returned val.
 * If Updateflg == True we can overwrite the first part of the mbuf data
 * This is used by the macros nfsm_disect and nfsm_disecton for tough
 * cases. (The macros use the vars. dpos and dpos2)
 */
nfsm_disct(mdp, dposp, siz, left, updateflg, cp2)
	struct mbuf **mdp;
	caddr_t *dposp;
	int siz;
	int left;
	int updateflg;
	caddr_t *cp2;
{
	register struct mbuf *mp, *mp2;
	register int siz2, xfer;
	register caddr_t p;

	mp = *mdp;
	while (left == 0) {
		*mdp = mp = mp->m_next;
		if (mp == NULL)
			return (EBADRPC);
		left = mp->m_len;
		*dposp = mtod(mp, caddr_t);
	}
	if (left >= siz) {
		*cp2 = *dposp;
		*dposp += siz;
		return (0);
	} else if (mp->m_next == NULL) {
		return (EBADRPC);
	} else if (siz > MCLBYTES) {
		panic("nfs S too big");
	} else {
		/* Iff update, you can overwrite, else must alloc new mbuf */
		if (updateflg) {
			NFSMINOFF(mp);
		} else {
			MGET(mp2, M_WAIT, MT_DATA);
			mp2->m_next = mp->m_next;
			mp->m_next = mp2;
			mp->m_len -= left;
			mp = mp2;
		}
		/* Alloc cluster iff we need it */
		if (!M_HASCL(mp) && siz > NFSMSIZ(mp)) {
			NFSMCLGET(mp, M_WAIT);
			if (!M_HASCL(mp))
				return (ENOBUFS);
		}
		*cp2 = p = mtod(mp, caddr_t);
		bcopy(*dposp, p, left);		/* Copy what was left */
		siz2 = siz-left;
		p += left;
		mp2 = mp->m_next;
		/* Loop around copying up the siz2 bytes */
		while (siz2 > 0) {
			if (mp2 == NULL)
				return (EBADRPC);
			xfer = (siz2 > mp2->m_len) ? mp2->m_len : siz2;
			if (xfer > 0) {
				bcopy(mtod(mp2, caddr_t), p, xfer);
				NFSMADV(mp2, xfer);
				mp2->m_len -= xfer;
				siz2 -= xfer;
			}
			if (siz2 > 0)
				mp2 = mp2->m_next;
		}
		mp->m_len = siz;
		*mdp = mp2;
		*dposp = mtod(mp2, caddr_t);
	}
	return (0);
}

/*
 * Advance the position in the mbuf chain with/without freeing mbufs
 */
nfs_adv(mdp, dposp, offs, left)
	struct mbuf **mdp;
	caddr_t *dposp;
	int offs;
	int left;
{
	register struct mbuf *m;
	register int s;

	m = *mdp;
	s = left;
	while (s < offs) {
		offs -= s;
		m = m->m_next;
		if (m == NULL)
			return (EBADRPC);
		s = m->m_len;
	}
	*mdp = m;
	*dposp = mtod(m, caddr_t)+offs;
	return (0);
}

/*
 * Copy a string into mbufs for the hard cases...
 */
nfsm_strtmbuf(mb, bpos, cp, siz)
	struct mbuf **mb;
	char **bpos;
	char *cp;
	long siz;
{
	register struct mbuf *m1, *m2;
	long left, xfer, len, tlen;
	u_long *p;
	int putsize;

	putsize = 1;
	m2 = *mb;
	left = NFSMSIZ(m2)-m2->m_len;
	if (left > 0) {
		p = ((u_long *)(*bpos));
		*p++ = txdr_unsigned(siz);
		putsize = 0;
		left -= NFSX_UNSIGNED;
		m2->m_len += NFSX_UNSIGNED;
		if (left > 0) {
			bcopy(cp, (caddr_t) p, left);
			siz -= left;
			cp += left;
			m2->m_len += left;
			left = 0;
		}
	}
	/* Loop around adding mbufs */
	while (siz > 0) {
		MGET(m1, M_WAIT, MT_DATA);
		if (siz > MLEN)
			NFSMCLGET(m1, M_WAIT);
		m1->m_len = NFSMSIZ(m1);
		m2->m_next = m1;
		m2 = m1;
		p = mtod(m1, u_long *);
		tlen = 0;
		if (putsize) {
			*p++ = txdr_unsigned(siz);
			m1->m_len -= NFSX_UNSIGNED;
			tlen = NFSX_UNSIGNED;
			putsize = 0;
		}
		if (siz < m1->m_len) {
			len = nfsm_rndup(siz);
			xfer = siz;
			if (xfer < len)
				*(p+(xfer>>2)) = 0;
		} else {
			xfer = len = m1->m_len;
		}
		bcopy(cp, (caddr_t) p, xfer);
		m1->m_len = len+tlen;
		siz -= xfer;
		cp += xfer;
	}
	*mb = m1;
	*bpos = mtod(m1, caddr_t)+m1->m_len;
	return (0);
}


#if	MACH
/*
 * These definitions need to move.
 * Keep them small at first to detect memory leaks faster.
 */
#define NFSMOUNT_MAX	(512)
#define SILLYRENAME_MAX	64
#define NFSREQ_MAX	128
#define READDIR_MAX	16

/* About the same, and linked, so just use one zone. */
#define NFSMOUNT_SIZE	(sizeof(struct nfsmount) > sizeof(struct nfshost) ? \
			 sizeof(struct nfsmount) : sizeof(struct nfshost))

/*
 * Called once to initialize data structures.
 * Memory zones allocated will initially be neither exhaustible nor pageable.
 * If we run out, we panic; this if fine for debugging, but not for production.
 * These attributes should should be examined further.  The sleepable attribute
 * is not used at this time, so is a noop.
 */
nfs_zone_init()
{
	/*
	 * nfsmount structures.  One per NFS mounted file system.
	 * Not too heavily used, and tend to be long-lived.
	 */
	if (nfsmount_zone == ZONE_NULL) {
		nfsmount_zone = zinit(NFSMOUNT_SIZE,
				      NFSMOUNT_MAX * NFSMOUNT_SIZE,
				      PAGE_SIZE, "nfsmount");
		if (nfsmount_zone == ZONE_NULL)
			panic("nfs_zone_init: no zones");
		nfsmount_zone->exhaustible = TRUE;
	}

	/*
	 * sillyrename structures.  These are used by the nfs client when
	 * removing files with a ref. count > 1.  These are not heavily
	 * used, and don't stay around all that long.  There shouldn't
	 * need to be very many.
	 */
	if (sillyrename_zone == ZONE_NULL) {
		sillyrename_zone = zinit(sizeof(struct sillyrename),
				      SILLYRENAME_MAX*sizeof(struct sillyrename),
				      PAGE_SIZE, "sillyrename");
		if (sillyrename_zone == ZONE_NULL)
			panic("nfs_zone_init: no zones");
	}
	/*
	 * nfsreq structures.  One per client NFS request.  Heavily used
	 * and transient.
	 */
	if (nfsreq_zone == ZONE_NULL) {
		nfsreq_zone = zinit(sizeof(struct nfsreq),
				      NFSREQ_MAX*sizeof(struct nfsreq),
				      PAGE_SIZE, "nfsreq");
		if (nfsreq_zone == ZONE_NULL)
			panic("nfs_zone_init: no zones");
	}
	/*
	 * readdir:  These are used for temporary storage for reading
	 * and translating directory reads.  They are currently up to
	 * NFS_MAXREADDIR in size (8k).
	 */
	if (nfsreaddir_zone == ZONE_NULL) {
		nfsreaddir_zone = zinit(NFS_MAXREADDIR,
					READDIR_MAX*NFS_MAXREADDIR,
					NFS_MAXREADDIR, "nfsreaddir");
		if (nfsreaddir_zone == ZONE_NULL)
			panic("nfs_zone_init: no zones");
	}
}
#endif	/* MACH */

nfs_init()
{
	register int i;

#if	MACH
	nfs_zone_init();
#endif
	rpc_vers = txdr_unsigned(RPC_VER2);
	rpc_call = txdr_unsigned(RPC_CALL);
	rpc_reply = txdr_unsigned(RPC_REPLY);
	rpc_msgdenied = txdr_unsigned(RPC_MSGDENIED);
	rpc_msgaccepted = txdr_unsigned(RPC_MSGACCEPTED);
	rpc_mismatch = txdr_unsigned(RPC_MISMATCH);
	rpc_auth_unix = txdr_unsigned(RPCAUTH_UNIX);
	nfs_vers = txdr_unsigned(NFS_VER2);
	nfs_prog = txdr_unsigned(NFS_PROG);
	nfs_true = txdr_unsigned(TRUE);
	nfs_false = txdr_unsigned(FALSE);
	/* Loop thru nfs procids */
	for (i = 0; i < NFS_NPROCS; i++)
		nfs_procids[i] = txdr_unsigned(i);
	v_type[0] = VNON;
	v_type[1] = VREG;
	v_type[2] = VDIR;
	v_type[3] = VBLK;
	v_type[4] = VCHR;
	v_type[5] = VLNK;
	v_type[6] = VSOCK;
	v_type[7] = VBAD;
	v_type[8] = VFIFO;
	nfs_xdrneg1 = txdr_unsigned(-1);
	nfs_nhinit();			/* Init the nfsnode table */
	nfsrv_initcache();		/* Init the server request cache */
	nfs_socketinit();		/* Init the socket stuff */
	nfs_mntidinit();
#if	!MACH
	rminit(nfsmap, (long)NFS_MAPREG, (long)1, "nfs mapreg", NFS_MSIZ);
#endif
	VSTATS_LOCK_INIT(&nfs_stats_lock);
	NFSXID_LOCK_INIT();
	NFSAUTH_LOCK_INIT();
	ASYNCD_LOCK_INIT();
	nfs_syscall_config();
	return (0);
}

/*
 * Initialize the unixauth header
 */
nfs_unixauth_init()
{
	register u_long *p;
	register int i;

	NFSAUTH_WRITE_LOCK();
	if (!unixauth_init) {
		unixauth_hdr.rpc_auth = txdr_unsigned(RPCAUTH_UNIX);
		HOSTNAME_READ_LOCK();
		unixauth_hdr.hostid = hostid;
		if (fillinhost) {
			unixauth_hdr.hostnamelen = txdr_unsigned(hostnamelen);
			i = nfsm_rndup(hostnamelen);
			bcopy(hostname, (caddr_t)&unixauth_hdr.hostname, hostnamelen);
		} else 
			unixauth_hdr.hostnamelen = 0;
		HOSTNAME_READ_UNLOCK();
		unixauth_size = ((int)&unixauth_hdr.hostname[i] -
				 (int)rpc_unixauth);
		unixauth_init = 1;
	}
	NFSAUTH_LOCK_DONE();
}

/*
 * Attribute cache routines.
 * nfs_loadattrcache() - loads or updates the cache contents from attributes
 *	that are on the mbuf list
 * nfs_getattrcache() - returns valid attributes if found in cache, returns
 *	error otherwise
 */

/*
 * Load the attribute cache (that lives in the nfsnode entry) with
 * the values on the mbuf list and
 * Iff vap not NULL
 *    copy the attributes to *vaper
 */
nfs_loadattrcache(vp, mdp, dposp, vaper)
	register struct vnode *vp;
	struct mbuf **mdp;
	caddr_t *dposp;
	struct vattr *vaper;
{
	register struct vattr *vap;
	register struct nfsv2_fattr *fp;
	extern struct vnodeops spec_nfsv2nodeops;
	extern struct vnodeops fifo_nfsv2nodeops;
	register struct nfsnode *np;
	register long t1;
	caddr_t dpos, cp2;
	int error = 0;
	struct mbuf *md;
	enum vtype type;
	dev_t rdev;
	struct timeval mtime;

	md = *mdp;
	dpos = *dposp;
	t1 = (mtod(md, caddr_t)+md->m_len)-dpos;
	if (error = nfsm_disct(&md, &dpos, NFSX_FATTR, t1, TRUE, &cp2))
		return (error);
	fp = (struct nfsv2_fattr *)cp2;
	type = nfstov_type(fp->fa_type);
	rdev = fxdr_unsigned(dev_t, fp->fa_rdev);
	/*
	 * Because the FIFO file type was not defined in the original
	 * NFS protocol spec, different vendors have come up with various
	 * ways to encode the FIFO type over the wire.
	 */
	if ((type == VCHR || type == VNON) && rdev == ~0) {
		type = VFIFO;
		rdev = 0;
	}
	fxdr_time(&fp->fa_mtime, &mtime);
	/*
	 * If v_type == VNON it is a new node, so fill in the v_type,
	 * n_mtime fields. Check to see if it represents a special
	 * device, and if so, check for a possible alias. Once the
	 * correct vnode has been obtained, fill in the rest of the
	 * information.
	 */
	np = VTONFS(vp);
	BM(VN_LOCK(vp));
	if (vp->v_type == VNON) {
		BM(VN_UNLOCK(vp));
		VN_LOCK(vp);
		vp->v_type = type;
		if (vp->v_type == VCHR || vp->v_type == VBLK) {
			vp->v_op = &spec_nfsv2nodeops;
			VN_UNLOCK(vp);
#ifdef	OSF1_ADFS
			if (error = specalloc(vp, rdev, -1))
#else
			if (error = specalloc(vp, rdev))
#endif
			{
				VN_LOCK(vp);
				vp->v_type = VNON;
				VN_UNLOCK(vp);
				return (error);
			}
			VN_LOCK(vp);
		} else if (vp->v_type == VSOCK) {
			if (vaper) {
				vp->v_socket =(struct socket *)vaper->va_socket;
				vaper = 0;	/* Ick. Don't want return. */
			} else
				vp->v_socket = 0;
		} else if (vp->v_type == VFIFO)
			vp->v_op = &fifo_nfsv2nodeops;
		VN_UNLOCK(vp);
		NP_LOCK(np);
		np->n_mtime = mtime.tv_sec;
		NP_UNLOCK(np);
	} else
		BM(VN_UNLOCK(vp));
	NP_LOCK(np);
	vap = &np->n_vattr;
	vap->va_type = type;
	vap->va_mode = nfstov_mode(fp->fa_mode);
	vap->va_nlink = fxdr_unsigned(u_short, fp->fa_nlink);
	vap->va_uid = fxdr_unsigned(uid_t, fp->fa_uid);
	vap->va_gid = fxdr_unsigned(gid_t, fp->fa_gid);
	vap->va_size = fxdr_unsigned(u_long, fp->fa_size);
	if ((np->n_flag & NMODIFIED) == 0 || vap->va_size > np->n_size)
		np->n_size = vap->va_size;
	vap->va_size_rsv = 0;		/* OR -1 ?? */
	vap->va_blocksize = fxdr_unsigned(long, fp->fa_blocksize);
	vap->va_rdev = rdev;
	vap->va_bytes = fxdr_unsigned(long, fp->fa_blocks) * NFS_FABLKSIZE;
	vap->va_bytes_rsv = 0;
	vap->va_fsid = vp->v_mount->m_stat.f_fsid.val[0];
	vap->va_fileid = fxdr_unsigned(long, fp->fa_fileid);
	vap->va_atime.tv_sec = fxdr_unsigned(long, fp->fa_atime.tv_sec);
	vap->va_atime.tv_usec = 0;
	vap->va_flags = fxdr_unsigned(u_long, fp->fa_atime.tv_usec);
	vap->va_mtime = mtime;
	vap->va_ctime.tv_sec = fxdr_unsigned(long, fp->fa_ctime.tv_sec);
	vap->va_ctime.tv_usec = 0;
	vap->va_gen = fxdr_unsigned(u_long, fp->fa_ctime.tv_usec);
	np->n_attrstamp = time.tv_sec;
	NP_UNLOCK(np);
	*dposp = dpos;
	*mdp = md;
	if (vaper != NULL) {
		bcopy((caddr_t)vap, (caddr_t)vaper, sizeof(*vap));
		if ((np->n_flag & NMODIFIED) && (np->n_size > vap->va_size)) {
			BM(NP_LOCK(np));
			vaper->va_size = np->n_size;
			BM(NP_UNLOCK(np));
		}
	}
	return (0);
}

/*
 * Check the time stamp
 * If the cache is valid, copy contents to *vap and return 0
 * otherwise return an error
 */
nfs_getattrcache(vp, vap)
	register struct vnode *vp;
	struct vattr *vap;
{
	register struct nfsnode *np;
	int s;
	long ts;

	np = VTONFS(vp);
	BM(s = splhigh());
	BM(TIME_READ_LOCK());
	ts = time.tv_sec;
	BM(TIME_READ_UNLOCK());
	BM(splx(s));
	BM(NP_LOCK(np));
	if ((ts-np->n_attrstamp) < NFS_ATTRTIMEO) {
		BM(NP_UNLOCK(np));
		NFS_STATS(nfsstats.attrcache_hits++);
		
		bcopy((caddr_t)&np->n_vattr,(caddr_t)vap,sizeof(struct vattr));
		if ((np->n_flag & NMODIFIED) == 0)
			np->n_size = vap->va_size;
		else if (np->n_size > vap->va_size)
			vap->va_size = np->n_size;
		return (0);
	} else {
		BM(NP_UNLOCK(np));
		NFS_STATS(nfsstats.attrcache_misses++);
		return (ENOENT);
	}
}

/*
 * nfs_namei - a liitle like namei(), but for one element only
 *	essentially look up file handle, fill in ndp and call VOP_LOOKUP()
 */
nfs_namei(ndp, fhp, len, mdp, dposp)
	register struct nameidata *ndp;
	fhandle_t *fhp;
	int len;
	struct mbuf **mdp;
	caddr_t *dposp;
{
	register int i, rem;
	register struct mbuf *md;
	register char *cp;
	struct vnode *dp;
	int flag;
	int error = 0;

#ifdef	OSF1_ADFS
	if ((ndp->ni_nameiop & HASCOMPBUF) == 0)
#else
	if ((ndp->ni_nameiop & HASBUF) == 0)
#endif
	{
		flag = ndp->ni_nameiop & OPFLAG;

		/*
		 * Copy the name from the mbuf list to the d_name field of ndp
		 * and set the various ndp fields appropriately.
		 */
		cp = *dposp;
		md = *mdp;
		rem = mtod(md, caddr_t)+md->m_len-cp;
		ndp->ni_hash = 0;
		for (i = 0; i < len;) {
			while (rem == 0) {
				md = md->m_next;
				if (md == NULL)
					return (EBADRPC);
				cp = mtod(md, caddr_t);
				rem = md->m_len;
			}
			if (*cp == '\0' || *cp == '/')
				return (EINVAL);
			if (*cp & 0200)
				if ((*cp&0377) == ('/'|0200) || flag != DELETE)
					return (EINVAL);
			ndp->ni_dent.d_name[i++] = *cp;
			ndp->ni_hash += (unsigned char)*cp * i;
			cp++;
			rem--;
		}
		*mdp = md;
		*dposp = cp;
		len = nfsm_rndup(len)-len;
		if (len > 0) {
			if (rem < len) {
				if (error = nfs_adv(mdp, dposp, len, rem))
					return (error);
			} else
				*dposp = cp+len;
		}
	} else
		i = len;
	ndp->ni_namelen = i;
	ndp->ni_dent.d_namlen = i;
	ndp->ni_dent.d_name[i] = '\0';
	ndp->ni_pathlen = 1;
	ndp->ni_pnbuf = ndp->ni_dirp = ndp->ni_ptr = &ndp->ni_dent.d_name[0];
	ndp->ni_next = &ndp->ni_dent.d_name[i];
#ifdef	OSF1_ADFS
	ndp->ni_nameiop |= (NOCROSSMOUNT | REMOTE | HASCOMPBUF);
#else
	ndp->ni_nameiop |= (NOCROSSMOUNT | REMOTE | HASBUF);
#endif

	if (error = nfsrv_fhtovp(fhp, &dp, ndp->ni_cred))
		return (error);
	BM(VN_LOCK(dp));
	if (dp->v_type != VDIR) {
		BM(VN_UNLOCK(dp));
		vrele(dp);
		return (ENOTDIR);
	}
	BM(VN_UNLOCK(dp));
	/*
	 * Must set current directory here to avoid confusion in namei()
	 * called from rename()
	 */
	ndp->ni_cdir = dp;
	ndp->ni_rdir = NULLVP;

	/*
	 * And call namei() to do the real work
	 */
	error = namei(ndp);
	vrele(dp);
	return (error);
}

/*
 * A fiddled version of m_adj() that ensures null fill to a long
 * boundary and only trims off the back end
 */
nfsm_adj(mp, len, nul)
	struct mbuf *mp;
	register int len;
	int nul;
{
	register struct mbuf *m;
	register int count, i;
	register char *cp;

	/*
	 * Trim from tail.  Scan the mbuf chain,
	 * calculating its length and finding the last mbuf.
	 * If the adjustment only affects this mbuf, then just
	 * adjust and return.  Otherwise, rescan and truncate
	 * after the remaining size.
	 */
	count = 0;
	m = mp;
	for (;;) {
		count += m->m_len;
		if (m->m_next == (struct mbuf *)0)
			break;
		m = m->m_next;
	}
	if (m->m_len > len) {
		m->m_len -= len;
		if (nul > 0) {
			cp = mtod(m, caddr_t)+m->m_len-nul;
			for (i = 0; i < nul; i++)
				*cp++ = '\0';
		}
		return;
	}
	count -= len;
	if (count < 0)
		count = 0;
	/*
	 * Correct length for chain is "count".
	 * Find the mbuf with last data, adjust its length,
	 * and toss data from remaining mbufs on chain.
	 */
	for (m = mp; m; m = m->m_next) {
		if (m->m_len >= count) {
			m->m_len = count;
			if (nul > 0) {
				cp = mtod(m, caddr_t)+m->m_len-nul;
				for (i = 0; i < nul; i++)
					*cp++ = '\0';
			}
			break;
		}
		count -= m->m_len;
	}
	while (m = m->m_next)
		m->m_len = 0;
}

/*
 * nfsrv_fhtovp() - convert a fh to a vnode ptr
 * 	- look up fsid in mount list (if not found ret error)
 *	- check that it is exported
 *	- get vp by calling VFS_FHTOVP() macro
 *	- if cred->cr_uid == 0 set it to m_exroot
 */
nfsrv_fhtovp(fhp, vpp, cred)
	fhandle_t *fhp;
	struct vnode **vpp;
	struct ucred *cred;
{
	register struct mount *mp;
	int error;

	if ((mp = getvfs(&fhp->fh_fsid)) == NULL)
		return (ESTALE);
	if ((mp->m_flag & M_EXPORTED) == 0) {
		UNMOUNT_READ_UNLOCK(mp);
		return (EACCES);
	}
	VFS_FHTOVP(mp, &fhp->fh_fid, vpp, error);
	if (error)  {
		UNMOUNT_READ_UNLOCK(mp);
		return (ESTALE);
	}
	if (cred->cr_uid == 0)
		cred->cr_uid = mp->m_exroot;
	UNMOUNT_READ_UNLOCK(mp);
	return (0);
}

#ifdef OSF1_ADFS
int
is_remote(md, dpos, remote, node)
        struct mbuf *md;
        caddr_t  dpos;
        boolean_t *remote;
        node_t   *node;
{
        fhandle_t	fh;
        int		error=0;

        /* first extract the file handle from the msgbuf */
        if (error = extract_nfs_fh(md, dpos, &fh)) {
                return(error);
        } 

        /* next peek into the file handle to get the node # where the file
         * is mounted/lives.
         */
	*node = dev_node_lookup(FSID1_2_NODE(fh.fh_fsid));
        if (*node == this_node) {
                *remote = FALSE;
        } else {
                *remote = TRUE;
        }
        return(error);
}

int
extract_nfs_fh(md, dpos, fh)
        struct mbuf *md;
        caddr_t     dpos;
        fhandle_t   *fh;
{

        u_long *temp_fhp;
        long t1;
        int error=0;

        t1 = mtod(md, caddr_t)+md->m_len-dpos; 
        if (t1 >= NFSX_FH) {
                temp_fhp = (u_long *)(dpos);

        } else {
                if (error = nfsm_disct_non_destructive(md, dpos, NFSX_FH, t1, 
                                                       &temp_fhp)) { 
                        return(error);
                }
        }
        
        bcopy((caddr_t)temp_fhp, (caddr_t)fh, NFSX_FH); 
        return(0);

}

/*
 * Help break down an mbuf chain by setting the first siz bytes contiguous
 * pointed to by returned val.
 * If Updateflg == True we can overwrite the first part of the mbuf data
 * This is used by the macros nfsm_disect and nfsm_disecton for tough
 * cases. (The macros use the vars. dpos and dpos2)
 */
nfsm_disct_non_destructive(md, dpos, siz, left,temp_fhp)
	struct mbuf *md;
	caddr_t dpos;
	int siz;
	int left;
	caddr_t *temp_fhp;
{
	register struct mbuf *mp = md;
        caddr_t    dp = dpos;

	while (left == 0) {
		mp = mp->m_next;
		if (mp == NULL)
                  return (EBADRPC);
		left = mp->m_len;
		dp = mtod(mp, caddr_t);
	}
	if (left >= siz) {
		*temp_fhp = dp;
		return (0);
	} else if (mp->m_next == NULL) {
		return (EBADRPC);
	} else if (siz > MCLBYTES) {
		panic("nfs S too big");
	} else {
                panic("nfsm_disct_non_destructive : cannot proceed\n");
        }

}
#endif /* OSF1_ADFS */
