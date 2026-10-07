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
static char rcsid[] = "@(#)$RCSfile: startdaemon.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:08:39 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * startdaemon.c	5.3 (Berkeley) 6/30/88
 */


/*
 * Tell the printer daemon that there are new files in the spool directory.
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include "lp.local.h"
#include "printer_msg.h"

nl_catd catd;
#define MSGSTR(n,s) catgets(catd,MS_PRINTER,n,s)

/* undefine sun which is declared by cpp on suns */
#undef	sun

startdaemon(printer)
	char *printer;
{
	struct sockaddr_un sun;
	register int s, n;
	char buf[BUFSIZ];
	static perr();

	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if (s < 0) {
		perr("socket");
		return(0);
	}
	sun.sun_family = AF_UNIX;
	strcpy(sun.sun_path, SOCKETNAME);
	if (connect(s, &sun, strlen(sun.sun_path) + 2) < 0) {
		perr("connect");
		(void) close(s);
		return(0);
	}
	(void) sprintf(buf, "\1%s\n", printer);
	n = strlen(buf);
	if (write(s, buf, n) != n) {
		perr("write");
		(void) close(s);
		return(0);
	}
	if (read(s, buf, 1) == 1) {
		if (buf[0] == '\0') {		/* everything is OK */
			(void) close(s);
			return(1);
		}
		putchar(buf[0]);
	}
	while ((n = read(s, buf, sizeof(buf))) > 0)
		fwrite(buf, 1, n, stdout);
	(void) close(s);
	return(0);
}

static
perr(msg)
	char *msg;
{
	extern char *name;
	extern int sys_nerr;
	extern char *sys_errlist[];
	extern int errno;

	fprintf(stderr,"%s: %s: ", name, msg);
	fputs(errno < sys_nerr ? sys_errlist[errno] : MSGSTR(STARTDAEMON_1, "Unknown error") , stderr);
	putc('\n',stderr);
}

