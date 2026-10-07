// 
// $Copyright
// Copyright 1993, 1994, 1995  Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
//	Copyright (c) 1989, 1992 Intel Corporation.
//	All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//
	.file	"fpsetmask.s"


M_STATUS_MASK = 0x1f
A_STATUS_SHIFT = 15
SHIFT_ADJUST = 12
EINVAL = 22

	.globl	_errno

_fpsetmask::
	orh	0x3f80,r0,r31		// 1.0
	ixfr	r31,f8
	fmul.ss	f8,f8,f8		// clean up current result error
	fadd.ss	f0,f0,f0		// clean up current result error

	andnot	M_STATUS_MASK,r16,r0	// check for extraneous bits.
	bnc	.error


	// Put new bits in r19, in kernel format
	subs	-1,r16,r16		// flip the bits
	shl	A_STATUS_SHIFT+SHIFT_ADJUST,r16,r19
	shr	SHIFT_ADJUST,r19,r19	// Get rid of leading 1's
	
	trap	r0,r16,r0		// get the IEEE control word

					// extract old bits, construct new control word
	mov	r16,r18			// save old word
	or	M_STATUS_MASK,r0,r17
	shl	A_STATUS_SHIFT,r17,r17	// r17 = mask
	andnot	r17,r16,r17		// r17=old word, minus status bits
	or	r19,r17,r16		// new control word

	trap	r0,r17,r0		// set new IEEE control word

					// return the old value
	subs	-1,r18,r18		// flip the bits
	shr	A_STATUS_SHIFT,r18,r16
	bri	r1
	  and	M_STATUS_MASK,r16,r16

.error:
	or	EINVAL,r0,r16
	orh	ha%_errno,r0,r31
	st.l	r16,l%_errno(r31)
	bri	r1
	  adds	-1,r0,r16
