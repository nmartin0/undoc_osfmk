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
 * Copyright 1988, 1989, 1990, 1991, 1992 by Intel Corporation,
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
 * Multi-processor startup routines.
 *
 * $Id: mp_start.s,v 1.18 1995/03/14 08:04:38 lenb Exp $
 */

#include <machine/psl.h>
#include <mach_lock_mon.h>
#include <mach_mp_debug.h>
#include <mach_ldebug.h>

	.file	"mp_start.s"
	.text
	.align	4

#include <cpus.h>

#if	MCMSG || (NCPUS > 1)

#include <machine/cpu_number.h>
#include <i860paragon/lbus.h>

/*
 * set my CPU number in a processor protected register.
 * required so cpu_number() works!!
 *
 * see <i860/cpu_number.h>
 *
 * inputs:
 *	mycpunumber	guess what.
 *
 * outputs:
 *	none.
 *
 * side effects:
 *	CPU_REG = cpu number.
 */

_set_my_cpu_number::
	SET_MY_CPU_NUMBER(r16)
	bri	r1
	  nop

/*
 * return MY cpu number
 *
 *	try fast way (out of processor priv reg) if magic cookie doesn't match
 *	then read the hardware and setup fast way.
 *
 * see <i860/cpu_number.h>
 *
 */

_cpu_number1::
_cpu_number::
	bri	r1
	 FAST_CPU_NUMBER(r16)	// n.b. assumes this macro is 1 instruction

/*
 * r13 bad node status value
 */
cpunum_fail::
	orh	ha%_badcpunum,r0,r16
	st.l	r13,l%_badcpunum(r16)

	orh	h%_boot_stack_hi,r0,sp
	or	l%_boot_stack_hi,sp,sp	/* tmp stack */
	orh	h%badCPUnum,r0,r16
	br	_panic
	  or	l%badCPUnum,r16,r16

	.data
badCPUnum:
	.string "node status register returned bad cpu number."
	.align	4
badSlaveStack:
	.string "mp_start() invalid stack"
	.align 4
_badcpunum::
	.long	0

	.text
	.align	4

	.globl	_panic

/*
 * While cpu0 runs pstart(), other cpus run mp_start().
 * They get here from alltraps() when a reset trap is detected.
 *
 * Disable interrupts (0->psr)
 * Set the fast cpu register with our valid cpu number
 *
 * if ((cpu_number() == 1) && boot_msg_proc)
 *	setup up mcp_stack
 *	unlatch bear
 *	call msgp_main()
 * else (i'm a regular application CPU)
 *	setup slave stack
 *	unlatch bear
 * 	call slave_main()
 *
 * inputs:
 *	none.
 *
 * todo:
 *	we still allocate both a slave_stack and a msgp_stack for cpu1
 *	and use just one or the other.
 */

_mp_start::
	st.c	r0,psr		/* clear psr, NO interrupts */
        st.c    r0,fsr
	or	0x4000,r0,r31		// Write-Protect mode
	st.c	r31,epsr
        st.c    r0,dirbase
        mov     r0,fp

        st.c    r0,p0
        st.c    r0,p1
        st.c    r0,p2
        st.c    r0,p3

	/*
	 * Read node status register for my cpu number and set our fast cpu
	 * number register. Assume all registers are scratch.
	 */
        orh     h%LB_NODE_STATUS,r0,r13
        or      l%LB_NODE_STATUS,r13,r13
        ldio.l  r13,r13			/* read node status reg */

	/*
	 * Determine the CPU number from the Node Status Register:
	 *
	 * if (NSR[21]) cpu 1
	 * else if (NSR[20]) cpu 0
	 * else if (NSR[19]) cpu 2	(NSR[19] undefined on GP node)
	 */

        shr     19,r13,r13              /* bit 19, 20 & 21 are cpu 2, 0 & 1 */
        and     4, r13, r0              /* Does CPU1 own the bus? */
        bnc.t	save_cpunum
	 mov     1, r13

        and     2, r13, r0              /* Does CPU0 own the bus? */
        bnc.t	save_cpunum
	 mov     0, r13

        and     1, r13, r0              /* Does CPU2 Own the bus? */
        bnc.t	save_cpunum
	 mov     2, r13

        br      cpunum_fail             /* Should never happen.. but... */
	 nop
	
