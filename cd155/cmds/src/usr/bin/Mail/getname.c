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
static char rcsid[] = "@(#)$RCSfile: getname.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:14 $";
#endif
/* 
 * COMPONENT_NAME: CMDMAILX getname.c
 * 
 * FUNCTIONS: getname, getuserid 
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	getname.c    5.2 (Berkeley) 6/21/85
 */

#include <pwd.h>

/*
 * Getname / getuserid for those with
 * hashed passwd data base).
 *
 */

#include "rcv.h"

/*
 * Search the passwd file for a uid.  Return name through ref parameter
 * if found, indicating success with 0 return.  Return -1 on error.
 * If -1 is passed as the user id, close the passwd file.
 */

getname(uid, namebuf)
	char namebuf[];
{
	struct passwd *pw;

	if (uid == -1) {
		return(0);
	}
	if ((pw = getpwuid(uid)) == NULL)
		return(-1);
	strcpy(namebuf, pw->pw_name);
	return 0;
}

/*
 * Convert the passed name to a user id and return it.  Return -1
 * on error.  Iff the name passed is -1 (yech) close the pwfile.
 */

getuserid(name)
	char name[];
{
	struct passwd *pw;

	if (name == (char *) -1) {
		return(0);
	}
	if ((pw = getpwnam(name)) == NULL)
		return 0;
	return pw->pw_uid;
}
