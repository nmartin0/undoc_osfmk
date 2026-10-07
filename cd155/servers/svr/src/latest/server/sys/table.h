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
 
/* Revision 2.10  93/01/13  15:14:10  loverso (SQUID! Pre-merged!)
 * 	Add unix domain sockets.
 */
/*
 * @OSF_COPYRIGHT@
 */
/*
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * Copyright (c) 1986 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: table.h,v $
 * Revision 1.17  1995/02/01  21:38:11  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1994/11/18  20:41:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/06/17  15:04:28  paul
 * Changes in support of netstat. Fixed the TBL_RTREE code to talk to all the
 * network servers to get all the routes system wide, rather than just return
 * routes on the node the user happens to randomly get. Also, added a debug
 * version of the TBL_RTREE_NODE table() call which returns invisible MI routes.
 * Also, fixed the table() command to return tnc style <node>interface type
 * interface names i.e. <201>el0.
 *
 *  Reviewer: Bernie Keany
 *  Risk: M
 *  Benefit or PTS #: 7952 8059
 *  Testing: on Plymouth, with 1, 2, & 3 netservers, boot/non-boot configs
 *  Module(s): tnc/dvp_vpsops.c bsd/cmu_syscalls.c sys/table.h vsocket/if_mi.c
 * 	    vfs/vfs_syscalls.c
 *
 * Revision 1.14  1994/03/14  02:03:27  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.13  1994/02/07  18:22:01  stefan
 * Merged fix for PTS #7899 from R1_2 branch into main trunk.
 *
 * Revision 1.12.4.1  1994/02/07  14:35:03  stefan
 * In order for enabling load_leveld be able to map the node number of
 * ROOT_FS_NODE to the corresponding logical node number a new option
 * TBL_PHYSNODEINFO had to be added to the table() system call.
 * TBL_PHYSNODEINFO returns the list of physical nodes in a partition.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 7899
 *  Testing: developer testing
 *  Module(s): server/sys/table.h
 *             server/bsd/cmu_syscalls.c
 *             server/tnc/dvp_vpsops.c
 *
 * Revision 1.12  1993/08/18  00:06:36  hobbes
 * Added support for the hippi_showmap command .. TBL_HIPPI_ART.
 *
 * Revision 1.11  1993/07/14  18:29:06  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:38:11  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/05/18  03:20:07  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.9  1993/05/13  09:23:17  stefan
 * Changed #ifdef NX to #if defined(NX) || defined(SLL).
 *
 * Revision 1.8  1993/05/11  01:23:04  cfj
 * Add TBL_PROCINFO_PART.
 *
 * Revision 1.7  1993/05/11  00:17:00  stefan
 * Moved NX-specific table call TBL_IPD_MSG_INFO  to a non-reserved id range
 * (starting at 500).
 * Added TBL_FASTNODE to NX-specific table calls.
 *
 * Revision 1.6  1993/05/06  19:20:22  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.5  1993/04/03  03:08:00  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.4  1993/03/25  23:23:56  cfj
 * T9 Merge.
 *
 * Revision 1.3.6.1  1993/03/24  23:41:20  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 *     Revision 2.12  93/02/22  17:14:04  mjl
 *     Add TBL_UNPCB defs for Unix domain sockets (loverso).  Support tables that
 *     pack a subtable number into the index argument.  Add node fields to routing
 *     related tables to indicate node where route resides.
 * 
 * Revision 1.3  1992/12/11  03:00:09  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:01:15  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.2  1992/11/30  22:44:33  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:42:10  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.11  1992/10/16  20:42:58  andrews
 * Added IPD debug support.
 *
 * Revision 2.11  92/11/23  16:09:18  klh
 * 	Revision 2.9  92/11/11  22:36:57  loverso
 * 		Lost 1 field in merge.
 * 
 * 	Revision 2.8  92/11/11  19:02:48  loverso
 * 		Revamped network table entries. (loverso)
 * 
 * 		Revision 3.3  92/04/08  20:45:57  barbou
 * 		Added definition of TBL_VERSION.
 * 
 * Revision 2.10  92/10/06  12:13:12  roman
 * Revision 1.1.1.1  1993/05/03  17:43:03  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.14  93/05/04  09:05:14  bolsen
 * [Bug 240] The structure tbl_pginfo_10 has a member that uses a constant
 * 	PATH_MAX that is defined in <sys/syslimits.h>.
 * 
 * Revision 2.13  1993/04/29  14:01:52  klh
 * 	Revision 2.11  93/03/25  09:59:38  durriya
 * 		define TBL_PGINFO and struct tbl_pginfo_10 for paging
 * 		statistics                                        (durriya)
 *
 * 	Revision 2.10  93/01/13  15:14:10  loverso
 * 		Add unix domain sockets.
 *
 * Revision 2.12  93/02/22  17:14:04  mjl
 * Add TBL_UNPCB defs for Unix domain sockets (loverso).  Support tables that
 * pack a subtable number into the index argument.  Add node fields to routing
 * related tables to indicate node where route resides.
 * 
 * Revision 2.11  92/11/23  16:09:18  klh
 * 	Revision 2.9  92/11/11  22:36:57  loverso
 * 		Lost 1 field in merge.
 * 
 * 	Revision 2.8  92/11/11  19:02:48  loverso
 * 		Revamped network table entries. (loverso)
 * 
 * 		Revision 3.3  92/04/08  20:45:57  barbou
 * 		Added definition of TBL_VERSION.
 * 
 * Revision 2.10  92/10/06  12:13:12  roman
 * Fix RCS comments.
 * 
 * Revision 2.9  92/10/05  13:59:50  klh
 * 	Revision 2.6  92/08/13  19:19:30  rabii
 * 		Added table call numbers for:  ("kmem project")
 * 		(TBL_)NFSSTAT,MBSTAT,IPSTAT,TCB,TCPSTAT,UCB,IFB,RTHASH_RTNET
 * 		RTHASH_RTHOST,RTHASH_SIZE,RTREE_RNH,RTREE_NUMBER
 * 		[92/08/11            srl]
 * 
 * Revision 2.8  92/10/01  10:26:41  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.7  92/09/28  16:40:35  roman
 * Change fields for extracting vproc data to use new names from 
 * 	<sys/vproc.h>.
 * 
 * Revision 2.6  92/04/20  15:10:49  chrisp
 * Added fields to return Mach right reference counts for receive and send 
 *	rights held for vproc.
 * 
 * Revision 2.5  92/03/27  11:19:27  roman
 * For TNC, add TBL_NODEINFO table option.
 * 
 * Revision 2.4  92/02/17  14:43:50  klh
 * For OSF merge, update version # to match LCC #
 * 
 * Revision 2.3  92/02/11  22:07:12  pjg
 * 	Add tbl_vprocinfo structure for TBL_VPROCINFO option (chrisp@locus).
 * 
 * Revision 2.2  91/08/31  14:14:03  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:45:49  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:54:54  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:18  gm]
 * 
 * Revision 1.6  90/08/24  12:27:39  devrcs
 * 	Removed dup defines
 * 	[90/08/15  01:03:52  knight]
 * 
 * 	Removed dup defines.
 * 	[90/08/15  00:53:05  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Removed dup defines.
 * 	[90/08/15  00:53:05  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * Revision 1.5  90/08/09  13:29:09  devrcs
 * 	Removed TBL_*MARK.
 * 	[90/07/31  10:38:37  bet]
 * 
 * Revision 1.4  90/07/27  09:08:16  devrcs
 * 	Added definitions for sysV ipc support in the table routine.
 * 	[90/07/23  15:05:14  bet]
 * 
 * 	Add TBL_SYSINFO, TBL_DKINFO, and TBL_TTYINFO with appropriate structs.
 * 	[90/07/19  12:10:32  brezak]
 * 
 * 	Add the mach_factor to the load average table call
 * 	[90/07/12  08:15:36  sp]
 * 
 * Revision 1.3  90/07/17  11:42:39  devrcs
 * 	Return actual ptr to arguments in TBL_ARGUMENTS.
 * 	Add TBL_ENVIRONMENT. Added to TBL_PROCINFO.
 * 	[90/07/06  11:09:30  brezak]
 * 
 * Revision 1.2  90/01/02  20:16:05  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:37  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:23:52  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.6  89/03/09  22:08:18  rpd
 * 	More cleanup.
 * 
 * Revision 2.5  89/02/25  17:56:52  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.4  88/07/15  16:03:57  mja
 * 	Made tl_loadavg structure independent of conditionals;  moved
 * 	UMODE_* bit definitions here from <sys/syscalls.h> since they
 * 	are now handled through the table() call.
 * 
 * Revision 0.0  88/03/15            dbg
 * 	Added TBL_PROCINFO (MACH only).
 * 	[88/03/15            dbg]
 * 
 * Revision 0.0  87/04/15            mja
 * 	Added TBL_LOADAVG.
 * 	[ V5.1(F8) ]
 * 	[87/04/15            mja]
 * 
 * Revision 0.0  87/01/28            mja
 * 	Added TBL_MAXUPRC, TBL_AID, and TBL_MODES definitions.
 * 	[ V5.1(F1) ]
 * 	[87/01/28            mja]
 * 
 * Revision 0.0  86/11/07            dbg
 * 	Added TBL_ARGUMENTS, also for MACH only.
 * 	[86/11/07            dbg]
 * 
 * Revision 0.0  86/07/24            dbg
 * 	Added TBL_UAREA.  For now it only works under MACH virtual
 * 	memory.
 * 	[86/07/24            dbg]
 * 
 * Revision 0.0  86/07/24            mja
 * 	Added TBL_INCLUDE_VERSION and TBL_FSPARAM.
 * 	[86/07/24            mja]
 * 
 * Revision 0.0  85/08/30            mja
 * 	Added TBL_U_TTYD.
 * 	[V1(1)]
 * 	[85/08/30            mja]
 * 
 * Revision 0.0  83/03/30            mja
 * 	Created (V3.06h).
 * 	[83/03/30            mja]
 * 
 * $EndLog$
 * 
 * Revision 2.5  92/05/01  10:13:49  rabii
 * 	Added fields to return Mach right reference counts for receive 
 * 	and send rights held for vproc.(chrisp)
 * 
 * Revision 2.4  92/04/05  17:01:31  pjg
 * 	For TNC, add TBL_NODEINFO table option. (roman)
 * 
 * Revision 2.3  92/02/11  22:07:12  pjg
 * 	Add tbl_vprocinfo structure for TBL_VPROCINFO option (chrisp@locus).
 * 
 * Revision 2.2  91/08/31  14:14:03  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.2  91/07/31  15:45:49  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.7  90/10/07  14:54:54  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:46:18  gm]
 * 
 * Revision 1.6  90/08/24  12:27:39  devrcs
 * 	Removed dup defines
 * 	[90/08/15  01:03:52  knight]
 * 
 * 	Removed dup defines.
 * 	[90/08/15  00:53:05  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Removed dup defines.
 * 	[90/08/15  00:53:05  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added values for ihs_type
 * 	[90/08/15  00:00:24  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * 	Added table INTR defines.
 * 	[90/08/13  23:38:59  knight]
 * 
 * Revision 1.5  90/08/09  13:29:09  devrcs
 * 	Removed TBL_*MARK.
 * 	[90/07/31  10:38:37  bet]
 * 
 * Revision 1.4  90/07/27  09:08:16  devrcs
 * 	Added definitions for sysV ipc support in the table routine.
 * 	[90/07/23  15:05:14  bet]
 * 
 * 	Add TBL_SYSINFO, TBL_DKINFO, and TBL_TTYINFO with appropriate structs.
 * 	[90/07/19  12:10:32  brezak]
 * 
 * 	Add the mach_factor to the load average table call
 * 	[90/07/12  08:15:36  sp]
 * 
 * Revision 1.3  90/07/17  11:42:39  devrcs
 * 	Return actual ptr to arguments in TBL_ARGUMENTS.
 * 	Add TBL_ENVIRONMENT. Added to TBL_PROCINFO.
 * 	[90/07/06  11:09:30  brezak]
 * 
 * Revision 1.2  90/01/02  20:16:05  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.1  89/10/16  19:48:37  gm
 * 	Mach 2.5 and Encore 0.6 merge
 * 
 * Revision 6.1  89/07/26  14:23:52  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.6  89/03/09  22:08:18  rpd
 * 	More cleanup.
 * 
 * Revision 2.5  89/02/25  17:56:52  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.4  88/07/15  16:03:57  mja
 * 	Made tl_loadavg structure independent of conditionals;  moved
 * 	UMODE_* bit definitions here from <sys/syscalls.h> since they
 * 	are now handled through the table() call.
 * 
 * Revision 0.0  88/03/15            dbg
 * 	Added TBL_PROCINFO (MACH only).
 * 	[88/03/15            dbg]
 * 
 * Revision 0.0  87/04/15            mja
 * 	Added TBL_LOADAVG.
 * 	[ V5.1(F8) ]
 * 	[87/04/15            mja]
 * 
 * Revision 0.0  87/01/28            mja
 * 	Added TBL_MAXUPRC, TBL_AID, and TBL_MODES definitions.
 * 	[ V5.1(F1) ]
 * 	[87/01/28            mja]
 * 
 * Revision 0.0  86/11/07            dbg
 * 	Added TBL_ARGUMENTS, also for MACH only.
 * 	[86/11/07            dbg]
 * 
 * Revision 0.0  86/07/24            dbg
 * 	Added TBL_UAREA.  For now it only works under MACH virtual
 * 	memory.
 * 	[86/07/24            dbg]
 * 
 * Revision 0.0  86/07/24            mja
 * 	Added TBL_INCLUDE_VERSION and TBL_FSPARAM.
 * 	[86/07/24            mja]
 * 
 * Revision 0.0  85/08/30            mja
 * 	Added TBL_U_TTYD.
 * 	[V1(1)]
 * 	[85/08/30            mja]
 * 
 * Revision 0.0  83/03/30            mja
 * 	Created (V3.06h).
 * 	[83/03/30            mja]
 * 
 * $EndLog$
 */

