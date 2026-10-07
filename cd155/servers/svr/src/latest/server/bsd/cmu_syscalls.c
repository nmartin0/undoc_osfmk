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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: cmu_syscalls.c,v $
 * Revision 1.38  1995/02/23  17:03:27  bolsen
 *  Reviewer(s): Bob Yasi, Michael Bergknoff, Surender Brahmaroutu
 *  Risk: Medium (many lines added)
 *  Benefit or PTS #: 10551
 *  Testing: Ran several commands which spend a large amount of time in the
 * 	  server and wrote a test program to print out the various SYS, USER
 * 	  and IDLE values.  I also used a modified version of the testcase
 * 	  provided with the bug report.
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  We found an undocumented MACH Kernel call that provides the necessary
 *  information for USER, SYS and IDLE time.  NICE is not available from
 *  the MACH Kernel.  The call is xxx_slot_info() and it copies the Kernel
 *  information into a slot_info structure.
 *
 * Revision 1.37  1995/02/01  21:25:21  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.36  1994/11/18  20:26:40  mtm
 * Copyright additions/changes
 *
 * Revision 1.35  1994/06/17  23:32:13  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.34  1994/06/17  15:02:45  paul
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
 * Revision 1.33  1994/04/19  13:12:23  stefan
 * Merged Revision 1.27.2.3 from R1_2 branch into main trunk.
 * Fix for PTS #9000.
 *
 * Revision 1.32  1994/03/14  02:00:04  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.31  1994/03/11  01:52:09  jlitvin
 * Fix minor merge problems.
 *
 * Revision 1.30  1994/03/09  16:23:29  nandy
 * Merged from R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.29  1994/02/07  18:34:33  stefan
 * Merged fix for PTS #7899 from R1_2 branch into main trunk.
 *
 * Revision 1.28  1994/01/13  17:54:43  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	bsd/uipc_usrreq.c, bsd/uipc_syscalls.c, bsd/tty_subr.c
 * 	bsd/tty_compat.c, bsd/svipc_shm.c, bsd/svipc_sem.c
 * 	bsd/subr_select.c, bsd/mach_signal.c, bsd/mach_core.c
 * 	bsd/mach_clock.c, bsd/ldr_exec.c, bsd/kern_utctime.c
 * 	bsd/kern_time.c, bsd/kern_sig.c, bsd/kern_resource.c
 * 	bsd/kern_prot.c, bsd/kern_proc.c, bsd/kern_mman.c
 * 	bsd/kern_fork.c, bsd/kern_exit.c, bsd/kern_exec.c
 * 	bsd/kern_descrip.c, bsd/kern_acct.c, bsd/init_main.c
 * 	bsd/cmu_syscalls.c
 *
 * Revision 1.27.2.3  1994/04/15  17:05:11  stefan
 * The static load leveling code has been modified so that sll_lock is
 * released before nx_map_node_proc() is called. This prevents a deadlock
 * in the static load leveling code path.
 *
 *  Reviewer: Charlie Johnson
 *  Risk: Low to Medium
 *  Benefit or PTS #: PTS #9000
 *  Testing: developer testing
 *  Module(s): server/sll/sll.c
 * 	    server/bsd/cmu_syscalls.c
 *
 * Revision 1.27.2.2  1994/03/09  16:19:22  nandy
 * process in the middle of a fork() might have an entry in the
 * proc table but the vproc is not set. Ignore such processes in pps_table()
 *
 *  Reviewer: cfj
 *  Risk: LOW
 *  Benefit or PTS #: 8421
 *  Testing: PTS test
 *  Module(s): server/bsd/cmu_syscalls.c
 *
 * Revision 1.27.2.1  1994/02/07  14:37:06  stefan
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
 * Revision 1.27  1993/11/03  18:56:36  yazz
 * Use local variable for vproc pointer within case TBL_NODEINFO.
 *
 * Revision 1.26  1993/09/09  16:06:11  cfj
 * Part of the fix for PTS bug #6449.  Remove the call to nx_sameuser()
 * in pps_table() and instead use the sameuser() function in the base code.
 *
 * Revision 1.25  1993/09/01  01:34:10  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.24  1993/08/19  16:39:30  cfj
 * Put the implementationof TBL_HIPPI_ART under RFS_1323 so LITE server links.
 *
 * Revision 1.23  1993/08/17  23:57:26  hobbes
 * Added the handling for hippi_showmap (cmds/libs) .. TBL_HIPPI_ART.
 *
 * Revision 1.22  1993/08/11  18:17:59  stefan
 * Modified table(TBL_NODEINFO,...) to only return valid nodes (fix for bug
 * # 5447).
 *
 * Revision 1.21  1993/07/29  21:51:04  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network server slowdown.
 *
 * Revision 1.20  1993/07/19  22:58:27  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.19  1993/07/14  17:46:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.8  1993/07/01  18:44:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.18  1993/06/23  18:26:22  stefan
 * Added check for valid node number in fast_node.
 *
 * Revision 1.17  1993/06/09  00:08:22  cfj
 * Change occurrences of #include <i860ipsc/mcmsg> to #include <i860paragon/mcmsg>
 *
 * Revision 1.16  1993/06/01  15:58:20  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.15  1993/05/20  16:02:41  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.14  1993/05/18  18:55:17  cfj
 * Add appropriate ifdefs so that the server will build when turning off NFS.
 *
 * Revision 1.13  1993/05/17  19:05:57  cfj
 * 05-06-93 MI driver drop from Locus.
 *
 * Revision 1.12  1993/05/13  09:14:33  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.11  1993/05/07  19:58:58  cfj
 * Fix for missing #endif.
 *
 * Revision 1.10  1993/05/06  19:01:34  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.9  1993/04/03  03:03:29  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.8  1993/03/25  21:29:21  cfj
 * T9 Merge.
 *
 * Revision 1.7.4.1  1993/03/24  23:38:51  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 * Revision 1.1.2.4.2.2  1993/02/16  20:02:13  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.7  1993/01/22  20:49:20  cfj
 * Delete declaration for vp from within #ifdef NX.
 *
 * Revision 1.6  1993/01/21  19:22:08  nandy
 * 1/20/93 drop from LOCUS.1/20/93 drop from LOCUS.
 *   Resolved the scope conflict of the "size" variable in pps_tablefor the
 *   TBL_ARPTAB comand.
 *
 *   The TBL_VPROCINFO option now dependent upon VPROC_DEBUG and the vproc table
 *   is now a linked list rather than a contiguous table.
 *
 *   Fix typo in TBL_UDPSTAT clause. [chrisp for roman]
 *
 * Revision 1.1.2.4.2.1  1992/12/16  05:58:01  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/11  05:21:31  cfj
 * Fix double use of out: label in pps_table().
 *
 * Revision 1.4  1992/12/11  02:54:00  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:14:57  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.4  1992/11/10  20:02:08  cfj
 * Final T6 Checkin.
 *
 * Revision 1.1.2.3  1992/11/09  20:19:54  cfj
 * Change proc pointer in some NX specific code from cp to p.
 *
 * Revision 1.1.2.2  1992/11/06  18:21:36  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:05:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 1.23  1992/10/17  18:52:30  cfj
 * Add a couple of casts to remove a warning which appeared to be
 * blowing up the compiler.
 *
 * Revision 1.22  1992/10/17  18:25:04  cfj
 * IPD & Allocator Support.
 *
 * Revision 1.21  1992/10/17  18:09:49  cfj
 * Fixed a typo in TBL_IPD_MSG_INFO.
 *
 * Revision 1.20  1992/10/16  20:45:44  andrews
 * Added IPD debug support.
 *
 * Revision 1.34  93/08/19  12:54:39  mjl
 * For TBL_RTREE_NODE, adjusting the number of routes by the MI invisible route
 * count should be done on a per-address-family basis.  Partial fix only.
 * 
 * Revision 1.33  93/07/29  11:33:22  mjl
 * [Bug #0327] Fixed IfnetVisible() macro to check correct flag.
 * 
 * Revision 1.32  93/07/13  15:53:56  slively
 * Made sure that the table call to NFSSTAT will not return a bogus
 * value if being called on a LITE server, but will return a zeroed
 * out structure instead, as this call is not allowed on a LITE server.
 *      Revision 1.22  93/06/29  16:17:46  rabii
 *      Put NFS conditional for NFS build (rabii)
 *
 * Revision 1.31  93/06/29  09:33:54  nina
 * Fix link errors in +tnc build
 * 
 * Revision 1.30  93/06/02  09:47:41  yazz
 * For Sys V IPC under TNC do not reject entire table() request just because
 * the size is incorrect; fill the structure only partially, as defined in
 * docs.
 * 
 * Revision 1.29  93/06/01  12:56:02  nina
 * Rework the code that prevents "invisible" ifnet structures
 * from being returned by TBL_IF.
 * 
 * Revision 1.28  93/05/11  14:24:56  mjl
 * Fix operator precedence problem in RtVisible() macro.  Also, pass ismask
 * boolean to find_radix_node() so RtVisible() can know that a tree entry is
 * a netmask, and is therefore visible (rt_flags is bogus for mask entries).
 * 
 * Revision 1.27  93/05/07  14:59:20  nina
 * Made modifications to table() code and find_radix_node
 * to support the new MI driver.
 * 
 * Revision 1.26  1993/04/29  13:49:32  klh
 * 	Revision 1.21  93/03/25  09:52:26  durriya
 * 		add support for TBL_PGINFO to get paging info statistics.
 *
 * Revision 1.25  93/02/22  17:06:32  mjl
 * Return the node where route resides with routing related table() calls.
 * 
 * Revision 1.24  93/01/13  17:59:04  bhk
 * Resolved the scope conflict of the "size" variable in pps_table for the
 * TBL_ARPTAB comand. 
 * 
 * Revision 1.23  92/12/29  12:38:47  chrisp
 * The TBL_VPROCINFO option now dependent upon VPROC_DEBUG and the vproc table
 * 	is now a linked list rather than a contiguous table.
 * 
 * Revision 1.22  92/11/24  14:17:37  chrisp
 * Fix typo in TBL_UDPSTAT clause. [chrisp for roman]
 * 
 * Revision 1.21  92/11/23  15:51:16  klh
 * 	Revision 1.17  92/11/18  14:48:17  loverso
 * 		Fix logic error in table size of TBL_IF.
 * 
 * 	Revision 1.16  92/11/12  22:21:44  loverso
 * 		Allow access to proc information of processes with same uid.
 * 
 * 	Revision 1.15  92/11/11  19:02:37  loverso
 * 		Revamped network table code.  Short circuit many network table size
 * 		calculations.  Take hint from SNMP: do not return error if get requested
 * 		item off end of table (allows simpler table walking).
 * 		(loverso)
 * 
 * 		Revision 3.8  92/04/08  20:44:29  barbou
 * 		Implemented TBL_VERSION (from OSF/1.1).
 * 
 * Revision 1.20  92/10/28  14:52:51  roman
 * Divide the table() system call into a virtual process system operation
 * 	and a physical process system operation (minimizing impact
 * 	on the existing code).
 * Change table() system call to return the number of elements in the table
 * 	if lel == 0 and nel >= MAX_SHORT (rather than lel == 0 and
 * 	nel == MAX_SHORT). Matches the man pages better and prepares
 * 	system for some rather large tables.
 * Add shortcuts in the code for common cases of the size of the proc table
 * 	and the size of the vproc table.
 * Fix minor typo in TBL_NFSSTAT.
 * Fix procedure prototypes for clean i860 compilations.
 * 
 * Revision 1.19  92/10/06  12:08:02  roman
 * Fix RCS comments.
 * 
 * Revision 1.18  92/10/05  15:21:37  klh
 * 	Revision 1.13  92/09/29  16:47:23  rabii
 * 		   Added more table calls:
 * 		   RTHASH_RTNET,RTHASH_RTHOST,RTHASH_SIZE,RTREE_RNH,
 *		   RTREE_NUMBER
 * 		[92/09/11            srl]
 * 
 * 	Revision 1.12  92/08/13  19:16:58  rabii
 * 		Added support for more table calls ("kmem" project) :
 * 		NFSSTAT,MBSTAT,IPSTAT,TCPSTAT,TCB,UCB,IFB
 * 		[92/08/11            srl]
 * 
 * Revision 1.17  92/10/01  10:18:19  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 1.16  92/09/28  16:26:17  roman
 * Use new field names ofr pvproc structure.
 * Incorporate new field for foster parent process into pvproc.
 * Change type of node_array to node_t.
 * 
 * Revision 1.15  92/07/07  15:06:04  roman
 * Minor format corrections to exactly match OSF source code.
 * 
 * Revision 1.14  92/06/05  15:27:54  roman
 * Fix bug in setting up controlling TTY device field in VPROCINFO
 * 	table() operation.
 * 
 * Revision 1.13  92/06/05  13:41:50  klh
 * 	Revision 1.10  92/05/24  14:11:49  pjg
 * 		92/03/07  18:00:09  sp
 * 		Implement table(TBL_INTR) (Bug #90)
 * 		[92/05/18            srl]
 * 
 * Revision 1.12  92/05/01  15:05:11  roman
 * Drop support of the pi_jobc field for the table command. Unused field that
 * 	causes problems for TNC clusters.
 * 
 * Revision 1.11  92/04/20  14:46:48  chrisp
 * Add support in the TBL_VPROCINFO option to table() to return Mach port right
 * 	reference counts for vproc ports.
 * 
 * Revision 1.10  92/03/27  10:53:43  roman
 * Add extra parameter to VPOP_CTTY_GETATTR().
 * For TNC only, add support for TBL_NODEINFO option.
 * 
 * Revision 1.9  92/03/24  10:18:43  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 1.7  92/03/09  14:01:09  durriya
 * 	Revision 3.6  92/01/30  16:04:49  sp
 * 	Remove MACH_NO_KERNEL conditionals
 * 
 * Revision 1.6  92/02/11  22:09:56  pjg
 * 	 TBL_VPROCINFO option to table() (chrisp@locus).
 * 
 * Revision 1.5  91/12/17  17:01:26  roy
 * 	91/12/02  16:58:50  sp
 * 	remove the ifdefs which prevented the TBL calls for System V ipc
 * 	from working.
 * 
 * 	91/11/26  15:32:15  sp
 * 	Upgrade to 1.0.3
 * 
 * Revision 1.4  91/11/22  14:54:03  rabii
 * 	Locus Merge 
 * 	Vproc operation VPOP_GET_CTTY replaced by VPOP_CTTY_GETATTR. (chrisp)
 * 
 * Revision 1.3  91/10/04  14:42:43  chrisp
 * Get rid of extraneous RCS $Log.
 * 
 * Revision 1.2  91/09/16  15:29:28  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	Make certain items temporarily inaccessible, change to use new
 * 	proc fields now that vprocs are around.
 * 
 * Revision 1.1  91/08/31  13:21:09  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/09  11:53:22  barbou
 * Updated to OSF/1.0.2.
 * 
 * Condensed relevant OSF/1 server history, reverse chronology:
 * 91/04/12 Original from BSD server.                      condict@gr.osf.org
 * 91/06/07 Implemented what ps(1) needs.                  barbou@gr.osf.org
 * 91/07/29 Implemented TBL_LOADAVG and TBL_SYSINFO.       barbou@gr.osf.org
 * 
 * Revision 1.15.7.2  91/08/22  10:24:45  garyf
 *	merged in the following fix from 1.1:
 *	Fixed calls to lookup pid 0 in table().
 *	Fixes bug #2661, resulting from calling
 *	pfind() with pid 0.
 *	[91/08/22  10:24:03  garyf]
 *
 * Revision 1.15.5.2  91/06/06  15:48:39  jeffc
 * 	Merged in the following fixes from 1.1:
 * 	- disk iostat values returned for 386
 * 	- TBL_PROCINFO and TBL_UAREA are more careful about derefencing
 * 	  NULL pointers.
 * 	- do not dynamically allocate the fake u-area in TBL_UAREA, the
 * 	  temp space is now all union-ed onto the stack.
 * 	- ridiculous security #ifdefs for non-existent features removed.
 * 	[91/06/06  09:20:24  jeffc]
 * 
 * Revision 1.15  90/10/07  13:16:24  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:50:24  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:54:00  gm]
 * 
 * 	There were a couple of instances still checking suser under SEC_PRIV,
 * 	not SEC_BASE as they should.
 * 	[90/09/21  10:03:26  seiden]
 * 
 * 	Added TBL_INTR switch
 * 	[90/08/13  22:57:46  knight]
 * 
 * Revision 1.14  90/09/13  11:41:43  devrcs
 * 	fix recent regressions in table()
 * 	[90/09/02  01:01:16  hosking]
 * 
 * Revision 1.13  90/08/24  11:15:05  devrcs
 * 	Got rid of set/getmodes, set/getaid system calls.
 * 	Changed table() to use new syscall interface.
 * 	[90/08/08  14:46:12  gmf]
 * 
 * 	The only function left here is table().
 * 
 * Revision 1.12  90/08/09  13:13:32  devrcs
 * 	Remove dead CMUCS code.
 * 	[90/08/03  17:35:54  tmt]
 * 
 * 	No longer support TBL_[MSG,SEM,SHM]MARK.
 * 	[90/07/31  10:40:26  bet]
 * 
 * Revision 1.11  90/07/27  08:43:23  devrcs
 * 	Added support for sysV ipcs kernel information:
 * 	[msg,sem,shm] data structure, info structure values, and mark values.
 * 	[90/07/23  13:09:42  bet]
 * 
 * 	Add TBL_SYSINFO, TBL_DKINFO, TBL_TTYINFO.
 * 	[90/07/19  12:05:22  brezak]
 * 
 * 	Check that tty has a session in TBL_PROCINFO.
 * 	[90/07/14  12:44:38  brezak]
 * 
 * 	Add the mach_factor to the load average table call.
 * 	[90/07/12  08:14:51  sp]
 * 
 * 	Fix null deref in TBL_PROCINFO.
 * 	[90/07/11  13:20:18  brezak]
 * 
 * 	Add TBL_SYSINFO, TBL_DKINFO, TBL_TTYINFO.
 * 	[90/07/19  12:05:22  brezak]
 * 
 * 	Check that tty has a session in TBL_PROCINFO.
 * 	[90/07/14  12:44:38  brezak]
 * 
 * 	Add the mach_factor to the load average table call.
 * 	[90/07/12  08:14:51  sp]
 * 
 * 	Fix null deref in TBL_PROCINFO.
 * 	[90/07/11  13:20:18  brezak]
 * 
 * Revision 1.10  90/07/17  11:18:12  devrcs
 * 	Make priv checks under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:40:25  seiden]
 * 
 * 	Have TBL_ARGUMENTS use u_argp to locate arguments. Add TBL_ENVIRONMENT.
 * 	Add more info to TBL_PROCINFO request.
 * 	[90/07/06  10:59:04  brezak]
 * 
 * Revision 1.10  90/07/17  11:18:12  devrcs
 * 	Make priv checks under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:40:25  seiden]
 * 
 * 	Have TBL_ARGUMENTS use u_argp to locate arguments. Add TBL_ENVIRONMENT.
 * 	Add more info to TBL_PROCINFO request.
 * 	[90/07/06  10:59:04  brezak]
 * 
 * Revision 1.9  90/06/22  20:05:17  devrcs
 * 	Prevent NULL dereference in table_ttyloc().
 * 	[90/06/15  07:41:41  tmt]
 * 
 * 	nags merge
 * 
 * 	Condensed relevant ancient history (reverse chronology):
 * 	SecureWare changes.				seiden@osf.org
 * 	Parallelized for OSF/1				nags@encore.com
 * 	TBL_UAREA: Don't hold task lock in fake_u call	ers@osf.org
 * 	Fixed table call, and tty fixes			ers@osf.org
 * 	Setaid deprecated; returns 0 for su.		gmf@osf.org
 * 	Change to use 4.4BSD pgrp structure.		coren@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Changes for 4.4BSD vnodes.			ers@osf.org
 * 	MACH X115 Update.				gm@osf.org
 * 	Merged Mach 2.5 with Encore parallelization	alan@encore.com
 * 	Use new vm_map_copy technology.			mwyoung@cmu.edu
 * 	One exception frame at top of stack for mips	af@cmu.edu
 * 	Moved xutimes to ufs_syscalls.c 		gm0w@cmu.edu
 * 	Declarations of table_ttyloc(), table_fsparam()	rvb@cmu.edu
 * 	Be alert for kmem_alloc_wait returning zero	mwyoung@cmu.edu
 * 	Correct use of vm_map_copy in table		mwyoung@cmu.edu
 * 	Corrected conditionals, include files		mwyoung@cmu.edu
 * 	Fix tl_avenrun reference for new union type.	mja@cmu.edu
 * 	Added a mount table lock.			alan@encore.com
 * 	Added TBL_PROCINFO to avoid reading proc table	dbg@encore.com
 * 	Replaced sizeof(sigcode) with SIGCODE_SIZE.	sanzi@cmu.edu
 * 	Use kernel_pageable_map in table()		dbg@cmu.edu
 * 	Use reference count on task or map		dbg@cmu.edu
 * 	Split ttyloc and fsparam parts of table call 	avie@cmu.edu
 * 	Fixed TBL_UAREA not to allocate uarea on stack	sanzi@cmu.edu
 * 	Add TBL_LOADAVG for load average, scale factor	mja@cmu.edu
 * 	Restored xutimes() call				mja@cmu.edu
 * 	Use new fake_u call for MACH.			avie@cmu.edu
 * 	Removed dead SPIN_LOCK code, simplified table() avie@cmu.edu
 * 	Updated for latest u-area hacks.		avie@cmu.edu
 * 	Revised table() call: to permit set and get 	mja@cmu.edu
 * 	Fixed TBL_UAREA to copy wired-down U areas	dbg@cmu.edu
 * 	Fixed TBL_UAREA call to give back the U-area 	dbg@cmu.edu
 * 	Fixed test condition in the "copy u area" loop	mwyoung@cmu.edu
 * 	Merge with official 4.3 release.		dbg@cmu.edu
 * 	vm_deallocate doesn't work on kernel maps	dbg@cmu.edu
 * 	Added TBL_UAREA to table() to get U area 	dbg@cmu.edu
 * 	Added TBL_INCLUDE_VERSION and TBL_FSPARAM.	mja@cmu.edu
 * 	Change privilege restrictions in setmodes() 	mja@cmu.edu
 * 	Added temporary xutimes()call			mja@cmu.edu
 * 	Added U_TTYD table.				mja@cmu.edu
 * 	Added table() system call.  			mja@cmu.edu
 * 	[90/06/12  14:11:31  gmf]
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

