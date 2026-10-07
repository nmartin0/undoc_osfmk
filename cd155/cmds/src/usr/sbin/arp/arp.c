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
 * $Log: arp.c,v $
 * Revision 1.3  1994/11/19  03:04:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/12/10  22:56:35  shala
 * Support the new table calls.
 *
 * Revision 2.2  92/11/18  14:33:31  loverso
 * 	Add use of table for OSF/1 AD.
 * 
 * Revision 2.7  90/10/07  22:04:28  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/29  14:26:58  gm]
 * 
 * Revision 2.6  90/07/27  11:16:38  devrcs
 * 	Update to BSD Reno release.
 * 	[90/07/17  18:35:49  tmt]
 * 
 * 	Security hooks from SecureWare.
 * 	[90/07/19  21:35:04  seiden]
 * 
 * Revision 2.5  90/06/22  22:08:41  devrcs
 * 	Grok new ARP flags.
 * 	[90/06/18  10:52:13  tmt]
 * 
 * Revision 2.4  90/03/27  20:03:49  gm
 * 	Working version against new Posix includes.
 * 	[90/03/14  16:41:01  tmt]
 * 
 * 	Update to 4.4 alpha release and OSF/1.
 * 	[90/03/06  13:01:40  tmt]
 * 
 * Revision 2.3  90/01/02  18:41:42  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  08:54:47  gm
 * 	Fix dump() to deal with new "valloc'd" arptab.
 * 	[89/01/14            parker]
 * 
 * $EndLog$
 */
/* @(#)arp.c	4.1 10:10:00 7/13/90 SecureWare */
/*
 * Copyright (c) 1984 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Sun Microsystems, Inc.
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
"@(#) Copyright (c) 1984 Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)arp.c	5.11 (Berkeley) 6/1/90";
#endif /* not lint */

/*
 * arp - display, set, and delete arp table entries
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>

extern priv_t *privvec();
static privvec_t saveprivs;
#endif

#if	!MACH
#include <machine/pte.h>

#include <sys/param.h>
#include <sys/vmmac.h>
#else
#include <sys/param.h>
#endif
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <net/if.h>
#include <netinet/if_ether.h>

#include <errno.h>
#include <nlist.h>
#include <stdio.h>
#include <paths.h>

#define TABLE_ARPTAB
#include <sys/table.h>

extern int errno;
static int kflag;

main(argc, argv)
	int argc;
	char **argv;
{
	int ch;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif

	while ((ch = getopt(argc, argv, "adsf")) != EOF)
		switch((char)ch) {
		case 'a': {
			char *mem;

			if (argc > 4)
				usage();
			if (argc == 4) {
				kflag = 1;
				mem = argv[3];
			}
			else
				mem = _PATH_KMEM;
			if (argc < 3)
				table_dump();
			else
				dump((argc >= 3) ? argv[2] : _PATH_UNIX, mem);
			exit(0);
		}
		case 'd':
			if (argc != 3)
				usage();
			delete(argv[2]);
			exit(0);
		case 's':
			if (argc < 4 || argc > 7)
				usage();
			exit(set(argc-2, &argv[2]) ? 1 : 0);
		case 'f':
			if (argc != 3)
				usage();
			exit (file(argv[2]) ? 1 : 0);
		case '?':
		default:
			usage();
		}
	if (argc != 2)
		usage();
	get(argv[1]);
	exit(0);
}

/*
 * Process a file to set standard arp entries
 */
file(name)
	char *name;
{
	FILE *fp;
	int i, retval;
	char line[100], arg[5][50], *args[5];

	if ((fp = fopen(name, "r")) == NULL) {
		fprintf(stderr, "arp: cannot open %s\n", name);
		exit(1);
	}
	args[0] = &arg[0][0];
	args[1] = &arg[1][0];
	args[2] = &arg[2][0];
	args[3] = &arg[3][0];
	args[4] = &arg[4][0];
	retval = 0;
	while(fgets(line, 100, fp) != NULL) {
		i = sscanf(line, "%s %s %s %s %s", arg[0], arg[1], arg[2],
		    arg[3], arg[4]);
		if (i < 2) {
			fprintf(stderr, "arp: bad line: %s\n", line);
			retval = 1;
			continue;
		}
		if (set(i, args))
			retval = 1;
	}
	fclose(fp);
	return (retval);
}

