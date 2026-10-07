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
static char rcsid[] = "@(#)$RCSfile: putc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:25 $";
#endif
/*
 * FUNCTIONS: putc 
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
 * putc.c	1.8  com/lib/c/io,3.1,8943 9/9/89 13:29:58
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#undef unlocked_putc
#endif
#undef putc

/*
 * FUNCTION:	A subroutine version of the macro putc.  This function is
 *		created to meet ANSI C standards.  The putc function writes
 *		the character specified by c (converted to an unsigned char)
 *		to the output stream pointed to by stream, at the position
 *		indicated by the assoicated file poistion indicator for the
 *		stream, and advances the indicator appropriately.
 *		POSIX 1003.4a requires that this function is locked by default
 *		so an unlocked version is also provided.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The putc function returns the character written.  If a write
 *		error occurs, the error indicator for the stream is set and
 * 		putc returns EOF.
 *
 */  

#ifdef _THREAD_SAFE
int 	
unlocked_putc(int c, FILE *stream)
#else
int 	
putc(int c, FILE *stream)
#endif
{
	if (--(stream)->_cnt < 0)
		return (_flsbuf((unsigned char) (c), (stream)));
	else
		return ((int) (*(stream)->_ptr++ = (unsigned char) (c)));
}

#ifdef	_THREAD_SAFE
int 	
putc(int c, FILE *stream)
{
	register filelock_t filelock;
	register int        rc;

	filelock = _flockfile(stream);
	if (--(stream)->_cnt < 0)
		rc = _flsbuf((unsigned char) (c), (stream));
	else
		rc = ((int) (*(stream)->_ptr++ = (unsigned char) (c)));

	_funlockfile(filelock);
	return rc;
}
#endif
