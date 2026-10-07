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
 
/* @(#)mount.c	3.2 10:05:08 6/24/90 SecureWare */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: mount.c,v $
 * Revision 1.20  1995/02/25  00:34:40  hobbes
 *  Reviewer: Nina Lepak
 *  Risk: Low
 *  Benefit or PTS #: 12438
 *  Testing: verified the fix by testing the steps that produced it
 *  Module(s): sbin/mount/Makefile usr/sbin/mount/Makefile
 * 	    usr/sbin/mount/mount.c usr/sbin/mountd/mountd.c
 *
 * Revision 1.19  1994/11/19  03:09:32  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/07/21  20:25:59  yazz
 *  Reviewer: Shala Arshi, Bob Yasi
 *  Risk: lo
 *  Benefit or PTS #: 10127 C-0
 *  Testing: Testcase now passes; also passes at customer site
 *  Module(s): /src/usr/sbin/umount/umount.c, /src/usr/sbin/umount/umount.c
 *
 * Always zero out the sockaddr_in structure before using, lest random stack
 * garbage foul route processing.
 *
 * Revision 1.17  1994/07/19  23:47:57  rlg
 * Fix one problem and one enhancement.
 *
 *     1.  PTS #10305 (default PFS stripe unit is 512K - should be 64K)
 *
 *         The default stripe unit size was set to the value of MAXBSIZE.
 *         The value of this mnemonic changed in R1.3, so the stripe unit
 * 	size was set to the value of a local mnemonic defined to be 64K.
 *
 *     2.  PTS #9712  (enhancement: /etc/pfstab should allow comment lines)
 *
 * 	A check was added to the sgroup_opts() subroutine, which fills
 *         in the PFS stripe_attr structure, to ignore lines whose first
 *         significant character is a '#'
 *
 *  Reviewer:  Dave Minturn
 *  Risk:      low
 *  Testing:   executing the mount system command with different options
 *               and comment lines in /etc/pfstab
 *  Module(s): cmds_libs/src/usr/sbin/mount/mount.c
 *
 * Revision 1.16  1994/06/21  16:36:35  suri
 *  Reviewer: shala
 *  Risk: low
 *  Benefit or PTS #: 9832
 *  Testing: ran the testcase on polaris
 *  Module(s): mountfs()
 *  Solution: modified the print message so that it is more informative
 *
 * Revision 1.15  1994/05/23  23:51:54  suri
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 9373
 *  Testing: Functional verification performed on polaris
 *  Module(s): vfs/vfs_syscalls.c (mount1())
 *             cmds_libs/src/usr/sbin/mount/mount.c
 *  Solution: Modified mount command to print a more appropriate message in
 *            the event of EBUSY.
 *
 * Revision 1.14  1993/10/12  22:34:59  shala
 * Added "-v" option to the usage line since it is supported.
 *
 * Revision 1.13  1993/10/12  22:26:08  shala
 * Print error in case "-f", "-v", or "-t" is used alone. These options
 * need to have "-a" or a mount point to properly.
 *
 * Revision 1.12  1993/06/06  00:58:35  brad
 * Fixed EACCESS error handling on PFS file system.
 *
 * Revision 1.11  1993/06/04  20:43:32  brad
 * Removed printf.
 *
 * Revision 1.10  1993/06/04  20:10:49  brad
 * Changed fsbuf parameter to statpfs, so statpfs system call does not try to
 * perform a statfs on all stripe directories ... which can result in an
 * EACCES when all the user is trying to do is display the mount table.
 *
 * Revision 1.9  1993/05/10  17:39:00  stans
 * '-E' switch has been removed in favor of 'Pstart..end' syntax. Ending pass
 * is optional.
 *
 * Revision 1.8  1993/04/20  23:07:44  stans
 *   Added EBUSY case and printf() to indicate a filesystem was already
 *   mounted.
 *
 * Revision 1.7  1993/04/20  17:22:13  stans
 *   Added 'pgmname' for error message output.
 *   Support '-Pn' and '-En' "/etc/fstab" pass switches. "-Pn" implies fstab
 *   processing will begin with pass 'n'. "-En" imples fstab processing will
 *   end after fstab pass 'n' has been completed. Both switches are utilized
 *   to control fsck & mount of local and remote filesystems during the
 *   multi-user boot sequence.
 *
 * Revision 1.6  1993/04/05  23:35:26  brad
 * Modified output so that sdirectories have more horizontal space when just
 * 'mount' is typed.
 *
 * Revision 1.5  1993/04/02  22:22:13  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4.2.5  1993/03/04  22:34:43  dbm
 * Updated to use new statpfs call and estatfs structure.
 *
 * Revision 1.4.2.4  1993/02/23  05:01:29  brad
 * Added PFS handling for EFSNOTSUPP.
 *
 * Revision 1.4.2.3  1993/02/19  20:00:16  dbm
 * Added support for pfs stripe groups, also added defaults and dynamic
 * allocation of the stripe directory buffer.
 *
 * Revision 1.4.2.2  1993/02/16  22:02:30  brad
 * Added a default stripe unit size of MAXBSIZE.
 *
 * Revision 1.4.2.1  1993/01/12  02:14:27  brad
 * Added support for PFS, including the stripeunit and stripedirs options.
 *
 * Revision 1.4  1992/10/30  00:08:29  shala
 * Added missing #endif.
 *
 * Revision 1.3  1992/10/27  16:12:42  shala
 * Define mach_port_t
 *
 * Revision 1.2  1992/10/12  22:14:03  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.15  90/10/31  15:36:58  devrcs
 * 	Change M_FORCE flag to M_FMOUNT.
 * 	It had a potential conflict with MNT_FORCE.
 * 	[90/10/13  21:40:07  gmf]
 * 
 * 	Fixed prmount problem, and cleaned up a bit.
 * 	[90/10/10  21:08:41  gmf]
 * 
 * Revision 2.14  90/10/07  22:21:13  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  15:29:21  gm]
 * 
 * Revision 2.13  90/09/13  13:24:24  devrcs
 * 	Have prmount say if a filesys is unextended.
 * 	[90/08/24  12:28:27  seiden]
 * 
 * Revision 2.12  90/08/25  12:30:44  devrcs
 * 	Filesystem clean flag support (option to forcibly
 * 	mount dirty filesystems).
 * 	[90/08/19  01:31:38  nags]
 * 
 * Revision 2.11  90/07/27  11:29:12  devrcs
 * 	Added authorization check and labeled filesystem support from
 * 	SecureWare.
 * 	[90/07/13  01:22:21  seiden]
 * 
 * Revision 2.10  90/07/17  12:39:59  devrcs
 * 	Change badvfstype call to badvfsname to make mount -a work.
 * 	[90/07/10  09:09:10  gmf]
 * 
 * Revision 2.9  90/07/06  00:46:16  devrcs
 * 	Sync'ed with latest 4.4 code.
 * 	[90/06/27  22:46:00  gmf]
 * 
 * Revision 2.8  90/06/29  14:41:31  devrcs
 * 	Removed references to defunct libsys library.
 * 	[90/06/20  17:56:22  gm]
 * 
 * Revision 2.7  90/06/22  22:22:46  devrcs
 * 	Add NOCONN option for NFS.
 * 	[90/06/06  15:57:12  tmt]
 * 
 * Revision 2.6  90/04/27  22:49:48  devrcs
 * 	Added support for system V filesystem.
 * 	[90/04/20  18:04:54  morris]
 * 
 * Revision 2.5  90/03/27  20:03:57  gm
 * 	Changes from George at Encore
 * 	Add parameter to getmntinfo.
 * 	Some random cleanup
 * 	[90/03/14  20:22:38  pam]
 * 
 * Revision 2.4  90/02/23  00:19:05  devrcs
 * 	Changes for snapshot
 * 	[90/02/20  23:18:44  gm]
 * 
 * Revision 2.3  90/01/02  18:43:05  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  09:04:08  gm
 * 	Changed to allow operator to mount file systems.
 * 	[80/11/06            mja]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1980, 1989 The Regents of the University of California.
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
"@(#) Copyright (c) 1980, 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)mount.c	5.40 (Berkeley) 11/24/89";
#endif /* not lint */

