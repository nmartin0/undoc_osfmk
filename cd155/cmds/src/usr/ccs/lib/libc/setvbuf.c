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
static char rcsid[] = "@(#)$RCSfile: setvbuf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:10 $";
#endif
/*
 * FUNCTIONS: setvbuf
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
 * setvbuf.c	1.11  com/lib/c/io,3.1,8943 9/9/89 13:43:56
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <malloc.h>

extern void free();
extern int isatty();

/*                                                                    
 * FUNCTION: Assigns buffering to a stream.
 */  

int 	
#ifdef	_THREAD_SAFE
unlocked_setvbuf(FILE *stream, char *buf, int mode, size_t size)
#else
setvbuf(FILE *stream, char *buf, int mode, size_t size)
#endif
{
	if(stream->_base != NULL && stream->_flag & _IOMYBUF)
		free((char*)stream->_base);
	stream->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	stream->_base = (unsigned char *)buf;
	switch (mode)  {
	    /*note that the flags are the same as the possible values for mode*/
	    case _IONBF:
		/* file is unbuffered */
		stream->_flag |= _IONBF;
		stream->_base = (unsigned char *) malloc(_SBFSIZ + 8);
		stream->_flag |= _IOMYBUF;
		_bufend(stream) = stream->_base + _SBFSIZ;
		break;
	    case _IOLBF:
	    case _IOFBF:
		stream->_flag |= mode;
		size = (size == 0) ? BUFSIZ : size;
		if ((size > BUFSIZ) || (stream->_base == NULL)) {
			stream->_base = (unsigned char *)
				     malloc((unsigned)(size + 8));
			stream->_flag |= _IOMYBUF;
		}
		_bufend(stream) = stream->_base + size;
		break;
	    default:
		return -1;
	}
	stream->_ptr = stream->_base;
	stream->_cnt = 0;
	return 0;
}

#ifdef	_THREAD_SAFE
int 	
setvbuf(FILE *stream, char *buf, int mode, size_t size)
{
	register filelock_t filelock;

	filelock = _flockfile(stream);
	if(stream->_base != NULL && stream->_flag & _IOMYBUF)
		free((char*)stream->_base);
	stream->_flag &= ~(_IOMYBUF | _IONBF | _IOLBF);
	stream->_base = (unsigned char *)buf;
	switch (mode)  {
	    /*note that the flags are the same as the possible values for mode*/
	    case _IONBF:
		/* file is unbuffered */
		stream->_flag |= _IONBF;
		stream->_base = (unsigned char *) malloc(_SBFSIZ + 8);
		stream->_flag |= _IOMYBUF;
		_bufend(stream) = stream->_base + _SBFSIZ;
		break;
	    case _IOLBF:
	    case _IOFBF:
		stream->_flag |= mode;
		size = (size == 0) ? BUFSIZ : size;
		if ((size > BUFSIZ) || (stream->_base == NULL)) {
			stream->_base = (unsigned char *)
				     malloc((unsigned)(size + 8));
			stream->_flag |= _IOMYBUF;
		}
		_bufend(stream) = stream->_base + size;
		break;
	    default:
		_funlockfile(filelock);
		return -1;
	}
	stream->_ptr = stream->_base;
	stream->_cnt = 0;
	_funlockfile(filelock);
	return 0;
}
#endif
