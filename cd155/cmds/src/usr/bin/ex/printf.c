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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char *rcsid = "@(#)$RCSfile: printf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:24:30 $";
#endif
/*
 * COMPONENT_NAME: (CMDEDIT) printf.c
 *
 * FUNCTION: printf
 *
 * ORIGINS: 3, 10, 13, 26, 27
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
 * printf.c  1.5  com/cmd/edit/vi,3.1,9013 10/10/89 16:34:28
 * 
 * 
 */

/*LINTLIBRARY*/
#include "ex.h"
#include <stdio.h>
#include <varargs.h>
#ifndef VPRINTF
#include <values.h>
extern int _doprnt();
#endif
/*
 * ex_printf performs the equivalent of an NLsprintf and then calls vi's
 * putchar() to output string[]
 */
/*VARARGS1*/
int
ex_printf(va_alist)
va_dcl
{
	static char string[4096];	/* for at least 2048 2-byte chars */
	register int count;
	char *format;
#ifndef VPRINTF
	FILE siop;
#endif
	va_list ap;

#ifndef VPRINTF
	siop._cnt = sizeof(string);
	siop._base = siop._ptr = (unsigned char *)string;
	siop._flag = (_IOWRT|_IONOFD);
#endif
	va_start(ap);
	format = va_arg(ap, char *);
	
#ifndef VPRINTF
	count = _doprnt(format, ap, &siop);
#else
	/* NB: vsprintf does NOT know sizeof(string)! */
	count = vsprintf(string, format, ap);
#endif
	va_end(ap);
#ifndef VPRINTF
	*siop._ptr = '\0'; /* plant terminating null character */
#endif
	if (count > 0) {
		register char *s = string;
		register NLchar nlc;
		while (nlc = NLsgetc(s))
			ex_putchar(nlc);
	}
	return(count);
}
