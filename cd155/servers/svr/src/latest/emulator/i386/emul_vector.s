/*
 *
 *$Copyright
 *Copyright 1993, 1994 , 1995 Intel Corporation
 *INTEL CONFIDENTIAL
 *The technical data and computer software contained herein are subject
 *to the copyright notices; trademarks; and use and disclosure
 *restrictions identified in the file located in /etc/copyright on
 *this system.
 *Copyright$
 *
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: emul_vector.s,v $
 *Revision 1.7  1995/02/01  21:24:11  bolsen
 * Reviewer(s): Jerry Toman
 * Risk: Medium (lots of files)
 * Module(s): Too many to list
 * Configurations built: STD, LITE, & RAMDISK
 *
 * Added or Updated the Locus Copyright message.
 *
 *Revision 1.6  1994/11/18  20:25:16  mtm
 *Copyright additions/changes
 *
 *Revision 1.5  1994/03/14  01:45:16  slk
 *Checkpoint Restart Code Drop
 * Reviewer: Stefan Tritscher
 * Risk: Medium
 * Benefit or PTS #: Enhancement
 * Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 * Module(s):
 *
 *Revision 1.4  1993/07/14  17:32:23  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 *Revision 1.1.1.3  1993/07/01  18:25:17  cfj
 *Adding new code from vendor
 *
 *Revision 1.3  1993/05/06  18:59:03  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:18:51  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:09:38  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:30:05  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  1992/10/08  11:24:16  roman
 * Change strategy of initial code for rexec arrival on a new node.
 * 	Now calls rexecve_init() with no parameters and then
 * 	calls e_rexecve_arrival() via fake system call interface.
 * 	This allows for correct handling of traced rexec-ed
 * 	processes.
 *
 * Revision 2.10  92/10/06  12:05:44  roman
 * Fix RCS comments.
 * 
 * Revision 2.9  92/10/05  13:46:56  klh
 * 	Revision 2.7  92/08/26  12:09:50  loverso
 * 		Manipulate count of threads on emulator stack at syscall 
 *		enter/exit. (loverso)
 * 
 * Revision 2.8  92/04/17  09:48:42  chrisp
 * For TNC, correct stack balancing in emul_sigreturn prior to faking the
 * 	sigreturn trap.
 * 
 * Revision 2.7  92/02/17  14:26:14  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.6  92/02/17  14:26:02  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.5  92/02/11  18:42:42  pjg
 * 	Add code for rfork/rexec/migrate (roman@locus).
 * 
 * Revision 2.4  91/12/16  09:17:51  roy
 * 	91/11/08  13:55:21  bernadat
 * 	Added "A la OSF/1 1.0.2" fpu initialization
 * 
 * Revision 2.3  91/10/14  12:44:20  srl
 * repaired comment lines to all start with <space> *
 * 
 * Revision 2.2  91/08/30  16:40:38  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.3  91/08/27  15:25:42  barbou
 * Upgrade to UX26.
 *
 * Revision 3.2  91/06/12  09:33:38  condict
 * Eliminate oexecve syscall and stubs (no longer needed for bootstrap).
 *
 * Revision 3.1  91/05/07  15:29:44  condict
 * Add OSF/1 e_execve function (and rename 4.3 version to oexecve).
 *
 * Revision 3.0  91/01/17  12:05:15  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 *
 * Revision 2.4  91/03/20  15:01:38  dbg
 * 	Unlock locks with a locked instruction.
 * 	More fixes for GNU preprocessor.
 * 	[91/02/01            dbg]
 * 
 * Revision 2.3  90/12/05  18:54:34  af
 * 	Made GNU preproc happy by separating dollars from tokens.
 * 	[90/12/05  18:53:24  af]
 * 
 * Revision 2.2  90/05/21  13:46:29  dbg
 * 	Wrote i386 version.
 * 	[90/03/14            dbg]
 * 
 * $EndLog$
 */
/*
 * Author:  Alessandro Forin (af) at Carnegie-Mellon University
 */
#include <sys/syscall.h>
#include <i386/fpreg.h>

/*
 * Emulator vector entry - allocate new stack
 */
	.data
	.globl	_emul_stack_lock
