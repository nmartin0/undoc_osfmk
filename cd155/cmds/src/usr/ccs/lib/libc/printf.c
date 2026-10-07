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
static char rcsid[] = "@(#)$RCSfile: printf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:24 $";
#endif
/*
 * FUNCTIONS: printf 
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
 * printf.c	1.10  com/lib/c/prnt,3.1,8943 9/7/89 10:16:10
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdarg.h>

extern int _doprnt();
/*VARARGS1*/

/*                                                                    
 * FUNCTION: writes output to stdout under control of the string pointed
 *           to by format that specifies how subsequent arguments are
 *           converted for output.
 *
 * PARAMETERS: format - format used to print arguments
 *	       ...    -   arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      If successful, returns number of characters printed
 *	      Otherwise returns negative value
 */

int	
printf(const char *format, ...) 
{
	int count;
	va_list ap;
#ifdef	_THREAD_SAFE
	register int rc;
	register filelock_t filelock;
#endif

	va_start(ap,format);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stdout);
	count = _doprnt(format, ap, stdout);
	rc = ferror(stdout)? EOF: count;
	_funlockfile(filelock);
#else
	count = _doprnt(format, ap, stdout);
#endif
	va_end(ap);

#ifdef	_THREAD_SAFE
	return(rc);
#else
	return(ferror(stdout)? EOF: count);
#endif
}
