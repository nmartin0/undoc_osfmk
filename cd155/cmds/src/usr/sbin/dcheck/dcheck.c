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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1980 Regents of the University of California.\n\
 All rights reserved.\n";
#endif not lint

#ifndef lint
static char sccsid[] = "@(#)dcheck.c	5.3 (Berkeley) 5/2/88";
#endif not lint

/*
 * dcheck - check directory consistency
 */
#define	NB	10
#define	MAXNINDIR	(MAXBSIZE / sizeof (daddr_t))

#include <sys/secdefines.h>

#include <sys/param.h>
#if	BSD > 43
#include <sys/time.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <ufs/fs.h>
#else
#include <sys/inode.h>
#include <sys/fs.h>
#endif
#include <sys/dir.h>
#include <stdio.h>
#include  <sys/ioctl.h>
#include  <sys/disklabel.h>

int sbsize, sboff, mach_record_size;
struct fs *sblock_ptr=NULL;
#define	sblock	(*sblock_ptr)

struct dirstuff {
	int loc;
	struct dinode *ip;
	char dbuf[MAXBSIZE];
};

struct	dinode	itab[MAXBSIZE / sizeof(struct dinode)];
struct	dinode	*gip;
ino_t	ilist[NB];

int	fi;
ino_t	ino;
ino_t	*ecount;
int	headpr;
int	nfiles;
long	dev_bsize = 1;

int	nerror;
daddr_t	bmap();
long	atol();
char	*malloc();

main(argc, argv)
char *argv[];
{
	register i;
	long n;

#if SEC_BASE
        set_auth_parameters(argc, argv);
        initprivs();

        if (!authorized_user("sysadmin")) {
                printf("dcheck: need sysadmin authorization\n");
                exit(1);
        }
#endif

	while (--argc) {
		argv++;
		if (**argv=='-')
		switch ((*argv)[1]) {

		case 'i':
			for(i=0; i<NB; i++) {
				n = atol(argv[1]);
				if(n == 0)
					break;
				ilist[i] = n;
				argv++;
				argc--;
			}
			ilist[i] = 0;
			continue;

		default:
			printf("Bad flag %c\n", (*argv)[1]);
			nerror++;
		}
		check(*argv);
	}
	return(nerror);
}

check(file)
char *file;
{
	register i, j, c;

	fi = open(file, 0);
	if(fi < 0) {
		printf("cannot open %s\n", file);
		nerror++;
		return;
	}
	headpr = 0;
	printf("%s:\n", file);
	sync();
        if (ioctl(fi, DIOMRINFO, &mach_record_size) < 0) {
                perror("ioctl DIOMRINFO");
		nerror++;
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
		nerror++;
                perror("Out of Memory");
		return;
	}


	bread(sboff, (char *)sblock_ptr, sbsize);
#if SEC_FSCHANGE
        if (sblock.fs_magic != FS_MAGIC && sblock.fs_magic != FS_SEC_MAGIC)
#else
	if (sblock.fs_magic != FS_MAGIC) 
#endif
        {
		printf("%s: not a file system\n", file);
		nerror++;
		return;
	}
#if SEC_FSCHANGE
        disk_set_file_system(&sblock, sblock.fs_bsize);
#endif
	dev_bsize = sblock.fs_fsize / fsbtodb(&sblock, 1);
	nfiles = sblock.fs_ipg * sblock.fs_ncg;
	ecount = (ino_t *)malloc((nfiles+1) * sizeof (*ecount));
	if (ecount == 0) {
		printf("%s: not enough core for %d files\n", file, nfiles);
		exit(04);
	}
	for (i = 0; i<=nfiles; i++)
		ecount[i] = 0;
	ino = 0;
	for (c = 0; c < sblock.fs_ncg; c++) {
		for (i = 0;
#if SEC_FSCHANGE
                     i / sblock.fs_frag * INOPB(&sblock) < sblock.fs_ipg;
#else
		     i < sblock.fs_ipg / INOPF(&sblock);
#endif
		     i += sblock.fs_frag) {
			bread(fsbtodb(&sblock, cgimin(&sblock, c) + i),
			    (char *)itab, sblock.fs_bsize);
			for (j = 0; j < INOPB(&sblock); j++) {
#if SEC_BASE
                                struct dinode *dip;

                                disk_inode_in_block(&sblock, (char *) itab,
                                                        &dip, ino);
                                pass1(dip);
#else
				pass1(&itab[j]);
#endif
				ino++;
			}
		}
	}
	ino = 0;
	for (c = 0; c < sblock.fs_ncg; c++) {
		for (i = 0;
#if SEC_FSCHANGE
                     i / sblock.fs_frag * INOPB(&sblock) < sblock.fs_ipg;
#else
		     i < sblock.fs_ipg / INOPF(&sblock);
#endif
		     i += sblock.fs_frag) {
			bread(fsbtodb(&sblock, cgimin(&sblock, c) + i),
			    (char *)itab, sblock.fs_bsize);
			for (j = 0; j < INOPB(&sblock); j++) {
#if SEC_BASE
                                struct dinode *dip;

                                disk_inode_in_block(&sblock, (char *) itab,
                                                        &dip, ino);
                                pass2(dip);
#else
				pass2(&itab[j]);
#endif
				ino++;
			}
		}
	}
	free(ecount);
	if (sblock_ptr) {
		free(sblock_ptr);
	}
}
 
