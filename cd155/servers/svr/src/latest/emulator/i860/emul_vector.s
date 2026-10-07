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
 *Revision 1.14  1995/02/01  21:24:55  bolsen
 * Reviewer(s): Jerry Toman
 * Risk: Medium (lots of files)
 * Module(s): Too many to list
 * Configurations built: STD, LITE, & RAMDISK
 *
 * Added or Updated the Locus Copyright message.
 *
 *Revision 1.13  1994/11/18  20:25:46  mtm
 *Copyright additions/changes
 *
 *Revision 1.12  1994/03/14  01:45:47  slk
 *Checkpoint Restart Code Drop
 * Reviewer: Stefan Tritscher
 * Risk: Medium
 * Benefit or PTS #: Enhancement
 * Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 * Module(s):
 *
 *Revision 1.11  1993/11/18  02:49:55  cfj
 *Ifdef out the force migrate code which was experimental and
 *never ment to be used.
 *
 * Reviewer:
 * Risk:M
 * Benefit or PTS #:7151
 * Testing:
 * Module(s):
 *
 *Revision 1.10  1993/07/14  17:33:02  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 *Revision 1.1.1.4  1993/07/01  18:26:17  cfj
 *Adding new code from vendor
 *
 *Revision 1.9  1993/05/06  18:58:52  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:19:17  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.8  1993/04/13  16:08:16  cfj
 * Merge with T9.5.
 *
 * Revision 1.6.6.1  1993/04/12  22:33:35  cfj
 * Do full context restore on sigreturn.
 *
 * Revision 1.7  1993/04/03  03:19:12  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.6  1992/12/19  00:26:36  cfj
 * Added another nop at emul_common in case we are in dual instruction mode.
 *
 * Revision 1.1.2.2.2.2  1993/01/09  00:03:03  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  05:57:49  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/14  18:28:41  nandy
 * Added an extra stack frame to the emulator. Emulator now pushes the return
 * address of the call made by the user. emul_machdep.c has also been changed.
 *
 * Revision 1.3  1992/11/30  22:10:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/09  17:53:32  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 * 
 * Revision 1.1.2.1  1992/11/05  22:30:34  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/03  23:58:41  cfj
 * Bump major revision number.
 *
 * Revision 2.11  1992/10/14  17:24:36  cfj
 * NX integration.
 *
 * Revision 2.11  1992/11/05  10:39:17  klh
 * r29 bug fixes recevied from Intel
 *
 * Revision 2.10  92/10/08  11:29:00  roman
 * Change completely the handling of arrived rexec-ed processes. Code
 * 	now calls rexecve_init() with no parameters and then
 * 	calls e_rexecve_arrival() via the fake system call interface
 * 	(needed to correctly handle traced rexec-ed processes).
 * 
 * Revision 2.9  92/10/06  12:07:41  roman
 * Fix RCS comments.
 * 
 * Revision 2.8  92/10/05  13:47:39  klh
 * 	Revision 2.6  92/09/29  16:51:42  rabii
 * 		[1992/09/22  16:48:17  cfj]
 * 		Fixed problem in ret_sig: where it was causing the CC bit in 
 *		the psr to be set at the wrong time.
 * 
 * 		[1992/09/08  11:01:48  cfj]
 * 		Fix for r29 trashing bug.  Major change is that sigreturn 
 *		assumes that sigcontext.sc_fir is now set to a valid address 
 *		in the user code of where to return to.  The old R29 
 *		mechanism is only used for system call traps.
 * 
 * 		[92/07/26  10:34:36  nandy]
 * 		NX integration
 * 
 * 		[92/07/07  15:05:39  roman]
 * 		Minor format corrections to exactly match OSF source code.
 * 
 * Revision 2.7  92/07/07  15:05:39  roman
 * Minor format corrections to exactly match OSF source code.
 * 
 * Revision 2.6  92/06/10  14:20:06  chrisp
 * emul_migrate() now propagates return code from migrate() back to caller.
 * 
 * Revision 2.5  92/03/27  10:52:03  roman
 * Put in "#ifdef TNC" around all TNC code in this file.
 * 
 * Revision 2.4  92/02/26  08:43:25  roman
 * Fix RCS comments.
 * 
 * Revision 2.3  92/02/26  08:13:56  roman
 * Integrate Intel changes.
 * 
 *	Revision 2.5  92/02/12  15:29:55  cfj
 *	Fixed emul_sigreturn so that it functions the same as the i860
 *	signal trampoline code in libc.
 * 
 *	Revision 2.4  92/02/12  09:37:39  cfj
 *	Changed emul_migrate and emul_sigreturn to do a trap instead of trying
 *	to fake the trap to the emulator.
 * 
 *	Revision 2.3  92/02/11  13:08:37  cfj
 *	Added support for rfork, rexec and migrate.
 * 
 * Revision 2.2  91/11/14  15:34:32  rabii
 * 	Initial check-in
 * 
 *	Revision 2.3  91/10/16  11:21:08  stans
 *	working version for signals and mk63
 *
 *	Revision 2.2  91/09/03  11:11:36  jsb
 * 	First checkin. Derived from mips sources by Intel SSD.
 * 	[91/09/03  08:33:17  jsb]
 * 
 *		Revision 2.3  90/12/05  18:54:34  af
 * 		Made GNU preproc happy by separating dollars from tokens.
 * 		[90/12/05  18:53:24  af]
 * 
 *		Revision 2.2  90/05/21  13:46:29  dbg
 * 		Wrote i386 version.
 * 		[90/03/14            dbg]
 * 
 * $EndLog$
 */
