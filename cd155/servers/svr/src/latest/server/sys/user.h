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
 * Copyright (c) 1991-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * Copyright (C) 1988,1989 Encore Computer Corporation.
 * All Rights Reserved.
 *
 * Property of Encore Computer Corporation.
 * This software is made available solely pursuant to the terms of
 * a software license agreement which governs its use. Unauthorized
 * duplication, distribution or sale are strictly prohibited.
 *
 */
/*
 * This file was extended and modified by the Center for High Performance 
 * Computing of Worcester Polytechnic Institute on behalf of OSF.
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
 * HISTORY
 * $Log: user.h,v $
 * Revision 1.21  1995/02/01  21:38:44  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.20  1994/12/20  22:58:14  suri
 *  Reviewer: jlitvin
 *  Risk: Low
 *  Benefit or PTS #: 11640
 *  Testing: Specific Testcase
 *  Module(s): Backing out the fix for PTS-11270/11317/10593 (vm_allocate/sbrk
 *  changes), as it was causing excessive wiring down of memory on the compute
 *  nodes under some specific cases (the testcase for PTS-11640 is one of them).
 *  The vm_allocate/sbrk changes have to be reimplemented, perhaps using
 *  vm_reserve().
 *
 * Revision 1.19  1994/11/18  20:42:09  mtm
 * Copyright additions/changes
 *
 * Revision 1.18  1994/10/25  23:54:17  suri
 *  Reviewer: jlitvin
 *  Risk: M
 *  Benefit or PTS #: 11317
 *  Testing: Specific testcase, fileio, pthreads, xtrnl, NQS/MACs EATs
 *  Module(s): obreak() in server/bsd/kern_mman.c
 *            setrlimit() in server/bsd/kern_resource.c
 *            coff_getxfile() in server/bsd/kern_exec.c
 *            user struct in server/sys/user.h
 *            rf_data struct in server/tnc/rtask.h
 *            rfork_pproc_load_msg() in server/tnc/rtask_cli_pproc.c
 *            rfork_pproc_unload_msg() in server/tnc/rtask_svr_pproc.c
 *
 * Revision 1.17  1994/10/25  23:26:49  yazz
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
 * Side-thread changes to both user.h and proc.h.  Define new side ref count
 * field.  Rename p_issig_count to p_issig_flag.  Rename p_sigref to p_exit_-
 * hold_count.  Define a new proc flags bit, SBLOCKEMUL, used when a signal
 * kills a process.  For assertful plus lock debugging servers (test+ldebug)
 * only, define a per-thread area for the call chain of the last locker and
 * unlocker of the master lock.  Made some 32-bit boolean_t's into chars to
 * save space.  Improved comments, column alignment.
 *
 * Revision 1.16  1994/09/13  15:43:39  johannes
 * defined new field 'uu_faulting_thread' in utask structure
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 10486
 *  Testing: developer tests, corefile EAT
 *  Module(s): svr/server/sys/user.h
 * 	    svr/server/bsd/kern_fork.c
 *             svr/server/bsd/mach_signal.c
 *             svr/server/paracore/core.c
 *
 * Revision 1.15  1994/07/27  16:15:51  johannes
 * added two fields in the utask containing the file name of the
 * execed program and the proxy vnodes of root and current directory at the
 * moment of the exec
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
 * Revision 1.14  1994/07/27  16:11:14  johannes
 *
 * Revision 1.13  1994/06/29  16:18:30  johannes
 * four new fields in utask to get information from 'proc_abort' to
 * core dumping of non-faulting processes
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
 * Revision 1.12  1994/05/04  21:43:34  mjl
 * TNC select rewrite.  New implicit vsoo_select() args in uthread struct
 * are unique integer id and flag word (see vsocket/vs_subr.h).
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
 * Revision 1.11  1993/07/14  18:30:35  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:39:43  cfj
 * Adding new code from vendor
 *
 * Revision 1.10  1993/05/07  19:18:01  nandy
 * Fixed a merge conflict.
 *
 * Revision 1.9  1993/05/06  19:20:59  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.8  1993/04/03  03:08:03  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.7  1993/03/19  03:18:14  cfj
 * Merge from T9.
 *
 * Revision 1.6  1993/03/11  16:17:05  cfj
 * Merged in final version of the new forkmulti() from Locus but kept the 
 * workaround.
 *
 * Revision 1.5.6.1  1993/03/19  01:25:29  cfj
 * Fix to new locking code from OSF.
 *
 * Revision 2.24  93/03/01  11:28:25  roman
 * [SPE #18] For a faster rforkmulti(), provide an option for procdup() to
 *         use a previously created task rather than always creating
 *         a task.
 * 
 * Revision 1.1.2.2.2.2  1993/01/09  00:05:08  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:01:19  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 * 
 * Revision 1.5  1992/12/14  15:56:22  cfj
 * Added some #ifdef so file can be used by commands and libs.
 *
 * Revision 1.4  1992/12/11  03:00:12  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:45:28  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:29:58  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:43:02  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.23  1992/10/16  20:44:16  andrews
 * Added IPD debug support.
 *
 * Revision 2.23  92/10/29  13:17:52  chrisp
 * Fix up RCS log entries to avoid line wraps.
 * 
 * Revision 2.28  93/06/16  15:27:27  klh
 * 	Revision 2.30  93/05/27  10:40:56  bolinger
 * 		Remove extraneous log entry (sigh).
 * 
 * 	Revision 2.29  93/05/27  10:32:33  bolinger
 * 		Fix up last log entry.
 * 
 * 	Revision 2.28  93/05/27  10:28:14  bolinger
 * 		Shrink utask (and hence proc) struct by allocating vm_mmap structs
 * 		dynamically.  Here, update members of utask and vm_mmap structs
 * 		appropriately.
 * 
 * 	Revision 2.27  93/05/16  21:03:22  loverso
 * 		Replace "struct user" with version from 1.2 which uses pads to
 * 		assure binary compatibility to user mode processes.  "struct user"
 * 		is only used in core dumps and with table(TBL_UAREA). (loverso)
 * 
 * Revision 2.27  93/06/02  09:56:44  yazz
 * For Sys V IPC under TNC define new structure member field "uu_svipc_flag"
 * and corresponding UTASK macro.
 * 
 * Revision 2.26  93/04/29  14:02:01  klh
 * 	Revision 2.26  93/04/08  11:33:21  loverso
 * 		Added uu_wired field to uthread structure.
 * 		[1992/06/04  14:52:13  jose]
 *
 * 	Revision 2.25  93/03/22  23:58:46  condict
 * 		Added uu_state to uthread structure, for correct implementation of
 * 		assert_wait(0,...).
 *
 * 	Revision 2.24  93/01/07  11:17:13  condict
 * 		Removed the per-thread profiling stuff.
 * 		[1992/06/10  17:53:24  emcmanus]
 *
 * Revision 2.25  93/03/22  21:14:40  yazz
 * OSF lock changes.  Add uu_state field, describing whether a thread is
 * runnable or waiting on a wannabe lock-holder queue.
 * 
 * Revision 2.24  93/03/01  11:28:25  roman
 * [SPE #18] For a faster rforkmulti(), provide an option for procdup() to
 *         use a previously created task rather than always creating
 *         a task.
 * 
 * Revision 2.23  92/10/29  13:17:52  chrisp
 * Fix up RCS log entries to avoid line wraps.
 * 
 * Revision 2.22  92/08/17  12:56:28  mjl
 * Got rid of overly elaborate protocol for passing file pointers to low-level
 * TNC code.
 * 
 * Revision 2.21  92/07/26  17:31:11  bhk
 * added support for select on remote sockets
 * 
 * Revision 2.20  92/06/15  17:34:23  mjl
 * Add #defines to overload uu_sel_* fields, now also used for passing
 * struct file *'s to lower level TNC code.
 * 
 * Revision 2.19  92/06/05  13:58:38  klh
 * 	Revision 2.20  92/05/31  18:58:58  loverso
 * 		Revision 2.18.1.1  92/05/21  15:15:30  loverso
 * 		Revert to using file port instead of file pointer for select.
 * 
 * 		Added new flag VM_MMAP_CHAR_DEVICE to be used to identify mmap
 * 		of char type devices (rabii)
 * 
 * 	Revision 2.19  92/05/24  14:46:29  pjg
 * 		Revision 3.25  92/04/03  14:13:53  condict
 * 		Back out changes to vm_mmap structures that were installed in
 * 		SVR88 (They have a bug).
 * 
 * 		Revision 3.24  92/03/23  18:04:15  condict
 * 		Moved curr_ipl into uthread: uu_ipl, so it doesn't have to be
 * 		saved and restored across sleeps.
 * 
 * 		Revision 3.23  92/03/23  10:28:59  sp
 * 		Change the vm_mmap data to be a list instead of an array
 * 
 * 		Revision 3.22  92/03/13  15:19:00  condict
 * 		Add uu_lock_sleep field.  See thread_block.
 * 
 * 		Revision 3.21  92/03/05  12:52:22  sp
 * 		Add new private flag for mmap
 * 
 * 	Revision 2.18  92/05/18  12:28:24  roy
 * 		Replaced server port with node number in vm_mmap structure.
 * 		[92/05/16            roy]
 * 
 * 	Revision 2.17  92/05/12  00:06:00  loverso
 * 		select information loses file port and picks up a file pointer
 * 		and an indicator of the calling function.
 * 
 * Revision 2.18  92/05/20  13:46:04  chrisp
 * Remove cmask in the shared RW user area, since handling umask() in the
 * 	emulator fails to inform the credentials service of the update.
 * 
 * Revision 2.17  92/04/14  10:25:05  roman
 * Remove defines for u_ruid and u_rgid fields. They are not used in the server.
 * 
 * Revision 2.16  92/04/06  19:09:50  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.15  92/04/05  17:02:47  pjg
 * 	Replaced struct vport with vnode proxies and changed all
 * 	uses of it accordingly.
 * 
 * Revision 2.14  92/03/15  14:38:35  roy
 * 	Replace vnode by memory_object in vm_mmap struct. For OSF1_ADFS also
 * 	save server port in vm_mmap structure.
 * 
 * Revision 2.13  92/03/09  12:19:10  durriya
 * 	[92/01/07  23:34:57  condict]
 * 	Move include of sys/ushared.h to before sys/proc.h, to avoid circular
 * 	dependencies in struct defs.
 * 
 * 	[91/12/18  17:18:00  sp]
 * 	Delete more unused fields inherited from BSD server
 * 
 * 	Revision 3.17  91/12/17  17:19:15  jose
 * 	Added VM_MMAP_ANONYMOUS definition
 * 
 * Revision 2.12  92/03/03  13:56:51  pjg
 * 	Changed MAX_MMAP_REGIONS to 128.
 * 
 * Revision 2.11  92/03/01  18:39:34  pjg
 * 	1992/03/01  pjg
 * 	Use vnode proxies for the cwd and root directories. Change the
 * 	definitions of uu_cdirport, uu_rdirport accordingly.
 * 
 * 	1992/03/01  loverso
 * 	struct servermsg is no more.
 * 	Add struct server_oip - operation in progress.
 * 
 * Revision 2.10  92/01/14  10:49:12  roy
 * 	Added credcache_tag to uthread struct for OSF1_ADFS.
 * 
 * Revision 2.9  92/01/05  20:10:11  roy
 * 	1991/11/12  17:29:45  noemi
 * 	Added u_syscode.
 * 
 * 	1991/09/21  20:21:42  noemi
 * 	Changed u_cdir and u_rdir to struct vports. 
 * 
 * Revision 2.8  91/12/16  18:21:21  roy
 * 	91/12/13  13:03:27  sp
 * 	Remove unused fields to do with BSD restarting of system calls,
 * 	ipl levels and resource pausing.
 * 
 * 	91/12/09  18:49:45  emcmanus
 * 	Added uu_mcount_data to the uthread structure when profiling the server.
 * 	Divert cthread_fork to gprof_cthread_fork in this case.
 * 
 * 	91/10/24  09:04:31  sp
 * 	Add the mmap region table to the utask structure
 * 
 * 	91/10/21  18:47:47  emcmanus
 * 	Extra fields used by lock debugging code.
 * 
 * 	91/10/15  12:17:37  sp
 * 	Add keep on exec area in the utask structure
 * 
 * Revision 2.7  91/11/22  15:05:13  rabii
 * 	Re-position VPROC debugging fields. (chrisp)
 * 	Per-thread pointer fields added for Vproc debugging. (chrisp)
 * 
 * Revision 2.6  91/11/13  14:44:30  rabii
 * 	[Mon Oct 21 09:49:35 PDT 1991] cfj@ssd.intel.com
 * 	     Added i860 ifdefs. 
 *
 * Revision 2.5  91/10/14  13:09:45  sjs
 * 	91/09/27  12:01:51  emcmanus
 * 	Added fields for profiling.
 * 
 * 	91/09/18  17:46:51  condict
 * 	Remove all ifdef OSF1_SERVER and change def of u not to depend on
 * 	STACK_GROWTH direction (uses ur_cthread_self now -- machine indep.).
 * 
 * Revision 2.4  91/10/04  15:05:19  chrisp
 * Add in the Locus copyright notice. Get rid of field uu_sel_cred_port
 * which is no longer used by server/bsd/subr_select.c and by the
 * selq structure.
 * 
 * Revision 2.3  91/09/17  12:06:13  sjs
 * integrate Locus changes	hao
 * Remove file descriptor table for uarea, and definition of file
 * state structure.  File descriptors now handled in the emulator.
 * Added fields for new values associated with new select() and poll()
 * system call interface.
 * 
 * Revision 2.2  91/08/31  14:16:28  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.9  91/07/31  15:47:22  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.17  90/10/07  14:56:25  devrcs
 * 	Remove conditional and unused fields from struct user.  Also
 * 	add back u_arg[] so "ps u" will work.
 * 	[90/09/30  20:58:34  jvs]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  16:09:24  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  11:48:40  gm]
 * 
 * Revision 1.16  90/09/23  16:00:25  devrcs
 * 	Removed unneeded quota fields.
 * 	[90/09/03  22:36:11  nags]
 * 
 * Revision 1.15  90/09/13  11:51:31  devrcs
 * 	Put references to u_error under #if SEC_BASE.
 * 	[90/08/22  14:36:37  gmf]
 * 
 * Revision 1.14  90/08/24  12:27:57  devrcs
 * 	Removed u.u_qsave and u.u_eosys -- no more longjmps!
 * 	[90/08/20  06:41:42  gmf]
 * 
 * 	Added u_file_state changes.
 * 	Removed u.u_rval* and its buddies in the union.
 * 
 * 	Removed  u.u_ap, and u.u_arg.
 * 	[90/08/17  17:48:26  nags]
 * 
 * Revision 1.13  90/07/27  09:08:35  devrcs
 * 	Fixed fake user area to use sig_t.
 * 	[90/07/13  19:31:59  jeffc]
 * 
 * 	Fix signal function pointer data types.
 * 	[90/07/13  16:54:40  jeffc]
 * 
 * Revision 1.12  90/07/05  23:14:47  devrcs
 * 	Add support for per thread synchronous signals
 * 	[90/06/29  09:15:38  sp]
 * 
 * 	Add u_argp and u_envp to user struct.
 * 	[90/06/29  15:32:03  brezak]
 * 
 * Revision 1.11  90/06/22  20:54:45  devrcs
 * 	Select event moved from uthread to thread structure.
 * 	[90/06/18  17:09:50  nags]
 * 
 * 	Moved flag_field structure out of _KERNEL conditional.
 * 	[90/06/14            gm]
 * 
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware changes.				seiden@osf.org
 * 	Added u_ioch for SysV accounting.		ers@osf.org
 * 	Added process lock; eliminated u_ttyd, u_ttyp.	ers@osf.org
 * 	Added uu_semundo, uu_shmsegs for SysV IPC.	bet@osf.org
 * 	Placed new kernel locks under #ifdef _KERNEL.	gm@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.		coren@osf.org
 * 	Integrated 4.4BSD file system changes [1/5/90].	noemi@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	New structure in user.h for syscontext.h.	gmf@osf.org
 * 	Merged 2.5, MACH_VFS, Encore parallelization.	alan@encore.com
 * 	Timer locks for parallelizing {get,set}itimer.	alan@encore.com
 * 	Revamp select for mp environment.		boykin@encore.com
 * 	File descriptor locking.			alan@encore.com
 * 	Remove unused fields from struct utask.		dbg@cmu.edu
 * 	New Signal Handling from Olivetti.		rvb@cmu.edu
 * 	Created new user identity structure.		gm0w@cmu.edu
 * 	More cleanup.					rpd@cmu.edu
 * 	Restored accidentally deleted cr_ruid field.	mrt@cmu.edu
 * 	Unconditionalized most items in user structs.	mrt@cmu.edu
 * 	Conditional hacking (MACH, ROMP, VICE).		gm0w@cmu.edu
 * 	MACH_AFS:  add cr_pag to ucred.			jsb@cmu.edu
 * 	Mips: signal trampoline like balance		af@cmu.edu
 * 	Vnode support, including credentials.		jsb@cmu.edu
 * 	Condensed CS conditionals.			mwyoung@cmu.edu
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Add stack start, end, direction for limits.	mwyoung@cmu.edu
 * 	u_rpause and u_rfs must be per-task.		dbg@cmu.edu
 * 	Added URPW_* resource pause definitions.	mja@cmu.edu
 * 	Move controlling terminal information to proc.	mja@cmu.edu
 * 	Optimize u-area references for the multimax	kirschen@encore.com
 * 	Add include of param.h for NGROUPS, etc.	kirschen@encore.com
 * 	Expand "u" to the constant U_ADDRESS.		rvb@cmu.edu
 * 	VICE:  Removed unused Fid typedef.		jjk@cmu.edu
 * 	Eliminated conditionals, purged history.	avie@cmu.edu
 * 	Move UMODE_*  definitions to <sys/syscall.h>.	mja@cmu.edu
 * 	CS_RFS: Add new RFS_EROOT definition.		mja@cmu.edu
 * 	CS_GENERIC:  u_maxuprc field for max processes.	mja@cmu.edu
 * 	CS_RFS:  added u_rfs definition.		mja@cmu.edu
 * 	CS_GENERIC:  modes field, symbol definitions.	mja@cmu.edu
 * 	CS_RPAUSE:  resource pause definitions.		mja@cmu.edu
 * 	CS_RFS:  added u_syscode definition.		mja@cmu.edu
 * 	Upgraded to 4.2BSD.				mja@cmu.edu
 * 	Include errno.h from include directory.		mja@cmu.edu
 * 	CS_AID:  u_aid field records process account ID	mja@cmu.edu
 * 	[90/06/12  21:40:03  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)user.h	7.1 (Berkeley) 6/4/86
 */

