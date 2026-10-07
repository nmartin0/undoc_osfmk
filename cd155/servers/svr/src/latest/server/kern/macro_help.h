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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: macro_help.h,v $
 * Revision 1.5  1994/11/18  20:32:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:01:42  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:48  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:16:46  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:43  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:23:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:22:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.2  1991/08/31  13:37:48  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/07  17:00:14  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  13:54:38  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:56:01  gm]
 * 
 * Revision 1.2  90/01/02  20:06:02  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:35:50  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  16:05:20  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.4  89/03/09  20:14:07  rpd
 * 	More cleanup.
 * 
 * Revision 2.3  89/02/25  18:06:34  gm0w
 * 	Kernel code cleanup.
 * 	Put entire file under #indef KERNEL.
 * 	[89/02/15            mrt]
 * 
 * Revision 2.2  88/10/18  03:36:20  mwyoung
 * 	Added a form of return that can be used within macros that
 * 	does not result in "statement not reached" noise.
 * 	[88/10/17            mwyoung]
 * 
 * 	Add MACRO_BEGIN, MACRO_END.
 * 	[88/10/11            mwyoung]
 * 
 * 	Created.
 * 	[88/10/08            mwyoung]
 * 
 * $EndLog$
 */
/*
 *	File:	kern/macro_help.h
 *
 *	Provide help in making lint-free macro routines
 *
 */

#ifndef	_KERN_MACRO_HELP_H_
#define _KERN_MACRO_HELP_H_

#include <mach/boolean.h>

#ifdef	lint
boolean_t	NEVER;
boolean_t	ALWAYS;
#else	lint
#define		NEVER		FALSE
#define		ALWAYS		TRUE
#endif	lint

#define		MACRO_BEGIN	do {
#define		MACRO_END	} while (NEVER)

#define		MACRO_RETURN	if (ALWAYS) return

#endif	_KERN_MACRO_HELP_H_
