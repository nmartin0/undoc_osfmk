/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: eflags.h,v $
 * Revision 1.4  1994/11/18  20:30:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:55:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:14:55  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:22  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:12:50  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:33:15  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.0  91/01/17  12:10:24  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/05/21  13:52:32  dbg
 * 	Created.
 * 	[90/02/08            dbg]
 * 
 */

#ifndef	_I386_EFLAGS_H_
#define	_I386_EFLAGS_H_

/*
 *	i386 flags register
 */
#define	EFL_CF		0x00000001		/* carry */
#define	EFL_PF		0x00000004		/* parity of low 8 bits */
#define	EFL_AF		0x00000010		/* carry out of bit 3 */
#define	EFL_ZF		0x00000040		/* zero */
#define	EFL_SF		0x00000080		/* sign */
#define	EFL_TF		0x00000100		/* trace trap */
#define	EFL_IF		0x00000200		/* interrupt enable */
#define	EFL_DF		0x00000400		/* direction */
#define	EFL_OF		0x00000800		/* overflow */
#define	EFL_IOPL	0x00003000		/* IO privilege level: */
#define	EFL_IOPL_KERNEL	0x00000000			/* kernel */
#define	EFL_IOPL_USER	0x00003000			/* user */
#define	EFL_NT		0x00004000		/* nested task */
#define	EFL_RF		0x00010000		/* resume without tracing */
#define	EFL_VM		0x00020000		/* virtual 8086 mode */

#define	EFL_USER_SET	(EFL_IF)
#define	EFL_USER_CLEAR	(EFL_IOPL|EFL_NT|EFL_RF)

#endif	_I386_EFLAGS_H_
