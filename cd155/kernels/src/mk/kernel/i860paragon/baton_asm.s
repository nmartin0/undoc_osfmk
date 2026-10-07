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
 * $Id: baton_asm.s,v 1.5 1995/04/04 20:55:55 lenb Exp $
 */
 
#if	ASMP
/*
 * baton_asm.s
 * primitives for ASMP microkernel
 *
 * Len Brown, Intel SSD.
 */

#include <mach_assert.h>
#include <kern/cpu_number.h>
#include <i860paragon/baton.h>
#define	BATON_SPINOUT	-90000000	/* gives us about 12 seconds */
#define	BUG_12770_WORKAROUND 1

	.file "baton_asm.s"
	.text

/*
 * void baton_enter(): returns with kernel_baton owned
 */

	// leaf: uses r16-r20 only
_baton_enter::
	mov	_kernel_baton, r16	// r16 = &kernel_baton
	FAST_CPU_NUMBER(r17)		// r17 = cpu_number()
	mov	BATON_FREE, r18		// r18 = BATON_FREE
	lock
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu [LOCK]
	unlock
	btne	r19, r18, baton_owned	// if(kernel_baton.cpu != BATON_FREE)
	bri	r1
	 st.l	r17, BATON_CPU(r16)	// kernel_baton.cpu = cpu_number() [UL]

baton_owned:	// somebody owns baton, is it me?
	btne	r19, r17, baton_block	// if(kernel_baton.cpu != cpu_number())
	ld.l	BATON_LEVEL(r16), r19	// r19 = kernel_baton.level [UNLOCK]
	adds	1, r19, r19		// r19++
	bri	r1
	 st.l	r19, BATON_LEVEL(r16)	// kernel_baton.level = r19

baton_try:	// looks free, try to acquire it.
	lock
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu
	unlock
	btne	r19, r18, baton_spin	// if(kernel_baton.cpu != BATON_FREE)
	bri	r1
	 st.l	r17, BATON_CPU(r16)	// kernel_baton.cpu = cpu_number();

baton_block:	// somebody else owns the baton
	mov	BATON_SPINOUT,r20	// r20 = BATON_SPINOUT

baton_spin:	// still not available
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu [UNLOCK]
	bte	r19, r18, baton_try	// if(kernel_baton.cpu == BATON_FREE)
	adds	1, r20, r20
	btne	r0, r20, baton_spin	// if (count != 0)

baton_timeout:	// it has been too long, give up.
	trap	r0, r0, r0
	br baton_timeout
	 nop

/*
 * void baton_exit()
 * if (kernel_baton.level != 0); kernel_baton.level--;
 * else kernel_baton.cpu = BATON_FREE;
 */
	// leaf: uses r16-r19 only
_baton_exit::
	mov	_kernel_baton, r16	// r16 = &kernel_baton
#if	BATON_DEBUG
	// verify that we own the baton before we execute the exit
	FAST_CPU_NUMBER(r17)		// r17 = cpu_number()
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu
	btne	r17, r19, baton_exit_failed
#endif	/* BATON_DEBUG */
	mov	BATON_FREE, r18		// r18 = BATON_FREE
#if	BUG_12770_WORKAROUND
	lock
#endif	/* BUG_12770_WORKAROUND */
	ld.l	BATON_LEVEL(r16), r19	// r19 = kernel_baton.level
#if	BUG_12770_WORKAROUND
	unlock
#endif	/* BUG_12770_WORKAROUND */
	btne	r19, r0, 1f		// if (kernel_baton.level != 0)
	bri	r1
	 st.l	r18, BATON_CPU(r16)	// kernel_baton.cpu = BATON_FREE

1:	// level was not zero, decrement it.
	adds	-1, r19, r19
	bri	r1
	 st.l	r19, BATON_LEVEL(r16)	// kernel_baton.level = level - 1

#if	BATON_DEBUG
baton_exit_failed:
	trap	r0, r0, r0
	br baton_exit_failed
	 nop
#endif	/* BATON_DEBUG */

