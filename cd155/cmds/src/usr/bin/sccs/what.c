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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: what.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:37:58 $";
#endif
/*
 * COMPONENT_NAME: CMDSCCS      Source Code Control System (sccs)
 *
 * FUNCTIONS: dowhat, main
 *
 * ORIGINS: 3, 10, 27
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
 * what.c 1.4 com/cmd/sccs/cmd,3.1,9021 9/15/89 13:39:09";
 */


#include <stdio.h>

#define MINUS_S "-s"
#define TRUE  1
#define FALSE 0

int found;
int silent;

#define pat0    '@'
char pat1[]  =  "(#)";

main(argc,argv)
int argc;
register char **argv;
{
	register int i;
	register done = FALSE;

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i],MINUS_S) == 0) {
			silent = TRUE;
			continue;
		}
		if (!(freopen(argv[i],"r",stdin)))
			perror(argv[i]);
		else {
			if (!(argc == 2 || (silent && (argc == 3))))
				printf("%s:\n",argv[i]);
			dowhat();
		}
		done = TRUE;
	}
	if (!done)
		dowhat();
	exit(!found);				/* shell return code */
}


dowhat()
{
	register int c;
	register char *p;

	while ((c = getchar()) != EOF)
		while (c == pat0)
			for (p = pat1;;) {
				if ((c = getchar()) == EOF) return;
				if (c != *p) break;
				if (!*++p) {
					putchar('\t');
					while ((c = getchar()) != EOF && c &&
					    !any(c,"\"\\>\n"))
						putchar(c);
					putchar('\n');
					found = TRUE;
					if (silent) return;
					break;
				}
			}
}
