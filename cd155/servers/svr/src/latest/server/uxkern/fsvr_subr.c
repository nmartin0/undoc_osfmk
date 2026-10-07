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
 * $Log: fsvr_subr.c,v $
 * Revision 1.25  1995/03/04  00:19:50  toman
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 12495
 *  Testing: EATs controlc, os_interfaces, sched
 *  Module(s): server/uxkern/fsvr_subr.c
 *  Checkin by: Bob Yasi (borrowing uid <toman>)
 *
 * Cleaned up fsvr dummy pprocs allocation, including zeroing the dummy
 * structure before each use.
 *
 * Revision 1.24  1995/03/03  18:59:44  toman
 *  Reviewer: Bob Yasi, John Litvin
 *  Risk: Medium (due to number of lines changed)
 *  Benefit or PTS #: 12508
 *  Testing: Tested syscall tracing with multiple netservers
 *  Module(s): server/uxkern/fsvr_subr.c
 *             server/vsocket/vs_ipc.c
 *             server/vsocket/vs_subr.c
 *             server/vsocket/vsocket.h
 *  Made new vsocket syscodes and passed them as parameters to
 *  start_vsockserver_op().  Also added corresponding names to a
 *  list of names in fsvr_thread_initialize(), which are displayed
 *  for extra syscodes (>= 2000) when syscall tracing is enabled.
 *  Also added parameter "serial" to start_vsockserver_op() and
 *  end_vsockserver_op(), but all vsocket operations currently pass
 *  0 for this param.
 *
 * Revision 1.23  1995/02/21  21:52:08  yazz
 *  Reviewer: Suri Brahmaroutu, Nina Lepak
 *  Risk: Lo
 *  Benefit or PTS #: 10994 C-0
 *  Testing: 500 NQs start/stop iterations + EATs controlc, sched
 *  Module(s): server/uxkern/fsvr_subr.c
 *
 * In fsvr_get_proc(), must zero p_sigignore field for new proc structs or
 * mpsleep() will fail to wake up fsvr thread properly.
 *
 * Revision 1.22  1995/02/01  22:25:22  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.21  1995/01/31  00:20:46  bolsen
 *  Reviewer: Jerry Toman, Bob Yasi, Surender Brahmaroutu
 *  Risk: Medium
 *  Benefit or PTS #: 12264
 *  Testing: ran several commands and checked syscall counts with a test
 *           program.
 *  Module(s): server/uxkern/fsvr_subr.c	1.20
 * 			 syscall_subr.c		1.15
 *
 *  Added code to increment the sccount[] array for system calls in the rest
 *  of the start system call functions.  This fixes the count values for all
 *  system calls.
 *
 * Revision 1.20  1994/11/18  20:48:24  mtm
 * Copyright additions/changes
 *
 * Revision 1.19  1994/10/25  22:40:02  yazz
 *  Reviewer: Nandini Ajmani
 *  Risk: Lo
 *  Benefit or PTS #: 11128
 *  Testing: EATs: controlc, sched, os_interfaces, messages, rmcall
 *  Module(s):
 * 	server/i386/bsd_machdep.c
 * 	server/i386/slock.s
 * 	server/i860/bsd_machdep.c
 * 	server/i860/slock.s
 * 	server/kern/parallel.h
 * 	server/kern/sched_prim.c
 * 	server/sys/unix_defs.h
 * 	server/tnc/rvp_subr.c
 * 	server/uxkern/cred_servers.c
 * 	server/uxkern/emul_user.c
 * 	server/uxkern/fsvr_subr.c
 * 	server/uxkern/ux_server_loop.c
 *
 * Make fsvr_thread_initialize() init the master lock hold counter to 0, and
 * make fsvr_uarea_terminate() assert that count has returned to 0.
 *
 * Revision 1.18  1994/05/04  22:04:18  mjl
 * TNC select rewrite.  Initialize uthread select-related fields when
 * allocating a u-area for a file server op.  Also get rid of some
 * compiler warnings.
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
 * Revision 1.17  1994/02/22  21:28:44  dbm
 * Mainline check in for bug fix #8179.   This is described in revision
 * 1.15.4.1 in R1.2.
 *
 * Revision 1.16  1994/01/12  17:46:31  jlitvin
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
 * Revision 1.15.4.1  1994/02/22  21:21:08  dbm
 * Initialized the p_sigmask field to 0 for dummy proc structures used
 * by file server ops.  This value was uninitialized and caused signals
 * to be masked incorrectly.
 *
 *  Reviewer: Dave Minturn, Nandini Ajmani
 *  Risk:Low
 *  Benefit or PTS #:8179
 *  Testing: Specific test case and ran VSX Eats.
 *  Module(s):
 * 	fsvr_subr.c
 *
 * Revision 1.15  1993/07/14  18:42:19  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:03:03  cfj
 * Adding new code from vendor
 *
 * Revision 1.14  1993/05/20  16:03:57  cfj
 * Merge of 05-18-93 code drop from Locus.
 *
 * Revision 1.13  1993/05/07  18:42:03  nandy
 * Fixed merge conflict.
 *
 * Revision 1.12  1993/05/06  19:30:02  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.11  1993/04/03  03:12:02  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.10  1993/03/29  23:08:08  nandy
 * Merged from T9 branch
 *
 * Revision 1.4.4.5  1993/03/29  22:53:36  nandy
 * Chages from loverso for OIP intr problem.
 *  Revision 2.40  93/03/24  15:38:30  loverso
 *  Replace previous restructuring with oip_intr_all() calling new
 *  mpportid_hash_forall_func() to avoid busy-spinning and accessing
 *  a uthread which has gone away.  Completely eliminate "oip mismatch"
 *  case (it becomes an ASSERT) and correctly fix original deadlock by
 *  use of simple_lock_try().  (loverso)
 *
 * Revision 1.4.4.4  1993/03/10  17:17:11  nandy
 * Merged from the main stem.
 *
 * Revision 1.8  1993/03/10  17:07:39  nandy
 * Restructure oip_intr_all() to remove deadlock with syscall end path.
 * Initialize the server_oip structure for each uthread.
 * (loverso)
 *
 * Revision 1.7  1993/03/07  01:33:53  cfj
 * Merge from T9.
 *
 * Revision 1.4.4.3  1993/03/07  01:16:46  cfj
 * Fix to make TNC work with OSF sync. close.
 *
 * Revision 1.4.4.2  1993/03/03  19:46:04  cfj
 * Incorrporated a bug fix from ASTG where start_fileserver_op() was
 * not incrementing he sequence number after an error had been returned
 * from fsvr_thread_initialize().
 *
 * Revision 1.4.4.1  1993/02/16  20:39:10  cfj
 * Synchronous close from OSF.
 *
 * Revision 1.1.2.1.2.3  1993/02/16  20:08:24  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/22  17:28:01  cfj
 * 01-20-93 Locus code drop.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  22:55:40  dbm
 * Added PFS token functionality.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:05:23  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:06:17  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:54:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:43:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.37  93/06/16  15:28:24  klh
 * 	Revision 2.42  93/05/16  20:58:53  loverso
 * 		Handle "intr" parameter (force EINTR or ERESTART) parameter for
 * 		oip_intr().  (Passed via the uthread structure with exiting)
 *
 * Revision 2.36  93/06/02  12:36:19  yazz
 * For Sys V IPC under TNC add the start & end svipc server operation routines,
 * svipc data structure lock and initialization.
 *
 * Revision 2.35  93/05/13  11:34:05  mjl
 * Add calls to file_port_increment_seqno() in error paths of start_file*_op()
 * routines.  *All* RPCs on file ports must be counted, even losing ones.
 *
 * Revision 2.34  1993/04/29  14:06:33  klh
 * 	Revision 2.41  93/04/05  18:04:47  durriya
 * 		msg2mbuf now copies data to an mbuf only if it can fit in one
 * 		mbuf. Otherwise it uses the external mbuf technique to save a
 * 		data copy
 * 		[93/04/05            durriya]
 *
 * 	Revision 2.40  93/03/24  15:38:30  loverso
 * 		Replace previous restructuring with oip_intr_all() calling new
 * 		mpportid_hash_forall_func() to avoid busy-spinning and accessing
 * 		a uthread which has gone away.  Completely eliminate "oip mismatch"
 * 		case (it becomes an ASSERT) and correctly fix original deadlock by
 * 		use of simple_lock_try().  (loverso)
 *
 * 	Revision 2.39  93/03/05  17:46:25  loverso
 * 		Restructure oip_intr_all() to remove deadlock with syscall end path.
 * 		Initialize the server_oip structure for each uthread.
 * 		(loverso)
 *
 * 	Revision 2.38  93/02/18  11:50:38  rabii
 * 		start_notify_op now has the dummy proc get a reference to the rcred
 * 		of proc[0] and end_notify_op releases it. Needed for operations which
 * 		expect a valid rcred while the corresponding process has been
 * 		terminated (rabii)
 *
 * 	Revision 2.36  93/01/26  11:05:28  durriya
 * 		increase initial size of dummy_proc_zone                     (durriya)
 *
 * 	Revision 2.35  93/01/12  17:06:00  roy
 * 		Added "open-with-token" to syscall tracing.
 * 		[93/01/04            roy]
 *
 * 	Revision 2.33  93/01/08  14:35:05  durriya
 * 		Fix syscall tracing in fsvr_thread_terminate.
 * 		[93/01/04            roy]
 *
 * Revision 2.33  93/04/03  11:50:04  klh
 * Enhance panic message for more debug information. (klh for bhk)
 *
 * Revision 2.32  93/03/10  14:21:35  yazz
 * Synchronous close merge from Intel.
 *
 * 	Revision 1.4.4.2  1993/03/03  19:46:04  cfj
 * 	Incorrporated a bug fix from ASTG where start_fileserver_op() was
 * 	not incrementing he sequence number after an error had been returned
 * 	from fsvr_thread_initialize().
 *
 * 	Revision 1.4.4.1  1993/02/16  20:39:10  cfj
 * 	Synchronous close from OSF.
 *
 * 	Revision 2.34  93/01/11  14:38:58  mmp
 * 	 Synchronous close support: add end_fileserver_op_nounref, which is
 * 	 end_fileserver_op without the FP_UNREF.  (mmp)
 *
 * 	Revision 1.4  1993/01/22  17:28:01  cfj
 * 	01-20-93 Locus code drop.
 *
 *   Revision 2.31  93/01/12  13:56:21  mjl
 *   Provide syscall tracing for various TNC "non-syscalls".  Allow an invalid
 *   creds port for the tnc_remote_fifo_death() RPC (this happens when racing
 *   against an exiting FIFO reader).
 *
 *   Revision 2.30  92/12/29  13:03:41  chrisp
 *   In call to zinit() for dummy_proc_zone, let max. size default to
 * 	  allocation and let this be size required for initial pre-allocation.
 *
 * 	Revision 1.3  1992/12/11  03:06:17  cfj
 * 	Merged 12-1-92 bug drop from Locus.
 *
 * 	Revision 1.2  1992/11/30  22:54:48  dleslie
 * 	Copy of NX branch back into main trunk
 *
 * 	Revision 1.1.2.1  1992/11/05  23:43:33  dleslie
 * 	Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.29  92/11/23  10:50:11  klh
 * Delete calls to no-op  tnc_fsvr_start_op() hook.  Now every start_*_op()
 * routine on a file port *must* call file_port_increment_seqno()! (This
 * routine now calls the TNC end-op hook). (klh for mjl).
 *
 * Revision 2.28  92/10/06  12:22:29  roman
 * Fix RCS comments.
 *
 * Revision 2.27  92/10/05  14:01:16  klh
 * 	Revision 2.30  92/10/05  12:08:56  rabii
 * 		Move routine for incrementing a token port sequence number to
 *		mf.c.
 * 		[92/10/01            roy]
 *
 * 	Revision 2.29  92/09/29  16:48:51  rabii
 * 		Remove debugging printf in end_token_op.
 * 		[92/09/28            roy]
 *
 * 	Revision 2.28  92/09/11  09:29:35  rabii
 * 		Added remote-print to syscall tracing (rabii).
 * 		[92/09/08            roy]
 *
 * 	Revision 2.27  92/08/26  12:14:03  loverso
 * 		Change start_vnodeserver_op to do vnode ref counting before
 *		possibly aborting with an error.
 * 		start_fsvrmisc_op may see a dead-name for the creds port due to
 *		a race with syscall exit.  This is ok; just ignore the request.
 * 		(loverso)
 *
 * Revision 2.26  92/09/28  13:35:17  klh
 * Added start/end_fsvr_miscop_with_proc(), needed so that the
 * tnc_remote_fifo_death() RPC can have a proc structure and u-area
 * (klh for mjl).
 *
 * Revision 2.25  92/08/08  01:53:37  jdh
 * make sure tnc_fsvr_end_op() is called for all file port
 * operations (it increments the port sequence count field) -- jdh
 *
 * Revision 2.24  92/08/06  13:33:34  klh
 * 	Revision 2.25  92/07/16  09:41:52  rabii
 * 		Check for success in calls to mach_port_deallocate (pjg).
 *
 * Revision 2.23  92/06/10  10:08:25  klh
 * 	Revision 2.24  92/06/08  18:30:42  pjg
 * 		Define msg2mbuf and mbuf2msg used for remote NFS server
 *		functions (durriya).
 *
 * Revision 2.22  92/06/05  17:46:01  klh
 * 	Revision 2.23  92/05/31  18:59:49  loverso
 * 		Remove duplicated include of parallel.h (pjg).
 *
 * 	Revision 2.22  92/05/24  13:58:48  pjg
 * 		Propagated the changes described below to
 *		fsvr_thread_inititalize and fsvr_thread_terminate (pjg).
 * 		Revision 3.12  92/03/24  21:04:22  barbou
 * 		Fix for bug #113: use cprintf() instead of printf() for syscall
 *		trace. Renamed TRACE() to SC_TRACE() to avoid conflict with
 *		sys/trace.h.
 *
 * 	Revision 2.21  92/05/18  12:27:44  roy
 * 		Revision 2.16.1.2  92/05/08  12:08:12  roy
 * 		Added sequence number synchronization to end_token_op.
 * 		[92/04/23            roy]
 *
 * 		Added token_not_found to syscall tracing.
 * 		[92/04/22            roy]
 *
 * 		Revision 2.16.1.1  92/04/22  10:04:50  roy
 * 		Added start_token_op and end_token_op for MAPPED_FILES.
 * 		[92/04/05            roy]
 *
 * 	Revision 2.20  92/05/12  10:52:00  loverso
 * 		Rename start_nosenders_op() to start_notify_op().
 * 		Delete start|end_fileserver_op_nocred(), as they are no
 *		longer needed. (loverso)
 * 		Get rid of ni_vp1, ni_vnode1, ni_vnodeport1, use ni_cdir
 *		instead (pjg).
 *
 * 	Revision 2.19  92/05/01  10:02:26  rabii
 * 		Fix typos in oip_register, oip_deregister, and oip_intr.
 * 		Add new start|end_fsvrmisc_op().  (loverso)
 *
 * Revision 2.21  92/04/14  10:49:22  roman
 * Keep Mach message reference counts on file port accurate.
 *
 * Revision 2.20  92/04/06  19:10:42  klh
 * For OSF merge, update version # to match LCC#
 *
 * Revision 2.18  92/04/05  17:09:32  pjg
 * 	Revised start_{vnode,exec,filevnode}server_op to use the new
 * 	vnode proxies (pjg).
 *
 * 	Removed deallocation of creds port from fsvr_thread_terminate and
 * 	pass in error to credentials_deregister (rabii)
 *
 * Revision 2.17  92/03/20  11:32:51  pjg
 * 	92/03/19  17:08:46  pjg
 * 	Use uth->u_procp->p_cred instead of uth->uu_nd.ni_credsport.
 * 	The credsport is now stored in the {dummy}proc only.
 *
 * 	92/03/19  17:08:46  loverso
 * 	Remove oip_credsport.  Don't register op if transid == 0.
 * 	Be heavy handed with the interruption in oip_intr if we are from exit.
 * 	Rename: fsvr_op_{,de}register --> oip_{,de}register
 *
 * Revision 2.16  92/03/16  18:28:46  pjg
 * 	92/03/06  20:42:36  noemi
 * 	Changed start_vnodeserver_op and start_execserver_op to initialize
 * 	u-area before translating ports and added back the calls to
 * 	MOUNT_TO_VNODE.
 *
 * Revision 2.15  92/03/15  14:30:37  roy
 * 	Added routine oip_intr, and iop_intr_all to handle server thread
 * 	interrupts (loverso)
 *
 * Revision 2.12  92/03/03  13:50:35  pjg
 * 	Initialize mount_port_hash_table with the correct size (pjg).
 * 	Changed type of server_oip_hash_table and converted the print_debug
 * 	to printf's in REMOVE_PORT and ENTER_PORT (should never happen)
 * 	(loverso).
 * 	Gracefully handle failure in credentials_register (rabii).
 *
 * Revision 2.11  92/03/01  18:33:43  pjg
 * 	92/02/28  pjg
 * 	Cleaned-up the error paths:
 * 	- the end_xxxserver_op routines cannot generate an error
 * 	- in case of error, nothing that came on a message can be deallocated,
 * 	  that is taken care in ux_server_loop
 * 	The start_xxxserver_op routines don't deallocate ports or memory
 * 	received in a message in case of error; the end_xxxserver_op routines
 * 	only deallocate ports or memory received in a message if (error == 0)
 * 	Initialize in fsvr_get_proc the dummy proc fields that may have been
 * 	modified by a previous file server operation.
 * 	Moved S_intr_delivery to fsvr_msg.c.
 * 	Don't use macro MOUNT_TO_VNODE because the server thread is not
 * 	initialized yet.
 *
 * 	92/02/28  loverso
 * 	Revamp fsvr_op_{reg,dereg}ister to use new uu_oip instead of servermsg.
 * 	Delete all signature port references
 * 	Delete all fileserver uses of "interrupt"
 *
 * 	Re-did credentials interface (rabii)
 *
 * 	92/02/28  17:02:05  noemi
 * 	Changed start_vnodeserver_op and start_execserver_op to attempt to
 * 	translate the request port to a mount structure port if the port
 * 	to vnode translation fails.  Removed bogus code in start_filevnode_op
 * 	and made minor change to end_vnodserver_op.
 *
 * Revision 2.10  92/02/21  16:56:55  durriya
 * 	allocate fsproc and servermsg structs from zones that are expandable
 *
 * Revision 2.9  92/02/11  21:49:29  pjg
 * 	Revision 2.8.1.2  92/01/30  11:15:11  loverso
 * 	    Add servermsg_null as a placeholder for non-interruptable system
 * 	    calls.
 * 	    Expand SM_POST_INTERRUPT inline.
 * 	    Fold intr_uthread into S_intr_delivery.
 * 	    Lock the proc while we are looking at it.
 * 	Revision 2.8.1.1  92/01/29  17:22:11  loverso
 * 	    Enable S_intr_delivery
 *
 * Revision 2.8  92/01/17  17:26:29  roy
 * 	Interruptible system call support (loverso).
 *
 * Revision 2.7  92/01/14  16:16:22  roy
 * 	After finished with p_rcred set it to NOCRED (not NULL).
 *
 * Revision 2.6  92/01/14  15:41:04  roy
 * 	Setup p->p_rcred in start_fileserver_op_nocred.
 *
 * Revision 2.5  92/01/14  11:29:15  roy
 * 	Slightly changed interface to credentials_uarea_register/deregister.
 * 	Added start and end ops for exec (pjg).
 *
 * Revision 2.4  92/01/09  23:02:01  roy
 * 	Make sure p->p_vproc is always set in fsvr_thread_initialize.
 *
 * Revision 2.3  92/01/09  16:05:28  roy
 * 	Fix credential handling.
 * 	Unix domain socket support (loverso).
 *
 * Revision 2.2  92/01/05  20:17:47  roy
 * 	92/01/02  14:15:18  roy
 * 	Complete overhaul to extract common code into routines.  Added:
 * 	fsvr_thread_initialize, fsvr_thread_terminate, fsvr_get_proc,
 * 	fsvr_release_proc, fsvr_op_register, fsvr_op_deregister,
 * 	fsvr_uarea_init, fsvr_uarea_terminate.
 *
 * 	91/12/28  16:45:16  roy
 * 	Take care of all error paths.
 *
 * 	1991/11/12  21:33:14  noemi
 * 	Added start_fsvrport_op and end_fsvrport_op functions.  Misc changes to
 * 	start_vnodeserver_op.  Fixed typos (naresh).
 *
 * 	1991/10/15  16:21:33  noemi
 * 	Allow start_fileserver_op, end_fileserver_op, start_vnodeserver_op and
 * 	end_vnodeserver_op to handle functions that do not receive credentials
 * 	ports.  This is a re-implementation of Locus mechanism.
 *
 * 	1991/10/14  20:46:43  noemi
 * 	Initial revision.  Based on code originally in syscall_subr.c.
 *
 * $EndLog$
 */

