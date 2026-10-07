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
 * Copyright (c) 1990 Carnegie-Mellon University
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: bsd_user_side.c,v $
 * Revision 1.20  1995/03/02  18:44:40  stans
 *  Vnode caching support.
 *
 *  Reviewer:jlitvin,suri,cfj
 *  Risk: medium
 *  Benefit or PTS #: 8129
 *  Testing:WW07 sats
 *
 * Revision 1.19  1995/02/18  01:16:40  yazz
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
 * Revision 1.18  1995/02/10  18:00:38  toman
 * Added fdt_port_modref() call to release references in e_rforkmulti_call()
 * and e_forkfamily_call() in the case when memory allocation fails.
 *
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 12392
 *  Testing: By inpsection.
 *  Module(s): emulator/bsd_user_side.c
 *             emulator/emul_chkpnt.c
 *
 * Revision 1.17  1995/02/01  21:21:16  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1994/11/18  20:22:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.15  1994/06/02  22:10:44  chrisp
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
 * Revision 1.14  1994/03/14  01:43:38  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, Checkpoint Restart specific, EATS
 *  Module(s):
 *
 * Revision 1.13  1993/12/14  15:09:17  nandy
 * Make sure the task port is still valid before inserting rights
 * to it in e_rforkmulti_call()
 *
 *  Reviewer: cfj
 *  Risk: Low
 *  Benefit or PTS #: 7382
 *  Testing: Developer testing
 *  Module(s): bsd_user_side.c
 *
 * Revision 1.12  1993/10/29  11:56:24  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.11  93/09/29  19:27:00  cfj
 * Modify the function e_exit() so that it returns the error code from the server
 * RPC as required.
 * 
 * Revision 1.10  1993/08/11  18:34:23  stefan
 * Fixed a bug in SLL that could make rfork(localnode) actually
 * go to a remote node.
 *
 * Revision 1.9  1993/07/14  17:30:28  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 *
 * Revision 1.1.1.6  1993/07/01  18:21:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/06/21  13:45:46  stefan
 * Made modifications so that a server that is compiled with -sll works with
 * an emulator that is compiled with -DSLL (and vice versa).
 *
 * Revision 1.1.1.6  1993/07/01  18:21:46  cfj
 * Adding new code from vendor
 *
 * Revision 1.7  1993/05/13  09:15:26  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.6  1993/05/06  20:14:32  brad
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.4  1993/05/03  17:17:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.54  93/06/20  18:25:15  yazz
 * [ ad1.04 merge ]
 * 	Add node parameter to e_rfork_call() call.
 * 
 * Revision 2.53  93/06/16  13:48:59  klh
 * 	Revision 2.46  93/06/03  17:34:46  rabii
 * 		Added node number to e_fork_call and bsd_fork. (rabii)
 * 
 * Revision 2.52  93/04/24  18:46:34  klh
 *
 * 	Revision 2.45  93/04/08  11:15:52  loverso
 * 		If task_terminate() fails, do not return to user.
 * 		(Gr4.1)
 *
 * 	Revision 2.43  92/12/10  15:01:10  durriya
 * 		check for ENAMETOOLONG in e_exec_call
 *
 * Revision 1.5  1993/04/03  03:17:22  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.3.2.2  1993/03/19  01:36:42  dbm
 * Added a parameter to the close_on_exit() call to allow PFS I/O modes to
 * work correctly with signal exits.
 *
 * Revision 1.1.2.3.2.1  1992/12/16  05:56:32  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  02:51:08  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:08:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/10  15:42:29  cfj
 * Fix up a merge problem.
 *
 * Revision 1.1.2.2  1992/11/06  18:20:37  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/05  22:15:25  dleslie
 * cal modifications for NX through noon, November 5, 1992ZZ
 *     Revision 2.49  1992/11/02  21:51:17  cfj
 *     Final integration and testing of IPD modifications
 *
 * Revision 2.51  92/11/24  14:15:52  chrisp
 * Fix typo in e_rexecve_call() call to copy_args(). [chrisp for roman]
 * 
 * Revision 2.50  92/11/23  17:37:21  klh
 * 	Revision 2.42  92/11/11  10:40:47  rabii
 * 		Re-did the algorithm in copy_args so it does not waste address 
 * 		space (rabii/loverso)
 * 
 * 	Revision 2.40  92/11/05  17:26:58  roy
 * 		Added setrlimit.
 * 		[92/10/27            roy]
 * 
 * Revision 2.49  92/11/02  11:28:17  roman
 * Add interface for rforkmulti() to be able to use out-of-line data
 * 	when necessary for very long node lists.
 * Change some types for cleaner compilation.
 * 
 * Revision 2.48  92/10/28  14:45:08  roman
 * Add code for TNC-specific table_node() system call.
 * Got rid of extraneous fdt_atomic_end() to allow rexecve() to fail
 * 	gracefully.
 * Fix some types for a cleaner compilation.
 * 
 * Revision 2.47  92/10/08  10:54:00  roman
 * No longer have migrate and rexec in the emulator invoke the callback 
 * 	thread to do the real work and to contact the server - 
 * 	instead the emulator directly invokes the server, and the 
 * 	server is responsible for ensuring that all threads are in 
 * 	the correct (quiescent) state.
 * Change interface to e_rexecve_arrival_call() to look similar to the
 * 	interface to e_execve_call().
 * 
 * Revision 2.46  92/10/06  12:01:27  roman
 * Fix RCS comment.
 * Add traced flag to rexecve (similar to execve).
 * 
 * Revision 2.45  92/10/05  13:49:40  klh
 * 	Revision 2.39  92/09/29  16:51:01  rabii
 * 		[92/09/08  09:45:27  cfj@ssd.intel.com]
 * 		Fixed a spot in e_signal where sigtramp was not getting set 
 *		correctly for the i860.
 * 
 * 	Revision 2.38  92/09/24  16:48:13  rabii
 * 		Set traced bit (STRC) from the server on exec();  SIGTRAP is 
 *		now generated by the emulator after all state set up.  
 *		(dwm; #376)
 * 
 * 	Revision 2.37  92/09/20  11:24:23  roy
 * 		Check for null path name and return ENOENT in e_exec_call(). 
 *		(mmp)
 * 
 * 	Revision 2.36  92/08/26  12:09:02  loverso
 * 		Added emul_blocking/emul_unblocking to PM calls.  (loverso)
 * 
 * Revision 2.44  92/10/01  10:16:41  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.43  92/09/28  16:23:06  roman
 * Fix types for a cleaner compilation.
 * 
 * Revision 2.42  92/08/06  16:36:31  roman
 * Fix RCS comment.
 * 
 * Revision 2.41  92/08/06  13:37:29  klh
 * 	Revision 2.34  92/07/29  08:35:12  rabii
 * 		Fixed RCS log
 * 
 * 	Revision 2.33  92/07/29  08:32:11  rabii
 * 		Added calls to fdt_atomic_begin and fdt_atomic_end.
 * 		send_sig now takes an extra arg which is the interrupt boolean 
 *		from it's caller, this will be set to TRUE to force the signal 
 *		to be taken ASAP. (rabii)
 * 		[92/07/21            roy]
 * 
 * Revision 2.40  92/07/30  15:54:43  chrisp
 * Make migrate() and rexec() atomic for the FDT. In addition, the server is
 * 	now called from the ISC thread for these calls so that no callback
 * 	or revoke port operations are pending.
 * 
 * Revision 2.39  92/07/17  11:22:34  roman
 * Add vaid address check to get_tnc_port().
 * 
 * Revision 2.38  92/07/07  14:56:57  roman
 * Fix rforkmulti() problem where the count was not set as an output parameter.
 * 
 * Revision 2.37  92/07/06  08:59:09  chrisp
 * For TNC, remove EPRINTs for failure of mach port/task manipulation after
 * 	rfork() and rforkmulti() to cater for premature termination of
 * 	children.
 * Add user_rcheck()s for parameters passed to rforkmulti().
 * 
 * Revision 2.36  92/06/18  12:10:36  roman
 * Prevent migration of multi-threaded processes for TNC.
 * 
 * Revision 2.35  92/06/10  10:08:56  klh
 * 	Revision 2.31  92/06/08  18:15:48  pjg
 * 		Fixed e_exit() to work in case of outstanding signals.
 * 		Add additional EFAULT checks to bsd_table().
 * 		(loverso)
 * 
 * Revision 2.34  92/06/05  13:53:32  klh
 * 	Revision 2.30  92/05/31  18:55:14  loverso
 * 		Revision 3.23  92/02/28  00:10:59  condict
 * 		Define e_exit to commit suicide if RPC to server ever returns.
 * 		Insert many calls to user access-checking functions, for
 * 		EFAULT recovery (avoids core-dump in emulator).
 * 
 * 	Revision 2.29  92/05/24  14:06:39  pjg
 * 		Replaced send_sigpipe with send_sig which now also handles 
 *		SIGXFSZ. (rabii)
 * 
 * 	Revision 2.28  92/05/18  12:25:52  roy
 * 		Revision 2.23.1.1  92/04/22  09:46:50  roy
 * 		Call close_on_exit() from e_exit().
 * 		[92/04/08            roy]
 * 
 * Revision 2.33  92/04/24  12:34:06  roman
 * Update rexecve() to put in v0.8.5.1 OSF fixes to execve() with respect
 * 	to maximum size of exec arguments and handling of auxv strings.
 * 
 * Revision 2.32  92/04/21  11:32:06  roman
 * Remove unused (unnecessary) last parameter to bsd_rforkmulti() invocation.
 * 
 * Revision 2.31  92/04/17  09:46:02  chrisp
 * For TNC, when setting the emulator's default SIGMIGRATE handler, set
 * 	sigreturnaddr variable also if i386. In addition, EPRINT a warning
 * 	if the sigaction call fails.
 * 
 * Revision 2.30  92/04/14  09:56:02  roman
 * Moved dopoll() to fsvr_user_side.c (with e_select/e_poll). (pjg)
 * Removed bogus transaction_id's. (loverso).
 * Removed current directory and root directory ports from bsd_execve 
 *	call. (pjg)
 * 
 * Revision 2.29  92/04/02  11:19:38  roman
 * Change user interface to rforkmulti() to return -2 if some forks were
 * done, but not all.
 * 
 * Revision 2.28  92/04/01  16:12:10  roman
 * Add support for rforkmulti() system call for TNC.
 * Minor formatting fixes.
 * 
 * Revision 2.27  92/03/27  17:28:43  roman
 * Optimize the caching of the "current node" to call norma_node_self() only
 *
 * Revision 2.26  92/04/07  13:38:50  pjg
 * 	Moved dopoll() to fsvr_user_side.c (with e_select/e_poll).
 * 	Removed bogus transaction_id's.  (loverso)
 * 
 * Revision 2.25  92/04/05  16:40:41  pjg
 * 	Optimize the caching of the "current node" to call norma_node_self() 
 * 	only when first necessary, rather than at the start of every process.
 * 	(roman)
 * 
 * 	Add in support for new system calls set_tnc_port(), get_tnc_port(),
 * 	and node_self().
 * 	Allow rfork() and migrate() system calls to short-circuit and succeed
 * 	if the node specified is the current node. (roman)
 * 
 * Revision 2.24  92/03/20  11:53:47  pjg
 * 	In e_exec_call, deallocate arg_size instead of arg_size+4096.
 * 
 * Revision 2.23  92/03/09  11:00:50  durriya
 * 	vm_(de)allocate the auxv_strings seperately in e_exec_call
 * 
 * Revision 2.22  92/03/03  13:57:16  pjg
 * 	Revision 2.21.1.1  92/03/02  14:11:08  jeffreyh
 * 		Fix e_getpid
 * 
 * Revision 2.21  92/03/01  18:48:31  pjg
 * 	Changed fork and exit to not use fsvr_vref and fsvr_vrele.
 * 
 * Revision 2.20  92/02/11  18:47:43  pjg
 * 	Moved all the functions that needed fdt.h to fsvr_user_side.c.
 * 	Add comment about signature_port; Fix missing args (loverso).
 * 	Add code for rfork/rexec/migrate (TNC only) (roman@locus.com).
 * 	Add code for SIGMIGRATe signal special handling in signal handlers.
 * 	Move fork_insert_rights() to fsvr_user_side.c.
 * 	Fix up auxv_mig_t and auxv_type_t handling for sanity.
 * 
 * Revision 2.19  92/01/17  21:09:26  roy
 * 	Make socket calls interruptible (loverso).
 * 
 * Revision 2.18  92/01/17  17:16:39  roy
 * 	Add interruptible system call support (loverso).
 * 
 * Revision 2.17  92/01/17  00:08:13  roy
 * 	Fix last fix.
 * 
 * Revision 2.16  92/01/16  17:41:48  roy
 * 	Handle sig_port and credentials_port properly in e_fork_call.
 * 
 * Revision 2.15  92/01/14  10:41:34  roy
 * 	92/01/11  18:50:52  ses
 * 	Added OSF1_ADFS code to take initial references on the root
 * 	directory and cwd (in e_fork_call) and to release them (in e_exit).
 * 
 * Revision 2.14  92/01/09  15:18:47  roy
 * 	Unix domain socket support.  (loverso)
 * 
 * Revision 2.13  92/01/05  18:15:47  roy
 * 	Added credentials port arg to bsd_execve.
 * 
 * Revision 2.12  92/01/02  18:36:39  roy
 * 	92/01/02  17:46:30  roy
 * 	Add start port and root port args to bsd_execve.
 * 
 * 	91/10/09  18:29:59  noemi
 * 	Moved all filesystem specific calls to fsvr_user_side.c
 * 
 * Revision 2.11  91/12/18  10:24:05  roy
 * 	91/12/13  13:01:55  sp
 * 	Remove old BSD code, VICE and CMUCS conditionals. Delete dead
 * 	e_mount/umount and e_xutimes functions.
 * 
 * 	91/12/05  08:42:27  sp
 * 	BUG 49: copy the null string terminator to the end of a readlink call
 * 
 * 	91/11/13  14:55:04  barbou
 * 	Fix for bug #33: return E2BIG when the OSF/1.0.2 limit reached.
 * 	Fix for bug #16: return EFAULT when unlink called with null pathname.
 * 	(that's what diff(1) does !).
 * 
 * 	91/10/30  17:41:30  bernadat
 * 	Fix for bug #30, handle NULL times argument to utimes(). [barbou]
 * 
 * 	91/10/29  16:10:47  barbou
 * 	Fix for bug #26: e_orecvfrom(): no memory fault if fromlenaddr NULL.
 * 
 * 	91/10/28  11:09:45  condict
 * 	Add functions e_msleep/e_mwakeup for Berkeley semaphores.
 * 
 * Revision 2.10  91/12/13  10:04:05  roy
 * 	91/11/16  17:32:21  roy
 * 	For read-only files, mapping must be done by the server.
 * 
 * 	91/11/12  10:15:04  roy
 * 	Initial working revision of MAPPED_FILES code.
 * 
 * 	91/10/29  17:48:54  roy
 * 	Major overhaul related to the fd table.  File
 * 	descriptors now point to entries (fdte's) which
 * 	are reference counted and contain a send right
 * 	to the server's corresponding file structure.
 * 
 * Revision 2.9  91/11/25  11:18:51  rabii
 * 	Added routine e_rmknod for making remote devices
 * 
 * Revision 2.8  91/11/14  15:32:49  rabii
 * 	Added i860 support
 * 
 * Revision 2.7  91/10/14  13:34:18  srl
 * repair header comments
 * 
 * Revision 2.6  91/10/14  13:05:45  srl
 * (Grenoble V2.2 merge)
 * 	91/10/04  16:35:19  sp
 * 	Add support for the creation of the auxiliary vector
 *
 * 	91/09/27  11:54:54  emcmanus
 * 	Added profil() system call.
 *
 * 	91/09/13  13:53:58  sp
 * 	Pass entry count properly to bsd_execve()
 * 
 * Revision 2.5  91/10/04  14:39:28  chrisp
 * Fix up some comments in the e_exit() code. Fix writev() to not pass
 * extra fdes parameter.
 * 
 * Revision 2.4  91/09/18  08:17:51  sjs
 * integrate Locus changes	yazz
 *         Returned pair of fd's for pipe and socketpair in [0] and [1]
 *         rather than in [1] and [2].
 * 
 * Revision 2.3  91/09/17  17:06:02  sjs
 * integrate Locus changes
 * 
 * Revision 2.2  91/08/30  16:39:52  rabii
 * 	Initial V2 Checkin
 * 
 * Revision 3.10  91/08/27  15:23:26  barbou
 * Upgrade to UX26. Fixed missing parenthesis in e_sigaction().
 * 
 * Revision 3.9  91/08/09  11:41:34  barbou
 * Added e_signal() for systemV signal(). Added missing rval parameters.
 * 
 * Revision 3.8  91/06/12  09:31:59  condict
 * Eliminated the oexecve sys call (no longer needed for bootstrap).
 * Also, eliminated unused setquota stub functions.
 * 
 * Revision 3.7  91/06/07  15:43:42  barbou
 * Let the server know about the arguments and the environment (for ps(1)).
 * 
 * Revision 3.6  91/05/30  17:23:07  jose
 * Added o prefix to compatibility syscalls.
 * 
 * Revision 3.5  91/05/15  17:16:43  barbou
 * i386 uses u_sigreturn instead of u_sigtramp.
 * New entry point: e_sigaction().
 * 
 * Revision 3.4  91/05/07  15:13:22  condict
 * Add oexecve function (renamed the 4.3 execve to oexecve; used only during
 * bootstrapping now).
 * 
 * Revision 3.3  91/04/12  13:43:30  condict
 * Comment out e_mount and e_umount; not appropriate for OSF/1
 *
 * Revision 3.2  91/03/08  15:51:24  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:20:26  condict
 * Conditionally omit e_ functions when VICE and CMUCS not defined.
 * 
 * Revision 3.0  91/01/17  12:05:03  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.14  91/03/20  15:01:21  dbg
 * 	Fix for ANSI C.
 * 	[91/02/22            dbg]
 * 
 * Revision 2.13  90/11/05  15:31:12  rpd
 * 	Added spin_lock_t.
 * 	[90/10/31            rwd]
 * 
 * Revision 2.12  90/10/25  15:06:18  rwd
 * 	Check for MACH_RCV_TOO_LARGE and check data size in pioctl().
 * 	[90/10/03            rwd]
 * 
 * Revision 2.11  90/08/06  15:29:59  rwd
 * 	Turn all if () EPRINT to if () {EPRINT} since the pmax
 * 	compiler did the wrong thing.
 * 	[90/08/03            rwd]
 * 	Fix for !shared_enabled and not MAP_UAREA and/or MAP_FILE.
 * 	[90/07/17            rwd]
 * 	Turn off debugging.
 * 	[90/06/27            rwd]
 * 	Change to reflect change in bsd_select().
 * 	[90/06/25            rwd]
 * 	Remove extra copying in bsd_select().
 * 	[90/06/11            rwd]
 * 	Fixed readv/writev bug.
 * 	[90/06/05            rwd]
 * 
 * Revision 2.10  90/06/19  23:06:36  rpd
 * 	Fixed argument to e_readwrite in e_readv.
 * 	[90/06/06            rpd]
 * 
 * Revision 2.9  90/06/02  15:20:16  rpd
 * 	Converted new functions to new IPC.
 * 	[90/06/01            rpd]
 * 
 * 	Fixed check for negative numbers in e_emulator_error.
 * 	[90/05/12            rpd]
 * 
 * 	Removed e_check_server_signals.
 * 	[90/05/10            rpd]
 * 	Added missing cast in e_sigvec.
 * 	[90/05/03            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:25:49  rpd]
 * 
 * Revision 2.8  90/05/29  20:22:24  rwd
 * 	Fix references to nocopy case of e_readwrite to release the
 * 	shared buffer themselves when finished.
 * 	[90/05/16            rwd]
 * 	Fix couple of coding problems in e_readv.
 * 	[90/05/14            rwd]
 * 	Make readv and writev use the existing shared interfaces.
 * 	[90/05/08            rwd]
 * 
 * 	Fix non shared_enabled case.
 * 	[90/05/02            rwd]
 * 	Pass count arguments for all path_name_ts.
 * 	[90/04/06            dbg]
 * 
 * 	Do argument shuffling for exec entirely within emulator.
 * 	[90/03/22            dbg]
 * 
 * 	Added MAP_FILE case to e_write.
 * 	[90/04/04            rwd]
 * 
 * Revision 2.7  90/05/21  13:45:30  dbg
 * 	Fix count passed to copystr from copy_args.
 * 	[90/05/11            dbg]
 * 
 * 	Always set u_sigtramp.
 * 	[90/04/23            dbg]
 * 
 * 	Pass count arguments for all path_name_ts.
 * 	[90/04/06            dbg]
 * 
 * 	Do argument shuffling for exec entirely within emulator.
 * 	[90/03/22            dbg]
 * 
 * Revision 2.6  90/03/14  21:22:41  rwd
 * 	Changed shared locks to use share_lock macros.  Added mapped
 * 	sigvec.
 * 	[90/02/16            rwd]
 * 	Fixed typo.  Added e_emulator_error.  Added MAP_UAREA code.
 * 	[90/01/26            rwd]
 * 
 * Revision 2.5  89/11/29  15:26:05  af
 * 	Made sigvec take an extra param, to support those implementations 
 * 	where the signal trampoline code address is passed to the kernel
 * 	dinamically (mips, balance, mmax...).
 * 	[89/11/16  15:15:07  af]
 * 
 * Revision 2.4  89/11/15  13:26:31  dbg
 * 	Add readv, writev, pioctl.
 * 	[89/11/07            dbg]
 * 	Add table call.
 * 	[89/10/25            dbg]
 * 
 * 	In exec, pass entry_count through to bsd_execve.
 * 	[89/10/24            dbg]
 * 
 * Revision 2.3  89/10/17  11:23:52  rwd
 * 	Add interrupt return value to all calls.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.2  89/08/09  14:35:14  rwd
 * 	Set buflen for all out sockarg_t
 * 	[89/08/08            rwd]
 * 
 * $EndLog
 */
/*
 * Glue routines between traps and MiG interfaces.
 */

#include <mach_init.h>
#include <mach/mig_errors.h>
#include <uxkern/bsd_1.h>
#include <uxkern/bsd_msg.h>

/* WARNING: the lengthy path name below is necessary to
 * prevent varargs.h from being found in the mk/release
 * subdirectory.  For some reason, even though CPATH has
 * "../server/include" before "mk/release/.../include",
 * the file is found in the latter directory, if it is
 * not qualified with a path prefix.
 */
#include <../server/include/varargs.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/mman.h>           /* For msleep/mwakeup */
#include <sys/uio.h>
#include <sys/syscall.h>
#include <sys/errno.h>
#include <sys/proc.h>
#include "emul.h"

#ifdef	MAP_UAREA
#include <sys/ushared.h>

extern int shared_enabled;
extern struct ushared_ro *shared_base_ro;
extern struct ushared_rw *shared_base_rw;
#endif	MAP_UAREA

extern int	nullcompat;

#ifdef NX
extern boolean_t nx_application;
#endif /* NX */


/*
 * Copy zero-terminated string and return its length,
 * including the trailing zero.  If longer than max_len,
 * return -1.
 */
int
copystr(from, to, max_len)
	register char	*from;
	register char	*to;
	register int	max_len;
{
	register int	count;

	count = 0;
	while (count < max_len) {
	    count++;
	    if ((*to++ = *from++) == 0) {
		return (count);
	    }
	}
	return (-1);
}

int
copy_args(argp, arg_count, envp, env_count, arg_addr, arg_size)
	register char	**argp;
	int		*arg_count;	/* OUT */
	register char	**envp;
	int		*env_count;	/* OUT */
	vm_address_t	*arg_addr;	/* IN/OUT */
	vm_size_t	*arg_size;	/* IN/OUT */
{
	register char		**app;
	register char		*ap;
	int			len;
	register unsigned int	maxlen = 0;
	register char		*cp;
	register int		na = 0;
	kern_return_t		kr;

	app = argp;
	while (app && *app) {
	    if (!user_strlen(*app, &len)) {
		return(EFAULT);
	    } else
		maxlen += len +1;
	    *app++;
	}
	app = envp;
	while (app && *app) {
	    if (!user_strlen(*app, &len)) {
		return(EFAULT);
	    } else
		maxlen += len +1;
	    *app++;
	}

	/*
	 * Simulate a limited space...
	 * It's a shame but we need to be OSF/1.0.2 compatible
	 * (this limitation is tested by the VSX)
	 */
	if (maxlen > ARG_MAX) {
		return E2BIG;
	}

	*arg_size = maxlen;
	kr = vm_allocate(mach_task_self(), arg_addr, *arg_size, TRUE);
	if (kr != KERN_SUCCESS)
		return (E2BIG);

	cp = (char *)*arg_addr;
	while (argp && (ap = *argp++) != 0) {
	    na++;
	    (void)strcpy(cp, ap);
	    cp += strlen(ap) +1;
	}
	*arg_count = na;

	na = 0;
	while (envp && (ap = *envp++) != 0) {
	    na++;
	    (void)strcpy(cp, ap);
	    cp += strlen(ap) +1;
	}
	*env_count = na;

	return (ESUCCESS);
}

/*
 * Send a signal to the current process. Use the value saved
 * from a prior call to getpid() if possible.
 */
void
send_sig(sig, intr)
int		sig;
boolean_t	*intr;
{
	boolean_t	interrupt = 0;		/* XXX: bogus */
	int		error;
	int		rval[2];
	int		uarg[2];

	if (sig == 0) {
		return;
	}

	*intr = TRUE;

	/*
	 * Send a signal to the current pid.
	 */
	EASSERT(current_pid > 0);	/* not -1, and not 0 */
	uarg[0] = (int) current_pid;
	uarg[1] = sig;
	(void) emul_vproc_generic(vproc_port, &interrupt, SYS_kill, uarg, 
				  rval);
}

int
e_getpid(serv_port, interrupt, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		*rvalp;
{
	int		error;
	int		*argp = NULL;

#ifdef	MAP_UAREA
	if (shared_enabled) {
		rvalp[0] = shared_base_ro->us_pid;
		rvalp[1] = shared_base_ro->us_ppid;
		current_pid = rvalp[0];
		e_checksignals(interrupt);
		return(ESUCCESS);
	}
#endif	/* MAP_UAREA */
	error = emul_vproc_generic(vproc_port, interrupt, SYS_getpid, 
			&argp, rvalp);
	if (error != 0)
		return(error);
	current_pid = rvalp[0];

	return(ESUCCESS);
}

int
e_exit(serv_port, interrupt, status, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		status;
	int		*rvalp;
{
	int		error;
	kern_return_t	rc;

	/* 
	 * We let most emulator ports be cleaned up by the kernel's task
	 * destruction code.  However, we need to do some open file
	 * teardown.
	 */
	close_on_exit(0);

	/*
	 * Handle the exit() via the generic interface.
	 */
	error = emul_vproc_generic(vproc_port, interrupt, SYS_exit, &status, 
				   rvalp);

	/*
	 * If there is a signal for the process, we must take it before
	 * we're allowed to register a server thread.  Thankfully,
	 * emul_syscall will retry the syscall.
	 */
	if (error == ERESTART || error == EINTR || error == EPIPE || 
	    *interrupt)
		return error;

	/* Exit should never return, so: */
	EPRINT(("e_exit returned 0x%x from server RPC. Using task_terminate\n",
		error));

	do {
		 rc = task_terminate(mach_task_self());
		 e_emulator_error("emulator: task_terminate(0x%x) returned 0x%x\n", mach_task_self(), rc);
	} while (1);
	/*NOTREACHED*/
}

int
e_getrusage(serv_port, interrupt, which, rusage, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		which;
	register
	struct rusage	*rusage;
	int		*rval;
{
	register int	error;
	struct thread_basic_info bi;
	unsigned int	bi_count;

	if (!user_rwcheck(rusage, sizeof *rusage))
		return EFAULT;
	emul_blocking();
	error = bsd_getrusage(serv_port, interrupt, which, rusage);
	emul_unblocking();
	if (error || which != RUSAGE_SELF)
	    return (error);

	bi_count = THREAD_BASIC_INFO_COUNT;
	(void) thread_info(mach_thread_self(),
			THREAD_BASIC_INFO,
			(thread_info_t)&bi,
			&bi_count);

	rusage->ru_utime.tv_sec  = bi.user_time.seconds;
	rusage->ru_utime.tv_usec = bi.user_time.microseconds;
	rusage->ru_stime.tv_sec  = bi.system_time.seconds;
	rusage->ru_stime.tv_usec = bi.system_time.microseconds;

	return (0);
}

int
e_signal(serv_port, interrupt, sig, hdlr, tramp, rval)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register int		sig;
	register int		(*hdlr)();
	register int		(*tramp)();
	int 			*((*rval)());
{
	register int	error;

#ifdef	TNC
	/*
	 * Don't allow SIG_DFL for SIGMIGRATE -
	 * the default is to be caught in the emulator.
	 */	
	if (sig == SIGMIGRATE && hdlr == (int (*)()) SIG_DFL) {
		extern void emul_sigmigrate_handler();
		hdlr = (int (*)()) emul_sigmigrate_handler;
	}
#endif	/* TNC */
#ifdef NX 
	if (hdlr != (int (*)())SIG_DFL && sig == SIGUSR2 && nx_application)
		return EINVAL;
#endif /* NX */

#ifdef	MAP_UAREA 	
	if (shared_enabled && 0) {
		/* no optimization implemented: signal() is obsolete... */
	} else {
#endif	/* MAP_UAREA */
	emul_blocking();
	error = bsd_signal(serv_port,
			interrupt,
			sig,
			(vm_address_t) hdlr,
			(vm_address_t *) rval,
			(vm_address_t) tramp);
	emul_unblocking();
#if defined(mips) || defined(i860)
	if (error == 0) {
		extern int (*sigtramp)();
		sigtramp = tramp;
	}
#endif  /* defined(mips) || defined(i860) */
	return (error);
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA
}

int
e_htg_syscall(argp, rvalp)
{
}

#ifdef	COMPAT_43
struct sigvec	zero_sv = { 0, 0, 0 };

int
e_osigvec(serv_port, interrupt, sig, nsv, osv, tramp, rval)
	mach_port_t		serv_port;
	boolean_t		*interrupt;
	register int		sig;
	register struct sigvec	*nsv;
	register struct sigvec	*osv;
	int			(*tramp)();
	int			*rval;
{
	register int	error;
	struct sigvec	old_sig_vec;
	

	if (osv && !user_rwcheck(osv, sizeof *osv))
		return EFAULT;
	if (nsv && !user_rcheck(nsv, sizeof *nsv))
		return EFAULT;
#ifdef	TNC
	/*
	 * Don't allow SIG_DFL for SIGMIGRATE -
	 * the default is to be caught in the emulator.
	 */	
	if (nsv) {
		if (nsv->sv_handler == SIG_DFL && sig == SIGMIGRATE) {
			/*
			 * Construct an overriding sigvec and point nsv here
			 */
			extern void emul_sigmigrate_handler();
			struct sigvec	new_sig_vec;
			new_sig_vec.sv_handler = emul_sigmigrate_handler;
			new_sig_vec.sv_mask    = nsv->sv_mask;
			new_sig_vec.sv_flags   = nsv->sv_flags;
			nsv = &new_sig_vec;
		}
	}
#endif	/* TNC */
#ifdef NX 
	if (nsv) {
	    if (nsv->sv_handler != SIG_DFL && sig == SIGUSR2 && nx_application)
		return EINVAL;
	}
#endif /* NX */

#ifdef	MAP_UAREA 
    if (shared_enabled) {
	int bit;

	if (sig <= 0 || sig > NSIG || sig == SIGKILL || sig == SIGSTOP)
		return EINVAL;
	bit = sigmask(sig);
	if (osv) {
		osv->sv_handler = (void(*))shared_base_rw->us_signal[sig];
		osv->sv_mask = shared_base_rw->us_usigmask[sig];
		bit = sigmask(sig);
		osv->sv_flags = 0;
		if ((shared_base_rw->us_sigonstack & bit) != 0)
			osv->sv_flags |= SV_ONSTACK;
		if ((shared_base_rw->us_sigintr & bit) != 0)
			osv->sv_flags |= SV_INTERRUPT;
	}
	if (nsv) {
		if (sig == SIGCONT && nsv->sv_handler == SIG_IGN)
			return EINVAL;
#if	defined(mips) || defined(i860)
		/*
		 * check for unaligned pc on sighandler
		 */
		if (nsv->sv_handler != SIG_IGN
		    && ((int)nsv->sv_handler & (sizeof(int)-1)))
			return EINVAL;
#endif	/* mips || i860 */
		/*
		 * Set the signal trampoline code address - the
		 * server does not always know where it is.
		 */
#if	defined(balance) || defined(mips) || defined(i860)
		shared_base_rw->us_sigtramp = tramp;
#endif	/* defined(balance) || defined(mips) || defined(i860) */

		share_lock(&shared_base_rw->us_siglock);
		shared_base_rw->us_signal[sig] = (sig_t) nsv->sv_handler;
		shared_base_rw->us_usigmask[sig] = nsv->sv_mask &~ cantmask;

		if (nsv->sv_flags & SV_INTERRUPT)
			shared_base_rw->us_sigintr |= bit;
		else
			shared_base_rw->us_sigintr &= ~bit;
		if (nsv->sv_flags & SV_ONSTACK)
			shared_base_rw->us_sigonstack |= bit;
		else
			shared_base_rw->us_sigonstack &= ~bit;
		if (nsv->sv_handler == SIG_IGN) {
			shared_base_rw->us_sig &= ~bit;
			shared_base_rw->us_sigignore |= bit;
			shared_base_rw->us_sigcatch &= ~bit;
		} else {
			shared_base_rw->us_sigignore &= ~bit;
			if (nsv->sv_handler == SIG_DFL)
				shared_base_rw->us_sigcatch &= ~bit;
			else
				shared_base_rw->us_sigcatch |= bit;
		}
		share_unlock(&shared_base_rw->us_siglock);
	}
#if defined(mips) || defined(i860)
	{
		extern int (*sigtramp)();
		sigtramp = tramp;
	}
#endif  /* defined(mips) || defined(i860) */
	e_checksignals(interrupt);
	return ESUCCESS;
    } else {
#endif	MAP_UAREA
	emul_blocking();
	error = bsd_osigvec(serv_port,
			interrupt,
			sig,
			nsv != 0,
			(nsv) ? *nsv : zero_sv,
			&old_sig_vec,
			(vm_address_t) tramp);
	emul_unblocking();
	if (error == 0 && osv)
	    *osv = old_sig_vec;
#if defined(mips) || defined(i860)
	if (error == 0) {
		extern int (*sigtramp)();
		sigtramp = tramp;
	}
#endif  /* defined(mips) || defined(i860) */
	return (error);
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA
}
#endif	/* COMPAT_43 */

struct sigaction	zero_sa = { 0, 0, 0 };
/* defaultignmask copied from server/bsd/kern_sig.c */
#define defaultignmask	(sigmask(SIGCONT)|sigmask(SIGIO)|sigmask(SIGURG)| \
			sigmask(SIGCHLD)|sigmask(SIGWINCH)|sigmask(SIGINFO))

int
e_sigaction(serv_port, interrupt, sig, nsa, osa, tramp, rval)
	mach_port_t			serv_port;
	boolean_t			*interrupt;
	register int			sig;
	register struct sigaction	*nsa;
	register struct sigaction	*osa;
	int				(*tramp)();
	int				*rval;
{
	register int		error;
	struct sigaction	old_sig_act;

	if (osa && !user_rwcheck(osa, sizeof *osa))
		return EFAULT;
	if (nsa && !user_rcheck(nsa, sizeof *nsa))
		return EFAULT;
#ifdef	TNC
	/*
	 * Don't allow SIG_DFL for SIGMIGRATE -
	 * the default is to be caught in the emulator.
	 */	
	if (nsa) {
		if (nsa->sa_handler == SIG_DFL && sig == SIGMIGRATE) {
			/*
			 * Construct an overriding sigaction and point nsa here
			 */
			extern void emul_sigmigrate_handler();
			struct sigaction	new_sig_act;
			new_sig_act.sa_handler = emul_sigmigrate_handler;
			new_sig_act.sa_mask    = nsa->sa_mask;
			new_sig_act.sa_flags   = nsa->sa_flags;
			nsa = &new_sig_act;
		}
	}
#endif	/* TNC */

#ifdef NX 
	if (nsa) {
	    if (nsa->sa_handler != SIG_DFL && sig == SIGUSR2 && nx_application)
		return EINVAL;
	}
#endif /* NX */

#ifdef	MAP_UAREA 
	/* setting SIGCHLD might require changing p->p_flag in RO area */
    if (shared_enabled &&
	!(sig == SIGCHLD && nsa &&
	  ((nsa->sa_flags&SA_NOCLDSTOP) == 0) != ((shared_base_ro->us_flag&SNOCLDSTOP) == 0))) {
	int bit;

#if	defined(i386)
	shared_base_rw->us_sigreturn = (int (*)())tramp;
#endif	/* defined(i386) */

	if (sig <= 0 || sig > NSIG)
		return EINVAL;
	bit = sigmask(sig);
	if (osa) {
		osa->sa_handler = (void(*))shared_base_rw->us_signal[sig];
		osa->sa_mask = shared_base_rw->us_usigmask[sig];
		osa->sa_flags = 0;
		if ((shared_base_rw->us_sigonstack & bit) != 0)
			osa->sa_flags |= SA_ONSTACK;
		if ((shared_base_rw->us_sigintr & bit) == 0)
			osa->sa_flags |= SA_RESTART;
		if (shared_base_ro->us_flag & SNOCLDSTOP)
			osa->sa_flags |= SA_NOCLDSTOP;
	}
	if (nsa) {
		if (nsa->sa_handler != SIG_DFL && (sig == SIGKILL || sig == SIGSTOP))
			return EINVAL;
#if defined(mips) || defined(i860)
		/*
		 * check for unaligned pc on sighandler
		 */
		if (nsa->sa_handler != SIG_IGN
		    && ((int)nsa->sa_handler & (sizeof(int)-1)))
			return EINVAL;
#endif  /* defined(mips) || defined(i860) */
		/*
		 * Set the signal trampoline code address - the
		 * server does not always know where it is.
		 */
#if	defined(balance) || defined(mips) || defined(i860)
		shared_base_rw->us_sigtramp = (int (*)())tramp;
#endif	/* defined(balance) || defined(mips) || defined(i860) */
		share_lock(&shared_base_rw->us_siglock);
		shared_base_rw->us_signal[sig] = (sig_t) nsa->sa_handler;
		shared_base_rw->us_usigmask[sig] = nsa->sa_mask &~ sigcantmask;

		if ((nsa->sa_flags & SA_RESTART) == 0)
			shared_base_rw->us_sigintr |= bit;
		else
			shared_base_rw->us_sigintr &= ~bit;
		if (nsa->sa_flags & SA_ONSTACK)
			shared_base_rw->us_sigonstack |= bit;
		else
			shared_base_rw->us_sigonstack &= ~bit;
#if	0	/* XXX should let server do it */
		if (sig == SIGCHLD) {
			if (nsa->sa_flags & SA_NOCLDSTOP)
			  shared_base_ro->us_flag |= SNOCLDSTOP;
			else
			  shared_base_ro->us_flag &= ~SNOCLDSTOP;
		}
#endif	/* 0 */
		if (nsa->sa_handler == SIG_IGN ||
		    (bit & defaultignmask && nsa->sa_handler == SIG_DFL)) {
			shared_base_rw->us_sig &= ~bit;
			if (sig != SIGCONT)
			  shared_base_rw->us_sigignore |= bit;
			shared_base_rw->us_sigcatch &= ~bit;
		} else {
			shared_base_rw->us_sigignore &= ~bit;
			if (nsa->sa_handler == SIG_DFL)
				shared_base_rw->us_sigcatch &= ~bit;
			else
				shared_base_rw->us_sigcatch |= bit;
		}
		share_unlock(&shared_base_rw->us_siglock);
	}
#if defined(mips) || defined(i860)
	{
		extern int (*sigtramp)();
		sigtramp = tramp;
	}
#endif /* defined(mips) || defined(i860) */
	e_checksignals(interrupt);
	return ESUCCESS;
    } else {
#endif	MAP_UAREA
	emul_blocking();
	error = osf1_sigaction(serv_port,
			interrupt,
			sig,
			nsa != 0,
			(nsa) ? *nsa : zero_sa,
			&old_sig_act,
			(vm_address_t) tramp);
	emul_unblocking();
	if (error == 0 && osa)
	    *osa = old_sig_act;
#if defined(mips) || defined(i860)
	if (error == 0) {
		extern int (*sigtramp)();
		sigtramp = tramp;
	}
#endif  /* defined(mips) || defined(i860) */
	return (error);
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA
}

struct sigstack	zero_ss = { 0, 0 };

int
e_sigstack(serv_port, interrupt, nss, oss)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	struct sigstack	*nss;
	struct sigstack	*oss;
{
	register int	error;
	struct sigstack	old_sig_stack;

	if (oss && !user_rwcheck(oss, sizeof *oss))
		return EFAULT;
	if (nss && !user_rcheck(nss, sizeof *nss))
		return EFAULT;
#ifdef	MAP_UAREA 
    if (shared_enabled) {
	share_lock(&shared_base_rw->us_lock);
	if (oss)
	    *oss = shared_base_rw->us_sigstack;
	if (nss)
	    shared_base_rw->us_sigstack = *nss;
	share_unlock(&shared_base_rw->us_lock);
	e_checksignals(interrupt);
	return ESUCCESS;
    } else {
#endif	MAP_UAREA
	emul_blocking();
	error = bsd_sigstack(serv_port,
			interrupt,
			(nss != 0),
			(nss) ? *nss : zero_ss,
			&old_sig_stack);
	emul_unblocking();
	if (error == 0 && oss)
	    *oss = old_sig_stack;
	return (error);
#ifdef	MAP_UAREA
    }
#endif	MAP_UAREA
}

struct timeval	zero_tv = { 0, 0 };
struct timezone	zero_tz = { 0, 0 };

int
e_settimeofday(serv_port, interrupt, tv, tz)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	struct timeval	*tv;
	struct timezone	*tz;
{
	register int	error;

	if (tv && !user_rcheck(tv, sizeof *tv))
		return EFAULT;
	if (tz && !user_rcheck(tz, sizeof *tz))
		return EFAULT;
	emul_blocking();
	error = bsd_settimeofday(serv_port,
			interrupt,
			(tv != 0),
			(tv) ? *tv : zero_tv,
			(tz != 0),
			(tz) ? *tz : zero_tz);
	emul_unblocking();
	return error;
}

int
e_adjtime(serv_port, interrupt, delta, old_delta)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	struct timeval	*delta;
	struct timeval	*old_delta;
{
	register int	error;
	struct timeval	local_old_delta;

	if (delta && !user_rcheck(delta, sizeof *delta))
		return EFAULT;
	if (old_delta && !user_rwcheck(old_delta, sizeof *old_delta))
		return EFAULT;
	emul_blocking();
	error = bsd_adjtime(serv_port,
			interrupt,
			(delta != 0),
			(delta) ? *delta : zero_tv,
			&local_old_delta);
	emul_unblocking();
	if( error == 0 && old_delta){
		*old_delta = local_old_delta;
	}
	return error;
}

struct itimerval zero_itv = { 0, 0, 0, 0 };

int 
e_setitimer(serv_port, interrupt, which, itv, oitv)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		which;
	struct itimerval *itv;
	struct itimerval *oitv;
{
	register int	error;
	struct itimerval old_itimer_val;

	if (oitv && !user_rwcheck(oitv, sizeof *oitv))
		return EFAULT;
	if (itv && !user_rcheck(itv, sizeof *itv))
		return EFAULT;
	emul_blocking();
	error = bsd_setitimer(serv_port,
			interrupt,
			which,
			(itv != 0),
			(itv) ? *itv : zero_itv,
			&old_itimer_val);
	emul_unblocking();
	if (error == 0 && oitv)
	    *oitv = old_itimer_val;
	return (error);
}

int
e_table(serv_port, interrupt, id, index, addr, nel, lel, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	int		id;
	int		index;
	char *		addr;
	int		nel;	/* >0 ==> get, <0 ==> set */
	unsigned int	lel;
	int		*rval;
{
	register int	error;
	int		nel_done;

	if (nel < 0) {
		/*
		 * Set.
		 */
		if (!user_rcheck(addr, -nel * lel))
			error = EFAULT;
		else {
			emul_blocking();
			error = bsd_table_set(serv_port, interrupt,
				id, index, lel, nel,
				addr, -nel*lel,
				&nel_done);
			emul_unblocking();
		}
	}
	else {
	    char *		out_addr;
	    unsigned int	out_count;

	    emul_blocking();
	    error = bsd_table_get(serv_port, interrupt,
			id, index, lel, nel,
			&out_addr, &out_count,
			&nel_done);
	    emul_unblocking();

	    if (error == 0) {
		/*
		 * Copy table to addr
		 */
		if (!user_bcopy(out_addr, addr, lel * nel_done))
			error = EFAULT;
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) out_addr,
				     (vm_size_t) out_count);
	    }
	}
	if (error == 0)
	    *rval = nel_done;
	return (error);
}

#define MAXPRINT 256
#define putchar(x) if (pos<MAXPRINT) error[pos++] = (x); else goto done
int
e_emulator_error(fmt, va_alist)
	char *fmt;
	va_dcl
{
	va_list adx;
	register int b, c, i;
	char *s;
	u_long	value;
	char prbuf[11];
	register char *cp;
	char error[MAXPRINT];
	int pos = 0, ret;

	sprintf(error, "EMUL pid=%d: ", current_pid);	/* # output */
	pos = strlen(error);
#ifdef	__STDC__
	va_start(adx,0);
#else
	va_start(adx);
#endif

loop:
	while ((c = *fmt++) != '%') {
	    if (c == '\0') {
		goto done;
	    }
	    putchar(c);
	}
	c = *fmt++;
	switch (c) {
	    case 'x': case 'X':
		b = 16;
		goto number;
	    case 'd': case 'D':
	    case 'u':
		b = 10;
		goto number;
	    case 'o': case 'O':
		b = 8;
number:
		value = va_arg(adx, u_long);
		if (b == 10 && (long)value < 0) {
		    putchar('-');
		    value = -value;
		}
		cp = prbuf;
		do {
		    *cp++ = "0123456789abcdef"[value%b];
		    value /= b;
		} while (value);
		do {
		    putchar(*--cp);
		} while (cp > prbuf);
		break;
	    case 'c':
		value = va_arg(adx, u_long);
		for (i = 24; i >= 0; i -= 8)
			if (c = (value >> i) & 0x7f)
				putchar(c);
		break;
	    case 's':
		s = va_arg(adx, char *);
		i = 0;
		while (c = *s++) {
		    putchar(c);
		}
		break;
	    case '%':
		putchar('%');
		break;
	}
	goto loop;
done:
	error[pos]='\0';
	va_end(adx);
	/*
	 * DO NOT PUT emul_blocking()/emul_unblocking() HERE!
	 * Used by debug path!
	 */
	return bsd_emulator_error(our_bsd_server_port, error, pos+1);
}

int
e_msleep(serv_port, interrupt, msem)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	msemaphore	*msem;
{
	register int	error;

	/* Indicate we want the semaphore, if it has not been removed: */
	if (msem->msem_wanted != -1)
		msem->msem_wanted = 1;

	/* Don't sleep if the semaphore has been unlocked or removed: */
	if (msem->msem_state != 1 || msem->msem_wanted != 1) {
		/* If the semaphore has been removed, ensure that both fields
		 * remain set to -1 to indicate this:
		 */
		if (msem->msem_state == -1 || msem->msem_wanted == -1) {
			msem->msem_state = -1; msem->msem_wanted = -1;
		}
		return 0;
	}

	emul_blocking();
	error = bsd_msleep(serv_port, interrupt, (int)msem);
	emul_unblocking();
	return error;
}

int
e_mwakeup(serv_port, interrupt, msem)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	msemaphore	*msem;
{
	register int	error;

	/* Indicate we are about to do a wakeup on the semaphore, if it has
	 * not been removed:
	 */
	if (msem->msem_wanted != -1)
		msem->msem_wanted = 0;

	/* If the semaphore has been removed, ensure that both fields
	 * remain set to -1 to indicate this:
	 */
	if (msem->msem_state == -1 || msem->msem_wanted == -1) {
		msem->msem_state = -1; msem->msem_wanted = -1;
	}
	emul_blocking();
	error = bsd_mwakeup(serv_port, interrupt, (int)msem);
	emul_unblocking();
	return error;
}

int
e_profil(serv_port, interrupt, buff, bufsiz, offset, scale, rval)
	mach_port_t	serv_port;
	boolean_t	*interrupt;
	char 	 	*buff;     /* != NULL if set, == NULL if get */
	int		bufsiz;
	int		offset;
	int		scale;
	int		*rval;
{
	int 		error;

	if (buff != NULL) {
	    /*
	     * Set (enable profiling).
	     */ 
	    emul_blocking();
	    error = bsd_profil_set(serv_port, interrupt,
				   (int) buff, bufsiz,
				   offset, scale);
	    emul_unblocking();
	}
	else {
	    /*
	     * Get (disable profiling).
	     */ 
	    char *		out_addr;
	    unsigned int	out_count;
	    char *		user_addr;	/* address used in
						 * the previous 
						 * bsd_profil_set call */
	    int			uaddr;

	    emul_blocking();
	    error = bsd_profil_get(serv_port, interrupt,
				   &uaddr, &out_addr,
				   &out_count);
	    emul_unblocking();
	    
	    if (!error) {
		    user_addr = (char *)uaddr;

		    /*
		     * Copy buffer area to user_addr
		     */
		    if (!user_bcopy(out_addr, user_addr, out_count))
			    error = EFAULT;
		    (void) vm_deallocate(mach_task_self(),
					 (vm_address_t) out_addr,
					 (vm_size_t) out_count);
	    }
	}
	return(error) ; /* profil is supposed to succeed always */
}     

/*
 * This and all "e_xxx_call" routines have machine-dependent pieces
 * in routines named "e_xxx".  As little machine dependent code
 * as possible is included there.
 */
#if ! defined(SLL) && ! defined(SLLCOMPAT)
e_fork_call(serv_port, interrupt, node, new_state, new_state_count, rvalp)
#else /* ! SLL && ! SLLCOMPAT */
e_fork_call(serv_port, interrupt, node, new_state, new_state_count, rvalp,
		force_local)
#endif /* ! SLL && ! SLLCOMPAT */
	mach_port_t		serv_port;
	boolean_t		*interrupt;	/* OUT */
	node_t			node;
	thread_state_t		new_state;
	unsigned int		new_state_count;
	int			*rvalp;		/* OUT */
#if defined(SLL) || defined(SLLCOMPAT)
	boolean_t		force_local;
#endif /* SLL || SLLCOMPAT */
{
	task_t			new_task;
	thread_t		new_thread;
	mach_port_t		new_vproc;
	mach_port_t		new_credentials;
	int 			error;
#if defined(SLL) || defined(SLLCOMPAT)
	boolean_t		was_rfork = 0;
#endif /* SLL || SLLCOMPAT */

	/*
	 * Duplication of the address space and insertion of file port
	 * rights must be done atomically.
	 */
	fdt_atomic_begin();

	/*
	 * Create the child.
	 * NO emul_blocking() - this is NOT A SAFE STOPPING POINT!
	 */
	error = bsd_fork(vproc_port, interrupt, node,
			     new_state, new_state_count,
			     &new_task,
			     &new_thread,
			     &new_vproc,
			     &new_credentials,
#if ! defined(SLL) && ! defined(SLLCOMPAT)
			     &rvalp[0]);
#else /* ! SLL && ! SLLCOMPAT */
			     &rvalp[0],
			     vproc_port,	/* just the NAME is passed */
			     credentials_port,	/* just the NAME is passed */
			     force_local,
			     &was_rfork);
#endif /* ! SLL && ! SLLCOMPAT */
	if (error) {
		fdt_atomic_end();
		return (error);	/* fork wasn't successful */
	}
	rvalp[1] = 0;

	/*
	 * Insert port rights associated with file descriptors into the 
	 * child task.
	 */
	fdt_fork_insert_rights(new_task);
#ifdef PFS
        /*
	 * Insert port rights for cached vnode ports into the 
	 * child task.
	 */
        pfs_vnode_port_cache_insert_rights(new_task);
#endif /* PFS */

	fdt_atomic_end();	/* end of atomic portion */

#if defined(SLL) || defined(SLLCOMPAT)
	if ( ! was_rfork ) {
		/*
		 * Only necessary if we didn't do a rfork().
		 */
#endif /* SLL || SLLCOMPAT */
	/*
	 * Assign port rights to the new vproc and the new credentials
	 * ports to the child task. Do it in a way that we don't
	 * have rights any more in the current task.
	 */
	error = mach_port_insert_right(new_task, 
				       credentials_port, new_credentials, 
				       MACH_MSG_TYPE_MOVE_SEND);
	if (error != KERN_SUCCESS)
		EPRINT(("e_fork_call: cred insert_right=0x%x", error));
	error = mach_port_insert_right(new_task, 
				       vproc_port, new_vproc, 
				       MACH_MSG_TYPE_MOVE_SEND);
	if (error != KERN_SUCCESS)
		EPRINT(("e_fork_call: vproc insert_right=0x%x", error));
#if defined(SLL) || defined(SLLCOMPAT)
	}	/* See if above. */
#endif /* SLL || SLLCOMPAT */

	/* 
	 * Assign port rights for root directory and current working
	 * directory to child task.
	 */
	error = mach_port_insert_right(new_task, 
				       rootdir_port, rootdir_port, 
				       MACH_MSG_TYPE_COPY_SEND);
	if (error != KERN_SUCCESS)
		EPRINT(("e_fork_call: root insert_right=0x%x", error));
	error = mach_port_insert_right(new_task, 
				       currentdir_port, currentdir_port, 
				       MACH_MSG_TYPE_COPY_SEND);
	if (error != KERN_SUCCESS)
		EPRINT(("e_fork_call: cwd insert_right=0x%x", error));

	/*
	 * Start up the initial child thread 
	 */
	error = thread_resume(new_thread);
	if (error != KERN_SUCCESS)
		EPRINT(("e_fork: thread_resume=0x%x",error));

	/* 
	 * We no longer need send rights to the task and thread 
	 */
	(void) mach_port_deallocate(mach_task_self(), (mach_port_t)new_task);
	(void) mach_port_deallocate(mach_task_self(), (mach_port_t)new_thread);
	return(ESUCCESS);
}

#ifdef TNC

#ifdef NX
int
e_migrate_call(serv_port, interrupt, new_node,
               new_state, new_state_count, force)
        mach_port_t             serv_port;
        boolean_t               *interrupt;     /* OUT */
        int                     new_node;
        thread_state_t          new_state;
        unsigned int            new_state_count;
        boolean_t               force;
#else
int
e_migrate_call(serv_port, interrupt, new_node, new_state, new_state_count)
	mach_port_t		serv_port;
	boolean_t		*interrupt;	/* OUT */
	node_t			new_node;
	thread_state_t		new_state;
	unsigned int		new_state_count;
#endif /* NX */
{
	int			error;
	kern_return_t		kr;
	thread_array_t		thread_list;
	mach_msg_type_number_t	thread_count;
	int			i;
	extern node_t		emul_tnc_mynode();

#ifdef NX
        if (!force)
#endif /* NX */
	/*
	 * If we're already on the requested node, immediate success.
	 */
	if (new_node == emul_tnc_mynode()) {
		return(ESUCCESS);
	}

	/*
	 * Currently, we are unable to migrate a multi-threaded process,
	 * due to limitations in the logic for thread_abort(). Hence we
	 * give up here if the process is multithreaded. Note that
	 * multithreaded means > 2 threads, since we must allow for
	 * the emulator callback thread, which always exists.
	 */
	kr = task_threads(mach_task_self(), &thread_list, &thread_count);
	if (kr != KERN_SUCCESS)
		emul_panic("e_migrate_call: task_threads failure");
	for (i = 0; i < thread_count; i++)
		(void) mach_port_deallocate(mach_task_self(), thread_list[i]);
	kr = vm_deallocate(mach_task_self(), 
			   (vm_address_t) thread_list,
			   thread_count * sizeof(*thread_list));
	if (kr != KERN_SUCCESS)
		emul_panic("e_migrate_call: vm_deallocate failure");
	if (thread_count > 2)
		return(ESUCCESS);

	/*
	 * Note that mach_task_self() is just a port NAME, not an
	 * actual port RIGHT. Note that it is normal for bsd_migrate()
	 * to return MIG_SERVER_DIED. This will happen if the server
	 * destroys the reply port prior to the current task being
	 * destroyed.
	 */
	emul_blocking();
	error = bsd_migrate(vproc_port, interrupt,
			    new_node,
			    new_state, new_state_count,
			    vproc_port,		/* just the NAME */
			    credentials_port,	/* just the NAME */
			    mach_task_self());	/* just the NAME */
	emul_unblocking();
	if (error) {
		if (error == MIG_SERVER_DIED) {
			for (;;)
				thread_suspend(mach_thread_self());
		}
		return(error);		/* migrate wasn't successful */
	}

	/*
	 * If we were successful, we shouldn't return here at all.
	 *
	 * Our rights will be extracted by the newly-migrated emulator.
	 */
	EPRINT(("e_migrate_call: erroneous return"));
}

#ifdef NX
int
e_rfork_call(serv_port, interrupt, new_node, new_state, new_state_count,
             force, rvalp)
#else
int
e_rfork_call(serv_port, interrupt, new_node, new_state, new_state_count, rvalp)
#endif /* NX */
	mach_port_t		serv_port;
	boolean_t		*interrupt;	/* OUT */
	node_t			new_node;
	thread_state_t		new_state;
	unsigned int		new_state_count;
#ifdef NX
        boolean_t               force;
#endif /* NX */
	int			*rvalp;		/* OUT */
{
	task_t			new_task;
	thread_t		new_thread;
	int			error;
	extern node_t		emul_tnc_mynode();

	/*
	 * If we're already on the requested node, do a plain fork
	 * instead.  We know the child proc's register setup for both
	 * the fork() and rfork() cases is identical.
	 */
#ifdef NX
        if (!force)
#endif /* NX */
	if (new_node == emul_tnc_mynode()) {
		return(e_fork_call(serv_port, interrupt, NONODE,
#if ! defined(SLL) && ! defined(SLLCOMPAT)
				new_state, new_state_count, rvalp));
#else /* ! SLL && ! SLLCOMPAT */
				new_state, new_state_count, rvalp, TRUE));
#endif /* ! SLL && ! SLLCOMPAT */
	}

	/*
	 * Duplication of the address space and insertion of file port
	 * rights must be done atomically.
	 */
	fdt_atomic_begin();

	/*
	 * Create the child on a remote node.
	 */
	if (error = bsd_rfork(vproc_port, interrupt,
			      new_node,	/* not in bsd_fork call */ 
			      new_state, new_state_count,
			      vproc_port,	/* just the NAME is passed */
			      credentials_port,	/* just the NAME is passed */
			      &new_task,
			      &new_thread,
			      &rvalp[0]) ) {
		fdt_atomic_end();
		return (error);	/* rfork wasn't successful */
	}
	rvalp[1] = 0;

	/* 
	 * Insert port rights associated with file descriptors into the 
	 * child task.
	 */
	fdt_fork_insert_rights(new_task);
#ifdef PFS
        /*
	 * Insert port rights for cached vnode ports into the 
	 * child task.
	 */
        pfs_vnode_port_cache_insert_rights(new_task);
#endif /* PFS */

	fdt_atomic_end();	/* end of atomic portion */

	/* 
	 * Assign port rights for root directory and current working
	 * directory to child task.
	 */
	error = mach_port_insert_right(new_task, rootdir_port, rootdir_port, 
				       MACH_MSG_TYPE_COPY_SEND);
	if (error != KERN_SUCCESS) {
		rvalp[0] = -2;
		goto child_error;
	}
	error = mach_port_insert_right(new_task, currentdir_port,
				       currentdir_port,
				       MACH_MSG_TYPE_COPY_SEND);
	if (error != KERN_SUCCESS) {
		rvalp[0] = -2;
		goto child_error;
	}

	/*
	 * Start up the initial child thread 
	 */
	error = thread_resume(new_thread);
	if (error != KERN_SUCCESS)
		rvalp[0] = -2;

child_error:
	/* 
	 * We no longer need send rights to the task and thread 
	 */
	(void) mach_port_deallocate(mach_task_self(), (mach_port_t)new_task);
	(void) mach_port_deallocate(mach_task_self(), (mach_port_t)new_thread);
	return(error);
}

int
e_rforkmulti_call(serv_port, interrupt, count, node_array, rval_array, 
			pid_array, new_state, new_state_count, rvalp)
	mach_port_t		serv_port;
	boolean_t		*interrupt;	/* OUT */
	unsigned int		*count;
	node_t			node_array[];
	int			rval_array[];	/* OUT */
	pid_t			pid_array[];	/* OUT */
	thread_state_t		new_state;
	unsigned int		new_state_count;
	int			*rvalp;		/* OUT */
{
	unsigned int		child_count;
	unsigned int		out_rval_count;
	unsigned int		out_pid_count;
	unsigned int		nfile_ports;
	unsigned int		num_ports;
	uint_t                  num_vnode_cache_ports;
	mach_port_t		*file_port_array;
	register int		i;
	int			error;
	kern_return_t		ret;

	/*
	 * Check that address parameters are valid:
	 */
	if (!user_rwcheck(count, sizeof *count))
		return(EFAULT);
	if (!user_rcheck(node_array, *count * sizeof(node_t)) ||
	    !user_rwcheck(pid_array, *count * sizeof(pid_t)) ||
	    !user_rwcheck(rval_array, *count * sizeof(int)))
		return(EFAULT);
		
	/*
	 * Duplication of the address space and insertion of file port
	 * rights must be done atomically.
	 */
	fdt_atomic_begin();

	/*
	 * Before giving away port rights, we must contact the fileservers
	 * to keep the bookkeeping straight.
	 */
	fdt_port_modref(*count);

	/*
	 * Get a list of all file ports to be conferred to the children.
	 * This is a two-pass process: first count then get into an array
	 * of adequate size. Note that this list will appear in the
	 * subsequent RPC to the server twice: as names and as rights.
	 */
	nfile_ports = fdt_get_rights(NULL, 0);
	num_ports = nfile_ports;
#ifdef PFS
	num_vnode_cache_ports = pfs_get_vnode_port_cache_rights(NULL);
	num_ports += num_vnode_cache_ports;
#endif /* PFS */
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &file_port_array,
			  (num_ports + 2) * 
			  sizeof(mach_port_t), TRUE);
	if (ret != KERN_SUCCESS) {
		fdt_port_modref(-*count);
		fdt_atomic_end();
		return(ENOMEM);
	}
	file_port_array[0] = rootdir_port;
	file_port_array[1] = currentdir_port;
	(void) fdt_get_rights(file_port_array + 2, nfile_ports);
#ifdef PFS
	(void) pfs_get_vnode_port_cache_rights(file_port_array + 2 + nfile_ports);
#endif /* PFS */
	num_ports += 2;

	/*
	 * Create the children. Use the short in-line form of the call
	 * if possible.
	 */
	child_count = *count;
	if (child_count <= MAX_MULTI_LIST_SIZE &&
	    num_ports <= MAX_MULTI_LIST_SIZE) {
		out_rval_count = child_count;
		out_pid_count = child_count;
		error = bsd_rforkmulti(vproc_port, interrupt,
				       node_array, child_count,
				       rval_array, &out_rval_count,
				       pid_array, &out_pid_count,
				       file_port_array, num_ports,
				       file_port_array, num_ports,
				       new_state, new_state_count,
				       vproc_port,
				       credentials_port);
		if (error != ESUCCESS) {
			fdt_port_modref(-child_count);
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) file_port_array,
					     num_ports*sizeof(mach_port_t));
			fdt_atomic_end();
			return (error);	/* rforkmulti wasn't successful */
		}
	} else {
		int		*new_rval_array = NULL;
		pid_t		*new_pid_array = NULL;
		out_pid_count = 0;
		error = bsd_rforkmulti_long(vproc_port, interrupt,
				            node_array, child_count,
				            &new_rval_array, &out_rval_count,
				            &new_pid_array, &out_pid_count,
				       	    file_port_array, num_ports,
				       	    file_port_array, num_ports,
				            new_state, new_state_count,
				            vproc_port,
				            credentials_port);
		if (error != ESUCCESS) {
			fdt_port_modref(-child_count);
			(void) vm_deallocate(mach_task_self(),
					     (vm_address_t) file_port_array,
					     num_ports*sizeof(mach_port_t));
			fdt_atomic_end();
			return (error);	/* rforkmulti wasn't successful */
		}
		/* copy output params back to user space and deallocate */
		bcopy(new_rval_array, rval_array, child_count*sizeof(int));
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) new_rval_array,
				     child_count*sizeof(int));
		bcopy(new_pid_array, pid_array, child_count*sizeof(pid_t));
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) new_pid_array,
				     child_count*sizeof(pid_t));
	}
	rvalp[0] = 1;
	rvalp[1] = 0;

	(void) vm_deallocate(mach_task_self(),
			     (vm_address_t) file_port_array,
			     num_ports*sizeof(mach_port_t));

	/*
	 * Check how successful this all was and adjust file port refs
	 * for each failure.
	 */
	*count = 0;
	for (i = 0; i < child_count; i++) {
		if (rval_array[i] != ESUCCESS) {
			rvalp[0] = -2;
			continue;
		}
		(*count)++;
	}

	/*
	 * Adjust server's file port count if all children weren't forked
	 */
	if (*count != child_count)
		fdt_port_modref((*count) - child_count);

	fdt_atomic_end();	/* end of atomic portion */

	return(error);
}

