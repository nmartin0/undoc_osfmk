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
 * $Id: ttrap.s,v 1.47 1995/04/04 21:22:09 lenb Exp $
 */

#include <cpus.h>
#include <ctrap_history.h>
#include <mach_assert.h>

#include <i860/psl.h>
#include <i860/trap.h>
#include <i860/cpu_number.h>
#ifdef	i860XP
#include <i860paragon/lbus.h>
#endif  /*i860XP*/
#if	PARAGON860
#include <i860paragon/baton.h>
#endif	/* PARAGON860 */

#include <assym.s>	/* get genassym offsets */

#if	PARAGON860
#if	NCPUS == 1 || defined(ASMP)
#define SCALL_SHORTCUT	1
#else
#define SCALL_SHORTCUT	0	/* XXX still needs some MP work */
#endif
#endif	PARAGON860

#if	iPSC860
#define SCALL_SHORTCUT	0	/* XXX still needs some work */
#endif	iPSC860

#define FAST_EMUL_SYSCALL_DETECT 1
#define LASC 0	/* Logic Analyzer Stack Check */

#if	PARAGON860
#define	LATRIGGER	0
#endif	PARAGON860

#if	defined(i860XP) && (NCPUS == 1 || ASMP)
#define FASTTRAPS	1
#endif	/* defined(i860XP) && (NCPUS == 1 || ASMP) */


	.file "ttrap.s"

#define	ALL_TRAP_BITS	(PSR_IT|PSR_IN|PSR_IAT|PSR_DAT|PSR_FT)
#define	K_FRAME_SIZE	512


	.page
	.dsect
uintreg:	.long	[32]0
ufltreg:	.long	[32]0
ut:		.double
uki:		.double
ukr:		.double
umerge:		.long	[2]0
ul1:		.double [2]0	// only first 8 bytes used on XR
ul2:		.double [2]0	// only first 8 bytes used on XR
ul3:		.double [2]0	// only first 8 bytes used on XR
ua1:		.double
ua2:		.double
ua3:		.double
um1:		.double
um2:		.double
um3:		.double
ui1:		.long	[2]0
ufsr1:		.long
ufsr2:		.long
ufsr:		.long
upsr:		.long
ufir:		.long
udirbase:	.long
udb:		.long
uepsr:		.long
ufp1:		.long	[2]0
ufp2:		.long	[2]0
ufp3:		.long	[2]0
ufp4:		.long	[2]0
utrapped_opcode:.long
urdest:		.long
usrc1:		.long
ur13:		.long
ur14:		.long
ur15:		.long
ukrki:		.long
utrapno:	.long
uvaddr:		.long
ucode:		.long
	.end

//uksp		=  -4	// kernel sp	- available kernel stack
utr13		=  -8	// int		- user r13 at entry
utr14		= -12	// int		- user r14 at entry
utr15		= -16	// int		- user r15 at entry
utsp		= -20	// int		- user r2 (sp) at entry
udummy		= -32	// must be 8-byte aligned

	.data
	.align 16
DoubleOne:	.double	1.0
DoubleZero:	.double 0.0
MinusZero:	.long 0x00000000,0x80000000
SingleOne:	.float	1.0
_DAT_ON::	.long 0

	.page
	.text
////////////////////////////////////////////////////////////////////////
//
// _alltraps
//	i860 trap handler common entry to all i860 traps.
//
////////////////////////////////////////////////////////////////////////

#ifdef	i860XP

	.align	32
_alltraps::
	//
	// Free up some registers...the only way to do it is
	// to save them relative to r0.
	// 
#ifdef i860XP
	// Done at 0xFFFFFF00
#else
	st.c	r13,p1		// save r13
	st.c	r14,p2		// save r14
	st.c	r15,p3		// save r15
#endif
	ld.c	fir,r14		// latch fir into r14 at trap entry

#if	LATRIGGER
	//
	// read the DP status register as soon as possible. Make sure
	// we only do this for INT-type traps. r15 will contain the
	// psr so look here for the INT bit.
	and	0x200, r15, r0
	bc	nola
	nop
	orh	h%0x70000210,r0,r13
	or	l%0x70000210,r13,r13
	ldio.l	r13,r13
nola:	nop
#endif	LATRIGGER

#if	FASTTRAPS
	//
	//	all instruction traps take a fast path...
	//
	and	PSR_IT,r15,r0
	bnc.t	_fast_syscall	// branch if PSR_IT set, and
	 ld.l	0(r14),r13	// ...fetch the trap instruction
.not_so_fast:
#endif	FASTTRAPS

#if	LASC
        orh     h%0x65300000,r0,r13
        stio.l  r14,r13 // fir
        stio.l  r15,r13 // psr
#endif

#if	MCMSG || (NCPUS > 1)
	.globl	_mp_start

	/*
	 * If no interesting trap bits set in psr
	 * assume this is a slave CPU coming out of reset.
	 */
	or	ALL_TRAP_BITS,r0,r13
	and	r13,r15,r0
	bc	_mp_start		// CC=1 implies zero value, ie. reset

	/*
	 * Since we're not a slave coming out of reset, we assume
	 * that CPU_REG and thus FAST_CPU_NUMBER() are valid.
	 */
	FAST_CPU_NUMBER(r13)
#else	/* MCMSG || (NCPUS > 1) */

	/*
	 * Uniprocessor -- cpu_number is always 0.
	 */
	or	r0, r0, r13

#endif	/* MCMSG || (NCPUS > 1) */
	//
	// where are we coming from?
	//
	and	PSR_PU,r15,r0		// previously in user mode?
	bc	was_kmode		// taken if we were in user mode.

	//
	// user-to-kernel traps:
	//	a) set r13 to current_pcb
	//	b) set sp to current_thread->kernel_stack+I860_MAGIC_COOKIES
	//	c) we don't move r13-r15 saved in p1...p3 to the saved state
	//
	// at this juncture we have:
	//
	//	p1 == saved r13
	//	p2 == saved r14
	//	p3 == saved r15
	//	r13 == cpu number
	//	r14 == fir
	//	r15 == psr
	//
was_umode:

#if	NCPUS > 1
	shl	2,r13,r13	// convert cpu # to longword offset
	orh	ha%_current_pcb,r13,r13
	ld.l	l%_current_pcb(r13),r13
#else
	orh	ha%_current_pcb,r0,r13
	ld.l	l%_current_pcb(r13),r13
#endif

	//	r13 == &current_pcb[cpu_number()]

	st.l	sp,8(r13)	// save exception time sp

	// get some scratch registers to work with
	st.l	r28,112(r13)	// save r28 (so it can be used)
	st.l	r29,116(r13)	// save r29 (so it can be used)
	st.l	r30,120(r13)	// save r30 (so it can be used)
	st.l	r31,124(r13)	// save r31 (so it can be used)

	// at this juncture we have:
	//
	//	p1 == saved r13
	//	p2 == saved r14
	//	p3 == saved r15
	//	sp == available
	//	r13 == &current_pcb[cpu_number()]
	//	r14 == fir
	//	r15 == psr
	//	r28 == available
	//	r29 == available
	//	r30 == available
	//	r31 == available
	//

#if	FAST_EMUL_SYSCALL_DETECT

#define SYS_CALL        0x47e0f800      /* trap r31,r31,r0 */
#define	SYS_CALL2	0x44008000	/* trap r16,r0,r0 */
	/*
	 * trampoline? emulated syscall
	 *	r13 == exception frame pointer.
	 *	r14 == fir
	 *	r15 == psr
	 *	r28...r31 scratch.
	 *
	 * instruction trap?
	 */
	and	PSR_IT,r15,r0
	bc	.no_tramp
	/* instruction trap in locked seq? */
	ld.c	epsr,r28
	and	EPSR_IL,r28,r0
	bnc	.no_tramp
	/*
	 * instruction trap not in a locked sequence, was it a syscall?
	 */
	ld.l	0(r14),r29		/* fetch user`s trap instruction */
	orh	h%SYS_CALL,r0,r28
	or	l%SYS_CALL,r28,r28
	bte	r28,r29,.uxcall1
	orh	h%SYS_CALL2,r0,r28
	or	l%SYS_CALL2,r28,r28
	btne    r28,r29,.no_tramp
.uxcall1:

	/*
	 * is it an emulated syscall, or Mach system call?
	 */ 
#if	NCPUS > 1
	FAST_CPU_NUMBER(r28)
	shl	2,r28,r28	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r28,r28
	ld.l	l%_active_threads(r28),r28	/* r28 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r28
	ld.l	l%_active_threads(r28),r28	/* r28 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_TASK(r28),r29		/* r29 == current task */
	ld.l	TASK_EML_DISPATCH(r29),r30	/* task's dispatch table */
	bte	r0,r30,.nscall			/* EML_DISPATCH_NULL? */
	/*
	 * Calculate the index into the dispatch table
	 * dispatch_table_index = code - task->eml_dispatch->disp_min;
	 * if (dispatch_table_index < 0) return
	 *
	 * r30 == task->eml_dispatch, r31 == syscall code from user
	 */
	ld.l	EMLDISP_MIN(r30),r29		/* r29 dispatch min */
	subs	r31,r29,r29			/* r29 = dispatch_table_index */
	bc	.nscall

	/*
	 * Make sure the syscall is not out of range of the dispatch table.
	 * if (dispatch_table_index >= task->eml_dispatch->disp_count) return;
	 *	r29 dispatch table index
	 *	r30 task->eml_dispatch
	 */
	ld.l	EMLDISP_CNT(r30),r28		/* entries in dispatch table */
	bte	r29,r28,.nscall
	subs	r28,r29,r0
	bc	.nscall

	/*
	 * if ((entry=task->eml_dispatch->disp_vector[dispatch_table_index]==0)
	 *	return;
	 */
	addu	EMLDISP_VECT,r30,r28	/* start of dispatch table */
	shl	2,r29,r29		/* longword index */
	ld.l	r29(r28),r31		/* emulated syscall start, user space */
	bte	r0,r31,.nscall		/* NULL is a loose */

	/*
	 * regs->r29 = regs->pc + 4;
	 * regs->pc = (unsigned int) entry;
	 */
	addu	4,r14,r29		/* user's syscall trap pc + 4 */
	ld.c    p2,r14			/* restore r14 */

        // restore scratch registers to exception time values
	ld.l    124(r13),r28		/* exception time r31 */
	st.c    r28,p2			/* save for on the way out */
	ld.l    112(r13),r28		// restore r28
	// r29 is clobbered with user's syscall return address
	ld.l    120(r13),r30		// restore r30
	ld.c    p1,r13			/* restore r13 */

        // clear out all of the trap bits, stay in supervisor mode,
        // keep interrupts turned off.  turn on 1 trap bit to return
        // from the trap.
        //
        andnot  PSR_FT|PSR_DAT|PSR_IAT|PSR_IN|PSR_IT|PSR_U|PSR_IM,r15,r15
        or      PSR_IT,r15,r15  // set a trap bit to cause return from trap
        st.c    r15,psr
	ld.c    p3,r15			/* restore r15 */
	bri	r31
	  ld.c	p2,r31			/* restore r31 */
	//return to user!!

	/*
	 * NOT an emulated syscall.
	 */

