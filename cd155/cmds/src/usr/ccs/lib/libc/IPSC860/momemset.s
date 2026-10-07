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
 
//      Copyright (c) 1989, Intel Corporation.
//      All rights reserved.
//
//        INTEL CORPORATION PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license
// agreement or nondisclosure agreement with Intel Corp.
// and may not be copied or disclosed except in accordance
// with the terms of that agreement.
//
//
	.file	"momemset.s"

#include      <i860/asm.h>

	.text

ENTRY(momemset)
	mov	r16,r19	// preserve original value
	and	3,r16,r0	// is address aligned mod 4
	bnc	.byteloop
	shl	8,r17,r20
	or	r20,r17,r17
	shl	16,r17,r20
	or	r20,r17,r17
.loop:
	addu	-4,r18,r18	// decrement bytes to set (unsigned)
	bnc	.done
	st.l	r17,0(r19)	// set 4 bytes
	br	.loop
	addu	4,r19,r19

.done:
	addu	4,r18,r18	// fall into byte loop
.byteloop:
	addu	-1,r18,r18	// decrement bytes to set (unsigned)
	bnc	.bytedone
	st.b	r17,0(r19)	// set byte
	br	.byteloop
	addu	1,r19,r19

.bytedone:
	bri	r1		// return original value from call
	nop
