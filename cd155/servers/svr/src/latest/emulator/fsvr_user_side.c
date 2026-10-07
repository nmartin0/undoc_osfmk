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
 * HISTORY
 * $Log: fsvr_user_side.c,v $
 * Revision 1.67  1995/02/18  01:18:22  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12240, including emul console logging cleanup
 *  Testing: EATs controlc, sched
 *  Module(s):
 * 	svr/emulator/bsd_user_side.c
 * 	svr/emulator/emul_chkpnt.c
 * 	svr/emulator/emul_init.c
 * 	svr/emulator/emul_mapped.c
 * 	svr/emulator/fsvr_user_side.c
 * 	svr/server/bsd/kern_sig.c
 * 	svr/server/bsd/mach_signal.c
 * 	svr/server/bsd/subr_prf.c
 * 	svr/server/conf/makesyscalls.sh
 * 	svr/server/tnc/dvp_vpops.c
 * 	svr/server/uxkern/boot_config.c
 * 	svr/server/uxkern/bsd_server_side.c
 * 	svr/server/uxkern/credentials.c
 * 	svr/server/uxkern/rpm_clock.c
 *
 * General cleanup of emulator console logging.  Added bootnode_printf()
 * routine to server.  Added server bootmagic variable ENABLE_RPM_TIMESTAMP
 * so printf() and bootnode_printf() messages are timestamped with the
 * 56-bit RPM global clock value.  This enables very fine timings to be
 * observable in console log output.
 *
 * Revision 1.66  1995/02/01  21:23:15  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.65  1994/11/18  20:23:37  mtm
 * Copyright additions/changes
 *
 * Revision 1.64  1994/11/04  18:15:49  toman
 * Fixed CVS comments.
 *
 * Revision 1.63  94/11/04  18:11:58  toman
 *    Created an e_orecv() routine to check readability and writeability
 *    of user buffer in order for recv() to return EFAULT before going to the
 *    server.  Also, added user_rwcheck() in e_orecvfrom() and e_recvfrom()
 *    for large buffer sizes as well as small buffer sizes.
 *
 *  Reviewer: John Litvin, Bob Yasi
 *  Risk: med, based on # of lines.
 *  Benefit or PTS #: 9422, 11405
 *  Testing: Lachman t_recv test, TCP/IP EAT
 *  Module(s): emulator/fsvr_user_side.c
 * 	    server/conf/syscalls.master
 * 	    server/uxkern/bsd_1.defs
 * 	    server/uxkern/bsd_server_side.c
 * 
 * Revision 1.62  94/08/31  22:45:51  mtm
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
 * Revision 1.61.2.1  1994/08/19  22:30:21  dbm
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
 * Revision 1.61  1994/06/22  18:11:31  brad
 * Moved PFS code block in fcntl() above the F_SETFL code, so the "fall
 * through" case in the F_SETFL case is handled properly.
 *
 *  Reviewer: Bob Godley
 *  Risk: Low
 *  Benefit or PTS #: 9946
 *  Testing: Ran developer tests, failing test case
 *  Module(s): emulator/fsvr_user_side.c
 *
 * Revision 1.60  1994/06/22  17:34:17  rlg
 * Added check to close_on_exit() so that if the sig_exit argument is
 * non-zero, then queue_sync() is *not* called.  The queue_sync() routine
 * calls syscall_suspend_barrier(), which should not be called by thread
 * processing the control-C event.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:9378
 *  Testing:  failing test cases, filio and pfs EATs
 *  Module(s):  emulator/fsvr_user_side.c
 *
 * Revision 1.59  1994/06/21  20:12:31  yazz
 *  Checkin on behalf of: Chris Peak <chrisp>
 *  Reviewer: Bob Yasi
 *  Risk: very lo
 *  Benefit or PTS #: 9918
 *  Testing: by inspection
 *  Module(s): emulator/fsvr_user_side.c
 *
 * Make the last scan thru the file descriptor table in the fdt_port_modref()
 * function check for empty and reserved entries, like all other such scans do.
 *
 * Revision 1.58  1994/06/16  02:15:19  brad
 * Very minor changes to prevent lint warnings in pfs2_user_side.c
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: get rid of lint warnings
 *  Testing: built, booted, brief developer testing
 *  Module(s): emulator/{fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.57  1994/06/14  17:42:31  brad
 * Added R1.3 support for getting, setting, and temporarily mapping
 * per-file PFS stripe attributes via new F_GETSATTR and F_SETSATTR
 * fcntl() requests.  Also added an F_GETFULLSATTR request for use by ls
 * in getting full (including stripe file pathnames) stripe attributes.
 * Also modified default attributes given a file when it is created: the
 * start stripe directory is now a random member of the stripe group,
 * rather than always the first member, so small files consume disk space
 * evenly across the stripe group.
 *
 *  Reviewer: rlg (in progress)
 *  Risk: Med
 *  Benefit or PTS #: 7593, 9686
 *  Testing: Developer tests, PFS EATs on 64 nodes
 *  Module(s): server/{sys/fcntl.h,pfs/pfs.h}
 *             emulator/{pfs_fdt.h,fsvr_user_side.c,pfs2_user_side.c}
 *
 * Revision 1.56  1994/06/13  23:37:51  jlitvin
 * e_getsockopt() should verify its' avalsize pointer before using it.
 * This looks like it was an oversite.  See e_accept().
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 9826
 *  Testing: test case
 *  Module(s): emulator/fsvr_user_side.c
 *
 * Revision 1.55  1994/06/13  15:32:44  rlg
 * Added the M_ASYNC I/O mode for shared files.  This mode is characterized by:
 *     o	each node has a unique file pointer,
 *     o	nodes are not synchronized
 *     o	file access is unrestricted
 *     o	standard UNIX file sharing semantics requiring atomicity of I/O
 * 	are not preserved.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  medium
 *  Benefit or PTS #:  7480
 *  Testing:  I/O mode unit test; 132 Eval I/O tests; rw performance test;
 *  Module(s):  emulator/fsvr_user_side.c		libnx/_gopen.c
 * 		      pfs2_user_side.c		      _pfs_setio.c
 * 		      pfs_iomode.c		      _setiomode.c
 * 		      pfs_iomode.h		      gopen.c
 * 		      pfs_tokenmgt.c		      gopen_.c
 * 		      pfs_user_side.c		      pfs_iomode.h
 * 						      setiomode.c
 *
 * Revision 1.54  1994/06/02  22:11:35  chrisp
 * e_rforkmulti_call() and e_forkfamily_call() call new routines
 * fdt_get_rights() and fdt_port_modrefs() to assemble a table of file
 * ports to be transferred to the server. Note that the first 2 entries
 * in this table are the parent's root and current directory ports.
 * Explicit installation of these ports into child tasks and the
 * release of child emulator threads has been eliminated.  Fileserver
 * RPC fsvr_file_ref() now takes an extra parameter giving the reference
 * adjustment required.
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): bsd_user_side.c emul_chkpnt.c fsvr_user_side.c pfs2_user_side.c
 *
 * Revision 1.53  1994/05/25  23:44:54  brad
 * Fixed typo in an EPRINT that caused compilation to fail if DEBUG defined.
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: it compiles
 *  Testing: compiled
 *  Module(s): emulator/fsvr_user_side.c
 *
 * Revision 1.52  1994/04/20  20:30:32  rlg
 * merge of revision 1.37.2.12 from the R1.2 branch
 *
 * Revision 1.37.2.12  1994/04/20  18:41:28  rlg
 * The PFS close function was enhanced to close the stripe files in parallel.
 *
 *  Reviewer: Brad Rullman
 *  Risk: medium
 *  Benefit or PTS #: PTS #8953
 *  Testing: failing test case; pfs and fileio EATs
 *  Module(s): emulator/fsvr_user_side.c
 *             emulator/pfs2_user_side.c
 *             server/uxkern/pfs2.defs
 *             server/uxkern/pfs2_server_side.c
 *
 * Revision 1.51  1994/04/12  23:53:13  rlg
 * Merged the changes from version R1.37.2.11 on the R1.2 branch into the
 * trunk.
 *
 * Revision 1.37.2.11  1994/04/12  21:41:51  rlg
 * Fixed problem of the three "close_" routines not having the fix to
 * e_close() for PTS #6489.
 *
 *  Reviewer:  Brad Rullman
 *  Risk:  low
 *  Benefit or PTS #:  PTS #8880
 *  Testing:  failing test case; fileio and pfs EATs
 *  Module(s):  emulator/fsvr_user_side.c [e_close(),
 *                                         close_for_dup2(
 *                                         close_on_exit(),
 *                                         close_on_exec()]
 *
 * Revision 1.50  1994/03/31  18:16:47  dbm
 * Merge of R1.2 version 1.37.2.10.
 *
 * Revision 1.37.2.10  1994/03/31  18:07:23  dbm
 * Added proper error checking to e_fchdir().
 *  Reviewer: Brad Rullman
 *  Risk:Low
 *  Benefit or PTS #: 8724
 *  Testing: Specific test case.
 *  Module(s):
 * 	emulator/fsvr_user_side.c
 * 	server/uxkern/fsvr_server_side.c
 *
 * Revision 1.37.2.9  1994/03/25  00:13:21  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #8334
 *  Testing: Boot & full multiuser (only panic msgs changed)
 *  Module(s): emulator/fsvr_user_side.c
 *
 *
 * Differentiate the three identical emul_panic() messages in dopoll().
 * (Also make braces match despite ifdefs so ctags works on sourcefile.)
 *
 * Revision 1.49  1994/03/24  20:36:05  yazz
 *
 * Revision 1.48  1994/03/14  01:44:16  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 1.47  1994/03/11  18:54:23  rlg
 * Changes from the R1.2 branch merged into the R1.3 branch (1.37.2.8)
 *
 * Revision 1.37.2.8  1994/03/11  16:29:44  rlg
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
 * Revision 1.46  1994/03/09  01:21:46  dbm
 * Mainline checking of R1.2 revision 1.37.2.7
 *
 * Revision 1.37.2.7  1994/03/09  00:40:45  dbm
 * Added support for O_SYNC mode with mapped files.
 *  Reviewer: Brad Rullman
 *  Risk: Low
 *  Benefit or PTS #: 8420
 *  Testing: Specific test cases using O_SYNC mode, PFS eats.
 *  Module(s):
 * 	pfs_iomode.c
 * 	pfs_tokenmgt.c
 * 	fsvr_user_side.c
 *
 * Revision 1.45  1994/03/04  22:23:37  dbm
 * Mainline merge for bug 6919, (1.2 rev 1.37.2.6)
 *
 * Revision 1.37.2.6  1994/03/04  21:40:20  dbm
 * Added extra parameter to tmgr_release_to_server(), initialized
 * token_refcnt to zero.
 *  Reviewer: Brad Rullman
 *  Risk:M
 *  Benefit or PTS #:6919
 *  Testing: PFS EATS, Overlapping PFS Sats.
 *  Module(s):
 * 	fsvr_user_side.c
 *
 * Revision 1.44  1994/02/16  00:29:32  dbm
 * Merge from 1.2 branch revision 1.37.2.5
 *
 * Revision 1.37.2.5  1994/02/15  23:30:46  dbm
 * Made modifications to allow for registering NFS file operations with
 * asynchronous threads.  Also modified token_release_all to cordinate
 * release_state() operations in M_RECORD mode.
 *
 *  Reviewer: Bob Godley
 *  Risk:Low
 *  Benefit or PTS #:6919 and 8067
 *  Testing: Ran overlapping PFS Sat tests on 13 nodes, ran specific test
 * 	  cases, ran PFS eats several times.
 *  Module(s):
 * 	fsvr_user_side.c
 *
 * Revision 1.43  1994/02/04  19:46:21  brad
 * Modified extended math support so that: 1) Emath routines set a new
 * error parameter instead of relying on a return value of -1 on overflow.
 * The latter method did not handle valid return values of -1 (this caused
 * eseek with resulting offset of -1 to return EQESIZE instead of EINVAL,
 * for example).  2) The emath code can be reused by libesize.a and libnx.a,
 * instead of having multiple copies of the same code in different places.
 *  Reviewer: None.
 *  Risk: Low.
 *  Benefit or PTS #:
 *  Testing: Ran PFS EATs, ran emath tests.
 *  Module(s): fsvr_user_side.c pfs2_user_side.c pfs_emath.c pfs_fdt.h
 *             pfs_iomode.c pfs_tokenmgt.c pfs_user_side.c
 *
 * Revision 1.37.2.4  1994/01/09  00:18:27  brad
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
 *             emulator/pfs_fdt.h
 *             emulator/pfs_tokenmgt.c
 *             emulator/pfs_user_side.c
 *             emulator/fsvr_user_side.c
 *             emulator/pfs_iomode.c
 *             server/pfs/pfs_vfsops.c
 *             server/uxkern/fsvr.defs
 *             server/uxkern/fsvr2.defs
 *             server/uxkern/fsvr2_server_side.c
 *             server/uxkern/fsvr_server_side.c
 *             server/uxkern/fsvr_types.h
 *             server/uxkern/pfs2.defs
 *             server/uxkern/fsvr_types.defs
 *
 * Revision 1.42  1994/01/07  00:31:41  dbm
 *  Reviewer:None.
 *  Risk:Low
 *  Benefit or PTS #: Merged from 1.2 branch of bug #5686.  This was originally
 * 		   checked into 1.2 as 1.37.2.3.
 *  Testing:See 1.37.2.3 entry.
 *
 * Revision 1.37.2.3  1994/01/06  22:56:21  dbm
 * Fix for adding support to PFS I/O modes for NFS files.
 *  Reviewer: Bob Godley
 *  Risk: Medium
 *  Benefit or PTS #: 5686
 *  Testing: Ran specific test case also ran PFS Eats on 16 nodes using NFS
 *  	  files.
 *  Module(s): fsvr_user_side.c
 *
 * Revision 1.41  1994/01/05  17:08:12  brad
 * Fixed lint warnings in PFS-related code.
 *
 * Revision 1.40  1993/12/23  01:47:07  brad
 * Fixed various compilers warnings, lint errors, and lint warnings.
 *
 * Revision 1.39  1993/12/21  19:02:40  rlg
 * Merged the R1.37.2.2 revisions into the main trunk
 *
 * Revision 1.37.2.2  1993/12/16  21:01:56  rlg
 * Code added to e_close to block the process until all currently outstanding
 * asynchronous I/O requests have complete.
 *  Benefit or PTS #: 6486
 *
 * Revision 1.37.2.1  1993/12/08  17:12:35  cfj
 * Correct the read semantics for /dev/null.  Read should always return
 * a count of zero.
 *
 *  Reviewer:jlitvin
 *  Risk:Low
 *  Benefit or PTS #:7409
 *  Testing:Test case and Unix EATs now pass.
 *  Module(s):emulator/fvsr_user_side.c
 *
 * Revision 1.37  1993/11/30  23:16:01  cfj
 * Bit bucket reads and writes to /dev/null at the emulator instead of sending
 * RPCs to the file server node.
 *
 *  Reviewer:brad, dbm
 *  Risk:M
 *  Benefit or PTS #:7261
 *  Testing:
 *  Module(s):server/sys/vnode.h
 * 	   server/uxkern/device_misc.c
 * 	   server/vfs/spec_vnops.c
 * 	   emulator/fsvr_user_side.c
 *
 * Revision 1.36  1993/09/28  21:34:48  brad
 * Disabled the open_with_token RPC interface, since it doesn't make sense
 * for shared files.
 *
 * Revision 1.35  1993/09/27  18:03:19  dbm
 * Fixed bug which was causing exit's to hang when using the I/O modes.
 *
 * Revision 1.34  1993/09/23  03:00:19  brad
 * Add #if PFS to all usage of fdte->fmode, since we don't go to the server
 * file table on PFS files.
 *
 * Revision 1.33  1993/09/08  20:59:51  dbm
 * Added PFS_TOKENMGT macro to dissable M_RECORD mode for UFS files.
 *
 * Revision 1.32  1993/09/01  01:33:38  bolsen
 * 08-31-93 Locus code drop for multiple netservers.
 *
 * Revision 1.31  1993/07/21  21:50:40  dbm
 * Fixed bug with not returning proper error code on close() call for
 * PFS I/O modes.
 *
 * Revision 1.30  1993/07/16  03:05:54  dbm
 * Added token optimization functionality.
 *
 * Revision 1.29  1993/07/14  17:31:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.28  1993/07/09  21:02:14  brad
 * Added PFS support to e_rename().
 *
 * Revision 1.1.1.6  1993/07/01  18:23:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.27  1993/06/16  20:32:43  dbm
 * Changed all references to pfs_iomode to pfs_iomode_info to allow single
 * node applications to obtain the PFS I/O mode info.
 *
 * Revision 1.26  1993/05/27  21:31:21  hobbes
 * added support for libhippi (FIO_DEVPORT).
 *
 * Revision 1.25  1993/05/25  18:36:55  dbm
 * Added interrupt support for asyncronous calls and also fixes for readv/writev
 * with PFS I/O modes.
 *
 * Revision 1.24  1993/05/18  20:09:55  cfj
 * Put #ifdef NFS where appropriate.
 *
 * Revision 1.23  1993/05/17  18:31:57  wunder
 * Added ifdef PFS around async I/O code.
 *
 * Revision 1.22  1993/05/12  00:06:53  brad
 * Fixed ad1.0.3 merge problem in #if MAPPED_FILES | PFS.
 *
 * Revision 1.21  1993/05/10  23:46:40  brad
 * Fixed ad1.0.3 merge problems.
 *
 * Revision 1.20  1993/05/06  20:15:06  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:18:10  cfj
 * Initial 1.0.3 code drop
 *
 * 	Revision 2.55  93/05/13  16:45:02  roy
 * 		Remove token_get_size and simplify length handling in read/write code.
 * 		[93/05/05            roy]
 * 
 * Revision 2.46  93/08/31  09:24:30  mjl
 * [LCC #0384] Direct copyout to *alen is not in fact safe; use user_bcopy().
 * 
 * Revision 2.45  93/08/27  11:23:11  bhk
 * Initialized the error variable in dopoll to prevent selects and
 * polls with zero timeouts from incorrectly missing reponses due to
 * garbage in the error variable.
 * 
 * Revision 2.44  93/08/22  21:12:19  bhk
 * Added a check to the timeval paramter to select to make sure that
 * only positive or zero values are passed. #367
 * 
 * Revision 2.43  93/06/16  15:24:43  klh
 * 	Revision 2.56  93/05/16  20:57:33  loverso
 * 		Make select/poll notice errors/interrupts sooner.
 * 
 * Revision 2.42  93/06/09  17:12:32  yazz
 * [ Bug #0107 ] Augment error message to include port name.
 * 
 * Revision 2.41  93/06/02  09:45:50  yazz
 * For Sys V IPC under TNC move svipc port on migrate.
 * 
 * Revision 2.40  93/04/29  13:56:09  klh
 * 	Revision 2.54  93/04/13  18:03:37  roy
 * 		Lock fdte when setting accessed or modified flags in e_read,
 * 		e_readv, e_write, e_writev in order to synchronize with callback
 * 		thread accessing the same word [minturn@ssd.intel.com].
 * 		[93/04/12            roy]
 *
 * 	Revision 2.53  93/04/08  11:16:50  loverso
 * 		Restore check in e_write for bad user buffer. (rabii)
 *
 * 		e_write() should return EINVAL if the data count is negative.
 * 		[1992/05/21  16:46:47  barbou]
 *
 * 	Revision 2.52  93/04/06  11:56:50  rabii
 * 		Added pointer and parameter validation to some routine.
 *
 * 	Revision 2.51  93/03/30  16:07:39  roy
 * 		Modified calling sequence to e_read2, e_write2, and e_lseek2.
 * 		Removed FAST_PATH_IO conditional (fast path code always enabled).
 * 		[93/02/16            roy]
 *
 * 	Revision 2.50  93/03/25  17:27:17  roy
 * 		In fdt_unref_entry, release token before calling vm_deallocate
 * 		to avoid paging deadlocks during object termination.  [sjs]
 *
 * 	Revision 2.49  93/03/10  10:45:31  mmp
 * 		fdt_unref_entry now gets an error back from fsvr_file_unref and
 * 		returns it to its caller.  e_close is the only interested caller,
 * 		and it now returns the error.  cast all other callers of
 * 		fdt_unref_entry to (void).
 *
 * 	Revision 2.48  93/02/02  15:21:55  rabii
 * 		Added credentials port to fsvr_cleanlocks and fsvr_file_unref (rabii)
 *
 * 		Added afs support (rabii)
 *
 * 	Revision 2.47  93/01/27  11:41:26  durriya
 * 		fsvr_open_with_token now passes revoke port as a polymorphic and
 * 		specifies MAKE_SEND for it                                (durriya)
 *
 * 	Revision 2.46  93/01/13  12:39:39  roy
 * 		Remove a couple printf's.
 * 		[93/01/13            roy]
 *
 * 	Revision 2.45  93/01/12  17:05:17  roy
 * 		Piggy-back token acquisition on open msg by using fsvr_open_with_token.
 * 		All window mapping is done within the emulator.
 * 		Removed mappable and fpio_mode fdte fields in favor of new iomode field.
 * 		[93/01/11            roy]
 *
 * Revision 2.39  93/03/10  14:16:18  yazz
 * Synchronous close merge from Intel.
 * 
 * 	Revision 1.7.6.2  1993/02/24  23:21:13  cfj
 * 	Fixes for bugs #4122 and #3314.
 *
 * 	Revision 1.7.6.1  1993/02/16  20:39:23  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.44  93/01/11  14:34:43  mmp
 * 	 Syncrhonous close support: get an extra file reference at fork time
 * 	 (fsvr_file_ref); release the file reference when file is closed
 * 	 (fsvr_file_unref).  Fix file lock behavior when file is closed: set
 * 	 cleanlocks bit if fcntl(..., F_SETLK, ...) is ever used; if set, call
 * 	 fsvr_cleanlocks when file descriptor is closed.  (mmp)
 * 
 * 	Revision 1.7  1993/01/06  22:13:03  shala
 * 	Fixed previous bug correctly this time,  (missing { ).
 *
 * 	Revision 1.6  1993/01/06  19:56:44  shala
 * 	Fixed problem with zero length writes, (fdte was not being dereferenced).
 *
 * 	Revision 1.5  1992/12/11  19:12:57  cfj
 * 	Fix up merge botch.
 *
 * 	Revision 1.4  1992/12/11  02:51:53  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.3  1992/11/30  22:08:49  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.5  1992/11/25  02:57:24  dbm
 * 	Added changes to support mapped files with PFS I/O modes.
 *
 * 	Revision 1.1.2.4  1992/11/17  22:44:02  joel
 * 	Fix PTS 3588, check timeout value for -1 and return EINVAL if true.
 *
 * 	Revision 1.1.2.3  1992/11/13  18:21:46  cfj
 * 	Fixup number of params to calls to isc_deregister.
 *
 * 	Revision 1.1.2.2  1992/11/06  18:20:55  dleslie
 * 	Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * 	into the NX tree
 *
 * 	Revision 1.1.2.1  1992/11/05  22:15:54  dleslie
 * 	cal modifications for NX through noon, November 5, 1992ZZ
 *
 * 	Revision 2.35  1992/10/22  14:58:50  dbm
 * 	Added PFS functionality.
 *
 * Revision 1.19  1993/04/30  23:44:43  brad
 * Fixed bug with uninitialized error variable in sync_readv.
 *
 * Revision 1.18  1993/04/27  16:55:30  brad
 * Removed PFS debug.
 *
 * Revision 1.17  1993/04/15  23:01:52  dbm
 * Fixed a problem to avoid releasing a token before all asynchronous requests
 * have finished.
 *
 * Revision 1.16  1993/04/15  18:35:17  dbm
 * Added locking code to protect fdte token state.
 *
 * Revision 1.15  1993/04/09  17:17:17  brad
 * Tweaked some comments and #ifdef's.
 *
 * Revision 1.14  1993/04/06  17:55:36  wunder
 * added ifdef PFSs around async changes to e_(read/write)/sync_(read/write)
 * split.  Also removed duplicate calls to fdt_ref_entry and modified calls
 * to dequeue_sync.
 *
 * Revision 1.13  1993/04/06  17:33:36  dbm
 * Fixed a couple references of token_release to be file_token_release.
 *
 * Revision 1.12  1993/04/05  17:57:58  brad
 * Fixed an fdt_unref_entry() merge problem.
 *
 * Revision 1.7.6.5  1993/04/05  16:49:09  cfj
 * In fdt_unref_entry(), release the token before deallocating the port to the
 * memory object.
 *
 * Revision 1.11  1993/04/04  00:55:53  cfj
 * Merge with T9.
 *
 * Revision 1.7.6.4  1993/04/04  00:37:45  cfj
 * Return ENAMETOOLONG if pfs_scandir() returns -1 instead of of EFAULT.
 *
 * Revision 1.10  1993/04/03  03:17:58  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.9  1993/03/27  18:15:07  cfj
 * Merge with T9
 *
 * Revision 1.7.6.3  1993/03/27  18:10:46  cfj
 * Added address validations to fix bug #4664 & #4586.
 *
 * Revision 1.8  1993/02/16  21:08:22  cfj
 * Merge sync close into main stem.
 *
 * Revision 1.7.6.1  1993/02/16  20:39:23  cfj
 * Synchronous close from OSF.
 *
 * Revision 2.44  93/01/11  14:34:43  mmp
 * 	Syncrhonous close support: get an extra file reference at fork time
 * 	(fsvr_file_ref); release the file reference when file is closed
 * 	(fsvr_file_unref).  Fix file lock behavior when file is closed: set
 * 	cleanlocks bit if fcntl(..., F_SETLK, ...) is ever used; if set, call
 * 	fsvr_cleanlocks when file descriptor is closed.  (mmp)
 * 
 * Revision 1.1.2.5  1992/11/25  02:57:24  dbm
 * Added changes to support mapped files with PFS I/O modes.
 *
 * Revision 1.1.2.4  1992/11/17  22:44:02  joel
 * Fix PTS 3588, check timeout value for -1 and return EINVAL if true.
 *
 * Revision 1.1.2.3.2.25  1993/03/25  01:34:13  dbm
 * Fixed readv/writev to work correctly with PFS files and PFS I/O modes,
 * especially M_GLOBAL.
 *
 * Revision 1.1.2.3.2.24  1993/03/23  01:59:44  dbm
 * Fixed up token_release_all to work with PFS files.  The releasing of a
 * token for mapped files is always after the operation has completed.  This
 * is not so with PFS files, which may be interrupted in the middle of an
 * operation.  This required the token release code to check the type of
 * file before checking the values of the token flags for assert conditions.
 *
 * Revision 1.1.2.3.2.23  1993/03/20  23:30:31  brad
 * Added support for using readv()/writev() on PFS files.
 *
 * Revision 1.1.2.3.2.22  1993/03/19  01:34:27  dbm
 * Added a parameter to close_on_exit() to allow PFS I/O modes to exit from
 * signal without having to sync with other nodes.
 *
 * Revision 1.1.2.3.2.21  1993/03/17  00:33:20  dbm
 * Added support for migrate() system call.  This involved making sure that
 * the file port rights were shared for each of the pfs file systems.
 *
 * Revision 1.1.2.3.2.20  1993/03/16  18:22:37  wunder
 * Added indirection for e_readv to call sync_readv and e_writev to call
 * sync_writev.
 *
 * Revision 1.1.2.3.2.19  1993/03/16  01:50:07  wunder
 * Added user asynchronous I/O support.
 *
 * Revision 1.1.2.3.2.18  1993/03/11  23:37:01  dbm
 * Removed PFS I/O mode support for non mapped and non PFS files.  Also
 * did some type casting to remove warning messages on some MIG calls.
 *
 * Revision 1.1.2.3.2.17  1993/03/10  06:24:06  brad
 * Now set I/O mode (VIO_NONE) in open RPC.  Added support for chmod,
 * chown, fchmod, fchown, fsync.
 *
 * Revision 1.1.2.3.2.16  1993/02/23  04:52:28  brad
 * Added validation of stripe attributes e_mount_pfs().  Added PFS support
 * to e_access() and e_truncate()/e_ftruncate().
 *
 * Revision 1.1.2.3.2.15  1993/02/16  20:00:32  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.1.2.3.2.14  1993/02/12  17:07:13  dbm
 * added M_GLOBAL i/o mode functionality.  Fixed lseek to work properly with
 * PFS files.  Updated readv/writev calls.
 *
 * Revision 1.7  1993/01/06  22:13:03  shala
 * Fixed previous bug correctly this time,  (missing { ).
 *
 * Revision 1.1.2.3.2.13  1993/02/09  22:35:23  brad
 * Added PFS support for fstat.  Added extended_flag to pfs_multi_{f}stat
 * calls.
 *
 * Revision 1.1.2.3.2.12  1993/01/11  17:23:12  dbm
 * Added changes to support PFS files with I/O modes.
 *
 * Revision 1.1.2.3.2.11  1993/01/08  02:08:18  brad
 * Added support for unlinking PFS files.
 *
 * Revision 1.6  1993/01/06  19:56:44  shala
 * Fixed problem with zero length writes, (fdte was not being dereferenced).
 *
 * Revision 1.1.2.3.2.10  1993/01/05  22:39:49  brad
 * Added #ifdef PFS where missed in a few places.
 *
 * Revision 1.1.2.3.2.9  1992/12/22  02:23:15  dbm
 * Changed parameter ordering on file_token_release() function.
 *
 * Revision 1.1.2.3.2.8  1992/12/21  21:49:36  brad
 * Added pfs_multi_lseek() logic to e_lseek().
 *
 * Revision 1.1.2.3.2.7  1992/12/16  23:12:00  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.3.2.6  1992/12/16  05:57:15  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.1.2.3.2.5  1992/12/14  22:54:49  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.5  1992/12/11  19:12:57  cfj
 * Fix up merge botch.
 *
 * Revision 1.4  1992/12/11  02:51:53  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.1.2.3.2.4  1992/12/12  01:48:07  brad
 * Update of latest PFS functionality in preparation for a merge with the
 * NX branch.
 *
 * Revision 1.1.2.3.2.3  1992/12/11  21:03:50  dbm
 * Added ifdef's to remove mapped file dependencies on file tokens.
 *
 * Revision 1.1.2.3.2.2  1992/12/03  00:17:53  dbm
 * Updated for pfs i/o mode information in the fdte entry.
 *
 * Revision 1.1.2.3.2.1  1992/11/25  23:01:00  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.3  1992/11/30  22:08:49  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.5  1992/11/25  02:57:24  dbm
 * Added changes to support mapped files with PFS I/O modes.
 *
 * Revision 1.1.2.4  1992/11/17  22:44:02  joel
 * Fix PTS 3588, check timeout value for -1 and return EINVAL if true.
 *
 * Revision 1.1.2.3  1992/11/13  18:21:46  cfj
 * Fixup number of params to calls to isc_deregister.
 *
 * Revision 1.1.2.2  1992/11/06  18:20:55  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/05  22:15:54  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *
 * Revision 2.35  1992/10/22  14:58:50  dbm
 * Added PFS functionality.
 *
 * Revision 2.37  92/11/23  16:04:58  klh
 * 	Revision 2.42  92/11/17  19:46:21  loverso
 * 		If nothing gets copied in e_write() and the length had been updated,
 * 		reset to original length.  (mmp)
 * 
 * 	Revision 2.41  92/11/11  10:42:10  rabii
 * 		Added fix to non MAPPED_FILES build.
 * 
 * 	Revision 2.39  92/11/05  17:25:30  roy
 * 		Use fdte_alloc/fdte_free instead of malloc/free to save VA space.
 * 		Implement EFBIG for mapped files.
 * 		Pass additional args to fsvr_ufs_mount and fsvr_mount_nfs. (durriya)
 * 		[92/10/23            roy]
 * 
 * 	Revision 2.38  92/11/03  11:16:31  loverso
 * 		Removed reply_port from emul_vm_map.
 * 		Added missing arg to emul_vm_map from e_writev (!!).
 * 		Fix e_ocreat: fmode is FWRITE.
 * 
 * 	Revision 2.37  92/11/02  16:34:38  mmp
 * 		In e_fcntl(), catch changes to FAPPEND flag.  (mmp)
 * 
 * 	Revision 2.36  92/11/02  16:29:21  mmp
 * 		In e_write() error path, make sure file length doesn't shrink. (mmp)
 * 
 * Revision 2.36  92/11/23  10:31:05  klh
 * Implement notify_asap bit for TNC Unix datagram sockets, which should be
 * notified on on either first read *or* first write on new execution node.
 * Also removed some stale code. (klh for mjl)
 * 
 * Revision 2.35  92/10/28  14:46:20  roman
 * Fix some types for a cleaner compilation.
 * 
 * Revision 2.34  92/10/08  11:14:24  roman
 * Change moveproc_insert_rights() to lock and unlock the fdt table
 * 	during its operation rather than depending upon the fact
 * 	that the table was locked at the time of the call. This
 * 	meant the routine name had to change from
 * 	fdt_moveproc_insert_rights() to moveproc_insert_rights().
 *
 * Revision 2.33  92/10/06  12:04:56  roman
 * Fix RCS comments.
 * 
 * Revision 2.32  92/10/05  13:50:03  klh
 * 	Revision 2.35  92/09/29  16:51:08  rabii
 * 		fdt_unref_entry deallocates the file port last to mitigate a 
 * 		race in the server.
 * 		[92/09/29            roy]
 * 
 * 	Revision 2.34  92/09/24  16:48:29  rabii
 * 		Must set win_size to zero in close_on_exec().
 * 		[92/09/24            roy]
 * 
 * 	Revision 2.33  92/09/20  11:24:05  roy
 * 		When growing mapped files, update length before modifying
 * 		the memory object.
 * 		[92/09/17            roy]
 * 
 * 	Revision 2.32  92/09/11  13:11:27  rabii
 * 		Changed the error in dup_internal from EINVAL to EBADF (mmp)
 * 
 * 	Revision 2.31  92/09/11  09:25:21  rabii
 * 		token_release_to_server now takes a 'revoked' arg.
 * 		[92/09/08            roy]
 * 
 * 		More fixes for EFAULT handling in read/write routines.  Use new
 * 		user_rcheck2/user_rwcheck2 routines.
 * 		[92/09/03            roy]
 * 
 * 	Revision 2.30  92/08/26  12:09:35  loverso
 * 		Add token_release_all() to release all tokens in emulator 
 *		prior to task suspension or exit.  Break out common code into 
 *		new routine token_release_to_server().  Fix some lint.  Mapped 
 *		files ops (fsvr_token_acquire, etc) are no longer 
 *		isc_register'd.  Remove transid argument to isc_deregister().
 * 		(loverso)
 * 
 * Revision 2.31  92/09/28  16:24:09  roman
 * Fix RCS comment.
 * 
 * Revision 2.30  92/08/17  12:46:59  mjl
 * FIFOs request migration notification from the emulator by passing back
 * a special iomode value.
 * 
 * Revision 2.29  92/08/08  01:26:27  jdh
 * turn on migrate bit in fde entry for fd's returned by an accept() syscall 
 * 
 * Revision 2.28  92/08/06  13:38:34  klh
 * 	Revision 2.28  92/07/29  08:36:35  rabii
 * 		Fixed RCS log
 * 
 * 	Revision 2.27  92/07/29  08:32:17  rabii
 * 		Fix EFAULT and count handling in read/write for mapped_files by
 * 		converting to use the new user_bcopy2 routine.
 * 		Modified sendsig to take argument interrupt so it can set it
 * 		to TRUE and thus take the generated signal (rabii).
 * 		[92/07/23            roy]
 * 
 * 		Reinitialize more fdte state in fdt_init_child.
 * 		[92/07/21            roy]
 * 
 * 	Revision 2.26  92/07/20  10:22:42  rabii
 * 		Corrected fix from locus (rabii)
 * 
 * 	Revision 2.25  92/07/15  17:01:49  rabii
 * 		Added fix to e_write from locus (rabii)
 * 
 * Revision 2.27  92/07/30  15:59:43  chrisp
 * Implement locking the FDT over fork/rfork/migrate/rexec. This fixes the
 * 	fdte spinlock problem (roy).
 * 
 * Revision 2.26  92/07/14  17:24:45  roman
 * Fix bug in e_write() where exit processing is skipped if 
 * 	user_rcheck() fails.
 * 
 * Revision 2.25  92/07/10  08:42:46  chrisp
 * Restore fix for moveproc_insert_rights() not resetting referenced bits -
 * 	this change was lost by the merge.
 * 
 * Revision 2.24  92/07/08  09:00:06  roman
 * Set up fdte fields correctly so that mapped files continue to work
 * 	after TNC migrate and remote exec.
 * 
 * Revision 2.23  92/07/07  15:04:17  roman
 * Change moveproc_insert_rights() to not extract mem_obj port unless this port
 * 	is non-NULL.
 * Remove handling of revoke_port from mach_port_insert_rights(). This is now
 * 	done in callback_init().
 * 
 * Revision 2.22  92/07/07  13:04:31  klh
 * 	Revision 2.24  92/07/02  09:55:13  loverso
 * 		Deallocate mapped file window in fdt_unref_entry.
 * 		[92/07/01            roy]
 * 
 * Revision 2.21  92/06/15  14:59:18  klh
 * 	Revision 2.23  92/06/11  13:26:09  pjg
 * 		Ifdef SOCKADDR_UN_MINLEN for the i860 because it triggers a bug
 * 		in the i860 compiler.
 * 
 * 	Revision 2.22  92/06/10  16:43:52  pjg
 * 		Change several calls to user_rcheck to check for
 * 		MAX(namelen, SOCKADDR_UN_MINLEN) instead of just namelen 
 *		(loverso).
 * 
 * Revision 2.20  92/06/10  14:10:42  chrisp
 * [Bug #22] For TNC, reset "referenced" bits correctly in
 * 	moveproc_insert_rights() after extracting file port rights from old
 * 	task.
 * 
 * Revision 2.19  92/06/10  10:06:17  klh
 * 	Revision 2.21  92/06/08  18:16:21  pjg
 * 		Add fast_path support under FAST_PATH_IO conditional (read 
 *		only). Routines that open and create files return an extra 
 *		argument with the iomode of the file, which defaults to 0 in 
 *		the normal case (non-mapped and non-fast_path) (pjg).
 * 
 * 		Add 'accessed' flag to fdte for MAPPED_FILES. (roy)
 * 
 * 		Add e_nfssvc (durriya)
 * 
 * 		Fix typo that caused poll to always return EFAULT. (loverso)
 * 		Fix typo that caused mknod and rmknod to pass wrong length.
 *		(loverso)
 * 
 * Revision 2.18  92/06/05  17:56:51  klh
 * 	Revision 2.20  92/05/31  18:55:28  loverso
 * 		Fixed error returns that were not calling fdt_unref_entry().
 * 		Fixed e_mount_ufs to play games with a NULL fspec, since fsck
 * 		passes one when updating a "hot root".
 * 		(loverso)
 * 
 * 		Revision 2.18.1.2  92/05/26  15:47:47  loverso
 * 		Ignore send once notifications on the dopoll "delayed reply 
 *		port". 
 * 		Panic on other strange messages.
 * 
 * 		Revision 2.18.1.1  92/05/21  15:11:12  loverso
 * 		Explicitly give the server a send right on the file port for
 * 		bsd_sel_poll_delay().  This is held until the server sends the
 * 		delayed reply.  Doing it in the message is ugly, but saves a 
 *		kernel call in the server!  (loverso)
 * 
 * 		(From gr3.5)
 * 		Revision 3.23  92/02/28  00:10:59  condict
 * 		Insert many calls to user access-checking functions, for
 * 		EFAULT recovery (avoids core-dump in emulator).
 * 
 * 	Revision 2.19  92/05/24  14:07:06  pjg
 * 		Changed send_sigpipe to a more general send_sig to also handle
 * 		sendig SIGXFSZ (rabii)
 * 
 * 	Revision 2.18  92/05/18  12:25:26  roy
 * 		Revision 2.13.1.4  92/05/08  12:02:03  roy
 * 		Use fsvr_token_not_found when necessary.
 * 		Move token send rights to the server in fsvr_token_release.
 * 		Added stub for swapon (rabii).
 * 		[92/04/22            roy]
 * 
 * 		Revision 2.13.1.3  92/04/22  09:50:19  roy
 * 		Changes in support of token caching for MAPPED_FILES.
 * 		[92/04/05            roy]
 * 
 * 		Revision 2.13.1.2  92/04/01  12:12:33  roy
 * 		Don't inherit open file mapped windows across fork
 * 		(until VM_INHERIT_SHARE is implemented across nodes).
 * 		[92/03/30            roy]
 * 
 * 		Revision 2.13.1.1  92/03/30  16:51:27  roy
 * 		Changed fdte lock macros to take fdte pointers instead of
 * 		lock pointers.
 * 		[92/03/30            roy]
 * 
 * 		dup_internal returns EINVAL instead of EBADF if the fdes is 
 *		invalid.
 * 		[92/03/26            roy]
 * 
 * Revision 2.17  92/04/14  13:33:11  roman
 * Change emul_tnc_mynode() from a variable to a function.
 *
 * Revision 2.16  92/04/07  13:39:49  pjg
 * 	Have e_select() and e_poll() hold the fdte ref during call to dopoll().
 * 	Moved dopoll() here from bsd_user_side, and made it interruptible.
 * 	(loverso)
 * 
 * Revision 2.15  92/04/05  16:41:37  pjg
 * 	dup_internal returns EINVAL instead of EBADF if the fdes is invalid.
 * 
 * 	Eliminate unnecessary copies in stat and fstat. Use struct stat in call
 * 	to fsvr_stat and fsvr_fstat. Add e_devstat (durriya)
 * 
 * 	Initial TNC migration notification code.  Let pipes, socketpairs, etc. 
 * 	know when a reading process has migrated. (chrisp)
 * 	[92/03/31            roy]
 * 
 * Revision 2.14  92/03/20  11:54:44  pjg
 * 	Don't pass credentials port in the bsd_mmap call (durriya)
 * 
 * Revision 2.13  92/03/15  14:29:01  roy
 * 	92/03/03  11:46:52  roy
 * 	Extensive mapped files changes.  Add compatibility hack for dup.
 * 
 * 	92/02/19  10:32:25  roy
 * 	Remote messages on behalf of mapped files code are now interruptible.
 * 
 * Revision 2.12  92/03/03  13:57:41  pjg
 * 	Revision 2.11.1.1  92/03/02  15:49:52  jeffreyh
 *	Changes from roy to make dup work more correctly.
 * 
 * Revision 2.11  92/03/01  18:49:07  pjg
 * 	Changed e_chdir, e_fchdir and e_chroot to send the cwd and root
 * 	ports to the PM (pjg).
 * 	Add e_mmap (durriya).
 * 	Got rid of the "interrupt" argument to isc_deregister (loverso).
 * 
 * Revision 2.10  92/02/11  18:55:12  pjg
 * 	Moved all the functions that needed fdt.h from bsd_user_side.c
 * 	to this file.
 * 
 * 	Put routine fork_insert_rights() into this file (more relevant 
 * 	here than in bsd_user_side.c). Add new routine 
 * 	moveproc_insert_rights() that is used to manipulate file 
 * 	descriptors during migrate and rexec for TNC (roman@locus.com)
 * 
 * Revision 2.9  92/01/17  21:09:36  roy
 * 	Make socket calls interruptible (loverso).
 * 
 * Revision 2.8  92/01/17  17:17:26  roy
 * 	Interruptible system call support (loverso).
 * 
 * Revision 2.7  92/01/16  17:42:30  roy
 * 	92/01/16  13:21:49  pjg
 * 	Added support for NFS client in mount().
 * 
 * Revision 2.6  92/01/14  10:42:02  roy
 * 	92/01/06  20:43:56  noemi
 * 	Changed e_statfs and e_getfh to use stubs.
 * 
 * 	92/01/05  21:03:59  noemi
 * 	Added e_sync and e_getfsstat.
 * 
 * Revision 2.5  92/01/09  22:57:52  roy
 * 	Remove some unix domain socket printf's.
 * 
 * Revision 2.4  92/01/09  15:28:10  roy
 * 	Unix domain socket support (loverso).
 * 
 * Revision 2.3  92/01/03  09:31:23  roy
 * 	Remove ADFS_HACK.
 * 
 * Revision 2.2  92/01/02  18:42:26  roy
 * 	91/12/23  16:20:19  roy
 * 	Converted fchdir to use fdte's.
 * 
 * 	91/10/15  23:56:42  noemi
 * 	Initial revision
 * 
 * $EndLog$
 */

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <uxkern/fsvr.h>
/* for ps_reply in dopoll() */
#include <uxkern/bsd_msg.h>

/* WARNING: the lengthy path name below is necessary to
 * prevent varargs.h from being found in the mk/release
 * subdirectory.  For some reason, even though CPATH has
 * "../server/include" before "mk/release/.../include",
 * the file is found in the latter directory, if it is
 * not qualified with a path prefix.
 */
#include <../server/include/varargs.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/fcntl.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/poll.h>
#include <sys/file.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <machine/vmparam.h>
#include "emul.h"
#include "fdt.h"
#ifdef	PFS
#include <sys/param.h>
#include <pfs/pfs.h>
#include "pfs_iomode.h"
#endif


#if	MACH_AFS
#define	SYS_ioctl	SYS_afs_xioctl
#endif	MACH_AFS

/*
 * WARNING: This long path is to get the correct stddef.h.   With just
 * <stddef.h>, gcc is picking up "/usr/local/gnu/lib/gcc-include/stddef.h".
 */
#include <uxkern/../../usr/include/stddef.h>	/* for offsetof */


fdt_slot_t	fdt[NOFILE];			/* file descriptor table */
int 		fdt_lastfile;			/* HWM of fdt */
spin_lock_t 	fdt_lock;			/* lock protecting fd table */	
fdt_entry_t	fdte_table[NOFILE];		/* storage for fdte's */
fdt_entry_t 	*fdte_free_head;		/* head of fdte free list */

int		edebug = 0;
int		nullcompat = 0;

void		token_release_to_server();

#ifdef	PFS
extern esize_t	ex_zero;
extern esize_t	ex_neg_one;

extern int	pfs_malloc();
extern void	pfs_free();
extern void	free();
#endif

/*
 * Initialize the file descriptor table.
 */
void
fdt_init()
{
	register int fdes;

	for (fdes=0; fdes < NOFILE; fdes++) {
		fdt[fdes].fdte = FD_EMPTY;
	}

	fdt_lastfile = 0;
	FDT_LOCK_INIT();

	fdte_free_head = NULL;
	for (fdes=0; fdes < NOFILE; fdes++) {
		fdte_table[fdes].next = fdte_free_head;
		fdte_free_head = &fdte_table[fdes];
	}
	EPRINT(("File descriptor table initialized."));
}

/*
 * Initialize file descriptor table state on the child branch of a fork.
 */
void
fdt_init_child()
{
	int 		fdes;
	fdt_entry_t 	*fdte;
#ifdef	PFS
	pfs_fd_t	*pfs_fd;
#endif

	FDT_LOCK_INIT();
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY)
			continue;
		if (fdte == FD_RESERVED) {
			fdt[fdes].fdte = FD_EMPTY;
			continue;
		}
		fdte_lock_init(fdte);
		fdte->referenced = 0;
#ifdef	TNC
		fdte->notify_on_migrate = 0;
		fdte->was_notified = 0;
#endif
#if	MAPPED_FILES | PFS
		/* 
		 * Child does not inherit tokens from its parent.  Thus, 
		 * clear fdte->flags for all open files to indicate that
		 * tokens are not held.  Also, child does not inherit 
		 * mapped windows from the parent so set win_size to zero 
		 * (VM_INHERIT_SHARE not impl. across nodes at press time).
		 */
		fdte_io_lock_init(fdte);
		fdte->flags = 0;
		fdte->win_size = 0;
		fdte->min_offset = INT_MAX;
		fdte->max_offset = 0;
		fdte->accessed = 0;
		fdte->modified = 0;
		fdte->must_release = 0;
		fdte->can_revoke = 1;
		fdte->token_refcnt = 0;
#endif
		/* child does not inherit file locks from parent */
		fdte->cleanlocks = 0;
		fdte_fpio_lock_init(fdte);
		fdte->fpio_offset = 0;
#ifdef	PFS
		pfs_fd = fdte->pfs_fd;
		if (pfs_fd != NULL) {
			fdte_pfsio_lock_init(pfs_fd);
			pfs_fd->p_offset.shigh = 0;
			pfs_fd->p_offset.slow = 0;
		}

		/*
		 * Set asynchronous struct pointer to NULL indicating that
		 * asynchronous scheduling has not been started.
		 */
		fdte->async_queue = NULL;
#endif
	}
}

/*
 * Allocate an fdte.  Must be called with fdt lock held.
 */
void
fdte_alloc(fdtep)
	fdt_entry_t	**fdtep;
{
	if (fdte_free_head == NULL)					
	        emul_panic("fdte_alloc");				      
	*fdtep = fdte_free_head;					
	fdte_free_head = ((fdt_entry_t *) fdte_free_head)->next;	
}
	
/*
 * Free an fdte.  Must NOT be called with fdt lock held.
 */
void
fdte_free(fdte)							
	fdt_entry_t	*fdte;
{
	FDT_LOCK();						
	fdte->next = fdte_free_head;				
	fdte_free_head = fdte;				

#ifdef	PFS
	/* free async scheduling structure if one exists */
	if (fdte->async_queue != NULL)
		free_async_queue(fdte);
#endif

	FDT_UNLOCK();							
}

/*
 * Initialize an fdte.
 */
void
fdte_init(fdte)
	fdt_entry_t *fdte;
{
	fdte->fp = MACH_PORT_NULL;
	fdte_lock_init(fdte);
	fdte->refcnt = 0;
	fdte->referenced = 0;
#ifdef	PFS
	fdte->iomode = VIO_NONE;	/* use default for this file system */
#else
	fdte->iomode = VIO_BUF;		/* default */
#endif
	fdte->cleanlocks = 0;
#ifdef	TNC
	fdte->notify_on_migrate = 0;
	fdte->notify_asap = 0;
	fdte->was_notified = 0;
#ifdef  CHKPNT
	/*
	 * Set fdte->offset to 0 for chkpnt/restart. Unlike the usual
	 * case, where the fdte->offset is set by write/seek/read to the
	 * file server, chkpnt/restart might check the file status
	 * through the emulator after the first open and before any
	 * write/seek/read.
	 */
	fdte->offset = 0;
#endif  CHKPNT
#endif
#if	MAPPED_FILES | PFS
	fdte_io_lock_init(fdte);
	fdte->mem_obj = MACH_PORT_NULL;
	fdte->win_size = 0;
	fdte->min_offset = INT_MAX;
	fdte->max_offset = 0;
	fdte->accessed = 0;
	fdte->modified = 0;
	fdte->flags = 0;
	fdte->must_release = 0;
	fdte->can_revoke = 1;
	fdte->token_refcnt = 0;
#endif
	fdte_fpio_lock_init(fdte);
	fdte->fpio_offset = 0;
#ifdef	PFS
	fdte->pfs_fd = NULL;
	fdte->pfs_iomode_info = NULL;
	fdte->pfs_iomode = M_UNIX;
	/*
	 * Set asynchronous struct pointer to NULL indicating that
	 * asynchronous scheduling has not been started.
	 */
	fdte->async_queue = NULL;
#endif
}

/*
 * Exported routines for atomically manipulating the fdt.
 * Called, for example, at fork time.
 */
void
fdt_atomic_begin()
{
	FDT_LOCK();
}

void
fdt_atomic_end()
{
	FDT_UNLOCK();
}

/*
 * Reserve a slot in the file descriptor table, and allocate
 * a file descriptor table entry.
 */
int
fdt_reserve(fdes, fdp, fdtep)
	register int fdes;
	int *fdp;
	fdt_entry_t **fdtep;
{
	FDT_LOCK();
	for (; fdes < NOFILE; fdes++) {
		if (fdt[fdes].fdte == FD_EMPTY) {
			fdt[fdes].fdte = FD_RESERVED;
			fdte_alloc(fdtep);	 /* must hold fdt lock */
			FDT_UNLOCK();
			*fdp = fdes;
			fdte_init(*fdtep);
			return(ESUCCESS);
		}
	}
	FDT_UNLOCK();
	return(EMFILE);
}

/*
 * Reserve a slot in the file descriptor table, but do NOT allocate
 * a file descriptor table entry.  Called by dup2().
 */
int
fdt_reserve_slot(fdes, fdp)
	register int fdes;
	int *fdp;
{
	FDT_LOCK();
	for (; fdes < NOFILE; fdes++) {
		if (fdt[fdes].fdte == FD_EMPTY) {
			fdt[fdes].fdte = FD_RESERVED;
			FDT_UNLOCK();
			*fdp = fdes;
			return(ESUCCESS);
		}
	}
	FDT_UNLOCK();
	return(EMFILE);
}

/*
 * Cancel a reservation in the file descriptor table.
 */
void
fdt_cancel(fdes, fdte)			
	int 		fdes;
	fdt_entry_t 	*fdte;
{

	fdt[fdes].fdte = FD_EMPTY;		
#ifdef	PFS
	if (fdte->pfs_fd) {	/* this should never happen */
		EPRINT(("fdt_cancel: bad pfs_fd entry in fdt, fdte = 0x%x, pfs_fd = 0x%x",
			fdte, fdte->pfs_fd));
		(void)pfs_fd_dealloc(fdte->pfs_fd);
		fdte->pfs_fd = NULL;
	}
	if (fdte->pfs_iomode_info) {
		free((void *)fdte->pfs_iomode_info);
		fdte->pfs_iomode_info = NULL;
		fdte->pfs_iomode = M_UNIX;
	}
#endif
	fdte_free(fdte); 
}

/*
 * Cancel a slot in the file descriptor table.
 * Exists for symmetry - currently not called.
 */
void
fdt_cancel_slot(fdes)			
	int 		fdes;
{					
	fdt[fdes].fdte = FD_EMPTY;		
}

/*
 * Install a file descriptor table entry into a reserved slot.
 * Must be called with knowledge that the fdte will not disappear
 * out from under us, either because noone else has access (i.e.,
 * was just allocated by fdt_reserve), or the caller holds a reference
 * (e.g., dup2 logic).  Also, the fd slot must be reserved.
 */
void
fdt_install(fdes, fdte)
	int 		fdes;
	fdt_entry_t 	*fdte;
{			
	fdte_lock(fdte);
	fdte->refcnt++;
	if (fdte->fp == MACH_PORT_NULL) 
		emul_panic("fdt_intall: null mach port");
#ifndef	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) 
		emul_panic("emulator mismatch w/ server");
#endif
	fdte_unlock(fdte);
	fdt[fdes].cloexec = FALSE;
	FDT_LOCK();
	fdt[fdes].fdte = fdte;  
	if (fdes > fdt_lastfile)
		fdt_lastfile = fdes;
	FDT_UNLOCK();
}

/*
 * Reference a fdte.
 */
int
fdt_ref_entry(fdes, fdtep)
	int fdes;
	fdt_entry_t **fdtep;
{
	fdt_entry_t	*fdte;

	if ((unsigned)fdes >= NOFILE)
		return (EBADF);

	FDT_LOCK();
	if (fdt[fdes].fdte == FD_EMPTY || fdt[fdes].fdte == FD_RESERVED) {
		FDT_UNLOCK();
		return(EBADF);			/* slot empty or reserved */
	}

	fdte = fdt[fdes].fdte;
	fdte_lock(fdte);
	FDT_UNLOCK();

	fdte->refcnt++;
	fdte_unlock(fdte);

	*fdtep = fdte;			
	return(ESUCCESS);
}

/*
 * Release a reference to a fdte.
 */
int
fdt_unref_entry(fdte)
	fdt_entry_t		*fdte;
{
	int			error = 0;
#ifdef	PFS
	int			pfs_error = 0;
#endif

	
	fdte_lock(fdte);
	if (--fdte->refcnt == 0) {
#if MAPPED_FILES | PFS
		/*
		 * The entry is no longer referenced.  
		 */
		if (fdte->flags) {
			/*
			 * Release the token.  This must be done:
			 * - asynchronously to avoid a deadlock with the
			 *   inode lock in the server
			 *
			 * - before the vm_deallocate below because that
			 *   call can block waiting for pageouts, which in
			 *   turn can repeatedly make token_get_size callbacks
			 *   (effectively blocking) as long as a token
			 *   on the file is outstanding.
			 */
			EASSERT(!fdte->must_release);
			token_release_to_server(fdte, FALSE);
		}

		if (fdte->mem_obj != MACH_PORT_NULL)
			(void) mach_port_deallocate(mach_task_self(), 	
						    fdte->mem_obj);
		if (fdte->win_size)
			if (error = vm_deallocate(mach_task_self(), 
						  fdte->win_addr,
						  fdte->win_size)) {
	EPRINT(("unref.vm_dealloc failure: addr=0x%x, size=%d, error=0x%x",
					fdte->win_addr, fdte->win_size, error));
				emul_panic("fdte_unref_entry");
			}
#ifdef	PFS
		if (fdte->pfs_fd) {
			pfs_error = pfs_multi_close(fdte);
		}
#endif
#endif  MAPPED_FILES | PFS
		/*
		 * Send a message to the server to release a reference on
		 * the file structure, and move the send right to the server.
		 */
		error = fsvr_file_unref(fdte->fp, credentials_port, fdte->fp);

		fdte_free(fdte);	 /* free the fdte data structure */
	} else
		fdte_unlock(fdte);

#ifdef	PFS
	if (pfs_error)
		return(pfs_error);
#endif
	return(error);
}

#if	MAPPED_FILES | PFS
/*
 * Lookup a fdte based on the token port contained within.
 * Return it locked.
 * XXX Consider a hash table.
 */
fdt_entry_t *
fdt_token_lookup(token)
	mach_port_t	token;
{
	int		fdes;
	fdt_entry_t	*fdte;

	FDT_LOCK();
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED)
			continue;
		fdte_lock(fdte);
		if (fdte->flags != 0 && fdte->token == token) {
			FDT_UNLOCK();
			return(fdte);
		}

#ifdef PFS
		if (fdte->token == PFS_TOKEN_MAGIC) {
			pfs_token_mgr_info      *info;

			info = fdte->pfs_iomode_info->token_mgr_info;
			if (token == info->server_token) {
				/*
				 * Server token revoke:
				 */
				if (info->flags) {
					FDT_UNLOCK();
					return(fdte);
				}
			} else if (token == info->client_token) {
				/*
				 * Client token revoke:
				 */
				if (fdte->flags) {
					FDT_UNLOCK();
					return(fdte);
				}
			}
		}
#endif
		fdte_unlock(fdte);
	}

	FDT_UNLOCK();
	return(NULL);
}
#endif

#if	MACH_AFS
#define OLD_MOUNT_AFS	3
#endif	MACH_AFS
int
e_mount(serv_port, interrupt, type, dir, flags, data, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		type;
	char		*dir;
	int		flags;
	caddr_t		data;
	int		*rvalp;
{
#if	MACH_AFS
	int	args[5];
#endif	MACH_AFS

	switch(type) {

	case MOUNT_UFS:
		return(e_mount_ufs(serv_port, interrupt, type, dir, flags,
				   data, rvalp));
		break;

#ifdef  PFS
	case MOUNT_PFS:
		return(e_mount_pfs(serv_port, interrupt, type, dir, flags,
				   data, rvalp));
		break;
#endif	PFS
		
	case MOUNT_NFS:
		return(e_mount_nfs(serv_port, interrupt, type, dir, flags,
				   data, rvalp));
		break;

#if	MACH_AFS
	case OLD_MOUNT_AFS:
	case MOUNT_AFS:
		args[0] = MOUNT_AFS;
		args[1] = dir;
		args[2] = flags;
		args[3] = data;
		args[4] = dir;

		return(emul_generic(serv_port, interrupt, SYS_mount,
			(int *)args, rvalp));
		break;
#endif	MACH_AFS

	default:
		/*
		 * We only support UFS mounts for now.
		 */
		return(EOPNOTSUPP);
	}
}


int
e_mount_ufs(serv_port, interrupt, type, dir, flags, data, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		type;
	char		*dir;
	int		flags;
	caddr_t		data;
	int		*rvalp;
{
	mach_port_t	start_port, dev_port;
	int		error;
	int		len_dir, len_fspec = 0;
	char		*mig_fspec = "";
	struct ufs_args	*uargsp;
	transaction_id_t trans_id;

	if (!user_strlen(dir, &len_dir))
		return EFAULT;
	if (*dir == '\0' && !nullcompat)
		return(ENOENT);

	uargsp = (struct ufs_args *)data;
	if (!user_rcheck(uargsp, sizeof *uargsp))
		return EFAULT;

	/* ufs_fsck does a mount update with a NULL fspec; we let that work! */
	if (uargsp->fspec && !user_strlen(uargsp->fspec, &len_fspec))
		return EFAULT;
	
	start_port = (*dir == '/') ? rootdir_port : currentdir_port;
	dev_port = (!uargsp->fspec || *uargsp->fspec == '/')
			? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_mount_ufs(start_port, credentials_port, trans_id,
		rootdir_port, dev_port, dir, len_dir + 1, dir, len_dir +1,
                flags, uargsp->fspec ? uargsp->fspec : mig_fspec,
		uargsp->fspec ? len_fspec + 1 : 0,
		uargsp->exflags, uargsp->exroot);
	isc_deregister(interrupt);

	return(error);
}

#ifdef	PFS
int
e_mount_pfs(serv_port, interrupt, type, dir, flags, data, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		type;
	char		*dir;
	int		flags;
	caddr_t		data;
	int		*rvalp;
{
	mach_port_t		start_port, dev_port;
	int			error;
	int			len_dir, len_uargs, len_fspec = 0;
	char			*mig_fspec = "";
	struct pfs_args 	*uargsp;
	struct ufs_args		*ufs_argsp;
	struct statpfs		*pfs_argsp;
	transaction_id_t 	trans_id;

	if (!user_strlen(dir, &len_dir))
		return(EFAULT);
	if (*dir == '\0' && !nullcompat)
		return(ENOENT);
	
	uargsp = (struct pfs_args *)data;
	ufs_argsp = (struct ufs_args *)&uargsp->fs_args;
	pfs_argsp = (struct statpfs *)&uargsp->stripe_attr;
	len_uargs = sizeof(struct ufs_args) + pfs_argsp->p_reclen;
	if (!user_rcheck(uargsp, len_uargs))
		return(EFAULT);
	if (ufs_argsp->fspec && !user_strlen(ufs_argsp->fspec, &len_fspec))
		return(EFAULT);
	if (error = pfs_check_stripe_attributes(interrupt, pfs_argsp,
						dir, len_dir))
		return(error);

	start_port = (*dir == '/') ? rootdir_port : currentdir_port;
	dev_port = (*ufs_argsp->fspec == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_mount_pfs(start_port, credentials_port, trans_id,
			       rootdir_port, dev_port,
			       dir, len_dir + 1, dir, len_dir + 1, flags,
			       ufs_argsp->fspec ? ufs_argsp->fspec : mig_fspec,
			       ufs_argsp->fspec ? len_fspec + 1 : 0,
			       ufs_argsp->exflags, ufs_argsp->exroot,
			       (char_array)uargsp, len_uargs);
	isc_deregister(interrupt);

	return(error);
}
#endif	PFS

int
e_mount_nfs(serv_port, interrupt, type, dir, flags, data, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		type;
	char		*dir;
	int		flags;
	caddr_t		data;
	int		*rvalp;
{
#ifdef NFS
	mach_port_t	start_port;
	int		error, len_dir, len_hostname;
	struct nfs_args	*uargsp;
	transaction_id_t trans_id;

	if (!user_strlen(dir, &len_dir))
		return EFAULT;
	if (*dir == '\0' && !nullcompat)
		return(ENOENT);
	
	uargsp = (struct nfs_args *)data;
	if (!user_rcheck(uargsp, sizeof *uargsp))
		return EFAULT;
	if (!user_strlen(uargsp->hostname, &len_hostname))
		return EFAULT;
	if (!user_rcheck(uargsp->addr, sizeof *uargsp->addr))
		return EFAULT;
	if (!user_rcheck(uargsp->fh, sizeof *uargsp->fh))
		return EFAULT;
	start_port = (*dir == '/') ? rootdir_port : currentdir_port;
	
	isc_register(start_port, &trans_id);
	error = fsvr_mount_nfs(start_port, credentials_port, trans_id,
			       dir, len_dir + 1, dir, len_dir + 1, flags,
			       *uargsp, *uargsp->addr, *uargsp->fh,
			       uargsp->hostname, len_hostname + 1);
	isc_deregister(interrupt);
	return(error);
#else
	return(EINVAL);
#endif /* NFS */
}

int
e_unmount(serv_port, interrupt, dir, flags, rvalp)
	mach_port_t     serv_port;
	boolean_t       *interrupt;
	char            *dir;
	int             flags;
	int             *rvalp;
{
	mach_port_t     start_port;
	int             error, len_dir;
	transaction_id_t trans_id;

	if (!user_strlen(dir, &len_dir))
		return EFAULT;
        if (*dir == '\0' && !nullcompat)
                return(ENOENT);
	start_port = (*dir == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_unmount(start_port, credentials_port, trans_id,
			     rootdir_port, dir, len_dir + 1, flags);
	isc_deregister(interrupt);

	return(error);
}


int
e_sync(serv_port, interrupt, dummy, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		dummy; 		/* Space holder for tag */
	int		*rvalp;
{
	int             error;
	transaction_id_t trans_id;

	isc_register(rootdir_port, &trans_id);
	error = fsvr_sync(rootdir_port, credentials_port, trans_id, -1);
	isc_deregister(interrupt);

	return(error);
}


int
e_getfsstat(serv_port, interrupt, statfsbuf, bufsize, flags, dummy, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	struct statfs	*statfsbuf;
	long		bufsize;
	int		flags;
	int		dummy; 		/* Space holder for tag */
	int		*rvalp;
{
	int             error;
	char		*data_addr;
	transaction_id_t trans_id;

	/*
	 * If statfsbuf is NULL, simply count the number of entries.
	 */
	if (statfsbuf == (struct statfs *)0) { 
		isc_register(rootdir_port, &trans_id);
		error = fsvr_getfsstat_count(rootdir_port, credentials_port,
					     trans_id, -1, rvalp);
		isc_deregister(interrupt);
		return(error);
	}

	/*
	 *  Collect statfs information and copy it to the user's buffer.
	 */
	isc_register(rootdir_port, &trans_id);
	error = fsvr_getfsstat(rootdir_port, credentials_port, trans_id, 
			       bufsize, flags, -1, &data_addr, 
			       (mach_msg_type_number_t *)rvalp);
	isc_deregister(interrupt);
	if (error == 0) {
		/*
		 * Copy data to user-supplied statfsbuf and 
		 * deallocate out-of-line data.
		 */
		if (!user_bcopy(data_addr, (char *)statfsbuf, *rvalp))
			error = EFAULT;
		(void) vm_deallocate(mach_task_self(), (vm_address_t)data_addr,
				     (vm_size_t) *rvalp);
		/*
		 * Return the number of statfs entries.
		 */
		*rvalp /= sizeof(struct statfs);
	}
	return(error);
}


int
e_stat(serv_port, interrupt, fname, ub, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	struct stat	*ub;
	int		*rval;
{

#ifdef	COMPAT_43
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)ub, TRUE, FALSE));
#else
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)ub, TRUE));
#endif				
}

int
e_lstat(serv_port, interrupt, fname, ub, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	struct stat	*ub;
	int		*rval;
{

#ifdef	COMPAT_43
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)ub, FALSE, FALSE));
#else
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)ub, FALSE));
#endif				

}

