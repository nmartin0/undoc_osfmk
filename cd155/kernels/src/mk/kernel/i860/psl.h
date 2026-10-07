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
 * $Log: psl.h,v $
 * Revision 2.7  1994/11/18  20:39:21  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1993/06/30  22:31:20  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.5  1992/09/22  17:20:05  SSD
 * *** empty log message ***
 *
 * Revision 2.5  91/12/10  16:31:00  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:32:53  jsb]
 * 
 * Revision 2.4  91/06/18  20:51:43  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:57:44  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:00  jsb
 * 	Corrected USERMODE definition; eliminated ptrace garbage.
 * 	[91/06/17  10:14:34  jsb]
 * 
 * Revision 2.2  90/12/04  14:49:13  jsb
 * 	First checkin.
 * 	[90/12/03  21:32:42  jsb]
 * 
 */

/*
 * i860 processor status register
 */

#define PSR_BR		0x00000001	/* break read			*/
#define PSR_BW		0x00000002	/* break write			*/
#define PSR_CC		0x00000004	/* condition code		*/
#define PSR_LCC		0x00000008	/* loop condition code		*/

#define PSR_IM		0x00000010	/* interrupt mode		*/
#define PSR_PIM		0x00000020	/* prev. interrupt mode		*/
#define PSR_U		0x00000040	/* user mode			*/
#define PSR_PU		0x00000080	/* prev. user mode		*/

#define PSR_IT		0x00000100	/* instruction trap		*/
#define PSR_IN		0x00000200	/* interrupt			*/
#define PSR_IAT		0x00000400	/* instruction access trap	*/
#define PSR_DAT		0x00000800	/* data access trap		*/
#define PSR_FT		0x00001000	/* floating-point trap		*/

#define PSR_DS		0x00002000	/* delayed switch		*/
#define PSR_DIM		0x00004000	/* dual instruction mode	*/
#define PSR_KNF		0x00008000	/* kill next float instr.	*/

#define USERMODE(psr)	(((psr) & PSR_PU) != 0)

/*
 *	i860 extended processor register
 */
#define EPSR_CPUTYPE	0x000000FF	/* processor type (mask) */
#define EPSR_CPUSTEP	0x00001F00	/* chip step number */
#define EPSR_IL		0x00002000	/* interlock */
#define EPSR_WP		0x00004000	/* write-protect mode */
#define EPSR_PEF	0x00008000	/* parity error flag (i860XP) */
#define EPSR_BEF	0x00010000	/* bus error flag (i860XP) */
#define EPSR_INT	0x00020000	/* value of the INT input pin */
#define EPSR_DCS	0x003C0000	/* data-cache size (mask) */
#define EPSR_PBM	0x00400000	/* page-table bit mode */
#define EPSR_BE		0x00800000	/* big-endian mode */
#define EPSR_OF		0x01000000	/* overflow flag */
#define EPSR_BS		0x02000000	/* BEF or PEF at suprv.level (i860XP) */
#define EPSR_DI		0x04000000	/* trap on delayed inst. (i860XP) */
#define EPSR_TAI	0x08000000	/* trap on autoincrement (i860XP) */
#define EPSR_PT		0x10000000	/* trap on pipeline use (i860XP) */
#define EPSR_PI		0x20000000	/* pipeline instruction (i860XP) */
#define EPSR_SO		0x40000000	/* strong ordering mode (i860XP) */
#define EPSR_rsvd	0x80000000	/* reserved */

/*
 *	i860 direct-ory pointer
 */
#define DIR_ATE		0x00000001	/* address translation enable	*/
#define DIR_ITI		0x00000020	/* tlb & i-cache invalidate */

/*
 *	i860 floating-point status register
 */
#define FSR_FZ		0x00000001	/* flush zero */
#define FSR_TI		0x00000002	/* trap inexact */
#define FSR_RM		0x0000000C	/* rounding mode (mask) */
#define FSR_U		0x00000010	/* update */
#define FSR_FTE		0x00000020	/* floating-point trap enable */
#define FSR_rsrvd0	0x00000040	/* reserved */
#define FSR_SI		0x00000080	/* sticky inexact flag */
#define FSR_SE		0x00000100	/* source exception */
#define FSR_MU		0x00000200	/* multiplier underflow */
#define FSR_MO		0x00000400	/* multiplier overflow */
#define FSR_MI		0x00000800	/* multiplier inexact */
#define FSR_MA		0x00001000	/* multiplier add one */
#define FSR_AU		0x00002000	/* adder underflow */
#define FSR_AO		0x00004000	/* adder overflow */
#define FSR_AI		0x00008000	/* adder inexact */
#define FSR_AA		0x00010000	/* adder add one */
#define FSR_RR		0x003E0000	/* result register (mask) */
#define FSR_AE		0x01C00000	/* adder exponent (mask) */
#define FSR_rsrvd1	0x02000000	/* reserved */
#define FSR_LRP		0x04000000	/* load pipe result precision */
#define FSR_IRP		0x08000000	/* integer pipe result precision */
#define FSR_MRP		0x10000000	/* multiplier pipe result precision */
#define FSR_ARP		0x20000000	/* adder pipe result precision */
#define FSR_rsrvd2	0x40000000	/* reserved */
#define FSR_rsrvd3	0x80000000	/* reserved */
