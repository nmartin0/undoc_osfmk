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
static char rcsid[] = "@(#)$RCSfile: wcschr.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:34 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcschr
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
 * wcschr.c	1.4  com/lib/c/nls,3.1,9013 2/27/90 21:42:22
 */

#include <NLchar.h>
#include <sys/types.h>

/*
 *  Return the ptr in sp at which the character c appears;
 *  NULL if not found.  Two versions here:  NLstrchr (recognizes
 *  NLchar as second argument) and NCstrchr (searches NLchar string
 *  for NLchar second argument).
 */

/*
 * NAME: wcschr
 *
 * FUNCTION: look for the first occurrence of a wchar_t
 *
 * RETURN VALUE DESCRIPTION: returns either a NULL (failure) or a 
 * pointer to the occurrence of the char.
 *
 */
wchar_t *
#ifdef _NO_PROTO
wcschr(sp, c)
wchar_t *sp;
wchar_t c;
#else
wcschr(wchar_t *sp, wchar_t c)
#endif
{
	do {
		if(*sp == c)
			return(sp);
	} while(*sp++);
	return((wchar_t *)NULL);
}
