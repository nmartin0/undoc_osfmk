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
 * Copyright (c) 1991 Center for High Performance Computing of
 *	Worcester Polytechnic Institute
 *	Rights granted to OSF/RI under contract.
 */
/*
 * HISTORY
 * $Log: dinode.h,v $
 * Revision 1.7  1994/11/18  20:45:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.6  1994/06/28  22:55:08  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.5  1993/07/14  18:37:09  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:52:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/27  03:06:47  wunder
 * Added define for IC_PREALLOCATED, to indicate when a file has preallocated
 * disk blocks.
 *
 * Revision 1.3  1993/05/06  20:30:19  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:48:53  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:50:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:38:38  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:47:08  cfj
 * Bump major revision number.
 *
 * Revision 2.5  93/10/20  15:27:03  dnoveck
 *      DEV_BSIZE elimination: Added warning about duplication between
 *      this file and kernel header boot_ufs/disk_inode.h.
 *
 * Revision 2.4  1992/09/11  09:28:02  rabii
 * 	Added RESERVE, DADDR, IS_RESERVED macros to support reserved daddrs.
 * 	[92/08/26            roy]
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

/****************************************************************************
*                                                                           *
*   *********************************************************************   *
*   *                                                                   *   *
*   *   WARNING: This file duplicates most of the definitions in        *   *
*   *            the kernel header file boot_ufs/disk_inode.h  Until    *   *
*   *            someone fixes this, please keep them in sync.          *   *
*   *                                                                   *   *
*   *********************************************************************   *
*                                                                           *
****************************************************************************/

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
#ifdef PFS
#define IC_PREALLOCATED	0x0002		/* Preallocated disk blocks */
#endif
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
