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
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * HISTORY
 * $Log: proc.h,v $
 * Revision 1.16  1995/02/01  21:36:53  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.15  1995/01/27  22:43:36  yazz
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
 * Revision 1.14  1994/11/18  20:41:00  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/11/08  20:08:45  yazz
 *  Reviewer: Chris Peak, John Litvin
 *  Risk: Med
 *  Benefit or PTS #: 9853, 11537, 11538
 *  Testing: concur & sched EATs
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
 * Revision 1.12  1994/11/03  16:05:46  yazz
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
 * Abolish the short lived SBLOCKEMUL proc flags bit.
 *
 * Revision 1.11  1994/10/25  23:27:31  yazz
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
 * Revision 1.10  1993/07/16  17:05:15  cfj
 * Added #ifdef _KERNEL around p_utask field.
 *
 * Revision 1.9  1993/07/14  18:26:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:35:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.8  1993/06/08  21:30:17  nandy
 * Used the last available bit in p_flag. See kern_exit.c.
 *
 * Revision 1.7  1993/05/06  19:19:52  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.6  1993/04/03  03:07:45  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 * Revision 1.1.1.1  1993/05/03  17:42:07  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.1.2.1.2.2  1993/01/09  00:05:00  brad
 * Merged changes between ...Locus_Bug_Drop_OK... and Jan5 main trunk
 * tags into the PFS branch, to bring PFS up-to-date with Transmittal
 * 7.
 *
 * Revision 1.5  1992/12/17  19:45:10  cfj
 * Put #ifdef OSF1_ADFS around queue_head_t p_servers so that users
 * can include proc.h.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:01:06  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 * 
 * Revision 1.4  1992/12/14  15:56:19  cfj
 * Added some #ifdef so file can be used by commands and libs.
 *
 * Revision 1.3  1992/12/11  03:00:02  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:43:08  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:40:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.21  92/11/23  16:01:38  klh
 * 	Revision 2.14  92/11/17  19:52:28  loverso
 * 		Add SINSUSP to indicate a unix_task_suspend() in progress.
 * 		(loverso)
 * 
 * Revision 2.23  93/06/22  17:56:23  yazz
 * [ Bug #0251 ] Define a new proc struct flag bit, SWAKEUPDONE.  (roman)
 * 
 * Revision 2.22  93/04/24  18:48:17  klh
 * 	Revision 2.15  93/01/07  11:17:07  condict
 * 		Added p_taskprofed.
 * 		[1992/08/05  18:00:14  emcmanus]
 *
 * Revision 2.21  92/11/23  16:01:38  klh
 * 	Revision 2.14  92/11/17  19:52:28  loverso
 * 		Add SINSUSP to indicate a unix_task_suspend() in progress.
 * 		(loverso)
 * 
 * Revision 2.20  92/10/06  12:13:01  roman
 * Fix RCS comments.
 * 
 * Revision 2.19  92/10/05  13:48:12  klh
 * 	Revision 2.13  92/08/13  19:19:19  rabii
 * 		Add p_callback_thread port to identify callback thread
 * 		(so as to not make it deliver signals). (loverso)
 * 
 * Revision 2.18  92/07/10  08:54:45  chrisp
 * For MAP_UAREA, add new mutex for use with the signal lock shared between
 * 	server and emulator.
 * 
 * Revision 2.17  92/06/05  13:58:25  klh
 * 	Revision 2.12  92/05/31  18:58:51  loverso
 * 		Make sure you #include <remote_proc.h>.  (loverso)
 * 
 * 		Added the new field p_mynode indicating which node the process
 * 		is running on. (rabii)
 * 
 * 	Revision 2.11  92/03/09  12:18:24  durriya
 * 		[92/01/30  16:05:41  sp]
 * 		Clean up auxiliary vector code
 * 
 * 		[92/01/07  23:34:48  condict]
 * 		Remove ifdef OSF1_SERVER conditionals.
 * 
 * Revision 2.16  92/04/02  11:22:15  roman
 * Put p_ruid and p_rgid always into the proc structure, no matter what
 * 	the condition of MAP_UAREA.
 * 
 * Revision 2.15  92/03/27  11:17:04  roman
 * Add "#ifdef MAP_UAREA"'s for p_ruid and p_rgid to use shared uarea fields.
 * 
 * Revision 2.14  92/03/24  08:59:48  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.10  92/03/01  18:39:06  pjg
 * 	Rename signature port.  Add new proc flag SDUMMY to indicate a 
 * 	dummy proc (loverso).
 * 
 * Revision 2.9  92/02/11  22:06:04  pjg
 * 	Add field for kill3 argument to SIGMIGRATE (TNC only).
 * 	Change type of p_auxv_structs to match MiG assumptions (roman@locus).
 * 
 * Revision 2.8  92/01/17  17:23:12  roy
 * 	Add sig_port field in support of int. system calls (loverso).
 * 
 * Revision 2.7  92/01/05  20:08:47  roy
 * 	1991/10/14  19:34:11  noemi
 * 	Added magic number and sequence number to proc structure and defined
 * 	P_MAGIC.  No code uses these yet.
 * 
 * Revision 2.6  91/12/16  18:38:38  roy
 * 	91/12/13  13:03:13  sp
 * 	Removed some fields copied from the uthread structure (!) which
 * 	are never used.
 * 
 * 	91/11/26  15:33:51  sp
 * 	Upgrade to 1.0.3
 * 
 * Revision 2.5  91/10/14  13:08:16  sjs
 * 	91/10/04  16:41:27  sp
 * 	Add space to communicate the auxiliary vector information around
 * 
 * 	91/09/27  12:01:25  emcmanus
 * 	Added p_profport for profiling.
 * 
 * Revision 2.4  91/10/04  15:02:52  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  16:47:08  rabii
 * 	Merge of V2.0 and Locus (locus check-in by roman)
 * 	Added credentials port field to proc. Also made numerous changes
 * 	due to certain fields moving to vproc.h and pvproc.h.
 * 
 * Revision 2.2  91/08/31  14:09:54  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.7  91/08/09  12:19:53  barbou
 * Added the p_issig_count field to avoid running bsd_issig_psig() twice.
 * 
 * Revision 3.6  91/07/31  15:44:21  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.9.2  91/07/26  14:28:28  dlb
 *	Rewrite sig_lock_to_exit() to move task_halt() call there from exit(),
 *	and avoid deadlock with thread_terminate().
 *	[91/07/26  14:23:23  dlb]
 *
 * Revision 1.12.7.2  91/03/19  08:57:35  bilbo
 * 	Added p_realtimer_coe to remember that the p_realtimer must be cleared upon
 * 	exec.  Remove this when nano_timers move into the kernel.  Bug 0732.
 * 	[91/03/19  08:44:43  bilbo]
 * 
 * Revision 1.12  90/10/31  14:06:53  devrcs
 * 	changed the type of cxenix from int * to void *.
 * 	[90/10/09  13:07:27  swallace]
 * 
 * 	Added the field cxenix for Xenix compatibility.
 * 	[90/10/08  13:43:25  swallace]
 * 
 * Revision 1.11  90/10/07  14:52:53  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  11:43:14  gm]
 * 
 * Revision 1.10  90/09/23  16:00:11  devrcs
 * 	Removed unneeded quota fields.
 * 	[90/09/03  22:36:03  nags]
 * 
 * Revision 1.9  90/08/24  12:26:49  devrcs
 * 	Removed unused proc flags.
 * 	[90/08/12  12:14:34  ers]
 * 
 * Revision 1.8  90/06/22  20:53:35  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Added various locks for OSF/1.				nags@encore.com
 * 	No more p_ttyd, p_ttyp; add SASSIGN for 4.3 compat.	ers@osf.org
 * 	Move p_semundo to utask structure.			ers@osf.org
 * 	SYSV_IPC:  semaphore undo struct pointer for proc.	bet@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.			coren@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes.	ers@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Posix tty support.					morris@osf.org
 * 	Merged 4.4BSD and Mach changes.				gm@osf.org
 * 	MACH X115 Update.					gm@osf.org
 * 	Integration of early Mach 2.5 and Encore MP.		alan@encore.com
 * 	Pass thread to thread_should_halt().			dlb@cmu.edu
 * 	Added utask pointer to proc for user-level programs.	gm0w@cmu.edu
 * 	More cleanup.						rpd@cmu.edu
 * 	Made p_rmt_seg field in struct proc unconditional.	mrt@cmu.edu
 * 	Massive conditional hacking (MACH, CMUCS, VICE).	mrt@cmu.edu
 * 	Updated macro definitions to the new style.		rpd@cmu.edu
 * 	Adjusted include file references.			mwyoung@cmu.edu
 * 	Document use of p_stat for MACH.			dlb@cmu.edu
 * 	MACH: Removed unused variables (whichqs).		mwyoung@cmu.edu
 * 	Move controlling tty info from u-area to proc.		mja@cmu.edu
 * 	Delinted.						dbg@cmu.edu
 * 	Removed MACH_NOFLOAT.					dbg@cmu.edu
 * 	More thread termination checks in sig_lock.		dbg@cmu.edu
 * 	Sig_lock forced_exit case must call thread_halt_self.	dbg@cmu.edu
 * 	Added second argument to task_dowait.			dlb@cmu.edu
 * 	Eliminated MACH conditionals.				avie@cmu.edu
 * 
 * 	Other contributors:  sanzi@cmu.edu, jjc@cmu.edu, beck@sequent.com,
 * 	jjk@cmu.edu, bolosky@cmu.edu.
 * 	[90/06/12  21:39:01  gmf]
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
 * Copyright (c) 1982, 1986, 1989  Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	%W% (Berkeley) %G%
 */

