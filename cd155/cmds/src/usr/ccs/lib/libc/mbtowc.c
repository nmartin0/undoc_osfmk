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
static char rcsid[] = "@(#)$RCSfile: mbtowc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:08 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mbtowc
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
 * mbtowc.c    1.12  com/lib/c/cnv,3.1,9021 3/17/90 15:29:50
 */

#include <sys/types.h>
#include <stdlib.h>
#include <NLchar.h>

/*
 * FUNCTION: Multibyte character and string functions.
 *           Converts multibyte characters to international characters.
 *
 * PARAMETERS: 
 *	     wchar_t *pwc  - pointer to resulting wide character 
 *	     char    *s    - input multibyte string to be converted
 *	     size_t  n     - maximum number of bytes to be examined
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the number of bytes that comprise the multibyte char
 *	     - returns 0 if s is a NULL pointer
 *	     - returns -1 if n bytes do not form valid multibyte character
 *
 */


int 	
#ifdef _NO_PROTO
mbtowc(pwc, s, n)
wchar_t *pwc; 
char *s; 
size_t n;
#else
mbtowc(wchar_t *pwc, const char *s, size_t n)
#endif
{
	int len;
	unsigned char *t = (unsigned char *)s;

	/* make sure s is a valid pointer */
	if (s == 0 )
		return(0);	/* No stateful encoding */

       if (n <= 0)
               return(-1);

       /* null string */
       if (*s == (int)NULL) {
               if (pwc != (wchar_t *)0)
                       *pwc = (wchar_t)0;
               return(0);
       }
       else {
		/* Determine the number of bytes : either 1 or 2 */
		len = NCisshift(t[0]) ? 2 : 1;
		/* len is guaranteed to be <= MB_CUR_MAX */
		if  (len > n)
			return(-1);
		else {
#ifdef _KJI
			if ( t[0] & 0x80 ) {
			    if ( (t[0] < 0xa0) && 
				 (t[0] > 0xdf) )
			         if (_jctype1_[_jctype0_[t[0]]] [t[1]] == 0) 
					return (-1);
			}
#endif
			if (pwc == 0)
				return(len);
			else {
				if (len > 1) {
		 			*pwc = ( (t[0] << 8) | (t[1] & 0xff) );
		 			return (2);			
				} else { 
					*pwc = t[0];
		 			return (1);
				}
			}
		}
      }
}
