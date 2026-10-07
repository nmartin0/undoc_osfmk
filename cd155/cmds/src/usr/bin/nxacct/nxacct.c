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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/*
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *
 *      Copyright 1992  Intel Corporation.
 *
 *      $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/bin/nxacct/nxacct.c,v 1.2 1994/11/19 01:32:36 mtm Exp $
 *
 * HISTORY
 * $Log: nxacct.c,v $
 * Revision 1.2  1994/11/19  01:32:36  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/10/01  18:37:37  rkl
 * Initial revision
 *
 *
 */

#include <sys/secdefines.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <nx/nxacct.h>
#include <locale.h>

#if SEC_BASE
#include <sys/security.h>

extern priv_t *privvec();
#endif

#define PATH	"PATH=:/bin:/usr/bin"
#define SUPATH	"PATH=:/bin:/etc:/usr/bin"
#define ELIM	128

char	PW[] = "Password: ";
char	NG[] = "Sorry";
char	PD[] = "Permission denied";
char	UA[] = "Unknown account";
char	NS[] = "You have no shell";

struct	nxacct *nx_getanam();
struct	passwd *getpwnam();
char	*cuserid();
char	*getpass();
char	*crypt();

char homedir[64]="HOME=";
char logname[20]="LOGNAME=";


char *envinit[ELIM];
extern char **environ;
char *path=PATH;
char *supath=SUPATH;


main(argc,argv)
char *argv[];
{
	register struct passwd *p;
	char *rname();
	int eflag = 0;
	int uid;
	acct_t	acct;
	char *shell, *dir, *name;
#if SEC_BASE
	privvec_t	saveprivs;
#endif


        (void) setlocale(LC_ALL,"");
#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
#ifdef	DEBUG
	chroot(".");
#endif
#if SEC_BASE
	if ((p = getpwuid(getluid())) == NULL)
#else
	if ((p = getpwuid(getuid())) == NULL)
#endif
		error(NG);
	endpwent();
	if(argc > 1 && *argv[1] == '-'){
		eflag++;
		argv++;
		--argc;
	}

	if (argc == 1) {
		struct nxacct *a;
		if (a = nx_getaid(nx_getacctid())) {
			printf("%s\n", a->acct_name);
			exit (0);
		} else
			error(UA);
			exit;
	}

	acct = chkacct(argv[1], p);
	uid  = p->pw_uid;
	dir  = strcpy(malloc(strlen(p->pw_dir)+1),p->pw_dir);
	name = strcpy(malloc(strlen(p->pw_name)+1),p->pw_name);
#if SEC_BASE
	if (forceprivs(privvec(SEC_SETPROCIDENT, -1), saveprivs))
		error("nxacct: insufficient privileges");
#endif
	if (nx_setacctid(acct) < 0 || setuid(getuid()) < 0)
		error(NG);
#if SEC_BASE
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	if (!*p->pw_shell)
	{
		p->pw_shell = getenv("SHELL");
	}
	if(eflag){
		char *simple;

		strcat(homedir, dir);
		strcat(logname, name);
		envinit[2] = logname;
		chdir(dir);
		envinit[0] = homedir;
		if (uid == 0)
			envinit[1] = supath;
		else
			envinit[1] = path;
		envinit[3] = NULL;
		environ = envinit;
		shell = strcpy(malloc(sizeof(p->pw_shell + 2)), "-");
		shell = strcat(shell,p->pw_shell);
		simple = strrchr(shell,'/');
		if(simple){
			*(shell+1) = '\0';
			shell = strcat(shell,++simple);
		}
	}
	else	shell = p->pw_shell;

	execl(p->pw_shell,shell, NULL);
	error(NS);
}

warn(s)
char *s;
{
	fprintf(stderr, "%s\n", s);
}

error(s)
char *s;
{
	warn(s);
	exit(1);
}

chkacct(aname, p)
char	*aname;
struct	passwd *p;
{
	register char **t;
	register struct nxacct *a;

	a = nx_getanam(aname);
	nx_endaent();
	if (a == NULL) {
		warn(UA);
		return -1;
	}
#ifndef SEC_BASE
	if (getuid() == 0)
		return a->acct_id;
#endif
	for (t = a->acct_mem; *t; ++t) {
		if ((**t == '!') && (strcmp(p->pw_name, ((*t)+1)) == 0))
			error(NG);
		if ((strcmp(p->pw_name, *t) == 0) || (strcmp(*t, "*") == 0))
			return a->acct_id;
	}

	if (*a->acct_passwd) {
		if (!isatty(fileno(stdin)))
			error(PD);
#if SEC_BASE
		if (nxacct_good_password(a->acct_passwd, PW))
#else
		if (strcmp(a->acct_passwd, crypt(getpass(PW), a->acct_passwd)) == 0)
#endif
			return a->acct_id;
	}
	warn(NG);
	return -1;
}
/*
 * return pointer to rightmost component of pathname
 */
char *rname(pn)
char *pn;
{
	register char *q;

	q = pn;
	while (*pn)
		if (*pn++ == '/')
			q = pn;
	return q;
}
