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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: feof.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:40 $";
#endif
/*
 * FUNCTIONS: feof 
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
 * feof.c	1.8  com/lib/c/io,3.1,8943 9/12/89 15:54:17
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#undef feof

/*
 * FUNCTION:	A subroutine version of the macro feof.  This function was
 *	        created to meet ANSI C standards. The feof function tests the
 *		end-of-file indicator for the stream pointed to by stream.
 *
 * RETURN VALUE DESCRIPTION:	
 *		non-zero if and only if the end-of-file indicator is set
 *		for stream.
 *
 */

int 	
feof(FILE *stream)
{
#ifdef	_THREAD_SAFE
	register int rc;
	register filelock_t filelock;

	filelock = _flockfile(stream);
	rc = (stream)->_flag & _IOEOF;
	_funlockfile(filelock);
	return(rc);
#else
	return((stream)->_flag & _IOEOF);
#endif
}
