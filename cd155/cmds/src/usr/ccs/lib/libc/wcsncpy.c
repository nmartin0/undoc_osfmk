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
static char rcsid[] = "@(#)$RCSfile: wcsncpy.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:37 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcsncpy
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
 * wcsncpy.c	1.3  com/lib/c/nls,3.1,9013 2/27/90 21:44:08
 */

#include <NLchar.h>
#include <sys/types.h>

/*
 *
 *  Copy s2 to s1, truncating or null-padding to always copy n elements
 *  (bytes or wchar_t).  Return s1.
 *
 */

/*
 * NAME: wcsncpy
 *
 * FUNCTION: copy a specific number of characters from one wide-characters 
 *    string to another wide-character string.
 *
 * RETURN VALUE DESCRIPTION: returns a pointer to the target string.
 */
wchar_t *
#ifdef _NO_PROTO
wcsncpy(s1, s2, n)
wchar_t *s1, *s2; 
size_t n;
#else
wcsncpy(wchar_t *s1, wchar_t *s2, size_t n)
#endif
{
	register wchar_t *os1 = s1;
	register len=n;

	while (--len >= 0)
		if ((*s1++ = *s2++) == 0)
			while (--len >= 0)
				*s1++ = 0;
	return (os1);
}