#ifndef	_SYS_TABLE_H_
#define _SYS_TABLE_H_

#include <sys/syslimits.h>

#define TBL_TTYLOC		0	/* index by device number */
#define TBL_U_TTYD		1	/* index by process ID */
#define TBL_UAREA		2	/* index by process ID */
#define TBL_LOADAVG		3	/* (no index) */
#define TBL_INCLUDE_VERSION	4	/* (no index) */
#define TBL_FSPARAM		5	/* index by device number */
#define TBL_ARGUMENTS		6	/* index by process ID */
#define TBL_MAXUPRC		7	/* index by process ID */
#define TBL_AID			8	/* index by process ID */
#define TBL_MODES		9	/* index by process ID */
#define TBL_PROCINFO		10	/* index by proc table slot */
#define TBL_ENVIRONMENT		11	/* index by process ID */
#define TBL_SYSINFO		12	/* (no index) */
#define TBL_DKINFO		13	/* index by disk */
#define TBL_TTYINFO		14	/* (no index) */
#define TBL_MSGDS		15	/* index by array index */
#define TBL_SEMDS		16	/* index by array index */
#define TBL_SHMDS		17	/* index by array index */
#define TBL_MSGINFO		18	/* index by structure element */
#define TBL_SEMINFO		19	/* index by structure element */
#define TBL_SHMINFO		20	/* index by structure element */
#define TBL_INTR		21	/* (no index) */
#define TBL_VERSION		23	/* system version string (no index) */
/* 24-38 used by OSF1 1.1 */