#ifdef	COMPAT_43
struct  ostat
{
	short   st_dev;                /* ID of device containing a directory*/
				       /*   entry for this file.  File serial*/
				       /*   no + device ID uniquely identify */
				       /*   the file within the system */
	ino_t   st_ino;                /* File serial number */
	u_short st_mode;               /* File mode; see #define's below */
	short   st_nlink;              /* Number of links */
	u_short st_uid;                /* User ID of the file's owner */
	u_short st_gid;                /* Group ID of the file's group */
	short   st_rdev;               /* ID of device */
				       /*   This entry is defined only for */
				       /*   character or block special files */
	off_t   st_size;               /* File size in bytes */
	time_t  st_atime;              /* Time of last access */
	int     st_spare1;
	time_t  st_mtime;              /* Time of last data modification */
	int     st_spare2;
	time_t  st_ctime;              /* Time of last file status change */
	int     st_spare3;
				       /* Time measured in seconds since */
				       /*   00:00:00 GMT, Jan. 1, 1970 */
	ulong_t st_blksize;            /* Size of block in file */
	long    st_blocks;             /* blocks allocated for file */
	u_long  st_flags;              /* user defined flags for file */
	u_long  st_gen;                /* file generation number */

};

int
e_ostat(serv_port, interrupt, fname, oub, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	struct ostat	*oub;
	int		*rval;
{
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)oub, TRUE, TRUE));
}