#include <sys/secdefines.h>
#if	SEC_BASE
#include <sys/security.h>
#endif

#include <cpus.h>
#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/buf.h>
#include <sys/vnode.h>
#include <sys/file.h>
#include <sys/proc.h>
#ifdef	TNC
#include <tnc/dpvproc.h>
#else	/* ! TNC */
#include <vproc/bpvproc.h>
#endif
#include <sys/tty.h>
#include <sys/conf.h>
#include <sys/ioctl.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/dk.h>
#include <sys/syscall.h>
#include <uxkern/proc_to_task.h>

#ifdef	OSF1_SERVER
#include <nfs.h>
#include <uxkern/import_mach.h>
#else	/* OSF1_SERVER */
#include <vm/vm_user.h>
#include <vm/vm_map.h>
#include <vm/vm_kern.h>
#include <mach/vm_param.h>
#include <kern/task.h>
#include <sys/vmmac.h>		/* only way to find size of u./kernel stack */
#endif	/* OSF1_SERVER */
#include <machine/vmparam.h>	/* only way to find the user stack (argblock) */

#include <sys/version.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/shm.h>

/* include all network structures; used in table.h */
#define	TABLE_NETWORK	
#include <net/net_globals.h>
#include <sys/domain.h>

#include <sys/mbuf.h>

