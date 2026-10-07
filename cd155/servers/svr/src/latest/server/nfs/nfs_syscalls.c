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
 * $Log: nfs_syscalls.c,v $
 * Revision 1.5  1994/11/18  20:37:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:16:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:38:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:33  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:53  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:35  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:25  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:44  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/06/08  18:21:29  pjg
 * 	Fix nfs_nfssvc for AD. NFS server now works for AD. (durriya)
 *
 * Revision 2.3  92/05/24  20:50:59  pjg
 * 	Disable the NFS server in AD (it doesn't work yet anyway).
 * 
 * Revision 2.2  91/08/31  13:52:10  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:40:31  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.6.5  91/06/20  14:58:39  tmt
 * 	Take missing ASYNCD lock above loop in async_daemon.
 * 	[91/06/20  08:53:09  tmt]
 * 
 * Revision 1.13.6.4  91/06/03  14:47:53  tmt
 * 	Fix PRIV_SUSER from returning with lock.
 * 	Don't call pmap_collect with lock.
 * 	[91/05/23  13:06:14  tmt]
 * 
 * Revision 1.13.6.2  91/04/29  12:10:49  tmt
 * 	Return error to client from server nfs_getreq(), if appropriate.
 * 	Make async daemons sleep interruptibly.
 * 	[91/04/10  10:11:16  tmt]
 * 
 * Revision 1.13.4.2  91/03/15  17:49:59  tmt
 * 	Make nfsd's and nfsiod's not swappable to avoid deadlock.
 * 	Attempt to discard their text pages with pmap_collect too.
 * 	[91/03/13  19:13:59  tmt]
 * 
 * Revision 1.13  90/10/07  14:39:31  devrcs
 * 	Check RC_NOCACHE return from server getcache, and don't
 * 	update cache with reply if so.
 * 	[90/10/02  17:51:21  tmt]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:06:45  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:01  gm]
 * 
 * 	Change buffer lock ownership debugging code to eliminate
 * 	race conditions.
 * 	[90/09/23  19:29:47  jeffc]
 * 
 * Revision 1.12  90/09/13  11:48:38  devrcs
 * 	Call BUF_INHERIT at splbio (bug 848)
 * 	[90/08/30  15:42:17  noemi]
 * 
 * Revision 1.11  90/08/24  12:16:01  devrcs
 * 	Changes for new system call interface
 * 	[90/08/17  17:45:56  nags]
 * 
 * 	Dynamic NFS loading.  New buffer lock assertions.
 * 	[90/08/19  01:07:36  nags]
 * 
 * Revision 1.10  90/08/09  13:27:11  devrcs
 * 	Changed for new getsock() interface.
 * 	[90/08/03  14:17:47  gmf]
 * 
 * 	Bug fix: call to privileged() should be using 'code'.
 * 	[90/07/30  09:00:40  seiden]
 * 
 * Revision 1.9  90/07/27  09:04:40  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:03:49  nags]
 * 
 * Revision 1.8  90/07/17  11:37:25  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:55:47  seiden]
 * 
 * Revision 1.7  90/06/22  20:40:13  devrcs
 * 	nags merge
 * 	[90/06/12  21:36:10  nags]
 * 
 * 	Compressed history (reverse chronology):
 * 
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Nags merge.						nags@encore.com
 * 	SecureWare changes.					seiden@osf.org
 * 	Updated for OSF/1 parallelization.			nags@encore.com
 * 	Wakeup one thread in server; longjmp when interrupted.	tmt@osf.org
 * 	Exportfs superseded by mount with M_UPDATE.		gmf@osf.org
 * 	Fix for server cache from rick@guelph.			gmf@osf.org
 * 	Nfsiods must clean up when killed.			gmf@osf.org
 * 	Changes to make async_daemon work.			gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	New networking code from BSD.				tmt@osf.org
 * 
 * $EndLog$
 */
