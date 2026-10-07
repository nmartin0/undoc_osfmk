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
 * $Log: inline_lock.h,v $
 * Revision 1.4  1994/11/18  20:30:34  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:55:57  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:18  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:46  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:07  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:34:16  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.1  91/08/05  13:57:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.3  90/10/07  13:39:22  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:32:06  gm]
 * 
 * Revision 1.2  90/08/24  12:01:10  devrcs
 * 	Add fast inline functions when building with GNU-C.
 * 	[90/08/11  11:16:40  brezak]
 * 
 * $EndLog$
 */

/*
 * Define inline functions to be used with compilers that can benefit
 * from this. For the multimax we define simple lock and spl macros.
 */
#if	__GNUC__
#if	MACH_LDEBUG
/*
 * When debugging simple locks on the PMAX we use the standard
 * functions in lock.c. Otherwise we can use the inlined functions.
 * By defining _NO_INLINE_LOCKS here we tell lock.h to not declare the standard
 * inline functions. This causes us to use the functions in lock.c.
 */
#define _NO_INLINE_LOCKS 1
#endif	/* MACH_LDEBUG */
#endif	/* __GNUC__ */