#ifndef	_SYS_USER_H_
#define _SYS_USER_H_

#if	defined(_KERNEL) || defined(KERNEL)
#include <sys/unix_defs.h>
#endif
#ifdef	KERNEL
#include <map_uarea.h>
#endif	KERNEL
#include <kern/event.h>
#include <uxkern/import_mach.h>

#include <mach/boolean.h>
#include <sys/types.h>
#include <sys/param.h>
#include <kern/queue.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/namei.h>
#include <sys/ucred.h>
#include <sys/sem.h>
#include <machine/pcb.h>
#ifdef _KERNEL
#include <sys/secdefines.h>
#include <kern/parallel.h>
#else
#ifndef	_KERN_LOCK_H_
#define _KERN_LOCK_H_
typedef int simple_lock_data_t;			/* XXX */
#endif
#endif	/* _KERNEL */

#include <uxkern/sthread.h>		/* for server_oip */

#ifdef NX
#include "dbglib.h"
#endif

/*
 * Per process structure containing data that
 * isn't needed in core when the process is swapped out.
 */

#define MAXCOMLEN	16		/* <= MAXNAMLEN, >= sizeof(ac_comm) */
#define MAXLOGNAME      12              /* >= UT_NAMESIZE */

struct flag_field {
	int	fi_flag;
#ifdef	_KERNEL
	udecl_simple_lock_data(,*fi_lock)
#else
	char	*dummy_fi_lock;		/* padding space to conserve size */
#endif
};

