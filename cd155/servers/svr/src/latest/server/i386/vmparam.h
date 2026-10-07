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
 * $Log: vmparam.h,v $
 * Revision 1.5  1994/11/18  20:31:28  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1994/04/28  19:03:40  chrisp
 * Changes emulator size for i386 checkpoint/restart.
 *
 *  Reviewer: slk
 *  Risk: L
 *  Benefit or PTS #: 9188
 *  Testing: Builds, builds, builds.
 *  Module(s): vmparam.h
 *
 * Revision 1.3  1993/07/14  17:57:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:16:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:21:09  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:20:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:13:56  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1991/12/16  18:43:54  roy
 * 	91/11/26  15:33:24  sp
 * 	Upgrade to 1.0.3
 *
 * Revision 2.3  91/10/14  12:15:35  sjs
 * 	Revision 3.2  91/10/04  16:39:37  sp
 * 	Add EMULATOR_SIZE
 * 
 * Revision 2.2  91/08/31  13:37:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:58:24  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6.4.2  91/05/28  09:56:48  dwm
 * 	Increased default stack to 6Mb from 0.5Mb.
 * 	[91/05/28  09:55:20  dwm]
 * 
 * Revision 1.6  90/10/07  13:48:26  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:55:04  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:46:15  gm]
 * 
 * Revision 1.5  90/08/09  13:19:51  devrcs
 * 	Fixed bad merge.
 * 	[90/07/25  12:39:42  kevins]
 * 
 * 	USRTEXT is 0x10000 [kupfer]
 * 	[90/02/28  15:48:44  rvb]
 * 
 * Revision 1.4  90/07/27  08:49:30  devrcs
 * 	Merged changes for 386 code
 * 	[90/07/23  12:24:16  kevins]
 * 
 * 	Set USRDATA and USRTEXT.
 * 	[90/07/17  17:07:25  jd]
 * 
 * Revision 1.3  90/07/17  11:33:09  devrcs
 * 	Updates for SS4
 * 	[90/06/29  09:35:05  kevins]
 * 
 * Revision 1.4  89/03/09  20:04:34  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:35:12  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 0.0  88/12/31            rvb
 * 	Derived from MACH2.0 vax release.
 * 	A lot of trash could be flushed from this file.
 * 	[88/12/31            rvb]
 * 
 * Revision 0.0  87/09/30            sanzi
 * 	Added definition SIGCODE_SIZE to kernel only clause.
 * 	[87/09/30            sanzi]
 * 
 * $EndLog$
 */
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)vmparam.h	7.1 (Berkeley) 6/5/86
 */

/*
 * Machine dependent constants for I386
 */
/*
 * Base address for U*X system call emulator.
 */
#define EMULATOR_BASE	0xa0000000
#define EMULATOR_END	0xa0060000
				/* 384 Kbytes */
#define	EMULATOR_SIZE	EMULATOR_END-EMULATOR_BASE

/*
 * Define base for user mapped files
 */

#define MAP_FILE_BASE	0x90000000

/*
 * i386 stack sits above emulator.
 */
#define EMULATOR_ABOVE_STACK	0

/*
 * USRTEXT is the start of the user text/data space, while USRSTACK
 * is the top (end) of the user stack.  LOWPAGES and HIGHPAGES are
 * the number of pages from the beginning of the P0 region to the
 * beginning of the text and from the beginning of the P1 region to the
 * beginning of the stack respectively.
 */
#define USRTEXT		0x10000

#define USRSTACK	(0xc0000000-sizeof(int [5]))	/* must agree with sigcode above */

/*
 * Virtual memory related constants, all in bytes
 */
#ifndef	MAXTSIZ
#define MAXTSIZ		(6*1024*1024)		/* max text size */
#endif
#ifndef	DFLDSIZ
#define DFLDSIZ		(16*1024*1024)		/* initial data size limit */
#endif
#ifndef	MAXDSIZ
#define MAXDSIZ		(16*1024*1024)		/* max data size */
#endif
#ifndef	DFLSSIZ
#define DFLSSIZ		(2*1024*1024)		/* initial stack size limit */
#endif
#ifndef	MAXSSIZ
#define MAXSSIZ		MAXDSIZ			/* max stack size */
#endif

#include <mach/i386/vm_param.h>
