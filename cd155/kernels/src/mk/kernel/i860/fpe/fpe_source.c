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
 * $Id: fpe_source.c,v 1.5 1994/11/18 20:40:29 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_source.c,v $
 * Revision 1.5  1994/11/18  20:40:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  22:34:26  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:28:53  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:29:12  dleslie
 * First R1_0 release
 *
 * Revision 1.3  1993/02/10  16:58:44  andyp
 * A fix from Koichi in Santa Clara to correct a problem with
 * source exceptions demonstrated by the sv_recp() runtime
 * routine.
 *
 * Revision 1.2  92/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 * 
 */

#ident "@(#)fpe:fpe_source.c    1.6"

#include "sys/types.h"
#include <i860/fpe/tss.h>
#include "fpe.h"
#include "fpe_macros.h"

#define FP_src1		((fp_t *)&FPptr->fp1) 
#define FP_src2		((fp_t *)&FPptr->fp2)
#define FP_msrc1	((fp_t *)&FPptr->fp1)
#define FP_msrc2	((fp_t *)&FPptr->fp2)
#define FP_asrc1	((fp_t *)&FPptr->fp3)
#define FP_asrc2	((fp_t *)&FPptr->fp4)
#define FP_res		((fp_t *)&current_thread()->pcb->fp5)
#define FP_mres		((fp_t *)&current_thread()->pcb->fp5)
#define FP_ares		((fp_t *)&current_thread()->pcb->fp6)
#define FP_fsr		((ulong *)&current_thread()->pcb->r_fsr)	
#define FP_psr		((ulong *)&FPptr->psr)

/*
 * determine_source_error()
 *
 * returns one of following:
 *      SE_DIV_BY_ZERO
 *      SE_INVALID
 *      SE_NO_IEEE_ERROR
 */