#if	(defined(_KERNEL) || defined(SHOW_UTT))
/*
 *	Per-thread U area.
 *
 *	It is likely that this structure contains no fields that must be
 *	saved between system calls.
 */
struct uthread {
	int	*uu_ar0;		/* address of users saved R0 */

/* syscall parameters, results and catches */
#if	SEC_BASE
	char	uu_error;		/* return error code */
#endif
#ifdef	OSF1_ADFS
	long	uu_syscode;		/* system call # */
#endif


/* namei & co. */
	struct unameicache {		/* last successful directory search */
		int nc_prevoffset;	/* offset at which last entry found */
		ino_t nc_inumber;	/* inum of cached directory */
		dev_t nc_dev;		/* dev of cached directory */
		time_t nc_time;		/* time stamp for cache entry */
	} uu_ncache;
	struct	nameidata uu_nd;
	long	uu_spare[8];

/* thread exception handling */
	int		uu_code;	/* ``code'' to trap */
	char		uu_cursig;	/* p_cursig for exc. */
	int  		uu_sig;		/* p_sig for exc. */

/* per thread signal state */
	sig_t	uu_tsignal[NSIG+1];	/* disposition of signals */

	struct proc	*uu_procp;	/* pointer to proc structure */
	boolean_t	uu_proc_exit;	/* inside proc_exit? */
	boolean_t	uu_lock_sleep;	/* going to sleep on busy lock? */
	queue_chain_t	uu_server_list;	/* chain server thread to process
					   it is serving */
#if	MAP_UAREA
	int		uu_share_lock_count;
					/* for panic test */
#endif	/* MAP_UAREA */
	unsigned int	uu_master_lock;	/* current count of # refs on ML */
#if	MACH_LDEBUG
	int		uu_slock_trace[MASTER_TRACE_COUNT];	/* parallel.h */
	int		uu_sunlock_trace[MASTER_TRACE_COUNT];
#endif	/* MACH_LDEBUG */
	int		uu_ipl;		/* current interrupt level, if this
					 * thread is using locking spl's */
	char		uu_interruptible; /* is thrd sleeping interruptibly? */
	char		uu_wired;	/* is this a wired thread? */
	char		uu_side_flag;	/* is this a side thread? */
	struct mutex	uu_lock;	/* for locking condition */
	struct condition		/* for sleeping and blocking */
			uu_condition;
	int		uu_timeout;	/* timeout in ticks */
	int		uu_wait_event;	/* event we are waiting on */
	int		uu_wait_result;	/* set by thread_wakeup*() */
	int		uu_state;	/* runnable or queued for wait? */
	char	        *uu_wait_mesg;	/* wait msg, for debugging */
	queue_chain_t	uu_sleep_link;	/* Protected by global sleep_lock,
 					 * not by the thread lock */

