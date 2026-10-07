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
 * $Id: fpe_fadd.c,v 1.4 1994/11/18 20:40:22 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_fadd.c,v $
 * Revision 1.4  1994/11/18  20:40:22  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:13  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_fadd.c    1.3"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"

void
fadd_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    int s1_deno, s2_deno;
    int flush_zero;

    flush_zero = (*fsr & FSR_FZ) ? 1 : 0; 

    if (!s_prec && r_prec) {
        /* xx.sd operation, convert inputs to double precision */
        to_double(src1);
	to_double(src2);
	s_prec = 1;
    }
    s1_deno = is_denormal(src1,s_prec);
    s2_deno = is_denormal(src2,s_prec);
    if (s1_deno && s2_deno) {
        if (r_prec)
	    da2den(src1,src2,res,fsr);
	else
	    sa2den(src1,src2,res,fsr);
    }
    else if (s1_deno || s2_deno) {
        if (r_prec)
	    da1den(src1,src2,res,fsr);
	else
	    sa1den(src1,src2,res,fsr);
    }
    else {
        if (r_prec)
	    danorm(src1,src2,res,fsr);
	else
	    sanorm(src1,src2,res,fsr);
    }
    /*
     * When FZ is set and underflow occurs, the result is set to zero.
     */
    if ((flush_zero) && (*fsr & FSR_AU)) {
        res->n_double = 0.0;
	*fsr &= ~FSR_AU;
    } 	
}

void
fsub_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    flip_sign(src2,s_prec);
    fadd_op(src1,src2,res,s_prec,r_prec,fsr);
}

sanorm( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/*
	 * Both operands are normal, compute result and return fsr
	 * Since there was no source exception, return FP_NOERR
	 */
	intr_disable();
	*fsr = faddss(s1, s2, d) & FSR_ABITS;
	intr_enable();
}

sa1den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * One operand is a denormal 
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.  
	 * If the 2 numbers are "close" (i.e, biased exponent of the normalized
	 * number is < 32 (arbitrary, for single precision)), we 
	 *	- normalize the denormal
	 *	- scale it up by 2**32 to bring it into operating range
	 *	- scale the normalized number by 2**32 
	 *	- do the addition
	 *	- scale it back by 2**-32
	 *
	 * Otherwise, we return the normalized number, with the inexact bit set.
	 */

	ulong	sign1, sign2;
	int	exp1, exp2;

	if (is_zero(s1,0)) {
		/* return the denormal */
		spassign(d, s2);
		*fsr = 0;		/* NO AE, AI, AO, AU */
		return;
	}
		
	if (is_zero(s2,0)) {
		/* return the denormal */
		spassign(d, s1);
		*fsr = 0;		/* NO AE, AI, AO, AU */
		return;
	}

	sign1 = spsign(s1);
	sign2 = spsign(s2);
	exp1 = spexponent(s1);
	exp2 = spexponent(s2);

	if (is_denormal(s1,0)) {
		if (exp2 >= 32) {
			spassign(s1, SPsmallest);
			if (sign1) spchgsign(s1);
			intr_disable();
			*fsr = faddss(s1, s2, d) & FSR_ABITS;
			intr_enable();
			return;
		}
		/* The numbers are "close" */
		spnormalize(s1);
		fmulss(s1, SPpow34, s1);
		fmulss(s2, SPpow32, s2);
	}
	if (is_denormal(s2,0)) {
		if (exp1 >= 32) {
			spassign(s2, SPsmallest);
			if (sign2) spchgsign(s2);
			intr_disable();
			*fsr = faddss(s1, s2, d) & FSR_ABITS;
			intr_enable();
			return;
		}
		spnormalize(s2);
		fmulss(s2, SPpow34, s2);
		fmulss(s1, SPpow32, s1);
	}
	intr_disable();
	*fsr = faddss(s1, s2, d) & FSR_ABITS;
	intr_enable();
	if (fmulss(d, SPpow_32, d) & FSR_MU) 	/* scale down */
		*fsr |= FSR_AU;
	
	if (spexponent(d)) *fsr|= FSR_AE;
}

