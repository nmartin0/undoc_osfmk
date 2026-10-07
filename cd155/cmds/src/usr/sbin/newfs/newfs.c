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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */
/* @(#)newfs.c	4.2 17:09:07 7/9/90 SecureWare */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: newfs.c,v $ $Revision: 2.23 $ (OSF) $Date: 1994/11/22 20:28:05 $";
#endif
/*
 * Copyright (c) 1983, 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*** "newfs.c	6.20 (Berkeley) 10/18/89"; ***/

/***
char copyright[] =
"@(#) Copyright (c) 1983, 1989 Regents of the University of California.\n\
 All rights reserved.\n";
***/

/*
 * newfs: friendly front end to mkfs
 */

#include <sys/secdefines.h>

#include <sys/param.h>
#include <sys/stat.h>
#include <ufs/fs.h>
#include <ufs/dir.h>
#include <sys/ioctl.h>
#ifdef	multimax
#include <layout.h>
#include <mmaxio/msioctl.h>
#undef MAXPARTITIONS
#endif	/* multimax */
#ifdef i386
#define DKTYPENAMES
#endif
#define KERNEL
#include <sys/disklabel.h>
#undef KERNEL
#include <sys/file.h>
#include <sys/mount.h>
#include <sys/wait.h>

#include <stdio.h>
#include <ctype.h>
#include <paths.h>

#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

#ifndef	multimax
#define COMPAT			/* allow non-labeled disks */
#endif

/*
 * The following two constants set the default block and fragment sizes.
 * Both constants must be a power of 2 and meet the following constraints:
 *	MINBSIZE <= DESBLKSIZE <= MAXBSIZE
 *	sectorsize <= DESFRAGSIZE <= DESBLKSIZE
 *	DESBLKSIZE / DESFRAGSIZE <= 8
 *
 * If making a System V file system, the default block size is DFL_S5BLKSIZE.
 */
#define	DFL_FRAGSIZE	8192
#define	DFL_BLKSIZE	65536
#define DFL_S5BLKSIZE   512

/*
 * Cylinder groups may have up to many cylinders. The actual
 * number used depends upon how much information can be stored
 * on a single cylinder. The default is to use 16 cylinders
 * per group.
 */
#define	DESCPG		16	/* desired fs_cpg */

/*
 * MINFREE gives the minimum acceptable percentage of file system
 * blocks which may be free. If the freelist drops below this level
 * only the superuser may continue to allocate blocks. This may
 * be set to 0 if no reserve of free blocks is deemed necessary,
 * however throughput drops by fifty percent if the file system
 * is run at between 90% and 100% full; thus the default value of
 * fs_minfree is 10%. With 10% free space, fragmentation is not a
 * problem, so we choose to optimize for time.
 */
#define MINFREE		10
#define DEFAULTOPT	FS_OPTTIME

/*
 * ROTDELAY gives the minimum number of milliseconds to initiate
 * another disk transfer on the same cylinder. It is used in
 * determining the rotationally optimal layout for disk blocks
 * within a file.
 *
 * This parameter is essentially outdated by SCSI, which performs its own 
 * rotational delay calculations independent of the file system.  For this
 * reason, the default is currently set to 0ms, so that the file system does
 * not adversely affect performance by adding even more rotational delay on
 * top of the delay that SCSI adds.
 */
#define ROTDELAY	0

/*
 * MAXCONTIG sets the default for the maximum number of blocks
 * that may be allocated sequentially. Since UNIX drivers are
 * not capable of scheduling multi-block transfers, this defaults
 * to 1 (ie no contiguous blocks are allocated).
 */
#define MAXCONTIG	1

/*
 * MAXBLKPG determines the maximum number of data blocks which are
 * placed in a single cylinder group. The default is one indirect
 * block worth of data blocks.
 */
#define MAXBLKPG(bsize)	((bsize) / sizeof(daddr_t))

/*
 * Each file system has a number of inodes statically allocated.
 * We allocate one inode slot per NFPI fragments, expecting this
 * to be far more than we will ever need.
 */
#define	NFPI		4

/*
 * For each cylinder we keep track of the availability of blocks at different
 * rotational positions, so that we can lay out the data to be picked
 * up with minimum rotational latency.  NRPOS is the default number of
 * rotational positions that we distinguish.  With NRPOS of 8 the resolution
 * of our summary information is 2ms for a typical 3600 rpm drive.
 */
#define	NRPOS		8	/* number distinct rotational positions */

/*
 * Borrow a define from mkfs.c
 */
#define POWEROF2(num)	(((num) & ((num) - 1)) == 0)