	/*
	 * These uu_sel_ fields are used to carry information from
	 * sbsd_sel_poll_delay to select_enqueue via FOP_SELECT
	 */
	mach_port_t	uu_sel_file_port;	/* file port */
	mach_port_t	uu_sel_delay_port;	/* delayed reply port */
#ifdef TNC
	unsigned long	uu_sel_id;		/* distributed select cookie */
	int		uu_sel_flags;		/* flags for distrib. select */
#endif
	short		uu_sel_index;		/* returned to emulator */
	short		uu_sel_events;		/* events bits requested */
	boolean_t	uu_sel_again;		/* true if requeued */
#ifdef	TNC
/* Use this to pass file port/file pointer to low-level TNC code. */
#define uu_opn_filep		uu_sel_file_port
/* Use this to pass task port to low-level TNC code. */
#define uu_fork_task		uu_sel_file_port
#endif

	mach_msg_header_t	*uu_reply_msg;	/* pointer to reply msg */
	vm_size_t		uu_current_size; /* size of current data area */

	struct server_oip	uu_oip;		/* server op in progress */

	char			*uu_vpargfmt;	/* VPROC debug printf format */
	char			*uu_pvpargfmt;	/* VPROC debug printf format */

#if MACH_LDEBUG	/* Known from <unix_defs.h> */
	unsigned int	uu_lock_count;
	int		uu_lock_addr[MAX_LOCK];
#endif

};

