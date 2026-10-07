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
	.file	"strncmp.s"

#include	<i860/asm.h>
	.text

ENTRY(strncmp)
	bte	r16,r17,.equal// Same string? - must be equal
.loop:
	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.equal
	ld.b	0(r16),r20
	ld.b	0(r17),r21
	ld.b	1(r16),r22	// pre-load the next byte
	btne	r20,r21,.notequal
	bte	r0,r20,.equal	// end of string, and match

	addu	-1,r18,r18	// decrement count
	bnc	.equal
	ld.b	1(r17),r23
	ld.b	2(r16),r20	// pre-load the next byte
	btne	r22,r23,.noteq
	bte	r0,r22,.equal	// end of string, and match

	addu	-1,r18,r18	// decrement count
	bnc	.equal
	ld.b	2(r17),r21
	ld.b	3(r16),r22	// pre-load the next byte
	btne	r20,r21,.notequal
	bte	r0,r20,.equal	// end of string, and match

	addu	-1,r18,r18	// decrement count
	bnc	.equal
	ld.b	3(r17),r23
	btne	r22,r23,.noteq
	addu	4,r16,r16
	addu	4,r17,r17
	btne	r0,r22,.loop	// end of string, and match
.notequal:
	and	0xff,r20,r20	// mask sign bits
	and	0xff,r21,r21
	bri	r1
	  subs	r20,r21,r16
.noteq:
	and	0xff,r22,r22	// mask sign bits
	and	0xff,r23,r23
	bri	r1
	  subs	r22,r23,r16
.equal:				// done, strings match
	bri	r1
	  mov	r0,r16
