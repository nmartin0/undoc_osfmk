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
 
#define FPDEBUG1 0
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: fpehutils.c,v $
 * Revision 1.3  1994/11/18  20:39:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1993/06/30  22:30:48  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.1  1992/08/26  10:45:33  stans
 * Initial revision
 *
 * Revision 2.3.1.1  92/05/27  00:40:26  jeffreyh
 * 	[stans@ssd.intel.com]
 * 	Support floating point exceptions.
 * 
 * Revision 2.3  91/06/18  20:51:16  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:54:12  jsb]
 * 
 * Revision 2.2  91/06/17  15:44:48  jsb
 * 	From Paul Pierce: created from iPSC/860 NX code.
 * 	[91/06/17  13:40:37  jsb]
 * 
 */

#if FPDEBUG1
int     fpe_debug1=0;
#define dprintf if (fpe_debug1) printf
#endif


/*************************************************************
*                                                            *
*  This module contains utility procedures used by the main  *
*  procedures of the i860FPEH.                               *
*                                                            *
* $Id: fpehutils.c,v 1.3 1994/11/18 20:39:00 mtm Exp $
*
*  updated to NX version: fpehutils.c 10.3 92/06/24 16:41:48 *
*  [ Wed Jun 24 13:58:09 PDT 1992 ] stan                     *
*                                                            *
*  Author: Tevi Devor Nov/87 ISWP                            *
*  fpehutils.c 8.3 91/01/10 10:48:40                         *
*                                                            *
*************************************************************/

#include <machine/psl.h>
#include <machine/fpdefs.h>
#include <machine/thread.h>

extern word32 fsr;
extern word32 *IEEE_status_ptr;
extern FPtrapcontrol *FPptr;

extern void compute_biased(), round_overflow();
void do_a_l_shift_d(), determine_inxct_rnd();

/*
 * nan_cmp_gt
 * compares two nans returns 1 if num1 > num2, 0 otherwise
 */
int
nan_cmp_gt(num1, num2, A_part_of_dualop)
    register i860_type *num1,
    *num2;
{
#if FPDEBUG1
    dprintf("Entering nan_cmp_gt\n");
#if FPDEBUG2
    print_i860type(num1);
    print_i860type(num2);
#endif
#endif
    if ((A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)) ||
	  (!(A_part_of_dualop) && (*IEEE_status_ptr & M_PRECISION_BIT))) {

	if (num1->b_double.exponent > num2->b_double.exponent)	/* nan and number */
	    return (1);
	if (num1->b_double.exponent < num2->b_double.exponent)	/* number and nan */
	    return (0);
	/* two nans */
	if (num1->b_double.mantissa_a == 1) {
	    if (num2->b_double.mantissa_a == 1) {	/* two qnans */
		if (num1->b_double.mantissa_b > num2->b_double.mantissa_b)
		    return (1);
		if (num1->b_double.mantissa_b < num2->b_double.mantissa_b)
		    return (0);
		return (num1->b_double.mantissa_c > num2->b_double.mantissa_c);
	    }
	    return (1);		/* qnan and snan (qnan always > snan) */
	}
	if (num2->b_double.mantissa_a == 1)
	    return (0);		/* snan and qnan */

	/* two SNaNs */
	if (num1->b_double.mantissa_b > num2->b_double.mantissa_b)
	    return (1);
	if (num1->b_double.mantissa_b < num2->b_double.mantissa_b)
	    return (0);
	return (num1->b_double.mantissa_c > num2->b_double.mantissa_c);
    } else {
	/* NaN and Number */
	if (num1->b_single.exponent > num2->b_single.exponent)
	    return (1);
	if (num1->b_single.exponent < num2->b_single.exponent)
	    return (0);
	/* two nans */
	if (num1->b_single.mantissa_a == 1) {
#if FPDEBUG2
	    printf("two qnans or qnan and snan\n");
#endif
	    if (num2->b_single.mantissa_a == 1)	/* two qnans */
		return (num1->b_single.mantissa_b > num2->b_single.mantissa_b);
	    return (1);		/* qnan and snan (qnan is always > snan) */
	}
	if (num2->b_single.mantissa_a == 1) {
#if FPDEBUG2
	    printf("snan and qnan\n");
#endif
	    return (0);		/* snan and qnan */
	}
	/* two SNaNs */
	return (num1->b_single.mantissa_b > num2->b_single.mantissa_b);
    }

}


/*
 * turn_on_msb
 * turns on most significant bit of snan in order to make it into a qnan
 */
void
turn_on_msb(num, A_part_of_dualop)
    i860_type *num;
{
#if FPDEBUG1
    dprintf("Entering turn_on_msb\n");
#if FPDEBUG2
    print_i860type(num);
#endif
#endif
    if ((A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)) ||
	    (!(A_part_of_dualop) && (*IEEE_status_ptr & M_PRECISION_BIT)))
	num->b_double.mantissa_a = 1;
    else
	num->b_single.mantissa_a = 1;
}


/*
 * infinity
 * returns 1 if x is +infinity -1 if x is -infinity 0 otherwise
 */
int
infinity(x, A_part_of_dualop)
    register i860_type *x;
{
#if FPDEBUG1
    dprintf("Entering infinity\n");
#if FPDEBUG2
    print_i860type(x);
#endif
#endif
    if ((A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)) ||
	  (!(A_part_of_dualop) && (*IEEE_status_ptr & M_PRECISION_BIT))) {

	if ((x->b_double.exponent == 0x7ff) && (x->b_double.mantissa_a == 0)
	&& (x->b_double.mantissa_b == 0) && x->b_double.mantissa_c == 0) {
	    if (x->b_double.sign) {
		return (-1);
	    } else {
	        return (1);
	    }
	}
	return (0);
    } else { /* single */
        if ((x->b_single.exponent == 0xff) && (x->b_single.mantissa_a == 0)
	        && (x->b_single.mantissa_b == 0)) {
	    if (x->b_single.sign) {
	        return (-1);
	    } else {
	        return (1);
            }
        }
    }
    return (0);
}


/* 
 * is_infinity
 * returns 1 if x is infinity 0 otherwise
 */
int
is_infinity(x, r_prec)
    i860_type x;
{

    if (r_prec) {
	if ((x.b_double.exponent == 0x7ff) && (x.b_double.mantissa_a == 0)
	    && (x.b_double.mantissa_b == 0) && x.b_double.mantissa_c == 0)
	    return (1);
	return (0);
    }
    if ((x.b_single.exponent == 0xff) && (x.b_single.mantissa_a == 0)
	    && (x.b_single.mantissa_b == 0))
	return (1);
    return (0);
}


/* 
 * quiet_nan
 * returns 1 if x is a quiet nan 0 otherwise
 */
int
quiet_nan(x, A_part_of_dualop)
    register i860_type *x;
{
#if FPDEBUG1
    dprintf("Entering quiet_nan\n");
#if FPDEBUG2
    print_i860type(x);
#endif
#endif
    if ((A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)) ||
	  (!(A_part_of_dualop) && (*IEEE_status_ptr & M_PRECISION_BIT))) {
#if FPDEBUG2
	printf("double part\n");
#endif
	if ((x->b_double.exponent == 0x7ff) && (x->b_double.mantissa_a == 1))
	    return (1);
	return (0);
    }
#if FPDEBUG2
    printf("single part\n");
#endif
    if ((x->b_single.exponent == 0xff) && (x->b_single.mantissa_a == 1))
	return (1);
    return (0);
}


/*
 * signaling_nan
 * returns 1 if x is a signaling nan 0 otherwise
 */
int
signaling_nan(x, A_part_of_dualop)
    register i860_type *x;
{
#if FPDEBUG1
    dprintf("Entering signaling_nan\n");
#if FPDEBUG2
    print_i860type(x);
#endif
#endif
    if ((A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)) ||
	  (!(A_part_of_dualop) && (*IEEE_status_ptr & M_PRECISION_BIT))) {
#if FPDEBUG2
	printf("double part\n");
#endif
	if ((x->b_double.exponent == 0x7ff) && (x->b_double.mantissa_a == 0) &&
	 ((x->b_double.mantissa_b != 0) || (x->b_double.mantissa_c != 0)))
	    return (1);
	return (0);
    }
#if FPDEBUG2
    printf("single part\n");
#endif
    if ((x->b_single.exponent == 0xff) && (x->b_single.mantissa_a == 0) &&
	    (x->b_single.mantissa_b != 0))
	return (1);
    return (0);
}


/*
 * is_zero
 * returns 1 if x is zero, 0 otherwise
 */
int
is_zero(x)
    register i860_type *x;
{
#if FPDEBUG1
    dprintf("Entering is_zero\n");
#if FPDEBUG2
    print_i860type(x);
#endif
#endif
    if (*IEEE_status_ptr & S_PRECISION_BIT) {
#if FPDEBUG2
	printf("double part\n");
#endif
	if ((x->b_double.exponent == 0) && (x->b_double.mantissa_a == 0) &&
	   (x->b_double.mantissa_b == 0) && (x->b_double.mantissa_c == 0))
	    return (1);
	return (0);
    }
#if FPDEBUG2
    printf("single part\n");
#endif
    if ((x->b_single.exponent == 0) && (x->b_single.mantissa_a == 0) &&
	    (x->b_single.mantissa_b == 0))
	return (1);
    return (0);
}


/* 
 * is_negative 
 * returns 1 if x is negative, 0 otherwise
 */
int
is_negative(x)
    i860_type *x;
{
#if FPDEBUG1
    dprintf("Entering is_negative\n");
#if FPDEBUG2
    print_i860type(x);
#endif
#endif
    if (*IEEE_status_ptr & M_PRECISION_BIT) {
	if (x->b_double.sign == 1)
	    return (1);
	return (0);
    }
    if (x->b_single.sign == 1)
	return (1);
    return (0);
}


/* 
 * is_positive
 * returns 1 if x is positive, 0 otherwise
 */
int 
is_positive(x, s_prec)	
    i860_type x;
    int s_prec;
{
    if (s_prec)
	return (!(x.b_double.sign));
    else
	return (!(x.b_single.sign));
}

/*
 * to_double_nan
 * single precision nan is expanded to double with zeroes
 */
void
to_double_nan(i860_ptr)
    register i860_type *i860_ptr;
{
#if FPDEBUG1
dprintf("Into to_double_nan "); print_i860type(i860_ptr);
#endif
    i860_ptr->b_double.sign = i860_ptr->b_single.sign;
    i860_ptr->b_double.mantissa_c = i860_ptr->b_single.mantissa_b;
    i860_ptr->b_double.mantissa_b = 0;
    i860_ptr->b_double.mantissa_a = 1;
    i860_ptr->b_double.exponent = 0x7ff;
#if FPDEBUG1
dprintf("Out of to_double_nan "); print_i860type(i860_ptr);
#endif
}


