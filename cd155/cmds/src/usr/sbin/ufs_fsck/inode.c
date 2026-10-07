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
 * $Log: inode.c,v $
 * Revision 1.9  1994/11/19  03:18:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/29  00:31:31  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 *
 * Revision 2.6  94/02/03  12:15:21  dnoveck
 *    Support indirect blocks smaller than logical block size.
 * 
 * Revision 2.5  93/12/22  12:23:07  roy
 *    Don't call iblock() recursively if the isize arg will be < 0.
 *    [93/07/01            roy]
 * 
 * Revision 2.4  93/10/21  11:31:56  dnoveck
 *    For DEV_BSIZE elimination: Fix i_blocks computation to be 
 *    insesitive to DEV_BSIZE.
 * 
 * Revision 1.7  1994/03/11  22:02:25  brad
 * Merged revision 1.5.4.3 from the R1.2 branch.
 *
 * Revision 1.5.4.3  1994/03/11  21:52:48  brad
 * Fixed printing of warning messages for reserved disk blocks that
 * need to be cleared: 1) now only one message displayed per indirect
 * block rather than one message per data block (which can result in
 * many many messages), and 2) messages displayed under BLK_RESERVE_SUPPORT
 * ifdef's now properly ask the user for guidance if -y not specified.
 *
 *  Reviewer: Dave Minturn
 *  Risk: Low
 *  Benefit or PTS #: 8445
 *  Testing: Developer testing, crashed system and ran fsck, etc.
 *  Module(s): inode.c pass1.c ufs_fsck.msg
 *
 * Revision 1.6  1994/02/16  03:53:51  brad
 * Merged revision 1.5.4.2 from the R1.2 branch.
 *
 * Revision 1.5.4.2  1994/02/16  03:41:15  brad
 * Fixed history logs.
 *
 * Revision 1.5.4.1  1994/02/16  03:32:05  brad
 * Code cleanup in relation to bug fix for PTS #6318.  No change
 * in implementation.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 6318
 *  Testing: Ran fsck many times on different file systems after
 *     performing extensive lsize testing and running EATs.
 *  Module(s): pass1.c, inode.c
 *
 * Revision 1.5  1993/07/08  00:37:01  brad
 * Fixed a bug in block reservation code ... iblock() shouldn't be called with
 * a size < 0.  This fix is from Paul Roy at OSF, and is going into OSF's
 * AD1.0.5 code base.
 *
 * Revision 1.4  1993/06/04  21:46:11  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.3  1993/05/27  02:51:38  wunder
 * Added checking for disk block preallocation, see ifdef PFS's.
 *
 * Revision 1.2  1992/10/12  22:07:56  shala
 * New version to support maj, min and node numbers.
 *
 * 03-Aug-92  Paul Roy (roy) at Open Software Foundation
 *	Added BLK_RESERVE_SUPPORT to support filesystems that reserve
 *	blocks by or'ing the high bit of the daddr to 1.  These daddrs
 *	need to be cleared by fsck.  
 *      Don't use message catalog for OSF1_ADFS.
 *
 * Revision 2.11.2.3  1992/04/08  17:20:10  hosking
 * 	fix security build errors
 * 	[1992/04/08  17:19:14  hosking]
 *
 * Revision 2.11.2.2  1992/03/04  20:03:58  garyf
 * 	change size used for inode reading
 * 	[1992/03/04  20:02:39  garyf]
 * 
 * Revision 2.11  1991/06/10  16:37:20  devrcs
 * 	use local BUFSIZ definition
 * 	[91/06/03  13:20:15  garyf]
 * 
 * 	add messaging
 * 	[91/05/13  11:34:03  garyf]
 * 
 * Revision 2.10  91/01/07  17:23:17  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:35  dwm]
 * 
 * Revision 2.9  90/10/07  22:50:51  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	Added di_flags to inode cache.
 * 	[90/10/02  09:36:47  gmf]
 * 
 * Revision 2.8  90/08/25  12:36:42  devrcs
 * 	OSF/1 must always recognize fast symbolic links.
 * 	[90/08/19  01:34:22  nags]
 * 
 * Revision 2.7  90/07/27  11:45:11  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:30:41  pam]
 * 
 * 	Initial security hooks from SecureWare.
 * 	[90/07/12  12:56:58  seiden]
 * 
 * 	     More changes for gcc.
 * 	     [90/07/08  17:40:15  gm]
 * 	[90/07/10  00:00:21  gm]
 * 
 * Revision 2.6  90/07/17  12:38:50  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:40:15  gm]
 * 
 * Revision 2.5  90/06/22  22:12:44  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:12  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: inode.c,v $ $Revision: 1.9 $ (OSF) $Date: 1994/11/19 03:18:11 $";
