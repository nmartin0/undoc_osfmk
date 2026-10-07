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
 * $Log: nfs.h,v $
 * Revision 1.5  1994/11/18  20:36:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:15:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:37:37  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:28:57  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:26  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:04  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:29:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:24:11  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:50:38  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:39:39  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.10.5.2  91/06/20  14:58:08  tmt
 * 	Add r_error to nfsreq (see nfs_socket.c).
 * 	[91/06/20  08:50:51  tmt]
 * 
 * Revision 1.10  90/10/07  14:38:42  devrcs
 * 	Added dependency file for dynamic recompiling with different compiler
 * 	[90/09/30  15:53:51  knight]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:20:41  gm]
 * 
 * Revision 1.9  90/08/24  12:15:21  devrcs
 * 	Track number of NFS daemons for unloading NFS.
 * 	[90/08/18  03:45:25  nags]
 * 
 * Revision 1.8  90/07/27  09:04:13  devrcs
 * 	NFS parallelization.
 * 	[90/07/20  17:02:51  nags]
 * 
 * Revision 1.7  90/05/13  18:45:08  devrcs
 * 	Add statistics for buffer cache code.
 * 	[90/05/01  11:08:56  tmt]
 * 
 * Revision 1.6  90/04/14  00:33:45  devrcs
 * 	Add signal mask, procp pointer for interrupts. Use _KERNEL
 * 	[90/04/09  16:02:46  tmt]
 * 
 * Revision 1.5  90/03/27  13:24:09  gm
 * 	added direofcache statistics
 * 	add direofcache* to nfsstats, but under ifdef notyet so we can
 * 	still use nfsstat.  Need to turn this on later.
 * 	Changes from George at Encore
 * 	[90/03/15  17:11:09  pam]
 * 
 * Revision cthon  90/02/12  13:05:48  tmt
 * 	Raise min timeout. Add timerinit field. Update from an old version!
 * 
 * Revision 1.4  90/01/18  08:48:34  gm
 * 	Remove and/or rearrange req, mntp, and hostinfo entries. Move socket
 * 	handling code to nfs_socket. Improve transport independence.
 * 	[90/01/02  14:39:02  tmt]
 * 
 * 	OSF/1 "one" snapshot revision.
 * 	[90/01/02  12:00:00  tmt]
 * 
 * 	- Base is BSD 4.4 (Alpha) networking.
 * 	- Encore multiprocessing merged in with some structural
 * 	  modifications to support flexible configuration.
 * 	- Glue for compiling and running in MACH or Unix 4.4 environments,
 * 	  lock testing under Unix, thread or software interrupt netisr's,
 * 	  locking and/or spl synchronization, single or multiple CPUs.
 * 	[89/12/20  12:00:00  tmt]
 * 
 * Revision 1.3  90/01/02  20:22:25  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:18:24  gm
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
 *	@(#)nfs.h	7.2 (Berkeley) 7/6/89
 */

#ifndef	_NFS_
#define	_NFS_

#if	MACH
#ifdef	_KERNEL
#include <kern/zalloc.h>
#include <nfs.h>				/* XXX dependency file */
					/* dynamic define dependancy */
#endif
#endif

/*
 * Tunable constants for nfs
 */

#define	NFS_MAXIOVEC	34
#define NFS_HZ		10		/* Ticks per second for NFS timeouts */
#define	NFS_TIMEO	(1*NFS_HZ)	/* Default timeout = 1 second */
#define	NFS_MINTIMEO	(NFS_HZ/2)	/* Min timeout to use */
#define	NFS_MAXTIMEO	(60*NFS_HZ)	/* Max timeout to backoff to */
#define	NFS_MAXREXMIT	100		/* Stop counting after this many */
#define	NFS_MAXWINDOW	1024		/* Max number of outstanding requests */
#define	NFS_RETRANS	10		/* Num of retrans for soft mounts */
#define NFS_FISHY	6		/* Host not responding at this count */
#define	NFS_ATTRTIMEO	5		/* Attribute cache timeout in sec */
#define	NFS_WSIZE	8192		/* Def. write data size <= 8192 */
#define	NFS_RSIZE	8192		/* Def. read data size <= 8192 */
#define	NFS_MAXREADDIR	NFS_MAXDATA	/* Max. size of directory read */
#define	NFS_MAXASYNCDAEMON 20		/* Max. number async_daemons runnable */
#define	NMOD(a)		((a) % nfs_asyncdaemons)

/*
 * The set of signals the interrupt an I/O in progress for NFSMNT_INT mounts.
 * What should be in this set is open to debate, but I believe that since
 * I/O system calls on ufs are never interrupted by signals the set should
 * be minimal. My reasoning is that many current programs that use signals
 * such as SIGALRM will not expect file I/O system calls to be interrupted
 * by them and break.
 */
#define NFSINT_SIGMASK	(sigmask(SIGHUP)|sigmask(SIGINT)|sigmask(SIGQUIT)| \
			 sigmask(SIGKILL)|sigmask(SIGTERM))


