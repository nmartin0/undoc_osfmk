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
static char rcsid[] = "@(#)$RCSfile: fseek.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:05 $";
#endif
/*
 * FUNCTIONS: fseek 
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
 * fseek.c	1.11  com/lib/c/io,3.1,9013 2/14/90 10:41:13
 */

/*LINTLIBRARY*/
/*
 * Seek for standard library.  Coordinates with buffering.
 */
#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif

extern long lseek();

int 	
fseek(FILE *stream, long int offset, int whence)
{
	int c;
	long	p;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
#endif
	stream->_flag &= ~_IOEOF;
	if(stream->_flag & _IOREAD) {
		if(whence < 2 && stream->_base && !(stream->_flag&_IONBF)) {
			c = stream->_cnt;
			p = offset;
			if(whence == 0)
				p += (long)c-lseek(fileno(stream), 0L, 1);
			else
				offset -= (long)c;
			if(!(stream->_flag&_IORW) && c > 0 && p <= c &&
					p >= stream->_base - stream->_ptr) {

				/* fseek() must erase any characters put on stream by ungetc() */
				if (stream->_flag & _IOUNGETC) {
					int fd = fileno(stream);

					stream->_flag &= ~_IOUNGETC;
                                        lseek(fd, -c, SEEK_CUR);
                                        read(fd, stream->_ptr, c);
				}
				stream->_ptr += (int)p;
                                stream->_cnt -= (int)p;
#ifdef	_THREAD_SAFE
				_funlockfile(filelock);
#endif
				return(0);
			}
		}
		if(stream->_flag & _IORW) {
			stream->_ptr = stream->_base;
			stream->_flag &= ~_IOREAD;
		}
		p = lseek(fileno(stream), offset, whence);
		stream->_cnt = 0;
	} else if(stream->_flag & (_IOWRT | _IORW)) {
#ifdef	_THREAD_SAFE
		p = unlocked_fflush(stream);
#else
		p = fflush(stream);
#endif
		if(stream->_flag & _IORW) {
			stream->_cnt = 0;
			stream->_flag &= ~_IOWRT;
			stream->_ptr = stream->_base;
		}
		if (p)
			lseek(fileno(stream), offset, whence);
		else
			p = lseek(fileno(stream), offset, whence);
	}
	else p = -1;			/* invalid or closed file block ptr */
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	return((p == -1)? -1: 0);
}

#ifdef	_THREAD_SAFE
/*
 * unlocked fseek for efficiency. for routines that have taken other
 * measures to serialize.
 */
int 	
unlocked_fseek(FILE *stream, long int offset, int whence)
{
	int c;
	long	p;

	stream->_flag &= ~_IOEOF;
	if(stream->_flag & _IOREAD) {
		if(whence < 2 && stream->_base && !(stream->_flag&_IONBF)) {
			c = stream->_cnt;
			p = offset;
			if(whence == 0)
				p += (long)c-lseek(fileno(stream), 0L, 1);
			else
				offset -= (long)c;
			if(!(stream->_flag&_IORW) && c > 0 && p <= c &&
					p >= stream->_base - stream->_ptr) {

				/* fseek() must erase any characters put on stream by ungetc() */
				if (stream->_flag & _IOUNGETC) {
					int fd = fileno(stream);

					stream->_flag &= ~_IOUNGETC;
                                        lseek(fd, -c, SEEK_CUR);
                                        read(fd, stream->_ptr, c);
				}
				stream->_ptr += (int)p;
                                stream->_cnt -= (int)p;
				return(0);
			}
		}
		if(stream->_flag & _IORW) {
			stream->_ptr = stream->_base;
			stream->_flag &= ~_IOREAD;
		}
		p = lseek(fileno(stream), offset, whence);
		stream->_cnt = 0;
	} else if(stream->_flag & (_IOWRT | _IORW)) {
		p = unlocked_fflush(stream);
		if(stream->_flag & _IORW) {
			stream->_cnt = 0;
			stream->_flag &= ~_IOWRT;
			stream->_ptr = stream->_base;
		}
		if (p)
			lseek(fileno(stream), offset, whence);
		else
			p = lseek(fileno(stream), offset, whence);
	}
	else p = -1;			/* invalid or closed file block ptr */
	return((p == -1)? -1: 0);
}
#endif
