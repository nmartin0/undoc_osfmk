/*
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
 * $Log: kern_exit.c,v $
 * Revision 1.33  1995/02/17  18:38:46  toman
 * Updated CVS comments in previous revision to fix line-wrapping problem.
 *
 * Revision 1.32  1995/02/17  18:29:28  toman
 *  Reviewer: Bob Yasi, Suri Brahmaroutu
 *  Risk: Low
 *  Benefit or PTS #: 12436
 *  Testing: VSTNC
 *  Module(s): server/bsd/kern_exit.c
 *             server/tnc/rtask_cli_pproc.c
 * Make sure pprocs have zero thread reference count (p_ref_count, which is
 * initialized to 1 when the pproc is allocated and increases/decreases as
 * server threads are registered/deregistered to it) before placing them back
 * on the free list.  Do this by decrementing p_ref_count in
 * rtask_pproc_remove() and by adding an assertion (p_ref_count == 0) in
 * pproc_clear().
 *
 * Revision 1.31  1995/02/01  21:26:49  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.30  1995/01/27  22:42:57  yazz
 *  Reviewer: John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 12237
 *  Testing: EATs sched, os_interfaces, controlc
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/sys/proc.h
 * 	server/sys/vproc.h
 * 	server/tnc/rtask_cli_pproc.c
 * Change pproc ref count mechanism so it never has to block.  Instead of
 * blocking until the ref count drops to zero before clearing out a pproc
 * structure and placing it back on the freelist, set a flag so that this
 * action will take place as soon as the ref count does drop to zero.
 *
 * Revision 1.29  1994/11/18  20:26:58  mtm
 * Copyright additions/changes
 *
 * Revision 1.28  1994/11/08  20:06:52  yazz
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
 * Revision 1.27  1994/11/03  15:54:23  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Med
 *  Benefit or PTS #: #11459
 *  Testing: Failing corefile EAT
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * 	server/uxkern/syscall_subr.c
 *
 * When clearing and freeing a physical proc struct in pproc_reap(), zero
 * the vproc ptr field too.  Any attempts to use this ptr after the reap
 * will generate page 0 exceptions now, instead of blundering forward using
 * stale data.
 *
 * Revision 1.26  1994/10/25  22:51:37  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: High -- many lines changed in many files
 *  Benefit or PTS #: 9853
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/bsd/init_main.c
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_fork.c
 * 	server/bsd/kern_prot.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/sys/user.h
 * 	server/tnc/chkpnt_pproc.c
 * 	server/tnc/rvp_subr.c
 * 	server/tnc/tnc_svipc.c
 * 	server/uxkern/bsd_2.defs
 * 	server/uxkern/syscall_subr.c
 * Side-thread changes.  Renamed p_sigref to the more general p_exit_hold_count.
 * Take master lock in pproc_exit() so that this hold count is respected in both
 * the TNC and non-TNC cases.
 *
 * Revision 1.25  1994/09/20  00:34:42  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 10989
 *  Testing: Sched EAT
 *  Module(s): server/bsd/kern_exit.c
 * Made pproc_exit() not kalloc an rusage structure until after the early
 * bailout check, plugging a memory leak.
 *
 * Revision 1.24  1994/08/31  22:46:27  mtm
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
 * Revision 1.21.2.2  1994/08/30  16:08:19  nandy
 * In pproc_exit() check if the port is not dead before calling
 * task_to_proc_remove().
 *
 *  Reviewer: John Loverso
 *  Risk: L
 *  Benefit or PTS #: 10687
 *  Testing: Control-C Eats
 *  Module(s): ux_notify.c
 * 		kern_exit.c
 *
 * Revision 1.21.2.1  1994/08/03  15:50:46  nandy
 * Merged from the main stem
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.22  1994/08/03  15:27:42  nandy
 * Replace nx_application() with nx_tam_attached() to allow processes
 * that have not called nx_init() to get debugged.
 *
 *  Reviewer: John Litvin
 *  Risk: M
 *  Benefit or PTS #:10047
 *  Testing: PTS test case
 *  Module(s):kern_sig.c
 * 	tnc/dpvproc.h
 * 	tnc/dvp_vpops.c
 * 	nx/nx.c
 *
 * Revision 1.21  1994/07/27  16:25:20  johannes
 * In pproc_exit() the exec dir proxies are released.
 *
 *  Reviewer: Nandini
 *  Risk: H
 *  Benefit or PTS #: information for absolute exec path in core files
 *  Testing: developer
 *  Module(s): server/sys: user.h
 *             server/bsd: kern_exec.c, kern_exit.c, kern_fork.c
 *             server/tnc: pvps.ops, tnc.defs, rtask_server.c
 *                         rtask_cli_pproc.c, rtask_cli_vproc.c
 *                         rtask_svr_pproc.c, rtask_svr_vproc.c
 *                         chkpnt_vproc.c
 *             server/paracore: core.c
 *
 * Revision 1.20  1994/06/29  16:47:15  johannes
 * pproc_exit(): calling of para_core_dump()
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
 * Revision 1.19  1994/06/21  15:12:21  cfj
 * pproc_exit() now check if the process was profiled and if so, tells
 * the kernel to stop profiling and shuts down profiling orderly.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:9284
 *  Testing:Test case controlc EAT
 *  Module(s):server/bsd/kern_exit.c
 *
 * Revision 1.18  1994/01/13  17:53:59  jlitvin
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
 * Revision 1.17  1993/07/14  17:47:16  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.16  1993/07/06  21:48:21  nandy
 * Mark a process unreaped while exiting.
 *
 * Revision 1.1.1.7  1993/07/01  18:46:20  cfj
 * Adding new code from vendor
 *
 * Revision 1.15  1993/06/11  00:47:50  nandy
 * In pproc_reap() don't mark the process unreaped if the parent and the tam are the
 * same.
 *
 * Revision 1.14  1993/06/10  18:09:25  cfj
 * Put #ifdef TNC around new Locus code in pproc_wakeup(),
 * pproc_assert_sleep_wait() and pproc_sleep_waiting() to prevent a
 * dereferencing of a NULL pointer in a non-TNC build.
 *
 * Revision 1.13  1993/06/09  20:17:36  nandy
 * Initialize error in pproc_sleep_waiting().
 *
 * Revision 1.12  1993/06/08  21:27:13  nandy
 * Add a new p_flag to avoid wakeup being sent before pproc_sleep_waiting() is called.
 *
 * Revision 1.11  1993/05/07  19:08:20  nandy
 * Fixed a merge conflict
 *
 * Revision 1.10  1993/05/06  19:02:59  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.9  1993/04/03  03:03:50  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.1.5  1993/05/03  17:23:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.8  1993/03/08  18:32:00  nandy
 * Merged from T9
 *
 * Revision 1.7.4.1  1993/03/06  23:33:44  nandy
 * Changes from OSF for new locking mechanism.
 *
 * Revision 1.1.2.2.2.3  1993/02/16  20:02:37  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.7  1993/01/29  17:21:14  nandy
 * Removed report_allocator() from pproc_exit().
 *
 * Revision 1.6  1993/01/28  00:47:39  nandy
 * Fix for ctrl-Z on nx applications.
 *
 * Moved report_allocator() to pproc_exit()
 *
 * Revision 1.1.2.2.2.2  1993/01/09  00:03:23  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  05:58:21  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 * 
 * Revision 1.5  1992/12/15  00:09:36  cfj
 * 	Revision 2.22  92/12/11  15:27:53  chrisp
 * 	Remove redundant test for process already exiting in pproc_exit().
 *
 * Revision 1.4  1992/12/11  02:54:16  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:15:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  18:21:42  dleslie
 * Conflict resolution resulting from merge of November 3 bugdrop from Locus
 * into the NX tree
 *
 * Revision 1.1.2.1  1992/11/06  00:05:45  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.21  92/11/23  15:59:50  klh
 * 	Revision 2.20  92/11/17  19:47:12  loverso
 * 		Only dump core from pproc_exit().  (loverso)
 * 
 * Revision 2.20  92/10/14  08:16:13  chrisp
 * [Bug #68] In pproc_sleep_waiting() and pproc_clear_sleep_wait(), release the
 * 	master lock only if it's held.
 * 
 * Revision 2.27  93/06/22  17:54:22  yazz
 * [ Bug #0251 ] Gate the wait() system call with a new proc struct flag
 * 	bit, SWAKEUPDONE.  This abolishes routine pproc_clear_sleep_-
 * 	wait() altogether, and modifies pproc_wakeup(), pproc_assert_-
 * 	sleep_wait() and pproc_sleep_waiting().
 * 	Also assert that the proc structure pointer passed to these
 * 	routines is not NULL.
 * 
 * Revision 2.26  93/06/02  09:49:13  yazz
 * For Sys V IPC under TNC perform semaphore adjustment at exit time
 * differently.
 * 
 * 	Revision 2.22  93/03/22  23:58:00  condict
 * 		Eliminate dropping and retaking of master lock around call to
 * 		credentials_deallocate.  It now calls ux_server_thread_blocking,
 * 		as it should, so this was now redundant and deadly.
 *
 * 	Revision 2.21  93/01/07  11:16:27  condict
 * 		Updated to new MK profiling interface.
 * 		[1992/09/24  19:45:57  emcmanus]
 *
 * Revision 2.24  93/04/22  16:33:32  roman
 * [Bug 222] Waiting for pgid has problems.
 * 
 * Revision 2.23  93/03/22  21:07:25  yazz
 * OSF lock changes.  Master lock no longer dropped here when calling creds
 * service.
 * 
 * Revision 2.22  92/12/11  15:27:53  chrisp
 * Remove redundant test for process already exiting in pproc_exit().
 * 
 * Revision 2.21  92/11/23  15:59:50  klh
 * 	Revision 2.20  92/11/17  19:47:12  loverso
 * 		Only dump core from pproc_exit().  (loverso)
 * 
 * Revision 2.20  92/10/14  08:16:13  chrisp
 * [Bug #68] In pproc_sleep_waiting() and pproc_clear_sleep_wait(), release the
 * 	master lock only if it's held.
 * 
 * Revision 2.19  92/10/06  12:08:34  roman
 * Fix RCS comments.
 * 
 * Revision 2.18  92/10/05  13:58:01  klh
 * 	Revision 2.18  92/09/24  16:49:42  rabii
 * 		Avoid acct()ing to each process all children's times;
 * 		remove extraneous bzero.  (dwm #377)
 * 
 * 	Revision 2.17  92/08/26  12:09:59  loverso
 * 		Deallocate the callback thread port on process exit.
 * 		Replace task_suspend with unix_task_suspend.
 * 		(loverso)
 * 
 * Revision 2.17  92/09/30  10:38:29  chrisp
 * Add routines pproc_assert_sleep_wait() and pproc_clear_sleep_wait().
 * 	pproc_assert_sleep_wait() is called in a process' wait()
 * 	loop while scanning for stopped or zombied child processes
 * 	to ensure that no signal (particularly SIGCHLD) is delivered
 * 	until pproc_sleeping_waiting() or pproc_clear_sleep_wait()
 * 	is called (marking the end of the scan).
 * 
 * Revision 2.16  92/09/28  16:27:10  roman
 * Allow pproc_add_rusage() to work for a process other than the current
 * process.
 * 
 * Revision 2.15  92/07/07  13:05:18  klh
 * 	Revision 2.16  92/06/30  22:45:35  loverso
 * 		Add master locking within pproc_exit(), pproc_reap() and
 * 		pproc_add_rusage() via new TNC_unix_{master,release} macros 
 *		defined in parallel.h. (chrisp/rabii)
 * 
 * Revision 2.14  92/06/05  14:01:30  klh
 * 	Revision 2.15  92/05/27  20:04:52  pjg
 * 		Use master_lock and master_unlock instead of manipulating the 
 *		lock directly to allow redefinition of the master lock.
 * 
 * 	Revision 2.14  92/05/24  14:13:32  pjg
 * 		Change to be closer to Grenoble code:  vm_mmap_exit -> vm_exit.
 * 		[92/05/20            roy]
 * 
 * 	Revision 2.13  92/05/18  12:34:05  roy
 * 		Call vm_mmap_exit from pproc_exit.
 * 		[92/05/18            roy]
 * 
 * Revision 2.13  92/04/06  15:45:37  chrisp
 * Add master locking within pproc_exit(), pproc_reap() and pproc_add_rusage().
 * 
 * Revision 2.16  92/06/30  22:45:35  loverso
 * 	Add master locking within pproc_exit(), pproc_reap() and
 * 	pproc_add_rusage() via new TNC_unix_{master,release} macros defined in
 * 	parallel.h. (chrisp/rabii)
 * 
 * Revision 2.15  92/05/27  20:04:52  pjg
 * 	Use master_lock and master_unlock instead of manipulating the lock
 * 	directly to allow redefinition of the master lock.
 * 
 * Revision 2.14  92/05/24  14:13:32  pjg
 * 	Change to be closer to Grenoble code:  vm_mmap_exit -> vm_exit.
 * 	[92/05/20            roy]
 * 
 * Revision 2.13  92/05/18  12:34:05  roy
 * 	Call vm_mmap_exit from pproc_exit.
 * 	[92/05/18            roy]
 * 
 * Revision 2.12  92/03/20  11:40:01  pjg
 * 	Drop master_mutex around credentials_deallocate; do this as late as 
 * 	possible in pproc_exit() (loverso).
 * 
 * Revision 2.11  92/03/16  19:02:25  pjg
 * 	Don't release unix_master around call to credentials_deallocate
 * 	in pproc_exit (temporary fix) (loverso).
 * 
 * Revision 2.10  92/03/15  14:40:02  roy
 * 	Replaced deallocate_credentials with new interface, and we
 * 	now drop unix_master lock before calling credentials_deallocate
 * 	since it may go to the fileserver and need to get it there in
 * 	which case a deadlock will occur if we don't release it (rabii)
 * 
 * Revision 2.9  92/03/09  14:03:54  durriya
 * 	Deallocate the callback port on process exit (loverso).
 * 
 * 	Revision 3.9  92/02/28  15:11:05  david
 * 	added locking in exit, possible race with server_thread_deregister
 * 	(bug 46)
 * 
 * Revision 2.8  92/03/01  18:42:05  pjg
 * 	Changed the references to the cwd and root vnodes to use vnode proxies.
 * 	Deallocate the port associated with the proc entry when the process
 * 	exits.
 * 
 * Revision 2.7  92/01/14  11:12:37  roy
 * 	92/01/11  18:45:32  ses
 * 	#ifdefed out vreles of rootdir and cwd ports for the OSF1_ADFS case
 * 	(now done in the emulator).
 * 
 * Revision 2.6  91/12/18  09:38:49  roy
 * 	91/12/13  13:02:35  sp
 * 	use OSF/1 restartable system call mechanism for rexit rather than
 * 	the BSD one which doesn't work anyway.
 * 
 * 	91/11/26  15:32:24  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/11/13  14:55:37  barbou
 * 	Fix for bug #39: fixed computation of the number of stopped processes in
 * 	a process group.
 * 
 * 	91/10/31  12:27:18  david
 * 	added semexit() to 3.0 for sv semaphores
 * 
 * 	91/10/29  16:11:07  barbou
 * 	Fix for bug #17: synchronize exit() and server_thread_register().
 * 
 * Revision 2.5  91/11/22  14:54:17  rabii
 * 	Locus Merge
 * 	Return params iszombie and isstop removed from pproc_reap.
 * 	Session leader exit now handled in VPROC layer; code removed. (chrisp)
 * 	Change interface to pproc_reap() to have explicit codes for stopped
 * 	state and zombie state. Added new pproc operation pproc_add_rusage().
 * 	Proper pvpop's added. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:44:14  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:32:42  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	File descriptors now handled in emulator. Remove closing of files
 * 	from here. Add call to deallocate credentials port. Move exit()
 * 	and wait() system calls and code dealing with exiting from sessions
 * 	and pgrps to vproc and pvproc directories.
 * 
 * Revision 2.2  91/08/31  13:21:36  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/07  17:16:05  jose
 * Adapted from 1.0.2 to OSF/1s environment
 * 
 * Revision 1.19.8.3  91/08/01  10:22:13  hermi
 * 	Fix for bug #2557 i.e. process accounting times are bogus.
 * 	Fixed by writing accounting record (acct()) after accumalating
 * 	times for the task and its threads. Also, these times were
 * 	being collected in the proc structure and as acct() used values
 * 	from the uarea - a copyback of resource usage from proc structure
 * 	to uarea was needed.
 * 	[91/08/01  10:07:16  hermi]
 * 
 * Revision 1.19.8.2  91/07/26  14:27:36  dlb
 * 	Remove call to task_halt() from exit().  This logic has moved
 * 	into the signal lock (sig_lock_to_exit()).
 * 	[91/07/26  14:18:43  dlb]
 * 
 * Revision 1.19.3.2  91/03/11  16:11:55  jph
 * 	Change status passed to acct() as in Sys5.
 * 	[91/03/11  16:05:06  jph]
 * 
 * Revision 1.19  90/10/31  13:48:53  devrcs
 * 	in exit(), added call to clx_exit() for Xenix clean up (386 only).
 * 	[90/10/08  13:20:44  swallace]
 * 
 * Revision 1.18  90/10/07  13:17:08  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:50:55  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:55:19  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:32:52  jeffc]
 * 
 * 	Eliminate old, useless QUOTA code.
 * 	[90/08/18  03:40:23  nags]
 * 
 * 	doc change only
 * 	[90/08/14  14:16:52  hosking]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:30  mcg]
 * 
 * 	Put usage of SULOCK flag under #if !MACH.
 * 	[90/08/12  12:06:10  ers]
 * 
 * Revision 1.17  90/09/23  15:42:53  devrcs
 * 	In exit(), send SIGHUP to foreground processes when a controlling
 * 	process (a session leader with a controlling terminal) exits; also,
 * 	send SIGHUP and SIGCONT to all processes in a newly-orphaned
 * 	process group if any of them are stopped.
 * 	In waitf(), enable test for invalid options except under BSD4.3
 * 	compatibility; formerly, this test was disabled by #ifdef notyet.
 * 	[90/09/13  15:08:11  coren]
 * 
 * 	Backed out previous change for cleanlocks().
 * 	It was incorrect, and has been moved to closef,
 * 	where it belongs (bug 1035).
 * 	[90/09/14  13:44:17  gmf]
 * 
 * 	In exit(), when file descriptors are being closef()'ed, check if the
 * 	vnode associated with the file descriptor has any file locks on it.
 * 	If so, call cleanlocks() to delete all file locks belonging to the
 * 	exiting process.
 * 	[90/09/10  13:51:01  swallace]
 * 
 * Revision 1.16  90/08/24  11:16:02  devrcs
 * 	removed u.u_error references
 * 	[90/08/20  12:34:04  gmf]
 * 
 * 	Remove setjmp in waitf -- use tsleep instead.
 * 	[90/08/20  06:39:48  gmf]
 * 
 * 	Changes for new system call interface.
 * 	[90/08/19  12:32:28  gmf]
 * 
 * 	Modify owait/wait4/waitf interface.
 * 
 * Revision 1.15  90/08/09  13:13:45  devrcs
 * 	Removed crfree and setting of u.u_cred to NOCRED.  Let
 * 	thread_deallocate free the creds.
 * 	[90/07/25  15:25:54  nags]
 * 
 * 	Deallocate credentials on thread exit; cleanup.
 * 	[90/07/24  13:03:10  nags]
 * 
 * Revision 1.14  90/07/27  08:43:48  devrcs
 * 	Send SIGCHLD to init when a parent of zombies exits.
 * 	[90/07/19  13:24:57  ers]
 * 
 * 	Zombies must retain creds.
 * 	[90/07/17  08:41:52  nags]
 * 
 * Revision 1.13  90/07/17  11:18:30  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:51:35  seiden]
 * 
 * 	Added cast for SIG_IGN.
 * 	[90/07/07  08:20:39  gm]
 * 
 * Revision 1.12  90/07/05  23:07:30  devrcs
 * 	Add support for per thread synchronous signals
 * 	[90/06/29  09:09:16  sp]
 * 
 * Revision 1.11  90/06/22  20:05:48  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:53:30  seiden]
 * 
 * 	Condensed ancient history (reverse chronology):
 * 
 * 	Nags merge.					nags@encore.com
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing	seiden@osf.org
 * 	Pass exit status to acct()			ers@osf.org
 * 	Changes for gcc					brezak@osf.org
 * 	Use kalloc instead of mbuf for zombie stats	tmt@osf.org
 * 	Made old syscalls COMPAT			coren@osf.org
 * 	Merge with noemi's changes.			bet@osf.org
 * 	Wait shouldn't clobber u.u_error after waitf.	noemi@osf.org
 * 	Removed sysv_ipc conditional			bet@osf.org
 * 	Added semexit to exit for System V semaphores.	bet@osf.org
 * 	Not quite ready for old syscalls to be COMPAT.	coren@osf.org
 * 	Merged Coren's and Morris's changes.		ers@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.		coren@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Fixed includes, vnode changes, etc.		ers@osf.org
 * 	MACH X115 Update.				gm@osf.org
 * 	Call closef on fd !=NULL && fd!=U_FD_RESERVED.	boykin@encore.com
 * 	Early Mach 2.5 merge with parallelized stuff.	alan@encore.com
 * 	MMAX_MP:  merge with preliminary MACH_VFS.	alan@encore.com
 * 	Fix time calculations in exit.			dlb@cmu.edu
 * 	Machine-independent version of wait3.		af@cmu.edu
 * 	Floating Point Support.				af@cmu.edu
 * 	Cleaned up the wait business for mips.		af@cmu.edu
 * 	Use new inode macros.				gm0w@cmu.edu
 * 	More cleanup.					rpd@cmu.edu
 * 	Changes for cleanup.				gm0w@cmu.edu
 * 	Code cleanup cataclysm.				mwyoung@cmu.edu
 * 	Changes for MIPS: added wait and friends.	af@cmu.edu
 * 	Changes for I386: i386 registers in wait.	rvb@cmu.edu
 * 	Vnode support.					jsb@cmu.edu
 * 	Removed old MACH conditionals.			mwyoung@cmu.edu
 * 	MMAX_MP:  Catch interrupted closef's in exit.	mach@encore.com
 * 	Corrected include file references.		mwyoung@cmu.edu
 * 	Eliminated use of kern/mach_ipc_defs.h.		rpd@cmu.edu
 * 	Fpaclose_thread now done by thread_terminate.	dbg@cmu.edu
 * 	Exit must walk entire file descriptor table.	alan@encore.com
 * 	MACH_TIME_NEW is now standard.			dlb@cmu.edu
 * 	MACH: Remove references to multprog.		mwyoung@cmu.edu
 * 	Use thread_read_times to get times.		dlb@cmu.edu
 * 	Added call to fpaclose_thread() in exit().	jjc@cmu.edu
 * 	Update MACH_TIME_NEW to use time_value_t's.	dlb@cmu.edu
 * 	Delinted.					dbg@cmu.edu
 * 	Exit halts all threads in task.			dbg@cmu.edu
 * 	New termination logic for task_terminate.	dbg@cmu.edu
 * 	Condensed conditionals, purged history.		avie@cmu.edu
 * 	MACH_VFS: Change inode references to vnodes.	king@next.com
 * 	[90/06/12  19:05:43  nags]
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
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_exit.c	7.12 (Berkeley) 11/25/89
 */

