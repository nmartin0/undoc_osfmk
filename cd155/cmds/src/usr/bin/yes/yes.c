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
static char rcsid[] = "@(#)$RCSfile: yes.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:50:37 $";
#endif
/*
 * COMPONENT_NAME: (CMDMISC) miscellaneous commands
 *
 * FUNCTIONS:
 *
 * ORIGINS: origins
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
 * yes.c       1.7  com/cmd/misc,3.1,9021 10/27/89 16:41:35
 */
/*
 * NOTE: yes [expletive]
 *      yes repeatedly outputs expletive if given, else if YESSTR
 *      is defined it repeatedly outputs the first value of YESSTR,
 *      otherwise it outputs 'y'(or the argv). Termination is by interruption.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 */

#include <locale.h>
#include <stdlib.h>
#include <string.h>

char *NLgetenv();

main(argc, argv)
char **argv;
{
	char *ystr;
	int in;

	setlocale(LC_ALL, "");

	if (argc>1)
		ystr = argv[1];
	else if (ystr = NLgetenv("YESSTR")) {
		in = strcspn(ystr, ":");
		ystr = strncpy((char *)calloc((size_t)(in+1),(size_t)1),ystr,(size_t)in);
	}
	else
		ystr = "y";

	for (;;)
		printf("%s\n", ystr);
}