int
e_olstat(serv_port, interrupt, fname, oub, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	struct ostat	*oub;
	int		*rval;
{
	return (e_stat_call(serv_port, interrupt, fname, (caddr_t)oub, FALSE, TRUE));
}
#endif	/* COMPAT_43 */

#ifdef	COMPAT_43
int
e_stat_call(serv_port, interrupt, fname, on_ub, follow, compat)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char 		*fname;
	caddr_t		*on_ub;		/* for lint */
	boolean_t	follow;
	boolean_t	compat;
#else
e_stat_call(serv_port, interrupt, fname, on_ub, follow)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char 		*fname;
	caddr_t		*on_ub;		/* for lint */
	boolean_t	follow;
#endif	/* COMPAT_43 */
{
	int		error, len_fname;
	mach_port_t	start_port;
	transaction_id_t trans_id;
#ifdef COMPAT_43
        struct stat     statbuf;
#endif
#ifdef	PFS
	char		pfs_path[MAXPATHLEN];
	ulong_t		iomode;
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (!user_rwcheck(on_ub, sizeof (struct stat)))
		return EFAULT;

	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);

#ifdef COMPAT_43
        if (compat) {
                error = fsvr_stat(start_port,
                                  credentials_port,
                                  trans_id,
                                  rootdir_port,
                                  fname,
                                  len_fname + 1,
                                  follow,
#ifdef	PFS
                                  &statbuf,
				  &iomode);
#else
                                  &statbuf);
#endif
		
        } else 
#endif
        {
        error = fsvr_stat(start_port,
                          credentials_port,
                          trans_id,
                          rootdir_port,
                          fname,
                          len_fname + 1,
                          follow,
#ifdef	PFS
                          (struct stat *)on_ub,
			  &iomode);
#else
                          (struct stat *)on_ub);
#endif
        }

	isc_deregister(interrupt);

	if (error)
		return (error);

#ifdef	PFS
	if (iomode == VIO_PFS) {
		error = pfs_multi_stat(interrupt, fname, len_fname, 
				       (caddr_t)on_ub, follow, FALSE);
		if (error)
			return(error);
	}
#endif

	/*
	 * Copy out stat fields
	 */
#ifdef	COMPAT_43
	if (compat) {
		struct ostat	*ob = (struct ostat *) on_ub;

		ob->st_dev	= statbuf.st_dev;
		ob->st_ino	= statbuf.st_ino;
		ob->st_mode	= statbuf.st_mode;
		ob->st_nlink	= statbuf.st_nlink;
		ob->st_uid	= statbuf.st_uid;
		ob->st_gid	= statbuf.st_gid;
		ob->st_rdev	= statbuf.st_rdev;
		ob->st_size	= statbuf.st_size;
		ob->st_atime	= statbuf.st_atime;
		ob->st_mtime	= statbuf.st_mtime;
		ob->st_ctime	= statbuf.st_ctime;
		ob->st_blksize	= statbuf.st_blksize;
		ob->st_blocks	= statbuf.st_blocks;
	}
#endif	/* COMPAT_43 */
	return (0);
}


int
e_readlink(serv_port, interrupt, fname, buf, count, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	char		*buf;
	int		count;
	int		*rval;
{
	int		buflen, len_fname;
	register int	error;
	mach_port_t	start_port;
	transaction_id_t trans_id;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (!user_rwcheck(buf, count))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	buflen = count;

	isc_register(start_port, &trans_id);
	error = fsvr_readlink(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
			count,		/* max length */
			buf,
			(mach_msg_type_number_t *)&buflen);
	isc_deregister(interrupt);

	if (error == 0) {
		rval[0] = buflen;
		if (buflen < count)
			buf[buflen] = '\0';
	}
	return (error);
}

int
e_acct(serv_port, interrupt, fname, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	int		*rval;
{

	mach_port_t	start_port;
	int		error, len_fname;
	transaction_id_t trans_id;
	
	if (fname) {
		if (!user_strlen(fname, &len_fname))
			return EFAULT;
		start_port = (*fname == '/') ? rootdir_port : currentdir_port;
	} else
		start_port = rootdir_port;

	isc_register(start_port, &trans_id);
	if (fname) {
		error = fsvr_acct(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
			TRUE);
	} else {
		error = fsvr_acct(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			"", 1,
			FALSE);
	}
	isc_deregister(interrupt);

	return(error);
}


int
e_open(proc_port, interrupt, fname, mode, crtmode, rval)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	char			*fname;
	int			mode;
	int			crtmode;
	int			*rval;
{
	fdt_entry_t		*fdte;
	int			fd;
	int			len_fname;
	int			error;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;
#ifdef	PFS 
	char			pfs_path[MAXPATHLEN];
#endif

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	if (error = fdt_reserve(0, &fd, &fdte))
		return(error);

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
#if	MAPPED_FILES && OPEN_WITH_TOKEN
	/*
	 * Use the composite operation for opening the file and acquiring 
	 * a token.  If the file is not mappable then the token and
	 * mem_obj out args will be MACH_PORT_NULL.
	 *
	 * Note that we needn't worry about revokers because the fdte
	 * is currently inaccessible to all others.  The token will become
	 * revokable once fdt_install is executed below.
	 */
	error = fsvr_open_with_token(start_port, credentials_port, trans_id,
			  rootdir_port, fname, len_fname + 1, mode,
			  crtmode, TOK_OFFSET|TOK_DATA_WRITE, revoke_port,
                          MACH_MSG_TYPE_MAKE_SEND,
			  &fdte->fp, &fdte->iomode, &fdte->token, 
			  &fdte->offset, &fdte->length, &fdte->mem_obj);

	if (error == 0 && fdte->token != MACH_PORT_NULL) {
		/* indicate token is held */
		fdte->flags = TOK_OFFSET|TOK_DATA_WRITE;	
	}

#else
	error = fsvr_open(start_port, credentials_port, trans_id,
			  rootdir_port, fname, len_fname + 1, mode,
			  crtmode, &fdte->fp, &fdte->iomode);
#endif
	isc_deregister(interrupt);

	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}
	
	if (edebug & EDEBUG_MF)
		EPRINT(("e_open: %s mappable: %s", 
			fname, (fdte->iomode == VIO_MAPPED) ? "YES" : "NO"));
#if 	MAPPED_FILES | PFS
	fdte->fmode = mode - FOPEN;	/* access/status flags: only */
					/*   used if mappable or PFS */
#endif
#ifdef	TNC
	if (fdte->iomode == VIO_REQNOTIFY) {
		fdte->notify_on_migrate = 1;
		fdte->iomode = VIO_BUF;
		if (edebug & EDEBUG_TNC_UIPC)
			EPRINT(("e_open: %s requests notification", fname));
	}
#endif
	fdte->fpio_offset = 0;
#ifdef	PFS
	if (fdte->iomode == VIO_PFS) {
		error = pfs_multi_open(interrupt, fname, len_fname, fdte,
				       mode, crtmode);
		if (error) {
			fdt_cancel(fd, fdte);
			return(error);
		}
	}
#endif
	fdt_install(fd, fdte);	
	*rval = fd;
	return (error);
}

#ifdef	COMPAT_43
int
e_oopen(proc_port, interrupt, fname, mode, crtmode, rval)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	char			*fname;
	int			mode;
	int			crtmode;
	int			*rval;
{
	int			fd;
	int			len_fname;
	fdt_entry_t		*fdte;
	int			error;
	mach_port_t		start_port;
	transaction_id_t 	trans_id;
#ifdef	PFS 
	char			pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	if (error = fdt_reserve(0, &fd, &fdte)) 
		return(error);

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_oopen(start_port, credentials_port, trans_id,
			   rootdir_port, fname, len_fname + 1, mode,
			   crtmode, &fdte->fp, &fdte->iomode);
	isc_deregister(interrupt);

	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}

	if (edebug & EDEBUG_MF)
		EPRINT(("e_oopen: %s mappable: %s", 
			fname, (fdte->iomode == VIO_MAPPED) ? "YES" : "NO"));
#if	MAPPED_FILES | PFS
	fdte->fmode = mode - FOPEN;	/* access/status flags: only */
					/*   used if mappable or PFS */
#endif
#ifdef	TNC
	if (fdte->iomode == VIO_REQNOTIFY) {
		fdte->notify_on_migrate = 1;
		fdte->iomode = VIO_BUF;
		if (edebug & EDEBUG_TNC_UIPC)
			EPRINT(("e_oopen: %s requests notification", fname));
	}
#endif
	fdte->fpio_offset = 0;
#ifdef	PFS
	if (fdte->iomode == VIO_PFS) {
		error = pfs_multi_open(interrupt, fname, len_fname, fdte,
				       mode, crtmode);
		if (error) {
			fdt_cancel(fd, fdte);
			return(error);
		}
	}
#endif
	fdt_install(fd, fdte);	

	*rval = fd;
	return (error);
}

int
e_ocreat(proc_port, interrupt, fname, crtmode, rval)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	char			*fname;
	int			crtmode;
	int			*rval;
{
	int			fd;
	int			len_fname;
	fdt_entry_t		*fdte;
	int			error;
	mach_port_t		start_port;
	transaction_id_t	trans_id;
#ifdef	PFS 
	char			pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	if (error = fdt_reserve(0, &fd, &fdte)) 
		return(error);

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_ocreat(start_port, credentials_port, trans_id,
 			    rootdir_port, fname, len_fname + 1, crtmode,
 			    &fdte->fp, &fdte->iomode);
	isc_deregister(interrupt);

	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}

	if (edebug & EDEBUG_MF)
		EPRINT(("e_ocreate: %s mappable: %s", 
			fname, (fdte->iomode == VIO_MAPPED) ? "YES" : "NO"));
#if	MAPPED_FILES | PFS
	fdte->fmode = FWRITE;		/* access/status flags: only */
	                                /*   used if mappable        */
#endif
	fdte->fpio_offset = 0;
#ifdef	PFS
	if (fdte->iomode == VIO_PFS) {
		error = pfs_multi_open(interrupt, fname, len_fname, fdte,
				       O_WRONLY|O_CREAT|O_TRUNC, crtmode);
		if (error) {
			fdt_cancel(fd, fdte);
			return(error);
		}
	}
#endif
	fdt_install(fd, fdte);	
	*rval = fd;
	return (error);
}
#endif	/* COMPAT_43 */

int
e_chdir(proc_port, interrupt, fname)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
{
	int		error = 0;
	int		len_fname;
	mach_port_t	vnodeport;
	mach_port_t	start_port;
	transaction_id_t trans_id;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	
	
	start_port = (*fname == '/') ? rootdir_port : currentdir_port;
	
	isc_register(start_port, &trans_id);
	error = fsvr_chdir(start_port, rootdir_port, credentials_port, trans_id,
			fname, len_fname+1, &vnodeport);
	isc_deregister(interrupt);
	if (error) 
		return (error);

	/*
	 * Inform the PM of our new cwd
	 */
	emul_blocking();
	error = bsd_setports(proc_port, vnodeport, rootdir_port, interrupt);
	emul_unblocking();
	if (error) {
		mach_port_deallocate(mach_task_self(), vnodeport);
		return(error);
	}

	/*
	 * Set cwd port to the new port.
	 */
	mach_port_deallocate(mach_task_self(), currentdir_port);
	currentdir_port = vnodeport;	
	return(error);
}

int
e_chroot(proc_port, interrupt, fname)
	mach_port_t     proc_port;
	boolean_t       *interrupt;
	char            *fname;
{
	int		error = 0;
	int		len_fname;
	mach_port_t	vnodeport;
	mach_port_t	start_port;
	transaction_id_t trans_id;
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;
	
	isc_register(start_port, &trans_id);
	error =	fsvr_chroot(start_port, rootdir_port,
			credentials_port, trans_id,
			fname, len_fname +1, &vnodeport);
	isc_deregister(interrupt);
	if (error) 
		return error;

	/*
	 * Inform the PM of our new root port
	 */
	emul_blocking();
	error = bsd_setports(proc_port, currentdir_port, vnodeport, interrupt);
	emul_unblocking();
	if (error) {
		mach_port_deallocate(mach_task_self(), vnodeport);
		return(error);
	}

	/*
	 * Set root port to the new port
	 */
	mach_port_deallocate(mach_task_self(), rootdir_port);
	rootdir_port = vnodeport;	
	return(error);
}

int
e_fchdir(proc_port, interrupt, fdes)
	mach_port_t     proc_port;
	boolean_t       *interrupt;
	int		fdes;
{
	int		error = 0;
	mach_port_t	vnodeport;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fchdir(fdte->fp, credentials_port, trans_id, &vnodeport);
	isc_deregister(interrupt);
	if (error)
		goto out;

	/*
	 * Inform the PM of our new cwd port
	 */
	emul_blocking();
	error = bsd_setports(proc_port, vnodeport, rootdir_port, interrupt);
	emul_unblocking();
	if (error) {
		mach_port_deallocate(mach_task_self(), vnodeport);
		goto out;
	}

	/*
	 * Set cwd port to the new port
	 */
	mach_port_deallocate(mach_task_self(), currentdir_port);
	currentdir_port = vnodeport;	
out:
	(void) fdt_unref_entry(fdte);
	return(error);
}


int
e_rmknod(proc_port, interrupt, fname, fmode, dev, node)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		fmode;
	int		dev;
	node_t		node;
{
	mach_port_t	start_port;
	int	error;
	transaction_id_t trans_id;
	int		len_fname;
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_rmknod(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
			fmode,
			dev,
			node);
	isc_deregister(interrupt);

	return(error);
}


int
e_mknod(proc_port, interrupt, fname, fmode, dev)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		fmode;
	int		dev;
{
	mach_port_t	start_port;
	int	error;
	transaction_id_t trans_id;
	int		len_fname;
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_mknod(start_port,
			   credentials_port,
			   trans_id,
			   rootdir_port,
			   fname, len_fname + 1,
			   fmode,
			   dev);
	isc_deregister(interrupt);

	return(error);
}


int
e_link(proc_port, interrupt, target, linkname)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*target;
	char		*linkname;
{

	mach_port_t	target_start_port;
	mach_port_t	linkname_start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_target, len_linkname;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(target, &len_target))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(target, pfs_path, &len_target)) {
		return ENAMETOOLONG;
	}
	target = &pfs_path[0];
#endif
	if (!user_strlen(linkname, &len_linkname))
		return EFAULT;
	if (*target == '\0' && !nullcompat)
		return(ENOENT);	
	if (*linkname == '\0' && !nullcompat)
		return(ENOENT);	

	target_start_port = (*target == '/') ? rootdir_port : currentdir_port;
	linkname_start_port = (*linkname == '/') ? rootdir_port
						 : currentdir_port;

	isc_register(target_start_port, &trans_id);
	error = fsvr_link(target_start_port,
			  credentials_port,
			  trans_id,
			  rootdir_port,
			  target, len_target + 1,
			  linkname_start_port,
			  linkname, len_linkname + 1);
	isc_deregister(interrupt);

	return(error);
}

int
e_symlink(proc_port, interrupt, target, linkname)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*target;
	char		*linkname;
{
	mach_port_t	linkname_start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_target, len_linkname;
	
	if (!user_strlen(target, &len_target))
		return EFAULT;
	if (!user_strlen(linkname, &len_linkname))
		return EFAULT;
	if (*target == '\0' && !nullcompat)
		return(ENOENT);	
	if (*linkname == '\0' && !nullcompat)
		return(ENOENT);	

	linkname_start_port = (*linkname == '/') ? rootdir_port :
							 currentdir_port;

	isc_register(linkname_start_port, &trans_id);
	error = fsvr_symlink(linkname_start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			target, len_target + 1,
			linkname, len_linkname + 1);
	isc_deregister(interrupt);

	return(error);
}

int
e_unlink(proc_port, interrupt, fname)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	ulong_t		iomode = VIO_NONE;
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_unlink(start_port,
			    credentials_port,
			    trans_id,
			    rootdir_port,
			    fname,
#ifdef	PFS
			    len_fname + 1,
			    &iomode);
#else
			    len_fname + 1);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_unlink(interrupt, fname, len_fname);
#endif

	return(error);
}

int
e_access(proc_port, interrupt, fname, fmode)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		fmode;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	ulong_t		iomode;
	char 		pfs_path[MAXPATHLEN];
#endif

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_access(start_port,
			    credentials_port,
			    trans_id,
			    rootdir_port,
			    fname, len_fname + 1,
#ifdef	PFS
			    fmode,
			    &iomode);
#else
			    fmode);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_access(interrupt, fname, len_fname, fmode);
