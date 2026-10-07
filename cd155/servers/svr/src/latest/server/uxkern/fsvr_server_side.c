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
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: fsvr_server_side.c,v $
 * Revision 1.49  1995/04/08  00:15:29  yazz
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
 * Revision 1.48  1995/04/04  18:41:56  rlg
 * The gopen step (for the M_UNIX, M_RECORD, and M_ASYNC I/O modes) of
 * duplicating the file server's file table was redesigned.  This change
 * results in only one "many to one" messages (sent from N-1 compute nodes
 * to logical node 0 in the application's partition) and one set of "one to
 * many" messages (sent from each file server to those N-1 compute nodes).
 * The old code required each of the compute nodes in the application's
 * partition (minus one) to execute a sequence of four RPCs to the file
 * servers containing the header file and each stripe file.  This resulted
 * in four "many to one" messages sent to each of those file servers.
 *
 *  Reviewer:  Balaji Narasimhan, Stan Smith
 *  Risk:      Medium (number of modules changed, scope of redesign)
 *  PTS#       9637
 *  Testing:   pfs and fileio EATs, five Eval gopen tests,
 *             rw and iomode integration tests
 *  Module(s): cmds_libs/src/usr/ccs/lib/libnx/_gopen.c
 *             cmds_libs/src/usr/ccs/lib/libnx/pfs_iomode.h
 *             server/svr/src/svr/emulator/pfs_user_side.c
 *             server/svr/src/svr/server/conf/syscalls.master
 *             server/svr/src/svr/server/uxkern/fsvr.defs
 *             server/svr/src/svr/server/uxkern/fsvr_server_side.c
 *             server/svr/src/svr/server/uxkern/pfs2.defs
 *             server/svr/src/svr/server/uxkern/pfs2_server_side.c
 *
 * Revision 1.47  1995/02/11  01:10:25  stans
 *  'lint' picking for a clean compile
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.46  1995/02/01  22:24:01  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.45  1994/11/18  20:48:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.44  1994/09/29  10:17:23  johannes
 * moved S_fsvr_rmdir() out of #ifdef FULLSERVER part into #ifdef PARACORE part
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
 * Revision 1.43  1994/08/31  22:47:45  mtm
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
 * Revision 1.40.2.3  1994/08/19  22:27:48  dbm
 * Added support for a new bootmagic, PFS_ASYNC_DFLT, this allows setting
 * the default PFS I/O mode to M_ASYNC.
 *
 *  Reviewer:Bob Godley
 *  Risk:M
 *  Benefit or PTS #:10569
 *  Testing: Specific test cases. PFS EATS (With and without bootmagic set)
 *  Module(s):
 *
 *     (server)
 *         uxkern/boot_config.c
 *         uxkern/fsvr_server_side.c
 *         uxkern/fsvr.defs
 *     (emulator)
 *         emul_init.c
 *         fsvr_user_side.c
 *         pfs2_user_side.c
 *         pfs_iomode.c
 *         pfs_tokenmgt.c
 *         pfs_iomode.h
 *         pfs_fdt.h
 *     (libnx)
 *         _pfs_setio.c
 *         _setiomode.c
 *
 * Revision 1.40.2.2  1994/08/17  01:08:20  jlitvin
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
 * Revision 1.40.2.1  1994/08/16  19:47:50  jlitvin
 * The change to PTS #9619 was too aggressive.  The SERVER_DEALLOC()
 * macro didn't call vm_deallocate() for cases where the system call
 * failed with an error besides NO_MIG_REPLY.  S_fsvr_write() has been
 * changed to only explicitly call vm_deallocate() for successful writes.
 *
 * Author(s): John Litvin & Dave Minturn
 * Special assistance: Terry Prickett & Sean Griffin
 * Reviewer: Bob Yasi
 * Risk: low
 * Benefit or PTS #: 10428
 * Testing: electrond
 * Module(s): server/uxkern/fsvr_server_side.c
 *
 * Revision 1.40  1994/07/11  17:10:59  johannes
 *  S_fsvr_mkdir(): changed defines for LITE server (resides not in fsvr_msg.c!)
 *  S_fsvr_unlink(): changed defines and position (resides not in fsvr_msg.c!)
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
 * Revision 1.39  1994/06/30  18:00:21  dbm
 * Moved the S_fsvr_mkdir() function outside of the #ifdef FULLSERVER to
 * allow it to be referenced by MIG.  This was required due to the parallel
 * core file support that was added.
 *
 * Revision 1.38  1994/06/22  18:30:23  jlitvin
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
 * Revision 1.37  1994/06/02  22:35:43  chrisp
 * RPCs bsd_rforkmulti and rforkmulti_long parameters change:  file
 * ports added; child task and thread ports eliminated.  Boolean boot
 * magic variable TNC_RFORKMULTI_BINARY_TREE added to determine the
 * spanning tree scheme rforkmulti is to use; it defaults to true i.e.
 * binary tree.  Parameter added for RPC fsvr_file_ref to specify the
 * required change in reference count. Previously, this operation
 * incremented counts by 1 only; now n can be specify and note that
 * n can be negative.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): boot_config.c bsd_1.defs bsd_types.defs bsd_types.h fsvr.defs
 *             fsvr_server_side.c
 *
 * Revision 1.36  1994/05/09  04:32:07  yazz
 * Merged R1.2 revision 1.27.2.6 into main stem.
 *
 * Revision 1.35  1994/04/20  23:43:53  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: #9094
 *  Testing: multiuser (checkin for R1.3 only)
 *  Module(s): server/uxkern/fsvr_server_side.c
 *
 * Init file_portp parameter to MACH_PORT_NULL in S_fsvr_openfh(), a checkpoint/
 * restart routine.  There and in S_fsvr_open(), rename pointers file_port and
 * iomode to file_portp and iomodep.
 *
 * Revision 1.34  1994/04/20  18:17:58  dbm
 * Merged 1.2 version 1.27.2.5 into mainline.
 *
 * Revision 1.33  1994/03/31  17:56:06  dbm
 * Merge R1.2 version 1.27.2.4.
 *
 * Revision 1.32  1994/03/14  02:08:41  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.31  1994/03/11  20:17:57  rlg
 * Merged changes from the R1.2 branch to the R1.3 branch (1.27.2.3).
 *
 * Revision 1.30  1994/03/09  00:48:53  dbm
 * Update of mainline with R1.2 revision 1.27.2.2
 *
 * Revision 1.29  1994/01/12  17:46:37  jlitvin
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
 * Revision 1.28  1993/12/23  01:47:35  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #: None.
 *  Testing: Booted and ran minimal PFS tests.
 *  Module(s): emulator/emul_stack_alloc.c
 *             emulator/fsvr_user_side.c
 *
 *
 * Revision 1.27.2.6  1994/04/27  22:13:25  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: GUBT
 *  Testing: VSX, EATS
 *  Module(s): server/uxkern/fsvr_server_side.c
 *
 * Init MIG OUT params for deallocatable Out-Of-Line (OOL) memory to null,
 * lest randomly specified pages of VM be accidentally transmitted in the
 * reply and unintentionally deallocated out from under the server.
 *
 * Revision 1.27.2.5  1994/04/15  17:27:06  dbm
 * Added initialization of port return values for fsvr_ rpc's
 *
 *  Reviewer:Charlie Johnson
 *  Risk:Low
 *  Benefit or PTS #: 8242,8968
 *  Testing: UFS Eats.
 *  Module(s):
 * 	fsvr_server_side.c
 *
 * Revision 1.27.2.4  1994/03/31  17:53:06  dbm
 * Fixed S_fsvr_fchdir() to check error conditions properly.
 *  Reviewer: Brad Rullman
 *  Risk:Low
 *  Benefit or PTS #:8724
 *  Testing: Specific test case.
 *  Module(s):
 * 	server/fsvr_server_side.c
 * 	emulator/fsvr_user_side.c
 *
 * Revision 1.27.2.3  1994/03/11  16:38:15  rlg
 * The utimes() function for PFS files was reimplemented following the model
 * of pfs_multi_stat(), so that the header file and all stripe files have the
 * same value set in the access and modification time fields.  The old code
 * only set these fields in the header file.
 *
 *  Reviewer:  Brad Rullnam
 *  Risk: medium
 *  Benefit or PTS #:  PTS #6870
 *  Module(s):  emulator/fsvr_user_side.c
 *              emulator/pfs2_user_side.c
 *              server/uxkern/fsvr.defs
 *              server/uxkern/pfs2.defs
 *              server/uxkern/fsvr_server_side.c
 *              server/uxkern/pfs2_server_side.c
 *              server/vfs/vfs_syscalls.c
 *
 * Revision 1.27.2.2  1994/03/09  00:33:16  dbm
 * Added S_fsvr_sync_data() to support O_SYNC mode with mapped files.
 *  Reviewer: Brad Rullman
 *  Risk: Low
 *  Benefit or PTS #: 8420
 *  Testing: Specific test cases using O_SYNC, PFS eats.
 *  Module(s):
 * 	fsvr_server_side.c
 *
 * Revision 1.27.2.1  1994/01/09  00:22:39  brad
 * Fixed bug found by lint (wrong number pf params to pfs_free() in
 * pfs2_user_side.c); also fixed lint warnings in PFS-related code.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: 7221
 *  Testing: ran PFS EATs on 64 nodes on 2 different systems
 *  Module(s): emulator/emul_callback.c
 *             emulator/emul_stack_alloc.c
 *             emulator/pfs2_user_side.c
 *             emulator/pfs_emath.c
 *             emulator/pfs_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr_types.defs
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *
 * Revision 1.27  1993/11/03  00:51:33  dbm
 *    Reviewer(s): Paul Roy
 *    Risk:        Low, Medium or High
 *    Benefit:     Fix for bug #7032, Processes hanging when accessing remote
 * 		ufs file systems.
 *    Testing:     File I/O Eats, PFS Eats, NQS test case from SDSC.
 *    Module(s):   server/uxkern/fsvr_server_side.c
 *
 * Revision 1.26  1993/09/28  21:34:24  brad
 * Disabled the open_with_token RPC interface, since it doesn't make sense
 * for shared files.
 *
 * Revision 1.25  1993/09/27  04:36:48  robboy
 * New ifdefs for FULLSERVER
 *
 * Revision 1.24  1993/09/02  06:46:32  brad
 * Cleaned up previous checkin.
 *
 * Revision 1.23  1993/09/01  01:38:54  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.22  1993/08/31  20:13:56  robboy
 * Restore r. 1.18, which was overwritten by mistake
 *
 * Revision 1.21  1993/08/18  16:36:50  cfj
 * Fix for PTS bug #6098.  Turn on selected code which previously was turned
 * off if FULLSERVER was not defined to allow pipes to work with the lite
 * server in compute paritions.
 *
 * Revision 1.20  1993/08/04  03:55:05  cfj
 * 08-03-93 Code drop from Locus.
 *
 * Revision 1.19  1993/07/20  17:38:59  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.18  1993/07/15  18:14:58  brad
 * Added cleanup of server state via VIO_ERROR to S_fsvr_unlink().  This
 * is part of the support to make removing a PFS file an atomic operation.
 *
 * Revision 1.17  1993/07/14  18:42:07  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.16  1993/07/09  20:59:56  brad
 * Added PFS support to S_fsvr_rename().
 *
 * Revision 1.1.1.3  1993/07/01  21:02:36  cfj
 * Adding new code from vendor
 *
 * Revision 1.15  1993/06/17  00:33:16  brad
 * Removed _pfs_unshare system call.
 *
 * Revision 1.14  1993/06/16  21:32:56  brad
 * Fixed AD1.0.3 merge problem: S_fsvr_mount_pfs now calls the new
 * mount_ad() routine instead of mount().
 *
 * Revision 1.13  1993/05/18  23:40:39  cfj
 * Add #include <nfs.h>
 *
 * Revision 1.12  1993/05/18  18:55:57  cfj
 * Add appropriate ifdefs so that the server will build when turning off NFS.
 *
 * Revision 1.11  1993/05/17  22:59:42  cfj
 * Add an #if MAPPED_FILES in S_fsvr_open_with_token().
 *
 * Revision 1.10  1993/05/12  18:41:58  cfj
 * Added a missing #ifdef PFS in S_fsvr_open_with_token() which assigns the iomode to
 * argument 4 before calling open().
 *
 * Revision 1.9  1993/05/07  23:06:32  cfj
 * More ad1.0.3 fixups.
 *
 * Revision 1.8  1993/05/06  19:29:32  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.2.1.2.10  1993/03/30  02:27:15  brad
 * Removed debug prints.
 *
 * Revision 1.1.2.1.2.9  1993/03/10  05:55:07  brad
 * Added PFS support for chown, chmod, lsize, statfs, ... and others.
 * All async PFS operations now use the response ID parameter, and
 * the server side stubs call in to the fsvr_server_side.c routines
 * to do the work.  Opens of PFS stripefiles in different I/O modes
 * (e.g. VIO_MAPPED and VIO_STRIPED) are now disallowed.
 *
 * Revision 1.6  1993/03/10  16:47:33  nandy
 * call file_port_increment_seqno with lockheld if the lock is already held.
 *
 * Revision 1.1.2.1.2.8  1993/02/23  04:45:36  brad
 * Added PFS support for access() and truncate()/ftruncate().
 *
 * Revision 1.3.6.1  1993/02/16  20:38:59  cfj
 * Synchronous close from OSF.
 *
 * Revision 1.1.2.1.2.7  1993/02/09  21:50:25  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.  Removed S_fsvr_pfs_fstat;
 * it will now be fsvr_pfs_fdevstat in pfs2_server_side.c.
 *
 * Revision 1.1.2.1.2.6  1993/01/11  17:32:40  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 2.22  93/01/11  14:38:35  mmp
 * 	Synchronous close support: add S_fsvr_file_ref, S_fsvr_file_unref,
 * 	and also S_fsvr_cleanlocks.  (mmp)
 * 
 * Revision 1.1.2.1.2.5  1993/01/08  02:18:09  brad
 * Added PFS support to unlink().
 *
 * Revision 1.1.2.1.2.4  1992/12/16  22:50:51  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.3  1992/12/16  06:05:16  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.1.2.2  1992/12/14  23:24:21  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:08  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:54:42  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:16:38  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.1.2.2  1992/11/26  01:46:19  brad
 * Fixed Fast Path I/O syntax error.
 *
 * Revision 1.1.2.1  1992/11/05  23:43:27  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:53:01  cfj
 * Bump major revision number.
 * 
 * Revision 2.21  1992/10/22  15:53:56  dbm
 * Added PFS functionality.
 *
 * Revision 2.21  92/11/05  17:28:16  roy
 * 	additional arguments for absolute pathname to fsvr_mount_ufs and
 * 	fsvr_mount_nfs which is passed to mount() (durriya).
 * 
 * Revision 2.29  93/08/10  15:44:21  slk
 * Moved S_fsvr_file_unref_svr from fsvr_server_side.c to fsvr2_server_side.c
 * to allow LITE servers to build and function.  Because file_unref_svr is
 * needed for sockets.
 * 
 * Revision 2.28  93/07/31  15:32:21  mjl
 * Add fsvr_file_unref_svr RPC used when dequeueing select entries from
 * TNC remote secondary sockets.  (mjl for bhk)
 * 
 * Revision 2.27  93/06/29  15:40:35  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.26  93/06/25  11:26:21  slively
 * Backout the LITE changes.  Remove #if FILSERVER and the code stubs.
 * 
 * Revision 2.25  93/06/22  20:07:45  slively
 * Support for LITE server. #if FILESERVER sections, lots of stubs in the 
 * #else FILESERVER section.
 * 
 * Revision 2.24  93/06/16  13:54:01  klh
 * 	Revision 2.31  93/06/02  17:21:46  rabii
 * 		Fixed mapped files conditional (ifdef -> if) (rabii)
 * 
 * 	Revision 2.30  93/05/18  14:41:38  loverso
 * 		Fix leak of revoke_port when opening a non-mappable file (becomes
 * 		a dead-name when the process exists).
 * 		Fix S_fsvr_open_with_token() to stub out when MAPPED_FILES is off.
 * 
 * 	Revision 2.29  93/03/30  16:08:19  roy
 * 		Fix assert in S_fsvr_read.
 * 		[93/03/09            roy]
 *
 * 	Revision 2.28  93/03/10  10:47:47  mmp
 * 		Enable error return in fsvr_file_unref and fp_unref_port so that
 * 		ENOSPC errors from closef (for NFS) can be passed back to the
 * 		emulator.  Rearrange fp_unref_port so that it waits until all
 * 		other file references go away if the last send right is being
 * 		released.
 *
 * 	Revision 2.27  93/02/03  15:00:52  mmp
 * 		Call start/end_fileserver_op in fsvr_file_ref to make sure
 * 		file_port_increment_seqno gets called for the message.
 *
 * 	Revision 2.26  93/02/02  15:24:14  rabii
 * 		Added creds port to S_fsvr_cleanlocks and S_fsvr_file_unref (rabii)
 *
 * 		Have S_fsvr_flock call afs_xflock when MACH_AFS is defined (rabii)
 *
 * 		Replace calls to mount in S_fsvr_mount_ufs and S_fsvr_mount_nfs with
 * 		mount_ad since mount now obeys single server semantics (rabii)
 *
 * 	Revision 2.25  93/01/27  11:41:10  durriya
 * 		fsvr_open_with_token now passes revoke port as a polymorphic and
 * 		specifies MOVE_SEND for it                                (durriya)
 *
 * 	Revision 2.24  93/01/13  11:30:08  roy
 * 		Remove erroneous assert in fp_unref_port.
 * 		[93/01/13            roy]
 *
 * 	Revision 2.23  93/01/12  17:06:26  roy
 * 		Implement fsvr_open_with_token.
 * 		Remove mappable flag from all routines.
 * 		[93/01/11            roy]
 *
 * Revision 2.22  93/03/10  14:21:07  yazz
 * Synchronous close merge from Intel (less PFS).
 * 
 * 	Revision 1.3.6.1  1993/02/16  20:38:59  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.22  93/01/11  14:38:35  mmp
 * 	 Synchronous close support: add S_fsvr_file_ref, S_fsvr_file_unref,
 * 	 and also S_fsvr_cleanlocks.  (mmp)
 * 
 * 	Revision 1.3  1992/12/11  03:06:08  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.2  1992/11/30  22:54:42  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.2  1992/11/26  01:46:19  brad
 * 	Fixed Fast Path I/O syntax error.
 *
 * 	Revision 1.1.2.1  1992/11/05  23:43:27  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * 	Revision 2.21  1992/10/22  15:53:56  dbm
 * 	Added PFS functionality.
 *
 * Revision 2.21  92/11/05  17:28:16  roy
 * 	additional arguments for absolute pathname to fsvr_mount_ufs and
 * 	fsvr_mount_nfs which is passed to mount() (durriya).
 * 
 * Revision 2.20  92/08/26  12:13:57  loverso
 * 	Fix lint.  Remove transaction_id from token calls.  (loverso)
 * 
 * Revision 2.19  92/07/10  18:26:07  rabii
 * 	Modify the previous fix to use a union with a larger buffer to
 * 	avoid setting the NULL one byte beyond the end of the structure
 * 	if the lenght equals the maximum size. Check for invalid
 * 	length (pjg).
 * 
 * Revision 2.18  92/06/30  22:47:57  loverso
 * 	For unix domain sockets, copy the struct sockaddr_un onto the stack
 * 	so that we can NUL terminate the pathname. (loverso)
 * 
 * Revision 2.17  92/06/11  13:27:01  pjg
 * 	Ifdef SOCKADDR_UN_PADLEN for the i860 because it triggers a bug
 * 	in the i860 compiler.
 * 
 * Revision 2.16  92/06/10  16:34:11  pjg
 * 	Use the correct offset SOCKADDR_UN_PADLEN instead of magic numbers
 * 	to correct the size of namelen in several socket calls. Change some
 * 	printfs to panics (loverso).
 * 
 * Revision 2.15  92/06/08  18:30:29  pjg
 * 	Return the iomode of the file in the routines that open or create
 * 	files. Fix bug that prevented opening a remote special file with
 * 	mapped files. Introduced fast_path support under FAST_PATH_IO
 * 	conditional (read only) (pjg).
 * 
 * 	Added 'accessed' arg to fsvr_token_release. (roy)
 * 
 * 	Added S_fsvr_nfssvc    (durriya)
 * 
 * Revision 2.14  92/06/01  15:38:45  loverso
 * 	Convert spec_len==0 to spec==NULL for S_fsvr_mount_ufs.
 * 
 * Revision 2.13  92/05/24  13:58:29  pjg
 * 	Change TRACE to SC_TRACE.
 * 
 * Revision 2.12  92/05/18  12:27:18  roy
 * 	Revision 2.10.1.2  92/05/08  12:04:31  roy
 * 	Implemented fsvr_token_not_found.  Diddled fsvr_token_release.
 * 	Implemented S_fsvr_swapon (rabii).
 * 	[92/04/22            roy]
 * 
 * 	Revision 2.10.1.1  92/04/22  10:03:35  roy
 * 	Implemented fsvr_token_change.
 * 	[92/04/05            roy]
 * 
 * Revision 2.11  92/04/05  17:09:13  pjg
 * 	Don't set ndp->ni_cdir to ndp->ni_vp2 in rename (pjg)
 * 
 * 	fsvr_stat and fsvr_fstat use stat_t (equiv. struct stat) instead of 
 * 	statb_t to eliminate extra translations/copies. 
 * 	Add fsvr_devstat (durriya)
 * 
 * Revision 2.10  92/03/15  14:29:33  roy
 * 	92/03/03  16:47:10  roy
 * 	Change parameters to release_token.
 * 
 * 	92/02/19  10:36:03  roy
 * 	Moved mapped files interfaces here from bsd_server_side.c.
 * 	Fix vm leaks (see calls to vm_deallocate) (pjg).
 * 
 * Revision 2.9  92/03/01  18:33:25  pjg
 * 	92/02/28  pjg
 * 	Changed deallocation of ports and memory received in messages
 * 	according to the new ux_server_loop.
 * 
 * 	92/02/28  loverso
 * 	Delete signature port and interrupt variable from all stubs.
 * 	Restructure forwarded op registering.
 * 
 * Revision 2.8  92/02/21  16:43:56  durriya
 * 	get rid of norma_ipc conditionals.
 * 
 * Revision 2.7  92/02/11  22:00:34  pjg
 * 	Temporarily pass the signature port as MACH_PORT_NULL to
 * 	start_vnodeserver_op in fsvr_sync due to bad interactions
 * 	between isc and remote operations.
 * 
 * Revision 2.6  92/01/17  17:32:09  roy
 * 	Interruptible system call support (loverso).
 * 
 * Revision 2.5  92/01/16  17:20:35  roy
 * 	92/01/16  13:25:24  pjg
 * 	Added support for NFS client.
 * 	Include the header file generated by MiG to allow type checking
 * 	by the compiler (ANSI C compilers only ?).
 * 
 * 	92/01/14  20:05:51  pjg
 * 	Don't pass root_port twice as parameter to *_vnodeserver_op in sync
 * 	and getfsstat because the port is deallocated twice. The system
 * 	wouldn't come up multi-node if the root fs was multi-user.
 * 
 * Revision 2.4  92/01/14  11:22:10  roy
 * 	92/01/10  22:07:33  noemi
 * 	Removed dummy S_fsvr_update_time function.
 * 
 * 	92/01/07  17:02:42  noemi
 * 	Added S_fsvr_getfsstat and S_fsvr_getfsstat_count. 
 * 
 * 	92/01/06  20:38:20  noemi
 * 	Added S_fsvr_statfs and S_fsvr_getfh.
 * 
 * 	92/01/05  21:05:29  noemi
 * 	Updated S_fsvr_sync; misc aesthetic changes.
 * 
 * Revision 2.3  92/01/09  16:03:42  roy
 * 	Unix domain socket support (loverso).
 * 
 * Revision 2.2  92/01/05  20:18:06  roy
 * 	1991/10/14  20:45:21  noemi
 * 	Initial revision
 * 
 * $EndLog$
 */
