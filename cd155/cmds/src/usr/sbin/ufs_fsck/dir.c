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
 * $Log: dir.c,v $
 * Revision 1.5  1994/11/19  03:18:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/29  00:30:32  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.3  1993/06/04  21:45:56  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.2  1992/10/12  22:07:52  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.10.2.4  1992/04/08  17:20:06  hosking
 * 	fix security build errors
 * 	[1992/04/08  17:19:00  hosking]
 *
 * Revision 2.10.2.3  1992/03/11  17:07:17  hosking
 * 	bug 4373: modify previous change to make non-security version build right
 * 	[1992/03/11  17:06:48  hosking]
 * 
 * Revision 2.10.2.2  1992/03/10  14:04:21  hosking
 * 	bug 4373: do MAC checks before reconnecting to lost+found
 * 	[1992/03/10  14:02:54  hosking]
 * 
 * Revision 2.9.2.2  91/06/03  13:41:12  garyf
 * 	use local BUFSIZ definition
 * 	[91/06/03  13:20:09  garyf]
 * 
 * 	add messaging support
 * 	[91/05/13  11:33:41  garyf]
 * 
 * Revision 2.9  91/01/07  17:23:08  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:21  dwm]
 * 
 * Revision 2.8  90/10/07  22:50:41  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:34:06  gmf]
 * 
 * Revision 2.7  90/07/27  11:45:03  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:30:25  pam]
 * 
 * 	Security hooks from SecureWare.
 * 	[90/07/19  21:35:58  seiden]
 * 
 * 	Initial security hooks from SecureWare.
 * 	[90/07/12  12:56:23  seiden]
 * 
 * 	     More changes for gcc.
 * 	     [90/07/08  17:40:02  gm]
 * 	[90/07/10  00:00:09  gm]
 * 
 * Revision 2.6  90/07/17  12:38:42  devrcs
 * 	More changes for gcc.
 * 	[90/07/08  17:40:02  gm]
 * 
 * Revision 2.5  90/06/22  22:12:37  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:30:58  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dir.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 03:18:10 $";
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

/*** "dir.c	5.18 (Berkeley) 7/20/90"; ***/

#include <sys/param.h>
#include <sys/secdefines.h>
#if SEC_MAC
#include <dirent.h>
#endif

#define _KERNEL
#include <ufs/dir.h>
#undef _KERNEL

#if SEC_BASE
#include <sys/security.h>
#if SEC_ARCH
#include <sys/secpolicy.h>
#endif
#endif
#if SEC_MAC
#include <mandatory.h>
#endif

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

char	*lfname = "lost+found";
int	lfmode = 01777;
struct	dirtemplate emptydir = { 0, DIRBLKSIZ };
struct	dirtemplate dirhead = { 0, 12, 1, ".", 0, DIRBLKSIZ - 12, 2, ".." };

struct dirent	*fsck_readdir();
struct bufarea	*getdirblk();

/*
 * Propagate connected state through the tree.
 */
propagate()
{
	register struct inoinfo **inpp, *inp;
	struct inoinfo **inpend;
	long change;

	inpend = &inpsort[inplast];
	do {
		change = 0;
		for (inpp = inpsort; inpp < inpend; inpp++) {
			inp = *inpp;
			if (inp->i_parent == 0)
				continue;
			if (statemap[inp->i_parent] == DFOUND &&
			    statemap[inp->i_number] == DSTATE) {
				statemap[inp->i_number] = DFOUND;
				change++;
			}
		}
	} while (change > 0);
}

/*
 * Scan each entry in a directory block.
 */
