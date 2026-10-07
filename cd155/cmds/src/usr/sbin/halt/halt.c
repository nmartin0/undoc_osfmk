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
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: halt
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



#ifdef MSG
#include "halt_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_HALT,n,s) 
#ifdef SEC_BASE
#define MSGSTR_SEC(n,s) catgets(catd,MS_HALT_SEC,n,s)
#endif
#else
#define MSGSTR(n,s) s
#endif

/*
 * NAME: Halt
 * FUNCTION: stops the processor.
 *   -n  Prevents the sync before stopping
 *   -q  Causes a quick halt, no graceful stop in attempted.
 *   -y  Halts the system from a dialup.
 *   -l  Does not log the halt in the system accounting records.
 */
#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif

#include <stdio.h>
#include <locale.h>
#include <sys/reboot.h>
#include <sys/types.h>
#include <sys/syslog.h>
#include <errno.h>
#include <signal.h>
#include <pwd.h>

#include <utmp.h>
#define SCPYN(a, b)	strncpy(a, b, sizeof(a))
struct utmp wtmp;

main(argc, argv)
	int argc;
	char **argv;
{
	int nosync = 0;
	char *ttyn = (char *)ttyname(2);
	register int qflag = 0;
	int needlog = 1;
	int howto, ch, i;
	char *user, *getlogin();
        extern uid_t geteuid();
	struct passwd *pw;

#ifdef NLS
	(void ) setlocale(LC_ALL,"");
#endif


#ifdef MSG
	catd = catopen(MF_HALT,0);
#endif

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
	if (!authorized_user("sysadmin")) {
		fprintf(stderr,
			MSGSTR_SEC(AUTH, "%s: need sysadmin authorization\n"),
			command_name);
		exit(1);
	}
	if (forceprivs(privvec(SEC_ALLOWDACACCESS, SEC_LIMIT, SEC_SHUTDOWN,
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
			MSGSTR_SEC(PRIV, "%s: insufficient privileges\n"),
			command_name);
		exit(1);
	}
#else /* SEC_BASE */

	if (geteuid()) {
                fprintf(stderr, MSGSTR(NO_ROOT, "NOT super-user\n"));
                exit(1);
        }

#endif /* SEC_BASE */
	howto = RB_HALT;
	while ((ch = getopt(argc, argv, "lnqy")) != EOF) {
		switch ((char)ch) {
		case 'n':
			howto |= RB_NOSYNC;
			break;
		case 'y':
			ttyn = 0;
			break;
		case 'q':
			qflag++;
			break;
		case 'l':
			needlog = 0;
			break;
		case '?':
		default:
			fprintf(stderr, MSGSTR(USAGE, "usage: halt [-y][-q][-l][-n]\n"));
			exit(1);
		}
	}
	if (ttyn && *(ttyn+strlen("/dev/tty")) == 'd') {
		fprintf(stderr, MSGSTR(DIALUP, "halt: dangerous on a dialup; use ``halt -y'' if you are really sure\n")); /*MSG*/
		exit(1);
	}

	if (needlog) {
		openlog("halt", 0, LOG_AUTH);
		user = getlogin();
		if (user == (char *)0 && (pw = getpwuid((uid_t)getuid())))
			user = pw->pw_name;
		if (user == (char *)0)
			user = "root";
		syslog(LOG_CRIT, MSGSTR(LOGHLT, "halted by %s"), user); /*MSG*/
	}

	signal(SIGHUP, SIG_IGN);		/* for network connections */
	signal(SIGTERM, SIG_IGN);		/* to not kill ourselves */
	if (kill(1, SIGTSTP) == -1) {
		fprintf(stderr, MSGSTR(IDLE, "halt: can't idle init\n"));
		exit(1);
	}
	sleep(1);
	(void) kill(-1, SIGTERM);	/* one chance to catch it */
	sleep(5);

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

				perror("halt: kill");
				kill(1, SIGHUP);
				exit(1);
			}
			if (i > 5) {
				fprintf(stderr, MSGSTR(CAUTION,"CAUTION: some process(es) wouldn't die\n"));
				break;
			}
			setalarm(2 * i);
			pause();
			if (!(howto & RB_NOSYNC))
				sync();
		}
	}
	printf(MSGSTR(HALTOVER, "....Halt completed....\n") ); /*MSG*/
	fflush(stdout);
	sleep(3);
	reboot(howto, (time_t *)0);
	perror("reboot");
	kill(1, SIGHUP);
	exit(1);
}

/*
 * NAME: sigalrm
 * FUNCTION: catch SIGALRM
 */
sigalrm(void)
{
}

/*
 * NAME: setalarm
 * FUNCTION:  set alarm
 */
setalarm(n)
{
	signal(SIGALRM, (void (*)(int))sigalrm);
	alarm(n);
}

/*
 * NAME: markdown
 * FUNCTION: log shutdown procedure
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
