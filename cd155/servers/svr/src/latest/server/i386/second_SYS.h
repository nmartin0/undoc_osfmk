/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
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
 *
 *	@(#)SYS.h	5.4 (Berkeley) 6/27/88
 */

/* WARNING: The following assumes that the parent server, under which
 * this server will run as a user process, has the same system call numbers
 * as the UNIX version under which we are compiling.  This may or may not
 * true, but it is the best we can do without further knowledge.
 */
#include </usr/include/sys/syscall.h>

#ifdef	SYS_sigvec
#else	/* SYS_sigvec */
/*
 * Compiling on an OSF/1 system that doesn't have the sigvec system call.
 * We could use sigaction() instead, but this would be incompatible with
 * running the server from a BSD server, so add this unclean backward
 * compatible define
 */
#define SYS_sigvec 108
#endif	/* SYS_sigvec */

#include <machine/asm.h>

#define	SYSCALL(x)	ENTRY(second_/**/x); movl	$SYS_/**/x, %eax; SVC; jb LCL(second_cerror)

#define	PSEUDO(x,y)	ENTRY(second_/**/x); movl	$SYS_/**/y, %eax; SVC
#define	CALL(x,y)	calls $x, EXT(y)

	.globl	LCL(second_cerror)
