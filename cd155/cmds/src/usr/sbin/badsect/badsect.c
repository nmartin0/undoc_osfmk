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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983 Regents of the University of California.\n\
 All rights reserved.\n";
#endif not lint

#ifndef lint
static char sccsid[] = "@(#)badsect.c	5.4 (Berkeley) 5/31/88";
#endif not lint

/*
 * badsect
 *
 * Badsect takes a list of file-system relative sector numbers
 * and makes files containing the blocks of which these sectors are a part.
 * It can be used to contain sectors which have problems if these sectors
 * are not part of the bad file for the pack (see bad144).  For instance,
 * this program can be used if the driver for the file system in question
 * does not support bad block forwarding.
 */
#include <stdio.h>
#include <sys/param.h>
#if	BSD > 43
#include <ufs/fs.h>
#else
#include <sys/fs.h>
#endif
#include <sys/dir.h>
#include <sys/stat.h>
#if	BSD > 43
#include <sys/time.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#else
#include <sys/inode.h>
#endif
#include  <sys/file.h>
#include  <sys/ioctl.h>
#include  <sys/disklabel.h>

struct fs *sblock_ptr=NULL;
int sbsize,sboff,mach_record_size;
#define sblock	(*sblock_ptr)
union {
	struct	cg cg;
	char	cgx[MAXBSIZE];
} ucg;
#define	acg	ucg.cg
struct	fs *fs;
int	fso, fsi;
int	errs;
long	dev_bsize = 1;

char buf[MAXBSIZE];


main(argc, argv)
	int argc;
	char *argv[];
{
	daddr_t number;
	struct stat stbuf, devstat;
	register struct dirent *dp;
	DIR *dirp;
	int fd;
	char name[BUFSIZ];

	if (argc < 3) {
		fprintf(stderr, "usage: badsect bbdir blkno [ blkno ]\n");
		exit(1);
	}
	if (chdir(argv[1]) < 0 || stat(".", &stbuf) < 0) {
		perror(argv[1]);
		exit(2);
	}
	strcpy(name, "/dev/");
	if ((dirp = opendir(name)) == NULL) {
		perror(name);
		exit(3);
	}
	while ((dp = readdir(dirp)) != NULL) {
		strcpy(&name[5], dp->d_name);
		if (stat(name, &devstat) < 0) {
			perror(name);
			exit(4);
		}
		if (stbuf.st_dev == devstat.st_rdev &&
		    (devstat.st_mode & IFMT) == IFBLK)
			break;
	}
	closedir(dirp);
	if (dp == NULL) {
		printf("Cannot find dev 0%o corresponding to %s\n",
			stbuf.st_rdev, argv[1]);
		exit(5);
	}
	if ((fsi = open(name, 0)) < 0) {
		perror(name);
		exit(6);
	}

        if (ioctl(fsi, DIOMRINFO, &mach_record_size) < 0) {
                perror("ioctl DIOMRINFO");
		exit(7);
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
		exit(8);
	}
	fs = sblock_ptr;
	rdfs(sboff, sbsize, (char *)fs);
	dev_bsize = fs->fs_fsize / fsbtodb(fs, 1);
	for (argc -= 2, argv += 2; argc > 0; argc--, argv++) {
		number = atoi(*argv);
		if (chkuse(number, 1))
			continue;
		if (mknod(*argv, IFMT|0600, dbtofsb(fs, number)) < 0) {
			perror(*argv);
			errs++;
		}
	}
	printf("Don't forget to run ``fsck %s''\n", name);
	if (sblock_ptr) {
		free(sblock_ptr);
	}
	exit(errs);
}

chkuse(blkno, cnt)
	daddr_t blkno;
	int cnt;
{
	int cg;
	daddr_t fsbn, bn;

	fsbn = dbtofsb(fs, blkno);
	if ((unsigned)(fsbn+cnt) > fs->fs_size) {
		printf("block %d out of range of file system\n", blkno);
		return (1);
	}
	cg = dtog(fs, fsbn);
	if (fsbn < cgdmin(fs, cg)) {
		if (cg == 0 || (fsbn+cnt) > cgsblock(fs, cg)) {
			printf("block %d in non-data area: cannot attach\n",
				blkno);
			return (1);
		}
	} else {
		if ((fsbn+cnt) > cgbase(fs, cg+1)) {
			printf("block %d in non-data area: cannot attach\n",
				blkno);
			return (1);
		}
	}
	rdfs(fsbtodb(fs, cgtod(fs, cg)), (int)sblock.fs_cgsize,
	    (char *)&acg);
	if (!cg_chkmagic(&acg)) {
		fprintf(stderr, "cg %d: bad magic number\n", cg);
		errs++;
		return (1);
	}
	bn = dtogd(fs, fsbn);
	if (isclr(cg_blksfree(&acg), bn))
		printf("Warning: sector %d is in use\n", blkno);
	return (0);
}

/*
 * read a block from the file system
 */
rdfs(bno, size, bf)
	int bno, size;
	char *bf;
{
	int n;

	if (lseek(fsi, bno * dev_bsize, 0) < 0) {
		printf("seek error: %ld\n", bno);
		perror("rdfs");
		exit(1);
	}
	n = read(fsi, bf, size);
	if(n != size) {
		printf("read error: %ld\n", bno);
		perror("rdfs");
		exit(1);
	}
}