/*
 * Fileserver thread service routines.
 */
#include <map_uarea.h>
#include <mapped_files.h>

#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/user.h>
#include <sys/signal_macros.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/file.h>
#include <sys/vproc.h>
#include <sys/mbuf.h>
#include <sys/ipc.h>
#include <sys/ucred.h>
#include <kern/zalloc.h>
#include <kern/sched_prim.h>

#include <uxkern/bsd_msg.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/syscall_subr.h>

#include <uxkern/syscalltrace.h>
#include <uxkern/sthread.h>
#include <uxkern/port_hash.h>
#include <uxkern/mf.h>

#include <nfs/nfsm_subs.h>

#ifdef	SYSCALLTRACE
extern int	nsysent;
extern char	*syscallnames[];
#endif

extern int	scmax;			/* SEE uxkern/syscall_subr.c */
extern int	sccount[];		/* system call counts */

struct dummy_proc {
        struct proc fsproc;
#if MAP_UAREA
        struct ushared_ro us_ro;
        struct ushared_rw us_rw;
#endif
};

zone_t dummy_proc_zone;
extern int	ux_server_receive_max;
struct dummy_proc *fsproc_free;
int		nfsproc;
struct mutex	fsproc_lock;

#ifdef	TNC
struct mutex	svipc_lock;
zone_t		svipc_zone;
struct svipc	*svipc_head;
#endif	/* TNC */

