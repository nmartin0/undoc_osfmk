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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: vadvise.h,v $
 * Revision 1.5  1994/11/18  20:42:14  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:31:01  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:40:09  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:21:04  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.2  1992/11/30  22:45:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:43:14  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:42:24  cfj
 * Bump major revision number.
 * Revision 1.1.1.1  1993/05/03  17:43:56  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.3  1991/10/15  11:42:27  roy
 * Fix revision history comments.
 *
 * Revision 2.2  91/10/14  13:10:18  sjs
 * 	Initial revision for this tree.
 * 
 * Revision 3.0  91/09/13  12:47:52  sp
 * needed for mmap
 * 
 * Revision 1.3  90/10/07  14:56:43  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:49:08  gm]
 * 
 * Revision 1.2  90/01/02  20:16:14  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:51  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 2.3  89/03/09  22:10:08  rpd
 * 	More cleanup.
 * 
 * Revision 2.2  89/02/25  17:58:14  gm0w
 * 	Changes for cleanup.
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)vadvise.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_VADVISE_H_
#define _SYS_VADVISE_H_

/*
 * Parameters to vadvise() to tell system of particular paging
 * behaviour:
 *	VA_NORM		Normal strategy
 *	VA_ANOM		Sampling page behaviour is not a win, don't bother
 *			Suitable during GCs in LISP, or sequential or random
 *			page referencing.
 *	VA_SEQL		Sequential behaviour expected.
 *	VA_FLUSH	Invalidate all page table entries.
 */
#define VA_NORM	0
#define VA_ANOM	1
#define VA_SEQL	2
#define VA_FLUSH 3

#endif	_SYS_VADVISE_H_
