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
static char rcsid[] = "@(#)$RCSfile: sprintf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:23 $";
#endif
/*
 * FUNCTIONS: sprintf, wsprintf 
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
 * sprintf.c	1.12  com/lib/c/prnt,3.1,8943 9/13/89 11:44:42
 */

/*LINTLIBRARY*/
#include <limits.h>
#include <stdio.h>
#include <stdarg.h>
#include <values.h>

extern int _doprnt();

/*VARARGS2*/

/*                                                                    
 * FUNCTION: Writes output to the array specified by s, under
 *           control of the string pointed to by format, that specifies
 *           how subsequent argumnts are converted for output.
 *
 * PARAMETERS: s      - array to be printed to
 *             format - format used to print arguments
 *	       ...    -   arguments to be printed
 *
 * RETURN VALUE DESCRIPTIONS:
 * 	      Returns number of characters printed
 */                                                                   

#ifdef _BSD
char *
#else	/* _BSD */
int
#endif	/* _BSD */
sprintf(char *s, const char *format, ...) 
{
	int count;
	FILE siop;
	va_list ap;

#ifdef _THREAD_SAFE
	siop._lock = NULL;
#endif
	siop._cnt = INT_MAX;
	siop._base = siop._ptr = (unsigned char *)s;
	siop._flag = (_IOWRT|_IONOFD);
	va_start(ap,format);
	count = _doprnt(format, ap, &siop);
	va_end(ap);
	*siop._ptr = '\0'; /* plant terminating null character */
#ifdef _BSD
	return (s);
#else	/* _BSD */
	return(count);
#endif /* _BSD */
}
#ifndef _BSD
#ifdef KJI
#include <wchar.h>
#undef wsprintf


/*VARARGS2*/
int
wsprintf(wchar_t *s, const char *format, ...) 
{
	int count;
	FILE siop;
	va_list ap;
        unsigned char *buf, *sp;
        int len;


        buf = (unsigned char*)s;
#ifdef _THREAD_SAFE
	siop._lock = NULL;
#endif
	siop._cnt = INT_MAX;
	siop._base = siop._ptr = (unsigned char *)buf;
	siop._flag = (_IOWRT|_IONOFD);
	va_start(ap,format);
	count = _doprnt(format, ap, &siop);
	va_end(ap);
	*siop._ptr = '\0'; /* plant terminating null character */

        len = strlen(buf);
        sp = malloc (len * sizeof(unsigned char)+1);
        if (sp == NULL)
            return (EOF);
        strncpy(sp, buf, len);
        strtows (s, sp);

        
	return(count);
}
#endif /* KJI */
#endif /* _BSD */