#if defined(OSF1_ADFS) || defined(OSF1AD)
/*
 * OSF/1-AD table entries start at 100
 */
#define TBL_VPROCINFO		100	/* index by array index */
#ifdef	TNC
#define TBL_NODEINFO		101	/* index by array index */
#endif
#define TBL_PGINFO              102     /* paging info - 1.0.4 based - index 
                                         * node no.
                                         */
#ifdef TNC
#define TBL_PHYSNODEINFO	103	/* physical node list */
#define TBL_RTREE_NODE_DEBUG	104	/* Same as TBL_RTREE_NODE, with dbg */
#endif /* TNC */

/*
 * Interface data: indexed by list ordinal
 * lel==0 returns list length
 */
#define TBL_IF			110	/* Network interface */
#define TBL_MBSTAT		111	/* mbuf stats (no index) */

#define TBL_ARPPARAM		112	/* ARP parameter block (no index) */
/*
 * Whole ARP table: indexed by (live) entry ordinal
 * lel==0 returns number of active entries
 */
#define TBL_ARPTAB		113	/* ARP table entry */

#define TBL_UNPCB		114	/* AF_UNIX protocol control block */

#define TBL_RTSTAT		115	/* Routing statistics (no index) */

/* obsolete - hash tables not used in OSF/1 kernel */
#if 0
/*
 * Routing hash tables: indexed by table ordinal
 * lel==0 returns number of used entries
 */
