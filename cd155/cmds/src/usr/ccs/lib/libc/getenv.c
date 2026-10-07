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
static char rcsid[] = "@(#)$RCSfile: getenv.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:30 $";
#endif
/*
 * FUNCTIONS: getenv
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
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * getenv.c	1.10  com/lib/c/gen,3.1,8943 9/8/89 08:48:33
 */

#include <stdio.h>			/* for NULL */
#ifdef _THREAD_SAFE
#include "rec_mutex.h"

extern struct rec_mutex	_environ_rmutex;
#endif

/* this is not declared extern because it needs to be
   exported from libc for shared libraries */
char **environ;			/* environment list		*/
static char *nvmatch();			/* find an environment match	*/

/*
 * NAME:	getenv
 *                                                                    
 * FUNCTION:	return the value of an environment variable
 *                                                                    
 * NOTES:	Getenv searches the environment list for 'name', which
 *		is a string of the form "name=value".
 *
 * RETURN VALUE DESCRIPTION:	NULL if the enviroment variable is not
 *		found, else a pointer to the value.
 */  
/* The getenv() function includes all the POSIX requirements */

char *
getenv(const char *name)
{
	char **p, *v;

#ifdef _THREAD_SAFE
	rec_mutex_lock(&_environ_mutex);
#endif

	if ((p = environ) != NULL)
		while (*p != NULL)
			if ((v = nvmatch(name, *p++)) != NULL)
				return(v);

#ifdef _THREAD_SAFE
	rec_mutex_unlock(&_environ_mutex);
#endif

	return(NULL);
}

/*
 * NAME:	nvmatch
 *                                                                    
 * FUNCTION:	see if 's1' matches 's2'.  's1' is either "name" or
 *		"name=value" and 's2' is "name=value".
 *                                                                    
 * RETURN VALUE DESCRIPTION:	s2 if they match, else NULL
 */  

static char *
nvmatch(s1, s2)
char *s1, *s2;
{
	while (*s1 == *s2++)
		if (*s1++ == '=')
			return(s2);

	if (*s1 == '\0' && *(s2-1) == '=')
		return(s2);

	return(NULL);
}
