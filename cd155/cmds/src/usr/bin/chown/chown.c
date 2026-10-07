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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: chown.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:20:21 $";
#endif
/*
 * COMPONENT_NAME:  (CMDSDAC) security: access control
 *
 * FUNCTIONS: chown
 *
 * ORIGINS: 3 26 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. date 1, date 2
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.

 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * chown.c	1.15  com/cmd/oper,3.1,9013 2/21/90 14:06:43
 * chown.c	4.1 23:49:01 7/12/90 SecureWare 
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <stdio.h>
#include <locale.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef PFS
#include <sys/estat.h>
#else
#include <sys/stat.h>
#endif PFS
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include "chown_msg.h"
nl_catd catd;
#define	MSGSTR(Num, Str) catgets(catd,MS_CHOWN,Num,Str)

extern char *optarg;
extern int optind;

struct	passwd	*pwd;
struct	group	*grp;
#ifdef PFS
struct	estat	stbuf;
#else
struct	stat	stbuf;
#endif PFS
uid_t	newuid;
gid_t   newgid;
int     fflag = 0,Rflag = 0;
int	status = 0;
char	*arg;
char	progname[BUFSIZ];

/*
 * NAME: chown [-frR] uid file ...
 * FUNCTION: changes the owner of the files to uid
 *    FLAGS
 *      -f   Suppresses all error reporting
 *      -R   Causes chgrp to descend recursively through its directory 
 *           arguments, setting the specified group ID.  When symbolic links 
 *           are encountered, their group is changed, but they are not 
 *           traversed.
 */
main(argc, argv)
char *argv[];
{
	register c;

	(void ) setlocale(LC_ALL,"");
	catd = catopen(MF_CHOWN,0);

	strcpy(progname, argv[0]);

	if(argc < 3) {
		fprintf(stderr, 
MSGSTR(USAGE,"usage: %s -fR {username|uid} file ...\n"),
							progname);
		exit(4);
	}
	while ((c = getopt(argc,argv,"fR")) != EOF) {
		switch(c) {
			case 'f':
				fflag++;	
				break;
			case 'R':
				Rflag++;	
				break;
			case '?':
				fprintf(stderr,
MSGSTR(USAGE,"usage: %s -fR {username|uid} file ...\n"),
							progname);
				exit(4);
				break;
		}
	}

	whatuid(argv[optind], &newuid);

	optind++;

	if( optind == argc) {
		fprintf(stderr,
MSGSTR(USAGE,"usage: %s -fR {username|uid} file ...\n"),
							progname);
		exit(4);
		}

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (authorized_user("chown") && !forcepriv(SEC_CHOWN)) {
		fprintf(stderr, MSGSTR(PRIV, "%s: insufficient privileges\n"),
			progname);
		exit(4);
	}
#endif

	for(; optind<argc; optind++) {
		/* do stat for directory arguments */
#ifdef PFS
		if (_lestat(argv[optind], &stbuf) < 0) {
#else
		if (lstat(argv[optind], &stbuf) < 0) {
#endif PFS
			status += Perror(argv[optind]);
			continue;
		}
		if (Rflag && ((stbuf.st_mode&S_IFMT) == S_IFDIR)) {
			status += chownr(argv[optind], newuid, -1);
			continue;
		}
#if SEC_BASE
		disablepriv(SEC_SUSPEND_AUDIT);
#endif
		if (chown(argv[optind], newuid, -1)) {
#if SEC_BASE
			forcepriv(SEC_SUSPEND_AUDIT);
#endif
			status += Perror(argv[optind]);
			continue;
		}
#if SEC_BASE
		forcepriv(SEC_SUSPEND_AUDIT);
#endif
	}
	exit(status);
}

/*
 * NAME:
 * FUNCTION: is s a number
 */
isnumber(s)
char *s;
{
	register c;

	while(c = *s++)
		if(!isdigit(c))
			return(0);
	return(1);
}

/*
 * NAME: chownr
 * FUCNTION: reccursively descend the directory and change the owner of each
 *  file
 */
chownr(dir, uid, gid)
char *dir;
uid_t uid;
gid_t gid;
{
	DIR *dirp;
	struct dirent *dp;
#ifdef PFS
	struct estat st;
#else
	struct stat st;
#endif PFS
	char savedir[1024];
	int ecode;
	extern char *getwd();

	if (getwd(savedir) == (char *)0)
		fatal(255, "%s", savedir);
	/*
	 * Change what we are given before doing it's contents.
	 */
#if SEC_BASE
	disablepriv(SEC_SUSPEND_AUDIT);
#endif
	if (chown(dir, uid, gid) < 0 && Perror(dir)) {
#if SEC_BASE
		forcepriv(SEC_SUSPEND_AUDIT);
#endif
		return (1);
	}
#if SEC_BASE
	forcepriv(SEC_SUSPEND_AUDIT);
#endif
	if (chdir(dir) < 0) {
		Perror(dir);
		return (1);
	}
	if ((dirp = opendir(".")) == NULL) {
		Perror(dir);
		return (1);
	}
	dp = readdir(dirp);
	dp = readdir(dirp); /* read "." and ".." */
	ecode = 0;
	for (dp = readdir(dirp); dp != NULL; dp = readdir(dirp)) {
#ifdef PFS
		if (_lestat(dp->d_name, &st) < 0) {
#else
		if (lstat(dp->d_name, &st) < 0) {
#endif PFS
			ecode = Perror(dp->d_name);
			if (ecode)
				break;
			continue;
		}
		if ((st.st_mode&S_IFMT) == S_IFDIR) {
			ecode = chownr(dp->d_name, uid, gid);
			if (ecode)
				break;
			continue;
		}
#if SEC_BASE
		disablepriv(SEC_SUSPEND_AUDIT);
#endif
		if (chown(dp->d_name, uid, gid) < 0 &&
		    (ecode = Perror(dp->d_name))) {
#if SEC_BASE
			forcepriv(SEC_SUSPEND_AUDIT);
#endif
			break;
		}
#if SEC_BASE
		forcepriv(SEC_SUSPEND_AUDIT);
#endif
	}
	closedir(dirp);
	if (chdir(savedir) < 0)
		fatal(255, 
MSGSTR(NOBACK, "can not change directory back to %s"), savedir);
	return (ecode);
}
/*
 * NAME: error
 * FUNCTION:  display error message to the user
 */
error(fmt, a)
	char *fmt, *a;
{

	if (!fflag) {
		fprintf(stderr, "chown: ");
		fprintf(stderr, fmt, a);
		putc('\n', stderr);
	}
	return (!fflag);
}

/*
 * NAME: fatal
 * FUNCTION: display fatal errors and exit
 */
fatal(status, fmt, a)
	int status;
	char *fmt, *a;
{
	(void) error(fmt, a);
	exit(status);
}

/*
 * NAME: Perror
 * FUNCTION: display perror messages
 */
Perror(s)
	char *s;
{

	if (!fflag) {
		fprintf(stderr, "chown: ");
		perror(s);
	}
	return (!fflag);
}


/*
 * BSDism of being able to specify both user and group 
 */
whatuid(arg, uidp)
char	*arg;
uid_t	*uidp;
{

	if (isnumber(arg)) 
		*uidp = atoi(arg);
	else 
	{
		if ((pwd=getpwnam(arg)) == NULL) 
		{
			fprintf(stderr, 
MSGSTR(BADUID, "%s: unknown username %s\n"), progname, arg);
			exit(4);
		}
		*uidp = pwd->pw_uid;
	}

}



