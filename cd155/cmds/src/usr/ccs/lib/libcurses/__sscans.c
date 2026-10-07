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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)__sscans.c	1.6  com/lib/curses,3.1,8943 10/16/89 22:52:56";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   __sscans
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

# include	"cursesext.h"
# include	<varargs.h>

/*
 * NAME:        __sscans
 *
 * FUNCTION:    executes the scanf from the window.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      This code calls vsscanf, which is like sscanf except
 *      that it takes a va_list as an argument pointer instead
 *      of the argument list itself.  We provide one until
 *      such a routine becomes available.
 */

__sscans(win, fmt, ap)
WINDOW	*win;
char	*fmt;
va_list	ap;
{
	char	buf[256];

	if (wgetstr(win, buf) == ERR)
		return ERR;

	return vsscanf(buf, fmt, ap);
}