#endif	/* TNC */

int
e_exec_call(serv_port, interrupt,
		fname, argp, envp, new_arg_addr, entry, entry_count, traced)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	char		*fname;
	char		**argp;
	char		**envp;
	vm_address_t	*new_arg_addr;	/* OUT */
	vm_address_t	*entry;		/* pointer to OUT array */
	unsigned int	*entry_count;	/* OUT */
	boolean_t	*traced;	/* OUT */
{
	vm_address_t	arg_addr;
	vm_size_t	arg_size;
	int		arg_count, env_count;
	unsigned int	char_count = 0;
	int		error;
	vm_address_t	arg_start;
	cfname_t	cfname;
	cfname_t	cfarg;
	path_name_t	save_fname;
	unsigned int	len_fname;
	mach_port_t	start_port;
	char		*ut_argp, *ut_envp;
	int		ut_arg_size, ut_env_size;
	auxv_mig_t	auxv_structs;
	auxv_strings_t	auxv_strings;
	unsigned int	auxv_structs_count = 
				sizeof(auxv_mig_t) / sizeof(auxv_type_t);
	unsigned int	auxv_strings_count = vm_page_size;

	/*
	 * Copy the argument and environment strings into
	 * contiguous memory.  Since most argument lists are
	 * small, we allocate a page to start, and add more
	 * if we need it.
	 */
	arg_count = 0;
	env_count = 0;

	/*
	 * first check for pathname so that we can return the 
	 * ENAMETOOLONG error correctly
	 */
	if (!user_strlen(fname, &len_fname)) {
		return EFAULT;
	}
	/* check for null path name */
	if (*fname == '\0' && !nullcompat) {
		return ENOENT;
	}

	/* check for pathname exceeding max size */
	if (len_fname > PATH_MAX) {
		return ENAMETOOLONG;
	}
	error = copy_args(argp, &arg_count, envp, &env_count,
			  &arg_addr, &arg_size);
	if (error) {
		return error;
	}

	char_count = arg_size;

	/*
	 * Save the file name in case a command file needs it.
	 * (The file name is in the old program address space,
	 * and will disappear if the exec is successful.)
	 */
	strcpy(save_fname, fname);

	error = vm_allocate(mach_task_self(),
			   (vm_address_t *) &auxv_strings,
			   auxv_strings_count,
			   TRUE);
	if (error) {
		(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);
		return E2BIG;
	}

	/*
	 * Exec the program.  Get back the command file name (if any),
	 * and the entry information (machine-dependent).
	 */
	emul_blocking();
	error = bsd_execve(serv_port, interrupt,
			   save_fname, len_fname + 1,
			   cfname, cfarg,
			   entry, entry_count,
			   auxv_structs, &auxv_structs_count,
			   auxv_strings, &auxv_strings_count,
			   traced);
	emul_unblocking();
	if (error) {
	    (void) vm_deallocate(mach_task_self(), 
					(vm_address_t) arg_addr, 
					arg_size);
	    (void) vm_deallocate(mach_task_self(), 
					(vm_address_t) auxv_strings, 
					auxv_strings_count);
	    return (error);
	}

	/*
	 * Close all the files that were set "close-on-exec".
	 */
	close_on_exec();

	/*
	 * Set up new argument list.  If command file name and argument
	 * have been found, use them instead of argv[0].
	 */
	{
	    register char	**ap;
	    register char	*cp;
	    register char	*argstrings = (char *)arg_addr;
	    register int	total_args;
	    register int	len;
	    char		*cmd_args[4];
	    register char	**xargp = 0;
	    char		*str_ptr;
	    auxv_t		*aptr;
	    int			i;

	    total_args = arg_count + env_count;
	    if (cfname[0] != '\0') {
		/*
		 * argv[0] becomes 'cfname'; skip real argv[0].
		 */
		len = strlen(argstrings) + 1;
		argstrings += len;
		char_count -= len;

		xargp = cmd_args;
		*xargp++ = cfname;
		char_count += (strlen(cfname) + 1);

		if (cfarg[0] != '\0') {
		    *xargp++ = cfarg;
		    char_count += (strlen(cfarg) + 1);
		    total_args++;
		    arg_count++;
		}
		*xargp++ = save_fname;
		char_count += len_fname + 1;
		total_args++;
		arg_count++;

		*xargp = 0;
		xargp = cmd_args;
	    }
	    char_count += auxv_strings_count;
	    char_count = (char_count + NBPW - 1) & ~(NBPW - 1);

	    arg_start = set_arg_addr(total_args*NBPW + 3*NBPW + char_count
					 + NBPW
					 + (auxv_structs_count*sizeof(auxv_t)));
	    ap = (char **)arg_start;
	    ut_argp = cp = (char *)arg_start + total_args*NBPW + 3*NBPW
				+ (auxv_structs_count*sizeof(auxv_t));

	    *ap++ = (char *)(total_args - env_count);
	    for (;;) {

		if (total_args == env_count) {
		    *ap++ = 0;
		    ut_arg_size = cp - ut_argp;
		    ut_envp = cp;
		}
		if (--total_args < 0)
		    break;
		*ap++ = cp;
		if (xargp && *xargp)
		    len = copystr(*xargp++, cp, (unsigned)char_count);
		else {
		    len = copystr(argstrings, cp, (unsigned)char_count);
		    argstrings += len;
		}
		cp += len;
		char_count -= len;
	    }
	    *ap++ = 0;
	    ut_env_size = cp - ut_envp;

	    /*
	     * set up the auxiliary vector
	     */
	    str_ptr = cp;
	    aptr = (auxv_t *)ap;
	    for (i = 0; i < auxv_structs_count; i++) {
		aptr->a_type = auxv_structs[i].a_type;
		switch (aptr->a_type) {
		case	AT_NULL:
		case	AT_IGNORE:
			break;
		case	AT_EXECFD:	/* == AT_EXEC_FD */
		case	AT_PHENT:
		case	AT_PHNUM:
		case	AT_PAGESZ:
		case	AT_FLAGS:
		case	AT_EXEC_LOADER_FLAGS:
			aptr->a_un.a_val = auxv_structs[i].a_un.a_val;
			break;
		case	AT_PHDR:
		case	AT_BASE:
		case	AT_ENTRY:
		case	AT_EXEC_FILENAME:
		case	AT_EXEC_LOADER_FILENAME:
			{
			char	*ptr;

			ptr = (char *)(auxv_strings
					+ auxv_structs[i].a_un.a_ptr);
			aptr->a_un.a_ptr = str_ptr;
			while (*ptr != '\0')
				*str_ptr++ = *ptr++;
			*str_ptr++ = '\0';
			ptr++;
			}
			break;
		default:
			break;
		}
		aptr++;
	    }
	}

#ifdef	STACK_GROWTH_UP
	*new_arg_addr = ((vm_address_t) cp + NBPW - 1) & ~(NBPW - 1);
#else	STACK_GROWTH_UP
	*new_arg_addr = arg_start;
#endif	STACK_GROWTH_UP

	(void) bsd_exec_args_set(serv_port,
				 interrupt,
				 (arg_count == 0) ? 0 : (int) ut_argp,
				 (arg_count == 0) ? 0 : ut_arg_size,
				 (env_count == 0) ? 0 : (int) ut_envp,
				 (env_count == 0) ? 0 : ut_env_size);

	(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);
	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t) auxv_strings, 
			     auxv_strings_count);