dirscan(idesc)
	register struct inodesc *idesc;
{
	register struct dirent *dp;
	register struct bufarea *bp;
	int dsize, n;
	long blksiz;
	char dbuf[DIRBLKSIZ];

	if (idesc->id_type != DATA)
		errexit(MSGSTR(WRONGT, "wrong type to dirscan %d\n"), idesc->id_type);
	if (idesc->id_entryno == 0 &&
	    (idesc->id_filesize & (DIRBLKSIZ - 1)) != 0)
		idesc->id_filesize = uroundup(idesc->id_filesize, DIRBLKSIZ);
	blksiz = idesc->id_numfrags * sblock.fs_fsize;
	if (chkrange(idesc->id_blkno, idesc->id_numfrags)) {
		idesc->id_filesize -= blksiz;
		return (SKIP);
	}
	idesc->id_loc = 0;
	for (dp = fsck_readdir(idesc); dp != NULL; dp = fsck_readdir(idesc)) {
		dsize = dp->d_reclen;
		bcopy((char *)dp, dbuf, (size_t)dsize);
		idesc->id_dirp = (struct dirent *)dbuf;
		if ((n = (*idesc->id_func)(idesc)) & ALTERED) {
			bp = getdirblk(idesc->id_blkno, blksiz);
			bcopy(dbuf, bp->b_un.b_buf + idesc->id_loc - dsize,
			    (size_t)dsize);
			dirty(bp);
			sbdirty();
		}
		if (n & STOP) 
			return (n);
	}
	return (idesc->id_filesize > 0 ? KEEPON : STOP);
}

/*
 * get next entry in a directory.
 */
struct dirent *
fsck_readdir(idesc)
	register struct inodesc *idesc;
{
	register struct dirent *dp, *ndp;
	register struct bufarea *bp;
	long size, blksiz, fix;

	blksiz = idesc->id_numfrags * sblock.fs_fsize;
	bp = getdirblk(idesc->id_blkno, blksiz);
	if (idesc->id_loc % DIRBLKSIZ == 0 && idesc->id_filesize > 0 &&
	    idesc->id_loc < blksiz) {
		dp = (struct dirent *)(bp->b_un.b_buf + idesc->id_loc);
		if (dircheck(idesc, dp))
			goto dpok;
		idesc->id_loc += DIRBLKSIZ;
		idesc->id_filesize -= DIRBLKSIZ;
		fix = dofix(idesc, MSGSTR(DIRBAD, "DIRECTORY CORRUPTED"));
		bp = getdirblk(idesc->id_blkno, blksiz);
		dp = (struct dirent *)(bp->b_un.b_buf + idesc->id_loc);
		dp->d_reclen = DIRBLKSIZ;
		dp->d_fileno = 0;
		dp->d_namlen = 0;
		dp->d_name[0] = '\0';
		if (fix)
			dirty(bp);
		return (dp);
	}
dpok:
	if (idesc->id_filesize <= 0 || idesc->id_loc >= blksiz)
		return NULL;
	dp = (struct dirent *)(bp->b_un.b_buf + idesc->id_loc);
	idesc->id_loc += dp->d_reclen;
	idesc->id_filesize -= dp->d_reclen;
	if ((idesc->id_loc % DIRBLKSIZ) == 0)
		return (dp);
	ndp = (struct dirent *)(bp->b_un.b_buf + idesc->id_loc);
	if (idesc->id_loc < blksiz && idesc->id_filesize > 0 &&
	    dircheck(idesc, ndp) == 0) {
		size = DIRBLKSIZ - (idesc->id_loc % DIRBLKSIZ);
		idesc->id_loc += size;
		idesc->id_filesize -= size;
		fix = dofix(idesc, MSGSTR(DIRBAD, "DIRECTORY CORRUPTED"));
		bp = getdirblk(idesc->id_blkno, blksiz);
		dp = (struct dirent *)(bp->b_un.b_buf + idesc->id_loc);
		dp->d_reclen += size;
		if (fix)
			dirty(bp);
	}
	return (dp);
}

/*
 * Verify that a directory entry is valid.
 * This is a superset of the checks made in the kernel.
 */
