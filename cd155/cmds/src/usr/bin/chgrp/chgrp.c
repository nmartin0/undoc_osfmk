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
static char rcsid[] = "@(#)$RCSfile: chgrp.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:20:12 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: chgrp
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 * 
 * chgrp.c	1.14  com/cmd/oper,3.1,9013 2/12/90 16:55:32
 */

/*
 * chgrp gid file ...
 */
#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif
 
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#ifdef PFS
#include <sys/estat.h>
#else
#include <sys/stat.h>
#endif PFS
#include <grp.h>
#include <dirent.h>
#include <locale.h>
#include "chgrp_msg.h" 
nl_catd  catd;   /* Cat descriptor for scmc conversion */
#define MSGSTR(num,str) catgets(catd,MS_chgrp,num,str)  /*MSG*/

extern char *optarg;
extern int optind;

struct	group	*gr;
#ifdef PFS
struct	estat	stbuf;
#else
struct	stat	stbuf;
#endif PFS
int	gid;
int	status = 0;
char	*arg;
int	fflag = 0;
int 	Rflag = 0;

/*
 * NAME: chgrp
 *                                                                    
 * FUNCTION: Chages the group ownership of a file or directory.
 *     FLAGS: 
 *      -f   Suppresses all error reporting
 *      -R   Causes chgrp to descend recursively through its directory 
 *           arguments, setting the specified group ID.  When symbolic links 
 *           are encountered, their group is changed, but they are not 
 *           traversed.
 */  
main(argc, argv)
int argc;
char *argv[];
{
	register int c;

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_CHGRP, 0);

#if SEC_BASE
        set_auth_parameters(argc, argv);
        initprivs();

        if (authorized_user("anygroup") && !forcepriv(SEC_CHOWN)) {
                fprintf(stderr, "chgrp: insufficient privileges\n");
                exit(4);
        }
#endif

	if(argc < 3) {
		fprintf(stderr,MSGSTR(M_MSG_0,
			"%s: usage: %s -fR gid file ...\n"),argv[0],argv[0]);
		exit(4);
	}

	arg = argv[1];

	while ((c = getopt(argc,argv,"fR")) != EOF) {
		switch(c) {
			case 'f':
				fflag++;	
				break;
			case 'R':
				Rflag++;	
				break;
			case '?':
				fprintf(stderr,MSGSTR(M_MSG_1,
					"%s: unknown option\n"),argv[0]);
				exit(4);
				break;
		}
	}

	if(isnumber(argv[optind])) {
		gid = atoi(argv[optind]);
	} else {
		if((gr=getgrnam(argv[optind])) == NULL) {
			if (!fflag) {
				fprintf(stderr,MSGSTR(M_MSG_2,
					"%s: unknown group: %s\n") ,argv[0],argv[optind]);
				exit(4);
			}
			exit(0);
		}
		gid = gr->gr_gid;
	}
	optind++;
	for(; optind<argc; optind++) {
		/* do stat for directory arguments */
#ifdef PFS
		if (_lestat(argv[optind], &stbuf)) {
#else
		if (lstat(argv[optind], &stbuf)) {
#endif PFS
			status += Perror(argv[optind]);
			continue;
		}
		if (Rflag && ((stbuf.st_mode & S_IFMT) == S_IFDIR)) {
			status += chownr(argv[optind], gid);
			continue;
		}
#if SEC_BASE
                disablepriv(SEC_SUSPEND_AUDIT);
#endif
		if(chown(argv[optind],-1,gid) < 0) {
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
 * NAME: isnumber
 * FUNCTION: is s a number
 * RETURN VALUES:   0 if not number
 *                  1 if number
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
 * FUNCTION: recursively descend directory and change the group id of all
 *   files.
 */
chownr(dir, gid)
char *dir;
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

	if (getwd(savedir) == 0)
		fatal(255, "%s", savedir);
	/*
	 * Change what we are given before doing its contents.
	 */
#if SEC_BASE
        disablepriv(SEC_SUSPEND_AUDIT);
#endif
	if (chown(dir, -1, gid) < 0 && Perror(dir)) {
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
		if ((st.st_mode & S_IFMT) == S_IFDIR) {
			ecode = chownr(dp->d_name, gid);
			if (ecode)
				break;
			continue;
		}
#if SEC_BASE
                disablepriv(SEC_SUSPEND_AUDIT);
#endif
		if (chown(dp->d_name, -1, gid) < 0 &&
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
		fatal(255,MSGSTR(M_MSG_4,"can't change back to %s"), savedir);
	return (ecode);
}

/*
 * NAME: error
 * FUNTION: display error message.
 */
error(fmt, a)
	char *fmt, *a;
{

	if (!fflag) {
		fprintf(stderr, "chgrp: ");
		fprintf(stderr, fmt, a);
		putc('\n', stderr);
	}
	return (!fflag);
}
/*
 * NAME: fatal
 * FUNCTION:  displays fatal error messages to the user
 */
fatal(status, fmt, a)
	int status;
	char *fmt, *a;
{

	fflag = 0;
	(void) error(fmt, a);
	exit(status);
}

/*
 * NAME: Perror
 * FUNCTION: displays perror messages
 */
Perror(s)
	char *s;
{

	if (!fflag) {
		fprintf(stderr, "chgrp: ");
		perror(s);
	}
	return (!fflag);
}