#define TBL_RTHASH_NET		116	/* Routing - network hash table */
#define TBL_RTHASH_HOST		117	/* Routing - host hash table */
#endif

#define TBL_RTREE_HEAD		118	/* Routing - Radix node head */
#define TBL_RTREE_NODE		119	/* Routing - Radix node */

/*
 * Internet control blocks: indexed by list ordinal
 * lel==0 returns list length
 */
#define TBL_TCPCB		120	/* TCP control block */
#define TBL_UDPCB		121	/* UDP control block */

#define TBL_IPSTAT		122	/* IP stats (no index) */
#define TBL_ICMPSTAT		123	/* ICMP stats (no index) */
#define TBL_TCPSTAT		124	/* TCP stats (no index) */
#define TBL_UDPSTAT		125	/* UDP stats (no index) */
#define TBL_NFSSTAT		126	/* NFS stats (no index) */

#endif  /* OSF1_ADFS  || OSF1AD */

#if defined(NX) || defined(SLL)
#define TBL_NX_START		500	/* start of NX-specific TABLE ids */
#define TBL_IPD_MSG_INFO	(TBL_NX_START + 0)	/* index by process ID */
#define TBL_FASTNODE		(TBL_NX_START + 1)	/* fastest node for SLL (no index) */
#define TBL_PROCINFO_PART       (TBL_NX_START + 2)      /* TBL_PROCINFO
							 * restricted to a partition. */
