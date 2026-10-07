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
	.file	"strcpy.s"

#include	<i860/asm.h>
	.text

ENTRY(strcpy)
	mov	r16,r18	// preserve original value
.loop:
	ld.b	0(r17),r19
	ld.b	1(r17),r20	// pre-load the next byte
	st.b	r19,0(r18)
	bte	r0,r19,.done

	ld.b	2(r17),r19	// pre-load the next byte
	st.b	r20,1(r18)
	bte	r0,r20,.done

	ld.b	3(r17),r20	// pre-load the next byte
	st.b	r19,2(r18)
	bte	r0,r19,.done

	st.b	r20,3(r18)
	addu	4,r17,r17
	addu	4,r18,r18
	btne	r0,r20,.loop

.done:
	bri	r1		// return original value from call
	nop
