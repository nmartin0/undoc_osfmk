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
#ifndef _FP_H_
#define _FP_H_

#include <machine/endian.h>

/*
 *      VALH(val)
 *
 *      Return the hipart of the double value of val as an unsigned integer.
 *      val must be a simple variable that can have its address taken.
 */

#define VALH(val) *((unsigned int *)&(val) + (BYTE_ORDER!=BIG_ENDIAN))


/*
 *      VALL(val)
 *
 *      Return the lopart of the double value of val as an unsigned integer.
 *      val must be a simple variable that can have its address taken.
 */

#define VALL(val) *(((unsigned int *)&(val))+(BYTE_ORDER==BIG_ENDIAN))

/*
 *      DBL(val,hi,lo)
 *
 *      The hi word of the double variable val is set to hi and
 *      the lo word of the double variable val is set to lo.
 *
 *      val must be a simple variable.
 */
#define DBL(val,hi,lo)	\
{			\
	VALH(val) = hi;	\
	VALL(val) = lo;	\
}

/*
 *      FINITE(x)
 *
 *      Is true if double x is finite (Not NaN or INF).
 *
 *      This macro is similar to the finite(x) function in the IEEE
 *      standard except it is not a function. 
 *	x must be a simple variable and not an expresion.
 */

#define FINITE(x)	( ( VALH(x) & 0x7ff00000 ) != 0x7ff00000 )

/*
 *      IS_INF(x)
 *
 *      Is true if double x is +INF or -INF.
 *      x must be a simple variable and not an expression.
 */

#define IS_INF(x) \
	( ( ( VALH(x) & 0x7fffffff ) == 0x7ff00000 ) && \
		( VALL(x) == 0 ) )

/*
 *      IS_QNAN(x)
 *
 *      Is true if double x is a quiet NaN.
 *      x must be a simple variable and not an expression.
 */

#define IS_QNAN(x) \
	( ( VALH(x) & 0x7ff80000 ) == 0x7ff80000 )

/*
 *      IS_NAN(x)
 *
 *      Is true if double x is any NaN.
 *      x must be a simple variable and not an expression.
 */

#define IS_NAN(x) \
	( ( ( VALH(x) & 0x7ff00000 ) == 0x7ff00000 ) && \
		( ( VALH(x) & 0x000fffff) | VALL(x) ) )

/*
 *      IS_ZERO(x)
 *
 *      Is true if double x is +0 or -0.
 *      x must be a simple variable and not an expression.
 */

#define IS_ZERO(x) \
	( !( ( VALH(x) & 0x7fffffff ) | VALL(x) ) )

/*
 *  INTS2DBL(x,y)
 *
 *  Put two unsigned long integers into IEEE double format for the
 *  current machine architecture.
 */
#if BYTE_ORDER == BIG_ENDIAN
#	define	INTS2DBL(x,y)	(x),(y)
#else
#	define	INTS2DBL(x,y)	(y),(x)
#endif /* BYTE_ORDER==BIG_ENDIAN */

#endif /* _FP_H_ */
