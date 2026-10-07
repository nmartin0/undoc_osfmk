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
 * HISTORY
 * $Log: thread.h,v $
 * Revision 2.13  1994/11/18  20:39:47  mtm
 * Copyright additions/changes
 *
 * Revision 2.12  1994/03/19  01:03:56  lenb
 *  Reviewer: andyp
 *  Risk: low
 *  Benefit: lint fix from andyp
 *  Testing: SAT
 *
 * Revision 2.11.8.1  1994/02/04  07:45:54  andyp
 * Workaround norma2 growing pains.  Remove an ANSI trigraph sequence
 * from thread.h that annoyed lint.
 *
 * Revision 2.11  1993/06/30  22:31:36  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.10  1992/11/20  20:02:28  andyp
 * Glue for the new FPE handler.
 *
 * Revision 2.9  1992/10/15  22:52:37  terry
 * changes to state save area for increased memory pipe line
 * sizes in do to quad word fetches.
 *
 * Revision 2.8  1992/10/08  21:01:56  terry
 * added user_pc() macro for profiling support.  Also added
 * PCB_SIGNAL_PFLDQ for pcb flags.
 *
 * Revision 2.7  1992/09/22  17:19:41  SSD
 * *** empty log message ***
 *
 * Revision 2.6.2.2  92/05/27  00:41:50  jeffreyh
 * 	Support floating point exceptions & flag bits in pcb struct.
 * 	[stans@ssd.intel.com]
 * 
 * Revision 2.6.2.1  92/03/28  10:08:17  jeffreyh
 * 	Pick up changes from MK71
 * 	[92/03/20            jeffreyh]
 * 
 * Revision 2.7  92/03/03  14:22:46  rpd
 * 	Added dummy definition of syscall_emulation_sync.
 * 	[92/03/03            rpd]
 * 
 * Revision 2.6  91/12/10  16:31:11  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:00  jsb]
 * 
 * Revision 2.5  91/08/28  11:12:27  jsb
 * 	From Intel SSD: changed pcb definition.
 * 	[91/08/26  16:38:06  jsb]
 * 
 * Revision 2.4  91/06/18  20:51:54  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:59:12  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:12  jsb
 * 	From Paul Pierce: modified for new trap handler.
 * 	[91/06/17  13:38:29  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:29  jsb
 * 	First checkin.
 * 	[90/12/03  21:34:23  jsb]
 * 
 */
/*
 *	File:	machine/thread.h
 *
 *	Author: Andy Pfiffer/SSD Intel
 *
 *	This file contains the structure definitions for the thread
 *	state as applied to i860 processors.
 *
 *	Complete overhaul for handoff scheduling and hooks for i860XP.
 */

#ifndef	_I860_THREAD_H_
#define _I860_THREAD_H_

#include <mach/boolean.h>
#include <mach/machine/vm_types.h>

/*
 *	Kernel state.  Saved and restored across context-switches
 *	inside the kernel.  We can ignore caller-saved registers.
 *	Kept at the base of the thread's stack.
 *
 *	reproduced from 11-1 of "i860 Microprocessor Family Programmer's
 *	Reference Manual (1991)":
 *
 *	Register	Purpose				Unchanged by Subroutine?
 *	r0		Always zero			Yes
 *	r1		Return address			No
 *	r2		Stack pointer			Yes*
 *	r3		Frame pointer			Yes
 *	r4-r15		Local values			Yes
 *	r16-r17		Return value			No
 *	r16-r27		Parameters and temporaries	No
 *	r28		Memory parameter pointer	No
 *	r29		Environment pointer		No
 *	r28-r30		Temporaries			No
 *	r31		Addressing temporary		No
 *	f0-f1		Always zero			Yes
 *	f2-f7		Local values			Yes
 *	f8-f15		Return value			No
 *	f8-f15		Parameters and temporaries	No
 *	f16-f31		Temporaries			No
 *
 *	NOTE:
 *	*The stack pointer is normally kept unchanged across a subroutine
 *	 call.  However, some subroutines may allocate stack space and
 *	 return with a different value in r2.
 */

struct i860_kernel_state {
	unsigned	ks_r[16];	/* non-volatile integer registers */
	unsigned	ks_f[8];	/* non-volatile fp registers */
#define	ks_r0	ks_r[0]
#define	ks_r1	ks_r[1]
#define	ks_sp	ks_r[2]
#define	ks_fp	ks_r[3]
#define	ks_r4	ks_r[4]
#define	ks_r5	ks_r[5]
#define	ks_r6	ks_r[6]
#define	ks_r7	ks_r[7]
#define	ks_r8	ks_r[8]
#define	ks_r9	ks_r[9]
#define	ks_r10	ks_r[10]
#define	ks_r11	ks_r[11]
#define	ks_r12	ks_r[12]
#define	ks_r13	ks_r[13]
#define	ks_r14	ks_r[14]
#define	ks_r15	ks_r[15]
#define ks_f0	ks_f[0]
#define ks_f1	ks_f[1]
#define ks_f2	ks_f[2]
#define ks_f3	ks_f[3]
#define ks_f4	ks_f[4]
#define ks_f5	ks_f[5]
#define ks_f6	ks_f[6]
#define ks_f7	ks_f[7]
};


/*
 *	Saved state.  Holds the state of user registers upon kernel entry
 *	from user mode.  It lives in the pcb.  It is pushed onto the stack
 *	for exceptions taken in kernel mode.
 */
struct i860_saved_state {
	unsigned int	r0;
	unsigned int	r1;
	unsigned int	sp;
	unsigned int	fp;
	unsigned int	r4;
	unsigned int	r5;
	unsigned int	r6;
	unsigned int	r7;
	unsigned int	r8;
	unsigned int	r9;
	unsigned int	r10;
	unsigned int	r11;
	unsigned int	r12;
	unsigned int	r13;
	unsigned int	r14;
	unsigned int	r15;
	unsigned int	r16;
	unsigned int	r17;
	unsigned int	r18;
	unsigned int	r19;
	unsigned int	r20;
	unsigned int	r21;
	unsigned int	r22;
	unsigned int	r23;
	unsigned int	r24;
	unsigned int	r25;
	unsigned int	r26;
	unsigned int	r27;
	unsigned int	r28;
	unsigned int	r29;
	unsigned int	r30;
	unsigned int	r31;
	unsigned int	f0;
	unsigned int	f1;
	unsigned int	f2;
	unsigned int	f3;
	unsigned int	f4;
	unsigned int	f5;
	unsigned int	f6;
	unsigned int	f7;
	unsigned int	f8;
	unsigned int	f9;
	unsigned int	f10;
	unsigned int	f11;
	unsigned int	f12;
	unsigned int	f13;
	unsigned int	f14;
	unsigned int	f15;
	unsigned int	f16;
	unsigned int	f17;
	unsigned int	f18;
	unsigned int	f19;
	unsigned int	f20;
	unsigned int	f21;
	unsigned int	f22;
	unsigned int	f23;
	unsigned int	f24;
	unsigned int	f25;
	unsigned int	f26;
	unsigned int	f27;
	unsigned int	f28;
	unsigned int	f29;
	unsigned int	f30;
	unsigned int	f31;
	double		spc_t;
	double		spc_ki;	
	double		spc_kr;
	double		spc_merge;
	double		psv_l1[2];
	double		psv_l2[2];
	double		psv_l3[2];
	double		psv_a1;
	double		psv_a2;
	double		psv_a3;
	double		psv_m1;
	double		psv_m2;
	double		psv_m3;
	double		psv_i1;
	unsigned int	fsr1;
	unsigned int	fsr2;
	unsigned int	fsr;
	unsigned int	psr;
	unsigned int	pc;	/* fir */
	unsigned int	dirbase;
	unsigned int	db;
	unsigned int	epsr;
	double		fp1;
	double		fp2;
	double		fp3;
	double		fp4;
	unsigned int	trapped_opcode;
	unsigned int	rdest;
	unsigned int	src1;
	unsigned int	save_r13;
	unsigned int	save_r14;
	unsigned int	save_r15;
	unsigned int	krki;
	unsigned int	trapno;
	unsigned int	vaddr;
	unsigned int	code;
};


/*
 *	At the high end of a kernel stack is an "exception link."
 *	It contains the C calling sequence's argument save area.
 *	It also contains a pointer to the exception frame (i860_saved_state).
 *	If the exception happened in user mode, then the exception frame
 *	is in the thread's pcb.  If the exception happened in kernel mode,
 *	then the exception frame is on the kernel stack.
 */
struct i860_exception_link {
	struct i860_saved_state *eframe;
};

/*
 *	On the kernel stack is:
 *	stack[KERNEL_STACK_SIZE]:
 *		struct i860_kernel_state
 *		struct i860_exception_link
 *	stack[KERNEL_STACK_SIZE-I860_STACK_COOKIES]:
 *		.
 *		.
 *		. 
 *	stack[2]:
 *	stack[1]:
 *	stack[0]:
 *
 *	The first two structures are later referred to as "stack cookies"
 *	in pcb.c.
 *
 *	*** WARNING ***
 *	pcb are assumed to be 16-byte aligned!!
 */
typedef struct pcb {
	struct i860_saved_state		i860ss;
	unsigned			flags;
	unsigned			ieee_fp_mask;
	double				pfldq_pipe[6];  /* 3 quad-words */
#if	NEWFPE
	unsigned long			ieee_status;
	double				fp5;
	double				fp6;
	unsigned long			r_fsr;
#endif	NEWFPE
} *pcb_t;	/* exported */

/*
 * flag bits in pcb->flags
 */
#define	PCB_IEEE_FP	(1<<0)	/* 1 == signal only IEEE FP exceptions, default.
				 * 0 == otherwise, signal all FP exceptions.
				 */

#define	PCB_SIGNAL_MA	(1<<1)	/* 1 == signal misaligned accesses
				 * 0 == perform ld/st quietly.
				 */

#define	PCB_SIGNAL_PFLDQ	(1<<2)	/* 1 == signal pfld.q fetches
					 * 0 == perform pfld.q quietly.
					 */


#define USER_REGS(th)	(&(th)->pcb->i860ss)

#define STACK_I860KS(stack)	\
	((struct i860_kernel_state *)((stack) + KERNEL_STACK_SIZE) - 1)
#define STACK_I860EL(stack)	\
	((struct i860_exception_link *)STACK_I860KS(stack) - 1)


/*
 *	Routine definitions
 */
#include <mach/kern_return.h>

void		pcb_init(), pcb_terminate(), pcb_collect();
kern_return_t	thread_setstatus(), thread_getstatus();

#define	user_pc(thread)		(USER_REGS(thread)->pc)
 
#define syscall_emulation_sync(task)	/* do nothing */

#endif	_I860_THREAD_H_
