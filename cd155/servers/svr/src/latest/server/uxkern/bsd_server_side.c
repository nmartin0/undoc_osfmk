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
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This file was extended and modified by the Center for High Performance
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: bsd_server_side.c,v $
 * Revision 1.36  1995/02/18  01:40:56  yazz
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
 * Revision 1.35  1995/02/10  23:56:13  stans
 *  'lint' picking with typedefs for a clean compile.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:12424
 *  Testing: WW05 sats
 *
 * Revision 1.34  1995/02/01  22:17:00  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.33  1994/11/18  20:47:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.32  1994/11/04  18:25:44  toman
 * Corrected CVS comments.
 *
 * Revision 1.31  94/11/04  18:23:08  toman
 * Added routines bsd_orecv_short() and bsd_orecv_long().
 * 
 *  Reviewer: John Litvin, Bob Yasi
 *  Risk: med
 *  Benefit or PTS #: 9422
 *  Testing: Lachman t_recv, TCP/IP EAT
 *  Module(s): emulator/fsvr_user_side.c
 * 	    server/conf/syscalls.master
 * 	    server/uxkern/bsd_1.defs
 * 	    server/uxkern/bsd_server_side.c
 * 
 * 
 * Revision 1.30  94/08/31  22:47:34  mtm
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
 * Revision 1.28.2.1  1994/08/17  01:08:41  jlitvin
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
 * Revision 1.28  1994/06/22  18:30:15  jlitvin
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
 * Revision 1.27  1994/05/25  01:53:31  yazz
 *  Reviewer: Chris Peak
 *  Risk: Lo
 *  Benefit or PTS #: 9497
 *  Testing: basic; no specific testcase
 *  Module(s): server/uxkern/bsd_server_side.c
 * Make routine sbsd_sel_poll_reply() not take the master lock (set serial to
 * FALSE instead of to TRUE).
 *
 * Revision 1.26  1994/05/09  04:13:50  yazz
 * Corrected CVS comments.
 *
 * Revision 1.25  1994/05/04  22:47:37  yazz
 * Merged 1.19.2.7 fix from R1.2 branch into main stem.
 *
 * Revision 1.19.2.7  1994/05/04  18:44:06  yazz
 *  Checkin for: mjl
 *  Reviewer: Bob Yasi, Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: 9217
 *  Testing: EATs for tcp-ip, NFS, X, XIPD; also the pvm/electrond testcase
 *  Module(s): server/uxkern/bsd_server_side.c
 *
 * Any non-zero return from the sbsd_sel_poll_reply() routine causes the
 * caller, ux_server_loop(), to destroy the port rights in the incoming
 * message, and that caused a "missing send right" panic in fp_unref_port().
 * Returns are now all 0, and this unintended destruction is prevented.
 *
 * Revision 1.24  1994/05/04  21:51:04  mjl
 * TNC select rewrite.  Initialize uthread select fields prior to calling
 * FOP_SELECT().  In sbsd_sel_poll_reply(), translate select id to get
 * drp name.  Also, if drp death is detected during a rescan, add a
 * FOP_SELECT() call to clean up remote secondaries.
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
 * Revision 1.23  1994/05/04  19:20:26  yazz
 * Merge 1.19.2.6 R1.2 checkin into main stem.
 *
 * Revision 1.19.2.6  1994/05/04  18:11:52  yazz
 *  Reviewer: Charlie Johnson
 *  Risk: Lo
 *  Benefit or PTS #: 9236 C-0
 *  Testing: testcase passes, plus EATS ipc, os_interfaces and prof.
 *  Module(s): server/uxkern/bsd_server_side.c
 *
 * Revision 1.22  1994/04/13  19:13:08  cfj
 * Merge revision 1.19.2.1 from R1_2.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.21  1994/02/16  18:53:28  stefan
 * Removed some junk code (SLL-specific).
 *
 * Revision 1.20  1994/01/12  17:47:28  jlitvin
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
 * Revision 1.19.2.1  1994/04/13  19:10:37  cfj
 * Initialize all out params from bsd_fork() to something rational.
 * Specfically, set any mach ports to MACH_PORT_NULL.
 *
 *  Reviewer:jlitvin
 *  Risk:L
 *  Benefit or PTS #:8870
 *  Testing:VSX EAT
 *  Module(s):server/uxkern/bsd_server_side.c
 *
 * Revision 1.19  1993/10/29  11:56:02  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.18  1993/08/11  18:41:52  stefan
 * Fixed a bug in SLL that could make rfork(localnode) actually
 * go to a remote node.
 *
 * Revision 1.17  1993/07/14  18:39:47  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.6  1993/07/01  20:58:34  cfj
 * Adding new code from vendor
 *
 * Revision 1.16  1993/06/23  18:23:32  stefan
 * For better modularity moved some code to function sll_fast_node
 * in file sll/sll.c.
 *
 * Revision 1.15  1993/06/21  13:54:31  stefan
 * Made modifications so that a server that is compiled with -sll works with
 * an emulator that is compiled with -DSLL (and vice versa).
 *
 * Revision 1.14  1993/06/01  16:14:10  stefan
 * Renamed sll/sll.h to sll/sll_types.h.
 *
 * Revision 1.13  1993/05/13  09:17:22  stefan
 * Integrated static load leveling support.
 *
 * Revision 1.12  1993/05/07  18:12:33  nandy
 * Fixed a merge bug.
 *
 * Revision 1.11  1993/05/06  19:25:04  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.10  1993/04/03  03:11:13  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.1.4  1993/05/03  17:50:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.9  1993/03/25  23:30:45  cfj
 * T9 Merge.
 *
 * Revision 1.5.4.4  1993/03/24  23:43:18  cfj
 * Locus 03-22-93 vsocket drop to fix select().
 *
 *     Revision 2.47  93/03/19  17:23:29  bhk
 *     Fixed timeing window in accept, removed extra FOP_SELECT for tnc.
 *     Fixes bug 0188
 * 
 * Revision 1.8  1993/03/03  19:53:27  cfj
 * Merged from T9.
 *
 * Revision 1.5.4.3  1993/03/03  19:48:09  cfj
 * Incorrporated a fix from bhk@locus.com where bsd_accept() was not
 * returning properly if start_fileserver_op() returned an error.
 *
 * Revision 1.5.4.2  1993/02/23  18:17:08  cfj
 * Delete annoying printf in sbsd_sel_poll_reply().
 *
 * Revision 1.5.4.1  1993/02/18  01:44:17  cfj
 * Complete select() fix from Locus.
 *
 * Revision 1.6  1993/02/18  01:18:17  cfj
 * Complete select() fix from Locus.
 *
 * Revision 2.46  93/02/10  16:56:38  klh
 * Fix RCS comments
 * 
 * Revision 2.45  93/02/08  16:50:27  bhk
 * Added a call to the file's select routine to cleanup
 * this is a nop for non-vsockets.
 * 
 * Revision 2.44  93/01/26  14:40:17  yazz
 * Added bug number to previous RCS comment.
 * 
 * Revision 2.43  93/01/26  12:23:56  yazz
 * For bug #0156:
 * Temporary workaround to prevent end_vprocserver_op()'s signal processing
 * at the end of bsd_fork().  Permanent solution will involve John LoVerso
 * from OSF.
 * 
 * Revision 1.1.2.2.2.2  1993/02/16  20:07:37  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.5  1993/01/26  18:13:52  cfj
 * Fix for hangs in rforkmulti() from LOCUS.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:04:33  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  03:04:58  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:53:00  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:34:02  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  23:41:55  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.42  92/11/23  16:02:58  klh
 * 	Revision 2.34  92/11/05  17:27:21  roy
 * 		bsd_set_callback changed to bsd_register_proc, and a new rlimit_fsize
 * 		out arg added.
 * 		[92/10/26            roy]
 * 
 * Revision 2.41  92/10/28  15:57:45  roman
 * Remove all TNC-specific system calls. They are now in the tnc
 * 	subdirectory.
 * 
 * Revision 2.40  92/10/16  11:20:18  chrisp
 * Remove call to rtask_pproc_remove() in bsd_migrate() and bsd_rxexec():
 * 	this is now called from migrate() and rexecve().
 * 
 * Revision 2.50  93/06/29  15:36:31  bhk
 * Partial update to OSF/1 AD V1.05b1 to fix select problems
 * 
 * Revision 2.49  93/06/16  13:51:43  klh
 * 	Revision 2.39  93/06/04  12:21:15  rabii
 * 		Added node argument to bsd_fork (rabii)
 * 
 * 	Revision 2.38  93/05/26  14:45:28  loverso
 * 		Correctly get an FP_REF to go along with the extra file port send
 * 		right in bsd_sel_poll_delay().  (loverso)
 * 
 * 	Revision 2.37  93/03/05  17:45:56  loverso
 * 		Redo previous change by using new END_VPROCSERVER_NOSIG() macro.
 * 		Remove debug printf from sbsd_sel_poll_reply().
 *
 * 	Revision 2.36  93/02/04  11:49:05  durriya
 * 		Prevent bsd_fork() from taking a signal or exiting from the
 * 		psig() call in end_vprocserver_op().  This is ugly, but
 * 		necessary.                                           (loverso)
 *
 * Revision 2.47  93/03/19  17:23:29  bhk
 * Fixed timeing window in accept, removed extra FOP_SELECT for tnc.
 * Fixes bug 0188
 * 
 * Revision 2.46  93/02/10  16:56:38  klh
 * Fix RCS comments
 * 
 * Revision 2.45  93/02/08  16:50:27  bhk
 * Added a call to the file's select routine to cleanup
 * this is a nop for non-vsockets.
 * 
 * Revision 2.44  93/01/26  14:40:17  yazz
 * Added bug number to previous RCS comment.
 * 
 * Revision 2.43  93/01/26  12:23:56  yazz
 * For bug #0156:
 * Temporary workaround to prevent end_vprocserver_op()'s signal processing
 * at the end of bsd_fork().  Permanent solution will involve John LoVerso
 * from OSF.
 * 
 * Revision 2.42  92/11/23  16:02:58  klh
 * 	Revision 2.34  92/11/05  17:27:21  roy
 * 		bsd_set_callback changed to bsd_register_proc, and a 
 *		new rlimit_fsize out arg added.
 * 		[92/10/26            roy]
 * 
 * Revision 2.41  92/10/28  15:57:45  roman
 * Remove all TNC-specific system calls. They are now in the tnc
 * 	subdirectory.
 * 
 * Revision 2.40  92/10/16  11:20:18  chrisp
 * Remove call to rtask_pproc_remove() in bsd_migrate() and bsd_rxexec():
 * 	this is now called from migrate() and rexecve().
 * 
 * Revision 2.39  92/10/07  16:14:48  bhk
 * Added notify port to the select_poll_reply command to insure
 * the correct port gets deadname notification.
 * 
 * Revision 2.38  92/10/06  12:15:33  roman
 * Fix RCS comments.
 * Add traced flag to rexecve (similar to what was done for execve).
 * 
 * Revision 2.37  92/10/05  13:49:05  klh
 * 	Revision 2.33  92/09/24  16:51:16  rabii
 * 		Report the STRC flag state to the emulator in bsd_execve();
 * 		added bsd_psignal() to allow emulator to signal its process.
 * 		also, use correct # for SC_TRACE in bsd_proc_exit,
 * 		remove extraneous port_to_proc_lookup in 
 *		bsd_{setports,exec_args_set}. (dwm; #376)
 * 
 * 	Revision 2.32  92/08/26  12:12:43  loverso
 * 		Rework bsd_set_callback_port to include callback thread port.
 * 		(loverso)
 * 
 * Revision 2.36  92/07/30  16:18:23  chrisp
 * end_vprocserver_op() now takes the vproc port as first parameter.
 * 
 * Revision 2.35  92/07/26  17:39:09  bhk
 * added support for select on remote sockets
 * 
 * Revision 2.34  92/07/10  09:07:30  chrisp
 * For TNC, call vproc_end_port_op() at completion of proc port operations
 * 	not using start_/end_server_op.
 * In bsd_share_wakeup(), use per-process mutex p_shared_mutex instead of
 * 	the master mutex.
 * 
 * Revision 2.33  92/07/09  11:37:34  roman
 * Fix up prior RCS comments.
 * 
 * Revision 2.32  92/06/26  14:03:31  chrisp
 * Explicit locking of the master mutex changed to 
 *	master_lock()/master_unlock().
 * 
 * Revision 2.31  92/06/19  14:43:51  chrisp
 * Remove SC_TRACE() invocation in bsd_shared_wakeup() - this fails since
 * 	u.u_procp is not invalid.
 * 
 * Revision 2.30  92/06/05  13:59:06  klh
 * 	Revision 2.29  92/05/31  18:59:33  loverso
 * 		Don't use START_SERVER in bsd_setports() or bsd_proc_exit() - 
 *		not in syscall array.  Enable bsd_emul_uacc().  Change 
 *		sel_poll to use file ports again.  Merge grenoble changes.  
 *		(loverso)
 * 
 * 		Revision 2.27.1.3  92/05/26  15:55:05  loverso
 * 		sbsd_sel_poll_reply() now takes a send right on the file port
 * 		(via MOVE_SEND) to carry the right created for file_port_send 
 *		in bsd_sel_poll_delay().  Since the delay_port is now a 
 *		MOVE_SEND_ONCE, we can abort when it becomes MACH_PORT_DEAD.  
 *		Cross-over into the select implementation is eliminated.
 * 
 * 		Revision 2.27.1.2  92/05/25  21:59:57  loverso
 * 		Select fixes: Fix bug where ports weren't freed when 
 *		select_enqueue was not called, but no revents returned.  
 *		Remove use of fileserver_port.
 * 
 * 		Revision 2.27.1.1  92/05/21  15:17:43  loverso
 * 		Hold a file port (and send right) instead of a file pointer (and
 * 		FP_REF) for the duration of an active select/poll.
 * 
 * 		Revision 3.26  92/04/08  20:46:09  barbou
 * 		Fix for bug #127: definition of bsd_proc_exit().
 * 
 * 	Revision 2.28  92/05/24  13:54:45  pjg
 * 		Call unix_master and unix_release if NCPUS == 1.
 * 
 * 		Revision 3.25  92/03/24  21:03:57  barbou
 * 		Renamed TRACE() to SC_TRACE() to avoid conflict with 
 *		sys/trace.h.
 * 
 * 	Revision 2.27  92/05/13  15:07:37  loverso
 * 		Only deallocate the delay_port if there was one.
 * 
 * 	Revision 2.26  92/05/13  14:09:08  loverso
 * 		Use select_deallocate_delayport() to remove a delay port and 
 *		dead-name notification.  mod ref the dead-name in 
 *		sbsd_sel_poll_reply() if select_deadname lost the race 
 *		with select_wakeup.
 * 
 * 	Revision 2.25  92/05/12  10:59:10  loverso
 * 		Disable debugging printfs.
 * 
 * 	Revision 2.24  92/05/12  00:03:06  loverso
 * 		Reworked select/poll handling.  (loverso)
 * 
 * Revision 2.29  92/04/22  09:26:50  roman
 * Change invocation of START_SERVER macro with "0" for array size to a "1"
 * 	so that ANSI C compiler is happy.
 * 
 * Revision 2.28  92/04/21  11:34:52  roman
 * Change START_VPROCSERVER() invocations to not have a "0" second parameter,
 * 	since arrays with zero elements are disliked by the ANSI C.
 * 
 * Revision 2.27  92/04/14  10:44:12  roman
 * Add rforkmulti() system call.
 * Use START_VPROCSERVER() and END_VPROCSERVER() macros instead of explicit 
 *	calls to the start_ and end_ op routines.
 * Move rtask_pproc_remove() calls before end_vprocserver() call so that the
 * 	uarea context remains in place.
 * 
 * Revision 2.26  92/04/06  19:10:00  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.22  92/04/05  17:07:26  pjg
 * 	Change bsd_setports to handle new vnode proxies. Got rid of cwd and
 * 	root ports in exec (pjg).
 * 
 * 	Add new system calls set_tnc_port and get_tnc_port for TNC. (roman)
 * 
 * Revision 2.21  92/03/20  11:30:38  pjg
 * 	Credentials port is no longer and argument to bsd_mmap. Don't pass 
 * 	it to smmap either. Don't set ni_credsport in bsd_execve (durriya)
 * 
 * Revision 2.20  92/03/15  14:29:56  roy
 * 	92/02/19  10:33:26  roy
 * 	Mapped files interfaces moved to fsvr.defs.
 * 	Fix vm leaks (see calls to vm_deallocate) (pjg).
 * 
 * Revision 2.19  92/03/09  13:12:12  durriya
 * 	92/02/28  18:00:00  barbou
 * 	Fix memory leak in bsd_recvfrom_long().
 * 
 * 	92/02/28  00:11:37  condict
 * 	Add bsd_emul_uacc for EFAULT recovery in the emulator.
 * 
 * 	92/01/30  16:05:47  sp
 * 	Clean up auxiliary vector code
 * 
 * Revision 2.18  92/03/03  13:48:41  pjg
 * 	Changed call to START_SERVER in bsd_setports (sjs).
 * 
 * Revision 2.17  92/03/01  18:31:26  pjg
 * 	92/02/28  pjg
 * 	Changed deallocation of ports and memory received in messages
 * 	according to the new ux_server_loop.
 * 	Add bsd_setports, used by chdir().
 * 
 * 	Add bsd_mmap (durriya).
 * 	Delete signature port everywhere and the "interrupt" variable for 
 * 	non-PM messages (loverso).
 * 
 * Revision 2.16  92/02/11  21:40:18  pjg
 * 	Add code for rexecve()/migrate()/rfork() (TNC only).
 * 	Change interface to execve() to not pass in creds port 
 * 	(it is already in the proc structure).
 * 	Change auxv types to match MiG definition  (roman@locus.com).
 * 
 * Revision 2.15  92/01/17  21:08:06  roy
 * 	Make socket system calls interruptible (loverso).
 * 
 * Revision 2.14  92/01/17  17:31:59  roy
 * 	Add bsd_set_signature_port (loverso).
 * 
 * Revision 2.13  92/01/14  11:25:53  roy
 * 	Pass serial flag to end ops in all places.
 * 
 * Revision 2.12  92/01/05  20:23:46  roy
 * 	92/01/02  17:46:30  roy
 * 	Add cwd, root, and creds port args to bsd_execve.
 * 	Many routines moved to fsvr_server_side.c  start and end op macros
 * 	removed.  (noemi)
 * 
 * Revision 2.11  91/12/17  12:00:33  roy
 * 	91/12/13  13:03:34  sp
 * 	Remove CMUCS and old BSD code which is never used.
 * 
 * 	91/12/11  15:12:26  condict
 * 	Change bsd_share_wakeup to do condition signal, instead of wakeup.
 * 
 * 	91/10/30  17:43:40  bernadat
 * 	Fix for bug #30, handle NULL times argument to utimes() [barbou].
 * 
 * 	91/10/28  11:10:13  condict
 * 	Add MIG stub functions bsd_msleep/bsd_mwakeup.
 * 
 * Revision 2.10  91/12/13  10:20:30  roy
 * 	91/11/12  10:17:50  roy
 * 	Added token interfaces.
 * 	Comment out bsd_rmknod until kernel can handle extra syscalls.
 * 
 * Revision 2.9  91/11/25  11:24:05  rabii
 * 	Imlemented bsd_rmknod body for remote devices
 * 
 * Revision 2.8  91/11/22  15:20:24  rabii
 * 	Locus merge
 * 	start_fileserver_op_nocred() passed pid and rcred pointer saved from
 * 	select(). (chrisp)
 *
 * Revision 2.7  91/11/13  15:48:16  rabii
 * 	Moved include <sys/vnode.h> to before include <ufs/inode.h>
 * 
 * Revision 2.6  91/10/14  13:23:43  sjs
 * 	91/10/04  16:42:31  sp
 * 	Add support for auxiliary vector
 * 
 * 	91/09/27  12:02:29  emcmanus
 * 	Added profilnig routines.
 * 
 * Revision 2.5  91/10/04  15:10:38  chrisp
 * Get rid of extraneous $Log. Pass in credentials port to end_fileserver_op().
 * Get rid of references to msgh_kind (it is now a sequence number).
 * Change sbsd_sel_poll_reply() to no longer use a credentials port, but
 * to instead receive a ucred structure and a pid directly. This required
 * a philosophical change in server/bsd/subr_select.c.
 * 
 * Revision 2.4  91/09/17  16:43:01  rabii
 * 	Merge of V2.0 with locus code (locus check-in by hao)
 * 	Major changes to this file, dealing with file descriptors now
 * 	being handled in the emulator. Many new routines now declared in
 * 	this file. Any parameter that was a file descriptor is now
 * 	a Mach port. Routines that had file descriptor as rvalp now
 * 	have Mach port as explicit output parameters. Additionally,
 * 	there is a new interface between emulator and server for fork().
 * 	The source code was changed to not depend on the fact the proc
 * 	ports and file ports currently have exactly the same values as
 * 	proc structure pointer and file structure pointers. Add handling 
 * 	for new messages for select() and poll() of individual file ports.
 * 
 * Revision 2.3  91/09/17  09:20:14  sjs
 * integrate Locus changes
 * 
 * Revision 2.2  91/08/31  14:24:06  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.14  91/08/27  15:37:21  barbou
 * Upgrade to UX26.
 * 
 * Revision 3.13  91/08/09  12:23:52  barbou
 * Added the bsd_signal() function for the sysV signal() system call.
 * 
 * Revision 3.12  91/07/05  16:23:50  jose
 * Corrected calls to unix_release on // syscalls
 * 
 * Revision 3.11  91/06/12  09:38:46  condict
 *  Eliminate oexecve syscall and stubs (no longer needed for bootstrap).
 * Also, eliminate sysquota stub func.
 * 
 * Revision 3.10  91/06/07  15:53:24  barbou
 * bsd_exec_args_set(): send some information to the server about argv and env.
 * 
 * Revision 3.9  91/06/05  16:20:25  condict
 * Delete VICE-specific sys call: pioctl().
 * 
 * Revision 3.8  91/05/30  17:38:25  jose
 * Added "o" prefix to compat syscalls.
 * 
 * Revision 3.7  91/05/29  16:17:49  condict
 * Change send to osend for 4.3 compatibility mode.
 * 
 * Revision 3.6  91/05/24  11:37:02  jose
 * Added bsd_init_process() entry point.
 * 
 * Revision 3.5  91/05/15  17:57:16  barbou
 * New routine: osf1_sigaction().
 * 
 * Revision 3.4  91/05/07  15:52:54  condict
 * Change START/END_SERVER_SERIAL/PARALLEL macro calls to new version that
 * looks up the serial vs. parallel field in the sysent table.
 * 
 * Revision 3.3  91/04/12  14:21:12  condict
 * Change name of syscall: SYS_setquota -> SYS_quotactl
 * 
 * Revision 3.2  91/03/08  16:05:59  condict
 * Modified to work with the OSF/1 header files
 * 
 * Revision 3.1  91/02/27  16:25:16  condict
 * Eliminate CMU and VICE syscalls.
 * 
 * Revision 3.0  91/01/17  12:05:47  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.13  91/03/20  14:59:07  dbg
 * 	Fixed bsd_write_short, bsd_write_long, bsd_readwrite to be serial.
 * 	[91/02/02            rpd]
 * 
 * Revision 2.12  91/03/12  21:52:23  dbg
 * 	Fix reference to user_request_it.
 * 	[91/03/12            rwd]
 * 
 * Revision 2.11  90/10/25  15:07:06  rwd
 * 	Check out data size in pioctl to avoid overwriting reply message.
 * 	[90/10/10            rwd]
 * 
 * Revision 2.10  90/08/06  15:34:29  rwd
 * 	Change bsd_select to reflect change in interface.
 * 	[90/06/25            rwd]
 * 	Fix share_lock references.
 * 	[90/06/08            rwd]
 * 
 * Revision 2.9  90/06/19  23:15:45  rpd
 * 	Fixed bug in bsd_readwrite.
 * 	[90/06/06            rpd]
 * 
 * Revision 2.8  90/06/02  15:26:55  rpd
 * 	Removed bsd_signals_wakeup.
 * 	[90/05/11            rpd]
 * 	More updates for new IPC.
 * 	[90/05/03            rpd]
 * 	Converted to new IPC.
 * 	[90/03/26  20:08:33  rpd]
 * 
 * Revision 2.7  90/05/29  20:24:47  rwd
 * 	Remove some debugging printouts.
 * 	[90/05/12            rwd]
 * 	Added override parameter to user_request_it call.
 * 	[90/04/14            rwd]
 * 	Pass count argument for each path_name_t parameter.
 * 	[90/04/06            dbg]
 * 
 * 	Move bsd_execve glue routine here.
 * 	[90/04/06            dbg]
 * 
 * 	Added calls used by MACH_NBC && MAP_UAREA.
 * 	[90/03/22            rwd]
 * 
 * Revision 2.6  90/05/21  14:00:52  dbg
 * 	Fix arguments to adjtime.
 * 	[90/04/12            dbg]
 * 
 * 	Pass count argument for each path_name_t parameter.
 * 	[90/04/06            dbg]
 * 
 * 	Move bsd_execve glue routine here.
 * 	[90/04/06            dbg]
 * 
 * Revision 2.5  90/03/14  21:30:36  rwd
 * 	Added bsd_share_wakeup for shared locks.
 * 	[90/02/16            rwd]
 * 	Added bsd_emulator_error and bsd_readwrite.
 * 	[90/01/26            rwd]
 * 
 * Revision 2.4  89/11/29  15:30:16  af
 * 	Revision 2.2.2.1  89/11/16  15:29:42  af
 * 		Sigvec takes one more argument.
 * 	[89/11/26  11:37:49  af]
 * 
 * Revision 2.3  89/11/15  13:27:28  dbg
 * 	Add bsd_pioctl.
 * 	[89/11/07            dbg]
 * 	Add bsd_table_set and bsd_table_get.  Table call is split into
 * 	two interfaces because we can't use a 'dealloc' flag on an
 * 	in-out argument.
 * 	[89/10/25            dbg]
 * 
 * Revision 2.2  89/10/17  11:26:57  rwd
 * 	Added interrupt return parameter to all calls.
 * 	[89/09/21            dbg]
 * 
 * $EndLog$
 */

#include <map_uarea.h>
#include <mach_nbc.h>
#include <mapped_files.h>

#include <uxkern/syscall_subr.h>
#include <uxkern/import_mach.h>
#include <uxkern/bsd_types.h>
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <sys/proc.h>
#include <sys/errno.h>
#include <sys/file.h>

#include <uxkern/proc_to_task.h>
#include <uxkern/syscalltrace.h>
#include <uxkern/bsd_msg.h>
#include <uxkern/mf.h>

#include "profiling.h"

#ifdef SLL
#include <sll/sll_types.h>
#endif /* SLL */

#ifdef	TNC
#include <vsocket/vsocket.h>
#include <vsocket/vs_subr.h>
#endif

/*
 * in uipc_syscalls
 */
#ifdef	COMPAT_43
int
bsd_osend_short(file_port, creds_port, transid,
		flags, data, data_count, amount_written)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[3];
        int syscode = SYS_osend;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	error = osend(fp, arg, amount_written);

        return(end_fileserver_op(fp, error, serial));
}

