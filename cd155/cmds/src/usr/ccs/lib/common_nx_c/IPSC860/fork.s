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
 
/*
 * Copyright 1988, 1989, 1990, 1991, 1992, by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
        .file   "fork.s"
	.extern _mach_init_routine
	.extern _forknx

#include "SYS.h"

SYSCALL(fork)
	bte	r0,r17,.parent
	// child code: do we call mach_init?
        adds    -16,sp,sp       // Make room on stack to save ret address
        st.l    r1,0(sp)        // save our return address.
        st.l    r17,4(sp)       // push return code
	orh	ha%_mach_init_routine,r0,r20
	ld.l	l%_mach_init_routine(r20),r21	// mach_init function pointer
	bte	r0,r21,.no_mach_init
	// call mach init routine
        calli	r21
	  nop
.no_mach_init:
        call _forknx
	  nop
        ld.l    0(sp),r1        // Restore return address
        ld.l    4(sp),r17       // pop r17
        adds    16,sp,sp        // deallocate stack space
	mov	r0,r16
	
.parent:
	SRET		// pid = fork()