_emul_stack_lock:
	.long	0
	.text

/*
 * Generic emulator entry
 *
 * ESP->	flags
 *		pc to return to
 * system call number is in %eax
 */

/*
 * Generic call
 */
	.globl	_emul_common
_emul_common:
	pushl	%eax			/ save registers that C does not
	pushl	%ecx
	pushl	%edx
0:	movl	$1,%eax
	xchg	%eax,_emul_stack_lock	/ lock emul_stack lock
	orl	%eax,%eax
	jne	0b
	movl	_on_emul_stack,%eax	/ increment count of emulator threads
	incl	%eax
	movl	%eax,_on_emul_stack
	movl	_emul_stack_list,%eax	/ grab an emulator stack
	orl	%eax,%eax
	jne	3f			/ if none:
	xchg	%eax,_emul_stack_lock	/   release the lock (%eax is zero)
	call	_emul_stack_alloc	/   allocate a stack
	jmp	4f			/   and use it
3:	movl	(%eax),%ecx		/   else
	movl	%ecx,_emul_stack_list	/   remove the stack from list
	xorl	%ecx,%ecx		/   and unlock
	xchg	%ecx,_emul_stack_lock	/   the lock
4:	movl	%esp,(%eax)		/ Save user`s SP at top of new stack
	movl	%eax,%esp		/ switch to emulator`s stack
	pushl	%ebx			/ save the remaining registers
	pushl	%esi
	pushl	%edi
	pushl	%ebp
	pushl	%esp			/ push address of registers
	call	_emul_syscall		/ call C code
	addl	$4,%esp			/ pop parameter
/*
 * Return
 */
	.globl	emul_exit