#endif

	return(error);
}

int
e_chmod(proc_port, interrupt, fname, fmode)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		fmode;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	ulong_t		iomode;
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_chmod(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
#ifdef	PFS
			fmode, &iomode);
#else
			fmode);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_chmod(interrupt, fname, len_fname, fmode);
#endif
	return(error);
}

int
e_chown(proc_port, interrupt, fname, uid, gid)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		uid;
	int		gid;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	ulong_t		iomode;
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_chown(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
			uid,
#ifdef	PFS
			gid, &iomode);
#else
			gid);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_chown(interrupt, fname, len_fname, uid, gid);
#endif
	return(error);
}

int
e_utimes(proc_port, interrupt, fname, times)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	timeval_2_t	times;
{
	int		error;
	int		len_fname;
	timeval_2_t 	mig_times;
	mach_port_t	start_port;
	transaction_id_t trans_id;

#ifdef	PFS 
	ulong_t		iomode;
	char 		pfs_path[MAXPATHLEN];
#endif


	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	/* times is an array */
	if (times && !user_rcheck(&times, sizeof times))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

        /*
	 * Set the access and modification time fields of the file to
	 * the value specified in the  times  argument.  Note that the
	 * iomode  output parameter from the  fsvr_utimes function is
	 * used to determine if  fname  is a PFS file.
	 */
	start_port = (*fname == '/') ? rootdir_port : currentdir_port;
        isc_register(start_port, &trans_id);
        error = fsvr_utimes(start_port,
			    credentials_port,
			    trans_id,
			    rootdir_port,
			    fname, len_fname + 1,
			    times ? times : mig_times,
#ifdef	PFS
			    times == NULL,
			    &iomode);
#else
			    times == NULL);
#endif
        isc_deregister(interrupt);

	if (error) {
	        goto out;
	}

#ifdef	PFS
	if (iomode == VIO_PFS) {
	        /*
		 * Set the access and modification times of all
		 * the stripefiles to the values set in the header
		 * file above:
		 */
		error = pfs_multi_utimes(interrupt,
					fname,
					len_fname,
					times);
	}
#endif

out:

	return(error);
}

int
e_truncate(proc_port, interrupt, fname, length)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		length;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	ulong_t		iomode;
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_truncate(start_port,
			      credentials_port,
			      trans_id,
			      rootdir_port,
			      fname, len_fname + 1,
#ifdef	PFS
			      length,
			      &iomode);
#else
			      length);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_truncate(interrupt, fname, len_fname, 
					   (size_t)length);
#endif
	return(error);
}

int
e_rename(proc_port, interrupt, from_name, to_name)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*from_name;
	char		*to_name;
{
	mach_port_t	from_start_port;
	mach_port_t	to_start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_from_name, len_to_name;
#ifdef	PFS
	ulong_t		iomode = VIO_NONE;
	char		pfs_pathf[MAXPATHLEN];
	char		pfs_patht[MAXPATHLEN];
#endif

	if (!user_strlen(from_name, &len_from_name))
		return EFAULT;
	if (!user_strlen(to_name, &len_to_name))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(from_name, pfs_pathf, &len_from_name)) {
		return ENAMETOOLONG;
	}
	if (pfs_scanpath(to_name, pfs_patht, &len_to_name)) {
		return ENAMETOOLONG;
	}
	from_name       = &pfs_pathf[0];
	to_name         = &pfs_patht[0];
#endif
	if (*from_name == '\0' && !nullcompat)
		return(ENOENT);	
	if (*to_name == '\0' && !nullcompat)
		return(ENOENT);	

	from_start_port = (*from_name == '/') ? rootdir_port : currentdir_port;
	to_start_port = (*to_name == '/') ? rootdir_port : currentdir_port;

	isc_register(from_start_port, &trans_id);
	error = fsvr_rename(from_start_port,
			    credentials_port,
			    trans_id,
			    rootdir_port,
			    from_name, len_from_name + 1,
			    to_start_port,
#ifdef	PFS
			    to_name, len_to_name + 1,
			    &iomode);
#else
			    to_name, len_to_name + 1);
#endif
	isc_deregister(interrupt);

#ifdef	PFS
	if (error)
		return (error);

	if (iomode == VIO_PFS)
		error = pfs_multi_rename(interrupt, from_name, len_from_name,
					 to_name, len_to_name);
#endif
	return(error);
}

int
e_mkdir(proc_port, interrupt, fname, dmode)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	int		dmode;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_mkdir(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1,
			dmode);
	isc_deregister(interrupt);

	return(error);
}

int
e_rmdir(proc_port, interrupt, fname)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
{
	mach_port_t	start_port;
	int		error;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_rmdir(start_port,
			credentials_port,
			trans_id,
			rootdir_port,
			fname, len_fname + 1);
	isc_deregister(interrupt);

	return(error);
}

/*
 * Mark the file closed in the file table.
 * FDT_LOCK must be held.
 */
void
doclose(fdes)
	int		fdes;
{
	fdt[fdes].fdte = FD_EMPTY;
	while (fdt_lastfile >= 0 && fdt[fdt_lastfile].fdte == FD_EMPTY) 
		fdt_lastfile--;
}




/*
 * Close a file.
 * Note that one thread can be doing an operation on a file while
 * another tries to close it.
 */
int
e_close(proc_port, interrupt, fdes, rvalp)
	mach_port_t proc_port;
	int *interrupt;
	int fdes;
	int *rvalp;
{
	fdt_entry_t	*fdte;
	int		error;
#ifdef PFS
	async_req	dummy_req;
	int		pfs_error = ESUCCESS;
	int		queued = 0;
#endif

	if ((unsigned)fdes >= NOFILE) 
		return (EBADF);

	FDT_LOCK();
	if (fdt[fdes].fdte == FD_EMPTY || fdt[fdes].fdte == FD_RESERVED) {
		FDT_UNLOCK();
		return(EBADF);			/* slot empty or reserved */
	}
	
	fdte = fdt[fdes].fdte;

#ifdef	PFS
	/*
	 * If this file has outstanding asynchronous I/O requests, then
	 * block the close request until those I/O operations complete.
	 * This insures that if the process making the close request has
	 * any of the outstanding I/O requests, then we know that the file
	 * system operation has completed.
	 * (This code was added to fix Bug #6486)
	 */
	if (fdte->async_queue != NULL) {
		FDT_UNLOCK();
	        /*
		 * Note:  for the close case, there is no reason to check
		 *        the return value from the queue_sync routine.
		 */
	        pfs_error = queue_sync(fdte->async_queue,
				       &dummy_req,
				       &queued);
		if (queued)
		      dequeue_sync(fdte->async_queue,
				   &dummy_req);
		FDT_LOCK();
	}

	/*
         * If the file was opened with any mode other than the default,
	 * then synchronize its closing:
	 */
	if (fdte->pfs_iomode_info) {
		FDT_UNLOCK();
		pfs_error = pfs_iomode_close(fdte,
					     interrupt);
		FDT_LOCK();
	}
#endif

	doclose(fdes);		/* must be called with FDT_LOCK held */
	FDT_UNLOCK();

	if (fdte->cleanlocks)
		fsvr_cleanlocks(fdte->fp, credentials_port);

	/* release the ref obtained at install time */
	error = fdt_unref_entry(fdte);

#ifdef PFS
	if (!error)
		error = pfs_error;
#endif
	return(error);
}




/*
 * Special version of close used by dup2().  
 * Leaves the slot in the file descriptor table reserved.
 */
int
close_for_dup2(fdes)
	int	      	fdes;
{
	fdt_entry_t	*fdte;
#ifdef PFS
	async_req	dummy_req;
	int		pfs_error;
	int		queued = 0;
#endif


	FDT_LOCK();
	if (fdt[fdes].fdte == FD_EMPTY) {
		fdt[fdes].fdte = FD_RESERVED;
		FDT_UNLOCK();
		return(ESUCCESS);
	} else if (fdt[fdes].fdte == FD_RESERVED) {
		/*
		 * We possibly could wait until it's no longer reserved,
		 * but one could also argue that a multi-threaded appl.
		 * should be synchronizing itself appropriately such that
		 * it's not concurrently calling dup2() and performing other
		 * accesses.
		 */
		FDT_UNLOCK();
		return(EBADF);
	} else {
		/*
		 * Close it and leave the slot reserved.
		 */
		fdte = fdt[fdes].fdte;
#ifdef	PFS
		if (fdte->async_queue != NULL) {
			FDT_UNLOCK();
			pfs_error = queue_sync(fdte->async_queue,
					       &dummy_req,
					       &queued);
			if (queued)
				dequeue_sync(fdte->async_queue,
					     &dummy_req);
			FDT_LOCK();
		}

		if (fdte->pfs_iomode_info) {
			int error;
			boolean_t interrupt;
			if ((error = pfs_iomode_close(fdte, &interrupt)) < 0) {
				FDT_UNLOCK();
				return (error);
			}
		}
#endif
		doclose(fdes);	
		fdt[fdes].fdte = FD_RESERVED;
		FDT_UNLOCK();

		/* release ref obtained by fdt_install */
		(void) fdt_unref_entry(fdte);
		return(ESUCCESS);
	}
}

/*
 * Called to close any open files at exit time.  
 */
void
close_on_exit(sig_exit)
int	sig_exit;
{
	int		fdes;
	fdt_entry_t	*fdte;
#ifdef PFS
	async_req	dummy_req;
	int		pfs_error;
	int		queued = 0;
#endif


	FDT_LOCK();
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		if (fdt[fdes].fdte != FD_EMPTY && 
		    fdt[fdes].fdte != FD_RESERVED) {
			fdte = fdt[fdes].fdte;
#ifdef  PFS
			if ((fdte->async_queue != NULL) &&
			    (sig_exit == 0)) {
				FDT_UNLOCK();
			        pfs_error = queue_sync(fdte->async_queue,
						       &dummy_req,
						       &queued);
				if (queued)
				      dequeue_sync(fdte->async_queue,
						   &dummy_req);
				FDT_LOCK();
			}

			if (fdte->pfs_iomode_info) {
				boolean_t interrupt;
				if (sig_exit) {
					if (PFS_TOKENMGT(fdte)) {
						pfs_rlse_token_mgr(fdte, 1);
					}
				} else {
					FDT_UNLOCK();
					/* 
					 * Need to unlock during this call
					 * because it will block waiting
					 * for message from other nodes.
					 * If the other node needs to 
					 * get the token, then a deadlock
					 * occurs on the FDT_LOCK.
					 */
					pfs_iomode_close(fdte, &interrupt);
					FDT_LOCK();
				}
			}
#endif
			fdt[fdes].fdte = FD_EMPTY;
			FDT_UNLOCK();
			if (fdte->cleanlocks)
				fsvr_cleanlocks(fdte->fp, credentials_port);
			(void) fdt_unref_entry(fdte);   
			FDT_LOCK();
		}
	}
	/* don't need to change fdt_lastfile because the proc is exiting */
	FDT_UNLOCK();
}

/*
 * Called to do close-on-exec processing.  This includes closing all files
 * marked close-on-exec, and losing knowledge of mapped window for mapped
 * files (because those window are not retained by exec).
 */
void
close_on_exec()
{
	int		fdes;
	fdt_entry_t	*fdte;
#ifdef PFS
	async_req	dummy_req;
	int		pfs_error;
	int		queued = 0;
#endif

	/*
	 * There's a possibility that the last close of a fdte (via
	 * fdt_unref_entry() below) will not succeed in deallocating
	 * the port if some other thread is currently accessing the file.
	 * Even if all the other threads have already been blown away, 
	 * isn't it possible that one was blown away while holding a fdte
	 * reference?  (implying accumulation of non-reclaimable ports
	 * across exec...)
	 */
	FDT_LOCK();
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte != FD_EMPTY && fdte != FD_RESERVED) {
#if	MAPPED_FILES
			fdte->win_size = 0;
#endif
			if (fdt[fdes].cloexec) {
#ifdef	PFS
				if (fdte->async_queue != NULL) {
					FDT_UNLOCK();
					pfs_error = queue_sync(fdte->async_queue,
							       &dummy_req,
							       &queued);
					if (queued)
						dequeue_sync(fdte->async_queue,
							     &dummy_req);
					FDT_LOCK();
				}

				if (fdte->pfs_iomode_info) {
					boolean_t interrupt;
					pfs_iomode_close(fdte, &interrupt);
				}
#endif
				fdt[fdes].fdte = FD_EMPTY;
				FDT_UNLOCK();
				if (fdte->cleanlocks)
					fsvr_cleanlocks(fdte->fp, 
							credentials_port);
				(void) fdt_unref_entry(fdte);   
				FDT_LOCK();
			}
		}
	}
	while (fdt_lastfile >= 0 && fdt[fdt_lastfile].fdte == FD_EMPTY)
		fdt_lastfile--;
	FDT_UNLOCK();
}

/*
 * Do all the work of dup(), dup2, and fcntl w/ F_DUPFD.
 */
int
dup_internal(oldfdes, newfdes, dup2, rvalp)
	int 		oldfdes;
	int 		newfdes;
	boolean_t	dup2;
	int 		*rvalp;
{
	fdt_entry_t	*oldfdte;
	int 		error;

	if ((unsigned)newfdes >= NOFILE) 
		return (EBADF);

	/*
	 * Get a ref to the file descriptor table entry to be 
	 * referenced again by a new slot.  This ensures that it won't
	 * disappear out from under us.
	 */
	if (error = fdt_ref_entry(oldfdes, &oldfdte))
		return(error);		/* oldfdes is not open or bogus */

	/*
	 * See if this is a dup2() with file descriptors being equal. If
	 * so, this is a no-op.
	 */
	if (newfdes == oldfdes && dup2) {
		(void) fdt_unref_entry(oldfdte);
		goto out;
	}

	/*
	 * Differentiate between dup() and dup2() behavior.
	 */
	if (dup2) {
		/* 
		 * dup2() logic 
		 *
		 * Close the original file that was in the newfdes slot
		 * (if there indeed was a file there), and leave the slot
		 * reserved.
		 */
		if (error = close_for_dup2(newfdes)) {
			(void) fdt_unref_entry(oldfdte);
			return(error);
		}
	} else {
		/*
		 * dup() logic. We can allocate any file descriptor
		 * >= newfdes.
		 */
		if (error = fdt_reserve_slot(newfdes, &newfdes)) {
			(void) fdt_unref_entry(oldfdte);
			return(error);
		}
	} 

	/*
	 * It all worked. Set up the new slot in the file table.  This
	 * takes a new reference to oldfdte.
	 */
	fdt_install(newfdes, oldfdte);

	/*
	 * Release the reference we initially obtained.
	 */
	(void) fdt_unref_entry(oldfdte);
 out:
	*rvalp = newfdes;
	return(ESUCCESS);
}

int
e_dup(proc_port, interrupt, fdes1, fdes2, rvalp)
	mach_port_t proc_port;
	int *interrupt;
	int fdes1;
	int fdes2;
	int *rvalp;
{
	if (fdes1 &~ 077) { 
		/*
		 * XXX Compatibility: /bin/sh relies on setting bit in
		 * fdes of dup() call to effect dup2 behavior.
		 */
		fdes1 &= 077;
		return(dup_internal(fdes1, fdes2, TRUE, rvalp)); 
	} else
		return(dup_internal(fdes1, 0, FALSE, rvalp));
}

int
e_dup2(proc_port, interrupt, oldfdes, newfdes, rvalp)
	mach_port_t proc_port;
	int *interrupt;
	int oldfdes;
	int newfdes;
	int *rvalp;
{
	return(dup_internal(oldfdes, newfdes, TRUE, rvalp));
}

