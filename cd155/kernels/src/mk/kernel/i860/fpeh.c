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
 
#define FPDEBUG 1
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
 * $Log: fpeh.c,v $
 * Revision 1.4  1994/11/18  20:38:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:30:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/06  17:51:40  stans
 * more FPDEBUG conditional code to assist debugging.
 *
 * Revision 1.1  1992/08/26  10:42:20  stans
 * Initial revision
 *
 * Revision 2.3.1.1  92/05/27  00:40:16  jeffreyh
 * 	[stans@ssd.intel.com]
 * 	Support floating point exceptions.
 * 
 * Revision 2.3  91/06/18  20:51:06  jsb
 * 	New code and copyright from Intel.
 * 	[91/06/18  18:53:32  jsb]
 * 
 * Revision 2.2  91/06/17  15:44:38  jsb
 * 	From Paul Pierce: created from iPSC/860 NX code.
 * 	[91/06/17  13:42:50  jsb]
 * 
 */

/*************************************************************
*                                                            *
*
* $Id: fpeh.c,v 1.4 1994/11/18 20:38:56 mtm Exp $
*
* [ Wed Jun 24 11:55:11 PDT 1992 ] stan			     *
* updated to NX version: fpeh.c 10.2 92/06/17 10:52:56	     *
*							     *
*  This module contains the main procedures of the i860FPEH. *
*                                                            *
*  Author: Tevi Devor Nov/87 ISWP                            *
*  fpeh.c 8.6 91/01/10 10:06:18                              *
*                                                            *
*************************************************************/

#include <i860/psl.h>
#include <i860/thread.h>
#include <i860/fpdefs.h>

#if FPDEBUG
int	fpe_debug=0;	/* TRUE enable noisy debug output */
#define	dprintf if (fpe_debug) printf
#endif

struct {
	int	fp_traps;
	int	fps_traps;
	int	fpi_traps;
	int	fpu_traps;
	int	fpo_traps;
} fpeh_info;

extern void turn_on_msb(), to_double_nan(), to_single_nan();
extern int nan_cmp_gt(), infinity(), quiet_nan(),
	signaling_nan();
extern int is_zero(), is_positive, is_negative();
extern int xor_signs(), get_from_fpreg(), get_kr(), get_ki(), get_t();
extern int get_mres2(), get_ares2();
extern void place_in_fpreg(), determine_inxct_rnd(), do_a_r_shift_d();
extern int do_a_l_shift_d();
extern i860_type add_func(), sub_func(), mul_func(), rcp_func(), rsqr_func(),
	fix_func(), feq_func(), fgt_func(), trunc_func(), dummy_func(),
	a_unit_func(), do_fmul(), do_frcp(), do_frsqr(), do_fadd(),
	do_fsub(), do_famov();
extern void to_double_precision();

void handle_unusual_src(), handle_invalid_operation(), handle_div_by_zero(),
	handle_overflow(), handle_underflow(), handle_inexact(),
	round_overflow(), fp_get_inputs(), place_se_result(),
	compute_biased();
/**************** GLOBAL VARIABLES ****************************/

FPtrapcontrol *FPptr;

int dualop_A_src, dualop_M_src, result_error;

word32 fsr;

i860_type temp_result1, temp_result2;

word32 *IEEE_status_ptr;

word32 inst_src1, inst_src2, dummy;

struct {
    int (*M_op1) ();
    word32 *param1;
    int (*M_op2) ();
    word32 *param2;
    int (*A_op1) ();
    word32 *param3;
    int (*A_op2) ();
    word32 *param4;
    int T_load;
    int Kr_load;
    int Ki_load;
} dpc_table[16]
= {
    get_kr, &dummy, get_from_fpreg, &inst_src2,
    get_from_fpreg, &inst_src1, get_mres2, &dummy,
    FALSE, FALSE, FALSE,

    get_kr, &dummy, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_mres2, &dummy,
    FALSE, TRUE, FALSE,

    get_kr, &dummy, get_from_fpreg, &inst_src2,
    get_from_fpreg, &inst_src1, get_ares2, &dummy,
    TRUE, FALSE, FALSE,

    get_kr, &dummy, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_ares2, &dummy,
    TRUE, TRUE, FALSE,

    get_ki, &dummy, get_from_fpreg, &inst_src2,
    get_from_fpreg, &inst_src1, get_mres2, &dummy,
    FALSE, FALSE, FALSE,

    get_ki, &dummy, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_mres2, &dummy,
    FALSE, FALSE, TRUE,

    get_ki, &dummy, get_from_fpreg, &inst_src2,
    get_from_fpreg, &inst_src1, get_ares2, &dummy,
    TRUE, FALSE, FALSE,

    get_ki, &dummy, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_ares2, &dummy,
    TRUE, FALSE, TRUE,

    get_kr, &dummy, get_ares2, &dummy,
    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    TRUE, FALSE, FALSE,

    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    get_ares2, &dummy, get_mres2, &dummy,
    FALSE, FALSE, FALSE,

    get_kr, &dummy, get_ares2, &dummy,
    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    FALSE, FALSE, FALSE,

    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_ares2, &dummy,
    TRUE, FALSE, FALSE,

    get_ki, &dummy, get_ares2, &dummy,
    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    TRUE, FALSE, FALSE,

    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_mres2, &dummy,
    FALSE, FALSE, FALSE,

    get_ki, &dummy, get_ares2, &dummy,
    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    FALSE, FALSE, FALSE,

    get_from_fpreg, &inst_src1, get_from_fpreg, &inst_src2,
    get_t, &dummy, get_ares2, &dummy,
    FALSE, FALSE, FALSE
};

i860_type(*func_table[NUM_OF_OPCODES]) () =
{
    mul_func, rcp_func, rsqr_func, add_func, sub_func,
    fix_func, fgt_func,
    fgt_func, feq_func,
    (i860_type(*) ()) NOT_USED,
    mul_func, mul_func,
    do_famov
};

i860_type(*func_table1[NUM_OF_OPCODES]) () =
{
    do_fmul, do_frcp, do_frsqr, do_fadd, do_fsub,
    (i860_type(*) ()) NOT_USED, (i860_type(*) ()) NOT_USED,
    (i860_type(*) ()) NOT_USED, (i860_type(*) ()) NOT_USED,
    (i860_type(*) ()) NOT_USED,
    do_fmul, do_fmul,
    do_famov
};


/*
 * first set sticky status bits appropriately, then determine what
 * value should be returned according to non sticky status bits and
 * mask bits
 */

int
determine_return_value()
{
    register word32 IEEE_status_word, M_unit_status, A_unit_status;
    register word32 IEEE_masks, sticky_bits, statuses_raised;

    IEEE_status_word = *IEEE_status_ptr;
#if FPDEBUG
    dprintf("Enter determine return value, IEEE_status_word %x\n",IEEE_status_word);
#endif
    M_unit_status = IEEE_status_word & M_STATUS_MASK;
    A_unit_status = (IEEE_status_word & A_STATUS_MASK) >> A_STATUS_SHIFT;
    IEEE_masks = (IEEE_status_word & IEEE_MASK_MASK) >> IEEE_MASK_SHIFT;

    sticky_bits = (IEEE_status_word & STICKY_STATUS_MASK)>>STICKY_STATUS_SHIFT;
    statuses_raised = M_unit_status | A_unit_status;
    sticky_bits |= statuses_raised;
    *IEEE_status_ptr |= (sticky_bits << STICKY_STATUS_SHIFT);
#if FPDEBUG
    dprintf("  statuses_raised %lx IEEE_masks %lx ~IEEE_masks %lx\n",
	  statuses_raised, IEEE_masks, ~IEEE_masks);
#endif
    if (statuses_raised & (~IEEE_masks)) {
#if FPDEBUG
	dprintf("  Returning call_user_handle\n");
#endif
	return (CALL_USER_HANDLER);
    } else
#if FPDEBUG
	dprintf("  Returning no_user_handle\n");
#endif
    return (NO_USER_HANDLER);
}

/*
 *	result_error_nonmasked
 *
 * inputs:
 *	none.
 *
 * outputs:
 *	0 == ??
 *	1 == ??
 *
 * side effects:
 *
 */
int
result_error_nonmasked()
{

    register word32 IEEE_masks, fsr_result_error;
    register unsigned IEEE_masks_bit1, IEEE_masks_bit2;

/*
 *  msb                                    lsb 
 * +------+---+---+---+---+---+---+---+---+---+
 * !--------------! 9 ! 7 ! 8 ! - ! 9 ! 7 ! 8 !
 * +______+___+___+___+___+___+___+___+___+___+
 *       +---+                            +---+
 * where ! - ! means 0 in this * bit, and ! 9 ! means bit #9 of IEEE_status_word
 *       +___+                            +___+
 * wants IEEE_masks to be in this position
 */

    IEEE_masks = (*IEEE_status_ptr & IEEE_MASK_MASK) >> (IEEE_MASK_SHIFT + 2);
    IEEE_masks &= 0x07;
    IEEE_masks_bit1 = IEEE_masks & 0x01;
    IEEE_masks_bit2 = IEEE_masks & 0x02;
    if (IEEE_masks_bit1)
	IEEE_masks |= 0x02;
    else
	IEEE_masks &= 0x05;
    if (IEEE_masks_bit2)
	IEEE_masks |= 0x01;
    else
	IEEE_masks &= 0x06;
    IEEE_masks |= (IEEE_masks << 4);
/*
* now we set up fsr_result_error to
*   msb                                    lsb
*  +------+---+---+---+---+---+---+---+---+---+
*  !--------------!AI !AO !AU ! - !MI !MU !MO !
*  +______+___+___+___+___+___+___+___+___+___+
*/

    fsr_result_error = (FPptr->fsrs[2] & RESULT_ERROR_BITS)>>RESULT_ERROR_SHIFT;
    fsr_result_error &= 0x077;
    /*
     * now and the complement of IEEE_masks and fsr_result_error tells
     * us if unmasked result error occured
     */
#if FPDEBUG
    dprintf("result_error_nonmasked() fsr_result_error %lx IEEE_masks %lx ~IEEE_masks %lx\n",
	  fsr_result_error, IEEE_masks, ~IEEE_masks);
    dprintf("result_error_nonmasked() returns %d\n",
	(fsr_result_error & (~IEEE_masks) ? 1 : 0));
#endif

    if (fsr_result_error & (~IEEE_masks))
	return (1);
    else
	return (0);
}

