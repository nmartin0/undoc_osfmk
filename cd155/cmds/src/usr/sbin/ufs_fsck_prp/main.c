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

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1980, 1986 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)main.c	5.27 (Berkeley) 8/7/90";
#endif /* not lint */

#include <sys/secdefines.h>
#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <sys/mount.h>
#include <fstab.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "fsck.h"
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#if SEC_FSCHANGE
#include <sys/secpolicy.h>
#endif

extern priv_t *privvec();
#endif

void	catch(), catchquit(), voidquit();
int	returntosingle;

/*
 * All globals are declared here, rather than in fsck.h.
 */
long numdirs, listmax, inplast;

char	*devname;		/* name of device being checked */
long	dev_bsize;		/* computed value of DEV_BSIZE */
long	secsize;		/* actual disk sector size */
char	nflag;			/* assume a no response */
char	yflag;			/* assume a yes response */
int	bflag;			/* location of alternate super block */
int	clnoverride;		/* override FS clean flag */
int	debug;			/* output debugging info */
int	cvtflag;		/* convert to old file system format */
char	preen;			/* just fix normal inconsistencies */
char	havesb;			/* superblock has been read */
int	fsmodified;		/* 1 => write done to file system */
int	fsreadfd;		/* file descriptor for reading file system */
int	fswritefd;		/* file descriptor for writing file system */

daddr_t	maxfsblock;		/* number of blocks in the file system */
char	*blockmap;		/* ptr to primary blk allocation map */
ino_t	maxino;			/* number of inodes in file system */
ino_t	lastino;		/* last inode in use */
char	*statemap;		/* ptr to inode state table */
short	*lncntp;		/* ptr to link count table */

ino_t	lfdir;			/* lost & found directory inode number */
char	*lfname;		/* lost & found directory name */
int	lfmode;			/* lost & found directory creation mode */

daddr_t	n_blks;			/* number of blocks in use */
daddr_t	n_files;		/* number of files in use */

main(argc, argv)
	int	argc;
	char	*argv[];
{
	int ch;
	int ret, maxrun = 0;
	extern int docheck(), checkfilesys();
	extern char *optarg;
	extern int optind;

#if SEC_BASE && !defined(SEC_STANDALONE)
        set_auth_parameters(argc, argv);
        initprivs();

        if (!authorized_user("sysadmin")) {
                printf("%s: need sysadmin authorization\n", command_name);
                exit(8);
        }
        if (forceprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
                                SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
                                SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
                                SEC_ALLOWNCAVACCESS,
#endif
                                -1), (priv_t *) 0)) {
                printf("%s: insufficient privileges\n", command_name);
                exit(8);
        }
#endif /* SEC_BASE && !defined(SEC_STANDALONE) */

	sync();
	while ((ch = getopt(argc, argv, "cdopnNyYb:l:m:")) != EOF) {
		switch (ch) {
		case 'o':
			clnoverride++;
			break;
		case 'p':
			preen++;
			break;

		case 'b':
			bflag = argtoi('b', "number", optarg, 10);
			printf("Alternate super block location: %d\n", bflag);
			break;

		case 'c':
			cvtflag++;
			break;

		case 'd':
			debug++;
			break;

		case 'l':
			maxrun = argtoi('l', "number", optarg, 10);
			break;

		case 'm':
			lfmode = argtoi('m', "mode", optarg, 8);
			if (lfmode &~ 07777)
				errexit("bad mode to -m: %o\n", lfmode);
			printf("** lost+found creation mode %o\n", lfmode);
			break;

		case 'n':
		case 'N':
			nflag++;
			yflag = 0;
			break;

		case 'y':
		case 'Y':
			yflag++;
			nflag = 0;
			break;

		default:
			errexit("%c option?\n", ch);
		}
	}
	argc -= optind;
	argv += optind;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void)signal(SIGINT, catch);
	if (preen)
		(void)signal(SIGQUIT, catchquit);
	if (argc) {
		while (argc-- > 0)
			(void)checkfilesys(*argv++, (char *)0, 0L, 0);
		exit(0);
	}
	ret = checkfstab(preen, maxrun, docheck, checkfilesys);
	if (returntosingle)
		exit(2);
	exit(ret);
}

argtoi(flag, req, str, base)
	int flag;
	char *req, *str;
	int base;
{
	char *cp;
	int ret;

	ret = (int)strtol(str, &cp, base);
	if (cp == str || *cp)
		errexit("-%c flag requires a %s\n", flag, req);
	return (ret);
}

/*
 * Determine whether a filesystem should be checked.
 */
docheck(fsp)
	register struct fstab *fsp;
{

	if (strcmp(fsp->fs_vfstype, "ufs") ||
	    (strcmp(fsp->fs_type, FSTAB_RW) &&
	     strcmp(fsp->fs_type, FSTAB_RO)) ||
	    fsp->fs_passno == 0)
		return (0);
	return (1);
}

/*
 * Check the specified filesystem.
 */