#endif
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
 */

/*** "inode.c	5.17 (Berkeley) 7/27/90"; ***/

#include <sys/secdefines.h>
#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <ufs/dir.h>
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include "fsck.h"
#if SEC_BASE
#include <sys/security.h>
#if SEC_ARCH
#include <sys/secpolicy.h>
#endif
#endif

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

static ino_t startinum;

ckinode(dp, idesc)
	struct dinode *dp;
	register struct inodesc *idesc;
{
	register daddr_t *ap;
	long ret, n, ndb, offset;
	struct dinode dino;

	if (idesc->id_fix != IGNORE)
		idesc->id_fix = DONTKNOW;
	idesc->id_entryno = 0;
	idesc->id_filesize = dp->di_size;
	if ((dp->di_mode & IFMT) == IFBLK || (dp->di_mode & IFMT) == IFCHR ||
		(dp->di_flags & IC_FASTLINK) != 0)
		return (KEEPON);
	dino = *dp;
	ndb = uhowmany(dino.di_size, sblock.fs_bsize);
	for (ap = &dino.di_db[0]; ap < &dino.di_db[NDADDR]; ap++) {
		if (--ndb == 0 && (offset = blkoff(&sblock, dino.di_size)) != 0)
			idesc->id_numfrags =
				numfrags(&sblock, fragroundup(&sblock, offset));
		else
			idesc->id_numfrags = sblock.fs_frag;
		if (*ap == 0)
			continue;
#ifdef	BLK_RESERVE_SUPPORT
		/*
		 * Reserved blocks should already have been handled in pass1.
		 */
		if (IS_RESERVED(*ap))
#ifdef PFS
			if (dino.di_flags & IC_PREALLOCATED) {
#ifdef DEBUG_PFS
				printf("ckinode: TEMPORARILY CLEAR RESERVE BIT IN DADDR %ld\n",
				       DADDR(*ap));
#endif
				idesc->id_blkno = DADDR(*ap);
			} else
#endif
				panic("chkinode: found reserved direct block");
		else
#endif
		idesc->id_blkno = *ap;
		if (idesc->id_type == ADDR)
			ret = (*idesc->id_func)(idesc);
		else
			ret = dirscan(idesc);
		if (ret & STOP)
			return (ret);
	}
	idesc->id_numfrags = sblock.fs_frag;
	for (ap = &dino.di_ib[0], n = 1; n <= NIADDR; ap++, n++) {
		if (*ap) 
#ifdef BLK_RESERVE_SUPPORT
		if (sblock.fs_bsize*NDADDR >= dino.di_size) {
		        /*
			 * Zero this indirect block pointer because it's
			 * beyond the eof.
			 */
			pwarn(MSGSTR(INRESBLOCK,
				     "INDIRECT BLOCK (%ld) RESERVED BEYOND EOF I=%lu"),
			      DADDR(*ap), idesc->id_number);
			if (preen)
				printf(MSGSTR(FREED, " (FREED)\n"));
			else if (reply(MSGSTR(FREE, "FREE")) == 0)
				continue;
		        *ap = 0;
			dp = ginode(idesc->id_number);
			dp->di_ib[n-1] = 0;
			inodirty();
			continue;
		} else 
#endif
		{
			idesc->id_blkno = *ap;
			idesc->id_numfrags =
			  numfrags(&sblock, NINDIR(&sblock)*sizeof(daddr_t));
#ifdef PFS
			idesc->id_flags = dino.di_flags;
#endif
			ret = iblock(idesc, n,
				     dino.di_size - sblock.fs_bsize * NDADDR);
			if (ret & STOP)
				return (ret);
		}
	}
	return (KEEPON);
}