/*****************************************************************
*
*  handle_exception
*
*   Called by the main i860 trap handler (ctrap.c), when bit #0 of the
*   exception_handler pointer is clear or when the exception is masked
*   (as specified in the mask bits of IEEE_status_word).
*
*  Calls
*
*  determine_fault
*  handle_unusual_src
*  handle_invalid_operation
*  handle_div_by_zero
*  handle_overflow
*  handle_underflow
*  handle_inexact
*
*
*  Global Variables
*
*  Note that (@) means global variable referenced but not set in
*  this procedure.
*
*  fsr -   is initialized here to 0 see compute_temp_result and
*          place_se_result for further details
*  (@) result_error - specifies if floating point trap is from source
*                     or result error.
*
*
* inputs:
*	FP	i860 exception frame as seen via the struct FPtrapcontrol
*	pcb	pcb pointer.
* outputs:
*	CALL_USER_HANDLER
*	NO_USER_HANDLER
 *	UNIX_DEFAULT
*
* side effects:
*
*********************************************************************/
int 
handle_FP_exception( FP, pcb )
	FPtrapcontrol_t	FP;
	pcb_t		pcb;
{
    register word32		fault_to_handle;
#if FPDEBUG
    word32 IEEE_status_TEMP;
    word32 IEEE_masks_TEMP;
#endif
    unsigned determine_fault();
    unsigned fp_set_mask();

   /* XXX global? make me (FPptr) totally local someday */
    FPptr = FP;

    /* ieee status pointer from pcb state. */
    IEEE_status_ptr = (word32 *)&pcb->ieee_fp_mask; /* XXX set more globals.. */

#if FPDEBUG
    dprintf("\nEntering FP exception handler:\n  FIR 0x%lx INSTR %lx OP %lx\nFSR %lx %lx %lx PSR %lx\n",
	FP->trapped_instruction, *(word32 *)FP->trapped_instruction,
	FP->trapped_opcode, FP->fsrs[0], FP->fsrs[1], FP->fsrs[2], FP->psr);
    print_FP( FP );
#endif

#if FPDEBUG1
    IEEE_status_TEMP = *IEEE_status_ptr;
    IEEE_masks_TEMP = (IEEE_status_TEMP & IEEE_MASK_MASK) >> IEEE_MASK_SHIFT;

    dprintf("IEEE_masks_TEMP %lx ~IEEE_masks_TEMP %lx\n",
	  IEEE_masks_TEMP, ~IEEE_masks_TEMP);
#endif

    fsr = 0;	/* XXX pretty weak... */

    fpeh_info.fp_traps++;

    fault_to_handle = determine_fault( FP );
    /*
     * 'fault_to_handle' can assume one of the following:
     *
     *  0x0  -- No Error detected
     *  0x1  -- i860 Source Error, no IEEE exception.
     *  0x2  -- Invalid Operation.
     *  0x4  -- Divide by zero.
     *  0x8  -- Overflow.
     *  0x10 -- Underflow.
     *  0x20 -- Inexact.
     *  0x40 -- IEEE Invalid operation on one pair of sources other unknown
     *          ,causes a call to handle_one_of_each from handle_exception.
     *  On a dualop the procedure may return the bit pattern resulting from
     *  the oring of any two of Overflow Underflow and Inexact
     */

#if FPDEBUG
    dprintf("determine_fault() returned: fault %lx, result_error %lx\n",
	fault_to_handle,result_error);
#endif
    if (result_error) {		/* set in determine fault */
#if FPDEBUG
	dprintf("result error is %x\n",result_error);
#endif

	if ( result_error_nonmasked() ) {
		/* unmasked/disabled exception occured */
#if FPDEBUG
		dprintf("handle_FP_exception() unmasked exception, returns UNIX_DEFAULT(%d).\n",UNIX_DEFAULT);
#endif
		return (UNIX_DEFAULT);	/* take unix default signal action */
	}
	if (fault_to_handle != INEXACT_TRAP)
	    fp_get_inputs( FP );

	if (fault_to_handle & OVERFLOW_TRAP)
		handle_overflow( FP );

	if (fault_to_handle & UNDERFLOW_TRAP)
		handle_underflow( FP );

	if (fault_to_handle & INEXACT_TRAP)
		handle_inexact( FP );
    }
    else {
	switch (fault_to_handle) {

	case SOURCE_ERR_VALID_OPER:
	    handle_unusual_src( FP );
#if FPDEBUG1
	    dprintf("after handle_unus in handle excep ares[0] is :");
	    print_i860type(&FP->ares[0]);
#endif
	break;

	case INVALID_OPERATION:
	    if (/* signals.usig[SIGFPE] == SIG_DFL */ 1 
		    && !(*IEEE_status_ptr & INVALID_MASK))
		return (UNIX_DEFAULT);	/* take unix default action */
	    else
		handle_invalid_operation( FP );
	break;

	case DIV_BY_ZERO_TRAP:
	    if (/* signals.usig[SIGFPE] == SIG_DFL */ 1 
		    && !(*IEEE_status_ptr & DIV_BY_ZERO_MASK))
		return (UNIX_DEFAULT);	/* take unix default action */
	    else
		handle_div_by_zero( FP );
	break;

	case ONE_UNKNOWN_AND_IEEE_ERROR:
	    if (/* signals.usig[SIGFPE] == SIG_DFL */ 1 
		    && !(*IEEE_status_ptr & INVALID_MASK))
		return (UNIX_DEFAULT);	/* take unix default action   */
	    else
		handle_one_of_each( FP );
	break;
/* rag B2 errata #26 and #34 */
/* do nothing */
	case NO_ERROR:
	break;
/* rag B2 errata #26 and #34 */
	} /* end switch() */
    }

    /* added oct 1988, turn off se bit in fsr */
    FP->fsrs[2] &= 0xfffffeff;

    return ( determine_return_value() );
}

/*****************************************************************
*
*  determine_fault()
*
*  Determine what type of fault occured. In case of IEEE invalid
*  source the result(s) are set within this procedure. In all cases
*  a code is returned identifying the type of error that occured.
*
*  Called by
*
*  handle_exception
*
*  Calls
*
*  fp_get_inputs
*
*
* Global Variables:
*
*  Global Inputs:
*
*	dualop_M_src,dualop_A_src - are flags set if source errors occured
*	in M and/or A units on dual operation. NO_ERROR means no source
*	error found, i860_ERROR  means i860 source error, which is IEEE valid,
*	found, IEEE_ERROR means IEEE invalid error found.
*
*  Global Outputs:
*	result_error - set if a result error was detected.
*	temp_result1,temp_result2 - are the results computed directly by
*	this procedure when we have an IEEE invalid operand. temp_result2 is for
*	the M unit when we have a dual op.
*
*  Returns
*
*  a bit pattern as follows
*  0x0  -- No Error detected
*  0x1  -- i860 Source Error, no IEEE exception.
*  0x2  -- Invalid Operation.
*  0x4  -- Divide by zero.
*  0x8  -- Overflow.
*  0x10 -- Underflow.
*  0x20 -- Inexact.
*  0x40 -- IEEE Invalid operation on one pair of sources other unknown
*          ,causes a call to handle_one_of_each from handle_exception.
*  On a dualop the procedure may return the bit pattern resulting from
*  the oring of any two of Overflow Underflow and Inexact
*
***********************************************************************/

word32 
determine_fault( FP )
	FPtrapcontrol	*FP;	
{
    register word32 return_code = NO_ERROR;
    register word32 last_stage_fsr;
    register word32 trapped_opcode;
    int snan1 = 0;
    int snan2 = 0;
#if FPDEBUG
    dprintf("Entering determine_fault, last_stage_fsr %x\n",FP->fsrs[2]);
#endif
    result_error = FALSE;
    last_stage_fsr = FP->fsrs[2];

    /* result errors */
    if ((last_stage_fsr & M_OVERFLOW_OCCURED_BIT)
	    || (last_stage_fsr & A_OVERFLOW_OCCURED_BIT)) {
	return_code = return_code | OVERFLOW_TRAP;
	fpeh_info.fpo_traps++;
    }

    if ((last_stage_fsr & M_UNDERFLOW_OCCURED_BIT)
	    || (last_stage_fsr & A_UNDERFLOW_OCCURED_BIT)) {
	return_code = return_code | UNDERFLOW_TRAP;
	fpeh_info.fpu_traps++;
    }

    if (!(last_stage_fsr & TI_BIT)
	    && ((last_stage_fsr & M_INEXACT_OCCURED_BIT)
		|| (last_stage_fsr & A_INEXACT_OCCURED_BIT))) {
	return_code = return_code | INEXACT_TRAP;
	fpeh_info.fpi_traps++;
    }

    if (return_code != NO_ERROR) {
#if FPDEBUG
	dprintf("  return code != NO_ERROR\n");
#endif
	result_error = TRUE;
	return (return_code);
    }

/*
 * rag B2 errata #26 and #34
 * check if no SE and just return,
 * don't count on SE being set when FT is set and no result exceptions are set.
 */
    if (!(last_stage_fsr & 0x100)) {
#if FPDEBUG
	dprintf("  FT with NO exception bits set.\n");
#endif
	return (return_code);	/* NO_ERROR */
    }
/*
 * rag B2 errata #26 and #34
 */
#if FPDEBUG
    dprintf("\n");
#endif
    fpeh_info.fps_traps++;

    /* source error */
    fp_get_inputs( FP );

#if FPDEBUG1
    printf("After get inputs %lx\n", *IEEE_status_ptr);
    printf("FP1 ");
    print_i860type(&(FP->fp1));
    printf("FP2 ");
    print_i860type(&FP->fp2);
    printf("FP3 ");
    print_i860type(&FP->fp3);
    printf("FP4 ");
    print_i860type(&FP->fp4);
#endif
    dualop_M_src = NO_ERROR;
    dualop_A_src = NO_ERROR;
    /*
     * for the opcodes fld pfld fst pst and ixfr if this is a single mode
     * instruction or the companion instruction is not a multiplier or
     * adder instruction, then ignore source error, we know to do this in
     * accordance with the flag bit in IEEE_status_word set in
     * fp_get_inputs
     */

    trapped_opcode = FP->trapped_opcode;
#if FPDEBUG
    dprintf("  Trapped op = %lx, FRCP is %lx\n", trapped_opcode, FRCP);
#endif
    if ((trapped_opcode == FRCP || trapped_opcode == FRSQR)
	    && is_zero(&FP->fp2)) {
#if FPDEBUG
	dprintf("  divide by 0\n");
#endif
	return (DIV_BY_ZERO_TRAP);
    }
    /* handle case of pair of sources containing SNaN
     * don't do- if (snan1 =   || snan2 = ).
     * It is eminnent that snan1 and snan2 get true values
     */
    snan1 = signaling_nan(&FP->fp1, FALSE);
    snan2 = signaling_nan(&FP->fp2, FALSE);

    if (snan1 || snan2) {
#if FPDEBUG
	dprintf("  signaling nan is true\n");
#endif
	return_code = INVALID_OPERATION;
	if (trapped_opcode == PFGT 
		|| trapped_opcode == PFEQ
		|| trapped_opcode == PFLE) {
	    if (trapped_opcode == PFLE)
		FP->psr |= SET_CC_BIT;
	    else
		FP->psr &= CLEAR_CC_BIT;
	    temp_result1.n_double = 0;
	    return (return_code);
	}
	if (trapped_opcode == FAMOV)
	    temp_result1 = FP->fp1;

/* DKG - MURALI:
*	If both operands are Snans, return the larger mantissa
*	and set the msb of the exponent.
*	If only one operand is a SNAN
*	then
*		If single precision, or double precision and the first operand
*		is not a QNAN, return a QNAN by converting the msb of the
*		exponent.
*	else	
*		return the QNAN.
*/

	else {
#if FPDEBUG
	    dprintf("  checking the Snans\n");
#endif
	    if (*IEEE_status_ptr & R_PRECISION_BIT)	/* double */
		if (snan1 && snan2) {	/* both are snans */
		    if (nan_cmp_gt(&FP->fp1, &FP->fp2, FALSE))
			temp_result1 = FP->fp1;
		    else
			temp_result1 = FP->fp2;
		} else if (snan1)
		    temp_result1 = FP->fp1;
		else {		/* SNAN in second operand */
		    if (quiet_nan(&FP->fp1, FALSE))
			temp_result1 = FP->fp1;
		    else
			temp_result1 = FP->fp2;
		}
	    else {		/* single */
		if (snan1 && snan2) {
		    if (nan_cmp_gt(&FP->fp1, &FP->fp2, FALSE))
			temp_result1 = FP->fp1;
		    else
			temp_result1 = FP->fp2;
		} else {
		    temp_result1 = (snan1 > snan2) ? FP->fp1 : FP->fp2;
		}
	    }
	}
	/* handle case where source precision != result precision */
	if (((*IEEE_status_ptr & R_PRECISION_BIT) << 1) !=
		(*IEEE_status_ptr & S_PRECISION_BIT)) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT)
		to_double_nan(&temp_result1);
	    else
		to_single_nan(&temp_result1);
	} else			/* precisions are equal */
	    turn_on_msb(&temp_result1, FALSE);
	if (*IEEE_status_ptr & DUALOP_BIT)
	    dualop_M_src = IEEE_ERROR_d;
	else
	    return (return_code);
    }
    if (*IEEE_status_ptr & DUALOP_BIT) {
	snan1 = signaling_nan(&FP->fp3, FALSE);
	snan2 = signaling_nan(&FP->fp4, FALSE);

	if (snan1 || snan2) {
	    if (trapped_opcode == FAMOV)
		temp_result2 = FP->fp3;
	    else {
		if (*IEEE_status_ptr & R_PRECISION_BIT)	/* double */
		    if (snan1 && snan2) {
			if (nan_cmp_gt(&FP->fp3, &FP->fp4, FALSE))
			    temp_result1 = FP->fp3;
			else
			    temp_result1 = FP->fp4;
		    } else {
			if (snan1)
			    temp_result1 = FP->fp3;
			else if (quiet_nan(&FP->fp3, FALSE))
			    temp_result1 = FP->fp3; /* SNAN in 2nd operand */
			else
			    temp_result1 = FP->fp4;
		    }
		else {		/* single */
		    if (snan1 && snan2)
			if (nan_cmp_gt(&FP->fp3, &FP->fp4, FALSE))
			    temp_result1 = FP->fp3;
			else
			    temp_result1 = FP->fp4;
		    else
			temp_result1 = (snan1 > snan2) ? FP->fp1 : FP->fp2;
		}
	    }
	    turn_on_msb(&temp_result2, TRUE);
	    dualop_A_src = IEEE_ERROR_d;
	    return_code = INVALID_OPERATION;
	    if (dualop_A_src == IEEE_ERROR_d && dualop_M_src == IEEE_ERROR_d)
		return (return_code);
	}
    }
    /*
     * handle other cases where one of the sources is an IEEE invalid
     */

    switch (trapped_opcode) {

    case PFAM:
	if (dualop_M_src == NO_ERROR) {
	    if ((infinity(&FP->fp1, FALSE) && is_zero(&FP->fp2)) ||
		 (infinity(&FP->fp2, FALSE) && is_zero(&FP->fp1))) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		    temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
		} else {
		    temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		    temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
		}
		dualop_M_src = IEEE_ERROR_d;
		return_code = INVALID_OPERATION;
	    }
	}
	if (dualop_A_src == NO_ERROR) {
	    if ((infinity(&FP->fp3, TRUE) * infinity(&FP->fp4, TRUE))
		== -1) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result2.two_words.top_word32 = DEFAULT_RESULT_D_U;
		    temp_result2.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
		} else {
		    temp_result2.two_words.top_word32 = DEFAULT_RESULT_S_U;
		    temp_result2.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
		}
		dualop_A_src = IEEE_ERROR_d;
		return_code = INVALID_OPERATION;
	    }
	}
    break;

    case PFSM:
	if (dualop_M_src == NO_ERROR) {
	    if ((infinity(&FP->fp1, FALSE) && is_zero(&FP->fp2)) ||
		 (infinity(&FP->fp2, FALSE) && is_zero(&FP->fp1))) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		    temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
		} else {
		    temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		    temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
		}
		dualop_M_src = IEEE_ERROR_d;
		return_code = INVALID_OPERATION;
	    }
	}
	if (dualop_A_src == NO_ERROR) {
	    if ((infinity(&FP->fp3, TRUE) * infinity(&FP->fp4, TRUE))
		== 1) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result2.two_words.top_word32 = DEFAULT_RESULT_D_U;
		    temp_result2.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
		} else {
		    temp_result2.two_words.top_word32 = DEFAULT_RESULT_S_U;
		    temp_result2.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
		}
		dualop_A_src = IEEE_ERROR_d;
		return_code = INVALID_OPERATION;
	    }
	}
    break;

    case FADD:

	if ((infinity(&FP->fp1, FALSE) * infinity(&FP->fp2, FALSE))
	    == -1) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    case FIX:

	if (infinity(&FP->fp1, FALSE)) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    case FTRUNC:

	if (infinity(&FP->fp1, FALSE)) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    case FSUB:
	if ((infinity(&FP->fp1, FALSE) * infinity(&FP->fp2, FALSE)) == 1) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    case PFEQ:
    case PFGT:
    case PFLE:
	if (quiet_nan(&FP->fp1, FALSE) || quiet_nan(&FP->fp2, FALSE)) {
	    if (trapped_opcode == PFLE)
		FP->psr |= SET_CC_BIT;
	    else
		FP->psr &= CLEAR_CC_BIT;
	    return_code = INVALID_OPERATION;
	    temp_result1.n_double = 0;
	}
    break;

    case FRSQR:
	if (is_negative(&FP->fp2)) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    case FMUL:
	if ((infinity(&FP->fp1, FALSE) && is_zero(&FP->fp2)) ||
		(infinity(&FP->fp2, FALSE) && is_zero(&FP->fp1))) {
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_D_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_D_B;
	    } else {
		temp_result1.two_words.top_word32 = DEFAULT_RESULT_S_U;
		temp_result1.two_words.bottom_word32 = DEFAULT_RESULT_S_B;
	    }
	    return_code = INVALID_OPERATION;
	}
    break;

    default:
    	break;
    } /* switch */

    if (*IEEE_status_ptr & DUALOP_BIT) {
	if (dualop_M_src == NO_ERROR && dualop_A_src == NO_ERROR)
	    /*
	     * this means that we have an i860 invalid operand which is
	     * IEEE valid
	     */
	    return_code = SOURCE_ERR_VALID_OPER;

	else if (dualop_M_src != IEEE_ERROR_d || dualop_A_src != IEEE_ERROR_d)
	    /*
	     * if only one of the pairs has been found to be IEEE invalid
	     * this case will be handled by handle_one_of_each
	     */
	    return_code = ONE_UNKNOWN_AND_IEEE_ERROR;
    } else if (return_code == NO_ERROR)
	return_code = SOURCE_ERR_VALID_OPER;
    return (return_code);
}