#ifndef mach_port_t
typedef unsigned int mach_port_t;
#endif

#include <sys/secdefines.h>

#include "pathnames.h"
#include <sys/param.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/wait.h>
#ifdef	PFS
#include <sys/types.h>
#include <nx.h>
#endif
#ifdef	TNC
#include <cthreads.h>
#endif
#include <fstab.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <sys/mount.h>
#ifdef NFS
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_prot.h>
#include <nfs/rpcv2.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#endif

#ifdef	PFS
#define PFSARGS_BUFSZ	 MAXBSIZE 
static  int	pfsargs_bufsz;	/* Initial buffer size */
static 	struct pfs_args *pfsargsp;
static 	struct statpfs 	*sattr;
static	pathname_t	*sdir;

/*
 * Define defaults:
 */
#define _DEFAULT_SUNIT_SIZE	65536
#define _PFSTAB_MAX_LINE	400
#define _PFSTAB_PATH 		"/etc/pfstab"
#define	_PFSTAB_DEFAULT_GROUP	"all"
#endif

#ifdef	OSF
/*
 * One day we'll change to use the MNT_* syntax
 */
#define MNT_RDONLY	M_RDONLY
#define MNT_EXRDONLY	M_EXRDONLY
#define MNT_EXPORTED	M_EXPORTED
#define MNT_UPDATE	M_UPDATE
#define MNT_NOEXEC	M_NOEXEC
#define MNT_NOSUID	M_NOSUID
#define MNT_NODEV	M_NODEV
#define MNT_FASTPATH	M_FASTPATH
#define MNT_SYNCHRONOUS	M_SYNCHRONOUS
#define MNT_QUOTA	M_QUOTA
#define MNT_LOCAL	M_LOCAL
#define MNT_VISFLAGMASK	M_VISFLAGMASK
#define MNT_FMOUNT	M_FMOUNT
#if	SEC_ARCH
#define MNT_SECURE	M_SECURE
#endif
#endif	/* OSF */

#define DEFAULT_ROOTUID -2

#if     UNIX_DOMAIN
#include <sys/un.h>
#endif 

#define	BADTYPE(type) \
	(strcmp(type, FSTAB_RO) && strcmp(type, FSTAB_RW) && \
	    strcmp(type, FSTAB_RQ))
#define	SETTYPE(type) \
	(!strcmp(type, FSTAB_RW) || !strcmp(type, FSTAB_RQ))

int	fake, verbose, mntflg, mnttype;
char	*pgmname, *mntname, **envp;
char	**vfslist, **makevfslist();

#ifdef NFS
AUTH *authunix_default();
int xdr_dir(), xdr_fh();
char *getnfsargs();
static void prmount();
struct nfs_args nfsdefargs = {
	(struct sockaddr_in *)0,
	(nfsv2fh_t *)0,
	NFSMNT_INT,
	NFS_WSIZE,
	NFS_RSIZE,
	NFS_TIMEO,
	NFS_RETRANS,
	(char *)0,
};

struct nfhret {
	u_long	stat;
	nfsv2fh_t nfh;
};
int retrycnt;
#define	DEF_RETRY	10000
#define	BGRND	1
#define	ISBGRND	2
int opflags = 0;
#endif	/* NFS */

int	startingPass=1,		/* fstab starting pass number */
	endingPass=0;		/* fstab ending pass number, 0 == do all */

#ifdef	PFS
static void	prpfsoptions();
long		checksize();
#endif

