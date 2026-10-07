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
static char rcsid[] = "@(#)$RCSfile: mblen.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:07:00 $";
#endif

/*
 * COMPONENT_NAME: (LIBCNLS) Standard C Library National Language Support
 *
 * FUNCTIONS: mblen
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
 * sccsid[] = "mblen.c   1.10  com/lib/c/cnv,3.1,9021 3/29/90 14:34:09";
 */

#include <stdlib.h>
#include <NLchar.h>

/*
 * FUNCTION: Multibye character and string functions.
 *           Return number of multibytes comprising a multibyte character
 *
 * PARAMETERS: 
 *	     char    *s1    - string to be converted
 *	     size_t  n	    - number of bytes to be evaluated
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns number of bytes comprising character pointed to by s
 *	     - returns 0 if s is NULL pointer or points at the NULL character
 *	     - returns -1 if n bytes not sufficient to form a valid character
 */


int 	
#ifdef _NO_PROTO
mblen(s, n)
char *s;
size_t n;
#else
mblen(const char *s, size_t n)
#endif
{
	int retval;
	unsigned char *t = (unsigned char *)s;

	/* Make sure s is a valid pointer */
	if (s == (char *)NULL || *s == (int)NULL)
		return(0);
	if (n < 1)			/* n too small... */
		return (-1);

	else {
#ifdef _KJI
		/* retval is length of next valid character */
		retval = 0;
		if (t[0] < 0x80 )
			retval = 1;
		else if ( (t[0] >= 0xa0) && 
			  (t[0] <= 0xdf) )
			retval = 1;
		     else {
			    if ( (n < 2) || !t[0] || (t[1] < 0x40) )
				return (-1);
			    if ( _jctype1_[_jctype0_[ t[0] ]] [ t[1] ] != 0 ) 
				retval = 2;
		     }
		return(retval);
#else
		return(1);
#endif

	}
}
