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
static char rcsid[] = "@(#)$RCSfile: wctomb.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:41 $";
#endif
/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: wctomb
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
 * wctomb.c	1.10  com/lib/c/cnv,3.1,9013 2/27/90 22:04:16
 */

#include <sys/types.h>
#include <stdlib.h>
#include <NLctype.h>

/*
 * FUNCTION: Multibye character and string functions.
 *           Converts international characters to multibyte characters .
 *
 * PARAMETERS: 
 *	     char    *s     - converted string
 *	     wchar_t wchar  - wide character to be converted
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns the number of bytes that comprise the multibyte char
 *	     - returns 0 if s is a NULL pointer
 *	     - returns -1 if wchar is not valid multibyte character
 *
 */

int 	
#ifdef _NO_PROTO
wctomb(s,wchar)
char *s;
wchar_t wchar;
#else
wctomb(char *s, wchar_t wchar)
#endif
{
	if (s == NULL)
		return(0);

	/* check for valid wide character  */
#ifdef _KJI
	if ( (wchar > 0xff) &&
	     ((_jctype1_[_jctype0_[wchar >> 8]] [wchar & 0xff]) == 0) )
#else
	if ( wchar > 255 )
#endif
		return (-1);

	if (wchar > 0xff) {		
		 s[0] = ( (wchar >> 8) & 0xff );
		 s[1] = ( wchar & 0xff );	
		 return (2);		
	} else { s[0] = ( wchar & 0xff );	
		 return (1);		
	}
}