main(argc, argv, arge)
	int argc;
	char **argv;
	char **arge;
{
	extern char *optarg;
	extern int optind;
	register struct fstab *fs;
	register int cnt;
#ifdef TNC
	int all, ch, rval, flags, ret, i;
	pid_t pgrp;
#else
	int all, ch, rval, flags, ret, pid, i;
#endif
	long mntsize;
	struct statfs *mntbuf, *getmntpt();
	char *type, *cp, *options = NULL;
	FILE *pidfile;

	if ( (type=strrchr(argv[0],'/')) )
		pgmname = ++type;
	else
		pgmname = argv[0];

#if SEC_BASE 
	mount_init(argc, argv);
#endif
	envp = arge;
	all = 0;
	type = NULL;
	mnttype = MOUNT_UFS;
	mntname = "ufs";
#if SEC_ARCH
	/*
	 * To minimize #ifdef clutter, we add the flag letters
	 * for all possible label options to the option string
	 * and just protect the individual cases with appropriate
	 * policy conditionals. If a flag for a non-configured
	 * policy is given, we still get the usage message by
	 * falling into the default case.
	 */
	while ((ch = getopt(argc, argv, "adfrwuvt:o:A:S:I:C:")) != EOF)
#else
	while ((ch = getopt(argc, argv, "adfrwuvt:o:P:E:")) != EOF)
#endif
		switch((char)ch) {
		case 'a':
			all = 1;
			break;
		case 'd':
			mntflg |= MNT_FMOUNT;
			break;
		case 'f':
			fake = 1;
			break;
		case 'r':
			type = FSTAB_RO;
			break;
		case 'u':
			mntflg |= MNT_UPDATE;
			break;
		case 'v':
			verbose = 1;
			break;
		case 'w':
			type = FSTAB_RW;
			break;
		case 'o':
			options = optarg;
			break;
		case 't':
			vfslist = makevfslist(optarg);
			mnttype = getmnttype(optarg);
			break;
#if SEC_ARCH
#if SEC_ACL
		case 'A':
#endif
#if SEC_MAC
		case 'S':
#endif
#if SEC_ILB
		case 'I':
#endif
#if SEC_NCAV
		case 'C':
#endif
			mount_optflag(ch, optarg);
			break;
#endif
		case 'P': /* -P startingPass-number */
                        /* if we have '..' syntax, isolate the staring pass */
                        if ( cp=strchr(optarg,'.') ) {
				/* must have '..' */
				if ( *(cp+1) != '.' ) {
					fprintf(stderr,
						"%s: Invalid -P %s.\n",optarg);
					exit(EINVAL);
				}
                                *cp = '\0';
                        }
			startingPass = atoi(optarg);
 			if (startingPass <= 0) {  
				fprintf(stderr,"%s: Invalid -P %d.\n",
					pgmname,startingPass);
				exit(EINVAL);
			}
			/* ending pass specified? -Pstart..end */
			if ( cp ) {
				cp += 2;        /* skip over '..' */
				endingPass = atoi(cp);
			}
			if ( (endingPass != 0) && (endingPass < startingPass)) {
				fprintf(stderr, "%s: Invalid -P %d..%d, end < starting Pass.\n",
					pgmname,startingPass,endingPass); 
				exit(EINVAL);
			}
			break;
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;

	/* NOSTRICT */

	if (all) {
		rval = 0;
		while (fs = getfsent()) {
			if (BADTYPE(fs->fs_type))
				continue;
			if (badvfsname(fs->fs_vfstype, vfslist))
				continue;
			/* `/' is special, it's always mounted */
			if (!strcmp(fs->fs_file, "/"))
				flags = MNT_UPDATE;
			else
				flags = mntflg;
			mnttype = getmnttype(fs->fs_vfstype);
#if SEC_ARCH
			/*
			 * Clear out any security labels that were
			 * specified in the fstab entry for the previous
			 * filesystem.
			 */
			mount_resetopts();
#endif /* SEC_ARCH */
                        /*
                         * Do we want to do this pass? Make sure we only look at
			 * real (non-NFS) filesystems.
                         */
			if ( mnttype != MOUNT_NFS ) {
                        	if ( fs->fs_passno < startingPass )
                                	continue;
                        	if (endingPass && (fs->fs_passno > endingPass))
                                	continue;
			}
			rval |= mountfs(fs->fs_spec, fs->fs_file, flags,
					type, options, fs->fs_mntops);
		}
		exit(rval);
	}

	if (argc == 0) {
		if (verbose || fake || type) {
			fprintf(stderr, "It needs additional argument, either -a option or mount point\n");
			usage();
		}
		if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) == 0) {
			fprintf(stderr,
				"mount: cannot get mount information\n");
			exit(1);
		}
		for (i = 0; i < mntsize; i++) {
			if (badvfstype(mntbuf[i].f_type, vfslist))
				continue;
			prmount(mntbuf[i].f_mntfromname, mntbuf[i].f_mntonname,
				mntbuf[i].f_flags, mntbuf[i].f_type);
		}
		exit(0);
	}

	if (argc == 1 && (mntflg & MNT_UPDATE)) {
		if ((mntbuf = getmntpt(*argv)) == NULL) {
			fprintf(stderr,
			    "mount: unknown special file or file system %s.\n",
			    *argv);
			exit(1);
		}
		mnttype = mntbuf->f_type;
		if (!strcmp(mntbuf->f_mntfromname, "root_device")) {
			fs = getfsfile("/");
			strcpy(mntbuf->f_mntfromname, fs->fs_spec);
		}
		ret = mountfs(mntbuf->f_mntfromname, mntbuf->f_mntonname,
			      mntflg, type, options, (char *)NULL);
	} else if (argc == 1) {
		if (!(fs = getfsfile(*argv)) && !(fs = getfsspec(*argv))) {
			fprintf(stderr,
			    "mount: unknown special file or file system %s.\n",
			    *argv);
			exit(1);
		}
		if (BADTYPE(fs->fs_type)) {
			fprintf(stderr,
			    "mount: %s has unknown file system type.\n", *argv);
			exit(1);
		}
		mnttype = getmnttype(fs->fs_vfstype);
		ret = mountfs(fs->fs_spec, fs->fs_file, mntflg,
			      type, options, fs->fs_mntops);
	} else if (argc != 2) {
		usage();
		ret = 1;
	} else {
		/*
		 * If -t flag has not been specified, and spec
		 * contains either a ':' or a '@' then assume that
		 * an NFS filesystem is being specified ala Sun.
		 */
		if (vfslist == (char **)0 &&
		    (index(argv[0], ':') || index(argv[0], '@')))
			mnttype = MOUNT_NFS;
		ret = mountfs(argv[0], argv[1], mntflg, type, options, 
			      (char *)NULL);
	}
	if ((pidfile = fopen(_PATH_MOUNTDPID, "r")) != NULL) {
#ifdef TNC
		pgrp = 0;
		fscanf(pidfile, "%d", &pgrp);
		if (pgrp > 0)
			killpg(pgrp, SIGHUP);
#else
		pid = 0;
		fscanf(pidfile, "%d", &pid);
		if (pid > 0)
			kill(pid, SIGHUP);
#endif
		fclose(pidfile);
	}
	exit (ret);
}

