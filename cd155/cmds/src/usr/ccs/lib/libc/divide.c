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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: divide.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:17 $";
#endif
/*
 * COMPONENT_NAME: (LIBCGEN) Standard C Library General Functions
 *
 * FUNCTIONS: div, ldiv
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
 * divide.c	1.7  com/lib/c/gen,3.1,8943 9/8/89 08:40:32
 */

#include <stdlib.h>
#include <limits.h>

/*
 * NAME: div
 *
 * FUNCTION: Find a quotient and a remainder
 *
 * PARAMETERS: 
 *	     int num - number to be divided
 *	     int denom - denominator
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns a structure of type div_t (see stdlib.h)
 *	     - structure contains a quotient and a remainder
 *
 */


struct div_t	
div(int numer, int denom)
{
	struct div_t xdiv;

        if (denom == 0) {
		if (numer >= 0)
			xdiv.quot = INT_MAX;
		else
			xdiv.quot = INT_MIN;
		xdiv.rem = 0;
	} else {
		xdiv.rem = numer % denom;
		xdiv.quot = numer / denom;
	}
	return(xdiv);
}

/*
 * NAME: ldiv
 *
 * FUNCTION: Find a quotient and a remainder
 *
 * PARAMETERS: 
 *	     long num - number to be divided
 *	     long denom - denominator
 *
 * RETURN VALUE DESCRIPTIONS:
 *	     - returns a structure of type ldiv_t (see stdlib.h)
 *	     - structure contains a quotient and a remainder
 *
 */
#include <stdlib.h>
#include <limits.h>

struct ldiv_t
ldiv(long int numer, long int denom)
{
	struct ldiv_t xldiv;

        if (denom == 0) {
		if (numer >= 0)
			xldiv.quot = LONG_MAX;
		else
			xldiv.quot = LONG_MIN;
		xldiv.rem = 0;
	} else {
		xldiv.rem = numer % denom;
		xldiv.quot = numer / denom;
	}
	return(xldiv);
}
