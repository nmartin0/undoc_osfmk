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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */

/*
 * OSF/1 Release 1.0.1
 */

/*
 * **** WARNING ****
 *
 *  This code needs to be consistant with OSF1AD signal handling, see
 *  libc/IPSC860/sigvec.s and OSF1AD/emulator!!
 *
 * AS IT STANDS the code is NOT CORRECT!! [ Fri Sep 27 09:59:46 PDT 1991 ] stan
 *
 *
 * signal(sig, func);
 * int sig;
 * int (*func)();
 *
 *
 */

#include	<i860/asm.h>
#include	<syscall.h>

	.text

	.extern __sigtramp
	.extern _cerror

ENTRY(signal)
	mov	__sigtramp,r18		// ???xxx
        or      SYS_signal,r0,r31
        trap    r31,r31,r0
        bc      _cerror
	RET
