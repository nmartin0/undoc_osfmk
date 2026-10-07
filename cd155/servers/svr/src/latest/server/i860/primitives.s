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
 * $Log: primitives.s,v $
 *Revision 1.5  1994/11/18  20:31:59  mtm
 *Copyright additions/changes
 *
 *Revision 1.4  1993/07/14  17:59:27  cfj
 *OSF/1 AD 1.0.4 code drop from Locus.
 *
 *Revision 1.1.1.3  1993/07/01  19:18:33  cfj
 *Adding new code from vendor
 *
 *Revision 1.3  1993/05/06  19:10:14  cfj
 *ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:02  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:07  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:12  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1993/04/15  22:47:50  condict
 * 	Added __rouent and __rouret funcs, to support server profiling.
 *
 * Revision 2.2  91/11/17  14:44:35  rabii
 * 	Initial Checkin
 * 
 * Revision 2.2  91/09/03  11:12:44  jsb
 * 	First checkin. Copied from i860 kernel source.
 * 	[91/09/03  10:19:31  jsb]
 * 
 * Revision 2.3  91/06/18  20:51:34  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:57:02  jsb]
 * 
 * Revision 2.2  90/12/04  14:48:57  jsb
 * 	First checkin.
 * 	[90/12/03  21:31:19  jsb]
 * 
 */

#include <i860/asm.h>

	.text

/*
 * Function deduced from i386 code:
 * htonl(0xddccbbaa) = 0xaabbccdd
 */

ENTRY(htonl)
	shr	16,r0,r0	// rotate right 16 bits
	shrd	r16,r16,r16
	shl	8,r16,r17	// shift left 8 bits and
	andnoth	255,r17,r17	// mask with 0xFF00FF00
	andnot	255,r17,r17	
	shr	8,r16,r16	// shift original value right 8
	andnoth	65280,r16,r16	// mask with 0x00FF00FF
	andnot	65280,r16,r16
	bri	r1
	  or	r16,r17,r16	// combine partial results


/*
 * htons(a)
 *
 * host-to-network-short
 *
 * memory
 *   host	    net
 *
 * [LSB,MSB] --> [MSB,LSB]
 *
 *
 */

ENTRY(htons)
	and	0xFFFF,r16,r16		// clear upper half of register
	and	0xFF,r16,r17		// isolate host-mem-MSB
	shl	8,r17,r17		// position to net-mem-MSB
	shr	8,r16,r16		// position host-mem-LSB --> net-mem-MSB
	bri	r1
	  or	r16,r17,r16		// combine

/*
 * network-to-host-long
 *
 * HH-HL-LH-LL
 * LL-LH-HL-HH
 *
 * 0xddccbbaa = htonl(0xaabbccdd)
 */

ENTRY(ntohl)
	mov	r16,r17
	mov	r0,r16

	and	0x00FF,r17,r18
	shl	24,r18,r18
	or	r18,r16,r16

	and	0xFF00,r17,r18
	shl	 8,r18,r18
	or	r18,r16,r16

	andh	0x00FF,r17,r18
	shr      8,r18,r18
	or	r18,r16,r16

	andh	0xFF00,r17,r18
	shr	24,r18,r18

	bri	r1
	  or	r18,r16,r16



/*
 *
 * network-to-host-short
 * HH-HL-LH-LL
 * HH-HL-LL-LH
 */

ENTRY(ntohs)
	mov	r16,r17
	mov	r0,r16

	and	0x00FF,r17,r18
	shl	8,r18,r18
	or	r18,r16,r16

	and	0xFF00,r17,r18
	shr	8,r18,r18

	bri	r1
	  or	r18,r16,r16

/*
 * FAKE a reference to '_start' so 'crt0.0' will be loaded out of libmach_sa.a
 * and the entire silly mess can actually link/start.
 */
			.data
                        .extern start
_a_true_hack:           .long   start
			.text

#if	GPROF

	.dsect		// dumped on the user stack
