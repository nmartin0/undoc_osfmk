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
 * $Log: kern_sig.c,v $
 * Revision 1.37  1995/02/18  01:20:45  yazz
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
 * Revision 1.36  1995/02/01  21:27:42  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.35  1994/11/18  20:27:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.34  1994/11/09  17:06:40  jlitvin
 * Fix ramdisk build error.
 *
 * Revision 1.33  1994/11/08  20:07:21  yazz
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
 * Revision 1.32  1994/11/03  15:58:56  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: med
 *  Benefit or PTS #: #11459
 *  Testing: corefile EAT
 *  Module(s):
 * 	server/bsd/kern_exit.c
 * 	server/bsd/kern_sig.c
 * 	server/bsd/mach_signal.c
 * 	server/sys/proc.h
 * 	server/tnc/dvp_pvpops.c
 * 	server/tnc/rtask_cli_pproc.c
 * 	server/uxkern/syscall_subr.c
 *
 * Dispense with the side thread that psig1() was using to perform exit
 * processing when a fatal signal was received.  Ensure for psig1()'s callers
 * that exit processing, if triggered, is completed before returning.
 *
 * Revision 1.31  1994/10/25  23:06:29  yazz
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
 * mechanism.  Create a side thread in psig1() to perform the exit processing
 * (that the receipt of some signals requires).  By requesting a side thread
 * to perform this processing, psig1() can avoid dropping the master lock
 * part way through.  This should solve the signalling-versus-exiting race
 * conditions.  Mark a process that is going to be terminated by a side
 * thread with a new proc flag bit to prevent it from running again in the
 * emulator.
 *
 * Revision 1.30  1994/09/13  15:33:29  johannes
 * disabled process memory deallocation in core_write()
 * by the (undefined) option CORE_WRITE_DEALLOCATE
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium (need of the deallocating unclear, no repercussion found)
 *  Benefit or PTS #: 9266
 *  Testing: special test case, corefile EAT
 *  Module(s): svr/server/bsd/kern_sig.c
 * 	    svr/server/paracore/core.c (special core_write() removed)
 *
 * Revision 1.29  1994/08/31  22:46:29  mtm
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
 * Revision 1.27.2.1  1994/08/03  15:49:29  nandy
 * Merged from the main stem
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.28  1994/08/03  15:18:39  nandy
 * Changed pproc_psignal() to allow tam to send SIGCHLD to INIT
 *
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 10047
 *  Testing: test case
 *  Module(s):kern_exit.c
 * 	   tnc/dpvproc.h
 * 	   tnc/dvp_vpops.c
 * 	   nx/nx.c
 *
 * Revision 1.27  1994/06/29  16:50:19  johannes
 * psig1(): enforce core dumping for SIGKILL
 * 	 is used for core dumping of non-faulting processes
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
 * Revision 1.26  1994/04/04  22:07:02  jlitvin
 * Fix very minor typo in signal debugging code.
 *
 * Revision 1.25  1994/03/14  02:00:32  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.24  1994/01/13  17:53:17  jlitvin
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
 * Revision 1.23  1993/11/03  19:00:41  yazz
 * Correct CVS comments of prior checkin.
 *
 * Revision 1.22  1993/11/02  03:00:25  yazz
 * Add physical layer routines pproc_timed_delay() and pproc_signal_received()
 * to be called by the vproc layer.
 *
 * Revision 1.21  1993/08/17  16:53:34  nandy
 * p_cursig is set to 0 AFTER calling stop(). This is to ensure that nx_no_signal() would
 * work correctly.
 *
 * Revision 1.20  1993/07/14  17:47:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:47:12  cfj
 * Adding new code from vendor
 *
 * Revision 1.19  1993/05/06  19:03:37  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.18  1993/04/03  03:03:59  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.1.1  1993/05/03  17:24:00  cfj
 * Initial 1.0.3 code drop
 * Revision 1.17  1993/04/03  01:23:00  cfj
 * Merge with T9.
 *
 * Revision 1.13.2.4  1993/04/03  01:14:38  cfj
 * Fix bug #4739 and turn off signal debug.
 *
 * Revision 1.16  1993/03/08  18:34:22  nandy
 * Merged from T9 branch.
 *
 * Revision 1.13.2.3  1993/03/06  23:32:57  nandy
 * Changes from OSF for new locking mechanism.
 *
 * Revision 1.13.2.2  1993/03/01  23:59:24  nandy
 * If a process is marked  exiting, issig() doesn't look for more signals
 * after getting one.
 *
 * Revision 1.13.2.1  1993/02/25  01:31:33  nandy
 * core() was calling remote_vnrdwr() with incorrect # of arguments.
 *
 * Revision 1.1.2.1.2.4  1993/02/16  20:02:51  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.13  1993/02/03  22:14:05  nandy
 * This time #ifdefs are in the right place.
 *
 * Revision 1.12  1993/02/02  17:49:15  nandy
 * Added necessary #ifdef NX around nx_gang_state()
 *
 * Revision 1.11  1993/01/28  19:25:15  nandy
 * Fixed psignal3() to make ctrl-Z to work on a gang stopped application.
 *
 * Revision 1.10  1993/01/28  02:33:05  nandy
 * Mask SIGUSR2 on getting SIGCONT and vice-versa.
 *
 * Revision 1.9  1993/01/22  18:20:51  nandy
 * Corrected the last check-in
 *
 * Revision 1.7  1993/01/15  23:35:11  cfj
 * Bug fix from Locus to fix signals to stopped processes.
 *
 *   Revision 2.39  93/01/15  10:05:14  chrisp
 *   Ensure that the process signal lock is not held across calls to
 * 	  VPOP_REPORT_STATE() since this may drop and re-acquire the master
 * 	  lock when performing a remote operation - this violate the locking
 * 	  hierarchy.
 * 
 *   Revision 2.38  93/01/08  11:01:02  chrisp
 *   In psignal3(), when a signal should stop a process, goto run allowing the
 * 	  signal to be seen and handled in psig() running on behalf of
 * 	  the signaled process.
 * 
 * Revision 1.6  1993/01/14  23:42:40  nandy
 * Set p_cursig to SIGUSR2 so nx_no_signal() can detect gang stop signal.
 *
 * Set p_stopsig after a traced process is stopped.
 *
 * Revision 1.1.2.1.2.3  1993/01/09  00:03:28  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.5  1992/12/31  17:25:26  nandy
 * Remove unnecessary sig_unlock() in psig1 for SIGUSR2 and non-nx-applications.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  05:58:32  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/15  00:08:28  cfj
 * 	Revision 2.37  92/12/11  15:30:31  chrisp
 * 	Before invoking VPOP_EXIT() [and releasing master lock in the TNC case],
 * 	     mark proc as exiting in issig() and psig1(). This avoids
 * 	     deadlock with issig_psig thread.
 *
 * Revision 1.1.2.1.2.1  1992/12/14  23:08:01  brad
 * Merged tip of old NX branch with PFS branch.
 * 
 * Revision 1.3  1992/12/11  02:54:29  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:15:54  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/24  21:31:10  cfj
 * Turn off SIG_DEBUG.
 *
 * Revision 1.1.2.1  1992/11/06  00:06:13  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.37  1992/11/02  21:48:41  cfj
 * Final integration and testing of IPD modifications
 *
 * Revision 2.36  1992/10/17  18:25:15  cfj
 * IPD & Allocator Support.
 *
 * Revision 2.36  92/11/23  16:00:57  klh
 * 	Revision 2.33  92/11/17  19:47:25  loverso
 * 		Remove "#if 0" & code from issig(); it is done in stop().
 * 		In stop(): Set process state first & release siglock to avoid
 * 		deadlock with issig_psig thread.
 * 		In issig(): when STRC, call stop(), instead of rolling our own.
 * 		In psig1(): don't call core; add WCOREFLAG and let exit do it.
 * 		Never call stop() except from psig1().
 * 		(loverso)
 * 
 * 		Revision 3.15  92/04/08  20:45:21  barbou
 * 		Made some SIG_DEBUG code conditional.
 * 
 * Revision 2.43  93/06/16  13:50:01  klh
 * 	Revision 2.38  93/05/16  21:03:37  loverso
 * 		Pass uemul_forward_signal() an indication of EINTR vs ERESTART.
 * 		Set p_stopsig before stopping task.
 * 
 * Revision 2.42  93/04/29  13:58:47  klh
 * 	Revision 2.37  93/03/22  23:57:53  condict
 * 		Changed cthread_yield to thread_yield.  (See kern/sched_prim.c)
 *
 * 	Revision 2.36  93/02/25  12:08:08  loverso
 * 		Fix wrong number of args in remote_vnrdwr() calls which results in a
 * 		trashed p_rcred. (Nandini)
 *
 * Revision 2.41  93/03/22  21:08:07  yazz
 * OSF lock changes.  Cthread_yield() calls become thread_yield().
 * 
 * Revision 2.40  93/02/23  12:16:33  chrisp
 * Remove unwanted ucred parameters in calls to remote_vnrdwr(). Their
 * 	presence causes (among other things) VSX tests to fail on
 * 	servers built without MACH_ASSERT.
 * 
 * Revision 2.39  93/01/15  10:05:14  chrisp
 * Ensure that the process signal lock is not held across calls to
 * 	VPOP_REPORT_STATE() since this may drop and re-acquire the master
 * 	lock when performing a remote operation - this violate the locking
 * 	hierarchy.
 * 
 * Revision 2.38  93/01/08  11:01:02  chrisp
 * In psignal3(), when a signal should stop a process, goto run allowing the
 * 	signal to be seen and handled in psig() running on behalf of
 * 	the signaled process.
 * 
 * Revision 2.37  92/12/11  15:30:31  chrisp
 * Before invoking VPOP_EXIT() [and releasing master lock in the TNC case],
 * 	mark proc as exiting in issig() and psig1(). This avoids
 * 	deadlock with issig_psig thread.
 * 
 * Revision 2.36  92/11/23  16:00:57  klh
 * 	Revision 2.33  92/11/17  19:47:25  loverso
 * 		Remove "#if 0" & code from issig(); it is done in stop().
 * 		In stop(): Set process state first & release siglock to avoid
 * 		deadlock with issig_psig thread.
 * 		In issig(): when STRC, call stop(), instead of rolling our own.
 * 		In psig1(): don't call core; add WCOREFLAG and let exit do it.
 * 		Never call stop() except from psig1().
 * 		(loverso)
 * 
 * 		Revision 3.15  92/04/08  20:45:21  barbou
 * 		Made some SIG_DEBUG code conditional.
 * 
 * Revision 2.35  92/10/06  12:09:58  roman
 * Fix RCS comments.
 * 
 * Revision 2.34  92/10/05  13:58:19  klh
 * 	Revision 2.31  92/09/24  16:49:45  rabii
 * 		Avoid deallocating emulator space too soon in core_write(). 
 *		(dwm #388)
 * 
 * 	Revision 2.30  92/09/17  13:41:57  rabii
 * 		[1992/09/10  18:17:24  cfj]
 * 		Fixed a bug where a process under debug receives a signal 
 *		which is ignored, the signal number is not saved and the 
 *		debugger cannot determine what signal caused the process to 
 *		stop.  In the case of ignored signals, we now save the signal 
 *		number ib p->p_stopsig for processes which are traced.  
 *		pproc_reap() will then return that signal number as the
 * 		cause for the process being stopped.
 * 
 * 	Revision 2.29  92/08/26  12:10:07  loverso
 * 		Replace task_suspend with unix_task_suspend.  Don't deliver 
 *		signal to callback thread, if it somehow ends up as the first 
 *		thread reported by task_threads.  Enable most ptrace code.  
 *		Fix lint. (loverso)
 * 
 * Revision 2.33  92/10/01  10:19:35  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.32  92/09/28  16:32:43  roman
 * Change pproc_killall() to be the more general physical process system
 * 	operation (ppsop) pps_sigprocset() to allow the signalling of
 * 	all processes in a process set on the current node.
 * Change unix_master() to TNC_unix_master() where necessary for more
 * 	efficient non-TNC operation.
 * Change VPOP_SET_STATE() to VPOP_REPORT_STATE() (more modern name).
 * 
 * Revision 2.31  92/08/06  13:35:02  klh
 * 	Revision 2.27  92/07/16  09:44:09  rabii
 * 		Fixed core() for OSF/1 AD (pjg).
 * 
 * Revision 2.30  92/07/07  13:08:08  klh
 * 	Revision 2.26  92/06/30  22:45:48  loverso
 * 		Added NULL ruid and rgid parameters to credentials_set_state(). 
 * 		(rabii)
 * 
 * Revision 2.29  92/07/06  09:01:01  chrisp
 * To avoid races, update run state in proc. structure before invoking
 * 	VPOP_SET_STATE.
 * 
 * Revision 2.28  92/06/26  13:41:54  chrisp
 * For TNC, release the master lock before invoking VPOP_EXIT() in psig1().
 * 
 * Revision 2.27  92/06/05  13:55:09  klh
 * 	Revision 2.25  92/05/31  18:58:06  loverso
 * 		Remove duplicated include of parallel.h (pjg).
 * 
 * 		Revision 3.17  92/04/29  16:51:19  barbou
 * 		Fix for bug #140: release the signals lock in psignal3() when 
 *		calling itself recursively.
 * 
 * 	Revision 2.24  92/05/24  14:19:51  pjg
 * 		Take out debugging declaration.
 * 
 * 	Revision 2.23  92/05/24  14:14:13  pjg
 * 		92/03/24  21:03:09  barbou
 * 		Fixed a few null pointers in the SIG_DEBUG trace.
 * 		Fix for bug #122: put p_cursig back into p_sig if no more 
 *		processable by psig().
 * 		Fix for bug #110: release master_lock before accessing user 
 *		memory when core dumping. There should be something to prevent 
 *		another thread to decide to core dump the same process 
 *		meanwhile...
 * 		[92/05/19            srl]
 * 
 * Revision 2.26  92/04/29  14:50:44  chrisp
 * For TNC, sigsuspend() has been made restartable when awoken by SIGMIGRATE
 * 	being caught by the default handler in the emulator.
 * 
 * Revision 2.25  92/04/21  12:05:01  chrisp
 * Deleted call to sig_unlock() in psignal3() when a signal is sent
 * 	to an exiting process.
 * 
 * Revision 2.24  92/04/06  15:52:17  chrisp
 * Add master locking inside pproc_killall(), pproc_psignal() and proc locking
 * inside pproc_get_attr() and pproc_set_attr().
 * 
 * Revision 2.23  92/03/31  08:48:42  chrisp
 * Parameter order corrected in cansignal() call from pproc_psignal() - sig
 * is the last parameter and not the first.
 * 
 * Revision 2.22  92/03/27  17:34:14  roman
 * Add extra (null) task pointer parameter to credentials_set_state().
 * 
 * Revision 2.21  92/03/27  10:59:37  roman
 * Re-instate panic if psignal3 is called with can_block TRUE and master lock
 * 	not held.
 * Eliminate VPOP_SIGPROC in favor of psignal() when signaling oneself in
 * 	the previously erroneous code handling stopped children of init.
 * 	(The error was an attempted signal of init rather than the child.)
 * 
 * Revision 2.20  92/03/20  11:40:36  pjg
 * 	Check for SWEXIT before creating the retry thread.
 * 	Set p_cursig to 0 if handling a stop (bug #150) (loverso).
 * 
 * Revision 2.19  92/03/15  14:39:07  roy
 * 	Added new credentials interface (rabii)
 * 	Disable routine core() to avoid crashing the server. Temporary, until
 * 	the code is written to handle the DFS (pjg).
 * 
 * Revision 2.18  92/03/09  18:20:32  durriya
 * 	fix typo
 * 
 * Revision 2.17  92/03/09  14:18:50  durriya
 * 	Don't deliver signals to dummy procs.  (loverso)
 * 
 * 	92/02/28  17:59:22  barbou
 * 	Kind of fix for bug #43: give a chance to the suspended process to realize
 * 	that it has been suspended... (not 100% reliable).
 * 	Fix for bug #100: deallocate ports returned by task_threads().
 * 
 * 	91/12/20  17:56:09  barbou
 * 	Refined test for PC in the emulator code.
 * 
 * 	91/12/18  17:14:42  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.16  92/03/01  18:42:43  pjg
 * 	Removed should_force_signal_delivery() and replaced each usage of it
 * 	with appropriate specific code.  It now correctly handles no MAP_UAREA.
 * 	(loverso)
 * 
 * Revision 2.15  92/02/11  22:14:09  pjg
 * 	Change should_force_signal_delivery to allow some emulator 
 * 	passage. Enable call to uemul_forward_signal() to enable 
 * 	interruptable system calls (loverso).
 * 
 * 	Amend issig() to stop a traced process correctly (chrisp@locus).
 * 
 * 	Change pproc_set_attr not to default a NULL proc pointer to 
 * 	u.u_procp. Add ability to kill traced processes in 
 * 	pproc_psignal(). Add attribute has_execd to 
 * 	pproc_get_attr(). Add SIGMIGRATE (with argument passing).
 * 	VPOP_SET_STOP_STATE becomes VPOP_SET_STATE (roman@locus).
 * 
 * Revision 2.14  92/01/17  17:23:41  roy
 * 	Forward signals as appropriate for OSF1_ADFS (loverso).
 * 
 * Revision 2.13  92/01/14  11:11:31  roy
 * 	proc_sigtrylock is defined to simple_try_lock for !MAP_UAREA (sjs).
 * 
 * Revision 2.12  92/01/09  16:29:40  roy
 * 	i860 tweek in mach_core.
 * 
 * Revision 2.11  91/12/18  17:46:32  roy
 * 	Include fcntl.h for core().
 * 
 * Revision 2.10  91/12/18  16:27:17  roy
 * 	91/11/26  15:32:33  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/11/13  14:55:49  barbou
 * 	Added more diagnostics about a potential problem in getting the thread
 * 	list of a task (this already happened, but couldn't be reproduced: 
 * 	this was maybe a problem of this specific application using Mach 
 * 	system calls directly).
 * 
 * 	91/10/21  18:47:15  emcmanus
 * 	Include kern/parallel.h for the fake syscall_on_master needed by 
 * 	asserts. Fixed core-dumping on big sparse processes.
 * 
 * 	91/10/09  18:34:45  emcmanus
 * 	Implemented coredump for server.
 * 
 * Revision 2.9  91/12/13  10:06:38  roy
 * 	91/12/12  13:20:45  roy
 * 	In pproc_set_attr, only call modify_credentials if the
 * 	process isn't a zombie.
 * 
 * Revision 2.8  91/12/08  10:56:37  rabii
 * 	Added call top modify_credentials
 * 
 * Revision 2.7  91/11/22  14:54:29  rabii
 * 	Locus drop mereg including the following:
 * 	pproc_chksigmask() added to perform process signal mask checks for the
 * 	Vproc layer.
 * 	pproc_psignal has extra parameter indicating a signal from a stopped
 * 	child which the target (parent) process may be ignoring.
 * 	Handling of terminal access signals (SIGTTIN,SIGTTOU,SIGTSTP) for
 * 	orphaned pgrp removed. (chrisp)
 * 	Eliminate panic in psignal3 if master_lock is not held at time of call.
 * 	The master_lock is not held for TNC operations. (roman)
 * 	Merged LCC and new OSF versions. (mbarnett)
 * 
 * Revision 2.6  91/11/13  12:51:01  rabii
 * 	Added case ISG_IGN to i860 for compiler issues
 * 
 * Revision 2.5  91/10/14  11:53:31  sjs
 * 	    Revision 3.6  91/09/17  11:50:00  barbou
 * 	    Added missing synchonization between sbsd_issig_psig() and exit().
 * 
 * Revision 2.7  91/10/30  12:43:17  roman
 * Allow LOCATE_VPROC_PID() to return a null pointer, which must be
 * checked for and recovered from.
 * 
 * Revision 2.6  91/10/25  10:38:14  roman
 * Change pproc_getattr() to not return pflag, which is non-portable
 * and unused. Change pproc_set_attr() to allow the setting of the
 * physical process's vproc op.
 * 
 * Revision 2.5  91/10/18  18:27:03  chrisp
 * 	pproc_get_attr() and pproc_set_attr() added.
 * 	pproc_psignal() added; includes privilege checks.
 * 
 * Revision 2.4  91/10/04  14:46:57  chrisp
 * Get rid of extraneous $Log. Change call to exit() to call VPOP_EXIT()
 * instead.
 * 
 * Revision 2.3  91/09/16  15:36:05  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Move system call handling for signals to vproc directory.
 * 
 * Revision 2.2  91/08/31  13:22:07  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.5  91/08/27  15:26:18  barbou
 * Bug fix: SIGKILL should resume a stopped process.
 * 
 * Revision 3.4  91/08/09  12:10:09  barbou
 * Added new code to force the signal delivery if we're not sure that
 * the emulator will ask for pending signals.
 * 
 * Updated to OSF/1.0.2.
 * Condensed relevant OSF/1 server history, reverse chronology:
 * 91/05/15 Original from OSF/1. Using only interfaces.    barbou@gr.osf.org
 * 91/06/07 Added gsignal3() and his friends.              barbou@gr.osf.org
 * 91/06/25 Added interruptible_only param to unsleep_proc condict@gr.osf.org
 * 91/07/04 Now use the OSF/1 internals.                   barbou@gr.osf.org
 * 
 * Revision 1.17.9.4  91/09/20  14:22:46  jvs
 *	Fix for bug #2687, kernel panic when dumping huge core.
 *	[91/09/20  14:22:08  jvs]
 *
 * Revision 1.17.9.3  91/09/18  18:41:49  dwm
 *	Fix SIG_IGN infinite loop hang; bug 3122.
 *	[91/09/18  14:50:58  dwm]
 *
 * Revision 1.17.9.2  91/07/26  14:27:46  dlb
 *	Deadlock fix:  Allow task_dowait to fail when stopping for ptrace.
 *	Check for pending thread halt (e.g., termination) when stopping for
 *	ptrace and recover if one is found.
 *	[91/07/26  14:19:50  dlb]
 *
 * Revision 1.17.6.2  91/05/29  12:24:01  dwm
 * 	Init error variable in sigstack, bug 2133.
 * 	[91/05/29  12:23:19  dwm]
 * 
 * Revision 1.17.4.4  91/03/05  13:39:34  lwa
 * 	Fix bug number 18??, thread_list_lock acquired
 * 	at wrong IPL (must be at splsched()).
 * 	[91/02/28  11:09:47  lwa]
 * 
 * Revision 1.17.4.3  91/01/09  09:00:07  dwm
 * 	Fixed coreprint initialization, bug 1680.
 * 	[91/01/07  20:29:12  dwm]
 * 
 * Revision 1.17.4.2  91/01/07  20:38:28  dwm
 * 	Fixed coreprint initialization, bug 1680.
 * 	[91/01/07  20:29:12  dwm]
 * 
 * Revision 1.17.2.2  90/11/14  13:31:56  jeffc
 * 	Removed Stoopid SWITCH_ANSI conditional.
 * 	[90/11/09  10:08:35  jeffc]
 * 
 * Revision 1.17  90/10/31  13:49:18  devrcs
 * 	Replace all references of NZERO with PRIZERO
 * 	[90/10/25  12:47:09  sp]
 * 
 * 	Removed unnecessary test for ancient compatibility code in ssig().
 * 	[90/10/18  15:13:08  coren]
 * 
 * 	Changed sigaction() to only reject SIGKILL and SIGSTOP if handler is
 * 	not SIG_DFL.
 * 	[90/10/16  09:00:29  coren]
 * 
 * 	Modified 9/21 fix so that caller is excluded if the signal being
 * 	sent is an uncatchable one.
 * 	[90/10/14  19:10:55  ers]
 * 
 * Revision 1.16  90/10/07  13:17:45  devrcs
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:11  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:56:41  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:35  jeffc]
 * 
 * 	Fixed for POSIX conformance: cansignal() now checks saved UID rather
 * 	than effective; kill(-1, SIG) no longer excludes current process.
 * 	[90/09/21  14:38:14  coren]
 * 
 * Revision 1.15  90/09/23  15:43:14  devrcs
 * 	Fixed test for core() success.  Also fixed writing out of kernel
 * 	stack to dump the high addresses, rather than the low if the entire
 * 	kernel stack doesn't fit.  This causes the processor registers
 * 	to be saved in the place that debuggers expect to find them.
 * 	[90/09/14  14:17:26  ers]
 * 
 * 	Fixed for ANSI compliance
 * 	[90/09/05  17:12:19  rossi]
 * 
 * 	fix bad placement of coreprint printf
 * 	[90/09/04  01:36:28  hosking]
 * 
 * Revision 1.14  90/09/13  11:42:02  devrcs
 * 	Osigsetmask must return 0.
 * 	[90/08/27  12:32:10  gmf]
 * 
 * Revision 1.13  90/08/24  11:17:02  devrcs
 * 	changed sleep to tsleep
 * 	[90/08/20  01:34:57  gmf]
 * 
 * 	Changes for new system call interface.
 * 	Removed include of syscontext.h
 * 	use void *, not int * for sig. handlers
 * 	Make osigvec return a value all the time.
 * 	[90/08/17  17:37:30  nags]
 * 
 * 	Use consistent credentials macros.
 * 	[90/08/19  00:03:59  nags]
 * 
 * 	misc. changes to localize security code that's
 * 	OS-specific or that needs locking for MP.
 * 	[90/08/14  14:54:34  hosking]
 * 
 * 	HP/Apollo M68K
 * 	[90/08/13  17:35:38  mcg]
 * 
 * 	Added ssig(), the target of the System V signal() system call.
 * 	[90/08/07  14:46:32  coren]
 * 
 * 	Added ssig(), the target of the System V signal() system call.
 * 	[90/08/07  14:46:32  coren]
 * 
 * Revision 1.12  90/08/09  13:14:01  devrcs
 * 	Fix bug 370: threads are not deallocated after a kill() of itself.
 * 	Caused by unnecessary thread_reference().
 * 	[90/07/30  15:53:33  sp]
 * 
 * Revision 1.11  90/07/27  08:44:07  devrcs
 * 	Removed references to SIGMSG and SIGEMSG, used for Mach IPC hack.
 * 	[90/07/17  12:59:51  coren]
 * 
 * 	Fixed numerous bogus type mismatches in signal code.
 * 	[90/07/13  16:53:57  jeffc]
 * 
 * 	Typo: #ifdef SEC_BASE should be #if SEC_BASE.
 * 	[90/07/11  10:44:54  seiden]
 * 
 * Revision 1.10  90/07/17  11:19:08  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:52:03  seiden]
 * 
 * Revision 1.9  90/07/05  23:07:43  devrcs
 * 	Ensure that the initial thread is the one that executes the signal
 * 	if it is one of the asynchronous variety.
 * 	[90/07/03  11:11:11  sp]
 * 
 * 	Add support for per thread synchronous signals
 * 	[90/06/29  09:09:36  sp]
 * 
 * Revision 1.8  90/06/22  20:06:14  devrcs
 * 	nags merge
 * 	Condensed relevant history, reverse chronology:
 * 	Parallelized for OSF/1				nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing	seiden@osf.org
 * 	Made old syscalls COMPAT			coren@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes	ers@osf.org
 * 	Not quite ready for old syscalls to be COMPAT.	coren@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.		coren@osf.org
 * 	Fixed calls to vn_rdwr to pass the fake_uarea	gmf@osf.org
 * 	MACH X115 Update				gm@osf.org
 * 	Merge Encore parallelization with Mach 2.5	alan@encore.com
 * 	core(): fake uarea on kernel pageable map	af@cmu.edu
 * 	Many psignal and issig buf fixes		dlb@cmu.edu
 * 	fake_uarea fixes				af@cmu.edu
 * 	bug fixes for PMAX and I386			rvb@cmu.edu
 * 	issig() fix to avoid action on system processes	mwyong@cmu.edu
 * 	When core() dumping, must unloc fd, file struct	alan@encore.com
 * 	No more SSLEEP.					dlb@cmu.edu
 * 	Allow exception signals to be used as ipc.  	dlb@cmu.edu
 * 	Call task_suspend_nowait from interrupt level.	dbg@cmu.edu
 * 	issig() and psig() are always called on master 	dbg@cmu.edu
 * 	completely set signal state before sendsig().	dbg@cmu.edu
 * 	[90/06/12  19:06:28  gmf]
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

