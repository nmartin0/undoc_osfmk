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
	.file	"fpsetround.s"

FP_ROUND_MASK = 0x3
FP_ROUND_SHIFT = 2
EINVAL = 22

	.globl	_errno

_fpsetround::
	andnot	FP_ROUND_MASK,r16,r0
	bnc	.error
	shl	FP_ROUND_SHIFT,r16,r16
	ld.c	fsr,r17
	shr	FP_ROUND_SHIFT,r17,r18	// save old rounding mode
	andnot	FP_ROUND_MASK<<FP_ROUND_SHIFT,r17,r17
	or	r16,r17,r17	// new rounding mode
	st.c	r17,fsr	// set new rounding mode
	bri	r1
	  and	FP_ROUND_MASK,r18,r16

.error:
	or	EINVAL,r0,r16
	orh	ha%_errno,r0,r31
	st.l	r16,l%_errno(r31)
	bri	r1
	  adds	-1,r0,r16



