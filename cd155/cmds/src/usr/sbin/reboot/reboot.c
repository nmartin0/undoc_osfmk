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
 
/* @(#)reboot.c	3.2 23:16:03 6/19/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */

/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: reboot
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
 * Copyright (c) 1980,1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/syslog.h>
#include <locale.h>
#include <sys/reboot.h>
#include <utmp.h>
#define SCPYN(a, b)	strncpy(a, b, sizeof(a))
struct utmp wtmp;

#ifdef MSG
#include "reboot_msg.h"
nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_REBOOT,n,s)
#ifdef SEC_BASE
#define MSGSTR_SEC(n,s) catgets(catd,MS_REBOOT_SEC,n,s)
#endif
#else
#define MSGSTR(n,s) s
#endif


/*
 * NAME:  Reboot
 * FUNCTION:  restarts the machine.
 *   -l   Do not log the reboot in the system accounting records.
 *        file. The -n and -q options imply -l.
 *   -n   Do not perform the sync.
 *   -q   Reboot quickly and ungracefully, without shutting down running 
 *        processes first.
 */
main(argc, argv)
	int argc;
	char **argv;
{
	int howto;
	int qflag = 0;
	int nflag = 0;
	int needlog = 1;
	int i;
	char *user;
	struct passwd *pw;

#ifdef NLS
	(void) setlocale(LC_ALL, "");
#endif
#ifdef MSG
	catd = NLcatopen(MF_REBOOT,0);
#endif

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

	if (!authorized_user("sysadmin")) {
		fprintf(stderr, MSGSTR_SEC(AUTH,
			"%s: command requires 'sysadmin' authorization.\n"),
			command_name);
		exit(1);
	}
	if (forceprivs(privvec(SEC_ALLOWDACACCESS, SEC_SHUTDOWN,
#if SEC_MAC
				SEC_ALLOWMACACCESS,
#endif
#if SEC_ILB
				SEC_ILNOFLOAT,
#endif
#if SEC_NCAV
				SEC_ALLOWNCAVACCESS,
#endif
				-1), (priv_t *) 0)) {
		fprintf(stderr,
			MSGSTR(PRIV, "%s: insufficient privileges\n"),
			command_name);
		exit(1);
	}

#else /* SEC_BASE */
	if (geteuid()) {
                fprintf(stderr, MSGSTR(NO_ROOT, "NOT super-user\n"));
                exit(1);
        }

#endif /* SEC_BASE */

	argc--, argv++;
	howto = RB_AUTOBOOT;
	while (argc > 0) {
		if (!strcmp(*argv, "-q"))	/* quick reboot */
			qflag++;
		else if (!strcmp(*argv, "-n"))	/* do not sync disks */
			howto |= RB_NOSYNC;
		else if (!strcmp(*argv, "-l"))	/* do not log reboot */
			needlog = 0;
		else {
			fprintf(stderr,
			    MSGSTR(USAGE,"usage: reboot [-l][-n][-q]\n"));
			exit(1);
		}
		argc--, argv++;
	}

	if (needlog) {
		openlog("reboot", 0, LOG_AUTH);
		user = getlogin();
		if (user == (char *)0 && (pw = getpwuid(getuid())))
			user = pw->pw_name;
		if (user == (char *)0)
			user = "root";
		syslog(LOG_CRIT,MSGSTR(LOGIT,"rebooted by %s"), user);
	}

	signal(SIGHUP, SIG_IGN);	/* for remote connections */
	signal(SIGTERM, SIG_IGN);	/* to not kill ourselves */
	if (kill(1, SIGTSTP) == -1) {
		fprintf(stderr, MSGSTR(IDLE, "reboot: can't idle init\n"));
		exit(1);
	}
	sleep(1);
	(void) kill(-1, SIGTERM);	/* one chance to catch it */
	if (!qflag) {
		if (!qflag && !(howto & RB_NOSYNC)) {
			markdown();
			sync();
		}
		for (i = 1; ; i++) {
			if (kill(-1, SIGKILL) == -1) {
				extern int errno;

				if (errno == ESRCH)
					break;

				perror("reboot: kill");
				kill(1, SIGHUP);
				exit(1);
			}
			if (i > 5) {
				fprintf(stderr, MSGSTR(CAUTION,"CAUTION: some process(es) wouldn't die\n"));
				break;
			}
			setalarm(2 * i);
			pause();
		}
		if (!(howto & RB_NOSYNC))
			sync();
	}
	printf(MSGSTR(REBOOT,"Rebooting . . .\n"));
	fflush(stdout);
	sleep((unsigned)3);
	reboot(howto, (time_t *)0);
	perror("reboot");
	kill(1, SIGHUP);
	exit(1);
}

/*
 * NAME: dingdong
 * FUNCTION:  catch SIGALRM
 */
dingdong(void)
{
	/* RRRIIINNNGGG RRRIIINNNGGG */
}

/*
 * NAME: setalarm
 * FUNCTION: set up alarm.
 */
setalarm(n)
{
	signal(SIGALRM, (void (*)(int))dingdong);
	alarm((unsigned)n);
}

/*
 * NAME: markdown
 * FUNCTION:  write shutdown entry in /usr/adm/wtmp file.
 */
markdown()
{
	register f = open(WTMP_FILE, 1);
	if (f >= 0) {
		lseek(f, 0L, 2);
		SCPYN(wtmp.ut_line, "~");
		SCPYN(wtmp.ut_name, "shutdown");
		SCPYN(wtmp.ut_host, "");
		wtmp.ut_type=USER_PROCESS;
		time(&wtmp.ut_time);
		write(f, (char *)&wtmp, sizeof(wtmp));
		close(f);
	}
}
