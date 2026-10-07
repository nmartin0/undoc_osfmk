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
static char rcsid[] = "@(#)$RCSfile: NLvprintf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:03:11 $";
#endif
/*
 * COMPONENT_NAME: (LIBCPRNT) Standard C Library Print Functions 
 *
 * FUNCTIONS: NLvprintf
 *
 * ORIGINS: 3, 27 
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1987, 1989 
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * NLvprintf.c	1.3  com/lib/c/prnt,3.1,8943 9/7/89 10:05:12
 */

#include <stdio.h>
#include <varargs.h>

/*                                                                    
 * FUNCTION: Writes output to to the stream pointed to by stream, under
 *           control of the string pointed to by format, that specifies
 *           how subsequent argumnts are converted for output.
 *
 * PARAMETERS: format - format used to print arguments
 *	       args   -   arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      If successful, returns number of characters printed
 *	      Otherwise returns negative value
 */

int
NLvprintf (char *format, va_list args)
{       
	register int rc;	/*---	return code ---*/

	rc = _doprnt (format, args, stdout);
	return (ferror(stdout)? EOF: rc);
}