dircheck(idesc, dp)
	struct inodesc *idesc;
	register struct dirent *dp;
{
	register int size;
	register char *cp;
	int spaceleft;

	size = DIRSIZ(dp);
	spaceleft = DIRBLKSIZ - (idesc->id_loc % DIRBLKSIZ);
	if (dp->d_fileno < maxino &&
	    dp->d_reclen != 0 &&
	    dp->d_reclen <= spaceleft &&
	    (dp->d_reclen & 0x3) == 0 &&
	    dp->d_reclen >= size &&
	    idesc->id_filesize >= size &&
	    dp->d_namlen <= NAME_MAX) {
		if (dp->d_fileno == 0)
			return (1);
		for (cp = dp->d_name, size = 0; size < dp->d_namlen; size++)
			if (*cp == 0 || (*cp++ == '/'))
				return (0);
		if (*cp == 0)
			return (1);
	}
	return (0);
}

direrror(ino, errmesg)
	ino_t ino;
	char *errmesg;
{

	fileerror(ino, ino, errmesg);
}

fileerror(cwd, ino, errmesg)
	ino_t cwd, ino;
	char *errmesg;
{
	register struct dinode *dp;
	char pathbuf[MAXPATHLEN + 1];

	pwarn("%s ", errmesg);
	pinode(ino);
	printf("\n");
	getpathname(pathbuf, cwd, ino);
	if (ino < ROOTINO || ino > maxino) {
		pfatal(MSGSTR(NAME, "NAME=%s\n"), pathbuf);
		return;
	}
	dp = ginode(ino);
	if (ftypeok(dp))
		pfatal("%s=%s\n",
		    (dp->di_mode & IFMT) == IFDIR ? MSGSTR(DIR, "DIR") : 
		       MSGSTR(FILE, "FILE"), pathbuf);
	else
		pfatal(MSGSTR(NAME, "NAME=%s\n"), pathbuf);
}

adjust(idesc, lcnt)
	register struct inodesc *idesc;
	short lcnt;
{
	register struct dinode *dp;

	dp = ginode(idesc->id_number);
	if (dp->di_nlink == lcnt) {
		if (linkup(idesc->id_number, (ino_t)0) == 0)
			clri(idesc, MSGSTR(UNREF, "UNREF"), 0);
	} else {
		pwarn(MSGSTR(LCOUNT, "LINK COUNT %s"), (lfdir == idesc->id_number) ? lfname :
			((dp->di_mode & IFMT) == IFDIR ? MSGSTR(DIR, "DIR") : MSGSTR(FILE, "FILE")));
		pinode(idesc->id_number);
		printf(MSGSTR(COUNT, " COUNT %d SHOULD BE %d"),
			dp->di_nlink, dp->di_nlink - lcnt);
		if (preen) {
			if (lcnt < 0) {
				printf("\n");
				pfatal(MSGSTR(LINKCOUNT, "LINK COUNT INCREASING"));
			}
			printf(MSGSTR(ADJUSTED, " (ADJUSTED)\n"));
		}
		if (preen || reply(MSGSTR(ADJUST, "ADJUST")) == 1) {
			dp->di_nlink -= lcnt;
			inodirty();
		}
	}
}

mkentry(idesc)
	struct inodesc *idesc;
{
	register struct dirent *dirp = idesc->id_dirp;
	struct dirent newent;
	int newlen, oldlen;

	newent.d_namlen = strlen(idesc->id_name);
	newlen = DIRSIZ(&newent);
	if (dirp->d_fileno != 0)
		oldlen = DIRSIZ(dirp);
	else
		oldlen = 0;
	if (dirp->d_reclen - oldlen < newlen)
		return (KEEPON);
	newent.d_reclen = dirp->d_reclen - oldlen;
	dirp->d_reclen = oldlen;
	dirp = (struct dirent *)(((char *)dirp) + oldlen);
	dirp->d_fileno = idesc->id_parent;	/* ino to be entered is in id_parent */
	dirp->d_reclen = newent.d_reclen;
	dirp->d_namlen = newent.d_namlen;
	bcopy(idesc->id_name, dirp->d_name, (size_t)dirp->d_namlen + 1);
	return (ALTERED|STOP);
}