mountfs(spec, name, flags, type, options, mntopts)
	char *spec, *name, *type, *options, *mntopts;
	int flags;
{
	extern int errno;
	int status;
	pid_t pid;
	int argc, i;
	struct ufs_args args;
	struct nfs_args nfsargs;
	char *argp, *argv[50];
	char execname[MAXPATHLEN + 1], flagval[12];

	nfsargs = nfsdefargs;
	if (mntopts)
		getstdopts(mntopts, &flags);
	if (options)
		getstdopts(options, &flags);
	if (type)
		getstdopts(type, &flags);

#if SEC_ARCH
	/*
	 * Look for label options specified with -o on the command line.
	 * These override corresponding ones specified in fstab.
	 */
	if (options)
		mount_getopts(options);
	/*
	 * Look for label options specified in the fstab entry.
	 */
	if (mntopts)
		mount_getopts(mntopts);
#endif
#if SEC_BASE
	mount_checkauth();
#endif
	switch (mnttype) {
	case MOUNT_UFS:
		if (mntopts)
			getufsopts(mntopts, &flags);
		if (options)
			getufsopts(options, &flags);
		args.fspec = spec;
		args.exroot = DEFAULT_ROOTUID;
		if (flags & MNT_RDONLY)
			args.exflags = MNT_EXRDONLY;
		else
			args.exflags = 0;
		argp = (caddr_t)&args;
		break;

#ifdef PFS
	case MOUNT_PFS:
		if (mntopts)
			getufsopts(mntopts, &flags);
		if (options) 
			getufsopts(options, &flags);

		/*
		 * Allocate a buffer for statpfs record.  Note that
		 * if the -o stripedirs... option is specified, this
		 * record must store a variable number of pathnames,
		 * each of variable length.  The buffer is initially
		 * allocated of size PFSARGS_BUFSZ, if more is needed
		 * the a larger buffer will be allocated.
		 */
		pfsargs_bufsz = PFSARGS_BUFSZ;
		pfsargsp = (struct pfs_args *) malloc(pfsargs_bufsz);

		if (pfsargsp == NULL) {
			fprintf(stderr,
				"mount: cannot allocate memory for stripe options\n");
			exit(1);
		}

		bzero((char *)pfsargsp, pfsargs_bufsz);
		/*
		 * Handle options from the fstab file:
		 */
		if (mntopts) {
			getpfsopts(mntopts);
		}

		/*
		 * Handle command line options:
		 */
		if (options) {
			getpfsopts(options);
		}

		/*
		 * If no command line or fstab entry, then
		 * use the defaults.
		 */
		if ((!options) && (!mntopts)) {
			getpfsopts(NULL);
		}
	
#ifdef	DEBUG_PFS
		dumppfsattr(&pfsargsp->stripe_attr);
#endif

		/*
		 * Finish up UFS stuff.
		 */
		pfsargsp->fs_args.fspec = spec;
		pfsargsp->fs_args.exroot = DEFAULT_ROOTUID;
		if (flags & MNT_RDONLY)
			pfsargsp->fs_args.exflags = MNT_EXRDONLY;
		else
			pfsargsp->fs_args.exflags = 0;

		argp = (caddr_t)pfsargsp;
		break;
#endif	/* PFS */

	case MOUNT_S5FS:
		if (mntopts)
			gets5fsopts(mntopts, &flags);
		if (options)
			gets5fsopts(options, &flags);
		args.fspec = spec;
		argp = (caddr_t)&args;
		break;

#ifdef NFS
	case MOUNT_NFS:
		retrycnt = DEF_RETRY;
		if (mntopts)
			getnfsopts(mntopts, &nfsargs, &opflags, &retrycnt);
		if (options)
			getnfsopts(options, &nfsargs, &opflags, &retrycnt);
		if (argp = getnfsargs(spec, &nfsargs)) {
#ifdef	DEBUG
			dumpnfsargs(&nfsargs);
#endif
			break;
		}
		return (1);
#endif /* NFS */

	case MOUNT_MFS:
	default:
		argv[0] = mntname;
		argc = 1;
		if (flags) {
			argv[argc++] = "-F";
			sprintf(flagval, "%d", flags);
			argv[argc++] = flagval;
		}
		if (mntopts)
			argc += getexecopts(mntopts, &argv[argc]);
		if (options)
			argc += getexecopts(options, &argv[argc]);
		argv[argc++] = spec;
		argv[argc++] = name;
		argv[argc++] = NULL;
		sprintf(execname, "%s/mount_%s", _PATH_EXECDIR, mntname);
		if (verbose) {
			(void)printf("exec: %s", execname);
			for (i = 1; i < argc; i++)
				(void)printf(" %s", argv[i]);
			(void)printf("\n");
		}
		if (fake)
			break;
		if (pid = vfork()) {
			if (pid == -1) {
				perror("mount: vfork starting file system");
				return (1);
			}
			if (waitpid(pid, &status, 0) != -1 &&
			    WIFEXITED(status) &&
			    WEXITSTATUS(status) != 0)
				return (WEXITSTATUS(status));
			spec = mntname;
			goto out;
		}
		execve(execname, argv, envp);
		fprintf(stderr, "mount: cannot exec %s for %s: ",
			execname, name);
		perror((char *)NULL);
		exit (1);
		/* NOTREACHED */

	}
#if SEC_ARCH
	/*
	 * Convert any label options that were specified into their
	 * internal representations, and mount the filesystem.
	 */
	mount_setattrs();
	errno = ESUCCESS;
	if (!fake && mount_domount(mnttype, name, flags, argp))
#else
	if (!fake && mount(mnttype, name, flags, argp))
		
#endif
	{
		if (opflags & ISBGRND)
			exit(1);
		fprintf(stderr, "%s on %s: ", spec, name);
#if SEC_ARCH
		{
		    extern int sec_errno;
		    extern char *sys_secerrlist[];
		    if (sec_errno) {
			fprintf(stderr, "%s\n", sys_secerrlist[sec_errno]);
			return(1);
		    }
		}
#endif
		switch (errno) {
		case EBUSY:
			fprintf(stderr, "Already mounted or busy mount point\n");
			break;
		case EDIRTY:
			fprintf(stderr, "Dirty file system\n");
			break;
		case EMFILE:
			fprintf(stderr, "Mount table full\n");
			break;
		case EINVAL:
			if (flags & MNT_UPDATE)
				fprintf(stderr, "Specified device does %s\n",
					"not match mounted device");
			else
				fprintf(stderr, "Bogus super block\n");
			break;
		case EOPNOTSUPP:
			fprintf(stderr, "Operation not supported\n");
			break;
#ifdef	PFS
		case EFSNOTSUPP:
			fprintf(stderr, "Invalid PFS stripe attribute specification\n");
			break;
#endif
		default:
			perror((char *)NULL);
			break;
		}

#ifdef	PFS
		if (mnttype == MOUNT_PFS) 
			free(pfsargsp);
#endif
		return(1);
	}

out:
	if (verbose)
		prmount(spec, name, flags, mnttype);

#ifdef	PFS
	if (mnttype == MOUNT_PFS) 
		free(pfsargsp);
#endif
	if (opflags & ISBGRND)
		exit(1);
	return(0);
}

#ifdef	PFS
static void
prpfsoptions(mount_name, first)
	char *mount_name;
	int first;
{
	struct statpfs *pfsbuf = NULL;
	pathname_t *sdir;
	int pfsbufsize = PFSARGS_BUFSZ;
	int i;

	/*
	 * Allocate a buffer large enough to hold the PFS stripe attributes,
	 * and get the attributes.
	 */
	for (;;) {
		pfsbuf = (struct statpfs *) malloc(pfsbufsize);
		if (pfsbuf == NULL) {
			fprintf(stderr, "mount: cannot allocate memory for stripe options\n");
			exit(1);
		}

		if (statpfs(mount_name, NULL, pfsbuf, pfsbufsize) < 0) {
			fprintf(stderr,
				"mount: cannot statpfs mounted file system %s\n",
				mount_name);
			perror((char *)NULL);
			return;
		}
		if (pfsbufsize >= pfsbuf->p_reclen)	/* success */
			break;

		pfsbufsize = pfsbuf->p_reclen;
		free(pfsbuf);
	}

	/*
	 * Now with the data in hand, print it out.
	 */
#ifdef	DEBUG_PFS
	printf("mount: prpfsoptions: statpfs succeeded\n");
	dumppfsattr(pfsbuf);
#endif
	sdir = &pfsbuf->p_sdirs;
	(void)printf("%sstripeunit=%d,stripedirs=", !first ? "" : ",",
		     pfsbuf->p_sunitsize);
	for (i = 0; i < pfsbuf->p_sfactor; i++) {
		(void)printf("\n    %s", sdir->name);
		sdir = NEXTPATH(sdir);
	}
	free(pfsbuf);
}
#endif	/* PFS */

