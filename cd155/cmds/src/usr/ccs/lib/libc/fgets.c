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
static char rcsid[] = "@(#)$RCSfile: fgets.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:43 $";
#endif
/*
 * FUNCTIONS: fgets 
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
 * fgets.c	1.11  com/lib/c/io,3.1,8943 10/18/89 10:18:40
 */

/*LINTLIBRARY*/
/*
 * This version reads directly from the buffer rather than looping on getc.
 * Ptr args aren't checked for NULL because the program would be a
 * catastrophic mess anyway.  Better to abort than just to return NULL.
 */
#include <errno.h>
#include <stdio.h>
#ifdef _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <memory.h>
#include "stdiom.h"

#define MIN(x, y)	(x < y ? x : y)

extern int _filbuf();

char 	*
fgets(char *s, int n, FILE *stream)
{
	char *p, *save = s;
	int i;
#ifdef _THREAD_SAFE
	filelock_t	filelock;
#endif

	if ((stream->_flag&(_IOREAD|_IORW))==0) {  /* not open for reading */
#ifdef _THREAD_SAFE
		seterrno(EBADF);
#else
		errno=EBADF;
#endif
		return(NULL);
	}
#ifdef _THREAD_SAFE
	filelock = _flockfile(stream);
#endif
	for (n--; n > 0; n -= i) {
		if (stream->_cnt <= 0) { /* empty buffer */
			if (_filbuf(stream) == EOF) {
				if (save == s) {
#ifdef _THREAD_SAFE
					_funlockfile(filelock);
#endif
					return (NULL);
				}
				break; /* no more data */
			}
			stream->_ptr--;
			stream->_cnt++;
		}
		i = MIN(n, stream->_cnt);
		if ((p = memccpy((void *)s,(void *)stream->_ptr,(int)'\n',(size_t)i)) !=NULL)
			i = p - s;
		s += i;
		stream->_cnt -= i;
		stream->_ptr += i;
		_BUFSYNC(stream);
		if (p != NULL)
			break; /* found '\n' in buffer */
	}
	*s = '\0';
#ifdef _THREAD_SAFE
	_funlockfile(filelock);
#endif
	return (save);
}
