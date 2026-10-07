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
 * Copyright (c) 1990 Carnegie-Mellon University
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
 * $Log: mach_signal.c,v $
 * Revision 1.36  1995/02/18  01:23:12  yazz
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
 * Revision 1.35  1995/02/13  23:00:27  nandy
 * Corrected a typing error in the last change.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.34  1995/02/13  18:57:22  nandy
 * In sig_default(), if the signal is SIGKILL, exit and return.
 *
 *  Reviewer: BoB Yasi
 *  Risk: L
 *  Benefit or PTS #: 12136
 *  Testing: IPD regression
 *  Module(s): bsd/mach_signal.c
 *
 * Revision 1.33  1995/02/01  21:28:36  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.32  1995/01/27  22:38:37  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 12239
 *  Testing: EATs sched, os_interfaces, concrolc
 *  Module(s): server/bsd/mach_signal.c
 * The side thread server routine now holds a pproc reference during the
 * side thread call (most relevant for side_issig_psig()).
 *
 * Revision 1.31  1994/11/18  20:27:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.30  1994/11/08  20:05:09  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 9853, 11537, 11538
 *  Testing: sched and concur EATs
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * Do not derefence the pproc pointer after the proc has been killed and its
 * issig_psig thread deregistered.
 *
 * Revision 1.29  1994/11/03  16:04:02  yazz
 *   Reviewer: Chris Peak, John Litvin
 *   Risk: med
 *   Benefit or PTS #: #11459
 *   Testing: corefile EAT
 *   Module(s):
 *  	server/bsd/kern_exit.c
 *  	server/bsd/kern_sig.c
 *  	server/bsd/mach_signal.c
 *  	server/sys/proc.h
 *  	server/tnc/dvp_pvpops.c
 *  	server/tnc/rtask_cli_pproc.c
 *  	server/uxkern/syscall_subr.c
 *
 * Have side_issig_psig() detect when it's been deregistered by exit processing
 * so it won't dereference a stale pproc pointer.
 *
 * Revision 1.28  1994/10/25  23:11:44  yazz
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
 * Side-thread changes.  Renamed p_sigref to the more general p_exit_hold_count
 * and p_issig_count to the more correct p_issig_flag.  Handle the special
 * psig_retry and issig_psig threads using the new standardized side-thread
 * mechanism.  Define the side-thread server routine, and a new side thread
 * routine to perform exit processing (that the receipt of some signals
 * requires).  SIG_DEBUG messages improved and available in all assertful
 * servers.
 *
 * Revision 1.27  1994/09/13  16:01:27  johannes
 * thread_signal(): updating the new field 'uu_faulting_thread' in the utask
 * 		 structure and the field 'u_code' in the uthread structure
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 10486
 *  Testing: developer tests, corefile EAT
 *  Module(s): svr/server/sys/user.h
 *             svr/server/bsd/kern_fork.c
 *             svr/server/bsd/mach_signal.c
 *             svr/server/paracore/core.c
 *
 * Revision 1.26  1994/08/31  22:46:32  mtm
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
 * Revision 1.24.2.1  1994/08/25  15:39:15  nandy
 * In sig_default() if SIGKILL is pending, call proc_exit directly for
 * SIGTRAP.
 *
 *  Reviewer: Chris Peak
 *  Risk: L
 *  Benefit or PTS #: 10613
 *  Testing: PTS test case
 *  Module(s): bsd/mach_signal.c
 *
 * Revision 1.24  1994/06/21  23:50:21  jlitvin
 * Fix RCS comments.
 *
 * Revision 1.23  1994/06/18  00:02:44  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.22  1994/04/18  16:01:29  jlitvin
 * Remove 1 of 2 conflicting bug fixes for the same problem.  There is no
 * need to run the same code twice!
 *
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 8734
 *  Testing: VSX
 *  Module(s): bsd/mach_signal.c
 *
 * Revision 1.21  1994/04/04  23:10:22  jlitvin
 * Fix some very minor typos in signal debugging code.
 *
 * Revision 1.20  1994/03/11  23:00:06  jlitvin
 * Fix another minor merge error.
 *
 * Revision 1.19  1994/03/02  16:12:15  nandy
 * Merged from R1_2 branch.
 *
 * Revision 1.18  1994/02/22  18:46:13  jlitvin
 * Fix another merge error found by Chris Peak of Locus.  As a bonus, I
 * noticed and fixed another problem with the last merge.  (PTS #8196)
 *
 * Revision 1.17  1994/02/16  15:23:28  nandy
 * Merged from the R1_2 branch.
 *
 * Revision 1.16  1994/02/08  15:13:59  cfj
 * Merge revision 1.14.4.1 from R1_2 branch.  See the
 * log entry for revision 1.14.4.1 for details.
 *
 * Revision 1.15  1994/01/13  17:52:45  jlitvin
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
 * Revision 1.14.4.3  1994/03/02  15:51:00  nandy
 * In thread_psignal(), raise the flag only if an exiting process gets another
 * exception in the emulator.
 *
 *  Reviewer: cfj
 *  Risk: LOW
 *  Benefit or PTS #: 8104
 *  Testing: PTS test
 *  Module(s): server/bsd/mach_signal.c
 *
 * Revision 1.14.4.2  1994/02/15  19:28:14  nandy
 * Drop the master lock before calls to VPOP_EXIT().
 *
 *  Reviewer: Chris Peak
 *  Risk: Low
 *  Benefit or PTS #: 7684
 *  Testing: Test in the PTS report
 *  Module(s): mach_signal.c
 *
 * Revision 1.14.4.1  1994/02/08  01:28:16  cfj
 * In threa_psignal(), check to make sure that thread_signal() did not
 * kill the process right after the call to thread_signal().
 *
 *  Reviewer: Chris Peak (Locus)
 *  Risk: L
 *  Benefit or PTS #:8090 Note: this fix made months ago by OSF in their
 *                   code base.
 *  Testing:
 *  Module(s):server/bsd/mach_signal.c
 *
 * Revision 1.14  1993/07/14  17:48:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.7  1993/07/01  18:48:39  cfj
 * Adding new code from vendor
 *
 * Revision 1.13  1993/05/06  19:04:38  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.5  1993/05/03  17:24:53  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.12  1993/04/03  03:04:14  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.11  1993/04/03  01:23:10  cfj
 * Merge with T9.
 *
 * Revision 1.6.6.4  1993/04/03  01:16:13  cfj
 * Turn off signal debug.
 *
 * Revision 1.10  1993/03/04  19:07:50  nandy
 * Cleaned merge junk.
 *
 * Revision 1.6.6.3  1993/03/04  18:48:23  nandy
 * unix_task_suspend() calls task_resume() if the task is alread suspended.
 *
 * Revision 1.6.6.2  1993/02/27  00:46:40  cfj
 * No longer suspend a task who takes an exception in the
 * emulator.  Print the error message and go ahead and deliver the signal.
 *
 * Revision 1.6.6.1  1993/02/15  04:50:47  nandy
 * When a traced process gets a signal while stooped on a break
 * point, it now finds the signal in thread_psignal().
 *
 * Revision 1.1.2.2.2.3  1993/01/09  00:03:40  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.6  1993/01/05  00:24:08  shala
 * Fix for nx_applications to not send signal to the parent. set p_cursig
 * to SIGUSR2.
 *
 * Revision 1.1.2.2.2.2  1992/12/16  05:58:49  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.5  1992/12/15  00:50:44  cfj
 * 	Revision 2.26  92/12/14  14:45:33  chrisp
 * 	In thread_psignal(), correct typo uprintf -> printf.
 *
 * Revision 1.1.2.2.2.1  1992/12/14  23:08:47  brad
 * Merged tip of old NX branch with PFS branch.
 *
 * Revision 1.4  1992/12/11  02:54:44  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:16:29  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.3  1992/11/24  21:31:22  cfj
 * Turn off SIG_DEBUG.
 *
 * Revision 1.1.2.2  1992/11/09  17:55:02  cfj
 * Conflict resolution of 11/05/92 bug fix drop from Locus.
 *
 * Revision 1.1.2.1  1992/11/06  00:06:44  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.24  1992/11/02  21:48:48  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.23  1992/10/17  18:25:22  cfj
 * IPD & Allocator Support.
 *
 * Revision 2.25  92/11/23  16:05:37  klh
 * 	Revision 2.22  92/11/17  19:47:37  loverso
 * 		Correctly pass exiting flag to uemul_release_state().
 * 		Since unix_task_suspend() can now get called for simultaneously,
 * 		prevent all but the first caller from doing anything.
 * 		proc_exit now just signals for pproc_exit to dump core.
 * 		If unix_task_suspend() gets back a MACH error, kill the process.
 * 		(loverso)
 * 
 * 		Allow thread_psignal to server task for NFS ENOSPC.  (mmp)
 * 
 * Revision 2.24  92/11/12  14:13:54  chrisp
 * For TNC, update proc port sequence count in bsd_unix_task_suspend().
 * 
 * Revision 2.23  92/11/04  11:15:25  chrisp
 * For TNC, ignore FPU exceptions in the server.
 * 
 *    Revision 2.28  93/05/16  21:03:48  loverso
 *            Set p_stopsig before stopping task.
 * 
 *    Revision 2.27  93/03/05  17:28:04  loverso
 *            If unix_task_suspend() is called on a task already suspended,
 *            DO NOT leave it's suspend count too high.  (nandini@intel)
 *
 *    Revision 2.26  93/02/25  12:06:06  loverso
 *            BUG_COMPAT is now a compile-time config option.
 *            allow_page_0_access defaults to false; use boot-config to change.
 *
 *    Revision 2.25  93/02/04  11:48:45  durriya
 *            [1992/09/23  12:05:07  barbou]
 *            Fix for bug #189: process pending signals when returning from
 *            exception.
 *
 *    Revision 2.24  93/01/06  14:58:49  loverso
 *            Fix typo.
 * 
 * Revision 2.22  92/10/06  12:10:33  roman
 * Fix RCS comments.
 *
 * Revision 2.21  92/10/05  13:58:43  klh
 * 	Revision 2.19  92/09/29  16:47:27  rabii
 * 		In unix_task_suspend(): enable ux_server_blocking|unblocking()
 * 		around calls to emulator; cleanup panic messages; add comments.
 * 		(loverso)
 * 
 * 	Revision 2.18  92/08/26  12:10:13  loverso
 * 		Enable BUG_COMPAT code.
 * 		Add unix_task_suspend and bsd_unix_task_suspend.
 * 		Replace task_suspend calls with unix_task_suspend.
 * 		WARNING: allow_page_0_access SHOULD BE FALSE!
 * 		(loverso)
 * 
 * 	Revision 2.17  92/08/13  19:17:42  rabii
 * 		Turned "on" code to upgrade page_0 protection to read only 
 *		and print console message in case of a process accessing 
 *		page_0 (rabii)
 * 
 * Revision 2.20  92/09/28  16:34:31  roman
 * Change VPOP_SET_STATE() to VPOP_REPORT_STATE() (more modern name).
 * 
 * Revision 2.19  92/08/06  16:38:25  roman
 * Fix RCS comments.
 * 
 * Revision 2.18  92/08/06  13:35:23  klh
 * 	Revision 2.16  92/07/29  08:59:32  rabii
 * 		Fixed RCS log
 * 
 * 	Revision 2.15  92/07/28  20:02:34  rabii
 * 		Replaced call to put_thread_state_pc with 
 *		put_thread_state_pc_err which also sets the error register 
 *		in the emulator task to the error code (rabii)
 * 
 * Revision 2.17  92/07/10  08:53:19  chrisp
 * For TNC, call vproc_end_port_op() on completion of RPC's using
 * 	a proc port but not using start/end_server_op. This is to
 * 	maintain message sequence counts.
 * 
 * Revision 2.16  92/07/09  11:32:32  roman
 * Fix prior RCS comments.
 * 
 * Revision 2.15  92/06/05  13:56:14  klh
 * 	Revision 2.14  92/05/31  18:58:21  loverso
 * 		Took "thread_psignal()" from gr4.0.
 * 		Page 0 code included, but disabled. (loverso)
 * 
 * 		Remove duplicated include of parallel.h (pjg).
 * 
 * 		Revision 3.15  92/03/13  15:18:37  condict
 * 		Change include dir for ux_exception.h.
 * 
 * 		Revision 3.14  92/02/28  17:59:28  barbou
 * 		Suspend the user task when it generates an unexpected emulator
 * 		exception, so that one can attach a debugger to it and 
 *		investigate this abnormal behaviour.
 * 
 * 		Revision 3.13  92/02/28  00:11:14  condict
 * 		If emulator gets an exception in the range of addresses 
 *		previously specified in its bsd_emul_uacc message, it is a 
 *		fault on a user addr, so instead of signaling, yank the 
 *		emulator to the error recovery address.
 * 
 * 		Revision 3.12  91/12/20  17:56:36  barbou
 * 		Send a message to user if a process gets an exception in 
 *		the emulator.
 * 
 * 	Revision 2.13  92/05/27  20:05:13  pjg
 * 		Don't catch signals in mpsleep in sbsd_issig_psig.
 * 
 * 	Revision 2.12  92/05/24  20:47:19  pjg
 * 		Fix number of parameters in new calls to uarea_terminate.
 * 
 * 	Revision 2.11  92/05/24  14:17:22  pjg
 * 		Call uarea_terminate in issig_psig after releasing the 
 *		master lock.
 * 
 * 		Revision 3.17  92/04/03  14:13:33  condict
 * 		Execute all signal code in the sbsd_* funcs with master 
 *		lock held.  Needed in the uniproc configuration.
 * 
 * 		Revision 3.16  92/03/24  21:03:19  barbou
 * 		Fixed a few null pointers in the SIG_DEBUG trace.
 * 		Fix for bug #121: don't report emulator EXC_BREAKPOINT 
 *		exceptions.
 * 		Allow read access to page 0 on memory exception if 
 *		`allow_page_0_access' is on (backward compatibility to be 
 *		able to run old servers as second servers).
 * 
 * Revision 2.14  92/03/24  08:59:28  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.10  92/03/09  14:27:25  durriya
 * 	Note: did not merge chunks of emulator exception handling (loverso)
 * 
 * 	92/02/28  17:59:28  barbou
 * 	Conditionalized some debug messages.
 * 
 * 	91/12/20  17:56:36  barbou
 * 	Removed server message when dumping core.
 * 	Partial fix for bug #44: job control signals were ignored by 
 *	bsd_take_signal: removed bogus code from the BSD server.
 * 
 * Revision 2.9  92/02/11  22:18:25  pjg
 * 	VPOP_SET_STOP_STATE becomes VPOP_SET_STATE.
 * 	Add SIGMIGRATE argument passing into signal handler (roman@locus).
 * 
 * Revision 2.8  92/01/14  11:10:32  roy
 * 	proc_sigtrylock is defined to simple_try_lock for !MAP_UAREA (sjs).
 * 
 * Revision 2.7  91/12/17  13:33:43  roy
 * 	91/12/13  13:03:05  sp
 * 	Remove VICE conditionals
 * 
 * 	91/10/29  16:11:21  barbou
 * 	Adapted to the new interface of server_thread_register().
 * 	Send core dump message to the user terminal instead of the console.
 * 
 * Revision 2.6  91/11/13  12:53:33  rabii
 * 	SIG_* (all defined in signal.h as (void *) cast to (int) for
 * 	switch stmts. Also moved <sys/vproc.h> before <ufs/inode.h>
 * 
 * Revision 2.5  91/10/14  12:09:06  sjs
 * 	91/09/17  11:51:12  barbou
 * 	Added missing synchronization between sbsd_issig_psig() and exit().
 * 
 * 	91/09/10  12:00:02  barbou
 * 	Use uarea_terminate() to decrement the reference count on the 
 * 	credentials.
 * 
 * Revision 2.4  91/10/04  14:49:34  chrisp
 * Get rid of extraneous $Log. Change call to exit() to call VPOP_EXIT() vproc
 * operation instead.
 * 
 * Revision 2.3  91/09/16  15:41:40  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Remove explicit dependency on the fact that ports and proc
 * 	pointers are equivalent (now hidden in port_to_proc_lookup() macro).
 * 	Set stop state via vproc operation.
 * 
 * Revision 2.2  91/08/31  13:22:56  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/08/12  15:35:11  sp
 * Fix missing braces courtesy of barbou
 * 
 * Revision 3.6  91/08/09  12:12:11  barbou
 * Added new code to try to force the delivery of pending signals if
 * the user process is not in the emulator code.
 * 
 * Revision 3.5  91/07/04  17:50:09  barbou
 * Discarded unused code (replaced by OSF/1 code). Debugging facility.
 * 
 * Revision 3.4  91/06/25  17:00:19  condict
 * Moved sys header files that were from OSF/1 kern dir, back to kern.
 * Added interruptible_only flag to unsleep; removed OSF1_SERVER ifdefs.
 * 
 * Revision 3.3  91/06/07  15:50:10  barbou
 * Don't use BSD gsignal() anymore. See kern_sig.c.
 * 
 * Revision 3.2  91/05/29  14:27:06  condict
 * Turn off SIG_DEBUG messages.
 * 
 * Revision 3.1  91/05/24  11:33:08  jose
 * Added proc_exit() function.
 * 
 * Revision 3.0  91/05/15  17:02:06  barbou
 * Original from BSD server. Use BSD internals while waiting for the OSF/1 ones.
 * 
 * Revision 2.11  91/03/20  14:58:20  dbg
 * 	Fixed issig to use psignal3, so proc_exit isn't called directly.
 * 	[91/02/03            rpd]
 * 
 * Revision 2.10  90/08/06  15:31:44  rwd
 * 	Fix new references to share_lock.
 * 	[90/06/27            rwd]
 * 	Fix references to share_lock.
 * 	[90/06/08            rwd]
 * 	Change sleeps PSPECL instead of PZERO to release a
 * 	server thread.
 * 	[90/05/29            rwd]
 * 
 * Revision 2.9  90/06/19  23:07:58  rpd
 * 	Removed obsolete debugging panic in sigpause.
 * 	[90/06/13            rpd]
 * 
 * 	Added psignal3, gsignal3.
 * 	[90/06/09            rpd]
 * 
 * 	Changed all the non-interruptible sleeps to use PSPECL.
 * 	[90/06/08            rpd]
 * 
 * Revision 2.8  90/06/02  15:22:14  rpd
 * 	Made thread_psignal more robust.  If it doesn't find a process,
 * 	it nukes the task.
 * 	[90/05/11            rpd]
 * 
 * 	Modified thread_psignal for new register/deregister conventions.
 * 	[90/05/10            rpd]
 * 	Fixed killpg1, gsignal loops to allow psignal to nuke processes.
 * 	[90/05/04            rpd]
 * 
 * 	Removed us_sigcheck; it is no longer needed.
 * 	[90/05/03            rpd]
 * 	In psignal, handle SIGKILL by nuking the victim directly.
 * 	[90/04/05            rpd]
 * 
 * 	Fixed thread_signal to not sleep if the signal is masked.
 * 	Instead just save the signal in p_sig, like the mainline does.
 * 	[90/03/27            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:39:03  rpd]
 * 	Fixed killpg1, gsignal loops to allow psignal to nuke processes.
 * 	[90/05/04            rpd]
 * 
 * 	Removed us_sigcheck; it is no longer needed.
 * 	[90/05/03            rpd]
 * 	In psignal, handle SIGKILL by nuking the victim directly.
 * 	[90/04/05            rpd]
 * 
 * 	Fixed thread_signal to not sleep if the signal is masked.
 * 	Instead just save the signal in p_sig, like the mainline does.
 * 	[90/03/27            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  19:39:03  rpd]
 * 
 * Revision 2.7  90/05/29  20:23:19  rwd
 * 	Took rpd version and undid newipc fixes.
 * 	[90/05/12            rwd]
 * 
 * 	Made thread_psignal more robust.  If it doesn't find a process,
 * 	it nukes the task.
 * 	[90/05/11            rpd]
 * 
 * 	Modified thread_psignal for new register/deregister conventions.
 * 	[90/05/10            rpd]
 * 
 * Revision 2.6  90/05/21  13:50:24  dbg
 * 	Always set sigtramp.
 * 	[90/04/23            dbg]
 * 
 * Revision 2.5  90/03/14  21:26:26  rwd
 * 	Fix typo in issig which masked sigignore at wrong times.
 * 	[90/03/01            rwd]
 * 	Change shared locks to use share_lock macros.
 * 	[90/02/16            rwd]
 * 	Added new MAP_UAREA code
 * 	[90/01/31            rwd]
 * 
 * Revision 2.4  89/11/29  15:27:44  af
 * 	Sigtramp must be set first, as setsigvec might deliver a signal
 * 	right away as a result of a different mask.
 * 	[89/11/26  11:32:18  af]
 * 
 * 	Signal trampoline support for mips et al.
 * 	[89/11/16  16:57:51  af]
 * 
 * Revision 2.3  89/11/15  13:26:57  dbg
 * 	Fix order of arguments to bsd_sigreturn to match MiG interface.
 * 	[89/11/08            dbg]
 * 	Thread_psignal does wakeup on exit if process is exiting.
 * 	[89/10/24            dbg]
 * 
 * Revision 2.2  89/10/17  11:25:56  rwd
 * 	Use p_siglock to protect p_sig*.
 * 
 * 	use p_siglock to protect p_sig* (ONLY!)
 * 	[89/09/25            dbg]
 * 
 * 	Completely rewrite.  Move signal handling back to server threads
 * 	for user process.
 * 	[89/09/21            dbg]
 * 
 * 	Add interrupt return parameter to bsd_take_signal and
 * 	bsd_sigreturn.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.1  89/08/04  14:08:09  rwd
 * Created.
 * 
 *  3-May-89  David Golub (dbg) at Carnegie-Mellon University
 *	Return signal parameters to user.  Signal frame manipulation is
 *	now done in emulator.
 *
 *  6-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Rearrange proc structure fields.
 *
 * 13-Feb-89  David Golub (dbg) at Carnegie-Mellon University
 *	Rewrite... make serial again (the locking is too messy to do
 *	partially)... make psignal deliver signal immediately if
 *	possible!
 *
 * 17-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Moved the surviving routines from kern_sig here.
 *
 *  5-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Moved out of kernel.
 *
 ****	Experimental...
 *	Move most of signal handling here.  Have it done by new
 *	kernel thread, to keep interrupt stuff out.
 *
 *  1-Oct-87  David Black (dlb) at Carnegie-Mellon University
 *	Created by cutting down psignal to only deal with exceptions.
 *
 * $EndLog$
 */

/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_sig.c	7.1 (Berkeley) 6/5/86
 */

/* Building with +test (STD+WS+test) sets MACH_ASSERT to 1 */
#include <mach_assert.h>
#define SIG_DEBUG_ON 	MACH_ASSERT

#if	SIG_DEBUG_ON

/*
 * Possible levels of debugging: (sig_debug variable)
 *	-1	no debugging (default)
 *	0	global signal activity:
 *		trace signal postings and deliveries (psignal3, bsd_take_signal)
 *	1	signal activity:
 *		trace the internal activity (function call and exit).
 *	2	internals:
 *		trace the internals of functions like psignal3, issig, psig...
 *	3	extra-internals:
 *		like 2 with some extra tracing.
 */
int	sig_debug = -1;		/* exported to kern_sig.c */

#define SIG_DEBUG(level, x)		\
	if (level <= sig_debug) {	\
		bootnode_printf x;	\
	} else 0

#else
#define	SIG_DEBUG(level, x)
#endif

#include <map_uarea.h>
#include <mapped_files.h>
#include <cpus.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <ufs/dir.h>
#include <sys/user.h>
#include <sys/acct.h>
#ifdef i860
#include <sys/vproc.h>
#endif /* i860 */
#include <ufs/inode.h>
#include <sys/proc.h>
#include <sys/wait.h>
#include <sys/signal_macros.h>
#ifndef i860
#include <sys/vproc.h>
#endif /* i860 */
#include <sys/kernel.h>
#include <kern/sched_prim.h>

#include <uxkern/vm_param.h>
#include <uxkern/import_mach.h>

#include <sys/syscall.h>
#include <uxkern/syscall_subr.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/bsd_2.h>
#include <sys/ux_exception.h>

#if	MAP_UAREA
#define	proc_sigtrylock(p)	share_try_lock(p, &(p)->p_siglock)
#else	MAP_UAREA
#define	proc_sigtrylock(p)	simple_lock_try(&(p)->p_siglock)
#endif	MAP_UAREA

#ifdef NX
extern boolean_t nx_application();
#endif /* NX */

/*
 *  Initialize this to TRUE if you want the user processes
 *  suspended in the emulator if the emulator takes an exception.
 */
boolean_t suspend_in_emulator = FALSE;

/*
 * From kern_sig.c
 */
#undef stopsigmask
#ifdef NX
#define stopsigmask     (sigmask(SIGSTOP)|sigmask(SIGTSTP)| \
                        sigmask(SIGTTIN)|sigmask(SIGTTOU) | \
                        sigmask(SIGUSR2))
#else
#define stopsigmask	(sigmask(SIGSTOP)|sigmask(SIGTSTP)| \
			sigmask(SIGTTIN)|sigmask(SIGTTOU))
#endif /* NX */

void
sbsd_side_thread_server(
	mach_port_t	*proc_port,	/* port this MIG rpc came in on */
	int		(*side_func)(),	/* routine to call */
	int		intarg1)	/* call with this integer arg */
{
	struct proc	*p = port_to_proc_lookup(proc_port);
	struct uthread	*uth = &u;
	struct vproc	*saved_vproc;
	int		tmp;

	uth->uu_master_lock = 0;	/* init thrd's ML at start of thread */
	uth->uu_side_flag = 1;		/* mark this as a side thread */

	ASSERT(p != NULL);

	/*
	 * Take the master lock to make sure the proc stays around as long
	 * as we keep holding it.  Release the exit hold count that kept
	 * the proc around while the simpleroutine rpc mach message that
	 * caused us to be called was still in transit.
	 */
	unix_master();

	/*
	 * Until p_exit_hold_count is back down to zero (and p_lock is
	 * unlocked) pproc_exit() will wait on p_exit_hold_count and not
	 * let the physical process disappear.  And pproc_exit() sets
	 * SWEXIT before waiting on p_exit_hold_count.
	 */
	ASSERT(p->p_exit_hold_count > 0);
	simple_lock(&p->p_lock);
	tmp = --p->p_exit_hold_count;	/* dec count, and save result */
	simple_unlock(&p->p_lock);	/* don't hold proc lock during wakeup */

	if (tmp == 0) {
		wakeup((caddr_t)&p->p_exit_hold_count);
	}

	saved_vproc = p->p_vproc;	/* save now in case proc destroyed */
	VPROC_HOLD(saved_vproc, "sbsd_side_thread_server");
					/* make sure vproc stays around */

	/*
	 * Try to register ourselves as a side thread of this proc.  If
	 * the proc is exiting this will fail and we don't call the side
	 * thread routine at all.  Side routine called with master lock
	 * already held.
	 */
	if (server_thread_register(uth, p) == 0) {
		SIG_DEBUG(1, ("sbsd_side_thread_server[%d]: calling side " \
				"func 0x%x\n", p->p_pid, side_func));
		uarea_init_side(uth, p);	/* won't clobber master_lock */


		(*side_func)(p, intarg1);


		SIG_DEBUG(1, ("sbsd_side_thread_server[%d]: back from func " \
				"0x%x\n", p->p_pid, side_func));
		uarea_terminate(uth);
		if (uth->uu_procp != 0) {	/* we're still reg'd */
			server_thread_deregister(uth, p);
		}
	} else {
		SIG_DEBUG(1, ("sbsd_side_thread_server[%d]: NOT calling side " \
				"func 0x%x\n", p->p_pid, side_func));
	}

	unix_release();


#ifdef	TNC
	vproc_end_port_op(saved_vproc, "sbsd_side_thread_server");
					/* seqno counting for *proc* port */
#endif	/* TNC */
	VPROC_RELEASE(saved_vproc, "sbsd_side_thread_server");
}


/*
 * Used to simulate a signal check at schedule time.
 */
void
side_issig_psig(
	struct proc	*p,
	int		dummyarg)
{
	register struct uthread *uth = &u;
	int error;
#if MACH_ASSERT
	int sanitycount = 0;
#endif

	SIG_DEBUG(1, ("side_issig_psig[%d]: enter p_cursig=%d p_sig=0x%08x " \
			"p_flag=0x%x\n", p->p_pid, p->p_cursig, p->p_sig, \
			p->p_flag));

	ASSERT(syscall_on_master());	/* master lock must be held */

	if (pproc_hold(p, p->p_pid) != ESUCCESS)
		goto out;

	do {
		if (uth->u_procp == NULL || (p->p_flag&SWEXIT) != 0) {
			SIG_DEBUG(2, ("side_issig_psig[%d]: proc exiting\n", \
					p->p_pid));
			break;		/* get out if proc now exiting */
		}
		if (p->p_cursig == 0) {
			SIG_DEBUG(2, ("side_issig_psig[%d]: call issig().\n", \
					p->p_pid));
		}
		if (p->p_cursig != 0 || issig()) {
			SIG_DEBUG(2, ("side_issig_psig[%d]: calling psig() " \
					"p_cursig=%d p_sig=0x%08x fl=0x%x\n", \
					p->p_pid, p->p_cursig, p->p_sig, \
					p->p_flag));

			psig1(TRUE);	/* deliver p_cursig to proc */

			SIG_DEBUG(2, ("side_issig_psig[%d]: back from psig() " \
					"p_cursig=%d, p_sig=0x%08x " \
					"p_flag=0x%x\n", p->p_pid, \
					p->p_cursig, p->p_sig, p->p_flag));
		}

		if (p->p_cursig == 0) {
			SIG_DEBUG(2, ("side_issig_psig[%d]: now p_cursig=0\n", \
					p->p_pid));
			break;
		}

		SIG_DEBUG(2, ("side_issig_psig[%d]: signal not delivered. " \
				"retry in 1 second. p_cursig=%d p_sig=0x%08x " \
				"p_flag=0x%x\n", p->p_pid, p->p_cursig, \
				p->p_sig, p->p_flag));
		unix_release();
		(void) mpsleep(&lbolt, PSLEP, "side_issig_psig", 0, 0, 0); 
		unix_master();
		SIG_DEBUG(1, ("side_issig_psig[%d]: 1 more time.\n", p->p_pid));
#if MACH_ASSERT
		++sanitycount;
		if (sanitycount > 5 &&
				(sanitycount < 16 || (sanitycount&63) == 0)
		) {
			printf("side_issig_psig[%d]: sanitycount=%d trying"
					"to deliver signal %d p_flag=0x%x\n",
					p->p_pid, sanitycount, p->p_cursig,
					p->p_flag);
					
		}
#endif

	} while (TRUE);

	pproc_release(p, p->p_pid);

out:
	if (uth->uu_procp != NULL) {
		ASSERT(p->p_issig_flag == 1);
		p->p_issig_flag = 0;
	}

	SIG_DEBUG(1, ("side_issig_psig[%d]: returning 0.\n", p->p_pid));
	return;
}


/*
 * Wake up all server threads in a process.
 *
 *	(interruptible_only == TRUE) means wake up the thread only if it
 *				     is interruptible.
 */
unsleep_proc(
	register struct proc	*p,
	boolean_t		interruptible_only)
{
	register uthread_t	uth;
	int caller_pid = (u.u_procp ? u.u_procp->p_pid : 0);

	ASSERT(p != NULL);
	SIG_DEBUG(1, ("unsleep_proc[%d]: entry dest_pid=%d int_only=%d.\n", \
			caller_pid, p->p_pid, interruptible_only));
	simple_lock(&p->p_lock);
	for (uth = (uthread_t)queue_first(&p->p_servers);
			!queue_end(&p->p_servers, (queue_entry_t)uth);
			uth = (uthread_t)queue_next(&uth->uu_server_list)) {
		clear_wait(uth, THREAD_INTERRUPTED, interruptible_only);
		SIG_DEBUG(3, ("unsleep_proc[%d]: clear_wait(uth=0x%x).\n", \
				caller_pid, uth));
        }
	simple_unlock(&p->p_lock);
	SIG_DEBUG(1, ("unsleep_proc[%d]: exiting.\n", caller_pid));
}

/*
 * Stop a process.
 */
bsd_stop(p, thread, sig, code)
	register struct proc *p;
	thread_t	thread;
	int		sig, code;
{
	int cur_pid = (u.u_procp ? u.u_procp->p_pid : 0);

	SIG_DEBUG(1, ("bsd_stop[%d]: entering with p=%d.\n", cur_pid,p->p_pid));
	p->p_stat = SSTOP;
	p->p_flag &= ~SWTED;
	p->p_stopsig = sig;
	(void) unix_task_suspend(p);

	if (thread != MACH_PORT_NULL)
	    p->p_thread = thread;
	SIG_DEBUG(2, ("bsd_stop[%d]: send SIGCHLD to parent=%d.\n", u.u_procp->p_pid, p->p_ppid));
	(void) VPOP_REPORT_STATE( p->p_vproc, VPROC_STOP); 
	SIG_DEBUG(1, ("bsd_stop[%d]: stopped p=%d and woke up pp=%d. return.\n", u.u_procp->p_pid, p->p_pid, p->p_ppid));
#ifdef NX
	(void)nx_signal_tam(p->p_vproc);
#endif /* NX */
}

/*
 * Take default action on signal.
 */
sig_default(sig)
	register int	sig;
{
	register struct proc *p = u.u_procp;
	boolean_t	dump;

	SIG_DEBUG(1, ("sig_default[%d]: entering with sig=%d.\n", p->p_pid, sig));
	switch (sig) {
	    /*
	     *	The new signal code for multiple threads makes it possible
	     *	for a multi-threaded task to get here (a thread that didn`t
	     *	originally process a "stop" signal notices that cursig is
	     *	set), therefore, we must handle this.
	     */
	    case SIGIO:
	    case SIGURG:
	    case SIGCHLD:
	    case SIGCONT:
	    case SIGWINCH:
	    case SIGTSTP:
	    case SIGTTIN:
	    case SIGTTOU:
	    case SIGSTOP:
