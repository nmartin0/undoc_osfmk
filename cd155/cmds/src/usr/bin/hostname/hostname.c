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
 
/* @(#)hostname.c	3.5 16:31:15 6/21/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: hostname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:06 $";
#endif
/* 
 * COMPONENT_NAME: TCPIP hostname.c
 * 
 * FUNCTIONS: Mhostname, printit, usage 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	hostname.c	5.1 (Berkeley) 4/30/85
 */

/*
 * hostname -- get (or set hostname)
 */
#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#include <prot.h>

extern priv_t *privvec();
#endif
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/syslog.h>

#include "hostname_msg.h" 
#define MSGSTR(n,s) NLgetamsg(MF_HOSTNAME,MS_HOSTNAME,n,s) 

#include <locale.h>

#define MAXDNAME	256  	/* maximum domain name	*/
static int usage();

char *cmd_name;

main(argc,argv)
int  argc;
char *argv[];

{

extern int optind;
int ch, sflag = 0;
char *p, hostname[MAXDNAME];

	setlocale(LC_ALL,"");

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	cmd_name = argv[0];
	if (argc > 3)
		usage();

	while ((ch = getopt(argc, argv, "s")) != EOF)
		switch((char)ch) {
			case 's':
				sflag = 1;
				break;
			case '?':
			default:
				usage();
		}
	argv += optind;

	if (*argv) {  /* set it */
#if SEC_BASE
		privvec_t saveprivs;
		int status;

		if (!authorized_user("sysadmin")) {
			fprintf(stderr, "%s: need sysadmin authorization\n",
				command_name);
			exit(1);
		}
		if (forceprivs(privvec(SEC_SYSATTR, -1), saveprivs)) {
			fprintf(stderr, "%s: insufficient privileges\n",
				command_name);
			exit(1);
		}
		disablepriv(SEC_SUSPEND_AUDIT);
		status = sethostname(*argv, strlen(*argv));
		seteffprivs(saveprivs, (priv_t *) 0);
		if (status < 0) {
			perror(MSGSTR(SETHOST,"sethostname"));
			exit(1);
		}
#else /* !SEC_BASE */
		if (sethostname(*argv,strlen(*argv))) {  /* error */
			perror(MSGSTR(SETHOST,"sethostname"));
			exit(1);
		}
#endif /* !SEC_BASE */
	}

	/* print it in either case */

	if(gethostname(hostname,sizeof(hostname))){
		perror(MSGSTR(GETHOSTNAME, "gethostname"));
		exit(1);
	}
	if(sflag && (p = index(hostname, '.')))  /* trim domain name */
		*p = '\0';
	puts(hostname);

	exit(0);
}

static usage()
{
        printf(MSGSTR(USAGE, "usage: %s [-s] [hostname]\n"), cmd_name ); /*MSG*/
	exit(1);
}
