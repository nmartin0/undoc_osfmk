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
 * Copyright (c) 1989 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * Copyright (c) 1992-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * This file was modified and extended by the Center for High Performance 
 * Computing (CHPC) on behalf of OSF.
 */
/*
 * HISTORY
 * $Log: ux_server_loop.c,v $
 * Revision 1.28  1995/05/04  21:45:10  stans
 *  Disable thread-reduction code as it doesn't work in ALL cases.
 *
 *  Reviewer:jlitvin
 *  Risk:low
 *  Benefit or PTS #:8993
 *  Testing: developer
 *
 * Revision 1.27  1995/04/07  17:31:22  bolsen
 * Author of fix: Brent Olsen
 * Reviewer(s): Jerry Toman, John Litvin
 * Risk: Medium (one file, several lines of critical code)
 * Benefit or PTS #: 12577
 * Module(s): server/uxkern/ux_server_loop.c
 * Configurations built: STD+WS, LITE+WS, STD+WS+test+ldebug, RAMDISK+WS
 * Testing: Purdue is running the fixed server on their Beta test system
 * 	 where the original bug was happening several times a day.
 *
 * We found a work around for a probable Mach kernel problem.  Since the
 * server is using a port set to receive requests it is believed from the
 * Mach documentation that we shouldn't receive a MACH_RCV_PORT_DIED error
 * when suppling a port set.  The work around in the server is to ignore
 * this error when we receive it since there is nothing the server can do.
 * We do print a message to the boot node so that there is a trace that
 * the problem occured.
 *
 * Revision 1.26  1995/03/29  22:15:46  stans
 *  Boost the server death-pill port queue limit. Numerous ux server threads
 *  become qlimit blocked while trying to send a death-pill message to the server.
 *  When receiving a death-pill, die immediately.
 *
 *  Reviewer: cfleck
 *  Risk: medium
 *  Benefit or PTS #: 12855
 *  Testing:
 * 	munops
 * 	WW11 sats
 * 	'pccm2' would not run without this patch. With it, twice.
 *
 * Revision 1.25  1995/03/28  22:27:26  stans
 *  ux server loop thrad reduction support.
 *  Default is UX_OLD_THREADS==0; (enabled).
 *  Aggressively recycle ux server threads back into the Cthread library for
 *  hot-standby and dispatch.
 *
 *  Reviewer: jlitvin
 *  Risk: high
 *  Benefit or PTS #: 8993
 *  Testing:
 *         WW10 sats: watson, a5, si160 & testjig
 *         EATS on a5
 *         munops on a5
 *         Developer tests:
 *                 pathkiller XPS150(1024 nodes), watson, a5, si160 and testjig
 *                 PFS i/o perf tests: a5, testjig, XPS150.
 *                   PFS read & write performance sees a slight increase with
 *                   reduced threads.
 *
 * Revision 1.24  1995/02/24  23:34:49  stans
 *  Remove debug code and debug counters from main server loop
 *
 *  Reviewer:suri,andyp
 *  Risk:low
 *  Benefit or PTS #:12425
 *  Testing:WW07 sats
 *
 * Revision 1.23  1995/02/01  22:31:19  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.22  1994/12/06  20:58:57  yazz
 *  Reviewer: Johnb Litvin
 *  Risk: Lo
 *  Benefit or PTS #: 11363
 *  Testing: controlc EAT
 *  Module(s): server/uxkern/ux_exception.c, server/uxkern/ux_server_loop.c
 * Make sure that the server's exception thread is wired.
 *
 * Revision 1.21  1994/11/18  20:49:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.20  1994/10/25  22:44:11  yazz
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
 * When ux_server_thread_unblocking is temporarily releasing the master lock,
 * add two asserts.  One makes sure the master lock hold counter is a sane
 * value, and the other makes sure that the calling thread that's about to
 * release the master lock is in fact the same thread that has it held.
 *
 * Revision 1.19  1994/06/18  00:33:33  jlitvin
 * Remove embedded comment characters to make lint happier.
 *
 * Revision 1.18  1994/05/04  19:49:54  yazz
 * Merge R1.2 fix 1.14.2.2 into main stem.
 *
 * Revision 1.17  1994/04/28  19:18:11  chrisp
 * Changes for introduction of tncgen and support of i386 builds.
 *
 *  Reviewer: dleslie, cfj
 *  Risk: M
 *  Benefit or PTS #: 9188
 *  Testing: Builds and tested on i386 platform.
 *  Module(s): server_init.c syscall_subr.c ux_server_loop.c
 *
 * Revision 1.16  1994/03/29  23:52:03  cfj
 * Merge revision 1.14.2.1 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.15  1994/01/12  17:45:51  jlitvin
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
 * Revision 1.14.2.1  1994/03/29  23:44:03  cfj
 * Make all server threads scheduling policy POLICY_FIXEDPRI.
 *
 *  Reviewer:jlitvin
 *  Risk: Low
 *  Benefit or PTS #:7923
 *  Testing:pccm2
 *  Module(s):server/uxkern/ux_server_loop.c
 * 	   server/kern/threadcall.c
 *
 * Revision 1.14  1993/11/09  19:16:10  cfj
 * Check if a deadnames notiifcation comes in on the allocator port and
 * call the appropriate function to handle it.
 *
 *  Reviewer:nandy
 *  Risk:M
 *  Benefit or PTS #:Allow users to kill and restart the allocator
 *  Testing:
 *  Module(s):server/nx/nx.c server/uxkern/ux_server_loop.c
 *
 * Revision 1.13  1993/08/03  17:48:27  robboy
 * Bumped UX_SERVER_RECEIVE_EXCESS from 2 back up to 14 for Lite server, to
 * avoid problem with wait()
 *
 * Revision 1.12  1993/07/19  23:00:02  robboy
 * Integrate OSF/Locus Lite server changes
 *
 * Revision 1.11  1993/07/14  18:45:24  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:07:06  cfj
 * Adding new code from vendor
 *
 * Revision 2.29  93/06/24  13:27:53  mjl
 * Add Locus copyright notice (again).
 * 
 * Revision 2.28  93/06/02  12:40:35  yazz
 * For Sys V IPC under TNC add the generic svipc server routine to the
 * list of those called to process messages from the emulator.
 * 
 * Revision 2.27  93/04/24  19:49:48  klh
 *
 * Revision 1.10  1993/05/10  16:46:49  cfj
 * Fix up merge problems.
 *
 * Revision 1.9  1993/05/06  19:34:20  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:54:08  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.7  1993/03/10  17:02:30  nandy
 * Initialize oip lock in server threads.
 *
 * Revision 1.6  1993/02/16  19:18:51  cfj
 * Remove debug error message in mach_msg_destroy_port().
 *
 * Revision 1.5  1993/02/16  19:15:06  nandy
 * ux_server_thread_blocking() now unlocks the ux_server_count_lock
 * before creating a new mach_thread.
 *
 * Revision 1.1.2.1.2.2  1992/12/16  06:05:57  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  15:46:20  cfj
 * Changed type of forward declaration of ux_create_thread() to cthread_t.
 *
 * Revision 1.1.2.1.2.1  1992/11/25  23:16:45  brad
 * Added first cut at PFS file striping capability.
 *
 * Revision 1.3  1992/12/11  03:06:57  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:56:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:45:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.27  1992/10/24  20:42:58  cfj
 * Reset the size of msg_buffer_1.space in ux_server_loop() back to 8192.
 *
 * Revision 2.26  1992/10/23  02:00:50  cfj
 * T6 merge.
 *
 * Revision 2.25  1992/10/15  17:48:54  cfj
 * Added the NX server to ux_server_loop().
 *
 * Revision 2.25  92/11/23  16:03:10  klh
 * 	Revision 2.22  92/11/11  17:25:49  condict
 * 		Arrange that ux_thread_wire is automatically performed for all
 * 		threads created with ux_thread_bootstrap, except ux_server_loop
 * 		threads.  The ux_thread_wire/unwire calls are now no-ops.
 * 		[92/11/11            condict]
 * 
 * Revision 2.27  1993/04/24  19:49:48  klh
 * 	Revision 2.28  93/04/08  11:44:10  loverso
 * 		Add ux_server_thread_counts and ux_server_thread_suspend
 * 		functions.
 * 		[1992/06/12  08:07:18  condict]
 *
 * 		Added uu_wired flag for wired threads detection.
 * 		[1992/06/04  15:03:10  jose]
 *
 * 		In uniproc configuration, protect thread count data with master lock,
 * 		instead of special mutex, for efficiency.
 * 		[1992/06/04  14:23:07  condict]
 *
 * 		Initially, create all server threads wired.  ux_server_loop will
 * 		then unwire itself.  This fixes the bug where the OSF1_main thread
 * 		was never being unwired when it became a ux_server_loop thread.
 * 		This revisits much of the changes in 2.22.  In particular,
 * 		ux_thread_wire() exists again, but it is static to this file.
 * 		(loverso)
 *
 * 		Cthreads name themselves by &u, not cthread_self().
 * 		Initialize the cthread stack size to de-couple it from the value
 * 		given by a first server.
 * 		(condict)
 *
 * 	Revision 2.27  93/03/30  16:09:21  roy
 * 		Remove fast_path_io conditional.
 * 		[93/03/30            roy]
 *
 * 	Revision 2.26  93/03/05  17:47:02  loverso
 * 		Initialize oip lock in server threads.  Correct previous change.
 * 		(loverso)
 *
 * 	Revision 2.25  93/02/25  12:02:26  loverso
 * 		Insert missing mutex_unlock().
 *
 * 	Revision 2.24  93/02/11  15:47:36  durriya
 * 		don't print error message if mach_port_deallocate returns error
 * 		in mach_msg_destroy_port - the error is entirely possible and ok
 * 		as the emulator may have gone away.                  - (durriya)
 *
 * 	Revision 2.23  93/01/07  11:15:29  condict
 * 		Got rid of per-thread profiling stuff.
 * 		[1992/06/15  15:23:52  emcmanus]
 *
 * Revision 2.26  93/02/24  15:49:46  slively
 * Add missing mutex_unlock in ux_server_loop.
 * 
 * Revision 2.25  92/11/23  16:03:10  klh
 * 	Revision 2.22  92/11/11  17:25:49  condict
 * 		Arrange that ux_thread_wire is automatically performed for all
 * 		threads created with ux_thread_bootstrap, except ux_server_loop
 * 		threads.  The ux_thread_wire/unwire calls are now no-ops.
 * 		[92/11/11            condict]
 * 
 * Revision 2.24  92/08/06  13:41:37  klh
 * 	Revision 2.20  92/07/28  20:00:37  rabii
 * 		Revision 3.21  92/04/08  20:46:50  barbou
 * 		Fix for (OSF/1 MK) bug #105: activate the notification server.
 * 
 * Revision 2.23  92/07/17  11:25:41  roman
 * Print out message id in decimal, because that is the way they are
 * 	recorded in .defs files.
 * 
 * Revision 2.22  92/07/07  15:19:46  roman
 * Fix format to match OSF source exactly.
 * 
 * Revision 2.21  92/06/16  18:42:28  bhk
 * conditionaly include the if_mi driver correctly
 * 
 * Revision 2.20  92/06/11  16:31:17  mjl
 * Added server routine for out-of-line socket relocation RPC.
 * 
 * Revision 2.19  92/06/10  10:12:06  klh
 * 	Revision 2.18  92/06/08  18:30:55  pjg
 * 		Call fsvr2_server dispatch routine under FAST_PATH_IO 
 *		conditional (pjg)
 * 
 * Revision 2.18  92/06/05  14:01:11  klh
 * 	Revision 2.17  92/05/24  14:00:58  pjg
 * 		Revision 3.19  92/03/31  15:41:39  emcmanus
 * 		Rename ux server threads so that the cthread_self value is 
 *		visible in the name as truncated by gdb.
 * 
 * 		Revision 3.18  92/03/23  18:05:00  condict
 * 		Release and retake master lock in 
 *		ux_server_thread_blocking/unblocking.
 * 
 * 		Revision 3.17  92/03/10  16:06:35  condict
 * 		Keep track of number of blocked ux_server threads and modify
 * 		ux_set_max_kernel_threads to take them into account.  Also, use
 *		loop of creating server threads after calling 
 *		cthread_set_kernel_limit, to ensure
 * 		that the new Mach kernel threads are actually created.
 * 
 * 	Revision 2.16  92/05/18  12:28:03  roy
 * 		Revision 2.13.1.1  92/04/22  10:05:47  roy
 * 		Print return codes in hex when panicing.
 * 		[92/04/18            roy]
 * 
 * 	Revision 2.15  92/05/12  00:04:02  loverso
 * 		fsvr_nosenders_notify() becomes more generic fsvr_notify(). 
 *		(loverso)
 * 
 * Revision 2.17  92/04/28  09:31:02  chrisp
 * Add message id to ux_server_loop panic message and print the kernel return
 * 	code in hex for greater convenience.
 * 
 * Revision 2.16  92/04/21  10:39:17  klh
 * Ifdef out call to mi_server if MI is not defined.
 * 
 * Revision 2.15  92/04/20  17:28:56  bhk
 * Added virtual sockets and mi driver server routines
 * 
 * Revision 2.14  92/04/01  16:25:28  roman
 * Add check for service for the the tnc_async.defs files (TNC only).
 * 
 * Revision 2.13  92/03/15  14:34:04  roy
 * 	Put back deallocation of reply_port if return code is 
 * 	MIG_NO_REPLY. Change copyright (pjg).
 * 	Add server_mach_msg(). Change some ifdef's to UX_THREAD_DEBUG (srl).
 * 
 * Revision 2.12  92/03/09  13:56:27  durriya
 * 	Revision 3.16  92/02/28  18:00:15  barbou
 * 	Don't complain about the invalid message that a second server will 
 * 	send to try to get the privileged ports: just ignore it as before.
 * 
 * 	Revision 3.15  92/02/25  17:48:51  condict
 * 	Extensive changes to implement partitioning of the ports serviced by
 * 	ux_server_loop threads into multiple port sets on a multi-processor, to
 * 	prevent the port-set locking bottleneck in the Mach kernel.
 * 	Also, added ux_server_thread_blocking/unblocking functions to allow 
 * 	server threads to do blocking Mach services without running out of Mach 
 * 	kernel threads.
 * 
 * 	Revision 3.14  92/02/18  18:57:17  jose
 * 	Added one more thread for async_io (not used yet).
 * 
 * Revision 2.11  92/03/01  18:34:44  pjg
 * 	92/02/28  pjg
 * 	Changed ux_server_loop which is now similar to mach_msg_server
 * 	(from libmach). ux_server_loop now destroys the message if the
 * 	routine returns an error. This means that server routines must
 * 	deallocate ports and messages in case of success, but cannot
 * 	deallocate then in case of error.
 * 	Temporarily add a version of mach_msg_destroy_port and
 * 	mach_msg_destroy_memory to print a message in case of error.
 * 
 * 	92/02/28  rabii
 * 	Added credentials master and cache servers.
 * 
 * Revision 2.10  92/02/11  21:56:15  pjg
 * 	Print out return code during call to panic() (roman@locus).
 * 
 * Revision 2.9  92/01/14  11:29:52  roy
 * 	92/01/10  22:09:56  noemi
 * 	Removed call to defunct fsvr_vnode_generic_server.
 * 
 * Revision 2.8  92/01/05  20:12:50  roy
 * 	91/12/18  21:13:05  noemi
 * 	Added call to fsvr_vnode_generic_server. Renamed ux_fs_generic_server
 * 	to fsvr_generic_fs_server.  Added fsvr_generic_server call.
 * 	Changed ux_fs_nosenders_notify to fsvr_nosenders_notify.
 * 	Added a call to fsvr_server in ux_server_loop.
 * 
 * Revision 2.7  91/12/17  10:19:04  roy
 * 	91/12/09  18:49:57  emcmanus
 * 	Create the mcount_thread when profiling the server.
 * 	Newly created threads need to be initialised for profiling in that 
 * 	case, and removed from the list of threads when they exit.
 * 
 * 	91/11/15  15:31:59  condict
 * 	Keep track of number of server threads that have been invoked but 
 * 	aren't yet receiving first message, to avoid gratuitous burst of 
 * 	thread creations.
 * 
 * 	91/11/07  15:15:33  condict
 * 	Set wired_threads at runtime, initialized by the value of the 
 * 	WIRED_THREADS option, and turned on by the -w flag to the server.
 * 
 * 	91/10/31  12:27:31  david
 * 	remove server_loop termination printf
 * 
 * 	91/10/28  11:10:19  condict
 * 	Increase ux_server_receive_max from 12 to 16.
 * 
 * 	91/10/17  18:34:25  barbou
 * 	Make the reserved uthread be the first local variable to allow easier 
 * 	compatibility of the offsets between ux_thread_bootstrap and 
 * 	call_thread.
 * 
 * 	91/10/17  15:45:41  condict
 * 	Substantial changes to: (1) Allow compilation with all threads wired, 
 * 	for ease of debugging (WIRED_THREADS option); (2) Eliminate all uses of
 * 	cthread_mach_msg, cthread_msg_busy/active.  Instead, maintain a simple
 * 	low and high water mark of server threads that are receiving messages.
 * 	Deleted ux_server_thread_busy/active as well.
 * 
 * Revision 2.6  91/11/22  15:20:43  rabii
 * 	locus merge
 * 	in ux_server_loop() add call to invoke TNC server (klh)
 * 
 * Revision 2.5  91/10/14  13:24:56  sjs
 * 	91/10/01  14:13:34  condict
 * 	Add DEBUG option which wires all server threads for ease of debugging.
 * 
 * 	91/09/27  12:03:43  emcmanus
 * 	Added profiling support.
 * 
 * 	91/09/18  17:48:16  condict
 * 	Change definition of u_offset to agree with new def. of u in sys/user.h.
 * 
 * Revision 2.4  91/10/04  15:31:12  chrisp
 * Add Locus copyright notice.
 * 
 * Revision 2.3  91/09/16  16:47:44  sjs
 * integrated Locus changes 	roman
 * Add a call to ux_fs_generic_server() and ux_vp_generic_server()
 * for handling generic file system calls (i.e. calls where the
 * destination port is a file or a vproc rather than a proc port).
 * Added call to a routine to handle no_more_sender messages.
 * 
 * Revision 2.2  91/08/31  14:29:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/07/05  16:24:30  jose
 * Added one more thread
 * 
 * Revision 3.2  91/06/25  17:15:22  condict
 * Increase max_kernel_threads to 24 to accomodate the 4 new networking
 * threads and 5 new vnode pager threads.
 * 
 * Revision 3.1  91/06/07  09:50:52  condict
 * Fix deadlock: increase number of Mach kernel threads to 20.
 * 
 * Revision 3.0  91/01/17  12:06:28  condict
 * Unchanged copy from Mach 3.0 BSD UNIX server
 * 
 * Revision 2.8  90/10/25  15:07:46  rwd
 * 	Add STACK_GROWTH_UP case for u_offset.
 * 	[90/10/11            rwd]
 * 
 * Revision 2.7  90/08/06  15:35:31  rwd
 * 	Change to reflect change in cthread_msg interface.
 * 	[90/06/06            rwd]
 * 
 * Revision 2.6  90/06/19  23:16:20  rpd
 * 	Modified the server loop to allow simpleroutines.
 * 	[90/06/09            rpd]
 * 
 * Revision 2.5  90/06/02  15:29:01  rpd
 * 	If a receive or rpc fails unexpectedly, panic.
 * 	[90/05/12            rpd]
 * 	Converted to new IPC.
 * 	Fixed bug checking return code in reply message.
 * 	Deallocate reply port if it isn't used.
 * 	[90/03/26  20:28:17  rpd]
 * 
 * Revision 2.4  90/01/19  14:38:21  rwd
 * 	Change to use cthread_msg_rpc which will do an rpc when
 * 	possible.
 * 	[89/12/20            rwd]
 * 	Allocate uarea on stack.
 * 	[89/12/14            rwd]
 * 
 * Revision 2.3  89/12/08  20:17:34  rwd
 * 	Changed to use new cthread_receive call.
 * 	[89/10/23            rwd]
 * 
 * Revision 2.2  89/10/17  11:27:52  rwd
 * 	Undo rfr's single-server-loop changes.
 * 	[89/09/21            dbg]
 * 
 * Revision 2.1  89/08/04  14:50:39  rwd
 * Created.
 * 
 *  6-Apr-89  David Golub (dbg) at Carnegie-Mellon University
 *	Split into different files.
 *
 *  6-Jan-89  David Golub (dbg) at Carnegie-Mellon University
 *	Created.
 *
 * $EndLog$
 */
