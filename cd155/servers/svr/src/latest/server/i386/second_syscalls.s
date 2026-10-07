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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

/*
 * system calls needed by second server to access the
 * primary server via unix system calls.
 * We just rename them unix_foo instead of foo not to
 * mix up with this server entries.
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)write.s	5.5 (Berkeley) 6/27/88"
#endif /* SYSLIBC_SCCS and not lint */

#include "second_SYS.h"

SYSCALL(open)
	ret

SYSCALL(fstat)
	ret

SYSCALL(access)
	ret

SYSCALL(lseek)
	ret

SYSCALL(write)
	ret

SYSCALL(read)
	ret

SYSCALL(close)
	ret

SYSCALL(ioctl)
	ret

#ifndef SYS_sigvec
#	define SYS_sigvec 108
#endif
SYSCALL(sigvec)
	ret

SYSCALL(sigreturn)
	ret

SYSCALL(getpid)
	ret

PSEUDO(_exit,exit)

	.data
	.globl	EXT(second_errno)

EXT(second_errno):
	.long	0

	.text

	.globl  LCL(second_cerror)

LCL(second_cerror):
	movl	%eax, EXT(second_errno)
	movl	$-1,%eax
	ret

