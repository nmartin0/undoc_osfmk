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
 * $Id: fpe_result.c,v 1.5 1994/11/18 20:40:28 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_result.c,v $
 * Revision 1.5  1994/11/18  20:40:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/10/19  01:41:23  lenb
 *         add RPMSOFT_STAT_INC() for adder or multipler overflow, underflow,
 *         inexact (10432)
 *
 * Revision 1.3  1993/06/30  22:34:24  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_result.c    1.3"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"
#include <i860paragon/rpm.h>

/*
 *  handle_overflow()
 *  computes the IEEE standard result and set it into the 
 *  destination register or the last stage of the pipe.
 */
handle_overflow(FPptr)
fp_frame_t *FPptr;
{
   int dest;
   fp_t result;
   int r_prec;

#if DEBUG
   if (fpe_debug) 
     printf("Entering handle_overflow\n");
#endif
   dest = (FPptr->fsrs[2] & FSR_RR) >> FSR_RR_SHIFT;
   if(FPptr->fsrs[2] & FSR_MO)
   {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_mo);
        /* the last stage contains the erroneous result in both
           scalar and pipelined operations                      */
        r_prec = (FPptr->fsrs[2] & FSR_MRP) ? 1 : 0;
        result = FPptr->mres[2];
        FPstatus |= IEEE_M_OFLW;
        if(!(FPstatus & IEEE_X_OFLW))
	    bias_fr_oflw(&FPptr->mres[2],&FPptr->fp1,r_prec,FPptr->fsrs[2],0);
	else {
	    FPptr->fsrs[2] |= FSR_MI;
	    round_overflow(&result,r_prec,FPptr->fsrs[2]);
            if (dest != 0)
            	set_fpreg(FPptr,dest,&result,r_prec);
            FPptr->mres[2] = result;
	    /* clear M unit overflow error status */
	    FPptr->fsrs[2] &= ~FSR_MO;
	}  
   }

   if(FPptr->fsrs[2] & FSR_AO)
   {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_ao);
        r_prec = (FPptr->fsrs[2] & FSR_ARP) ? 1 : 0;
	result = FPptr->ares[2];
        FPstatus |= IEEE_A_OFLW;
        if (!(FPstatus & IEEE_X_OFLW)) {
	    bias_fr_oflw(&FPptr->ares[2],&FPptr->fp2,FPptr->fsrs[2],r_prec,1);
	}
	else {
	    FPptr->fsrs[2] |= FSR_AI;
            round_overflow(&result,r_prec,FPptr->fsrs[2]);
            if (dest != 0)
            	set_fpreg(FPptr,dest,&result,r_prec);
            FPptr->ares[2] = result;
  	    /* clear A overflow result error status */
	    FPptr->fsrs[2] &= ~FSR_AO;
	}
    }
}

/*
 * handle_underflow()
 * computes the IEEE standard result and set it into the 
 * destination register or the last stage of the pipe.
 */
handle_underflow(FPptr)
fp_frame_t *FPptr;
{

    int dest;
    int rprec,denfsr;
    fp_t result;

#if DEBUG
    if (fpe_debug) 
      printf("Entering handle_underflow\n");
#endif

    dest = (FPptr->fsrs[2] & FSR_RR) >> FSR_RR_SHIFT;
    /*
     * Underflow on the Multiplier
     */
    if (FPptr->fsrs[2] & FSR_MU)
    {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_mu);
	rprec = (FPptr->fsrs[2] & FSR_MRP) ? 1 : 0;
	result = FPptr->mres[2];
	if (rprec)
	  denfsr = dpdenormalize(&result);
	else {
	  if (spexponent((ulong *)&result))
		denfsr = spdenormalize(&result, 0x7);
	  else
		denfsr = spdenormalize(&result, 0);
	}
	if (denfsr & FSR_AA) 
	    FPptr->fsrs[2] |= FSR_MA;
	if (denfsr & FSR_AI) 
	    FPptr->fsrs[2] |= (FSR_MI | FSR_SI);
        if (FPptr->fsrs[2] & FSR_MI)
	    FPstatus |= IEEE_M_UFLW;
        if (!(FPstatus & IEEE_X_UFLW))
	    bias_fr_uflw(&FPptr->mres[2],&FPptr->fp1,rprec,FPptr->fsrs[2],0);
        else {
	    if (dest != 0)
       	        set_fpreg(FPptr,dest,&result,rprec);
       	    FPptr->mres[2] = result;
	    /* clear M unit underflow error status */
       	    FPptr->fsrs[2] &= ~FSR_MU;
#ifdef DEBUG
	    if (fpe_debug) {
	  	printf("clear mul underflow bit of fsr\n");
	  	printf("fsr[2] = %x\n",FPptr->fsrs[2]);
	    }
#endif
        }
    }
    /*
     * Underflow on the Adder
     */
    if (FPptr->fsrs[2] & FSR_AU)
    {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_au);
 	/* Adder underflow */
	rprec   = (FPptr->fsrs[2] & FSR_ARP) ? 1 : 0;
	result = FPptr->ares[2];
	if (rprec)
	  denfsr = dpdenormalize(&result);
	else
	  denfsr = spdenormalize(&result,
			(FPptr->fsrs[2] & FSR_AE) >> FSR_AE_SHIFT);
	if (denfsr & FSR_AA) 
	    FPptr->fsrs[2] |= FSR_AA;
	if (denfsr & FSR_AI) 
	    FPptr->fsrs[2] |= (FSR_AI | FSR_SI);
	if (FPptr->fsrs[2] & FSR_AI)
	    FPstatus |= IEEE_A_UFLW;
        if (!(FPstatus & IEEE_X_UFLW))
	  bias_fr_uflw(&FPptr->ares[2],&FPptr->fp2,rprec,FPptr->fsrs[2],1);
	else {
	    if (dest != 0)
                set_fpreg(FPptr,dest,&result,rprec);
            FPptr->ares[2] = result;
	    /* clear A unit underflow error status */
            FPptr->fsrs[2] &= ~FSR_AU;  
	}
     }
}