#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/unpcb.h>
#include <sys/un.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_var.h>
#include <netinet/in_systm.h>
#include <netinet/if_ether.h>
#if	NETSYNC_LOCK
/* This is in if_ether.c */
extern lock_data_t	global_arp_lock;
#define ARP_LOCK()	{ NETSPL(s,net); lock_write(&global_arp_lock); }
#define ARP_UNLOCK()	{ lock_done(&global_arp_lock); NETSPLX(s); }
#else
#define ARP_LOCK()	NETSPL(s,net)
#define ARP_UNLOCK()	NETSPLX(s)
#endif


#include <netinet/ip.h>
#include <netinet/ip_var.h>

#include <netinet/in_pcb.h>

#include <netinet/ip_icmp.h>
#include <netinet/icmp_var.h>

#include <netinet/tcp.h>
#include <netinet/tcpip.h>
#include <netinet/tcp_fsm.h>
#include <netinet/tcp_seq.h>
#include <netinet/tcp_timer.h>
#include <netinet/tcp_var.h>

#include <netinet/udp.h>
#include <netinet/udp_var.h>

#include <nfs/nfsv2.h>
#include <nfs/nfs.h>

#if defined(TNC)
#include "lnsvr/netname_defs.h"
#include "mi.h"
#include "vsocket/mi_var.h"
#endif

struct radix_node *find_radix_node(
	struct radix_node	*rn,
	int			*rn_leaf_id,
	int			ismask,
	int			show_invisible);
int radix_node_count(
	struct radix_node *rn);

#include <sys/table.h>

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>

#if __STDC__ == 1
extern int nx_get_info(struct pvproc *pvp,
                       APPLINFO_T *applinfo,
                       LP_MAP_T *nodelist,
                       int      *nodelistcnt);
#else
extern int nx_get_info();
#endif /* __STDC__ */

#include <net/if_llc.h>
#include <net/if_hippi.h>

#endif /* NX */

#ifdef SLL
#include <sll/sll_types.h>
#endif /* SLL */

/*
 * Chose an arbitrary size since there is no way of knowing what the
 * MACH Kernel's NCPUS value is in the server.  Currently, MACH's NCPUS
 * value is 3 and the Server's NCPUS value is 2.
 */
#define MAXSYSINFOSZ	6
  
/*
 * For TNC, the network interface name can have fancy <node> constructs in
 * it, so we use a special routine to deal with them, otherwise we just bcopy.
 */
#if defined(TNC)
void ifnamecopy(char *ifname,
		register char *dest,
		int device_node);
#else
#define ifnamecopy(source, dest, node) \
	bcopy(source, dest, IFNAMSIZ);
#endif
  
/*
 * High level interface for the table() system call. The system call invokes
 * the virtual process system operation so that TNC can correctly deal
 * with the table() call being distributed.
 */
table(cp, args, retval)
	struct proc *cp;
	void *args;
	int *retval;
{
	register struct args {
		int id;
		int index;
		caddr_t addr;
		int nel;	/* >0 ==> get, <0 ==> set */
		u_int lel;
	} *uap = (struct args *) args;
	return(VPSOP_TABLE(uap->id, 
			   uap->index, 
			   uap->addr, 
			   uap->nel, 
			   uap->lel,
			   retval));
}

/*
 *  table - get/set element(s) from system table
 *
 *  This call is intended as a general purpose mechanism for retrieving or
 *  updating individual or sequential elements of various system tables and
 *  data structures.
 *
 *  One potential future use might be to make most of the standard system
 *  tables available via this mechanism so as to permit non-privileged programs
 *  to access these common SYSTAT types of data.
 *
 *  Parameters:
 *
 *  id		= an identifer indicating the table in question
 *  index	= an index into this table specifying the starting
 *		  position at which to begin the data copy
 *  addr	= address in user space to receive/supply the data
 *  nel		= number of table elements to retrieve/update
 *  lel		= expected size of a single element of the table.  If this
 *		  is smaller than the actual size, extra data will be
 *		  truncated from the end.  If it is larger, holes will be
 *		  left between elements copied to/from the user address space.
 *
 *		  The intent of separately distinguishing these final two
 *		  arguments is to insulate user programs as much as possible
 *		  from the common change in the size of system data structures
 *		  when a new field is added.  This works so long as new fields
 *		  are added only to the end, none are removed, and all fields
 *		  remain a fixed size.
 *
 *  Returns:
 *
 *  val1	= number of elements retrieved/updated (this may be fewer than
 *		  requested if more elements are requested than exist in
 *		  the table from the specified index).
 *
 *  Note:
 *
 *  A call with lel == 0 and nel == MAXSHORT can be used to determine the
 *  length of a table (in elements) before actually requesting any of the
 *  data.
 */

#define MAXLEL	(sizeof(long))	/* maximum element length (for set) */

