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
static char rcsid[] = "@(#)$RCSfile: setbuf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:57 $";
#endif
/*
 * FUNCTIONS: setbuf 
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
 * setbuf.c	1.9  com/lib/c/io,3.1,8943 10/19/89 17:18:48
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdlib.h>

extern int isatty();
extern unsigned char _smbuf[][_SBFSIZ];
extern unsigned char *_stdbuf[];

void 	
setbuf(FILE *stream, char *buf)
{
	int fno;  /* file number */
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
#endif
	fno = fileno(stream);

	if(stream->_base != NULL && stream->_flag & _IOMYBUF)
		free((void *)stream->_base);
	stream->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	if((stream->_base = (unsigned char*)buf) == NULL) {
		stream->_flag |= _IONBF; /* file unbuffered except in fastio */
		if (fno < 2)  /* for stdin, stdout, use the existing bufs */
			_bufend(stream) = (stream->_base = _stdbuf[fno]) + BUFSIZ;

		else {   /* otherwise, use small buffer */
			stream->_base = (unsigned char *) malloc((size_t)_SBFSIZ+8);
			stream->_flag |= _IOMYBUF;
			_bufend(stream) = stream->_base + _SBFSIZ;
		}
	}
	else {  /* regular buffered I/O, standard buffer size */
		_bufend(stream) = stream->_base + BUFSIZ;
		if (isatty(fno))
			stream->_flag |= _IOLBF;
	}
	stream->_ptr = stream->_base;
	stream->_cnt = 0;
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
}