int
bsd_osend_long(file_port, creds_port, transid,
		flags, data, data_count, amount_written)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[3];
        int syscode = SYS_osend;
        int serial = !sysent[syscode].sy_parallel;
        kern_return_t   kr;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	error = osend(fp, arg, amount_written);

	/*
	 * WARNING:
	 *
	 * We don't want to deallocate the data buffer if the osend failed.
	 *
	 * Specifically, if (error != 0), then the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(),
					(vm_address_t)data,
					(vm_size_t)data_count);
		if (kr != KERN_SUCCESS) {
			panic("bsd_osend_long: vm_deallocate(0x%x,%d) = 0x%x\n",
				data, data_count, kr);
		}
	}
	return(end_fileserver_op(fp, error, serial));
}
#endif	/* COMPAT_43 */

int
bsd_sendto_short(file_port, creds_port, transid, flags,
		to, tolen, data, data_count, amount_written)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	char		*to;
	int		tolen;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_sendto;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	arg[3] = (int)to;
	arg[4] = tolen;
	error = sendto(fp, arg, amount_written);

        return(end_fileserver_op(fp, error, serial));
}

int
bsd_sendto_long(file_port, creds_port, transid, flags,
		to, tolen, data, data_count, amount_written)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	char		*to;
	int		tolen;
	char		*data;
	unsigned int	data_count;
	int		*amount_written;	/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_sendto;
        int serial = !sysent[syscode].sy_parallel;
        kern_return_t   kr;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = (int)data_count;
	arg[2] = flags;
	arg[3] = (int)to;
	arg[4] = tolen;
	error = sendto(fp, arg, amount_written);

	/*
	 * WARNING:
	 *
	 * We don't want to deallocate the data buffer if the sendto failed.
	 *
	 * Specifically, if (error != 0), then the ux_server_loop()
	 * function will deallocate the data buffer for us.
	 */
	if (!error) {
		kr = vm_deallocate(mach_task_self(),
					(vm_address_t)data,
					(vm_size_t)data_count);
		if (kr != KERN_SUCCESS) {
			panic("bsd_sendto_long: vm_deallocate(0x%x,%d)= 0x%x\n",
				data, data_count, kr);
		}
	}
	return(end_fileserver_op(fp, error, serial));
}