#define TBL_HIPPI_ART		(TBL_NX_START + 3)	/* HIPPI address resolution */
#endif /* NX || SLL */

#define MSGINFO_MAX		0	/* max message size */
#define MSGINFO_MNB		1	/* max # bytes on queue */
#define MSGINFO_MNI		2	/* # of message queue identifiers */
#define MSGINFO_TQL		3	/* # of system message headers */

#define	SEMINFO_MNI		0	/* # of semaphore identifiers */
#define	SEMINFO_MSL		1	/* max # of semaphores per id */
#define	SEMINFO_OPM		2	/* max # of operations per semop call */
#define	SEMINFO_UME		3	/* max # of undo entries per process */
#define	SEMINFO_VMX		4	/* semaphore maximum value */
#define	SEMINFO_AEM		5	/* adjust on exit max value */

#define SHMINFO_MAX		0	/* max shared memory segment size */
#define SHMINFO_MIN		1	/* min shared memory segment size */
#define SHMINFO_MNI		2	/* num shared memory identifiers */
#define SHMINFO_SEG		3	/* max attached shared memory segments per process */

/*
 * Values for ihs_type -- don't really belong here, but...
 */
					/* interrupt types */
#define INTR_NOSPEC   0x0000
#define INTR_HARDCLK  0x0001
#define INTR_SOFTCLK  0x0002
#define INTR_DEVICE   0x0004
#define INTR_OTHER    0x0008
#define INTR_STRAY    0x8000
#define INTR_DISABLED 0x4000

#define INTR_CLOCK    (INTR_HARDCLK|INTR_SOFTCLK)
#define INTR_NOTCLOCK (~INTR_CLOCK)

/*
 *  Some tables use subtable id's embedded in the "index" argument.
 */
#ifdef	TNC
#define HAS_SUBTABLES(id)	((id) == TBL_RTREE_NODE || \
				 (id) == TBL_RTREE_NODE_DEBUG)
#else
#define HAS_SUBTABLES(id)	((id) == TBL_RTREE_NODE)
#endif

#define SUBTBL_SHIFT		24
#define SUBTBL_MASK		0x00ffffff
#define SUBTBL_IDX(stid, sidx)	((stid<<SUBTBL_SHIFT) | sidx)
#define SUBTBL_STID(idx)	((idx & ~SUBTBL_MASK) >> SUBTBL_SHIFT)
#define SUBTBL_SIDX(idx)	(idx & SUBTBL_MASK)

/*
 *  TBL_FSPARAM data layout
 */

struct tbl_fsparam
{
    long tf_used;		/* free fragments */
    long tf_iused;		/* free inodes */
    long tf_size;		/* total fragments */
    long tf_isize;		/* total inodes */
};


/*
 *  TBL_LOADAVG data layout
 */