#include <cputypes.h>
#include <sys/secdefines.h>
#include <machine/reg.h>
#ifdef	ibmrt
#include <ca/scr.h>
#endif
#if	!defined(ibmrt) && !defined(mips)
#include <machine/psl.h>
#endif
#include <sys/unix_defs.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/map.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/tty.h>
#include <sys/signal.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/vnode.h>
#include <sys/syslog.h>
#include <mach/kern_return.h>
#include <kern/kalloc.h>
#include <kern/sched_prim.h>
#include <kern/parallel.h>
#include <mach/time_value.h>
#if SEC_BASE
#include <sys/security.h>
#endif

#include "profiling.h"

#define TASK_NULL	MACH_PORT_NULL

/*
 * Terminate the specified task p, with wait() status return value rv.
 * May be called by the process itself, as in the exit(rv) system call,
 * or by others, such as when signal delivery causes the termination of
 * a signalled proc (with signal-related wait() status rv).
 *
 * Release resources.
 * Save u. area for parent to look at.
 * Enter zombie state.
 * Wake up parent and init processes,
 * and dispose of children.
 */
pproc_exit(p, rv)
	register struct proc *p;
	int rv;
{
	register int i;
	register int x;
	struct rusage *ru;
	struct proc *save_proc   = u.u_procp;

	unix_master();

#ifdef	PGINPROF
	vmsizmon();
#endif

#ifdef	OSF1_SERVER
	/*
	 * This file can't be compiled with this flag turned off: the
	 * alternate code has been suppressed for readability reasons
	 * (jqr).
	 */
	/*
	 * If the task is already exiting, quit.
	 */
        if (p->p_flag & SWEXIT) {
#if MACH_ASSERT
		printf("pproc_exit(): SWEXIT already set pid %d -- returning\n",
			p->p_pid);
#endif	/* MACH_ASSERT */
                unix_release();
                return;
        }

	/*
	 * Mark it as exiting.
	 * Take the process lock to avoid new server threads to register 
	 * for this process. When SWEXIT flag is on, registration requests
	 * are refused, so it's safe after that.
	 */
	simple_lock(&p->p_lock);
	p->p_flag |= SWEXIT;
	p->p_flag &= ~STRC;
#ifdef NX
        p->p_flag &= ~SWTED;
#endif /* NX */

	simple_unlock(&p->p_lock);

	ru = (struct rusage *)kalloc(sizeof (struct rusage));

	/*
	 * Suspend the user task.
	 */
	(void) unix_task_suspend(p);

	/*
	 * Wait until any outstanding side threads get started up and say
	 * it's OK for us to proceed.  (They check for SWEXIT we just set.)
	 * While the process is active, increments and decrements of
	 * p_exit_hold_count are locked by p_lock.  Once the process is
	 * exiting, no increments should happen.
	 */
	while (p->p_exit_hold_count > 0) {
		sleep((caddr_t) &p->p_exit_hold_count, PSPECL);
	}

	/*
	 * If the process is stopped, wake up the server threads.
	 * Do not use start() because that will change the stop signal.
	 */
	if (p->p_stat == SSTOP) {
	    p->p_stat = SRUN;
	    wakeup((caddr_t)&p->p_stat);
	}

	/*
	 * Terminate all server threads working for that task, except for
	 * the current thread which we deregister instead.
	 */
	{
	    register uthread_t	cth;
	    extern   uthread_t	server_thread_find();
	    extern   void	server_thread_deregister();

	    while ((cth = server_thread_find(p)) != 0) {
		if (cth == &u) {
		    server_thread_deregister(cth, p);
		    save_proc = 0;
		}
		else {
		    clear_wait(cth, THREAD_SHOULD_TERMINATE, FALSE);
		    sleep((caddr_t)&p->p_flag, PSPECL);
		}
	    }
	}

#ifdef PARACORE
	/*
	 * Get a core dump if requested.
	 */
	if (WCOREDUMP(rv)) {
		if (para_core_dump(p, WTERMSIG(rv)) != 0)
			rv &= ~WCOREFLAG;		/* no core */
	}
	
	if (p->p_utask.uu_ports_copied && 
	    p->p_utask.uu_core_directory != MACH_PORT_NULL) {
	    	kern_return_t ret;
		/*
		 * Deallocate the core port.
		 */
		ret = mach_port_deallocate(mach_task_self(),
					   p->p_utask.uu_core_directory);
		if (ret != KERN_SUCCESS) {
			printf("pproc_exit: Could not deallocate core port (error=0x%x)\n", ret);
		}
	}

	if (p->p_utask.uu_ports_copied && 
	    p->p_utask.uu_root_directory != MACH_PORT_NULL) {
	    	kern_return_t ret;
		/*
		 * Deallocate the root port got from proxy.
		 */
		ret = mach_port_deallocate(mach_task_self(),
					   p->p_utask.uu_root_directory);
		if (ret != KERN_SUCCESS) {
			printf("pproc_exit: Could not deallocate root port (error=0x%x)\n", ret);
		}
	}

#ifdef	OSF1_ADFS
	/*
	 * Release references to exec directory proxies.
	 */
	remote_vrele(&p->p_utask.uu_exec_utnd.utnd_cdir);	
	remote_vrele(&p->p_utask.uu_exec_utnd.utnd_rdir);
#endif	OSF1_ADFS
#else /* PARACORE */
	/*
	 * Get a core dump if requested.
	 */
	if (WCOREDUMP(rv)) {
		switch (core_dump(p)) {
		case -1:	rv &= ~WCOREFLAG;	/* no core */
				break;
		case 0:		break;			/* success */
		default:	break;			/* failed */
		}
	}
#endif /* PARACORE */

	/*
	 * Pretend we are that process, for the duration of the call.
	 */
	u.u_procp = p;
	u.uu_proc_exit = TRUE;

	for (i = 0; i < NSIG; i++)
		signal_disposition(i) = (void (*)()) SIG_IGN;
	untimeout(realitexpire, (caddr_t)p);
	p->p_sigignore = ~0;

	/*
	 * Get rid of the credentials port.
	 */
	credentials_deallocate(p->p_cred, &p->p_utask.uu_ru.ru_inblock, 
				&p->p_utask.uu_ru.ru_oublock);

#else	/* OSF1_SERVER */
#endif	/* OSF1_SERVER */

#ifdef OSF1_ADFS
	remote_vrele(&u.u_cdirproxy);
	remote_vrele(&u.u_rdirproxy);
#else
	vrele(u.u_cdir);
	if (u.u_rdir)
		vrele(u.u_rdir);
#endif
	u.u_rlimit[RLIMIT_FSIZE].rlim_cur = RLIM_INFINITY;
#if     SEC_BASE
	audstub_exit1();	/* MP note: pendable */
#endif
#if SEC_ARCH
	sp_clear_tags();
#endif

#ifdef TNC
	tnc_semexit();
#else
	semexit();		
#endif

#ifdef i386
	clx_exit();		/* Xenix clean up */
#endif
	simple_lock(&p->p_lock);
	if (p->p_ref_count != 1)
		panic("proc_exit");
	p->p_ref_count = 0;
	simple_unlock(&p->p_lock);
	if (*p->p_prev = p->p_nxt)		/* off allproc queue */
		p->p_nxt->p_prev = p->p_prev;
	if (p->p_nxt = zombproc)		/* onto zombproc */
		p->p_nxt->p_prev = &p->p_nxt;
	p->p_prev = &zombproc;
	zombproc = p;
	p->p_stat = SZOMB;
	i = PIDHASH(p->p_pid);
	x = p - proc;
	if (pidhash[i] == x)
		pidhash[i] = p->p_idhash;
	else {
		for (i = pidhash[i]; i != 0; i = proc[i].p_idhash)
			if (proc[i].p_idhash == x) {
				proc[i].p_idhash = p->p_idhash;
				goto done;
			}
		panic("exit");
	}
	if (p->p_pid == 1) {
		printf("init exited with %d\n", WEXITSTATUS(rv));
		if (u.u_data_start == 0) {
			printf("Can't exec /etc/init\n");
			for (;;)
				;
		} else
			panic("init died");
	}
done:
	p->p_xstat = rv;
	/*
	 *	Setting the signal lock to exit state stopped all the
	 *	threads except this one in their tracks, so it's ok to
	 *	get their times here.
	 */

#ifdef	OSF1_SERVER
	/*
	 * Get the times for all threads in the process.
	 */
	{
	    struct task_thread_times_info
				thread_times;
	    struct task_basic_info
				bi;
	    unsigned int	count;

	    /*
	     * Get times for dead threads
	     */
	    count = TASK_BASIC_INFO_COUNT;
	    (void) task_info(p->p_task,
			     TASK_BASIC_INFO,
			     (task_info_t)&bi,
			     &count);

	    /*
	     * Get times for live threads
	     */
	    count = TASK_THREAD_TIMES_INFO_COUNT;
	    (void) task_info(p->p_task,
			     TASK_THREAD_TIMES_INFO,
			     (task_info_t)&thread_times,
			     &count);

	    /*
	     * Add user run times.
	     */
	    timevaladd(&bi.user_time, &thread_times.user_time);
	    p->p_utask.uu_ru.ru_utime.tv_sec  = bi.user_time.seconds;
	    p->p_utask.uu_ru.ru_utime.tv_usec = bi.user_time.microseconds;

	    /*
	     * Add system run times.
	     */
	    timevaladd(&bi.system_time, &thread_times.system_time);
	    p->p_utask.uu_ru.ru_stime.tv_sec  = bi.system_time.seconds;
	    p->p_utask.uu_ru.ru_stime.tv_usec = bi.system_time.microseconds;

	}
#else	/* OSF1_SERVER */
#endif	/* OSF1_SERVER */
	if( p->p_task != MACH_PORT_DEAD)
		task_to_proc_remove(p->p_task);
	*ru = u.u_ru;
	p->p_ru = ru;
	acct(rv);			/* write acct record */
	ruadd(p->p_ru, &u.u_cru);	/* now add in children for wait */

#ifdef	OSF1_SERVER
	/*
	 * blow away the address maps
	 */
	vm_exit(p);

#if PROFILING
#if GPROF
	if (p->p_taskprofed) {	/* We are emulator_profiling. */
		if (!p->p_profport)
			panic("exit: profport");
		task_sample(p->p_task, MACH_PORT_NULL);
		pport_to_proc_sleep(p->p_profport);
	}
#endif

	if (p->p_utask.uu_prof_on == TRUE) {

	    /* Disable profiling of the process */
	    p->p_utask.uu_prof_on = FALSE;

	    /* Make a syscall to disable the sampling service in
	     * the micro-kernel.
	     * The value of the 'reply_port' parameter is set to
	     * MACH_PORT_NULL to indicate that sampling must be
	     * disabled.
	     */

	    /*
	     *	Disable the sampling service at micro-kernel level.
	     */
	    thread_sample((mach_port_t) p->p_thread, MACH_PORT_NULL);

	    /* This call will tell the micro-kernel to send the last buffer.
	     * As soon as the receiver (eat_buffer) receives it and
	     * makes the last updates to the internal buffer, it will
	     * wake us up.
	     */

	    pport_to_proc_sleep(p->p_profport);
	}

	/* Deallocate the per user process profiling port */
	if (p->p_profport)
		pport_to_proc_remove(p->p_profport);

#endif /* PROFILING */

	/*
	 * Get rid of send right on the emulator callback thread port.
	 */
	if (p->p_callback_thread != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(),
					    p->p_callback_thread);
		p->p_callback_thread = MACH_PORT_NULL;
	}

	/*
	 * Get rid of send right on the emulator callback port.
	 */
	if (p->p_callback != MACH_PORT_NULL) {
		(void) mach_port_deallocate(mach_task_self(), p->p_callback);
		p->p_callback = MACH_PORT_NULL;
	}

	(void) task_terminate(p->p_task);

	/*
	 * Yes, it can happen.  The user-reference counts on
	 * the task/thread port names might be greater than one
	 * because other threads in the Unix server were doing
	 * things with the ports.
	 */

	(void) mach_port_deallocate(mach_task_self(), p->p_task);
	p->p_task = MACH_PORT_NULL;
	if (MACH_PORT_VALID(p->p_thread))
	    (void) mach_port_deallocate(mach_task_self(), p->p_thread);
	p->p_thread = MACH_PORT_NULL;

	/*
	 * Deallocate the port associated with the proc structure, obtained
	 * with task_get_bootstrap_port in procdup().
	 */
	(void) mach_port_deallocate(mach_task_self(), (mach_port_t) p);
	/*
	 * Return to our former identity.
	 */
	u.uu_proc_exit = FALSE;
	u.u_procp = save_proc;


	unix_release();
