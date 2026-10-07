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
static char rcsid[] = "@(#)$RCSfile: strtol.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 02:08:44 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV strtol
 *
 * FUNCTIONS: strtol
 *
 * ORIGINS: 27
 *
 * IBM CONFIDENTIAL -- (IBM Confidential Restricted when
 * combined with the aggregated modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * strtol.c	1.19  com/lib/c/cnv,3.1,9013 2/26/90 21:55:08
 */

#include <limits.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#ifdef  _THREAD_SAFE
#define SETERR(err)     seterrno(err)
#else
#define SETERR(err)     errno = err
#endif

#define DIGIT(x) (isdigit((int)x)? ((x)-'0'): (10+tolower((int)x)-'a'))
#define MBASE 36

/*
 * NAME: strtol
 *                                                                    
 * FUNCTION: Converts a string to an integer
 *                                                                    
 * NOTES:
 *
 * RETURNS: returns a long integer formed from *nptr
 *	    returns 0 if an integer could not be formed
 *	    ULONG_MAX if overflow occurs on conversion
 *
 */


long  int
strtol(const char *nptr, char **endptr, int base)
{
	char *orig;
	unsigned long val;
	int xx, sign;
	int digits = 0;        /* number of valid digits read */
	int overflag = 0;
 	unsigned long maxval;

	val = 0L;
	sign = 1;
	if(base < 0 || base > MBASE) {
		SETERR(EINVAL);
	} else {
		/*
		 * If nptr is NULL, return 0, leave endptr alone.
		 */
		if (nptr)
			orig = (char *)nptr;
		else
			return(0L);
		while (isspace(*nptr))
			++nptr;
		if (*nptr == '-') {
			++nptr;
			sign = -1;
		} else if (*nptr == '+') {
		  	++nptr;
	       	}
		if (base == 0) {
			if (*nptr == '0') {
				++nptr;
				if (*nptr == 'x' || *nptr == 'X') {
					++nptr;
					base = 16;
				} else {
					base = 8;
					++digits;
				}
			} else
				base = 10;
		} else if (base == 16)
			if (nptr[0] == '0' && (nptr[1] == 'x'||nptr[1] == 'X'))
				nptr += 2;
		/*
	 	 * for any base > 10, the digits incrementally following
	 	 *	9 are assumed to be "abc...z" or "ABC...Z"
	 	 */

 		/* Check for overflow.  If the current value (before
 	   	 * adding current xx digit) is already greater than
           	 * ULONG_MAX / base, we know that another digit will
 	   	 * not fit.  Also if after the current digit is added,
           	 * if the new value is less than the old value, we 
 	   	 * know that overflow will occur.
         	 */
 		maxval = ULONG_MAX / (long)base;
		while(isalnum((int)*nptr) && (xx=DIGIT(*nptr)) < base) {
 			if ((val > maxval) || (base*val + xx < val)) {
				overflag = 1;
				if (sign == 1)
					val = LONG_MAX;
				else
					val = LONG_MIN;
				SETERR(ERANGE);
				break;
			}
			val = base*val + xx;
			++nptr;
			++digits;
		}
		/* If overflow occurred, keep scanning characters because
		 * the endptr is the first character that is not consistant
		 * with the base.
		 */
		if (overflag) {
			while(isalnum((int)*nptr) && (xx=DIGIT(*nptr)) < base)
				++nptr;
		}
	}
	if(endptr) {
		/*
		 * If endptr is not NULL, and if nptr is not NULL, then
	 	 * nptr has an invalid final string.  If there is no valid
		 * subject sequence, return unsigned 0 and nptr in *endptr.
		 * Else return the invalid final string in *endptr.
		 */
		if(!digits) {
			*endptr = orig;
			sign = 1;
		} else {
			*endptr = (char *)nptr;
		}
	} else {
		/*
		 * If *endptr is NULL, and there is no valid subject
		 * sequence, then return unsigned 0.
		 */
		if(!digits)
			sign = 1;
	}
	return(sign*val);
}
