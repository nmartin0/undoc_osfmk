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
	.file	"strchr.s"

#include	<i860/asm.h>
	.text

ENTRY(strchr)
	shl     24, r17, r17
	shra    24, r17, r17  // sign-extend the character we seek

.loop:
	ld.b	0(r16),r18
	bte	r17,r18,.found
	addu	1,r16,r16
	bte	r0,r18,.notfound

	ld.b	0(r16),r18
	bte	r17,r18,.found
	addu	1,r16,r16
	bte	r0,r18,.notfound

	ld.b	0(r16),r18
	bte	r17,r18,.found
	addu	1,r16,r16
	bte	r0,r18,.notfound

	ld.b	0(r16),r18
	bte	r17,r18,.found
	addu	1,r16,r16
	btne	r0,r18,.loop

.notfound:
	bri	r1
	  mov	r0,r16
.found:
	bri	r1
	nop
