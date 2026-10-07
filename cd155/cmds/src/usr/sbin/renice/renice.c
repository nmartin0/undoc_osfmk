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
static char rcsid[] = "@(#)$RCSfile: renice.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:14:11 $";
#endif
/*
 * COMPONENT_NAME: (CMDCNTL) system control commands
 *
 * FUNCTIONS:
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
 *
 * renice.c	1.8  com/cmd/cntl,3.1,9021 11/27/89 11:31:54 
 * renice.c	4.2 02:01:31 7/13/90 SecureWare 
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
#include <pwd.h>
#include <nl_types.h>
#include <locale.h>
#include "renice_msg.h"

#define         MSGSTR(num,str) catgets(catd,MS_RENICE,num,str)  /*MSG*/

nl_catd	catd;
char	*progname;

#if SEC_BASE
static int	has_auth = 0;
extern priv_t	*privvec();
#endif

/*
 * Change the priority (nice) of processes
 * or groups of processes which are already
 * running.
 */
main(argc, argv)
	char **argv;
{
	int which = PRIO_PROCESS;
	int who = 0, prio, errs = 0;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_RENICE, 0);
	progname = argv[0];
#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	argc--, argv++;
	if (argc < 2) {
		fprintf(stderr,MSGSTR(USAGE, 
		"usage: %s priority [ [ -p ] pids ] [ [ -g ] pgrps ] [ [ -u ] users ]\n") ,progname);
		exit(1);
	}
	prio = atoi(*argv);
	argc--, argv++;
	if (prio > PRIO_MAX)
		prio = PRIO_MAX;
	if (prio < PRIO_MIN)
		prio = PRIO_MIN;
#if SEC_BASE
	has_auth = authorized_user("sysadmin");
#endif
	for (; argc > 0; argc--, argv++) {
		if (strcmp(*argv, "-g") == 0) {
			which = PRIO_PGRP;
			continue;
		}
		if (strcmp(*argv, "-u") == 0) {
			which = PRIO_USER;
			continue;
		}
		if (strcmp(*argv, "-p") == 0) {
			which = PRIO_PROCESS;
			continue;
		}
		if (which == PRIO_USER) {
			register struct passwd *pwd = getpwnam(*argv);
			
			if (pwd == NULL) {
				fprintf(stderr, MSGSTR(UNK_USER,"%s: User %s is unknown.\n"),
					progname,*argv);
				continue;
			}
			who = pwd->pw_uid;
		} else {
			who = atoi(*argv);
			if (who < 0) {
				fprintf(stderr, MSGSTR(BADVALUE,"%s: Parameter %s is bad.\n"),
					progname,*argv);
				continue;
			}
		}
		errs += donice(which, who, prio);
	}
	exit(errs != 0);
}

donice(which, who, prio)
	int which, who, prio;
{
	int oldprio;
	extern int errno;
#if SEC_BASE
	privvec_t saveprivs;
#endif

	errno = 0, oldprio = getpriority(which, who);
	if (oldprio == -1 && errno) {
		fprintf(stderr, MSGSTR(GETPRIO,"%s: %d: "), progname,who);
		perror("getpriority");
		return (1);
	}
#if SEC_BASE
	if (has_auth) {
		if (forceprivs(privvec(SEC_OWNER, SEC_LIMIT,
#if SEC_MAC
					SEC_ALLOWMACACCESS,
#endif
					-1), saveprivs)) {
			fprintf(stderr,
				MSGSTR(PRIV, "%s: insufficient privileges\n"),
				progname);
			exit(1);
		}
	} else
		getpriv(SEC_EFFECTIVE_PRIV, saveprivs);
	disablepriv(SEC_SUSPEND_AUDIT);
#endif
	if (setpriority(which, who, prio) < 0) {
#if SEC_BASE
		seteffprivs(saveprivs, (priv_t *) 0);
#endif
		fprintf(stderr, MSGSTR(SETPRIO,"%s: %d: "), progname ,who);
		perror("setpriority");
		return (1);
	}
#if SEC_BASE
	seteffprivs(saveprivs, (priv_t *) 0);
#endif
	fprintf(stdout,MSGSTR(NEWPRI, 
		"%d: old priority %d, new priority %d\n"), who, oldprio, prio);
	return (0);
}