chgino(idesc)
	struct inodesc *idesc;
{
	register struct dirent *dirp = idesc->id_dirp;

	if (bcmp(dirp->d_name, idesc->id_name, (int)dirp->d_namlen + 1))
		return (KEEPON);
	dirp->d_fileno = idesc->id_parent;
	return (ALTERED|STOP);
}

#if SEC_MAC
/*
 * Tell if it is OK to reconnect an inode with MAC tag 'orphan_tag' to a
 * lost+found directory with MAC tag 'lf_tag'
 */

sec_reconnect_ok(orphan, orphan_tag, lf_tag)
ino_t orphan;
tag_t orphan_tag, lf_tag;
{
	int dec;

	if (lf_tag == SEC_WILDCARD_TAG_VALUE) {
		pwarn(MSGSTR(WILDLAF,
		"SORRY.  lost+found HAS A WILDCARD SENSITIVITY\nLABEL"));
		return(0);	/* wildcard directory == trouble */
	}
	if (lf_tag == SEC_MAC_SYSLO_TAG)
		return(1);	/* anything dominates syslo */
	if (lf_tag == orphan_tag)
		return(1);	/* anything dominates itself */
	dec = mand_tag_relationship(orphan_tag, lf_tag);
	if (dec && (dec & (MAND_SDOM | MAND_EQUAL)))
		return(1);
	pwarn(MSGSTR(DOMLAF,
	     "SORRY.  INODE DOES NOT DOMINATE lost+found DIRECTORY\nINODE"));
	pinode(orphan);
	printf("\n\n");
	return(0);
}
#endif

