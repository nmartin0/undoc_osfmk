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
static char rcsid[] = "@(#)$RCSfile: tty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:44:33 $";
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
 * tty.c	1.6  com/cmd/stat,3.1,9008 9/8/89 08:01:56
 */

#include	<stdio.h>
#include	<locale.h>
char	*ttyname();
extern int	optind;
int		sflg;

#include "tty_msg.h"

#define MSGSTR(Num, Str) NLgetamsg(MF_TTY, MS_TTY, Num, Str)
nl_catd	catd;

main(argc, argv)
int argc;
char **argv;
{
	register char *p;
	register int	i;

	(void) setlocale (LC_ALL,"");

	while((i = getopt(argc, argv, "s?")) != EOF)
		switch(i) {
		case 's':
			sflg = 1;
			break;
		case '?':
			printf(MSGSTR(USAGE, "Usage: tty [-s]\n"));
			exit(2);
		}
	p = ttyname(0);
	if(!sflg)
		printf("%s\n", (p? p: MSGSTR(NOTTTY, "not a tty")));
	exit(p? 0: 1);
}
