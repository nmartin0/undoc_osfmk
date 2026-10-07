/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: fp.h,v $
 * Revision 1.3  1994/11/18  21:09:42  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:16  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:15:46  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:26  nandy
 * Initial revision
 *
 * Revision 1.4  92/04/13  14:35:28  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.4  90/10/07  20:27:26  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:45:25  gm]
 * 
 * Revision 1.3  90/08/09  14:26:10  devrcs
 * 	Correct big- vs little-endian problems with VALL and VALH macros
 * 	[90/08/01  15:49:13  lehotsky]
 * 
 * Revision 1.2  90/03/13  21:22:15  mbrown
 * 	AIX merge first cut - new file.
 * 	[90/02/12  18:10:50  tom]
 * 
 * $EndLog$
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