#ifdef	COMPAT_43
int
bsd_orecv_short(file_port, creds_port, transid, flags, len, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		*data;		/* pointer to OUT array */
	unsigned int	*data_count;	/* out */
{
	register int    error;
	struct file *fp;
	int arg[3];
	int syscode = SYS_orecv;
	int serial = !sysent[syscode].sy_parallel;

	error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
	if (error)
		return(error);

	arg[0] = (int)data;
	arg[1] = len;
	arg[2] = flags;

	error = orecv(fp, (void *)arg, (int *)data_count);

	return(end_fileserver_op(fp, error, serial));
}

int
bsd_orecv_long(file_port, creds_port, transid, flags, len, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		**data;		/* out, dealloc (ool memory) */
	unsigned int	*data_count;	/* out, size of the above */
{
	register int	error;
	struct file	*fp;
	int		arg[3];
	int		syscode = SYS_orecv;
	int		serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*data = 0;
	*data_count = 0;

	error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
	if (error)
		return(error);

	if (vm_allocate(mach_task_self(), (vm_address_t *)data, len, TRUE) 
	   != KERN_SUCCESS) {
		error = ENOBUFS;
	} else {
		arg[0] = (int)*data;
		arg[1] = len;
		arg[2] = flags;

		error = orecv(fp, (void *)arg, (int *)data_count);
	}
	/*
	 * On error, deallocate all the data.
	 * On normal return, deallocate the unused data (MiG won't deallocate
	 * this extra memory, since it thinks data_count is the real size).
	 */
	error = end_fileserver_op(fp, error, serial);
	if (*data) {
		if (error)
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *data, len);
		else {
			len -= round_page(*data_count);
			if (len > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*data + round_page(*data_count));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}		
	return(error);
}
#endif	/* COMPAT_43 */

#ifdef	COMPAT_43
int
bsd_orecvfrom_short(file_port, creds_port, transid,
		flags, len, from, fromlen, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		*from;		/* pointer to OUT array */
	int		*fromlen;	/* in/out */
	char		*data;		/* pointer to OUT array */
	unsigned int	*data_count;	/* out */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_orecvfrom;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = len;
	arg[2] = flags;
	arg[3] = (int)from;
	arg[4] = (int)fromlen;

	error = orecvfrom(fp, (void *)arg, (int *)data_count);

        return(end_fileserver_op(fp, error, serial));
}

int
bsd_orecvfrom_long(file_port, creds_port, transid, flags,
		len, from, fromlen, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		*from;		/* pointer to OUT array */
	int		*fromlen;	/* ptr to size of the above */
	char		**data;		/* out, dealloc (ool memory) */
	unsigned int	*data_count;	/* out, size of the above */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_orecvfrom;
        int serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*data = 0;
	*data_count = 0;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	if (vm_allocate(mach_task_self(), (vm_offset_t *)data, len, TRUE) 
	    != KERN_SUCCESS) {
		error = ENOBUFS;
	} else {
	    arg[0] = (int)*data;
	    arg[1] = len;
	    arg[2] = flags;
	    arg[3] = (int)from;
	    arg[4] = (int)fromlen;

	    error = orecvfrom(fp, (void *)arg, (int *)data_count);
	}
	/*
	 * On error, deallocate all the data.
	 * On normal return, deallocate the unused data (MiG won't deallocate
	 * this extra memory, since it thinks data_count is the real size).
	 */
	error = end_fileserver_op(fp, error, serial);
	if (*data) {
		if (error)
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *data, len);
		else {
			len -= round_page(*data_count);
			if (len > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*data + round_page(*data_count));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}		
	return(error);
}
#endif	/* COMPAT_43 */

int
bsd_recvfrom_short(file_port, creds_port, transid, flags,
		len, from, fromlen, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		*from;		/* pointer to OUT array */
	int		*fromlen;	/* in/out */
	char		*data;		/* pointer to OUT array */
	unsigned int	*data_count;	/* out */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_recvfrom;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int)data;
	arg[1] = len;
	arg[2] = flags;
	arg[3] = (int)from;
	arg[4] = (int)fromlen;

	error = recvfrom(fp, (void *)arg, (int *)data_count);

        return(end_fileserver_op(fp, error, serial));
}

int
bsd_recvfrom_long(file_port, creds_port, transid, flags,
		len, from, fromlen, data, data_count)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	int		flags;
	int		len;
	char		*from;		/* pointer to OUT array */
	int		*fromlen;	/* pointer to size of the above */
	char		**data;		/* out, dealloc (ool memory) */
	unsigned int	*data_count;	/* out, size of the above */
{
        register int    error;
        struct file *fp;
        int arg[5];
        int syscode = SYS_recvfrom;
        int serial = !sysent[syscode].sy_parallel;

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*data = 0;
	*data_count = 0;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	if (vm_allocate(mach_task_self(), (vm_offset_t *)data, len, TRUE)
	    != KERN_SUCCESS) {
		error = ENOBUFS;
	} else {
	    arg[0] = (int)*data;
	    arg[1] = len;
	    arg[2] = flags;
	    arg[3] = (int)from;
	    arg[4] = (int)fromlen;

	    error = recvfrom(fp, (void *)arg, (int *)data_count);
	}
	/*
	 * On error, deallocate all the data.
	 * On normal return, deallocate the unused data (MiG won't deallocate
	 * this extra memory, since it thinks data_count is the real size).
	 */
	error = end_fileserver_op(fp, error, serial);
	if (*data) {
		if (error)
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t) *data, len);
		else {
			len -= round_page(*data_count);
			if (len > 0) {
				vm_address_t	start;
				start = (vm_address_t) 
					(*data + round_page(*data_count));
				(void) vm_deallocate(mach_task_self(), 
						     start, len);
			}
		}
	}		
	return(error);
}

