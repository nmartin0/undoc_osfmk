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
 * Copyright 1991 by Intel Corporation,
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
 * $Id: cswitch.s,v 2.11 1995/04/04 21:23:14 lenb Exp $
 */
/*
 *	Kernel thread switch routines for i860.
 *
 *	Much of the work is actually done by the callers of these
 *	functions.
 *
 *	Only the non-volatile registers are saved and restored.
 */

#include <cpus.h>			/* NCPUS */
#include <i860/cpu_number.h>		/* FAST_CPU_NUMBER */
#include <i860/psl.h>			/* PSR_IAT */
#include <mach/i860/vm_param.h>		/* KERNEL_STACK_SIZE (16K) */
#define	__GENASSYM_FRAME_DEFS__		/* get genassym offsets */
#include <assym.s>

#if	defined(i860XP) && (NCPUS == 1 || ASMP)
#define	FASTTRAPS	1
#endif	/* defined(i860XP) && (NCPUS == 1 || ASMP) */

	.file	"cswitch.s"

//
// DANGER WILL ROBINSON!!!
//
// This structure MUST BE consistent with struct i860_kernel_state
// defined in thread.h
//
	.dsect
ks_r0:	.long		// not really restored (but saved)
ks_r1:	.long		// return address
ks_sp:	.long		// stack pointer
ks_fp:	.long		// frame pointer
ks_r4:	.long		// non-volatile
ks_r5:	.long		// non-volatile
ks_r6:	.long		// non-volatile
ks_r7:	.long		// non-volatile
ks_r8:	.long		// non-volatile
ks_r9:	.long		// non-volatile
ks_r10:	.long		// non-volatile
ks_r11:	.long		// non-volatile
ks_r12:	.long		// non-volatile
ks_r13:	.long		// non-volatile
ks_r14:	.long		// non-volatile
ks_r15:	.long		// non-volatile
ks_f0:	.long		// not really restored (but saved)
ks_f1:	.long		// not really restored (but saved)
ks_f2:	.long		// non-volatile
ks_f3:	.long		// non-volatile
ks_f4:	.long		// non-volatile
ks_f5:	.long		// non-volatile
ks_f6:	.long		// non-volatile
ks_f7:	.long		// non-volatile
	.end

//
//	void i860_context_load(ks, retval)
//	struct i860_kernel_state *ks;
//	int retval;
//
//	"ks" points to a saved-state area.
//	Assumes splsched().
//	Only non-volatiles are reloaded.
//	Assumes caller has installed the needed values on the
//	globals like active_threads, kernel_stack, etc.
//
//	thread_t i860_context_switch(newks, cont, oldks, retval)
//	void (*cont)();
//	struct i860_kernel_state *oldks, *newks;
//	int retval;
//
//	Save the non-volatile register set in oldks.
//	Restore the non-volatile registers from newks by
//	flowing into to i860_context_load() with the registers
//	arranged just so...
//
//	We do not have to save the registers if there is an
//	explicit continuation.
//

#define	EXPERIMENTAL_CONTEXT_LOAD	1
#define	EXPERIMENTAL_CONTEXT_SAVE	1
#define	PIPED_EXPERIMENTAL_CONTEXT_LOAD	0	/*(EXPERIMENTAL_CONTEXT_LOAD && i860XP)*/

	.text
	.align	32
	.globl	_i860_context_switch
	.globl	_i860_context_load
_i860_context_switch:
	btne	r0,r17,1f

#if	EXPERIMENTAL_CONTEXT_SAVE

	//
	//	Push the non-volatile integer registers over to the
	//	volatile fp registers, and then push everything out
	//	in big chunks...
	//
	ixfr	r1,f17
	ixfr	sp,f18
	ixfr	fp,f19
	ixfr	r4,f20
	ixfr	r5,f21
	ixfr	r6,f22
	ixfr	r7,f23
	ixfr	r8,f24
	ixfr	r9,f25
	ixfr	r10,f26
	ixfr	r11,f27
	ixfr	r12,f28
	ixfr	r13,f29
	ixfr	r14,f30
	ixfr	r15,f31
	//
	//	If PARANOID_PCB isn't on, perhaps:
	//	fst.q f16,ks_r0(r18) would be "simpler"
	//
	//fst.l	f16,ks_r0(r18)		// don't save r0
	fst.l	f17,ks_r1(r18)		// save staged r1
	fst.d	f18,ks_sp(r18)		// save staged sp-fp (r2-r3)
	fst.q	f20,ks_r4(r18)		// save staged r4-r7
	fst.q	f24,ks_r8(r18)		// save staged r8-r11
	fst.q	f28,ks_r12(r18)		// save staged r12-r15
	fst.q	f4,ks_f4(r18)		// save f4-f7
	fst.d	f2,ks_f2(r18)		// save f2-f3

