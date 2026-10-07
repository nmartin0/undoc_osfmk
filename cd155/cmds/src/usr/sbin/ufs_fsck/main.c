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
 * HISTORY
 * $Log: main.c,v $
 * Revision 1.17  1994/11/19  03:18:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.16  1994/07/15  20:10:02  shala
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
 * Revision 1.15  1994/06/29  00:32:10  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.14  1993/06/09  18:59:25  stans
 *    Added Misc. comments plus docheck() now understands its OK to check
 *    'pfs' fileystems.
 *
 * Revision 1.13  1993/06/04  21:46:22  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.12  1993/05/17  22:14:16  stans
 * Support parallel fsck on Paragon only.
 *
 * Revision 1.11  1993/05/10  17:42:50  stans
 * Added stronger syntax checking for '-Ps..e' switch. If the ending pass is
 * specified, then the '..' syntax must be correct.
 *
 * Revision 1.10  1993/05/10  16:53:09  stans
 * '-E endingPass' switch has been removed in favor of an expanded '-Ps..e'
 * format. The '..e' portion is optional. Motivation is less documentation
 * impact and a cleaner interface.
 *
 * Revision 1.9  1993/04/20  17:38:12  stans
 *   Support '-En' "/etc/fstab" pass switch. "-En" imples fstab processing
 *   will end after fstab pass 'n' has been completed. Switch is utilized
 *   to control fsck & mounting of local and/or remote filesystems during the
 *   multi-user boot sequence.
 *
 * Revision 1.8  1993/03/25  22:25:11  stans
 * cleanup rcs comments
 *
 * Revision 1.7  1993/03/25  22:21:57  stans
 * 	Support "-P fstab-pass-number" switch. "-P pass" implies that the
 * 	filesystem checks should start with pass number 'pas' as defined in
 * 	'/etc/fstab' passno field. If '/etc/fstab/' is defined according to
 * 	fstab rules: only the root filesystem at pass number #1 while
 * 	everything else is > 1. Now by specifing "-P 2" one can run fsck on
 * 	all filesystem EXCEPT the root. This functionality is required during
 * 	booting after the mesh has been started. Real win here is that fsck
 * 	will do parallel FS checks ONLY if processing fstab, no cmd line FS
 * 	specifications.  Handle new switch "-P pass" here, pass on to
 * 	checkfstab() routine.
 *
 * Revision 1.6  1993/01/18  19:45:47  stans
 * Seems the REBOOT UNIX message is just extra incorrect noise. Turns out the
 * best approach is to delay output of the error message and let the hot-root
 * code * output an error if it can NOT update the hot-root. Update failure
 * message mandates a REBOOT.
 *
 * Revision 1.5  1993/01/18  19:29:32  stans
 * Modified routine "checkfilesys()" to output the "REBOOT UNIX" message only
 * when you did NOT have a HOT ROOT and propagate the error return to the
 * caller; we use HOT ROOT as the default. When the HOT ROOT has been
 * sucessfully updated output a message indicating so and let the ufs_fsck
 * continue. At the call site of "checkfilesys()" for the case of cmd-line
 * specified fileSystems, sum the return codes from ALL specified fileSystems
 * and return to the user. Previously "0" was always returned.
 *
 * Revision 1.4  1992/10/28  22:12:05  stans
 * Added missing #endif for mach3 mach_port_t def.
 *
 * Revision 1.3  1992/10/27  16:17:02  shala
 * Define mach_port_t
 *
 * Revision 1.2  1992/10/12  22:07:59  shala
 * New version to support maj, min and node numbers.
 *
 * Print version string for BLK_RESERVE_SUPPORT config.
 * Don't use message catalog for OSF1_ADFS.
 *
 * Revision 2.14.2.10  1992/03/22  20:29:28  kittlitz
 * 	bug 5799. ismounted must diagnose failure to open block device
 * 	[1992/03/22  20:24:27  kittlitz]
 *
 * Revision 2.14.2.9  1992/03/03  19:06:23  garyf
 * 	check values of argument to -b
 * 	[1992/03/03  18:49:16  garyf]
 * 
 * Revision 2.14.2.8  1992/02/25  13:51:35  garyf
 * 	grow buffer for filesystem name in ismounted()
 * 	[1992/02/25  13:51:00  garyf]
 * 
 * Revision 2.14.2.7  1992/02/11  23:09:32  garyf
 * 	bug 4529: -l doesn't check for invalid values
 * 	[1992/02/11  23:03:42  garyf]
 * 
 * Revision 2.14.2.6  1992/01/30  15:56:08  garyf
 * 	allow read-only mounted filesystems to be
 * 	checked; add -O to force checking; if -p
 * 	print warning, skip, and continue
 * 	[1992/01/30  15:54:38  garyf]
 * 
 * Revision 2.14.2.5  1991/12/31  14:39:03  garyf
 * 	change previous to allow root.
 * 	[1991/12/31  14:36:43  garyf]
 * 
 * Revision 2.14.2.4  1991/12/12  15:49:14  garyf
 * 	prevent fsck of mounted filesystem
 * 	[1991/12/12  15:48:36  garyf]
 * 
 * Revision 2.14.2.3  1991/10/21  12:48:34  lehotsky
 * 	Clean nightly build
 * 	[91/10/21  12:45:02  lehotsky]
 * 
 * Revision 2.14.2.2  91/10/09  13:55:01  sue
 * 	Fixed bug 3260.  Don't mark a fs clean if it had fatal errors
 * 	and the user replied no to a question.  I.e. we make marking a fs
 * 	clean much more conservative.
 * 	[91/10/09  13:53:31  sue]
 * 
 * Revision 2.14  91/06/10  16:37:23  devrcs
 * 	add messaging
 * 	[91/05/13  11:34:16  garyf]
 * 
 * Revision 2.13  91/03/24  15:25:58  devrcs
 * 	Use fprintf to stderr when checking authorization.
 * 	[91/03/12  13:23:42  seiden]
 * 
 * Revision 2.12  91/01/07  17:23:22  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:32:41  dwm]
 * 
 * Revision 2.11  90/10/31  15:40:54  devrcs
 * 	Added automatic mount update of root if it was
 * 	changed and mounted read-only, and it was not
 * 	yet exported.  In other cases, a reboot
 * 	message is printed, and an error is returned.
 * 	[90/10/10  18:25:25  gmf]
 * 
 * Revision 2.10  90/10/07  22:50:56  devrcs
 * 	If not preen mode, don't use simple-minded check for "/"
 * 	as last mounted directory to determine if we're doing
 * 	root.  Let's just let it mark the root as clean.
 * 	If forced to fsck manually, the -o flag may be required.
 * 	We could do elaborate checks, but it's probably not
 * 	worth it.
 * 	[90/10/02  15:08:55  gmf]
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	Put globals here, rather than in fsck.h.
 * 	[90/10/02  09:37:27  gmf]
 * 
 * Revision 2.9  90/09/23  17:01:12  devrcs
 * 	Fixed to not mark root as clean.
 * 	[90/09/11  14:33:18  nags]
 * 
 * Revision 2.8  90/08/25  12:36:46  devrcs
 * 	Filesystem clean flag support.
 * 	[90/08/19  01:34:31  nags]
 * 
 * Revision 2.7  90/07/27  11:45:15  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:30:50  pam]
 * 
 * 	Initial security hooks from SecureWare.
 * 	[90/07/12  12:57:17  seiden]
 * 
 * Revision 2.6  90/06/22  22:12:47  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:31:18  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: main.c,v $ $Revision: 1.17 $ (OSF) $Date: 1994/11/19 03:18:14 $";
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