extern	int sig_debug;		/* mach_signal.c */

#define SIG_DEBUG(level, x)		\
	if (level <= sig_debug) {	\
		bootnode_printf x;	\
	} else 0
#else
#define SIG_DEBUG(level, x)
#endif

#include <machine/reg.h>
#include <machine/vmparam.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/signal_macros.h>
#include <sys/wait.h>
#include <sys/acct.h>
#include <kern/sched_prim.h>
#ifdef OSF1_SERVER
#include <sys/fcntl.h>
#include <kern/kalloc.h>
#include <sys/mount.h>
#include <sys/vnode.h>
#include <sys/synch.h>
#include <kern/parallel.h>
#include <uxkern/proc_to_task.h>
#endif
#ifdef PARACORE
#include <paracore/core_types.h>
#endif /* PARACORE */

#undef stopsigmask
#ifdef NX
extern boolean_t nx_application();

#define stopsigmask     (sigmask(SIGSTOP)|sigmask(SIGTSTP)| \
                        sigmask(SIGTTIN)|sigmask(SIGTTOU) | \
                        sigmask(SIGUSR2))
#else	/* NX */
#define stopsigmask	(sigmask(SIGSTOP)|sigmask(SIGTSTP)| \
			sigmask(SIGTTIN)|sigmask(SIGTTOU))
