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
static char rcsid[] = "@(#)$RCSfile: wdsto2fp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:09:42 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV _wdsto2fp
 *
 * FUNCTIONS: _wdsto2fp
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
 * dsto2fp.c	1.10  R2/lib/c/cnv,3.1,8943 10/11/89 15:23:53
 */

#include <float.h>
#include <ctype.h>
#include <math.h>
#include <machine/endian.h>
#include <nl_types.h>
#include <langinfo.h>

/*
 *      Macro to multiply the current pair of fp numbers by 10.0 and
 *      add the new character.
 */
#define ACCUM d = ch[c & 0xf];	\
	t = ten * h + d;	\
	h = (ten * h - t) + d;	\
	l = ten * l + h;	\
	h = t

static double const ch[10] = {
	0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

static int checknf( const NLchar *bp );
static int checknan( const NLchar *bp );

/*
 * NAME: _wdsto2fp
 *                                                                    
 * FUNCTION: convert decimal string to 2 fp numbers (hi part & low part)
 *                                                                    
 * NOTES:
 *
 * _wdsto2fp - convert decimal string to 2 fp numbers (hi part & low part)
 *
 *      Called with p -     pntr to ascii string
 *                  dbl -   pntr to array 2 doubles.
 *                  expon - pntr to the exponent returned.
 *                  class - pointer to result class
 *                          The class value is the same as returned by
 *                          the class() function except norms and denorms
 *                          both return FP_PLUS_NORM.
 *
 *      Returns pointer to character that terminates scan.
 *
 *      A valid decimal string has the following form:
 *
 *      ( {<digit>} [.{<digit>}] [(e|E) [+|-|<sp>] {<digit>} ] | "NaNS" |
 *      "NaNQ" | "INF" )
 *
 *      {} = 0 or more times
 *      [] = 0 or 1 times
 *
 *
 * RETURNS: 
 *      Returns pointer to character that terminates scan.
 */
#ifdef KJI

NLchar *_wdsto2fp (p, dbl, expon, class)
NLchar   *p;
double dbl[2];
int    *expon;
int    *class;

{
	int    strncmp();
	NLchar   *s = p;        /* current character pointer                  */
	NLchar   *Eptr;         /* start of exponent string                   */
	char     *radix;        /* radix character from locale info           */
	NLchar   c;             /* current character holder                   */
	double d, h, l, t;    /* temps used to calculate the double results */
	double ten = 10.0;    /* constant 10.0                              */
	int    expsign = 1;   /* sign of exponent (+1 = '+'; -1 = '-')      */
	int    ct = 0;        /* number of significant digits collected     */
	int    expct = 0;     /* number of significant digits in exponent   */
	unsigned int exp = 0; /* holds the converted exponent               */
	int    nanret, ret = 0;

	*expon = 0;           /* start out with exponent = 0                */
	*class = FP_PLUS_NORM; /* assume number will be normalized fp       */
	h = 0.0;              /* init the high & low part of result to +0.0 */
	l = 0.0;

	c = *s++;             /* get the first character */
	c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
	while (c < 0x7f && isdigit(c))
	{
		/* Multiply the previous value by 10 and add in the new digit */
		if (ct < 30) { 
			ACCUM; 
		} 
		else 
			(*expon)++;
		if ((c != '0') || ct ) 
			ct++;
		c = *s++;
	        c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
	}
	/* Here on radix character, e, E, Q, S, I, or end of string */
	/* If radix character, get the stuff that's after it.       */
	/* If radix character is NULL, set to "." per X/Open.       */
	radix = nl_langinfo(RADIXCHAR);
	if ((radix == 0) || !(*radix))
		radix = ".";
        
	if ( c == (NLchar)*radix ) {
		c = *s++;
	        c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
	        while (c < 0x7f && isdigit(c)){
			/* Multiply the previous value by 10 and add 
			 * in the new digit  */
			if (ct < 30) {
				ACCUM;
				(*expon)--;    /* decrement the exponent   */
			}
			if ((c != '0') || ct ) 
				ct++;
			c = *s++;
	                c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
		}  /* end while */
	}  /* end if */
	/* Get the exponent if there is one */
	if ( ((c == 'e') || (c == 'E')) && ( p != s-1)) {
		Eptr = s-1;      /* mark string start in case exp is invalid */
                c = *s++;
                c = isascii(c)? c: _jistoa(c);
		switch (c) {
		case '-': 
			expsign = -1;
		case '+': 
			c = *s++;
                        c = isascii(c)? c: jistoa(c);
		}
	        c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
          	while (c < 0x7f && isdigit(c)){
			exp = (exp*10) + (c & 0xf);
			c = *s++;
	                c = NCisdigit(c)? (isascii(c)? c: jistoa(c)):jistoa(c);
#ifdef JUNK
			if (exp > 0) {		/* Skip leading zeros */
				expct++;
				if (expct > 3)
					break;
			}
#endif
		}
	}

  /* If there are more than 3 significant digits in the exponent, an overflow
   * or an underflow will occur.  Therefore, we set exp to be 500
   * and return that value so that the routines that call this one 
   * can take the appropriate action.
   */

	if (expct > 3)
		exp = 500;

	*expon += exp*expsign;

	/* If we are still at 1st char, look for the IEEE special strings. */
	if ( (s-1) == p) {
		switch (c) {
		case 'n': 
		case 'N': 
			nanret = checknan(p);
			if (nanret == 1) {		/* NaNQ */
				h = DBL_QNAN;
				*class = FP_QNAN;
				s +=4; 
			}
			else if (nanret == 2) {		/* NaNS */
				h = DBL_SNAN;
				*class = FP_SNAN;
				s +=4; 
			}
			break;
		case 'i': 
		case 'I': 
			if ((ret = checknf(p)) != 0) {
				h = DBL_INFINITY;
				*class = FP_PLUS_INF;
				s += ret; 
			}
			break;
		}
	}

	if (h == 0.0) *class = FP_PLUS_ZERO;  /* if hi==0 then whole nbr == 0 */

#if BYTE_ORDER == LITTLE_ENDIAN
	dbl[0] = h;           /* Result = h and l   */
	dbl[1] = l;           /* Return in memory */
#else	/* BYTE_ORDER == LITTLE_ENDIAN */
	dbl[1] = h;           /* Result = h and l   */
	dbl[0] = l;           /* Return in memory */
#endif	/* BYTE_ORDER == LITTLE_ENDIAN */

	return (s-1);         /* Return value is ptr to char that ended scan */
}

/*
 * NAME: checknf
 *                                                                    
 * FUNCTION: check to see in string is infinity
 *                                                                    
 * NOTES:
 *	The check for infinity is case insensitive
 *
 * RETURNS: 1 for infinity string; 0 otherwise
 *
 */

static int
checknf(const NLchar *bp)
{
register NLchar c;
	*bp++;
        c = isascii(*bp)? *bp: _jistoa(*bp);
	if ((c == 'n') || (c == 'N'))
	{   *bp++;
            c = isascii(*bp)? *bp: _jistoa(*bp);
	    if ((c == 'f') || (c == 'F'))
	    {   
		return (1);
	    }
	}

	return (0);
}

/*
 * NAME: checknan
 *                                                                    
 * FUNCTION: check to see in string is NaNQ or NaNS
 *                                                                    
 * NOTES:
 *	The check for NaN is case insensitive
 *
 * RETURNS: 1 for NaNQ string; 2 for NaNS or NaN strings; 0 otherwise
 *
 */
static int
checknan(const NLchar *bp)
{
register NLchar c;
	*bp++;
        c = isascii(*bp)? *bp: _jistoa(*bp);
	if ((c == 'a') || (c == 'A'))
	{	*bp++;
                c = isascii(*bp)? *bp: _jistoa(*bp);
		if ((c == 'n') || (c == 'N'))
		{	*bp++;
                        c = isascii(*bp)? *bp: _jistoa(*bp);
			if ((c == 'q') || (c == 'Q')) {
				/* NanQ */
				return (1);
			}
			/* anything else is treated as a NaNS */
			return (2);
		}
	}

	return(0);
}
#endif /* end of KJI */ 
