// 
// $Copyright
// Copyright 1991 , 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * HISTORY
 * $Log: csw.s,v $
// Revision 1.6  1994/11/18  21:07:25  mtm
// Copyright additions/changes
//
// Revision 1.5  1993/06/30  23:06:37  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.4  1993/05/17  22:47:00  andyp
// Cleaned up context switch code.  Killed a dormant bug at the bottom
// of cproc_prepare() -- there was no instruction in the delay slot of the
// bri -- it had been commented out.  Also, don't save/restore f8-f15.
// Reorded two instructions in lock.s to have one less instruction under
// the bus lock.
//
// Revision 1.3  1993/04/27  21:00:50  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  19:07:47  dleslie
// First R1_0 release
//
 * Revision 2.3  91/12/10  16:33:16  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:31:50  jsb]
 * 
 * Revision 2.2  91/09/04  11:29:13  jsb
 * 	First checkin. Contributed by Intel/SSD.
 * 	[91/09/02  08:54:36  jsb]
 * 
 */
	.text

#define	PRIME_FOR_WRITEBACK	1	/* prime the cache to get store/hit */

/*
 * Suspend the current thread and resume the next one.
 *
 *	void
 *	cproc_switch(cur, next, lock)
 *		int *cur;
 *		int *next;
 *		int *lock;
 *
 *	1) save the non-volatile register set on the stack (r0-r16,f0-f7)
 *	   with the obvious optimizations w/ respect to r0, f0, f1.
 *	2) *cur = sp
 *	3) sp = *next;
 *	4) restore the register set from the stack
 *	5) clear the lock and return
 */
	.align	4
	.globl	_cproc_switch
_cproc_switch:
	addu	-96,sp,sp	// 96 == 16 (of 32) iregs + 8 (of 32) fpregs

#if	PRIME_FOR_WRITEBACK
	ld.l	 0(sp),r0	// prime the cache to make the stores hit cache
	ld.l	32(sp),r0	// prime
	ld.l	64(sp),r0	// prime
#endif	PRIME_FOR_WRITEBACK

	//
	//	non-volatile iregs
	//
	//st.l	r0,0(sp)	// not saved
	st.l	r1,4(sp)	// return address
	st.l	sp,8(sp)	// stack pointer -- saved for debug purposes
	st.l	fp,12(sp)	// frame pointer
	st.l	r4,16(sp)
	st.l	r5,20(sp)
	st.l	r6,24(sp)
	st.l	r7,28(sp)
	st.l	r8,32(sp)
	st.l	r9,36(sp)
	st.l	r10,40(sp)
	st.l	r11,44(sp)
	st.l	r12,48(sp)
	st.l	r13,52(sp)
	st.l	r14,56(sp)
	st.l	r15,60(sp)

	//
	//	non-volatile fregs (don't save f0 and f1)
	//
	fst.d	f2,72(sp)	//         f2, f3
	fst.q	f4,80(sp)	// f4, f5, f6, f7

	st.l	sp,0(r16)	// *cur = sp
				// switch stacks
	ld.l	0(r17),sp	// sp = *next

	//
	//	reload saved non-volatile iregs
	//
	//ld.l	0(sp),r0	// not saved
	ld.l	4(sp),r1	// return address
	//ld.l	8(sp),sp	// stack pointer -- no need to reload
	ld.l	12(sp),fp	// frame pointer
	ld.l	16(sp),r4
	ld.l	20(sp),r5
	ld.l	24(sp),r6
	ld.l	28(sp),r7
	ld.l	32(sp),r8
	ld.l	36(sp),r9
	ld.l	40(sp),r10
	ld.l	44(sp),r11
	ld.l	48(sp),r12
	ld.l	52(sp),r13
	ld.l	56(sp),r14
	ld.l	60(sp),r15

	//
	//	reload saved non-volatile fregs
	//
	fld.d	72(sp),f2	//         f2, f3
	fld.q	80(sp),f4	// f4, f5, f6, f7

	st.l	r0,0(r18)	// clear the lock
	bri	r1		// return to next thread
	 addu	96,sp,sp	// unwind the stack pointer

/*
 *	void
 *	cproc_start_wait(parent_context, child, stackp, lock)
 *		int *parent_context;
 *		cproc_t child;
 *		int stackp;
 *		int *lock;
 */
        .globl  _cproc_start_wait
