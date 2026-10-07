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
 
/* @(#)umount.c	3.2 09:40:13 6/18/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * Copyright (c) 1980, 1989 The Regents of the University of California.
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

/*
 * HISTORY:
 * $Log: umount.c,v $
 * Revision 1.6  1995/01/13  17:57:59  shane
 *  Reviewer: Stan Smith / Shane Story
 *  Risk: Low
 *  Benefit or PTS #: 12020
 *  Testing: Ran and compared the new umount command with the one
 *           already on the system.
 *
 *  Module(s): umount.c
 *
 * Revision 1.5  1994/11/19  03:18:41  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/07/21  20:24:33  yazz
 *  Reviewer: Shala Arshi, Bob Yasi
 *  Risk: lo
 *  Benefit or PTS #: 10127 C-0
 *  Testing: Testcase now passes; also passes at customer site
 *  Module(s): /src/usr/sbin/umount/umount.c, /src/usr/sbin/umount/umount.c
 *
 * Always zero out the sockaddr_in structure before using, lest random stack
 * garbage foul route processing.
 *
 * Revision 1.3  1993/04/22  15:06:43  stans
 *   "-vat nfs" was not working at all. Cleaned up code/design to make it
 *   work.
 *
 */

#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1980, 1989 The Regents of the University of California.\n\
 All rights reserved.\n";
#endif /* not lint */

#ifndef lint
static char sccsid[] = "@(#)umount.c	5.14 (Berkeley) 6/1/90";
#endif /* not lint */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <sys/audit.h>
#include <prot.h>
#endif
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/mount.h>

#ifdef NFS
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_prot.h>
#include <nfs/rpcv2.h>
#endif

#include <fstab.h>
#include <stdio.h>
#include <string.h>

#ifdef NFS
/* 
 * need our own version of authunix_create_default() to limit # groups 
 * RPC can't handle more than 8
 */
AUTH *authunix_default(); /* need our own version to limit # groups */
int xdr_dir();
char *nfshost;
#endif

int	vflag, all, errs, fake, All;
int	fflag = MNT_NOFORCE;
char	*getmntname();

#define	MNTON	1
#define	MNTFROM	2
#define	MNTONON 3
#define	MNTTYPE 4

int *typelist, *maketypelist();

main(argc, argv)
	int argc;
	char **argv;
{
	extern char *optarg;
	extern int optind;
	int ch;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	sync();
	while ((ch = getopt(argc, argv, "AafFh:t:v")) != EOF)
		switch((char)ch) {
		case 'v':
			vflag++;
			break;
		case 'f':
			fflag = MNT_FORCE;
			break;
		case 'F':
			fake++;
			break;
		case 'A':
			All++;
			break;
		case 'a':
			all++;
			break;
		case 't':
			typelist = maketypelist(optarg);
			break;
#ifdef	NFS
		case 'h':
			/* -h flag implies -a, and "-t nfs" if no -t flag */
			nfshost = optarg;
			all++;
			if (typelist == NULL)
				typelist = maketypelist("nfs");
			break;
#endif /* NFS */
		case '?':
		default:
			usage();
			/* NOTREACHED */
		}
	argc -= optind;
	argv += optind;

	if (argc == 0 && !(all || All))
		usage();
#if SEC_BASE
	umount_checkauth();
#endif
	if (All) {
		if (argc > 0)
			usage();
		exit( Umountall(typelist) );
	}
	if (all) {
		if (argc > 0)
			usage();
		if (setfsent() == 0)
			perror(FSTAB), exit(1);
		umountall(typelist);
		exit(0);
	} else
		setfsent();
	while (argc > 0) {
		if (umountfs(*argv++, 0) == 0)
			errs++;
		argc--;
	}
	exit(errs);
}

usage()
{
	fprintf(stderr,
		"%s\n%s\n",
		"Usage: umount [-fv] special | node",
#ifndef	NFS
		"    or umount -a[fv] [-t fstypelist]"
#else
		"    or umount -a[fv] [-h host] [-t fstypelist]"
#endif
	      );
	exit(1);
}

/*
 * unmount all filesystems identified as mounted. 
 */

Umountall(typelist)
	int *typelist;
{
	struct statfs *mntbuf;
	int i, mntsize, err = 0;

	if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) == 0) {
		fprintf(stderr, "umount: cannot get mount information\n");
		exit(1);
	}
	for (i = mntsize - 1; i; i--) {
		/*
		 * Do type check here, since umountfs may try to
		 * do something that could hang on a troublesome
		 * filesystem.
		 */
		if (badtype(mntbuf[i].f_type, typelist))
			continue;
		if ( umountfs(mntbuf[i].f_mntonname, typelist) == 0 )
			err++;
	}
	return (err ? 1 : 0);
}