int	mfs;			/* run as the memory based filesystem */
int	Nflag;			/* run without writing file system */
int	fssize;			/* file system size */
int	ntracks;		/* # tracks/cylinder */
int	nsectors;		/* # sectors/track */
int	nphyssectors;		/* # sectors/track including spares */
int	secpercyl;		/* sectors per cylinder */
int	trackspares = -1;	/* spare sectors per track */
int	cylspares = -1;		/* spare sectors per cylinder */
int	sectorsize;		/* bytes/sector */
#ifdef tahoe
int	realsectorsize;		/* bytes/sector in hardware */
#endif
int	rpm;			/* revolutions/minute of drive */
int	interleave;		/* hardware sector interleave */
int	trackskew = -1;		/* sector 0 skew, per track */
int	headswitch = 0;		/* head switch time, usec */
int	trackseek = 0;		/* track-to-track seek, usec */
int	fsize = 0;		/* fragment size */
int	bsize = 0;		/* block size */
int     fragindir = 0;          /* fragments in indirect blocks */
int	cpg = DESCPG;		/* cylinders/cylinder group */
int	cpgflg;			/* cylinders/cylinder group flag was given */
int	minfree = MINFREE;	/* free space threshold */
int	opt = DEFAULTOPT;	/* optimization preference (space or time) */
int	density;		/* number of bytes per inode */
int	maxcontig = MAXCONTIG;	/* max contiguous blocks to allocate */
int	rotdelay = ROTDELAY;	/* rotational delay between blocks */
int	maxbpg;			/* maximum blocks per file in a cyl group */
int	nrpos = NRPOS;		/* # of distinguished rotational positions */
int	bbsize = BBSIZE;	/* boot block size */
int	sbsize = SBSIZE;	/* superblock size */
int	sboff = SBOFF;		/* superblock offset */
struct  fs	*fs_ptr=NULL;	/* superblock pointer. */
int	mntflags;		/* flags to be passed to mount */
u_long	memleft;		/* virtual memory available */
caddr_t	membase;		/* start address of memory based filesystem */

int     fstype = FS_BSDFFS;     /* type of file system, currently supported are
				   BSD FFS and System V style file systems.
				   Default file system type is BSD fast file
				   system */

#if	defined(COMPAT) || defined(i386)
char	*disktype;
int	unlabelled;
#endif
#if	defined(i386)
int	partno = -1;
#endif

char	device[MAXPATHLEN];
char	*progname;

#if SEC_ACL
char *acl_label = (char *) 0;
#endif
#if SEC_MAC
char *mand_label = (char *) 0;
#endif
#if SEC_ILB
char *ilb_label = (char *) 0;
#endif
#if SEC_BASE
static privvec_t additional_privs;
#endif
#if SEC_FSCHANGE
int mkfs_extended_fs;		/* make an extended format filesystem */
static int untagged;		/* make an untagged format filesystem */
#endif

extern	int errno;
char	*index();
char	*rindex();
char    *itoa();

main(argc, argv)
	int argc;
	char *argv[];
{
	char *cp, *special, *rindex();
	register struct partition *pp = NULL;
	register struct disklabel *lp = NULL;
	struct disklabel *getdisklabel();
	struct partition oldpartition;
	struct mfs_args args;
	struct stat st;
	int trust_partition = 0;
	int fsi, fso;
	register int i;
	int status;
#if	!defined(i386)
	int partno = -1;
#endif
	char buf[BUFSIZ];
	char dev_path[MAXPATHLEN];		/* device's path name. */

	int s5mkfs_pid;
	int stat_loc;                           /* result of wait */
	char *s5mkfs_arg;
	char bsize_string[30];
	char fssize_string[30];
	char *s5fs_proto = NULL;
	int s5fs_gap = 0;
	int s5fs_blk_per_cyl = 0;
	int s5fs_inodes = 0;
	char *s5fs_gap_string = "0";
	char *s5fs_blk_string = "0";
	char *s5fs_inode_string;
	int error;
#if SEC_BASE && !defined(SEC_STANDALONE)
	privvec_t saveprivs;

	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("sysadmin")) {
		fprintf(stderr, "%s: need sysadmin authorization\n",
			command_name);
		exit(1);
	}
	setprivvec(additional_privs, SEC_ALLOWDACACCESS, SEC_FILESYS,
#if SEC_MAC
			SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
			SEC_ALLOWNCAVACCESS,
#endif
			-1);
