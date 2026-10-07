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
 *	Don't use rpmatch in 1.0.4 environment.
 *      Don't use message catalog for OSF1_ADFS.
 *
 * $Log: utilities.c,v $
 * Revision 1.6  1994/11/19  03:18:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/07/15  20:10:27  shala
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: Bug #9941
 *  Testing: Made a new ufs_fsck and fsck'ed file systems.
 *  Module(s): cmds_libs/src/usr/sbin/ufs_fsck/main.c
 *             cmds_libs/src/usr/sbin/ufs_fsck/preen.c
 *             cmds_libs/src/usr/sbin/ufs_fsck/utilities.c
 *
 * Close file descriptors before exiting the programs.
 *
 * Revision 1.4  1994/06/29  00:36:15  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.3  1994/05/10  17:29:50  shala
 *  Risk: Low
 *  Benefit or PTS #: Bug #9344
 *  Testing: Build a new fsck and check on the message
 *  Module(s): cmds_libs/src/usr/sbin/ufs_fsck/utilities.c
 *
 * Remove message "RUN fsck MANUALLY" since it was misleading with
 * our version of bcheckrc.
 *
 * Revision 1.2  1992/10/12  22:08:31  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.12.2.2  91/10/09  13:55:13  sue
 * 	Fix bug 3260.  Added increments of no_cnt and fatal_cnt when they occur.
 * 	[91/10/09  13:53:40  sue]
 * 
 * Revision 2.12  91/08/16  09:54:43  devrcs
 * 	Update for new NLS.
 * 	 Replace NLyesno with rpmatch.
 * 	[91/07/22  17:23:41  tom]
 * 
 * Revision 2.11  91/06/10  16:37:50  devrcs
 * 	add messaging support
 * 	including yes/no support
 * 	[91/05/13  11:35:26  garyf]
 * 
 * Revision 2.10  91/01/07  17:24:08  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:33:47  dwm]
 * 
 * Revision 2.9  90/10/31  15:41:02  devrcs
 * 	Fix problem in ckfini in which it calls flush with
 * 	null bp (can happen if interrupted early in fsck).
 * 	[90/10/12  14:53:53  gmf]
 * 
 * 	don't automatically close fswritefd
 * 	[90/10/09  13:38:59  gmf]
 * 
 * Revision 2.8  90/10/07  22:51:57  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:41:55  gmf]
 * 
 * Revision 2.7  90/07/27  11:45:56  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:32:11  pam]
 * 
 * 	More changes for gcc.
 * 	[90/07/08  17:40:27  gm]
 * 
 * Revision 2.6  90/07/17  12:38:57  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:40:27  gm]
 * 
 * Revision 2.5  90/06/22  22:13:15  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:32:31  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: utilities.c,v $ $Revision: 1.6 $ (OSF) $Date: 1994/11/19 03:18:24 $";
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

/*** "utilities.c	5.29 (Berkeley) 7/27/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <ufs/dir.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "fsck.h"

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include <langinfo.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

long	diskreads, totalreads;	/* Disk cache statistics */
long	lseek();

ftypeok(dp)
	struct dinode *dp;
{
	switch (dp->di_mode & IFMT) {

	case IFDIR:
	case IFREG:
	case IFBLK:
	case IFCHR:
	case IFLNK:
	case IFSOCK:
	case IFIFO:
		return (1);

	default:
		if (debug)
			printf(MSGSTR(BADFILE, "bad file type 0%o\n"), dp->di_mode);
		return (0);
	}
}

reply(question)
	char *question;
{
	int persevere;
	char c;
	char response[2];

	if (preen)
		pfatal(MSGSTR(INTERNAL1, "INTERNAL ERROR: GOT TO reply()"));
	persevere = !strcmp(question, MSGSTR(CONTINUE, "CONTINUE"));
	printf("\n");
	if (!persevere && (nflag || fswritefd < 0)) {
		printf(MSGSTR(NO, "%s? no\n\n"), question);
		no_cnt++;
		return (0);
	}
	if (yflag || (persevere && nflag)) {
		printf(MSGSTR(YES, "%s? yes\n\n"), question);
		return (1);
	}
	response[0] = '\0';
	response[1] = '\0';
	do	{
		printf(MSGSTR(YESNO, "%s? [yn] "), question);
		(void) fflush(stdout);
		c = getc(stdin);
		while (c != '\n' && getc(stdin) != '\n')
			if (feof(stdin))
				return (0);
		response[0] = c;
 	} while (c != 'y' && c != 'Y' && c != 'n' && c != 'N');
	printf("\n");
 	if (c == 'y' || c == 'Y')
		return (1);
	no_cnt++;
	return (0);
}

