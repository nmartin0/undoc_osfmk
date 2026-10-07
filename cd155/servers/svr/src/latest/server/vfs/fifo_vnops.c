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
 * HISTORY
 * $Log: fifo_vnops.c,v $
 * Revision 1.7  1995/02/01  22:32:12  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.6  1994/11/18  20:50:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/01/11  18:24:42  jlitvin
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
 * Revision 1.4  1993/07/14  18:45:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:08:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:31:33  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:23  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:57:23  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:45:57  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:56:57  cfj
 * Bump major revision number.
 *
 * Revision 2.10  1992/10/22  16:06:14  dbm
 * Added PFS functionality.
 *
 * Revision 2.9  1992/10/05  15:41:20  klh
 * 	Revision 2.6  92/09/20  11:26:09  roy
 * 		Another op for OSF1_ADFS.
 * 		[92/09/15            roy]
 *
 * 	Revision 2.5  92/08/26  12:14:27  loverso
 * 		Additional ops for OSF1_ADFS.
 * 		[92/08/19            roy]
 *
 * Revision 2.8  92/09/28  13:35:59  klh
 * Use V_STRG(vp) macro instead of testing VS_XX bit (klh for mjl).
 * 
 * Revision 2.7  92/09/24  17:07:08  bhk
 * changed order of includes to allow a clean compile
 * 
 * Revision 2.6  92/08/19  08:26:23  klh
 * Remove calls to debugging macro WATCH_VP (per mjl)
 * 
 * Revision 2.5  92/08/17  13:26:50  mjl
 * Add TNC FIFO relocation hooks.
 * 
 * Revision 2.4  92/08/13  13:22:41  klh
 * include sys/so_defs.h for non TNC case
 * 
 * Revision 2.3  92/08/08  01:54:11  jdh
 * fifo change (mjl) -- jdh
 * 
 * Revision 2.2  91/08/31  14:30:03  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/01  17:02:17  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12  90/10/31  14:08:24  devrcs
 * 	Set both sides of newly-created fifo to CANTSENDMORE
 * 	and CANTRCVMORE.  Flags are turned off when fifo
 * 	is opened for reading/writing.  Fixes PCTS bug with
 * 	read of nonblock fifo with no writers.
 * 	[90/10/20  17:36:50  lwa]
 * 
 * 	Seperate NDELAY and NONBLOCK.
 * 	[90/10/12  10:18:57  jvs]
 * 
 * 	Call spec_seek on seek operations.
 * 	[90/10/08  17:12:31  collins]
 * 
 * Revision 1.11  90/10/07  15:00:07  devrcs
 * 	Fix sense of soreadable test. Comment out printf.
 * 	[90/10/03  15:17:05  tmt]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:54:28  gm]
 * 
 * 	Add FIFO_LOCK_SOCKET macro to properly protect sockets.
 * 	Use new MSG_NONBLOCK flag on sosend/soreceive.
 * 	[90/09/29  17:16:19  tmt]
 * 
 * 	Posix/AES: fix fifo_read to leave SS_CANTRCVMORE alone.
 * 	[90/09/27  13:45:12  tmt]
 * 
 * 	Make FIFO write operations atomic as for pipes.
 * 	[90/09/20  18:40:58  tmt]
 * 
 * Revision 1.10  90/09/23  16:01:16  devrcs
 * 	Fixed typo in tmt's FNDELAY support in fifo_write().
 * 	[90/09/06  15:32:03  ers]
 * 
 * 	Add macros to "hide" socket internals. TODO: finish locking
 * 	with funnels, pass NBIO flag to sosend/soreceive.
 * 	[90/09/05  17:40:13  tmt]
 * 	Add FNDELAY support. Add fifo_ioctl(). Use tsleep().
 * 	[90/09/04  10:55:09  tmt]
 * 
 * Revision 1.9  90/09/13  11:51:49  devrcs
 * 	Temporarily bump the vnode write count when returning ENXIO in
 * 	fifo_open.   fifo_close will decrement the count.
 * 	[90/08/31  16:27:14  noemi]
 * 
 * Revision 1.8  90/08/24  12:29:53  devrcs
 * 	Remove setjmp dependencies
 * 	[90/08/20  03:47:16  gmf]
 * 
 * Revision 1.7  90/07/27  09:09:31  devrcs
 * 	VOP_OPEN changes for clone driver support.
 * 	[90/07/20  17:08:42  nags]
 * 
 * Revision 1.6  90/06/22  20:56:13  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant history, reverse chronology:
 * 	   Delete extern uipc reference use stat info      tmt@osf.org
 * 	   Changed for new select interface		   coren@osf.org
 * 	   Parallelized for OSF/1.                         nags@encore.com
 * 	   Support for FIFOs (named pipes).                ers@osf.org
 * 	[90/06/12  21:42:48  gmf]
 * 
 * $EndLog$
 */
