/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
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
 * Copyright (c) 1991 Center for High Performance Computing of
 *	Worcester Polytechnic Institute
 *	Rights granted to OSF/RI under contract.
 */
/*
 * HISTORY
 * 26-Aug-92  Paul Roy (roy) at Open Software Foundation
 *	Added RESERVE, DADDR, IS_RESERVED macros to support reserved daddrs.
 *
 * $Log: dinode.h,v $
 * Revision 1.2  1994/11/19  03:05:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/12  21:46:26  shala
 * New version to understand maj, min and node numbers.
 *
 * Revision 2.3  92/01/05  19:28:12  roy
 * 	1991/11/12  19:37:27  noemi
 * 	Replaced one of the spare fields with di_node.
 * 
 * Revision 2.2  91/08/31  14:17:47  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/01  16:59:03  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.5  90/10/07  14:57:51  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:51:03  gm]
 * 
 * Revision 1.4  90/06/22  20:54:56  devrcs
 * 	nags merge
 * 	[90/06/12  21:40:19  gmf]
 * 
 * 	     Changed uid and gid fields from uid_t and gid_t to u_short, since
 * 	     uid_t and gid_t are now longs, and we don't want to break compatibility
 * 	     for existing filesystems.
 * 	     [90/06/06  13:05:58  ers]
 * 	[90/06/10  17:25:34  ers]
 * 
 * 	     Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	     [90/06/09  18:46:52  seiden]
 * 
 * 	Condensed history (reverse chronology):
 * 	Updated for OSF/1 parallelization.		nags@encore.com
 * 	Defined new file type: IFIFO.			ers@osf.org
 * 	New 4.4BSD file system file [1/5/90].		noemi@osf.org
 * 	Integrated with 4.4BSD sources [11/30/89].	noemi@osf.org
 * 	[90/06/10  02:08:31  seiden]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1989 The Regents of the University of California.
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
 *	@(#)dinode.h	7.6 (Berkeley) 10/24/89
 */

#ifndef	_UFS_DINODE_H_
#define	_UFS_DINODE_H_

#include <sys/secdefines.h>
#if	SEC_FSCHANGE
#include <sys/security.h>
#endif

#include <sys/types.h>
#include <machine/endian.h>

/*
 * This structure defines the on-disk format of an inode.
 *
 * Refer to ../sys/inode.h for an explanation of locking constraints
 * when the inode is in-core.
 */

#define	NDADDR	12		/* direct addresses in inode */
#define	NIADDR	3		/* indirect addresses in inode */

#define MAX_FASTLINK_SIZE	((NDADDR + NIADDR) * sizeof (daddr_t))

struct dinode {
	u_short	di_mode;	/*  0: mode and type of file */
	short	di_nlink;	/*  2: number of links to file */
	u_short	di_uid;		/*  4: owner's user id */
	u_short	di_gid;		/*  6: owner's group id */
	quad	di_qsize;	/*  8: number of bytes in file */
	time_t	di_atime;	/* 16: time last accessed */
	long	di_atspare;
	time_t	di_mtime;	/* 24: time last modified */
	long	di_mtspare;
	time_t	di_ctime;	/* 32: last time inode changed */
	long	di_ctspare;
	union {
	    struct {
		daddr_t	Mb_db[NDADDR]; /* 40: disk block addresses*/
		daddr_t	Mb_ib[NIADDR]; /* 88: indirect blocks */
	    } di_Mb;
#define di_db	di_Mun.di_Mb.Mb_db
#define di_ib	di_Mun.di_Mb.Mb_ib
	    char	di_Msymlink[MAX_FASTLINK_SIZE];
						/* 40: symbolic link name */
	} di_Mun;
#define di_symlink	di_Mun.di_Msymlink
	long	di_flags;	/* 100: status, currently unused */
#define IC_FASTLINK	0x0001		/* Symbolic link in inode */
	long	di_blocks;	/* 104: blocks actually held */
	long	di_gen;		/* 108: generation number */
#ifdef	OSF1_ADFS
	node_t	di_node;	/* 112: node # */
	long	di_spare[3];	/* 116: reserved, currently unused */
#else
	long	di_spare[4];	/* 112: reserved, currently unused */
#endif
};

#if	SEC_FSCHANGE

/*
 * Security extensions to the on-disk inode format:
 */
struct dinode_sec {
	priv_t  di_gpriv[2];    /* granted privilege vector */
	priv_t  di_ppriv[2];    /* potential privilege vector */
	tag_t   di_tag[SEC_TAG_COUNT];  /* security policy tags */
	ino_t   di_parent;      /* inode number of parent of MLD child */
	u_short di_type_flags;  /* type flags (MLD, 2 person rule, etc.) */
};

/*
 * On-disk inode format for a secure filesystem:
 */
struct sec_dinode {
	struct dinode           di_node;
	struct dinode_sec       di_sec;
};
#endif	/* SEC_FSCHANGE */

#if	BYTE_ORDER == LITTLE_ENDIAN
#define	di_size		di_qsize.val[0]
#else
#define	di_size		di_qsize.val[1]
#endif
#define	di_rdev		di_db[0]

/* file modes */
#define	IFMT		0170000		/* type of file */
#define	IFIFO		0010000		/* fifo (named pipe) */
#define	IFCHR		0020000		/* character special */
#define	IFDIR		0040000		/* directory */
#define	IFBLK		0060000		/* block special */
#define	IFREG		0100000		/* regular */
#define	IFLNK		0120000		/* symbolic link */
#define	IFSOCK		0140000		/* socket */

#define	ISUID		04000		/* set user id on execution */
#define	ISGID		02000		/* set group id on execution */
#define	ISVTX		01000		/* save swapped text even after use */
#define	IREAD		0400		/* read, write, execute permissions */
#define	IWRITE		0200
#define	IEXEC		0100

#ifdef	OSF1_ADFS
/*
 * Define macros used for reserved daddrs (i.e., those with the high-bit set).
 */
#define	RESERVE(daddr)		((daddr) | 0x80000000)
#define DADDR(daddr)		((daddr) & 0x7fffffff)
#define IS_RESERVED(daddr)	((daddr) & 0x80000000 ? TRUE : FALSE)
#endif	/* OSF1_ADFS */

#endif	/* _UFS_DINODE_H_ */
