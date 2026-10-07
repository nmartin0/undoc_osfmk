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
 *              INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *  This software is supplied under the terms of a license
 *  agreement or nondisclosure agreement with Intel Corporation 
 *  and may not be copied or disclosed except in accordance
 *  with the terms of that agreement.
 *
 *      Copyright 1992 Intel Corporation.
 *
 *
 * HISTORY
 * $Log: mul.s,v $
 *Revision 1.2  1994/11/18  20:25:47  mtm
 *Copyright additions/changes
 *
 *Revision 1.1  1993/01/20  19:49:45  brad
 *Initial revision
 *
 * 
 */

	.file	"mul.s"

/*
 * Calling sequence:
 *
 * 	mulx( <long>, <long>, <extended result> )
 *
 * Parameters:
 *
 *	r16	first operand (long)
 *
 *	r17	second operand (long)
 *
 *	r18	points to result of r16 * r17 (extended)
 *
 * Description:
 *	0(r18) = lo_32(r16*r17)
 *	4(r18) = hi_32(r16*r17)
 */

	.text
	.align	4
_mulx::
	fmov.ss	f0,f17			// zero high bits
	mov	r16,r19			// split into low and high 16 bits
	fmov.ss	f0,f19			// zero high bits
	and	0xffff,r16,r16		// low 16 bits of r16
	shr	16,r19,r19		// put high 16 into low 16 bits
	and	0xffff,r19,r19		// zero high bits
	ixfr	r16,f16			// low 16 bits of r16
	ixfr	r17,f18
	fmlow.dd	f16,f18,f20	// low 48 bits of r16*r17
	fxfr	f21,r21
	fxfr	f20,r20			// low 32 bits of 48 prod.
	and	0xffff,r21,r21		// high 16 bits of 48 prod.
	ixfr	r19,f16			// high 16 bits of r16
	fmlow.dd	f16,f18,f20	// high 48 bits of r16*r17
	fxfr	f21,r23			
	fxfr	f20,r22			// low 32 bits of 48 prod.
	and	0xffff,r23,r23		// high 16 bits of 48 prod.
	shl	16,r22,r24		// position low 16 bits
	shr	16,r22,r25		// position middle 16 bits
	shl	16,r23,r26		// position high 16 bits
	and	0xffff,r25,r25		// and clear other middle bits
	or	r25,r26,r25		// combine middle and high 16 bits
	addu	r20,r24,r20		// combine low 32 bits
	bnc	no_cc
	addu	1,r21,r21		// add carry bit
no_cc:	addu	r21,r25,r21		// combine high 32 bits
	st.l	r20,0(r18)		// save low bits
	bri	r1			// return
	st.l	r21,4(r18)		// save high bits
