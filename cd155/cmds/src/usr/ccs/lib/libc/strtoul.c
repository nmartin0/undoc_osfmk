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
/*
 * COMPONENT_NAME: LIBCCNV strtoul
 *
 * FUNCTIONS: strtoul
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
 * strtoul.c   1.10  com/lib/c/cnv,3.1,9021 12/7/89 20:35:12";
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
 * NAME: strtoul
 *                                                                    
 * FUNCTION: Converts a string to an integer
 *                                                                    
 * RETURNS: returns an unsigned long integer formed from *nptr
 *	    returns 0 if an integer could not be formed
 *	    ULONG_MAX if overflow occurs on conversion
 *
 */


unsigned long int
strtoul(const char *nptr, char * *endptr, int base)
{
	unsigned long val;
	int xx;
	int signchar = 0;      /* true if a "+" precedes the number */
	int digits = 0;        /* number of valid digits read */
	unsigned long maxval;
	char *ptr = (char *) nptr;

	val = 0L;
	if(base >= 0 || base <= MBASE) {
		while(isspace(*ptr))
			++ptr;

		if(*ptr == '+') 
		  	++ptr;
		else if(*ptr == '-'){
		        ++ptr;
		  	signchar = 1;
		}

		if(base == 0) {
			if(*ptr == '0') {
				++ptr;
				if(*ptr == 'x' || *ptr == 'X') {
					++ptr;
					base = 16;
				} else {
				        ++nptr;
					base = 8;
				       }
			} else
				base = 10;
		} else if(base == 16)
			if(ptr[0] == '0' && (ptr[1] == 'x' || ptr[1] == 'X'))
				ptr += 2;
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
          	 
		maxval = ULONG_MAX / (unsigned long)base;
		while(isalnum((int)*ptr) && (xx=DIGIT(*ptr)) < base) {
			if ((val > maxval) || (base*val + xx < val)) {
				val = ULONG_MAX;
				SETERR(ERANGE);
				break;
			}
			val = base*val + xx;
			++ptr;
			++digits;
		}
    	}
	if(signchar == 1)
	    val = -val;
	if(endptr != 0) 
	  if((digits == 0) && (signchar == 0)) 
	    *endptr = (char *) nptr;
	  else if(digits > 0)
	    *endptr = (char *) ptr;


	return(val);
}