typedef	struct uthread *	uthread_t;

struct utask_nd {
#ifdef  OSF1_ADFS
	struct vnode_proxy utnd_cdir;	/* current directory port/vnode */
	struct vnode_proxy utnd_rdir;	/* root directory port/vnode */
#else
	struct vnode *utnd_cdir;	/* current directory */
	struct vnode *utnd_rdir;	/* root directory of current process */
#endif
#ifdef	_KERNEL
	udecl_simple_lock_data(,utnd_lock)
#endif
};

/*
 * Structure associated with mapped regions 
 * (incl. keep on exec functionality).
 */
struct vm_mmap {
	struct vm_mmap	*link;
	vm_address_t	start;
	vm_size_t	length;
        memory_object_t pager;
#ifdef OSF1_ADFS
	node_t		node;
#endif
	vm_offset_t	offset;
	unsigned	flags;
};

typedef	struct vm_mmap	*vm_mmap_t;

#define	VM_MMAP_FORCE_KEEP_ON_EXEC	0x01
#define	VM_MMAP_KEEP_ON_EXEC		0x02
#define	VM_MMAP_ANONYMOUS		0x04
#define	VM_MMAP_PRIVATE			0x08
#define	VM_MMAP_CHAR_DEVICE		0x10

/*
 *	Per-task U area - global process state.
 */