iblock(idesc, ilevel, isize)
	struct inodesc *idesc;
	register long ilevel;
	u_long isize;
{
	register daddr_t *ap;
	register daddr_t *aplim;
	int i, n, (*func)(), nif, sizepb;
	register struct bufarea *bp;
	char buf[FSCK_BUFSIZ];
	extern int dirscan(), pass1check();
#ifdef	BLK_RESERVE_SUPPORT	
	int first_daddr;
	int doit;
#endif
#ifdef PFS
	extern int preallocated_blocks;
#endif

#ifdef	BLK_RESERVE_SUPPORT
	if ((long)isize < 0)
	        panic("iblock: isize < 0. ino=%lu ilevel=%d isize=0x%x\n",
		      idesc->id_number, ilevel, isize);
#endif
	if (idesc->id_type == ADDR) {
		func = idesc->id_func;
		if (((n = (*func)(idesc)) & KEEPON) == 0)
			return (n);
	} else
		func = dirscan;
	if (chkrange(idesc->id_blkno, idesc->id_numfrags))
		return (SKIP);
	bp = getdatablk(idesc->id_blkno, NINDIR(&sblock)*sizeof(daddr_t));
	ilevel--;
	for (sizepb = sblock.fs_bsize, i = 0; i < ilevel; i++)
		sizepb *= NINDIR(&sblock);
	nif = isize / sizepb + 1;
	if (nif > NINDIR(&sblock))
		nif = NINDIR(&sblock);

	if (idesc->id_func == pass1check && nif < NINDIR(&sblock)) {
		/*
		 * Free any blocks (possibly indirect, depending on ilevel)
		 * referenced by this indirect block that have been allocated
		 * past the end of file.
		 */
		aplim = &bp->b_un.b_indir[NINDIR(&sblock)];
#ifdef	BLK_RESERVE_SUPPORT
		first_daddr = 1;
#endif
		for (ap = &bp->b_un.b_indir[nif]; ap < aplim; ap++) {
			if (*ap == 0)
				continue;
#ifdef	BLK_RESERVE_SUPPORT
			/*
			 * Zero out data block pointers that are marked 
			 * reserved.      
			 */
			if (IS_RESERVED(*ap)) {
				if (first_daddr) {
					first_daddr = 0;
					pwarn(MSGSTR(RESBLOCKS,
						     "BLOCKS (starting at %ld) RESERVED BEYOND EOF I=%lu"),
					      DADDR(*ap), idesc->id_number);
					if (preen) {
						doit = 1;
						printf(MSGSTR(FREEING, " (FREEING)\n"));
					} else {
						doit = reply(MSGSTR(FREE,
								    "FREE"));
					}
					if (ilevel > 0)
						pwarn(MSGSTR(INRESBLOCKS,
							     "WARNING: RESERVED INDIRECT BLOCKS I=%lu (level=%d)\n"),
						      idesc->id_number,
						      ilevel);
				}
				if (doit) {
					*ap = 0;
					dirty(bp);
				}
				continue;
			}
#endif
			(void)sprintf(buf, MSGSTR(PARTI, "PARTIALLY TRUNCATED INODE I=%lu"),
				idesc->id_number);
			if (dofix(idesc, buf)) {
				*ap = 0;
				dirty(bp);
			}
		}
		flush(fswritefd, bp);
	}

	/*
	 * Now loop through blocks (possibly indirect, depending on ilevel)
	 * referenced by this indirect block that have been allocated inside
	 * the valid range of file data.
	 */
	aplim = &bp->b_un.b_indir[nif];
#ifdef  BLK_RESERVE_SUPPORT
	first_daddr = 1;
#endif
	for (ap = bp->b_un.b_indir, i = 1; ap < aplim; ap++, i++) {
		if (*ap) {
			idesc->id_blkno = *ap;
			if (ilevel > 0) {
				idesc->id_numfrags =
				  numfrags(&sblock,
				  	   NINDIR(&sblock)*sizeof(daddr_t));

				if (i * sizepb >= isize) {
					*ap = 0;
					dirty(bp);
					continue;
				} else {
					n = iblock(idesc, ilevel, 
						   isize - i * sizepb);
				}

			} else if (IS_RESERVED(*ap)) {
				idesc->id_numfrags = sblock.fs_frag;
#ifdef BLK_RESERVE_SUPPORT
#ifdef PFS
				if (idesc->id_flags & IC_PREALLOCATED) {
#ifdef DEBUG_PFS
					printf("iblock: ALLOWING PREALLOCATED DADDR=%ld IN I=%lu\n", 
				       		DADDR(*ap), idesc->id_number);
#endif
					idesc->id_blkno = DADDR(*ap);
					preallocated_blocks = TRUE;
					n = (*func)(idesc);

				} else {
#endif	PFS
					if (first_daddr) {
						first_daddr = 0;
						pwarn(MSGSTR(RESBLOCKS2,
							     "BLOCKS (starting at %ld) IN I=%lu ARE RESERVED"),
						      DADDR(*ap),
						      idesc->id_number);
						if (preen) {
							doit = 1;
							printf(MSGSTR(FREEING,
							      " (FREEING)\n"));
						} else {
							doit = reply(
								MSGSTR(FREE,
								       "FREE"));
						}
					}

					if (doit) {
						*ap = 0;
						dirty(bp);
					}
					continue;
#ifdef	PFS
				}
#endif PFS
			} else {
#endif BLK_RESERVE_SUPPORT
				idesc->id_numfrags = sblock.fs_frag;
				n = (*func)(idesc);
			}

			if (n & STOP) {
			        flush(fswritefd, bp);
				bp->b_flags &= ~B_INUSE;
				return (n);
			}
		}
	}
	flush(fswritefd, bp);
	bp->b_flags &= ~B_INUSE;
	return (KEEPON);
}

