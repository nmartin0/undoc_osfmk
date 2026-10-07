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
 * HISTORY
 * 03-Aug-92  Paul Roy (roy) at Open Software Foundation
 *      Added OSF1_ADFS support:  BLK_RESERVE_SUPPORT added and 
 *	MAXINOBUFSIZE temporarily changed from 1Mb to 56Kb because
 *      of a paging bug.
 *
 * $Log: fsck.h,v $
 * Revision 1.8  1994/11/19  03:18:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/06/29  00:37:38  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.6  1993/10/13  16:28:13  shala
 * Used INT_MAX which is 2G-1 to set the MAXFILESIZE instead of 2G.
 * Fixed bug 6789.
 *
 * Revision 1.5  1993/06/21  21:55:59  wunder
 * Added back in code merged out by r1.4.
 *
 * Revision 1.4  1993/06/04  21:46:05  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.3  1993/05/27  02:50:05  wunder
 * Added id_flags field to carry disk inode preallocated flags.
 *
 * Revision 1.2  1992/10/12  22:08:34  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.11.2.3  1992/03/04  20:04:02  garyf
 * 	remove INOBUFSIZE
 * 	add MAXINOBUFSIZE
 * 	[1992/03/04  20:02:29  garyf]
 *
 * Revision 2.11.2.2  1991/10/09  13:54:50  sue
 * 	Added no_cnt and fatal_cnt to fix bug 3260.
 * 	[91/10/09  13:53:04  sue]
 * 
 * Revision 2.11  91/06/10  16:37:18  devrcs
 * 	make local BUFSIZ definition
 * 	[91/06/03  13:22:07  garyf]
 * 
 * Revision 2.10  91/01/07  17:23:13  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:28  dwm]
 * 
 * Revision 2.9  90/10/07  22:50:46  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	Move global declarations to main.c; leave externs here.
 * 	[90/10/02  09:34:47  gmf]
 * 
 * Revision 2.8  90/08/25  12:36:38  devrcs
 * 	Filesystem clean flag support (override clean
 * 	flag and check anyway).  Eliminate MACH_FASTLINK,
 * 	OSF/1 must always recognize fast symbolic links.
 * 	[90/08/18  03:54:38  nags]
 * 
 * 	move MACH_FASTLINK define from Makefile to fsck.h
 * 	[90/08/15  09:59:05  gmf]
 * 
 * Revision 2.7  90/07/27  11:45:07  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:30:33  pam]
 * 
 * 	Initial security hooks from SecureWare.
 * 	[90/07/12  12:56:42  seiden]
 * 
 * 	     More changes for gcc.
 * 	     [90/07/08  17:40:09  gm]
 * 	[90/07/10  00:00:15  gm]
 * 
 * Revision 2.6  90/07/17  12:38:46  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:40:09  gm]
 * 
 * Revision 2.5  90/06/22  22:12:41  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:05  gmf]
 * 
 * $EndLog$
 */
/* @(#)$RCSfile: fsck.h,v $ $Revision: 1.8 $ (OSF) $Date: 1994/11/19 03:18:10 $ */
/*
 * Copyright (c) 1980, 1986 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted provided
 * that: (1) source distributions retain this entire copyright notice and
 * comment, and (2) distributions including binaries display the following
 * acknowledgement:  ``This product includes software developed by the
 * University of California, Berkeley and its contributors'' in the
 * documentation or other materials provided with the distribution and in
 * all advertising materials mentioning features or use of this software.
 * Neither the name of the University nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	fsck.h	5.17 (Berkeley) 7/27/90
 */
#if SEC_BASE
#include <sys/secdefines.h>
#endif

#include <limits.h>

#define uhowmany(x, y)   (((unsigned long)(x)+(unsigned long)((unsigned long)(y)-1))/(unsigned long)(y))
#define uroundup(x, y)   (((unsigned long)((unsigned long)(x)+(unsigned long)((unsigned long)(y)-1))/(unsigned long)(y))*(unsigned long)(y))

#ifdef	OSF1_ADFS
#define BLK_RESERVE_SUPPORT 	/* reserved blocks are understood */
#endif

