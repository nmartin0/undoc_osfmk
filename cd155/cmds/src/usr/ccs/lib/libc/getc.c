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
static char rcsid[] = "@(#)$RCSfile: getc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:26 $";
#endif
/*
 * FUNCTIONS: getc 
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
 * getc.c	1.8  com/lib/c/io,3.1,8943 9/12/89 18:21:51
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#undef unlocked_getc
#endif
#undef getc

/*
 * FUNCTION:	A subroutine version of the macro getc.  This function was
 *		created to meet ANSI C standards.  The getc function obtains
 *		the next character (if present) as an unsigned char
 *		converted to an int, from the file pointed to by stream, and
 *		advances the associated file position indicator for the stream.
 *		POSIX 1003.4a defines this to be locked by default so the
 *		single thread version becomes unlocked_getc() and a new
 *		thread safe version is defined with the same interface.
 *
 * RETURN VALUE DESCRIPTION:	
 *		Returns the next character from the input stream pointed to
 *		by stream.  If the stream is at end-of-file, the end-of-file
 *		indicator for the stream is set and getc returns EOF.  If a 
 *		read error occurs, the error indicator for the stream is set 
 *		and getc returns EOF.
 *
 */  

#ifdef _THREAD_SAFE
int	
unlocked_getc(FILE *stream)
#else
int	
getc(FILE *stream)
#endif
{
	if (--(stream)->_cnt < 0)
		return(_filbuf(stream));
	else
		return((int) *(stream)->_ptr++);
}

#ifdef _THREAD_SAFE
int	
getc(FILE *stream)
{
	register filelock_t filelock;
	register int        rc;

	filelock = _flockfile(stream);
	if (--(stream)->_cnt < 0)
		rc = _filbuf(stream);
	else
		rc = (int) *(stream)->_ptr++;

	_funlockfile(filelock);
	return(rc);
}
#endif