/*
 * Malloc buffers and set up cache.
 */
bufinit()
{
	register struct bufarea *bp;
	long bufcnt, i;
	char *bufp;

	pbp = pdirbp = (struct bufarea *)0;
	bufp = malloc((unsigned int)sblock.fs_bsize);
	if (bufp == 0)
		errexit(MSGSTR(NOALLOC6, "cannot allocate buffer pool\n"));
	cgblk.b_un.b_buf = bufp;
	initbarea(&cgblk);
	bufhead.b_next = bufhead.b_prev = &bufhead;
	bufcnt = MAXBUFSPACE / sblock.fs_bsize;
	if (bufcnt < MINBUFS)
		bufcnt = MINBUFS;
	for (i = 0; i < bufcnt; i++) {
		bp = (struct bufarea *)malloc(sizeof(struct bufarea));
		bufp = malloc((unsigned int)sblock.fs_bsize);
		if (bp == NULL || bufp == NULL) {
			if (i >= MINBUFS)
				break;
			errexit(MSGSTR(NOALLOC6, "cannot allocate buffer pool\n"));
		}
		bp->b_un.b_buf = bufp;
		bp->b_prev = &bufhead;
		bp->b_next = bufhead.b_next;
		bufhead.b_next->b_prev = bp;
		bufhead.b_next = bp;
		initbarea(bp);
	}
	bufhead.b_size = i;	/* save number of buffers */
}

/*
 * Manage a cache of directory blocks.
 */
struct bufarea *
getdatablk(blkno, size)
	daddr_t blkno;
	long size;
{
	register struct bufarea *bp;

	for (bp = bufhead.b_next; bp != &bufhead; bp = bp->b_next)
		if (bp->b_bno == fsbtodb(&sblock, blkno))
			goto foundit;
	for (bp = bufhead.b_prev; bp != &bufhead; bp = bp->b_prev)
		if ((bp->b_flags & B_INUSE) == 0)
			break;
	if (bp == &bufhead)
		errexit(MSGSTR(DEADLOCK, "deadlocked buffer pool\n"));
	getblk(bp, blkno, size);
	/* fall through */
foundit:
	totalreads++;
	bp->b_prev->b_next = bp->b_next;
	bp->b_next->b_prev = bp->b_prev;
	bp->b_prev = &bufhead;
	bp->b_next = bufhead.b_next;
	bufhead.b_next->b_prev = bp;
	bufhead.b_next = bp;
	bp->b_flags |= B_INUSE;
	return (bp);
}

void
getblk(bp, blk, size)
	register struct bufarea *bp;
	daddr_t blk;
	long size;
{
	daddr_t dblk;

	dblk = fsbtodb(&sblock, blk);
	if (bp->b_bno != dblk) {
		flush(fswritefd, bp);
		diskreads++;
		bp->b_errs = bread(fsreadfd, bp->b_un.b_buf, dblk, size);
		bp->b_bno = dblk;
		bp->b_size = size;
	}
}

flush(fd, bp)
	int fd;
	register struct bufarea *bp;
{
	register int i, j;

	if (!bp->b_dirty)
		return;
	if (bp->b_errs != 0)
		pfatal(MSGSTR(WRITING, "WRITING %sZERO'ED BLOCK %d TO DISK\n"),
		    (bp->b_errs == bp->b_size / secsize) ? "" : MSGSTR(PARTIALLY, "PARTIALLY "),
		    bp->b_bno);
	bp->b_dirty = 0;
	bp->b_errs = 0;
	bwrite(fd, bp->b_un.b_buf, bp->b_bno, (long)bp->b_size);
	if (bp != &sblk)
		return;
	for (i = 0, j = 0; i < sblock.fs_cssize; i += sblock.fs_bsize, j++) {
		bwrite(fswritefd, (char *)sblock.fs_csp[j],
		    fsbtodb(&sblock, sblock.fs_csaddr + j * sblock.fs_frag),
		    sblock.fs_cssize - i < sblock.fs_bsize ?
		    sblock.fs_cssize - i : sblock.fs_bsize);
	}
}

rwerror(mesg, blk)
	char *mesg;
	daddr_t blk;
{

	if (preen == 0)
		printf("\n");
	pfatal(MSGSTR(CANNOT, "CANNOT %s: BLK %ld"), mesg, blk);
	if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
		errexit(MSGSTR(TERMINATED, "Program terminated\n"));
}