/*
 * Server thread utilities and main loop.
 */

/* #define UX_LOOP_DEBUG 1 */
/* #define UX_THREAD_DEBUG */

#include <fullserver.h>
#include <wired_threads.h>
#include <uxkern/import_mach.h>

#include <sys/param.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/synch.h>
#include <kern/parallel.h>

vm_size_t cthread_stack_size = 64 * 1024;

/* We partition the ports that are serviced by the ux_server_loop threads into
 * multiple port sets to avoid a bottleneck on the port-set lock inside the
 * Mach kernel.
 *
 * However, the overhead of maintaining too many port sets implies that we
 * should probably limit it to some small number.  How about 8?
 */
#define MAX_PORT_SETS	8

typedef struct ux_port_set_info {
	mach_port_t	name;		 /* Port name of this port set.    */
	mach_port_t	port_in_set;	 /* One of the ports in this set.  */
	int		receive_count;	 /* Num threads receiving on it.   */
	int		threads_initing; /* Number of threads initiated    *
					  * for it but not yet receiving.  */
	int		num_msgs;	 /* Num msgs received (decays).	   */
} *ux_port_set_info_t;

struct ux_port_set_info ux_server_port_set[MAX_PORT_SETS];
int		   ux_num_port_sets;
ux_port_set_info_t ux_server_end_port_sets;

