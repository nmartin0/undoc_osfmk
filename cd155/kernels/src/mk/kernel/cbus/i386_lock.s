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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */

/*
 * HISTORY
 * $Log: i386_lock.s,v $
// Revision 1.5  1994/11/18  20:25:44  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  22:16:43  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:14:17  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  18:13:53  dleslie
// First R1_0 release
//
 * Revision 2.1  91/06/27  05:01:44  bernadat
 * Created.
 * 
 */

/*
 * Copyright 1990 by Open Software Foundation,
 * Grenoble, FRANCE
 *
 * 		All Rights Reserved
 * 
 *   Permission to use, copy, modify, and distribute this software and
 * its documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and
 * that both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of OSF or Open Software
 * Foundation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior
 * permission.
 * 
 *   OSF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL OSF BE LIABLE FOR ANY SPECIAL, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN ACTION OF CONTRACT,
 * NEGLIGENCE, OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <cpus.h>
#include <i386/asm.h>

#define	LOCK_DEBUG 1

#if	NCPUS > 1

Entry(simple_lock_init)
ENTRY(simple_unlock)
	movl	0x04(%esp),%eax	
	lock
	btr	$0,(%eax)
	ret

ENTRY(simple_lock)
	movl	0x04(%esp),%eax	
1:
	lock
	bts	$0,(%eax)
#ifdef	LOCK_DEBUG
	jb	2f
#else	LOCK_DEBUG
	jb	1b
#endif	LOCK_DEBUG
	ret
2:
	jmp	EXT(retry_simple_lock)

ENTRY(simple_lock_try)
	movl	0x04(%esp),%eax	
	lock
	bts	$0,(%eax)
	jb	1f
	movl	$1, %eax
	ret
1:
	xorl	%eax, %eax
	ret

ENTRY(i_bit_set)
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	lock
	bts	%dl, (%eax)
	ret

ENTRY(i_bit_clear)
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	lock
	btr	%dl, (%eax)
	ret

ENTRY(bit_lock)
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	movl	%edx, %ecx
	shrl	$7, %edx
	addl	%edx, %eax
1:
	lock
	bts	%ecx, (%eax)
#ifdef	LOCK_DEBUG
	jb	2f
#else	LOCK_DEBUG
	jb	1b
#endif	LOCK_DEBUG
	ret
2:
	jmp	EXT(retry_bit_lock)

ENTRY(bit_lock_try)
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	movl	%edx, %ecx
	shrl	$7, %edx
	addl	%edx, %eax
	lock
	bts	%ecx, (%eax)
	jb	bit_lock_failed
	ret			/ %eax better not be null !
bit_lock_failed:
	xorl	%eax, %eax
	ret

ENTRY(bit_unlock)
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	movl	%edx, %ecx
	shrl	$7, %edx
	addl	%edx, %eax
	lock
	btr	%ecx, (%eax)
	ret

#endif	NCPUS > 1
