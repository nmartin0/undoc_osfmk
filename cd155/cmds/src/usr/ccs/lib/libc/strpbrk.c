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
static char rcsid[] = "@(#)$RCSfile: strpbrk.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:37 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: strpbrk
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
 * strpbrk.c	1.11  com/lib/c/str,3.1,8943 10/20/89 12:57:52
 */

#include <string.h>

/*
 * FUNCTION: Returns a pointer to the first occurrence in the string
 *	     pointed to by s1 of any character from the string pointed
 *	     to by s2.  A NULL pointer is returned if no character
 *	     matches are found.
 *
 * PARAMETERS:
 *	     char *s1 - string to be searched
 *	     char *s2 - string containing characters to be found
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer to the first occurrence of
 *	     any character from s2 in s1; NULL if no matches are found.
 */
/*LINTLIBRARY*/


char *
strpbrk(const char *s1, const char *s2)
{
	char *p;

	do {
		for(p=(char *)s2; *p != '\0' && *p != *s1; ++p)
			;
		if(*p != '\0')
			return((char *)s1);
	}
	while(*s1++);
	return(NULL);
}
