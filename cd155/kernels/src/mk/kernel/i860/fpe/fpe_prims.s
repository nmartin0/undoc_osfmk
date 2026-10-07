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
 
//      Copyright (c) 1989,1990 Intel Corporation.
//      All rights reserved.                     
//                                                   
//         INTEL CORPORATION PROPRIETARY INFORMATION  
//                                                   
//   This software is supplied under the terms of a license
//   agreement or nondisclosure agreement with Intel Corp. 
//   and may not be copied or disclosed except in accordance
//   with the terms of that agreement.                      

/*
 * $Id: fpe_prims.s,v 1.4 1994/11/18 20:40:27 mtm Exp $
 *
 * HISTORY
 * $Log: fpe_prims.s,v $
 * Revision 1.4  1994/11/18  20:40:27  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/06/30  22:34:22  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.2  1992/11/14  00:01:15  andyp
 * Nifty new FPE handler from SVR4.
 *
 */

#ident "@(#)fpe:fpe_prims.s     1.1"

#define NO_XP_A0	1

//
// void normsd(s)
// ulong *s;
//
// Convert a normalized single prec. number to double prec. number.
// Result is always normalized and exact.
//
	.globl	_normsd
_normsd:
	fld.l	0(r16),f16
	fmov.sd	f16,f18
	fst.l	f18,0(r16)
	bri	r1
	fst.l	f19,4(r16)


// 
// void densd(s)
// ulong *s;
//
// Convert a denormal single prec. number to a double precision number. Note
// that the result becomes normalized, always exact.
//
	.data
	.align	8
DPpow_126:
	.long	0,0x3c100000	//2**-126 in double precision

	.text
	.globl	_densd
_densd:
	ld.l	0(r16),r17	//float1 = s[0];
	orh	0x3f80,r17,r17	//float1 *= (2 ** 126)
	ixfr	r17,f16		//float1 = input whose 0 exp is replaced by 3f8
	andh	0xff80,r17,r17	//
	ixfr	r17,f17		//float2 = float1 without fraction bits.
	fsub.ss	f16,f17,f16	//float1 = float1 - float2 (rid of integer bit)
	fmov.sd	f16,f18	//double = (double) float1
	orh	ha%DPpow_126,r0,r31
	fld.d	l%DPpow_126(r31),f16
	fmul.dd	f18,f16,f18	//double *= (2**-126) (because of step 2)
	fst.l	f18,0(r16)	//s[0] = low word of dp. number
	bri	r1
	fst.l	f19,4(r16)	//s[1] = hi  word of dp. number

//
// fmuldd(s1, s2, d);
//
	.globl	_fmuldd
_fmuldd:
	fld.l	0(r16),f16
	fld.l	4(r16),f17
	fld.l	0(r17),f18
	fld.l	4(r17),f19
	fmul.dd	f16,f18,f20
	fst.l	f20,0(r18)
	fst.l	f21,4(r18)
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif

// 
// fmulss(s1, s2, d)
//
	.globl	_fmulss
_fmulss:
	fld.l	0(r16),f16	//s1
	fld.l	0(r17),f17	//s2
	fmul.ss	f16,f17,f18	//d = s1 * s2
	fst.l	f18,0(r18)
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif


//
// fadddd(s1, s2, d);
// double *s1, *s2, *d;
//
	.globl	_fadddd
_fadddd:
	fld.l	0(r16),f16
	fld.l	4(r16),f17	//s1
	fld.l	0(r17),f18
	fld.l	4(r17),f19	//s2
	fadd.dd	f16,f18,f20
	fst.l	f20,0(r18)	//d = s1 + s2
	fst.l	f21,4(r18)
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif

// 
// faddss(s1, s2, d)
// float *s1, *s2, *d
//
	.globl	_faddss
_faddss:
	fld.l	0(r16),f16	//s1
	fld.l	0(r17),f17	//s2
	fadd.ss	f16,f17,f18	//d = s1 + s2
	fst.l	f18,0(r18)
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif

	.globl	_famovds
_famovds:
	fld.l	0(r16),f16
	fld.l	4(r16),f17	//source
	fmov.ds	f16,f18	//single precision result.
	fst.l	f18,0(r17)
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif

	.globl	_fpsetfsr
_fpsetfsr:
	ld.c	fsr,r17
	st.c	r16,fsr
	bri	r1
	mov	r17,r16

	.globl	_fpgetfsr
_fpgetfsr:
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop

	.globl	_get_fsr
_get_fsr:
#ifdef NO_XP_A0
	bri	r1
	ld.c	fsr,r16
#else
	ld.c	fsr,r16
	nop;nop;nop
	bri	r1
	nop
#endif

	.globl	_set_fsr
_set_fsr:
	bri	r1
	st.c	r16,fsr

	.globl	_sqrtss
_sqrtss:
	frsqr.ss	f8,f10
	bri	r1
	fmov.ss	f10,f8

	.globl	_sqrtsd
_sqrtsd:
	frsqr.sd	f8,f10
	bri	r1
	fmov.dd	f10,f8

	.globl	_sqrtdd
_sqrtdd:
	frsqr.dd	f8,f10
	bri	r1
	fmov.dd	f10,f8

