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
static char rcsid[] = "@(#)$RCSfile: NLscanf.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:02:53 $";
#endif

/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: (LIBCIO) Standard C Library I/O Functions 
 *
 * FUNCTIONS: NLscanf, NLfscanf, NLsscanf 
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
 * sccsid[] = "NLscanf.c 1.15  com/lib/c/io,3.1,9021 1/18/90 09:49:39";
 */

#include <stdio.h>
#ifdef  _THREAD_SAFE
#include "stdio_lock.h"
#endif
#include <stdarg.h>

/* 
 * EXTERNAL PROCEDURE CALLED
 */

extern int _doscan();

/*
 * NAME: NLscanf 
 *
 * FUNCTION: Reads character data including NLchars from stdin,    
 *           according to fmt and save data to va_alist. 
 *
 * NOTE:     extern _doscan() does all the scan and match.
 *
 * RETURN VALUE DESCRIPTION: Returns the number of matches found between
 *           fmt and va_alist, and EOF on the end of the input. 
 */

int
NLscanf(unsigned char *fmt, ...) 
{
#ifdef  _THREAD_SAFE
        register int rc;
        register filelock_t filelock;
#endif
        va_list ap;     /* the argument list */ 

	va_start(ap, fmt);	/* initialize ap */ 
#ifdef  _THREAD_SAFE
        filelock = _flockfile(stdin);
        rc = _doscan(stdin, fmt, ap);
        _funlockfile(filelock);

        return(rc);
#else
	return(_doscan(stdin, fmt, ap));
#endif
}

/*
 * NAME: NLfscanf 
 *
 * FUNCTION: Reads character data including NLchars from stream iop,   
 *           according to fmt and save data to va_alist. 
 *
 * NOTE:     extern _doscan() does all the scan and match.
 *
 * RETURN VALUE DESCRIPTION: Returns the number of matches found between
 *           fmt and va_alist, and EOF on the end of the input. 
 */

int
NLfscanf(FILE *iop, unsigned char *fmt, ...)
/* FILE *iop		 an input file */
/* char *fmt       	 a conversion specifications */
{
#ifdef  _THREAD_SAFE
        register int rc;
        register filelock_t filelock;
#endif
        va_list ap;	/* argument list */

	va_start(ap, fmt);   /* initialize ap */
#ifdef  _THREAD_SAFE
        filelock = _flockfile(stdin);
        rc = _doscan(iop, fmt, ap);
        _funlockfile(filelock);

        return(rc);
#else
        return(_doscan(iop, fmt, ap));
#endif
}

/*
 * NAME: NLsscanf 
 *
 * FUNCTION: Read character data including NLchars from str according
 *           to fmt and save data to va_alist.  
 *
 * NOTE:     extern _doscan() does all the scan and match.
 *
 * RETURN VALUE DESCRIPTION: Returns the number of matches found between
 *           fmt and va_alist, and EOF on the end of the input. 
 */

int
NLsscanf(unsigned char *str, unsigned char *fmt, ...)
/* char *str	 	an input string */
/* char *fmt           the conversion specifications */  
{
	va_list ap;	/* argument list */
	FILE strbuf;    /* temporary file */

	va_start(ap, fmt);	/* initialize ap */

/*  Convert data type from char to FILE */ 
	strbuf._flag = (_IOREAD | _IONOFD);
	strbuf._ptr = strbuf._base = (unsigned char*)str;
	strbuf._cnt = strlen(str);
	strbuf._file = _NFILE;
	return(_doscan(&strbuf, fmt, ap));
}
