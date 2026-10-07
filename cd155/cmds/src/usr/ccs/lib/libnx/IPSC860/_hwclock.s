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
 
//
//		INTEL CORPORATION PROPRIETARY INFORMATION
//
//	This software is supplied under the terms of a license
//	agreement or nondisclosure agreement with Intel Corporation
//	and may not be copied or disclosed except in accordance
//	with the terms of that agreement.
//
//	_hwclock.s 9.1 91/12/09 17:15:09
//
	.file	"_hwclock.s"

counter_reg	=	0xFFFF4630

		.text
		.align	4

__hwclock::
_hwclock_::
_hwclock::
		orh	ha%counter_reg, r0, r31	// Set up register address
		fld.d	l%counter_reg(r31), f16	// Read counter into f16-17
		fst.l	f16, 0(r16)		// Store at pointer as two
		bri	r1			// Return,
		 fst.l	f17, 4(r16)		//  longs to avoid traps