#endif /* SEC_BASE && !defined(SEC_STANDALONE) */

	if ((progname = rindex(*argv, '/') + 1) == (char *)1)
		progname = *argv;
	if ( !strcmp(progname, "mfs") || !strcmp(progname, "mount_mfs") ) {
		Nflag++;
		mfs++;
	}
	argc--, argv++;
	while (argc > 0 && argv[0][0] == '-') {
		for (cp = &argv[0][1]; *cp; cp++)
			switch (*cp) {

			case 'F':
				if (!mfs)
					fatal("-F: unknown flag");
				if (argc < 1)
					fatal("-F: missing mount flags");
				argc--, argv++;
				mntflags = atoi(*argv);
				if (mntflags == 0)
					fatal("%s: bad mount flags", *argv);
				goto next;

			case 'N':
				Nflag++;
				break;

			case 'S':
				if (argc < 1)
					fatal("-S: missing sector size");
				argc--, argv++;
				sectorsize = atoi(*argv);
				if (sectorsize <= 0)
					fatal("%s: bad sector size", *argv);
				goto next;
#ifdef COMPAT
			case 'D':
				if (argc < 1)
					fatal("-D: missing disk type");
				argc--, argv++;
				disktype = *argv;
				goto next;
#endif

			case 'a':
				if (argc < 1)
					fatal("-a: missing max contiguous blocks\n");
				argc--, argv++;
				maxcontig = atoi(*argv);
				if (maxcontig <= 0)
					fatal("%s: bad max contiguous blocks\n",
						*argv);
				goto next;

			case 'b':
				if (argc < 1)
					fatal("-b: missing block size");
				argc--, argv++;
				bsize = atoi(*argv);
				goto next;

			case 'c':
				if (argc < 1)
					fatal("-c: missing cylinders/group");
				argc--, argv++;
				cpg = atoi(*argv);
				if (cpg <= 0)
					fatal("%s: bad cylinders/group", *argv);
				cpgflg++;
				goto next;

			case 'd':
				if (argc < 1)
					fatal("-d: missing rotational delay\n");
				argc--, argv++;
				rotdelay = atoi(*argv);
				if (rotdelay < 0)
					fatal("%s: bad rotational delay\n",
						*argv);
				goto next;

			case 'e':
				if (argc < 1)
					fatal("-e: missing blocks per file in a cyl group\n");
				argc--, argv++;
				maxbpg = atoi(*argv);
				if (maxbpg <= 0)
					fatal("%s: bad blocks per file in a cyl group\n",
						*argv);
				goto next;

			case 'f':
				if (argc < 1)
					fatal("-f: missing frag size");
				argc--, argv++;
				fsize = atoi(*argv);
				if (fsize <= 0)
					fatal("%s: bad frag size", *argv);
				goto next;

			case 'i':
				if (argc < 1)
					fatal("-i: missing bytes per inode\n");
				argc--, argv++;
				density = atoi(*argv);
				if (density <= 0)
					fatal("%s: bad bytes per inode\n",
						*argv);
				goto next;

			case 'j':
				if (argc < 1)
					fatal("%s: missing number of frag in indirectory block\n", *argv);
				argc--, argv++;
				fragindir = atoi(*argv);
				/*
				 * check and make sure fragindir is a
				 * power-of-two number
				 */
				if (!POWEROF2(fragindir))
					fatal("%s: not power-of-two number\n",
						*argv);
				goto next;

			case 'k':
				if (argc < 1)
					fatal("-k: missing track skew");
				argc--, argv++;
				trackskew = atoi(*argv);
				if (trackskew < 0)
					fatal("%s: bad track skew", *argv);
				goto next;

			case 'l':
				if (argc < 1)
					fatal("-l: missing interleave");
				argc--, argv++;
				interleave = atoi(*argv);
				if (interleave <= 0)
					fatal("%s: bad interleave", *argv);
				goto next;

			case 'm':
				if (argc < 1)
					fatal("-m: missing free space %%\n");
				argc--, argv++;
				minfree = atoi(*argv);
				if (minfree < 0 || minfree > 99)
					fatal("%s: bad free space %%\n",
						*argv);
				goto next;

			case 'n':
				if (argc < 1)
					fatal("-n: missing rotational layout count\n");
				argc--, argv++;
				nrpos = atoi(*argv);
				if (nrpos <= 0)
					fatal("%s: bad rotational layout count\n",
						*argv);
				goto next;

			case 'o':
				if (argc < 1)
					fatal("-o: missing optimization preference");
				argc--, argv++;
				if (strcmp(*argv, "space") == 0)
					opt = FS_OPTSPACE;
				else if (strcmp(*argv, "time") == 0)
					opt = FS_OPTTIME;
				else
					fatal("%s: bad optimization preference %s",
					    *argv,
					    "(options are `space' or `time')");
				goto next;

			case 'p':
				if (argc < 1)
					fatal("-p: spare sectors per track");
				argc--, argv++;
				trackspares = atoi(*argv);
				if (trackspares < 0)
					fatal("%s: bad spare sectors per track", *argv);
				goto next;

			case 'P':
				if (argc < 1)
				  fatal ("-P: s5fs proto file name");
				argc--; argv++;
				s5fs_proto = *argv;
				goto next;

			case 'G':
				if (argc < 1)
				  fatal ("-G: gap (s5fs only)");
				argc--; argv++;
				s5fs_gap = atoi(*argv);
				s5fs_gap_string = *argv;
				goto next;

			case 'C':
				if (argc < 1)
				  fatal ("-C: blocks/cyl (s5fs only)");
				argc--; argv++;
				s5fs_blk_per_cyl = atoi(*argv);
				s5fs_blk_string = *argv;
				goto next;

			case 'I':
				if (argc < 1)
				  fatal ("-I: number of inodes (s5fs only)");
				argc--; argv++;
				s5fs_inodes = atoi(*argv);
				s5fs_inode_string = *argv;
				goto next;

			case 'r':
				if (argc < 1)
					fatal("-r: missing revs/minute\n");
				argc--, argv++;
				rpm = atoi(*argv);
				if (rpm <= 0)
					fatal("%s: bad revs/minute\n", *argv);
				goto next;

			case 's':
				if (argc < 1)
					fatal("-s: missing file system size");
				argc--, argv++;
				fssize = atoi(*argv);
				if (fssize <= 0)
					fatal("%s: bad file system size",
						*argv);
				goto next;

			case 't':
				if (argc < 1)
					fatal("-t: missing track total");
				argc--, argv++;
				ntracks = atoi(*argv);
				if (ntracks <= 0)
					fatal("%s: bad total tracks", *argv);
				goto next;

			case 'u':
				if (argc < 1)
					fatal("-u: missing sectors/track");
				argc--, argv++;
				nsectors = atoi(*argv);
				if (nsectors <= 0)
					fatal("%s: bad sectors/track", *argv);
				goto next;

			case 'x':
				if (argc < 1)
					fatal("-x: spare sectors per cylinder");
				argc--, argv++;
				cylspares = atoi(*argv);
				if (cylspares < 0)
					fatal("%s: bad spare sectors per cylinder", *argv);
				goto next;

			case 'T':
				if (argc < 1)
					fatal("-T: file system type [ufs|s5fs]");
				
				argc--, argv++;
				if ( !strcmp(*argv, "ufs"))
				      fstype = FS_BSDFFS;
				else if ( !strcmp(*argv, "s5fs"))
				      fstype = FS_SYSV;
				else
				         fatal("%s: bad file system type", *argv);
				goto next;
#if SEC_FSCHANGE
			case 'L':
				switch (cp[1]) {
#if SEC_ACL
				case 'A':
					if (argc < 1)
						fatal("-LA specified but no ACL Label\n");
					argc--, argv++;
					acl_label = *argv;
					mkfs_extended_fs = 1;
					goto next;
#endif
#if SEC_ILB
				case 'I':
					if (argc < 1)
						fatal("-LI specified but no Information Label\n");
					argc--, argv++;
					ilb_label = *argv;
					mkfs_extended_fs = 1;
					goto next;
#endif
#if SEC_MAC
				case 'S':
					if (argc < 1)
						fatal("-LS specified but no Sensitivity Label\n");
					argc--, argv++;
					mand_label = *argv;
					mkfs_extended_fs = 1;
					goto next;
#endif
				case 'U':
					untagged = 1;
					goto next;
				default:
					fatal("-L%c: unknown label option\n",
						cp[1]);
				}
#endif	SEC_FSCHANGE
				  
			default:
				fatal("-%c: unknown flag", *cp);
			}
next:
		argc--, argv++;
	}
