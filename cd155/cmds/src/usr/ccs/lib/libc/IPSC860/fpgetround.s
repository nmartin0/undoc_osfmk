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
	.file	"fpgetround.s"

FP_ROUND_MASK = 0x3
FP_ROUND_SHIFT = 2

_fpgetround::
	ld.c	fsr,r16
	shr	FP_ROUND_SHIFT,r16,r16
	bri	r1
	  and	FP_ROUND_MASK,r16,r16
