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
static char rcsid[] = "@(#)$RCSfile: cuserid.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:04:13 $";
#endif
/*
 * FUNCTIONS: cuserid 
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
 * cuserid.c	1.11  com/lib/c/io,3.1,8943 10/26/89 12:48:23
 */

#include <unistd.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>

#ifndef _THREAD_SAFE
static char res[L_cuserid];
#endif

char *
cuserid(char *s)
{
	register struct passwd *pw;
	register char *p;
#ifdef _THREAD_SAFE
	char line[BUFSIZ];
	struct passwd password;
        FILE    *pw_fp = 0;
#endif	/* _THREAD_SAFE */

#ifdef _THREAD_SAFE
	if (s == NULL)
		return(NULL);
	password.pw_name = line;
	if (getpwuid_r(geteuid(), &password, line, BUFSIZ) < 0)
		pw = NULL;
	else
		pw = &password;
	endpwent_r(pw_fp);
#else
	pw = getpwuid(geteuid());
	endpwent();
#endif /* _THREAD_SAFE */

	/* If successful, then copy the name into the string and return it. */
	if (pw != NULL) {
#ifndef _THREAD_SAFE
		if (s == NULL)
			s = res;
#endif
		return (strcpy(s, pw->pw_name));
	}

#ifndef _THREAD_SAFE
	/* If failure and a NULL was passed in, return the NULL. */
	if (s == NULL)
		return(NULL);
#endif

	/*
	 * If failure and a string was passed in, zero the first element of
	 * the string and return the orginal string.
	 */
	*s = '\0';
	return (s);
}
