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
static char rcsid[] = "@(#)$RCSfile: wcscspn.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:35 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcscspn
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
 * wcscspn.c	1.4  com/lib/c/nls,3.1,9013 2/27/90 21:42:47
 */
#include <sys/types.h>

/*
 * NAME: wcscspn
 *
 * FUNCTION: Find the length of the initial portion of wide-character 
 *  string1 of characters not in wide-character string2.
 *
 * PARAMETERS:
 *	wchar_t *string1	-	the wide character string
 *	wchar_t *string2	-	the wide character string
 *
 * RETURN VALUE DESCRIPTION: the number of wchar_t in the segment.
 */
size_t 
#ifdef _NO_PROTO
wcscspn(string1, string2)
wchar_t *string1, *string2;
#else
wcscspn(wchar_t *string1,wchar_t *string2)
#endif
{
	register wchar_t *q;
	register wchar_t *p;

	for(q=string1; *q != '\0'; q++) {
		for (p = string2; *p != 0 && *p != *q; ++p)
			;
		if(*p != 0)
			break;
	}
	return(q - string1);
}