#ifdef	TNC
	/*
	 * Establish the emulator's SIGMIGRATE handler
	 */
	{
		extern void emul_sigmigrate_handler();
		extern unsigned emul_sigreturn();
		struct sigaction nsa = {
				emul_sigmigrate_handler,
				-1,
				SA_RESTART};
		int error, rval;
		error = e_sigaction(serv_port, interrupt,
				    SIGMIGRATE,
				    &nsa,
				    NULL,
				    emul_sigreturn,
				    &rval);
		if (error)
			EPRINT(("Unable to set default SIGMIGRATE handler, sigaction returns %d\n", error));
		else {
#ifdef	i386
			extern vm_address_t sigreturnaddr;
			sigreturnaddr = (vm_address_t) emul_sigreturn;
#endif	/* i386 */
		}
#ifdef  CHKPNT
		/*
		 * Save fname and root/cwd context for subsequent checkpoint.
		 */
		(void) emul_chkpnt_exec(interrupt, save_fname, *traced);
#endif  CHKPNT
	}
#endif	/* TNC */

	return (error);
}


int
e_exec_with_loader(serv_port, interrupt, arg1, arg2, arg3, arg4, arg5, rvalp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		arg1, arg2, arg3, arg4, arg5;
	int		*rvalp;		/* OUT */
{
	int		error;
	int 		uarg[5];

	uarg[0] = arg1;			/* set up array for generic call */
	uarg[1] = arg2;
	uarg[2] = arg3;
	uarg[3] = arg4;
	uarg[4] = arg5;

	error = emul_generic(serv_port, interrupt, SYS_exec_with_loader,
			uarg, rvalp);

	/*
	 * If exec successful, close all files that were close-on-exec.
	 */
	if (error != ESUCCESS)
		return(error);

	/*
	 * Close all the files that were set "close-on-exec".
	 */
	close_on_exec();

	return(ESUCCESS);
}

int
e_setrlimit(serv_port, interrupt, which, rlimit)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		which;
	struct rlimit	*rlimit;
{
	int		error;

	error = bsd_setrlimit(serv_port, interrupt, which, rlimit);
	if (error)
		return(error);

#ifdef	MAPPED_FILES
	/*
	 * If the file size limit was changed, update the emulator's 
	 * cached copy.
	 */
	if (which == RLIMIT_FSIZE) 
		rlimit_fsize = rlimit->rlim_cur;
#endif  

	return(ESUCCESS);
}

#ifdef	TNC 

int
e_rexecve_call(serv_port, interrupt,
		fname, argp, envp, new_node, new_state, new_state_count)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	char		*fname;
	char		**argp;
	char		**envp;
	node_t		new_node;
	thread_state_t	new_state;
	unsigned int	new_state_count;
{
	vm_address_t	arg_addr;
	vm_size_t	arg_size;
	int		arg_count, env_count;
	unsigned int	char_count = 0;
	unsigned int	len_fname;
	int		error;
	vm_address_t	arg_start;

	/*
	 * Copy the argument and environment strings into
	 * contiguous memory.  Since most argument lists are
	 * small, we allocate a page to start; copy_args() adds more
	 * if we need it.
	 */
	arg_count = 0;
	env_count = 0;
	error = copy_args(argp, &arg_count, envp, &env_count,
			  &arg_addr, &arg_size);
	if (error) {
		return (error);
	}
	char_count = arg_size;

	/* Check accessbility of file name string: */
	if (!user_strlen(fname, &len_fname)) {
		(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);
		return EFAULT;
	}

	/*
	 * Remotely Exec the program.  We will not return from this call
	 * if it is successful.  Instead we will start on the new node
	 * at location "_rexecve_arrival".  (For rexec to a different
	 * architecture, we will start a fresh emulator on that other
	 * node with a special flag that tells it that it is an
	 * rexec'ing emulator.)  Then the new node's emulator will
	 * call its server with bsd_rexecve_arrival().  This will
	 * _supply_ the new emulator with the info that the bsd_execveve()
	 * routine would return in MiG out parameters, including the
	 * command file name (for rexec'ing a shell script) and the
	 * entry information.
	 * By contrast, we _inherit_ the file port names (and close-on-exec
	 * flags) along with the arg and env strings.
  	 */
	emul_blocking();
	error = bsd_rexecve(vproc_port,
  			    interrupt,
  			    fname, len_fname + 1,
  			    new_node,
  			    new_state, new_state_count,
			    vproc_port,		/* just the NAME */
			    credentials_port,	/* just the NAME */
			    mach_task_self(),	/* just the NAME */
  			    arg_addr, arg_size,
  			    arg_count, env_count, char_count);
	emul_unblocking();
  	if (error) {
  		(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);
  		return (error);		/* rexec wasn't successful */
  	}

	/*
	 * No error on the bsd_rexecve call.  We should never get
	 * here because the server should suspend us.
	 */
	EPRINT(("bsd_rexecve returned OK, shouldn't have returned at all"));
}

int
e_rexecve_arrival_call(serv_port, interrupt,
	 	       new_arg_addrp, entry, entry_countp, traced)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	vm_address_t	*new_arg_addrp;
	vm_address_t	*entry;
	unsigned int	*entry_countp;
	boolean_t	*traced;
{
	vm_address_t	arg_addr;
	vm_size_t	arg_size;
	int		arg_count, env_count;
	int		char_count = 0;
	int		error;
	vm_address_t	arg_start;
	cfname_t	cfname;
	cfname_t	cfarg;
	path_name_t	save_fname;
	unsigned int	save_fname_count = sizeof(save_fname);
	char		*ut_argp, *ut_envp;
	int		ut_arg_size, ut_env_size;
	auxv_mig_t	auxv_structs;
	auxv_strings_t	auxv_strings;
	unsigned int	auxv_structs_count = 
				sizeof(auxv_mig_t) / sizeof(auxv_type_t);
	unsigned int	auxv_strings_count = vm_page_size;

	/*
	 * Allocate space for the auxv_strings that are returned
	 * from rexecve_arrival().
	 */
	(void) vm_allocate(mach_task_self(), 
			   (vm_address_t *) &auxv_strings, 
			   vm_page_size, 
			   TRUE);

	/*
	 * Obtain from our server the info that plain execve would
	 * get back from its bsd_execve() call.  The server generates
	 * that info for us when bsd_rexecve() is called, and saves
	 * it until we, the newly rexec'd emulator, ask for it.
	 */
	emul_blocking();
	error = bsd_rexecve_arrival(serv_port, interrupt,
				    save_fname, &save_fname_count,
				    &arg_addr, &arg_size,
				    &arg_count, &env_count, &char_count,
				    cfname,
				    cfarg,
				    entry, entry_countp,
				    auxv_structs, &auxv_structs_count,
				    auxv_strings, &auxv_strings_count,
				    traced);
	emul_unblocking();
	if (error != KERN_SUCCESS)
		emul_panic("bsd_rexecve_arrival failure");

	/*
	 * Set up new argument list.  If command file name and argument
	 * have been found, use them instead of argv[0].
	 */
	{
	    register char	**ap;
	    register char	*cp;
	    register char	*argstrings = (char *)arg_addr;
	    register int	total_args;
	    register int	len;
	    char		*cmd_args[4];
	    register char	**xargp = 0;
	    char		*str_ptr;
	    auxv_t		*aptr;
	    int			i;

	    total_args = arg_count + env_count;
	    if (cfname[0] != '\0') {

		/*
		 * argv[0] becomes 'cfname'; skip real argv[0].
		 */
		len = strlen(argstrings) + 1;
		argstrings += len;
		char_count -= len;

		xargp = cmd_args;
		*xargp++ = cfname;
		char_count += (strlen(cfname) + 1);

		if (cfarg[0] != '\0') {
		    *xargp++ = cfarg;
		    char_count += (strlen(cfarg) + 1);
		    total_args++;
		    arg_count++;
		}
		*xargp++ = save_fname;
		char_count += (strlen(save_fname) + 1);
		total_args++;
		arg_count++;

		*xargp = 0;
		xargp = cmd_args;
	    }
	    char_count += auxv_strings_count;
	    char_count = (char_count + NBPW - 1) & ~(NBPW - 1);

	    arg_start = set_arg_addr(total_args*NBPW + 3*NBPW + char_count
					 + NBPW
					 + (auxv_structs_count*sizeof(auxv_t)));
	    ap = (char **)arg_start;
	    ut_argp = cp = (char *)arg_start + total_args*NBPW + 3*NBPW
				+ (auxv_structs_count*sizeof(auxv_t));

	    *ap++ = (char *)(total_args - env_count);
	    for (;;) {

		if (total_args == env_count) {
		    *ap++ = 0;
		    ut_arg_size = cp - ut_argp;
		    ut_envp = cp;
		}
		if (--total_args < 0)
		    break;
		*ap++ = cp;
		if (xargp && *xargp)
		    len = copystr(*xargp++, cp, (unsigned)char_count);
		else {
		    len = copystr(argstrings, cp, (unsigned)char_count);
		    argstrings += len;
		}
		cp += len;
		char_count -= len;
	    }
	    *ap++ = 0;
	    ut_env_size = cp - ut_envp;

	    /*
	     * set up the auxiliary vector
	     */
	    str_ptr = cp;
	    aptr = (auxv_t *)ap;
	    for (i = 0; i < auxv_structs_count; i++) {
		aptr->a_type = auxv_structs[i].a_type;
		switch (aptr->a_type) {
		case	AT_NULL:
		case	AT_IGNORE:
			break;
		case	AT_EXECFD:	/* == AT_EXEC_FD */
		case	AT_PHENT:
		case	AT_PHNUM:
		case	AT_PAGESZ:
		case	AT_FLAGS:
		case	AT_EXEC_LOADER_FLAGS:
			aptr->a_un.a_val = auxv_structs[i].a_un.a_val;
			break;
		case	AT_PHDR:
		case	AT_BASE:
		case	AT_ENTRY:
		case	AT_EXEC_FILENAME:
		case	AT_EXEC_LOADER_FILENAME:
			{
			char	*ptr;

			ptr = (char *)(auxv_strings
					+ auxv_structs[i].a_un.a_ptr);
			aptr->a_un.a_ptr = str_ptr;
			while (*ptr != '\0')
				*str_ptr++ = *ptr++;
			*str_ptr++ = '\0';
			ptr++;
			}
			break;
		default:
			break;
		}
		aptr++;
	    }
	}

#ifdef	STACK_GROWTH_UP
	*new_arg_addrp = ((vm_address_t) cp + NBPW - 1) & ~(NBPW - 1);
#else	STACK_GROWTH_UP
	*new_arg_addrp = arg_start;
#endif	STACK_GROWTH_UP

	/*
	 * Inform our server of the addresses and sizes of the items
	 * below.  (This info is used for ps.)
	 */
	emul_blocking();
	(void) bsd_exec_args_set(our_bsd_server_port,
				 interrupt,
				 (arg_count == 0) ? 0 : (int) ut_argp,
				 (arg_count == 0) ? 0 : ut_arg_size,
				 (env_count == 0) ? 0 : (int) ut_envp,
				 (env_count == 0) ? 0 : ut_env_size);
	emul_unblocking();

	/*
	 * Deallocate the extra memory that the old emulator had
	 * allocated (and which we inherited) where these strings
	 * had been stored before we re-arranged them into a normal
	 * startup stack for the user program that we'll go to when
	 * we finish here.
	 */
	(void) vm_deallocate(mach_task_self(), arg_addr, arg_size);

	/*
	 * Deallocate the auxv_strings area that we allocated earlier
	 * during this call.
	 */
	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t) auxv_strings, 
			     vm_page_size);

	/*
	 * Establish the emulator's SIGMIGRATE handler
	 */
	{
		extern void emul_sigmigrate_handler();
		extern unsigned emul_sigreturn();
		struct sigaction nsa = {
				emul_sigmigrate_handler,
				-1,
				SA_RESTART};
		int error, rval;
		error = e_sigaction(our_bsd_server_port, interrupt,
				    SIGMIGRATE,
				    &nsa,
				    NULL,
				    emul_sigreturn,
				    &rval);
		if (error)
			EPRINT(("Unable to set default SIGMIGRATE handler, "
				"sigaction returns %d\n", error));
		else {
#ifdef	i386
			extern vm_address_t sigreturnaddr;
			sigreturnaddr = (vm_address_t) emul_sigreturn;
#endif	/* i386 */
		}
#ifdef  CHKPNT
		/*
		 * Save fname and root/cwd context for subsequent checkpoint.
		 */
		(void) emul_chkpnt_exec(interrupt, save_fname, *traced);
#endif  CHKPNT
	}
	return(ESUCCESS);
}