save_cpunum:
	SET_MY_CPU_NUMBER(r13)		/* (p0) for FAST access */

	// r13 == my cpu_number()

	btne 1, r13, not_mcp		/* if (cpu_number() != 1) */

	orh ha%_boot_msg_proc, r0, r13
	ld.l l%_boot_msg_proc(r13), r13
	or r0, r13, r13			// CC set if result zero
	bc.t not_mcp
	 or	1, r0, r13		/* restore cpunum 1 to r13 */

	orh	ha%_msgp_stack,r0,r13
	ld.l	l%_msgp_stack(r13),sp	// now we have a valid stack.
	nop
	andnot	0xf,sp,sp		// sanity: double-word align

        // unlatch bear to catch future parity or bus errors...
	nop
	nop
        ld.c    bear,r0
	nop
	nop

	call	_msgp_main	// see i860paragon/msgp/msgp_mp.c
	 nop

mcp_returned:

	orh     h%msgp_string,r0,r16
	call	_panic
	  or      l%msgp_string,r16,r16
	br	.tight
	 nop


not_mcp:
#if	NCPUS > 1
	.globl	_slave_stacks
	.globl	_slave_main

	// find my startup stack.
	shl	2,r13,r31		// convert cpu # to longword offset
	orh	ha%_slave_stacks,r31,r31
	ld.l	l%_slave_stacks(r31),sp	// now we have a valid stack.
	nop
	andnot	0xf,sp,sp		// sanity: double-word align

        // unlatch bear to catch future parity or bus errors...
	nop
	nop
        ld.c    bear,r0
	nop
	nop

	call	_slave_main	// see kern/startup.c 
	  nop

slave_returned:

	orh     h%slave_msg,r0,r16
	call	_panic
	  or      l%slave_msg,r16,r16
#endif	/* NCPUS > 1 */
.tight:	br	.tight
	  nop

	.data
msgp_string:
	.string	"mp_start() msgp_main() returned!"
	.align	4
	.text

#if	NCPUS > 1
	.data
slave_msg:
	.string	"mp_start() slave_main() returned!"
	.align	4
	.text


/*
 * void switch_to_shutdown_context(thread_t thread,
 *                                 void (*routine)(processor_t),
 *                                 processor_t processor)
 *
 * saves the kernel context of the thread,
 * switches to the interrupt stack,
 * continues the thread (with thread_continue),
 * then runs routine on the interrupt stack.
 *
 * Assumes that the thread is a kernel thread (thus
 * has no FPU state)
 */

	.globl	_active_stacks

_switch_to_shutdown_context::
        FAST_CPU_NUMBER(r29)
// save my state on active_stacks[cpu_number()]
	orh     h%_active_stacks,r0,r31
	or      l%_active_stacks,r31,r31
	shl     2,r29,r29
	addu    r29,r31,r29
	ld.l    0(r29),r31		// r31 base of kernel base.

	st.l	r0,0(r31)
	st.l	r1,4(r31)
	st.l	sp,8(r31)
	st.l	fp,12(r31)
	st.l	r4,16(r31)
	st.l	r5,20(r31)
	st.l	r6,24(r31)
	st.l	r7,28(r31)
	st.l	r8,32(r31)
	st.l	r9,36(r31)
	st.l	r10,40(r31)
	st.l	r11,44(r31)
	st.l	r12,48(r31)
	st.l	r13,52(r31)
	st.l	r14,56(r31)
	st.l	r15,60(r31)
	st.l	r16,64(r31)
	st.l	r17,68(r31)
	st.l	r18,72(r31)
	st.l	r19,76(r31)
	st.l	r20,80(r31)
	st.l	r21,84(r31)
	st.l	r22,88(r31)
	st.l	r23,92(r31)
	st.l	r24,96(r31)
	st.l	r25,100(r31)
	st.l	r26,104(r31)
	st.l	r27,108(r31)
	st.l	r28,112(r31)	// already saved
	//st.l	r29,116(r31)	// don't care
	st.l	r30,120(r31)	// already saved
	//st.l	r31,124(r31)	// don't care

// XXX: clear continuation

	// switch to my original slave stack
	// get a valid stack, pointers are the TOP(highest adrs) of the kstack.
        FAST_CPU_NUMBER(r29)
	orh	h%_slave_stacks,r0,r31  // base of slave_stack vector
	or	l%_slave_stacks,r31,r31
	shl	2,r29,r20               // scale CPU number to a longword offset
	addu	r20,r31,r31             // compute base address + cpu offset
	ld.l	0(r31),sp               // switch to my kernel stack.

	// reschedule thread
	addu	-16,sp,sp		// save routine adrs and it's arg
	st.l	r18,0(sp)		// arg
	st.l	r17,4(sp)		// routine
	call	_thread_dispatch	// r16 == thread pointer
	  nop
	ld.l	4(sp),r17		// rtn() to call
	ld.l	0(sp),r16		// rtn( r16 )

	// call specified routine.
	calli	r17
	  addu	16,sp,sp		// prune stack

	/* NEVER REACHED */

#endif	/* NCPUS > 1 */
#endif	/* MCMSG || (NCPUS > 1) */

