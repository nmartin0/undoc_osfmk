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
 * This source file was created by the Center for High Performance 
 * Computation (CHPC) on behalf of OSF.
 *
 */
/*
 * HISTORY
 * $Log: fsvr_msg.c,v $
 * Revision 1.45  1995/04/08  00:14:53  yazz
 *  Authors of fix: Ray Shapouri and Bob Yasi
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12851
 *  Testing: EATs controlc, sched, os_interfaces MUNOPS SAT runs
 *  Module(s):
 * 	server/uxkern/fsvr_msg.c
 * 	server/uxkern/fsvr_server_side.c
 * 	server/uxkern/syscall_subr.c
 * 	server/uxkern/syscall_subr.h
 * Added new routines vm_allocate_strict() and vm_deallocate_strict().  They
 * do what their corresponding Mach OS calls do, but panic on any failure.
 *
 * Remember to dealloc VM in both the local and remote cases for PFS and
 * other mounted filesystems.
 *
 * Revision 1.44  1995/04/08  00:09:40  yazz
 *
 * Revision 1.43  1995/03/10  20:37:43  stans
 *  table(TBL_PG_INFO)
 *  get_pginfo_local() now uses vm_statistics() call to fill in missing
 *  pg_pagein_count & pg_pageout_count values for the default paging file.
 *
 *  Reviewer:John Litvin
 *  Risk:low
 *  Benefit or PTS #:12675
 *  Testing:
 *         WW10 sats ran with NO errors.
 *         Developer tests: pgs - paging statistics program
 *
 * Revision 1.42  1995/02/10  23:59:46  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.41  1995/02/01  22:20:43  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.40  1994/11/29  22:04:35  jlitvin
 * This is a Stefan Tritscher bug fix.  He noticed that the server is
 * using the wrong parameter for the fsvr_pf_file_init() function call.
 * This could result in memory corruption.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11713
 *  Testing: pgstat tool
 *  Module(s): server/uxkern/fsvr_msg.c
 *
 * Revision 1.39  1994/11/18  20:48:02  mtm
 * Copyright additions/changes
 *
 * Revision 1.38  1994/10/04  17:41:58  stefan
 * Checked this in for johannes:
 * In order to prevent that such files are created (until PFS
 * implementation has been changed) it is checked if the directory
 * where the core dump has to be written to is not in the PFS.
 * For this the remote_lookup() function is extended to notify
 * in the rdev parameter that a directory is in PFS. Thus PFS property
 * can be check by calling remote_lookup.
 *
 *  Reviewer: stefan
 *  Risk: low
 *  Benefit or PTS #: 10483
 *  Testing: developer testing
 *  Module(s): svr/server/paracore/dump.c
 *             svr/server/uxkern/fsvr_msg.c
 *
 * Revision 1.37  1994/09/29  10:12:02  johannes
 * new stub remote_rmdir() for removing existing core directory
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: 10705
 *  Testing: developer testing, corefile EAT
 * 	  compiling/running with other configurations (LITE, test,
 *           no PARACORE)
 *  Module(s): svr/server/paracore/dump.c
 * 	    svr/server/uxkern/fsvr_msg.c
 *             svr/server/uxkern/fsvr.defs
 *             svr/server/uxkern/fsvr_server_side.c
 *
 * Revision 1.36  1994/08/31  22:47:41  mtm
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
 * Revision 1.34.2.1  1994/08/17  01:08:34  jlitvin
 * The change to PTS #9619 was too aggressive.  The SERVER_DEALLOC()
 * macro didn't call vm_deallocate() for cases where the system call
 * failed with an error besides NO_MIG_REPLY.  Change the other 6 calls
 * to only explicitly call vm_deallocate() for successful writes.  (PTS
 * #10428 was caused by the 7th call.)
 *
 * Reviewer(s): dbm, yazz
 * Risk: Low
 * PTS #: 10626
 * Mandatory: No
 * Testing: developer
 * Module(s): server/uxkern/{fsvr_server_side.c,bsd_server_side.c,fsvr_msg.c}
 *
 * Revision 1.34  1994/07/11  17:00:00  johannes
 * remote_mkdir(): changed destination port to forward port
 * remote_unlink(): new function to remove files inside the server
 * remote_vnreaddir(): new function to read a directory inside the server
 * S_fsvr_mkdir(): changed defines for LITE server
 * S_fsvr_unlink(): new function, MIG server part, calling unlink()
 * S_fsvr_vnreaddir(): new function, MIG server part, calling vn_readdir()
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: L
 *  Benefit or PTS #: cleanup of core directory (PARACORE)
 *  Testing: developer tests
 *  Module(s): 	server/paracore/allocinfo.c
 * 		server/paracore/core.c
 * 		server/paracore/dump.c
 * 		server/uxkern/fsvr.defs
 * 		server/uxkern/fsvr_msg.c
 * 		server/uxkern/fsvr_server_side.c
 * 		server/vfs/vfs_vnops.c
 *
 * Revision 1.33  1994/07/05  19:21:30  cfj
 * The S_fsvr_getmntid() function now initializes the returned
 * mount id to -1 and only calls getmntid() if FULLSERVER is
 * defined.
 *
 *  Reviewer:suri
 *  Risk:L
 *  Benefit or PTS #:9992
 *  Testing: test case
 *  Module(s):server/vfs/vfs_subr.c server/uxkern/fsvr_msg.c
 *
 * Revision 1.32  1994/06/29  17:12:22  johannes
 * new function remote_mkdir() used for creating core directory
 *
 *  Initial check-in of parallel core dumping
 *  Reviewer: stefan, jlitvin
 *  Risk: Medium
 *  Benefit or PTS #: OS support for Postmortem Debugging
 *  Testing: developer tests
 *  Module(s):
 * 	svr/server/conf/MASTER
 * 	svr/server/conf/MASTER.i860
 * 	svr/server/conf/files.i860
 * 	svr/server/paracore/core_types.h
 * 	svr/server/paracore/allocinfo.c
 * 	svr/server/paracore/core.c
 * 	svr/server/paracore/dump.c
 * 	svr/server/paracore/dvp_pvpcore.c
 * 	svr/server/sys/allocinfo.h
 * 	svr/server/sys/core.h
 * 	svr/server/sys/user.h
 * 	svr/server/nx/nx.defs
 * 	svr/server/nx/nx.c
 * 	svr/server/bsd/kern_exit.c
 * 	svr/server/bsd/kern_fork.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/tnc/dpvproc.h
 * 	svr/server/tnc/dvp_init.c
 * 	svr/server/tnc/dvp_pvpops.c
 * 	svr/server/tnc/pvp.ops
 * 	svr/server/uxkern/fsvr_msg.c
 * 	cmds_libs/src/usr/sbin/allocator/alloc.defs
 * 	cmds_libs/src/usr/sbin/allocator/misc_rpcs.c
 * 	cmds_libs/src/usr/sbin/allocator/Makefile
 * 	cmds_libs/src/usr/include/README.locate
 * 	cmds_libs/src/usr/include/sys/Makefile
 *
 * Revision 1.31  1994/06/22  18:30:07  jlitvin
 * SERVER_DEALLOC() macro doesn't deallocate OOL memory after most system
 * call errors.  Remove this macro and always call vm_deallocate().
 * Thanks to Bob Yasi of Locus for finding these potential memory leaks.
 *
 *  Reviewer: cfj & yazz
 *  Risk: low
 *  Benefit or PTS #: 9619
 *  Testing: it builds and boots
 *  Module(s): uxkern/{syscall_subr.h,bsd_server_side.c,fsvr_msg.c,
 * 		fsvr_server_side.c}
 *
 * Revision 1.30  1994/05/13  15:31:57  chrisp
 * RPC fsvr_exec_check_vnode() modified not to return a non-zero value
 * if text busy check fails but to set a new output parameter.
 *
 *  Reviewer: slk
 *  Risk: L
 *  Benefit or PTS #: 8906
 *  Testing: Restart succeeds on node other than location of checkpoint dir.
 *  Module(s): fsvr.defs, fsvr_msg.c
 *
 * Revision 1.29  1994/05/09  04:28:32  yazz
 * Merged R1.2 revision 1.20.2.4 into main stem.
 *
 * Revision 1.28  1994/05/06  17:43:29  cfj
 * Merge revision 1.20.2.5 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.27  1994/05/05  20:51:41  cfj
 * Fix remote_get_paging_stats_all so that it does not bail out of
 * the for loop after the first node.
 *
 *  Reviewer:
 *  Risk:L
 *  Benefit or PTS #:9297
 *  Testing:test case
 *  Module(s):server/uxkern/fsvr_msg.c
 * 	   server/tnc/dvp_vpsops.c
 *
 * Revision 1.26  1994/04/22  18:06:32  dbm
 * Mainline merge of R1.2 version 1.20.2.3
 *
 * Revision 1.25  1994/04/13  16:25:59  cfj
 * Merge revision 1.20.2.2 from the R1_2 branch.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.24  1994/03/14  02:08:20  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.23  1994/01/22  00:05:00  cfj
 * Put #if FULLSERVER around references to spec_inodeops so that
 * MACH_ASSERT can be turned on for lite servers.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.22  1994/01/12  17:46:57  jlitvin
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
 * Revision 1.21  1993/12/21  23:17:16  cfj
 * Merge R1.2 changed into main-stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.20.2.5  1994/05/06  17:27:41  cfj
 * In S_fsvr_exec_lookup() remove the initialization of *command_lenp to
 * zero.  This confused mig and caused it to return a MIG_ARRAY_TOO_LARGE
 * error on execs of files on nodes other than the boot node.
 *
 *  Reviewer:dbm, jlitvin
 *  Risk:L
 *  Benefit or PTS #:9309
 *  Testing:test case, VSX EAT, message EAT
 *  Module(s):server/uxkern/fsvr_msg.c
 *
 * Revision 1.20.2.4  1994/04/27  22:11:52  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/uxkern/fsvr_msg.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.20.2.4  1994/04/27  22:11:52  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/uxkern/fsvr_msg.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.20.2.3  1994/04/22  18:02:20  dbm
 * Added ifdef FULLSERVER around device open requests to keep compute node
 * from trying to open device specific files.
 *
 *  Reviewer: Brad Rullman
 *  Risk:Low
 *  Benefit or PTS #:8590
 *  Testing: UFS eats, specific test case
 *  Module(s): fsvr_msg.c
 *
 * Revision 1.20.2.2  1994/04/13  16:23:23  cfj
 * In S_fsvr_exec_lookup(), initialize the out parameters to something
 * rational so protect the OS from ux_server_loop panics.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:8040
 *  Testing:kenbus,VSX EAT
 *  Module(s):server/uxkern/fsvr_msg.c
 *
 * Revision 1.20.2.1  1993/12/21  21:34:03  cfj
 * Added BOGUS_VNODE_PROXY_PANIC bootmagic which if set will panic the
 * server when the "bogus vnode_proxy" condition is detected.  Also puts
 * the printf inside #if MACH_ASSERT.  This has been added for debug
 * purposes only.  It does not FIX any bugs bug may allow us to debug
 * this problem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):server/uxkern/boot_config.c
 *            server/uxkern/fsvr_msg.c
 *
 * Revision 1.20  1993/09/27  04:34:42  robboy
 * New ifdefs for FULLSERVER
 *
 * Revision 1.19  1993/09/01  01:50:08  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.18  1993/09/01  00:46:42  brad
 * Fix for bug 6314 ... PFS code to detect an attempt to execute a PFS file
 * (not supported in R1.1) was not deallocating all ports in the remote case
 * (requesting server and server owning the file on different nodes).  Result
 * was a "no more senders" and panic in vrele.
 *
 * Revision 1.17  1993/08/18  16:37:27  cfj
 * Fix for PTS bug #6098.  Turn on selected code which previously was turned off if FULLSERVER
 * was not defined to allow pipes to work with the lite server in compute paritions.
 *
 * Revision 1.16  1993/07/19  22:59:41  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.15  1993/07/14  18:41:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:01:42  cfj
 * Adding new code from vendor
 *
 * Revision 1.14  1993/05/18  22:56:33  cfj
 * Add #include <nfs.h>
 *
 * Revision 1.13  1993/05/18  22:45:33  cfj
 * Add #include <nfs/nfs.h>
 *
 * Revision 1.12  1993/05/18  18:55:50  cfj
 * Add appropriate ifdefs so that the server will build when turning off NFS.
 *
 * Revision 1.11  1993/05/12  00:24:37  brad
 * Removed PFS debug.
 *
 * Revision 1.10  1993/05/07  18:30:51  nandy
 * Fixed a merge conflict
 *
 * Revision 1.9  1993/05/06  19:28:18  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.8  1993/04/03  03:11:44  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.5  1993/03/30  02:24:12  brad
 * Added send and receive stubs for forwarding PFS mount operations when the
 * disk partition to be mounted is remote.
 *
 * Revision 1.7  1993/03/29  23:07:56  nandy
 * Merged from T9 branch
 *
 * Revision 1.5.4.4  1993/03/29  22:51:22  nandy
 * Fixes from loverso to fix OIP intr.
 *  Use new OIP_INTR macro to lookup credsport and interrupt the
 *  thread with appropriate locking held in S_intr_delivery.  (loverso)
 *
 * Revision 1.5.4.3  1993/03/10  17:14:07  nandy
 * Merged from the main stem.
 *
 * Revision 1.6  1993/03/10  16:57:20  nandy
 * An oip macro changed names (LOOKUP_PORT --> OIP_LOOKUP)
 *
 * Revision 1.1.2.1.2.4  1993/02/16  20:08:00  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.1.2.1.2.3  1993/02/12  17:24:34  dbm
 * Added code to check for PFS file with exec and core and then return
 * an error message.
 *
 * Revision 1.1.2.1.2.2  1993/02/09  21:46:24  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.5  1993/01/22  17:19:04  cfj
 * 01-20-93 Locus code drop.
 * 
 * Revision 1.4  1993/01/15  02:37:46  cfj
 * Multiple service partition fixes from Locus.
 * 
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:00  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:05:28  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:54:25  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.30  1992/10/16  18:17:40  cfj
 * Modification so that NX programs can run.
 *
 * Revision 2.41  93/08/26  10:49:48  mjl
 * Remove include of obsolete <vsocket/ins_var.h>.
 * 
 * Revision 2.40  93/07/13  16:09:36  slively
 * Includes removing some obsolete functions, update_time and
 * remote_update_time.
 *      Revision 2.51  93/06/29  16:16:56  rabii
 *      Lite server mods (rabii)
 *
 * Revision 2.39  93/06/25  11:24:13  slively
 * Backout the LITE server changes.  Remove #if NFS, #if UFS and related code.
 * 
 * Revision 2.38  93/06/22  20:03:15  slively
 * Support for LITE server, #if UFS sections and #NFS sections as well as 
 * the includes nfs.h and ufs.h.
 * 
 * Revision 2.37  93/06/16  13:53:29  klh
 * 	Revision 2.49  93/05/16  20:59:15  loverso
 * 		Pass "intr" parameter in S_intr_delivery().
 * 
 * 	Revision 2.48  93/04/12  16:52:49  durriya
 * 		fixed typographical error in previous check-in
 * 		[93/04/12            durriya]
 *
 * 	Revision 2.47  93/04/12  16:43:00  durriya
 * 		pgstat now queires only nodes in the pager_node_list instead 
 * 		of all nodes in the domain for info on all paging files
 * 		[93/04/11            durriya]
 *
 * 	Revision 2.46  93/04/05  18:05:07  durriya
 * 		NFS performance changes - Use new msg2mbuf() in remote_nfsrv_op
 * 		and fsvr_nfsrv_op. msg2mbuf() now returns a boolean indicating
 * 		whether the caller should vm_deallocate the data area.
 * 		[93/04/05            durriya]
 *
 * 	Revision 2.45  93/03/30  16:08:40  roy
 * 		Allow fast path files to be exec'd.
 * 		[93/03/22            roy]
 *
 * 	Revision 2.44  93/03/25  10:09:58  durriya
 * 		add interfaces to support returning paging statistics. This
 * 		includes remote_get_paging_stats[_all], fsvr_get_paging_stats,
 * 		etc                                                  (durriya)
 *
 * 	Revision 2.43  93/03/24  15:38:22  loverso
 * 		Use new OIP_INTR macro to lookup credsport and interrupt the
 * 		thread with appropriate locking held in S_intr_delivery.  (loverso)
 *
 * 	Revision 2.42  93/03/05  17:46:12  loverso
 * 		An oip macro changed names (LOOKUP_PORT --> OIP_LOOKUP)
 *
 * 	Revision 2.41  93/02/18  11:50:25  rabii
 * 		Added stubs for remote_pf_file_init (rabii)
 *
 * 	Revision 2.40  93/02/02  15:23:36  rabii
 * 		[93/01/12]  17:38:48  mjl
 *		In S_fsvr_unp_vsockread(), don't vrele() the vnode
 *		directly when cleaning up after errors---deallocating
 *		the acquired vnode port takes care of this.
 * 
 * 	Revision 2.39  93/01/27  11:40:37  durriya
 * 		fix calling sequence of getmntid()                        (durriya)
 *
 * 	Revision 2.38  93/01/25  22:56:39  durriya
 * 		add offset as arg to fsvr_spec_read and fsvr_spec_write.
 * 		signature of fsvr_vnode_forw was modified                     (durriya)
 *
 * 	Revision 2.36  92/12/08  10:47:43  durriya
 * 		get rid of remote_set_unmount and remote_clear_unmount. Add
 * 		remote_unmount_set and remote_unmount_done.
 * 		fix assert in remote_spec_read & remote_spec_write(durriya)
 * 
 * Revision 2.35  93/04/23  07:54:10  roman
 * Correct RCS message in last submission.
 * 
 * Revision 2.34  93/04/22  16:42:04  roman
 * Fix up RCS comments for format and remove unncessary RCS comments
 * from last merge.
 * 
 * Revision 2.33  93/01/12  17:38:48  mjl
 * In S_fsvr_unp_vsockread(), don't vrele() the vnode directly when cleaning
 * up after errors---deallocating the acquired vnode port takes care of this.
 * 
 * Revision 2.32  92/12/10  17:37:34  mjl
 * Call get_bind_port_from_vnode() instead of doing it in-line.  Use new
 * debug macros.  Cleaned up and renamed fsvr_unp_unbind().
 * 
 * Revision 2.31  92/12/01  11:48:27  chrisp
 * [Bug #116] In S_intr_delivery(), skip the body of the routine if
 * 	start_fsvrmisc_op() returns KERN_FAILURE rather than panicking.
 * 
 * Revision 2.30  92/11/23  15:56:43  klh
 * 	Revision 2.35  92/11/09  15:26:50  rabii
 * 		[92/08/17  13:23:54  mjl]
 * 		Have S_fsvr_report_migrate() issue migration notifications to FIFO vnodes.
 * 
 * 		[92/08/12  15:15:39  jdh]
 * 		added missing brackets and fixed a comment -- jdh
 * 
 * 	Revision 2.34  92/11/03  11:54:40  mmp
 * 		Add fsvr_server_register, fsvr_halt, and wrappers for shutdown.
 *		(mmp)
 * 
 * Revision 2.29  92/10/06  12:29:57  klh
 * 	Revision 2.33  92/09/29  16:48:34  rabii
 * 		If remote_getmntid() gets a invalid port from
 * 		node_to_fileserver_port(), return ENODEV.  (loverso)
 * 
 * 	Revision 2.32  92/09/11  09:29:22  rabii
 * 		Rewrite remote_getmntid to handle local and remote cases.
 * 		[92/09/10            roy]
 * 
 * 	Revision 2.31  92/08/26  12:13:45  loverso
 * 		Call VOP_UPDATE from both remote_getinfo and S_fsvr_updateinfo.
 * 
 * 		S_intr_delivery should return KERN_FAILURE if it's lookup fails.
 * 		This will cause the message to be destroyed, rather then
 * 		end_fsvrmisc_op free'ing the creds_port.  Cause: race with
 * 		process destruction causes creds_port to be MACH_PORT_DEAD.
 * 		(loverso)
 * 		[92/08/18            roy]
 * 
 * 	Revision 2.30  92/08/14  11:03:32  rabii
 * 		Fixed ASSERT in S_fsvr_updateinfo.
 * 
 * Revision 2.28  92/08/17  13:23:54  mjl
 * Have S_fsvr_report_migrate() issue migration notifications to FIFO vnodes.
 * 
 * Revision 2.27  92/08/12  15:15:39  jdh
 * added missing brackets and fixed a comment -- jdh
 * 
 * Revision 2.26  92/08/08  01:52:14  jdh
 * modified code to support storing socket port in unix domain stream
 * socket's bound vnode -- jdh
 * 
 * Revision 2.25  92/08/06  16:42:51  roman
 * Fixes to allows builds without REMOTE_PROC.
 * 
 * Revision 2.24  92/08/06  13:31:57  klh
 * 	Revision 2.28  92/07/28  19:52:58  rabii
 * 		Added new routines remote_rproc_server_register and 
 * 		remote_vnode_pager_get_state and thier server sides. These 
 * 		are used in registering remote nodes for rproc and for 
 * 		synchronizing pager initialization respectively. (rabii)
 * 
 * 	Revision 2.27  92/07/16  09:58:01  rabii
 * 		Added remote_vnopen, remote_vnrdwr and remote_vnsetattr 
 * 		and their server-side correspondents. They are used by 
 * 		the core dumping code. Changed exec_read* to use the new 
 * 		functions (pjg).
 * 
 * 	Revision 2.26  92/07/16  09:41:58  rabii
 * 		Added remote_vnopen, remote_vnrdwr and remote_vnsetattr 
 * 		and their server-side correspondents. They are used by 
 * 		the core dumping code. Changed exec_read* to use the new 
 * 		functions (pjg).
 * 
 * Revision 2.23  92/07/07  13:06:25  klh
 * 	Revision 2.25  92/06/30  22:47:48  loverso
 * 		Pass in MACH_PORT_NULL to fsvr_vnode_pager_get. (rabii)
 * 
 * Revision 2.22  92/06/10  12:19:22  klh
 * 	Revision 2.24  92/06/09  16:42:39  pjg
 * 		Fix type checking error in nfs_nfsrv_op.
 * 
 * 	Revision 2.23  92/06/08  18:29:46  pjg
 * 		Set iomode to VIO_BUF in exec because fast_path doesn't support
 * 		exec yet (pjg).
 * 		Added new routines for getting remote device attributes (rabii)
 * 		Define remote_nfsrv_op and S_fsvr_nfsrv_op   (durriya)
 * 		Add node as argument to getmntid and fsvr_getmntid   (durriya)
 * 
 * Revision 2.21  92/06/05  14:00:42  klh
 * 	Revision 2.22  92/05/18  12:26:46  roy
 * 		Revision 2.14.2.1  92/04/22  09:57:59  roy
 * 		Modified interfaces to remote_vnode_pager_get, 
 * 		remote_vnode_pager_flush, remote_inode_pager_setup_fport to 
 * 		deal with node numbers rather than file server ports.  
 * 		Simplifies handling of mmap regions and allows for
 * 		local optimization.  Name change: 
 * 		vnode_flush_object -> vnode_pager_flush.
 * 		[92/03/25            roy]
 * 
 * 		Modified get_vnode_pager_node to allow for remote vnode pager 
 *		(rabii).
 * 
 * 	Revision 2.21  92/05/12  00:05:10  loverso
 * 		Deallocate the pathname buffer in remote_exec_lookup if namei
 * 		allocated one. Fixes bug of pathbufs zone expanding (pjg).
 * 
 * 	Revision 2.20  92/05/04  15:04:25  rabii
 * 		Fixed S_intr_delivery 
 * 
 * 	Revision 2.19  92/05/01  15:52:46  rabii
 * 		Set HASPATHBUF in remote_exec_lookup (pjg).
 * 
 * 	Revision 2.18  92/05/01  10:01:19  rabii
 * 		Changed S_fsvr_exec_lookup and remote_exec_lookup to return the
 * 		command name and got rid of unused parameters (vtype, vrdev, 
 *		node) (pjg).
 * 
 * 		S_intr_delivery() now uses "start_fsvrmisc_op" to obey proper 
 * 		protocols.(loverso)
 * 
 * Revision 2.20  92/05/01  13:08:09  klh
 * move declaration of port_to_fileserver_port() to global declarations to
 * for i860 compilation.
 * 
 * Revision 2.19  92/05/01  07:56:09  roman
 * Changes from pjg to make sure that u.u_comm is set up correctly when
 * 	an exec of a file that is remote is done.
 * 
 * Revision 2.18  92/04/14  10:48:25  roman
 * Keep reference count of messages on a file port accurate.
 * 
 * Revision 2.17  92/04/07  13:46:14  pjg
 * 	Consume on success the cred port in S_intr_delivery (rabii).
 * 
 * Revision 2.16  92/04/05  17:08:23  pjg
 * 	Revised vnode_proxies and associated routines (pjg).
 * 
 * 	Add S_fsvr_getmntid and remote_getmntid                 (durriya)
 * 
 * 	Moved vsocket.h to vsocket directory
 * 	Added S_fsvr_report_migrate(), server side of TNC pipe/FIFO migration
 * 	notification.  Invokes VSOP_NOTIFY virtual socket op. (chrisp)
 * 
 * 	Add new routines get_vnode_port_from_proxy() and 
 * 	set_proxy_from_vnode_port() for manipulating vnode proxies. (roman)
 * 
 * Revision 2.15  92/03/20  11:31:49  pjg
 * 	Moved get_fserver_port to fsvr_port.c and renamed to 
 * 	port_to_fileserver_port (pjg).
 * 	S_intr_delivery now passes through a flag indicating we are exit 
 * 	processing (loverso).
 * 	Removed call to vrele and changed assert in S_fsvr_spec_close (noemi).
 * 	Use u.uu_procp->p_cred instead of ni_credsport.
 * 	creds_port no longer passed to remote_inode_pager_setup_fport and
 * 	remote_vnode_apger_get.                      (durriya)
 * 
 * Revision 2.14  92/03/16  18:27:01  pjg
 * 	92/03/16  20:20:23  noemi
 * 	Call wait_for_vxlock from S_fsvr_spec_reclaim to set VXLOCK flag in the
 * 	vnode.
 * 
 * 	92/03/16  18:18:32  noemi
 * 	Increment reader/writer counts in remote vnode agent as vn_close will
 * 	decrement them later.
 * 
 * Revision 2.13  92/03/15  14:30:26  roy
 * 	Moved most of S_intr_delivery to fsvr_subr.c (loverso)
 * 	Fix vm leaks (see calls to vm_deallocate) (pjg).
 * 	Return an error in fsvr_get_root_port if the root port is null
 * 	(the server may still be initializing) (pjg).
 * 	Allowing mmap of anon memory to be backed by vnode pager (durriya).
 * 
 * Revision 2.12  92/03/03  13:50:07  pjg
 * 	Fix vrele's in spen_*open. Add 'flag' parameter to *exec_read
 * 	functions. Correct calls to *_vnodeserver_op in mmap support
 * 	functions (pjg).
 * 	Change type of server_oip_hash_table to mpportid_hash_table_t
 * 	(loverso).
 * 
 * Revision 2.11  92/03/01  18:32:56  pjg
 * 	92/02/28  pjg
 * 	Changed deallocation of ports and memory received in messages
 * 	according to the new ux_server_loop.
 * 	Use vnode_proxies in remote_vref and remote_vrele.
 * 	Change the names of the FS support routines for exec to
 * 	remote_* to be in line with the other names.
 * 	Initialize bufcount in exec_read().
 * 
 * 	92/02/28  loverso
 * 	Changed all the interfaces to get rid of the signature port and
 * 	interrupt variable. Use macros OIP_SET_FORW and OIP_END_FORW when
 * 	operation goes remote.
 * 
 * 	92/02/28  durriya
 * 	Add support functions for mmap.
 * 
 * 	92/02/28  16:55:03  noemi
 * 	Changed all uses of covered vnode and file system root vnode ports to
 * 	mount structure ports.
 * 
 * Revision 2.8  92/01/17  19:44:50  roy
 * 	Various fixes to last revision (pjg).
 * 
 * Revision 2.7  92/01/17  18:16:06  roy
 * 	Interruptible system call support (loverso).
 * 
 * Revision 2.6  92/01/16  17:23:34  roy
 * 	92/01/16  13:27:45  pjg
 * 	Added Support for NFS client.
 * 	Include the header file generated by MiG to allow type checking
 * 	by the compiler (ANSI C compilers only ?).
 * 
 * Revision 2.5  92/01/14  11:20:39  roy
 * 	92/01/11  18:48:07  ses
 * 	Added S_fsvr_vref.
 * 
 * 	92/01/10  22:02:55  noemi
 * 	Added S_fsvr_update_time and remote_update_time.
 * 
 * 	92/01/07  17:00:32  noemi
 * 	Added remote_getfsstat and remote_getfsstat_count.  
 * 
 * 	92/01/05  21:12:40  noemi
 * 	Added remote_sync.
 * 
 * Revision 2.4  92/01/09  23:00:00  roy
 * 	Many places weren't passing serial arg to end_*_ops.
 * 
 * Revision 2.3  92/01/09  16:00:55  roy
 * 	Unix domain socket support (loverso).
 * 
 * Revision 2.2  92/01/05  20:22:19  roy
 * 	1991/12/16  18:39:51  pjg
 * 	Added routines to support remote exec().
 * 
 * 	1991/10/14  20:34:23  noemi
 * 	Initial revision
 * 
 * $EndLog$
 */