struct tbl_loadavg
{
    union {
	    long   l[3];
	    double d[3];
    } tl_avenrun;
    int    tl_lscale;		/* 0 scale when floating point */
    long   tl_mach_factor[3];
};


/*
 *  TBL_INTR data layout
 */

struct tbl_intr
{
	long   	in_devintr;	/* Device interrupts (non-clock) */
	long   	in_context;	/* Context switches */
	long   	in_syscalls;	/* Syscalls */
	long   	in_forks;	/* Forks */
	long   	in_vforks;	/* Vforks */
};


/*
 *  TBL_MODES bit definitions
 */

#define UMODE_P_GID	01	/* - 4.2 parent GID on inode create */
#define UMODE_NOFOLLOW	02	/* - don't follow symbolic links */
#define UMODE_NONICE	04	/* - don't auto-nice long job */



/*
 *	TBL_PROCINFO data layout
 */
#define PI_COMLEN	19	/* length of command string */
struct tbl_procinfo
{
    int		pi_uid;		/* (effective) user ID */
    int		pi_pid;		/* proc ID */
    int		pi_ppid;	/* parent proc ID */
    int		pi_pgrp;	/* proc group ID */
    int		pi_ttyd;	/* controlling terminal number */
    int		pi_status;	/* process status: */
#define PI_EMPTY	0	    /* no process */
#define PI_ACTIVE	1	    /* active process */
#define PI_EXITING	2	    /* exiting */
#define PI_ZOMBIE	3	    /* zombie */
    int		pi_flag;	/* other random flags */
    char	pi_comm[PI_COMLEN+1];
				/* short command name */
    int		pi_ruid;        /* (real) user ID */
    int		pi_svuid;       /* saved (effective) user ID */
    int         pi_rgid;        /* (real) group ID */
    int         pi_svgid;       /* saved (effective) group ID */
    int		pi_session;	/* session ID */
    int         pi_tpgrp;       /* tty pgrp */
    int         pi_tsession;    /* tty session id */
    int         pi_jobc;        /* # procs qualifying pgrp for job control */
    int         pi_cursig;
    int         pi_sig;         /* signals pending */
    int         pi_sigmask;     /* current signal mask */
    int         pi_sigignore;   /* signals being ignored */
    int         pi_sigcatch;    /* signals being caught by user */
};

/*
 *	TBL_SYSINFO data layout
 */
struct tbl_sysinfo {
        long	si_user;		/* User time */
        long	si_nice;		/* Nice time */
        long	si_sys;			/* System time */
        long	si_idle;		/* Idle time */
        long    si_hz;
        long    si_phz;
	long 	si_boottime;		/* Boot time in seconds */
};

/*
 *	TBL_DKINFO data layout
 */
#define DI_NAMESZ	8
struct tbl_dkinfo {
        int	di_ndrive;
        int	di_busy;
        long	di_time;
        long	di_seek;
        long	di_xfer;
        long	di_wds;
        long	di_wpms;
        int	di_unit;
        char    di_name[DI_NAMESZ+1];
};
        
/*
 *	TBL_TTYINFO data layout
 */
struct tbl_ttyinfo {
        long	ti_nin;
        long	ti_nout;
        long	ti_cancc;
        long	ti_rawcc;
};

/* 
 * TBL_PGINFO data layout - 1.0.4 version. The 1.1 version is different
 *
 * struct tbl_pginfo_10
 * 
 * This structure is used to return paging information through the table
 * interface.  NOTE - this structure must be long word aligned size since we 
 * copy this array like a array of bytes using (sizeof(struct tbl_pginfo_10))
 */
struct tbl_pginfo_10
{
	unsigned long pg_free;		/* # of unallocated pages */
	unsigned long pg_npgs;		/* total # of pages */
	unsigned long pg_pagein_count;	/* # of page read requests */
	unsigned long pg_pagein_fail;	/* # of page read errors */
	unsigned long pg_pageout_count;	/* # of page write requests */
	unsigned long pg_pageout_fail;	/* # of page write errors */
        unsigned long pg_pageinit_count; /* # of page initialisations */
        unsigned long pg_pageinit_write; /* # of " " actually written */
        unsigned long pg_hipage;        /* highest page # allocated */
        int           pg_type;          /* type of paging file */
#define PG_KERN_DEFAULT    0            /* kernel default paging file */
#define PG_VNODE_FILE      1            /* vnode pager paging file */
#define PG_VNODE_RAWPART   2            /* vnode pager-paging to raw partition */
	char pg_name[PATH_MAX+1];	/* paging file pathname	*/
	int          pg_prefer;	        /* preferred paging file ? */
        int          pg_node;           /* for vnode pgr : the node that services
                                         *   this file/partition
                                         * for ker def pgr : node #
                                         */
};