/* 
 * to_single_nan
 * double nan is condensed to single nan
 */
void
to_single_nan(i860_ptr)
    register i860_type *i860_ptr;
{
    i860_ptr->b_single.sign = i860_ptr->b_double.sign;
    i860_ptr->b_single.exponent = 0xff;
    i860_ptr->b_single.mantissa_a = 1;
    i860_ptr->two_words.top_word32 = 0;
}

/* 
 * to_i860_underflow
 * noramlizes a denormal, returns the i860 underflow number
 */
void
to_i860_underflow(num, source_precision)
    register i860_type *num;
    int source_precision;
{
    int num_shifted, sign;
#if FPDEBUG1
    dprintf("Enter to_i860_underflow\n");
#endif
    num_shifted = 0;
    if (source_precision) {
	sign = num->b_double.sign;
	while (!num->b_double.mantissa_a) {
	    /* do a shift */
	    do_a_l_shift_d(num);
	    num_shifted++;
	}
	do_a_l_shift_d(num);	/* now we have an implicit 1 preceding
				 * binary point */
	num->b_double.sign = sign;
	num_shifted++;
	num->b_double.exponent = ((word32) (-1 * (num_shifted - 1)) & 0x7ff);
    } else {
	sign = num->b_single.sign;
	while (!num->b_single.mantissa_a) {
	    /* do a shift */
	    num->two_words.bottom_word32 = num->two_words.bottom_word32 << 1;
	    num_shifted++;
	}
	num->two_words.bottom_word32 = num->two_words.bottom_word32 << 1;
	num->b_single.sign = sign;
	num_shifted++;
	num->b_single.exponent = ((word32) (-1 * (num_shifted - 1)) & 0x0ff);
    }
}


/*
 * to_double_precision
 * converts to double precision
 */
void
to_double_precision(num)
    register i860_type *num;
{
    i860_type tmp;
    tmp.n_double = 0;
    if (is_denormal(*num, SINGLE)) {
	to_i860_underflow(num, SINGLE);
#if FPDEBUG1
	dprintf("IN to double, The single is denormal\n");
	dprintf("The number from after to_i860_underflow is ");
	print_i860type(num);
#endif
	tmp.b_double.sign = num->b_single.sign;
	if (num->b_single.exponent == 0)
	  tmp.b_double.exponent =  DOUBLE_BIAS - SINGLE_BIAS;
	else
	  tmp.b_double.exponent = (word32)num->b_single.exponent - 256 - SINGLE_BIAS + DOUBLE_BIAS;
	tmp.b_double.mantissa_a = num->b_single.mantissa_a;
	tmp.b_double.mantissa_b = num->b_single.mantissa_b >> 3;
	tmp.b_double.mantissa_c = (num->b_single.mantissa_b & 0x3) << 29;
	*num = tmp;
    } else			/* is normal */
	num->n_double = (i860_double) (num->n_single);
}


/* 
 * inf_gt
 * compares two sources at least one of which is infinity
 * returns 1 if scr1 > src2, 0 otherwise
 */
int
inf_gt(src1, src2)
    i860_type src1,
     src2;
{
#if FPDEBUG1
    dprintf("Entering inf_gt\n");
#endif
    if ((src1.two_words.top_word32 == src2.two_words.top_word32) &&
	    (src1.two_words.bottom_word32 == src2.two_words.bottom_word32))
	return (0);
    if (*IEEE_status_ptr & S_PRECISION_BIT) {
	if (src1.b_double.exponent == src2.b_double.exponent)
	/* src1=src2=infinity */
	    return (src1.b_double.sign < src2.b_double.sign);
	if (src1.b_double.exponent < src2.b_double.exponent)
	/* src2=infinity */
	    return ((src2.b_double.sign == 0) ? 0 : 1);
	else			/* src1=infinity */
	    return ((src1.b_double.sign == 0) ? 1 : 0);
    } else {
	if (src1.b_single.exponent == src2.b_single.exponent)
	/* src1=src2=infinity */
	    return (src1.b_single.sign < src2.b_single.sign);
	if (src1.b_single.exponent < src2.b_single.exponent)
	/* src2=infinity */
	    return ((src2.b_single.sign == 0) ? 0 : 1);
	else			/* src1=infinity */
	    return ((src1.b_single.sign == 0) ? 1 : 0);
    }
}

/* 
 * denormal_gt
 * compares two sources at least one of which is infinity
 * returns 1 if scr1 > src2, 0 otherwise
 */
denormal_gt(src1, src2, s_prec)
    i860_type	src1, src2;
    int		s_prec;
{
#if FPDEBUG1
    dprintf("Entering denorm_gt( %lx %lx %lx)\n", src1, src2, s_prec);
#endif
    if (s_prec) {
	if (src1.b_double.sign < src2.b_double.sign)
	    return (1);
	if (src1.b_double.sign > src2.b_double.sign)
	    return (0);
/* rag */
	if (src1.b_double.sign) {	/* both negative */
	    if (src1.b_double.exponent > src2.b_double.exponent)
		return (0);
	    if (src1.b_double.exponent < src2.b_double.exponent)
		return (1);
	    if (src1.b_double.mantissa_a > src2.b_double.mantissa_a)
		return (0);
	    if (src1.b_double.mantissa_a < src2.b_double.mantissa_a)
		return (1);
	    if (src1.b_double.mantissa_b > src2.b_double.mantissa_b)
		return (0);
	    if (src1.b_double.mantissa_b < src2.b_double.mantissa_b)
		return (1);
	    if (src1.b_double.mantissa_c > src2.b_double.mantissa_c)
		return (0);
	    return (0);		/* equal */
	} else {		/* both positive */
	    if (src1.b_double.exponent > src2.b_double.exponent)
		return (1);
	    if (src1.b_double.exponent < src2.b_double.exponent)
		return (0);
	    if (src1.b_double.mantissa_a > src2.b_double.mantissa_a)
		return (1);
	    if (src1.b_double.mantissa_a < src2.b_double.mantissa_a)
		return (0);
	    if (src1.b_double.mantissa_b > src2.b_double.mantissa_b)
		return (1);
	    if (src1.b_double.mantissa_b < src2.b_double.mantissa_b)
		return (0);
	    if (src1.b_double.mantissa_c > src2.b_double.mantissa_c)
		return (1);
	    return (0);		/* equal */
	}
/* rag */
    } else {
	if (src1.b_single.sign < src2.b_single.sign)
	    return (1);
	if (src1.b_single.sign > src2.b_single.sign)
	    return (0);
/* rag */
	if (src1.b_single.sign) {	/* both negative */
	    if (src1.b_single.exponent > src2.b_single.exponent)
		return (0);
	    if (src1.b_single.exponent < src2.b_single.exponent)
		return (1);
	    if (src1.b_single.mantissa_a > src2.b_single.mantissa_a)
		return (0);
	    if (src1.b_single.mantissa_a < src2.b_single.mantissa_a)
		return (1);
	    if (src1.b_single.mantissa_b > src2.b_single.mantissa_b)
		return (0);
	    return (0);		/* equal */
	} else {		/* both positive */
	    if (src1.b_single.exponent > src2.b_single.exponent)
		return (1);
	    if (src1.b_single.exponent < src2.b_single.exponent)
		return (0);
	    if (src1.b_single.mantissa_a > src2.b_single.mantissa_a)
		return (1);
	    if (src1.b_single.mantissa_a < src2.b_single.mantissa_a)
		return (0);
	    if (src1.b_single.mantissa_b > src2.b_single.mantissa_b)
		return (1);
	    return (0);		/* equal */
	}
/* rag */
    }
}


/*
 *xor_signs
 * returns the xor of the signs of src1, src2
 */
int
xor_signs(src1, src2)
    i860_type src1,
     src2;
{
#if FPDEBUG1
    dprintf("Entering xor_signs\n");
#endif
    if (*IEEE_status_ptr & S_PRECISION_BIT)
	return (src1.b_double.sign ^ src2.b_double.sign);
    return (src1.b_single.sign ^ src2.b_single.sign);
}


/* 
 * get_from_fpreg
 * used to get a specified source out of the fregs array of FPtrapcontrol
 */
int 
get_from_fpreg(reg_num, i860_ptr, A_part_of_dualop)
    word32 reg_num;
    register i860_type *i860_ptr;
    int A_part_of_dualop;
{
    register int index;
#if FPDEBUG1
    dprintf("Entering get_from_fpreg\n");
#endif


    i860_ptr->n_double = 0;
    if (reg_num < 2)
	return;
    index = (reg_num / 2) - 1;
#if FPDEBUG2
    printf("index is %lx status word is %lx\n", index, *IEEE_status_ptr);
#endif
    if ((*IEEE_status_ptr & S_PRECISION_BIT) ||
	    (A_part_of_dualop && (*IEEE_status_ptr & A_PRECISION_BIT)))
	i860_ptr->two_words = FPptr->fpreg[index].two_words;
    else {
	if (reg_num % 2) {
	    i860_ptr->two_words.bottom_word32 =
		FPptr->fpreg[index].two_words.top_word32;
#if FPDEBUG2
	    printf("got top\n");
#endif
	} else {
	    i860_ptr->two_words.bottom_word32 =
		FPptr->fpreg[index].two_words.bottom_word32;
#if FPDEBUG2
	    printf("got bottom\n");
#endif
	}
    }
}


/* 
 * get_kr
 * is a function needed for the dpc_table
 */
int 
get_kr(dummy, i860_ptr)
    int dummy;
    i860_type *i860_ptr;
{
    if (*IEEE_status_ptr & S_PRECISION_BIT) {
	*i860_ptr = FPptr->kr;
#ifdef FPDEBUG2
	printf(" KRd "); print_i860type(i860_ptr);
#endif
    } else {
	i860_ptr->two_words.bottom_word32 = FPptr->kr.two_words.bottom_word32 >> 29;
	i860_ptr->two_words.bottom_word32 &= 0x7;
	i860_ptr->two_words.bottom_word32 |= FPptr->kr.two_words.top_word32 << 3;
	i860_ptr->two_words.bottom_word32 &= 0x7fffffff;
	i860_ptr->two_words.bottom_word32 |= FPptr->kr.two_words.top_word32 & 0x80000000;
	i860_ptr->two_words.top_word32 = 0;
#ifdef FPDEBUG2
	printf(" KRs "); print_i860type(i860_ptr);
#endif
    }
}


