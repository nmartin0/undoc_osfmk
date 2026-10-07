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
 * $Log: reg.h,v $
 * Revision 2.7  1994/11/18  20:39:25  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1993/06/30  22:31:22  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.5  1992/10/15  22:51:07  terry
 * changes to state save area for increased memory pipe line
 * sizes in do to quad word fetches.
 *
 * Revision 2.3.1.1  92/09/15  17:55:56  jeffreyh
 * 	Major cleanup. Needs other thread files to match.
 * 	[92/09/15            stans@ssd.intel.com]
 * 
 * Revision 2.3  91/06/18  20:51:45  jsb
 * 	New copyright from Intel. Fixed i860REGCOUNT definition.
 * 	[91/06/18  18:58:15  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:16  jsb
 * 	First checkin.
 * 	[90/12/03  21:32:59  jsb]
 * 
 */

#ifndef _SYS_REG_H
#define _SYS_REG_H

/*
 * These longword offsets reflect the user's view of a thread state
 * (thread_{get/set}_status() calls. Hence the offsets MUST match what's
 * defined in mach/i860/thread_status.h
 *
 *  Note that all the following defines must be a multiple of 4
 *	to guarantee that the offset will always be aligned properly
 */

#define IREGS	(0)		/* Offset of Integer-register set */

#define R0	(IREGS+0)
#define R1	(IREGS+1)
#define SP	(IREGS+2)
#define FP	(IREGS+3)
#define R4	(IREGS+4)
#define R5	(IREGS+5)
#define R6	(IREGS+6)
#define R7	(IREGS+7)
#define R8	(IREGS+8)
#define R9	(IREGS+9)
#define R10	(IREGS+10)
#define R11	(IREGS+11)
#define R12	(IREGS+12)
#define R13	(IREGS+13)
#define R14	(IREGS+14)
#define R15	(IREGS+15)
#define R16	(IREGS+16)
#define R17	(IREGS+17)
#define R18	(IREGS+18)
#define R19	(IREGS+19)
#define R20	(IREGS+20)
#define R21	(IREGS+21)
#define R22	(IREGS+22)
#define R23	(IREGS+23)
#define R24	(IREGS+24)
#define R25	(IREGS+25)
#define R26	(IREGS+26)
#define R27	(IREGS+27)
#define R28	(IREGS+28)
#define R29	(IREGS+29)
#define R30	(IREGS+30)
#define R31	(IREGS+31)

#define FREGS	(IREGS+32)

#define F0	(FREGS+0)
#define F1	(FREGS+1)
#define F2	(FREGS+2)
#define F3	(FREGS+3)
#define F4	(FREGS+4)
#define F5	(FREGS+5)
#define F6	(FREGS+6)
#define F7	(FREGS+7)
#define F8	(FREGS+8)
#define F9	(FREGS+9)
#define F10	(FREGS+10)
#define F11	(FREGS+11)
#define F12	(FREGS+12)
#define F13	(FREGS+13)
#define F14	(FREGS+14)
#define F15	(FREGS+15)
#define F16	(FREGS+16)
#define F17	(FREGS+17)
#define F18	(FREGS+18)
#define F19	(FREGS+19)
#define F20	(FREGS+20)
#define F21	(FREGS+21)
#define F22	(FREGS+22)
#define F23	(FREGS+23)
#define F24	(FREGS+24)
#define F25	(FREGS+25)
#define F26	(FREGS+26)
#define F27	(FREGS+27)
#define F28	(FREGS+28)
#define F29	(FREGS+29)
#define F30	(FREGS+30)
#define F31	(FREGS+31)

#define SPREGS	(FREGS+32)

#define SPC_T		(SPREGS+0)
#define SPC_KI		(SPREGS+2)
#define SPC_KR		(SPREGS+4)
#define SPC_MERGE 	(SPREGS+6)

#define PSVREGS		(SPREGS+8)

#define PSV_L1		(PSVREGS+0)
#define PSV_L2		(PSVREGS+4)
#define PSV_L3		(PSVREGS+8)
#define PSV_A1		(PSVREGS+12)
#define PSV_A2		(PSVREGS+14)
#define PSV_A3		(PSVREGS+16)
#define PSV_M1		(PSVREGS+18)
#define PSV_M2		(PSVREGS+20)
#define PSV_M3		(PSVREGS+22)
#define PSV_I1		(PSVREGS+24)

/* longword fsr registers */
#define PSV_FSR1	(PSVREGS+26)
#define PSV_FSR2	(PSVREGS+27)
#define PSV_FSR3	(PSVREGS+28)
#define PSV_FSR		PSV_FSR3
#define FSR		PSV_FSR3

#define	EXCREGS		(PSVREGS+29)

#define PSR		(EXCREGS+0)
#define PC		(EXCREGS+1)	/* fir */
#define DIRBASE		(EXCREGS+2)	/* dirbase */
#define DB		(EXCREGS+3)
#define EPSR		(EXCREGS+4)
#define FPE_FP1		(EXCREGS+5)	/* USER mode IEEE FP handler args */
#define FPE_FP2		(EXCREGS+7)	/* FPE_FPx are size double */
#define FPE_FP3		(EXCREGS+9)
#define FPE_FP4		(EXCREGS+11)
#define FPE_TRAPPED_OP	(EXCREGS+13)	/* FP source error opcode */
#define FPE_RDEST	(EXCREGS+14)
#define FPE_SRC1	(EXCREGS+15)
#define FPE_IEEE_STATUS	(EXCREGS+16)

#define	i860REGCOUNT	(FPE_IEEE_STATUS+2)

#endif	_SYS_REG_H
