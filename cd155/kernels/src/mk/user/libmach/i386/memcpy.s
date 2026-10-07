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
 * $Log: memcpy.s,v $
// Revision 1.5  1994/11/18  21:06:24  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  23:05:23  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:59:38  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  19:05:45  dleslie
// First R1_0 release
//
 * Revision 2.2.2.2  92/09/15  17:37:40  jeffreyh
 * 	Set retval for memcpy a la svid/posix (bug 296).
 * 	[92/09/11            dwm]
 * 
 * Revision 2.2.2.1  92/03/28  10:23:02  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:39:32  jeffreyh]
 * 
 * Revision 2.2  92/02/19  16:02:14  elf
 * 	Created.
 * 
 */

#include <i386/asm.h>

/ memcpy(to, from, bcount)

ENTRY(memcpy)
	pushl	%ebp
	movl	%esp,%ebp
	pushl	%edi
	pushl	%esi
	movl	B_ARG0,%edi
	movl	B_ARG1,%esi
	movl	B_ARG2,%edx
	movl	%edi,%eax
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

