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
 
//      Copyright (c) 1992 Intel Corporation.
//      All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//    This software is supplied under the terms of a license
//    agreement or nondisclosure agreement with Intel Corpo-
//    ration and may not be copied or disclosed except in
//    accordance with the terms of that agreement.
//

        .file "isnan.s"
        .text

// Essentially, these routines returns ~0 if the argument is a NaN, 
// and 0 if the argument is
// any other number, zero (of either sign), or an infinity (of either sign).
//
// The algorithm used is non-signalling and trap free, without memory 
// references. It attempts to return as early as possible on false conditions.
//
// Essentially, a NaN has an exponent of all binary ones, and a mantissa that 
// is non-zero.
//
// Infinity is similar to a NaN, except it has a zero mantissa.
//
// int isnan();
// int isnand(double);

        .align  32

_isnan::
_isnand::
        fxfr    f9,r17
        shr     20,r17,r16
        adds    0x7ff,r0,r18
        and     r18,r16,r16
        btne     r18,r16,.falsef	// not a NaN if exponent not all ones
					// return 0
// the exponent was all ones, so it's a NaN or Infinity
        fxfr    f8,r16
        btne    0,r16,.true          // a NaN if mantissa [0-31] not all zero
        shl     12,r17,r17

// sas: btne changed to bte for correctness

        bte     0,r17,.falsef        // infinity if mantissa [32-51] all zero
.true:
        bri     r1                      // return true
          or	1,r0,r16

// Essentially, this routine returns ~0 if the argument is a NaN, and 0 
// if the argument is
// any other number, zero (of either sign), or an infinity (of either sign).
//
// The algorithm used is non-signalling and trap free, without memory refs.
// It should return as early as possible on false conditions.
//
// Essentially, a NaN has an exponent of all binary ones, and a mantissa that is
// non-zero.
//
// Infinity is similar to a NaN, except it has a zero mantissa.
//
// int isnanf(float);

        .align  32

_isnanf::

        fxfr    f8,r17
        shl     1,r17,r16
        shr     24,r16,r16
        or      0x00ff,r0,r18
        btne    r18,r16,.falsef	// not a NaN if exponent not all ones
        			// It's a normal or denormal number, return 0
.nan_or_inf_f:

        // the exponent was all ones, so it's a NaN or Infinity

        shl     9,r17,r17
        shr     1,r17,r17
        bte     r0,r17,.falsef         // infinity if mantissa [0-22] zero

        // It is a nan
 
        bri     r1                     // return 1 (true) 
          or	1,r0,r16

        // It is an infinity, return 0 (false)
       	// It's a normal or denormal number, return 0
.falsef:
        bri     r1
          mov   r0,r16