int
bsd_setports(proc_port, cwd_port, root_port, interrupt)
	mach_port_t		proc_port;
	mach_port_t		cwd_port;
	mach_port_t		root_port;
	boolean_t		*interrupt;	/* out */
{
	register int 		error;
	uthread_t		uth = &u;

	if (error = start_server_op(proc_port, 1005))
		return (error);

	/*
	 * The PM keeps exactly one reference to the cwd and root
	 * ports in the u-area.
	 */
	U_HANDY_LOCK();

	VN_LOCK_PROXY(&uth->u_cdirproxy);
	ASSERT(uth->u_cdirproxy.vpx_usecount == 1);
	if (uth->u_cdirport != cwd_port) {
		mach_port_t temp = uth->u_cdirport;
		uth->u_cdirport = cwd_port;
		if (temp) {
			int error = mach_port_deallocate(mach_task_self(), 
							 temp);
			if (error) 
				printf ("bsd_setports: cwd port=0x%x, ret=0x%x\n", temp, error);
		}
		if (uth->u_cdir)
			VNODE_LOOKUP_DONE(uth->u_cdir);

		PORT_TO_VNODE_LOOKUP(cwd_port, uth->u_cdir);
		uth->u_cdirproxy.vpx_usecount = 1;
	} else
		mach_port_deallocate(mach_task_self(), cwd_port);
	VN_UNLOCK_PROXY(&uth->u_cdirproxy);

	VN_LOCK_PROXY(&uth->u_rdirproxy);
	ASSERT(uth->u_rdirproxy.vpx_usecount == 1);
	if (uth->u_rdirport != root_port) {
		mach_port_t temp = uth->u_rdirport;
		uth->u_rdirport = root_port;
		if (temp) {
			int error = mach_port_deallocate(mach_task_self(), 
							 temp);
			if (error) 
				printf ("bsd_setports: root port=0x%x, ret=0x%x\n", temp, error);
		}
		if (uth->u_rdir)
			VNODE_LOOKUP_DONE(uth->u_rdir);

		PORT_TO_VNODE_LOOKUP(cwd_port, uth->u_rdir);
		uth->u_rdirproxy.vpx_usecount = 1;
	} else
		mach_port_deallocate(mach_task_self(), root_port);
	VN_UNLOCK_PROXY(&uth->u_rdirproxy);

	U_HANDY_UNLOCK();

	error = ESUCCESS;

	return (end_server_op(error, interrupt));
}

#if ! defined(SLL) && ! defined(SLLCOMPAT)
bsd_fork(vproc_port, interrupt, node, new_state, new_state_count,
         new_task, new_thread, new_vproc, new_cred, child_pid)
#else /* ! SLL && ! SLLCOMPAT */
int
bsd_fork(vproc_port, interrupt, node, new_state, new_state_count, 
	 new_task, new_thread, new_vproc, new_cred, child_pid,
	 vproc_port_name, cred_port_name, force_local, was_rfork)
#endif /* ! SLL && ! SLLCOMPAT */
	mach_port_t	vproc_port;
	boolean_t	*interrupt;
	node_t		node;
	thread_state_t	new_state;
	unsigned int	new_state_count;
	task_t		*new_task;	/* OUT */
	thread_t	*new_thread;	/* OUT */
	mach_port_t	*new_vproc;	/* OUT */
	mach_port_t	*new_cred;	/* OUT */
	int		*child_pid;	/* OUT */
#if defined(SLL) || defined(SLLCOMPAT)
	mach_port_t	vproc_port_name;
	mach_port_t	cred_port_name;
	boolean_t	force_local;
	boolean_t	*was_rfork;	/* OUT */
