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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ferror.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:40 $";
#endif
/*
 * FUNCTIONS: ferror 
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
 * ferror.c	1.8  com/lib/c/io,3.1,8943 9/13/89 09:50:37
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#undef ferror

/*                                                                    
 * FUNCTION:	A subroutine version of the macro ferror.  This function was
 *	        created to meet ANSI C standards.  The ferror function tests
 *		for the error indicator for the stream pointed to by stream.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The ferror function returns non-zero if and only if the error
 *		indicator is set for stream.
 *
 */  

int 	
ferror(FILE *stream)
{
#ifdef	_THREAD_SAFE
	register int err;
	register filelock_t filelock;


	filelock = _flockfile(stream);
	err = (stream)->_flag & _IOERR;
	_funlockfile(filelock);

	return err;
#else
	return((stream)->_flag & _IOERR);
#endif
}