#ifndef	_SYS_PROC_H_
#define _SYS_PROC_H_

#include <mach/boolean.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/auxv.h>

#ifdef	_KERNEL
#include <kern/lock.h>
#else	/* _KERNEL */
#ifndef	_KERN_LOCK_H_
#define _KERN_LOCK_H_
typedef int simple_lock_data_t;
#endif	/* _KERN_LOCK_H_ */
#endif	/* _KERNEL */

#ifndef _EMULATOR
#ifndef REMOTE_PROC
#include <remote_proc.h>
#endif
#endif

#ifdef	OSF1_ADFS
#define	P_MAGIC	0xae
#endif

/*
 * One structure allocated per active
 * process. It contains all data needed
 * about the process while the
 * process may be swapped out.
 * Other per process data (user.h)
 * is swapped with the process.
 */
struct	proc {
#ifdef	OSF1_ADFS
	u_char	p_magic;	/* magic # for proc structures */
	u_long	p_seqno;	/* sequence number */
#endif
	struct	proc *p_link;	/* linked list of running processes */
	struct	proc *p_rlink;
	struct	proc *p_nxt;	/* linked list of allocated proc slots */
	struct	proc **p_prev;		/* also zombies, and free proc's */
	char	p_stat;
#ifdef	ibmrt
	short	p_nice;		/* nice for cpu usage */
#else
	char	p_nice;		/* nice for cpu usage */
#endif
#if	MAP_UAREA
#	define	p_cursig	p_shared_ro->us_cursig
#	define 	p_sig		p_shared_rw->us_sig
#	define 	p_sigmask	p_shared_rw->us_sigmask
#	define	p_sigignore	p_shared_rw->us_sigignore
#	define 	p_sigcatch	p_shared_rw->us_sigcatch
#	define	p_flag		p_shared_ro->us_flag
#else	/* MAP_UAREA */
	char	p_cursig;
	int	p_sig;		/* signals pending to this process */
	int	p_sigmask;	/* current signal mask */
	int	p_sigignore;	/* signals being ignored */
	int	p_sigcatch;	/* signals being caught by user */
       	int	p_flag;
#endif	/* MAP_UAREA */
	mach_port_t p_cred;	/* credentials port for this process */

