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
 * 01-Oct-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Allow MACH_LDEBUG configuration option
 *	Fixed bit lock/unlock code
 *
 * $Log: i386_lock.s,v $
// Revision 1.5  1994/11/18  20:32:57  mtm
// Copyright additions/changes
//
// Revision 1.4  1993/06/30  22:23:55  dleslie
// Adding copyright notices required by legal folks
//
// Revision 1.3  1993/04/27  20:22:33  dleslie
// Copy of R1.0 sources onto main trunk
//
// Revision 1.1.10.2  1993/04/22  18:22:13  dleslie
// First R1_0 release
//
 * Revision 2.1.2.1.2.2  92/05/26  11:19:19  jeffreyh
 * 		Since some code tests locks not using simple_lock/simple_unlock
 * 	routines (intel pmap TLB routines) but just testing the lock
 * 	word, used xchgl instead of bts (bit set).
 * 	[92/05/18            bernadat]
 * 
 * Revision 2.1.2.1.2.1  92/02/18  18:46:21  jeffreyh
 * 	Moved from cbus/i386_lock.s and adapted for MACH_LOCK_MON
 * 	[91/12/23            bernadat]
 * 
 * Revision 2.1.4.1  91/09/26  04:27:29  bernadat
 * 	Created
 * 	[91/06/27  05:02:08  bernadat]
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
#include <mach_lock_mon.h>
#include <mach_mp_debug.h>
#include <mach_ldebug.h>

/*
 * These routines are only called when MACH_MP_DEBUG is on.
 * This allows us to detect dead locks.
 * Of course they are less performant then the macros otherwise used
 */

#if	NCPUS > 1 && MACH_MP_DEBUG

/*
 * If lock monitoring is on, redefine rename entry points
 */

#if	(MACH_LOCK_MON || MACH_LDEBUG)
#define simple_lock	_simple_lock
#define simple_lock_try _simple_lock_try
#define simple_unlock	_simple_unlock
#endif	(MACH_LOCK_MON || MACH_LDEBUG)


Entry(simple_lock_init)
ENTRY(simple_unlock)
	movl	0x04(%esp),%edx	
	xorl	%eax, %eax
	xchgl	(%edx), %eax
	ret

ENTRY(simple_lock)
	movl	0x04(%esp),%edx	
1:
	movl	$1, %eax
	xchgl	(%edx), %eax
	testl	%eax, %eax	
#if	MACH_MP_DEBUG
	jne	2f
#else	MACH_MP_DEBUG
	jne	1b
#endif	MACH_MP_DEBUG
	ret
#if	MACH_MP_DEBUG
2:
	jmp	EXT(retry_simple_lock)
#endif	MACH_MP_DEBUG

ENTRY(simple_lock_try)
	movl	0x04(%esp),%edx	
	movl	$1, %eax
	xchgl	(%edx), %eax
	testl	%eax, %eax	
	jne	1f
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
	movl	4(%esp), %ecx
	movl	8(%esp), %eax
1:
	lock
	bts	%ecx, (%eax)
#if	MACH_MP_DEBUG
	jb	2f
#else	MACH_MP_DEBUG
	jb	1b
#endif	MACH_MP_DEBUG
	ret
#if	MACH_MP_DEBUG
2:
	jmp	EXT(retry_bit_lock)
#endif	MACH_MP_DEBUG

ENTRY(bit_lock_try)
	movl	4(%esp), %ecx
	movl	8(%esp), %eax
	lock
	bts	%ecx, (%eax)
	jb	bit_lock_failed
	ret			/ %eax better not be null !
bit_lock_failed:
	xorl	%eax, %eax
	ret

ENTRY(bit_unlock)
	movl	4(%esp), %ecx
	movl	8(%esp), %eax
	lock
	btr	%ecx, (%eax)
	ret

#endif	NCPUS > 1 && MACH_MP_DEBUG