pps_table(args, retval)
	void *args;
	int *retval;
{
	register struct args {
		int id;
		int index;
		caddr_t addr;
		int nel;	/* >0 ==> get, <0 ==> set */
		u_int lel;
	} *uap = (struct args *) args;
	struct proc *p;
	struct vproc *vp = NULL;
	caddr_t data;
	unsigned size;
	int error = 0;
	int set;
	vm_offset_t	arg_addr;
	vm_size_t	arg_size;
	vm_offset_t	copy_start;
	vm_offset_t	copy_end;
	vm_size_t	copy_size;
#ifndef	OSF1_SERVER
	vm_map_copy_t	copy_result;
	vm_map_t	proc_map;
#endif	OSF1_SERVER
	vm_offset_t	dealloc_start;	/* area to remove from kernel map */
	vm_size_t	dealloc_end;
#ifdef NX
        APPLINFO_T      applinfo;
        LP_MAP_T        nodelist = (LP_MAP_T)NULL;
        int             nodelistcnt;
        int             has_priv;
        uid_t           uid;
        boolean_t       in_partition;
#endif /* NX */
	int		ncpu;
	unsigned	count;
	long		multi;
	long		ticks;
	struct machine_slot		si[MAXSYSINFOSZ];
	struct task_thread_times_info	svr_live_info;
	struct task_basic_info		svr_termd_info;

	/*
	 *  Verify that any set request is appropriate.
	 */
	set = 0;
	if (uap->nel < 0) {
		switch (uap->id) {
		case TBL_MAXUPRC:
#if	SEC_BASE
			if (!privileged(SEC_LIMIT, EPERM))
				return (EPERM);
#else
			if (error = suser(u.u_cred, &u.u_acflag))
				return (error);
#endif
		        break;
#ifdef NX
		case TBL_IPD_MSG_INFO:
			break;
#endif /* NX */
#ifdef SLL
		case TBL_FASTNODE: {
#if SEC_BASE
			/*
			 * Must have the SEC_SYSATTR privilege.
			 */
			if ( !privileged(SEC_SYSATTR, EPERM) ) {
				SLL_DEBUGF("table(FASTNODE) no SEC_SYSATTR permission\n");
				return (EPERM);
			}
#else /* SEC_BASE */
			/*
			 * Must be super user
			 */
			if ( error = suser(u.u_cred, &u.u_acflag) ) {
				SLL_DEBUGF("table(FASTNODE) not super user\n");
				return (error);
			}
#endif /* SEC_BASE */
			break;
		}
#endif /* SLL */
		default:
			return (EINVAL);
		}
		set++;
		uap->nel = -(uap->nel);
	}

	*retval = 0;

	/*
	 *  Perform some common case code outside the main loop:
	 *  Verify common case of only current process index.
	 *	Perform common case of obtaining several table sizes.
	 */
	switch (uap->id) {
	case TBL_U_TTYD:
	case TBL_MAXUPRC:
		if ((uap->index != u.u_procp->p_pid && uap->index != 0) ||
				(uap->nel != 1))
			goto bad;
		break;
	case TBL_PROCINFO:
		if (uap->lel == 0 && uap->nel >= SHRT_MAX) {
			if (error = suser(u.u_cred, &u.u_acflag))
				return(error);
			*retval = nproc;
			return(ESUCCESS);
		}
		break;
#ifdef NX
#ifdef TNC
	case TBL_NODEINFO:
	case TBL_PHYSNODEINFO:
	{
		struct vproc *uvp = LOCATE_VPROC_PID(u.uu_procp->p_pid);
		if (uvp == 0)
			panic("pps_table: cannot find myself");
		if ((in_partition = nx_in_partition(uvp))) {
			(void) pproc_get_attr(0,0,0,0,0,&has_priv,&uid,0,0,0);
			if (PVPOP_NX_GET_INFO(uvp,
					&applinfo,
					&nodelist,
					&nodelistcnt,
					has_priv ? VPROC_HAS_PRIV : 0,
					uid, &error)
				!= KERN_SUCCESS) {
				nodelist = (LP_MAP_T)NULL;
			}
		}
		VPROC_RELEASE(uvp, "table");
		break;
	}
#endif /* TNC */
#endif /* NX */
#ifdef SLL
	case TBL_FASTNODE: {
		/*
		 * Lock data structures.
		 */
		SLL_LOCK(sll_lock);

		/*
		 * Check for time-out if we don't set fast_node.
		 */
		if ( ! set ) {
			(void) sll_timeout();
		}
		break;
	}
#endif SLL
#ifdef VPROC_DEBUG
	case TBL_VPROCINFO:
		if (uap->lel == 0 && uap->nel >= SHRT_MAX) {
			*retval = nvproc;
			return(ESUCCESS);
		}
		break;
#endif  /* VPROC_DEBUG */
	}

	/*
	 *  Main loop for each element.
	 */

	while (uap->nel > 0) {
		dev_t nottyd;
		int iv;

		union {
			struct tbl_loadavg t_u_tl;
			struct tbl_procinfo t_u_tp;
			struct tbl_vprocinfo t_u_tv;
			struct tbl_sysinfo t_u_ts;
			struct tbl_intr t_u_ti;
			struct tbl_dkinfo t_u_td;
			struct tbl_ttyinfo t_u_tt;
			struct user t_u_fake;
                        struct tbl_pginfo_10 t_u_pginfo;

			struct tbl_if t_u_if;
			struct tbl_rnh t_u_rnh;
			struct tbl_rn t_u_rn;
#ifdef TBL_RTHASH_HOST
			struct tbl_rthash t_u_rthash;
#endif
			struct tbl_arptab t_u_arptab;
			struct tbl_arpparam t_u_arpparam;
			struct tbl_unpcb t_u_unpcb;
			struct rtstat t_u_rtstat;
			struct mbstat t_u_mbstat;
			struct tbl_inpcb t_u_inpcb;
			struct ipstat t_u_ipstat;
			struct icmpstat t_u_icmpstat;
			struct tcpstat t_u_tcpstat;
			struct udpstat t_u_udpstat;
			struct nfsstats t_u_nfsstats;
		} tbl_data;
#define	tl	tbl_data.t_u_tl
#define	tp	tbl_data.t_u_tp
#define	tv	tbl_data.t_u_tv
#define ts	tbl_data.t_u_ts
#define ti	tbl_data.t_u_ti
#define td	tbl_data.t_u_td
#define tt	tbl_data.t_u_tt
#define fake	tbl_data.t_u_fake

#define  td_pginfo      tbl_data.t_u_pginfo

#define td_if		tbl_data.t_u_if
#define td_rnh		tbl_data.t_u_rnh
#define td_rn		tbl_data.t_u_rn
#define td_rthash	tbl_data.t_u_rthash

#define td_arptab	tbl_data.t_u_arptab
#define td_arpparam	tbl_data.t_u_arpparam
#define td_unpcb	tbl_data.t_u_unpcb

#define td_inpcb	tbl_data.t_u_inpcb
#define td_rtstat	tbl_data.t_u_rtstat
#define td_mbstat	tbl_data.t_u_mbstat
#define td_ipstat	tbl_data.t_u_ipstat
#define td_icmpstat	tbl_data.t_u_icmpstat
#define td_tcpstat	tbl_data.t_u_tcpstat
#define td_udpstat	tbl_data.t_u_udpstat
#define td_nfsstats	tbl_data.t_u_nfsstats

#ifdef	OSF1_SERVER
		extern mach_port_t host_port;
		host_load_info_data_t load_info;
		int count;
		extern char version[];
#endif	OSF1_SERVER
                
		dealloc_start = (vm_offset_t) 0;
		switch (uap->id) {
		case TBL_U_TTYD:
			nottyd = -1;
			data = (caddr_t)&nottyd;
			size = sizeof (dev_t);
			break;
		case TBL_LOADAVG:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;
#ifdef	OSF1_SERVER
			count = HOST_LOAD_INFO_COUNT;
			(void) host_info(host_port, HOST_LOAD_INFO,
					 &load_info, &count);
			bcopy((caddr_t)load_info.avenrun,
			      (caddr_t)&tl.tl_avenrun,
			      sizeof(tl.tl_avenrun));
#else	OSF1_SERVER
			bcopy((caddr_t)&avenrun[0], (caddr_t)&tl.tl_avenrun,
					sizeof(tl.tl_avenrun));
#endif	OSF1_SERVER
			tl.tl_lscale = LSCALE;
#ifdef	OSF1_SERVER
			bcopy((caddr_t)load_info.mach_factor,
			      (caddr_t)&tl.tl_mach_factor,
			      sizeof(tl.tl_mach_factor));
#else	OSF1_SERVER
			bcopy((caddr_t)&mach_factor, (caddr_t)&tl.tl_mach_factor,
					sizeof(tl.tl_mach_factor));
#endif	OSF1_SERVER
			data = (caddr_t)&tl;
			size = sizeof (tl);
			break;
		case TBL_INTR:
		    {
			int	i;
			extern int	scmax;
			extern int	sccount[];
#ifndef	OSF1_SERVER
			extern long	swtch_tsk_ctxt_cnt;
			extern long	swtch_thrd_ctxt_cnt;
#endif
			extern long	handler_stats();

			if (uap->index != 0 || uap->nel != 1)
				goto bad;
			
			bzero((caddr_t)&ti, sizeof(ti));

			ti.in_devintr = handler_stats(INTR_NOTCLOCK);

#ifdef	OSF1_SERVER
                        ti.in_context = 0;	/* This info not available */
#else
                        ti.in_context = swtch_tsk_ctxt_cnt +swtch_thrd_ctxt_cnt;
#endif

			ti.in_syscalls = 0;
			for(i=0; i < scmax; i++)
				ti.in_syscalls += sccount[i];
                        ti.in_forks = sccount[SYS_fork];
                        ti.in_vforks = sccount[SYS_vfork];

                        data = (caddr_t)&ti;
                        size = sizeof(ti);
			break;
		    }
		case TBL_INCLUDE_VERSION:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;
			iv = INCLUDE_VERSION;
			data = (caddr_t)&iv;
			size = sizeof(iv);
			break;
		case TBL_MAXUPRC:
			data = (caddr_t)&u.u_maxuprc;
		        size = sizeof(u.u_maxuprc);
			break;
		case TBL_UAREA:
		   {
#ifndef	OSF1_SERVER
			task_t		task;
			thread_t	thread;
#endif	OSF1_SERVER

			/*
			 *	Lookup process by pid
			 *	Proc 0 not in the hash table
			 */
			if (uap->index == 0)
				p = &proc[0];
			else
				p = pfind(uap->index);

			if (p == (struct proc *)0)
				/*
				 *	No such process
				 */
				return (ESRCH);

#if	SEC_BASE
			/*
			 * To retrieve information about another process
			 * requires privilege.
			 */
			if (p != u.u_procp && !privileged(SEC_DEBUG, EPERM))
				return (EPERM);
#else
			if (p != u.u_procp &&
			    (error = sameuser(u.u_cred, p->p_rcred)) &&
			    (error = suser(u.u_cred, &u.u_acflag)))
				return (error);
#endif

#ifdef	OSF1_SERVER
			fake_u(&fake, p, p->p_thread);
#else	OSF1_SERVER
			/*
			 *	Before we can block (any VM code), make
			 *	another reference to the task to keep it
			 *	alive.
			 */

			task = p->task;
			if (!task) return(ESRCH);
			task_reference(task);

			task_lock(task);
			thread = (thread_t) task->thread_list.next;
			thread_reference(thread);
			task_unlock(task);

			if (!thread) {
				task_deallocate(task);
				return(ESRCH);
			}

			fake_u(&fake, thread);
			task_deallocate(task);
			thread_deallocate(thread);
#endif	OSF1_SERVER

			data = (caddr_t) &fake;
			size = (vm_size_t) sizeof(struct user);
			break;
		   }

		case TBL_ARGUMENTS:
                   {
			register struct utask	*utaskp;
			/*
			 *	Returns the first N bytes of the user args,
			 *	Odd data structure is for compatibility.
			 */
			/*
			 *	Lookup process by pid
			 *	Proc 0 not in the hash table
			 */
			if (uap->index == 0)
				p = &proc[0];
			else
				p = pfind(uap->index);
			if (p == (struct proc *)0)
				/*
				 *	No such process
				 */
				return (ESRCH);
#if	SEC_BASE
			/*
			 * To retrieve information about another process
			 * requires privilege.
			 */
			if (p != u.u_procp && !privileged(SEC_DEBUG, EPERM))
				return (EPERM);
#else
			if (p != u.u_procp &&
			    (error = sameuser(u.u_cred, p->p_rcred)) &&
			    (error = suser(u.u_cred, &u.u_acflag)))
				return (error);
#endif
                        /*
                         *      Get task struct
                         */
#ifdef	OSF1_SERVER
			utaskp = &p->p_utask;
#else	OSF1_SERVER
                        utaskp = p->task->u_address;

			/*
			 *	Get map for process
			 */
			proc_map = p->task->map;
#endif	OSF1_SERVER

			/*
			 *	If the user expects no more than N bytes of
			 *	argument list, use that as a guess for the
			 *	size.
			 */
			if ((arg_size = uap->lel) == 0) {
				error = EINVAL;
				goto bad;
			}
			arg_addr = (vm_offset_t)utaskp->uu_argp;
                        copy_size = MIN(utaskp->uu_arg_size, arg_size);

                        /*
                         * If there are no arguments then return empty
                         * buffer
                         */
                        if (copy_size == 0 || utaskp->uu_argp == NULL) {
                                return(0);
                        }
                        
#ifdef	OSF1_SERVER
			if (vm_read(p->p_task,
				    arg_addr,
				    copy_size,
				    &copy_start,
				    &copy_size) != KERN_SUCCESS)
			  goto bad;
#else	OSF1_SERVER
			/*
			 *	Make a reference to the map to keep it alive,
			 *	then make the copy.
			 */
			vm_map_reference(proc_map);
			if (vm_map_copyin(proc_map, arg_addr, copy_size, FALSE, &copy_result)
			    != KERN_SUCCESS) {
			    	vm_map_deallocate(proc_map);
				goto bad;
			}
			vm_map_deallocate(proc_map);

			/*
			 *	Drop the copy here.
			 */

			if (vm_map_copyout(kernel_pageable_map, &copy_start, copy_result)
			    != KERN_SUCCESS) {
				vm_map_copy_discard(copy_result);
				goto bad;
			}
#endif	OSF1_SERVER

			/*
			 * Return arguments
			 */
			data = (caddr_t) copy_start;
			size = (vm_size_t) copy_size;

			copy_end = copy_start + copy_size;

			dealloc_start = copy_start;
			dealloc_end = copy_end;
			break;
		}
#ifdef NX
		case TBL_IPD_MSG_INFO:
		   {
			register struct utask	*utaskp;

			p = pfind(uap->index);
			if (p == (struct proc *)0)
				return (ESRCH);

#ifdef OSF1_SERVER
			utaskp = &p->p_utask;
#else
			utaskp = p->task->u_address;
			proc_map = p->task->map;
#endif
			data =(caddr_t) &(utaskp->uu_ipd_msg_info);
			size = uap->lel;
			break;
		   }
#endif	/* NX */	
		case TBL_ENVIRONMENT: {
			register struct utask	*utaskp;
			/*
			 *	Returns the first N bytes of the environment,
			 *	Odd data structure is for compatibility.
			 */
			/*
			 *	Lookup process by pid
			 *	Proc 0 not in the hash table
			 */
			if (uap->index == 0)
				p = &proc[0];
			else
				p = pfind(uap->index);
			if (p == (struct proc *)0)
				/*
				 *	No such process
				 */
				return (ESRCH);
#if	SEC_BASE
			/*
			 * To retrieve information about another process
			 * requires privilege.
			 */
			if (p != u.u_procp && !privileged(SEC_DEBUG, EPERM))
				return (EPERM);
#else
			if (p != u.u_procp &&
			    (error = sameuser(u.u_cred, p->p_rcred)) &&
			    (error = suser(u.u_cred, &u.u_acflag)))
				return (error);
#endif
                        /*
                         *      Get task struct
                         */
#ifdef	OSF1_SERVER
			utaskp = &p->p_utask;
#else	OSF1_SERVER
                        utaskp = p->task->u_address;

			/*
			 *	Get map for process
			 */
			proc_map = p->task->map;
#endif	OSF1_SERVER

			/*
			 *	If the user expects no more than N bytes of
			 *	argument list, use that as a guess for the
			 *	size.
			 */
			if ((arg_size = uap->lel) == 0) {
				error = EINVAL;
				goto bad;
			}
			arg_addr = (vm_offset_t)utaskp->uu_envp;
                        copy_size = MIN(utaskp->uu_env_size, arg_size);

                        /*
                         * If there is no environment then return empty
                         * buffer
                         */
                        if (copy_size == 0 || utaskp->uu_envp == NULL ) {
                                return (0);
                        }
                        
#ifdef	OSF1_SERVER
			if (vm_read(p->p_task,
				    arg_addr,
				    copy_size,
				    &copy_start,
				    &copy_size) != KERN_SUCCESS)
			  goto bad;
			copy_end = copy_start + copy_size;
#else   OSF1_SERVER
			/*
			 *	Make a reference to the map to keep it alive,
			 *	then make the copy.
			 */
			vm_map_reference(proc_map);
			if (vm_map_copyin(proc_map, arg_addr, copy_size, FALSE, &copy_result)
			    != KERN_SUCCESS) {
			    	vm_map_deallocate(proc_map);
				goto bad;
			}
			vm_map_deallocate(proc_map);

			/*
			 *	Drop the copy here.
			 */

			if (vm_map_copyout(kernel_pageable_map, &copy_start, copy_result)
			    != KERN_SUCCESS) {
				vm_map_copy_discard(copy_result);
				goto bad;
			}
#endif	OSF1_SERVER

                        /*
                         * Return arguments
                         */
                        data = (caddr_t) copy_start;
                        size = (vm_size_t) copy_size;
                        
			copy_end = copy_start + copy_size;

			dealloc_start = copy_start;
			dealloc_end = copy_end;
                        break;
                    }
		case TBL_PROCINFO:
		    {
			register struct utask	*utaskp;

			/*
			 *	Index is entry number in proc table.
			 */
			if (uap->index >= nproc || uap->index < 0)
			    goto bad;

			p = &proc[uap->index];
#if	SEC_BASE
			/*
			 * To retrieve information about another process
			 * requires privilege.
			 */
			if (p != u.u_procp && !privileged(SEC_DEBUG, EPERM))
				return (EPERM);
#else
			if (p != u.u_procp &&
			    (error = sameuser(u.u_cred, p->p_rcred)) &&
			    (error = suser(u.u_cred, &u.u_acflag)))
				return (error);
#endif
    		        PROC_LOCK(p);
			if ((p->p_stat == 0) || (p->p_stat == SIDL) || (p->p_vproc == NULL)) {
			    bzero((caddr_t)&tp, sizeof(tp));
			    tp.pi_status = PI_EMPTY;
			    PROC_UNLOCK(p);	
			}
			else {
			    tp.pi_uid	= p->p_rcred->cr_uid;
                            tp.pi_ruid  = p->p_ruid;
                            tp.pi_svuid = p->p_svuid;
                            tp.pi_rgid  = p->p_rgid;
                            tp.pi_svgid = p->p_svgid;
			    PROC_UNLOCK(p);
			    tp.pi_pid	= p->p_pid;
			    tp.pi_ppid	= p->p_ppid;
                            tp.pi_pgrp  = p->p_pgid;
			    tp.pi_jobc	= 0;	/* Not easily supported */
                            tp.pi_session = p->p_sid;
			    tp.pi_flag	= p->p_flag;
                            tp.pi_cursig = p->p_cursig;
                            tp.pi_sig   = p->p_sig;
                            tp.pi_sigmask = p->p_sigmask;
                            tp.pi_sigignore = p->p_sigignore;
                            tp.pi_sigcatch = p->p_sigcatch;
                            
#ifdef	OSF1_SERVER
			    if (p->p_task == MACH_PORT_NULL)
#else	OSF1_SERVER
			    if (p->task == TASK_NULL)
#endif	OSF1_SERVER
			    {
				tp.pi_status = PI_ZOMBIE;
			    }
			    else {
#ifdef	OSF1_SERVER
				utaskp = &p->p_utask;
#else	OSF1_SERVER
				utaskp = p->task->u_address;
#endif	OSF1_SERVER

				if (VPOP_CTTY_GETATTR(p->p_vproc,
						      0,
						      (pid_t *)&tp.pi_tpgrp,
						      (dev_t *)&tp.pi_ttyd,
						      0,
						      0) == ENOTTY) {
				    tp.pi_ttyd = NODEV;
                                    tp.pi_tsession = 0;
                                    tp.pi_tpgrp = 0;
                                }
				else {
                                    tp.pi_tsession = p->p_sid;
                                }
				bcopy(utaskp->uu_comm, tp.pi_comm,
				      MAXCOMLEN);
				tp.pi_comm[MAXCOMLEN] = '\0';

				if (p->p_flag & SWEXIT)
				    tp.pi_status = PI_EXITING;
				else
				    tp.pi_status = PI_ACTIVE;
			    }
			}

			data = (caddr_t)&tp;
			size = sizeof(tp);
			break;
		    }
                case TBL_SYSINFO:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			/*
			 * Get the slot info from the MACH Kernel
			 *
			 * NOTE: Can't use the NCPUS value here because the
			 *       value in the server differs from the value
			 *       used in the MACH Kernel.  IF the MACH Kernel
			 *       NCPUS value is larger than our arbitrary
			 *       MAXSYSINFOSZ then we flag the error.
			 */
			ncpu = ts.si_user = ts.si_sys = ts.si_idle = 0;
			while ((xxx_slot_info(mach_task_self(),
						ncpu,
						&si[ncpu]))
							== KERN_SUCCESS) {
                        	ts.si_user += 
					si[ncpu].cpu_ticks[CPU_STATE_USER];
                        	ts.si_sys += 
					si[ncpu].cpu_ticks[CPU_STATE_SYSTEM];
                        	ts.si_idle += 
					si[ncpu].cpu_ticks[CPU_STATE_IDLE];
				ncpu++;
				if (ncpu >= MAXSYSINFOSZ) {
					printf("table(TBL_SYSINFO) ncpu "
						">= MAXSYSINFOSZ\n");
					goto bad;
				}
			}
			/*
			 * Check that there was at least one valid
			 * xxx_slot_info() call
			 */
			if (ncpu == 0)
				goto bad;

		        /*
			 * Get times for dead threads in the server
			 */
			count = TASK_BASIC_INFO_COUNT;
			if ((task_info(mach_task_self(),
					TASK_BASIC_INFO,
					(task_info_t)&svr_termd_info,
					(mach_msg_type_number_t *)&count))
							!= KERN_SUCCESS)
				goto bad;
			
			/*
			 * Get times for live threads in the server
			 */
			count = TASK_THREAD_TIMES_INFO_COUNT;
			if ((task_info(mach_task_self(),
					TASK_THREAD_TIMES_INFO,
					(task_info_t)&svr_live_info,
					(mach_msg_type_number_t *)&count))
							!= KERN_SUCCESS)
				goto bad;

			/*
			 * Adjust the SYSTEM time to include the server task
			 * user time and subtract it from the USER time
			 */
			time_value_add(&svr_termd_info.user_time,
						&svr_live_info.user_time);

			multi = TIME_MICROS_MAX / hz;
			ticks = svr_termd_info.user_time.seconds * hz;
			ticks += svr_termd_info.user_time.microseconds / multi;
			if ((svr_termd_info.user_time.microseconds % multi)
							>= (multi / 2))
				ticks++;
				
			ts.si_sys += ticks;
			ts.si_user -= ticks;
					
			/*
			 * The NICE value is NOT available
			 */
                        ts.si_nice = 0;

                        ts.si_hz = hz;
                        ts.si_phz = phz;
			ts.si_boottime = boottime.tv_sec;
                        data = (caddr_t)&ts;
                        size = sizeof(ts);
                        break;
                case TBL_DKINFO:
#ifdef	OSF1_SERVER
			printf("table(TBL_DKINFO) not implemented\n");
			goto bad;
#else	OSF1_SERVER
			if (uap->index > DK_NDRIVE || uap->nel != 1)
				goto bad;
                        td.di_ndrive = dk_ndrive;
                        td.di_busy = dk_busy;
                        td.di_time = dk_time[uap->index];
                        td.di_seek = dk_seek[uap->index];
                        td.di_xfer = dk_xfer[uap->index];
                        td.di_wds = dk_wds[uap->index];
                        td.di_wpms = dk_wpms[uap->index];
#ifdef mips
#define __DKNAME
                        {
                                extern char *szinfo();
                                data = szinfo(uap->index, &td.di_unit);
                                if (data)
                                        bcopy(data, td.di_name, DI_NAMESZ);
                                else
                                        goto bad;
                                td.di_name[DI_NAMESZ] = '\0';
                        }
#endif
#ifdef multimax
#define __DKNAME
                        {
                                extern char *msdinfo();
                                data = msdinfo(uap->index, &td.di_unit);
                                if (data)
                                        bcopy(data, td.di_name, DI_NAMESZ);
                                else
                                        goto bad;
                                td.di_name[DI_NAMESZ] = '\0';
                        }
#endif
#ifdef i386
#define __DKNAME
                        {
                                extern char *hd_info();
                                data = hd_info(uap->index, &td.di_unit);
                                if (data)
                                        bcopy(data, td.di_name, DI_NAMESZ);
                                else
                                        goto bad;
                                td.di_name[DI_NAMESZ] = '\0';
                        }
#endif
#ifndef __DKNAME
                        td.di_unit = uap->index;
                        bcopy("dk", td.di_name, 3);
#endif                        
#undef __DKNAME
                        data = (caddr_t)&td;
                        size = sizeof(td);
                        break;
#endif	OSF1_SERVER
                case TBL_TTYINFO:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;
                        tt.ti_nin = tk_nin;
                        tt.ti_nout = tk_nout;
                        tt.ti_cancc = tk_cancc;
                        tt.ti_rawcc = tk_rawcc;
                        data = (caddr_t)&tt;
                        size = sizeof(tt);
                        break;
 		case TBL_MSGDS:
			if (uap->index > msginfo.msgmni || uap->index < 0)
			    goto bad;
	
			data = (caddr_t) &msgque[uap->index];
			size = sizeof(struct msqid_ds);

#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}
#endif

			break;
		case TBL_SEMDS:
			if (uap->index > seminfo.semmni || uap->index < 0)
			    goto bad;

			data = (caddr_t) &sema[uap->index];
			size = sizeof(struct semid_ds);

#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}
#endif

			break;
		case TBL_SHMDS:
			if (uap->index > shminfo.shmmni || uap->index < 0)
			    goto bad;

			data = (caddr_t) &(shmem[uap->index].s);
			size = sizeof(struct shmid_ds);

#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}
#endif

			break;
		case TBL_MSGINFO:
			switch (uap->index) {
			  case MSGINFO_MAX:
			    data = (caddr_t)&msginfo.msgmax;
			    size = sizeof(msginfo.msgmax);
			    break;
			  case MSGINFO_MNB:
			    data = (caddr_t)&msginfo.msgmnb;
			    size = sizeof(msginfo.msgmnb);
			    break;
			  case MSGINFO_MNI:
			    data = (caddr_t)&msginfo.msgmni;
			    size = sizeof(msginfo.msgmni);
			    break;
			  case MSGINFO_TQL:
			    data = (caddr_t)&msginfo.msgtql;
			    size = sizeof(msginfo.msgtql);
			    break;
			  default:
			    goto bad;
			}	

#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}
#endif
			
			break;
		case TBL_SEMINFO:
			switch (uap->index) {
			  case SEMINFO_MNI:
			    data = (caddr_t)&seminfo.semmni;
			    size = sizeof(seminfo.semmni);	
			    break;
			  case SEMINFO_MSL:
			    data = (caddr_t)&seminfo.semmsl;
			    size = sizeof(seminfo.semmsl);
			    break;
			  case SEMINFO_OPM:
			    data = (caddr_t)&seminfo.semopm;
			    size = sizeof(seminfo.semopm);
			    break;				
			  case SEMINFO_UME:
			    data = (caddr_t)&seminfo.semume;
			    size = sizeof(seminfo.semume);
			    break;
			  case SEMINFO_VMX:
			    data = (caddr_t)&seminfo.semvmx;
			    size = sizeof(seminfo.semvmx);
			    break;
			  case SEMINFO_AEM:
			    data = (caddr_t)&seminfo.semaem;
			    size = sizeof(seminfo.semaem);
			    break;
			  default:
			    goto bad;
			}	
#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}	
#endif
			
			break;
		case TBL_SHMINFO:
			switch (uap->index) {
			  case SHMINFO_MAX:
			    data = (caddr_t)&shminfo.shmmax;
			    size = sizeof(shminfo.shmmax);
			    break;
			  case SHMINFO_MIN:
			    data = (caddr_t)&shminfo.shmmin;
			    size = sizeof(shminfo.shmmin);
			    break;
			  case SHMINFO_MNI:
			    data = (caddr_t)&shminfo.shmmni;
			    size = sizeof(shminfo.shmmni);
			    break;
			  case SHMINFO_SEG:
			    data = (caddr_t)&shminfo.shmseg;
			    size = sizeof(shminfo.shmseg);
			    break;
			  default:
			    goto bad;
			}		

#ifdef SVIPC_PIDSIZE
			if (size != uap->lel) {
			    error = EINVAL;
			    goto bad;
			}	
#endif

			break;
		case TBL_VERSION:
			{
			extern char version[];
			data = version;
			size = strlen(version)+1;
			}
			break;

                case TBL_PGINFO:
                        size = 0;
                        if (uap->index < -1) {
                                goto bad;
                        } else {
                                if ((uap->lel != 0) && 
                                    (uap->lel != sizeof(struct tbl_pginfo_10))){
                                        return(EINVAL);
                                }

                                if (uap->index == -1) {
                                        /* get stats for all nodes */
                                        error = remote_get_paging_stats_all(
                                                 (uap->lel == 0) ? TRUE : FALSE,
                                                 uap->addr, uap->nel, retval);
                                } else {
                                        error = remote_get_paging_stats(
                                                 uap->index, 
                                                 (uap->lel == 0) ? TRUE : FALSE,
                                                 uap->addr, uap->nel, retval);
                                }


                                /* 
                                 * we won't go around the while loop for each 
                                 * entry - it's pretty bogus. Just get them all
                                 * in one shot and set nel correctly to make 
                                 * the while loop happy. Also size is 0 so we 
                                 * will avoid the coyin at the bottom of the
                                 * case. Decrement retval by 1 since it gets
                                 * incremented by 1 a the end of the case!!!
                                 */ 
                                uap->nel = 1 ;
                                if (!error) {
                                        *retval -= 1;
                                }
                        }
                        break;

#ifdef	VPROC_DEBUG
		case TBL_VPROCINFO:
		    {
			register struct pvproc *pvp;

			/* check we're within bounds of the vproc table */
			if ((uap->index < 0) || (uap->index >= nvproc))
				goto bad;

			bzero((caddr_t)&tv, sizeof(tv));
                        data = (caddr_t)&tv;
                        size = sizeof(tv);
			/*
			 * If we're looping through a series of vprocs
			 * then vp will be our last position. Otherwise,
			 * we have to scan through the table (a linked list)
			 * until we arrive at the one we want.
			 */
			if (vp != NULL)
				vp = vp->vp_next;
			else { 
				for (vp = vproc;
				     vp != NULL && vp->vp_index != uap->index;
				     vp = vp->vp_next)
					;
			}
			if (vp == NULL)
				goto bad;

			pvp = PVP(vp);
			/*
			 * continue only if the vproc is in use
			 */
			tv.vi_ref_cnt = vp->vp_ref_cnt;
			if (tv.vi_ref_cnt <= 0 ) {
				tv.vi_status = VI_EMPTY;
				break;
			}

			/*
			 * load data into user buffer, purifying where necessary
			 */

			tv.vi_pid	= vp->vp_pid;
			tv.vi_flag	= pvp->pvp_flag;

			/*
			 * these fields must be converted from addresses
			 * to (p)vproc array indexes
			 */
#define	VPROC_INDEX(vp) \
	((vp)==NULL ? -1 : ((vp)->vp_index))
			tv.vi_head_childl = VPROC_INDEX(pvp->pvp_head_childl);
			tv.vi_head_foster_childl =
					    VPROC_INDEX(pvp->pvp_head_foster_childl);
			tv.vi_foster_childl =
					    VPROC_INDEX(pvp->pvp_foster_childl);
			tv.vi_childl	  = VPROC_INDEX(pvp->pvp_childl);
			tv.vi_head_pgrpl  = VPROC_INDEX(pvp->pvp_head_pgrpl);
			tv.vi_pgrpl	  = VPROC_INDEX(pvp->pvp_pgrpl);
			tv.vi_sessionl	  = VPROC_INDEX(pvp->pvp_sessionl);

			tv.vi_jobc	= pvp->pvp_jobc;
			tv.vi_cttyd	= pvp->pvp_cttyp ?
						pvp->pvp_cttyp->t_dev : 0;

                        tv.vi_mach_portname = vproc_to_port_lookup(vp);

			(void) mach_port_get_refs(mach_task_self(),
						  vproc_to_port_lookup(vp),
						  MACH_PORT_RIGHT_RECEIVE,
						  &tv.vi_mach_receives);
			(void) mach_port_get_refs(mach_task_self(),
						  vproc_to_port_lookup(vp),
						  MACH_PORT_RIGHT_SEND,
						  &tv.vi_mach_sends);

#ifdef	TNC
			/*
			 * continue so long as the process is local
			 */
			if (!(pvp->pvp_flag & PV_IS_LOCAL)) {
				tv.vi_status = VI_REMOTE;
				break;
			}
#endif
			tv.vi_status	= VI_LOCAL;
			tv.vi_pproc_slot= pvp->pvp_pproc ?
						(pvp->pvp_pproc - proc) : -1;
			tv.vi_ppid	= pvp->pvp_ppid;
			tv.vi_foster_ppid = pvp->pvp_foster_ppid;
			tv.vi_pgid	= pvp->pvp_pgid;
			tv.vi_sid	= pvp->pvp_sid;

			break;

		    }
#endif	/* VPROC_DEBUG */

#ifdef TNC
		/*
		 * Note:
		 * A call with lel == 0 and nel == MAXSHORT can be used to
		 * determine the length of a table (in elements) before
		 * actually requesting any of the data.
		 * We have to munge this a little to return the total number
		 * of nodes and not the number of ranges in the node_array,
		 * for example:
		 *	[0][9]
		 *	[20][29]
		 *	[40][49]	
		 * specifies 3 ranges but a total of 30 nodes.
		 */
		case TBL_NODEINFO:
		case TBL_PHYSNODEINFO:
		{
			/*
			 * return nodes from the node table entry
			 * specified bu uap->index.
			 */
			extern int node_array_entries;
			extern node_t node_array[][2];
			static int retnode;     /* return node */
			int nodesum = 0;     	/* nodes counted so far */
			int tableindex = 0;	/* node table index */
			int nodes;		/* number of nodes this row */

			if (uap->index < 0)
				goto bad;
#ifdef NX
			if ( in_partition ) {
				/*
				 * We return the index of the uap->index-th
				 * valid node.
				 */

				int	node_index = -1;
				int	node_cnt = uap->index;


				if ( node_cnt >= nodelistcnt ) {
					/*
					 * Not that many nodes.
					 */
					goto bad;
				}

				while ( node_cnt >= 0 &&
					++node_index < nodelistcnt) {

					if ( is_node_valid(nodelist[node_index]) ) {
						/*
						 * We count the valid node.
						 */
						node_cnt--;
					}
				}

				if ( node_index >= nodelistcnt ) {
					/*
					 * Not that many nodes.
					 */
					goto bad;
				}

				if ( uap->id == TBL_PHYSNODEINFO ) {
					retnode = nodelist[node_index];
				}
				else {
					retnode = node_index;
				}
				data = (caddr_t) &retnode;
				size = sizeof(retnode);
				break;
			}
#endif /* NX */

			/*
			 * Find the slot in the node table that contains
			 * the desired node pointed to by uap->index.
			 * Count the nodes present in each table row
			 * and keep a running total in the nodesum variable
			 * so we know in which table row the desired node
			 * is contained.
			 */
			while (tableindex < node_array_entries) {
				nodes = node_array[tableindex][1] -
					node_array[tableindex][0] + 1;
				nodesum += nodes;
				if (uap->index < nodesum)
					break; 
				else
					tableindex++;
			}

			/*
			 * if the index has gone past the bounds, bail out
			 */
			if (tableindex >= node_array_entries)
				goto bad;

			/*
			 * tableindex now holds the row in the node_array 
			 * with the desired node.  Calculate the node
			 * to return.
			 */
			retnode = node_array[tableindex][0] + 
			  	  (uap->index - (nodesum - nodes));
			data = (caddr_t)&retnode;
			size = sizeof(retnode);
			break;	
		}
#endif /* TNC */

		case TBL_RTSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			NETSTAT_LOCK(&rtstat.rts_lock);
			bcopy((caddr_t)&rtstat,
				(caddr_t)&td_rtstat,
				sizeof(struct rtstat));
			NETSTAT_UNLOCK(&rtstat.rts_lock);

			data = (caddr_t)&td_rtstat;
			size = sizeof(struct rtstat);
			break;

		case TBL_MBSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			MBSTAT(bcopy((caddr_t)&mbstat,
				     (caddr_t)&td_mbstat,
				     sizeof(struct mbstat)));

			data = (caddr_t)&td_mbstat;
			size = sizeof(struct mbstat);
			break;

		case TBL_IPSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			NETSTAT_LOCK(&ipstat.ipslock);
			bcopy((caddr_t)&ipstat,
				(caddr_t)&td_ipstat,
				sizeof(struct ipstat));
			NETSTAT_UNLOCK(&ipstat.ipslock);

			data = (caddr_t)&td_ipstat;
			size = sizeof(struct ipstat);
			break;

		case TBL_ICMPSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			NETSTAT_LOCK(&icmpstat.icmpslock);
			bcopy((caddr_t)&icmpstat,
				(caddr_t)&td_icmpstat,
				sizeof(struct icmpstat));
			NETSTAT_UNLOCK(&icmpstat.icmpslock);

			data = (caddr_t)&td_icmpstat;
			size = sizeof(struct icmpstat);
			break;

		case TBL_TCPSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			NETSTAT_LOCK(&tcpstat.tcpslock);
			bcopy((caddr_t)&tcpstat,
				(caddr_t)&td_tcpstat,
				sizeof(struct tcpstat));
			NETSTAT_UNLOCK(&tcpstat.tcpslock);
			
			data = (caddr_t)&td_tcpstat;
			size = sizeof(struct tcpstat);
			break;
			
		case TBL_UDPSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			NETSTAT_LOCK(&udpstat.udpslock);
			bcopy((caddr_t)&udpstat,
				(caddr_t)&td_udpstat,
				sizeof(struct udpstat));
			NETSTAT_UNLOCK(&udpstat.udpslock);
			
			data = (caddr_t)&td_udpstat;
			size = sizeof(struct udpstat);
			break;

		case TBL_NFSSTAT:
			if (uap->index != 0 || uap->nel != 1)
				goto bad;

#if NFS			
			NFS_STATS(bcopy((caddr_t)&nfsstats,
					(caddr_t)&td_nfsstats,
					sizeof(struct nfsstats)));
#else   /* NFS */
			bzero(&td_nfsstats, sizeof(struct nfsstats));
#endif /* NFS */
			
			data = (caddr_t)&td_nfsstats;
			size = sizeof(struct nfsstats);
			break;

		case TBL_UNPCB: {
			int		index = 0;
			struct file	*fp = file;
			struct socket	*so;
			struct unpcb	*unp;
			extern struct domain unixdomain;

			if (uap->index < 0)
				goto bad;
			
			/* short-circuit */
			if (uap->lel == 0) {
				*retval = unixdomain.dom_refcnt;
				goto out;
			}

			/*
			 * Sadly, there is no real way to find AF_UNIX
			 * sockets.  This is the scheme used by netstat.
			 * (Rated XXX)
			 */

			index = uap->index;

			/*
			 * Index only counts AF_UNIX sockets.
			 * We want to do a "safe check" to only take
			 * locks on sockets, so we look at f_type twice.
			 */
			for (fp = flisthead; fp != (struct file *)NULL; 
						fp = fp->f_filelist) {
				if (fp->f_count == 0 ||
#ifdef OSF1_ADFS
				    fp->f_magic != F_MAGIC ||
#endif
				    fp->f_type != DTYPE_SOCKET)
					continue;
				FP_LOCK(fp);
				/* Make sure it didn't change */
				if (fp->f_count == 0 ||
#ifdef OSF1_ADFS
				    fp->f_magic != F_MAGIC ||
#endif
				    fp->f_type != DTYPE_SOCKET) {
					FP_UNLOCK(fp);
					continue;
				}
				so = (struct socket *)fp->f_data;
				/* XXX: socket lock */
				if (so->so_proto < unixdomain.dom_protosw ||
				    so->so_proto >= unixdomain.dom_protoswNPROTOSW ||
				    !so->so_pcb) {
					FP_UNLOCK(fp);
					continue;
				}
				if (index-- == 0)
					break;
				FP_UNLOCK(fp);
			}

			/* We *may* have hit EOF, so do not return error */
			if (fp == (struct file *)NULL)
				goto out;

#define C(f) td_unpcb.f = so->f
			C(so_type);
			C(so_type);
			C(so_options);
			C(so_linger);
			C(so_state);
			C(so_q0len);
			C(so_qlen);
			C(so_qlimit);
			C(so_dqlen);
			C(so_timeo);
			C(so_error);
			C(so_special);
			C(so_pgid);
			C(so_oobmark);
			C(so_rcv);
			C(so_snd);
#undef C

			unp = (struct unpcb *)so->so_pcb;
			td_unpcb.unpcb = *unp;
			if (unp->unp_addr)
				td_unpcb.unp_addr =
					*mtod(unp->unp_addr,
					      struct sockaddr_un *);
			else
				bzero((char *)&td_unpcb.unp_addr,
					sizeof (struct sockaddr_un));

			FP_UNLOCK(fp);

			data = (caddr_t)&td_unpcb;
			size = sizeof(struct tbl_unpcb);
			
			break;
		}

		case TBL_TCPCB:
		case TBL_UDPCB: {
			extern struct inpcb tcb, udb;
			struct inpcb	*pcb, *head;
			int		index = 0;

			if (uap->index < 0)
				goto bad;

			head = uap->id == TBL_TCPCB ? &tcb : &udb;

			INHEAD_READ_LOCK(head);

			/* short-circuit */
			if (uap->lel == 0) {
				index = 0;
				for (pcb = head->inp_next;
				     pcb != head;
				     pcb = pcb->inp_next)
					index++;
				INHEAD_READ_UNLOCK(head);
				*retval = index;
				goto out;
			}

			index = uap->index;

			for (pcb = head->inp_next;
			     pcb != head && index-- > 0;
			     pcb = pcb->inp_next)
				;

			/* We *may* have hit EOF, so do not return error */
			if (pcb == head) {
				INHEAD_READ_UNLOCK(head);
				goto out;
			}

			INPCB_LOCK(pcb);

			td_inpcb.inp_next = (caddr_t)pcb->inp_next;
#define C(f) td_inpcb.f = pcb->f
			C(inp_ppcb);
			C(inp_faddr);
			C(inp_fport);
			C(inp_laddr);
			C(inp_lport);
#undef C

#define C(f) td_inpcb.f = pcb->inp_socket->f
			C(so_type);
			C(so_type);
			C(so_options);
			C(so_linger);
			C(so_state);
			C(so_q0len);
			C(so_qlen);
			C(so_qlimit);
			C(so_dqlen);
			C(so_timeo);
			C(so_error);
			C(so_special);
			C(so_pgid);
			C(so_oobmark);
			C(so_rcv);
			C(so_snd);
#undef C

			if (uap->id == TBL_TCPCB)
				td_inpcb.i_tcpcb =
					*(struct tcpcb *)pcb->inp_ppcb;

			INPCB_UNLOCK(pcb);
			INHEAD_READ_UNLOCK(head);
			
			data = (caddr_t)&tbl_data.t_u_inpcb;
			size = sizeof(struct tbl_inpcb);
			
			break;
		}
/*
 *  For TNC, some ifnet structures marked invisible.
 *  They should only be seen if a particular debugging flag is set.
 */
#ifdef  TNC
#  if NMI > 0 && defined(MI_DEBUG)
#    define IfnetVisible(ifp) \
        ( (mi_debug & MD_INVF) || \
         (!(ifp->if_flags & IFF_INVISIBLE)) )
#  else
#    define IfnetVisible(ifp) \
        ( !(ifp->if_flags & IFF_INVISIBLE) )
#  endif
#else /* ! TNC */
#  define IfnetVisible(ifp) (TRUE)
#endif

		case TBL_IF: {
			struct ifnet	*ifp;
			struct ifaddr	*ifap;
			int		ctr = 0, index;
			extern struct ifnet *ifnet;

			if (uap->index < 0)
				goto bad;

			/* XXX locking */

			/* short-circuit */
			if (uap->lel == 0) {
				index = 0;
				for (ifp = ifnet; ifp; ifp = ifp->if_next){
					if( IfnetVisible(ifp) )
						index++;
				}					
				*retval = index;
				goto out;
			}
			
			index = uap->index;
			
			for (ifp = ifnet;
			     ifp && index > 0;
			     ifp = ifp->if_next){
				if( IfnetVisible(ifp) )
					index--;
			}

			/* We *may* have hit EOF, so out */
			if (!ifp)
				goto out;

			td_if.if_net = *ifp;

			ifnamecopy(ifp->if_name,
				   &td_if.if_name[0],
				   ifp->if_dvnode);

			for (ifap = ifp->if_addrlist;
			     ctr < MAX_IFADDRS && ifap != NULL;
			     ctr++, ifap = ifap->ifa_next) {
				td_if.ifaddrs[ctr].ifa =
					*(union ifaddrany *)ifap;
				if (ifap->ifa_addr != NULL)
					td_if.ifaddrs[ctr].sa =
						*(struct SockAddr *)
							ifap->ifa_addr;
				else
					bzero(&td_if.ifaddrs[ctr].sa,
						sizeof(struct SockAddr));
			}
			td_if.ifaddrcount = ctr;

			data = (caddr_t)&td_if;
			size = sizeof(struct tbl_if);

			break;
		}

		case TBL_ARPPARAM: {
			extern int	arptab_size, arptab_bsiz, arptab_nb,
					arpkillc, arpkilli, arprefresh,
					arphold, arplost, arpdead;

			if (uap->index != 0 || uap->nel != 1)
				goto bad;

			td_arpparam.arptab_size = arptab_size;
			td_arpparam.arptab_bsiz = arptab_bsiz;
			td_arpparam.arptab_nb = arptab_nb;
			td_arpparam.arpkillc = arpkillc;
			td_arpparam.arpkilli = arpkilli;
			td_arpparam.arprefresh = arprefresh;
			td_arpparam.arphold = arphold;
			td_arpparam.arplost = arplost;
			td_arpparam.arpdead = arpdead;

			data = (caddr_t)&td_arpparam;
			size = sizeof(struct tbl_arpparam);
			break;
		}

		case  TBL_ARPTAB: {
			int	i = 0, index, retsize = 0;
			extern int	arptab_size;
			register struct arptab *at;
			extern struct arptab *arptab;	/* arptab is malloc'd */
			NETSPL_DECL(s)
			
			index = uap->index;
			if (index < 0)
				goto bad;

			ARP_LOCK();
			at = arptab;
			for (i = 0; i < arptab_size; i++, at++) {
				if (at->at_flags == 0)
					continue;
				retsize++;
				if (uap->lel != 0 && index-- == 0)
					break;
			}

			/* short-circuit */
			if (uap->lel == 0) {
				ARP_UNLOCK();
				*retval = retsize;
				goto out;
			}

			/* didn't find index; no error! */
			if (i == arptab_size) {
				ARP_UNLOCK();
				goto out;
			}

			td_arptab.t_arptab = *at;
			if (at->at_if != NULL) {

				ifnamecopy(at->at_if->if_name,
					   td_arptab.if_name,
					   at->at_if->if_dvnode);

				td_arptab.if_unit = at->at_if->if_unit;
				td_arptab.if_addrlen = at->at_if->if_addrlen;
			} else {
				td_arptab.if_name[0] = '\0';
				td_arptab.if_unit = 0;
				td_arptab.if_addrlen = 0;
			}
			ARP_UNLOCK();

			data = (caddr_t)&td_arptab;
			size = sizeof(struct tbl_arptab);

			break;
		}

#ifdef RFC_1323
		case  TBL_HIPPI_ART: {
			int	i = 0, index, retsize = 0;
			register struct art_pentry *ah;
			extern struct art_pentry arttab;

			index = uap->index;
			if (uap->index < 0)
				goto bad;

			ah = (struct art_pentry *)&arttab;

			/* XXX should lock this data struct */
			for (i = 0; i < MAXARTSIZE; i++, ah++) {
				if (ah->ap_ha.al_valid == 0)
					continue;
				retsize++;
				if (uap->lel != 0 && index-- == 0)
					break;
			}

			/* short-circuit */
			if (uap->lel == 0) {
				*retval = retsize;
				goto out;
			}

			/* didn't find index; no error! */
			if (i == MAXARTSIZE) {
				goto out;
			}

			data = (caddr_t)ah;
			size = sizeof(struct art_pentry);
			break;
		}
#endif /* RFC_1323 */

#ifdef TBL_RTHASH_HOST		/* obsoleted by radix tree code */
		/*
		 * These could in-line nel work to avoid excess
		 * linked-list walking, at some added complexity.
		 *
		 * OSF/1 does not support this code.  There is no locking.
		 */
		case  TBL_RTHASH_HOST:
		case  TBL_RTHASH_NET: {
			int			i, index, size = 0;
			struct ortentry		*rte;
			struct mbuf		*rtable, *rtbucket, *m;
			extern int rthashsize;

			if (uap->index < 0)
				goto bad;

			rtable = (uap->id == TBL_RTHOST) ? rthost : rtnet;
			
			/* short-circuit */
			if (uap->lel == 0) {
				for (i = 0; i < rthashsize; i++) {
					m = rtable[i];
					for (; m; m = m->m_next)
						size++;
				}
				*retval = size;
				goto out;
			}

			index = uap->index;
			m = rtbucket = rtable[0];
			for (; index && i < rthashsize; i++) {
				m = rtbucket = rtable[i];
				for (; index && m; m = m->m_next)
					index--;
			}

			if (!m)
				goto out;

			rte = (struct ortentry *)(rtable[i]->m_dat);
			bcopy((char *)rte,
				(char &)&td_rthash.t_rtentry,
				sizeof(struct ortentry));
			td_rthash.rt_dst = *(struct SockAddr *)rte->rt_dst;
			td_rthash.rt_gateway = *(struct SockAddr *)
							rte->rt_gateway;
			if (rte->rt_ifp) {

				ifnamecopy(rte->rt_ifp->if_name,
					   td_rthash.if_name,
					   rte->rt_ifp->if_dvnode);

				td_rthash.if_unit = rte->rt_ifp->if_unit;
			} else {
				td_rthash.if_name[0] = '\0';
				td_rthash.if_unit = 0;
			}

			data = (caddr_t)&td_rthash;
			size = sizeof(struct tbl_rthash);

			break;
		}
#endif

		case TBL_RTREE_HEAD: {
			register struct radix_node_head *rnh;
			int index;
			ROUTE_LOCK_DECL()

			if (uap->index < 0)
				goto bad;

			if (uap->lel == 0) {
				ROUTE_READ_LOCK();
				index = 0;
				for (rnh = radix_node_head;
				     rnh;
				     rnh = rnh->rnh_next)
					index++;
				ROUTE_READ_UNLOCK();
				*retval = index;
				goto out;
			}

			index = uap->index;

			ROUTE_READ_LOCK();
			for (rnh = radix_node_head;
			     rnh && index-- > 0;
			     rnh = rnh->rnh_next)
				;
			if (!rnh) {
				ROUTE_READ_UNLOCK();
				goto out;
			}
			td_rnh.rnh_af = rnh->rnh_af;
#ifdef	TNC
			td_rnh.rnh_node = this_node;
#else
			td_rnh.rnh_node = (node_t)0;
#endif
			ROUTE_READ_UNLOCK();

			data = (caddr_t)&td_rnh;
			size = sizeof(struct tbl_rnh);

			break;
		}

#ifdef TNC
		case TBL_RTREE_NODE_DEBUG:
#endif /* TNC */
		case TBL_RTREE_NODE: {
			struct radix_node_head	*rnh;
			struct radix_node	*rn;
			struct rtentry		*rte;
			struct ifnet		*ifp = NULL;
			int			ord, af, find;
			unsigned int		index;
#if defined(TNC)
			/* see vsocket/if_mi.c for details */
			extern int		invisible_route_count;
#endif
			ROUTE_LOCK_DECL();

			index = (unsigned int)uap->index;

			ord = TRNORD(index);
			af = TRNAF(index);

			ROUTE_READ_LOCK();
			for (rnh = radix_node_head;
			     rnh && af != rnh->rnh_af;
			     rnh = rnh->rnh_next)
				;
			if (!rnh) {
				ROUTE_READ_UNLOCK();
				goto bad;
			}

			/* short-circuit */
			if (uap->lel == 0) {
				*retval = radix_node_count(rnh->rnh_treetop);
				ROUTE_READ_UNLOCK();
#if NMI > 0
				if (uap->id != TBL_RTREE_NODE_DEBUG) {
					/*
					 * Unless we're debugging, we're not
					 * interested in the MI routes. 
					 *
					 * XXX invisible_route_count must be
					 * kept on a per address family
					 * basis!!!  AF_INET check is
					 * temporary while MI driver only
					 * supports this address family.
					 */
					if (af == AF_INET)
						*retval -=
							invisible_route_count;
				}
#endif
				goto out;
			}
		
			find = ord;
			rn = find_radix_node(rnh->rnh_treetop,
					     &find,
					     (af == 0),
#ifdef TNC
					     (uap->id == TBL_RTREE_NODE_DEBUG)
#else
					     0
#endif
					     );
			if (!rn) {
				ROUTE_READ_UNLOCK();
				goto out;
			}

			td_rn.rn_b = rn->rn_b;
			td_rn.rn_bmask = rn->rn_bmask;
			td_rn.rn_flags = rn->rn_flags;
			if (rn->rn_key)
				td_rn.key = *(struct SockAddr *)rn->rn_key;
			if (rn->rn_mask)
				td_rn.mask = *(struct SockAddr *)rn->rn_mask;
			
			if (af != 0) {
				rte = (struct rtentry *)rn;

				if (rte->rt_gateway)
					td_rn.rt_gateway = *(struct SockAddr *)
							   rte->rt_gateway;
				td_rn.rt_flags = rte->rt_flags;
				td_rn.rt_refcnt = rte->rt_refcnt;
				td_rn.rt_use = rte->rt_use;
				td_rn.rt_rmx = rte->rt_rmx;
				td_rn.rt_idle = rte->rt_idle;

				ifp = rte->rt_ifp;
			}

			if (ifp) {
				ifnamecopy(ifp->if_name,
					   td_rn.if_name,
					   ifp->if_dvnode);
				td_rn.if_unit = ifp->if_unit;
			} else {
				td_rn.if_name[0] = '\0';
				td_rn.if_unit = 0;
			}

#ifdef	TNC
			td_rn.rn_node = this_node;
#else
			td_rn.rn_node = (node_t)0;
#endif
			ROUTE_READ_UNLOCK();

			data = (caddr_t)&td_rn;
			size = sizeof(struct tbl_rn);

			break;
		}
#ifdef SLL
		case TBL_FASTNODE: {
			/*
			 * Currently index must be 0 and nel must be 1.
			 */
			if ( uap->index != 0 || uap->nel != 1 ) {
				/*
				 * Unlock data structures.
				 */
				SLL_UNLOCK(sll_lock);

				goto bad;
			}
			data = (caddr_t) &fast_node;
			size = sizeof(node_t);

			break;
		}
#endif /* SLL */

		default:
bad:
#ifdef NX
                        if (nodelist != (LP_MAP_T)NULL) {
                            vm_deallocate(mach_task_self(),
                                          (vm_address_t)nodelist,
                            (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                        }
#endif /* NX */
			/*
			 *	Return error only if all indices
			 *	are invalid.
			 */
			if (*retval == 0)
				error = EINVAL;
			else
				error = 0;
			return (error);
		}
		/*
		 * This code should be generalized if/when other tables
		 * are added to handle single element copies where the
		 * actual and expected sizes differ or the table entries
		 * are not contiguous in kernel memory (as with TTYLOC)
		 * and also efficiently copy multiple element
		 * tables when contiguous and the sizes match.
		 */

		size = MIN(size,uap->lel);
		if (size) {
			if (set) {
				char buff[MAXLEL];

			        error = copyin(uap->addr, buff, size);
				if (error == 0)
					bcopy(buff, data, size);
			}
			else {
			  error = copyout(data, uap->addr, size);
			}

		}
		if (dealloc_start != (vm_offset_t) 0) {
#ifdef	OSF1_SERVER
			(void) vm_deallocate(mach_task_self(),
					     dealloc_start,
					     dealloc_end - dealloc_start);
#else	OSF1_SERVER
			kmem_free(kernel_pageable_map, dealloc_start,
				dealloc_end - dealloc_start);
#endif	OSF1_SERVER
		}
		if (error) {
#ifdef NX
                    if (nodelist != (LP_MAP_T)NULL) {
                        vm_deallocate(mach_task_self(),
                                      (vm_address_t)nodelist,
                                      (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                    }
#endif /* NX */
#ifdef SLL
			/*
			 * If TBL_FASTNODE unlock data structures.
			 */
			if ( uap->id == TBL_FASTNODE ) {
				SLL_UNLOCK(sll_lock);
			}
#endif /* SLL */
			return (error);
		}
		uap->addr += uap->lel;
		uap->nel -= 1;
		uap->index += 1;
		*retval += 1;
	}
out:
#ifdef SLL
		/*
		 * Do postprocessing for TBL_FASTNODE.
		 */
		if ( uap->id == TBL_FASTNODE ) {
			if ( set ) {
				/*
				 * Set time-out for fast_node.
				 */
				sll_set_timeout();

				/*
				 * Unlock data structures.
				 */
				SLL_UNLOCK(sll_lock);

				/*
				 * Check for valid node number.
				 */
				if ( sll_check_fast_node() == -1 ) {
#ifdef NX
                			if (nodelist != (LP_MAP_T)NULL) {
                    				vm_deallocate(mach_task_self(),
                                  		(vm_address_t)nodelist,
                                  		(nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                			}
#endif /* NX */
					return(EINVAL);
				}
			}
			else {
				/*
				 * Unlock data structures.
				 */
				SLL_UNLOCK(sll_lock);
			}
		}
#endif /* SLL */
#ifdef NX
                if (nodelist != (LP_MAP_T)NULL) {
                    vm_deallocate(mach_task_self(),
                                  (vm_address_t)nodelist,
                                  (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
                }
#endif /* NX */

	return(0);
}

#ifdef	TABLE_NETWORK

/*
 *  For TNC, some routes stored in the radix tree are marked invisible.
 *  They should only be seen if a particular debugging flag is set.
 */
#ifdef	TNC
#  define RtVisible(rn) \
	(ismask || show_invisible || \
	(!(((struct rtentry *)(rn))->rt_flags & RTF_INVISIBLE)))
#else /* ! TNC */
#  define RtVisible(rt)	(TRUE)
#endif

/*
 * This function returns nodes from the radix tree implementation
 * of the routing table.
#ifdef	TNC
 * The ismask argument is only used by TNC (see RtVisible() macro above),
 * but we pass it for non-TNC simply to avoid ugly #ifdef's.
#endif
 */
struct radix_node *
find_radix_node(
	struct radix_node	*rn,
	int			*rn_leaf_id,
	int			ismask,
	int			show_invisible)
{
	struct radix_node	*res;

	if (*rn_leaf_id < 0 || !rn)
		return NULL;

again:
	if (rn->rn_b < 0) {
		if ((rn->rn_flags & RNF_ROOT) == 0) {
			if (RtVisible(rn) && !(*rn_leaf_id)--){
				return rn;
			}
		}
		if (rn = rn->rn_dupedkey)
			goto again;
	} else {
		res = find_radix_node(rn->rn_l,
				      rn_leaf_id,
				      ismask,
				      show_invisible);
		if (res)
			return res;

		res = find_radix_node(rn->rn_r,
				      rn_leaf_id,
				      ismask,
				      show_invisible);
		if (res)
			return res;
	}

	return NULL;
}

int
radix_node_count(
	struct radix_node *rn)
{
	int count = 0;

	if (!rn)
		return 0;

again:
	if (rn->rn_b < 0) {
		if ((rn->rn_flags & RNF_ROOT) == 0)
			count++;
		if (rn = rn->rn_dupedkey)
			goto again;
	} else {
		count += radix_node_count(rn->rn_l);
		count += radix_node_count(rn->rn_r);
	}

	return count;
}

#if defined(TNC)
/*
 * This TNC only function takes the interface name and device node number
 * and copies it to the destination, checking that it doesn't exceed a
 * string length of IFNAMSIZ.
 */
void
ifnamecopy(
	char *ifname,
	register char *dest,
	int device_node)
{
	char temp_string[IFNAMSIZ];
	register int scnt;
	register char *cp;

	/*
	 * Convert the node number to an ASCII representation
	 */
	itoa(device_node, temp_string);
	cp = temp_string;

	scnt = 0;

	/*
	 * Don't add the <node> construct if it's already in the string.
	 * This is typically only the case for invisible routes where the
	 * MI interface name already includes the node number.
	 */
	if (ifname[0] != '<' && ifname[1] != '<') {
		*dest++ = '<';
		scnt = 1;
		while (scnt < IFNAMSIZ && *cp) {
			*dest++ = *cp++;
			scnt++;
		}
		if (scnt < IFNAMSIZ) {
			*dest++ = '>';
			scnt++;
		}
	}

	/*
	 * Now add the rest of the interface name onto the end after the
	 * "<node>" part of the construct.
	 */
	cp = ifname;
	while (scnt < IFNAMSIZ && *cp) {
		*dest++ = *cp++;
		scnt++;
	}
	if (scnt < IFNAMSIZ) {
		*dest++ = '\0';
		scnt++;
	}

	return;

}
#endif

#endif	/* TABLE_NETWORK */