/*
 * Author:  Alessandro Forin (af) at Carnegie-Mellon University
 */
#include <sys/syscall.h>
#include <sys/errno.h>

/*
 * Emulator vector entry - allocate new stack
 */
			.data
			.align	4
			.globl	_emul_stack_lock
_emul_stack_lock:	.long	0

/*
 * FAKE a reference to 'start' so 'crt0.0' will be loaded out of libmach_sa.a
 * and the entire silly mess can actually link/start.
 */
			.extern start
_a_true_hack:		.long	start

			.text

/*
 * The following structure is dumped on the user's stack on entrance
 * to the emulator.
 */
	.dsect		// dumped on the user stack
u_onstk:	.long		// see sigcontext
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
u_r29:	.long		// "emulator return address for traps" (see i860/trap.c)
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
u_fir:	.long		// pc, used by sigreturn
	.align	16
u_end:
	.end

/*
 * special extra frame for signal delivery
 */
	.dsect
sf_pc:	.long		// handler address
sf_r16:	.long		// signal number
sf_r17:	.long		// code
sf_r18:	.long		// sigcontext pointer
sf_r19:	.long		// handler
	.end

/*
 * Trampoline here from micro-kernel i860/ttrap.s
 * System call number is in r31.
 * r29 points to the instruction *after* the trap (where we want
 * the emulator to return to). (see i860/trap.c)
 * r16-r27 (syscall params) are preserved on user's stack.
 * Dump onto the user's stack the non-volatiles and syscall params.
 * Get an emulator stack, and switch to it.
 * Call emul_syscall() with pointers to the emulator stack & the user's.
 */
	.globl	_emul_common
	nop