int
e_fcntl(serv_port, interrupt, fdes, cmd, arg, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	int		cmd;
	int		arg;
	int		*rvalp;		/* OUT */
{
	int		uarg[3];
#ifdef	PFS
	fdt_entry_t	*fdte;
	struct sattr	*sattr = (struct sattr *)arg;
	struct statpfs	*stripe_attr = (struct statpfs *)arg;
	int		error;
#endif

	/*
	 * We can handle some commands here.
	 */
	switch (cmd) {
	case F_DUPFD:
		if ((unsigned)arg >= NOFILE) 
			/* 
			 * fcntl expects EINVAL instead of EBADF in this case
			 */
			return (EINVAL);  
		return(dup_internal(fdes, arg, FALSE, rvalp));

	case F_GETFD:
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		rvalp[0] = fdt[fdes].cloexec ? FD_CLOEXEC : 0;
		FDT_UNLOCK();
		break;

	case F_SETFD:
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		fdt[fdes].cloexec = (arg & FD_CLOEXEC) ? TRUE : FALSE;
		FDT_UNLOCK();
		break;

#ifdef	PFS
	case F_GETSATTR:
		if (error = fdt_ref_entry(fdes, &fdte))
			return(error);
		if (fdte->pfs_fd == NULL) {
			(void) fdt_unref_entry(fdte);
			return(ENOTPFS);
		}

		sattr->s_sunitsize = fdte->pfs_fd->p_stripe_unit_size;
		sattr->s_sfactor = fdte->pfs_fd->p_stripe_factor;
		sattr->s_start_sdir = fdte->pfs_fd->p_start_stripedir;

		(void) fdt_unref_entry(fdte);
		break;
	case F_SETSATTR:
		if (error = fdt_ref_entry(fdes, &fdte))
			return(error);
		if (fdte->pfs_fd == NULL) {
			(void) fdt_unref_entry(fdte);
			return(ENOTPFS);
		}

		if (fdte->fmode & FWRITE) 
			error = pfs_set_sattr(interrupt, fdte, sattr);
		else if (fdte->fmode & FREAD)
			error = pfs_map_sattr(fdte, sattr);
		else
			error = EBADF;

		(void) fdt_unref_entry(fdte);
		if (error)
			return(error);
		break;
	case F_GETFULLSATTR:
		if (error = fdt_ref_entry(fdes, &fdte))
			return(error);
		if (fdte->pfs_fd == NULL) {
			(void) fdt_unref_entry(fdte);
			return(ENOTPFS);
		}

		if (error = pfs_get_stripe_attributes(fdte, stripe_attr)) {
			(void) fdt_unref_entry(fdte);
			return(error);
		}

		(void) fdt_unref_entry(fdte);
		break;
#endif

	case F_SETLK:
	case F_SETLKW:
		/*
		 * set cleanlocks flag so that fsvr_cleanlocks will be
		 * called when file is closed.  This flag will never be
		 * cleared; it's an optimization so that files that have
		 * never had locks don't need to send a message to the
		 * server at close time to release them.
		 */
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		fdt[fdes].fdte->cleanlocks = TRUE;
		FDT_UNLOCK();
		goto callserver;

#if	MAPPED_FILES | PFS
	case F_SETFL:
		/* catch changes to FAPPEND flag and set in fdte->fmode */
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		if (arg & FAPPEND)
			fdt[fdes].fdte->fmode |= FAPPEND;
		else
			fdt[fdes].fdte->fmode &= ~FAPPEND;
		FDT_UNLOCK();
		/* fall through to call server to set flags in file struct */
#endif

	default:
	callserver:
		uarg[0] = fdes;
		uarg[1] = cmd;
		uarg[2] = arg;
		return(emul_fs_generic(serv_port, interrupt, SYS_fcntl,
				uarg, rvalp)); 
	}

	return(ESUCCESS);
}

int
e_revoke(proc_port, interrupt, fname)
	mach_port_t	proc_port;
	int		*interrupt;
	char		*fname;
{
	int		error;
	mach_port_t	start_port;
	transaction_id_t trans_id;
	int		len_fname;

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_revoke(start_port,
			    credentials_port,
			    trans_id,
			    rootdir_port,
			    fname, len_fname +1);
	isc_deregister(interrupt);

	return(error);	
}

e_ofstat(proc_port, interrupt, fdes, ob)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	struct ostat	*ob;
{
	int		error;
	struct stat	statbuf;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (!user_rwcheck(ob, sizeof *ob))
		return EFAULT;
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fstat(fdte->fp,
			   credentials_port,
			   trans_id,
			   &statbuf);
	isc_deregister(interrupt);

	ob->st_dev	= statbuf.st_dev;
	ob->st_ino	= statbuf.st_ino;
	ob->st_mode	= statbuf.st_mode;
	ob->st_nlink	= statbuf.st_nlink;
	ob->st_uid	= statbuf.st_uid;
	ob->st_gid	= statbuf.st_gid;
	ob->st_rdev	= statbuf.st_rdev;
	ob->st_size	= statbuf.st_size;
	ob->st_atime	= statbuf.st_atime;
	ob->st_mtime	= statbuf.st_mtime;
	ob->st_ctime	= statbuf.st_ctime;
	ob->st_blksize	= statbuf.st_blksize;
	ob->st_blocks	= statbuf.st_blocks;

	(void) fdt_unref_entry(fdte);
	return(error);	
}

int
e_fstat(proc_port, interrupt, fdes, sb)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	struct stat	*sb;
{
	int		error;
	transaction_id_t trans_id;
	fdt_entry_t	*fdte;

	if (!user_rwcheck(sb, sizeof *sb))
		return EFAULT;
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fstat(fdte->fp,
			   credentials_port,
			   trans_id,
			   sb);
	isc_deregister(interrupt);

#ifdef	PFS
	if ((!error) && (fdte->pfs_fd))
		error = pfs_multi_fstat(interrupt, fdte, (caddr_t)sb, FALSE);
#endif	/* PFS */

	(void) fdt_unref_entry(fdte);
	return(error);	
}

e_fsync(proc_port, interrupt, fdes)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
{
	int	error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fsync(fdte->fp,
			   credentials_port,
			   trans_id);
	isc_deregister(interrupt);

#ifdef	PFS
	if ((!error) && (fdte->pfs_fd))
		error = pfs_multi_fsync(interrupt, fdte);
#endif
	(void) fdt_unref_entry(fdte);
	return(error);	
}

e_fchown(proc_port, interrupt, fdes, uid, gid)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	int		uid;
	int		gid;
{
	int	error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fchown(fdte->fp,
			    credentials_port,
			    trans_id,
			    uid,
			    gid);
	isc_deregister(interrupt);

#ifdef	PFS
	if ((!error) && (fdte->pfs_fd))
		error = pfs_multi_fchown(interrupt, fdte, uid, gid);
#endif
	(void) fdt_unref_entry(fdte);
	return(error);	
}

e_fchmod(proc_port, interrupt, fdes, mode)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	int		mode;
{
	int	error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_fchmod(fdte->fp,
			    credentials_port,
			    trans_id,
			    mode);
	isc_deregister(interrupt);

#ifdef	PFS
	if ((!error) && (fdte->pfs_fd))
		error = pfs_multi_fchmod(interrupt, fdte, mode);
#endif
	(void) fdt_unref_entry(fdte);
	return(error);	
}

e_ftruncate(proc_port, interrupt, fdes, len)
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	int		len;
{
	int	error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

#ifdef	PFS
	if (fdte->pfs_fd) {
		error = pfs_multi_ftruncate(interrupt, fdte, (size_t)len);
		fdt_unref_entry(fdte);
		return(error);
	}
#endif
	isc_register(fdte->fp, &trans_id);
	error = fsvr_ftruncate(fdte->fp,
			       credentials_port,
			       trans_id,
			       len);
	isc_deregister(interrupt);

	(void) fdt_unref_entry(fdte);
	return(error);	
}

#if	MACH_AFS
e_afs_xflock(proc_port, interrupt, fdes, cmd)
#else	/* MACH_AFS */
e_flock(proc_port, interrupt, fdes, cmd)
#endif	MACH_AFS
	mach_port_t	proc_port;
	int		*interrupt;
	int		fdes;
	int		cmd;
{
	int		error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = fsvr_flock(fdte->fp,
			credentials_port,
			trans_id,
			cmd);
	isc_deregister(interrupt);

	(void) fdt_unref_entry(fdte);
	return(error);	
}

int
e_statfs(serv_port, interrupt, fname, buf, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	struct statfs	*buf;
	int		*rval;
{
	register int	error;
	mach_port_t	start_port;
	transaction_id_t trans_id;
	int		len_fname;
#ifdef	PFS 
	char 		pfs_path[MAXPATHLEN];
#endif
	
	if (!user_strlen(fname, &len_fname))
		return EFAULT;
#ifdef	PFS
	if (pfs_scanpath(fname, pfs_path, &len_fname)) {
		return ENAMETOOLONG;
	}
	fname = &pfs_path[0];
#endif
	if (!user_rwcheck(buf, sizeof *buf))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_statfs(start_port, credentials_port, trans_id,
			    rootdir_port, fname, len_fname+1, buf);
	isc_deregister(interrupt);

	return (error);
}


int 
e_devstat(serv_port, interrupt, fname, devsb)
	mach_port_t	serv_port;
	boolean_t	interrupt;
	char		*fname;
	struct devstat	*devsb;
{
	register int	error;
	mach_port_t	start_port;
	transaction_id_t trans_id;
	int		len_fname;

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (!user_rwcheck(devsb, sizeof *devsb))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_devstat(start_port, 
			credentials_port,
			trans_id, 
			rootdir_port,
			fname, len_fname + 1,
			devsb);
	isc_deregister(interrupt);

	return(error);
}



int
e_getfh(serv_port, interrupt, fname, fhp, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char		*fname;
	fhandle_t	*fhp;
	int		*rval;
{
	register int	error;
	mach_port_t	start_port;
	transaction_id_t trans_id;
	int		len_fname;

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (!user_rwcheck(fhp, sizeof *fhp))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_getfh(start_port, credentials_port, trans_id,
			   rootdir_port, fname, len_fname+1, fhp);
	isc_deregister(interrupt);

	return (error);
}


#if	MAPPED_FILES | PFS
/*
 * Acquire a token.  Called with the fdte_io_lock locked and a reference
 * to the fdte held.
 *
 * 'flags' arg specifies whether access should be obtained to the file 
 * offset (logically part of the file structure) and/or the file itself
 * for read or write access.  The latter guarantees atomicity of Unix file 
 * I/O, and protects the length of the file during an operaton.
 */
int
token_acquire(fdte, interrupt, flags, offset, length, debug_val)
	fdt_entry_t	*fdte;
	boolean_t	*interrupt;
	int		flags;
	off_t		*offset;	/* OUT */
	int		*length;	/* OUT */
	int		debug_val;
{
	int		curflags;
	mach_port_t	newtoken;
	int		error;
	char		*s;

	/*
	 * Note: this routine always acquires an offset token regardless
	 * of whether it's being requested.  Also, it assumes that read
	 * or write access is always being requested.
	 */

	if (edebug & EDEBUG_MF) {
		if (debug_val == 0)
			s = "LSEEK";
		else if (debug_val == 1)
			s = "READ";
		else
			s = "WRITE";
		EPRINT(("token_acquire %s: flags=%d curflags=%d", 
			s, flags, fdte->flags));
	}

	fdte_lock(fdte);
	fdte->can_revoke = 0;		/* keep revokers away */
	fdte->token_refcnt++;		/* Increment the reference count. */
	fdte_unlock(fdte);

	curflags = fdte->flags;	        /* current access */

	flags &= ~TOK_OFFSET;		/* turn off temporarily */

	/*
	 * XXX At the moment, the server doesn't treat read and write
	 * tokens differently.  Thus, for now always get a write token
	 * in order to avoid any subsequent token_change messages.
	 */
	flags = TOK_DATA_WRITE;		/* XXX */
	if (curflags & TOK_DATA_WRITE) {
		*length = fdte->length;
		flags &= ~TOK_DATA_WRITE;
	}
#ifdef 	notdef
	if (flags == TOK_DATA_READ) {
		/*
		 * Must acquire data read access.  Having either data
		 * read or data write access does the job.
		 */
		if (curflags & (TOK_DATA_READ | TOK_DATA_WRITE)) {
			*length = fdte->length;
			flags &= ~TOK_DATA_READ;
		}
	} else if (flags == TOK_DATA_WRITE) {
		/*
		 * Must acquire data write access.
		 */
		if (curflags & TOK_DATA_WRITE) {
			*length = fdte->length;
			flags &= ~TOK_DATA_WRITE;
		}
	} else {
		EPRINT(("token_acquire: bad flags=%d",flags));
		emul_panic("token_acquire: bad flags");
	}
#endif  /* notdef */

	if (flags == 0) {
		*offset = fdte->offset;	
		return (ESUCCESS);
	}
	flags |= TOK_OFFSET;	

	/*
	 * Determine whether to acquire a new (or different) token.
	 */
	if (curflags == 0) {
		/*
		 * Request a new token from the server.
		 *
		 * If we haven't obtained the memory object port get
		 * it now along with the tokens.  But, we can only do 
		 * this if the open file is writeable (memory object 
		 * ports currently always grant read/write access). 
		 *
		 * XXX For now, always get the memory object.  Thus, 
		 * get_window will always do the mapping within the
		 * emulator, but the protections on the window will
		 * be read-only if the fmode doesn't include FWRITE.
		 * 
		 */
		if (fdte->mem_obj == MACH_PORT_NULL 
		    /* && (fdte->fmode & FWRITE)*/ ) { 
			if (edebug & EDEBUG_MF)
			   EPRINT((" Calling token_acquire w/ MO: flags=%d",
					flags)); 	
			error = fsvr_token_acquire_with_mo(fdte->fp, 
							   credentials_port, 
							   flags,
							   revoke_port, 
							   &newtoken, 
						    (int *)&fdte->offset, 
							   &fdte->length, 
							   &fdte->mem_obj);
			if (error || fdte->mem_obj == MACH_PORT_NULL) {
			    EPRINT(("can't acquire tokens and mo: error = 0x%x",
					error));
			    EPRINT(("mem_obj=0x%x, token=0x%x", fdte->mem_obj,
					fdte->token));
			    emul_panic("token_acquire");
			}
		} else {
			if (edebug & EDEBUG_MF)
				EPRINT((" Calling token_acquire: flags=%d", 
					flags));
			error = fsvr_token_acquire(fdte->fp, 
						   credentials_port, 
						   flags,
						   revoke_port, 
						   &newtoken,
					    (int *)&fdte->offset, 
						   &fdte->length);
			if (error) {
				EPRINT(("can't acquire: error = 0x%x", error));
				emul_panic("token_acquire");
			}
		}
	} else {
		/*
		 * XXX Should never be calling fsvr_token_change
		 * as long as the code for always acquiring write
		 * tokens is enabled.
		 */
		emul_panic("Shouldn't be calling fsvr_token_change");
#ifdef 	notdef
		/*
		 * Request a different token from the server.
		 * The send right to the old token is moved to the
		 * server via the second arg to fsvr_token_change.
		 *
		 * This code assumes the only possible change is from
		 * read to write access.
		 */
		if (curflags & TOK_DATA_WRITE) {
			EPRINT(("token_acquire: bad curflags=%d",curflags));
			emul_panic("token_acquire: bad curflags");
		}

		if (edebug & EDEBUG_MF)
			EPRINT((" Calling token_change: flags=%d cur=%d",  
				flags, curflags));

		error = fsvr_token_change(fdte->token, fdte->token,
                                          flags,
                                          &newtoken);
		if (error) /* || fdte->mem_obj == MACH_PORT_NULL) */ {
		    EPRINT(("can't change tokens: error = 0x%x", error));
		    /* EPRINT(("mem_obj=0x%x, token=0x%x", fdte->mem_obj,
				fdte->token)); */
		    emul_panic("token_acquire");
		}

		fdte_lock(fdte);
		if (fdte->must_release) {
			/*
			 * Someone tried to revoke the token that we just 
			 * changed.  Force the server thread to retry.
			 */
			if (edebug & EDEBUG_MF)
			     EPRINT(("token_acquire change: race with revoke"));

			fdte->must_release = 0;
			
			/* async. message */
			error = fsvr_token_not_found(fdte->token, fdte->token);
			if (error) 
				/*
				 * There may have been an error because the 
				 * server blew away it's receive right.  
				 * That's ok.
				 */
			     EPRINT(("token_acquire: release err=0x%x",error));
		}
		fdte_unlock(fdte);
#endif  /* notdef */	       
	}

	fdte_lock(fdte);	/* synchronize with revocation */
	fdte->flags = flags;
	fdte->token = newtoken;
	fdte_unlock(fdte);

	if (edebug & EDEBUG_MF)
		EPRINT(("token_acquire success: offset=%d, length=%d, flags=%x",
			fdte->offset, fdte->length, fdte->flags));

	*offset = fdte->offset;
	*length = fdte->length;
	return (ESUCCESS);
}

/*
 * Release a token to the server.  'revoked' arg specifies whether
 * the token was revoked by the file server.  Must be called with fdte
 * lock held.
 */
void
token_release_to_server(fdte, revoked)
	fdt_entry_t	*fdte;
	boolean_t	revoked;
{
	int		error;

	/* 
	 * The token is held and it can be revoked.
	 */
	if (edebug & EDEBUG_MF) {
		char *str;
		str = revoked ? "revoke" : "close";
		EPRINT(("Calling release (%s): flags=%d offset=%d len=%d",
			str, fdte->flags, fdte->offset, fdte->length));
		EPRINT(("accessed=%d modified=%d min_offset=%d, max_offset=%d",
			fdte->accessed, fdte->modified, fdte->min_offset,
			fdte->max_offset));
	}
#ifdef	PFS
	if (fdte->pfs_fd) {
		/* 
	 	 * Send an async. message to release the token. 
	 	 * We have at least one send right from the acquire, and
		 * perhaps another (due to a revoke), that are "deallocated"
		 * by moving them to the server.  
	 	 */
/*
		EPRINT(("fsvr_pfs_token_release, offset =(%d,%d), len = (%d,%d)\n",
		fdte->pfs_fd->p_offset.shigh, fdte->pfs_fd->p_offset.slow,
		fdte->pfs_fd->p_length.shigh, fdte->pfs_fd->p_length.slow));
*/


		if (!fdte->pfs_fd->p_use_token) {
			/*
			 * Just update the file offset:
			 */
			error = fsvr_pfs_update_off(fdte->fp,
						    fdte->pfs_fd->p_offset);

		} else {

			error = fsvr_pfs_token_release(fdte->token, fdte->token,
					               revoked ? fdte->token : 
							         MACH_PORT_NULL,
						       fdte->pfs_fd->p_offset, 
					   	       fdte->pfs_fd->p_length, 
						       fdte->accessed, 
						       fdte->modified);
		}

		if (error) {
			EPRINT(("fsvr_pfs_token_release error = 0x%x",error));
			emul_panic("token_release_to_server");
		}
		fdte->flags = 0;       /* indicates no token is held */
		fdte->accessed = 0;
		fdte->modified = 0;
		fdte->must_release = 0;
		fdte->token_refcnt=0;
		return;
	} else
#endif
	/* 
	 * Send an async. message to release the token. 
	 * We have at least one send right from the acquire, and perhaps 
	 * another (due to a revoke), that are "deallocated" by moving
	 * them to the server.  
	 */
	error = fsvr_token_release(fdte->token, fdte->token,
				   revoked ? fdte->token : MACH_PORT_NULL,
				   fdte->offset, 
				   fdte->length, 
				   fdte->accessed, fdte->modified,
				   fdte->min_offset, fdte->max_offset);
	if (error) {
		EPRINT(("fsvr_token_release error = 0x%x",error));
		emul_panic("token_release_to_server");
	}

	fdte->flags = 0;       /* indicates no token is held */
	fdte->min_offset = INT_MAX;
	fdte->max_offset = 0;
	fdte->accessed = 0;
	fdte->modified = 0;
	fdte->must_release = 0;
	fdte->token_refcnt=0;
}

/*
 * WARNING:
 *
 * This assumes that it is only when other emulator activity has quiesced.
 * Thus, we should not come across any held locks, and all tokens should
 * be immediately revokeable.  Anything else is panic-worthy.
 */
int
token_release_all()
{
	int		fdes;
	fdt_entry_t	*fdte;

	if (!FDT_TRY_LOCK())
		emul_panic("file descr table lock held");
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED)
			continue;
		if (!fdte_try_lock(fdte))
			emul_panic("file descr table entry lock held");

#ifdef PFS
		if ((fdte->flags != 0) ||
		    (PFS_TOKENMGT(fdte)))
#else
		if (fdte->flags != 0)
#endif
		{

#ifdef DEBUG
			extern int suspend_debug;
			if (suspend_debug)
				EPRINT(("fd %d/%x token %xcan_revoke %d\n",
					fdes, fdte,
					fdte->token, fdte->can_revoke));
#endif
#ifdef  PFS
			if (!fdte->pfs_fd)
			  EASSERT(fdte->can_revoke && !fdte->must_release);

			if (PFS_TOKENMGT(fdte)) {
				esize_t curr_offset, curr_length,
					max_offset, max_length;
			       /*
				* Send the max request around the ring to 
				* get all of the emulators in sync with 
				* each other.  Then release the token back
				* to the server.
				*/
				if (fdte->pfs_fd) {
					curr_offset = fdte->pfs_fd->p_offset;
					curr_length = fdte->pfs_fd->p_length;
				} else if (fdte->iomode == VIO_MAPPED) {
					curr_offset.shigh = 0;
					curr_offset.slow = fdte->offset;
					curr_length.shigh = 0;
					curr_length.slow = fdte->length;
				}

				pfs_iomode_max(	fdte,
						&curr_offset,
						&curr_length,
						&max_offset,
						&max_length);
 
 
				if (!fdte->pfs_iomode_info->my_node_number) {
					pfs_token_mgr_info *info = 
					  fdte->pfs_iomode_info->token_mgr_info;
					/*
					 * Set the value of the token length
					 * to EINVAL so that it is computed 
					 * when we reacquire the token:
					 */
					info->length = max_length;
					info->modified = 1;
					info->accessed = 1;
					if (info->flags) {
						tmgr_release_to_server(fdte, 0);
					}
				}
				/*
				 * Just release the token w/o
				 * sending a message to the
				 * tokenmgr since it knows
				 * we are releasing anyway.
				 */
        			fdte->flags = 0;
        			fdte->accessed = 0;
        			fdte->modified = 0;
        			fdte->must_release = 0;
				fdte->token_refcnt = 0;

			} else {
				token_release_to_server(fdte, FALSE);
			}
#else
	token_release_to_server(fdte, FALSE);
#endif
		}
		fdte_unlock(fdte);
	}
	FDT_UNLOCK();

	return 0;
}


/*
 * Release access that's been granted.  Called with the fdte_io_lock locked
 * and a reference to the fdte held.
 */
int
token_release(fdte, interrupt, flags, offset, length)
	fdt_entry_t	*fdte;
	boolean_t	*interrupt;
	int		flags;
	off_t		offset;	
	int		length;	
{
	if (edebug & EDEBUG_MF)
		EPRINT(("token_release: flags=%d, offset=%d, length=%d",
			flags, offset, length));

	fdte_lock(fdte);
	/*
	 * offset or length values of -1 say not to update the fdte values.
	 */
	if ((flags & TOK_OFFSET) && offset != -1)
		fdte->offset = offset;
	if ((flags & (TOK_DATA_READ | TOK_DATA_WRITE)) && length != -1)
		fdte->length = length;

#if	defined(PFS) || defined(MAPPED_FILES)
	if (--fdte->token_refcnt == 0) {
        	if (fdte->must_release) {
			token_release_to_server(fdte, TRUE);
		} 
		fdte->can_revoke = 1;/* allow revokers to do their job*/
	}
#endif /* PFS || MAPPED_FILES */
	fdte_unlock(fdte);
	return (ESUCCESS);
}

int
token_revoke(token)
        mach_port_t     token;
{
	fdt_entry_t	*fdte;
	int		error;

	/*
         * Map from token to an fdte, returning with the fdte_lock held.
	 * It's possible we can't find a fdte due to races:
	 * - token_revoke processed prior to completion of token_acquire.
	 * - token_revoke processed in parallel with token_release.
	 *
	 * In the latter case, the server thread initiating the revoke
	 * will be awoken via the token_release in progress.  In the fomer,
	 * we must force the server thread to retry (by returning EINVAL).
	 */

	if ((fdte = fdt_token_lookup(token)) == NULL) {
		/* 
		 * Force the server thread to retry.
		 */
                if (edebug & EDEBUG_MF)
			EPRINT(("token_revoke: token not found"));

		/* async. message */
                error = fsvr_token_not_found(token, token);
                if (error) 
			/*
			 * There may have been an error because the server
			 * blew away it's receive right.  That's ok.
			 */
                        EPRINT(("token_revoke: release error=0x%x",error));

		return(ESUCCESS);
	}

#ifdef PFS
	if ((fdte->token == PFS_TOKEN_MAGIC) &&
            (token == fdte->pfs_iomode_info->token_mgr_info->server_token)) {
		/*
		 * This indicates that the token revoke is from the server
		 * and that the token needs to be revoked from all of the
		 * nodes using it:
		 */
		tmgr_revoke(fdte);
		fdte_unlock(fdte);
		return (ESUCCESS);
	}
#endif

        if (fdte->can_revoke) {
		EASSERT(!fdte->must_release);
#ifdef PFS
		if ((fdte->token != PFS_TOKEN_MAGIC) &&
		    (PFS_TOKENMGT(fdte)) &&
                    (fdte->pfs_iomode_info->tmgr_req_port != MACH_PORT_NULL)) {
			token_release_to_tmgr(fdte);
		} else
#endif
			token_release_to_server(fdte, TRUE);
        } else {
		/*
		 * The token is held, but it can't be released at the moment.
		 */
                if (edebug & EDEBUG_MF)
			EPRINT(("token_revoke: setting must_release"));
                fdte->must_release = 1;
	}

        fdte_unlock(fdte);
	return (ESUCCESS);
}

/*
 * Get a mapped window into the file at the specified offset and size.
 * This routine cannot fail.  If the return value is non-zero, then the
 * the caller should call release_window (before release the fdte's io_lock).
 */
int
get_window(fdte, offset, size, addr)
	fdt_entry_t	*fdte;
	off_t		offset;	
	int		size;
	vm_address_t	*addr;		/* OUT */
{
	vm_address_t	dealloc_addr;
	vm_prot_t	prot;
	int		dealloc_size;
	int		error;

	if (edebug & EDEBUG_MF)
		EPRINT(("get_window: offset=%d(0x%x), size=%d", 
			offset, offset, size));

	if (fdte->win_size != 0) {
		/*
		 * We currently have a mapped window.
		 * See if it satisfies the request.
		 */
		if (offset >= fdte->win_offset && 
		    offset + size < fdte->win_offset + fdte->win_size) 
			goto success;
		else {
			dealloc_addr = fdte->win_addr;
			dealloc_size = fdte->win_size;
		}

	} else 
		/*
		 * We don't have a mapped window.
		 */
		dealloc_size = 0;

	/*
	 * Get a mapped window.
	 */
	fdte->win_offset = trunc_page(offset);
	fdte->win_size = MAX(round_page(size + offset) - fdte->win_offset, 
					MIN_WIN_SIZE);
	fdte->win_addr = EMULATOR_END;	/* map beyond the emulator */

	if (fdte->mem_obj != MACH_PORT_NULL) {
		/*
		 * Map the memory object locally.
		 */
		if (edebug & EDEBUG_MF)
		    EPRINT(("  Calling emul_vm_map: win_offset=%d, win_size=%d",
			    fdte->win_offset, fdte->win_size));

		/*
		 * Window is protected read-only if file isn't open for write.
		 */
		prot = (fdte->fmode & FWRITE) == 0 ? 
			VM_PROT_READ : VM_PROT_READ|VM_PROT_WRITE;

                /*
                 * We might like to use VM_INHERIT_SHARE in the vm_map call
                 * but it isn't yet implemented across nodes.  fdt_child_init
                 * and fsvr_get_window are also aware of this fact.
                 */
		error = emul_vm_map(mach_task_self(),
				  &fdte->win_addr, fdte->win_size, 0, 
				  TRUE, fdte->mem_obj, fdte->win_offset, FALSE, 
				  prot, prot, VM_INHERIT_NONE);
		if (error) {
			EPRINT(("vm_map failure: offset=%d size=%d error=0x%x",
				fdte->win_offset,fdte->win_size,error));
			emul_panic("get_window.emul_vm_map");
		} 
	} else {
		/* 
		 * XXX This should never happen because the memory object
		 * is always mapped locally.
		 */
		emul_panic("Shouldn't be attempting to call fsvr_get_window");
#ifdef	notdef
		/*
		 * Must ask the server to do the mapping for us.
		 */
		if (edebug & EDEBUG_MF)
		 EPRINT((" Calling get_window: win_offset=%d, win_size=%d",
			 fdte->win_offset, fdte->win_size));

		error = fsvr_get_window(fdte->fp, credentials_port, 
					fdte->win_offset, 
					fdte->win_size, &fdte->win_addr); 
		if (error) {
			EPRINT(("fsvr_get_window: offset=%d size=%d error=0x%x",
				fdte->win_offset,fdte->win_size,error));
			emul_panic("get_window.fsvr_get_window");
		} 
#endif  /* notdef */
	}
	
	if (dealloc_size)
		if ((error = vm_deallocate(mach_task_self(), dealloc_addr,
					 dealloc_size)) != KERN_SUCCESS) {
			EPRINT(("vm_deallocate failure: addr=0x%x, size=%d",
				dealloc_addr, dealloc_size));
			emul_panic("get_window.vm_deallocate");
		}

 success:
	*addr = (vm_address_t) ((char *) fdte->win_addr + 
				(offset - fdte->win_offset));
	/*
	 * If the window size is > MIN_WIN_SIZE then the 'size' arg must
	 * have been > MIN_WIN_SIZE.  This implies the caller is doing
	 * a large I/O and most likely won't benefit from the emulator
	 * caching the mapped window.  Hence, inform the caller to discard
	 * the mapped window when it's through (by calling release_window).
	 * This code is necessary to avoid running out of VA space in 
	 * the appl. due to caching many large mapped windows.
	 */
	return(fdte->win_size > MIN_WIN_SIZE ? 1 : 0);
}

/*
 * Release a window obtained from get_window().
 */
void 
release_window(fdte)
	fdt_entry_t	*fdte;
{
	int		error;

	EASSERT(fdte->win_size != 0);

	if ((error = vm_deallocate(mach_task_self(), fdte->win_addr,
				   fdte->win_size)) != KERN_SUCCESS) {
		EPRINT(("vm_deallocate failure 0x%x: addr=0x%x, size=%d",
			error, fdte->win_addr, fdte->win_size));
		emul_panic("release_window.vm_deallocate");
	}

	fdte->win_size = 0;		/* indicate lack of window */
}

#endif

int
e_lseek(proc_port, interrupt, fdes, inoffset, sbase, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	off_t		inoffset;
	int		sbase;
	int		*rval;		/* OUT */
{
	int		error;
	fdt_entry_t	*fdte;
	transaction_id_t trans_id;
#if	MAPPED_FILES | PFS
	off_t		offset;
	char		flags;
	int		length;
#endif
#ifdef	PFS
	int		queued = 0;
	async_req	dummy_req;
#endif	/* PFS */

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

#ifdef	PFS
	/* test to see if asynchronous fdte wait queue must be used */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req,
				       &queued)) {
			goto lseek_exit;
		}
	}

	if ( (fdte->pfs_iomode_info) &&
	     ((fdte->pfs_iomode == M_RECORD) ||
	      (fdte->pfs_iomode == M_SYNC)   ||
	      (fdte->pfs_iomode == M_GLOBAL)) ) {
		esize_t new_offset;
		esize_t ex_inoffset;
		int	dont_care;

		ex_inoffset = __eadd1(ex_zero, (long)inoffset, &dont_care);
		error = pfs_iomode_lseek(fdte, &ex_inoffset, sbase, 
					 &new_offset);
		rval[0] = error ? -1 : new_offset.slow;
		goto lseek_exit;
	}
#endif
#ifdef 	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) {
		fdte_io_lock(fdte);  /* atomicity lock */

		/*
		 * We acquire a data token because, 1) we may need it for  
		 * seeking relative to the end-of-file (need file length), 
		 * or 2) lseek usually preceeds an I/O, so acquiring the data 
		 * read token at the same time is an optimization.  
		 */

		flags = TOK_OFFSET | TOK_DATA_READ;
		/* last arg '0' is for debug only - indicates lseek is caller */
		error = token_acquire(fdte, interrupt, flags, &offset, 
				      &length, 0);
		if (!error) {
			switch (sbase) {

			case L_INCR:
				offset += inoffset;
				break;

			case L_XTND:
				offset = inoffset + length;
				break;

			case L_SET:
				offset = inoffset; 
				break;

			default:	
				offset = -1;
				/* fall through */
			}

			/*
			 * OSF/1 monolithic calls VOP_SEEK at this point, but of
			 * of course we want to avoid the RPC.  Implementation 
			 * of VOP_SEEK by ufs, nfs, and sysVufs all simply 
			 * check to make sure newoffset is not < 0.
			 */
			if ((int) offset < 0) {
				offset = -1;  	/* offset won't be changed */
				error = EINVAL;
			}

			token_release(fdte, interrupt, flags, offset, -1);
			fdte_io_unlock(fdte);
			/* if error, rval will be -1 which is what we want */
			rval[0] = offset;  
			goto lseek_exit;

		} else {
			fdte_io_unlock(fdte);
			rval[0] = -1;
			goto lseek_exit;
		}
	}
#endif	/* MAPPED_FILES */

#ifdef	PFS
	if (fdte->pfs_fd) {
		esize_t ex_inoffset;
		esize_t new_offset;
		int	dont_care;

		ex_inoffset = __eadd1(ex_zero, (long)inoffset, &dont_care);
		error = pfs_multi_lseek(interrupt, fdte, ex_inoffset, sbase,
					FALSE, &new_offset);
		rval[0] = (error) ? -1 : new_offset.slow;
		goto lseek_exit;
	}
#endif	/* PFS */

	if (fdte->iomode == VIO_FASTPATH) {
		error = e_lseek2(fdte, inoffset, sbase, rval);
	} else {
		isc_register(fdte->fp, &trans_id);
		error = fsvr_lseek(fdte->fp,
				   credentials_port,
				   trans_id,
				   inoffset,
				   sbase,
				   rval);
		isc_deregister(interrupt);
	}

lseek_exit:
#ifdef	PFS
	/* if we had been queued previously we must exit the queue */
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);
#endif
	(void) fdt_unref_entry(fdte);
	return(error);	
}

#ifdef	PFS
int
e_read(proc_port, interrupt, fdes, data, count, rval)
	mach_port_t	proc_port;
	int		*interrupt;	/* out */
	int		fdes;
	char		*data;
	unsigned int	count;
	int		*rval;		/* out */
{
	int		error;
	int		queued = 0;
	fdt_entry_t	*fdte;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	/* test to see if asynchronous fdte wait queue must be used */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req,
				       &queued)) {
			goto read_out;
		}
	}

	error = sync_read(proc_port, interrupt, fdte, data, count, rval);

	/* if we had been queued previously we must exit the queue */
read_out:
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);

	fdt_unref_entry(fdte);

	return(error);
}

/*
 * Changed from e_read to sync_read to support asychronous requests.
 */
int
sync_read(proc_port, interrupt, fdte, data, count, rval)
	mach_port_t	proc_port;
	int		*interrupt;	/* out */
	fdt_entry_t	*fdte;
	char		*data;
	unsigned int	count;
	int		*rval;		/* out */
#else
int
e_read(proc_port, interrupt, fdes, data, count, rval)
	mach_port_t	proc_port;
	int		*interrupt;	/* out */
	int		fdes;
	char		*data;
	unsigned int	count;
	int		*rval;		/* out */
#endif	/* PFS */
{
	char		*addr;
	int		amount, error;
	transaction_id_t trans_id;
#ifndef	PFS
	fdt_entry_t	*fdte;
#endif	/* PFS */
#if 	MAPPED_FILES | PFS
	off_t		offset;
	char		flags;
	int		length, release = 0;
#endif
#ifdef	PFS
	esize_t		ex_offset;
	esize_t		ex_length;
	int		pfs_global_snd_flg = 0;
	int		orig_count = count;
#endif

	rval[0] = 0;

#ifndef	PFS
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);
#endif
#ifdef LOCAL_DEVNULL
	if (fdte->iomode == VIO_DEVNULL) {
	    rval[0] = 0;
	    error = ESUCCESS;
	    goto out;
	}
#endif /* LOCAL_DEVNULL */

#ifdef	PFS
	if (fdte->pfs_iomode_info) {
		if (fdte->pfs_iomode == M_SYNC) {
			/*
		 	 * Need to synchronize with the other nodes
		 	 * in M_SYNC I/O mode.
		 	 */
			if (error = pfs_iomode_syncoff( fdte,
							count,
							PFS_OP_READ,
							interrupt)) {
				goto out;
			}

		} else if (fdte->pfs_iomode == M_GLOBAL) {

			if (fdte->pfs_iomode_info->my_node_number == 0) {
				pfs_global_snd_flg = 1;

			} else {
				/*
			 	 * Handle the reception of the data 
			 	 * here instead of going through the 
			  	 * normal file i/o logic.
#ifdef DEBUG_GLOBAL
	EPRINT(("fsvr_read: M_GLOBAL, calling pfs_global_rcv\n"));
#endif
			 	 */
				error = pfs_global_rcv(	fdte,	
							PFS_OP_READ,
							data,
							count,
							rval);
				return error;
			}
		}
	}
#endif

#ifdef 	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) {
		if ((int)count < 0) {
			error = EINVAL;
			goto out;
		}

		if ((fdte->fmode & FREAD) == 0) {
			error = EBADF;
			goto out;
		}

		fdte_io_lock(fdte);  /* atomicity lock */

		flags = TOK_OFFSET | TOK_DATA_READ;
		/* last arg '1' is for debug only - indicates read is caller */
#ifdef	PFS
		error = file_token_acquire(fdte, interrupt, PFS_OP_READ, count,
					   &ex_offset, &ex_length);
		offset = ex_offset.slow;
		length = ex_length.slow;
#else
		error = token_acquire(fdte, interrupt, flags, &offset, 
				      &length, 1);
#endif
		if (!error) {
			/*
			 * Restrict the amount to read by the end-of-file.
			 */
			if (offset >= length)
				count = 0;
			else
				count = MIN(count, length - offset);

			if (count > 0) {
				/*
				 * Get a mapped window into the memory object.
				 */
				release = get_window(fdte, offset, count, 
						     (vm_address_t *)&addr);

				/*
				 * Copy data from the memory object.
				 * If an error occurrs, the returned count is 
				 * the number of bytes copied prior to the err.
				 */
				error = user_bcopy2(addr, data, &count);
				if (count > 0) {
					fdte_lock(fdte);
					fdte->accessed = 1;
					fdte_unlock(fdte);
					/* if any bytes copied, ret success */
					error = ESUCCESS;
				}
			}
#ifdef	PFS
			ex_offset.slow = offset + count;
			ex_offset.shigh = 0;
			file_token_release(fdte, interrupt, PFS_OP_READ, count,
					   &ex_offset, &ex_neg_one);
#else
			token_release(fdte, interrupt, flags, offset+count, -1);
#endif
			if (release)
				release_window(fdte);
			fdte_io_unlock(fdte);
			rval[0] = count;
			goto out;		
		} else {
			fdte_io_unlock(fdte);
			goto out;
		}
	}
#endif	/* MAPPED_FILES */

#ifdef	PFS
	if (fdte->pfs_fd) {
		error = pfs_multi_read(interrupt, fdte, PFS_OP_READ,
				       data, count, rval);
		goto out;
	}
#endif	/* PFS */

#ifdef	TNC
	if (fdte->notify_on_migrate && ! fdte->was_notified)
		report_migration(fdte);
#endif
	/*
	 * Find out how much of the user's buffer is valid.
	 */
	if (!user_rwcheck(data, count)) {
		error = EFAULT;
		goto out;
	}

	if (fdte->iomode == VIO_FASTPATH) 
		error = e_read2(fdte, count, &addr, &count);
	else {
#ifdef  PFS
                if (fdte->pfs_iomode_info) {
                        error = file_token_acquire(fdte, interrupt, PFS_OP_READ,
                                                   count, &ex_offset,
                                                   &ex_length);
                        if (error)
                                goto out;
                }
		isc_register_chk_async(fdte->fp, &trans_id);
#else
		isc_register(fdte->fp, &trans_id);
#endif
		error = fsvr_read(fdte->fp,
				  credentials_port,
				  trans_id,
				  count,
				  &addr,
				  &count);
		isc_deregister(interrupt);
#ifdef PFS
                if (fdte->pfs_iomode_info) {
                        file_token_release(fdte, interrupt, PFS_OP_READ, count,
                                           &ex_offset, &ex_neg_one);
                }
#endif
	}

	if (error == ESUCCESS) {
		amount = count;
		if ((error = user_bcopy2(addr, data, &count)) && count > 0)
			error = ESUCCESS;  /* some bytes were copied */
		(void) vm_deallocate(mach_task_self(), (vm_address_t)addr,
				     (vm_size_t)amount);
		rval[0] = count;
	}