/*
 *	TBL_VPROCINFO
 */
struct tbl_vprocinfo {
	int	vi_status;
#define VI_EMPTY	0	    /* no process */
#define VI_LOCAL	1	    /* local process */
#define VI_REMOTE	2	    /* remote process */
	int	vi_pid;
	long	vi_ref_cnt;
	int	vi_flag;
	int	vi_pproc_slot;
	int	vi_head_childl;
	int	vi_childl;
	int	vi_head_pgrpl;
	int	vi_pgrpl;
	int	vi_sessionl;
	int	vi_ppid;
	int	vi_pgid;
	int	vi_sid;
	int	vi_cttyd;
	long	vi_jobc;
	unsigned int	vi_mach_portname;
	unsigned int	vi_mach_receives;
	unsigned int	vi_mach_sends;
	int	vi_foster_ppid;
	int	vi_head_foster_childl;
	int	vi_foster_childl;
};

/*
 * These #ifdefs are here to prevent having to include network include files
 * for other uses of table.h.
 *
 * Because some of these structures are variable sized (depending upon
 * build options), their contents are exploded below.  There has to be a
 * better way.  It is bogus to directly return any kernel/server structures
 * that contain context-dependent information (i.e., pointers!), but
 * being able to do a structure copy is a win.
 *
 * Another problem deals with sockaddrs.  All told, it is a variable length
 * structure; "struct sockaddr" only represents the first 16 bytes.
 * ISO uses 32 byte sockaddrs; thus we use the SockAddr as our unit of
 * transfer.  Some code in netstat uses up to 512 bytes for a single
 * sockaddr, other code uses up to 128 bytes.  I cannot embed that much
 * in the structures, so I pick 32.
 */
#if	defined(TABLE_NETWORK) || \
	defined(TABLE_INPCB) || defined(TABLE_ARPTAB) || \
	defined(TABLE_IF) || defined(TABLE_ROUTE)
/* We can cheat and cast a "struct sockaddr" into one of these */
struct SockAddr {
	union {
		struct sockaddr sa;
		u_char pad[32];
	} sau;
};
#endif

#if	defined(TABLE_INPCB) || defined(TABLE_NETWORK)
struct tbl_inpcb {
	/* fields from struct socket */
	short	so_type;		/* generic type, see socket.h */
	short	so_options;		/* from socket call, see socket.h */
	short	so_linger;		/* time to linger while closing */
	short	so_state;		/* internal flags; SS_* socketvar.h */
	short	so_q0len;		/* partials on so_q0 */
	short	so_qlen;		/* number of connections on so_q */
	short	so_qlimit;		/* max number queued connections */
	short	so_dqlen;		/* listener dequeues in progress */
	short	so_timeo;		/* connection timeout */
	u_short	so_error;		/* error affecting connection */
	short	so_special;		/* special state flags SP_*, below */
	pid_t	so_pgid;		/* pgid for signals */
	u_long	so_oobmark;		/* chars to oob mark */
	struct	sockbuf so_rcv, so_snd;

	/* fields from struct inpcb */
	caddr_t		inp_next;	/* XXX: netstat -A (for debug) */
	caddr_t 	inp_ppcb;	/* XXX: netstat -A (for debug) */
	struct in_addr	inp_faddr, inp_laddr;
	u_short		inp_fport, inp_lport;

	/* protocol specific data */
	union {
		/* XXX should be exploded */
		struct tcpcb	iu_tcpcb;	/* TCP */
	} i_ps;
#define i_tcpcb i_ps.iu_tcpcb
};
#endif

