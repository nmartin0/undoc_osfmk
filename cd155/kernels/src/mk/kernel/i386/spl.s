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
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * 09-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Do not use this setsoftclock entry for MPs.
 *
 * $Log: spl.s,v $
// Revision 1.5  1994/11/18  20:34:02  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  22:25:10  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:23:47  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  18:23:41  dleslie
// First R1_0 release
//
 * Revision 2.8.8.3  92/09/15  17:14:57  jeffreyh
 * 	Changes for profiling of kernel task & threads.
 * 	Arrange so that profiling is accurate even when
 * 	masked against the clock (ie: let clock interrupt
 * 	raise but deffer the calll to mach_clock()). Also
 * 	Let "sti" be last codeop before "ret" so that
 * 	caller gets sampled when lowering ipl level. interrupts
 * 	usually pop at sti+2 instructions.
 * 	[92/07/24            bernadat]
 * 
 * Revision 2.8.8.2  92/04/30  11:51:06  bernadat
 * 	Adaptations for Corollary and Systempro
 * 	[92/04/08            bernadat]
 * 
 * Revision 2.8.8.1  92/02/18  18:49:36  jeffreyh
 * 	Fixed slave interrupt code for new interrupt interface
 * 	Must not overwrite %ecx in handler
 * 	[91/08/22            bernadat]
 * 
 * 	Support for the Corollary MP
 * 	[91/06/25            bernadat]
 * 
 * Revision 2.8  91/06/06  17:04:01  jsb
 * 	Added spldcm for i386ipsc.
 * 	[91/05/13  16:53:38  jsb]
 * 
 * Revision 2.7  91/05/14  16:16:41  mrt
 * 	Correcting copyright
 * 
 * Revision 2.6  91/05/08  12:42:29  dbg
 * 	Put parentheses around substituted immediate expressions, so
 * 	that they will pass through the GNU preprocessor.
 * 
 * 	Add set_spl_noi to reset PIC masks but leave interrupts disabled
 * 	(IF clear).
 * 	[91/04/26  14:38:31  dbg]
 * 
 * Revision 2.5  91/02/05  17:14:45  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:38:06  mrt]
 * 
 * Revision 2.4  90/12/20  16:36:50  jeffreyh
 * 	Changes for __STDC__
 * 	[90/12/07            jeffreyh]
 * 
 * Revision 2.3  90/11/26  14:48:50  rvb
 * 	Change Prime copyright as per Peter J. Weyman authorization.
 * 	[90/11/19            rvb]
 * 
 * Revision 2.2  90/05/03  15:37:36  dbg
 * 	Stole from Prime.
 * 	[90/02/14            dbg]
 * 
 */

/*
Copyright (c) 1988,1989 Prime Computer, Inc.  Natick, MA 01760
All Rights Reserved.

Permission to use, copy, modify, and distribute this
software and its documentation for any purpose and
without fee is hereby granted, provided that the above
copyright notice appears in all copies and that both the
copyright notice and this permission notice appear in
supporting documentation, and that the name of Prime
Computer, Inc. not be used in advertising or publicity
pertaining to distribution of the software without
specific, written prior permission.

THIS SOFTWARE IS PROVIDED "AS IS", AND PRIME COMPUTER,
INC. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  IN
NO EVENT SHALL PRIME COMPUTER, INC.  BE LIABLE FOR ANY
SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY
DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR
OTHER TORTIOUS ACTION, ARISING OUR OF OR IN CONNECTION
WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#include <cpus.h>
#include <mach_prof.h>
#include <platforms.h>
#include <i386/asm.h>
#include <i386/ipl.h>
#include <i386/pic.h>

#if	NCPUS > 1 && AT386

#include <i386at/mp/mp.h>
#define	CX(addr,reg)	addr(,reg,4)

#else	NCPUS > 1 && AT386

#define	CPU_NUMBER(reg)
#define	CX(addr,reg)	addr

#endif	NCPUS > 1 && AT386

///////////////////////////////////////////////////////////////////////////////
/	SET_PIC_MASK : this routine is run to set the pic masks.  It is 
/	implemented as a macro for efficiency reasons.
///////////////////////////////////////////////////////////////////////////////

#define SET_PIC_MASK							\
		movl	EXT(master_ocw),%edx	;			\
		OUTB				;			\
		addw	$(SIZE_PIC),%dx		;			\
		movb	%ah,%al			;			\
		OUTB

///////////////////////////////////////////////////////////////////////////////
/	SPLn : change interrupt priority level to that in %eax
/	SPLOFF : disable all interrupts, saving interrupt flag
/	SPLON: re-enable interrupt flag, undoes sploff()
/	Warning: SPLn and SPLOFF aren`t nestable.  That is,
/		a = sploff();
/		...
/		b = splmumble();
/		...
/		splx(b);
/		...
/		splon(a);
/	is going to do the wrong thing.
///////////////////////////////////////////////////////////////////////////////

ENTRY(spl0)
	movl    $(SPL0), %eax
	jmp	EXT(set_spl)

ENTRY(spl1)
ENTRY(splsoftclock)
	movl    $(SPL1), %eax
	jmp	EXT(set_spl)

ENTRY(spl2)
	movl    $(SPL2), %eax
	jmp	EXT(set_spl)

ENTRY(spl3)
	movl    $(SPL3), %eax
	jmp	EXT(set_spl)

ENTRY(splnet)
ENTRY(splhdw)
ENTRY(spl4)
	movl    $(SPL4), %eax
	jmp	EXT(set_spl)

ENTRY(splbio)
ENTRY(spldcm)
ENTRY(spl5)
	movl    $(SPL5), %eax
	jmp	EXT(set_spl)

ENTRY(spltty)
ENTRY(spl6)
	movl    $(SPL6), %eax
	jmp	EXT(set_spl)

ENTRY(splclock)
ENTRY(splimp)
ENTRY(splvm)
ENTRY(splsched)

#if	MACH_PROF
	/*
	 * If profiling is on, let clock interrupts raise even when mask
	 * against it
	 */

	movl	$(SPL7), %eax
	jmp	EXT(set_spl)
