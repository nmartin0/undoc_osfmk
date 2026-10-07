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
 
//	Copyright (c) 1989, Intel Corporation.
//	All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//
	.file	"abs.s"

//	/* Assembler program to implement the following C program */
//	int
//	abs(arg)
//	int	arg;
//	{
//		return((arg < 0)? -arg: arg);
//	}

	.text
	.align	4
_abs::
	shra	31,r16,r17	// propagate sign bit
	xor	r16,r17,r16	// logical not or nop, depending on sign bit
	subu	r0,r17,r17	// 1 or 0, depending on sign bit
	bri	r1
	  addu	r16,r17,r16	// negated or unchanged