int
e_set_tnc_port(serv_port, interrupt, id, port)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		id;
	mach_port_t	port;
{
	return (bsd_set_tnc_port(serv_port, interrupt, id, port));
}

int
e_get_tnc_port(serv_port, interrupt, id, node, portp)
	mach_port_t	serv_port;
	boolean_t	*interrupt;	/* OUT */
	int		id;
	node_t		node;
	mach_port_t	*portp;		/* OUT */
{

	/*
	 * check that address parameters are valid:
	 */
	if (!user_rwcheck(portp, sizeof *portp))
		return(EFAULT);
	return (bsd_get_tnc_port(serv_port, interrupt, id, node, portp));
}

node_t
e_node_self(proc_port, interrupt, rvalp)
	mach_port_t	proc_port;
	int		*interrupt;
	int		*rvalp;
{
	extern node_t	emul_tnc_mynode();

	*rvalp = emul_tnc_mynode();
	return(ESUCCESS);
}

node_t
emul_tnc_mynode()
{
	kern_return_t	ret;
	extern node_t	tnc_mynode;

	if (tnc_mynode == (node_t) -1) {
#ifdef NX
                ret = NX_node_self(vproc_port, &tnc_mynode);
                if (tnc_mynode != (node_t) -1)
                    return(tnc_mynode);
#endif /* NX */
		ret = norma_node_self(mach_task_self(), &tnc_mynode);
		if (ret != KERN_SUCCESS)
			EPRINT(("emul_tnc_mynode failure, ret = 0x%x", ret));
	}
	return(tnc_mynode);
}

