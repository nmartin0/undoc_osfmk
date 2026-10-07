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
static char rcsid[] = "@(#)$RCSfile: fputc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:00 $";
#endif
/*
 * FUNCTIONS: fputc 
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
 * fputc.c	1.8  com/lib/c/io,3.1,8943 9/12/89 17:56:31
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif

int 	
fputc(int c, FILE *stream)
{
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
	register int rc;

	filelock = _flockfile(stream);
	rc = unlocked_putc(c, stream);
	_funlockfile(filelock);
	return rc;
#else
	return(putc(c, stream));
#endif
}