#endif /* SLL || SLLCOMPAT */
{
	struct proc *childp;
#ifdef SLL
	node_t		fork_node;
#endif /* SLL */

	START_VPROCSERVER(SYS_fork, 5)

	SC_TRACE(("(%s)","fork"));

	/*
	 *  Initialize all out parameters 
	 *  which are ports to MACH_PORT_NULL.
	 */
	*new_task = (task_t) MACH_PORT_NULL;
	*new_thread = (thread_t) MACH_PORT_NULL;
	*new_vproc = MACH_PORT_NULL;
	*new_cred = MACH_PORT_NULL;
	*child_pid = 0;

#ifdef SLL
	/*
	 * If we are not forced to do the fork locally,
	 * get the most lightly loaded node.
	 */
	if ( ! force_local ) {
		fork_node = sll_fast_node();
		SLL_DEBUGF("bsd_fork: fast_node=%d\n", fork_node);
	}
	else {
		/*
		 * We must stay local.
		 */
		fork_node = -1;
		SLL_DEBUGF("bsd_fork: force_local=TRUE\n");
	}

	/*
	 * If we got a valid fork_node do a rfork instead of fork.
	 */
	if ( fork_node !=  -1 ) {
		*was_rfork = TRUE;
		error = rfork(vproc_port, fork_node,
				new_state, new_state_count,
				vproc_port_name, cred_port_name,
				new_task, new_thread, child_pid);

		*new_vproc = MACH_PORT_NULL;
		*new_cred = MACH_PORT_NULL;
	}
	else {
		/*
		 * Do a normal fork if we don't have a valid fork node.
		 */
#endif /* SLL */
#if defined(SLL) || defined(SLLCOMPAT)
		*was_rfork = FALSE;
#endif /* SLL || SLLCOMPAT */

	arg[0] = (int) node;
	arg[1] = (int)new_state;
	arg[2] = new_state_count;
	arg[3] = (int) new_task;
	arg[4] = (int) new_thread;
	error = fork(vprocp, arg, child_pid);

	if (error == 0 && (childp = pfind(*child_pid)) != NULL) {
		*new_thread = childp->p_thread;
		*new_task = childp->p_task;
		*new_vproc = vproc_to_port_lookup(childp->p_vproc);
		*new_cred = childp->p_cred;
	} else {
		if (error == 0)
			error = EAGAIN;
		*new_thread = MACH_PORT_NULL;
		*new_task = MACH_PORT_NULL;
		*new_vproc = MACH_PORT_NULL;
		*new_cred = MACH_PORT_NULL;
	}

#ifdef SLL
	}	/* See else above */
#endif /* SLL */

	/*
	 * We cannot check for signals or die here;  we've a stub process
	 * to deal with.  Thus, we omit the signal check in 
         * end_vprocserver_op but cause the emulator to deal with the 
         * signals later.
	 */
	END_VPROCSERVER_NOSIG(SYS_fork)
}

/*
 * More glue
 */
int
bsd_setgroups(proc_port, interrupt, gidsetsize, gidset)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	unsigned int	gidsetsize;
	int		*gidset;
{
	START_SERVER(SYS_setgroups, 2)

	arg[0] = gidsetsize;
	arg[1] = (int)gidset;

	error = setgroups(procp, arg, 0);

	END_SERVER(SYS_setgroups)
}

int
bsd_setrlimit(proc_port, interrupt, which, lim)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		which;
	struct rlimit	*lim;
{
	START_SERVER(SYS_setrlimit, 2)

	arg[0] = which;
	arg[1] = (int)lim;

	error = setrlimit(procp, arg, 0);

	END_SERVER(SYS_setrlimit)
}

#ifdef	COMPAT_43
int
bsd_osigvec(proc_port, interrupt, signo, have_nsv, nsv, osv, tramp)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		signo;
	boolean_t	have_nsv;
	struct sigvec	nsv;
	struct sigvec	*osv;		/* OUT */
	int		tramp;
{
	START_SERVER(SYS_osigvec, 4)

	arg[0] = signo;
	arg[1] = (have_nsv) ? (int)&nsv : 0;
	arg[2] = (int)osv;
	arg[3] = tramp;

	{ 
		int retval;
		error = osigvec(procp, arg, &retval);
	}

	END_SERVER(SYS_osigvec)
}
#endif	/* COMPAT_43 */

int
bsd_signal(proc_port, interrupt, signo, handler, ohandler, tramp)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* OUT */
	int		signo;
	int		handler;
	int		*ohandler;	/* OUT */
	int		tramp;
{
	START_SERVER(SYS_signal, 3);

	arg[0] = signo;
	arg[1] = handler;
	arg[2] = tramp;

	error = ssig((struct proc *)proc_port, arg, ohandler);

	END_SERVER(SYS_signal)
}	

int
osf1_sigaction(proc_port, interrupt, signo, have_nsa, nsa, osa, tramp)
	mach_port_t		proc_port;
	boolean_t		*interrupt;	/* OUT */
	int			signo;
	boolean_t		have_nsa;
	struct sigaction	nsa;
	struct sigaction	*osa;		/* OUT */
	int			tramp;
{
	START_SERVER(SYS_sigaction, 4)

	arg[0] = signo;
	arg[1] = (have_nsa) ? (int)&nsa : 0;
	arg[2] = (int)osa;
	arg[3] = tramp;

	{
		int retval;
		error = sigaction(procp, arg, &retval);
	}

	END_SERVER(SYS_sigaction)
}

int
bsd_sigstack(proc_port, interrupt, have_nss, nss, oss)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	boolean_t	have_nss;
	struct sigstack	nss;
	struct sigstack	*oss;		/* OUT */
{
	START_SERVER(SYS_sigstack, 2)

	arg[0] = (have_nss) ? (int)&nss : 0;
	arg[1] = (int)oss;

	error = sigstack(procp, arg, 0);

	END_SERVER(SYS_sigstack)
}

int
bsd_settimeofday(proc_port, interrupt, have_tv, tv, have_tz, tz)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	boolean_t	have_tv;
	struct timeval	tv;
	boolean_t	have_tz;
	struct timezone	tz;
{
	START_SERVER(SYS_settimeofday, 2)

	arg[0] = (have_tv) ? (int)&tv : 0;
	arg[1] = (have_tz) ? (int)&tz : 0;

	error = settimeofday(procp, arg, 0);

	END_SERVER(SYS_settimeofday)
}

int
bsd_adjtime(proc_port, interrupt, delta_supplied, delta, olddelta)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	boolean_t	delta_supplied;
	struct timeval	delta;
	struct timeval	*olddelta;
{
	START_SERVER(SYS_adjtime, 2)

	arg[0] = (int)&delta;
	arg[1] = (int)olddelta;

	error = adjtime(procp, arg, 0);

	END_SERVER(SYS_adjtime)
}

int
bsd_setitimer(proc_port, interrupt, which, have_itv, itv, oitv)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		which;
	boolean_t	have_itv;
	struct itimerval itv;
	struct itimerval *oitv;		/* OUT */
{
	START_SERVER(SYS_setitimer, 3)

	arg[0] = which;
	arg[1] = (have_itv) ? (int)&itv : 0;
	arg[2] = (int)oitv;

	error = setitimer(procp, arg, 0);

	END_SERVER(SYS_setitimer)
}

int
bsd_sethostname(proc_port, interrupt, hostname, len)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*hostname;
	int		len;
{
	START_SERVER(SYS_sethostname, 2)

	arg[0] = (int)hostname;
	arg[1] = len;

	error = sethostname(procp, arg, 0);

	END_SERVER(SYS_sethostname)
}

int
bsd_bind(file_port, creds_port, transid, name, namelen)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	char		*name;
	int		namelen;
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_bind;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int) name;
	arg[1] = namelen;

	error = bind(fp, arg, 0);

        return(end_fileserver_op(fp, error, serial));
}

#ifdef	COMPAT_43
int
bsd_oaccept(file_port, creds_port, transid, name, anamelen, fpp)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*name;		/* OUT */
	int			*anamelen;	/* OUT */
	mach_port_t		*fpp;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[3];
        int syscode = SYS_oaccept;
        int serial = !sysent[syscode].sy_parallel;
        struct file *filep;

	*fpp = MACH_PORT_NULL;		/* init MIG out param to null */

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error) {
                return(error);
        }

	*anamelen = sizeof(sockarg_t);
	arg[0] = (int) name;
	arg[1] = (int) anamelen;
	arg[2] = (int) &filep;
	error = oaccept(fp, arg, 0);
        if (error == 0) {
                FILE_TO_PORT_LOOKUP(filep, *fpp);	/* sets *fpp = filep */
	}
        return(end_fileserver_op(fp, error, serial));
}
#endif	/* COMPAT_43 */