#include "profiling.h"

#if PROFILING
mach_port_t ux_profil_port_set = MACH_PORT_NULL;
#endif
int	u_offset = 0;

#if   NCPUS > 1

struct mutex	ux_server_count_lock = MUTEX_INITIALIZER;
#define UX_SERVER_COUNT_LOCK()		mutex_lock(&ux_server_count_lock)
#define UX_SERVER_COUNT_UNLOCK()	mutex_unlock(&ux_server_count_lock)

#else NCPUS > 1

#define UX_SERVER_COUNT_LOCK()
#define UX_SERVER_COUNT_UNLOCK()

#endif        NCPUS > 1


/* ----- BEGIN vars protected by the ux_server_count_lock mutex { ----- */

	/* The following two tuneable parameters control the number of
	 * ux_server_loop threads that are simultaneously receiving
	 * messages at the top of the server loop:
	 */
#define	UX_SERVER_RECEIVE_MIN_PER_SET  2  /* Min num ux_server_loop     *
					   * threads receiving on each  *
					   * port set.                  */

#define UX_SERVER_RECEIVE_HIGH_WATER	4 /* Threads above min_per_set	*/

#define UX_SERVER_RECEIVE_EXCESS      14  /* Total number of extra	*
					   * threads (beyond min per	*
					   * set) allowed to be rcving	*
					   * on all port sets		*/

	/* The following are calculated and maintained by the server
	 * and are not tuneable:
	 */

