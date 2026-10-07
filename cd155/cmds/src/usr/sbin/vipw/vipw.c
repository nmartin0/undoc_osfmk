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
 * OSF/1 Release 1.0.3
*/
/* @(#)vipw.c	5.2 17:33:32 8/20/90 SecureWare */
static char sccsid[] = "@(#)vipw.c	1.2  com/cmd/oper,3.1, 9/12/89 13:42:29";
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: vipw
 *
 * ORIGINS: 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>
#endif

#include <stdio.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>

#include <stdio.h>
#include <errno.h>
#include <signal.h>

#ifdef MSG
#include "vipw_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_VIPW,n,s) 
#else
#define MSGSTR(n,s) s
#endif

#if SEC_BASE
extern priv_t *privvec();
#endif

/*
 * NAME: vipw
 * FUNCTION:  Password file editor with locking.
 */
char	temp[] = "/etc/ptmp";        /* temp files used by vipw and mkpasswd */
char	temp_pag[] = "/etc/ptmp.pag";
char	temp_dir[] = "/etc/ptmp.dir";
char	passwd[] = "/etc/passwd";
char	passwd_pag[] = "/etc/passwd.pag";
char	passwd_dir[] = "/etc/passwd.dir";
char	vi[] = "/usr/bin/vi";
char	ed[] = "/usr/bin/ed";
char	buf[BUFSIZ];
char	*getenv();
char	*index();
extern	int errno;

main(argc, argv)
	char *argv[];
{
	int fd;
	FILE *ft, *fp;
	char *editor;
#if SEC_BASE
	privvec_t saveprivs;
#endif

	(void ) setlocale(LC_ALL,"");
#ifdef MSG
	catd = catopen(MF_VIPW,0);
#endif
#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("auth")) {
		fprintf(stderr, "vipw: need auth authorization\n");
		exit(1);
	}

	if (forceprivs(privvec(SEC_ALLOWDACACCESS, SEC_CHOWN, SEC_LIMIT,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), saveprivs)) {
		fprintf(stderr, "vipw: insufficient privileges\n");
		exit(1);
	}
#endif
	signal(SIGHUP, SIG_IGN);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	setbuf(stderr, NULL);
#if SEC_BASE
	if (create_file_securely(temp, AUTH_VERBOSE, 0) != CFS_GOOD_RETURN) {
		fprintf(stderr,MSGSTR(BUSY,"vipw: password file busy\n"));
		exit(1);
	}
	if ((fd = open(temp, O_WRONLY | O_SYNC)) < 0) {
		fprintf(stderr, "vipw: ");
		perror(temp);
		exit(1);
	}
#else /* !SEC_BASE */
	umask(0);
	fd = open(temp, O_WRONLY|O_CREAT|O_EXCL|O_SYNC, 0644);
	if (fd < 0) {
		if (errno == EEXIST) {
			fprintf(stderr,MSGSTR(BUSY,"vipw: password file busy\n"));
			exit(1);
		}
		fprintf(stderr,MSGSTR(VIPW, "vipw: ")); perror(temp);
		exit(1);
	}
#endif /* !SEC_BASE */
	ft = fdopen(fd, "w");
	if (ft == NULL) {
		fprintf(stderr,MSGSTR(VIPW,"vipw: ")); perror(temp);
		bad();
	}
	fp = fopen(passwd, "r");
	if (fp == NULL) {
		fprintf(stderr,MSGSTR(VIPW,"vipw: ")); perror(passwd);
		bad();
	}
	while (fgets(buf, sizeof (buf) - 1, fp) != NULL)
		fputs(buf, ft);
	fclose(ft); fclose(fp);
	editor = getenv("EDITOR");
	if (editor == 0) {
		editor = vi;
		if (access(vi,X_OK) == -1) {
			editor = ed;
			fprintf(stdout,MSGSTR(EDMSG,"Unable to find vi editor using ed.\n"));
			fprintf(stdout,MSGSTR(EDMSG2,"Enter q to exit ed\n"));
		}
	}
	sprintf(buf, "%s %s", editor, temp);
	if (system(buf) == 0) {
		struct stat sbuf;
		int ok;

		/* sanity checks */
		if (stat(temp, &sbuf) < 0) {
			fprintf(stderr,MSGSTR(STAT,
			    "vipw: can't stat temp file, %s unchanged\n"),
			    passwd);
			bad();
		}
		if (sbuf.st_size == 0) {
			fprintf(stderr, MSGSTR(BADTEMP,
				"vipw: bad temp file, %s unchanged\n"), passwd);
			bad();
		}
		ft = fopen(temp, "r");
		if (ft == NULL) {
			fprintf(stderr,MSGSTR(REOPEN,
			    "vipw: can't reopen temp file, %s unchanged\n"),
			    passwd);
			bad();
		}
		ok = 0;
		while (fgets(buf, sizeof (buf) - 1, ft) != NULL) {
			register char *cp;

			cp = index(buf, '\n');
			if (cp == 0)
				continue;
			*cp = '\0';
			cp = index(buf, ':');
			if (cp == 0)
				continue;
			*cp = '\0';
			if (strcmp(buf, "root"))
				continue;
			/* password */
			cp = index(cp + 1, ':');
			if (cp == 0)
				break;
			/* uid */
			if (atoi(cp + 1) != 0)
				break;
			cp = index(cp + 1, ':');
			if (cp == 0)
				break;
			/* gid */
			cp = index(cp + 1, ':');
			if (cp == 0)
				break;
			/* gecos */
			cp = index(cp + 1, ':');
			if (cp == 0)
				break;
			/* login directory */
			if (strncmp(++cp, "/:", 2))
				break;
			cp += 2;
			if (*cp && strcmp(cp, "/sbin/sh") && 
			           strcmp(cp, "/bin/sh") &&
			           strcmp(cp, "/bin/csh") && 
			           strcmp(cp, "/bin/ksh") && 
			           strcmp(cp, "/bin/bsh"))
				break;
			ok++;
		}
		fclose(ft);
		if (ok) {
			if (makedb(temp) < 0)
				fprintf(stderr, MSGSTR(MKPASS,"vipw: mkpasswd failed\n"));
			else if (rename(temp_pag, passwd_pag) < 0)
				fprintf(stderr, MSGSTR(VIPW,"vipw: ")), perror(temp_pag);
			else if (rename(temp_dir, passwd_dir) < 0)
				fprintf(stderr,MSGSTR(VIPW, "vipw: ")), perror(temp_dir);
			else if (rename(temp, passwd) < 0)
				fprintf(stderr,MSGSTR(VIPW, "vipw: ")), perror("rename");
			else
				exit(0);
		} else
			fprintf(stderr,MSGSTR(MANGLED,
			    "vipw: you mangled the temp file, %s unchanged\n"),
			    passwd);
	}
	bad();
}

/*
 * NAME: bad
 * FUNCTION: cleanup and exit
 */
bad()
{
	unlink(temp_pag);
	unlink(temp_dir);
	unlink(temp);
	exit(1);
}

/*
 * NAME: makedb
 * FUNCTION: make database
 */
makedb(file)
	char *file;
{
	int status, pid, w;

	if ((pid = vfork()) == 0) {
		execl("/usr/sbin/mkpasswd", "mkpasswd", file, 0);
		exit(1);
	}
	while ((w = wait(&status)) != pid && w != -1)
		;
	if (w == -1 || status != 0)
		status = -1;
	return(status);
}
