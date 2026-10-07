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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * This source file was  modified and extended by the Center  
 * for High Performance Computing ("CHPC") on behalf of OSF."
 */
/*
 * HISTORY
 * $Log: namei.h,v $
 * Revision 1.5  1994/11/18  20:40:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:26:17  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:17  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:19:38  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:42:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:37  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:31  cfj
 * Bump major revision number.
 *
 * Revision 1.1.1.1  1993/05/03  17:42:02  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.8  1992/12/08  10:45:50  durriya
 * 	1.1 unmount sync changes - Eliminated BYPASSVP flag and ni_bypassvp
 * 	definition.	Eliminated some misplaced comments. (durriya)
 *
 * Revision 2.7  92/05/12  00:06:10  loverso
 * 	Get rid of ni_vnode1, ni_vp1 and ni_vnodeport1 (pjg).
 * 
 * Revision 2.6  92/04/05  17:00:37  pjg
 * 	Replaced struct vport with vnode proxies and changed all
 * 	uses of it accordingly.
 * 
 * Revision 2.5  92/03/20  11:39:05  pjg
 * 	Got rid of ni_credsport. The creds port is stored only in the
 * 	{dummy} proc in p->p_cred.
 * 
 * Revision 2.4  92/03/01  18:38:54  pjg
 * 	1992/03/01  pjg
 * 	Use vnode proxies for the cwd and root directories. Change the
 * 	definitions of ni_cdir, ni_rdir, ni_cdirport and ni_rdirport
 * 	accordingly.
 * 
 * 	1992/03/01  loverso
 * 	struct servermsg is no more.
 * 
 * Revision 2.3  92/01/05  20:08:25  roy
 * 	1991/10/14  19:32:24  noemi
 * 	Changed some fields in nameidata and added others.  Also, replaced the
 * 	BYPASSVP option with the MOUNT option.  Broke HASBUF into two cases:
 * 	HASCOMPBUF and HASPATHBUF.
 * 
 * 	1991/09/21  20:23:47  noemi
 * 	Added ports for OSF1_ADFS and MOUNT define.
 * 
 * Revision 2.2  91/08/31  14:09:15  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.5  91/07/31  15:44:02  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.9  90/10/31  14:06:46  devrcs
 * 	Added namei option to "bypass" a vnode during a mount:
 * 	effectively a NOCROSSMOUNT option that applies only to
 * 	the specified vnode.
 * 	[90/10/24  14:43:54  nags]
 * 
 * 	Added STRIPSLASH namei operation.
 * 	[90/10/13  12:50:14  gmf]
 * 
 * Revision 1.8  90/10/07  14:52:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:42:52  gm]
 * 
 * Revision 1.7  90/07/27  09:07:32  devrcs
 * 	Preallocated pathname buffer option (HASBUF).
 * 	[90/07/20  17:06:47  nags]
 * 
 * Revision 1.6  90/07/17  11:42:16  devrcs
 * 	Changed struct direct to dirent.
 * 	[90/07/07  21:49:49  gm]
 * 
 * Revision 1.5  90/06/22  20:53:32  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Secureware changes.				seiden@osf.org
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Added support for system V filesystem.		morris@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	ni_segflg ==> ni_uioseg (compiler confusion).	gmf@osf.org
 * 	Merge early Mach 2.5, Encore parallelization.	alan@encore.com
 * 	More cleanup.					rpd@cmu.edu
 * 	Made variable declarations use "extern".	rpd@cmu.edu
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Include dir.h for struct direct.		mwyoung@cmu.edu
 * 	Updraged to 4.3.				avie@cmu.edu
 * 	[90/06/12  21:38:55  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1985, 1989 Regents of the University of California.
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
 *	@(#)namei.h	7.6 (Berkeley) 8/25/89
 */

#ifndef	_SYS_NAMEI_H_
#define _SYS_NAMEI_H_

#ifdef	_KERNEL
#include <sys/unix_defs.h>
#endif

#include <sys/types.h>
#include <sys/uio.h>
#ifdef OSF1_ADFS
#include <sys/vnode.h>
#endif
#include <ufs/dir.h>
#include <s5fs/s5dir.h>
#ifdef	_KERNEL
#include <kern/zalloc.h>
#endif


/*
 * Encapsulation of namei parameters.
 * One of these is located in the u. area to
 * minimize space allocated on the kernel stack.
 */

