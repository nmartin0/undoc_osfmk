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
 * $Id: fpe_utils.c,v 1.4 1994/11/18 20:40:30 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_utils.c,v $
 * Revision 1.4  1994/11/18  20:40:30  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:28  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_utils.c    1.2"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"

/*
 * is_infinity()
 * returns 1 if x is +infinity
 *        -1 if x is -infinity
 *         0 otherwise
 */ 
int       
is_infinity(x,prec)  
fp_t *x;
int prec;
{
#if DEBUG
    if (fpe_debug) {
      printf("Entering is_infinity()\n");
      print_i860type(x);
    }
#endif
    if (prec) {
        if ((x->b_double.exponent==0x7ff) && (x->b_double.mantissa_a==0)
            && (x->b_double.mantissa_b==0) && (x->b_double.mantissa_c==0))
            return((x->b_double.sign) ? -1 : 1);
        else 
            return(0);
    }
    else {
        if ((x->b_single.exponent==0xff) && (x->b_single.mantissa_a==0)
            && (x->b_single.mantissa_b==0)) 
	    return((x->b_single.sign) ? -1 : 1);
	else 
            return(0);
    }
}

/*
 * is_nan()
 * returns 1 if x is a +nan
 *        +1 if x if a -nan
 *         0 otherwise
 */
int
is_nan(x,prec)
fp_t *x;
int prec;
{
    if (is_infinity(x,prec))
	return(0);
    if (prec) {
	if (x->b_double.exponent == 0x7ff)
	    return((x->b_double.sign) ? -1 : 1);
	else 
	    return(0);
    }
    else {
	if (x->b_single.exponent == 0xff)
	    return((x->b_single.sign) ? -1 : 1);
	else
	    return(0);
    }
}

/*
 * is_qnan()
 * returns 1 if x is a +quiet_nan
 *        +1 if x if a -quiet_nan
 *         0 otherwise
 */
int
is_qnan(x,prec)
fp_t *x;
int prec;
{
#if DEBUG
  if (fpe_debug) {
    printf("Entering is_qnan()\n");
    print_i860type(x);
  }
#endif
    if (prec) {
#if DEBUG
   if (fpe_debug)
     printf("is_qnan(): double part\n");
#endif
        if ((x->b_double.exponent==0x7ff) && (x->b_double.mantissa_a==1))
            return((x->b_double.sign) ? -1 : 1);
        else
            return(0);
    }
#if DEBUG
    if (fpe_debug)
      printf("is_qnan(): single part\n");
#endif
        if ((x->b_single.exponent==0xff) && (x->b_single.mantissa_a==1))
            return((x->b_single.sign) ? -1 : 1);
        else
            return(0);
}

/* 
 * is_snan()
 * returns 1 if x is a +signaling_nan
 *        -1 if x is a -signaling_nan
 *         0 otherwise
 */
int
is_snan(x,prec)
fp_t *x;
int prec;
{
#if DEBUG
    if (fpe_debug) {
      printf("Entering is_snan()\n");
      print_i860type(x);
    }
#endif
    if (prec) { 
#if DEBUG
    if (fpe_debug)
      printf("is_snan(): double part\n");
#endif
        if ((x->b_double.exponent==0x7ff) && (x->b_double.mantissa_a==0) &&
            ((x->b_double.mantissa_b!=0) || (x->b_double.mantissa_c!=0))) 
            return((x->b_double.sign) ? -1 : 1);
        else 
            return(0);
    }
    else {
#if DEBUG
    if (fpe_debug)
      printf("is_snan(): single part\n");
#endif
        if ((x->b_single.exponent==0xff) && (x->b_single.mantissa_a==0) 
           && (x->b_single.mantissa_b!=0))
            return((x->b_single.sign) ? -1 : 1);
	else 
            return(0);
    }
}

/*
 * is_zero()
 * returns 1 if x is +zero
 *        -1 if x is -zero 
 *         0 otherwise
 */
int 
is_zero(x,prec)   /* returns 1 if x is zero, 0 otherwise */ 
fp_t *x;
int prec;
{
#if DEBUG
    if (fpe_debug) {
      printf("Entering is_zero()\n");
      print_i860type(x);
    }
#endif
    if (prec) {
#if DEBUG
      if (fpe_debug)
	printf("is_zero(): double part\n");
#endif
        if ((x->b_double.exponent==0) && (x->b_double.mantissa_a==0) &&
            (x->b_double.mantissa_b==0) && (x->b_double.mantissa_c==0))
            return((x->b_double.sign) ? -1 : 1);
	else
            return(0);
    }
    else {
#if DEBUG
   if (fpe_debug)
     printf("is_zero(): single part\n");
#endif
        if ((x->b_single.exponent==0) && (x->b_single.mantissa_a==0)
            && (x->b_single.mantissa_b==0))
            return((x->b_single.sign) ? -1 : 1);
        else
            return(0);
    }
}