#else	EXPERIMENTAL_CONTEXT_SAVE

	//st.l	r0,ks_r0(r18)	// XXX see i860/pcb.c, PARANOIDPCB -- andyp
	st.l	r1,ks_r1(r18)
	st.l	sp,ks_sp(r18)
	st.l	fp,ks_fp(r18)
	st.l	r4,ks_r4(r18)
	st.l	r5,ks_r5(r18)
	st.l	r6,ks_r6(r18)
	st.l	r7,ks_r7(r18)
	st.l	r8,ks_r8(r18)
	st.l	r9,ks_r9(r18)
	st.l	r10,ks_r10(r18)
	st.l	r11,ks_r11(r18)
	st.l	r12,ks_r12(r18)
	st.l	r13,ks_r13(r18)
	st.l	r14,ks_r14(r18)
	st.l	r15,ks_r15(r18)
	fst.q	f0,ks_f0(r18)
	fst.q	f4,ks_f4(r18)

#endif	EXPERIMENTAL_CONTEXT_SAVE

1:
	mov	r19,r17			// to return "old"

	// and flow into...

_i860_context_load:
#if	EXPERIMENTAL_CONTEXT_LOAD

	//
	//	Use the floating point registers as a staging
	//	area for the integer registers.
	//

#if	PIPED_EXPERIMENTAL_CONTEXT_LOAD

	//
	//	Pipeline in the non-volatile registers
	//	in the largest chunks possible.
	//
	//	XXX Assuming no cache hits, the following
	//	XXX nine loads will complete in about
	//	XXX 540ns on a 50MHz Paragon GP node
	//	XXX and will not pollute the dcache.
	//	XXX
	//	XXX From i860_context_load() through
	//	XXX the pfld's and fxfr's to the bri
	//	XXX the bottom, I count (best case)
	//	XXX about 900ns.
	//
	pfld.q	ks_r0(r16),f16	// prime   r0-r3
	pfld.q	ks_r4(r16),f16	// prime   r4-r7
	pfld.q	ks_r8(r16),f16	// prime   r8-r11
	pfld.q	ks_r12(r16),f16	// stage   r0-r3, prime r12-r15
	pfld.q	ks_f4(r16),f20	// stage   r4-r7, prime f4-f7
	//fxfr	f16,r0
	fxfr	f17,r1
	fxfr	f18,sp
	fxfr	f19,fp
	pfld.d	ks_f2(r16),f24	// stage  r8-r11, prime f2-f3
	fxfr	f20,r4
	fxfr	f21,r5
	fxfr	f22,r6
	fxfr	f23,r7
	pfld.d	ks_r0(r16),f28	// stage r12-r15, drain
	fxfr	f24,r8
	fxfr	f25,r9
	fxfr	f26,r10
	fxfr	f27,r11
	pfld.d	ks_r0(r16),f4	// restore f4-f7, drain
	fxfr	f28,r12
	fxfr	f29,r13
	fxfr	f30,r14
	fxfr	f31,r15
	pfld.d	ks_r0(r16),f2	// restore f2-f3, drain


#else	PIPED_EXPERIMENTAL_CONTEXT_LOAD

	fld.d   ks_f2(r16),f2           // restore f2-f3
	fld.q   ks_f4(r16),f4           // restore f4-f7

	fld.q	ks_r0(r16),f16		// stage r0-r3
	fld.q	ks_r4(r16),f20		// stage r4-r7
	fld.q	ks_r8(r16),f24		// stage r8-r11
	fld.q	ks_r12(r16),f28		// stage r12-r15

	//
	//	Contents of r0-r15 are now staged in
	//	f16-f31; transfer them.
	//
	//fxfr	f16,r0			// always 0 unless PARANOID_PCB
	fxfr	f17,r1
	fxfr	f18,sp
	fxfr	f19,fp
	fxfr	f20,r4
	fxfr	f21,r5
	fxfr	f22,r6
	fxfr	f23,r7
	fxfr	f24,r8
	fxfr	f25,r9
	fxfr	f26,r10
	fxfr	f27,r11
	fxfr	f28,r12
	fxfr	f29,r13
	fxfr	f30,r14
	fxfr	f31,r15

#endif	PIPED_EXPERIMENTAL_CONTEXT_LOAD

#else	EXPERIMENTAL_CONTEXT_LOAD

	//ld.l	ks_r0(r16),r0		// always 0
	ld.l	ks_r1(r16),r1		// return address
	ld.l	ks_sp(r16),sp		// stack pointer
	ld.l	ks_fp(r16),fp		// frame pointer
	ld.l	ks_r4(r16),r4
	ld.l	ks_r5(r16),r5
	ld.l	ks_r6(r16),r6
	ld.l	ks_r7(r16),r7
	ld.l	ks_r8(r16),r8
	ld.l	ks_r9(r16),r9
	ld.l	ks_r10(r16),r10
	ld.l	ks_r11(r16),r11
	ld.l	ks_r12(r16),r12
	ld.l	ks_r13(r16),r13
	ld.l	ks_r14(r16),r14
	ld.l	ks_r15(r16),r15
	fld.d	ks_f2(r16),f2		// load f2,f3
	fld.q	ks_f4(r16),f4		// load f4,f5,f6,f7

