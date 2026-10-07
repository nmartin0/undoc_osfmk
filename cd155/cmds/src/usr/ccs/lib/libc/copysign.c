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
static char rcsid[] = "@(#)$RCSfile: copysign.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:02 $";
#endif
/*
 * COMPONENT_NAME: (LIBM) math library
 *
 * FUNCTIONS: copysign
 *
 * ORIGINS: 11, 26
 *
 * OBJECT CODE ONLY SOURCE MATERIALS
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
 * copysign.c	1.10  com/lib/m,3.1,8946 11/10/89 10:57:38
 */

#include <math.h>
#include <fp.h>

/*
 * NAME: copysign
 *                                                                    
 * FUNCTION: RETURNS X WITH THE SIGN OF Y.
 *                                                                    
 * NOTES:
 *
 * RETURNS: returns x with the sign of y
 *
 */

static unsigned long msign=0x7fffffff;

double copysign(double x,double y)
{
	VALH(x) = (VALH(x) & msign) | (VALH(y) & ~msign);
	return(x);
}