#include <remote_proc.h>
#include <norma_ipc.h>
#include <fullserver.h>
#include <ufs.h>
#include <nfs.h>
#if NORMA_IPC
#include <mach/norma_special_ports.h>
#endif
#include <sys/param.h>
#include <sys/kernel.h>
#include <sys/user.h>
#include <sys/ucred.h>
#include <uxkern/import_mach.h>
#include <uxkern/syscall_subr.h>
#include <uxkern/sthread.h>
#include <uxkern/port_hash.h>
#include <uxkern/rmtnode.h>
#include <uxkern/fsvr_generic.h>
#include <uxkern/fsvr.h>
#include <uxkern/port_hash.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/specdev.h>
#include <kern/zalloc.h>
#include <nfs/nfsv2.h>
#include <sys/reboot.h>
#include <sys/table.h>

mach_port_t		port_to_fileserver_port();
#ifdef	TNC
#include <sys/mbuf.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <vsocket/vsocket.h>
#include <vsocket/vs_types.h>
#include <sys/socketvar.h>
#include <tnc/un.h>	/* variables used for debugging */
#else	/* TNC */
#include "sys/so_defs.h"
#endif	/* TNC */
#if NFS
extern int (*nfsrv_procs[NFS_NPROCS])();
#endif /* NFS */

/*
 * Server and client side "glue" for server-to-server messages.
 */

/*
 * SERVER SIDE ROUTINES.
 */

/*
 * Server side routine that receives the interrupt message
 * (fileservers only).
 */

extern mpportid_hash_table_t    server_oip_hash_table;
extern struct vnodeops 		spec_rmtnodeops;
#if	FULLSERVER
extern struct vnodeops 		spec_inodeops;
#endif  /* FULLSERVER */
extern  node_t                  pager_node;
extern  int                     import_paging;
extern  node_t                  *pager_node_list;
extern  size_t                  pager_node_list_ent;

/*
 * Handles destination end of the forwarding of an interupt request.
 * oip_intr, in turn, could forward this off further.
 * The chain is initiated by either the issig_psig thread calling
 * uemul_forward_signal() or by pproc_exit() calling credentials_deallocate().
 *
 * intr is an indication to force EINTR (vs ERESTART).
 * exiting is an indication that we are in exit processing.
 */
int
S_intr_delivery(forwport, credsport, transid, intr, exiting)
	mach_port_t		forwport;
	mach_port_t		credsport;
	transaction_id_t	transid;
	boolean_t		exiting;
{
	register int		error;

	error = start_fsvrmisc_op(forwport, credsport);
	if (!error) {
		int oip_intr();

		u.uu_oip_intr = intr;
		u.uu_oip_exiting = exiting;
		if (!OIP_INTR(credsport, transid, oip_intr))
			error = KERN_FAILURE;		/* we go away */
	}

	error = end_fsvrmisc_op(forwport, credsport, error);
	return(error);
}


/*
 * SERVER SIDE ROUTINES FOR BOOTSTRAPPING.
 */ 

int
S_fsvr_get_root_port(root_fsvr_port, root_vn_port)
	mach_port_t	root_fsvr_port;
	mach_port_t	*root_vn_port;	/* out */
{
	register int	error;
	extern 		mach_port_t root_vnode_port;

	error = start_fsvrport_op(root_fsvr_port, MACH_PORT_NULL, 0, 0, 0);
	if (error)
		panic("S_fsvr_get_root_port: can't start fsvrport op");

	if (root_vnode_port == MACH_PORT_NULL) {
		error = ENXIO;
	} else {
		ASSERT(root_vnode_port != MACH_PORT_NULL);
		get_vnode_port(rootdir, root_vn_port);
	}
	error = end_fsvrport_op(error, 0);
	return(error);
}

/*
 * SERVER SIDE FOR GENERAL VNODE OPERATIONS.
 */


/*
 * Lookup message.  Just set up nameidata and call namei.
 */
int
S_fsvr_lookup(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, options, typep, rdevp, nodep, vnode_portp)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	u_int			pathname_len;
	int			options;
	int			*typep;		/* out */
	int			*rdevp;		/* out */
	int			*nodep;		/* out */
	mach_port_t		*vnode_portp;	/* out */
{
	register int	error;
	struct nameidata *ndp = &u.u_nd;
	register struct vnode *vp = NULLVP;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				creds_port, transid,
				pathname, pathname_len, 0, 0);
	if (error)
		return(error);
	ndp->ni_nameiop = options | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	error = namei(ndp);
	/*
	 * If a remote mount was detected, forward the lookup message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_lookup(ndp->ni_forwport, creds_port,
				    transid, root_port, 
				    ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				    options, typep, rdevp, nodep, 
				    vnode_portp);
		OIP_END_FORW(oipp);
	} else if (!error) {
		vp = ndp->ni_vp;
		/*
		 * Get the port for the vnode, allocating a new one if
		 * necessary. Release the reference acquired by namei.
		 * The only reference is the one obtained by get_vnode_port.
		 */
		get_vnode_port(vp, vnode_portp);
		vrele(vp);
		*typep = vp->v_type;
		if (vp->v_type == VBLK || vp->v_type == VCHR)
			*rdevp = vp->v_rdev;
		else	
			*rdevp = 0;
#if	defined(PARACORE) && defined(PFS)
		/* XXX
		 * It's ugly to use the rdevp parameter, 
		 * but that's the easiest way to get 
		 * the information whether a directory is in PFS 
		 * without changing the interface.
		 */
		if (vp->v_type == VDIR) {
			extern struct vnodeops pfs_vnodeops;
			
			*rdevp = (int)(vp->v_op == &pfs_vnodeops);
		}