linkup(orphan, parentdir)
	ino_t orphan;
	ino_t parentdir;
{
	register struct dinode *dp;
	int lostdir;
	ino_t oldlfdir;
	struct inodesc idesc;
	char tempname[FSCK_BUFSIZ];
	extern int pass4check();
#if SEC_MAC
	struct sec_dinode *sdp;
	static tag_t orphan_tag;	/* MAC tag of orphaned inode */
#endif
	bzero((char *)&idesc, sizeof(struct inodesc));
	dp = ginode(orphan);
	
#if SEC_MAC
	/*
	 * If the orphan has a MAC tag, save it for later dominance checks.
	 */
	if (dp && disk_secure_file_system()) {
		mand_init();
		sdp = (struct sec_dinode *) dp;
		orphan_tag =
		  sdp->di_sec.di_tag[mand_config.first_obj_tag];
	}
#endif

	lostdir = (dp->di_mode & IFMT) == IFDIR;
	pwarn(MSGSTR(UNREF1, "UNREF %s "), lostdir ? MSGSTR(DIR, "DIR") :
	      MSGSTR(FILE, "FILE"));
	pinode(orphan);
	if (preen && dp->di_size == 0)
		return (0);
	if (preen)
		printf(MSGSTR(RECONNECTED, " (RECONNECTED)\n"));
	else
		if (reply(MSGSTR(RECONNECT, "RECONNECT")) == 0)
			return (0);
	if (lfdir == 0) {
		dp = ginode(ROOTINO);
#if SEC_MAC
		/*
		 * Can't make lost+found directory in multilevel dir
		 */
		if (((struct sec_dinode *) dp)->di_sec.di_type_flags &
						SEC_I_MLD) {
			pfatal(MSGSTR(MLDC, 
"SORRY. CANNOT CREATE lost+found DIRECTORY IN A MULTILEVEL DIRECTORY"));
			printf("\n\n");
			return 0;
		}
#endif
		idesc.id_name = lfname;
		idesc.id_type = DATA;
		idesc.id_func = findino;
		idesc.id_number = ROOTINO;
		if ((ckinode(dp, &idesc) & FOUND) != 0) {
			lfdir = idesc.id_parent;
		} else {
			pwarn(MSGSTR(LFOUND5, "NO lost+found DIRECTORY"));
			if (preen || reply(MSGSTR(CREATE, "CREATE"))) {
				lfdir = allocdir(ROOTINO, (ino_t)0, lfmode);
				if (lfdir != 0) {
					if (makeentry(ROOTINO, lfdir, lfname) != 0) {
						if (preen)
							printf(MSGSTR(CREATED, " (CREATED)\n"));
					} else {
						freedir(lfdir, ROOTINO);
						lfdir = 0;
						if (preen)
							printf("\n");
					}
				}
			}
		}
		if (lfdir == 0) {
			pfatal(MSGSTR(LFOUND, "SORRY. CANNOT CREATE lost+found DIRECTORY"));
			printf("\n\n");
			return (0);
		}
	}
	dp = ginode(lfdir);
	if ((dp->di_mode & IFMT) != IFDIR) {
		pfatal(MSGSTR(LFOUND1, "lost+found IS NOT A DIRECTORY"));
		if (reply(MSGSTR(REALLOCATE, "REALLOCATE")) == 0)
			return (0);
		oldlfdir = lfdir;
		if ((lfdir = allocdir(ROOTINO, (ino_t)0, lfmode)) == 0) {
			pfatal(MSGSTR(LFOUND2, "SORRY. CANNOT CREATE lost+found DIRECTORY\n\n"));
			return (0);
		}
		if ((changeino(ROOTINO, lfname, lfdir) & ALTERED) == 0) {
			pfatal(MSGSTR(LFOUND2, "SORRY. CANNOT CREATE lost+found DIRECTORY\n\n"));
			return (0);
		}
		inodirty();
		idesc.id_type = ADDR;
		idesc.id_func = pass4check;
		idesc.id_number = oldlfdir;
		adjust(&idesc, lncntp[oldlfdir] + 1);
		lncntp[oldlfdir] = 0;
		dp = ginode(lfdir);
	}
	if (statemap[lfdir] != DFOUND) {
		pfatal(MSGSTR(LFOUND3, "SORRY. NO lost+found DIRECTORY\n\n"));
		return (0);
	}
#if SEC_MAC
	/*
	 * If this is a secure file system, don't allow the inode to be
	 * reconnected unless it dominates lost+found.
	 */

	if (dp  && disk_secure_file_system()
	    && (orphan_tag != SEC_WILDCARD_TAG_VALUE)
	    && (orphan_tag != SEC_MAC_SYSHI_TAG)) {
		sdp = (struct sec_dinode *) dp;
		if (!sec_reconnect_ok(orphan, orphan_tag,
			sdp->di_sec.di_tag[mand_config.first_obj_tag]))
			return(0);
	}
#endif
	(void)lftempname(tempname, orphan);
	if (makeentry(lfdir, orphan, tempname) == 0) {
		pfatal(MSGSTR(LFOUND4, "SORRY. NO SPACE IN lost+found DIRECTORY"));
		printf("\n\n");
		return (0);
	}
	lncntp[orphan]--;
	if (lostdir) {
		if ((changeino(orphan, "..", lfdir) & ALTERED) == 0 &&
		    parentdir != (ino_t)-1)
			(void)makeentry(orphan, lfdir, "..");
		dp = ginode(lfdir);
		dp->di_nlink++;
		inodirty();
		lncntp[lfdir]++;
		pwarn(MSGSTR(CONNECTED, "DIR I=%lu CONNECTED. "), orphan);
		if (parentdir != (ino_t)-1)
			printf(MSGSTR(PARENTI, "PARENT WAS I=%lu\n"), parentdir);
		if (preen == 0)
			printf("\n");
	}
	return (1);
}

/*
 * fix an entry in a directory.
 */
