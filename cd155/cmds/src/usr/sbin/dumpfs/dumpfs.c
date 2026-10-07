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
 
/* @(#)dumpfs.c	3.2 23:22:38 6/19/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Copyright (c) 1983 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)dumpfs.c	5.10 (Berkeley) 6/1/90";
#endif /* not lint */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#endif
#include <sys/param.h>
#include <ufs/dinode.h>
#include <ufs/fs.h>

#include <stdio.h>
#include <fstab.h>
#include <sys/ioctl.h>
#include <sys/disklabel.h>

/*
 * dumpfs
 */

struct fs *sblock_ptr=NULL;
int sbsize,sboff,mach_record_size;
#define afs  (*sblock_ptr)


union {
	struct cg cg;
	char pad[MAXBSIZE];
} cgun;
#define	acg	cgun.cg

long	dev_bsize = 1;

#if SEC_BASE
extern priv_t *privvec();
#endif

main(argc, argv)
	char **argv;
{
	register struct fstab *fs;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("sysadmin")) {
		fprintf(stderr, "%s: need sysadmin authorization\n",
			command_name);
		exit(1);
	}
#endif
	argc--, argv++;
	if (argc < 1) {
		fprintf(stderr, "usage: dumpfs fs ...\n");
		exit(1);
	}
	for (; argc > 0; argv++, argc--) {
		fs = getfsfile(*argv);
		if (fs == 0)
			dumpfs(*argv);
		else
			dumpfs(fs->fs_spec);
	}
}

dumpfs(name)
	char *name;
{
	int c, i, j, k, size;

	close(0);
#if SEC_BASE
	{
		privvec_t	saveprivs;
		int		fd;

		if (forceprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
					SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
					SEC_ALLOWNCAVACCESS,
#endif
					-1), saveprivs)) {
			fprintf(stderr, "%s: insufficient privileges\n",
				command_name);
			exit(1);
		}
		fd = open(name, 0);
		seteffprivs(saveprivs, (priv_t *) 0);
		if (fd != 0) {
			perror(name);
			return;
		}
	}
#else /* !SEC_BASE */
	if (open(name, 0) != 0) {
		perror(name);
		return;
	}
#endif /* !SEC_BASE */
	
        if (ioctl(0, DIOMRINFO, &mach_record_size) < 0) {
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

	lseek(0, sboff, 0);
	if (read(0, &afs, sbsize) != sbsize) {
		perror(name);
		return;
	}
	if (afs.fs_postblformat == FS_42POSTBLFMT)
		afs.fs_nrpos = 8;
	dev_bsize = afs.fs_fsize / fsbtodb(&afs, 1);
#if SEC_FSCHANGE
	printf("magic\t%x (%s)\tformat\t%s\ttime\t%s", afs.fs_magic,
	    afs.fs_magic == FS_SEC_MAGIC ? "Extended" : "Unextended",
	    afs.fs_postblformat == FS_42POSTBLFMT ? "static" : "dynamic",
	    ctime(&afs.fs_time));
#else
	printf("magic\t%x\tformat\t%s\ttime\t%s", afs.fs_magic,
	    afs.fs_postblformat == FS_42POSTBLFMT ? "static" : "dynamic",
	    ctime(&afs.fs_time));
#endif
	printf("nbfree\t%d\tndir\t%d\tnifree\t%d\tnffree\t%d\n",
	    afs.fs_cstotal.cs_nbfree, afs.fs_cstotal.cs_ndir,
	    afs.fs_cstotal.cs_nifree, afs.fs_cstotal.cs_nffree);
	printf("ncg\t%d\tncyl\t%d\tsize\t%d\tblocks\t%d\n",
	    afs.fs_ncg, afs.fs_ncyl, afs.fs_size, afs.fs_dsize);
	printf("bsize\t%d\tshift\t%d\tmask\t0x%08x\n",
	    afs.fs_bsize, afs.fs_bshift, afs.fs_bmask);
	printf("fsize\t%d\tshift\t%d\tmask\t0x%08x\n",
	    afs.fs_fsize, afs.fs_fshift, afs.fs_fmask);
	printf("frag\t%d\tshift\t%d\tfsbtodb\t%d\n",
	    afs.fs_frag, afs.fs_fragshift, afs.fs_fsbtodb);
	printf("cpg\t%d\tbpg\t%d\tfpg\t%d\tipg\t%d\n",
	    afs.fs_cpg, afs.fs_fpg / afs.fs_frag, afs.fs_fpg, afs.fs_ipg);
	printf("minfree\t%d%%\toptim\t%s\tmaxcontig %d\tmaxbpg\t%d\n",
	    afs.fs_minfree, afs.fs_optim == FS_OPTSPACE ? "space" : "time",
	    afs.fs_maxcontig, afs.fs_maxbpg);
	printf("rotdelay %dms\theadswitch %dus\ttrackseek %dus\trps\t%d\n",
	    afs.fs_rotdelay, afs.fs_headswitch, afs.fs_trkseek, afs.fs_rps);
	printf("ntrak\t%d\tnsect\t%d\tnpsect\t%d\tspc\t%d\n",
	    afs.fs_ntrak, afs.fs_nsect, afs.fs_npsect, afs.fs_spc);
	printf("trackskew %d\tinterleave %d\n",
	    afs.fs_trackskew, afs.fs_interleave);
	printf("nindir\t%d\tinopb\t%d\tnspf\t%d\n",
	    afs.fs_nindir, afs.fs_inopb, afs.fs_nspf);
	printf("sblkno\t%d\tcblkno\t%d\tiblkno\t%d\tdblkno\t%d\n",
	    afs.fs_sblkno, afs.fs_cblkno, afs.fs_iblkno, afs.fs_dblkno);
	printf("sbsize\t%d\tcgsize\t%d\tcgoffset %d\tcgmask\t0x%08x\n",
	    afs.fs_sbsize, afs.fs_cgsize, afs.fs_cgoffset, afs.fs_cgmask);
	printf("csaddr\t%d\tcssize\t%d\tshift\t%d\tmask\t0x%08x\n",
	    afs.fs_csaddr, afs.fs_cssize, afs.fs_csshift, afs.fs_csmask);
	printf("cgrotor\t%d\tfmod\t%d\tronly\t%d\tclean\t%d\n",
	    afs.fs_cgrotor, afs.fs_fmod, afs.fs_ronly, afs.fs_clean);
	if (afs.fs_cpc != 0)
		printf("blocks available in each of %d rotational positions",
		     afs.fs_nrpos);
	else
		printf("insufficient space to maintain rotational tables\n");
	for (c = 0; c < afs.fs_cpc; c++) {
		printf("\ncylinder number %d:", c);
		for (i = 0; i < afs.fs_nrpos; i++) {
			if (fs_postbl(&afs, c)[i] == -1)
				continue;
			printf("\n   position %d:\t", i);
			for (j = fs_postbl(&afs, c)[i], k = 1; ;
			     j += fs_rotbl(&afs)[j], k++) {
				printf("%5d", j);
				if (k % 12 == 0)
					printf("\n\t\t");
				if (fs_rotbl(&afs)[j] == 0)
					break;
			}
		}
	}
	printf("\ncs[].cs_(nbfree,ndir,nifree,nffree):\n\t");
	for (i = 0, j = 0; i < afs.fs_cssize; i += afs.fs_bsize, j++) {
		size = afs.fs_cssize - i < afs.fs_bsize ?
		    afs.fs_cssize - i : afs.fs_bsize;
		afs.fs_csp[j] = (struct csum *)calloc(1, size);
		lseek(0, fsbtodb(&afs, (afs.fs_csaddr + j * afs.fs_frag)) *
		    dev_bsize, 0);
		if (read(0, afs.fs_csp[j], size) != size) {
			perror(name);
			return;
		}
	}
	for (i = 0; i < afs.fs_ncg; i++) {
		struct csum *cs = &afs.fs_cs(&afs, i);
		if (i && i % 4 == 0)
			printf("\n\t");
		printf("(%d,%d,%d,%d) ",
		    cs->cs_nbfree, cs->cs_ndir, cs->cs_nifree, cs->cs_nffree);
	}
	printf("\n");
	if (afs.fs_ncyl % afs.fs_cpg) {
		printf("cylinders in last group %d\n",
		    i = afs.fs_ncyl % afs.fs_cpg);
		printf("blocks in last group %d\n",
		    i * afs.fs_spc / NSPB(&afs));
	}
	printf("\n");
	for (i = 0; i < afs.fs_ncg; i++)
		dumpcg(name, i);
	close(0);
	if (sblock_ptr) {
		free(sblock_ptr);
	}
};

