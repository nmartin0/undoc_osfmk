/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/* 
 * HISTORY
 * $Log: pcb.h,v $
 * Revision 1.4  1994/11/18  20:31:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:33  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:53  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:29  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:30  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/05  13:58:03  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.4  90/10/07  13:42:50  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:37:19  gm]
 * 
 * Revision 1.3  90/07/17  11:31:37  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:29:37  kevins]
 * 
 * 	Fix t_bitmapbase to be a short as advertised.
 * 	[90/04/11            rvb]
 * 
 * 	Revision 1.13  89/11/16  16:24:28  kupfer
 * 		Add workaround for Errata 17 for B1 step 80386s (coprocessor
 * 		instruction that spans page boundary).
 * 
 * 	Revision 1.12  89/11/02  14:15:38  kupfer
 * 		Try to rely less on magic numbers.  Resurrect pcb_synch.
 * 	[90/02/28  15:48:19  rvb]
 * 
 * 	Add Intel copyright.
 * 	[90/01/08            rvb]
 * 
 * 	Flush some dead fields in pcb_.
 * 	[89/11/21            rvb]
 * 
 * Revision 1.14  90/02/26  17:54:24  kupfer
 * 	Synch with CMU.  h/w workarounds ifdef'd by h/w type.
 * 
 * Revision 1.11  89/09/26  11:56:10  lance
 * 	X109 checkin
 * 
 * Revision 1.7  89/09/25  12:25:17  rvb
 * 	File was provided by Intel 9/18/89.
 * 	[89/09/23            rvb]
 * 
 * Revision 1.6  89/09/20  17:26:43  rvb
 * 	Revision 1.9  89/09/14  13:07:24  kupfer
 * 	Add some comments.  Also add prototypes for a couple functions in
 * 	pcb.c.
 * 
 * 	Revision 1.8  89/08/07  17:28:54  kupfer
 * 	Better handling of single-step traps.  Some reformatting.
 * 
 * 	Revision 1.7  89/07/07  17:55:26  kupfer
 * 	X79 merge, 2nd attempt.
 * 
 * Revision 1.5  89/07/17  10:38:15  rvb
 * 	Olivetti Changes to X79 upto 5/9/89:
 * 	[89/07/11            rvb]
 * 
 * Revision 1.4  89/03/09  20:03:18  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:32:53  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
 
/*
 *  Copyright 1988, 1989 by Intel Corporation
 *
 *         INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *     This software is supplied under the terms of a license 
 *    agreement or nondisclosure agreement with Intel Corpo-
 *    ration and may not be copied or disclosed except in
 *    accordance with the terms of that agreement.
 *    Copyright 1988 Intel Corporation.
 */

#ifndef	_PCB_
#define _PCB_

#include <sys/types.h>
#include <i386/fpreg.h>

/* Flags Register */

typedef struct tss386_flags {
	u_int	fl_cf	:  1,		/* carry/borrow */
			:  1,		/* reserved */
		fl_pf	:  1,		/* parity */
			:  1,		/* reserved */
		fl_af	:  1,		/* carry/borrow */
			:  1,		/* reserved */
		fl_zf	:  1,		/* zero */
		fl_sf	:  1,		/* sign */
		fl_tf	:  1,		/* trace */
		fl_if	:  1,		/* interrupt enable */
		fl_df	:  1,		/* direction */
		fl_of	:  1,		/* overflow */
		fl_iopl :  2,		/* I/O privilege level */
		fl_nt	:  1,		/* nested task */
			:  1,		/* reserved */
		fl_rf	:  1,		/* reset */
		fl_vm	:  1,		/* virtual 86 mode */
		fl_res	: 14;		/* reserved */
} tss386_flags_t;

#define PS_C		0x0001		/* carry bit			*/
#define PS_P		0x0004		/* parity bit			*/
#define PS_AC		0x0010		/* auxiliary carry bit		*/
#define PS_Z		0x0040		/* zero bit			*/
#define PS_N		0x0080		/* negative bit			*/
#define PS_T		0x0100		/* trace enable bit		*/
#define PS_IE		0x0200		/* interrupt enable bit		*/
#define PS_D		0x0400		/* direction bit		*/
#define PS_V		0x0800		/* overflow bit			*/
#define PS_IOPL		0x3000		/* I/O privilege level		*/
#define PS_NT		0x4000		/* nested task flag		*/
#define PS_RF		0x10000		/* Reset flag			*/
#define PS_VM		0x20000		/* Virtual 86 mode flag		*/

/*
 * Maximum I/O address that will be in TSS bitmap
 */
#define MAXTSSIOADDR	0x3ff

struct tss386 {
	u_long t_link;
	u_long t_esp0;
	u_long t_ss0;
	u_long t_esp1;
	u_long t_ss1;
	u_long t_esp2;
	u_long t_ss2;
	u_long t_cr3;
	u_long t_eip;
	u_long t_eflags;
	u_long t_eax;
	u_long t_ecx;
	u_long t_edx;
	u_long t_ebx;
	u_long t_esp;
	u_long t_ebp;
	u_long t_esi;
	u_long t_edi;
	u_long t_es;
	u_long t_cs;
	u_long t_ss;
	u_long t_ds;
	u_long t_fs;
	u_long t_gs;
	u_long t_ldt;
	u_short t_t;
	u_short t_bitmapbase;
};

/*
 * 386 TSS definition
 */

struct pcb {

	struct tss386 pcb_tss;

	/* 
	 * Software extensions.  Note that the fpvalid and fps fields 
	 * are assumed to be adjacent (see ldt_init()).
	 * XXX - It would[ be better if fpstate.state were a struct 
	 *       instead of a byte array, but I don't know anything 
	 *       about the '287 layout. -mdk
	 */
	struct pt_entry *pcb_cr3;	/* page directory pointer - CR3 */
	int	pcb_context[7];		/* save context for kernel */
	int	pcb_fpvalid;		/* saved fp state is valid */
	struct	fpstate {
		u_char	state[FP_STATE_BYTES];	
					/* 287/387 saved state; see fpreg.h */
		int	status;		/* status word at exception */
	} pcb_fps;
	int	pcb_flags;
	/* 
	 * Start of COPROC_SPANPAGE_BUG fields.  These aren't ifdef'd, 
	 * because user programs have no way of knowing whether the 
	 * kernel was config'd for that fix.
	 */
	int	pcb_last_uip;		/* prev. user IP at interrupt */
	int	pcb_ip_same;		/* number times IP was the same */
	/* End of COPROC_SPANPAGE_BUG fields. */
};


#define		PS_USER		3
#define		PS_KERNEL	0


/* 
 * Flags.
 * 
 * PSF_SINGLESTEP can be set if there is a debug trap in system mode.
 * The flag tells syscall() to set the single-step flag in the user's
 * flags register, so that the user will be put back into single-step
 * mode after returning from the syscall.
 */

#define PSF_SINGLESTEP	0x1


/* 
 * Public functions exported by pcb.c.
 */

extern void pcb_synch();

#if	defined(__STDC__) || defined(__GNUC__)
extern void pcb_init(/*struct thread *thread, vm_offset_t ksp*/);
extern void ldt_init(/*struct seg_desc *ldt, char *fpstart, int fpsize*/);
#endif	/* __STDC__ || __GNUC__ */

#endif	/* _PCB_ */