static void
prmount(spec, name, flags, type)
	char *spec, *name;
	register short flags;
	register short type;
{
	register int first;

	if (opflags & ISBGRND)
		return;

	(void)printf("%s on %s", spec, name);

	/*
	 * Print the type of filesystem mounted.
	 */
#define	PRT(msg)	(void)printf(" type %s", msg)
	switch (type) {
	case MOUNT_UFS:
		PRT("ufs");
		break;
/*
	case MOUNT_FFS:
		PRT("ffs");
		break;
*/
#ifdef	PFS
	case MOUNT_PFS:
		PRT("pfs");
		break;
#endif
	case MOUNT_S5FS:
		PRT("s5fs");
		break;
	case MOUNT_NFS:
		PRT("nfs");
		break;
	case MOUNT_MFS:
		PRT("mfs");
		break;
	default:
		break;
	} /* end switch type */

        if (!(flags & MNT_VISFLAGMASK)) {
                (void)printf("\n");
                return;
	}
	first = 0;
#define	PR(msg)	(void)printf("%s%s", !first++ ? " (" : ",", msg)
	if (flags & MNT_RDONLY)
		PR("read-only");
	if (flags & MNT_NOEXEC)
		PR("noexec");
	if (flags & MNT_NOSUID)
		PR("nosuid");
	if (flags & MNT_NODEV)
		PR("nodev");
	if (flags & MNT_FASTPATH)
		PR("fastpath");
	if (flags & MNT_SYNCHRONOUS)
		PR("synchronous");
	if (flags & MNT_QUOTA)
		PR("with quotas");
	if (flags & MNT_LOCAL)
		PR("local");
#if SEC_ARCH
        if ((flags & MNT_SECURE) == 0)
                PR("unextended");
#endif
	if (flags & MNT_EXPORTED)
		if (flags & MNT_EXRDONLY)
			PR("NFS exported read-only");
		else
			PR("NFS exported");
#ifdef PFS
	if (type == MOUNT_PFS)
		prpfsoptions(name, first);
#endif
	(void)printf(")\n");
}

getmnttype(fstype)
	char *fstype;
{

	mntname = fstype;
	if (!strcmp(fstype, "ufs"))
		return (MOUNT_UFS);
	if (!strcmp(fstype, "s5fs"))
		return (MOUNT_S5FS);
	if (!strcmp(fstype, "nfs"))
		return (MOUNT_NFS);
	if (!strcmp(fstype, "mfs"))
		return (MOUNT_MFS);
#ifdef	PFS
/*
	if (!strcmp(fstype, "ffs"))
		return (MOUNT_FFS);
*/
	if (!strcmp(fstype, "pfs"))
		return (MOUNT_PFS);
#endif	/* PFS */
	return (0);
}

usage()
{
#if SEC_ARCH
        char labelargs[48];
	
	sprintf(labelargs, "%s%s%s%s",
#if SEC_ACL
		" [-A acl]",
#endif
#if SEC_MAC
		" [-S slevel]",
#endif
#if SEC_ILB
		" [-I ilevel]",
#endif
#if SEC_NCAV
		" [-C ncav]",
#endif
		"", "", "", "");
	fprintf(stderr, "usage: mount [-adfuvrw]\n");
	fprintf(stderr, "   or  mount [-dfuvrw]%s special | node\n", labelargs);
	fprintf(stderr, "   or  mount [-dfuvrw]%s special node\n", labelargs);
#else
#ifdef	PFS
	fprintf(stderr, "usage:\n  mount %s %s\n  mount %s\n  mount %s\n",
		"[ -dfrwuv ] [ -t nfs | ufs | pfs | external_type ]",
		"[ -o options ] special node",
		"[ -adfrwuv ] [ -t nfs | ufs | pfs | external_type ]",
		"[ -dfrwuv ] special | node");
#else
	fprintf(stderr, "usage:\n  mount %s %s\n  mount %s\n  mount %s\n",
		"[ -dfrwuv ] [ -t nfs | ufs | external_type ]",
		"[ -o options ] special node",
		"[ -adfrwuv ] [ -t nfs | ufs | external_type ]",
		"[ -dfrwuv ] special | node");
#endif /* PFS */
#endif
	exit(1);
}

getstdopts(options, flagp)
	char *options;
	long *flagp;
{
	register char *opt;
	int negative;
	char optbuf[BUFSIZ];

	(void)strcpy(optbuf, options);
	for (opt = strtok(optbuf, ","); opt; opt = strtok((char *)NULL, ",")) {
		if (opt[0] == 'n' && opt[1] == 'o') {
			negative++;
			opt += 2;
		} else {
			negative = 0;
		}
		if (!negative && !strcasecmp(opt, FSTAB_RO)) {
			*flagp |= MNT_RDONLY;
			continue;
		}
		if (!negative && !strcasecmp(opt, FSTAB_RW)) {
			*flagp &= ~MNT_RDONLY;
			continue;
		}
		if (!strcasecmp(opt, "exec")) {
			if (negative)
				*flagp |= MNT_NOEXEC;
			else
				*flagp &= ~MNT_NOEXEC;
			continue;
		}
		if (!strcasecmp(opt, "suid")) {
			if (negative)
				*flagp |= MNT_NOSUID;
			else
				*flagp &= ~MNT_NOSUID;
			continue;
		}
		if (!strcasecmp(opt, "dev")) {
			if (negative)
				*flagp |= MNT_NODEV;
			else
				*flagp &= ~MNT_NODEV;
			continue;
		}
		if (!strcasecmp(opt, "fastpath")) {
			if (!negative)
				*flagp |= MNT_FASTPATH;
			else
				*flagp &= ~MNT_FASTPATH;
			continue;
		}
		if (!strcasecmp(opt, "synchronous")) {
			if (!negative)
				*flagp |= MNT_SYNCHRONOUS;
			else
				*flagp &= ~MNT_SYNCHRONOUS;
			continue;
		}
	}
}

gets5fsopts(options, flagp)
	char *options;
	long *flagp;
{

	return;
}

getufsopts(options, flagp)
	char *options;
	int *flagp;
{

	return;
}

#ifdef	PFS
/*
 * Fill in the PFS stripe_attr structure.  Somewhat complicated parsing here
 * since the user can specify a variable number of stripedirs, each of
 * variable length.
 */
