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
 * $Log: rtask_cli_pproc.c,v $
 * Revision 1.20  1995/03/24  23:19:07  yazz
 *  Reviewer: Ray Anderson
 *  Risk: Lo
 *  Benefit or PTS #: #11960
 *  Testing: EATs controlc, os_interfaces, specific testcase to cause msg.
 *  Module(s): server/tnc/rtask_cli_pproc.c
 * Display a warning on the bootnode console for assertful servers only,
 * if a process calls rfork() or rforkmulti() while holding an mmap()'d
 * type of shared memory region.
 *
 * Revision 1.19  1995/02/17  18:39:12  toman
 * Updated CVS comments in previous revision to fix line-wrapping problem.
 *
 * Revision 1.18  1995/02/17  18:30:14  toman
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
 * Revision 1.17  1995/02/01  21:47:42  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.16  1995/01/27  22:44:43  yazz
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
 * Revision 1.15  1994/12/20  23:04:15  suri
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
 * Revision 1.14  1994/11/18  20:43:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/11/08  20:11:58  yazz
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
 * Revision 1.12  1994/11/03  16:11:58  yazz
 *  Reviewer: Chris Peak
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
 * Take master lock for the call to unix_task_suspend().
 *
 * Revision 1.11  1994/10/25  23:50:23  suri
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
 * Revision 1.10  1994/07/27  16:38:34  johannes
 * extensions to rfork_pproc_load_msg() and migrate_pproc_load_msg()
 * in order to get the exec path information from the utask fields
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
 * Revision 1.9  1994/06/18  21:51:49  chrisp
 * In routine rtask_pproc_remove(), add code to handle profiling and
 * to deallocate emulator callback port and callback thread port.
 *
 *  Reviewer: yazz@locus.com
 *  Risk: L
 *  Benefit or PTS #: 9920
 *  Testing: TNC functional tests.
 *  Module(s): rtask_cli_pproc.c
 *
 * Revision 1.8  1994/03/14  02:05:34  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.7  1993/07/14  18:33:27  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:45:51  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:22:59  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:45:47  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:08:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:02:17  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  03:01:47  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:47:43  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:31:26  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:45:52  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.27  93/06/19  15:25:23  yazz
 * [ ad1.04 merge ]
 * 	Respect new dynamically allocated vm_mmap region structures when
 * 	loading & unloading proc structure during remote tasking operations.
 * 
 * Revision 3.26  93/06/02  12:27:49  yazz
 * For Sys V IPC under TNC load svipc flag field in migrate and remote
 * exec operations.  Also add missing printf formatting to panic messages.
 * 
 * Revision 3.25  92/11/02  11:42:29  roman
 * Add code to allow credentials cache vector to expand past the default size;
 * 	new vector is vm_allocated if necessary.
 *
 * Revision 3.24  92/10/16  11:08:16  chrisp
 * [Bug #66] Introduce rtask_pproc_unquiesce() to undo rtask_pproc_quiesce()
 * 	- called in error cases before returning to the emulator.
 * 
 * Revision 3.23  92/10/08  11:30:54  roman
 * Add routine rtask_pproc_quiesce() that makes sure all threads in
 * 	user space have reached a "safe point" prior to going
 * 	through the migrate or rexec code.
 * 
 * Revision 3.22  92/10/01  10:29:54  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.21  92/09/29  08:04:18  roman
 * Change reference from "site" to "node".
 * 
 * Revision 3.20  92/07/13  07:41:10  chrisp
 * Don't send p_issig_count over the wire - any issig_psig thread will be
 * 	terminated and not restarted on the new node - hence this count
 * 	will be zeroed.
 * 
 * Revision 3.19  92/07/07  15:17:02  roman
 * Pass in new ruid and rgid parameters to credentials_migrate_in().
 * 
 * Revision 3.18  92/06/19  14:41:36  chrisp
 * Mark migrated processes as exiting until cleanup complete. Without this,
 * 	bsd_issig_psig thread won't die and neither will the proc be removed.
 * 
 * Revision 3.17  92/06/16  10:43:29  chrisp
 * Transfer user area field u_sigtramp for i860 also.
 * 
 * Revision 3.16  92/06/05  15:38:22  roman
 * Correct code for handling mmap'ed files to follow v0.89 integration.
 * 
 * Revision 3.15  92/05/01  15:06:26  roman
 * Clear unexpired timers on client node after process has migrated or
 * 	rexec'ed to server node.
 * 
 * Revision 3.14  92/05/01  07:44:25  roman
 * Use new interface to credentials_migrate_in(), which adds a bunch of
 * 	new parameters to the call.
 * 
 * Revision 3.13  92/04/29  14:52:30  chrisp
 * In migrate() and rexec(), convert any outstanding realtime timer from
 * 	absolute to relative time before transfer to server node.
 * 
 * Revision 3.12  92/04/14  10:29:07  roman
 * Add calls to remote_vrele() at appropriate times, now that OSF1_ADFS code
 * 	has decided what the reference counting strategy is.
 * Change call to credentials_migrate_in() to take into account that it
 * 	returns the credentials port rather than a return code.
 * 
 * Revision 3.11  92/04/01  16:18:59  roman
 * Fix interface to credentials_migrate_in().
 * 
 * Revision 3.10  92/03/27  17:35:45  roman
 * Get rid of the "blocks" parameters to credentials_migrate_out().
 * 
 * Revision 3.9  92/03/27  11:28:26  roman
 * Major changes to take into account OSF/1 AD Release v0.8.5.1 handling
 * 	of credentials and root directory and current directory ports.
 * 
 * Revision 3.8  92/03/12  15:27:06  roman
 * Change mach_port_deallocate() to mach_port_destroy() for the task
 * and thread ports, because pproc_destroy() is sometimes called
 * (from a failed migrate or rexec on the server side) with
 * more than 1 port reference.
 * During recovery from failed rexec or migrate, do not attempt to rename
 * the process port, since we always leave a send right behind
 * and hence the name will be preserved.
 * 
 * Revision 3.7  92/02/14  08:47:31  roman
 * Change text on panic string to correctly reflect routine name.
 * With OSF1_ADFS it is no longer necessary to manipulate vnode reference counts.
 * 
 * Revision 3.6  92/01/15  16:43:07  roman
 * Simplify most of the variable accesses from the uarea.
 * Optimize the access to the mmap area to use pointer variables for the
 * sake of efficiency.
 * Change reinstall_cred_port() to reinstall_process_ports() and make the
 * routine also reinstall the proc port in addition to the cred
 * port.
 * 
 * Revision 3.5  92/01/14  13:17:55  yazz
 * More info included in panic messages, since that is allowed now.
 * 
 * Revision 3.4  92/01/10  14:52:59  yazz
 * Credentials (.74 version) merge.  Rcv rt to existing cred port is now
 * moved to the new node on remote task operations.  On remote-task
 * failures, the three moved Rcv rts (vproc, cred and proc) are properly
 * returned to the originating node where they are properly re-installed.
 * 
 * Revision 3.3  92/01/10  10:09:03  roman
 * For rexecve(), only pass over the mmap structures that are preserved
 * for exec to the remote node, rather than all of them.
 * 
 * Revision 3.2  92/01/07  18:21:45  roman
 * Fill in the mmap structures that must be passed from server to server
 * for rfork/rexec/migrate.
 * 
 * Revision 3.1  91/12/16  09:30:52  roman
 * Chnages to get rexec to work for the first time.
 * 
 * Revision 3.0  91/12/13  09:13:19  roman
 * Initial submission. This file contains the physical process code for
 * the client side of rfork/rexec/migrate.
 * 
 * 
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/signal.h>
#include <sys/kernel.h>
#include <tnc/rtask.h>
#include <uxkern/proc_to_task.h>
#include <mach/kern_return.h>
#include <mach/time_value.h>
#include <kern/sched_prim.h>
#include <kern/parallel.h>
#include <kern/kalloc.h>


void
rtask_pproc_remove(
	struct proc	*p)
{
	register int	i;
	register int	x;

	/*
	 * Cause the interval timers to not go off any more.
	 */
	untimeout(realitexpire, (caddr_t)p);

	/*
	 * Release vnodes for current and root directories (Note that the
	 * releases are done using the proc structure pointer rather
	 * than the "u.u_*" form. This is because the uarea for the
	 * process is not set up correctly at the time this is called
	 * on the server side for failed rexec calls).
	 */
#ifdef OSF1_ADFS
	remote_vrele(&p->p_utask.uu_cdirproxy);
	remote_vrele(&p->p_utask.uu_rdirproxy);
#else
	vrele(p->p_utask.uu_cdir);
	if (p->p_utask.uu_rdir)
		vrele(p->p_utask.uu_rdir);
#endif

	/*
	 * Mark it as exiting.
	 * Take the process lock to avoid new server threads to register 
	 * for this process. When SWEXIT flag is on, registration requests
	 * are refused, so it's safe after that.
	 */
	simple_lock(&p->p_lock);
	p->p_flag |= SWEXIT;
	p->p_flag &= ~STRC;
	simple_unlock(&p->p_lock);

	/*
	 * Terminate all server threads working for that task,
	 * except for the current thread.
	 */
	{
	    register uthread_t	cth;
	    extern   uthread_t	server_thread_find();
	    extern   void	server_thread_deregister();

	    while ((cth = server_thread_find(p)) != 0) {
		if (cth == &u) {
		    server_thread_deregister(cth, p);
		}
		else {
		    clear_wait(cth, THREAD_SHOULD_TERMINATE, FALSE);
		    sleep((caddr_t)&p->p_flag, PSPECL);
		}
	    }
	}

	/*
	 * Get rid of mmap'ed files.
	 */
	vm_exit(p);

#if PROFILING
#if GPROF
	if (p->p_taskprofed) {	/* We are emulator_profiling. */
		if (!p->p_profport)
			panic("rtask_pproc_remove: profport");
		task_sample(p->p_task, MACH_PORT_NULL);
		pport_to_proc_sleep(p->p_profport);
	}
#endif
	/* Deallocate the per user process profiling port */
	if (p->p_profport)
		pport_to_proc_remove(p->p_profport);
#endif

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

	/*
	 * Get rid of rights to task and thread ports.
	 */
	task_to_proc_remove(p->p_task);
	(void) mach_port_destroy(mach_task_self(), p->p_task);
	p->p_task = MACH_PORT_NULL;
	if (MACH_PORT_VALID(p->p_thread))
		(void) mach_port_destroy(mach_task_self(), p->p_thread);
	p->p_thread = MACH_PORT_NULL;
		
	/*
	 * Do all the process queue manipulation.
	 */
	i = PIDHASH(p->p_pid);		/* off proc hash list */
	x = p - proc;
	if (pidhash[i] == x)
		pidhash[i] = p->p_idhash;
	else {
		for (i = pidhash[i]; i != 0; i = proc[i].p_idhash)
			if (proc[i].p_idhash == x) {
				proc[i].p_idhash = p->p_idhash;
				goto done;
			}
		panic("rtask_pproc_remove: pidhash list removal");
	}
done:
	TNC_unix_master();

	ASSERT(p->p_ref_count == 1);
	p->p_ref_count--;

	if (*p->p_prev = p->p_nxt)	/* off allproc queue */
		p->p_nxt->p_prev = p->p_prev;
	p->p_stat = NULL;		/* clear major process state field */

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
		p->p_flag |= SREAPED;	/* call pproc_clear() later */
	} else {
		pproc_clear(p);		/* call it now */
	}

	TNC_unix_release();
}