#if SEC_FSCHANGE
	if (mkfs_extended_fs && untagged)
		fatal("-LU option is illegal when labels are specified");

	if (!mkfs_extended_fs && !untagged) {
#ifndef SEC_STANDALONE
#if SEC_MAC
		/*
		 * Must specify a sensitivity label when
		 * building tagged filesystem.
		 */
		if (mand_label == (char *) 0)
			fatal("sensitivity label required for extended filesystem");
#endif
#if SEC_ILB
		/*
		 * Print warning if no information label specified
		 */
		if (ilb_label == (char *) 0)
			fprintf(stderr,
				"WARNING: using WILDCARD information label\n");
#endif
#endif /* !SEC_STANDALONE */
		/* Default to tagged format */
		mkfs_extended_fs = 1;
	}
#endif /* SEC_FSCHANGE */
	if (argc < 1) {
		if (mfs)
			fprintf(stderr,
			    "usage: mfs [ fsoptions ] special-device %s\n",
			    "mount-point");
		else
#ifdef COMPAT
			fprintf(stderr, "usage: %s\n",
			    "newfs [ fsoptions ] special-device [device-type]");
#else
			fprintf(stderr,
			    "usage: newfs [ fsoptions ] special-device\n");
#endif
		fprintf(stderr, "where fsoptions are:\n");
		fprintf(stderr, "\t-N do not create file system, %s\n",
			"just print out parameters");
#ifdef COMPAT
		fprintf(stderr, "\t-D disktype\n");
#endif
		fprintf(stderr, "\t-a maximum contiguous blocks\n");
		fprintf(stderr, "\t-b block size\n");
		fprintf(stderr, "\t-c cylinders/group\n");
		fprintf(stderr, "\t-C blocks/cyl (s5fs only)\n");
		fprintf(stderr, "\t-d rotational delay between %s\n",
			"contiguous blocks");
		fprintf(stderr, "\t-e maximum blocks per file in a %s\n",
			"cylinder group");
		fprintf(stderr, "\t-f frag size\n");
		if (mfs)
			fprintf(stderr, "\t-F mount flags\n");
		fprintf(stderr, "\t-G gap (s5fs only)\n");
		fprintf(stderr, "\t-i number of bytes per inode\n");
		fprintf(stderr, "\t-I number of inodes (s5fs only)\n");
		fprintf(stderr, "\t-j number of frags in an indirect block\n");
		fprintf(stderr, "\t-k sector 0 skew, per track\n");
		fprintf(stderr, "\t-l hardware sector interleave\n");
		fprintf(stderr, "\t-m minimum free space %%\n");
		fprintf(stderr, "\t-n number of distinguished %s\n",
			"rotational positions");
		fprintf(stderr, "\t-o optimization preference %s\n",
			"(`space' or `time')");
		fprintf(stderr, "\t-p spare sectors per track\n");
		fprintf(stderr, "\t-P proto file name (s5fs only)\n");
		fprintf(stderr, "\t-r revolutions/minute\n");
		fprintf(stderr, "\t-s file system size (sectors)\n");
		fprintf(stderr, "\t-S sector size\n");
		fprintf(stderr, "\t-t tracks/cylinder\n");
		fprintf(stderr, "\t-T file system type [ufs|s5fs]\n");
		fprintf(stderr, "\t-u sectors/track\n");
		fprintf(stderr, "\t-x spare sectors per cylinder\n");
#if SEC_ACL
		fprintf(stderr, "\t-LA ACL label\n");
#endif
#if SEC_ILB
		fprintf(stderr, "\t-LI Information label\n");
#endif
#if SEC_MAC
		fprintf(stderr, "\t-LS Sensitivity label\n");
#endif
#if SEC_FSCHANGE
		fprintf(stderr, "\t-LU make unextended filesystem\n");
#endif
		exit(1);
	}
	/* Locate the appropriate file/device to create the filesystem
	 * on. The user may specify:
	 *  (1) a simple device name of the block device, to be
	 *	located in /dev. The character device name will be
	 *	be formed by inserting an 'r' in front of the name.
	 *  (2) a simple device name of the character device, to be
	 *	located in /dev.
	 *  (3) an absolute or relative pathname (including /'s) of
	 *	the block device. A character device name will be
	 *	be formed by inserting an 'r' in front of the final
	 * 	pathname component.
	 *  (4) an absolute or relative pathname of the character
	 *	device.
	 *  (5) an absolute or relative pathname of an ordinary file.
	 *	The file must be pre-existing.
	 */
	special = argv[0];
	cp = rindex(special, '/');
	if (cp == 0) {
		/* The argument is either case (1) or (2) above. */
		/* Check for device in /dev/ */
		(void)sprintf(device, "%s%s", _PATH_DEV, special);
		if (stat(device, &st) < 0)
			fatal("cannot find %s", device);
		if ((st.st_mode & S_IFMT) == S_IFBLK) {
			/* This is a block device, case (1), form the
			 * character device name. */
			(void)sprintf(device, "%sr%s", _PATH_DEV, special);
		}
		/* special is now the path to an allegedly character device */
		special = device;
	} else {
		/* The argument is either case (3), (4), or (5) */
		if (stat(special, &st) < 0)
			fatal("cannot find %s", special);
		if ((st.st_mode & S_IFMT) == S_IFBLK) {
			/* This is a block device, case (3), form the
			 * character device name. */
			(void)strncpy(dev_path, special, (cp+1) - special);
			(void)sprintf(device, "%sr%s", dev_path, cp + 1);
			special = device;
		}
		/* special is now the path to an allegedly character
		 * device, or an existing ordinary file. */
	}