mpportid_hash_table_t	server_oip_hash_table;
mpport_hash_table_t	mount_port_hash_table;


void
oip_init(uth)
	register uthread_t	uth;
{
	register struct server_oip	*oipp = &uth->uu_oip;

	oipp->oip_flag = OIP_IDLE;
	oipp->oip_transid = 0;
	oipp->oip_forwport = MACH_PORT_NULL;

	OIP_LOCK_INIT(oipp);
}



/*
 * Register an in-progress operation to support interruptible system calls.
 *
 * If no creds port or the transaction id is 0, then this does nothing.
 */
void
oip_register(uth, creds_port, transid)
	register uthread_t	uth;
	mach_port_t		creds_port;
	transaction_id_t	transid;
{
	register struct server_oip	*oipp = &uth->uu_oip;

	ASSERT(oipp->oip_flag == OIP_IDLE);
	ASSERT(oipp->oip_transid == 0);
	ASSERT(oipp->oip_forwport == MACH_PORT_NULL);

	if (creds_port == MACH_PORT_NULL || transid == 0)
		return;

	ASSERT(creds_port == uth->u_procp->p_cred);

	OIP_LOCK(oipp);

	oipp->oip_transid = transid;

	OIP_ENTER(creds_port, transid, uth);

	OIP_UNLOCK(oipp);
	/* interrupt requests for this syscall can now be fielded */
}

/*
 * Unregister an in-progress operation.
 */
void
oip_deregister(uth)
        register uthread_t      uth;
{
        register struct server_oip      *oipp = &uth->uu_oip;

        /* If we didn't do anything to register, do no work now */
        if (!OIP_INUSE(oipp))
                return;

        /* sync with oip_intr */
        OIP_LOCK(oipp);

        if (!OIP_REMOVE(uth->u_procp->p_cred, oipp->oip_transid, uth)) {
                printf("OIP_REMOVE(c=%x, t=%d, uth=%x) failed\n",
                        uth->u_procp->p_cred, oipp->oip_transid, uth);
                panic("oip_deregister");
        }

        oipp->oip_flag = OIP_IDLE;
        oipp->oip_transid = 0;
        oipp->oip_forwport = MACH_PORT_NULL;

        OIP_UNLOCK(oipp);
}

/*
 * Interrupt an operation in progress.
 *
 * credsport and transid must match the ones stored in the server_oip struct.
 *
 * If intr is TRUE, we should wakeup with an EINTR instead of ERESTART.
 *
 * If exiting is TRUE if we are in exit processing.
 * This will result in SWEXIT set on the dummy proc, as well as
 * clear_wait() being told to interrupt any sleep.
 *
 * The OIP_LOCK is assumed taken by the caller.
 */
int
oip_do_intr(credsport, transid, uth, intr, exiting)
	mach_port_t		credsport;
	transaction_id_t	transid;
	register uthread_t	uth;		/* of victum thread */
	boolean_t		intr, exiting;
{
	register struct server_oip	*oipp = &uth->uu_oip;
	register struct proc		*p;
	register int sig = SIGHUP, sigbit = sigmask(SIGHUP);

        /*
         * Must check this under the lock, or else the uthread could modify it
         */
        if (!OIP_INUSE(oipp)) {
                return KERN_SUCCESS;
        }

	/*
	 * We are now guaranteed that the uthread we are poking at has not
	 * gone away.  That includes not starting to work for some other
	 * syscall.
	 */
	ASSERT(uth->u_procp->p_cred == credsport &&
		oipp->oip_transid == transid);

	oipp->oip_flag |= OIP_INTERRUPT;

	/*
	 * If it's off on another node, forward this request
	 */
	if (OIP_FORWARDED(oipp)) {
		mach_port_t forwport;
		forwport = oipp->oip_forwport;

		return intr_delivery(forwport, credsport, transid,
				     intr, exiting);
	}

	p = uth->uu_procp;
	if (!p) {
		/* this should no longer happen, but... */
		return KERN_SUCCESS;
	}

	/* Tell dummy proc it should be "exiting" */

	simple_lock(&p->p_lock);

	/*
	 * Fake a signal;  Cause mpsleep to never sleep or generate
	 * EINTR/ERESTART on wakeup.
	 */
	if (intr)
		uth->u_sigintr |= sigbit;
	else
		uth->u_sigintr &= ~sigbit;
	p->p_sig = sigbit;
	p->p_cursig = sig;
	if (exiting) {
		p->p_flag |= SWEXIT;
	}
	simple_unlock(&p->p_lock);
	if (exiting)
		clear_wait(uth, THREAD_SHOULD_TERMINATE, FALSE);
	else
		clear_wait(uth, THREAD_INTERRUPTED, TRUE);

	return KERN_SUCCESS;
}



