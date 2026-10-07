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
static char rcsid[] = "@(#)$RCSfile: fputs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:01 $";
#endif
/*
 * FUNCTIONS: fputs 
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
 * fputs.c	1.9  com/lib/c/io,3.1,8943 9/12/89 17:56:55
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include "stdiom.h"

extern char *memccpy();

/*                                                                    
 * FUNCTION: The  fputs subroutine  writes the  null-terminated string
 *           pointed to by the s parameter to the output stream speci-
 *           fied by the stream  parameter.  The fputs subroutine does
 *           not append a new-line character.
 *
 *           This version writes directly to the buffer rather than looping
 *           on putc.  Ptr args aren't checked for NULL because the program
 *           would be a catastrophic mess anyway.  Better to abort than just
 *           to return NULL.
 *
 * PARAMETERS: char *s      - NULL-terminated string to be written to
 *             FILE *stream      - File to be written to
 *
 * RETURN VALUE DESCRIPTIONS:
 *           Upon successful completion, the fputs subroutine returns
 *	     the number of characters written.  fputs returns EOF on an
 *	     error.  This happens if the routines try to write on a file
 *	     that has not been opened for writing.
 */

int 	
fputs(const char *s, FILE *stream)
{
	int ndone = 0, n;
	unsigned char *cs, *bufend;
	char *p;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
#endif
	if (_WRTCHK(stream)) {
#ifdef	_THREAD_SAFE
		_funlockfile(filelock);
#endif
		return (EOF);
	}
	bufend = _bufend(stream);

	for ( ; ; s += n) {
		while ((n = bufend - (cs = stream->_ptr)) <= 0)  /* full buf */
			if (_xflsbuf(stream) == EOF) {
#ifdef	_THREAD_SAFE
				_funlockfile(filelock);
#endif
				return(EOF);
			}
		if ((p = memccpy((char *) cs, s, '\0', n)) != NULL)
			n = (p - (char *) cs) - 1;
		stream->_cnt -= n;
		stream->_ptr += n;
		_BUFSYNC(stream);
		ndone += n;
		if (p != NULL)  { /* done; flush buffer if "unbuffered" or if
				     line-buffered */
			if (stream->_flag & (_IONBF | _IOLBF))
				if (_xflsbuf(stream) == EOF)
					ndone = EOF;
#ifdef	_THREAD_SAFE
			_funlockfile(filelock);
#endif
			return(ndone);
		}
	}
}