void
rfork_pproc_load_msg(
	struct proc	*p,
	struct rf_data	*rf_data,
	task_t		*par_taskp,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	*mmap_countp,
	mach_port_t	*rdir_portp,
	mach_port_t	*cdir_portp,
	char		*command_name,
	char		*logname
#ifdef PARACORE
,	mach_port_t	*exec_rdir_portp,
	mach_port_t	*exec_cdir_portp,
	char		*exec_prg_name
#endif /* PARACORE */
	)
{
	register struct ucred	*credp;
	register int		i;

	/*
	 * Begin loading info from the proc structure.
	 */
	rf_data->rf_p_nice = p->p_nice;
	rf_data->rf_p_sigmask = p->p_sigmask;
	rf_data->rf_p_sigignore = p->p_sigignore;
	rf_data->rf_p_sigcatch = p->p_sigcatch;
	rf_data->rf_p_flag = p->p_flag;
	rf_data->rf_p_ruid = p->p_ruid;
	rf_data->rf_p_svuid = p->p_svuid;
	rf_data->rf_p_rgid = p->p_rgid;
	rf_data->rf_p_svgid = p->p_svgid;

	/* load the credentials info */
	credp = p->p_rcred;
	rf_data->rf_p_cr_uid = credp->cr_uid;
#ifdef NX
	rf_data->rf_p_cr_acctid = credp->cr_acctid;
#endif /* NX */
	rf_data->rf_p_cr_gid = credp->cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		rf_data->rf_p_cr_groups[i] = credp->cr_groups[i];
	}
	rf_data->rf_p_cr_ngroups = credp->cr_ngroups;