int
bsd_accept(file_port, creds_port, transid, name, anamelen, fpp)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*name;		/* OUT */
	int			*anamelen;	/* OUT */
	mach_port_t		*fpp;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[3];
        int syscode = SYS_accept;
        int serial = !sysent[syscode].sy_parallel;
        struct file *filep;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error) {
                *fpp = MACH_PORT_NULL;
        }

	*anamelen = sizeof(sockarg_t);
	arg[0] = (int) name;
	arg[1] = (int) anamelen;
	arg[2] = (int) &filep;
	error = accept(fp, arg, 0);
        if (error == 0)
                FILE_TO_PORT_LOOKUP(filep, *fpp);
        else
                *fpp = MACH_PORT_NULL;
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_connect(file_port, creds_port, transid, name, namelen)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*name;
	int			namelen;
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_connect;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	arg[0] = (int) name;
	arg[1] = namelen;
	error = connect(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_setsockopt(file_port, creds_port, transid, level, name, val, valsize)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			level;
	int			name;
	char			*val;
	int			valsize;
{
        register int    error;
        struct file *fp;
        int arg[4];
        int syscode = SYS_setsockopt;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	arg[0] = level;
	arg[1] = name;
	arg[2] = (valsize > 0) ? (int)val : 0;
	arg[3] = valsize;
	error = setsockopt(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_getsockopt(file_port, creds_port, transid, level, name, val, avalsize)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			level;
	int			name;
	char			*val;		/* OUT */
	int			*avalsize;	/* IN-OUT */
{
        register int    error;
        struct file *fp;
        int arg[4];
        int syscode = SYS_getsockopt;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	*avalsize = sizeof(sockarg_t);

	arg[0] = level;
	arg[1] = name;
	arg[2] = (int)val;
	arg[3] = (int)avalsize;
	error = getsockopt(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

#ifdef	COMPAT_43
int
bsd_ogetsockname(file_port, creds_port, transid, asa, alen)
	mach_port_t		file_port;
	transaction_id_t	transid;
	mach_port_t		creds_port;
	char			*asa;		/* OUT */
	int			*alen;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_ogetsockname;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	*alen = sizeof(sockarg_t);
	arg[0] = (int)asa;
	arg[1] = (int)alen;
	error = ogetsockname(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_ogetpeername(file_port, creds_port, transid, asa, alen)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*asa;		/* OUT */
	int			*alen;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_ogetpeername;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	*alen = sizeof(sockarg_t);

	arg[0] = (int)asa;
	arg[1] = (int)alen;
	error = ogetpeername(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}
#endif	COMPAT_43

int
bsd_getsockname(file_port, creds_port, transid, asa, alen)
	mach_port_t		file_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*asa;		/* OUT */
	int			*alen;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_getsockname;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
				syscode, serial);
        if (error)
                return(error);

	*alen = sizeof(sockarg_t);
	arg[0] = (int)asa;
	arg[1] = (int)alen;
	error = getsockname(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_getpeername(file_port, creds_port, transid, asa, alen)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	transaction_id_t	transid;
	char		*asa;		/* OUT */
	int		*alen;		/* OUT */
{
        register int    error;
        struct file *fp;
        int arg[2];
        int syscode = SYS_getpeername;
        int serial = !sysent[syscode].sy_parallel;

        error = start_fileserver_op(&fp, file_port, creds_port, transid,
					syscode, serial);
        if (error)
                return(error);

	*alen = sizeof(sockarg_t);
	arg[0] = (int)asa;
	arg[1] = (int)alen;
	error = getpeername(fp, arg, 0);
        return(end_fileserver_op(fp, error, serial));
}

int
bsd_table_set(proc_port, interrupt, id, index, lel, nel,
		addr, count, nel_done)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	int		id;
	int		index;
	int		lel;
	int		nel;
	char		*addr;
	unsigned int	count;
	int		*nel_done;	/* out */
{
	START_SERVER(SYS_table, 5)

	arg[0] = id;
	arg[1] = index;
	arg[2] = (int)addr;
	arg[3] = nel;
	arg[4] = lel;

	error = table(procp, arg, nel_done);

	END_SERVER(SYS_table)
}

int
bsd_table_get(proc_port, interrupt, id, index, lel, nel,
		addr, count, nel_done)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	int		id;
	int		index;
	int		lel;
	int		nel;
	char		**addr;		/* out, dealloc (ool memory) */
	unsigned int	*count;		/* out, length of the above */
	int		*nel_done;	/* out */
{
	START_SERVER(SYS_table, 5)

	*count = nel * lel;
	if (vm_allocate(mach_task_self(), (vm_offset_t *)addr, *count, TRUE)
	    != KERN_SUCCESS) {
		error = ENOBUFS;
		*addr = 0;
		*count = 0;
	} else {
		arg[0] = id;
		arg[1] = index;
		arg[2] = (int)*addr;
		arg[3] = nel;
		arg[4] = lel;

		error = table(procp, arg, nel_done);
    	}

	if (NCPUS == 1 || serial)
		unix_release();
	error = end_server_op(error, interrupt);
	if (error && *addr) {
		(void) vm_deallocate(mach_task_self(), (vm_offset_t) *addr,
				     *count);
		*count = 0;
	}
	return(error);
}

int
bsd_emulator_error(proc_port, error_message, size)
	mach_port_t	proc_port;
	char *		error_message;
	int		size;
{
	error_message[size] = '\0';
	bootnode_printf("%s\n", error_message);
#ifdef	TNC
	vproc_end_port_op(port_to_proc_lookup(proc_port)->p_vproc,
			  "bsd_emulator_error");
#endif	/* TNC */
	return KERN_SUCCESS;
}

/* Not a system call.  Used by the emulator when it releases a shared lock
 * and notices that there is a server thread sleeping, waiting for the lock.
 */
int
bsd_share_wakeup(proc_port, offset)
	mach_port_t	proc_port;
	int		offset;
{
#if	MAP_UAREA
	register struct proc	*p;
	register struct shared_lock *x;

	if ((p = port_to_proc_lookup(proc_port)) == (struct proc *)0)
		return (ESRCH);
#ifdef notdef
	SC_TRACE(("[%d]bsd_share_wakeup(%x, %x)", p->p_pid, (int)p, offset));
#endif
	x = (struct shared_lock *) ((int)(p->p_shared_rw) + offset);
	mutex_lock(&p->p_shared_mutex);
	x->timed_out = 0;
	condition_signal(&x->lock_is_free);
	mutex_unlock(&p->p_shared_mutex);
#ifdef	TNC
	vproc_end_port_op(p->p_vproc, "bsd_share_wakeup");
#endif	/* TNC */
	return (0);
#else MAP_UAREA
	return (EINVAL);
#endif MAP_UAREA
}

int
bsd_execve(proc_port, interrupt,
	   fname, fname_count,
	   cfname,
	   cfarg,
	   entry,
	   entry_count,
	   auxv_structs,
	   auxv_structs_count,
	   auxv_strings,
	   auxv_strings_count,
	   traced)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*fname;
	unsigned int	fname_count;
	char		*cfname;	/* OUT */
	char		*cfarg;		/* OUT */
	int		*entry;		/* pointer to OUT array */
	unsigned int	*entry_count;	/* OUT */
	auxv_mig_t	auxv_structs;	/* OUT */
	unsigned int	*auxv_structs_count;	/* OUT */
	auxv_strings_t	auxv_strings;	/* OUT */
	unsigned int	*auxv_strings_count;	/* OUT */
	boolean_t	*traced;	/* OUT */
{
	struct execr {				/* exec return arguments */
		char		*cfname;	/* shell file name */
		char		*cfarg;		/* shell args */
		int		*entry;		/* pointer to pc entry points */
		unsigned int	*entry_count;	/* number of entries */
	} rtv;

	START_SERVER(SYS_execve, 3)

	SC_TRACE(("(%s)",fname));

	arg[0] = (int)fname;		/* file name for exec */
	arg[1] = 0;			/* handled in the emulator */
	arg[2] = 0;			/* handled in the emulator */

	rtv.cfname = cfname;
	rtv.cfarg = cfarg;
	rtv.entry = entry;
	rtv.entry_count = entry_count;

	procp->p_auxv_structs = auxv_structs;
	procp->p_auxv_strings = auxv_strings;

	error = execve(procp, arg, &rtv);

	*auxv_structs_count = procp->p_auxv_structs_count;
	*auxv_strings_count = procp->p_auxv_strings_count;

	*traced = (procp->p_flag & STRC);

	END_SERVER(SYS_execve)
}

int
bsd_exec_args_set(proc_port, interrupt,
		  argp,
		  arg_size,
		  envp,
		  env_size)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	char		*argp;
	u_short		arg_size;
	char		*envp;
	u_short		env_size;
{
	register int 	error;

	if (error = start_server_op(proc_port, 1003))
		return (error);

	error = exec_args_set(u.u_procp, argp, arg_size, envp, env_size);

	return (end_server_op(error, interrupt));
}
	
int
bsd_profil_set(proc_port, interrupt, buff, bufsiz,
			offset, scale)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	 /* out */ 
        int 	        buff;
	int		bufsiz;
	int		offset;
	int		scale;
{
	START_SERVER(SYS_profil, 4)

	arg[0] = buff;
	arg[1] = bufsiz;
	arg[2] = offset;
	arg[3] = scale;

	error = profil_set((struct proc *)proc_port, arg, (void *)0);

	END_SERVER(SYS_profil)
}

int
bsd_profil_get(proc_port, interrupt, uaddr,
			out_addr, out_count)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	 /* out */ 
        int 	        *uaddr;		 /* out */
	char		**out_addr; 	 /* out, dealloc (ool memory) */
	int		*out_count; 	 /* out, size of the above */
{
	START_SERVER(SYS_profil, 4)

	/*
	 * Always init MIG's OUT params for ports and ool memory to null,
	 * lest random values be accidentally transmitted in the reply.
	 */
	*out_addr = 0;
	*out_count = 0;

	if (vm_allocate(mach_task_self(), (vm_offset_t *)out_addr,
			uth->u_prof.pr_size, TRUE) != KERN_SUCCESS) {
		error = ENOBUFS;
	} else {
		arg[0] = (int) *out_addr;
		error = profil_get((struct proc *)proc_port, arg, (void *)0);
        }

	if (error && *out_addr) {
		(void) vm_deallocate(mach_task_self(), (vm_offset_t) *out_addr,
				     uth->u_prof.pr_size);
	}
	if (error == 0) {
		*out_count = (int) uth->u_prof.pr_size;
		*uaddr    = (int) uth->u_prof_buf;
	}

	if (NCPUS == 1 || serial)
		unix_release();
	error = end_server_op(error, interrupt);
	return (error);
}

int
bsd_init_process(proc_port, interrupt)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
{
	register int	error;

	if (error = start_server_op(proc_port, 1002))
	    return (error);

	error = init_process();

	return (end_server_op(error, interrupt));
}

int
bsd_msleep(proc_port, interrupt, msem_addr)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		msem_addr;
{
	START_SERVER(SYS_msleep, 1)

	SC_TRACE(("(%x)",msem_addr));
	arg[0] = msem_addr;
	error = msleep((struct proc *)proc_port, arg, 0);

	END_SERVER(SYS_msleep)
}

int
bsd_mwakeup(proc_port, interrupt, msem_addr)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		msem_addr;
{
	START_SERVER(SYS_mwakeup, 1)

	SC_TRACE(("(%x)",msem_addr));
	arg[0] = msem_addr;
	error = mwakeup((struct proc *)proc_port, arg, 0);

	END_SERVER(SYS_mwakeup)
}

/*
 * Store the emulator addresses for recovering the emulator from a user
 * address exception, so it can return EFAULT to the user:
 */
int
bsd_emul_uacc(proc_port, uacc_start, uacc_end, uacc_err)
	mach_port_t	proc_port;
	int uacc_start;
	int uacc_end;
	int uacc_err;
{
	struct proc *p = port_to_proc_lookup(proc_port);

	if (p == (struct proc *)0)
		return ESRCH;
	if (p->p_pid == 1) {
#ifdef notdef
                SC_TRACE(p->p_pid, ("(0x%x, 0x%x)\n",
                                    uacc_end, uacc_err));
#endif
		set_emul_uacc(uacc_start, uacc_end, uacc_err);
		return 0;
	} else {
		printf("bsd_emul_uacc: illegally invoked from proc %d(%s)\n",
		       p->p_pid, p->p_utask.uu_comm);
		return EPERM;
	}
}

int
bsd_socket(proc_port, interrupt, domain, type, protocol, fpp)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	int			domain;
	int			type;
	int			protocol;
	mach_port_t		*fpp;		/* OUT */
{
	struct file *fp;

	START_SERVER(SYS_socket, 4)

	arg[0] = domain;
	arg[1] = type;
	arg[2] = protocol;
	arg[3] = (int) &fp;
	error = socket(procp, arg, 0);
	if (error == 0)
                FILE_TO_PORT_LOOKUP(fp, *fpp);
	else
		*fpp = MACH_PORT_NULL;

	END_SERVER(SYS_socket)
}

int
bsd_socketpair(proc_port, interrupt, domain, type, protocol, fpp1, fpp2)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	int			domain;
	int			type;
	int			protocol;
	mach_port_t		*fpp1;		/* OUT */
	mach_port_t		*fpp2;		/* OUT */
{
	struct file *fp1, *fp2;

	START_SERVER(SYS_socketpair, 6)

	arg[0] = domain;
	arg[1] = type;
	arg[2] = protocol;
	arg[3] = (int) &fp1;
	arg[4] = (int) &fp2;
	error = socketpair(procp, arg, 0);
	if (error == 0) {
                FILE_TO_PORT_LOOKUP(fp1, *fpp1);
                FILE_TO_PORT_LOOKUP(fp2, *fpp2);
	} else {
		*fpp1 = MACH_PORT_NULL;
		*fpp2 = MACH_PORT_NULL;
	}

	END_SERVER(SYS_socketpair)
}

int
bsd_pipe(proc_port, interrupt, fpp1, fpp2)
	mach_port_t		proc_port;
	boolean_t		*interrupt;
	mach_port_t		*fpp1;		/* OUT */
	mach_port_t		*fpp2;		/* OUT */
{
	struct file *fp1, *fp2;

	START_SERVER(SYS_pipe, 2)

	arg[0] =(int) &fp1;
	arg[1] =(int) &fp2;
	error = pipe(procp, arg, 0);
	if (error == 0) {
                FILE_TO_PORT_LOOKUP(fp1, *fpp1);
                FILE_TO_PORT_LOOKUP(fp2, *fpp2);
	} else {
		*fpp1 = MACH_PORT_NULL;
		*fpp2 = MACH_PORT_NULL;
	}

	END_SERVER(SYS_pipe)
}


/*
 * Select & poll.
 *
 * None of these are server-side interruptible, so the a 0 transaction_id is
 * used below.
 * 
 * These use select's system call number on the setup side,
 * and poll's system call number on the delayed reply side.
 *
 * select_enqueue() takes a send right on the creds port and a ref on the file
 * structure when it enqueues the event.  The send right is consumed by the
 * credentials_register in sbsd_sel_poll_reply; the ref is explicitly released
 * there.
 */
#ifndef SELQDEBUG
#define SELQDEBUG(s)
#else
#undef SELQDEBUG
#define SELQDEBUG(s) if (selqdebug) s
extern int selqdebug;
#endif
#ifndef SELDEBUG
#define	SELDEBUG(s)
#else
#undef	SELDEBUG
#define SELDEBUG(s)	s
#endif
int
bsd_sel_poll_immed(file_port, creds_port, events, revents)
	mach_port_t	file_port;
	mach_port_t	creds_port;
	short		events;
	short		*revents;		/*OUT */
{
	return bsd_sel_poll_delay(file_port, /*file_port_send*/MACH_PORT_NULL,
				  creds_port, /*delay_port*/MACH_PORT_NULL,
				  events, /*index*/(short)-1, revents);
}


int
bsd_sel_poll_delay(file_port, file_port_send, creds_port, delay_port, events,
		   index, revents)
	mach_port_t	file_port;
	mach_port_t	file_port_send;		/* MACH_PORT_NULL if _immed */
	mach_port_t	creds_port;
	mach_port_t	delay_port;		/* MACH_PORT_NULL if _immed */
	short		events;
	short		index;			/* index==-1, no delayed rep */
	short		*revents;		/* OUT */
{
	struct uthread	*uth = &u;
	struct file	*filep;
	kern_return_t	error;
	boolean_t	freeports = FALSE;
        int		serial = !sysent[SYS_select].sy_parallel;
	int		dummy = 0;


	*revents = 0;		/* a mig call: *revents could be anything */

	/* Verify args */
	ASSERT((index >= 0 && delay_port != MACH_PORT_NULL &&
		file_port_send == file_port) ||
	       (index == -1 && delay_port == MACH_PORT_NULL
		&& file_port_send == MACH_PORT_NULL));

	error = start_fileserver_op(&filep, file_port, creds_port,
				/*transid*/0, SYS_select, serial);
	if (error)
		return error;

	/*
	 * Take a file port reference that will go along with the extra send
	 * right in file_port_send.  This exists for the life of file_port_send
	 * (aka the life of the select - until the drp becomes a dead-name or
	 * the select is satisfied).
	 */
	if (file_port_send != MACH_PORT_NULL) {
		extern void fp_ref_port_svr();
		fp_ref_port_svr(filep);
	}
	uth->uu_reply_msg = 0;

	/*
	 * Store some things for when select_enqueue() is called.
	 */
	uth->uu_sel_file_port = file_port;
	uth->uu_sel_delay_port = delay_port;
	uth->uu_sel_events = events;
	uth->uu_sel_index = index;
	uth->uu_sel_again = FALSE;		/* 1st time through */
#ifdef TNC
	uth->uu_sel_id = SEL_ID_NULL;
	uth->uu_sel_flags = 0;
#endif

	/*
	 * Here we call the device driver's select routine.  That will
	 * either fill in revents or call select_enqueue.  If select_enqueue
	 * does enqueue the request, it takes responsibility for delay_port.
	 * O/w, we have get rid of it and file_port_send, if any.
	 */
	FOP_SELECT(filep, &events, revents, 1, error);
	if (*revents != 0 || index == (short)-1 || error != 0) {
		/*
		 * "Clear out all selq entries generated above; if not cleaned
		 * up here, they'll be cleared later."
		 * No they won't get cleared up later, not anymore.
		 *
		 * This whole thing is silly; to effect a poll, we've enqueued
		 * a wakeup request.  This call then tears that down.
		 * Actually, the AD select code will not enqueue when
		 * index==-1 (or delay_port==NULL), making this second
		 * FOP_SELECT needless.
#ifdef	TNC
		 * However!  TNC virtual sockets needs this FOP_SELECT call
		 * to clean up all the remote secondary sockets.  I leave
		 * the above comments in place for historical reasons, and in
		 * the hope that whoever wrote them will one day clean them up.
#endif
		 */
		SELQDEBUG(printf("*** delay tear down %x error %d\n",
			delay_port, error));
		uth->uu_sel_delay_port = delay_port;
		FOP_SELECT(filep, &events, &dummy, 0, dummy);
#ifdef	TNC
		if (uth->uu_sel_id != SEL_ID_NULL) {
			ASSERT_DRP_UNMAPPED(delay_port, 0/*islocked*/,
			    ("_delay clnup failed, drp 0x%x, uu_sel_id 0x%x, "
			     "uu_selfags 0x%x\n", delay_port, uth->uu_sel_id,
			     uth->uu_sel_flags));
		}
#endif
		/* select_dequeue will never free ports */
		if (delay_port != MACH_PORT_NULL) {
			freeports = TRUE;
		}
	} else if (uth->uu_sel_delay_port == delay_port) {
		/*
		 * This indicates that select_enqueue was never called.
		 * Thus, be sure to cleanup.
		 */
#ifdef	TNC
		ASSERT_DRP_UNMAPPED(delay_port, 0/*islocked*/,
				    ("_delay: enqueue never called, drp 0x%x "
				     "uu_sel_id 0x%x uu_selfalgs 0x%x\n",
				     delay_port, uth->uu_sel_id,
				     uth->uu_sel_flags));
#endif
		freeports = TRUE;
	}

	/*
	 * freeports can only be true when this select is not enqueued.
	 * In particular, this also means that delay_port and file_port_send
	 * are exant.
	 */
	if (freeports) {
		kern_return_t ret;

		/*
		 * discard the delay reply port (select_dequeue will not)
		 */
#ifdef	TNC
		if (uth->uu_sel_id != SEL_ID_NULL)
		    ASSERT_DRP_UNMAPPED(delay_port, 0/*islocked*/,
			("_delay, freeports, uu_sel_id=%x, uu_sel_flags=%x\n",
			 uth->uu_sel_id, uth->uu_sel_flags));
#endif
		ret = mach_port_deallocate(mach_task_self(), delay_port);
		if (ret != KERN_SUCCESS) {
			SELQDEBUG(
			    printf("bsd_sel_poll_delay: deallocate drp %x=%d\n",
				   delay_port, ret));
		}

		/*
		 * discard the send right on the file port (created by the MiG)
		 * and drop the extra file reference that was taken
		 */
		fp_unref_port(file_port_send,-1);
	}

	return end_fileserver_op(filep, error, serial);
}


struct ps_reply ps_reply_template = {
	{
		MACH_MSGH_BITS(MACH_MSG_TYPE_MOVE_SEND_ONCE, 0),
						/* msgh_bits */
		sizeof(struct ps_reply),	/* msgh_size */
		MACH_PORT_NULL,			/* msgh_remote_port */
		MACH_PORT_NULL,			/* msgh_local_port */
		0,				/* msgh_seqno */
		POLL_SEL_REPLY_MSG_ID		/* msgh_id */
	},
	{
		MACH_MSG_TYPE_INTEGER_32,	/* msgt_name */
		32,				/* msgt_size */
		2,				/* msgt_number 2 ints */
		TRUE,				/* msgt_inline */
		FALSE,				/* msgt_longform */
		FALSE,				/* msgt_deallocate */
		0				/* msgt_unused */
	},
	0,0					/* the two ints themselves */
};


/*
 * sbsd_sel_poll_reply - ubsd_ version of this routine is called
 *			 from select_wakeup() in subr_select.c
 *
 * This inherits control of:
 *	- an explicit file port send right (in file_port_send)
 *	- an implicit extra file struct ref to go along with the send right
 *	- a send once right named by delay_port; this is passed in as a
 *	  PORT_NAME, but this always works.
 *	- a credentials port send right that was consed up by select_enqueue()
 *	  (this is consumed in the start/end_fileserver_op)
 *
 * The first three are either passed back down to select_enqueue() or
 * reaped as necessary.
 *
#ifdef	TNC
 *  If the sel_id is non-NULL, the delay_port passed as an argument is
 *  bogus and can be ignored and safely overwritten.
#endif
 */
/*simpleroutine*/
kern_return_t
sbsd_sel_poll_reply(
	mach_port_t	file_port,
	mach_port_t	file_port_send,		/* send right */
	mach_port_t	creds_port,
	mach_port_t	delay_port,
#ifdef	TNC
	sel_id_t	sel_id,
	node_t		reply_node,
#endif
	short		index,
	short		events)
{
	register struct uthread	*uth = &u;
	struct proc	*p;
	struct file	*filep;
	short		revents = 0;
	int		error;
	boolean_t	dummy, freedrp = FALSE, freeref = FALSE;
        int		serial = FALSE;		/* do not take master lock */
	kern_return_t	ret;
	struct ps_reply	ps_reply = ps_reply_template;
#ifdef	TNC
	selid_drp_map_t	*sdm;
	extern node_t	this_node;
	int		old_flags; /*zzz*/

	if (sel_id != SEL_ID_NULL) {
		/*
		 *  Translate TNC select id to emulator delayed reply port.
		 *  If we can't find a drp, someone must have cleaned up
		 *  while this RPC was in transit.
		 */
		SELID_DRP_LOOKUP(sel_id, sdm);
		delay_port = (sdm ? sdm->sdm_drp : MACH_PORT_NULL);
		if (delay_port == MACH_PORT_NULL) {
			SELDEBUG(printf(
				"sbsd_sel_poll_reply: no drp for selid=0x%x\n",
				sel_id));
			/* The file_port_send right we inherited must go! */
			fp_unref_port((struct file *)file_port_send, -1);
			return KERN_SUCCESS;
	 	}
		SELID_DRP_LOOKUP_DONE(sdm);
	}
#endif	/* TNC */

	if (delay_port == MACH_PORT_DEAD) {
		SELQDEBUG(printf("**** sbsd_sel_poll_reply: MACH_PORT_DEAD\n"));
		return KERN_SUCCESS;
	}

	/* file_port_send is used to MOVE_SEND */
	ASSERT(file_port == file_port_send);

	/*
	 * transid == 0 since we are NOT interruptible!
	 */
	error = start_fileserver_op(&filep, file_port, creds_port,
				/*transid*/0, SYS_poll, serial);
	if (error) {
		SELQDEBUG(printf(
		    "**** sbsd_sel_poll_reply: start_fileserver_op: err 0x%x\n",
		     error));
		return KERN_SUCCESS;
	}

	/* init some values in the uarea that FOP_SELECT will need below */
	uth->uu_sel_file_port = file_port;
	uth->uu_sel_delay_port = delay_port;
#ifdef	TNC
	uth->uu_sel_id = sel_id;
	if (sel_id != SEL_ID_NULL) {
		uth->uu_sel_flags = (SQ_VSOCK|SQ_DRP);
		if (reply_node != this_node)
			uth->uu_sel_flags |= SQ_REMOTE_REPLY;
	} else {
		uth->uu_sel_flags = 0;
	}
#endif
	uth->uu_sel_index = index;
	uth->uu_sel_events = events;
	/* indicate that we're just coming around again */
	uth->uu_sel_again = TRUE;
	uth->uu_reply_msg = 0;

	/* see if there are any requested events out there */
	FOP_SELECT(filep, &events, &revents, 1, error);

	/* if we get something this time */
	if (revents != 0 || error != 0) {
		/*
		 * clear out new selq entries this generates
		 * (but it shouldn't generate any)
		 */
		SELQDEBUG(printf("*** reply tear down %x error %d\n",
			delay_port, error));
		uth->uu_sel_delay_port = delay_port;
		FOP_SELECT(filep, &events, &dummy, 0, dummy);
#if	defined(TNC) && MACH_ASSERT
		uth->uu_sel_flags |= 0x80000000; /*zzz*/
		if (sel_id) {
			/* Should be unmapped whether SQ_VSOCK or not. */
			ASSERT_DRP_UNMAPPED(delay_port, 0/*not locked*/,
			    ("_reply clnup, uu_sel_id=%x uu_sel_flags=%x "
			     "oldflags=%x oldselid=%x fp=%x\n",
			     uth->uu_sel_id,
			     uth->uu_sel_flags,
			     old_flags, sel_id, filep));
		}
#endif
		if (error != 0) {
#ifdef TNC
			uth->uu_sel_flags |= 0x40000000; /*zzz*/
#endif /* TNC */
			freedrp = TRUE;
		} else {

			/* use specified port */
			ps_reply.hdr.msgh_remote_port = delay_port;

			/* tell emulator the index & returned events */
			ps_reply.index = index;
			ps_reply.revents = revents;

			/*
			 * This will consume the delay reply port, and
			 * discard the d-n notification.  Else, if the
			 * drp just became a d-n, this will fail, and we
			 * don't care.
			 */
			ret = mach_msg(&ps_reply.hdr,
				MACH_SEND_MSG|MACH_SEND_CANCEL,
				ps_reply.hdr.msgh_size, 0, MACH_PORT_NULL,
				MACH_MSG_TIMEOUT_NONE, file_port);
			if (ret == MACH_SEND_INVALID_DEST) {
				/*
				 * This occurs when the send-once has gone
				 * away.  However, since we've disabled
				 * the dead-name reap code at this point,
				 * we need to free the 2 urefs on the dead-name
				 * at this point.
				 */
				 ret = mach_port_mod_refs(mach_task_self(),
						delay_port,
						MACH_PORT_RIGHT_DEAD_NAME,
						-2);
				if (ret)
					SELQDEBUG(printf("mod ret=%d\n", ret));
			} else if (ret)
				SELQDEBUG(printf("reply ret=%d\n", ret));
		}
		freeref = TRUE;
	} else if (uth->uu_sel_delay_port == delay_port) {
		/*
		 * This indicates that select enqueue was never called
		 * Thus, be sure to cleanup
		 */
#ifdef	TNC
		/* Must clean up remote secondaries too! */
		if (sel_id) {
			uth->uu_sel_flags |= 0x20000000; /*zzz*/
			uth->uu_sel_delay_port = delay_port;
			FOP_SELECT(filep, &events, &dummy, 0, dummy);
		}
#endif
		freeref = TRUE;
		freedrp = TRUE;
	} else {
		/* else, we've just requeue'd this request */
		SELQDEBUG(printf("sbsd_sel_poll_reply(drp=%x) no events!\n",
			delay_port));
	}
	if (freedrp) {
#ifdef TNC	   
		ASSERT_DRP_UNMAPPED(delay_port, 0/*not locked*/,
			    ("..._reply, freedrp: uu_sel_id %x\n",
			     uth->uu_sel_id));
#endif /* TNC */

		ret = mach_port_deallocate(mach_task_self(), delay_port);
		if (ret != KERN_SUCCESS)
		    SELQDEBUG(printf
			      ("bsd_sel_poll_reply: deallocate drp %x=%d\n",
			       file_port, ret));
	}
	if (freeref) {
		/*
		 * Free the extra file_port send right
		 * and free the implicitly passed in file pointer reference
		 */
		fp_unref_port(file_port_send, -1);
	}

	if (error != ESUCCESS)
		SELQDEBUG(printf("sbsd_sel_poll_reply: error %d (ignored)\n",
				 error));
	(void) end_fileserver_op(filep, error, serial);
	return KERN_SUCCESS;
}


/*
 * Hook for emulator to register itself with the server.
 */
int
bsd_register_proc(proc_port, cb_thread, cb_port, rlimit_fsize)
	mach_port_t	proc_port, cb_thread, cb_port;
	unsigned int	*rlimit_fsize;
{
	struct proc	*procp;
	int		error;

	procp = port_to_proc_lookup(proc_port);
	ASSERT(procp);
	if (!procp->p_callback) {
		procp->p_callback_thread = cb_thread;
		procp->p_callback = cb_port;
		error = KERN_SUCCESS;
	} else
		error = EPERM;

	*rlimit_fsize = procp->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur;
		
#ifdef	TNC
	vproc_end_port_op(port_to_proc_lookup(proc_port)->p_vproc,
		  "bsd_register_proc");
#endif	/* TNC */

	return error;
}

int 
bsd_mmap(proc_port, fport, addr, len, prot, flags, pos, retval,
		interrupt)
        mach_port_t  proc_port;
        mach_port_t  fport;
        caddr_t	     addr;
        size_t	     len;
        int          prot;
        int	     flags;
        off_t	     pos;
	caddr_t      *retval;
        boolean_t    *interrupt;
{
        START_SERVER(SYS_mmap, 6)

        arg[0] = fport;
        arg[1] = (int) addr;
        arg[2] = len;
        arg[3] = prot;
        arg[4] = flags;
        arg[5] = pos;

        error = smmap(procp, arg, retval);
        
        END_SERVER(SYS_mmap)
}

/*
 * Force the caller to exit.
 */
int
bsd_proc_exit(proc_port, interrupt, signal, dump)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	int		signal;
	boolean_t	dump;
{
	register int 	error;

	if (error = start_server_op(proc_port, 1004))
		return (error);
	unix_master();
	
	SC_TRACE(("(%d, 0x%x)\n", signal, dump));
	error = proc_exit(u.u_procp, signal, dump);

	unix_release();
	return (end_server_op(error, interrupt));
}

/*
 * Send a process a signal (not a system call; used by the emulator)
 */
int
bsd_psignal(proc_port, interrupt, signal)
	mach_port_t	proc_port;
	boolean_t	*interrupt;	/* out */
	int		signal;
{
	register int	error;
	uthread_t	uth = &u;

	if (error = start_server_op(proc_port, 1006))
		return(error);

	uth->uu_reply_msg = 0;
	unix_master();
	error = psignal(u.u_procp, signal);
	unix_release();

	return(end_server_op(error, interrupt));
}
