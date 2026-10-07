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
 * Don't use message catalog for OSF1_ADFS.
 * $Log: pass2.c,v $
 * Revision 1.4  1994/11/19  03:18:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/04  21:46:41  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.2  1992/10/12  22:08:07  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.9.2.2  91/06/03  13:41:48  garyf
 * 	add messaging support
 * 	[91/05/13  11:34:41  garyf]
 * 
 * Revision 2.9  91/01/07  17:23:35  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:59  dwm]
 * 
 * Revision 2.8  90/10/07  22:51:14  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	Use cached di_flags from struct inoinfo.
 * 	[90/10/02  09:39:44  gmf]
 * 
 * Revision 2.7  90/07/27  11:45:27  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:12  pam]
 * 
 * 	More changes for gcc.
 * 	[90/07/08  17:40:21  gm]
 * 
 * Revision 2.6  90/07/17  12:38:53  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:40:21  gm]
 * 
 * Revision 2.5  90/06/22  22:12:56  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:51  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: pass2.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 03:18:16 $";
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

/*** "pass2.c	5.16 (Berkeley) 9/18/90"; ***/

#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#define _KERNEL
#include <ufs/dir.h>
#undef _KERNEL
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

#define MINDIRSIZE	(sizeof (struct dirtemplate))

int	pass2check(), blksort();

