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
 
/* @(#)atq.c	4.2 09:45:35 7/12/90 SecureWare */
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: atq.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:18:52 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: atq
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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <sys/secdefines.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <pwd.h>
#include "cron.h"

/*
 * NAME: atq
 *                                                                    
 * FUNCTION: command line processing to call the correct atq functions
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 * 	This function will compute the command line arguments and
 *	call the correct "at" function based on the user flags.
 *                                                                   
 */  

#include "cron_msg.h"
#define MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)
nl_catd catd;

#define INVALIDUSER	"you are not a valid user (no entry in /etc/passwd)"
#define CANTPEEK	"you may only look at your own jobs.\n"

main(argc,argv)
int argc;
char *argv[];
{
	register int 	i;		/* loop variable */
	register int 	flag=0;		/* boolean indicator */
	char 		argbuf[BUFSIZ];	/* argument buffer */
	uid_t 		uid;		/* real userid of caller */
	struct passwd 	*nptr;		/* passwd struct of caller */
	char 		*user;		/* username of caller */
	short		cflag=0;	/* creation order */
	short		nflag=0;	/* number of jobs */

	(void ) setlocale(LC_ALL,"");

	catd = catopen(MF_CRON,0);

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();

#if SEC_MAC
	disablepriv(SEC_MULTILEVELDIR);
#endif

	if ((nptr = getpwuid(uid = getluid())) == NULL)
#else /* !SEC_BASE */
	if ((nptr=getpwuid(uid=getuid())) == NULL)
#endif /* !SEC_BASE */
	{
		fprintf(stderr,"atq: %s\n",MSGSTR(MS_INVALIDUSER, INVALIDUSER));
		exit(1);
	}
	else 
		user = nptr->pw_name;

	if ((argc == 1) ) {	/* atq */	/* list in execution order */
#if SEC_BASE
		list_aj(CRON_SORT_M, at_authorized() ? NULL : user);
#else
		list_aj(CRON_SORT_M,(uid == ROOT)? NULL : user);
#endif
		exit(0);
	}

	/* 
 	 * if flags are specified but no user names are
	 * listed, then display at jobs for this user
	 */

	if (argv[1][0] == '-') {
		switch (argv[1][1]) {
		   case 'c':		/* list in order of submission */
			cflag=1;
			break;

		   case 'n':		/* count total number of at jobs */
			nflag=1;
			break;

		   case '?':
			usage();
		   default:
			usage();
		}

		if (argc==2) {	
		    if (cflag)
			list_aj(CRON_SORT_E, user);	
		    else /* nflag */
			list_aj(CRON_COUNT_JOBS, user);
		} else {
#if SEC_BASE
			int listed = 0;

			for (i = 2; i < argc; i++) {
				if (strcmp(argv[i], user) && !at_authorized())
					continue;
				++listed;
				if (cflag)
					list_aj(CRON_SORT_E, argv[i]);
				else
					list_aj(CRON_COUNT_JOBS,argv[i]);	
			}
			if (listed == 0) {
				fprintf(stderr, "atq: %s\n",
					MSGSTR(MS_CANTPEEK, CANTPEEK));
				exit(1);
			}
#else /* !SEC_BASE */
			if ((uid != ROOT) && ((argc > 3) || (strcmp(argv[2], user)))) {
				fprintf(stderr, "atq: %s\n", MSGSTR(MS_CANTPEEK, CANTPEEK));
				exit(1);
			}
		    
			if(cflag)
				for (i=2; i<argc; i++) 
					list_aj(CRON_SORT_E,argv[i]);	
			else 
				for (i=2; i<argc; i++) 
					list_aj(CRON_COUNT_JOBS,argv[i]);	
#endif /* !SEC_BASE */
		}
	}
	else {
#if SEC_BASE
		int	listed = 0;
#else
		if ((uid != ROOT) && ((argc > 2) || (strcmp(argv[1], user)))) {
			fprintf(stderr, MSGSTR(MS_CANTPEEK, CANTPEEK));
			exit(1);
		}
#endif

		for (i=1; i<argc; i++) {	/* list in execution order */
#if SEC_BASE
			if (strcmp(argv[i], user) && !at_authorized())
				continue;
			++listed;
#endif
			list_aj(CRON_SORT_M,argv[i]);	
		}
#if SEC_BASE
		if (listed == 0) {
			fprintf(stderr, MSGSTR(MS_CANTPEEK, CANTPEEK));
			exit(1);
		}
#endif
	}
#if SEC_BASE
	/* Bug fix */
	exit(0);
#endif

}

usage()
{
	fprintf(stderr, MSGSTR(MS_ATQUSAGE, 
	"Usage: atq [-c|-n] [username...]\n"));
	exit(1);
}