int
oip_intr(credsport, transid, uth)
	mach_port_t		credsport;
	transaction_id_t	transid;
	register uthread_t	uth;
{
	register struct server_oip	*oipp = &uth->uu_oip;
	int				ret = KERN_SUCCESS;

	/*
	 * If we cannot get the lock, then there is either someone else
	 * already in oip_deregister (or oip_intr) OR someone is in
	 * OIP_SET_FORW()/OIP_END_FORW().  In any case we really don't
	 * have much to do.
	 *
	 * If u.uu_oip_exiting, then the retry mechanism in the local
	 * cc_cache_getrusage() will retry this.  If we were racing
	 * with S_intr_delivery(), however, then the thread doing
	 * the actual interuption might not have u.uu_oip_exiting set.
	 * This is deemed "not bad".
	 *
	 * If u.uu_oip_exiting is FALSE, then the retry in sbsd_issig_psig()
	 * in the PM will try us again.
	 */
	if (OIP_LOCK_TRY(oipp)) {
		ret = oip_do_intr(credsport, transid, uth,
				  u.uu_oip_intr, u.uu_oip_exiting);
		OIP_UNLOCK(oipp);
	}
	return ret;
}

int
oip_intr_all(credsport)
	mach_port_t	credsport;
{
	int		count = 0;

	u.uu_oip_intr = TRUE;
	u.uu_oip_exiting = TRUE;

	/*
	 * For each port (regardless of transid) in hash table, call oip_intr.
 	 *
	 * This depends upon the retry mechanism in cc_cache_getrusage().
	 */
	count = OIP_FORALL(credsport, oip_intr);
	return count;
}


/*
 * Associate a fileserver thread with a temporary proc structure.
 */
struct proc *
fsvr_get_proc(uth)
	register uthread_t	uth;
{
	struct proc		*p;
        struct dummy_proc       *dp;
        int                     j;

	ASSERT(uth->uu_procp == 0);

	/*
	 * Get a dummy proc structure for this request.
	 */
	mutex_lock(&fsproc_lock);
	if ((dp = fsproc_free) == (struct dummy_proc *)NULL) {
                dp = (struct dummy_proc *)zalloc(dummy_proc_zone);
                if (dp == NULL)
                	panic("fsvr_get_proc(): Can't zalloc dummy_proc\n");
                nfsproc += 1;
	} else {
		fsproc_free = (struct dummy_proc *)dp->fsproc.p_nxt;
	}
	mutex_unlock(&fsproc_lock);

	bzero(dp, sizeof(*dp));		/* zero whole dummy proc */

        p = &(dp->fsproc);		/* isolate just the proc ptr */

#if MAP_UAREA
       	p->p_shared_ro = &(dp->us_ro);
       	p->p_shared_rw = &(dp->us_rw);
#endif

	p->p_ref_count = 1;
        simple_lock_init(&p->p_lock);
        queue_init(&p->p_servers);
        p->p_utask.uu_procp = p;
        simple_lock_init(&(p)->p_siglock);

        for (j = 0; j < sizeof(p->p_utask.uu_rlimit)/
			sizeof(p->p_utask.uu_rlimit[0]); j++) {
                p->p_utask.uu_rlimit[j].rlim_cur = RLIM_INFINITY;
                p->p_utask.uu_rlimit[j].rlim_max = RLIM_INFINITY;
        }

        uarea_lock_init(&p->p_utask);
        p->p_rcred = NOCRED;
        p->p_cred = MACH_PORT_NULL;
        p->p_stat = SRUN;
	p->p_flag = SDUMMY;

	/*
	 * Needed for S_intr_delivery
	 * uu_sigintr for a real proc starts as 0x4006: INT,QUIT,TERM
	 */
	p->p_utask.uu_sigintr = sigmask(SIGTERM);

	uth->uu_procp = p;		/* point current thread at the proc */
	return(p);
}

/*
 * Release a temporary proc structure.
 */
void
fsvr_release_proc(uth, p)
	register uthread_t	uth;
	register struct proc 	*p;
{
	ASSERT(uth->uu_procp == p);
	ASSERT(p->p_cred == MACH_PORT_NULL);

	--p->p_ref_count;
	ASSERT(p->p_ref_count == 0);

	/*
	 * Put the temp process back on its list.
	 */
	mutex_lock(&fsproc_lock);
	p->p_nxt = (struct proc *)fsproc_free;
	fsproc_free = (struct dummy_proc *)p;
	mutex_unlock(&fsproc_lock);

	uth->uu_procp = 0;
}

/*
 * Set up per-thread U area for the current file server thread.
 */
void
fsvr_uarea_init(uth, p)
	register struct uthread *uth;
	struct proc *p;
{
#if     MAP_UAREA
	uth->uu_share_lock_count = 0;
#endif  /* MAP_UAREA */
	uth->uu_reply_msg = 0;
	uth->uu_master_lock = 0;
#ifdef	TNC
	uth->uu_sel_id = 0;
	uth->uu_sel_flags = 0;
#endif	/* TNC */

	uth->uu_nd.ni_iov = &uth->uu_nd.ni_iovec;

	/*
	 * Storage for the thread's nameidata structure is in its
	 * utask structure.  Note that the utask structure is not
	 * shared amongst multiple threads.
	 */
	uth->uu_nd.ni_utnd = &uth->uu_procp->p_utask.uu_utnd;

	/*
	 * Refer to the proc structure's credentials, but only if there
	 * is a creds port associated with this operation.
	 */
	if (uth->uu_procp->p_cred != MACH_PORT_NULL) {
		uth->uu_nd.ni_cred = p->p_rcred;
		crhold(uth->uu_nd.ni_cred);
	} else
		uth->uu_nd.ni_cred = NOCRED;
}

/*
 * Clean out the per thread uarea after use.
 */
void
fsvr_uarea_terminate(uth)
	register struct uthread *uth;
{
	ASSERT(uth->uu_master_lock == 0);

	if (uth->uu_nd.ni_cred != NOCRED) {
		crfree(uth->uu_nd.ni_cred);
		uth->uu_nd.ni_cred = NOCRED;
	}
}


/*
 * Initialize a file server thread.
 */
int
fsvr_thread_initialize(uth, creds_port)
	register uthread_t	uth;
	mach_port_t		creds_port;
{
	struct proc		*p;
	kern_return_t		ret;

	uth->uu_master_lock = 0;	/* must init before cred_reg call */

	/*
	 * Associate the thread with a temporary proc structure.
	 */
	p = fsvr_get_proc(uth);

	/*
	 * Initialize the proc structure and then the uarea.
	 */
	ASSERT(p->p_rcred == NOCRED);
	if (creds_port != MACH_PORT_NULL) {
		ret = credentials_register(creds_port, p);
		if (ret != KERN_SUCCESS) {
			if (ret == EINTR) {
				/* cleanup and return error */
				fsvr_release_proc(uth, p);
				return(EINTR);
			}
			panic("fsvr_thread_initialize: can't register creds");
		}

		/* Point dummy proc at vproc */
		p->p_vproc = LOCATE_VPROC_PID(p->p_pid);
#ifdef	TNC
		if (p->p_vproc == NULL)
			panic("fsvr_thread_initialize");
#endif
	} else {
		p->p_vproc = NULL;
	}

	fsvr_uarea_init(uth, p);

#ifdef  SYSCALLTRACE
        /*
         * If tracing system calls, then print some info. Note that pid should
         * actually be correct, as it was set up from the credentials
         * structure.
         */
	if (syscalltrace && (syscalltrace == p->p_pid || syscalltrace < 0)) {
		char 	*s;
		char 	num[25];
		int 	syscode = uth->uu_syscode, pid = p->p_pid;
		static char *extra_syscallnames[] = {
			"(token_acquire)",      	/* 2000 */
			"(token_acquire_with_mo)", 	/* 2001 */
			"(get_window)",         	/* 2002 */
			"(token_change)",       	/* 2003 */
			"(token_not_found)",       	/* 2004 */
			"(token_release)",      	/* 2005 */
			"(file no-senders)",       	/* 2006 */
			"(token no-senders)",  		/* 2007 */
			"(vnode no-senders)",      	/* 2008 */
			"(dead-name)", 			/* 2009 */
			"(remote-print)",		/* 2010 */
			"(open-with-token)",		/* 2011 */
#ifdef	TNC
			"(tnc-relocation-op)",		/* 2012 */
			"(tnc-remote-fifo-death)",	/* 2013 */
			"(bsd_get_my_svipc_port)",	/* 2014 */
#else
			"(#2012)",
			"(#2013)",
			"(#2014)",
#endif
			"(VSOCK:socreate)",		/* 2015 */
			"(VSOCK:soclose)",		/* 2016 */
			"(VSOCK:soconnect)",		/* 2017 */
			"(VSOCK:sosetopt)",		/* 2018 */
			"(VSOCK:sogetopt)",		/* 2019 */
			"(VSOCK:select_check)",		/* 2020 */
			"(VSOCK:select_enqueue)",	/* 2021 */
			"(VSOCK:select_dequeue)",	/* 2022 */
			"(VSOCK:scrub_remote_selects)",	/* 2023 */
			"(VSOCK:sobind)",		/* 2024 */
			"(VSOCK:sosend_short)",		/* 2025 */
			"(VSOCK:sosend_long)",		/* 2026 */
			"(VSOCK:sorecv)",		/* 2027 */
			"(VSOCK:solisten)",		/* 2028 */
			"(VSOCK:sodequeue)",		/* 2029 */
			"(VSOCK:ioctl)",		/* 2030 */
			"(VSOCK:global_ioctl)",		/* 2031 */
			"(VSOCK:sogetaddr)",		/* 2032 */
			"(VSOCK:getstate_immed)",	/* 2033 */
			"(VSOCK:getstate_delay)",	/* 2034 */
			"(VSOCK:soreadable)"		/* 2035 */
		};
		static int extra_nsysent = sizeof(extra_syscallnames) /
					    sizeof(extra_syscallnames[0]);

		if (syscode >= nsysent || syscode < 0) {

			if (syscode - 2000 >= extra_nsysent || syscode < 0) {
				sprintf(num, "%d", syscode);
				s = num;
			} else {
				s = extra_syscallnames[syscode - 2000];
				if (syscode >= 2003 && syscode <= 2009) {
					/* uu_spare[0] is set in start_*_op */
					pid = uth->uu_spare[0];
				}
			}
		} else {
			s = syscallnames[syscode];
		}
		if (pid == p->p_pid) {
			SC_TRACE(("[%d]%s", pid, s));
		} else {
			/* print hex address of structure */
			SC_TRACE(("[0x%x]%s", pid, s));
		}
	}
#endif  SYSCALLTRACE

	return(KERN_SUCCESS);
}