#ifdef NX
            case SIGUSR2:

            if (sig == SIGUSR2 && !nx_application(p)) {
                dump = FALSE;
                break;
            }
#endif /* NX */
		SIG_DEBUG(2, ("sig_default[%d]: default for %d is SIG_IGN. return.\n", p->p_pid, sig));
		return;

	    case SIGILL:
	    case SIGIOT:
	    case SIGBUS:
	    case SIGQUIT:
	    case SIGTRAP:
	    case SIGEMT:
	    case SIGFPE:
	    case SIGSEGV: 
	    case SIGSYS:
		/*
		 * Just die if SIGKILL is pending
		 */
		if((sig == SIGTRAP) && ( sigmask(SIGKILL) & p->p_sig)) {
			p->p_stopsig = sig;
			p->p_utask.uu_acflag.fi_flag |= AXSIG;	/* exited w/signal */
			proc_exit(p,SIGKILL,0);
			return;
		}
		/*
		 * Kill the process with a core dump.
		 */
		SIG_DEBUG(2, ("sig_default[%d]: default for %d is exit with core.\n", p->p_pid, sig));
		dump = TRUE;
		break;
	
	    default:
		SIG_DEBUG(2, ("sig_default[%d]: default for %d is exit without core.\n", p->p_pid, sig));
		dump = FALSE;
		break;
	}

	p->p_stopsig = sig;
	p->p_utask.uu_acflag.fi_flag |= AXSIG;	/* exited w/signal */
	SIG_DEBUG(1, ("sig_default[%d]: calling proc_exit(p=%d). return.\n", p->p_pid, p->p_pid));
	proc_exit(p, sig, dump);
}

