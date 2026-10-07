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

	.file	"_setjmp.s"

/* akp
 * C library -- _setjmp, _longjmp
 *
 *	_longjmp(a,v)
 * will generate a "return(v)" from
 * the last call to
 *	_setjmp(a)
 * by restoring registers from the stack,
 * The previous signal state is NOT restored.
 */
	.dsect
u_r0:	.long		// not really restored (but saved)
u_r1:	.long		// return address
u_sp:	.long		// stack pointer
u_fp:	.long		// frame pointer
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
u_f0:	.long		// not really restored (but saved)
u_f1:	.long		// not really restored (but saved)
u_f2:	.long		// non-volatile
u_f3:	.long		// non-volatile
u_f4:	.long		// non-volatile
u_f5:	.long		// non-volatile
u_f6:	.long		// non-volatile
u_f7:	.long		// non-volatile
	.end

	.text
	.align	4
__setjmp::
#ifdef _THREAD_SAFE
_setjmp::
#endif /* _THREAD_SAFE */
	st.l	r0,u_r0(r16)		// always 0
	st.l	r1,u_r1(r16)		// return address
	st.l	sp,u_sp(r16)		// stack pointer
	st.l	fp,u_fp(r16)		// frame pointer
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
#if 1
	fst.l	f0,u_f0(r16)		// store f0...f7
	fst.l	f1,u_f1(r16)
	fst.l	f2,u_f2(r16)
	fst.l	f3,u_f3(r16)
	fst.l	f4,u_f4(r16)
	fst.l	f5,u_f5(r16)
	fst.l	f6,u_f6(r16)
	fst.l	f7,u_f7(r16)
#else
	fst.q	f0,u_f0(r16)		// store f0,f1,f2,f3
	fst.q	f4,u_f4(r16)		// store f4,f5,f6,f7
#endif
	bri	r1
	 mov	r0,r16			// return 0

	.text
	.align	4
__longjmp::
#ifdef _THREAD_SAFE
_longjmp::
#endif /* _THREAD_SAFE */


        btne    0,r17,non_zero          // if rval is zero
        adds    1,r0,r17                // then set rval to be 1
non_zero:
	mov	r17,r18			// return v of _longjmp(e,v)

	//ld.l	u_r0(r16),r0		// always 0
	ld.l	u_r1(r16),r1		// return address
	ld.l	u_sp(r16),sp		// stack pointer
	ld.l	u_fp(r16),fp		// frame pointer
	ld.l	u_r4(r16),r4
	ld.l	u_r5(r16),r5
	ld.l	u_r6(r16),r6
	ld.l	u_r7(r16),r7
	ld.l	u_r8(r16),r8
	ld.l	u_r9(r16),r9
	ld.l	u_r10(r16),r10
	ld.l	u_r11(r16),r11
	ld.l	u_r12(r16),r12
	ld.l	u_r13(r16),r13
	ld.l	u_r14(r16),r14
	ld.l	u_r15(r16),r15
#if 1
	fld.l	u_f2(r16),f2		// load f2...f7
	fld.l	u_f3(r16),f3
	fld.l	u_f4(r16),f4
	fld.l	u_f5(r16),f5
	fld.l	u_f6(r16),f6
	fld.l	u_f7(r16),f7
#else
	fld.d	u_f2(r16),f2		// load f2,f3
	fld.q	u_f4(r16),f4		// load f4,f5,f6,f7
#endif
	bri	r1
	 mov	r18,r16			// return v

