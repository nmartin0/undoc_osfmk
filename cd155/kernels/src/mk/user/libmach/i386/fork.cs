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
 * Copyright (c) 1992 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon 
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * 02-Oct-92  Dave Mitchel (dwm) at Open Software Foundation
 * In-line code for cerror to avoid dependencies on (_)cerror,
 * which is different on 1.1/1.0.4/Mach2.6.  Yuck.
 *
 * $Log: fork.cs,v $
 * Revision 1.5  1994/11/18  21:06:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:05:17  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:32  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:38  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:22:50  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:39:01  jeffreyh]
 * 
 * Revision 2.5  92/02/19  16:30:32  elf
 * 	Cloned from the tahoe vax/fork.s; added call to mach_init
 * 	on child.
 * 	[92/02/19            rvb]
 * 
 */

/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that: (1) source distributions retain this entire copyright
 * notice and comment, and (2) distributions including binaries display
 * the following acknowledgement:  ``This product includes software
 * developed by the University of California, Berkeley and its contributors''
 * in the documentation or other materials provided with the distribution
 * and in all advertising materials mentioning features or use of this
 * software. Neither the name of the University nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#if defined(SYSLIBC_SCCS) && !defined(lint)
	.asciz "@(#)fork.s	5.6 (Berkeley) 6/1/90"
#endif /* SYSLIBC_SCCS and not lint */

#include <i386/asm.h>

#define SYS_fork	2

	.globl	EXT(errno)

ENTRY(fork)
	movl	$(SYS_fork), %eax
	SVC
	jb	1f	/* jb LCL(cerror) */
	orl	%edx, %edx
	jz	0f	/* parent, since r1 == 0 in parent, 1 in child */
	call	_mach_init
	xorl	%eax, %eax
0:
	ret		/* pid = fork() */

1:			/* in-line cerror w/o _Seterrno */
	movl    %eax, EXT(errno)
	movl    $-1, %eax
	ret
