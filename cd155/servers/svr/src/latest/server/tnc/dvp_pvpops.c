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
 * $Log: dvp_pvpops.c,v $
 * Revision 1.54  1995/03/14  01:54:47  raya
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
 * Revision 1.53  1995/02/17  18:47:11  toman
 *  Reviewer: Bob Yasi, John Litvin
 *  Risk: Low
 *  Benefit or PTS #: 12454
 *  Testing: VSTNC
 *  Module(s): server/tnc/dvp_pvpops.c
 * In dpvpop_rmv_child_from_parent(), dpvpop_rmv_orphan_from_foster_list(),
 * dpvpop_rmv_prgp_list(), and dpvpop_rmv_session_list(), the testcase of
 * the for loop has been changed to check for a NULL vproc before
 * dereferencing it.  This allows the proper panic message to be displayed,
 * rather than causing an exception panic.
 *
 * Revision 1.52  1995/02/17  18:04:49  raya
 * Added code to reset the PV_NX_NO_SIGCHLD flag when process calls setsid.
 *
 *  Reviewer:		Nandini
 *  Risk:			Low
 *  Benefit or PTS #:	12465
 *  Testing:		Selective developer testing.
 *  Module(s):		server/tnc/dvp_pvpops.c
 *
 * Revision 1.51  1995/02/01  21:41:37  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.50  1995/01/20  20:28:12  yazz
 *  Reviewer: Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: 11819
 *  Testing: EATs controlc, sched, os_interfaces
 *  Module(s): server/tnc/dvp_pvpops.c
 * Make the table() calls that operate on a particular process hold a pproc
 * reference during the call, so the pproc cannot disappear during the
 * operation.
 *
 * Revision 1.49  1995/01/10  17:18:46  yazz
 *  Reviewer: John Litvin, Suri Brahmaroutu
 *  Risk: Lo
 *  Benefit or PTS #: #12107
 *  Testing: by inspection, plus multiuser boot
 *  Module(s): server/tnc/dvp_pvpops.c
 * In dvpop_reap(), release the exclusive vproc lock before taking an
 * ESRCH error return.
 *
 * Revision 1.48  1994/12/19  17:03:43  johannes
 * dpvpop_sigproc(): Changed typo 'struct pproc' to 'struct proc'.
 *
 *  Reviewer: Bob Yazzi
 *  Risk: High (locking issue)
 *  Benefit or PTS #: 11799
 *  Testing: test case, developer testing, corefile EAT, controlc EAT
 *  Module(s): svr/server/tnc/pvp.ops
 *             svr/server/tnc/dvp_pvpops.c
 *             svr/server/paracore/dvp_pvpcore.c
 *
 * Revision 1.47  1994/11/18  20:43:12  mtm
 * Copyright additions/changes
 *
 * Revision 1.46  1994/11/08  20:11:33  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 9853, 11537, 11538
 *  Testing: sched & concur EATs
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * Implement a pproc ref count mechanism to prevent stale data in reaped
 * pprocs from being treated as current.
 *
 * Revision 1.45  1994/11/03  16:10:30  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: med
 *  Benefit or PTS #: 11459
 *  Testing: corefile EAT
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * 	server/uxkern/syscall_subr.c
 * Handle pproc_psignal() returning an error, indicating the process being
 * signalled is no longer valid.
 *
 * Revision 1.44  1994/07/19  13:42:28  nandy
 * In dpvpop_reap and dpvpop_ptrace check to make sure the process is not
 * already reaped.
 *
 *  Reviewer: cfj
 *  Risk: L
 *  Benefit or PTS #: 10048
 *  Testing: PTS test case
 *  Module(s): tnc/dvp_pvpops.c
 *
 * Revision 1.43  1994/06/29  17:05:52  johannes
 * dpvpop_rmv_pgrp_list(): calling free_pvp_core_data()
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
 * Revision 1.42  1994/06/22  02:54:08  yazz
 *  Author of fix: Stefan Tritscher
 *  Reviewer: Chris Peak, Bob Yasi
 *  Risk: lo
 *  Benefit or PTS #: 9857
 *  Testing: no testcase; builds & multiuser boot
 *  Module(s): server/tnc/dvp_pvpops.c
 *
 * Use a vproc pointer for the correct process -- the first one in the array
 * instead of the first one in the process group leader's list -- in
 * dpvpop_sigpgrp(), macro PVPOP_SIGPROC_MULTI().
 *
 * Revision 1.41  1994/06/18  21:40:46  chrisp
 * In dpvpop_reap(), take the vproc lock before copying output parameters.
 * This is a (possibly innocuous) merge error connected with waitmulti().
 *
 *  Reviewer: yazz@locus.com
 *  Risk: L
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s): dvp_pvpops.c
 *
 * Revision 1.40  1994/06/09  20:49:10  jlitvin
 * A kill() of a zombie process should return -1 and ESRCH instead of success.
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 4738
 *  Testing: VSX EAT
 *  Module(s): server/tnc/dvp_pvpops.c
 *
 * Revision 1.39  1994/06/02  22:28:10  chrisp
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
 * Revision 1.38  1994/05/03  23:23:37  dbm
 * Mainline merge of R1.2 versoin 1.33.2.2
 *
 * Revision 1.37  1994/04/27  01:06:07  jlitvin
 * Add a verbose comment to prevent confusion on the part of developers
 * who are trying to understand the VPROC locking hierarchy.
 *
 * Revision 1.36  1994/04/22  18:23:53  dbm
 * Mainline merge of R1.2 version 1.33.2.1
 *
 * Revision 1.35  1994/03/31  01:54:59  yazz
 *  Reviewer: Chris Peak, Charlie Johnson
 *  Risk: lo
 *  Benefit or PTS #: #7978
 *  Testing: zombies no longer signalled
 *  Module(s): .../server/tnc/dvp_pvpops.c
 *
 * When trying to signal a process that is expected to be stopped, but we
 * find that it's a zombie, return ESRCH instead of ESUCCESS.
 *
 * Revision 1.34  1994/03/14  02:04:48  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.33.2.2  1994/05/03  23:16:38  dbm
 * Re-fixed bug 8729 due to first change breaking IPD.  The IPD debugger can
 * have valid zombie tasks.  The real fix is to check to make sure that the
 * real proc is non null before dereferencing it.
 *
 *  Reviewer: John Litvin
 *  Risk:Low
 *  Benefit or PTS #:8729
 *  Testing: IPD, VSX, and Fileio Eats, specific test case.
 *  Module(s):
 * 	dvp_pvpops.c
 *
 * Revision 1.33.2.1  1994/04/22  18:19:27  dbm
 * Added check to dpvpop_get_attr to return proper uid/riud for zombie processes.
 *  Reviewer: Charlie Johnson
 *  Risk:Low
 *  Benefit or PTS #:8729
 *  Testing: Specific test case, VSX Eats
 *  Module(s):
 *         bsd/tty.c
 *         tnc/dvp_pvpops.c
 *         tnc/dvp_vpops.c
 *         nx/nx.c
 *
 * Revision 1.33  1993/11/17  22:04:50  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Low
 *  Benefit or PTS #: 7156
 *  Testing: By code inspection
 *  Module(s): dvp_pvpops.c
 *
 * Remove a stray line of code that duplicated the exclusive vproc unlock at
 * the end of dpvpop_add_pgrp_list().
 *
 * Revision 1.32  1993/11/17  21:46:14  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Low
 *  Benefit or PTS #: 7186
 *  Testing: The tests for #4882 have been successfully repeated.
 *  Module(s): dvp_pvpops.c
 *
 * Set the member and leader sequence number values correctly in setpgid both
 * when the process is joining an existing pgrp as a member and also when it
 * is becoming the leader of its own new pgrp.
 *
 * Revision 1.31  1993/11/16  16:29:31  nandy
 * dpvpop_setpgid() now delays setting the pgroup till it has created a list
 * of members of the process group.
 *
 *  Reviewer: cfj
 *  Risk: Medium
 *  Benefit or PTS #: Bergen Acceptance, 6827
 *  Testing: Bergen Acceptance
 *  Module(s): server/tnc/dvp_pvpops.c
 *
 * Revision 1.30  1993/11/03  19:16:24  yazz
 * Establish a sequence number mechanism whereby process group signals, such as
 * those generated by CTRL/C, are guaranteed to be delivered to child processes
 * of a reproducing (fork(), rfork(), rforkmulti(), etc.) task.  Many unixes
 * have a timing window where new child procs can miss out on a pgrp-style signal.
 *
 * Revision 1.29  1993/11/03  16:02:07  nandy
 * PTS: 6551
 * Reviewer: cfh jlitvin
 * Risk: 	  Medium
 * benefits: At times wait() finds a process reapable becuase the
 * process is in stopped state. However, by the time actual reap
 * happens the stopped process could have started exiting. This
 * causes a deadlock between the parent and the child. dpvpop_reap()
 * now returns with a EAGAIN if it finds a process in the middle of
 * exit code.
 * Testing: DONE.
 *
 * Revision 1.28  1993/10/29  15:35:10  nandy
 * Reviewers : cfj jlitvin
 * Risk : Medium
 * Benefits : Users cannot break Gang Scheduling by sending SIGCONT.
 * PTS  :  4119
 * Testing : Done.
 *
 * Revision 1.27  1993/10/21  23:33:58  bolsen
 * 10-21-93 Locus code drop for Generic Spanning Tree.
 *
 * Revision 1.26  1993/10/11  13:11:16  nandy
 * Added ruid and uid to dpvpop_get_attr().
 *
 * Revision 1.25  1993/08/31  20:13:00  nandy
 * In dpvpop_reap(), get the lock on the vproc early on to avoid reap and exit
 * happen at the same time.
 *
 * Revision 1.24  1993/08/06  15:01:10  nandy
 * Receive all the messages before destroying the reply port in svr_sigproc_multi().
 *
 * Revision 1.23  1993/07/28  16:15:36  nandy
 * Return KERN_SUCCESS if svr_sigproc_multi succeeds.
 *
 * Revision 1.22  1993/07/23  20:50:52  nandy
 * Instead of sending the message to yourself in dvpop_sigpgrp(), send message to
 * the first element of the nodelist.
 *
 * Revision 1.21  1993/07/22  21:55:10  nandy
 * sigpgrp() now uses spanning tree to deliver signals.
 *
 * Revision 1.20  1993/07/14  18:32:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.5  1993/07/01  20:44:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.19  1993/06/09  00:09:23  cfj
 * Change occurances of #include <i860ipsc/mcmsg/*.h> to #include <i860paragon/mcmsg/*.h>
 *
 * Revision 1.18  1993/05/20  16:03:14  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.17  1993/05/10  17:10:41  cfj
 * Remove additional parameter to calls to PVPOP_ADJUST_JOB_CONTROL_COUNT.
 *
 * Revision 1.16  1993/05/06  19:21:53  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.3  1993/05/03  17:44:49  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.15  1993/04/27  18:31:54  nandy
 * Call nx_report_allocator after releasing the locks.
 *
 * Revision 1.14  1993/04/27  15:51:53  nandy
 * Added nx_get_p_flag() to get p_flag of a remote proc.
 *
 * Revision 1.13  1993/04/19  20:45:31  cfj
 * Merge with T9.5.
 *
 * Revision 1.8.4.5  1993/04/19  20:17:11  cfj
 * Added the vproc of the caller as a parameter to PVPOP_ADJUST_JOB_CONTROL_COUNT() so that we can check and not send a signal to the calling process.
 *
 * Revision 1.12  1993/04/07  22:00:17  cfj
 * Merged with T9.
 *
 * Revision 1.8.4.3  1993/04/04  20:13:41  nandy
 * Merged from the main stem.
 *
 * Revision 1.11  1993/04/03  03:08:26  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.10  1993/03/13  03:14:44  cfj
 * Merge from T9.
 *
 * Revision 1.9  1993/03/09  00:43:09  nandy
 * Added nx_get_flag() to get pvp_flag of the target process.
 *
 * Revision 1.8.4.1  1993/03/13  03:10:41  cfj
 * Fix a problem in dvpop_exit() in dvp_vpops.c so that the exclusive lock
 * on the vproc until after the process is marked a ZOMBIE.  Also fixed a
 * problem in dpvpop_rmv_pgrp_list() in dvp_pvpops.c so that the vproc is
 * not released until after all references to the vproc data are made.
 *
 * Revision 1.1.2.2.2.3  1993/02/16  20:05:38  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.8  1993/02/02  17:52:39  nandy
 * Added necessary #ifdef NX
 *
 * Revision 1.7  1993/02/01  20:00:30  nandy
 * Allocator is notified when a process group that was a part of 
 * the nx application is deleted.
 *
 * Revision 1.6  1993/01/21  19:09:12  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.1.2.2.2.2  1993/01/09  00:05:21  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.5  1992/12/18  17:32:55  nandy
 * Don't do a pproc_wakeup if NX_NO_SIGNAL is set in report_state.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:01:50  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 * 
 * Revision 1.4  1992/12/11  03:01:26  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:47:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:30:57  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.50  93/09/16  08:39:50  chrisp
 * [SPE 0030] Generic Spanning Trees:
 * 	dpvpop_sigpgrp() now uses PVPOP_SIGPROC_MULTI() if a spanning
 * 	tree signal delivery is appropriate.
 * 	dpvop_sigproc_multi() added to perform the spanning tree work...
 * 	it in turn invokes PVPOP_SIGPROC_MULTI_SEND() to propagate the
 * 	signal in (up/down?) the tree asynchronously, PVPOP_SIGPROC() to
 * 	make the local delivery and then PVPOP_SIGPROC_MULTI_RECEIVE()
 * 	to wait for remote completions.
 * 
 * Revision 3.49  93/08/23  12:39:57  slk
 * Fixed problem with processes not getting reaped Intel bug # ???
 * by taking the exclusive lock on the vproc before pproc_reap() 
 * is called in dvpop_reap().  (Checked in for Roman and Nandini).
 * 
 * Revision 3.48  93/06/24  13:52:48  mjl
 * [ Bug 0251 ] Remove obsolete call to pproc_clear_sleep_wait().  (mjl for yazz)
 * 
 * Revision 3.47  93/05/14  12:10:41  slively
 * Add dpvpop_table_set.
 * 
 * Revision 3.46  93/04/22  16:37:56  roman
 * [Bug 222] Waiting for pgid has problems. Included extra parameters for
 * 	rmv_pgrp_list and get_attr pvproc ops.
 * Fixup of minor typos in strings.
 * 
 * Revision 3.45  93/03/15  15:36:48  roman
 * [Bug 0136] Do not add orphaned child to foster parent list of init -
 * 	the child is already on the parent-child-sibling list of init.
 * 
 * Revision 3.44  93/01/05  09:36:59  chrisp
 * Unlock vproc on early return from dpvpop_reassign_child().
 * Increment reference count for foster parent vproc before adding orhan.
 * 
 * Revision 3.43  92/11/02  11:30:03  roman
 * Get rid of type buffer_t. It's really a char_array.
 * 
 * Revision 3.42  92/10/28  15:09:11  roman
 * Include prototypes of routines generated by makeTNCtables.sh (ensures
 * 	better type-checking).
 * Add new pvpop for table_get().
 * Minor formatting fixes.
 * 
 * Revision 3.41  92/10/27  12:17:29  chrisp
 * [Bug #88] setpgid() from parent to child in own pgroup hangs.
 * 	In PVPOP_SET_ATTR(), if group vproc is identical to subject vproc,
 * 	don't use PVPOP_ADJUST_JOB_CONTROL_COUNT() but make adjustment
 * 	directly. This avoids attempting to lock the pgroup lock twice.
 * 
 * Revision 3.40  92/10/16  11:03:03  chrisp
 * In dpvpop_get_task_port() retrun ESRCH and MACH_PORT_NULL if target
 * 	process is zombied.
 * 
 * Revision 3.39  92/10/14  08:22:34  chrisp
 * [Bug #67] Check for no controlling terminal node (-1) in
 * 	dpvpop_ctty_getattr() and return ENOTTY - it's possible that the
 * 	session leader has exited.
 * [Bug #68] In dpvpop_reap(), call pproc_clear_sleep_wait() before attempting
 * 	to take vproc locks to ensure that no physical lock is held.
 * 
 * Revision 3.38  92/10/09  09:54:49  roman
 * Fix RCS comments.
 * Add procedure prototypes for clean 860 compilations.
 * 
 * Revision 3.37  92/09/29  07:51:12  roman
 * Change all reference from "site" to "node".
 * Fix RCS comments.
 * Add concept of foster children. This means the addition of two new
 * 	pvproc ops for adding and removing of foster children and
 * 	changes to the reassign_child pvproc op for foster children.
 * Change pvproc field names to the new (better, more consistant) names.
 * Change pvproc code to not use any VPOPs, but to use PVPOPs explicitly.
 * Move the VPROC_TERMINAL_ACCESS checking code from the sigpgrp
 * 	pvproc op and add a new terminal_sigpgrp pvproc op to
 * 	do it.
 * Change all code to use the new lock invocation macros.
 * Get rid of setpinit() pvproc op - no longer used - logic done directly
 * 	by vproc op.
 * Change pvproc ops to use pvps ops (private virtual process system 
 *	operations).
 * 
 * Revision 3.36  92/09/22  11:01:53  chrisp
 * [Bug #59] VPOP_SIGPGRP() should return ESUCCESS if at least one process
 * 	has been signaled and should return any error received from
 * 	pproc_signal() [e.g. EPERM] if no process is signaled.
 * 
 * Revision 3.35  92/09/21  14:11:34  chrisp
 * pproc_wakeup() required only for SIGCHLD of parent - move call from
 * VPOP_SIGPROC() to VPOP_SET_STATE().
 * 
 * Revision 3.34  92/08/25  14:45:33  chrisp
 * Fix for OSF bug #386, ^Z job control defective.
 * 
 * Revision 3.33  92/07/30  16:06:40  chrisp
 * Add check for attempted signaling of a deceased process.
 * 
 * Revision 3.32  92/07/08  09:07:27  roman
 * Correct prior RCS comment.
 * 
 * Revision 3.31  92/07/06  09:04:18  chrisp
 * In dpvpop_reassign_child(), release former parent vproc AFTER reassignment
 * 	and increment init's reference BEFORE calling add_child_to_parent().
 * 
 * Revision 3.30  92/06/26  13:53:37  chrisp
 * [Bug #35] Zombie or stopped children assigned to a new parent must have
 * 	their correct state recorded in their vproc on the parent's node.
 * 
 * Revision 3.29  92/06/17  09:12:59  roman
 * [Bug 0026] New vproc op VPOP_GET_TASK_PORT() added to get task_by_pid() 
 *	system call working.
 * 
 * Revision 3.28  92/05/29  10:35:16  chrisp
 * [Bug #18] PVPOP_CTTY_GETATTR() returns ENOTTY when the cttynode is -1 -
 * which signifies that the session leader has exited.
 * 
 * Revision 3.27  92/05/22  09:55:48  chrisp
 * [Bug #17] dpvpop_pgrp_nice() always returns ESRCH - "=" -> "=="
 * 
 * Revision 3.26  92/05/07  10:44:01  chrisp
 * [Bug #10] Add optionally second signal number to PVPOP_SIGPROC().
 * 	Use this to optimize the delivery of SIGHUP/SIGCONT
 * [Bug #11] Add vproc locking in PVPOP_REAP().
 * 
 * Revision 3.25  92/05/06  13:50:45  roman
 * Correct spelling error in prior RCS comment.
 * 
 * Revision 3.24  92/05/06  09:17:44  chrisp
 * Add calls to VPROC_[UN]LOCK_FLAG macros to guard updates to the pvp_flag
 * 	field of pvprocs.
 * In pvpop_sigproc(), don't deliver signal to zombied processes. Also,
 * 	introduced a local pvproc pointer variable to save mulitple PVP(v)s.
 * 
 * Revision 3.23  92/04/30  10:49:03  chrisp
 * [Bug #3] PVPOP_REAP() now takes an additional parameter requiring a reaped
 * 	process belong to a specified pgid (if not WAIT_ANY); error ESRCH
 * 	is returned if this is not the case and the process is not reaped.
 * 	This avoids VPOP_WAIT() using PVPOP_GETATTR() for each child's pgid.
 * 
 * Revision 3.22  92/04/22  12:30:28  chrisp
 * Alter existence rules for session leader vproc, mark:
 * 	- on node of each pgrp leader in session;
 * 	- on node of session leader for each pgrp in session list.
 * 
 * Revision 3.21  92/04/14  13:35:06  roman
 * Simplify the check for putting a process into its own pgrp. This
 * 	situation is caught by the check of adding a process to the
 * 	pgrp it is already a member of.
 * 
 * Revision 3.20  92/04/14  10:27:26  roman
 * To fix a "vproc not in session list" panic: add short-circuit for the no-op
 * 	of setting a process into the process group in which it already is
 * 	a member.
 * 
 * Revision 3.19  92/04/06  16:01:57  chrisp
 * Correct reference counting: a superfluous VPROC_HOLD was being done to
 * init when an orphaned child was reassigned to it.
 * 
 * Revision 3.18  92/03/30  13:43:12  chrisp
 * Bump reference count of init's vproc on node of reassigned child.
 * 
 * Revision 3.17  92/03/19  14:58:08  chrisp
 * Correct creation of new pgrp in setpgid() - pvp_pgrpl must be set NULL.
 * 
 * Revision 3.16  92/03/18  12:10:54  roman
 * Put in locking code (major changes) to support multi-threaded interruptible
 * server. Additionally, some (simple) pvproc ops are now done in-line
 * to avoid lock deadlock problems.
 * 
 * Revision 3.15  92/03/12  15:16:45  roman
 * Major changes to the way orphan pgrps are tracked. This involves now
 * keeping track of the parent's pgid and sid in the pvproc of
 * the child process. This resulted in fewer inter-node operations,
 * and is a lead-in to preparation for adding multi-threading locks.
 * Get rid of resign_pgrp pvproc op, and perform its operations in-line (it
 * was always done on the execution node of the vproc anyway.
 * Add processing for VPROC_CHECK_CTTY option - don't signal if target
 * process has no controlling tty.
 * 
 * Revision 3.14  92/03/03  16:18:30  chrisp
 * Null out pproc pointer on reap (for to see leaderless pgrps).
 * Add ADD_CHILD_TO_PARENT remote op to support the single remote init 
 * architecture.
 * 
 * Revision 3.13  92/02/10  14:29:16  chrisp
 * Move resign pgrp on reap into dpvpop_reap from dpvpop_wait.
 * 
 * Revision 3.12  92/01/30  14:17:16  chrisp
 * Correct REASSIGN_CHILD to release the former parent's vproc on the child's 
 * node.
 * 
 * Revision 3.11  92/01/17  11:08:29  chrisp
 * Remove superfluous "session leader on member node" reference counting.
 * 
 * Revision 3.10  92/01/16  11:40:15  chrisp
 * Correct order of VPROC_RELEASEs in remote reap.
 * 
 * Revision 3.9  92/01/15  10:24:56  chrisp
 * Decrement use count once more if reaping on origin node.
 * 
 * Revision 3.8  92/01/03  11:13:58  chrisp
 * setpgid() checks corrected to reject case when new group is a vproc which
 * is not the subject vproc (becoming a group leader) and is not a group leader. 
 * 
 * Revision 3.7  91/12/20  16:40:11  chrisp
 * Orphan pgrp handling revamped.
 * Killing traced children on parent exit added.
 * SIGUP/SIGCONT signals now dispatched to newly orphaned pgrp.
 * Checks in setpgid now complete.
 * 
 * Revision 3.6  91/12/04  07:51:59  chrisp
 * Add pvpops SET_CTTY and CTTY_GETATTR.
 * Correct instances of if (!x&y) to be if (!(x&y)).
 * 
 * Revision 3.5  91/11/26  11:56:49  chrisp
 * Comments expanded.
 * pvpop_reap corrected to free only zombied vproc (NOT stopped vproc).
 * setsid added as a pvpop.
 * pvpop_sigproc corrected to return errors from pproc_ to caller.
 * 
 * Revision 3.4  91/11/22  10:21:01  chrisp
 * pvpop_set_stop_state generalized to pvpop_set_state.
 * Extra vproc marking added to retain pgrp leader and session leader vprocs
 * after their death until group and sessions are empty.
 * VPROC_HOLD and VPROC_RELEASE messages altered to clarify location of marking.
 * 
 * Revision 3.3  91/11/19  10:09:31  chrisp
 * Remove piggy-backing of PV_SZOMB flag on SIGCHLD.
 * 
 * Revision 3.2  91/11/18  11:32:07  chrisp
 * SIG* extra param.
 * Controlling terminal changes.
 * 
 * Revision 3.1  91/10/30  14:16:12  roman
 * Change some comments on the VPROC_HOLD and VPROC_RELEASE macros. Panic
 * if process is not found in pgrp list rather than silently failing.
 * Add VPROC_RELEASE during ppproc_reap() to conform with new rules for
 * maintaining session leader vprocs.
 * 
 * Revision 3.0  91/10/25  10:27:39  roman
 * First appearance of this file, which extracts the pvproc ops from the
 * files in the pvproc directory, which is being deleted. Also numerous
 * minor bug fixes.
 * 
 */
#include <machine/reg.h>
#include <sys/unix_defs.h>
#include <tnc/dpvproc.h>
#include <sys/vnode.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/ptrace.h>
#include <uxkern/proc_to_task.h>
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
#else
extern int nx_get_info();
#endif /* __STDC__ */

#endif /* NX */

struct rusage_dev zero_ru;	/* zeroed out rusage for dummy addition */

/* Prototypes generated from dpvproc.h */
#include <tnc/dpvproc_protos_gen.h>

/* Forward references */
int dpvproc_orphan_pgrp_check(struct vproc *, pid_t, pid_t);


/*
 * NAME:	dpvpop_reap
 *
 * FUNCTION:	Reap zombie process (or obtain status for stopped process).
 *
 * RETURNS:	ESRCH if a pgid other than WAIT_ANY is given but the process
 *		is not a member of that process group;
 *		ESUCCESS, with wstat being the exit or stop status and ru_loc
 *		containing usage stats if the process is a zombie.
 */
int
dpvpop_reap(
	struct vproc *v,		/* child vproc */
	pid_t *pgid,			/* conditional pgrp membership */
	int options,			/* options to vary system call */
	int *state,			/* child state */
	int *wstat,			/* child termination status */
	struct rusage_dev *ru_loc)	/* pointer to child resource usage */
{
	struct pvproc	*pvp = PVP(v);
	int		error;
	struct rusage_dev tmp_rusage;


        /*
         * Check that we haven't caught a terminating process too early
         * before it's had time to report its zombied state. This could
         * happen if the process is being aborted directly from a previously
         * reported stop state. If this is the case, try again later. To
         * continue here may result in deadlock.
         */
        VPROC_LOCK_FLAG(v, "dpvpop_reap()");
        if ((pvp->pvp_flag & PV_EXITING) && !(pvp->pvp_flag & PV_SZOMB)) {
                VPROC_UNLOCK_FLAG(v, "dpvpop_reap()");
                return(EAGAIN);
        }

	/*
	 * Check for zombie or stopped state as recorded in the vproc.
	 * This check is necessary since a process may have physically exited
	 * and thus be physically reapable before the vproc exit is
	 * complete.
	 */
        if (!(pvp->pvp_flag & (PV_SZOMB | PV_SSTOP))) {
                VPROC_UNLOCK_FLAG(v, "dpvpop_reap()");
                return(ESRCH);
        }
        VPROC_UNLOCK_FLAG(v, "dpvpop_reap()");

	/*
	 * If called with a pgid argument other than WAIT_ANY, check that this
	 * process is a member of this pgrp and return error if not.
	 */
	if (*pgid != WAIT_ANY && *pgid != pvp->pvp_pgid)
		return(ECHILD);

	/*
	 * Note that this lock ensures that we're not reaping
	 * a process that is still in vproc exit code.
	 */
	VPROC_LOCK_EXCL(v, "dpvpop_reap");
	*pgid = pvp->pvp_pgid;
	*state = pvp->pvp_flag & (PV_SZOMB | PV_SSTOP);

	/* First make sure the proc structure is still there */
	if (pvp->pvp_pproc == NULL) {
		VPROC_UNLOCK_EXCL(v, "dpvpop_reap");
		return(ESRCH);
	}

	/* call the base physical code to do the real work */
	error = pproc_reap(pvp->pvp_pproc, options, wstat, &tmp_rusage);

	if (error != ESUCCESS) {
		VPROC_UNLOCK_EXCL(v, "dpvpop_reap");
		return(error);
	}

#ifdef NX
        if (options & WNOWAIT) {
		VPROC_UNLOCK_EXCL(v, "dpvpop_reap");
        	return(ESUCCESS);
	}
#endif /* NX */

	/*
	 * If zombied, resign from process group, release the vproc, its parent
	 * and session leader - noting that the order of release is important
	 * to avoid trashing pvproc fields ppid/sid in the reaped vproc.
	 */
	if ((pvp->pvp_flag&PV_SZOMB) && (!(options & VPROC_WNOWAIT))) {

		/*
		 * If there is a "true" foster parent, then have the
		 * foster parent inherit the resource usage statistics.
		 *
		 * Note: after this inheritance, the foster_ppid field
		 * is set to zero so that the foster parent doing
		 * a PVPOP_REASSIGN_CHILD() will know that the foster
		 * child was reaped.
		 */
		if (pvp->pvp_foster_ppid != pvp->pvp_ppid) {
			struct vproc *fvp;

			fvp = VPROCPTR(pvp->pvp_foster_ppid);
			ASSERT(fvp != NULL);
			(void) PVPOP_RMV_ORPHAN_FROM_FOSTER_LIST(fvp, 
								 v, 
								 &tmp_rusage);
			VPROC_RELEASE(fvp,"dpvpop_reap(foster parent)");
			*ru_loc = zero_ru;
			pvp->pvp_foster_ppid = 0;
		} else
			*ru_loc = tmp_rusage;

		/*
		 * Null out the proc pointer so that if the vproc persists
		 * as a non-empty pgrp or session, the death of the leader can
		 * be inferred.
		 */ 
		pvp->pvp_pproc = NULL;

		/* Resign from the pgrp */
		if (pvp->pvp_pgid != 0) {
			register struct vproc *g = VPROCPTR(pvp->pvp_pgid);
			/*
			 * Remove from pgrp list unless parent is leader.
			 */
			if (pvp->pvp_pgid != pvp->pvp_ppid)
				(void) PVPOP_RMV_PGRP_LIST(g, v, 0);
			VPROC_RELEASE(g, 
				      "dpvpop_reap(pgrp ldr on member node)");
		}

		VPROC_RELEASE(VPROCPTR(pvp->pvp_ppid),
			      "dpvpop_reap(parent on child node)");

		/*
		 * Sure, this looks wrong: 
		 *	VPROC_RELEASE(v);
		 *	VPROC_UNLOCK_EXCL(v);
		 *
		 * BUT, it's OK in this instance:
		 *
		 * There are 2 cases to consider about when dpvpop_reap()
		 * is called to reap a child process: when the child is
		 * local to the parent and when the child is remote.  In
		 * the local case, the child vproc has an extra reference
		 * because it is on its parent's child-list and this reference
		 * isn't decremented until after PVPOP_REAP() returns to
		 * dvpop_wait(). So the child vproc doesn't go away until
		 * then (earliest). In the case where the child is on a node
		 * other than its parent's, this argument doesn't apply.
		 * But instead, since dpvpop_reap() is called as a remote
		 * operation, there's an extra reference held on the target
		 * vproc (the child's) for the duration of the remote operation.
		 * Hence, the child vproc won't be vaporized until the end
		 * of the operation - after dpvpop_reap() has returned.
		 */
		VPROC_RELEASE(v,"dpvpop_reap(active)");
		if (VPROC_IS_AT_ORIGIN(v))
			VPROC_RELEASE(v,"dpvpop_reap(origin)");
	}

	VPROC_UNLOCK_EXCL(v, "dpvpop_reap");
	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_add_child_to_parent
 *
 * FUNCTION:	Add child vproc to adopted parent's parent-child-sibling list.
 *
 * RETURNS:	ESUCCESS on completion.
 *
 */
int
dpvpop_add_child_to_parent(
	struct vproc	*pp,	/* parent */
	struct vproc	*v,	/* child */
	int		state)	/* state of child - zombie | stopped */
{
	/*
	 * Add child to its new parent's parent-sibling-child list
	 */
	VPROC_LOCK_EXCL(pp, "dpvpop_add_child_to_parent(parent)");
	PVP(v)->pvp_childl = PVP(pp)->pvp_head_childl;
	PVP(pp)->pvp_head_childl = v;
	VPROC_HOLD(v, "dpvpop_add_child_to_parent(child)");

	/*
	 * Ensure that the state is correct on the new parent's vproc
	 */
	VPROC_LOCK_FLAG(v, "dpvpop_add_child_to_parent");
	PVP(v)->pvp_flag &= ~(PV_SZOMB|PV_SSTOP);
	PVP(v)->pvp_flag |= state;
	VPROC_UNLOCK_FLAG(v, "dpvpop_add_child_to_parent");
	VPROC_UNLOCK_EXCL(pp, "dpvpop_add_child_to_parent(parent)");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_rmv_child_from_parent
 *
 * FUNCTION:	Remove child vproc from its parent's parent-child-sibling list.
 *
 * RETURNS:	ESUCCESS on completion (panics if vproc not in list).
 *
 */
int
dpvpop_rmv_child_from_parent(
	struct vproc *pp,	/* parent */
	struct vproc *v)	/* child */
{
	register struct vproc *w;

	VPROC_LOCK_EXCL(pp, "dpvpop_add_child_to_parent(parent)");

	if (PVP(pp)->pvp_head_childl == v)
		PVP(pp)->pvp_head_childl = PVP(v)->pvp_childl;
	else {
		for (w = PVP(pp)->pvp_head_childl;
				 w && PVP(w)->pvp_childl != v;
				 w = PVP(w)->pvp_childl)
			{ ; }

		if (w == NULL)
			panic("dpvpop_child_from_parent: "
			      "vproc [%d] not on parent [%d]'s list",
				v->vp_pid, pp->vp_pid);

		PVP(w)->pvp_childl = PVP(v)->pvp_childl;
	}

	/* update the vproc reference count */
	VPROC_RELEASE(v, "dpvpop_rmv_child_from_parent(child)");

	VPROC_UNLOCK_EXCL(pp, "dpvpop_add_child_to_parent(parent)");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_reassign_child
 *
 * FUNCTION:	Reassign a process as a child of init.
 *
 * RETURNS:	ESUCCESS on completion.
 *		ESRCH if child was reaped before the vproc could be locked
 *
 */
int
dpvpop_reassign_child(
	struct vproc *v,		/* vproc of process being reassigned */
	struct vproc *fvp,		/* vproc of foster parent */
	int flags)			/* indicating whether parent exiting */
{
	register struct pvproc	 *pvp = PVP(v);
	register struct vproc	 *vp = VPROCPTR(pvp->pvp_ppid);
	register struct vproc	 *vi = VPROCPTR(1);

	VPROC_LOCK_EXCL(v, "dpvpop_reassign_child");

	/*
	 * If there is an orphaned parent, then add the child to the
	 * orphaned parent's list.
	 *
	 * Note that the order of operations below is extremely important:
	 * the reassignment of foster_ppid MUST take place before the
	 * PVPOP_ADD_ORPHAN_TO_FOSTER_LIST().
	 *
	 * Note also that the operation on the child process is done with
	 * no locks whatsoever. Should work, though.
	 */
	if (flags & (PV_EXIT | PV_FOSTER_ONLY)) {

		/*
		 * If the vproc has been reaped (in a timing window
		 * between the call to PVPOP_REASSIGN_CHILD() and the
		 * VPROC_LOCK_EXCL() above), then don't attempt to
		 * do anything.
		 */
		if (pvp->pvp_foster_ppid == 0) {
			ASSERT(pvp->pvp_flag & PV_SZOMB);
			ASSERT(flags & PV_FOSTER_ONLY);
			VPROC_UNLOCK_EXCL(v, "dpvpop_reassign_child");
			return(ESRCH);
		}

		/*
		 * Remove vproc from the old foster child list (if it was
		 * on one).
		 */
		if (pvp->pvp_foster_ppid != pvp->pvp_ppid) {
			struct vproc *ofvp = VPROCPTR(pvp->pvp_foster_ppid);
			(void) PVPOP_RMV_ORPHAN_FROM_FOSTER_LIST(ofvp, 
								 v, 
								 &zero_ru);
			VPROC_RELEASE(ofvp,
				      "dpvpop_reassign_child(foster parent)");
		}

		/*
		 * Add vproc to new foster child list provided that the
		 * intended foster parent isn't init (the reap code assumes
		 * that (ppid == foster_ppid) means that the process is not
		 * on any foster child list)
		 */
		if (fvp->vp_pid != 1) {
			VPROC_HOLD(fvp, "dpvpop_reassign_child(foster parent)");
			(void) PVPOP_ADD_ORPHAN_TO_FOSTER_LIST(fvp, v);
			pvp->pvp_foster_ppid = fvp->vp_pid;
		} else {
			/* Do not put on foster child list since either init
			 * is already the parent or is about to become the
			 * parent.
			 */
			ASSERT(pvp->pvp_ppid == 1 || !(flags & PV_FOSTER_ONLY));
			pvp->pvp_foster_ppid = 1;
		}
	}

	/*
	 * If no real child reassignment takes place, then we're done.
	 */
	if (flags & PV_FOSTER_ONLY) {
		VPROC_UNLOCK_EXCL(v, "dpvpop_reassign_child");
		return(ESUCCESS);
	}

	/*
	 * The process being reassigned may cause its pgrp to
	 * become orphaned.
	 */
	if (pvp->pvp_pgid != 0 &&
			pvp->pvp_sid == pvp->pvp_pp_sid && 
			pvp->pvp_pgid != pvp->pvp_pp_pgid)
		(void) PVPOP_ADJUST_JOB_CONTROL_COUNT(VPROCPTR(pvp->pvp_pgid),
						      PV_SUBTRACT | flags);
	
	/*
	 * Reassign as new child of INIT. 
	 *
	 * Note that if neither PV_EXIT nor PV_FOSTER_ONLY is set then 
	 * the foster parent is also set to INIT.
	 */
	VPROC_HOLD(vi, "dpvpop_reassign_child(init)");
	(void) PVPOP_ADD_CHILD_TO_PARENT(vi, v, pvp->pvp_flag &
						(PV_SZOMB|PV_SSTOP));
	pvp->pvp_ppid = 1;
	if ((flags & PV_EXIT) == 0)
		pvp->pvp_foster_ppid = 1;
	pproc_set_attr(pvp->pvp_pproc, 0, 0, &pvp->pvp_ppid, 0, 0, 0);
	pvp->pvp_pp_pgid = 1;
	pvp->pvp_pp_sid = 1;

	/*
	 * If state reporting is enabled through an elder process,
	 * disable this, release the associated vproc, and clear
	 * the has-reported flag for completeness.
	 */
	if (pvp->pvp_epid != 0) {
		if (pvp->pvp_epid != v->vp_pid)
			VPROC_RELEASE(VPROCPTR(pvp->pvp_epid),
				      "dpvpop_reassign_child(elder)");
		pvp->pvp_epid = 0;
		VPROC_LOCK_FLAG(v, "dpvpop_reassign_child");
		pvp->pvp_flag &= ~PV_HAS_REPORTED;
		VPROC_UNLOCK_FLAG(v, "dpvpop_reassign_child");
	}

	VPROC_UNLOCK_EXCL(v, "dpvpop_reassign_child");

	/* release the former parent vproc on the child's node */
	VPROC_RELEASE( vp, "dpvpop_reassign_child(parent on child node");

	/*
	 * Do special processing on child node if parent exits
	 */
	if ((flags & PV_EXIT) != 0) {
		if ((pvp->pvp_flag & PV_SZOMB) != 0)
			/* if proc is a zombie, signal INIT to clean up */
			(void) PVPOP_SIGPROC(vi, SIGCHLD, 
					    0, 0, 0, 0, 0, 0, 
					    VPROC_HAS_PRIV);
		else
			/* kill the child if it's being traced */
			(void) PVPOP_SIGPROC(v, SIGKILL, 
					    0, 0, 0, 0, 0, 0, 
					    VPROC_HAS_PRIV | VPROC_IFTRACED);
	}

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_add_orphan_to_foster_list
 *
 * FUNCTION:	Add orphaned vproc to foster parent's foster child list
 *
 * RETURNS:	ESUCCESS on completion.
 *
 */
int
dpvpop_add_orphan_to_foster_list(
	struct vproc	*fpp,	/* foster parent */
	struct vproc	*ovp)	/* orphaned child */
{
	VPROC_LOCK_FOSTER_LIST_EXCL(fpp, "dpvpop_add_orphan_to_foster_list");

	ASSERT(!(PVP(fpp)->pvp_flag & PV_SZOMB));

	/*
	 * Add child to foster parent's list
	 */
	PVP(ovp)->pvp_foster_childl = PVP(fpp)->pvp_head_foster_childl;
	PVP(fpp)->pvp_head_foster_childl = ovp;
	VPROC_HOLD(ovp, "dpvpop_add_orphan_to_foster_list(orphan)");

	VPROC_UNLOCK_FOSTER_LIST_EXCL(fpp, "dpvpop_add_orphan_to_foster_list");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_rmv_orphan_from_foster_list
 *
 * FUNCTION:	Add rusage to orphaned parent's rusage
 *
 * RETURNS:	ESUCCESS on completion.
 *
 */
int
dpvpop_rmv_orphan_from_foster_list(
	struct vproc		*fpp,	/* foster parent */
	struct vproc		*ovp,	/* orphaned child */
	struct rusage_dev	*ru_loc) /* pointer to child resource usage */
{
	struct vproc 		*v;

	VPROC_LOCK_FOSTER_LIST_EXCL(fpp, "rmv_orphan_from_foster_list");
	
	/*
	 * Remove the orphaned child from the foster parent's list. 
	 */
	if (PVP(fpp)->pvp_head_foster_childl == ovp)
		PVP(fpp)->pvp_head_foster_childl = PVP(ovp)->pvp_foster_childl;
	else {
		for (v = PVP(fpp)->pvp_head_foster_childl;
				v && PVP(v)->pvp_foster_childl != ovp;
				v = PVP(v)->pvp_foster_childl)
			{ ; }

		if (v == NULL)
			panic("dpvpop_rmv_orphan_from_foster_list, "
			      "vproc [%d] not on foster parent [%d]'s list",
				ovp->vp_pid, fpp->vp_pid);

		PVP(v)->pvp_foster_childl = PVP(ovp)->pvp_foster_childl;
	}
	
	/*
	 * Add to rusage of physical process.
	 */
	(void) pproc_add_rusage(PVP(fpp)->pvp_pproc, ru_loc);

	/*
	 * Release the orphaned child since it's not on the list any more.
	 */
	VPROC_RELEASE(ovp, "dpvpop_rmv_orphan_from_foster_list(orphan)");

	VPROC_UNLOCK_FOSTER_LIST_EXCL(fpp, "rmv_orphan_from_foster_list");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_setpgid
 *
 * FUNCTION:	Change the process group id of a process.
 *
 * RETURNS:	EPERM, if the process id a session leader
 *		EPERM, if the new group is outside the process' session.
 *		ESRCH, if the process is not the caller & not caller's child
 *		EACCES, if the proc has ever exec'd
 *		ESUCCESS on sucessful completion.
 */
int
dpvpop_setpgid(
	struct vproc *v,	/* vproc of target process */
	struct vproc *g,	/* vproc of process group to be assigned */
	pid_t pid,		/* pid of calling process */
	pid_t sid)		/* session id of calling process */ 
{
	register struct vproc	*vs;	/* session leader vproc */
	struct pvproc		*pvp = PVP(v);
	pid_t			group_sid;
	int			group_jobc;
	int			error = ESUCCESS;
	int			seqno;

	VPROC_LOCK_EXCL(v, "dpvpop_setpgid(process)");

	/*
	 * First, some checks...
	 *
	 * The process having its group set must be the caller
	 * or a child of the caller. If a child, it must not have exec'ed
	 */
	if (v->vp_pid != pid) {
		if (pvp->pvp_ppid != pid) {
			error = ESRCH;
			goto out;
		} else {
			int has_execd;
			(void) pproc_get_attr(pvp->pvp_pproc,
					      0,0,0,0,0,0,0,0,&has_execd);
			if (has_execd) {
				error = EACCES;
				goto out;
			}
		}
	}

	/*
	 * The process having its group set cannot be a session leader.
	 */
	if (pvp->pvp_flag & PV_SESSIONLEADER) {
		error = EPERM;
		goto out;
	}

	/*
	 * If the process to be set is a child of the caller,
	 * the child must be in the same session as the caller.
	 */
	if (pvp->pvp_ppid == pid && pvp->pvp_sid != sid) {
		error = EPERM;
		goto out;
	}

	/*
	 * The process group must be the process itself
	 * or a group in the same session as the caller.
	 */
	if (g->vp_pid != v->vp_pid) {
		if (VPOP_GET_ATTR(g,0,0,&group_sid,0,0,&group_jobc,0) != ESUCCESS) {
			error = ESRCH;
			goto out;
		}
		if (group_jobc == VPROC_NOT_PGRPLEADER || group_sid != sid) {
			error = EPERM;
			goto out;
		}
	} else {
		if ((pvp->pvp_flag & PV_PGRPLEADER) == 0)
			group_jobc = VPROC_NOT_PGRPLEADER;
		else
			group_jobc = (pvp->pvp_jobc != 0) ? VPROC_JOB_CONTROL
							  : VPROC_ORPHANED;
	}

	/*
	 * To set a process into a group of which it is already a member
	 * is short-circuited to avoid fruitless process group manipulations.
	 * In addition, the premature removal of the pgrp vproc from its
	 * session list in the case when the process leader no longer exists
	 * but has a single member, would be erroneous.
	 *
	 * Shells frequently indulge in behavior of performing a series
	 * of setpgid(pid,pid) calls for a process (POSIX recommends
	 * doing it twice - once from the parent and once from the child,
	 * but shells have been spotted doing this four times per process).
	 * As this operation can cause a pgrp to be torn down
	 * (when resigning from the pgrp) and then set-up again, messages to
	 * the session leader (which may be on a different node) can
	 * occur.
	 */
	if (g->vp_pid != pvp->pvp_pgid) {

		ASSERT(v->vp_pid != g->vp_pid ||
			(pvp->pvp_flag & PV_PGRPLEADER) != 0 ||
			pvp->pvp_jobc == 0);
		/*
		 * This operation may orphan the old pgrp or unorphan the new 
		 * pgrp or it may either orphan or unorphan the pgrps of 
		 * children of this process.
		 */
		dpvproc_orphan_pgrp_check(v, g->vp_pid, pvp->pvp_sid);

		/* resign from current group */
		if (pvp->pvp_pgid != 0) {
			register struct vproc *og = VPROCPTR(pvp->pvp_pgid);
			(void) PVPOP_RMV_PGRP_LIST(og, v, pvp->pvp_ppid);
			VPROC_RELEASE(og, "dvpop_setpgid(old pgrp leader on member node)");
		}

		/* Create a pgrp if necessary */
		if (group_jobc == VPROC_NOT_PGRPLEADER) {
			ASSERT(v->vp_pid == g->vp_pid);
			if (pvp->pvp_sid) {
				/*
				 * add new pgrp to session list
				 * noting that the LOCATE marks the session
				 * vproc on the node of the new pgrp leader
				 */
				vs = LOCATE_VPROC_PID(pvp->pvp_sid);
				PVPOP_ADD_SESSION_LIST(vs, v);
			}

			/*
			 * Add process to its own pgrp list - effectively, this
			 * is an inline version of PVPOP_ADD_PGRP_LIST(v,v,...).
                         * Note that taking the process group list lock can be
                         * avoided provided that the leadership flag is not
                         * set until we've put ourself in our own pgrp list
                         * and we don't set our new pgid earlier than this.
                         * Thus, we aren't visible as a process group
                         * while the list is being initialized.
			 */
			ASSERT(pvp->pvp_head_pgrpl == NULL);
                	pvp->pvp_pgrp_ldr_seqno = 0;
                	pvp->pvp_pgrp_mem_seqno = 0;
			pvp->pvp_head_pgrpl = v;
			pvp->pvp_pgrpl = NULL;
			VPROC_HOLD(v, "dpvpop_setpgid(member on leader node)");
			VPROC_HOLD(v, "dpvpop_setpgid(leader on leader node)");
			VPROC_HOLD(v, "dpvpop_setpgid(leader on member node)");
			VPROC_LOCK_FLAG(v, "dpvpop_setpgid");
			pvp->pvp_flag |=  PV_PGRPLEADER;
			VPROC_UNLOCK_FLAG(v, "dpvpop_setpgid");
		} else {

			/*
			 * Add process to pgrp list
			 */
                	pvp->pvp_pgrp_ldr_seqno = 0;
			pvp->pvp_pgrp_mem_seqno = -1;	/* obtains new seqno */
			PVPOP_ADD_PGRP_LIST(g, v, &pvp->pvp_pgrp_mem_seqno);
			VPROC_HOLD(g,
			    "dpvpop_setpgid(new pgrp leader on member node)");
		}

		/*
		 * Finally, set the new process group id.
		 */
		pvp->pvp_pgid = g->vp_pid;

		/* keep physical proc updated */
		pproc_set_attr(pvp->pvp_pproc,
			       0,0,0,
			       &pvp->pvp_pgid,
			       &pvp->pvp_sid,
			       0);
	}

out:
	VPROC_UNLOCK_EXCL(v, "dpvpop_setpgid(process)");
	return(error);
}


/*
 * NAME:	dpvpop_setsid
 *
 * FUNCTION:	Create a new session.
 *
 * RETURNS:	EPERM, if the vproc is a process group leader.
 *		ESUCCESS, when the session has been created.
 *
 */
int
dpvpop_setsid(
	struct vproc *v)	/* vproc of process becoming session leader */
{
	register struct pvproc	*pvp;
	boolean_t		false = FALSE;
	int			error = ESUCCESS;

	pvp = PVP(v);

	VPROC_LOCK_EXCL(v, "dpvpop_setsid");

	if (pvp->pvp_pgid == v->vp_pid ||
	    pvp->pvp_flag & PV_PGRPLEADER) {
		error = EPERM;
		goto out;
	}

	/* This may orphan old pgrp or pgrps of children */
	ASSERT(pvp->pvp_jobc == 0);
	dpvproc_orphan_pgrp_check(v, v->vp_pid, v->vp_pid);

	/* resign from current process group */
	if (pvp->pvp_pgid != 0) {
		register struct vproc *g = VPROCPTR(pvp->pvp_pgid);
		PVPOP_RMV_PGRP_LIST(g, v, pvp->pvp_ppid);
		VPROC_RELEASE(g, "dpvpop_setsid(old pgrp leader on member node)");
	}

	/*
	 * Add process to its own pgrp list - effectively, this
	 * is an inline version of PVPOP_ADD_PGRP_LIST(v,v,...)
	 */
	ASSERT(pvp->pvp_head_pgrpl == NULL);
	pvp->pvp_head_pgrpl = v;
	pvp->pvp_pgrpl = NULL;
	VPROC_HOLD(v, "dpvpop_setsid(pgrp member on leader node)");
	VPROC_HOLD(v, "dpvpop_setsid(pgrp leader on leader node)");
	VPROC_HOLD(v, "dpvpop_setsid(pgrp leader on member node)");

	/*
	 * Add pgrp to session list - except the session leader isn't
	 * so we just bump reference counts.
	 */ 
	ASSERT(pvp->pvp_sessionl == NULL);
	VPROC_HOLD(v, "dpvpop_setsid(session leader on leader node)");
	VPROC_HOLD(v, "dpvpop_setsid(session member on leader node)");
	VPROC_HOLD(v, "dpvpop_setsid(session leader on member node)");

	/* keep physical proc updated */
	pproc_set_attr(pvp->pvp_pproc,0,0,0,&v->vp_pid,&v->vp_pid,&false);

	/*
	 * Assign to the vproc the new sess & pgrp ids, and set its
	 * count of pgrp-style signals sent and received to zero.
	 */
	pvp->pvp_sid  = pvp->pvp_pgid = v->vp_pid;
	pvp->pvp_pgrp_ldr_seqno = 0;
	pvp->pvp_pgrp_mem_seqno = 0;

	VPROC_LOCK_FLAG(v, "dpvpop_setsid");
	pvp->pvp_flag &= ~PV_SCTTY;
	pvp->pvp_flag |= (PV_PGRPLEADER | PV_SESSIONLEADER);
	VPROC_UNLOCK_FLAG(v, "dpvpop_setsid");

#ifdef NX
        /*
         * Since we are forming a new session and pgroup, there cannot
         * be a tam monitoring this process yet.
         */
        VPOP_REPORT_STATE(v, VPROC_AGAIN_SIGCHLD);
#endif /* NX */

out:
	VPROC_UNLOCK_EXCL(v, "dpvpop_setsid");
	return(error);
}


/*
 * NAME:	dpvproc_orphan_pgrp_check
 *
 * FUNCTION:	Re-assess elibility to job control due to change of pgrp.
 *		This assumes that the vproc in question has been locked.
 *
 * RETURNS:	None
 */
int
dpvproc_orphan_pgrp_check(
	struct vproc *v,	/* vproc of process changing or exiting */
	pid_t new_pgid,		/* pgrp we are changing to */
	pid_t new_sid)		/* session we are changing to */
{
	register struct pvproc *pvp = PVP(v);
	register struct vproc *vc;

	/*
	 * Subtract from current pgrp's jobc if we were contributing to this 
	 * pgrp being unorphaned
	 */
	if (pvp->pvp_pgid != 0
			&& pvp->pvp_pp_sid == pvp->pvp_sid
			&& pvp->pvp_pp_pgid != pvp->pvp_pgid)
		(void) PVPOP_ADJUST_JOB_CONTROL_COUNT(VPROCPTR(pvp->pvp_pgid),
						      PV_SUBTRACT);
	/*
	 * Add to new pgrp's jobc if we are going to contribute to it
	 * being unorphaned.
	 */
	if (new_pgid != 0
			&& pvp->pvp_pp_sid == new_sid
			&& pvp->pvp_pp_pgid != new_pgid)
		(void) PVPOP_ADJUST_JOB_CONTROL_COUNT(VPROCPTR(new_pgid),
						      PV_ADD);

	/*
	 * Inform all children of the changed status of the parent. This
	 * should be an infrequent operation in practice, since a parent
	 * seldom changes process groups or sessions.
	 */
	for (vc = pvp->pvp_head_childl; vc; vc = PVP(vc)->pvp_childl)
		(void) PVPOP_SET_ATTR(vc, new_pgid, new_sid);
}


/*
 * NAME:	dpvpop_add_pgrp_list
 *
 * FUNCTION:	Add a process to a process group list.
 *
 * RETURNS:	ESUCCESS on completion.
 *
 */
int
dpvpop_add_pgrp_list(
	struct vproc *g,	/* vproc of process group leader */
	struct vproc *v,	/* vproc of process being added to group */
	int *seqnop)		/* INOUT seq num we must match, or -1 */
{
	struct vproc *w;	/* group member pointer */
	int error = ESUCCESS;

	ASSERT(PVP(g)->pvp_flag & PV_PGRPLEADER);

	VPROC_LOCK_PGRP_LIST_EXCL(g, "dpvpop_add_pgrp_list");
	if (*seqnop != PVP(g)->pvp_pgrp_ldr_seqno && *seqnop != -1) {
		if (*seqnop < PVP(g)->pvp_pgrp_ldr_seqno) {
			error = EAGAIN;
			goto out;
		}
		panic("dpvpop_add_pgrp_list: member seqno > leader (%d > %d)",
				*seqnop, PVP(g)->pvp_pgrp_ldr_seqno);
	}
	PVP(v)->pvp_pgrpl = PVP(g)->pvp_head_pgrpl;
	PVP(g)->pvp_head_pgrpl = v;
	VPROC_HOLD(g, "dpvpop_add_pgrp_list(leader on leader node)");
	VPROC_HOLD(v, "dpvpop_add_pgrp_list(member on leader node)");
	*seqnop = PVP(g)->pvp_pgrp_ldr_seqno;

out:
	VPROC_UNLOCK_PGRP_LIST_EXCL(g, "dpvpop_add_pgrp_list");
	return(error);
}


/*
 * NAME:	dpvpop_rmv_pgrp_list
 *
 * FUNCTION:	Remove process from a process group list
 *
 * RETURNS:	ESUCCESS on completion (panics if vproc not in list).
 *
 */
int
dpvpop_rmv_pgrp_list(
	struct vproc *g,	/* vproc of process group leader */
	struct vproc *v,	/* vproc of process being removed from group */
	pid_t pp)		/* parent of v, if parent is to be informed */
{
	register struct vproc *w;
	register struct vproc *s;
#ifdef NX
	pid_t	report_allocator = 0;
#endif /* NX  */

	ASSERT(PVP(g)->pvp_flag & PV_PGRPLEADER);

	VPROC_LOCK_PGRP_LIST_EXCL(g, "dpvpop_rmv_pgrp_list");
	if (PVP(g)->pvp_head_pgrpl == v)
		/* v is the first in the list */
		PVP(g)->pvp_head_pgrpl = PVP(v)->pvp_pgrpl;
	else {
		/* walk through the remaining list looking for v */
		for (w = PVP(g)->pvp_head_pgrpl; 
				w && PVP(w)->pvp_pgrpl != v; 
				w = PVP(w)->pvp_pgrpl)
			{ ; }

		if (w == NULL)
			panic("dpvpop_rmv_pgrp_list: "
			      "vproc [%d] not in pgrp [%d]",
			      v->vp_pid, g->vp_pid);

		PVP(w)->pvp_pgrpl = PVP(v)->pvp_pgrpl;
	}

	/* is anyone still here ? */
	if (PVP(g)->pvp_head_pgrpl == NULL) {
#ifdef NX
		
		if( nx_application_vproc(g))
			report_allocator = g->vp_pid;
#endif /* NX */

		/* no more process group - resign from session too */
		if (PVP(g)->pvp_sid != 0) {
			s = VPROCPTR(PVP(g)->pvp_sid);
			PVPOP_RMV_SESSION_LIST(s, g);
			VPROC_RELEASE(s, "dpvpop_rmv_pgrp_list(session on member node)");
		}
		/* clear the flag indicating pgrp leadership */
		VPROC_LOCK_FLAG(v, "dpvpop_rmv_pgrp_list");
		PVP(g)->pvp_flag &= ~PV_PGRPLEADER;
		VPROC_UNLOCK_FLAG(v, "dpvpop_rmv_pgrp_list");

#ifdef PARACORE
		/*
		 * Free the pvp_core_data structure.
		 */
		if ( PVP(g)->pvp_core_data != NULL ) {
			(void) free_pvp_core_data(PVP(g)->pvp_core_data);
			PVP(g)->pvp_core_data = NULL;
		}
#endif /* PARACORE */
	}

	VPROC_UNLOCK_PGRP_LIST_EXCL(g, "dpvpop_rmv_pgrp_list");

	/* Inform parent of v if necessary. Usually parent == vproc g */
	if (pp != 0) {
		register struct vproc *ppv = LOCATE_VPROC_PID(pp);
		ASSERT(ppv != NULL);
		(void) PVPOP_REPORT_STATE(ppv, v, VPROC_LEFT_PGRP);
		VPROC_RELEASE(ppv, "dpvpop_rmv_pgrp_list(parent)");
	}

	VPROC_RELEASE(g, "dpvpop_rmv_pgrp_list(leader on leader node)");
	VPROC_RELEASE(v, "dpvpop_rmv_pgrp_list(member on leader node)");

#ifdef NX

	if( report_allocator )
	    nx_report_allocator(report_allocator);
#endif /* NX */
	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_add_session_list
 *
 * FUNCTION:	Add a process group leader to a session list.
 *
 * RETURNS:	ESUCCESS on completion.
 *
 */
int
dpvpop_add_session_list(
	struct vproc *s,	/* vproc of session leader */
	struct vproc *g)	/* vproc of group leader being added to sess */
{
	VPROC_LOCK_SESSION_LIST_EXCL(s, "dpvpop_add_session_list");

	if (g == s) {
		/* we're adding ourself to create a new session */
		PVP(g)->pvp_sessionl = NULL;
		PVP(g)->pvp_sid = s->vp_pid;
	} else {
		/* insert g at the head to the session list */
		PVP(g)->pvp_sessionl = PVP(s)->pvp_sessionl;
		PVP(s)->pvp_sessionl = g;
	}
	/* another reason to hang on to both vprocs */
	VPROC_HOLD(s, "dpvpop_add_session_list(leader on leader node)");
	VPROC_HOLD(g, "dpvpop_add_session_list(member on leader node)");

	VPROC_UNLOCK_SESSION_LIST_EXCL(s, "dpvpop_add_session_list");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_rmv_session_list
 *
 * FUNCTION:	Remove a process group leader from a session.
 *
 * RETURNS:	ESUCCESS on completion (panic if vproc not found).
 *
 */
int
dpvpop_rmv_session_list(
	struct vproc *s,	/* vproc of session leader */
	struct vproc *g)	/* group leader being removed from session */
{
	register struct vproc *w;

	VPROC_LOCK_SESSION_LIST_EXCL(s, "dpvpop_rmv_session_list");

	if (s != g) {
		/* walk the session list chain looking for g */
		for (w = s; w && PVP(w)->pvp_sessionl != g;
				w = PVP(w)->pvp_sessionl)
			{ ; }

		if (w == NULL)
			panic("dpvpop_rmv_session_list: "
			      "vproc [%d] not on session [%d]'s list",
			      g->vp_pid, s->vp_pid);

		PVP(w)->pvp_sessionl = PVP(g)->pvp_sessionl;
	}
	VPROC_RELEASE(s, "dpvpop_rmv_session_list(leader on leader node)");
	VPROC_RELEASE(g, "dpvpop_rmv_session_list(member on leader node)");

	VPROC_UNLOCK_SESSION_LIST_EXCL(s, "dpvpop_rmv_session_list");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_adjust_job_control_count
 *
 * FUNCTION:	Update pgrp job count for process group.
 *
 * RETURNS:	EPERM, if the specified vproc is not a process group leader.
 * 		ESUCCESS on completion.
 */
int
dpvpop_adjust_job_control_count(
	struct vproc *g,	/* vproc of pgrp leader */
	int  cause)		/* PV_ADD, PV_SUBTRACT, or PV_EXIT */
{
	register struct pvproc *pvp = PVP(g);

	VPROC_LOCK_PGRP_LIST_EXCL(g, "dpvpop_adjust_job_control_count");

	/*
	 * Whether a process group is not orphaned is determined by there
	 * being at least one member with a parent in the same session
	 * but in a different process group. The count of such process
	 * group members is the jobc field. It is incremented as each
	 * such process joins the group, and decremented as each such member
	 * leaves the group. If such a member (or its parent) exits, the
	 * count is decremented and if it reaches 0 the group becomes
	 * orphaned and the group is signaled.
	 */
	if ((cause & PV_ADD) != 0)
		pvp->pvp_jobc++;
	else
		pvp->pvp_jobc--;

	/*
	 * Here if the group has been orphaned by an exiting process.
	 * Signal the group with SIGUP/SIGCONT if any member is stopped.
	 */
	if ((cause & PV_EXIT) != 0 && pvp->pvp_jobc == 0) {
		register struct vproc *q;
		for (q = pvp->pvp_head_pgrpl; q != NULL; q = PVP(q)->pvp_pgrpl){
			/*
			 * post SIGHUP/SIGCONT conditionally on the process
			 * being stopped
			 */
			if (PVPOP_SIGPROC(q, SIGHUP, SIGCONT, 
					  0, 0, 0, 0, 0,
					  VPROC_HAS_PRIV|VPROC_IFSTOPPED) 
							== ESUCCESS) {
				register struct vproc *r;
				/*
				 * and scan again through all other members 
				 * sending * them the signal pair
				 */
				for (r = pvp->pvp_head_pgrpl; 
						r; 
						r = PVP(r)->pvp_pgrpl) {
					/* skip the first stopped process */
					if (r == q)
						continue;
					(void) PVPOP_SIGPROC(r,
							     SIGHUP, SIGCONT, 
					  		     0, 0, 0, 0, 0,
					  		     VPROC_HAS_PRIV); 
				}
				break;
			}
		}
	}

	VPROC_UNLOCK_PGRP_LIST_EXCL(g, "dpvpop_adjust_job_control_count");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_proc_nice
 *
 * FUNCTION:	Read/write process nice value.
 *
 * RETURNS:	EINVAL, if operation is neither VPROC_GET or VPROC_SET.
 *		EPERM, if setting is not permitted for the caller.
 *		ESUCCESS on completion, with nice value returned if VPROC_SET.
 */
int
dpvpop_proc_nice(
	struct vproc *v,	/* vproc of target process */
	int *nice,		/* nice to set or returned value */
	uid_t euid,		/* effective uid of caller */
	uid_t ruid,		/* real uid of caller */
	int flag)		/* set or get */
{
	switch (flag&(VPROC_SET|VPROC_GET)) {
	case VPROC_GET:
		return(pproc_get_nice(PVP(v)->pvp_pproc, nice));
	case VPROC_SET:	
		return(pproc_set_nice(PVP(v)->pvp_pproc,
			euid, ruid, flag&VPROC_HAS_PRIV, *nice));
	default:
		return(EINVAL);
	}
}
	

/*
 * NAME:	dpvpop_pgrp_nice
 *
 * FUNCTION:	 Read/write process group nice value.
 *
 * RETURNS:	EINVAL, if operation is neither VPROC_GET or VPROC_SET.
 *		ESRCH, if the process group is empty.
 *		EPERM, if setting is not permitted for the caller.
 *		ESUCCESS on completion, with nice value returned if VPROC_SET.
 */
int
dpvpop_pgrp_nice(
	struct vproc *g,	/* vproc of target group leader process */
	int *nice,		/* nice to set or returned value */
	uid_t euid,		/* effective uid of caller */
	uid_t ruid,		/* real uid of caller */
	int flag)		/* set or get */
{
	register struct vproc *v;
	register low = PRIO_MAX + 1;
	register found = 0, error = 0;
	int member_nice;

	switch (flag&(VPROC_SET|VPROC_GET)) {
	case VPROC_GET:
		/* visit all group members to compute lowest nice value */
		VPROC_LOCK_PGRP_LIST_SHARED(g, "dpvpop_pgrp_nice");
		for (v = PVP(g)->pvp_head_pgrpl; 
				v != NULL; 
				v = PVP(v)->pvp_pgrpl) {
			(void) PVPOP_PROC_NICE(v, &member_nice, 0, 0, flag);
			if ( member_nice < low )
				low = member_nice;
		}
		VPROC_UNLOCK_PGRP_LIST_SHARED(g, "dpvpop_pgrp_nice");
		if (low == PRIO_MAX + 1)
			return(ESRCH);
		*nice = low;
		return(ESUCCESS);

	case VPROC_SET:	
		/* visit all group members setting nice */
		VPROC_LOCK_PGRP_LIST_SHARED(g, "dpvpop_pgrp_nice");
		for (v = PVP(g)->pvp_head_pgrpl; 
				v != NULL; 
				v = PVP(v)->pvp_pgrpl) {
			error = PVPOP_PROC_NICE(v, nice,
					     euid, ruid, flag);
			found++;
		}
		VPROC_UNLOCK_PGRP_LIST_SHARED(g, "dpvpop_pgrp_nice");
		if (found == 0)
			return(ESRCH);
		return(error);
	default:
		return(EINVAL);
	}
}


/*
 * NAME:	dpvpop_get_attr
 *
 * FUNCTION:	Get relationship attributes for process.
 *		An attribute is returned only for non-NULL pointers supplied.
 *
 * RETURNS:	ESUCCESS on completion.
 */
int
dpvpop_get_attr(
	struct vproc *v,	/* vproc of calling process */
	pid_t *ppid,		/* returned id of parent */
	pid_t *pgid,		/* returned process group id*/
	pid_t *sid,		/* returned sesion id */
        uid_t *uid,             /* returned user id */
        uid_t *ruid,            /* returned real uid */
	boolean_t *job_control,	/* returns if process subject to job control */
	boolean_t *has_sctty,	/* returns if session tty control */
	boolean_t lock_flag)	/* fetch attributes under lock control */
{
	struct pvproc *pvp;

	pvp = PVP(v);

	if (lock_flag)
		VPROC_LOCK_EXCL(v, "dpvpop_get_attr");

	if (ppid)
		*ppid = pvp->pvp_ppid;

	if (pgid)
		*pgid = pvp->pvp_pgid;

	if (sid)
		*sid = pvp->pvp_sid;

        if (uid) {
		if (pvp->pvp_pproc == NULL)
			*uid = NOUID;
		else
        	       	*uid = pvp->pvp_pproc->p_rcred->cr_uid;
	}

        if (ruid) {
		if (pvp->pvp_pproc == NULL)
			*ruid = NOUID;
		 else
                	*ruid = pvp->pvp_pproc->p_ruid;
	}

	if (job_control) {
		if (pvp->pvp_flag&PV_PGRPLEADER)
			*job_control = pvp->pvp_jobc ? VPROC_JOB_CONTROL
						     : VPROC_ORPHANED;
		else
			*job_control = VPROC_NOT_PGRPLEADER;
	}

	if (has_sctty)
		*has_sctty = pvp->pvp_flag&PV_SCTTY;

	if (lock_flag)
		VPROC_UNLOCK_EXCL(v, "dpvpop_get_attr");

	return (ESUCCESS);

}


/*
 * NAME:	dpvpop_set_attr
 *
 * FUNCTION:	Set relationship attributes (in particular the parent
 *		process's process group and session id)
 *
 * RETURNS:	ESUCCESS on completion.
 */
int
dpvpop_set_attr(
	struct vproc *v,	/* vproc of calling process */
	pid_t pp_pgid,		/* process group id of parent process */
	pid_t pp_sid)		/* session id of parent process */
{
	register struct pvproc *pvp = PVP(v);

	VPROC_LOCK_PGRP_LIST_EXCL(v, "dpvpop_set_attr");

	/*
	 * adjust own pgrp's jobc if we were contributing to this pgrp being
	 * unorphaned before and now we are not, or if we were not contributing
	 * to the pgrp being unorphaned before and now we are.
	 */
	if (pvp->pvp_pgid != 0){
		boolean_t is_contributor_before =
				pvp->pvp_pp_sid == pvp->pvp_sid 
					&& pvp->pvp_pp_pgid != pvp->pvp_pgid;
		boolean_t is_contributor_after =
				pp_sid == pvp->pvp_sid
					&& pp_pgid != pvp->pvp_pgid;
		int adjustment = 0;
		if (is_contributor_before && !is_contributor_after)
			adjustment = PV_SUBTRACT;
		else if (!is_contributor_before && is_contributor_after)
			adjustment = PV_ADD;
		if (adjustment != 0) {
			/*
			 * Check if we're adjusting ourself.
			 * If so, do it and cut out the remote op
			 * which also avoids would be deadlock attempting
			 * to get the pgrp lock for a second time.
			 */
			if (pvp->pvp_pgid == v->vp_pid) {
				if (adjustment == PV_ADD)
					pvp->pvp_jobc++;
				else
					pvp->pvp_jobc--;
			} else
				(void) PVPOP_ADJUST_JOB_CONTROL_COUNT(
							VPROCPTR(pvp->pvp_pgid),
							adjustment);
		}
	}

	/*
	 * Change the pvproc fields for the parent process pgrp and sid.
	 */
	pvp->pvp_pp_sid = pp_sid;
	pvp->pvp_pp_pgid = pp_pgid;

	VPROC_UNLOCK_PGRP_LIST_EXCL(v, "dpvpop_set_attr");

	return(ESUCCESS);
}


/*
 * NAME:	dpvpop_sigproc
 *
 * FUNCTION:	Delivers signal to local process.
 *		The posting behaviour may be modified by the following flags:
 *		  VPROC_HAS_PRIV	caller is superuser (or the kernel)
 *		  VPROC_CANT_BLOCK	caller cannot block
 *		  VPROC_CHILD_STOP	sender is a stopped child
 *		  VPROC_IFTRACED	deliver only if process is traced
 *		  VPROC_IFSTOPPED	deliver only if process is stopped
 *		  VPROC_CHECK_CTTY	deliver only if process has a
 *					controlling terminal.
 *		  VPROC_PGRP_SIG	sig is going to whole process group
 *
 * RETURNS:	EPERM, if permissions disallow signal to be sent.
 *		ESRCH, if process not stopped when required.
 *		ESUCCESS on completion.
 */
int
dpvpop_sigproc(
	struct vproc *vto,	/* vproc to be signaled */
	int signo,		/* signal to be sent */
	int signo2,		/* second signal to be sent */
	int arg,		/* argument for SIGMIGRATE */
	uid_t euid,		/* sender's effective uid */
	uid_t ruid,		/* sender's real uid */
	pid_t pid,		/* sender's pid */
	pid_t sid,		/* sender's session id */
	int flags)		/* modifies posting behavior */
{
	struct pvproc *pvp = PVP(vto);
	struct proc *p;
	int error;

	/*
	 * If delivery is conditional on stopped state, return error if
	 * process is *not* stopped, which includes cases where it's a zombie.
	 */
	if ( (flags&VPROC_IFSTOPPED) &&
			((pvp->pvp_flag&PV_SSTOP) == 0 ||
			 (pvp->pvp_flag&PV_SZOMB) != 0 )) {
		return(ESRCH);
	}

	/*
	 * Return success for signalling all other zombies.
	 */
	if (pvp->pvp_flag & (PV_SZOMB|PV_EXITING))
		return(ESRCH);

#ifdef NX
        /*
         * Don't send a SIGCONT to a gang stopped application unless
         * the sender has root privileges
         */

        if(( signo ==  SIGCONT ) && ( pvp->pvp_flag & PV_NX_GANG_STOP ) &&
                        ( euid != 0 )) {

                        return (EPERM);
        }

#endif /* NX */

	/*
	 * If delivery is conditional on there being a controlling terminal
	 * but there isn't, simply return.
	 */
	if (flags&VPROC_CHECK_CTTY && !(pvp->pvp_flag&PV_SCTTY))
		return(ESUCCESS);

	/*
	 * Make sure the proc is still alive, and cannot completely die
	 * (be reaped) while we're signalling it.
	 */
	p = pvp->pvp_pproc;
	error = pproc_hold(p, vto->vp_pid);
	if (error != ESUCCESS)
		return(ESRCH);

	/* Call the base physical code. */
	error = pproc_psignal(p,
			      signo,
			      arg,
			      !(flags&VPROC_CANT_BLOCK),
			      flags&VPROC_CHILD_STOP,
			      flags&VPROC_IFTRACED,
			      (flags & VPROC_HAS_PRIV) != 0,
			      euid,
			      ruid,
			      sid);
	if (error)
		goto out;

	/*
	 * If this sig is going to all the procs in the process group,
	 * save it.  If the proc is doing a fork(), rfork() or rforkmulti()
	 * we will need to see to it that the new child procs get the signal
	 * too (since all new child procs are in the same proc group as the
	 * parent).
	 */
	if ((flags&VPROC_PGRP_SIG) != 0) {
		VPROC_LOCK_PGRP_MEM_SEQNO(vto, "dpvproc_sigproc lock mem");
		if (error == 0 ) {
			SIGADDSET(pvp->pvp_fork_sigset, signo);
		}
		++pvp->pvp_pgrp_mem_seqno;	/* to match pgrp ldr's seqno */
		VPROC_UNLOCK_PGRP_MEM_SEQNO(vto, "dpvproc_sigproc unlock mem");
	}

	/*
	 * Call the physical code again for a piggy-backed second signal
	 * if this option is being exploited.
	 */
	if (signo2 != 0 && error == ESUCCESS)
		(void) pproc_psignal(p,
				     signo2,
				     arg,
				     !(flags&VPROC_CANT_BLOCK),
				     flags&VPROC_CHILD_STOP,
				     flags&VPROC_IFTRACED,
				     (flags & VPROC_HAS_PRIV) != 0,
				     euid,
				     ruid,
				     sid);

out:
	pproc_release(p, vto->vp_pid);
	return(error);
}


/*
 * NAME:	dpvpop_sigpgrp
 *
 * FUNCTION:	Sends signal to process group.
 *		The posting behaviour may be modified by the following flags:
 *		  VPROC_HAS_PRIV	caller is superuser (or the kernel)
 *		  VPROC_CANT_BLOCK	caller cannot block
 *		  VPROC_CHILD_STOP	sender is a stopped child
 *		  VPROC_NOSIGSELF	don't signal the sender
 *
 * RETURNS:	EPERM, if permissions disallow signal to be sent.
 *		ESRCH, if specified vproc is not a process group leader.
 *		ENOTTY, if terminal access signal to an orphaned group,
 *			or from a process ignoring such signal.
 *		ESUCCESS on completion.
 */
int
dpvpop_sigpgrp(
	struct vproc *g,	/* pgrp leader vproc to be signaled */
	int signo,		/* signal to be sent */
	int arg,		/* argument for SIGMIGRATE */
	uid_t euid,		/* sender's effective uid */
	uid_t ruid,		/* sender's real uid */
	pid_t pid,		/* sender's pid */
	pid_t sid,		/* sender's session id */
	int *nproc,		/* number of processes signaled */
	int flags)		/* modifies posting behavior */
{
	register struct vproc *w;
	boolean_t is_masked, is_ignored;
	register int error = ESRCH;
	register int nsuccesses = 0;
	int pid_array_size = 0;

	if (!(PVP(g)->pvp_flag&PV_PGRPLEADER))
		return(ESRCH);

	/*
	 * Check for terminal access signals which shouldn't be delivered.
	 */
	if (signo == SIGTTIN || signo == SIGTTOU || signo == SIGTSTP)
		if (PVP(g)->pvp_jobc == 0)
			return(ENOTTY);

	flags |= VPROC_PGRP_SIG;	/* say sig goes to whole proc group */
	VPROC_LOCK_PGRP_LDR_SEQNO(g, "dpvpop_sigpgrp");
	++PVP(g)->pvp_pgrp_ldr_seqno;	/* bump ldr's seqno before members' */
	VPROC_UNLOCK_PGRP_LDR_SEQNO(g, "dpvpop_sigpgrp");

	VPROC_LOCK_PGRP_LIST_SHARED(g, "dpvpop_sigpgrp");

	/*
	 * Go thru the pgrp list and calculate number of vprocs to be
	 * signalled.
	 */
	for (w = PVP(g)->pvp_head_pgrpl; 
			w != NULL; 
			w = PVP(w)->pvp_pgrpl) {
		pid_array_size++;
	}

	/*
	 * Check if we are to use spanning trees to do the signalling, or
	 * if just going through the list is OK.
	 */
	if (pid_array_size > SPANNING_TREE_BENEFITS) {
		pid_t local_pid_array[VP_STACK_ARRAY_SIZE];
		pid_t *pid_array;
		int pid_array_count = 0;

		/*
		 * If we can use a stack array for the pid list, do so. 
		 * Otherwise, allocate one.
		 */
		if (pid_array_size > VP_STACK_ARRAY_SIZE) {
			pid_array=(pid_t *)kalloc(pid_array_size*sizeof(pid_t));
			if (pid_array == 0)
				goto slow_way;
		} else {
			pid_array = local_pid_array;
		}

		/*
		 * Fill in the array (skip myself if necessary)
		 */
		for (w = PVP(g)->pvp_head_pgrpl;
				w != NULL; 
				w = PVP(w)->pvp_pgrpl) {
                	if ((flags & VPROC_NOSIGSELF) && (w->vp_pid == pid))
                        	continue;
			pid_array[pid_array_count] = w->vp_pid;
			pid_array_count++;
		}
		
		/*
		 * Signal all the processes in the array.
		 */
		error = PVPOP_SIGPROC_MULTI(VPROCPTR(pid_array[0]),
					    signo,
					    0,
					    arg,
					    euid,
					    ruid,
					    pid,
					    sid,
					    flags,
					    0,
					    pid_array, 
					    pid_array_count,
					    NULL);

		/*
		 * Free any memory we allocated.
		 */
		if (pid_array != local_pid_array)
			kfree(pid_array, pid_array_size * sizeof(pid_t));

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if (error == EAGAIN)
			goto slow_way;

        	VPROC_UNLOCK_PGRP_LIST_SHARED(g, "dpvpop_sigpgrp");

		return(error);
	}

	/*
	 * Go through the list signalling each member.
	 */
slow_way:
        for (w = PVP(g)->pvp_head_pgrpl;
                        w != NULL;
                        w = PVP(w)->pvp_pgrpl) {
                if ((flags & VPROC_NOSIGSELF) && (w->vp_pid == pid))
                        continue;
                error = PVPOP_SIGPROC(w,signo,0,arg,euid,ruid,pid,sid,flags);
                if (error == ESUCCESS)
                        nsuccesses++;
        }

        VPROC_UNLOCK_PGRP_LIST_SHARED(g, "dpvpop_sigpgrp");

	if (nproc)
		*nproc = nsuccesses;
	return(nsuccesses ? ESUCCESS : error);
}


/*
 * NAME:	dpvpop_sigproc_multi
 *
 * FUNCTION:	Sends signal to a group of processes, using a spanning tree and
 *		asynchronous techniques..
 *
 *		To call this routine, the "h" parameter should be "NULL" and
 *		the pid_array_index parameter should be "0". These values
 *		are non-zero only for recursive calls to the routine.
 *
 *		The posting behaviour may be modified by the following flags:
 *		  VPROC_HAS_PRIV	caller is superuser (or the kernel)
 *		  VPROC_CANT_BLOCK	caller cannot block
 *		  VPROC_CHILD_STOP	sender is a stopped child
 *		  VPROC_NOSIGSELF	don't signal the sender
 *
 * RETURNS:	EPERM, if permissions disallow signal to be sent.
 *		ENOTTY, if terminal access signal to an orphaned group,
 *			or from a process ignoring such signal.
 *		ESUCCESS on completion.
 */
int
dpvpop_sigproc_multi(
	struct vproc *v,	/* (local) vproc to be signaled */
	int signo,		/* signal to be sent */
	int signo2,		/* second (optional) signal to be sent */
	int arg,		/* argument (for SIGMIGATE) */
	uid_t euid,		/* sender's effective uid */
	uid_t ruid,		/* sender's real uid */
	pid_t pid,		/* sender's pid */
	pid_t sid,		/* sender's session id */
	int flags,		/* modifies posting behavior */
	int pid_array_index,	/* index of v in pid_array */
	pid_t pid_array[],	/* full array of pids */
	int pid_array_count,	/* number of entries in pid_array */
	msg_handle_t *h)	/* message handle (unused for dpvp) */
{
	struct vproc	*w;
	int		error;
	int		nsuccesses = 0;
	register int	i;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];

	ASSERT(v->vp_pid == pid_array[pid_array_index]);

	/*
	 * Find the entries in the pid array that are below this entry
	 * in the spanning tree.
	 */
	error =  get_spanning_tree(pid_array_count, pid_array_index,
				   entry_array, &entry_array_count);
	if (error)
		return(EAGAIN);

	/*
	 * Perform the asynchronous "send" of the message to all the
	 * processes that are below this entry in the spanning tree.
	 */
	for (i=0; i<entry_array_count; i++) {
		w = LOCATE_VPROC_PID(pid_array[entry_array[i]]);
		ASSERT(w);
		error_array[i] = PVPOP_SIGPROC_MULTI_SEND(w,
					                  signo,
							  0,
					                  arg,
					                  euid,
					                  ruid,
					                  pid,
					                  sid,
					                  flags,
					                  entry_array[i],
					                  pid_array, 
					                  pid_array_count,
					                  &handle_array[i]);
	}

	/*
	 * Signal the local process.
	 */
        error = PVPOP_SIGPROC(v,signo,signo2,arg,euid,ruid,pid,sid,flags);
	if (error == 0)
		nsuccesses++;

	/*
	 * Perform the asynchronous "receive" of replies to the message for 
	 * all the processes that are below this entry in the spanning tree.
	 */
	for (i=0; i<entry_array_count; i++) {
		w = VPROCPTR(pid_array[entry_array[i]]);
		ASSERT(w);
		if (error_array[i] == 0) {
			error = PVPOP_SIGPROC_MULTI_RECEIVE(w,
						            signo,
							    0,
						            arg,
						            euid,
						            ruid,
						            pid,
						            sid,
						            flags,
					                    entry_array[i],
					                    pid_array, 
					                    pid_array_count,
					                    &handle_array[i]);
			if (error == 0)
				nsuccesses++;
		} else
			error = error_array[i];
		VPROC_RELEASE(w, "dpvpop_sigproc_multi");
	}

	return(nsuccesses ? ESUCCESS : error);
}


/*
 * NAME:	dpvpop_report_state
 *
 * FUNCTION:	Alter the state of a proc on the node of its parent.
 *
 * RETURNS:	ESUCCESS on successful completion, otherwise result
 *		of failing signal operation to parent.
 */
int
dpvpop_report_state(
	struct vproc *p,	/* vproc of parent process */
	struct vproc *v,	/* vproc of process */
	int state)		/* zombie or stop or unstop */
{
	int error = ESUCCESS;
#ifdef NX
        int no_signal;
#endif /* NX */

#ifdef NX
        /* Check if we should send a SIGCHLD to parent */
        no_signal = PVP(v)->pvp_flag & PV_NX_NO_SIGCHLD;
#endif /* NX */

	/* action depends on new state */
	switch (state) {
	case VPROC_ZOMBIE:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag |= PV_SZOMB;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		/* take the opportunity here to signal the parent */
#ifdef NX
                if (no_signal != PV_NX_NO_SIGCHLD) {
#endif /* NX */
		error = PVPOP_SIGPROC(p, SIGCHLD, 0, 0, 0, 0, v->vp_pid, 0,
					VPROC_HAS_PRIV);
		(void) pproc_wakeup(PVP(p)->pvp_pproc); 
#ifdef NX
		}
#endif /* NX */
		break;
	case VPROC_STOP:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag |= PV_SSTOP;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		/* take the opportunity here to signal the parent */
#ifdef NX
		if (no_signal != PV_NX_NO_SIGCHLD)
		    if ((PVP(v)->pvp_flag & PV_NX_GANG_STOP) 
			!= PV_NX_GANG_STOP) {
#endif /* NX */
		error = PVPOP_SIGPROC(p, SIGCHLD, 0, 0, 0, 0, v->vp_pid, 0,
					VPROC_HAS_PRIV | VPROC_CHILD_STOP);
		(void) pproc_wakeup(PVP(p)->pvp_pproc);
#ifdef NX
		}
#endif /* NX */
		break;
	case VPROC_UNSTOP:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag &= ~PV_SSTOP;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		break;
	case VPROC_LEFT_PGRP:
		/* wake up parent in case parent sleeping on old pgid */
		(void) pproc_wakeup(PVP(p)->pvp_pproc);
		break;
#ifdef NX
	case VPROC_NO_SIGCHLD:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag |= PV_NX_NO_SIGCHLD;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		break;
	case VPROC_AGAIN_SIGCHLD:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag &= ~PV_NX_NO_SIGCHLD;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		break;
	case VPROC_GANG_STOP:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag |= PV_NX_GANG_STOP;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		break;
	case VPROC_GANG_UNSTOP:
		VPROC_LOCK_FLAG(v, "dpvpop_report_state");
		PVP(v)->pvp_flag &= ~PV_NX_GANG_STOP;
		VPROC_UNLOCK_FLAG(v, "dpvpop_report_state");
		break;
#endif /* NX */
	}

	return(error);

}

/*
 * NAME:	dpvpop_report_state_elder
 *
 * FUNCTION:	State change of child processs reported to elder (child).
 *
 * RETURNS:	ESUCCESS on successful completion, otherwise result
 *		of failing operation to parent.
 */
int
dpvpop_report_state_elder(
	struct vproc *ve,	/* vproc of elder process */
	struct vproc *v,	/* vproc of process */
	int state)		/* zombie or stop or unstop */
{
	struct pvproc	*pvp = PVP(ve);
	int		error = ESUCCESS;

	/*
	 * If there's already an outstanding report we don't need to do
	 * anything further, otherwise set this flag and continue.
	 */
	VPROC_LOCK_FLAG(ve, "dpvpop_report_state_elder");
	if (pvp->pvp_flag & PV_HAS_REPORTED) {
		VPROC_UNLOCK_FLAG(ve, "dpvpop_report_state_elder");
		return(ESUCCESS);
	}
	pvp->pvp_flag |= PV_HAS_REPORTED;
	VPROC_UNLOCK_FLAG(ve, "dpvpop_report_state_elder");

	/*
	 * If the elder is not the eldest child and has an elder itself then
	 * repeat the report to it. Otherwise, a regular report directly
	 * to the parent is required.
	 */
	if (pvp->pvp_epid != 0 && pvp->pvp_epid != ve->vp_pid)
		error = PVPOP_REPORT_STATE_ELDER(VPROCPTR(pvp->pvp_epid),
						 v, state);
	else
		error = PVPOP_REPORT_STATE(VPROCPTR(pvp->pvp_ppid),
					   v, state);

	return(error);

}

/*
 * NAME:	dpvpop_ctty_getattr
 *
 * FUNCTION:	Get attributes of the controlling terminal a process.
 *
 * RETURNS:	ENOENT, if the controlling terminal is remote from the
 *			session leader and node returns where this is.
 *		ENOTTY, if no controlling terminal exists.
 *		ESUCCESS on successful completion with attributes returned
 *			for the non-NULL pointers given. 
 */
int
dpvpop_ctty_getattr(
	struct vproc *s,	/* vproc of session leader */
	pid_t *t_pgid,		/* returned foreground pgrp */
	dev_t *dev,		/* returned controlling device */
	node_t *node)		/* returned controlling terminal node */
{
	node_t	cttynode = PVP(s)->pvp_cttynode;

	if (cttynode == -1)
		return(ENOTTY);
	else {
		if (node)
			*node = cttynode;
		if (t_pgid == NULL && dev == NULL)
			return(ESUCCESS);

		/*
		 * We're on the execution node of the session leader but this
		 * may not be the location of the controlling terminal.
		 * Forward the operation to the correct node.
		 */
		return(PVPSOP_CTTY_GETATTR(cttynode, s, t_pgid, dev));
	}
}


/*
 * NAME:	dpvpop_set_ctty
 *
 * FUNCTION:	Set (a pointer to) the tty struct of the controlling terminal
 *		for the caller's vproc.
 *
 * RETURNS:	EPERM, if setting ctty for process other than session leader.
 *		EPERM, if setting no ctty for a session leader.
 *		ESUCCESS on successful completion.
 */
int
dpvpop_set_ctty(
	struct vproc *v,	/* vproc of calling process */
	int node,		/* controlling tty node (or -1) */
	int flags)		/* set or clear */
{
	boolean_t	true = TRUE;
	boolean_t	false = FALSE;
	struct pvproc	*pvp = PVP(v);
	int		error = ESUCCESS;

	VPROC_LOCK_EXCL(v, "dpvpop_set_ctty");

	if (flags&VPROC_SET) {
		if (pvp->pvp_flag&PV_SESSIONLEADER) {
			/* Set is permitted only for session leader */
			VPROC_LOCK_FLAG(v, "dpvpop_set_ctty");
			pvp->pvp_flag |= PV_SCTTY;
			VPROC_UNLOCK_FLAG(v, "dpvpop_set_ctty");
			pvp->pvp_cttynode = node;
			pproc_set_attr(pvp->pvp_pproc,0,0,0,0,0,&true);
		} else
			error = EPERM;

	}
	else if (flags&VPROC_CLEAR) {
		/* Clear is not permitted for session leader */
		if (pvp->pvp_flag&PV_SESSIONLEADER)
			error = EPERM;
		else {
			VPROC_LOCK_FLAG(v, "dpvpop_set_ctty");
			pvp->pvp_flag &= ~PV_SCTTY;
			VPROC_UNLOCK_FLAG(v, "dpvpop_set_ctty");
			pproc_set_attr(pvp->pvp_pproc,0,0,0,0,0,&false);
		}
	}

	VPROC_UNLOCK_EXCL(v, "dpvpop_set_ctty");

	return(error);
}


/*
 * NAME: dpvpop_ptrace
 *
 * FUNCTION: Process tracing functions for a child process.
 *
 * RETURNS:	ESRCH, if no child exists.
 *		EINVAL, if the requested operation code is invalid.
 *		ESUCCESS on successful completion.
 */
dpvpop_ptrace(
	struct vproc *v,	/* vproc of traced process */
	int req,		/* ptrace request */
	vm_address_t addr,	/* address within process */
	int data,		/* data to be written */
	int *retval)		/* returned data if read from address */
{
	struct pvproc	*pvp;
	pvp = PVP(v);
	/* First make sure the process is not exiting or is already
	 * a ZOMBIE 
	 */

        if ((pvp->pvp_flag & PV_EXITING) || (pvp->pvp_flag & PV_SZOMB)) {
                return(ESRCH);
        }

	/*
	 * We translate the request code from "VPROC space"
	 * to "PROC space" - even though this is an identity operation.
	 */
	switch (req) {
	case VPROC_PT_TRACE_ME:	/* enable tracing for caller */
		req = PT_TRACE_ME; break;
	case VPROC_PT_READ_I:	/* read word in child's I space */
		req = PT_READ_I; break;
	case VPROC_PT_READ_D:	/* read word in child's D space */
		req = PT_READ_D; break;
	case VPROC_PT_READ_U:	/* read word in child's user structure */
		req = PT_READ_U; break;
	case VPROC_PT_WRITE_I:	/* write word in child's I space */
		req = PT_WRITE_I; break;
	case VPROC_PT_WRITE_D:	/* write word in child's D space */
		req = PT_WRITE_D; break;
	case VPROC_PT_WRITE_U:	/* write word in child's user structure */
		req = PT_WRITE_U; break;
	case VPROC_PT_CONTINUE:	/* continue the child */
		req = PT_CONTINUE; break;
	case VPROC_PT_KILL:	/* kill the child process */
		req = PT_KILL; break;
	case VPROC_PT_STEP:	/* single step the child */
		req = PT_STEP; break;
	default:
		return(EINVAL);
	}

	/*
	 * Call the physical level to perform the trace request.
	 */
	/* First make sure the proc structure is still there */
	if(pvp->pvp_pproc == NULL) {
		return(ESRCH);
	}
	return(pproc_ptrace(PVP(v)->pvp_pproc, req, (int*) addr, data, retval));
}


/*
 * NAME:	dpvpop_get_task_port
 *
 * FUNCTION:	Get Mach task port corresponding to physical process
 *
 * RETURNS:	ESRCH, if vproc is not active
 *		EACCES, if calling process does not have permission
 *		ESUCCESS, if successful
 */
int
dpvpop_get_task_port(
	struct vproc *v,	/* vproc of target process */
	uid_t euid,		/* effective uid of caller */
	int flag,		/* privileged flag */
	mach_port_t *task_port)	/* task port for physical process */
{
	struct pvproc	*pvp = PVP(v);

	if (pvp->pvp_flag & PV_SZOMB) {
		*task_port = MACH_PORT_NULL;
		return(ESRCH);
	}

	return(pproc_get_task_port(PVP(v)->pvp_pproc, 
				   euid, 
				   flag&VPROC_HAS_PRIV, 
				   task_port));
}


/*
 * NAME:	dpvpop_terminal_sigpgrp
 *
 * FUNCTION:	Sends signal to process group of specified process.
 *
 * RETURNS: 	ENOTTY, if terminal access signal to an orphaned group,
 *			or from a process ignoring such signal.
 *		ESUCCESS on completion.
 */
int
dpvpop_terminal_sigpgrp(
	struct vproc *v,	/* process whose pgrp is to be signaled */
	int signo)		/* signal to be sent */
{
	boolean_t is_masked, is_ignored;

	/*
	 * We check the caller's masks before re-directing the operation
	 * to the caller's pgrp.
	 */
	pproc_chksigmask(PVP(v)->pvp_pproc, signo, &is_masked, &is_ignored);
	if (is_masked || is_ignored)
		return(ENOTTY);
	return(PVPOP_SIGPGRP(VPROCPTR(PVP(v)->pvp_pgid), 
		signo, 0, 0, 0, 0, 0, NULL, 
		VPROC_HAS_PRIV | VPROC_CHECK_CTTY));
}


/*
 * NAME:	dpvpop_table_get
 *
 * FUNCTION:	Performs table() system call operations on a process.
 *
 * RETURNS:	ESUCCESS on completion.
 */
/*
 * Local private virtual process system operation for PVPSOP_TABLE()
 */
int
dpvpop_table_get(
	struct vproc *v,	/* process being queried */
	int id,			/* type of table() query */
	char_array *buffer,	/* address of address of buffer */
	u_int *bufferlen,	/* length of buffer */
	u_int lel)		/* length of each buffer element */
{
	int error;
	int dummy;
	struct args {
		int	id;
		int	index;
		caddr_t	addr;
		int	nel;
		u_int	lel;
	} ua;

	ua.id = id;
	ua.index = v->vp_pid;
	ua.addr = *buffer;
	ua.nel = 1;
	ua.lel = lel;

	error = pproc_hold(PVP(v)->pvp_pproc, v->vp_pid);
	if (error != ESUCCESS)
		return(ESRCH);
	error = pps_table(&ua, &dummy);
	pproc_release(PVP(v)->pvp_pproc, v->vp_pid);
	return(error);
}


/*
 * NAME:	dpvpop_table_set
 *
 * FUNCTION:	Performs table() system call operations on a process.
 *
 * RETURNS:	ESUCCESS on completion.
 */
/*
 * Local private virtual process system operation for PVPSOP_TABLE()
 */
int
dpvpop_table_set(
	struct vproc *v,	/* process being queried */
	int id,			/* type of table() query */
	char_array buffer,	/* address of buffer */
	u_int bufferlen,	/* length of buffer */
	u_int lel)		/* length of each buffer element */
{
	int dummy;
	int error;
	struct args {
		int	id;
		int	index;
		caddr_t	addr;
		int	nel;
		u_int	lel;
	} ua;

	ua.id = id;
	ua.index = v->vp_pid;
	ua.addr = buffer;
	ua.nel = -1;
	ua.lel = lel;

	error = pproc_hold(PVP(v)->pvp_pproc, v->vp_pid);
	if (error != ESUCCESS)
		return(ESRCH);
	error = pps_table(&ua, &dummy);
	pproc_release(PVP(v)->pvp_pproc, v->vp_pid);
	return(error);
}


/*
 * NAME:	dpvpop_get_fork_signals
 *
 * FUNCTION:	Obtains pgrp-style signals "recently" sent to a process.
 *
 * RETURNS:	ESUCCESS on completion.
 */
int
dpvpop_get_fork_signals(
	struct vproc *v,	/* process whose recent pgrp sigs are sought */
	sigset_t *sigsetp,	/* OUT bitmask of recent pgrp signals */
	long *sigmigargp,	/* OUT extra arg used with SIGMIGRATE */
	int *seqnop)		/* OUT sequence number of info we return */
{
	struct pvproc *pvp = PVP(v);

	VPROC_LOCK_PGRP_MEM_SEQNO(v, "dpvpop_get_fork_signals");

	*sigsetp = pvp->pvp_fork_sigset;
	*sigmigargp = pvp->pvp_fork_sigmigarg;
	*seqnop = pvp->pvp_pgrp_mem_seqno;

	VPROC_UNLOCK_PGRP_MEM_SEQNO(v, "dpvpop_get_fork_signals");
	return(ESUCCESS);
}

#ifdef NX

/*
 * NAME: dpvpop_nx_get_info
 *
 * FUNCTION: Get partition node list for process
 *
 * RETURNS:
 *              ESUCCESS on successful completion.
 */
dpvpop_nx_get_info(
		   struct vproc *v,           /* target process */
                   APPLINFO_T   *applinfo,    /* Application info */
                   LP_MAP_T     *nodelist,    /* Partition node list */
                   int          *nodelistcnt, /* Size of partition list */
                   int           flag,        /* Permissions flag */
                   uid_t         euid,        /* Effective uid of caller */
                   int          *err)         /* error code */
{

        /*
         * Call the physical level to perform the get_info request.
         */
        return(nx_get_info(PVP(v), applinfo, nodelist, nodelistcnt,
                           flag, euid, err));
}

/*
 * NAME: dpvpop_nx_get_flag
 *
 * FUNCTION: Get pvp_flag for the process
 *
 * RETURNS: ESUCCESS on completion.
 *
 */

dpvpop_nx_get_flag(
		struct vproc *v,	/* target process */
		u_int *pvpflag)		/* pvp_flag of the process */
{

	struct pvproc	*pvp;

	
	pvp=PVP(v);
	*pvpflag = pvp->pvp_flag;
	return(ESUCCESS);
}

/*
 * NAME: dpvpop_nx_get_p_flag
 *
 * FUNCTION: Get p_flag for the process
 *
 * RETURNS: ESUCCESS on completion.
 *
 */

dpvpop_nx_get_p_flag(
                struct vproc *v,	/* target process */
                u_int *pflag)		/* pvp_flag of the process */
{

        struct pvproc   *pvp;


        pvp=PVP(v);
        *pflag = pvp->pvp_pproc->p_flag;
        return(ESUCCESS);
}

#endif /* NX */
