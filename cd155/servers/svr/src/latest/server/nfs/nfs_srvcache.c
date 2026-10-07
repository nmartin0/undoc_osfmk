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
 * $Log: nfs_srvcache.c,v $
 * Revision 1.6  1995/02/11  01:08:53  stans
 *  #undef TRUE and FALSE in preference of local defs
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.5  1994/11/18  20:37:08  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:16:23  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:29  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:46  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:26  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:37  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/12/16  19:48:49  roy
 * 	91/11/26  15:33:35  sp
 * 	Upgrade to 1.0.3
 *
 * Revision 2.2  91/08/31  13:51:49  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/07/31  15:40:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7.9.2  91/10/28  11:56:11  jvs
 * 	Turn NFS server cache back on and fix two missed unlocks.
 * 	This is a correction for bug #2796.
 * 	[91/10/28  11:53:25  jvs]
 * 
 * Revision 1.7.7.2  91/08/12  14:30:10  jvs
 * 	"Fix" the nfssrvcache deadlock by disabling the cache.
 * 	[91/08/12  14:25:44  jvs]
 * 
 * Revision 1.7.5.2  91/04/29  12:10:42  tmt
 * 	Enable server cache. Fix rc_reply (only when RC_REPMBUF set).
 * 	Move semicolon into BM statements.
 * 	[91/04/10  09:55:19  tmt]
 * 
 * Revision 1.7  90/10/07  14:39:18  devrcs
 * 	Return RC_NOCACHE when cache entry create fails/disabled.
 * 	[90/10/02  17:50:29  tmt]
 * 
 * 	Fix server cache: uninitialized variable, return without value,
 * 	broken cache matching (IP addresses only). Add cache enable
 * 	variable to save memory if cache not critical, it's really
 * 	hungry as is! Should drain off cache entries periodically, and
 * 	must do so on demand when out of mbufs. TODO.
 * 	[90/10/02  15:07:26  tmt]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:37  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:21:46  gm]
 * 
 * Revision 1.6  90/07/27  09:04:31  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:03:31  nags]
 * 
 * Revision 1.5  90/03/27  13:24:51  gm
 * 
 * 	Changes from nags at Encore
 * 	[90/03/15  17:18:40  pam]
 * 
 * Revision 1.3  90/03/08  12:51:21  gmf
 * 	add repvalid argument to nfsrv_updatecache
 * 
 * Revision cthon  90/02/12  13:04:51  gmf/tmt/rick
 * 	Un-static idempotent array
 * 
 * 	Fix server cache to consider valid or invalid replies
 * 
 * Revision 1.4  90/02/05  15:51:22  robert
 * 	Integrated 4.4BSD file system changes as of 1/5/90
 * 	[90/01/18  19:21:42  gmf]
 * 
 * Revision 1.3  90/01/02  20:22:33  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:19:48  gm
 * 	New networking code from BSD.
 * 	[89/12/16            tmt]
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
 *	@(#)nfs_srvcache.c	7.4 (Berkeley) 8/30/89
 */

#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <nfs/nfsm_subs.h>
#include <nfs/nfsv2.h>
#include <nfs/nfsrvcache.h>
#include <nfs/nfs.h>

#if	((NFSRCHSZ&(NFSRCHSZ-1)) == 0)
#define	NFSRCHASH(xid)		(((xid)+((xid)>>16))&(NFSRCHSZ-1))
#else
#define	NFSRCHASH(xid)		(((unsigned)((xid)+((xid)>>16)))%NFSRCHSZ)
#endif

struct rhead {
	union {
		struct rhead		*rhu_head[2];
		struct nfsrvcache	*rhu_chain[2];
	} rhu;
	u_long				rh_timestamp; /* hash chain modified */
	udecl_simple_lock_data(,rh_lock)
} rhead[NFSRCHSZ];
#define	rh_head			rhu.rhu_head
#define	rh_chain		rhu.rhu_chain
#define	NSC_HASH_NULL		((struct rhead *) NULL)

#define	NSC_HASH_LOCK(rh)	usimple_lock(&(rh)->rh_lock)
#define	NSC_HASH_UNLOCK(rh)	usimple_unlock(&(rh)->rh_lock)
#define	NSC_HASH_LOCK_TRY(rh)	usimple_lock_try(&(rh)->rh_lock)
#define	NSC_HASH_LOCK_INIT(rh)	usimple_lock_init(&(rh)->rh_lock)

static struct nfsrvcache nfsrvcachehead;
static struct nfsrvcache nfsrvcache[NFSRVCACHESIZ];
udecl_simple_lock_data(static,nfsrvcachehead_lock)

#define	NSC_LRU_LOCK()		usimple_lock(&nfsrvcachehead_lock)
#define	NSC_LRU_UNLOCK()	usimple_unlock(&nfsrvcachehead_lock)
#define	NSC_LRU_LOCK_INIT()	usimple_lock_init(&nfsrvcachehead_lock)

#ifdef	TRUE
#undef	TRUE
#undef	FALSE
#endif

#define TRUE	1
#define	FALSE	0

/*
 * Static array that defines which nfs rpc's are nonidempotent
 */
int nonidempotent[NFS_NPROCS] = {
	FALSE,
	FALSE,
	TRUE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	FALSE,
	FALSE,
};

/* True iff the rpc reply is an nfs status ONLY! */
static int repliesstatus[NFS_NPROCS] = {
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	FALSE,
	TRUE,
	TRUE,
	TRUE,
	TRUE,
	FALSE,
	TRUE,
	FALSE,
	FALSE,
};

/*
 * Initialize the server request cache list
 */
nfsrv_initcache()
{
	register int i;
	register struct nfsrvcache *rp = nfsrvcache;
	register struct nfsrvcache *hp = &nfsrvcachehead;
	register struct  rhead *rh = rhead;

	for (i = NFSRCHSZ; --i >= 0; rh++) {
		rh->rh_head[0] = rh;
		rh->rh_head[1] = rh;
		rh->rh_timestamp = 0;
		NSC_HASH_LOCK_INIT(rh);
	}
	hp->rc_next = hp->rc_prev = hp;
	bzero((caddr_t)rp, sizeof nfsrvcache);
	for (i = NFSRVCACHESIZ; i-- > 0; ) {
		rp->rc_state = RC_UNUSED;
		rp->rc_forw = rp;
		rp->rc_back = rp;
		rp->rc_next = hp->rc_next;
		rp->rc_hash_chain = NSC_HASH_NULL;
		NSC_LOCK_INIT(rp);
		hp->rc_next->rc_prev = rp;
		rp->rc_prev = hp;
		hp->rc_next = rp;
		rp++;
	}
	NSC_LRU_LOCK_INIT();
}

/*
 * Look for the request in the cache
 * If found then
 *    return action and optionally reply
 * else
 *    insert it in the cache
 *
 * The rules are as follows:
 * - if in progress, return DROP request
 * - if completed within DELAY of the current time, return DROP it
 * - if completed a longer time ago return REPLY if the reply was cached or
 *   return DOIT
 * Update/add new request at end of lru list
 *
 * The cache may be disabled in order to save on mbufs. A cache entry
 * typically uses one mbuf for the requestor's address and often another
 * for the reply, at minimum. These entries are not (yet) timed out or
 * drained on demand. This is a lot with 128-entry caches!
 */

int nfsrvcache_enable = 1;

/*
 * Cache matching. The problem here is making positively sure that the
 * cache matches the request. Since multiple hosts, and even multiple
 * requestors on a single host, can use the same xid, we need a second
 * level match, so we look at the nfs proc plus the sender sockaddr (the
 * Reno code uses the IP address.) For the sake of transport independence
 * and minimal assumptions, we use the entire sockaddr, but we may reject
 * some requests from using the cache when they might (because the port
 * may not match). However, using the port is strictly most correct, and
 * will typically not be a problem with most client implementations.
 */
#define CACHE_MATCH(rp, xid, proc, nam)				\
	((xid) == (rp)->rc_xid && (proc) == (rp)->rc_proc &&	\
	 (nam)->m_len == (rp)->rc_rnam->m_len &&		\
	 bcmp(mtod((nam),caddr_t),mtod((rp)->rc_rnam,caddr_t),	\
	      (unsigned)((nam)->m_len)) == 0)

nfsrv_getcache(nam, xid, proc, repp)
	struct mbuf *nam;
	u_long xid;
	int proc;
	struct mbuf **repp;
{
	register struct nfsrvcache *rp;
	register struct nfsrvcache *rp2;
	register struct  rhead *rh;
	struct rhead *hc;
	int ret, flag;
	long ts;
	u_long ostamp;
	BM(int s;)

	if (!nfsrvcache_enable)
		return RC_NOCACHE;

	rh = &rhead[NFSRCHASH(xid)];
loop:
	NSC_HASH_LOCK(rh);
	for (rp = rh->rh_chain[0]; rp != (struct nfsrvcache *)rh;
	     rp = rp->rc_forw) {
		if (CACHE_MATCH(rp, xid, proc, nam)) {
			NSC_LOCK(rp);
			NSC_HASH_UNLOCK(rh);
			if ((rp->rc_flag & RC_LOCKED) != 0) {
				rp->rc_flag |= RC_WANTED;
				assert_wait((int)rp, FALSE);
				NSC_UNLOCK(rp);
				thread_block();
				goto loop;
			}
			rp->rc_flag |= RC_LOCKED;
			flag = rp->rc_flag;
			NSC_UNLOCK(rp);
			NSC_LRU_LOCK();
			put_at_head(rp);
			NSC_LRU_UNLOCK();
			if (rp->rc_state == RC_UNUSED)
				panic("nfsrv cache");
			BM(s = splhigh());
			BM(TIME_READ_LOCK());
			ts = time.tv_sec;
			BM(TIME_READ_UNLOCK());
			BM(splx(s));
			if (rp->rc_state == RC_INPROG ||
			   (ts - rp->rc_timestamp) < RC_DELAY) {
				NFS_STATS(nfsstats.srvcache_inproghits++);
				ret = RC_DROPIT;
			} else if (flag & RC_REPSTATUS) {
				struct mbuf *mb; /* scratch for nfs_rephead */
				caddr_t bpos;
				NFS_STATS(nfsstats.srvcache_idemdonehits++);
				nfs_rephead(0, xid, rp->rc_status, repp, &mb,
					&bpos);
				rp->rc_timestamp = ts;
				ret = RC_REPLY;
			} else if (flag & RC_REPMBUF) {
				NFS_STATS(nfsstats.srvcache_idemdonehits++);
				*repp = NFSMCOPY(rp->rc_reply, 0, M_COPYALL,
						M_WAIT);
				rp->rc_timestamp = ts;
				ret = RC_REPLY;
			} else {
				NFS_STATS(nfsstats.srvcache_nonidemdonehits++);
				rp->rc_state = RC_INPROG;
				ret = RC_DOIT;
			}
			NSC_LOCK(rp);
			flag = rp->rc_flag;
			rp->rc_flag &= ~(RC_LOCKED|RC_WANTED);
			NSC_UNLOCK(rp);
			if (flag & RC_WANTED)
				thread_wakeup((int)rp);
			return (ret);
		}
	}
	ostamp = rh->rh_timestamp;
	NSC_HASH_UNLOCK(rh);
	NFS_STATS(nfsstats.srvcache_misses++);
	NSC_LRU_LOCK();
	for (rp=nfsrvcachehead.rc_prev; rp!=&nfsrvcachehead; rp=rp->rc_prev) {
		NSC_LOCK(rp);
		/*
		 * Skip locked entries and entries with locked hash headers.
		 */
		if ((rp->rc_flag & (RC_LOCKED|RC_WANTED)) ||
		    ((hc = rp->rc_hash_chain) != NSC_HASH_NULL &&
		     !NSC_HASH_LOCK_TRY(hc))) {
			NSC_UNLOCK(rp);
			continue;
		}
		nfsrvhead_remove(rp);
		NSC_LRU_UNLOCK();
		if (hc != NSC_HASH_NULL) {
			remque(rp);
			NSC_HASH_UNLOCK(hc);
		}
		break;
	}
	if (rp == &nfsrvcachehead) {
		NSC_LRU_UNLOCK();
		NFS_STATS(nfsstats.srvcache_reqdrops++);
		return RC_NOCACHE;
	}
	flag = rp->rc_flag;
	/* Init cache entry to hold new request. Add reply later. */
	if ((rp->rc_flag & RC_REPMBUF) && rp->rc_reply)
		m_freem(rp->rc_reply);
	rp->rc_reply = 0;
	if (rp->rc_rnam) (void) m_free(rp->rc_rnam);
	rp->rc_rnam = m_copym(nam, 0, nam->m_len, M_WAIT);
	rp->rc_xid = xid;
	rp->rc_proc = proc;
	rp->rc_flag = 0;
	rp->rc_state = RC_INPROG;
	rp->rc_hash_chain = rh;
	NSC_UNLOCK(rp);
	NSC_HASH_LOCK(rh);
	if (rh->rh_timestamp != ostamp) {
		for (rp2 = rh->rh_chain[0]; rp2 != (struct nfsrvcache *)rh;
		     rp2 = rp2->rc_forw) {
			if (CACHE_MATCH(rp2, xid, proc, nam)) {
				NSC_HASH_UNLOCK(rh);
				rp->rc_hash_chain = NSC_HASH_NULL;
				rp->rc_state = RC_UNUSED;
				(void) m_free(rp->rc_rnam);
				rp->rc_rnam = 0;
				NSC_LRU_LOCK();
				nfsrvhead_lruadd(rp);
				NSC_LRU_UNLOCK();
				goto loop;
			}
		}
	}
	insque(rp, rh);
	NSC_LRU_LOCK();
	nfsrvhead_add(rp);
	NSC_LRU_UNLOCK();
	rh->rh_timestamp++;
	NSC_HASH_UNLOCK(rh);
	return (RC_DOIT);
}

/*
 * Update a request cache entry after the rpc has been done
 */
nfsrv_updatecache(nam, xid, proc, repvalid, repstat, repmbuf)
	struct mbuf *nam;
	u_long xid;
	int proc;
	int repvalid;
	int repstat;
	struct mbuf *repmbuf;
{
	register struct nfsrvcache *rp;
	register struct	rhead *rh;
	int flag = 0;
	BM(int s;)

	rh = &rhead[NFSRCHASH(xid)];
loop:
	NSC_HASH_LOCK(rh);
	for (rp = rh->rh_chain[0]; rp != (struct nfsrvcache *)rh; rp = rp->rc_forw) {
		if (CACHE_MATCH(rp, xid, proc, nam)) {
			NSC_LOCK(rp);
			NSC_HASH_UNLOCK(rh);
			if ((rp->rc_flag & RC_LOCKED) != 0) {
				rp->rc_flag |= RC_WANTED;
				assert_wait((int)rp, FALSE);
				NSC_UNLOCK(rp);
				thread_block();
				goto loop;
			}
			rp->rc_flag |= RC_LOCKED;
			NSC_UNLOCK(rp);
			rp->rc_state = RC_DONE;
			if (repvalid) {
				BM(s = splhigh());
				BM(TIME_READ_LOCK());
				rp->rc_timestamp = time.tv_sec;
				BM(TIME_READ_UNLOCK());
				BM(splx(s));
				if (nonidempotent[proc]) {
					if (repliesstatus[proc]) {
						rp->rc_status = repstat;
						flag = RC_REPSTATUS;
					} else {
						rp->rc_reply = NFSMCOPY(repmbuf,
							0, M_COPYALL, M_WAIT);
						flag = RC_REPMBUF;
					}
				}
			} else {
				rp->rc_timestamp = 0;
			}
			NSC_LOCK(rp);
			rp->rc_flag |= flag;
			flag = rp->rc_flag;
			rp->rc_flag &= ~(RC_LOCKED|RC_WANTED);
			NSC_UNLOCK(rp);
			if (flag & RC_WANTED)
				thread_wakeup((int)rp);
			return;
		}
	}
	NSC_HASH_UNLOCK(rh);
}
