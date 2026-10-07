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
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Rick Macklem at The University of Guelph.
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
"@(#) Copyright (c) 1989 Regents of the University of California.\n\
 All rights reserved.\n";
#endif not lint

#ifndef lint
static char sccsid[] = "@(#)nfsd.c	5.1 (Berkeley) 7/16/89";
#endif not lint

#ifdef TNC
#define OSF1_ADFS	/* needed for node_t in types.h */
#include <cthreads.h>
#endif
#include <sys/types.h>
#include <sys/signal.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <netdb.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include <rpc/pmap_prot.h>
#include <nfs/rpcv2.h>
#include <nfs/nfsv2.h>
#if	UNIX_DOMAIN
#include <sys/un.h>
#define NFSD_PATH_NAME "/tmp/.nfsd"
#endif	/* UNIX_DOMAIN */

/* Global defs */
#ifdef DEBUG
#define	syslog(e, s)	fprintf(stderr,(s))
int debug = 1;
#else
int debug = 0;
#endif

/*
 * Nfs server daemon mostly just a user context for nfssvc()
 * 1 - do file descriptor and signal cleanup
 * 2 - create server socket
 * 3 - register socket with portmap
 * 4 - nfssvc(sock, msk, mtch)
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	register int i;
	int cnt, sock;
#if	UNIX_DOMAIN
	struct sockaddr_un saddr_un;
#else
	struct sockaddr_in saddr;
#endif	/* UNIX_DOMAIN */
	u_long msk, mtch;

#ifdef TNC
	int	val;
	int 	rc;
	node_t	mynode;

	/* Fork a copy of ourselves on each configured
 	 * network server in the system.
	 */
	rc = tnc_clone_daemon();
	if (rc == 1) 
		/* 
		 * We're the parent and everything worked.
		 * Our work is done so exit and let the
		 * child processes take over.
		 */
		exit(0);	
	else if (rc < 0)
		/* 
		 * We're either the parent or a child and 
		 * something failed. Errors have been logged
		 * in tnc_clone_daemon().
		 */
		exit(1);
	/*
	 * If we get this far, we're a child process who
	 * has been successfully rforked.
	 */
#endif

#if	!UNIX_DOMAIN
	if (debug == 0) {
		daemon(0, 0);
		signal(SIGINT, SIG_IGN);
		signal(SIGQUIT, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		signal(SIGHUP, SIG_IGN);
	}
#endif	/* UNIX_DOMAIN */
	openlog("nfsd:", LOG_PID, LOG_DAEMON);
#if	UNIX_DOMAIN
	/*
	 * we need to change this so it's more flexible and can accept
	 * other domains as well.  At least make it so that one nfsd 
	 * call can listen on both INET and UNIX domains.
	 */
	if ((sock = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "Can't create socket");
		exit(1);
	}
	saddr_un.sun_family = AF_UNIX;
	strcpy(saddr_un.sun_path, NFSD_PATH_NAME);
	saddr_un.sun_len = 0;
	sizeof(u_char) + 1;
	if (bind(sock, &saddr_un, strlen(saddr_un.sun_path) +
		 sizeof(u_char) + sizeof(u_char) + 1) < 0) {
		syslog(LOG_ERR, "Can't bind addr - Unix Domain");
		exit(1);
	}
#else
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		syslog(LOG_ERR, "Can't create socket");
		exit(1);
	}
	bzero((char *)&saddr, sizeof(struct sockaddr_in));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = INADDR_ANY;
	saddr.sin_port = htons(NFS_PORT);
#ifdef TNC
	val = 1;
	if (setsockopt(sock, SOL_SOCKET, VS_BINDLOCAL, (char*)&val, 
		       sizeof(int)) < 0) {
		syslog(LOG_ERR, "cannot setsockopt.");
		exit(1);
	}	  
#endif
	if (bind(sock, &saddr, sizeof(saddr)) < 0) {
		syslog(LOG_ERR, "Can't bind addr - Inet domain");
		exit(1);
	}
	pmap_unset(RPCPROG_NFS, NFS_VER2);
	if (!pmap_set(RPCPROG_NFS, NFS_VER2, IPPROTO_UDP, NFS_PORT)) {
		syslog(LOG_ERR, "Can't register with portmap");
		exit(1);
	}
#endif	/* UNIX_DOMAIN */
	if (argc == 2) {
		if ((cnt = atoi(argv[1])) <= 0 || cnt > 20)
			cnt = 1;
		msk = 0;
		mtch = 0;
	} else if (argc == 4) {
		if ((cnt = atoi(argv[1])) <= 0 || cnt > 20)
			cnt = 1;
		msk = inet_addr(argv[2]);
		mtch = inet_addr(argv[3]);
	} else {
		cnt = 1;
		msk = 0;
		mtch = 0;
	}
#ifdef TNC
	/*
	 * Get pid number, right shift 16 to extract
	 * node number
	 */
	mynode = getpid() >> 16;
	for (i = 1; i < cnt; i++)
		if (rfork(mynode) == 0)
			break;
#else
	for (i = 1; i < cnt; i++)
		if (fork() == 0)
			break;
#endif
	if (nfssvc(sock, msk, mtch) < 0)	/* Only returns on error */
		syslog(LOG_ERR, "nfssvc() failed %m");
	exit();
}