changeino(dir, name, newnum)
	ino_t dir;
	char *name;
	ino_t newnum;
{
	struct inodesc idesc;

	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = DATA;
	idesc.id_func = chgino;
	idesc.id_number = dir;
	idesc.id_fix = DONTKNOW;
	idesc.id_name = name;
	idesc.id_parent = newnum;	/* new value for name */
#if SEC_MAC
	/*
	 * Clear the multilevel child bit of the directory when moving it
	 */

	if (FsSEC(&sblock))
	{
		struct sec_dinode *secdchild;

		secdchild = (struct sec_dinode *) ginode(dir);
                if (secdchild->di_sec.di_type_flags & SEC_I_MLDCHILD) {
                        secdchild->di_sec.di_type_flags &= ~SEC_I_MLDCHILD;
                        inodirty();
                }
		return (ckinode((struct dinode *) secdchild, &idesc));
	}
#endif
	return (ckinode(ginode(dir), &idesc));
}

/*
 * make an entry in a directory
 */
makeentry(parent, ino, name)
	ino_t parent, ino;
	char *name;
{
	struct dinode *dp;
	struct inodesc idesc;
	char pathbuf[MAXPATHLEN + 1];
	
	if (parent < ROOTINO || parent >= maxino ||
	    ino < ROOTINO || ino >= maxino)
		return (0);
	bzero((char *)&idesc, sizeof(struct inodesc));
	idesc.id_type = DATA;
	idesc.id_func = mkentry;
	idesc.id_number = parent;
	idesc.id_parent = ino;	/* this is the inode to enter */
	idesc.id_fix = DONTKNOW;
	idesc.id_name = name;
	dp = ginode(parent);
	if (dp->di_size % DIRBLKSIZ) {
		dp->di_size = uroundup(dp->di_size, DIRBLKSIZ);
		inodirty();
	}
	if ((ckinode(dp, &idesc) & ALTERED) != 0)
		return (1);
	getpathname(pathbuf, parent, parent);
	dp = ginode(parent);
	if (expanddir(dp, pathbuf) == 0)
		return (0);
	return (ckinode(dp, &idesc) & ALTERED);
}

/*
 * Attempt to expand the size of a directory
 */
expanddir(dp, name)
	register struct dinode *dp;
	char *name;
{
	daddr_t lastbn, newblk;
	register struct bufarea *bp;
	char *cp, firstblk[DIRBLKSIZ];

	lastbn = lblkno(&sblock, dp->di_size);
	if (lastbn >= NDADDR - 1 || dp->di_db[lastbn] == 0 || dp->di_size == 0)
		return (0);
	if ((newblk = allocblk(sblock.fs_frag)) == 0)
		return (0);
	dp->di_db[lastbn + 1] = dp->di_db[lastbn];
	dp->di_db[lastbn] = newblk;
	dp->di_size += sblock.fs_bsize;
	dp->di_blocks += fsbtodb(&sblock, sblock.fs_frag); 
	bp = getdirblk(dp->di_db[lastbn + 1],
		(long)dblksize(&sblock, dp, lastbn + 1));
	if (bp->b_errs)
		goto bad;
	bcopy(bp->b_un.b_buf, firstblk, DIRBLKSIZ);
	bp = getdirblk(newblk, sblock.fs_bsize);
	if (bp->b_errs)
		goto bad;
	bcopy(firstblk, bp->b_un.b_buf, DIRBLKSIZ);
	for (cp = &bp->b_un.b_buf[DIRBLKSIZ];
	     cp < &bp->b_un.b_buf[sblock.fs_bsize];
	     cp += DIRBLKSIZ)
		bcopy((char *)&emptydir, cp, sizeof emptydir);
	dirty(bp);
	bp = getdirblk(dp->di_db[lastbn + 1],
		(long)dblksize(&sblock, dp, lastbn + 1));
	if (bp->b_errs)
		goto bad;
	bcopy((char *)&emptydir, bp->b_un.b_buf, sizeof emptydir);
	pwarn(MSGSTR(NOSPACE, "NO SPACE LEFT IN %s"), name);
	if (preen)
		printf(MSGSTR(EXPANDED, " (EXPANDED)\n"));
	else if (reply(MSGSTR(EXPAND, "EXPAND")) == 0)
		goto bad;
	dirty(bp);
	inodirty();
	return (1);
bad:
	dp->di_db[lastbn] = dp->di_db[lastbn + 1];
	dp->di_db[lastbn + 1] = 0;
	dp->di_size -= sblock.fs_bsize;
	dp->di_blocks -= fsbtodb(&sblock, sblock.fs_frag);
	freeblk(newblk, sblock.fs_frag);
	return (0);
}

