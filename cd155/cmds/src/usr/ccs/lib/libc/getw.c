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
static char rcsid[] = "@(#)$RCSfile: getw.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:59 $";
#endif
/*
 * FUNCTIONS: getw  
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
 * getw.c	1.6  com/lib/c/io,3.1,8943 9/12/89 18:33:18
 */

/*LINTLIBRARY*/
/*
 * The intent here is to provide a means to make the order of
 * bytes in an io-stream correspond to the order of the bytes
 * in the memory while doing the io a `word' at a time.
 */
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif

int
getw(stream)
register FILE *stream;
{
	int w;
	register char *s = (char *)&w;
	register int i = sizeof(int);
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
	while (--i >= 0) {
		*s++ = unlocked_getc(stream);
		if(feof(stream) || ferror(stream)) {
			_funlockfile(filelock);
			return(EOF);
		}
	}
	_funlockfile(filelock);
#else
	while (--i >= 0) {
		*s++ = getc(stream);
		if(feof(stream) || ferror(stream))
			return(EOF);
	}
#endif
	return(w);
}
