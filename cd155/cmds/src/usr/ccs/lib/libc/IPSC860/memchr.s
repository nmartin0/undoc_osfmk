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
	.file	"memchr.s"

#include	<i860/asm.h>

//
//	parameters: r16 -- pointer to memory to search
//		    r17 -- character being sought
//		    r18 -- maximum number of bytes to check
//
//	The loop is unrolled, since there is a 2-clock delay on a taken
//	bte/btne.
//
	.text

ENTRY(memchr)
	shl	24,r17,r17
	shra	24,r17,r17	// sign-extend the char in r17

.loop:
	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.notfound	// just went to 0xffffffff, so we are done
	ld.b	0(r16),r19	// get next char
	addu	1,r16,r16	// advance pointer
	bte	r17,r19,.found// if latest char is the one we want, branch

	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.notfound	// just went to 0xffffffff, so we are done
	ld.b	0(r16),r19	// get next char
	addu	1,r16,r16	// advance pointer
	bte	r17,r19,.found// if latest char is the one we want, branch

	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.notfound	// just went to 0xffffffff, so we are done
	ld.b	0(r16),r19	// get next char
	addu	1,r16,r16	// advance pointer
	bte	r17,r19,.found// if latest char is the one we want, branch

	addu	-1,r18,r18	// decrement count (unsigned)
	bnc	.notfound	// just went to 0xffffffff, so we are done
	ld.b	0(r16),r19	// get next char
	addu	1,r16,r16	// advance pointer
	btne	r17,r19,.loop	// if latest char is not the one we want, loop

.found:
	bri	r1		// return
	addu	-1,r16,r16	// pointer to where we found it

.notfound:
	bri	r1		// search failed
	mov	r0,r16	// return NULL