#else	/* OSF1_SERVER */
#endif	/* OSF1_SERVER */
}


/*
 * Zero out phys proc structure and put it back on the free list.  Called
 * either from pproc_reap(), or from pproc_release() if this final reap step
 * had been delayed due to an outstanding pproc ref.
 */
void
pproc_clear(p)
	struct proc *p;
{
	ASSERT(syscall_on_master());
	ASSERT(p->p_stat == NULL);
	ASSERT((p->p_flag&SWEXIT) != 0);
	ASSERT(p->p_ref_count == 0);
	p->p_xstat = 0;
	if (p->p_ru) {
		kfree(p->p_ru, sizeof (struct rusage));
		p->p_ru = 0;
	}
	p->p_pid = 0;
	p->p_ppid = 0;
	crfree(p->p_rcred);		/* kill off old creds */
	p->p_sig = 0;
	p->p_sigcatch = 0;
	p->p_sigignore = 0;
	p->p_sigmask = 0;
	p->p_flag = 0;
	p->p_vproc = NULL;
	p->p_stopsig = 0;
	p->p_cursig = 0;
	p->p_nxt = freeproc;		/* onto freeproc */
	freeproc = p;
}

/*
 * Lay zombie to rest
 */
int
pproc_reap(p, options, wstat, rusage)
	struct proc *p;
	int options;
	int *wstat;
	struct rusage_dev *rusage;
{
	int stopsig;