u_r1:	.long		// return address (volatile)
u_r16:	.long		// volatile
u_r17:	.long		// volatile
u_r18:	.long		// volatile
u_r19:	.long		// volatile
u_r20:	.long		// volatile
u_r21:	.long		// volatile
u_r22:	.long		// volatile
u_r23:	.long		// volatile
u_r24:	.long		// volatile
u_r25:	.long		// volatile
u_r26:	.long		// volatile
u_r27:	.long		// volatile
u_r28:	.long		// volatile
u_r30:	.long		// volatile

u_f16:	.long		// volatile
u_f17:	.long		// volatile
u_f18:	.long		// volatile
u_f19:	.long		// volatile
u_f20:	.long		// volatile
u_f21:	.long		// volatile
u_f22:	.long		// volatile
u_f23:	.long		// volatile
u_f24:	.long		// volatile
u_f25:	.long		// volatile
u_f26:	.long		// volatile
u_f27:	.long		// volatile
	.align	16
u_end:
	.end

	.extern	_mcount

/*
 * ___rouent is called on entry to each routine f, when compiled with -p
 * It is called before the saving of anything and before the construction
 * of a stack frame.  Its calling sequence is special.
 *
 * Params: r29 = ptr to struct containing entry address of f (ignored).
 *	   r1  = address in f to which we must return -- mcount needs this.
 *	   r30 = return address in f's caller -- mcount needs this.  Our
 *		 caller requires this value to be in r1 on return.
 */
	.globl	___rouent
___rouent:
	orh ha%_profiling, r0, r31	/* Check the boolean profiling       */
	ld.l l%_profiling(r31), r29	/* variable (it uses reverse logic!).*/
	bte r0, r29, yes_prof	/* If profiling == 0, profiling is *on*.     */
	mov r1, r29		/* Get out fast. Save our return address.    */
	bri	r29		/* Return to our caller, but first:	     */
	mov r30, r1		/* Restore r1 to what the caller expects.    */
yes_prof:
	addu	-u_end,sp,sp

/*
 * Save int registers.  Note: The value in r30 is saved now and then restored
 * into r1, and vice versa for r1.  See comments above.
 */
	st.l	r30,u_r1(sp)
	st.l	r16,u_r16(sp)
	st.l	r17,u_r17(sp)
	st.l	r18,u_r18(sp)
	st.l	r19,u_r19(sp)
	st.l	r20,u_r20(sp)
	st.l	r21,u_r21(sp)
	st.l	r22,u_r22(sp)
	st.l	r23,u_r23(sp)
	st.l	r24,u_r24(sp)
	st.l	r25,u_r25(sp)
	st.l	r26,u_r26(sp)
	st.l	r27,u_r27(sp)
	st.l	r28,u_r28(sp)
	st.l	r1, u_r30(sp)

/* Save FP regs */

	fst.l	f16,u_f16(sp)
	fst.l	f17,u_f17(sp)
	fst.l	f18,u_f18(sp)
	fst.l	f19,u_f19(sp)
	fst.l	f20,u_f20(sp)
	fst.l	f21,u_f21(sp)
	fst.l	f22,u_f22(sp)
	fst.l	f23,u_f23(sp)
	fst.l	f24,u_f24(sp)
	fst.l	f25,u_f25(sp)
	fst.l	f26,u_f26(sp)
	fst.l	f27,u_f27(sp)

/* Call real mcount now passing callers pc and callers callers pc */
	mov	r1,r16
	call	_mcount
	mov	r30,r17

/* Restore int regs */
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
	ld.l	u_r30(sp),r30

/* FP regs */

	fld.l	u_f16(sp),f16
	fld.l	u_f17(sp),f17
	fld.l	u_f18(sp),f18
	fld.l	u_f19(sp),f19
	fld.l	u_f20(sp),f20
	fld.l	u_f21(sp),f21
	fld.l	u_f22(sp),f22
	fld.l	u_f23(sp),f23
	fld.l	u_f24(sp),f24
	fld.l	u_f25(sp),f25
	fld.l	u_f26(sp),f26
	fld.l	u_f27(sp),f27

/* return and restore sp */
	bri	r30
	addu	u_end,sp,sp


	.globl	___rouret
___rouret:
	mov r1, r29
	bri	r29		/* Return to our caller, but first:	   */
	mov r30, r1		/* Restore r1 to what the caller expects.  */

#endif	GPROF
