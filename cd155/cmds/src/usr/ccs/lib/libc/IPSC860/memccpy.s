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
	.file	"memccpy.s"

#include        <i860/asm.h>

//
//	parameters:  r16 -- move target
//		     r17 -- move source
//		     r18 -- character to stop on if found
//		     r19 -- max bytes to move
//
//	the loop is unrolled 4 times for speed
//
	.text
	.align	4

ENTRY(memccpy)
	addu	-1,r16,r16	// back up target by 1 (loop rearrangement)
	shl	24,r18,r18
	shra	24,r18,r18	// sign-extend the char in r18

.loop:
	addu	-1,r19,r19	// decrement bytes to move (unsigned)
	bnc	.notfound	// if it goes to 0xffffffff, we are done
	ld.b	0(r17),r20	// load current source byte
	addu	1,r16,r16	// increment the target pointer
	st.b	r20,0(r16)	// store byte
	bte	r18,r20,.found // branch if it is the byte sought

	addu	-1,r19,r19	// decrement bytes to move (unsigned)
	bnc	.notfound	// if it goes to 0xffffffff, we are done
	ld.b	1(r17),r20	// load current source byte
	addu	1,r16,r16	// increment the target pointer
	st.b	r20,0(r16)	// store byte
	bte	r18,r20,.found // branch if it is the byte sought

	addu	-1,r19,r19	// decrement bytes to move (unsigned)
	bnc	.notfound	// if it goes to 0xffffffff, we are done
	ld.b	2(r17),r20	// load current source byte
	addu	1,r16,r16	// increment the target pointer
	st.b	r20,0(r16)	// store byte
	bte	r18,r20,.found // branch if it is the byte sought

	addu	-1,r19,r19	// decrement bytes to move (unsigned)
	bnc	.notfound	// if it goes to 0xffffffff, we are done
	ld.b	3(r17),r20	// load current source byte
	addu	1,r16,r16	// increment the target pointer
	addu	4,r17,r17	// advance the source pointer
	st.b	r20,0(r16)	// store byte
	btne	r18,r20,.loop	// branch if it is not the byte sought

.found:
	bri	r1		// yes, return byte after in dest
	addu	1,r16,r16	// point to "byte after"

.notfound:
	bri	r1		// search fails
	mov	r0,r16	// return NULL