_emul_common:
	nop		// Make sure there are 2 nops in case we are in
			// dual instruction mode.
	nop		// nop is advised since trap handler starts at fir+4.
	st.l	sp,u_sp-u_end(sp)	// align(46*4, 16) == 192
	adds	-u_end,sp,sp

	st.l	r0,u_r0(sp)
	st.l	r1,u_r1(sp)
	st.l	fp,u_fp(sp)
	st.l	r4,u_r4(sp)
	st.l	r5,u_r5(sp)
	st.l	r6,u_r6(sp)
	st.l	r7,u_r7(sp)
	st.l	r8,u_r8(sp)
	st.l	r9,u_r9(sp)
	st.l	r10,u_r10(sp)
	st.l	r11,u_r11(sp)
	st.l	r12,u_r12(sp)
	st.l	r13,u_r13(sp)
	st.l	r14,u_r14(sp)
	st.l	r15,u_r15(sp)
	st.l	r16,u_r16(sp)	// syscall param 1
	st.l	r17,u_r17(sp)	// syscall param 2
	st.l	r18,u_r18(sp)	// syscall param 3
	st.l	r19,u_r19(sp)	// syscall param 4
	st.l	r20,u_r20(sp)	// syscall param 5
	st.l	r21,u_r21(sp)	// syscall param 6
	st.l	r22,u_r22(sp)	// syscall param 7
	st.l	r23,u_r23(sp)	// syscall param 8
	st.l	r24,u_r24(sp)	// syscall param 9
	st.l	r25,u_r25(sp)	// syscall param 10
	st.l	r26,u_r26(sp)	// syscall param 11
	st.l	r27,u_r27(sp)	// syscall param 12
	st.l	r28,u_r28(sp)	// structure parameter address
	st.l	r29,u_r29(sp)	// ptr to instruction after the syscall trap

	st.l	r29,u_fir(sp)	// Sometimes we use fir.

	st.l	r30,u_r30(sp)	// unused
	st.l	r31,u_r31(sp)	// syscall code
#if 1
	fst.l	f0,u_f0(sp)	// store f0...f7
	fst.l	f1,u_f1(sp)
	fst.l	f2,u_f2(sp)
	fst.l	f3,u_f3(sp)
	fst.l	f4,u_f4(sp)
	fst.l	f5,u_f5(sp)
	fst.l	f6,u_f6(sp)
	fst.l	f7,u_f7(sp)
#else
	fst.q	f0,u_f0(sp)	// store f0,f1,f2,f3
	fst.q	f4,u_f4(sp)	// store f4,f5,f6,f7
#endif
	ld.c	psr,r16
	st.l	r16,u_psr(sp)
	ld.c	fsr,r16
	st.l	r16,u_fsr(sp)
	ld.c	epsr,r16
	st.l	r16,u_epsr(sp)

	// get an emulator stack

	// get the lock
	adds	l%1,r0,r16
0:	lock
	ld.l	_emul_stack_lock,r17
	unlock
	st.l	r16,_emul_stack_lock
	btne	r17,r0,0b
	// got the lock

	// increment _on_emul_stack
	ld.l	_on_emul_stack,r16
	addu    1,r16,r16
	st.l	r16,_on_emul_stack

	// now get a stack...
	ld.l	_emul_stack_list,r16	// grab an emulator stack
	btne	r16,r0,3f		// branch if we got something,
	call	_emul_stack_alloc	//   else go get a stack,
	  nop
	st.l	r0,_emul_stack_lock	//   and release the lock.
	br	4f
	  nop

3:	ld.l	0(r16),r17		// remove a stack from the list
	st.l	r17,_emul_stack_list	// emul_stack_list = r6->next
	st.l	r0,_emul_stack_lock	// release the lock

	// r16 == emulator stack-to-be
4:
	mov	r16,r18			// save original esp
	andnot	0xf,r16,r16		// coerce 16-byte alignment
	mov	sp,r17			// where the exception frame is, usp

	//
	// DANGER WILL ROBINSON!  0(sp) depends upon the
	// "struct emul_stack{}" and "struct emul_stack_top{}"
	// sizes. see i860/emul_machdep.c line #176 ...
	//
	
	adds	-32,r16,r16		// make space for storing emul_stack
	st.l	fp,8(r16)		// store old fp on the top of the stack
	adds 	8,r16,fp		// make the frame pointer point to it
	st.l    r29,12(r16)		// save the return addr for intr after trap	
	st.l    sp,0(r16)               // also store pointer to the ex. frame
	st.l    r18,4(r16)              // save emulator stack (esp).



	//
	// handle syscall and switch to emul stack (16-byte aligned) in the
	// shadow.
	//
	// r15 == original emulator stack ptr
	// emul_syscall(emulator_sp,user_sp)
	//
	call	_emul_syscall
	 mov	r16,sp			// switch to emulator stack.

