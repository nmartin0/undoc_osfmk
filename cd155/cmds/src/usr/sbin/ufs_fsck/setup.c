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
 * $Log: setup.c,v $
 * Revision 1.5  1994/11/19  03:18:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/06/29  00:33:35  dbm
 * Added modifications required to support IPI-3 devices.
 *  Reviewer: Dave Minturn / Dave Noveck (OSF)
 *  Risk:M
 *  Benefit or PTS #: PTS # 10033, added file system support for IPI-3 devices.
 *  Testing: fileio/pfs/vsx eats, PFS sats.
 *  Module(s): Complete list of the files is contained in the description of
 *             PTS 10033.
 *
 * Revision 1.3  1993/06/04  21:46:58  shala
 * Merged the work done by OSF (rabii) to support 2G files.
 * Replaced calls to howmany and roundup with calls to uhowmany and
 * uroundup which do unsigned arithmatic and can handle 2G files.
 * The two macros for roundup and uhowmany are defined in fsck.h.
 * Also removed condition that rejected files of 2G size in pass1.c
 * file.
 *
 * Revision 1.2  1992/10/12  22:08:22  shala
 * New version to support maj, min and node numbers.
 *
 * Revision 2.10.2.2  91/06/03  13:42:13  garyf
 * 	add messaging support
 * 	[91/05/13  11:35:13  garyf]
 * 
 * Revision 2.10  91/01/07  17:23:55  devrcs
 * 	rcsid/RCSfile header cleanup
 * 	[90/12/01  18:33:29  dwm]
 * 
 * Revision 2.9  90/10/31  15:40:58  devrcs
 * 	don't automatically close fswritefd
 * 	[90/10/09  13:38:53  gmf]
 * 
 * Revision 2.8  90/10/07  22:51:42  devrcs
 * 	Sync up with latest Berkeley fixes and cleanup.
 * 	[90/10/02  09:44:48  gmf]
 * 
 * Revision 2.7  90/08/25  12:36:54  devrcs
 * 	Filesystem clean flag support.
 * 	[90/08/19  01:34:39  nags]
 * 
 * Revision 2.6  90/07/27  11:45:45  devrcs
 * 	Moved to ufs_fsck directory
 * 	[90/07/20  11:31:48  pam]
 * 
 * 	Initial security hooks from SecureWare.
 * 	[90/07/12  12:57:34  seiden]
 * 
 * Revision 2.5  90/06/22  22:13:12  devrcs
 * 	Use new, faster, 4.4 fsck; FIFO, fast symlink support
 * 	[90/06/18  16:32:25  gmf]
 * 
 * $EndLog$
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: setup.c,v $ $Revision: 1.5 $ (OSF) $Date: 1994/11/19 03:18:21 $";
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

/*** "setup.c	5.32 (Berkeley) 7/22/90"; ***/

#define DKTYPENAMES
#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>
#include <sys/file.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <machine/endian.h>
#include <ctype.h>
#include "fsck.h"
#if SEC_BASE
#include <sys/security.h>
#if SEC_ARCH
#include <sys/secpolicy.h>
#endif
#endif

#ifdef  OSF1_ADFS
#define MSGSTR(n,s) s
#else
#include <locale.h>
#include "ufs_fsck_msg.h"

extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_UFS_FSCK,n,s) 
#endif  /* OSF1_ADFS */

struct bufarea asblk;
int	sbsize = SBSIZE;
int	sboff = SBOFF;
#define altsblock (*asblk.b_un.b_fs)
#define POWEROF2(num)	(((num) & ((num) - 1)) == 0)

/*
 * The size of a cylinder group is calculated by CGSIZE. The maximum size
 * is limited by the fact that cylinder groups are at most one block.
 * Its size is derived from the size of the maps maintained in the 
 * cylinder group and the (struct cg) size.
 */
#define CGSIZE(fs) \
    /* base cg */	(sizeof(struct cg) + \
    /* blktot size */	(fs)->fs_cpg * sizeof(long) + \
    /* blks size */	(fs)->fs_cpg * (fs)->fs_nrpos * sizeof(short) + \
    /* inode map */	uhowmany((fs)->fs_ipg, NBBY) + \
    /* block map */	uhowmany((fs)->fs_cpg * (fs)->fs_spc / NSPF(fs), NBBY))

struct	disklabel *getdisklabel();