/*
 * Check that a block in a legal block number.
 * Return 0 if in range, 1 if out of range.
 */
chkrange(blk, cnt)
	daddr_t blk;
	int cnt;
{
	register int c;

	/*
	 * Extra paranoia check for very large or negative disk addresses
	 */
	if (blk < 0 || cnt < 0)
		return(1);
	if ((unsigned)(blk + cnt) > maxfsblock)
		return (1);
	c = dtog(&sblock, blk);
	if (blk < cgdmin(&sblock, c)) {
		if ((blk + cnt) > cgsblock(&sblock, c)) {
			if (debug) {
				printf(MSGSTR(BLKCG, "blk %ld < cgdmin %ld;"),
				    blk, cgdmin(&sblock, c));
				printf(MSGSTR(BLKCG1, " blk + cnt %ld > cgsbase %ld\n"),
				    blk + cnt, cgsblock(&sblock, c));
			}
			return (1);
		}
	} else {
		if ((blk + cnt) > cgbase(&sblock, c+1)) {
			if (debug)  {
				printf(MSGSTR(BLKCG2, "blk %ld >= cgdmin %ld;"),
				    blk, cgdmin(&sblock, c));
				printf(MSGSTR(BLKCG3, " blk + cnt %ld > sblock.fs_fpg %ld\n"),
				    blk+cnt, sblock.fs_fpg);
			}
			return (1);
		}
	}
	return (0);
}

/*
 * General purpose interface for reading inodes.
 */
