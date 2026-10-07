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
	.file	"strcat.s"

#include	<i860/asm.h>
	.text

ENTRY(strcat)
	addu	-1,r16,r18	// preserve original value
// find end of destination string
.loop1:
	ld.b	1(r18),r19
	addu	1,r18,r18
	btne	r0,r19,.loop1

// Most suffixes are not very long; not worth setup for fancy moves
.loop2:
	ld.b	0(r17),r19
	addu	1,r17,r17
	st.b	r19,0(r18)
	addu	1,r18,r18
	btne	r0,r19,.loop2

	bri	r1
	nop