/*****************************************************************
*
*  handle_unusual_src()
*
*  Calls compute_temp_result with the proper parameters to obtain
*  the IEEE standard result(s) and then call place_se_result to place
*  the obtained result(s).
*
*  Called by
*
*  Main program.
*
*  Calls
*
*  compute_temp_result
*  place_se_result
*
*  Global Variables
*
*  temp_result1,temp_result2 - are the results computed by
*  this procedure when we have an i860 invalid operand.
*  temp_result2 is for the M unit when we have a dual op.
*  (@) *IEEE_status_ptr & DUALOP_BIT - set by fp_get_inputs.
*
********************************************************************/

void
handle_unusual_src( FP )
        FPtrapcontrol   *FP;
{
    i860_type compute_temp_result();
#if FPDEBUG
    dprintf("Entering handle_unusual_src\n");
#endif
    temp_result1 = compute_temp_result(FP->fp1, FP->fp2, FALSE);
#if FPDEBUG1
    dprintf("temp_result1 in handle_unusual ");
    print_i860type(&temp_result1);
#endif
    if (*IEEE_status_ptr & DUALOP_BIT)
	temp_result2 = compute_temp_result(FP->fp3, FP->fp4, TRUE);

    place_se_result( FP );
}


/*****************************************************************
*
*  handle_one_of_each()
*
*  Called only when the dualop had one pair of sources that were
*  IEEE invalid and the other are as yet undetermined. Calls
*  compute_temp_result with the proper parameters to obtain the IEEE
*  standard result for the pair that are as yet undetermined. The
*  other result has already been computed in determine_fault. Finally
*  place_se_result is called to place the results
*
*  Called by
*
*  handle_exception
*
*  Calls
*
*  compute_temp_result
*  place_se_result
*
*  Global Variables
*
*  temp_result1,temp_result2 - are the results computed by
*                              this procedure.
*  (@) dulaop_M_src - set in detrmine fault
*
***********************************************************************/

int
handle_one_of_each( FP )
        FPtrapcontrol   *FP;
{
    i860_type compute_temp_result();
#if FPDEBUG
    dprintf("Entering handle_one_of_each\n");
#endif
    if (dualop_M_src != IEEE_ERROR_d) {
	*IEEE_status_ptr |= A_INV_OPER_BIT;
	temp_result1 = compute_temp_result(FP->fp1, FP->fp2, FALSE);
    } else {
	*IEEE_status_ptr |= M_INV_OPER_BIT;
	temp_result2 = compute_temp_result(FP->fp3, FP->fp4, TRUE);
    }
    place_se_result( FP );
}

/*****************************************************************
*
*  compute_temp_result(src1,src2,int A_part_of_dualop)
*
*  compute and return the IEEE specified result given the sources
*  and the opcode.
*
*  Called by
*
*  handle_unusual_src
*  handle_one_of_each
*
*  Input parameters
*
*  src1,src2 - the sources for which the result must be computed.
*  A_part_of_dualop - True for add/sub part dualop instructions.
*
*
*  Global Variables
*
*  fsr - the current value of the fsr (returned by a call to getfsr)
*        is placed into this variable
*
*  Calls
*
*  reexecute_instruction
*
********************************************************************/

i860_type 
compute_temp_result(src1, src2, A_part_of_dualop)
    i860_type src1, src2;
    int A_part_of_dualop;
{
    register int sign1, sign2, sign;
    i860_type temp_result, reexecute_instruction();
#if FPDEBUG
    dprintf("Entering compute_temp_result (A_part? %lx)\n",A_part_of_dualop);
    print_i860type(&src1);
    print_i860type(&src2);
#endif
    if ((quiet_nan(&src1, A_part_of_dualop))
	|| (quiet_nan(&src2, A_part_of_dualop)))
	if (nan_cmp_gt(&src1, &src2, A_part_of_dualop)) {
	    if (((*IEEE_status_ptr & R_PRECISION_BIT) << 1) !=
		    (*IEEE_status_ptr & S_PRECISION_BIT)) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    if(!(A_part_of_dualop&&(*IEEE_status_ptr&DUALOP_BIT)))
			to_double_nan(&src1);
		} else {
		    to_single_nan(&src1);
		}
	    }
#if FPDEBUG
	    dprintf("compute_temp_result = ");
	    print_i860type(&src1);
#endif
	    return (src1);
	} else {
	    if (((*IEEE_status_ptr & R_PRECISION_BIT) << 1) !=
		    (*IEEE_status_ptr & S_PRECISION_BIT)) {
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    if(!(A_part_of_dualop&&(*IEEE_status_ptr&DUALOP_BIT)))
			to_double_nan(&src2);
		} else {
		    to_single_nan(&src2);
		}
	    }
#if FPDEBUG
	    dprintf("compute_temp_result = ");
	    print_i860type(&src1);
#endif
	    return (src2);
	}
    sign1 = infinity(&src1, A_part_of_dualop);
    sign2 = infinity(&src2, A_part_of_dualop);
#if FPDEBUG
    dprintf("SIGN1, SIGN2, TRAPPED_OPCODE = %lx %lx %lx\n",
	   sign1,sign2,FPptr->trapped_opcode);
