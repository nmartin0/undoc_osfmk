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
 * OSF/1 Release 1.0.3
 */
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: gcvt.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:05:21 $";
#endif
/*
 * FUNCTIONS: gcvt
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
 * gcvt.c	1.9  com/lib/c/cnv,3.1,8943 10/27/89 16:14:03
 */

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <fp.h>
#include <nl_types.h>
#include <langinfo.h>
#include "print.h"

/* TRIMZERO causes trailing zeros to be removed.  This is
 * required for bsd compatability.  The #ifdef around this
 * code has been around since sccs level 1.1, so it came
 * with the code
 */

#define TRIMZERO

/*
 * NAME: gcvt
 *                                                                    
 * FUNCTION: convert double to readable ascii string
 *                                                                    
 * NOTES:
 *
 * RETURNS: a character string
 *
 */

char *
gcvt(number, ndigit, buf)
double	number;
int	ndigit;
char	*buf;
{
	int sign, decpt;
	register char *p1, *p2 = buf;
	register int i;
	char	*radix;
	int	radixlen;
#ifdef _THREAD_SAFE
	char	lbuf[NDIG];

	p1 = lbuf;
	ecvt_r(number, ndigit, &decpt, &sign, lbuf, NDIG);
#else
	p1 = ecvt(number, ndigit, &decpt, &sign);
#endif
	if (sign)
		*p2++ = '-';
	/* If string returned from ecvt is not numeric then it should	*/
	/* 		be "NaNQ", "NaNS", or "INF"			*/
	if( !FINITE(number) ) {
		strcpy(p2,p1);
		return(buf);
	}

	radix = nl_langinfo(RADIXCHAR);
	if ( !radix || !(*radix))
	  	radix = ".";	/* Use "." if no other radix-point preferred */
	radixlen = strlen(radix);

	if (decpt > ndigit || decpt <= -4) {	/* E-style */
		decpt--;
		*p2++ = *p1++;

		strcpy(p2,radix);		/* insert "decimal point" */
		p2 += radixlen;

		for (i = 1; i < ndigit; i++)
			*p2++ = *p1++;
#ifdef TRIMZERO
		while (p2[-1] == '0')
			p2--;
		if (!strcmp( &p2[-radixlen], radix ))
		    	p2 -= radixlen;

#endif
		*p2++ = 'e';
		if (decpt < 0) {
			decpt = -decpt;
			*p2++ = '-';
		} else
			*p2++ = '+';
		for (i = 1000; i != 0; i /= 10) /* 3B or CRAY, for example */
			if (i <= decpt || i <= 10) /* force 2 digits */
				*p2++ = (decpt / i) % 10 + '0';
	} else {
		if (decpt <= 0) {
			*p2++ = '0';
			strcpy(p2,radix);
			p2 += radixlen;
			while (decpt < 0) {
				decpt++;
				*p2++ = '0';
			}
		}
		for (i = 1; i <= ndigit; i++) {
			*p2++ = *p1++;
			if (i == decpt) {
			  	strcpy(p2, radix);
				p2 += radixlen;
			}
		}
		if (ndigit < decpt) {
			while (ndigit++ < decpt)
				*p2++ = '0';
			strcpy(p2, radix);
			p2 += radixlen;
		}
#ifdef TRIMZERO
		while (*--p2 == '0' && p2 > buf) /* back over trailing zeros */
			;			 /*  til anything else */
		if (!strcmp(&p2[1-radixlen], radix))	/* if it was the "radix-point" */
		  	p2 -= (radixlen-1);		/*  back up to just after last digit */
#endif
	}
	*p2 = '\0';
	return (buf);
}

