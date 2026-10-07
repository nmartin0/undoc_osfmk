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
 * $Log: dvp_vpops.c,v $
 * Revision 1.51  1995/04/07  23:57:45  yazz
 *  Reviewer: Ray Shapouri, Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 12895
 *  Testing: EATs controlc, sched, os_interfaces, MUNOPS SAT runs
 *  Module(s): server/tnc/dvp_vpops.c
 * Made use of new vm_allocate/deallocate_strict() routines and dealloc'd
 * OOL memory from nx_get_info(), which had been leaking.
 *
 * Revision 1.50  1995/03/14  01:55:00  raya
 * Modified original fix for PTS-12465 to call VPOP_REPORT_STATE()
 * in order to reset the PV_NX_NO_SIGCHLD flag rather than resetting
 * it directly.  VPOP_REPORT_STATE() will update the value of the
 * flag on both the local node as well as the node on which the
 * parent process is running.
 *
 *  Reviewer:		Johannes
 *  Risk:			Low
 *  Benefit or PTS #:	12465
 *  Testing:		Selective developer testing
 *  Module(s):		dvp_pvpops.c dvp_vpops.c vproc.h
 *
 * Revision 1.49  1995/02/24  22:49:41  stans
 *  lint picking
 *
 *  Reviewer:self
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW07 sats
 *
 * Revision 1.48  1995/02/18  01:24:31  yazz
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
 * Revision 1.47  1995/02/10  23:49:09  yazz
 *  Reviewer: Jerry Toman
 *  Risk: Lo
 *  Benefit or PTS #: instrumentation to catch #11366 C-0 (not a fix)
 *  Testing: EATs controlc
 *  Module(s): server/tnc/dvp_vpops.c
 * 	    server/uxkern/proc_to_task.c
 * For assertful servers, added debug printfs for unusual vproc situations.
 * Added debug routine to print more info for vproc-related panics elsewhere.
 *
 * Revision 1.46  1995/02/01  21:42:32  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.45  1994/12/22  18:27:24  nandy
 * n dvpop_wait, if pid is > 0 and the process is not ready
 * to be reaped, try again instead of returning ECHILD.
 *
 *  Reviewer: jlitvin
 *  Risk: L
 *  Benefit or PTS #: 11762
 *  Testing: VSX test
 *  Module(s): tnc/dvp_vpops.c
 *
 * Revision 1.44  1994/12/18  03:47:52  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11832 C-1
 *  Testing:  EATs controlc, concur, sched
 *  Module(s): server/tnc/dvp_vpops.c
 * Take a ref on the pproc struct before pproc_psignal() call on that proc.
 *
 * Revision 1.43  1994/11/19  01:30:16  jlitvin
 * Back out the fix for PTS #11059.  In fact, we increased the delay to
 * 30,000 since the original delay of 3000 wasn't helping on 1024 nodes.
 *
 *  Reviewer: suri
 *  Risk: low
 *  Benefit or PTS #: 11676
 *  Testing: rrands/scotth hang-on-exit parallel apps ran OK
 *  Module(s): server/tnc/dvp_vpops.c
 *
 * Revision 1.42  1994/11/18  20:43:19  mtm
 * Copyright additions/changes
 *
 * Revision 1.41  1994/09/29  23:19:32  yazz
 *  Author of fix: Chris Peak
 *  Reviewer: Bob Yasi, Nandini Ajmani
 *  Risk: Medium
 *  Benefit or PTS #: 9346 (plus 10485, 6266, 7916, 9265, 8194)
 *  Testing: control-c EAT case 3 (20 iterations), other EATs os_interfaces,
 * 		xtrnl, controlc, rmcall, fileio, message
 *  Module(s):
 * 	server/sys/vproc.h
 * 	server/uxkern/proc_to_task.h
 * 	server/uxkern/proc_to_task.c
 * 	server/tnc/dvp_vpops.c
 * 	server/tnc/rvp_subr.c
 *
 * Abolish the renaming of vproc ports for send rights on remote nodes.
 * No longer try to prevent the arrival of new vproc ports during dvpop_free().
 *
 * Revision 1.40  1994/09/27  15:53:52  jlitvin
 * A temporary fix was added to the server's exit function to workaround
 * an old NORMA many-to-one bug.  Now that we have the new DIPC, we no
 * longer need this kludge.
 *
 *  Reviewer: robboy
 *  Risk: low
 *  Benefit or PTS #: 11059
 *  Testing: gsync(); exit() test on 512 nodes
 *  Module(s): server/tnc/dvp_vpops.c
 *
 * Revision 1.39  1994/08/31  22:47:11  mtm
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
 * Revision 1.37.2.1  1994/08/03  15:52:10  nandy
 * Merged from the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.38  1994/08/03  15:37:59  nandy
 * dvpop_fork() was not inheriting the trace flags.
 *
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 10047
 *  Testing: PTS test case
 *  Module(s): nx/nx.c
 * 	tnc/dpvproc.h
 * 	bsd/kern_sig.c
 * 	bsd/kern_exit.c
 *
 * Revision 1.37  1994/06/23  22:26:42  jlitvin
 * Fix RCS comments.
 *
 * Revision 1.36  1994/06/22  16:47:28  nandy
 * Moved nx_signal_tam() rom dvpop_wait to dvpop_report_state(). This
 * had to be done because local state is not set beore VPOP_REPORT_STATE()
 * is called.
 *
 *  Reviewer: Chris Peak
 *  Risk: M
 *  Benefit or PTS #: 9854
 *  Testing: Eats
 *  Module(s): dvp_vpops.c
 *
 * Revision 1.35  1994/06/15  14:49:35  cfj
 * Modified dvpop_report_state() so that VPOP_REPORT_STATE(v, VPROC_NO_SIGCHLD)
 * only causes the PV_NX_NO_SIGCHLD be set.
 *
 * Revision 1.34  1994/06/02  22:28:26  chrisp
 * In dpvpop_reap(), perform PVPOP_RMV_PGRP_LIST() for zombie child
 * only if child's pgrp leader is not its parent; return child pgid.
 * In dvpop_wait(), analyze pgid returned for zombie child and call
 * PVPOP_RMV_PGRP_LIST() if this is the parent pid.
 *
 * Support added for waitmulti() - viz: elder reporting and reap multi
 * operation (refer to rtask_cli_vproc.c).
 *
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #: 6463
 *  Testing:
 *  Module(s): dpvproc.h dvp_pvpops.c dvp_vpops.c pvp.ops pvps.ops rtask.h
 * 	    rtask_cli_vproc.c rtask_server.c rtask_svr_vproc.c
 * 	    spanning_tree.c tnc_async.defs tnc_server_side.c
 * 	    tnc_types.defs tnc_types.h tnc_types_gen.c
 *
 * Revision 1.33  1994/05/24  20:38:48  yazz
 * When multiple threads are going thru dvpop_free(), ensure that only the
 * 1st such thread actually disables incoming server requests and that only
 * the last such thread actually re-enables them.  This prevents the unexpected
 * arrival of vproc ports via such incoming server request messages.  See also
 * #9302.
 *
 *  Reviewer: Roman Zajcew, Chris Peak, Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: #9346
 *  Testing: basic; no specific testcase
 *  Module(s): server/tnc/dvp_vpops.c
 *
 * Revision 1.32  1994/05/03  23:35:00  dbm
 * Merge of R1.2 version 1.23.2.5
 *
 * Revision 1.31  1994/04/28  19:14:10  chrisp
 * tncgen: autugenerated code now derived from *.ops definition files
 * and processed by tncgen and associated gen*.sh scripts. #ifdefs
 * introduced where previously they were ignored.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds, builds, builds.
 *  Module(s):
 *      Modified Files:
 *  	dpvproc.h dvp_pvpsops.c dvp_vpops.c rvp_pvpops.c
 *  	rvp_pvpops_server.c rvp_pvpsops.c rvp_pvpsops_server.c
 *  	tnc.defs tnc_mig.c
 *      Added Files:
 *  	gen_client_stubs.sh gen_mig.sh gen_migrate_wrappers.sh
 *  	gen_server_stubs.sh pvp.ops pvps.ops tncgen.mk
 *      Removed Files:
 *  	dpvproc_struct.h makeTNCtables.sh
 *
 * Revision 1.30  1994/04/22  18:33:39  dbm
 * Mainline merge of R1.2 version 1.23.2.4
 *
 * Revision 1.29  1994/03/19  00:44:38  jlitvin
 * Fix minor merge problem.
 *
 * Revision 1.28  1994/03/18  20:23:29  nandy
 * Merged from R1_2 branch.
 *
 * Revision 1.27  1994/03/16  23:23:16  jlitvin
 * Fix very minor merge problem.
 *
 * Revision 1.26  1994/03/14  02:05:07  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *
 * Revision 1.25  1994/02/16  15:20:07  nandy
 * Merged from R1_2 branch.
 *
 * Revision 1.24  1994/01/12  19:13:59  cfj
 * Merge bug fix from R1_2.
 *
 * Revision 1.23.2.5  1994/05/03  23:26:18  dbm
 * Fixed a coding error when the original fix for #8729 was put in.
 *
 *  Reviewer: John Litvin
 *  Risk:Low
 *  Benefit or PTS #: 8729
 *  Testing: VSX, IPD, Fileio Eats.
 *  Module(s):
 * 	dvp_vpops.c
 *
 * Revision 1.23.2.4  1994/04/22  18:31:45  dbm
 * Added check for return of VPOP_GET_ATTR()
 *  Reviewer: Charlie Johnson
 *  Risk: Low
 *  Benefit or PTS #: 8729
 *  Testing: Specific test case, VSX Eats.
 *  Module(s):
 *         bsd/tty.c
 *         tnc/dvp_pvpops.c
 *         tnc/dvp_vpops.c
 * 	nx/nx.c
 *
 * Revision 1.23.2.3  1994/03/18  20:13:22  nandy
 * Arguments to has_sctty__using_cache() from  were dvpop_ctty_getattr()incorrect.
 *
 *  Reviewer: cfj
 *  Risk: L
 *  Benefit or PTS #: 8607
 *  Testing: NX applications
 *  Module(s): tnc/dvp_vpops.c
 *
 * Revision 1.23.2.2  1994/02/08  19:52:02  nandy
 * In dvpop_wait(), do not change the state on the parent node if the
 * process already has a tam. Call to REPORT_STATE() is moved to
 * nx_tam_wait().
 *
 *  Reviewer: Chris Peak
 *  Risk: MEDIUM
 *  Benefit or PTS #: 8084
 *  Testing: OS interface eats and IPD eats on 16 node paragon
 *  Module(s):  nx.c dvp_vpops.c
 *
 * Revision 1.23.2.1  1994/01/12  19:09:23  cfj
 * Modified dvpop_fork() so that the call to
 * pproc_set_attr() is made before the call to dvp_child_join_pgrp_end() to
 * avoid trying to use the p->p_vproc field before it was initialized.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: L
 *  Benefit or PTS #: 7655
 *  Testing: Entered test case.
 *  Module(s): server/tnc/rtask_svr_vproc.c
 *             server/tnc/dvp_vpops.c
 *
 * Revision 1.23  1993/11/17  19:03:06  nandy
 * Store the foster parent after the vproc is locked in dvpop_exit()
 *
 *  Reviewer: yazz
 *  Risk: Low
 *  Benefit or PTS #: 7145
 *  Testing: Ran the test that used to show this bug
 *  Module(s): dvp_vpops.c
 *
 * Revision 1.22  1993/11/03  19:16:47  yazz
 * Establish a sequence number mechanism whereby process group signals, such as
 * those generated by CTRL/C, are guaranteed to be delivered to child processes
 * of a reproducing (fork(), rfork(), rforkmulti(), etc.) task.  Many unixes
 * have a timing window where new child procs can miss out on a pgrp-style signal.
 *
 * Revision 1.21  1993/11/03  16:10:28  nandy
 * pproc_exit() can go to sleep while waiting for other server threads
 * to terminate. Delay taking exclusive lock on the vproc in
 * dvpop_exit() till after pproc_exit().
 *
 * PTS:	6794
 * Reviewer:	cfj, jlitvin
 * Risk:		medium
 * testing:	Done.
 *
 * Revision 1.20  1993/11/01  19:33:37  robboy
 * Added variable delay loop to dvpop_exit().  Avoids excessive all-to-one
 * messages in case many nodes exit at once.
 *
 * Revision 1.19  1993/10/21  23:34:26  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.18  1993/10/11  13:13:28  nandy
 * Added uid and ruid to dvpop_get_attr().
 *
 * Revision 1.17  1993/07/14  18:32:49  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:44:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.16  1993/06/09  00:09:28  cfj
 * Change occurances of #include <i860ipsc/mcmsg> to #include <i860paragon/mcmsg>
 *
 * Revision 1.15  1993/06/07  23:35:10  nandy
 * Prevent exit code from ever executing twice
 *
 * Revision 1.14  1993/05/06  19:22:13  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:45:05  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.13  1993/04/19  20:45:41  cfj
 * Merge with T9.5.
 *
 * Revision 1.10.4.2  1993/04/19  20:17:28  cfj
 * Added the vproc of the caller as a parameter to 
 * PVPOP_ADJUST_JOB_CONTROL_COUNT() so that we can check and not 
 * send a signal to the calling process.
 *
 * Revision 1.12  1993/04/03  03:08:34  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.11  1993/03/13  03:14:53  cfj
 * Merge from T9.
 *
 * Revision 1.10.4.1  1993/03/13  03:10:52  cfj
 * Fix a problem in dvpop_exit() in dvp_vpops.c so that the exclusive lock
 * on the vproc until after the process is marked a ZOMBIE.  Also fixed a
 * problem in dpvpop_rmv_pgrp_list() in dvp_pvpops.c so that the vproc is
 * not released until after all references to the vproc data are made.
 *
 * Revision 1.1.2.2.2.2  1993/02/16  20:05:51  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.10  1993/02/01  20:03:08  nandy
 * nx_report_allocator() is no longer called from dvpop_exit().
 *
 * Revision 1.9  1993/01/29  17:18:56  nandy
 * Fixed the way RPC is sent to the alocator when proxy exits.
 *
 * Revision 1.8  1993/01/26  18:17:23  cfj
 * unlock the VPROC_LOCK_FLAG before calling report_state()
 *
 * Revision 1.7  1993/01/21  19:17:57  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.1.2.2.2.1  1993/01/09  00:05:27  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal 7.
 *
 * Revision 1.6  1993/01/05  20:01:31  shala
 * Added necessary ifdef NX for the last change.
 *
 * Revision 1.5  1993/01/05  19:43:27  shala
 * While exiting, a proxy process reports to the allocator.
 *
 * Revision 1.4  1992/12/18  17:28:21  nandy
 * The pvp_flag is set to SZOMBIE before report_state is called in dvpop_exit().
 *
 * Revision 1.3  1992/11/30  22:47:14  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:31:06  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.49  93/09/16  08:54:42  chrisp
 * [SPE 0030] Generic Spanning Trees: pvproc_ops dispatch table becomes
 * 	pvproc_ops_vector to include asyncchronous modes of operation.
 * 
 * Revision 3.48  93/06/22  18:09:20  yazz
 * [ Bug #0288 ] When an exiting process obtains its parent's vproc ptr (so
 * 	it can trigger parent's wait() call behavior), use LOCATE_VPROC_-
 * 	PID macro instead of VPROCPTR macro.  This guarantees that the
 * 	parent's vproc is held, and will not disappear unexpectedly.
 * 
 * Revision 3.47  93/06/19  15:16:24  yazz
 * [ ad1.04 merge ]
 * 	Add new node parameter for plain fork().
 * 
 * Revision 3.46  93/06/03  18:54:36  yazz
 * [Bug #0282] Prevent VPOP_EXIT (dvpop_exit()) from ever performing its work
 * 	more than once on the same vproc.
 * 
 * Revision 3.45  93/04/22  16:38:57  roman
 * [Bug 222] Waiting for pgid has problems. Included reorganizing the
 * 	vpop_wait code for clarity.
 * 
 * Revision 3.44  93/03/15  15:38:40  roman
 * [Bug 0201] Change locking for VPOP_EXIT() to ensure that child reports
 * 	zombie state to "correct" parent (i.e. make sure that the
 * 	parent does not change during VPOP_EXIT()).
 * 
 * Revision 3.43  93/01/05  09:41:08  chrisp
 * [Bug #139] Add a lock to guard the generation of PIDs (and thus prevent
 * 	the same PID being assigned to different processes).
 * [Bug #137] In dvpop_free(), clear PV_HAS_PORT_RIGHT flag in addition
 * 	to PV_IS_LOCAL and PV_HAS_REMOTE_RIGHT.
 * 
 * Revision 3.42  92/10/28  15:11:19  roman
 * Include procedure prototypes generated by makeTNCsyscalls.sh for better
 * 	type-checking.
 * 
 * Revision 3.41  92/10/14  08:24:24  chrisp
 * [Bug #68] There's now no need to call pproc_clear_sleep_wait()
 * 	after a call to pproc_sleep_waiting().
 * Remove fatuous return after goto!
 * 
 * Revision 3.40  92/10/09  09:58:26  roman
 * Add procedure prototypes for clean i860 compilations.
 * Minor formatting fix.
 * 
 * Revision 3.39  92/10/08  17:03:19  chrisp
 * Change names of operations tables for consistency.
 * Add a second parameter to vproc_port_allocate() to indicate whether
 * 	a vproc has a receive right.
 * 
 * Revision 3.38  92/09/30  10:52:35  chrisp
 * [Bug #59] Modify the wait loop to ensure that a child cannot issue
 * 	a pproc_wakeup() for its parent (as a result of a SIGCHLD)
 * 	while the parent is scanning its parent-sibling-child list
 * 	and before calling pproc_sleep_waiting(). This involves
 * 	the use of 2 new pproc routines:  pproc_assert_sleep_wait()
 * 	and pproc_clear_sleep_wait().
 * 
 * Revision 3.37  92/09/29  07:59:10  roman
 * Have code invoke (new) pricate virtual process system operations (pvps ops)
 * 	when doing operations directiy to a specific node. Related
 * 	change to parameters to ctty_getattr vproc op.
 * Add new vproc op for terminal_sigpgrp.
 * Change code for setpinit vproc op to do all the work in-line (pvproc op
 * 	not really needed).
 * Change vproc op set_state to report_state.
 * Get rid of killall vproc op (replaced by pvps op).
 * Change all lock invocations to use the new locking macros.
 * Change all references to pvproc field names to use the new (better,
 * 	more consistant) names.
 * Change all reference from "site" to "node".
 * Add the concept of foster children to the system (shildren of
 * 	init that give their resource usage statistics to a foster
 * 	parent rather than init, the real parent). Involves changes
 * 	to exit vproc op.
 * 
 * Revision 3.36  92/09/21  14:12:26  chrisp
 * In dvpop_free(), turn off port right bits when freeing a vproc.
 * 
 * Revision 3.35  92/08/25  14:49:06  chrisp
 * Fix for bug #51. In dvpop_set_ctty() an RPC to the ctty node is
 * required to release an exiting session leader's ctty when the node
 * recorded in the pvproc is neither -1 nor the local node.
 * 
 * Revision 3.34  92/07/30  16:09:27  chrisp
 * Before deallocating a vproc, ensure that the port is idle ... that is,
 * 	no message has been received but not fully processed.
 * 
 * Revision 3.33  92/07/08  09:08:08  roman
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.32  92/06/17  09:13:20  roman
 * [Bug 0026] New vproc op VPOP_GET_TASK_PORT() added to get task_by_pid()
 *	system call working.
 * 
 * Revision 3.31  92/05/29  10:40:26  chrisp
 * Don't panic if tnc_get_server_port() fails when attempting to contact
 * a remote ctty node in dvpop_ctty_getattr(). Instead, return ENOTTY.
 * 
 * Revision 3.30  92/05/18  12:41:59  chrisp
 * [Bug #15] Set zombie state after all exit processing has been completed.
 * 	This avoids the possibility of the parent attempting to reap too soon.
 * 
 * Revision 3.29  92/05/07  10:45:42  chrisp
 * [Bug #10] Add new extra argument of 0 in call to PVPOP_SIGPROC()
 * 	in dvpop_sigproc().
 * 
 * Revision 3.28  92/05/06  09:21:55  chrisp
 * Add calls to VPROC_[UN]LOCK_FLAG to guard updates to pvproc field pvp_flag.
 * Re-order vproc locking and call to pproc_exit() in dvpop_exit() to close
 * 	window.
 * Optimize dvpop_set_state() not to invoke a remote operation if in fact
 * 	there's no state change.
 * 
 * Revision 3.27  92/04/30  10:49:22  chrisp
 * [Bug #3] PVPOP_REAP() now takes an additional parameter requiring a reaped
 * 	process belong to a specified pgid (if not WAIT_ANY); error ESRCH
 * 	is returned if this is not the case and the process is not reaped.
 * 	This avoids VPOP_WAIT() using PVPOP_GETATTR() for each child's pgid.
 * 
 * Revision 3.26  92/04/27  11:29:46  roman
 * Fix waitpid() to work when waiting for a specific pid.
 * 
 * Revision 3.25  92/04/22  12:32:17  chrisp
 * Alter existence rules for session leader vproc, mark:
 * 	- on node of each pgrp leader in session;
 * 	- on node of session leader for each pgrp in session list.
 * In fork(), no longer mark session leader for each sessaion member process.
 * 
 * Revision 3.24  92/04/22  08:48:20  chrisp
 * In fork(), take parent and child vproc locks before adding child to hash
 *	list, and set up op tables before this also.
 * 
 * Revision 3.23  92/03/18  12:13:36  roman
 * Add locking (major changes) to support multi-threaded interruptible
 * server.
 * 
 * Revision 3.22  92/03/12  15:23:34  roman
 * Major changes to the way orphaned pgrps are tracked.
 * Move several REMOVE_CHILD_FROM_PARENT pvproc ops in-line, taking
 * advantage of knowledge of the current state of the
 * parent-child-sibling list.
 * Fix bug in vproc reference counting with respect to reassigning
 * children of exiting process to init.
 * Restructure PVPOP_FREE() to take into account that vprocs now always
 * keep around a send right, even if a receive right is present.
 * Numerous optimizations to get code to use an existing pvproc pointer
 * rather than dereferencing the vproc pointer.
 * The vproc hash lists now protected by "vproc_list_lock" rather than
 * "vproc_hash_lock".
 * Change code to reflect the fact the tnc_get_server_port() returns
 * errors in the "errno.h" space rather than the Mach error space.
 * 
 * Revision 3.21  92/03/03  16:20:11  chrisp
 * Add ADD_CHILD_TO_PARENT remote op to support single remote init architecture.
 * Substantial changes to ctty operations in order to use pid/sctty caches on
 * the node of the controlling tty (in the session vproc).
 * 
 * Revision 3.20  92/02/20  09:19:13  roman
 * Get rid of "register" declaration for "sid" variable that was referenced
 * as "&sid" (got rid of compiler warning).
 * Minor formatting fixup.
 * 
 * Revision 3.19  92/02/14  08:45:26  roman
 * Turn on the PV_IS_LOCAL flag for the vproc port when a vproc
 * for a new process is initially created.
 * 
 * Revision 3.18  92/02/10  14:31:08  chrisp
 * There was a reason to do the resign pgrp before the reap - do it remotely
 * on the child node from dpvpop_reap() before releasing the child. This also
 * saves an RPC.
 * 
 * Revision 3.17  92/02/10  13:08:30  chrisp
 * During reaping, temporarily increment the zombie vproc's reference count
 * until the reap and relationship list updates are complete.
 * 
 * Revision 3.16  92/01/30  14:18:50  chrisp
 * Add ptrace vproc op.
 * Correct reaping in the case of stopped (traced) children.
 * 
 * Revision 3.15  92/01/28  15:51:18  roman
 * Change some indenting for pretification.
 * Print out return codes when mach_port_get_refs() fails.
 * 
 * Revision 3.14  92/01/16  11:41:57  chrisp
 * VPROC_RELEASE of parent during reap moved to node of the reaped child.
 * 
 * Revision 3.13  92/01/15  10:26:55  chrisp
 * Re-organize dvpop_free to free origin node's (original) vproc if remote
 * from origin node.
 * 
 * Revision 3.12  92/01/10  10:08:07  roman
 * Fix up code for the VPOP_FREE() operation to remove the tnc_server_port
 * from the port set when we want to really make sure we can get
 * rid of a vproc.
 * 
 * Revision 3.11  91/12/24  10:28:39  roman
 * Add extensive code to dpvpop_free() to make sure the vproc is not freed
 * if there are any send rights to the vproc port outstanding that have
 * not yet been processed.
 * 
 * Revision 3.10  91/12/20  16:43:30  chrisp
 * Orphan pgrp handling revamped.
 * SETPGID becomes a PVPOP.
 * RESIGN_PGRP no longer is a VPOP.
 * 
 * Revision 3.9  91/12/13  11:02:38  roman
 * Fix bug during exit where it is possible for child vproc to be freed
 * prior to a future reference.
 * Fix bug in reap where vproc can be freed prior to being removed from
 * prgp.
 * 
 * Revision 3.8  91/12/04  07:58:24  chrisp
 * Correct some instances of !x&y needind to be !(x&y).
 * Vpops SET_CTTY and CTTY_GETATTR now have pvpops for remote controlling node.
 * Vpop exit corrected to prevent hang on exit of session leader without a
 * controlling terminal.
 * 
 * Revision 3.7  91/11/27  09:43:27  chrisp
 * Change dvpop_wait - it was assuming that all child vprocs contained valid
 * pgid fields. A (potentially) remote operation is called ONLY if a pgrp id
 * has been specified rather than a process id.
 * 
 * Revision 3.6  91/11/26  11:58:56  chrisp
 * setsid becomes a pvpop.
 * 
 * Revision 3.5  91/11/22  10:24:41  chrisp
 * vpop_set_stop_state generalized to vpop_set_state.
 * HOLD/RELEASE message tags altered to clarify location of operation.
 * 
 * Revision 3.4  91/11/19  10:12:23  chrisp
 * pproc_set_attr now take 7 args.
 * 
 * Revision 3.3  91/11/18  11:33:22  chrisp
 *  Controlling terminal changes.
 * 
 * Revision 3.2  91/11/01  17:16:05  roman
 * Change assumptions for when vproc_port_deallocate is done (now done
 * when vproc's reference count goes to zero). Add new VPOP_FREE vproc op.
 * Turn on PV_HAS_PORT_RIGHT when proc is created. Added VPROC_HOLD
 * for session leader when new process is created.
 * 
 * Revision 3.1  91/10/31  09:41:31  roman
 * Change order of operations during exit so that parent process is
 * signalled after the process is in zombie state rather than before.
 * 
 * Revision 3.0  91/10/25  10:28:56  roman
 * First appearance of this file, which extracts the vproc ops from the
 * files in the pvproc directory, which is being deleted. Also numerous
 * minor bug fixes.
 * 
 */
#include <machine/reg.h>
#include <sys/unix_defs.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/tty.h>
#include <sys/vnode.h>
#include <uxkern/import_mach.h>
#include <uxkern/proc_to_task.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <kern/parallel.h>

/* External declarations */
extern struct vproc *vproc_alloc();

#ifdef NX
#include <i860paragon/mcmsg/mcmsg_info.h>

#if __STDC__ == 1
extern int nx_get_info(struct pvproc *pvp,
                       APPLINFO_T *applinfo,
                       LP_MAP_T *nodelist,
                       int      *nodelistcnt,
                       int       flag,
                       uid_t     euid,
                       int      *err);

extern int nx_put_info(struct pvproc  *pvp,
                       LP_MAP_T       nodelist,
                       int            nodelistcnt,
                       APPLINFO_T    *applinfo_p,
                       pid_t    pid);
#else
extern int nx_get_info();
extern int nx_put_info();
#endif /* __STDC__ */

#endif /* NX */

/* Forward references */
void update_session_cache(struct vproc *, struct vproc	*, boolean_t);
void update_cache(struct pidsctty *, pid_t, boolean_t);
int has_sctty_using_cache(struct vproc *, struct vproc *, boolean_t *);
boolean_t read_cache(struct pidsctty *, pid_t, boolean_t *);
pid_t dvp_pidgen();

/* Prototypes generated from vproc.h */
#include <tnc/dvproc_protos_gen.h>


/*
 * Capture the current value of the "pgrp sigs received" sequence
 * number and zero the "pgrp signals accumulated during this fork()
 * operation" signal mask.  We need to be able to tell later on
 * (when the new child is joining the parent's pgrp) whether any
 * pgrp signals were delivered to the parent between now and then.
 * If any more pgrp signals are delivered during this fork, we need
 * to know so we can make sure the new child gets them too.
 *
 * (If any signals have been delivered between the time the user
 * called fork() and right now, we simply return ERESTART to the
 * emulator so the signal can be delivered and then the fork() can
 * be restarted.)
 */

int
dvp_child_join_pgrp_setup(
	struct vproc *vp, 	/* vproc of parent */
	int *seqnop)		/* OUT parent's pgrp_mem_seqno value */
{
	struct pvproc *pvp = PVP(vp);

	VPROC_LOCK_PGRP_MEM_SEQNO(vp, "dvpop_fork startup");

	SIGINITSET(pvp->pvp_fork_sigset);	/* zero out bitmask of sigs */
	*seqnop = pvp->pvp_pgrp_mem_seqno;	/* return seqno to caller */

	VPROC_UNLOCK_PGRP_MEM_SEQNO(vp, "dvpop_fork startup");

	if (pproc_signal_received(pvp->pvp_pproc)) {
		return(ERESTART);
	}

	return(ESUCCESS);
}


/*
 * Have the child join the parent's process group.  We may not
 * join a process group until we are fully up to date with pgrp
 * signals -- some may have been delivered while we were forking.
 * We detect this using the pgrp sequence numbers in the pvproc.
 * (A new member's seqno must match the pgrp leader's seqno.)  If
 * the new child is not up to date with the pgrp leader then we
 * get the most recently sent signals (ones sent since the fork()
 * started) from our parent, update our own seqno, and try to join
 * the process group again.  In the pathological cases where we
 * have to perform this step more than once, we pause for an
 * increasing number of seconds in each iteration to prevent
 * undesired hysteresis.
 */
void
dvp_child_join_pgrp_end(
	struct vproc *vp, 	/* vproc of parent */
	struct vproc *vc, 	/* vproc of child */
	struct vproc *vg)	/* vproc of group leader */
{
	struct pvproc *pvp = PVP(vp);
	struct pvproc *pvc = PVP(vc);
	sigset_t tmpsigs;	/* one iteration's sigs only */
	sigset_t extrasigs;	/* accumulated sigs from multiple iterations */
	long extrasigmigarg;
	int seqno;
	int nsecs;		/* number of seconds of real-time delay */
	int error, err2, i;

	SIGINITSET(extrasigs);	/* zero out bitmask of signals */

	/*
	 * Seqno will not be changed by this particular call to the
	 * add_pgrp_list routine.  (Seqno has to be -1 on input in
	 * order for it to ever be modified on output.)
	 */
	error = PVPOP_ADD_PGRP_LIST(vg, vc, &pvc->pvp_pgrp_mem_seqno);

	for (nsecs=0; error == EAGAIN; ++nsecs) {

		/*
		 * The attempt to join the pgrp list failed (with EAGAIN)
		 * which means that at least one pgrp signal has been sent
		 * out by the pgrp leader that was NOT YET delivered to the
		 * forking parent as of the start of the fork operation.
		 * This is detected be comparing the pgrp leader's LEADER
		 * seqno value (which is incremented for every pgrp signal
		 * SENT) against the forking parent's MEMBER seqno value
		 * (which is incremented for every pgrp signal RECEIVED).
		 * When they don't match, the ADD_PGRP_LIST operation fails
		 * with EAGAIN.  So we must obtain those signals, along with
		 * the newly-updated seqno before we can join the process group.
		 *
		 * If there is an unusually large process group, it could take
		 * quite a while before our parent has a pgrp signal delivered
		 * to it, making it quite a while before we can successfully
		 * obtain that signal from our parent.  To handle this case,
		 * we loop pausing a few seconds each time, until we find we
		 * can successfully join the process group.
		 *
		 * Once we've successfully joined the process group we have
		 * to deliver to the new child process each of the signals we
		 * obtained.  All this to ensure that the child doesn't miss
		 * any of the pgrp signals (such ask SIGINT from CTRL/C).
		 */
		if (nsecs) {			/* skip delay 1st time thru */
			if (nsecs > 15) {
				nsecs = 15;	/* sanity limit */
			}
			pproc_timed_delay(nsecs);
		}

		SIGINITSET(tmpsigs);
		/*
		 * Seqno is always set by GET_FORK_SIGNALS.
		 */
		err2 = PVPOP_GET_FORK_SIGNALS(vp, &tmpsigs, &extrasigmigarg,
				&pvc->pvp_pgrp_mem_seqno);
		if (err2 != ESUCCESS ) {
			panic("dvpop_fork: GET_FORK_SIGNALS err=0x%x", err2);
		}
		SIGORSET(extrasigs, tmpsigs);	/* accum the new sigs */

		/*
		 * Seqno will not be changed by this particular call to the
		 * add_pgrp_list routine.  (Seqno has to be -1 on input in
		 * order for ADD_PGRP_LIST to ever modify it on output.)
		 */
		error = PVPOP_ADD_PGRP_LIST(vg, vc, &pvc->pvp_pgrp_mem_seqno);
	}

	/*
	 * We now have a mask containing the extra signals we need to send
	 * to the newly-joining child process (which is on the current node).
	 * Note that we have already passed the privilege checks so we bypass
	 * additional checking below, by telling the physical layer that we
	 * are privileged.
	 */
	for (i=1; i<=NSIG; ++i) {	/* check each possible signal */
		if (SIGISMEMBER(extrasigs, i)) {
			if (pproc_hold(pvc->pvp_pproc,vc->vp_pid) == ESUCCESS) {
				pproc_psignal(pvc->pvp_pproc, i, extrasigmigarg,
						0,0,0,VPROC_HAS_PRIV,0,0,0);
				pproc_release(pvc->pvp_pproc,vc->vp_pid);
			}
		}
	}
}


/*
 * NAME:	dvpop_fork
 *
 * FUNCTION:	Fork process, creating a new child.
 *
 * RETURNS:	ENOMEM, if unable to allocate a new vproc.
 *		EFAULT/EAGAIN, for errors performing the physical fork.
 *		ESUCCESS on successful completion.
 */
int
dvpop_fork(
	struct vproc *pp, 		/* vproc of caller (parent) */
	pid_t *pid,			/* process ID generated */
	node_t node,			/* node number for task */
	caddr_t new_state,		/* thread info */
	unsigned int new_state_count)	/* server thread info */
{
	register struct vproc *vc;
	register struct pvproc *pvc;
	register struct pvproc *pvp;
	struct vproc *w;
	register error;
	struct proc *procp;
	int hashidx;
	sigset_t extrasigs;
	long extrasigmigarg;
	int parent_seqno;
	extern struct vproc_ops		dvproc_ops_table;
	extern struct pvproc_ops_vector	dpvproc_ops_table;
#ifdef NX
        int                     nodelistcnt = 0;
        LP_MAP_T                nodelist = (LP_MAP_T) NULL;
        APPLINFO_T              applinfo;
        int                     ii, err;
        boolean_t               in_nodelist = FALSE;
	boolean_t		send_sigtrap;
#endif /* NX */

	pvp = PVP(pp);
#ifdef NX
        /*
         * Check if this is a NX application and a parition has
         * been allocated to it. If so cause the nodelist and applinfo
         * to be inheirited.
         */
        if (nx_in_partition(pp)) {
            if (nx_get_info(pvp,
                            &applinfo,
                            &nodelist,
                            &nodelistcnt,
                             TRUE,
                             0,
                            &err)
                != KERN_SUCCESS) {
					/* allocates new pages of OOL VM */
                nodelist = (LP_MAP_T)NULL;
            } else {
                for(ii=0; ii<nodelistcnt; ii++) {
                    if (this_node == nodelist[ii])
                        in_nodelist = TRUE;
                }
                if (!in_nodelist) {
                    error = ERFORK;
		    goto out2;
                }
            }
        }
#endif /* NX */

	/* allocate a vproc for the child and zeroize its pvproc */
	if ((vc = vproc_alloc()) == NULL) {
		error = ENOMEM;
		goto out2;
	}
	*pid = dvp_pidgen();
	pvc = PVP(vc);

	/*
	 * Seize the vproc during the fork operation.  We must prevent
	 * anyone from changing the parent's process group id during
	 * the fork; otherwise the new child may not wind up in the same
	 * process group as its parent.  We don't usually like to hold
	 * this lock while an RPC is occurring, but signal delivery is
	 * not blocked by this lock; only exit's, setpgid's and setsid's
	 * are prevented.
	 */
	VPROC_LOCK_EXCL(pp,"dvpop_fork(parent)");

	/*
	 * When creating new child processes, the management of that new
	 * child's joining its parent's process group requires special
	 * handling to prevent the child from missing signals directed
	 * at the entire process group.  Routines dvp_child_join_pgrp_setup()
	 * and dvp_child_join_pgrp_end() perform this special handling.
	 */
	error = dvp_child_join_pgrp_setup(pp, &parent_seqno);
	if (error) {
		(void) vproc_dealloc(vc);
		goto out;		/* ERESTART a distinct possiblilty */
	}

	/* do the physical fork */
	if ((error = pproc_fork(pvp->pvp_pproc,
				*pid, &procp,
				node,
				(thread_state_t) new_state,
				new_state_count)) != 0 ) {
		(void) vproc_dealloc(vc);
		goto out;
	}

	dpvproc_struct_init(vc);
	pvc->pvp_flag |= PV_IS_LOCAL | PV_IS_ORIGIN;


	vc->vp_ops = &dvproc_ops_table;
	pvc->pvp_ops = &dpvproc_ops_table;

	VPROC_LOCK_EXCL(vc,"dvpop_fork(child)");

	/* put the vproc on the vproc hash chain */
	VPROC_LIST_LOCK();
	vc->vp_pid = *pid;
	w = vproc_hash[hashidx = VPROCPIDHASH(*pid)];
	vc->vp_hashbwd = NULL;
	vc->vp_hashfwd = w;
	vproc_hash[hashidx] = vc;
	if (w)
		w->vp_hashbwd = vc;
	VPROC_HOLD(vc, "dvpop_fork(origin)");
	VPROC_HOLD(vc, "dvpop_fork(active)");
	VPROC_LIST_UNLOCK();

	/*
	 * Allocate a Mach port for the vproc.
	 */
	(void) vproc_port_allocate(vc);
	VPROC_LOCK_FLAG(vc, "dvpop_fork");
	pvc->pvp_flag |= PV_HAS_PORT_RIGHT;
#ifdef NX
        if (pvp->pvp_flag & PV_NX_PARTITIONED) {
            pvc->pvp_flag |= PV_NX_PARTITIONED;
        }
        if (pvp->pvp_flag & PV_NX_APPLICATION) {
            pvc->pvp_flag |= PV_NX_APPLICATION;
        }
        if (pvp->pvp_flag & PV_NX_ATH_PGRP) {
            pvc->pvp_flag |= PV_NX_ATH_PGRP;
        }
#endif /* NX */

	/*
	 * point the vproc at the physical process
	 */
	pvc->pvp_pproc = procp;

	/* sort out the immediate family relationships */
	pvc->pvp_childl = pvp->pvp_head_childl; /* put child on its parent's */
	pvp->pvp_head_childl = vc;	/* child list */
	pvc->pvp_head_childl = NULL;	/* child has no children */
	pvc->pvp_ppid = pp->vp_pid;	/* set child's parent id */
	pvc->pvp_foster_ppid = pp->vp_pid; /* set child's foster parent id */
	pvc->pvp_pgid = pvp->pvp_pgid;	/* child's pgrp is parent's */
	pvc->pvp_sid = pvp->pvp_sid;	/* child's session is parent's */
	pvc->pvp_pp_pgid = pvp->pvp_pgid; /* parent's pgrp is parent's */
	pvc->pvp_pp_sid = pvp->pvp_sid; /* parent's session is parent's */
	pvc->pvp_pgrp_ldr_seqno = 0;	/* new child is never a pgrp leader */
	pvc->pvp_pgrp_mem_seqno = parent_seqno;	/* orig value from parent */

	/* propagate the parent's susceptibility to controlling terminal */
	pvc->pvp_flag |= pvp->pvp_flag&PV_SCTTY;
	VPROC_UNLOCK_FLAG(vc, "dvpop_fork");

	VPROC_HOLD(vc, "dvpop_fork(child)");
	VPROC_HOLD(pp, "dvpop_fork(parent)");

	/* initialize the process structure with values from the pvproc */
	pproc_set_attr(pvc->pvp_pproc,
		       vc,
		       &vc->vp_pid,
		       &pvc->pvp_ppid,
		       &pvc->pvp_pgid,
		       &pvc->pvp_sid,
		       0);

	if (pvp->pvp_pgid != 0) {
		/* join parent's process group */
		struct vproc *vg = VPROCPTR(pvp->pvp_pgid);
		VPROC_HOLD(vg, "dvpop_fork(pgrp leader on member node)");
		dvp_child_join_pgrp_end(pp, vc, vg);
	}
#ifdef NX
        send_sigtrap = nx_trace_inherit(vc, nx_get_trace_pflags(pvp->pvp_pproc));
#endif /* NX */

	VPROC_UNLOCK_EXCL(vc,"dvpop_fork(child)");
#ifdef NX
        /*
         *  If we are an NX application then put the appl_info and
         *  the node list into the task of the child.
         */

        if (pvp->pvp_flag & PV_NX_PARTITIONED && nodelist != (LP_MAP_T)NULL) {
            error = nx_put_info(pvc,
                                nodelist,
                                nodelistcnt,
                               &applinfo,
                                vc->vp_pid);
            if (error != KERN_SUCCESS) {
                panic("dvpop_fork: failure nx_put_info ret=0x%x",
                      error);
            }
        }

	if(send_sigtrap) {
		(void)VPOP_REPORT_STATE(vc, VPROC_NO_SIGCHLD);
		error = VPOP_SIGPROC(vc, SIGTRAP, 0,
				VPROC_HAS_PRIV | VPROC_CHILD_STOP);
	}
#endif /* NX */

	error = ESUCCESS;

out:
	VPROC_UNLOCK_EXCL(pp,"dvpop_fork(parent)");

out2:
#ifdef NX
	if (nodelist != (LP_MAP_T)NULL) {
		vm_deallocate_strict(mach_task_self(), (vm_address_t)nodelist,
                              (nodelistcnt * sizeof(LP_MAP_ENTRY_T)));
	}
#endif /* NX */

	return(error);
}


/*
 * NAME:	dvp_pidgen
 *
 * FUNCTION:	Generate a process ID
 */
pid_t
dvp_pidgen()
{
	static pid_t		mpid = 1;
	static pvp_lock_t	pid_lock = PVP_LOCK_INITIALIZER;
	register pid_t		start_pid;
	register pid_t		gen_pid;
	extern pid_t		tnc_pid_node;

	pvp_lock_lock(&pid_lock);

	/* start/restart pid at 2 since 0 and 1 are used by the system */
	gen_pid = start_pid = (mpid < LOCPIDMAX) ? mpid+1 : 2;

	/* verify the pid is not being used */
	while ( VPROCPTR(gen_pid | tnc_pid_node) != NULL ) {
		gen_pid = (gen_pid < LOCPIDMAX) ? ++gen_pid : 2;
		if (gen_pid == start_pid)
			panic("dvp_pidgen(): no more usable pids\n");
	}
	mpid = gen_pid;

	pvp_lock_unlock(&pid_lock);

	return(gen_pid | tnc_pid_node);
}


/*
 * NAME:	dvpop_exit
 *
 * FUNCTION:	Terminate the calling process.
 *
 * RETURNS:	ESUCCESS
 */
int
dvpop_exit(
	struct vproc *v,	/* vproc of process exiting system */
	int rv)			/* exit code */
{
	register struct pvproc *pv = PVP(v);
	register struct vproc *vc;
	register struct vproc *fvp;
#ifdef NX
	volatile int    i,j;
#endif /* NX */


	/*
	 * Prevent exit code from ever executing twice, no matter how
	 * many triggers for it occur simultaneously.
	 */
	VPROC_LOCK_FLAG(v, "dvpop_exit");
	if (pv->pvp_flag & PV_EXITING) {
		VPROC_UNLOCK_FLAG(v, "dvpop_exit");
		return(ESUCCESS); 	/* been here already */
	}
	pv->pvp_flag |= PV_EXITING;	/* this flag bit not turned off */
	VPROC_UNLOCK_FLAG(v, "dvpop_exit");

#ifdef NX
	/* Variable delay loop added as a workaround for Mach kernel problem.
         * If many  nodes exit at once, the all-to-one message traffic can
         * clobber the boot node kernel.  The variable delay seems to avoid
         * that problem.  When NORMA/IPC is more robust, we should not
         * need this.
	 *
	 * This delay is not necessary if elder reporting is enabled
	 * - that is, when the parent has a waitmulti() outstanding.
         */
        if ((pv->pvp_flag & PV_NX_APPLICATION) && (pv->pvp_epid == 0)){
                for (i=0; i < this_node*30000; i++)
                        j = i;
        }
#endif /* NX */

	/*
	 * Session leader exit involves signaling foreground pgrp
	 * and releasing the controlling terminal device.
	 */
	if (pv->pvp_flag&PV_SESSIONLEADER && pv->pvp_flag&PV_SCTTY) {
		pid_t t_pgid;
		(void) PVPSOP_RELEASE_CTTY(pv->pvp_cttynode, v, &t_pgid);
		pv->pvp_cttynode = -1;
		if (t_pgid != 0) {
			struct vproc *g = LOCATE_VPROC_PID(t_pgid);
			if (g != 0) {
				(void) PVPOP_SIGPGRP(g, SIGHUP,
					     0, 0, 0, v->vp_pid, 0, NULL,
					     VPROC_HAS_PRIV | VPROC_NOSIGSELF);
				VPROC_RELEASE(g, "dvpop_exit: foreground pgrp");
			}
		}
	}

	/* Now let's get physical */
	pproc_exit(pv->pvp_pproc, rv);
	
	/*
	 * pproc_exit() might go to slep waiting for other server threads
	 * to terminate. Therefore, don't take the exclusive lock on the
	 * vproc till we are back from pproc_exit()
	 */

	VPROC_LOCK_EXCL(v, "dvpop_exit");
#ifdef NX
        nx_cleanup_tams(v);
#endif /* NX */
	fvp = VPROCPTR(pv->pvp_foster_ppid); 
	
	/*
	 * The process exiting may orphan its own pgrp. While it is also
	 * possible that children of this process may have their pgrps
	 * orphaned, this is taken care of in the PVPOP_REASSIGN_CHILD()
	 * pvproc op.
	 */
	if (pv->pvp_pgid != 0 &&
			pv->pvp_sid == pv->pvp_pp_sid &&
			pv->pvp_pgid != pv->pvp_pp_pgid)
		(void) PVPOP_ADJUST_JOB_CONTROL_COUNT(VPROCPTR(pv->pvp_pgid),
						      PV_SUBTRACT | PV_EXIT);
	
	/*
	 * reassign children
	 * N.B. pvp_head_childl becomes next sibling
	 */
	while ((vc = pv->pvp_head_childl) != NULL) {
	
		/*
		 * Remove child from parent's list (NOTE: no need for
		 * PVPOP_RMV_CHILD_FROM_PARENT() because we are on the
		 * parent node).
		 */
		pv->pvp_head_childl = PVP(vc)->pvp_childl;

		/* send child to the init institution */
		(void) PVPOP_REASSIGN_CHILD(vc, fvp, PV_EXIT);

		/*
		 * Now release the child vproc on the node of the parent.
		 * Wait till after all references to "vc" are done.
		 */
		VPROC_RELEASE(vc, "dvpop_exit(child)");

		/*
		 * In TNC, clearing pending terminal access signals for
		 * potentially remote descendants isn't so easy and it
		 * certainly isn't atomic. Hence, the call to spgrp() and
		 * described as:
		 * 	Protect this process from future
		 * 	tty signals, clear TSTP/TTIN/TTOU if pending
		 * isn't attempted.
		 */
	}

	/*
	 * reassign foster children
	 * N.B. pvp_head_foster_childl becomes next sibling
	 */
	VPROC_LOCK_FOSTER_LIST_EXCL(v, "dvpop_exit");
	while ((vc = pv->pvp_head_foster_childl) != NULL) {

		/*
		 * In order to not violate the locking hierarchy, we give
		 * up the foster list lock now (PVPOP_REASSIGN_CHILD()
		 * acquires foster list locks, and only one can be held
		 * at a time). Giving up the lock means that vc can
		 * disappear, so we must give its reference count a
		 * temporary boost.
		 */
		VPROC_HOLD(vc, "dvpop_exit(foster child temporary)");
		VPROC_UNLOCK_FOSTER_LIST_EXCL(v, "dvpop_exit");

		/*
		 * Reassign the foster children to the exiting process's
		 * foster parent. Note that this may fail because the
		 * foster child may get reaped and remove itself from
		 * this list. But that's OK.
		 */
		(void) PVPOP_REASSIGN_CHILD(vc, fvp, PV_FOSTER_ONLY);

		/*
		 * We're done with vc, so we can decrement the reference
		 * count.
		 */
		VPROC_RELEASE(vc, "dvpop_exit(foster child temporary)");
		VPROC_LOCK_FOSTER_LIST_EXCL(v, "dvpop_exit");
	}
	VPROC_UNLOCK_FOSTER_LIST_EXCL(v, "dvpop_exit");

	/*
	 * Mark the vproc as a zombie.
	 */

	(void) VPOP_REPORT_STATE(v, VPROC_ZOMBIE);

	VPROC_UNLOCK_EXCL(v, "dvpop_exit");

	return(ESUCCESS);
}


/*
 * NAME:	dvpop_free
 *
 * FUNCTION:	Perform implementation-specific actions when a vproc is freed.
 *
 * RETURNS:	ESUCCESS
 */
int
dvpop_free(
	struct vproc *v) 	/* vproc just freed */
{
	int	error = ESUCCESS;

	/*
	 * This layer only does work if the server has port rights to
	 * the vproc port (this is the typical case).
	 *
	 * NOTE: No locking is necessary when referencing these flags
	 * because the reference count for the vproc is zero and that means
	 * no vproc can be changing the flag values. The caller is responsible
	 * for locks that prevent the reference count from going higher.
	 */
	if (PVP(v)->pvp_flag & PV_HAS_PORT_RIGHT) {
		mach_port_t vproc_port = vproc_to_port_lookup(v);

		/*
		 * If the server has either received the vproc rights remotely
		 * or has given them away, there are two things that must be
		 * worried about:
		 * - if the server currently has receive rights, it may
		 *	be necessary to give an extra VPROC_RELEASE() at
		 *	the vproc origin node
		 * - there may be additional references to the vproc
		 *	enqueued on "this_node_tnc_server_port".
		 * Since we only rename receive rights, it is OK to
		 * destroy a send right associated with this vproc and allow
		 * incoming rights to be renamed to a different vproc.
		 * Rights of any kind won't be incoming if we have the
		 * receive right. 
		 */
		if (PVP(v)->pvp_flag & PV_HAS_REMOTE_RIGHT) {
			kern_return_t		ret;
			mach_port_urefs_t	rcv_refs,
						snd_refs;
			extern mach_port_t	this_node_tnc_server_port;

			/*
			 * If the server currently holds receive rights and
			 * this is not the vproc's origin node, then we must
			 * tell the original server to deallocate the origin
			 * vproc with its send right.
			 */
			if ((PVP(v)->pvp_flag & PV_IS_LOCAL) != 0
					&& !VPROC_IS_AT_ORIGIN(v)) {
#if MACH_ASSERT
bootnode_printf("dvpop_free(): calling REMOTE_VPROC_FREE pid=%d v=0x%x flags=0x%x\n",v->vp_pid,v,PVP(v)->pvp_flag);
#endif MACH_ASSERT
				if (PVPSOP_REMOTE_VPROC_FREE(
						VPROCNODE(v->vp_pid), v)
							!= ESUCCESS)
					panic("dvpop_free: PVPSOP_REMOTE_VPROC_FREE failure");
#if MACH_ASSERT
bootnode_printf("dvpop_free(): called  REMOTE_VPROC_FREE pid=%d v=0x%x flags=0x%x\n",v->vp_pid,v,PVP(v)->pvp_flag);
#endif MACH_ASSERT
			}

			/*
			 * See if we can safely destroy all port rights to the
			 * vproc - ensuring no vproc port operation is
			 * outstanding.
			 */
			if (error == ESUCCESS) {
				if (vproc_port_idle(v, "dvpop_free")) {
					(void) vproc_port_deallocate(v,
						PVP(v)->pvp_flag & PV_IS_LOCAL);
				} else {
#if MACH_ASSERT
					bootnode_printf("dvpop_free() port not "
					 "idle, returning EAGAIN. v=%x pid=%d "
					 "flags=%x\n",
					 v, v->vp_pid, PVP(v)->pvp_flag);
#endif	/* MACH_ASSERT */
					error = EAGAIN;
				}
			}
		} else {
			
			/*
			 * In the (typical) case where no remote rights
			 * for the vproc are involved, we just free
			 * up the vproc port.
			 */
			(void) vproc_port_deallocate(v, PVP(v)->pvp_flag &
								PV_IS_LOCAL);
		}
	}

	if (error == ESUCCESS) {
		/*
		 * Turn off port right flag bits
		 */
		PVP(v)->pvp_flag &= ~(PV_IS_LOCAL |
				      PV_HAS_REMOTE_RIGHT |
				      PV_HAS_PORT_RIGHT);
	}

	return(error);
}


/*
 * NAME:	dvpop_wait
 *
 * FUNCTION:	Wait for stopped or exited child process.
 *
 * RETURNS:	ECHILD, if no suitable child found.
 *		ESUCCESS on completion, with ret_val returning child pid
 *		and ru_loc containing usage stats.
 */
int
dvpop_wait(
	struct vproc *vp,		/* parent vproc */
	pid_t pid,			/* pid value, WAIT_ANY, 0, -pgrp id */
	int options,			/* options to vary system call */
	int *wstat,			/* child termination status */
	struct rusage_dev *ru_loc,	/* pointer to child resource usage */
	pid_t *ret_val)			/* awaited pid */
{
	register struct pvproc *pvp = PVP(vp);
	register struct vproc *vc;
	register struct pvproc *pvc;
	register struct vproc *vo;
	register f;
	int error = ESUCCESS;
	int ret;
	pid_t pgid;

	/*
	 * A positive pid or WAIT_ANY requests any process group,
	 * a pid of 0 requests any child in the parent's process group
	 * or a pid < 0 specifies another process group.
	 */
	if (pid == WAIT_ANY)
		pgid = WAIT_ANY;
	else if (pid > 0)
		pgid = WAIT_ANY;
	else if (pid == 0)
		pgid = pvp->pvp_pgid;
	else if (pid < 0)
		pgid = -pid;

loop:
	f = 0;
	VPROC_LOCK_EXCL(vp, "dvpop_wait");
	(void) pproc_assert_sleep_wait(pvp->pvp_pproc);
	for (vo = NULL, vc = pvp->pvp_head_childl;
				vc != NULL;
				vo = vc, vc = PVP(vc)->pvp_childl) {

		if (pid > 0) {
			/* we're waiting for a specific child */
			if (vc->vp_pid != pid )
				continue;
		}
#if SEC_BASE
		/*
		 * If we are init (pid 1) don't count unkillable processes.
		 * This is to avoid waiting forever for audit and policy
	   	 * daemons to teminate as a result of a signal.
	    	 */
		/* TNC can't easily do this */
#endif
		f++;

		pvc = PVP(vc);

		if (pvc->pvp_flag & PV_SZOMB || pvc->pvp_flag & PV_SSTOP) {
			pid_t	ch_pgid = pgid;
			int	state;

			/* (potentially) go remote to get stop/exit status */
repeat:			ret = PVPOP_REAP(vc, &ch_pgid, options,
					 &state, wstat, ru_loc);

			if (ret != ESUCCESS) {
				/* If pgid > 0 and the child's pgid did not
				 * match, we should also decrease f to
				 * indicate child not found.
				 */
				if ((ret == ECHILD) || (ret == EAGAIN))
					f--;
				if((ret == EAGAIN) && (pid > 0)) {
					goto repeat;
				}
				continue;
			}

			/*
			 * If the process is a zombie, then remove it
			 * from the parent-child-sibling list.
			 */
			if ((state & PV_SZOMB) &&
					(!(options & VPROC_WNOWAIT))) {
				if (vo == NULL)
				    pvp->pvp_head_childl = pvc->pvp_childl;
				else
				    PVP(vo)->pvp_childl = pvc->pvp_childl;

				/*
				 * If the child was in the parent's pgrp
				 * then the remote reap will have left the
				 * pgrp list removal to us as an optimization.
				 */
				if (ch_pgid == vp->vp_pid)
					(void) PVPOP_RMV_PGRP_LIST(vp, vc, 0);

				/* Accumulate child's stats in parent */
				pproc_add_rusage(PVP(vp)->pvp_pproc, ru_loc);

				/* remember child's ID */
				*ret_val = vc->vp_pid;

				/*
				 * Now release the child vproc on the node of
				 * the parent. Wait till after all references
				 * to "vc" are done.
				 */
				VPROC_RELEASE(vc, "dvpop_wait(child)");
				break;
			}
		
			/*
			 * stop & child is traced by parent or VPROC_WUNTRACED
			 * or we specified VPROC_WNOWAIT
			 */
			*ret_val = vc->vp_pid;
			break;
		} else {

			/*
			 * If the wait is on a particular pgid, then we should
			 * check the child's pgid to see if it matches. Since
			 * number of child found is irrelevent if the number
			 * is greater than zero, we only do the check if
			 * this is the first child found so far. Do the
			 * PVPOP_GET_ATTR() under lock control, in case the
			 * parent was woken up due to a VPROC_LEFT_PGRP
			 * state change and the child hasn't had a chance
			 * to adjust its pgrp membership yet.
			 */
			if (pgid > 0 && f == 1) {
				pid_t vc_pgid;
				ret = PVPOP_GET_ATTR(vc, 0, &vc_pgid, 0, 0, 0, 0, 0, TRUE);
				if (ret != ESUCCESS) 
                			panic("dvpop_wait: RVPOP_GET_ATTR ret=0x%x",
						ret);
				if (vc_pgid != pgid)
					f--;
			}
		}
	}
	VPROC_UNLOCK_EXCL(vp, "dvpop_wait");
	if (vc != NULL)
		error = ESUCCESS;
	else if (f == 0)
		error = ECHILD;
	else if (options & VPROC_WNOHANG) {
		*ret_val = 0;
		error = ESUCCESS;
	}
	else if (!(error = pproc_sleep_waiting(pvp->pvp_pproc)))
		goto loop;
	return (error);
}


/*
 * NAME:	dvpop_proc_nice
 *
 * FUNCTION:	Read/write process nice value for process.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 *		ESUCCESS, if getting value argument nice returns this.
 */
int
dvpop_proc_nice(
	struct vproc *v,	/* vproc of target process */
	int *nice,		/* nice to set or returned value */
	int flag)		/* set or get */
{
	uid_t euid = 0, ruid = 0;
	int has_priv = 0;

	/* find caller's privilege if we're setting nice */
	if (flag == VPROC_SET)
		(void) pproc_get_attr(0,0,0,0,0,&has_priv,&euid,&ruid,0,0);

	return(PVPOP_PROC_NICE(v,
			       nice,
			       euid,
			       ruid,
			       flag | (has_priv ? VPROC_HAS_PRIV : 0)));
}

/*
 * NAME:	dvpop_pgrp_nice
 *
 * FUNCTION:	Read/write process nice value for process group.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 *		ESUCCESS, if getting value argument nice returns this.
 */
int
dvpop_pgrp_nice(
	struct vproc *g,	/* vproc of target process group */
	int *nice,		/* nice to set or returned value */
	int flag)		/* set or get */
{
	uid_t euid = 0, ruid = 0;
	int has_priv = 0;

	/* find caller's privilege if we're setting nice */
	if (flag == VPROC_SET)
		(void) pproc_get_attr(0,0,0,0,0,&has_priv,&euid,&ruid,0,0);

	return(PVPOP_PGRP_NICE(g,
			       nice,
			       euid,
			       ruid,
			       flag | (has_priv ? VPROC_HAS_PRIV : 0)));
}


/*
 * NAME:	dvpop_sigproc
 *
 * FUNCTION:	Sends signal to process.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */

int
dvpop_sigproc(
	struct vproc *vto,	/* vproc to be signaled */
	int signo,		/* signal to be sent */
	int arg,		/* argument (for SIGMIGRATE) */
	int flags)		/* modifies posting behavior */
{
	uid_t uid;
	uid_t ruid;
	pid_t pid, sid;
	boolean_t has_priv = TRUE;

	if (!(flags&VPROC_HAS_PRIV))
		pproc_get_attr(0,&pid,0,0,&sid,&has_priv,&uid,&ruid,0,0);

	return(PVPOP_SIGPROC(vto,
			     signo,
			     0,
			     arg,
			     uid,
			     ruid,
			     pid,
			     sid,
			     flags | (has_priv ? VPROC_HAS_PRIV : 0)));
}


/*
 * NAME:	dvpop_sigpgrp
 *
 * FUNCTION:	Sends signal to process group.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_sigpgrp(
	struct vproc *gto,	/* pgrp leader vproc to be signaled */
	int signo,		/* signal to be sent */
	int arg,			/* argument (for SIGMIGRATE) */
	int *nproc,		/* number of processes signaled */
	int flags)		/* modifies posting behavior */
{
	uid_t uid;
	uid_t ruid;
	pid_t pid, sid;
	boolean_t has_priv = TRUE;

	if (!(flags&VPROC_HAS_PRIV))
		pproc_get_attr(0,&pid,0,0,&sid,&has_priv,&uid,&ruid,0,0);

	return(PVPOP_SIGPGRP(gto, signo, arg, uid, ruid, pid, sid, nproc,
				flags|(has_priv ? VPROC_HAS_PRIV : 0)));
}


/*
 * NAME:	dvpop_report_state
 *
 * FUNCTION:	Alter the state (stop, unstopped or zombie) of a proc.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_report_state(
	struct vproc *v,	/* vproc of process changing state */
	int state)		/* zombie or stop or unstop */
{
	register int	old_state, new_state;

	/* set the local vproc */
	VPROC_LOCK_FLAG(v, "dvpop_report_state");
	old_state = PVP(v)->pvp_flag;
	switch (state) {
	case VPROC_ZOMBIE:
		PVP(v)->pvp_flag |= PV_SZOMB;
#ifdef NX
		if ( nx_signal_tam(v) ) {
			PVP(v)->pvp_flag |= PV_NX_NO_SIGCHLD;
		}
#endif /* NX */
		break;
	case VPROC_STOP:
		PVP(v)->pvp_flag |= PV_SSTOP;
		break;
	case VPROC_UNSTOP:
		PVP(v)->pvp_flag &= ~PV_SSTOP;
		break;
#ifdef NX
	case VPROC_NO_SIGCHLD:
		PVP(v)->pvp_flag |= PV_NX_NO_SIGCHLD;
		break;
	case VPROC_AGAIN_SIGCHLD:
		PVP(v)->pvp_flag &= ~PV_NX_NO_SIGCHLD;
		break;
	case VPROC_GANG_STOP:
		PVP(v)->pvp_flag |= PV_NX_GANG_STOP;
		break;
	case VPROC_GANG_UNSTOP:
		PVP(v)->pvp_flag &= ~PV_NX_GANG_STOP;
		break;
#endif /* NX */
	case VPROC_LEFT_PGRP:
		/* no state change, but ensure that pvproc op always called */
		old_state = ~old_state;
		break;
	}
	new_state = PVP(v)->pvp_flag;
	VPROC_UNLOCK_FLAG(v, "dvpop_report_state");

	/*
	 * If there's been no change, there's nothing more to do,
	 * otherwise update our parent's node.
	 */
	if (new_state == old_state)
		return(ESUCCESS);
	else {
		if (PVP(v)->pvp_epid == 0)
			/* Elder reporting not in effect */
			return(PVPOP_REPORT_STATE(
					VPROCPTR(PVP(v)->pvp_ppid), v, state));
		else if ((PVP(v)->pvp_flag & PV_HAS_REPORTED) == 0)
			/* Elder reporting enabled and no report outstanding */
			return(PVPOP_REPORT_STATE_ELDER(
					VPROCPTR(PVP(v)->pvp_epid), v, state));
		else
			/* Elder reporting enabled but report outstanding */
			return(ESUCCESS);
	}
}


/*
 * NAME:	dvpop_setpgid
 *
 * FUNCTION:	Change the process group id of a process.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_setpgid(
	struct vproc *v,	/* vproc of target process */
	struct vproc *g,	/* vproc of process group to be assigned */
	pid_t pid,		/* pid of calling process */
	pid_t sid)		/* session id of calling process */
{
	return(PVPOP_SETPGID(v, g, pid, sid));
}


/*
 * NAME:	dvpop_setsid
 *
 * FUNCTION:	Create a new session.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_setsid(
	struct vproc *v)	/* vproc of process becoming session leader */
{
	/*
	 * When called during a tty operation, we may have to go remote.
	 */
	return(PVPOP_SETSID(v));
}


/*
 * NAME:	dvpop_get_attr
 *
 * FUNCTION:	Get relationship attributes for process.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_get_attr(
	struct vproc *v,	/* vproc of calling process */
	pid_t *ppid,		/* returned id of parent */
	pid_t *pgid,		/* returned process group id*/
	pid_t *sid,		/* returned sesion id */
	uid_t *uid,		/* returned user id */
	uid_t *ruid,		/* returned real uid */
	boolean_t *job_control,	/* returns if process subject to job control */
	boolean_t *has_sctty)	/* returns if session tty control */
{
	return(PVPOP_GET_ATTR(v, ppid, pgid, sid, uid, ruid, job_control, has_sctty, FALSE));
}


/*
 * NAME:	dvpop_setpinit
 *
 * FUNCTION:	Converts process to a child of init.
 *
 * RETURNS:	ESUCCESS
 */
int
dvpop_setpinit(
	struct vproc *v)	/* vproc in question */
{
	register struct vproc *pp;

	pp = VPROCPTR(PVP(v)->pvp_ppid);

	/* remove process from current parent */
	(void) PVPOP_RMV_CHILD_FROM_PARENT(pp, v);

	/* let init adopt it */
	(void) PVPOP_REASSIGN_CHILD(v, NULL, 0);

	/* make it its own process group */
	(void) PVPOP_SETPGID(v, v, v->vp_pid, PVP(v)->pvp_sid);

	return(ESUCCESS);
}
	

/*
 * NAME:	dvpop_ctty_getattr
 *
 * FUNCTION:	Get attributes of the controlling terminal for a process.
 *
 * RETURNS:	ENOTTY, if the process has no controlling terminal.
 *		ESRCH, if no session exists.
 *		ESUCCESS on successful completion with attributes returned
 *		for the non-NULL pointers given.
 */
int
dvpop_ctty_getattr(
	struct vproc *v,	/* vproc of process being queried */
	struct vproc *s,	/* vproc of session leader of process */
	pid_t *t_pgid,		/* returned foreground pgrp */
	dev_t *dev,		/* returned controlling device */
	node_t *node,		/* returned controlling terminal node */
	struct tty **ttyp)	/* returned ptr to controlling tty (or NULL) */
{
	register struct vproc	*vs;
	int 			error;
	pid_t			sid = 0;
	boolean_t		has_sctty;

	/*
	 * If called with a null session vproc, we need to get the sid
	 * for the process by getting attributes. However, if this reveals
	 * that the process has no controlling terminal, return ENOTTY
	 * without further ado. From sid, we can locate the session.
	 * Note: this operation should not be called with a null session vproc
	 * during terminal i/o so optimization is not negated by going remote
	 * here!
	 */
	if ((vs = s) == NULL) {
		error = PVPOP_GET_ATTR(v, 0, 0, &sid, 0, 0, 0, &has_sctty, FALSE);
		if (error != ESUCCESS)
			return (error);
		if (!has_sctty)
			return(ENOTTY);
		if ((sid == 0) || (vs=LOCATE_VPROC_PID(sid)) == NULL)
			return(ESRCH);
	}

	/*
	 * Since this operation can be invoked for each terminal i/o
	 * call on the node of a controlling terminal, we must avoid
	 * going remote back to the execution node of the calling process.
	 */
	if (PVP(vs)->pvp_flag & PV_CTTY_NODE) {
		/*
		 * This is the case where we are on the controlling terminal
		 * node. We must check whether the caller has a controlling
		 * terminal using the cache in the session vproc to avoid
		 * going remote unless our pid is not in the cache.
		 */
		struct tty	*tp;
		tp = PVP(vs)->pvp_cttyp;
		error = has_sctty_using_cache(v, vs,  &has_sctty);
		if (error == ESUCCESS) {
			if (has_sctty) {
				if (node)
					*node = this_node;
				if (ttyp)
					*ttyp = tp;
				if (dev)
					*dev = tp? tp->t_dev : 0;
				if (t_pgid)
					*t_pgid = tp? tp->t_pgid : 0;
			} else
				error = ENOTTY;
		}
	} else {
		/*
		 * If we haven't already determined whether this process
		 * has a controlling terminal (calling PVPOP_GET_ATTR), do
		 * so now.
		 */
		if (sid == 0) {
			error = PVPOP_GET_ATTR(v, 0, 0, 0, 0, 0, 0, &has_sctty, FALSE);
			if (error) 
				goto end;
		}

		/*
		 * There's nothing to do if there's no controlling terminal.
		 */
		if (!has_sctty) {
			error = ENOTTY;
			goto end;
		}

		/*
		 * Try for the attributes on the node of the session leader;
		 * it knows where the controlling tty is located and will
		 * do the appropriate forwarding.
		 */
		error = PVPOP_CTTY_GETATTR(vs, t_pgid, dev, node);
		if (error == ESUCCESS && ttyp != NULL)
			*ttyp = NULL;
	}

end:
	if (s == NULL)
		VPROC_RELEASE(vs, "dvpop_ctty_getattr");
	return(error);
}

/*
 * NAME:	dvpop_set_ctty
 *
 * FUNCTION:	Set (a pointer to) the tty struct of the controlling terminal
 *		for the caller's vproc.
 *
 * RETURNS:	EPERM, if setting ctty for process other than session leader.
 *		EPERM, if setting no ctty for a session leader.
 *		ESUCCESS on successful completion.
 */
int
dvpop_set_ctty(
	struct vproc *v,	/* vproc of calling process */
	struct vproc *s,	/* vproc of session leader */
	struct tty *ttyp,	/* pointer to controlling tty (or NULL) */
	int flags)		/* set or clear */
{
	register int 		error;
	register struct vproc	*vs;
	pid_t			sid;

	/*
	 * If called with a null session vproc, we need to get the sid
	 * for the process by getting attributes.
	 * From sid, we can locate the session.
	 */
	if ((vs = s) == NULL) {
		error = PVPOP_GET_ATTR(v, 0, 0, &sid, 0, 0, 0, 0, FALSE);
		if (error) 
			return (error);
		if ((sid == 0) || (vs=LOCATE_VPROC_PID(sid)) == NULL)
			return(ESRCH);
	}

	/*
	 * Now we get into some devious optimizations to avoid going
	 * remote for subsequent ctty_getattr operations, although we
	 * first have to visit the execution node of the caller.
	 */
	error = PVPOP_SET_CTTY(v, this_node, flags);

	/*
	 * If the operation was permitted, we can now perform the
	 * associated local operations. This includes updating the
	 * cache of pid/sctty values in the controlling node's session vproc.
	 */
	if (error == ESUCCESS && flags&VPROC_SET) {
		if (ttyp != NULL) {
			/*
			 * Here if we're a session leader in tty code
			 * on the node of the controlling terminal.
			 * Retain this vproc to reference the tty
			 * structure and cache pid/sctty pairs for
			 * session members accessing their controlling
			 * terminal.
			 */
			VPROC_LOCK_EXCL(vs, "dvpop_set_ctty(controlling session)");
			VPROC_HOLD(vs, "dvpop_set_ctty(ctty node)");
			VPROC_LOCK_FLAG(vs, "dvpop_set_ctty(ctty node)");
			PVP(vs)->pvp_flag |= PV_CTTY_NODE;
			VPROC_UNLOCK_FLAG(vs, "dvpop_set_ctty(ctty node)");
			PVP(vs)->pvp_cttynode = this_node;
			PVP(vs)->pvp_cttyp = ttyp;
			update_session_cache(vs, v, TRUE);
			VPROC_LOCK_FLAG(v, "dvpop_set_ctty");
			PVP(v)->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(v, "dvpop_set_ctty");
			VPROC_UNLOCK_EXCL(vs, "dvpop_set_ctty(controlling session)");
		}
	} else if (error == ESUCCESS && flags&VPROC_CLEAR) {
		/*
		 * Here if we're a session member in tty code
		 * on the node of the (previously) controlling terminal.
		 * Update the session leader cache to reflect the non
		 * controlling state.
		 */
		VPROC_LOCK_FLAG(v, "dvpop_set_ctty(controlling session)");
		PVP(v)->pvp_flag &= ~PV_SCTTY;
		VPROC_UNLOCK_FLAG(v, "dvpop_set_ctty(controlling session)");
		update_session_cache(vs, v, FALSE);
	}

	if (s == NULL)
		VPROC_RELEASE(vs, "dvpop_set_ctty");
	return(error);
}

/*
 * Function:	has_sctty_using_cache
 *
 * Description:	Determines whether vproc v has a controlling terminal
 *		by searching in the (pid,has_sctty) cache in the session
 *		vproc. If this is absent from the cache it is fetched and
 *		the cache is updated.
 *
 * Returns:	ESUCCESS on successful completion; otherwise error from
 *		remote operation.
 */
int
has_sctty_using_cache(
	struct vproc	*v,
	struct vproc	*s,
	boolean_t	*has_sctty)
{
	register int error;

	if (!read_cache(&PVP(s)->pvp_sctty_cache, v->vp_pid, has_sctty)) {
		/*
		 * This pid isn't in the cache. So, do a remote op
		 * to fetch it and place it into the cache.
		 */
		error = PVPOP_GET_ATTR(v, 0, 0, 0, 0, 0, 0, has_sctty, FALSE);
		if (error)
			return(error);
		update_cache(&PVP(s)->pvp_sctty_cache, v->vp_pid, *has_sctty);
	}

	return(ESUCCESS);
}

boolean_t
read_cache(
	struct pidsctty *ps,
	pid_t pid,
	boolean_t *has_sctty)
{
	register int	i;
	/*
	 * Scan through cache from top to bottom looking for given pid,
	 * update and return if match found.
	 */
	VPROC_LOCK_SCTTY_CACHE(ps);
	for (i = 0; i <= N_PIDSCTTY_CACHE; i++)
		if (ps->cache_array[i].pid == pid) {
			*has_sctty = ps->cache_array[i].has_sctty;
			VPROC_UNLOCK_SCTTY_CACHE(ps);
			return(TRUE);
		}
	VPROC_UNLOCK_SCTTY_CACHE(ps);
	return(FALSE);
}

void
update_session_cache(
	struct vproc	*s,
	struct vproc	*v,
	boolean_t	has_sctty)
{
	update_cache(&PVP(s)->pvp_sctty_cache, v->vp_pid, has_sctty);
}

void
update_cache(struct pidsctty *ps, pid_t pid, boolean_t has_sctty)
{
	register int	i;

	/*
	 * Scan through cache from top to bottom looking for given pid,
	 * update and return if match found.
	 */
	VPROC_LOCK_SCTTY_CACHE(ps);
	for (i = 0; i <= N_PIDSCTTY_CACHE; i++)
		if (ps->cache_array[i].pid == pid) {
			ps->cache_array[i].has_sctty = has_sctty;
			VPROC_UNLOCK_SCTTY_CACHE(ps);
			return;
		}

	/*
	 * Here if this pid isn't already in the cache -- so add it.
	 * to fetch it and place it into the cache.
	 */
	ps->cache_array[ps->next_in].pid	= pid;
	ps->cache_array[ps->next_in].has_sctty	= has_sctty;
	if (++ps->next_in == N_PIDSCTTY_CACHE)
		ps->next_in = 0;
	VPROC_UNLOCK_SCTTY_CACHE(ps);
}

/*
 * NAME: dvpop_ptrace
 *
 * FUNCTION: Process tracing functions for a child process.
 *
 * RETURNS:	ESRCH, if no child exists.
 *		ESUCCESS on successful completion.
 */
dvpop_ptrace(struct vproc *vd,	/* vproc of calling (debugger) process */
	     struct vproc *ve,	/* vproc of traced process */
	     int req,		/* ptrace request */
	     int *addr,		/* address within process */
	     int data,		/* data to be written */
	     int *retval)	/* returned data if read from address */
{
	register struct vproc *vp;

	/*
	 * Check that the traced process is indeed a child of the debugger.
	 * We don't do this check if the caller is requesting to be traced,
	 * instead, we call the physical level directly.
	 */
	if (req == VPROC_PT_TRACE_ME)
		return(pproc_ptrace(PVP(vd)->pvp_pproc,
				    PT_TRACE_ME, addr, data, retval));

	VPROC_LOCK_EXCL(vd, "dvpop_ptrace(debugger)");
	vp = PVP(vd)->pvp_head_childl;
	for (; vp != ve; vp = PVP(vp)->pvp_childl)
		if (vp == NULL) {
			VPROC_UNLOCK_EXCL(vd, "dvpop_ptrace(debugger)");
			return(ESRCH);
		}
	VPROC_UNLOCK_EXCL(vd, "dvpop_ptrace(debugger)");

	/*
	 * Perform the ptrace request on the potentially remote child.
	 */
	return(PVPOP_PTRACE(ve, req, addr, data, retval));
}

/*
 * NAME:	dvpop_get_task_port
 *
 * FUNCTION:	Get Mach task port corresponding to physical process
 *
 * RETURNS:	ESRCH, if vproc is not active
 *		EACCES, if calling process does not have permission
 *		ESUCCESS, if successful
 */
int
dvpop_get_task_port(
	struct vproc *v,	/* vproc of target process */
	mach_port_t *task_port)	/* task port for physical process */
{
	uid_t euid;
	int has_priv;

	(void) pproc_get_attr(0,0,0,0,0,&has_priv,&euid,0,0,0);

	return(PVPOP_GET_TASK_PORT(v,
				   euid,
			       	   has_priv ? VPROC_HAS_PRIV : 0,
				   task_port));
}


/*
 * NAME:	dvpop_terminal_sigpgrp
 *
 * FUNCTION:	Sends signal to process group indicated by process.
 *
 * RETURNS:	(Return from corresponding PVPOP.)
 */
int
dvpop_terminal_sigpgrp(
	struct vproc *v,	/* process whose pgrp is to be signaled */
	int signo)		/* signal to be sent */
{
	return(PVPOP_TERMINAL_SIGPGRP(v, signo));
}


/*
 * Debugging routine.  Put a null-terminated msg into buf about the
 * passed vproc port.
 */
void
vproc_debug_msg1(v, buf)
	struct vproc *v;
	char *buf;
{
	mach_port_t	porttype, structtype;
	int		portret, structret;

	porttype = 0;
	portret = mach_port_type(mach_task_self(), vproc_to_port_lookup(v),
				(mach_port_type_t *)&porttype);
	structtype = 0;
	structret = mach_port_type(mach_task_self(), (mach_port_t)v,
				(mach_port_type_t *)&structtype);

	sprintf(buf, "vproc info: pid=%d(0x%x)  pvpflags=0x%x\n"
			"vproc   PORT=0x%x  type=0x%x (type req ret=0x%x)\n"
			"vproc STRUCT=0x%x  type=0x%x (type req ret=0x%x)\n",
			v->vp_pid, v->vp_pid, PVP(v)->pvp_flag,
			vproc_to_port_lookup(v), porttype, portret,
			v, structtype, structret);
}
