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
 * $Log: nfsstat.c,v $
 * Revision 1.3  1994/11/19  01:31:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/12/10  23:02:42  shala
 * Support the new table call.
 *
 * Revision 2.2  92/11/18  14:27:14  loverso
 * 	Add use of table() for OSF/1 AD.
 * 
 * Revision 1.4  90/10/07  16:47:25  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  14:56:16  gm]
 * 
 * Revision 1.3  90/07/27  11:40:14  devrcs
 * 	Add directory stats to client output. Join two server stats
 * 	lines so it fits on 24-line screen. Rearrange code to avoid
 * 	warning in signal arguments.
 * 	[90/07/21  19:04:26  tmt]
 * 
 * Revision 1.2  90/02/23  00:19:28  devrcs
 * 	Changes for snapshot
 * 	[90/02/20  23:25:54  gm]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1983, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983, 1989 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)nfsstat.c	5.1 (Berkeley) 7/16/89";
#endif /* not lint */

#include <mach.h>
#include <sys/param.h>
#include <sys/vmmac.h>
#include <sys/file.h>
#if	!MACH
#include <machine/pte.h>
#endif
#include <sys/time.h>
#include <sys/namei.h>
#include <sys/mount.h>
#include <nfs/nfsv2.h>
#include <nfs/nfs.h>
#include <nfs/nfsmount.h>
#include <ctype.h>
#include <errno.h>
#include <nlist.h>
#include <stdio.h>
#include <paths.h>
#include <strings.h>
#include <sys/table.h>

#define	YES	1
#define	NO	0

struct nlist nl[] = {
#define	N_NFSSTAT	0
	{ "_nfsstats" },
#if	0
#define	N_SYSMAP	1
	{ "_Sysmap" },
#define	N_SYSSIZE	2
	{ "_Syssize" },
#endif	0
	"",
};

#if	0
struct	pte *Sysmap;
#endif	0

char	*system = _PATH_UNIX;
char	*kmemf = _PATH_KMEM;
int	kmem = -1;
int	kflag;
int	interval;

extern	char *malloc();
extern	off_t lseek();

main(argc, argv)
	int argc;
	char *argv[];
{
	extern char *optarg;
	extern int optind;
	int ch;
	int interval = 0;

	while ((ch = getopt(argc, argv, "i:")) != EOF)
		switch((char)ch) {
		case 'i':
			interval = atoi(optarg);
			break;
		default:
			usage();
		}
	if (interval < 0)
		usage();
	argc -= optind;
	argv += optind;
	if (argc > 0) {
		system = *argv;
		argv++, argc--;
		if (argc > 0) {
			kmemf = *argv;
			kflag++;
		}
		if (nlist(system, nl) < 0 || nl[0].n_type == 0) {
			fprintf(stderr, "%s: no namelist\n", system);
			exit(1);
		}
		kmem = open(kmemf, O_RDONLY);
		if (kmem < 0) {
			perror(kmemf);
			exit(1);
		}
	}
#if	0
	if (kflag) {
		off_t off;

		Sysmap = (struct pte *)
		   malloc((u_int)(nl[N_SYSSIZE].n_value * sizeof(struct pte)));
		if (!Sysmap) {
			fputs("nfsstat: can't get memory for Sysmap.\n", stderr);
			exit(1);
		}
		off = nl[N_SYSMAP].n_value & ~KERNBASE;
		(void)lseek(kmem, off, L_SET);
		(void)read(kmem, (char *)Sysmap,
			(int)(nl[N_SYSSIZE].n_value * sizeof(struct pte)));
	}
#endif	0
	intpr(interval, nl[N_NFSSTAT].n_value);
	exit(0);
}

/*
 * Seek into the kernel for a value.
 */
off_t
klseek(fd, base, off)
	int fd, off;
	off_t base;
{
#if	0
	if (kflag) {
		/* get kernel pte */
		base &= ~KERNBASE;
		base = ctob(Sysmap[btop(base)].pg_pfnum) + (base & PGOFSET);
	}
#endif	0
	return (lseek(fd, base, off));
}

usage()
{
	fputs("Usage: nfsstat [-i interval [ system [ corefile ] ] ]\n", stderr);
	exit(1);
}

