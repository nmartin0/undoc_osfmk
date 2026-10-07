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
static char rcsid[] = "@(#)$RCSfile: echo.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:23:06 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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
 *
 * com/cmd/sh,3.1,9021 9/13/89 20:59:04";
 */

#include <locale.h>

#ifdef KJI
#include <NLctype.h>
#endif

/*
 * NAME:	echo
 *
 * FUNCTION:	echo - write arguments to standard output
 *
 * SYNOPSIS:	echo [-n] [arg] ...
 *
 * NOTES:	Echo writes its arguments to standard output.
 *		Escape characters are recognized and processed.
 *
 *		If the '-n' flag is used, no newline is added.
 *
 * RETURN VALUE DESCRIPTION:	0
 */

int
main(argc, argv)
int argc;
char **argv;
{
	register char	*cp;
	register int	i, wd;
	int	j, newline;

	(void) setlocale (LC_ALL, "");

	/*
	 * any arguments at all?  if not, exit
	 */
	if(--argc == 0) {
		putchar('\n');
		return (0);
	}

	/*
	 * check for -n ...
	 */
	if (strcmp(argv[1], "-n") == 0) {
		newline = 0;
		argc--;
		argv++;
		}
	else
		newline = 1;

	/*
	 * process arguments ...
	 */
	for(i = 1; i <= argc; i++) {
		for(cp = argv[i]; *cp; cp++) {
			if(*cp == '\\')
				/*
				 * process escape sequences
				 */
				switch(*++cp) {
					case 'b':	/* backspace	*/
						putchar('\b');
						continue;

					case 'c':	/* no newline	*/
						return (0);

					case 'f':	/* formfeed	*/
						putchar('\f');
						continue;

					case 'n':	/* newline	*/
						putchar('\n');
						continue;

					case 'r':	/* carriage return */
						putchar('\r');
						continue;

					case 't':	/* tab	*/
						putchar('\t');
						continue;

					case '\\':	/* backslash	*/
						putchar('\\');
						continue;

					case '0':	/* octal char	*/
						j = wd = 0;
						while ((*++cp >= '0' &&
						   *cp <= '7') && j++ < 3) {
							wd <<= 3;
							wd |= (*cp - '0');
							}
						putchar(wd);
						--cp;
						continue;

					default:
						cp--;
				}

			putchar(*cp);
#ifdef KJI
			if (NCisshift(*cp)) /* if a 2 byte character */
			    putchar(*++cp); /* output second byte */
#endif
		}

		/*
		 * space between arguments...
		 */
		if (i < argc)
			putchar(' ');
	}

	/*
	 * ending newline
	 */
	if (newline)
		putchar('\n');

	return (0);
}