#endif	EXPERIMENTAL_CONTEXT_LOAD

	mov	r17,r16			// return retval
	bri	r1
	 mov	r0,r1			// clear the return address

//
// Continuation routines do not return so all registers are available.
//
//	r1	temp, used to save branch after call
//	r4	flags:
//	CALL_CONTINUATION	r0
#define	EXCEPTION_RETURN	1
#define	BOOTSTRAP_RETURN	2
#define	SYSCALL_RETURN		4
//	r5	SYSCALL_RETURN flag
//	r6	EXCEPTION_RETURN flag
//	r7	current_thread()
//	r8	current_stack()
//	r9	retval (if any)
//	r13	&regs
//	r16	&continuation (if any)
//	r31	temp

_call_continuation::
	//
	// void call_continuation(void (*continuation)())
	//
	br	thread_continue_common
	 adds	r0, r0, r4

_thread_ctrap_return::
	//
	// void thread_ctrap_return()
	//
	br	thread_continue_common		// skip the baton_exit()
	 adds	EXCEPTION_RETURN, r0, r4

#if	ASMP
_thread_bootstrap_return::
	//
	// void thread_bootstrap_return()
	//
	mov	thread_continue_common, r1
	br	_baton_exit
	 adds	BOOTSTRAP_RETURN, r0, r4

_thread_exception_return::
	//
	// void thread_exception_return()
	//
	mov	thread_continue_common, r1
	br	_baton_exit
	 adds	EXCEPTION_RETURN, r0, r4

_thread_syscall_return::
	//
	// void thread_syscall_return(kern_return_t retval)
	//
	mov	r16, r9				// r9 = retval
thread_syscall_baton_exit::			// label for baton_disable()
	call	_baton_exit
	 adds	SYSCALL_RETURN, r0, r4
	// fall-through

#else	/* !ASMP */
_thread_bootstrap_return::
	br	thread_continue_common
	 mov	BOOTSTRAP_RETURN, r4

_thread_exception_return::
	br	thread_continue_common
	 mov	EXCEPTION_RETURN, r4

_thread_syscall_return::
	mov	r16, r9				// r9 = retval
	mov	SYSCALL_RETURN, r4
	// fall-through
#endif	/* !ASMP */

thread_continue_common:
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31			// r31 = (cpu_number << 2)

	orh	ha%_active_threads,r31,r7
	ld.l	l%_active_threads(r7),r7	// r7 = current_thread()

	orh	ha%_active_stacks,r31,r8
	ld.l	l%_active_stacks(r8),r8		// r8 = current_stack()

	ld.l	THREAD_PCB(r7), r13		// r13 = thread->pcb
						// r13 = &regs (pcb offset 0)

	mov	SYSCALL_RETURN, r5		// r5 = SYSCALL_RETURN
	btne	r4, r5, .not_syscall		// if (!SYSCALL_RETURN) br

	ld.l	ufir(r13), r31			// r31 = regs->pc
	st.l	r9, ur16(r13)			// regs->r16 = retval
	adds	4, r31, r31			// r31 += 4

	br	do_continuation
	 st.l	r31, ufir(r13)			// regs->pc = pc+4

.not_syscall:
	bte	r0, r4, do_continuation		// CALL_CONTINUATION case

	mov	EXCEPTION_RETURN, r6		// r6 = EXCEPTION_RETURN
	btne	r4, r6, do_continuation	// if (!EXCEPTION_RETURN) br

	ld.l	upsr(r13),r31			// r31 = regs->psr
	and	PSR_IAT, r31, r31		// r31 = PSR_IAT?
	bte	r0, r31, do_continuation	// if !(regs->psr & PSR_IAT) br

	call	_flush
	 nop

do_continuation:
	//
	// reset stack pointer to base of stack
	//
	adds	KERNEL_STACK_SIZE - I860_STACK_COOKIES, r8, r31
	andnot	15, r31, sp			// sp = TOPOF(stack)
	mov	r0, fp				// zero the frame pointer
	mov	r0, r1				// zero the return address

	btne	r4, r5, not_syscall

	//
	// SYSCALL_RETURN
	//
#if	FASTTRAPS
	br	_fast_syscall_continue
	 nop
#else	FASTTRAPS
	br	_bootstrap_return_from_trap
	 st.c	r0,psr				// r13 = &regs
#endif	FASTTRAPS

not_syscall:
	btne	r0, r4, not_call_continuation

	//
	// CALL_CONTINUATION
	//
	bri	r16
	 mov	r13, r16			// r16 = &regs

not_call_continuation:
	btne	r4, r6, not_exception

	//
	// EXCEPTION_RETURN
	//
	br	_continue_return_from_trap
	 st.c	r0,psr				// r13 = &regs

not_exception:
	//
	// BOOTSTRAP_RETURN
	//
	br	_bootstrap_return_from_trap
	 st.c	r0,psr				// r13 = &regs