struct dinode *
ginode(inumber)
	ino_t inumber;
{
	daddr_t iblk;
#if SEC_FSCHANGE
        struct dinode *dp;
#endif

	if (inumber < ROOTINO || inumber > maxino)
		errexit(MSGSTR(BADI, "bad inode number %d to ginode\n"), inumber);
	if (startinum == 0 ||
	    inumber < startinum || inumber >= startinum + INOPB(&sblock)) {
		iblk = itod(&sblock, inumber);
		if (pbp != 0)
			pbp->b_flags &= ~B_INUSE;
		pbp = getdatablk(iblk, sblock.fs_bsize);
		startinum = (inumber / INOPB(&sblock)) * INOPB(&sblock);
	}
#if SEC_FSCHANGE
        disk_inode_in_block(&sblock, pbp->b_un.b_buf, &dp, inumber);
        return(dp);
#else
        return (&pbp->b_un.b_dinode[inumber % INOPB(&sblock)]);
#endif
}

/*
 * Special purpose version of ginode used to optimize first pass
 * over all the inodes in numerical order.
 */
ino_t nextino, lastinum;
long readcnt, readpercg, fullcnt, inobufsize, partialcnt, partialsize;
#if SEC_FSCHANGE
char *inodebuf;
#else
struct dinode *inodebuf;
#endif

struct dinode *
getnextinode(inumber)
	ino_t inumber;
{
	long size;
	daddr_t dblk;
	static struct dinode *dp;

	if (inumber != nextino++ || inumber > maxino)
		errexit(MSGSTR(BADI1, "bad inode number %d to nextinode\n"), inumber);
	if (inumber >= lastinum) {
		readcnt++;
		dblk = fsbtodb(&sblock, itod(&sblock, lastinum));
		if (readcnt % readpercg == 0) {
			size = partialsize;
			lastinum += partialcnt;
		} else {
			size = inobufsize;
			lastinum += fullcnt;
		}
		(void)bread(fsreadfd, (char *)inodebuf, dblk, size); /* ??? */
#if SEC_FSCHANGE
		dp = (struct dinode *) inodebuf;
		return dp;
#else
		dp = inodebuf;
#endif
	}
#if SEC_FSCHANGE
	else {
		/*
		 * if last inode in block, increment to next block
		 */

		if ((inumber % INOPB(&sblock)) == 0) {
			dp = (struct dinode *) (((int) dp) -
				((INOPB(&sblock) - 1) * disk_dinode_size()) +
				sblock.fs_bsize);
		}
		else
			disk_inode_incr(&dp, 1);
		return dp;
	}
#else
	return (dp++);
#endif
}

resetinodebuf()
{

	startinum = 0;
	nextino = 0;
	lastinum = 0;
	readcnt = 0;
	inobufsize = sblock.fs_bsize * (sblock.fs_ipg / INOPB(&sblock));
	if (inobufsize > MAXINOBUFSIZE)
		inobufsize = MAXINOBUFSIZE;
	inobufsize = blkroundup(&sblock, inobufsize);
#if SEC_FSCHANGE
	fullcnt = uhowmany(inobufsize, sblock.fs_bsize) * INOPB(&sblock);
#else
	fullcnt = inobufsize / sizeof(struct dinode);
#endif
	readpercg = sblock.fs_ipg / fullcnt;
	partialcnt = sblock.fs_ipg % fullcnt;
#if SEC_FSCHANGE
	partialsize = uhowmany(partialcnt, INOPB(&sblock)) * sblock.fs_bsize;
#else
	partialsize = partialcnt * sizeof(struct dinode);
#endif
	if (partialcnt != 0) {
		readpercg++;
	} else {
		partialcnt = fullcnt;
		partialsize = inobufsize;
	}
#if SEC_FSCHANGE
	if (inodebuf == NULL &&
	    (inodebuf = (char *) malloc((unsigned)inobufsize)) == NULL)
		errexit(MSGSTR(ALLOC, "Cannot allocate space for inode buffer\n"));
#else
	if (inodebuf == NULL &&
	    (inodebuf = (struct dinode *)malloc((unsigned)inobufsize)) == NULL)
		errexit(MSGSTR(ALLOC, "Cannot allocate space for inode buffer\n"));
#endif
	while (nextino < ROOTINO)
		(void)getnextinode(nextino);
}