int
e_table_node(serv_port, interrupt, node, id, index, addr, nel, lel, rval)
	mach_port_t	serv_port;
	node_t		node;
	boolean_t	*interrupt;
	int		id;
	int		index;
	char *		addr;
	int		nel;	/* >0 ==> get, <0 ==> set */
	unsigned int	lel;
	int		*rval;
{
	register int	error;
	int		nel_done;

	if (nel < 0) {
		/*
		 * Set.
		 */
		if (!user_rcheck(addr, -nel * lel))
			error = EFAULT;
		else {
			emul_blocking();
			error = bsd_table_node_set(serv_port, interrupt,
				node, id, index, lel, nel,
				addr, -nel*lel,
				&nel_done);
			emul_unblocking();
		}
	}
	else {
	    char *		out_addr;
	    unsigned int	out_count;

	    emul_blocking();
	    error = bsd_table_node_get(serv_port, interrupt,
			node, id, index, lel, nel,
			&out_addr, &out_count,
			&nel_done);
	    emul_unblocking();

	    if (error == 0) {
		/*
		 * Copy table to addr
		 */
		if (!user_bcopy(out_addr, addr, lel * nel_done))
			error = EFAULT;
		(void) vm_deallocate(mach_task_self(),
				     (vm_address_t) out_addr,
				     (vm_size_t) out_count);
	    }
	}
	if (error == 0)
	    *rval = nel_done;
	return (error);
}

#endif	/* TNC */