#define	MAXDUP		10	/* limit on dup blks (per inode) */
#define	MAXBAD		10	/* limit on bad blks (per inode) */
#define	MAXBUFSPACE	40*1024	/* maximum space to allocate to buffers */

#ifdef	OSF1_ADFS
/* #define MAXINOBUFSIZE   1024*1024  XXX Change back to this value. */
#define MAXINOBUFSIZE   56*1024 /* maximum space to allocate for inodes
				this is intended to limit fsck memory usage
				as fsck is often before paging is enabled */
#else
#define MAXINOBUFSIZE   1024*1024 /* maximum space to allocate for inodes
				this is intended to limit fsck memory usage
				as fsck is often before paging is enabled */
#endif
#define	MAXFILESIZE	INT_MAX

#ifndef FSCK_BUFSIZ
#define FSCK_BUFSIZ 1024
#endif

#define	USTATE	01		/* inode not allocated */
#define	FSTATE	02		/* inode is file */
#define	DSTATE	03		/* inode is directory */
#define	DFOUND	04		/* directory found during descent */
#define	DCLEAR	05		/* directory is to be cleared */
#define	FCLEAR	06		/* file is to be cleared */

/*
 * buffer cache structure.
 */
struct bufarea {
	struct bufarea	*b_next;		/* free list queue */
	struct bufarea	*b_prev;		/* free list queue */
	daddr_t	b_bno;
	int	b_size;
	int	b_errs;
	int	b_flags;
	union {
		char	*b_buf;			/* buffer space */
		daddr_t	*b_indir;		/* indirect block */
		struct	fs *b_fs;		/* super block */
		struct	cg *b_cg;		/* cylinder group */
		struct	dinode *b_dinode;	/* inode block */
	} b_un;
	char	b_dirty;
};

#define	B_INUSE 1

#define	MINBUFS		5	/* minimum number of buffers required */
struct bufarea bufhead;		/* head of list of other blks in filesys */
struct bufarea sblk;		/* file system superblock */
struct bufarea cgblk;		/* cylinder group blocks */
struct bufarea *pdirbp;		/* current directory contents */
struct bufarea *pbp;		/* current inode block */
struct bufarea *getdatablk();

#define	dirty(bp)	(bp)->b_dirty = 1
#define	initbarea(bp) \
	(bp)->b_dirty = 0; \
	(bp)->b_bno = (daddr_t)-1; \
	(bp)->b_flags = 0;

#define	sbdirty()	sblk.b_dirty = 1
#define	cgdirty()	cgblk.b_dirty = 1
#define	sblock		(*sblk.b_un.b_fs)
#define	cgrp		(*cgblk.b_un.b_cg)

enum fixstate {DONTKNOW, NOFIX, FIX, IGNORE};

struct inodesc {
	enum fixstate id_fix;	/* policy on fixing errors */
	int (*id_func)();	/* function to be applied to blocks of inode */
	ino_t id_number;	/* inode number described */
	ino_t id_parent;	/* for DATA nodes, their parent */
	daddr_t id_blkno;	/* current block number being examined */
	int id_numfrags;	/* number of frags contained in block */
	long id_filesize;	/* for DATA nodes, the size of the directory */
	int id_loc;		/* for DATA nodes, current location in dir */
	int id_entryno;		/* for DATA nodes, current entry number */
	struct dirent *id_dirp;	/* for DATA nodes, ptr to current entry */
	char *id_name;		/* for DATA nodes, name to find or enter */
	char id_type;		/* type of descriptor, DATA or ADDR */
#ifdef PFS
	unsigned int id_flags;  /* inode flag indicating block preallocation */
#endif
};
/* file types */
#define	DATA	1
#define	ADDR	2

/*
 * Linked list of duplicate blocks.
 * 
 * The list is composed of two parts. The first part of the
 * list (from duplist through the node pointed to by muldup)
 * contains a single copy of each duplicate block that has been 
 * found. The second part of the list (from muldup to the end)
 * contains duplicate blocks that have been found more than once.
 * To check if a block has been found as a duplicate it is only
 * necessary to search from duplist through muldup. To find the 
 * total number of times that a block has been found as a duplicate
 * the entire list must be searched for occurences of the block
 * in question. The following diagram shows a sample list where
 * w (found twice), x (found once), y (found three times), and z
 * (found once) are duplicate block numbers:
 *
 *    w -> y -> x -> z -> y -> w -> y
 *    ^		     ^
 *    |		     |
 * duplist	  muldup
 */