/* @(#)nfs_syscalls.c   2.1 16:13:10 4/20/90 SecureWare */
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
 *	@(#)nfs_syscalls.c	7.2 (Berkeley) 8/30/89
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/buf.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/nfsrvcache.h>
#include <nfs/nfssys.h>
#if	MACH
#include <kern/sched_prim.h>
#endif

/* Global defs. */
extern u_long nfs_prog, nfs_vers;
extern int (*nfsrv_procs[NFS_NPROCS])();
struct file *getsock();

/*
 * Maximum allowable number of asyncdaemons (patchable).
 */
int	max_asyncdaemons = NFS_MAXASYNCDAEMON;

/*
 * The asyncdaemon lock protects several variables:
 *	asyncdaemon_requests	--	# of requests pending + in-progress
 *	nfs_asyncdaemons	--	# of asyncdaemons in existence
 *	nfs_bqueue		--	queue of requests to the daemons
 */
udecl_simple_lock_data(,asyncdaemon_lock)

#ifdef	notyet
/*
 * When killing off asyncdaemons, it is necessary to handle outstanding
 * requests on nfs_bqueue that may have been enqueued just before or while
 * the asyncdaemon was being signalled.  The current behavior handles all
 * pending requests before actually exiting but it may be more reasonable
 * to handle a limited number of requests, instead.  A prior analysis
 * suggests that at most only one request will ever need to be handled
 * so we choose a more conservative value here.
 */
#define NFS_NUMDYING	2    /* async_daemon requests to handle before dying */
#endif

/*
 * NFS server system calls
 */

/*
 * Mask some of the security differences.
 */
#if	SEC_BASE
#define	PRIV_SUSER(cr,acflag,priv,code,retcode,error)			\
	if (!privileged(priv,code))	   				\
		error = retcode;					\
	else								\
		error = 0;
#else
#define	PRIV_SUSER(cr,acflag,priv,code,retcode,error)			\
	error = suser((cr),(acflag))
#define	SEC_REMOTE	0
#endif
	
int nfs_nfsds = 0;

/*
 * Nfs server psuedo system call for the nfsd's
 * Never returns unless it fails or gets killed
 */
nfs_nfssvc(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
                struct file *s;
		u_long ormask;
		u_long matchbits;
	} *uap = (struct args *) args;
	register struct mbuf *m;
	register int siz;
	register struct ucred *cr;
	struct file *fp;
	struct mbuf *mreq, *mrep, *nam, *md;
	struct socket *so;
	caddr_t dpos;
	int procid, cacherep;
	u_long retxid;
	u_long msk, mtch;
	int repstat;
	int error;
#ifdef OSF1_ADFS
        node_t node;
        boolean_t remote;
#endif
        
	/*
	 * Must be super user
	 */
	PRIV_SUSER(u.u_cred, &u.u_acflag, SEC_REMOTE, 0, EPERM, error);
	if (error == 0)
		fp = getsock(uap->s, &error);
	if (error != 0) {
		NFSD_UNLOCK();
		return (error);
	}
	nfs_nfsds++;
	NFSD_UNLOCK();
	so = (struct socket *)fp->f_data;
	SOCKET_LOCK(so);
	so->so_rcv.sb_flags |= SB_WAKEONE;	/* Wake up one at a time */
	SOCKET_UNLOCK(so);
	/*
	 * must copy credentials so others don't see changes
	 */
	cr = u.u_cred = crcopy(u.u_cred);
	msk = uap->ormask;
	mtch = uap->matchbits;

	/*
	 * Just loop around doin our stuff until SIGKILL
	 */
	for (;;) {
		if (error = nfs_getreq(so, nfs_prog, nfs_vers, NFS_NPROCS-1,
		   &nam, &mrep, &md, &dpos, &retxid, &procid, cr,msk,mtch)) {
			switch (error) {
			case ERPCMISMATCH:
			case EPROGUNAVAIL:
			case EPROGMISMATCH:
			case EPROCUNAVAIL:
			case EBADRPC:
				nfs_rephead(0,retxid,error,&mreq,&mrep,&dpos);
				goto sendit;
			case EPIPE:
			case ERESTART:
			case EINTR:
				if (nam)
					m_freem(nam);
				error = 0;
				goto out;
			}
			if (nam)
				m_freem(nam);
			so->so_error = 0;
			continue;
		}
		if (nam)
			cacherep = nfsrv_getcache(nam, retxid, procid, &mreq);
		else
			cacherep = RC_NOCACHE;
		
		switch (cacherep) {
		case RC_DOIT:
		case RC_NOCACHE:
#ifdef OSF1_ADFS
                        if ((procid == NFSPROC_NULL) || 
                            (procid == NFSPROC_ROOT) || 
                            (procid == NFSPROC_WRITECACHE)) {
                                /* for all the above nfs ops no arguments 
                                 * (including file handle) is
                                 * provided
                                 */
                                error = (*(nfsrv_procs[procid]))(mrep,
                                                                 md, 
                                                                 dpos,
                                                                 cr, 
                                                                 retxid,
                                                                 &mreq, 
                                                                 &repstat);
                        } else if (!(error = is_remote(md, dpos, 
                                                       &remote, &node))) {
                                if (!remote) {
                                        error = (*(nfsrv_procs[procid]))(mrep,
                                                                md, 
                                                                dpos,
                                                                cr, 
                                                                retxid,
                                                                &mreq, 
                                                                &repstat);
                                } else {
                                        error = remote_nfsrv_op(node, procid, 
                                                                mrep, md, dpos,
                                                                cr, retxid, 
                                                                &mreq, 
                                                                &repstat);
                                }
                        } else {
                                panic("nfs_nfssvc:Cannot get location of NFS req\n");
                        }

                        if (error) {
                                if (nam) {
					if (cacherep != RC_NOCACHE)
					  nfsrv_updatecache(nam, retxid, 
                                                            procid, FALSE, 
                                                            repstat, mreq);
					m_freem(nam);
				}
				NFS_STATS(nfsstats.srv_errs++);
				break;
			}
#else /* OSF1_ADFS */
			if (error = (*(nfsrv_procs[procid]))(mrep, md, dpos,
				cr, retxid, &mreq, &repstat)) {
				if (nam) {
					if (cacherep != RC_NOCACHE)
					  nfsrv_updatecache(nam, retxid, procid,
						FALSE, repstat, mreq);
					m_freem(nam);
				}
				NFS_STATS(nfsstats.srv_errs++);
				break;
			}
#endif /* OSF1_ADFS */
			NFS_STATS(nfsstats.srvrpccnt[procid]++);
			if (nam && cacherep != RC_NOCACHE)
				nfsrv_updatecache(nam, retxid, procid, TRUE,
				 	repstat, mreq);
		sendit:
			mrep = (struct mbuf *)0;
		case RC_REPLY:
			m = mreq;
			siz = 0;
			while (m) {
				siz += m->m_len;
				m = m->m_next;
			}
			if (siz <= 0 || siz > 9216) {
				printf("mbuf siz=%d\n",siz);
				panic("Bad nfs svc reply");
			}
			error = nfs_send(so, nam, mreq, 0, siz);
			if (nam)
				m_freem(nam);
			if (mrep)
				m_freem(mrep);
			if (error) {
				if (error == EPIPE || error == EINTR ||
					error == ERESTART)
					goto out;
				so->so_error = 0;
			}
			break;
		case RC_DROPIT:
			m_freem(mrep);
			m_freem(nam);
			break;
		}
		if (error == EINTR) {
			error = 0;
			goto out;
		}
	}
out:
	FP_UNREF(fp);
	NFSD_LOCK();
	nfs_nfsds--;
	NFSD_UNLOCK();
	return (error);
}