#endif	MACH_PROF


ENTRY(splhigh)
ENTRY(splhi)
	cli				/  3 / disable interrupts
	CPU_NUMBER(%edx)
	movl	CX(_curr_ipl, %edx), %eax
	movl	$(IPLHI), CX(_curr_ipl, %edx) 
	ret				/ 10 /
/					------
/					  19

ENTRY(sploff)				/  7 /
	pushf				/  2 / Flags reg NOT accessable
	popl	%eax			/  2 / push onto stk, pop it into reg.
	cli				/  3 / DISABLE ALL INTERRUPTS
	ret				/  7 /
/					------
/					  21

ENTRY(splon)				/  7 /
	pushl	4(%esp)		/  4 / Flags regs not directly settable.
	popf				/  4 / push value, pop into flags reg.
					/      IF ints were enabled before 
					/	then they are re-enabled now.
	ret				/  7 /
/					------
/					  22

#if	MACH_PROF
/*
 * If profiling is on, we let clock interrupts happen at any time
 * (except splhi) but then check if we are masked against it.
 * If yes we just memorize the PC so that  profiling is accurate.
 *  So at splx time we must call the
 * real clock handler (mach_clock) if this happened
 */

#define	CHECK_MISSED_CLOCK(cpu_reg)			\
	cmpl	$SPL7, %eax			;\
	jge	0f				;\
	cmpl	$0, CX(_missed_clock, cpu_reg)	;\
	je	0f				;\
	pushl	%edx				;\
	pushl	%eax				;\
	pushl	%ecx				;\
	call	EXT(delayed_clock)		;\
	popl	%ecx				;\
	popl	%eax				;\
	popl	%edx				;\
0:
#else	MACH_PROF
#define CHECK_MISSED_CLOCK(cpu_reg)
#endif	MACH_PROF

////////////////////////////////////////////////////////////////////////////////
/	 SPL : this routine sets the interrupt priority level, it is called from
/	 one of the above spln subroutines ONLY, NO-ONE should EVER call set_spl
/	 directly as it assumes that the parameters passed in are gospel.
/	SPLX	: This routine is used to set the ipl BACK to a level it was at
/	before spln() was called, which in turn calls set_spl(), which returns
/	(via %eax) the value of the curr_ipl prior to spln() being called, this 
/	routine is passed this level and so must check that it makes sense and 
/	if so then simply calls set_spl() with the appropriate interrupt level.
///////////////////////////////////////////////////////////////////////////////

ENTRY(splx)
	movl	0x04(%esp),%eax		/  4 / get interrupt level from stack

	cmpl	$0x00,%eax		/  2 / check if  < 0
	jl	splxpanic		/  3 /

	cmpl	$(SPLHI),%eax		/  2 / check if too high
	ja	splxpanic		/  3 /
/					------
/					  14

///////////////////////////////////////////////////////////////////////////////
/	SET_SPL : This routine sets curr_spl, ipl, and the pic_mask as 
/	appropriate, basically given an spl# to adopt, see if it is the same as
/	the  old spl#, if so return. If the numbers are different, then set 
/	curr_spl, now check the corresponding ipl for the spl requested, if they
/	are the same then return otherwise set the new ipl and set the pic masks
/	accordingly.
///////////////////////////////////////////////////////////////////////////////

	.globl	EXT(set_spl)