.nscall:
#if	SCALL_SHORTCUT
	// setup r29 in the following fashion:
	//	 0 : no syscall
	//	 1 : take syscall trap
	// 	 2 : take FP result error

	// result bits of FSR are checked here
	//    and it will work around the erratum #30
	ld.c	fsr, r29
	and 	0x0020,r29,r0		// if FTE of FSR is 0
	bc.t	.ns_0			// go to check_next
	 mov	1, r29			// take a syscall shortcut
	and	0xee00,r29,r0		// if any result bits are not set
	bc.t	.ns_0			// go to check_next
	 mov	1, r29			// take a syscall shortcut
	and	0x6600,r29,r0 		// if MU,MO,AU,AO bits of FSR are set
	bnc	.handle_er30		// go to handle_er30 
	and 	0x0002,r29,r0		// if TI of FSR is not set  
	bc.t	.ns_0			// go to check_next
	 mov	1, r29			// take a syscall shortcut
.handle_er30:
	or	PSR_FT,r15,r15		// set FT bit of PSR	
	andnot	PSR_IT,r15,r15		// clear IT bit of PSR
#endif	SCALL_SHORTCUT

.no_tramp:
#if	SCALL_SHORTCUT
	mov	0, r29		// indicate we do NOT have a syscall shortcut
.ns_0:
#endif	SCALL_SHORTCUT

#endif	FAST_EMUL_SYSCALL_DETECT

	ld.c	p1,r28		// saved r13
	ld.c	p2,r30		// saved r14
	ld.c	p3,r31		// saved r15
	st.l	r28,52(r13)	// save r13 at trap entry
	st.l	r30,56(r13)	// save r14 at trap entry
	st.l	r31,60(r13)	// save r15 at trap entry

	/*
	 * compute the TOP of current_thread()->kernel_stack and set 'sp'.
	 */
#if	NCPUS > 1
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r31,r31
	ld.l	l%_active_threads(r31),r31	/* r31 == current_thread() */
#else
	orh	ha%_active_threads,r0,r31
	ld.l	l%_active_threads(r31),r31	/* r31 == current_thread() */
#endif
	ld.l	THREAD_KSTACK(r31),sp	/* base (low adrs) of kernel stack */
	addu	KSTACK_SIZE,sp,sp	/* compute top of stack */

	// at this juncture we have:
	//
	//	p1 == available
	//	p2 == available
	//	p3 == available
	//	sp == valid
	//	r13 == &current_pcb[cpu_number()]
	//	r14 == fir
	//	r15 == psr
	//	r28 == available
	//	r29 == scall_shortcut flag
	//	r30 == available
	//	r31 == available
	//

	br	save_state
	  andnot 0xf,sp,sp		/* 16-byte align my kstack */

	// NOT REACHED

was_kmode:
	//
	// kernel-to-kernel traps:
	// at this juncture we have:
	//	p1 == saved r13
	//	p2 == saved r14
	//	p3 == saved r15
	//	r13 == scratch
	//	r14 == fir
	//	r15 == psr
	//	sp == exception time sp, not saved yet.
	//
	// Stay on same kernel stack after 16-byte align and  exception frame
	// allocation, r13 points to base of exception frame..
	//

	andnot  0xf,sp,r13		// 16-byte alignment
	addu	-K_FRAME_SIZE,r13,r13	// allocate an exception frame
	st.l	sp,8(r13)		// save exception time sp
	or	r13,r0,sp		// reset sp for next k-mode exception.

	// get some scratch register space.
	st.l	r28,112(r13)	// save r28 (so it can be used)
	st.l	r29,116(r13)	// save r29 (so it can be used)
	st.l	r30,120(r13)	// save r30 (so it can be used)
	st.l	r31,124(r13)	// save r31 (so it can be used)

	ld.c	p1,r29		// saved r13
	ld.c	p2,r30		// saved r14
	ld.c	p3,r31		// saved r15
	st.l	r29,52(r13)	// save r13 at trap entry
	st.l	r30,56(r13)	// save r14 at trap entry
	st.l	r31,60(r13)	// save r15 at trap entry

#if	SCALL_SHORTCUT
	or	r0,r0,r29	// set syscall shortcut boolean FALSE
#endif
	// at this juncture we have:
	//
	//	p1 == available
	//	p2 == available
	//	p3 == available
	//	sp == valid kernel stack pointer
	//	r13 == &current_pcb[cpu_number()]
	//	r14 == fir
	//	r15 == psr
	//	r28 == available
	//	r29 == 0 for scall_shortcut
	//	r30 == available
	//	r31 == available
	//

	/*
	 * fall through to save_state:
	 */

#else	/* i860XP */


	.align	32

_alltraps::
	//
	// Free up some registers...the only way to do it is
	// to save them relative to r0.
	// 
	st.l	sp,utsp(r0)	// save sp
	st.l	r13,utr13(r0)	// save r13
	st.l	r14,utr14(r0)	// save r14
	st.l	r15,utr15(r0)	// save r15
	ld.c	psr,r15		// latch psr into r15 at trap entry
	st.c	r0,psr		// ensure interrupts and trap bits are off
	ld.c	fir,r14		// latch fir into r14 at trap entry

	//
	// where are we coming from?
	//
	and	PSR_PU,r15,r0		// previously in user mode?
	bc	was_kmode		// taken if we were in user mode.

	//
	// user-to-kernel traps:
	//	a) set r13 to current_pcb
	//	b) move sp & r13-r15 saved on page 0 to the saved state
	//	c) set sp to kstack_current
	//
was_umode:
	orh	ha%_current_pcb,r0,r13
	ld.l	l%_current_pcb(r13),r13

	st.l	r28,112(r13)	// save r28 (so it can be used)
	st.l	r29,116(r13)	// save r29 (so it can be used)
	st.l	r30,120(r13)	// save r30 (so it can be used)
	st.l	r31,124(r13)	// save r31 (so it can be used)

#if	FAST_EMUL_SYSCALL_DETECT

#define SYS_CALL        0x47e0f800      /* trap r31,r31,r0 */
#define	SYS_CALL2	0x44008000	/* trap r16,r0,r0 */
	/*
	 * trampoline? emulated syscall
	 *	r13 == exception frame pointer.
	 *	r14 == fir
	 *	r15 == psr
	 *	r28...r31 scratch.
	 *
	 * instruction trap?
	 */
	and	PSR_IT,r15,r0
	bc	.no_tramp
	/* instruction trap in locked seq? */
	ld.c	epsr,r28
	and	EPSR_IL,r28,r0
	bnc	.no_tramp
	/*
	 * instruction trap not in a locked sequence, was it a syscall?
	 */
	ld.l	0(r14),r29		/* fetch user`s trap instruction */
	orh	h%SYS_CALL,r0,r28
	or	l%SYS_CALL,r28,r28
	bte	r28,r29,.uxcall2
	orh	h%SYS_CALL,r0,r28
	or	l%SYS_CALL2,r28,r28
	btne	r28,r29,.no_tramp
.uxcall2:
	/*
	 * is it an emulated syscall, or Mach system call?
	 */ 
#if	NCPUS > 1
	FAST_CPU_NUMBER(r28)
	shl	2,r28,r28	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r28,r28
	ld.l	l%_active_threads(r28),r28	/* r28 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r28
	ld.l	l%_active_threads(r28),r28	/* r28 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_TASK(r28),r29		/* r29 == current task */
	ld.l	TASK_EML_DISPATCH(r29),r30	/* task's dispatch table */
	bte	r0,r30,.no_tramp		/* EML_DISPATCH_NULL? */
	/*
	 * Calculate the index into the dispatch table
	 * dispatch_table_index = code - task->eml_dispatch->disp_min;
	 * if (dispatch_table_index < 0) return
	 *
	 * r30 == task->eml_dispatch, r31 == syscall code from user
	 */
	ld.l	EMLDISP_MIN(r30),r29		/* r29 dispatch min */
	subs	r31,r29,r29			/* r29 = dispatch_table_index */
	bc	.no_tramp
	/*
	 * Make sure the syscall is not out of range of the dispatch table.
	 * if (dispatch_table_index >= task->eml_dispatch->disp_count) return;
	 *	r29 dispatch table index
	 *	r30 task->eml_dispatch
	 */
	ld.l	EMLDISP_CNT(r30),r28		/* entries in dispatch table */
	bte	r29,r28,.no_tramp
	subs	r28,r29,r0
	bc	.no_tramp
	/*
	 * if ((entry=task->eml_dispatch->disp_vector[dispatch_table_index]==0)
	 *	return;
	 */
	addu	EMLDISP_VECT,r30,r28	/* start of dispatch table */
	shl	2,r29,r29		/* longword index */
	ld.l	r29(r28),r31		/* emulated syscall start, user space */
	bte	r0,r31,.no_tramp	/* NULL is a loose */

	/*
	 * regs->r29 = regs->pc + 4;
	 * regs->pc = (unsigned int) entry;
	 */
	addu	4,r14,r29		/* user's syscall trap pc + 4 */
	ld.l    utr14(r0),r14		/* restore r14 */

        // restore scratch registers to exception time values
	ld.l    124(r13),r28		/* exception time r31 */
	st.l    r28,utr14(r0)		/* save for on the way out */
	ld.l    112(r13),r28		// restore r28
	// r29 is clobbered with user's syscall return address
	ld.l    120(r13),r30		// restore r30
	ld.l    utr13(r0),r13		/* restore r13 */

        // clear out all of the trap bits, stay in supervisor mode,
        // keep interrupts turned off.  turn on 1 trap bit to return
        // from the trap.
        //
        andnot  PSR_FT|PSR_DAT|PSR_IAT|PSR_IN|PSR_IT|PSR_U|PSR_IM,r15,r15
        or      PSR_IT,r15,r15  // set a trap bit to cause return from trap
        st.c    r15,psr
	ld.l    utr15(r0),r15		/* restore r15 */
	bri	r31
	  ld.l	utr14(r0),r31		/* restore r31 */
	//return to user!!

	/*
	 * NOT an emulated syscall.
	 */