/* 
 * get_ki
 * is a function needed for the dpc_table
 */
int 
get_ki(dummy, i860_ptr)
    int dummy;
    i860_type *i860_ptr;
{
    if (*IEEE_status_ptr & S_PRECISION_BIT) {
	*i860_ptr = FPptr->ki;
#ifdef FPDEBUG2
	printf(" KId "); print_i860type(i860_ptr);
#endif
    } else {
	i860_ptr->two_words.bottom_word32 = FPptr->ki.two_words.bottom_word32 >> 29;
	i860_ptr->two_words.bottom_word32 &= 0x7;
	i860_ptr->two_words.bottom_word32 |= FPptr->ki.two_words.top_word32 << 3;
	i860_ptr->two_words.bottom_word32 &= 0x7fffffff;
	i860_ptr->two_words.bottom_word32 |= FPptr->ki.two_words.top_word32 & 0x80000000;
	i860_ptr->two_words.top_word32 = 0;
#ifdef FPDEBUG2
	printf(" KIs "); print_i860type(i860_ptr);
#endif
    }
}


/* 
 * get_t
 * a function needed for the dpc_table
 */
int 
get_t(dummy, i860_ptr)
    int dummy;
    i860_type *i860_ptr;
{
    if (*IEEE_status_ptr & R_PRECISION_BIT) {
	*i860_ptr = FPptr->t;
#ifdef FPDEBUG2
	printf(" Td "); print_i860type(i860_ptr);
#endif
    } else {
	i860_ptr->two_words.bottom_word32 = FPptr->t.two_words.bottom_word32 >> 29;
	i860_ptr->two_words.bottom_word32 &= 0x7;
	i860_ptr->two_words.bottom_word32 |= FPptr->t.two_words.top_word32 << 3;
	i860_ptr->two_words.bottom_word32 &= 0x7fffffff;
	i860_ptr->two_words.bottom_word32 |= FPptr->t.two_words.top_word32 & 0x80000000;
	i860_ptr->two_words.top_word32 = 0;
#ifdef FPDEBUG2
	printf(" Ts "); print_i860type(i860_ptr);
#endif
    }
}


/* 
 * get_mres2
 * a function needed for the dpc_table
 */
int 
get_mres2(dummy, i860_ptr)
    int dummy;
    i860_type *i860_ptr;
{
    *i860_ptr = FPptr->mres[2];
#ifdef FPDEBUG2
    printf(" M "); print_i860type(i860_ptr);
#endif
}


/* 
 * get_ares2
 * a function needed for the dpc_table
 */
int 
get_ares2(dummy, i860_ptr)
    int dummy;
    i860_type *i860_ptr;
{
    *i860_ptr = FPptr->ares[2];
#ifdef FPDEBUG2
    printf(" A "); print_i860type(i860_ptr);
#endif
}


/* 
 * place_in_fpreg
 * used to place a result in to the fregs array of the FPtrapcontrol
 */
void
place_in_fpreg(dest, result, which_unit, on_last_stage_prec)
    register word32 dest;
    int which_unit, on_last_stage_prec;
    i860_type *result;
{
    register int index;
    unsigned precision_bit;
#if FPDEBUG1
    dprintf("Entering place_in_fpreg "); print_i860type(result);
#endif
    if (which_unit == A_UNIT)
	precision_bit = A_UNIT_PRECISION_BIT;
    else if (which_unit == M_UNIT)
	precision_bit = M_UNIT_PRECISION_BIT;


    if (dest < 2)
	return;
    index = (dest / 2) - 1;
#if FPDEBUG2
    printf("index is %lx\n", index);
#endif
    if ((on_last_stage_prec && (FPptr->fsrs[2] & precision_bit)) ||
	  (!on_last_stage_prec && (*IEEE_status_ptr & R_PRECISION_BIT))) {
#if FPDEBUG2
	printf("got all ");
	print_i860type(result);
#endif
	FPptr->fpreg[index].two_words = result->two_words;
    } else {
	if (dest % 2) {
#if FPDEBUG2
	    printf("got top "); print_i860type(result);
#endif
	    FPptr->fpreg[index].two_words.top_word32 =
		result->two_words.bottom_word32;
	} else {
#if FPDEBUG2
	    printf("got bottom "); print_i860type(result);
#endif
	    FPptr->fpreg[index].two_words.bottom_word32 =
		result->two_words.bottom_word32;
	}
    }
}


/*
 * determine_inxct_rnd
 * is used by denormalize to determine if inexactness will occur during
 * denormalization (ie at least one bit whose value is 1 will be shifted
 * out). also determines if a 1 should be added to the mantissa after
 * denormalization.
 * 
 * inxctnss is set to 1 if inexactness will occur. post_rounding is set to 1
 * if a 1 should be added.
 */
/*
 * DKG This function is also used by scale_back, to determine if there are
 * any lost bits, and to take necessary actions, i.e. add one to the
 * mantissa after shifting The additional input is from_scale_back : 1- if
 * this function is called from scale_back 0- otherwise. last_bit : This
 * function might be called twice in computing a result by reexecutuing
 * the instruction(e.g. mul_func). The first time would be from
 * denormalize if in computing the result (e.g. do_fmul) one reveals that
 * an underflow occured. If so, one calls denoramlize which calls this
 * function. It might be, that by the denormalize process one looses a 1
 * bit when shifting right, and not adding it to the mantissa when
 * rounding mode isn't TO_ZERO. If the rounding mode is TO_NEAREST this
 * bit might still have significance, and so last_bit is set to one when
 * in denoramlize one looses a bit, but one does not add one to the
 * mantissa The second time would be from scale_back, again checking for
 * loosing a bit when shifting right. The intricate case is that when
 * loosing a bit with rounding TO_NEAR there is a case when one has to
 * take in account the previos lost bit, and that is the role of last_bit.
 * 
 * Another difference with this function relating to who calls it, is with
 * the rounding mode TO_NEAREST. This is when one shifts right and
 * actually the lsb of the exponent is an original mantissa bit (e.g.
 * denormals).
 */

void
determine_inxct_rnd(inxctnss, post_rounding, precision, num_to_shift, i860_num,
		    rounding_mode, from_scale_back, last_bit)
    int *inxctnss, *post_rounding, precision;
    register num_to_shift, rounding_mode;
    i860_type i860_num;
    int from_scale_back, last_bit;			/* DKG */
{
    *inxctnss = *post_rounding = FALSE;
#ifdef FPDEBUG2
printf("d_i_r prec %lx, num2 %lx, rnd %lx\n",precision,num_to_shift,rounding_mode);
printf("from_sb %lx, last_bit %lx\n",from_scale_back, last_bit);
#endif
    if (precision == DOUBLE) {
	word32 lsb_32;		/* 32 least significant bits of mantissa */
	word32 msb_20;		/* 20 most  significant bits of mantissa */
	lsb_32 = i860_num.two_words.bottom_word32;
	msb_20 = i860_num.two_words.top_word32 & 0x00fffff;

	if (num_to_shift < SIZE32 /* size of lsb_32 */ ) {
	    register word32 bits_lost;
	    word32 msb_of_bits_lost;	/* has a 1 in bit #(num_to_shift-1)
					 * 0 elsewhere */
	    msb_of_bits_lost = 0x01;	/* not yet*/
	    bits_lost = lsb_32;
	    bits_lost = (bits_lost << (SIZE32 - num_to_shift)) >>
		(SIZE32 - num_to_shift);
	    if (bits_lost) {
		*inxctnss = TRUE;
		switch (rounding_mode) {

		case TO_ZERO:
		break;

		case DOWNWARD:
		    if (i860_num.b_double.sign)
			*post_rounding = TRUE;
		break;

		case UPWARD:
		    if (!(i860_num.b_double.sign))
			*post_rounding = TRUE;
		break;

		case TO_NEAREST:
		    msb_of_bits_lost = msb_of_bits_lost << (num_to_shift - 1);
		    if (bits_lost > msb_of_bits_lost)
			*post_rounding = TRUE;
		    else if (bits_lost == msb_of_bits_lost) {
			if ((msb_of_bits_lost << 1) & i860_num.two_words.bottom_word32)
			    *post_rounding = TRUE;
		    }
		    if ((from_scale_back) && (last_bit))
			*post_rounding = TRUE;	/* DKG */
		break;
		}
	    }
	} else {
	    word32 bits_lost;
	    word32 msb_of_bits_lost;	/* has a 1 in bit #(num_to_shift-1)
					 * 0 elsewhere */
	    msb_of_bits_lost = 0x01;	/* not yet */
	    if (num_to_shift == SIZE32)
		bits_lost = 0x0;
	    else
		bits_lost = (msb_20 << (SIZE64 - num_to_shift)) >>
		    (SIZE64 - num_to_shift);
#if FPDEBUG
	    dprintf("msb_20 %lx bits_lost %lx lsb_32 %lx\n",
		   msb_20, bits_lost, lsb_32);
#endif
	    if (bits_lost || lsb_32) {
		*inxctnss = TRUE;
		switch (rounding_mode) {

		case TO_ZERO:
		break;

		case DOWNWARD:
		    if (i860_num.b_double.sign)
			*post_rounding = TRUE;
		break;

		case UPWARD:
		    if (!(i860_num.b_double.sign))
			*post_rounding = TRUE;
		break;

		case TO_NEAREST:
		    msb_of_bits_lost = msb_of_bits_lost << (num_to_shift - 1);
		    if ((bits_lost > msb_of_bits_lost) ||
			    ((bits_lost == msb_of_bits_lost) && lsb_32))
			*post_rounding = TRUE;
		    else if ((bits_lost == msb_of_bits_lost) && ((num_to_shift !=
				   MANTISSA_SIZE_D) || from_scale_back)) { /* DKG */
			if ((msb_of_bits_lost << 1) & i860_num.two_words.top_word32)
			    *post_rounding = TRUE;
		    }
		    if ((from_scale_back) && (last_bit))
			*post_rounding = TRUE;	/* DKG */
		break;
		}
	    }
	}

    } else { /* SINGLE */
	word32 bits_lost;	/* the least significant num_to_shift bits */
	word32 msb_of_bits_lost;/* has a 1 in bit #(num_to_shift-1) 0
				 * elsewhere */
	msb_of_bits_lost = 0x01;/* not yet */

	bits_lost = i860_num.two_words.bottom_word32;
	bits_lost &= SINGLE_MANTISSA_BITS;
#if FPDEBUG
	dprintf("bits_lost %lx num_to_shift %lx\n", bits_lost, num_to_shift);
#endif
	if (num_to_shift < SIZE32 ) { /* RAG begin */
	    bits_lost = (bits_lost << (SIZE32 - num_to_shift)) >>
	    (SIZE32 - num_to_shift);
	} else {
	    bits_lost = 0;
	} /* RAG end */
#if FPDEBUG
	dprintf("bits_lost %lx \n", bits_lost);
#endif
	if (bits_lost) {
	    *inxctnss = TRUE;
	    switch (rounding_mode) {

	    case TO_ZERO:
	    break;

	    case DOWNWARD:
		if (i860_num.b_single.sign)
		    *post_rounding = TRUE;
	    break;

	    case UPWARD:
		if (!(i860_num.b_single.sign))
		    *post_rounding = TRUE;
	    break;

	    case TO_NEAREST:
		msb_of_bits_lost = msb_of_bits_lost << (num_to_shift - 1);
		if (bits_lost > msb_of_bits_lost) {
		    *post_rounding = TRUE;
		} else if ((bits_lost == msb_of_bits_lost) && ((num_to_shift !=
				   MANTISSA_SIZE_S) || from_scale_back)) {	/* DKG */
		    if ((msb_of_bits_lost << 1) & i860_num.two_words.bottom_word32) {
			*post_rounding = TRUE;
			}
		} else if( bits_lost && (num_to_shift == (MANTISSA_SIZE_S+1))) {
		    *post_rounding = TRUE;	/* RAG */
		}
		if ((from_scale_back) && (last_bit)) {
		    *post_rounding = TRUE;	/* DKG */
		}
	    break;
	    }
	}
    }
#if FPDEBUG
dprintf("post_rounding %lx inexact %lx\n",*post_rounding,*inxctnss);
#endif
}


