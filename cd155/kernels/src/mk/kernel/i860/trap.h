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
 * Copyright (c) 1991 Carnegie Mellon University
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
 * Copyright 1988, 1989, 1990, 1991 by Intel Corporation,
 * Santa Clara, California.
 * 
 *                          All Rights Reserved
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appears in all copies and that
 * both the copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Intel not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 * 
 * INTEL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
 * SHALL INTEL BE LIABLE FOR ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN ACTION OF CONTRACT, NEGLIGENCE, OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF
 * THIS SOFTWARE.
 */
/*
 * HISTORY
 * $Log: trap.h,v $
 * Revision 2.7  1994/11/18  20:39:51  mtm
 * Copyright additions/changes
 *
 * Revision 2.6  1993/06/30  22:31:42  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 2.5  1992/09/22  17:35:42  SSD
 * *** empty log message ***
 *
 * Revision 2.5  91/12/10  16:31:20  jsb
 * 	Fixes from Intel
 * 	[91/12/10  15:33:06  jsb]
 * 
 * Revision 2.4  91/06/18  20:52:01  jsb
 * 	New copyright from Intel.
 * 	[91/06/18  18:59:54  jsb]
 * 
 * Revision 2.3  91/06/17  15:45:18  jsb
 * 	Cleaned up.
 * 	[91/06/17  10:21:15  jsb]
 * 
 * Revision 2.3  90/10/25  14:45:03  rwd
 * 	Added watchpoint support.
 * 	[90/10/18            rpd]
 * 
 * Revision 2.2  90/05/03  15:38:10  dbg
 * 	Created.
 * 	[90/02/08            dbg]
 * 
 */

#ifndef	_I860_TRAP_H_
#define	_I860_TRAP_H_

/*
 * Hardware trap vectors for i860.
 */
#define	T_DEBUG			0
#define	T_SYSCALL		1		/* system call */
#define	T_BPTFLT		2		/* breakpoint instruction */
#define	T_OVERFLOW		3		/* overflow test */
#define	T_INVALID_OPCODE	4		/* invalid op code */
#define	T_PAGE_FAULT		5
#define	T_FLOATING_POINT_ERROR	6
#define	T_WATCHPOINT		7
#define T_LOCKEXPIRED		8		/* missing unlock */
#define T_UNALIGNED		9		/* unaligned data access */

/*
 * Page-fault trap codes.
 *
 *	Bit	Value	Description
 *	0	0	page not present
 *		1	page-level protection violation
 *	1	0	access was a read
 *		1	access was a write
 *	2	0	access was supervisor-mode
 *		1	access was user-mode
 *	3	0	alignment was okay
 *		1	misaligned data access
 */
#define	T_PF_PROT		0x1		/* protection violation */
#define	T_PF_WRITE		0x2		/* write access */
#define	T_PF_USER		0x4		/* from user state */

#endif	_I860_TRAP_H_