.no_tramp:

#endif	FAST_EMUL_SYSCALL_DETECT

	ld.l	utsp(r0),r28	// saved sp  (r0 relative)
	ld.l	utr13(r0),r29	// saved r13 (r0 relative)
	ld.l	utr14(r0),r30	// saved r14 (r0 relative)
	ld.l	utr15(r0),r31	// saved r15 (r0 relative)
	st.l	r28,8(r13)	// save sp at trap entry
	st.l	r29,52(r13)	// save r13 at trap entry
	st.l	r30,56(r13)	// save r14 at trap entry
	st.l	r31,60(r13)	// save r15 at trap entry

	/*
	 * set 'sp' to to of current_thread()->kernel_stack
	 */
#if	NCPUS > 1
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r31,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_KSTACK(r31),sp	/* base (low adrs) of kernel stack */
	addu	KSTACK_SIZE,sp,sp	/* compute top of kstack */
	br	save_state
	  andnot 0xf,sp,sp		/* 16-byte align my kstack */

	/* NOT REACHED */

	//
	// kernel-to-kernel traps:
	//
was_kmode:
	andnot  0xf,sp,sp
	addu	-K_FRAME_SIZE,sp,sp	// sp  == stay on same stack
	mov	sp,r13			// r13 == where saved state will go

	st.l	r28,112(r13)	// save r28 (so it can be used)
	st.l	r29,116(r13)	// save r29 (so it can be used)
	st.l	r30,120(r13)	// save r30 (so it can be used)
	st.l	r31,124(r13)	// save r31 (so it can be used)
	ld.l	utsp(r0),r28	// saved sp  (r0 relative)
	ld.l	utr13(r0),r29	// saved r13 (r0 relative)
	ld.l	utr14(r0),r30	// saved r14 (r0 relative)
	ld.l	utr15(r0),r31	// saved r15 (r0 relative)
	st.l	r28,8(r13)	// save sp at trap entry
	st.l	r29,52(r13)	// save r13 at trap entry
	st.l	r30,56(r13)	// save r14 at trap entry
	st.l	r31,60(r13)	// save r15 at trap entry

#endif	/* i860XP */

save_state:
	// at this juncture we have:
	//
	//	p1..p3	available as long as we're at sploff
	//	sp	on interrupt stack or kernel stack
	//	r13	points to kernel stack or current pcb
	//	r14	fir at trap entry
	//	r15	psr at trap entry
	//	r28,r30,r31 available for use
	//	r29	syscall shortcut boolean
	//

	st.l	r14,ufir(r13)	// save fir
	st.l	r15,upsr(r13)	// save psr

	st.l	r0,0(r13)
	st.l	r1,4(r13)
	//st.l	sp,8(r13)	// already saved
	st.l	fp,12(r13)
	st.l	r4,16(r13)
	st.l	r5,20(r13)
	st.l	r6,24(r13)
	st.l	r7,28(r13)
	st.l	r8,32(r13)
	st.l	r9,36(r13)
	st.l	r10,40(r13)
	st.l	r11,44(r13)
	st.l	r12,48(r13)

	//st.l	r13,52(r13)	// already saved
	//st.l	r14,56(r13)	// already saved
	//st.l	r15,60(r13)	// already saved
	//st.l	r28,112(r13)	// already saved
	//st.l	r29,116(r13)	// already saved
	//st.l	r30,120(r13)	// already saved
	//st.l	r31,124(r13)	// already saved


	//
	// can't have fp traps enabled while saving fp regs
	//
disable_fp_traps:
	ld.c	fsr,r28		// save third stage result status
	st.l	r28,ufsr(r13)	// in the save area (pcb or on kernel stack)
	andnot	FSR_FTE,r28,r28	// clear FTE bit
	st.c	r28,fsr		// disable FP traps

	//
	// save control registers
	//
save_cntrl_regs:
	ld.c	db,r28		// save db
	st.l	r28,udb(r13)
	st.c	r0,db		// clear db
	.align	16		// B1/B2/B3 errata #25 align
	ixfr	r0,f0		// B1/B2/B3 errata #25 and sync
	ld.c	epsr,r28	// save espr
	st.l	r28,uepsr(r13)
	andnot	0x2000,r28,r28	// clear the IL bit
	st.c	r28,epsr
	ld.c	dirbase,r28	// save dirbase
	st.l	r28,udirbase(r13)

	//
	// save all floating point registers
	//
save_fp_regs:
	fst.q	f0,ufltreg(r13)
	fst.q	f4,ufltreg+(4*4)(r13)

#if 	SCALL_SHORTCUT
/*
 * Mach system call shortcut
 *
 * minimal saved registers at this point. Let the compiler save the
 * appro. registers according to "C" calling conventions.
 *
 */
	// at this juncture we have:
	//
	//	p1..p3	available while we still have interrupts off
	//	r1	saved
	//	sp	saved, on interrupt stack or kernel stack
	//	fp	saved
	//	r4-r12	saved
	//	r13	saved, points to kernel stack or current pcb
	//	r14	saved, fir at trap entry
	//	r15	saved, psr at trap entry
	//	r16..27	NOT saved, valid user registers
	//	r28	saved, scratch
	//	r29	saved, boolean syscall shortcut flag
	//			 0 : no syscall
	//			 1 : take syscall trap
	//			 2 : take FP result error
	//	r30	saved, scratch
	//	r31	saved, scratch
	//	f0..f7	saved
	//	f8..f31	NOT saved, valid user registers
	//	db	saved
	//	dirbase	saved
	//	epsr	saved
	//	fir	saved
	//	psr	saved
	//

	btne	1,r29,full_metal_jacket	// are we doing syscall shortcut?

	// yes - Mach syscall shortcut

	ld.l	124(r13),r29		// get syscall number from r31
	subs	r0,r29,r29		// get id right

	//	need to check syscall range
	//	if out of range, force to set it 0
	bc.t	call_func		// if r29<, syscode forced to be zero

	 mov	r0,r29

	orh	ha%_mach_trap_count,r0,r31
	ld.l	l%_mach_trap_count(r31),r31
	subs	r31,r29,r0		// if r29>r31
	bc.t	call_func		//	syscode is forced to be zero
	 mov	r0,r29

call_func:
	shl	4,r29,r29		// manual indexing
	// get address for mach_trap_table
	orh	h%_mach_trap_table,r0,r30
	or	l%_mach_trap_table,r30,r30
	addu	r29,r30,r29		// get mach_trap_table entry

	// Blocking syscall? return to normal heavyweight syscall path.
	ld.l	8(r29),r30		// get mach_trap_stack boolean 
	btne	r0,r30,full_metal_jacket


	ld.l	4(r29),r30		// get address for syscall_func
	ld.c	psr,r31	
	or	PSR_IM,r31,r31		// interrupt enabled
#if	!ASMP
	calli	r30			// call a corresponding syscall_func
	 st.c	r31,psr			// splon
#else	ASMP
	call	_baton_enter_syscall	// uses r6-r10 only
	 st.c	r31,psr			// splon

	calli	r30			// call a corresponding syscall_func
	 nop
#endif	ASMP

	//	returned from syscall

	st.c	r0,psr			// sploff

	// place retvalue to r16
	// (r13 was preserved by C calling conventions across the syscall)

	st.l	r16, 64(r13)

#if	ASMP
	//	volatiles have been consumed by syscall and are now available.
	call	_baton_exit
	 nop
#endif	ASMP

	ld.l	ufir(r13), r28		
	addu	4,r28,r28		
	st.l	r28,ufir(r13)		// advance pc

//	ld.l	upsr(r13),r15	// safer
//	ld.l	ufir(r13),r14	// safer

#if	NCPUS > 1
	FAST_CPU_NUMBER(r28)
	shl	2,r28,r28
	orh	ha%_need_ast,r28,r28
	ld.l	l%_need_ast(r28),r28	// r28 = need_ast[cpu_number]
#else	/* NCPUS > 1 */
	orh	ha%_need_ast,r0,r28
	ld.l	l%_need_ast(r28),r28	// r28 = need_ast[cpu_number]
#endif	/* NCPUS > 1 */
	btne	0,r28,call_astcheck

	br	restore_scall
	 ld.l	ufsr(r13),r16		// get fsr for restoring 

	// NOT REACHED

call_astcheck:
#if	NCPUS > 1
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r31,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_KSTACK(r31),sp	/* base (low adrs) of kernel stack */
	addu	KSTACK_SIZE,sp,sp	/* compute top of stack */
	call	_i860_call_asttaken
	 andnot  0xf,sp,sp		/* 16-byte align my kstack */

full_metal_jacket:
#endif 	SCALL_SHORTCUT
	
	// save volatile registers: r16...r27 & f8...f28
	st.l	r16,64(r13)
	st.l	r17,68(r13)
	st.l	r18,72(r13)
	st.l	r19,76(r13)
	st.l	r20,80(r13)
	st.l	r21,84(r13)
	st.l	r22,88(r13)
	st.l	r23,92(r13)
	st.l	r24,96(r13)
	st.l	r25,100(r13)
	st.l	r26,104(r13)
	st.l	r27,108(r13)

	fst.q	f8,ufltreg+(8*4)(r13)
	fst.q	f12,ufltreg+(12*4)(r13)
	fst.q	f16,ufltreg+(16*4)(r13)
	fst.q	f20,ufltreg+(20*4)(r13)
	fst.q	f24,ufltreg+(24*4)(r13)
	fst.q	f28,ufltreg+(28*4)(r13)

	//
	// we could skip this if we *know* that we aren't a B2/B3/C1 step
	//