/*
 * Nfs pseudo system call for asynchronous i/o daemons.
 * These babies just pretend to be disk interrupt service routines
 * for client nfs. They are mainly here for read ahead/write behind.
 * Never returns unless it fails or gets killed
 */
nfs_async_daemon(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct buf	*bp, *dp;
	uthread_t		th = current_thread();
	int			error;
	int			dying=0;

	/*
	 * Must be super user
	 */
	PRIV_SUSER(u.u_cred, &u.u_acflag, SEC_REMOTE, 0, EPERM, error);
	if (error == 0) {
		ASYNCD_LOCK();
		if (nfs_asyncdaemons >= max_asyncdaemons)
			error = EBUSY;
		else
			++nfs_asyncdaemons;
		ASYNCD_UNLOCK();
	}
	NFSD_UNLOCK();
	if (error)
		return error;

	/*
	 * Just loop around doin our stuff until SIGKILL
	 */
	ASYNCD_LOCK();
	for (dp = &nfs_bqueue;;) {
		while (dp->b_actf == NULL) {
			if (dying)
				goto out;
			assert_wait(&asyncdaemon_requests, TRUE);
			ASYNCD_UNLOCK();
			thread_block();
			switch (th->uu_wait_result) {
			    case THREAD_TIMED_OUT:
				panic("async_daemon:  thread timed out");
				/* NOTREACHED */
			    case THREAD_INTERRUPTED:
			    case THREAD_SHOULD_TERMINATE:
				/*
				 * We've been interrupted!
				 * Make ourselves unavailable but
				 * handle requests until the queue empties.
				 * Then exit, above.  (NB:  may want to change
				 * this behavior to handle no more than one
				 * additional request to prevent indefinite
				 * postponement problems.)
				 */
				if (!dying++) {
					NFSD_LOCK();
					ASYNCD_LOCK();
					--nfs_asyncdaemons;
					ASYNCD_UNLOCK();
					NFSD_UNLOCK();
				}
				break;
			    default:
				break;
			}
			ASYNCD_LOCK();
		}
		/* Take one off the end of the list */
		ASSERT(asyncdaemon_requests > 0);
#ifdef notyet
		if (dying && (dying++ > NFS_NUMDYING))
			goto out;
#endif
		bp = dp->b_actl;
		if (bp->b_actl == dp) {
			dp->b_actf = dp->b_actl = (struct buf *)0;
		} else {
			dp->b_actl = bp->b_actl;
			bp->b_actl->b_actf = dp;
		}
		ASYNCD_UNLOCK();

		ASSERT(bp->b_flags&B_ASYNC);
		LASSERT(BUF_IS_LOCKED(bp));
		BUF_ACCEPT(bp);

		(void) nfs_doio(bp);

		ASYNCD_LOCK();
		--asyncdaemon_requests;
	}
out:
	ASYNCD_UNLOCK();
	return (0);
	/* NOTREACHED */
}
