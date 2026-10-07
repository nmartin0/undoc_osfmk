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
static char rcsid[] = "@(#)$RCSfile: whatnow.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:06 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: whatnow
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
 * whatnow.c	1.2  com/cmd/man/learn,3.1,9021 9/14/89 06:48:04
 */

#include "stdio.h"
#include "lrnref.h"

#include "learn_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

extern	char	togo[];
extern	int	review;

whatnow()
{
	if (again) {
		if (!review)
			printf(MSGSTR(LTHATWASLESS, "\nOK.  That was lesson %s.\n\n"), todo); /*MSG*/
		fflush(stdout);
		strcpy(level, togo);
		return;
	}
	if (skip) {
		printf(MSGSTR(LOKTHATWAS, "\nOK.  That was lesson %s.\n"), todo); /*MSG*/
		printf(MSGSTR(LSKIPPING, "Skipping to next lesson.\n\n")); /*MSG*/
		fflush(stdout);
		strcpy(level, todo);
		skip = 0;
		return;
	}
	if (todo == 0) {
		more=0;
		return;
	}
	if (didok) {
		strcpy(level,todo);
		if (speed<=9) speed++;
	}
	else {
		speed -= 4;
		/* the 4 above means that 4 right, one wrong leave
		    you with the same speed. */
		if (speed <0) speed=0;
	}
	if (wrong) {
		speed -= 2;
		if (speed <0 ) speed = 0;
	}
	if (didok && more) {
		printf(MSGSTR(LGOODLESSON, "\nGood.  That was lesson %s.\n\n"),level); /*MSG*/
		fflush(stdout);
	}
}
