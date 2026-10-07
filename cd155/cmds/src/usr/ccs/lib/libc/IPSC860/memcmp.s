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
	.file	"memcmp.s"

#include        <i860/asm.h>

	.text

// This implementation conforms to SVID but does not implement
// the same algorithm as the portable version because it is
// inconvenient to get the difference of the differing characters.
// Cannot compare word at a time because of byte ordering and
// the comparison of chars is signed.

ENTRY(memcmp)
	bte	r16,r17,.equal// The same string?
.loop:
	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.equal
	ld.b	0(r16),r19
	ld.b	0(r17),r20
	addu	1,r16,r16
	addu	1,r17,r17
	bte	r19,r20,.loop	// compare the bytes
.notequal:
	bri	r1
	subs	r19,r20,r16	// make it + or - according to result
.equal:
	bri	r1
	mov	r0,r16