LEXT(set_spl)

	pushl	%ebp			/ 2 /
	movl	%esp,%ebp		/ 2 /
	cli				/ 3 / disable interrupts
	CPU_NUMBER(%edx)
	CHECK_MISSED_CLOCK(%edx)
#if	NCPUS > 1 && AT386
	cmpl	$0, %edx
	jne	slave_set_spl
#endif	NCPUS > 1 && AT386
	movl	EXT(curr_ipl), %edx	/ 4 / get OLD ipl level
	pushl	%edx			/ 2 / save old level for return
set_spl_recur:
	movl	%eax,EXT(curr_ipl)	/ 4 / set NEW ipl level
	cmpl	$(SPLHI), %eax		/  2 / if SPLHI
	jne	1f
	popl	%eax			/ 2 / return old level
	pop	%ebp			/ 2 /
	ret				/10 / return with interrupt off
1:
	cmpl	$(SPL0), %eax		/ if SPL0
	jne	2f			/ and
	cmpl	$0,EXT(dotimein)	/ softclock request,
	jz	2f			/ take a softclock interrupt first
	call	do_soft_clock
	jmp	set_spl_recur

2:
	movw	EXT(pic_mask)(,%eax,2), %ax / 5 /
	cmpw	EXT(curr_pic_mask),%ax 	/ 5 /
	je	spl_return		/ 7 /
	movw	%ax, EXT(curr_pic_mask)
	SET_PIC_MASK			/16 /
.globl	_spl_return
_spl_return:
spl_return:
	popl	%eax			/ 2 / return old level
	pop	%ebp			/ 2 /
	sti				/ 3 /
	ret				/10 /
/					-----
/				         79

#if	NCPUS > 1 && AT386
slave_set_spl:
	xchgl	CX(_curr_ipl, %edx), %eax
	cmpl	$(MP_IPL), CX(_curr_ipl, %edx)
	jl	slave_spl_return
	pop	%ebp
	ret				/ return with interrupt masked
slave_spl_return:
	pop	%ebp
	sti
	ret
#endif	NCPUS > 1 && AT386


splxpanic:
#if	NCPUS > 1 && AT386
	CPU_NUMBER(%edx)
	shl	$2, %edx
	pushl	EXT(curr_ipl)(%edx)
#else	NCPUS > 1 && AT386
	pushl	EXT(curr_ipl)		/ current level
#endif	NCPUS > 1 && AT386
	pushl	%eax			/ new level
	pushl	$splxpanic2
	call	EXT(panic)
	hlt

	.data
splxpanic2:
	String	"splx(old %x, new %x): logic error in locore.s\n"
	.byte	0
	.text

/*
 * Take a softclock interrupt request.
 */
do_soft_clock:
	movl	$0,EXT(dotimein)	/	clear softclock request
	movl	$(SPL1),%eax
	call	EXT(set_spl)		/	run at spl1
	call	EXT(softclock)		/	run softclock interrupt
	movl	$(SPL0),%eax		/	return to spl0
	cli				/	disable interrupts
	ret

#if	NCPUS == 1
ENTRY(setsoftclock)
	movl	$1,EXT(dotimein)	/ request soft-clock interrupt
	ret
#endif	NCPUS == 1

	.data
	.globl	EXT(dotimein)
LEXT(dotimein)
	.long	0
	.text

/*
 * Set SPL, but leave interrupts disabled.  Called when returning
 * from interrupt.  Interrupts are already disabled.
 * New interrupt level is in %eax;
 * can't be SPLHI.
 */
	.globl	EXT(set_spl_noi)
LEXT(set_spl_noi)
	CPU_NUMBER(%edx)
	CHECK_MISSED_CLOCK(%edx)
#if	NCPUS > 1 && AT386 
	CPU_NUMBER(%edx)
	cmpl	$0, %edx
	jne	slave_set_spl_noi
#endif	NCPUS > 1 && AT386
	movl	%eax,EXT(curr_ipl)	/ set new SPL level
	cmpl	$(SPL0),%eax		/ if SPL0
	jne	0f			/ and
	cmpl	$0,EXT(dotimein)	/ softclock request,
	jz	0f			/ take a softclock interrupt first
	call	do_soft_clock
	jmp	EXT(set_spl_noi)
0:
	movw	EXT(pic_mask)(,%eax,2), %ax	/ get new pic mask
	cmpw	EXT(curr_pic_mask),%ax	/ if different,
	je	1f
	movw	%ax,EXT(curr_pic_mask)	/ change it
	SET_PIC_MASK
1:
	ret

slave_set_spl_noi:
	shl	$2, %edx
	addl	$EXT(curr_ipl), %edx
	movl	%eax, (%edx)
	ret