/*
 * Store the emulator addresses for recovering the emulator from a user
 * address exception, so it can return EFAULT to the user:
 */
void
set_emul_uacc(uacc_start, uacc_end, uacc_err)
	int uacc_start, uacc_end, uacc_err;
{
	if (!uacc_start || !uacc_end || !uacc_err || uacc_start > uacc_end) {
		printf("Bad emulator uacc values: %x %x %x\n",
					uacc_start, uacc_end, uacc_err);
		panic("set_emul_uacc");
	}
	SIG_DEBUG(0, ("setting emulator uacc values: %x %x %x\n",
		      uacc_start, uacc_end, uacc_err));
	emul_uacc_start = uacc_start;
	emul_uacc_end = uacc_end;
	emul_uacc_err = uacc_err;
}

#if	BUG_COMPAT
/*
 * This variable allows some backward bug compatibility:
 * set it to TRUE at run-time if you want to allow processes to
 * fo read accesses to page 0.
 * set it to FALSE if you want strict OSF/1 compliance.
 */
boolean_t allow_page_0_access = FALSE;
#endif	/* BUG_COMPAT */

/*
 * New thread_psignal.  Runs as part of the normal service - thus
 * we have a thread per user exception, so it can wait.
 */
thread_psignal(task, thread, sig, code)
	task_t		task;
	thread_t	thread;
	register int	sig;
	int		code;
{
	register struct proc	*p = task_to_proc_lookup(task);
	register uthread_t	uth = &u;
	boolean_t		exception_processed = FALSE;
	boolean_t		valid_pc;
	int			pc = -1;
	thread_state_data_t	thread_state;

	if (task == mach_task_self()) {
#ifdef	TNC
		if (sig == SIGFPE)
			return;
#endif
#if	MAPPED_FILES
		if ((sig == SIGSEGV) && (code == ENOSPC)) {
			/*
			 * ENOSPC for NFS server (really mf_write),
			 * currently the only exception the server is
			 * allowed to take.
			 * Check if the exception came during user_bcopy2,
			 * and if so, reset pc to the error recovery point
			 * and pass back the error code.
			 */
			extern int uacc_start(), uacc_end(), uacc_err();

			valid_pc = (get_thread_state_pc(thread, &pc,
					(thread_state_t)&thread_state)
				    == KERN_SUCCESS);
			if (valid_pc && pc >= (int)uacc_start &&
			    pc <= (int)uacc_end) {
				put_thread_state_pc_err(thread, uacc_err, 
					code, (thread_state_t)&thread_state);
				return;
			}
		}
#endif
		printf("** SERVER EXCEPTION sig=%d code=%d **\n", 
		       sig, code);
		panic("server exception");
		while (1);
	}
	if (p == 0) {
	    printf("Exception from unknown task=%d,thread=%d. Terminating it.\n",
		   task,thread);
	    (void) task_terminate(task);
	    SIG_DEBUG(1, ("thread_psignal[?]: no proc, so task_terminate(). return.\n"));
	    return;
	}
	SIG_DEBUG(1, ("thread_psignal[%d]: entering with p=%d, sig=%d.\n", p->p_pid, p->p_pid, sig));

	if (sig < 0 || sig > NSIG)
	    return;

	valid_pc = (get_thread_state_pc(thread, &pc,
					(thread_state_t)&thread_state)
		    == KERN_SUCCESS);

	/*
	 * Register thread as service thread
	 */
	if (server_thread_register(uth, p) != 0) {
		if (EXITING(p) && (p->p_flag & SINSUSP)) {
			if (valid_pc && pc >= EMULATOR_BASE && 
							pc <= EMULATOR_END) {
				/* Be abrupt; the emulator is hosed */
				(void) task_terminate(p->p_task);

				printf("%s at pc=0x%x in proc %d(%s). Signal=%d, Code=%d.\n",
					"** EXCEPTION while exit/release_state",
					pc, p->p_pid, p->p_utask.uu_comm, sig, code);
				printf("** User task suspended for debug.\n");

				/*
			 	 * If we weren't going to debug, we'd make the
			 	 * uemul_release_state wakeup by either destroying
			 	* the reply port or terminating the task.
			 	*/
			} else
				/* the task is exiting */
				return;
		}

		/* process doesn't accept system calls anymore */
		return;
	}
	uarea_init(uth, p);
	unix_master();

	valid_pc = (get_thread_state_pc(thread, &pc,
					(thread_state_t)&thread_state)
		    == KERN_SUCCESS);

	/*
	 * Check if the exception comes from the emulator and signal it
	 * to the user. Let the offending task suspended so that the
	 * user could debug it. Otherwise proceed with the exception 
	 * signal...
	 * EXC_BREAKPOINT exceptions are not relevant at that point.
	 */
	if (sig != SIGTRAP) {
		if (valid_pc && pc >= EMULATOR_BASE && pc <= EMULATOR_END) {
			/* Check if the emulator got an exception during
			 * access of a user addr.  If so, resume it at
			 * the previously specified error recovery
			 * point, so it can return EFAULT to user.
			 */
			if (pc >= emul_uacc_start && pc <= emul_uacc_end) {
				put_thread_state_pc_err(thread, emul_uacc_err, 
						code,
						(thread_state_t)&thread_state);
				SIG_DEBUG(0, ("thread_psignal[%d]: reset emulator from %x to %x (uacc err).\n", p->p_pid, pc, emul_uacc_err));
			} else {
				/* Be abrupt; the emulator is hosed */
				uprintf(
				   "%s%x in proc %d(%s). Signal=%d, Code=%d.\n",
					"** EMULATOR EXCEPTION at pc=0x", pc,
					p->p_pid, p->p_utask.uu_comm, sig,
					code);

				/*
				 * Check to see if the task should be
				 * suspend in the emulator.  Suspending in the
				 * emulator makes for easier debugging.
				 */
				if (suspend_in_emulator) {
				    (void) task_suspend(p->p_task);
				    uprintf("** User task suspended for debug.\n");
				} else {
				    goto bail_out;
				}
			}
			exception_processed = TRUE;
		}
	}

bail_out:
#if	BUG_COMPAT
	/*
	 * Check if the exception can have been generated by a bad access
	 * to the protected page 0. If so, unprotect this page and try
	 * the access again, to allow some bug compatibility.
	 */
	if (allow_page_0_access == TRUE && 
	    exception_processed == FALSE &&
	    (sig == SIGSEGV || sig == SIGBUS)) {
		vm_address_t address;
		vm_size_t size;
		vm_prot_t protection, max_protection;
		vm_inherit_t inheritance;
		boolean_t shared;
		mach_port_t object_name;
		vm_offset_t offset;

		address = 0;
		if ((vm_region(task, &address, &size, &protection,
			       &max_protection, &inheritance, &shared, 
			       &object_name, &offset) == KERN_SUCCESS) &&
		    address == 0 && 
		    (protection&VM_PROT_READ) == 0 &&
		    (max_protection&VM_PROT_READ) != 0) {
			if (vm_protect(task, address, size, FALSE,
				       protection|VM_PROT_READ) 
			    == KERN_SUCCESS) {
				printf("Unprotected page 0 for process %d(%s) after exception at pc=0x%x%s.\n", p->p_pid, p->p_utask.uu_comm, pc, valid_pc ? "" : "<unknown>");
				exception_processed = TRUE;
			}
		}
	}
#endif	/* BUG_COMPAT */

	if (exception_processed == FALSE)
		thread_signal(p, thread, sig, code);
			
	/*
	 * If thread_signal killed the process, then proc_exit
	 * might have already deregistered us.
	 */
	if (uth->uu_procp == 0) {
		SIG_DEBUG(1, ("thread_psignal[?]: no more proc (exited). return.\n"));
		unix_release();
		uarea_terminate(uth, p);	/* for credentials */
		return;
	}

	/*
	 * We hold a reference on the proc, so we must deal with possibly
	 * pending signals here. Otherwise, a process could be looping on
	 * a masked exception signal and not be killable by the first
	 * KILL signal.
	 */
	if (p->p_cursig != 0 || HAVE_SIGNALS(p)) {
		if (p->p_cursig != 0 || issig()) {
			/* it's OK to force delivery here */
			psig1(TRUE);
		}
		if (uth->uu_procp == 0) {
			SIG_DEBUG(1, ("thread_psignal[?]: no more proc (exited.2). return.\n"));
			unix_release();
			uarea_terminate(uth, p);	/* for credentials */
			return;
		}
	}

	unix_release();
	uarea_terminate(uth, p);
	server_thread_deregister(uth, p);

	if (p->p_flag & SWEXIT) {
		wakeup((caddr_t)&p->p_flag);
		SIG_DEBUG(1, ("thread_psignal[%d]: woke &p->p_flag (=0x%x)\n", \
			p->p_pid, &p->p_flag));
	}

	SIG_DEBUG(1, ("thread_psignal[%d]: return.\n", p->p_pid));
}