/*
 * Print a description of the network interfaces.
 */
intpr(interval, nfsstataddr)
	int interval;
	off_t nfsstataddr;
{
	struct nfsstats nfsstats;

	if (kmem >= 0 && nfsstataddr == 0) {
		printf("nfsstat: symbol not defined\n");
		return;
	}
	if (interval) {
		sidewaysintpr((unsigned)interval, nfsstataddr);
		return;
	}

	if (kmem >= 0) {
		klseek(kmem, nfsstataddr, 0);
		read(kmem, (char *)&nfsstats, sizeof(struct nfsstats));
	} else {
		if (table(TBL_NFSSTAT, 0, (char *)&nfsstats, 1,
			sizeof(struct nfsstats)) != 1) {
			perror("table");
			exit(-1);
		}
	}

	printf("Client Info:\n");
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"Getattr", "Setattr", "Lookup", "Readlink", "Read",
		"Write", "Create", "Remove");
	printf("%9d %9d %9d %9d %9d %9d %9d %9d\n",
		nfsstats.rpccnt[1],
		nfsstats.rpccnt[2],
		nfsstats.rpccnt[4],
		nfsstats.rpccnt[5],
		nfsstats.rpccnt[6],
		nfsstats.rpccnt[8],
		nfsstats.rpccnt[9],
		nfsstats.rpccnt[10]);
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"Rename", "Link", "Symlink", "Mkdir", "Rmdir",
		"Readdir", "Statfs");
	printf("%9d %9d %9d %9d %9d %9d %9d\n",
		nfsstats.rpccnt[11],
		nfsstats.rpccnt[12],
		nfsstats.rpccnt[13],
		nfsstats.rpccnt[14],
		nfsstats.rpccnt[15],
		nfsstats.rpccnt[16],
		nfsstats.rpccnt[17]);
	printf("%11.11s %11.11s %11.11s %11.11s %11.11s\n",
		"Rpc requsts", "Rpc retries", "Rpc tmouts",
		"Rpc unexp", "Rpc invalid");
	printf("%11d %11d %11d %11d %11d\n",
		nfsstats.rpcrequests, nfsstats.rpcretries,
		nfsstats.rpctimeouts, nfsstats.rpcunexpected,
		nfsstats.rpcinvalid);
	printf("Cache Info:\n");
	printf("%9.9s %9.9s %9.9s %9.9s",
		"Attr Hits", "Misses", "Lkup Hits", "Misses");
	printf(" %9.9s %9.9s %9.9s %9.9s\n",
		"BioR Hits", "Misses", "BioW Hits", "Misses");
	printf("%9d %9d %9d %9d",
		nfsstats.attrcache_hits, nfsstats.attrcache_misses,
		nfsstats.lookupcache_hits, nfsstats.lookupcache_misses);
	printf(" %9d %9d %9d %9d\n",
		nfsstats.biocache_reads-nfsstats.read_bios,
		nfsstats.read_bios,
		nfsstats.biocache_writes-nfsstats.write_bios,
		nfsstats.write_bios);
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"RDir Hits", "Misses", "RLnk Hits", "Misses",
		"EDir Hits", "Misses");
	printf("%9d %9d %9d %9d %9d %9d\n",
		nfsstats.biocache_readdirs-nfsstats.readdir_bios,
		nfsstats.readdir_bios,
		nfsstats.biocache_readlinks-nfsstats.readlink_bios,
		nfsstats.readlink_bios,
		nfsstats.direofcache_hits,
		nfsstats.direofcache_misses);
	printf("Server Info:\n");
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"Getattr", "Setattr", "Lookup", "Readlink", "Read",
		"Write", "Create", "Remove");
	printf("%9d %9d %9d %9d %9d %9d %9d %9d\n",
		nfsstats.srvrpccnt[1],
		nfsstats.srvrpccnt[2],
		nfsstats.srvrpccnt[4],
		nfsstats.srvrpccnt[5],
		nfsstats.srvrpccnt[6],
		nfsstats.srvrpccnt[8],
		nfsstats.srvrpccnt[9],
		nfsstats.srvrpccnt[10]);
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"Rename", "Link", "Symlink", "Mkdir", "Rmdir",
		"Readdir", "Statfs");
	printf("%9d %9d %9d %9d %9d %9d %9d\n",
		nfsstats.srvrpccnt[11],
		nfsstats.srvrpccnt[12],
		nfsstats.srvrpccnt[13],
		nfsstats.srvrpccnt[14],
		nfsstats.srvrpccnt[15],
		nfsstats.srvrpccnt[16],
		nfsstats.srvrpccnt[17]);
	printf("%17.17s %13.13s\n", "Server Rpc Errors", "Server Faults");
	printf("%17d %13d\n", nfsstats.srvrpc_errs, nfsstats.srv_errs);
	printf("Server Cache Stats:\n");
	printf("%9.9s %9.9s %9.9s %9.9s %9.9s\n",
		"Inprog", "Idem", "Non-idem", "Misses", "Drops");
	printf("%9d %9d %9d %9d %9d\n",
		nfsstats.srvcache_inproghits,
		nfsstats.srvcache_idemdonehits,
		nfsstats.srvcache_nonidemdonehits,
		nfsstats.srvcache_misses,
		nfsstats.srvcache_reqdrops);
}

