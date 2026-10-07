/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*      Copyright (c) 1989,1990 Intel Corporation.         */
/*      All rights reserved.                               */
/*                                                         */
/*        INTEL CORPORATION PROPRIETARY INFORMATION        */
/*                                                         */
/* This software is supplied under the terms of a license  */
/* agreement or nondisclosure agreement with Intel Corp.   */
/* and may not be copied or disclosed except in accordance */
/* with the terms of that agreement.                       */

/*
 * $Id: fpe_deno.c,v 1.4 1994/11/18 20:40:21 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_deno.c,v $
 * Revision 1.4  1994/11/18  20:40:21  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:11  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_deno.c    1.3"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"

/*
 * Normalizing a denormal number.
 *
 * Here we take a denormal number, and shift the mantissa left until
 * the integer bit is a 1.  Reduce the exponent by 1 each time you shift
 * left.  Note that the exponent starts with the value -1, whereas the
 * exponent implied by the IEEE format is really +1, so we are always
 * off by a factor of 2**2.
 *
 */
spnormalize(s)
ulong	*s;
{
	/*
	 * Create a new single precision number whose exponent is reduced by 4
	 * Then create a new single precision number whose exponent is the same
	 * as that of the above number, whose mantissa is 0.
	 * Subtract the two numbers. We get rid of the hidden integer bit
	 * and the i860 normalizes.  Raise the resulting exponent by 2,
	 * resulting in a normalized number which is off by a factor of 2**2.
	 */
	int	exp;
	ulong	bignum[2];

	s[0] |= 0x7e800000; 		/* set biased exponent to -4 */
	bignum[0] = s[0] & 0xff800000;	/* same sign & exp, mantissa = 0 */
	bignum[0] ^= 0x80000000;	/* opposite sign as input */
	faddss(s, bignum, s);		/* normalize */
	exp = (s[0] | 0x80000000) >> 23;/* extract signed exponent */
	exp += 2;
	s[0] &= 0x807fffff;		/* get rid of exponent */
	s[0] |= (exp & 0xff) << 23;	/* new exponent */
}

dpnormalize(s)
ulong	*s;
{
	int	exp;
	ulong	bignum[2];

	s[1] |= 0x7fd00000;		/* reduced biased exponent by -4 */
	bignum[1] = s[1] & 0xfff00000;
	bignum[0] = 0;			/* same sign & exp, mantissa = 0 */
	bignum[1] ^= 0x80000000;	/* opposite sign as input */
	fadddd(s, bignum, s);		/* normalize */
	exp = (s[1] | 0x80000000) >> 20;/* extract signed exponent */
	exp += 2;			/* now off by factor of 2**2 */
	s[1] &= 0x800fffff;		/* get rid of exponent */
	s[1] |= (exp & 0x7ff) << 20;	/* put in new exponent. */
}

/*
 * Given a result which has underflow, convert it to a denormalized number.
 * I must use addone and inexact flags, but how?
 */
spdenormalize(s, ae)
ulong *s;
int	ae;
{
	int	exp;
	ulong	bignumber[2];
	ulong	fsr;
	int 	oldr;

	/*
	 * The logic is very simple:
	 *
	 * Treat the underflowed exponent as a negative number (or 0 if it
	 * really is 0.) Subtract 4 from this exponent.
	 *
	 * Build a number with the exponent 0x7e8, and the sign of the
	 * input.  The idea here is as follows:
	 *
	 *	The original number should be denormalized by raising its
	 *	exponent to 001, thus bringing out the hidden integer bit.
	 *	Then you set the exponent to 0.
	 *
	 *	However, we want to use the fadd.ss instruction. We generate
	 *	a number with the exponent 0x7e8 (or, what is the same, -3)
	 *	Since we subtracted 4 from the input, the input exponent is
	 *	now -4 or lower.  Raising the input exponent from its original
	 *	value (0 or lower) to 0x001 is the same as raising the new 
	 *	exponent from -4 or lower to  -3.  The resulting result
	 *	is the correctly rounded denormal, and we just set its
	 *	exponent to 0.
	 */

	exp = (s[0] & 0x7f800000) >> 3; 	/* Make room for 3 more exp */
	exp |= (ae << 28);
	if (exp) { exp |= 0x80000000; exp >>= 20; }	/* sign extend */
	exp -= 4;				/* reduce exp by 4 */
	if (exp < -128) exp = -128;
	s[0] &= 0x807fffff;			/* zero out old exponent */
	s[0] |= ((exp << 23) & 0x7f800000);	/* create new exponent */
	bignumber[0] = spsign(s) | 0x7e800000;	/* sp number with exp 0x7e8 */
	intr_disable();
	fsr = faddss(bignumber, s, s);		/* raise input exponent */
	intr_enable();
	/*
	 * When raising the input exponent to 0x7e8, the addition may cause
	 * an overflow.. In this case, the number becomes normalized.
	 */
	exp = spexponent(s) + 3;		/* bring it back to 0 */
	s[0] &= 0x807fffff;			/* zero out exponent */
	s[0] |= ((exp & 0x0ff) << 23);
	return fsr & (FSR_AA | FSR_AI);
}