int
getpfsopts(options)
	char *options;
{
	register char 	*opt, *nextopt;
	char		*optval, *dirp, *nextdirp;
	char		*stripe_group;
	pathname_t	dummy_sdir;

	sattr = &pfsargsp->stripe_attr;
	sdir = &sattr->p_sdirs;

	/*
	 * Start going through the options:
	 */
	opt = options;
	optval = NULL;
	while (opt != NULL && *opt != '\0') {
		if ((nextopt = index(opt, ',')) != NULL)
			*nextopt++ = '\0';
		if ((optval = index(opt, '=')) != NULL) {
			*optval++ = '\0';
		}

		/*
		 * Test for a string match on this option and set the
		 * appropriate field in the PFS stripe_attr structure.
		 */
		if (!strcasecmp(opt, "stripeunit") && optval != NULL) {
			sattr->p_sunitsize = checksize(optval);
			if (sattr->p_sunitsize <= 0) {
				fprintf(stderr,
					"mount: invalid stripe unit size\n");
				free(pfsargsp);
				exit(1);
			}

		} else if (!strcasecmp(opt, "stripedirs") && optval != NULL) {
			dirp = optval;
			/*
			 * Zero out any stripedir data that was left over
			 * from the fstab entry and reset the size. 
			 */
			sattr->p_reclen = sizeof(struct statpfs) - sizeof(pathname_t);
			do {
				/* isolate next stripedir in command line */
				nextdirp = index(dirp, ':');
				if (nextdirp != NULL)
					*nextdirp++ = '\0';
				/*
				 * Add stripe directory name:
				 */
				addstripe_dir(dirp);
				dirp = nextdirp;

			} while (nextdirp != NULL);

		} else if (!strcasecmp(opt, "stripegroup") && optval != NULL) {
			stripe_group = optval;
			sattr->p_reclen = sizeof(struct statpfs) - 
					  sizeof(pathname_t);
			if (sgroup_opts(stripe_group) < 0) {
				free(pfsargsp);
				exit(1);
			}

		} else {
			if ((!strcmp(opt, FSTAB_RW)) ||
			    (!strcmp(opt, FSTAB_RQ)) ||
			    (!strcmp(opt, FSTAB_RO)) ||
			    (!strcmp(opt, FSTAB_SW)) ||
			    (!strcmp(opt, FSTAB_XX))) {

			} else {
		 		/*
		  	 	 * Don't understand the option:
		  	 	 */
				fprintf(stderr,
					"mount: unknown option %s\n", opt);
				free(pfsargsp);
				exit(1);
			}
		}
		opt = nextopt;
	}

	/*
	 * Set up some defaults only if the pfs_args structure has not 
	 * been updated yet, (the pfs_args structure may have been initialized
	 * from the fstab file if the -a switch was specified or from the 
	 * command line if a -o switch was specified.
	 */
	if (sattr->p_sunitsize == 0) {
		/*
		 * use the default stripe unit size:
		 */
		sattr->p_sunitsize = _DEFAULT_SUNIT_SIZE;
	}

	if (sattr->p_reclen == 0) {
		/*
		 * If no stripe directories or stripe groups were specified,
		 * then the default is the stripe group "all" from the
		 * /etc/pfstab file.  If "all" not specified or the 
		 * /etc/pfstab file in non-existent, then an error
		 * is printed.
		 */
		sattr->p_reclen = sizeof(struct statpfs) - sizeof(pathname_t);
		if (sgroup_opts(_PFSTAB_DEFAULT_GROUP, sattr, &pfsargsp) < 0) {
			free(pfsargsp);
			exit(1);
		}
		
	}
}

/*
 * Read the pfstab file and fill in the PFS stripe_attr structure 
 * with all stripe directories that match the specified stripe 
 * group name.
 */
int
sgroup_opts(stripe_group)
	char *stripe_group;
{
	FILE		*fp;
	char		*cp;
	char		line[_PFSTAB_MAX_LINE];
	char		*stripe_dir;
	char		*group;
	int		found = 0;

	/*
	 * Open the pfstab file:
	 */
	if ((fp = fopen(_PFSTAB_PATH, "r")) == NULL) {
		fprintf(stderr,
			"\nmount: unable to open %s\n",_PFSTAB_PATH);	
		return -1;
	}
	/*
	 * Read the file looking for all occurrences of the 
	 * stripe group name.
	 */
	while((cp = fgets(line, _PFSTAB_MAX_LINE, fp)) != NULL) {
		/*
		 * First item on every line in file is always
		 * the stripe directory name.
		 */
		stripe_dir = strtok(cp, " \t\n"); 
		if (stripe_dir == NULL) {
			continue;
		}

		/*
		 * If the first character on this line is a '#',
		 * then treat the line as a comment.
		 */
		if (*stripe_dir == '#') {
			continue;
		}

		while((group = strtok((char *)NULL, " \t\n")) != NULL) {
			/*
			 * See if in stripe group:
			 */
			if (!strcmp(group, stripe_group)) {
				addstripe_dir(stripe_dir);
				found = 1;
			}
		}
	}

	(void) fclose(fp);
	
	if (!found) {
		fprintf(stderr, "\nmount: stripe group %s not in pfstab\n",
			stripe_group);
		return -1;
	}
				
	return 0;
}


int		
addstripe_dir(stripedir_name)
char	*stripedir_name;	/* Stripe directory name to add. */
{
	pathname_t	dummy_sdir;
	/* 
	 * range check on pfsargs 
	 */
	dummy_sdir.namelen = (uint_t) strlen(stripedir_name);
	if (((char *)sdir + PATHSIZE(&dummy_sdir)) >
	    ((char *)pfsargsp + pfsargs_bufsz)) {

        	struct pfs_args *newpfsargsp;
		uint_t	sdir_offset;
		/*
		 * Allocate another buffer twice as large and copy
		 * the contents of the current buffer into it.  Re-
		 * adjust the pointers and then deallocate the 
		 * current buffer.
		 */
		newpfsargsp = (struct pfs_args *) malloc(pfsargs_bufsz * 2);
		if (newpfsargsp == NULL) {
			fprintf(stderr,
				"mount: cannot allocat memory for stripe options\n");
				free(pfsargsp);
				exit(1);
		}
		bzero((char *)newpfsargsp, pfsargs_bufsz * 2);
		bcopy((char *) pfsargsp, (char *)newpfsargsp,  pfsargs_bufsz);
		pfsargs_bufsz *=2;
		sdir_offset = (uint_t)sdir - (uint_t)pfsargsp;
		free(pfsargsp);
		pfsargsp = newpfsargsp;
		sdir = (pathname_t *)((uint_t)pfsargsp + (uint_t)sdir_offset);
		sattr = &pfsargsp->stripe_attr;
	}

	sdir->namelen = strlen(stripedir_name);
	strcpy(sdir->name, stripedir_name);
	sattr->p_reclen += PATHSIZE(sdir);
	sattr->p_sfactor++;
	sdir = NEXTPATH(sdir);
	return 0;
}
	

long checksize(s)
        char    *s;
{
	esize_t t;
        char    *p;
        int     eflg = 0;

        p = s;
	t.slow = 0;
	t.shigh = 0;
        while (*p >= '0' && *p <= '9') {
                t = _emul(t, 10);
		t.slow += *p++ - '0';
/*
                t = _eadd1(_emul(t, 10), *p++ - '0');
*/
        }
        if (*p == 'k' || *p == 'K') {
                t = _emul(t, 1024);
		*p++;
        }
        else if (*p == 'm' || *p == 'M') {
                t = _emul(t, 1048576);
		*p++;
        }
        else if (*p == 'g' || *p == 'G') {
                t = _emul(t, 1073741824);
		*p++;
        }

	/* this will check for any other invalid trailing character */
        if (*p != '\0' && *p != 0) {
                eflg = 1;
        }

        if (eflg || t.slow == 0) {
		fprintf(stderr, "mount: invalid stripe unit size\n");
		return 0;
        }
	/* if the upper part of the 64bit size value has any value except
           zero OR the lower part has gone negative then we have exceeded
           the 2G-1 stripe unitsize limit which if also the UFS file size
           limit for each stripe file */
        if (t.shigh != 0 || t.slow < 0) {
		fprintf(stderr, "mount: stripe unit size %s is too large\n",
			s);
		return 0;
        }
        return t.slow;
}
#endif	/* PFS */

