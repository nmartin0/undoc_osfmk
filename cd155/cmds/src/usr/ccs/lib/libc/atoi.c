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
static char rcsid[] = "@(#)$RCSfile: atoi.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:03:28 $";
#endif
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
/*
 * COMPONENT_NAME: LIBCCNV atoi
 *
 * FUNCTIONS: atoi
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * atoi.c    1.11  com/lib/c/cnv,3.1,9021 5/1/90 09:43:19
 */

#include <limits.h>
#include <errno.h>
#include <math.h>

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

/*
 * NAME: atoi
 *                                                                    
 * FUNCTION: Converts a string to an integer
 *                                                                    
 * NOTES:
 *
 * RETURNS: returns an integer formed from *nptr
 *	    INT_MIN or INT_MAX if overflow occurs on conversion
 *
 */

int 	
atoi(const char *nptr)
{
	int n, sign;

	n = 0;
	sign = 0;
	while (isspace(*nptr)) nptr++;	/* skip leading whitespace */
	switch (*nptr) {
	case '-':
		sign++;
		/* Fall Through */
	case '+':
		nptr++;
	}
	while (isdigit(*nptr)) {
 	      /*
		Check for overflow.  If the current value (before
 	   	adding current digit) is already greater than
           	INT_MAX / 10, we know that another digit will
 	   	not fit.  Also if after the current digit is added,
           	if the new value is less than the old value, we 
 	   	know that overflow will occur.
              */
 		if (((n * 10 + *nptr - '0') < n) || (n > (INT_MAX /10))) {
			if (sign == 1)
				n = INT_MIN;
			else
				n = INT_MAX;
			SETERR(ERANGE);
			return (n);
		}
		n = n * 10 + *nptr++ - '0';
	}
	return(sign? -n: n);
}
