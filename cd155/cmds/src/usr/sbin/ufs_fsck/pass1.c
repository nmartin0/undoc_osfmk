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
 * $Log: pass1.c,v $
 * Revision 1.9  1994/11/19  03:18:15  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/06/29  00:32:55  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.7  1994/03/11  22:02:02  brad
 * Merged revision 1.5.4.3 from the R1.2 branch.
 *
 * Revision 1.5.4.3  1994/03/11  21:52:51  brad
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
 * Revision 1.6  1994/02/16  03:55:26  brad
 * Merged revision 1.5.4.2 from the R1.2 branch.
 *
 * Revision 1.5.4.2  1994/02/16  03:41:17  brad
 * Fixed history logs.
 *
 * Revision 1.5.4.1  1994/02/16  03:32:08  brad
 * Code cleanup in relation to bug fix for PTS #6318.  No change
 * in implementation.
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 6318
 *  Testing: Ran fsck many times on different file systems after
 *     performing extensive lsize testing and running EATs.
 *  Module(s): pass1.c, inode.c
 *
 * Revision 1.5  1993/08/03  16:40:40  wunder
 * Placed PFS printf's for preallocation around ifdefs for debug only. To
 * many unneccessary messages printed for large preallocated files on fsck
 * of filesystem on bootup.
 *
 * Revision 1.4  1993/06/04  21:46:32  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.3  1993/05/27  02:52:25  wunder
 * Added checking for disk block preallocation, see ifdef PFS's.
 *
 * Revision 1.2  1992/10/12  22:08:02  shala
 * New version to support maj, min and node numbers.
 *
 * 03-Aug-92  Paul Roy (roy) at Open Software Foundation
 *	Added BLK_RESERVE_SUPPORT to support filesystems that reserve
 *	blocks by or'ing the high bit of the daddr to 1.  These daddrs
 *	need to be cleared by fsck.
 *      Don't use message catalog for OSF1_ADFS.
 *
 * Revision 2.10.2.4  1992/02/28  17:16:36  garyf
 * 	add check for directories with IC_FASTLINK on
 * 	[1992/02/28  17:15:52  garyf]
 *
 * Revision 2.10.2.3  1991/12/30  23:43:40  garyf
 * 	don't check direct and indirect blocks for fast links
 * 	fix typo in block # check
 * 	[1991/12/30  23:42:43  garyf]
 * 
 * Revision 2.10.2.2  1991/12/12  15:31:04  garyf
 * 	add more checking:
 * 	1) check if device files have non-zero size
 * 	2) verify if direct and indirect block pointers
 * 	   point beyond end of partition
 * 	[1991/12/12  15:30:05  garyf]
 * 
 * Revision 2.10  1991/06/10  16:37:26  devrcs
 * 	add messaging support
 * 	[91/05/13  11:34:22  garyf]
 * 
 * Revision 2.9  91/01/07  17:23:27  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:47  dwm]
 * 
 * Revision 2.8  90/10/07  22:51:01  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:38:44  gmf]
 * 
 * Revision 2.7  90/08/25  12:36:50  devrcs
 * 	OSF/1 must always recognize fast symbolic links.
 * 	[90/08/18  03:54:56  nags]
 * 
 * Revision 2.6  90/07/27  11:45:18  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:30:58  pam]
 * 
 * Revision 2.5  90/06/22  22:12:50  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:23  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pass1.c,v $ $Revision: 1.9 $ (OSF) $Date: 1994/11/19 03:18:15 $";
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

/*** "pass1.c	5.15 (Berkeley) 7/20/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <stdlib.h>
#include <string.h>
#include "fsck.h"

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

static daddr_t badblk;
static daddr_t dupblk;
int pass1check();
struct dinode *getnextinode();

#ifdef PFS
/*
 * Flag that indicates, after pass1 is completed, if a file still contains
 * preallocated blocks.
 */
int	preallocated_blocks;
#endif