/*
 * FIFO (named pipe) vnode operations
 */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/uio.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/mbuf.h>

#ifdef TNC
#include <vsocket/vsocket.h>
#else
#include <sys/so_defs.h>
#endif	/* TNC */

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/unpcb.h>
#include <sys/poll.h>
#include <kern/macro_help.h>
#ifndef	OSF1_SERVER
#include <kern/thread.h>
#include <kern/sched_prim.h>
#endif	/* OSF1_SERVER */
#include <kern/parallel.h>

/*
 * We synchronize fifo_open and fifo_close to avoid several races.
 * Some races may only occur in a multiprocessor envirnoment.
 * But there are also races in these two functions that can also
 * occur in the uniprocessor case.  We must avoid races which occur when
 * several threads are racing to open and close FIFOs.  We do not have
 * a convenient data structure to lock and we don't want to add a
 * blocking lock to the vnode to be used just by FIFOs.  So we use
 * two flags in the vnode (VFLOCK and VFWAIT) for synchronization.
 * The vnode v_rdcnt, v_wrcnt, and v_socket fields for FIFOs are only
 * changed in fifo_open and fifo_close under VFLOCK.
 */ 

#define FIFO_LOCK(vp)						\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	while (vp->v_flag & VFLOCK) {				\
		vp->v_flag |= VFWAIT;				\
		assert_wait((int)&FIFO_WRITE_SOCKET(vp), FALSE);\
		VN_UNLOCK(vp);					\
		(void)tsleep((caddr_t)0, PZERO, "fifo_lock", 0);\
		VN_LOCK(vp);					\
	}							\
	vp->v_flag |= VFLOCK;					\
	VN_UNLOCK(vp);						\
MACRO_END

#define FIFO_UNLOCK(vp)						\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	if (vp->v_flag & VFWAIT) {				\
		vp->v_flag &= ~(VFWAIT|VFLOCK);			\
		VN_UNLOCK(vp);					\
		wakeup((caddr_t)&FIFO_WRITE_SOCKET(vp));	\
	} else {						\
		vp->v_flag &= ~VFLOCK;				\
		VN_UNLOCK(vp);					\
	}							\
MACRO_END

/*
 * The FIFO_LOCK and open reference protect connections.
 */
#define FIFO_WRITE_SOCKET(vp) ((vp)->v_socket)
#define FIFO_READ_SOCKET(vp)  (sotounpcb((vp)->v_socket)->unp_conn->unp_socket)

/*
 * In order to lock a socket, we must go through the socket funnel first.
 * This looks like a lot, but it's usually just an spl or a no-op.
 */
#define FIFO_LOCK_SOCKET(so)			\
	DOMAIN_FUNNEL_DECL(f)			\
	DOMAIN_FUNNEL(sodomain(so), f);		\
	SOCKET_LOCK(so)				\

#define FIFO_UNLOCK_SOCKET(so)			\
	SOCKET_UNLOCK(so);			\
	DOMAIN_UNFUNNEL(f)			\

#define FIFO_CANSEND(so)			\
MACRO_BEGIN					\
	FIFO_LOCK_SOCKET(so);			\
	(so)->so_state &= ~SS_CANTSENDMORE;	\
	FIFO_UNLOCK_SOCKET(so);			\
MACRO_END

#define FIFO_CANTSEND(so)			\
MACRO_BEGIN					\
	FIFO_LOCK_SOCKET(so);			\
	socantsendmore(so);			\
	FIFO_UNLOCK_SOCKET(so);			\
MACRO_END

#define FIFO_CANRECV(so)			\
MACRO_BEGIN					\
	FIFO_LOCK_SOCKET(so);			\
	(so)->so_state &= ~SS_CANTRCVMORE;	\
	FIFO_UNLOCK_SOCKET(so);			\