int	ux_server_receive_min;		  /* Min total threads rcving	*
					   * on all port sets		*/

int	ux_server_high_water = 0;	  /* Delta beyond receive_min	*/
int	old_threads = 1;		  /* 1: use OLD style thread	*
					   * allocation.		*/

int	ux_server_receive_max_per_set;	  /* Max num ux_server_loop	*
					   * threads receiving on each	*
					   * port set.			*/

int	ux_server_receive_max;		  /* Max total threads rcving	*
					   * on all port sets		*/

int	ux_server_receive_count = 0;	  /* Current total threads	*
					   * rcving on all port sets	*/

#ifdef UX_LOOP_DEBUG
int	ux_server_loop_count = 0;	  /* Num threads in server loop */
#endif

int	ux_server_blocked_threads = 0;	  /* Num server loop threads	*
					   * in blocking Mach services	*/

#ifdef UX_THREAD_DEBUG
int	ux_server_suspended_threads = 0;  /* Num server loop threads    *
					   * in thread_block.           */
#endif

int	ux_num_cpus;			  /* Num CPUs on this host.	*/

int	ux_num_wired_threads;   	  /* Num wired threads		*
					   * throughout the server.	*/

int	ux_max_kernel_threads;		  /* Max number of Mach kernel	*
					   * threads that C-threads can	*
					   * use.  Must be enough for	*
					   * all wired threads with	*
					   * some left over to be	*
					   * shared by unwired threads.	*/

/* ----- } END vars protected by the ux_server_count_lock mutex ----- */

/*
 * ux_server_receive_max and receive_max_per_set have to be kept in sync:
 */
#define SET_receive_max(newval) do {				  \
	ux_server_receive_max = (newval);			  \
	ux_server_receive_max_per_set = ux_server_receive_max /   \
						ux_num_port_sets; \
} while (0)

any_t		ux_server_loop();		/* forward */
void	 	ux_set_max_kernel_threads();
void		ux_thread_wire(), ux_thread_unwire();

/*
 * Should all server C-threads be wired to a Mach kernel thread?  Can be
 * set at run-time using the -w flag:
 */
boolean_t	wired_threads = WIRED_THREADS;

#if PROFILING
mach_port_t     server_prof_port;

extern  void eat_buffers();
extern	void pport_to_proc_init();

cthread_t ux_create_thread();	/* forward */

void
ux_profil_add_port(port)
	mach_port_t	port;
{
	(void) mach_port_move_member(mach_task_self(),
				     port, ux_profil_port_set);
}


#ifndef	UX_PROFIL_REMOVE_PORT
#define	UX_PROFIL_REMOVE_PORT

void	ux_server_remove_port(); /* forward */
#define	ux_profil_remove_port(port)  ux_server_remove_port(port)

#endif	UX_PROFIL_REMOVE_PORT
#endif	/* PROFILING */

static boolean_t	ux_server_initialized = FALSE;
int                     ux_min_quantum = 0;

void
ux_server_init()
{
	kern_return_t		kr;
	int			host_buff[HOST_BASIC_INFO_COUNT];
	mach_msg_type_number_t	host_buff_size = HOST_BASIC_INFO_COUNT;
	host_t			mach_host_self();
	int			i;

	ASSERT(!ux_server_initialized);
	ux_server_initialized = TRUE;

	/*
	 * Calculate number of CPUs on this host: Idea being that we balance
	 * the servers ports between the set(s).
	 */
	if ((kr = host_info(mach_host_self(), HOST_BASIC_INFO,
				host_buff, &host_buff_size)) != KERN_SUCCESS) {
		printf(
		     "Warning: Can't get host_info for num cpus (Err = %d).\n",
			kr);
		printf("         Using num cpus = 1.\n");
		ux_num_cpus = 1;
	} else {
		ux_num_cpus =
		      ((struct host_basic_info *)host_buff)->avail_cpus;
#if MACH_ASSERT
		printf("Server scheduling set for %d cpus.  NCPUS=%d\n",
				ux_num_cpus, NCPUS);
#endif
		if (ux_num_cpus < 1 || ux_num_cpus > 256) {
			printf("Warning: host_info says num cpus == %d",
				ux_num_cpus);
			if (ux_num_cpus < 1)
				ux_num_cpus = 1;
			else
				ux_num_cpus = 256;
			printf(".  Using %d\n", ux_num_cpus);
		}
	}

	{
	kern_return_t		kr;
	int			host_sched[HOST_SCHED_INFO_COUNT];
	mach_msg_type_number_t	host_sched_size = HOST_SCHED_INFO_COUNT;
	extern host_t		mach_host_self();
	

	kr = host_info(mach_host_self(), HOST_SCHED_INFO,
		       host_sched, &host_sched_size);
	if (kr == KERN_SUCCESS) {
	    ux_min_quantum = ((host_sched_info_t)host_sched)->min_quantum;
	    kr = thread_policy(mach_thread_self(), 
			       POLICY_FIXEDPRI, 
			       ux_min_quantum);
	    switch (kr) {
		case KERN_SUCCESS:
		    break;
	        case KERN_FAILURE:
		    printf("Fixed priority scheduling policy denied.\n");
		    ux_min_quantum = 0;
		    break;
		default:
		    printf("thread_policy() failed: %d\n", kr);
		    ux_min_quantum = 0;
	    }
	}
        }

	/* Initialize the array of port sets for receiving client requests:
	 */
	ux_num_port_sets = ux_num_cpus;
	if (ux_num_port_sets > MAX_PORT_SETS)
		ux_num_port_sets = MAX_PORT_SETS;
	ux_server_end_port_sets = &ux_server_port_set[ux_num_port_sets];

	for (i = 0; i < ux_num_port_sets; i++) {
		ux_server_port_set[i].num_msgs = 0;
		ux_server_port_set[i].receive_count = 0;
		ux_server_port_set[i].threads_initing = 0;
		ux_server_port_set[i].name = MACH_PORT_NULL;
		kr = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_PORT_SET,
					&ux_server_port_set[i].name);

		/* Put a port in each set, for use by kill_one_thread: */
		ux_server_port_set[i].port_in_set = MACH_PORT_NULL;
		kr = mach_port_allocate(mach_task_self(),
					MACH_PORT_RIGHT_RECEIVE,
					&ux_server_port_set[i].port_in_set);

                /* boost 'death-pill' port queue limit */
                kr = mach_port_set_qlimit(mach_task_self(),
                                          ux_server_port_set[i].port_in_set,
					  128);

		kr = mach_port_insert_right(mach_task_self(),
					    ux_server_port_set[i].port_in_set,
					    ux_server_port_set[i].port_in_set,
					    MACH_MSG_TYPE_MAKE_SEND);
		kr = mach_port_move_member(mach_task_self(),
					   ux_server_port_set[i].port_in_set,
					   ux_server_port_set[i].name);
	}

	/* Following is the only use of the "EXCESS" variable, after which
	 * receive_max can float up and down because of use of the
	 * ux_server_thread_blocking/unblocking functions.
	 *
	 * NOTE: The thread_not_receiving macro is much more efficient if
	 * receive_max is an even multiple of num_port_sets, otherwise threads
	 * may spend a lot of time migrating between port sets, so round the
	 * specified EXCESS up:
	 */
	{ int excess = ((UX_SERVER_RECEIVE_EXCESS + ux_num_port_sets - 1)
					/ ux_num_port_sets) * ux_num_port_sets;
	  SET_receive_max((ux_num_port_sets * UX_SERVER_RECEIVE_MIN_PER_SET) +
									excess);
	}
	/* Following is just used externally by mach_init.c: */
	ux_server_receive_min = ux_num_port_sets *
					UX_SERVER_RECEIVE_MIN_PER_SET;
	ux_server_high_water = ux_server_receive_min +
					UX_SERVER_RECEIVE_HIGH_WATER;