	TNC_unix_master();

	if (p->p_stat == SZOMB && (options & VPROC_WNOWAIT)) {
		*wstat = p->p_xstat;
		TNC_unix_release();
		return (ESUCCESS);
	}
	if (p->p_stat == SZOMB) {
		*wstat = p->p_xstat;
		*rusage = *(struct rusage_dev *)p->p_ru;
#ifdef NX
                if (!(options & WNOWAIT)) {
#endif /* NX */
		p->p_stat = NULL;		/* clear process state field */
		if (*p->p_prev = p->p_nxt)	/* off zombproc */
			p->p_nxt->p_prev = p->p_prev;

		/*
		 * We are about to call pproc_clear() to zero out the
		 * physical proc structure and and put that structure back
		 * on the proc struct free list.  If someone is still
		 * referencing the proc struct, just set the SREAPED flag
		 * bit.  When pproc_release() decrements the ref count back
		 * to zero, it will see the SREAPED but and call pproc_clear()
		 * directly.
		 */
		if (p->p_pproc_hold_count > 0) {
			p->p_flag |= SREAPED;	/* finish reap later */
		} else {
			pproc_clear(p);		/* finish reap now */
		}

#ifdef NX
                }
#endif /* NX */
		TNC_unix_release();
		return (ESUCCESS);
	}
	if (p->p_stat == SSTOP && 	/* we'll see that later (jqr) */
	    (p->p_flag & SWTED) == 0 &&
	    (p->p_flag & STRC || options & VPROC_WUNTRACED)) {
		p->p_flag |= SWTED;
#ifdef NX
		/*
                 * Turn reaped off if the parent and the tam are the same
                 */

	        if (nx_tam_attached(p) && (p->p_flag & STRC))
			p->p_flag &= ~SWTED;
#endif /* NX */
		/*
 		 *	Stop-class signals are in p_stopsig instead
 		 *	of p_cursig under some circumstances.
 		 */
		if (p->p_cursig == 0)
 			stopsig = p->p_stopsig;
		else
			stopsig = p->p_cursig;
		*wstat = W_STOPCODE(stopsig);
		TNC_unix_release();
		return (ESUCCESS);
	}
	TNC_unix_release();
	return(ESRCH);
}

