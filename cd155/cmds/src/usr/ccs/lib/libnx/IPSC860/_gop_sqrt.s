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
 
//       Copyright (c) 1989,1990,1992 Intel Corporation.
//       All rights reserved.
//
//         INTEL CORPORATION PROPRIETARY INFORMATION
//
//  This software is supplied under the terms of a license
//  agreement or nondisclosure agreement with Intel Corp.
//  and may not be copied or disclosed except in accordance
//  with the terms of that agreement.
//

	.file	"_gop_sqrt.s"

//
//	Double Sqrt computes the principal square root of x.
//	Special cases:
//

	.text
	.align	32	
// ---------------| _gop_sqrt |------------------
	.globl	_gop_sqrt

_gop_sqrt:
  fld.d   onehalf,f20
  pfgt.dd f8,f0,f0                // if(x>0)
  bnc     .sqt_fail1              //  goto .sqrt_pass1
  frsqr.dd        f8,f24  // r0 : f8 = original value
  fmul.dd f20,f8,f18              // .5*x
  fld.d   onept5,f22
  fmul.dd f24,f24,f26             // r0*r0
  fmul.dd f18,f26,f28             // r0*r0*x*.5
  fsub.dd f22,f28,f28             // 1.5 - t
  fmul.dd f24,f28,f28             // r1 = r0 * (t)
  fmul.dd f28,f28,f26             // -> start 2nd
  fmul.dd f18,f26,f26
  fsub.dd f22,f26,f26
  fmul.dd f28,f26,f28             // -> 2nd guess complete
  fld.d   one,f22
  fmul.dd f8,f28,f26              // z = x*r2
  fmul.dd f28,f26,f28             // ans, z*r2
  fsub.dd f22,f28,f28             // ans, 1- z*r2
  fmul.dd f20,f26,f8              // ans, .5*z
  fmul.dd f28,f8,f28              // ans, .5*z *(1-z*r2)
  bri     r1
  fadd.dd f26,f28,f8              // f12 = ans = z + t


.sqt_fail1:

  pfeq.dd f0,f8,f0                // if (x==0)
  bc      .sqt_retn               // return x;
  frsqr.dd        f8,f8   // r0

.sqt_retn:
  bri     r1
  nop

  .data
  .align 8
  one:    .double    1.0
  onept5: .double    1.50
  onehalf:.double    0.50

// _gop_sqrt.s 3.1 92/04/03 10:21:03