_cproc_start_wait:
	addu	-96,sp,sp	// 96 == 16 (of 32) iregs + 8 (of 32) fpregs

#if	PRIME_FOR_WRITEBACK
	ld.l	 0(sp),r0	// prime the cache to make the stores hit cache
	ld.l	32(sp),r0	// prime
	ld.l	64(sp),r0	// prime
#endif	PRIME_FOR_WRITEBACK

	//
	//	non-volatile iregs
	//
	//st.l	r0,0(sp)	// not saved
	st.l	r1,4(sp)	// return address
	st.l	sp,8(sp)	// stack pointer -- saved for debug purposes
	st.l	fp,12(sp)	// frame pointer
	st.l	r4,16(sp)
	st.l	r5,20(sp)
	st.l	r6,24(sp)
	st.l	r7,28(sp)
	st.l	r8,32(sp)
	st.l	r9,36(sp)
	st.l	r10,40(sp)
	st.l	r11,44(sp)
	st.l	r12,48(sp)
	st.l	r13,52(sp)
	st.l	r14,56(sp)
	st.l	r15,60(sp)

	//
	//	non-volatile fregs (don't save f0 and f1)
	//
	fst.d	f2,72(sp)	//         f2, f3
	fst.q	f4,80(sp)	// f4, f5, f6, f7

	st.l	sp,0(r16)	// *parent_context = sp
	andnot	0xf,r18,sp	// align and switch stacks (sp = stackp)
	st.l	r0,0(r19)	// clear the lock
	br	_cproc_waiting	// jump directly there
	 mov	r17,r16		// argument to cproc_waiting()

/*
 *
 * Set up a thread's stack so that when cproc_switch switches to
 * it, it will start up as if it called cproc_body(child)
 *
 *	void
 *	cproc_prepare(child, child_context, stack)
 *		cproc_t	child;
 *		int	*child_context;
 *		int	*stack;
 *
 */
	.globl	_cproc_prepare
_cproc_prepare:
	addu	-16,r18,r18	// save "child" for later use by "loadarg"
	st.l	r16,0(r18)

	addu	-96,r18,r18	// cproc_switch's "frame"

#if	PRIME_FOR_WRITEBACK
	ld.l	 0(r18),r0	// prime the cache so the following stores hit
	ld.l	32(r18),r0	// prime
	ld.l	64(r18),r0	// prime
#endif	PRIME_FOR_WRITEBACK

	//st.l	r0,0(r18)	// not really needed
	st.l	r1,4(r18)	// return address
	st.l	sp,8(r18)	// stack pointer -- saved for debug purposes
	st.l	fp,12(r18)	// frame pointer
	st.l	r4,16(r18)
	st.l	r5,20(r18)
	st.l	r6,24(r18)
	st.l	r7,28(r18)
	st.l	r8,32(r18)
	st.l	r9,36(r18)
	st.l	r10,40(r18)
	st.l	r11,44(r18)
	st.l	r12,48(r18)
	st.l	r13,52(r18)
	st.l	r14,56(r18)
	st.l	r15,60(r18)

	fst.d	f2,72(r18)	//         f2, f3
	fst.q	f4,80(r18)	// f4, f5, f6, f7

	st.l	r18,0(r17)	// *child_context = r18

	// it gets a little complicated now...
	// cthread_body() will expect its single argument (child)
	// in r16.  Save it "above" the prepared stack and instead of
	// setting the return address to cthread_body(), we instead return
	// to "loadarg" which loads it from 0(sp) into r16.
	// It *then* calls cthread_body().

	orh	h%loadarg,r0,r31
	or	l%loadarg,r31,r31
	bri	r1
	 st.l	r31,4(r18)		// return to "loadarg" when switched

	// Can only get here as a result of a cproc_switch().
	// Need to jump to cthread_body() with "child" in r16.
	// "child" was stored at 0(r18) by cproc_prepare().
loadarg:
	orh	h%_cthread_body,r0,r31
	or	l%_cthread_body,r31,r31
	bri	r31			// branch to cthread_body()
	 ld.l	0(sp),r16		// "child" put here by cproc_prepare()