int
pproc_hold(
	struct proc	*p,
	pid_t		pid)
{
#define	PPROC_HOLD_MAX	7	/* just a sanity limit */
	int		error = ESUCCESS;

	TNC_unix_master();
	if (p != NULL && p->p_pid == pid && p->p_vproc->vp_pid == pid
				&& (p->p_flag&SREAPED) == 0) {
		simple_lock(&p->p_lock);
		ASSERT((unsigned)p->p_pproc_hold_count <= PPROC_HOLD_MAX);
		++p->p_pproc_hold_count;
		simple_unlock(&p->p_lock);
	} else {
		error = ESRCH;	/* proc struct already gone or going */
	}
	TNC_unix_release();
	return(error);
}

void
pproc_release(
	struct proc	*p,
	pid_t		pid)
{
	ASSERT(p != NULL);
	ASSERT(p->p_pid == pid);
	ASSERT(p->p_vproc != NULL);
	ASSERT(p->p_vproc->vp_pid == pid);

	TNC_unix_master();
	simple_lock(&p->p_lock);
	--p->p_pproc_hold_count;
	ASSERT((unsigned)p->p_pproc_hold_count <= PPROC_HOLD_MAX);
	if ((p->p_flag&SREAPED) != 0 && p->p_pproc_hold_count == 0) {
		simple_unlock(&p->p_lock);
		pproc_clear(p);		/* zero pproc & put on free list */
	} else {
		simple_unlock(&p->p_lock);
	}
	TNC_unix_release();
}