out:
#ifdef	PFS
	if (pfs_global_snd_flg) {
		error = pfs_global_snd(	fdte,
					PFS_OP_READ,
					data,
					orig_count,
					rval[0],
					error);
	}
#endif
#ifndef	PFS
	(void) fdt_unref_entry(fdte);
#endif	/* PFS */
	return(error);
}


#ifdef	PFS
int
e_readv(proc_port, interrupt, fdes, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
{
	int		error;
	int		queued = 0;
	fdt_entry_t	*fdte;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	/* test to see if asynchronous fdte wait queue must be used */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req,
				       &queued)) {
			goto readv_out;
		}
	}

	error = sync_readv(proc_port, interrupt, fdte, iov, iovcnt, rval);

	/* if we had been queued previously we must exit the queue */
readv_out:
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);

	fdt_unref_entry(fdte);

	return(error);
}

/*
 * Changed from e_readv to sync_readv to support asychronous requests.
 */
int
sync_readv(proc_port, interrupt, fdte, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	fdt_entry_t	*fdte;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
#else
int
e_readv(proc_port, interrupt, fdes, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
#endif	/* PFS */
{
	register int	i;
	register struct iovec *iovp;
	char		*addr;
	unsigned int	count;
	char		*data_addr;
	int		len, error = ESUCCESS;
	transaction_id_t trans_id;
#ifndef	PFS
	fdt_entry_t     *fdte;
#endif
	int		amount;
#if 	MAPPED_FILES | PFS
	off_t		offset;
	char		flags;
	int		length, release = 0;
#endif
#ifdef	PFS
	esize_t		ex_offset;
	esize_t		ex_length;
	int		pfs_global_snd_flg = 0;
	int		orig_count;
#endif

	rval[0] = 0;

	if (iovcnt > 16) 
		return (EINVAL);
	if (!user_rcheck(iov, iovcnt * sizeof iov))
		return EFAULT;

#ifndef	PFS
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);
#endif	/* PFS */

#ifdef LOCAL_DEVNULL
	if (fdte->iomode == VIO_DEVNULL) {
	    rval[0] = 0;
	    error = ESUCCESS;
	    goto out;
	}
#endif /* LOCAL_DEVNULL */

#ifdef	PFS
	if ((fdte->pfs_iomode_info) && 
	   ((fdte->pfs_iomode == M_SYNC) || 
	    (fdte->pfs_iomode == M_GLOBAL))) {
		/*
		 * Compute the count:
		 */
		count = 0;
		for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
			if (iovp->iov_len < 0) {
				error = EINVAL;
				goto out;
			}
			count += iovp->iov_len;
			if ((int)count < 0) {
				error = EINVAL;
				goto out;
			}
		}
		orig_count = count;
		if (fdte->pfs_iomode == M_SYNC) {
			/*
		 	 * Need to synchronize with the other nodes
		 	 * in M_SYNC I/O modes.
		 	 */
			if (error = pfs_iomode_syncoff( fdte,
							count,
							PFS_OP_READV,
							interrupt)) {
				goto out;
			}

		} else if (fdte->pfs_iomode == M_GLOBAL) {

			if (fdte->pfs_iomode_info->my_node_number == 0) {
				pfs_global_snd_flg = 1;

			} else {
				/*
			 	 * Handle the reception of the data 
			 	 * here instead of going through the 
			  	 * normal file i/o logic.
			 	 */
				error = pfs_global_rcv_vec(fdte,	
							   PFS_OP_READV,
							   iov,
							   iovcnt,
							   count,
							   rval);

				return error;
			}
		}
	}
#endif

#ifdef 	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) {
		count = 0;
		for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
			if (iovp->iov_len < 0) {
				error = EINVAL;
				goto out;
			}
			count += iovp->iov_len;
			if ((int)count < 0) {
				error = EINVAL;
				goto out;
			}
		}

		if ((fdte->fmode & FREAD) == 0) {
			error = EBADF;
			goto out;
		}
		fdte_io_lock(fdte);  /* atomicity lock */

		flags = TOK_OFFSET | TOK_DATA_READ;
		/* last arg '1' is for debug only - indicates read is caller */
#ifdef	PFS
		error = file_token_acquire(fdte, interrupt, PFS_OP_READV,
					   count, &ex_offset, &ex_length);
		offset = ex_offset.slow;
		length = ex_length.slow;

#else
		error = token_acquire(fdte, interrupt, flags, &offset,
				      &length, 1);
#endif
		if (!error) {
			/*
			 * Restrict the amount to read by the end-of-file.
			 */
			if (offset >= length)
				count = 0;
			else
				count = MIN(count, length - offset);

			if (count > 0) {
				/*
				 * Get a mapped window into the memory object.
				 */
				release = get_window(fdte, offset, count,
						     (vm_address_t *)&addr);
#ifdef	PFS
				data_addr = addr;	/* Save address for
							   later use. */
#endif
				/*
				 * Copy data from the memory object.
				 */
				amount = count;
				for (i = 0, iovp = iov; i < iovcnt; 
				     i++, iovp++) {
					len = MIN(iovp->iov_len, amount);
					error = user_bcopy2(addr, 
							    iovp->iov_base, 
							    &len);
					if ((amount -= len) == 0 || error)
						break;
					addr += len;
				}

				if ((count -= amount) > 0) {
					fdte_lock(fdte);
					fdte->accessed = 1;
					fdte_unlock(fdte);
					/* if any bytes copied, ret success */
					error = ESUCCESS;
				}
			}

#ifdef	PFS
			ex_offset.slow = offset + count;
			ex_offset.shigh = 0;
			file_token_release(fdte, interrupt, PFS_OP_READV,
					   count, &ex_offset, &ex_neg_one);
#else
			token_release(fdte, interrupt, flags, offset+count, -1);
#endif
			if (release)
				release_window(fdte);
			fdte_io_unlock(fdte);
			rval[0] = count;
			goto out;		
		} else {
			fdte_io_unlock(fdte);
			goto out;
		}
	}
#endif	/* MAPPED_FILES */

#ifdef	TNC
	if (fdte->notify_on_migrate && ! fdte->was_notified)
		report_migration(fdte);
#endif

	count = 0;
	for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
		amount = iovp->iov_len;
		if (amount < 0) {
			error = EINVAL;
			goto out;
		}
		/*
		 * Is the buffer valid?
		 */
		if (!user_rwcheck(iovp->iov_base, amount)) {
			error = EFAULT;
			goto out;
		}
		count += amount;
#if 0					/* user_rwcheck2 */
		if (error) {
			if (count == 0) {
				error = EFAULT;
				goto out;
			} else
				break;
		} 
		if (count < 0) {
			error = EINVAL;
			goto out;
		}
#endif
	}

#ifdef	PFS
	if (fdte->pfs_fd) {
		error = pfs_multi_read(interrupt, fdte, PFS_OP_READV,
				       (caddr_t)iov, count, rval);

		goto out;
	}
#endif	/* PFS */

	if (fdte->iomode == VIO_FASTPATH) 
		error = e_read2(fdte, count, &data_addr, &count);
	else {
#ifdef PFS
		if (fdte->pfs_iomode_info) {
			error = file_token_acquire(fdte, interrupt,
						   PFS_OP_READV, count,
						   &ex_offset, &ex_length);
			if (error)
				goto out;
		}
		isc_register_chk_async(fdte->fp, &trans_id);
#else
		isc_register(fdte->fp, &trans_id);
#endif
		error = fsvr_read(fdte->fp,
				  credentials_port,
				  trans_id,
				  count,
				  &data_addr,
				  &count);
		isc_deregister(interrupt);
#ifdef PFS
		if (fdte->pfs_iomode_info) {
			file_token_release(fdte, interrupt, PFS_OP_READV,
					   count, &ex_offset, &ex_neg_one);
		}
#endif

	}

	if (error == 0) {
		addr = data_addr;
		amount = count;
		for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
			len = MIN(iovp->iov_len, amount);
			error = user_bcopy2(addr, iovp->iov_base, &len);
			if ((amount -= len) == 0 || error)
				break;
			addr += len;
		}

		(void) vm_deallocate(mach_task_self(), (vm_offset_t)data_addr,
				     count);
		rval[0] = count - amount;
		if (rval[0] > 0)
			error = ESUCCESS;
	}

out:
#ifdef	PFS
	if (pfs_global_snd_flg) {
		error = pfs_global_snd_vec( fdte,
					    PFS_OP_READV,
					    iov,
					    iovcnt,
					    orig_count,
					    rval[0],
					    error);
	}
#endif
#ifndef	PFS
	(void) fdt_unref_entry(fdte);
#endif	/* PFS */
	return(error);
}


#ifdef	PFS
int
e_write(serv_port, interrupt, fdes, data, count, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	char		*data;
	unsigned int	count;
	int		*rval;		/* OUT */
{
	int		error;
	int		queued = 0;
	fdt_entry_t	*fdte;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	/* test to see if asynchronous fdte wait queue must be used */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req,
				       &queued)) {
			goto write_out;
		}
	}

	error = sync_write(serv_port, interrupt, fdte, data, count, rval);

	/* if we had been queued previously we must exit the queue */
write_out:
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);

	fdt_unref_entry(fdte);

	return(error);
}

/*
 * Changed from e_write to sync_write to support asychronous requests.
 */
int
sync_write(serv_port, interrupt, fdte, data, count, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	fdt_entry_t	*fdte;
	char		*data;
	unsigned int	count;
	int		*rval;		/* OUT */
#else
int
e_write(serv_port, interrupt, fdes, data, count, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	char		*data;
	unsigned int	count;
	int		*rval;		/* OUT */
#endif	/* PFS */
{
	int		error;
	transaction_id_t trans_id;
#ifndef	PFS
	fdt_entry_t	*fdte;
#endif	/* PFS */
#if 	MAPPED_FILES | PFS
	char		*addr;
	off_t		offset;
	char		flags;
	int		length, release = 0;
#endif
#ifdef	PFS
	esize_t		ex_offset;
	esize_t		ex_length;
	int		orig_count = count;
	int		pfs_global_snd_flg = 0;
#endif

	rval[0] = 0;	

	if ((int)count < 0)
		return EINVAL;

#ifndef	PFS
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);
#endif	/* PFS */

#ifdef LOCAL_DEVNULL
	if (fdte->iomode == VIO_DEVNULL) {
	    rval[0] = count;
	    error = ESUCCESS;
	    goto out;
	}
#endif /* LOCAL_DEVNULL */

#ifdef	PFS
	if (fdte->pfs_iomode_info) {

		if (fdte->pfs_iomode == M_SYNC) {

			if (error = pfs_iomode_syncoff( fdte,
							count,
							PFS_OP_WRITE,
							interrupt)) {
				goto out;
			}

		} else if (fdte->pfs_iomode == M_GLOBAL) {

			if (fdte->pfs_iomode_info->my_node_number == 0) {
				pfs_global_snd_flg = 1;

			} else {
				/*
				 * Handle the reception of the write status
				 * here instead of going through the normal
				 * file I/O logic.
				 */
				 error = pfs_global_rcv( fdte,
							 PFS_OP_WRITE,
							 data,
							 count,
							 rval);
				return error;
			}
		}
	}
#endif

	if ((int)count == 0) {
#ifdef	PFS
		if (fdte->pfs_iomode_info == NULL) {
#endif
			error = ESUCCESS;
			goto out;
#ifdef	PFS
		}
#endif
	}

#ifdef 	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) {
		if ((fdte->fmode & FWRITE) == 0) {
			error = EBADF;
			goto out;
		}

		fdte_io_lock(fdte);  /* atomicity lock */

		flags = TOK_OFFSET | TOK_DATA_WRITE;
		/* last arg '2' is for debug only - indicates write is caller */
#ifdef	PFS
		error = file_token_acquire(fdte, interrupt, PFS_OP_WRITE, count,
					   &ex_offset, &ex_length);
		offset = ex_offset.slow;
		length = ex_length.slow;
#else
		error = token_acquire(fdte, interrupt, flags, &offset,
				      &length, 2);
#endif
		if (!error) {
			/*
			 * Handle append mode writes.
			 */
			if (fdte->fmode & FAPPEND) 
#ifdef	PFS
				if ((fdte->pfs_iomode_info == NULL ) ||  
				    (fdte->pfs_iomode != M_RECORD))
#endif
					offset = length;

			/*
			 * Check for EFBIG.
			 */
			if (offset >= rlimit_fsize) {
#ifdef	PFS
				file_token_release(fdte, interrupt, 
						   PFS_OP_WRITE, count,
						   &ex_neg_one, &ex_neg_one);
#else
				token_release(fdte, interrupt, flags, -1, -1);
#endif
				fdte_io_unlock(fdte);
				send_sig(SIGXFSZ, interrupt);
				error = EFBIG;
				goto out;
			}
			if (offset + count > rlimit_fsize)
				count = rlimit_fsize - offset;

			/*
			 * Get a mapped window into the memory object.
			 */
			release = get_window(fdte, offset, count,
					     (vm_address_t *)&addr);

			/*
			 * Copy data to the memory object.
			 * If an error occurrs, the returned count is 
			 * the number of bytes copied prior to the err.
			 */
			error = user_bcopy2(data, addr, &count);

			if (count > 0) {
				/* update the length if necessary */
				if (offset + count > length)
					length = offset + count;

				fdte_lock(fdte);
				fdte->modified = 1;	
				fdte_unlock(fdte);
				/* if any bytes copied, return success */
				error = ESUCCESS; 

				/*
				 * Update the VA range that needs to be cleaned.
				 */
				if (offset < fdte->min_offset)
					fdte->min_offset = offset;
				if (offset + count > fdte->max_offset)
					fdte->max_offset = offset + count;
			}
#ifdef PFS
			/*
			 * See if file was opened with O_SYNC, if so then
			 * the file data needs to be flushed to the disk
			 * before returning to the user.
			 */
			if ((fdte->fmode & FSYNC) &&
			    (fdte->pfs_iomode != M_SYNC)) {

				isc_register_chk_async(fdte->fp, &trans_id);
			
				error = fsvr_sync_data(fdte->token,length,
						       fdte->min_offset,
						       fdte->max_offset);
				if (error) {
					EPRINT(("fsvr_sync_data error = 0x%x",
						error));
					emul_panic("fsvr_sync_data error");
				}
				/*
				 * Reset modified since the fsvr_sync_data()
				 * already updated the file.
				 */ 
				fdte->modified = 0;
				isc_deregister(interrupt);
			}

			ex_offset.slow = offset + count;
			ex_offset.shigh = 0;
			ex_length.slow = length;
			ex_length.shigh = 0;

			file_token_release(fdte, interrupt, PFS_OP_WRITE, count,
						&ex_offset, &ex_length);
#else
			token_release(fdte, interrupt, flags, offset+count, 
				      length);
#endif

			if (release)
				release_window(fdte);
			fdte_io_unlock(fdte);
			rval[0] = count;
			goto out;		
		} else {
			fdte_io_unlock(fdte);
			goto out;
		}
	}
#endif	/* MAPPED_FILES */

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif

	/*
	 * Find out how much of the user's buffer is valid.
	 */
	if (!user_rcheck(data, count)) {
		error = EFAULT;
		goto out;
	}

#ifdef	PFS
	if (fdte->pfs_fd) {
		error = pfs_multi_write(interrupt, fdte, PFS_OP_WRITE,
					data, count, rval);
		if (error == EPIPE) 
			send_sig(SIGPIPE, interrupt);
		else if (error == EFBIG) 
			send_sig(SIGXFSZ, interrupt);
		goto out;
	}
#endif	/* PFS */

	if (fdte->iomode == VIO_FASTPATH) 
		error = e_write2(fdte, data, count, &rval[0]);
	else {
#ifdef  PFS
		if (fdte->pfs_iomode_info) {
			error = file_token_acquire(fdte, interrupt,
						   PFS_OP_WRITE, count,
						   &ex_offset, &ex_length);
			if (error)
				goto out;
		}
		isc_register_chk_async(fdte->fp, &trans_id);
#else
		isc_register(fdte->fp, &trans_id);
#endif
		error = fsvr_write(fdte->fp,
				   credentials_port,
				   trans_id,
				   data,
				   count,
				   &rval[0]);
		isc_deregister(interrupt);
#ifdef PFS
		if (fdte->pfs_iomode_info) {
                	file_token_release(fdte, interrupt, PFS_OP_WRITE, count,
                       	                    &ex_offset, &ex_length);
		}
#endif
	}

	if (error == EPIPE) 
		send_sig(SIGPIPE, interrupt);
	else if (error == EFBIG) 
		send_sig(SIGXFSZ, interrupt);

out:	
#ifdef	PFS
	if (pfs_global_snd_flg) {
		/*
		 * Send the status of the write to the next node.
		 */
		error = pfs_global_snd ( fdte,
					 PFS_OP_WRITE,
					 data,
					 orig_count,
					 rval[0],
					 error);
	}
#endif
#ifndef	PFS
	(void) fdt_unref_entry(fdte);
#endif	/* PFS */
	return(error);
}


#ifdef	PFS
int
e_writev(proc_port, interrupt, fdes, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
{
	int		error;
	int		queued = 0;
	fdt_entry_t	*fdte;
	async_req	dummy_req;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	/* test to see if asynchronous fdte wait queue must be used */
	if (fdte->async_queue != NULL) {
		if (error = queue_sync(fdte->async_queue, &dummy_req,
				       &queued)) {
			goto writev_out;
		}
	}

	error = sync_writev(proc_port, interrupt, fdte, iov, iovcnt, rval);

	/* if we had been queued previously we must exit the queue */
writev_out:
	if (queued)
		dequeue_sync(fdte->async_queue, &dummy_req);

	fdt_unref_entry(fdte);

	return(error);
}

/*
 * Changed from e_writev to sync_writev to support asychronous requests.
 */
int
sync_writev(proc_port, interrupt, fdte, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	fdt_entry_t	*fdte;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
#else
int
e_writev(proc_port, interrupt, fdes, iov, iovcnt, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	struct iovec	*iov;
	unsigned	iovcnt;
	int		*rval;		/* OUT */
#endif	/* PFS */
{
	register int	i;
	register struct iovec *iovp;
	char		*addr;
	unsigned int	count;
	int		amount, temp, error;
	char 		*bufptr;
	char		buf[SMALL_ARRAY_LIMIT];
	transaction_id_t trans_id;
#ifndef	PFS
	fdt_entry_t     *fdte;
#endif	/* PFS */
#if 	MAPPED_FILES | PFS
	off_t		offset;
	char		flags;
	int		length, release = 0;
	boolean_t	length_updated = FALSE;
#endif
#ifdef	PFS
	esize_t		ex_offset;
	esize_t		ex_length;
	int		pfs_global_snd_flg = 0;
	int		orig_count;
#endif

	rval[0] = 0;

	if (iovcnt > 16)
	    return(EINVAL);
	if (!user_rcheck(iov, iovcnt * sizeof iov))
		return EFAULT;

	count = 0;
	for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
	    if (iovp->iov_len < 0) 
		return(EINVAL);
	    count += iovp->iov_len;
	    if ((int)count < 0) 
		return(EINVAL);
	}

        if ((int)count == 0)
#ifdef	PFS
		if (fdte->pfs_iomode_info == NULL)
#endif
			return(ESUCCESS);

#ifndef	PFS
	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);
#endif	/* PFS */

#ifdef LOCAL_DEVNULL
	if (fdte->iomode == VIO_DEVNULL) {
	    rval[0] = count;
	    error = ESUCCESS;
	    goto out;
	}
#endif /* LOCAL_DEVNULL */

#ifdef	PFS
	if (fdte->pfs_iomode_info) {

		if (fdte->pfs_iomode == M_SYNC) {

			if (error = pfs_iomode_syncoff(	fdte,
							count,
							PFS_OP_WRITEV,
							interrupt)) {
				goto out;
			}

		} else if (fdte->pfs_iomode == M_GLOBAL) {

                	orig_count = count;
			if (fdte->pfs_iomode_info->my_node_number == 0) {
				pfs_global_snd_flg = 1;

			} else {
				/*
				 * Handle the reception of the write status
				 * here instead of going through the normal
				 * file I/O logic.
				 */
				error = pfs_global_rcv(	fdte,
							PFS_OP_WRITEV,
							NULL,
							count,
							rval);
				return error;
			}
		}
	}
#endif
#ifdef 	MAPPED_FILES
	if (fdte->iomode == VIO_MAPPED) {
		if ((fdte->fmode & FWRITE) == 0) {
			error = EBADF;
			goto out;
		}

		fdte_io_lock(fdte);  /* atomicity lock */

		flags = TOK_OFFSET | TOK_DATA_WRITE;
		/* last arg '2' is for debug only - indicates write is caller */
#ifdef	PFS
		error = file_token_acquire(fdte, interrupt,
					   PFS_OP_WRITEV, count,
					   &ex_offset, &ex_length);
		offset = ex_offset.slow;
		length = ex_length.slow;
#else
		error = token_acquire(fdte, interrupt, flags, &offset,
				      &length, 2);
#endif
		if (!error) {
			/*
			 * Handle append mode writes.
			 */
			if (fdte->fmode & FAPPEND)
#ifdef	PFS
				if ((fdte->pfs_iomode_info == NULL ) ||  
				    (fdte->pfs_iomode != M_RECORD))
#endif
				offset = length;  

			/*
			 * Check for EFBIG.
			 */
			if (offset >= rlimit_fsize) {
#ifdef	PFS
				file_token_release(fdte, interrupt,
						   PFS_OP_READ, count,
						   &ex_neg_one, &ex_neg_one);
#else
				token_release(fdte, interrupt, flags, -1, -1); 
#endif
				fdte_io_unlock(fdte);
				send_sig(SIGXFSZ, interrupt);
				error = EFBIG;
				goto out;
			}
			if (offset + count > rlimit_fsize)
				count = rlimit_fsize - offset;

			/*
			 * Get a mapped window into the memory object.
			 */
			release = get_window(fdte, offset, count,
					     (vm_address_t *)&addr);

			/*
			 * Copy data to the memory object.
			 */
			count = 0;
			for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
				amount = iovp->iov_len;
				error = user_bcopy2(iovp->iov_base, addr+count, 
					    &amount);
				count += amount;
				if (error)
					break;
			}

			if (count > 0) {
				/* update the length if necessary */
				if (offset + count > length)
					length = offset + count;

				fdte_lock(fdte);
				fdte->modified = 1;	
				fdte_unlock(fdte);
				/* if any bytes copied, return success */
				error = ESUCCESS; 

				if (length_updated == TRUE)
					length = offset + count;

				/*
				 * Update the VA range that needs to be cleaned.
				 */
				if (offset < fdte->min_offset)
					fdte->min_offset = offset;
				if (offset + count > fdte->max_offset)
					fdte->max_offset = offset + count;
			}
#ifdef	PFS
			/*
			 * See if file was opened with O_SYNC, if so then
			 * the file data needs to be flushed to the disk
			 * before returning to the user.
			 */
			if ((fdte->fmode & FSYNC) &&
			    (fdte->pfs_iomode != M_SYNC)) {

				isc_register_chk_async(fdte->fp, &trans_id);

				error = fsvr_sync_data(fdte->token,length,
						       fdte->min_offset,
						       fdte->max_offset);
				if (error) {
					EPRINT(("fsvr_sync_data error = 0x%x",
						error));
					emul_panic("fsvr_sync_data error");
				}
				/*
				 * Reset modified since the fsvr_sync_data()
				 * already updated the file.
				 */ 
				fdte->modified = 0;
				isc_deregister(interrupt);
			}
			ex_offset.slow = offset + count;
			ex_offset.shigh = 0;
			ex_length.slow = length;
			ex_length.shigh = 0;

			file_token_release(fdte, interrupt, PFS_OP_WRITEV, 
					   count, &ex_offset, &ex_length);
#else
			token_release(fdte, interrupt, flags, offset+count, 
				      length);
#endif
			if (release)
				release_window(fdte);
			fdte_io_unlock(fdte);
			rval[0] = count;
			goto out;		
		} else {
			fdte_io_unlock(fdte);
			goto out;
		}
	}
#endif	/* MAPPED_FILES */

#ifdef	PFS
	if (fdte->pfs_fd) {
		error = pfs_multi_write(interrupt, fdte, PFS_OP_WRITEV,
					(caddr_t)iov, count, rval);
		if (error == EPIPE) 
			send_sig(SIGPIPE, interrupt);
		else if (error == EFBIG) 
			send_sig(SIGXFSZ, interrupt);
		goto out;
	}
#endif	/* PFS */

	if (count <= SMALL_ARRAY_LIMIT) {
	    /*
	     * Short write.  Copy into buffer.
	     */
	    bufptr = buf;
	} else {
	    /*
	     * Long write.  Allocate memory to fill.
	     * (Hope that no one uses this to write large
	     * amounts of data; we'll lose on the copying.)
	     * Allocate memory beyond the emulator.
	     */
	    bufptr = (char *) EMULATOR_END;     
	    (void) emul_vm_map(mach_task_self(),
			       (vm_offset_t *)&bufptr, count, 0, TRUE,
			       MEMORY_OBJECT_NULL, 0, FALSE, 
			       VM_PROT_READ|VM_PROT_WRITE, 
			       VM_PROT_READ|VM_PROT_WRITE, VM_INHERIT_NONE);
        }
	
	amount = 0;
	addr = bufptr;
	for (i = 0, iovp = iov; i < iovcnt; i++, iovp++) {
		temp = iovp->iov_len;
		error = user_bcopy2(iovp->iov_base, addr, &temp);
		amount += temp;
		if (error) {
			if (amount == 0) 
				goto free;
			else
				break; 
		}
		addr += temp;
	}

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif

	if (fdte->iomode == VIO_FASTPATH) 
		error = e_write2(fdte, bufptr, amount, &rval[0]);
	else {
#ifdef  PFS
		if (fdte->pfs_iomode_info) {
			error = file_token_acquire(fdte, interrupt,
						   PFS_OP_WRITEV, count,
						   &ex_offset, &ex_length);
			if (error)
				goto out;
		}
		isc_register_chk_async(fdte->fp, &trans_id);
#else
		isc_register(fdte->fp, &trans_id);
#endif
		error = fsvr_write(fdte->fp,
				   credentials_port,
				   trans_id,
				   bufptr,
				   amount,
				   &rval[0]);
		isc_deregister(interrupt);
#ifdef  PFS
		if (fdte->pfs_iomode_info) {
			file_token_release(fdte, interrupt, PFS_OP_WRITEV,
					   amount, &ex_offset, &ex_length);
		}
#endif
	}

	if (error == EPIPE) 
		send_sig(SIGPIPE, interrupt);
	else if (error == EFBIG) 
		send_sig(SIGXFSZ, interrupt);

free:
	if (count > SMALL_ARRAY_LIMIT)
	    (void) vm_deallocate(mach_task_self(), (vm_offset_t)bufptr, count);

out:
#ifdef	PFS
	if (pfs_global_snd_flg) {
		/*
		 * Send the status of the write to the next node.
		 */
		error = pfs_global_snd ( fdte,
					 PFS_OP_WRITEV,
					 NULL,
					 orig_count,
					 rval[0],
					 error);
	}
#endif
#ifndef	PFS
	(void) fdt_unref_entry(fdte);
#endif	/* PFS */
	return(error);
}


/*
 * dopoll - common processing for poll and select
 */