#if defined(MACH_AFS)
	rf_data->rf_p_cr_pag = credp->cr_pag;
#endif

	/* continue with more proc info */
	rf_data->rf_p_pid = p->p_pid;
	rf_data->rf_p_pgid = p->p_pgid;
	rf_data->rf_p_sid = p->p_sid;
	rf_data->rf_p_realtimer_coe = p->p_realtimer_coe;	/* shouldn't fork zero this out? */

	/*
	 * Now load info from the user structure.
	 */
	rf_data->rf_u_tsize = u.u_tsize;
	rf_data->rf_u_dsize = u.u_dsize;
	rf_data->rf_u_ssize = u.u_ssize;
	rf_data->rf_u_text_start = u.u_text_start;
	rf_data->rf_u_data_start = u.u_data_start;
	rf_data->rf_u_stack_start = u.u_stack_start;
	rf_data->rf_u_stack_end = u.u_stack_end;
	rf_data->rf_u_stack_grows_up = u.u_stack_grows_up;
	rf_data->rf_u_outime = u.u_outime;	/* ???????? reinitted at fork time? */
	for (i=0; i<NSIG+1; ++i) {
		rf_data->rf_u_signal[i] = u.u_signal[i];
		rf_data->rf_u_sigmask[i] = u.u_sigmask[i];
	}
