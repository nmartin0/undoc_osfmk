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
static char rcsid[] = "@(#)$RCSfile: fopen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:54 $";
#endif
/*
 * FUNCTIONS: fopen, freopen 
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
 * fopen.c	1.15  com/lib/c/io,3.1,9013 2/14/90 19:09:08
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <fcntl.h>
#include <errno.h>

extern FILE *_findiop();
static FILE *_endopen();

/*
 * FUNCTION: The fopen subroutine opens the file named by the path
 *           parameter and associates a stream with it. 
 *
 * PARAMETERS: 
 *	     char *filename  - points to a character string that contains
 * 	                   the name of the file to be opened.
 *	     char *mode  - points to a character string that has one
 *	                   of the following values:
 *   			   "r", "w", "a", "r+", "w+", "a+", "rb", "wb",
 *			   "ab", "r+b", "w+b", "a+b"
 *
 *                         Note: In this implementation, there is not any 
 *                               difference between text and binary files.
 *                               Therefore, the b type will be ignored.
 *
 * RETURN VALUE DESCRIPTIONS:
 *           - returns a pointer to the FILE structure of this stream.
 *
 */

FILE	*
fopen(const char *filename, const char *mode)
{
	return (_endopen(filename, mode, _findiop()));
}

/*
 *
 * FUNCTION: The  freopen subroutine  substitutes  the  named file  in
 *           place of the open stream.   The original stream is closed
 *           whether  or not  the  open succeeds.   
 *
 * PARAMETERS: 
 *	     char *filename  - points to a character string that contains
 * 	                   the name of the file to be opened.
 *	     char *mode  - points to a character string that has one
 *	                   of the following values:
 *   			   "r", "w", "a", "r+", "w+", "a+", "rb", "wb",
 *			   "ab", "r+b", "w+b", "a+b"
 *
 *                         Note: In this implementation, there is not any 
 *                               difference between text and binary files.
 *                               Therefore, the b type will be ignored.
 *	     FILE *stream   - points to an open stream
 *
 * RETURN VALUE DESCRIPTIONS:
 *           - returns a pointer to the FILE structure associated with stream.
 *
 */

FILE 	*
freopen(const char *filename, const char *mode, FILE *stream)
{
	(void) fclose(stream); /* doesn't matter if this fails */
	return (_endopen(filename, mode, stream));
}

static FILE *
_endopen(filename, mode, stream)
char	*filename, *mode;
FILE *stream;
{
	int	plus, oflag, fd;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
#endif

	if (stream == NULL || filename == NULL || filename[0] == '\0') {
#ifdef	_THREAD_SAFE
		seterrno(ENOENT);
#else
		errno = ENOENT;
#endif
		return (NULL);
	}
	plus = ((mode[1] == '+') || (mode[2] == '+'));
	switch (mode[0]) {
	case 'w':
		oflag = (plus ? O_RDWR : O_WRONLY) | O_TRUNC | O_CREAT;
		break;
	case 'a':
		oflag = (plus ? O_RDWR : O_WRONLY) | O_APPEND | O_CREAT;
		break;
	case 'r':
		oflag = plus ? O_RDWR : O_RDONLY;
		break;
	default:
#ifdef	_THREAD_SAFE
		seterrno(EINVAL);
		filelock = _flockfile(stream);
		stream->_flag = 0;		/* release the iob */
		_funlockfile(filelock);
#else
		errno = EINVAL;
#endif
		return (NULL);
	}
	if ((fd = open(filename, oflag, 0666)) < 0)
		return (NULL);
	if (mode[0] == 'a' && !plus)
		lseek(fd, (off_t)0, SEEK_END);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stream);
#endif
	stream->_cnt = 0;
	stream->_file = fd;
	stream->_flag = plus ? _IORW : (mode[0] == 'r') ? _IOREAD : _IOWRT;
	stream->_bufendp = stream->_base = stream->_ptr = NULL;
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	return (stream);
}
