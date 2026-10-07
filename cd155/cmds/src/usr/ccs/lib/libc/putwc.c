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
static char rcsid[] = "@(#)$RCSfile: putwc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:35 $";
#endif
/*
 * FUNCTIONS: putwc 
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * putwc.c	1.5  com/lib/c/io,3.1,8943 9/9/89 13:30:54
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <NLchar.h>

/*
 * FUNCTION:	
 * Put a wide char to stream.
 *
 * RETURN VALUE DESCRIPTION:	
 * Returns EOF on any error condition.
 *
 */

int putwc(int c, FILE *stream)
{
	NLchar	nlc = c;
	char	buf[2];
#ifdef	_THREAD_SAFE
	register filelock_t filelock;
#endif
	c = NCencode(&nlc,buf);

#ifdef	_THREAD_SAFE
	filelock = _flockfile(stream);
	if (c == 1)
		c = unlocked_putc(buf[0],stream);
	else if (unlocked_putc(buf[0],stream) == EOF)
		c = EOF;
	else if (unlocked_putc(buf[1],stream) == EOF)
		c = EOF;
	_funlockfile(filelock);
	return(c);
#else
	if (c == 1)
		return(putc(buf[0],stream));
	if (putc(buf[0],stream) == EOF)
		return(EOF);
	if (putc(buf[1],stream) == EOF)
		return(EOF);
	return(c);
#endif
}
