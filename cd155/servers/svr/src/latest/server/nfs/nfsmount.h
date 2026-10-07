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
 * $Log: nfsmount.h,v $
 * Revision 1.5  1994/11/18  20:37:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:16:55  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:05  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:49  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:35:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:32:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:30:49  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:05  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:53:57  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:39:58  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:07:02  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:22:43  gm]
 * 
 * Revision 1.6  90/07/27  09:04:59  devrcs
 * 	NFS parallelization.
 * 
 * 	Condensed history, reverse chronology:
 * 	[90/07/20  17:04:38  nags]
 * 
 * 	Parallelized for OSF/1					nags@encore.com
 * 	Remove #defines for nm_host and nm_path			gmf@osf.org
 * 	Remove #defines for nm_host and nm_path			gmf@osf.org
 * 	Mount hang fix:  get rid of nm_host and nm_path.	gmf@osf.org
 * 	Cache server sockaddr in mntp for noconnect flag 	tmt@osf.org
 * 	Remove and/or rearrange req, mntp, and hostinfo entries	tmt@osf.org
 * 	Move socket handling code to nfs_socket. 		tmt@osf.org
 * 	Allow lock or spl synchronization			tmt@osf.org
 * 	Merged Encore parallelization				tmt@osf.org
 * 	New networking code from BSD.				tmt@osf.org
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
 *	@(#)nfsmount.h	7.1 (Berkeley) 7/5/89
 */

#ifndef	_NFSMOUNT_
#define	_NFSMOUNT_

/*
 * Mount structure.
 * One allocated on every nfs mount.
 * Holds nfs specific info for mount.
 */
struct	nfsmount {
	int	nm_flag;		/* Flags for soft/hard... */
	struct	mount *nm_mountp;	/* Vfs structure for this filesystem */
	nfsv2fh_t nm_fh;		/* File handle of root dir */
	struct	socket	*nm_so;		/* Rpc socket */
	struct	mbuf *nm_srvaddr;	/* Server address or NULL */
	struct	nfshost *nm_hostinfo;	/* Host and congestion information */
	short	nm_retry;		/* Max retry count */
	short	nm_rexmit;		/* Rexmit on previous request */
	short	nm_rtt;			/* Round trip timer ticks @ NFS_HZ */
	short	nm_rto;			/* Current timeout */
	short	nm_srtt;		/* Smoothed round trip time */
	short	nm_rttvar;		/* RTT variance */
	int	nm_rsize;		/* Max size of read rpc */
	int	nm_wsize;		/* Max size of write rpc */
#ifdef	_KERNEL
	udecl_simple_lock_data(,nm_lock)/* multiprocessor exclusion */
#endif
};

/*
 * Hostinfo/congestion structure.
 * One allocated per nfs server.
 * Holds host address, congestion limits, request count, etc.
 * Reference count is of nfsmounts which point to it.
 */
struct nfshost {
	struct	nfshost *nh_next, *nh_prev;
	short	nh_refcnt;		/* Reference count */
	short	nh_currto;		/* Current rto of any nfsmount */
	short	nh_currexmit;		/* Max rexmit count of nfsmounts */
	short	nh_sent;		/* Request send count */
	short	nh_window;		/* Request send window (max) */
	short	nh_winext;		/* Window incremental value */
	short	nh_ssthresh;		/* Slowstart threshold */
	short	nh_salen;		/* Actual length of nh_sockaddr */
	struct	mbuf *nh_sockaddr;	/* Address of server */
};

#ifdef	_KERNEL
/*
 * Nfsmount locking - protects the contents of the nfsmount structure.
 * The following fields are read-only:
 *	nm_fh
 *	nm_srvaddr
 *	nm_hostinfo
 *	nm_rsize
 *	nm_wsize
 */
#define	NFSMOUNT_LOCK(nmp)		usimple_lock(&(nmp)->nm_lock)
#define	NFSMOUNT_UNLOCK(nmp)		usimple_unlock(&(nmp)->nmlock)
#define	NFSMOUNT_LOCK_TRY(nmp)		usimple_lock_try(&(nmp)->nmlock)
#define	NFSMOUNT_LOCK_INIT(nmp)		usimple_lock_init(&(nmp)->nmlock)
#define	NFSMOUNT_LOCK_HOLDER(nmp)	SLOCK_HOLDER(&(nmp)->nmlock)

struct nfsmount *vfs_to_nfs();
#endif
#endif	_NFSMOUNT_