#if PROFILING
	/* 
	 *	Allocate the internal profiling port set. 
	 */
	kr = mach_port_allocate(mach_task_self(), MACH_PORT_RIGHT_PORT_SET,
				&ux_profil_port_set);

	/*
	 *	Initialize the hash table for port-proc translation.
	 */
	pport_to_proc_init();

	/* 
	 *	Launch the guardian thread charged with the reception of
	 *	the buffers send by the micro-kernel
	 */
        ux_create_thread(eat_buffers);
#endif	/* PROFILING */

	/* Set the max kernel threads to be used by C-threads and cause
	 * their creation right now:
	 */
	ux_set_max_kernel_threads();
}

/*
 * Add a port to one of the port-sets being serviced by the server loop.  We
 * use load balancing here, assigning the port to the port-set that has the
 * lowest recent usage, based on a decaying count of messages received.
 * Returns the index of the chosen port set.
 */
void
ux_server_add_port(port)
	mach_port_t	port;
{
	ux_port_set_info_t ps,
			   best_port_set = &ux_server_port_set[0];
	int		   least_msgs =    best_port_set->num_msgs;

	/* Now compare the first one with the rest: */
	if (ux_num_port_sets > 1) {
		/* Decay the usage counts by 25%: */
		best_port_set->num_msgs = best_port_set->num_msgs * 3 / 4;
		for (ps = &ux_server_port_set[1];
					ps < ux_server_end_port_sets; ps++) {
			if (ps->num_msgs < least_msgs) {
				least_msgs = ps->num_msgs;
				best_port_set = ps;
			}
			/* Decay the usage counts by 25%: */
			ps->num_msgs = ps->num_msgs * 3 / 4;
		}
	}
#ifdef UX_LOOP_DEBUG
	printf("DEBUG: Adding port %d to portset %d (usage count = %d).\n",
				port, best_port_set - ux_server_port_set,
				least_msgs);
#endif
	(void) mach_port_move_member(mach_task_self(), port,
				     best_port_set->name);
}

void
ux_server_remove_port(port)
	mach_port_t	port;
{
	(void) mach_port_move_member(mach_task_self(), port, MACH_PORT_NULL);
}

any_t
ux_thread_bootstrap(arg)
	any_t	arg;
{
	struct uthread	uthread;

	register int	routine_addr = (int)arg;
	cthread_fn_t	real_routine = (cthread_fn_t)routine_addr;
	register int calc;

	bzero((char *)&uthread, sizeof(struct uthread));
	calc = (int)&uthread - (int)&(ur_cthread_self());
	if (u_offset) {
		if (u_offset != calc)
			panic("ux_thread_bootstrap: different u_offsets");
	} else {
		u_offset = calc;
	}

	mutex_init(&uthread.uu_lock);
	condition_init(&uthread.uu_condition);

	oip_init(&uthread);

	/* all threads initially start out wired; u.uu_wired cleared by
	 * previous bzero().
	 */
	ux_thread_wire();

	if (ux_min_quantum != 0) {
		kern_return_t		kr;
		
		kr = thread_policy(mach_thread_self(), 
				   POLICY_FIXEDPRI, 
				   ux_min_quantum);
		if (kr != KERN_SUCCESS) 
			printf("thread_policy() failed: %d\n", kr);
        }

	return ((*real_routine)((any_t)0));
}


/*
 * Create a thread with a UX u-area.
 */
cthread_t
ux_create_thread(routine)
	cthread_fn_t	routine;
{
	int		routine_addr = (int)routine;
	cthread_t	new_thread;;

	new_thread = cthread_fork(ux_thread_bootstrap, (any_t)routine_addr);
	cthread_detach(new_thread);
	return new_thread;
}

void
ux_create_server_thread()
{
	ux_create_thread(ux_server_loop);
}

/*
 * Used at initialization, after wiring a thread, and when an unwired
 * (ux_server_loop) thread is about to do a blocking Mach service.  Ensures
 * that there are enough Mach kernel threads to avoid blocking the whole task
 * due to no kernel thread being available to execute a C thread:
 */
void
ux_set_max_kernel_threads()
{
	/* This should be exported by C-threads but is not: */
	extern int cthread_kernel_threads;

	if (wired_threads) {
		/* Allow as many Mach kernel threads as C-threads: */
		static int first_time = 1;
		if (first_time) {
			first_time = 0;
			UX_SERVER_COUNT_LOCK();
			ux_max_kernel_threads = 0;	/* No limit */
			cthread_set_kernel_limit(ux_max_kernel_threads);
			UX_SERVER_COUNT_UNLOCK();
		}
	} else {
		int needed_kernel_threads;

		UX_SERVER_COUNT_LOCK();

		/* Must be > num_wired_threads + blocked_threads +
		 * receive_max, to avoid blocking.  Optimally, should
		 * be greater by n, on an n-CPU multi-processor, to
		 * allow one runnable C-thread on each processor:
		 */
		needed_kernel_threads = ux_num_wired_threads +
					ux_server_blocked_threads +
					ux_server_receive_max + ux_num_cpus;

		if (ux_max_kernel_threads < needed_kernel_threads) {
			ux_max_kernel_threads = needed_kernel_threads;
#ifdef UX_THREAD_DEBUG
			printf("%s %d (wired=%d, rcvmax=%d, blocked=%d)\n",
				"DEBUG: Increasing max kernel threads to",
				ux_max_kernel_threads, ux_num_wired_threads,
				ux_server_receive_max,
				ux_server_blocked_threads);
#endif
			cthread_set_kernel_limit(ux_max_kernel_threads);

			/* Arrange that all the Mach threads actually get
			 * created (only cthread_fork creates a Mach thread):
			 */
			while (cthread_kernel_threads < ux_max_kernel_threads)
				ux_create_server_thread();
		}
		UX_SERVER_COUNT_UNLOCK();
	}
}

/* Keeps track of number of wired threads.  No other function is allowed to
 * call cthread_(un)wire, or deadlock may result due to the server loop not
 * knowing how many wired threads exist.
 */
void
ux_thread_wire()
{
	ASSERT(ux_server_initialized);

	if (u.uu_wired == FALSE) {
		UX_SERVER_COUNT_LOCK();
		++ux_num_wired_threads;

#ifdef UX_THREAD_DEBUG
	printf("DEBUG: ++ Num wired threads == %d\n", ux_num_wired_threads);
#endif
		UX_SERVER_COUNT_UNLOCK();
		ux_set_max_kernel_threads();
		cthread_wire();
		u.uu_wired = TRUE;
	}
}

static void
ux_thread_unwire()
{
	ASSERT(ux_server_initialized);

	if (u.uu_wired == TRUE) {
		UX_SERVER_COUNT_LOCK();
		--ux_num_wired_threads;

#ifdef UX_THREAD_DEBUG
	printf("DEBUG: -- Num wired threads == %d\n", ux_num_wired_threads);
#endif
		UX_SERVER_COUNT_UNLOCK();

		cthread_unwire();
		u.uu_wired = FALSE;
	}
}


/*
 * Wake up a receiving thread on the port set with the most threads, using a
 * no-op message, so it will commit suicide:
 */
void
ux_server_kill_one_thread()
{
	ux_port_set_info_t	ps,
				best_port_set = &ux_server_port_set[0];
	int			max_threads =   best_port_set->receive_count;

	/* Now compare the first one with the rest: */
	if (ux_num_port_sets > 1) {
		for (ps = &ux_server_port_set[1];
					ps < ux_server_end_port_sets ; ps++) {
			if (ps->receive_count > max_threads) {
				best_port_set = ps;
				max_threads = ps->receive_count;
			}
		}
	}

#ifdef UX_THREAD_DEBUG
	printf("DEBUG: -- sending no-op message to port %d of set %d.\n",
		best_port_set->port_in_set, best_port_set - ux_server_port_set);
#endif
	ubsd_no_op_msg(best_port_set->port_in_set);
}