dumpcg(name, c)
	char *name;
	int c;
{
	int i,j;

	printf("\ncg %d:\n", c);
	lseek(0, fsbtodb(&afs, cgtod(&afs, c)) * dev_bsize, 0);
	i = lseek(0, 0, 1);
	if (read(0, (char *)&acg, afs.fs_bsize) != afs.fs_bsize) {
		printf("dumpfs: %s: error reading cg\n", name);
		return;
	}
	printf("magic\t%x\ttell\t%x\ttime\t%s",
	    afs.fs_postblformat == FS_42POSTBLFMT ?
	    ((struct ocg *)&acg)->cg_magic : acg.cg_magic,
	    i, ctime(&acg.cg_time));
	printf("cgx\t%d\tncyl\t%d\tniblk\t%d\tndblk\t%d\n",
	    acg.cg_cgx, acg.cg_ncyl, acg.cg_niblk, acg.cg_ndblk);
	printf("nbfree\t%d\tndir\t%d\tnifree\t%d\tnffree\t%d\n",
	    acg.cg_cs.cs_nbfree, acg.cg_cs.cs_ndir,
	    acg.cg_cs.cs_nifree, acg.cg_cs.cs_nffree);
	printf("rotor\t%d\tirotor\t%d\tfrotor\t%d\nfrsum",
	    acg.cg_rotor, acg.cg_irotor, acg.cg_frotor);
	for (i = 1, j = 0; i < afs.fs_frag; i++) {
		printf("\t%d", acg.cg_frsum[i]);
		j += i * acg.cg_frsum[i];
	}
	printf("\nsum of frsum: %d\niused:\t", j);
	pbits(cg_inosused(&acg), afs.fs_ipg);
	printf("free:\t");
	pbits(cg_blksfree(&acg), afs.fs_fpg);
	printf("b:\n");
	for (i = 0; i < afs.fs_cpg; i++) {
		if (cg_blktot(&acg)[i] == 0)
			continue;
		printf("   c%d:\t(%d)\t", i, cg_blktot(&acg)[i]);
		for (j = 0; j < afs.fs_nrpos; j++) {
			if (afs.fs_cpc > 0 &&
			    fs_postbl(&afs, i % afs.fs_cpc)[j] == -1)
				continue;
			printf(" %d", cg_blks(&afs, &acg, i)[j]);
		}
		printf("\n");
	}
};

pbits(cp, max)
	register char *cp;
	int max;
{
	register int i;
	int count = 0, j;

	for (i = 0; i < max; i++)
		if (isset(cp, i)) {
			if (count)
				printf(",%s", count % 6 ? " " : "\n\t");
			count++;
			printf("%d", i);
			j = i;
			while ((i+1)<max && isset(cp, i+1))
				i++;
			if (i != j)
				printf("-%d", i);
		}
	printf("\n");
}