/*
 * handle_inexact()
 * handles inexact exception.
 */
handle_inexact(FPptr)
fp_frame_t *FPptr;
{
#if DEBUG
    if (fpe_debug)
      printf("Entering handle_inexact\n");
#endif

    if (FPptr->fsrs[2] & FSR_AI) {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_ai);
        FPstatus |= IEEE_A_INEXT;
	if (FPstatus & IEEE_X_INEXT)
	        FPptr->fsrs[2] &= ~FSR_AI;
    }
    if(FPptr->fsrs[2] & FSR_MI) {
	RPMSOFT_STAT_INC(cpu_number(), rpms_fpe_mi);
        FPstatus |= IEEE_M_INEXT;
	if (FPstatus & IEEE_X_INEXT)
	        FPptr->fsrs[2] &= ~FSR_MI;
    }
    if ((FPstatus & IEEE_X_INEXT))
        FPptr->fsrs[2] &= ~(FSR_TI);
}

/*
 *  round_overflow()
 *  computes the result according to current rounding mode.
 */
round_overflow(num_ptr,prec,fsr)
fp_t *num_ptr;
int prec;
ulong fsr;
{
     ulong rounding_mode;

#if DEBUG
    if (fpe_debug)
      printf("Entering round_overflow\n");
#endif
     rounding_mode = (fsr & FSR_RM) >> FSR_RM_SHIFT;
     switch(rounding_mode){
     case RM_TO_NEAREST:
#if DEBUG
    if (fpe_debug)
      printf("TO_NEAREST\n");
#endif
         if (prec)
            if (num_ptr->b_double.sign==0) 
	        num_ptr->dlong = DoublePosInfinity;
            else 
	        num_ptr->dlong = DoubleNegInfinity;
         else 
            if (num_ptr->b_single.sign==0) 
	        num_ptr->dlong = SinglePosInfinity;
            else
	        num_ptr->dlong = SingleNegInfinity;
        break;

      case RM_CHOP:
#if DEBUG
    if (fpe_debug)
      printf("TO_ZERO\n");
#endif
         if (prec)
            if (num_ptr->b_double.sign==0)
	        num_ptr->dlong = DoublePosBiggest;
            else
	        num_ptr->dlong = DoubleNegBiggest;
        else
            if (num_ptr->b_single.sign==0)
	        num_ptr->dlong = SinglePosBiggest;
            else
	        num_ptr->dlong = SingleNegBiggest;
        break;
        
     case RM_ROUND_DOWN:
#if DEBUG
    if (fpe_debug)
       printf("DOWNWARD\n");
#endif
        if (prec)
            if (num_ptr->b_double.sign==0) 
	        num_ptr->dlong = DoublePosBiggest;
            else 
	        num_ptr->dlong = DoubleNegInfinity;
        else
            if (num_ptr->b_single.sign==0)
	        num_ptr->dlong = SinglePosBiggest;
            else
	        num_ptr->dlong = SingleNegInfinity;
        break;
     case RM_ROUND_UP:
#if DEBUG
    if (fpe_debug) 
      printf("UPWARD\n");
#endif
         if (prec)
            if (num_ptr->b_double.sign==0)
	        num_ptr->dlong = DoublePosInfinity;
            else
	        num_ptr->dlong = DoubleNegBiggest;
         else
            if (num_ptr->b_single.sign==0) 
	        num_ptr->dlong = SinglePosInfinity;
            else
	        num_ptr->dlong = SingleNegBiggest;
         break;
     default:
         break;
     }
 }


