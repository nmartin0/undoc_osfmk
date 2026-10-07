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
static char rcsid[] = "@(#)$RCSfile: nohup.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:32:10 $";
#endif
/*
 * COMPONENT_NAME: (CMDCNTL) system control commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * nohup.c	1.6  com/cmd/cntl,3.1,9013 9/10/89 06:22:19
 */

/*
 * Nohup runs commands, ignoring all hangups and quit signals.  If
 * no output is specified, output is redirected to nohup.out.
 */                                                                   

#include <stdio.h>
#include <nl_types.h>
#include "nohup_msg.h"
#define MSGS(n,s)	NLgetamsg(MF_NOHUP,MS_NOHUP,n,s)

#include <locale.h>
#include <signal.h>
#include <stdlib.h>

char	nout[PATH_MAX] = "nohup.out";
extern	char	*sys_errlist[];
extern	int	errno;

main(argc, argv)
char **argv;
{
	char	*home;
	FILE *temp;
	int	err;

	(void) setlocale (LC_ALL,"");
	if(argc < 2) {
		fprintf(stderr,MSGS(USAGE,"usage: nohup command arg ...\n"));   /*MSG*/
		exit(1);
	}
	argv[argc] = 0;
	signal(SIGHUP, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if(isatty(1)) {
		if( (temp = fopen(nout, "a")) == NULL) {
			if((home=getenv("HOME")) == NULL) {
				fprintf(stderr,MSGS(NOCREAT,"nohup: cannot open/create nohup.out\n")); /*MSG*/
				exit(1);
			}
			strcpy(nout,home);
			if (strcmp(home,"/")==0)
				strcat(nout,"nohup.out");
			else
				strcat(nout,"/nohup.out");
			if(freopen(nout, "a", stdout) == NULL) {
				fprintf(stderr,MSGS(NOCREAT,"nohup: cannot open/create nohup.out\n")); /*MSG*/
				exit(1);
			}
		}
		else {
			fclose(temp);
			freopen(nout, "a", stdout);
		}
		fprintf(stderr,MSGS(SENDOUT,"Sending output to %s\n"), nout);   /*MSG*/
	}
	if(isatty(2)) {
		close(2);
		dup(1);
	}
	execvp(argv[1], &argv[1]);
	err = errno;

/* It failed, so print an error */
	freopen("/dev/tty", "w", stderr);
	fprintf(stderr,"%s: %s: %s\n", argv[0], argv[1], sys_errlist[err]);
	exit(1);
}
