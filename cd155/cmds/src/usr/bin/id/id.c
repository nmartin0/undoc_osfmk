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
static char rcsid[] = "@(#)$RCSfile: id.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:26:14 $";
#endif
/*
 * COMPONENT_NAME: (CMDSAUTH) security: authentication functions
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 27
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
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 *
 * id.c		1.9  com/cmd/sh,3.1,9021 12/20/89 10:06:56;
 */

#include <stdio.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <locale.h>

#include "id_msg.h"

void puid(), pgid();
extern uid_t getuid(), geteuid();
extern gid_t getgid(), getegid();

/*
 * NAME:	id
 *
 * SYNTAX:	id
 *
 * FUNCTION:	id - display system identity of user
 *
 * NOTES:	Id displays the user and group ids (and corresponding
 *		names) of the invoking process.  If effective user and
 *		group ids do not match their corresponding ids, they
 *		are printed also.
 *
 * RETURN VALUE DESCRIPTION:	0
 */

int
main()
{
	uid_t uid, euid;
	gid_t gid, egid;
	static char stdbuf[BUFSIZ];

	nl_catd catd = catopen(MF_ID, 0);

	(void) setlocale (LC_ALL, "");

	/*
	 * buffer output for speed...
	 */
	setbuf (stdout, stdbuf);

	/*
	 * get our ids...
	 */
	uid = getuid();
	gid = getgid();
	euid = geteuid();
	egid = getegid();

	/*
	 * print real uid and gid first
	 */
	puid (catgets(catd, MS_ID, SUID, "uid"), uid);
	pgid (catgets(catd, MS_ID, SGID, " gid"), gid);

	/*
	 * print effective uid and effective gid if they don't match
	 * the real uid/gid ...
	 */
	if (uid != euid)
		puid (catgets(catd, MS_ID, SEUID, " euid"), euid);
	if (gid != egid)
		pgid (catgets(catd, MS_ID, SEGID, " egid"), egid);

	putchar ('\n');

	return(0);
}

/*
 * NAME:	puid
 *
 * FUNCTION:	puid - print uid or euid information
 *
 * NOTES:	puid prints a user id string, complete with user name.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
puid (s, id)
	char *s;
	register uid_t id;
{
	register struct passwd *pw;

	printf ("%s=%u", s, id);
	setpwent();

	if ((pw = getpwuid(id)) != NULL)
		printf ("(%s)", pw->pw_name);
}

/*
 * NAME:	pgid
 *
 * FUNCTION:	pgid - print gid or egid information
 *
 * NOTES:	pgid prints a group id string, complete with group name.
 *
 * RETURN VALUE DESCRIPTION:	none
 */

void
pgid (s, id)
	char *s;
	register gid_t id;
{
	register struct group *gr;

	printf ("%s=%u", s, id);

	setgrent();

	if ((gr = getgrgid(id)) != NULL)
		printf ("(%s)", gr->gr_name);
}