emul_exit:
	popl	%ebp			/ restore registers
	popl	%edi
	popl	%esi
	popl	%ebx
	movl	%esp,%ecx		/ save emulator stack address
	movl	(%esp),%esp		/ return to user`s stack

0:	movl	$1,%eax
	xchg	%eax,_emul_stack_lock	/ lock emul_stack lock
	orl	%eax,%eax
	jne	0b
	movl	_on_emul_stack,%eax	/ decrement count of emulator threads
	decl	%eax
	movl	%eax,_on_emul_stack
	movl	_emul_stack_list,%eax
	movl	%eax,(%ecx)		/ chain emulator stack on list
	movl	%ecx,_emul_stack_list
	xorl	%ecx,%ecx		/ release
	xchg	%ecx,_emul_stack_lock	/ the lock

	popl	%edx			/ restore regs that C does not save
	popl	%ecx
	popl	%eax
	popf				/ restore flags
	ret				/ return to user

/*
 * Child starts executing here with return values in eax/edx,
 * stack pointing to saved edx/ecx/eax/flags/eip.
 */
	.globl	_child_fork
_child_fork:
	movl	%eax,8(%esp)		/ save eax
	movl	%edx,(%esp)		/ and edx
	call	_child_init		/ initialize emulator for child
	call	_fpinit
	popl	%edx			/ restore registers that C does not
	popl	%ecx
	popl	%eax
	popf				/ restore flags
	clc				/ clear carry for Success return
	ret				/ return to user

	.globl	_fpinit

/* 
 * OSF1 does not inheritate fp regs on fork. Only the csw
 * is initialized on first fp op with a default OSF/1 kernel
 * value. Unfortunately the OSF/1 default value is not the micro
 * kernel one. So the csw is initialized as in fpinit() in i386/fpsup.c
 * The micro kernel does not support machines without FPUs for now.
 * Latter on, this will have to be done using the thread_set_status
 * on the server side, to check presence of the fpu then to set csw.
 *
 * There should be no performance penalty, the kernel only save and restores
 * the fpu if it is really used regardless of the fpu saved state.
 */

_fpinit:
	fninit
	pushl	%eax		/ get stack space
	fstcw	(%esp)
	popl	%eax
	andl	$(~(FPPC|FPRC)), %eax
	orl	$(FPSIG53|FPRTN|FPZDIV|FPOVR|FPUNR|FPINV|FPDNO|FPPRE), %eax
	pushl	%eax
	fldcw	(%esp)
	popl	%eax
	ret

#ifdef	TNC
/*
 * Newly-arrived migrating task starts executing here with return values
 * in eax/edx, stack pointing to saved edx/ecx/eax/flags/eip.
 */
	.globl	_migrate_arrival
_migrate_arrival:
	movl	%eax,8(%esp)		/ save eax from kernel
	movl	%edx,(%esp)		/ and edx from kernel

	call	_migrate_init		/ initialize emulator for child

	popl	%edx			/ restore registers that C does not
	popl	%ecx
	popl	%eax
	popf				/ restore flags
	xorl	%eax,%eax		/ clear return value
	clc				/ clear carry for Success return
	ret				/ return to user

/*
 * Newly-arrived rexec'ing emulator task starts executing here with
 * return values in eax/edx, stack pointing to saved edx/ecx/eax/flags/eip.
 * Note that this is true only for emulators rexec'ing to a node with
 * identical architecture.  For rexec'ing to a node with a differing
 * architecture, a fresh copy of the emulator is started up, passed
 * a flag to tell it that it is a new rexec'ing emulator.  Based
 * upon that flag, that new emulator will perform the same init call
 * below. After initializing the new emulator's stack, a trap is faked
 * into the arrival code so that the rexecve is completed on the emulator's
 * stack.
 */
	.globl	_rexecve_arrival
_rexecve_arrival:
	call	_rexecve_init		/ initialize emulator
	movl	$SYS_rexecve_arrival,%eax / system call number for
					/   emulator-internal arrival fn
	pushl	$emul_rexec_error	/ push error return address
	pushfl				/ push flags
	jmp	_emul_common		/ take a system call 'trap'

	/* we only get here on failure */
emul_rexec_error:
	hlt				/ what else ?

#endif	/* TNC */

/*
 * The easiest way to exec /etc/init is to take
 * an emulator trap (it sets up the registers nicely).
 */
	.globl	_emul_execve
_emul_execve:
	movl	$ SYS_execve,%eax	/ exec system call number
	pushl	$emul_exec_error	/ push error return address
	pushfl				/ push flags
	jmp	_emul_common		/ take a system call 'trap'

	/* we only get here on failure */
emul_exec_error:
	movl	$-1,%eax		/ failure return
	ret

/*
 * Get segment registers
 */
	.globl	_get_seg_regs
_get_seg_regs:
	movl	4(%esp),%edx	/ point to register buffer
	xorl	%eax,%eax	/ clear high 16 bits
	mov	%cs,%ax
	movl	%eax,0(%edx)	/ cs to regs[0]
	mov	%ss,%ax
	movl	%eax,4(%edx)	/ ss to regs[1]
	mov	%ds,%ax
	movl	%eax,8(%edx)	/ ds to regs[2]
	mov	%es,%ax
	movl	%eax,12(%edx)	/ es to regs[3]
	mov	%fs,%ax
	movl	%eax,16(%edx)	/ fs to regs[4]
	mov	%gs,%ax
	movl	%eax,20(%edx)	/ gs to regs[5]
	ret

#ifdef	TNC
/*
 * This is the sigreturn trampoline code used by the sigmigrate handler
 * internal to the emulator. It fixes up the stack and fakes a sigreturn
 * system call to dismiss the signal.
 */
	.globl	_emul_sigreturn
_emul_sigreturn:
	addl	$8,%esp			/ pop enough signal handler context
					/   prior to sigreturn trap
	movl	$SYS_sigreturn,%eax	/ exec sigreturn call number
	pushl	$emul_error		/ push error return address
	pushfl				/ push flags
	jmp	_emul_common		/ take a system call 'trap'

	/* we only get here on failure */
emul_error:
	movl	$-1,%eax		/ failure return
	ret

/*
 * This routine is used by the emulator's default SIGMIGRATE handler
 * to invoke the migrate/chkpnt_async system call. Since we're in the emulator
 * already we fake a system call as above.
 */
	.globl	_emul_migrate
_emul_migrate:
	movl	$SYS_migrate,%eax	/ migrate system call number
	pushl	$emul_migreturn		/ push return address
	pushfl				/ push flags
	jmp	_emul_common		/ take a system call 'trap'

emul_migreturn:
	ret

#endif	/* TNC */