#ifdef i386
	rf_data->rf_u_sigreturn = u.u_sigreturn;
#endif
#ifdef multimax
	rf_data->rf_u_sigcatch = u.u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	rf_data->rf_u_sigtramp = u.u_sigtramp;
#endif
	rf_data->rf_u_sigonstack = u.u_sigonstack;
	rf_data->rf_u_sigintr = u.u_sigintr;
	rf_data->rf_u_oldmask = u.u_oldmask;
	rf_data->rf_u_sigstack_ss_sp = u.u_sigstack.ss_sp;
	rf_data->rf_u_sigstack_ss_onstack = u.u_sigstack.ss_onstack;
	rf_data->rf_u_cmask = u.u_cmask;
	rf_data->rf_u_ioch = u.u_ioch;	/* shouldn't this be zeroed on fork? */

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		rf_data->rf_u_timer[i] = u.u_timer[i];
	}

	rf_data->rf_u_prof_base = u.u_prof.pr_base;
	rf_data->rf_u_prof_size = u.u_prof.pr_size;
	rf_data->rf_u_prof_off = u.u_prof.pr_off;
	rf_data->rf_u_prof_scale = u.u_prof.pr_scale;
	rf_data->rf_u_maxuprc = u.u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		rf_data->rf_u_rlimit[i] = u.u_rlimit[i];
	}

	rf_data->rf_u_shmsegs = u.u_shmsegs;
	rf_data->rf_u_argp = (int)u.u_argp;
	rf_data->rf_u_envp = (int)u.u_envp;
	rf_data->rf_u_arg_size = u.u_arg_size;
	rf_data->rf_u_env_size = u.u_env_size;

	/*
	 * Fill in the mmap data.
	 */
	{
		struct mmap_struct	*ms = &mmap_structs[0];
		vm_mmap_t		cur = p->p_utask.uu_mmap;
		mach_port_t		*pagerp = &mmap_pagers[0];
		struct utask		*utask = &p->p_utask;

		U_MMAP_READ_LOCK(utask);

		for (i=0; cur!=NULL && i<TNC_MAX_MMAP_REGIONS;
				++i,cur=cur->link,++ms,++pagerp) {
			ms->start = cur->start;
			ms->length = cur->length;
			ms->node = cur->node;
			ms->offset = cur->offset;
			ms->flags = cur->flags;

			*pagerp = (mach_port_t) cur->pager;
#if MACH_ASSERT
			if ( (cur->flags & VM_MMAP_FORCE_KEEP_ON_EXEC) == 0 ) {
							/* not emulator */
				bootnode_printf("Warning: pid %d '%s' rfork()/"
					"rforkmulti() with extra region addr="
					"0x%x len=%dK flags=0x%x pager=0x%x\n",
					p->p_pid, utask->u_comm, cur->start,
					cur->length/1024, cur->flags);
			}
#endif
		}
		ASSERT(i > 0 && i < TNC_MAX_MMAP_REGIONS);
		*mmap_countp = i;

		U_MMAP_UNLOCK(utask);
	}

	/*
	 * Fill in the root and current directories.
	 */
	get_vnode_port_from_proxy(&p->p_utask.uu_rdirproxy, rdir_portp);
	get_vnode_port_from_proxy(&p->p_utask.uu_cdirproxy, cdir_portp);

#ifdef PARACORE
	/*
	 * Fill in the exec root and current directories.
	 */
	get_vnode_port_from_proxy(&p->p_utask.uu_exec_utnd.utnd_rdir, 
				  exec_rdir_portp);
	get_vnode_port_from_proxy(&p->p_utask.uu_exec_utnd.utnd_cdir, 
				  exec_cdir_portp);
	/*
	 * Fill in the exec program name string parameter.
	 */
	strncpy(exec_prg_name, p->p_utask.uu_exec_prg_name, PATH_MAX);
#endif /* PARACORE */

	/*
	 * Fill in the string parameters
	 */
	bcopy(u.u_comm, command_name, MAXCOMLEN+1);	/* yes, +1 */
	bcopy(u.u_logname, logname, MAXLOGNAME);	/* no +1 here */

	/*
	 * Return the task port and we're done.
	 */
	*par_taskp = p->p_task;
}