void
pproc_wakeup(p)
	struct proc *p;
{
	ASSERT(p != NULL);

	TNC_unix_master();

	simple_lock(&p->p_lock);
	p->p_flag |= SWAKEUPDONE;	/* matters only during wait() */
	simple_unlock(&p->p_lock);

	wakeup(p);

	TNC_unix_release();

	return;
}

int
pproc_assert_sleep_wait(p)
	struct proc *p;
{
	ASSERT(p != NULL);

	simple_lock(&p->p_lock);
	p->p_flag &= ~SWAKEUPDONE;	/* matters only during wait() */
	simple_unlock(&p->p_lock);

	return(ESUCCESS);
}

int
pproc_sleep_waiting(p)
	struct proc *p;
{
	int	error = ESUCCESS;
	int	flags;

	ASSERT(p != NULL);

	TNC_unix_master();

	simple_lock(&p->p_lock);
	flags = p->p_flag;
	simple_unlock(&p->p_lock);

	if ((flags&SWAKEUPDONE) == 0) {
		error = tsleep((caddr_t)p, PWAIT | PCATCH, "wait", 0);
	}

	TNC_unix_release();
	return(error);
}

pproc_add_rusage(p, ru_loc)
	struct proc *p;
	struct rusage_dev *ru_loc;
{
	TNC_unix_master();
	ruadd(&p->p_utask.u_cru, ru_loc);
	TNC_unix_release();
}

kern_return_t	init_process()
/*
 *	Make the current process an "init" process, meaning
 *	that it doesn't have a parent, and that it won't be
 *	gunned down by kill(-1, 0).
 */
{
	register struct proc *p;

#if     SEC_BASE
	if (!privileged(SEC_SETPROCIDENT, EPERM))
#else
	if (suser(u.u_cred, &u.u_acflag) != 0)
#endif
		return(KERN_NO_ACCESS);

#ifndef	TNC
	unix_master();
#endif	/* TNC */

	p = u.u_procp;

	/*
	 *	Take us out of the sibling chain, and
	 *	out of our parent's child chain.
	 */

	(void) VPOP_SETPINIT(p->p_vproc);

#ifndef	TNC
	unix_release();
#endif	/* TNC */

	return(KERN_SUCCESS);
}