#endif
	}
	error = end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
			error, 0);
	if (error) {
		if (vp != NULLVP)
			vrele(vp);
		if (*vnode_portp != MACH_PORT_NULL)
			mach_port_deallocate(mach_task_self(), *vnode_portp);
		*vnode_portp = MACH_PORT_NULL;
		*typep = 0;
		*rdevp = 0;
	} else
		ASSERT(*vnode_portp != MACH_PORT_NULL);
	return(error);
}



/*
 * SERVER SIDE FUNCTIONS FOR MOUNT RELATED OPERATIONS.
 */

/*
 * unmount_set message.  Translate the covered vnode port passed in and call
 * unmount_set
 */
int
S_fsvr_unmount_set(mount_port, creds_port, transid)
	mach_port_t		mount_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
{
	int 	error;
	struct 	nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(mount_port, MACH_PORT_NULL,MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the covered vnode in ndp->ni_cdir.
	 */
	ASSERT(ndp->ni_cdir != NULLVP);
	error = unmount_set(ndp->ni_cdir);
	return(end_vnodeserver_op(mount_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0));
}

/*
 * Clear_unmount message.  Translate the covered vnode port passed in and
 * call clear_unmount.
 */
int
S_fsvr_unmount_done(mount_port, creds_port, transid, inerror)
	mach_port_t		mount_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			inerror;
{
	int	error;
	struct 	nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(mount_port, MACH_PORT_NULL,MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the covered vnode in ndp->ni_cdir.
	 */
	ASSERT(ndp->ni_cdir != NULLVP);
	error = unmount_done(ndp->ni_cdir, inerror);
	return(end_vnodeserver_op(mount_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0));
}

/*
 * Mount_update message.  Translate the covered vnode port and call
 * mount_update.
 */
int
S_fsvr_mount_update(mount_port, creds_port, transid, flag)
	mach_port_t		mount_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			flag;
{
	int	error;
	struct 	nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(mount_port, MACH_PORT_NULL,MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the covered vnode in ndp->ni_cdir.
	 */
	ASSERT(ndp->ni_cdir != NULLVP);
	error = mount_update(ndp->ni_cdir, flag);
	return(end_vnodeserver_op(mount_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0));
}

#if     UFS
/*
 * Ufs_mountfs message.  Translate the device file port into a vnode and call
 * ufs_mountfs.
 */

int
S_fsvr_ufs_mountfs(file_port, creds_port, transid, remote_port, dir,
		   dir_len, spec, spec_len, flag, mount_portp)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		remote_port;
	char			*dir;
	int			dir_len;
	char			*spec;
	int			spec_len;
	int			flag;	
	mach_port_t		*mount_portp;	/* out */
{
	int error;
	struct vnode 	*devvp;
	struct file	*fp;

	error = start_fileserver_op(&fp, file_port, creds_port,
			transid, 0, 0);
	if (error)
		return(error);
	devvp = (struct vnode *)fp->f_data;
	VREF(devvp);
	error = ufs_mountfs(devvp, flag, remote_port, dir, spec, mount_portp);
	/*
	 * Release the reference on the vnode.  The open performed earlier
	 * already acquired a reference.
	 */
	vrele(devvp);
	return(end_fileserver_op(fp, error, 0));
}
#endif  /* UFS */

#ifdef	PFS
/*
 * pfs_mountfs message.  Translate the device file port into a vnode and call
 * pfs_mountfs.
 */
int
S_fsvr_pfs_mountfs(file_port, creds_port, transid, remote_port, dir, dir_len,
		   spec, spec_len, flag, stripe_attr, stripe_attr_len,
		   mount_portp)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		remote_port;
	char			*dir;
	int			dir_len;
	char			*spec;
	int			spec_len;
	int			flag;	
	struct statpfs		*stripe_attr;
	int			stripe_attr_len;
	mach_port_t		*mount_portp;	/* out */
{
	int		error;
	struct vnode 	*devvp;
	struct file	*fp;
	kern_return_t	kr;

	error = start_fileserver_op(&fp, file_port, creds_port,
				    transid, 0, 0);
	if (error)
		return(error);
	devvp = (struct vnode *)fp->f_data;
	VREF(devvp);
	error = pfs_mountfs(devvp, flag, remote_port, dir, spec, stripe_attr,
			    mount_portp);
	/*
	 * Release the reference on the vnode.  The open performed earlier
	 * already acquired a reference.
	 */
	vrele(devvp);

	/*
	 * Deallocate VM allocated by MiG when passing out-of-line data.
	 *
	 * Actually, we only want to do that for successful calls.
	 * For error cases (if (error != 0)), the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(),
				   (vm_address_t)stripe_attr,
				   (vm_size_t)stripe_attr_len);
		if (kr != KERN_SUCCESS) {
		   panic("S_fsvr_pfs_mountfs: vm_deallocate(0x%x,%d) = 0x%x\n",
				stripe_attr, stripe_attr_len, kr);
		}
	}

	return(end_fileserver_op(fp, error, 0));
}
#endif	PFS

#if NFS
/*
 * nfs_mountfs message. This message was sent from the file-server handling
 * the directory where the mount is being made and is received here
 * by the NFS-client file-server.
 * It calls nfs_mountfs() which eventually calls mountnfs() to do
 * the real work.
 */
int
S_fsvr_nfs_mountfs(netserver_port, creds_port, transid, remote_port, 
		   pathname, pathname_len, flags, args, addr, fh, hostname, 
		   hostname_len, mount_portp)
	mach_port_t	netserver_port;
	mach_port_t	creds_port;
	transaction_id_t transid;
	mach_port_t     remote_port;
	char            *pathname;
	int		pathname_len;
	int             flags;
	nfs_args_t	args;
	sockaddr_in_t	addr;
	nfsv2fh_t	fh;
	char		*hostname;
	int		hostname_len;
	mach_port_t	*mount_portp;	/* out */
{
	int error;

        if (error = start_fsvrport_op(netserver_port, creds_port, transid,
				0, 0))
		return(error);

	args.addr = &addr;
	args.fh   = &fh;
	args.hostname = hostname;

	error = nfs_mountfs(remote_port, pathname, flags, &args, mount_portp);

	return(end_fsvrport_op(error, 0));
}
#endif /* NFS */


/*
 * SERVER SIDE ROUTINES FOR OPERATIONS ON SPECIAL FILES.
 */

/*
 * Open an unopened special file and return a file structure port and
 * the device vnode port.
 */
int
S_fsvr_spec_fsopen(fsvr_port, creds_port, transid, node, dev, type,
		mode, tag, vnode_port, file_portp, dev_portp)
	mach_port_t		fsvr_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	node_t			node;
	dev_t			dev;
	enum vtype		type;
	int			mode;
	enum vtagtype		tag;
	mach_port_t		vnode_port;
	mach_port_t		*file_portp;	/* out */
	mach_port_t		*dev_portp;	/* out */
{
#if FULLSERVER
	int			error;
	struct vnode		*specvp;
	struct specinfo		*si;
	struct file		*fp;
	int 			syscode = SYS_open;
	int 			serial = !sysent[syscode].sy_parallel;
	struct 			nameidata *ndp = &u.u_nd;
	extern struct fileops 	vnops;
	extern int		remote_vnode_agents;
	extern struct mutex	rmt_agent_lock;
	extern rmtspec_sync();

	*file_portp = MACH_PORT_NULL;
	*dev_portp = MACH_PORT_NULL;

	error = start_fsvrport_op(fsvr_port, creds_port, transid,
			syscode, serial);
	if (error)
		return(error);
	error = falloc(&fp);
	if (error) {
		error = end_fsvrport_op(error, serial);
		return(error);
	}
	/*
	 * Allocate a new device vnode to act as a agent of the special file
	 * vnode on the remote node.
	 */
	bdevvp(dev, node, type, &specvp);

	/*
	 * Save the vnode port passed in the specinfo structure.
	 * Also, change the v_op to spec_rmtnodeops if the remote vnode
	 * is a UFS one.  This is truly ugly and violates layering - XXX
	 */
	si = specvp->v_specinfo;
	if (tag == VT_UFS) {
		struct rmtnode *rp = (struct rmtnode *)(specvp->v_data);

		specvp->v_op = &spec_rmtnodeops;
		bzero((caddr_t)rp, sizeof(struct rmtnode));
		RMT_LOCK_INIT(rp);
	}
	SPEC_WRITE_LOCK(si);
	si->si_specport = vnode_port;
	SPEC_WRITE_UNLOCK(si);

	/*
	 * Open the special file.
	 */
	error = spec_open(&specvp, mode, ndp->ni_cred);
	ASSERT(error != EREMOTEPORT);
	error = end_fsvrport_op(error, serial);
	if (error) {
		fdealloc(fp);
		vrele(specvp);
		SPEC_WRITE_LOCK(si);
		si->si_specport = MACH_PORT_NULL;
		SPEC_WRITE_UNLOCK(si);
		return(error);
	}
	FP_LOCK(fp);
	fp->f_flag = mode & FMASK;
	fp->f_type = DTYPE_VNODE;
	fp->f_ops = &vnops;
	fp->f_data = (caddr_t)specvp;
#if	SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp);
	FILE_TO_PORT_LOOKUP(fp, *file_portp);
	/*
	 * Increment the reader/writer counts in the vnode.
	 */
	if (mode & FREAD)
		specvp->v_rdcnt++;
	if (mode & FWRITE)
		specvp->v_wrcnt++;
	/*
	 * Both the open and the port each hold a reference.
	 */
	get_vnode_port(specvp, dev_portp);
	/*
	 * If this is the first vnode agent, set up for remote vnode agent
	 * syncing.
	 */
	mutex_lock(&rmt_agent_lock);
	if (remote_vnode_agents == 0) {
		remote_vnode_agents++;
		timeout(rmtspec_sync, (caddr_t)0, RMT_TIMEOUT * hz);
	}
	mutex_unlock(&rmt_agent_lock);
	return(0);
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Open an unopened special file and return the device vnode port.
 */
int
S_fsvr_spec_open(fsvr_port, creds_port, transid, node, dev, type,
		mode, tag, vnode_port, dev_portp)
	mach_port_t		fsvr_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	node_t			node;
	dev_t			dev;
	enum vtype		type;
	int			mode;
	enum vtagtype		tag;
	mach_port_t		vnode_port;
	mach_port_t		*dev_portp;	/* out */
{
#if FULLSERVER

	int			error;
	struct vnode		*specvp;
	struct specinfo		*si;
	struct 	nameidata 	*ndp = &u.u_nd;
	extern int 		remote_vnode_agents;
	extern struct mutex 	rmt_agent_lock;
	extern rmtspec_sync();

	*dev_portp = MACH_PORT_NULL;

	error = start_fsvrport_op(fsvr_port, creds_port, transid,
			0, 0);
	if (error)
		return(error);

	/*
	 * Allocate a new device vnode to act as a agent of the special file
	 * vnode on the remote.
	 */
	bdevvp(dev, node, type, &specvp);

	/*
	 * Save the vnode port passed in the specinfo structure.
	 * Also, change the v_op to spec_rmtnodeops if the remote vnode
	 * is a UFS one.  This is truly ugly and violates layering - XXX
	 */
	si = specvp->v_specinfo;
	if (tag == VT_UFS) {
		struct rmtnode *rp = (struct rmtnode *)(specvp->v_data);

		specvp->v_op = &spec_rmtnodeops;
		bzero((caddr_t)rp, sizeof(struct rmtnode));
		RMT_LOCK_INIT(rp);
	}
	SPEC_WRITE_LOCK(si);
	si->si_specport = vnode_port;
	SPEC_WRITE_UNLOCK(si);

	/*
	 * Open the special file.
	 */
	error = spec_open(&specvp, mode, ndp->ni_cred);
	ASSERT(error != EREMOTEPORT);
	error = end_fsvrport_op(error, 0);
	if (error) {
		vrele(specvp);
		SPEC_WRITE_LOCK(si);
		si->si_specport = MACH_PORT_NULL;
		SPEC_WRITE_UNLOCK(si);
		return(error);
	}
	/*
	 * Increment the reader/writer counts in the vnode.
	 */
	if (mode & FREAD)
		specvp->v_rdcnt++;
	if (mode & FWRITE)
		specvp->v_wrcnt++;
	/*
	 * Don't reference the vnode since this spec_open is not the result
	 * of an open system call.
	 */
	get_vnode_port(specvp, dev_portp);
	vrele(specvp);

	/*
	 * If this is the first vnode agent, set up for remote vnode agent
	 * syncing.
	 */
	mutex_lock(&rmt_agent_lock);
	if (remote_vnode_agents == 0) {
		remote_vnode_agents++;
		timeout(rmtspec_sync, (caddr_t)0, RMT_TIMEOUT * hz);
	}
	mutex_unlock(&rmt_agent_lock);
	return(0);
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Open an already opened  special file and return a file structure port.
 * The vnode port used as the request port can represent either the special
 * file vnode on the node servicing the special file or the pseudo device
 * vnode on the node servicing the device.
 */

int
S_fsvr_spec_vnode_fsopen(vnode_port, creds_port, transid, mode,
		file_portp)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			mode;
	mach_port_t		*file_portp;	/* out */
{
#if FULLSERVER
	int			error;
	struct 	nameidata 	*ndp = &u.u_nd;
	struct 	vnode 		*vp;
	struct 	file  		*fp;
	int			ref = 0;
	int 			syscode = SYS_open;
	int 			serial = !sysent[syscode].sy_parallel;
	extern struct fileops 	vnops;

	*file_portp = MACH_PORT_NULL;
	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, syscode,
			serial);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, serial);
		return(error);
	}
	BM(VN_UNLOCK(vp));
	/*
	 * Allocate a file structure.
	 */
	error = falloc(&fp);
	if (error) {
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, error, serial);
		return(error);
	}
	error = spec_open(&vp, mode, ndp->ni_cred);
	/*
	 * This open is a result of a remote open system call.  Reference
	 * the special file vnode so both the open and the port each hold a
	 * reference.   Also reference the vnode if it represents a remote
	 * device.  This way a lookup operation followed by a spec_open
	 * results in one reference on the vnode just as an open operation
	 * would.  (The lookup operation releases its reference on the vnode
	 * before it returns its associated port.)
	 */
	if (!error || error == EREMOTEPORT) {
		VREF(vp);
		ref++;
	}
	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, serial);
	if (error) {
		fdealloc(fp);
		if (ref && error != EREMOTEPORT)
			vrele(vp);
		if (error == EREMOTEPORT) {
			/*
			 * The device is serviced remotely.  The remote
			 * file structure port for its pseudo vnode was
			 * saved in ndp->ni_forwport by spec_open.  Return
			 * that port to the client.
			 */
			ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
			*file_portp = ndp->ni_forwport;
			error = 0;
		}
		return(error);
	}
	FP_LOCK(fp);
	fp->f_flag = mode & FMASK;
	fp->f_type = DTYPE_VNODE;
	fp->f_ops = &vnops;
	fp->f_data = (caddr_t)vp;
#if	SER_COMPAT
	fp->f_funnel = FUNNEL_NULL;
#endif
	FP_UNLOCK(fp);
	FILE_TO_PORT_LOOKUP(fp, *file_portp);
	return(0);
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Open an already opened special file without returning a file structure port.
 */
int
S_fsvr_spec_vnode_open(vnode_port, creds_port, transid, mode)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			mode;
{
#if FULLSERVER
	int			error;
	struct 	nameidata 	*ndp = &u.u_nd;
	struct 	vnode 		*vp;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, 0);
		return(error);
	}
	BM(VN_UNLOCK(vp));
	error = spec_open(&vp, mode, ndp->ni_cred);
	ASSERT(error != EREMOTEPORT);
	/*
	 * This open is not a result of a remote open system call so don't
	 * reference the vnode.
	 */
	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0);
	return(error);
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Close a special file representing a remote device.  The actual close
 * operation takes place on the remote node servicing the device.  All
 * that must be done here is to mark the specinfo structure closed and
 * release the vnode reference held by the open.
 */
int
S_fsvr_spec_close(vnode_port, creds_port, transid, flag)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			flag;
{
#if FULLSERVER
	struct nameidata 	*ndp = &u.u_nd;
	struct vnode 		*vp;
	struct specinfo		*si;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
		creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, 0);
		return(error);
	}
	/*
	 * The vnode should hold at least two references: one for the vnode
	 * port, and another for the operation in progress. The reference
	 * for the vnode port remains until the special file vnode is
	 * reclaimed.
	 */
	ASSERT(vp->v_usecount >= 2);
	si = vp->v_specinfo;
	BM(VN_UNLOCK(vp));
	if (si->si_flag & SI_RMTDEV) {
		/*
		 * If this message was sent from the node servicing the
		 * remote device, just mark the specinfo structure closed.
		 */
		SI_LOCK(si);
		si->si_flag = SI_CLOSED;
		SI_UNLOCK(si);
	} else if (si->si_flag & SI_RMTSPEC) {
		/*
		 * If this is the node servicing the device, call the actual
		 * close routine, which may be spec_close or rmtspec_close.
		 */
		VOP_CLOSE(vp, flag, ndp->ni_cred, error);
	}
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 0,
		0));
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Read a remote device.
 */

