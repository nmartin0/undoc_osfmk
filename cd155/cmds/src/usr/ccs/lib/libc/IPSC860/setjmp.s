// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
//	Copyright (c) 1989, Intel Corporation.
//	All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//
	.file	"setjmp.s"

#ifndef  _THREAD_SAFE
/* akp
 * C library -- setjmp, longjmp
 *
 *	longjmp(a,v)
 * will generate a "return(v)" from
 * the last call to
 *	setjmp(a)
 * by restoring registers from the stack,
 * The previous value of the signal mask is
 * restored.
 *
 */
#include <i860/psl.h>
/*
 * This must match the sigcontext structure!!
 */
	.dsect
u_onstk: .long		// see sigcontext
u_mask:	.long		//
u_r0:	.long		// always 0
u_r1:	.long		// return address (non-volatile)
u_sp:	.long		// stack pointer (non-volatile)
u_fp:	.long		// frame pointer (non-volatile)
u_r4:	.long		// non-volatile
u_r5:	.long		// non-volatile
u_r6:	.long		// non-volatile
u_r7:	.long		// non-volatile
u_r8:	.long		// non-volatile
u_r9:	.long		// non-volatile
u_r10:	.long		// non-volatile
u_r11:	.long		// non-volatile
u_r12:	.long		// non-volatile
u_r13:	.long		// non-volatile
u_r14:	.long		// non-volatile
u_r15:	.long		// non-volatile
u_r16:	.long		// parameters and temporaries (volatile)
u_r17:	.long		// parameters and temporaries (volatile)
u_r18:	.long		// parameters and temporaries (volatile)
u_r19:	.long		// parameters and temporaries (volatile)
u_r20:	.long		// parameters and temporaries (volatile)
u_r21:	.long		// parameters and temporaries (volatile)
u_r22:	.long		// parameters and temporaries (volatile)
u_r23:	.long		// parameters and temporaries (volatile)
u_r24:	.long		// parameters and temporaries (volatile)
u_r25:	.long		// parameters and temporaries (volatile)
u_r26:	.long		// parameters and temporaries (volatile)
u_r27:	.long		// parameters and temporaries (volatile)
u_r28:	.long		// memory parameters or structures (volatile)
u_r29:	.long		// "emulator return address" (see i860/trap.c)
u_r30:	.long		// unused (volatile)
u_r31:	.long		// system call code (volatile)
u_f0:	.long		// always 0
u_f1:	.long		// always 0
u_f2:	.long		// non-volatile
u_f3:	.long		// non-volatile
u_f4:	.long		// non-volatile
u_f5:	.long		// non-volatile
u_f6:	.long		// non-volatile
u_f7:	.long		// non-volatile
u_psr:	.long		// processor status register
u_fsr:	.long		// floating-point status register
u_epsr:	.long		// extended processor status register
u_fir:	.long		// pc, sometimes...
	.end

	.text
	.align	4
_setjmp::
	st.l	r0,u_r0(r16)
	st.l	r1,u_r1(r16)
	st.l	sp,u_sp(r16)
	st.l	fp,u_fp(r16)
	st.l	r4,u_r4(r16)
	st.l	r5,u_r5(r16)
	st.l	r6,u_r6(r16)
	st.l	r7,u_r7(r16)
	st.l	r8,u_r8(r16)
	st.l	r9,u_r9(r16)
	st.l	r10,u_r10(r16)
	st.l	r11,u_r11(r16)
	st.l	r12,u_r12(r16)
	st.l	r13,u_r13(r16)
	st.l	r14,u_r14(r16)
	st.l	r15,u_r15(r16)
	st.l	r16,u_r16(r16)
	st.l	r17,u_r17(r16)
	st.l	r18,u_r18(r16)
	st.l	r19,u_r19(r16)
	st.l	r20,u_r20(r16)
	st.l	r21,u_r21(r16)
	st.l	r22,u_r22(r16)
	st.l	r23,u_r23(r16)
	st.l	r24,u_r24(r16)
	st.l	r25,u_r25(r16)
	st.l	r26,u_r26(r16)
	st.l	r27,u_r27(r16)
	st.l	r28,u_r28(r16)
	st.l	r29,u_r29(r16)
	st.l	r30,u_r30(r16)
	st.l	r31,u_r31(r16)
#if 1
	// NOT really aligned beyond single-word
	fst.l	f0,u_f0(r16)	// store f0...f7
	fst.l	f1,u_f1(r16)
	fst.l	f2,u_f2(r16)
	fst.l	f3,u_f3(r16)
	fst.l	f4,u_f4(r16)
	fst.l	f5,u_f5(r16)
	fst.l	f6,u_f6(r16)
	fst.l	f7,u_f7(r16)
#else
	fst.q	f0,u_f0(r16)	// store f0,f1,f2,f3
	fst.q	f4,u_f4(r16)	// store f4,f5,f6,f7
#endif
	ld.c	psr,r30
	andnot	PSR_CC,r30,r30	// clear condition-code (no syscall errs)
	st.l	r30,u_psr(r16)
	ld.c	fsr,r30
	st.l	r30,u_fsr(r16)
	ld.c	epsr,r30
	st.l	r30,u_epsr(r16)

	st.l	r1,u_fir(r16)	// sigreturn now assume u_fir is set up!!

	mov	r16,r4		// hold onto the jmpbuf pointer
	addu	-16,sp,sp	// local temporary space

	// get current signal stack state
	mov	sp,r17		// r17 = temp space
	call	_sigstack	// sigstack(0, r17)
	 mov	r0,r16
	ld.l	0(sp),r16	// jmpbuf[u_onstk] = sp[0];
	st.l	r16,u_onstk(r4)

	// get current signal mask
	call	_sigblock	// jmpbuf[u_mask] = sigblock(0)
	 mov	r0,r16
	st.l	r16,u_mask(r4)

	// unwind and leave
	addu	16,sp,sp
	mov	r4,r16
	ld.l	u_r1(r16),r1	// restore r1
	ld.l	u_r4(r16),r4	// restore r4
	bri	r1
	 mov	r0,r16			// return 0

/*
 *    longjmp(env, val)
 *	jmp_buf	env;
 *	int	val;
 *
 *	setjmp(&jmpbuf) saved all registers; r1 is place to return to.
 *	'sigreturn(&jmpbuf)' does a syscall which restores all registers and
 *	returns to instruction after the syscall_trap. PSR bits are set for
 *	no errror so 'r1' becomes the sigreturn() return address, which is
 *	really the original setjmp() return address... And so the magic goes!!
 */
	.text
	.align	4
_longjmp::
	ld.l	u_psr(r16),r30
	andnot	PSR_CC,r30,r30	// clear condition-code (no syscall errs)
	st.l	r30,u_psr(r16)

	btne	0,r17,non_zero	// if rval is zero
	adds	1,r0,r17	// then set rval to be 1
non_zero:
        br      _sigreturn
	 st.l	r17,u_r16(r16)	// jmpbuf[u_r16] = return value

	// not reached...theorectically
#endif  /* _THREAD_SAFE */