/***
char copyright[] =
"@(#) Copyright (c) 1980, 1986 The Regents of the University of California.\n\
 All rights reserved.\n";
***/

/*** "main.c	5.27 (Berkeley) 8/7/90"; ***/

#ifndef mach_port_t
typedef unsigned int mach_port_t;
#endif

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
#include <fcntl.h>
#include <errno.h>
#include "fsck.h"
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#if SEC_FSCHANGE
#include <sys/secpolicy.h>
#endif

extern priv_t *privvec();
#endif

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

void	catch(), catchquit(), voidquit();
int	returntosingle;
static int ismounted();

/*
 * All globals are declared here, rather than in fsck.h.
 */
long numdirs, listmax, inplast;

char	*devname;		/* name of device being checked */
long	secsize;		/* actual disk sector size */
char	nflag;			/* assume a no response */
char	yflag;			/* assume a yes response */
int	bflag;			/* location of alternate super block */
int	clnoverride;		/* override FS clean flag */
int	writeableoverride;	/* override RW filesystem mounted */
int	debug;			/* output debugging info */
int	cvtflag;		/* convert to old file system format */
int	Cflag;			/* convert to new fsbtodb value */
char	preen;			/* just fix normal inconsistencies */
char	havesb;			/* superblock has been read */
int	fsmodified;		/* 1 => write done to file system */
int	fsreadfd;		/* file descriptor for reading file system */
int	fswritefd;		/* file descriptor for writing file system */
int	startingPass=1;		/* fstab starting pass number */
int	endingPass=0;		/* fstab ending pass number, 0 == no end */
int	RFork=1;		/* Use rfork() instead of fork() */