pass2()
{
	register struct dinode *dp;
	register struct inoinfo **inpp, *inp;
	struct inoinfo **inpend;
	struct inodesc curino;
	struct dinode dino;
	char pathbuf[MAXPATHLEN + 1];

	switch (statemap[ROOTINO]) {

	case USTATE:
		pfatal(MSGSTR(ROOTI, "ROOT INODE UNALLOCATED"));
		if (reply(MSGSTR(ALLOCATE, "ALLOCATE")) == 0)
			errexit("");
		if (allocdir(ROOTINO, ROOTINO, 0755) != ROOTINO)
			errexit(MSGSTR(NOALLOC, "CANNOT ALLOCATE ROOT INODE\n"));
		break;

	case DCLEAR:
		pfatal(MSGSTR(DUPS, "DUPS/BAD IN ROOT INODE"));
		if (reply(MSGSTR(REALLOCATE, "REALLOCATE"))) {
			freeino(ROOTINO);
			if (allocdir(ROOTINO, ROOTINO, 0755) != ROOTINO)
				errexit(MSGSTR(NOALLOC, "CANNOT ALLOCATE ROOT INODE\n"));
			break;
		}
		if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
			errexit("");
		break;

	case FSTATE:
	case FCLEAR:
		pfatal(MSGSTR(ROOTI1, "ROOT INODE NOT DIRECTORY"));
		if (reply(MSGSTR(REALLOCATE, "REALLOCATE"))) {
			freeino(ROOTINO);
			if (allocdir(ROOTINO, ROOTINO, 0755) != ROOTINO)
				errexit(MSGSTR(NOALLOC, "CANNOT ALLOCATE ROOT INODE\n"));
			break;
		}
		if (reply(MSGSTR(FIX0, "FIX")) == 0)
			errexit("");
		dp = ginode(ROOTINO);
		dp->di_mode &= ~IFMT;
		dp->di_mode |= IFDIR;
		inodirty();
		break;

	case DSTATE:
		break;

	default:
		errexit(MSGSTR(BADSTATE, "BAD STATE %d FOR ROOT INODE"), statemap[ROOTINO]);
	}
	statemap[ROOTINO] = DFOUND;
	/*
	 * Sort the directory list into disk block order.
	 */
	qsort((char *)inpsort, (size_t)inplast, sizeof *inpsort, blksort);
	/*
	 * Check the integrity of each directory.
	 */
	bzero((char *)&curino, sizeof(struct inodesc));
	curino.id_type = DATA;
	curino.id_func = pass2check;
	dino.di_mode = IFDIR;
	dp = &dino;
	inpend = &inpsort[inplast];
	for (inpp = inpsort; inpp < inpend; inpp++) {
		inp = *inpp;
		if (inp->i_isize == 0)
			continue;
		if (inp->i_isize < MINDIRSIZE) {
			direrror(inp->i_number, MSGSTR(DIRSHORT, "DIRECTORY TOO SHORT"));
			inp->i_isize = uroundup(MINDIRSIZE, DIRBLKSIZ);
			if (reply(MSGSTR(FIX0, "FIX")) == 1) {
				dp = ginode(inp->i_number);
				dp->di_size = inp->i_isize;
				inodirty();
				dp = &dino;
			}
		} else if ((inp->i_isize & (DIRBLKSIZ - 1)) != 0) {
			getpathname(pathbuf, inp->i_number, inp->i_number);
			pwarn(MSGSTR(DIRLEN, "DIRECTORY %s: LENGTH %d NOT MULTIPLE OF %d"),
				pathbuf, inp->i_isize, DIRBLKSIZ);
			if (preen)
				printf(MSGSTR(ADJUSTED, " (ADJUSTED)\n"));
			inp->i_isize = uroundup(inp->i_isize, DIRBLKSIZ);
			if (preen || reply(MSGSTR(ADJUST, "ADJUST")) == 1) {
				dp = ginode(inp->i_number);
				dp->di_size = uroundup(inp->i_isize, DIRBLKSIZ);
				inodirty();
				dp = &dino;
			}
		}
		dp->di_size = inp->i_isize;
		bcopy((char *)&inp->i_blks[0], (char *)&dp->di_db[0],
			(size_t)inp->i_numblks);
		dp->di_flags = inp->i_flags;
		curino.id_number = inp->i_number;
		curino.id_parent = inp->i_parent;
		(void)ckinode(dp, &curino);
	}
	/*
	 * Now that the parents of all directories have been found,
	 * make another pass to verify the value of `..'
	 */
	for (inpp = inpsort; inpp < inpend; inpp++) {
		inp = *inpp;
		if (inp->i_parent == 0 || inp->i_isize == 0)
			continue;
		if (statemap[inp->i_parent] == DFOUND &&
		    statemap[inp->i_number] == DSTATE)
			statemap[inp->i_number] = DFOUND;
		if (inp->i_dotdot == inp->i_parent ||
		    inp->i_dotdot == (ino_t)-1)
			continue;
		if (inp->i_dotdot == 0) {
			inp->i_dotdot = inp->i_parent;
			fileerror(inp->i_parent, inp->i_number, MSGSTR(MISSING4, "MISSING '..'"));
			if (reply(MSGSTR(FIX0, "FIX")) == 0)
				continue;
			(void)makeentry(inp->i_number, inp->i_parent, "..");
			lncntp[inp->i_parent]--;
			continue;
		}
		fileerror(inp->i_parent, inp->i_number,
			MSGSTR(BADINUM, "BAD INODE NUMBER FOR '..'"));
		if (reply(MSGSTR(FIX0, "FIX")) == 0)
			continue;
		lncntp[inp->i_dotdot]++;
		lncntp[inp->i_parent]--;
		inp->i_dotdot = inp->i_parent;
		(void)changeino(inp->i_number, "..", inp->i_parent);
	}
	/*
	 * Mark all the directories that can be found from the root.
	 */
	propagate();
}

pass2check(idesc)
	struct inodesc *idesc;
{
	register struct dirent *dirp = idesc->id_dirp;
	register struct inoinfo *inp;
	int n, entrysize, ret = 0;
	struct dinode *dp;
	char *errmsg;
	struct dirent proto;
	char namebuf[MAXPATHLEN + 1];
	char pathbuf[MAXPATHLEN + 1];