#endif
    if (sign1 || sign2) {
	switch (FPptr->trapped_opcode) {
	case FAMOV:
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		temp_result.b_double.sign = (sign1 == -1) ? 1 : 0;
	    } else {
		temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		temp_result.b_single.sign = (sign1 == -1) ? 1 : 0;
	    }
	break;
	case FADD:
	    /*
	     * +infinity + +infinity = +infinity
	     * -infinity + -infinity = -infinity
	     * #infinity + #X        = $infinity
	     * #X        + #infinity = $infinity
	     * 
	     * #: means whatever sign
	     * $: means the sign of the infinity on the left hand side
	     */

	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		if (sign1 != 0)
		    temp_result.b_double.sign = (sign1 == -1) ? 1 : 0;
		else
		    temp_result.b_double.sign = (sign2 == -1) ? 1 : 0;
	    } else {
		temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		if (sign1 != 0)
		    temp_result.b_single.sign = (sign1 == -1) ? 1 : 0;
		else
		    temp_result.b_single.sign = (sign2 == -1) ? 1 : 0;
	    }
	break;

	case FSUB:
	    /*
	     * +infinity - -infinity = +infinity
	     * -infinity - +infinity = -infinity
	     * #infinity - #X        = $infinity
	     * #X        - #infinity = !infinity
	     * 
	     * !: means the opposite signed infinity of the l.h.s infinity
	     */

	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		if (sign1 != 0)
		    if (sign2 != 0)
			temp_result.b_double.sign = (sign2 == 1) ? 1 : 0;
		    else
			temp_result.b_double.sign = src1.b_double.sign;
		else
		    temp_result.b_double.sign = ~(src2.b_double.sign);
	    } else {
		temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		if (sign1 != 0)
		    if (sign2 != 0)
			temp_result.b_single.sign = (sign2 == 1) ? 1 : 0;
		    else
			temp_result.b_single.sign = src1.b_single.sign;
		else
		    temp_result.b_single.sign = ~(src2.b_single.sign);
	    }
	break;

	case PFEQ:
	    if ((src1.two_words.top_word32 == src2.two_words.top_word32) &&
	    (src1.two_words.bottom_word32 == src2.two_words.bottom_word32))
		FPptr->psr |= SET_CC_BIT;
	    else
		FPptr->psr &= CLEAR_CC_BIT;
	    temp_result.n_double = 0;
	break;

	case PFGT:
	case PFLE:
	    if (inf_gt(src1, src2))
		FPptr->psr |= SET_CC_BIT;
	    else
		FPptr->psr &= CLEAR_CC_BIT;
	    temp_result.n_double = 0;
	break;


	case FRSQR:
	    /*
	     * sqrt(+infinity) = infinity
	     */

	    temp_result.n_double = 0;
	break;

	case FRCP:
	    /*
	     * 1/+infinity     = 0  1/-infinity     = -0
	     */
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result.n_double = 0;
		temp_result.b_double.sign = (sign2 != 1);
	    } else {
		temp_result.n_double = 0;
		temp_result.b_single.sign = (sign2 != 1);
	    }
	break;

	case FMUL:
	    /*
	     * #infinity * #infinity = ~infinity
	     * #X        * #infinity = ~infinity
	     * #infinity * #X        = ~infinity
	     * 
	     * ~: means the xor of the two signs on the l.h.s
	     */

	    sign = xor_signs(src1, src2);
	    if (*IEEE_status_ptr & R_PRECISION_BIT) {
		temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		temp_result.b_double.sign = sign;
	    } else {
		temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		temp_result.b_single.sign = sign;
	    }
	break;

	case PFSM:
	    if (A_part_of_dualop) {
		/*
		 * +infinity - -infinity = +infinity
		 * -infinity - +infinity = -infinity
		 * #infinity - #X        = $infinity
		 * #X        - #infinity = !infinity
		 * 
		 * !: means the opposite signed infinity of the l.h.s infinity
		 */

		if (*IEEE_status_ptr & A_PRECISION_BIT) {
		    temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		    if (sign1 != 0)
			if (sign2 != 0)
			    temp_result.b_double.sign = (sign2 == 1) ? 1 : 0;
			else
			    temp_result.b_double.sign = src1.b_double.sign;
		    else
			temp_result.b_double.sign = ~(src2.b_double.sign);
		} else {
		    temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		    if (sign1 != 0)
			if (sign2 != 0)
			    temp_result.b_single.sign = (sign2 == 1) ? 1 : 0;
			else
			    temp_result.b_single.sign = src1.b_single.sign;
		    else
			temp_result.b_single.sign = ~(src2.b_single.sign);
		}
	    } else {
		/*
		 * #infinity * #infinity = ~infinity
		 * #X        * #infinity = ~infinity
		 * #infinity * #X        = ~infinity
		 * 
		 * ~: means the xor of the two signs on the l.h.s
		 */

		sign = xor_signs(src1, src2);
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		    temp_result.b_double.sign = sign;
		} else {
		    temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		    temp_result.b_single.sign = sign;
		}
	    }
	break;

	case PFAM:
	    if (A_part_of_dualop) {
		/*
		 * +infinity + +infinity = +infinity
		 * -infinity + -infinity = -infinity
		 * #infinity + #X        = $infinity
		 * #X        + #infinity = $infinity
		 * 
		 * #: means whatever sign
		 * $: means the sign of the infinity on the left hand side
		 */

		if (*IEEE_status_ptr & A_PRECISION_BIT) {
		    temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		    if (sign1 != 0)
			temp_result.b_double.sign = (sign1 == -1) ? 1 : 0;
		    else
			temp_result.b_double.sign = (sign2 == -1) ? 1 : 0;
		} else {
		    temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		    if (sign1 != 0)
			temp_result.b_single.sign = (sign1 == -1) ? 1 : 0;
		    else
			temp_result.b_single.sign = (sign2 == -1) ? 1 : 0;
		}
	    } else {
		/*
		 * #infinity * #infinity = ~infinity
		 * #X        * #infinity = ~infinity
		 * #infinity * #X        = ~infinity
		 * 
		 * ~: means the xor of the two signs on the l.h.s
		 */

		sign = xor_signs(src1, src2);
		if (*IEEE_status_ptr & R_PRECISION_BIT) {
		    temp_result.two_words.top_word32 = POS_INFINITY_D_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_D_B;
		    temp_result.b_double.sign = sign;
		} else {
		    temp_result.two_words.top_word32 = POS_INFINITY_S_U;
		    temp_result.two_words.bottom_word32 = POS_INFINITY_S_B;
		    temp_result.b_single.sign = sign;
		}
	    }
	break;

	} /* switch */
    } else {			/* either denormal or good part of dualop */
	temp_result = reexecute_instruction(src1, src2, A_part_of_dualop);
    }

#if FPDEBUG
    dprintf("compute_temp_result = ");
    print_i860type(&temp_result);
#endif
    return (temp_result);
}

/*****************************************************************
*
*  handle_invalid_operation()
*
*  Handle case when IEEE invalid operation occurred, note that
*  dualop never results in a call to this procedure.
*
*  Called by
*
*  handle_exception
*
*  Calls
*
*  place_se_result
*
*********************************************************************/

void
handle_invalid_operation( FP )
        FPtrapcontrol   *FP;
{
#if FPDEBUG
    dprintf("Entering handle_invalid_operation\n");
#if FPDEBUG1
    print_i860type(&temp_result1);
    print_i860type(&temp_result2);
#endif
#endif

    if (!(*IEEE_status_ptr & DUALOP_BIT) || dualop_M_src == IEEE_ERROR_d) {
	*IEEE_status_ptr |= M_INV_OPER_BIT;
    }

    if (dualop_A_src == IEEE_ERROR_d) {
	*IEEE_status_ptr |= A_INV_OPER_BIT;
    }

    place_se_result( FP );
}


/*****************************************************************
*
*  handle_div_by_zero()
*
*  Handle divide by zero exception.
*
*  Called by
*
*  handle_exception
*
*  Calls
*
*
*  place_se_result
*
*  Global Variables
*
*  temp_result1 - is the result computed by
*                 this procedure.
*
**********************************************************************/

void
handle_div_by_zero( FP )
        FPtrapcontrol   *FP;
{
#if FPDEBUG
    dprintf("Entering handle_div_by_zero\n");
#endif
    *IEEE_status_ptr |= M_DIV_BY_ZERO_BIT;

    /* src2 is either positive zero or negative zero */
    if (*IEEE_status_ptr & S_PRECISION_BIT) {
	if ((FP->fp2.b_double.sign == 1) && FP->trapped_opcode == FRCP) {
	    temp_result1.two_words.top_word32 = NEG_INFINITY_D_U;
	    temp_result1.two_words.bottom_word32 = NEG_INFINITY_D_B;
	} else {
	    temp_result1.two_words.top_word32 = POS_INFINITY_D_U;
	    temp_result1.two_words.bottom_word32 = POS_INFINITY_D_B;
	}
    }
    else if (*IEEE_status_ptr & R_PRECISION_BIT) {
	if ((FP->fp2.b_single.sign == 1) && FP->trapped_opcode == FRCP) {
	    temp_result1.two_words.top_word32 = NEG_INFINITY_D_U;
	    temp_result1.two_words.bottom_word32 = NEG_INFINITY_D_B;
	} else {
	    temp_result1.two_words.top_word32 = POS_INFINITY_D_U;
	    temp_result1.two_words.bottom_word32 = POS_INFINITY_D_B;
	}

    }
    else if ((FP->fp2.b_single.sign == 1) && FP->trapped_opcode == FRCP) {
	temp_result1.two_words.top_word32 = NEG_INFINITY_S_U;
	temp_result1.two_words.bottom_word32 = NEG_INFINITY_S_B;
    } else {
	temp_result1.two_words.top_word32 = POS_INFINITY_S_U;
	temp_result1.two_words.bottom_word32 = POS_INFINITY_S_B;
    }

    place_se_result( FP );
}


/*****************************************************************
*
*  handle_overflow()
*
*  Compute the IEEE standard result in to temp_result1 (and
*  temp_result2 in the case of a dualop A unit result error)
*  Also compute the IEEE biased exponent results in to FPptr->fp1
*  and FPptr->fp2.
*
*  Called by
*
*  handle_exception
*
*  Calls
*
*  compute_biased
*  round_overflow
*
*  Global Variables
*
*  temp_result1,temp_result2 - are the results computed by
*                              this procedure.
*
************************************************************************/

void
handle_overflow( FP )
	    FPtrapcontrol *FP;
{
    word32 dest;
#if FPDEBUG
    dprintf("Entering handle_overflow\n");
#endif
    dest = (FP->fsrs[2] & RR_BITS) >> RR_SHIFT;
    if (FP->fsrs[2] & M_OVERFLOW_OCCURED_BIT) {
	/*
	 * the last stage contains the erroneous result in both scalar and
	 * pipelined operations
	 */
	temp_result1 = FP->mres[2];
	*IEEE_status_ptr |= M_OVERFLOW_BIT;
	if (!(*IEEE_status_ptr & OVERFLOW_MASK))
	    compute_biased(FP->mres[2], &FP->fp1, OVERFLOW, M_UNIT, 0);
	round_overflow(&temp_result1, M_UNIT, 0);
	if (dest != 0)
	    place_in_fpreg(dest, &temp_result1, M_UNIT, TRUE);
	else
	    FP->mres[2] = temp_result1;
	/* 
	 * clear M unit overflow error status
	 */
	FP->fsrs[2] &= ~M_OVERFLOW_OCCURED_BIT;


    }
    if (FP->fsrs[2] & A_OVERFLOW_OCCURED_BIT) {
	temp_result1 = FP->ares[2];
	*IEEE_status_ptr |= A_OVERFLOW_BIT;
	if (!(*IEEE_status_ptr & OVERFLOW_MASK))
	    /* compute and place IEEE standard biased result */
	    if (*IEEE_status_ptr & DUALOP_BIT)
		compute_biased(FP->ares[2], &FP->fp2, OVERFLOW, A_UNIT, 0);
	    else
		compute_biased(FP->ares[2], &FP->fp1, OVERFLOW, A_UNIT, 0);
	round_overflow(&temp_result1, A_UNIT, 0);
	if (dest != 0)
	    place_in_fpreg(dest, &temp_result1, A_UNIT, TRUE);
	else
	    FP->ares[2] = temp_result1;
	/* 
	 * clear A overflow result error status
	 */
	FP->fsrs[2] &= ~A_OVERFLOW_OCCURED_BIT;
    }
}


/*****************************************************************
*
*  handle_underflow()
*
*  Handle underflow exception.
*
*  Called by
*
*  handle_exception.
*
*  Calls
*
*  denormalize
*
*  Global Variables
*
*  temp_result1,temp_result2 - are the results computed by
*                              this procedure.
*  (@) *IEEE_status_ptr & DUALOP_BIT - set in fp_get_inputs
*
*******************************************************************/

void
handle_underflow( FP )
    FPtrapcontrol *FP;
{

    i860_type denormalize(), dummy;
    int inexactness_occured = 0;
    word32 dest;
#if FPDEBUG
    dprintf("Entering handle_underflow\n");
#endif
    dest = (FP->fsrs[2] & RR_BITS) >> RR_SHIFT;
    if (FP->fsrs[2] & M_UNDERFLOW_OCCURED_BIT) {
	*IEEE_status_ptr |= M_UNDERFLOW_BIT;
	temp_result1 = denormalize(M_UNIT, &inexactness_occured, 0, dummy,
/* rag in */
		 ((*IEEE_status_ptr & R_PRECISION_BIT) == R_PRECISION_BIT)
/* rag end */
	    );
	if (inexactness_occured || FP->fsrs[2] & M_INEXACT_BIT) {
/*DKG out
 *          *IEEE_status_ptr |= M_UNDERFLOW_BIT;
 *DKG end */
	    if (!(*IEEE_status_ptr & INEXACT_MASK))
		*IEEE_status_ptr |= M_INEXACT_BIT;
	}
	if (!(*IEEE_status_ptr & UNDERFLOW_MASK))
	    /* compute and place IEEE standard biased result */
	    compute_biased(FP->mres[2], &FP->fp1, UNDERFLOW, M_UNIT, 0);
	if (dest != 0)
	    place_in_fpreg(dest, &temp_result1, M_UNIT, TRUE);
	else
	    FP->mres[2] = temp_result1;
	/* 
	 * clear M unit underflow error status
	 */
	FP->fsrs[2] &= NO_M_UNIT_UNDERFLOW_OR_INEXACT;

    }
    if (FP->fsrs[2] & A_UNDERFLOW_OCCURED_BIT) {
	*IEEE_status_ptr |= A_UNDERFLOW_BIT;
	temp_result1 = denormalize(A_UNIT, &inexactness_occured, 0, dummy,
/* rag in */
		 ((*IEEE_status_ptr & R_PRECISION_BIT) == R_PRECISION_BIT)
/* rag end */
	    );
	if (inexactness_occured || FP->fsrs[2] & A_INEXACT_BIT) {
	    /*
	     * IEEE_status_ptr |= A_UNDERFLOW_BIT;
	     */
	    if (!(*IEEE_status_ptr & INEXACT_MASK))
		*IEEE_status_ptr |= A_INEXACT_BIT;
	}
	if (!(*IEEE_status_ptr & UNDERFLOW_MASK))
	    compute_biased(FP->ares[2], &FP->fp2, UNDERFLOW, A_UNIT, 0);
	if (dest != 0)
	    place_in_fpreg(dest, &temp_result1, A_UNIT, TRUE);
	else
	    FP->ares[2] = temp_result1;
	/* 
	 * clear A unit underflow error status
	 */
	FP->fsrs[2] &= NO_A_UNIT_UNDERFLOW_OR_INEXACT;
    }
}


