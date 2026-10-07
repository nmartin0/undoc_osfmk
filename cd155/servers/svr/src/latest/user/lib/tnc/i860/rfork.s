// 
// $Copyright
// Copyright 1993, 1994 , 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 *      Copyright (c) 1992, Intel Corporation.
 *      All rights reserved.
 *
 *         INTEL CORPORATION PROPRIETARY INFORMATION
 *
 *    This software is supplied under the terms of a license
 *    agreement or nondisclosure agreement with Intel Corpo-
 *    ration and may not be copied or disclosed except in
 *    accordance with the terms of that agreement.
 */
/*
 * HISTORY
 * $Log: rfork.s,v $
// Revision 1.7  1994/11/18  20:56:53  mtm
// Copyright additions/changes
//
// Revision 1.6  1994/01/17  21:17:48  slk
// Fix for #7797, programs that return from main() core dump.
//
//  Reviewer: Chris Peak and Charlie Johnson
//  Risk: Low.
//  Benefit or PTS #: #7797, allows Locus test suites to complete properly.
//  Testing: Ran Locus test suites
//  Module(s): SYS.h
//
// Revision 1.5  1993/07/14  18:53:18  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.4  1993/05/06  19:32:27  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.3  1993/07/01  21:20:54  cfj
// Adding new code from vendor
//
// Revision 1.3  1993/02/23  15:59:41  cfj
// Now matches versions in commands and libs.
//
// Revision 1.1.1.1  1993/05/03  17:58:59  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.2  1992/06/23  12:06:19  stans
 * added child code to re-initialize mach cached variables; call mach_init()
 * when mach_init_routine function pointer is non-zero.
 * 
 * Revision 3.1  1992/04/23  17:17:58  klh
 * 	Initial check-in of Intel provided code.
 * 	Fix RCS comment.
 *
 * Revision 1.1  92/04/22  14:42:52  cfj
 * Initial revision
 * 
 * $EndLog$
 */
        .file   "rfork.s"
	.globl	_mach_init_routine

#include "SYS.h"

SYSCALL(rfork)
	bte	r0,r17,5f	/* parent? r17 == 0 */
	/*
	 * child code: should we call mach_init()?
	 */
	orh	h%_mach_init_routine,r0,r18
	or	l%_mach_init_routine,r18,r18
	ld.l	0(r18),r18	// load mach_init function pointer
	bte	r0,r18,1f	// <null> == no call, skip ahead.
        adds    -16,sp,sp       // Make room on stack to save ret address
        st.l    r1,0(sp)	// nice to save our return address.
        calli	r18		// call mach_init()
          nop
        ld.l    0(sp),r1        // Restore return address
        adds    16,sp,sp	// deallocate stack space
1:
	mov	r0,r16		// set child return code.
5:
	SRET			// pid = rfork()