int
migrate_pproc_load_msg(
	struct proc	*p,
	struct mi_data	*mi_data,
	task_t		*cur_taskp,
	mach_port_t	*cur_proc_portp,
	mach_port_t	*cur_cred_portp,
	node_t		*cur_cred_cache_vectorp[],
	unsigned int	*cur_cred_cache_vector_sizep,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	*mmap_countp,
	mach_port_t	*rdir_portp,
	mach_port_t	*cdir_portp,
	char		*command_name,
	char		*logname
#ifdef PARACORE
,	mach_port_t	*exec_rdir_portp,
	mach_port_t	*exec_cdir_portp,
	char		*exec_prg_name
#endif /* PARACORE */
	)
{
	register struct ucred	*credp;
	register int		i;
	int			ret;
	struct timeval		*timerp;
	unsigned int		old_cred_cache_size;

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

	/*
	 * Prepare credentials for migration.
	 */
	if (cur_cred_cache_vector_sizep != NULL) {
		old_cred_cache_size = *cur_cred_cache_vector_sizep;
		ret = credentials_migrate_out(p->p_cred, 
				      *cur_cred_cache_vectorp,
				      cur_cred_cache_vector_sizep);
		while (ret != KERN_SUCCESS && 
			*cur_cred_cache_vector_sizep > old_cred_cache_size) {
			ret = vm_allocate(mach_task_self(),
				  (vm_address_t *) cur_cred_cache_vectorp,
				  *cur_cred_cache_vector_sizep * sizeof(node_t),
				  TRUE);
			if (ret != KERN_SUCCESS)
				return(EAGAIN);
			old_cred_cache_size = *cur_cred_cache_vector_sizep;
			ret = credentials_migrate_out(p->p_cred, 
					      *cur_cred_cache_vectorp,
					      cur_cred_cache_vector_sizep);
			if (ret != KERN_SUCCESS)
				(void) vm_deallocate(mach_task_self(),
				   (vm_address_t) *cur_cred_cache_vectorp,
			 	   old_cred_cache_size * sizeof(node_t));
		}
		if (ret != KERN_SUCCESS) {
			panic("migrate_pproc_load_msg: credentials_migrate_out "
			      "failed port=0x%x ret=0x%x", *cur_cred_portp, ret);
		}
	}

	/*
	 * Begin loading info from the proc structure.
	 */
	mi_data->mi_p_nice = p->p_nice;
	mi_data->mi_p_cursig = p->p_cursig;
	mi_data->mi_p_sig = p->p_sig;
	mi_data->mi_p_sigmask = p->p_sigmask;
	mi_data->mi_p_sigignore = p->p_sigignore;
	mi_data->mi_p_sigcatch = p->p_sigcatch;
	mi_data->mi_p_flag = p->p_flag;
	mi_data->mi_p_ruid = p->p_ruid;
	mi_data->mi_p_svuid = p->p_svuid;
	mi_data->mi_p_rgid = p->p_rgid;
	mi_data->mi_p_svgid = p->p_svgid;

	/* load the credentials info */
	credp = p->p_rcred;
	mi_data->mi_p_cr_uid = credp->cr_uid;
#ifdef NX
	mi_data->mi_p_cr_acctid = credp->cr_acctid;
#endif /* NX */
	mi_data->mi_p_cr_gid = credp->cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		mi_data->mi_p_cr_groups[i] = credp->cr_groups[i];
	}
	mi_data->mi_p_cr_ngroups = credp->cr_ngroups;

#if defined(MACH_AFS)
	mi_data->mi_p_cr_pag = credp->cr_pag;
#endif

	/* continue with more proc info */
	mi_data->mi_p_pid = p->p_pid;
	mi_data->mi_p_ppid = p->p_ppid;	/* don't forget parent's pid */
	mi_data->mi_p_pgid = p->p_pgid;
	mi_data->mi_p_sid = p->p_sid;
	mi_data->mi_p_realtimer_coe = p->p_realtimer_coe;
	mi_data->mi_p_stopsig = p->p_stopsig;
	mi_data->mi_p_realtimer = p->p_realtimer;
	mi_data->mi_p_logdev = p->p_logdev;
	mi_data->mi_sigwait = p->sigwait;

	/*
	 * Convert any outstanding realtime timer from absolute to relative
	 * time so not to be affected by clock skew between nodes.
	 */
	timerp = &mi_data->mi_p_realtimer.it_value;
	if (timerisset(timerp)) {
		TIME_READ_LOCK();
		if (timercmp(timerp, &time, <))
			timerclear(timerp);
		else
			timevalsub(timerp, &time);
		TIME_READ_UNLOCK();
	}
	
	/*
	 * Now load info from the user structure.
	 */
	mi_data->mi_u_tsize = u.u_tsize;
	mi_data->mi_u_dsize = u.u_dsize;
	mi_data->mi_u_ssize = u.u_ssize;
	mi_data->mi_u_text_start = u.u_text_start;
	mi_data->mi_u_data_start = u.u_data_start;
	mi_data->mi_u_stack_start = u.u_stack_start;
	mi_data->mi_u_stack_end = u.u_stack_end;
	mi_data->mi_u_stack_grows_up = u.u_stack_grows_up;
	mi_data->mi_u_outime = u.u_outime;
	for (i=0; i<NSIG+1; ++i) {
		mi_data->mi_u_signal[i] = u.u_signal[i];
		mi_data->mi_u_sigmask[i] = u.u_sigmask[i];
	}
