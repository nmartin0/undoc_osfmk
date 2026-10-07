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
static char rcsid[] = "@(#)$RCSfile: ungetc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:23 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: ungetc 
 *
 * ORIGINS: 3, 27 
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
 * sccsid[] = "ungetc.c  1.11  com/lib/c/io,3.1,9021 2/6/90 16:39:46";
 */

/*LINTLIBRARY*/
#include <stdio.h>
#ifdef  _THREAD_SAFE
#include "stdio_lock.h"
#endif

int	
ungetc(int c, FILE *stream)
{
#ifdef  _THREAD_SAFE
        register filelock_t filelock;
#endif

	if(c == EOF)
		return(-1);
#ifdef  _THREAD_SAFE
        filelock = _flockfile(stream);
#endif
	if(stream->_base == NULL)
		_findbuf(stream);
	if((stream->_flag & _IOREAD) == 0 || stream->_ptr <= stream->_base)
		if(stream->_ptr == stream->_base && stream->_cnt == 0)
			++stream->_ptr;
		else {
#ifdef  _THREAD_SAFE
                        _funlockfile(filelock);
#endif
			return(-1);
		}
	/* If the stream is really a string (possibly in read-only storage),
	   can't write to it. */
	if (stream->_flag & _IONOFD)
		--stream->_ptr;
	else {
		/* mark stream as having ungetc() chars on it.  See fseek() */
		stream->_flag |= _IOUNGETC;
		*--stream->_ptr = c;
	}
	++stream->_cnt;
#ifdef  _THREAD_SAFE
        _funlockfile(filelock);
#endif

	/* clear EOF */
	clearerr(stream);
	if (stream->_flag &  _IORW)
		stream->_flag |= _IOREAD;
	return(c);
}
