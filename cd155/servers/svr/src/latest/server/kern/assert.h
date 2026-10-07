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
 * $Log: assert.h,v $
 * Revision 1.5  1994/11/18  20:32:29  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:01:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:05  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:25  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:22:51  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:50  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/04/05  16:55:04  pjg
 * 	support ANSI C (PGI compiler) version of assert() macro.
 * 	(rabii for stans)
 *
 * Revision 2.2  91/08/31  13:37:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/08/27  14:39:49  sp
 * fix ASSERT and assert redefinition problem
 * 
 * Revision 3.1  91/08/07  16:59:23  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  13:49:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:48:28  gm]
 * 
 * Revision 1.5  90/08/09  13:20:23  devrcs
 * 	Changed assert macro to print the assertion itself.
 * 	[90/08/02  12:48:42  gmf]
 * 
 * Revision 1.4  90/06/22  20:13:05  devrcs
 * 	nags merge
 * 	[90/06/12  21:23:30  gmf]
 * 
 * Revision 2.1  90/03/19  11:38:34  gmf
 * 	submitted to osc.8
 * 
 * Revision 1.2  90/03/12  23:30:09  gmf
 * 	file layer parallelization changes
 * 
 * Revision 1.1  90/03/09  16:36:55  gmf
 * 	Initial revision
 * 
 * Revision 1.2  90/01/03  11:52:56  gm
 * 	Fixes for first snapshot.
 * 	[90/01/03  09:28:56  gm]
 * 
 * Revision 1.1  89/10/16  19:35:03  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:14:54  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.8  89/05/26  14:23:11  alan
 * 	multimax version of ASSERT.
 * 
 * Revision 2.10  89/03/09  20:10:48  rpd
 * 	More cleanup.
 * 
 * Revision 2.9  89/02/25  17:59:55  gm0w
 * 	Changed CMUCS_ASSERT to MACH_ASSERTIONS
 * 	Put entire file under #ifdef KERNEL
 * 	[89/02/15            mrt]
 * 
 * Revision 2.8  89/02/07  01:01:43  mwyoung
 * 	Relocated from sys/assert.h
 * 
 * Revision 2.7  88/12/19  02:41:59  mwyoung
 * 	It appears to be impossible to avoid lint for expressions
 * 	of the form (constant1 < constant2).  Make assert_static empty
 * 	for lint.
 * 	[88/12/17            mwyoung]
 * 
 * Revision 2.6  88/10/18  03:37:27  mwyoung
 * 	Use MACRO_BEGIN, MACRO_END.
 * 	[88/10/11            mwyoung]
 * 
 * 	Avoid lint warnings about constants in the "while" clause.
 * 	[88/10/06            mwyoung]
 * 
 * Revision 2.5  88/10/01  21:58:26  rpd
 * 	Changed CS_ASSERT to CMUCS_ASSERT
 * 	[88/10/01  21:32:39  rpd]
 * 
 * Revision 2.4  88/09/25  22:15:40  rpd
 * 	Changed to use Debugger instead of panic.
 * 	[88/09/12  23:04:20  rpd]
 * 
 * Revision 2.3  88/08/24  02:22:40  mwyoung
 * 	Adjusted include file references.
 * 	[88/08/17  02:08:36  mwyoung]
 * 
 * Revision 2.2  88/07/20  16:44:48  rpd
 * 	Modify assert for kernel use.
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

/*	assert.h	4.2	85/01/21	*/

/*
 * Handy assert macro.
 */

#ifndef	_KERN_ASSERT_H_
#define _KERN_ASSERT_H_

#ifdef	KERNEL
#include <mach_assert.h>
#endif	KERNEL

/*
 * We don't want to pick up someone elses definition of ASSERT by accident
 */
#ifdef	ASSERT
#undef	ASSERT
#endif
#ifdef	assert
#undef	assert
#endif

#ifdef	multimax
#include <mmax_debug.h>
#include <mmax_idebug.h>
/*
 * The ASSERT macro passes the string representation of the failing
 * assertion to assfail to be printed out.  The assert macro, used by
 * CMU, doesn't try to pass the string representation because CMU
 * frequently uses multi-line assertions without escaping the newlines,
 * which would cause preprocessor errors by placing newlines inside
 * string constants.
 */
#if	!lint && (MMAX_DEBUG || MACH_LDEBUG || MMAX_IDEBUG)
#define ASSERT(EX)	((EX) ? 0 : assfail("EX", __FILE__, __LINE__))
#define	ASSCMU(EX)	((EX) ? 0 : assfail("", __FILE__, __LINE__))
#else
#define ASSERT(x)
#define	ASSCMU(x)
#endif	/* !lint && (MMAX_DEBUG || MACH_LDEBUG || MMAX_IDEBUG) */
#if	MACH_ASSERT
#define	assert(ex)		((ex) ? 0 : assfail("", __FILE__, __LINE__))
#define	assert_static(ex)	ASSCMU(ex)
#else
#define	assert(ex)
#define	assert_static(ex)
#endif
#else	/* multimax */

#include <kern/macro_help.h>

#if	MACH_ASSERT
#define ASSERT(ex)	assert(ex)

#if	__STDC__ == 1

#define assert(ex)							\
MACRO_BEGIN								\
	if (!(##ex)) {							\
		printf("Assertion failed: file: \"%s\", line: %d test: %s\n", \
		       __FILE__, __LINE__, # ex);			\
		Debugger("assertion failure");				\
	}								\
MACRO_END

#else	/* __STDC__ */

#define assert(ex)							\
MACRO_BEGIN								\
	if (!(ex)) {							\
		printf("Assertion failed: file: \"%s\", line: %d test: %s\n", \
		       __FILE__, __LINE__, "ex");			\
		Debugger("assertion failure");				\
	}								\
MACRO_END
#endif	/* __STDC__ */

#ifdef	lint
#define assert_static(x)
#else
#define assert_static(x)	assert(x)
#endif

#else	/* MACH_ASSERT */
#define ASSERT(ex)
#define assert(ex)
#define assert_static(ex)
#endif	/* MACH_ASSERT */
#endif	/* multimax */

#endif	/* _KERN_ASSERT_H_ */