/*
 * void baton_enter_syscall(): returns with kernel_baton owned
 *
 * Like baton_enter(), but used on syscall entry where the
 * "volatile" registers are available b/c they're saved in the PCB.
 * Also, doesn't have to check for recursion.
 */
_baton_enter_syscall::
	mov	_kernel_baton, r6	// r6 = &kernel_baton
	FAST_CPU_NUMBER(r7)		// r7 = cpu_number()
	mov	BATON_FREE, r8		// r8 = BATON_FREE
	mov	BATON_SPINOUT,r10	// r10 = BATON_SPINOUT
baton_try_s:
	lock
	ld.l	BATON_CPU(r6), r9	// r9 = kernel_baton.cpu [LOCK]
	unlock
	btne	r9, r8, baton_spin_s	// if(kernel_baton.cpu != BATON_FREE)
	bri	r1
	 st.l	r7, BATON_CPU(r6)	// kernel_baton.cpu = cpu_number();

baton_spin_s:	// still not available
	ld.l	BATON_CPU(r6), r9	// r9 = kernel_baton.cpu [UNLOCK]
	bte	r9, r8, baton_try_s	// if(kernel_baton.cpu == BATON_FREE)
	adds	1, r10, r10
	btne	r0, r10, baton_spin_s	// if (count != 0)

baton_timeout_s:	// it has been too long, give up.
	trap	r0, r0, r0
	br baton_timeout_s
	 nop

#if	BATON_DEBUG_HW
	// uses only r6, r7, r11
_baton_assert_owner_syscall::
	mov	_kernel_baton, r6	// r6 = &kernel_baton
	FAST_CPU_NUMBER(r7)		// r7 = cpu_number()
	ld.l	BATON_CPU(r6), r11	// r11 = kernel_baton.cpu
	bte	r11, r7, 0f		// if(kernel_baton.cpu == cpu_number())
	trap	r0, r0, r0

0:	bri	r1
	 nop
#endif	/* BATON_DEBUG_HW */

#if	MACH_ASSERT || BATON_DEBUG

_baton_assert_owner::
	mov	_kernel_baton, r16	// r16 = &kernel_baton
	FAST_CPU_NUMBER(r17)		// r17 = cpu_number()
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu
	bte	r19, r17, 0f		// if(kernel_baton.cpu == cpu_number())
	trap	r0, r0, r0

0:	bri	r1
	 nop

_baton_assert_not_owner::
	mov	_kernel_baton, r16	// r16 = &kernel_baton
	FAST_CPU_NUMBER(r17)		// r17 = cpu_number()
	ld.l	BATON_CPU(r16), r19	// r19 = kernel_baton.cpu
	btne	r19, r17, 0f		// if(kernel_baton.cpu != cpu_number())
	trap	r0, r0, r0

0:	bri	r1
	 nop
#endif	/* MACH_ASSERT || BATON_DEBUG */

/*
 * Disable baton locking.
 * This is called only upon boot of Uniprocessor nodes.
 */
_baton_disable::
	mov	0x40000800, r16		/* bri r1 */
	mov	0xa0000000, r17		/* nop */

	ld.c	psr, r20
	st.c	r0, psr			// r20 = sploff()

	mov	_baton_enter, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)

	mov	_baton_enter_syscall, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)

	mov	_baton_exit, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)

#if	BATON_DEBUG_HW
	mov	_baton_assert_owner_syscall, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)
#endif	/* BATON_DEBUG_HW */

#if	MACH_ASSERT || BATON_DEBUG
	mov	_baton_assert_owner, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)

	mov	_baton_assert_not_owner, r18
	st.l	r16, 0(r18)
	st.l	r17, 4(r18)

#endif	/* MACH_ASSERT || BATON_DEBUG */

	//
	// NOP out fast_syscall's "call baton_enter()"
	//
	mov	fastcall_baton_enter, r18
	st.l	r17, 0(r18)

	//
	// NOP out syscall's "call baton_exit()"
	//
	mov	thread_syscall_baton_exit, r18
	st.l	r17, 0(r18)

	br	_flush
	 nop

#endif	/* ASMP */



