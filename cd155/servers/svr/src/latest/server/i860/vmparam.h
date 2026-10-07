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
 * Copyright (c) 1991 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: vmparam.h,v $
 * Revision 1.13  1994/11/18  20:32:20  mtm
 * Copyright additions/changes
 *
 * Revision 1.12  1994/10/31  23:39:21  jlitvin
 * For BETA, temporarily lower the maximum stack segment size from 511MB
 * to 128MB.  This allows "csh; limit stacksize unlimited; NX_program" to
 * successfully run.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11445
 *  Testing: PTS test case, MACS and NQS EATs
 *  Module(s): server/i860/vmparam.h
 *
 * Revision 1.11  1994/10/19  00:52:06  suri
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 10037
 *  Testing: Specific testcase and fileio, pthreads, message_passing
 * 	  xtrnl and PFS EATs
 *  Module(s): Set DFLSSIZ to 64M and MAXSSIZ to 511M
 *
 * Revision 1.10  1994/07/20  15:34:27  suri
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 10037
 *  Testing: None; The change for PTS-10037 is backed out since it exposed a
 * 	  serious problem with memory allocation in several library routines.
 * 	  See PTS-10193, 10245, 10246, 10275 and 10276 for more details.
 *  Module(s): DFLSSIZ and MAXSSIZ macros
 *
 * Revision 1.8  1993/10/06  00:03:35  cfj
 * Move EMULATOR_BASE to 0x60000000 to support the GP node memory daughter card.
 *
 * Revision 1.7  1993/07/14  18:00:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:19:24  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:11:11  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:06:00  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/20  01:39:42  brad
 * Bumped EMULATOR_END up yet again.
 *
 * Revision 1.1.2.1.2.1  1993/02/16  20:04:04  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/28  18:25:12  cfj
 * Set DFLSSIZ back down to 2MB.
 *
 * Revision 1.3  1993/01/22  17:31:37  terry
 * modified the EMULATOR_END address to 0x80000 from 0x70000 to allow more
 * dynamic space
 *
 * Revision 1.2  1992/11/30  22:22:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:21:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.8  1992/10/14  17:17:04  cfj
 * NX integration.
 *
 * Revision 2.8  1993/02/22  13:39:57  yazz
 * Increase the maximum emulator size from 256K to 384K, for Paragon.
 *
 * Revision 2.7  92/06/08  18:20:38  pjg
 * 	USRSTACK now defined in terms of VM_MAX_ADDRESS.  Decrease DFLSSIZ
 * 	to 4MB. (stans@intel)
 * 
 * Revision 2.6  92/05/13  11:45:16  loverso
 * 	92/05/11  16:54:42  stans
 * 	Reduced MAX stack size to 64 MegaBytes from 2 GigaBytes.  Now we can 
 * 	have more than 11 pthreads.  Reset TEXT & Data start VA.
 * 
 * Revision 2.5  92/05/01  10:29:36  rabii
 * 	boost MAX text & data to Paragon limits. (stans)
 * 
 * Revision 2.4  92/01/09  16:59:25  roy
 * 	91/12/20  10:13:39  stans
 * 	DFLDSIZ (6*1024*1024) --> (16*1024*1024)
 * 	DFLSSIZ (512*1024) -->(2*1024*1024)
 * 
 * Revision 2.3  91/12/19  12:47:29  roy
 * 	Add EMULATOR_SIZE.
 * 
 * Revision 2.2  91/11/13  12:23:45  rabii
 * 	Initial checkin
 * 
 * Revision 2.2  91/09/03  11:12:58  jsb
 * 	First checkin. Derived from i386 sources by Intel SSD.
 * 	[91/09/03  10:20:58  jsb]
 * 
 * Revision 2.3  90/05/29  20:23:59  rwd
 * 	Put shared region below emulator for now.
 * 	[90/05/22            rwd]
 * 
 * Revision 2.2  90/05/21  13:54:25  dbg
 * 	Altered for u*x server/emulator.  Changed USRTEXT to the value
 * 	actually used.
 * 	[90/03/14            dbg]
 * 
 * Revision 1.4  89/03/09  20:04:34  rpd
 * 	More cleanup.
 * 
 * Revision 1.3  89/02/26  12:35:12  gm0w
 * 	Changes for cleanup.
 * 
 * 31-Dec-88  Robert Baron (rvb) at Carnegie-Mellon University
 *	Derived from MACH2.0 vax release.
 *	A lot of trash could be flushed from this file.
 *
 * 30-Sep-87  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	Added definition SIGCODE_SIZE to kernel only clause.
 *
 */ 
 
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)vmparam.h	7.1 (Berkeley) 6/5/86
 */

/*
 * Machine dependent constants for I860
 */
/*
 * Base address for U*X system call emulator.
 */
#define	EMULATOR_BASE	0x60000000
#define	EMULATOR_END	0x600b0000
				/* 704 Kbytes */
#define	EMULATOR_SIZE	EMULATOR_END-EMULATOR_BASE

/*
 * i860 stack sits above emulator.
 */
#define	EMULATOR_ABOVE_STACK	0

/*
 * USRTEXT is the start of the user text/data space, while USRSTACK
 * is the top (end) of the user stack.  LOWPAGES and HIGHPAGES are
 * the number of pages from the beginning of the P0 region to the
 * beginning of the text and from the beginning of the P1 region to the
 * beginning of the stack respectively.
 */
/* MUST MATCH what ld860 does!! */
#define USRTEXT		0x10000
#define USRDATA		0x4000000
 /* BOGUS for now; it really floats, immediately after the DATA section. */
#define USRBSS		(USRDATA+0x100000)

/* see mk/kernel/mach/i860/vm_param.h */
#define USRSTACK	VM_MAX_ADDRESS

/*
 * Virtual memory related constants, all in bytes
 */
#ifndef	MAXTSIZ
#define MAXTSIZ		(256*1024*1024)		/* max text size */
#endif
#ifndef	DFLDSIZ
#define DFLDSIZ		(256*1024*1024)		/* initial data size limit */
#endif
#ifndef	MAXDSIZ
#define MAXDSIZ		(2048*1024*1024)	/* max data size */
#endif
#ifndef	DFLSSIZ
#define DFLSSIZ		(64*1024*1024)		/* initial stack size limit */
#endif
#ifndef	MAXSSIZ
#define MAXSSIZ		(128*1024*1024)		/* max stack size */
#endif

#include <mach/i860/vm_param.h>


