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
static char rcsid[] = "@(#) $RCSfile: unexpand.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:44:44 $";
#endif

/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: unexpand
 *
 * ORIGINS: 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */
#if !defined(lint)
static char sccsid[] = "unexpand.c	1.6  com/cmd/files,3.1,9008 9/11/89 14:40:14";
#endif

#include <stdio.h>
#include <locale.h>

#include "expand_msg.h" 
#define MSGSTR(n,s) NLgetamsg(MF_EXPAND, MS_EXPAND, n, s) 

char	genbuf[BUFSIZ];
char	linebuf[BUFSIZ];
int	all;
char *progname;

static void tabify();

/*
 * NAME: unexpand [-a]
 *
 * FUNCTION: put tabs into a file replacing blanks
 * NOTE:   If -a is used then tabs are inserted whenever they would 
 *         compress the resultant file by replacing two or more characters.
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	register char *cp;
	progname = argv[0];

	(void) setlocale(LC_ALL,"");
	argc--, argv++;
	if (argc > 0 && argv[0][0] == '-') {
		if (strcmp(argv[0], "-a") != 0) {
			(void)fprintf(stderr,MSGSTR(USAGE,"usage: %s [-a] file ...\n")
				,progname);
			exit(1);
		}
		all++;
		argc--, argv++;
	}
	do {
		if (argc > 0) {
			if (freopen(argv[0], "r", stdin) == NULL) {
				perror(argv[0]);
				exit(1);
			}
			argc--, argv++;
		}
		while (fgets(genbuf, BUFSIZ, stdin) != NULL) {
			for (cp = linebuf; *cp; cp++)
				continue;
			if (cp > linebuf)
				cp[-1] = 0;
			tabify(all);
			(void)printf("%s", linebuf);
		}
	} while (argc > 0);
	return(0);
}

/*
 * NAME: tabify
 *
 * FUNCTION:  add tabs where need.
 */
static void
tabify(c)
	char c;
{
	register char *cp, *dp;
	register int dcol;
	int ocol;

	ocol = 0;
	dcol = 0;
	cp = genbuf, dp = linebuf;
	for (;;) {
		switch (*cp) {

		case ' ':
			dcol++;
			break;

		case '\t':
			dcol += 8;
			dcol &= ~07;
			break;

		default:
			while (((ocol + 8) &~ 07) <= dcol) {
				if (ocol + 1 == dcol)
					break;
				*dp++ = '\t';
				ocol += 8;
				ocol &= ~07;
			}
			while (ocol < dcol) {
				*dp++ = ' ';
				ocol++;
			}
			if (*cp == 0 || c == 0) {
				(void)strcpy(dp, cp);
				return;
			}
			*dp++ = *cp;
			ocol++, dcol++;
		}
		cp++;
	}
}