	/* 
	 * check for "."
	 */
	if (idesc->id_entryno != 0)
		goto chk1;
	if (dirp->d_fileno != 0 && strcmp(dirp->d_name, ".") == 0) {
		if (dirp->d_fileno != idesc->id_number) {
			direrror(idesc->id_number, MSGSTR(BADINUM1, "BAD INODE NUMBER FOR '.'"));
			dirp->d_fileno = idesc->id_number;
			if (reply(MSGSTR(FIX0, "FIX")) == 1)
				ret |= ALTERED;
		}
		goto chk1;
	}
	direrror(idesc->id_number, MSGSTR(MISSING3, "MISSING '.'"));
	proto.d_fileno = idesc->id_number;
	proto.d_namlen = 1;
	(void)strcpy(proto.d_name, ".");
	entrysize = DIRSIZ(&proto);
	if (dirp->d_fileno != 0 && strcmp(dirp->d_name, "..") != 0) {
		pfatal(MSGSTR(NOFIX0, "CANNOT FIX, FIRST ENTRY IN DIRECTORY CONTAINS %s\n"),
			dirp->d_name);
	} else if (dirp->d_reclen < entrysize) {
		pfatal(MSGSTR(NOFIX1, "CANNOT FIX, INSUFFICIENT SPACE TO ADD '.'\n"));
	} else if (dirp->d_reclen < 2 * entrysize) {
		proto.d_reclen = dirp->d_reclen;
		bcopy((char *)&proto, (char *)dirp, (size_t)entrysize);
		if (reply(MSGSTR(FIX0, "FIX")) == 1)
			ret |= ALTERED;
	} else {
		n = dirp->d_reclen - entrysize;
		proto.d_reclen = entrysize;
		bcopy((char *)&proto, (char *)dirp, (size_t)entrysize);
		idesc->id_entryno++;
		lncntp[dirp->d_fileno]--;
		dirp = (struct dirent *)((char *)(dirp) + entrysize);
		bzero((char *)dirp, (size_t)n);
		dirp->d_reclen = n;
		if (reply(MSGSTR(FIX0, "FIX")) == 1)
			ret |= ALTERED;
	}
chk1:
	if (idesc->id_entryno > 1)
		goto chk2;
	inp = getinoinfo(idesc->id_number);
	proto.d_fileno = inp->i_parent;
	proto.d_namlen = 2;
	(void)strcpy(proto.d_name, "..");
	entrysize = DIRSIZ(&proto);
	if (idesc->id_entryno == 0) {
		n = DIRSIZ(dirp);
		if (dirp->d_reclen < n + entrysize)
			goto chk2;
		proto.d_reclen = dirp->d_reclen - n;
		dirp->d_reclen = n;
		idesc->id_entryno++;
		lncntp[dirp->d_fileno]--;
		dirp = (struct dirent *)((char *)(dirp) + n);
		bzero((char *)dirp, (size_t)proto.d_reclen);
		dirp->d_reclen = proto.d_reclen;
	}
	if (dirp->d_fileno != 0 && strcmp(dirp->d_name, "..") == 0) {
		inp->i_dotdot = dirp->d_fileno;
		goto chk2;
	}
	if (dirp->d_fileno != 0 && strcmp(dirp->d_name, ".") != 0) {
		fileerror(inp->i_parent, idesc->id_number, MSGSTR(MISSING4, "MISSING '..'"));
		pfatal(MSGSTR(NOFIX2, "CANNOT FIX, SECOND ENTRY IN DIRECTORY CONTAINS %s\n"),
			dirp->d_name);
		inp->i_dotdot = (ino_t)-1;
	} else if (dirp->d_reclen < entrysize) {
		fileerror(inp->i_parent, idesc->id_number, MSGSTR(MISSING4, "MISSING '..'"));
		pfatal(MSGSTR(NOFIX3, "CANNOT FIX, INSUFFICIENT SPACE TO ADD '..'\n"));
		inp->i_dotdot = (ino_t)-1;
	} else if (inp->i_parent != 0) {
		/*
		 * We know the parent, so fix now.
		 */
		inp->i_dotdot = inp->i_parent;
		fileerror(inp->i_parent, idesc->id_number, MSGSTR(MISSING4, "MISSING '..'"));
		proto.d_reclen = dirp->d_reclen;
		bcopy((char *)&proto, (char *)dirp, (size_t)entrysize);
		if (reply(MSGSTR(FIX0, "FIX")) == 1)
			ret |= ALTERED;
	}
	idesc->id_entryno++;
	if (dirp->d_fileno != 0)
		lncntp[dirp->d_fileno]--;
	return (ret|KEEPON);
chk2:
	if (dirp->d_fileno == 0)
		return (ret|KEEPON);
	if (dirp->d_namlen <= 2 &&
	    dirp->d_name[0] == '.' &&
	    idesc->id_entryno >= 2) {
		if (dirp->d_namlen == 1) {
			direrror(idesc->id_number, MSGSTR(EXTRA, "EXTRA '.' ENTRY"));
			dirp->d_fileno = 0;
			if (reply(MSGSTR(FIX0, "FIX")) == 1)
				ret |= ALTERED;
			return (KEEPON | ret);
		}
		if (dirp->d_name[1] == '.') {
			direrror(idesc->id_number, MSGSTR(EXTRA1, "EXTRA '..' ENTRY"));
			dirp->d_fileno = 0;
			if (reply(MSGSTR(FIX0, "FIX")) == 1)
				ret |= ALTERED;
			return (KEEPON | ret);
		}
	}
	idesc->id_entryno++;
	n = 0;
	if (dirp->d_fileno > maxino || dirp->d_fileno <= 0) {
		fileerror(idesc->id_number, dirp->d_fileno, MSGSTR(RANGE, "I OUT OF RANGE"));
		n = reply(MSGSTR(REMOVE, "REMOVE"));
	} else {
again:
		switch (statemap[dirp->d_fileno]) {
		case USTATE:
			if (idesc->id_entryno <= 2)
				break;
			fileerror(idesc->id_number, dirp->d_fileno, MSGSTR(UNALLOCATED, "UNALLOCATED"));
			n = reply(MSGSTR(REMOVE, "REMOVE"));
			break;

		case DCLEAR:
		case FCLEAR:
			if (idesc->id_entryno <= 2)
				break;
			if (statemap[dirp->d_fileno] == DCLEAR)
				errmsg = MSGSTR(ZEROLEN1, "ZERO LENGTH DIRECTORY");
			else
				errmsg = MSGSTR(DUPBAD, "DUP/BAD");
			fileerror(idesc->id_number, dirp->d_fileno, errmsg);
			if ((n = reply(MSGSTR(REMOVE, "REMOVE"))) == 1)
				break;
			dp = ginode(dirp->d_fileno);
			statemap[dirp->d_fileno] =
			    (dp->di_mode & IFMT) == IFDIR ? DSTATE : FSTATE;
			lncntp[dirp->d_fileno] = dp->di_nlink;
			goto again;

		case DSTATE:
			if (statemap[idesc->id_number] == DFOUND)
				statemap[dirp->d_fileno] = DFOUND;
			/* fall through */

		case DFOUND:
			inp = getinoinfo(dirp->d_fileno);
			if (inp->i_parent != 0 && idesc->id_entryno > 2) {
				getpathname(pathbuf, idesc->id_number,
				    idesc->id_number);
				getpathname(namebuf, dirp->d_fileno, dirp->d_fileno);
				pwarn("%s %s %s\n", pathbuf,
				    MSGSTR(EXTRAN, "IS AN EXTRANEOUS HARD LINK TO DIRECTORY"),
				    namebuf);
				if (preen)
					printf(MSGSTR(IGNORED, " (IGNORED)\n"));
				else if ((n = reply(MSGSTR(REMOVE, "REMOVE"))) == 1)
					break;
			}
			if (idesc->id_entryno > 2)
				inp->i_parent = idesc->id_number;
			/* fall through */

		case FSTATE:
			lncntp[dirp->d_fileno]--;
			break;

		default:
			errexit(MSGSTR(BADSTATE, "BAD STATE %d FOR INODE I=%d"),
			    statemap[dirp->d_fileno], dirp->d_fileno);
		}
	}
	if (n == 0)
		return (ret|KEEPON);
	dirp->d_fileno = 0;
	return (ret|KEEPON|ALTERED);
}

/*
 * Routine to sort disk blocks.
 */
blksort(inpp1, inpp2)
	struct inoinfo **inpp1, **inpp2;
{

	return ((*inpp1)->i_blks[0] - (*inpp2)->i_blks[0]);
}