#endif	/* NX */

#define defaultignmask	(sigmask(SIGCONT)|sigmask(SIGIO)|sigmask(SIGURG)| \
			sigmask(SIGCHLD)|sigmask(SIGWINCH)|sigmask(SIGINFO))

/*
 * Generalized interface signal handler.
 */

#if	MAP_UAREA
#define	proc_sigtrylock(p)	share_try_lock(p, &(p)->p_siglock)
#else	MAP_UAREA
#define	proc_sigtrylock(p)	simple_lock_try(&(p)->p_siglock)
#endif	MAP_UAREA

/*
 * Can local process p send the signal signo to local process q?
 */
#define	CANSIGNAL(p, q, signo)	pproc_cansignal(p, q, signo)

sigaction(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	register struct args {
		int	signo;
		struct	sigaction *nsa;
		struct	sigaction *osa;
#if	defined(balance) || defined(mips) || defined(i860)
		int	(*sigtramp)();		/* signal trampoline code */
#endif /* defined(balance) || defined(mips) || defined(i860) */
#if 	defined(i386)
		int	(*sigreturn)();		/* sigreturn() address */
#endif
	} *uap = (struct args *) args;
	struct sigaction vec;
	register struct sigaction *sa;
	register int sig;
	int bit, error;

	sig = uap->signo;
	ASSERT(syscall_on_master());
#ifdef	multimax
	/*
	 * On the MMax the u area is not visible to the user tasks,
	 * therefore, the task must declare the address of its
	 * trampoline code before it attempts to catch signals.
	 * This is done by using the special signal SIGCATCHALL
	 */
	/* If this is the intermediate signal catcher then set it. */
	if (sig == SIGCATCHALL) {

		/* Copy the uap->nsa structure from user space to system
		 *	space.  Then set the return value and the signal
		 *	catcher.
		 */
		if (error = copyin ((caddr_t)uap->nsa, (caddr_t)&vec,
		    sizeof(vec)))
			return (error);
		*retval = (int)u.u_sigcatch;
		u.u_sigcatch = (void (*)) vec.sa_handler;
		return (0);
	}
#endif
	if (sig <= 0 || sig > NSIG)
		return (EINVAL);
	sa = &vec;
	if (uap->osa) {
		sa->sa_handler = signal_disposition(sig);
		sa->sa_mask = u.u_sigmask[sig];
		bit = sigmask(sig);
		sa->sa_flags = 0;
		if ((u.u_sigonstack & bit) != 0)
			sa->sa_flags |= SA_ONSTACK;
		if ((u.u_sigintr & bit) == 0)
			sa->sa_flags |= SA_RESTART;
		if (p->p_flag & SNOCLDSTOP)
			sa->sa_flags |= SA_NOCLDSTOP;
		if (error = copyout((caddr_t)sa, (caddr_t)uap->osa,
		    sizeof (vec)))
			return (error);
	}
	if (uap->nsa) {
		if (error = copyin((caddr_t)uap->nsa, (caddr_t)sa,
		    sizeof (vec)))
			return (error);
#ifdef NX
                if (sa->sa_handler != SIG_DFL &&
                    (sig == SIGUSR2 && nx_application(p)))
                        return(EINVAL);
#endif /* NX */
		if (sa->sa_handler != SIG_DFL && (sig == SIGKILL || sig == SIGSTOP))
			return(EINVAL);

#if defined(mips) || defined(i860)
		/*
		 * check for unaligned pc on sighandler
		 */
		if (sa->sa_handler != SIG_IGN
		    && ((int)sa->sa_handler & (sizeof(int)-1))) {
			return (EINVAL);
		}
#endif /* defined(mips) || defined(i860) */
#if	defined(balance) || defined(mips) || defined(i860)
		/*
		 * On the Sequent Balance and Mips, struct user isn't
		 * visible to the executing thread; thus the trampoline
		 * code pointer is explicitly passed in when setting
		 * a signal handler.
		 */
		u.u_sigtramp = uap->sigtramp;
#endif /* defined(balance) || defined(mips) || defined(i860) */
#ifdef	i386
		u.u_sigreturn = uap->sigreturn;
#endif
		setsigvec(p, sig, sa);
	}

	return (0);
}

/*
 * ssig is the target of the System V signal() system call, preserved here 
 * for binary compatibility and for programs explicitly linked for System V
 * behavior. Any process that calls this interface gets its P_SOUSIG flag set,
 * meaning that it will get "System V" signal behavior for caught signals.
 */

ssig(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	signo;
		int	(*fun)();
#if	defined(balance) || defined(mips) || defined(i860)
		int	(*sigtramp)();		/* signal trampoline code */
#endif /* defined(balance) || defined(mips) || defined(i860) */
#if	defined(i386)
		int	(*sigreturn)();		/* sigreturn() address */
#endif
	} *uap = (struct args *) args;
	register int a;
	struct sigaction act;
	register struct sigaction *sa = &act;

	a = uap->signo;
	sa->sa_handler = (void (*)())uap->fun;

	if (a <= 0 || a > NSIG || 
	    (sa->sa_handler != SIG_DFL && (a == SIGKILL || a == SIGSTOP)) ||
#ifdef NX
            (sa->sa_handler != SIG_DFL && (a == SIGUSR2 && nx_application(p)))||
#endif /* NX */
	    (a == SIGCONT && sa->sa_handler == SIG_IGN))
		return (EINVAL);
	sa->sa_mask = 0;
	sa->sa_flags = 0;
	*retval = (int)signal_disposition(a);
#if	defined(balance) || defined(mips) || defined(i860)
		/*
		 * On the Sequent Balance and Mips, struct user isn't
		 * visible to the executing thread; thus the trampoline
		 * code pointer is explicitly passed in when setting
		 * a signal handler.
		 */
		u.u_sigtramp = uap->sigtramp;
#endif /* defined(balance) || defined(mips) || defined(i860) */
#if	defined(i386)
	u.u_sigreturn = uap->sigreturn;
#endif
	setsigvec(p, a, sa);
	p->p_flag |= SOUSIG;		/* mark as simulating old stuff */
	return (0);
}

setsigvec(p, sig, sa)
	register struct proc *p;
	int sig;
	register struct sigaction *sa;
{
	register int bit;

	ASSERT(syscall_on_master());
	bit = sigmask(sig);
	/*
	 * Change setting atomically.
	 */
	(void) splhigh();
	sig_lock_simple(p);
	signal_disposition(sig) = sa->sa_handler;
	u.u_sigmask[sig] = sa->sa_mask &~ sigcantmask;

	if ((sa->sa_flags & SA_RESTART) == 0)
		u.u_sigintr |= bit;
	else
		u.u_sigintr &= ~bit;
	if (sa->sa_flags & SA_ONSTACK)
		u.u_sigonstack |= bit;
	else
		u.u_sigonstack &= ~bit;
	if (sig == SIGCHLD) {
		if (sa->sa_flags & SA_NOCLDSTOP)
			p->p_flag |= SNOCLDSTOP;
		else
			p->p_flag &= ~SNOCLDSTOP;
	}
	/*
	 * Set bit in p_sigignore for signals that are set to SIG_IGN,
	 * and for signals set to SIG_DFL where the default is to ignore.
	 * However, don't put SIGCONT in p_sigignore,
	 * as we have to restart the process.
	 */
	if (sa->sa_handler == SIG_IGN ||
	   (bit & defaultignmask && sa->sa_handler == SIG_DFL)) {
		p->p_sig &= ~bit;		/* never to be seen again */
		if (sig != SIGCONT)
			p->p_sigignore |= bit;	/* easier in psignal */
		p->p_sigcatch &= ~bit;
	} else {
		p->p_sigignore &= ~bit;
		if (sa->sa_handler == SIG_DFL)
			p->p_sigcatch &= ~bit;
		else
			p->p_sigcatch |= bit;
	}
	sig_unlock(p);
	(void) spl0();
}

/*
 * Initialize signal state for process 0;
 * set to ignore signals that are ignored by default.
 */
siginit(p)
	struct proc *p;
{

	p->p_sigignore = defaultignmask &~ sigmask(SIGCONT);
}

/*
 * Reset signals for an exec of the specified process.
 */
execsigs(p)
	register struct proc *p;
{
	register int nc, mask;

#ifndef	multimax
	ASSERT(syscall_on_master());
#endif

	/*
	 * Reset caught signals.  Held signals remain held
	 * through p_sigmask (unless they were caught,
	 * and are now ignored by default).
	 */
	while (p->p_sigcatch) {
		nc = ffs((long)p->p_sigcatch);
		mask = sigmask(nc);
		p->p_sigcatch &= ~mask;
		if (mask & defaultignmask) {
			if (nc != SIGCONT)
				p->p_sigignore |= mask;
			p->p_sig &= ~mask;

#ifdef	notyet	/* XXX - barbou@gr.osf.org */
			/* if this is a thread signal, clear it
			 * from the current thread as well.
		         */

			if (mask & threadmask) {
				u.u_sig &= ~mask;
			}
#endif	notyet
		}
		signal_disposition(nc) = SIG_DFL;
	}
	/*
	 * Reset stack state to the user stack.
	 * Clear set of signals caught on the signal stack.
	 */
	u.u_onstack = 0;
	u.u_sigsp = 0;
	u.u_sigonstack = 0;
}

/*
 * Manipulate signal mask.
 * Note that we receive new mask, not pointer,
 * and return old mask as return value;
 * the library stub does the rest.
 */
sigprocmask(p, args, retval)
	register struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		int	how;
		sigset_t mask;
	} *uap = (struct args *) args;
	int error = 0;

	ASSERT(syscall_on_master());
	*retval = p->p_sigmask;
	(void) splhigh();

	switch (uap->how) {
	case SIG_BLOCK:
		p->p_sigmask |= uap->mask &~ sigcantmask;
		break;

	case SIG_UNBLOCK:
		p->p_sigmask &= ~uap->mask;
		break;

	case SIG_SETMASK:
		p->p_sigmask = uap->mask &~ sigcantmask;
		break;
	
	default:
		error = EINVAL;
		break;
	}
	(void) spl0();
	return (error);
}

sigpending(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;

{

	*retval = p->p_sig;
	return (0);
}


#if COMPAT_43
/*
 * Generalized interface signal handler, 4.3-compatible.
 */

osigvec(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	register struct args {
		int	signo;
		struct	sigvec *nsv;
		struct	sigvec *osv;
#if	defined(balance) || defined(mips) || defined(i860)
		int	(*sigtramp)();		/* signal trampoline code */
#endif /* defined(balance) || defined(mips) || defined(i860) */
#if	defined(i386)
		int	(*sigreturn)();		/* sigreturn() address */
#endif
	} *uap = (struct args *) args;
	struct sigvec vec;
	register struct sigvec *sv;
	register int sig;
	int bit, error;

	ASSERT(syscall_on_master());
	sig = uap->signo;
#ifdef	multimax
	/*
	 * On the MMax the u area is not visible to the user tasks,
	 * therefore, the task must declare the address of its
	 * trampoline code before it attempts to catch signals.
	 * This is done by using the special signal SIGCATCHALL
	 */
	/* If this is the intermediate signal catcher then set it. */
	if (sig == SIGCATCHALL) {

		/* Copy the uap->nsv structure from user space to system
		 *	space.  Then set the return value and the signal
		 *	catcher.
		 */
		if (error = copyin ((caddr_t)uap->nsv, (caddr_t)&vec,
		    sizeof(vec)))
			return (error);
		*retval = (int)u.u_sigcatch;
		u.u_sigcatch = (void (*))vec.sv_handler;
		return (0);
	}
#endif
	if (sig <= 0 || sig > NSIG) {
		return (EINVAL);
	}
	sv = &vec;
	if (uap->osv) {
		sv->sv_handler = signal_disposition(sig);
		sv->sv_mask = u.u_sigmask[sig];
		bit = sigmask(sig);
		sv->sv_flags = 0;
		if ((u.u_sigonstack & bit) != 0)
			sv->sv_flags |= SV_ONSTACK;
		if ((u.u_sigintr & bit) != 0)
			sv->sv_flags |= SV_INTERRUPT;
		if (p->p_flag & SNOCLDSTOP)
			sv->sv_flags |= SA_NOCLDSTOP;
		if (error = copyout((caddr_t)sv, (caddr_t)uap->osv,
                    sizeof (vec)))
			return (error);
	}
	if (uap->nsv) {
		if (error = copyin((caddr_t)uap->nsv, (caddr_t)sv,
                    sizeof (vec)))
			return (error);
#ifdef NX
                if (sv->sv_handler != SIG_DFL && (sig == SIGUSR2 &&
                                                  nx_application(p)))
                        return(EINVAL);
#endif /* NX */
		if (sv->sv_handler != SIG_DFL && (sig == SIGKILL || sig == SIGSTOP))
			return(EINVAL);
		if (sig == SIGCONT && sv->sv_handler == SIG_IGN) {
			return (EINVAL);
		}
#if defined(mips) || defined(i860)
		/*
		 * check for unaligned pc on sighandler
		 */
		if (sv->sv_handler != SIG_IGN
		    && ((int)sv->sv_handler & (sizeof(int)-1))) {
			return (EINVAL);
		}
#endif /* defined(mips) || defined(i860) */
		sv->sv_flags ^= SA_RESTART; /* opposite of SV_INTERRUPT */
#if	defined(balance) || defined(mips) || defined(i860)
		/*
		 * On the Sequent Balance and Mips, struct user isn't
		 * visible to the executing thread; thus the trampoline
		 * code pointer is explicitly passed in when setting
		 * a signal handler.
		 */
		u.u_sigtramp = uap->sigtramp;
#endif /* defined(balance) || defined(mips) || defined(i860) */
#ifdef	i386
		u.u_sigreturn = uap->sigreturn;
#endif
		setsigvec(p, sig, (struct sigaction *)sv);
	}
	return (0);
}

osigblock(p, args, retval)
	register struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		int	mask;
	} *uap = (struct args *) args;

	ASSERT(syscall_on_master());
	(void) splhigh();

	*retval = p->p_sigmask;
	p->p_sigmask |= uap->mask &~ sigcantmask;

	(void) spl0();
	return (0);
}