/*
 * Emulated system calls return to here.
 *
 * Unwind the emulator stack. Switch back to the user's stack; emul_syscall()
 * may modify the user's sp so we reload it from where emul_syscall() mod/used
 * it. Save the just-used emulator stack on the emulator stack list.
 * Unwind the user's stack and "return" to the user.
 *
 * r16 contains return value from emul_syscall().
 * sp+0 == user's stack-ptr
 * sp+4 == original (unaligned) emulator stack ptr.
 */
	.globl	emul_exit
emul_exit:

        ld.l    4(sp),r18               // r18 == original emulator stack ptr
        ld.l    0(sp),sp                // restore user's stack-ptr

	mov	r16,r29			// save the return value for later

	// r18 now contains the emulator stack to be added to the list

	// get the lock
	adds	l%1,r0,r16
0:	lock
	ld.l	_emul_stack_lock,r17
	unlock
	st.l	r16,_emul_stack_lock
	btne	r17,r0,0b
	// got the lock

	// decrement _on_emul_stack
	ld.l	_on_emul_stack,r16
	addu    -1,r16,r16
	st.l	r16,_on_emul_stack

	// put the just-used emulator stack (now in r18) on the list
	or	l%_emul_stack_list,r0,r16
	orh	h%_emul_stack_list,r16,r16	// r16 = &emul_stack_list
	ld.l	0(r16),r17			// r17 = *r16;
	st.l	r17,0(r18)			// *r18 = r17;
	st.l	r18,0(r16)			// *r16 = r18;
	st.l	r0,_emul_stack_lock		// clear the lock

	// if (r29 == 0), it is a simple return
	// if (r29 == 1), it is a sigreturn (ie, full context restore)
	// if (r29 != 0 && r29 != 1), call handler, then do a "sigreturn"

	bte	r0,r29,ret_fast		// doesn't have a delay slot
	bte	1,r29,_Trap_sigreturn	// doesn't have a delay slot

	// call a signal handler -- r29 points to a "sigframe"
	ld.l	sf_r16(r29),r16		// sig
	ld.l	sf_r17(r29),r17		// code
	ld.l	sf_r18(r29),r18		// scp
	ld.l	sf_r19(r29),r19		// handler
	ld.l	sf_pc(r29),r29		// contents of sigtramp
	mov	sp,r15			// save current sp for later
	calli	r29			// XXX?? do I want to do *this*
	 mov	r18,sp			// XXX?? or should I bri r29??
	mov	r15,sp			// restore sp after handler returns

	//fall into...

ret_sig::
	// It is a sigreturn -- restore entire context
	// sigreturn depends on sigcontext.u_fir to be set to the
	// address of where to return in the user's code.  This is now
	// different that the standard trap case where r29 provides that
	// function.
	ld.l	u_fp(sp),fp
	ld.l	u_r4(sp),r4
	ld.l	u_r5(sp),r5
	ld.l	u_r6(sp),r6
	ld.l	u_r7(sp),r7
	ld.l	u_r8(sp),r8
	ld.l	u_r9(sp),r9
	ld.l	u_r10(sp),r10
	ld.l	u_r11(sp),r11
	ld.l	u_r12(sp),r12
	ld.l	u_r13(sp),r13
	ld.l	u_r14(sp),r14
	ld.l	u_r15(sp),r15
#if 1
	fld.l	u_f2(sp),f2	// load f2...f7
	fld.l	u_f3(sp),f3
	fld.l	u_f4(sp),f4
	fld.l	u_f5(sp),f5
	fld.l	u_f6(sp),f6
	fld.l	u_f7(sp),f7