thread_signal(p, thread, sig, code)
	register struct proc *p;
	thread_t	thread;
	register int	sig;
	int		code;
{
	register int	mask;

	SIG_DEBUG(1, ("thread_signal[%d]: entering with p=%d, sig=%d.\n", p->p_pid, p->p_pid, sig));
	mask = sigmask(sig);
	for (;;) {
	    if (p->p_sigmask & mask) {
		/*
		 * Save the signal in p_sig.
		 */
		p->p_sig |= mask;
		SIG_DEBUG(1, ("thread_signal[%d]: sig %d is masked. return.\n", p->p_pid, sig));
		return;
	    }

	    while (p->p_stat == SSTOP || (p->p_flag & SWEXIT)) {
		if (p->p_flag & SWEXIT) {
			SIG_DEBUG(1, ("thread_signal[%d]: proc is exiting. return.\n", p->p_pid));
		    return;
		}
		SIG_DEBUG(3, ("thread_signal[%d]: proc is stopped. sleeping.\n", p->p_pid));
		sleep((caddr_t)&p->p_stat, PSPECL);
		SIG_DEBUG(3, ("thread_signal[%d]: woken up.\n", p->p_pid));
	    }
	    if (p->p_sigmask & mask) {
		/* could have changed */
		continue;
	    }

	    if (p->p_flag & STRC) {
		/*
		 * Stop for trace.
		 */
		bsd_stop(p, thread, sig, code);
		SIG_DEBUG(3, ("thread_signal[%d]: stopped for trace. sleeping.\n", p->p_pid));
		sleep((caddr_t)&p->p_stat, PSPECL);
		SIG_DEBUG(3, ("thread_signal[%d]: woken up.\n", p->p_pid));

		if (p->p_flag & SWEXIT) {
			SIG_DEBUG(1, ("thread_signal[%d]: proc is exiting. return.\n", p->p_pid));
		    return;
		}

		if ((p->p_flag & STRC) == 0) {
			SIG_DEBUG(3, ("thread_signal[%d]: proc is no more traced.\n", p->p_pid));
		    continue;
		}

		sig = p->p_cursig;
		if (sig == 0) {
			SIG_DEBUG(1, ("thread_signal[%d]: debugger has discarded the signal. return.\n", p->p_pid));
		    return;
		}

		mask = sigmask(sig);
		if (p->p_sigmask & mask) {
		    SIG_DEBUG(2, ("thread_signal[%d]: signal %d is masked.\n", p->p_pid, sig));
		    continue;
		}
	    }
	    break;
	}

	SIG_DEBUG(2, ("thread_signal[%d]: got signal %d.\n", p->p_pid, sig));
	switch ((int)u.u_signal[sig]) {
	    case (int)SIG_IGN:
	    case (int)SIG_HOLD:
		/*
		 * Should not get here unless traced.
		 */
		SIG_DEBUG(2, ("thread_signal[%d]: action for %d is SIG_IGN or SIG_HOLD.\n", p->p_sig, sig));
		break;

	    case (int)SIG_DFL:
		SIG_DEBUG(2, ("thread_signal[%d]: action for %d is SIG_DFL.\n", p->p_pid, sig));
#ifdef PARACORE
		u.u_code = code;
		p->p_utask.uu_faulting_thread = thread;
#endif PARACORE
		sig_default(sig);
		break;

	    default:
		/*
		 * Send signal to user thread.
		 */
		SIG_DEBUG(2, ("thread_signal[%d]: action for %d is SIG_CATCH.\n", p->p_pid, sig));
		send_signal(p, thread, sig, code);
		break;
	}
	SIG_DEBUG(1, ("thread_signal[%d]: sig=%d processed. return.\n", p->p_pid, sig));
}

