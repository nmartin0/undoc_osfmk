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
 * $Log: nfs_node.c,v $
 * Revision 1.5  1994/11/18  20:37:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:15:56  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:37:57  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:13  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:15  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:24  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:51:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:59  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.4.2  91/03/01  16:47:57  gmf
 * 	Return error consistently from nfsspec_reclaim().
 * 	[91/02/25  13:39:17  gmf]
 * 
 * Revision 1.12  90/10/07  14:39:01  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:18  gm]
 * 
 * 	Remove special-case code for VROOT in nfs_inactive.
 * 	The work is done in mountnfs().
 * 	[90/09/24  14:30:47  gmf]
 * 
 * Revision 1.11  90/09/23  15:56:18  devrcs
 * 	Fixed for ANSI compliance
 * 	[90/09/05  17:13:07  rossi]
 * 
 * Revision 1.10  90/09/13  11:48:35  devrcs
 * 	Set v_mount field of root vnode to DEADMOUNT so it doesn't have a bad
 * 	pointer reference when the mount structure goes away upon failure to
 * 	mount.
 * 	[90/08/22  14:36:24  gmf]
 * 
 * Revision 1.9  90/08/09  13:26:59  devrcs
 * 	nfs_nput takes vp, not np.
 * 	[90/07/31  13:38:06  gmf]
 * 
 * Revision 1.8  90/07/27  09:04:22  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:03:10  nags]
 * 
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Serialized for OSF/1.					nags@encore.com
 * 	Mods for NFS going through buffer cache.		tmt@osf.org
 * 	Add init. of n_direofstamp.				gmf@osf.org
 * 	"Fixed" nfs_abortop; mkm will fix remaining problems.	gmf@osf.org
 * 	Eliminate redundant globals (nfsnode, nfsnodeNNFSNODE).	gmf@osf.org
 * 	Integrated 4.4BSD file system changes as of 1/5/90.	gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Fix calls to vm_info_init.				gm@osf.org
 * 	New networking code from BSD.				tmt@osf.org
 * 	[90/06/12  21:35:37  nags]
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
 *	@(#)nfs_node.c	7.18 (Berkeley) 1/4/90
 */
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/dir.h>
#include <sys/namei.h>
#include <sys/errno.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/nfsnode.h>
#include <nfs/nfsmount.h>
#include <sys/kernel.h>
#include <sys/lock_types.h>
#if	MACH
#include <kern/zalloc.h>
#include <kern/mfs.h>
#else
#include <sys/malloc.h>
#endif
#include <kern/assert.h>

long	nextnfsnodeid;		/* unique id generator */

/* The request list head */
extern struct nfsreq nfsreqh;

#define	NFSNOHSZ	512
#if	((NFSNOHSZ&(NFSNOHSZ-1)) == 0)
#define	NFSNOHASH(fhsum)	((fhsum)&(NFSNOHSZ-1))
#else
#define	NFSNOHASH(fhsum)	(((unsigned)(fhsum))%NFSNOHSZ)
#endif

struct nhead {
	union {
		struct  nhead *nhu_head[2];
		struct nfsnode *nhu_chain[2];
	} nhu;
	u_long nh_timestamp;
	udecl_simple_lock_data(,nh_lock)
} nhead[NFSNOHSZ];

#define nh_head		nhu.nhu_head
#define nh_chain	nhu.nhu_chain
#define	NP_HASH_LOCK(nh)	usimple_lock(&(nh)->nh_lock)
#define NP_HASH_UNLOCK(nh)	usimple_unlock(&(nh)->nh_lock)
#define NP_HASH_LOCK_INIT(nh)	usimple_lock_init(&(nh)->nh_lock)

/*
 * Initialize hash links for nfsnodes
 * and build nfsnode free list.
 */
nfs_nhinit()
{
	register int i;
	register struct  nhead *nh = nhead;

#ifndef lint
	if (vn_maxprivate < sizeof(struct nfsnode))
		panic("nfs_nhinit: too small");
#endif
	for (i = NFSNOHSZ; --i >= 0; nh++) {
		nh->nh_head[0] = nh;
		nh->nh_head[1] = nh;
		NP_HASH_LOCK_INIT(nh);
		nh->nh_timestamp = 0;
	}
}

/*
 * Compute an entry in the NFS hash table structure
 */
struct nhead *
nfs_hash(fhp)
	register nfsv2fh_t *fhp;
{
	register u_char *fhpp;
	register u_long fhsum;
	int i;

	fhpp = &fhp->fh_bytes[0];
	fhsum = 0;
	for (i = 0; i < NFSX_FH; i++)
		fhsum += *fhpp++;
	return (&nhead[NFSNOHASH(fhsum)]);
}

/*
 * Look up a vnode/nfsnode by file handle.
 * Callers must check for mount points!!
 * In all cases, a pointer to a
 * nfsnode structure is returned.
 *
 * MP: This routine holds the NP_HASH_LOCK on the hash chain across the
 * call to insmntque.  While this is ugly, it closes a hole and keeps things
 * tidy.
 */
nfs_nget(mntp, fhp, npp)
	struct mount *mntp;
	register nfsv2fh_t *fhp;
	struct nfsnode **npp;
{
	register struct nfsnode *np, *np1;
	register struct vnode *vp;
	extern struct vnodeops nfsv2_vnodeops;
	struct vnode *nvp;
	struct nhead *nh;
	int error, stamp;

	nh = nfs_hash(fhp);
loop:
	NP_HASH_LOCK(nh);
	for (np = nh->nh_chain[0]; np != (struct nfsnode *)nh; np = np->n_forw) {
		NP_LOCK(np);
		if (mntp != NFSTOV(np)->v_mount ||
		    bcmp((caddr_t)fhp, (caddr_t)&np->n_fh, NFSX_FH)) {
			NP_UNLOCK(np);
			continue;
		}
		NP_UNLOCK(np);
		NP_HASH_UNLOCK(nh);
		vp = NFSTOV(np);
		if (vget(vp))
			goto loop;
		*npp = np;
		return(0);
	}
	stamp = nh->nh_timestamp;
	NP_HASH_UNLOCK(nh);
	if (error = getnewvnode(VT_NFS, &nfsv2_vnodeops, &nvp)) {
		*npp = 0;
		return (error);
	}
	vp = nvp;
	np = VTONFS(vp);
	np->n_vnode = vp;
	np->n_flag = 0;
	bcopy((caddr_t)fhp, (caddr_t)&np->n_fh, NFSX_FH);
	np->n_attrstamp = 0;
	np->n_direofoffset = 0;
	np->n_sillyrename = (struct sillyrename *)0;
	np->n_size = 0;
	np->n_mtime = 0;
	NP_LOCK_INIT(np);
	NP_IO_LOCK_INIT(np);
	/*
	 * Check to make sure a duplicate nfsnode hasn't been inserted into
	 * the hash chain.
	 */
	NP_HASH_LOCK(nh);
	if (stamp != nh->nh_timestamp) {
		for (np1 = nh->nh_chain[0]; np1 != (struct nfsnode *)nh; np1 = np1->n_forw) {
			NP_LOCK(np1);
			if (mntp != NFSTOV(np1)->v_mount ||
		    	    bcmp((caddr_t)fhp, (caddr_t)&np1->n_fh, NFSX_FH)) {
				NP_UNLOCK(np1);
				continue;
			}
			NP_UNLOCK(np1);
			NP_HASH_UNLOCK(nh);
			nfs_nput(vp);
			goto loop;
		}
	}
	/*
	 * Insert the nfsnode in the hash queue for its new file handle
	 */
	insque(np, nh);
	nh->nh_timestamp++;
	insmntque(vp, mntp);
	NP_HASH_UNLOCK(nh);
	*npp = np;
	return (0);
}

nfs_inactive(vp)
	register struct vnode *vp;
{
	register struct nfsnode *np;
	register struct nameidata *ndp;
	register struct sillyrename *sp;
	struct nfsnode *dnp;
	extern int prtactive;

	np = VTONFS(vp);
	BM(VN_LOCK(vp));
	if (prtactive && vp->v_usecount != 1) {
		BM(VN_UNLOCK(vp));
		vprint("nfs_inactive: pushing active", vp);
	} else
		BM(VN_UNLOCK(vp));
	NP_LOCK(np);
	sp = np->n_sillyrename;
	np->n_sillyrename = (struct sillyrename *)0;
	NP_UNLOCK(np);
	if (sp) {
		/*
		 * Remove the silly file that was rename'd earlier
		 */
		ndp = &sp->s_namei;
		if (!nfs_nget(vp->v_mount, &sp->s_fh, &dnp)) {
			ndp->ni_dvp = NFSTOV(dnp);
			nfs_removeit(ndp);
			nfs_nput(ndp->ni_dvp);
		}
		crfree(ndp->ni_cred);
#if	MACH
			ZFREE(sillyrename_zone, sp);
#else
			free((caddr_t)sp, M_TEMP);
#endif
	}
	NP_LOCK(np);
	np->n_flag &= NMODIFIED;
	NP_UNLOCK(np);
	return (0);
}

nfsspec_reclaim(vp)
	register struct vnode *vp;
{
	int error;
	if ((error = spec_reclaim(vp)) == 0)
		error = nfs_reclaim(vp);
	return (error);
}

/*
 * Reclaim an nfsnode so that it can be used for other purposes.
 */
nfs_reclaim(vp)
	register struct vnode *vp;
{
	register struct nfsnode *np = VTONFS(vp);
	extern int prtactive;
	struct nhead *nh;

	BM(NP_LOCK(np));
	nh = nfs_hash(&(np->n_fh));
	BM(NP_UNLOCK(np));
	BM(VN_LOCK(vp));
	if (prtactive && vp->v_usecount != 0) {
		BM(VN_UNLOCK(vp));
		vprint("nfs_reclaim: pushing active", vp);
	} else
		BM(VN_UNLOCK(vp));
	/*
	 * Remove the nfsnode from its hash chain.
	 */
	NP_HASH_LOCK(nh);
	remque(np);
	np->n_forw = np;
	np->n_back = np;
	NP_HASH_UNLOCK(nh);
	cache_purge(vp);
	NP_LOCK(np);
	np->n_flag = 0;
	np->n_direofoffset = 0;
	NP_UNLOCK(np);
	return (0);
}


#if	MACH_ASSERT
nfs_nput(vp)
	struct vnode *vp;
{
	vrele(vp);
}
#endif

/*
 * Nfs abort op, called after namei() when a CREATE/DELETE isn't actually
 * done. Currently nothing to do.
 */

nfs_abortop(ndp)
	register struct nameidata *ndp;
{
	return (0);
}

/*
 * This is silly, but if you use a macro and try and use it in a file
 * that has mbuf.h included, m_data --> m_hdr.mh_data and this is not
 * a good thing
 */
struct nfsmount *vfs_to_nfs(mp)
	struct mount *mp;
{
	struct nfsmount *nmp;

	ASSERT(mp != DEADMOUNT);
	MOUNT_LOCK(mp);
	nmp = (struct nfsmount *)mp->m_data;
	MOUNT_UNLOCK(mp);
	return(nmp);
}
