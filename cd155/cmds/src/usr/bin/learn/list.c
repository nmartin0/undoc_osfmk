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
static char rcsid[] = "@(#)$RCSfile: list.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:59 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: list, stop
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
 * list.c	1.3  com/cmd/man/learn,3.1,9021 11/28/89 13:33:24
 */

#include "stdio.h"
#include "lrnref.h"
#include "signal.h"

int istop;

list(r)
char *r;
{
	int stop(void), intrpt(void);
	FILE *ft;
	char s[LEN_MAX];

	if (r==0)
		return;
	istop = 1;
	signal(SIGINT, (void (*)(int))stop);
	ft = fopen(r, "r");
	if (ft != NULL) {
		while (fgets(s, LEN_MAX, ft) && istop)
			fputs(s, stdout);
		fclose(ft);
	}
	signal(SIGINT, (void (*)(int))intrpt);
}

stop(void)
{
	istop=0;
}