daddr_t	maxfsblock;		/* number of blocks in the file system */
char	*blockmap;		/* ptr to primary blk allocation map */
int	old_fsbtodb;		/* value corrected for fsbtodb or minus one
				   if original value was ok */
int	new_fsbtodb;		/* new value for fsbtodb or minus one if
				   original value was ok */
int	dev_bshift;		/* shift to switch between bytes and
				   conventional blocks */
ino_t	maxino;			/* number of inodes in file system */
ino_t	lastino;		/* last inode in use */
char	*statemap;		/* ptr to inode state table */
short	*lncntp;		/* ptr to link count table */

ino_t	lfdir;			/* lost & found directory inode number */
char	*lfname;		/* lost & found directory name */
int	lfmode;			/* lost & found directory creation mode */

daddr_t	n_blks;			/* number of blocks in use */
daddr_t	n_files;		/* number of files in use */

int	fatal_cnt;		/* count of fatal errors */
int	no_cnt;			/* count of no replies */

#define VERSION  "Fsck Version OSF/1 MK-AD 1.0"

main(argc, argv)
	int	argc;
	char	*argv[];
{
	int ch;
	int ret, maxrun = 0;
	char *cp;
	extern int docheck(), checkfilesys();
	extern char *optarg;
	extern int optind;

#ifndef OSF1_ADFS
	(void) setlocale( LC_ALL, "" );
	catd = catopen(MF_UFS_FSCK,0);
#endif

#if SEC_BASE && !defined(SEC_STANDALONE)
        set_auth_parameters(argc, argv);
        initprivs();

        if (!authorized_user("sysadmin")) {
                fprintf(stderr, MSGSTR(SYSADMIN, "%s: need sysadmin authorization\n"), 
			command_name);
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
                printf(MSGSTR(PRIV, "%s: insufficient privileges\n"), command_name);
                exit(8);
        }
#endif /* SEC_BASE && !defined(SEC_STANDALONE) */

	sync();
	while ((ch = getopt(argc, argv, "cCdoOpnNyYb:l:m:P:R")) != EOF) {
		switch (ch) {
		case 'o':
			clnoverride++;
			break;
		case 'O':
			writeableoverride++;
			break;
		case 'p':
			preen++;
			break;
		case 'R':
			RFork=0;
			break;

		case 'b':
			bflag = argtoi('b', MSGSTR(NUMBER, "number"), optarg, 10);
 			if (bflag < 0)
 				errexit(MSGSTR(INVALIDB, "Invalid -b (alternate super block) of %d.\n"),
					       bflag);
			printf(MSGSTR(ALTSUP, "Alternate super block location: %d\n"), bflag);
			break;

		case 'c':
			cvtflag++;
			break;

		case 'C':
			Cflag++;
			break;

		case 'd':
			debug++;
			break;

		case 'l':
			maxrun = argtoi('l', MSGSTR(NUMBER, "number"), optarg, 10);
 			if (maxrun < 0)  
 				errexit(MSGSTR(INVALID,
					       "Invalid -l (parallel checks) of %d.\n"),
 						maxrun); 
			break;

		case 'm':
			lfmode = argtoi('m', MSGSTR(MODE, "mode"), optarg, 8);
			if (lfmode &~ 07777)
				errexit(MSGSTR(BADMODE, "bad mode to -m: %o\n"), lfmode);
			printf(MSGSTR(LFOUND7, "** lost+found creation mode %o\n"), lfmode);
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

		case 'P': /* -P startingPass-number{..endingPass} */
			
			/* if we have '..' syntax, isolate the 1st number */
			if ( cp=strchr(optarg,'.') ) {
				/* must have '..' */
				if ( *(cp+1) != '.' ) {
 					errexit( MSGSTR(INVALID,
						"%s: Invalid -P %s.\n"),optarg);
				}
				*cp = '\0';
			}
			startingPass = argtoi('P',MSGSTR(NUMBER,"number"),optarg,10);
			if ( cp ) {
				*cp = '.';
				cp += 2;	/* skip over '..' */
				endingPass = argtoi('P',MSGSTR(NUMBER,"number"),cp,10);
			}
 			if (startingPass <= 0) {  
 				errexit(
					MSGSTR(INVALID,"Invalid -P %d.\n"),
					startingPass); 
			}
			if ( (endingPass != 0) && (endingPass < startingPass) ) {
 				errexit(
					MSGSTR(INVALID,"Invalid ending Pass %d, less than starting Pass %d.\n"),
					endingPass,startingPass); 
			}
			break;
		default:
			errexit(MSGSTR(OPTION, "%c option?\n"), ch);
		}
	}

#ifdef	i860
	/* only rfork() if a Paragon */
	if (_i860_cpu_type() == 1)
		RFork = 0;
#endif

#ifdef BLK_RESERVE_SUPPORT
	printf("%s\n", VERSION);
#endif
	argc -= optind;
	argv += optind;
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		(void)signal(SIGINT, catch);
	if (preen)
		(void)signal(SIGQUIT, catchquit);

	/*
	 * check filesystem listed on the command line.
	 */
	if (argc) {
		int	sumRC=0;
		while (argc-- > 0)
			sumRC |= checkfilesys(*argv++, (char *)0, 0L, 0);
                (void)close(fsreadfd);
                (void)close(fswritefd);
		exit(sumRC);
	}

	/*
	 * process filesystems from fstab
	 */
	ret = checkfstab( startingPass, endingPass, preen, maxrun,
				docheck, checkfilesys);
        (void)close(fsreadfd);
        (void)close(fswritefd);
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
		errexit(MSGSTR(FLAG, "-%c flag requires a %s\n"), flag, req);
	return (ret);
}

