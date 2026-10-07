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
 * $Log: nfsv2.h,v $
 * Revision 1.5  1994/11/18  20:37:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:17:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:39:26  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  20:29:56  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:36:09  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:33:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:31:00  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:25:20  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:54:38  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/07/31  15:41:16  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:40:15  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:23:09  gm]
 * 
 * Revision 1.4  90/06/22  20:40:28  devrcs
 * 	Added NFS_FABLKSIZE for # of blocks calculations.
 * 	Added NFFIFO file type, for compatibility with HP's NFS.
 * 	[90/06/18  00:17:52  ers]
 * 
 * Revision 1.3  90/01/02  20:22:41  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:22:45  gm
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
 *	@(#)nfsv2.h	7.2 (Berkeley) 8/30/89
 */

/*
 * nfs definitions as per the version 2 specs
 */

/*
 * Constants as defined in the Sun NFS Version 2 spec.
 * "NFS: Network File System Protocol Specification" RFC1094
 */

#define NFS_PORT	2049
#define	NFS_PROG	100003
#define NFS_VER2	2
#define	NFS_MAXDATA	8192
#define	NFS_MAXPATHLEN	1024
#define	NFS_MAXNAMLEN	255
#define	NFS_FHSIZE	32
#define	NFS_NPROCS	18
#define NFS_FABLKSIZE	512

/* Stat numbers for rpc returns */
#define	NFS_OK		0
#define	NFSERR_PERM	1
#define	NFSERR_NOENT	2
#define	NFSERR_IO	5
#define	NFSERR_NXIO	6
#define	NFSERR_ACCES	13
#define	NFSERR_EXIST	17
#define	NFSERR_NODEV	19
#define	NFSERR_NOTDIR	20
#define	NFSERR_ISDIR	21
#define	NFSERR_FBIG	27
#define	NFSERR_NOSPC	28
#define	NFSERR_ROFS	30
#define	NFSERR_NAMETOOLONG	63
#define	NFSERR_NOTEMPTY	66
#define	NFSERR_DQUOT	69
#define	NFSERR_STALE	70
#define	NFSERR_WFLUSH	99

/* Sizes in bytes of various nfs rpc components */
#define	NFSX_FH		32
#define	NFSX_UNSIGNED	4
#define	NFSX_FATTR	68
#define	NFSX_SATTR	32
#define	NFSX_COOKIE	4
#define NFSX_STATFS	20

/* nfs rpc procedure numbers */
#define	NFSPROC_NULL		0
#define	NFSPROC_GETATTR		1
#define	NFSPROC_SETATTR		2
#define	NFSPROC_ROOT		3		/* Obsolete */
#define	NFSPROC_LOOKUP		4
#define	NFSPROC_READLINK	5
#define	NFSPROC_READ		6
#define	NFSPROC_WRITECACHE	7		/* Obsolete */
#define	NFSPROC_WRITE		8
#define	NFSPROC_CREATE		9
#define	NFSPROC_REMOVE		10
#define	NFSPROC_RENAME		11
#define	NFSPROC_LINK		12
#define	NFSPROC_SYMLINK		13
#define	NFSPROC_MKDIR		14
#define	NFSPROC_RMDIR		15
#define	NFSPROC_READDIR		16
#define	NFSPROC_STATFS		17

/* Conversion macros */
#define	vtonfs_mode(t,m) txdr_unsigned(MAKEIMODE(t,m))
#define	nfstov_mode(a)	(fxdr_unsigned(u_short, (a))&07777)
#define	vtonfs_type(a)	txdr_unsigned(nfs_type[((long)(a))])
#define	nfstov_type(a)	v_type[fxdr_unsigned(u_long, (a))]

/* File types */
typedef enum { NFNON=0, NFREG=1, NFDIR=2, NFBLK=3, NFCHR=4, NFLNK=5,
/* Note: the following are not part of the protocol spec, but NFFIFO
   is used by at least one vendor (HP) */
		NFSOCK=6, NFBAD=7, NFFIFO=8
} nfstype;

/* Structs for common parts of the rpc's */
struct nfsv2_time {
	u_long	tv_sec;
	u_long	tv_usec;
};

struct nfsv2_fattr {
	u_long	fa_type;
	u_long	fa_mode;
	u_long	fa_nlink;
	u_long	fa_uid;
	u_long	fa_gid;
	u_long	fa_size;
	u_long	fa_blocksize;
	u_long	fa_rdev;
	u_long	fa_blocks;
	u_long	fa_fsid;
	u_long	fa_fileid;
	struct nfsv2_time fa_atime;
	struct nfsv2_time fa_mtime;
	struct nfsv2_time fa_ctime;
};

struct nfsv2_sattr {
	u_long	sa_mode;
	u_long	sa_uid;
	u_long	sa_gid;
	u_long	sa_size;
	struct nfsv2_time sa_atime;
	struct nfsv2_time sa_mtime;
};

struct nfsv2_statfs {
	u_long	sf_tsize;
	u_long	sf_bsize;
	u_long	sf_blocks;
	u_long	sf_bfree;
	u_long	sf_bavail;
};