#include "vsocket/vs_subr.h"	/*zzz*/

/*
 * Tear down a file server thread.
 */
void
fsvr_thread_terminate(uth, error)
	register uthread_t	uth;
	int			error;
{
	struct proc		*p = uth->uu_procp;

	ASSERT(uth->uu_master_lock == 0);

#ifdef	SYSCALLTRACE
	/*
	 * Do some output if we're tracing system calls.
	 */
	if (syscalltrace && (syscalltrace == p->p_pid || syscalltrace < 0)) {
		int 	pid = p->p_pid;
		int	syscode = uth->uu_syscode;
		if (syscode >= 2003 && syscode <= 2009) {
			SC_TRACE(("    [0x%x] returns %d\n", uth->uu_spare[0], error));
		} else {
			SC_TRACE(("    [%d] returns %d\n", pid, error));
		}
	}
#endif	/* SYSCALLTRACE */

	fsvr_uarea_terminate(uth);

	if (uth->uu_procp->p_cred != MACH_PORT_NULL) {
		/* Release the vproc */
		if (p->p_vproc != NULL)
			VPROC_RELEASE(p->p_vproc, "fsvr_thread_terminate");

		credentials_deregister(p, error);

		/* only needed for ASSERT in fsvr_thread_initialize */
		p->p_rcred = NOCRED;
	}
	fsvr_release_proc(uth, p);
}

/*
 * Start file operation.
 */
int
start_fileserver_op(fpp, port, creds_port, transid, syscode, serial)
	struct file 		**fpp;
	mach_port_t		port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			syscode;
	int			serial;
{
	struct uthread	*uth = &u;
	struct file 	*fp;
	int		error;

	/*
	 * Make sure that port is not bogus. Should never happen though...
	 */
	PORT_TO_FILE_LOOKUP(port, fp);
	if (fp == (struct file *)0) {
		return(EBADF);
	}
	*fpp = fp;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port)) {
		FILE_LOOKUP_DONE(fp);
		file_port_increment_seqno(fp);
		return(error);
	}
	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);
}

/* NOTE: any changes to this must also be made to end_fileserver_op_nounref */
int
end_fileserver_op(fp, error, serial)
	struct file	*fp;
	int		error;
	int		serial;
{
	struct uthread	*uth = &u;

	file_port_increment_seqno(fp);

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();

	/*
 	 * Release the reference on the file structure.
	 */
	FILE_LOOKUP_DONE(fp);

	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);

	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);
	return(error);
}

/* same as end_fileserver_op without FP_UNREF (FILE_LOOKUP_DONE) */
int
end_fileserver_op_nounref(fp, error, serial)
	struct file	*fp;
	int		error;
	int		serial;
{
	struct uthread	*uth = &u;

	file_port_increment_seqno(fp);

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();

	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);

	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);
	return(error);
}

/*
 * Start vnode operation.
 */
int
start_vnodeserver_op(start_port1, root_port, start_port2, creds_port, transid,
		     pathname, path_len, syscode, serial)
	mach_port_t		start_port1;
	mach_port_t		root_port;
	mach_port_t		start_port2;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*pathname;
	int			path_len;
	int			syscode;
	int			serial;
{
	struct uthread		*uth = &u;
	struct nameidata 	*ndp = &uth->uu_nd;
	struct vnode 		*cvp = NULLVP;
	struct vnode 		*rvp = NULLVP;
	struct vnode 		*tvp = NULLVP;
	struct mount 		*mp;
	mach_port_t		cvp_port;
	int			error = 0;
	int			nms_wakeup = 0;

	/*
	 * Translate the first start port.  It must be local.
	 * We handle normal vnode ports here, to be sure the seqno handling
	 * always happens.  We'll handle mount ports below, because they
	 * might require the U area be initialized [MOUNT_TO_VNODE might
	 * call VFS_ROOT].
	 */
	PORT_TO_VNODE_LOOKUP(start_port1, cvp);
	if (cvp != NULLVP) {
		ASSERT(cvp->v_magic == V_MAGIC);

		/*
		 * Increment the vnode's sequence number.  If a no-more-senders
		 * notification is waiting on the sequence number, wake it up.
		 */
		VN_LOCK(cvp);
		cvp->v_seqno++;
		if (cvp->v_flag & VNMSWAIT) {
			cvp->v_flag &= ~VNMSWAIT;
			nms_wakeup++;
		}
		VN_UNLOCK(cvp);
		if (nms_wakeup)
			thread_wakeup((int)&cvp->v_seqno);
		cvp_port = start_port1;
	}

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port))
		goto bad;
	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	if (cvp == NULLVP) {
		/*
		 * If the port to vnode translation fails on the request
		 * port, attempt to translate it to a mount structure.
		 */
		MOUNT_PORT_LOOKUP(start_port1, mp);
		MOUNT_TO_VNODE(mp, cvp, error);
		if (cvp == NULLVP) {
			error = EBADPORT;
			goto bad;
		}
		cvp_port = MACH_PORT_NULL;
	}

	/*
	 * Set up nameidata info needed for system call.
	 */
	ndp->ni_cdir = cvp;
	ndp->ni_cdirport = cvp_port;
	ndp->ni_cdirproxy.vpx_usecount = 1;

	ndp->ni_rdirport = root_port;
	if (root_port != MACH_PORT_NULL)
		PORT_TO_VNODE_LOOKUP(root_port, rvp);
	ndp->ni_rdir = rvp;
	ndp->ni_rdirproxy.vpx_usecount = 1;

	ndp->ni_vnodeport2 = start_port2;
	if (start_port2 != MACH_PORT_NULL)
		PORT_TO_VNODE_LOOKUP(start_port2, tvp);
	ndp->ni_vp2 = tvp;
	ndp->ni_vnode2.vpx_usecount = 1;

	ndp->ni_forwport = MACH_PORT_NULL;
	ndp->ni_allocbuf = 0;
	if (pathname) {
		ndp->ni_dirp = ndp->ni_ptr = ndp->ni_pnbuf = pathname;
		ndp->ni_pathlen = path_len;
	}

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);

bad:
	if (cvp != NULLVP)
		VNODE_LOOKUP_DONE(cvp);

	return(error);
}