/*
 * Glue modified for OSF1_SERVER system calls.
 */

#include <map_uarea.h>
#include <mach_nbc.h>
#include <mapped_files.h>
#include <mach_afs.h>
#include <nfs.h>
#include <ufs.h>
#include <fullserver.h>

#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/fsvr.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/file.h>
#include <sys/un.h>
/*
 * WARNING: This long path is to get the correct stddef.h.   With just
 * <stddef.h>, gcc is picking up "/usr/local/gnu/lib/gcc-include/stddef.h".
 */
#include <uxkern/../../usr/include/stddef.h>	/* for offsetof */

#ifdef	OSF1_ADFS
#include <sys/mount.h>
#include <uxkern/sthread.h>
#endif

#include <uxkern/proc_to_task.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/bsd_msg.h>
#include <uxkern/mf.h>


void     display_port_rights();



#ifdef PFS
/*
 * Bootmagic variable:
 */
int	pfs_async_dflt = 0;

int     dbg_print_flag = 0;
#endif

int
S_fsvr_write(file_port, creds_port, transid, data, count, amount)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char 			*data;
	unsigned int		count;
	int			*amount;		/* out */
{
	register int	error;
	struct file *fp;
	int arg[2];
	int syscode = SYS_write;
	int serial = !sysent[syscode].sy_parallel;
	kern_return_t	kr;

	error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
	if (error)
		return(error);
	arg[0] = (int)data;
	arg[1] = (int)count;
	error = write(fp, arg, amount);

	/*
	 * WARNING:
	 *
	 * We don't want to deallocate the data buffer if the write failed.
	 *
	 * Specifically, if (error != 0), then the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(), (vm_address_t)data, count);
		if (kr != KERN_SUCCESS) {
			panic("S_fsvr_write: vm_deallocate(0x%x,%d) ret=0x%x\n",
			data, count, kr);
		}
	}
	return(end_fileserver_op(fp, error, serial));
}



S_fsvr_read(file_port, creds_port, transid, count, buf, amount_read)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			count;
	char			**buf;		/* OUT, dealloc (ool memory) */
	int			*amount_read;	/* size of the above */
{
	register int	error;
	struct file *fp;
	int arg[2];
	int syscode = SYS_read;
	int serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*buf = 0;
	*amount_read = 0;

	error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
	if (error)
		return(error);

	ASSERT(!VIO_IS_FASTPATH((struct vnode *)fp->f_data));

	if (vm_allocate(mach_task_self(), (vm_offset_t *)buf, count, TRUE) !=
		KERN_SUCCESS) {
		error = ENOMEM;
        } else {
		arg[0] = (int)*buf;
		arg[1] = (int)count;
		error = read(fp, arg, amount_read);
	}
bad:
	error = end_fileserver_op(fp, error, serial);
	if (*buf) {
		if (error) {
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *buf, count);
			*buf = 0;
			*amount_read = 0;
		} else {
			count -= round_page(*amount_read);
			if (count > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*buf + round_page(*amount_read));
				(void) vm_deallocate(mach_task_self(), 
						     start, count);
			}
		}
	}		
	return(error);
}