struct utask {
	struct	proc *uu_procp;		/* pointer to proc structure */
	char	uu_comm[MAXCOMLEN + 1];

/* 1.1 - processes and protection */
#if	ibmrt
	char	uu_calltype;	/* ROMP_DUALCALL 0 - old calling sequence */
#endif
	char    uu_logname[MAXLOGNAME]; /* login name, if available */

/* 1.2 - memory management */
	size_t	uu_tsize;		/* text size (clicks) */
#if	!MAP_UAREA
	size_t	uu_dsize;		/* data size (clicks) */
#endif	/* !MAP_UAREA */
	size_t	uu_ssize;		/* stack size (clicks) */
	caddr_t	uu_text_start;		/* text starting address */
#if	!MAP_UAREA
	caddr_t	uu_data_start;		/* data starting address */
#endif	/* !MAP_UAREA */
	caddr_t	uu_stack_start;		/* stack starting address */
	caddr_t	uu_stack_end;		/* stack ending address */
	boolean_t uu_stack_grows_up;	/* stack grows at high end? */
	time_t	uu_outime;		/* user time at last sample */

/* 1.3 - signal management */
#if	!MAP_UAREA
	sig_t	uu_signal[NSIG+1];	/* disposition of signals */
	int	uu_sigmask[NSIG+1];	/* signals to be blocked */
#ifdef	i386
	int	(*uu_sigreturn)();
#endif
#ifdef	multimax
	int	(*uu_sigcatch)();	/* used as a way not to do tramp. */
#endif
#if	defined(balance) || defined(mips) || defined(i860)
	int	(*uu_sigtramp)();	/* signal trampoline code */
#endif /* defined(balance) || defined(mips) || defined(i860) */
	int	uu_sigonstack;		/* signals to take on sigstack */
	int	uu_sigintr;		/* signals that interrupt syscalls */
#endif	/* !MAP_UAREA */
	int	uu_oldmask;		/* saved mask from before sigpause */
#if	!MAP_UAREA
	struct	sigstack uu_sigstack;	/* sp & on stack state variable */
#endif	/* !MAP_UAREA */

	struct utask_nd uu_utnd;

	short	uu_cmask;		/* mask for file creation */

/* 1.5 - timing and statistics */
	struct	rusage uu_ru;		/* stats for this proc */
	struct	rusage uu_cru;		/* sum of stats for reaped children */
	long	uu_ioch;		/* # of chars read/written */
	struct	itimerval uu_timer[3];
	struct	timeval uu_start;
	struct	flag_field uu_acflag;

/* This should be #if PROFILING, but emulator doesn't know about that. */
	struct uuprof {			/* profile arguments */
		simple_lock_data_t *pr_lock;	/* lock for thread updating */
		short	*pr_base;	/* buffer base */
		unsigned pr_size;	/* buffer size */
		unsigned pr_off;	/* pc offset */
		unsigned pr_scale;	/* pc scaling */
	} uu_prof;
	boolean_t	uu_prof_on;	/* enable task profiling if TRUE */
	short		*uu_prof_buf;	/* buffer-base address in user space */
/* #endif PROFILING. */

	u_short	uu_maxuprc;		/* max processes per UID (per tree) */

/* 1.6 - resource controls */
#if	!MAP_UAREA
	struct	rlimit uu_rlimit[RLIM_NLIMITS];
#endif	/* !MAP_UAREA */

/* sysv ipc */
	struct sem_undo *uu_semundo; 	/* semaphore undo structure */
	u_short		 uu_shmsegs;	/* # attached shared-memory-segments */
#ifdef TNC
	u_char		 uu_svipc_flag;	/* set if task Ever does an svipc op */
#endif

	u_char	uu_lflags;		/* process lock flags */
#define UL_TXTLOCK	1
#define UL_DATLOCK	2

/* pointers and length to args and env */
        char		*uu_argp;
        char		*uu_envp;
        u_short		uu_arg_size;
        u_short		uu_env_size;
        
/* IPD debug info */
#ifdef NX
	ipd_msg_info_t	*uu_ipd_msg_info;
#endif

/* mapped region data */
	lock_data_t	uu_mmap_lock;
	vm_mmap_t	uu_mmap;
        
#ifdef	_KERNEL
	udecl_simple_lock_data(,uu_timer_lock) /* protects u_time[] */
	udecl_simple_lock_data(,uu_handy_lock) /* handy lock for misc. data */
#endif

#if defined(_KERNEL) && defined(PARACORE)
	
	/*
	 * The following fields are used to support core dumping of
	 * parallel applications.
	 */
	int		uu_core_action;		/* requested core action */
	mach_port_t	uu_core_directory;	/* core directory vnode port */
	mach_port_t	uu_root_directory;	/* root directory vnode port */
	boolean_t	uu_ports_copied;	/* for deallocate ports */
	
	/*
	 * The following fields are used to get the absolute pathname
	 * of the program been execed.
	 */
	char		uu_exec_prg_name[PATH_MAX];
	struct utask_nd	uu_exec_utnd;

	/*
	 * The following field is used to support core dumping of
	 * multithread applications.
	 */
	thread_t	uu_faulting_thread;
	
#endif /* defined(_KERNEL) && defined(PARACORE) */
};


#endif	/* (defined(_KERNEL) || defined(SHOW_UTT)) */

/*
 * Struct user:
 * This structure is purely for the benefit of user-level code using
 * the table() call to get what looks like a more traditional u. area.
 * It contains only fields that could be of interest to a user level 
 * privileged program, such as ps, or a debugger.  It should really
 * be moved to sys/table.h.
 *
 * NOTE:  for now, we pad it out to OSF/1 R1.0 size for binary compatibility.
 * The pad fields are marked u_pad_x, and should be removed when
 * compatibility is no longer required.
 */
/*
 * This MUST match the definition of "struct user" in OSF/1 IK
 * and cannot contain any #ifdef's
 */