/*
 * is_negative(x)
 * returns 1 if x is negative,
 *         0 otherwise
 */
is_negative(x,prec) 
fp_t *x;
int prec;
{
#if DEBUG
    if (fpe_debug) {
      printf("Entering is_negative\n");
      print_i860type(x);
    }
#endif
    if (prec) {
        if (x->b_double.sign==1)
            return(1);
	else
            return(0);
    }
    else {
        if (x->b_single.sign==1)
            return(1);
	else
	    return(0);
    }
}

/*
 * is_denormal()
 * returns 1 if x is +denormal
 *        -1 if x is -denormal
 *	   0 otherwise
 */
int
is_denormal(x,prec)
fp_t *x;
int prec;
{   
    if (is_zero(x,prec))
	return(0);
    else if (prec) {
        if (x->b_double.exponent == 0) {
	    if (x->b_double.sign)
                return(-1);
            else
                return(1);
	}
	else
	    return(0);
    }
    else {
        if (x->b_single.exponent == 0) {
            if (x->b_single.sign)
                return(-1);
            else
                return(1);
	}
	else
	    return(0);
    }
}

/*
 * nan_gt()
 * compares two nans, and returns 1 if num1 > num2 
 *                                0 ohterwise
 */
int
nan_gt(num1,num2,prec)
fp_t *num1,*num2;
int prec;
{
#if DEBUG
    if (fpe_debug) {
      printf("entering nan_cmp_gt()\n");
      print_i860type(num1);
      print_i860type(num2);
    }
#endif
    if (prec) {
        if(num1->b_double.exponent>num2->b_double.exponent) /* nan and number */
            return(1);
        if(num1->b_double.exponent<num2->b_double.exponent) /* number and nan */
            return(0);
        /* two nans */
        if(num1->b_double.mantissa_a==1)
        {
            if(num2->b_double.mantissa_a==1) /* two qnans */
            {
                if(num1->b_double.mantissa_b>num2->b_double.mantissa_b)
                    return(1);
                if(num1->b_double.mantissa_b<num2->b_double.mantissa_b)
                    return(0);
                return(num1->b_double.mantissa_c>num2->b_double.mantissa_c);
            }
            return(1); /* qnan and snan (qnan always > snan) */
         }   
        if(num2->b_double.mantissa_a==1)
            return(0); /* snan and qnan */
                
        /* two snans */
        if(num1->b_double.mantissa_b>num2->b_double.mantissa_b)
            return(1);
        if(num1->b_double.mantissa_b<num2->b_double.mantissa_b)
            return(0);
        return(num1->b_double.mantissa_c>num2->b_double.mantissa_c);
    }
    else
    {
        /* nan and number */
        if(num1->b_single.exponent>num2->b_single.exponent)
            return(1);
        if(num1->b_single.exponent<num2->b_single.exponent)
            return(0);
        /* two nans */
        if(num1->b_single.mantissa_a==1)
        {
#if DEBUG
	  if (fpe_debug)
	    printf("two qnans or qnan and snan\n");
#endif
            if(num2->b_single.mantissa_a==1)  /* two qnans */
                return(num1->b_single.mantissa_b>num2->b_single.mantissa_b);
            return(1); /* qnan and snan (qnan is always > snan) */
        }
        if(num2->b_single.mantissa_a==1)
        {
#if DEBUG
	  if (fpe_debug)
	    printf("snan and qnan\n");
#endif
            return(0);  /* snan and qnan */
        }
        /* two snans */
        return(num1->b_single.mantissa_b>num2->b_single.mantissa_b);
    }
}

/*
 * fp_gt()
 * compares denormal and normal numbers, and
 * returns 1 if src1 > src2 
 *         0 otherwise 
 */