/*
 * Implementation of the kill_one_thread() 'ubsd_no_op_msg'  server-server RPC
 * invoked above.
 *
 * We have swallowed a death-pill, die now!
 */
void
sbsd_no_op_msg(proc_port)
	mach_port_t proc_port;
{
#ifdef UX_THREAD_DEBUG
	printf("DEBUG: -- no-op message received.\n");
#endif

	/* Just to keep the count of wired threads accurate: */
	if (wired_threads)
		ux_thread_unwire();

#ifdef UX_LOOP_DEBUG
	UX_SERVER_COUNT_LOCK();
	--ux_server_loop_count;
	UX_SERVER_COUNT_UNLOCK();
#endif
	cthread_exit(KERN_SUCCESS);
}

/*
 * Should be called whenever a ux_server_loop thread is about to do a blocking
 * Mach service, such as a message_receive or a device_read.  Since this will
 * make the Mach kernel thread unavailable for the duration of the Mach
 * service, we need to temporarily lower the max allowed number of threads that
 * can block in message_receive at the top of the server loop.  Also, we
 * need to wake one of them up, if we currently are at the max:
 */
void
ux_server_thread_blocking()
{
	if (!wired_threads && !u.uu_wired) {

		UX_SERVER_COUNT_LOCK();

		++ux_server_blocked_threads;

		/* Don't let receive_max get too close to (num_port_sets *
		 * receive_min), or bad performance will result due to
		 * frequent creation and suicide of server threads:
		 */
		if (ux_server_receive_max > ux_server_receive_min + 4) {
			SET_receive_max(ux_server_receive_max - 1);
			if (ux_server_receive_count > ux_server_receive_max) {
				UX_SERVER_COUNT_UNLOCK();
				ux_server_kill_one_thread();
			} else
				UX_SERVER_COUNT_UNLOCK();
		} else {
			UX_SERVER_COUNT_UNLOCK();

			/* Cannot reduce max receiving threads, so we need
			 * a new Mach thread, unfortunately:
			 */
			ux_set_max_kernel_threads();
		}
	}

	if (u.uu_master_lock) {
		ASSERT(master_mutex_aux.slthread == (char *)&u.uu_master_lock);
		ASSERT((unsigned)u.uu_master_lock < 7);
		/* In case this thread is using locking spl's (see synch.h): */
		if (u.uu_ipl > 0)
			/* Shouldn't be blocking at raised spl: */
			printf("WARNING: server thread %s (%d)\n",
			        "blocking at raised spl", u.uu_ipl);
		master_unlock();
	}
}


/*
 * Should be called immediately after a ux_server_loop thread completes a
 * blocking Mach service (reverses the effect of ux_server_thread_blocking;
 * see above).
 */
void
ux_server_thread_unblocking()
{
	if (u.uu_master_lock)
		master_lock();

	if (!wired_threads && !u.uu_wired) {

		UX_SERVER_COUNT_LOCK();

		SET_receive_max(ux_server_receive_max + 1);
		--ux_server_blocked_threads;

		UX_SERVER_COUNT_UNLOCK();
	}
}

/*
 * Called by thread_block to put a server loop thread to sleep until it
 * is awakened by some future event.  The function is here because it needs
 * to deal with server-thread count issues and may in the future need to
 * do such things as initiate another receiving thread to take the place of
 * the one that is sleeping, etc.
 */
void
ux_server_thread_suspend(uth, lock)
	struct uthread	*uth;
	struct mutex	*lock;
{
#ifdef UX_THREAD_DEBUG
	if (!uth->uu_wired) {
		UX_SERVER_COUNT_LOCK();
		++ux_server_suspended_threads;
		UX_SERVER_COUNT_UNLOCK();
	}
#endif

	condition_wait(&uth->uu_condition, lock);

#ifdef UX_THREAD_DEBUG
	if (!uth->uu_wired) {
		UX_SERVER_COUNT_LOCK();
		if (--ux_server_suspended_threads < 0)
			panic("ux_server_suspended_threads < 0");
		UX_SERVER_COUNT_UNLOCK();
	}
#endif
}

#ifdef UX_THREAD_DEBUG
/*
 * Returns (through its pointer arguments) the counts indicating what the
 * server loop threads are doing.
 *
 * Invariant:	ux_server_receive_count +
 *		ux_server_blocked_threads +
 *		ux_server_suspended_threads <= ux_server_loop_count
 */
void
ux_server_thread_counts(server_th, receive_th, mach_blocked_th, suspended_th)
	int *server_th, *receive_th, *mach_blocked_th, *suspended_th;
{
		UX_SERVER_COUNT_LOCK();
		*server_th = ux_server_loop_count;
		*receive_th = ux_server_receive_count;
		*mach_blocked_th = ux_server_blocked_threads;
		*suspended_th = ux_server_suspended_threads;
		UX_SERVER_COUNT_UNLOCK();
}
#endif /* UX_THREAD_DEBUG */

/*
 * Internal version of mach_msg that keeps number of ux threads correct.
 */
mach_msg_return_t
server_mach_msg(msg, option, send_size, rcv_size, rcv_name, timeout, notify)
    mach_msg_header_t *msg;
    mach_msg_option_t option;
    mach_msg_size_t send_size;
    mach_msg_size_t rcv_size;
    mach_port_t rcv_name;
    mach_msg_timeout_t timeout;
    mach_port_t notify;
{
	mach_msg_return_t ret;

	ux_server_thread_blocking();
	ret = mach_msg(msg,option,send_size,rcv_size,rcv_name,timeout,notify);
	ux_server_thread_unblocking();

	return(ret);
}

/*
 * Find and return index of one of the port sets that has the least number of
 * server threads, preferably one for which a new thread has already
 * been initiated (indicated by threads_initing > 0).  NOTE: must be called
 * with the ux_server_count_lock taken.
 */
#define ux_least_threads_port_set()	  \
	(ux_num_port_sets == 1 ?	  \
		&ux_server_port_set[0]	  \
	:				  \
		_least_threads_port_set() \
	)

ux_port_set_info_t
_least_threads_port_set()
{
	ux_port_set_info_t	ps,
				best_port_set = &ux_server_port_set[0];
	int			least_threads =
					best_port_set->receive_count;

	/* Now compare the first one with the rest: */
	for (ps = &ux_server_port_set[1]; ps < ux_server_end_port_sets ; ps++) {
		if (ps->receive_count <= least_threads &&
					(ps->receive_count < least_threads ||
						    ps->threads_initing > 0)) {
			best_port_set =  ps;
			least_threads = ps->receive_count;
		}
	}
#ifdef UX_LOOP_DEBUG
	printf("DEBUG receiving from port_set %d, least threads = %d\n",
			best_port_set - ux_server_port_set, least_threads);
#endif /* UX_LOOP_DEBUG */
	return best_port_set;
}


/* Called when we are about to receive a syscall message.  We want
 * to detect if there are already the maximum number of receivers,
 * so the thread can exit instead of blocking a Mach kernel
 * thread with a receive operation.
 */
#define	thread_receiving(port_set_p, too_many_receivers)    do {	\
	UX_SERVER_COUNT_LOCK();						\
	if (++ux_server_receive_count > ux_server_receive_max) {	\
		--ux_server_receive_count;				\
		too_many_receivers = 1;					\
	}								\
	else if ( old_threads == 0 && ux_server_blocked_threads == 0 && \
		 port_set_p->receive_count > ux_server_high_water) {	\
			--ux_server_receive_count;			\
			too_many_receivers = 1;				\
	}								\
	else {								\
		if (++(port_set_p->receive_count) >			\
				 ux_server_receive_max_per_set) {	\
			/* Switch to another port set: */		\
			--(port_set_p->receive_count);			\
			port_set_p = ux_least_threads_port_set();	\
			++(port_set_p->receive_count);			\
			if (port_set_p->threads_initing > 0)		\
				--(port_set_p->threads_initing);	\
		}							\
		too_many_receivers = 0;					\
	}								\
	UX_SERVER_COUNT_UNLOCK();					\
} while (0)

