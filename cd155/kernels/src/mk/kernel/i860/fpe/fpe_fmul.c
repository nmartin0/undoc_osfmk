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
 * $Id: fpe_fmul.c,v 1.4 1994/11/18 20:40:23 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_fmul.c,v $
 * Revision 1.4  1994/11/18  20:40:23  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:15  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_fmul.c    1.3"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"

void
fmul_op(src1,src2,res,s_prec,r_prec,fsr)
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
    if (is_zero(src1,s_prec) || is_zero(src2,s_prec)) { 
	if (r_prec)
	   dmzero(src1,src2,res,fsr);
        else
	   smzero(src1,src2,res,fsr); 
    }
    else { 
        s1_deno = is_denormal(src1,s_prec);
        s2_deno = is_denormal(src2,s_prec);
        if (s1_deno && s2_deno) {
            if (r_prec) 
                dm2den(src1,src2,res,fsr);
            else 
	        sm2den(src1,src2,res,fsr);
        }
        else if (s1_deno || s2_deno) {
	    if (r_prec)
	        dm1den(src1,src2,res,fsr);
	    else
                sm1den(src1,src2,res,fsr);
        } 
        else {
	    if (r_prec)
	        dmnorm(src1,src2,res,fsr);
	    else
                smnorm(src1,src2,res,fsr);
        }
    }
    /*
     * When FZ is set and underflow occurs, the result is set to zero.
     */
    if ((flush_zero) && (*fsr & FSR_MU)) {
        res->n_double = 0.0;
	*fsr &= ~FSR_MU;
    } 	
}   

sm2den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* Both operands are denormals
	 * 
	 * Denormal flag has already been set.
	 * 
	 * Product of 2 denormals results in a very tiny result, with
  	 * loss of precision.  We normalize, do the multiply, but set
	 * the underflow and inexact bits ourselves.
 	 * This number in the pipeline will later cause a result exception, 
	 * and we let the result exception handler take care of it.
	 */

	extern ulong SPpow4[];

	spnormalize(s1);
	spnormalize(s2);
	intr_disable();
	*fsr = (fmulss(s1, s2, d) & (FSR_MA | FSR_MI | FSR_SI));
	intr_enable();
	*fsr |= FSR_MU;
	fmulss(d, SPpow4, d);		/* Compensate for 2 denormals */
}

sm1den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * One operand is a denormal 
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.  
	 * Normalize the denormal, compute the answer and fsr.
	 * Note that normalizing a denormal returns an i860 number
	 * which is less by a factor of 4, which is why we multiply
	 * the result by SPpow2.
	 */

	extern ulong SPpow2[];

    	if (is_denormal(s1,0))
	     spnormalize(s1);
        else
             spnormalize(s2);

	/* Extract only the ofl, ufl, add-one and inexact flags */
	intr_disable();
	*fsr = fmulss(s1, s2, d) & FSR_MBITS;
	intr_enable();
	fmulss(d, SPpow2, d);	/* compensate for normalizing */

	/* 
	 * If an overflow happened in this normalizing mode,
	 * then the result is normal. If it did not happen, then 
	 * really, an underflow should have happened!
	 */
	if (*fsr & FSR_MO)  *fsr &= ~FSR_MO;
	else *fsr |= FSR_MU;
}
		
smnorm( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/*
	 * Both operands are normal, compute result and return fsr
	 * Since there was no source exception, return FP_NOERR
	 */
	intr_disable();
	*fsr = fmulss(s1, s2, d) & FSR_MBITS;
	intr_enable();
}


dm2den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* Both operands are denormals
	 * 
	 * Denormal flag has already been set.
	 * 
	 * Product of 2 denormals results in a very tiny result, with
  	 * loss of precision.  We normalize, do the multiply, but set
	 * the underflow and inexact bits ourselves.
 	 * This number in the pipeline will later cause a result exception, 
	 * and we let the result exception handler take care of it.
	 */

	extern ulong DPpow4[];

	dpnormalize(s1);
	dpnormalize(s2);
	intr_disable();
	*fsr = (fmuldd(s1, s2, d) & (FSR_MA | FSR_MI | FSR_SI));
	intr_enable();
	*fsr |= FSR_MU;
	fmuldd(d, DPpow4, d);	/* Compensate for 2 denormals */
}

dm1den( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* 
	 * One operand is a denormal 
	 *
	 * Denormal sticky flag has already been set
	 *
	 * Denormal exception is masked.  
	 * Normalize the denormal, compute the answer and fsr.
	 * Note that normalizing a denormal returns an i860 number
	 * which is less by a factor of 4, which is why we multiply
	 * the result by SPpow2.
	 */

	extern ulong DPpow2[];

	if (is_denormal(s1,1)) 
	    dpnormalize(s1);
	else
	    dpnormalize(s2);	

	/* Extract only the ofl, ufl, add-one and inexact flags */
	intr_disable();
	*fsr = fmuldd(s1, s2, d) & FSR_MBITS;
	intr_enable();
	fmuldd(d, DPpow2, d);	/* compensate for normalizing */

	/* 
	 * If an overflow happened in this normalizing mode,
	 * then the result is normal. If it did not happen, then 
	 * really, an underflow should have happened!
	 */
	if (*fsr & FSR_MO)  *fsr &= ~FSR_MO;
	else *fsr |= FSR_MU;
}

dmnorm( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/*
	 * Both operands are normal, compute result and return fsr
	 * Since there was no source exception, return FP_NOERR
	 */
	intr_disable();
	*fsr = fmuldd(s1, s2, d) & FSR_MBITS;
	intr_enable();
}

smzero( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* Result is a zero because one or both inputs is zero */
	d[0] = spsign(s1) ^ spsign(s2);	/* make sure sign is correct */
	*fsr = 0;	/* no MA, MI, MO, MU */
}

dmzero( s1, s2, d, fsr)
ulong	*s1, *s2, *d, *fsr;
{
	/* Result is a zero because one or both inputs is zero */
	d[1] = dpsign(s1) ^ dpsign(s2);	/* make sure sign is correct */
	d[0] = 0;
	*fsr = 0;	/* no MA, MI, MO, MU */
}


