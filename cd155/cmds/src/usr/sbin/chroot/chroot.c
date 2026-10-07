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
 
/* @(#)chroot.c	5.1 15:41:42 8/16/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid [] = "@(#)chroot.c	1.1  com/cmd/fs/progs,3.1,8928 7/13/89 16:36:43";
/* chroot */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <stdio.h>

#if defined(NLS) || defined(KJI)
#define	NLSKJI 1
#include <NLctype.h>
#include <NLchar.h>
#define printf NLprintf
#define fprintf NLfprintf
#endif
#ifdef NLS
#include <locale.h>
#endif

#ifdef MSG
#include "chroot_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) NLcatgets(catd,MS_CHROOT,Num,Str)
#else
#define MSGSTR(Num,Str) Str
#endif

main(argc, argv)
char **argv;
{
#ifdef NLS
	(void) setlocale (LC_ALL,"");
#endif
#ifdef MSG
	catd = NLcatopen(MF_CHROOT,0);
#endif
	if(argc < 3) {
		printf(MSGSTR(USAGE,"usage: chroot rootdir command arg ...\n"));
		exit(1);
	}
	argv[argc] = 0;
	if(argv[argc-1] == (char *) -1) /* don't ask why */
		argv[argc-1] = (char *) -2;

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("chroot")) {
		printf(MSGSTR(AUTH, "%s: need chroot authorization\n"),
			"chroot");
		exit(1);
	}

	if (!forcepriv(SEC_CHROOT)) {
		printf(MSGSTR(PRIV, "%s: insufficient privileges\n"), "chroot");
		exit(1);
	}
	disablepriv(SEC_SUSPEND_AUDIT);
#endif

	if (chroot(argv[1]) < 0) {
		perror(argv[1]);
		exit(1);
	}
#if SEC_BASE
	forcepriv(SEC_SUSPEND_AUDIT);
#endif
	if (chdir("/") < 0) {
		printf(MSGSTR(CANTCHROOT,"Can't chdir to new root\n"));
		exit(1);
	}
	execv(argv[2], &argv[2]);
	close(2);
	open("/dev/tty", 1);
	printf(MSGSTR(NOTFOUND,"%s: not found\n"),argv[2]);
	exit(1);
}