pass1()
{
	register int c, i, j;
	register struct dinode *dp;
	struct zlncnt *zlnp;
	int ndb, cgd;
	long newsize;
	struct inodesc idesc;
	ino_t inumber;

	/*
	 * Set file system reserved blocks in used block map.
	 */
	for (c = 0; c < sblock.fs_ncg; c++) {
		cgd = cgdmin(&sblock, c);
		if (c == 0) {
			i = cgbase(&sblock, c);
			cgd += uhowmany(sblock.fs_cssize, sblock.fs_fsize);
		} else
			i = cgsblock(&sblock, c);
		for (; i < cgd; i++)
			setbmap(i);
	}
	/*
	 * Find all allocated blocks.
	 */
	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = ADDR;
	idesc.id_func = pass1check;
	inumber = 0;
	n_files = n_blks = 0;
	resetinodebuf();
	for (c = 0; c < sblock.fs_ncg; c++) {
		for (i = 0; i < sblock.fs_ipg; i++, inumber++) {
			if (inumber < ROOTINO)
				continue;
			dp = getnextinode(inumber);
#ifdef PFS
			preallocated_blocks = FALSE;
#endif
			if ((dp->di_mode & IFMT) == 0) {
				if (bcmp((char *)dp->di_db, (char *)zino.di_db,
					NDADDR * sizeof(daddr_t)) ||
				    bcmp((char *)dp->di_ib, (char *)zino.di_ib,
					NIADDR * sizeof(daddr_t)) ||
#ifdef PFS
				    dp->di_mode || dp->di_size ||
				    dp->di_flags) {
#ifdef DEBUG_PFS
					if ((dp->di_flags & IC_PREALLOCATED) ==
					    IC_PREALLOCATED)
						printf("pass1: I=%lu IS MARKED PREALLOCATED\n",
						       inumber);
#endif
#else
				    dp->di_mode || dp->di_size) {
#endif
					pfatal(MSGSTR(PARTIALA, "PARTIALLY ALLOCATED INODE I=%lu"),
					       inumber);
					if (reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
						dp = ginode(inumber);
						clearinode(dp);
						inodirty();
					}
				}
				statemap[inumber] = USTATE;
				continue;
			}
			if (((dp->di_mode & IFMT) == IFCHR) || ((dp->di_mode & IFMT) == IFBLK)) {
				if (dp->di_size) {
					pfatal(MSGSTR(BADSPCL, "INCORRECT BLOCKS IN SPECIAL FILE INODE I=%lu"),
						inumber);
					if (reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
						dp = ginode(inumber);
						clearinode(dp);
						inodirty();
						statemap[inumber] = USTATE;
					}
					continue;
				}
			}
			lastino = inumber;
			if ( dp->di_size < 0 ||
					(unsigned long)dp->di_size >
					(unsigned long)MAXFILESIZE) {
				if (debug)
					printf(MSGSTR(BADSIZE, "bad size %lu:"), 
						(ulong_t)dp->di_size);
				goto unknown;
			}
			if (!preen && (dp->di_mode & IFMT) == IFMT &&
			    reply(MSGSTR(HOLD, "HOLD BAD BLOCK")) == 1) {
				dp = ginode(inumber);
				dp->di_size = sblock.fs_fsize;
				dp->di_mode = IFREG|0600;
				inodirty();
			}
			ndb = uhowmany(dp->di_size, sblock.fs_bsize);
			if (ndb < 0) {
				if (debug)
					printf(MSGSTR(BADSIZE2, "bad size %lu ndb %d:"),
						(ulong_t)dp->di_size, ndb);
				goto unknown;
			}
			if ((dp->di_mode & IFMT) == IFBLK ||
			    (dp->di_mode & IFMT) == IFCHR)
				ndb++;
			/*
			 * If there are data blocks allocated past the end of
			 * file, loop through them and free them.
			 *
			 * Skip disk block checks for fast symbolic links.
			 */
			if ((dp->di_flags & IC_FASTLINK) == 0) {
				for (j = ndb; j < NDADDR; j++)
#ifdef	BLK_RESERVE_SUPPORT
					/* 
					 * Zero reserved blocks because they
					 * don't contain valid data.  Note
					 * that indirect block pointers should
					 * never be marked reserved.
					 */
					if (IS_RESERVED(dp->di_db[j])) {
					    pwarn(MSGSTR(RESBLOCK,
							 "DIRECT BLOCK (%ld) RESERVED BEYOND EOF I=%lu"),
						  DADDR(dp->di_db[j]),
						  inumber);
					    if (preen ||
						reply(MSGSTR(FREE, "FREE")) == 1) {
						if (preen)
						    printf(MSGSTR(FREED,
								  " (FREED)\n"));
						if ((dp->di_mode & IFMT) !=
						    IFREG)
							pwarn(MSGSTR(RESNOTREG,
								     "WARNING: RESERVED BLOCK NOT FROM FILE (mode=0x%x)\n"),
								     dp->di_mode);
						dp = ginode(inumber);
						dp->di_db[j] = 0;
						inodirty();
					    }
					} else 
#endif
					if (dp->di_db[j] != 0) {
					    if (debug)
						printf(MSGSTR(BADDIRECT, "bad direct addr: %ld\n"),
							dp->di_db[j]);
					    goto unknown;
					}

				for (j = 0, ndb -= NDADDR; ndb > 0; j++)
					ndb /= NINDIR(&sblock);
				
				for (; j < NIADDR; j++)
					if (dp->di_ib[j] != 0) {
#ifdef	BLK_RESERVE_SUPPORT
					    pwarn(MSGSTR(INRESBLOCK,
							 "INDIRECT BLOCK (%ld) RESERVED BEYOND EOF I=%lu"),
						  DADDR(dp->di_ib[j]),
						  inumber);
					    if (preen ||
						reply(MSGSTR(FREE, "FREE")) == 1) {
						if (preen)
						    printf(MSGSTR(FREED,
								  " (FREED)\n"));
						dp = ginode(inumber);
						dp->di_ib[j] = 0;
						inodirty();
					    }
#else
					    if (debug)
					       printf(MSGSTR(BADINDDIR, "bad indirect addr: %ld\n"),
						       dp->di_ib[j]);
					    goto unknown;
#endif
					}
			}

			/*
			 * Now loop through data blocks allocated
			 * inside the valid range of file data.
			 */
			if (((dp->di_mode & IFMT) != IFBLK) &&
			    ((dp->di_mode & IFMT) != IFCHR) &&
			    (((dp->di_mode & IFMT) != IFLNK) ||
			     (((dp->di_mode & IFMT) == IFLNK) &&
			      ((dp->di_flags & IC_FASTLINK) == 0))) ) {
				for (j=0; j < NDADDR; j++)
#ifdef	BLK_RESERVE_SUPPORT
					/* 
					 * Zero reserved blocks because
					 * they don't contain valid data.
					 * 
					 * A note about the file size:
					 * because of the way UFS has always
					 * worked it's possible the file
					 * size in the inode is too large
					 * relative to the disk blocks that
					 * have been allocated.  The block
					 * reserve support does not exacerbate
					 * this problem, but we sanity check
					 * below anyways.
					 */
					if (IS_RESERVED(dp->di_db[j])) {
#ifdef PFS
					    if (dp->di_flags & IC_PREALLOCATED) {
#ifdef DEBUG_PFS
						printf("pass1: LOCATED PREALLOCATED DADDR=%ld IN I=%lu\n",
						       dp->di_db[j], inumber);
#endif
						preallocated_blocks = TRUE;
					    } else {
#endif
						pwarn(MSGSTR(RESBLOCK2,
							     "DIRECT BLOCK (%ld) IN I=%lu IS RESERVED"),
						      DADDR(dp->di_db[j]),
						      inumber);
						if (preen ||
						    reply(MSGSTR(FREE,
								 "FREE")) == 1) {
							if (preen)
							    printf(MSGSTR(FREED,
									  " (FREED)\n"));
							if ((dp->di_mode & IFMT) != IFREG)
							    pwarn(MSGSTR(RESNOTREG,
									 "WARNING: RESERVED BLOCK NOT FROM FILE (mode=0x%x)\n"),
								  dp->di_mode);
							dp = ginode(inumber);
							dp->di_db[j] = 0;
							/* sanity check the size */
							if (dp->di_size > 
							    j*sblock.fs_bsize &&
							    dp->di_size <= 
							    (j+1)*sblock.fs_bsize) {
							        pwarn(MSGSTR(SIZETOOBIG,
									     "WARNING: FILE SIZE TOO LARGE I=%lu (size=%lu)\n"),
									     inumber,
									     dp->di_size);
							}
							inodirty();
						}
					    }
					} else 
#endif
					if (dp->di_db[j] > sblock.fs_size) {
					        printf(MSGSTR(BADDIRECT,
							      "bad direct addr: %ld, in: %d\n"),
						       dp->di_db[j], inumber);
						if (reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
							dp = ginode(inumber);
							clearinode(dp);
							inodirty();
							statemap[inumber] = USTATE;
					        }
					        goto nextinode;
					}
				for (j=0; j < NIADDR; j++)
					if (dp->di_ib[j] > sblock.fs_size) {
						printf(MSGSTR(BADINDDIR,
						"bad indirect addr: %ld, in: %d\n"),
						 dp->di_ib[j], inumber);
						if (reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
							dp = ginode(inumber);
							clearinode(dp);
							inodirty();
							statemap[inumber] = USTATE;
						}
						goto nextinode;
					}
			}
			if (ftypeok(dp) == 0)
				goto unknown;
			n_files++;
			lncntp[inumber] = dp->di_nlink;
			if (dp->di_nlink <= 0) {
				zlnp = (struct zlncnt *)malloc(sizeof *zlnp);
				if (zlnp == NULL) {
					pfatal(MSGSTR(LINKCTT, "LINK COUNT TABLE OVERFLOW"));
					if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
						errexit("");
				} else {
					zlnp->zlncnt = inumber;
					zlnp->next = zlnhead;
					zlnhead = zlnp;
				}
			}
			if ((dp->di_mode & IFMT) == IFDIR) {
				if (dp->di_flags != 0) {
					dp = ginode(inumber);
					printf(MSGSTR(FASTLINK, "FASTLINK on for directory, in: %d "), inumber);
					printf(MSGSTR(CORRECTED, " (CORRECTED)\n"));
					dp->di_flags = 0;
					inodirty();
				}
				if (dp->di_size == 0)
					statemap[inumber] = DCLEAR;
				else
					statemap[inumber] = DSTATE;
				cacheino(dp, inumber);
			} else
				statemap[inumber] = FSTATE;
			badblk = dupblk = 0;
			idesc.id_number = inumber;
			(void)ckinode(dp, &idesc);
			correct_blocks(&idesc, dp->di_blocks, inumber, dp);
			continue;
	unknown:
			pfatal(MSGSTR(UNKNOWN1, "UNKNOWN FILE TYPE I=%lu"), inumber);
			statemap[inumber] = FCLEAR;
			if (reply(MSGSTR(CLEAR, "CLEAR")) == 1) {
				statemap[inumber] = USTATE;
				dp = ginode(inumber);
				clearinode(dp);
				inodirty();
			}

	nextinode:	;
		}
	}
	freeinodebuf();
}