int 
determine_source_error(FPptr)
fp_frame_t *FPptr;
{
    ulong return_code;
    int op,adder_op,s_prec,r_prec;
    ulong mfsr = FPptr->fsrs[2] & FSR_FZ;
    ulong afsr = FPptr->fsrs[2] & FSR_FZ; 

    /*    
     * get_se_operands() sets up the global variable FP_src1 and FP_src2 
     * in the case of the source error on the scaler instruction and 
     * the pipelined instruction,  and also sets up FP_msrc1, FP_msrc2, 
     * FP_asrc1, FP_asrc2 in the case of the source error on the 
     * dual operation instruction.
     */   
    get_se_operands(FPptr);
    
    op = FPptr->trapped_opcode;
    s_prec = (FPstatus & IEEE_S_PREC) ? 1 : 0;
    r_prec = (FPstatus & IEEE_R_PREC) ? 1 : 0;
    *FP_fsr = FPptr->fsrs[2] & FSR_FZ;

    if (((op == FRCP) || (op == FRSQR)) && (is_zero(FP_src2, s_prec))) {
        divz_op(op,FP_src1,FP_src2,FP_res,s_prec,r_prec);
        FPstatus |= IEEE_M_DIVZ;
        return(SE_DIV_BY_ZERO);
    }
    if ((op == FRSQR) && (is_negative(FP_src2, s_prec))) {
	(FP_res)->dlong = (s_prec) ? DoubleDefaultResult : SingleDefaultResult;
	FPstatus |= IEEE_M_INV;
	return(SE_INVALID);
    } 
    if (FPstatus & IEEE_DUAL_OP) {
        return_code = SE_NO_IEEE_ERROR;
        if ((is_nan(FP_msrc1,s_prec)) || (is_nan(FP_msrc2,s_prec))) {
	    if (nans_op(FMUL,FP_msrc1,FP_msrc2,FP_mres,s_prec,r_prec,FP_psr)) {
	        return_code = SE_INVALID;
		FPstatus |= IEEE_M_INV;
	    }
	}
        else if ((is_infinity(FP_msrc1,s_prec)) || (is_infinity(FP_msrc2,s_prec))) {
	    if (infs_op(FMUL,FP_msrc1,FP_msrc2,FP_mres,s_prec,r_prec,FP_psr)) { 
	        return_code = SE_INVALID;
                FPstatus |= IEEE_M_INV;
	    }
	}
        else 
	    re_execute_fp_instruction
		 (FMUL,FP_msrc1,FP_msrc2,FP_mres,s_prec,r_prec,&mfsr,FP_psr);
	/*
         * In the case of dual operation instructions,
         * the result precision controls in this case both the precision
         * of the source operands of the addition or subtraction and 
         * the precision of all the results.
         */
	adder_op = (op == PFAM) ? FADD : FSUB;
        if ((is_nan(FP_asrc1,r_prec)) || (is_nan(FP_asrc2,r_prec))) { 
	    if (nans_op(adder_op,FP_asrc1,FP_asrc2,FP_ares,r_prec,r_prec,FP_psr)) {
	        return_code = SE_INVALID;
		FPstatus |= IEEE_A_INV;
	    }
	}
        else if ((is_infinity(FP_asrc1,r_prec)) || 
			(is_infinity(FP_asrc2,r_prec))) {
	    if (infs_op(adder_op,FP_asrc1,FP_asrc2,FP_ares,r_prec,r_prec,FP_psr)) { 
	        return_code = SE_INVALID;
		FPstatus |= IEEE_A_INV;
	    }
	}
        else 
	    re_execute_fp_instruction
		 (adder_op,FP_asrc1,FP_asrc2,FP_ares,r_prec,r_prec,&afsr,FP_psr);

	*FP_fsr = mfsr | afsr;
        return(return_code);
    }
    else {
        if ((is_nan(FP_src1,s_prec)) || (is_nan(FP_src2,s_prec))) { 
	    if (nans_op(op,FP_src1,FP_src2,FP_res,s_prec,r_prec,FP_psr)) {
		if (IS_ADDER_OP(op))
		    FPstatus |= IEEE_A_INV;   	
		else
		    FPstatus |= IEEE_M_INV;
	        return(SE_INVALID);
	    }
	}
	else if ((is_infinity(FP_src1,s_prec)) || 
		 	(is_infinity(FP_src2,s_prec))) {
	    if (infs_op(op,FP_src1,FP_src2,FP_res,s_prec,r_prec,FP_psr)) {
		if (IS_ADDER_OP(op))
		    FPstatus |= IEEE_A_INV;
		else
		    FPstatus |= IEEE_M_INV;
	        return(SE_INVALID);
	    }
	}
        else
	    re_execute_fp_instruction
		(op,FP_src1,FP_src2,FP_res,s_prec,r_prec,FP_fsr,FP_psr);
        return(SE_NO_IEEE_ERROR);
    }
}
    

/*
 *  divz_op()
 *  handles divide by zero exception.
 */
divz_op(op,src1,src2,res,s_prec,r_prec)
int op;
fp_t *src1,*src2,*res;
int s_prec,r_prec;
{
    if (s_prec) {
        if((src2->b_double.sign == 1) && (op == FRCP)) 
            res->dlong = DoubleNegInfinity;
        else
	    res->dlong = DoublePosInfinity;
      }
    else {
        if (r_prec)
            if((src2->b_single.sign == 1) && (op == FRCP))
	        res->dlong = DoubleNegInfinity;
            else
	        res->dlong = DoublePosInfinity;
        else
            if((src2->b_single.sign == 1) && (op == FRCP))
	        res->dlong = SingleNegInfinity;
            else
	        res->dlong = SinglePosInfinity;
    }
}
      
/* 
 * nans_op()
 * supports operations between nan operands, and
 * returns 1 if one of sources is a signaling nan,
 *         0 otherwise.
 */
