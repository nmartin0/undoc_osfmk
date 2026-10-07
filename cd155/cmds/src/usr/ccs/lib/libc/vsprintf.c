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
static char rcsid[] = "@(#)$RCSfile: vsprintf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:28 $";
#endif
/*
 * FUNCTIONS: vsprintf 
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
 * vsprintf.c	1.10  com/lib/c/prnt,3.1,8943 9/7/89 10:17:32
 */

#include <stdarg.h>
#include <stdio.h>
#include <limits.h>
#include <values.h>

extern int _doprnt();

/*                                                                    
 * FUNCTION: Writes output to to the array pointed to by s, under
 *           control of the string pointed to by format, that specifies
 *           how subsequent argumnts are converted for output.
 *
 * PARAMETERS: s      - array to be printed to
 *             format - format used to print arguments
 *	       arg   - arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      If successful, returns number of characters printed
 *	      Otherwise returns negative value
 */                                                                   

int	
vsprintf(char *s, const char *format, va_list arg)
{
	FILE siop;
	int rc;

#ifdef _THREAD_SAFE
	siop._lock = NULL;
#endif
	siop._cnt = INT_MAX;
	siop._base = siop._ptr = (unsigned char *)s;
	siop._flag = (_IOWRT|_IONOFD);
	rc = _doprnt(format, arg, &siop);
	*siop._ptr = '\0';
	return (rc);
}
