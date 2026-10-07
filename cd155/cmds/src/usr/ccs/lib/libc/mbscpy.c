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
static char rcsid[] = "@(#)$RCSfile: mbscpy.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:05 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbscpy, wcscpy
 *
 * ORIGINS: 3 27
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
 * Copyright (c) 1984 AT&T	
 * All Rights Reserved  
 *
 * THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF AT&T	
 * The copyright notice above does not evidence any   
 * actual or intended publication of such source code.
 *
 * mbscpy.c	1.5  com/lib/c/nls,3.1,9013 2/27/90 21:40:24
 */
#include <sys/types.h>

/*
 * NAME: mbscpy
 *                                                                    
 * FUNCTION: Copy characters (code points) from one multibyte character 
 *	     string to another multibyte character string.
 *
 * PARAMETERS: 
 *	     char *s1 - overlaid string
 *	     char *s2 - copied string
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer equal to s1.
 */

char  *
#ifdef _NO_PROTO
mbscpy(s1, s2)
char *s1, *s2;
#else
mbscpy(char *s1, const char *s2)
#endif
{
	char *os1, *os2;

	os1 = s1;
	os2 = (char *)s2;
	while(*s1++ = *os2++)
		;
	return(os1);
}
/*
 * NAME: wcscpy
 *
 * FUNCTION: like mbscpy except it copies wide-character characters instead of 
 * multibyte chars.
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the target string.
 */
wchar_t *
#ifdef _NO_PROTO
wcscpy(s1, s2)
wchar_t *s1, *s2;
#else
wcscpy(wchar_t *s1, wchar_t *s2)
#endif
{
	register wchar_t *os1;

	os1 = s1;
	while(*s1++ = *s2++)
		;
	return(os1);
}
