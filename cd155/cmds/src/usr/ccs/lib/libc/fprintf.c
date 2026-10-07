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
static char rcsid[] = "@(#)$RCSfile: fprintf.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:04:59 $";
#endif
/*
 * FUNCTIONS: fprintf 
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
 * fprintf.c	1.13  com/lib/c/prnt,3.1,8943 9/7/89 10:15:30
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdarg.h>
#include <errno.h>

#ifdef	_THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

extern int _doprnt();
/*VARARGS2*/

/*                                                                    
 * FUNCTION: Writes output to to the stream pointed to by stream, under
 *           control of the string pointed to by format, that specifies
 *           how subsequent arguments are converted for output.
 *
 * PARAMETERS: stream - stream to be printed to
 *             format - format used to print arguments
 *	       ...    -   arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      If successful, returns number of characters printed
 *	      Otherwise returns negative value
 */

int	
fprintf(FILE *stream, const char *format, ...) 
{
	int count;
	va_list ap;
	register int err;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
#endif

	va_start(ap,format);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stream);
#endif
	if (!(stream->_flag & _IOWRT)) {
		/* if no write flag */
		if (stream->_flag & _IORW) {
			/* if ok, cause read-write */
			stream->_flag |= _IOWRT;
		} else {
			/* else error */
			SETERR(EBADF);
			return EOF;
		}
	}
	count = _doprnt(format, ap, stream);
	err = ferror(stream)? EOF: count;
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	va_end(ap);
	return err;
}