osigsetmask(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	mask;
	} *uap = (struct args *) args;

	ASSERT(syscall_on_master());
	(void) splhigh();

	*retval = p->p_sigmask;
	p->p_sigmask = uap->mask &~ sigcantmask;

	(void) spl0();
	return (0);
}

#endif	/* COMPAT_43 */

/*
 * Suspend process until signal, providing mask to be set
 * in the meantime.  Note nonstandard calling convention:
 * libc stub passes mask, not pointer, to save a copyin.
 */
sigsuspend(p, args, retval)
	register struct proc *p;
	void *args;	
	int *retval;
{
	struct args {
		sigset_t mask;
	} *uap = (struct args *) args;

	ASSERT(syscall_on_master());

	/*
	 * When returning from sigsuspend, we want
	 * the old mask to be restored after the
	 * signal handler has finished.  Thus, we
	 * save it here and mark the proc structure
	 * to indicate this (should be in u.).
	 */
	u.u_oldmask = p->p_sigmask;
	p->p_flag |= SOMASK;
	p->p_sigmask = uap->mask &~ sigcantmask;
	(void) tsleep((caddr_t)&u, PSLEP | PCATCH, "pause", 0);
	/* always return EINTR rather than ERESTART... */
#ifdef	TNC
	/*
	 * ... unless the signal is a SIGMIGRATE being caught by the
	 * emulator's default handler - in which case, ERESTART
	 */
	if (p->p_cursig == SIGMIGRATE) {
		unsigned int action;
		action = (unsigned int) p->p_utask.uu_signal[SIGMIGRATE];
		if (EMULATOR_BASE <= action && action <= EMULATOR_END)
			return (ERESTART);
	}
#endif	/* TNC */
	return (EINTR);
}

sigstack(p, args, retval)
	struct proc *p;
	void *args;	
	int *retval;
{
	register struct args {
		struct	sigstack *nss;
		struct	sigstack *oss;
	} *uap = (struct args *) args;
	struct sigstack ss;
	int error = 0;

	ASSERT(syscall_on_master());
	if (uap->oss && (error = copyout((caddr_t)&u.u_sigstack,
            (caddr_t)uap->oss, sizeof (struct sigstack))))
		return (error);
	if (uap->nss && (error = copyin((caddr_t)uap->nss, (caddr_t)&ss,
            sizeof (ss))) == 0)
		u.u_sigstack = ss;
	return (error);
}

pps_sigprocset(ps, signo, arg, has_priv, uid, ruid, pid, sid, nproc) 
	struct procset *ps;
	int signo;
	int arg;
	int has_priv;
	uid_t uid, ruid;
	pid_t pid, sid;
	int *nproc;
{
	register struct proc *p;
	int f = 0, error = ESRCH;
	
	TNC_unix_master();
	for (p = allproc; p != NULL; p = p->p_nxt) {
		if (!proc_in_procset(p, ps))
			continue;
		if (p->p_ppid == 0 || p->p_flag&SSYS || (!has_priv &&
	    	  !(cansignal(uid,ruid,sid,p->p_svuid,p->p_ruid,p->p_sid,signo)
			|| ((p->p_pid == 1) && (signo == SIGCHLD)))))
			continue;
#ifdef NX
                if ((signo == SIGUSR2 && nx_application(p))
                    && p == u.u_procp)
                        continue;
#endif /* NX */
		if ((signo == SIGKILL || signo == SIGSTOP) && p->p_pid == pid)
			continue;
#if     SEC_ARCH
                        if (!sec_can_kill(p))
			continue;
                        audstub_proc_levels(p);
#endif
		f++;
		if (signo) {
#ifdef	TNC
			if (signo == SIGMIGRATE)
				p->p_sig_arg = arg;
#endif			/* TNC */
			psignal3(p, signo, TRUE);
		}
	}
	TNC_unix_release();
	if (nproc)
		*nproc = f;

	return (f ? 0 : error);
}

/*
 * Send the specified signal to
 * all processes with 'pgid' as
 * process group.
 */

gsignal(pgid, sig)
{
	gsignal3(pgid, sig, TRUE);
}

gsignal3(pgid, sig, can_block)
{
	gsignal4(pgid, sig, 0, can_block);
}

gsignal_chktty(pgid, sig)
{
	gsignal4(pgid, sig, TRUE, TRUE);
}

gsignal4(pgid, sig, checkctty, canblock)
	pid_t pgid;
{
	register struct vproc *g;

	if (pgid) {
		/*
		 * call vproc operation to do a process group signal
		 *   passing on the controlling tty check and blocking flags
		 */
		g = LOCATE_VPROC_PID(pgid);
		if (g != 0) {
			(void) VPOP_SIGPGRP(g, sig, 0,
					    NULL,
					    (VPROC_HAS_PRIV |
				     		(checkctty ? VPROC_CHECK_CTTY 
							   : FALSE) |
				     		(canblock ? 0 
							  : VPROC_CANT_BLOCK)));
			VPROC_RELEASE(g,"gsignal4");
		}
	}
}

#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
/*
 * Send a signal caused by a trap to the current process.
 * If it will be caught immediately, deliver it with correct code.
 * Otherwise, post it normally.
 */
trapsignal(sig, code)
	register int sig;
	unsigned code;
{
	register struct proc *p = u.u_procp;	/* XXX */
	int mask;

	ASSERT(syscall_on_master());
	mask = sigmask(sig);
	if ((p->p_flag & STRC) == 0 && (p->p_sigcatch & mask) != 0 &&
	    (p->p_sigmask & mask) == 0) {
		u.u_ru.ru_nsignals++;
		sendsig(signal_disposition(sig), sig, p->p_sigmask, code);
		p->p_sigmask |= u.u_sigmask[sig] | mask;
	} else {
		u.u_code = code;	/* XXX for core dump/debugger */
		psignal(p, sig);
	}
}
#endif	/* OSF1_SERVER */

/*
 * Put the argument process into the stopped
 * state and notify the parent via wakeup.
 * Signals are handled elsewhere.
 */
stop(p)
	register struct proc *p;
{
#if	SIG_DEBUG_ON
	int cur_pid = (u.u_procp ? u.u_procp->p_pid : 0);
#endif

	ASSERT(syscall_on_master());
	SIG_DEBUG(1, ("stop[%d]: going to stop %d.\n", cur_pid, p->p_pid));

	p->p_stat = SSTOP;
	p->p_flag &= ~SWTED;

	sig_unlock(p);
	(void) unix_task_suspend(p);
#ifdef NX
        if (nx_no_signal(p)) {
	    (void) VPOP_REPORT_STATE(p->p_vproc, VPROC_GANG_STOP);
	}
#endif /* NX */
	(void) VPOP_REPORT_STATE(p->p_vproc, VPROC_STOP);
	sig_lock_simple(p);

	SIG_DEBUG(1, ("stop[%d]: stopped %d and wokenup %d. exiting\n", cur_pid, p->p_pid, p->p_ppid));
#ifdef NX
	(void)nx_signal_tam(p->p_vproc);
#endif /* NX */
}


/*
 * Send the specified signal to
 * the specified process.
 */
psignal(p, sig)
	struct proc *p;
	int sig;
{
	psignal3(p, sig, TRUE);
}

pproc_chksigmask(p, sig, ismasked, isignored)
	struct proc *p;
	int sig;
	int *ismasked, *isignored;
{
	if (ismasked)
		*ismasked  = sigmask(sig) & p->p_sigmask;
	if (isignored)
		*isignored = sigmask(sig) & p->p_sigignore;
}
	

pproc_psignal(p, sig, arg,
		can_block, child_stop, iftraced, has_priv, uid, ruid, sid) 
	struct proc *p;
	int sig;
	int arg;
	int can_block;
	int child_stop;
	int iftraced;
	int has_priv;
	uid_t uid, ruid;
	pid_t sid;
{
	ASSERT(p != NULL);
	ASSERT(p->p_pproc_hold_count > 0);	/* pproc_hold() was called */

	PROC_LOCK(p);				/* to guard p_flag */

	if (child_stop && ((p->p_flag&SNOCLDSTOP) != 0)) {
		PROC_UNLOCK(p);
		return(ESUCCESS);
	}

	if (!has_priv &&
	    !(cansignal(uid,ruid,sid,p->p_svuid,p->p_ruid,p->p_sid,sig) ||
	    ((p->p_pid == 1) && (sig == SIGCHLD)))) {
		PROC_UNLOCK(p);
		return(EPERM);
	}

	/*
	 * If so requested and the destination is being traced, force delivery
	 * (of SIGKILL) by untracing proc, else don't do anything.
	 */
	if (iftraced) {
		if (p->p_flag&STRC)
			p->p_flag &= ~STRC;
		else {
			PROC_UNLOCK(p);
			return(ESUCCESS);
		}
	}
		
	if (sig) {
#ifdef	TNC
		if (sig == SIGMIGRATE)
			p->p_sig_arg = arg;
#endif

		PROC_UNLOCK(p);
		unix_master();
		psignal3(p, sig, can_block);
		unix_release();
	} else {
		PROC_UNLOCK(p);
	}
	return(ESUCCESS);

}

/*
 * Side thread version of psignal3().  Psignal3() requests this call.
 * It runs with the same proc context but by a different server thread
 * so can_block can be true.
 */
void
side_psignal3(
	struct proc	*p,
	int		sig)
{
	psignal3(p, sig, TRUE);

	return;
}
/*
 * Send the specified signal to the specified process.
 * Only allowed to block/sleep if can_block is TRUE.
 * When called from interrupt handlers, can_block must be FALSE.
 */
