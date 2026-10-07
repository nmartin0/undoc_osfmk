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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)username.c	1.6  com/lib/PW,3.1,8943 9/11/89 09:38:40";
/*
 * COMPONENT_NAME: (LIBPW) Programmers Workbench Library
 *
 * FUNCTIONS: username
 *
 * ORIGINS: 3 27
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 */

#include <stdio.h>
#include <pwd.h>

/*
 * FUNCTION: Gets user's login name.
 *
 * RETURN VALUE DESCRIPTIONS:
 *	Returns pointer to login name on success,
 *	pointer to string representation of used ID on failure.
 */
/*
	Gets user's login name.
	Note that the argument must be an integer.
	Returns pointer to login name on success,
	pointer to string representation of used ID on failure.

	Remembers user ID and login name for subsequent calls.
 */

#ifndef NLS
#define NLsprintf sprintf
#define NLstrncpy strncpy
#endif


char *
username(uid_t uid)
{
#define	ONAMMAX		9
	static uid_t ouid = -1;
	static char onam[ONAMMAX];
	struct passwd *pwd;

	if (ouid != uid || ouid == -1) {
		if ((pwd = getpwuid(uid)) == NULL)
			NLsprintf(onam, "%d", uid);
		else {
			NLstrncpy(onam, pwd->pw_name, ONAMMAX);
			onam[ONAMMAX - 1] = '\0';
		}
		ouid = uid;
	}
	return(onam);
}

