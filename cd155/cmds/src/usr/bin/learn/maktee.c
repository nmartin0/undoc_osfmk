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
static char rcsid[] = "@(#)$RCSfile: maktee.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:29:01 $";
#endif
 
/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: maktee, untee
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
 * maktee.c	1.2  com/cmd/man/learn,3.1,9021 9/14/89 06:40:08
 */

#include "stdio.h"
#include "signal.h"
#include "lrnref.h"

#include "learn_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

static int oldout;
static char tee[50];

maktee()
{
	int fpip[2], in, out;

	if (tee[0] == 0)
		sprintf(tee, "%s/bin/lrntee", direct);
	pipe(fpip);
	in = fpip[0];
	out= fpip[1];
	if (fork() == 0) {
		signal(SIGINT, SIG_IGN);
		close(0);
		close(out);
		dup(in);
		close(in);
		execl (tee, "lrntee", 0);
		perror(tee);
		fprintf(stderr, MSGSTR(LMAKTEE, "Maktee:  lrntee exec failed\n")); /*MSG*/
		exit(1);
	}
	close(in);
	fflush(stdout);
	oldout = dup(1);
	close(1);
	if (dup(out) != 1) {
		perror("dup");
		fprintf(stderr, MSGSTR(LERRORTEE, "Maktee:  error making tee for copyout\n")); /*MSG*/
	}
	close(out);
	return(1);
}

untee()
{
	int x;

	fflush(stdout);
	close(1);
	dup(oldout);
	close(oldout);
	wait(&x);
}