int
S_fsvr_spec_read(vnode_port, creds_port, transid, offset, len, ioflag, buf,
		bufcount)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
        int                     offset;
	int			len;
	int			ioflag;
	char			**buf;		/* out, dealloc (ool memory) */
	int			*bufcount;	/* size of the above */
{
#if FULLSERVER
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	struct uio 		auio;
	struct iovec 		aiov;
	int			error;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*buf = 0;
	*bufcount = 0;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, 0);
		return(error);
	}
	BM(VN_UNLOCK(vp));
	
	/*
	 * Allocate memory for the read.
	 */
	if (vm_allocate(mach_task_self(), (vm_address_t *)buf, len, TRUE) !=
		KERN_SUCCESS) {
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, ENOMEM, 0);
		return(error);
	}

	/*
	 * Set up uio structure as if we came in from a local read system
	 * call.
	 */
	aiov.iov_base = *buf;
	aiov.iov_len = len;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = len;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_READ;
        auio.uio_offset = offset;

	/*
	 * This read may be handled by spec_read or rmtspec_read.
	 */
	VOP_READ(vp, &auio, ioflag, ndp->ni_cred, error);

	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0);

	if (*buf) {
		if(error) {
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *buf, len);
			*buf = NULL;
			*bufcount = 0;
		}  else {
			*bufcount = len - auio.uio_resid;
			len -= round_page(*bufcount);
			if (len > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*buf + round_page(*bufcount));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}
	return(error);
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Write a remote device.
 */

int
S_fsvr_spec_write(vnode_port, creds_port, transid, offset, ioflag, buf,
		bufcount, amount)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
        int                     offset;
	int			ioflag;
	char			*buf;
	int			bufcount;
	int			*amount;	/* out */
{
#if FULLSERVER
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	struct uio 		auio;
	struct iovec 		aiov;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, 0);
		return(error);
	}
	BM(VN_UNLOCK(vp));
	
	/*
	 * Set up uio structure as if we came in from a local read system
	 * call.
	 */
	aiov.iov_base = buf;
	aiov.iov_len = bufcount;
	auio.uio_iov = &aiov;
	auio.uio_iovcnt = 1;
	auio.uio_resid = bufcount;
	auio.uio_segflg = UIO_SYSSPACE;
	auio.uio_rw = UIO_WRITE;
        auio.uio_offset = offset;

	/*
	 * This read may be handled by spec_write or rmtspec_write.
	 */
	VOP_WRITE(vp, &auio, ioflag, ndp->ni_cred, error);
	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
		error, 0);
	if (!error)
		*amount = bufcount - auio.uio_resid;
	return(error);
#else
	return (ENXIO);
#endif FULLSERVER
}



/*
 * Call clearalias for special file vnode.
 */
int
S_fsvr_clearalias(vnode_port, creds_port, transid)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
{
#if FULLSERVER
	struct nameidata 	*ndp = &u.u_nd;
	struct vnode 		*vp;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	BM(VN_LOCK(vp));
	if (vp->v_type != VBLK && vp->v_type != VCHR) {
		BM(VN_UNLOCK(vp));
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, EINVAL, 0);
		return(error);
	}
	BM(VN_UNLOCK(vp));
	clearalias(vp);
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 0,
		0));
#else
	return (ENXIO);
#endif FULLSERVER
}


/*
 * Reclaim a remote special file vnode.  Mark its specinfo structure as
 * SI_RECLAIM.
 */

int
S_fsvr_spec_reclaim(vnode_port, creds_port, transid)
	mach_port_t 		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
{
#if FULLSERVER
	struct nameidata 	*ndp = &u.u_nd;
	struct vnode 		*vp;
	struct specinfo		*si;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	VN_LOCK(vp);
	if (vp->v_type != VBLK && vp->v_type != VCHR)
		error = EINVAL;
	/*
	 * This really needs to use VX_LOCK define in vfs_subr.c - XXX.
	 */
	if (error || wait_for_vxlock(vp, 1)) {
		/*
		 * If the vnode doesn't represent a block or character device,
		 * or the reclaim is already in progress, return.
		 */
		VN_UNLOCK(vp);
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
			MACH_PORT_NULL, error, 0);
		return(error);
	}
	si = vp->v_specinfo;
	VN_UNLOCK(vp);
	SI_LOCK(si);
	si->si_flag |= SI_RECLAIM;
	SI_UNLOCK(si);
	error = spec_reclaim(vp);
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 
		error, 0));
#else
	return (ENXIO);
#endif FULLSERVER
}



/*
 * CLIENT SIDE ROUTINES FOR SERVERS ACTING AS CLIENTS OF OTHER SERVERS.
 */


/*
 * CLIENT SIDE ROUTINES FOR GENERAL VNODE OPERATIONS.
 */

/*
 * Lookup sends an fsvr_lookup message.  It hides the details of the
 * fsvr_lookup stub from its callers.
 */
int
remote_lookup(ndp, path, options, typep, rdevp, nodep, portp)
	struct nameidata 	*ndp;
	char 			*path;
	int			options;
	int  			*typep;
	int  			*rdevp;
	int  			*nodep;
	mach_port_t 		*portp;
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;

	/*
	 * Note: this function expects the request port for the lookup message
	 * to be in ndp->ni_forwport.
	 */
	ASSERT(ndp->ni_forwport != MACH_PORT_NULL);

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_lookup(ndp->ni_forwport, u.uu_procp->p_cred,
			oipp->oip_transid,
			ndp->ni_rdirport, path, strlen(path)+1,
			options, typep, rdevp, nodep, portp);
	OIP_END_FORW(oipp);
	return(error);
}

#ifdef PARACORE		

int
remote_mkdir(ndp, mode)
	struct nameidata *ndp;
	int		 mode;
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_mkdir(ndp->ni_forwport, u.uu_procp->p_cred,
			   oipp->oip_transid, ndp->ni_rdirport,
			   ndp->ni_dirp, strlen(ndp->ni_dirp)+1,
			   mode);
	OIP_END_FORW(oipp);
	return(error);
}

int
remote_rmdir(ndp)
	struct nameidata *ndp;
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_rmdir(ndp->ni_forwport, u.uu_procp->p_cred,
			   oipp->oip_transid, ndp->ni_rdirport,
			   ndp->ni_dirp, strlen(ndp->ni_dirp)+1);
	OIP_END_FORW(oipp);
	return(error);
}

#ifndef	PFS
#define VIO_NONE 0xffffffff
#endif

int
remote_unlink(ndp)
	struct nameidata *ndp;
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;
#if  	defined(PFS) || defined(PARACORE)
	u_long			iomode = VIO_NONE; /* to keep fsvr_rename happy */
#endif 	


	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_unlink(ndp->ni_forwport, u.uu_procp->p_cred,
			   oipp->oip_transid, ndp->ni_rdirport,
			   ndp->ni_dirp, 
#if	defined(PFS) || defined(PARACORE)
			   strlen(ndp->ni_dirp)+1, &iomode);
#else
			   strlen(ndp->ni_dirp)+1);
#endif
	OIP_END_FORW(oipp);
	return(error);
}

#ifndef	PFS
#undef 	VIO_NONE
#endif

#endif /* PARACORE */

int
remote_vnopen(ndp, fmode, cmode, vnode_port, type)
	struct nameidata *ndp;
	int		fmode;
	int		cmode;
	mach_port_t	*vnode_port;
	int		*type;	/* OUT */
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_vnopen(ndp->ni_cdirport, u.uu_procp->p_cred,
			    oipp->oip_transid, ndp->ni_rdirport,
			    ndp->ni_dirp, strlen(ndp->ni_dirp)+1, fmode, cmode,
			    vnode_port, type);
	OIP_END_FORW(oipp);
	return(error);
}

int
S_fsvr_vnopen(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, fmode, cmode, vnode_port, type)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			fmode;
	int			cmode;
	mach_port_t		*vnode_port;	/* out */
	int			*type;		/* out */
{
	register int	error;
	int syscode = SYS_open;
	int serial = !sysent[syscode].sy_parallel;
	struct nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     pathname, pathname_len, syscode, serial);
	if (error) 
		return(error);
#ifdef	PFS
	if ((error = vn_open(ndp, fmode, cmode, VIO_NONE)) == 0)
#else
	if ((error = vn_open(ndp, fmode, cmode)) == 0)
#endif
	{
		struct vnode *vp;

		vp = ndp->ni_vp;
#ifdef PFS
		if (VIO_IS_PFS(vp)) {
			/*
			 * Server-initiated I/O is not currently supported on
			 * PFS files.  Release the reference acquired on the
			 * vnode by namei and set error.
			 */
			vrele(vp);
			error = EFSNOTSUPP;
			*vnode_port = 0;
			*type = 0;
			goto out;
		}
#endif
		/*
		 * Get the port for the vnode, allocating a new one if
		 * necessary. Release the reference acquired by namei.
		 * The only reference is the one obtained by get_vnode_port.
		 */
		get_vnode_port(vp, vnode_port);
		vrele(vp);
		BM(VN_LOCK(vp));
		*type = vp->v_type;
		BM(VN_UNLOCK(vp));
	} else {
		*vnode_port = 0;
		*type = 0;
	}
out:
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
				  serial));
}


zone_t vnode_proxy_zone = 0;

void
remote_getnewvnode(vpp)
	struct vnode_proxy **vpp;
{
	register struct vnode_proxy *vpx;
	int nvnode_proxy = 0;
	extern int nproc;
	static int first_time = 1;

	if (first_time) {
		first_time = 0;
		nvnode_proxy = nproc; /* Arbitrary choice */
		vnode_proxy_zone = zinit(sizeof(struct vnode_proxy),
				       sizeof(struct vnode_proxy)*nvnode_proxy,
				       0,
				       "vnode_proxy_zone");
		if (vnode_proxy_zone == (zone_t)0)
			panic("remote_vnode_init");
	}

	vpx = (struct vnode_proxy *) zalloc(vnode_proxy_zone);
	if (vpx == (struct vnode_proxy *) 0)
			panic ("remote_getnewvnode");
	vpx->vpx_usecount = 1;
	*vpp = vpx;
}

/*
 * Take a reference on remote vnode represented by vnode.
 */
int
remote_vref(vpx)
	struct vnode_proxy	*vpx;
{
	ASSERT(vpx);
	VN_LOCK_PROXY(vpx);
	if (vpx->vpx_vnode != NULLVP) {
		VREF(vpx->vpx_vnode);
	}
	vpx->vpx_usecount++;
	VN_UNLOCK_PROXY(vpx);
	return(0);
}

/*
 * Release reference on remote vnode represented by vnode and but don't
 * deallocate the proxy vnode. Used for statically allocated proxy vnodes.
 */
int
remote_vrele(vpx)
	struct vnode_proxy	*vpx;
{
extern boolean_t bogus_vnode_proxy_panic;

	ASSERT(vpx);
	VN_LOCK_PROXY(vpx);
	if (vpx->vpx_vnode != NULLVP) {
		vrele(vpx->vpx_vnode);
	}
	vpx->vpx_usecount--;
	if (vpx->vpx_usecount == 0) {
		if (vpx->vpx_port != MACH_PORT_NULL) {
			int error = mach_port_deallocate(mach_task_self(),
							 vpx->vpx_port);
			if (error != KERN_SUCCESS) {
#if MACH_ASSERT
		printf ("remote_vrele: bogus vnode_proxy port=%x, error=%x\n",
				vpx->vpx_port, error);
#endif /* MACH_ASSERT */

			if (bogus_vnode_proxy_panic)
		panic ("remote_vrele: bogus vnode_proxy port=%x, error=%x\n",
				   vpx->vpx_port, error);
		    }

		}
		vpx->vpx_vnode = NULLVP;
		vpx->vpx_port  = MACH_PORT_NULL;
	}
	ASSERT(vpx->vpx_usecount >= 0);
	VN_UNLOCK_PROXY(vpx);
	return(0);
}

/*
 * Release reference on remote vnode represented by vnode and deallocate the
 * proxy vnode if the ref count drops to 0.
 */
int
remote_vfree(vpx)
	struct vnode_proxy	*vpx;
{
	ASSERT(vpx);
	VN_LOCK_PROXY(vpx);
	if (vpx->vpx_vnode != NULLVP) {
		vrele(vpx->vpx_vnode);
	}
	vpx->vpx_usecount--;
	if (vpx->vpx_usecount == 0) {
		if (vpx->vpx_port != MACH_PORT_NULL) {
			int error = mach_port_deallocate(mach_task_self(),
							 vpx->vpx_port);
			if (error != KERN_SUCCESS)
			printf ("remote_vfree: bogus vnode_proxy port=%x, error=%x\n",
				vpx->vpx_port, error);
		}
		vpx->vpx_vnode = NULLVP;
		vpx->vpx_port  = MACH_PORT_NULL;
		VN_UNLOCK_PROXY(vpx);
		ZFREE(vnode_proxy_zone, vpx);
		return(0);
	}
	VN_UNLOCK_PROXY(vpx);
	return(0);
}

void
vnode_proxy_init(vpx)
	struct vnode_proxy	*vpx;
{
	if (vpx->vpx_port != MACH_PORT_NULL) {
		if (vpx->vpx_vnode == NULLVP) {
			/*
			 * If vnode is remote, we have a send right
			 */
			int error;
			error = mach_port_mod_refs(mach_task_self(),
						   vpx->vpx_port,
						   MACH_PORT_RIGHT_SEND,1);
			if (error) 
				printf ("vnode_proxy_init:mod_refs: port=0x%x, ret=0x%x\n",
					vpx->vpx_port, error);
		} else {
			/*
			 * If vnode is local, we have a receive right
			 * (maybe also a send right, but not necessarily).
			 */
			get_vnode_port(vpx->vpx_vnode, &vpx->vpx_port);
		}
	}
	vpx->vpx_usecount = 0;
	remote_vref(vpx);
}

/*
 * Get a vnode port given a vnode proxy.
 */
void
get_vnode_port_from_proxy(vpx, portp)
	struct vnode_proxy	*vpx;
	mach_port_t		*portp;
{
	kern_return_t		ret;

	ASSERT(vpx);
	VN_LOCK_PROXY(vpx);
	if (vpx->vpx_vnode != NULLVP) {
		get_vnode_port(vpx->vpx_vnode, portp);
	} else if (vpx->vpx_port != MACH_PORT_NULL) {
		ret = mach_port_mod_refs(mach_task_self(), vpx->vpx_port,
					 MACH_PORT_RIGHT_SEND, +1);
		if (ret != KERN_SUCCESS) {
		panic("get_vnode_port_from_proxy: mod_refs port=0x%x, ret=0x%x",
			vpx->vpx_port, ret);
		}
		*portp = vpx->vpx_port;
	}
	VN_UNLOCK_PROXY(vpx);
}

/*
 * Set up a vnode proxy given a vnode port.
 */
void
set_proxy_from_vnode_port(port, vpx)
	mach_port_t		port;
	struct vnode_proxy	*vpx;
{
	vpx->vpx_usecount = 1;
	vpx->vpx_port = port;
	PORT_TO_VNODE_LOOKUP(port, vpx->vpx_vnode);
}

/*
 * Sync a remote filesystem.
 */
int
remote_sync(vnode_port, tag)
	mach_port_t	vnode_port;
	int		tag;
{
#if     !FULLSERVER
        return(EINVAL);
#else   /* FULLSERVER */
	int			error;
        struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_sync(vnode_port, u.uu_procp->p_cred,
			oipp->oip_transid,
			tag);
	OIP_END_FORW(oipp);
	return(error);
#endif  /* FULLSERVER */
}


/*
 * Return the number of mounted filesystems on a remote node.
 */
int
remote_getfsstat_count(vnode_port, tag, countp)
	mach_port_t	vnode_port;
	int		tag;
	int		*countp;
{
#if     !FULLSERVER
        return(EINVAL);
#else   /* FULLSERVER */
	int			error;
        struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_getfsstat_count(vnode_port, u.uu_procp->p_cred, 
			oipp->oip_transid,
			tag, countp);
	OIP_END_FORW(oipp);
	return(error);
#endif  /* FULLSERVER */
}


/*
 * Get statfs structures for remote filesystems.
 */
int
remote_getfsstat(vnode_port, tag, bufp, maxentries, flags, countp)
	mach_port_t 	vnode_port;
	int		tag;
	char		**bufp;
	int		maxentries;
	int		flags;
	int		*countp;
{
#if     !FULLSERVER
        return(EINVAL);
#else   /* FULLSERVER */
	int			error;
        struct nameidata 	*ndp = &u.u_nd;
	char			*bufaddr;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_getfsstat(vnode_port, u.uu_procp->p_cred,
			oipp->oip_transid,
			maxentries * sizeof(struct statfs),
			flags, tag, &bufaddr, (mach_msg_type_number_t *)countp);
	OIP_END_FORW(oipp);
	if (error == 0) {
		ASSERT((*countp % sizeof(struct statfs)) == 0);
		/*
		 * Copy the data to the reply message.
		 */
		error = copyout((caddr_t)bufaddr, *bufp, *countp);
		/*
		 * Adjust the buffer address to account for the bytes
		 * copied out.  Also, return the number of statfs entries
		 * copied out.
		 */
		*bufp += *countp;

		/*
		 * deallocate ool memory allocated by fsvr_getfsstat
		 */
		if (bufaddr)
			vm_deallocate_strict(mach_task_self(),
					(vm_address_t)bufaddr, *countp);
		*countp /= sizeof(struct statfs);
	}
	return(error);
#endif  /* FULLSERVER */
}

/*
 * CLIENT SIDE ROUTINES FOR MOUNT RELATED OPERATIONS.
 */
int 
remote_unmount_set(mount_port)
        mach_port_t    mount_port;
{
	int 			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(mount_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, mount_port);
	error = fsvr_unmount_set(mount_port, u.uu_procp->p_cred,
			oipp->oip_transid);
	OIP_END_FORW(oipp);
	return(error);
}

