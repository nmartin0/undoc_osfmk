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
 * $Log: bzero.s,v $
// Revision 1.5  1994/11/18  20:32:16  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  22:23:14  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:21:50  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  18:21:22  dleslie
// First R1_0 release
//
 * Revision 2.4  91/05/14  16:04:14  mrt
 * 	Correcting copyright
 * 
 * Revision 2.3  91/02/05  17:10:53  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  17:30:45  mrt]
 * 
 * Revision 2.2  90/05/03  15:24:56  dbg
 * 	From Bob Baron.
 * 	[90/04/30            dbg]
 * 
 */

#include <i386/asm.h>

/*
 * bzero(char * addr, unsigned int length)
 */
Entry(blkclr)
ENTRY(bzero)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	movl	B_ARG1,%edx
	movl	B_ARG0,%edi
	xorl	%eax,%eax
	cld
/ zero longs
	movl	%edx,%ecx
	shrl	$2,%ecx
	rep
	stosl
/ zero bytes
	movl	%edx,%ecx
	andl	$3,%ecx
	rep
	stosb
	popl	%edi
	leave
	ret
