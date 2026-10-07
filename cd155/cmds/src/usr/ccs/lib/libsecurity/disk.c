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
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Copyright (c) 1988-90 SecureWare, Inc.  All rights reserved.
 */

#ident "@(#)disk.c	2.1 16:17:31 4/20/90 SecureWare"
/*
 * Based on:
 *   "@(#)disk.c	2.5 09:46:13 5/24/89 SecureWare, Inc."
 */

/*LINTLIBRARY*/

/*
 * This file contains a set of routines used to make programs
 * more secure.  Specifically, routines that manipulate the raw
 * disk are prepared to handle both secure and insecure inode formats.
 */

#include <sys/secdefines.h>

#if SEC_ARCH /*{*/

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <sys/param.h>

#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>

#ifdef AUX
#include <sys/time.h>
#include <sys/vnode.h>
#include <svfs/inode.h>
#include <svfs/filsys.h>
#endif

#ifdef SYSV
#include <sys/fs/s5param.h>
#include <sys/fs/s5inode.h>
#include <sys/inode.h>
#include <sys/ino.h>
#ifdef SYSV_3
#include <sys/fs/s5filsys.h>
#else
#include <sys/filsys.h>
#endif
#endif

#ifdef _OSF_SOURCE
#include <ufs/inode.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#endif

static int secure_file_system = 0;
static int dinode_size = 0;

#if defined(SYSV) || defined(AUX) /*{*/
static int inodes_per_block = 0;
static int inode_shift = 0;

static int shift();


/*
 * Based on the fs argument, determine the type of file system, whether
 * secure or insecure.  Compute some constants that are to be used by
 * other routines.
 *
 * This routine MUST be called before any of the other ones in this file
 * to initialize global variables.  The routine should be called once
 * prior to activity on each file system in turn.  One cannot "mix and
 * match" file system handling, but must be done in turn, with this routine
 * getting called prior to each new file system.
 */
void
disk_set_file_system(fs, block_size)
	struct filsys *fs;
	int block_size;
{
	secure_file_system = ((fs->s_type & FsSW) != 0);

	dinode_size = secure_file_system
#if defined(AUX)
			? sizeof(struct sec_dinode)
			: sizeof(struct dinode);
#else
			? sizeof(struct dinode)
			: sizeof(struct insec_dinode);
#endif

	inodes_per_block = block_size / dinode_size;

	inode_shift = shift();
}
#else
/*
 * All BSD constants are built into the macros.
 */
void
disk_set_file_system(fs, block_size)
	struct fs *fs;
	int block_size;
{
	if (secure_file_system = FsSEC(fs))
		dinode_size = sizeof(struct sec_dinode);
	else
		dinode_size = sizeof(struct dinode);
}
#endif /*}*/


/*
 * Return 1 if the file system is secure and 0 if not.
 */
int
disk_secure_file_system()
{
	return secure_file_system;
}


/*
 * Increment the inode pointer by incr number of entries.  The hard
 * value of the result depends on the type of file system.
 */
void
disk_inode_incr(dp, incr)
	struct dinode **dp;
	int incr;
{
#if defined(AUX) || defined(_OSF_SOURCE)
	if (secure_file_system)
		*(struct sec_dinode **)dp += incr;
	else
		*dp += incr;
#else
	if (secure_file_system)
		*dp = *dp + incr;
	else
		*dp = (struct dinode *) ((struct insec_dinode *) *dp + incr);
#endif
}


/*
 * Return the size of an inode on the current file system being used.
 */
int
disk_dinode_size()
{
	return dinode_size;
}


#if !defined(_OSF_SOURCE) /*{*/
/*
 * Return 1 if the file system appears valid, and 0 if not.
 * A file system, to be valid, is necessary to satisfy the following
 * conditions:
 *
 *	1. The total blocks exceeds smaller totals, like free blocks
 *	   and the last inode block.
 *	2. The total number of inode blocks must be more than the
 *	   block count of free inodes.
 *	3. The number of inodes must fit into a short, or some inodes
 * 	   cannot be references.
 */
int
disk_valid(fs)
	register struct filsys *fs;
{
	register int total_inodes;

	total_inodes = (fs->s_isize - (SUPERB+1)) * inodes_per_block;

	return (int) ((fs->s_fsize > fs->s_isize) &&
		      (fs->s_fsize > fs->s_tfree) &&
		      (total_inodes >= fs->s_tinode) &&
		      ((ulong) total_inodes < ((ulong) ((ushort) ~0)) + 1));
}


/*
 * Given an inode number, return the file system disk block containing
 * that inode.
 */
daddr_t
disk_itod(inode_number)
	int inode_number;
{
	return (daddr_t) (((unsigned)inode_number + (2*inodes_per_block-1)) >>
		inode_shift);
}


/*
 * Given an inode number, return the offset within the file system disk
 * block containing that inode.
 */
daddr_t
disk_itoo(inode_number)
	int inode_number;
{
	return (daddr_t) (((unsigned)inode_number + (2*inodes_per_block-1)) &
		(inodes_per_block-1));
}


/*
 * Return the number of inodes per block on the file system.
 */
int
disk_inodes_per_block()
{
	return inodes_per_block;
}

/*
 * Compute the amount of shifting needed for the given inode size.  The
 * results are used in calculations of inode disk block locations.
 */
static int
shift()
{
	register int shifts = 0;
	register int value;

	value = inodes_per_block >> 1;
	while (value > 0)  {
		shifts++;
		value >>= 1;
	}

	return shifts;
}
#endif /*}*/

#ifdef _OSF_SOURCE /*{*/
/*
 * increment a block pointer to point to the dinode entry corresponding
 * to the inode number passed in.  Acts differently for secure and non-secure
 * file systems.
 */
void
disk_inode_in_block(fs, bp, dipp, inumber)
struct fs *fs;
char *bp;
struct dinode **dipp;
ino_t inumber;
{
	int i = itoo(fs, inumber);

	if (FsSEC(fs))
		*dipp = (struct dinode *) (((struct sec_dinode *) bp) + i);
	else
		*dipp = ((struct dinode *) bp) + i;
}
#endif /*}*/

#endif /*} SEC_ARCH */
