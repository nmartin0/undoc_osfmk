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
/*
 * Copyright (c) 1988 The Regents of the University of California.
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
 */
/* @(#)logout.c	5.3 (Berkeley) 4/18/89 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: logout.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:38:19 $";
#endif

#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <utmp.h>
#include <stdio.h>

extern struct utmp *getutline(struct utmp *);
extern struct utmp *pututline(struct utmp *);

/* 0 on failure, 1 on success */

logout(line)
	register char *line;
{
	register FILE *fp;
	struct utmp ut;
	struct utmp *utptr;
	int rval;
	time_t time();

	rval = 0;
	setutent();
	strncpy(ut.ut_line, line, sizeof(ut.ut_line));
	if ((utptr = getutline(&ut)) != NULL) {
		utptr->ut_type = DEAD_PROCESS;
		bzero(utptr->ut_user, sizeof(utptr->ut_user));
                bzero(utptr->ut_host, sizeof(utptr->ut_host));
		(void) time(&utptr->ut_time);
		if (pututline(utptr) != NULL) rval = 1;
	}
	endutent();
	return(rval);
}
