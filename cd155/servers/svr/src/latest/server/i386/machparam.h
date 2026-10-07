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
 * $Log: machparam.h,v $
 * Revision 1.4  1994/11/18  20:30:54  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:56:13  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:15:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:20:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:19:54  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:17  cfj
 * Bump major revision number.
 *
 * Revision 2.2  1991/08/31  13:35:10  rabii
 * 	Initial V2.0 Checkin
 *
 * Revision 3.2  91/08/05  13:57:49  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  13:42:25  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:36:44  gm]
 * 
 * Revision 1.5  90/08/09  13:18:16  devrcs
 * 	Merged '386 code
 * 	[90/08/06  18:50:59  kevins]
 * 
 * 	LOCORE -> ASSEMBLER
 * 	[90/07/31  13:09:36  jd]
 * 
 * Revision 1.4  90/07/17  11:31:18  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:24:33  kevins]
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
 * Revision 0.0  88/12/31            rvb
 * 	Derived from MACH2.0 vax release.
 * 	[88/12/31            rvb]
 * 
 * Revision 0.0  86/01/25            avie
 * 	Upgraded to 4.3
 * 	[86/01/25            avie]
 * 
 * Revision 0.0  85/06/19            mja
 * 	CMUCS:  Expanded UPAGES to 12 since with the extended
 * 	number of file descriptors, the old size only allowed for one
 * 	kernel stack page before reaching the red-zone.
 * 	[V1(1)]
 * 	[85/06/19            mja]
 * 
 * $EndLog$
 */
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)machparam.h	7.1 (Berkeley) 6/5/86
 */
#include  <i386/endian.h>

#define MACHINE "i386"

/*
 * Machine dependent constants for 80386.
 */
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
#define USMODE	3		/* current Xlevel == user */
#define SEL_RPL		0x03
#define USERMODE(cs)	(((cs) & SEL_RPL) == USMODE)
#define BASEPRI(x)  ((x) != 0)

#ifdef	KERNEL
#ifndef	ASSEMBLER
int	cpuspeed;
#endif  ASSEMBLER
#define DELAY(n)	{ register int N = cpuspeed * (n); while (--N > 0); }

#else	KERNEL
#define DELAY(n)	{ register int N = (n); while (--N > 0); }
#endif	KERNEL
