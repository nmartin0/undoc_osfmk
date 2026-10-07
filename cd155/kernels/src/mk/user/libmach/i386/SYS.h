/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 */
/*
 * HISTORY
 * $Log: SYS.h,v $
 * Revision 1.5  1994/11/18  21:06:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:05:04  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:21  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:26  dleslie
 * First R1_0 release
 *
 * Revision 2.4.2.1  92/03/28  10:21:26  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:39:20  jeffreyh]
 * 
 * Revision 2.4  92/02/19  15:10:46  elf
 * 	Add __STDC__ conditonal.
 * 	[92/02/07            jvh]
 * 
 * Revision 2.3  92/01/23  15:21:59  rpd
 * 	Removed dependency on syscall.h by adding
 * 	an explicit definition of SYS_fork.
 * 	[92/01/16            rpd]
 * 
 * Revision 2.2  92/01/16  00:03:45  rpd
 * 	Moved from user collection to mk collection.
 * 
 * Revision 2.2  91/04/11  11:43:16  mrt
 * 	Copied from libc.a
 * 
 * Revision 2.2  91/03/18  17:26:03  rpd
 * 	Created.
 * 	[91/03/18            rpd]
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

#include <machine/asm.h>

#ifdef __STDC__
#define	SYSCALL(x)	ENTRY(x); movl	$(SYS_ ## x), %eax; SVC; jb LCL(cerror)
#define	PSEUDO(x,y)	ENTRY(x); movl	$(SYS_ ## y), %eax; SVC
#else __STDC__
#define	SYSCALL(x)	ENTRY(x); movl	$SYS_/**/x, %eax; SVC; jb LCL(cerror)
#define	PSEUDO(x,y)	ENTRY(x); movl	$SYS_/**/y, %eax; SVC
#endif __STDC__
#define	CALL(x,y)	calls $x, EXT(y)

	.globl	LCL(cerror)