psignal3(p, sig, can_block)
	register struct proc	*p;
	register int		sig;
	register int		can_block;
{
	register sig_t		action;
	register task_t		sig_task;
#ifndef	OSF1_SERVER
	register int		s;
	register thread_t	sig_thread;
	register thread_t	cur_thread;
#else	/* OSF1_SERVER */
#if	SIG_DEBUG_ON
	register int		cur_pid = (u.u_procp ? u.u_procp->p_pid : 0);
#endif
	extern void		side_issig_psig(int);
#endif	/* OSF1_SERVER */
	int			mask;
	int			error;

	SIG_DEBUG(0, ("psignal3[%d]: posting sig=%d to p=%d\n", \
			cur_pid, sig, p->p_pid));
	ASSERT(syscall_on_master());
	ASSERT((unsigned)sig <= NSIG && sig != 0);
	ASSERT((p->p_flag & SDUMMY) == 0);

#ifdef	OSF1_SERVER
	/*
	 * Just return without doing anything if the proc is exiting, or if
	 * it has exited and been reaped (zeroed out proc struct) or if even
	 * further, the proc struct has been re-used by another proc.
	 */
	if ((p->p_flag & SWEXIT) != 0 ||
			p->p_vproc == NULL ||
			p->p_vproc->vp_pid != p->p_pid) {
		SIG_DEBUG(1, ("psignal3[%d]: p=%d is exiting. return.\n", \
				cur_pid, p->p_pid));
		return;
	}

	if (can_block) {
		sig_lock_simple(p);
	} else
#ifndef TNC
		if (!proc_sigtrylock(p))
#endif
	{
		/*
		 * Send a message to a server thread, which *can* block.
		 * If the proc is already exiting, no side thread is
		 * created.
		 */
		(void)request_side_thread(p, side_psignal3, (int)sig);
		return;
	}
#ifdef TNC
	/*
	 * If this proc's parent is on a different node, TNC will need to
	 * go remote when it reports the change in the child's state to
	 * that parent.  Going remote always involves dropping the master
	 * lock and we need to make sure the phys proc doesn't exit and get
	 * reaped out from under us while we're away, so we bump the proc's
	 * exit hold counter for the duration of psignal3().
	 */
	simple_lock(&p->p_lock);
	p->p_exit_hold_count++;
	simple_unlock(&p->p_lock);

#endif	/* TNC */
#endif	/* OSF1_SERVER */

	mask = sigmask(sig);

#ifdef	OSF1_SERVER
	sig_task = p->p_task;
#else
	/*
	 *	We will need the task pointer later.  Grab it now to
	 *	check for a zombie process.  Also don't send signals
	 *	to kernel internal tasks.
	 */
	if (((sig_task = p->task) == TASK_NULL) || sig_task->kernel_vm_space)
		goto final_out;
#endif

	/*
	 * If proc is traced, always give parent a chance.
	 */
	if (p->p_flag & STRC) {
		SIG_DEBUG(2, ("psignal3[%d]: p=%d is traced, so SIG_DFL.\n", \
				cur_pid, p->p_pid));
		action = SIG_DFL;
	} else {
		/*
		 * If the signal is being ignored,
		 * then we forget about it immediately.
		 * (Note: we don't set SIGCONT in p_sigignore,
		 * and if it is set to SIG_IGN,
		 * action will be SIG_DFL here.)
		 */
		if (p->p_sigignore & mask) {
			sig_unlock(p);
			SIG_DEBUG(1, ("psignal3[%d]: SIG_IGN. return.\n", \
					cur_pid));
			goto final_out;
		}
		if (p->p_sigmask & mask) {
			action = SIG_HOLD;
			SIG_DEBUG(2, ("psignal3[%d]: SIG_HOLD.\n", cur_pid));
		} else if (p->p_sigcatch & mask) {
			action = SIG_CATCH;
			SIG_DEBUG(2, ("psignal3[%d]: SIG_CATCH.\n", cur_pid));
		} else {
			action = SIG_DFL;
			SIG_DEBUG(2, ("psignal3[%d]: SIG_DFL.\n", cur_pid));
		}
	}

	p->p_sig |= mask;

#ifdef NX
	/*
	 * On getting a SIGUSR2 clear SIGCONT and vice-versa
	 */

        if( nx_application(p) ) {
                        if( sig == SIGUSR2 ) {
                                p->p_sig &= ~sigmask(SIGCONT);
                        } else if ( sig == SIGCONT ) {
                                p->p_sig &= ~sigmask(SIGUSR2);
                        }
        }

#endif /* NX */

	switch (sig) {
		
	case SIGTERM:
		if ((p->p_flag&STRC) || action != SIG_DFL)
			break;
		/* fall into ... */

	case SIGCONT:
#ifdef NX
		if (!(nx_gang_state(p))) {
		    int temp_mask = stopsigmask;

		    /*
		     *  If it is NOT a NX application, remove SIGUSR2
		     *  from the stopsigmask because it does not apply.
		     */
		    if (!nx_application(p))
			temp_mask &= ~sigmask(SIGUSR2);

		    p->p_sig &= ~temp_mask;
		}
		break;
#endif /* NX */
			p->p_sig &= ~stopsigmask;
		break;
		
	case SIGSTOP:
	case SIGTSTP:
	case SIGTTIN:
	case SIGTTOU:
#ifdef NX
        case SIGUSR2:
                if (sig == SIGUSR2 && !nx_application(p))
                    break;
#endif /* NX */
		p->p_sig &= ~sigmask(SIGCONT);
		break;
	}
	/*
	 * Defer further processing for signals which are held,
	 * except that stopped processes must be continued by SIGCONT.
	 */
	if (action == SIG_HOLD && (sig != SIGCONT || p->p_stat != SSTOP)) {
		sig_unlock(p);
		SIG_DEBUG(1, ("psignal3[%d]: deferring held signal (%d). return.\n", cur_pid, sig));
		goto final_out;
	}

#ifdef	OSF1_SERVER
#else
	/*
	 *	Deliver the signal to the first thread in the task. This
	 *	allows single threaded applications which use signals to
	 *	be able to be linked with multithreaded libraries.  We have
	 *	an implicit reference to the current_thread, but need
	 *	an explicit one otherwise.  The thread reference keeps
	 *	the corresponding task data structures around too.  This
	 *	reference is released by thread_deallocate_interrupt
	 *	because psignal() can be called from interrupt level).
	 */
	s = splsched();		/* at least splsched */

	cur_thread = current_thread();
	/*
	 *	This is a mess.  The thread_list_lock is a special
	 *	lock that excludes insert and delete operations
	 *	on the task's thread list for our benefit (can't
	 *	grab task lock because we might be at interrupt
	 *	level).  Check if there are any threads in the
	 *	task.  If there aren't, sending it a signal
	 *	isn't going to work very well, so just return.
	 */
	simple_lock(&sig_task->thread_list_lock);
	if (queue_empty(&sig_task->thread_list)) {
		simple_unlock(&sig_task->thread_list_lock);
		(void) splx(s);
		goto final_out;
	}
	sig_thread = (thread_t) queue_first(&sig_task->thread_list);
	if (sig_thread != cur_thread)
		thread_reference(sig_thread);
	simple_unlock(&sig_task->thread_list_lock);

	/*
	 *	SIGKILL priority twiddling moved here from above because
	 *	it needs sig_thread.  Could merge it into large switch
	 *	below if we didn't care about priority for tracing
	 *	as SIGKILL's action is always SIG_DFL.
	 */
	if ((sig == SIGKILL) && (p->p_nice > PRIZERO)) {
		p->p_nice = PRIZERO;
		thread_max_priority(sig_thread, sig_thread->processor_set,
			BASEPRI_USER);
		thread_priority(sig_thread, BASEPRI_USER, FALSE);
	}
#endif

	if (p->p_flag&STRC) {
		/*
		 *	Process is traced - wake it up (if not already
		 *	stopped) so that it can discover the signal in
		 *	issig() and stop for the parent.
		 */
		if (p->p_stat != SSTOP) {
			/*
			 *	Wake it up to get signal
			 */
			SIG_DEBUG(2, ("psignal3[%d]: debugger to wake up.\n", cur_pid));
			goto run;
		}
		goto out;
	}
	else if (action != SIG_DFL) {
		/*
		 *	User wants to catch the signal.
		 *	Wake up the thread, but don't un-suspend it
		 *	(except for SIGCONT).
		 */
		if (sig == SIGCONT) {
#ifdef	OSF1_SERVER
			/*
			 * XXX - wake up the process before changing its
			 * state, so that it has a chance to realize that it
			 * has been stopped (see thread_block()).
			 */
			unsleep_proc(p, TRUE);
			thread_yield();
			p->p_stat = SRUN;

			sig_unlock(p);
#ifdef NX
                        (void) VPOP_REPORT_STATE(p->p_vproc,
                                                 VPROC_GANG_UNSTOP);
#endif /* NX */
			(void) VPOP_REPORT_STATE(p->p_vproc, VPROC_UNSTOP);

			sig_lock_simple(p);
			(void) task_resume(sig_task);
			SIG_DEBUG(2, ("psignal3[%d]: p=%d resumed SIGCONT.\n", \
					cur_pid, p->p_pid));
			goto out_force;
#else	/* OSF1_SERVER */
			(void) task_resume(sig_task);
			/*
			 *	Process will be running after 'run'
			 */
			p->p_stat = SRUN;
#endif	/* OSF1_SERVER */
		}
		goto run;
	}
	else {
		/*
		 *	Default action - varies
		 */

		if (mask & stopsigmask) {
			/*
			 * These are the signals which by default
			 * stop a process.
			 *
			 * Don't clog system with children of init
			 * stopped from the keyboard.
			 */
#ifdef NX
                        if (sig == SIGUSR2 && !nx_application(p))
                            goto no_nx_stop;
#endif /* NX */
			if (sig != SIGSTOP && p->p_ppid == 1) {
#ifdef NX
                                if (sig == SIGUSR2)
                                    goto stop_the_task;
#endif /* NX */
				SIG_DEBUG(2, ("psignal3[%d]: can't stop p=%d (init son). sending SIGKILL.\n", cur_pid, p->p_pid));
				sig_unlock(p);
				psignal(p, SIGKILL);
				sig_lock_simple(p);
				SIG_DEBUG(2, ("psignal3[%d]: back after sending SIGKILL to p=%d.\n", cur_pid, p->p_pid));
				p->p_sig &= ~mask;
				goto out;
			}
#ifdef NX
stop_the_task:
#endif /* NX */
			/*
			 *	Stop the task.
			 */
			p->p_sig &= ~mask;
			if (p->p_stat != SSTOP) {
/*
 * "in-line" can_stop_process from MK1.0.4.1
 * We can NEVER stop the process unless we
 * are from that process; defer to issig_psig
 * or psig().
 */
#if 0
				/*
				 *	If task hasn't already been stopped by
				 *	a signal, stop it.
				 */
#ifdef NX
                                if(sig == SIGUSR2 && nx_application(p)) {
                                        p->p_cursig = SIGUSR2;
                                }
#endif /* NX */
				stop(p);
				p->p_stopsig = sig;
#else
				/* Can't stop now */
				p->p_sig |= mask;
				goto run;
#endif
			}
			goto out;
		}
#ifdef NX
no_nx_stop:
#endif /* NX */

		switch (sig) {
			/*
			 * Signals ignored by default have been dealt
			 * with already, since their bits are on in
			 * p_sigignore.
			 */

		case SIGKILL:
#ifdef	OSF1_SERVER
			p->p_stat = SRUN;
			(void) task_resume(sig_task);
			unsleep_proc(p, FALSE);
			SIG_DEBUG(3, ("psignal3[%d]: resumed and unslept p=%d.\n", cur_pid, p->p_pid));
			goto out_force;
#else
			/*
			 * Kill signal always sets process running and
			 * unsuspends it.
			 */
			while (sig_task->user_stop_count > 0)
				(void) task_resume(sig_task);
			/*
			 *	Process will be running after 'run'
			 */
			p->p_stat = SRUN;

			/*
			 * Break it out of user wait, as well.
			 */
			while (sig_thread->user_stop_count > 0)
				(void) thread_resume(sig_thread);

			/*
			 * Clear system wait if possible.  The
			 * THREAD_SHOULD_TERMINATE is overkill, but
			 * saves us from potentially buggy code elsewhere.
			 */
			clear_wait(sig_thread, THREAD_SHOULD_TERMINATE, FALSE);
#if	MACH_HOST
			/*
			 * Make sure it can run.
			 */
			if (sig_thread->processor_set->empty)
				thread_assign(sig_thread, &default_pset);
#endif
			/*
			 * If we're delivering the signal to some other
			 * thread, that thread might be stuck in an
			 * exception.  Break it out.  Can't call
			 * thread_exception_abort from high spl, but
			 * SIGKILL can't be sent from interrupt level, so
			 * it's ok to drop spl.  Can call thread_deallocate
			 * for same reason.
			 */
			splx(s);
			if (sig_thread != cur_thread) {
				thread_exception_abort(sig_thread);
				thread_deallocate(sig_thread);
			}
#endif
			goto final_out;

		case SIGCONT:
			/*
			 * Let the process run.  If it's sleeping on an
			 * event, it remains so.
			 */
			p->p_stat = SRUN;

			sig_unlock(p);
#ifdef NX
			(void) VPOP_REPORT_STATE(p->p_vproc, 
						 VPROC_GANG_UNSTOP);
#endif /* NX */
			(void) VPOP_REPORT_STATE(p->p_vproc, VPROC_UNSTOP);
			sig_lock_simple(p);

			(void) task_resume(sig_task);
			SIG_DEBUG(2, ("psignal3[%d]: p=%d resumed by SIGCONT.\n", cur_pid, p->p_pid));
			goto out;

		default:
			/*
			 * All other signals wake up the process, but don't
			 * resume it.
			 */
			goto run;
		}
	}
	/*NOTREACHED*/
run:
	/*
	 *	BSD used to raise priority here.  This has been broken
	 *	for ages and nobody's noticed.  Code deleted. -dlb
	 */

	/*
	 *	Wake up the thread if it is interruptible.
	 */
#ifdef	OSF1_SERVER
	unsleep_proc(p, TRUE);
	SIG_DEBUG(2, ("psignal3[%d]: unslept p=%d.\n", cur_pid, p->p_pid));
#else
	clear_wait(sig_thread, THREAD_INTERRUPTED, TRUE);
#endif

out_force:
#ifdef 	OSF1_SERVER
	/*
	 * We only need to start an issig_psig side thread when:
	 *	We don't have one going already,
	 *	and
	 *	The process does not already have a thread in the PM --
	 *		not including the "side" threads -- because such
	 *		a thread will pick up a pending signal on its way
	 *		out of the server back to the emulator.  (Side
	 *		threads do not return to the emulator so they
	 *		can't take any signals back there with them.)
	 *
	 * What issig_psig() does is send a message to the emulator, telling
	 * it to pick up signal info from the server (via bsd_take_signals()).
	 * If the proc is already exiting, no side thread is created.
	 */

	PROC_LOCK(p);
	SIG_DEBUG(2, ("psignal3[%d]: issig_psig? ref_cnt=%d side_cnt=%d "
			"issig_flag=%d\n",
			cur_pid, p->p_ref_count, p->p_side_ref_count,
			p->p_issig_flag));
	if (p->p_ref_count - p->p_side_ref_count == 1 && p->p_issig_flag == 0) {
		p->p_issig_flag = 1;		/* issig_psig thrd now viable */
		PROC_UNLOCK(p);
		SIG_DEBUG(2, ("psignal3[%d]: starting issig_psig(%d).\n", 
				cur_pid, p->p_pid));
		(void)request_side_thread(p, side_issig_psig, (int)0);
	} else {
		PROC_UNLOCK(p);
	}
#endif

out:
#ifndef	OSF1_SERVER
	splx(s);
	if (sig_thread != cur_thread)
		thread_deallocate_interrupt(sig_thread);
#endif

#ifdef NX
	if(nx_gang_state(p))
		p->p_sig |= mask;
#endif /* NX */

	sig_unlock(p);

	SIG_DEBUG(1, ("psignal3[%d]: returning. p_sig(%d)=0x%08x\n", 
		      cur_pid, p->p_pid, p->p_sig));

final_out:
#ifdef TNC
	simple_lock(&p->p_lock);
	if (--p->p_exit_hold_count == 0) {
		wakeup((caddr_t) &p->p_exit_hold_count);
	}
	simple_unlock(&p->p_lock);
#endif	/* TNC */
;
}

/*
 * Returns true if the current
 * process has a signal to process.
 * The signal to process is put in p_cursig.
 * This is asked at least once each time a process enters the
 * system (though this can usually be done without actually
 * calling issig by checking the pending signal masks.)
 * A signal does not do anything
 * directly to a process; it sets
 * a flag that asks the process to
 * do something to itself.
 */
