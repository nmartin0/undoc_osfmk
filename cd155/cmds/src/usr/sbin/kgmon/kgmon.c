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
 * Copyright (c) 1983 The Regents of the University of California.
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
/*
 * OSF/1 Release 1.0
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: kgmon.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:07:31 $";
#endif

#include <sys/param.h>
#include <sys/file.h>
#include <sys/vm.h>
#include <sys/gprof.h>
#include <stdio.h>
#include <nlist.h>
#include <ctype.h>
#include <paths.h>

#define	PROFILING_ON	0
#define	PROFILING_OFF	3

u_long	s_textsize;
off_t	sbuf, klseek(), lseek();
int	ssiz;

struct nlist nl[] = {
#define N_HZ		0
	{ "_hz" },
#define N_FROMS		1
	{ "_froms" },
#define	N_PROFILING	2
	{ "_profiling" },
#define	N_S_LOWPC	3
	{ "_s_lowpc" },
#define	N_S_TEXTSIZE	4
	{ "_s_textsize" },
#define	N_SBUF		5
	{ "_sbuf" },
#define N_SSIZ		6
	{ "_ssiz" },
#define	N_TOS		7
	{ "_tos" },
#if	mips
#define N_PHDR		8
	{ "_phdr" },
#endif
	0,
};

int	kmem;
int	bflag, hflag, rflag, pflag;
int	debug = 0;

main(argc, argv)
	int argc;
	char **argv;
{
	extern char *optarg;
	extern int optind;
	int ch, mode, disp, openmode;
	char *system, *kmemf, *malloc();

	while ((ch = getopt(argc, argv, "bhpr")) != EOF)
		switch((char)ch) {
		case 'b':
			bflag++;
			break;
		case 'h':
			hflag++;
			break;
		case 'p':
			pflag++;
			break;
		case 'r':
			rflag++;
			break;
		default:
			(void)fprintf(stderr,
			    "usage: kgmon [-bhrp] [system]\n");
			exit(1);
		}
	argc -= optind;
	argv += optind;

	kmemf = _PATH_KMEM;
	if (*argv) {
		system = *argv;
	}
	else
		system = _PATH_UNIX;

	if (nlist(system, nl) < 0 || nl[0].n_type == 0) {
		(void)fprintf(stderr, "kgmon: %s: no namelist\n", system);
		exit(2);
	}
	if (!nl[N_PROFILING].n_value) {
		(void)fprintf(stderr,
		    "kgmon: profiling: not defined in kernel.\n");
		exit(10);
	}

	openmode = (bflag || hflag || pflag || rflag) ? O_RDWR : O_RDONLY;
	kmem = open(kmemf, openmode);
	if (kmem < 0) {
		openmode = O_RDONLY;
		kmem = open(kmemf, openmode);
		if (kmem < 0) {
			perror(kmemf);
			exit(3);
		}
		(void)fprintf(stderr, "%s opened read-only\n", kmemf);
		if (rflag)
			(void)fprintf(stderr, "-r supressed\n");
		if (bflag)
			(void)fprintf(stderr, "-b supressed\n");
		if (hflag)
			(void)fprintf(stderr, "-h supressed\n");
		rflag = bflag = hflag = 0;
	}
	mode = kfetch(N_PROFILING);
	if (hflag)
		disp = PROFILING_OFF;
	else if (bflag)
		disp = PROFILING_ON;
	else
		disp = mode;
	if (pflag) {
		if (openmode == O_RDONLY && mode == PROFILING_ON)
			(void)fprintf(stderr, "data may be inconsistent\n");
		dumpstate();
	}
	if (rflag)
		resetstate();
	turnonoff(disp);
	(void)fprintf(stdout,
	    "kernel profiling is %s.\n", disp ? "off" : "running");
}

dumpstate()
{
	extern int errno;
	struct rawarc rawarc;
	struct tostruct *tos;
	u_long frompc;
	off_t kfroms, ktos;
	u_short *froms;		/* froms is a bunch of u_shorts indexing tos */
	int i, fd, fromindex, endfrom, fromssize, tossize, toindex;
	char buf[BUFSIZ], *s_lowpc, *malloc(), *strerror();
#if	mips
        struct phdr phdr;
        int hdr;
#endif

	turnonoff(PROFILING_OFF);
	fd = creat("gmon.out", 0666);
	if (fd < 0) {
		perror("gmon.out");
		return;
	}
#if	mips
	ksteal(N_PHDR, &phdr, sizeof(phdr));
        write(fd, &phdr.lowpc, sizeof(phdr.lowpc));
        write(fd, &phdr.highpc, sizeof(phdr.highpc));
        write(fd, &phdr.pc_bytes, sizeof(phdr.pc_bytes));

        ssiz = phdr.pc_bytes;
        sbuf = (off_t)phdr.pc_buf;
#else
	ssiz = kfetch(N_SSIZ);
	sbuf = kfetch(N_SBUF);