#if SEC_BASE && !defined(SEC_STANDALONE)
	if (forceprivs(additional_privs, saveprivs))
		fatal("%s: insufficient privileges", command_name);
#endif
	if (!Nflag) {
		fso = open(special, O_WRONLY);
		if (fso < 0) {
			perror(special);
			exit(2);
		}
	} else
		fso = -1;
	fsi = open(special, O_RDONLY);
	if (fsi < 0) {
		perror(special);
		exit(3);
	}
	if (fstat(fsi, &st) < 0) {
		int err = errno;
		fprintf(stderr, "%s: ", progname);
		errno = err;
		perror(special);
		exit(4);
	}
#if SEC_BASE && !defined(SEC_STANDALONE)
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	if ((st.st_mode & S_IFMT) == S_IFCHR) {

		cp = index(argv[0], '\0') - 1;
		if (cp == 0 ||
#if	defined(multimax) || defined(i386)
			(*cp < 'a' || *cp > 'p')
#else
			(*cp < 'a' || *cp > 'o') 
#endif
			&& !isdigit(*cp))
			fatal("%s: can't figure out file system partition",
					argv[0]);
		partno = isdigit(*cp) ? *cp - '0' : *cp - 'a'; 

#ifdef	multimax
		getheaderinfo(special, partno);
#else
#ifdef COMPAT
		if (!mfs && disktype == NULL)
			disktype = argv[1];
#endif
		lp = getdisklabel(special, fsi);

#ifdef	DEBUG 
		dumplabel(lp);
#endif 
		if (lp) {
			if (partno >= lp->d_npartitions) {
				fatal("%s : only %d partitions", argv[0],
					lp->d_npartitions);
			}
			pp = &lp->d_partitions[partno];
			/*
			 * Ignore some partition info if types don't match 
			 */
			if (fstype == pp->p_fstype)
				trust_partition = 1;
		}
#ifndef COMPAT
		else {
			fatal("%s: can't read disk label", argv[0]);
		}
#endif	/* COMPAT */
#endif /* multimax */
	} else if ((st.st_mode & S_IFMT) != S_IFREG)
		fatal("%s: bad file type", argv[0]);

	if (fssize == 0) {
		if (pp) {
			if (pp->p_size == 0)
				fatal("%s: `%c' partition is unavailable",
					argv[0], *cp);
		} else {
#ifdef COMPAT
			if (unlabelled && disktype)
		fatal("%s: disk is unlabeled and no entry found in disktab",
				argv[0]);
			else if (unlabelled) {
		fatal("%s: can't read disk label; disk type must be specified",
				argv[0]);
		}
#endif
			fatal("%s: must specify file system size", argv[0]);
		}
		fssize = pp->p_size;
	}
	if (pp && (fssize > pp->p_size) && !mfs)
	       fatal("%s: maximum file system size on the `%c' partition is %d",
			argv[0], *cp, pp->p_size);

#ifdef DEBUG_S5FS
	printf("making fstype %s\n",fstypenames[fstype]);