#if	!defined(i860XP)
//	B2/B3/C1 errata #49 -- dual instuction DAT and pipe->scalar floating
//      ***********************************************************************
//	**	Rev 1.02		10/31/90
//	**
//	** This should be inserted immediately after saving all states, but
//	** prior to emptying or restoring any pipes. (PFLD pipe exempt)
//	**
//	** The assumptions are that all the registers are available for use,
//	** and that:
//	**
//	**	r14 		-	contains the contents of FIR
//	**  	r15 	 	-	contains the contents of PSR
//	**	r5,r6,r7,r8 	- 	use as temporaries
//      **
//      **  NOTE: The ld.l's that read the trapping instruction may be cacheable
//      **        If this may cause a problem, then pflds and an fxfr may be
//      **        used to avoid modifying the data cache.
//	***********************************************************************
erratum_49:
        and	l%0x1f00,r15,r6	// check if any trap bit is set
	mov	r14,r5		// copy FIR
	xor	l%0x800,r6,r0	// see that DAT is only trap
        bnc 	norm_trp 
        and	l%0x6000,r15,r0	// check if DIM or DS is set
        bc	norm_trp
	ld.l	l%0x0(r5),r7	// get trapped instruction
	andh	l%0xfc00,r7,r6
	xorh	l%0x4800,r6,r0	// trapped instruction a floating pt?
	bnc	cont_trp1

//	Now that we've established that we have a dual mode DAT, and
//	the trapping instruction is a floating point.  We need to check to see
//	if the instruction preceding the trap is a delay branch instruction.
//	If so, we want to skip this workaround.

	or	l%0x4,r0,r6
	subu	r5,r6,r8
	ld.l	l%0x0(r8),r7	// get previous instruction
        andh	l%0xf800,r7,r6
        xorh	l%0x6800,r6,r0	// BR or CALL ?
	bc	norm_trp
	andh	l%0xf400,r7,r6
	xorh	l%0x7400,r6,r0	// BC.T or BNC.T ?
	bc	norm_trp
	andh	l%0xfc00,r7,r6	
	xorh	l%0x4000,r6,r0	// BRI ?
	bc	norm_trp
	xorh	l%0xb400,r6,r0	// BLA ?
	bc	norm_trp
	xorh	l%0x4c00,r6,r0	// core escape instruction ?
	bnc	cont_trp0
	and	l%0x1f,r7,r6
	xor	l%0x2,r6,r0	// CALLI ?
	bc	norm_trp
cont_trp0:
	addu	l%0x4,r5,r5	// if trap addr. contains floating point
				// opcode, increment address pointer.
cont_trp1:
        and	l%0x1c,r5,r6	// check instruction that caused trap
        xor	l%0x1c,r6,r0	// is it at page or cache boundary?
        bc	norm_trp	// if so, skip workaround
	ld.l	l%0x0(r5),r7	// get trapped instruction
	andh	l%0xe800,r7,r0	// in addition, proceed only if it's LD.x
	bnc	norm_trp	// if not, skip workaround

//	Once we get here, we have to check the instruction
//	following the trapped instruction.  Skip this workaround for those
//	floating point instructions which won't cause this bug.

        ld.l	l%0x4(r5),r7 	// get the instruction following trap
        andh	l%0xfc00,r7,r6
        xorh	l%0x4800,r6,r0	// is it a floating point instr.?
        bnc     norm_trp
        and     l%0x7f,r7,r6
        xor     l%0x40,r6,r0	// FXFR?
        bc      norm_trp
        and     l%0x400,r7,r6	// pipelined FP instruction?
        bnc     norm_trp    
        and     l%0x70,r7,r6
	bte	r6,r0,norm_trp 	// pfmam instruction?
        xor     l%0x10,r6,r0   	// pfmsm instruction?
        bc      norm_trp

//	When we get to this part, we should be left with only scalar
//	A, M, or G unit floating point instructions.  Then we must execute
//	one pipeline instruction for the corresponding scalar instruction.

	and	l%0x70,r7,r6
        xor     l%0x20,r6,r0   	// is it M unit scalar?
        bc.t    norm_trp 
        pfmul.ss f0,f0,f0
        xor     l%0x30,r6,r0    // is A unit scalar instruction?
        bc.t    norm_trp
        pfadd.ss f0,f0,f0
        pfiadd.ss f0,f0,f0	// otherwise, must be G unit scalar.

//
//      ** after this point, we have the normal trap processing **
//
//	B2/B3/C1 errata #49 -- dual instuction DAT and pipe->scalar floating
//
#endif	/* !defined(i860XP) */
norm_trp:

	//
	// When we tune, we could get a little smarter here and
	// avoid the pipeline saves for certain traps...
	//

	//
	// save the pipes -- collect the whole set!
	//
save_pipes:
	// Save third, second and first stage results
	orh	ha%DoubleOne,r0,r31
	fld.d	l%DoubleOne(r31),f4	// set doubleprecision 1.0
	pfmul.ss f0,f0,f16	// save third stage M result
	fst.d	f16,um3(r13)	//
	pfadd.ss f0,f0,f18	// save third stage A result
	fst.d	f18,ua3(r13)	//
	pfld.d	udummy(r0),f20	// save third stage pfld result
#ifdef	i860XP
	fst.q	f20,ul3(r13)	// do it in quads on XP
#else	/* i860XP */
	fst.d	f20,ul3(r13)	// do it in doubles on XR
#endif	/* i860XP */
	ld.c	fsr,r27		// save second stage result status
	st.l	r27,ufsr2(r13)	//   in proc_state.
	pfmul.ss f0,f0,f16	// save second stage M result
	fst.d	f16,um2(r13)	//
	pfadd.ss f0,f0,f18	// save second stage A result
	fst.d	f18,ua2(r13)	//
	pfld.d	udummy(r0),f20	// save second stage pfld result
#ifdef	i860XP
	fst.q	f20,ul2(r13)	// do it in quads on XP
#else	/* i860XP */
	fst.d	f20,ul2(r13)	// do it in doubles on XR
#endif	/* i860XP */
	ld.c	fsr,r27		// save first stage result status
	st.l	r27,ufsr1(r13)	//   in proc_state.
	pfmul.ss f0,f0,f16	// save first stage M result
	fst.d	f16,um1(r13)	//
	pfadd.ss f0,f0,f18	// save first stage A result
	fst.d	f18,ua1(r13)	//
	pfld.d	udummy(r0),f20	// save first stage pfld result
#ifdef	i860XP
	fst.q	f20,ul1(r13)	// do it in quads on XP
#else	/* i860XP */
	fst.d	f20,ul1(r13)	// do it in doubles on XR
#endif	/* i860XP */
	pfiadd.dd f0,f0,f16
	fst.d	f16,ui1(r13)	//

	// save KR, KI, T and MERGE

// documentation change #27 to -002 PRM -- keep -0.0 in t from rounding to +0.0
	andnot	0x2c,r27,r31	// Clear RM, clear FTE
	or	4,r31,r31	// Set RM=01 to dound down so -0 preserved when
	st.c	r31,fsr		// added to f0
	r2p1.dd	f0,f4,f0	// M first stage contains KR
	ld.c	fsr,r27		// A first stage contains T 
	i2p1.dd	f0,f4,f0	// M first stage contains KI
	ld.c	fsr, r28
save_kr:
	pfmul.dd	f0,f0,f16	// save KR register
	and		0x100,r27,r0	// cc set if no source error
	bc.t		save_ki
	fst.d		f16,ukr(r13)	
kr_src_err:
	fxfr		f17,r27		// high 32 bits of KR
	andh		0x7ff0,r27,r0	// cc set if exp-bits are all 0's
	bc.t		save_ki
	fst.d		f16,ukr(r13)
	orh		0x7ff0,r27,r27
	ixfr		r27,f17
	fst.d		f16,ukr(r13)
save_ki:
	pfmul.dd	f0,f0,f16
	and		0x100,r28,r0
	bc.t		save_t
	fst.d		f16,uki(r13)	
ki_src_err:
	fxfr		f17,r28
	andh		0x7ff0,r28,r0
	bc.t		save_t
	fst.d		f16,uki(r13)
	orh		0x7ff0,r28,r28
	ixfr		r28,f17
	fst.d		f16,uki(r13)
save_t:
	m12tpm.dd	f0,f0,f0
	ld.c		fsr,r27
	m12tsm.dd	f0,f0,f0
	pfadd.dd	f0,f0,f0	// adder third stage gets T
	pfadd.dd	f0,f0,f16	// save T register
	pfadd.dd	f0,f0,f18	

	and		0x100,r27,r0
	bc.t		save_merge	
	fst.d		f16,ut(r13)

	fxfr		f17,r27
	andh		0x8000,r27,r0
	bc		t_src_err
	fiadd.dd	f18,f0,f16
	fxfr		f17,r27
t_src_err:
	andh		0x7ff0,r27,r0
	bc.t		save_merge
	fst.d		f16,ut(r13)
	orh		0x7ff0,r27,r27
	ixfr		r27,f17
	fst.d		f16,ut(r13)
save_merge:
	form	f0,f16		// save MERGE register
	fst.d	f16,umerge(r13)

#if	!defined(i860XP)
//
// B2/B3 errata #37: If the FZ is set in the FSR,
//                   clear MU and AU bits in the pipes.
// NX trap.s version 10.7 [ Wed Jul  8 17:05:26 PDT 1992 ] stan
//
	ld.l	ufsr(r13),r16
	and	0x0001,r16,r0    // Check FZ bit in FSR
	bc	err_37_done	// taken if FZ bit clear
	ld.l	ufsr2(r13),r28
	ld.l	ufsr1(r13),r29
	andnot	0x2200,r16,r16  // clear MU and AU bits of FSR
	st.l	r16,ufsr(r13)
	andnot	0x2200,r28,r28  // clear MU and AU bits of FSR2
	st.l	r28,ufsr2(r13)
	andnot	0x2200,r29,r29  // clear MU and AU bits of FSR1
	st.l	r29,ufsr1(r13)
err_37_done:
//
// end errata #37

//
// B2/B3 errata #30 workaround -- double check fsr if FT is not set
//
//  *** WARNING *** r16 contains ufsr(r13) from errata #37
//
eratum_30:
	//ld.l	upsr(r13),r15
	and	PSR_FT,r15,r0	// did FT occur?
	bnc	post_wa_30	// yes.
//	ld.l	ufsr(r13),r16	already loaded in err #37 above.
	and	0x0020,r16,r0	// FTE
	bc	post_wa_30	// taken if no exceptions enabled
	and	0xEF00,r16,r0	// AI || AO || AU || MI || MO || MU || SE
	bc	post_wa_30	// taken if no FP exceptions pending
	and	0x6700,r16,r0	// AO || AU || MO || MU || SE
	bnc	wa_30		// taken if not inexact
	and	0x0002,r16,r0	// TI
	bc	post_wa_30	// taken if inexact not enabled
	and	0x8800,r16,r0	// AI || MI
	bc	post_wa_30	// taken if no inexact exceptions pending
