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
 * $Id: fpe_ops.c,v 1.4 1994/11/18 20:40:26 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_ops.c,v $
 * Revision 1.4  1994/11/18  20:40:26  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:20  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_ops.c    1.4"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"

/*
 * frcp_op()
 */
void
frcp_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    fp_t do_frcp();
    int num_shifted,overflow_occured;

    res->n_double = 0;
#if dbg3
printf("entering frcp_op\n");
#endif

    num_shifted = normalize(src2,s_prec);
    if (s_prec) {
      src2->b_double.exponent += 0x3ff;
      num_shifted += 0x3ff;
    }
    else {
      src2->b_single.exponent += 0x7f;
      num_shifted += 0x7f;
    }
    *res = do_frcp(*src2,s_prec,r_prec,fsr);
#if DEBUG
    if (fpe_debug) {
      printf("the result before shift_up is\n");
      print_i860type(res);
    }
#endif
    shift_up(res,num_shifted,r_prec,&overflow_occured);
    if(overflow_occured) 
        *fsr |= FSR_MO;
}

/*
 * frsqr_op()
 */
void
frsqr_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    fp_t do_frsqr();
    ulong num_shifted;
    int overflow_occured;

    res->n_double = 0;
    num_shifted = normalize(src2,s_prec);
    if (s_prec) {
      src2->b_double.exponent += 0x3ff;
      num_shifted += 0x3ff;
    }
    else {
      src2->b_single.exponent += 0x7f;
      num_shifted += 0x7f;
    }
    if(num_shifted % 2)   /* if odd number of left shifts the make it even because
                             rescaling will do half as many right shifts  */
    {
        if(s_prec)
            src2->b_double.exponent++;
        else
            src2->b_single.exponent++;
	num_shifted++;
    }
    *res = do_frsqr(*src2,s_prec,r_prec,fsr);
    shift_up(res,((ulong)(num_shifted) >> 1),r_prec,&overflow_occured);
    if(overflow_occured) 
        *fsr |= FSR_MO;
}


/*
 * fix_op()
 */
fix_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    int rounding_mode;

    res->n_double = 0;

    rounding_mode = (get_fsr() & FSR_RM) >> FSR_RM_SHIFT;
    switch(rounding_mode) {

      case RM_CHOP:
      case RM_TO_NEAREST:
        break;

      case RM_ROUND_DOWN:
        if(s_prec)
        {
            if(src1->b_double.sign)
            {
                res->n_double = (double)-1.0;
                *fsr |= FSR_AA;
            }    
        }
        else
            if(src1->b_single.sign)
            {
                if(r_prec)
                    res->n_double = (double)-1.0;
                else
                    res->n_single = (float)-1.0;
                *fsr |= FSR_AA;
            }
       break;

       case RM_ROUND_UP:
        if(s_prec)
        {
            if(!src1->b_double.sign)
            {
                res->n_double = (double)1.0;
                *fsr |= FSR_AA;
            }
        }
        else
            if(!src1->b_single.sign)
            {
                if(r_prec)
                    res->n_double = (double)1.0;
                else
                    res->n_single = (float)1.0;
                *fsr |= FSR_AA;
#if DEBUG
		if (fpe_debug) {
		  printf("the result is 1 \n");
		  print_i860type(res);
		}
#endif
            }
        break;
       
     }
}

void
ftrunc_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    res->n_double = 0;
}

void
famov_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    if (s_prec == r_prec)
	/* famov.ss or famov.dd */  
	*res = *src1;
    else if (!s_prec && r_prec) {
	/* famov.sd */
	famovsd_den(src1);
	*res = *src1;
    }
    else if (s_prec && !r_prec) {
	/* famov.ds */
        res->dlong = SinglePosSmallest;
	res->b_single.sign = src1->b_double.sign;
        res->b_single.exponent = 200;
	*fsr = FSR_AU | FSR_AI | FSR_AE;
    }
 }

