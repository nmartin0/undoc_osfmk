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
 * $Log: fpdefs.h,v $
 * Revision 1.6  1994/11/18  20:38:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:30:39  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1992/11/12  02:55:26  stans
 * updated NaN comments to be correct.
 *
 * Revision 1.3  1992/11/06  16:56:27  stans
 * commented different types of NaN's.
 *
 * Revision 1.2  1992/10/28  22:35:19  stans
 * l unit pipline increased in depth (3-->6) in support of pfld.q i860XP
 *
 * Revision 1.1  1992/08/26  10:40:53  stans
 * Initial revision
 *
 * Revision 2.3.1.1  92/05/27  00:40:12  jeffreyh
 * 	[stans@ssd.intel.com]
 * 	Support floating point exceptions.
 * 
 * Revision 2.3  91/06/18  20:51:03  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:53:12  jsb]
 * 
 * Revision 2.2  91/06/17  15:44:34  jsb
 * 	From Paul Pierce: created from iPSC/860 NX code.
 * 	[91/06/17  13:43:28  jsb]
 * 
 */

/*
 * definitions for the ieee floating point trap handler
 * NX version fpdefs.h 8.2 91/01/10 15:41:59
 *
 * $Id: fpdefs.h,v 1.6 1994/11/18 20:38:54 mtm Exp $
 */

/* double floating point 64 bit */
#define i860_double double

/* single floating point 32 bit */
#define i860_single float

/* word of length 32 */
#define word32 unsigned

/*************** end of 386 specific definitions **************/

typedef struct  {
	word32 bottom_word32;
	word32 top_word32;
} word64;


typedef struct {
	unsigned mantissa_c : 32;
	unsigned mantissa_b : 19;
	unsigned mantissa_a :  1;
	unsigned exponent : 11;
	unsigned sign : 1;
} double_pattern;

typedef struct {
	unsigned mantissa_b : 22; 
	unsigned mantissa_a :  1;
	unsigned exponent : 8;
	unsigned sign : 1;
	word32 dummy;
} single_pattern; 

typedef union {
	i860_double n_double;
	double_pattern b_double;
	i860_single n_single;
	single_pattern b_single;
	word64 two_words;
} i860_type; 

typedef union {
	word64 two_words;
	i860_double double_res;
	i860_single single_res[2];
} double_2_singles;
                               
typedef struct trapper { 
	word32 intreg[32];  /* integer registers */
	double_2_singles fpreg0;    /* floating point register 0 */
	double_2_singles fpreg[15]; /* floating point registers */
	i860_type t;
	i860_type ki;
	i860_type kr;
	word64   merge;   /* lower 32 bits of merge register */
	i860_type lres[6]; /* l unit pipeline */
	i860_type ares[3]; /* a unit pipeline */
	i860_type mres[3]; /* m unit pipeline */
	i860_type ires[1]; /* i unit pipeline */
	word32 fsrs[3]; /* fsr pipeline */
	word32 psr;
	word32 *trapped_instruction; /* FIR */
	word32 dirbase;
	word32 db;
	word32 epsr;
	i860_type fp1; /* pass operands to user IEEE exception handler */
	i860_type fp2;
	i860_type fp3;
	i860_type fp4;
	word32 trapped_opcode;  /* of source error */ 
	word32 rdest; /* dest field of trapped instruction on source error */
	word32 src1;  /* the reg# of src1, needed for k loads */
} FPtrapcontrol;

typedef FPtrapcontrol *FPtrapcontrol_t;


/********************* OPCODES *****************************************/
#define FMUL 1
#define FRCP 2
#define FRSQR 3
#define FADD 4
#define FSUB 5
#define FIX 6
#define PFGT 7
#define PFLE 8
#define PFEQ 9
#define FTRUNC 10
#define PFAM 11
#define PFSM 12
#define FAMOV 13
#define NUM_OF_OPCODES 13