int
fp_gt(src1,src2,prec)
fp_t *src1,*src2;
int prec;
{
    if (prec) {
	if (src1->b_double.sign < src2->b_double.sign) 
	    return(1);
	if (src1->b_double.sign > src2->b_double.sign)
	    return(0);
	if ((src1->b_double.sign == 0) && (src2->b_double.sign == 0)) {
	    if (src1->dlong.hi > src2->dlong.hi) 
		return(1);
	    if (src1->dlong.hi < src2->dlong.hi)
		return(0);
	    if (src1->dlong.lo > src2->dlong.lo)
		return(1);
	    else
	        return(0);
	}
	else {
	    if (src1->dlong.hi < src2->dlong.hi)
		return(1);
	    if (src1->dlong.hi > src2->dlong.hi)
		return(0);
	    if (src1->dlong.lo < src2->dlong.lo)
		return(1);
	    else
		return(0);
	}
    }	
    else {
	if (src1->b_single.sign < src2->b_single.sign)
	    return(1);
	if (src1->b_single.sign > src2->b_single.sign)
	    return(0);
	if ((src1->b_single.sign == 0) && (src2->b_single.sign == 0)) {
	    if (src1->dlong.lo > src2->dlong.lo)
		return(1);
	    else
		return(0);
	}
	else {
	    if (src1->dlong.lo < src2->dlong.lo)
		return(1);
	    else 
		return(0);
	}
    }
}
   

/* inf_gt()
 * compares two sources at least one of which is infinity
 * returns 1 if src1 > src2,
 *         0 otherwise
 */                                 
int
inf_gt(src1,src2,prec)
fp_t *src1,*src2;
int prec;
{
#if DEBUG
  if (fpe_debug)
    printf("Entering inf_gt\n");
#endif

    if ((src1->dlong.hi == src2->dlong.hi) && 
	      (src1->dlong.lo == src2->dlong.lo))
        return(0);
    if(prec) {
        if (src1->b_double.exponent == src2->b_double.exponent) 
	    /* src1=src2=infinity */
            return(src1->b_double.sign < src2->b_double.sign);
        if (src1->b_double.exponent < src2->b_double.exponent) 
	    /* src2=infinity */
            return((src2->b_double.sign == 0) ? 0:1);  
        else  /* src1=infinity */
            return((src1->b_double.sign == 0) ? 1:0);
    }    
    else {
        if (src1->b_single.exponent == src2->b_single.exponent) 
	    /* src1=src2=infinity */
            return(src1->b_single.sign < src2->b_single.sign);
        if (src1->b_single.exponent < src2->b_single.exponent) 
	    /* src2=infinity */
            return((src2->b_single.sign == 0) ? 0:1);  
        else  /* src1=infinity */
            return((src1->b_single.sign == 0) ? 1:0);
    }    
}

/* 
 * do_a_r_shift_d()
 * shifts a double precision mantissa 1 bit to the right 
 */
do_a_r_shift_d(num)  
fp_t *num;
{
    int shifted_out_1;

    shifted_out_1 = (num->dlong.hi & 0x01) ? 1 : 0;
    num->dlong.hi =  num->dlong.hi >> 1;
    num->dlong.lo = num->dlong.lo >> 1;
    if(shifted_out_1)
        num->dlong.lo |= 0x80000000;
}

/*
 * do_a_l_shift_d()
 * shifts a double precision mantissa 1 bit to the left
 */
do_a_l_shift_d(num) 
fp_t *num;
{
    int shifted_out_1;
    shifted_out_1 = (num->dlong.lo & 0x80000000) ? 1 : 0;
    num->dlong.lo = num->dlong.lo << 1;
    num->dlong.hi = num->dlong.hi << 1;
    if(shifted_out_1)
        num->dlong.hi |= 0x1;
}

set_fpreg(FPptr,dest,res,prec)
fp_frame_t *FPptr;
int dest;
fp_t *res;
int prec;
{
    if (prec) {
        FPptr->fregs[dest] = res->dlong.lo;
	FPptr->fregs[dest+1] = res->dlong.hi;
    }
    else
	FPptr->fregs[dest] = res->dlong.lo;
}    

get_fpreg(FPptr,src,dest,prec)
fp_frame_t *FPptr;
int src;
fp_t *dest;
int prec;
{
    if (prec) {
	dest->dlong.lo = FPptr->fregs[src];
	dest->dlong.hi = FPptr->fregs[src+1];
    }
    else {
	dest->dlong.lo = FPptr->fregs[src];		
	dest->dlong.hi = 0;
    }
}

spassign(d,s)
ulong *d,*s;
{
    d[0] = s[0];
}

dpassign(d,s)
ulong *d,*s;
{
    d[0] = s[0];
    d[1] = s[1];
}
	
/*
 * to_double(x) 
 * convert the normal or denormal number of single precision to 
 * double precision.
 */
void
to_double(x)
fp_t *x;
{
    fp_t tmp;

    tmp.n_double = 0;
    if(is_denormal(x,0)) 
	famovsd_den(x);
    else { /* is normal */
	tmp.n_double = (double)(x->n_single);
	tmp.b_double.sign = x->b_single.sign;
	*x = tmp;	
    }
}