#if	FULLSERVER

int
S_fsvr_open(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode, crt_mode, file_portp, iomodep)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			crt_mode;
	mach_port_t		*file_portp;	/* out */
	u_long			*iomodep;	/* inout */
{
	register int	error;
#ifdef	PFS
	int arg[5];
#else
	int arg[4];
#endif
	int syscode = SYS_open;
	int serial = !sysent[syscode].sy_parallel;
	struct file *fp;
	struct nameidata *ndp = &u.u_nd;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*file_portp = MACH_PORT_NULL;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     pathname, pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = crt_mode;
	arg[3] = (int)&fp;
#ifdef	PFS
	arg[4] = *iomodep;
#endif
	error = open(u.u_procp, arg, 0);

	/* 
	 * If a remote mount point was detected, forward the open message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_open(ndp->ni_forwport, creds_port, transid,
				  root_port,
				  ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				  mode, crt_mode, file_portp, iomodep);
		OIP_END_FORW(oipp);
	} else {
		if (error == 0) {
			FILE_TO_PORT_LOOKUP(fp, *file_portp);
						/* sets *file_portp non-null */
			*iomodep = VIO_GETMODE((struct vnode *)fp->f_data);
		}
		else if (error != EREMOTEPORT) {
			*file_portp = MACH_PORT_NULL;
			*iomodep = 0;
		}
		else {
			ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
			*file_portp = ndp->ni_forwport;
			*iomodep = 0;
			error = 0;
		}
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

#ifdef	TNC
#ifdef	CHKPNT
/*
 * Return a file_port to the emulator from a given file 
 * handle.  This code is mostly taken from the open()
 * system call.
 */
int
S_fsvr_openfh(cwd_port, creds_port, transid, root_port, fh, 
		mode, file_portp, iomodep)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	struct fhandle		fh;
	int			mode;
	mach_port_t		*file_portp;	/* out */
	u_long			*iomodep;	/* inout */
{
	register int	error;
	int syscode = 9999;
	int serial = FALSE;
	struct file *fp;
	mach_port_t		mach_port_remote;
	struct nameidata *ndp = &u.u_nd;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*file_portp = MACH_PORT_NULL;

	error = start_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0,
				     syscode, serial);
	if (error) 
		return(error);

	error = openfh(&fh, mode, &fp, &mach_port_remote);

	/* 
	 * If a remote mount point was detected, forward the open message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;
		OIP_SET_FORW(oipp, mach_port_remote);
		error = fsvr_openfh(mach_port_remote, creds_port, transid,
				  root_port, (fhandle_t) fh, mode,
				  (mach_port_t *) file_portp, 
				  (unsigned *) iomodep);
		OIP_END_FORW(oipp);
	} else {
		if (error == 0) {
			FILE_TO_PORT_LOOKUP(fp, *file_portp);
						/* sets *file_portp non-null */
			*iomodep = VIO_GETMODE((struct vnode *)fp->f_data);
		}
		else if (error != EREMOTEPORT) {
			*file_portp = MACH_PORT_NULL;
			*iomodep = 0;
		}
		else {
			ASSERT(mach_port_remote != MACH_PORT_NULL);
			*file_portp = ndp->ni_forwport;
			*iomodep = 0;
			error = 0;
		}
	}
	return(end_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}
#endif CHKPNT
#endif /* TNC */

int
S_fsvr_open_with_token(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode, crt_mode, flags, revoke_port,
		file_port, iomode, token, offset, length, mem_obj)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			crt_mode;
	int			flags;
	mach_port_t		revoke_port;
	mach_port_t		*file_port;	/* out */
	u_long			*iomode;	/* out */
	mach_port_t		*token;		/* out */
	off_t			*offset;	/* out */
	int			*length;	/* out */
	mach_port_t		*mem_obj;	/* out */
{
#if 	MAPPED_FILES && OPEN_WITH_TOKEN
	register int	error;
	int arg[6];
	int syscode = 2011;
	int serial = !sysent[SYS_open].sy_parallel;
	struct file *fp;
	struct nameidata *ndp = &u.u_nd;
	mach_port_t token_port;
        memory_object_t	mo;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     pathname, pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = crt_mode;
	arg[3] = (int)&fp;
#ifdef	PFS
	arg[4] = *iomode;
#endif
	error = open(u.u_procp, arg, 0);

	/* 
	 * If a remote mount point was detected, forward the open message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_open_with_token(ndp->ni_forwport, creds_port, 
				  transid, root_port, ndp->ni_ptr, 
				  strlen(ndp->ni_ptr)+1, mode, crt_mode,
				  flags, revoke_port, MACH_MSG_TYPE_COPY_SEND,
                                  file_port,
				  iomode, token, offset, length, mem_obj);
		OIP_END_FORW(oipp);
		if (!error) {
			/* Must discard send right on revoke port */
			mach_port_deallocate(mach_task_self(), revoke_port);
		}
	} else {
		*token = MACH_PORT_NULL;	/* initialize */
		*mem_obj = MACH_PORT_NULL;
		if (error == 0) {
			FILE_TO_PORT_LOOKUP(fp, *file_port);
			*iomode = VIO_GETMODE((struct vnode *)fp->f_data);
			if (*iomode == VIO_MAPPED) {
				/*
				 * Now get a token for the open file.  We don't
				 * need an extra reference on the file structure
				 * to call mf_token_acquire_with_mo because we
				 * hold a file structure send right.
				 */
				arg[0] = flags;
				arg[1] = revoke_port;
				arg[2] = (int)&token_port;
				arg[3] = (int)offset;
				arg[4] = (int)length;
				/*
				 * XXX Always return a memory object regardless
				 * of whether the file is open for write.	
				 */
				arg[5] = (int)&mo;
				error = mf_token_acquire_with_mo(fp, arg);
				if (error == 0) {
					*mem_obj = mo;
					*token = token_port;
				}
#ifdef	notdef
				if (fp->f_flag & FWRITE) { 
					arg[5] = (int)&mo;
					error = mf_token_acquire_with_mo(fp, 
									 arg);
					if (error == 0) {
						*mem_obj = mo;
						*token = token_port;
					}
				} else {
					error = mf_token_acquire(fp, arg);
					if (error == 0) 
						*token = token_port;
				}
#endif	/* notdef */
			} else {
				/* Must discard send right on revoke port */
				mach_port_deallocate(mach_task_self(),
						     revoke_port);
			}
		}
		else if (error != EREMOTEPORT) {
			*file_port = MACH_PORT_NULL;
			*iomode = 0;
			/* revoke port discarded when message destroyed */
		}
		else {
			ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
			*file_port = ndp->ni_forwport;
			*iomode = 0;
			error = 0;
			/* Must discard send right on revoke port */
			mach_port_deallocate(mach_task_self(), revoke_port);
		}
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
#else
	return EINVAL;
#endif
}

