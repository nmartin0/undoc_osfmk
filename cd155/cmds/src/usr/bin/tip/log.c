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
static char rcsid[] = "@(#)$RCSfile: log.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:42:40 $";
#endif
/*
log.c	1.3  com/cmd/tip,3.1,9013 10/15/89 10:42:31";
 */
/* 
 * COMPONENT_NAME: UUCP log.c
 * 
 * FUNCTIONS: MSGSTR, flock, logent, loginit 
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

/* static char sccsid[] = "log.c	5.1 (Berkeley) 4/30/85"; */

#include <time.h>
#include "tip.h"

#ifdef ACULOG
static	FILE *flog = NULL;

/*
 * Log file maintenance routines
 */

logent(group, num, acu, message)
	char *group, *num, *acu, *message;
{
	char *user, *timestamp;
	struct passwd *pwd;
	long t;
	extern char *getlogin();

	if (flog == NULL)
		return;
	if (flock(fileno(flog), LOCK_EX) < 0) {
		perror(MSGSTR(FLOCK, "tip: flock")); /*MSG*/
		return;
	}
	if ((user = getlogin()) == NOSTR)
		if ((pwd = getpwuid(getuid())) == NOPWD)
			user = "???";
		else
			user = pwd->pw_name;
	t = time(0);
	timestamp = ctime(&t);
	timestamp[24] = '\0';
	fprintf(flog, "%s (%s) <%s, %s, %s> %s\n",
		user, timestamp, group,
#ifdef PRISTINE
		"",
#else
		num,
#endif
		acu, message);
	(void) fflush(flog);
	(void) flock(fileno(flog), LOCK_UN);
}

loginit()
{

	flog = fopen(value(LOG), "a");
	if (flog == NULL)
		fprintf(stderr, MSGSTR(CANTOPEN2, "can't open log file\r\n")); /*MSG*/
#endif
}