/* 
 * is_denormal
 * returns 1 if num is a denormal 0 otherwise
 */
int
is_denormal(num, source_precision)
    i860_type num;
    int source_precision;
{
    if (source_precision) {
	if (num.b_double.exponent == 0 && !(num.b_double.mantissa_a == 0 &&
					    num.b_double.mantissa_b == 0 &&
					    num.b_double.mantissa_c == 0))
	    return (1);
	else
	    return (0);
    } else {
	if (num.b_single.exponent == 0 && !(num.b_single.mantissa_a == 0 &&
					    num.b_single.mantissa_b == 0))
	    return (1);
	else
	    return (0);
    }
}


/*
 * do_a_r_shift_d
 * shifts a double precision mantissa 1 bit to the right
 */
void
do_a_r_shift_d(num)
    register i860_type *num;
{
    int shifted_out_1;
    if (num->two_words.top_word32 & 0x01)
	shifted_out_1 = TRUE;
    else
	shifted_out_1 = FALSE;
    num->two_words.top_word32 = num->two_words.top_word32 >> 1;
    num->two_words.bottom_word32 = num->two_words.bottom_word32 >> 1;
    if (shifted_out_1)
	num->two_words.bottom_word32 |= 0x80000000;
#if FPDEBUG2
    printf("In do_a_r_shift normalized num is "); print_i860type(num);
#endif
}


/*
 * do_a_l_shift_d
 * shifts a double precision mantissa 1 bit to the left
 */
void
do_a_l_shift_d(num)	
    register i860_type *num;
{
    int shifted_out_1;
#if FPDEBUG2
    printf("Enter do_a_l_shift_d");
#endif
    if (num->two_words.bottom_word32 & 0x80000000) {
#if FPDEBUG2
	printf(" nshifted out 1 is true ");
#endif
	shifted_out_1 = TRUE;
    } else
	shifted_out_1 = FALSE;
    num->two_words.bottom_word32 = num->two_words.bottom_word32 << 1;
    num->two_words.top_word32 = num->two_words.top_word32 << 1;
    if (shifted_out_1)
	num->two_words.top_word32 |= 0x1;
#if FPDEBUG2
    printf(" After the shifting "); print_i860type(num);
#endif
}


/*
 * normalize
 * noramlizes a denormal, returns number of left shifts done
 */
int 
normalize(num, source_precision)
    register i860_type *num;
    int source_precision;
{
    int num_shifted,
     sign;
#if FPDEBUG2
    printf("In normalize  num is "); print_i860type(num);
#endif
    num_shifted = 0;
    if (source_precision) {
	sign = num->b_double.sign;
	while (!num->b_double.mantissa_a) {
	    /* do a shift */
	    do_a_l_shift_d(num);
	    num_shifted++;
	}
	do_a_l_shift_d(num);	/* now we have an implicit 1 preceding
				 * binary point */
	num->b_double.exponent = 1;
	num->b_double.sign = sign;
	num_shifted++;
#if FPDEBUG2
	printf(" num shifted is %lx ", num_shifted);
#endif
    } else {
	sign = num->b_single.sign;
	while (!num->b_single.mantissa_a) {
	    /* do a shift */
	    num->two_words.bottom_word32 = num->two_words.bottom_word32 << 1;
	    num_shifted++;
	}
	num->two_words.bottom_word32 = num->two_words.bottom_word32 << 1;
	num->b_single.exponent = 1;
	num->b_single.sign = sign;
	num_shifted++;
    }
#if FPDEBUG2
    printf("In normalize normalized num is "); print_i860type(num);
#endif
    return (num_shifted);
}


/*
 * is used to turn two denormals in to normals. the number of left shifts
 * that take place is the max(#needed to normalize *num1, #needed to
 * normalize *num2) this number is returned by the procedure
 */
int 
scale_up(num1, num2, source_precision)
    register i860_type *num1,
    *num2;
    int source_precision;
{
    word32 num_shifted1,
     num_shifted2,
     increase;
    i860_type *num_to_increase_exponent;
    num_shifted1 = normalize(num1, source_precision);
    num_shifted2 = normalize(num2, source_precision);
#if FPDEBUG2
    printf(" in scale up after normalizing both %lx %lx\n", num_shifted1,
	  num_shifted2);
    print_i860type(num1);
    print_i860type(num2);
#endif
    if (num_shifted1 != num_shifted2) {
	if (num_shifted1 > num_shifted2) {
	    num_to_increase_exponent = num2;
	    increase = num_shifted1 - num_shifted2;
	} else {
	    num_to_increase_exponent = num1;
	    increase = num_shifted2 - num_shifted1;
	}
#if FPDEBUG2
	printf("increases not the same num to increase is %lx\n", increase);
#endif
	if (source_precision)
	    num_to_increase_exponent->b_double.exponent += increase;
	else
	    num_to_increase_exponent->b_single.exponent += increase;
	return ((num_shifted1 > num_shifted2) ? num_shifted1 : num_shifted2);
    } else
	return (num_shifted1);
}


/* 
 * do_fmul
 * is a function needed for the dpc_table
 */
i860_type 
do_fmul(src1, src2, s_prec, r_prec, underflow_occured, inexact_mul_func)
    i860_type src1, src2;
    int s_prec, r_prec, *underflow_occured, *inexact_mul_func;
{
    i860_type denormalize();
    int inexactness_occured = 0;
    i860_type result;
    result.n_double = 0;
#if FPDEBUG
    dprintf(" entering do_fmul src1 src2 s %lx r %lx\n", s_prec, r_prec);
    print_i860type(&src1); print_i860type(&src2);
#endif
    if (r_prec)			/* double */
	if (s_prec)
	    result.n_double = src1.n_double * src2.n_double;
	else
	    result.n_double = src1.n_single * src2.n_single;
    else if (s_prec)		/* DKG is this right ????? */
	result.n_single = src1.n_double * src2.n_double;
    else
	result.n_single = src1.n_single * src2.n_single;
/*
*    fsr |= (get_fsr() & M_INEXACT_OCCURED_BIT);
*/

    fsr = get_fsr();
#ifdef FPDEBUG2
    printf("fsr after do_fmul/get_fsr %lx\n",fsr);
#endif

    if (fsr & M_UNDERFLOW_OCCURED_BIT) {
	result = denormalize(M_UNIT, &inexactness_occured, 1, result, r_prec);
	*underflow_occured = 1;
	*inexact_mul_func = inexactness_occured;
	if (inexactness_occured) {
	    fsr |= M_INEXACT_BIT;
#if FPDEBUG2
	    printf("from denormalize inexcatness is %lx\n", inexactness_occured);
#endif
        }
        fsr &= ~M_UNDERFLOW_OCCURED_BIT;
    }
/* DKG
* no need to this because one calls denormalize if underflow occured
*   if(is_denormal(result,s_prec))
*    {
* #if FPDEBUG1
* printf("to_i860_underflow done\n");
* #endif
*        to_i860_underflow(&result,s_prec);
*        fsr |= M_UNDERFLOW_OCCURED_BIT;
*    }
*/
#if FPDEBUG
    dprintf("result from do_fmul is "); print_i860type(&result);
#endif
    return (result);
}


/* 
 * do_frcp
 * is a function needed for the dpc_table
 */
i860_type 
do_frcp(src2, s_prec, r_prec)
    i860_type src2;
    int s_prec,
     r_prec;
{
    i860_type result;
    result.n_double = 0;
#if FPDEBUG1
    dprintf("Entering do_frcp src2 "); print_i860type(&src2);
#endif
    if (r_prec)
	if (s_prec)
	    result.n_double = 1 / src2.n_double;
	else
	    result.n_double = 1 / src2.n_single;
    else if (s_prec)
	result.n_single = 1 / src2.n_double;
    else
	result.n_single = 1 / src2.n_single;
/*    fsr |= (get_fsr() & M_INEXACT_OCCURED_BIT); */
    fsr = get_fsr();
#ifdef FPDEBUG2
    printf("fsr after do_frcp/get_fsr %lx\n",fsr);
#endif
    return (result);
}


/* 
 * do_frsqr
 * is a function needed for the dpc_table
 */
i860_type 
do_frsqr(src2, s_prec, r_prec)
    i860_type src2;
    int s_prec, r_prec;
{
    double sqrtdd();
    float sqrtss();
    double sqrtsd();
    i860_type result;
    result.n_double = 0;
#if FPDEBUG1
    dprintf("Entering do_frsqr src2 "); print_i860type(&src2);
#endif
    if (r_prec) {
	if (s_prec) {
	    result.n_double = sqrtdd(src2.n_double);
	} else {
	    result.n_double = sqrtsd(src2.n_single);
	}
    } else {
	result.n_single = sqrtss(src2.n_single);
    }
    fsr = get_fsr();
#ifdef FPDEBUG2
    printf("fsr after do_frsqrt/get_fsr %lx\n",fsr);
    printf("and the result is "); print_i860type(&result);
#endif
    return (result);
}


