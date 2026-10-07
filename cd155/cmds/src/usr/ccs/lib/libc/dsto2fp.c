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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.1
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: dsto2fp.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:29 $";
#endif
/*
 * COMPONENT_NAME: LIBCCNV _dsto2fp
 *
 * FUNCTIONS: _dsto2fp
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
 * dsto2fp.c	1.17  R2/lib/c/cnv,3.1,9021 5/15/90 16:57:16";
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

static int checknf( const char *bp );
static int checknan( const char *bp );

/*
 * NAME: _dsto2fp
 *                                                                    
 * FUNCTION: convert decimal string to 2 fp numbers (hi part & low part)
 *                                                                    
 * NOTES:
 *
 * _dsto2fp - convert decimal string to 2 fp numbers (hi part & low part)
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

char *_dsto2fp (p, dbl, expon, class)
char   *p;
double dbl[2];
int    *expon;
int    *class;

{
	int    strncmp();
	char   *s = p;        /* current character pointer                  */
	char   *Eptr;         /* start of exponent string                   */
	char   *radix;        /* radix character from locale info           */
	char   c;             /* current character holder                   */
	double d, h, l, t;    /* temps used to calculate the double results */
	double ten = 10.0;    /* constant 10.0                              */
	int    expsign = 1;   /* sign of exponent (+1 = '+'; -1 = '-')      */
	int    ct = 0;        /* number of significant digits collected     */
	unsigned long exp = 0; /* holds the converted exponent               */
	int    nanret, ret = 0;

	Eptr = (void *) NULL;	/* Error indicator for exponent problems */
	*expon = 0;           /* start out with exponent = 0                */
	*class = FP_PLUS_NORM; /* assume number will be normalized fp       */

	h = 0.0;              /* init the high & low part of result to +0.0 */
	l = 0.0;

	c = *s++;             /* get the first character */
	while (isdigit(c))    /* while in front of the decimal point */
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
	}
	/* Here on radix character, e, E, Q, S, I, or end of string */
	/* If radix character, get the stuff that's after it.       */
	/* If radix character is NULL, set to "." per X/Open.       */
	radix = nl_langinfo(RADIXCHAR);
	if ((radix == 0) || !(*radix))
		radix = ".";
	if ( c == *radix ) {
		c = *s++;
		while ( isdigit(c) ) {
			/* Multiply the previous value by 10 and add 
			 * in the new digit  */
			if (ct < 30) {
				ACCUM;
				(*expon)--;    /* decrement the exponent   */
			}
			if ((c != '0') || ct ) 
				ct++;
			c = *s++;
		}  /* end while */
	}  /* end if */
	/* Get the exponent if there is one */
	if ( ((c == 'e') || (c == 'E')) && ( p != s-1)) {
		Eptr = s-1;      /* mark string start in case exp is invalid */
		switch (c = *s++) {
		case '-': 
			expsign = -1;
		case '+': 
			c = *s++;
		}

/*
 * accumulate exponent.  We can possibly get a series of digits
 * which would cause a long to overflow, so we want to stop
 * accumulating when the value gets to be larger than anything
 * that won't cause an overflow in the floating point number
 */
		
		while ( isdigit(c) ) {
		  	if (exp < (DBL_MAX_EXP*2))	/* plenty of safety */
				exp = (exp*10) + (c & 0xf);
			c = *s++;
			Eptr = s-1;		/* Advance error indicator */
		}
	}

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
			else if  (nanret == 3) {	/* NaN */
			  	h = DBL_SNAN;
				*class = FP_SNAN;
				s +=3;
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

	/*
	 * Avoid comparing h if known to be nan	(avoid mmax hw trap)
	 * otherwise if hi==0 then whole nbr == 0
	 */
	if ( *class == FP_PLUS_NORM && h == 0.0 )
		*class = FP_PLUS_ZERO;


	dbl[0] = h;           /* Result = h and l   */
	dbl[1] = l;           /* Return in memory */

	if (Eptr == (void *) NULL)
	  return (s-1);		/* Return ptr to char that ended scan */
	else
	  return (Eptr);	/* Return ptr from bad convert of exponent */
}

/*
 * NAME: checknf
 *                                                                    
 * FUNCTION: check to see in string is infinity
 *                                                                    
 * NOTES:
 *	The check for infinity is case insensitive
 *
 * RETURNS: 3 for "inf" string;
 *	    8 for "infinity" string;
 *	    0 otherwise.
 *
 */

static int
checknf(const char *bp)
{
	bp++;
	if ((*bp == 'n') || (*bp == 'N'))
	{   bp++;
	    if ((*bp == 'f') || (*bp == 'F'))
	    {   
	    /* if here, we have inf.  Scan some more to check for infinity. */
		if (
			((*++bp == 'i') || (*bp == 'I')) &&
			((*++bp == 'n') || (*bp == 'N')) &&
			((*++bp == 'i') || (*bp == 'I')) &&
			((*++bp == 't') || (*bp == 'T')) &&
			((*++bp == 'y') || (*bp == 'Y'))
	       	    )
			return (8);
		else 	return (3);
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
 * RETURNS: 1 for NaNQ string;
 *	    2 for NaNS string;
 *	    3 for NaN  string;
 *          0 otherwise
 *
 */
static int
checknan(const char *bp)
{
	bp++;
	if ((*bp == 'a') || (*bp == 'A'))
	{	bp++;
		if ((*bp == 'n') || (*bp == 'N'))
		{	bp++;
		/* if here we have a NaN; now just classify it */
			if ((*bp == 'q') || (*bp == 'Q')) {
				return (1);		/* NaNQ */
			}
			else if ((*bp == 's') || (*bp == 'S')) {
				return (2);		/* explicit NaNS */
			}
			else return (3);		/* implicit NaNS */
		}
	}

	return(0);
}