/*
 * allocate a new directory
 */
allocdir(parent, request, mode)
	ino_t parent, request;
	int mode;
{
	ino_t ino;
	char *cp;
	struct dinode *dp;
	register struct bufarea *bp;

#if SEC_ARCH
	tag_t tags[SEC_TAG_COUNT];

	/*
	 * Copy the tags from the parent directory to
	 * the newly allocated inode
	 */

	if (FsSEC(&sblock)) {

		dp = ginode(parent);
		bcopy((char *) (((struct sec_dinode *)dp)->di_sec.di_tag),
			(char *) tags, sizeof(tags));
	}
#endif
	ino = allocino(request, IFDIR|mode);
	dirhead.dot_ino = ino;
	dirhead.dotdot_ino = parent;
	dp = ginode(ino);
#if SEC_ARCH
	if (FsSEC(&sblock))
		bcopy((char *) tags,
		      (char *) (((struct sec_dinode *)dp)->di_sec.di_tag),
			sizeof(tags));
#endif
	bp = getdirblk(dp->di_db[0], sblock.fs_fsize);
	if (bp->b_errs) {
		freeino(ino);
		return (0);
	}
	bcopy((char *)&dirhead, bp->b_un.b_buf, sizeof dirhead);
	for (cp = &bp->b_un.b_buf[DIRBLKSIZ];
	     cp < &bp->b_un.b_buf[sblock.fs_fsize];
	     cp += DIRBLKSIZ)
		bcopy((char *)&emptydir, cp, sizeof emptydir);
	dirty(bp);
	dp->di_nlink = 2;
	inodirty();
	if (ino == ROOTINO) {
		lncntp[ino] = dp->di_nlink;
		return(ino);
	}
	if (statemap[parent] != DSTATE && statemap[parent] != DFOUND) {
		freeino(ino);
		return (0);
	}
	statemap[ino] = statemap[parent];
	if (statemap[ino] == DSTATE) {
		lncntp[ino] = dp->di_nlink;
		lncntp[parent]++;
	}
	dp = ginode(parent);
	dp->di_nlink++;
	inodirty();
	return (ino);
}

/*
 * free a directory inode
 */
freedir(ino, parent)
	ino_t ino, parent;
{
	struct dinode *dp;

	if (ino != parent) {
		dp = ginode(parent);
		dp->di_nlink--;
		inodirty();
	}
	freeino(ino);
}

/*
 * generate a temporary name for the lost+found directory.
 */
lftempname(bufp, ino)
	char *bufp;
	ino_t ino;
{
	register ino_t in;
	register char *cp;
	int namlen;

	cp = bufp + 2;
	for (in = maxino; in > 0; in /= 10)
		cp++;
	*--cp = 0;
	namlen = cp - bufp;
	in = ino;
	while (cp > bufp) {
		*--cp = (in % 10) + '0';
		in /= 10;
	}
	*cp = '#';
	return (namlen);
}

/*
 * Get a directory block.
 * Insure that it is held until another is requested.
 */
struct bufarea *
getdirblk(blkno, size)
	daddr_t blkno;
	long size;
{

	if (pdirbp != 0)
		pdirbp->b_flags &= ~B_INUSE;
	pdirbp = getdatablk(blkno, size);
	return (pdirbp);
}