issig()
{
	register struct proc *p;
	register int sig;
	int sigbits, mask;
#ifdef	OSF1_SERVER
#else
	thread_t initial_thread;
	thread_t cur_thread;
	int s;
#endif

	p = u.u_procp;
	ASSERT(p != NULL);
	/*
	 *	This must be called on master cpu
	 */
#ifdef	OSF1_SERVER
	ASSERT(syscall_on_master());
#else
	if (cpu_number() != master_cpu)
		panic("issig not on master");
#endif
	SIG_DEBUG(1, ("issig[%d]: entering with p_cursig=%d, p_sig=0x%08x.\n", \
			p->p_pid, p->p_cursig, p->p_sig));

	/*
	 *	Try for the signal lock.
	 *	If we already have it, return FALSE: don't handle any signals.
	 *	If we must halt, return TRUE to clean up our state.
	 */
	sig_lock_or_return(p, return(FALSE), return(TRUE));
	for (;;) {
#ifdef	OSF1_SERVER
		/*
		 * It is possible that a thread calls issig() between the
		 * issig() and the psig() of bsd_issig_psig(). In this case,
		 * a signal might have been left in p->p_cursig.
		 */
		if (p->p_cursig != 0) {
			p->p_sig |= sigmask(p->p_cursig);
			p->p_cursig = 0;
		}
		sigbits = p->p_sig & ~p->p_sigmask;
#else
		/*
		 *	In a multi-threaded task it is possible for
		 *	one thread to interrupt another's issig(); psig()
		 *	sequence.  In this case, the thread signal may
		 *	be left in u.u_cursig.  We recover here
		 *	by getting it out and starting over.
		 */
		if (u.u_cursig != 0) {
		    u.u_sig |= sigmask(u.u_cursig);
		    u.u_cursig = 0;
		}
		sigbits = (u.u_sig | p->p_sig) &~ p->p_sigmask;
#endif
		if (p->p_flag&SVFORK)
			sigbits &= ~stopsigmask;

#ifdef	OSF1_SERVER
		sig = ffs((long)sigbits);
		SIG_DEBUG(2, ("issig[%d]: extract signal %d.\n", \
				p->p_pid, sig));
		mask = sigmask(sig);
#else
		/*
		 * only allow delivery of process signals (asynchronous)
		 * to the initial thread. This is the first thread in
		 * the tasks thread list.
		 */
		s = splsched();
		simple_lock(&p->task->thread_list_lock);
		initial_thread = (thread_t)queue_first(&p->task->thread_list);
		simple_unlock(&p->task->thread_list_lock);
		splx(s);
		cur_thread = current_thread();
		for (;;) {
			if (sigbits == 0)
				break;
			sig = ffs((long)sigbits);
			mask = sigmask(sig);
			if (mask & threadmask) {
				u.u_sig &= ~mask;
				u.u_cursig = sig;
				break;
			} else {
				if (cur_thread != initial_thread)
					sigbits &= ~mask;
				else
					break;
			}
		}
#endif
		if (sigbits == 0)
			break;

		p->p_sig &= ~mask;		/* take the signal! */
		/*
		 * If the signal is being ignored, look for the next one.
		 * It has now been removed from p->p_sig, and will never
		 * be seen again. Note that this is a change from the
		 * 4.3BSD behavior, where ignored signals were excluded
		 * from sigbits above, and thus remained pending. This would
		 * mean that if the action for such a signal were changed
		 * later (possibly MUCH later), the old signal would
		 * suddenly get delivered. --rsc
		 */
		if (mask & p->p_sigignore && (p->p_flag&STRC) == 0) {
			SIG_DEBUG(2, ("issig[%d]: signal %d: SIG_IGN.\n", \
					p->p_pid, sig));
			continue;	/* only if STRC was on when posted */
		}
		p->p_cursig = sig;
		if (p->p_flag&STRC && (p->p_flag&SVFORK) == 0) {
#ifdef	OSF1_SERVER
#else
			register task_t	task;
#endif

			SIG_DEBUG(3, ("issig[%d]: send SIGCHLD to debugger "\
					"(calling stop()).\n", p->p_pid));
			/*
			 * If traced, always stop, and stay
			 * stopped until released by the parent.
			 */
			p->p_stopsig = sig;
			stop(p);
			SIG_DEBUG(3, ("issig[%d]: back from stop(), awaiting " \
					"kill or continue from debugger.\n", \
					p->p_pid));

			/* 
			 * XXX - we should block on something here !!!
			 */
#if	1
			/*
			 * XXX - temporary and non-determinist solution :
			 * let thread_block() enter an active loop !
			 */
			sig_unlock(p);
			thread_block();
			sig_lock_simple(p);
#endif
			/*
			 *	We get here only if task
			 *	is continued or killed.  Kill condition
			 *	is signalled by adding NSIG to p_cursig.
			 *	Pass original p_cursig as exit value in
			 *	this case.
			 */
			if (p->p_cursig <= NSIG) {
				SIG_DEBUG(3, ("issig[%d]: NO kill from " \
					"debugger (%d)", p->p_pid,p->p_cursig));
			} else {
				sig = p->p_cursig - NSIG;
				SIG_DEBUG(3, ("issig[%d]: got kill from " \
					"debugger (%d-%d=%d)", p->p_pid, \
					p->p_cursig, NSIG, sig));
				/*
				 *	Wait event may still be outstanding;
				 *	clear it, since sig_lock_to_exit will
				 *	wait.
				 */
				clear_wait(current_thread(),
					THREAD_INTERRUPTED,
					FALSE);
#ifdef	OSF1_SERVER
				sig_unlock(p);
#else	/* OSF1_SERVER */
				sig_lock_to_exit(p);
#endif	/* OSF1_SERVER */
				/*
				 * Mark it as exiting.
				 */
				simple_lock(&p->p_lock);
				p->p_flag |= SWEXIT;
				p->p_flag &= ~STRC;
				simple_unlock(&p->p_lock);

				TNC_unix_release();
				(void) VPOP_EXIT(p->p_vproc, sig);
				TNC_unix_master();
			}

#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
			/*
			 *	We may have to quit
			 */
			if (thread_should_halt(current_thread())) {
				sig_unlock(p);
				return(TRUE);
			}
#endif	/* OSF1_SERVER */

			/*
			 * If the traced bit got turned off,
			 * then put the signal taken above back into p_sig
			 * and go back up to the top to rescan signals.
			 * This ensures that p_sig* and u_signal are consistent.
			 */
			if ((p->p_flag&STRC) == 0) {
#ifdef	OSF1_SERVER
				SIG_DEBUG(3, ("issig[%d]: detached from debugger. restore sig=%d.\n", u.u_procp->p_pid, sig));
				if((p->p_flag&SWEXIT) != 0)
					goto send;
				p->p_sig |= mask;
				p->p_cursig = 0;
#else
				if (mask & threadmask)
					u.u_sig |= mask;
				else
					p->p_sig |= mask;
#endif
				continue;
			}

			/*
			 * If parent wants us to take the signal,
			 * then it will leave it in p->p_cursig;
			 * otherwise we just look for signals again.
			 */
			sig = p->p_cursig;
			if (sig == 0) {
SIG_DEBUG(2, ("issig[%d]: signal %d discarded by debugger.\n", u.u_procp->p_pid, sig));
				continue;
			}

			/*
			 * If signal is being masked put it back
			 * into p_sig and look for other signals.
			 */
			mask = sigmask(sig);
			if (p->p_sigmask & mask) {
#ifdef	OSF1_SERVER
SIG_DEBUG(2, ("issig[%d]: signal %d deferred cuz masked.\n", u.u_procp->p_pid, sig));
				p->p_sig |= mask;
#else
				if (mask & threadmask)
					u.u_sig |= mask;
				else
					p->p_sig |= mask;
#endif
				continue;
			}
		}

		/*
		 * intr_delivery on a dummy proc
		 * Don't *DO* anything, but do succeed.
		 */
		if (p->p_flag & SDUMMY)
			goto send;

		switch ((int)signal_disposition(sig)) {
		case (int) SIG_DFL:

			SIG_DEBUG(3, ("issig[%d]: action(%d) is SIG_DFL.\n", u.u_procp->p_pid, sig));

			/*
			 * Don't take default actions on system processes.
			 */
			if (p->p_ppid == 0) {
				SIG_DEBUG(2, ("issig[%d]: sig %d SIG_DFL not applied to system process.\n", u.u_procp->p_pid, sig));
#ifdef	OSF1_SERVER
				p->p_cursig = 0;
#else
				u.u_cursig = 0;
#endif
				break;
			}
			if (mask & stopsigmask) {
#ifdef NX
                                if (sig == SIGUSR2 && !nx_application(p))
                                    goto send;
#endif /* NX */
				SIG_DEBUG(3, ("issig[%d]: signal %d is a stop signal.\n", u.u_procp->p_pid, sig));
				if (p->p_flag&STRC) {
					SIG_DEBUG(2, ("issig[%d]: stop sig %d discarded cuz traced.\n", u.u_procp->p_pid, sig));
#ifdef	OSF1_SERVER
					p->p_cursig = 0;
#endif
					continue;
				}
				/*
				 * Insert test here for process in orphaned
				 * process group -- signal should be discarded
				 */
/*
 * "in-line" can_stop_process from MK1.0.4.1.
 * We can never stop in issig.
 */
#if	0
				stop(p);
#else
				/* Can't stop now. Let psig have stop signal */
				goto send;
#endif
				/*
				 * XXX - we should block on something here !!!!
				 */
#if	1
				/*
				 * XXX - temporary and non-determinist solution:
				 * let thread_block() enter an active loop !
				 */
				sig_unlock(p);
				thread_block();
				sig_lock_simple(p);
#endif	1

				p->p_cursig = 0;
				continue;
			} else if (mask & defaultignmask) {
				/*
				 * Except for SIGCONT, shouldn't get here,
				 * since ignored signals were masked out earlier.
				 * Default action is to ignore; drop it.
				 */
				SIG_DEBUG(3, ("issig[%d]: default action for sig %d is SIG_IGN. discarded.\n", u.u_procp->p_pid, sig));
				p->p_cursig = 0;

				/*
				 *  Save the signal in p_stopsig
				 *  so that debuggers can find out why
				 *  a processed stopped due to a signal
				 *  which is to be ignored.
				 */
				if (p->p_flag&STRC) {
				    p->p_stopsig = sig;
				}
				continue;		/* == ignore */
			} else
				goto send;
			/*NOTREACHED*/

#if	defined(multimax) || defined(PS2) || defined(i860)
		/* Multimax and PS2 compiler needs this cast. */
		case ((int)SIG_IGN):
#else
		case SIG_IGN:
#endif
			SIG_DEBUG(3, ("issig[%d]: action(%d) is SIG_IGN. discarding.\n", u.u_procp->p_pid, sig));
			/*
			 * Masking above should prevent us ever trying
			 * to take action on an ignored signal other
			 * than SIGCONT, unless process is traced.
			 */
#ifndef	OSF1_SERVER
			if (sig != SIGCONT && (p->p_flag&STRC) == 0)
				u.u_cursig = 0;
#endif	/* OSF1_SERVER */
			p->p_cursig = 0;
			continue;

		default:
			SIG_DEBUG(3, ("issig[%d]: action(%d) is SIG_CATCH.\n", u.u_procp->p_pid, sig));
			/*
			 * This signal has an action, let
			 * psig process it.
			 */
			goto send;
		}
		/*NOTREACHED*/
	}
	/*
	 * Didn't find a signal to send.
	 */
	p->p_cursig = 0;
#ifdef	OSF1_SERVER
#else
	u.u_cursig = 0;
#endif
	sig_unlock(p);
	SIG_DEBUG(1, ("issig[%d]: returning with no signals.\n", u.u_procp->p_pid));
	return (0);

send:
	/*
	 * Let psig process the signal.
	 */
	sig_unlock(p);
	SIG_DEBUG(1, ("issig[%d]: returning with signal %d.\n", u.u_procp->p_pid, sig));
	return (sig);
}

/*
 * Perform the action specified by
 * the current signal.
 * The usual sequence is:
 *	if (issig())
 *		psig();
 * The signal bit has already been cleared by issig,
 * and the current signal number stored in p->p_cursig.
 */
psig()
{
	psig1(FALSE);
}