/*
 * Nfs outstanding request list element
 */
struct nfsreq {
	struct nfsreq	*r_next;
	struct nfsreq	*r_prev;
	struct mbuf	*r_mreq;
	struct mbuf	*r_mrep;
	struct nfsmount *r_mntp;
	struct vnode	*r_vp;
	struct proc	*r_procp;
	int		r_msiz;
	int		r_error;
	u_long		r_xid;
	short		r_flags;	/* flags on request, see below */
	short		r_retry;	/* max retransmission count */
	short		r_rexmit;	/* current retrans count */
	short		r_timer;	/* tick counter on reply */
	short		r_timerinit;	/* reinit tick counter on reply */
};

/* Flag values for r_flags */
#define R_TIMING	0x01		/* timing request (in mntp) */
#define R_SENT		0x02		/* request has been sent */

#ifdef	_KERNEL
/*
 * Silly rename structure that hangs off the nfsnode until the name
 * can be removed by nfs_inactive()
 */
struct sillyrename {
	int	s_flag;
	nfsv2fh_t s_fh;
	struct nameidata s_namei;
#if	MACH
	struct  utask_nd s_utnd;
#endif
};

/* And its flag values */
#define REMOVE		0
#define	RMDIR		1
#endif	/* _KERNEL */

/*
 * Stats structure
 */
struct nfsstats {
	int	attrcache_hits;
	int	attrcache_misses;
	int	lookupcache_hits;
	int	lookupcache_misses;
	int	direofcache_hits;
	int	direofcache_misses;
	int	biocache_reads;
	int	read_bios;
	int	read_physios;
	int	biocache_writes;
	int	write_bios;
	int	write_physios;
	int	biocache_readlinks;
	int	readlink_bios;
	int	biocache_readdirs;
	int	readdir_bios;
	int	rpccnt[NFS_NPROCS];
	int	rpcretries;
	int	srvrpccnt[NFS_NPROCS];
	int	srvrpc_errs;
	int	srv_errs;
	int	rpcrequests;
	int	rpctimeouts;
	int	rpcunexpected;
	int	rpcinvalid;
	int	srvcache_inproghits;
	int	srvcache_idemdonehits;
	int	srvcache_nonidemdonehits;
	int	srvcache_misses;
	int	srvcache_reqdrops;
};

#ifdef _KERNEL

extern struct nfsstats nfsstats;
#define	NFS_STATS(clause)	STATS_ACTION(&nfs_stats_lock, (clause))
vdecl_simple_lock_data(extern,nfs_stats_lock)

extern struct buf nfs_bqueue;		/* buffers awaiting async I/O */
extern int nfs_nfsds;			/* # of server daemons */

extern int nfs_asyncdaemons;		/* # of I/O threads that exist */
extern int asyncdaemon_requests;	/* requests on nfs_bqueue */
udecl_simple_lock_data(extern,asyncdaemon_lock)
#define	ASYNCD_LOCK()		usimple_lock(&asyncdaemon_lock)
#define	ASYNCD_UNLOCK()		usimple_unlock(&asyncdaemon_lock)
#define	ASYNCD_LOCK_INIT()	usimple_lock_init(&asyncdaemon_lock)

#if	!MACH_ASSERT
#define	nfs_nput(vp)	vrele(vp)
#endif

#if	MACH
extern	zone_t	sillyrename_zone;
extern	zone_t	nfsreq_zone;
extern	zone_t	nfsmount_zone;
extern	zone_t	nfsreaddir_zone;
#endif

#endif	/* _KERNEL */
#endif	/* _NFS_ */