/*****************************************************************
*
*  handle_inexact()
*
*  Handle inexact exception, when underflow did not occur.
*
*  Called by
*
*  handle_exception.
*
*******************************************************************/

void
handle_inexact( FP )
    FPtrapcontrol *FP;
{
#if FPDEBUG
    dprintf("Entering handle_inexact\n");
#endif
    if (FP->fsrs[2] & A_INEXACT_OCCURED_BIT) {
	*IEEE_status_ptr |= A_INEXACT_BIT;
	FP->fsrs[2] &= ~A_INEXACT_OCCURED_BIT;
    }
    if (FP->fsrs[2] & M_INEXACT_OCCURED_BIT) {
	*IEEE_status_ptr |= M_INEXACT_BIT;
	FP->fsrs[2] &= ~M_INEXACT_OCCURED_BIT;
    }
    if (!(*IEEE_status_ptr & INEXACT_MASK))
	FP->fsrs[2] &= ~(TI_BIT);
}



/*****************************************************************
*
*  round_overflow(num_ptr)
*
*  Compute the result according to current rounding mode.
*
*  Called by
*
*  handle_overflow
*
*  Input parameters
*
*  num_ptr - the i860 number to be rounded.
*  from_fp_trap - 0 called from handle_overflow
*				  1 called from frcp_func
*
*  Global Variables
*
*  None
*
********************************************************************/

void
round_overflow(num_ptr, which_unit, from_fp_trap)
    i860_type *num_ptr;
    int which_unit,
     from_fp_trap;
{
    register unsigned precision_bit,
     rounding_mode;
#if FPDEBUG
    dprintf("Entering round_overflow\n");
#endif
    if (which_unit == A_UNIT)
	precision_bit = A_UNIT_PRECISION_BIT;
    else
	precision_bit = M_UNIT_PRECISION_BIT;
    if (!from_fp_trap)
	rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS)
			 >> ROUNDING_MODE_SHIFT;
    else
	rounding_mode = (fsr & ROUNDING_MODE_BITS)
			 >> ROUNDING_MODE_SHIFT;

    switch (rounding_mode) {
    case TO_NEAREST:
#if FPDEBUG
	dprintf("TO_NEAREST\n");
#endif
	if ((!from_fp_trap && (FPptr->fsrs[2] & precision_bit)) ||
		(from_fp_trap && (fsr & precision_bit)))
	    if (num_ptr->b_double.sign == 0) {
		num_ptr->two_words.top_word32 = POS_INFINITY_D_U;
		num_ptr->two_words.bottom_word32 = POS_INFINITY_D_B;
	    } else {
		num_ptr->two_words.top_word32 = NEG_INFINITY_D_U;
		num_ptr->two_words.bottom_word32 = NEG_INFINITY_D_B;
	    }
	else if (num_ptr->b_single.sign == 0) {
	    num_ptr->two_words.top_word32 = POS_INFINITY_S_U;
	    num_ptr->two_words.bottom_word32 = POS_INFINITY_S_B;
	} else {
	    num_ptr->two_words.top_word32 = NEG_INFINITY_S_U;
	    num_ptr->two_words.bottom_word32 = NEG_INFINITY_S_B;
	}
    break;

    case TO_ZERO:
#if FPDEBUG
	dprintf("TO_ZERO\n");
#endif
	if ((!from_fp_trap && (FPptr->fsrs[2] & precision_bit)) ||
		(from_fp_trap && (fsr & precision_bit)))
	    if (num_ptr->b_double.sign == 0) {
		num_ptr->two_words.top_word32 = POS_BIGGEST_D_U;
		num_ptr->two_words.bottom_word32 = POS_BIGGEST_D_B;
	    } else {
		num_ptr->two_words.top_word32 = NEG_BIGGEST_D_U;
		num_ptr->two_words.bottom_word32 = NEG_BIGGEST_D_B;
	    }
	else if (num_ptr->b_single.sign == 0) {
	    num_ptr->two_words.top_word32 = POS_BIGGEST_S_U;
	    num_ptr->two_words.bottom_word32 = POS_BIGGEST_S_B;
	} else {
	    num_ptr->two_words.top_word32 = NEG_BIGGEST_S_U;
	    num_ptr->two_words.bottom_word32 = NEG_BIGGEST_S_B;
	}
    break;

    case DOWNWARD:
#if FPDEBUG
	dprintf("DOWNWARD\n");
#endif
	if ((!from_fp_trap && (FPptr->fsrs[2] & precision_bit)) ||
		(from_fp_trap && (fsr & precision_bit)))
	    if (num_ptr->b_double.sign == 0) {
		num_ptr->two_words.top_word32 = POS_BIGGEST_D_U;
		num_ptr->two_words.bottom_word32 = POS_BIGGEST_D_B;
	    } else {
		num_ptr->two_words.top_word32 = NEG_INFINITY_D_U;
		num_ptr->two_words.bottom_word32 = NEG_INFINITY_D_B;
	    }
	else if (num_ptr->b_single.sign == 0) {
	    num_ptr->two_words.top_word32 = POS_BIGGEST_S_U;
	    num_ptr->two_words.bottom_word32 = POS_BIGGEST_S_B;
	} else {
	    num_ptr->two_words.top_word32 = NEG_INFINITY_S_U;
	    num_ptr->two_words.bottom_word32 = NEG_INFINITY_S_B;
	}
    break;
    case UPWARD:
#if FPDEBUG
	dprintf("UPWARD\n");
#endif
	if ((!from_fp_trap && (FPptr->fsrs[2] & precision_bit)) ||
		(from_fp_trap && (fsr & precision_bit)))
	    if (num_ptr->b_double.sign == 0) {
		num_ptr->two_words.top_word32 = POS_INFINITY_D_U;
		num_ptr->two_words.bottom_word32 = POS_INFINITY_D_B;
	    } else {
		num_ptr->two_words.top_word32 = NEG_BIGGEST_D_U;
		num_ptr->two_words.bottom_word32 = NEG_BIGGEST_D_B;
	    }
	else if (num_ptr->b_single.sign == 0) {
	    num_ptr->two_words.top_word32 = POS_INFINITY_S_U;
	    num_ptr->two_words.bottom_word32 = POS_INFINITY_S_B;
	} else {
	    num_ptr->two_words.top_word32 = NEG_BIGGEST_S_U;
	    num_ptr->two_words.bottom_word32 = NEG_BIGGEST_S_B;
	}
    break;
    default:
    break;
    }
}


/*****************************************************************
*
*  fp_get_inputs()
*
*  Using the FIR field of the FPtrapcontrol structure, decode the
*  instruction that caused the trap, determining all relevant flags
*  and global variables.
*
*  Called by
*
*  determine_fault
*  handle_underflow
*  handle_overflow
*  handle_div_by_zero.
*
*  Global Variables
*
*  Bits 23-27 of IEEE_status_word have their values set by this procedure.
*  FPtrapcontrol.fp1-FPtrapcontrol.fp4
*  in regular scalar and pipeline operations fp1 and fp2 are set
*  to src1 and src2 respectively. in dualop operations fp1,fp2 apply
*  to the multiply and fp3-fp4 apply to the add sources
*
**********************************************************************/

void
fp_get_inputs( FP )
        FPtrapcontrol   *FP;
{
    register word32 info_bits = 0;
    union {
	word32 u_inst;
	struct {
	    word32 dpc:4;
	    word32 top_3_opcode:3;
	    word32 result_precision:1;
	    word32 source_precision:1;
	    word32 instruction_mode:1;
	    word32 pipeline_flag:1;
	    word32 src1:5;
	    word32 dest:5;
	    word32 src2:5;
	    word32 float_code:6;
	} b_inst;
    } instruction;
#if FPDEBUG
    dprintf("Entering fp_get_inputs\n");
#endif
    instruction.u_inst = *(word32 *) (FP->trapped_instruction);
#if FPDEBUG1
    dprintf("The instruction is: %lx\n", instruction.u_inst);
#endif


    /* initialize IEEE_status_word */
#if FPDEBUG1
    dprintf("the 1a IEEE mask is %lx\n", *IEEE_status_ptr);
#endif
    *IEEE_status_ptr &= ONLY_MASK_AND_STICKY;	/* except for mask and
						 * sticky bits all bits set
						 * to 0 */
#if FPDEBUG1
    dprintf("the 1 IEEE mask is %lx\n", *IEEE_status_ptr);
#endif
    if (instruction.b_inst.pipeline_flag)
	info_bits |= PIPELINE_BIT;
    if (instruction.b_inst.source_precision)
	info_bits |= S_PRECISION_BIT;
    if (instruction.b_inst.result_precision)
	info_bits |= R_PRECISION_BIT;
    *IEEE_status_ptr |= info_bits;

    FP->fp1.n_double = 0;
    FP->fp2.n_double = 0;
    FP->fp3.n_double = 0;
    FP->fp4.n_double = 0;
#if FPDEBUG1
    dprintf("the 1 IEEE mask is %lx\n", *IEEE_status_ptr);
#endif
    /* decode trapped opcode */
    if (instruction.b_inst.top_3_opcode == 0) {
	*IEEE_status_ptr |= DUALOP_BIT;
#if FPDEBUG1
	dprintf("setting dualop %lx\n", *IEEE_status_ptr);
#endif
	if (RESULT_ERROR_OCCURED)
	    return;
    } else if (RESULT_ERROR_OCCURED)
	return;
    if (instruction.b_inst.top_3_opcode == 1) {
	*IEEE_status_ptr |= DUALOP_BIT;
	if (RESULT_ERROR_OCCURED)
	    return;
    }
    if (FP->trapped_opcode != PFAM && FP->trapped_opcode != PFSM) {
	get_from_fpreg((word32) instruction.b_inst.src1, &(FP->fp1), FALSE);
#if FPDEBUG
	dprintf("fp1 set to "); print_i860type(&FP->fp1);
#endif
	get_from_fpreg((word32) instruction.b_inst.src2, &(FP->fp2), FALSE);
#if FPDEBUG
	dprintf("fp2 set to "); print_i860type(&FP->fp2);
#endif
    } else {
	word32 index = instruction.b_inst.dpc;
	inst_src1 = (word32) instruction.b_inst.src1;
	inst_src2 = (word32) instruction.b_inst.src2;
	FP->src1 = inst_src1;

	(dpc_table[index].M_op1) (*(dpc_table[index].param1), &(FP->fp1));
#if FPDEBUG
	dprintf("fp1 set to "); print_i860type(&FP->fp1);
#endif
	(dpc_table[index].M_op2) (*(dpc_table[index].param2), &(FP->fp2));
#if FPDEBUG
	dprintf("fp2 set to "); print_i860type(&FP->fp2);
#endif
	(dpc_table[index].A_op1) (*(dpc_table[index].param3), &(FP->fp3), TRUE);
#if FPDEBUG
	dprintf("fp3 set to "); print_i860type(&FP->fp3);
#endif
	(dpc_table[index].A_op2) (*(dpc_table[index].param4), &(FP->fp4), TRUE);
#if FPDEBUG
	dprintf("fp4 set to "); print_i860type(&FP->fp4);
#endif
	info_bits = 0;
	if (dpc_table[index].T_load)
	    info_bits |= T_LOAD_BIT;
	if (dpc_table[index].Kr_load)
	    info_bits |= KR_LOAD_BIT;
	if (dpc_table[index].Ki_load)
	    info_bits |= KI_LOAD_BIT;
	*IEEE_status_ptr |= info_bits;
#if FPDEBUG
	dprintf("fp_get_input return IEEE_status %lx\n", *IEEE_status_ptr);
#endif

    }


    FP->rdest = instruction.b_inst.dest;
}