psig1(force_delivery)
{
	register uthread_t uth = &u;
	register struct proc *p = uth->u_procp;
	register int sig;
	int mask, returnmask;
	register sig_t action;
	kern_return_t rc;
	unsigned int pc;

	/*
	 *	This must be called on master cpu
	 */
#ifdef	OSF1_SERVER
	if (u.u_master_lock == 0)
#else
	if (cpu_number() != master_cpu)
#endif
		panic("psig not on master");

	SIG_DEBUG(1, ("psig[%d]: entering with p_cursig=%d, p_sig=0x%08x.\n", p->p_pid, p->p_cursig, p->p_sig));
	/*      Keep repeating this as long as there are signals that
	 *	need delivering
	 */
	do {
		/*
		 *	Try for the signal lock.  Don't proceed further if we
		 *	are already supposed to halt.
		 */
		sig_lock(p);
		sig = p->p_cursig;
		SIG_DEBUG(2, ("psig[%d]: got signal %d.\n", p->p_pid, sig));
		mask = sigmask(sig);
		
		/*
		 *	If another thread got here first (sig == 0) or this is
		 *	a thread signal for another thread, bail out.
		 */
#ifdef	OSF1_SERVER
		if (sig == 0)
#else
		if ((sig == 0) || ((mask & threadmask) && (sig != u.u_cursig))) 
#endif
		{
			SIG_DEBUG(2, ("psig[%d]: no sig. redo issig().\n", p->p_pid));
			sig_unlock(p);
			continue;
		}
		
		action = signal_disposition(sig);
		if (action != SIG_DFL) {
#ifdef	OSF1_SERVER
			thread_array_t thread_table;
			mach_port_t signal_thread;
			mach_msg_type_number_t table_size;
			int i;
#endif	/* OSF1_SERVER */

			if (action == SIG_IGN || (p->p_sigmask & mask)) {
				/* panic("psig action"); */
				p->p_sig |= mask;
				p->p_cursig = 0;
				SIG_DEBUG(2, ("psig[%d]: p_cursig=%d is no masked or ignored.\n", p->p_pid, sig));
				sig_unlock(p);
				continue;
			}
			SIG_DEBUG(3, ("psig[%d]: action for sig %d is SIG_CATCH.\n", p->p_pid, sig));
#ifdef	OSF1_SERVER
			/*
			 * If force_delivery is false, don't poke the emulator
			 * (to make it gather the signal we have for it).  Even
			 * if force_delivery is true, we *still* don't need to
			 * poke the emulator if there is already another thread
			 * in the PM that is going to be returning to the
			 * emulator anyway.  This means we must exclude the
			 * count of side threads, since they do *not* return
			 * to the emulator.  If such a thread exists then *it*
			 * will deliver the signal back to the emulator (via
			 * end_server_op or equiv).
			 */
			PROC_LOCK(p);
			if (force_delivery == FALSE ||
					p->p_ref_count - p->p_side_ref_count
					> 1) {
				PROC_UNLOCK(p);

				sig_unlock(p);

				SIG_DEBUG(2, ("psig[%d]: sig %d ignored " \
						"Force=%d PMcnt=%d side=%d " \
						"p_cursig=%d.\n", \
						p->p_pid, sig, force_delivery, \
						p->p_ref_count, \
						p->p_side_ref_count, \
						p->p_cursig));
				break;
			} 

			/*
			 * If we need to, forward the interrupt to the emulator.
			 * ONLY if MAP_UAREA is set can we (cheaply) tell if
			 * the process is in the emulator.
			 */
#if MAP_UAREA
			if (p->p_shared_rw->us_in_emulator) {
				PROC_UNLOCK(p);
				SIG_DEBUG(1,
					(">>>> Forward 1: callback port=%x\n",
					     p->p_callback));
				uemul_forward_signal(p->p_callback,
					(u.u_sigintr & sigmask(p->p_cursig)));

				sig_unlock(p);
				break;
			}
#endif
			PROC_UNLOCK(p);
			(void) task_suspend(p->p_task);
			SIG_DEBUG(3, ("psig[%d]: task suspended.\n", p->p_pid));
			/*
			 * For OSF1/AD: should check for table_size < 2.
			 * Should proc_exit() if not KERN_INVALID_ARGUMENT.
			 */
			if (((rc = task_threads(p->p_task, &thread_table,
						&table_size)) != KERN_SUCCESS)
			    || (table_size <= 0)) {
				if (rc == KERN_INVALID_ARGUMENT)
					printf("psig: no more task for proc %d(%s)\n", p->p_pid, p->p_utask.uu_comm);
				else 
					printf("psig: task_threads() for proc %d(%s) returned 0x%x.\n", p->p_pid, p->p_utask.uu_comm, rc);
				(void) task_resume(p->p_task);

				sig_unlock(p);
				break;
			}

			signal_thread = thread_table[0];
			if (signal_thread == p->p_callback_thread) {
				if (table_size <= 1) {
					printf("psig: no more threads in proc %d(%s)\n", p->p_pid, p->p_utask.uu_comm);
					/* XXX: should Kill the process */
					(void) task_resume(p->p_task);

					sig_unlock(p);
					break;
				}
				signal_thread = thread_table[1];
			}

			/* free task_threads state 'cept send right we need */
			for (i = 0; i < table_size; i++) {
				if (thread_table[i] != signal_thread)
					(void) mach_port_deallocate(
							mach_task_self(),
							thread_table[i]);
			}
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t)thread_table, 
					     table_size * 
					     sizeof(*thread_table));

			/*
			 * If the process is in the emulator,
			 * forward the interrupt
			 */
			rc = get_thread_pc(signal_thread, &pc);
			if (rc == KERN_SUCCESS &&
			    pc >= EMULATOR_BASE && pc <= EMULATOR_END)
			{
				SIG_DEBUG(3,("psig[%d]: thread in emulator\n",
					      p->p_pid));

				(void) mach_port_deallocate(mach_task_self(),
							    signal_thread);
				(void) task_resume(p->p_task);

				SIG_DEBUG(1,(">>>> Forw 2: callback port=%x\n",
					     p->p_callback));
				uemul_forward_signal(p->p_callback,
					(u.u_sigintr & sigmask(p->p_cursig)));

				sig_unlock(p);
				break;
			}

			SIG_DEBUG(2, ("psig[%d]: will force delivery of sig %d.\n", p->p_pid, sig));
			if (thread_abort(signal_thread) != KERN_SUCCESS)
				panic("psig: thread_abort()");
			SIG_DEBUG(3, ("psig[%d]: thread aborted.\n", p->p_pid));
#endif
			/*
			 * Set the new mask value and also defer further
			 * occurences of this signal (unless we're simulating
			 * the old signal facilities).
			 *
			 * Special case: user has done a sigpause.  Here the
			 * current mask is not of interest, but rather the
			 * mask from before the sigpause is what we want 
			 * restored after the signal processing is completed.
			 */
#ifdef	OSF1_SERVER
/* avoid deadlock between softclock, network or devices and bsd_issig_psig */
#else	/* OSF1_SERVER */
			(void) splhigh();
#endif	/* OSF1_SERVER */
			if (p->p_flag & SOUSIG) {
				SIG_DEBUG(3, ("psig[%d]: S5 signal %d : not masked inside handler.\n", p->p_pid, sig));
				if (sig != SIGILL && sig != SIGTRAP) {
					signal_disposition(sig) = SIG_DFL;
					p->p_sigcatch &= ~mask;
					SIG_DEBUG(3, ("psig[%d]: reset S5 signal %d to SIG_DFL.\n", p->p_pid, sig));
				}
				mask = 0;
			}
			if (p->p_flag & SOMASK) {
				SIG_DEBUG(3, ("psig[%d]: was in sigpause: use saved mask.\n", p->p_pid));
				returnmask = u.u_oldmask;
				p->p_flag &= ~SOMASK;
			} else
				returnmask = p->p_sigmask;
			p->p_sigmask |= u.u_sigmask[sig] | mask;
			/*
			 *	Fix up the signal state and unlock before
			 *	we send the signal.
			 */
			p->p_cursig = 0;
			if (sigmask(sig) & threadmask)
				u.u_cursig = 0;
			sig_unlock(p);
#ifdef	OSF1_SERVER
#else	/* OSF1_SERVER */
			(void) spl0();
#endif	/* OSF1_SERVER */
			u.u_ru.ru_nsignals++;
#ifdef	OSF1_SERVER
			SIG_DEBUG(0, ("psig[%d]: calling sendsig(p=0x%x, thread=0x%x, action=0x%x, sig=%d, code=%d, returnmask=0x%08x).\n", p->p_pid, p, signal_thread, action, sig, 0, returnmask));
			sendsig(p, signal_thread, action, sig, 0, returnmask);
			SIG_DEBUG(2, ("psig[%d]: back from sendsig(sig=%d).\n", p->p_pid, sig));

			(void) mach_port_deallocate(mach_task_self(),
						    signal_thread);
			(void) task_resume(p->p_task);
#else
			sendsig(action, sig, returnmask);
#endif
			continue;
		}
		/*
		 *  The new signal code for cultiple threads makes it
		 *  possible for a multi-threaded task to get here (a
		 *  thread that didn't originally process a "stop"
		 *  signal notices that cursig is set), therefore, we
		 *  must handle this.
		 */
		switch (sig) {
		case SIGTSTP:
		case SIGTTIN:
		case SIGTTOU:
		case SIGSTOP:
#ifdef NX
                case SIGUSR2:

                        if (sig == SIGUSR2 && !nx_application(p)) {
                        	break;
                        } else if ( sig == SIGUSR2 && nx_application(p)) {
				p->p_cursig = SIGUSR2;
			}
#endif /* NX */
			SIG_DEBUG(2, ("psig[%d]: got a stop signal (%d) with SIG_DFL action.\n", p->p_pid, sig));

/* can stop now! */
#ifndef NX
			p->p_stopsig = sig;
			p->p_cursig = 0;
#endif /* NX */
			stop(p);
#ifdef NX
			p->p_stopsig = sig;
			p->p_cursig = 0;
#endif /* NX */

		/*FALLTHRU*/
#ifdef	TNC
		case SIGMIGRATE:
#endif
			sig_unlock(p);
			continue;
		}

		/*
		 * SIG_DFL cases that cause process termination
		 */
		U_HANDY_LOCK();
		u.u_acflag.fi_flag |= AXSIG;
		U_HANDY_UNLOCK();
		switch (sig) {
		case SIGILL:
		case SIGIOT:
		case SIGBUS:
		case SIGQUIT:
		case SIGTRAP:
		case SIGEMT:
		case SIGFPE:
		case SIGSEGV:
		case SIGSYS:
#ifdef	OSF1_SERVER
			SIG_DEBUG(3, ("psig[%d]: SIG_DFL for sig=%d is to exit"
					" with core.\n", p->p_pid, sig));
			sig_unlock(p);
#else
			u.u_sig = sig;
			/*
			 *	Indicate that we are about to exit.
			 *	disables all further signal processing for p.
			 */
			sig_lock_to_exit(p);
#endif
			/* signal that exit should call core */
			sig |= WCOREFLAG;
			break;
#ifdef PARACORE
		case SIGKILL: {
			if ( p->p_utask.uu_core_action != CORE_ACTION_CONT ) {
				/*
				 * Indicate that we want to dump core.
				 */
				sig |= WCOREFLAG;
			}
#ifdef OSF1_SERVER
			sig_unlock(p);
#else /* OSF1_SERVER */
			sig_lock_to_exit(p);
#endif /* OSF1_SERVER */
			break;
		}
#endif /* PARACORE */
		default:
#ifdef	OSF1_SERVER
			SIG_DEBUG(3, ("psig[%d]: SIG_DFL for sig=%d is to exit"
					" without core.\n",
					p->p_pid, sig));
			sig_unlock(p);
			break;
#else
			sig_lock_to_exit(p);
#endif
		}

#ifdef TNC
		/*
		 * Perform exit processing now.  Callers to psig expect a
		 * proc to be gone if a fatal signal is received.
		 */
		TNC_unix_release();
		(void) VPOP_EXIT(p->p_vproc, W_EXITCODE(0,sig));
		TNC_unix_master();
#else	/* TNC */
		(void) VPOP_EXIT(p->p_vproc, W_EXITCODE(0,sig));
#endif	/* TNC */
		break;
	}
	while (ISSIG (p));
	SIG_DEBUG(1, ("psig[%d]: exiting. p_cursig=%d, p_sig=0x%08x.\n",
			p->p_pid, p->p_cursig, p->p_sig));
}

#ifdef	i386
#define USER_STACK_POINTER	UESP
#endif
#ifdef	mips
#define USER_STACK_POINTER	EF_SP
#endif
#if	!defined(i386) && !defined(mips)
#define	USER_STACK_POINTER	SP
#endif

/*
 * Create a core image on the file "core"
 * If you are looking for protection glitches,
 * there are probably a wealth of them here
 * when this occurs to a suid command.
 *
 * It writes UPAGES block of the
 * user.h area followed by the entire
 * data+stack segments.
 *
 * Return:
 *	-1		if we did not attempt do dump core
 *	0		if we successfully dumped core
 *	>0 (errno)	if we failed dumping core
 */

int coreprint = 0;	/* XXX Just for debugging */

core()
{
#ifdef	OSF1_ADFS
	mach_port_t	vp;
#else
	register struct vnode *vp;
#endif
	struct vattr vattr;
	int error;
	register struct nameidata *ndp = &u.u_nd;
	vm_size_t	stack_size;
	vm_offset_t	stack_addr;
	enum vtype	type;
	struct ucred	*cred;
	struct proc	*p;
#ifdef	multimax
	struct ptrace_user	*fake_uarea;
#else	/* multimax */
#ifndef	sun
	struct user	*fake_uarea;
#endif
#endif	/* multimax */
#ifdef	sun
	off_t		offset = 0;
#endif
#ifdef	OSF1_SERVER
	caddr_t		fake_kernel_stack;
	int		fake_kernel_stack_size;
#endif

	ASSERT(syscall_on_master());
	p = u.u_procp;
	if (coreprint)	/* XXX Just for debugging */
		printf("(%s) pid %d uid %d dumped core\n",
			u.u_comm, u.u_procp -> p_pid, p->p_ruid);

	PROC_LOCK(p);
	if (p->p_flag & SXONLY) {
		PROC_UNLOCK(p);
		return -1;
	}
	crfree(u.u_cred);
	u.u_cred = p->p_rcred;
	crhold(p->p_rcred);
	p->p_rcred->cr_uid = p->p_ruid;
	p->p_rcred->cr_gid = p->p_rgid;
	PROC_UNLOCK(p);

	if (ctob(UPAGES+u.u_dsize+u.u_ssize) >=
	    u.u_rlimit[RLIMIT_CORE].rlim_cur)
		return -1;

	/*
	 *	Make sure all registers, etc. are in pcb so they get
	 *	into core file.
	 */
#if	!defined(i386) && !defined(i860)
	pcb_synch(current_thread());
#endif	/* defined(i386) || defined(i860) */

	ndp->ni_segflg = UIO_SYSSPACE;
	ndp->ni_dirp = "core";

#ifndef	OSF1_ADFS
#if     SEC_BASE
	if (error = vn_open(ndp, FCREAT|FWRITE, SEC_CORE_MODE))
#else
	if (error = vn_open(ndp, FCREAT|FWRITE, 0644))
#endif
		return (error);
	cred = u.u_cred;
	vp = ndp->ni_vp;
	BM(VN_LOCK(vp));
	type = vp->v_type;
	BM(VN_UNLOCK(vp));
	if (type != VREG) {
		error = EFAULT;
		goto out;
	}
	VOP_GETATTR(vp, &vattr, u.u_cred, error);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		goto out;
	}
	/*
	 * 4.4 has some code here ifdef MMAP to unmap devices.
	 * We have not included that code.
	 */
	vattr_null(&vattr);
	vattr.va_size = 0;
	VOP_SETATTR(vp, &vattr, cred, error);
#else	/* OSF1_ADFS */
#if     SEC_BASE
	error = remote_vnopen(ndp, FCREAT|FWRITE, SEC_CORE_MODE, &vp, &type);
#else
	error = remote_vnopen(ndp, FCREAT|FWRITE, 0644, &vp, &type);
#endif
	if (error) {
		if (error != EACCES)
			return error;
		else
			return -1;
	}
	cred = u.u_cred;
	if (type != VREG) {
		error = EFAULT;
		goto out;
	}
	error = remote_getattr(vp, &vattr);
	if (error || vattr.va_nlink != 1) {
		if (error == 0)
			error = EFAULT;
		goto out;
	}
	/*
	 * 4.4 has some code here ifdef MMAP to unmap devices.
	 * We have not included that code.
	 */
	vattr_null(&vattr);
	vattr.va_size = 0;
	error = remote_setattr(vp, &vattr);
#endif	/* OSF1_ADFS */

	U_HANDY_LOCK();
	u.u_acflag.fi_flag |= ACORE;
	U_HANDY_UNLOCK();
	/*
	 *	Fool Unix about the size of the stack.  Then
	 *	let it dump the stack in the way it normally does
	 *	below.
	 */

#ifdef	OSF1_SERVER
	machine_core_info(p, &stack_addr, &fake_kernel_stack,
			  &fake_kernel_stack_size);
	if (stack_addr == NULL)		/* Should not happen. */
		stack_addr = (vm_offset_t)USRSTACK;	/* XXX */
#else
	stack_addr = u.u_ar0[USER_STACK_POINTER];
#endif
	stack_addr = trunc_page(stack_addr);
	stack_size = (vm_offset_t)USRSTACK - stack_addr;
	stack_size = round_page(stack_size);
	u.u_ssize = btoc(stack_size);
#ifdef	sun
	/*
	 *	Write out the core structure, data, stack, and then
	 *	the u-area for the Sun
	 */
	sun_core(vp, &offset);
#else	/* sun*/
	/*
	 *	Allocate fake uarea and fake the required fields
	 *	to keep the debuggers happy.
	 */
#ifdef	multimax
	fake_uarea = (struct ptrace_user *)
		kmem_alloc(kernel_map, sizeof *fake_uarea);
	if (fake_uarea == 0) {
		error = ENOMEM;
		goto out;
	}
	mmax_fake_ptrace_u(fake_uarea, current_thread());
#else	/* multimax */
#ifdef OSF1_SERVER
	fake_uarea = (struct user *) kalloc(sizeof *fake_uarea);
#else
	fake_uarea = (struct user *)
		kmem_alloc(kernel_map, sizeof *fake_uarea);
#endif
	if (fake_uarea == 0) {
		error = ENOMEM;
		goto out;
	}
	fake_u(fake_uarea, p, current_thread());