struct	user {
	struct	pcb u_pcb;
	struct	proc *u_procp;		/* pointer to proc structure */
	int	*u_ar0;			/* address of users saved R0 */
	char	u_comm[MAXCOMLEN + 1];
	int	u_pad_1[8];		/* XXX */
#define u_arg u_pad_1			/* XXX */

/* 1.1 - processes and protections */
	char	u_logname[MAXLOGNAME];	/* login name, if available */

/* 1.2 - memory management */
	size_t	u_tsize;		/* text size (clicks) */
	size_t	u_dsize;		/* data size (clicks) */
	size_t	u_ssize;		/* stack size (clicks) */
	int	u_pad_2[5];		/* XXX u_{text,data}_start, u_stack_* */
	time_t	u_outime;		/* user time at last sample */

/* 1.3 - signal management */
	sig_t	u_signal[NSIG+1];	/* disposition of signals */
	int	u_pad_3[NSIG+1+3];	/* XXX */
	int	u_code;			/* for core dump/debugger XXX */
	int	u_pad_4[2];		/* XXX */

/* 1.4 - descriptor management */
	int	u_pad_5[83];		/* XXX  64 int + 64 char + 3 int */
	short	u_cmask;		/* mask for file creation */

/* 1.5 - timing and statistics */
	struct	rusage u_ru;		/* stats for this proc */
	struct	rusage u_cru;		/* sum of stats for reaped children */
	int	u_pad_6[15];		/* XXX */
	struct	timeval u_start;
	struct	flag_field u_acflag;

/* The remainder of the old structure is not of interest to user code */
};

/* u_error codes */
#include <sys/errno.h>

#ifdef	_KERNEL

#if 	MAP_UAREA
#include <sys/ushared.h>
#endif

/*
 *	To get p_utask, we include sys/proc.h.  But this introduces
 *	circular includes.  Hence we must do this include after
 *	defining struct utask, which is the part of sys/user.h that
 *	sys/proc.h needs.
 */

#include <sys/proc.h>

extern int u_offset;
#define u (*(struct uthread *)((int)&(ur_cthread_self()) + u_offset))

#define current_thread()	(&u)

#ifdef	__STDC__
#define	UTASK(x)	uu_procp->p_utask.uu_##x
#define UTHREAD(x)	uu_##x
#else	/* __STDC__ */
#define	UTASK(x)	uu_procp->p_utask.uu_/**/x
#define UTHREAD(x)	uu_/**/x
#endif	/* __STDC__ */

#if 	MAP_UAREA

#ifdef	__STDC__
#define USHARED_RO(x)	uu_procp->p_shared_ro->us_##x
#define USHARED_RW(x)	uu_procp->p_shared_rw->us_##x
#else	/* __STDC__ */
#define USHARED_RO(x)	uu_procp->p_shared_ro->us_/**/x
#define USHARED_RW(x)	uu_procp->p_shared_rw->us_/**/x
#endif	/* __STDC__ */

#define u_shared_ro	uu_procp->p_shared_ro
#define u_shared_rw	uu_procp->p_shared_rw
#define us_shared_lock(x)	spin_lock(&x->u_shared_rw->us_lock);
#define us_shared_unlock(x)	spin_unlock(&x->u_shared_rw->us_lock);
#define uu_rlimit	u_shared_ro->us_rlimit
#define	uu_data_start	u_shared_ro->us_data_start
#define uu_dsize	u_shared_rw->us_dsize
#define uu_sigstack	u_shared_rw->us_sigstack
#define uu_sigonstack	u_shared_rw->us_sigonstack
#define uu_sigintr	u_shared_rw->us_sigintr
#define uu_sigmask	u_shared_rw->us_usigmask
#if	defined(i386)
#define uu_sigreturn	u_shared_rw->us_sigreturn
#endif	/* defined(i386) */
#define uu_signal	u_shared_rw->us_signal
#if 	defined(balance) || defined(mips) || defined(i860)
#define	uu_sigtramp	u_shared_rw->us_sigtramp
#endif	/* defined(balance) || defined(mips) || defined(i860) */

#else	/* MAP_UAREA */

#define USHARED_RO(x)	UTASK(x)
#define USHARED_RW(x)	UTASK(x)

#endif	/* MAP_UAREA */

#define u_findx		uu_procp->p_utask.uu_findx
#define u_fmap		uu_procp->p_utask.uu_fmap
#define	u_reply_msg	uu_reply_msg
#define	u_master_lock	uu_master_lock

/* shortcuts for BSD4.3 to OSF/1 translation */
#if     OSF1_ADFS
#define uu_cdir		uu_utnd.utnd_cdir.vpx_vnode
#define uu_rdir		uu_utnd.utnd_rdir.vpx_vnode
#define uu_cdirport     uu_utnd.utnd_cdir.vpx_port
#define uu_rdirport     uu_utnd.utnd_rdir.vpx_port
#define uu_cdirproxy	uu_utnd.utnd_cdir
#define uu_rdirproxy	uu_utnd.utnd_rdir
#else
#define uu_cdir		uu_utnd.utnd_cdir
#define uu_rdir		uu_utnd.utnd_rdir
#endif
/* end of shortcuts */

#define u_pcb		UTHREAD(pcb)
#define u_procp		uu_procp

#define u_ar0		UTHREAD(ar0)
#ifdef	OSF1_ADFS
#define	u_syscode	UTHREAD(syscode)
#endif
#define u_comm		UTASK(comm)
#define u_calltype	UTASK(calltype)	/* ROMP_DUALCALL */
#define u_logname	UTASK(logname)
#define	u_utnd		UTASK(utnd)

#if	SEC_BASE
#define u_error		UTHREAD(error)
#endif