#if	defined(TABLE_UNPCB) || defined(TABLE_NETWORK)
struct tbl_unpcb {
	/* fields from struct socket */
	short	so_type;		/* generic type, see socket.h */
	short	so_options;		/* from socket call, see socket.h */
	short	so_linger;		/* time to linger while closing */
	short	so_state;		/* internal flags; SS_* socketvar.h */
	short	so_q0len;		/* partials on so_q0 */
	short	so_qlen;		/* number of connections on so_q */
	short	so_qlimit;		/* max number queued connections */
	short	so_dqlen;		/* listener dequeues in progress */
	short	so_timeo;		/* connection timeout */
	u_short	so_error;		/* error affecting connection */
	short	so_special;		/* special state flags SP_*, below */
	pid_t	so_pgid;		/* pgid for signals */
	u_long	so_oobmark;		/* chars to oob mark */
	struct	sockbuf so_rcv, so_snd;

	/* pointer to socket included within  (XXX for netstat) */
	struct	unpcb unpcb;
	struct	sockaddr_un unp_addr;
};
#endif

#if	defined(TABLE_IF) || defined(TABLE_NETWORK)
#define MAX_IFADDRS     10			/* XXX */
struct tbl_if {
	char		if_name[IFNAMSIZ];	/* if_unit in if_net */
	int		ifaddrcount;
	struct {
		/* XXX netstat also defines this union */
		union ifaddrany {
			struct ifaddr		ifa;
			struct in_ifaddr	in;
#if NS
			struct ns_ifaddr	ns;
#endif
			u_char			ifa_any[128];	/* max */
		} ifa;
		struct SockAddr	sa;
	} ifaddrs[MAX_IFADDRS];

	/* XXX: last, since its size may vary; should be exploded */
	struct ifnet	if_net;
};
#endif

#if	defined(TABLE_ARPTAB) || defined(TABLE_NETWORK)
/* This could be settable */
struct tbl_arpparam {
	int	arptab_size;		/* Total size (bsiz & nb) */
	int	arptab_bsiz;		/* Bucket size */
	int	arptab_nb;		/* number of buckets */
	int	arpkillc;		/* kill completed entry in 20 mins. */
	int	arpkilli;		/* kill incomplete entry in 3 minutes */
	int	arprefresh;		/* time to refresh entry */
	int	arphold;		/* time to keep at_hold packet */
	int	arplost;		/* retry threshold for broadcast */
	int	arpdead;		/* retry threshold for backoff */
};
struct tbl_arptab {
	char			if_name[IFNAMSIZ];
	short			if_unit;
	u_char			if_addrlen;

	/* XXX should be exploded */
	struct arptab		t_arptab;
};
#endif

#if	defined(TABLE_ROUTE) || defined(TABLE_NETWORK)
#ifdef	TBL_RTHASH_HOST
struct tbl_rthash {
	struct SockAddr	key;		/* Object of search */
	struct SockAddr	gateway;	/* value */
	char			if_name[IFNAMSIZ];
	short			if_unit;

	/* XXX should be exploded */
	struct ortentry		t_rtentry;
};
#endif

/*
 * Radix tree head (indexed by ordinal)
 */
struct tbl_rnh {
	/* from struct radix_node_head */
	int	rnh_af;
	node_t rnh_node;
};

/*
 * Radix tree nodes (indexed by RNIDX(af, ord))
 */
#define TRNIDX(af, ord)	SUBTBL_IDX(af, ord)
#define TRNAF(idx)	SUBTBL_STID(idx)
#define TRNORD(idx)	SUBTBL_SIDX(idx)
struct tbl_rn {
	/* from struct radix_node */
	short	rn_b;			/* bit offset; -1-index(netmask) */
	char	rn_bmask;		/* node: mask for bit test*/
	u_char	rn_flags;		/* enumerated next */
	struct SockAddr	key;		/* Object of search */
	struct SockAddr	mask;		/* netmask, if present */

	/* from struct rtentry */
	struct SockAddr rt_gateway;	/* value */
	short	rt_flags;		/* up/down?, host/net */
	short	rt_refcnt;		/* # held references */
	u_long	rt_use;			/* raw # packets forwarded */
	struct	rt_metrics rt_rmx;	/* metrics used by rx'ing protocols */
	short	rt_idle;		/* easy to tell llayer still live */

	/* from struct ifnet */
	char	if_name[IFNAMSIZ];	/* interface to use */
	short	if_unit;

	node_t	rn_node;		/* node where this route entry lives */
};
#endif

#endif	_SYS_TABLE_H_
