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
 * Copyright (c) 1980 The Regents of the University of California.
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

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1980 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)df.c	5.14 (Berkeley) 10/17/89";
#endif /* not lint */

/*
 * df
 */
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>
#include <sys/file.h>
#include <stdio.h>
#include <strings.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>
#ifdef COMPAT_43
#include <errno.h>
#define OLD_WIDTH	11
#endif
#ifdef NLS
#include <locale.h>
#endif

#ifdef MSG
#include "df_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_DF,n,s) 
#else
#define MSGSTR(n,s) s
#endif

#if defined(KJI) || defined(NLS)
#include <NLchar.h>
#include <NLctype.h>
#endif

char	*getmntpt();
int	iflag, kflag, nflag, tflag;
#ifdef COMPAT_43
int	oflag;
#endif /* COMPAT_43 */
int     fstype = MOUNT_NONE;
char    *fstypes[] = {
        /* MOUNT_NONE */
        "ufs",      /* MOUNT_UFS */
        "nfs",      /* MOUNT_NFS */
        "mfs",      /* MOUNT_MFS */
        "pc",       /* MOUNT_PC */
        "s5fs",     /* MOUNT_S5FS */
#ifdef PFS
	"pfs",      /* MOUNT_PFS */
#endif
};

        
main(argc, argv)
	int argc;
	char **argv;
{
	extern int errno, optind;
        extern char *optarg;
	int err, ch, i, pass;
	long width, maxwidth=OLD_WIDTH, mntsize, getmntinfo();
	char *mntpt, *mktemp(), **saved_argv;
	struct stat stbuf;
	struct statfs statfsbuf, *mntbuf;
	struct ufs_args mdev;

#ifdef NLS
	(void) setlocale( LC_ALL, "" );
#endif

#ifdef MSG
	catd = catopen(MF_DF,0);
#endif
	while ((ch = getopt(argc, argv, "ikont:")) != EOF)
		switch(ch) {
		case 'i':
			iflag = 1;
			break;
		case 'k':
			kflag = 1;
			break;
		case 'n':
			nflag = 1;
			break;
#ifdef COMPAT_43
		case 'o':
			oflag = 1;
			break;
#endif /* COMPAT_43 */
                case 't':
                        tflag = 1;
                        for (i = 0; i < MOUNT_MAXTYPE; i++)
                                if (strcmp(fstypes[i], optarg) == 0) {
#ifdef PFS
					if (strcmp(fstypes[i], "pfs") == 0) 
						fstype = MOUNT_PFS;
					else
#endif
                                        	fstype = i+1;
                                        break;
                                }
                        if (fstype == MOUNT_NONE) {
                                fprintf(stderr,
                                        MSGSTR(FTYPE, "df: %s: unknown file system type.\n"), optarg);
                                exit(1);
                        }
                        break;
		case '?':
		default:
			fprintf(stderr,
#ifdef PFS
			    MSGSTR(USAGE, "usage: df [-ikn] [-t nfs|ufs|s5fs|pfs] [file | file_system ...]\n"));
#else
			    MSGSTR(USAGE, "usage: df [-ikn] [-t nfs|ufs|s5fs] [file | file_system ...]\n"));
#endif
			exit(1);
		}
	argc -= optind;
	argv += optind;

#ifdef COMPAT_43
	if (oflag) {
		olddf(argv);
		exit(0);
	}
#endif /* COMPAT_43 */
	/*
	 * scan through to calculate the the maximum
	 * width.  If type specified, only use widths of that fs type.
	 */
	mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
	maxwidth = 0;
	for (i = 0; i < mntsize; i++) {
		if (!tflag || (tflag && mntbuf[i].f_type == fstype)) {
			width = strlen(mntbuf[i].f_mntfromname);
			if (width > maxwidth)
				maxwidth = width;
		}
	}
	if (!*argv) {
		/*
		 * scan through to calculate the the maximum
		 * width.  If type specified, only use widths of that fs type.
		 */
		mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
		maxwidth = 0;
		for (i = 0; i < mntsize; i++) {
			if (!tflag || (tflag && mntbuf[i].f_type == fstype)) {
				width = strlen(mntbuf[i].f_mntfromname);
				if (width > maxwidth)
					maxwidth = width;
			}
		}
		/*
		 * Print information for all mounted file systems, 
		 * discriminating by type if tflag specified.
		 * If -n specified, get possibly stale information, but
		 * don't hang
		 */
		mntsize = getmntinfo(&mntbuf, (nflag ? MNT_NOWAIT : MNT_WAIT));
		for (i = 0; i < mntsize; i++)
                        if (!tflag || (tflag && mntbuf[i].f_type == fstype))
                                prtstat(&mntbuf[i], maxwidth);
		exit(0);
	}

	pass = 0;
	maxwidth = 0;
	saved_argv = argv;