struct nameidata {
		/* arguments to namei and related context: */
	caddr_t	ni_dirp;		/* pathname pointer */
	enum	uio_seg ni_segflg;	/* location of pathname */
#ifdef	OSF1_ADFS
	long	ni_nameiop;		/* see below */
#else
	short	ni_nameiop;		/* see below */
#endif
#if	MACH
	struct	utask_nd *ni_utnd;	/* utask nameidata structure */

#ifdef	OSF1_ADFS
#define ni_cdir		ni_utnd->utnd_cdir.vpx_vnode
#define ni_cdirport	ni_utnd->utnd_cdir.vpx_port
#define ni_cdirproxy	ni_utnd->utnd_cdir
#define ni_rdir		ni_utnd->utnd_rdir.vpx_vnode
#define ni_rdirport	ni_utnd->utnd_rdir.vpx_port
#define ni_rdirproxy	ni_utnd->utnd_rdir
#else	/* OSF1_ADFS */
#define ni_cdir		ni_utnd->utnd_cdir
#define ni_rdir		ni_utnd->utnd_rdir
#endif	/* OSF1_ADFS */
#else	/* MACH */
	struct	vnode *ni_cdir;		/* current directory */
	struct	vnode *ni_rdir;		/* root directory, if not normal root */
#endif
	struct	ucred *ni_cred;		/* credentials */

		/* shared between namei, lookup routines and commit routines: */
	caddr_t	ni_pnbuf;		/* pathname buffer */
	char	*ni_ptr;		/* current location in pathname */
	char	*ni_next;		/* next location in pathname */
	u_int	ni_pathlen;		/* remaining chars in path */
	u_long	ni_hash;		/* hash value of current component */
	short	ni_namelen;		/* length of current component */
	short	ni_loopcnt;		/* count of symlinks encountered */
	char	ni_makeentry;		/* 1 => add entry to name cache */
	char	ni_isdotdot;		/* 1 => current component name is .. */
#ifdef	OSF1_ADFS
	char	ni_allocbuf;		/* 1 => allocated a pathname buffer for
					 * a symlink with HASPATHBUF option
					 */
#endif
		/* results: */
	struct	vnode *ni_vp;		/* vnode of result */
	struct	vnode *ni_dvp;		/* vnode of intermediate directory */
	union {
		struct	dirent ni_ufsdent;	/* final component name */
		struct	gpdirect ni_gpfsdent;	/* general purpose final name */
	} ni_dir;

		/* side effects: */
	/* BEGIN UFS SPECIFIC */
	off_t	ni_endoff;		/* end of useful directory contents */
	struct ndirinfo {		/* saved info for new dir entry */
		struct	iovec nd_iovec;		/* pointed to by ni_iov */
		struct	uio nd_uio;		/* directory I/O parameters */
	} ni_nd;
	/* END UFS SPECIFIC */

	long	ni_dirstamp;		/* for directories */
	int	ni_nchtimestamp;	/* for name cache enters */

#ifdef	OSF1_ADFS
	mach_port_t	ni_forwport;	/* port for forwarding message */
	struct vnode_proxy ni_vnode2;	/* start port & vp for 2nd path */
#define	ni_vnodeport2	ni_vnode2.vpx_port
#define ni_vp2		ni_vnode2.vpx_vnode
#endif	/* OSF1_ADFS */

#if	UNIX_LOCKS
	u_long	ni_vpid;		/* capability of result's vnode */
#else
	u_long	dummy_ni_vpid;		/* padding space to conserve size */
#endif
};

#define	ni_iovec	ni_nd.nd_iovec
#define	ni_base		ni_nd.nd_iovec.iov_base
#define	ni_count	ni_nd.nd_iovec.iov_len
#define	ni_uioseg	ni_nd.nd_uio.uio_segflg
#define	ni_iov		ni_nd.nd_uio.uio_iov
#define	ni_iovcnt	ni_nd.nd_uio.uio_iovcnt
#define	ni_offset	ni_nd.nd_uio.uio_offset
#define	ni_resid	ni_nd.nd_uio.uio_resid
#define	ni_rw		ni_nd.nd_uio.uio_rw
#define	ni_uio		ni_nd.nd_uio
#define ni_dent		ni_dir.ni_ufsdent
#define ni_gpdent	ni_dir.ni_gpfsdent

#ifdef	_KERNEL
/*
 * namei operations and modifiers
 */
