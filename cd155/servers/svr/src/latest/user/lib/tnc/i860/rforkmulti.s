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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
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
 * $Log: rforkmulti.s,v $
// Revision 1.9  1995/02/02  00:11:52  bolsen
//  Reviewer(s): Jerry Toman
//  Risk: Medium (lots of files)
//  Module(s): Too many to list
//  Configurations built: STD, LITE, & RAMDISK
//
//  Added or Updated the Locus Copyright message.
//
// Revision 1.8  1994/11/18  20:56:54  mtm
// Copyright additions/changes
//
// Revision 1.7  1994/06/02  22:37:31  chrisp
// Trap code added for waitmulti().
//
//  Reviewer: cfj
//  Risk: M
//  Benefit or PTS #: 6463
//  Testing:
//  Modules: rforkmulti.s
//
// Revision 1.6  1994/01/17  21:17:51  slk
// Fix for #7797, programs that return from main() core dump.
//
//  Reviewer: Chris Peak and Charlie Johnson
//  Risk: Low.
//  Benefit or PTS #: #7797, allows Locus test suites to complete properly.
//  Testing: Ran Locus test suites
//  Module(s): SYS.h
//
// Revision 1.5  1993/07/14  18:53:24  cfj
// OSF/1 AD 1.0.4 code drop from Locus.
//
// Revision 1.4  1993/05/06  19:32:35  stefan
// ad103+tnc merged with Intel code.
//
// Revision 1.1.1.3  1993/07/01  21:20:57  cfj
// Adding new code from vendor
//
// Revision 1.3  1993/02/23  15:59:42  cfj
// Now matches versions in commands and libs.
//
// Revision 1.1.1.1  1993/05/03  17:59:00  cfj
// Initial 1.0.3 code drop
//
 * Revision 3.2  1992/06/23  12:07:42  stans
 * added child code to re-initialize mach cached variables; call mach_init()
 * when mach_init_routine function pointer is non-zero.
 *
 * Revision 3.1  92/04/23  17:18:55  klh
 * 	Initial check-in of Intel provided code.
 * 	Fix RCS comment.
 * 
 * Revision 1.1  92/04/22  14:43:00  cfj
 * Initial revision
 * 
 * $EndLog$
 */
        .file   "rforkmulti.s"

#include "SYS.h"
//
// rforkmulti(count, node_array, rval_array, pid_array)
//	int	*count;
//	int	*node_array
//	int	*rval_array
//	int	*pid_array
//

	.file	"rforkmulti.s"
	.globl	_mach_init_routine

SYSCALL(rforkmulti)
        bte     r0,r17,5f	// r17 == 0 then we are the parental unit.
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
        mov     r0,r16
5:
        SRET

//
// waitmulti(count, pid_array, status_array, options)
//	int	*count;
//	int	*pid_array;
//	int	*status_array;
//	int	options;
//
SYSCALL(waitmulti)
	SRET