#ifdef	COMPAT_43
int
S_fsvr_oopen(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode, crt_mode, file_port, iomode)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			crt_mode;
	mach_port_t		*file_port;	/* out */
	u_long			*iomode;	/* out */
{
	register int		error;
#ifdef	PFS
	int arg[5];
#else
	int arg[4];
#endif
	int syscode = SYS_oopen;
	int serial = !sysent[syscode].sy_parallel;
	struct file *fp;
	struct nameidata *ndp = &u.u_nd;

	*file_port = MACH_PORT_NULL;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     pathname, pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = crt_mode;
	arg[3] = (int)&fp;
#ifdef	PFS
	arg[4] = VIO_NONE;
#endif
	error = oopen(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the open message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_oopen(ndp->ni_forwport, creds_port, transid,
				   root_port,
				   ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				   mode, crt_mode, file_port, iomode);
		OIP_END_FORW(oipp);
	} else {
		if (error == 0) {
			FILE_TO_PORT_LOOKUP(fp, *file_port);
			*iomode = VIO_GETMODE((struct vnode *)fp->f_data);
		}
		else if (error != EREMOTEPORT) {
			*file_port = MACH_PORT_NULL;
			*iomode = 0;
		}
		else {
			ASSERT(ndp->ni_forwport != MACH_PORT_NULL);
			*file_port = ndp->ni_forwport;
			*iomode = 0;
			error = 0;
		}
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_ocreat(cwd_port, creds_port, transid, root_port, pathname,
	      pathname_len, mode, file_port, iomode)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	mach_port_t		*file_port;	/* out */
	u_long			*iomode;	/* out */
{
	register int		error;
	int arg[3];
	int syscode = SYS_ocreat;
	int serial = !sysent[syscode].sy_parallel;
	struct file *fp;

	*file_port = MACH_PORT_NULL;
	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = (int)&fp;
	error = ocreat(u.u_procp, arg, 0);

	/*
	* If a remote mount point was detected, forward the creat message.
	*/
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_ocreat(ndp->ni_forwport, creds_port,
				    transid, root_port,
				    ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				    mode, file_port, iomode);
		OIP_END_FORW(oipp);
	} else {
		if (error == 0) {
			FILE_TO_PORT_LOOKUP(fp, *file_port);
			*iomode = VIO_GETMODE((struct vnode *)fp->f_data);
		}
		else {
			*file_port = MACH_PORT_NULL;
			*iomode = 0;
		}
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}
#endif	/* COMPAT_43 */


int
S_fsvr_mknod(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode, dev)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			dev;
{
	register int error;
	int arg[3];
	int syscode = SYS_mknod;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = dev;
	error = mknod(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the mknod message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_mknod(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				mode, dev);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_rmknod(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode, dev, node)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			mode;
	int			dev;
	int			node;
{
	register int    error;
	int arg[4];
	int syscode = SYS_rmknod;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = mode;
	arg[2] = dev;
	arg[3] = node;
	error = rmknod(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the rmknod message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_rmknod(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				mode, dev, node);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_link(from_cwd_port, creds_port, transid, root_port, from_name,
		from_len, to_cwd_port, to_name, to_len)
	mach_port_t	from_cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *from_name;
	unsigned int    from_len;
	mach_port_t     to_cwd_port;
	char            *to_name;
	unsigned int    to_len;
{
        register int    error;
        int arg[2];
        int syscode = SYS_link;
        int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(from_cwd_port, root_port, to_cwd_port,
				     creds_port, transid, from_name, 
				     from_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%s)",from_name, to_name));
	arg[0] = (int)from_name;
	arg[1] = (int)to_name;
	error = link(u.u_procp, arg, 0);
	/*
	 * Link only returns EREMOTE if the first pathname translation
	 * went remote.  In this case, we forward the link message,
	 * passing the remainder of the first pathname and the entire
	 * second pathname.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_link(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				to_cwd_port, to_name, to_len);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(from_cwd_port, root_port, to_cwd_port, error,
			   serial));
}


int
S_fsvr_symlink(cwd_port, creds_port, transid, root_port, from_name,
		from_len, to_name, to_len)
        mach_port_t	cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *from_name;
        unsigned int    from_len;
        char            *to_name;
        unsigned int    to_len;
{

	register int    error;
	int arg[2];
	int syscode = SYS_symlink;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     to_name, to_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%s)",to_name,from_name));
	arg[0] = (int)from_name;
	arg[1] = (int)to_name;
	error = symlink(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the symlink message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_symlink(ndp->ni_forwport, creds_port,
				transid, root_port,
				from_name, from_len,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

int
S_fsvr_access(cwd_port, creds_port, transid, root_port, pathname, pathname_len,
#ifdef	PFS
	mode, iomode)
#else
	mode)
#endif
	mach_port_t	cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *pathname;
	unsigned int    pathname_len;
	int		mode;
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
	register int    error;
	int arg[2];
	int syscode = SYS_access;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%o)",pathname,mode));
	arg[0] = (int)pathname;
	arg[1] = mode;

	error = saccess(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the access message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_access(ndp->ni_forwport, creds_port,
				    transid, root_port,
				    ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				    mode, iomode);
#else
				    mode);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

int
S_fsvr_stat(cwd_port, creds_port, transid, root_port, pathname,	
#ifdef	PFS
	    pathname_len, follow, stat, iomode)
#else
	    pathname_len, follow, stat)
#endif
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *pathname;
	unsigned int    pathname_len;
        boolean_t       follow;
        struct stat 	*stat;		/* out */
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
        register int    error;
        int arg[2];
        int syscode = follow ? SYS_stat : SYS_lstat;
        int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = (int)stat;
	error = stat1(u.u_procp, arg, 0, (follow ? FOLLOW : NOFOLLOW));

	/*
	 * If a remote mount point was detected, forward the access message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_stat(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				follow, stat, iomode);
#else
				follow, stat);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}

	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_devstat(cwd_port, creds_port, transid, root_port, pathname,	
               pname_len, devsb)
        mach_port_t       cwd_port;
        mach_port_t       creds_port;
	transaction_id_t  transid;
	mach_port_t       root_port;
	char              *pathname;
	unsigned int      pname_len;
        struct devstat    *devsb;		/* out */
{
        register int      error;
        int arg[2];
        int serial = (!sysent[SYS_devstat].sy_parallel);

        error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
                                     creds_port, transid, pathname, pname_len,
                                     SYS_devstat, serial);
        if (error)
          return(error);
        arg[0] = (int)pathname;
        arg[1] = (int)devsb;
        
        error = devstat(u.u_procp, arg);

        /*
         * If a remote mount point was detected, forward the message
         */
        if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
                error = fsvr_devstat(ndp->ni_forwport, creds_port, transid, 
                                     root_port, ndp->ni_ptr, 
                                     strlen(ndp->ni_ptr) +1, devsb);
		OIP_END_FORW(oipp);
        }
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
                
}        

          



int
S_fsvr_statfs(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, statfsb)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	struct statfs		*statfsb;	/* out */
{
	register int    error;
	int arg[2];
	int syscode = SYS_statfs;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname,
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = (int)statfsb;
	error =	statfs(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the statfs message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_statfs(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				statfsb);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_getfh(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, fhp)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	fhandle_t		*fhp;		/* out */
{
	register int    error;
	int arg[2];
	int syscode = SYS_getfh;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = (int)fhp;
	error =	getfh(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the statfs message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_getfh(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				fhp);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


S_fsvr_readlink(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, count, buf, bufcount)
	mach_port_t     cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *pathname;
	unsigned int    pathname_len;
	int		count;
	char		*buf;		/* out */
	unsigned int	*bufcount;	/* out */
{
	register int    error;
	int arg[3];
	int syscode = SYS_readlink;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = (int)buf;
	arg[2] = count;
	error =	readlink(u.u_procp, arg, bufcount);

	/*
	 * If a remote mount point was detected, forward the readlink message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_readlink(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				count, buf, bufcount);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

int
S_fsvr_chmod(cwd_port, creds_port, transid, root_port, pathname, pathname_len,
#ifdef	PFS
	     mode, iomode)
#else
	     mode)
#endif
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
 	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	int		mode;
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
	register int    error;
	int arg[2];
	int syscode = SYS_chmod;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%o)",pathname,mode));
	arg[0] = (int)pathname;
	arg[1] = mode;
	error =	chmod(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the chmod message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_chmod(ndp->ni_forwport, creds_port,
				   transid, root_port,
				   ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				   mode, iomode);
#else
				   mode);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_chown(cwd_port, creds_port, transid, root_port, pathname, pathname_len,
#ifdef	PFS
	     uid, gid, iomode)
#else
	     uid, gid)
#endif
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	int		uid;
	int		gid;
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
	register int    error;
	int arg[3];
	int syscode = SYS_chown;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%d,%d)",pathname,uid,gid));
	arg[0] = (int)pathname;
	arg[1] = uid;
	arg[2] = gid;
	error =	chown(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the chown message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_chown(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				uid, gid, iomode);
#else
				uid, gid);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));

}


int
S_fsvr_utimes(cwd_port, creds_port, transid, root_port, pathname,
#ifdef	PFS
	      pathname_len, times, current_time, iomode)
#else
	      pathname_len, times, current_time)
#endif
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	struct timeval	*times;
	boolean_t	current_time;
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
	register int    error;
	int arg[2];
	int syscode = SYS_utimes;
	int serial = !sysent[syscode].sy_parallel;


	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = (int)(current_time ? NULL : times);
	error = utimes(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the utimes message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_utimes(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				times, current_time, iomode);
#else
				times, current_time);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_truncate(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len,
#ifdef	PFS
		length, iomode)
#else
		length)
#endif
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	int		length;
#ifdef	PFS
	u_long		*iomode;	/* out */
#endif
{
	register int    error;
	int arg[2];
	int syscode = SYS_truncate;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = length;
	error = truncate(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the utimes message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_truncate(ndp->ni_forwport, creds_port,
				      transid, root_port,
				      ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				      length, iomode);
#else
				      length);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));

}

int
S_fsvr_rename(from_cwd_port, creds_port, transid, root_port,
#ifdef	PFS
	      from_name, from_len, to_cwd_port, to_name, to_len, iomode)
#else
	      from_name, from_len, to_cwd_port, to_name, to_len)
#endif
	mach_port_t	from_cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *from_name;
	unsigned int    from_len;
	mach_port_t     to_cwd_port;
	char            *to_name;
	unsigned int    to_len;
#ifdef	PFS
	u_long		*iomode;	/* in/out */
#endif
{

        register int    error;
        int arg[2];
        int syscode = SYS_rename;
        int serial = !sysent[syscode].sy_parallel;
	struct nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(from_cwd_port, root_port, to_cwd_port,
				     creds_port, transid, from_name,
				     from_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%s)",from_name,to_name));
	arg[0] = (int)from_name;
	arg[1] = (int)to_name;
#ifdef	PFS
	if (*iomode == VIO_PFS)
		/*
		 * HACK ALERT: Since we're here, the client emulator has 
		 * already been given notification that this is a PFS file via
		 * a previous rename RPC.  Now the client wants to perform a
		 * "real" rename operation on the PFS header file.  We
		 * temporarily use the retval parameter of rename as a flag to
		 * indicate that the header file is to be renamed.
		 * 
		 * Eventually the pfs_multi_rename() in the emulator should be
		 * implemented in the server, so that the double rename() calls
		 * to the PFS header file can be avoided.
		 */
		error = rename(u.u_procp, arg, 1);
	else
#endif
		error = rename(u.u_procp, arg, 0);

	/*
	 * Rename only returns EREMOTE if the first pathname translation
	 * went remote.  In this case, we forward the rename message,
	 * passing the remainder of the first pathname and the entire
	 * second pathname.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_rename(ndp->ni_forwport, creds_port,
				    transid, root_port,
				    ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
#ifdef	PFS
				    to_cwd_port, to_name, to_len, iomode);
#else
				    to_cwd_port, to_name, to_len);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		/*
		 * Return indication that this is a PFS file, so the client
		 * emulator can do the right thing with PFS stripe data.
		 */
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(from_cwd_port, root_port, to_cwd_port, error,
			   serial));
}


S_fsvr_mount_ufs(cwd_port, creds_port, transid, root_port, dev_port,
                 pathname, pathname_len, abs_pathname, abs_plen, flags, spec, 
                 spec_len, exflags, exroot)
	mach_port_t     cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	mach_port_t     dev_port;
	char            *pathname;
	int		pathname_len;
        char            *abs_pathname;
        int             abs_plen;
	int             flags;
	char		*spec;
	int		spec_len;
	int		exflags;
	int		exroot;
{
	register int    error;
	int arg[5];
	struct ufs_args ufs_args;
	int syscode = SYS_mount;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, dev_port,
				     creds_port, transid, pathname, 
	                             pathname_len, syscode, serial);
	if (error) 
		return(error);

	if (spec_len) {
		SC_TRACE(("(%s,%s)",pathname,spec));
	} else {
		/* fsck passes a NULL for updating hot root */
		spec = NULL;
		SC_TRACE(("(%s,NULL)",pathname));
	}

	ufs_args.fspec = spec;
	ufs_args.exflags = exflags;
	ufs_args.exroot = exroot;

	arg[0] = MOUNT_UFS;
	arg[1] = (int)pathname;
	arg[2] = flags;
	arg[3] = (int)&ufs_args;
        arg[4] = (int)abs_pathname;
	error = mount_ad(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the mount message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_mount_ufs(ndp->ni_forwport, creds_port,
				transid, root_port,
				dev_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
                                abs_pathname, abs_plen,
				flags, spec, spec_len, exflags, exroot);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, dev_port, error,
				  serial));
}

#if	NFS
S_fsvr_mount_nfs(cwd_port, creds_port, transid, pathname, 
		 pathname_len, abs_pathname, abs_plen, 
                 flags, args, addr, fh, hostname, hostname_len)
	mach_port_t     cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	char            *pathname;
	int		pathname_len;
        char            *abs_pathname;
        int             abs_plen;
	int             flags;
	nfs_args_t	args;
	sockaddr_in_t	addr;
	nfsv2fh_t	fh;
	char		*hostname;
	int		hostname_len;
{
	register int    error;
	int arg[5];
	int syscode = SYS_mount;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
	                             pathname_len, syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s,%s)",pathname,hostname));

	args.addr = &addr;
	args.fh   = &fh;
	args.hostname = hostname;

	arg[0] = MOUNT_NFS;
	arg[1] = (int)pathname;
	arg[2] = flags;
	arg[3] = (int)&args;
        arg[4] = (int)abs_pathname;
	error = mount_ad(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the mount message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_mount_nfs(ndp->ni_forwport, creds_port, 
				       transid, ndp->ni_ptr, 
				       strlen(ndp->ni_ptr)+1,
                                       abs_pathname, abs_plen, flags, 
				       args, addr, fh, hostname, 
				       hostname_len);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}
#endif /* NFS */


S_fsvr_unmount(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, flags)
	mach_port_t     cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *pathname;
	unsigned int    pathname_len;
	int		flags;
{
	register int    error;
	int arg[2];
	int syscode = SYS_unmount;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = flags;
	error = unmount(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the unmount message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_unmount(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				flags);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
				  serial));
}

#endif	FULLSERVER

#if 	FULLSERVER || defined(PARACORE)
int
S_fsvr_mkdir(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, mode)
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	int		mode;
{
#if 	FULLSERVER
	register int    error;
	int arg[2];
	int syscode = SYS_mkdir;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s,%o)",pathname,mode));
	arg[0] = (int)pathname;
	arg[1] = mode;
	error = mkdir(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the mkdir message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_mkdir(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				mode);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
#else	/* FULLSERVER */
	return(EINVAL);
#endif	/* FULLSERVER */
}

int
S_fsvr_rmdir(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len)
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
{
#if 	FULLSERVER
	register int    error;
	int syscode = SYS_rmdir;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	error = rmdir(u.u_procp, &pathname, 0);
	/*
	 * If a remote mount point was detected, forward the rmdir message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_rmdir(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
#else	/* FULLSERVER */
	return(EINVAL);
#endif	/* FULLSERVER */
}

int
S_fsvr_unlink(cwd_port, creds_port, transid, root_port, pathname,
#if	defined(PFS) || defined(PARACORE)
	      pathname_len, iomode)
#else
	      pathname_len)
#endif
	mach_port_t	cwd_port;
	mach_port_t     creds_port;
	transaction_id_t	transid;
	mach_port_t     root_port;
	char            *pathname;
	unsigned int    pathname_len;
#if	defined(PFS) || defined(PARACORE)
	u_long		*iomode;	/* in/out */
#endif
{
#if 	FULLSERVER
        register int    error;
        int syscode = SYS_unlink;
        int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname,
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
        SC_TRACE(("(%s)",pathname));
#ifdef	PFS
	if (*iomode == VIO_PFS)
		/*
		 * HACK ALERT: Since we're here, the client emulator has 
		 * already been given notification that this is a PFS file via
		 * a previous unlink RPC.  Now the client wants to perform a
		 * "real" unlink operation on the PFS header file.  We
		 * temporarily use the retval parameter of unlink as a flag to
		 * indicate that the header file is to be removed.
		 * 
		 * Eventually the pfs_multi_unlink() in the emulator should be
		 * implemented in the server, so that the double unlink() calls
		 * to the PFS header file can be avoided.
		 */
		error = unlink(u.u_procp, &pathname, 1);
	else if (*iomode == VIO_ERROR)
		/*
		 * Clean up state.
		 */
		error = unlink(u.u_procp, &pathname, 2);
	else
#endif
		error = unlink(u.u_procp, &pathname, 0);

	/*
	 * If a remote mount point was detected, forward the unlink message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_unlink(ndp->ni_forwport, creds_port,
				    transid, root_port, ndp->ni_ptr,
#if	defined(PFS) || defined(PARACORE)
				    strlen(ndp->ni_ptr)+1, iomode);
#else
				    strlen(ndp->ni_ptr)+1);
#endif
		OIP_END_FORW(oipp);
#ifdef	PFS
	} else if (error == ESTRIPED) {
		/*
		 * Return indication that this is a PFS file, so the client
		 * emulator can do the right thing with PFS stripe data.
		 */
		*iomode = VIO_PFS;
		error = ESUCCESS;
	} else {
		*iomode = VIO_NONE;
#endif
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
#else	/* FULLSERVER */
	return(EINVAL);
#endif	/* FULLSERVER */
}
#endif	/* FULLSERVER || defined(PARACORE) */

int
S_fsvr_getfsstat_count(root_port, creds_port, transid, tag, entries)
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			tag;
	int			*entries;	/* out */
{
	int 	error;
	int	syscode = SYS_getfsstat;
	int 	serial = !sysent[syscode].sy_parallel;
	extern 	mach_port_t	root_vnode_port;
	extern	node_t		this_node;
	extern	node_t		root_fs_node;

	error = start_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid,
				     NULL, 0, syscode, serial);
	if (error)
		return(error);
	/*
	 * If this message originated from the emulation library and
	 * this server is not the root fileserver, forward the message
	 * to the root fileserver.  Note: this may only happen after
	 * a chroot.
	 */
	SC_TRACE(("(%x,%x,%x,%x)",0,0,0,tag));

	if (tag == -1 && this_node != root_fs_node) {
		error = fsvr_getfsstat_count(root_vnode_port, creds_port,
					     transid, tag, entries);
	} else {
		int arg[4];

		arg[0] = arg[1] = arg[2] = 0;
		arg[3] = tag;
		error = getfsstat(u.u_procp, arg, entries);
	}
	return(end_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}


int
S_fsvr_getfsstat(root_port, creds_port, transid, maxsize, flags,
		tag, buf, bufsize)
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			maxsize;
	int			flags;
	int			tag;
	char			**buf;		/* out, dealloc (ool memory) */
	unsigned int		*bufsize;	/* size of the above */
{
	int 	error;
	int	syscode = SYS_getfsstat;
	int 	serial = !sysent[syscode].sy_parallel;
	int 	arg[4];
	extern 	mach_port_t	root_vnode_port;
#if	NORMA_IPC
	extern	node_t		this_node;
	extern	node_t		root_fs_node;
#endif

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*buf = 0;
	*bufsize = 0;

	error = start_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				     creds_port, transid, NULL, 0,
				     syscode, serial);
	if (error)
		return(error);

	/*
	 * If this message originated from the emulation library and
	 * this server is not the root fileserver, forward the message
	 * to the root fileserver.  Note: this may only happen after
	 * a chroot.
	 */
	SC_TRACE(("(%x,%x,%x,%x)",*buf,maxsize,flags,tag));

	if (tag == -1 && this_node != root_fs_node)  {
		error = fsvr_getfsstat(root_vnode_port, creds_port,
				       transid, maxsize, flags, tag, buf,
				       bufsize);

		return(end_vnodeserver_op(root_port, MACH_PORT_NULL, 
					  MACH_PORT_NULL, error, 
					  serial));
	}
	/*
	 * Allocate enough space for statfs structures.
	 */
	if (vm_allocate(mach_task_self(), (vm_offset_t *)buf, maxsize, TRUE) !=
		KERN_SUCCESS) {
		error = ENOMEM;
		*buf = 0;
		*bufsize = 0;
	} else {
		arg[0] = (int)*buf;
		arg[1] = maxsize;
		arg[2] = flags;
		arg[3] = tag;
		error = getfsstat(u.u_procp, arg, bufsize);
	}

	error = end_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL, 
				   error, serial);

	if (*buf) {
		if (error) {
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *buf, maxsize);
			*buf = 0;
			*bufsize = 0;
		} else {
			/*
			 * Return bufsize in bytes;
			 */
			*bufsize *= sizeof(struct statfs);

			maxsize -= round_page(*bufsize);
			if (maxsize > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*buf + round_page(*bufsize));
				(void) vm_deallocate(mach_task_self(), 
						     start, maxsize);
			}
		}
	}		
	return(error);
}


