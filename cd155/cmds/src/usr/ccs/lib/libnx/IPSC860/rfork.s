/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
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
 * $Log: rfork.s,v $
 * Revision 3.9  1994/11/19  02:33:30  mtm
 * Copyright additions/changes
 *
 * Revision 3.8  1993/08/05  22:30:46  stans
 *    Added conditional assembly for the pthreads library versions.
 *
 * Revision 3.7  1993/08/04  20:36:37  stans
 *    Dump the '_NX_present' hack (a bad idea).
 *    Added 'pthread_rfork' entry point as the pthread library interface.
 *
 * Revision 3.6  1993/08/03  23:38:20  stans
 *    Call pthread library initialization routine.
 *
 * Revision 3.5  1993/04/29  16:02:41  shala
 * Support stack frame.
 *
 * Revision 3.4  1993/02/23  15:55:58  shala
 * Restore 3.2 version.
 * 
 * Revision 3.2  1992/06/23  12:06:19  stans
 * added child code to re-initialize mach cached variables; call mach_init()
 * when mach_init_routine function pointer is non-zero.
 * 
 * Revision 3.1  92/04/23  17:17:58  klh
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

#ifndef	_PTHREADS

SYSCALL(rfork)

#else

/*
 * pthreads library version of rfork(node) syscall; same interface.
 * The pthread library steals the name rfork() so some pthread specific
 * library cleanup and reinitialization can be performed by the child.
 */
PSEUDO(pthread_rfork,rfork)

#endif
	bte	r0,r17,.rf_exit	/* parent? r17 == 0 */
	/*
	 * child code: should we call mach_init()?
	 */
	orh	h%_mach_init_routine,r0,r18
	or	l%_mach_init_routine,r18,r18
	ld.l	0(r18),r18	// load mach_init function pointer
	bte	r0,r18,.rf_nc	// <null> == no call, skip ahead.
        adds    -16,sp,sp       // Make room on stack to save ret address
        st.l    r1,0(sp)	// nice to save our return address.
        calli	r18		// call mach_init()
          nop
        ld.l    0(sp),r1        // Restore return address
        adds    16,sp,sp	// deallocate stack space
.rf_nc:
	mov	r0,r16		// set child return code.
.rf_exit:
	SRET			// pid = rfork()

