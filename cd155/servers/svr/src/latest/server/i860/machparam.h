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
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: machparam.h,v $
 * Revision 1.7  1995/03/02  19:17:23  stans
 *  DELAY() macro never waits due to uninitialized (BSS allocated hence 0)
 *  variable 'cpuspeed'. Remove old crufty integrated kernel code, replace with
 *  definition which fits an i860. Utilized in uxkern/misc.c rtn pps_bootsync().
 *
 *  Reviewer:Len Brown, John Litvin
 *  Risk:low
 *  Benefit or PTS #:12563
 *  Testing:WW07 sats
 *
 * Revision 1.6  1994/11/18  20:31:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1993/07/14  17:59:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:18:23  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  19:09:50  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:29:59  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/04/03  03:05:58  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/14  23:12:55  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.2  1992/11/30  22:22:01  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/17  20:15:45  cfj
 * Added _i860_cpu_type() which is used when initializing ufsname.machine.
 *
 * Revision 1.1.2.1  1992/11/05  23:21:07  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.3  1992/05/01  10:29:31  rabii
 * 	chnaged "i860" --> "__i860__" ANSI names (stans)
 *
 * 
 * Revision 2.2  91/11/13  12:22:34  rabii
 * 	Initial checkin
 * 
 * Revision 2.2  91/09/03  11:12:35  jsb
 * 	dfc 2/11/91 From Jim Hurley's 2.5 version.
 * 	[91/09/02  14:19:30  jsb]
 * 
 * Revision 1.5  89/09/09  15:20:13  rvb
 * 	CLSIZE ====== 1 not 2
 * 	[89/09/07            rvb]
 * 
 * Revision 1.4  89/03/09  20:03:11  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:32:34  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *
 * 25-Jan-86  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Upgraded to 4.3
 *
 * 19-Jun-85  Mike Accetta (mja) at Carnegie-Mellon University
 *	CMUCS:  Expanded UPAGES to 12 since with the extended
 *	number of file descriptors, the old size only allowed for one
 *	kernel stack page before reaching the red-zone.
 *	[V1(1)]
 */
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)machparam.h	7.1 (Berkeley) 6/5/86
 */
#ifndef	_I860_MACHPARAM_H_
#define	_I860_MACHPARAM_H_ 1

/*
 * utsname in ../conf/param.c
 */
#ifdef	__i860__

#define MACHINE "unknown"

#define MACHINE_IPSC860 "ipsc860"
#define MACHINE_PARAGON "paragon"

#endif	/* __i860__ */

/*
 * Machine dependent constants for 80860.
 */

#define i860_XR 1
#define i860_XP 2

#define NBPG	4096		/* bytes/page */
#define PGOFSET	(NBPG-1)	/* byte offset into page */
#define PGSHIFT	12		/* LOG2(NBPG) */

#define CLSIZE		1
#define CLSIZELOG2	0

#define SSIZE	1		/* initial stack size/NBPG */
#define SINCR	1		/* increment of stack/NBPG */

#define UPAGES	16		/* pages of u-area */

/*
 * Some macros for units conversion
 */
/* Core clicks (512 bytes) to segments and vice versa */
#define ctos(x)	(x)
#define stoc(x)	(x)

/* Core clicks (512 bytes) to disk blocks */
#define ctod(x)	(x)
#define dtoc(x)	(x)
#define dtob(x)	((x)<<9)

/* clicks to bytes */
#define ctob(x)	((x)<<9)

/* bytes to clicks */
#define btoc(x)	((((unsigned)(x)+511)>>9))

/*
 * Macros to decode processor status word.
 */
#define BASEPRI(x)  ((x) != 0)

#define DELAY(n)	{ register int N = (n)*100; while (N > 0) N--; }

#endif	/* _I860_MACHPARAM_H_ */