int 
nans_op(inst,src1,src2,res,s_prec,r_prec,psr)
int inst;
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *psr;
{
    int return_code = 0;

    if (is_snan(src1,s_prec)) {
        return_code = 1;
        to_qnan(src1,s_prec);
    }
    if (is_snan(src2,s_prec)) {
        return_code = 1;
        to_qnan(src2,s_prec);
    }
 
    if (nan_gt(src1,src2,s_prec)) {
        if (s_prec != r_prec) {
            if (r_prec)
                to_double_nan(src1);
            else
                to_single_nan(src1);
        }
        *res = *src1;
    }
    else {
        if (s_prec != r_prec) {
            if (r_prec) 
                to_double_nan(src2);
            else
                to_single_nan(src2);
        }
        *res = *src2;
    }
    /* 
     * I am not sure whether it is OK or not to affect user program's psr here.
     * however, at this point just do so.
     */
    if (inst == PFGT || inst == PFEQ || inst == PFLE) {
	if (inst == PFLE)
	    *psr |= PSR_CC;
	else
	    *psr &= ~PSR_CC;
	res->n_double = 0;
    }
    else if (inst == FRCP || inst == FRSQR) 
	*res = *src2;
    return(return_code);
}

/*
 * infs_op()
 * supports operations between infinity operands, and
 * returns 1 if the operation is invalid,
 *         0 otherwise.
 *
 * the supporting operations are:
 *    FADD, FSUB, FIX, FTRUNC, PFEQ, PFLE, PFGT, FRSQR, AND FMUL.  
 *   
 */
