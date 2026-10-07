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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This source file was modified by the Center for High Performance
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_port.c,v $
 * Revision 1.31  1995/03/10  21:37:41  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: Augment panic message to help with #11204
 *
 * Revision 1.30  1995/03/02  19:43:05  stans
 *  Vnode caching support: pfs_host_init() syscall
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk:medium
 *  Benefit or PTS #:8129
 *  Testing:WW07 sats
 *
 * Revision 1.29  1995/02/01  22:21:34  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.28  1994/11/18  20:48:05  mtm
 * Copyright additions/changes
 *
 * Revision 1.27  1994/10/10  15:57:38  johannes
 * vnode_port_no_senders(): added "nms" wakeup needed in case of second NMS msg
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 10487
 *  Testing: special test case
 *  Module(s): svr/server/uxkern/fsvr_port.c
 *
 * Revision 1.26  1994/05/17  08:12:27  yazz
 * Corrected typo in r1.25 comments.
 *
 * Revision 1.25  1994/05/16  20:44:21  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: #8017
 *  Testing: No testcase; by code inspection.
 *  Module(s): server/uxkern/fsvr_port.c
 * In fsvr_notify(), take into account improbable cases where the portname
 * relevant to the notification could be MACH_PORT_NULL or MACH_PORT_DEAD
 * instead of just dereferencing those values (0 or -1) like valid pointers.
 *
 * Revision 1.24  1994/05/04  22:00:48  mjl
 * TNC select rewrite.  There is no longer a separate TNC deadname handler.
 *
 *  Reviewer: Charlie Johnson (Intel), Bob Yasi (Locus)
 *  Risk: Medium
 *  Benefit or PTS #: #7537 + select rewrite
 *  Testing: VSX, EATS, bobtest, Eval
 *  Module(s):
 * 	server/bsd/subr_select.c
 * 	server/sys/select.h
 * 	server/sys/socketvar.h
 * 	server/sys/user.h
 * 	server/tnc/un_debug.c
 * 	server/tnc/un_debug.h
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/bsd_server_side.c
 * 	server/uxkern/fsvr.defs
 * 	server/uxkern/fsvr2_server_side.c
 * 	server/uxkern/fsvr_port.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/port_hash.c
 * 	server/uxkern/port_hash.h
 * 	server/vsocket/mi_config.c
 * 	server/vsocket/sys_vsocket.c
 * 	server/vsocket/two_way_hash.h
 * 	server/vsocket/vs.defs
 * 	server/vsocket/vs_chouse.c
 * 	server/vsocket/vs_debug.c
 * 	server/vsocket/vs_init.c
 * 	server/vsocket/vs_ipc.c
 * 	server/vsocket/vs_netops.c
 * 	server/vsocket/vs_subr.c
 * 	server/vsocket/vs_subr.h
 * 	server/vsocket/vs_types.h
 * 	server/vsocket/vsocket.h
 *
 * Revision 1.23  1994/04/05  14:44:38  cfj
 * Merge revision 1.21.2.1 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.22  1994/01/12  17:46:46  jlitvin
 * Checked in some preliminary changes to make lint happier.
 *
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: Reduce lint complaints.
 *  Testing: compiled server
 *  Module(s):
 * 	uxkern/vm_unix.c
 * 	uxkern/ux_server_loop.c
 * 	uxkern/tty_io.c
 * 	uxkern/syscall.c
 * 	uxkern/server_init.c
 * 	uxkern/raw_hippi.c
 * 	uxkern/misc.c
 * 	uxkern/mf.c
 * 	uxkern/inittodr.c
 * 	uxkern/hippi_io.c
 * 	uxkern/fsvr_subr.c
 * 	uxkern/fsvr_server_side.c
 * 	uxkern/fsvr_rmtspec_ops.c
 * 	uxkern/fsvr_port.c
 * 	uxkern/fsvr_msg.c
 * 	uxkern/ether_io.c
 * 	uxkern/disk_io.c
 * 	uxkern/device_reply_hdlr.c
 * 	uxkern/credentials.c
 * 	uxkern/cons.c
 * 	uxkern/bsd_server_side.c
 * 	uxkern/boot_config.c
 * 	uxkern/block_io.c
 * 	uxkern/rpm_clock.c
 * 	i386/conf.c
 * 	i860/conf.c
 *
 * Revision 1.21.2.1  1994/04/05  14:39:30  cfj
 * Call SDRP_TO_USER_DRP_LOOKUP() in fsvr_notify() to determine if the port
 * which received the send-once notification was a sdrp.  If so just
 * return TRUE. (i.e. it was not a file port.)
 *
 *  Reviewer:yazz@locus.com,jlitvin
 *  Risk:L
 *  Benefit or PTS #:7616
 *  Testing:testcase,VSX EAT
 *  Module(s):	server/vsocket/sys_vsocket.c
 * 		server/uxkern/fsvr_port.c
 *
 * Revision 1.21  1993/10/28  03:15:12  yazz
 * Augment panic() message to include affected port name.
 *
 * Revision 1.20  1993/10/06  19:51:01  cfj
 * In dev_node_lookup(), always return phys_node if the search of DEV_TAB
 * fails.
 *
 * Revision 1.19  1993/09/01  01:38:34  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.18  1993/08/04  03:54:58  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.17  1993/07/29  21:54:34  cfj
 * 07-29-93 Locus code drop to fix select() and multiple network
 * server slowdown.
 *
 * Revision 1.16  1993/07/14  18:41:54  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.7  1993/07/09  15:06:53  cfj
 * 07-08-93 Locus bug fix drop for select().
 *
 * Revision 1.1.1.6  1993/07/01  21:02:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.15  1993/05/18  03:20:38  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.14  1993/05/07  18:32:28  nandy
 * Fixed a merge conflict
 *
 * Revision 1.13  1993/05/06  19:28:49  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:52:10  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.6.4.5  1993/04/04  01:19:44  cfj
 * Yet another select fix. (bhk@locus.com)
 *
 * Revision 1.6.4.4  1993/03/24  23:44:26  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 *     Revision 2.31  93/03/19  17:25:32  bhk
 *     incremented the sequence numbers for tnc selects
 * 
 *     Revision 2.30  93/03/10  14:20:25  yazz
 *     Synchronous close merge from Intel.
 * 
 * Revision 1.6.4.3  1993/03/10  16:54:08  nandy
 * Merged from the Main Stem.
 *
 * Revision 1.9  1993/03/10  16:44:55  nandy
 * file_port_increment_seqno() calls FP_LOCK only if the lock is not held.
 *
 * Revision 1.6.4.2  1993/02/18  01:45:26  cfj
 * Complete select() fix from Locus.
 *
 * Revision 1.8  1993/02/18  01:18:25  cfj
 * Complete select() fix from Locus.
 *
 * Revision 1.11  1993/04/03  03:11:51  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.10  1993/03/25  23:31:08  cfj
 * T9 Merge.
 *
 * Revision 1.9  1993/03/10  16:44:55  nandy
 * file_port_increment_seqno() calls FP_LOCK only if the lock is not held.
 *
 * Revision 1.6.4.2  1993/02/18  01:45:26  cfj
 * Complete select() fix from Locus.
 *
 * Revision 1.8  1993/02/18  01:18:25  cfj
 * Complete select() fix from Locus.
 *
 * Revision 1.7  1993/02/16  21:07:27  cfj
 * Merge sync close into main stem.
 *
 * Revision 1.6.4.1  1993/02/16  20:38:55  cfj
 * Synchronous close from OSF.
 *
 * Revision 1.1.2.2.2.4  1993/02/16  20:08:16  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.6  1993/02/03  15:49:52  cfj
 * Fixed up the RCS log comment.
 *
 * Revision 1.5  1993/02/03  01:38:05  cfj
 * Partial select fix from bhk@locus.com.
 *
 *   Revision 2.27  93/02/02  12:14:13  bhk
 *   Added file_port_increment_seqno fix from OSF (2.31) to allow
 *   sockets to close when selects are posted
 *      Revision 2.31  93/01/29  15:06:40  durriya
 *              call file_port_increment_seqno() if a dead-name or send-once
 *              notification is received on a file port.   (durriya)
 *
 * Revision 1.1.2.2.2.3  1992/12/16  22:56:25  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.2.2.2  1992/12/16  06:05:09  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.2.2.1  1992/12/14  23:24:14  brad
 * Merged tip of old NX branch with PFS branch.
 *
 *
 * Revision 1.4  1992/12/11  03:06:02  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:54:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/20  17:54:09  cfj
 * In node_to_fileserver_port() and node_to_master_device_port() put a
 * call to is_node_valid() to double check that the node is in the
 * BOOT_NODE_LIST.
 *
 * Revision 1.1.2.2  1992/11/06  20:34:21  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:43:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.43  93/08/16  18:46:28  bhk
 * Added a check to see if selqdebug was enabled before printing information on
 * held server references in fp_unref_port
 * 
 * Revision 2.42  93/08/11  08:58:05  bhk
 * Removed long useless code path from select.
 * Bug # 336
 * 
 * Revision 2.41  93/07/30  14:08:53  yazz
 * Augmented panic message to include port name.
 * 
 * Revision 2.40  93/07/27  11:39:29  mjl
 * [Bug #0308] In file_port_no_senders(), use FP_SEQNO_{UN,}LOCK()
 * macros to acquire/release shared TNC FIFO seqno r/w lock.  Likewise
 * in file_port_increment_seqno().  Also, tnc_fsvr_end_op() is now split
 * into separate tnc_should_relocate() and tnc_relocate_try() routines.
 * 
 * Revision 2.39  93/07/07  16:41:53  bhk
 * Unwind the stack if a deadlock was detected in socket and FIFO migration
 * and retry the operation.
 * 
 * Revision 2.38  93/06/29  15:39:36  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.37  93/06/14  14:15:09  paul
 * Fixes bug 0278 - Pipe/Socket relocation under TNC
 * Reinstalled some code which was lost as part of merge with ad1.0.3.
 * This was where a #if 0 was removed which was probably not right, since it
 * was being used to temporarilly turn off some code which was going to be
 * turned back on when pipe relocation was re-enabled...
 * 
 * Revision 2.36  93/05/05  16:38:22  bolsen
 * [Bug 239] added a new is_node_valid() function.
 * 
 * Revision 2.35  93/04/29  14:05:17  klh
 * 	Revision 2.34  93/03/25  10:02:04  durriya
 * 		don't panic in node_to_fileserver_port if an invalid node is
 * 		specified return MACH_PORT_NULL instead            (durriya)
 *
 * 	Revision 2.33  93/02/04  12:15:44  durriya
 * 		fix 860 compilation error in mmp's fix below       (durriya)
 *
 * 	Revision 2.32  93/02/03  15:23:37  mmp
 * 		Make sure port is valid before looking at f_magic in
 * 		fsvr_notify.  Get rid of annoying printf in
 * 		file_port_no_senders.
 *
 * 	Revision 2.31  93/01/29  15:06:40  durriya
 * 		call file_port_increment_seqno() if a dead-name or send-once
 * 		notification is received on a file port.                    (durriya)
 *
 * 	Revision 2.30  1993/01/15  19:59:04  loverso
 * 		dev_tab_fun() uses new boot config parsing parameters.
 *
 * 	Revision 2.29  1993/01/11  14:38:19  mmp
 * 		Synchronous close support: keep track of file port send rights;
 * 		file_port_no_senders releases one file ref for each outstanding
 * 		send right.  (mmp)
 *
 * Revision 2.34  93/04/03  11:49:04  klh
 * vs_select_deadname no longer arrives on a file port.
 * 
 * Revision 2.33  93/03/23  19:52:40  yazz
 * Verified a port/struct ptr is valid before dereferencing.
 * 
 * Revision 2.32  93/03/22  21:18:10  yazz
 * Validate port names in dead name notification messages before dereferencing
 * them as pointers to data structures (use MACH_PORT_VALID).
 * 
 * Revision 2.31  93/03/19  17:25:32  bhk
 * incremented the sequence numbers for tnc selects
 * 
 * Revision 2.30  93/03/10  14:20:25  yazz
 * Synchronous close merge from Intel.
 * 
 * 	Revision 1.6.4.2  1993/02/18  01:45:26  cfj
 * 	Complete select() fix from Locus.
 *
 * 	Revision 1.8  1993/02/18  01:18:25  cfj
 * 	Complete select() fix from Locus.
 *
 * Revision 2.29  93/02/10  17:07:12  klh
 * Fix RCS comments
 * 
 * Revision 2.28  93/02/08  16:51:44  bhk
 * added a call to vs_select_deadname to cleanup on canceled selects
 * 	Revision 1.7  1993/02/16  21:07:27  cfj
 * 	Merge sync close into main stem.
 *
 * 	Revision 1.6.4.1  1993/02/16  20:38:55  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 1.6  1993/02/03  15:49:52  cfj
 * 	Fixed up the RCS log comment.
 *
 * 	Revision 1.5  1993/02/03  01:38:05  cfj
 * 	Partial select fix from bhk@locus.com.
 *
 * 
 * Revision 2.27  93/02/02  12:14:13  bhk
 * Added file_port_increment_seqno fix from OSF (2.31) to allow sockets to close
 * when selects are posted
 * 
 * Revision 2.26  92/11/23  16:05:50  klh
 *      Revision 2.31  93/01/29  15:06:40  durriya
 *              call file_port_increment_seqno() if a dead-name or send-once
 *              notification is received on a file port.   (durriya)
 *
 * 	Revision 1.4  1992/12/11  03:06:02  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.3  1992/11/30  22:54:32  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.3  1992/11/20  17:54:09  cfj
 * 	In node_to_fileserver_port() and node_to_master_device_port() put a
 * 	call to is_node_valid() to double check that the node is in the
 * 	BOOT_NODE_LIST.
 *
 * 	Revision 1.1.2.2  1992/11/06  20:34:21  dleslie
 * 	Merged bug drop from Locus November 3, 1992, with NX development
 *
 * 	Revision 1.1.2.1  1992/11/05  23:43:18  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * 	Revision 2.27  92/11/17  19:53:11  loverso
 * 		Rename dev_tab_lookup() to dev_node_lookup() and have it always
 * 		return a valid node.  Change vnode_to_fileserver_node() to just
 * 		call dev_node_lookup() with v_devnode.  (mmp)
 * 
 * 	Revision 2.26  92/11/11  10:40:22  rabii
 * 		Made node_to_fileserver_port not retry to get a fileserver port. The
 * 		caller is now responsible to retry. (rabii)
 * 		Added debug_node support to node_to_fileserver_port.
 * 		[92/10/07            roy]
 * 
 * 	Revision 2.24  92/11/05  17:27:58  roy
 * 		Added mscount to token_port_no_senders call.
 * 		[92/10/30            roy]
 * 
 * 	Revision 2.23  92/11/03  11:55:03  mmp
 * 		Change vnode_port_no_senders() to wait until v_seqno catches up to
 * 		or passes the NMS seqno, instead of waiting until they're equal.
 * 		Also increment v_seqno to count the NMS message in case of a race
 * 		with new send rights.  (mmp)
 * 
 * Revision 2.25  92/11/23  10:48:20  klh
 * Add file_port_increment_seqno() and file_port_no_senders() to fix nms race.
 * (klh for mjl).
 * 
 * Revision 2.24  92/10/09  12:27:33  roman
 * Fix the retry logic to correctly wait for norma_get_nameserver_port()
 * 	to succeed.
 * 
 * Revision 2.23  92/10/06  12:22:06  roman
 * Fix RCS comments.
 * 
 * Revision 2.22  92/10/05  14:00:50  klh
 * 	Revision 2.22  92/09/29  16:48:47  rabii
 * 		node_to_fileserver_port() should NOT panic on bogus node 
 *		numbers. (loverso)
 * 
 * 	Revision 2.21  92/09/17  13:43:22  rabii
 * 		Modified node_to_fileserver_port to retry when attempting to 
 *		get nameserver and fileserver ports, and to panic in case 
 *		of timeout (rabii)
 * 
 * 	Revision 2.20  92/09/11  09:29:32  rabii
 * 		Added new routines dev_tab_fun and dev_tab_lookup to build and
 *		access table to map physical device node into logical device 
 *		node (node where the server is running) using boot config 
 *		info. Also modified the routine vnode_to_fileserver_node to 
 *		use dev_tab_lookup as a part of its translation (rabii)
 * 
 * 	Revision 2.19  92/08/13  19:21:22  rabii
 * 		Added back rev 2.17 rcs comments. (rabii)
 * 
 * Revision 2.21  92/09/28  13:34:17  klh
 * Add TNC hook, called on vnode port deallocation. (klh for mjl).
 * 
 * Revision 2.20  92/08/25  14:53:07  chrisp
 * Fix for bug #49: for TNC, remove assertion in get_vnode_port() since it
 * certainly fails for the root directory port on the i860.
 * 
 * Revision 2.19  92/08/17  13:25:37  mjl
 * In get_vnode_port(), don't call ux_server_add_port() if the vnode is
 * relocating.
 * 
 * Revision 2.18  92/07/14  15:00:29  rabii
 * 	Backed out last revision (rabii)
 *
 * Revision 2.17  92/07/14  15:00:29  rabii
 *	No changes.
 * 
 * Revision 2.16  92/05/31  18:59:45  loverso
 * 	Revision 2.14.1.2  92/05/26  15:55:10  loverso
 * 	send-once notifications will be generated because of removing the
 * 	select delay port dead-name notification.  Also, remove the last
 * 	uref manually for a dead-name, as mach_msg_destroy won't.
 * 
 * 	Revision 2.14.1.1  92/05/25  21:59:19  loverso
 * 	Clean up dead-name handling.
 * 
 * Revision 2.15  92/05/24  13:55:37  pjg
 * 	Renamed v_node to v_devnode.
 *
 * Revision 2.14  92/05/18  12:27:28  roy
 * 	Revision 2.6.1.2  92/05/08  12:06:02  roy
 * 	Moved token_port_* routines to the mapped files module (mf.c).
 * 	Added port_to_node().
 * 	[92/04/23            roy]
 * 
 * 	Revision 2.6.1.1  92/04/22  10:02:40  roy
 * 	Assert that ports become invalid names when their receive rights
 * 	are deallocated.  Print error codes when port allocation fails.
 * 	[92/03/31            roy]
 * 
 * Revision 2.13  92/05/14  15:05:54  loverso
 * 	Re-order tests in fsvr_notify to avoid checking "magic numbers" on
 * 	unrenamed ports.
 * 
 * Revision 2.12  92/05/13  15:07:49  loverso
 * 	Remove debugging printf.
 * 
 * Revision 2.11  92/05/13  14:09:20  loverso
 * 	Cleanup panics on notifications we don't expect.
 * 
 * Revision 2.10  92/05/12  00:03:29  loverso
 * 	Rename fsvr_nosenders_notify() to fsvr_notify() and add various
 * 	notification handling.  (loverso)
 * 
 * Revision 2.9  92/05/01  10:01:49  rabii
 * 	Added the new routine node_to_master_device_port used by device
 * 	opening routines to find the master port for the node to which
 * 	the actual device is attached. (rabii)
 * 
 * 	Also added the changes requested by Intel to vnode_to_filesever 
 * 	port. (rabii)
 * 
 * Revision 2.8  92/04/05  17:08:40  pjg
 * 	Fix node_to_fileserver_port.
 * 
 * Revision 2.7  92/03/20  11:32:30  pjg
 * 	Moved here all the routines that convert a {vnode, node, port} to a
 * 	fileserver {node,port}. Renamed the routines to 
 * 	{vnode,node,port}_to_fileserver_{node,port}. 
 * 	The port returned by these routines should NOT be deallocated.
 * 
 * Revision 2.6  92/03/15  14:30:04  roy
 * 	92/02/19  10:43:30  roy
 * 	Don't bother checking address range of ports in no-senders
 * 	notify routine (checking magic cookie is sufficient).  Upgrade
 * 	MAPPED_FILES support.
 * 
 * Revision 2.5  92/03/01  18:33:09  pjg
 * 	Changed get_vnode_port to increment the ref count on the vnode
 * 	iif it allocates a port; don't vrele the vnode in get_vnode_port. 
 * 	This reference counts for all the outstanding send rights to the port
 * 	and is deallocated in vnode_port_deallocate when a no-more-senders is 
 * 	received on the port.
 * 	Initialized the vnode message sequence number on 0 and test for
 * 	equality when the no-more-senders arrives.
 * 	Moved bsd_get_bootstrap_directories to syscall.c
 * 
 * 	92/02/28  16:56:27  noemi
 * 	Added mount_port_allocate and mount_port_deallocate.  Fixed a few
 * 	comments.
 * 
 * Revision 2.4  92/02/21  16:42:46  durriya
 *    don't include <norma_ipc.h>
 *
 * Revision 2.3  92/01/09  16:02:07  roy
 * 	Remove extra vrele in vnode no-senders logic.
 * 
 * Revision 2.2  92/01/05  20:20:14  roy
 * 	92/01/02  14:31:17  roy
 * 	Twiddle fsvr_nosenders_notify to fix file no-senders and work with new
 * 	interface to start_nosenders_op.  file_port_no_senders no longer 
 * 	exists.  Also removed f_seqno from file structure (not needed).
 * 
 * 	91/09/22  21:53:58  noemi
 * 	Initial revision.  Created from defunct file_to_port.c.
 * 	Changed file port allocation and deallocation to use the magic number 
 * 	and sequence number in the file structure.  Replaced 
 * 	vnode_port_allocate with the get_vnode_port function.  Changed vnode 
 * 	port deallocate to reset vnode's magic number, sequence number and 
 * 	make send count.  Rewrote no-more-senders notificatiion scheme.  
 * 	Added file_port_no_senders and vnode_port_no_senders functions.
 * 
 * $EndLog$
 */

#include <norma_ipc.h>
#include <mapped_files.h>
#include <sys/time.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/specdev.h>
#include <sys/mount.h>
#include <sys/proc.h>
#include <sys/select.h>
#include <mach/mach_port.h>
#include <mach/message.h>
#include <mach/notify.h>
#include <mach/norma_special_ports.h>
#include <kern/queue.h>
#include <uxkern/syscall_subr.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/port_hash.h>

#define SELQDEBUG MACH_ASSERT

#ifndef SELQDEBUG
#define SELQDEBUG(s)
#else
#undef SELQDEBUG
#define SELQDEBUG(s) if (selqdebug) s
extern int selqdebug;
#endif

#if     MAPPED_FILES | PFS
#include <uxkern/mf.h>
#endif	MAPPED_FILES | PFS

#ifdef	TNC
#include <tnc/un_ff.h>
#endif

extern mach_port_t 		privileged_host_port;
/*
 * The following macros should only be used in this file.
 */
#define	PORT_TO_STRUCTURE(port,sp)				\
	sp = ((void *)port);

#define	VNODE_TO_PORT_LOOKUP(vp, port)				\
	port = ((mach_port_t)vp)

/*
 * Allocate a file structure port and associate it with its associated
 * file structure.  Mark the file structure to indicate that it has
 * a file structure port attached and add the port to the server's port set.
 */
void
file_port_allocate(fp)
	struct file	*fp;
{
	mach_port_t	port, previous;
	int		error;

	/*
	 * Allocate a port for file structure. 
	 */
	port = (mach_port_t) fp;
	error = mach_port_allocate_name(mach_task_self(),
		MACH_PORT_RIGHT_RECEIVE, port);
	if (error != KERN_SUCCESS) 
		panic("file_port_allocate: can't alloc file port "
				"ret=0x%x name=0x%x", error, port);

	/*
	 * Arrange for a 'no-more-senders' notification 
	 */
	error = mach_port_request_notification(mach_task_self(), port,
			MACH_NOTIFY_NO_SENDERS, 1, port,
			MACH_MSG_TYPE_MAKE_SEND_ONCE, &previous);
	if (error != KERN_SUCCESS || previous != MACH_PORT_NULL)
		panic("file_port_allocate: req notification failed (0x%x)",
		      error);
	/*
	 * Allocate a send right to send back to requester.
	 */
	error = mach_port_insert_right(mach_task_self(),
		port, port, MACH_MSG_TYPE_MAKE_SEND);
	if (error != KERN_SUCCESS)
		panic("file_port_allocate: can't acquire send rights (0x%x)",
		      error);

	/*
	 * Set the magic number in the file structure to indicate that
	 * this structure has an attached port.
	 */
	fp->f_magic = F_MAGIC;

	/*
	 * initialize number of send rights created and released
	 */
	fp->f_makesend = 1;
	fp->f_relsend = 0;
	fp->f_svrsend = 0;

#if     MAPPED_FILES
	/*
	 * No memory object has yet been allocated for the server to
	 * perform local map operations.
	 */
	fp->mem_obj = MACH_PORT_NULL;
#endif  /* MAPPED_FILES */

	/*
	 * Tell the server to service the port.
	 */
	ux_server_add_port(port);
}

/*
 * Remove a file structure port from the server's port set and destroy it.
 */
void
file_port_deallocate(fp)
	struct file 	        *fp;
{
	mach_port_t	        port;
        mach_port_type_t        ptype;
	int		        error;

	ASSERT(fp->f_magic == F_MAGIC);
	FILE_TO_PORT_LOOKUP(fp, port);
	ux_server_remove_port(port);


	/* error = mach_port_mod_refs(mach_task_self(), port,
                                   MACH_PORT_RIGHT_RECEIVE, -1);
	if (error != KERN_SUCCESS) {
		panic("file_port_deallocate: can't dealloc rcv rt=0x%x kr=0x%x",
				port, error);
	}
        ASSERT(mach_port_type(mach_task_self(), port, &ptype) == 
               KERN_INVALID_NAME);
	 */

	error = mach_port_destroy(mach_task_self(), port);
	if (error != KERN_SUCCESS) {
		panic("file_port_deallocate: can't destroy port=0x%x kr=0x%x",
				port, error);
	}

	/*
	 * Reset the magic number for the file structure to indicate
	 * it no longer has an attached port.
	 */
	fp->f_magic = 0;

	/* reset send counts */
	fp->f_makesend = 0;
	fp->f_relsend = 0;

#if     MAPPED_FILES
	if (fp->mem_obj != MACH_PORT_NULL) {
		/*
		 * A memory object was obtained by the server itself
		 * to perform mapping operations on behalf of clients.
		 */
		inode_pager_release(fp->mem_obj);
		fp->mem_obj = MACH_PORT_NULL;
	}
#endif  /* MAPPED_FILES */

}


/*
 * Deal with no-more-senders notification on a file port.
 *
 * Decrements reference count for file after waiting for outstanding
 * messages on the file port to complete.
 */
void
file_port_no_senders(
	struct file		*fp,
	mach_port_seqno_t	seqno)
{
	int numrights;

#ifndef	TNC
	FP_LOCK(fp);
	while (seqno > fp->f_seqno) {
		fp->f_flag |= FNMSWAIT;
		assert_wait((int)&fp->f_seqno, FALSE);
		FP_UNLOCK(fp);
		thread_block();
		FP_LOCK(fp);
	}

	/* Get rid all external (on behalf of emulator) references. */
	numrights = fp->f_makesend - fp->f_relsend;
	fp->f_relsend = fp->f_makesend - 1;
	FP_UNLOCK(fp);
#else	/* TNC */
	FP_SEQNO_LOCK(fp);
	FP_LOCK(fp);
	while (seqno > fp->f_seqno) {
		fp->f_flag |= FNMSWAIT;
		assert_wait((int)&fp->f_seqno, FALSE);
		FP_UNLOCK(fp);
		FP_SEQNO_UNLOCK(fp);
		thread_block();
		FP_SEQNO_LOCK(fp);
		FP_LOCK(fp);
	}

	/* Get rid all external (on behalf of emulator) references. */
	numrights = fp->f_makesend - fp->f_relsend;
	fp->f_relsend = fp->f_makesend - 1;
	FP_UNLOCK(fp);
	FP_SEQNO_UNLOCK(fp);
#endif	/* TNC */

	if (numrights > 0)
		FP_UNREF_NUM(fp, numrights);
}


/*
 * Routine that tracks sequence numbers of file port operations. Needed
 * so that no-more-senders can track completed operations.
 *
 * *MUST* be called at the end of every file port operation to
 * increment the server-maintained port sequence number.  If the port
 * has received a no-more-senders message, wakeup the waiting nms
 * processing thread.
 */
void
file_port_increment_seqno(
	struct file	*fp)
{
	int do_wakeup = 0;

#ifndef	TNC
	FP_LOCK(fp);
	fp->f_seqno++;
	if (fp->f_flag & FNMSWAIT) {
		fp->f_flag &= ~FNMSWAIT;
		do_wakeup++;
	}
	FP_UNLOCK(fp);

	if (do_wakeup)
		thread_wakeup((int)&fp->f_seqno);
#else	/* TNC */
	int do_relocate = 0;

	FP_SEQNO_LOCK(fp);
	FP_LOCK(fp);
	fp->f_seqno++;
	if (fp->f_flag & FNMSWAIT) {
		fp->f_flag &= ~FNMSWAIT;
		do_wakeup++;
	} else if (tnc_should_relocate(fp)) {
		do_relocate++;
	}
	FP_UNLOCK(fp);
	FP_SEQNO_UNLOCK(fp);

	if (do_wakeup) {
		thread_wakeup((int)&fp->f_seqno);
	} else if (do_relocate) {
		(void) tnc_relocate_try(fp);
	}

#endif	/* TNC */
}


/*
 * Get another reference on the file structure to account for a new
 * send right created by the emulator.
 */
void
fp_ref_port(fp)
	struct file *fp;
{
	FP_LOCK(fp);
	fp->f_count++;
	fp->f_makesend++;
	FP_UNLOCK(fp);
}

/*
 * Get another reference on the file structure to account for a new
 * send right created for the server.
 */
void
fp_ref_port_svr(fp)
	struct file *fp;
{
	FP_LOCK(fp);
	fp->f_count++;
	fp->f_svrsend++;
	FP_UNLOCK(fp);
}

/*
 * Remove n file references and one send right from the file struct/port.
 * The last send right is not explicitly removed, but it will disappear
 * when closef calls file_port_deallocate.  This is done to avoid getting
 * a no more senders on the file port.
 *
 * Note that fp_unref_port subsumes the functionality provided by FP_UNREF 
 * in that it provides a way to release file struct reference(s) while also
 * handling file port references.
 */
fp_unref_port(fp, n)
	struct file *fp;
	int n;			/* number of file references to remove */
{
	int		error = 0;
	mach_port_t	port;
	boolean_t	needwakeup = FALSE;
	boolean_t	svrright = FALSE;

	FP_LOCK(fp);
	if (n < 0) {
		/* zzz ASSERT(fp->f_svrsend > 0); */
		if (fp->f_svrsend < 1) {
			struct uthread	*uth = current_thread();
#ifdef TNC
			panic("fp_unref_port: svrref: fp=0x%x selflags=0x%x\n",
			      fp, uth->uu_sel_flags);
#else
			panic("fp_unref_port: svrref: fp=0x%x\n", fp);
#endif /* TNC */
		}
		svrright = TRUE;
		n = (-n);
	}

	ASSERT(fp->f_makesend > fp->f_relsend);
	if (fp->f_makesend > fp->f_relsend + 1 || svrright) {
		/* This is not the last send right on the port. */
		ASSERT(fp->f_count > n);
		fp->f_count -= n;
		if (svrright)
			fp->f_svrsend--;
		else
			fp->f_relsend++;
		FILE_TO_PORT_LOOKUP(fp, port);

		error = mach_port_mod_refs(mach_task_self(), port,
						MACH_PORT_RIGHT_SEND, -1);
		if (error != KERN_SUCCESS)
			panic("fp_unref_port: m_p_mod_refs: port=0x%x kr=0x%x",
			      port, error);

		if (fp->f_flag & FUNREFWAIT) {
			fp->f_flag &= ~FUNREFWAIT;
			needwakeup = TRUE;
		}
		FP_UNLOCK(fp);
		if (needwakeup)
			thread_wakeup((int)&fp->f_count);
	} else {
		/* This is the last send right to be deallocated. */
		ASSERT(fp->f_count >= n);
		if (fp->f_count > n) {
			/*
			 * This is the last send right, but there are other
			 * file struct references (e.g., due to tokens).
			 * Wait for them to disappear, then close the file.
			 */
			while (fp->f_count != n) {
#ifdef SELQDEBUG
if (selqdebug && (fp->f_svrsend > 0))
	printf("fp %x svrsend %d makesend %d relsend %d f_count %d n %d\n",
		fp, fp->f_svrsend, fp->f_makesend, fp->f_relsend,
		fp->f_count, n);
#endif
				fp->f_flag |= FUNREFWAIT;
				assert_wait((int)&fp->f_count, FALSE);
				FP_UNLOCK(fp);
				thread_block();
				FP_LOCK(fp);
			}
		}
		ASSERT(fp->f_svrsend == 0);
		/*
		 * The only file struct references left are the ones we
		 * are giving up.  We also know it's not possible to create
		 * a new send right to this file struct.
		 */
		ASSERT(fp->f_makesend == fp->f_relsend + 1);
		fp->f_count = 1;
		FP_UNLOCK(fp);
		error = closef(fp);
	}
	return(error);
}


/*
 * Return the attached port for a vnode, allocating a new one, if necessary.
 * Increment the reference count on the vnode if a port is allocated.
 * This counts for all the outstanding send rights to the vnode and is
 * decremented when there are no more senders to the port 
 * in vnode_port_no_senders().
 */

void
get_vnode_port(vp, portp)
	struct vnode 	*vp;
	mach_port_t 	*portp;
{
	int error;
	int wakeup = 0;
	mach_port_t port, previous;
	
	*portp = MACH_PORT_NULL;
	VN_LOCK(vp);
	/*
	 * Wait until the port lock is available and acquire it.
	 */
	while (vp->v_flag & VPORTLOCK) {
		vp->v_flag |= VPORTWAIT;
		assert_wait((int)&vp->v_mscount, FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}
	vp->v_flag |= VPORTLOCK;
	if (vp->v_magic == V_MAGIC) {
		/* 
		 * If the vnode already has an attached port, create
		 * a send right and increment the make send count in
		 * the vnode.
		 */
		VN_UNLOCK(vp);
		VNODE_TO_PORT_LOOKUP(vp, port);
		ASSERT(port != MACH_PORT_NULL);
		error = mach_port_insert_right(mach_task_self(), port,
			port, MACH_MSG_TYPE_MAKE_SEND);
		if (error != KERN_SUCCESS)
			panic("get_vnode_port: can't create send right (0x%x)",
			      error);
		*portp = port;
		VN_LOCK(vp);
		vp->v_mscount++;
		goto out;
	}
	/*
	 * The vnode doesn't have an attached port.  Allocate a new one,
	 * add it to the server's port set, make a send right for the client
	 * and return the port.
	 */
	VN_UNLOCK(vp);
	port = (mach_port_t) vp;
	error = mach_port_allocate_name(mach_task_self(),
		MACH_PORT_RIGHT_RECEIVE, port);
	if (error != KERN_SUCCESS)
		panic("get_vnode_port: can't alloc vnode port "
				"err=0x%x name=0x%x", error, port);

	/*
	 * Arrange for a 'no-more-senders' notification
	 */
	error = mach_port_request_notification(mach_task_self(), port,
			MACH_NOTIFY_NO_SENDERS, 1, port,
			MACH_MSG_TYPE_MAKE_SEND_ONCE, &previous);
	if (error != KERN_SUCCESS || previous != MACH_PORT_NULL)
		panic("get_vnode_port: can't request notify (0x%x)", error);

	ASSERT(previous == MACH_PORT_NULL);
	/*
	 * Allocate a send right to send back to requester.
	 */
	error = mach_port_insert_right(mach_task_self(),
		port, port, MACH_MSG_TYPE_MAKE_SEND);
	if (error != KERN_SUCCESS)
		panic("get_vnode_port: can't create send right2 (0x%x)",
		      error);
	*portp = port;
	/*
	 * Increment the reference count on the vnode
	 */
	VREF(vp);
	/*
	 * Set the magic number in the vnode to indicate that it has an
	 * attached port.
	 */
	VN_LOCK(vp);
	vp->v_mscount++;
	vp->v_magic = V_MAGIC;
out:
	wakeup = (vp->v_flag & VPORTWAIT);
	vp->v_flag &= ~(VPORTWAIT|VPORTLOCK);
	VN_UNLOCK(vp);
	if (wakeup)
		thread_wakeup((int)&vp->v_mscount);
	/*
	 * Tell the server to service the port 
	 */
#ifdef	TNC
	VN_LOCK(vp);
	if ( ! (vp->v_flag & VRELOCATING) )
#endif
	ux_server_add_port(port);
#ifdef	TNC
	VN_UNLOCK(vp);
#endif
}


/*
 * Remove a vnode port from the server's port set and destroy it.
 */
void
vnode_port_deallocate(vp)
	struct vnode 	        *vp;
{
	mach_port_t	        port;
        mach_port_type_t        ptype;
	int		        error;

	ASSERT(vp->v_magic == V_MAGIC);
	VNODE_TO_PORT_LOOKUP(vp, port);
	ux_server_remove_port(port);

	error = mach_port_mod_refs(mach_task_self(), port,
                                   MACH_PORT_RIGHT_RECEIVE, -1);
	if (error != KERN_SUCCESS)
		panic("vnode_port_deallocate: unable to destroy port (0x%x)",
		      error);
        ASSERT(mach_port_type(mach_task_self(), port, &ptype) == 
               KERN_INVALID_NAME);

	/*
	 * Reset the magic number in the vnode to indicate that the
	 * structure no longer has an attached port.
	 */
	VN_LOCK(vp);
	vp->v_magic = 0;
	vp->v_seqno = 0;
	vp->v_mscount = 0;
	VN_UNLOCK(vp);
	/*
	 * Decrement the reference count obtained with get_vnode_port
	 */
	vrele(vp);
}

void
vnode_port_no_senders(port, vp, mscount, seqno)
	mach_port_t 	port;
	struct vnode 	*vp;
	u_long		mscount;
	u_long		seqno;
{
	u_long		vmscount;
	int 		wakeup = 0, nms_wakeup = 0;
	int		error;

	/*
	 * Wait for any outstanding messages on this port to be
	 * received, by waiting until v_seqno catches up to the
	 * seqno that came with the NMS message.  If v_seqno passes
	 * seqno, then a new send right has been created since the
	 * NMS was sent, and this will be handled below.
	 *
	 * Note that message sequence numbers started at zero, while
	 * v_seqno is actually a count of messages received on the
	 * vnode port.  If there was a race and the port is not going
	 * to be deallocated, then v_seqno needs to be incremented
	 * to count this NMS message.
	 */
	VN_LOCK(vp);
	while (seqno > vp->v_seqno) {
		vp->v_flag |= VNMSWAIT;
		assert_wait((int)&vp->v_seqno, FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}

	/*
	 * Wait until the vnode port lock is available and acquire it.
	 * Then compare the vnode's make-send count with the make-send
	 * count in the no-more-senders message while holding the port
	 * lock.  This prevents the server from creating new send rights
	 * while the port is being destroyed.
	 */
	while (vp->v_flag & VPORTLOCK) {
		vp->v_flag |= VPORTWAIT;
		assert_wait((int)&vp->v_mscount, FALSE);
		VN_UNLOCK(vp);
		thread_block();
		VN_LOCK(vp);
	}
	vp->v_flag |= VPORTLOCK;
	vmscount = vp->v_mscount;
	VN_UNLOCK(vp);

	/*
 	 * If the make send count in the notification message is less
	 * than the make-send count in the vnode, the server has created
	 * a new send right since the no-more-senders notification was
	 * triggered.  In this case, request another no-more-senders
	 * notification.
	 */
	if (mscount < vmscount) {
		mach_port_t	previous;

		error = mach_port_request_notification(mach_task_self(), port,
			MACH_NOTIFY_NO_SENDERS, vmscount, port,
			MACH_MSG_TYPE_MAKE_SEND_ONCE, &previous);
		if (error != KERN_SUCCESS || previous != MACH_PORT_NULL)
			panic("vnode_port_no_senders: can't req notify (0x%x)",
			      error);
		VN_LOCK(vp);
		vp->v_seqno++;		/* count this NMS message */
		goto out;
	}
	
	ASSERT(seqno == vp->v_seqno);
	ASSERT(mscount == vmscount);
	/*
 	 * The two make-send counts are equal, so no new send rights have
	 * been created since the notification was sent, and all outstanding
	 * messages should have been received (sequence numbers are equal).
	 * Call vnode_port_deallocate to destroy the port and decrement 
	 * the reference count on the vnode.
	 */
	vnode_port_deallocate(vp);
#ifdef	TNC
	tnc_vnode_port_destroyed(vp);
#endif
	VN_LOCK(vp);
out:
	wakeup = (vp->v_flag & VPORTWAIT);
	nms_wakeup = (vp->v_flag & VNMSWAIT);
	vp->v_flag &= ~(VPORTWAIT|VPORTLOCK|VNMSWAIT);
	VN_UNLOCK(vp);
	if (wakeup)
		thread_wakeup((int)&vp->v_mscount);
	if (nms_wakeup)
		thread_wakeup((int)&vp->v_seqno);
}

/*
 * Allocate a mount structure port and insert it into the mount port
 * hash table.  Also, add it to the server's port set.
 */
void
mount_port_allocate(mp, portp)
	struct mount	*mp;
	mach_port_t	*portp;
{
	kern_return_t	error;

	error = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_RECEIVE,
			portp);
	if (error != KERN_SUCCESS) 
		panic("mount_port_allocate: can't allocate mount port (0x%x)",
                      error);

	/*
	 * Allocate a send right to send back to requester.
	 */
	error = mach_port_insert_right(mach_task_self(),
		*portp, *portp, MACH_MSG_TYPE_MAKE_SEND);
	if (error != KERN_SUCCESS)
		panic("mount_port_allocate: can't acquire send rights (0x%x)",
		      error);

	/*
	 * boost the port queue limit as we will be expecting higher than
	 * normal message traffic.
	 */

	error = mach_port_set_qlimit(mach_task_self(), *portp, 15);
	if (error != KERN_SUCCESS) {
		printf("mount_port_set_qlimit(port %x,lim 15) kr=%d\n",
			*portp, error);
	}

	/*
	 * Insert the port into the mount port hash table and save it
	 * in the mount structure.  This allows us to translate from
	 * the port to the mount structure and vice versa.
	 */
	MOUNT_LOCK(mp);
	mp->m_mountport = *portp;
	MOUNT_UNLOCK(mp);
	MOUNT_PORT_ENTER(*portp, mp);

	/*
	 * Tell the server to service the port.
	 */
	ux_server_add_port(*portp);
}


/* 
 * Remove a mount structure port from the server's port set and destroy it.
 */
void
mount_port_deallocate(mp)
	struct mount	        *mp;
{
	mach_port_t	        mountport, remoteport;
        mach_port_type_t        ptype;
	int		        error;

	/* 
	 * Attempt to obtain the mount structure port from the mount structure.
	 * Also attempt to locate the remote mount structure port.
	 */
	MOUNT_LOCK(mp);
	mountport = mp->m_mountport;
	remoteport = mp->m_remoteport;
	mp->m_remoteport = mp->m_mountport = MACH_PORT_NULL;
	MOUNT_UNLOCK(mp);

	if (mountport != MACH_PORT_NULL) {
		/*
		 * If the mount structure had an attached port,
		 * remove it from the server's port set and destroy it.
		 * It also should have a send right to a remote mount
		 * structure port.
		 */
		ux_server_remove_port(mountport);
		MOUNT_PORT_REMOVE(mountport);

                error = mach_port_mod_refs(mach_task_self(), mountport,
                                           MACH_PORT_RIGHT_RECEIVE, -1);
                if (error != KERN_SUCCESS)
			panic("mount_port_deallocate: unable to destroy (0x%x)",
                              error);
                ASSERT(mach_port_type(mach_task_self(), mountport, &ptype) == 
                       KERN_INVALID_NAME);
	}
	if (remoteport != MACH_PORT_NULL) {
		/*
		 * If the mount structure contains a send right to
		 * a remote mount structure port, deallocate it.
		 */
		mach_port_deallocate(mach_task_self(), remoteport);
	}
}

/*
 * Handle a notifications from the main server loop
 */
int
fsvr_notify(in, out)
	mach_msg_header_t		*in;
	mach_msg_header_t		*out;
{
	mach_no_senders_notification_t	*nms_msg = NULL;
	mach_dead_name_notification_t	*dn_msg = NULL;
	mach_send_once_notification_t	*so_msg = NULL;
	mach_port_deleted_notification_t	*pd_msg = NULL;
	mach_port_t			port = MACH_PORT_NULL;
	void				*sp, *lp;
	kern_return_t			ret;
#if	MAPPED_FILES | PFS
	token_info_t			*tip;
#endif	MAPPED_FILES | PFS

	/*
	 * Verify that the message is something we might handle
	 */
	if (in->msgh_bits != MACH_MSGH_BITS(0, MACH_MSG_TYPE_PORT_SEND_ONCE))
		return(FALSE);
	switch (in->msgh_id) {
	case MACH_NOTIFY_NO_SENDERS:
		nms_msg = (mach_no_senders_notification_t *)in;
		port = in->msgh_local_port;
		if(!MACH_PORT_VALID(port))
			return TRUE;	/* ignore M_P_DEAD & M_P_NULL */
		break;
	case MACH_NOTIFY_DEAD_NAME:
		dn_msg = (mach_dead_name_notification_t *)in;
		port = dn_msg->not_port;
		if(!MACH_PORT_VALID(port))
			return TRUE;	/* ignore M_P_DEAD & M_P_NULL */
		break;
	case MACH_NOTIFY_PORT_DELETED:
		pd_msg = (mach_port_deleted_notification_t *)in;
		port = pd_msg->not_port;
		/*
		 * We shouldn't get this, but we might.
		 * This happens when a send-once right with a dead-name
		 * notification is deallocated; the send-once right for the
		 * notification is used to generate this.
		 * The select code tries to prevent this.
		 *
		 * If we didn't call panic, we would just return TRUE as
		 * there is nothing to handle.
		 */
#ifdef TNC
		{ extern /*node_t*/ long this_node;
		printf("[node: %d] mach_port_deleted_notification 0x%x\n",
		       this_node, port);
		}
#else
		panic("mach_port_deleted_notification %x\n", port);
#endif
		return TRUE;
	case MACH_NOTIFY_SEND_ONCE:
		so_msg = (mach_send_once_notification_t *)in;
		/*
		 * You get one of these each time a send-once right is
		 * deallocated.  This may happen when select_cleanup
		 * frees the send-once right that would have been used
		 * to carry a dead-name notificaion.
		 */
		SELQDEBUG(printf("mach_send_once_notification r %x l %x\n",
			in->msgh_remote_port,
			in->msgh_local_port));
		/* 
		 * extract the port for which we have the notification. If
		 * it is a file port we have to increment the seqno in the
		 * file stucture so as to process the nms on file ports
		 * correctly
		 */
		PORT_TO_STRUCTURE((so_msg->not_header.msgh_local_port), lp);
		if (MACH_PORT_VALID(so_msg->not_header.msgh_local_port) &&
		    (((struct file *)lp)->f_magic == F_MAGIC)) 
			file_port_increment_seqno((struct file *)lp);

		return TRUE;
	default:
		return(FALSE);
	}

	/*
	 * Determine what type of port received the notification
	 */
	PORT_TO_STRUCTURE(port, sp);

	if (nms_msg && ((struct vnode *)sp)->v_magic == V_MAGIC) {
		start_notify_op(sp, 2008);  

		SELQDEBUG(printf("vnode no-senders %x\n",port);)
		vnode_port_no_senders(port, (struct vnode *)sp, 
		        nms_msg->not_count, nms_msg->not_header.msgh_seqno);

	} else if (nms_msg && ((struct file *)sp)->f_magic == F_MAGIC) {
		start_notify_op(sp, 2006);  

		SELQDEBUG(printf("file no-senders %x\n",port);)
		file_port_no_senders((struct file *)sp,
				     nms_msg->not_header.msgh_seqno);

#if     MAPPED_FILES | PFS
        } else if (nms_msg && ((tip = PORT_TO_TINFO(port)) != NULL)) {
		start_notify_op(sp, 2007); 

		SELQDEBUG(printf("token no-senders %x\n",port);)
		token_port_no_senders(port, tip, nms_msg->not_count,
				      nms_msg->not_header.msgh_seqno);
#endif  MAPPED_FILES | PFS

        } else if (dn_msg) {
		/*
		 * We try each routine expecting a dead-name notification
		 * in turn.  One will return TRUE meaning it accepted the
		 * notification.  If they all return FALSE, then somewhere
		 * this dead-name must still be `active' in the system,
		 * and will eventually be found out to be a dead-name.
		 *
		 * No matter what, we de-allocate the uref that came from
		 * the generation of the dead-name message.  We cannot
		 * depend upon mach_msg_destroy to remove this ref, as
		 * it is actually only a name.
		 */
		start_notify_op(sp, 2009);  

		/*
		 * Try each type of possible dead-name.
		 */
 		if (select_deadname(port)) {
 			/* 
 			  * if the notification comes on a file port we
 			  * must increment the seqno in the file struct
 			  * so as to process nms on file ports correctly.
 			  */
			if(MACH_PORT_VALID(dn_msg->not_header.msgh_local_port)) {
				PORT_TO_STRUCTURE((dn_msg->not_header.msgh_local_port),
					lp);
				if (((struct file *)lp)->f_magic == F_MAGIC) {
					file_port_increment_seqno((struct file *)lp);
				}
			}
 		} else {
			SELQDEBUG(printf("***deadname didn't handle %x\n",
					port));
		}
		ret = mach_port_deallocate(mach_task_self(), port);
		if (ret != KERN_SUCCESS) {
			SELQDEBUG(printf("***fsvr_notify: deallocate d-n %x=%d\n",
				port, ret));
		}
	} else 
		return(FALSE);

	end_notify_op();
	return TRUE;
}


mach_port_t	node_to_fileserver_port();
mach_port_t	port_to_fileserver_port();

node_t	dev_node_lookup();

node_t
vnode_to_fileserver_node(vp)
        struct vnode    *vp;
{
	return(dev_node_lookup(vp->v_devnode));
}

mach_port_t
vnode_to_fileserver_port(vp)
	struct vnode    *vp;
{
	return(node_to_fileserver_port(vnode_to_fileserver_node(vp)));
}

#if     NORMA_IPC

/*
 * Check whether a node is in the node table and Hypercube I/O node table.
 */
int
is_node_valid(
	node_t		node)
{
	node_t		low, high, mid;
	extern node_t	node_array[][2];
	extern int	node_array_entries;
#ifdef	__i860__
	extern node_t	cube_io_node_array[][2];
	extern int	cube_io_node_array_entries;
#endif	/* __i860__ */

	ASSERT(node_array_entries > 0);

	/*
	 * Use a simple binary search algorithm.
	 */
	low = 0;
	high = node_array_entries - 1;
	while (low <= high) {
		mid = (low + high) >> 1;
		if (node < node_array[mid][0])
			high = mid - 1;
		else if (node > node_array[mid][1])
			low = mid + 1;
		else
			return(TRUE);
	}

#ifdef	__i860__
	/*
	 * Check for valid i386 nodes on the Hypercube.
	 */
	if (cube_io_node_array_entries == 0)
	    return(FALSE);

	/*
	 * Use a simple binary search algorithm.
	 */
	low = 0;
	high = cube_io_node_array_entries - 1;
	while (low <= high) {
		mid = (low + high) >> 1;
		if (node < cube_io_node_array[mid][0])
			high = mid - 1;
		else if (node > cube_io_node_array[mid][1])
			low = mid + 1;
		else
			return(TRUE);
	}
#endif	/* __i860__ */

	return(FALSE);
}

node_t
port_to_node(p)
	mach_port_t 	p;
{
	node_t          node;

        if (norma_port_location_hint(mach_task_self(), p,
                                     &node) != KERN_SUCCESS) 
		return(-1);
	else
		return(node);
}

mach_port_t
port_to_fileserver_port(p)
	mach_port_t 	p;
{
	node_t          node;

	if ((node = port_to_node(p)) == -1)
                return (MACH_PORT_NULL);
        else 
		return (node_to_fileserver_port(node));
}

/*
 * Routine to translate a node to a fileserver port
 */
mach_port_t
node_to_fileserver_port(node)
	node_t		node;
{
	mach_port_t			ns_port = MACH_PORT_NULL;
	mach_port_t			fport = MACH_PORT_NULL;
	struct fileserver_port_entry	*ftep;
	queue_head_t			*qp;
#define HASH_Q_NO 16
#define NODE_HASH(node) (node % HASH_Q_NO)
	static queue_head_t		fileserver_port_table[HASH_Q_NO];
	static zone_t			fileserver_port_table_zone;
	static struct mutex		fileserver_port_table_lock;
	static int			fileserver_table_entries_inuse = 0;
	static int			first_time = 1;
        int 		max_retry = 75;   /* try 75 times for server contact */
        int 		print_count = 10; /* print a message every 10 retries */
        int 		sleep_secs = 10;  /* sleep 10 secs between each retry */
	int		i;
	kern_return_t	ret;
	extern node_t	debug_node;

	struct fileserver_port_entry {
		struct queue_entry	hlink;
		mach_port_t		fileserver_port;
		node_t			node;
	};

#ifdef TNC
	if (!is_node_valid(node)) {
		return(MACH_PORT_NULL);
	}
#endif /* TNC */

	/* XXX the extra printfs in this routine should be removed. */

	if (this_node == debug_node && node != this_node) {
		/*
		 * This routine mustn't succeed on the debug node
		 * (unless it's looking up its own root fs port).
		 */
		printf("node_to_fileserver_port: node lookup rejected on debug node\n");
		return(MACH_PORT_NULL);
	}

	if (first_time) {
		for (i = 0; i < HASH_Q_NO; i++) {
			queue_init(&fileserver_port_table[i]);
		}
		mutex_init(&fileserver_port_table_lock);
		fileserver_port_table_zone = zinit(sizeof(struct fileserver_port_entry),
				vm_page_size, 0, "fileserver_port_entry");
		first_time = 0;
	}

	/*
	 * Lookup in the hash table
	 */
	mutex_lock(&fileserver_port_table_lock);
	qp = &fileserver_port_table[NODE_HASH(node)];
	ftep = (struct fileserver_port_entry *) queue_first(qp);
	while (!queue_end(ftep, (struct fileserver_port_entry *) qp)) {
		if (ftep->node == node ) {
			fport = ftep->fileserver_port;
			goto done;
		}
		ftep =(struct fileserver_port_entry *)queue_next(&ftep->hlink);
	}

	/*
	 * We did not find it in the hash queues, must do it the hard way.
	 * Start by getting the nameserver port.
	 */
	for (i=0; i<max_retry; i++) {
		ret = norma_get_nameserver_port(privileged_host_port, node,
						&ns_port);
                if (ret == KERN_INVALID_ARGUMENT) {
                        /* 
                         * if node is not in the norma domain just return 
                         * MACH_PORT_NULL
                         */
                        goto done;
                } else 	if (ret != KERN_SUCCESS)
			panic("norma_get_nameserver_port failed 0x%x", ret);
		if (MACH_PORT_VALID(ns_port))
			break;
		if (!(i % print_count)) {
			printf("node_to_fileserver_port: %s %d ...\n",
					"Waiting for nameserver on node", node);
		}
		mach_init_sleep(sleep_secs);
	}
	if (!MACH_PORT_VALID(ns_port)) {
		printf("node_to_fileserver_port(node=%d): Unable to get nameserver port\n", node);
		goto done;
	}

	netname_look_up(ns_port, "FILESERVER", "FILESERVER", &fport);

	if (!MACH_PORT_VALID(fport)) {
		printf("node_to_fileserver_port(node=%d): Unable to get fileserver port\n", node);
		fport = MACH_PORT_NULL;
		goto done;
	}

	ZALLOC(fileserver_port_table_zone, ftep,struct fileserver_port_entry*);
	if (ftep == (struct fileserver_port_entry *) NULL) {
		panic("node_to_fileserver_port: unable to allocate entry");
	}

	fileserver_table_entries_inuse++;
	ftep->node = node;
	ftep->fileserver_port = fport;
	enqueue(qp, &ftep->hlink);
	mach_port_deallocate(mach_task_self(), ns_port);
done:
	mutex_unlock(&fileserver_port_table_lock);
	return(fport);
}
/*
 * Routine to translate a node to a master_device_port
 */
mach_port_t
node_to_master_device_port(node)
	node_t		node;
{
	mach_port_t			mport = MACH_PORT_NULL;
	queue_head_t			*qp;
	static queue_head_t		master_device_port_table[HASH_Q_NO];
	static zone_t			master_device_port_table_zone;
	static struct mutex		master_device_port_table_lock;
	static int			master_device_table_entries_inuse = 0;
	static int			first_time = 1;
	struct master_device_port_entry	*mtep;
	kern_return_t			ret;

	struct master_device_port_entry {
		struct queue_entry	hlink;
		mach_port_t		master_device_port;
		node_t			node;
	};

#ifdef TNC
	if (!is_node_valid(node)) {
		return(MACH_PORT_NULL);
	}
#endif /* TNC */

	if (first_time) {
		int i;
		for (i = 0; i < HASH_Q_NO; i++) {
			queue_init(&master_device_port_table[i]);
		}
		mutex_init(&master_device_port_table_lock);
		master_device_port_table_zone = 
				zinit(sizeof(struct master_device_port_entry),
				vm_page_size, 0, "master_device_port_entry");
		first_time = 0;
	}
	/*
	 * Lookup in the hash table
	 */
	mutex_lock(&master_device_port_table_lock);
	qp = &master_device_port_table[NODE_HASH(node)];
	mtep = (struct master_device_port_entry *) queue_first(qp);
	while (!queue_end(mtep, (struct master_device_port_entry *) qp)) {
		if (mtep->node == node ) {
			mport = mtep->master_device_port;
			goto done;
		}
		mtep = (struct master_device_port_entry *)
					queue_next(&mtep->hlink);
	}
	/*
	 * We did not find it in the hash queues , must do it the hard way
	 * by calling the micro-kernel
	 */
	ret = norma_get_device_port(privileged_host_port, node, &mport);
	if (ret != KERN_SUCCESS) {
		printf("node_to_master_device_port: ret is %d\n", ret);
		goto done;
	}

	ZALLOC(master_device_port_table_zone, mtep,
				struct master_device_port_entry *);
	if (mtep == (struct master_device_port_entry *) NULL) {
		panic("node_to_master_device_port: unable to allocate entry");
	}

	master_device_table_entries_inuse++;
	mtep->node = node;
	mtep->master_device_port = mport;
	enqueue(qp, &mtep->hlink);
done:
	mutex_unlock(&master_device_port_table_lock);
	return(mport);
}

struct	dev_tab_entry	{
		node_t			pnode;
		node_t			lnode;
		struct 	dev_tab_entry	*next;
	};

#define	DEV_TAB_ELEMENTS 	16
#define DEV_HASH(node)		(node % DEV_TAB_ELEMENTS)
struct	dev_tab_entry		*dev_table[DEV_TAB_ELEMENTS];

/*
 * Routine to build the mapping from physical device node to
 * logical device node using boot config info. 
 * This routine also enforces the rule that a physical node
 * may be serviced by one and only one server (logical node)
 */
boolean_t
dev_tab_fun(
	node_t	*log_node,
	node_t	*phys_node,
	node_t	this_node)
{
	static	boolean_t	first_time = TRUE;
	struct 	dev_tab_entry	*dtep;
	int	i;

	if (first_time == TRUE) {
		first_time = FALSE;

		for (i = 0; i < DEV_TAB_ELEMENTS; i++) {
			dev_table[i] = (struct dev_tab_entry *) NULL;
		}
	}
	if (!log_node || !phys_node)
		return FALSE;

	i = DEV_HASH(*phys_node);
	dtep = dev_table[i];
	while (dtep != (struct dev_tab_entry *)NULL) {
		if (dtep->pnode == *phys_node) {
			printf("Node %d: Warning: DEV_TAB: physical node %d is already assigned to %d\n",
				this_node, *phys_node, dtep->lnode);
			return FALSE;
		}
		dtep = dtep->next;
	}
	/*
	 * We must use malloc since it is too early for zalloc
	 */
	dtep = (struct dev_tab_entry *) malloc(sizeof(struct dev_tab_entry));
	if (dtep == NULL)
		panic("dev_tab_fun: Unable to malloc entries\n");
	dtep->pnode = *phys_node;
	dtep->lnode = *log_node;
	dtep->next = dev_table[i];
	dev_table[i] = dtep;

	return TRUE;
}

node_t
dev_node_lookup(phys_node)
node_t	phys_node;
{
	struct 	dev_tab_entry	*dtep;

	dtep = dev_table[DEV_HASH(phys_node)];
	while (dtep != (struct dev_tab_entry *) NULL) {
		if (dtep->pnode == phys_node) {
			return(dtep->lnode);
		}
		dtep = dtep->next;
	}
        return(phys_node);
}
#else	/* NORMA_IPC */

node_t
port_to_node(p)
	mach_port_t p;
{
	return(this_node);
}

mach_port_t
port_to_fileserver_port(p)
	mach_port_t p;
{
	panic ("port_to_fileserver_port on a non-NORMA_IPC server");
}

mach_port_t
node_to_fileserver_port(node)
	node_t	node;
{
	return(fileserver_port);
}

mach_port_t
node_to_master_device_port(node)
	node_t		node;
{
	panic ("node_to_master_device_port on a non-NORMA_IPC server");
}
#endif  /* NORMA_IPC */
