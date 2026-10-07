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
static char rcsid[] = "@(#)$RCSfile: puts.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:33 $";
#endif
/*
 * FUNCTIONS: puts 
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
 * puts.c	1.10  com/lib/c/io,3.1,8943 9/9/89 13:30:28
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include "stdiom.h"

extern char *memccpy();

/*                                                                   
 * FUNCTION:	The puts function writes the string pointed to by s to the
 *		stream pointed to by stdout, and appends a new-line character
 *		to the output.  The terminating character is not written.
 *
 * 		This version reads directly from the buffer rather than
 *		looping on putc.  Ptr args aren't checked for NULL because
 *		the program would be a catastrophic mess anyway.  Better
 *		to abort than just to return NULL.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The puts function returns EOF if a write error occurs;
 *		otherwise it returns a nonnegative value
 *
 */  

int 	
puts(const char *s)
{
	char *p;
	int ndone = 0, n;
	unsigned char *cs, *bufend;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stdout);
#endif

	if (_WRTCHK(stdout)) {
#ifdef	_THREAD_SAFE
		_funlockfile(filelock);
#endif
		return (0);
	}

	bufend = _bufend(stdout);

	for ( ; ; s += n) {
		while ((n = bufend - (cs = stdout->_ptr)) <= 0) /* full buf */
			if (_xflsbuf(stdout) == EOF) {
#ifdef	_THREAD_SAFE
				_funlockfile(filelock);
#endif
				return(EOF);
			}
		if ((p = memccpy((char *) cs, s, '\0', n)) != NULL)
			n = p - (char *) cs;
		stdout->_cnt -= n;
		stdout->_ptr += n;
		_BUFSYNC(stdout);
		ndone += n;
		if (p != NULL) {
			stdout->_ptr[-1] = '\n'; /* overwrite '\0' with '\n' */
			if (stdout->_flag & (_IONBF | _IOLBF)) /* flush line */
				if (_xflsbuf(stdout) == EOF) {
#ifdef	_THREAD_SAFE
					_funlockfile(filelock);
#endif
					return(EOF);
				}
#ifdef	_THREAD_SAFE
			_funlockfile(filelock);
#endif
			return(ndone);
		}
	}
}
