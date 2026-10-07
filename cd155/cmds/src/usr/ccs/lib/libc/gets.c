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
static char rcsid[] = "@(#)$RCSfile: gets.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:54 $";
#endif
/*
 * FUNCTIONS: gets 
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
 * gets.c	1.8  com/lib/c/io,3.1,8943 9/12/89 18:32:40
 */

/*LINTLIBRARY*/
/*
 * This version reads directly from the buffer rather than looping on getc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include "stdiom.h"

extern int _filbuf();
extern _bufsync();
extern char *memccpy();

char *
gets(char *s)
{
	char *p, *s0 = s;
	int n;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stdin);
#endif

	for ( ; ; ) {
		if (stdin->_cnt <= 0) { /* empty buffer */
			if (_filbuf(stdin) == EOF) {
				if (s0 == s)
					return (NULL);
				break; /* no more data */
			}
			stdin->_ptr--;
			stdin->_cnt++;
		}
		n = stdin->_cnt;
		if ((p = memccpy(s, (char *) stdin->_ptr, '\n', n)) != NULL)
			n = p - s;
		s += n;
		stdin->_cnt -= n;
		stdin->_ptr += n;
		_BUFSYNC(stdin);
		if (p != NULL) { /* found '\n' in buffer */
			s--; /* step back over '\n' */
			break;
		}
	}
	*s = '\0';
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	return (s0);
}
