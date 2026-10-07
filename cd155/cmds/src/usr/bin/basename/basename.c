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
  static char rcsid[] = "@(#)$RCSfile: basename.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:19:25 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: 26, 27
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
 * basename.c	1.6  com/cmd/sh,3.1,9008 9/13/89 20:58:10";
 */

#include	<stdio.h>

/*
 * NAME: basename
 * FUNCTION: Returns the base naem of a string parameter
 */
main(argc, argv)
int argc;
char **argv;
{
	register char *p1, *p2, *p3;

	if (argc < 2) {
		putchar('\n');
		exit(1);
	}
	p1 = argv[1];
	p2 = p1;
	while (*p1) {
		if (*p1++ == '/')
			p2 = p1;
	}
	if (argc>2) {
		for(p3=argv[2]; *p3; p3++) 
			;
		while(p1>p2 && p3>argv[2])
			if(*--p3 != *--p1)
				goto output;
		*p1 = '\0';
	}
output:
	puts(p2);
	exit(0);
}
