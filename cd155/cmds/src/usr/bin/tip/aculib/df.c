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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.2
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: df.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:56 $";
#endif
/*
df.c	1.2  com/cmd/tip/aculib,3.1,9013 10/17/89 16:56:45";
 */
/* 
 * COMPONENT_NAME: UUCP df.c
 * 
 * FUNCTIONS: MSGSTR, df02_dialer, df03_dialer, df_abort, df_dialer, 
 *            df_disconnect, timeout 
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

/* static char sccsid[] = "df.c	5.1 (Berkeley) 6/6/85"; */

/*
 * Dial the DF02-AC or DF03-AC
 */

#include "tip.h"

static jmp_buf Sjbuf;
static void timeout();

df02_dialer(num, acu)
	char *num, *acu;
{

	return (df_dialer(num, acu, 0));
}

df03_dialer(num, acu)
	char *num, *acu;
{

	return (df_dialer(num, acu, 1));
}

df_dialer(num, acu, df03)
	char *num, *acu;
	int df03;
{
	register int f = FD;
	struct sgttyb buf;
	int speed = 0, rw = 2;
	char c = '\0';

	ioctl(f, TIOCHPCL, 0);		/* make sure it hangs up when done */
	if (setjmp(Sjbuf)) {
		printf(MSGSTR(TIMEDOUT2, "connection timed out\r\n")); /*MSG*/
		df_disconnect();
		return (0);
	}
	if (boolean(value(VERBOSE)))
		printf(MSGSTR(DIALINGIT, "\ndialing...")); /*MSG*/
	fflush(stdout);
#ifdef TIOCMSET
	if (df03) {
		int st = TIOCM_ST;	/* secondary Transmit flag */
		ioctl(f, TIOCGETP, &buf);
		if (buf.sg_ospeed != B1200) {	/* must dial at 1200 baud */
			speed = buf.sg_ospeed;
			buf.sg_ospeed = buf.sg_ispeed = B1200;
			ioctl(f, TIOCSETP, &buf);
			ioctl(f, TIOCMBIC, &st); /* clear ST for 300 baud */
		} else
			ioctl(f, TIOCMBIS, &st); /* set ST for 1200 baud */
	}
#endif
	signal(SIGALRM, timeout);
	alarm(5 * strlen(num) + 10);
	ioctl(f, TIOCFLUSH, &rw);
	write(f, "\001", 1);
	sleep(1);
	write(f, "\002", 1);
	write(f, num, strlen(num));
	read(f, &c, 1);
#ifdef TIOCMSET
	if (df03 && speed) {
		buf.sg_ispeed = buf.sg_ospeed = speed;
		ioctl(f, TIOCSETP, &buf);
	}
#endif
	return (c == 'A');
}

df_disconnect()
{
	int rw = 2;

	write(FD, "\001", 1);
	sleep(1);
	ioctl(FD, TIOCFLUSH, &rw);
}


df_abort()
{

	df_disconnect();
}


static void
timeout()
{

	longjmp(Sjbuf, 1);
}
