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
static char rcsid[] = "@(#)$RCSfile: nice.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:32:01 $";
#endif
/*
 * COMPONENT_NAME: (CMDCNTL) system control commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * nice.c	1.7  com/cmd/cntl,3.1,9013 10/27/89 16:59:54
 */

/*
 *   The nice command lets you  run the specified command at a
 *   lower priority (or higher if you are the superuser).
 */                                                                   

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <stdio.h>
#include <ctype.h>
#include <nl_types.h>
#include "nice_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_NICE,Num,Str)
#include <locale.h>

main(argc, argv)
int argc;
char *argv[];
{
	int	nicarg = 10;

	(void) setlocale (LC_ALL,"");
	catd = catopen(MF_NICE,0);
#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	if(argc > 1 && argv[1][0] == '-') {
		register char	*p = argv[1];

		if(*++p != '-') {
			--p;
		}
		while(*++p)
			if(!isdigit((int)*p)) {
				fprintf(stderr, MSGSTR(MSGBNUM,"nice: argument must be numeric.\n")); /*MSG*/
				exit(2);
			}
		nicarg = atoi(&argv[1][1]);
		argc--;
		argv++;
	}
	if(argc < 2) {
		fprintf(stderr, MSGSTR(USAGE,"nice: usage: nice [-num] command\n")); /*MSG*/
		exit(2);
	}
#if SEC_BASE
	if (authorized_user("sysadmin"))
		forcepriv(SEC_LIMIT);
	disablepriv(SEC_SUSPEND_AUDIT);
#endif
	(void) nice(nicarg);	/* Set priority of this process. */
				/* We don't care if it failed.  */
	execvp(argv[1], &argv[1]);
	perror (argv[1]);
	exit(2);
}