/* Called after receiving a syscall message.  If there are less than the
 * minimum threads receiving on this port set, we start up another one, so
 * that system calls don't get left unanswered.  However, we need to
 * count as well the number of threads that have been recently invoked	
 * and have not yet reached their first message_receive (threads_initing),
 * to avoid starting up more new threads than desired before any of them
 * get to their first msg receive:
 */
#define	thread_not_receiving(port_set_p)    do {		    \
	UX_SERVER_COUNT_LOCK();					    \
	++port_set_p->num_msgs;					    \
	--ux_server_receive_count;				    \
	if ( --(port_set_p->receive_count) +			    \
		port_set_p->threads_initing <			    \
				   UX_SERVER_RECEIVE_MIN_PER_SET) { \
		++port_set_p->threads_initing;			    \
		UX_SERVER_COUNT_UNLOCK();			    \
		ux_create_server_thread();			    \
	} else {						    \
		UX_SERVER_COUNT_UNLOCK();			    \
	}							    \
} while (0)

/*
 * Main loop of UX server.
 */
/*ARGSUSED*/
any_t
ux_server_loop(arg)
	any_t	arg;
{
	register kern_return_t	ret;
	register int		too_many_receivers = 0;
	ux_port_set_info_t	my_port_set;

	union request_msg {
	    mach_msg_header_t	hdr;
	    mig_reply_header_t	death_pill;
	    char		space[8192];
	} msg_buffer_1, msg_buffer_2;

	mach_msg_header_t * request_ptr;
	mig_reply_header_t * reply_ptr;
	mach_msg_header_t * tmp;

	char	name[64];

	extern mach_port_t	ux_notify_port;
#ifdef NX
	extern mach_port_t	nx_notify_port;
#endif /* NX */

	if (!wired_threads)
		ux_thread_unwire();

	sprintf(name, "ux %x server thread", &u);
	cthread_set_name(cthread_self(), name);

	request_ptr = &msg_buffer_1.hdr;
	reply_ptr = &msg_buffer_2.death_pill;

	UX_SERVER_COUNT_LOCK();

		/* Assign this thread to one of the port sets that has the
		 * least number of server threads assigned to it:
		 */
		my_port_set = ux_least_threads_port_set();

		/* Indicate that we are done initializing.  Threads_initing
		 * only gets incremented if a new server thread is created
		 * by the thread_not_receiving macro, hence it might go
		 * negative if we unconditionally decrement it (because
		 * server_loop threads are created elsewhere than within
		 * the macro):
		 */
		if (my_port_set->threads_initing > 0)
			--(my_port_set->threads_initing);
#ifdef UX_LOOP_DEBUG
		++ux_server_loop_count;
#endif

	UX_SERVER_COUNT_UNLOCK();

#ifdef UX_LOOP_DEBUG
        printf("BEGIN %s (count = %d, rcv = %d, portset = %d)\n",
		name, ux_server_loop_count,
		ux_server_receive_count, my_port_set - ux_server_port_set);
#endif /* UX_LOOP_DEBUG */

	do {
	    thread_receiving(my_port_set, too_many_receivers);
	    if (too_many_receivers) break;

	    ret = mach_msg(request_ptr, MACH_RCV_MSG,
				   0, sizeof msg_buffer_1,
				   my_port_set->name,
				   MACH_MSG_TIMEOUT_NONE, MACH_PORT_NULL);

	    thread_not_receiving(my_port_set);

	    if (ret != MACH_MSG_SUCCESS) {
 		if (ret == MACH_RCV_PORT_DIED) {
 			mach_port_type_t	my_port_set_type;
 
 			/* the "rcv_name" value is no longer valid */
 			ret = mach_port_type(mach_task_self(),
 						my_port_set->name,
 						&my_port_set_type);
 			if (ret == KERN_SUCCESS) {
 				bootnode_printf("ux_server_loop receive: "
						"my_port_set->name=0x%x, "
 						"my_port_set_type=0x%x, "
						"ret=MACH_RCV_PORT_DIED\n",
						my_port_set->name,
 						my_port_set_type);
				if (my_port_set_type & MACH_PORT_TYPE_PORT_SET)
					continue;
 			}
 			panic("ux_server_loop receive: "
				"my_port_set->name(0x%x), "
				"ret=MACH_RCV_PORT_DIED (0x%x), "
				"mpt ret=0x%x",
				my_port_set->name,
 				MACH_RCV_PORT_DIED,
				ret);
		} else
			panic("ux_server_loop receive: ret=0x%x", ret);
	    }

	    while (ret == MACH_MSG_SUCCESS) {
		if (fsvr_server(request_ptr, &reply_ptr->Head)){}
		else if (bsd_1_server(request_ptr, &reply_ptr->Head)){}
		else if (fsvr2_server(request_ptr, &reply_ptr->Head)){}
#ifdef	PFS
		else if (pfs2_server(request_ptr, &reply_ptr->Head)){}
#endif	PFS
		else if (fsvr_fs_generic_server(request_ptr,&reply_ptr->Head)){}
		else if (ux_generic_server(request_ptr, &reply_ptr->Head)){}
		else if (ux_vproc_generic_server(request_ptr, &reply_ptr->Head)){}
#ifdef		TNC
		else if (tnc_server(request_ptr, &reply_ptr->Head)){}
		else if (tnc_async_server(request_ptr, &reply_ptr->Head)){}
		else if (nsrv_server(request_ptr, &reply_ptr->Head)) {}
		else if (tnc_ool_server(request_ptr, &reply_ptr->Head)) {}
		else if (svipc_generic_server(request_ptr, &reply_ptr->Head)) {}
#include "mi.h"
#if	NMI > 0
		else if (mi_server(request_ptr, &reply_ptr->Head)) {}
#endif
#endif		/* TNC */
		else if (exc_server(request_ptr, &reply_ptr->Head)){}
		else if (bsd_2_server(request_ptr, &reply_ptr->Head)){}
#ifdef NX
		else if (request_ptr->msgh_local_port == nx_notify_port &&
			 nx_notify_server(request_ptr, &reply_ptr->Head)){}
#endif /* NX */
		else if (request_ptr->msgh_local_port == ux_notify_port &&
			 ux_notify_server(request_ptr, &reply_ptr->Head)){}
		else if (fsvr_notify(request_ptr, &reply_ptr->Head)){}
		else if (fsvr_generic_server(request_ptr, &reply_ptr->Head)){}
		else if (cred_server(request_ptr, &reply_ptr->Head)){}
		else if (credcache_server(request_ptr, &reply_ptr->Head)){}
#ifdef NX
                else if (nx_svr_server(request_ptr, &reply_ptr->Head)) {}
#endif /* NX */
		else if (request_ptr->msgh_id == 999999) {
			/*
			 * The above test looks for the kind of message that 
			 * a server sends to its bootstrap port to get
			 * the priviledged ports. It can come from a second
			 * server, so ignore it, and let this server try
			 * another way of getting its ports...
			 * (see get_config_info() in bsd/mach_init.c)
			 */
		}
		else {
			printf("ux_server_loop: Warning -- Invalid message (id = %d)\n", request_ptr->msgh_id);
		}

		/* For now we have to avoid this, because it can happen in
		 * sbsd_issig_psig:
		 */
#if 0
		/* if uu_procp != 0, then the thread didn't cleanup properly */
		ASSERT(u.uu_procp == 0);  
#else
		u.uu_procp = 0;
#endif

		if (reply_ptr->RetCode != KERN_SUCCESS) {
			if (reply_ptr->RetCode == MIG_NO_REPLY) {
				/* deallocate reply port right */
				(void) mach_port_deallocate(mach_task_self(),
					     reply_ptr->Head.msgh_remote_port);

				break;
			} else if (reply_ptr->RetCode == MIG_REMOTE_ERROR) {
				/* destroy the message and don't send a reply*/
				mach_msg_destroy(request_ptr);
				break;
			} else {
				/* don't destroy the reply port,
				   so we can reply with an error message */
				request_ptr->msgh_remote_port = MACH_PORT_NULL;
				mach_msg_destroy(request_ptr);
			}
		}

		if (reply_ptr->Head.msgh_remote_port == MACH_PORT_NULL) {
			/* no reply port, destroy the reply */
			if (reply_ptr->Head.msgh_bits & MACH_MSGH_BITS_COMPLEX)
				mach_msg_destroy(&reply_ptr->Head);
			break;
		}

		tmp = request_ptr;
		request_ptr = (mach_msg_header_t *) reply_ptr;
		reply_ptr = (mig_reply_header_t *) tmp;

		thread_receiving(my_port_set, too_many_receivers);

		if (too_many_receivers) {
			/* Only send reply, don't receive: */
			ret = mach_msg(request_ptr,
				       MACH_SEND_MSG,
				       request_ptr->msgh_size,
				       0,
				       MACH_PORT_NULL,
				       MACH_MSG_TIMEOUT_NONE,
				       MACH_PORT_NULL);
			/*
			 * break on thru to the other side...JM.
			 * If 'too many receivers' here, then the condition will
			 * be the same in the outer loop test. Eventual
			 * outcome is to exit the function. 
			 */
			break;
		} else {
			/* Send reply to prev request and receive another: */
			ret = mach_msg(request_ptr,
				       MACH_SEND_MSG|MACH_RCV_MSG,
				       request_ptr->msgh_size,
				       sizeof msg_buffer_2,
				       my_port_set->name,
				       MACH_MSG_TIMEOUT_NONE,
				       MACH_PORT_NULL);
			thread_not_receiving(my_port_set);
		}
	    } /* while (ret == MACH_MSG_SUCCESS) */

	    if (ret != KERN_SUCCESS) {
		if (ret == MACH_SEND_INVALID_DEST) {
			/* the reply can't be delivered, so destroy it */
			mach_msg_destroy(request_ptr);
 		} else if (ret == MACH_RCV_PORT_DIED) {
 			mach_port_type_t	my_port_set_type;
 
 			/* the "rcv_name" value is no longer valid */
 			ret = mach_port_type(mach_task_self(),
 						my_port_set->name,
 						&my_port_set_type);
 			if (ret == KERN_SUCCESS) {
 				bootnode_printf("ux_server_loop rpc: "
						"my_port_set->name=0x%x, "
 						"my_port_set_type=0x%x, "
						"ret=MACH_RCV_PORT_DIED\n",
						my_port_set->name,
 						my_port_set_type);
				if (my_port_set_type & MACH_PORT_TYPE_PORT_SET)
					continue;
 			}
 			panic("ux_server_loop rpc: my_port_set->name(0x%x), "
				"ret=MACH_RCV_PORT_DIED (0x%x), "
				"mpt ret=0x%x",
				my_port_set->name,
 				MACH_RCV_PORT_DIED,
				ret);
		} else
			panic("ux_server_loop rpc: id=%d, ret=0x%x",
				request_ptr->msgh_id, ret);
	    }

	} while (1);

	/* Just to keep the count of wired threads accurate: */
	if (wired_threads)
		ux_thread_unwire();

#ifdef UX_LOOP_DEBUG
	UX_SERVER_COUNT_LOCK();
	--ux_server_loop_count;
	UX_SERVER_COUNT_UNLOCK();

        printf("END %s (threads remaining = %d)\n", name,
							ux_server_loop_count);
#endif /* UX_LOOP_DEBUG */

	return ((any_t)0);	/* exit */
}

