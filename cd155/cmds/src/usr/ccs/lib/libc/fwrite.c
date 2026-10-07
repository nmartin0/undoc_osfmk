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
static char rcsid[] = "@(#)$RCSfile: fwrite.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:20 $";
#endif
/*
 * FUNCTIONS: fwrite 
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
 * fwrite.c	1.13  com/lib/c/io,3.1,9013 2/22/90 11:46:48
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <string.h>
#include "stdiom.h"

/*
 * FUNCTION:	The fwrite function writes, from the array pointed to by ptr,
 *		up to nmemb mebers whose size is specified by size, to the
 *		stream pointed to by stream.
 *
 * 		This version reads directly from the buffer rather than
 *		looping on putc.  Ptr args aren't checked for NULL because
 *		the program would be a catastrophic mess anyway.  Better
 *		to abort than just to return NULL.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The fwrite function returns the number of members successfully
 *		written, which will be less than nmemb only if a write error
 *	 	is encountered.
 *
 */                                                                   

size_t	
#ifdef	_THREAD_SAFE
unlocked_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
#else
fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
#endif
{
	unsigned nleft;
	int n;
	unsigned char *cptr, *bufend;
	int nwritten;

	if (size <= 0 || nmemb <= 0 || _WRTCHK(stream))
		return (0);

	bufend = _bufend(stream);
	nleft = nmemb*size;

	/* if the file is unbuffered, or if the buffer is empty and we are
	   writing more than a buffer full, do a direct write */
	if (stream->_base >= stream->_ptr)  {	/*this covers the unbuffered case, too*/
		if (stream->_flag & _IONBF || nleft >= bufend - stream->_base)  {
			n = 0;
			cptr = (unsigned char *)ptr;	/* yacks on void pointer arith */
			while (nleft > 0) {
				nwritten = write(fileno(stream),(cptr+n),nleft);
				if (nwritten > 0) {
					nleft -= nwritten;
					n += nwritten;
				} else {		/* write FAILED */
					stream->_flag |= _IOERR;
					break;		/* exit the WHILE */
				}
			}
			return (n/size);
		}
	}
	for (; ; ptr = (void *)((char *)ptr + n)) {
		while ((n = bufend - (cptr = stream->_ptr)) <= 0)  /* full buf */
			if (_xflsbuf(stream) == EOF)
				return (nmemb - (nleft + size - 1)/size);
		if (n > nleft) n = nleft;
		(void) memcpy((void *)cptr, (void *)ptr, (size_t)n);
		stream->_cnt -= n;
		stream->_ptr += n;
		_BUFSYNC(stream);
		if ((nleft -= n) == 0)
			break;
	}
	/* flush if linebuffered with a newline */
	if (stream->_flag & _IOLBF && memchr((void *)cptr, (int)'\n', (size_t)n))
		(void) _xflsbuf(stream);
	return (nmemb);
}

#ifdef	_THREAD_SAFE
size_t	
fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	register filelock_t filelock;
	register size_t rc;

	filelock = _flockfile(stream);
	rc = unlocked_fwrite(ptr, size, nmemb, stream);
	_funlockfile(filelock);
	return rc;
}
#endif