#define ADDER_OP(x) (x>=FADD  && x<=FTRUNC)
#define OVERFLOW 0
#define UNDERFLOW 1
#define NO_USER_HANDLER 0
#define CALL_USER_HANDLER 1
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif
#define NOT_USED -1
#define SINGLE 0
#define DOUBLE 1
#define M_UNIT 1
#define A_UNIT 0
#define SUBTRACT 1
#define ADDITION 0
#define IEEE_ERROR_d 1
#define NO_ERROR 0
#define SOURCE_ERR_VALID_OPER 0x01
#define INVALID_OPERATION 0x02 
#define DIV_BY_ZERO_TRAP 0x04  
#define OVERFLOW_TRAP 0x08 
#define UNDERFLOW_TRAP 0x010 
#define INEXACT_TRAP 0x020 
#define ONE_UNKNOWN_AND_IEEE_ERROR 0x040
#define FLOAT_CODE 0x12
#define UNIX_DEFAULT 2
#define SIZE32 32
#define SIZE64 64

/*************** BIT PATTERNS FOR HANDLING IEEE_status_word *********/
#define ONLY_MASK_AND_STICKY 0x0ffc00
#define KNF_BIT               0x0400000
#define DUALOP_BIT           0x0800000
#define PIPELINE_BIT         0x01000000
#define S_PRECISION_BIT      0x0200000
#define M_PRECISION_BIT      S_PRECISION_BIT
#define R_PRECISION_BIT      0x0100000
#define A_PRECISION_BIT      R_PRECISION_BIT  /* precision bit for add part
                                                 of dualop is same as result
                                                 precision for operation    */
#define KR_LOAD_BIT          0x04000000
#define KI_LOAD_BIT          0x02000000
#define T_LOAD_BIT           0x08000000
#define M_INV_OPER_BIT       0x01
#define A_INV_OPER_BIT       0x020
#define M_DIV_BY_ZERO_BIT    0x02
#define M_OVERFLOW_BIT       0x04
#define A_OVERFLOW_BIT       0x080
#define M_UNDERFLOW_BIT      0x08
#define A_UNDERFLOW_BIT      0x0100
#define M_INEXACT_BIT        0x010
#define A_INEXACT_BIT        0x0200
#define INVALID_MASK         0x08000
#define DIV_BY_ZERO_MASK     0x010000
#define OVERFLOW_MASK        0x020000
#define UNDERFLOW_MASK       0x040000
#define INEXACT_MASK         0x080000
#define M_STATUS_MASK 0x01f
#define A_STATUS_MASK 0x03e0
#define A_STATUS_SHIFT 5
#define STICKY_STATUS_MASK 0x07c00
#define STICKY_STATUS_SHIFT 10
#define IEEE_MASK_MASK 0x0f8000
#define IEEE_MASK_SHIFT 15

/*************** BIT PATTERNS FOR HANDLING fsr *********/
#define M_OVERFLOW_OCCURED_BIT  0x0400
#define A_OVERFLOW_OCCURED_BIT  0x04000
#define M_UNDERFLOW_OCCURED_BIT 0x0200
#define A_UNDERFLOW_OCCURED_BIT 0x02000
#define M_INEXACT_OCCURED_BIT   0x0800
#define A_INEXACT_OCCURED_BIT   0x08000
#define A_UNIT_PRECISION_BIT    0x20000000
#define M_UNIT_PRECISION_BIT    0x10000000
#define AE_BITS                 0x01c00000
#define RR_BITS                 0x03e0000
#define FZ_BIT                  0x01
#define TI_BIT                  0x02
#define SI_BIT                  0x80
#define MA_BIT                  0x01000
#define AA_BIT                  0x010000
#define ROUNDING_MODE_BITS      0x0000000c
#define ONLY_M_INFO             0x10001e00
#define EXCEPT_M_INFO           0x2fffe1ff
#define FSR_CLEAR_M_UNIT        0x2fffe1ff
#define ONLY_A_INFO             0x2001e000
#define EXCEPT_A_INFO           0x1ffe1fff
#define FSR_CLEAR_A_UNIT        0x1ffe1fff

