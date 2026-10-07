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
static char rcsid[] = "@(#)$RCSfile: tipout.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:48 $";
#endif
/*
static char sccsid[] = "@(#)tipout.c	1.4  com/cmd/tip,3.1,9013 12/7/89 08:43:54";
*/
/* 
 * COMPONENT_NAME: UUCP tipout.c
 * 
 * FUNCTIONS: intEMT, intIOT, intSYS, intTERM, sigmask, tipout 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/* static char sccsid[] = "tipout.c	5.1 (Berkeley) 4/30/85"; */

#include "tip.h"
/*
 * tip
 *
 * lower fork of tip -- handles passive side
 *  reading from the remote host
 */

static	jmp_buf sigbuf;

/*
 * TIPOUT wait state routine --
 *   sent by TIPIN when it wants to posses the remote host
 */
intIOT()
{

	write(repdes[1],&ccc,1);
	read(fildes[0], &ccc,1);
	longjmp(sigbuf, 1);
}

/*
 * Scripting command interpreter --
 *  accepts script file name over the pipe and acts accordingly
 */
intEMT()
{
	char c, line[256];
	register char *pline = line;
	char reply;

	read(fildes[0], &c, 1);
	while (c != '\n') {
		*pline++ = c;
		read(fildes[0], &c, 1);
	}
	*pline = '\0';
	if (boolean(value(SCRIPT)) && fscript != NULL)
		fclose(fscript);
	if (pline == line) {
		boolean(value(SCRIPT)) = FALSE;
		reply = 'y';
	} else {
		if ((fscript = fopen(line, "a")) == NULL)
			reply = 'n';
		else {
			reply = 'y';
			boolean(value(SCRIPT)) = TRUE;
		}
	}
	write(repdes[1], &reply, 1);
	longjmp(sigbuf, 1);
}

intTERM()
{

	if (boolean(value(SCRIPT)) && fscript != NULL)
		fclose(fscript);
	exit(0);
}

intSYS()
{

	boolean(value(BEAUTIFY)) = !boolean(value(BEAUTIFY));
	longjmp(sigbuf, 1);
}


/*
 * ****TIPOUT   TIPOUT****
 */
tipout()
{
	char buf[BUFSIZ];
	register char *cp;
	register int cnt;
	extern int errno;
	int omask;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGEMT, intEMT);		/* attention from TIPIN */
	signal(SIGTERM, intTERM);	/* time to go signal */
	signal(SIGIOT, intIOT);		/* scripting going on signal */
	signal(SIGHUP, intTERM);	/* for dial-ups */
	signal(SIGSYS, intSYS);		/* beautify toggle */
	(void) setjmp(sigbuf);
	for (omask = 0;; sigsetmask(omask)) {
		cnt = read(FD, buf, BUFSIZ);
		if (cnt <= 0) {
			/* lost carrier */
			if (cnt < 0 && errno == EIO) {
				sigblock(sigmask(SIGTERM));
				intTERM();
				/*NOTREACHED*/
			}
			continue;
		}
#define	ALLSIGS	sigmask(SIGEMT)|sigmask(SIGTERM)|sigmask(SIGIOT)|sigmask(SIGSYS)
		omask = sigblock(ALLSIGS);
		for (cp = buf; cp < buf + cnt; cp++)
			*cp &= 0177;
		write(1, buf, cnt);
		if (boolean(value(SCRIPT)) && fscript != NULL) {
			if (!boolean(value(BEAUTIFY))) {
				fwrite(buf, 1, cnt, fscript);
				continue;
			}
			for (cp = buf; cp < buf + cnt; cp++)
				if ((*cp >= ' ' && *cp <= '~') ||
				    any(*cp, value(EXCEPTIONS)))
					putc(*cp, fscript);
		}
	}
}
