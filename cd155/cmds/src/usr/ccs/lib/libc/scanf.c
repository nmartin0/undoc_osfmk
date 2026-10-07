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
static char rcsid[] = "@(#)$RCSfile: scanf.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:07:56 $";
#endif
/*
 * FUNCTIONS: scanf, fscanf, sscanf 
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
 * scanf.c	1.12  com/lib/c/io,3.1,8943B 10/30/89 11:04:47
 */

#include <stdio.h>
#ifdef	_THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdarg.h>

extern int _doscan();

/*
 * FUNCTION:	The scanf function reads input from stdin, under control
 *		of the string pointed to by format that specifies the 
 *		admissible input sequences and how they are to be converted
 *		for assignment.
 *
 * RETURN VALUE DESCRIPTION:	
 *		returns the value of EOF if an input failure occurs before
 *		any conversion.  Otherwise, the fscanf function returns
 *		the number of input items assigned.
 *
 */                                                                   

int	
scanf(const char *format, ...) 
{
	va_list ap;
#ifdef	_THREAD_SAFE
	register int rc;
	register filelock_t filelock;
#endif

	va_start(ap,format);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stdin);
	rc = _doscan(stdin, format, ap);
	_funlockfile(filelock);

	return rc;
#else
	return(_doscan(stdin, format, ap));
#endif
}

int	
fscanf(FILE *stream, const char *format, ...)
{
	va_list ap;
#ifdef	_THREAD_SAFE
	register int rc;
	register filelock_t filelock;
#endif

	va_start(ap,format);
#ifdef	_THREAD_SAFE
	filelock = _flockfile(stream);
	rc = _doscan(stream, format, ap);
	_funlockfile(filelock);

	return rc;
#else
	return(_doscan(stream, format, ap));
#endif
}

int	
sscanf(const char *s, const char *format, ...) 
{
	va_list ap;
	FILE strbuf;

	va_start(ap,format);
	strbuf._flag = (_IOREAD|_IONOFD);
	strbuf._ptr = strbuf._base = (unsigned char*)s;
	strbuf._cnt = strlen(s);
#ifdef _THREAD_SAFE
	strbuf._lock = NULL;
#endif
#if 0
	strbuf._file = _NFILE;
#endif
	if (ap.mem_ptr == NULL) 
		ap.mem_ptr = (long *)malloc(80);

	return(_doscan(&strbuf, format, ap));
}

#ifdef KJI
typedef unsigned short wchar;

int	
wsscanf(const wchar *s, const char *format, ...) 
{
	va_list ap;
	FILE strbuf;
	unsigned char *buf;
	int len;

	len = wstrlen (s);
	buf = malloc (len * sizeof(wchar)+1);
	if (buf == NULL)
	    return (EOF);

	wstrtos (buf, s);

	va_start(ap,format);
	strbuf._flag = (_IOREAD|_IONOFD);
	strbuf._ptr = strbuf._base = buf;
	strbuf._cnt = strlen(buf);
#ifdef _THREAD_SAFE
	strbuf._lock = NULL;
#endif
#if 0
	strbuf._file = _NFILE;
#endif

	len = _doscan(&strbuf, format, ap);

	free (buf);
	return (len);
}
#endif
