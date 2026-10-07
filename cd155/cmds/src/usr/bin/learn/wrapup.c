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
static char rcsid[] = "@(#)$RCSfile: wrapup.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:06 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: wrapup
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
 * wrapup.c	1.2  com/cmd/man/learn,3.1,9021 9/14/89 06:44:54
 */

#include "signal.h"
#include "stdio.h"
#include "lrnref.h"

#include "learn_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

extern char learnrc[];

wrapup(n)
int n;
{
	FILE *fp;
/* this routine does not use 'system' because it wants interrupts turned off */

	signal(SIGINT, SIG_IGN);
	chdir("..");
	if (fork() == 0) {
		signal(SIGHUP, SIG_IGN);
		execl("/usr/bin/rm", "rm", "-rf", dir, 0);
		perror("/usr/bin/rm");
		fprintf(stderr, MSGSTR(LCANTRM, "Wrapup:  can't find 'rm' command.\n")); /*MSG*/
		exit(0);
	}
	if (n == -1)
		unlink(learnrc);
	else if (!n && todo) {
		if ((fp=fopen(learnrc, "w")) == NULL)
			exit(0);
		fprintf(fp, "%s %s %d\n", sname, todo, speed);
		fclose(fp);
	}
	printf(MSGSTR(LBYEDOT, "Bye.\n")); /*MSG*/
	/* not only does this reassure user but it stalls for time while deleting directory */
	fflush(stdout);
	wait(0);
	exit(n);
}