pass1:
	for (; *argv; argv++) {
		/*
		 * don't try to avoid hanging here, so don't even
		 * look at nflag.  We're doing the fs's one at a time
		 */
		if (stat(*argv, &stbuf) < 0) {
			err = errno;
			if ((mntpt = getmntpt(*argv)) == 0) {
				if (pass != 0) {
					errno = err;
					perror(*argv);
				}
				continue;
			}
		} else if ((stbuf.st_mode & S_IFMT) == S_IFBLK) {
			if ((mntpt = getmntpt(*argv)) == 0) {
				mntpt = mktemp("/df.XXXXXX");
				mdev.fspec = *argv;
				if (!mkdir(mntpt,0700) &&
				    !mount(MOUNT_UFS, mntpt, M_RDONLY, &mdev) &&
				    !statfs(mntpt, &statfsbuf)) {
					if (pass == 0) {
					    width = 
						strlen(statfsbuf.f_mntfromname);
					    if (width > maxwidth)
						maxwidth = width;
					} else {
					    statfsbuf.f_mntonname[0] = '\0';
					    prtstat(&statfsbuf, maxwidth);
					}
				} else if (pass != 0)
					perror(*argv);
				(void)umount(mntpt, MNT_NOFORCE);
				(void)rmdir(mntpt);
				continue;
			}
		} else
			mntpt = *argv;
		/*
		 * Statfs does not take a `wait' flag, so we cannot
		 * implement nflag here
		 */
		if (statfs(mntpt, &statfsbuf) < 0) {
			if (pass != 0)
				perror(mntpt);
			continue;
		}
		if (pass == 0) {
			width = strlen(statfsbuf.f_mntfromname);
			if (width > maxwidth)
				maxwidth = width;
		} else 
                	prtstat(&statfsbuf, maxwidth);
	}
	if (pass == 0) {
		pass++;
		argv = saved_argv;
		goto pass1;
	}
	exit(0);
}

char *
getmntpt(name)
	char *name;
{
	long mntsize, i;
	struct statfs *mntbuf;

	mntsize = getmntinfo(&mntbuf, 0);
	for (i = 0; i < mntsize; i++) {
		if (!strcmp(mntbuf[i].f_mntfromname, name))
			return (mntbuf[i].f_mntonname);
	}
	return (0);
}

/*
 * Print out status about a filesystem.
 */
prtstat(sfsp, maxwidth)
	register struct statfs *sfsp;
        long maxwidth;
{
	long used, availblks, inodes, tmp_bavail;
        static int timesthrough;
        int fsys_len = strlen(MSGSTR(FSYS, "Filesystem")) + 1;
        
	if (maxwidth < fsys_len)
		maxwidth = fsys_len;

        if (++timesthrough == 1) {
                printf(MSGSTR(MSG1, "%-*.*s%s    used   avail capacity"),
                       maxwidth, maxwidth, MSGSTR(FSYS, "Filesystem"),
                       kflag ? MSGSTR(KBYTES, "  kbytes") : MSGSTR(BLKS, "512-blks"));
                if (iflag)
                        printf(MSGSTR(IUSED, " iused   ifree  %%iused"));
                printf(MSGSTR(MOUNTED, "  Mounted on\n"));
        }
	printf("%-*.*s", maxwidth, maxwidth, sfsp->f_mntfromname);
	used = sfsp->f_blocks - sfsp->f_bfree;
	availblks = sfsp->f_bavail + used;
	tmp_bavail = sfsp->f_bavail;
	if (tmp_bavail < 0) {
		tmp_bavail = 0;
	        availblks = 0;
	}
	printf("%8ld%8ld%8ld",
	    sfsp->f_blocks * sfsp->f_fsize / (kflag ? 1024 : 512),
	    used * sfsp->f_fsize / (kflag ? 1024 : 512),
	    tmp_bavail * sfsp->f_fsize / (kflag ? 1024 : 512));
	printf("%6.0f%%",
	    availblks == 0 ? 100.0 : (double)used / (double)availblks * 100.0);
	if (iflag) {
		inodes = sfsp->f_files;
		used = inodes - sfsp->f_ffree;
		printf("%8ld%8ld%6.0f%% ", used, sfsp->f_ffree,
		   inodes == 0 ? 100.0 : (double)used / (double)inodes * 100.0);
	} else 
		printf("  ");
	printf("  %s\n", sfsp->f_mntonname);
}

#ifdef COMPAT_43
/*
 * This code constitutes the old df code for extracting
 * information from filesystem superblocks.
 */
#include <ufs/fs.h>
#include <fstab.h>

char	root[MAXPATHLEN];


struct fs *sblock_ptr=NULL;
#define sblock  (*sblock_ptr)

int	fi;
char	*strcpy();