int
end_vnodeserver_op(start_port1, root_port, start_port2, error, serial)
	mach_port_t		start_port1;
	mach_port_t		root_port;
	mach_port_t		start_port2;
	int			error;
	int			serial;
{
	struct uthread		*uth = &u;
	struct nameidata 	*ndp = &uth->uu_nd;

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();

	/*
	 * If namei allocated a pathname buffer, deallocate it.  Then
	 * decrement references on current directory vnode and root vnode,
	 * if local.  Also, release send right to root vnode port and to
	 * second starting vnode, if one exists.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	/* cwd vnode */
	if (ndp->ni_cdir) {
		/*
		 * The file system operation may have already released
		 * the reference on the cwd vnode.
		 */
		VNODE_LOOKUP_DONE(ndp->ni_cdir);
	}
	/* root vnode */
	if (ndp->ni_rdir) {
		VNODE_LOOKUP_DONE(ndp->ni_rdir);
	}
	/* starting vnode for 2nd path */
	if (ndp->ni_vp2) {
		VNODE_LOOKUP_DONE(ndp->ni_vp2);
	}
	if (error == ESUCCESS) {
		kern_return_t		kr;
		if (ndp->ni_rdirport != MACH_PORT_NULL) {
			kr = mach_port_deallocate(mach_task_self(),
						  ndp->ni_rdirport);
			ASSERT(kr == KERN_SUCCESS);
		}
		if (ndp->ni_vnodeport2 != MACH_PORT_NULL) {
			kr = mach_port_deallocate(mach_task_self(),
						  ndp->ni_vnodeport2);
			ASSERT(kr == KERN_SUCCESS);
		}
	}
	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);
	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);

	return(error);
}

/*
 * Start fileserver operation to support exec.
 */
int
start_execserver_op(start_port1, root_port, start_port2, creds_port, transid,
		    pathname, path_len, syscode, serial)
	mach_port_t		start_port1;
	mach_port_t		root_port;
	mach_port_t		start_port2;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*pathname;
	int			path_len;
	int			syscode;
	int			serial;
{
	struct uthread		*uth = &u;
	struct nameidata 	*ndp = &uth->uu_nd;
	struct vnode 		*cvp = NULLVP;
	struct vnode 		*rvp = NULLVP;
	struct vnode 		*tvp = NULLVP;
	struct mount		*mp;
	mach_port_t		cvp_port;
	int			error = 0;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port))
		goto bad;
	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	/*
	 * Translate the first start port.  It must be local.
	 *
	 * Doesn't increment the vnode's sequence number because the
	 * message was received on the fileserver_port and not on the
	 * vnode port.
	 */
	PORT_TO_VNODE_LOOKUP(start_port1, cvp);
	if (cvp == NULLVP) {
		/*
		 * If the port to vnode translation fails on the request
		 * port, attempt to translate it to a mount structure.
		 */
		MOUNT_PORT_LOOKUP(start_port1, mp);
		MOUNT_TO_VNODE(mp, cvp, error);
		if (cvp == NULLVP) {
			error = EBADPORT;
			goto bad1;
		}
		cvp_port = MACH_PORT_NULL;
	} else {
		ASSERT(cvp->v_magic == V_MAGIC);
		cvp_port = start_port1;
	}

	/*
	 * Set up nameidata info needed for system call.
	 */
	ndp->ni_cdir = cvp;
	ndp->ni_cdirport = cvp_port;
	ndp->ni_cdirproxy.vpx_usecount = 1;

	ndp->ni_rdirport = root_port;
	if (root_port != MACH_PORT_NULL)
		PORT_TO_VNODE_LOOKUP(root_port, rvp);
	ndp->ni_rdir = rvp;
	ndp->ni_rdirproxy.vpx_usecount = 1;

	ndp->ni_vnodeport2 = start_port2;
	if (start_port2 != MACH_PORT_NULL)
		PORT_TO_VNODE_LOOKUP(start_port2, tvp);
	ndp->ni_vp2 = tvp;
	ndp->ni_vnode2.vpx_usecount = 1;

	ndp->ni_forwport = MACH_PORT_NULL;
	ndp->ni_allocbuf = 0;
	if (pathname) {
		ndp->ni_dirp = ndp->ni_ptr = ndp->ni_pnbuf = pathname;
		ndp->ni_pathlen = path_len;
	}

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);
bad1:
	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);
	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);
bad:
	if (cvp != NULLVP)
		VNODE_LOOKUP_DONE(cvp);

	return(error);
}

int
end_execserver_op(start_port1, root_port, start_port2, error, serial)
	mach_port_t		start_port1;
	mach_port_t		root_port;
	mach_port_t		start_port2;
	register int		error;
	int			serial;
{
	struct uthread		*uth = &u;
	struct nameidata 	*ndp = &uth->uu_nd;

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();

	/*
	 * If namei allocated a pathname buffer, deallocate it.  Then
	 * decrement references on current directory vnode and root vnode,
	 * if local.  Also, release send right to root vnode port and to
	 * second starting vnode, if one exists.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	if (ndp->ni_cdir) {
		/*
		 * The file system operation may have already released
		 * the reference on the cwd vnode.
		 */
		VNODE_LOOKUP_DONE(ndp->ni_cdir);/* cwd vnode */
	}
	if (ndp->ni_rdir) {			/* root vnode */
		VNODE_LOOKUP_DONE(ndp->ni_rdir);
	}
	if (ndp->ni_vp2) {		      /* starting vnode for 2nd path */
		VNODE_LOOKUP_DONE(ndp->ni_vp2);
	}
	if (error == ESUCCESS) {
		kern_return_t		kr;
		if (root_port != MACH_PORT_NULL) {
			kr = mach_port_deallocate(mach_task_self(),
						  root_port);
			ASSERT(kr == KERN_SUCCESS);
		}
		if (start_port2) {
			kr = mach_port_deallocate(mach_task_self(),
						  start_port2);
			ASSERT(kr == KERN_SUCCESS);
		}
		/*
		 * Deallocate start_port1 (cwd_port) because the message
		 * was received on the fileserver_port.
		 */
		if (start_port1) {
			kr = mach_port_deallocate(mach_task_self(),
						  start_port1);
			ASSERT(kr == KERN_SUCCESS);
		}
	}
	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);
	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);

	return(error);
}

/*
 * Start file service operation.
 */
int
start_fsvrport_op(fsvr_port, creds_port, transid, syscode, serial)
	mach_port_t		fsvr_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			syscode;
	int			serial;
{
	struct uthread	*uth = &u;
	int		error;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */

	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port))
		return(error);

	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);
}

int
end_fsvrport_op(error, serial)
	register int	error;
	int		serial;
{
	struct uthread	*uth = &u;

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();
	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth, error);
	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);

	return(error);
}

extern	struct	proc	*proc;
/*
 * Start no-more-senders or dead-name operation.
 */
void
start_notify_op(structure, syscode)
	void		*structure;
	int		syscode;
{
	struct uthread	*uth = &u;
	struct	proc	*p;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */

	p = &proc[0];

	/*
	 * Initialize the server thread for this request.
	 * If syscall tracing, set pid to the address of the structure
	 * being garbage collected (for printf purposes only).
	 */
#ifdef	SYSCALLTRACE
	uth->uu_spare[0] = (int) structure;  /* async. op => no process */
#endif
	(void) fsvr_thread_initialize(uth, MACH_PORT_NULL);
	/* Now put in the cred structure for proc[0] */
	crhold(p->p_rcred);
	uth->u_procp->p_rcred = p->p_rcred;
}

void
end_notify_op()
{
	struct uthread	*uth = &u;
	struct	proc	*p;

	/*
	 * Tear down the server thread.
	 */
	p = &proc[0];
	uth->u_procp->p_rcred = NOCRED;
	crfree(p->p_rcred);
	fsvr_thread_terminate(&u, KERN_SUCCESS);
}

#if	MAPPED_FILES | PFS
/*
 * Start a token op.
 */
int
start_token_op(tipp, port, syscode)
	token_info_t 		**tipp;
	mach_port_t		port;
	int			syscode;
{
	struct uthread	*uth = &u;
	int	        error;

	/*
	 * Lookup the port.
	 */
	*tipp = PORT_TO_TINFO(port);
	if (*tipp == NULL) {
		return(EINVAL);
	}

	/*
	 * Get a reference to the tinfo structure while the op is in progress.
	 */
	token_port_ref(*tipp);

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 * If syscall tracing, set pid to the address of the token
	 * structure (for printf purposes only).
	 */
#ifdef	SYSCALLTRACE
	uth->uu_spare[0] = (int) *tipp;
#endif
	if (error = fsvr_thread_initialize(uth, MACH_PORT_NULL)) {
		token_port_unref(*tipp);
		return(error);
	}

	return(KERN_SUCCESS);
}

int
end_token_op(tip, error)
	token_info_t	*tip;
	int		error;
{
	/*
	 * Release reference to tinfo structure.
	 */
	token_port_unref(tip);

	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(&u, error);
	return(error);
}
#endif	/* MAPPED_FILES  | PFS */

/*
 * Start combined UX fileserver/vnodeserver call:
 * map (file) port to file and (vnode) port to vnode; set up U area.
 * We don't update the seqno on the vnode port because the message wasn't
 * sent there.
 *
 * It is *OK* if the start_port is not on this node; however, then
 * the (later) caller of namei() must manually forward the request.
 */