/*
 * Set an individual arp entry 
 */
set(argc, argv)
	int argc;
	char **argv;
{
	struct arpreq ar;
	struct hostent *hp;
	struct sockaddr_in *sin;
	u_char *ea;
	int s;
	char *host = argv[0], *eaddr = argv[1];

	argc -= 2;
	argv += 2;
	bzero((caddr_t)&ar, sizeof ar);
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) {
		if (!(hp = gethostbyname(host))) {
			fprintf(stderr, "arp: %s: ", host);
			herror((char *)NULL);
			return (1);
		}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
	}
	ea = (u_char *)ar.arp_ha.sa_data;
	if (ether_aton(eaddr, ea))
		return (1);
	ar.arp_flags = ATF_PERM;
	while (argc-- > 0) {
		if (strncmp(argv[0], "temp", 4) == 0)
			ar.arp_flags &= ~ATF_PERM;
		else if (strncmp(argv[0], "pub", 3) == 0)
			ar.arp_flags |= ATF_PUBL;
		else if (strncmp(argv[0], "trail", 5) == 0)
			ar.arp_flags |= ATF_USETRAILERS;
		argv++;
	}
	
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("arp: socket");
		exit(1);
	}
#if SEC_BASE
	raiseprivs(saveprivs);
#endif
	if (ioctl(s, SIOCSARP, (caddr_t)&ar) < 0) {
		perror(host);
		exit(1);
	}
#if SEC_BASE
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	close(s);
	return (0);
}

/*
 * Display an individual arp entry
 */
get(host)
	char *host;
{
	struct arpreq ar;
	struct hostent *hp;
	struct sockaddr_in *sin;
	u_char *ea;
	int s;
	char *inet_ntoa();

	bzero((caddr_t)&ar, sizeof ar);
	ar.arp_pa.sa_family = AF_INET;
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) {
		if (!(hp = gethostbyname(host))) {
			fprintf(stderr, "arp: %s: ", host);
			herror((char *)NULL);
			exit(1);
		}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("arp: socket");
		exit(1);
	}
	if (ioctl(s, SIOCGARP, (caddr_t)&ar) < 0) {
		if (errno == ENXIO)
			printf("%s (%s) -- no entry\n",
			    host, inet_ntoa(sin->sin_addr));
		else
			perror("SIOCGARP");
		exit(1);
	}
	close(s);
	ea = (u_char *)ar.arp_ha.sa_data;
	printf("%s (%s) at ", host, inet_ntoa(sin->sin_addr));
	if (ar.arp_flags & ATF_COM)
		ether_print(ea);
	else
		printf("(incomplete)");
	if (ar.arp_flags & ATF_PERM)
		printf(" permanent");
	if (ar.arp_flags & ATF_PUBL)
		printf(" published");
	if (ar.arp_flags & ATF_USETRAILERS)
		printf(" trailers");
#ifdef	ATF_STALE
	if (ar.arp_flags & ATF_USE802)	/* not used */
		printf(" 802.3");
	if (ar.arp_flags & ATF_STALE)
		printf(" stale");
	if (ar.arp_flags & ATF_DEAD)
		printf(" not responding");
#endif
	printf("\n");
}

/*
 * Delete an arp entry 
 */
delete(host)
	char *host;
{
	struct arpreq ar;
	struct hostent *hp;
	struct sockaddr_in *sin;
	int s;

	bzero((caddr_t)&ar, sizeof ar);
	ar.arp_pa.sa_family = AF_INET;
	sin = (struct sockaddr_in *)&ar.arp_pa;
	sin->sin_family = AF_INET;
	sin->sin_addr.s_addr = inet_addr(host);
	if (sin->sin_addr.s_addr == -1) {
		if (!(hp = gethostbyname(host))) {
			fprintf(stderr, "arp: %s: ", host);
			herror((char *)NULL);
			exit(1);
		}
		bcopy((char *)hp->h_addr, (char *)&sin->sin_addr,
		    sizeof sin->sin_addr);
	}
	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		perror("arp: socket");
		exit(1);
	}
