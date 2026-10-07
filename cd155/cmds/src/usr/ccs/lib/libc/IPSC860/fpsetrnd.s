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
 
/*	Copyright (c) 1991, Intel Corporation.
 *	All rights reserved.
 *
 *	INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *	This software is supplied under the terms of a license
 *	agreement or nondisclosure agreement with Intel Corpo-
 *	ration and may not be copied or disclosed except in
 *	accordance with the terms of that agreement.
 *
 * fpsetrnd( new_rounding_mode )
 *
 * sets the i860 processor's rounding mode to new_rounding_mode
 * and returns the old rounding mode.
 *
 *
 *  The bit patterns and corresponding rounding modes as specified by ANSI are:
 *      00      RZ      Round toward zero
 *      01      RN      Round toward nearest representable value
 *      10      RP      Round toward positive infinity
 *      11      RM      Round toward minus infinity
 */


	.file	"fpsetrnd.s"

#include <i860/asm.h>

	.text

ENTRY(fpsetrnd)
ENTRY(write_rnd)
	ld.c	fsr,r17
	shl	2,r16,r16	// adjust RR for FSR
	andnot	0x0c,r17,r18	// clear previous RR
	or	r16,r18,r18	// set new RR
	st.c	r18,fsr
	shr	2,r17,r16
	bri	r1
	  and	0x03,r16,r16	// return old RR