/*
 * do_fadd
 * is a function needed for the dpc_table
 */
i860_type 
do_fadd(src1, src2, s_prec, r_prec, underflow_occured, inxctness)
    i860_type src1,
     src2;
    int s_prec,
     r_prec,
    *underflow_occured,
    *inxctness;
{
    i860_type denormalize();
    int inexactness_occured = 0;
    i860_type result;
#if FPDEBUG1
    dprintf("In do_fadd src1 src2 s %lx r %lx\n", s_prec, r_prec);
    print_i860type(&src1);
    print_i860type(&src2);
#endif
    result.n_double = 0;


    if (r_prec) {
	if (s_prec)
	    result.n_double = src1.n_double + src2.n_double;
	else
	    result.n_double = src1.n_single + src2.n_single;
    } else {
	if (s_prec)
	    result.n_single = src1.n_double + src2.n_double;
	else
	    result.n_single = src1.n_single + src2.n_single;
    }


/*
   fsr |= (get_fsr() & ( A_INEXACT_OCCURED_BIT | A_UNDERFLOW_OCCURED_BIT));
*/
    fsr = get_fsr();
#if FPDEBUG1
    dprintf("fsr after do_fadd/get_fsr %lx\n",fsr);
    dprintf("in do_fadd after bare addition done "); print_i860type(&result);
    dprintf("result precision is %lx\n", r_prec);
#endif
    if (fsr & A_UNDERFLOW_OCCURED_BIT) {
	result = denormalize(A_UNIT, &inexactness_occured, 1, result, r_prec);
	*underflow_occured = 1;
	*inxctness = inexactness_occured;
	if (inexactness_occured)
	    fsr |= A_INEXACT_BIT;
    }
/*DKG
*    if(is_denormal(result,s_prec))
*    {
* #if FPDEBUG
* printf("to_i860_underflow done\n");
* #endif
*        to_i860_underflow(&result,s_prec);
*        fsr |= A_UNDERFLOW_OCCURED_BIT;
*        if(!s_prec)
*            fsr | = 0x1c00000;
*    }
*/
#if FPDEBUG1
    dprintf("result from do_fadd is "); print_i860type(&result);
#endif
    return (result);
}


/*
 * do_fsub
 * is a function needed for the dpc_table
 */
i860_type 
do_fsub(src1, src2, s_prec, r_prec, underflow_occured, inxctness)
    i860_type src1,
     src2;
    int s_prec,
     r_prec,
    *underflow_occured,
    *inxctness;
{
    int inexactness_occured = 0;
    i860_type result;
    result.n_double = 0;
#if FPDEBUG1
    dprintf(" entering do_fsub src1 src2 s %lx r %lx\n", s_prec, r_prec);
    print_i860type(&src1); print_i860type(&src2);
#endif
    if (r_prec)
	if (s_prec)
	    result.n_double = src1.n_double - src2.n_double;
	else
	    result.n_double = src1.n_single - src2.n_single;
    else if (s_prec)
	result.n_single = src1.n_double - src2.n_double;
    else
	result.n_single = src1.n_single - src2.n_single;

    fsr = get_fsr();
#ifdef FPDEBUG2
    printf("fsr after do_fsub/get_fsr %lx\n",fsr);
#endif
/*DKG | */

    if (fsr & A_UNDERFLOW_OCCURED_BIT) {
	result = denormalize(A_UNIT, &inexactness_occured, 1, result, r_prec);
	*underflow_occured = 1;
	*inxctness = inexactness_occured;
	if (inexactness_occured)
	    fsr |= A_INEXACT_BIT;
    }
/*   DKG
*if(is_denormal(result,s_prec))
*    {
* #if FPDEBUG1
* printf("to_i860_underflow done\n");
* #endif
*        to_i860_underflow(&result,s_prec);
*        fsr |= A_UNDERFLOW_OCCURED_BIT;
*        if(!s_prec)
*            fsr | = 0x1c00000;
*    }
*/
#if FPDEBUG1
    dprintf("result from do_fsub is "); print_i860type(&result);
#endif
    return (result);
}


/*
 * scale_back
 * this procedure takes a number an divides it by 2**num_shifted if the
 * division results in underflow the result returned will be the usual
 * i860 result for underflow, i.e. the mantissa is the true mantissa and
 * the exponent is the low order bits of the true exponent. also in this
 * case underflow_occured will be set to 1
 */
/*
 * DKG Update: If there are bits lost when right shifting is done, add one
 * to the mantissa if needed
 */

scale_back(num, num_shifted, r_prec, underflow_occured, which_unit,
	rounding_mode, inexact)
    register i860_type *num;
    int num_shifted, r_prec, underflow_occured, which_unit, rounding_mode,
    	inexact;
{
    int i, sign, new_exponent;
    register word32 fsr_ae_bits;
    int add_one_to_mantissa(), last_bit;
    int scale_b_inexact = 0;
    int post_rounding = 0;	/* DKG */
    int tnum_shifted;		/* DKG */
    i860_type temp_num;
#if FPDEBUG2
    printf("In scale_back with r_prec == %lx\n", r_prec);
    printf("In scale_back with inexact == %lx\n", inexact);
    printf("In scale_back with numshifted == %lx\n", num_shifted);
    printf("In scale_back with underflow == %lx\n", underflow_occured);
#endif
    if (rounding_mode == TO_NEAREST)
	last_bit = inexact;
    else
	last_bit = 0;

    temp_num.two_words.bottom_word32 = num->two_words.bottom_word32;
    temp_num.two_words.top_word32 = num->two_words.top_word32;

    if (r_prec) {		/* double */
	if (num->n_double == 0 && !(underflow_occured))
	    return;
	if (underflow_occured)
/*
TRying a bug fix - 1.11.89        ||
        			(num_shifted == num->b_double.exponent))
*/
	{
	    sign = num->b_double.sign;
	    num->b_double.sign = 0;
	    new_exponent = 0;
	    determine_inxct_rnd(&scale_b_inexact, &post_rounding, r_prec,
		    num_shifted, temp_num, rounding_mode, TRUE, last_bit);
	    for (i = 0; i < num_shifted; i++)
		do_a_r_shift_d(num);
	    num->b_double.sign = sign;
	} else {
	    if (num_shifted >= num->b_double.exponent) {
		sign = num->b_double.sign;
		num->b_double.sign = 0;
		new_exponent = 0;
		tnum_shifted = num_shifted - num->b_double.exponent + 1;
		num->b_double.exponent = 1;
		temp_num.b_double.exponent = 1;
		determine_inxct_rnd(&scale_b_inexact, &post_rounding, r_prec,
		   tnum_shifted, temp_num, rounding_mode, TRUE, last_bit);
		for (i = 0; i < tnum_shifted; i++)
		    do_a_r_shift_d(num);
		num->b_double.sign = sign;
	    } else
		new_exponent = ((int) (num->b_double.exponent)) - num_shifted;
	}
	/* 11 bits of exponent */
	num->b_double.exponent = (word32) (new_exponent) & 0x07ff;


    } else {			/* single */
	if (num->n_single == 0 && !(underflow_occured))
	    return;

	/* DKG previously underflow_occ && which_unit == A_UNIT */

	if ((underflow_occured) ||
		(num_shifted == num->b_single.exponent)) {
#if FPDEBUG2
  printf(" num_shifted %lx exp %lx\n",num_shifted,num->b_single.exponent);
#endif
/* DKG , not needed because denormalize allready executed
*
*            tmp_exp = (fsr & AE_BITS) >> AE_SHIFT;
*            tmp_exp |= ((num->two_words.bottom_word32 & SINGLE_EXP_BITS) >> MANTISSA_SIZE_S);
*            new_exponent = ((int)(tmp_exp)) - num_shifted;
*/
/* roger golliver made changes 3/22/91 started here */
	    sign = num->b_single.sign;
	    num->b_single.sign = 0;
	    new_exponent = 0;

	    if(num->b_single.exponent) { /* not already denormal */
	       tnum_shifted = 1;
	       num->b_single.exponent = 1; /* the hidden bit */

	       temp_num.b_single.exponent = 1;
	    } else {
	       tnum_shifted = num_shifted;
	    }

	    determine_inxct_rnd(&scale_b_inexact, &post_rounding, r_prec,
		    num_shifted, temp_num, rounding_mode, TRUE, last_bit);
	    num->two_words.bottom_word32 = num->two_words.bottom_word32 >>
		tnum_shifted;
/* roger golliver made changes 3/22/91 ended here */

	    num->b_single.sign = sign;
	} else if (num_shifted > num->b_single.exponent) {
#if FPDEBUG2
  printf(" num_shifted %lx exp %lx\n",num_shifted,num->b_single.exponent);
#endif
	    sign = num->b_single.sign;
	    num->b_single.sign = 0;
	    new_exponent = 0;

/**DKG *** added the next 6 lines*/

	    tnum_shifted = num_shifted - num->b_single.exponent + 1;
	    num->b_single.exponent = 1;

	    temp_num.b_single.exponent = 1;
	    determine_inxct_rnd(&scale_b_inexact, &post_rounding, r_prec,
		   tnum_shifted, temp_num, rounding_mode, TRUE, last_bit);
	    num->two_words.bottom_word32 = num->two_words.bottom_word32 >>
		tnum_shifted;
/* DKG ,it works without errors this way according to the test*/
/*
* This is the original code:
*				num->two_words.bottom_word32 = num->two_words.bottom_word32 >>
*					(num_shifted - (num->b_single.exponent-1));
*/

	    num->b_single.sign = sign;
	} else {
#if FPDEBUG2
  printf(" num_shifted %lx exp %lx\n",num_shifted,num->b_single.exponent);
#endif
	    new_exponent = ((int) (num->b_single.exponent)) - num_shifted;
	}
	num->b_single.exponent = (word32) (new_exponent) & 0x0ff;	/* eight lsb of true
									 * exponent */
#if FPDEBUG2
	printf("the new exponent is %lx %lx %lx\n",
	      num->b_single.exponent, num_shifted, ((word32) new_exponent & 0x7ff));
#endif


    }				/* end single */
    if (new_exponent <= 0 || underflow_occured) {	/* underflow occurs */

/*DKG
        *underflow_occured = 1;
*/
	if (which_unit == A_UNIT) {
	    /* set AE bits of fsr */
	    fsr_ae_bits = ((word32) (new_exponent) & 0x0700) << AE_SHIFT;
	    fsr |= fsr_ae_bits;
#if FPDEBUG2
	    printf("the ae bits are %lx \n", fsr_ae_bits);
#endif
	}
	/* scale back might also cause loosing a bit */
	if (scale_b_inexact)
	    if (which_unit == A_UNIT)
		fsr |= A_INEXACT_BIT;
	    else
		fsr |= M_INEXACT_BIT;


    }
/*
 *  remember the last bit which indicates if one adds one to the mantissa.
 */

    if (post_rounding)
	add_one_to_mantissa(num, r_prec);


#if FPDEBUG2
    printf("The resulted number from scale_back is "); print_i860type(num);
#endif
}