umountall(typelist)
	int *typelist;
{
	register struct fstab *fs;
	int *tlist, rc;
	struct fstab *allocfsent();

	if ((fs = getfsent()) == (struct fstab *)0)
		return;

	fs = allocfsent(fs);

	umountall(typelist);

	if (strcmp(fs->fs_file, "/") == 0) {
		freefsent(fs);
		return;
	}

	if (strcmp(fs->fs_type, FSTAB_RW) &&
	    strcmp(fs->fs_type, FSTAB_RO) &&
	    strcmp(fs->fs_type, FSTAB_RQ)) {
		freefsent(fs);
		return;
	}
	/*
	 * Do type check here, since umountfs may try to
	 * do something that could hang on a troublesome
	 * filesystem.
	 */
	tlist = maketypelist(fs->fs_vfstype);

	if ( ! badtype(*tlist, typelist) )
		(void) umountfs(fs->fs_file, 0);

	free( tlist );	/* clean what was malloc'ed in maketypelist() */
	freefsent(fs);
}

struct fstab *
allocfsent(fs)
	register struct fstab *fs;
{
	register struct fstab *new;
	register char *cp;
	char *malloc();

	new = (struct fstab *)malloc((unsigned)sizeof(struct fstab));

	new->fs_file = strdup(fs->fs_file);
	new->fs_type = strdup(fs->fs_type);
	new->fs_spec = strdup(fs->fs_spec);

	new->fs_passno = fs->fs_passno;
	new->fs_freq = fs->fs_freq;

	new->fs_vfstype = strdup(fs->fs_vfstype);

	return (new);
}

freefsent(fs)
	register struct fstab *fs;
{

	if (fs->fs_file)
		free(fs->fs_file);
	if (fs->fs_spec)
		free(fs->fs_spec);
	if (fs->fs_type)
		free(fs->fs_type);
	if (fs->fs_vfstype)
		free(fs->fs_vfstype);
	free((char *)fs);
}

/*
 * unmount a fileSystem
 *
 * inputs:
 *	name		filesystem name string pointer
 *	typelist	list of filesystem types.
 *
 * outputs:
 *	0 == fail
 *	1 == success
 *
 * side effects:
 */
umountfs(name, typelist)
	char *name;
	int *typelist;
{
	char *mntpt;
	struct stat stbuf;
	int type;
#ifdef NFS
	register CLIENT *clp;
	struct hostent *hp = 0;
	struct sockaddr_in saddr;
	struct timeval pertry, try;
	enum clnt_stat clnt_stat;
	int so = RPC_ANYSOCK;
	char *hostp, *delimp;
#endif /* NFS */

	if (stat(name, &stbuf) < 0) {
		/*
		 * Could be either a non-pathname argument or a
		 * remote filesystem with a stale handle.
		 */
		if ((mntpt = getmntname(name, MNTON, &type, 0)) == 0)
			if ((mntpt = getmntname(name, MNTONON, &type, 1)) == 0)
				return (0);
	} else if ((stbuf.st_mode & S_IFMT) == S_IFBLK) {
		if ((mntpt = getmntname(name, MNTON, &type, 1)) == 0)
			return (0);
	} else if ((stbuf.st_mode & S_IFMT) == S_IFDIR) {
		mntpt = name;
		if ((name = getmntname(mntpt, MNTFROM, &type, 1)) == 0)
			return (0);
	} else {
		fprintf(stderr, "%s: not a directory or special device\n",
			name);
		return (0);
	}

	if (badtype(type, typelist))
		return(1);
#ifdef NFS
	if ((delimp = index(name, '@')) != NULL) {
		hostp = delimp + 1;
		*delimp = '\0';
		hp = gethostbyname(hostp);
		*delimp = '@';
	} else if ((delimp = index(name, ':')) != NULL) {
		*delimp = '\0';
		hostp = name;
		hp = gethostbyname(hostp);
		name = delimp+1;
		*delimp = ':';
	}

	if (!namematch(hp, nfshost))
		return(1);
#endif	/* NFS */
#if SEC_BASE
	if (!fake && umount_do_umount(mntpt, fflag) < 0)
#else
	if (!fake && umount(mntpt, fflag) < 0)
#endif
	{
		perror(mntpt);
		return (0);
	}
	if (vflag)
		fprintf(stderr, "%s: Unmounted from %s\n", name, mntpt);

#ifdef	NFS
	if (!fake && hp != NULL && (fflag & MNT_FORCE) == 0) {
		*delimp = '\0';
		bzero( (char *)&saddr, sizeof(struct sockaddr_in) );
		bcopy(hp->h_addr,(caddr_t)&saddr.sin_addr,hp->h_length);
		saddr.sin_family = AF_INET;
		saddr.sin_port = 0;
		pertry.tv_sec = 3;
		pertry.tv_usec = 0;
		if ((clp = clntudp_create(&saddr, RPCPROG_MNT, RPCMNT_VER1,
		    pertry, &so)) == NULL) {
			clnt_pcreateerror("Cannot MNT PRC");
			return (1);
		}
#ifdef notyet
/*
 * should be able to use the REAL one when we get up to rpc 4.0
 */
		clp->cl_auth = authunix_create_default();
#else
		clp->cl_auth = authunix_default();
#endif
		try.tv_sec = 20;
		try.tv_usec = 0;
		clnt_stat = clnt_call(clp, RPCMNT_UMOUNT, xdr_dir, name,
			xdr_void, (caddr_t)0, try);
		if (clnt_stat != RPC_SUCCESS) {
			clnt_perror(clp, "Bad MNT RPC");
			return (1);
		}
		auth_destroy(clp->cl_auth);
		clnt_destroy(clp);
	}
#endif /* NFS */
	return (1);
}

