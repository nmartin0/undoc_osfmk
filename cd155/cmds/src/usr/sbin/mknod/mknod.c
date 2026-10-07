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
 
/* @(#)mknod.c	4.1 15:11:03 7/9/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#ifndef lint
static char *sccsid = "@(#)mknod.c	4.2 (Berkeley) 8/28/85";
#endif not lint

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: mknod.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:09:12 $";
#endif

#include <stdio.h>
#include <sys/stat.h>
#include <sys/secdefines.h>

#ifdef NLS
#include <locale.h>
#endif

#ifdef MSG
#include "mknod_msg.h"
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_MKNOD,n,s)
#ifdef SEC_BASE
#define MSGSTR_SEC(n,s) catgets(catd,MS_MKNOD_SEC,n,s)
#endif
#else
#define MSGSTR(n,s) s
#endif

#if defined(KJI) || defined(NLS)
#include <NLchar.h>
#include <NLctype.h>
#endif

#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

main(argc, argv)
	int argc;
	char **argv;
{
	int m, a, b;

	setlocale( LC_ALL, "" );
	catd = catopen(MF_MKNOD,0);

	if(argc == 3) {
		if (strcmp(argv[2],"p") != 0)
			goto usage;
		if(mkfifo(argv[1], 0666) < 0) {
			fprintf(stderr, "mkfifo: ");
			perror(argv[1]);
			exit(1);
		}
		exit(0);

	} else if(argc != 5) {
		fprintf(stderr, MSGSTR(ARGCOUNT, "arg count\n"));
		goto usage;
	}
	if(*argv[2] == 'b')
		m = 060666; else
	if(*argv[2] == 'c')
		m = 020666; else
		goto usage;
	a = number(argv[3]);
	if(a < 0)
		goto usage;
	b = number(argv[4]);
	if(b < 0)
		goto usage;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
	if (!authorized_user("mknod")) {
		fprintf(stderr, MSGSTR_SEC(AUTH,
			"%s: need mknod authorization\n"), command_name);
		exit(1);
	}
	if (forceprivs(privvec(SEC_MKNOD
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
				-1), (priv_t *) 0) ||
            enableprivs(privvec(SEC_ALLOWDACACCESS,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), (priv_t *) 0)) {
		fprintf(stderr,
			MSGSTR_SEC(PRIV, "%s: insufficient privileges\n"),
			command_name);
		exit(1);
	}
	disablepriv(SEC_SUSPEND_AUDIT);
#endif /* SEC_BASE */
	if(mknod(argv[1], m, (a<<8)|b) < 0) {
		fprintf(stderr, "mknod: ");
		perror(argv[1]);
		exit(1);
	}
	exit(0);

usage:
	fprintf(stderr, MSGSTR(USAGE,
		"Usage: mknod specialfile b/c major minor\nor mknod fifo p\n"));
	exit(1);
}

number(s)
char *s;
{
	int n, c;

	n = 0;
	while(c = *s++) {
		if(c<'0' || c>'9')
			return(-1);
		n = n*10 + c-'0';
	}
	return(n);
}
