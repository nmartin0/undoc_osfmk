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
static char rcsid[] = "@(#)$RCSfile: getpass.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:49 $";
#endif
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: getpass 
 *
 * ORIGINS: 3, 27 
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
 * getpass.c	1.9  com/lib/c/io,3.1,8943 9/12/89 18:23:20
 */

/*LINTLIBRARY*/
#include <stdio.h>
#include <sys/limits.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <termios.h>

static int intrupt;
static int catch(void);

char *
getpass(prompt)
char	*prompt;
{
	struct termios ttyb;
	char     savel;
	tcflag_t flags;
	register char *p;
	register int c;
	FILE	*fi;
	static char pbuf[PASS_MAX+1];
	static int catch();
	sig_t	sig;

	if((fi = fopen("/dev/tty", "r+")) == NULL)
		return((char*)NULL);
	else
		setbuf(fi, (char*)NULL);
	sig = signal(SIGINT, (void (*)(int))catch);
	intrupt = 0;
	(void) tcgetattr(fileno(fi), &ttyb);
	flags = ttyb.c_lflag;
	ttyb.c_lflag &= ~(ECHO | ECHOE | ECHOK | ECHONL);
	(void) tcsetattr(fileno(fi), TCSAFLUSH, &ttyb);
	(void) fputs(prompt, stderr);

	for(p=pbuf; !intrupt && (c = getc(fi)) != '\n' && c != EOF; ) {
		if(p < &pbuf[PASS_MAX])
			*p++ = c;
	}
	*p = '\0';
	(void) putc('\n', stderr);
	ttyb.c_lflag = flags;
	(void) tcsetattr(fileno(fi), TCSADRAIN, &ttyb);
	(void) signal(SIGINT, (void (*)(int))sig);
	if(fi != stdin)
		(void) fclose(fi);
	if(intrupt)
		(void) kill(getpid(), SIGINT);
	return(pbuf);
}

static int
catch()
{
	++intrupt;
}