setup(dev)
	char *dev;
{
	long cg, size, asked, i, j;
	long bmapsize;
	struct disklabel *lp;
	struct stat statb;
	struct fs proto;
	int	mach_record_size;

	havesb = 0;
	fswritefd = -1;
	if (stat(dev, &statb) < 0) {
		printf(MSGSTR(NOSTAT2, "Can't stat %s: %s\n"), dev, strerror(errno));
		return (0);
	}
	if ((statb.st_mode & S_IFMT) != S_IFCHR) {
		pfatal(MSGSTR(NOTCHAR1, "%s is not a character device"), dev);
		if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
			return (0);
	}
	if ((fsreadfd = open(dev, O_RDONLY)) < 0) {
		printf(MSGSTR(NOOPEN, "Can't open %s: %s\n"), dev, strerror(errno));
		return (0);
	}
	if (preen == 0)
		printf(MSGSTR(STAR, "** %s"), dev);
	if (nflag || (fswritefd = open(dev, O_WRONLY)) < 0) {
		fswritefd = -1;
		if (preen)
			pfatal(MSGSTR(NOWRITE1, "NO WRITE ACCESS"));
		printf(MSGSTR(NOWRITE, " (NO WRITE)"));
	}
	if (preen == 0)
		printf("\n");
	fsmodified = 0;
	lfdir = 0;
	initbarea(&sblk);
	initbarea(&asblk);


        if (lp = getdisklabel((char *)NULL, fsreadfd))
                secsize = lp->d_secsize;
        else
                secsize = DISK_GRANULE;

	/*
	 * Need to determine what the offset of the superblock
	 * is for this device.
	 */ 
        if (ioctl(fsreadfd, DIOMRINFO, &mach_record_size) < 0)
		mach_record_size = DISK_GRANULE;

        if (mach_record_size > SBSIZE) {
                sbsize = mach_record_size;
		sboff = BBOFF + sbsize;
        } else {
                sbsize = SBSIZE;
		sboff = SBOFF;
        }

	sblk.b_un.b_buf = malloc(sbsize);
	asblk.b_un.b_buf = malloc(sbsize);
	if (sblk.b_un.b_buf == NULL || asblk.b_un.b_buf == NULL)
		errexit(MSGSTR(NOALLOC1, "cannot allocate space for superblock\n"));
	/*
	 * Read in the superblock, looking for alternates if necessary
	 */
	i = readsb(1);
	if (i < 0)
		return(0);
	if (i == 0) {
		if (bflag || preen || calcsb(dev, fsreadfd, &proto) == 0)
			return(0);
		if (reply(MSGSTR(LOOKALT, "LOOK FOR ALTERNATE SUPERBLOCKS")) == 0)
			return (0);
		for (cg = 0; cg < proto.fs_ncg; cg++) {
			bflag = fsbtodb(&proto, cgsblock(&proto, cg));
			if (readsb(0) != 0)
				break;
		}
		if (cg >= proto.fs_ncg) {
			printf("%s\n%s\n%s\n",
MSGSTR(SFAIL, "SEARCH FOR ALTERNATE SUPER-BLOCK FAILED. YOU MUST USE THE"),
MSGSTR(SFAIL1, "-b OPTION TO FSCK TO SPECIFY THE LOCATION OF AN ALTERNATE"),
MSGSTR(SFAIL2, "SUPER-BLOCK TO SUPPLY NEEDED INFORMATION; SEE fsck(8)."));
			return(0);
		}
		pwarn(MSGSTR(ALTSUP1, "USING ALTERNATE SUPERBLOCK AT %d\n"), bflag);
	}
	maxfsblock = sblock.fs_size;
	maxino = sblock.fs_ncg * sblock.fs_ipg;
	/*
	 * Check and potentially fix certain fields in the super block.
	 */
	if (sblock.fs_optim != FS_OPTTIME && sblock.fs_optim != FS_OPTSPACE) {
		pfatal(MSGSTR(BADOP, "UNDEFINED OPTIMIZATION IN SUPERBLOCK"));
		if (reply(MSGSTR(SETTO, "SET TO DEFAULT")) == 1) {
			sblock.fs_optim = FS_OPTTIME;
			sbdirty();
		}
	}
	if ((sblock.fs_minfree < 0 || sblock.fs_minfree > 99)) {
		pfatal(MSGSTR(BADMIN, "IMPOSSIBLE MINFREE=%d IN SUPERBLOCK"),
			sblock.fs_minfree);
		if (reply(MSGSTR(SETTO, "SET TO DEFAULT")) == 1) {
			sblock.fs_minfree = 10;
			sbdirty();
		}
	}
	if (sblock.fs_interleave < 1 ||
	    sblock.fs_interleave > sblock.fs_nsect) {
		pwarn(MSGSTR(BADINTER, "IMPOSSIBLE INTERLEAVE=%d IN SUPERBLOCK"),
			sblock.fs_interleave);
		sblock.fs_interleave = 1;
		if (preen)
			printf(MSGSTR(FIXED1, " (FIXED)\n"));
		if (preen || reply(MSGSTR(SETTO, "SET TO DEFAULT")) == 1) {
			sbdirty();
			dirty(&asblk);
		}
	}
	if (sblock.fs_npsect < sblock.fs_nsect ||
	    sblock.fs_npsect > sblock.fs_nsect*2) {
		pwarn(MSGSTR(BADNPS, "IMPOSSIBLE NPSECT=%d IN SUPERBLOCK"),
			sblock.fs_npsect);
		sblock.fs_npsect = sblock.fs_nsect;
		if (preen)
			printf(MSGSTR(FIXED1, " (FIXED)\n"));
		if (preen || reply(MSGSTR(SETTO, "SET TO DEFAULT")) == 1) {
			sbdirty();
			dirty(&asblk);
		}
	}
	if (cvtflag) {
		if (sblock.fs_postblformat == FS_42POSTBLFMT) {
			/*
			 * Requested to convert from old format to new format
			 */
			if (preen)
				pwarn(MSGSTR(CONVERT1, "CONVERTING TO NEW FILE SYSTEM FORMAT\n"));
			else if (!reply(MSGSTR(CONVERT2, "CONVERT TO NEW FILE SYSTEM FORMAT")))
				return(0);
			sblock.fs_postblformat = FS_DYNAMICPOSTBLFMT;
			sblock.fs_nrpos = 8;
			sblock.fs_postbloff =
			    (char *)(&sblock.fs_opostbl[0][0]) -
			    (char *)(&sblock.fs_link);
			sblock.fs_rotbloff = &sblock.fs_space[0] -
			    (u_char *)(&sblock.fs_link);
			sblock.fs_cgsize =
				fragroundup(&sblock, CGSIZE(&sblock));
			/*
			 * Planning now for future expansion.
			 */
#			if (BYTE_ORDER == BIG_ENDIAN)
				sblock.fs_qbmask.val[0] = 0;
				sblock.fs_qbmask.val[1] = ~sblock.fs_bmask;
				sblock.fs_qfmask.val[0] = 0;
				sblock.fs_qfmask.val[1] = ~sblock.fs_fmask;
#			endif /* BIG_ENDIAN */
#			if (BYTE_ORDER == LITTLE_ENDIAN)
				sblock.fs_qbmask.val[0] = ~sblock.fs_bmask;
				sblock.fs_qbmask.val[1] = 0;
				sblock.fs_qfmask.val[0] = ~sblock.fs_fmask;
				sblock.fs_qfmask.val[1] = 0;
#			endif /* LITTLE_ENDIAN */
			sbdirty();
			dirty(&asblk);
		} else if (sblock.fs_postblformat == FS_DYNAMICPOSTBLFMT) {
			/*
			 * Requested to convert from new format to old format
			 */
			if (sblock.fs_nrpos != 8 || sblock.fs_ipg > 2048 ||
			    sblock.fs_cpg > 32 || sblock.fs_cpc > 16) {
				printf(
				MSGSTR(PARMS, "PARAMETERS OF CURRENT FILE SYSTEM DO NOT\n\t"));
				errexit(
				MSGSTR(PARMS1, "ALLOW CONVERSION TO OLD FILE SYSTEM FORMAT\n"));
			}
			if (preen)
				pwarn(MSGSTR(CONVERT3, "CONVERTING TO OLD FILE SYSTEM FORMAT\n"));
			else if (!reply(MSGSTR(CONVERT4, "CONVERT TO OLD FILE SYSTEM FORMAT")))
				return(0);
			sblock.fs_postblformat = FS_42POSTBLFMT;
			sblock.fs_cgsize = fragroundup(&sblock,
			    sizeof(struct ocg) + uhowmany(sblock.fs_fpg, NBBY));
			sbdirty();
			dirty(&asblk);
		} else {
			errexit(MSGSTR(UNKNOWN2, "UNKNOWN FILE SYSTEM FORMAT\n"));
		}
	}
	if (asblk.b_dirty) {
		bcopy((char *)&sblock, (char *)&altsblock,
			(size_t)sblock.fs_sbsize);
		flush(fswritefd, &asblk);
	}
	/*
	 * read in the summary info.
	 */
	asked = 0;
	for (i = 0, j = 0; i < sblock.fs_cssize; i += sblock.fs_bsize, j++) {
		size = sblock.fs_cssize - i < sblock.fs_bsize ?
		    sblock.fs_cssize - i : sblock.fs_bsize;
		sblock.fs_csp[j] = (struct csum *)calloc(1, (unsigned)size);
		if (bread(fsreadfd, (char *)sblock.fs_csp[j],
		    fsbtodb(&sblock, sblock.fs_csaddr + j * sblock.fs_frag),
		    size) != 0 && !asked) {
			pfatal(MSGSTR(BADSUM1, "BAD SUMMARY INFORMATION"));
			if (reply(MSGSTR(CONTINUE, "CONTINUE")) == 0)
				errexit("");
			asked++;
		}
	}
	/* see if we can skip this fsck run. conditions are:
	 *      - not processing root file system
	 *      - super block has not been modified
	 *      - not using alternative superblock
	 */
	if (!hotroot && sblock.fs_clean == FS_CLEAN &&
	    sblk.b_dirty == 0 && !bflag && !nflag && !clnoverride) {
		pwarn(MSGSTR(CLEANLY, "File system unmounted cleanly - no fsck needed\n"));
		/* release the space we allocated */
		free(sblk.b_un.b_buf);
		return (FS_CLEAN);
	}
	/*
	 * allocate and initialize the necessary maps
	 */
	bmapsize = uroundup(uhowmany(maxfsblock, NBBY), sizeof(short));
	blockmap = calloc((unsigned)bmapsize, sizeof (char));
	if (blockmap == NULL) {
		printf(MSGSTR(NOALLOC2, "cannot alloc %u bytes for blockmap\n"), 
			(unsigned)bmapsize);
		goto badsb;
	}
	statemap = calloc((unsigned)(maxino + 1), sizeof(char));
	if (statemap == NULL) {
		printf(MSGSTR(NOALLOC3, "cannot alloc %u bytes for statemap\n"), 
			(unsigned)(maxino + 1));
		goto badsb;
	}
	lncntp = (short *)calloc((unsigned)(maxino + 1), sizeof(short));
	if (lncntp == NULL) {
		printf(MSGSTR(NOALLOC4, "cannot alloc %u bytes for lncntp\n"), 
		    (unsigned)(maxino + 1) * sizeof(short));
		goto badsb;
	}
	numdirs = sblock.fs_cstotal.cs_ndir;
	inplast = 0;
	listmax = numdirs + 10;
	inpsort = (struct inoinfo **)calloc((unsigned)listmax,
	    sizeof(struct inoinfo *));
	inphead = (struct inoinfo **)calloc((unsigned)numdirs,
	    sizeof(struct inoinfo *));
	if (inpsort == NULL || inphead == NULL) {
		printf(MSGSTR(NOALLOC5, "cannot alloc %u bytes for inphead\n"), 
		    (unsigned)numdirs * sizeof(struct inoinfo *));
		goto badsb;
	}
	bufinit();
	return (1);

badsb:
	ckfini();
	return (0);
}