freeinodebuf()
{

	if (inodebuf != NULL)
		free((char *)inodebuf);
	inodebuf = NULL;
}

/*
 * Routines to maintain information about directory inodes.
 * This is built during the first pass and used during the
 * second and third passes.
 *
 * Enter inodes into the cache.
 */
cacheino(dp, inumber)
	register struct dinode *dp;
	ino_t inumber;
{
	register struct inoinfo *inp;
	struct inoinfo **inpp;
	unsigned int blks;

	blks = uhowmany(dp->di_size, sblock.fs_bsize);
	if (blks > NDADDR)
		blks = NDADDR + NIADDR;
	inp = (struct inoinfo *)
		malloc(sizeof(*inp) + (blks - 1) * sizeof(daddr_t));
	if (inp == NULL)
		return;
	inpp = &inphead[inumber % numdirs];
	inp->i_nexthash = *inpp;
	*inpp = inp;
	inp->i_parent = (ino_t)0;
	inp->i_dotdot = (ino_t)0;
	inp->i_number = inumber;
	inp->i_isize = dp->di_size;
	inp->i_flags = dp->di_flags;
	inp->i_numblks = blks * sizeof(daddr_t);
	bcopy((char *)&dp->di_db[0], (char *)&inp->i_blks[0],
	    (size_t)inp->i_numblks);
	if (inplast == listmax) {
		listmax += 100;
		inpsort = (struct inoinfo **)realloc((char *)inpsort,
		    (unsigned)listmax * sizeof(struct inoinfo *));
		if (inpsort == NULL)
			errexit(MSGSTR(DIRLIST, "cannot increase directory list"));
	}
	inpsort[inplast++] = inp;
}

/*
 * Look up an inode cache structure.
 */
struct inoinfo *
getinoinfo(inumber)
	ino_t inumber;
{
	register struct inoinfo *inp;

	for (inp = inphead[inumber % numdirs]; inp; inp = inp->i_nexthash) {
		if (inp->i_number != inumber)
			continue;
		return (inp);
	}
	errexit(MSGSTR(FIND, "cannot find inode %d\n"), inumber);
	return ((struct inoinfo *)0);
}

/*
 * Clean up all the inode cache structure.
 */
inocleanup()
{
	register struct inoinfo **inpp;

	if (inphead == NULL)
		return;
	for (inpp = &inpsort[inplast - 1]; inpp >= inpsort; inpp--)
		free((char *)(*inpp));
	free((char *)inphead);
	free((char *)inpsort);
	inphead = inpsort = NULL;
}
	
inodirty()
{
	
	dirty(pbp);
}

clri(idesc, type, flag)
	register struct inodesc *idesc;
	char *type;
	int flag;
{
	register struct dinode *dp;

	dp = ginode(idesc->id_number);
	if (flag == 1) {
		pwarn("%s %s", type,
		    (dp->di_mode & IFMT) == IFDIR ? MSGSTR(DIR, "DIR") : MSGSTR(FILE, "FILE"));
		pinode(idesc->id_number);
	}
	if (preen || reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
		if (preen)
			printf(MSGSTR(CLEARED, " (CLEARED)\n"));
		n_files--;
		(void)ckinode(dp, idesc);
		clearinode(dp);
		statemap[idesc->id_number] = USTATE;
		inodirty();
	}
}

findname(idesc)
	struct inodesc *idesc;
{
	register struct dirent *dirp = idesc->id_dirp;

	if (dirp->d_fileno != idesc->id_parent)
		return (KEEPON);
	bcopy(dirp->d_name, idesc->id_name, (size_t)dirp->d_namlen + 1);
	return (STOP|FOUND);
}