int
start_filevnode_op(fpp, file_port, start_port, root_port, creds_port, transid,
			pathname, path_len, syscode, serial)
	struct file		**fpp;
	mach_port_t		file_port;
	mach_port_t		start_port;
	mach_port_t		root_port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	char			*pathname;
	int			path_len;
	int			syscode;
	int			serial;
{
	struct uthread	*uth = &u;
	struct file	*fp;
	struct vnode 		*cvp = NULLVP;
	struct vnode 		*rvp = NULLVP;
	struct nameidata *ndp = &uth->uu_nd;
	int		error = 0;

	/*
	 * Make sure that file_port is not bogus.
	 */
	PORT_TO_FILE_LOOKUP(file_port, fp);
	if (fp == (struct file *)0) {
		return(EBADF);
	}
	*fpp = fp;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port)) {
		goto bad;
	}
	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	/*
	 * Set up nameidata info needed for system call.
	 */
	/*
	 * Translate the start port.
	 */
	PORT_TO_VNODE_LOOKUP(start_port, cvp);
	/*
	 * Only do something if the start port is local.  If it isn't,
	 * someone will just have to eventually forward this operation.
	 */
	ndp->ni_cdirport = start_port;
	ndp->ni_cdir = cvp;
	ndp->ni_cdirproxy.vpx_usecount = 1;

	ndp->ni_rdirport = root_port;
	if (root_port != MACH_PORT_NULL)
		PORT_TO_VNODE_LOOKUP(root_port, rvp);
	ndp->ni_rdir = rvp;
	ndp->ni_rdirproxy.vpx_usecount = 1;

	ndp->ni_vnodeport2 = MACH_PORT_NULL;
	ndp->ni_vp2 = NULLVP;
	ndp->ni_vnode2.vpx_usecount = 1;

	ndp->ni_allocbuf = 0;
	ndp->ni_forwport = MACH_PORT_NULL;
	if (pathname) {
		ndp->ni_dirp = ndp->ni_ptr = ndp->ni_pnbuf = pathname;
		ndp->ni_pathlen = path_len;
	}

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);

bad:
 	file_port_increment_seqno(fp);
	if (cvp != NULLVP)
		VNODE_LOOKUP_DONE(cvp);
	if (rvp != NULLVP)
		VNODE_LOOKUP_DONE(rvp);
        return(error);
}

int
end_filevnode_op(fp, start_port, root_port, error, serial)
	struct file	*fp;
	mach_port_t	start_port;
	mach_port_t	root_port;
	register int	error;
	int		serial;
{
	struct uthread	*uth = &u;
	struct nameidata *ndp = &uth->uu_nd;

	file_port_increment_seqno(fp);

	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();
	/*
	 * Release the reference on the file structure.
	 */
	FILE_LOOKUP_DONE(fp);
	/*
	 * If namei allocated a pathname buffer, deallocate it.  Then
	 * decrement references on current directory vnode and root vnode,
	 * if local.  Also, release send right to root vnode port.
	 */
	if (ndp->ni_allocbuf) {
		ASSERT(ndp->ni_allocbuf == 1);
		PN_DEALLOCATE(ndp->ni_pnbuf);
		ndp->ni_allocbuf = 0;
	}
	if (ndp->ni_cdir) {	/* cwd vnode */
		VNODE_LOOKUP_DONE(ndp->ni_cdir);
	}
	if (ndp->ni_rdir) {	/* root vnode */
		VNODE_LOOKUP_DONE(ndp->ni_rdir);
	}
	if (error == ESUCCESS) {
		kern_return_t		kr;
		if (start_port != MACH_PORT_NULL) {
			kr = mach_port_deallocate(mach_task_self(),
						  start_port);
			ASSERT(kr == KERN_SUCCESS);
		}
		if (root_port != MACH_PORT_NULL) {
			kr = mach_port_deallocate(mach_task_self(),
						  root_port);
			ASSERT(kr == KERN_SUCCESS);
		}
	}
	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);
	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);

	return(error);
}

/*
 * Start miscellaneious fileserver operation.
 *
 * For this call, no dummy proc is used.  This is a "transient" operation.
 */
int
start_fsvrmisc_op(port, creds_port)
	mach_port_t		port;
	mach_port_t		creds_port;
{
	struct file 	*fp;
	struct vnode 	*vp = NULLVP;
	struct mount 	*mp;
	int		nms_wakeup = 0;
	int		error;

	/*
	 * Increment sequence number as necessary.
	 * We try to figure out what type of port we have by trying each
	 * type, in turn.
	 */
	PORT_TO_FILE_LOOKUP(port, fp);
	if (fp != (struct file *)0) {
		FILE_LOOKUP_DONE(fp);
	} else {
		/*
		 * Translate the first start port.  It must be local.
		 */
		PORT_TO_VNODE_LOOKUP(port, vp);
		if (vp == NULLVP) {
			/*
			 * If the port to vnode translation fails on the
			 * request port, attempt to translate it to a
			 * mount structure.
			 */
			MOUNT_PORT_LOOKUP(port, mp);
			MOUNT_TO_VNODE(mp, vp, error);
			if (vp == NULLVP) {
				goto nothing_to_do;
			}

			VNODE_LOOKUP_DONE(vp);
		} else {
			ASSERT(vp->v_magic == V_MAGIC);

			/*
			 * Increment the vnode's sequence number.  If a
			 * no-more-senders notification is waiting on the
			 * sequence number, wake it up.
			 */
			VN_LOCK(vp);
			vp->v_seqno++;
			if (vp->v_flag & VNMSWAIT) {
				vp->v_flag &= ~VNMSWAIT;
				nms_wakeup++;
			}
			VN_UNLOCK(vp);
			if (nms_wakeup)
				thread_wakeup((int)&vp->v_seqno);

			VNODE_LOOKUP_DONE(vp);
		}

	}

nothing_to_do:

	/*
	 * If we are intr_delivery and lost a race with syscall return
	 * of an exitting process, we might see this
	 */
	if (!MACH_PORT_VALID(creds_port))
		return KERN_FAILURE;

	return KERN_SUCCESS;
}

int
end_fsvrmisc_op(port, creds_port, error)
	mach_port_t	port;
	mach_port_t	creds_port;
	int		error;
{
	struct uthread	*uth = &u;
	struct file 	*fp;

	/*
	 *  Increment sequence number for *every* file port operation
	 */
	PORT_TO_FILE_LOOKUP(port, fp);
	if (fp != (struct file *)0) {
		file_port_increment_seqno(fp);
		FILE_LOOKUP_DONE(fp);
	}

	/*
	 * Release creds port reference
	 */
	if (error == KERN_SUCCESS) {
		kern_return_t kr;
		kr = mach_port_deallocate(mach_task_self(), creds_port);
		ASSERT (kr == KERN_SUCCESS);
	}
	return(error);
}

#ifdef	TNC

/*
 * Start miscellaneious fileserver operation with dummy proc structure.
 */
int
start_fsvrmisc_op_with_proc(port, creds_port)
	mach_port_t		port;
	mach_port_t		creds_port;
{
	struct uthread *uth = &u;
	int error;

	if (error = fsvr_thread_initialize(uth, creds_port))
		return (error);

	error = start_fsvrmisc_op(port, creds_port);

	/*
	 *  Syscode 2013 is the tnc_remote_fifo_death() RPC.
	 *  In the case of a remote FIFO reader exiting, there
	 *  is a race between FIFO close() and creds port
	 *  deallocation.  If the creds port was already
	 *  deallocated by the time we got here, ignore the
	 *  error.
	 */
	if (uth->uu_syscode == 2013 &&
	    !MACH_PORT_VALID(creds_port) &&
	    error == KERN_FAILURE)
		error = KERN_SUCCESS;

	return (error);
}

int
end_fsvrmisc_op_with_proc(port, creds_port, error)
	mach_port_t	port;
	mach_port_t	creds_port;
	int		error;
{
	struct uthread	*uth = &u;

	if (port != MACH_PORT_NULL && creds_port != MACH_PORT_NULL)
		end_fsvrmisc_op(port, creds_port, error);
	fsvr_thread_terminate(uth, error);
}

/*
 * Start System V IPC operation -- much like fileserver operation.
 */
int
start_svipcserver_op(svpp, port, creds_port, transid, syscode, serial)
	struct svipc 		**svpp;
	mach_port_t		port;
	mach_port_t		creds_port;
	transaction_id_t	transid;
	int			syscode;
	int			serial;
{
	struct uthread	*uth = &u;
	struct svipc 	*svp;
	int		error;

	/*
	 * Make sure that port is not bogus.  Should never happen though.
	 */
	PORT_TO_SVIPC_LOOKUP(port, svp);
	if (svp == (struct svipc *)0) {
		return(EINVAL);
	}
	*svpp = svp;

	uth->uu_syscode = syscode;

	if (syscode < scmax && syscode >= 0)
		sccount[syscode]++;             /* count system calls */
	/*
	 * Initialize the server thread for this request.
	 */
	if (error = fsvr_thread_initialize(uth, creds_port)) {
		return(error);
	}
	++svp->sv_refcnt;		/* bump ref count */

	/*
	 * Place the saved semundo and shmsegs values in the proc
	 * structure, from our saved svipc structure.
	 */
	uth->u_semundo = svp->sv_semundo;
	uth->u_shmsegs = svp->sv_shmsegs;

	/*
	 * Register the operation in support of interruptible system calls.
	 */
	oip_register(uth, creds_port, transid);

	if (NCPUS == 1 || serial)
		unix_master();

	return(KERN_SUCCESS);
}