#define u_nd		UTHREAD(nd)
#define u_spare		UTHREAD(spare)
#define u_cdir		UTASK(cdir)
#define u_rdir		UTASK(rdir)
#ifdef  OSF1_ADFS
#define u_cdirport      UTASK(cdirport)
#define u_rdirport      UTASK(rdirport)
#define u_cdirproxy	UTASK(cdirproxy)
#define u_rdirproxy	UTASK(rdirproxy)
#endif
#define u_cred		u_nd.ni_cred
#define u_uid		u_cred->cr_uid
#define u_gid		u_cred->cr_gid
#define u_ngroups	u_cred->cr_ngroups
#define u_groups	u_cred->cr_groups

#define u_tsize		UTASK(tsize)
#define u_dsize		USHARED_RW(dsize)
#define u_ssize		UTASK(ssize)
#define u_text_start	UTASK(text_start)
#define u_data_start	USHARED_RO(data_start)
#define u_stack_start	UTASK(stack_start)
#define u_stack_end	UTASK(stack_end)
#define u_stack_grows_up UTASK(stack_grows_up)
#define u_outime	UTASK(outime)

#define u_signal	USHARED_RW(signal)

#if	MAP_UAREA
  /* 
   * XXX u_sigmask is redirected to us_usigmask to avoid name conflict
   * with the redirection of p_sigmask...
   */
#define u_sigmask	USHARED_RW(usigmask)
#else	/* MAP_UAREA */
#define u_sigmask	USHARED_RW(sigmask)
#endif	/* MAP_UAREA */

/* XXX - barbou@gr.osf.org: no thread signal dispositions yet */
#define u_tsignal	USHARED_RW(signal)
#ifdef	i386
#define	u_sigreturn	UTASK(sigreturn)
#endif
#ifdef	multimax
#define u_sigcatch	UTASK(sigcatch)
#endif
#if	defined(balance) || defined(mips) || defined(i860)
#define u_sigtramp	USHARED_RW(sigtramp)
#endif /* defined(balance) || defined(mips) || defined(i860) */
#define u_sigonstack	USHARED_RW(sigonstack)
#define u_sigintr	USHARED_RW(sigintr)
#define u_oldmask	UTASK(oldmask)
#define u_code		UTHREAD(code)
#define u_sigstack	USHARED_RW(sigstack)

#define u_onstack	u_sigstack.ss_onstack
#define u_sigsp		u_sigstack.ss_sp

#if	UNIX_LOCKS
#define	u_timer_lock	UTASK(timer_lock)
#define u_handy_lock    UTASK(handy_lock)
#endif
#define u_cmask		UTASK(cmask)

#define u_ru		UTASK(ru)
#define u_cru		UTASK(cru)
#define u_ioch		UTASK(ioch)
#define u_timer		UTASK(timer)
#define u_XXX		UTASK(XXX)
#define u_start		UTASK(start)
#define u_acflag	UTASK(acflag)

#define u_prof		UTASK(prof)
#define u_prof_on	UTASK(prof_on)
#define u_prof_buf	UTASK(prof_buf)

#define u_maxuprc	UTASK(maxuprc)

#define u_lflags	UTASK(lflags)

#define u_rlimit	USHARED_RO(rlimit)

#define u_ncache	UTHREAD(ncache)

#define u_sig		UTHREAD(sig)
#define u_cursig	UTHREAD(cursig)

#define u_semundo	UTASK(semundo)
#define u_shmsegs	UTASK(shmsegs)
#ifdef TNC
#define u_svipc_flag	UTASK(svipc_flag)
#endif

#define u_argp          UTASK(argp)
#define u_envp          UTASK(envp)
#define u_arg_size      UTASK(arg_size)
#define u_env_size      UTASK(env_size)

#define	U_HANDY_LOCK()		usimple_lock(&u.u_handy_lock)
#define	U_HANDY_UNLOCK()	usimple_unlock(&u.u_handy_lock)
#define	U_HANDY_LOCK_INIT(up)	usimple_lock_init(&(up)->uu_handy_lock)

#define	UTND_LOCK(utndp)	usimple_lock(&(utndp)->utnd_lock)
#define	UTND_UNLOCK(utndp)	usimple_unlock(&(utndp)->utnd_lock)
#define	UTND_LOCK_INIT(utndp)	usimple_lock_init(&(utndp)->utnd_lock)

#define	FLAG_LOCK(flagp)	usimple_lock((flagp)->fi_lock)
#define	FLAG_UNLOCK(flagp)	usimple_unlock((flagp)->fi_lock)

#define U_MMAP_READ_LOCK(up)	lock_read(&(up)->uu_mmap_lock)
#define U_MMAP_WRITE_LOCK(up)	lock_write(&(up)->uu_mmap_lock)
#define U_MMAP_UNLOCK(up)	lock_done(&(up)->uu_mmap_lock)
#define U_MMAP_LOCK_INIT(up)	lock_init(&(up)->uu_mmap_lock, 1)

/*
 * The U_TIMER_LOCK must always be taken before the time lock.  Or else!
 * The U_TIMER_LOCK must always be held at splhigh.
 */
#define	U_TIMER_LOCK()		usimple_lock(&u.u_timer_lock)
#define	U_TIMER_UNLOCK()	usimple_unlock(&u.u_timer_lock)
#define	U_TIMER_LOCK_INIT(p)	usimple_lock_init(&(p)->uu_timer_lock)

#endif	/* _KERNEL */
#endif	/* _SYS_USER_H_ */