#define LOOKUP		0x0000	/* perform name lookup only */
#define CREATE		0x0001	/* setup for file creation */
#define DELETE		0x0002	/* setup for file deletion */
#define	RENAME		0x0003	/* setup for file renaming */
#define	OPFLAG		0x0003	/* mask for operation */
#define	WANTPARENT	0x0010	/* want parent vnode returned unlocked */
#define NOCACHE		0x0020	/* name must not be left in cache */
#define FOLLOW		0x0040	/* follow symbolic links */
#define NOFOLLOW	0x0000	/* don't follow symbolic links (pseudo) */
#define	NOCROSSMOUNT	0x0080	/* do not cross mount points */
#define	REMOTE		0x0100	/* lookup for remote filesystem servers */
#ifndef	OSF1_ADFS
#define	HASBUF		0x0200	/* has preallocated pathname buffer */
#endif
#define	STRIPSLASH	0x0400	/* strip trailing slashes */
#if	SEC_MAC
#define	MLDCREATE	0x4000	/* creating mld subdirectory */
#endif
#ifdef	OSF1_ADFS
#define MOUNT		0x00010000	/* look up a device for mounting */
#define	HASCOMPBUF	0x00020000	/* has path buf for 1 component */
#define	HASPATHBUF	0x00040000	/* has path buf for entire path */
#endif
#endif	/* _KERNEL */

/*
 * This structure describes the elements in the cache of recent
 * names looked up by namei.
 */

#define	NCHNAMLEN	15	/* maximum name segment length we bother with */
#define	NCH_NULL	((struct nchash *) 0)

struct	namecache {
	struct	namecache *nc_forw;	/* hash chain, MUST BE FIRST */
	struct	namecache *nc_back;	/* hash chain, MUST BE FIRST */
	struct	namecache *nc_nxt;	/* LRU chain */
	struct	namecache **nc_prev;	/* LRU chain */
	struct	vnode *nc_dvp;		/* vnode of parent of name */
	u_long	nc_dvpid;		/* capability number of nc_dvp */
	struct	vnode *nc_vp;		/* vnode the name refers to */
	u_long	nc_vpid;		/* capability number of nc_vp */
	char	nc_nlen;		/* length of name */
	char	nc_name[NCHNAMLEN];	/* segment name */
#if	UNIX_LOCKS
	struct	nchash *nc_hash_chain;	/* header of my hash chain */
#endif
};

#ifdef	_KERNEL
/*
 * Namei cache hash chain header.
 */
struct nchash {
	union nchash_u {
		struct	nchash *nch_u_head[2];
		struct	namecache *nch_u_chain[2];
	} nch_u;
	int	nch_timestamp;
	udecl_simple_lock_data(, nch_lock)
};

#define nch_head	nch_u.nch_u_head
#define nch_chain	nch_u.nch_u_chain
#define	nch_forw	nch_chain[0]
#define	nch_back	nch_chain[1]

extern struct	namecache *namecache;
extern int	nchsize;
#define MINNCHSZ 64
extern int	nchsz;
extern struct nchash	*nchash;
extern u_long	nextvnodeid;
extern zone_t	pathname_zone;
#endif

/*
 * Stats on usefulness of namei caches.
 */
struct	nchstats {
	long	ncs_goodhits;		/* hits that we can really use */
	long	ncs_neghits;		/* negative hits that we can use */
	long	ncs_badhits;		/* hits we must drop */
	long	ncs_falsehits;		/* hits with id mismatch */
	long	ncs_miss;		/* misses */
	long	ncs_long;		/* long names that ignore cache */
	long	ncs_pass2;		/* names found with passes == 2 */
	long	ncs_2passes;		/* number of times we attempt it */
	long	ncs_dirscan;		/* # of times we rescan directory */
#ifdef	_KERNEL
	udecl_simple_lock_data(,ncs_lock)
#endif
};

#ifdef	_KERNEL
#define NC_STATS(c)     STATS_ACTION(&nchstats.ncs_lock, (c))
#ifdef	OSF1_ADFS
#define PN_ALLOCATE(buf)                ZALLOC(pathname_zone, (buf), caddr_t)
#define PN_DEALLOCATE(buf)              ZFREE(pathname_zone, (buf))
#endif
#endif

/*
 * Flags to checkdir when proposing to add a name
 * or remove a name from a directory.
 */
#define ADD	0x1
#define DEL	0x2
#endif	/* _SYS_NAMEI_H_ */