	mach_port_t p_callback_thread;	/* emulator callback thread port */
	mach_port_t p_callback;	/* emulator callback port */

	uid_t	p_ruid;		/* real user id */
	uid_t	p_svuid;	/* saved effective user id */
	gid_t	p_rgid;		/* real group id */
	gid_t	p_svgid;	/* saved effective group id */
	struct	ucred *p_rcred;	/* user credentials (uid, gid, etc) */
#if	REMOTE_PROC
	node_t	p_mynode;	/* node on which the process is running */
#endif	/* REMOTE_PROC */
#define p_uid	p_rcred->cr_uid
#define p_gid	p_rcred->cr_gid
#if	MAP_UAREA
#	define 	p_pid	p_shared_ro->us_pid
#	define	p_ppid	p_shared_ro->us_ppid
#else	/* MAP_UAREA */
	pid_t	p_pid;		/* unique process id */
	pid_t	p_ppid;		/* process id of parent */
#endif	/* MAP_UAREA */
	pid_t	p_pgid;		/* process group id */
	pid_t	p_sid;		/* session id */
	struct vproc *p_vproc;	/* back pointer to vproc */
	u_short	p_xstat;	/* exit information */
	short	p_realtimer_coe;/* clear p_realtimer on exec */
	struct	rusage *p_ru;	/* mbuf holding exit information */
	int	p_stopsig;	/* signal that stopped us. */
	short	p_ndx;		/* proc index for memall (because of vfork) */
	short	p_idhash;	/* hashed based on p_pid for kill+exit+... */
	struct	itimerval p_realtimer;
/* This should be #if PROFILING, but the emulator doesn't know about that. */
	mach_port_t	p_profport;	/* Server internal profiling port
					   for the process */
	int	p_taskprofed;	/* True if entire task profiled */
/* #endif PROFILING. */
	int	p_traceflag;	/* kernel trace points */
	struct	vnode *p_tracep;/* trace to vnode */
#if defined(tahoe)
	int	p_ckey;		/* code cache key */
	int	p_dkey;		/* data cache key */
#endif
	dev_t	    p_logdev;	/* logged-in controlling device */

