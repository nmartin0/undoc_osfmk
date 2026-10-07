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
 * $Log: kern_fork.c,v $
 * Revision 1.15  1995/02/01  21:27:06  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.14  1994/11/18  20:27:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/10/25  22:53:33  yazz
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
 * Side-thread changes.  Init new p_side_ref_count.  Renamed p_sigref to the
 * more general p_exit_hold_count.  Renamed p_issig_count to the more correct
 * p_issig_flag.  Init new p_side_ref_count to 0 for new proc.
 *
 * Revision 1.12  1994/09/13  15:53:33  johannes
 * initialization of new field 'uu_faulting_thread' in utask structure
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
 * Revision 1.11  1994/07/27  16:27:12  johannes
 * In newproc_body() the new utask fields are passed to the new process like
 * it is done for root und current directories.
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
 * Revision 1.10  1994/06/29  16:48:18  johannes
 * newproc(): initializing the new utask fields
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
 * Revision 1.9  1994/01/13  17:53:50  jlitvin
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
 * Revision 1.8  1993/07/16  20:28:58  cfj
 * Put back check in procdup() to check u.uu_fork_task before calling norma_task_create().
 *
 * Revision 1.7  1993/07/14  17:47:22  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:30  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/18  03:19:38  cfj
 * Complete MI Driver merge.
 *
 * Revision 1.5  1993/05/06  19:03:04  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/14  14:24:49  cfj
 * Merge with T9.5.
 *
 * Revision 1.1.1.1  1993/05/03  17:23:45  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2.8.1  1993/04/12  16:36:42  cfj
 * Go with latest final version of rforkmulti for T9.5.
 *
 * Revision 1.3  1993/03/11  16:16:39  cfj
 * Merged in final version of the new forkmulti() from Locus but kept the workaround.
 *
 * Revision 2.32  93/06/20  18:26:04  yazz
 * [ ad1.04 merge ]
 * 	Surround norma_task_create() call with ux_server_thread_blocking/
 * 	unblocking() calls.
 * 
 * Revision 2.31  93/06/16  15:25:41  klh
 * 	Revision 2.31  93/06/08  11:54:19  rabii
 * 		Made the valid_node_list dynamically allocated, also we now use macros
 * 		from  param.h to do the bitmaps. (rabii)
 * 
 * 	Revision 2.30  93/06/04  12:25:35  rabii
 * 		Added node argument to pproc_fork and procdup, also newproc now calls
 * 		newproc_body which implements newproc but takes a node argument. 
 * 		Sanity check is also added to prevent norma_task_create on invalid
 * 		nodes. (rabii)
 * 
 * 	Revision 2.29  93/05/27  10:34:16  bolinger
 * 		Fix up last log entry.
 * 
 * 	Revision 2.28  93/05/27  10:27:49  bolinger
 * 		Add initialization for new lock protecting task's vm_mmap region list.
 * 
 * Revision 2.30  93/06/02  09:49:48  yazz
 * For Sys V IPC under TNC clear an additional svipc-related flag at fork time.
 * 
 * Revision 2.29  93/05/04  16:51:25  yazz
 * Added ux_server_thread_blocking/unblocking() surrounding
 * norma_task_create().
 * 
 * Revision 2.28  1993/04/29  13:58:33  klh
 * 	Revision 2.27  93/01/14  13:10:58  loverso
 * 		Moved rproc initialization here. (loverso)
 *
 * 	Revision 2.26  93/01/07  11:16:36  condict
 * 		1992/10/21  12:51:27  emcmanus
 * 		Updated to new MK profiling interface.
 * 		[1992/09/24  19:46:24  emcmanus]
 *
 * 		Emulator profiling changes: need to enable profiling for forked
 * 		task if emulator profiling is on.
 * 		[1992/08/05  17:56:02  emcmanus]
 *
 * Revision 2.27  93/03/01  11:28:02  roman
 * [SPE #18] For a faster rforkmulti(), provide an option for procdup() to
 * 	use a previously created task rather than always creating
 * 	a task.
 * 
 * Revision 2.26  92/10/06  12:09:05  roman
 * Fix RCS comments.
 * 
 * Revision 2.25  92/10/05  13:37:19  klh
 * 	Revision 2.25  92/09/11  15:44:58  rabii
 * 		Removed reference to "rproc" in printf (rabii)
 * 
 * 	Revision 2.24  92/09/11  14:17:27  rabii
 * 		Added message to tell when a node registers for rproc (rabii)
 * 
 * 	Revision 2.23  92/08/26  12:10:03  loverso
 * 		Initialize p_callback_thread in the proc struct (loverso).
 * 
 * Revision 2.24  92/08/06  16:37:50  roman
 * Make fixes to allow builds without REMOTE_PROC to work.
 * 
 * Revision 2.23  92/08/06  13:34:29  klh
 * 	Revision 2.21  92/07/28  20:02:29  rabii
 * 		Modified to use new rproc linked list. (rabii)
 * 
 * 		Revision 3.17  92/04/08  20:45:02  barbou
 * 		Fix for bug #105: request dead name notification for task 
 *		ports. (loverso)
 * 
 * Revision 2.22  92/07/08  09:02:54  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 2.21  92/07/07  13:07:13  klh
 * 	Revision 2.20  92/06/30  22:45:38  loverso
 * 		Pass in ruid and rgid in credentials_allocate (rabii)
 * 
 * Revision 2.20  92/06/17  12:15:28  roman
 * Change to credentials_allocate() lost during merges for v0.9. Restored
 * 	to former glory.
 * 
 * Revision 2.19  92/06/05  13:54:59  klh
 * 	Revision 2.19  92/05/24  14:13:46  pjg
 * 		Modified last change to be closer to Grenoble code 
 * 		(vm_mmap_fork -> vm_fork).  Added uarea_lock_init (condict).
 * 		[92/05/20            roy]
 * 
 * 	Revision 2.18  92/05/18  12:33:52  roy
 * 		Call vm_mmap_fork from procdup.
 * 		[92/05/18            roy]
 * 
 * Revision 2.18  92/05/01  07:42:26  roman
 * For credentials, use new interface to credentials_allocate() that makes
 * 	the subsequent call to credentials_set_state() unnecessary.
 * 
 * Revision 2.17  92/04/14  10:00:17  roman
 * Take master lock within pproc_fork().
 * 
 * Revision 2.16  92/04/07  13:41:16  pjg
 * 	Print the debug message of remote_proc under a pathchable conditional
 * 	(rproc_debug).
 * 
 * Revision 2.15  92/04/05  16:46:30  pjg
 * 	Changed referencing and dereferencing of vnode proxies in newproc
 * 	(pjg).
 * 
 * 	Get rid of task parameter to credentials_allocate(), and add the 
 * 	parameter to credentials_set_state(). (roman)
 * 
 * Revision 2.14  92/03/15  17:14:36  roy
 * 	Added new credentials interface (rabii)
 * 
 * Revision 2.13  92/03/09  14:06:33  durriya
 * 	Initialize the callback_port in the proc struct (loverso).
 * 
 * 	Revision 3.13  92/03/02  11:33:33  condict
 * 	Fix newproc panic.  If procdup fails, need to clear ref count of proc.
 * 
 * 	Revision 3.12  92/02/28  15:11:08  david
 * 	in fork, proc now enters idle state (SIDL) when initialised, 
 * 	possible race with server_thread_register (bug #46)
 * 
 * 	Revision 3.11  92/01/07  23:33:04  condict
 * 	Get rid of mutex_init on a simple lock.  Use simple_lock_init instead.
 * 
 * 	Revision 3.10  91/12/20  17:56:00  barbou
 * 	Initialize new shared fields for server-emulator consistency 
 *	verification.
 * 
 * Revision 2.12  92/03/01  18:42:15  pjg
 * 	Changed the references to the cwd and root vnodes to use vnode proxies.
 * 
 * Revision 2.11  92/02/11  22:13:09  pjg
 * 	Change task_create_remote() to norma_task_create() (new name).
 * 	Change code so that task_create_remote() is always called when TNC
 * 	is in place. Change printf's to report correct names of 
 * 	routines (roman@locus).
 * 
 * Revision 2.10  92/01/14  11:08:46  roy
 * 	allocate_credentials now called from pproc_fork instead of newproc.
 * 	Added remote_proc support (sjs).
 * 
 * Revision 2.9  91/12/17  16:19:17  roy
 * 	91/10/24  09:04:01  sp
 * 	Copy parents mapped region information on fork
 * 
 * 	91/10/23  16:37:39  condict
 * 	Use the time var and TIME LOCKS to get the time, instead of get_time.
 * 
 * 	91/10/17  15:44:28  condict
 * 	Fix bug: convert ro_pages_nb to number of bytes in address calculation.
 * 
 * Revision 2.8  91/12/08  09:57:33  rabii
 * 	Changed the call to allocate credentials from procdup to newproc
 * 	after all the proc fields maing p_pid have been correctly initialized
 * 	to prevent passing in incorrect proc info to the master service.
 * 
 * Revision 2.7  91/11/22  14:54:22  rabii
 * 	Locus Merge
 * 	Merged LCC and new OSF versions.
 * 	a) Initialize profiling information in new process.
 * 	b) Dynamic computation of the number of pages shared with the emulator.
 * 	(mbarnett)
 * 	Change variable names so that they make more sense. (roman)
 * 	pproc_fork replaces pproc_fork1, and isvfork parameter dropped. (chrisp)
 * 
 *
 * Revision 2.6  91/11/05  12:16:39  sjs
 * Fixed RCS comments
 * 
 * Revision 2.5  91/10/14  11:52:46  sjs
 * 	Revision 3.5  91/09/27  11:56:15  emcmanus
 * 	Initialize profiling information in new process.
 * 
 * 	Revision 3.4  91/09/20  19:15:19  barbou
 * 	Dynamic computation of the number of pages shared with the emulator.
 * 
 * Revision 2.4  91/10/04  14:44:30  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:33:20  rabii
 * 	Merge of V2.0 and Locus (locus check-in by hao)
 * 	File descriptors now handled in emulator. Remove the dup-ing of
 * 	file descriptors from parent to child from this file. Additionally,
 * 	the emulator of the parent is responsible for the initial 
 * 	thread_resume() call now, so remove this from the server code.
 * 	Add call to allocate credentials port. Moved actual system call
 * 	handling to vproc directory. Removed pid allocation and process
 * 	relationship manipulation to vproc and pvproc directories.
 * 
 * Revision 2.2  91/08/31  13:21:42  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/12  15:34:24  sp
 * Initialize new signal delivery variables
 * 
 * Revision 3.2  91/08/07  17:16:30  jose
 * Adapted from 1.0.2 to OSF/1s environment
 * 
 * Revision 1.13.4.2  91/04/25  15:55:37  jeffc
 * 	Fix deletion of proc table entry during failed fork. Proc
 * 	was being incorrectly removed from the pid hash chain, resulting
 * 	in panic: exit.
 * 	[91/03/27  12:55:29  jeffc]
 * 
 * Revision 1.13  90/10/31  13:48:57  devrcs
 * 	Changed fork and vfork to return the return value of their called
 * 	work routine, fork1 to their caller, syscall.
 * 	[90/10/26  16:39:59  hankin]
 * 
 * Revision 1.12  90/10/07  13:17:14  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:55:32  gm]
 * 
 * 	Fixed to copy the saved setuid and saved setgid to the new proc
 * 	structure. Also added p_ruid, p_svuid, p_rgid, and p_svgid to the
 * 	fields that are zeroed in the procdup() failure cleanup.
 * 	[90/09/24  16:34:30  coren]
 * 
 * Revision 1.11  90/08/24  11:16:15  devrcs
 * 	Changes for new syscall interface (gmf).
 * 	Changes for u_file_state (sue).
 * 	[90/08/17  17:36:34  nags]
 * 
 * 	Use consistent credentials macros.
 * 	Eliminate old, useless QUOTA code.
 * 	[90/08/18  23:58:06  nags]
 * 
 * 	Put usage of SKEEP proc flag under #if !MACH.
 * 	Removed no longer needed 4.3 compatibility code.
 * 	[90/08/12  12:06:17  ers]
 * 
 * Revision 1.10  90/07/17  11:18:35  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:51:42  seiden]
 * 
 * 	Fixed another bug in procdup() failure cleanup (setting p_stat
 * 	back to NULL).
 * 	[90/07/10  14:51:44  coren]
 * 
 * Revision 1.9  90/07/05  23:07:34  devrcs
 * 	Fixed small bug in procdup() failure cleanup.
 * 	[90/06/28  12:37:28  coren]
 * 
 * 	Recover and clean up if procdup() fails.
 * 	[90/06/25  14:25:23  coren]
 * 
 * Revision 1.8  90/06/22  20:05:52  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing	seiden@osf.org
 * 	Removed p_ttyp, p_ttyd refs			ers@osf.org
 * 	Propagate tty compatability flag across forks	ers@osf.org
 * 	File layer locks, uarea initializations.	noemi@osf.org
 * 	Change to use 4.4BSD pgrp structure.		coren@osf.org
 * 	Removed include of <sys/dir.h>			robert@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Fix includes, syscontext in user.h.		gmf@osf.org
 * 	Merge 2.5 and Encore parallelization.		alan@encore.com
 * 	Merge with preliminary Release 2.5.		alan@encore.com
 * 	MMAX_MP: Added lock around system time.		boykin@encore.com
 * 	Removed include of psl.h for mips too.		af@cmu.edu
 * 	Use new inode macros for manipulating inodes.	gm0w@cmu.edu
 * 	Save pointer to utask structure in proc struct.	mrt@cmu.edu
 * 	More cleanup.					rpd@cmu.edu
 * 	Changes for cleanup.				gm0w@cmu.edu
 * 	Code cleanup cataclysm.				mwyoung@cmu.edu
 * 	Vnode support.					jsb@cmu.edu
 * 	Corrected include file references.		mwyoung@cmu.edu
 * 	MMAX_MP:  lock file structures, inodes.		alan@encore.com
 * 	Set p_stat to SRUN before resuming child.	dlb@cmu.edu
 * 	Remove references to multprog.			mwyoung@cmu.edu
 * 	Newproc must copy controlling tty fields.	mja@cmu.edu
 * 	Delinted.					dbg@cmu.edu
 * 	Deleted #ifdef ibmrt call to float_fork().	sanzi@cmu.edu
 * 	Cleaned up conditionals.  Deleted old history.	avie@cmu.edu
 * 	MACH_VFS: Convert from inodes to vnodes.	king@next.com
 * 	CMUCS:  don't copy p_logdev; added u_maxuprc.	mja@cmu.edu
 * 	MACH:  restored include of pte.h for !MACH.	mja@cmu.edu
 * 	VICE:  added hooks for ITC/Andrew remote fs.	jjk@cmu.edu
 * 	CS_RFS:  split RFS hooks into two calls.	mja@cmu.edu
 * 	CS_RFS:  hook from newproc into RFS.		mja@cmu.edu
 * 	Upgraded to 4.2BSD.				gm0w@cmu.edu
 * 	CMUCS:  incr/decr macros for inode ref count.	mja@cmu.edu
 * 	[90/06/12  19:05:51  gmf]
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
 *	@(#)kern_fork.c	7.1 (Berkeley) 6/5/86
 */

#ifdef	OSF1_SERVER
#include <map_uarea.h>
#include <mach_nbc.h>
#include <remote_proc.h>
#endif	/* OSF1_SERVER */
 
#include <sys/secdefines.h>
#include <cputypes.h>

#include <machine/reg.h>
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
#include <sys/vnode.h>
#include <ufs/inode.h>
#include <sys/file.h>
#include <sys/acct.h>

#ifndef	OSF1_SERVER
#include <sys/vm.h>
#include <kern/thread.h>
#endif	/* OSF1_SERVER */
#include <kern/parallel.h>
#if	UNIX_LOCKS
#include <sys/lock_types.h>
#endif
#if     SEC_BASE
#include <sys/security.h>
#endif

#include "profiling.h"

#ifdef	OSF1_SERVER
#if	MAP_UAREA
#include <machine/vmparam.h>
#endif	MAP_UAREA
#endif	/* OSF1_SERVER */

#ifdef PARACORE
#include <paracore/core_types.h>
#endif /* PARACORE */

#undef	MAXUPRC
#define MAXUPRC		u.u_maxuprc

#ifdef	OSF1_SERVER
struct proc *newproc();
struct proc *newproc_body();
int enable_emulator_debugging = 0;
extern int io_pages_nb, rw_pages_nb, ro_pages_nb;
extern char *server_build;
#if GPROF
extern int emulator_profiling;	/* 0 if profiling enabled. */
#endif
#else	/* OSF1_SERVER */
thread_t	newproc(), procdup();
#endif	/* OSF1_SERVER */

#ifdef	OSF1_SERVER
#define THREAD_NULL	((thread_t) 0)
#endif	/* OSF1_SERVER */
	
#if	REMOTE_PROC
#include <uxkern/rproc.h>
struct	mutex			rproc_list_lock;
/*
 * The two macros below are must be called while holding rproc_list_lock
 */
#define VALID_RPROC_NODE(node) ((node <= max_valid_nodes) && (isset(valid_node_list,node)))

#define INSERT_RPROC_NODE(node) insert_rproc_node(node)
#define	NODE_LIST_CHUNK		64

char	*malloc();
char	*valid_node_list;
int	max_valid_nodes;
zone_t	rproc_list_zone;
struct 	rproc_list_entry	*rproc_list_ptr;
struct	mutex			rproc_list_lock;

void
procman_setup()
{
	struct rproc_list_entry	*rep;
	extern node_t root_fs_node;

	/* initialize rproc list and mutex */
	mutex_init(&rproc_list_lock);
	rproc_list_zone = zinit(sizeof(struct rproc_list_entry),
                                vm_page_size, 0, "rproc_list_entry");
	ZALLOC(rproc_list_zone, rep, struct rproc_list_entry *);
	if (rep == NULL)
		panic("procman_setup: No more entries\n");

	ASSERT(this_node == root_fs_node);

	/* Initialize to just root node */
	rep->node = this_node;
	rep->next = rep;
	rproc_list_ptr = rep;
	valid_node_list = malloc(NODE_LIST_CHUNK);
	if (valid_node_list == (char *) NULL) {
		panic("procman_setup: malloc failed");
	}
	max_valid_nodes = NODE_LIST_CHUNK * NBBY;
	INSERT_RPROC_NODE(this_node);
}

node_t
get_rproc_node()
{
	node_t	node;

	mutex_lock(&rproc_list_lock);
	rproc_list_ptr = rproc_list_ptr->next;
	node = rproc_list_ptr->node;
	mutex_unlock(&rproc_list_lock);
	return(node);
}

void
rproc_server_register(node)
	node_t	node;
{
	struct	rproc_list_entry        *rep;

	ZALLOC(rproc_list_zone, rep, struct rproc_list_entry *);
        if (rep == NULL)
                panic("rproc_server_register: No more entries\n");
	
	/* should check that we aren't registering an already registered node */
	mutex_lock(&rproc_list_lock);
	rep->node = node;
	rep->next = rproc_list_ptr->next;
	rproc_list_ptr->next = rep;
	INSERT_RPROC_NODE(node);
	mutex_unlock(&rproc_list_lock);
	printf("Node %d registering for remote process creation.\n", node);
}

insert_rproc_node(node)
{
	int	node_list_size;

	if (node > max_valid_nodes ) { 
		free(valid_node_list);
		printf("max_valid_nodes was %d\n", max_valid_nodes);
		
		node_list_size = (((max_valid_nodes + 1) / 
					(NODE_LIST_CHUNK * NBBY)) + 1 ) * 
					NODE_LIST_CHUNK;
		max_valid_nodes = node_list_size * NBBY;
		printf("max_valid_nodes is %d\n", max_valid_nodes);
		valid_node_list = malloc(node_list_size);
		if (valid_node_list == (char *) NULL) {
			panic("inset_rproc_node: malloc failed");
		}
		bzero(valid_node_list, node_list_size);
	}
	setbit(valid_node_list, node);
}
#endif	/* REMOTE_PROC */

int
pproc_fork(pp, pid, pchild, node, new_state, new_state_count)
	struct proc *pp;
	pid_t pid;
	struct proc **pchild;
	node_t		node;
	thread_state_t	new_state;
	unsigned int	new_state_count;
{
	register struct proc *pc;
	register a;
	thread_t	th = THREAD_NULL;
#ifndef	OSF1_SERVER
	int		s;
#endif	/* OSF1_SERVER */
	register uid_t	puid;

	a = 0;

	unix_master();

	PROC_LOCK(pp);
	puid = pp->p_rcred->cr_uid;
	PROC_UNLOCK(pp);
#if     SEC_BASE
        /*
         * Check if process is allowed to exceed per-user proc limit or to
         * take last proc slot.  If so, don't bother to scan the proc table.
         * The second argument of -1 prevents this call from counting as a
         * use of privilege for purposes of auditing.  Use of the privilege
         * will be recorded by the second call below if it is actually needed.
         */
	if (!privileged(SEC_LIMIT, -1))
#else
	if (puid != 0)
#endif
	{
		for (pc = allproc; pc; pc = pc->p_nxt) {
			PROC_LOCK(pc);
			if (pc->p_rcred->cr_uid == puid)
				a++;
			PROC_UNLOCK(pc);
		}
		for (pc = zombproc; pc; pc = pc->p_nxt) {
			PROC_LOCK(pc);
			if (pc->p_rcred->cr_uid == puid)
				a++;
			PROC_UNLOCK(pc);
		}
	}
	/*
	 * Disallow if
	 *  No processes at all;
	 *  not su and too many procs owned; or
	 *  not su and would take last slot.
	 */
	pc = freeproc;
	if (pc==NULL)
		tablefull("proc");
#if     SEC_PRIV
	if (pc == NULL || (!privileged(SEC_LIMIT, 0) && (pc->p_nxt == NULL ||
	    a > MAXUPRC)))
#else
	if (pc == NULL || (puid != 0 && (pc->p_nxt == NULL || a > MAXUPRC)))
#endif
	{
/*		retval[1] = 0; */
		unix_release();
		return (EAGAIN);
	}

#ifdef	OSF1_SERVER		/* This code comes from bsd/mach_fork.c */

	/* 
	 * newproc_body takes an extra node number in ad, and
	 * returns a proc structure.. 
	 */
	pc = newproc_body(pid, FALSE, node);
	if (pc == 0) {
/*		retval[1] = 0; */
		unix_release();
		return (EAGAIN);
	}

	/*
	 * Allocate a credentials port for the child, now that the 
	 * proc is fully initialized.
	 */
	pc->p_cred = credentials_allocate(pc->p_pid, pc->p_pgid, pc->p_sid, 
			pc->p_rcred, pc->p_utask.uu_cmask,
			pc->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
			pc->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
			pc->p_ruid, pc->p_rgid,
			pc->p_task);

	th = pc->p_thread;	/* ... since in this context, th is a port !!*/
	/*
	 * Clone the parent's registers, but mark it as the child
	 * process.
	 */
	if (!thread_dup(th, new_state, new_state_count, pp->p_pid, 1)) {
/*		retval[1] = 0; */
		unix_release();
		return (EFAULT);
	}
	/*
	 * Record its start time:
	 */
	TIME_READ_LOCK();
	pc->p_utask.uu_start = time;
	TIME_READ_UNLOCK();
	pc->p_utask.uu_acflag.fi_flag = AFORK;

#else	/* OSF1_SERVER */

	th = newproc(isvfork);
	if (th == THREAD_NULL) {
/*		retval[1] = 0; */
		unix_release();
		return (EAGAIN);
	}
	thread_dup(current_thread(), th);
	s = splhigh();
	TIME_READ_LOCK();
	th->u_address.utask->uu_start = time;
	TIME_READ_UNLOCK();
	splx(s);
	th->u_address.utask->uu_acflag.fi_flag = AFORK;
	ASSERT(th->u_address.uthread->uu_nd.ni_cred != NOCRED);

#endif	/* OSF1_SERVER */

	*pchild = pc;

	unix_release();
	return(0);		/* thread_resume() done in emulator */
}

/*
 * Create a new process-- the internal version of
 * sys fork.
 * It returns 1 in the new process, 0 in the old.
 */
#ifdef	OSF1_SERVER
struct proc *
newproc(mpid, is_sys_proc)
	pid_t		mpid;
	boolean_t	is_sys_proc;
{
	return(newproc_body(mpid, is_sys_proc, NONODE));
}

struct proc *
newproc_body(mpid, is_sys_proc, node)
	pid_t		mpid;
	boolean_t	is_sys_proc;
	node_t		node;
#else	/* OSF1_SERVER */
thread_t
newproc(isvfork)
	int isvfork;
#endif	/* OSF1_SERVER */
{
	register struct proc *rpp, *rip;
	register int n;
#ifndef	OSF1_SERVER
	register struct file *fp;
	thread_t	th;
#else	/* OSF1_SERVER */
	boolean_t	is_superuser = FALSE;
	if (suser(u.u_cred,&u.u_acflag) == 0) {
		is_superuser = TRUE;
	}
#endif	/* OSF1_SERVER */
	/*
	 * First, just locate a slot for a process
	 * and copy the useful info from this process into it.
	 * The panic "cannot happen" because fork has already
	 * checked for the existence of a slot.
	 */
	
	if ((rpp = freeproc) == NULL)
		panic("no procs");

	freeproc = rpp->p_nxt;			/* off freeproc */

	/*
	 * Make a proc table entry for the new process.
	 */
	rip = u.u_procp;

#ifdef	OSF1_SERVER
	if (rpp->p_ref_count != 0)
		panic("newproc: 0x%x", rpp);

	simple_lock_init(&rpp->p_lock);
	rpp->p_ref_count = 1;
	queue_init(&rpp->p_servers);
	rpp->p_side_ref_count = 0;
	rpp->p_issig_flag = 0;
	rpp->p_exit_hold_count = 0;
#endif	/* OSF1_SERVER */
	rpp->p_stat = SIDL;

	/*
	 * Do not need to hold PROC_TIMER_LOCK for the child because no
	 * other threads know about the child yet.
	 */
	timerclear(&rpp->p_realtimer.it_value);
#ifndef	OSF1_SERVER
	rpp->p_flag = SLOAD | (rip->p_flag & (SPAGV|SOUSIG|SXONLY|SCTTY));
#endif	/* OSF1_SERVER */
	rpp->p_ndx = rpp - proc;
	PROC_LOCK(rip);
	rpp->p_ruid = rip->p_ruid;
	rpp->p_rgid = rip->p_rgid;
	rpp->p_svuid = rip->p_svuid;
	rpp->p_svgid = rip->p_svgid;
	PROC_UNLOCK(rip);
	rpp->p_pgid = rip->p_pgid;
	rpp->p_sid = rip->p_sid;
	rpp->p_nice = rip->p_nice;
#ifndef	OSF1_SERVER
	rpp->p_pid = mpid;		/* shared */
	rpp->p_ppid = rip->p_pid;	/* shared */
#endif	/* OSF1_SERVER */
#ifndef	OSF1_SERVER
	rpp->p_time = 0;
	rpp->p_cpu = 0;
	rpp->p_sigmask = rip->p_sigmask; /* shared */
	rpp->p_sigcatch = rip->p_sigcatch; /* shared */
	rpp->p_sigignore = rip->p_sigignore; /* shared */
	/* take along any pending signals like stops? */
	rpp->p_rssize = 0;
	rpp->p_maxrss = rip->p_maxrss;
	rpp->p_slptime = 0;
	rpp->p_pctcpu = 0;
	rpp->p_cpticks = 0;
#endif	/* OSF1_SERVER */
	rpp->p_logdev = NODEV;

	/*
	 * Emulator callback thread & port
	 */
	rpp->p_callback_thread = MACH_PORT_NULL;
	rpp->p_callback = MACH_PORT_NULL;

#ifdef	balance
	save_fpu();	       /* inherit FPU context across fork() */
#endif
	n = PIDHASH(mpid);     /* was rpp->p_pid but this field is now shared */
	rpp->p_idhash = pidhash[n];
	pidhash[n] = rpp - proc;

	/*
	 * increment reference counts on shared objects
	 */
#ifdef OSF1_ADFS
	/*
	 * Grab u_handy_lock to ensure that the current or root dir
	 * don't change until we've decremented the ref count on the
	 * parent, below. Otherwise, we could increment the ref count
	 * on one vnode and decrement on another.
	 */
	U_HANDY_LOCK();
	remote_vref(&u.u_cdirproxy);
	remote_vref(&u.u_rdirproxy);
#ifdef PARACORE
	remote_vref(&u.uu_procp->p_utask.uu_exec_utnd.utnd_cdir);
	remote_vref(&u.uu_procp->p_utask.uu_exec_utnd.utnd_rdir);
#endif /* PARACORE */	
#else
	if (u.u_cdir)
		VREF(u.u_cdir);
	if (u.u_rdir)
		VREF(u.u_rdir);
#endif	/* OSF1_ADFS */

	PROC_LOCK(rip);
	crhold(rip->p_rcred);
	rpp->p_rcred = rip->p_rcred;
	PROC_UNLOCK(rip);

	/*
	 * This begins the section where we must prevent the parent
	 * from being swapped.
	 */
#ifndef	OSF1_SERVER
#if	!MACH
	rip->p_flag |= SKEEP;		/* shared */
#endif
#endif	/* OSF1_SERVER */
	rpp->sigwait = FALSE;
	rpp->exit_thread = THREAD_NULL;
#ifdef	OSF1_SERVER
	if (!procdup(rpp, rip, node, is_superuser)) {	/* child, parent */
#else	/* OSF1_SERVER */
	th = procdup(rpp, rip);		/* child, parent */
	if (th == THREAD_NULL) {
#endif	/* OSF1_SERVER */
		int i, x;
		/*
		 * couldn't make new task/thread, must undo what we've done
		 */
		PROC_LOCK(rip);
		crfree(rip->p_rcred);
		PROC_UNLOCK(rip);
#ifdef	OSF1_ADFS
		remote_vrele(&u.u_cdirproxy);
		remote_vrele(&u.u_rdirproxy);
#ifdef PARACORE
		remote_vrele(&u.uu_procp->p_utask.uu_exec_utnd.utnd_cdir);
		remote_vrele(&u.uu_procp->p_utask.uu_exec_utnd.utnd_rdir);
#endif /* PARACORE */	
		U_HANDY_UNLOCK();
#else
		if (u.u_cdir)
			VUNREF(u.u_cdir);
		if (u.u_rdir)
			VUNREF(u.u_rdir);
#endif	/* OSF1_ADFS */
		x = rpp - proc;
		i = PIDHASH(mpid);			/* was rpp->p_pid */

		if (pidhash[i] == x)
			pidhash[i] = rpp->p_idhash;
		else {
			for (i = pidhash[i]; i != 0; i = proc[i].p_idhash)
				if (proc[i].p_idhash == x) {
					proc[i].p_idhash = rpp->p_idhash;
					goto done;
				}
			panic("newproc pid hash removal");
		}
	done:
		rpp->p_idhash = 0;	/* also need to do this in exit() */

		/*
		 * clear out a few things
		 */
#ifndef	OSF1_SERVER			/* Those fields weren't initialized */
		rpp->p_pid = 0;
		rpp->p_ppid = 0;
#endif	/* OSF1_SERVER */
		rpp->p_ruid = 0;
		rpp->p_rgid = 0;
		rpp->p_svuid = 0;
		rpp->p_svgid = 0;
#ifndef	OSF1_SERVER
		rpp->p_flag = 0;
		rpp->p_sig = 0;
		rpp->p_cursig = 0;
#endif	/* OSF1_SERVER */
		rpp->p_sigcatch = 0;
		rpp->p_sigignore = 0;
		rpp->p_sigmask = 0;
		rpp->p_pgid = 0;
		rpp->p_sid = 0;
                rpp->p_stat = NULL;
#ifdef	OSF1_SERVER
		rpp->p_ref_count = 0;		/* Was set to 1 above. */
#endif	/* OSF1_SERVER */
		rpp->p_nxt = freeproc;		/* onto freeproc */
		freeproc = rpp;
#ifndef	OSF1_SERVER
		(void) spl0();
#if	!MACH
		rip->p_flag &= ~SKEEP;
#endif
		return(th);
		}
#else	/* OSF1_SERVER */
		return((struct proc *) NULL);
#endif	/* OSF1_SERVER */
	}
#if     SEC_BASE
	secinfo_dup(rip, rpp);
#endif

#ifdef	OSF1_SERVER
	/* Those fields may only be initialized after the shared memory
	 * has been set for the child process (in procdup)
	 */
	rpp->p_flag = SLOAD | (rip->p_flag & (SPAGV|SOUSIG|SXONLY|SCTTY));
	if (is_sys_proc)
	    rpp->p_flag |= SSYS;		/* shared */
	rpp->p_pid = mpid;			/* shared */
	rpp->p_ppid = rip->p_pid;		/* shared */
	rpp->p_sigmask = rip->p_sigmask; 	/* shared */
	rpp->p_sigcatch = rip->p_sigcatch; 	/* shared */
	rpp->p_sigignore = rip->p_sigignore; 	/* shared */
	rpp->p_sig = 0;				/* shared */
	rpp->p_cursig = 0;			/* shared */
	rpp->p_stopsig = 0;
#if	I_DONT_KNOW
	rpp->p_stopcode = 0;
	rpp->p_stop_clear_port = MACH_PORT_NULL;
#endif
#if	MAP_UAREA
	rpp->p_shared_rw->us_in_emulator = 0;
	share_lock_init(&rpp->p_siglock); 	/* shared */
#else	MAP_UAREA
	simple_lock_init(&rpp->p_siglock);
#endif	MAP_UAREA
#if PROFILING
#if GPROF
	if (emulator_profiling == 0) {	/* We are doing emulator profiling. */
		rpp->p_profport = pport_to_proc_enter(rpp);
		task_sample((mach_port_t) rpp->p_task, rpp->p_profport);
		rpp->p_taskprofed = 1;
	} else
#endif
	{
		rpp->p_profport = MACH_PORT_NULL;
		rpp->p_taskprofed = 0;
	}
#endif

	/*
	 * Copy task u-area from parent and zero the timing fields.
	 * Must not be blocked between incrementing file reference
	 * counts (above) and copying u-area; otherwise, another
	 * thread could open a file while we were blocked, leaving
	 * an entry in the file table that only has one reference
	 * but will be copied to the new task.
	 */
	rpp->p_utask = rip->p_utask;
	bzero((caddr_t) &rpp->p_utask.uu_ru,  sizeof(struct rusage));
	bzero((caddr_t) &rpp->p_utask.uu_cru, sizeof(struct rusage));

#ifdef	TNC
	/*
	 * Also clear System V IPC port flag -- child does not get parent's.
	 */
	rpp->p_utask.uu_svipc_flag = 0;
#endif	/* TNC */

#ifdef	OSF1_ADFS

#ifdef PARACORE
	
	/*
	 * Also clear uu_core_action, uu_core_directory and uu_root_directory.
	 */
	rpp->p_utask.uu_core_directory = MACH_PORT_NULL;
	rpp->p_utask.uu_root_directory = MACH_PORT_NULL;
	rpp->p_utask.uu_core_action = CORE_ACTION_CONT;
	rpp->p_utask.uu_ports_copied = FALSE;
	
	/*
	 * Initialize the exec vnode proxies of the child and transfer
	 * the references from the parent to the child.
	 */
	vnode_proxy_init(&rpp->p_utask.uu_exec_utnd.utnd_cdir);
	vnode_proxy_init(&rpp->p_utask.uu_exec_utnd.utnd_rdir);
	remote_vrele(&u.uu_procp->p_utask.uu_exec_utnd.utnd_cdir);
	remote_vrele(&u.uu_procp->p_utask.uu_exec_utnd.utnd_rdir);
	
	/*
	 * Also initialize uu_faulting_thread.
	 */
	rpp->p_utask.uu_faulting_thread = rpp->p_thread;

#endif /* PARACORE */

	/*
	 * Initialize the vnode proxies of the child and transfer
	 * the references from the parent to the child.
	 */
	vnode_proxy_init(&rpp->p_utask.uu_cdirproxy);
	vnode_proxy_init(&rpp->p_utask.uu_rdirproxy);
	remote_vrele(&u.u_cdirproxy);
	remote_vrele(&u.u_rdirproxy);
	U_HANDY_UNLOCK();
	/*
	 * Initialize the lock in the child because the u-area was copied
	 * with the lock held (it would remain locked).
	 */
	U_HANDY_LOCK_INIT(&rpp->p_utask);
#endif	/* OSF1_ADFS */

	/*
	 * and fix uu_procp
	 */
	rpp->p_utask.uu_procp = rpp;


	uarea_lock_init(&rpp->p_utask);

	/*
	 * Copy the keep on exec information
	 */
	vm_fork(rip, rpp);

#if PROFILING
	/* Process profiling status is disabled (default) */
	rpp->p_utask.uu_prof_on = FALSE;
#endif

#else	/* OSF1_SERVER */
	uarea_lock_init(th->u_address.utask);
	ASSERT(th->u_address.uthread->uu_nd.ni_cred != NOCRED);
	rpp->utask = th->u_address.utask;
#endif	/* OSF1_SERVER */

	/*
	 *	It is now safe to link onto allproc
	 */
	rpp->p_nxt = allproc;			/* onto allproc */
	rpp->p_nxt->p_prev = &rpp->p_nxt;	/*   (allproc is never NULL) */
	rpp->p_prev = &allproc;
	allproc = rpp;
	rpp->p_stat = SRUN;			/* XXX */
#ifndef	OSF1_SERVER
	(void) spl0();
#endif	/* OSF1_SERVER */

	/*
	 * Cause child to take a non-local goto as soon as it runs.
	 * On older systems this was done with SSWAP bit in proc
	 * table; on VAX we use u.u_pcb.pcb_sswap so don't need
	 * to do rpp->p_flag |= SSWAP.  Actually do nothing here.
	 */
	/* rpp->p_flag |= SSWAP; */

	/*
	 * Now can be swapped.
	 */
#ifndef	OSF1_SERVER
#if	!MACH
	rip->p_flag &= ~SKEEP;
#endif
	return(th);
#else	/* OSF1_SERVER */
	return(rpp);
#endif	/* OSF1_SERVER */
}


/*
 * We need to create the task now so we have some
 * place to create the shared region. Note that we give
 * an extra send right to the task and the thread so
 * that these rights can be given back to the parent
 * process.
 */
#if	REMOTE_PROC
int rproc_debug = 0;
#endif

#ifdef	OSF1_SERVER		/* take node argument */

procdup(rpp, rip, node, is_superuser)		/* child, parent */
	node_t		node;
	boolean_t	is_superuser;

#else 	/* OSF1_SERVER */

procdup(rpp, rip)		/* child, parent */

#endif 	/* OSF1_SERVER */

	struct proc *rpp, *rip;
{

	kern_return_t	result;
	mach_port_t	new_req_port;
	extern mach_port_t	ux_notify_port;
	mach_port_t	previous;

#ifdef	TNC
	{
		extern node_t this_node;

		if (u.uu_fork_task == MACH_PORT_NULL) {
		    ux_server_thread_blocking();
		    result = norma_task_create(rip->p_task, TRUE, this_node,
					       &rpp->p_task);
		    ux_server_thread_unblocking();
		} else {
			rpp->p_task = u.uu_fork_task;
			result = KERN_SUCCESS;
		}
	}
#elif	REMOTE_PROC
	{
		int	pm_fork_node;

		/*
		 * simple round robin - no frills
		 */
		if (node == NONODE) {
			pm_fork_node = get_rproc_node();
		} else {
			if (is_superuser == FALSE) {
				/* 
				 * Non super-user rproc is only allowed 
				 * on nodes that have registered
				 */
				mutex_lock(&rproc_list_lock);
				if (VALID_RPROC_NODE(node)) {
					mutex_unlock(&rproc_list_lock);
					pm_fork_node = node;
				} else {
					mutex_unlock(&rproc_list_lock);
					return(FALSE);
				}
			} else {
				pm_fork_node = node;
			}
		}
		if (rproc_debug)
			printf ("create task on node %d\n", pm_fork_node);
		ux_server_thread_blocking();
		result = norma_task_create(rip->p_task, TRUE, pm_fork_node++, 
					   &rpp->p_task);
		ux_server_thread_unblocking();
        }
#else	/* REMOTE_PROC && TNC */
	result = task_create(rip->p_task, TRUE, &rpp->p_task);
#endif	/* REMOTE_PROC && TNC */

	if (result != KERN_SUCCESS) {
		printf("procdup: task create failure %d\n", result);
		return(FALSE);
	}
	result = mach_port_mod_refs(mach_task_self(), rpp->p_task, 
			MACH_PORT_RIGHT_SEND, 1);
	if (result != KERN_SUCCESS) {
		printf("procdup: modify task reference failure %d\n", result);
		return(FALSE);
	}
	result = thread_create(rpp->p_task, &rpp->p_thread);
	if (result != KERN_SUCCESS) {
		(void) task_terminate(rpp->p_task);
		(void) mach_port_deallocate(mach_task_self(), rpp->p_task);
		printf("procdup: thread create failure %d\n", result);
		return(FALSE);
	}
	result = mach_port_mod_refs(mach_task_self(), rpp->p_thread, 
			MACH_PORT_RIGHT_SEND, 1);
	if (result != KERN_SUCCESS) {
		printf("procdup: modify thread reference failure %d\n", result);
		return(FALSE);
	}
	new_req_port = task_to_proc_enter(rpp->p_task, rpp);

	/*
	 * Insert the BSD request port for the task as
	 * its bootstrap port.
	 */
	result = task_set_bootstrap_port(rpp->p_task,
					 new_req_port);
	if (result != KERN_SUCCESS) {
		printf("procdup: can't set bootstrap port %d\n", result);
		return(FALSE);
	}

	/*
	 * Request a dead-name notification.
	 */
	result = mach_port_request_notification(mach_task_self(),
						rpp->p_task,
						MACH_NOTIFY_DEAD_NAME,
						1,
						ux_notify_port,
						MACH_MSG_TYPE_MAKE_SEND_ONCE,
						&previous);
	if (result != KERN_SUCCESS) {
		printf("procdup: mach_port_request_notification 0x%x\n",
			result);
	}
	(void) mach_port_deallocate(mach_task_self(), previous);

#if	MAP_UAREA
	if ((result = mapin_user(rpp)) != KERN_SUCCESS) {
		printf("procdup: mapin_user %d\n", result);
		return(FALSE);
	}
	bcopy(rip->p_shared_ro,rpp->p_shared_ro,sizeof(struct ushared_ro));
	bcopy(rip->p_shared_rw,rpp->p_shared_rw,sizeof(struct ushared_rw));
	rpp->p_shared_rw->us_inuse = 0;
	rpp->p_shared_rw->us_debug = enable_emulator_debugging ;
	share_lock_init(&rpp->p_shared_rw->us_lock);
	rpp->p_shared_ro->us_version = USHARED_VERSION;
	(void) strncpy(rpp->p_shared_ro->us_server_build, 
		       server_build, 
		       sizeof(rpp->p_shared_ro->us_server_build));
	rpp->p_shared_ro->us_server_build[sizeof(rpp->p_shared_ro->us_server_build) - 1] = '\0';
	rpp->p_shared_ro->us_io_pages_nb = io_pages_nb;
	rpp->p_shared_ro->us_rw_size = sizeof (struct ushared_rw);
	rpp->p_shared_ro->us_ro_size = sizeof (struct ushared_ro);
	rpp->p_shared_ro->us_nofile = NOFILE;
	rpp->p_shared_ro->us_proc_pointer = (int)rpp;
	bcopy(u.u_rlimit, rpp->p_shared_ro->us_rlimit, sizeof(u.u_rlimit));
	rpp->p_shared_rw->us_sigstack = rip->p_shared_rw->us_sigstack;
#endif	MAP_UAREA

	return(TRUE);
}

#ifdef	OSF1_SERVER

#if	MAP_UAREA
kern_return_t
mapin_user(p)
	register struct proc *p;
{
	vm_address_t	user_addr;
	kern_return_t	result;
	extern mach_port_t	shared_memory_port;
	
	user_addr = EMULATOR_END - ro_pages_nb*vm_page_size;
	result = vm_map(p->p_task, &user_addr, ro_pages_nb*vm_page_size, 0,
			0, shared_memory_port,
			p->p_shared_off+(io_pages_nb+rw_pages_nb)*vm_page_size, 
			0, VM_PROT_READ, VM_PROT_READ, VM_INHERIT_NONE);
	if (result != KERN_SUCCESS)
		return result;
	user_addr -= (io_pages_nb + rw_pages_nb) * vm_page_size;
	result = vm_map(p->p_task, &user_addr,
			(io_pages_nb + rw_pages_nb) * vm_page_size, 0,
			0, shared_memory_port, p->p_shared_off,
			0, VM_PROT_READ|VM_PROT_WRITE,
			VM_PROT_READ|VM_PROT_WRITE, VM_INHERIT_NONE);
	return result;
}
#endif	MAP_UAREA

#else	/* OSF1_SERVER */

/*
 * Initialize per-thread uarea information.
 * There are two types of clients, those of thread_create() and
 * those of fork (newproc).
 *
 * NOTE:  there is a seemingly redundant call to this function in
 * 	  newproc() which is necessary because procdup() will undo
 *	  any work done here.
 */
uarea_init(th)
	register thread_t	th;
{
	register struct uthread *uthread = th->u_address.uthread;

	uthread->uu_nd.ni_iov = &uthread->uu_nd.ni_iovec;
	/*
	 * The nameidata structure in the thread must point into the user
	 * task structure so that current and root directories
	 * can be shared among all threads in a task.
	 * If we want to change this, much file system code must be changed
	 * to know where these entities are stored.  Currently, it assumes
	 * that it's in the nameidata struct.
	 */
	uthread->uu_nd.ni_utnd = &th->u_address.utask->uu_utnd;
	/*
	 * Initialize per-thread credentials.  This for two types of
	 * clients -- kernel threads (via thread_create()) and fork().
	 */
	uthread->uu_nd.ni_cred = NOCRED;
	cr_threadinit(th);
}

uarea_lock_init(utaskp)
	struct utask *utaskp;
{
#if	UNIX_LOCKS && MACH_SLOCKS
	/*
	 * The accounting flag should simply use the u-area handy lock.
	 */
	utaskp->uu_acflag.fi_lock = &utaskp->uu_handy_lock;
#endif
	U_HANDY_LOCK_INIT(utaskp);
	U_TIMER_LOCK_INIT(utaskp);
	UTND_LOCK_INIT(&utaskp->uu_utnd);
	U_MMAP_LOCK_INIT(utaskp);
	/*
	 * Init proc timer lock here rather than hack vm/vmunix_.c:procdup().
	 */
	PROC_TIMER_LOCK_INIT(utaskp->uu_procp);
}

uarea_zero(th)
	thread_t	th;
{
	bzero((caddr_t) th->u_address.uthread, sizeof(struct uthread));
}

#endif	/* OSF1_SERVER */

