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
static char rcsid[] = "@(#)$RCSfile: atrm.c,v $ $Revision: 1.4 $ (OSF) $Date: 1994/11/19 01:18:53 $";
#endif
/*
 * COMPONENT_NAME: (CMDOPER) commands needed for basic system needs
 *
 * FUNCTIONS: atrm
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
 * 
 * atrm.c	1.9  com/cmd/oper/cron,3.1,9021 4/3/90 19:03:07
 * atrm.c	4.2 13:36:06 7/11/90 SecureWare 
 */


#include <sys/secdefines.h>

#include <stdio.h>
#include <unistd.h>
#include <locale.h>
#include <pwd.h>
#include <dirent.h>
#include "cron.h"

/*
 * NAME: atrm
 *                                                                    
 * FUNCTION: command line processing to call the correct atrm functions
 *                                                                    
 * EXECUTION ENVIRONMENT:
 *                                                                   
 * 	This function will compute the command line arguments and
 *	call the correct "at" function based on the user flags.
 *	The correct at command flag arguments are added and then 
 *	at is called.
 *                                                                   
 * RETURNS: nothing
 */  

#include "cron_msg.h"
#define MSGSTR(Num,Str) catgets(catd,MS_CRON,Num,Str)
nl_catd catd;

extern char *strcpy();
extern char *strcat();
extern char *strchr();
extern char *setlocale(int category, const char *locale);
extern struct passwd *getpwuid(uid_t uid);

#define ATRMDEL "atrm: only jobs belonging to user: %s may be deleted\n"

main(argc,argv)
int argc;
char *argv[];
{
	DIR	*dir;
	char	**ptr_argv;
	char	*name;
	char	argbuf[BUFSIZ];
	int	allflag = 0;
	int	iflag = 0;
	int	fflag = 0;
	int	user;
	struct dirent *dp;
	struct passwd *pwinfo;	/* struct for pwd name info */

	(void ) setlocale(LC_ALL,"");

	catd = catopen(MF_CRON,0);

#if SEC_BASE
	set_auth_parameters(argc, argv);
	initprivs();
#endif
	/* 
	 * If job number, user name, or "-" is not specified, just print
	 * usage info and exit.
	 */
	if (argc < 2)
		usage ();
	--argc; ++argv;

	while (argc > 0 && **argv == '-') {
		if (*(++(*argv)) == '\0') {
			++allflag;
		} else while (**argv)
			 switch (*(*argv)++) {
				case 'f':	++fflag;
						break;
				case 'i':	++iflag;
						break;
				default:
						usage ();
			}
		++argv; --argc;
	}
	/*
	 * If all jobs are to be removed and extra command line arguments
	 * are given, print usage info and exit.
	 */
	if (allflag && argc)
		usage();

	/*
	 * If only certain jobs are to be removed and no job #'s or user
	 * names are specified, print usage info and exit.
	 */
	if (!allflag && !argc)
		usage();

	/*
	 * If interavtive removal and quiet removal are requexted, override
	 * quiet removal and run interactively.
	 */
	if (iflag && fflag)
		fflag = 0;

	/*
	 * Move to spooling area and get user name of person requesting 
 	 * removal.
	 */
#if SEC_BASE
	user = getluid();
#else
	user = getuid();
#endif
	pwinfo = getpwuid(user);

#if SEC_BASE
	if (at_authorized()) {
#else
	if (!user) { /* super user */
#endif
		if (allflag) {
			if (iflag)
				remove_aj(CRON_PROMPT, NULL);
			else if (fflag)
				remove_aj(CRON_QUIET, NULL);
			else remove_aj(NULL, NULL);
		}
		else {
			strcpy (argbuf, "/usr/bin/at -r ");
			if (iflag)
				strcat(argbuf, "-i ");
			else if (fflag)
				strcat(argbuf, "-f ");
			for (; argc--; argv++) {
				if ((strchr(*argv, '.')) != NULL) {
					/* job#'s */
					strcat(argbuf, *argv);
					strcat(argbuf, " ");
				}
				else {	/* users name */
					strcat(argbuf,"-u ");
					strcat(argbuf, *argv);
					strcat(argbuf, " ");
				}
			}
			exit(system(argbuf));
		}
	}
	else {	/* normal user */
		if(allflag) {	/* "-" flag is good for super user only */
			fprintf(stderr,MSGSTR(MS_ATRMDEL,ATRMDEL), pwinfo->pw_name);
                        exit(1);
		}
		strcpy (argbuf, "/usr/bin/at -r ");
		if (iflag)
			strcat(argbuf, "-i ");
		else if (fflag)
			strcat(argbuf, "-f ");
		for (; argc--; argv++) {
			if ((strchr(*argv, '.')) != NULL) { /* job#'s */
				strcat(argbuf, *argv);
				strcat(argbuf, " ");
			}
			else {	/* users  name */
				strcat(argbuf,"-u ");
				strcat(argbuf, *argv);
				strcat(argbuf, " ");
			}
		}
		exit(system(argbuf));
	}
}

usage()
{
	fprintf(stderr, MSGSTR(MS_ATRMUSAGE, 
	"Usage: atrm [-f|-i] [-] [job #|username]\n"));
	exit(1);
}