void
to_double_nan(i860_ptr) /* single precision nan is expanded to double
                          with zeroes                                */
fp_t *i860_ptr;
{

    i860_ptr->b_double.sign = i860_ptr->b_single.sign;
#ifdef NON_387
    i860_ptr->b_double.mantissa_c = i860_ptr->b_single.mantissa_b;
    i860_ptr->b_double.mantissa_b = 0;
#else
    i860_ptr->b_double.mantissa_b = i860_ptr->b_single.mantissa_b >> 3;
    i860_ptr->b_double.mantissa_c = i860_ptr->b_single.mantissa_b << 29;
#endif
    i860_ptr->b_double.mantissa_a = 1;
    i860_ptr->b_double.exponent = 0x7ff;
}

void
to_single_nan(i860_ptr)  /* double nan is condensed to single nan */
fp_t *i860_ptr;
{
    fp_t fp_tmp;
    fp_tmp.b_single.sign = i860_ptr->b_double.sign;
    fp_tmp.b_single.exponent = 0xff;
    fp_tmp.b_single.mantissa_a = 1;
    fp_tmp.b_single.mantissa_b = (i860_ptr->b_double.mantissa_b << 3) |
				   (i860_ptr->b_double.mantissa_c >> 29);
    fp_tmp.dlong.hi = 0;
    *i860_ptr = fp_tmp;
}

to_qnan(x,prec)
fp_t *x;
int prec;
{
    if (prec) 
        x->b_double.mantissa_a = 1;
    else 
	x->b_single.mantissa_a = 1;
}

/*
 * set_rounding()
 * sets i860's rounding mode and returns the previsous rounding mode
 */
ulong  
set_rounding(rmode)
ulong rmode;
{
	ulong oldfsr;

	oldfsr = get_fsr();
	set_fsr((oldfsr & ~FSR_RM) | (rmode << FSR_RM_SHIFT));
	return((oldfsr & 0x0c) >> FSR_RM_SHIFT);
}

/* void unpack(d, s)
 * ulong *d, *s;
 *
 * The registers KI, KR and T store single precision numbers in double
 * precision format by zero expanding the exponent field from 8 to 11 bits, and
 * storing the 23 bit single precision mantissa in the top 23 bits of the
 * 52 bit double precision mantissa, with zeros in the low order 29 bits.
 * The bias is unchanged. Given a single precision number s, this procedure 
 * does this unpacking, and returns the double precision number in d.
 *
 * The one variation in the zero expansion that we do is when the single 
 * precision number has an exponent of 7f8 (ie. infinity or Nan).  In this
 * case, we set the high bits to 1, so that when KI, KR or T is loaded in
 * double precision format, we still generate a source exception when they
 * are used later on.  Zero extending infinities or Nans will not generate the
 * appropriate source exception.
 *
 */

unpack( d, s)
ulong	*d, *s;
{

	d[0] =  (s[0] << 29);			/* bottom 3 bits */
	d[1] =  (s[0] & 0x7fffffff) >> 3;	/* top 20 bits and exponent */
	d[1] |= (s[0] & 0x80000000);		/* sign bit */
	if ((s[0] & 0x7f800000) == 0x7f800000) {
		/* input is a single precision infinity or Nan */
		d[1] |= 0x70000000;		/* set top 3 bits also */
	}
}

/*
 * Does the reverse of the above operation: d is single precision, s is double
 * precision. (d = dest, s = source): i.e, moves a "double precision lookalike"
 * in KI,KR or T into a single precision register.
 */
pack ( d, s)
ulong	*d, *s;		
{
	d[0] =  (s[1] & 0x80000000) |	        /* sign */
	       ((s[1] & 0x0fffffff) << 3) |	/* exponent + 20 bit mant */
	        (s[0] >> 29);			/* 3 more mantissa bits */
}

/*
 * flip_sign()
 */
flip_sign(x,prec)
fp_t *x;
int prec;
{
    if (prec)
	x->b_double.sign = (x->b_double.sign) ? 0 : 1;
    else
	x->b_single.sign = (x->b_single.sign) ? 0 : 1;
}

/* 
 * xor_signs()
 * returns the xor of the signs of src1, src2 
 */
xor_signs(src1,src2,prec)  
fp_t *src1,*src2;
int prec;
{
#if DEBUG
  if (fpe_debug)
    printf("Entering xor_signs\n");
#endif

    if (prec)
        return(src1->b_double.sign ^ src2->b_double.sign);
    else
        return(src1->b_single.sign ^ src2->b_single.sign);
}