getexecopts(options, argv)
	char *options;
	char **argv;
{
	register int argc = 0;
	register char *opt;

	for (opt = strtok(options, ","); opt; opt = strtok(NULL, ",")) {
		if (opt[0] != '-')
			continue;
		argv[argc++] = opt;
		if (opt[2] == '\0' || opt[2] != '=')
			continue;
		opt[2] = '\0';
		argv[argc++] = &opt[3];
	}
	return (argc);
}

struct statfs *
getmntpt(name)
	char *name;
{
	long mntsize;
	register long i;
	struct statfs *mntbuf;

	mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
	for (i = 0; i < mntsize; i++) {
		if (!strcmp(mntbuf[i].f_mntfromname, name) ||
		    !strcmp(mntbuf[i].f_mntonname, name))
			return (&mntbuf[i]);
	}
	return ((struct statfs *)0);
}

static int skipvfs;

badvfstype(vfstype, vfslist)
	short vfstype;
	char **vfslist;
{

	if (vfslist == 0)
		return(0);
	while (*vfslist) {
		if (vfstype == getmnttype(*vfslist))
			return(skipvfs);
		vfslist++;
	}
	return (!skipvfs);
}

badvfsname(vfsname, vfslist)
	char *vfsname;
	char **vfslist;
{

	if (vfslist == 0)
		return(0);
	while (*vfslist) {
		if (strcmp(vfsname, *vfslist) == 0)
			return(skipvfs);
		vfslist++;
	}
	return (!skipvfs);
}

char **
makevfslist(fslist)
	char *fslist;
{
	register char **av, *nextcp;
	register int i;

	if (fslist == NULL)
		return (NULL);
	if (fslist[0] == 'n' && fslist[1] == 'o') {
		fslist += 2;
		skipvfs = 1;
	}
	for (i = 0, nextcp = fslist; *nextcp; nextcp++)
		if (*nextcp == ',')
			i++;
	av = (char **)malloc((size_t)(i+2) * sizeof(char *));
	if (av == NULL)
		return (NULL);
	nextcp = fslist;
	i = 0;
	av[i++] = nextcp;
	while (nextcp = index(nextcp, ',')) {
		*nextcp++ = '\0';
		av[i++] = nextcp;
	}
	av[i++] = 0;
	return (av);
}


#ifdef NFS
/*
 * Handle the getoption arg.
 * Essentially update "opflags", "retrycnt" and "nfsargs"
 */
getnfsopts(optarg, nfsargsp, opflagsp, retrycntp)
	char *optarg;
	struct nfs_args *nfsargsp;
	int *opflagsp;
	int *retrycntp;
{
	register char *cp, *nextcp;
	int num;
	char *nump;

	cp = optarg;
	while (cp != NULL && *cp != '\0') {
		if ((nextcp = index(cp, ',')) != NULL)
			*nextcp++ = '\0';
		if ((nump = index(cp, '=')) != NULL) {
			*nump++ = '\0';
			num = atoi(nump);
		} else
			num = -1;
		/*
		 * Just test for a string match and do it
		 */
		if (!strcasecmp(cp, "bg")) {
			*opflagsp |= BGRND;
		} else if (!strcasecmp(cp, "soft")) {
			nfsargsp->flags |= NFSMNT_SOFT;
		} else if (!strcasecmp(cp, "nointr")) {
			nfsargsp->flags &= ~NFSMNT_INT;
		} else if (!strcasecmp(cp, "noconn")) {
			nfsargsp->flags |= NFSMNT_NOCONN;
		} else if (!strcasecmp(cp, "retry") && num > 0) {
			*retrycntp = num;
		} else if (!strcasecmp(cp, "rsize") && num > 0) {
			nfsargsp->rsize = num;
			nfsargsp->flags |= NFSMNT_RSIZE;
		} else if (!strcasecmp(cp, "wsize") && num > 0) {
			nfsargsp->wsize = num;
			nfsargsp->flags |= NFSMNT_WSIZE;
		} else if (!strcasecmp(cp, "timeo") && num > 0) {
			nfsargsp->timeo = num;
			nfsargsp->flags |= NFSMNT_TIMEO;
		} else if (!strcasecmp(cp, "retrans") && num > 0) {
			nfsargsp->retrans = num;
			nfsargsp->flags |= NFSMNT_RETRANS;
		} 
		cp = nextcp;
	}
}

char *
getnfsargs(spec, nfsargsp)
	char *spec;
	struct nfs_args *nfsargsp;
{
	extern int errno;
	register CLIENT *clp;
	struct hostent *hp;
#if     UNIX_DOMAIN
	static struct sockaddr_un saddr_un;
	int     using_local = 0;
#endif 
	static struct sockaddr_in saddr;
	struct timeval pertry, try;
	enum clnt_stat clnt_stat;
	int so = RPC_ANYSOCK;
	char *hostp, *delimp;
	u_short tport;
	static struct nfhret nfhret;
	static char nam[MNAMELEN + 1];