int
dopoll(proc_port, interrupt, nfps, file_ports, events, revents, timeout)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	mach_port_t	file_ports[];
	int		nfps;
	short		events[];
	short		revents[];	/* OUT */
	mach_msg_timeout_t
			timeout;	/* # of ms to wait; -1 = wait forever */
{
	int		i;
	int		error = 0, ret = 0;
	int		do_timeout;
	mach_port_t	delayed_reply_port = MACH_PORT_NULL;
	struct ps_reply	ps_reply;	/* defined in bsd_msg.h */
	transaction_id_t trans_id;

	/*
	 * General strategy: For each fp, send a message to which the server
	 * will immediately reply, saying whether or not its specified
	 * conditions have IMMEDIATELY been met.  If 1) the caller requested
	 * a non-zero timeout value (including an infinite value), and 2) no
	 * prior fp has already met its specified conditions immediately, then
	 * the message tells the server to make an additional, delayed reply
	 * (if its conditions are not immediately met) when its conditions
	 * ARE met.  If a timeout was requested and no fp met its conditions
	 * immediately, then we wait at the delayed reply port for up to the
	 * timeout value.  If no delayed replies arrive before time runs
	 * out, then we are done.  If something arrives we gather all
	 * such messages that have arrived "simultaneously" at the delayed
	 * reply port and then deallocate the port.
	 * The fp (server side) cleans up after itself on its own without
	 * additional help from us here in the emulator.
	 */

	/* zero the returned-events array */
	bzero((char *)(&revents[0]), nfps*sizeof(revents[0]));

	/* allocate the delayed reply port */
	do_timeout = (timeout != 0);
	if (do_timeout) {

		error = mach_port_allocate(mach_task_self(),
				MACH_PORT_RIGHT_RECEIVE, &delayed_reply_port);
		if (error != KERN_SUCCESS) {
			EPRINT(("dopoll: can't alloc delayed port err=0x%x",
					error));
			emul_panic("dopoll: fatal error 1");
		}
	}

	/* 
	 * Scan through all the file ports, performing bsd_sel_poll_*() on 
	 * each. Note that we may change from bsd_sel_poll_delay() to
	 * bsd_sell_poll_immed() in the middle of the loop.
	 */
	for (i=0; !error && i < nfps; ++i) {
		if (do_timeout) {
			isc_register_local(&trans_id);
			error = bsd_sel_poll_delay(file_ports[i],
					/* AND give the server a send right */
					file_ports[i],
					credentials_port,
					delayed_reply_port,
					events[i],
					i,
					&revents[i]);
			isc_deregister(interrupt);
			if (error)
				EPRINT(("dopoll: _delay returned 0x%x", error));
		}
		else {
			isc_register_local(&trans_id);
			error = bsd_sel_poll_immed(file_ports[i],
						   credentials_port,
						   events[i],
						   &revents[i]);
			isc_deregister(interrupt);
			if (error)
				EPRINT(("dopoll: _immed returned 0x%x",error));
		}
		if (revents[i] != 0)		/* this file is ready */
			do_timeout = 0;
	}

	/*
	 * If no fp was ready we have to wait for a delayed reply when
	 * one becomes ready.
	 */
	error = MACH_MSG_SUCCESS;
	if (do_timeout) {		/* await delayed replies */
	    while (error == MACH_MSG_SUCCESS) {
		if (timeout == (mach_msg_timeout_t)(-1)) {
			/* no timeout -- wait forever */

			isc_register_local(&trans_id);
			error = mach_msg(&ps_reply.hdr,
				MACH_RCV_MSG | MACH_RCV_INTERRUPT, 0,
					sizeof(ps_reply), delayed_reply_port,
					MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);
			isc_deregister(interrupt);
		} else {
			isc_register_local(&trans_id);
			error = mach_msg(&ps_reply.hdr,
					MACH_RCV_MSG | MACH_RCV_TIMEOUT |
						MACH_RCV_INTERRUPT, 0,
					sizeof(ps_reply), delayed_reply_port,
					timeout, MACH_PORT_NULL);
			isc_deregister(interrupt);
		}

		switch (error) {
		/* if something showed up before the timeout */
		case MACH_MSG_SUCCESS:
			switch (ps_reply.hdr.msgh_id) {
			case POLL_SEL_REPLY_MSG_ID:
				/*
				 * get returned events
				 */
				revents[ps_reply.index] = ps_reply.revents;
				/*
				 * Continue the while loop, getting additional
				 * msgs that are already enqueued but don't
				 * wait for any additional time.
				 */
				timeout = 0;
				break;
			case MACH_NOTIFY_SEND_ONCE:
				/*
				 * Ignore this; the server has discarded the
				 * send-once we gave it because it had nothing
				 * to enqueue on.
				 */
				break;
			default:
				EPRINT(("dopoll: delay msg rcv id 0x%x",
						ps_reply.hdr.msgh_id));
				emul_panic("dopoll: rcv strange msg");
			}
			break;

		/* if specified time ran out */
		case MACH_RCV_TIMED_OUT:
			break;

		/* We've been interrupted */
		case MACH_RCV_INTERRUPTED:
			ret = EINTR;
			break;		

		default:
			EPRINT(("dopoll: delay msg rcv failed err=0x%x",
					error));
			emul_panic("dopoll: fatal error 2");
		}
	    }
	}

	/*
	 * The revents array has its contents and we must cleanup
	 */
	if (delayed_reply_port != MACH_PORT_NULL) {
		/* to destroy this port, decr rcv right */
		error = mach_port_mod_refs(mach_task_self(), delayed_reply_port,				MACH_PORT_RIGHT_RECEIVE, -1);
		if (error != KERN_SUCCESS) {
			EPRINT(("dopoll: delay port deallocate failed err=%d",
					error));
			emul_panic("dopoll: fatal error 3");
		}
	}

	return(ret);
}