int
infs_op(op,src1,src2,res,s_prec,r_prec,psr)
int op;
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *psr;
{
    int inf1,inf2;
    int sign;
    int return_code = 0;

    inf1 = is_infinity(src1,s_prec);
    inf2 = is_infinity(src2,s_prec);
    switch (op) {
    case FAMOV:
	if (r_prec)
	    res->dlong = (inf1 == -1) ? 
	        DoubleNegInfinity : DoublePosInfinity;
	else
	    res->dlong = (inf1 == -1) ?
	        SingleNegInfinity : SinglePosInfinity;
	break;

    case FADD: 
        if ((inf1 * inf2) == -1) {
	    /* invalid operation */
	    res->dlong = (r_prec) ? 
	        DoubleDefaultResult : SingleDefaultResult;
	    return_code = 1;
        }
	else if (r_prec) {
	    if (inf1)
	        res->dlong = (inf1 == -1) ?
		    DoubleNegInfinity : DoublePosInfinity;
            else
	        res->dlong = (inf2 == -1) ?
		    DoubleNegInfinity : DoublePosInfinity;
	}
	else {
	    if (inf1)
	        res->dlong = (inf1 == -1) ?
		    SingleNegInfinity : SinglePosInfinity;
	    else 
	        res->dlong = (inf2 == -1) ?
		    SingleNegInfinity : SinglePosInfinity;
	}
	break;
    
    case FSUB:
	if ((inf1 * inf2) == 1) { 
	    /* invalid operation */
	    res->dlong = (r_prec) ?
	        DoubleDefaultResult : SingleDefaultResult;
            return_code = 1;
        } 
	else
	/*
         * +infinity - -infinity = +infinity
         * -infinity - +infinity = -infinity
         * #infinity - #x        = $infinity
         * #x        - #infinity = !infinity
         * !: means the opposite signed infinity of the l.h.s infinity
         */ 
        {
	unsigned s_sign_bit1;
	unsigned s_sign_bit2;
	if (s_prec) {
	    s_sign_bit1 = src1->b_double.sign;
	    s_sign_bit2 = src2->b_double.sign;
	}
	else {
	    s_sign_bit1 = src1->b_single.sign;
	    s_sign_bit2 = src2->b_single.sign;
	}	
	if (r_prec) {
	    res->dlong = DoublePosInfinity;
            if (inf1)
                if (inf2)
                    res->b_double.sign = (inf2==1) ? 1:0;
                else
                    res->b_double.sign = s_sign_bit1;
            else
                res->b_double.sign= !s_sign_bit2;
        }
        else {
	    res->dlong = SinglePosInfinity;
            if (inf1)
                if (inf2)
                    res->b_single.sign = (inf2 == 1) ? 1:0;
                else
                    res->b_single.sign = s_sign_bit1;
            else
                res->b_single.sign= !s_sign_bit2;
        }
      }
      break;
      
    case FIX: 
        if (is_infinity(src1,s_prec)) {
            if (r_prec) 
	        res->dlong = DoubleDefaultResult;
            else 
	        res->dlong = SingleDefaultResult;
            return_code = 1;
        }
        break; 
      
    case FTRUNC:
        if (is_infinity(src1,s_prec)) {
            if (r_prec)
	        res->dlong = DoubleDefaultResult;
            else 
	        res->dlong = SingleDefaultResult;
            return_code = 1;
        }
        break; 
      
    case PFEQ:
        if ((src1->dlong.hi == src2->dlong.hi) && 
            (src1->dlong.lo == src2->dlong.lo)) 
            *psr |= PSR_CC;
        else 
            *psr &= ~PSR_CC;
        res->n_double = 0;
        break; 
      
    case PFGT: 
    case PFLE:
	/* 
	 * inf_gt() is not implemented yet, however it will be 
	 * in fpe_utils.c
	 */
	if (inf_gt(src1,src2,s_prec))
            *psr |= PSR_CC;
        else
            *psr &= ~PSR_CC;
        res->n_double = 0;
        break; 

    case FRCP:
	 res->n_double = 0;
	 if (r_prec)
	     res->b_double.sign = src2->b_double.sign;
	 else
	     res->b_single.sign = src2->b_single.sign;
	 break;	
    case FRSQR: 
	/*
	 * if the operand is negative, it will be invalid operation.
	 * otherwise, the result will be +0.
	 */
         if (is_negative(src2,s_prec)) { 
             if (r_prec) 
	         res->dlong = DoubleDefaultResult;
             else
	         res->dlong = SingleDefaultResult;
             return_code = 1;
         }
         res->n_double = 0;
	 break; 

    case FMUL: 
	 /*
	  *  0 * infinity or infinity * 0 will be invalid operation.  
	  */
         if ( ((inf1) && is_zero(src2, s_prec)) || 
              ((inf2) && is_zero(src1, s_prec)) ) { 
             if (r_prec) 
	         res->dlong = DoubleDefaultResult;
             else 
	         res->dlong = SingleDefaultResult;
             return_code = 1;
	     break;
         }
         sign = xor_signs(src1,src2,s_prec);
         if (r_prec) {
	     res->dlong = DoublePosInfinity;
             res->b_double.sign = sign;
         }
         else {
	     res->dlong = SinglePosInfinity;
             res->b_single.sign = sign;
         }
         break;
    default:
	printf("infs_op(): wrong opreation %d\n", op);
	break;
    }
    return(return_code);
}

/* 
 * place_se_result()
 */