	strncpy(nam, spec, MNAMELEN);
	nam[MNAMELEN] = '\0';
	if ((delimp = index(spec, '@')) != NULL) {
		hostp = delimp + 1;
	} else if ((delimp = index(spec, ':')) != NULL) {
		hostp = spec;
		spec = delimp + 1;
	} else {
		fprintf(stderr,
		    "No <host>:<dirpath> or <dirpath>@<host> spec\n");
		return (0);
	}
	*delimp = '\0';
#if     UNIX_DOMAIN
#define NFSD_PATH_NAME  "/tmp/.nfsd"
#define LOCAL   "localhost"
	/*
	 * recognize the following as indications of UNIX domain mounts:
	 * 	no hostname (e.g. "path@" or ":path")
 	 * 	hostname == "unix" (e.g. "path@unix" or "unix:path"
	 * The former will be indicated at this point by a hostp
	 * pointing at a null.  The latter is a simple comparision.
	 */
	if ( (*hostp == '\0') || (strcmp(hostp, "unix") == 0) ) {
		hostp=LOCAL;
		using_local = 1;
	}
#endif  /* UNIX_DOMAIN */
	if ((hp = gethostbyname(hostp)) == NULL) {
		fprintf(stderr, "Can't get net id for host\n");
		return (0);
	}
	bzero( (char *)&saddr, sizeof(struct sockaddr_in) );
	bcopy(hp->h_addr, (caddr_t)&saddr.sin_addr, hp->h_length);
	nfhret.stat = EACCES;	/* Mark not yet successful */
	while (retrycnt > 0) {
		saddr.sin_family = AF_INET;
#if     UNIX_DOMAIN
		if (!using_local) {
			saddr.sin_port = htons(PMAPPORT);
			tport = pmap_getport(&saddr, RPCPROG_NFS,
				NFS_VER2, IPPROTO_UDP);
			if (tport == 0) {
				if ((opflags & ISBGRND) == 0)
					clnt_pcreateerror("NFS Portmap");
				goto skip_mount;
			}
		}
#else   /* UNIX_DOMAIN */
		saddr.sin_port = htons(PMAPPORT);
		if ((tport = pmap_getport(&saddr, RPCPROG_NFS,
		    NFS_VER2, IPPROTO_UDP)) == 0) {
			if ((opflags & ISBGRND) == 0) {
				clnt_pcreateerror("NFS Portmap");
			}
		} else {
#endif  /* UNIX_DOMAIN */
			saddr.sin_port = 0;
			pertry.tv_sec = 10;
			pertry.tv_usec = 0;
			if ((clp = clntudp_create(&saddr, RPCPROG_MNT,
			    RPCMNT_VER1, pertry, &so)) == NULL) {
				if ((opflags & ISBGRND) == 0)
					clnt_pcreateerror("Cannot MNT PRC");
			} else {
#ifdef notyet
				clp->cl_auth = authunix_create_default();
#else
				clp->cl_auth = authunix_default();
#endif
				try.tv_sec = 10;
				try.tv_usec = 0;
				clnt_stat = clnt_call(clp, RPCMNT_MOUNT,
				    xdr_dir, spec, xdr_fh, &nfhret, try);
				if (clnt_stat != RPC_SUCCESS) {
					if ((opflags & ISBGRND) == 0)
						clnt_perror(clp, "Bad MNT RPC");
				} else {
					auth_destroy(clp->cl_auth);
					clnt_destroy(clp);
					retrycnt = 0;
				}
			}
#if     UNIX_DOMAIN
skip_mount:
#else
		}
#endif  /* UNIX_DOMAIN */
		if (--retrycnt > 0) {
			if (opflags & BGRND) {
				opflags &= ~BGRND;
				if (fork())
					return (0);
				else
					opflags |= ISBGRND;
			} 
			sleep(10);
		}
	}
	if (nfhret.stat) {
		if (opflags & ISBGRND)
			exit(1);
		fprintf(stderr, "Can't access %s: ", spec);
		errno = nfhret.stat;
		perror(NULL);
		return (0);
	}
#if     UNIX_DOMAIN
        if (using_local) {
                saddr_un.sun_family = AF_UNIX;
                strcpy(saddr_un.sun_path, NFSD_PATH_NAME);
                /* saddr_un.sun_len = 0; */
                /*
                 * should change def. of nfsargs->addr to be
                 * a struct sockaddr *, but until then ...
                 */
                nfsargsp->addr = (struct sockaddr_in *) &saddr_un;
                nfsargsp->hostname = "unix";
        } else {
                saddr.sin_port = htons(tport);
                nfsargsp->addr = &saddr;
                nfsargsp->hostname = nam;
        }
        nfsargsp->fh = &nfhret.nfh; /* junk */
        return ((caddr_t)nfsargsp);

#else
	saddr.sin_port = htons(tport);
	nfsargsp->addr = &saddr;
	nfsargsp->fh = &nfhret.nfh;
	nfsargsp->hostname = nam;
	return ((caddr_t)nfsargsp);
#endif  /* UNIX_DOMAIN */
}

/*
 * xdr routines for mount rpc's
 */
xdr_dir(xdrsp, dirp)
	XDR *xdrsp;
	char *dirp;
{
	return (xdr_string(xdrsp, &dirp, RPCMNT_PATHLEN));
}

xdr_fh(xdrsp, np)
	XDR *xdrsp;
	struct nfhret *np;
{
	if (!xdr_u_long(xdrsp, &(np->stat)))
		return (0);
	if (np->stat)
		return (1);
	return (xdr_opaque(xdrsp, (caddr_t)&(np->nfh), NFSX_FH));
}

AUTH *
authunix_default()
{
	register int glen;
	char hostname[MAX_MACHINE_NAME+1];
	int	uid;
	int	gid;
	int	grps[NGROUPS];

	if (gethostname(hostname, MAX_MACHINE_NAME) < 0)
		abort();		
	hostname[MAX_MACHINE_NAME] = 0;
	uid = geteuid();
	gid = getegid();
	if ((glen = getgroups(NGROUPS, grps)) < 0)
		abort();
	/*
	 * major hack -- temporary only -- gmf
	 * Sun RPC cannot handle more than 8 groups.
	 */
	if (glen >= 8)
		glen = 7;

	return (authunix_create(hostname, uid, gid, glen, grps));
}
#endif /* NFS */

#ifdef DEBUG
dumpfsent(fs)
	register struct fstab *fs;
{
	fprintf(stderr, "fs_spec (block dev.): %s\n", fs->fs_spec);
	fprintf(stderr, "fs_file (path): %s\n", fs->fs_file);
	fprintf(stderr, "fs_type (e.g. ro, rw): %s\n", fs->fs_type);
	fprintf(stderr, "fs_freq (dump frequency): %d\n", fs->fs_freq);
	fprintf(stderr, "fs_passno (fsck pass no.): %d\n", fs->fs_passno);
	fprintf(stderr, "fs_vfstype (fs type): %s\n", fs->fs_vfstype);
	if (fs->fs_mntops)
		fprintf(stderr, "fs_mntops (mount options): %s\n", fs->fs_mntops);
}
dumpnfsargs(nfsargs)
struct nfs_args *nfsargs;
{
	fprintf(stderr, "nfsargs: \n");
	fprintf(stderr, "	flags:	 %x\n", nfsargs->flags);
	fprintf(stderr, "	wsize:	 %x\n", nfsargs->wsize);
	fprintf(stderr, "	rsize:	 %x\n", nfsargs->rsize);
	fprintf(stderr, "	timeo:	 %x\n", nfsargs->timeo);
	fprintf(stderr, "	retrans: %x\n", nfsargs->retrans);
	fprintf(stderr, "	host:    %s\n", nfsargs->hostname);
}

#ifdef	PFS
dumppfsattr(sattr)
	struct statpfs *sattr;
{
	pathname_t *sdir = &sattr->p_sdirs;
	int i;

	fprintf(stderr, "pfs stripe attributes: \n");
	fprintf(stderr, "  record length = %d\n", sattr->p_reclen);
	fprintf(stderr, "  magic = 0x%x\n", sattr->p_magic);
	fprintf(stderr, "  stripe unit size = %d\n", sattr->p_sunitsize);
	fprintf(stderr, "  stripe factor = %d\n", sattr->p_sfactor);

	for (i = 0; i < sattr->p_sfactor; i++) {
		fprintf(stderr, "  stripedir %d, length = %d, dir = %s\n",
		       i, sdir->namelen, sdir->name);
		sdir = NEXTPATH(sdir);
	}
}	
#endif	/* PFS */
#endif
