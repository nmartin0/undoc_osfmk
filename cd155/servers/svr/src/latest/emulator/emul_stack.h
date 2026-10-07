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
 * Copyright (c) 1990 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: emul_stack.h,v $
 * Revision 1.9  1995/04/05  06:54:20  johannes
 * new define EMUL_INIT_STACK_SIZE for use in the server
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: low
 *  Benefit or PTS #: 11007
 *  Testing: simple test with kill; kill from pthread; corefile EATs
 *  Module(s): server/paracore/core.c (adjust_thread_info)
 * 	    emulator/emul_stack.h (define for emulator stack size)
 *             emulator/emul_stack_alloc.c (use of new define)
 *
 * Revision 1.8  1994/11/18  20:23:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/08/31  22:45:44  mtm
 *    This commit is part of the R1_3 branch -> mainline collapse. This
 *    action was approved by the R1.X meeting participants.
 *
 *    Reviewer:        None
 *    Risk:            Something didn't get merged properly, or something
 *                     left on the mainline that wasn't approved for RTI
 *                     (this is VERY unlikely)
 *    Benefit or PTS#: All R1.3 work can now proceed on the mainline and
 *                     developers will not have to make sure their
 *                     changes get onto two separate branches.
 *    Testing:         R1_3 branch will be compared (diff'd) with the new
 *                     main. (Various tags have been set incase we have to
 *                     back up)
 *    Modules:         Too numerous to list.
 *
 * Revision 1.5.14.1  1994/08/10  18:51:55  rlg
 * The queue limit for the server's root vnode port was increased to
 * MACH_PORT_QLIMIT_MAX.  This change, coupled with the changes to the
 * emulator for the reply port, work around a deadlock problem with the
 * new NORMA implementation.
 *
 * Committing in R1.3 WW33 to fix PTS #10409
 *
 * Modified Files:   server/bsd/init_main.c
 *                   emulator/emul_stack.h
 *                   emulator/emul_stack_alloc.c
 *                   emulator/pfs2_user_side.c
 *
 * Revision 1.5  1993/07/14  17:31:03  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.3  1993/07/01  18:22:54  cfj
 * Adding new code from vendor
 *
 * Revision 1.4  1993/05/06  20:14:52  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:17:34  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.3  1993/04/03  03:17:39  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/03/16  01:44:33  wunder
 * Added a 4 byte padding word to top of stack structure to fix byte alignment
 * needed for some floating point operations.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:00:44  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.2  1992/11/30  22:08:31  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:15:43  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 4.1  1992/11/03  23:57:25  cfj
 * Bump major revision number.
 *
 * Revision 2.7  1992/10/06  12:03:58  roman
 * Fix RCS comments.
 *
 * Revision 2.6  92/10/05  13:41:36  klh
 * 	Revision 2.5  92/08/26  12:09:23  loverso
 * 		Define EMUL_STACK_BASE here, so that it can be used other 
 *		places to compute the base of an emulator stack w/o a function 
 *		call. (loverso)
 * 
 * Revision 2.5  92/06/05  13:54:33  klh
 * 	Revision 2.4  92/05/18  12:25:39  roy
 * 		Revision 2.3.1.1  92/04/22  09:48:32  roy
 * 		Added extern for emulthread_stack_alloc().
 * 		[92/04/06            roy]
 * 
 * Revision 2.4  92/02/17  14:23:20  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.3  92/02/11  18:56:40  pjg
 * 	Add declaration for new routine emul_stack_reinit() 
 * 	(used by migrate()) (roman@locus).
 * 
 * Revision 2.2  91/08/30  16:40:18  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.0  91/01/17  12:05:10  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.2  90/06/02  15:20:39  rpd
 * 	Created for new IPC.
 * 	[90/03/26  20:32:04  rpd]
 * 
 * $EndLog$
 */

#ifndef	_EMUL_STACK_H_
#define	_EMUL_STACK_H_

/*
 * Top of emulator stack holds link and reply port.
 */
struct emul_stack_top {
	struct emul_stack_top	*link;
	mach_port_t		reply_port;
#ifdef	PFS
	mach_port_t		pfs_reply_port;	/* for supporting concurrent */
						/*  operations thru one port */
	mach_port_msgcount_t	pfs_reply_port_q_limit;
						/* current value of the pfs_ */
						/*  reply_port queue limit   */
						/* note that the emulators   */
						/*  stack is 8 byte aligned  */
#endif
};
typedef	struct emul_stack_top	*emul_stack_t;

extern emul_stack_t		emul_stack_init();
extern emul_stack_t		emul_stack_reinit();
extern emul_stack_t 		emul_stack_alloc();
extern emul_stack_t		emulthread_stack_alloc();

extern vm_offset_t		emul_stack_size;
extern vm_offset_t		emul_stack_mask;

#ifdef NX
/* For Paragon we want a 32k stack */
#define EMUL_INIT_STACK_SIZE	(4096*8)	/* 32 K */
#else
#define EMUL_INIT_STACK_SIZE	(4096*4)	/* 16 K */
#endif /* NX */

/*
 * Return base of the emulator stack of the given address on an emulator stack
 */
#ifdef  STACK_GROWTH_UP
#define EMUL_STACK_BASE(sp) \
	((emul_stack_t)((vm_offset_t)(sp) & emul_stack_mask))
#else
#define EMUL_STACK_BASE(sp) \
	(((emul_stack_t) \
		(((vm_offset_t)(sp) & emul_stack_mask) + emul_stack_size)) \
	  - 1)
#endif

#endif	_EMUL_STACK_H_
