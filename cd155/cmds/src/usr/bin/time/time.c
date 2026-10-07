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
static char rcsid[] = "@(#)$RCSfile: time.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:25 $";
#endif
/*
 * COMPONENT_NAME: (CMDSTAT) status
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
 * time.c	1.10  com/cmd/stat,3.1,9008 12/21/89 22:10:46";
 */

#include	<stdio.h>
#include	<signal.h>
#include	<errno.h>
#include	<time.h> 		/* CLK_TCK defined here */
#include	<locale.h>

/* NO NLS modifications required. */
#include <nl_types.h>
#include "time_msg.h"
nl_catd	catd;
#define MSGSTR(n,s)	NLcatgets(catd,MS_TIME,n,s)
 
static void printt();
char errbuffer[BUFSIZ];

main(argc, argv)
int argc;
char **argv;
{
	struct {
		long user;
		long sys;
		long childuser;
		long childsys;
	} buffer;

	register int p;
	extern	errno;
	extern	char	*sys_errlist[];
	int	status;
	clock_t	before, after;
	extern long times();

	(void) setlocale (LC_ALL,"");
	catd = NLcatopen(MF_TIME,0);
	setbuf(stderr,errbuffer);

	before = times(&buffer);
	if(argc<=1)
		exit(0);
	p = fork();
	if(p == -1) {
		fprintf(stderr,MSGSTR(NOFORK,"time: cannot fork -- try again.\n"));
		exit(2);
	}
	if(p == 0) {
/*		close(1);	lem commented this out	*/
		execvp(argv[1], &argv[1]);
	        fprintf(stderr, "%s: %s\n", sys_errlist[errno], argv[1]);
		exit(2);
	}
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	while(wait(&status) != p);
	if((status & 0377) != '\0')
		fprintf(stderr,MSGSTR(ABTERM, "time: command terminated abnormally.\n"));	/*MSG*/
	after = times(&buffer);
	fprintf(stderr,"\n");
	printt(MSGSTR(REAL,"real"), (after-before));
	printt(MSGSTR(USER,"user"), buffer.childuser);
	printt(MSGSTR(SYS,"sys "), buffer.childsys);
	exit(status >> 8);
}

/*
The following use of CLK_TCK/10 will work correctly only if 
CLK_TCK is a multiple of 10.  However the only values for CLK_TCK now 
in use are 100 for the 3B and 60 for other machines.

But now the DECstation comes along and has a CLK_TCK of 64, so
we just do the integer division and realize we will have some error.
*/
char quant[] = { CLK_TCK/10, 10, 10, 6, 10, 6, 10, 10, 10 }; 
char *pad  = "000      ";
char *sep  = "\0\0.\0:\0:\0\0";
char *nsep = "\0\0.\0 \0 \0\0";


/*
 *  NAME:  printt
 *
 *  FUNCTION:	prints out the string followed by a number.
 *		The number is right justified and buffered.
 *	      
 *  RETURN VALUE:  	 void
 */

static void
printt(s, a)
char *s;
long a;
{
	register int i;
	char	digit[9];
	char	c;
	int	nonzero;


	for(i=0; i<9; i++) {
		digit[i] = a % quant[i];
		a /= quant[i];
	}
	fprintf(stderr,s);
	nonzero = 0;
	while(--i>0) {
		c = digit[i]!=0 ? digit[i]+'0':
		    nonzero ? '0':
		    pad[i];
		if (c != '\0')
			putc (c, stderr);
		nonzero |= digit[i];
		c = nonzero?sep[i]:nsep[i];
		if (c != '\0')
			putc (c, stderr);
	}
	fprintf(stderr,"\n");
}
