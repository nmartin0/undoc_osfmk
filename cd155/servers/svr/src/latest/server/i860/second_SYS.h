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
#include <sys/syscall.h>

#include <i860/asm.h>

#undef SYSCALL
#define	SYSCALL(x)	.globl	LCL(second_cerror);	\
			ENTRY(second_/**/x); 		\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	LCL(second_cerror);	\
			 nop

#undef PSEUDO
#define	PSEUDO(x,y)	ENTRY(second_/**/x);		\
			or	SYS_/**/y,r0,r31;	\
			trap	r31,r31,r0;		\
			bri	r1;			\
			nop



