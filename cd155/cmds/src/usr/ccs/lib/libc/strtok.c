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
static char rcsid[] = "@(#)$RCSfile: strtok.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:43 $";
#endif
/*
 * FUNCTIONS: strtok
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
 * strtok.c	1.9  com/lib/c/str,3.1,8943 9/13/89 16:32:08
 */

#include <string.h>

/*
 * FUNCTION: Returns a pointer to an occurrence of a text token in the
 *	     string pointed to by s1.  The string pointed to by s2
 *	     defines a set of token delimiters.  If s1 is anything other
 *	     than NULL, the string pointed to by s1 is read until one of
 *	     the delimiter characters is found.  A null character is
 *	     stored into the string, replacing the found delimiter, and
 *	     a pointer to the first character of the text token is
 *	     returned.  Subsequent calls with a NULL value in s1 step
 *	     through the string.  The delimiters pointed to by s2 can
 *	     be changed on subsequent calls.  A NULL pointer is returned
 *	     when no tokens remain in the string pointed to by s1.
 *                                                                    
 * PARAMETERS:
 *	     char *s1 - scanned for text tokens
 *	     char *s2 - set of token delimiters
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to a text token, or NULL,
 *	     as described above.
 */
/*LINTLIBRARY*/

/*
*	Thread-safe strtok
*	Changed to be re-entrant by not using
*	static data and passing in the data instead.
*/
#ifdef _THREAD_SAFE
#define SAVEPT	*savept
#else
#define SAVEPT	savept
#endif

#ifdef _THREAD_SAFE
char *
strtok_r(char *s1, char *s2, char **savept )
#else /* _THREAD_SAFE */
char *
strtok(char *s1, const char *s2)
#endif /* _THREAD_SAFE */
{
	char	*p, *q, *r;
#ifndef _THREAD_SAFE
	static char	*savept;
#endif _THREAD_SAFE

	/*first or subsequent call*/
	p = (s1 == NULL)? SAVEPT: s1;

	if(p == 0)		/* return if no tokens remaining */
		return(NULL);

	q = p + strspn(p, s2);	/* skip leading separators */

	if(*q == '\0')		/* return if no tokens remaining */
		return(NULL);

	if((r = strpbrk(q, s2)) == NULL)	/* move past token */
		SAVEPT = 0;
	else {
		*r = '\0';
		SAVEPT = ++r;
	}
	return(q);
}
