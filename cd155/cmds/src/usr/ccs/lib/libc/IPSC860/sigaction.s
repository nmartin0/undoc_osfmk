/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * Signal trampoline code. Called/jmp'ed from emulator to start the processing
 * of a user supplied signal handler. Sigtramp calls user specified handler
 * which returns here and we do s sigreturn syscall so the server/emulator can
 * resume execution correctly.
 *
 *	_sigtramp( sig, code, *context, *handler() )
 *		    r16   r17    r18     r19
 *
 * Emulator is passed the address of _sigtramp() via sigvec() or sigaction()
 * syscalls.
 *
 *
 * Remember that with 4.3 semantics signals & longjumps are tied together!
 *
 * HISTORY:
 * $Log: sigaction.s,v $
 * Revision 1.6  1994/11/19  02:14:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/04/28  22:09:18  shala
 * Support Stack frame.
 *
 * Revision 1.4  1992/12/22  21:50:13  shala
 * Need more nops to get away from dual instruction mode problems.
 *
 * Revision 1.3  1992/12/18  21:47:27  shala
 * Added 2 nops at the start of sigtramp.  This causes the CPU to get
 * out of dual instruction mode (if it was in it.) before calling the signal
 * handler.  Do not remove those NOPs !!!
 *
 * Revision 1.2  1992/08/27  10:34:43  stans
 * added comments and RCS macros
 *
 *
 * $Id: sigaction.s,v 1.6 1994/11/19 02:14:14 mtm Exp $
 */

        .file "sigaction.s"

#include "SYS.h"

	.text
	.align	8
	.globl	__sigtramp
	.globl	_sigreturn
__sigtramp:
	nop			// Note: Do NOT remove these nops.  We
	nop			// might be in dual instruction mode and
	nop			// need the nops to get out!!
	nop
	nop
	nop
	mov	r18,r4		// trust me, its okay to do this because r4
				// will be restored from the sigcontext.
	calli	r19
	 nop
	nop
	mov	r4,r16		// sigreturn( *signal_context )
	br	_sigreturn
	 nop

/*
 * struct sigaction {
 *      int (*sa_handler)();		signal handler, or action value
 *      int sa_mask;			signals to block while in handler
 *      int sa_flags;			signal action flags
 * };
 *
 * sigaction(sig, action, oaction);
 *	int sig;
 *	struct sigaction *action, *oaction;
 *
 *
 * Emulator is passed the address of _sigtramp() via sigaction()
 *
 */

FENTRY(sigaction)
	mov	__sigtramp,r19          // signal trampoline stub 
	or	SYS_sigaction,r0,r31
	trap	r31,r31,r0
	bc	_cerror
	FRET
