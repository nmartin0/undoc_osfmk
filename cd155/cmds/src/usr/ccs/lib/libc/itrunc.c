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
static char rcsid[] = "@(#)$RCSfile: itrunc.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:06:35 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV itrunc
 *
 * FUNCTIONS: itrunc
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
 * itrunc.c	1.7  com/lib/c/cnv,3.1,8943 9/13/89 09:23:52
 */

#include <math.h>
#include <limits.h>

/*
 * NAME: itrunc
 *                                                                    
 * FUNCTION: returns the nearest signed integer to x in the direction of 0
 *                                                                    
 * NOTES:
 *
 *   int  itrunc(double x);          --- returns signed int
 *
 *   Portable Version
 *
 *   Returns the nearest signed integer to x in the direction of
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
 * RETURNS: a integer value
 *	    LONG_MIN is paramter is a NaN
 *
 */

int
itrunc(double x)
{
	if (x != x)			/* Check for NaN */
		return (LONG_MIN);

	return(trunc(x));  /* assume hardware does conversion correctly */
}