/*
 * Called if an interval expires before sidewaysintpr has completed a loop.
 * Sets a flag to not wait for the alarm.
 */
u_char	signalled;			/* set if alarm goes off "early" */
void
catchalarm()
{
	signalled = YES;
}
/*
 * Print a running summary of nfs statistics.
 * Repeat display every interval seconds, showing statistics
 * collected over that interval.  Assumes that interval is non-zero.
 * First line printed at top of screen is always cumulative.
 */
sidewaysintpr(interval, off)
	unsigned interval;
	off_t off;
{
	struct nfsstats nfsstats, lastst;
	register int line;
	int oldmask;

	(void)signal(SIGALRM, catchalarm);
	signalled = NO;
	(void)alarm(interval);
	bzero((caddr_t)&lastst, sizeof(lastst));
banner:
	printf("        %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s %8.8s\n",
		"Getattr", "Lookup", "Readlink", "Read",
		"Write", "Rename", "Link", "Readdir");
	fflush(stdout);
	line = 0;
loop:
	if (kmem >= 0) {
		klseek(kmem, off, 0);
		read(kmem, (char *)&nfsstats, sizeof(struct nfsstats));
	} else {
		if (table(TBL_NFSSTAT, 0, (char *)&nfsstats, 1,
			sizeof(struct nfsstats)) != 1) {
			perror("table");
			exit(-1);
		}
	}
	printf("Client: %8d %8d %8d %8d %8d %8d %8d %8d\n",
		nfsstats.rpccnt[1]-lastst.rpccnt[1],
		nfsstats.rpccnt[4]-lastst.rpccnt[4],
		nfsstats.rpccnt[5]-lastst.rpccnt[5],
		nfsstats.rpccnt[6]-lastst.rpccnt[6],
		nfsstats.rpccnt[8]-lastst.rpccnt[8],
		nfsstats.rpccnt[11]-lastst.rpccnt[11],
		nfsstats.rpccnt[12]-lastst.rpccnt[12],
		nfsstats.rpccnt[16]-lastst.rpccnt[16]);
	printf("Server: %8d %8d %8d %8d %8d %8d %8d %8d\n",
		nfsstats.srvrpccnt[1]-lastst.srvrpccnt[1],
		nfsstats.srvrpccnt[4]-lastst.srvrpccnt[4],
		nfsstats.srvrpccnt[5]-lastst.srvrpccnt[5],
		nfsstats.srvrpccnt[6]-lastst.srvrpccnt[6],
		nfsstats.srvrpccnt[8]-lastst.srvrpccnt[8],
		nfsstats.srvrpccnt[11]-lastst.srvrpccnt[11],
		nfsstats.srvrpccnt[12]-lastst.srvrpccnt[12],
		nfsstats.srvrpccnt[16]-lastst.srvrpccnt[16]);
	lastst = nfsstats;
	fflush(stdout);
	line++;
	oldmask = sigblock(sigmask(SIGALRM));
	if (! signalled) {
		sigpause(0);
	}
	sigsetmask(oldmask);
	signalled = NO;
	(void)alarm(interval);
	if (line == 21)
		goto banner;
	goto loop;
	/*NOTREACHED*/
}
