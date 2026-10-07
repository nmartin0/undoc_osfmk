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
 * (c) Copyright 1990, 1991, 1992 OPEN SOFTWARE FOUNDATION, INC. 
 * ALL RIGHTS RESERVED 
 */
/*
 * OSF/1 Release 1.0.4
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: mesg.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:30:13 $";
#endif
/*
 * COMPONENT_NAME: (CMDCOMM) user to user communication
 *
 * FUNCTIONS: mesg
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 * 
 * mesg.c    1.10  com/cmd/comm,3.1,9021 2/7/90 14:17:03
 * mesg.c	4.1 22:03:52 7/15/90 SecureWare 
 */
/*
 *                                                                    
 * mesg -- set current tty to accept or
 *	forbid write permission.
 *
 *	mesg [-y] [-n]
 *		y allow messages
 *		n forbid messages
 *	return codes
 *		0 if messages are ON or turned ON
 *		1 if messages are OFF or turned OFF
 *		2 if usage error
 */

#include <sys/secdefines.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <locale.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/access.h>

#include <nl_types.h>
#include "mesg_msg.h"
nl_catd catd;
#define MSGSTR(Num,Str) catgets(catd,MS_MESG,Num,Str)
char *NLgetenv();

struct stat sbuf;

char *tty;

char *progname;

main(argc, argv)
char *argv[];
{
	int i, c, errflag=0;
	char *ystr = "y";
	char *nstr = "n";

	catd = catopen(MF_MESG,0);
	(void ) setlocale(LC_ALL,"");

	if (ystr = NLgetenv("YESSTR")) {
		i = strcspn(ystr, ":");
		ystr = strncpy((char *)calloc((size_t)(i+1),
			       (size_t)1),
				ystr,(size_t)i);
	} else	ystr = "y";
	if (nstr = NLgetenv("NOSTR")) {
		i = strcspn(nstr, ":");
		nstr = strncpy((char *)calloc((size_t)(i+1),
			       (size_t)1), 
				nstr,(size_t)i);
	} else	nstr = "n";

	progname = argv[0];

	for(i = 0; i <= 2; i++) {
		if ((tty = ttyname(i)) != NULL)
			break;
	}
	if (tty == NULL) {
		fprintf(stderr, MSGSTR(NOTERM,"Cannot find terminal.\n"));
		exit(2);
	}
	if (stat(tty, &sbuf) < 0) {
		fprintf(stderr, 
		         MSGSTR(NOSTATUS,"Cannot get status of \"%s\".\n"), tty);
		exit(2);
	}
	if (argc < 2) {
		if (sbuf.st_mode & 020) {
			printf(MSGSTR(IS,"is %s\n"), ystr); /*MSG*/
			exit(0);
		}
		else  {
			printf(MSGSTR(IS,"is %s\n"), nstr); /*MSG*/
			exit(1);
		}
	}

#define OTHER_WRITE     020
#define YES 1
#define NO  0

	if (argv[1][0] == '-')
		argv[1]++;

	switch(NLyesno(argv[1])) {
		case YES:
			newmode(sbuf.st_mode | OTHER_WRITE);
			exit(0);
		case NO:
			newmode(sbuf.st_mode &~ OTHER_WRITE);
			exit(1);
		default: 
			fprintf(stderr, 
			        MSGSTR(USAGE,"usage: %s [-%s] [-%s]\n"), 
			        progname, ystr, nstr); 
			exit(2);
	}

}


/* 
 * NAME: newmode
 * FUNCTION: change permission mode on tty
 */

newmode(mode)
mode_t	mode;
{
#if SEC_BASE
	/* Deny any WORLD access modes */
	if (chmod(tty, mode & 07770) < 0)
#else
	if ( chmod(tty, mode) < 0 )
#endif
	{
		fprintf(stderr, 
		  MSGSTR(CANTCHG,"Cannot change mode of \"%s\".\n"), tty); /*MSG*/
		exit(2);
	}
}