int
remote_unmount_done(mount_port, inerror)
	mach_port_t	mount_port;
	int		inerror;
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(mount_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, mount_port);
	error = fsvr_unmount_done(mount_port, u.uu_procp->p_cred, 
				   oipp->oip_transid, inerror);
	OIP_END_FORW(oipp);
	return(error);
}

/*
 * Remote mount update.
 */
int
remote_mount_update(mount_port, flag)
	mach_port_t	mount_port;
	int		flag;
{
	int 			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(mount_port != MACH_PORT_NULL);
	flag = (flag & ~M_REMOTE_DIR) | M_REMOTE_FS;

	OIP_SET_FORW(oipp, mount_port);
	error = fsvr_mount_update(mount_port, u.uu_procp->p_cred,
				  oipp->oip_transid, flag);
	OIP_END_FORW(oipp);
	return(error);
}

#if     UFS
/*
 * Perform remote UFS mount operation.
 */
int
remote_ufs_mountfs(devfsport, mount_port, dir, spec, flag, remote_portp)
	mach_port_t	devfsport;
	mach_port_t	mount_port;
	char		*dir;
	char		*spec;
	int		flag;
	mach_port_t	*remote_portp;	/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(devfsport != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, devfsport);
	error = fsvr_ufs_mountfs(devfsport, u.uu_procp->p_cred,
			oipp->oip_transid,
			mount_port, dir, strlen(dir)+1,
			spec, strlen(spec)+1, flag,
			remote_portp);
	OIP_END_FORW(oipp);
	return(error);
}
#endif	/* UFS */

#ifdef	PFS
/*
 * Perform remote PFS mount operation.
 */
int
remote_pfs_mountfs(devfsport, mount_port, dir, spec, flag, stripe_attr,
		   remote_portp)
	mach_port_t	devfsport;
	mach_port_t	mount_port;
	char		*dir;
	char		*spec;
	int		flag;
	struct statpfs	*stripe_attr;
	mach_port_t	*remote_portp;	/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(devfsport != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, devfsport);
	error = fsvr_pfs_mountfs(devfsport, u.uu_procp->p_cred,
				 oipp->oip_transid,
				 mount_port, dir, strlen(dir)+1,
				 spec, strlen(spec)+1, flag,
				 (char_array)stripe_attr,
				 stripe_attr->p_reclen,
				 remote_portp);
	OIP_END_FORW(oipp);
	return(error);
}
#endif

#if NFS
/*
 * Perform remote NFS mount operation.
 */
int
remote_nfs_mountfs(netserverport, mount_port, pathname, flags, args,
	remote_portp)
	mach_port_t	netserverport;
	mach_port_t	mount_port;
	char		*pathname;
	int 		flags;
	nfs_args_t	*args;
	mach_port_t	*remote_portp;	/* out */
{
	int                     error;
        struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(netserverport != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, netserverport);
	error = fsvr_nfs_mountfs(netserverport, u.uu_procp->p_cred,
			 oipp->oip_transid, 
			 mount_port, pathname, strlen(pathname)+1, flags, 
			 *args, *args->addr, *args->fh,
			 args->hostname, strlen(args->hostname)+1,
			 remote_portp);
	OIP_END_FORW(oipp);
	return(error);
}
#endif /* NFS */


/*
 * CLIENT SIDE ROUTINES FOR REMOTE OPERATIONS ON SPECIAL FILES.
 */

/*
 * Perform the first open of a device represented by a remote special file
 * and establish device to special file mapping.
 */
int
remote_spec_open(fsvr_port, node, dev, type, mode, tag, vnode_port, file_portp,
		dev_portp)
	mach_port_t 	fsvr_port;
	node_t		node;
	dev_t		dev;
	enum vtype	type;
	int		mode;
	enum vtagtype	tag;
	mach_port_t	vnode_port;
	mach_port_t	*file_portp;	/* out */
	mach_port_t	*dev_portp;	/* out */
{
	int			error = KERN_SUCCESS;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(fsvr_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, fsvr_port);
	if (file_portp == (mach_port_t *) 0)
		error = fsvr_spec_open(fsvr_port, u.uu_procp->p_cred,
				oipp->oip_transid,
				node, dev, type, mode, tag,
				vnode_port, dev_portp);
	else
		error = fsvr_spec_fsopen(fsvr_port, u.uu_procp->p_cred,
				oipp->oip_transid,
				node, dev, type, mode, tag,
				vnode_port, file_portp, dev_portp);
	OIP_END_FORW(oipp);
	return(error);
}


/*
 * Open a already opened device represented by a remote special file
 * using the established device to special file mapping.
 */
int
remote_spec_vnode_open(vnode_port, mode, file_portp)
	mach_port_t 	vnode_port;
	int		mode;
	mach_port_t	*file_portp;	/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	if (file_portp == (mach_port_t *) 0)
		error = fsvr_spec_vnode_open(vnode_port,
				u.uu_procp->p_cred,
				oipp->oip_transid,
				mode);
	else
		error = fsvr_spec_vnode_fsopen(vnode_port,
				u.uu_procp->p_cred,
				oipp->oip_transid,
				mode, file_portp);
	OIP_END_FORW(oipp);
	return(error);
}


/*
 * Close a remote special file by marking the specinfo structure as closed
 * and releasing the reference on the real special file vnode.
 */

int
remote_spec_close(vnode_port, flag)
	mach_port_t	vnode_port;
	int		flag;
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;
	
	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_spec_close(vnode_port,  u.uu_procp->p_cred,
				oipp->oip_transid,
				flag);
	OIP_END_FORW(oipp);
	return(error);
}


/*
 * Remote clearalias function for remote special devices.
 */
int
remote_clearalias(vnode_port)
	mach_port_t	vnode_port;
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_clearalias(vnode_port,  u.uu_procp->p_cred,
			oipp->oip_transid);
	OIP_END_FORW(oipp);
	return(error);
}


/*
 * Reclaim a remote special file vnode.
 */
int
remote_spec_reclaim(vnode_port)
	mach_port_t	vnode_port;
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_spec_reclaim(vnode_port, u.uu_procp->p_cred,
			oipp->oip_transid);
	OIP_END_FORW(oipp);
	return(error);
}


/*
 * Read a remote device.
 */

int
remote_spec_read(vnode_port, uio, ioflag)
	mach_port_t		vnode_port;
	struct uio		*uio;
	int			ioflag;
{
	int			error, amount;
	struct nameidata 	*ndp = &u.u_nd;
	struct iovec		*iov = uio->uio_iov;
	char			*buf;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	/*
	 * Emulation library coverts all readvs to reads
	 */
	ASSERT(uio->uio_iovcnt == 1);
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_spec_read(vnode_port,  u.uu_procp->p_cred,
			oipp->oip_transid, uio->uio_offset,
			iov->iov_len, ioflag, &buf,
			(mach_msg_type_number_t *)&amount);
	OIP_END_FORW(oipp);
	if (!error) {
		/*
		 * Set uio fields to correct values.
		 */
		iov->iov_base = buf + amount;
		iov->iov_len -= amount;
		uio->uio_resid -= amount;
		uio->uio_offset += amount;
	}
	return(error);
}


/*
 * Write a remote device.
 */

int
remote_spec_write(vnode_port, uio, ioflag)
	mach_port_t		vnode_port;
	struct uio		*uio;
	int			ioflag;
{
	int			error, amount;
	struct nameidata 	*ndp = &u.u_nd;
	struct iovec		*iov = uio->uio_iov;
	char			*buf;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vnode_port != MACH_PORT_NULL);
	/*
	 * Emulation library coverts all readvs to reads
	 */
	ASSERT(uio->uio_iovcnt == 1);

	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_spec_write(vnode_port, u.uu_procp->p_cred,
				oipp->oip_transid, uio->uio_offset,
				ioflag, buf, iov->iov_len, &amount);
	OIP_END_FORW(oipp);
	if (!error) {
		/*
		 * Set uio fields to correct values.
		 */
		iov->iov_base = buf + amount;
		iov->iov_len -= amount;
		uio->uio_resid -= amount;
		uio->uio_offset += amount;
	}
	return(error);
}


/*
 * Remote spec_ioctl.  Uses generic interface.
 */

remote_spec_ioctl(spec_port, com, data, fflag, cred)
	mach_port_t	spec_port;
	int		com;
	caddr_t		data;
	int		fflag;
	struct ucred	*cred;
{
	struct nameidata	*ndp = &u.u_nd;
	int			error;
	int 			arg[6];
	int			rval[2];
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(spec_port != MACH_PORT_NULL);
	arg[0] = com;
	arg[1] = (int)data;
	arg[2] = fflag;
	arg[3] = arg[4] = arg[5] = 0;

	if (cred == NOCRED) {
		/* Not interruptible w/o cred port !!! */
		error = fsvr_vnode_forw(spec_port,
				MACH_PORT_NULL, 0,
				SPEC_IOCTL, arg, rval);
	} else {
		OIP_SET_FORW(oipp, spec_port);
		error = fsvr_vnode_forw(spec_port,
				u.uu_procp->p_cred,oipp->oip_transid,
				SPEC_IOCTL, arg, rval);
		OIP_END_FORW(oipp);
	}
	return(error);
}

/*
 * SERVER SIDE OPERATIONS TO SUPPORT exec()
 */

/*
 * Exec lookup message.  Just set up nameidata and call namei.
 * This very, very similar to S_fsvr_lookup but the message is received
 * on the fileserver port instead of the vnode port, for security reasons.
 */
int
S_fsvr_exec_lookup(fserver_port, cwd_port, creds_port, transid,
		   root_port, pathname, pathname_len, options, commandp,
		   command_lenp, vnode_portp)
	mach_port_t		fserver_port;
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	u_int			pathname_len;
	int			options;
	char			*commandp;	/* out */
	int			*command_lenp;	/* out */
	mach_port_t		*vnode_portp;	/* out */
{
	register int		error;
	struct nameidata	*ndp = &u.u_nd;
	register struct vnode	*vp = NULLVP;

	/*
	 *  Initialize all out parameters 
	 *  which are ports to MACH_PORT_NULL.
	 */

	*vnode_portp = MACH_PORT_NULL;

        error = start_execserver_op(cwd_port, root_port, MACH_PORT_NULL,
				    creds_port, transid, pathname,
				    pathname_len, 0, 0);
	if (error)
		return(error);
	ndp->ni_nameiop = options | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	error = namei(ndp);
	/*
	 * If a remote mount was detected, forward the lookup message.
	 */
	if (error == EREMOTE) {
		/*
		 * Crack the fserver_port for the fileserver.
		 */
		mach_port_t fserver_port;

		fserver_port = port_to_fileserver_port(ndp->ni_forwport); 
		if (fserver_port == MACH_PORT_NULL) {
			error = EBADPORT;
		} else {
			struct server_oip *oipp = &u.uu_oip;

			OIP_SET_FORW(oipp, ndp->ni_forwport);
			error = fsvr_exec_lookup(fserver_port,
						 ndp->ni_forwport, 
						 creds_port,
						 transid, 
						 root_port,
						 ndp->ni_ptr, 
						 (mach_msg_type_number_t)
						 	strlen(ndp->ni_ptr)+1, 
						 options,
						 commandp,
						 (mach_msg_type_number_t *)
							command_lenp,
						 vnode_portp);
			OIP_END_FORW(oipp);
		}
	} else if (!error) {
		*command_lenp = ndp->ni_dent.d_namlen;
		bcopy(ndp->ni_dent.d_name, commandp, *command_lenp);
		vp = ndp->ni_vp;
#ifdef PFS
		if (VIO_IS_PFS(vp)) {
			/*
			 * Server-initiated I/O is not currently supported on
			 * PFS files.  Set error; the reference acquired on the
			 * vnode by namei will be released at the out label.
			 */
			error = EFSNOTSUPP;
			goto out;
		}
#endif
		/*
		 * Get the port for the vnode, allocating a new one if
		 * necessary. Release the reference acquired by namei.
		 * The only reference is the one obtained by get_vnode_port.
		 */
		get_vnode_port(vp, vnode_portp);
		vrele(vp);
	}
#ifdef	PFS
out:
#endif
	error = end_execserver_op(cwd_port, root_port, MACH_PORT_NULL,
			error, 0);
	if (error) {
		if (vp != NULLVP)
			vrele(vp);
		if (*vnode_portp != MACH_PORT_NULL)
			mach_port_deallocate(mach_task_self(), *vnode_portp);
		*vnode_portp = MACH_PORT_NULL;
	} else
		ASSERT(*vnode_portp != MACH_PORT_NULL);
	return(error);
}


int
S_fsvr_exec_setup_suid(vnode_port, creds_port, transid, pflag,
		       suid, new_uid, new_gid, new_is_priv)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			pflag;
	boolean_t		suid;
	int			*new_uid;	/* out */
	int			*new_gid;	/* out */
	boolean_t		*new_is_priv;	/* out */
{
	struct nameidata        *ndp = &u.u_nd;
	struct vnode            *vp;
	int			error;
	
	if (suid) { 
		*new_uid = *new_gid = -1;
		*new_is_priv = FALSE;
	}
	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);

	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	error = do_exec_setup_suid(vp, pflag, suid, new_uid, new_gid,
				   new_is_priv);

	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, 0));
}

int
S_fsvr_inode_pager_setup(vnode_port, creds_port, transid,
		 is_text, can_cache, pager)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	boolean_t		is_text;
	boolean_t		can_cache;
	mach_port_t		*pager;		/* out */
{
#if	FULLSERVER
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	int			error;
	
	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	*pager = inode_pager_setup(vp, is_text, can_cache);
	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 
			error, 0);
	if (error && *pager != MACH_PORT_NULL) {
		mach_port_deallocate(mach_task_self(), *pager);
	}
	return(error);
#else	/* FULLSERVER */
	panic("S_fsvr_inode_pager_setup");
#endif  /* FULLSERVER */
}

int
S_fsvr_exec_check_vnode(vnode_port, creds_port, transid, access, text)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			*access;	/* in/out */
	int			*text;
{
	struct nameidata        *ndp = &u.u_nd;
	struct vnode            *vp;
	int			error;
	
        error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);

	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	*text = ((vp->v_flag & VTEXT) == 0 && vp->v_usecount != 1 &&
		 vp->v_wrcnt);

	VOP_ACCESS(vp, VREAD, u.u_cred, *access);

	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 
				  error, 0));
}

int
S_fsvr_vnread_inline(vnode_port, creds_port, transid,
		     len, offset, flag, buf, bufcount, resid)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			len;
	off_t			offset;
	int			flag;
	char			*buf;		/* out */
	int			*bufcount;	/* out */
	int			*resid;		/* out */
{
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL,MACH_PORT_NULL,
				     creds_port, transid, NULL, 0,
				     0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	error = vn_rdwr(UIO_READ, vp, (caddr_t)buf, len, offset, UIO_SYSSPACE,
			flag, u.u_cred, resid);
	if ((error == 0) && (len > *resid)) {
		*bufcount = len - *resid;
	} else {
		*bufcount = 0;
	}
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 
		error, 0));
}

int
S_fsvr_vnread_outline(vnode_port, creds_port, transid,
		      len, offset, flag, buf, bufcount, resid)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			len;
	off_t			offset;
	int			flag;
	char			**buf;		/* out, dealloc (ool memory) */
	int			*bufcount;	/* out, size of the above */
	int			*resid;		/* out */
{
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	int			error;
	
	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*buf = 0;
	*bufcount = 0;
	*resid = 0;		/* this needs zeroing too */

	/*
	 * Should do a start_fileserver_op and call vn_rdwr or read ...
	 */
	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL,MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * Allocate memory for the read.
	 */
	if (vm_allocate(mach_task_self(), (vm_address_t *)buf, len, TRUE) !=
		KERN_SUCCESS) {
		error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL,
					   MACH_PORT_NULL, ENOMEM, 0);
		return(error);
	}
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	error = vn_rdwr(UIO_READ, vp, (caddr_t)*buf, len, offset, UIO_SYSSPACE,
			flag, u.u_cred, resid);

	error = end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
			error, 0);

	if (*buf) {
		if (error) {
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *buf, len);
			*buf = NULL;
			*bufcount = *resid = 0;
		}  else {
			*bufcount = len - (*resid);
			len -= round_page(*bufcount);
			if (len > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*buf + round_page(*bufcount));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}		
	return(error);
}