print_i860type(i860_ptr)
    i860_type *i860_ptr;
{
#if	FPDEBUG
	if ( fpe_debug )
		printf(" %lx %lx\n", i860_ptr->two_words.top_word32,
			i860_ptr->two_words.bottom_word32);
#endif
}

#if	FPDEBUG
/*
 * print an i860_type var with possible suffix string
 */
printFP(i860_ptr,s)
    i860_type *i860_ptr;
    char	*s;
{
        if ( fpe_debug ) {
                printf("%lx %lx %s", i860_ptr->two_words.top_word32,
                        i860_ptr->two_words.bottom_word32,
			( s ? s : "" ) );
	}
}

#endif


/*****************************************************************
*
*  place_se_result()
*
*  This procedure is called, after a source error has been handled,
*  (ie when the trapped instruction will not be reexecuted).
*  It places the computed result(s) in to the proper place(s),
*  and in the case of pipelined operations updates the memory image
*  of the pipe(s) appropriately.
*
*  Called by
*
*  handle_unusual_src
*  handle_invalid_operation
*  handle_div_by_zero.
*
*  Global Variables
*
*  (@) temp_result1, (@) temp_result2 - are the results to be placed
*
******************************************************************/

void
place_se_result( FP )
       FPtrapcontrol   *FP;
{
#if FPDEBUG
    dprintf("Entering place_se_result IEEE_status %lx\n",*IEEE_status_ptr);
    dprintf("  temp_result1 "); printFP(&temp_result1," temp_result2 ");
    printFP(&temp_result2,"\n");
#endif
#if FPDEBUG1
    print_FP( FP );
#endif
    if (*IEEE_status_ptr & DUALOP_BIT) {
	register int i;
#ifdef	FPDEBUG
	dprintf("  DUALOP_BIT true\n");
#endif
	/* carry out loads */
	if (*IEEE_status_ptr & T_LOAD_BIT) {
	    FP->t = FP->mres[2];
	    if (!(*IEEE_status_ptr & S_PRECISION_BIT)) {
		FP->t.two_words.top_word32 =
		    FP->t.two_words.bottom_word32 & 0x80000000 |
		    (FP->t.two_words.bottom_word32 & 0x7fffffff) >> 3;
		FP->kr.two_words.bottom_word32 =
		    FP->t.two_words.bottom_word32 << 29 & 0xe0000000;
	    }
#if FPDEBUG
	    dprintf("T  reg "); print_i860type(&(FP->t));
#endif
	}
	if (*IEEE_status_ptr & KR_LOAD_BIT) {
	    get_from_fpreg((word32) FP->src1, &(FP->kr), FALSE);
	    if (!(*IEEE_status_ptr & S_PRECISION_BIT)) {
		FP->kr.two_words.top_word32 =
		    FP->kr.two_words.bottom_word32 & 0x80000000 |
		    (FP->kr.two_words.bottom_word32 & 0x7fffffff) >> 3;
		FP->kr.two_words.bottom_word32 =
		    FP->kr.two_words.bottom_word32 << 29 & 0xe0000000;
	    }
#if FPDEBUG
	    dprintf("KR reg "); print_i860type(&(FP->kr));
#endif
	} else if (*IEEE_status_ptr & KI_LOAD_BIT) {
	    get_from_fpreg((word32) FP->src1, &(FP->ki), FALSE);
	    if (!(*IEEE_status_ptr & S_PRECISION_BIT)) {
		FP->ki.two_words.top_word32 =
		    FP->ki.two_words.bottom_word32 & 0x80000000 |
		    (FP->ki.two_words.bottom_word32 & 0x7fffffff) >> 3;
		FP->ki.two_words.bottom_word32 =
		    FP->ki.two_words.bottom_word32 << 29 & 0xe0000000;
	    }
#if FPDEBUG
	    dprintf("KI reg "); print_i860type(&(FP->ki));
#endif
	}
	/* added Feb 89 to handle the new dualops */
	if (*IEEE_status_ptr & PIPELINE_BIT) {	/* this means that the
						 * result is from the A
						 * unit */
	    place_in_fpreg(FP->rdest, &FP->ares[2], A_UNIT, TRUE);
	} else
	    place_in_fpreg(FP->rdest, &FP->mres[2], M_UNIT, TRUE);

	/* update M pipe */
	if (*IEEE_status_ptr & S_PRECISION_BIT) {	/* single double or
							 * double double,
							 * handled the same */
	    FP->fsrs[2] = (FP->fsrs[2] & EXCEPT_M_INFO) |
		(FP->fsrs[0] & ONLY_M_INFO);
	    FP->fsrs[1] = (FP->fsrs[1] & EXCEPT_M_INFO) |
		M_UNIT_PRECISION_BIT;
	    FP->mres[1].n_double = 0;
	    FP->mres[2] = FP->mres[0];
	} else {
	    /* rag ??? why commented out ???
	     * if(FP->fsrs[0] & M_UNIT_PRECISION_BIT)  double to single
	     * { FP->fsrs[1] = (FP->fsrs[1] & EXCEPT_M_INFO) |
	     * (FP->fsrs[0] & ONLY_M_INFO); FP->mres[1] =
	     * FP->mres[0]; FP->mres[2].n_double = 0; FP->fsrs[2]
	     * = (FP->fsrs[2] & EXCEPT_M_INFO) | M_UNIT_PRECISION_BIT; }
	     * else
	     */

	    {			/* single single or double single */
		for (i = 2; i > 0; i--) {
		    FP->mres[i] = FP->mres[i - 1];
		    FP->fsrs[i] = (FP->fsrs[i] & EXCEPT_M_INFO) |
			(FP->fsrs[i - 1] & ONLY_M_INFO);
		}
	    }
	}
	FP->mres[0] = temp_result1;

	/* update the memory image of the A */
	for (i = 2; i > 0; i--) {
	    FP->ares[i] = FP->ares[i - 1];
	    FP->fsrs[i] = (FP->fsrs[i] & EXCEPT_A_INFO) |
		(FP->fsrs[i - 1] & ONLY_A_INFO);
	}
	FP->ares[0] = temp_result2;

	/* clear result status in fsrs[0] for both units */
	FP->fsrs[0] &= FSR_CLEAR;

	/* update result status in fsrs[0] in accordance with fsr */
	FP->fsrs[0] |= fsr & RESULT_INFO_BITS;
	if (*IEEE_status_ptr & R_PRECISION_BIT) {
	    FP->fsrs[0] |= M_UNIT_PRECISION_BIT;
	    FP->fsrs[0] |= A_UNIT_PRECISION_BIT;
	}
    } else if (*IEEE_status_ptr & PIPELINE_BIT) {
#ifdef	FPDEBUG
	dprintf("  PIPELINE_BIT true\n");
#endif
	if (ADDER_OP(FP->trapped_opcode)) {
	    int i;
	    word32 save_fsr2_mrp;
	    save_fsr2_mrp = FP->fsrs[2] & M_UNIT_PRECISION_BIT;
	    place_in_fpreg(FP->rdest, &FP->ares[2], A_UNIT, TRUE);
	    for (i = 2; i > 0; i--) {
		FP->ares[i] = FP->ares[i - 1];
		FP->fsrs[i] = (FP->fsrs[i] & EXCEPT_A_INFO) |
		    (FP->fsrs[i - 1] & ONLY_A_INFO);
	    }
	    if (save_fsr2_mrp)
		FP->fsrs[2] |= M_UNIT_PRECISION_BIT;
	    else
		FP->fsrs[2] &= ~M_UNIT_PRECISION_BIT;
	    FP->ares[0] = temp_result1;
	    /* clear result status in fsrs[0] for A unit */
	    FP->fsrs[0] &= FSR_CLEAR_A_UNIT;
	    if (*IEEE_status_ptr & R_PRECISION_BIT)
		FP->fsrs[0] |= A_UNIT_PRECISION_BIT;
	} else {
	    register int i;
	    place_in_fpreg(FP->rdest, &FP->mres[2], M_UNIT, TRUE);

	    if (*IEEE_status_ptr & S_PRECISION_BIT) {
	    /* single double or double double, handled the same */
		FP->fsrs[2] = (FP->fsrs[2] & EXCEPT_M_INFO) |
		    (FP->fsrs[0] & ONLY_M_INFO);
		FP->fsrs[1] = (FP->fsrs[1] & EXCEPT_M_INFO) |
		    M_UNIT_PRECISION_BIT;
		FP->mres[1].n_double = 0;
		FP->mres[2] = FP->mres[0];
	    } else {
		/* rag ??? why commented out ???
		 * if(FP->fsrs[0] & M_UNIT_PRECISION_BIT)  double to
		 * single { FP->fsrs[1] = (FP->fsrs[1] &
		 * EXCEPT_M_INFO) | (FP->fsrs[0] & ONLY_M_INFO);
		 * FP->mres[1] = FP->mres[0]; FP->mres[2].n_double
		 * = 0; FP->fsrs[2] = (FP->fsrs[2] & EXCEPT_M_INFO) |
		 * M_UNIT_PRECISION_BIT; } else
		 */
		{		/* single single or double single */
		    for (i = 2; i > 0; i--) {
			FP->mres[i] = FP->mres[i - 1];
			FP->fsrs[i] = (FP->fsrs[i] & EXCEPT_M_INFO) |
			    (FP->fsrs[i - 1] & ONLY_M_INFO);
		    }
		}
	    }
	    FP->mres[0] = temp_result1;
	    /* clear result status in fsrs[0] for M unit */
	    FP->fsrs[0] &= FSR_CLEAR_M_UNIT;
	    if (*IEEE_status_ptr & R_PRECISION_BIT)
		FP->fsrs[0] |= M_UNIT_PRECISION_BIT;
	}


	/* update result status in fsrs[0] in accordance with fsr */
	FP->fsrs[0] |= (fsr & RESULT_INFO_BITS);
    } else {			/* scalar operation */
#ifdef	FPDEBUG
	dprintf("  scalar operation: temp_result1 ");printFP(&temp_result1,"\n");
#endif
	place_in_fpreg(FP->rdest, &temp_result1, NOT_USED, FALSE);
	/* clear result status in fsrs[2] for affected unit */
	if (ADDER_OP(FP->trapped_opcode)) {
	    FP->fsrs[2] &= FSR_CLEAR_A_UNIT;
	    if (*IEEE_status_ptr & R_PRECISION_BIT)
		FP->fsrs[2] |= A_UNIT_PRECISION_BIT;
	} else {
	    FP->fsrs[2] &= FSR_CLEAR_M_UNIT;
	    if (*IEEE_status_ptr & R_PRECISION_BIT)
		FP->fsrs[2] |= M_UNIT_PRECISION_BIT;
	}
	/* update result status in fsrs[2] in accordance with fsr */
	FP->fsrs[2] |= (fsr & RESULT_INFO_BITS);
    }

    /* turn on KNF bit in IEEE status word and in psr */
    *IEEE_status_ptr |= KNF_BIT;
    FP->psr |= PSR_KNF_BIT;
#if FPDEBUG
    dprintf("Leaving place_se_result KNF! IEEE_status %lx\n",*IEEE_status_ptr);
#endif
}


#ifdef	_NOT_USED

/*****************************************************************
*
*  fp_set_re_result()
*
*  This procedure is called when the trapped instruction will be
*  reexecuted (ie result error). It places the computed result(s)
*  in to the proper place(s), and in the case of pipelined
*  operations updates the memory image of the pipe(s) appropriately.
*
*  Called by
*
*  handle_exception
*
*
*******************************************************************/
void
fp_set_re_result(result1, result2)
    i860_type result1, result2;
{

    register word32 dest;
    register int which_unit;
#if FPDEBUG
    dprintf("Entering fp_set_re_result\n");
#endif
    dest = (FPptr->fsrs[2] & RR_BITS) >> RR_SHIFT;
    if (dest != 0) {
	if (M_UNIT_RESULT_ERROR) {
	    which_unit = M_UNIT;
	    FPptr->fsrs[2] &= FSR_CLEAR_M_UNIT;
	} else {
	    which_unit = A_UNIT;
	    FPptr->fsrs[2] &= FSR_CLEAR_A_UNIT;
	}
	place_in_fpreg(dest, &result1, which_unit, TRUE);
    } else {
	if (M_UNIT_RESULT_ERROR) {
	    FPptr->mres[2] = result1;
	    FPptr->fsrs[2] &= FSR_CLEAR_M_UNIT;
	}
	if (A_UNIT_RESULT_ERROR) {
	    if (*IEEE_status_ptr & DUALOP_BIT)
		FPptr->ares[2] = result2;
	    else
		FPptr->ares[2] = result1;
	    FPptr->fsrs[2] &= FSR_CLEAR_A_UNIT;
	}
    }

}
#endif	/* _NOT_USED /*