pass1(ip)
	register struct dinode *ip;
{
	register struct dirent *dp;
	struct dirstuff dirp;
	int k;
	struct dirent *dcheck_readdir();

	if((ip->di_mode&IFMT) != IFDIR)
		return;
	dirp.loc = 0;
	dirp.ip = ip;
	gip = ip;
	for (dp = dcheck_readdir(&dirp); dp != NULL; dp = dcheck_readdir(&dirp)) {
		if(dp->d_fileno == 0)
			continue;
		if(dp->d_fileno > nfiles || dp->d_fileno < ROOTINO) {
			printf("%d bad; %d/%s\n",
			    dp->d_fileno, ino, dp->d_name);
			nerror++;
			continue;
		}
		for (k = 0; ilist[k] != 0; k++)
			if (ilist[k] == dp->d_fileno) {
				printf("%d arg; %d/%s\n",
				     dp->d_fileno, ino, dp->d_name);
				nerror++;
			}
		ecount[dp->d_fileno]++;
	}
}

pass2(ip)
register struct dinode *ip;
{
	register i;

	i = ino;
	if ((ip->di_mode&IFMT)==0 && ecount[i]==0)
		return;
	if (ip->di_nlink==ecount[i] && ip->di_nlink!=0)
		return;
	if (headpr==0) {
		printf("     entries  link cnt\n");
		headpr++;
	}
	printf("%u\t%d\t%d\n", ino,
	    ecount[i], ip->di_nlink);
}

/*
 * get next entry in a directory.
 */
struct dirent *
dcheck_readdir(dirp)
	register struct dirstuff *dirp;
{
	register struct dirent *dp;
	daddr_t lbn, d;

	for(;;) {
		if (dirp->loc >= dirp->ip->di_size)
			return NULL;
		if ((lbn = lblkno(&sblock, dirp->loc)) == 0) {
			d = bmap(lbn);
			if(d == 0)
				return NULL;
			bread(fsbtodb(&sblock, d), dirp->dbuf,
			    dblksize(&sblock, dirp->ip, lbn));
		}
		dp = (struct dirent *)
		    (dirp->dbuf + blkoff(&sblock, dirp->loc));
		dirp->loc += dp->d_reclen;
		if (dp->d_fileno == 0)
			continue;
		return (dp);
	}
}

bread(bno, buf, cnt)
daddr_t bno;
char *buf;
{
	register i;

	lseek(fi, bno * dev_bsize, 0);
	if (read(fi, buf, cnt) != cnt) {
		printf("read error %d\n", bno);
		for(i=0; i < cnt; i++)
			buf[i] = 0;
	}
}

daddr_t
bmap(i)
{
	daddr_t ibuf[MAXNINDIR];

	if(i < NDADDR)
		return(gip->di_db[i]);
	i -= NDADDR;
	if(i > NINDIR(&sblock)) {
		printf("%u - huge directory\n", ino);
		return((daddr_t)0);
	}
	bread(fsbtodb(&sblock, gip->di_ib[0]), (char *)ibuf, 
		NINDIR(&sblock)*sizeof(daddr_t));
	return(ibuf[i]);
}