int
S_fsvr_vnwrite(vnode_port, creds_port, transid,
	       offset, flag, buf, bufcount, resid)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	off_t			offset;
	int			flag;
	char			*buf;
	int			bufcount;
	int			*resid;		/* out */
{
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	int			error;
	kern_return_t		kr;
	
	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL,MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	error = vn_rdwr(UIO_WRITE, vp, buf, bufcount, offset, UIO_SYSSPACE,
			flag, u.u_cred, resid);

	/*
	 * WARNING:
	 *
	 * We don't want to deallocate the data buffer if the vn_rdwr failed.
	 *
	 * Specifically, if (error != 0), then the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(),
				   (vm_address_t)buf,
				   (vm_size_t)bufcount);
		if (kr != KERN_SUCCESS) {
		panic("S_fsvr_vnwrite: vm_deallocate(0x%x,%d) returns 0x%x\n",
				buf, bufcount, kr);
		}
	}
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, 0));
}

#ifdef PARACORE
int
S_fsvr_vnreaddir(vnode_port, creds_port, transid,
		 len, offset, buf, bufcount, resid)
	mach_port_t		vnode_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			len;
	off_t			*offset;	/* in/out */
	char			*buf;		/* out */
	int			*bufcount;	/* out */
	int			*resid;		/* out */
{
	struct nameidata	*ndp = &u.u_nd;
	struct vnode		*vp;
	int			error;

	error = start_vnodeserver_op(vnode_port, MACH_PORT_NULL,MACH_PORT_NULL,
				     creds_port, transid, NULL, 0,
				     0, 0);
	if (error)
		return(error);
	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;
	error = vn_readdir(vp, (caddr_t)buf, len, offset,
			   u.u_cred, resid);
	if ((error == 0) && (len > *resid)) {
		*bufcount = len - *resid;
	} else {
		*bufcount = 0;
	}
	return(end_vnodeserver_op(vnode_port, MACH_PORT_NULL, MACH_PORT_NULL, 
		error, 0));
}
#endif /* PARACORE */

/*
 * CLIENT SIDE ROUTINES FOR OPERATIONS SUPPORTING exec()
 */

int
remote_exec_lookup(ndp, vpxp)
	struct nameidata	*ndp;
	struct vnode_proxy	**vpxp;	/* out */
{
	int			error;
	char			command[MAXPATHLEN];
	int			command_len = MAXPATHLEN;
	boolean_t		abs_path, local_root, local_cwd, is_local;
	struct vnode		*vp = NULLVP;
	mach_port_t		vnode_port = MACH_PORT_NULL;
	mach_port_t		forwport, fserver_port = MACH_PORT_NULL;

	abs_path = (*ndp->ni_dirp == '/');
	local_cwd  = (ndp->ni_cdir != NULLVP);
	local_root = (ndp->ni_rdir != NULLVP);

	/*
	 * Set some fields in nameidata that will be needed in both
	 * paths through the code
	 */
	ndp->ni_ptr = ndp->ni_pnbuf = ndp->ni_dirp;

	if (abs_path && local_root  ||  !abs_path && local_cwd) {
		/*
		 * The name translation should start on this node.
		 * Set up the nameidata info and call namei.
		 */
		ndp->ni_allocbuf = 0;
		ndp->ni_vp2 = NULLVP;
		ndp->ni_forwport = MACH_PORT_NULL;
		ndp->ni_pathlen = strlen(ndp->ni_dirp)+1;
		ndp->ni_nameiop |= HASPATHBUF;
		is_local = TRUE;

		error = namei(ndp);

		forwport = ndp->ni_forwport;
	} else {
		forwport = (abs_path) ? ndp->ni_rdirport : ndp->ni_cdirport;
		is_local = FALSE;
	}

	if (error == EREMOTE || is_local == FALSE) {
		/*
		 * Crack the fserver_port for the fileserver.
		 */
		fserver_port = port_to_fileserver_port(forwport); 
		if (fserver_port == MACH_PORT_NULL) {
			error = EBADPORT;
		} else {
			struct server_oip	*oipp = &u.uu_oip;

			OIP_SET_FORW(oipp, fserver_port);

			error = fsvr_exec_lookup(fserver_port,
					forwport, 
					u.uu_procp->p_cred,
					oipp->oip_transid, 
					ndp->ni_rdirport,
					ndp->ni_ptr, 
					(mach_msg_type_number_t)
						(strlen(ndp->ni_ptr)+1), 
					ndp->ni_nameiop,
					command,
					(mach_msg_type_number_t *)&command_len,
					(mach_port_t *)&vnode_port );

			OIP_END_FORW(oipp);
			is_local = FALSE;
		}
	}
	if (error == ESUCCESS) {
		struct vnode_proxy	*vpx;

		remote_getnewvnode(&vpx);
		vpx->vpx_port = vnode_port;
		vpx->vpx_vnode = (is_local) ? ndp->ni_vp : NULLVP;
		*vpxp = vpx;
		if (!is_local) {
			bcopy(command, ndp->ni_dent.d_name, command_len);
			ndp->ni_dent.d_name[command_len] = '\0';
			ndp->ni_dent.d_namlen = command_len;
		}
#ifdef PFS
		if ((is_local) && (vpx->vpx_vnode->v_iomode == VIO_PFS)) {
			/*
			 * Server-initiated I/O is not currently supported on
			 * PFS files.  Release the reference acquired on the
			 * vnode by namei and set error.
			 */
			error = EFSNOTSUPP;
			if (vpx->vpx_vnode != NULLVP)
				vrele(vpx->vpx_vnode);
			if (vnode_port != MACH_PORT_NULL)
				mach_port_deallocate(mach_task_self(),
						     vnode_port);
			vpx->vpx_vnode = NULLVP;
			*vpxp = 0;
		}
#endif
	} else {
		*vpxp = 0;
	}
	if (is_local && ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	return(error);
}

int
remote_exec_setup_suid(vp, pflag, suid, new_uid, new_gid, new_is_priv)
	struct vnode_proxy *vp;
	int		pflag;
	boolean_t	suid;
	int		*new_uid;	/* out */
	int		*new_gid;	/* out */
	boolean_t	*new_is_priv;	/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	mach_port_t		vnode_port;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vp);
	/*
	 * If the fileserver port is local, don't send any
	 * messages.
	 */
	if (vp->vpx_vnode) {
		return (do_exec_setup_suid(vp->vpx_vnode, pflag,
				suid, new_uid, new_gid, new_is_priv));
	}

	vnode_port = vp->vpx_port;
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_exec_setup_suid(vnode_port, 
                                     u.uu_procp->p_cred, oipp->oip_transid,
                                     pflag, suid, new_uid, new_gid, 
                                     new_is_priv);
	OIP_END_FORW(oipp);
	return(error);
}


int
remote_exec_read(vp, base, len, offset, flag, resid)
	struct vnode_proxy *vp;
	char		*base;
	int		len;
	off_t		offset;
	int		flag;
	int		*resid;		/* out */
{
	ASSERT(vp);
	/*
	 * If the fileserver port is local, don't send any
	 * messages.
	 */
	if (vp->vpx_vnode) {
		return (vn_rdwr(UIO_READ, vp->vpx_vnode, (caddr_t)base, 
				len, offset,
				UIO_SYSSPACE, flag, u.u_cred, resid));
	} else {
		return (remote_vnrdwr(UIO_READ, vp->vpx_port, base, len, 
				      offset, flag, resid));
	}
}

int
remote_exec_check_vnode(vp, read_access)
	struct vnode_proxy	*vp;
	int			*read_access;	/* in/out */
{
        struct nameidata 	*ndp = &u.u_nd;
	mach_port_t		vnode_port;
	int			error;
	
	if (vp->vpx_vnode) {
		struct vnode *vnp = vp->vpx_vnode;

		VN_LOCK(vnp);
		error = ((vnp->v_flag & VTEXT) == 0 && vnp->v_usecount != 1 &&
			 vnp->v_wrcnt);
		VN_UNLOCK(vnp);

		/* look for read access to file  */
		VOP_ACCESS(vnp, VREAD, u.u_cred, *read_access);
	} else {
		struct server_oip	*oipp = &u.uu_oip;
		boolean_t		is_text;

		vnode_port = vp->vpx_port;
		OIP_SET_FORW(oipp, vnode_port);
		error = fsvr_exec_check_vnode(vnode_port,
                                              u.uu_procp->p_cred,
                                              oipp->oip_transid,
                                              read_access, &is_text);
		if (!error)
			error = is_text;
		OIP_END_FORW(oipp);
	}
	return(error);
}

int
remote_unp_vsockcreate(sockid, portp)
#ifdef TNC
	mach_port_t	sockid;
#else	/* TNC */
	caddr_t		sockid;
#endif	/* TNC */
	mach_port_t	*portp;
{
	int 			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_unp_vsockcreate(ndp->ni_forwport, u.uu_procp->p_cred,
			oipp->oip_transid, ndp->ni_rdirport,
			ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
			sockid, portp);
	OIP_END_FORW(oipp);
	return(error);
}


int
S_fsvr_unp_vsockcreate(cwd_port, creds_port, transid, root_port,
		pathname, pathname_len, sockid, vn_portp)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
#ifdef TNC
	mach_port_t		sockid;
#else	/* TNC */
	caddr_t			sockid;
#endif	/* TNC */
	mach_port_t		*vn_portp;		/* out */
{
	register int		error;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				creds_port, transid,
				pathname, pathname_len, 0, 0);
	if (error) 
		return(error);
	error = unp_vsockcreate(sockid, vn_portp);

	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
				0));
}


remote_unp_vsockread(sockid, portp)
	mach_port_t	*sockid;
	mach_port_t	*portp;
{
	int 			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_unp_vsockread(ndp->ni_forwport, u.uu_procp->p_cred,
			oipp->oip_transid, ndp->ni_rdirport,
			ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
			ndp->ni_nameiop, sockid, portp);
	OIP_END_FORW(oipp);
	return(error);
}



/*
 * Read socket address from vnode.  Just set up nameidata and call namei.
 * Too bad, this could have been part of fsvr_lookup.
 */
int
S_fsvr_unp_vsockread(cwd_port, creds_port, transid, root_port,
		pathname, pathname_len, options, sockid, vn_portp)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			options;
	mach_port_t		*sockid;		/* out */
	mach_port_t		*vn_portp;		/* out */
{
	register int		error;
	struct nameidata	*ndp = &u.u_nd;
	register struct vnode	*vp = NULLVP;

	*sockid = MACH_PORT_NULL;
	*vn_portp = MACH_PORT_NULL;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				creds_port, transid,
				pathname, pathname_len, 0, 0);
	if (error)
		return(error);

	ndp->ni_nameiop = options | HASPATHBUF;
	ndp->ni_segflg = UIO_SYSSPACE;
	error = namei(ndp);
	/*
	 * If a remote mount was detected, forward the lookup message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_unp_vsockread(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				options, sockid, vn_portp);
		OIP_END_FORW(oipp);
	} else if (!error) {
		vp = ndp->ni_vp;
		/*
		 * Get the port for the vnode, allocating a new one if
		 * necessary. Release the reference acquired by namei.
		 * The only reference is the one obtained by get_vnode_port.
		 */
		get_vnode_port(vp, vn_portp);
		vrele(vp);
		if (vp->v_type != VSOCK) {
			error = ENOTSOCK;
		} else {
			VOP_ACCESS(vp, VWRITE, ndp->ni_cred, error);
			if (error)
				goto out;
#ifdef TNC
			error = get_bind_port_from_vnode(vp, sockid);
#else
			*sockid = vp->v_socket;
#endif
		}
	}
out:
	error = end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				error, 0);
	if (error) {
		if (*vn_portp != MACH_PORT_NULL)
			mach_port_deallocate(mach_task_self(), *vn_portp);
		*vn_portp = MACH_PORT_NULL;
#ifdef TNC
		if (*sockid != MACH_PORT_NULL)
			mach_port_deallocate(mach_task_self(), *sockid);
		*sockid = MACH_PORT_NULL;
#else
		*sockid = 0;
#endif
	} else
		ASSERT(*vn_portp != MACH_PORT_NULL);
	return error;
}



#ifdef TNC
/* 
 *  Destroy the socket port send right stored in this vnode.
 */
remote_unp_unbind(vn_port)
	mach_port_t	vn_port;
{
	int 			error;
	struct nameidata 	*ndp = &u.u_nd;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_unp_unbind(vn_port, u.uu_procp->p_cred,
			oipp->oip_transid);
	OIP_END_FORW(oipp);
	return(error);
}


int 
S_fsvr_unp_unbind(
	mach_port_t		vn_port,
	mach_port_t		creds_port,
	transaction_id_t	transid)
{
	register int		error = ESUCCESS;
	register struct vnode	*vp;
	mach_port_t		sockid;
	struct nameidata	*ndp = &u.u_nd;

        error = start_vnodeserver_op(vn_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, 0, 0);
	if (error)
		return(error);

	/*
	 * start_vnodeserver_op left the vnode in ndp->ni_cdir.
	 */
	vp = ndp->ni_cdir;

	/*
	 *  Get rid of the send right.
	 */
	if ( (sockid = (mach_port_t) vp->v_socket) == MACH_PORT_NULL ) {
		UNDEBUG(U_BINDPORT,
			("S_fsvr_unp_unbind: vp 0x%x not bound\n", vp));
		goto out;
	}
	UNDEBUG(U_BINDPORT, ("S_fsvr_unp_unbind: vp 0x%x, bind port 0x%x\n",
			     vp, sockid));

	error = mach_port_deallocate(mach_task_self(), sockid);
	if (error != KERN_SUCCESS)
		UNDEBUG(U_RELOC,
			("S_fsvr_unp_unbind: mach_port_deallocate: kr 0x%x\n",
			 error));

	vp->v_socket = (struct socket *)MACH_PORT_NULL;

out:
	error = (end_vnodeserver_op(vn_port, MACH_PORT_NULL, MACH_PORT_NULL, 
				    error, 0));

	return(error);
}
#endif	/* TNC */



/* 
 * This call combines the function of fsvr_file_to_vnode and 
 * fsvr_inode_pager_setup. It is used by mmap to reduce 2 fsvr mesages
 * to 1 for each mmap call.
 */
int 
S_fsvr_inode_pager_setup_fport(file_port, creds_port, is_text, can_cache,
                               vtypep, fflagp, pagerp)
        
        mach_port_t		file_port;
        mach_port_t		creds_port;
        boolean_t		is_text;
        boolean_t		can_cache;
        int			*vtypep;	/* out */
        int                     *fflagp;        /* out */
	mach_port_t		*pagerp;	/* out */
{
#if	FULLSERVER
        struct file     *fp;
        int             error;
        struct vnode    *vp;

	error = start_fileserver_op(&fp, file_port, creds_port,
				    0, 0, 0);
	if (error) {
                *pagerp = MACH_PORT_NULL;
		return(error);
	}

	ASSERT(fp && fp->f_magic == F_MAGIC);
	vp = (struct vnode *)fp->f_data;
#ifdef	PFS
	/*
	 * Don't allow a PFS file to be mmap'd ... server-initiated I/O
	 * is not currently supported on PFS files.
	 */
	if (VIO_IS_PFS(vp)) {
		error = EFSNOTSUPP;
		goto out;
	}
#endif

	*vtypep = vp->v_type;
        *fflagp = fp->f_flag;

        *pagerp = inode_pager_setup(vp, is_text, can_cache);

#ifdef	PFS
out:
#endif        
	return (end_fileserver_op(fp, error, 0));
#else	/* FULLSERVER */
	panic("S_fsvr_inode_pager_setup_fport");
#endif 	/* FULLSERVER */
}
        

int
remote_inode_pager_setup_fport(file_port,  is_text, can_cache,
                               vtypep, fflagp, pagerp, nodep)

        mach_port_t		file_port;
        boolean_t		is_text;
        boolean_t		can_cache;
        int			*vtypep;	/* out */
        int                     *fflagp;        /* out */
	mach_port_t		*pagerp;	/* out */
	node_t			*nodep;		/* out */
{
	struct file		*fp;
	struct vnode		*vp;
        int                     error;

	*nodep = port_to_node(file_port);
	if (*nodep == this_node) {
		/* this is a bit ugly */
		fp = (struct file *) file_port;
		ASSERT(fp->f_magic == F_MAGIC);
		vp = (struct vnode *)fp->f_data;

#ifdef	PFS
		/*
		 * Don't allow a PFS file to be mmap'd ... server-initiated I/O
		 * is not currently supported on PFS files.
		 */
		if (VIO_IS_PFS(vp)) {
			return(EFSNOTSUPP);
		}
#endif
		*vtypep = vp->v_type;
		*fflagp = fp->f_flag;

		*pagerp = inode_pager_setup(vp, is_text, can_cache);
		return(ESUCCESS);

	} else {
		/* XXX shouldn't bother passing creds port */
		error = fsvr_inode_pager_setup_fport(file_port,
						     u.uu_procp->p_cred,
						     is_text, can_cache, 
						     vtypep, fflagp, pagerp);
		return(error);
	}
}

mach_port_t
remote_inode_pager_setup(vp, is_text, can_cache)
	struct vnode_proxy	*vp;
	boolean_t	is_text;
	boolean_t	can_cache;
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	mach_port_t		pager, vnode_port;
	struct server_oip	*oipp = &u.uu_oip;

	ASSERT(vp);
	/*
	 * If the fileserver port is local, don't send any
	 * messages.
	 */
	if (vp->vpx_vnode) {
		return (inode_pager_setup(vp->vpx_vnode, is_text, 
					  can_cache));
	}
	vnode_port = vp->vpx_port;

	/*
	 * XXX Don't bother making this op interruptible.  
	 */
	OIP_SET_FORW(oipp, vnode_port);
	error = fsvr_inode_pager_setup(vnode_port,
                                       u.uu_procp->p_cred,
                                       oipp->oip_transid,
                                       is_text, can_cache, &pager);
	OIP_END_FORW(oipp);
	/* XXX: panic on error?! */
	if (error) {
		panic("exec_inode_pager_setup: fsvr_inode_pager_setup()");
	}
	return(pager);
}


/*
 * This function implments remote vnode_pager_flush. It is used by by
 * vm_msync to flush pages for an object backed by a remote vnode pager.
 * This is executed by the file server when it gets a fsvr_vnode_pager_flush
 * message.
 */
int
S_fsvr_vnode_pager_flush(fsvr_port, pager, creds_port, offset, size, wait, 
			 should_clean, should_flush, lock_value)

        mach_port_t     fsvr_port;
        mach_port_t     creds_port;
	vm_offset_t	offset;
	vm_size_t	size;
	int		wait;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	lock_value;
{
#if	FULLSERVER
        int             error;
        
	error = start_fsvrport_op(fsvr_port, creds_port, 0, 0, 0);
	if (error)
		return(error);
	
	error = vnode_pager_flush(pager, offset, size, wait, should_clean, 
                                  should_flush, lock_value);

        if (!error) 
		/* consume on success */
		mach_port_deallocate(mach_task_self(), pager);

	return (end_fsvrport_op(error, 0));
#else	/* FULLSERVER */
	panic("S_fsvr_vnode_pager_flush");
#endif 	/* FULLSERVER */
}
        

