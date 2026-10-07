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
	.file	"fpgetsticky.s"


STICKY_STATUS_MASK = 0x7c00
STICKY_STATUS_SHIFT = 10

_fpgetsticky::
	orh	0x3f80,r0,r31	// 1.0
	ixfr	r31,f8
	fmul.ss	f8,f8,f8	// clean up current result error
	fadd.ss	f0,f0,f0	// clean up current result error
	trap	r0,r16,r0	// get IEEE control word
	and	STICKY_STATUS_MASK,r16,r16
	bri	r1
	  shr	STICKY_STATUS_SHIFT,r16,r16