/*****************************************************************
*
*  denormalize(int,*int)
*
*  This procedure is called from handle_underflow to compute and
*  return a denormal from its input parameter.
*
*  Called by
*
*  handle_underflow
*
* UPDATE:
* This Procedure is also called by the reexecute functions, for denormal
* numbers, when an underflow occurs. If so the parameter from_fp_trap
* is set to one, otherwise it is 0.
*******************************************************************/
i860_type 
denormalize(which_unit, inexactness_occured, from_fptrap,
	    fpt_result, reexecute_prec)
    int which_unit, *inexactness_occured, from_fptrap,
     reexecute_prec;
    i860_type fpt_result;
{
    typedef struct {
	unsigned mantissa:23;
	unsigned exponent:8;
	unsigned sign:1;
	word32 dummy;
    } single_pattern_a;

    union {
	i860_double n_double;
	double_pattern b_double;
	i860_single n_single;
	single_pattern b_single;
	single_pattern_a b_single_a;
	word64 two_words;
    } result;

    register int num_to_shift, result_precision, rounding_mode;
    int inxctnss, post_rounding, sign, i;
    word32 added_one;
    i860_type *i860_ptr, num;
#if FPDEBUG
    dprintf("Entering denormalize result_precision %lx fsr %lx\n",
	  reexecute_prec,fsr);
#endif
    rounding_mode = (FPptr->fsrs[2] & ROUNDING_MODE_BITS)
			>> ROUNDING_MODE_SHIFT;


    if (which_unit == M_UNIT) {
	if (from_fptrap) {
#if FPDEBUG
	    dprintf("IN M_UNIT\n");
#endif
	    i860_ptr = &fpt_result;
	    result_precision = reexecute_prec;
	    added_one = fsr & MA_BIT;
	} else {
	    i860_ptr = &FPptr->mres[2];
	    result_precision = FPptr->fsrs[2] & M_UNIT_PRECISION_BIT;
	    added_one = FPptr->fsrs[2] & MA_BIT;
	}
    } else {
	if (from_fptrap) {
	    i860_ptr = &fpt_result;
	    result_precision = reexecute_prec;
	    added_one = fsr & AA_BIT;
	} else {
	    i860_ptr = &FPptr->ares[2];
	    result_precision = FPptr->fsrs[2] & A_UNIT_PRECISION_BIT;
	    added_one = FPptr->fsrs[2] & AA_BIT;
	}
    }
    result.two_words.top_word32 = i860_ptr->two_words.top_word32;
    result.two_words.bottom_word32 = i860_ptr->two_words.bottom_word32;
#if FPDEBUG
    dprintf("The source is "); print_i860type(&result);
#endif
    if (result_precision) {	/* is double */
#if FPDEBUG
	dprintf("DOUBLE\n");
#endif
	if (i860_ptr->b_double.exponent == 0)
	    num_to_shift = 1;
	else
	    /*
	     * the number of bits to right shift the mantissa is 801h -
	     * (the hex number which is the 11 bit exponent field)
	     */
	    num_to_shift = DOUBLE_FIXER - i860_ptr->b_double.exponent;
#if FPDEBUG
	dprintf("the number to shift is %lx\n", num_to_shift);
#endif
	if ((rounding_mode == TO_NEAREST) && added_one) { /* undo rounding */
#if FPDEBUG
	    dprintf("undoing rounding\n");
#endif
	    result.b_double.mantissa_c -= 0x1;
	    if (result.b_double.mantissa_c == (unsigned long) 0xffffffff) {
		result.b_double.mantissa_b -= 0x1;
		if (result.b_double.mantissa_b == 0x7ffff) {
		    result.b_double.mantissa_a -= 0x1;
		    if (result.b_double.mantissa_a == 0x1)
			num_to_shift++;
		}
	    }
#if FPDEBUG
	    dprintf("num is now "); print_i860type(&result);
#endif
	    if (!from_fptrap)
		if (which_unit == A_UNIT)
		    FPptr->fsrs[2] &= ~AA_BIT;
		else
		    FPptr->fsrs[2] &= ~MA_BIT;
	}
	/*
	 * determine if inexactness will occur and if denormal will have to
	 * be rounded
	 */
	determine_inxct_rnd(&inxctnss, &post_rounding, DOUBLE, num_to_shift,
			    result, rounding_mode, FALSE, 0);
	*inexactness_occured = inxctnss;


	sign = result.b_double.sign;
	if (num_to_shift > MANTISSA_SIZE_D)
	    result.n_double = 0;
	else {
/* changed this code for FZ_BIT set -- rag */
	    if( ( ( from_fptrap) && ((           fsr & FZ_BIT) == 0) )
	     || ( (!from_fptrap) && ((FPptr->fsrs[2] & FZ_BIT) == 0) )
	     ){
	        result.b_double.sign = 0;
	        result.b_double.exponent = 1;  /* set implicit bit */
	        for (i = 0; i < num_to_shift; i++)
		    do_a_r_shift_d(&result);
	    } else  { /* flush denormal to zero */
	        result.two_words.bottom_word32 = 0;
	    }
	}
	result.b_double.sign = sign;
#if FPDEBUG
	dprintf("num is now after shift"); print_i860type(&result);
#endif
	/* carry out post rounding if so indicated */
	if (post_rounding) {	/* indicates that 1 should be added to
				 * mantissa */
	    if (!from_fptrap) {
		if (which_unit == A_UNIT)
		    FPptr->fsrs[2] |= AA_BIT;
		else
		    FPptr->fsrs[2] |= MA_BIT;
	    }
	    result.b_double.mantissa_c += 0x1;
	    if (result.b_double.mantissa_c == 0x00000000) {
		result.b_double.mantissa_b += 0x1;
		if (result.b_double.mantissa_b == 0x00000) {
		    result.b_double.mantissa_a += 0x1;
		    if (result.b_double.mantissa_a == 0x0)
			result.b_double.exponent = 0x01;
		}
	    }
#if FPDEBUG
	    dprintf("num is now after post rounding"); print_i860type(&result);
#endif
	}
    } else {			/* result precision is SINGLE */
#if FPDEBUG
	dprintf("SINGLE\n");
#endif
	if (which_unit == A_UNIT) {
	    /*
	     * if the exponent is 0 and the ae bits are not 0 this means
	     * that the number in the exponent is actually the
	     * representation of -256
	     * 
	     * if ae bits are not 0x7 this means that the number in the
	     * exponent is actually the representation of a number < -256
	     * 
	     * in either of the above two cases the denormal will lose all of
	     * its bits, we signal this by setting num_to_shift to be
	     * 1+MANTISSA_SIZE_S
	     */
	    unsigned tmp_exp;
	    if (from_fptrap)
		tmp_exp = (fsr & AE_BITS) >> AE_SHIFT;
	    else
		tmp_exp = (FPptr->fsrs[2] & AE_BITS) >> AE_SHIFT;
#if FPDEBUG
	    dprintf("tmp_exp %lx fsr is %lx\n", tmp_exp,fsr);
#endif
#if FPDEBUG
	    dprintf("i860ptr->exp %lx\n", i860_ptr->b_single.exponent);
#endif
	    if (i860_ptr->b_single.exponent == 0)
		if (tmp_exp)
		    num_to_shift = 1 + MANTISSA_SIZE_S;
		else		/* 0 represents actually -127 so shift is 1 */
		    num_to_shift = 1;
	    else if (tmp_exp != 0x700)
		num_to_shift = 1 + MANTISSA_SIZE_S;
	    else {
		num_to_shift = SINGLE_FIXER - i860_ptr->b_single.exponent;
	    }
	} else {
	    if (i860_ptr->b_single.exponent == 0)
		num_to_shift = 1;
	    else
		num_to_shift = SINGLE_FIXER - i860_ptr->b_single.exponent;
	}
#if FPDEBUG
	dprintf("the number to shift is %lx\n", num_to_shift);
#endif
	if ((rounding_mode == TO_NEAREST) && added_one) { /* undo rounding */
#if FPDEBUG
	    dprintf("undoing rounding         \n");
#endif
	    result.b_single_a.mantissa -= 0x1;
	    if (result.b_single_a.mantissa == 0x7fffff)
		num_to_shift++;
#if FPDEBUG
	    dprintf("num is now "); print_i860type(&result);
#endif
	    if (!from_fptrap)
		if (which_unit == A_UNIT)
		    FPptr->fsrs[2] &= ~AA_BIT;
		else
		    FPptr->fsrs[2] &= ~MA_BIT;
	}
	/*
	 * determine if inexactness will occur and if denormal will have to
	 * be rounded
	 */
	determine_inxct_rnd(&inxctnss, &post_rounding, SINGLE, num_to_shift,
			    result, rounding_mode, FALSE, 0);
	*inexactness_occured = inxctnss;

	sign = result.b_single.sign;
	if (num_to_shift > MANTISSA_SIZE_S)
	    result.n_single = 0;
	else {
/* changed this code for FZ_BIT set -- rag */
	    if( ( ( from_fptrap) && ((           fsr & FZ_BIT) == 0) )
	     || ( (!from_fptrap) && ((FPptr->fsrs[2] & FZ_BIT) == 0) )
	     ){
	        result.b_single.sign = 0;
	        result.b_single.exponent = 1;  /* set implicit bit */
	        result.two_words.bottom_word32 =
		    result.two_words.bottom_word32 >> num_to_shift;
	        result.b_single.exponent = 0;
	    } else  { /* flush denormal to zero */
	        result.two_words.bottom_word32 = 0;
	    }
	}
	result.b_single.sign = sign;

#if FPDEBUG
	dprintf("num is now after shift"); print_i860type(&result);
#endif

	/* carry out post rounding if so indicated */
	if (post_rounding) {	/* indicates that 1 should be added to
				 * mantissa */
#if FPDEBUG
	    dprintf("this time post rounding occured in underflow\n");
#endif
	    if (!from_fptrap)
		if (which_unit == A_UNIT)
		    FPptr->fsrs[2] |= AA_BIT;
		else
		    FPptr->fsrs[2] |= MA_BIT;
	    result.b_single_a.mantissa += 0x01;
	    if (result.b_single_a.mantissa == 0x0)
		result.b_single.exponent = 0x01;
	}
    }
    num.two_words.top_word32 = result.two_words.top_word32;
    num.two_words.bottom_word32 = result.two_words.bottom_word32;
/* if called through  reexecute, and rounding mode is TO_NEAREST,
 * it might have further effect, when scaling back the result of the reexecute,
 * on the final result. Hence, one has to remember any lost bits
 */
    if (from_fptrap)
	if (rounding_mode == TO_NEAREST)
	    *inexactness_occured = inxctnss - post_rounding;
    /* if set, at least one bit is lost */
	else
	    *inexactness_occured = 0;

    return (num);
}


/*****************************************************************
*
*  compute_biased(i860_num,fp1_or_fp2,over_or_under,from_fp_trap)
*
*  This procedure is called to compute the IEEE standard biased
*  result and place it in the specified location in the FPtrapcontrol
*  structure (ie either fp1 or fp2). SINGLE_SHIFT is 192 see [1],
*  DOUBLE_SHIFT is 1536 see [1].
*
*  Called By
*
*  handle_overflow
*  handle_underflow
*
*  Calls
*
*  None
*
*  Global Variables
*
*  None
*
*	Update: This procedure can be called from functions which are called
* 		through reexecute_instruction, e.g. rcp_func.
*		This is indicated by the parameter- from_fp_trap, which is
*		set when cause by an overflow/underflow which occured in
* 		the FPEH.
*********************************************************************/