wa_30:
	or	PSR_FT,r15,r15	// coerce an fp trap
	st.l	r15,upsr(r13)
post_wa_30:
//
// end - B2/B3 errata #30 workaround -- double check fsr if FT is not set
//
#endif	/* !defined(i860XP) */


save_complete:

//
// Congratulations!
//
// You are now the proud owner of a struct i860_saved_state.
// Take good care of it...you'll need it to get out of here later.
//
	//
	// call ctrap() with a pointer to the saved state information.
	//
	// The following registers *should* be preserved by the
	// defined calling conventions:
	//	r13 == pointer to saved state area
	//	r14 == fir at time of trap
	//	r15 == psr at time of trap
	//
	call	_ctrap
	 mov	r13,r16

	and	PSR_PU,r15,r0
	bc	0f
	call	_ctrap_safetynet
	 mov	r13,r16
0:

	//
	// prepare to return from the trap.
	//
	//	r11 is used by return_from_trap:
	//	"1 in r11 indicates that upon return from trap,
	//	 if a delayed control transfer was encountered,
	//	 then the control transfer instruction is to be emulated
	//	 and not reexecuted."
	//
	//	r13 needs to be set to point at the saved state area.
	//	r14 needs to be set to the fir at the time of the trap.
	//	r15 needs to be set to the psr at the time of the trap.
	//	r18 is used in return_from_trap() as the instr. @ fir-4.
	//
prepare_to_return:
_continue_return_from_trap::
	ld.l	ufir(r13),r14	// safer
	ld.l	upsr(r13),r15	// safer
	ld.l	ur15(r13),r11	// did we emulated an instruction

/*
 *	[alan@osf.org]
 *	Before doing anything else -- like assuming that
 *	user-space instructions exist -- check for the
 *	presence of ASTs.
 *
 */
	and	PSR_PU,r15,r0
	bc	no_ast2
	call	_i860_astcheck
	 mov	r15,r16
no_ast2:


/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/
/*%%%% h  o  m  e     f  r  e  e %%%%*/
/*%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

return_from_trap:

	// check instruction at fir - 4
	// if a delayed control instruction, resume execution
	//    at that instruction.
	// if a BLA instruction, subtract src1 from src2.
	// if r11 equals 1 then a control transfer instruction
	//    is to be emulated and not reexecuted.

	st.c	r0,psr			// interrupts are disabled now!!
	ld.l	-4(r14),r18		// kernel mode: instruction at fir - 4

	// check BRI instruction

	andh	0xfc00,r18,r22	// mask for reg format opcodes
	orh	0x4000,r0,r21	// opcode of BRI
	btne r21,r22,check_br_and_call    // taken if not BRI

	// check if we should skip instruction at FIR
	and	1,r11,r0
	bc	resume_at_fir_minus_4 // we should not skip inst at FIR
	// we now must work out target of bri and place it in ufir(r13)
	shr	9,r18,r27
	and	0x7c,r27,r27
	ld.l	r27(r13),r27
	br	check_if_to_set_KNF
	 st.l	r27,ufir(r13)
    
	// check BR & CALL instructions

check_br_and_call::
	andh	0xf800,r18,r22	// mask for ctrl format opcode
	orh	0x6800,r0,r21	// opcode of BR or CALL
	btne	r21,r22,check_calli // taken if not (BR or CALL)

	// check if we should skip instruction at FIR
	and	1,r11,r0
	bc	resume_at_fir_minus_4 // we should not skip inst at FIR
	// we now must work out target of call or br and place it in ufir(r13)
	shl	6,r18,r20   
	shra	4,r20,r20
	addu	r20,r14,r20 // r14 contains the fir at trap point
	br	check_if_to_set_KNF
	 st.l	r20,ufir(r13)

	// check CALLI instruction

check_calli::
	and	0xfc00001f,r18,r22	// mask for core escape format opcode
	mov	0x4c000002,r21	// opcode of CALLI
	btne	r21,r22,check_bc_bnc_dot_t    // taken if not CALLI

	// check if we should skip instruction at FIR
	and	1,r11,r0
	bc	resume_at_fir_minus_4        // we should not skip inst at FIR
	// we now must work out target of calli and place it in ufir(r13)
	shr	9,r18,r27
	and	0x7c,r27,r27
	ld.l	r27(r13),r27
	br	check_if_to_set_KNF
	 st.l	r27,ufir(r13)    // done
    
	// check BC.T & BNC.T instructions

check_bc_bnc_dot_t::
	andh	0xf400,r18,r22	// mask for ctrl format opcode
	orh	0x7400,r0,r21	// opcode of BC.T or BNC.T
	btne	r21,r22,check_BLA // taken if not (BC.T or BNC.T)

	// check if we should skip instruction at FIR
	and	1,r11,r0
	bnc	emulate_bc_t	// we should emulate inst at FIR
    
	// check for a special case of BC.T or BNC.T where:
	// 1. a source exception has occured.
	// 2. the instruction at fir is PFGT, PFLE or PFEQ

check_for_ft::
	and	0x1000,r15,r0	// did FT occure?
	bc	resume_at_fir_minus_4	// taken if no FT

	ld.l	ufsr(r13),r19	// did SE occure?
	and	0x100,r19,r0	// mask SE bit in fsr
	bc	resume_at_fir_minus_4	// taken if no SE

	ld.l	0(r14),r20	// instruction at fir
	andh	0xfc00,r20,r22	// mask for fp instruction
	orh	0x4800,r0,r21	// code of fp inst.
	btne	r21,r22,resume_at_fir_minus_4 // taken if not a fp instruction.

	and	0x3f,r20,r22	// mask for PFGT (and PFLE)
	or	0x34,r0,r21	// code for PFGT
	bte	r21,r22,emulate_bc_t	// taken if inst is PFGT

	or	0x35,r0,r21	// code for PFEQ
	btne	r21,r22,resume_at_fir_minus_4	// taken if not PFEQ

emulate_bc_t::
	// emulate the bc.t (bnc.t) instruction

	shl	6,r18,r18	// calc the destination of bc.t (bnc.t)
	shra	4,r18,r18
	addu	r18,r14,r18	// r14 contains the fir at trap point
	br	check_if_to_set_KNF
	 st.l	r18,ufir(r13)

check_BLA:
	// check BLA instruction

	andh	0xfc00,r18,r22	// mask for reg format opcode
	orh	0xb400,r0,r21	// opcode of BLA
	btne	r21,r22,not_in_control_transfer_sequence

	// instruction at fir - 4 is BLA

	// check if we should skip instruction at FIR
	and	1,r11,r0
	bc	undo_bla	// we should not skip inst at FIR

	// find destination of bla and put it in ufir(r13)
_bla_lcc::
	andh	0x03e0,r18,r26	// get value in src2 register
	shr	19,r26,r26
	ld.l	r26(r13),r28
	and	0xf800,r18,r27	// get value in src1 register
	shr	9,r27,r27
	ld.l	r27(r13),r29
	subs	r28,r29,r28	// pre-bla state
	adds	r29,r28,r0	// check lcc value
	ld.l	upsr(r13),r30	// get psr value
	bnc.t	_update_lcc
	 or	8,r30,r31	// set lcc
	andnot	8,r30,r31	// clear lcc
_update_lcc::
	st.l	r31,upsr(r13)	// update lcc in psr

	and	8,r30,r0
	bc	_lcc_clr
_lcc_set::
	andh	0x1f,r18,r30
	and	0x7ff,r18,r31
	shl	11,r30,r30
	shl	16,r31,r31
	or	r30,r31,r31
	shra	14,r31,r31	// we now have offset in  r31
	addu	r31,r14,r31
	br	check_if_to_set_KNF
	 st.l	r31,ufir(r13)
_lcc_clr::
	addu	4,r13,r31	// bla falls through to fir+4
	br	check_if_to_set_KNF
	 st.l	r31,ufir(r13)
//rag--end
    
undo_bla::
	andh	0x03e0,r18,r26	// get value in src2 register
	shr	19,r26,r26
	ld.l	r26(r13),r28

	and	0xf800,r18,r27	// get value in src1 register
	shr	9,r27,r27
	ld.l	r27(r13),r29

	xor	r26,r27,r0	// check if src1=src2 (reg # are actually x4)
	bnc.t	undo_bla1	// taken if src1 != src2
	 subs	r28,r29,r28	// subtract src1 from src2 and
	shra	1,r28,r28	// divide src2 by 2
undo_bla1::
	adds	r26,r13,r26	// ????
	st.l	r28,0(r26)	// set the result in return frame

	// resume execution at fir - 4

resume_at_fir_minus_4:
	ld.l	ufir(r13),r18	// execution should be resumed at
	adds	-4,r18,r18	// fir - 4
	st.l	r18,ufir(r13)
	
	// if CPU was in DIM:
	// 1. set DS and clear DIM

	and	0x4000,r15,r0	// test DIM bit in psr
	bc	check_if_to_set_KNF	// taken if cpu was not in DIM
	ld.l	upsr(r13),r18	//
	andnot	0x4000,r18,r18	// clear DIM in return psr
	or	0x2000,r18,r18	// set DS in return psr
	br	check_if_to_set_KNF
	 st.l	r18,upsr(r13)

not_in_control_transfer_sequence::
	and	1,r11,r0
	bc	after_not_ct

	// Reached here if data alignment trap was emulated.
	// return address & action are according to DIM & DS
	// DIM DS  action
	//  0   0  return to FIR+4
	//  0   1  return to FIR+4, set DIM, clear DS
	//  1   0  return to FIR+8
	//  1   1  return to FIR+8, clear DIM & DS

	shr	13,r15,r18
	and	3,r18,r18	// isolate DIM & DS
	bte	0,r18,not_ct_case_0
	bte	2,r18,not_ct_case_2
	bte	3,r18,not_ct_case_3

not_ct_case_1:
	ld.l	upsr(r13),r19
	andnot	0x2000,r19,r19
	or	0x4000,r19,r19
	st.l	r19,upsr(r13)

not_ct_case_0:
	ld.l	ufir(r13),r19	// increment FIR by 4
	addu	4,r19,r19
	br	after_not_ct
	 st.l	r19,ufir(r13)

not_ct_case_3:
	ld.l	upsr(r13),r19
	andnot	0x6000,r19,r19
	st.l	r19,upsr(r13)

not_ct_case_2:
	ld.l	ufir(r13),r19	// increment FIR by 8
	addu	8,r19,r19
	st.l	r19,ufir(r13)

after_not_ct:
// *BL* - destination addr of next inst changed to implement i860 work around
//	br	i860_work_around
//	 nop

check_if_to_set_KNF::

	// set KNF bit in psr if:
	// 1. all following 4 conditions occur -
	//    1) the trap was caused in DIM
	//    2) the trapped instruction is a fp instruction
	//    3) only DAT occured with no other traps
	//    4) the trapped instruction is not FXFR (A-STEP silicon only).
	// 2.
	//    1) the trap was caused by a source exception of any fp
	//       instruction (except when a PFGT, PFLE or PFEQ follows
	//       a conditional branch instruction).
	//
	// The KNF bit is reset if bit 0 of r11 is set since the
	// return is not to the point of trap but after it. The
	// instructions at the point of trap were emulated.
	//

	// reset the KNF bit in the PSR if bit 0 of r11 is set

	and	1,r11,r0
	bc	set_KNF1
	ld.l	upsr(r13),r27
	andnot	0x8000,r27,r27
	br	next_to_setting_KNF
	 st.l	r27,upsr(r13)
    
set_KNF1::
	and	0x4000,r15,r0	// check if in DIM
	bc	next_to_setting_KNF // taken if not in DIM

	orh	0x4800,r0,r19	// r19 - code of fp inst.
	ld.l	0(r14),r17	// r17 - faulting instruction
	andh	0xfc00,r17,r18	// 6 MSB distinguish fp from other inst.
	btne	r18,r19,next_to_setting_KNF	// taken in inst is not fp

	and	0x1700,r15,r0	// check trap bits other then DAT
	bnc	next_to_setting_KNF	// taken if traps other then DAT occured

	// upsr(r13) might have been modified by debug() routine to clear
	// BW & BR so we have to work on it, not on r15.

	ld.l	upsr(r13),r18
	or	0x8000,r18,r18	// set KNF bit in psr 
	st.l	r18,upsr(r13)	// save modified psr

	//
	// almost home...
	//
next_to_setting_KNF::
restore_pipes::
_bootstrap_return_from_trap::

	ld.l	ufir(r13),r14	// safer
	ld.l	upsr(r13),r15	// safer

#if	PARAGON860 && CTRAP_HISTORY
	//	XXX see i860/ctrap.c
	call	_ctrap_log_exit
	mov	r13,r16
#endif	PARAGON860

	//
	// if returning to user-mode, check for ast's.
	//
check_ast:
	and	PSR_PU,r15,r0
	bc	no_ast
	call	_i860_astcheck
	mov	r15,r16
	st.c	r0,psr
	btne	r0,r16,_continue_return_from_trap
no_ast:

	//
	// if previously in a locked sequence,
	// restart at the lock instruction
	//
	ld.l	uepsr(r13),r17	// epsr at trap point
	and	EPSR_IL,r17,r0	// is IL set?
	bc	not_locked
	call	_i860_restart_lock
	 mov	r13,r16
	st.c	r0,psr
not_locked:
#if	ASMP && BATON_DEBUG
	and	PSR_PU,r15,r0	// returning to user mode?
	bc	0f
	call	_baton_assert_not_owner
	nop
0:
#endif	/* ASMP && BATON_DEBUG */

	//
	// below this point, we are committed to returning from
	// the exception.
	//