void
place_se_result(FPptr)
fp_frame_t *FPptr;
{
    int pprec,s_prec,r_prec;
    fp_t apipeout,mpipeout;
    int src1,dest;
    ulong fpinst = *FPptr->fir;

    src1 =  (fpinst >> 11) & 0x1f;
    dest  = (fpinst >> 16) & 0x1f;

    s_prec = FPstatus & IEEE_S_PREC;
    r_prec = FPstatus & IEEE_R_PREC;

#if DEBUG
    if (fpe_debug)
      printf("Entering place_se_result\n");
#endif

    if (FPstatus & IEEE_DUAL_OP) {
        /* carry out loads */
        if (FPstatus & IEEE_T_LOAD) {	
            pprec = FPptr->fsrs[2] & FSR_MRP ? 1 : 0;
            if (pprec)
		FPptr->t = FPptr->mres[2];
	    else 
		unpack(&FPptr->t,&FPptr->mres[2]);
        }
        if (FPstatus & IEEE_KR_LOAD) {
	    if (s_prec)
                get_fpreg(FPptr,src1,&FPptr->kr,s_prec);
            else 
		unpack(&FPptr->kr,&FPptr->fregs[src1]);
        }
        else if(FPstatus & IEEE_KI_LOAD) {
	    if (s_prec)
                get_fpreg(FPptr,src1,&FPptr->ki,s_prec);
	    else 
                unpack(&FPptr->ki,&FPptr->fregs[src1]);
        }
	      
	advance_mpipe(FPptr,FP_mres,&mpipeout,*FP_fsr);
	advance_apipe(FPptr,FP_ares,&apipeout,*FP_fsr);

	if (FPstatus & IEEE_PIPE_OP) { 
	    /*
             * This is a pfam or pfsm, so adder pipe out goes to
             * destination register.
	     */
	     pprec = (FPptr->fsrs[2] & FSR_ARP) ? 1 : 0;
	     set_fpreg(FPptr,dest,&apipeout,pprec);
	 }
	 else {
	    /* 
	     * This is is a pfmam or pfmsm, so multiplier pipe 
             * output goes to destination register.
	     */
	     pprec = (FPptr->fsrs[2] & FSR_MRP) ? 1 : 0;
             set_fpreg(FPptr,dest,&mpipeout,pprec);
         }
    }
    else if (FPstatus & IEEE_PIPE_OP) {
        if (IS_ADDER_OP(FPptr->trapped_opcode)) {
	    pprec = (FPptr->fsrs[2] & FSR_ARP) ? 1 : 0;
	    advance_apipe(FPptr,FP_res,&apipeout,*FP_fsr);
            set_fpreg(FPptr,dest,&apipeout,pprec);
        }
        else {
	    pprec = (FPptr->fsrs[2] & FSR_MRP) ? 1 : 0;
	    advance_mpipe(FPptr,FP_res, &mpipeout,*FP_fsr);
            set_fpreg(FPptr,dest,&mpipeout,pprec);
        }
    }
    else {
        /* scalar operation */
        if (IS_ADDER_OP(FPptr->trapped_opcode)) {
	    FPptr->ares[2]  = *FP_res;
	    fix_fsr_abits(FPptr,r_prec,*FP_fsr);
        }
        else {
	    FPptr->mres[2] = *FP_res;
	    fix_fsr_mbits(FPptr,r_prec,*FP_fsr);
        }
	set_fpreg(FPptr,dest,FP_res,r_prec);
	FPptr->fsrs[2] |= (dest << FSR_RR_SHIFT);
    }
    /* turn on KNF bit in IEEE status word and in psr */
    FPstatus |= IEEE_KNF_SET;
    FPptr->psr |= PSR_KNF;
}

advance_mpipe(FPptr,pipein,pipeout,fsr)
fp_frame_t *FPptr;
fp_t *pipein,*pipeout;
ulong fsr;
{
    *pipeout = FPptr->mres[2]; 
    if (FPstatus & IEEE_S_PREC) {
        /* single double or double double, handled the same */
        FPptr->mres[2] = FPptr->mres[0];
        FPptr->mres[1].n_double = 0;
        FPptr->fsrs[2] = ((FPptr->fsrs[2] & ~FSR_MBITS)
	                    |(FPptr->fsrs[0] & FSR_MBITS));
        FPptr->fsrs[1] = (FPptr->fsrs[1] & ~FSR_MBITS) | FSR_MRP;
    }
    else {
        FPptr->mres[2] = FPptr->mres[1];
        FPptr->mres[1] = FPptr->mres[0];
        FPptr->fsrs[2] = ((FPptr->fsrs[2] & ~FSR_MBITS)
	                     |(FPptr->fsrs[1] & FSR_MBITS));
        FPptr->fsrs[1] = ((FPptr->fsrs[1] & ~FSR_MBITS)
                             |(FPptr->fsrs[0] & FSR_MBITS));
    }
    FPptr->fsrs[0] &= ~FSR_MBITS;
    FPptr->fsrs[0] |= (FSR_MBITS & fsr);
    FPptr->mres[0] = *pipein;
    if (FPstatus & IEEE_R_PREC)
        FPptr->fsrs[0] |= FSR_MRP;
    else
	FPptr->fsrs[0] &= ~FSR_MRP;
    if (fsr & FSR_MI)
        FPptr->fsrs[0] |= FSR_SI;
}
        