/*
 * Read in the super block and its summary info.
 */
readsb(listerr)
	int listerr;
{
	daddr_t super = bflag ? bflag : btodg(sboff);
	int i;
	int set_fsbtodb = 0;

	if (dev_bshift == 0) 
		dev_bshift = DISK_GSHIFT;
	super = bflag ? bflag : btodu(sboff);

	if (bread(fsreadfd, (char *)&sblock, super, (long)sbsize) != 0)
		return (0);
	sblk.b_bno = super;
	sblk.b_size = sbsize;
	/*
	 * run a few consistency checks of the super block
	 */
#if SEC_FSCHANGE
	if ((sblock.fs_magic != FS_MAGIC) && (sblock.fs_magic != FS_SEC_MAGIC))
#else
	if (sblock.fs_magic != FS_MAGIC)
#endif
		{ badsb(listerr, MSGSTR(BADMAGIC1, "MAGIC NUMBER WRONG")); return (0); }
	if (sblock.fs_ncg < 1)
		{ badsb(listerr, MSGSTR(BADNCG, "NCG OUT OF RANGE")); return (0); }
	if (sblock.fs_cpg < 1)
		{ badsb(listerr, MSGSTR(BADCPG, "CPG OUT OF RANGE")); return (0); }
	if (sblock.fs_ncg * sblock.fs_cpg < sblock.fs_ncyl ||
	    (sblock.fs_ncg - 1) * sblock.fs_cpg >= sblock.fs_ncyl)
		{ badsb(listerr, MSGSTR(BADNCYL, "NCYL LESS THAN NCG*CPG")); return (0); }
	if (sblock.fs_sbsize > sbsize)
		{ badsb(listerr, MSGSTR(PREPSIZE, "SIZE PREPOSTEROUSLY LARGE")); return (0); }
#if SEC_FSCHANGE
	if (preen == 0)
		if (sblock.fs_magic == FS_SEC_MAGIC)
			printf(MSGSTR(EXTFS, "** Extended Format Filesystem\n"));
		else
			printf(MSGSTR(NXFS, "** Non-extended Format Filesystem\n"));

	disk_set_file_system(&sblock, sblock.fs_bsize);
#endif
	/*
	 * Compute the proper value for fs_fsbtodb based on DISK_GRANULE
	 * and provide conversion logic to support the old practice of basing
	 * this on sector size.
	 */
	old_fsbtodb = sblock.fs_fsbtodb;
	new_fsbtodb = compute_fsbtodb(btodg(sblock.fs_fsize));
	dev_bshift = DISK_GSHIFT;
	secsize = sblock.fs_fsize / sblock.fs_nspf;
	if (Cflag > 1) {
		new_fsbtodb = compute_fsbtodb(sblock.fs_nspf);
		dev_bshift = sblock.fs_fshift - new_fsbtodb;
 		sblk.b_bno >>= dev_bshift - DISK_GSHIFT;
	}
	if (new_fsbtodb == old_fsbtodb) {
		if (Cflag != 0)
			printf(MSGSTR(FSBTODB6,
				"-C ignored.  Conversion unneeded.\n"));
		old_fsbtodb = -1;
		new_fsbtodb = -1;
	}
	else {
		printf(MSGSTR(FSBTODB1, "fs_fsbtodb SET TO %d\n"),
			old_fsbtodb);
		printf(MSGSTR(FSBTODB2, "CORRECT VALUE is %d\n"),
			new_fsbtodb);
		if (Cflag == 0) {
			printf(MSGSTR(FSBTODB3,
				"-C MUST BE SPECIFIED FOR CONVERSION\n"));
			return(0);
		}
		if (preen == 0 && reply(MSGSTR(FSBTODB4, "CORRECT")) == 0)
			return(0);
		printf(MSGSTR(FSBTODB5,
			"di_blocks VALUES WILL BE SILENTLY CONVERTED\n"));
		sblock.fs_fsbtodb = new_fsbtodb;
		set_fsbtodb = 1;
	}

	/*
	 * Set all possible fields that could differ, then do check
	 * of whole super block against an alternate super block.
	 * When an alternate super-block is specified this check is skipped.
	 */
	getblk(&asblk, cgsblock(&sblock, sblock.fs_ncg - 1), sblock.fs_sbsize);
	if (asblk.b_errs)
		return (0);
	if (bflag) {
		goto finish;
	}
	altsblock.fs_link = sblock.fs_link;
	altsblock.fs_rlink = sblock.fs_rlink;
	altsblock.fs_time = sblock.fs_time;
	altsblock.fs_cstotal = sblock.fs_cstotal;
	altsblock.fs_cgrotor = sblock.fs_cgrotor;
	altsblock.fs_fmod = sblock.fs_fmod;
	altsblock.fs_clean = sblock.fs_clean;
	altsblock.fs_ronly = sblock.fs_ronly;
	altsblock.fs_flags = sblock.fs_flags;
	altsblock.fs_maxcontig = sblock.fs_maxcontig;
	altsblock.fs_minfree = sblock.fs_minfree;
	altsblock.fs_optim = sblock.fs_optim;
	altsblock.fs_rotdelay = sblock.fs_rotdelay;
	altsblock.fs_maxbpg = sblock.fs_maxbpg;
	bcopy((char *)sblock.fs_csp, (char *)altsblock.fs_csp,
		sizeof sblock.fs_csp);
	bcopy((char *)sblock.fs_fsmnt, (char *)altsblock.fs_fsmnt,
		sizeof sblock.fs_fsmnt);
	bcopy((char *)sblock.fs_sparecon, (char *)altsblock.fs_sparecon,
		sizeof sblock.fs_sparecon);
	/*
	 * The following should not have to be copied.
	 */
	altsblock.fs_fsbtodb = sblock.fs_fsbtodb;
	altsblock.fs_interleave = sblock.fs_interleave;
	altsblock.fs_npsect = sblock.fs_npsect;
	altsblock.fs_nrpos = sblock.fs_nrpos;
	if (bcmp((char *)&sblock, (char *)&altsblock, (int)sblock.fs_sbsize)) {
		badsb(listerr,
MSGSTR(VALUES, "VALUES IN SUPER BLOCK DISAGREE WITH THOSE IN FIRST ALTERNATE"));
		return (0);
	}
finish:
	havesb = 1;
	if (set_fsbtodb)
		sbdirty();
	return (1);
}

