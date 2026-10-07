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
 * Copyright (c) 1991, Locus Computing Corporation
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
 * Revision 3.11  1994/11/19  02:33:30  mtm
 * Copyright additions/changes
 *
 * Revision 3.10  1994/06/15  15:30:38  cfj
 * Add the waitmulti() syscall.
 *
 *  Reviewer:shala
 *  Risk:L
 *  Benefit or PTS #:rforkmulti/waitmulti integration
 *  Testing:The following EATs:
 * 	 message
 * 	 controlc
 * 	 misc
 * 	 rmcall
 * 	 rmcmd
 *  Module(s):libnx/nx_waitall.c libnx/IPSC860/rforkmulti.s
 *
 * Revision 3.9  1993/08/05  22:30:54  stans
 *    Added conditional assembly for the pthreads library versions.
 *
 * Revision 3.8  1993/08/04  20:36:56  stans
 *    Dump the '_NX_present' hack (a bad idea).
 *    Added 'pthread_rforkmulti' entry point as the pthread library interface.
 *
 * Revision 3.7  1993/08/04  16:24:31  stans
 *    Child code clears _NX_present so pthread exit rtns can run from crt0.c
 *
 * Revision 3.6  1993/08/03  23:11:57  stans
 *    Added "I am a parallel pgm" boolean '_NX_present' which is set here
 *    to indicate we are going parallel. '_NX_present' is utilized by crt0.c
 *    to skip the pthreads library initialization normally done in crt0.c
 *    as the initialization needs to be performed at the destination node
 *    instead of the shepard process node, otherwise it's not valid!!
 *
 *    Call 'pthread_init()' as the child to initialize the pthread library.
 *
 * Revision 3.5  1993/04/29  16:02:43  shala
 * Support stack frame.
 * 
 * Revision 3.4  1993/02/23  15:57:56  shala
 * Restore 3.2 version.
 * 
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

	.globl	_mach_init_routine

	.text

#ifndef	_PTHREADS

SYSCALL(rforkmulti)

#else

/*
 * pthread library interface to rforkmulti(); Same interface as rforkmulti().
 * The pthread library steals the name rforkmulti() so some pthread specific
 * library cleanup and reinitialization can be performed by the child.
 */

PSEUDO(pthread_rforkmulti,rforkmulti)

#endif
        bte     r0,r17,.rfm_exit // r17 == 0 then we are the parental unit.
	/*
	 * child code: should we call mach_init()?
	 */
	orh	h%_mach_init_routine,r0,r18
	or	l%_mach_init_routine,r18,r18
	ld.l	0(r18),r18	// load mach_init function pointer
	bte	r0,r18,.rfm_nc	// <null> == no call, skip ahead.
        adds    -16,sp,sp       // Make room on stack to save ret address
        st.l    r1,0(sp)	// nice to save our return address.
        calli	r18		// call mach_init()
          nop
        ld.l    0(sp),r1        // Restore return address
        adds    16,sp,sp	// deallocate stack space
.rfm_nc:
        mov     r0,r16		// set child's return code.
.rfm_exit:
        SRET

//
// waitmulti(count, pid_array, status_array, options)
//      int     *count;
//      int     *pid_array;
//      int     *status_array;
//      int     options;
//
SYSCALL(waitmulti)
        SRET