#else
	fld.d	u_f2(sp),f2	// load f2,f3
	fld.q	u_f4(sp),f4	// load f4,f5,f6,f7
	//fld.q	u_f8(sp),f8	// someday, when we have a full restore
	//fld.q	u_f12(sp),f12
	//fld.q	u_f16(sp),f16
	//fld.q	u_f20(sp),f20
	//fld.q	u_f24(sp),f24
	//fld.q	u_f28(sp),f38
#endif
	ld.l	u_fsr(sp),r31	// someday, when we have a full restore
	st.c	r31,fsr
	ld.l	u_epsr(sp),r31
	st.c	r31,epsr

	/*
	 *  We cannot depend on the signal context returned by sigreturn
	 *  to be below the "old" sp the signal context.  Grab r29
	 *  out of the signal context and store it on the new (really old)
	 *  stack.  Then restore r29 from there.
	 */
	ld.l	u_sp(sp), r29		// Get old stack pointer
	adds	-16, r29,r29		// Make space for r29
	ld.l	u_r29(sp), r1		// Stuff r29 there
	st.l	r1,0(r29)		//

	ld.l	u_psr(sp),r16
	st.c	r16,psr			// for CC bit

	ld.l	u_r1(sp),r1
	ld.l	u_r16(sp),r16
	ld.l	u_r17(sp),r17
	ld.l	u_r18(sp),r18
	ld.l	u_r19(sp),r19
	ld.l	u_r20(sp),r20
	ld.l	u_r21(sp),r21
	ld.l	u_r22(sp),r22
	ld.l	u_r23(sp),r23
	ld.l	u_r24(sp),r24
	ld.l	u_r25(sp),r25
	ld.l	u_r26(sp),r26
	ld.l	u_r27(sp),r27
	ld.l	u_r28(sp),r28
	
	ld.l	u_fir(sp),r29		// pc where we return to!

	ld.l	u_r30(sp),r30
	ld.l	u_r31(sp),r31
	ld.l	u_sp(sp),sp		// old stack pointer

	bri	r29
	 ld.l	-16(sp),r29		// Get r29 back

ret_fast:
	ld.l	u_psr(sp),r16
	st.c	r16,psr			// for CC bit
	ld.l	u_fp(sp),fp		// frame pointer got changed
	ld.l	u_r1(sp),r1
	ld.l	u_r16(sp),r16
	ld.l	u_r17(sp),r17
	ld.l	u_r18(sp),r18
	ld.l	u_r19(sp),r19
	ld.l	u_r20(sp),r20
	ld.l	u_r21(sp),r21
	ld.l	u_r22(sp),r22
	ld.l	u_r23(sp),r23
	ld.l	u_r24(sp),r24
	ld.l	u_r25(sp),r25
	ld.l	u_r26(sp),r26
	ld.l	u_r27(sp),r27
	ld.l	u_r28(sp),r28
	ld.l	u_fir(sp),r29		// pc where we return to!
	ld.l	u_r30(sp),r30
	ld.l	u_r31(sp),r31
	ld.l	u_sp(sp),sp		// old stack pointer
ret_end:
	bri	r29
	 ld.l	u_r29-u_end(sp),r29	// align(46*4, 16) == 192

/*
 * Reinitialize the emulator before getting back to the child
 * We get here with the stack pointing to our copy of the
 * saved state, and the relevant regs modified.
 */
	.globl	_child_fork
	nop
_child_fork:
	nop				// superstition, probably...
	addu	-16,sp,sp
	st.l	r1,0(sp)		// push return address
	st.l	r16,4(sp)		// push r16
	st.l	r17,8(sp)		// push r17
	call	_child_init		// initialize emulator for child
	 st.l	r29,12(sp)		// push r29 (in shadow)
	ld.l	12(sp),r29		// pop r29
	ld.l	0(sp),r1		// pop r1
	ld.l	4(sp),r16		// pop r16
	ld.l	8(sp),r17		// pop r17
	bri	r29			// return to inst. after the trap
	 addu	16,sp,sp		// unwind stack (in shadow)