#endif	/* multimax*/

	/*
	 *	MACH breaks conventional debuggers because the kernel
	 *	stack is no longer at the top of memory.  Dump the forged
	 *	uarea where it is expected, followed by the current
	 *	kernel stack at the end of the UPAGES where the u-area
	 *	used to be.  Since the thread uarea contains the thread pcb
	 *	which contains the kernel stack offset, there is enough
	 *	information to decode what's going on.  This assumes that a
	 *	struct user, and a kernel stack all fit in UPAGES.
	 */
#ifdef	multimax
	/*
	 *	Multimax does this differently -- just dump the faked
	 *	uarea followed by data and user stack, that's all.
	 */
#endif
#ifdef	ibmrt
	/*
	 *	The RT's kernel stack is BELOW the U area, unlike everyone
	 *	else.
	 */
    {
	u_int k_max = ctob(UPAGES) - sizeof(struct user);
	if (k_max >= KERNEL_STACK_SIZE) {
	    error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)(current_thread()->kernel_stack), KERNEL_STACK_SIZE,
		(off_t)(k_max - KERNEL_STACK_SIZE), UIO_SYSSPACE,
		IO_UNIT, ndp->ni_cred, (int *)0);
	}
	else {
	    u_int k_diff = KERNEL_STACK_SIZE - k_max;
	    error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)(current_thread()->kernel_stack + k_diff),
		KERNEL_STACK_SIZE - k_diff,
		(off_t) 0, UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *)0);
	}
    }
	if (error == 0)
	    error = vn_rdwr(UIO_WRITE, vp,
	    	(caddr_t)fake_uarea, sizeof(struct user),
		(off_t)(ctob(UPAGES)-sizeof(struct user)),
		UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *) 0);
#else	/* ibmrt */
#ifdef	multimax
 	error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)fake_uarea, sizeof(struct ptrace_user),
		(off_t)0, UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *)0);
#else	/* multimax */
#ifdef	OSF1_ADFS
 	error = remote_vnrdwr(UIO_WRITE, vp, (caddr_t)fake_uarea, 
			      sizeof(struct user), (off_t)0,
			      IO_UNIT,  (int *)0);
#else
 	error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)fake_uarea, sizeof(struct user),
		(off_t)0, UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *)0);
#endif	/* OSF1_ADFS */
#ifndef OSF1_SERVER
	if (error == 0) {
		off_t off;
		caddr_t start;
		int length;

		/* if kernel stack will fit, right-adjust it in UPAGES */
		
		if ((ctob(UPAGES) - KERNEL_STACK_SIZE) >= sizeof(struct user)) {
			off = ctob(UPAGES) - KERNEL_STACK_SIZE;
			start = (caddr_t)(current_thread()->kernel_stack);
			length = KERNEL_STACK_SIZE;
		}
		else {
			/*
			 * whole kernel stack won't fit,
			 * make sure we get the upper end
			 */
			length = ctob(UPAGES) - sizeof(struct user);
			start = (caddr_t)((current_thread()->kernel_stack)
					  + KERNEL_STACK_SIZE - length);
			off = sizeof(struct user);
		}			
		error = vn_rdwr(UIO_WRITE, vp,
				start, length,
				off, UIO_SYSSPACE, IO_UNIT,
				ndp->ni_cred, (int *)0);
	}
#else	/* OSF1_SERVER */
	if (error == 0 && fake_kernel_stack) {
		ASSERT(fake_kernel_stack_size < ctob(UPAGES));
#ifdef	OSF1_ADFS
		error = remote_vnrdwr(UIO_WRITE, vp, fake_kernel_stack,
				      fake_kernel_stack_size,
				      ctob(UPAGES) - fake_kernel_stack_size,
				      IO_UNIT, (int *)0);
#else
		error = vn_rdwr(UIO_WRITE, vp, fake_kernel_stack,
				fake_kernel_stack_size,
				ctob(UPAGES) - fake_kernel_stack_size,
				UIO_SYSSPACE, IO_UNIT, ndp->ni_cred, (int *)0);
#endif	/* OSF1_ADFS */
		kfree(fake_kernel_stack, fake_kernel_stack_size);
	}
#endif	/* OSF1_SERVER */
#endif	/* multimax */
#endif	/* ibmrt */
#ifdef OSF1_SERVER
	kfree(fake_uarea, sizeof *fake_uarea);
#else
	kmem_free(kernel_map, fake_uarea, sizeof *fake_uarea);
#endif
#endif	/* sun */
#ifdef	multimax
 	if (error == 0)
 		error = vn_rdwr(UIO_WRITE, vp,
		    (caddr_t)u.u_data_start, (int)ctob(u.u_dsize),
		    sizeof(struct ptrace_user),
		    UIO_USERSPACE, IO_UNIT,
		    ndp->ni_cred, (int *)0);
 	if (error == 0)
 		error = vn_rdwr(UIO_WRITE, vp,
		    (caddr_t)stack_addr, (int)stack_size,
		    sizeof(struct ptrace_user)+ctob(u.u_dsize),
		    UIO_USERSPACE, IO_UNIT,
		    ndp->ni_cred, (int *)0);
#else	/* multimax */
	if (error == 0) {
		caddr_t base = (caddr_t)u.u_data_start;
		error = core_write(vp, ndp->ni_cred, base, 
		    (int)ctob(u.u_dsize),
#ifdef	sun
		    offset);
#else
		    (off_t)ctob(UPAGES));
#endif
	}
#ifdef	sun
	offset += ctob(u.u_dsize);
#endif
	if (error == 0) {
		caddr_t base =	(caddr_t)stack_addr;
		int size = 	(int)stack_size;
		error = core_write(vp, ndp->ni_cred, base, size,
#ifdef	sun
		    offset);
#else
		    (off_t)ctob(UPAGES)+ctob(u.u_dsize));
#endif
	}
#ifdef	sun
	offset += ctob(u.u_ssize);
#endif
#endif	/* multimax */
#ifdef	sun
	/*
	 *	Write u-area last
	 */
	error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)(current_thread()->u_address.uthread),
		sizeof(struct user), offset,
		UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *)0);
	if (error == 0)
	    error = vn_rdwr(UIO_WRITE, vp,
		(caddr_t)(current_thread()->kernel_stack), KERNEL_STACK_SIZE,
		(off_t)(offset + ctob(UPAGES) - KERNEL_STACK_SIZE),
		UIO_SYSSPACE, IO_UNIT,
		ndp->ni_cred, (int *)0);
#endif
out:
#ifdef	OSF1_ADFS
	mach_port_deallocate(mach_task_self(), vp);
#else
	vrele(vp);
#endif
	return (error);
}


#ifndef multimax

/* Write a section of the task's address space to the file in vp.  We
   should be able to use vn_rdwr with UIO_USERSPACE to do this, but that
   is broken under the OSF/1 server.
   When we read a page from the process, we call it into existence even
   if it was previously just demand-zero.  Since we don't want to need
   backing store for a possibly huge virtual address space, we examine a
   block at a time and deallocate each block from the process after using
   it.  We also optimise space in the core file by creating holes for
   sufficiently large patches of zero bytes.  */
int
core_write(vp, cred, base, len, offset)
#ifdef	OSF1_ADFS
	mach_port_t vp;
#else
	struct vnode *vp;
#endif
	struct ucred *cred;
	caddr_t base;
	int len;
	off_t offset;
{
	int start, error = 0;
	char *kbase;
	vm_size_t klen;
#define COREBLOCKSIZE 32768
	assert(round_page(COREBLOCKSIZE) == COREBLOCKSIZE);

	/*
	 * XXX - need to release the master lock when accessing 
	 * the user's address space, because we may activate
	 * a page fault, and produce a dead lock with the vnode
	 * pager...
	 */
	if (u.u_master_lock)
		master_unlock();
	for (start = 0; start < len; start += COREBLOCKSIZE) {
	    vm_address_t bottom, top;
	    int size = len - start;
	    if (size > COREBLOCKSIZE)
		size = COREBLOCKSIZE;
	    bottom = (vm_address_t)(base + start);
	    top = bottom + size;

	    if (vm_read(u.u_procp->p_task, bottom,
			(vm_size_t)size, (vm_offset_t *)&kbase,
			(mach_msg_type_number_t *) &klen) != KERN_SUCCESS) {
		error = EFAULT;
		break;
	    }
	    /* If this is not the last block and it is entirely zero, write
	       nothing and allow a hole in the core file to take its place. */
	    if (start + COREBLOCKSIZE >= len || !isempty(kbase, klen)) {
	      if (u.u_master_lock)
		master_lock();
#ifdef	OSF1_ADFS
	      error = remote_vnrdwr(UIO_WRITE, vp, kbase, size, offset + start,
				    IO_UNIT, (int *)0);
#else
	      error = vn_rdwr(UIO_WRITE, vp, kbase, size, offset + start,
			      UIO_SYSSPACE, IO_UNIT, cred, (int *)0);
#endif	/* OSF1_ADFS */
	      if (u.u_master_lock)
		master_unlock();
	    }
	    /* Avoid deallocating emulator space too soon */
#ifdef CORE_WRITE_DEALLOCATE /* causes bug PTS #9266 */
	    if (  !(bottom >= (vm_address_t)EMULATOR_BASE &&
		    bottom <  (vm_address_t)EMULATOR_END  ||
		    top    >  (vm_address_t)EMULATOR_BASE &&
		    top    <= (vm_address_t)EMULATOR_END)) {
		(void)vm_deallocate(u.u_procp->p_task, bottom, size);
	    }
#endif /* CORE_WRITE_DEALLOCATE */
	    (void) vm_deallocate(mach_task_self(), (vm_address_t) kbase, klen);
	    if (error)
		break;
	}
	if (u.u_master_lock)
		master_lock();

	return error;
}


/* Return true only if len bytes starting at base are all 0. */
boolean_t
isempty(base, len)
char *base;
int len;
{
	int *p = (int *) base;
	assert(len % sizeof(int) == 0);
	for (len /= sizeof(int); len > 0; len--)
	    if (*p++)
		return FALSE;
	return TRUE;
}
#endif	/* multimax */

pproc_cansignal(p, q, signo)
register struct proc *p;
register struct proc *q;
int signo;
{
	register struct ucred	*pcr;
	pid_t 			psid;
	pid_t 			qsid;
	uid_t			pruid, qruid, qsvuid;

	PROC_LOCK(p);
	pcr = p->p_rcred;
	psid = p->p_sid;
	pruid = p->p_ruid;
	PROC_UNLOCK(p);
	PROC_LOCK(q);
	qsvuid = q->p_svuid;
	qsid = q->p_sid;
	qruid = q->p_ruid;
	PROC_UNLOCK(q);
	/*
	 * N.B.  Session check should probably be under some
	 * as-yet-undefined pgrp lock.			XXXX
	 */
	return(cansignal(pcr->cr_uid,pruid,psid,qsvuid,qruid,qsid,signo));
}

/*
 * The following function returns process attributes for proc p,
 * or for the current process context if p is NULL.
 */
int
pproc_get_attr(p,pid,ppid,pgid,sid,has_priv,uid,ruid,has_sctty,has_execd) 
	struct proc *p;
	pid_t *pid, *ppid, *pgid, *sid;	
	int *has_priv;
	uid_t *uid, *ruid;
	int *has_sctty, *has_execd;
{
	if (p == NULL)
		p = u.u_procp;

	PROC_LOCK(p);
	if (pid)
		*pid = p->p_pid;
	if (ppid)
		*ppid = p->p_ppid;
	if (pgid)
		*pgid = p->p_pgid;
	if (sid)
		*sid = p->p_sid;

	if (uid)
		*uid = p->p_rcred->cr_uid;
	if (ruid)
		*ruid = p->p_ruid;
	if (has_priv)
		*has_priv = suser(u.u_cred,&u.u_acflag) == 0;
	if (has_sctty) 
		*has_sctty = p->p_flag&SCTTY;
	if (has_execd) 
		*has_execd = p->p_flag&SEXEC;

	PROC_UNLOCK(p);
	return(0);
}

/*
 * The following function sets some attributes for proc p or for
 * the current process context if p is NULL.
 */
int
pproc_set_attr(p, v, pid, ppid, pgid, sid, has_sctty) 
	struct proc *p;
	struct vproc *v;
	pid_t *pid, *ppid, *pgid, *sid;	
	int *has_sctty;
{
	PROC_LOCK(p);
	if (v)
		p->p_vproc = v;
	if (pid)
		p->p_pid = *pid;
	if (ppid)
		p->p_ppid = *ppid;
	if (pgid)
		p->p_pgid = *pgid;
	if (sid)
		p->p_sid = *sid;
	if (has_sctty) {
		if (*has_sctty)
			p->p_flag |= SCTTY;
		else
			p->p_flag &= ~SCTTY;
	}

	/* zombie processes have already had their credentials deallocated */
	if ((pgid || sid) && p->p_stat != SZOMB) {
		credentials_set_state(p->p_cred, &p->p_pgid, &p->p_sid, 
				(struct ucred *) NULL, (int *) NULL,
				(unsigned int *) NULL,
				(unsigned int *) NULL,
				(uid_t *) NULL,
				(uid_t *) NULL,
				(task_t *) NULL);
	}

	PROC_UNLOCK(p);
	return(0);
}

cansignal(puid, pruid, psid, qsvuid, qruid, qsid, signo)
	uid_t puid, pruid; 
	uid_t qsvuid, qruid; 
	pid_t psid, qsid;
	int signo;

{
#if     SEC_BASE
	return(pruid == qruid ||
	       puid == qruid ||
	       pruid == qsvuid ||
	       puid == qsvuid ||
	       ((signo) == SIGCONT &&
	       psid == qsid) ||
	       privileged(SEC_KILL, 0));
#else
	return(puid == 0 ||
	       pruid == qruid ||
	       puid == qruid ||
	       pruid == qsvuid ||
	       puid == qsvuid ||
	       ((signo) == SIGCONT &&
	       psid == qsid));
#endif
}

/*
 * Some system calls need to ensure that no signals have arrived between
 * the time a user task makes the call and important synchronization code
 * at the start of the syscall processing takes place.
 */
int
pproc_signal_received(
	struct proc *p)
{
	return( p->p_cursig || HAVE_SIGNALS(p) );
}

/*
 * Allows a thread to delay for a specified number of seconds.  The
 * routine performs the required ux_server_thread_blocking/unblocking
 * calls.
 */
void
pproc_timed_delay(
	int nsecs)
{
	extern void mach_init_sleep();

	if (nsecs == 0)			/* handle degenerate case */
		return;
	ux_server_thread_blocking();
	mach_init_sleep(nsecs);		/* uses mach_msg() for timer */
	ux_server_thread_unblocking();
}