struct dups {
	struct dups *next;
	daddr_t dup;
};
struct dups *duplist;		/* head of dup list */
struct dups *muldup;		/* end of unique duplicate dup block numbers */

/*
 * Linked list of inodes with zero link counts.
 */
struct zlncnt {
	struct zlncnt *next;
	ino_t zlncnt;
};
struct zlncnt *zlnhead;		/* head of zero link count list */

/*
 * Inode cache data structures.
 */
struct inoinfo {
	struct	inoinfo *i_nexthash;	/* next entry in hash chain */
	ino_t	i_number;		/* inode number of this entry */
	ino_t	i_parent;		/* inode number of parent */
	ino_t	i_dotdot;		/* inode number of `..' */
	size_t	i_isize;		/* size of inode */
	u_int	i_numblks;		/* size of block array in bytes */
	long	i_flags;		/* flags word */
	daddr_t	i_blks[1];		/* actually longer */
} **inphead, **inpsort;

extern long numdirs, listmax, inplast;

extern char	*devname;		/* name of device being checked */
extern long	secsize;		/* actual disk sector size */
extern char	nflag;			/* assume a no response */
extern char	yflag;			/* assume a yes response */
extern int	bflag;			/* location of alternate super block */
extern int	clnoverride;		/* override FS clean flag */
extern int	debug;			/* output debugging info */
extern int	cvtflag;		/* convert to old file system format */
extern int      Cflag;                  /* convert to new fsbtodb value */
extern char	preen;			/* just fix normal inconsistencies */
extern char	hotroot;		/* checking root device */
extern char	havesb;			/* superblock has been read */
extern int	fsmodified;		/* 1 => write done to file system */
extern int	fsreadfd;		/* file descriptor for reading file system */
extern int	fswritefd;		/* file descriptor for writing file system */
extern daddr_t	maxfsblock;		/* number of blocks in the file system */
extern char	*blockmap;		/* ptr to primary blk allocation map */
extern int	old_fsbtodb;		/* value corrected for fsbtodb or minus
					   one if original value was ok */
extern int	new_fsbtodb;		/* new value for fsbtodb or minus one
					   if original value was ok */
extern int	dev_bshift;		/* shift to switch between bytes and
					   conventional blocks */
extern ino_t	maxino;			/* number of inodes in file system */
extern ino_t	lastino;		/* last inode in use */
extern char	*statemap;		/* ptr to inode state table */
extern short	*lncntp;		/* ptr to link count table */
extern ino_t	lfdir;			/* lost & found directory inode number */
extern char	*lfname;		/* lost & found directory name */
extern int	lfmode;			/* lost & found directory creation mode */

extern int	fatal_cnt;		/* count of fatal errors */
extern int	no_cnt;			/* count of no replies */

extern daddr_t	n_blks;			/* number of blocks in use */
extern daddr_t	n_files;		/* number of files in use */

#define	clearinode(dp)	(*(dp) = zino)
#if SEC_FSCHANGE
#define zapino(x)       \
        {                                               \
        long tgen = x->di_gen;                          \
        bzero((char *) x, disk_dinode_size());          \
        x->di_gen = tgen;                               \
        }
#else
#define zapino(x)       (*(x) = zino)
#endif
struct	dinode zino;

#define	setbmap(blkno)	setbit(blockmap, blkno)
#define	testbmap(blkno)	isset(blockmap, blkno)
#define	clrbmap(blkno)	clrbit(blockmap, blkno)

#define btodu(b)  ((b) >> dev_bshift)
#define dutob(d)  ((d) << dev_bshift)

#define	STOP	0x01
#define	SKIP	0x02
#define	KEEPON	0x04
#define	ALTERED	0x08
#define	FOUND	0x10

time_t time();
struct dinode *ginode();
struct inoinfo *getinoinfo();
void getblk();
ino_t allocino();
int findino();