/*
 * Determine whether a filesystem should be checked.
 * We support "ufs" or "pfs"
 *
 * outputs:
 *	1 == check the filesystem
 *	0 == do NOT check the filesystem.
 */
docheck(fsp)
	register struct fstab *fsp;
{
	/* ONLY ufs or pfs are valid filesystem types. */
	if ( strcmp(fsp->fs_vfstype, "ufs") && strcmp(fsp->fs_vfstype, "pfs") )
		return (0);

	/* must be Read-Write or Read-Only to be checked. */
	if (strcmp(fsp->fs_type, FSTAB_RW) && strcmp(fsp->fs_type, FSTAB_RO))
		return (0);

	/* skip if pass 0, 0 has been handled */
	if ( fsp->fs_passno == 0 )
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
		pwarn(MSGSTR(STARTING, "starting\n"));
	/*
	 * Check in preen mode for skipping disk check.
	 */
	switch (setup(filesys)) {
		case 0:
			if (preen)
				pfatal(MSGSTR(NOCHECK, "CAN'T CHECK FILE SYSTEM."));
			/* fall through */
		case FS_CLEAN:
			return(0);         /* don't check partition */
		case 1:
			break;
		default:
			errexit(MSGSTR(INTERR, "internal error: bad return from setup\n"));
	}
	/*
	 * Check if filesystem is mounted
	 */
	if (ismounted(filesys)) {
	  	if (preen) {
			return(0); /* don't check partition */
		}
		fprintf(stderr,  MSGSTR( FS_MTD, 
			"cannot fsck a mounted filesystem %s.\n"),
			filesys);
		exit(1);
	}
	/*
	 * 1: scan inodes tallying blocks used
	 */
	if (preen == 0) {
		printf(MSGSTR(LMOUNT, "** Last Mounted on %s\n"), sblock.fs_fsmnt);
		if (hotroot)
			printf(MSGSTR(ROOT, "** Root file system\n"));
		printf(MSGSTR(PHASE1, "** Phase 1 - Check Blocks and Sizes\n"));
	}
	pass1();

	/*
	 * 1b: locate first references to duplicates, if any
	 */
	if (duplist) {
		if (preen)
			pfatal(MSGSTR(INTERR1, "INTERNAL ERROR: dups with -p"));
		printf(MSGSTR(PHASE1B, "** Phase 1b - Rescan For More DUPS\n"));
		pass1b();
	}

	/*
	 * 2: traverse directories from root to mark all connected directories
	 */
	if (preen == 0)
		printf(MSGSTR(PHASE2, "** Phase 2 - Check Pathnames\n"));
	pass2();

	/*
	 * 3: scan inodes looking for disconnected directories
	 */
	if (preen == 0)
		printf(MSGSTR(PHASE3, "** Phase 3 - Check Connectivity\n"));
	pass3();

	/*
	 * 4: scan inodes looking for disconnected files; check reference counts
	 */
	if (preen == 0)
		printf(MSGSTR(PHASE4, "** Phase 4 - Check Reference Counts\n"));
	pass4();

	/*
	 * 5: check and repair resource counts in cylinder groups
	 */
	if (preen == 0)
		printf(MSGSTR(PHASE5, "** Phase 5 - Check Cyl groups\n"));
	pass5();

	/*
	 * print out summary statistics
	 */
	n_ffree = sblock.fs_cstotal.cs_nffree;
	n_bfree = sblock.fs_cstotal.cs_nbfree;
	pwarn(MSGSTR(FILES, "%ld files, %ld used, %ld free "),
	    n_files, n_blks, n_ffree + sblock.fs_frag * n_bfree);
	printf(MSGSTR(FRAGS, "(%ld frags, %ld blocks, %.1f%% fragmentation)\n"),
	    n_ffree, n_bfree, (float)(n_ffree * 100) / sblock.fs_dsize);
	if (debug &&
	    (n_files -= maxino - ROOTINO - sblock.fs_cstotal.cs_nifree))
		printf(MSGSTR(MISSING, "%ld files missing\n"), n_files);
	if (debug) {
		n_blks += sblock.fs_ncg *
			(cgdmin(&sblock, 0) - cgsblock(&sblock, 0));
		n_blks += cgsblock(&sblock, 0) - cgbase(&sblock, 0);
		n_blks += uhowmany(sblock.fs_cssize, sblock.fs_fsize);
		if (n_blks -= maxfsblock - (n_ffree + sblock.fs_frag * n_bfree))
			printf(MSGSTR(MISSING2, "%ld blocks missing\n"), n_blks);
		if (duplist != NULL) {
			printf(MSGSTR(DUPBLOCK, "The following duplicate blocks remain:"));
			for (dp = duplist; dp; dp = dp->next)
				printf(" %ld,", dp->dup);
			printf("\n");
		}
		if (zlnhead != NULL) {
			printf(MSGSTR(ZEROLINK, "The following zero link count inodes remain:"));
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

		/*
		 * Only mark a filesystem clean if all fatal errors had
		 * a yes reply
		 */
		if (fatal_cnt && no_cnt) {
			/*
			 * There were fatal errors and the user answered
			 * no, so be conservative and don't mark it clean.
			 */
			sblock.fs_clean = 0;
		} else 
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
	if (!preen)
		printf(MSGSTR(MODIFIED,"\n***** FILE SYSTEM WAS MODIFIED *****\n"));
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
					    (caddr_t) &args);
				if (ret == 0) {
					if ( !preen )
						printf(MSGSTR(HOTROOT,"\n**** HOT ROOT UPDATED. ****\n\n"));
					return(0);
				}
			}
		}
		printf(MSGSTR(HOTROOT, "\n**** COULD NOT UPDATE HOT ROOT ****\n"));
		printf(MSGSTR(REBOOT1, "\n**** REBOOT IMMEDIATELY ****\n"));
		/* sync(); */
		return (4);
	}
	return (0);
}