/* ARGSUSED */
checkfilesys(filesys, mntpt, auxdata, child)
	char *filesys, *mntpt;
	long auxdata;
{
	daddr_t n_ffree, n_bfree;
	struct dups *dp;
	struct zlncnt *zlnp;

	if (preen && child)
		(void)signal(SIGQUIT, voidquit);
	devname = filesys;
	if (debug && preen)
		pwarn("starting\n");
	/*
	 * Check in preen mode for skipping disk check.
	 */
	switch (setup(filesys)) {
		case 0:
			if (preen)
				pfatal("CAN'T CHECK FILE SYSTEM.");
			/* fall through */
		case FS_CLEAN:
			return(0);         /* don't check partition */
		case 1:
			break;
		default:
			errexit("internal error: bad return from setup\n");
	}
	/*
	 * 1: scan inodes tallying blocks used
	 */
	if (preen == 0) {
		printf("** Last Mounted on %s\n", sblock.fs_fsmnt);
		if (hotroot)
			printf("** Root file system\n");
		printf("** Phase 1 - Check Blocks and Sizes\n");
	}
	pass1();

	/*
	 * 1b: locate first references to duplicates, if any
	 */
	if (duplist) {
		if (preen)
			pfatal("INTERNAL ERROR: dups with -p");
		printf("** Phase 1b - Rescan For More DUPS\n");
		pass1b();
	}

	/*
	 * 2: traverse directories from root to mark all connected directories
	 */
	if (preen == 0)
		printf("** Phase 2 - Check Pathnames\n");
	pass2();

	/*
	 * 3: scan inodes looking for disconnected directories
	 */
	if (preen == 0)
		printf("** Phase 3 - Check Connectivity\n");
	pass3();

	/*
	 * 4: scan inodes looking for disconnected files; check reference counts
	 */
	if (preen == 0)
		printf("** Phase 4 - Check Reference Counts\n");
	pass4();

	/*
	 * 5: check and repair resource counts in cylinder groups
	 */
	if (preen == 0)
		printf("** Phase 5 - Check Cyl groups\n");
	pass5();

	/*
	 * print out summary statistics
	 */
	n_ffree = sblock.fs_cstotal.cs_nffree;
	n_bfree = sblock.fs_cstotal.cs_nbfree;
	pwarn("%ld files, %ld used, %ld free ",
	    n_files, n_blks, n_ffree + sblock.fs_frag * n_bfree);
	printf("(%ld frags, %ld blocks, %.1f%% fragmentation)\n",
	    n_ffree, n_bfree, (float)(n_ffree * 100) / sblock.fs_dsize);
	if (debug &&
	    (n_files -= maxino - ROOTINO - sblock.fs_cstotal.cs_nifree))
		printf("%ld files missing\n", n_files);
	if (debug) {
		n_blks += sblock.fs_ncg *
			(cgdmin(&sblock, 0) - cgsblock(&sblock, 0));
		n_blks += cgsblock(&sblock, 0) - cgbase(&sblock, 0);
		n_blks += howmany(sblock.fs_cssize, sblock.fs_fsize);
		if (n_blks -= maxfsblock - (n_ffree + sblock.fs_frag * n_bfree))
			printf("%ld blocks missing\n", n_blks);
		if (duplist != NULL) {
			printf("The following duplicate blocks remain:");
			for (dp = duplist; dp; dp = dp->next)
				printf(" %ld,", dp->dup);
			printf("\n");
		}
		if (zlnhead != NULL) {
			printf("The following zero link count inodes remain:");
			for (zlnp = zlnhead; zlnp; zlnp = zlnp->next)
				printf(" %ld,", zlnp->zlncnt);
			printf("\n");
		}
	}
	zlnhead = (struct zlncnt *)0;
	duplist = (struct dups *)0;
	inocleanup();
	if (!bflag && !nflag && !hotroot) {
		int oldfsmod = fsmodified;

		/* file system is clean -- mark it */
		sblock.fs_clean = FS_CLEAN;
		(void)time(&sblock.fs_time);
		sbdirty();
		/* write out super block */
		flush(fswritefd, &sblk);
		fsmodified = oldfsmod;
	}
	ckfini();
	free(blockmap);
	free(statemap);
	free((char *)lncntp);
	if (!fsmodified)
		return (0);
	if (!preen) {
		printf("\n***** FILE SYSTEM WAS MODIFIED *****\n");
		if (hotroot)
			printf("\n***** REBOOT UNIX *****\n");
	}
	if (hotroot) {
		struct statfs stfs_buf;
		/*
		 * We modified the root.  Do a mount update on
		 * it, unless it is read-write, so we can continue.
		 * Used to be:
		 * sync();
		 * return (4);
		 */
		if (statfs("/", &stfs_buf) == 0) {
			short flags = stfs_buf.f_flags;
			struct ufs_args args;
			int ret;

			if ((flags & M_RDONLY) &&
			    (flags & M_EXPORTED) == 0) {
				args.fspec = 0;
				args.exflags = 0;
				args.exroot = 0;
				ret = mount(MOUNT_UFS, "/", flags|M_UPDATE,
					    &args);
				if (ret == 0)
					return(0);
			}
		}
		printf("\n**** COULD NOT UPDATE HOT ROOT ****\n");
		printf("\n**** REBOOT IMMEDIATELY ****\n");
		/* sync(); */
		return (4);
	}
	return (0);
}