sa2den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * Both operands are denormals.
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.   We
	 *	- normalize the denormals
	 *	- scale both up by 2**34 to bring it into operating range
	 *	- do the addition
	 *	- scale result down by 2**-32
	 */

	spnormalize(s1); 
	fmulss(s1, SPpow34, s1);
	spnormalize(s2); 
	fmulss(s2, SPpow34, s2);
	intr_disable();
	*fsr = (faddss(s1, s2, d) & FSR_ABITS);
	intr_enable();
	if (fmulss(d, SPpow_32, d) & FSR_MU) /* scale down */
		*fsr |= FSR_AU;
	
	if (spexponent(d)) *fsr |= FSR_AE;
}

danorm( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/*
	 * Both operands are normal, compute result and return fsr
	 * Since there was no source exception, return FP_NOERR
	 */
	intr_disable();
	*fsr = fadddd(s1, s2, d) & FSR_ABITS;
	intr_enable();
}

da1den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * One operand is a denormal, other is a zero or normal. 
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.  
	 * If the 2 numbers are "close" (i.e, biased exponent of the normalized
	 * number is < 64 (arbitrary, for double precision)), we 
	 *	- normalize the denormal
	 *	- scale it up by 2**64 to bring it into operating range
	 *	- scale the normalized number by 2**64 
	 *	- do the addition
	 *	- scale it back by 2**-64
	 *
	 * Otherwise, we return the normalized number, with the inexact bit set.
	 */

	ulong	sign1, sign2;
	int	exp1, exp2;

	if (is_zero(s1,1)) {
		/* return the denormal */
		dpassign(d, s2);
		*fsr = 0;		/* NO AE, AI, AO, AU */
		return;
	}
		
	if (is_zero(s2,1)) {
		/* return the denormal */
		dpassign(d, s1);
		*fsr = 0;		/* NO AE, AI, AO, AU */
		return;
	}

	sign1 = dpsign(s1);
	sign2 = dpsign(s2);
	exp1 = dpexponent(s1);
	exp2 = dpexponent(s2);

	if (is_denormal(s1,1)) {
		if (exp2 >= 64) {
			dpassign(s1, DPsmallest);
			if (sign1) dpchgsign(s1);
			intr_disable();
			*fsr = fadddd(s1, s2, d) & FSR_ABITS;
			intr_enable();
			return;
		}
		dpnormalize(s1);
		fmuldd(s1, DPpow66, s1);
		fmuldd(s2, DPpow64, s2);
	}
	if (is_denormal(s2,1)) {
		if (exp1 >= 64) {
			dpassign(s2, DPsmallest);
			if (sign2) dpchgsign(s2);
			*fsr = fadddd(s1, s2, d) & FSR_ABITS;
			return;
		}
		dpnormalize(s2);
		fmuldd(s2, DPpow66, s2);
		fmuldd(s1, DPpow64, s1);
	}
	intr_disable();
	*fsr = fadddd(s1, s2, d) & FSR_ABITS;
	intr_enable();
	if (fmuldd(d, DPpow_64, d) & FSR_MU)	/* scale down */
		*fsr |= FSR_AU;
}

da2den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * Both operands are denormals.
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.   We
	 *	- normalize the denormals
	 *	- scale both up by 2**64 to bring it into operating range
	 *	- do the addition
	 *	- scale result down by 2**-64
	 */

	dpnormalize(s1); fmuldd(s1, DPpow66, s1);
	dpnormalize(s2); fmuldd(s2, DPpow66, s2);
	intr_disable();
	*fsr = (fadddd(s1, s2, d) & FSR_ABITS);
	intr_enable();
#ifdef	DEBUG
	if (fpe_debug) {
		printf("%.08x%.08x, %.08x%.08x, %.08x%.08x\n", s1[1], s1[0],
			s2[1], s2[0], d[1], d[0]);
	}
#endif
	if (fmuldd(d, DPpow_64, d) & FSR_MU) /* scale down */
		*fsr |= FSR_AU;
#ifdef	DEBUG
	if (fpe_debug) {
		printf("After scale down: %.08x%.08x\n", d[1], d[0]);
	}
#endif
}