/* this routine determines if we should continue with
   running fsck on a filesystem.  Here are the rules:
	if not mounted, ok
	if -O, ok
	if root mounted as "root_device", ok
	if read-only, ok
	if preen mode & r/w, return & let caller print
	   a message
*/
static ismounted(fsname)
	char	*fsname;
{
	char	bname[PATH_MAX + 1], *bnp, *tnp;
	int	fd;
	struct	statfs *mntbuf;
	int	i,mntsize;
	extern char * unrawname();
	extern char * blockcheck();
	char	*msgbuf1, *msgbuf2;
	int	err;

	/* get block interface name from raw name.
	 * 	assuming /dev/r<??> format
	 */
	strcpy(bname, fsname);
	bnp = unrawname(bname);

	if ((fd = open(bnp, O_RDWR)) >= 0) {
		close(fd);
		return(FALSE);
	}
	err = errno;
	if (writeableoverride)
		return(FALSE);
	tnp = blockcheck(fsname);
	if (err == EBUSY) {
		if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) != 0) {
			for (i = 0; i < mntsize; i++) {
				if (hotroot)
					if (!strcmp(mntbuf[i].f_mntfromname,
						    "root_device"))
						return(FALSE);
				if (!strcmp(mntbuf[i].f_mntfromname, bnp)) {
					if (mntbuf[i].f_flags & M_RDONLY)
						return(FALSE);
					tnp = mntbuf[i].f_mntonname;
					break;
				}
			}
		}

		if (preen)
			fprintf(stderr, MSGSTR(SKIPFS,
				"skipping mounted filesystem (%s)\n"),
				fsname);
		else fprintf(stderr, MSGSTR(ISMOUNTED,
			       "filesystem %s is mounted as %s.\n"), bnp, tnp);
	} else {
		msgbuf1 = MSGSTR(ISMOUNTED_ERROR,
				"Cannot determine if %s is mounted");
		msgbuf2 = malloc(strlen(msgbuf1) + strlen(bname) + 1);
		errno = err;
		if (msgbuf2 == NULL) {
			perror(bname);
		} else {
			sprintf(msgbuf2, msgbuf1, bname);
			perror(msgbuf2);
			free(msgbuf2);
		}
	}

	if (preen)
		return(TRUE);
	if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
		errexit("");
	
	return(FALSE);
}

#ifdef	i860
/*
 *  _860_cpu_type returns 1 for XR, 2 for XP.
 */
_i860_cpu_type()
{
    asm(" ld.c epsr,r16");
    asm(" and  0xff,r16,r16");
}
#endif
