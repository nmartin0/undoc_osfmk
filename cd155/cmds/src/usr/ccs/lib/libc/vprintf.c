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
static char rcsid[] = "@(#)$RCSfile: vprintf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:27 $";
#endif
/*
 * FUNCTIONS: vprintf 
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
 * vprintf.c	1.11  com/lib/c/prnt,3.1,8943 9/7/89 10:17:10
 */

#include <stdarg.h>
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif

/*
 * FUNCTION: Writes output to stdout under control of the string pointed
 *	     to by format that specifies how subsequent argumnts are
 *	     converted for output.
 *
 * PARAMETERS: format - format used to print arguments
 *	       arg   - arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 *	      If successful, returns number of characters printed
 *	      Otherwise returns negative value
 */

int	
vprintf(const char *format, va_list arg) 
{
	int rc;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
	filelock = _flockfile(stdout);
#endif

	if (!(stdout->_flag & _IOWRT)) {
		/* if no write flag */
		if (stdout->_flag & _IORW) {
			/* if ok, cause read-write */
			stdout->_flag |= _IOWRT;
		} else {
			/* else error */
#ifdef	_THREAD_SAFE
			_funlockfile(filelock);
#endif
			return EOF;
		}
	}
	rc = _doprnt (format, arg, stdout);
#ifdef	_THREAD_SAFE
	rc = ferror(stdout)? EOF: rc;
	_funlockfile(filelock);
	return(rc);
#else
	return (ferror(stdout)? EOF: rc);
#endif
}