olddf(argv)
	char *argv[];
{
	struct fstab *fsp;

	sync();
	if (!*argv) {
		if (setfsent() == 0)
			perror(_PATH_FSTAB), exit(1);
		while (fsp = getfsent()) {
			if (strcmp(fsp->fs_type, FSTAB_RW) &&
			    strcmp(fsp->fs_type, FSTAB_RO) &&
			    strcmp(fsp->fs_type, FSTAB_RQ))
				continue;
			if (root[0] == 0)
				(void) strcpy(root, fsp->fs_spec);
			dfree(fsp->fs_spec, 1);
		}
		(void)endfsent();
		exit(0);
	}
	while (*argv)
		dfree(*argv++, 0);
	exit(0);
}

dfree(file, infsent)
	char *file;
	int infsent;
{
	extern int errno;
	struct stat stbuf;
	struct statfs statfsbuf;
	register struct statfs *sfsp;
	struct fstab *fsp;
	char *mntpt;
	int sboff, sbsize, mach_record_size;

	if (stat(file, &stbuf) == 0 &&
	    (stbuf.st_mode&S_IFMT) != S_IFCHR &&
	    (stbuf.st_mode&S_IFMT) != S_IFBLK) {
		if (infsent) {
			fprintf(stderr, MSGSTR(SCREWY, "df: %s: screwy fstab entry\n"), file);
			return;
		}
		(void)setfsent();
		while (fsp = getfsent()) {
			struct stat stb;

			if (stat(fsp->fs_spec, &stb) == 0 &&
			    stb.st_rdev == stbuf.st_dev) {
				file = fsp->fs_spec;
				(void)endfsent();
				goto found;
			}
		}
		(void)endfsent();
		fprintf(stderr, MSGSTR(UNKDEV, "df: %s: mounted on unknown device\n"), file);
		return;
	}
found:
	if ((fi = open(file, O_RDONLY)) < 0) {
		perror(file);
		printf(MSGSTR(CANTOP, "Cant's open %s\n"), file);
		return;
	}

       if (ioctl(fi, DIOMRINFO, &mach_record_size) < 0) {
                perror("ioctl DIOMRINFO");
		return;
        }
        /*
         * Determine the sector size of the device to enable the correct
         * super block size to be used.
         */
        if (mach_record_size > SBSIZE) {
                sbsize = mach_record_size;
                sboff = BBOFF + sbsize;
        } else {
                sbsize = SBSIZE;
                sboff = SBOFF;
        }

        sblock_ptr = (struct fs *)malloc(sbsize);
        if (sblock_ptr == NULL ) {
                perror("Out of Memory");
		return;
        }

	if (bread((long)sboff, (char *)&sblock, sbsize) == 0) {
		(void) close(fi);
		return;
	}
	sfsp = &statfsbuf;
	sfsp->f_type = MOUNT_UFS;
	sfsp->f_flags = 0;
	sfsp->f_fsize = sblock.fs_fsize;
	sfsp->f_bsize = sblock.fs_bsize;
	sfsp->f_blocks = sblock.fs_dsize;
	sfsp->f_bfree = sblock.fs_cstotal.cs_nbfree * sblock.fs_frag +
		sblock.fs_cstotal.cs_nffree;
	sfsp->f_bavail = (sblock.fs_dsize * (100 - sblock.fs_minfree) / 100) -
		(sblock.fs_dsize - sfsp->f_bfree);
	if (sfsp->f_bavail < 0)
		sfsp->f_bavail = 0;
	sfsp->f_files =  sblock.fs_ncg * sblock.fs_ipg;
	sfsp->f_ffree = sblock.fs_cstotal.cs_nifree;
	sfsp->f_fsid.val[0] = 0;
	sfsp->f_fsid.val[1] = 0;
	if ((mntpt = getmntpt(file)) == 0)
		mntpt = "";
	bcopy((caddr_t)mntpt, (caddr_t)&sfsp->f_mntonname[0], MNAMELEN);
	bcopy((caddr_t)file, (caddr_t)&sfsp->f_mntfromname[0], MNAMELEN);
	prtstat(sfsp, OLD_WIDTH);
	(void) close(fi);
	if (sblock_ptr) {
		free(sblock_ptr);
	}	
}

long lseek();

bread(off, buf, cnt)
	long off;
	char *buf;
{
	int n;
	extern errno;

	(void) lseek(fi, off, SEEK_SET);
	if ((n=read(fi, buf, cnt)) != cnt) {
		/* probably a dismounted disk if errno == EIO */
		if (errno != EIO) {
			printf(MSGSTR(READERR, "\nread error off = %ld\n"), off);
			printf(MSGSTR(CERRNO, "count = %d; errno = %d\n"), n, errno);
		}
		return (0);
	}
	return (1);
}
#endif /* COMPAT_43 */
