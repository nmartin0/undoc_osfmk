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
static char rcsid[] = "@(#)$RCSfile: uitrunc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:20 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV uitrunc
 *
 * FUNCTIONS: uitrunc
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
 * uitrunc.c	1.7  com/lib/c/cnv,3.1,8943 10/27/89 16:15:18
 */

/*
 * NAME: uitrunc
 *                                                                    
 * FUNCTION: return an unsigned integer
 *                                                                    
 * NOTES:
 *
 *   unsigned uitrunc(double x);    --- returns unsigned int
 *
 *   Portable Version
 *
 *   Returns the nearest unsigned integer to x in the direction of
 *   0.0. This is equivalent to discarding the fraction of x (truncating x).
 *
 *   Note: This routine differs from trunc() because it returns an
 *         integer rather a double. This function is trivial
 *         on any machine that has hardware/software to correctly
 *         convert the truncated double to an integer as specified
 *         in the IEEE standard. This may not be the case for all
 *         hardware in which case this routine would have to handle
 *         things like infinities, NaN's, etc.
 *
 * RETURNS: an unsigned integer
 *	    0 for NaN or negative number
 *
 */

#include <math.h>
#include <float.h>
#include <fp.h>

/* The following static constant is used by rint and uitrunc */

static double L = 4503599627370496.0E0;   /* 2**52 -- the magic number */


unsigned
uitrunc(x)
double x;
{
	unsigned oldmode;
	double y;

	if (( x != x ) || (x < 0.0))  {     /* if NaN or negative number */
	      /*setfpflag(FP_INVALID);        set invalid exception bit */
	      y = HUGE_VAL - HUGE_VAL;
	      return(0);                    /* return 0 */
	}

	if ( x >= 4294967296.0) {           /* if x too big to convert   */
	      /*setfpflag(FP_INVALID);        set invalid exception bit */
	      y = HUGE_VAL - HUGE_VAL;
	      return(0xffffffff);           /* return max unsigned nbr.  */
	}

	oldmode = write_rnd(FP_RND_RZ);  /* Set to do truncate         */
	y = x + L;                       /* Add the magic number       */
	write_rnd(oldmode);              /* restore caller's rnd mode  */

	/* The integer is now right justified in the low part of y  */
	return(VALL(y));
}