MACRO_END

#define FIFO_CANTRECV(so)			\
MACRO_BEGIN					\
	FIFO_LOCK_SOCKET(so);			\
	socantrcvmore(so);			\
	FIFO_UNLOCK_SOCKET(so);			\
MACRO_END

int fifo_open();
int fifo_close();
int fifo_read();
int fifo_write();
int fifo_ioctl();
int fifo_getattr();
int fifo_select();
int fifo_print();

extern int spec_lookup();
extern int spec_badop();
extern int spec_ebadf();
extern int spec_nullop();
extern int spec_seek();

struct vnodeops fifo_vnodeops = {
	spec_lookup,		/* lookup */
	spec_badop,		/* create */
	spec_badop,		/* mknod */
	fifo_open,		/* open */
	fifo_close,		/* close */
#ifdef	TNC
	/* XXX Temporarily allow anyone access to fifos.  FIX THIS!!! */
	spec_nullop,		/* access */
#else
	spec_ebadf,		/* access */
#endif
	fifo_getattr,		/* getattr */
	spec_ebadf,		/* setattr */
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
	spec_nullop,		/* inactive */
	spec_nullop,		/* reclaim */
	spec_ebadf,		/* bmap */
	spec_ebadf,		/* strategy */
	fifo_print,		/* print */
#if	MACH
	spec_badop,		/* page_read */
	spec_badop,		/* page_write */
#endif	
#ifdef	PFS
	spec_badop,		/* preallocate and set size */
#endif
#ifdef	OSF1_ADFS
	spec_badop,		/* pagein */
	spec_badop,		/* pageout */
	spec_badop,		/* alloc */
	spec_badop,		/* update */
	spec_badop,		/* getsize */
#endif
};


#ifdef	TNC
#include <tnc/un_ff.h>
/*
 *  Restart vnode operations on FIFOs that are midst of relocating,
 *  or have relocated since the current syscall did a namei().
 */
#define TNC_CHECK_RESTART(vp)					\
MACRO_BEGIN							\
	VN_LOCK(vp);						\
	if (vp->v_flag & VRELOCATING ||				\
	    (vp->v_tncdata && ! V_STRG(vp))) {			\
		VN_UNLOCK(vp);					\
		return (ERESTART);				\
	}							\
	VN_UNLOCK(vp);						\
MACRO_END
#else	/* ! TNC */
#define	TNC_CHECK_RESTART(vp)	/*no-op*/
#endif	/* ! TNC */

/*
 * Open a fifo.
 * Check whether a socket pair already exists for this fifo.  If
 * so, use it, otherwise create one.
 */
/*ARGSUSED*/
int
fifo_open(vpp, mode, cred)
	struct vnode **vpp;
	int mode;
	struct ucred *cred;
{
	struct socket *rso;
	struct socket *wso;
	register struct vnode *vp = *vpp;
	int error;

	TNC_CHECK_RESTART(vp);

	FIFO_LOCK(vp);