pass1check(idesc)
	register struct inodesc *idesc;
{
	int res = KEEPON;
	int anyout, nfrags;
	daddr_t blkno = idesc->id_blkno;
	register struct dups *dlp;
	struct dups *new;

	if ((anyout = chkrange(blkno, idesc->id_numfrags)) != 0) {
		blkerror(idesc->id_number, MSGSTR(BAD, "BAD"), blkno);
		if (++badblk >= MAXBAD) {
			pwarn(MSGSTR(EXCESS, "EXCESSIVE BAD BLKS I=%lu"),
				idesc->id_number);
			if (preen)
				printf(MSGSTR(SKIPPING, " (SKIPPING)\n"));
			else if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
				errexit("");
			return (STOP);
		}
	}
	for (nfrags = idesc->id_numfrags; nfrags > 0; blkno++, nfrags--) {
		if (anyout && chkrange(blkno, 1)) {
			res = SKIP;
		} else if (!testbmap(blkno)) {
			n_blks++;
			setbmap(blkno);
		} else {
			blkerror(idesc->id_number, MSGSTR(DUP, "DUP"), blkno);
			if (++dupblk >= MAXDUP) {
				pwarn("EXCESSIVE DUP BLKS I=%lu",
					idesc->id_number);
				if (preen)
					printf(" (SKIPPING)\n");
				else if (reply("CONTINUE") == 0)
					errexit("");
				return (STOP);
			}
			new = (struct dups *)malloc(sizeof(struct dups));
			if (new == NULL) {
				pfatal(MSGSTR(DUPTAB, "DUP TABLE OVERFLOW."));
				if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
					errexit("");
				return (STOP);
			}
			new->dup = blkno;
			if (muldup == 0) {
				duplist = muldup = new;
				new->next = 0;
			} else {
				new->next = muldup->next;
				muldup->next = new;
			}
			for (dlp = duplist; dlp != muldup; dlp = dlp->next)
				if (dlp->dup == blkno)
					break;
			if (dlp == muldup && dlp->dup != blkno)
				muldup = new;
		}
		/*
		 * count the number of blocks found in id_entryno
		 */
		idesc->id_entryno++;
	}
	return (res);
}