/*
 * The easiest way to exec /etc/init is to take
 * an emulator trap (it sets up the registers nicely).
 */
	.globl	_emul_execve
_emul_execve:
	mov	r0,r31
	or	l%SYS_execve,r0,r31	// exec system call number
	trap	r31,r31,r0
	bri	r1
	 addu	-1,r0,r16		// only if it failed...

#ifdef TNC

/*
 * Newly-arrived migrating task starts executing here with return values
 * in r16/r17, stack pointing to saved registers.
 */
	.globl	_migrate_arrival
_migrate_arrival:
	addu	-16,sp,sp
	st.l	r1,0(sp)		// push return address
	st.l	r16,4(sp)		// push r16
	st.l	r17,8(sp)		// push r17

	call	_migrate_init		// initialize emulator for child
	 st.l	r29,12(sp)		// push r29 (in shadow)
	ld.l	12(sp),r29		// pop r29
	ld.l	0(sp),r1		// pop r1
//	ld.l	4(sp),r16		// pop r16
	ld.l	8(sp),r17		// pop r17
	mov	r0, r16			// clear return value
	bri	r1			// return to inst. after the trap
	 addu	16,sp,sp		// unwind stack (in shadow)


/*
 * Newly-arrived rexec'ing emulator task starts executing here with
 * return values in r16/r17, stack pointing to saved registers.
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
	call	_rexecve_init
	 nop
	mov	r0,r31
	or	l%SYS_rexecve_arrival,r0,r31 // system call number for
					//      emulator-internal arrival fn
	trap	r31,r31,r0
	bri	r1
	 addu	-1,r0,r16		// only if it failed...

#endif	/* TNC */

/*
 * This is the sigreturn trampoline code used by the sigmigrate handler
 * internal to the emulator.
 */
	.globl	_emul_sigreturn
_emul_sigreturn:
	adds	-16,sp,sp
	st.l	r1,4(sp)
	calli	r19			// Call the signal handler
	 st.l	r18,0(sp)		// Save the sigcontext
	ld.l	0(sp),r16		// Put the signal context into
					// the proper register.
	mov	r0,r31
	or	l%SYS_sigreturn,r0,r31	// sigreturn system call number
	trap	r31,r31,r0
	ld.l	4(sp),r1
	adds	16,sp,sp
	bri	r1
	 addu	-1,r0,r16		// only if it failed...

#ifdef TNC

/*
 * This routine is used by the emulator's default SIGMIGRATE handler to
 * invoke the migrate/chkpnt_async system call. But since we're in the emulator
 * already we fake a system call as above.
 */
	.globl	_emul_migrate
_emul_migrate:
	adds	-16,sp,sp
	st.l	r1,0(sp)
	mov	r0,r31
	or	l%SYS_migrate,r0,r31	// migrate system call number
	trap	r31,r31,r0
	ld.l	0(sp),r1
	adds	16,sp,sp
	bri	r1
	 nop				// leave return code from migrate

#ifdef NX
#ifdef FORCE_MIGRATION	
        .globl  _emul_force_migrate
_emul_force_migrate:
        adds    -16,sp,sp
        st.l    r1,0(sp)
        mov     r0,r31
        or      l%SYS_force_migrate,r0,r31 // force_migrate system call number
        trap    r31,r31,r0
        ld.l    0(sp),r1
        adds    16,sp,sp
        bri     r1
         nop
#endif /* FORCE_MIGRATION */	
#endif /* NX */
#endif /* TNC */

 #
 # Trap_sigreturn() checks u_r0. If non zero, it asks the micro-kernel by 
 # invoking trap r0,r26,r0 instruction to restore the user state with the 
 # i860_thread_state structure. If zero, it goes to ret_sig and do the normal 
 # sigreturn.
_Trap_sigreturn::
 	ld.l	u_r0(sp), r16
	bte	0,r16,ret_sig
	trap	r0,r26,r0
