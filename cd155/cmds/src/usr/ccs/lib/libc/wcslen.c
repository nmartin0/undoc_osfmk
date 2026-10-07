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
static char rcsid[] = "@(#)$RCSfile: wcslen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:35 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcslen
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
 * wcslen.c	1.3  com/lib/c/nls,3.1,9013 2/27/90 21:43:12
 */

#include <NLchar.h>
#include <sys/types.h>

/*
 * NAME: wcslen
 *
 * FUNCTION: Determine the number of characters in the wide-character string.
 *
 * RETURN VALUE DESCRIPTION: An integer, the number of wchar_t characters 
 *     that precede the terminating wchar_t null character.
 */
size_t 
#ifdef _NO_PROTO
wcslen(s)
wchar_t *s;
#else
wcslen(wchar_t *s)
#endif
{
	register wchar_t *s0 = s + 1;

	while (*s++ != 0)
		;
	return (s - s0);
}