badsb(listerr, s)
	int listerr;
	char *s;
{

	if (!listerr)
		return;
	if (preen)
		printf("%s: ", devname);
	pfatal(MSGSTR(BADSUP, "BAD SUPER BLOCK: %s\n"), s);
}

/*
 * Calculate a prototype superblock based on information in the disk label.
 * When done the cgsblock macro can be calculated and the fs_ncg field
 * can be used. Do NOT attempt to use other macros without verifying that
 * their needed information is available!
 */
calcsb(dev, devfd, fs)
	char *dev;
	int devfd;
	register struct fs *fs;
{
	register struct disklabel *lp;
	register struct partition *pp;
	register char *cp;
	int i;

	cp = index(dev, '\0') - 1;
	if (cp == (char *)-1 || (*cp < 'a' || *cp > 'h') && !isdigit(*cp)) {
		pfatal(MSGSTR(BADPART, "%s: CANNOT FIGURE OUT FILE SYSTEM PARTITION\n"), dev);
		return (0);
	}
	lp = getdisklabel(dev, devfd);
	if (isdigit(*cp))
		pp = &lp->d_partitions[0];
	else
		pp = &lp->d_partitions[*cp - 'a'];
	if (pp->p_fstype != FS_BSDFFS) {
		pfatal(MSGSTR(BADLABEL, "%s: NOT LABELED AS A BSD FILE SYSTEM (%s)\n"),
			dev, pp->p_fstype < FSMAXTYPES ?
			fstypenames[pp->p_fstype] : MSGSTR(UNKNOWN, "unknown"));
		return (0);
	}
	bzero((char *)fs, sizeof(struct fs));
	fs->fs_fsize = pp->p_fsize;
	fs->fs_frag = pp->p_frag;
	fs->fs_cpg = pp->p_cpg;
	fs->fs_size = pp->p_size;
	fs->fs_ntrak = lp->d_ntracks;
	fs->fs_nsect = lp->d_nsectors;
	fs->fs_spc = lp->d_secpercyl;
	fs->fs_nspf = fs->fs_fsize / lp->d_secsize;
	fs->fs_sblkno = uroundup(
		uhowmany(lp->d_bbsize + lp->d_sbsize, fs->fs_fsize),
		fs->fs_frag);
	fs->fs_cgmask = 0xffffffff;
	for (i = fs->fs_ntrak; i > 1; i >>= 1)
		fs->fs_cgmask <<= 1;
	if (!POWEROF2(fs->fs_ntrak))
		fs->fs_cgmask <<= 1;
	fs->fs_cgoffset = uroundup(
		uhowmany(fs->fs_nsect, NSPF(fs)), fs->fs_frag);
	fs->fs_fpg = (fs->fs_cpg * fs->fs_spc) / NSPF(fs);
	fs->fs_ncg = uhowmany(fs->fs_size / fs->fs_spc, fs->fs_cpg);
	fs->fs_fsbtodb = compute_fsbtodb(btodg(fs->fs_fsize));
	return (1);
}

compute_fsbtodb(bct)
	int	bct;
{
	int ret = 0;
	int i;

	for (i = bct; i > 1; i >>= 1)
		ret++;
	return(ret);
}

struct disklabel *
getdisklabel(s, fd)
	char *s;
	int	fd;
{
	static struct disklabel lab;

	if (ioctl(fd, DIOCGDINFO, (char *)&lab) < 0) {
		if (s == NULL)
			return ((struct disklabel *)NULL);
		pwarn(MSGSTR(BADIOCTL, "ioctl (GCINFO): %s\n"), strerror(errno));
		errexit(MSGSTR(CANTREAD, "%s: can't read disk label\n"), s);
	}
	return (&lab);
}
