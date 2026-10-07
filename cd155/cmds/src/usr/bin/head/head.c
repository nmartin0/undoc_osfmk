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
static char rcsid[] = "@(#)$RCSfile: head.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:00 $";
#endif
/*
 * COMPONENT_NAME: (CMDSCAN) commands that scan files
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
 */

/*
 *                                                                    
 * FUNCTION:	Displays the first "count" of lines of each of the 
 *		specified files, or of the standard input.  If "count"
 *		is omitted, it defaults to 10.
 *
 */  


#include <stdio.h>
#include <locale.h>
#include        <nl_types.h>
#include        "head_msg.h"

#define         MSGSTR(num,str) catgets(catd,MS_HEAD,num,str)  /*MSG*/

nl_catd         catd;

static void copyout();
static int  getnum();

main(argc, argv)
int argc;
char *argv[];
{
	int linecnt = 10;	/* Default number of file lines to print */
	int numfiles = --argc;  /* Number of files to be printed         */
	char *name;             /* Name of file to be printed            */
	int morethanone = 0;    /* Flag to indicate more than one file   */

	(void) setlocale(LC_ALL,"");
	catd = catopen(MF_HEAD, 0);

	argv++;
	do {
		/* retrieve "count" if present */
		while (argc > 0 && argv[0][0] == '-') {
			linecnt = getnum(argv[0] + 1);
			argc--;
			argv++;
			numfiles--;
		}

		/* If extra -"count" at end, break (i.e. head -20 file1 -10) */
		if (argc == 0 && morethanone)
			break;

		/* If filename present, substitute file for stdin */
		if (argc > 0) {
			close(0);
			if (freopen(argv[0], "r", stdin) == NULL) {
				perror(argv[0]);
				exit(1);
			}
			name = argv[0];
			argc--;
			argv++;
		} else /* use stdin */
			name = 0;

		/* If more than one file to be printed, 
		   put space between files and print name  */
		if (morethanone)
			putchar('\n');
		morethanone++;
		if (numfiles > 1 && name)
			printf("==> %s <==\n", name);

		/* Print out specified number of lines */
		copyout(linecnt);
		fflush(stdout);
	} while (argc > 0);
}

/* Print "count" lines of file */
static void
copyout(cnt)
int cnt;
{
	char lbuf[BUFSIZ];

	while (cnt > 0 && fgets(lbuf,(int)sizeof(lbuf), stdin) != 0) {
		printf("%s", lbuf);
		fflush(stdout);
		cnt--;
	}
}

/* Retrieve from command line number of lines of each file to be printed */
/* Return value: number of lines.   Like atoi but does error checking.   */
static int
getnum(cp)
char *cp;
{
	int i;

	for (i = 0; *cp >= '0' && *cp <= '9'; cp++)
		i *= 10, i += *cp - '0';
	if (*cp) {
		fprintf(stderr, MSGSTR(BADNUM, "Badly formed number\n"));
		exit(1);
	}
	return (i);
}