	if ((wso = FIFO_WRITE_SOCKET(vp)) == NULL) {
		/* No readers or writers yet.
		 *
		 * Since this is a one-way pipe, it seems wasteful
		 * to allocate to socket structures.  One socket never
		 * uses its receive queue, and the other never uses its
		 * send queue.  I would prefer to allocate one and
		 * have unp_conn refer to itself.
		 * However, I'm not sure whether the socket locking
		 * can handle this, so for now I'll be safe and use
		 * two sockets.
		 *
		 * (Actually, there are other reasons, all of them
		 *  uipc socket internals. Two sockets are best.)
		 */
		if (error = SOCREATE(AF_UNIX, &wso, SOCK_STREAM, VOS_FIFO)) {
			FIFO_UNLOCK(vp);
			return(error);
		}

		if (error = SOCREATE(AF_UNIX, &rso, SOCK_STREAM, VOS_FIFO)) {
			(void)soclose(wso);
			FIFO_UNLOCK(vp);
			return(error);
		}

		/*
		 * We shouldn't need to lock these sockets because we
		 * just created them.
		 */
		wso->so_state |= SS_CANTSENDMORE|SS_CANTRCVMORE;
		wso->so_special |= SP_WATOMIC;	/* Atomic behavior like pipes */
		rso->so_state |= SS_CANTSENDMORE|SS_CANTRCVMORE;
		if (error = soconnect2(wso, rso)) {
			(void)soclose(wso);
			(void)soclose(rso);
			FIFO_UNLOCK(vp);
			return(error);
		}
		FIFO_WRITE_SOCKET(vp) = wso;
	}
	error = 0;
	rso = FIFO_READ_SOCKET(vp);
	if (mode & FREAD) {
		if (vp->v_rdcnt++ == 0) {
			FIFO_CANSEND(wso);
			/* Wake up any waiting writers */
			if (vp->v_wrcnt)
				wakeup((caddr_t) &vp->v_rdcnt);
		}
	}
	if (mode & FWRITE) {
		/* was (mode & FNDELAY) */
		if ((vp->v_rdcnt == 0) && (mode & (FNDELAY|FNONBLOCK))) {
			/*
			 * We need to temporarily increment the vnode
			 * writer count here because fifo_close will
			 * decrement the count.
			 */
			vp->v_wrcnt++;
			error = ENXIO;
			goto out;
		}
		if (vp->v_wrcnt++ == 0) {
			FIFO_CANRECV(rso);
			/* Wake up any waiting readers */
			if (vp->v_rdcnt)
				wakeup((caddr_t) &vp->v_wrcnt);
		}
	}
	if (mode & FREAD) {
		/*
		 * If FNDELAY is set or there is data in the pipe,
		 * return.
		 */
		/* was (mode & FNDELAY) */
		if ((vp->v_wrcnt == 0) && (mode & (FNDELAY|FNONBLOCK)))
			goto out;
		while (vp->v_wrcnt == 0) {
			int rstat;
			FIFO_LOCK_SOCKET(rso);
			SOCKBUF_LOCK(&rso->so_rcv);
			/*
			 * Block if there is no data in the pipe.
			 * Can't use soreadable() here as it will say
			 * readable if no writers.
			 */
			rstat = (rso->so_rcv.sb_cc > 0);
			if (rstat == 0)
				assert_wait((int)&vp->v_wrcnt, TRUE);
			SOCKBUF_UNLOCK(&rso->so_rcv);
			FIFO_UNLOCK_SOCKET(rso);
			FIFO_UNLOCK(vp);
			if (rstat || (error = tsleep((caddr_t)0,
					(PZERO+1)|PCATCH, "fifo_ropen", 0)))
				goto out2;
			FIFO_LOCK(vp);
		}
	}
	if (mode & FWRITE) {
		while (vp->v_rdcnt == 0) {
			assert_wait((int)&vp->v_rdcnt, TRUE);
			FIFO_UNLOCK(vp);
			error = tsleep((caddr_t)0, (PZERO+1)|PCATCH,
					"fifo_wopen", 0);
			if (error)
				goto out2;
			FIFO_LOCK(vp);
		}
	}
out:
	FIFO_UNLOCK(vp);
out2:
#ifdef	TNC
	un_ff_open(vp, mode, error);
#endif
	if (error)
		(void) fifo_close(vp, mode & FMASK, cred);
	return (error);
}

/*
 * Close called
 */
/* ARGSUSED */
fifo_close(vp, fflag, cred)
	struct vnode *vp;
	int fflag;
	struct ucred *cred;
{
	register struct socket *wso = FIFO_WRITE_SOCKET(vp);
	struct socket *rso;


#ifdef	TNC
	un_ff_close(vp);
#endif
	if (wso == NULL)
		return (0);
	FIFO_LOCK(vp);
	rso = FIFO_READ_SOCKET(vp);
	if (fflag & FREAD) {
		ASSERT(vp->v_rdcnt > 0);
		if (--vp->v_rdcnt == 0)
			FIFO_CANTSEND(wso);
	}
	if (fflag & FWRITE) {
		ASSERT(vp->v_wrcnt > 0);
		if (--vp->v_wrcnt == 0)
			FIFO_CANTRECV(rso);
	}
	if (vp->v_wrcnt == 0 && vp->v_rdcnt == 0) {
		(void)soclose(wso);
		(void)soclose(rso);
		FIFO_WRITE_SOCKET(vp) = NULL;
	}
	FIFO_UNLOCK(vp);
	return (0);
}