#define NO_M_UNIT_UNDERFLOW_OR_INEXACT 0xfffff5ff
#define NO_A_UNIT_UNDERFLOW_OR_INEXACT 0xffff5fff
#define FSR_CLEAR_RE 0x3fff01ff
#define FSR_CLEAR    0x3fff00ff
#define A_RESULT_ERROR_BITS 0x0e000
#define A_UNIT_RESULT_ERROR FPptr->fsrs[2] & A_RESULT_ERROR_BITS
#define M_RESULT_ERROR_BITS 0x00e00
#define M_UNIT_RESULT_ERROR FPptr->fsrs[2] & M_RESULT_ERROR_BITS
#define RESULT_ERROR_BITS 0x0ee00
#define RESULT_INFO_BITS 0x01c1fe00
#define RESULT_ERROR_OCCURED FPptr->fsrs[2] & RESULT_ERROR_BITS
#define RESULT_ERROR_SHIFT 9
#define AE_SHIFT 14
#define RR_SHIFT 17
#define ROUNDING_MODE_SHIFT 2
#define TO_NEAREST 0x0
#define TO_ZERO 0x3
#define DOWNWARD 0x1
#define UPWARD 0x2


/*************** BIT PATTERNS FOR HANDLING psr *****************/

#define CLEAR_CC_BIT 0xfffffffb
#define SET_CC_BIT   0x00000004
#define PSR_KNF_BIT  0x00008000

/*************** BIT PATTERNS FOR HANDLING i860_TYPE *****************/
#define MANTISSA_SIZE_S 23
#define MANTISSA_SIZE_D 52
#define SINGLE_FIX 0x100
#define SINGLE_FIXER 0x101
#define DOUBLE_FIX 0x800
#define DOUBLE_FIXER 0x801
#define SINGLE_SHIFT 192
#define DOUBLE_SHIFT 1536
#define DOUBLE_BIAS 1023
#define SINGLE_BIAS 127
#define SINGLE_LOWEST 126   /* actually -126 but is subtracted so +126 */
#define SINGLE_EXP_BITS 0x7f800000
#define SINGLE_MANTISSA_BITS 0x007fffff
/* 
 * DKG changed according to ieee of 387 (by murali's test )
 * #define DEFAULT_RESULT_S_B 0x7fc00000
 * #define DEFAULT_RESULT_D_U 0x7ff80000
 */
 /* 0x7ff80000 double precision quiet NaNQ
  * 0x7ff40000  makes signaling NaNS
  * 0xfff80000 double precision quiet -NaNQ
  * 0xfff40000 double precision signaling -NaNS
  */
#define DEFAULT_RESULT_S_U 0x0
#define DEFAULT_RESULT_D_U 0xfff80000 /* double precision quiet -NaNQ */
#define DEFAULT_RESULT_S_B 0xffc00000	/* single precision quiet -NANQ */
#define DEFAULT_RESULT_D_B 0x0

#define POS_INFINITY_D_U 0x7ff00000
#define POS_INFINITY_D_B 0x0
#define POS_INFINITY_S_U 0x0
#define POS_INFINITY_S_B 0x7f800000

#define NEG_INFINITY_D_U 0xfff00000
#define NEG_INFINITY_D_B 0x0
#define NEG_INFINITY_S_U 0x0
#define NEG_INFINITY_S_B 0xff800000

#define POS_BIGGEST_S_U 0x0
#define POS_BIGGEST_S_B 0x7f7fffff
#define POS_BIGGEST_D_U 0x7fefffff
#define POS_BIGGEST_D_B 0xffffffff

#define NEG_BIGGEST_S_U 0x0
#define NEG_BIGGEST_S_B 0xff7fffff
#define NEG_BIGGEST_D_U 0xffefffff
#define NEG_BIGGEST_D_B 0xffffffff
