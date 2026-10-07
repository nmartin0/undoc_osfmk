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
 * HISTORY
 * $Log: SYS.h,v $
 * Revision 1.5  1994/11/18  21:06:31  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:05:32  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:45  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:49  dleslie
 * First R1_0 release
 *
 * Revision 2.1.2.1  92/03/28  10:23:16  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:43:55  jeffreyh]
 * 
 *
 */
#ifndef	_I860_ASM_H_
#define	_I860_ASM_H_ 1


#ifdef	ASSEMBLER
//
//	This file contains #defines used mainly in the libc stubs to make the
//	interface as portable as possible.  Taken from AD server
//	./i860/asm.h
//
#endif	/* ASSEMBLER */

#define ALIGN 4
#define TEXT_ALIGN 16

#ifdef	__STDC__

#define EXT(x) _##x
#define LCL(x) .##x
#define MK_LABEL(a,b) a##b

#else	/*__STDC__*/

#define EXT(x) _/**/x
#define LCL(x) ./**/x
#define MK_LABEL(a,b) a/**/b

#endif	/*__STDC__*/


#define AS_SET(x)	.data; .align ALIGN; EXT(x)::
#define ASSIGN(x)	.data; .align ALIGN; EXT(x)::

#define	ENTRY(x)	.text; .align TEXT_ALIGN; EXT(x)::
#define	ASENTRY(x)	.text; .align TEXT_ALIGN; EXT(x)::

#define	RET		bri	r1;	\
			nop


/*
 * For OSF1 library generation ONLY. Libs use the following definition of
 * SYSCALL()
 */
#ifndef	OSF1_SERVER

#ifdef	__STDC__

#define	SYSCALL(x)	.globl	_cerror;		\
			ENTRY(x);			\
			or	SYS_##x,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	CALL(x)		.globl	_cerror;		\
			or	SYS_##x,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror


#define	PSEUDO(x,y)	ENTRY(x);			\
			or	SYS_##y,r0,r31;		\
			trap	r31,r31,r0;		\
			bc	_cerror

#else	/*__STDC__*/

#define	SYSCALL(x)	.globl	_cerror;		\
			ENTRY(x);			\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	CALL(x)		.globl	_cerror;		\
			or	SYS_/**/x,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#define	PSEUDO(x,y)	ENTRY(x);			\
			or	SYS_/**/y,r0,r31;	\
			trap	r31,r31,r0;		\
			bc	_cerror

#endif	/*__STDC__*/


#endif	/* OSF1_SERVER */
#endif	/* _I860_ASM_H_ */
