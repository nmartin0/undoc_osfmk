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
#if !defined( lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#) $RCSfile: eprintf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:03:44 $";
#endif
/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !lint
static char sccsid[] = "eprintf.c	1.1  com/cmd/acct/lib,3.1,8935 5/15/89 17:16:48";
#endif
/*
 * flush stdout and then print arguments on stderr
 */

#include <stdio.h>
#include <varargs.h>
#ifdef NLS
#include <standards.h>
#endif

eprintf(va_alist) va_dcl
{
	va_list arglist;
	register char *fmt;
	register int ret;

	va_start(arglist);
	fmt = va_arg(arglist, char *);
	(void)fflush(stdout);
	ret = vfprintf(stderr, fmt, arglist);
	(void)fflush(stderr);
	va_end(arglist);
	return ret;
}
