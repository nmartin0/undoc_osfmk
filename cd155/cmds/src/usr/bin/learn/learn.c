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
static char rcsid[] = "@(#)$RCSfile: learn.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:28:57 $";
#endif

/*
 * COMPONENT_NAME: (CMDMAN) commands that allow users to read online
 * documentation
 *
 * FUNCTIONS: hangup, intrpt
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
 * learn.c	1.3  com/cmd/man/learn,3.1,9021 11/28/89 12:56:10
 */

#include "stdio.h"
#include "lrnref.h"
#include "signal.h"

#include "learn_msg.h" 
nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_LEARN,n,s) 

char	*direct	= "/usr/lib/learn";	/* CHANGE THIS ON YOUR SYSTEM */
int	more;
char	*level;
int	speed;
char	*sname;
char	*todo;
FILE	*incopy	= NULL;
int	didok;
int	sequence	= 1;
int	comfile	= -1;
int	status;
int	wrong;
char	*pwline;
char	*dir;
FILE	*scrin;
int	logging	= 0;	/* set to 0 to turn off logging */
int	ask;
int	again;
int	skip;
int	teed;
int	total;

main(argc,argv)
int argc;
char *argv[];
{
	extern hangup(void), intrpt(void);
	extern char * getlogin(), *malloc();

	catd = catopen(MF_LEARN,0);
	speed = 0;
	more = 1;
	pwline = getlogin();
	selsub(argc, argv);
	chgenv();
	signal(SIGHUP, (void (*)(int))hangup);
	signal(SIGINT, (void (*)(int))intrpt);
	while (more) {
		selunit();
		dounit();
		whatnow();
	}
	wrapup(0);
}

hangup(void)
{
	wrapup(1);
}

intrpt(void)
{
	char response[LEN_MAX], *p;

	signal(SIGINT, (void (*)(int))hangup);
	write(2, MSGSTR(LINTERRUPT, "\nInterrupt.\nWant to go on?  "), strlen(MSGSTR(LINTERRUPT, "\nInterrupt.\nWant to go on?  "))); /*MSG*/
	p = response;
	*p = 'n';
	while (read(0, p, 1) == 1 && *p != '\n')
		p++;
	*p = NULL;
	if (NLyesno(response) != 1)
		wrapup(0);
	ungetwc((NLchar)'\n', stdin);
	signal(SIGINT, (void (*)(int))intrpt);
}
