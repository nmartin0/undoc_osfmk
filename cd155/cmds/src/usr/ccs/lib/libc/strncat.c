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
static char rcsid[] = "@(#)$RCSfile: strncat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:37 $";
#endif
/*
 * COMPONENT_NAME: (LIBCSTR) Standard C Library String Handling Functions
 *
 * FUNCTIONS: strncat
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
 * strncat.c	1.11  com/lib/c/str,3.1,8943 9/13/89 16:30:36
 */

#include <string.h>

/*
 * FUNCTION: Appends not more than n characters of the string pointed to
 *	     by s2, stopping before a null character is appended, to the
 *	     end of the string pointed to by s1.  The initial character
 *	     of s2 overwrites the null character at the end of s1.  A
 *	     terminating null character is always appended to the result.
 *	     No check is made for overflow of the array pointed to by s1.
 *	     Overlapping copies toward the left work as expected, but
 *	     overlapping copies to the right may give unexpected results.
 *
 * NOTES:    Handles the pathological case where the value of n equals
 *	     the maximum value of an unsigned long integer.
 *
 * PARAMETERS:
 *	     char *s1 - appended string
 *	     char *s2 - copied string
 *	     size_t n - number of characters to append
 *
 * RETURN VALUE DESCRIPTION: Returns a pointer equal to s1.
 */                                                                   
/*LINTLIBRARY*/


char	*
strncat(char *s1, const char *s2, size_t n)
{
	char *os1;

	os1 = s1;
	while(*s1++)
		;
	--s1;
	while(*s1++ = *s2++)
		if(n-- == 0) {
			*--s1 = '\0';
			break;
		}
	return(os1);
}