_REAL_trap_return::

	fld.l	SingleOne,f30	// get single precision 1.0
	fld.d	DoubleOne,f4	// get double precision 1.0
	fld.d	MinusZero,f6	// get double (single too) precision -0.0
	st.c	r0,fsr		// clear FTE

	// restore KR, KI and T
	fld.d	ut(r13),f16	// load T from proc_state
	fld.d	ukr(r13),f18	// load KR from proc_state
	fld.d	uki(r13),f20	// load KR from proc_state
	pfmul.dd f4,f16,f0	// put value of T in M 1'st stage

	r2pt.dd		f18,f0,f0	// load KR, advance T
	i2apt.dd	f20,f0,f0	// load KI and T
	// restore MERGE

	ld.l	umerge(r13),r16	// load MERGER low from proc_state.
	ld.l	umerge+4(r13),r18 // load MERGER high from proc_state.
	shl	16,r16,r20	// move low 16 bits to high 16.
	ixfr	r20,f10
	shl	16,r18,r20	// move low 16 bits to high 16.
	ixfr	r20,f11
	ixfr	r16,f8
	ixfr	r18,f9
	faddz	f0,f10,f0	// merge low 16s
	faddz	f0,f8,f0	// merge high 16s

	// load stages values into registers

	fld.d	um3(r13),f16
	fld.d	ua3(r13),f18
	fld.d	um2(r13),f20
	fld.d	ua2(r13),f22
	fld.d	um1(r13),f24
	fld.d	ua1(r13),f26
	fld.d	ui1(r13),f28
	ld.l	ufsr(r13),r16
	ld.l	ufsr2(r13),r17
	ld.l	ufsr1(r13),r18

	// restore 3'rd stage

// B2/B3 errata #39 for -denoral in adder pipes
	andh	0x2000,r16,r0	// test adder result precision ARP
	fxfr	f18,r30		// get low 32 bits of double or all of single
	bnc.t	d0		// taken if it was double
	 fxfr	f19,r31		// get high 32 bits of double
	andh	0x8000,r30,r0	// sign bit set?
	bc.t	l0		// no
	 pfamov.ss f18,f0	// insert +single result
	andnoth	0x8000,r30,r0	// -zero?
	bc.t	l0		// yes
	 pfamov.ss f18,f0	// insert -zero single result
	andh	0x7f80,r30,r0	// -denormal?
	bnc.t	l0		// no
	 pfamov.ss f18,f0	// insert -single result
	br	l0		// no
	 pfadd.ss f18,f7,f0	// insert -denormal single result
d0:	andh	0x8000,r31,r0	// sign bit set?
	bc.t	l0		// no
	 pfamov.dd f18,f0	// insert +double result
	andh	0x7ff0,r31,r0	// -denormal or -zero?
	bnc.t	l0		// no
	 pfamov.dd f18,f0	// insert -single result
	andnoth	0x8000,r31,r0	// -zero top part?
	bnc.t	l0		// no
	 pfadd.dd f18,f6,f0	// insert -denormal double result
	andnot	0x0000,r30,r0	// -zero bottom part?
	bc.t	l0		// yes
	 pfamov.dd f18,f0	// insert -zero double result
	pfadd.dd f18,f6,f0	// insert -denormal double result
l0:
	andh	0x400,r16,r0	// test load result precision LRP
#ifdef	i860XP
	bc.t	l1		// taken if it was single
	 pfld.l	ul3(r13),f0	// insert single result
	andh	0x200,r16,r0	// test load result precision LRP0 on XP
	bc.t   l1		// taken if it was double
	 pfld.d	ul3(r13),f0	// insert double result
	pfld.q	ul3(r13),f0	// insert quad result
#else	/* i860XP */
	bc.t	l1		// taken if it was single
	 pfld.l	ul3(r13),f0	// insert single result
	pfld.d	ul3(r13),f0	// insert double result
#endif	/* i860XP */
l1:
	andh	0x1000,r16,r0	// test multiplier result precision MRP
	bc.t	l2		// taken if it was single
	 pfmul.ss f16,f30,f0	// insert single result
	pfmul3.dd f16,f4,f0	// insert double result
l2:
	or	0x10,r16,r20	// set U (update) bit so that st.c
				//   will update status bits in pipeline
	andnot	0x20,r20,r20	// clear FTEbit so as not to cause traps
	st.c	r20,fsr		// update stage 3 result status

	// restore 2'nd stage

	andh	0x2000,r17,r0	// test adder result precision ARP
	fxfr	f22,r30		// get low 32 bits of double or all of single
	bnc.t	d3		// taken if it was double
	 fxfr	f23,r31		// get high 32 bits of double
	andh	0x8000,r30,r0	// sign bit set?
	bc.t	l3		// no
	 pfamov.ss f22,f0	// insert +single result
	andnoth	0x8000,r30,r0	// -zero?
	bc.t	l3		// yes
	 pfamov.ss f22,f0	// insert -zero single result
	andh	0x7f80,r30,r0	// -denormal?
	bnc.t	l3		// no
	 pfamov.ss f22,f0	// insert -single result
	br	l3		// no
	 pfadd.ss f22,f7,f0	// insert -denormal single result
d3:	andh	0x8000,r31,r0	// sign bit set?
	bc.t	l3		// no
	 pfamov.dd f22,f0	// insert +double result
	andh	0x7ff0,r31,r0	// -denormal or -zero?
	bnc.t	l3		// no
	 pfamov.dd f22,f0	// insert -single result
	andnoth	0x8000,r31,r0	// -zero top part?
	bnc.t	l3		// no
	 pfadd.dd f22,f6,f0	// insert -denormal result
	andnot	0x0000,r30,r0	// -zero bottom part?
	bc.t	l3		// yes
	 pfamov.dd f22,f0	// insert -zero double result
	pfadd.dd f22,f6,f0	// insert -denormal double result
l3:
	andh	0x400,r17,r0	// test load result precision LRP
#ifdef	i860XP
	bc.t	l4		// taken if it was single
	 pfld.l	ul2(r13),f0	// insert single result
	andh	0x200,r17,r0	// test load result precision LRP0 on XP
	bc.t   l4		// taken if it was double
	 pfld.d	ul2(r13),f0	// insert double result
	pfld.q	ul2(r13),f0	// insert quad result
#else	/* i860XP */
	bc.t	l4		// taken if it was single
	 pfld.l	ul2(r13),f0	// insert single result
	pfld.d	ul2(r13),f0	// insert double result
#endif	/* i860XP */
l4:
	or	0x10,r17,r20	// set U (update) bit
	andnot	0x20,r20,r20	// clear FTE
	andh	0x1000,r17,r0	// test multiplier result precision MRP
	bc.t	l5		// taken if it was single
	 pfmul.ss f20,f30,f0	// insert single result
	pfmul3.dd f20,f4,f0	// insert double result