	task_t		p_task;		/* corresponding task */
	thread_t	p_thread;	/* corresponding thread */
#ifdef _KERNEL
	struct utask	p_utask;	/* task U-area */
#endif /* _KERNEL */
#if	MAP_UAREA
#	define 		siglock		p_shared_rw->us_siglock
#	define 		p_siglock 	siglock
#else	/* MAP_UAREA */
	simple_lock_data_t siglock;	/* multiple thread signal lock */
#	define 		p_siglock	siglock
#endif	/* MAP_UAREA */
	boolean_t	sigwait;	/* indication to suspend */
	thread_t	exit_thread;
#ifdef	sun
	struct	proc *p_tptr;	/* pointer to process structure of tracer */
#endif
#ifdef i386
	void	*cxenix;	/* for Xenix compatibility */
#endif
#ifdef	_KERNEL
	udecl_simple_lock_data(,p_lock)		/* general proc lock */
	udecl_simple_lock_data(,p_timer_lock)	/* protects realtimer */
#endif
#if	MAP_UAREA
	vm_offset_t	p_shared_off;	/* offset in shared object */
	struct ushared_rw
			*p_shared_rw;	/* shared memory with emulator */
	struct ushared_ro
			*p_shared_ro;	/* shared memory with emulator */
	char *		p_readwrite;	/* shared I/O buffer with emulator */

	struct proc *	p_mulink;	/* for linking inode "IT" queue */
	struct mutex	p_shared_mutex;	/* mutex guarding shared locking */
#endif	/* MAP_UAREA */

	short		p_ref_count;	/* # svr thrds registered to proc */
	short		p_side_ref_count; /* # of above that are "side" thrds */
	queue_head_t	p_servers;	/* linked list of proc's svr thrds */
	char		p_issig_flag;	/* !=0 if issig_psig side thrd viable */
	short		p_exit_hold_count; /* pproc_exit() blocks while >0 */
	short		p_pproc_hold_count; /* proc struct not clr'd while >0 */

	/*
	 * This is needed to comunicate the area to store the auxiliary
	 * vector for exec.
	 */
	auxv_type_t	*p_auxv_structs;
	auxv_strings_t	p_auxv_strings;
	unsigned int	p_auxv_structs_count;
	unsigned int	p_auxv_strings_count;
#ifdef	TNC
	struct rexecve_save *p_rexecve_save;
	/*
	 * Kill3 argument.
	 */
	int		p_sig_arg;
#endif	/* TNC */
};