int
end_svipcserver_op(svp, error, serial)
	struct svipc	*svp;
	int		error;
	int		serial;
{
	struct uthread	*uth = &u;
	extern void	dealloc_svp();

	/*
	 * Replace the semundo and shmsegs values in the proc
	 * structure in our saved svipc structure.
	 */
	svp->sv_semundo = uth->u_semundo;
	svp->sv_shmsegs = uth->u_shmsegs;


	/*
	 * Get off master if it was necessary to be on it.
	 */
	if (NCPUS == 1 || serial)
		unix_release();

	/*
	 * Unregister the operation in progress.
	 */
	oip_deregister(uth);

	/*
	 * Tear down the server thread.
	 */
	fsvr_thread_terminate(uth, error);

	/*
	 * Decrement ref count and deallocate structure & port if
	 * zero.
	 */
	if (--svp->sv_refcnt <= 0 ) {
		dealloc_svp(svp);
	}

	return(error);
}

/*
 * Initialize the System V IPC (svipc) structures if we are the SVIPC
 * server.  Svipc structures are allocated and deallocated infrequently.
 * One svipc structure is created for the life of each process (system-wide)
 * the first time it uses any System V-style IPC mechanisms.  Svipc zones
 * are expandable; we start with zone size of a single page and expand
 * dynamically.  Also init the head of the svipc server's linked list of
 * svipc structures to null, and init that list's mutual exclusion lock.
 */
void
svipc_init()
{
	svipc_zone = zinit(sizeof(struct svipc), vm_page_size, 0, "svipc_zone");
	svipc_head = (struct svipc *)NULL;
	mutex_init(&svipc_lock);
}

#endif	/* TNC */

/*
 * Initialize all the temportary proc structures that are used for handling
 * requests on file-system-related ports.
 */
void
fsproc_init()
{
        /*
         * Initialize the zone for dummy proc structures.  We no longer init
	 * a bunch of them in advance, which makes servers run smaller.
         */
        dummy_proc_zone = zinit(sizeof(struct dummy_proc), 0,
			3*ux_server_receive_max*sizeof(struct dummy_proc),
			"dummy_procs_zone");

        nfsproc = 0;
        fsproc_free = NULL;
	mutex_init(&fsproc_lock);

	/*
	 * Allocate the port hash table.
	 */
	server_oip_hash_table = mpportid_hash_init(SERVER_OIP_HASHSZ);
	mount_port_hash_table = mpport_hash_init(MOUNT_PORT_HASHSZ);
}

/*
 * This function is called by mbufintr to free an mbuf data - this mbuf was made
 * by wrapping a header around an array received in a mach_msg. mclgetx() can be
 * called to make such an mbuf. mclgetx() sets the pointer to the free function,
 * the data pointer, data size and one additional argument. m_free() puts
 * such external mbufs on a 'mfreelater' queue. 'mbufintr' when freeing the
 * mbufs will call the the free function with the latter 3 arguments
 *
 * The arguments are :
 *      buf - pointer to the data received in a mach_msg and putin an mbuf
 *      size - size of 'buf'
 *      extra_arg - ignored. We define the function with this arg
 *                  since m_free will call it with 3 args.
 */
void
free_msgmbuf(buf, size, extra_arg)
        void       *buf;
        int        size;
        int        extra_arg;
{
        kern_return_t kr;

        kr = vm_deallocate(mach_task_self(),
			   (vm_address_t)buf,
			   (vm_size_t)size);
	if (kr != KERN_SUCCESS) {
                panic("free_msgmbuf: cannot deallocate 0x%x error = 0x%x",
                      buf, kr);
        }
}

/*
 * This function converts a chain of mbufs to an array of bytes that
 * can be sent across in a mach msg. It is used in the NFS server when the
 * request needs to be serviced by a node different from the one on which the
 * network message was received.
 *
 * If space can't be allocated, returns FALSE. Otherwise it returns TRUE.
 * NOTE : The caller has to take car of vm_deallocat'ing this data.
 */
boolean_t
mbuf2msg(m, dpos, data, len, doff)
	struct mbuf *m;		/* IN - pointer to an mbuf */
	caddr_t	dpos;		/* IN - an addr which could be in mbuf chain */
	char	**data;		/* OUT - pointer to an array */
	int	*len;		/* OUT - size in bytes of the array */
	int	*doff;		/* OUT - if dpos is in the mbuf chain, then the
					 offset from data; -1 otherwise */
{
	register int mlen = 0;
	register struct mbuf *n = m;
	register caddr_t dest;

	ASSERT(m);

	*doff = -1;

	/* compute size of mbuf chain */
	while (n) {
		mlen += n->m_len;
		n = n->m_next;
	}

	/* allocate the space */
	if (vm_allocate(mach_task_self(), (vm_address_t *)data,
			(vm_size_t)mlen, TRUE) != KERN_SUCCESS) {
		*data = 0;
		*len = 0;
		return FALSE;
	}

	/*
	 * Copy from mbuf chain into new space.
	 * Can't use m_copydata because of dpos/doff.
 	 */
	*len = mlen;
	dest = *data;
	while (m) {
		register char * md = mtod(m, char *);
		bcopy(md, dest, m->m_len);
		if (*doff < 0 && dpos >= md && dpos <= md + m->m_len)
			*doff = (*len-mlen) + dpos - md;
		dest += m->m_len;
		mlen -= m->m_len;
		ASSERT(mlen >= 0);
		m = m->m_next;
	}

	return TRUE;
}


/*
 * This function converts an array of bytes to a chain of mbufs
 * It is used in the NFS server when the request needs to be serviced by
 * a node different from the one on which the network message was
 * received.
 *
 * If an mbuf can't be allocated, returns with *m == (struct mbuf *)NULL.
 *
 * If we can fit the data into 1 mbuf, we will copy it into an mbuf. Otherwise,
 * we create an external mbuf where the mbuf header directly points to the
 * 'data'. This 'data' will be deallocated when the mbuf is free'd by mbufintr()
 * by calling free_msgmbuf(). 'dealloc' is set to true when the data is copied
 * into the mbuf to let the caller know that it can vm_deallocate() the data.
 * If we create and external mbuf, dealloc is set to 'false' so that the caller
 * does not vm_deallocate data - it will be vm_deallocated by free_msgmbuf
 */
void
msg2mbuf(data, dlen, doff, mhead, mposp, dealloc)
	char		*data;		/* IN - pointer to an array */
	int		dlen;		/* IN - size of marr */
	int		doff;		/* IN - offset into marr */
	register struct mbuf **mhead;	/* OUT - created mbuf chain */
	caddr_t		*mposp;		/* OUT - pointer in mbuf chain,
					         if doff is not negative */
        boolean_t       *dealloc;       /* OUT - caller can vm_dealloc data */
{
	register struct mbuf **mpp = mhead;
	extern struct mbuf *mclgetx();

	*mpp = NULL;

	ASSERT(dlen >= 0);

        if (dlen <= MHLEN) {
                /*
                 * If we can fit it into 1 mbuf, it is more efficient
                 * to copy the data into a regular mbuf than to use an
                 * external mbuf. For the data to be freed from an
                 * external mbuf, the netisr thread has to be scheduled,
                 * and a context switch is a loss for small data
                 */
                MGETHDR(*mpp, M_WAIT, MT_DATA);

                (*mpp)->m_len = dlen;

                (*mpp)->m_pkthdr.len = dlen;
                (*mpp)->m_pkthdr.rcvif = (struct ifnet *)0;

                /*
                 * Copy this chunk of data
                 */
                bcopy(data, mtod(*mpp, caddr_t), dlen);
                if (doff >= 0) {
                        if (doff < dlen) {
                                *mposp = mtod(*mpp, caddr_t) + doff;
                        }
                }
                *dealloc = TRUE;
        } else {
                /* make it an external mbuf with the mbuf pointing
                 * directly to the data that came in the msg. The data
                 * area will be deallocated by mbufintr calling
                 * free_msgmbuf.
                 */

                *mpp = mclgetx(free_msgmbuf, data, data, dlen, M_WAIT);
                if (*mpp == NULL) {
                        panic("msg2mbuf : out of mbufs\n");
                }

                (*mpp)->m_pkthdr.len = dlen;
                (*mpp)->m_pkthdr.rcvif = (struct ifnet *)0;

                if (doff >= 0) {
                        if (doff < dlen) {
                                *mposp = (caddr_t)data + doff;
                        }
                }
                *dealloc = FALSE;
        }

}
