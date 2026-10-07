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
static char rcsid[] = "@(#)$RCSfile: begin.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:54 $";
#endif
/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: begin
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
 * begin.c	1.2  com/cmd/man/learn,3.1,9021 9/14/89 06:29:08
 */

#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#undef direct			/* Temp fix for namespace pollution */
#include "lrnref.h"

#include <NLctype.h>

#include "learn_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

begin(lesson)
char *lesson;
{

  int c, n;
  char where [LEN_MAX];
  DIR *dp;
  struct dirent *ep;
  NLchar NLtemp;

	if (((dp = opendir(".")) == NULL)) {	/* clean up play directory */
		perror(MSGSTR(LSTPLYDIR, "Start:  play directory")); /*MSG*/
		wrapup(1);
	}	
	for (ep = readdir(dp); ep != NULL; ep = readdir(dp)) {
		if (ep->d_fileno == 0)
			continue;
		n = ep->d_namlen;
		if (ep->d_name[n-2] == '.' && ep->d_name[n-1] == 'c')
			continue;
		NCdec(ep->d_name, &NLtemp);
		if (NCisalpha(NLtemp))
			unlink(ep->d_name);
	}
	closedir(dp);
	if (ask)
		return;
	sprintf(where, "%s/%s/L%s", direct, sname, lesson);
	if (access(where, 04)==0)	/* there is a file */
		return;
	perror(where);
	fprintf(stderr, MSGSTR(LSTRTNOLESS, "Start:  no lesson %s\n"),lesson); /*MSG*/
	wrapup(1);
}