#if	FULLSERVER
/*
 * Sync
 */
int
S_fsvr_sync(root_port, creds_port, transid, tag)
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			tag;
{
	int 	error;
	int	syscode = SYS_sync;
	int 	serial = !sysent[syscode].sy_parallel;
	extern 	mach_port_t	root_vnode_port;
	extern	node_t		this_node;
	extern	node_t		root_fs_node;

	error = start_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
			creds_port, transid, NULL, 0, syscode,
			serial);
	if (error)
		return(error);

	/*
	 * If this message originated from the emulation library and
	 * this server is not the root fileserver, forward the message
	 * to the root fileserver.  Note: this may only happen after
	 * a chroot.
	 */
	SC_TRACE(("(%x)",tag));

	if (tag == -1 && this_node != root_fs_node)
		error = fsvr_sync(root_vnode_port, creds_port,
				transid, tag);
	else
		error = sync(u.u_procp, &tag, 0);

	return(end_vnodeserver_op(root_port, MACH_PORT_NULL, MACH_PORT_NULL,
				  error, serial));
}


int
S_fsvr_chdir(cwd_port, root_port, creds_port, transid, pathname,
		pathname_len, newcwd_port)
	mach_port_t		cwd_port;
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*pathname;
	unsigned int		pathname_len;
	mach_port_t		*newcwd_port;	/* out */
{
	register int    error;
	struct vnode	*vp;
	int syscode = SYS_chdir;
	int serial = !sysent[syscode].sy_parallel;
	struct nameidata *ndp = &u.u_nd;


	*newcwd_port = MACH_PORT_NULL;
	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	error =	chdir(u.u_procp, &pathname, 0);

	/*
	 * If a remote mount point was detected, forward the chown message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_chdir(ndp->ni_forwport, root_port,
				creds_port, transid,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				newcwd_port);
		OIP_END_FORW(oipp);
	} else if (!error) {
		vp = ndp->ni_vp;
		get_vnode_port(vp, newcwd_port);
		vrele(vp);
	}

	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			serial));
}

int
S_fsvr_chroot(cwd_port, root_port, creds_port, transid, pathname,
		pathname_len, newroot_port)
	mach_port_t		cwd_port;
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*pathname;
	unsigned int		pathname_len;
	mach_port_t		*newroot_port;	/* out */
{
	register int    error;
	struct vnode	*vp;
	int syscode = SYS_chroot;
	int serial = !sysent[syscode].sy_parallel;
	struct nameidata *ndp = &u.u_nd;
	*newroot_port = MACH_PORT_NULL;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	error =	chroot(u.u_procp, &pathname, 0);

	/*
	 * If a remote mount point was detected, forward the chown message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_chroot(ndp->ni_forwport, root_port,
				creds_port, transid,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				newroot_port);
		OIP_END_FORW(oipp);
	} else if (!error) {
		vp = ndp->ni_vp;
		get_vnode_port(vp, newroot_port);
		vrele(vp);
	}

	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

int
S_fsvr_fchdir(fp_port, creds_port, transid, newcwd_port)
        mach_port_t     fp_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     *newcwd_port;	/* out */
{
	register int    error;
	struct file	*fp;
	struct vnode	*vp;
	int syscode = SYS_fchdir;
	int serial = !sysent[syscode].sy_parallel;

	*newcwd_port = MACH_PORT_NULL;
	error = start_fileserver_op(&fp, fp_port, creds_port,
				     transid, syscode, serial);
	if (error) 
		return(error);
	error = fchdir(fp, 0, 0);
	if (!error) {
		vp = (struct vnode *)fp->f_data;
		get_vnode_port(vp, newcwd_port);
		vrele(vp);
	}

	return(end_fileserver_op(fp, error, serial));
}

int
S_fsvr_swapon(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, flags, lowat, hiwat)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			flags;
	int			lowat;
	int			hiwat;
{
	register int	error;
	int arg[4];
	int syscode = SYS_swapon;
	int serial = !sysent[syscode].sy_parallel;
	struct nameidata *ndp = &u.u_nd;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid,
				     pathname, pathname_len, syscode, serial);
	if (error) 
		return(error);
	SC_TRACE(("(%s)",pathname));
	arg[0] = (int)pathname;
	arg[1] = flags;
	arg[2] = lowat;
	arg[3] = hiwat;
	error = swapon(u.u_procp, arg, 0);

	/* 
	 * If a remote mount point was detected, forward the open message.
	 */
	if (error == EREMOTE) {
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_swapon(ndp->ni_forwport, creds_port, transid,
					root_port,
					ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
					flags, lowat, hiwat);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}

#endif	FULLSERVER


int
S_fsvr_lseek(file_port, creds_port, transid, off, sbase, newoff)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	u_long			off;
	int			sbase;
	u_long			*newoff;	/* out */
{
	register int	error;
	struct file *fp;
	u_int arg[2];
	int syscode = SYS_lseek;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);
	arg[0] = (u_int)off;
	arg[1] = sbase;
	error = lseek(fp, arg, newoff);
	return (end_fileserver_op(fp, error, serial));
}


int
S_fsvr_fchmod(file_port, creds_port, transid, mode)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			mode;
{
	register int	error;
	struct file *fp;
	int syscode = SYS_fchmod;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);
	error = fchmod(fp, &mode, 0);
	return (end_fileserver_op(fp, error, serial));
}



int
S_fsvr_fchown(file_port, creds_port, transid, uid, gid)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			uid;
	int			gid;
{
	register int	error;
	struct file *fp;
	int arg[2];
	int syscode = SYS_fchown;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);
	arg[0] = uid;
	arg[1] = gid;
	error = fchown(fp, arg, 0);
	return (end_fileserver_op(fp, error, serial));
}


int
S_fsvr_ftruncate(file_port, creds_port, transid, len)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	u_int			len;
{
	register int	error;
	struct file *fp;
	int syscode = SYS_ftruncate;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);
	error = ftruncate(fp, &len, 0);
	return (end_fileserver_op(fp, error, serial));
}


int
S_fsvr_fsync(file_port, creds_port, transid)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
{
	register int	error;
	struct file *fp;
	int syscode = SYS_fsync;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);
	error = fsync(fp, 0, 0);
	return (end_fileserver_op(fp, error, serial));
}