/* 
 * This function is called by vm_msync to flush pages for an object 
 * backed by a remote vnode pager. This function is executed in the 
 * same server that executes the vm_msync call (i.e the process manager).
 */
kern_return_t
remote_vnode_pager_flush(node, pager, offset, size, wait, should_clean,
                         should_flush, lock_value)
	node_t		node;
        mach_port_t     pager;
	vm_offset_t	offset;
	vm_size_t	size;
	int		wait;
	boolean_t	should_clean;
	boolean_t	should_flush;
	vm_prot_t	lock_value;
{ 
        mach_port_t     	fsvr_port;
        int  		        error;

	if (node == this_node) {
		/*
		 * Just handle locally.
		 */
		error = vnode_pager_flush(pager, offset, size, wait, 
					  should_clean, should_flush, 
					  lock_value);
	} else {
		fsvr_port = node_to_fileserver_port(node);
		ASSERT(fsvr_port != MACH_PORT_NULL);

		/* XXX get rid of creds port arg */
		error = fsvr_vnode_pager_flush(fsvr_port, pager,
					       u.uu_procp->p_cred,
					       offset, size, wait, 
					       should_clean, should_flush, 
					       lock_value);
	}

        return(error);
}


int
S_fsvr_vnode_pager_get(fsvr_port, creds_port, can_cache, size, pagerp)
        mach_port_t     fsvr_port;
        mach_port_t     creds_port;
	boolean_t	can_cache;
	vm_size_t	size;
        memory_object_t *pagerp;
{
#if	FULLSERVER
        int             error;

	error = start_fsvrport_op(fsvr_port, creds_port, 0, 0, 0);
	if (error)
		return(error);
	
        *pagerp = vnode_pager_get(can_cache, size);

        return(end_fsvrport_op(error, 0));
#else	/* FULLSERVER */
	panic("S_fsvr_vnode_pager_get");
#endif	/* FULLSERVER */
}

node_t
get_vnode_pager_node()
{

	/* the default case is to just return the local vnode pager */
	if (!import_paging) {
		return(this_node);
	} else {
		return(pager_node);
	}
}

/* 
 * This function is called for getting a memory_object_t (*pagerp) serviced
 * by a remote vnode pager. As part of boot-time 
 * configuration information we get info on which node is providing vnode_pager
 * services for anonymous memory for any given node. It makes remote call
 * to the fileserver (using the node's fileserver port) on the node which 
 * is providing the remote vnode paging for anonymous memory on 'node_id'
 */
kern_return_t
remote_vnode_pager_get(can_cache, size, pagerp, nodep)
	boolean_t	can_cache;
	vm_size_t	size;
        memory_object_t *pagerp;
        node_t		*nodep;
{
        int                     error;

        *nodep = get_vnode_pager_node();
	
	if (*nodep == this_node) {
#if	FULLSERVER
		*pagerp = vnode_pager_get(can_cache, size);
		return(ESUCCESS);
#else	/* FULLSERVER */
		panic("remote_vnode_pager_get");
#endif  /* FULLSERVER */
	} else {
		/* XXX get rid of creds port arg */
		error = fsvr_vnode_pager_get(node_to_fileserver_port(*nodep),
					     MACH_PORT_NULL,
					     can_cache, size, pagerp);
		return(error);
	}

}


int
S_fsvr_getmntid(fsvr_port, creds_port, transid, node, rdev, mntid)
        mach_port_t       fsvr_port;
        mach_port_t       creds_port;
	transaction_id_t  transid;
        node_t            node;
        dev_t             rdev;
        ulong_t           *mntid;
{
        int             error = ESUCCESS;

	error = start_fsvrport_op(fsvr_port, creds_port, transid, 0, 0);
	if (error)
		return(error);

	*mntid = -1;
#if     FULLSERVER	
        getmntid(node, rdev, mntid);
#endif  /* FULLSERVER */

        return(end_fsvrport_op(error, 0));
}

int
remote_getmntid(vp, devnode, rdev, mntid)
        struct vnode 		*vp;
	node_t			devnode;
        dev_t      		rdev;
        ulong_t    		*mntid;
{
        mach_port_t        	fsvr_port;
	node_t			fsvr_node;		
        int                	error;
	struct server_oip  	*oipp = &u.uu_oip;

	fsvr_node = vnode_to_fileserver_node(vp);
	if (fsvr_node == this_node)
		error = getmntid(devnode, rdev, mntid);
	else {
		fsvr_port = node_to_fileserver_port(fsvr_node);
		if (!MACH_PORT_VALID(fsvr_port))
			return ENODEV;
		OIP_SET_FORW(oipp, fsvr_port);
		error = fsvr_getmntid(fsvr_port, u.uu_procp->p_cred, 
				      oipp->oip_transid,
				      (int)devnode, (int)rdev,
				      (unsigned *)mntid);
		OIP_END_FORW(oipp);
	}

        return(error);
}

#if NFS
int 
S_fsvr_nfsrv_op(fsvr_port, creds_port, transid, procid, cr, md_arr,
                md_arrsz, doff, retxid, mreq_arr, mreq_arrsz, 
                mreq_dealloc, repstat)
        mach_port_t      fsvr_port;
        mach_port_t      creds_port;
	transaction_id_t transid;
        int              procid;
        struct ucred     cr;
        char             *md_arr;
        unsigned int     md_arrsz;
        int              doff;
        int              retxid;
        char             **mreq_arr;
        int              *mreq_arrsz;
        boolean_t        *mreq_dealloc;
        int              *repstat;
{
        int              error=0;
        struct mbuf      *mreq, *mrep, *md;
        int              temp_doff;
        caddr_t          dpos = NULL;
        kern_return_t     kr;
        boolean_t         mdarr_dealloc;

        if (error = start_fsvrport_op(fsvr_port, creds_port, transid, 0, 0)) {
                return(error);
        }

        /* convert md_arr into mbuf structures */
        /* convert dpos from offset into md_arr struct to caddr_t */

        msg2mbuf(md_arr, md_arrsz, doff, &md, &dpos, &mdarr_dealloc);

        mrep = md;

        if (mdarr_dealloc) {
                if ((kr = vm_deallocate(mach_task_self(),
				(vm_address_t)md_arr,
				(vm_size_t)md_arrsz)) != KERN_SUCCESS)
                     panic("fsvr_nfsrv_op:unable to vm_deallocate md_arr:0x%x\n",
                              kr);
        }
        
        error = (*(nfsrv_procs[procid]))(mrep, md, dpos, &cr, retxid,
                                         &mreq, repstat);
        
        /* convert mreq to an array */
        mbuf2msg(mreq, NULL, mreq_arr, mreq_arrsz, &temp_doff);
        
        /* 
         * tell mach_msg to vm_deallocate the mreq_arr region after it has 
         * been mapped in the receiver's address space - This takes care of
         * deallocating the memory (mreq_arr) vm_allocated in mbuf2msg
         */
        *mreq_dealloc = TRUE;
        m_freem(mreq);
        
	error = end_fsvrport_op(error, 0);
        return(error);
}                                         


int 
remote_nfsrv_op(node, procid, mrep, md, dpos, cr, retxid, mreq, repstat)
        node_t         node;
        int            procid;
        struct mbuf    *mrep;
        struct mbuf    *md;
        caddr_t        dpos;
        struct ucred   *cr;
        u_long         retxid;
        struct mbuf    **mreq;
        int            *repstat;
{
        caddr_t        temp_dpos;
        char           *md_arr, *mreq_arr;
        int            md_arrsz, mreq_arrsz, doff;
        int            error;
        kern_return_t  kr;
        boolean_t      md_dealloc;
        mach_port_t        fsvr_port;
	struct server_oip  *oipp = &u.uu_oip;
	cred_t		*ucr = (cred_t *) cr;
        boolean_t       mreqarr_dealloc;

        fsvr_port = node_to_fileserver_port(node);
        if (fsvr_port == NULL) {
                /* 
                 * this may just be an an old NFS request pending 
                 */
                return(ESTALE);
        }

	OIP_SET_FORW(oipp, fsvr_port);

        /* translate mrep and md mbufs to an array of bytes rather than 
         * mbuf structures 
         * Also dpos has to be translated from an addr to an offset into 
         * md.
         */
        /*
         * Now although there are two mbuf pointer in the request md and mrep
         * md is actually where the real info for the server operation
         * begins.
         * Most of the time mrep == md, however when there is more control
         * info in the message, like the uid, gid, groups info which actually
         * is already extracted by nfs_getreq, md may actaully be advanced
         * to the next mbuf in the chain.  
         * So we will only send the mbufs starting from md across to
         * the slave node. 
         */

        mbuf2msg(md, dpos, &md_arr, &md_arrsz, 
                                     &doff);
        ASSERT(md_arr != 0);

        /*
         * tells mach_msg to deallocate the md_arr reqion after it has 
         * been mapped in the receiver's address space. This takes care of
         * deallocating memory vm_allocated in mbuf2msg
         */
        md_dealloc = TRUE;

        /*
         * free the mbuf once we have copied the data into md_arr. Note
         * that we free mrep since that is the start of our mbuf chain.
         */
        m_freem(mrep);

        error = fsvr_nfsrv_op(fsvr_port, u.uu_procp->p_cred, oipp->oip_transid,
			      procid, *ucr, md_arr, md_arrsz, md_dealloc,
			      doff, retxid, &mreq_arr, 
			      (mach_msg_type_number_t *)&mreq_arrsz,
			      repstat);

        /* convert mreq_arr back to an mbuf */
        msg2mbuf(mreq_arr, mreq_arrsz, -1, mreq, &temp_dpos, &mreqarr_dealloc);

        /* 
         * deallocate once we copy it into the mbuf 
         */
        if (mreqarr_dealloc) {
                if ((kr = vm_deallocate(mach_task_self(),(vm_address_t)mreq_arr,
				(vm_size_t)mreq_arrsz)) !=  KERN_SUCCESS) 
                panic("remote_nfsrv_op:unable to vm_deallocate mreq_arr:0x%x\n",
                        kr);
        }

        return(error);
}
#endif /* NFS */

/*
 * SERVER SIDE FOR TNC FILE PORT OPERATIONS
 */

int
S_fsvr_report_migrate(file_port, cred_port, pid, new_node, status)
	mach_port_t	file_port;
	mach_port_t	cred_port;
	pid_t		pid;
	int		new_node;
	caddr_t		*status;
#ifndef	TNC
{ return (ESUCCESS); }
#else
{
	int error;
	struct file *fp;
	extern int un_ff_notify(struct vnode *, pid_t, node_t);
	extern int vsdebug;

	*status = (caddr_t)0;	/* not yet used... */

	/*
	 *  Things I might have to do (?):
	 *  - Use some credential mechanism to insure this is not
	 *    a bogus report.
	 */

	error = start_fileserver_op(&fp, file_port, cred_port,
				    0 /*transid*/,
				    0 /*syscode*/,
				    0 /*serial*/);
	if ( error != KERN_SUCCESS )
		return error;

#ifdef NX
	new_node = nx_map_node_pid(pid, new_node);
	if (new_node == -1) {
	    return EINVAL;
	}
#endif /* NX */

	switch ( fp->f_type ) {
	case DTYPE_SOCKET:
		VSDEBUG( VSDEBENTRY,
		  ("notifying so 0x%x, pid %d, node %d, vsops 0x%x, rtn 0x%x\n",
		   (struct socket *)fp->f_data, pid, new_node, 
		   ((struct socket *)fp->f_data)->vs_ops,
		   (((struct socket *)fp->f_data)->vs_ops ? 
		   ((struct socket *)fp->f_data)->vs_ops->vsop_notify 
		   : NULL)));
		error = VSOP_NOTIFY((struct socket *)fp->f_data, 
				    pid, new_node);
		break;
	case DTYPE_VNODE:
		if ( ((struct vnode *)fp->f_data)->v_type == VFIFO ) {
			error = un_ff_notify((struct vnode *)fp->f_data,
					     pid, new_node);
		} else {
			error = EBADF;
		}
		break;
	default:
		error = EBADF;
		break;
	}

	end_fileserver_op(fp, error, 0 /*serial*/);
	return error;
}
#endif	/* TNC */

remote_getattr(vport, vap)
	mach_port_t	vport;
	struct vattr	*vap;
{
	int error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, vport);
	error = fsvr_getattr(vport, u.uu_procp->p_cred, oipp->oip_transid, 
							(vattr_t *) vap);
	OIP_END_FORW(oipp);
	return(error);
}

S_fsvr_getattr(vport, creds_port, transid, vap)
	mach_port_t		vport;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	vattr_t			*vap;

{
	int	error;
	int 			syscode = SYS_fstat;
	int 			serial = !sysent[syscode].sy_parallel;
	struct 	nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, serial);
	if (error)
		return(error);

	ASSERT(ndp->ni_cdir != NULLVP);
        VOP_GETATTR(ndp->ni_cdir, vap, u.uu_procp->p_rcred, error);

	return(end_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}

remote_setattr(vport, vap)
	mach_port_t	vport;
	vattr_t		*vap;
{
#if     !FULLSERVER
        return(EINVAL);
#else   /* FULLSERVER */
	int error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, vport);
	error = fsvr_setattr(vport, u.uu_procp->p_cred, oipp->oip_transid, 
			     *vap);
	OIP_END_FORW(oipp);
	return(error);
#endif  /* FULLSERVER */
}

S_fsvr_setattr(vport, creds_port, transid, vap)
	mach_port_t		vport;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	vattr_t			vap;

{
	int	error;
	int 			syscode = SYS_fstat;
	int 			serial = !sysent[syscode].sy_parallel;
	struct 	nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, serial);
	if (error)
		return(error);

	ASSERT(ndp->ni_cdir != NULLVP);
        VOP_SETATTR(ndp->ni_cdir, &vap, u.uu_procp->p_rcred, error);

	return(end_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}

int
remote_vnrdwr(rw, vp, base, len, offset, ioflg, aresid)
	enum uio_rw	rw;
	mach_port_t	vp;
	char		*base;
	int		len;
	off_t		offset;
	int		ioflg;
	int		*aresid;		/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	char			*buf;
	int			bufcount;
	struct server_oip	*oipp = &u.uu_oip;
	mach_port_t		cred = u.uu_procp->p_cred;
	int resid;

	OIP_SET_FORW(oipp, vp);
	bufcount = len;
	if (rw == UIO_READ) {
		if (len <= SMALL_ARRAY_LIMIT) {
			error = fsvr_vnread_inline(vp, cred,
						   oipp->oip_transid,
						   len, offset, ioflg, base, 
						   (mach_msg_type_number_t *)
							&bufcount,
						   &resid);
		} else {
			error = fsvr_vnread_outline(vp, cred,
						    oipp->oip_transid,
						    len, offset, ioflg, &buf, 
						    (mach_msg_type_number_t *)
							&bufcount,
						    &resid);
			if (error == 0 && bufcount > 0) {
				bcopy (buf, base, bufcount);
				(void) vm_deallocate(mach_task_self(),
						     (vm_address_t)buf,
						     (vm_size_t)bufcount);
			}
		}
	} else {
		error = fsvr_vnwrite(vp, cred, oipp->oip_transid, offset,
				     ioflg, base, bufcount, &resid);
	}
	OIP_END_FORW(oipp);
	if (error == 0 && aresid) {
		*aresid = resid;
	}
	return(error);
}

#ifdef PARACORE
int
remote_vnreaddir(vp, base, len, offset, aresid)
	mach_port_t	vp;
	char		*base;
	int		len;
	off_t		*offset;		/* in/out */
	int		*aresid;		/* out */
{
	int			error;
	struct nameidata 	*ndp = &u.u_nd;
	char			*buf;
	int			bufcount;
	struct server_oip	*oipp = &u.uu_oip;
	mach_port_t		cred = u.uu_procp->p_cred;
	int 			resid;

	OIP_SET_FORW(oipp, vp);
	bufcount = len;
	if (len <= SMALL_ARRAY_LIMIT) {
		/* inline */
		error = fsvr_vnreaddir(vp, cred,
				       oipp->oip_transid,
				       len, (int *)offset, base, 
				       (mach_msg_type_number_t *) &bufcount, 
				       (int *) &resid);
	} else {
		/* outline, not implemented */
		printf("remote_vnreaddir: SMALL_ARRAY_LIMIT exceeded\n");
		error = EINVAL;
	}
	OIP_END_FORW(oipp);
	if (error == 0 && aresid) {
		*aresid = resid;
	}
	return(error);
}
#endif /* PARACORE */

extern	int		vnode_pager_is_set;
extern	mach_port_t	root_fs_port;
extern	mach_port_t	pager_fs_port;
extern	mach_port_t     vnode_pager_default;

kern_return_t
remote_vnode_pager_get_state(has_backingp, pager_portp)
	int		*has_backingp;
	mach_port_t	*pager_portp;
{
	int	error;
	error = fsvr_vnode_pager_get_state(pager_fs_port, 
				has_backingp, pager_portp);
	return(error);
}


kern_return_t
S_fsvr_vnode_pager_get_state(fsvr_port, has_backingp, pager_portp)
	mach_port_t	fsvr_port;
	int		*has_backingp;
	mach_port_t	*pager_portp;
{
#if	FULLSERVER
	*has_backingp = vnode_pager_is_set;
	if (vnode_pager_is_set == TRUE) {
		 *pager_portp = vnode_pager_default;
	} else {
		 *pager_portp = MACH_PORT_NULL;
	}
	return(KERN_SUCCESS);

#else	/* FULLSERVER */
	panic("S_fsvr_vnode_pager_get_state");
#endif	/* FULLSERVER */
}