/*
 * Unblock thread signal if it was masked off.
 */
check_proc_signals(p)
	register struct proc *p;
{
	wakeup((caddr_t)&p->p_sigmask);
	SIG_DEBUG(2, ("check_proc_signals[%d]: woke up p->p_sigmask. return.\n", p->p_pid));
}

/*
 * Send the signal to a thread.  The thread must not be executing
 * any kernel calls.
 */
send_signal(p, thread, sig, code)
	register struct proc *p;
	thread_t	thread;
	register int	sig;
	int		code;
{
	register int	mask;
	int	returnmask;
	sig_t	action;

	SIG_DEBUG(1, ("send_signal[%d]: entering with sig=%d.\n", p->p_pid, sig));
	action = p->p_utask.uu_signal[sig];
	mask = sigmask(sig);

	/*
	 * At this point, thread is in either exception_raise,
	 * or emulator::take_signal, waiting for a reply message.
	 * There should be no need to suspend it.
	 */

	/*
	 * Signal action may have changed while we blocked for
	 * task_suspend or thread_abort.  Check it again.
	 */
	sig_lock_simple(p);

	if ((p->p_sigcatch & mask) == 0) {
	    /*
	     * No longer catching signal.  Put it back if
	     * not ignoring it, and start up the task again.
	     */
	    sig_unlock(p);
	    SIG_DEBUG(2, ("send_signal[%d]: sig %d masked. resend it and resume.\n", p->p_pid, sig));
	    psignal(p, sig);
	    (void) task_resume(p->p_task);
	    SIG_DEBUG(1, ("send_signal[%d]: sig=%d resent. return.\n", p->p_pid, sig));
	    return;
	}
	if (p->p_flag & SOUSIG) {
	    if (sig != SIGILL && sig != SIGTRAP) {
		p->p_utask.uu_signal[sig] = SIG_DFL;
		p->p_sigcatch &= ~mask;
		SIG_DEBUG(3, ("send_signal[%d]: sig=%d was set by signal(). reset to SIG_DFL.\n", p->p_pid, sig));
	    }
	    mask = 0;
	}
	if (p->p_flag & SOMASK) {
	    returnmask = p->p_utask.uu_oldmask;
	    p->p_flag &= ~SOMASK;
	    SIG_DEBUG(3, ("send_signal[%d]: returnmask from before sigsuspend.\n", p->p_pid));
	}
	else {
	    returnmask = p->p_sigmask;
	    SIG_DEBUG(3, ("send_signal[%d]: returnmask is current mask.\n", p->p_pid));
        }
	p->p_sigmask |= p->p_utask.uu_sigmask[sig] | mask;
	sig_unlock(p);

	SIG_DEBUG(0, ("send_signal[%d]: calling sendsig(p=0x%x, thread=0x%x, action=0x%x, sig=%d, code=%d, returnmask=0x%08x).\n", p->p_pid, p, thread, action, sig, 0, returnmask));
	sendsig(p, thread, action, sig, code, returnmask);
	SIG_DEBUG(2, ("send_signal[%d]: back from sendsig(sig=%d).\n", p->p_pid, sig));

	/*
	 * At this point, thread is in either exception_raise,
	 * or emulator::take_signal, waiting for a reply message.
	 * The reply message should resume it.
	 */
	SIG_DEBUG(1, ("send_signal[%d]: return after sendsig().\n", p->p_pid));
}