/*
 * will_cause_overflow
 * multiplies a i860_number by 2**num_to_shift, if overflow would be
 * produced by this multiplication then src remains unchanged and a 1 is
 * returned, otherwise src is multiplied by 2**num_to_shift and 0 is
 * returned
 */
int 
will_cause_overflow(num_to_shift, src, s_prec)
    int num_to_shift, s_prec;
    register i860_type *src;
{
    register word32 new_exponent;
    if (s_prec) {
	if (src->n_double == 0)
	    return (0);
	new_exponent = src->b_double.exponent + (word32) num_to_shift;
	if (new_exponent >= 0x07ff)
	    return (1);
	else {
	    src->b_double.exponent = new_exponent;
	    return (0);
	}
    } else {
	if (src->n_single == 0)
	    return (0);
	new_exponent = src->b_single.exponent + (word32) num_to_shift;
#if FPDEBUG2
	printf("in will_cause new exponent is %lx %lx %lx\n",
	      src->b_single.exponent, (word32) num_to_shift, new_exponent);
#endif
	if (new_exponent >= 0x0ff)
	    return (1);
	else {
	    src->b_single.exponent = new_exponent;
	    return (0);
	}
    }
}


/*
 * subtract_one_from_mantissa
 * just what it says
 */
subtract_one_from_mantissa(result, r_prec)
    register i860_type *result;
{
#if FPDEBUG1
    dprintf("entering sub one to mantissa\n");
#if FPDEBUG2
    print_i860type(result);
#endif
#endif
    if (r_prec) {
	result->b_double.mantissa_c -= 0x1;
	if (result->b_double.mantissa_c == (unsigned int) 0xffffffff) {
	    result->b_double.mantissa_b -= 0x1;
	    if (result->b_double.mantissa_b == 0x7ffff) {
		result->b_double.mantissa_a -= 0x1;
		if (result->b_double.mantissa_a == 0x1)
		    result->b_double.exponent -= 1;
	    }
	}
    } else {
	result->b_single.mantissa_b -= 0x1;
	if (result->b_single.mantissa_b == 0x3fffff) {
	    result->b_single.mantissa_a -= 0x1;
	    if (result->b_single.mantissa_a == 0x1) {
		result->b_single.exponent -= 0x1;
	    }
	}
    }

}


/*
 * add_one_to_mantissa
 * just what it says
 */
add_one_to_mantissa(result, r_prec)
    register i860_type *result;
    int r_prec;
{
#if FPDEBUG1
    dprintf("entering add one to mantissa\n");
#if FPDEBUG2
    print_i860type(result);
#endif
#endif
    fsr |= AA_BIT;
    if (r_prec) {
	result->b_double.mantissa_c += 0x1;
	if (result->b_double.mantissa_c == 0x00000000) {
	    result->b_double.mantissa_b += 0x1;
	    if (result->b_double.mantissa_b == 0x00000) {
		result->b_double.mantissa_a += 0x1;
		if (result->b_double.mantissa_a == 0x0)
		    result->b_double.exponent += 0x01;
	    }
	}
    } else {
	result->b_single.mantissa_b += 0x1;
	if (result->b_single.mantissa_b == 0x0) {
	    result->b_single.mantissa_a += 0x1;
	    if (result->b_single.mantissa_a == 0x0) {
		result->b_single.exponent += 0x01;
	    }
	}
    }
}


/*
 * flip_sign
 * just what it says
 */
flip_sign(num, source_precision)
    i860_type *num;
    int source_precision;
{
    if (source_precision)
	num->b_double.sign = (num->b_double.sign) ? 0 : 1;
    else
	num->b_single.sign = (num->b_single.sign) ? 0 : 1;
}


/*
 * flip_sign
 * just what it says
 */
i860_type 
a_unit_func(src1, src2, s_prec, r_prec, which_op)
    i860_type src1,
     src2;
    int s_prec,
     r_prec,
     which_op;
{
    int is_denorm1,
     is_denorm2,
     num_shifted,
     rounding_mode,
     underflow_occured,
     which_src_is_normal,
     ordinary_is_positive,
     denormal_is_positive;
    int inxctnss = 0;
    i860_type result,
     ordinary_source,
     denormal_source,
     do_fsub(),
     do_fadd();
#if FPDEBUG
    dprintf("Entering a_unit_func s_prec %lx r_prec %lx\n", s_prec, r_prec);
    print_i860type(&src1); print_i860type(&src2);
#endif
    rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS) >> ROUNDING_MODE_SHIFT;
    underflow_occured = FALSE;
    result.n_double = 0;
    is_denorm1 = is_denormal(src1, s_prec);
    is_denorm2 = is_denormal(src2, s_prec);


    if (is_denorm1 && is_denorm2) {
#if FPDEBUG2
	printf("both are denormals \n");
#endif
	if (FPptr->fsrs[2] & FZ_BIT)
	    return (result);	/* which is 0 */
	num_shifted = scale_up(&src1, &src2, s_prec);
#if FPDEBUG2
	printf("normalized denormals \n");
	print_i860type(&src1);
	print_i860type(&src2);
#endif
	if (which_op == SUBTRACT)
	    result = do_fsub(src1, src2, s_prec, r_prec, &underflow_occured,
			     &inxctnss);
	else
	    result = do_fadd(src1, src2, s_prec, r_prec, &underflow_occured,
			     &inxctnss);
/*   DKG     fsr = get_fsr(); DKG supposed to remove this */
/*   DKG     if(fsr & A_UNDERFLOW_OCCURED_BIT)
            underflow_occured = TRUE;  */
#if FPDEBUG2
	printf("the result before scaling back is\n");
	print_i860type(&result);
#endif
	scale_back(&result, num_shifted, r_prec, underflow_occured, A_UNIT,
		   rounding_mode, inxctnss);
	if (underflow_occured)
	    fsr |= A_UNDERFLOW_OCCURED_BIT;
#if FPDEBUG2
	printf("The result returned is \n");
	print_i860type(&result);
#endif
    } else if (!is_denorm1 && !is_denorm2) {	/* this is the non
						 * erroneous part of dualop  */
#if FPDEBUG2
	printf("both are not denormals \n");
#endif
	if (which_op == SUBTRACT)
	    result = do_fsub(src1, src2, s_prec, r_prec, &underflow_occured,
			     &inxctnss);
	else
	    result = do_fadd(src1, src2, s_prec, r_prec, &underflow_occured,
			     &inxctnss);
/* DKG        fsr = get_fsr();  supposed to remove */
    } else {			/* only one of them is denormal */
	if (!is_denorm1) {
	    which_src_is_normal = 1;
	    ordinary_source = src1;
	    denormal_source = src2;
	} else {
	    which_src_is_normal = 2;
	    ordinary_source = src2;
	    denormal_source = src1;
	}
#if FPDEBUG2
	printf("one is one isn't denormals \n");
	print_i860type(&ordinary_source);
	print_i860type(&denormal_source);
#endif
	num_shifted = normalize(&denormal_source, s_prec);
#if FPDEBUG2
	printf(" num_shifted is %lx ", num_shifted);
	print_i860type(&denormal_source);
#endif
	if (!will_cause_overflow(num_shifted, &ordinary_source, s_prec)) {
#if FPDEBUG2
	    printf("won't cause overflow\n");
#endif
	    if (which_op == SUBTRACT) {
		if (which_src_is_normal == 1)
		    result = do_fsub(ordinary_source, denormal_source, s_prec, r_prec,
				     &underflow_occured, &inxctnss);
		else
		    result = do_fsub(denormal_source, ordinary_source, s_prec, r_prec,
				     &underflow_occured, &inxctnss);
	    } else
		result = do_fadd(denormal_source, ordinary_source, s_prec, r_prec,
				 &underflow_occured, &inxctnss);
/* DKG            fsr |= get_fsr(); DKG supposed to remove */
#if FPDEBUG2
	    printf("the result before scaling back is\n");
	    print_i860type(&result);
#endif
	    scale_back(&result, num_shifted, r_prec, underflow_occured, A_UNIT,
		       rounding_mode, inxctnss);
	    if (underflow_occured)
		fsr |= A_UNDERFLOW_OCCURED_BIT;
#if FPDEBUG2
	    printf("the result after scaling back is\n");
	    print_i860type(&result);
#endif
	} else {		/* will cause overflow if ordinary number
				 * is shifted */
#if FPDEBUG2
	    printf("will cause overflow\n");
#endif
	    fsr |= A_INEXACT_OCCURED_BIT;
	    fsr |= SI_BIT;
	    if (which_op == SUBTRACT) {	/* change sign of second operand */
		if (which_src_is_normal == 1)
		    flip_sign(&denormal_source, s_prec);
		else
		    flip_sign(&ordinary_source, s_prec);
	    }
	    ordinary_is_positive = is_positive(ordinary_source, s_prec);
	    denormal_is_positive = is_positive(denormal_source, s_prec);
#if FPDEBUG2
	    printf("the ordinary one is\n");
	    print_i860type(&ordinary_source);
	    printf("s_prec %lx r_prec %lx\n", s_prec, r_prec);
#endif
	    if (s_prec != r_prec) {
		result = ordinary_source;
		to_double_precision(&result);
#if FPDEBUG2
		printf("to_double is\n");
		print_i860type(&result);
#endif
	    } else {
		result = ordinary_source;
#if FPDEBUG2
		printf("not to_double\n");
		print_i860type(&result);
#endif
	    }
#if FPDEBUG2
	    printf("rounding mode %lx\n", rounding_mode);
	    printf("result is");
	    print_i860type(&result);
#endif
	    switch (rounding_mode) {
	    case TO_ZERO:
		if (ordinary_is_positive != denormal_is_positive) {
		    subtract_one_from_mantissa(&result, r_prec);
		}
	    break;

	    case DOWNWARD:
		if (denormal_is_positive);
		/*
		 * the result will be just the ordinary (non denormal)
		 * number
		 */
		else if (ordinary_is_positive) {	/* denormal is negative */
		    subtract_one_from_mantissa(&result, r_prec);
		} else
		    /* ordinary is negative denormal is negative */
		{
		    add_one_to_mantissa(&result, r_prec);
		}
	    break;

	    case UPWARD:
#if FPDEBUG2
		printf("rounding mode is upward denormal_is_positive %lx ord_is_p %lx\n",
		      denormal_is_positive, ordinary_is_positive);
#endif
		if (!denormal_is_positive);
		/*
		 * the result will be just the ordinary (non denormal)
		 * number
		 */
		else if (ordinary_is_positive) { /* denormal is positive */
		    add_one_to_mantissa(&result, r_prec);
		} else
		    /* ordinary is negative denormal is positive */
		{
		    subtract_one_from_mantissa(&result, r_prec);
		}
	    break;

	    case TO_NEAREST:
		;
		/*
		 * the result will be just the ordinary (non denormal)
		 * number
		 */
	    break;
	    }
	    if (is_infinity(result, r_prec))
		fsr |= A_OVERFLOW_OCCURED_BIT;
	}
    }
    return (result);
}