advance_apipe(FPptr,pipein,pipeout,fsr)
fp_frame_t *FPptr;
fp_t *pipein,*pipeout;
ulong fsr;
{
    *pipeout = FPptr->ares[2];
    FPptr->ares[2] = FPptr->ares[1];
    FPptr->ares[1] = FPptr->ares[0];
    FPptr->ares[0] = *pipein;
    FPptr->fsrs[2] = (FPptr->fsrs[2] & ~FSR_ABITS)|(FPptr->fsrs[1] & FSR_ABITS);
    FPptr->fsrs[1] = (FPptr->fsrs[1] & ~FSR_ABITS)|(FPptr->fsrs[0] & FSR_ABITS);
    FPptr->fsrs[0] &= ~FSR_ABITS;
    FPptr->fsrs[0] |= (FSR_ABITS & fsr);
    if (FPstatus & IEEE_R_PREC)
        FPptr->fsrs[0] |= FSR_ARP;
    else
	FPptr->fsrs[0] &= ~FSR_ARP;
    if (fsr & FSR_AI)
        FPptr->fsrs[0] |= FSR_SI;
}


fix_fsr_abits(FPptr,rprec, afsr)
fp_frame_t *FPptr;
int	rprec;
ulong	afsr;
{
	ulong	scalar_bits;

	scalar_bits = FPptr->fsrs[2] & FSR_OTHRBITS;

	FPptr->fsrs[2] = scalar_bits | 
		(FPptr->fsrs[2]  & (FSR_MBITS|FSR_MRP))| (afsr & FSR_ABITS);
	FPptr->fsrs[1] = scalar_bits | (FPptr->fsrs[1] & (FSR_MBITS|FSR_MRP));
	FPptr->fsrs[0] = scalar_bits | (FPptr->fsrs[0] & (FSR_MBITS|FSR_MRP));

	if (rprec) 
	    FPptr->fsrs[2] |= FSR_ARP;
	if (afsr & FSR_AI) 
	    FPptr->fsrs[2] |= FSR_SI;
}

fix_fsr_mbits(FPptr,rprec, mfsr)
fp_frame_t *FPptr;
int	rprec;
ulong	mfsr;
{
	ulong	scalar_bits;

	scalar_bits = FPptr->fsrs[2] & FSR_OTHRBITS;

	if (rprec == 0) {
		/* single precision, so 3 stage m pipe */
	FPptr->fsrs[2] = scalar_bits | 
		(FPptr->fsrs[2]  & (FSR_ABITS|FSR_ARP))| (mfsr & FSR_MBITS);
	FPptr->fsrs[1] = scalar_bits | (FPptr->fsrs[1] & (FSR_ABITS|FSR_ARP));
	FPptr->fsrs[0] = scalar_bits | (FPptr->fsrs[0] & (FSR_ABITS|FSR_ARP));
	}
	else {
		/* Double precision, so 2 stage m pipe */
	FPptr->fsrs[2]  = scalar_bits | 
		(FPptr->fsrs[2]  & (FSR_ABITS|FSR_ARP))| (mfsr & FSR_MBITS);
	FPptr->fsrs[1] = scalar_bits | (FPptr->fsrs[1] & (FSR_ABITS|FSR_ARP));
	FPptr->fsrs[0] = scalar_bits | (FPptr->fsrs[0] & (FSR_ABITS|FSR_ARP));
	FPptr->fsrs[2] |= FSR_MRP;
	}
	if (mfsr & FSR_MI) 
	    FPptr->fsrs[2] |= FSR_SI;
}