void
compute_biased(i860_num, fp1_or_fp2, over_or_under, which_unit, from_fp_trap)
    i860_type *fp1_or_fp2,
     i860_num;
    int over_or_under,
     which_unit,
     from_fp_trap;
{
    unsigned precision_bit;
#if FPDEBUG
    dprintf("Entering compute_biased\n");
#endif
    if (which_unit == A_UNIT)
	precision_bit = A_UNIT_PRECISION_BIT;
    else
	precision_bit = M_UNIT_PRECISION_BIT;
#if FPDEBUG1
    dprintf("precision bit is compute_b is %lx ", precision_bit);
#endif
    if (((!from_fp_trap) && (FPptr->fsrs[2] & precision_bit)) ||
	    (from_fp_trap && (fsr & precision_bit))) {

#if FPDEBUG1
	dprintf("In double\n");
#endif
	if (over_or_under == OVERFLOW) {
	    if (i860_num.b_double.exponent == 0x7ff)
		i860_num.b_double.exponent = (unsigned int) (0x7ff - DOUBLE_SHIFT - DOUBLE_BIAS);
	    else
		i860_num.b_double.exponent = (DOUBLE_FIX + i860_num.b_double.exponent) -
		    DOUBLE_SHIFT - DOUBLE_BIAS;
	} else {
	    if (i860_num.b_double.exponent == 0)
		i860_num.b_double.exponent = (unsigned int) (DOUBLE_SHIFT + DOUBLE_BIAS);
	    else
		i860_num.b_double.exponent = (i860_num.b_double.exponent - DOUBLE_FIX)
		    + DOUBLE_SHIFT + DOUBLE_BIAS;
	}
    } else {			/* single precision */
	register unsigned tmp_exp;
	register int exp;
	i860_type tmp;
	tmp.n_double = 0;
#if FPDEBUG1
	dprintf("In single\n");
#endif
	if (which_unit == A_UNIT) {
	    if (!from_fp_trap)
		tmp_exp = (FPptr->fsrs[2] & AE_BITS) >> AE_SHIFT;
	    else
		tmp_exp = (fsr & AE_SHIFT) >> AE_SHIFT;
	    tmp_exp |= ((i860_num.two_words.bottom_word32 & SINGLE_EXP_BITS)
			>> MANTISSA_SIZE_S);
	} else
	    tmp_exp = ((i860_num.two_words.bottom_word32 & SINGLE_EXP_BITS)
			>> MANTISSA_SIZE_S);
	/* convert to a double */
	tmp.b_double.mantissa_a = i860_num.b_single.mantissa_a;
	tmp.b_double.mantissa_b = i860_num.b_single.mantissa_b >> 3;
	tmp.b_double.mantissa_c = (i860_num.b_single.mantissa_b & 0x3) << 29;
	if (over_or_under == OVERFLOW) {
	    if (tmp_exp == 0xff)
		exp = 255;
	    else {
		if (tmp_exp < 0xff)	/* ae==0 or was m unit op */
		    exp = tmp_exp + 0x100;
		else
		    exp = (int) tmp_exp;
	    }

	    tmp.b_double.exponent = (exp - SINGLE_BIAS) - DOUBLE_BIAS - DOUBLE_SHIFT;
	} else {		/* underflow */
	    if (tmp_exp == 0)
		exp = 0;
	    else {
		tmp_exp |= 0xfffff000;
		exp = (int) (tmp_exp);
	    }

	    tmp.b_double.exponent = (exp - SINGLE_BIAS) + DOUBLE_BIAS +
		DOUBLE_SHIFT;
	}
	i860_num.two_words.bottom_word32 = tmp.two_words.bottom_word32;
	i860_num.two_words.top_word32 = tmp.two_words.top_word32;
    }

    *fp1_or_fp2 = i860_num;
}


/*****************************************************************
*
*  fp_set_mask(mask_num)
*
*  Takes bit pattern and places it in to the mask bit portion of
*  the IEEE status word. The bit pattern must be between 0h
*  (all unmasked) and 1fh (all masked). The masks turned on correspond
*  to the condition signalled by the bit # in the status word.
*  (ie if the argument is 2h then mask bit #16 will be set, if 3h bits
*  #15 and #16 will be set). The procedure returns the value of the 5
*  bits prior to the call, or ERROR_VALUE if invalid mask bit specified.
*
******************************************************************/


#define ERROR_VALUE 0x20

unsigned 
fp_set_mask(mask_num)
    unsigned mask_num;
{
    word32 current_masks;
    /* if mask_num not in permissible range return ERROR_VALUE */
    if (mask_num > 0x1f)
	return (ERROR_VALUE);

    /* if mask contains inexact mask turn on TI bit in fsr */
    if (mask_num & 0x10)
	FPptr->fsrs[2] |= TI_BIT;
    /*
     * set return value to current value of current mask bits in
     * IEEE_status_word field
     */
    current_masks = (*IEEE_status_ptr & IEEE_MASK_MASK) >> IEEE_MASK_SHIFT;

    /* set mask bits of IEEE_status_word to mask_num */
    *IEEE_status_ptr &= ~(IEEE_MASK_MASK);	/* turn off all masks */
    *IEEE_status_ptr |= (mask_num << IEEE_MASK_SHIFT);

    return (current_masks);
}


i860_type 
reexecute_instruction(src1, src2, A_part_of_dualop)
    i860_type src1,
     src2;
    int A_part_of_dualop;
{
    int user_fsr,
     temp_fsr;
    i860_type result;
    register int s_prec,
     r_prec,
     index;
/* rag in */
    int under_flow_occurred = FALSE,
     inxact_alu_op = 0;
/* rag end */
#if FPDEBUG
    dprintf("Entering reexecute_instruct opcode %lx\n",FPptr->trapped_opcode);
#if FPDEBUG1
    print_i860type(&src1);
    print_i860type(&src2);
#endif
#endif
    result.n_double = 0;
    if (FPptr->trapped_opcode == FTRUNC)
	return (result);	/* which is zero */

/***DKG**** 
 * Set the rounding mode to the users rounding mode when the trap ocurred
 */
#if FPDEBUG
    dprintf("setting the rounding mode according to user\n");
#endif
    temp_fsr = get_fsr();
    user_fsr = FPptr->fsrs[2] & 0x0000000C;	/* get the user's fsr */
    temp_fsr &= 0xfffffff3;	/* reset the rounding mode       */
    temp_fsr |= user_fsr;	/* set the current rounding mode */
    set_fsr(temp_fsr);		/* to the users rounding mode   */

    r_prec = ((*IEEE_status_ptr & R_PRECISION_BIT) == R_PRECISION_BIT);
#if FPDEBUG
    dprintf("R_PREC %lx IEEE_status %lx \n", r_prec, *IEEE_status_ptr);
#endif
    if (A_part_of_dualop) {
#if FPDEBUG
	dprintf("in reexecute , with A_part_of_dual\n");
#endif
	s_prec = ((*IEEE_status_ptr & A_PRECISION_BIT) == A_PRECISION_BIT);
	if (!s_prec && r_prec) {/* a .sd operation merely convert both to
				 * double and do operation */
	    s_prec = DOUBLE;
	    to_double_precision(&src1);
	    to_double_precision(&src2);
	    if (FPptr->trapped_opcode == PFAM)
		result = do_fadd(src1, src2, s_prec, r_prec,
/* rag in */
				 &under_flow_occurred, &inxact_alu_op
/* rag end */
		    );
	    else
		result = do_fsub(src1, src2, s_prec, r_prec,
/* rag in */
				 &under_flow_occurred, &inxact_alu_op
/* rag end */
		    );
	} else {
	    if (FPptr->trapped_opcode == PFAM)
		result = a_unit_func(src1, src2, s_prec, r_prec, ADDITION);
	    else
		result = a_unit_func(src1, src2, s_prec, r_prec, SUBTRACT);
	}
    } else {
	index = ((int) (FPptr->trapped_opcode)) - 1;
#if FPDEBUG
	dprintf(" in reexecute , index = %lx \n",index);
#endif
	s_prec = ((*IEEE_status_ptr & S_PRECISION_BIT) == S_PRECISION_BIT);
	if (!s_prec && r_prec && (index < FSUB || index > PFEQ) &&
		(index != 0xc)) {
	    /*
	     * a .sd operation merely convert both to double and do
	     * operation (except for FIX PFGT PFEQ)
	     */
	    s_prec = DOUBLE;
	    to_double_precision(&src1);
	    to_double_precision(&src2);
#if FPDEBUG
	dprintf("  (func_table1[%d])(%x %x %x %x %x %x)\n",index,
		src1, src2, s_prec, r_prec, &under_flow_occurred,
		&inxact_alu_op);
#endif
	    result = (func_table1[index]) (src1, src2, s_prec, r_prec,
/* rag in */
				      &under_flow_occurred, &inxact_alu_op
/* rag end */
		);
	} else {
#if FPDEBUG
	dprintf("  (func_table[%d])(%x %x %x %x)\n",index,
		src1, src2, s_prec, r_prec);
#endif
	    result = (func_table[index]) (src1, src2, s_prec, r_prec);
	}
    }
/* retrieve the floating point trap handler rounding mode */
    temp_fsr &= 0xfffffff3;
    temp_fsr |= 0x00000004;	/* set the rounding mode back to RM = 01 */
/* rag DO NOT DO? set_fsr(temp_fsr); */
#if FPDEBUG
    dprintf("back from calling func table\n");
#endif
    return (result);
}

#if FPDEBUG

#if 1

print_FP( FP )
       FPtrapcontrol   *FP;
{
	char			buf[128],num[24];
	register char		*cp;
	register unsigned	a,i;
	extern void		itoa();

	if ( fpe_debug == 0 )
		return;

	a = FP->fsrs[2];	/* fp status register */

	a &= ~(FSR_rsrvd3+FSR_rsrvd2+FSR_rsrvd1+FSR_rsrvd0);
	cp = buf;
	*cp = '\0';

	if ( a & FSR_ARP )
		strcat(cp,"ARP ");

	if ( a & FSR_MRP )
		strcat(cp,"MRP ");

	if ( a & FSR_IRP )
		strcat(cp,"IRP ");

	if ( a & FSR_LRP )
		strcat(cp,"LRP ");

	if ( a & FSR_AE ) {
		i = (a & FSR_AE)>>22;
		itoa(i,num);
		strcat(cp,"AE(0x");
		strcat(cp,num);
		strcat(cp,") ");
	}

	if ( a & FSR_RR ) {
		i = (a & FSR_RR)>>17;
		strcat(cp,"RR(0x");
		itoa(i,num);
		strcat(cp,num);
		strcat(cp,") ");
	}

	if ( a & FSR_AA )
		strcat(cp,"AA ");

	if ( a & FSR_AI )
		strcat(cp,"AI ");

	if ( a & FSR_AO )
		strcat(cp,"AO ");

	if ( a & FSR_AU )
		strcat(cp,"AU ");

	if ( a & FSR_MA )
		strcat(cp,"MA ");

	if ( a & FSR_MI )
		strcat(cp,"MI ");

	if ( a & FSR_MO )
		strcat(cp,"MO ");

	if ( a & FSR_MU )
		strcat(cp,"MU ");

	if ( a & FSR_SE )
		strcat(cp,"SE ");

	if ( a & FSR_SI )
		strcat(cp,"SI ");

	if ( a & FSR_FTE )
		strcat(cp,"FTE ");

	if ( a & FSR_U )
		strcat(cp,"U ");

	i = (a & FSR_RM)>>2;
	strcat(cp,"RM(0x");
	itoa(i,num);
	strcat(cp,num);
	strcat(cp,") ");

	if ( a & FSR_TI )
		strcat(cp,"TI ");

	if ( a & FSR_FZ )
		strcat(cp,"FZ");

	printf("fsr(%x) %s\n",a,buf);
}

#else

print_FP( FP )
       FPtrapcontrol   *FP;
{
    int i;

    printf("fp1 ");
    print_i860type(&(FP->fp1));
    printf("fp2 ");
    print_i860type(&FP->fp2);
    printf("fp3 ");
    print_i860type(&FP->fp3);
    printf("fp4 ");
    print_i860type(&FP->fp4);
    printf("mres0 ");
    print_i860type(&FP->mres[0]);
    printf("mres1 ");
    print_i860type(&FP->mres[1]);
    printf("mres2 ");
    print_i860type(&FP->mres[2]);
    printf("ares0 ");
    print_i860type(&FP->ares[0]);
    printf("ares1 ");
    print_i860type(&FP->ares[1]);
    printf("ares2 ");
    print_i860type(&FP->ares[2]);
    printf("ki ");
    print_i860type(&FP->ki);
    printf("kr ");
    print_i860type(&FP->kr);
    printf("t ");
    print_i860type(&FP->t);
    printf("the floating point regsiters \n");

    for (i = 0; i < 16; i++) {
	printf(" i  =  %lx  , even = %lx , odd = %lx \n",
	      i,
	      FP->fpreg[i].two_words.bottom_word32,
	      FP->fpreg[i].two_words.top_word32);
    }
}
#endif
#endif
