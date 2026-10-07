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
 * @OSF_COPYRIGHT@
 */
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: ptrace.h,v $
 * Revision 1.4  1994/11/18  20:41:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  18:26:43  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:50  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:43:18  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:56  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:39:50  cfj
 * Bump major revision number.
 *
 * Revision 2.3  1991/10/17  11:18:57  sjs
 * fixed RCS messages
 *
 * Revision 2.2  91/10/14  13:08:25  sjs
 * Check in
 * 
 * Revision 3.0  91/10/03  10:30:45  jose
 * Original version from 1.0.2
 * 
 * Revision 1.3  90/10/07  14:53:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:28  gm]
 * 
 * Revision 1.2  90/01/03  12:43:48  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:41:26  gm]
 * 
 * Revision 1.1  89/10/16  19:48:28  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:21:55  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.7  89/05/30  10:43:47  rvb
 * 	Cleaned up mips case: include separate file like everybody else.
 * 	[89/04/20            af]
 * 
 * Revision 2.6  89/03/09  22:06:49  rpd
 * 	More cleanup.
 * 
 * Revision 2.5  89/02/25  17:55:36  gm0w
 * 	Changed MULTIMAX to multimax and removed include of cputypes.h
 * 	[89/02/14            mrt]
 * 
 * Revision 2.4  89/01/23  22:28:09  af
 * 	Added mips register offsets
 * 	[89/01/10            af]
 * 
 * Revision 2.3  89/01/18  01:18:29  jsb
 * 	Sun: include sun/ptrace.h.
 * 	[89/01/13            jsb]
 * 
 * Revision 2.2  88/08/24  02:40:17  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:20:42  mwyoung]
 * 
 * $EndLog$
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.  All Rights Reserved
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * Copyright (c) 1980, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)ptrace.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_PTRACE_H_
#define _SYS_PTRACE_H_

#define PT_TRACE_ME	0	/* child declares it's being traced */
#define PT_READ_I	1	/* read word in child's I space */
#define PT_READ_D	2	/* read word in child's D space */
#define PT_READ_U	3	/* read word in child's user structure */
#define PT_WRITE_I	4	/* write word in child's I space */
#define PT_WRITE_D	5	/* write word in child's D space */
#define PT_WRITE_U	6	/* write word in child's user structure */
#define PT_CONTINUE	7	/* continue the child */
#define PT_KILL		8	/* kill the child process */
#define PT_STEP		9	/* single step the child */

#ifdef mips
#include <mips/mips_ptrace.h>
#endif mips

#ifdef	multimax
#include <mmax/mmax_ptrace.h>
#endif	multimax

#ifdef	sun
#include <sun/ptrace.h>
#endif	sun

#endif	_SYS_PTRACE_H_
