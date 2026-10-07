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
 * $Log: thread_status.h,v $
 * Revision 1.6  1994/11/18  20:54:49  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/06/30  22:48:51  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:40:13  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:41:35  dleslie
 * First R1_0 release
 *
 * Revision 2.6  1993/02/09  17:55:27  stans
 * norma13.16 (SSD T8 version).
 *
 * Revision 2.5.1.1  92/09/15  17:55:43  jeffreyh
 * 	Cleanup from Intel
 * 	[92/09/15            jeffreyh]
 * 
 * Revision 2.5  91/12/11  08:41:48  jsb
 * 	Another fix from Intel.
 * 	[91/12/11  08:40:08  jsb]
 * 
 * Revision 2.4  91/05/14  16:53:42  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:32:55  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:11:31  mrt]
 * 
 * Revision 2.2  90/12/04  14:51:16  jsb
 * 	First checkin.
 * 	[90/12/03  22:04:00  jsb]
 * 
 */
/*
 * Machine dependent thread state definitions for i860.
 * Derived from i386 version.
 */

#ifndef	_MACH_I860_THREAD_STATUS_H_
#define _MACH_I860_THREAD_STATUS_H_

#define i860_THREAD_STATE	(1)
#define i860_PIPE_STATE		(2)
#define i860_XP_STATE		(3)

#ifdef	NOT_YET
/*
 *	Pipeline registers and status.  This is always saved on the i860XR,
 *	and is asynchronously demand scheduled (or will be! :^) on the i860XP.
 *	The i860XP can be instructed to trap on pipeline operations whereas
 *	the i860XR cannot.
 *	Other important information (dual-instruction, dual-operation,
 *	precision info, etc.) can be found in copies of the psr and fsr.
 */
struct i860_pipe_state {
	double		ps_pfld[6];	/* floating-point load pipe */
	double		ps_pfadd[3];	/* adder pipe */
	double		ps_pfmul[3];	/* multiplier (2 for dp, 3 for sp) */
	double		ps_kr;		/* multiplier intermediate value */
	double		ps_ki;		/*     "            "        "   */
	double		ps_t;		/* adder intermediate value      */
	double		ps_merge;	/* special purpose graphics reg  */
	unsigned	ps_psr;		/* copy of the psr */
	unsigned	ps_fsr;		/* copy of the fsr */
};


/*
 *	Additional state information (possibly) needed for the i860XP.
 *	This includes the bus error address register, the concurrency
 *	registers, and the four supervisor-only registers.
 *	These registers do not exist on an i860XR.
 */
struct i860xp_state {
	unsigned	xp_p[4];	/* supervisor-only registers */
	unsigned	xp_bear;	/* bus-error address register */
	unsigned	xp_ccr;		/* concurrency control register */
	unsigned	xp_newcurr;	/* ccu stuff */
	unsigned	xp_stat;	/* ccu stuff */
};
#endif	/* NOT_YET */


/*
 *
 *	i860_thread_state	this is the structure that is exported
 *				to user threads for use in status/mutate
 *				calls.  This structure should never
 *				change.
 *
 * i860_thread_state offsets are defined in <i860/reg.h>. Make sure they all
 * match!
 */

struct i860_thread_state {
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
        double          spc_t;
        double          spc_ki;
        double          spc_kr;
        double          spc_merge;
        double          psv_l1[2];
        double          psv_l2[2];
        double          psv_l3[2];
        double          psv_a1;
        double          psv_a2;
        double          psv_a3;
        double          psv_m1;
        double          psv_m2;
        double          psv_m3;
        double          psv_i1;
        unsigned int    fsr1;
        unsigned int    fsr2;
        unsigned int    fsr;		/* 3rd FP pipe stage */
        unsigned int    psr;
	unsigned int	pc;	/* fir */
	unsigned int	dirbase;
	unsigned int	db;
        unsigned int    epsr;
        double          fp1;	/* operands for USER mode IEEE FP handler */
        double          fp2;
        double          fp3;
        double          fp4;
        unsigned int	fpe_trapped_op;	/* FP source error opcode */
        unsigned int	fpe_rdest;
        unsigned int	fpe_src1;
        unsigned int	fpe_ieee_status;
};

#define i860_THREAD_STATE_COUNT	(sizeof(struct i860_thread_state)/sizeof(unsigned int))
#define i860_PIPE_STATE_COUNT	(sizeof(struct i860_pipe_state)/sizeof(unsigned int))
#define i860_XP_STATE_COUNT	(sizeof(struct i860xp_state)/sizeof(unsigned int))

#endif	_MACH_I860_THREAD_STATUS_H_
