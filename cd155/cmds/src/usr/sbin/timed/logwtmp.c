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
static char rcsid[] = "@(#)$RCSfile: logwtmp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:17:47 $";
#endif
/* 
 * COMPONENT_NAME: TCPIP logwtmp.c
 * 
 * FUNCTIONS: logwtmp 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
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
 *
 */
/* logwtmp.c	1.2  com/sockcmd/timed,3.1,9021 10/8/89 17:46:03 */

#include <sys/types.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <utmp.h>


static int fd;

logwtmp(line, name, host)
	char *line, *name, *host;
{
	struct utmp ut;
	struct stat buf;
	time_t time();
	char *strncpy();

	if (!fd && (fd = open(WTMP_FILE, O_WRONLY|O_APPEND, 0)) < 0)
		return;
	if (!fstat(fd, &buf)) {
#ifdef OSF
		/*
		 * Fill in the larger System V utmp structure.
		 * This matches the utmp record the "date" command writes.
		 */
		if (strcmp("|", line) == 0) {
			ut.ut_type = OLD_TIME;
			strncpy(ut.ut_line, OTIME_MSG, sizeof(ut.ut_line));
		}
		else {
			ut.ut_type = NEW_TIME;
			strncpy(ut.ut_line, NTIME_MSG, sizeof(ut.ut_line));
		}
		ut.ut_id[0] = '\0';
		ut.ut_pid = 0;
		ut.ut_exit.e_termination = 0;
		ut.ut_exit.e_exit = 0;
		ut.ut_user[0] = '\0';
		ut.ut_host[0] = '\0';
		(void)time(&ut.ut_time);

#else
		(void)strncpy(ut.ut_line, line, sizeof(ut.ut_line));
		(void)strncpy(ut.ut_name, name, sizeof(ut.ut_name));
		(void)strncpy(ut.ut_host, host, sizeof(ut.ut_host));
		(void)time(&ut.ut_time);
#endif /* OSF */
		if (write(fd, (char *)&ut, sizeof(struct utmp)) !=
		    sizeof(struct utmp))
			(void)ftruncate(fd, buf.st_size);
	}
}