l5:
	st.c	r20,fsr		// update stage 2 result status

	// restore 1'st stage

	andh	0x2000,r18,r0	// test adder result precision ARP
	fxfr	f26,r30		// get low 32 bits of double or all of single
	bnc.t	d6		// taken if it was double
	 fxfr	f27,r31		// get high 32 bits of double
	andh	0x8000,r30,r0	// sign bit set?
	bc.t	l6		// no
	 pfamov.ss f26,f0	// insert +single result
	andnoth	0x8000,r30,r0	// -zero?
	bc.t	l6		// yes
	 pfamov.ss f26,f0	// insert -zero single result
	andh	0x7f80,r30,r0	// -denormal?
	bnc.t	l6		// no
	 pfamov.ss f26,f0	// insert -single result
	br	l6		// no
	 pfadd.ss f26,f7,f0	// insert -single result
d6:	andh	0x8000,r31,r0	// sign bit set?
	bc.t	l6		// no
	 pfamov.dd f26,f0	// insert +double result
	andh	0x7ff0,r31,r0	// -denormal or -zero?
	bnc.t	l6		// no
	 pfamov.dd f26,f0	// insert -double result
	andnoth	0x8000,r31,r0	// -zero top part?
	bnc.t	l6		// no
	 pfadd.dd f26,f6,f0	// insert -denormal double result
	andnot	0x0000,r30,r0	// -zero bottom part?
	bc.t	l6		// no
	 pfamov.dd f26,f0	// insert -zero double result
	pfadd.dd f26,f6,f0	// insert -denormal double result
l6:

	andh	0x1000,r18,r0	// test multiplier result precision MRP
	bc.t	l7		// taken if it ws single
	 pfmul.ss f24,f30,f0	// insert single result
	pfmul3.dd f24,f4,f0	// insert double result
l7:

	andh	0x400,r18,r0	// test load result precision LRP
#ifdef	i860XP
	bc.t	l8		// taken if it was single
	 pfld.l	ul1(r13),f0	// insert single result
	andh	0x200,r18,r0	// test load result precision LRP0 on XP
	bc.t   l8		// taken if it was double
	 pfld.d	ul1(r13),f0	// insert double result
	pfld.q	ul1(r13),f0	// insert quad result
#else	/* i860XP */
	bc.t	l8		// taken if it ws single
	 pfld.l	ul1(r13),f0	// insert single result
	pfld.d	ul1(r13),f0	// insert double result
#endif	/* i860XP */
l8:
	andh	0x800,r18,r0	// test vector-integer result precision IRP
	bc.t	l9		// taken if it ws single
	 pfmov.ss f28,f0	// insert single result
	pfmov.dd f28,f0		// insert double result
l9:
	or	0x10,r18,r18	// set U (update) bit
	andnot	0x20,r18,r18	// clear FTE
	st.c	r18,fsr		// update stage 1 result status
// B2/B3 errata #39 for -denoral in adder pipes

restore_reg_start:
	ld.l	68(r13),r17
	ld.l	72(r13),r18
	ld.l	76(r13),r19
	ld.l	80(r13),r20
	ld.l	84(r13),r21
	ld.l	88(r13),r22
	ld.l	92(r13),r23
	ld.l	96(r13),r24
	ld.l	100(r13),r25
	ld.l	104(r13),r26
	ld.l	108(r13),r27
	ld.l	112(r13),r28
	ld.l	116(r13),r29
	ld.l	120(r13),r30
	ld.l	124(r13),r31	

	fld.q	ufltreg+(8*4)(r13),f8
	fld.q	ufltreg+(12*4)(r13),f12
	fld.q	ufltreg+(16*4)(r13),f16
	fld.q	ufltreg+(20*4)(r13),f20
	fld.q	ufltreg+(24*4)(r13),f24
	fld.q	ufltreg+(28*4)(r13),f28

restore_scall:
	fld.d	ufltreg+(4*2)(r13),f2
	fld.q	ufltreg+(4*4)(r13),f4
	st.c	r16,fsr		// restore nonpipelined FSR status

	// restore all integer registers

restore_int_regs::
	//ld.l	0(r13),r0	// never need this
	ld.l	4(r13),r1
	//ld.l	8(r13),sp	// not yet...
	ld.l	12(r13),fp
	ld.l	16(r13),r4
	ld.l	20(r13),r5
	ld.l	24(r13),r6
	ld.l	28(r13),r7
	ld.l	32(r13),r8
	ld.l	36(r13),r9
	ld.l	40(r13),r10
	ld.l	44(r13),r11
	ld.l	48(r13),r12
	//ld.l	52(r13),r13	// not yet, we still need it
	ld.l	56(r13),r14
	ld.l	60(r13),r15
//	ld.l	64(r13),r16	// used as a temporary

	ld.l	udb(r13),r16	// restore the db register
	st.c	r16,db
	ld.l	uepsr(r13),r16	// restore epsr at trap point
	st.c	r16,epsr

	//
	// use some discretion when restoring the psr...
	//
	// clear out all of the trap bits, stay in supervisor mode,
	// keep interrupts turned off.  turn on 1 trap bit to return
	// from the trap.
	// 
	st.c	r0,psr
	ld.l	upsr(r13),r16	// psr to be restored
	andnot	PSR_FT|PSR_DAT|PSR_IAT|PSR_IN|PSR_IT|PSR_U|PSR_IM,r16,r16
	or	PSR_IT,r16,r16	// set a trap bit to cause return from trap
	st.c	r16,psr

	ld.l	52(r13),r16	// move value of r13 at trap point
#ifdef	i860XP
	st.c	r16,p1		// restore r13 on our way out....
#else
	st.l	r16,utr13(r0)	// ...to page 0
#endif

	ld.l	8(r13),sp	// restore the sp
	ld.l	64(r13),r16	// restore r16

	ld.l	ufir(r13),r13	// load r13 with return address

_exit_trap::

#ifdef	i860XP

	bri	r13
	  ld.c	p1,r13		// restore r13 from cpu reg p1 on the way out

#else	/* i860XP */

	ld.l	utr13(r0),r0	// For B1/B2/B3 errata #10 load twice
	bri	r13
	  ld.l	utr13(r0),r13	// restore r13 from page 0 on the way out

#endif	/* i860XP */

#if	MACH_ASSERT
/*
 * write to specified LA trigger addrss, logic Analyzer printf()
 *
 *      la_trigger( adrs, value )
 */

_la_trigger::
	btne	r0,r16,lat.0
	orh	h%0x60300000,r0,r16
lat.0:
	stio.l	r17,r16
	bri	r1
	  nop

#endif	/* MACH_ASSERT */

#if	FASTTRAPS
	//
	//	branch here for all cases of instruction trap
	//
	//	r13 == *fir (the "trap" instruction)
	//	r14 == fir (at the time of the trap)
	//	r15 == psr (at the time of the trap)
	//	p0  == CPU number
	//	p1  == r13 (at the time of the trap)
	//	p2  == r14 (at the time of the trap)
	//	p3  == r15 (at the time of the trap)
	//
	//	Determine the syscall type (or breakpoint), and
	//	reject accordingly.
	//
	//	Don't worry about reloading *fir -- it's in
	//	the dcache by now.
	//
_fast_syscall::

#define	FAST_CALL	0x44008000	/* trap r16,r0,r0 */

	//
	//	the FAST_CALL trap was picked to make it easy
	//	to accept/reject the trap instruction (it's
	//	easy to flip the 0x4400 bits and the 0x8000 bit
	//	with xor and see if you're left with 0x00000000).
	//
	xorh	h%FAST_CALL,r13,r13
	xor	l%FAST_CALL,r13,r13
	btne	r0,r13,.not_so_fast

	//
	//	okay, it's a fast call.
	//

	//
	//	disable FP traps so we can safely use ixfr
	//
	ld.c	fsr,r13		// r13 = fsr at time of trap
	st.c	r0,fsr

	//	need to free up a few more registers for our use; can
	//	spill some iregs into the fregs that are considered
	//	volatile across function calls (and traps by convention).
	//
	ixfr	r28,f16		// overwrite w/ caller's r28
	ixfr	r29,f17		// overwrite w/ caller's r29
	ixfr	r30,f18		// overwrite w/ caller's r30
	ixfr	r31,f19		// overwrite w/ caller's r31

	//
	//	r31 = -r31	(code = -code)
	//
	//	if (r31 < 0) r31 = 0;
	//	else if (r31 >= mach_trap_count) r31 = 0;
	//
	subs	r0,r31,r31
	bc.t	.fastcall
	 or	r0,r0,r31		// force to 0 if less than 0.

	orh	ha%_mach_trap_count,r0,r28
	ld.l	l%_mach_trap_count(r28),r28
	subs	r28,r31,r0
	bc.t	.fastcall		// force to 0 if >= mach_trap_count
	 or	r0,r0,r31

.fastcall:
	//
	//	get a pointer to mach_trap_table[code]
	//
	//	XXX don't even think about changing the size of
	//	XXX mach_trap_t without looking here first.
	//
	orh	h%_mach_trap_table,r0,r28
	or	l%_mach_trap_table,r28,r28
	shl	4,r31,r29		// each entry is 16 bytes
	addu	r28,r29,r30		// r30 = &mach_trap_table[code];
	fxfr	f19,r31			// restore user's r31
	//
	//	r28 available
	//	r29 available
	//	r30 &mach_trap_table[ adjusted syscall code ]
	//	r31 original syscall code

	//
	//	Some functions, like task_by_pid(), are emulated but
	//	built in libmach so that they look like real Mach traps.
	//
	//	In order to support those kinds of calls without
	//	impacting the performance of non-emulated syscalls,
	//	a check is made here to see if the mach_trap_function
	//	field is the routine kern_invalid().
	//
	ld.l	4(r30),r30		// r30 = r30->mach_trap_function;
	orh	h%_kern_invalid,r0,r29
	or	l%_kern_invalid,r29,r29
	bte	r29,r30,.fastcall_abort

	//
	//	r29 = current_pcb[cpu_number()]
	//
#if	NCPUS > 1
	FAST_CPU_NUMBER(r29)
	shl	2,r29,r29	// convert cpu # to longword offset
	orh	ha%_current_pcb,r29,r29
	ld.l	l%_current_pcb(r29),r29
#else
	orh	ha%_current_pcb,r0,r29
	ld.l	l%_current_pcb(r29),r29
