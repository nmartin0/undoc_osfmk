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
static char rcsid[] = "@(#)$RCSfile: mbstowcs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:07 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbstowcs
 *
 * ORIGINS: 27
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
 * mbstowcs.c  1.14  com/lib/c/cnv,3.1,9021 3/17/90 15:29:26
 */

#include <sys/types.h>
#include <stdlib.h>

/*
 * FUNCTION: Multibye character and string functions.
 *           Converts multibyte character string to international char string
 *
 * PARAMETERS: 
 *	     wchar_t *pwc  - pointer to resulting wide character string
 *	     char    *s    - multibyte string to be converted
 *	     size_t  n     - maximum number of wide characters to be stored
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the number of array elements that were modified not
 *                      including a terminating zero code, if any
 *	     - returns -1 if an invalid multibyte character is encountered
 *
 */


size_t 	
#ifdef _NO_PROTO
mbstowcs(pwcs, s0, n)
wchar_t *pwcs;
char *s0;
size_t n;
#else
mbstowcs(wchar_t *pwcs, const char *s0, size_t n)
#endif
{
	char *s = (char *)s0;
	wchar_t *opwcs = pwcs;
	long strlen = n;
	int  cnt;

	/*  While the pointer is pointing to valid information,
         *  call mbtowc to return wide character string 
         */
	while (strlen-- > 0) {
               if ((cnt = mbtowc(pwcs, s, (size_t)2)) < 0)
                       return(-1);
               if (*pwcs == (wchar_t)NULL)
			break;
	       s += cnt;
	       ++pwcs;
	}
	return (pwcs - opwcs);
}