#undef NOTDEF
#ifdef NOTDEF
/*
 * Temporary duplication of mach_msg_destroy from
 * libmach to print error messages in case of error
 */
static void mach_msg_destroy_port();
static void mach_msg_destroy_memory();

/*
 *	Routine:	mach_msg_destroy
 *	Purpose:
 *		Deallocates all port rights and out-of-line memory
 *		found in a received message.
 */

int
mach_msg_destroy(msg)
    mach_msg_header_t *msg;
{
    mach_msg_bits_t mbits = msg->msgh_bits;

    /*
     *	The msgh_local_port field doesn't hold a port right.
     *	The receive operation consumes the destination port right.
     */

    mach_msg_destroy_port(msg->msgh_remote_port, MACH_MSGH_BITS_REMOTE(mbits));

    if (mbits & MACH_MSGH_BITS_COMPLEX) {
	vm_offset_t saddr;
	vm_offset_t eaddr;

	saddr = (vm_offset_t) (msg + 1);
	eaddr = (vm_offset_t) msg + msg->msgh_size;

	while (saddr < eaddr) {
	    mach_msg_type_long_t *type;
	    mach_msg_type_name_t name;
	    mach_msg_type_size_t size;
	    mach_msg_type_number_t number;
	    boolean_t is_inline;
	    vm_size_t length;
	    vm_offset_t addr;

	    type = (mach_msg_type_long_t *) saddr;
	    is_inline = type->msgtl_header.msgt_inline;
	    if (type->msgtl_header.msgt_longform) {
		    name = type->msgtl_name;
		    size = type->msgtl_size;
		    number = type->msgtl_number;
		    saddr += sizeof(mach_msg_type_long_t);
	    } else {
		    name = type->msgtl_header.msgt_name;
		    size = type->msgtl_header.msgt_size;
		    number = type->msgtl_header.msgt_number;
		    saddr += sizeof(mach_msg_type_t);
	    }

	    /* calculate length of data in bytes, rounding up */
	    length = ((((number * size) + 7) >> 3) + 3) &~ 3;

	    addr = is_inline ? saddr : * (vm_offset_t *) saddr;

	    if (MACH_MSG_TYPE_PORT_ANY(name)) {
		mach_port_t *ports = (mach_port_t *) addr;
		mach_msg_type_number_t i;

		for (i = 0; i < number; i++)
		    mach_msg_destroy_port(*ports++, name);
	    }

	    if (is_inline) {
		/* inline data sizes round up to int boundaries */
		saddr += length;
	    } else {
		mach_msg_destroy_memory(addr, length);
		saddr += sizeof(vm_offset_t);
	    }
	}
    }
}

static void
mach_msg_destroy_port(port, type)
    mach_port_t port;
    mach_msg_type_name_t type;
{
	int ret;

    if (MACH_PORT_VALID(port)) switch (type) {
      case MACH_MSG_TYPE_PORT_SEND:
      case MACH_MSG_TYPE_PORT_SEND_ONCE:
	ret = mach_port_deallocate(mach_task_self(), port);
	break;

      case MACH_MSG_TYPE_PORT_RECEIVE:
	ret = mach_port_mod_refs(mach_task_self(), port,
				  MACH_PORT_RIGHT_RECEIVE, -1);
	if (ret) 
	  printf("mach_msg_destroy_port:mach_port_mod_refs(%x) returns %x\n",
		 port, ret);
	break;
    }
}

static void
mach_msg_destroy_memory(addr, size)
    vm_offset_t addr;
    vm_size_t size;
{
	int ret;

    if (size > 0) {
	ret = vm_deallocate(mach_task_self(), addr, size);
	if (ret) 
	  printf("mach_msg_destroy_memory:vm_deallocate(%x,%d) returns %x\n",
		 addr, size, ret);
    }
}
#endif NOTDEF
