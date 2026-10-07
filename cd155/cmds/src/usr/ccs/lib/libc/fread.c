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
static char rcsid[] = "@(#)$RCSfile: fread.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:05:04 $";
#endif
/*
 * FUNCTIONS: fread 
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
 * fread.c	1.12  com/lib/c/io,3.1,8943 10/18/89 11:00:04
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <string.h>
#include <errno.h>
#include "stdiom.h"

#ifdef	_THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

#define MIN(x, y)	(x < y ? x : y)

extern int _filbuf();
extern _bufsync();

/*
 * FUNCTION:	The fread function reads, into the array pointed to by
 *		ptr, up to nmemb members whose size is specified by size, 
 *		from the stream pointed to be stream.
 *
 * 		This version reads directly from the buffer rather than
 *		looping on getc.  Ptr args aren't checked for NULL because
 *		the program would be a catastrophic mess anyway.  Better
 *		to abort than just to return NULL.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The fread function returns the number of members successfully
 *		read, which may be less than nmemb if a read error or end-of-
 *		file is encountered.  If size or nmemb is zero, fread returns
 *		zero and the contents of the array and the state of the
 *		stream remains unchanged.
 *
 */  

size_t 	
fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	unsigned nleft;
	int n;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
#endif

	if (size <= 0 || nmemb <= 0)
		return (0);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stream);
#endif

        /* Check if stream was opened for reading */
        if (!(stream->_flag & _IOREAD)) {
                if (stream->_flag & _IORW) {
                        stream->_flag |= _IOREAD;
                }
                else {
                        SETERR(EBADF);
#ifdef  _THREAD_SAFE
                        _funlockfile(filelock);
#endif
                        return (0);
                }
        }

	for (nleft = nmemb * size; ; ) {
		if (stream->_cnt <= 0) { /* empty buffer */
			if (_filbuf(stream) == EOF) {
#ifdef	_THREAD_SAFE
				_funlockfile(filelock);
#endif
				return (nmemb - (nleft + size - 1)/size);
			}
			stream->_ptr--;
			stream->_cnt++;
		}
		n = MIN(nleft, stream->_cnt);
		ptr = (char *)memcpy(ptr, (void *) stream->_ptr, (size_t)n) + n;
		stream->_cnt -= n;
		stream->_ptr += n;
		_BUFSYNC(stream);
		if ((nleft -= n) == 0) {
#ifdef	_THREAD_SAFE
			_funlockfile(filelock);
#endif
			return (nmemb);
		}
	}
}

#ifdef	_THREAD_SAFE
/*
 * unlocked fread for efficiency. for routines that have taken other
 * measures to serialize.
 */
size_t 	
unlocked_fread(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	unsigned nleft;
	int n;

	if (size <= 0 || nmemb <= 0)
		return (0);

        /* Check if stream was opened for reading */
        if (!(stream->_flag & _IOREAD)) {
                if (stream->_flag & _IORW) {
                        stream->_flag |= _IOREAD;
                }
                else {
                        SETERR(EBADF);
                        return (0);
                }
        }

	for (nleft = nmemb * size; ; ) {
		if (stream->_cnt <= 0) { /* empty buffer */
			if (_filbuf(stream) == EOF) {
				return (nmemb - (nleft + size - 1)/size);
			}
			stream->_ptr--;
			stream->_cnt++;
		}
		n = MIN(nleft, stream->_cnt);
		ptr = (char *)memcpy(ptr, (void *) stream->_ptr, (size_t)n) + n;
		stream->_cnt -= n;
		stream->_ptr += n;
		_BUFSYNC(stream);
		if ((nleft -= n) == 0) {
			return (nmemb);
		}
	}
}
#endif