correct_blocks(idesc, blocks, inumber, dp)
	register struct inodesc *idesc;
        long blocks;
	ino_t inumber;
        struct dinode *dp;
{
        long frags = idesc->id_entryno;

        idesc->id_entryno = fsbtodb(&sblock, frags);

	if (blocks != idesc->id_entryno) {
		if (old_fsbtodb >= 0 && (blocks == frags << old_fsbtodb))
			blocks = idesc->id_entryno;
		if (blocks != idesc->id_entryno) {
	        	pwarn(MSGSTR(BADBLOCK, 
			     "INCORRECT BLOCK COUNT I=%lu (%ld should be %ld)"),
			      idesc->id_number, blocks, idesc->id_entryno);
			if (preen)
				printf(MSGSTR(CORRECTED, " (CORRECTED)\n"));
			else if (reply(MSGSTR(CORRECT, "CORRECT")) == 0)
				return;
		}
		dp = ginode(idesc->id_number);
		dp->di_blocks = idesc->id_entryno;
		inodirty();
	}	
#ifdef PFS
	if ((preallocated_blocks == FALSE) &&
	    (dp->di_flags & IC_PREALLOCATED)==IC_PREALLOCATED) {
#ifdef DEBUG_PFS
		printf("pass1: CLEARING PREALLOCATED FLAG\n");
#endif
		dp = ginode(inumber);
		dp->di_flags &= (~IC_PREALLOCATED);
		inodirty();
	}
#ifdef DEBUG_PFS
	else if ((dp->di_flags & IC_PREALLOCATED)==IC_PREALLOCATED) {
		  printf("pass1: NOT CLEARING PREALLOCATED FLAG\n");
	}
#endif
#endif
	return;
}