findino(idesc)
	struct inodesc *idesc;
{
	register struct dirent *dirp = idesc->id_dirp;

	if (dirp->d_fileno == 0)
		return (KEEPON);
	if (strcmp(dirp->d_name, idesc->id_name) == 0 &&
	    dirp->d_fileno >= ROOTINO && dirp->d_fileno <= maxino) {
		idesc->id_parent = dirp->d_fileno;
		return (STOP|FOUND);
	}
	return (KEEPON);
}

pinode(ino)
	ino_t ino;
{
	register struct dinode *dp;
	register char *p;
	struct passwd *pw;
	char *ctime();

	printf(MSGSTR(IEQ, " I=%lu "), ino);
	if (ino < ROOTINO || ino > maxino)
		return;
	dp = ginode(ino);
	printf(MSGSTR(OWNER, " OWNER="));
	if ((pw = getpwuid((int)dp->di_uid)) != 0)
		printf("%s ", pw->pw_name);
	else
		printf("%u ", (unsigned)dp->di_uid);
	printf(MSGSTR(MODE1, "MODE=%o\n"), dp->di_mode);
	if (preen)
		printf("%s: ", devname);
	printf(MSGSTR(SIZE, "SIZE=%lu "), dp->di_size);
	p = ctime(&dp->di_mtime);
	printf(MSGSTR(MTIME, "MTIME=%12.12s %4.4s "), p + 4, p + 20);
}

blkerror(ino, type, blk)
	ino_t ino;
	char *type;
	daddr_t blk;
{

	pfatal(MSGSTR(BLKERR, "%ld %s I=%lu"), blk, type, ino);
	printf("\n");
	switch (statemap[ino]) {

	case FSTATE:
		statemap[ino] = FCLEAR;
		return;

	case DSTATE:
		statemap[ino] = DCLEAR;
		return;

	case FCLEAR:
	case DCLEAR:
		return;

	default:
		errexit(MSGSTR(BADSTATE, "BAD STATE %d TO BLKERR"), statemap[ino]);
		/* NOTREACHED */
	}
}

/*
 * allocate an unused inode
 */
ino_t
allocino(request, type)
	ino_t request;
	int type;
{
	register ino_t ino;
	register struct dinode *dp;

	if (request == 0)
		request = ROOTINO;
	else if (statemap[request] != USTATE)
		return (0);
	for (ino = request; ino < maxino; ino++)
		if (statemap[ino] == USTATE)
			break;
	if (ino == maxino)
		return (0);
	switch (type & IFMT) {
	case IFDIR:
		statemap[ino] = DSTATE;
		break;
	case IFREG:
	case IFLNK:
		statemap[ino] = FSTATE;
		break;
	default:
		return (0);
	}
	dp = ginode(ino);
	dp->di_db[0] = allocblk((long)1);
	if (dp->di_db[0] == 0) {
		statemap[ino] = USTATE;
		return (0);
	}
	dp->di_mode = type;
	(void)time(&dp->di_atime);
	dp->di_mtime = dp->di_ctime = dp->di_atime;
	dp->di_size = sblock.fs_fsize;
	dp->di_blocks = fsbtodb(&sblock, 1);
#if SEC_FSCHANGE
	/*
	 * Zero extra fields in the security part of the inode
	 */
	bzero((char *) &(((struct sec_dinode *) dp)->di_sec),
	      sizeof(struct dinode_sec));
#endif
	n_files++;
	inodirty();
	return (ino);
}

/*
 * deallocate an inode
 */
freeino(ino)
	ino_t ino;
{
	struct inodesc idesc;
	extern int pass4check();
	struct dinode *dp;

	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = ADDR;
	idesc.id_func = pass4check;
	idesc.id_number = ino;
	dp = ginode(ino);
	(void)ckinode(dp, &idesc);
	clearinode(dp);
	inodirty();
	statemap[ino] = USTATE;
	n_files--;
}