#define SINGLE_FIX 		0x100
#define DOUBLE_FIX 		0x800
#define SINGLE_SHIFT 		192
#define DOUBLE_SHIFT 		1536
#define DOUBLE_BIAS 		1023
#define SINGLE_BIAS 		127
#define SINGLE_EXP_BITS 	0x7f800000
#define MANTISSA_SIZE_S         23

/*
 * bias_fr_uflw()
 * produces a biased number from the underflowed result. 
 */
bias_fr_uflw(src,res,prec,fsr,a_unit)
fp_t *src,*res;
int prec;
ulong fsr;
int a_unit;
{
    unsigned tmp_exp;
    int exp;

    if (prec) {
        res->dlong = src->dlong;
        if (res->b_double.exponent == 0)
	    res->b_double.exponent = DOUBLE_SHIFT + DOUBLE_BIAS;
	else
	    res->b_double.exponent = (res->b_double.exponent-DOUBLE_FIX) 
	                             + DOUBLE_SHIFT + DOUBLE_BIAS;
    }
    else {
        if (a_unit) {
            /*
	     *  If the underflow was caused by fadd.ds,fsub.ds and famov.ds
	     *  operations, extra higer 3 bits of exponent could be stored    
	     *  into AE bits of FSR.
	     */
	    tmp_exp = (fsr & FSR_AE) >> (FSR_AE_SHIFT - 8);
	    tmp_exp |= ((src->dlong.lo & SINGLE_EXP_BITS) >> MANTISSA_SIZE_S);
	}
	else
	    tmp_exp = ((src->dlong.lo & SINGLE_EXP_BITS) >> MANTISSA_SIZE_S);
	res->b_double.mantissa_a = src->b_single.mantissa_a;
        res->b_double.mantissa_b = src->b_single.mantissa_b >> 3;
	res->b_double.mantissa_c = (src->b_single.mantissa_b & 0x3) << 29;
	if (tmp_exp == 0) 
	    exp = 0;
	else {
	    tmp_exp |= 0xfffff800;
	    exp = (int)tmp_exp;
	}
	res->b_double.exponent = (exp - SINGLE_BIAS) + DOUBLE_BIAS + DOUBLE_SHIFT;
    }
}

/*
 * bias_fr_oflow()
 * produces a biased number from the overflowed result.
 */
bias_fr_oflw(src,res,prec,fsr,a_unit)
fp_t *src,*res;
int prec;
ulong fsr;
int a_unit;
{
    unsigned exp;

    if (prec) {
        res->dlong = src->dlong;
        if (res->b_double.exponent == 0x7ff)
	    res->b_double.exponent = 0x7ff - DOUBLE_SHIFT - DOUBLE_BIAS;
	else
	    res->b_double.exponent = (DOUBLE_FIX + src->b_double.exponent) 
	                             - DOUBLE_SHIFT - DOUBLE_BIAS;
    }
    else {
        if (a_unit) {
            /*
	     *  If the overflow was caused by fadd.ds,fsub.ds and famov.ds
	     *  operations, extra higer 3 bits of exponent could be stored    
	     *  into AE bits of FSR.
	     */
	    exp = (fsr & FSR_AE) >> (FSR_AE_SHIFT - 8);
	    exp |= ((src->dlong.lo & SINGLE_EXP_BITS) >> MANTISSA_SIZE_S);
	}
	else
	    exp = ((src->dlong.lo & SINGLE_EXP_BITS) >> MANTISSA_SIZE_S);
	res->b_double.mantissa_a = src->b_single.mantissa_a;
        res->b_double.mantissa_b = src->b_single.mantissa_b >> 3;
	res->b_double.mantissa_c = (src->b_single.mantissa_b & 0x3) << 29;
	if (exp < 0xff) exp += 0x100;
	res->b_double.exponent = (exp - SINGLE_BIAS)
	                         - DOUBLE_BIAS - DOUBLE_SHIFT;
    }
}