/*
 * Vnode op for reading.
 */
/* ARGSUSED */
fifo_read(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct socket *wso = FIFO_WRITE_SOCKET(vp);
	register struct unpcb *unp;
	register struct socket *rso;
	int flags = 0;
	
	TNC_CHECK_RESTART(vp);

	if (wso == NULL)
		panic("fifo_read: no sock");
	if (uio->uio_resid == 0)
		return (0);
	unp = sotounpcb(wso);
	if (unp->unp_conn == NULL || (rso = unp->unp_conn->unp_socket) == NULL) 
		panic("fifo_read: not connected");
	if (ioflag & IO_NDELAY)
		flags = MSG_NONBLOCK;
	return soreceive(rso, (struct mbuf **)0,
		uio, (struct mbuf **)0, (struct mbuf **)0, &flags);
}

/*
 * Vnode op for writing.
 */
/* ARGSUSED */
fifo_write(vp, uio, ioflag, cred)
	struct vnode *vp;
	struct uio *uio;
	int ioflag;
	struct ucred *cred;
{
	register struct socket *wso = FIFO_WRITE_SOCKET(vp);
	int flags = 0;
	
	TNC_CHECK_RESTART(vp);

	if (wso == NULL)
		panic("fifo_write: no sock");
	if (ioflag & IO_NDELAY)
		flags = MSG_NONBLOCK;
	return sosend(wso, (struct mbuf *)0,
		uio, (struct mbuf *)0, (struct mbuf *)0, flags);
}

/*
 * Vnode op for ioctl.
 */
/* ARGSUSED */
fifo_ioctl(vp, com, data, fflag, cred)
	struct vnode *vp;
	int com;
	caddr_t data;
	int fflag;
	struct ucred *cred;
{
	register struct socket *wso = FIFO_WRITE_SOCKET(vp);
	register struct unpcb *unp;
	struct file ftemp;

	TNC_CHECK_RESTART(vp);

	if (com == FIONBIO)	/* Handled each r/w */
		return (0);
	if (fflag & FREAD) {
                unp = sotounpcb(wso);
                if (unp->unp_conn == NULL ||
		    (ftemp.f_data = (caddr_t)unp->unp_conn->unp_socket) == NULL)
                        panic("fifo_ioctl: not connected");
	} else
		ftemp.f_data = (caddr_t)wso;
	return (soo_ioctl(&ftemp, com, data));
}

/*
 * Vnode getattr op.  Just fill in the fields that we know about.
 */
/* ARGSUSED */
fifo_getattr(vp, vap, cred)
	struct vnode *vp;
	register struct vattr *vap;
	struct ucred *cred;
{
	register struct socket *wso = FIFO_WRITE_SOCKET(vp);
	struct stat sb;
	int error;

	TNC_CHECK_RESTART(vp);

	if (wso == NULL) {
		vap->va_size = vap->va_bytes = 0;
		vap->va_blocksize = 0;
	} else {
		if (error = soo_stat(wso, &sb))
			return (error);
		vap->va_size = vap->va_bytes = sb.st_size;
		vap->va_blocksize = sb.st_blksize;
	}
	vap->va_size_rsv = 0;
	return(0);
}

/*
 * Vnode select op.  Just use soo_select to do the work.
 */
/* ARGSUSED */
fifo_select(vp, events, revents, scanning, cred)
	struct vnode *vp;
	short *events, *revents;
        int scanning;
	struct ucred *cred;
{
	register struct socket *so = FIFO_WRITE_SOCKET(vp);
	struct unpcb *unp;
	struct file ftemp;

	TNC_CHECK_RESTART(vp);

	if (so == NULL)
		panic("fifo_select: no sock");
	if ((scanning) && (*events & POLLNORM)) {
		unp = sotounpcb(so);
		if (unp->unp_conn == NULL || (so = unp->unp_conn->unp_socket) == NULL)
			panic("fifo_select: not connected");
	}
	ftemp.f_data = (caddr_t)so;
	soo_select(&ftemp, events, revents, scanning);
	return (0);
}

/*
 * Print out the contents of a fifo.
 */
fifo_print(vp)
	struct vnode *vp;
{
	/*printf("Nothing to say about this fifo");*/
	return(0);
}