ckfini()
{
	register struct bufarea *bp, *nbp;
	extern	int sboff;
	int cnt = 0;

	if (fswritefd < 0) {
		(void)close(fsreadfd);
		return;
	}
	flush(fswritefd, &sblk);
	if (havesb && sblk.b_bno != sboff / btodu(sboff) &&
	    !preen && reply(MSGSTR(UPDSTD,  "UPDATE STANDARD SUPERBLOCK"))) {
		sblk.b_bno = btodu(sboff);
		sbdirty();
		flush(fswritefd, &sblk);
	}
	flush(fswritefd, &cgblk);
	free(cgblk.b_un.b_buf);
	for (bp = bufhead.b_prev; bp && (bp != &bufhead); bp = nbp) {
		cnt++;
		flush(fswritefd, bp);
		nbp = bp->b_prev;
		free(bp->b_un.b_buf);
		free((char *)bp);
	}
	if (bufhead.b_size != cnt)
		errexit(MSGSTR(PANICBUF, "Panic: lost %d buffers\n"), bufhead.b_size - cnt);
	pbp = pdirbp = (struct bufarea *)0;
	if (debug)
		printf(MSGSTR(CACHEMISS, "cache missed %ld of %ld (%d%%)\n"), diskreads,
		    totalreads, (int)(diskreads * 100 / totalreads));
	(void)close(fsreadfd);
	(void)close(fswritefd);
}

bread(fd, buf, blk, size)
	int fd;
	char *buf;
	daddr_t blk;
	long size;
{
	char *cp;
	int i, errs;

	if (lseek(fd, dutob(blk), 0) < 0)
		rwerror(MSGSTR(SEEK, "SEEK"), blk);
	else if (read(fd, buf, (int)size) == size)
		return (0);
	rwerror(MSGSTR(READ, "READ"), blk);
	if (lseek(fd, dutob(blk), 0) < 0)
		rwerror(MSGSTR(SEEK, "SEEK"), blk);
	errs = 0;
	bzero(buf, (size_t)size);
	printf(MSGSTR(NOTREAD, "THE FOLLOWING DISK SECTORS COULD NOT BE READ:"));
	for (cp = buf, i = 0; i < size; i += secsize, cp += secsize) {
		if (read(fd, cp, (int)secsize) != secsize) {
			(void)lseek(fd, dutob(blk) + i + secsize, 0);
			if (secsize != DISK_GRANULE)
				printf(" %ld (%ld),",
				    (dutob(blk) + i) / secsize,
				    blk + btodu(i));
			else
				printf(" %ld,", blk + btodu(i));
			errs++;
		}
	}
	printf("\n");
	return (errs);
}

bwrite(fd, buf, blk, size)
	int fd;
	char *buf;
	daddr_t blk;
	long size;
{
	int i;
	char *cp;

	if (fd < 0)
		return;
	if (lseek(fd, dutob(blk), 0) < 0)
		rwerror(MSGSTR(SEEK, "SEEK"), blk);
	else if (write(fd, buf, (int)size) == size) {
		fsmodified = 1;
		return;
	}
	rwerror(MSGSTR(WRITE, "WRITE"), blk);
	if (lseek(fd, dutob(blk), 0) < 0)
		rwerror(MSGSTR(SEEK, "SEEK"), blk);
	printf(MSGSTR(NOTWRITE, "THE FOLLOWING SECTORS COULD NOT BE WRITTEN:"));
	for (cp = buf, i = 0; i < size; i += secsize, cp += secsize)
		if (write(fd, cp, (int)secsize) != secsize) {
			(void)lseek(fd, dutob(blk) + i + secsize, 0);
			if (secsize != DISK_GRANULE)
				printf(" %ld (%ld),",
					(dutob(blk) + i) / secsize,
					blk + btodu(i));
			else
				printf(" %ld,", blk + btodu(i));
		}
	printf("\n");
	return;
}

/*
 * allocate a data block with the specified number of fragments
 */
allocblk(frags)
	long frags;
{
	register int i, j, k;

	if (frags <= 0 || frags > sblock.fs_frag)
		return (0);
	for (i = 0; i < maxfsblock - sblock.fs_frag; i += sblock.fs_frag) {
		for (j = 0; j <= sblock.fs_frag - frags; j++) {
			if (testbmap(i + j))
				continue;
			for (k = 1; k < frags; k++)
				if (testbmap(i + j + k))
					break;
			if (k < frags) {
				j += k;
				continue;
			}
			for (k = 0; k < frags; k++)
				setbmap(i + j + k);
			n_blks += frags;
			return (i + j);
		}
	}
	return (0);
}

/*
 * Free a previously allocated block
 */
freeblk(blkno, frags)
	daddr_t blkno;
	long frags;
{
	struct inodesc idesc;

	idesc.id_blkno = blkno;
	idesc.id_numfrags = frags;
	(void)pass4check(&idesc);
}

/*
 * Find a pathname
 */