#endif DEBUG_S5FS
	if (rpm == 0) {
		if (lp) rpm = lp->d_rpm;
		if (rpm <= 0)
			rpm = 3600;
	}
	if (ntracks == 0) {
		if (lp) ntracks = lp->d_ntracks;
		if (ntracks <= 0)
			fatal("%s: no default #tracks", argv[0]);
	}
	if (nsectors == 0) {
		if (lp) nsectors = lp->d_nsectors;
		if (nsectors <= 0)
			fatal("%s: no default #sectors/track", argv[0]);
	}
	if (sectorsize == 0) {
		if (lp) sectorsize = lp->d_secsize;
		else sectorsize = DISK_GRANULE;
		if (sectorsize <= 0)
			fatal("%s: no default sector size", argv[0]);
	}
	if (trackskew == -1) {
		if (lp) trackskew = lp->d_trackskew;
		if (trackskew < 0)
			trackskew = 0;
	}
	if (interleave == 0) {
		if (lp) interleave = lp->d_interleave;
		if (interleave <= 0)
			interleave = 1;
	}
	if (fsize == 0) {
		if (trust_partition) 
			fsize = pp->p_fsize;
		if (fsize <= 0)
			fsize = MAX(DFL_FRAGSIZE, lp?lp->d_secsize:0);
	}
	if (bsize == 0) {
	        if (fstype == FS_SYSV) {
			if (trust_partition)
				bsize = pp->p_fsize;
			if (bsize <= 0)
				bsize = DFL_S5BLKSIZE;
		} else {	/* must be FS_BSDFFS */
			if (trust_partition) 
				bsize = pp->p_frag * pp->p_fsize;
			if (bsize <= 0)
				bsize = MIN(DFL_BLKSIZE, 8 * fsize);
		}
	}
	if (fstype == FS_BSDFFS && bsize < MINBSIZE)
		fatal("%s: bad block size", *argv);
	if (density == 0)
		density = NFPI * fsize;
	if (minfree < 10 && opt != FS_OPTSPACE) {
		fprintf(stderr, "Warning: changing optimization to space ");
		fprintf(stderr, "because minfree is less than 10%%\n");
		opt = FS_OPTSPACE;
	}
	if (trackspares == -1) {
		if (lp) trackspares = lp->d_sparespertrack;
		if (trackspares < 0)
			trackspares = 0;
	}
	nphyssectors = nsectors + trackspares;
	if (cylspares == -1) {
		if(lp) cylspares = lp->d_sparespercyl;
		if (cylspares < 0)
			cylspares = 0;
	}
	secpercyl = nsectors * ntracks - cylspares;
	if (lp && (secpercyl != lp->d_secpercyl))
		fprintf(stderr, "%s (%d) %s (%d)\n",
			"Warning: calculated sectors per cylinder", secpercyl,
			"disagrees with disk label", lp->d_secpercyl);
	if (maxbpg == 0)
		maxbpg = MAXBLKPG(bsize);
	if (lp) {
		headswitch = lp->d_headswitch;
		trackseek = lp->d_trkseek;
		bbsize = lp->d_bbsize;
		sbsize = lp->d_sbsize;
		oldpartition = *pp;
	}
	/*
	 * Allocate some memory to be used to hold the
	 * superblock.
	 */
	fs_ptr = (struct fs *)malloc(sbsize);
	if (fs_ptr == NULL) {
		fatal("Unable to allocate fs_ptr");
	}
	sboff = BBOFF + bbsize;
#ifdef tahoe
	realsectorsize = sectorsize;
	if (sectorsize != DEV_BSIZE) {		/* XXX */
		int secperblk = DEV_BSIZE / sectorsize;

		sectorsize = DEV_BSIZE;
		nsectors /= secperblk;
		nphyssectors /= secperblk;
		secpercyl /= secperblk;
		fssize /= secperblk;
		pp->p_size /= secperblk;
	}
#endif
	if (fstype == FS_BSDFFS) {
		mkfs((pp ? pp : &oldpartition), special, fsi, fso);
#ifdef tahoe
		if (realsectorsize != DEV_BSIZE)
			pp->p_size *= DEV_BSIZE / realsectorsize;
#endif
	} else {        /* System V file system */
		if (s5fs_proto == NULL) {
			itoa(bsize,bsize_string);
			itoa(fssize, fssize_string);
			s5mkfs_arg = (char *) 
					(strcat (strcat(bsize_string, ":"), 
						fssize_string));
			if (s5fs_inodes != 0)
			s5mkfs_arg = (char *) 
					(strcat (strcat(s5mkfs_arg, ":"), 
						s5fs_inode_string));
		} else 
			s5mkfs_arg = s5fs_proto;
		argv[0] = "/sbin/s5mkfs";
		argv[1] = special;
		argv[2] = s5mkfs_arg;
		argv[3] = s5fs_gap_string;

		if (s5fs_blk_per_cyl)
		            argv[4] = s5fs_blk_string;
		else 
			argv[4] = (char * ) 0;
	        argv[5] = (char *) 0;

		if ((s5mkfs_pid = fork()) == 0) {	/* perform the mkfs */
			execvp ("/sbin/s5mkfs", argv);
			fatal("Returned from execl");
		} else {		/* wait for s5mkfs to finish */
			wait(&stat_loc);
			if (WIFEXITED(stat_loc)) {
				if ((error = WEXITSTATUS(stat_loc)) != 0){
				     fatal("/sbin/s5mkfs returned %d\n",error);
				} else if (pp) {
					pp->p_fstype = FS_SYSV;
					pp->p_fsize = bsize;
				}
			} else
				fatal ("%s: /sbin/s5mkfs unexpected termination\n",argv[0]);
		}
	}             /* System V file system */
#ifndef	multimax
	if (!Nflag && pp && bcmp(pp, &oldpartition, sizeof(oldpartition)))
		rewritelabel(special, fso, lp);
#endif
	if (!Nflag)
		close(fso);
	close(fsi);
#if SEC_ARCH && !defined(SEC_STANDALONE)
	{
		/*
		 * Run the setfstag program if any policies specify labels
		 * Cannot be run in stand-alone mode because there are no
		 * daemons.
		 */

		int have_labels = 0;
#if SEC_ACL
		if (acl_label)
			have_labels++;
#endif
#if SEC_MAC
		if (mand_label)
			have_labels++;
#endif
#if SEC_ILB
		if (ilb_label) 
			have_labels++;
#endif
		if (have_labels) {
		        cp = rindex(special, '/');
                        if (cp != 0) {
			    (void)strncpy(dev_path, special, (cp+1) - special);
			    (void)sprintf(device, "%s%s", dev_path, cp + 2);
			  }
			setfstag(device);
		      }
	}
#endif /* SEC_ARCH */

	if (mfs) {
		sprintf(buf, "mfs:%d", getpid());
		args.name = buf;
		args.base = membase;
		args.size = fssize * sectorsize;
		if (mount(MOUNT_MFS, argv[1], mntflags, &args) < 0) {
			perror("mfs: mount");
			exit(5);
		}
	}
	exit(0);
}
#ifndef	multimax