void
fgt_op(src1,src2,res,s_prec,r_prec,fsr,psr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr,*psr;
{
    if(fp_gt(src1,src2,s_prec))
        *psr |= PSR_CC;
    else
        *psr &= ~PSR_CC;
}

void
fle_op(src1,src2,res,s_prec,r_prec,fsr,psr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr,*psr;
{
    fgt_op(src1,src2,res,s_prec,r_prec,fsr,psr);
}

void
feq_op(src1,src2,res,s_prec,r_prec,fsr,psr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr,*psr;
{
    if(s_prec) { 
        if((src1->dlong.hi == src2->dlong.hi) &&
           (src1->dlong.lo == src2->dlong.lo))
            *psr |= PSR_CC;
        else
            *psr &= ~PSR_CC;
    }
    else {
	if (src1->dlong.lo == src2->dlong.lo)
	    *psr |= PSR_CC;
	else
	    *psr &= ~PSR_CC;
    }
}

void
fbug_op(src1,src2,res,s_prec,r_prec,fsr)
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr;
{
    printf("fbug_op: unknown the source error\n");
}

fp_t 
do_frcp(src2,s_prec,r_prec,fsr)
fp_t src2;
int s_prec,r_prec;
ulong *fsr;
{
    fp_t result;
    result.n_double = 0;
    intr_disable();
    if(r_prec)
        if(s_prec)
            result.n_double = 1/src2.n_double;
        else
            result.n_double = 1/src2.n_single;
    else
        if(s_prec)
            result.n_single = 1/src2.n_double;
        else
            result.n_single = 1/src2.n_single;
    *fsr = get_fsr();
    intr_enable();
    return(result);
}

fp_t 
do_frsqr(src2,s_prec,r_prec,fsr)
fp_t src2;
int s_prec,r_prec;
ulong *fsr;
{
    fp_t result;
    result.n_double = 0;
    intr_disable();
    if(r_prec)
        if(s_prec)
            result.n_double = sqrtdd(src2.n_double);
        else
            result.n_double = sqrtsd(src2.n_single);
    else
        result.n_single = sqrtss(src2.n_single);
    *fsr = get_fsr();
    intr_enable();
    return(result);
}

/*
 * normalize() normalizes a denormal number, and returns number of
 * left shifts done
 */
int 
normalize(num,source_precision) 
fp_t *num;
int source_precision;
{
    int num_shifted,sign;

#if DEBUG
    if (fpe_debug) {
      printf("In normalize  num is\n");
      print_i860type(num);
    }
#endif
    num_shifted = 0;
    if(source_precision)
    {
        sign = num->b_double.sign;
        while ((!num->b_double.mantissa_a)&(num_shifted<52))
        {
            /* do a shift */
            do_a_l_shift_d(num);
            num_shifted ++;
        }
        do_a_l_shift_d(num);  /* now we have an implicit 1 preceding binary point */
        num->b_double.exponent = 1;
        num->b_double.sign = sign;
        num_shifted ++;
    }
    else
    {
        sign = num->b_single.sign;
        while ((!num->b_single.mantissa_a)&(num_shifted<23))
        {
            /* do a shift */
            num->dlong.lo = num->dlong.lo << 1;
            num_shifted ++;
        }
        num->dlong.lo = num->dlong.lo << 1;
        num->b_single.exponent = 1;
        num->b_single.sign = sign;
        num_shifted ++;
    }
#if DEBUG
    if (fpe_debug) {
      printf("In normalize normalized num is\n");
      print_i860type(num);
    }
#endif
    return(num_shifted);
}

shift_up(num,num_shifted,result_precision,overflow_occurs)
fp_t *num;
int result_precision,*overflow_occurs;
ulong num_shifted;
{
    ulong new_exponent; 
#if DEBUG
    if (fpe_debug) {
      printf("in shift up\n");
      print_i860type(num);
    }
#endif
    *overflow_occurs = 0;

    if(result_precision)
    {
        new_exponent = num->b_double.exponent +  num_shifted;
        if(new_exponent >= 0x7ff)
        {
            *overflow_occurs = 1;
            num->b_double.exponent = new_exponent & 0x7ff;
        }
        else
            num->b_double.exponent = new_exponent;
    }
    else
    {
        new_exponent = num->b_single.exponent +  num_shifted;
#if DEBUG
	if (fpe_debug)
	  printf("new exponent is %x\n",new_exponent);
#endif
        if(new_exponent >= 0xff)
        {
            *overflow_occurs = 1;
            num->b_single.exponent = new_exponent & 0xff;
        }
        else
            num->b_single.exponent = new_exponent;
    }
}