/*
 * add_func
 * used by func_table
 */
i860_type 
add_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    return (a_unit_func(src1, src2, s_prec, r_prec, ADDITION));
}


/*
 * sub_func
 * used by func_table
 */
i860_type 
sub_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    return (a_unit_func(src1, src2, s_prec, r_prec, SUBTRACT));
}


/*
 * mul_func
 * used by func_table
 */
i860_type 
mul_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    i860_type result, ordinary_source, denormal_source, do_fmul();
    int src1_is_denormal, src2_is_denormal, num_shifted, underflow_occured;
    int rounding_mode;		/* DKG */
    int inexact_mul_func = 0;	/* DKG */
    underflow_occured = FALSE;
    result.n_double = 0;
#if FPDEBUG
    dprintf("entering mul_func\n");
#endif
    src1_is_denormal = is_denormal(src1, s_prec);
    src2_is_denormal = is_denormal(src2, s_prec);

    if (src1_is_denormal && src2_is_denormal) {
	/*
	 * underflow always occurs on multiplication of two denormals
	 */
	if (FPptr->fsrs[2] & FZ_BIT)
	    return (result);	/* which is 0 */
/* -- not needed roger golliver, caused double execptions with bad result reg
 *	fsr |= M_UNDERFLOW_OCCURED_BIT;
 */
	num_shifted = 2 * (scale_up(&src1, &src2, s_prec));
	/* (2*) since both are shifted and this is multiplication */
	result = do_fmul(src1, src2, s_prec, r_prec, &underflow_occured,
			 &inexact_mul_func);
/*        fsr = fsr | get_fsr(); DKG  probably not needed*/
	/*
	 * DKG- do_fmul will find out that an underflow occured *
	 * if(r_prec) result.b_double.exponent -= num_shifted; else
	 * result.b_single.exponent -= num_shifted;
	 */
    } else if (!src1_is_denormal && !src2_is_denormal) {
	result = do_fmul(src1, src2, s_prec, r_prec, &underflow_occured,
			 &inexact_mul_func);
/*        fsr = fsr | get_fsr(); DKG check this too */
    } else {
	if (!src1_is_denormal) {
	    ordinary_source = src1;
	    denormal_source = src2;
	} else {
	    ordinary_source = src2;
	    denormal_source = src1;
	}

	num_shifted = normalize(&denormal_source, s_prec);
	result = do_fmul(ordinary_source, denormal_source,
		   s_prec, r_prec, &underflow_occured, &inexact_mul_func);
#if FPDEBUG2
	printf("the result before scaleback (underflow_occur %lx)\n",
		underflow_occured);
	print_i860type(&result);
#endif
/*        fsr |= get_fsr(); DKG check this too */

	/* DKG rounding mode for correct mantissa */
	rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS) >> ROUNDING_MODE_SHIFT;

	scale_back(&result, num_shifted, r_prec, underflow_occured, M_UNIT,
		   rounding_mode, inexact_mul_func);
	/* scale back might also cause underflow not known till now */
	/* rag -- added is_denormal check to trigger FZ_BIT test */
	if (underflow_occured || is_denormal(result, s_prec) ) {
	    if (FPptr->fsrs[2] & FZ_BIT) {
		result.n_double = 0;
		return (result);/* which is 0 */
	    }
/* -- not needed roger golliver, caused double execptions with bad result reg
 *	    fsr |= M_UNDERFLOW_OCCURED_BIT;
 */
	}
    }
    return (result);

}


/*
 * do_famov
 * used by func_table
 */
i860_type 
do_famov(src1, src2, s_prec, r_prec)
    i860_type src1,
     src2;
    int s_prec,
     r_prec;
{
    i860_type result;
    int sign,
     rounding_mode,
     num_shifted,
     i;
    result.two_words.top_word32 = src1.two_words.top_word32;
    result.two_words.bottom_word32 = src1.two_words.bottom_word32;

    if (s_prec && !r_prec)	/* famov.ds */
	/*
	 * t this is a source exception due to a denormal which is in a
	 * double format. It is emminent that the single precision result
	 * is also a denormal with a mantissa of zero or 0...1
	 */
    {
	rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS) >> ROUNDING_MODE_SHIFT;
	sign = result.b_double.sign;
	result.b_double.sign = 0;
	/*
	 * If mantissa != zero , compute according according to rounding
	 * mode, else compute zero
	 */

	if (result.two_words.bottom_word32 || result.two_words.top_word32) {
	    if ((rounding_mode == DOWNWARD && sign) ||
		    (rounding_mode == UPWARD && !sign))
		result.two_words.bottom_word32 = 1;
	    else
		result.two_words.bottom_word32 = 0;
	} else
	    result.two_words.bottom_word32 = 0;
	result.two_words.top_word32 = 0;
	result.b_single.sign = sign;

    } else
     /* famov.dd or famov.sd */ if (r_prec)
	/* shift up to normalize ,assign, then shift down */
	if (s_prec) {		/* famov.dd */
	    sign = result.b_double.sign;
	    result.b_double.sign = 0;
	    num_shifted = normalize(&result, r_prec);
	    for (i = 0; i < num_shifted; i++)
		do_a_r_shift_d(&result);
	    result.b_double.sign = sign;
	} else {		/* famov.sd */
	    to_double_precision(&result);
	}
    else {			/* famov.ss */
	num_shifted = normalize(&result, r_prec);
	sign = result.b_single.sign;
	result.b_single.sign = 0;
	result.two_words.bottom_word32 =
	    result.two_words.bottom_word32 >> num_shifted;
	result.b_single.sign = sign;
    }
    return (result);
}


/*
 * shift_up
 */
void
shift_up(num, num_shifted, result_precision, overflow_occurs)
    register i860_type *num;
    int result_precision,
    *overflow_occurs,
     num_shifted;
{
    word32 new_exponent;
    word32 temp_ae_bits;
#if FPDEBUG1
    dprintf("in shift up\n");
#if FPDEBUG2
    print_i860type(num);
#endif
#endif
    *overflow_occurs = 0;

    if (result_precision) {
	new_exponent = num->b_double.exponent + (word32) num_shifted;
	if (new_exponent >= 0x7ff) {
	    *overflow_occurs = 1;
	    temp_ae_bits = (word32) ((new_exponent & 0x0700) << AE_SHIFT);
	    fsr = (fsr & ~AE_BITS) | (temp_ae_bits);
	    num->b_double.exponent = new_exponent & 0x7ff;
	} else
	    num->b_double.exponent = new_exponent;
    } else {
	new_exponent = num->b_single.exponent + (word32) num_shifted;
#if FPDEBUG2
	printf("new exponent is %lx\n", new_exponent);
#endif
	if (new_exponent >= 0xff) {
	    *overflow_occurs = 1;
	    temp_ae_bits = (word32) ((new_exponent & 0x0700) << AE_SHIFT);
	    fsr = (fsr & ~AE_BITS) | (temp_ae_bits);
	    num->b_single.exponent = new_exponent & 0xff;
	} else
	    num->b_single.exponent = new_exponent;
    }
}


/*
 * rcp_func
 * used by func_table
 */
i860_type 
rcp_func(src1, src2, s_prec, r_prec)
    i860_type src1,
     src2;
    int s_prec,
     r_prec;
{
    i860_type result,
     do_frcp();
    int num_shifted,
     overflow_occured;
    result.n_double = 0;
#if FPDEBUG1
    dprintf("entering rcp_func\n");
#endif


    num_shifted = normalize(&src2, s_prec);
    if (s_prec) {
	src2.b_double.exponent += 0x3ff;
	num_shifted += 0x3ff;
    } else {
	src2.b_single.exponent += 0x7f;
	num_shifted += 0x7f;
    }
#if FPDEBUG2
    printf("the number after normalize before shift_up");
    printf("num_shifted is 0x%lx\n", num_shifted);
    print_i860type(&src2);
#endif
    result = do_frcp(src2, s_prec, r_prec);
#if FPDEBUG2
    printf("the result before shift_up is ");
    print_i860type(&result);
#endif
    shift_up(&result, num_shifted, r_prec, &overflow_occured);
    if (overflow_occured) {
	fsr |= M_OVERFLOW_OCCURED_BIT;
	if (!(*IEEE_status_ptr & OVERFLOW_MASK)) {
	    if (r_prec)
		fsr |= (word32) M_UNIT_PRECISION_BIT;
	    else
		fsr &= (word32) (~M_UNIT_PRECISION_BIT);
	    compute_biased(result, &FPptr->fp1, OVERFLOW, M_UNIT, 1);
/*			compute_biased(result,&result,OVERFLOW,M_UNIT,1);*/
	}
	round_overflow(&result, M_UNIT, 1);
    }
#if FPDEBUG2
    printf("the result returned from compute_b is ");
    print_i860type(&result);
#endif
    return (result);
}


/*
 * rsqr_func
 * used by func_table
 */
i860_type 
rsqr_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    i860_type result, do_frsqr();
    int num_shifted, overflow_occured;
    result.n_double = 0;
#if FPDEBUG1
    printf("entering rsqr_func\n");
#if FPDEBUG2
    print_i860type(&src2);
