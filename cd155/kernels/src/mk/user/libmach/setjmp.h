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
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
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
 * $Log: setjmp.h,v $
 * Revision 1.5  1994/11/18  21:05:55  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:04:39  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:58:57  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:02  dleslie
 * First R1_0 release
 *
 * Revision 2.8.1.1  92/05/27  10:04:26  jeffreyh
 * 	use  __i860__ instead of i860
 * 
 * Revision 2.8  91/09/04  11:29:03  jsb
 * 	From Intel SSD: added i860 support.
 * 	[91/09/04  10:14:32  jsb]
 * 
 * Revision 2.7  91/07/09  23:24:06  danner
 * 	Added luna88k support, some missing comments.
 * 	[91/06/27            danner]
 * 
 * Revision 2.6  91/05/14  17:54:56  mrt
 * 	Correcting copyright
 * 
 * Revision 2.5  91/02/14  14:18:43  mrt
 * 	Added new Mach copyright
 * 	[91/02/13  12:44:53  mrt]
 * 
 * Revision 2.4  90/11/05  14:36:03  rpd
 * 	Changed the mips jmp_buf definition to the normal size.
 * 	[90/10/30            rpd]
 * 
 * Revision 2.3  90/05/03  15:54:23  dbg
 * 	Add i386 definitions.
 * 	[90/02/05            dbg]
 * 
 * Revision 2.2  89/11/29  14:18:52  af
 * 	Added mips defs.  This file is only ok for standalone Mach use,
 * 	not inside U*x.
 * 	[89/10/28  10:23:02  af]
 * 
 * Revision 2.1  89/08/03  17:06:57  rwd
 * Created.
 * 
 * Revision 2.3  88/12/22  17:06:02  mja
 * 	Correct __STDC__ return value type and allow for recursive inclusion.
 * 	[88/12/20            dld]
 * 
 * Revision 2.2  88/12/14  23:34:14  mja
 * 	Added ANSI-C (and C++) compatible argument declarations.
 * 	[88/01/18            dld@cs.cmu.edu]
 * 
 *  3-Sep-87  Michael Jones (mbj) at Carnegie-Mellon University
 *	Added definition of jump buffer for Multimax.
 *
 * 29-May-87  Robert Baron (rvb) at Carnegie-Mellon University
 *	ns32000 jmp_buf for sequent and possibly mmax
 *
 * 17-Nov-86  Jonathan Chew (jjc) at Carnegie-Mellon University
 *	Added defintion of jump buffer for SUN.
 *
 * 22-Sep-86  Glenn Marcy (gm0w) at Carnegie-Mellon University
 *	Added changes for IBM RT.
 *
 */

/*	setjmp.h	4.1	83/05/03	*/

#ifndef _SETJMP_H_PROCESSED_
#define _SETJMP_H_PROCESSED_ 1

#ifdef multimax
typedef int jmp_buf[10];
#endif /* multimax */

#ifdef	balance
typedef int jmp_buf[11];	/* 4 regs, ... */
#endif	/* balance */

#ifdef sun3
typedef int jmp_buf[15];	/* pc, sigmask, onsstack, d2-7, a2-7 */
#endif /* sun3 */

#ifdef ibmrt
typedef int jmp_buf[16];
#endif /* imbrt */

#ifdef vax
typedef int jmp_buf[10];
#endif /* vax */

#ifdef mips
typedef int jmp_buf[75];
#endif /* mips */

#ifdef i386
typedef int jmp_buf[21];
#endif /* i386 */

#ifdef luna88k
typedef int jmp_buf[19]; /* r1, r14-r31 */
#endif /* luna88k */

#ifdef __i860__
typedef int jmp_buf[32];  /* f2 - f15, r1 - r16 registers */
#endif /* __i860__ */

#if defined(CMUCS) && defined(__STDC__)
extern int setjmp (jmp_buf);
extern void longjmp (jmp_buf, int);
extern int _setjmp (jmp_buf);
extern void _longjmp (jmp_buf, int);
#endif

#endif /* _SETJMP_H_PROCESSED_ */