#ifdef i386
	mi_data->mi_u_sigreturn = u.u_sigreturn;
#endif
#ifdef multimax
	mi_data->mi_u_sigcatch = u.u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	mi_data->mi_u_sigtramp = u.u_sigtramp;
#endif
	mi_data->mi_u_sigonstack = u.u_sigonstack;
	mi_data->mi_u_sigintr = u.u_sigintr;
	mi_data->mi_u_oldmask = u.u_oldmask;
	mi_data->mi_u_sigstack_ss_sp = u.u_sigstack.ss_sp;
	mi_data->mi_u_sigstack_ss_onstack = u.u_sigstack.ss_onstack;
	mi_data->mi_u_cmask = u.u_cmask;
	mi_data->mi_u_ioch = u.u_ioch;
	mi_data->mi_u_ru = u.u_ru;
	mi_data->mi_u_cru = u.u_cru;

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		mi_data->mi_u_timer[i] = u.u_timer[i];
	}

	mi_data->mi_u_prof_base = u.u_prof.pr_base;
	mi_data->mi_u_prof_size = u.u_prof.pr_size;
	mi_data->mi_u_prof_off = u.u_prof.pr_off;
	mi_data->mi_u_prof_scale = u.u_prof.pr_scale;
	mi_data->mi_u_maxuprc = u.u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		mi_data->mi_u_rlimit[i] = u.u_rlimit[i];
	}

	mi_data->mi_u_shmsegs = u.u_shmsegs;
	mi_data->mi_u_svipc_flag = u.u_svipc_flag;
	mi_data->mi_u_argp = (int)u.u_argp;
	mi_data->mi_u_envp = (int)u.u_envp;
	mi_data->mi_u_arg_size = u.u_arg_size;
	mi_data->mi_u_env_size = u.u_env_size;

	/*
	 * Fill in the mmap data.
	 */
	if (mmap_structs != NULL) {
		struct mmap_struct	*ms = &mmap_structs[0];
		vm_mmap_t		cur = p->p_utask.uu_mmap;
		mach_port_t		*pagerp = &mmap_pagers[0];
		struct utask		*utask = &p->p_utask;

		U_MMAP_READ_LOCK(utask);

		for (i=0; cur!=NULL && i<TNC_MAX_MMAP_REGIONS;
				++i,cur=cur->link,++ms,++pagerp) {
			ms->start = cur->start;
			ms->length = cur->length;
			ms->node = cur->node;
			ms->offset = cur->offset;
			ms->flags = cur->flags;

			*pagerp = (mach_port_t) cur->pager;
		}
		ASSERT(i > 0 && i < TNC_MAX_MMAP_REGIONS);
		*mmap_countp = i;

		U_MMAP_UNLOCK(utask);
	}

	/*
	 * Fill in the root and current directories.
	 */
	if (rdir_portp != NULL)
		get_vnode_port_from_proxy(&p->p_utask.uu_rdirproxy, rdir_portp);
	if (cdir_portp != NULL)
		get_vnode_port_from_proxy(&p->p_utask.uu_cdirproxy, cdir_portp);

#ifdef PARACORE
	/*
	 * Fill in the exec root and current directories.
	 */
	if (exec_rdir_portp != NULL)
		get_vnode_port_from_proxy(&p->p_utask.uu_exec_utnd.utnd_rdir, 
					  exec_rdir_portp);
	if (exec_cdir_portp != NULL)
		get_vnode_port_from_proxy(&p->p_utask.uu_exec_utnd.utnd_cdir, 
					  exec_cdir_portp);
	/*
	 * Fill in the exec program name string parameter.
	 */
	 if (exec_prg_name != NULL)
		strncpy(exec_prg_name, p->p_utask.uu_exec_prg_name, PATH_MAX);