#if	REMOTE_PROC

kern_return_t
remote_rproc_server_register(node)
	node_t	node;
{
	int	error;
	error = fsvr_rproc_server_register(root_fs_port, node);
	return(error);
}

kern_return_t
S_fsvr_rproc_server_register(fsvr_port, node)
	mach_port_t	fsvr_port;
	node_t		node;
{
	rproc_server_register(node);
	return(KERN_SUCCESS);
}

#endif	/* REMOTE_PROC */

kern_return_t
remote_server_register(node)
	node_t	node;
{
	int	error;
	error = fsvr_server_register(root_fs_port, node);
	return(error);
}

kern_return_t
S_fsvr_server_register(fsvr_port, node)
	mach_port_t	fsvr_port;
	node_t		node;
{
	server_register(node);
	return(KERN_SUCCESS);
}

/*
 * called during shutdown by root_fs_node to halt another node.
 */
kern_return_t
remote_server_halt(node, options)
	node_t node;
	int options;
{
	mach_port_t	port;
	int error = KERN_SUCCESS;

	port = node_to_fileserver_port(node);
	if (port != MACH_PORT_NULL)
		error = fsvr_halt(port, options);
	return(error);
}

kern_return_t
S_fsvr_halt(port, options)
	mach_port_t port;
	int options;
{
	struct uthread	*uth = &u;
	(void) fsvr_thread_initialize(uth, MACH_PORT_NULL);
	printf("Rebooting at request of root_fs_node\n");
	pps_boot(RB_BOOT, options);
	printf("fsvr_halt: got past boot???\n");
	fsvr_thread_terminate(&u, KERN_SUCCESS);
	return(KERN_SUCCESS);
}

/*
 * This routine is called during fstat and close of a remote device
 * from the node servicing the remote device to the node holding
 * the device's inode so the modification flags can be passed on.
 */
remote_updateinfo(vport, accessed, modified, vap)
	mach_port_t	vport;
	int		accessed;
	int		modified;
	struct vattr	*vap;
{
	int error;
	struct server_oip	*oipp = &u.uu_oip;

	OIP_SET_FORW(oipp, vport);
	error = fsvr_updateinfo(vport, u.uu_procp->p_cred, oipp->oip_transid, 
							accessed,
							modified,
							(vattr_t *) vap);
	OIP_END_FORW(oipp);
	return(error);
}

/*
 * Runs on the node holding the device file (real inode)
 */
S_fsvr_updateinfo(vport, creds_port, transid, accessed, modified, vap)
	mach_port_t		vport;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			accessed;
	int			modified;
	vattr_t			*vap;

{
	int			error;
	int 			syscode = SYS_fstat;
	int 			serial = !sysent[syscode].sy_parallel;
	struct uthread		*uth = &u;
	struct 	nameidata 	*ndp = &uth->uu_nd;

	error = start_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, serial);
	if (error)
		goto out;

	/* assert right kind of vnode */
	ASSERT(ndp->ni_cdir != NULLVP);
#if FULLSERVER
	ASSERT(ndp->ni_cdir->v_op == &spec_inodeops);
#endif
	if (accessed == TRUE || modified == TRUE) {
        	VOP_UPDATE(ndp->ni_cdir, accessed, modified, -1, 0,
			   uth->uu_procp->p_rcred, error);
		if (error)
			goto out;
	}
       	VOP_GETATTR(ndp->ni_cdir, vap, uth->uu_procp->p_rcred, error);
out:
	return(end_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}

/*
 * This routine is called during ufs_getattr, for a remote device
 * in order to obtain access/modified flags from the remote node.
 */
remote_getinfo(vp, vport)
	struct vnode	*vp;
	mach_port_t	vport;
{
	struct uthread		*uth = &u;
	struct server_oip	*oipp = &uth->uu_oip;
	int 			accessed, modified, error;

#if FULLSERVER
	ASSERT(vp->v_op == &spec_inodeops);
#endif

	OIP_SET_FORW(oipp, vport);
	error = fsvr_getinfo(vport, uth->uu_procp->p_cred, oipp->oip_transid, 
			     &accessed, &modified);
	OIP_END_FORW(oipp);
	if (!error && (accessed || modified))
		/* update the state to the underlying file system */
		VOP_UPDATE(vp, accessed, modified, -1, 0, 
			   uth->uu_procp->p_rcred, error);

	return(error);
}

S_fsvr_getinfo(vport, creds_port, transid, accessedp, modifiedp)
	mach_port_t		vport;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			*accessedp;
	int			*modifiedp;
{
	int			error;
	int 			syscode = SYS_stat;
	int 			serial = !sysent[syscode].sy_parallel;
	struct 	nameidata 	*ndp = &u.u_nd;

	error = start_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0, 0, serial);
	if (error)
		goto out;

	/* assert right kind of vnode */
	ASSERT(ndp->ni_cdir != NULLVP);
	ASSERT(ndp->ni_cdir->v_op == &spec_rmtnodeops);
       	error = rmtspec_getinfo(ndp->ni_cdir, accessedp, modifiedp);

out:
	return(end_vnodeserver_op(vport, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}

/*
 * This routine is called during swapon of a block device when
 * the device is being serviced by a fileserver remote with
 * respect to the server holding the device inode. The call
 * is initiated in swapon and will cause the remote server's
 * vnode pager to be initialized.
 */
remote_pf_file_init(vp, lowat, hiwat, prefer, name)
	struct vnode	*vp;
	int		lowat;
	int		hiwat;
	int		prefer;
	char		*name;
{
	int		type;
	dev_t		dev;
	int		node;
	int 		error = 0;

	VN_LOCK(vp);
	type = vp->v_type;
	if ((type == VBLK) && ((node = vnode_to_fileserver_node(vp)) 
						!= this_node)) {
		struct proc	*p;
		mach_port_t	rport;
		node_t		dev_node;
		enum vtagtype	tag;

		tag = vp->v_tag;
		dev_node = vp->v_devnode;
		p = u.u_procp;
		dev = vp->v_specinfo->si_rdev;
		VN_UNLOCK(vp);
		rport = node_to_fileserver_port(node);
		error = fsvr_pf_file_init(rport, p->p_cred, dev, tag, 
					dev_node, lowat, hiwat, prefer, 
					VBLK, name, strlen(name) + 1);
	} else {
		VN_UNLOCK(vp);

		error = internal_pf_file_init(vp, lowat, hiwat, name, prefer);
	}
	return error;
}

S_fsvr_pf_file_init(fsvr_port, creds_port, dev, tag, dev_node, lowat, hiwat, 
				prefer, type, name, namelen)
	mach_port_t	fsvr_port;
	mach_port_t	creds_port;
	dev_t		dev;
	enum vtagtype	tag;
	node_t		dev_node;
	int		lowat;
	int		hiwat;
	int		prefer;
	enum vtype      type;
	char		*name;
	int		namelen;
{
#if	FULLSERVER
	int			error;
	struct vnode		*specvp;
	struct specinfo		*si;
	struct 	nameidata 	*ndp = &u.u_nd;
	extern int 		remote_vnode_agents;
	extern struct mutex 	rmt_agent_lock;
	extern rmtspec_sync();
	struct rmtnode *rp;

	error = start_fsvrport_op(fsvr_port, creds_port, 0, 0, 0);
	if (error)
		return(error);

	/*
	 * Allocate a new device vnode to act as a agent of the special file
	 * vnode on the remote.
	 */
	bdevvp(dev, dev_node, type, &specvp);

	if ((tag != VT_UFS) || (type != VBLK)) {
		return(EINVAL);
	}

	si = specvp->v_specinfo;
	rp = (struct rmtnode *)(specvp->v_data);
	specvp->v_op = &spec_rmtnodeops;
	bzero((caddr_t)rp, sizeof(struct rmtnode));
	RMT_LOCK_INIT(rp);

	/*
	 * Initialize vnode pager
	 */
	error = internal_pf_file_init(specvp, lowat, hiwat, name, prefer);
	ASSERT(error != EREMOTEPORT);
	error = end_fsvrport_op(error, 0);
	if (error) {
		vrele(specvp);
		return(error);
	}
	return(0);
#else	/* FULLSERVER */
	panic("S_fsvr_pf_file_init");
#endif	/* FULLSERVER */
}

/* 
 * get paging stats for all nodes in the domain. We will contact all nodes in
 * pager_node_list. This list is built during parsing of the boot config. 
 * It basically is the set of
 *      (all node) - (nodes importing paging) + (nodes exporting paging)
 *
 * For each paging file (vnode/kernel) an entry is filled in 'pginfo'. Atmost
 * 'pgisze' entries are filled in. The actual # of entries written in pginfo is
 * returned in 'retsize'. If 'size_only' is true, no entries are filled - 
 * just a count of the # of entries is returned. The caller can therefore 
 * first call this function with size_only set to true to determine the 
 * space that needs to be allocated for pginfo. Subsequent call would 
 * have 'size_only' set to false and sufficient space allocated for pginfo.
 */
int
remote_get_paging_stats_all(size_only, pginfo, pgsize, retsize)
        boolean_t            size_only;
        struct tbl_pginfo_10 *pginfo;
        int                  pgsize;
        int                  *retsize;
{
        int i, rsize;

        *retsize = 0;
        for (i=0; i < pager_node_list_ent; i++) {
                if (pager_node_list[i] != NONODE) {
                        if (!remote_get_paging_stats(pager_node_list[i], 
                                                     size_only, pginfo, 
                                                     pgsize, &rsize)) {
                                *retsize += rsize;
                                if (!size_only) {
                                        pginfo+= rsize;
                                        pgsize-= rsize;
                                        if (pgsize <= 0) {
                                                return(0);
                                        }
                                }
                        }
                }
        }
	return(0);
}

/* 
 * get paging stats for 'node'. 
 *
 * For each paging file (vnode/kernel) an entry is filled in 'pginfo'. Atmost
 * 'pgisze' entries are filled in. The actual # of entries written in pginfo is
 * returned in 'retsize'. If 'size_only' is true, no entries are filled - 
 * just a count of the # of entries is returned. The caller can therefore 
 * first call this function with size_only set to true to determine the space 
 * that needs to be allocated for pginfo. Subsequent call would have 
 * 'size_only' set to false and sufficient space allocated for pginfo.
 *
 * Return : error code
 */
int
remote_get_paging_stats(node, size_only, pginfo, pgsize, retsize)
        node_t               node;
        boolean_t            size_only;
        struct tbl_pginfo_10 *pginfo;
        int                  pgsize; 
	int                  *retsize;
{
	struct uthread		*uth = &u;
	struct server_oip	*oipp = &uth->uu_oip;
        int                     error=0;
        void                    *tmp_pginfo;
        int                     tmp_pgsize_bytes;
        mach_port_t             fsvr_port;

        *retsize = 0;
        if (node == this_node) {
                if (size_only) {
                        return(get_pginfo_size_local(retsize));
                } else {
                        return(get_pginfo_local(pginfo, pgsize, retsize));
                }
        }

        /* send a message to the fileserver port on the node */
        if ((fsvr_port = node_to_fileserver_port(node)) == MACH_PORT_NULL) {
                return(EINVAL);
        }
	OIP_SET_FORW(oipp, fsvr_port);
        error = fsvr_get_paging_stats(fsvr_port, uth->uu_procp->p_cred,
                                      oipp->oip_transid, size_only,
                                      retsize,
                                      (char_array *)&tmp_pginfo,
                                      (mach_msg_type_number_t *)&tmp_pgsize_bytes);
	OIP_END_FORW(oipp);

        if (size_only) {
                return(error);
        }

        if (!error) {
                *retsize = MIN(*retsize, pgsize);
                /* copy the pginfo into our local buffer */
                bcopy(tmp_pginfo, pginfo, 
                      *retsize * sizeof(struct tbl_pginfo_10));
        } else {
                *retsize = 0;
        }
        return(error);
}

/* 
 * Server function for paging file info - This function is called by 
 * remote_get_paging_stats. This function does not forward the operation onto 
 * another node.
 *
 * Entries corresponding to paging files on the current node are returned in
 * the 'pginfo' structure. 'pginfo' is an out argument to this function so 
 * space for it is allocated by this function. 'num_entries' returns the actual
 * number of entries filled in 'pginfo'. pgsize_bytes is teh retunred size of 
 * 'pginfo' in bytes.
 */
int
S_fsvr_get_paging_stats(fsvr_port, creds_port, transid, size_only,
                        num_entries, pginfo, pgsize_bytes)
        mach_port_t      fsvr_port;
        mach_port_t      creds_port;
        transaction_id_t transid;
        boolean_t        size_only;
        int              *num_entries;
        void             **pginfo;  	/* out, dealloc (ool memory) */
        int              *pgsize_bytes;	/* out, size of the above */
{
#if	FULLSERVER
        int error = 0;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
        *pginfo = NULL;
        *pgsize_bytes = 0;
        *num_entries = 0;	/* zero this too */

	error = start_fsvrport_op(fsvr_port, creds_port, transid, 0, 0);
	if (error)
		return(error);

        /* first find out how many entries there will be */
        if (error = get_pginfo_size_local(num_entries)) {
                goto out;
        }

        if (size_only) {
                /* we are done */
                goto out;
        }

        /*
         * allocate space for the out buffer - This space will be deallocated
         * by the MIG stub
         */
        error = vm_allocate( mach_task_self(),
				(vm_address_t *) pginfo, 
				(vm_size_t) ((*num_entries) *
					sizeof(struct tbl_pginfo_10)),
				TRUE);
	if ( error )
                goto out;
        
        /* get the pginfo */
        if (error = get_pginfo_local((void *)*pginfo, *num_entries, 
                                     num_entries)) {
                goto out;
        }
        *pgsize_bytes = *num_entries * sizeof(struct tbl_pginfo_10);

      out:
	error = end_fsvrport_op(error, 0);
        return(error);
#else	/* FULLSERVER */
	panic("S_fsvr_get_paging_stats");
#endif	/* FULLSERVER */
}

/*
 * returns max number of paging files on the local node 
 */
int 
get_pginfo_size_local(n)
int *n;
{
        int error;
        
        error = vnode_pager_info(NULL, 0, n);

        if (!import_paging) {
                (*n)++;         /* add for kernel default pager */
        }
        return(error);
}

extern vm_size_t vm_page_size;
extern mach_port_t default_pager_port;

/*
 * get_pginfo_local() fills in the 'pginfo' array with entries cooresponding to 
 * both vnode & kernel paging files on the current node. 
 * Atmost 'pgsize' entries are filled. The actual # of entries filled is 
 * returned in 'entries_read'.
 */
int 
get_pginfo_local(pginfo, pgsize, entries_read)
        struct tbl_pginfo_10 *pginfo; 
        int                   pgsize;
        int                  *entries_read; 
{
        int error=0;
        vm_size_t total, free;
        char node_string[10];
        
        /* first get the vnode pagers info */
        error = vnode_pager_info(pginfo,pgsize, entries_read);
        pginfo += *entries_read;
        pgsize -= *entries_read;

        /* 
         * get kernel default paging info statistics - the kernel's pager
         * is not a vnode pager only if we are not importing paging - otherwise
         * it is part of (some other nodes) vnode pager.
         */
        if ((pgsize > 0) && (!import_paging)) {

                error = default_pager_info( default_pager_port, &total, &free );
                if ( error == KERN_SUCCESS ) {
			vm_statistics_data_t	vms;
			kern_return_t		kr;

                        /* clear it first - then fill in relevant fields */
                        bzero(pginfo, sizeof(struct tbl_pginfo_10)); 
                        pginfo->pg_free = free/vm_page_size;
                        pginfo->pg_npgs = total/vm_page_size;
                        pginfo->pg_type = PG_KERN_DEFAULT;
                        pginfo->pg_node = this_node;
                        strcpy(pginfo->pg_name, 
                               "Kernel Default");
                        sprintf(node_string, ".%d",this_node);
                        strcat(pginfo->pg_name, node_string);
			/*
			 * since the default_pager_info() interface does not
			 * include pagins or pageouts, get them from the
			 * vm_stat() kernel interface. Should we see an error,
			 * don't worry the fields were previously cleared.
			 */
			kr = vm_statistics( mach_task_self(),
						(vm_statistics_data_t *)&vms );
			if ( kr == KERN_SUCCESS ) {
				pginfo->pg_pageout_count = vms.pageouts;
				pginfo->pg_pagein_count = vms.pageins;
			}

                        pginfo++;
                        pgsize--;
                        (*entries_read)++;
                }
        }
        return(error);
}

#ifdef	TNC
#ifdef	CHKPNT
int
remote_rename(ndp, to_name, to_len)
	struct nameidata	*ndp;
	char			*to_name;
	unsigned int		to_len;
{
	int 			error;
	struct server_oip	*oipp = &u.uu_oip;
#ifdef  PFS
	u_long			iomode; /* to keep fsvr_rename happy */
#endif PFS

	OIP_SET_FORW(oipp, ndp->ni_forwport);
	error = fsvr_rename(ndp->ni_forwport, u.uu_procp->p_cred,
			    oipp->oip_transid, ndp->ni_rdirport,
			    ndp->ni_dirp, strlen(ndp->ni_dirp)+1,
			    *to_name == '/' ?
				ndp->ni_rdirport : ndp->ni_cdirport,
#ifdef  PFS
			    to_name, to_len, &iomode);
#else
			    to_name, to_len);
#endif PFS
	OIP_END_FORW(oipp);
	return(error);
}

#endif	CHKPNT
#endif	/* TNC */