#endif
	(void)klseek(kmem, (off_t)sbuf, L_SET);
	for (i = ssiz; i > 0; i -= BUFSIZ) {
		read(kmem, buf, i < BUFSIZ ? i : BUFSIZ);
		write(fd, buf, i < BUFSIZ ? i : BUFSIZ);
	}
	s_textsize = kfetch(N_S_TEXTSIZE);
	fromssize = s_textsize / HASHFRACTION;
	froms = (u_short *)malloc((u_int)fromssize);
	kfroms = kfetch(N_FROMS);
	(void)klseek(kmem, kfroms, L_SET);
	i = read(kmem, ((char *)(froms)), fromssize);
	if (i != fromssize) {
		(void)fprintf(stderr, "read kmem: request %d, got %d: %s",
		    fromssize, i, strerror(errno));
		exit(5);
	}
	tossize = (s_textsize * ARCDENSITY / 100) * sizeof(struct tostruct);
	tos = (struct tostruct *)malloc((u_int)tossize);
	ktos = kfetch(N_TOS);
	(void)klseek(kmem, ktos, L_SET);
	i = read(kmem, ((char *)(tos)), tossize);
	if (i != tossize) {
		(void)fprintf(stderr, "read kmem: request %d, got %d: %s",
		    tossize, i, strerror(errno));
		exit(6);
	}
	s_lowpc = (char *)kfetch(N_S_LOWPC);
	if (debug)
		(void)fprintf(stderr, "s_lowpc 0x%x, s_textsize 0x%x\n",
		    s_lowpc, s_textsize);
	endfrom = fromssize / sizeof(*froms);
	for (fromindex = 0; fromindex < endfrom; fromindex++) {
		if (froms[fromindex] == 0)
			continue;
		frompc = (u_long)s_lowpc +
		    (fromindex * HASHFRACTION * sizeof(*froms));
		for (toindex = froms[fromindex]; toindex != 0;
		   toindex = tos[toindex].link) {
			if (debug)
			    (void)fprintf(stderr,
			    "[mcleanup] frompc 0x%x selfpc 0x%x count %d\n" ,
			    frompc, tos[toindex].selfpc, tos[toindex].count);
			rawarc.raw_frompc = frompc;
			rawarc.raw_selfpc = (u_long)tos[toindex].selfpc;
			rawarc.raw_count = tos[toindex].count;
			write(fd, (char *)&rawarc, sizeof (rawarc));
		}
	}
	close(fd);
}

resetstate()
{
	off_t kfroms, ktos;
	int i, fromssize, tossize;
	char buf[BUFSIZ];
#if	mips
        struct phdr phdr;
#endif
        
	turnonoff(PROFILING_OFF);
	bzero(buf, BUFSIZ);
#if	mips
	ksteal(N_PHDR, &phdr, sizeof(phdr));
        ssiz = phdr.pc_bytes;
        sbuf = (off_t)phdr.pc_buf;
#else
	ssiz = kfetch(N_SSIZ);
	sbuf = kfetch(N_SBUF);
	ssiz -= sizeof(struct phdr);
	sbuf += sizeof(struct phdr);
#endif
	(void)klseek(kmem, (off_t)sbuf, L_SET);
	for (i = ssiz; i > 0; i -= BUFSIZ)
		if (write(kmem, buf, i < BUFSIZ ? i : BUFSIZ) < 0) {
			perror("sbuf write");
			exit(7);
		}
	s_textsize = kfetch(N_S_TEXTSIZE);
	fromssize = s_textsize / HASHFRACTION;
	kfroms = kfetch(N_FROMS);
	(void)klseek(kmem, kfroms, L_SET);
	for (i = fromssize; i > 0; i -= BUFSIZ)
		if (write(kmem, buf, i < BUFSIZ ? i : BUFSIZ) < 0) {
			perror("kforms write");
			exit(8);
		}
	tossize = (s_textsize * ARCDENSITY / 100) * sizeof(struct tostruct);
	ktos = kfetch(N_TOS);
	(void)klseek(kmem, ktos, L_SET);
	for (i = tossize; i > 0; i -= BUFSIZ)
		if (write(kmem, buf, i < BUFSIZ ? i : BUFSIZ) < 0) {
			perror("ktos write");
			exit(9);
		}
}

turnonoff(onoff)
	int onoff;
{
	(void)klseek(kmem, (long)nl[N_PROFILING].n_value, L_SET);
	(void)write(kmem, (char *)&onoff, sizeof (onoff));
}

kfetch(index)
	int index;
{
	off_t off;
	int value;

	if ((off = nl[index].n_value) == 0) {
		printf("%s: not defined in kernel\n", nl[index].n_name);
		exit(11);
	}
	if (klseek(kmem, off, L_SET) == -1) {
		perror("lseek");
		exit(12);
	}
	if (read(kmem, (char *)&value, sizeof (value)) != sizeof (value)) {
		perror("read");
		exit(13);
	}
	return (value);
}

#if	mips
ksteal(index, buf, sz)
	int index;
        char *buf;
        int sz;
{
	off_t off;

	if ((off = nl[index].n_value) == 0) {
		printf("%s: not defined in kernel\n", nl[index].n_name);
		exit(11);
	}
	if (klseek(kmem, off, L_SET) == -1) {
		perror("lseek");
		exit(12);
	}
	if (read(kmem, (char *)buf, sz) != sz) {
		perror("read");
		exit(13);
	}
}
#endif

off_t
klseek(fd, base, off)
	int fd, off;
	off_t base;
{
	return (lseek(fd, base, off));
}