/*
 * Convert an underflowed double precision result to a denormal number.
 * I must use addone and inexact flags, but how?
 */
 
dpdenormalize(s)
ulong	*s;
{
	ulong	exp;
	ulong	bignumber[2];
	ulong	fsr;
	int	oldr;

	/*
	 * The logic is very simple:
	 *
	 * Treat the underflowed exponent as a negative number (or 0 if it
	 * really is 0.) Subtract 4 from this exponent.
	 *
	 * Build a number with the exponent 0x7fd, and the sign of the
	 * input.  The idea here is as follows:
	 *
	 *	The original number should be denormalized by raising its
	 *	exponent to 001, thus bringing out the hidden integer bit.
	 *	Then you set the exponent to 0.
	 *
	 *	However, we want to use the fadd.dd instruction. We generate
	 *	a number with the exponent 0x7fd (or, what is the same, -3)
	 *	Since we subtracted 4 from the input, the input exponent is
	 *	now -4 or lower.  Raising the input exponent from its original
	 *	value (0 or lower) to 0x001 is the same as raising the new 
	 *	exponent from -4 or lower to  -3.  The resulting result
	 *	is the correctly rounded denormal, and we just set its
	 *	exponent to 0.
	 */

	exp = s[1] & 0x7ff00000;
	if (exp) 
		exp = (exp | 0x80000000) >> 20;
	exp -= 4;
	s[1] &= 0x800fffff;			/* zero out old exponent */
	s[1] |= ((exp << 20) & 0x7ff00000);	/* create new exponent */
	bignumber[1] = dpsign(s) | 0x7fd00000;	/* dp number with exp 0x7fd */
	bignumber[0] = 0;
	intr_disable();
	fsr = fadddd(bignumber, s, s);		/* raise input exponent */
	intr_enable();
	/*
	 * When raising the input exponent to 0x7fd, the addition may cause
	 * an overflow.. In this case, the number becomes normalized.
	 */
	exp = (s[1] >> 20) + 3;			/* bring it back to 0 */
	s[1] &= 0x800fffff;			/* zero out exponent */
	s[1] |= (exp & 0x7ff) << 20;
	return fsr & (FSR_AA | FSR_AI);
}

/* 
 * Move a denormal single precision number to double precision format.
 * Note that the result becomes normalized since the exponent range of the
 * double precision number is much bigger.
 */

famovsd_den(s)
ulong	*s;
{
	int	exp;
	
	/*
	 * The algorithm is:
	 *
	 * First we normalize the number.  This results in a normalized number
	 * which is less than the original by a factor of 2**2.  Then we
	 * raise the exponent by (0x3ff - 0x7f + 2), which is the
	 * difference between the sp and dp bias + 2 to account for the above
	 * factor.
	 */

	spnormalize(s);
 	exp = (s[0] | 0x80000000);		/* add sign bit */
 	exp  >>=  23;				/* sign extended. exponent */
 	exp += (0x3ff - 0x7f + 2);		/* biased dp. exponent */
  	s[1] = spsign(s) | ((exp & 0x7ff) << 20) | ((s[0] & 0x007fffff) >> 3);
  	s[0] <<= 29;
}

