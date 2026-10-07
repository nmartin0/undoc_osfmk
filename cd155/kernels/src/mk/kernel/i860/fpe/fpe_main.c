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
 * $Id: fpe_main.c,v 1.4 1994/11/18 20:40:25 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_main.c,v $
 * Revision 1.4  1994/11/18  20:40:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:18  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_main.c    1.2"

#include "sys/types.h"
#include <i860/fpe/siginfo.h>
#include <i860/fpe/tss.h>
#include "fpe.h"

/*
 * set_ieee_status()
 * sets sticky status bits of the ieee status word 
 * according to the multiplyer and adder status bits.
 */ 
void
set_ieee_status(FPptr)
fp_frame_t *FPptr;
{ 

    FPstatus |= (((FPstatus & IEEE_MBITS) | 
                  ((FPstatus & IEEE_ABITS) >> IEEE_A_SHIFT)) << IEEE_S_SHIFT);
    /*FPptr->ieee_status = FPstatus;	*/	/* XXX */
}

/*
 * reset_ieee_status()
 * resets the ieee status word, clearing all status bits except
 * sticky and mask bits.
 */
void
reset_ieee_status(FPptr) 
fp_frame_t *FPptr;
{
    FPstatus &= (IEEE_MASKS|IEEE_SBITS);
    /*FPptr->ieee_status = FPstatus; */		/* XXX */
}

/*
 *  fp_exception()
 *  is the entrance of the IEEE floating point exception handler,
 *  and called by the u_trap() when u.u_fp_mode is 0.
 *  The fp_exception() returns 0 if the exception was masked or
 *  the exception is not IEEE error like denormals and QNaNs, and
 *  returns the SIGFPE signal codes defined in "sys/siginfo.h". 
 */
int fp_exception(FPptr)
fp_frame_t *FPptr;
{
    ulong fsr,old_rm;
    int sigcode = 0;

#if DEBUG
    if (fpe_debug) printf("\n\nEntering handle exception\n");
#endif

    fsr = FPptr->fsrs[2];

    reset_ieee_status(FPptr);
    old_rm = set_rounding((fsr & FSR_RM) >> FSR_RM_SHIFT);

    if (fsr & FSR_RBITS) {
        /*
         * result error exception 
         */
        if (fsr & (FSR_MO|FSR_AO)) {
            handle_overflow(FPptr); 
	    sigcode = (FPstatus & IEEE_X_OFLW) ? sigcode : FPE_FLTOVF;
	}
        if (fsr & (FSR_MU|FSR_AU)) {
            handle_underflow(FPptr); 
	    sigcode = (FPstatus & IEEE_X_UFLW) ? sigcode : FPE_FLTUND;
	}
        if (FPptr->fsrs[2] & (FSR_MI|FSR_AI)) {
            handle_inexact(FPptr); 
	    sigcode = (FPstatus & IEEE_X_INEXT) ? sigcode : FPE_FLTRES;
	}
    }
    else if (fsr & FSR_SE) {
        /* 
         * source error exception
         */
        switch (determine_source_error(FPptr)) {
        case SE_DIV_BY_ZERO:
	    if (FPstatus & IEEE_X_DIVZ) 
	        place_se_result(FPptr);
	    else
	        sigcode = FPE_FLTDIV;
            break;
	case SE_INVALID:
	    if (FPstatus & IEEE_X_INV)
	        place_se_result(FPptr);
	    else
	        sigcode = FPE_FLTINV;
	    break;
	case SE_NO_IEEE_ERROR:
	    place_se_result(FPptr);
	    break;
	}
    } 
    else {
	extern int bug_print;
	if (bug_print) 
	    printf("fp_exception: unknown floating point interrupt.\n");
	fpe_trace(FPptr->fir,FPptr->psr,FPptr->fsrs[2]);
    }
    set_ieee_status(FPptr);
    set_rounding(old_rm);
    return(sigcode);
}

/*
 * fp_sw_exception()
 * is the other entrance of the floating point exception handler, and is called
 * when the software trap occured with the "trap r0,r4,r0" instruction.  
 */

#define NO_EXCEPTION 	0
#define INVALID		0x1
#define DIVBYZERO	0x2
#define OVERFLOW 	0x4
#define UNDERFLOW	0x8
#define INEXACT		0x10
#define FPE_FLTILL	9

int
fp_sw_exception(FPptr)
ulong *FPptr;
{
	unsigned mul_status,add_status,res_status;
	int sigcode;
	
	sigcode = NO_EXCEPTION;

	mul_status = FPstatus & 0x1f;
	add_status = (FPstatus >> 5) & 0x1f;

	res_status = mul_status | add_status;

	switch (res_status) {
	case INVALID:
	    sigcode = (FPstatus & IEEE_X_INV) ? sigcode : FPE_FLTINV;
    	    break;
	case DIVBYZERO:
	    sigcode = (FPstatus & IEEE_X_DIVZ) ? sigcode : FPE_FLTDIV;
	    break;
	case OVERFLOW:
	    sigcode = (FPstatus & IEEE_X_OFLW) ? sigcode : FPE_FLTOVF;
	    break;
	case UNDERFLOW:
	    sigcode = (FPstatus & IEEE_X_UFLW) ? sigcode : FPE_FLTUND;
	    break;
	case INEXACT:
	    sigcode = (FPstatus & IEEE_X_INEXT)	? sigcode : FPE_FLTRES;
	    break;
	default:
	    return(FPE_FLTILL);
	}
	set_ieee_status(FPptr);
	return(sigcode);
}