#endif /* PARACORE */

	/*
	 * Fill in the string parameters
	 */
	bcopy(u.u_comm, command_name, MAXCOMLEN+1);	/* yes, +1 */
	bcopy(u.u_logname, logname, MAXLOGNAME);	/* no +1 here */

	/*
	 * Return the task port (Snd rt) and the proc port (Rcv rt).
	 */
	if (cur_cred_portp != NULL)
		*cur_cred_portp = p->p_cred;
	if (cur_taskp != NULL)
		*cur_taskp = p->p_task;
	if (cur_proc_portp != NULL)
		*cur_proc_portp = proc_to_port_lookup(p);

	return(ESUCCESS);
}


int
rexecve_pproc_load_msg(
	struct proc *p,
	struct re_data	*re_data,
	task_t		*cur_taskp,
	mach_port_t	*cur_proc_portp,
	mach_port_t	*cur_cred_portp,
	node_t		*cur_cred_cache_vectorp[],
	unsigned int	*cur_cred_cache_vector_sizep,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	*mmap_countp,
	mach_port_t	*rdir_portp,
	mach_port_t	*cdir_portp,
	char		*logname)
{
	register struct ucred	*credp;
	register int		i, j;
	int			ret;
	struct timeval		*timerp;
	unsigned int		old_cred_cache_size;

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

	/*
	 * Prepare credentials for migration.
	 */
	old_cred_cache_size = *cur_cred_cache_vector_sizep;
	ret = credentials_migrate_out(p->p_cred, 
				      *cur_cred_cache_vectorp,
				      cur_cred_cache_vector_sizep);
	while (ret != KERN_SUCCESS && 
			*cur_cred_cache_vector_sizep > old_cred_cache_size) {
		ret = vm_allocate(mach_task_self(),
				  (vm_address_t *) cur_cred_cache_vectorp,
				  *cur_cred_cache_vector_sizep * sizeof(node_t),
				  TRUE);
		if (ret != KERN_SUCCESS)
			return(EAGAIN);
		old_cred_cache_size = *cur_cred_cache_vector_sizep;
		ret = credentials_migrate_out(p->p_cred, 
					      *cur_cred_cache_vectorp,
					      cur_cred_cache_vector_sizep);
		if (ret != KERN_SUCCESS)
			(void) vm_deallocate(mach_task_self(),
				   (vm_address_t) *cur_cred_cache_vectorp,
			 	   old_cred_cache_size * sizeof(node_t));
	}
	if( ret != KERN_SUCCESS ) {
		panic("rexecve_pproc_load_msg: credentials_migrate_out failed "
		      "port=0x%x ret=0x%x", *cur_cred_portp, ret);
	}

	/*
	 * Begin loading info from the proc structure.
	 */
	re_data->re_p_nice = p->p_nice;
	re_data->re_p_cursig = p->p_cursig;
	re_data->re_p_sig = p->p_sig;
	re_data->re_p_sigmask = p->p_sigmask;
	re_data->re_p_sigignore = p->p_sigignore;
	re_data->re_p_sigcatch = p->p_sigcatch;
	re_data->re_p_flag = p->p_flag;
	re_data->re_p_ruid = p->p_ruid;
	re_data->re_p_svuid = p->p_svuid;
	re_data->re_p_rgid = p->p_rgid;
	re_data->re_p_svgid = p->p_svgid;

	/* load the credentials info */
	credp = p->p_rcred;
	re_data->re_p_cr_uid = credp->cr_uid;
#ifdef NX
	re_data->re_p_cr_acctid = credp->cr_acctid;
#endif /* NX */
	re_data->re_p_cr_gid = credp->cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		re_data->re_p_cr_groups[i] = credp->cr_groups[i];
	}
	re_data->re_p_cr_ngroups = credp->cr_ngroups;

#if defined(MACH_AFS)
	re_data->re_p_cr_pag = credp->cr_pag;
#endif

	/* continue with more proc info */
	re_data->re_p_pid = p->p_pid;
	re_data->re_p_ppid = p->p_ppid;
	re_data->re_p_pgid = p->p_pgid;
	re_data->re_p_sid = p->p_sid;
	re_data->re_p_realtimer_coe = p->p_realtimer_coe;
	re_data->re_p_stopsig = p->p_stopsig;
	re_data->re_p_realtimer = p->p_realtimer;
	re_data->re_p_logdev = p->p_logdev;
	re_data->re_sigwait = p->sigwait;

	/*
	 * Convert any outstanding realtime timer from absolute to relative
	 * time so not to be affected by clock skew between nodes.
	 */
	timerp = &re_data->re_p_realtimer.it_value;
	if (timerisset(timerp)) {
		TIME_READ_LOCK();
		if (timercmp(timerp, &time, <))
			timerclear(timerp);
		else
			timevalsub(timerp, &time);
		TIME_READ_UNLOCK();
	}
	
	/*
	 * Now load info from the user structure.
	 */
	re_data->re_u_outime = u.u_outime;
	for (i=0; i<NSIG+1; ++i) {
		re_data->re_u_signal[i] = u.u_signal[i];
		re_data->re_u_sigmask[i] = u.u_sigmask[i];
	}
