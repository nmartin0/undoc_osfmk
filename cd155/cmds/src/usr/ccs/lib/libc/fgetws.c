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
static char rcsid[] = "@(#)$RCSfile: fgetws.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:47 $";
#endif
/*
 * LIBCIO: fgetws.c
 *
 * FUNCTIONS: fgetws
 *
 * ORIGINS: 10, 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */
/*
 * This function gets a string, possibly containing two-byte characters,
 * from a stream.  Fgetws reads characters from a stream, converts them
 * to NLchars, and places them in the NLchar array pointed to by ptr.
 * Fgetws reads until n-1 NLchar characters are transferred to ptr,
 * or a new-line character or an end-of-file condition is encountered.
 * (The new-line character is discarded.)  The NLchar string is then
 * terminated with a null character.  
 */
/* fgetws.c	1.1  com/lib/c/io/KJI,3.1,9021 10/30/89 11:11:12 */
#include <stdio.h>
#include <NLchar.h>

NLchar *
fgetws(ptr, size, stream)
register NLchar *ptr;
register int size;
register FILE *stream;
{
    NLchar *ptr0 = ptr;
    register int c;

    while (--size > 0) {
	c = getwc (stream);
	if (c == EOF && ptr0 == ptr)
	    return (NULL);
	if (c == '\n' || c == EOF) 
	    break;
	*ptr++ = c;
    }
    *ptr = '\0';
    return (ptr0);
}
