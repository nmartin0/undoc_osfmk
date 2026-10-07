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
static char rcsid[] = "@(#) $RCSfile: expand.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:32 $";
#endif

/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: expand
 *
 * ORIGINS: 3, 26, 27
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
static char sccsid[] = "expand.c	1.6  com/cmd/files,3.1,9021 9/11/89 14:39:54";
#endif

#include <stdio.h>
#include <locale.h>
#include <limits.h>

#include "expand_msg.h" 
#define MSGSTR(n,s) NLgetamsg(MF_EXPAND, MS_EXPAND, n, s) 

int	nstops = 0;
int	tabstops[100];

/*
 * NAME: expand [-tabstop] [-tab1,tab2,...,tabn] [file...]
 *
 * FUNCTION: expand tabs to equivalent spaces
 * NOTE:  Default tab is 8.
 *        If -tabstop is given then the width of each tab is tabstop.
 *        If -tab1,tab2,...,tabn is given then tabs are set at those 
 *        specific columns.
 */
main(argc, argv)
	int argc;
	char *argv[];
{
	register int c, column;
	register int n;

	(void) setlocale(LC_ALL,"");
	argc--, argv++;
	do {
		while (argc > 0 && argv[0][0] == '-') {
			getstops(argv[0]);
			argc--, argv++;
		}
		if (argc > 0) {
			if (freopen(argv[0], "r", stdin) == NULL) {
				perror(argv[0]);
				exit(1);
			}
			argc--, argv++;
		}
		column = 0;
		for (;;) {
			c = getc(stdin);
			if (c == -1)
				break;
			switch (c) {

			case '\t':
				if (nstops == 0) {
					do {
						putchar(' ');
						column++;
					} while (column & 07);
					continue;
				}
				if (nstops == 1) {
					do {
						putchar(' ');
						column++;
					} while (((column - 1) % tabstops[0]) != (tabstops[0] - 1));
					continue;
				}
				for (n = 0; n < nstops; n++)
					if (tabstops[n] > column)
						break;
				if (n == nstops) {
					putchar(' ');
					column++;
					continue;
				}
				while (column < tabstops[n]) {
					putchar(' ');
					column++;
				}
				continue;

			case '\b':
				if (column)
					column--;
				putchar('\b');
				continue;

			default:
				putchar(c);
				column++;
				continue;

			case '\n':
				putchar(c);
				column = 0;
				continue;
			}
		}
	} while (argc > 0);
	return(0);
}

/*
 * NAME: getstops
 *
 * FUNCTION: checks for tabs that will make the string too long and for
 *           tab stops that are too small.
 */
getstops(cp)
	register char *cp;
{
	register int i;

	cp++;
	for (;;) {
		i = 0;
		while (*cp >= '0' && *cp <= '9')
			i = i * 10 + *cp++ - '0';
		if (i <= 0 || i >= CHAR_MAX) {
bad:
			(void)fprintf(stderr,MSGSTR(BADTAB, "Bad tab stop spec\n"));
			exit(1);
		}
		if (nstops > 0 && i <= tabstops[nstops-1])
			goto bad;
		tabstops[nstops++] = i;
		if (*cp == 0)
			break;
		if (*cp++ != ',')
			goto bad;
	}
}
