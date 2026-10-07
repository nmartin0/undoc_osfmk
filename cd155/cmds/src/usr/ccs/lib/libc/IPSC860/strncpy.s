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
	.file	"strncpy.s"

#include	<i860/asm.h>
	.text

// Assume relatively long strings and small numbers of nulls at end.

ENTRY(strncpy)
	mov	r16,r19	// preserve original value
.loop:
	addu	-1,r18,r18	// decrement bytes to copy (unsigned)
	bnc	.done
	ld.b	0(r17),r20
	ld.b	1(r17),r21	// pre-load the next byte
	st.b	r20,0(r19)
	bte	r0,r20,.fill1

	addu	-1,r18,r18	// decrement bytes to copy
	bnc	.done
	ld.b	2(r17),r20	// pre-load the next byte
	st.b	r21,1(r19)
	bte	r0,r21,.fill2

	addu	-1,r18,r18	// decrement bytes to copy
	bnc	.done
	ld.b	3(r17),r21	// pre-load the next byte
	st.b	r20,2(r19)
	bte	r0,r20,.fill3

	addu	-1,r18,r18	// decrement bytes to copy
	bnc	.done
	st.b	r21,3(r19)
	addu	4,r17,r17
	addu	4,r19,r19
	btne	r0,r21,.loop
	addu	-3,r19,r19	// realign pointer
.fill3:
	addu	1,r19,r19
.fill2:
	addu	1,r19,r19
.fill1:
	addu	1,r19,r19
	addu	-1,r18,r18	// decrement bytes to fill
	bnc	.done
	br	.fill1
	st.b	r0,0(r19)

.done:
	bri	r1		// return original value from call
	nop