#define	KIREG	32
#define	KRREG	34
#define	TREG	36
#define	MRES	38
#define	ARES	40

struct	dpc {
	int	mop1, mop2, aop1, aop2, tload, kload;
};

struct	dpc	pfam[16] = {

	/* 	Mop1,	Mop2,	Aop1,	Aop2,	Tload,	Kload	*/
	
	{	KRREG,	2,	1,	MRES,	0,	0	},
	{	KRREG,	2,	TREG,	MRES,	0,	KRREG	},
	{	KRREG,	2,	1,	ARES,	1,	0	},
	{	KRREG,	2,	TREG,	ARES,	1,	KRREG	},
	{	KIREG,	2,	1,	MRES,	0,	0	},
	{	KIREG,	2,	TREG,	MRES,	0,	KIREG	},
	{	KIREG,	2,	1,	ARES,	1,	0	},
	{	KIREG,	2,	TREG,	ARES,	1,	KIREG	},
	{	KRREG,	ARES,	1,	2,	1,	0	},
	{	1,	2,	ARES,	MRES,	0,	0	},
	{	KRREG,	ARES,	1,	2,	0,	0	},
	{	1,	2,	TREG,	ARES,	1,	0	},
	{	KIREG,	ARES,	1,	2,	1,	0	},
	{	1,	2,	TREG,	MRES,	0,	0	},
	{	KIREG,	ARES,	1,	2,	0,	0	},
	{	1,	2,	TREG,	ARES,	0,	0	},
};

struct	dpc	pfmam[16] = {

	/* 	Mop1,	Mop2,	Aop1,	Aop2,	Tload,	Kload	*/
	
	{	KRREG,	2,	1,	MRES,	0,	0	},
	{	KRREG,	2,	TREG,	MRES,	0,	KRREG	},
	{	KRREG,	2,	1,	MRES,	1,	0	},
	{	KRREG,	2,	TREG,	MRES,	1,	KRREG	},
	{	KIREG,	2,	1,	MRES,	0,	0	},
	{	KIREG,	2,	TREG,	MRES,	0,	KIREG	},
	{	KIREG,	2,	1,	MRES,	1,	0	},
	{	KIREG,	2,	TREG,	MRES,	1,	KIREG	},
	{	KRREG,	MRES,	1,	2,	1,	0	},
	{	1,	2,	MRES,	MRES,	0,	0	},
	{	KRREG,	MRES,	1,	2,	0,	0	},
	{	1,	2,	TREG,	MRES,	1,	0	},
	{	KIREG,	MRES,	1,	2,	1,	0	},
	{	1,	2,	TREG,	MRES,	0,	0	},
	{	KIREG,	MRES,	1,	2,	0,	0	},
	{	1,	2,	TREG,	ARES,	0,	0	},
	/* The last line above is invalid, but I copied it from pfam[] above */
};

/*
 * get_se_operands()
 * sets up the global variable FP_src1 and FP_src2 
 * in the case of the source error on the scaler instruction and 
 * the pipelined instruction,  and also sets up FP_msrc1, FP_msrc2, 
 * FP_asrc1, FP_asrc2 in the case of the source error on the 
 * dual operation instruction.
 */   