#ifdef	_KERNEL
#include <kern/macro_help.h>


/*
 * Multiprocessor exclusion to a proc data structure.
 * Procs are annoying but must be kept around for backwards
 * compatibility.
 */
#define	PROC_LOCK(p)		usimple_lock(&(p)->p_lock)
#define	PROC_UNLOCK(p)		usimple_unlock(&(p)->p_lock)
#define	PROC_LOCK_INIT(p)	usimple_lock_init(&(p)->p_lock)

/*
 * The proc timer lock must always be held at splhigh.  Furthermore, there
 * is a "natural" lock ordering between the proc timer lock and the time
 * lock:  always take the proc timer lock FIRST.  Otherwise, well, ....
 */
#define	PROC_TIMER_LOCK_INIT(p)	usimple_lock_init(&(p)->p_timer_lock)
#define	PROC_TIMER_LOCK(p)	usimple_lock(&(p)->p_timer_lock)
#define	PROC_TIMER_UNLOCK(p)	usimple_unlock(&(p)->p_timer_lock)

/*
 *	Signal lock has the following states and corresponding actions
 *	that the locker must take:
 *
 *	Locked (siglock) - simple lock acquires the lock when free.
 *	Unlocked (sigwait = 0 && exit_thread == 0)  simple lock.
 *	Waiting (sigwait != 0) - Drop siglock after acquiring it, and
 *		call thread_block().  Thread that set the lock to
 *		wait has done a task_suspend().
 *	Exiting (exit_thread != 0) - The thread in exit_thread is going to
 *		call exit().  If we're not that thread, permanently stop
 *		in favor of that thread.  If we're that thread, immediately
 *		bail out (no signal processing is permitted once we're
 *		committed to exit) and indicate that signals should not be
 *		processed.  If we have been asked to halt, bail out and
 *		indicate that signals should be processed (to clean up any
 *		saved state).
 *
 *	The logic for this is in the sig_lock_or_return macro.
 */

#if	MAP_UAREA
#define sig_lock_simple(p)	share_lock(&(p)->siglock, p)
#define sig_unlock(p)		share_unlock(&(p)->siglock, p)
#else	/* MAP_UAREA */
#define sig_lock_simple(p)	simple_lock(&(p)->siglock)
#define sig_unlock(p)		simple_unlock(&(p)->siglock)
#endif	/* MAP_UAREA */

/*
 *	Try to grab signal lock.  If we are already exiting,
 *	execute 'false_return'.  If some other thread is exiting,
 *	hold.  If we must halt, execute 'true_return'.
 */
#define sig_lock_or_return(p, false_return, true_return)	\
MACRO_BEGIN							\
	sig_lock_simple(p);					\
	while ((p)->p_stat == SSTOP || ((p)->p_flag & SWEXIT)) {\
	    sig_unlock(p);					\
	    if ((p)->p_flag & SWEXIT) {				\
		    /*						\
		     *	Already exiting - no signals.		\
		     */						\
		    false_return;				\
	    }							\
	    thread_block();					\
	    sig_lock_simple(p);					\
	}							\
MACRO_END

/*
 *	Try to grab signal lock.  Return to caller if
 *	we must halt or task is exiting.
 */
#define sig_lock(p)		sig_lock_or_return(p, return, return)

#define sig_lock_to_wait(p)			\
MACRO_BEGIN					\
	(p)->sigwait = TRUE; 			\
	sig_unlock(p);				\
MACRO_END

#define sig_wait_to_lock(p)			\
MACRO_BEGIN					\
	sig_lock_simple(p);	 		\
	(p)->sigwait = FALSE;			\
MACRO_END

/*
 *	sig_lock_to_exit() also shuts down all other threads except the
 *    current one.  There is no sig_exit_to_lock().  The signal lock is
 *    left in exit state and is cleaned up by exit().  If task_halt
 *    fails, then cooperate with any attempt to halt this thread.
 *    After this, the lock must be reacquired via a full sig_lock.
 */