#endif
#endif


    num_shifted = normalize(&src2, s_prec);
    if (s_prec) {
	src2.b_double.exponent += 0x3ff;
	num_shifted += 0x3ff;
    } else {
	src2.b_single.exponent += 0x7f;
	num_shifted += 0x7f;
    }

    if (num_shifted % 2) {
	/*
	 * if odd number of left shifts the make it
	 * even because rescaling will do half as
	 * many right shifts  */
	if (s_prec)
	    src2.b_double.exponent++;
	else
	    src2.b_single.exponent++;
	num_shifted++;
    }
#if FPDEBUG2
    printf("the number after normalize before shift_up");
    printf(" num_shifted is 0x %lx ", num_shifted);
    print_i860type(&src2);
#endif
    result = do_frsqr(src2, s_prec, r_prec);

    /* what about inexcat ? */
#if FPDEBUG2
    printf("the number after do_frsqr & before shift_up");
    printf(" num_shifted is 0x %lx ", num_shifted);
    print_i860type(&result);
#endif
    shift_up(&result, ((int) (num_shifted) >> 1), r_prec, &overflow_occured);
#if FPDEBUG2
    printf("the result after shift_up is ");
    print_i860type(&result);
#endif
    if (overflow_occured)
	fsr |= M_OVERFLOW_OCCURED_BIT;
    return (result);
}


/*
 * fix_func
 * used by func_table
 */
i860_type 
fix_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    int rounding_mode;
    i860_type result;
    result.n_double = 0;
#if FPDEBUG1
    dprintf("entering fix_func\n");
#endif
    rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS) >> ROUNDING_MODE_SHIFT;
    switch (rounding_mode) {

    case TO_ZERO:
    case TO_NEAREST:
    break;

    case DOWNWARD:
	if (s_prec) {
	    if (src1.b_double.sign) {
		result.n_double = (i860_double) - 1.0;
		fsr |= AA_BIT;
	    }
	} else if (src1.b_single.sign) {
	    if (r_prec)
		result.n_double = (i860_double) - 1.0;
	    else
		result.n_single = (i860_single) - 1.0;
	    fsr |= AA_BIT;
	}
    break;

    case UPWARD:
	if (s_prec) {
	    if (!src1.b_double.sign) {
		result.n_single = (i860_double) 1.0;
		fsr |= AA_BIT;
	    }
	} else if (!src1.b_single.sign) {
	    if (r_prec)
		result.n_double = (i860_double) 1.0;
	    else
		result.n_single = (i860_single) 1.0;
	    fsr |= AA_BIT;
#if FPDEBUG2
	    printf("the result is ");
	    print_i860type(&result);
#endif
	}
    break;

    }
    return (result);
}


/*
 * feq_func
 * used by func_table
 */
i860_type
feq_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    i860_type junk = {0};
#if FPDEBUG1
    dprintf("entering feq_func\n");
#endif
    if ((src1.two_words.top_word32 == src2.two_words.top_word32) &&
	    (src1.two_words.bottom_word32 == src2.two_words.bottom_word32))
	FPptr->psr |= SET_CC_BIT;
    else
	FPptr->psr &= CLEAR_CC_BIT;

    return junk; /* for compiler */
}


/*
 * fgt_func
 * used by func_table
 */
i860_type
fgt_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    i860_type junk = {0};

#if FPDEBUG1
    dprintf("entering fgt_func(%lx %lx %lx %lx)\n",src1, src2, s_prec, r_prec);
#endif
    if ( denormal_gt(src1, src2, s_prec) )
	FPptr->psr |= SET_CC_BIT;
    else
	FPptr->psr &= CLEAR_CC_BIT;
#if FPDEBUG1
    dprintf("exit fgt_func()\n");
#endif
    return junk;
}


/*
 * trunc_func
 * used by func_table
 */
i860_type 
trunc_func(src1, src2, s_prec, r_prec)
    i860_type src1, src2;
    int s_prec, r_prec;
{
    i860_type result;
#if FPDEBUG1
    dprintf("entering trunc_func\n");
#endif
    result.n_double = 0;
    return (result);
}


/*
 * dummy_func
 * used by func_table
 */
i860_type 
dummy_func(src1, src2, s_prec, r_prec)
    i860_type src1,
     src2;
    int s_prec,
     r_prec;
{
    i860_type junk = {0};

#if FPDEBUG1
    dprintf("entering dummy_func\n");
#endif
    return junk;
}

/*
 **** From NX ctrap.c ****
 *	version: ctrap.c 10.1 92/06/24 13:50:27
 */

/************************************************************************
 *
 * check_if_to_ignore_SE
 *	Ignore the SE bit for fp traps on fld, pfld, fst, pst
 *	and ixfr instructions when in single instruction mode
 *	or when in dual-instruction mode and the companion
 *	instruction is not a multiplier or adder operation
 *
 *	(It is assumed that SE occured).
 *
 * inputs:
 *
 *	i860_saved_state *u.
 *
 * outputs:
 *
 *	0 == ignore SE, otherwise deal with SE
 *
 * side effects:
 *
 *	SE bit in fsr cleared if SE is to be ignored.
 *
 */
int
check_if_to_ignore_SE(u)
	struct i860_saved_state *u;
{
	register unsigned long	inst;
	register unsigned long	finst;

	if (u->psr & PSR_DIM) /* SE occured in dual-instruction mode */
		inst = *(unsigned long *)(u->pc + 4);
	else                  /* SE occured in single-instruction mode */
		inst = *(unsigned long *)u->pc;

	switch (inst >> 27)
	{
		case 1:  /* ixfr */
		case 4:  /* fld  */
		case 5:  /* fst  */
		case 7:  /* pst  */
			/*
			 * distinguish pst from st.c/ld.c
			 */
			if (!(inst & 0x04000000))
				break;
		case 12: /* pfld */
			if (u->psr & PSR_DIM) {
				/* SE occured in dual-instruction mode */
				finst = *(unsigned long *)u->pc;
				if (finst & 0x40) {   /* i unit instructions */
					/* clear SE bit in fsr */
					u->fsr &= ~FSR_SE;
			         }
			}
			else { /* SE occured in single-instruction mode */
				u->fsr &= ~FSR_SE; /* clear SE bit in fsr */
			}
	}
	return( u->fsr & FSR_SE ); /* 0 == ignore SE, else deal with it */
}

/************************************************************************
 *
 * set_trapped_opcode
 *	Set a code in the trapped_opcode field of the proc state which
 *	represents the FPEH for the instruction.
 *
 * inputs:
 *
 *	inst - the trapped instruction
 *
 * outputs:
 *
 *	A code to set the trapped_opcode field of the proc state.
 *
 *
 */

int
set_trapped_opcode( addr, psr )
	unsigned long *addr;
	unsigned long psr;
{
	register unsigned long	inst, opcode;

	if ( psr & PSR_DIM ) { /* Dual Instruction Mode */
	    inst = *(addr-1);
	} else {
	    inst = *addr;
	}
	opcode = inst & 0x7F;

#ifdef FPDEBUG
showf(" INST = %lx, opcode = %lx DIM %lx\n",inst,opcode,psr);
#endif
	switch (opcode)
	{
		case 0x20:
			return FMUL;

		case 0x22:
			return FRCP;

		case 0x23:
			return FRSQR;

		case 0x30:
			return FADD;

		case 0x31:
			return FSUB;

		case 0x32:
			return FIX;

		case 0x33:
			return FAMOV;

		case 0x34:

			if(inst&0x80)
				return PFLE;
			else
				return PFGT;

		case 0x35:
			return PFEQ;

		case 0x3a:
			return FTRUNC;

		default:
			if ((opcode >= 0) && (opcode <= 0xf))
				return PFAM;
			else if ((opcode >= 0x10) && (opcode <= 0x1f))
				return PFSM;
			else
				return 0;
	}
}

/*****************************************************************
 *
 * Algorithm for i860 bug around
 *
 * If the preceding instruction (fir - 4) is not a delayed
 *  control operation
 * begin
 *  if the next instruction (fir+4 in single mode, fir+12 in dual mode)
 *    is a branch operation, restart at the previous instruction
 *    (which should be a dummy store).
 *  else if the current instruction is a branch operation (fir+4 in
 *    dual mode), restart at the second previous instruction
 *    (which should be a dummy store).
 *
 */
int
is_nop_op_at(inst_addr)
unsigned long inst_addr;
{
	unsigned long inst;

	inst = *(unsigned long *)inst_addr;
	if (inst == 0xa0000000) /* check if inst is nop */
		return 1;
	else
		return 0;
}

int
is_st_op_at(inst_addr)
	unsigned long inst_addr;
{
	unsigned long inst;

	inst = *(unsigned long *)inst_addr;
	if ((inst & 0xec000000) == 0x0c000000) /* check if inst is st.x */
		return 1;
	else
		return 0;
}

int
is_branch_op_at(inst_addr)
	unsigned long inst_addr;
{
	unsigned long inst;

	inst = *(unsigned long *)inst_addr;
	if ( ((inst & 0xfc000000) == 0x40000000) /* check if inst is bri */
	  || ((inst & 0xe0000000) == 0x60000000) /* check if inst is CTRL fmt */
	  || ((inst & 0xfc00001f) == 0x4c000002) /* check if inst is calli */
	  || ((inst & 0xf0000000) == 0x50000000) /* check if inst is bt(n)e */
	   )
		return 1;
	else
		return 0;
}

int ct_work_around(U, psr, fir, inst_fir)
unsigned long psr, fir, inst_fir;
	struct i860_saved_state *U;
{
	register unsigned long next_inst_addr;
	register unsigned long prev_inst_addr;
	register unsigned long second_prev_inst_addr;
	register unsigned long inst;

	if (psr & 0x4000) /* check if DIM */
	{
		next_inst_addr = fir + 12;
		second_prev_inst_addr = fir - 12;
	}
	else
	{
		next_inst_addr = fir + 4;
		second_prev_inst_addr = fir - 8;
	}
	if (is_nop_op_at(inst_fir) && is_branch_op_at(next_inst_addr)
		&& is_st_op_at(fir - 4))
	{
		if (psr & 0x4000)
			U->pc -= 8;
		else
			U->pc -= 4;
	}
	else if (is_branch_op_at(inst_fir) && is_nop_op_at(fir - 4) &&
			 is_st_op_at(second_prev_inst_addr))
	{
				U->pc = second_prev_inst_addr;
				if (psr & 0x4000) /* DIM */
					U->pc -= 4;
	}
}
