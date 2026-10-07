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
static char rcsid[] = "@(#)$RCSfile: sleep.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:44 $";
#endif
/*
 * COMPONENT_NAME: (CMDCNTL) system control commands
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
 * sleep.c	1.6  com/cmd/cntl,3.1,9008 9/10/89 06:22:49";
 */

/*
 *	sleep -- suspend execution for an interval
 */                                                                   

#include	<stdio.h>

#include <locale.h>
#include "sleep_msg.h"

#define MSGSTR(c,d)	NLgetamsg(MF_SLEEP,MS_SLEEP,c,d)

main(argc, argv)
char **argv;
{
	int	c, n=0;
	char	*s;

	(void) setlocale (LC_ALL,"");
	if(argc < 2) {
		fprintf(stderr, MSGSTR(USG,"usage: sleep time\n"));	/*MSG*/
		exit(2);
	}
	s = argv[1];
	while(c = *s++) {
		if(c<'0' || c>'9') {
			fprintf(stderr, MSGSTR(BADCHAR,			/*MSG*/
				"sleep: bad character in argument\n"));	/*MSG*/
			exit(2);
		}
		n = n*10 + c - '0';
	}
	(void) sleep(n);	/* Don't worry how long we actually slept. */
	exit (0);
}
