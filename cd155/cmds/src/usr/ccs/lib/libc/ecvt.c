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
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: ecvt.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:04:30 $";
#endif
/*
 * FUNCTIONS: ecvt, fcvt, cvt
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
 * ecvt.c	1.9  com/lib/c/cnv,3.1,8943 10/27/89 16:12:54
 */

#include <math.h>
#include <fp.h>
#ifdef MAXFLOAT
#undef MAXFLOAT
#endif
#include <values.h>
#ifdef _THREAD_SAFE
#include <errno.h>
#endif
#include "print.h"

#define	NMAX	((DSIGNIF * 3 + 19)/10) /* restrict max precision */

/*
 * NAME: ecvt
 *                                                                    
 * FUNCTION: convert double to character string
 *                                                                    
 * NOTES:
 *
 *	ecvt converts to decimal
 *	the number of digits is specified by ndigit
 *	decpt is set to the position of the decimal point
 *	sign is set to 0 for positive, 1 for negative
 *
 * RETURNS: a character string
 */

#ifndef _THREAD_SAFE
static char *cvt();

char *
ecvt(value, ndigit, decpt, sign)
double	value;
int	ndigit, *decpt, *sign;
{
	return (cvt(value, ndigit, decpt, sign, 0));
}

#else /* _THREAD_SAFE */
int
ecvt_r(double value, int ndigit, int decpt, int sign, char *buf, int len)
{
	return(cvt_r(value, ndigit, decpt, sign, 0, buf, len));
}
#endif /* _THREAD_SAFE */

/*
 * NAME: fcvt
 *                                                                    
 * FUNCTION: convert double to character string if FORTRAN format
 *                                                                    
 * NOTES:
 *
 * RETURNS: a character string
 *
 */

#ifndef _THREAD_SAFE
char *
fcvt(value, ndigit, decpt, sign)
double	value;
int	ndigit, *decpt, *sign;
{
	return (cvt(value, ndigit, decpt, sign, 1));
}
#else /* _THREAD_SAFE */
int
fcvt_r(double value, int ndigit, int decpt, int sign, char *buf, int len)
{
	return(cvt_r(value, ndigit, decpt, sign, 1, buf, len));
}
#endif

/*
 * NAME: cvt
 *                                                                    
 * FUNCTION: service routine for both ecvt and fcvt
 *                                                                    
 * RETURNS: a character string
 *                                                                    
 */

#ifdef _THREAD_SAFE
#define	RETURN(s)	{ strcpy(buf, s); return(0); }
#else
#define RETURN(s)	return(s)

static char buf[NDIG];
#endif

#ifdef _THREAD_SAFE
int
cvt_r(register double value, int ndigit, register int *decpt, int *sign,
						int f_flag, char *buf, int len)
#else
static char *
cvt(value, ndigit, decpt, sign, f_flag)
register double value;
int	ndigit, *sign, f_flag;
register int	*decpt;
#endif
{
	register char *p;
	register char *p_last;
	double valuem = value;       /* in memory for VALH to work */

#ifdef _THREAD_SAFE
	if ((buf == NULL) || (len < 1)) {
		seterrno(EINVAL);
		return(-1);
	}
#endif
	p = &buf[0];
	p_last = &buf[ndigit];
	buf[0] = '\0';
	*decpt = 0;
	*sign = VALH(valuem) >> 31;

	if( IS_QNAN(valuem) )
		RETURN("NaNQ");
	if( IS_NAN(valuem) )
		RETURN("NaNS");
	if( IS_INF(valuem) )
		RETURN("INF");
	if( IS_ZERO(valuem) ) {
		for ( ; p < p_last ; p++ )
			*p = '0';	
		*p = '\0';
#ifdef _THREAD_SAFE
		return(0);
#else
		return(buf);
#endif
	}

	VALH(valuem) &= 0x7fffffff;
	value = valuem;
	{
		/* rescale to range [1.0, 10.0) */
		/* in binary for speed and to minimize error build-up */
		/* even for the IEEE standard with its high exponents,
		   it's probably better for speed to just loop on them */
		static struct s { double p10; int n; } s[] = {
			1e32,	32,
			1e16,	16,
			1e8,	8,
			1e4,	4,
			1e2,	2,
			1e1,	1,
		};
		register struct s *sp = s;

		++*decpt;
		if (value >= 2.0 * DMAXPOWTWO) /* can't be precisely integral */
			do {
				for ( ; value >= sp->p10; *decpt += sp->n)
					value /= sp->p10;
			} while ((sp++)->n > 1);
		else if (value >= 10.0) { /* convert integer part separately */
			register double pow10 = 10.0, powtemp;

			while ((powtemp = 10.0 * pow10) <= value)
				pow10 = powtemp;
			for ( ; ; pow10 /= 10.0) {
				register int digit = value/pow10;
				*p++ = digit + '0';
				value -= digit * pow10;
				++*decpt;
				if (pow10 <= 10.0)
					break;
			}
		} else if (value < 1.0)
			do {
				for ( ; value * sp->p10 < 10.0; *decpt -= sp->n)
					value *= sp->p10;
			} while ((sp++)->n > 1);
	}
	if (f_flag)
		p_last += *decpt;
	if (p_last >= buf) {
		if (p_last > &buf[NDIG - 2])
			p_last = &buf[NDIG - 2];
		for ( ; ; ++p) {
			if (value == 0 || p >= &buf[NMAX])
				*p = '0';
			else {
				register int intx; /* intx in [0, 9] */
				*p = (intx = (int)value) + '0';
				value = 10.0 * (value - (double)intx);
			}
			if (p >= p_last) {
				p = p_last;
				break;
			}
		}
		if (*p >= '5') /* check rounding in last place + 1 */
			do {
				if (p == buf) { /* rollover from 99999... */
					buf[0] = '1'; /* later digits are 0 */
					++*decpt;
					if (f_flag)
						++p_last;
					break;
				}
				*p = '0';
			} while (++*--p > '9'); /* propagate carries left */
		*p_last = '\0';
	}
#ifdef _THREAD_SAFE
	return(0);
#else
	return (buf);
#endif
}