/*
 * Called by user to take a pending signal.
 */
int
bsd_take_signal(proc_port, interrupt, old_mask, old_onstack, o_sig, o_code,
		o_handler, new_sp)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	int		*old_mask;	/* out */
	int		*old_onstack;	/* out */
	int		*o_sig;		/* out */
	int		*o_code;	/* out */
	int		*o_handler;	/* out */
	int		*new_sp;	/* out */
{
	register struct proc *	p;
	register int	error;
	register int	sig, mask;
	register sig_t	action;
	int		returnmask;
	int		oonstack;

	error = start_server_op(proc_port, 1000);
		/* XXX code for take-signal */
	if (error)
	    return (error);

	unix_master();

	p = u.u_procp;
	SIG_DEBUG(1, ("bsd_take_signal[%d]: entering. p_cursig=%d, p_sig=0x%08x\n", p->p_pid, p->p_cursig, p->p_sig));
	/*
	 * Process should be running.
	 * Should not get here if process is stopped.
	 */
	if (p->p_stat != SRUN || (p->p_flag & SWEXIT)) {
		SIG_DEBUG(1, ("bsd_take_signal[%d]: sig=%d, not SRUN (%d)\n", p->p_pid, o_sig, p->p_stat));
	    unix_release();
	    return (end_server_op(ESRCH, interrupt));
	}

	/*
	 * Set up return values in case no signals pending.
	 */
	*old_mask = 0;
	*old_onstack = 0;
	*o_sig = 0;
	*o_code = 0;
	*o_handler = 0;
	*new_sp = 0;

	/*
	 * Get pending signal.
	 */
	if (p->p_cursig || issig()) {
		/*
		 * issig() will already have handled SIG_DFL actions
		 * if necessary.
		/* psig() might have something to do (?) */
		psig();
	}

	if (p->p_cursig == 0) {
	    /*
	     * No signals - return to user.
	     */
	    unix_release();
	    SIG_DEBUG(1, ("bsd_take_signal[%d]: return with no signal. p_cursig=%d, p_sig=0x%08x.\n", p->p_pid, p->p_cursig, p->p_sig));
	    return (end_server_op(0, interrupt));
	}

	sig = p->p_cursig;
	p->p_cursig = 0;
	action = p->p_utask.uu_signal[sig];
	switch ((int)action) {
	    case (int)SIG_IGN:
		/*
		 * Should not get here.
		 */
		SIG_DEBUG(2, ("bsd_take_signal[%d]: sig %d is SIG_IGN'ed.!?.\n", p->p_pid, sig));
		sig = 0;
		break;

	    case (int)SIG_HOLD:
		/*
		 * Should not get here.
		 */
		SIG_DEBUG(2, ("bsd_take_signal[%d]: sig %d is SIG_HOLD'ed.!?.\n", p->p_pid, sig));
		sig = 0;
		break;

	    case (int)SIG_DFL:
		/*
		 * Should not get here
		 */
		sig_lock_simple(p);
		if (sigmask(sig) & stopsigmask) {
#ifdef NX
		        if (!nx_application(p) && sig == SIGUSR2) {
			    sig_unlock(p);
			    sig = 0;
			    break;
			} else if (nx_application(p) && sig == SIGUSR2) {
			    p->p_cursig =  SIGUSR2;
			}
#endif /* NX */
			stop(p);
			p->p_stopsig = sig;
		} else {
			SIG_DEBUG(2, ("bsd_take_signal[%d]: sig %d is SIG_DFL'ed.!?.\n", p->p_pid, sig));
		}
		sig_unlock(p);
		sig = 0;
		break;

	    default:
		/*
		 * user gets signal
		 */
		SIG_DEBUG(2, ("bsd_take_signal[%d]: action for sig %d is SIG_CATCH.\n", p->p_pid, sig));
		mask = sigmask(sig);

		sig_lock_simple(p);

		if ((p->p_sigcatch & mask) == 0) {
		     sig_unlock(p);
		     SIG_DEBUG(2, ("bsd_take_signal[%d]: sig %d is masked. respost it.\n", p->p_pid, sig));
		    psignal(p, sig);
		    sig = 0;
		    break;
		}
		if (p->p_flag & SOUSIG) {
		    if (sig != SIGILL && sig != SIGTRAP) {
			p->p_utask.uu_signal[sig] = SIG_DFL;
			p->p_sigcatch &= ~mask;
			SIG_DEBUG(3, ("bsd_take_signal[%d]: sig %d was set by signal(). reset it to SIG_DFL.\n", p->p_pid, sig));
		    }
		    mask = 0;
		}
		if (p->p_flag & SOMASK) {
		    returnmask = p->p_utask.uu_oldmask;
		    p->p_flag &= ~SOMASK;
		    SIG_DEBUG(3, ("bsd_take_signal[%d]: use the saved returnmask from sigsuspend.\n", p->p_pid));
		}
		else {
		    returnmask = p->p_sigmask;
		    SIG_DEBUG(3, ("bsd_take_signal[%d]: use the current mask as returnmask.\n", p->p_pid));
	        }
		p->p_sigmask |= p->p_utask.uu_sigmask[sig] | mask;
		SIG_DEBUG(3, ("bsd_take_signal[%d]: set mask to 0x%08x.\n", p->p_pid, p->p_sigmask));
		sig_unlock(p);
#if	MAP_UAREA
		share_lock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
		oonstack = p->p_utask.u_sigstack.ss_onstack;
		if (!oonstack && (p->p_utask.uu_sigonstack & mask)) {
		    *new_sp = (int) p->p_utask.u_sigstack.ss_sp;
		    p->p_utask.u_sigstack.ss_onstack = 1;
		}
		else
		    *new_sp = 0;	/* use existing stack */

#if	MAP_UAREA
		share_unlock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
		*old_mask = returnmask;
		*old_onstack = oonstack;
		*o_sig = sig;
#ifdef	TNC
		if (sig == SIGMIGRATE) {
			*o_code = p->p_sig_arg;
			p->p_sig_arg = 0;
		} else
#endif	/* TNC */
			*o_code = 0;
		*o_handler = (int)action;
		SIG_DEBUG(0, ("bsd_take_signal[%d]: sig=%d, code=%d, handler=0x%x, returnmask=0x%08x, onstack=%d, new_sp=0x%x.\n", p->p_pid, *o_sig, *o_code, *o_handler, *old_mask, *old_onstack, *new_sp));
		break;
	}

	unix_release();
	SIG_DEBUG(1, ("bsd_take_signal[%d]: return sig=%d, code=%d, handler=0x%x, returnmask=0x%08x, onstack=%d, new_sp=0x%x.\n", p->p_pid, *o_sig, *o_code, *o_handler, *old_mask, *old_onstack, *new_sp));
	return (end_server_op(0, interrupt));
}

