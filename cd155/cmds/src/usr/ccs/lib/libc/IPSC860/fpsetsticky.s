//    
//    $Copyright
//    Copyright 1993, 1994, 1995  Intel Corporation
//    INTEL CONFIDENTIAL
//    The technical data and computer software contained herein are subject
//    to the copyright notices; trademarks; and use and disclosure
//    restrictions identified in the file located in /etc/copyright on
//    this system.
//    Copyright$
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
//
// $Id: fpsetsticky.s,v 1.4 1994/11/19 02:11:20 mtm Exp $
//
// HISTORY
// $Log: fpsetsticky.s,v $
//    Revision 1.4  1994/11/19  02:11:20  mtm
//    Copyright additions/changes
//
//    Revision 1.3  1992/07/07  11:29:27  stans
//    bnc --> bc so TI (trap inexact) is set ONLY when it's required instead of
//    all the time!`
//
//
	.file	"fpsetsticky.s"


M_STICKY_MASK = 0x1f
FP_X_IMP = 10
A_STICKY_SHIFT = 10
EINVAL = 22

	.globl	_errno

_fpsetsticky::
	orh	0x3f80,r0,r31		// 1.0
	ixfr	r31,f8
	fmul.ss	f8,f8,f8		// clean up current result error
	fadd.ss	f0,f0,f0		// clean up current result error

	andnot	M_STICKY_MASK,r16,r0	// check for extraneous bits.
	bnc	.error

	shl	A_STICKY_SHIFT,r16,r19
	trap	r0,r16,r0		// get the IEEE control word

	and	FP_X_IMP<<A_STICKY_SHIFT,r19,r0	// if INEXACT to be enabled, then TI must be set
	bc	.no_imprecise
	ld.c	fsr,r20
	or	0x2,r20,r20
	st.c	r20,fsr			// set TI bit of fsr register
.no_imprecise:
					// extract old bits, construct new control word
	or	M_STICKY_MASK,r0,r17
	shl	A_STICKY_SHIFT,r17,r17
	and	r17,r16,r18		// r18 now has old bits in returned positions
	andnot	r17,r16,r17
	or	r19,r17,r16		// new control word

	trap	r0,r17,r0		// set new IEEE control word
	bri	r1
	  shr	A_STICKY_SHIFT,r18,r16

.error:
	or	EINVAL,r0,r16
	orh	ha%_errno,r0,r31
	st.l	r16,l%_errno(r31)
	bri	r1
	  adds	-1,r0,r16