int
e_select(serv_port, interrupt, nd, in, ou, ex, tv, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		nd;
	fd_set		*in;
	fd_set		*ou;
	fd_set		*ex;
	timeval_t	*tv;
	int		*rval;
{
	mach_port_t	file_ports[NOFILE];
	short		events[NOFILE], revents[NOFILE];
	int		fd[NOFILE];
	fdt_entry_t	*fdte[NOFILE];
	register int	i;
	register int	nfds;
	int		nfds_out;
	int		ni_size;
	int		error;
	fd_set		zeros;
	fd_set		in_set, ou_set, ex_set;
	mach_msg_timeout_t
			timeout;

	FD_ZERO(&zeros);

	if (nd > NOFILE)
		nd = NOFILE;	/* 'forgiving, if slightly wrong' */
	else if (nd < 0)
		return EINVAL;	/* OK, we got a bug report */
	ni_size = howmany(nd, NFDBITS) * sizeof(fd_mask);

	/* Create temporary versions of all the sets */
	if (!user_bcopy((in) ? (char *)in : (char *)&zeros, (char *)&in_set, 
			(unsigned)ni_size))
		return EFAULT;
	if (!user_bcopy((ou) ? (char *)ou : (char *)&zeros, (char *)&ou_set, 
			(unsigned)ni_size))
		return EFAULT;
	if (!user_bcopy((ex) ? (char *)ex : (char *)&zeros, (char *)&ex_set, 
			(unsigned)ni_size))
		return EFAULT;

	if (tv == NULL)
		/* wait forever for event */
		timeout = (mach_msg_timeout_t)(-1);
	else {
		if (!user_rcheck(tv, sizeof *tv))
			return EFAULT;
                if((tv->tv_sec < 0 ) || ( tv->tv_usec < 0 ))
                        return EINVAL;
		/* if zero, just poll; no waiting */
		timeout = tv->tv_sec*1000 + tv->tv_usec/1000;
	}


	/* Create arguments expected by dopoll() */
	for (i = 0, nfds = 0; i < nd; i++) {
		register int inbit, oubit, exbit;
		inbit = FD_ISSET(i, &in_set);
		oubit = FD_ISSET(i, &ou_set);
		exbit = FD_ISSET(i, &ex_set);
		if (inbit || oubit || exbit) {
			error = fdt_ref_entry(i, &fdte[nfds]);
			if (error) {
				for (i = 0; i < nfds; i++)
					(void) fdt_unref_entry(fdte[i]);
				return(error);
			}
			file_ports[nfds] = fdte[nfds]->fp;
			fd[nfds] = i;
			events[nfds] = 0;
			if (inbit)
				events[nfds] |= POLLNORM;
			if (oubit)
				events[nfds] |= POLLOUT;
			if (exbit)
				events[nfds] |= POLLPRI;
			nfds++;
		}
	}

	/* Do common processing for poll() and select() */
	error = dopoll(serv_port,
			interrupt,
			nfds,
			file_ports,
			events,
			revents,
			timeout);

	if (error) {
		for (i = 0; i < nfds; i++)
			(void) fdt_unref_entry(fdte[i]);
		return (error);
	}

	/* Change the revents from dopoll() to the select() fd_sets */
	FD_ZERO(&in_set);
	FD_ZERO(&ou_set);
	FD_ZERO(&ex_set);
	nfds_out = 0;		/* count # of fd's we return.  Each
					can be counted up to 3 times */
	for (i = 0; i < nfds; i++) {
		(void) fdt_unref_entry(fdte[i]);
		if (revents[i] &  POLLNORM) {
			FD_SET(fd[i], &in_set);
			++nfds_out;
		}
		if (revents[i] &  POLLOUT) {
			FD_SET(fd[i], &ou_set);
			++nfds_out;
		}
		if (revents[i] &  POLLPRI) {
			FD_SET(fd[i], &ex_set);
			++nfds_out;
		}
	}

	/* Copy back all the fd_sets */
	if (in && !user_bcopy((char *)&in_set, (char *)in, (unsigned)ni_size))
		return EFAULT;
	if (ou && !user_bcopy((char *)&ou_set, (char *)ou, (unsigned)ni_size))
		return EFAULT;
	if (ex && !user_bcopy((char *)&ex_set, (char *)ex, (unsigned)ni_size))
		return EFAULT;

	rval[0] = nfds_out;

	return (error);
}


int
e_poll(serv_port, interrupt, fds, maxnfds, timeout, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	struct pollfd	fds[];
	int		maxnfds;
	int		timeout;
	int		*rval;
{
	mach_port_t	file_ports[NOFILE];
	short		events[NOFILE], revents[NOFILE];
	fdt_entry_t	*fdte[NOFILE];
	register int	i;
	register
	struct pollfd	*pfp;
	int		nfds;
	int		nfds_out;
	int		error;

	if (maxnfds > NOFILE)
		maxnfds = NOFILE;	/* 'forgiving, if slightly wrong' */
	else if (maxnfds < 0)
		return EINVAL;		/* OK, we got a bug report */
	if (timeout < -1)
		return(EINVAL);
	if (!user_rwcheck(fds, maxnfds * sizeof *fds))
		return EFAULT;

	/*
	 * Create arguments expected by dopoll().
	 * Note that we get rid of all file descriptors that are negative.
	 * poll() specifies that these are ignored, and we don't want to
	 * bother dopoll() with these.
	 */
	for (i = 0, pfp = &fds[0], nfds = 0; i < maxnfds; i++, pfp++) {
		if (pfp->fd >= 0) {
			error = fdt_ref_entry(pfp->fd, &fdte[nfds]);
			if (error) {
				for (i = 0; i < nfds; i++)
					(void) fdt_unref_entry(fdte[i]);
				return(error);
			}
			file_ports[nfds] = fdte[nfds]->fp;
			events[nfds] = pfp->events;
			nfds++;
		}
	}

	/* Do common processing for poll() and select() */
	error = dopoll(serv_port,
			interrupt,
			nfds,
			file_ports,
			events,
			revents,
			(mach_msg_timeout_t)timeout);

	if (error) {
		for (i = 0; i < nfds; i++)
			(void) fdt_unref_entry(fdte[i]);
		return (error);
	}

	/* Set the output argument to poll() */
	for (i = 0, pfp = &fds[0], nfds = 0, nfds_out = 0; i < maxnfds;
			i++, pfp++) {
		if (pfp->fd < 0)
			pfp->revents = 0;
		else {
			(void) fdt_unref_entry(fdte[nfds]);
			if ((pfp->revents=revents[nfds]) != 0)
				nfds_out++;
			++nfds;
		}
	}
	rval[0] = nfds_out;

	return (error);
}

int
e_socket(proc_port, interrupt, domain, type, protocol, rval)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	int			domain;
	int			type;
	int			protocol;
	int			*rval;
{
	mach_port_t		fp;
	int			fd;
	fdt_entry_t		*fdte;
	int			error;

	if (error = fdt_reserve(0, &fd, &fdte)) 
		return(error);

	emul_blocking();
	error = bsd_socket(proc_port,
			interrupt,
			domain,
			type,
			protocol,
			&fp);
	emul_unblocking();
	if (error) {
		fdt_cancel(fd, fdte);
		return(error);
	}

	fdte->fp = fp;		/* no need for locking - we have sole access */
#ifdef	TNC
	if (domain == AF_UNIX) {
		fdte->notify_on_migrate = 1;
		if (type == SOCK_DGRAM)
			fdte->notify_asap = 1;
	}
#endif
	fdt_install(fd, fdte);	

	*rval = fd;
	return (error);
}

int
e_socketpair(proc_port, interrupt, domain, type, protocol, rsv)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	int			domain;
	int			type;
	int			protocol;
	int			*rsv;
{
	mach_port_t		fp1, fp2;
	int			fd1, fd2;
	fdt_entry_t		*fdte1, *fdte2;
	int			error;

	if (!user_rwcheck(rsv, sizeof(int)*2))
	    return EFAULT;

	if (error = fdt_reserve(0, &fd1, &fdte1)) 
		return(error);

	if (error = fdt_reserve(0, &fd2, &fdte2)) {
		fdt_cancel(fd1, fdte1);
		return(error);
	}

	emul_blocking();
	error = bsd_socketpair(proc_port,
			interrupt,
			domain,
			type,
			protocol,
			&fp1,
			&fp2);
	emul_unblocking();
	if (error) {
		fdt_cancel(fd1, fdte1);
		fdt_cancel(fd2, fdte2);
		return(error);
	}

	fdte1->fp = fp1;	/* no need for locking - we have sole access */
	fdte2->fp = fp2;
#ifdef	TNC
	if (domain == AF_UNIX) {
		fdte1->notify_on_migrate = 1;
		fdte2->notify_on_migrate = 1;
		if (type == SOCK_DGRAM) {
			fdte1->notify_asap = 1;
			fdte2->notify_asap = 1;
		}
	}
#endif
	fdt_install(fd1, fdte1);	
	fdt_install(fd2, fdte2);	

	rsv[0] = fd1;
	rsv[1] = fd2;
	return (error);
}

int
e_bind(proc_port, interrupt, s, name, namelen, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		s;
	caddr_t		name;
	int		namelen;
	int		*rval;		/* OUT */
{
	return e_bindconn(SYS_bind,
		proc_port, interrupt, s, name, namelen, rval);
}

int
e_connect(proc_port, interrupt, s, name, namelen, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		s;
	caddr_t		name;
	int		namelen;
	int		*rval;		/* OUT */
{
	return e_bindconn(SYS_connect,
		proc_port, interrupt, s, name, namelen, rval);
}


/*
 * We may need to access the first byte of a sun_path in a sockaddr_un
 */
#ifndef i860
#define SOCKADDR_UN_MINLEN (offsetof(struct sockaddr_un, sun_path) + 1)
#else
#define SOCKADDR_UN_MINLEN (2*sizeof(u_char)+1)
#endif

/*
 * Common for e_bind and e_connect
 *	we either call common fsvr_uds_bindconn
 *	or the original bsd_bind or bsd_connect
 */
int
e_bindconn(syscode, proc_port, interrupt, s, name, namelen, rval)
	int		syscode;
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		s;
	caddr_t		name;
	int		namelen;
	int		*rval;		/* OUT */
{
	mach_port_t	start_port;
	transaction_id_t trans_id;
	fdt_entry_t     *fdte;
	int		error;
	struct sockaddr *sa = (struct sockaddr *)name;
	register int	family = AF_UNSPEC;
	extern int	bsd_bind();
	extern int	bsd_connect();

	if (!user_rcheck(name, MAX(namelen, SOCKADDR_UN_MINLEN)))
		return EFAULT;

	if (namelen <= 0)
	        return EINVAL;

	if (error = fdt_ref_entry(s, &fdte)) {
		return(error);
	}

#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	/*
	 * MUST be careful about using backwards compatible socket names
	 */
	if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
		family = sa->sa_len;
	else
		family = sa->sa_family;
#else
	family = sa->sa_family;
#endif

	/*
	 * If UNIX domain socket in address, set up to marshall everything
	 * it might need to do a pathname lookup, and call special stub.
	 *
	 * Either way, the next node will be the one where fp lives.
	 */
	if (family == AF_UNIX) {
		struct sockaddr_un *soun = (struct sockaddr_un *)sa;

		if (*soun->sun_path == '\0' && !nullcompat) {
			(void) fdt_unref_entry(fdte);
			return(ENOENT);	
		}

		start_port = (*soun->sun_path == '/')
				? rootdir_port
				: currentdir_port;
		isc_register(fdte->fp, &trans_id);
		error = fsvr_uds_bindconn(
				fdte->fp,
				start_port,
				credentials_port,
				trans_id,
				rootdir_port,
				syscode,
				name,
				namelen);
		isc_deregister(interrupt);
	} else {
		isc_register(fdte->fp, &trans_id);
		error = (syscode == SYS_bind
			  ? bsd_bind
			  : bsd_connect)(
				fdte->fp,
				credentials_port,
				trans_id,
				name,
				namelen);
		isc_deregister(interrupt);
	}

	(void) fdt_unref_entry(fdte);
	return error;
}


int
e_sendto(proc_port, interrupt, s, data, count, flags, to, tolen, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		s;
	char		*data;
	unsigned int	count;
	int		flags;
	caddr_t		to;
	int		tolen;
	int		*rval;		/* OUT */
{
	mach_port_t	start_port;
	transaction_id_t trans_id;
	fdt_entry_t     *fdte;
	int		error;
	struct sockaddr *sa = (struct sockaddr *)to;
	register int	family = AF_UNSPEC;
	extern int	bsd_sendto_short();
	extern int	bsd_sendto_long();
	extern int	fsvr_uds_sendto_short();
	extern int	fsvr_uds_sendto_long();

	if (!user_rcheck(data, count))
		return EFAULT;
	if (to && !user_rcheck(to, MAX(tolen, SOCKADDR_UN_MINLEN)))
		return EFAULT;

	if (error = fdt_ref_entry(s, &fdte)) {
		return(error);
	}

#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	if (sa) {
		/*
		 * MUST be careful about using backwards compatible socket names
		 */
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
			family = sa->sa_len;
		else
			family = sa->sa_family;
	}
#else
	if (sa)
		family = sa->sa_family;
#endif

	/*
	 * If UNIX domain socket in address, set up to marshall everything
	 * it might need to do a pathname lookup, and call special sendto.
	 *
	 * Either way, the next node will be the one where fp lives.
	 */
	if (sa && family == AF_UNIX) {
		struct sockaddr_un *soun = (struct sockaddr_un *)sa;

		if (*soun->sun_path == '\0' && !nullcompat) {
			(void) fdt_unref_entry(fdte);
			return(ENOENT);	
		}
#ifdef	TNC
		if (fdte->notify_asap && ! fdte->was_notified)
			report_migration(fdte);
#endif
		start_port = (*soun->sun_path == '/')
				? rootdir_port
				: currentdir_port;
		isc_register(fdte->fp, &trans_id);
		error = ((count <= SMALL_ARRAY_LIMIT)
			  ? fsvr_uds_sendto_short
			  : fsvr_uds_sendto_long)(
				fdte->fp,
				start_port,
				credentials_port,
				trans_id,
				rootdir_port,
				flags,
				to,
				tolen,
				data,
				count,
				&rval[0]);
		isc_deregister(interrupt);
	} else {
		isc_register(fdte->fp, &trans_id);
		error = ((count <= SMALL_ARRAY_LIMIT)
			  ? bsd_sendto_short
			  : bsd_sendto_long)(
				fdte->fp,
				credentials_port,
				trans_id,
				flags,
				to,
				tolen,
				data,
				count,
				&rval[0]);
		isc_deregister(interrupt);
	}

	if (error == EPIPE)
		send_sig(SIGPIPE, interrupt);

	(void) fdt_unref_entry(fdte);
	return error;
}


#ifdef	COMPAT_43
int
e_osendmsg(serv_port, interrupt, fileno, msg, flags, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fileno;
	caddr_t		msg;
	int		flags;
	int		*rval;
{
	return e_dosendmsg(SYS_osendmsg,
			serv_port, interrupt, fileno, msg, flags, rval);
}
#endif

int
#ifdef	COMPAT_43
e_nsendmsg(serv_port, interrupt, fileno, msg, flags, rval)
#else
e_sendmsg(serv_port, interrupt, fileno, msg, flags, rval)
#endif
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fileno;
	caddr_t		msg;
	int		flags;
	int		*rval;
{
	return e_dosendmsg(SYS_sendmsg,
			serv_port, interrupt, fileno, msg, flags, rval);
}


e_dosendmsg(syscode, proc_port, interrupt, s, msg, flags, rval)
	int		syscode;
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		s;
	struct msghdr	*msg;
	int		flags;
	int		*rval;
{
	int		uarg[3];
	mach_port_t	start_port;
	transaction_id_t trans_id;
	fdt_entry_t     *fdte;
	int		error;
	struct sockaddr *sa;
	register int	family = AF_UNSPEC;

	if (!user_rcheck(msg, sizeof *msg))
		return EFAULT;
	sa = (struct sockaddr *)msg->msg_name;
	if (sa && !user_rcheck(sa, MAX(msg->msg_namelen, SOCKADDR_UN_MINLEN)))
		return EFAULT;

#if defined(COMPAT_43) && BYTE_ORDER != BIG_ENDIAN
	if (sa) {
		/*
		 * MUST be careful about using backwards compatible socket names
		 */
		if (sa->sa_family == 0 && sa->sa_len < AF_MAX)
			family = sa->sa_len;
		else
			family = sa->sa_family;
	}
#else
	if (sa)
		family = sa->sa_family;
#endif

	/*
	 * If UNIX domain socket in address, set up to marshall everything
	 * it might need to do a pathname lookup, and call special sendmsg.
	 *
	 * Either way, the next node will be the one where fp lives.
	 */
	if (sa && family == AF_UNIX) {
		struct sockaddr_un *soun = (struct sockaddr_un *)sa;

		if (*soun->sun_path == '\0' && !nullcompat)
			return(ENOENT);	

		if (error = fdt_ref_entry(s, &fdte)) {
			return(error);
		}
#ifdef	TNC
		if (fdte->notify_asap && ! fdte->was_notified)
			report_migration(fdte);
#endif
		start_port = (*soun->sun_path == '/')
				? rootdir_port
				: currentdir_port;
		isc_register(fdte->fp, &trans_id);
		error = fsvr_uds_sendmsg(fdte->fp,
				start_port,
				credentials_port,
				trans_id,
				rootdir_port,
				syscode,	/* SYS_{o,}sendmsg */
				flags,
				msg->msg_name,
				msg->msg_namelen,
				(int)msg,
				&rval[0]);
		isc_deregister(interrupt);
		(void) fdt_unref_entry(fdte);
	} else {
		uarg[0] = s;
		uarg[1] = (int) msg;
		uarg[2] = flags;
		error = emul_fs_generic(proc_port, interrupt, syscode,
					uarg, rval);
	}

	if (error == EPIPE)
		send_sig(SIGPIPE, interrupt);
	return error;
}

#ifdef	COMPAT_43
int
e_oaccept(serv_port, interrupt, s, name, anamelen, rvalp)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			s;
	char *			name;
	int			*anamelen;
	int			*rvalp;
{
	mach_port_t		newfp;
	int			newfd;
	int			namelen = sizeof(sockarg_t);
	sockarg_t		out_name;
	int			error;
	fdt_entry_t		*fdte, *newfdte;
	transaction_id_t	trans_id;

	if (anamelen != (int *)NULL) {
	    if (!user_rwcheck(anamelen, sizeof(int)))
	        return EFAULT;
	}
	if (name != (char *)NULL) {
	    if (!user_rwcheck(name, *anamelen))
	        return EFAULT;
	}
	if (error = fdt_ref_entry(s, &fdte))
		return(error);

	if (error = fdt_reserve(0, &newfd, &newfdte)) {
		(void) fdt_unref_entry(fdte);
		return(error);
	}

	isc_register(fdte->fp, &trans_id);
	error = bsd_oaccept(fdte->fp,
			credentials_port,
			trans_id,
			out_name,
			&namelen,
			&newfp);
	isc_deregister(interrupt);
	if (error) {
		(void) fdt_unref_entry(fdte);
		fdt_cancel(newfd, newfdte);
		return(error);
	}

	/* NOTE: We may return EFAULT even though the fd is installed */
	if (name) {
		int user_namelen;
		if (!user_bcopy(anamelen, &user_namelen, sizeof user_namelen))
			error = EFAULT;
		else {
			if (namelen > user_namelen)
				namelen = user_namelen;
			if (!user_bcopy(out_name, name, namelen))
				error = EFAULT;
			/* We know that *anamelen is safe */
			*anamelen = namelen;
		}
	}

	newfdte->fp = newfp;	/* no need for locking - we have sole access */
#ifdef	TNC
	if (fdte->notify_on_migrate) {
		/*
		 * the listening socket notifies on migrate,
		 * so the accept socket should also.
		 */
		newfdte->notify_on_migrate = 1;
		if (edebug & EDEBUG_TNC_UIPC)
			EPRINT(("e_oaccept: setting notify_on_migrate"));
	}
#endif	/* TNC */
	fdt_install(newfd, newfdte);	
	(void) fdt_unref_entry(fdte);
	*rvalp = newfd;
	return (error);
}
#endif	/* COMPAT_43 */

#ifdef	COMPAT_43
int
e_naccept(serv_port, interrupt, s, name, anamelen, rvalp)
#else	/* !COMPAT_43 */
int
e_accept(serv_port, interrupt, s, name, anamelen, rvalp)
#endif	/* !COMPAT_43 */
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	int			s;
	char *			name;
	int			*anamelen;
	int			*rvalp;
{
	mach_port_t		newfp;
	int			newfd;
	int			namelen = sizeof(sockarg_t);
	sockarg_t		out_name;
	int			error;
	fdt_entry_t		*fdte, *newfdte;
	transaction_id_t	trans_id;

	if (anamelen != (int *)NULL) {
	    if (!user_rwcheck(anamelen, sizeof(int)))
	        return EFAULT;
	}
	if (name != (char *)NULL) {
	    if (!user_rwcheck(name, *anamelen))
	        return EFAULT;
	}
	if (error = fdt_ref_entry(s, &fdte))
		return(error);

	if (error = fdt_reserve(0, &newfd, &newfdte)) {
		(void) fdt_unref_entry(fdte);
		return(error);
	}

	isc_register(fdte->fp, &trans_id);
	error = bsd_accept(fdte->fp,
			credentials_port,
			trans_id,
			out_name,
			&namelen,
			&newfp);
	isc_deregister(interrupt);
	if (error) {
		(void) fdt_unref_entry(fdte);
		fdt_cancel(newfd, newfdte);
		return(error);
	}

	/* NOTE: We may return EFAULT even though the fd is installed */
	if (name) {
		int user_namelen;
		if (!user_bcopy(anamelen, &user_namelen, sizeof user_namelen))
			error = EFAULT;
		else {
			if (namelen > user_namelen)
				namelen = user_namelen;
			if (!user_bcopy(out_name, name, namelen))
				error = EFAULT;
			/* We know that *anamelen is safe */
			*anamelen = namelen;
		}
	}

	newfdte->fp = newfp;	/* no need for locking - we have sole access */
#ifdef	TNC
	if (fdte->notify_on_migrate) {
		/*
		 * the listening socket notifies on migrate,
		 * so the accept socket should also.
		 */
		newfdte->notify_on_migrate = 1;
		if (edebug & EDEBUG_TNC_UIPC)
			EPRINT(("e_accept: setting notify_on_migrate"));
	}
#endif	/* TNC */
	fdt_install(newfd, newfdte);	
	(void) fdt_unref_entry(fdte);
	*rvalp = newfd;
	return (error);
}

int
e_setsockopt(serv_port, interrupt, s, level, name, val, valsize)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		s;
	int		level;
	int		name;
	char *		val;
	int		valsize;
{
	int		error;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (val && !user_rcheck(val, valsize))
		return EFAULT;
	if (error = fdt_ref_entry(s, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	error = bsd_setsockopt(fdte->fp,
			credentials_port,
			trans_id,
			level,
			name,
			val,
			(val) ? valsize : 0);
	isc_deregister(interrupt);

	(void) fdt_unref_entry(fdte);
	return(error);
}

int
e_getsockopt(serv_port, interrupt, s, level, name, val, avalsize)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		s;
	int		level;
	int		name;
	char *		val;
	int		*avalsize;
{
	register int	error;
	int		valsize = sizeof(sockarg_t);
	sockarg_t	val_buf;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (!user_rwcheck(avalsize, sizeof(int)))
		return EFAULT;

	if (error = fdt_ref_entry(s, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	error = bsd_getsockopt(fdte->fp,
			credentials_port,
			trans_id,
			level,
			name,
			val_buf,
			&valsize);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);
	if (error)
		return (error);

	if (val) {
		int user_valsize;
		if (!user_bcopy(avalsize, &user_valsize, (sizeof user_valsize)))
			return EFAULT;
		if (valsize > user_valsize)
			valsize = user_valsize;
		if (!user_bcopy(val_buf, val, valsize))
			return EFAULT;
		/* We know that avalsize is safe */
		*avalsize = valsize;
	}

	return (error);
}

#ifdef	COMPAT_43
int
e_ogetsockname(serv_port, interrupt, s, asa, alen)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		s;
	char *		asa;
	int		*alen;
{
	register int	error;
	int		buflen = sizeof(sockarg_t);
	sockarg_t	asa_buf;
	int		user_len;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (error = fdt_ref_entry(s, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	error = bsd_ogetsockname(fdte->fp,
			credentials_port,
			trans_id,
			asa_buf,
			&buflen);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);
	if (error)
		return (error);

	if (!user_bcopy(alen, &user_len, (sizeof user_len)))
		return EFAULT;
	if (buflen > user_len)
		buflen = user_len;
	if (!user_bcopy(asa_buf, asa, buflen))
		return EFAULT;
	if (!user_bcopy(alen, &buflen, (sizeof buflen)))
		return EFAULT;

	return (error);
}
#endif	/* COMPAT_43 */

#ifdef	COMPAT_43
int
e_ngetsockname(serv_port, interrupt, s, asa, alen)
#else	/* !COMPAT_43 */
int
e_getsockname(serv_port, interrupt, s, asa, alen)
#endif	/* !COMPAT_43 */
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		s;
	char *		asa;
	int		*alen;
{
	register int	error;
	int		buflen = sizeof(sockarg_t);
	sockarg_t	asa_buf;
	int		user_len;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (error = fdt_ref_entry(s, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	error = bsd_getsockname(fdte->fp,
			credentials_port,
			trans_id,
			asa_buf,
			&buflen);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);
	if (error) {
		return (error);
	}

	if (!user_bcopy(alen, &user_len, (sizeof user_len)))
		return EFAULT;
	if (buflen > user_len)
		buflen = user_len;
	if (!user_bcopy(asa_buf, asa, buflen))
		return EFAULT;
	if (!user_bcopy(alen, &buflen, (sizeof buflen)))
		return EFAULT;

	return (error);
}

#ifdef	COMPAT_43
int
e_osend(serv_port, interrupt, fileno, data, count, flags, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fileno;
	char		*data;
	unsigned int	count;
	int		flags;
	int		*rval;
{
	extern int		bsd_osend_short();
	extern int		bsd_osend_long();
	int			error;
	fdt_entry_t		*fdte;
	transaction_id_t	trans_id;

	if (!user_rcheck(data, count))
		return EFAULT;
	if (error = fdt_ref_entry(fileno, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_asap && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	error = ((count <= SMALL_ARRAY_LIMIT) ? bsd_osend_short
					      : bsd_osend_long
		)(fdte->fp,
		  credentials_port,
		  trans_id,
		  flags,
		  data,
		  count,
		  &rval[0]);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);

	if (error == EPIPE)
		send_sig(SIGPIPE, interrupt);

	return(error);
}
#endif	/* COMPAT_43 */

#ifdef	COMPAT_43
int
e_orecv(serv_port, interrupt, fileno, data, len, flags, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fileno;
	char		*data;
	int		len;
	int		flags;
	int		*rval;
{
	register int		error;
	unsigned int		data_count;
	fdt_entry_t		*fdte;
	transaction_id_t	trans_id;

	data_count = len;

	if (!user_rwcheck(data, len))
		return EFAULT;
	if (error = fdt_ref_entry(fileno, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_on_migrate && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	if (len <= SMALL_ARRAY_LIMIT) {
		error = bsd_orecv_short(fdte->fp,
				credentials_port,
				trans_id,
				flags,
				len,
				data,
				&data_count);
		isc_deregister(interrupt);
	} else {
		char *data_addr;

		error = bsd_orecv_long(fdte->fp,
				credentials_port,
				trans_id,
				flags,
				len,
				&data_addr,
				&data_count);
		isc_deregister(interrupt);
		if (error == 0) {
			if (!user_bcopy(data_addr, data, data_count))
				error = EFAULT;
			(void) vm_deallocate(mach_task_self(),
					(vm_offset_t)data_addr,
					data_count);
		}
	}
	(void) fdt_unref_entry(fdte);

	if (error == 0)
		rval[0] = data_count;

	return (error);
}
#endif	/* COMPAT_43 */


#ifdef	COMPAT_43
int
e_orecvfrom(serv_port, interrupt,
	   fileno, data, len, flags, from, fromlenaddr, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fileno;
	char		*data;
	int		len;
	int		flags;
	char		*from;
	int		*fromlenaddr;
	int		*rval;
{
	/*
	 * We receive the address into a temporary buffer,
	 * since the MiG interface always returns the full
	 * amount.
	 */
	register int		error;
	unsigned int		data_count;
	sockarg_t		from_buf;
	int			from_count;
	fdt_entry_t		*fdte;
	transaction_id_t	trans_id;

	data_count = len;
	if (fromlenaddr) {
		if (!user_bcopy(fromlenaddr, &from_count, sizeof from_count))
			return EFAULT;
	} else {
		from_count = sizeof(from_buf);
	}

	if (!user_rwcheck(data, len))
		return EFAULT;
	if (error = fdt_ref_entry(fileno, &fdte))
		return(error);

#ifdef	TNC
	if (fdte->notify_on_migrate && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	if (len <= SMALL_ARRAY_LIMIT) {
		error = bsd_orecvfrom_short(fdte->fp,
				credentials_port,
				trans_id,
				flags,
				len,
				from_buf,
				&from_count,
				data,
				&data_count);
		isc_deregister(interrupt);
	} else {
		char *data_addr;

		error = bsd_orecvfrom_long(fdte->fp,
				credentials_port,
				trans_id,
				flags,
				len,
				from_buf,
				&from_count,
				&data_addr,
				&data_count);
		isc_deregister(interrupt);
		if (error == 0) {
			if (!user_bcopy(data_addr, data, data_count))
				error = EFAULT;
			(void) vm_deallocate(mach_task_self(),
					(vm_offset_t)data_addr,
					data_count);
		}
	}
	(void) fdt_unref_entry(fdte);

	if (error == 0) {
		rval[0] = data_count;
		if (from) {
			if (!user_bcopy(from_buf, from, from_count))
				return EFAULT;
			if (fromlenaddr) {
				/* We know that fromlenaddr is safe */
				*fromlenaddr = from_count;
			}
		}
	}

	return (error);
}
#endif	/* COMPAT_43 */


#ifdef	COMPAT_43
int
e_nrecvfrom(serv_port, interrupt,
		fileno, data, len, flags, from, fromlenaddr, rval)
#else	/* !COMPAT_43 */
int
e_recvfrom(serv_port, interrupt,
		fileno, data, len, flags, from, fromlenaddr, rval)
#endif	/* !COMPAT_43 */
	mach_port_t	serv_port;
	boolean_t       *interrupt;
	int		fileno;
	char		*data;
	int		len;
	int		flags;
	char		*from;
	int		*fromlenaddr;
	int		*rval;
{
	/*
	 * We receive the address into a temporary buffer,
	 * since the MiG interface always returns the full
	 * amount.
	 */
	register int		error;
	unsigned int		data_count;
	sockarg_t		from_buf;
	int			from_count;
	fdt_entry_t		*fdte;
	transaction_id_t	trans_id;

	if (!user_rwcheck(data, len))
		return EFAULT;
	if (error = fdt_ref_entry(fileno, &fdte))
		return(error);

	data_count = len;
	if (fromlenaddr) {
		if (!user_bcopy(fromlenaddr, &from_count, sizeof from_count))
			return EFAULT;
	} else {
		from_count = sizeof(from_buf);
	}

#ifdef	TNC
	if (fdte->notify_on_migrate && ! fdte->was_notified)
		report_migration(fdte);
#endif
	isc_register(fdte->fp, &trans_id);
	if (len <= SMALL_ARRAY_LIMIT) {
		error = bsd_recvfrom_short(fdte->fp,
			credentials_port,
			trans_id,
			flags,
			len,
			from_buf,
			&from_count,
			data,
			&data_count);
		isc_deregister(interrupt);
	}
	else {
		char *		data_addr;

		error = bsd_recvfrom_long(fdte->fp,
				credentials_port,
				trans_id,
				flags,
				len,
				from_buf,
				&from_count,
				&data_addr,
				&data_count);
		isc_deregister(interrupt);
		if (error == 0) {
			if (!user_bcopy(data_addr, data, data_count))
				error = EFAULT;
			(void) vm_deallocate(mach_task_self(),
					(vm_offset_t)data_addr,
					data_count);
		}
	}
	(void) fdt_unref_entry(fdte);

	if (error == 0) {
		rval[0] = data_count;
		if (from) {
			if (!user_bcopy(from_buf, from, from_count))
				return EFAULT;
			if (fromlenaddr) {
				/* We know that fromlenaddr is safe */
				*fromlenaddr = from_count;
			}
		}
	}

	return (error);
}

#ifdef COMPAT_43
int
e_ogetpeername(serv_port, interrupt, fdes, asa, alen)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fdes;
	char *		asa;
	int		*alen;
{
	register int	error;
	int		buflen = sizeof(sockarg_t);
	sockarg_t	asa_buf;
	int		user_len;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = bsd_ogetpeername(fdte->fp,
			credentials_port,
			trans_id,
			asa_buf,
			&buflen);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);
	if (error)
		return (error);

	if (!user_bcopy(alen, &user_len, (sizeof user_len)))
		return EFAULT;
	if (buflen > user_len)
		buflen = user_len;
	if (!user_bcopy(asa_buf, asa, buflen))
		return EFAULT;
	if (!user_bcopy(alen, &buflen, (sizeof buflen)))
		return EFAULT;

	return (error);
}
#endif	COMPAT_43


#ifdef	COMPAT_43
int
e_ngetpeername(serv_port, interrupt, fdes, asa, alen)
#else	/* !COMPAT_43 */
int
e_getpeername(serv_port, interrupt, fdes, asa, alen)
#endif	/* !COMPAT_43 */
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		fdes;
	char *		asa;
	int		*alen;
{
	register int	error;
	int		buflen = sizeof(sockarg_t);
	sockarg_t	asa_buf;
	int		user_len;
	fdt_entry_t	*fdte;
	transaction_id_t	trans_id;

	if (error = fdt_ref_entry(fdes, &fdte))
		return(error);

	isc_register(fdte->fp, &trans_id);
	error = bsd_getpeername(fdte->fp,
			credentials_port,
			trans_id,
			asa_buf,
			&buflen);
	isc_deregister(interrupt);
	(void) fdt_unref_entry(fdte);
	if (error)
		return (error);

	if (!user_bcopy(alen, &user_len, (sizeof user_len)))
		return EFAULT;
	if (buflen > user_len)
		buflen = user_len;
	if (!user_bcopy(asa_buf, asa, buflen))
		return EFAULT;
	if (!user_bcopy(alen, &buflen, (sizeof buflen)))
		return EFAULT;

	return (error);
}


int
#if	MACH_AFS
e_afs_xioctl(serv_port, interrupt, fdes, cmd, arg, rvalp)
#else	/* MACH_AFS */
e_ioctl(serv_port, interrupt, fdes, cmd, arg, rvalp)
#endif	MACH_AFS
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		fdes;
	int		cmd;
	int		arg;
	int		*rvalp;		/* OUT */
{
	int		uarg[3];

	/*
	 * We can handle some commands here.
	 */

	switch (cmd) {

	case FIOCLEX:
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		fdt[fdes].cloexec = TRUE;
		FDT_UNLOCK();
		break;

	case FIONCLEX:
		FDT_LOCK();
		if ((unsigned)fdes >= NOFILE || fdt[fdes].fdte == FD_EMPTY || 
		    fdt[fdes].fdte == FD_RESERVED) {
			FDT_UNLOCK();
			return(EBADF);		/* slot empty or reserved */
		}
		fdt[fdes].cloexec = FALSE;
		FDT_UNLOCK();
		break;

	case FIODEVPORT:
		uarg[0] = fdes;
		uarg[1] = cmd;
		uarg[2] = arg;
		return(emul_fs_get_device_port(	serv_port, interrupt,
						SYS_ioctl, uarg, rvalp));

	default:
		uarg[0] = fdes;
		uarg[1] = cmd;
		uarg[2] = arg;
		return (emul_fs_generic(serv_port, interrupt, SYS_ioctl,
				uarg, rvalp));
	}

	return(ESUCCESS);
}

int
e_pipe(proc_port, interrupt, rval)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	int			*rval;
{
	mach_port_t		fp1, fp2;
	int			fd1, fd2;
	fdt_entry_t		*fdte1, *fdte2;
	int			error;

	if (error = fdt_reserve(0, &fd1, &fdte1)) 
		return(error);

	if (error = fdt_reserve(0, &fd2, &fdte2)) {
		fdt_cancel(fd1, fdte1);
		return(error);
	}

	emul_blocking();
	error = bsd_pipe(proc_port,
			interrupt,
			&fp1,
			&fp2);
	emul_unblocking();
	if (error) {
		fdt_cancel(fd1, fdte1);
		fdt_cancel(fd2, fdte2);
		return(error);
	}

	fdte1->fp = fp1;	/* no need for locking - we have sole access */
	fdte2->fp = fp2;
#ifdef	TNC
	fdte1->notify_on_migrate = 1;
	fdte2->notify_on_migrate = 1;
#endif
	fdt_install(fd1, fdte1);	
	fdt_install(fd2, fdte2);	

	rval[0] = fd1;
	rval[1] = fd2;
	return (error);
}

int
e_mmap(proc_port, interrupt, addr, len, prot, flags, fdes, off, rvalp)
        mach_port_t  proc_port;
        boolean_t    *interrupt;
        caddr_t      addr;
        size_t       len;
        int          prot;
        int          flags;
        int          fdes;
        off_t        off;
        caddr_t      *rvalp;
{
        int          error = 0;
	fdt_entry_t  *fdte;
        mach_port_t  fport;

        if ((flags & MAP_TYPE) == MAP_ANON) {
                if (fdes != -1) {
                        *rvalp = (caddr_t)-1;
                        return(EBADF);
                }
                fport = MACH_PORT_NULL;
        } else {
                if (error = fdt_ref_entry(fdes, &fdte))
			return(error);
                fport = fdte->fp;
        }
	emul_blocking();
        error = bsd_mmap(proc_port, fport, addr, len, prot, flags, off, 
                         rvalp, interrupt);
	emul_unblocking();

        if ((flags & MAP_TYPE) != MAP_ANON)
		(void) fdt_unref_entry(fdte);

        return(error);
}


int 
e_nfssvc(proc_port, interrupt, sock, mask, match, rvalp)
        mach_port_t  proc_port;
        boolean_t    *interrupt;
        int          sock;
        int          mask;
        int          match;
        caddr_t      *rvalp;
{
#ifdef NFS
        int          error;
	fdt_entry_t  *fdte;
        transaction_id_t    trans_id;

        if (error = fdt_ref_entry(sock, &fdte)) {
                return(error);
        }
        
        isc_register(fdte->fp, &trans_id);
        error = fsvr_nfssvc(fdte->fp, credentials_port, trans_id, mask, 
                            match, (int *)rvalp);
        isc_deregister(interrupt);

        (void) fdt_unref_entry(fdte);

        return(error);
#else
	return(EINVAL);
#endif /* NFS */
}


/*
 * Insert port rights associated with file descriptors into a task.
 * Called at fork time with FDT_LOCK held.
 */
void
fdt_fork_insert_rights(task)
	task_t			task;
{
	fdt_entry_t		*fdte;
	int			fdes, nfds, i;
	int			error;
	struct {
		fdt_entry_t	*fdte;
		mach_port_t	fp;
#ifdef 	MAPPED_FILES
		mach_port_t	mem_obj;
#endif
	} fork_info[NOFILE];

	/*
	 * Because multiple fd slots can refer to the same fd entry,
	 * we're careful to ensure that a right is inserted into
	 * the child only once.
	 */
	nfds = 0;  		/* index to fork_info array */
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		/* don't insert a right into the new task more than once */
		if (fdte->referenced)
			continue;
		fork_info[nfds].fdte = fdte;
#ifdef	MAPPED_FILES
		fork_info[nfds].mem_obj = fdte->mem_obj;
#endif
		fork_info[nfds++].fp = fdte->fp;
		fdte->referenced = 1;
	}

	/*
	 * Clear the 'referenced' flags, and then insert all the
	 * port rights.
	 */
	for (i=0; i<nfds; ++i) 
		fork_info[i].fdte->referenced = 0;

	for (i=0; i<nfds; ++i) { 
		error = mach_port_insert_right(task, fork_info[i].fp, 
			            fork_info[i].fp, MACH_MSG_TYPE_COPY_SEND);
        	if (error != KERN_SUCCESS)
			EPRINT(("fork_insert_rights fp failure=%d", error));
#ifdef	MAPPED_FILES
		if (fork_info[i].mem_obj != MACH_PORT_NULL) {
			error = mach_port_insert_right(task, 
						       fork_info[i].mem_obj, 
						       fork_info[i].mem_obj, 
						       MACH_MSG_TYPE_COPY_SEND);
			if (error != KERN_SUCCESS)
			        EPRINT(("fork_insert_rights mo failure=%d", 
					error));
		}
#endif		
		/*
		 * send a message to the server to get another reference
		 * on the file structure
		 */
		error = fsvr_file_ref(fork_info[i].fp, 1);
		if (error) {
			EPRINT(("fsvr_file_ref failed, fp=%x fdte->refcnt=%d\n",
				fork_info[i].fp, fork_info[i].fdte->refcnt));
			EPRINT(("fsvr_file_ref failure: error=0x%x", error));
			emul_panic("fork_insert_rights: fsvr_file_ref failed");
		}
#ifdef	PFS
		if (fork_info[i].fdte->pfs_fd) {
			pfs_fd_t	*pfs_fd;
			int		sfd;
			mach_port_t	sfp;

			pfs_fd = fork_info[i].fdte->pfs_fd;

			/*
			 * Need to get the rights for each of the stripefiles,
			 * and get another reference on each file structure.
			 */
			for (sfd = 0; sfd < pfs_fd->p_stripe_factor; sfd++) {
				sfp = pfs_fd->p_stripe_fdt[sfd].s_fp; 

				error = mach_port_insert_right(
						task, sfp, sfp,
						MACH_MSG_TYPE_COPY_SEND);
       			 	if (error != KERN_SUCCESS)
				   EPRINT(("fork_insert_rights sfp failure=%d",
					   error));

				error = fsvr_file_ref(sfp, 1);
				if (error) {
				   EPRINT(("fsvr_file_ref failed, sfp=%x\n", 
					   sfp));
				   EPRINT(("fsvr_file_ref failure: error=0x%x",
					   error));
				   emul_panic("fork_insert_rights: fsvr_file_ref failed");
				}
			}
			
		}
#endif
	}
}

#ifdef	TNC
/*
 * Get port rights associated with file descriptors.
 * Called at fork time with FDT_LOCK held.
 */
int
fdt_get_rights(port_table, table_size)
	mach_port_t		*port_table;
	unsigned int		table_size;
{
	fdt_entry_t		*fdte;
	int			fdes, nfds, nports, i;
	struct {
		fdt_entry_t	*fdte;
		mach_port_t	fp;
#ifdef 	MAPPED_FILES
		mach_port_t	mem_obj;
#endif
	} fork_info[NOFILE];

	/*
	 * Because multiple fd slots can refer to the same fd entry,
	 * we're careful to ensure that a right is inserted into
	 * the child only once.
	 */
	nfds = 0;  		/* index to fork_info array */
	nports = 0;
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		/* don't insert a right into the new task more than once */
		if (fdte->referenced)
			continue;
		nports++;
		fork_info[nfds].fdte = fdte;
#ifdef	MAPPED_FILES
		fork_info[nfds].mem_obj = fdte->mem_obj;
		if (fdte->mem_obj != MACH_PORT_NULL)
			nports++;
#endif
#ifdef	PFS
		if (fdte->pfs_fd) {
			int		sfd;
			pfs_fd_t	*pfs_fd = fdte->pfs_fd;
			for (sfd = 0; sfd < pfs_fd->p_stripe_factor; sfd++)
				nports++;
		}
#endif
		fork_info[nfds++].fp = fdte->fp;
		fdte->referenced = 1;
	}

	/*
	 * Clear the 'referenced' flags, and then insert all the
	 * port rights.
	 */
	for (i=0; i<nfds; ++i) 
		fork_info[i].fdte->referenced = 0;

	/*
	 * If no port_table was provided, simply return the size required.
	 */
	if (!port_table) {
		return(nports);
	}

	/*
	 * If the port_table provided is too small, return error.
	 */
	if (nports > table_size) {
		return(-1);
	}

	/*
	 * Load all ports into the table.
	 */
	nports = 0;		
	for (i=0; i<nfds; ++i) { 
		port_table[nports++] = fork_info[i].fp; 
#ifdef	MAPPED_FILES
		if (fork_info[i].mem_obj != MACH_PORT_NULL) {
			port_table[nports++] = fork_info[i].mem_obj; 
		}
#endif		
#ifdef	PFS
		if (fork_info[i].fdte->pfs_fd) {
			pfs_fd_t	*pfs_fd;
			int		sfd;
			mach_port_t	sfp;

			pfs_fd = fork_info[i].fdte->pfs_fd;

			/*
			 * Need to get the rights for each of the stripefiles,
			 * and get another reference on each file structure.
			 */
			for (sfd = 0; sfd < pfs_fd->p_stripe_factor; sfd++) {
				sfp = pfs_fd->p_stripe_fdt[sfd].s_fp; 
				port_table[nports++] = sfp; 
			}
			
		}
#endif
	}
	return(nports);
}

/*
 * Alter the reference count (up or down) by n for a file port
 * right. This reference counting is maintained by the fileserver -
 * which must be contacted to make this adjustment.
 */
void
port_modref(fp, n)
	mach_port_t	fp;
	int		n;
{
	int		error;
	int		i;

	/*
	 * Call the fileserver to do this in one operation.
	 */
	error = fsvr_file_ref(fp, n);
	if (error) {
		EPRINT(("fsvr_file_ref fp=%x n=%d\n", fp, n));
		EPRINT(("fsvr_file_ref: error=0x%x", error));
		emul_panic("port_modref: fsvr_file_ref failed");
	}
}
	
/*
 * Modify port right references associated with file descriptors.
 * Called at rforkmulti time with FDT_LOCK held.
 * The argument specifies the number of additional references to
 * be registered for all unique open files; this is the number of new
 * child processes that will be created.
 */
void
fdt_port_modref(n)
	int	n;
{
	fdt_entry_t		*fdte;
	int			fdes, i;

	/*
	 * Because multiple fd slots can refer to the same fd entry,
	 * we're careful to ensure that a right is counted only once for
	 * each child created.
	 */
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		/* skip if this port right has already been accounted for */
		if (fdte->referenced)
			continue;
#ifdef	PFS
		if (fdte->pfs_fd) {
			pfs_fd_t	*pfs_fd = fdte->pfs_fd;
			mach_port_t	sfp;
			int		sfd;
			for (sfd = 0; sfd < pfs_fd->p_stripe_factor; sfd++) {
				sfp = pfs_fd->p_stripe_fdt[sfd].s_fp; 
				(void) port_modref(sfp, n);
			}
		}
#endif
		(void) port_modref(fdte->fp, n);
		fdte->referenced = 1;
	}

	/*
	 * Clear the 'referenced' flags.
	 */
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED)
			continue;
		fdte->referenced = 0;
	}
}

/*
 * Insert port rights associated with file descriptors into a task after
 * a process has moved to a new node (and thus is in a new task).
 */
void
moveproc_insert_rights(task)
	task_t			task;
{
	fdt_entry_t		*fdte;
	int			fdes;
	int			error;
	mach_msg_type_name_t	tmp_type;

	/*
	 * Because multiple fd slots can refer to the same fd entry,
	 * we're careful to ensure that a right is extracted from the
	 * old task only once.
	 */
	if (!FDT_TRY_LOCK())
		emul_panic("moveproc_insert_rights: FDT lock held");
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;

		/* don't extract a right from the old task more than once */
		if (fdte->referenced)
			continue;

		/* files needing migrate notification must be notified again */
		if (fdte->notify_on_migrate)
			fdte->was_notified = 0;

		/* extract rights for file port and (possibly) the mem_obj */
		error = mach_port_extract_right(task, 
						fdte->fp,
						MACH_MSG_TYPE_MOVE_SEND,
						&fdte->fp,
						&tmp_type);
		if (error != KERN_SUCCESS)
			EPRINT(("moveproc_insert_rights fp failure=%d", error));
#ifdef	MAPPED_FILES
		if (fdte->mem_obj != MACH_PORT_NULL) {
			error = mach_port_extract_right(task, 
							fdte->mem_obj,
							MACH_MSG_TYPE_MOVE_SEND,
							&fdte->mem_obj,
							&tmp_type);
			if (error != KERN_SUCCESS)
				EPRINT(("moveproc_insert_rights mem_obj err=%d",
					error));
		}
		if (fdte->flags) {
			emul_panic("moveproc_insert_rights: token held");
		}

		/* unfortunately, the window cannot be inherited */
		fdte->win_size = 0;
#endif
#ifdef	PFS
		if (fdte->pfs_fd) {
			/*
			 * Need to extract the rights from each of the
			 * stripe directories.
			 */
			int sfd;
			mach_port_t *sfp;
			int sfactor = fdte->pfs_fd->p_stripe_factor;

			for(sfd = 0; sfd < sfactor; sfd++) {
				sfp = &fdte->pfs_fd->p_stripe_fdt[sfd].s_fp;
				error = mach_port_extract_right(
						task, *sfp, 
						MACH_MSG_TYPE_MOVE_SEND,
						sfp, &tmp_type);
                                if (error != KERN_SUCCESS)
                                        EPRINT(("move_proc_extract_right sfp failure=%d",
						error));
                        }
		}
#endif
		fdte->referenced = 1;
	}

	/*
	 * Clear the 'referenced' flags.
	 */
	for (fdes=0; fdes<=fdt_lastfile; ++fdes) {
		fdte = fdt[fdes].fdte;
		if (fdte == FD_EMPTY || fdte == FD_RESERVED) 
			continue;
		fdte->referenced = 0;
	}

	FDT_UNLOCK();

	/*
	 * Extract the root and current directory ports.
	 */
	error = mach_port_extract_right(task, 
					rootdir_port,
					MACH_MSG_TYPE_MOVE_SEND,
					&rootdir_port,
					&tmp_type);
	if (error != KERN_SUCCESS)
		EPRINT(("moveproc_insert_rights rootdir_port failure=%d", 
			error));
	error = mach_port_extract_right(task, 
					currentdir_port,
					MACH_MSG_TYPE_MOVE_SEND,
					&currentdir_port,
					&tmp_type);
	if (error != KERN_SUCCESS)
		EPRINT(("moveproc_insert_rights currentdir_port failure=%d", 
			error));

	if (our_svipc_port != MACH_PORT_NULL) {
		error = mach_port_extract_right(task, 
						our_svipc_port,
						MACH_MSG_TYPE_MOVE_SEND,
						&our_svipc_port,
						&tmp_type);
		if (error != KERN_SUCCESS)
			EPRINT(("moveproc_insert_rights: svipc_port failure=%d port=0x%x",
				error, our_svipc_port));
	}
}


/*
 *  Inform a file port that a process holding a Snd rt has moved.
 *  Pipes, FIFOs, and sockets are all interested in this information,
 *  as they may wish to relocate to a different storage node or make
 *  arrangements for maintaining contact with network nodes.
 */
int
report_migration(fdte)
	fdt_entry_t		*fdte;
{
	extern node_t emul_tnc_mynode();
	int status;

	EASSERT(current_pid > 0);
	if (edebug & EDEBUG_TNC_UIPC)
		EPRINT(("report_migration: fp=0x%x cred=0x%x pid=%d node=%d\n",
			fdte->fp, credentials_port, current_pid,
			emul_tnc_mynode()));
	fsvr_report_migrate(fdte->fp, credentials_port,
			    current_pid, emul_tnc_mynode(), &status);
	fdte->was_notified = 1;
	return(status);
}
#endif	/* TNC */


int
e_swapon(proc_port, interrupt, fname, flags, lowat, hiwat)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	char			*fname;
	int			flags;
	int			lowat;
	int			hiwat;
{
	int			error;
	mach_port_t		start_port;
	transaction_id_t	trans_id;
	int			len_fname;

	if (!user_strlen(fname, &len_fname))
		return EFAULT;
	if (*fname == '\0' && !nullcompat)
		return(ENOENT);	

	start_port = (*fname == '/') ? rootdir_port : currentdir_port;

	isc_register(start_port, &trans_id);
	error = fsvr_swapon(start_port,
			  credentials_port,
			  trans_id,
			  rootdir_port,
			  fname, len_fname + 1,
			  flags,
			  lowat,
			  hiwat);
	isc_deregister(interrupt);

	return (error);
}