char *
getmntname(name, what, type, perr)
	char *name;
	int what;
	int *type;
	int perr;
{
	int mntsize, i;
	struct statfs *mntbuf;

	if ((mntsize = getmntinfo(&mntbuf, MNT_NOWAIT)) == 0) {
		perror("umount");
		return (0);
	}
	for (i = 0; i < mntsize; i++) {
		if (what == MNTON && !strcmp(mntbuf[i].f_mntfromname, name)) {
			if (type)
				*type = mntbuf[i].f_type;
			return (mntbuf[i].f_mntonname);
		}
		if (what == MNTFROM && !strcmp(mntbuf[i].f_mntonname, name)) {
			if (type)
				*type = mntbuf[i].f_type;
			return (mntbuf[i].f_mntfromname);
		}
		if (what == MNTONON && !strcmp(mntbuf[i].f_mntonname, name)) {
			if (type)
				*type = mntbuf[i].f_type;
			return (mntbuf[i].f_mntonname);
		}
	}
	if (perr)
		fprintf(stderr, "%s: not currently mounted\n", name);
	return (0);
}

static int skipvfs;

badtype(type, typelist)
	int type;
	int *typelist;
{
	if (typelist == 0)
		return(0);
	while (*typelist) {
		if (type == *typelist)
			return(skipvfs);
		typelist++;
	}
	return(!skipvfs);
}

int *
maketypelist(fslist)
	char *fslist;
{
	register char *nextcp;
	register int *av, i;
	char *malloc();

	if (fslist == NULL)
		return(NULL);
	if (fslist[0] == 'n' && fslist[1] == 'o') {
		fslist += 2;
		skipvfs = 1;
	} else
		skipvfs = 0;
	for (i = 0, nextcp = fslist; *nextcp; nextcp++)
		if (*nextcp == ',')
			i++;
	av = (int *)malloc((i+2) * sizeof(int));
	if (av == NULL)
		return(NULL);
	for (i = 0; fslist; fslist = nextcp) {
		if (nextcp = index(fslist, ','))
			*nextcp++ = '\0';
		if (strcmp(fslist, "ufs") == 0)
			av[i++] = MOUNT_UFS;
#ifdef	PFS
		else if (strcmp(fslist, "pfs") == 0)
			av[i++] = MOUNT_PFS;
#endif
		else if (strcmp(fslist, "nfs") == 0)
			av[i++] = MOUNT_NFS;
		else if (strcmp(fslist, "mfs") == 0)
			av[i++] = MOUNT_MFS;
		else if (strcmp(fslist, "pc") == 0)
			av[i++] = MOUNT_PC;
	}
	av[i++] = 0;

	return(av);
}

#ifdef	NFS
namematch(hp, nfshost)
	struct hostent *hp;
	char *nfshost;
{
	register char *cp;
	register char **np;

	if (hp == NULL || nfshost == NULL)
		return(1);
	if (strcasecmp(nfshost, hp->h_name) == 0)
		return(1);
	if (cp = index(hp->h_name, '.')) {
		*cp = '\0';
		if (strcasecmp(nfshost, hp->h_name) == 0)
			return(1);
	}
	for (np = hp->h_aliases; *np; np++) {
		if (strcasecmp(nfshost, *np) == 0)
			return(1);
		if (cp = index(*np, '.')) {
			*cp = '\0';
			if (strcasecmp(nfshost, *np) == 0)
				return(1);
		}
	}
	return(0);
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