struct disklabel *
getdisklabel(s, fd)
	char *s;
	int fd;
{
	static struct disklabel lab;

	if (ioctl(fd, DIOCGDINFO, (char *)&lab) < 0) {
#ifdef i386
		if (get_disk_parms(fd, &lab)) {
			unlabelled++;
			return(&lab);
		}
#endif
#ifdef COMPAT
		unlabelled++;
		if (disktype) {
			struct disklabel *getdiskbyname();

			return (getdiskbyname(disktype));
		}
#endif
		return(NULL);
	}
	return (&lab);
}

rewritelabel(s, fd, lp)
	char *s;
	int fd;
	register struct disklabel *lp;
{
#if SEC_BASE && !defined(SEC_STANDALONE)
	privvec_t saveprivs;
#endif

#if	defined(COMPAT) || defined(i386)
	if (unlabelled)
		return;
#endif
	lp->d_checksum = 0;
	lp->d_checksum = dkcksum(lp);
#if SEC_BASE && !defined(SEC_STANDALONE)
	forceprivs(additional_privs, saveprivs);
#endif
	if (ioctl(fd, DIOCWDINFO, (char *)lp) < 0) {
		perror("ioctl (WDINFO)");
		fatal("%s: can't rewrite disk label", s);
	}
#if vax
	if (lp->d_type == DTYPE_SMD && lp->d_flags & D_BADSECT) {
		register i;
		int cfd;
		daddr_t alt;
		char specname[64];
		char blk[1024];
		char *cp;

		/*
		 * Make name for 'c' partition.
		 */
		strcpy(specname, s);
		cp = specname + strlen(specname) - 1;
		if (!isdigit(*cp))
			*cp = 'c';
		cfd = open(specname, O_WRONLY);
		if (cfd < 0) {
			perror(specname);
			exit(6);
		}
		bzero(blk, sizeof(blk));
		*(struct disklabel *)(blk + LABELOFFSET) = *lp;
		alt = lp->d_ncylinders * lp->d_secpercyl - lp->d_nsectors;
		for (i = 1; i < 11 && i < lp->d_nsectors; i += 2) {
			if (lseek(cfd, (off_t)(alt + i) * lp->d_secsize, L_SET) == -1) {
				perror("lseek to badsector area");
				exit(7);
			}
			if (write(cfd, blk, lp->d_secsize) < lp->d_secsize) {
				int oerrno = errno;
				fprintf(stderr, "alternate label %d ", i/2);
				errno = oerrno;
				perror("write");
			}
		}
		close(cfd);
	}
#endif
#if SEC_BASE && !defined(SEC_STANDALONE)
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
}
#endif	/* multimax */

/*VARARGS*/
fatal(fmt, arg1, arg2)
	char *fmt;
{

	fprintf(stderr, "%s: ", progname);
	fprintf(stderr, fmt, arg1, arg2);
	putc('\n', stderr);
	exit(8);
}

#ifdef	multimax

#define BUFSIZE		((sizeof(layout_t) + DEV_BSIZE - 1) & ~(DEV_BSIZE -1))

char	layout_info[BUFSIZE];
char	headerdev[MAXPATHLEN];

getheaderinfo(rdev, pnum)
	char *rdev;
	int   pnum;
{
#if SEC_BASE && !defined(SEC_STANDALONE)
	privvec_t saveprivs;
#endif

	register layout_t 	*layout=(layout_t *)&layout_info[0];
				/* Pointer to the disk header layout */
	char *headerptr, *cp, *tcp; 
	int fd, err, p_size;
	
	/* 
	 * open the header parition of this disk. The 4th partition of 
	 * this disk, ie., rmdxd, will be used as header diskpartition. 
	 */
	headerptr = &headerdev[0];
	strcpy(headerptr, rdev);

	tcp = index(headerptr, '\0') - 1;
	cp = tcp; 

	if (isdigit(*tcp))
		*tcp ='3';
	else
		*tcp = 'd';
#if SEC_BASE && !defined(SEC_STANDALONE)
	forceprivs(additional_privs, saveprivs);
#endif
	fd = open(headerptr, O_RDONLY);   
	if (fd < 0) {
		if (fssize == 0) {
			perror(headerptr);
			exit(1);
		}
		return;
	}
#if SEC_BASE && !defined(SEC_STANDALONE)
	seteffprivs(saveprivs, (priv_t *) 0);
#endif

	if ((err = ioctl(fd, (int)MSIOCRDLAY, (char *)layout)) == -1) {
		if (fssize == 0) {
			err = errno;
			perror("READ HEADER PARTITION ERROR");
			exit(1);
		}
		return;
	}

	if ((p_size = layout->partitions[pnum].part_size) == 0)
		fatal("%s: `%c' partition is unavailable", rdev, *cp);
	if (fssize == 0)
		fssize = p_size;
	if (fssize > p_size && !mfs)
	       fatal("%s: maximum file system size on the `%c' partition is %d",
			rdev, *cp, p_size);
	/* 
	 * NOTE: Multimax disk header does not provide the following info: 
	 *  - rpm
	 *  - trackskew
	 *  - interleave
	 *  - headswitch
	 *  - trackseek
	 *  - cylspares
	 *  - trackspares
	 *  - secpercyl
	 * These values are all defaulted by the caller of this
	 * routine if not provided by the user.
	 */
	if (ntracks == 0) {
		ntracks = layout->lay_geom.track_cylinder;
		if (ntracks <= 0)
			fatal("%s: no default #tracks", rdev);
	}

	if (nsectors == 0) {
		nsectors = layout->lay_geom.sector_track;
		if (nsectors <= 0)
			fatal("%s: no default #sectors/track", rdev);
	}

	if (sectorsize == 0) {
		sectorsize = layout->lay_geom.byte_sector;
		if (sectorsize <= 0)
			fatal("%s: no default sector size", rdev);
	}

	if (fsize == 0) 
		fsize = MAX(DFL_FRAGSIZE, layout->lay_geom.byte_sector);

	close(fd);
}