int
S_fsvr_revoke(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
{
        register int    error;
        int syscode = SYS_revoke;
        int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
        SC_TRACE(("(%s)",pathname));

	error = revoke(u.u_procp, &pathname, 0);

	/*
	 * If a remote mount point was detected, forward the revoke message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_revoke(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));
}


int
S_fsvr_flock(file_port, creds_port, transid, cmd)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			cmd;
{
	register int    error;
#if	MACH_AFS
	int syscode = 	SYS_afs_xflock;
#else	MACH_AFS
	int syscode = 	SYS_flock;
#endif	MACH_AFS
	int serial = 	!sysent[syscode].sy_parallel;
	struct file	*fp;

        error = start_fileserver_op(&fp, file_port, creds_port,
                                    transid, syscode, serial);
	if (error) 
		return(error);
#if	MACH_AFS
	error = afs_xflock(fp, &cmd, 0);
#else	MACH_AFS
	error = flock(fp, &cmd, 0);
#endif	MACH_AFS
        return(end_fileserver_op(fp, error, serial));
}

int
S_fsvr_fstat(file_port, creds_port, transid, stat)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
        struct stat          *stat;
{
	register int    error;
	int arg[1];
	int syscode = 	SYS_fstat;
	int serial = 	!sysent[syscode].sy_parallel;
	struct file	*fp;

        error = start_fileserver_op(&fp, file_port, creds_port,
                                    transid, syscode, serial);
	if (error) 
		return(error);
	arg[0] = (int)stat;
	error = fstat(fp, arg, 0);

        return(end_fileserver_op(fp, error, serial));
}


/*
 * in kern_acct
 */

int
S_fsvr_acct(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, acct_on)
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
        char            *pathname;
        unsigned int    pathname_len;
	boolean_t       acct_on;
{
	register int    error;
	int arg[1];
	int syscode = SYS_acct;
	int serial = !sysent[syscode].sy_parallel;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname, 
				     pathname_len, syscode, serial);
	if (error) 
		return(error);
	arg[0] = (acct_on) ? (int)pathname : 0;
	error = sysacct(u.u_procp, arg, 0);
	/*
	 * If a remote mount point was detected, forward the acct message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;

		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_acct(ndp->ni_forwport, creds_port,
				transid, root_port,
				ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				acct_on);
		OIP_END_FORW(oipp);
	}
	return(end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
			   serial));

}

#if	NFS
int
S_fsvr_nfssvc(file_port, creds_port, transid, mask, match, rvalp)
        mach_port_t        file_port;
        mach_port_t        creds_port;
        transaction_id_t   transid;
        int                mask;
        int                match;
        int                *rvalp;
{
        struct file  *fp;
	int syscode = SYS_nfssvc;
	int serial = !sysent[syscode].sy_parallel;
        int   arg[3];
        int   error;

        error = start_fileserver_op(&fp, file_port, creds_port, transid, 
                                    syscode, serial);
        if (error) 
          return(error);

        arg[0] = (int)fp;
        arg[1] = mask;
        arg[2] = match;
        
        error = nfssvc(u.u_procp, arg, rvalp);
        
        return(end_fileserver_op(fp, error, serial));
}
#endif /* NFS */

/*
 * Adjustment to get the length of sun_path
 */
#ifndef i860
#define SOCKADDR_UN_PADLEN (offsetof(struct sockaddr_un, sun_path))
#else
#define SOCKADDR_UN_PADLEN (2*sizeof(u_char))
#endif

/*
 * syscode is either SYS_bind or SYS_connect
 */
int
S_fsvr_uds_bindconn(file_port, cwd_port, creds_port, transid,
		root_port, syscode, name, namelen)
	mach_port_t	file_port;
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
	int		syscode;
	char		*name;
	unsigned int	namelen;
{
	register int    error;
	struct file *fp;
	int arg[2];
	int serial = !sysent[syscode].sy_parallel;
        union {
                struct sockaddr_un soun;
                char dummy[sizeof(struct sockaddr_un)+1];
        } usoun;
	extern int	bind();
	extern int	connect();

	if (namelen > sizeof(struct sockaddr_un))
		return(EINVAL);
        usoun.soun = *(struct sockaddr_un *)name;
	usoun.soun.sun_path[namelen - SOCKADDR_UN_PADLEN] = '\0';
	error = start_filevnode_op(&fp, file_port, cwd_port, root_port,
				creds_port, transid,
				usoun.soun.sun_path,
				namelen - SOCKADDR_UN_PADLEN,
				syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s,%d)",usoun.soun.sun_path, namelen));
	arg[0] = (int) &usoun.soun;
	arg[1] = namelen;
	error = (syscode == SYS_bind ? bind : connect)(fp, arg, 0);

	/*
	 * unp_bind is not supposed to return on remote mount points,
	 * as it handles the forwarding itself.  GULP
	 */
	if (error == EREMOTE) {
		panic("S_fsvr_uds_bindconn: got EREMOTE");
	}
	return(end_filevnode_op(fp, cwd_port, root_port, error,
				serial));
}


/*
 * We might be calling unp_connect to bind to "name"
 */
int
S_fsvr_uds_sendto_short(file_port, cwd_port, creds_port, transid,
		root_port, flags, name, namelen, data, data_count,
		amount_written)
	mach_port_t	file_port;
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
	int		flags;
	char		*name;
	unsigned int	namelen;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* out */
{
	register int    error;
	struct file *fp;
	int arg[5];
        int syscode = SYS_sendto;
	int serial = !sysent[syscode].sy_parallel;
        union {
                struct sockaddr_un soun;
                char dummy[sizeof(struct sockaddr_un)+1];
        } usoun;

	if (namelen > sizeof(struct sockaddr_un))
		return(EINVAL);
        usoun.soun = *(struct sockaddr_un *)name;
        usoun.soun.sun_path[namelen - SOCKADDR_UN_PADLEN] = '\0';

	error = start_filevnode_op(&fp, file_port, cwd_port, root_port,
				creds_port, transid,
				usoun.soun.sun_path,
				namelen - SOCKADDR_UN_PADLEN,
				syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s,%d)",usoun.soun.sun_path, namelen));
	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	arg[3] = (int)&usoun.soun;
	arg[4] = namelen;
	error = sendto(fp, arg, amount_written);

	/*
	 * We are not supposed to return on remote mount points,
	 * as it unp_connect handles the forwarding itself.  GULP
	 */
	if (error == EREMOTE) {
		panic("S_fsvr_uds_sendto_short: got EREMOTE");
	}
	return(end_filevnode_op(fp, cwd_port, root_port, error,
				serial));
}


/*
 * We might be calling unp_connect to bind to "name"
 */
int
S_fsvr_uds_sendto_long(file_port, cwd_port, creds_port, transid,
		root_port, flags, name, namelen, data, data_count,
		amount_written)
	mach_port_t	file_port;
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
	int		flags;
	char		*name;
	unsigned int	namelen;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* out */
{
	register int    error;
	struct file *fp;
	int arg[5];
        int syscode = SYS_sendto;
	int serial = !sysent[syscode].sy_parallel;
        union {
                struct sockaddr_un soun;
                char dummy[sizeof(struct sockaddr_un)+1];
        } usoun;
	kern_return_t	kr;

	if (namelen > sizeof(struct sockaddr_un))
		return(EINVAL);
        usoun.soun = *(struct sockaddr_un *)name;
        usoun.soun.sun_path[namelen - SOCKADDR_UN_PADLEN] = '\0';
	error = start_filevnode_op(&fp, file_port, cwd_port, root_port,
				creds_port, transid,
				usoun.soun.sun_path,
				namelen - SOCKADDR_UN_PADLEN,
				syscode, serial);
	if (error)
		return(error);

	SC_TRACE(("(%s,%d)",usoun.soun.sun_path, namelen));
	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	arg[3] = (int)&usoun.soun;
	arg[4] = namelen;
	error = sendto(fp, arg, amount_written);

	/*
	 * We are not supposed to return on remote mount points,
	 * as it unp_connect handles the forwarding itself.  GULP
	 */
	if (error == EREMOTE) {
		panic("S_fsvr_uds_sendto_long: got EREMOTE");
	}

	/*
	 * WARNING:
	 *
	 * We don't want to deallocate the data buffer if the sendto failed.
	 *
	 * Specifically, if (error != 0), then the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(), (vm_address_t)data, data_count);
		if (kr != KERN_SUCCESS) {
		panic("S_fsvr_uds_sendto_long: vm_deallocate(0x%x,%d) = 0x%x\n",
			data, data_count, kr);
		}
	}
	return(end_filevnode_op(fp, cwd_port, root_port, error, serial));
}


/*
 * syscode is either SYS_sendmsg or SYS_osendmsg
 *
 * Note that name is contained in msg; but I didn't want to use copyin to get it
 */
int
S_fsvr_uds_sendmsg(file_port, cwd_port, creds_port, transid,
		root_port, syscode, flags, name, namelen, msg, amount_written)
	mach_port_t	file_port;
        mach_port_t     cwd_port;
        mach_port_t     creds_port;
	transaction_id_t	transid;
        mach_port_t     root_port;
	int		syscode;
	int		flags;
	char		*name;
	unsigned int	namelen;
	caddr_t		msg;
	int		*amount_written;	/* out */
{
	register int    error;
	struct file *fp;
	int arg[2];
	int serial = !sysent[syscode].sy_parallel;
        union {
                struct sockaddr_un soun;
                char dummy[sizeof(struct sockaddr_un)+1];
        } usoun;
	extern int	sendmsg();
#ifdef COMPAT_43
	extern int	osendmsg();
#endif

	if (namelen > sizeof(struct sockaddr_un))
		return(EINVAL);
        usoun.soun = *(struct sockaddr_un *)name;
        usoun.soun.sun_path[namelen - SOCKADDR_UN_PADLEN] = '\0';

	error = start_filevnode_op(&fp, file_port, cwd_port, root_port,
				creds_port, transid,
				usoun.soun.sun_path,
				namelen - SOCKADDR_UN_PADLEN,
				syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s,%d)",usoun.soun.sun_path, namelen));
	arg[0] = (int) msg;
	arg[1] = flags;
#ifdef COMPAT_43
	error = (syscode == SYS_sendmsg ? sendmsg : osendmsg)
#else
	error = sendmsg
#endif
		(fp, arg, amount_written);

	/*
	 * unp_bind is not supposed to return on remote mount points,
	 * as it handles the forwarding itself.  GULP
	 */
	if (error == EREMOTE) {
		panic("S_fsvr_uds_sendmsg: got EREMOTE");
	}
	return(end_filevnode_op(fp, cwd_port, root_port, error, serial));
}

int
S_fsvr_token_acquire(file_port, creds_port, flags, revoke_port,
		     token, offset, length)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	int		flags;
	mach_port_t	revoke_port;
	mach_port_t	*token;
	off_t		*offset;
	int		*length;
{
#if 	MAPPED_FILES
        register int    error;
        struct file 	*fp;
        int 		arg[6];
        int 		syscode = 2000;
        int 		serial = FALSE;

        error = start_fileserver_op(&fp, file_port, creds_port, 
                                    /*transid*/0, syscode, serial);
        if (error)
                return(error);

	arg[0] = flags;
	arg[1] = revoke_port;
	arg[2] = (int)token;
	arg[3] = (int)offset;
	arg[4] = (int)length;
	error = mf_token_acquire(fp, arg);

        return(end_fileserver_op(fp, error, serial));

#else	MAPPED_FILES
	return(EINVAL);
#endif	MAPPED_FILES
}

int
S_fsvr_token_acquire_with_mo(file_port, creds_port, flags, 
			     revoke_port, token, offset, length, mem_obj)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	int		flags;
	mach_port_t	revoke_port;
	mach_port_t	*token;
	off_t		*offset;
	int		*length;
	mach_port_t	*mem_obj;	/* OUT */
{
#if 	MAPPED_FILES
        register int    error;
        struct file 	*fp;
        int 		arg[6];
        int 		syscode = 2001;
        int 		serial = FALSE;
	mach_port_t	token_port;
        memory_object_t	mo;

        error = start_fileserver_op(&fp, file_port, creds_port, 
                                    /*transid*/0, syscode, serial);
        if (error)
                return(error);

	arg[0] = flags;
	arg[1] = revoke_port;
	arg[2] = (int)&token_port;
	arg[3] = (int)offset;
	arg[4] = (int)length;
	arg[5] = (int)&mo;
	error = mf_token_acquire_with_mo(fp, arg);
	if (error == 0) {
		*token = token_port;
		*mem_obj = mo;
	} else {
		*token = MACH_PORT_NULL;
		*mem_obj = MACH_PORT_NULL;
	}
	
        return(end_fileserver_op(fp, error, serial));

#else	MAPPED_FILES
	return(EINVAL);
#endif	MAPPED_FILES
}

int
S_fsvr_get_window(file_port, creds_port, offset, size, addr)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	off_t		offset;	
	int		size;	
	vm_address_t	*addr;
{
#if 	MAPPED_FILES
        register int    error;
        struct file 	*fp;
        int 		arg[3];
        int 		syscode = 2002;
        int 		serial = FALSE;

        error = start_fileserver_op(&fp, file_port, creds_port, 
                                    /*transid*/0, syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)offset;
	arg[1] = size;
	arg[2] = (int)addr;
	error = mf_get_window(fp, arg);

        return(end_fileserver_op(fp, error, serial));

#else	MAPPED_FILES
	return(EINVAL);
#endif	MAPPED_FILES
}

int
S_fsvr_token_release(token_port, move_token_right1, move_token_right2, 
		     offset, length, accessed, modified, min_offset, 
		     max_offset)
	mach_port_t	token_port;
	mach_port_t	move_token_right1;
	mach_port_t	move_token_right2;
	off_t		offset;	
	int		length;	
	int		accessed;
	int		modified;
	off_t		min_offset;
	off_t		max_offset;
{
#if 	MAPPED_FILES
        register int    error;
        token_info_t	*tip;
        int 		arg[7];
        int 		syscode = 2005;

	/*
	 * Note:  move_token_right1 and move_token_right2 are used as a way 
	 * of moving token_port send rights from the emulator to the 
	 * server (thus avoiding a no-senders message).  It's possible
	 * move_token_right2 is NULL, in which case only one send right was
	 * moved to the server.
	 */
        error = start_token_op(&tip, token_port, syscode);
        if (error)
                return(error);

	arg[0] = (int)offset;
	arg[1] = length;
	arg[2] = accessed;
	arg[3] = modified;
	arg[4] = (int)min_offset;
	arg[5] = (int)max_offset;
	arg[6] = (int)(move_token_right2 == MACH_PORT_NULL ? 1 : 2);
	error = mf_token_release(tip, arg);

        return(end_token_op(tip, error));

#else	MAPPED_FILES
	return(EINVAL);
#endif	MAPPED_FILES
}

int
S_fsvr_token_not_found(token_port, move_token_right)
	mach_port_t	token_port;
	mach_port_t	move_token_right;
{
#if 	MAPPED_FILES | PFS
        register int    error;
        token_info_t	*tip;
        int 		syscode = 2004;

	/*
	 * Note:  move_token_right is used as a way of moving the token_port
	 * send rights from the emulator to the server (thus avoiding
	 * a no-senders message).
	 */
        error = start_token_op(&tip, token_port, syscode);
        if (error)
                return(error);

	error = mf_token_not_found(tip);
	
        return(end_token_op(tip, error));

#else	MAPPED_FILES | PFS
	return(EINVAL);
#endif	MAPPED_FILES | PFS
}

int
S_fsvr_token_change(token_port, move_token_right, flags, token)
	mach_port_t	token_port;
	mach_port_t	move_token_right;
	int		flags;
	mach_port_t	*token;
{
#if 	MAPPED_FILES | PFS
        register int    error;
	mach_port_t	temp;
        token_info_t	*tip;
        int 		arg[2];
        int 		syscode = 2003;

	/*
	 * Note:  move_token_right is used as a way of moving the token_port
	 * send rights from the emulator to the server (thus avoiding
	 * a no-senders message).
	 */
        error = start_token_op(&tip, token_port, syscode);
        if (error)
                return(error);

	arg[0] = flags;
	arg[1] = (int)&temp;
	error = mf_token_change(tip, arg);
	if (error == 0) 
		*token = temp;
	else 
		*token = MACH_PORT_NULL;
	
        return(end_token_op(tip, error));

#else	MAPPED_FILES | PFS
	return(EINVAL);
#endif	MAPPED_FILES | PFS
}

int
S_fsvr_sync_data(token_port, length, min_offset, max_offset)
	mach_port_t	token_port;
        size_t		length;
        off_t           min_offset;
        off_t           max_offset;
{
#if     MAPPED_FILES
        register int    error;
        token_info_t    *tip;
        int             arg[3];
        int             syscode = 2006;

        error = start_token_op(&tip, token_port, syscode);
        if (error)
                return(error);

        arg[0] = length;
        arg[1] = (int)min_offset;
        arg[2] = (int)max_offset;
	error = mf_sync_data(tip, arg);
        return(end_token_op(tip, error));
#else	MAPPED_FILES
	return(EINVAL);
#endif	MAPPED_FILES
}


/*
 * call cleanlocks for the emulator.  The traditional close(2) calls closef
 * on for every file descriptor close.  Since the emulator does some close
 * processing and closef ends up being called only on the last close of the
 * file structure, the emulator needs to call cleanlocks.
 */
int
S_fsvr_cleanlocks(file_port, creds_port)
	mach_port_t	file_port;
	mach_port_t	creds_port;
{
	register int	error;
	struct file *fp;

	error = start_fileserver_op(&fp, file_port, creds_port, NULL,
								NULL, NULL);
	if (error)
		return(error);

	cleanlocks(fp);

	return(end_fileserver_op(fp, error, NULL));
}



int
S_fsvr_file_ref(file_port, n)
	mach_port_t	file_port;
	int		n;
{
	register int	error;
	struct file *fp;

	/*
	 * Get further references to the file structure.  Called at fork
	 * time to get the reference for the child.  Also increment
	 * f_makesend since this reference represents a new send right.
	 *
	 * This is used with n>1 for multi-child creation to avoid repeated
	 * fileserver operations. Furthermore, n<0 is used to discard
	 * redundant references in cases where it is know that the file 
	 * will not become completely un-referenced.
	 */
	error = start_fileserver_op(&fp, file_port, MACH_PORT_NULL,
				    NULL, NULL, NULL);
	if (error)
		return(error);

	FP_LOCK(fp);
	fp->f_count += n;				/* FP_REF */
	fp->f_makesend += n;
	FP_UNLOCK(fp);

	return(end_fileserver_op(fp, error, NULL));
}

int
S_fsvr_file_unref(file_port, creds_port, move_file_right)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	mach_port_t	move_file_right;
{
	register int	error;
	struct file *fp;

	error = start_fileserver_op(&fp, file_port, creds_port, NULL,
								NULL, NULL);
	if (error)
		return(error);

	/*
	 * Release a user's reference to the file structure and remove a
	 * send right from the port if it isn't the last one.  If it is
	 * the last one, the port will be destroyed when closef is called
	 * from fp_unref_port.
	 *
	 * One file reference is released for the user and the other is
	 * the one obtained by start_fileserver_op.  These two file refs
	 * and the send right all have to be released together while FP_LOCK
	 * is held to avoid races with NMS processing.  Since the file
	 * references are already released by fp_unref_port, a modified
	 * version of end_fileserver_op that doesn't do FP_UNREF is called.
	 */
	error = fp_unref_port(fp, 2);
	return(end_fileserver_op_nounref(fp, error, NULL));
}

#ifdef	PFS
S_fsvr_mount_pfs(cwd_port, creds_port, transid, root_port, dev_port,
		 pathname, pathname_len, abs_pathname, abs_plen, flags, spec,
		 spec_len, exflags, exroot, args, args_len)
	mach_port_t     	cwd_port;
	mach_port_t     	creds_port;
	transaction_id_t	transid;
	mach_port_t     	root_port;
	mach_port_t     	dev_port;
	char            	*pathname;
	int			pathname_len;
	char			*abs_pathname;
	int			abs_plen;
	int             	flags;
	char			*spec;
	int			spec_len;
	int			exflags;
	int			exroot;
	struct pfs_args		*args;
	int			args_len;
{
	register int	error;
	int		arg[5];
	struct ufs_args	*ufs_argsp = &args->fs_args;
	int		syscode = SYS_mount;
	int		serial = !sysent[syscode].sy_parallel;
	kern_return_t	kr;

	error = start_vnodeserver_op(cwd_port, root_port, dev_port,
				     creds_port, transid, pathname, 
	                             pathname_len, syscode, serial);
	if (error) 
		return(error);

	SC_TRACE(("(%s,%s)",pathname,spec));

	ufs_argsp->fspec = spec;
	ufs_argsp->exflags = exflags;
	ufs_argsp->exroot = exroot;

	arg[0] = MOUNT_PFS;
	arg[1] = (int)pathname;
	arg[2] = flags;
	arg[3] = (int)args;
        arg[4] = (int)abs_pathname;
	error = mount_ad(u.u_procp, arg, 0);

	/*
	 * If a remote mount point was detected, forward the mount message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;
  
		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_mount_pfs(ndp->ni_forwport, creds_port, transid,
				       root_port, dev_port,
				       ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				       abs_pathname, abs_plen,
				       flags, spec, spec_len, exflags, exroot,
				       (char_array)args, args_len);
		OIP_END_FORW(oipp);
	}

	/*
	 * Deallocate VM allocated by MiG when passing out-of-line data.
	 *
	 * Actually, we only want to do that for successful calls.
	 * For error cases (if (error != 0)), the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(), (vm_address_t)args, args_len);
		if (kr != KERN_SUCCESS) {
		panic("S_fsvr_mount_pfs: vm_deallocate(0x%x,%d) returns 0x%x\n",
			args, args_len, kr);
		}
	}

	return(end_vnodeserver_op(cwd_port, root_port, dev_port, error,
				  serial));
}


int
S_fsvr__lsize(file_port, creds_port, transid, off, sbase, newlen)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	u_long			off;
	int			sbase;
	u_long			*newlen;	/* out */
{
	register int	error;
	struct file *fp;
	u_int arg[2];
	int syscode = SYS__lsize;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, 
				    transid, syscode, serial);
	if (error) 
		return(error);

	arg[0] = (u_int)off;
	arg[1] = sbase;
	error = _lsize(fp, arg, newlen);

	return(end_fileserver_op(fp, error, serial));
}


int
S_fsvr_unshare(oldfl_port, creds_port, transid, newfl_port)
        mach_port_t     	oldfl_port;
        mach_port_t     	creds_port;
	transaction_id_t	transid;
        mach_port_t     	*newfl_port;	/* out */
{
	register int	error;
	struct file	*old_fp;
	struct file 	*new_fp;
	int		syscode = 0;
	int		serial = FALSE;


	*newfl_port = MACH_PORT_NULL;
	error = start_fileserver_op(&old_fp,
				    oldfl_port,
				    creds_port,
				    transid,
				    syscode,
				    serial);
	if (error) {
		return(error);
	}

	error = _pfs_unshare(old_fp, &new_fp);
	if (error == 0) {
		FILE_TO_PORT_LOOKUP(new_fp, *newfl_port);
	}

	error = end_fileserver_op(old_fp, error, serial);
	return error;
}


/*
 * pfs_unshare:	Create a new single reference file table entry from an
 *		existing file table entry.
 */
int
_pfs_unshare(old_fp, new_fp)
	struct file	*old_fp;	/* Pointer to the fp to unshare. */
	struct file	**new_fp;	/* Pointer to the new fp. */
{
	int 		error;
	struct vnode	*vp; 
	struct file	*nfp;

	/*
	 * Allocate a new fp.
	 */
	if (error = falloc(new_fp)) {
		return error;
	}
	nfp = *new_fp;

	/*
	 * Lock the original fp.
	 */
        FP_LOCK(old_fp);

	/*
	 * Increment vnode reference.
	 */
	vp = (struct vnode *)old_fp->f_data;	
	VREF(vp);

	/*
	 * Lock the vnode of the orignal fp.
	 */
	VN_LOCK(vp);

	/*
	 * Fill in the needed information.
	 */
	nfp->f_flag  	= old_fp->f_flag;	/* Open flags. */
	nfp->f_ops 	= old_fp->f_ops;	/* file ops. */
	nfp->f_data 	= old_fp->f_data;	/* Vnode reference. */
	nfp->f_offset 	= old_fp->f_offset;	/* Fd offset. */
	nfp->f_type 	= old_fp->f_type;	/* Descriptor type. */
	nfp->f_msgcount	= 0;			/* IPC Stuff. */
	nfp->f_count 	= 1;			/* Reference count */

	if (VIO_IS_PFS(vp)) {
		nfp->pfs_offset.shigh = old_fp->pfs_offset.shigh;
		nfp->pfs_offset.slow  = old_fp->pfs_offset.slow;
	}

	/*
	 * Increment vnode references:
	 */
	if ( nfp->f_flag & FREAD) { 
		vp->v_rdcnt++;
	}
	if ( nfp->f_flag & FWRITE) { 
		vp->v_wrcnt++;
	}

	/*
	 * Unlock the vnode and fp:
	 */
	FP_UNLOCK(old_fp);
	VN_UNLOCK((struct vnode *)old_fp->f_data);

	return 0;
}


int
S_fsvr_statpfs(cwd_port, creds_port, transid, root_port, pathname,
		pathname_len, statpfsb_len, statpfsb, actual_len)
	mach_port_t		cwd_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	mach_port_t		root_port;
	char			*pathname;
	unsigned int		pathname_len;
	int			statpfsb_len;	/* length of supplied buffer */
	struct statpfs		**statpfsb;	/* out, dealloc (ool memory) */
	int			*actual_len;	/* out, size of the above */
{
	register int    error;
	int arg[3];
	int syscode = SYS_statpfs;
	int serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*statpfsb = 0;
	*actual_len = 0;

	error = start_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL,
				     creds_port, transid, pathname,
				     pathname_len, syscode, serial);
	if (error) 
		return(error);

	if (vm_allocate(mach_task_self(), (vm_offset_t *)statpfsb, 
			statpfsb_len, TRUE) != KERN_SUCCESS) {
		error = ENOMEM;
        } else {
		SC_TRACE(("(%s)",pathname));
		arg[0] = (int)pathname;
		arg[1] = (int)*statpfsb;
		arg[2] = (int)statpfsb_len;
		error =	statpfs(u.u_procp, arg, actual_len);
	}

	/*
	 * If a remote mount point was detected, forward the statpfs message.
	 */
	if (error == EREMOTE) {
		struct nameidata *ndp = &u.u_nd;
		struct server_oip *oipp = &u.uu_oip;
  
 		/*
 		 * We need to deallocate the vm_allocated mem above since in
 		 * next call we will be vm_allocating again. 
 		 */
 		vm_deallocate_strict(mach_task_self(), 
 				     (vm_address_t) *statpfsb, 
 				     statpfsb_len);
 		*statpfsb = 0;
 		*actual_len = 0;
 
		OIP_SET_FORW(oipp, ndp->ni_forwport);
		error = fsvr_statpfs(ndp->ni_forwport, creds_port,
				     transid, root_port,
				     ndp->ni_ptr, strlen(ndp->ni_ptr)+1,
				     statpfsb_len, (char_array *)statpfsb,
				     (mach_msg_type_number_t *)actual_len);
		OIP_END_FORW(oipp);
	}

	error = end_vnodeserver_op(cwd_port, root_port, MACH_PORT_NULL, error,
				   serial);
	if (*statpfsb) {
		if (error) {
			/*
			 * Deallocate the buffer that was allocated above.
			 */
			vm_deallocate_strict(mach_task_self(),
					(vm_address_t) *statpfsb,
					statpfsb_len);
			*statpfsb = 0;
			*actual_len = 0;
		} else {
			/*
			 * Deallocate any unused pages in the buffer.
			 */
			statpfsb_len -= round_page(*actual_len);
			if (statpfsb_len > 0) {
				vm_address_t	start;
				start = (vm_address_t) *statpfsb +
					round_page(*actual_len);
				vm_deallocate_strict(mach_task_self(), 
						     start, statpfsb_len);
			}
		}
	}		
	return(error);
}


int
S_fsvr_fstatpfs(file_port, creds_port, transid,
		statpfsb_len, statpfsb, actual_len)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			statpfsb_len;	/* length of supplied buffer */
	struct statpfs		**statpfsb;	/* out, dealloc (ool memory) */
	int			*actual_len;	/* out, size of the above */
{
	register int    error;
	int		arg[2];
	int		syscode = SYS_fstatpfs;
	int		serial = !sysent[syscode].sy_parallel;
	struct file	*fp;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*statpfsb = 0;
	*actual_len = 0;

        error = start_fileserver_op(&fp, file_port, creds_port,
                                    transid, syscode, serial);
	if (error) 
		return(error);

	if (vm_allocate(mach_task_self(), (vm_offset_t *)statpfsb, 
			statpfsb_len, TRUE) != KERN_SUCCESS) {
		error = ENOMEM;
        } else {
		arg[0] = (int)*statpfsb;
		arg[1] = (int)statpfsb_len;
		error =	fstatpfs(fp, arg, actual_len);
	}

	error = end_fileserver_op(fp, error, serial);
	if (*statpfsb) {
		if (error) {
			/*
			 * Deallocate the buffer that was allocated above.
			 */
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *statpfsb, 
					     statpfsb_len);
			*statpfsb = 0;
			*actual_len = 0;
		} else {
			/*
			 * Deallocate any unused pages in the buffer.
			 */
			statpfsb_len -= round_page(*actual_len);
			if (statpfsb_len > 0) {
				vm_address_t	start;
				start = (vm_address_t) *statpfsb + 
					round_page(*actual_len);
				(void) vm_deallocate(mach_task_self(), 
						     start, statpfsb_len);
			}
		}
	}		
	return(error);
}


int
S_fsvr_pfs_fdevstat(file_port, creds_port, transid, pstat)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
        struct pfs_stat		*pstat;
{
	register int    error;
	int arg[1];
	int syscode = 	0;
	int serial = 	FALSE;
	struct file	*fp;

        error = start_fileserver_op(&fp, file_port, creds_port,
                                    transid, syscode, serial);
	if (error) 
		return(error);
	arg[0] = (int)pstat;
	error = pfs_fdevstat(fp, arg, 0);

        return(end_fileserver_op(fp, error, serial));
}


int
S_fsvr_pfs_token_acquire(file_port, creds_port, flags, revoke_port,
		     token, offset, length)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	int		flags;
	mach_port_t	revoke_port;
	mach_port_t	*token;
	esize_t		*offset;
	esize_t		*length;
{
        register int    error;
        struct file 	*fp;
        int 		arg[6];
        int 		syscode = 2000;
        int 		serial = FALSE;

        error = start_fileserver_op(&fp, file_port, creds_port, 
                                    /*transid*/0, syscode, serial);
        if (error)
                return(error);

	arg[0] = flags;
	arg[1] = revoke_port;
	arg[2] = (int)token;
	arg[3] = (int)offset;
	arg[4] = (int)length;

	error = mf_token_acquire(fp, arg);
        return(end_fileserver_op(fp, error, serial));

	return(EINVAL);
}


int
S_fsvr_pfs_token_release(token_port, move_token_right1, move_token_right2, 
		     offset, length, accessed, modified)

	mach_port_t	token_port;
	mach_port_t	move_token_right1;
	mach_port_t	move_token_right2;
	esize_t		offset;	
	esize_t		length;	
	int		accessed;
	int		modified;
{
        register int    error;
        token_info_t	*tip;
        int 		arg[7];
        int 		syscode = 2005;

	/*
	 * Note:  move_token_right1 and move_token_right2 are used as a way 
	 * of moving token_port send rights from the emulator to the 
	 * server (thus avoiding a no-senders message).  It's possible
	 * move_token_right2 is NULL, in which case only one send right was
	 * moved to the server.
	 */
        error = start_token_op(&tip, token_port, syscode);
        if (error)
                return(error);

	arg[0] = (int)&offset;
	arg[1] = (int)&length;
	arg[2] = accessed;
	arg[3] = modified;
	arg[4] = 0;
	arg[5] = 0;
	arg[6] = (int)(move_token_right2 == MACH_PORT_NULL ? 1 : 2);
	error = mf_token_release(tip, arg);
        return(end_token_op(tip, error));
}


int
S_fsvr_pfs_update_off(file_port, offset)
	mach_port_t             file_port;
	esize_t                 offset;
{
	register int    error=ESUCCESS;
	struct file *fp;
 
	error = start_fileserver_op(&fp, file_port, MACH_PORT_NULL, NULL,
				    NULL, NULL);
	if (error)
		return(error);
 
	FP_LOCK(fp);
	fp->pfs_offset.shigh = offset.shigh;
	fp->pfs_offset.slow =  offset.slow;
	FP_UNLOCK(fp);
	return(end_fileserver_op(fp, error, NULL));
}

int
S_fsvr_pfs_get_off(file_port, offset)
	mach_port_t             file_port;
	esize_t                 *offset;
{
	register int    error=ESUCCESS;
	struct file *fp;
 
	error = start_fileserver_op(&fp, file_port, MACH_PORT_NULL, NULL,
				    NULL, NULL);
	if (error)
		return(error);
 
	FP_LOCK(fp);
	offset->shigh = fp->pfs_offset.shigh;
	offset->slow  = fp->pfs_offset.slow;
	FP_UNLOCK(fp);
	return(end_fileserver_op(fp, error, NULL));
}

int
S_fsvr_pfs_async_dflt(file_port, async_dflt)
	mach_port_t             file_port;
	int			*async_dflt;
{
	*async_dflt = pfs_async_dflt; 
	return ESUCCESS;
}

/*
 * This routine duplicate an existing file table entry and returns the
 * corresponding port to each member of the input port vector:
 */
int
S_fsvr_duplicate(oldfl_port, creds_port, transid,
		 response_id, remote_ports, count)
        mach_port_t     	oldfl_port;
        mach_port_t     	creds_port;
	transaction_id_t	transid;
	uint_t			response_id;	/* in, id needed by emulator  */
	mach_port_t		*remote_ports;	/* in, dealloc (ool memory)   */
        int                     count;		/* in, number of ports in
						     remote_ports (and number
						     of file table entries
						     to duplicate)            */
{
        mach_port_t     	file_port;
	int			i1;
	kern_return_t           mach_error;
	mach_msg_return_t       message_rtn;
	int			msg_hdr_bits;
	struct file 		*new_fp;
	struct file		*old_fp;

	struct dup_send_msg {
	    mach_msg_header_t    hdr;
	    mach_msg_type_t      type;
	    mach_port_t          file_port;
	} dup_send_msg;

	int		error = 0;
	int		syscode = 0;
	int		serial = FALSE;
	int             unshare_error;

#define GOPEN_2   0x5a5a0000


	error = start_fileserver_op(&old_fp,
				    oldfl_port,
				    creds_port,
				    transid,
				    syscode,
				    serial);
	if (error) {
	    return(error);
	}

	unshare_error = 0;
	for (i1 = 0; i1 < count; i1++) {
	    /*
	     * If no errors have occured on a previous loop iteration,
	     * then duplicate the file table entry, which also creates
	     * the corresponding file port:
	     */
	    if (!unshare_error) {
		unshare_error = _pfs_unshare(old_fp, &new_fp);
	    }

	    if (unshare_error) {
		file_port = MACH_PORT_NULL;
	    } else {
		FILE_TO_PORT_LOOKUP(new_fp, file_port);
	    }

	    /*
	     * send the duplicated file_port to the
	     * compute node via the port vector entry:
	     */
	    msg_hdr_bits = MACH_MSGH_BITS_COMPLEX |
			   MACH_MSGH_BITS(MACH_MSG_TYPE_COPY_SEND, 0);

	    dup_send_msg.hdr.msgh_bits        = msg_hdr_bits;
	    dup_send_msg.hdr.msgh_size        = sizeof(dup_send_msg);
	    dup_send_msg.hdr.msgh_remote_port = remote_ports[i1];
	    dup_send_msg.hdr.msgh_local_port  = MACH_PORT_NULL;
	    dup_send_msg.hdr.msgh_seqno       = 0;
	    dup_send_msg.hdr.msgh_id          = GOPEN_2 + response_id;

	    dup_send_msg.type.msgt_name       = MACH_MSG_TYPE_MOVE_SEND;
	    dup_send_msg.type.msgt_size       = 32;
	    dup_send_msg.type.msgt_number     = 1;
	    dup_send_msg.type.msgt_inline     = TRUE;
	    dup_send_msg.type.msgt_longform   = FALSE;
	    dup_send_msg.type.msgt_deallocate = FALSE;
	    dup_send_msg.type.msgt_unused     = 0;

	    dup_send_msg.file_port            = file_port;

	    message_rtn = mach_msg(&dup_send_msg,
				   MACH_SEND_MSG,
				   sizeof(dup_send_msg),
				   0, MACH_PORT_NULL,
				   MACH_MSG_TIMEOUT_NONE,
				   MACH_PORT_NULL);
	    if (message_rtn != MACH_MSG_SUCCESS) {
		printf("fsvr_duplicate(): mach_msg %s\n",
		       mach_error_string(message_rtn));
	    }

	    /*
	     * release this server's reference to the remote port:
	     */
	    mach_error = mach_port_deallocate(mach_task_self(),
					      remote_ports[i1]);
	    if (mach_error != KERN_SUCCESS) {
		printf("fsvr_duplicate(): mach_port_deallocate %s\n",
		       mach_error_string(mach_error));
	    }

	}

	error = end_fileserver_op(old_fp, error, serial);


	/*
	 * deallocate the port vector array:
	 */
	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t)remote_ports,
			     count * sizeof(mach_port_t));

	if ((!error) && unshare_error) {
	    error = unshare_error;
	}
	return(error);
}

#endif	PFS
