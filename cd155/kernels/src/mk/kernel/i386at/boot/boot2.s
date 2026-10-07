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
 * @OSF_FREE_COPYRIGHT@
 */
/*
 * HISTORY
 * boot2.s,v
// Revision 1.1.4.2  1993/02/12  17:35:41  dleslie
// Adding new code from vendor
//
 * Revision 1.1  1992/09/30  02:43:21  robert
 * Initial revision
 *
 * $EndLog$
 */
/* CMU_HIST */
/*
 * Revision 2.1.2.1  92/04/30  11:53:11  bernadat
 * 	Copied from main line
 * 	[92/04/21            bernadat]
 * 
 * Revision 2.2  92/04/04  11:35:26  rpd
 * 	From 2.5
 * 	[92/03/30            rvb]
 * 
 * Revision 2.2  91/04/02  14:39:21  mbj
 * 	Put into rcs tree
 * 	[90/02/09            rvb]
 * 
 */
/* CMU_ENDHIST */
/*
 * Mach Operating System
 * Copyright (c) 1992, 1991 Carnegie Mellon University
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
 */

#include	"machine/asm.h"

/*
 * boot2() -- second stage boot
 */

ENTRY(boot2)
	movl	%cs, %ax
	movl	%ax, %ds
	movl	%ax, %es
	/* change to protected mode */
	data16
	call	_real_to_prot

	pushl	%edx
	call	_boot
	ret
