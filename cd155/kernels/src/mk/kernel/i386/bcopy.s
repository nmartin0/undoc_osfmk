// 
// $Copyright
// Copyright 1991 , 1994, 1995 Intel Corporation
// INTEL CONFIDENTIAL
// The technical data and computer software contained herein are subject
// to the copyright notices; trademarks; and use and disclosure
// restrictions identified in the file located in /etc/copyright on
// this system.
// Copyright$
// 
 
/*
 * Mach Operating System
 * Copyright (c) 1991,1990 Carnegie Mellon University
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
 * $Log: bcopy.s,v $
// Revision 1.5  1994/11/18  20:32:12  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  22:23:13  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:21:48  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  18:21:20  dleslie
// First R1_0 release
//
 * Revision 2.5  91/05/14  16:03:58  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:10:49  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:41  mrt]
 * 
 * Revision 2.3  90/11/05  14:26:58  rpd
 * 	Introduce bcopy16.  For 16bit copies to bus memory
 * 	[90/11/02            rvb]
 * 
 * Revision 2.2  90/05/03  15:24:53  dbg
 * 	From Bob Baron.
 * 	[90/04/30            dbg]
 * 
 */

#include <i386/asm.h>

/ bcopy(from, to, bcount)

ENTRY(bcopy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	movl	B_ARG0,%esi
	movl	B_ARG1,%edi
bcopy_common:
	movl	B_ARG2,%edx
	cld
/ move longs
	movl	%edx,%ecx
	sarl	$2,%ecx
	rep
	movsl
/ move bytes
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	movsb
	popl	%esi
	popl	%edi
	leave
	ret


/ memcpy(to, from, count)

ENTRY(memcpy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	movl	B_ARG0,%edi
	movl	B_ARG1,%esi
	jmp	bcopy_common

/ bcopy16(from, to, bcount) using word moves

ENTRY(bcopy16)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
/	movl	8+12(%esp),%edx		/  8 for the two pushes above
/	movl	8+ 8(%esp),%edi
/	movl	8+ 4(%esp),%esi
	movl	B_ARG2, %edx
	movl	B_ARG1, %edi
	movl	B_ARG0, %esi
/ move words
0:	cld
	movl	%edx,%ecx
	sarl	$1,%ecx
	rep
	movsw
/ move bytes
	movl	%edx,%ecx
	andl	$1,%ecx
	rep
	movsb
	popl	%esi
	popl	%edi
	leave
	ret	