#if SEC_BASE
	raiseprivs(saveprivs);
#endif
	if (ioctl(s, SIOCDARP, (caddr_t)&ar) < 0) {
		if (errno == ENXIO)
			printf("%s (%s) -- no entry\n",
			    host, inet_ntoa(sin->sin_addr));
		else
			perror("SIOCDARP");
		exit(1);
	}
#if SEC_BASE
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	close(s);
	printf("%s (%s) deleted\n", host, inet_ntoa(sin->sin_addr));
}

struct nlist nl[] = {
#define	X_ARPTAB	0
	{ "_arptab" },
#define	X_ARPTAB_SIZE	1
	{ "_arptab_size" },
#if	!MACH
#define	N_SYSMAP	2
	{ "_Sysmap" },
#define	N_SYSSIZE	3
	{ "_Syssize" },
#endif
	{ "" },
};

#if	!MACH
static struct pte *Sysmap;
#endif

/*
 * Dump the entire arp table
 */
dump(kernel, mem)
	char *kernel, *mem;
{
	struct arptab *at;
	int mf, arptab_size, sz;
	char *malloc();
	off_t lseek();
#if	MACH
	caddr_t arptabptr;
#endif

	if (nlist(kernel, nl) < 0 || nl[X_ARPTAB_SIZE].n_type == 0) {
		fprintf(stderr, "arp: %s: bad namelist\n", kernel);
		exit(1);
	}
	mf = open(mem, O_RDONLY);
	if (mf < 0) {
		fprintf(stderr, "arp: cannot open %s\n", mem);
		exit(1);
	}
#if	!MACH
	if (kflag) {
		off_t off;

		Sysmap = (struct pte *)
		   malloc((u_int)(nl[N_SYSSIZE].n_value * sizeof(struct pte)));
		if (!Sysmap) {
			fputs("arp: can't get memory for Sysmap.\n", stderr);
			exit(1);
		}
		off = nl[N_SYSMAP].n_value & ~KERNBASE;
		(void)lseek(mf, off, L_SET);
		(void)read(mf, (char *)Sysmap,
		    (int)(nl[N_SYSSIZE].n_value * sizeof(struct pte)));
	}
#endif
	klseek(mf, (long)nl[X_ARPTAB_SIZE].n_value, L_SET);
	read(mf, &arptab_size, sizeof arptab_size);
#if	MACH
	if (arptab_size <= 0) {
#else
	if (arptab_size <= 0 || arptab_size > 1000) {
#endif
		fprintf(stderr, "arp: %s: namelist wrong\n", kernel);
		exit(1);
	}
	sz = arptab_size * sizeof (struct arptab);
	at = (struct arptab *)malloc((u_int)sz);
	if (at == NULL) {
		fputs("arp: can't get memory for arptab.\n", stderr);
		exit(1);
	}
	klseek(mf, (long)nl[X_ARPTAB].n_value, L_SET);
#if	MACH
	read(mf, &arptabptr, sizeof (arptabptr));
	klseek(mf, (long)arptabptr, L_SET);
#endif
	if (read(mf, (char *)at, sz) != sz) {
		perror("arp: error reading arptab");
		exit(1);
	}
	close(mf);
	dumpall(arptab_size, at, NULL);
}

table_dump()
{
	register struct tbl_arptab *ta;
	int arptab_size;
	int rc;

	/*
	 * Get size of arp tabe
	 */
	rc = table(TBL_ARPTAB, 0, NULL, SHRT_MAX, 0);
	if (rc < 0) {
		perror("arp: table: get arp size");
		return;
	}
	arptab_size = rc;
	ta = (struct tbl_arptab *)malloc(sizeof(struct tbl_arptab)*arptab_size);
	if (ta == NULL) {
		printf("arp: could not allocate %d arptab entries\n",
			arptab_size);
		return;
	}
	rc = table(TBL_ARPTAB, 0, (char *)ta, arptab_size,
					sizeof(struct tbl_arptab));
	if (rc < 0) {
		perror("arp: table: get arp entries");
		return;
	}
	arptab_size = rc;		/* in case table shrunk */
	dumpall(arptab_size, NULL, ta);
}

dumpall(arptab_size, arptab, table_arptab)
	int arptab_size;
	struct arptab *arptab;
	struct tbl_arptab *table_arptab;
{
	struct arptab *at;
	extern int h_errno;
	struct hostent *hp;
	int bynumber;
	char *host;

	if (arptab)
		at = arptab - 1;
	for (bynumber = 0; arptab_size-- > 0;) {
		if (arptab)
			at++;
		else
			at = &table_arptab++->t_arptab;
		if (at->at_iaddr.s_addr == 0 || at->at_flags == 0)
			continue;
		if (bynumber == 0)
			hp = gethostbyaddr((caddr_t)&at->at_iaddr,
			    sizeof at->at_iaddr, AF_INET);
		else
			hp = 0;
		if (hp)
			host = hp->h_name;
		else {
			host = "?";
			if (h_errno == TRY_AGAIN)
				bynumber = 1;
		}
		printf("%s (%s) at ", host, inet_ntoa(at->at_iaddr));
		if (at->at_flags & ATF_COM)
			ether_print(at->at_enaddr);
		else
			printf("(incomplete)");
		if (at->at_flags & ATF_PERM)
			printf(" permanent");
		if (at->at_flags & ATF_PUBL)
			printf(" published");
		if (at->at_flags & ATF_USETRAILERS)
			printf(" trailers");
#ifdef	ATF_STALE
		if (at->at_flags & ATF_USE802)	/* not used */
			printf(" 802.3");
		if (at->at_flags & ATF_STALE)
			printf(" stale");
		if (at->at_flags & ATF_DEAD)
			printf(" not responding");
#endif
		printf("\n");
	}
}

/*
 * Seek into the kernel for a value.
 */
klseek(fd, base, off)
	int fd, off;
	off_t base;
{
	off_t lseek();

#if	!MACH
	if (kflag) {	/* get kernel pte */
		base &= ~KERNBASE;
		base = ctob(Sysmap[btop(base)].pg_pfnum) + (base & PGOFSET);
	}
#endif
	(void)lseek(fd, base, off);
}

ether_print(cp)
	u_char *cp;
{
	printf("%x:%x:%x:%x:%x:%x", cp[0], cp[1], cp[2], cp[3], cp[4], cp[5]);
}

ether_aton(a, n)
	char *a;
	u_char *n;
{
	int i, o[6];

	i = sscanf(a, "%x:%x:%x:%x:%x:%x", &o[0], &o[1], &o[2],
					   &o[3], &o[4], &o[5]);
	if (i != 6) {
		fprintf(stderr, "arp: invalid Ethernet address '%s'\n", a);
		return (1);
	}
	for (i=0; i<6; i++)
		n[i] = o[i];
	return (0);
}

usage()
{
	printf("usage: arp hostname\n");
	printf("       arp -a [kernel] [kernel_memory]\n");
	printf("       arp -d hostname\n");
	printf("       arp -s hostname ether_addr [temp] [pub] [trail]\n");
	printf("       arp -f filename\n");
	exit(1);
}

#if SEC_BASE
/*
 * Check user's authorization to modify the ARP table, and raise the
 * required privilege to do it.
 */
raiseprivs(save)
	priv_t *save;
{
	static int	auth_checked = 0;
	privvec_t	needed;

	/*
	 * Only check authorization once to avoid multiple auditing
	 */
	if (!auth_checked) {
		auth_checked = 1;
		if (!authorized_user("sysadmin")) {
			fprintf(stderr, "arp: need sysadmin authorization\n");
			exit(1);
		}
		/* Initialize the needed privilege vector */
		setprivvec(needed, SEC_REMOTE, -1);
	}
	if (forceprivs(needed, save)) {
		fprintf(stderr, "arp: insufficient privileges\n");
		exit(1);
	}
	disablepriv(SEC_SUSPEND_AUDIT);
}
#endif