int
bsd_sigreturn(proc_port, interrupt, old_on_stack, old_sigmask)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		old_on_stack;
	int		old_sigmask;
{
	register int error;
	register struct proc *p;

	error = start_server_op(proc_port, SYS_sigreturn);
	if (error)
	    return (error);

	unix_master();

	p = u.u_procp;
	SIG_DEBUG(1, ("bsd_sigreturn[%d](old_on_stack=%d, old_sigmask=%d): onstack=%d, sigmask=0x%08x.\n", p->p_pid, old_on_stack, old_sigmask, p->p_utask.u_sigstack.ss_onstack, p->p_sigmask));

#if	MAP_UAREA
	share_lock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA
	p->p_utask.u_sigstack.ss_onstack = old_on_stack & 01;
#if	MAP_UAREA
	share_unlock(&p->p_shared_rw->us_lock, p);
#endif	MAP_UAREA

	sig_lock_simple(p);
	p->p_sigmask = old_sigmask & ~cantmask;
	sig_unlock(p);

	check_proc_signals(p);

	unix_release();

	SIG_DEBUG(1, ("bsd_sigreturn[%d]: onstack=%d, sigmask=0x%08x. return.\n", p->p_pid, p->p_utask.u_sigstack.ss_onstack, p->p_sigmask));
	return (end_server_op(0, interrupt));
}

