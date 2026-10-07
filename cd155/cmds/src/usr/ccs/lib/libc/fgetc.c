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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: fgetc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:41 $";
#endif
/*
 * FUNCTIONS: fgetc 
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
 * fgetc.c	1.8  com/lib/c/io,3.1,8943 9/12/89 15:12:59
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif

int 	
fgetc(FILE *stream)
{
#ifdef	_THREAD_SAFE
	register int c;
	register filelock_t filelock;

	filelock = _flockfile(stream);
	c = unlocked_getc(stream);
	_funlockfile(filelock);
	return(c);
#else
	return(getc(stream));
#endif
}
