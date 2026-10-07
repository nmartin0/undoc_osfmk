// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/* 
 * Mach Operating System
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 **********************************************************************
 * Author:  Alessandro Forin (af) at Carnegie-Mellon University
 *
 * HISTORY
 * $Log: emul_vector.s,v $
// Revision 1.4  1994/11/18  20:26:04  mtm
// Copyright additions/changes
//
// Revision 1.3  1993/07/14  17:34:37  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
 *
// Revision 1.1.1.3  1993/07/01  18:26:57  cfj
// Adding new code from vendor
//
// Revision 1.2  1992/11/30  22:10:29  dleslie
// Copy of NX branch back into main trunk
//
// Revision 1.1.2.1  1992/11/05  22:59:04  dleslie
// Local changes for NX through noon, November 5, 1992.
//
 * Revision 4.1  1992/11/03  23:59:07  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/30  16:41:02  rabii
 * 	Initial V2 Checkin
 *
Revision 3.0  91/01/17  12:05:32  condict
Unchanged copy from Mach 3.0 BSD UNIX server

 * Revision 2.2  89/08/31  16:28:32  rwd
 * 	Changed to reflect change in trampline code which pushes syscall
 * 	# on stack.
 * 	[89/08/23            rwd]
 * 
 * Revision 2.1  89/08/04  14:04:38  rwd
 * Created.
 * 
 * 29-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Switch to emulator-mode stack to avoid problems with user code
 *	with small stacks (VICE) and signal return bugs (thread_abort).
 *	Move most of the work into C code.
 *
 * 13-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added more individual entry points.
 *
 * 10-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	getrusage(), sigreturn(), osigcleanup() now call individual
 *	routines.
 *
 *  7-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Added entry points for execv, execve.  Made fork and vfork call
 *	e_fork.
 *
 * DBG - mangled for temporary emulator.
 */

#include <sys/syscall.h>
#include <sys/errno.h>

/*
 * Emulator vector entry - allocate new stack
 */
	.data
	.globl	_emul_stack_lock
_emul_stack_lock:
	.long	0
	.text

/*
 * Both of these sequences must be 8 bytes long, and leave the stacked
 * PC pointing at the end of the sequence.
 */
/*
 * Generic emulator entry
 */
#define	EE		nop; nop; jsb _emul_common

/*
 * Generic call
 */
	.globl	_emul_common
_emul_common:
	pushr	$0x3f			# save R0..R5
	brb	1f
0:	blbs	_emul_stack_lock,0b	# lock emul_stack lock
1:	bbssi	$0,_emul_stack_lock,0b
	movl	_emul_stack_list,r0	# grab an emulator stack
	bneq	3f			# if none:
	bbcci	$0,_emul_stack_lock,2f	#   release the lock
2:	calls	$0,_emul_stack_alloc	#   allocate a stack
	brb	4f			#   and use it
3:	movl	(r0),_emul_stack_list	# else remove the stack from list
	bbcci	$0,_emul_stack_lock,4f  #   and unlock the lock
4:	movl	sp,(r0)			# Save user's SP at top of new stack
	movl	r0,sp			# switch to emulator's stack
	calls	$0,_emul_syscall_tramp	# call C code, saving all registers
/*
 * Return
 */
	.globl	emul_exit
emul_exit:
	movl	sp,r1			# save emulator stack address
	movl	(sp),sp			# return to user's stack
	brb	1f
0:	blbs	_emul_stack_lock,0b	# lock emul_stack lock
1:	bbssi	$0,_emul_stack_lock,0b
	movl	_emul_stack_list,(r1)	# chain emulator stack on list
	movl	r1,_emul_stack_list
	bbcci	$0,_emul_stack_lock,2f	# release the lock
2:
	popr	$0x3f			# restore registers 0..5
	addl2	$4,sp			# pop jump table address
	rei				# and return to user

_emul_syscall_tramp:
	.word	0xfc0			# save R6..R11
	jmp	_emul_syscall+2;	# call emul_syscall

/*
 * Child starts executing here with return values in R0/R1,
 * stack pointing to saved R0..R5/PC/PSL.
 */
	.globl	_child_fork
_child_fork:
	movq	r0,(sp)			# save return values
	calls	$0,_child_init		# initialize emulator for child
	popr	$0x3f			# restore registers 0..5
	addl2	$4,sp			# pop off jump table address
	bicl2	$1,4(sp)		# turn carry off (in saved PSL)
					# for success
	rei				# and return to user

/*
 * Amazingly enough, the easiest way to exec /etc/init is to take
 * an emulator trap!
 */
	.globl	_emul_execve
_emul_execve:
	.word	0
	chmk	$SYS_execve
	/* we only get here on failure */
	mnegl	$1,r0
	ret
