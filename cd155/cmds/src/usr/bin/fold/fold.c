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
static char rcsid[] = "@(#)$RCSfile: fold.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:25:01 $";
#endif
/*
 * COMPONENT_NAME: (CMDFILES) commands that manipulate files
 *
 * FUNCTIONS: fold 
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
 * fold.c    1.8  com/cmd/files,3.1,9021 3/18/90 16:46:38
 */
/*
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

#include <NLchar.h>
#include <NLctype.h>
#include <stdio.h>
#include <locale.h>
#include "fold_msg.h" 

#define MSGSTR(n,s) NLgetamsg(MF_FOLD, MS_FOLD, n, s) 

/*
 * NAME:  fold [-width] [file ...]
 * FUNCTION:  fold long lines for finite output devices
 */

int	fold =  80;
int	col;
int	lastc;

main(argc, argv)
	int argc;
	char *argv[];
{
	register c;

	(void) setlocale(LC_ALL,"");
	argc--, argv++;
	if (argc > 0 && argv[0][0] == '-') {
		fold = 0;
		argv[0]++;
		while (*argv[0] >= '0' && *argv[0] <= '9')
			fold *= 10, fold += *argv[0]++ - '0';
		if (*argv[0]) {
			fprintf(stderr, MSGSTR(BADNUM, "Bad number for fold\n")); /*MSG*/
			exit(1);
		}
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
		for (;;) {
			lastc = c;
			c = getwc(stdin);
			if (c == -1)
				break;
			putch(c);
		}
	} while (argc > 0);
	exit(0);
}

/*
 * NAME: putch
 * FUNCTION: put character c out to stdout but, first check for special 
 *    characters
 */
putch(c)
	NLchar c;
{
	register ncol;

	switch (c) {
		case '\n':
			ncol = 0;
			break;
		case '\t':
			ncol = (col + 8) &~ 7;
			break;
		case '\b':
#if defined(KJI)
			ncol = col ? col - NCchrlen(lastc) : 0;
#else
			ncol = col ? col - 1 : 0;
#endif
			break;
		case '\r':
			ncol = 0;
			break;
		default:
#if defined(KJI) 
			ncol = col + NCchrlen(c);
#else
			ncol = col + 1;
#endif
	}
	if (ncol > fold)
		putc('\n',stdout), col = 0;
	putwc(c,stdout);
	switch (c) {
		case '\n':
			col = 0;
			break;
		case '\t':
			col += 8;
			col &= ~7;
			break;
		case '\b':
			if (col)
#if defined(KJI)
				col -= NCchrlen(lastc);
#else
				col--;
#endif
			break;
		case '\r':
			col = 0;
			break;
		default:
#if defined(KJI)
			col+= NCchrlen(c);
#else
			col++;
#endif
			break;
	}
}