#endif	/* multimax */

#ifdef i386 /* really should be more specific than this */

/* The following #define is due to an unfortunate namespace
 * collision between this disk driver and the disklabel code above.
 */
#define partition PaRtItIoN

#include <i386/disk.h>

#undef partition

int
get_disk_parms(fd, lp)
int fd;
struct disklabel *lp;
{
struct disk_parms disk_parms;

	if (ioctl(fd, V_GETPARMS, &disk_parms) < 0) {
		return(0);
	}
	/*
	 * Fake up the contents of a disk label so that the
	 * caller will be happy enough to create a filesystem.
	 */
	lp->d_magic	= DISKMAGIC;
	lp->d_type = (disk_parms.dp_type==DPT_WINI)?DTYPE_ESDI:DTYPE_FLOPPY;
	lp->d_subtype	= 0;	/* no clue */
	strcpy(dktypenames, lp->d_typename);
	bzero(lp->d_packname, 16);
	lp->d_secsize	= disk_parms.dp_secsiz;	/* size of a sector (bytes) */
	lp->d_nsectors	= disk_parms.dp_sectors;/* sectors per track */
	lp->d_ntracks	= disk_parms.dp_heads;	/* tracks per cylinder */
	lp->d_ncylinders = disk_parms.dp_cyls;	/* total # of cylinders */
	lp->d_secpercyl = lp->d_nsectors * lp->d_ntracks;
	lp->d_secperunit = 0;			/* will be computed */
	lp->d_sparespertrack = 0;
	lp->d_sparespercyl = 0;
	lp->d_acylinders = 0;
	lp->d_rpm	= 0;				/* will be defaulted */
	lp->d_interleave = 1;	 /* This disk doesn't provide this info */
	lp->d_cylskew	= 0;	 /* This disk doesn't provide this info */
	lp->d_headswitch = 0;	 /* This disk doesn't provide this info */
	lp->d_trkseek	= 0;	 /* This disk doesn't provide this info */
	lp->d_flags = (lp->d_type==DTYPE_FLOPPY)?D_REMOVABLE:0;
	bzero(lp->d_drivedata, sizeof(u_long) * NDDATA);
	bzero(lp->d_spare, sizeof(u_long) * NSPARE);
	lp->d_magic2 = DISKMAGIC;
	lp->d_checksum = 0;

	lp->d_npartitions = 1;
	lp->d_bbsize = BBSIZE;	/* meaningless on this disk */
	lp->d_sbsize = SBSIZE;
	lp->d_partitions[0].p_size = disk_parms.dp_pnumsec;
	lp->d_partitions[0].p_offset = 0;	/* always */
	lp->d_partitions[0].p_fsize = 0; /* default or set from argv */
	lp->d_partitions[0].p_fstype = FS_UNUSED;/* we just don't know */
	lp->d_partitions[0].p_frag = 0;	/* will be overridden */
	lp->d_partitions[0].p_cpg = 0;	/* unused anyway */

	partno = 0;

	return(1);
}
#endif	/* i386 */

#ifdef	DEBUG
dumplabel(lp)
register struct disklabel *lp;
{
register struct partition *pptr;
register int i;

	printf("d_magic: %x\n", lp->d_magic);
	printf("d_type: %d\n", lp->d_type);
	printf("d_subtype: %d\n", lp->d_subtype);
	printf("d_typename: %16s\n", lp->d_typename);
	printf("d_secsize: %d\n", lp->d_secsize);
	printf("d_nsectors: %d\n", lp->d_nsectors);
	printf("d_ntracks: %d\n", lp->d_ntracks);
	printf("d_ncylinders: %d\n", lp->d_ncylinders);
	printf("d_secpercyl: %d\n", lp->d_secpercyl);
	printf("d_secperunit: %d\n", lp->d_secperunit);
	printf("d_rpm: %d\n", lp->d_rpm);
	printf("d_npartitions: %d\n", lp->d_npartitions);
	for (i=0; i< lp->d_npartitions; i++) {
		pptr = &(lp->d_partitions[i]);
		printf("partition %d\n", i);
		printf("        p_size %d\n", pptr->p_size);
		printf("        p_offset %d\n", pptr->p_offset);
		printf("        p_fsize %d\n", pptr->p_fsize);
		printf("        p_fstype %d\n", pptr->p_fstype);
		printf("        p_frag %d\n", pptr->p_frag);
		printf("        p_cpg %d\n", pptr->p_cpg);
		printf("\n");
	}
}
#endif	/* DEBUG */

char * itoa (x,itoa_string)
int x;
char itoa_string[];
{
  char tmp[30];
  int y=0;
  int z=0;
  int foo;

  for (; tmp[y++] = x % 10, (x = x/10) > 0; );

  while (--y >= 0)
    itoa_string[z++] = tmp[y] + '0';

  itoa_string[z] = 0;

  return (itoa_string);
}