#endif

	//
	//	save the fsr, fir, psr and db in the pcb.
	//
	st.l	r13,ufsr(r29)	// fsr at time of trap
	st.l	r14,ufir(r29)	// fir at time of trap
	st.l	r15,upsr(r29)	// psr at time of trap
	ld.c    db,r28          // save db
	st.l    r28,udb(r29)

	//
	//	restore user's r13-r15, r28
	//
	ld.c	p1,r13		// restore user's saved r13
	ld.c	p2,r14		// restore user's saved r14
	ld.c	p3,r15		// restore user's saved r15
	fxfr	f16,r28		// restore r28 to user's r28

	//
	//	store the non-volatile FP regs and the non-volatile
	//	iregs that will be staged in the FP reg set.
	//
	//	the ireg staging is interleaved with the stores
	//	to avoid stalling due to a full write-buffer
	//	and to fill the 2-cycle internal pipeline delay
	//	between an ixfr and usage as a source operand.
	//
	fst.d	 f2,ufltreg+(2*4)(r29)
	fst.q	 f4,ufltreg+(4*4)(r29)
#if	FP_ARGS_TO_SYSCALLS
	//
	//	if someone decides to pass floating-point parameters to
	//	Mach or NX system calls, these two lines need to be
	//	enabled.  Thank you, Mr. i860 Calling Conventions.
	//
	fst.q	 f8,ufltreg+(8*4)(r29)	// only really needed if FP args are...
	fst.q	f12,ufltreg+(12*4)(r29)	// ...passed to system calls.
#endif	FP_ARGS_TO_SYSCALLS
	ixfr	r0,f16	// xxx not necessary?
	ixfr	r1,f17	// xxx not necessary?
	ixfr	sp,f18
	ixfr	fp,f19
	ixfr	r4,f20
	ixfr	r5,f21
	ixfr	r6,f22
	ixfr	r7,f23
	fst.q	f16,0(r29)	//  r0-r3
	fst.q	f20,16(r29)	//  r4-r7
	ixfr	r8,f24
	ixfr	r9,f25
	ixfr	r10,f26
	ixfr	r11,f27
	ixfr	r12,f28
	ixfr	r13,f29
	ixfr	r14,f30
	ixfr	r15,f31
	fst.q	f24,32(r29)	//  r8-r11
	fst.q	f28,48(r29)	// r12-r15

	//
	//	register status at this moment:
	//
	//	 r0-r15 == saved in proper slots of pcb (and still intact)
	//	r16-r28 == syscall params, still intact
	//	r29     == current_pcb() (original value lost)
	//	r30	== kernel function pointer
	//	r31     == original syscall code
	//	 f0-f7  == saved in proper slots of pcb (and still intact)
	//	 f8-f15 == not saved (no need to do so)
	//	f16-f31 == copies of user's r0-r15 (user's originals destroyed)
	//	fir	== user's fir at time of trap (saved)
	//	psr	== user's psr at time of trap (saved)
	//	fsr	== user's fsr at time of trap (saved)

	//
	//	switch to this thread's kernel stack.
	//
#if	NCPUS > 1
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r31,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_KSTACK(r31),sp		// base of kernel stack
	addu	KSTACK_SIZE,sp,sp		// compute top of stack
	andnot	0xf,sp,sp			// enforce alignment

	//
	//	make the call
	//
	mov	r29, r13		// stash pcb pointer for return

	ld.c	psr,r31	
	or	PSR_IM,r31,r31		// interrupts on

#if	!ASMP
fast_syscall_calli::			// label for pretty stack trace
	calli	r30			// make the call
	 st.c	r31,psr			// enable interrupts
#else	ASMP
fastcall_baton_enter::			// label for baton_disable()
	call	_baton_enter_syscall	// uses r6-r10 only
	 st.c	r31,psr			// enable interrupts

#if	BATON_DEBUG_HW
	call	_baton_assert_owner_syscall
	 nop
#endif	/* BATON_DEBUG_HW */
fast_syscall_calli::			// label for pretty stack trace
	calli	r30
	 nop
#endif	ASMP

	st.c	r0,psr			// disable interrupts

	//
	//	the syscall returned, but not via thread_syscall_return(),
	//	so the return code needs to be "put" into the pcb (in
	//	case there is an AST that needs service),
	//	and the fir needs to step over the trap instruction.
	//

	// (r13 was  preserved by C calling conventions across the syscall)
	//	r13 = current_pcb[cpu_number()]
	//
	ld.l	ufir(r13),r31		// advance fir over the trap
	addu	4,r31,r31
	st.l	r31,ufir(r13)
	st.l	r16,64(r13)		// store syscall return code

#if	ASMP
	//	volatiles have been consumed by syscall and are now available.
	call	_baton_exit
	 nop
#endif	ASMP

	//
	//	fast_syscall_continue() is an invokable continuation
	//	called by thread_syscall_return().
	//
_fast_syscall_continue::

	st.c	r0,psr			// disable interrupts

	//
	//	check for pending AST's
	//
#if	NCPUS > 1
	FAST_CPU_NUMBER(r28)
	shl	2,r28,r28
	orh	ha%_need_ast,r28,r28
	ld.l	l%_need_ast(r28),r28	// r28 = need_ast[cpu_number]
#else	/* NCPUS > 1 */
	orh	ha%_need_ast,r0,r28
	ld.l	l%_need_ast(r28),r28	// r28 = need_ast[cpu_number]
#endif	/* NCPUS > 1 */
	btne	0,r28,.fastcall_handle_ast	// branches not taken are fast

	//
	//	reload the relevant registers from the pcb
	//
#if	NCPUS > 1
	FAST_CPU_NUMBER(r29)
	shl	2,r29,r29	// convert cpu # to longword offset
	orh	ha%_current_pcb,r29,r29
	ld.l	l%_current_pcb(r29),r29
#else
	orh	ha%_current_pcb,r0,r29
	ld.l	l%_current_pcb(r29),r29
#endif
	//	r29 = current_pcb[cpu_number()]
	//	reloading r29 was redundant if we were called from
	//	thread_syscall_return

	fld.d	ufltreg+(2*4)(r29),f2	// f2-f3
	fld.q	ufltreg+(4*4)(r29),f4	// f4-f7
	fld.q	 0(r29),f16	//  r0-r3
	fld.q	16(r29),f20	//  r4-r7
	fld.q	32(r29),f24	//  r8-r11
	fld.q	48(r29),f28	// r12-r15

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

	//
	//	register status at this moment:
	//
	//	 r0-r15 == restored
	//	r16     == syscall return value (restored in shadow of bri)
	//	r17-r28 == undefined
	//	r29     == current_pcb()
	//	r30	== available
	//	r31     == available
	//	 f0-f7  == restored
	//	 f8-f15 == undefined
	//	f16-f31 == copies of r0-r15 (user's originals destroyed)
	//	fir	== user's fir at time of trap (saved)
	//	psr	== user's psr at time of trap (saved)
	//	fsr	== user's fsr at time of trap (saved)

	//
	//	fix up the psr for the return to user-mode
	//
	ld.l	upsr(r29),r30
        andnot  PSR_FT|PSR_DAT|PSR_IAT|PSR_IN|PSR_IT|PSR_U|PSR_IM,r30,r30
        or      PSR_IT|PSR_PU|PSR_PIM,r30,r30
        st.c    r30,psr
        //
        //  restore db
        //
        ld.l    udb(r29),r30    // restore db
        st.c    r30,db

	//
	// clear T, KR, KI
	//
	st.c	r0,fsr		// disable fp traps
        r2apt.ss f0,f0,f0
        r2apt.ss f0,f0,f0
        r2apt.ss f0,f0,f0
        i2apt.ss f0,f0,f0
        famov.ss f0,f0

	//
	//	restore the fsr
	//
	ld.l	ufsr(r29),r30
	st.c	r30,fsr

	//
	//	return from the fast call
	//
	ld.l	ufir(r29),r31
	bri	r31
	 ld.l	64(r29),r16	// syscall return value

	//
	//	Branch here if there is an AST
	//	that needs service.
	//
.fastcall_handle_ast:
#if	NCPUS > 1
	FAST_CPU_NUMBER(r31)
	shl	2,r31,r31	/* convert cpu # to longword offset */
	orh	ha%_active_threads,r31,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#else	/* NCPUS > 1 */
	orh	ha%_active_threads,r0,r31
	ld.l	l%_active_threads(r31),r31	/* r31 = current_thread() */
#endif	/* NCPUS > 1 */
	ld.l	THREAD_KSTACK(r31),sp		// base of kernel stack
	addu	KSTACK_SIZE,sp,sp		// compute top of stack
	call	_i860_call_asttaken
	 andnot	0xf,sp,sp			// enforce alignment
	//
	//	NOTREACHED
	//	i860_call_asttaken() does not return to its caller.
	//
	br	_thread_exception_return
	 nop

	//
	//	Branch here for fastcalls (trap r16,r0,r0) that evaluate
	//	to the routine kern_invalid().  What will happen is we'll
	//	patch things up and branch to the "not_so_fast" part of
	//	the syscall path which has the emulated syscall logic.
	//
	//
	//	register status at this moment:
	//
	//	 r0-r12 == untouched
	//	r13	== saved fsr (original r13 in p1)
	//	r14	== fir (original in p2)
	//	r15	== psr (original in p3)
	//	r16-r27 == syscall params (untouched)
	//	>r28	== &mach_trap_table[0] (original in f16)
	//	>r29    == pointer to kern_invalid() (original in f17)
	//	>r30	== pointer to kern_invalid() (original in f18)
	//	r31	== "untouched"
	//	 f0-f15 == untouched
	//	<f16	== user's r28 (original f16 lost)
	//	<f17	== user's r29 (original f17 lost)
	//	<f18	== user's r30 (original f18 lost)
	//	<f19	== user's r31 (original f19 lost)
	//	f20-f31 == untouched
	//	fsr	== untouched
	//	p0	== undefined
	//	p1	== user's r13
	//	p2	== user's r14
	//	p3	== user's r15
	//
.fastcall_abort:
	st.c	r13,fsr		// restore fsr at time of trap
	fxfr	f16,r28		// restore user's r28
	fxfr	f17,r29		// restore user's r29
	br	.not_so_fast
	 fxfr	f18,r30			// ...andl restore r30.

#endif	FASTTRAPS