#define sig_lock_to_exit(p)					\
MACRO_BEGIN							\
	(p)->exit_thread = current_thread();			\
	simple_unlock(&(p)->siglock);				\
	while (task_halt(current_task()) != KERN_SUCCESS) {	\
		simple_lock(&(p)->siglock);			\
		(p)->exit_thread = THREAD_NULL;			\
		simple_unlock(&(p)->siglock);			\
		while (thread_should_halt(current_thread())) {  \
			thread_halt_self();			\
		}						\
		sig_lock(p);					\
		(p)->exit_thread = current_thread();		\
		simple_unlock(&(p)->siglock);			\
	}							\
MACRO_END
#endif	/* _KERNEL */


#define PIDHSZ		64
#define PIDHASH(pid)	((pid) & (PIDHSZ - 1))

#ifdef	_KERNEL
extern pid_t	pidhash[PIDHSZ];
extern struct	proc *pfind();
extern struct	proc *proc, *procNPROC;	/* the proc table itself */
extern struct	proc *freeproc, *zombproc, *allproc;
			/* lists of procs in various states */
extern int	nproc;

#define NQS	32		/* 32 run queues */
extern struct	prochd {
	struct	proc *ph_link;	/* linked list of running processes */
	struct	proc *ph_rlink;
} qs[NQS];

#define SESS_LEADER(p)	((p)->p_pid == (p)->p_sid)
#define PCTCPU_SCALE	1000	/* scaling for p_pctcpu */
#endif	/* _KERNEL */

/* stat codes */
/*
 *	MACH uses only NULL, SRUN, SZOMB, and SSTOP.
 */
#define SSLEEP	1		/* awaiting an event */
#define SWAIT	2		/* (abandoned state) */
#define SRUN	3		/* running */
#define SIDL	4		/* intermediate state in process creation */
#define SZOMB	5		/* intermediate state in process termination */
#define SSTOP	6		/* process being traced */

/* flag codes */
#define SLOAD	0x00000001	/* in core */
#define SSYS	0x00000002	/* swapper or pager process */
#define STRC	0x00000010	/* process is being traced */
#define SWTED	0x00000020	/* another tracing flag */
#define SOMASK	0x00000200	/* restore old mask after taking signal */
#define SWEXIT	0x00000400	/* working on exiting */
#define SPHYSIO	0x00000800	/* doing physical i/o */
#define SVFORK	0x00001000	/* process resulted from vfork() */
#define SPAGV	0x00008000	/* init data space on demand, from vnode */
#define SSEQL	0x00010000	/* user warned of sequential vm behavior */
#define SUANOM	0x00020000	/* user warned of random vm behavior */
#define STIMO	0x00040000	/* timing out during sleep */
#define	SOUSIG	0x00100000	/* using old signal mechansim */
#define SOWEUPC	0x00200000	/* owe process an addupc() call at next ast */
#define SCTTY	0x00800000	/* has a controlling terminal */
#define SXONLY	0x02000000	/* process image read protected	*/
#define	SNOCLDSTOP \
		0x40000000	/* no SIGCHLD when children stop */
#define SEXEC	0x80000000	/* process called exec */

#define	SINSUSP	0x00400000	/* Waiting for emulator to suspend */
#define SDUMMY	0x08000000	/* OSF1/AD dummy process */
#define	SWAKEUPDONE \
		0x10000000	/* gate wait() system call */

#if MACH
#define SREAPED	0x00000004	/* reap done; zero out pproc struct & put it
				   on free list when ref cnt drops to 0 */
/* Available flags */
#else	/* !MACH */
#define SLOCK	0x00000004	/* process being swapped out */
#define SSWAP	0x00000008	/* save area flag */
#define SULOCK	0x00000040	/* user settable lock in core */
#define SPAGE	0x00000080	/* process in page wait state */
#define SKEEP	0x00000100	/* another flag to prevent swap out */
#define SVFDONE	0x00002000	/* another vfork flag */
#define SNOVM	0x00004000	/* no vm, parent in a vfork() */
#define SACTIVE	0x00080000	/* process is executing */
#define	SPTECHG	0x01000000	/* pte's for process have changed */
#define SIDLE	0x04000000	/* is an idle process */
#define SKTR	0x20000000	/* pass kernel tracing flags to children */
#endif

#endif	/* _SYS_PROC_H_ */
