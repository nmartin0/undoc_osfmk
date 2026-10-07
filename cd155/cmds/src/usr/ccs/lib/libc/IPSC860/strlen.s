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
	.file	"strlen.s"

#include      <i860/asm.h>

	.text

ENTRY(strlen)
	mov	r0,r17	// initialize count
.loop:
	ld.b	0(r16),r18
	addu	1,r16,r16
	addu	1,r17,r17	// pre-increment count (unsigned)
	bte	r0,r18,.done

	ld.b	0(r16),r18
	addu	1,r16,r16
	addu	1,r17,r17	// pre-increment count
	bte	r0,r18,.done

	ld.b	0(r16),r18
	addu	1,r16,r16
	addu	1,r17,r17	// pre-increment count
	bte	r0,r18,.done

	ld.b	0(r16),r18
	addu	1,r16,r16
	addu	1,r17,r17	// pre-increment count
	btne	r0,r18,.loop

.done:
	bri	r1
	addu	-1,r17,r16
