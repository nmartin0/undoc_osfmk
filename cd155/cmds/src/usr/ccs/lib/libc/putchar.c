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
static char rcsid[] = "@(#)$RCSfile: putchar.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:26 $";
#endif
/*
 * FUNCTIONS: putchar 
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
 * putchar.c	1.10  com/lib/c/io,3.1,8943 9/9/89 13:30:14
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#undef unlocked_putchar
#endif
#undef putchar

/*
 * FUNCTION:	A subroutine version of the macro putchar.  This function is
 *		created to meet ANSI C standards.  The putchar function writes
 *		the character specified by c (converted to an unsigned char)
 *		to stdout, at the position indicated by the assoicated file
 *		poistion indicator for the stream, and advances the indicator
 *		appropriately.
 *
 * RETURN VALUE DESCRIPTION:	
 *		The putchar function returns the character written.  If a write
 *		error occurs, the error indicator for the stream is set and
 * 		putchar returns EOF.
 *
 */                                                                   

int 	
putchar(int c)
{
#ifdef	_THREAD_SAFE
	return(locked_putc(c, stdout));
#else
	return(putc(c, stdout));
#endif
}

#ifdef	_THREAD_SAFE
int 	
unlocked_putchar(int c)
{
	return(unlocked_putc(c, stdout));
}
#endif
