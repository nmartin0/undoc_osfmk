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
static char rcsid[] = "@(#)$RCSfile: floor.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:04:51 $";
#endif
/*
 * COMPONENT_NAME: (LIBM) math library
 *
 * FUNCTIONS: floor, ceil, nearest, trunc
 *
 * ORIGINS: 11, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1985 Regents of the University of California.
 * 
 * Use and reproduction of this software are granted  in  accordance  with
 * the terms and conditions specified in  the  Berkeley  Software  License
 * Agreement (in particular, this entails acknowledgement of the programs'
 * source, and inclusion of this notice) with the additional understanding
 * that  all  recipients  should regard themselves as participants  in  an
 * ongoing  research  project and hence should  feel  obligated  to report
 * their  experiences (good or bad) with these elementary function  codes,
 * using "sendbug 4bsd-bugs@BERKELEY", to the authors.
 *
 * floor.c	1.13  com/lib/m,3.1,8946 11/15/89 15:17:07
 */

#include <math.h>
#include <float.h>
#include <fp.h>

extern double modf ( double value , register double * iptr ) ;

/*
 *
 *   This module contains the following routines:
 *
 *      double    floor(double x);
 *
 *      double    ceil(double x);
 *
 *      double    nearest(double x);
 *
 *      double    trunc(double x);
 *
 *
 *      Most of the code in this module was adapted from floor.c
 *      in the Berkeley math library. It has been modified to assume
 *      that the underlying hardware conforms to the IEEE standard and
 *      that the IEEE required and recommended functions are
 *      available.
 *
 *      Where applicable, the routines have been changed
 *      to reflect the ANSI C standard.
 *
 *      The routines assume that double IEEE floating point numbers are
 *      encoded with the sign in the ms bit followed by the exponent in
 *      the next 11 ms bits. (This is the common representation found in
 *      in almost all current implementations of the IEEE standard.)
 *
 *      Lastly, these routines assume the existence of a hardware
 *      specific function (write_rnd) that writes a new IEEE rounding
 *      mode and returns the previous rounding mode. The function takes
 *      and returns rounding modes with the encodings specified by the
 *      ANSI C Standard.
 *
 */


/*
 * NAME: floor
 *                                                                    
 * FUNCTION: RETURNS THE GREATEST DOUBLE INTEGER <= X
 *                                                                    
 * NOTES:
 *
 * RETURNS: the largest fp integer not greate than x
 *
 */

double
#ifdef _FRTINT
_floor(double x)
#else
floor ( double x )
#endif
{
	double y ;  
	return ( modf ( x , & y ) < 0 ? y - 1 : y ) ;
}


#ifndef _FRTINT

/*
 * NAME: ceil
 *                                                                    
 * FUNCTION: RETURNS THE SMALLEST DOUBLE INTEGER >= X
 *                                                                    
 * NOTES:
 *
 * RETURNS: the smallest fp integer not less than x
 *
 */


double
ceil ( double x )
{
	double y ;  
	return ( modf ( x , & y ) > 0 ? y + 1 : y ) ;
}

/*
 * NAME: nearest
 *                                                                    
 * FUNCTION: RETURNS THE DOUBLE INTEGER NEAREST TO X.
 *                                                                    
 * NOTES:
 *   If x lies half way between two double integers then the even 
 *   integer is returned.
 *
 * RETURNS: nearest fp integer to x
 *
 */


double
nearest(double x)
{
	int oldmode;
	double retval;

	oldmode = write_rnd((unsigned int) FP_RND_RN);
	retval = rint(x);
	write_rnd((unsigned int) oldmode);
	return(retval);
}



/*
 * NAME: trunc
 *                                                                    
 * FUNCTION: RETURNS THE NEAREST DOUBLE INTEGER TO X IN THE DIRECTION OF 0.0.
 *                                                                    
 * NOTES:
 *
 * RETURNS: nearest fp integer to x in the direction of 0
 *
 */


double
trunc(double x)
{
	int oldmode;
	double retval;

	oldmode = write_rnd((unsigned int) FP_RND_RZ);
	retval = rint(x);
	write_rnd((unsigned int) oldmode);
	return(retval);
}

#endif
 
