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
static char rcsid[] = "@(#)$RCSfile: setbuffer.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:01 $";
#endif

/*
 * FUNCTIONS: setbuffer, setlinebuf 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 * setbuffer.c 1.5  com/lib/c/io,3.1,9013 10/18/89 15:08:42
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <malloc.h>

/*                                                                    
 * FUNCTION: Assigns buffering to a stream.
 *
 * RETURN VALUE DESCRIPTION: None.
 */  

/*
 * setbuffer - setup stdio stream buffering after opening, but before
 *	it is read or written.
 */
void setbuffer( stream, buf, size )
FILE	*stream;
char	*buf;
int	size;
/* size_t	size; */
{
	/* we just call the sysV setvbuf(3) */
	(void)setvbuf( stream, buf, _IOFBF, size );
	/* BSD returns indeterminate value */
}

/*
 * setlinebuf - change stdio stream buffering from block or unbuffered to
 *	line buffered, may be used even after reading or writting.
 */
void setlinebuf(stream)
FILE	*stream;
{
	char	*buf;		/* ptr for tmp usage */

#ifdef	_THREAD_SAFE
	register filelock_t filelock;

	filelock = _flockfile(stream);
#endif

#ifdef	_THREAD_SAFE
	unlocked_fflush(stream);
	/* close down buffering */
	unlocked_setvbuf(stream,(char *)NULL,_IONBF,(size_t)0);
#else
	fflush(stream);	/* force out all output */
	setvbuf(stream,(char *)NULL,_IONBF,(size_t)0);/* close down buffering */
#endif
	buf = (char *)malloc((size_t)BUFSIZ);	      /* get a new buffer */
	if( buf != NULL ) {
#ifdef	_THREAD_SAFE
		(void)unlocked_setvbuf(stream, buf, _IOLBF, (size_t)BUFSIZ);
#else
		(void)setvbuf(stream, buf, _IOLBF, (size_t)BUFSIZ);
#endif
		/* say this buffer belongs to stdio */
		stream->_flag |= _IOMYBUF;
	}
	/* BSD returns indeterminate value */
#ifdef	_THREAD_SAFE
	_funlockfile(filelock);
#endif
}
