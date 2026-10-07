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
static char rcsid[] = "@(#)$RCSfile: ftell.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:15 $";
#endif
/*
 * FUNCTIONS: ftell 
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
 * static char sccsid[] = "%Z%%M%	%I%  %W% %G% %U%";
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <errno.h>

extern long lseek();

/*                                                                    
 * FUNCTION: Returns the number of bytes from the beginning of the file
 *           for the current position in the stream.
 *
 * PARAMETERS: FILE *stream      - stream to be searched
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      zero if successful
 *	      -1 if not successful
 *	      errno is set on error to indicate the error
 */

long int
ftell(FILE *stream)
{
	long	tres;
	int adjust;
#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
#endif
	if(stream->_cnt < 0)
		stream->_cnt = 0;
	if(stream->_flag & _IOREAD)
		adjust = - stream->_cnt;
	else if(stream->_flag & (_IOWRT | _IORW)) {
		adjust = 0;
#ifdef	_THREAD_SAFE
		(void) unlocked_fflush(stream);
#else
		(void) fflush(stream);
#endif
		if(stream->_flag & _IORW) {
			stream->_cnt = 0;
			stream->_flag &= ~_IOWRT;
			stream->_ptr = stream->_base;
		}
	} else {
#ifdef	_THREAD_SAFE
		_funlockfile(filelock);
		seterrno(EBADF);
#else
		errno = EBADF;
#endif
		return(-1);
	}
	tres = lseek(fileno(stream), 0L, 1);
	if(tres >= 0)
		tres += (long)adjust;

#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
	return(tres);
}
