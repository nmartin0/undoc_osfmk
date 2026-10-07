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
 * $Id: fpe.h,v 1.5 1994/11/18 20:40:17 mtm Exp $
 *
 * HISTORY
 * $Log: fpe.h,v $
 * Revision 1.5  1994/11/18  20:40:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:10:09  dleslie
 * Added copyright required by legal
 *
 * Revision 1.3  1992/11/19  03:49:48  andyp
 * Small cosmetic change to glue the FPE handler to the user-mode
 * routines fpsetmask(), fpgetmask().
 * XXX not extensively tested.
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe.h     3.0"

#include <kern/thread.h>

#define	intr_enable()	splon(PSR_IM)
#define	intr_disable()	sploff()

typedef unsigned long	ulong;


/*
 *  data types used in the floating point trap handler
 */
typedef struct  {
	ulong 		lo;
	ulong		hi;
} dlong_t;

typedef struct {
	unsigned 	mantissa_c : 32;
	unsigned 	mantissa_b : 19;
	unsigned 	mantissa_a :  1;
	unsigned 	exponent : 11;
	unsigned 	sign : 1;
} double_pattern;

typedef struct {
	unsigned 	mantissa_b : 22; 
	unsigned 	mantissa_a :  1;
	unsigned 	exponent : 8;
	unsigned 	sign : 1;
	ulong 		pad;
} single_pattern; 

typedef union {
        double 		n_double;
	double_pattern 	b_double;
	float 		n_single;
	single_pattern 	b_single;
	dlong_t 	dlong;
} fp_t; 

/*
 * fp_frame_t has the same structure as mcontext_t defined in "sys/ucontext.h".
 * The offsets for each members of fp_frame_t are also defined in "sys/reg.h".
 */    
typedef struct { 
	ulong intreg[32];		/* interger registers */
	ulong fregs[32];                /* floating point registers */
        fp_t t;				/* t register */
        fp_t ki;			/* ki register */
        fp_t kr;			/* kr register */
	fp_t merge;			/* merge register */
	fp_t lres[6];			/* load pipeline */
        fp_t ares[3]; 			/* a unit pipeline */
        fp_t mres[3]; 	    		/* m unit pipeline */
        fp_t ires[1]; 			/* i unit pipeline */
	ulong fsrs[3];			/* pipeline floating status register */
	ulong psr;			/* process status register */
	ulong *fir;			/* fir */
	ulong dirbase;			/* dirbase */
	ulong db;			/* db register */
	ulong epsr;			/* epsr register */
        fp_t fp1; 			/* fp1-fp4 */
        fp_t fp2;			/*   passing to */
        fp_t fp3;			/*   the signal handler */ 
        fp_t fp4;			/*  			*/
        ulong trapped_opcode;  		/* of source error */ 
} fp_frame_t;


#define FPstatus	(current_thread()->pcb->ieee_status)

/* 
 * SE_DIV_BY_ZERO, SE_INVALID, and SE_NO_IEEE_ERROR are return values
 * from determine_source_error() and indicates the type of i860's source 
 * error. 
 */
#define SE_DIV_BY_ZERO		0x01
#define SE_INVALID		0x02
#define SE_NO_IEEE_ERROR	0x04

/*
 * bits fields of ieee_status 
 */

#define IEEE_M_INV	0x00000001	
#define	IEEE_M_DIVZ	0x00000002
#define IEEE_M_OFLW	0x00000004
#define IEEE_M_UFLW	0x00000008
#define IEEE_M_INEXT	0x00000010
#define IEEE_A_INV	0x00000020
#define IEEE_A_DIVZ	0x00000040
#define IEEE_A_OFLW	0x00000080
#define IEEE_A_UFLW	0x00000100
#define IEEE_A_INEXT	0x00000200
#define IEEE_S_INV	0x00000400
#define IEEE_S_DIVZ	0x00000800
#define IEEE_S_OFLW	0x00001000
#define IEEE_S_UFLW	0x00002000
#define IEEE_S_INEXT 	0x00004000
#define IEEE_X_INV	0x00008000
#define IEEE_X_DIVZ	0x00010000
#define IEEE_X_OFLW	0x00020000
#define IEEE_X_UFLW	0x00040000
#define IEEE_X_INEXT	0x00080000
#define IEEE_R_PREC	0x00100000
#define IEEE_A_PREC	0x00100000
#define IEEE_S_PREC	0x00200000
#define IEEE_M_PREC	0x00200000
#define IEEE_KNF_SET	0x00400000
#define IEEE_DUAL_OP	0x00800000
#define IEEE_PIPE_OP	0x01000000
#define IEEE_KI_LOAD	0x02000000
#define IEEE_KR_LOAD	0x04000000
#define IEEE_T_LOAD	0x08000000

#define IEEE_MBITS	(IEEE_M_INV|IEEE_M_DIVZ|IEEE_M_OFLW|IEEE_M_UFLW|IEEE_M_INEXT)
#define IEEE_ABITS	(IEEE_A_INV|IEEE_A_DIVZ|IEEE_A_OFLW|IEEE_A_UFLW|IEEE_A_INEXT)
#define IEEE_SBITS	(IEEE_S_INV|IEEE_S_DIVZ|IEEE_S_OFLW|IEEE_S_UFLW|IEEE_S_INEXT)
#define IEEE_MASKS	(IEEE_X_INV|IEEE_X_DIVZ|IEEE_X_OFLW|IEEE_X_UFLW|IEEE_X_INEXT)

#define IEEE_A_SHIFT	5
#define IEEE_S_SHIFT	10
#define IEEE_X_SHIFT	15


/*
 * bit fields of i860 floating point instruction
 */
#define FINS_PIPE	0x00000400
#define FINS_DUAL	0x00000200
#define FINS_S_PREC	0x00000100
#define FINS_R_PREC	0x00000080 

/*
 * Trapped Codes 
 */

#define FMUL 		1
#define FRCP 		2
#define FRSQR 		3
#define FADD 		4
#define FSUB 		5
#define FIX 		6
#define FAMOV           7
#define PFGT 		8
#define PFLE           	9 
#define PFEQ 		10	
#define FTRUNC 		11
#define PFAM 		12
#define PFSM 		13
#define NUM_OF_OPCODES  13

#define IS_ADDER_OP(x) 		(x>=FADD  && x<=FTRUNC)

/*
 * externs
 */
extern dlong_t SingleDefaultResult,DoubleDefaultResult;
extern dlong_t SinglePosInfinity,DoublePosInfinity;
extern dlong_t SingleNegInfinity,DoubleNegInfinity;
extern dlong_t SinglePosBiggest,DoublePosBiggest;
extern dlong_t SingleNegBiggest,DoubleNegBiggest;
extern dlong_t SinglePosSmallest;

extern	ulong	SPindefinite[];
extern	ulong	SPinf[];
extern	ulong	SPlargest[];
extern	ulong	SPpow34[];
extern	ulong	SPpow32[];
extern	ulong	SPpow4[];
extern	ulong	SPpow2[];
extern	ulong	SPzero[];
extern	ulong	SPpow_32[];
extern	ulong	SPsmallest[];

extern	ulong	DPindefinite[];
extern	ulong	DPinf[];
extern	ulong	DPlargest[];
extern	ulong	DPpow64[];
extern	ulong	DPpow66[];
extern	ulong	DPpow4[];
extern	ulong	DPpow2[];
extern	ulong	DPzero[];
extern	ulong	DPpow_64[];
extern	ulong	DPsmallest[];

extern	ulong	INzero[];
extern	ulong	INindefinite[];


#ifdef DEBUG
extern int fpe_debug;
#endif
