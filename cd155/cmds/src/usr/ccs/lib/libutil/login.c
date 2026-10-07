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
/* @(#)login.c	5.2 (Berkeley) 4/18/89 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: login.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:38:18 $";
#endif

#include <sys/types.h>
#include <sys/file.h>
#include <utmp.h>
#include <stdio.h>

extern struct utmp *getutent(void);
extern struct utmp *pututline(struct utmp *);

void
login(ut)
	struct utmp *ut;
{
	register int fd;
	int tty;
	off_t lseek();
	struct utmp *xut;

	/*
	 * The bsd programs which call this routine will fill in the
	 * bsd elements of the utmp structure: name, line, time and host
	 */
	setutent();
	ut->ut_type = USER_PROCESS;
	ut->ut_pid = getpid();
	ut->ut_exit.e_exit = 0;
	ut->ut_exit.e_termination = 0;

	/*
	 * look for a slot in the utmp file with our pid 
	 * If we find one, when use that id (which is probably from inittab
	 */
	while ((xut = getutent()) != NULL) 
		if (xut->ut_pid == ut->ut_pid) {
		    strncpy(ut->ut_id, xut->ut_id, sizeof(ut->ut_id));
		    break;
		}
	/*
	 * If we couldn't find our pid, then use the last two characters
	 * of the tty name (probably p0, p1, etc)
	 */
	if (xut == NULL)
		strncpy(ut->ut_id , ut->ut_line + strlen(ut->ut_line)-2, 2);

	(void)pututline(ut);
	endutent();

	if ((fd = open(WTMP_FILE, O_WRONLY|O_APPEND, 0)) >= 0) {
		(void)write(fd, (char *)ut, sizeof(struct utmp));
		(void)close(fd);
	}
}