#ifdef i386
	re_data->re_u_sigreturn = u.u_sigreturn;
#endif
#ifdef multimax
	re_data->re_u_sigcatch = u.u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	re_data->re_u_sigtramp = u.u_sigtramp;
#endif
	re_data->re_u_sigintr = u.u_sigintr;
	re_data->re_u_oldmask = u.u_oldmask;
	re_data->re_u_cmask = u.u_cmask;
	re_data->re_u_ioch = u.u_ioch;
	re_data->re_u_ru = u.u_ru;
	re_data->re_u_cru = u.u_cru;

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		re_data->re_u_timer[i] = u.u_timer[i];
	}

	re_data->re_u_maxuprc = u.u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		re_data->re_u_rlimit[i] = u.u_rlimit[i];
	}

	re_data->re_u_shmsegs = u.u_shmsegs;
	re_data->re_u_svipc_flag = u.u_svipc_flag;

	/*
	 * Fill in the mmap data.
	 */
	{
		struct mmap_struct	*ms = &mmap_structs[0];
		vm_mmap_t		cur = p->p_utask.uu_mmap;
		mach_port_t		*pagerp = &mmap_pagers[0];
		int			j = 0;
		struct utask		*utask = &p->p_utask;

		U_MMAP_READ_LOCK(utask);

		for (i=0; cur!=NULL && i<TNC_MAX_MMAP_REGIONS;
				++i,++j,cur=cur->link,++ms,++pagerp) {
			if (cur->flags & (VM_MMAP_KEEP_ON_EXEC | 
					   VM_MMAP_FORCE_KEEP_ON_EXEC)) {
				ms->start = cur->start;
				ms->length = cur->length;
				ms->node = cur->node;
				ms->offset = cur->offset;
				ms->flags = cur->flags;

				*pagerp = (mach_port_t) cur->pager;
			}
		}
		ASSERT(i > 0 && i < TNC_MAX_MMAP_REGIONS);
		*mmap_countp = j;

		U_MMAP_UNLOCK(utask);
	}

	/*
	 * Fill in the root and current directories.
	 */
	get_vnode_port_from_proxy(&p->p_utask.uu_rdirproxy, rdir_portp);
	get_vnode_port_from_proxy(&p->p_utask.uu_cdirproxy, cdir_portp);

	/*
	 * Fill in the string parameter
	 */
	bcopy(u.u_logname, logname, MAXLOGNAME);	/* no +1 here */

	/*
	 * Return the task port (Snd rt) and the proc port (Rcv rt).
	 */
	*cur_cred_portp = p->p_cred;
	*cur_taskp = p->p_task;
	*cur_proc_portp = proc_to_port_lookup(p);

	return(ESUCCESS);
}

/*
 * For migrate and rexecve, some process receive rights (the process port
 * and the credentials port) are moved to the new node. If the call fails,
 * then these ports are returned, and must be reinstalled with the
 * appropriate characteristics (name, port set) so they can continue
 * to be used on this node.
 */
void
reinstall_process_ports(
	mach_port_t	ret_proc_port,
	mach_port_t	ret_cred_port,
	int		*cur_cred_cache_vector,
	int		cur_cred_cache_vector_size)
{
	struct proc	*p = u.u_procp;

	ASSERT(ret_proc_port == proc_to_port_lookup(p));
	ux_server_add_port(proc_to_port_lookup(p));

	p->p_cred = credentials_migrate_in(p->p_pid,
				p->p_pgid, p->p_sid, 
				p->p_rcred, p->p_utask.uu_cmask,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
				p->p_ruid, p->p_rgid,
				p->p_task,
				ret_cred_port, 
				cur_cred_cache_vector, 
				cur_cred_cache_vector_size);
	if(p->p_cred == MACH_PORT_NULL) {
		panic("reinstall_process_ports: credentials_migrate_in failed");
	}
}

int
rtask_pproc_quiesce(
	struct proc	*p)
{
	int	error;

	unix_master();
	error = unix_task_suspend(p);
	unix_release();
	if (error != ESUCCESS) {
		printf("rtask_pproc_quiesce: "
		       "unix_task_suspend failed, error=%d\n", error);
	}
	return(error);
}

int
rtask_pproc_unquiesce(
	struct proc	*p)
{
	int	error;

	error = task_resume(p->p_task);
	if (error != ESUCCESS) {
		printf("rtask_pproc_unquiesce: "
		       "task_resume failed, error=%d\n", error);
	}
	return(error);
}
