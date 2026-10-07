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
static char rcsid[] = "@(#)$RCSfile: wcstombs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:39 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wcstombs
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
 * wcstombs.c	1.7  com/lib/c/cnv,3.1,8943 9/14/89 08:44:58
 */

#include <sys/types.h>
#include <stdlib.h>
#include <NLchar.h>

/*
 * FUNCTION: Multibyte character and string functions.
 *           Converts international character string to multibyte char string.
 *
 * PARAMETERS: 
 *	     char    *s    - pointer to resulting multibyte string
 *	     wchar_t *pwc  - pointer to sequence of codes to be converted
 *	     size_t  n     - maximum number of bytes to be modified in
 *                              array pointed to by s
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the number of bytes modified, not including a
 *                      terminating null character, if any.
 *	     - returns 0 if s is a NULL pointer
 *	     - returns -1 if an invalid multibyte is encountered
 *
 */


size_t 	
#ifdef _NO_PROTO
wcstombs(s, pwcs0, n)
char *s;
wchar_t *pwcs0;
size_t n;
#else
wcstombs(char *s, const wchar_t *pwcs0, size_t n)
#endif
{
	wchar_t *pwcs = (wchar_t *)pwcs0;
	int slen;
	char *os = s;
        int cnt;
        signed long localn = (signed long) n;

	/*  While the pointer is pointing to valid information,
         *  call wctomb to return multibyte character string 
         */
	if (0 < localn)
		for (; ;) {
			if ((localn -= (slen = NCchrlen(*pwcs))) < 0)
				break;
			if ((cnt = wctomb(s, *pwcs)) == -1)
				return (-1);
			if (*pwcs == (wchar_t)NULL)
				break;
			s += slen;
                        ++pwcs;
		}
	return (s - os);
}
