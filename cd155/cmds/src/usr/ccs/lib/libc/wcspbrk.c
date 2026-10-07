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
static char rcsid[] = "@(#)$RCSfile: wcspbrk.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:37 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcspbrk
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
 * wcspbrk.c	1.4  com/lib/c/nls,3.1,9013 2/27/90 21:44:33
 */
#include <sys/types.h>

/*
 *  NAME: wcspbrk
 *
 *  FUNCTION: Locate the first occurence of characters in a string.
 *
 * PARAMETERS:
 *	wchar_t *string1	-	the wide character string
 *	wchar_t *string2	-	the wide character string
 *
 *  RETURN VALUE DESCRIPTION:
 *  Return ptr to first occurrence of any wchar_t from `string2'
 *  in the wchar_t string `string1'; NULL if none exists.
 */

wchar_t *
#ifdef _NO_PROTO
wcspbrk(string1, string2)
wchar_t *string1,*string2;
#else
wcspbrk(wchar_t *string1,wchar_t *string2)
#endif
{
	register wchar_t *p;

	do {
		for(p = string2; *p != 0 && *p != *string1; ++p)
			;
		if(*p != 0) {
			return(string1);
		}
	}
	while(*string1++);
	return(NULL);
}