getpathname(namebuf, curdir, ino)
	char *namebuf;
	ino_t curdir, ino;
{
	int len;
	register char *cp;
	struct inodesc idesc;
	static int busy = 0;
	extern int findname();

	if (busy ||
	   (statemap[curdir] != DSTATE && statemap[curdir] != DFOUND)) {
		(void)strcpy(namebuf, "?");
		return;
	}
	busy = 1;
	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = DATA;
	idesc.id_fix = IGNORE;
	cp = &namebuf[MAXPATHLEN - 1];
	*cp = '\0';
	if (curdir != ino) {
		idesc.id_parent = curdir;
		goto namelookup;
	}
	while (ino != ROOTINO) {
		idesc.id_number = ino;
		idesc.id_func = findino;
		idesc.id_name = "..";
		if ((ckinode(ginode(ino), &idesc) & FOUND) == 0)
			break;
	namelookup:
		idesc.id_number = idesc.id_parent;
		idesc.id_parent = ino;
		idesc.id_func = findname;
		idesc.id_name = namebuf;
		if ((ckinode(ginode(idesc.id_number), &idesc)&FOUND) == 0)
			break;
		len = strlen(namebuf);
		cp -= len;
		if (cp < &namebuf[NAME_MAX])
			break;
		bcopy(namebuf, cp, (size_t)len);
		*--cp = '/';
		ino = idesc.id_number;
	}
	busy = 0;
	if (ino != ROOTINO) {
		(void)strcpy(namebuf, "?");
		return;
	}
	bcopy(cp, namebuf, (size_t)(&namebuf[MAXPATHLEN] - cp));
}

void
catch()
{
	ckfini();
	exit(12);
}

/*
 * When preening, allow a single quit to signal
 * a special exit after filesystem checks complete
 * so that reboot sequence may be interrupted.
 */
void
catchquit()
{
	extern returntosingle;

	printf(MSGSTR(SINGLE, "returning to single-user after filesystem check\n"));
	returntosingle = 1;
	(void)signal(SIGQUIT, SIG_DFL);
}

/*
 * Ignore a single quit signal; wait and flush just in case.
 * Used by child processes in preen.
 */
void
voidquit()
{

	sleep(1);
	(void)signal(SIGQUIT, SIG_IGN);
	(void)signal(SIGQUIT, SIG_DFL);
}

/*
 * determine whether an inode should be fixed.
 */
dofix(idesc, msg)
	register struct inodesc *idesc;
	char *msg;
{

	switch (idesc->id_fix) {

	case DONTKNOW:
		if (idesc->id_type == DATA)
			direrror(idesc->id_number, msg);
		else
			pwarn(msg);
		if (preen) {
			printf(MSGSTR(SALVAGED, " (SALVAGED)\n"));
			idesc->id_fix = FIX;
			return (ALTERED);
		}
		if (reply(MSGSTR(SALVAGE, "SALVAGE")) == 0) {
			idesc->id_fix = NOFIX;
			return (0);
		}
		idesc->id_fix = FIX;
		return (ALTERED);

	case FIX:
		return (ALTERED);

	case NOFIX:
	case IGNORE:
		return (0);

	default:
		errexit(MSGSTR(UNKNOWN3, "UNKNOWN INODESC FIX MODE %d\n"), idesc->id_fix);
	}
	/* NOTREACHED */
}

/* VARARGS1 */
errexit(s1, s2, s3, s4)
	char *s1;
{
	printf(s1, s2, s3, s4);
        (void)close(fsreadfd);
        (void)close(fswritefd);
	exit(8);
}

/*
 * An unexpected inconsistency occured.
 * Die if preening, otherwise just print message and continue.
 */
/* VARARGS1 */
pfatal(s, a1, a2, a3)
	char *s;
{

	if (preen) {
		printf("%s: ", devname);
		printf(s, a1, a2, a3);
		printf("\n");
		printf(MSGSTR(UNEXP, "%s: UNEXPECTED INCONSISTENCY.\n"),
			devname);
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit(8);
	}
	fatal_cnt++;
	printf(s, a1, a2, a3);
}

/*
 * Pwarn just prints a message when not preening,
 * or a warning (preceded by filename) when preening.
 */
/* VARARGS1 */
pwarn(s, a1, a2, a3, a4, a5, a6)
	char *s;
{

	if (preen)
		printf("%s: ", devname);
	printf(s, a1, a2, a3, a4, a5, a6);
}

#ifndef lint
/*
 * Stub for routines from kernel.
 */
panic(s)
	char *s;
{

	pfatal(MSGSTR(INTERNAL2, "INTERNAL INCONSISTENCY:"));
	errexit(s);
}
#endif