get_se_operands(FPptr)
fp_frame_t *FPptr;
{

    ulong opcode,fpinst,*fpreg;
    struct dpc *dpctbl;
    int s1,s2;
    int s_prec = 0;
    int r_prec = 0;
    int pipelined = 0;
    
    FP_src1->n_double  = 0;
    FP_src2->n_double  = 0;
    FP_asrc1->n_double = 0;
    FP_asrc2->n_double = 0;
    *FP_fsr = 0;

    fpinst = *FPptr->fir;
    opcode = fpinst & 0x3f;  

    s1 = (fpinst >> 11) & 0x1f;
    s2 = (fpinst >> 21) & 0x1f;

    if (fpinst & FINS_S_PREC) {
        s_prec = 1;
	FPstatus |= IEEE_S_PREC;
    }
    if (fpinst & FINS_R_PREC) {
        r_prec = 1;
	FPstatus |= IEEE_R_PREC;
    }
    if (fpinst & FINS_PIPE) {
        FPstatus |= IEEE_PIPE_OP;
	pipelined = 1;
    }

    fpreg = (ulong *)&FPptr->fregs[0];
    if (opcode <= 31) {
        /*
         * dual operation instruction
         */
        FPstatus |= IEEE_DUAL_OP;
        dpctbl = pipelined ? &pfam[opcode & 0xf] : &pfmam[opcode & 0xf];
	
	if (s_prec) r_prec = 1; /* not allowed pfxxx.ds operations */

	switch (dpctbl->mop1) {
	case 1:		
	    fpassign(s_prec,FP_msrc1,fpreg+s1); 
	    break;	
	case KIREG: 	
	    if (s_prec) 
	        dpassign(FP_msrc1,&FPptr->ki); 
	    else 
		pack(FP_msrc1,&FPptr->ki);
	    break;
	case KRREG: 
  	    if (s_prec) 
	        dpassign(FP_msrc1,&FPptr->kr); 
	    else 
	        pack(FP_msrc1,&FPptr->kr);
	    break;
	}

	switch(dpctbl->mop2) {
	case 1:		
	    fpassign(s_prec,FP_msrc2,fpreg+s1); break;
	case 2:		
	    fpassign(s_prec,FP_msrc2,fpreg+s2); break;
	case MRES:	
	    fpassign(s_prec,FP_msrc2,&FPptr->mres[2]); break;
	case ARES:	
	    fpassign(s_prec,FP_msrc2,&FPptr->ares[2]); break;
	}

	switch(dpctbl->aop1) {
	case 1: 	
	    fpassign(r_prec,FP_asrc1,fpreg+s1); break;
	case TREG: 	
	    if (s_prec) 
	        dpassign(FP_asrc1,&FPptr->t);
	    else 
	        pack(FP_asrc1,&FPptr->t);
	    break;
	case ARES: 	
	    fpassign(r_prec,FP_asrc1,&FPptr->ares[2]); break;
	case MRES: 	
	    fpassign(r_prec,FP_asrc1,&FPptr->mres[2]); break;
	}

	switch(dpctbl->aop2) {
	case 2:		
	    fpassign(r_prec,FP_asrc2,fpreg+s2); break;
	case MRES:	
	    fpassign(r_prec,FP_asrc2,&FPptr->mres[2]); break;
	case ARES:	
	    fpassign(r_prec,FP_asrc2,&FPptr->ares[2]); break;
	}

        /* Should KI or KR be loaded? */
        switch (dpctbl->kload) {
        case KIREG:
            FPstatus |= IEEE_KI_LOAD;
	    break;
        case KRREG:
            FPstatus |= IEEE_KR_LOAD;
            break;
        default:
	    break;
        }
        if (dpctbl->tload) 
            FPstatus |= IEEE_T_LOAD;
    }
    else {
        /*
         * pipeline or scalar instruction
         */
        fpassign(s_prec,FP_src1,fpreg+s1); 
	fpassign(s_prec,FP_src2,fpreg+s2);
    }
}   


extern void fbug_op(),fmul_op(),frcp_op(),frsqr_op(),fadd_op(),fsub_op();
extern void fix_op(),famov_op(),fgt_op(),fle_op(),feq_op(),ftrunc_op(); 

void (*fp_op_table[])() = {
    fbug_op, fmul_op, frcp_op,  frsqr_op, fadd_op, fsub_op, 
    fix_op,  famov_op, fgt_op,   fle_op,  feq_op, ftrunc_op };

re_execute_fp_instruction(op,src1,src2,res,s_prec,r_prec,fsr,psr)
int op;
fp_t *src1,*src2,*res;
int s_prec,r_prec;
ulong *fsr,*psr;
{
  (fp_op_table[op])(src1,src2,res,s_prec,r_prec,fsr,psr);
}