proc_exit(p, rv, dump)
	register struct proc	*p;
	int			rv;
	boolean_t		dump;
{
	int cur_pid = (u.u_procp ? u.u_procp->p_pid : 0);

	SIG_DEBUG(1, ("proc_exit[%d]: entering with p=%d, rv=0x%x, dump=%d.\n", cur_pid, p->p_pid, rv, dump));
	if (dump) {
		rv |= WCOREFLAG;
	}
	TNC_unix_release();
	(void) VPOP_EXIT(p->p_vproc, rv);
	TNC_unix_master();
	SIG_DEBUG(1, ("proc_exit[%d]: returning after exit().\n", u.u_procp->p_pid));
}


/*
 * Server side of the UNIX task suspension logic.
 * Use this instead of a direct "task_suspend(p->p_task)" to allow an
 * emulator holding system resources to gracefully release them.
 * Failure to do so may cause deadlock (i.e., when the pager tries to
 * revoke a token from the suspended emulator!)
 *
 * General alg:
 *	// this will not return until the emulator has cleaned itself up
 *	// emulator leaves all threads but callback thread_suspend()d.
 *	uemul_release_state()
 *	task_suspend()
 *	for each thread t in tt
 *		if t != callback thread
 *			thread_resume(t)
 * 	// task is left suspended
 *
 * If the task goes away somewhere in the middle of this, we'll spit some
 * messages out to the console.
 *
 * This really needs to be able to destroy the process on error.
 * However, since we may be called from the bowels of the signal code,
 * that is not easy.  The best solution might be to just task_terminate()
 * the task on error.
 *
 * This should be called with master_mutex() held.  This CANNOT be called
 * with either p_lock or p_siglock held.
 */
#if MACH_ASSERT
boolean_t	uts_debug = 0;		/* set non-zero for UTSDEBUG printfs */
#define UTSDEBUG(s) if (uts_debug) printf(s)
static char ta_errmsg[] = "unix_task_suspend: %s of proc %d(%s) = 0x%x%s\n";
static char th_errmsg[] = "unix_task_suspend: %s(%x) of proc %d(%s) = 0x%x\n";
#else
#define UTSDEBUG(s)
#endif

int
unix_task_suspend(p)
	struct proc *p;
{
	kern_return_t           kr;
	thread_array_t          thread_list;
	mach_msg_type_number_t  thread_count;
	task_basic_info_data_t	taskInfo;
	mach_msg_type_number_t  taskInfoCnt = TASK_BASIC_INFO_COUNT;
	int			i, error = ESUCCESS;
	boolean_t		dead = FALSE,
				exiting = EXITING(p),
				intr = p->p_utask.uu_sigintr &
						sigmask(p->p_stopsig);

	ASSERT(syscall_on_master());

	if (p->p_flag & SINSUSP) {
		UTSDEBUG(("unix_task_suspend: proc %x SINSUSP\n", p));
		/*
		 * There is a unix_task_suspend in progress.
		 * That one cannot complete until we drop master_mutex;
		 * it will then finish up and wake us up.
		 */
		sleep((caddr_t)&p->sigwait, PSPECL);
		return ESUCCESS;
	}

	kr = task_suspend(p->p_task);
	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "task_suspend",
			p->p_pid, p->p_utask.uu_comm, kr,
			(kr == KERN_INVALID_ARGUMENT ||
			 kr == MACH_SEND_INVALID_DEST)
				? " (no task)" : ""));

		/* maybe only kill the process if != KERN_INV_ARG ? */
		error = ESRCH;
		dead = TRUE;
		goto killit;
	}

	/*
	 * Make sure the task is not already suspended
	 */
	kr = task_info(p->p_task, TASK_BASIC_INFO, (task_info_t)&taskInfo,
			&taskInfoCnt);
	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "task_info",
			p->p_pid, p->p_utask.uu_comm, kr,
			(kr == KERN_INVALID_ARGUMENT ||
			 kr == MACH_SEND_INVALID_DEST)
				? " (no task)" : ""));

		/* maybe only kill the process if != KERN_INV_ARG ? */
		error = ESRCH;
		dead = TRUE;
		goto killit;
	}
	if (taskInfo.suspend_count > 1) {
		UTSDEBUG(("unix_task_suspend: proc %x already suspended: %d\n",
			p, taskInfo.suspend_count));
		(void)task_resume(p->p_task);
		return ESUCCESS;
	}

	/* Nothing more to do?  (hint: leave task suspended) */
	if (p->p_callback == MACH_PORT_NULL ||
	    p->p_callback_thread == MACH_PORT_NULL)
		return ESUCCESS;

	/* darn, we need to resume the task to send to the callback thread */
	kr = task_resume(p->p_task);
	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "task_resume",
			p->p_pid, p->p_utask.uu_comm, kr, ""));
		/* maybe only kill the process if != KERN_INV_ARG ? */
		error = ESRCH;
		dead = TRUE;
		goto killit;
	}

	/*
	 * The emulator may resume some of it's threads, but it must
	 * guarantee suspending them again before returning to us.
	 *
	 * We mark SINSUSP while we have master_mutex held.
	 * Since pproc_exit MUST go through this path, the process
	 * cannot disappear on us.
	 */
	p->p_flag |= SINSUSP;

	ux_server_thread_blocking();
	kr = uemul_release_state(p->p_callback, intr, exiting);
	ux_server_thread_unblocking();

	p->p_flag &= ~SINSUSP;

	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "uemul_release_state",
			p->p_pid, p->p_utask.uu_comm, kr, ""));

		error = ESRCH;
		dead = TRUE;
		goto killit;
	}

	kr = task_suspend(p->p_task);
	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "task_suspend",
			p->p_pid, p->p_utask.uu_comm, kr, ""));

		/* maybe only kill the process if != KERN_INV_ARG ? */
		error = ESRCH;
		dead = TRUE;
		goto killit;
	}

	kr = task_threads(p->p_task, &thread_list, &thread_count);
	if (kr != KERN_SUCCESS) {
		UTSDEBUG((ta_errmsg, "task_threads",
			p->p_pid, p->p_utask.uu_comm, kr,
 			(kr == KERN_INVALID_ARGUMENT ||
			 kr == MACH_SEND_INVALID_DEST)
				? " (no task)" : ""));

		/* maybe only kill the process if != KERN_INV_ARG ? */
		error = ESRCH;
		dead = TRUE;
		goto killit;
	}

	if (thread_count < 2) {
		UTSDEBUG((ta_errmsg, "count of threads",
				p->p_pid, p->p_utask.uu_comm, thread_count));

		error = ESRCH;
		dead = TRUE;
		goto dealloc;
	}

	for (i = 0; i < thread_count; i++) {
		if (thread_list[i] != p->p_callback_thread) {
			kr = thread_resume(thread_list[i]);
			if (kr != KERN_SUCCESS) {
				UTSDEBUG((th_errmsg, "thread_resume",
					thread_list[i],
					p->p_pid, p->p_utask.uu_comm, kr));
				/* XXX maybe should kill the process */
			}
		}
	}

	/* LEAVE TASK SUSPENDED! */

	/* But that won't run until we drop master_mutex */
	wakeup((caddr_t)&p->sigwait);

dealloc:
	for (i = 0; i < thread_count; i++) {
		(void) mach_port_deallocate(mach_task_self(), thread_list[i]);
	}
	(void) vm_deallocate(mach_task_self(), (vm_offset_t)thread_list, 
			     (vm_size_t) thread_count * sizeof(thread_list));

	if (!dead)
		return error;

killit:
	/*
	 * If we weren't called from exit, then we must kill this process
	 */
	if (!exiting) {
		printf("unix_task_suspend: error %x for proc %d(%s); killing\n",
			kr, p->p_pid, p->p_utask.uu_comm);
		TNC_unix_release();
		(void) VPOP_EXIT(p->p_vproc, SIGKILL);
		TNC_unix_master();
	}
	return error;
}


int
bsd_unix_task_suspend(proc_port, interrupt, task)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	task_t		task;
{
	register struct proc *p;
	int		ret = ESUCCESS;

	unix_master();

	p = task_to_proc_lookup(task);
	if (p == NULL || p->p_flag & SWEXIT) {
		unix_release();
		ret = ESRCH;
		goto out;
	}
	ASSERT(p->p_task == task);

	ret = unix_task_suspend(p);

	unix_release();

        /* get rid of the send right on task port */
	(void) mach_port_deallocate(mach_task_self(), task);

	*interrupt = FALSE;
out:
#ifdef	TNC
	vproc_end_port_op(port_to_proc_lookup(proc_port)->p_vproc,
			  "bsd_unix_task_suspend");
#endif	/* TNC */
	return ret;
}
