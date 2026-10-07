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
 * $Log: rtask_svr_pproc.c,v $
 * Revision 1.16  1995/02/01  21:49:28  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.15  1994/12/20  23:05:09  suri
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
 * Revision 1.14  1994/11/18  20:43:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.13  1994/10/25  23:51:52  suri
 *  Reviewer jlitvin
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
 * Revision 1.12  1994/07/27  16:53:10  johannes
 * In the extended rtask_pproc_fork() the exec ports are stored in utask.
 *
 * In the extended rfork_pproc_unload_msg() the file name is stored in utask.
 *
 * In the extended migrate_pproc_unload_msg() the file name is stored in utask.
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
 * Revision 1.11  1994/06/06  09:43:58  stefan
 * Removed i386-specific include files. Apparently these files are not even
 * needed for i386 builds.
 *
 *  Reviewer: chrisp, cfj
 *  Risk: low
 *  Benefit or PTS #: remove compiler warnings and potential for incompatibility
 *                    problems
 *  Testing: built for i860 and for i386
 *  Module(s): server/tnc/rtask_svr_pproc.c
 *
 * Revision 1.10  1994/03/14  02:06:10  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.9  1993/12/11  01:14:23  yazz
 *  Reviewer: cfj
 *  Risk: low
 *  Benefit or PTS #: 6916
 *  Testing: multitest.c now passes
 *  Module(s): server/tnc/rtask_svr_pproc.c + server/tnc/rtask_svr_vproc.c
 *
 *
 * When the physical layer can't fork a new process, assign MACH_PORT_NULL
 * to the task and thread ports.  Formerly these were left untouched in this
 * error case, leaving random values to be used as port rights.
 *
 * Revision 1.8  1993/10/28  03:10:03  yazz
 * Augment panic() mesage to include affected port name.
 *
 * Revision 1.7  1993/07/14  18:33:51  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.4  1993/07/01  20:46:31  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:23:44  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.2  1993/05/03  17:46:22  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/04/03  03:09:11  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.2.2.1  1992/12/16  06:02:42  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.4  1992/12/11  03:02:08  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.3  1992/11/30  22:48:06  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.2  1992/11/06  20:31:47  dleslie
 * Merged bug drop from Locus November 3, 1992, with NX development
 *
 * Revision 1.1.2.1  1992/11/05  22:46:08  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 3.31  93/06/20  18:26:54  yazz
 * [ ad1.04 merge ]
 * 	Add node parameter to pproc_fork() call.
 * 
 * Revision 3.30  93/06/19  15:27:20  yazz
 * [ ad1.04 merge ]
 * 	Respect new dynamically allocated vm_mmap region structures when
 * 	loading & unloading proc structure during remote tasking operations.
 * 
 * Revision 3.29  93/06/02  12:28:36  yazz
 * For Sys V IPC under TNC unload svipc flag field in migrate and remote
 * exec operations.
 * 
 * Revision 3.28  93/04/22  16:39:58  roman
 * Fix spelling mistake and minor code formatting correction.
 *
 * Revision 3.27  92/11/02  11:45:21  roman
 * Add code to deal with the fact that the credentials vector can be
 * 	arbitrarily large; memory is vm_allocated when necessary.
 * 
 * Revision 3.26  92/10/28  15:28:22  roman
 * Change types for cleaner compilation.
 * 
 * Revision 3.25  92/10/01  10:30:39  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 3.24  92/09/29  08:09:55  roman
 * Add typecasts for clean compilation.
 * 
 * Revision 3.23  92/07/13  07:42:50  chrisp
 * Don't propagate p_issig_count over the wire - any issig_psig thread will
 * 	not be restarted on the new node - hence this count will be zero.
 * 
 * Revision 3.22  92/07/08  09:09:24  roman
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 3.21  92/07/07  15:17:28  roman
 * Pass in new ruid and rgid parameters to credentials_migrate_in().
 * 
 * Revision 3.20  92/06/16  10:44:00  chrisp
 * Transfer user area field u_sigtramp in the i860 case also.
 * 
 * Revision 3.19  92/06/05  15:39:20  roman
 * Fix code for mmap'ed files following v0.89 integration.
 * 
 * Revision 3.18  92/05/01  07:45:29  roman
 * Use new interface to credentials_migrate_in() to set up the new credentials
 * 	structure correctly. It also makes the subsequent call to
 * 	credentials_set_state() unnecessary.
 * 
 * Revision 3.17  92/04/29  14:54:15  chrisp
 * In migrate() and rexec(), convert outstanding realtime timer from relative
 * 	to absolute time and re-establish the timeout on the new node.
 * 
 * Revision 3.16  92/04/22  08:49:03  chrisp
 * Add master locking around call to execve().
 * 
 * Revision 3.15  92/04/14  14:28:44  roman
 * Get rid of extraneous root directory and current directory parameters to
 * 	rexecve_pproc_exec().
 * 
 * Revision 3.14  92/04/14  10:37:36  roman
 * Change error logic for rtask_pproc_fork() to branch to common endpoint.
 * Check the return from credentials_migrate_in() correctly - it returns
 * 	MACH_PORT_NULL on failure.
 * Add calls to remote_vrele() in rtask_pproc() fork to get the vnode proxy
 * 	reference counts right in the dummy process.
 * Set up u.u_nd.ni_utnd correctly for the duration of the execve() call
 * 	(this allows root and current directories to be correctly
 * 	referenced).
 * Remove permanently some dead commented-out code for setting up the namei
 * 	data structure.
 * 
 * Revision 3.13  92/04/02  11:05:38  chrisp
 * Set norma_task_create'd tasks exception ports to the exception port
 * serviced by the local server.
 * 
 * Revision 3.12  92/03/27  17:37:52  roman
 * Get rid of the task parameter from credentials_migrate_in().
 * Add call to credentials_set_state() to actually set up the credentials'
 * 	task.
 * Minor printf fix.
 * 
 * Revision 3.11  92/03/27  11:30:23  roman
 * Major changes to take into account the new OSF/1 AD v0.8.5.1 handling of
 * 	root and current working directory ports and credentials.
 * 
 * Revision 3.10  92/02/14  08:52:52  roman
 * Reorganize rexec so that execve() failure occurs prior to difficult-to-undo
 * deallocation and renaming of process ports.
 * Make sure process port and credentials port are now in/out parameters so
 * higher level routines are cognizant of name changes.
 * 
 * Revision 3.9  92/01/28  16:23:45  roman
 * Set up namei data structure for current directory and root directory
 * ports prior to calling execve().
 * Change name of routine on some panic messages.
 * Change code to use separate variables for Mach error codes and Unix
 * error codes.
 * 
 * Revision 3.8  92/01/15  16:49:45  roman
 * Get rid of call to task_set_child_node() since this is now extraneous,
 * given the task_create_remote() can be used during all calls to
 * newproc().
 * Change uarea references to be simpler throughout the file.
 * Change mmap references to use pointer dereferencing rather that array
 * manipulation (for the sake of efficiency).
 * Numerous minor formatting and indenting corrections to make the file
 * consistent in style throughout.
 * 
 * Revision 3.7  92/01/14  13:21:12  yazz
 * Additional info now included in panic messages, since that is allowed now.
 * 
 * Revision 3.6  92/01/10  15:02:41  yazz
 * Credentials (.74 version) merge.  Remote task calls can fail and have
 * the original task put back together, including Rcv rts being returned
 * to the original node and being moved back into listening port-sets.
 * On remote tasking fewer node-to-node port moving operations now take
 * place due to passing of port names, instead of actual ports, where
 * possible.
 * 
 * Revision 3.5  92/01/10  10:11:53  roman
 * Merge rexecve_arrival1() and rexecve_arrival2() into a single routine
 * called rexecve_arrival().
 * Divide rexecve_pproc_fix() into two routines - rexecve_pproc_exec() and
 * rexecve_pproc_fix(). This does the actual exec a lot earlier, and
 * returns an error code, allowing something reasonable to be done
 * if rexecve has bad parameters.
 * 
 * Revision 3.4  92/01/07  18:25:34  roman
 * Fill in the mmap structures into the dummy process used by
 * rfork/rexec/migrate so that memory mapped files can be inherited
 * across these calls.
 * Change variable declarations for auxv variables to correspond closer with
 * reality.
 * 
 * Revision 3.3  92/01/06  18:21:59  roman
 * Add missing return(0) to rexecve_arrival1() and rexecve_arrival2().
 * 
 * Revision 3.2  91/12/18  16:47:35  roman
 * Remove much unused debugging code.
 * 
 * Revision 3.1  91/12/16  09:32:08  roman
 * Chnages to get rexec to work for the first time.
 * 
 * Revision 3.0  91/12/13  09:14:43  roman
 * Initial submission. This file contains the physical process code for the
 * server side of rfork/rexec/migrate.
 * 
 *
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/signal.h>
#include <sys/auxv.h>
#include <sys/vnode.h>
#include <tnc/rtask.h>
#include <uxkern/proc_to_task.h>
#include <uxkern/bsd_types.h>
#include <sys/ux_exception.h>
#include <kern/parallel.h>

/*
 * We do the physical layer's work for svr_rfork(), svr_migrate() and
 * svr_rexecve().
 */
int
rtask_pproc_fork(
	pid_t		ch_pid,		/* pid, not ptr to pid */
	struct proc	**ch_procpp,	/* ptr to a (phys) proc ptr */
	thread_state_t	ch_state,
	unsigned int	ch_state_count,
	mach_port_t	rdir_port,
	mach_port_t	cdir_port,
	task_t		*ch_taskp,
	thread_t	*ch_threadp,
	mach_port_t	*cred_portp,	/* Rcv rt (or NULL) */
	mach_port_t	cred_port_name,	/* name new task will know port by */
	node_t		cred_cache_vector[],
	unsigned int	cred_cache_vector_size
#ifdef PARACORE
,	mach_port_t	exec_rdir_port,
	mach_port_t	exec_cdir_port
#endif /* PARACORE */
		)
{
	int			error;
	register struct	proc	*pp = u.u_procp;
	register struct proc	*p;
	kern_return_t		ret;
	int			dummy;

	/*
	 * Set up the root directory and the current directory
	 * in the uarea.
	 */
	set_proxy_from_vnode_port(rdir_port, &pp->p_utask.uu_rdirproxy);
	set_proxy_from_vnode_port(cdir_port, &pp->p_utask.uu_cdirproxy);

#ifdef PARACORE
	/*
	 * Set up the exec root and current directory
	 * in the utask.
	 */
	set_proxy_from_vnode_port(exec_rdir_port,
				  &pp->p_utask.uu_exec_utnd.utnd_rdir);
	set_proxy_from_vnode_port(exec_cdir_port,
				  &pp->p_utask.uu_exec_utnd.utnd_cdir);
#endif /* PARACORE */

	/*
	 * Translate the virtual layer's fork request into a
	 * physical fork and return the status.
	 */
	error = pproc_fork(pp, ch_pid, ch_procpp, NONODE,
			ch_state, ch_state_count);
	if (error) {
		*ch_taskp = MACH_PORT_NULL;
		*ch_threadp = MACH_PORT_NULL;
		goto out;
	}

	/*
	 * Return the names of the task and thread ports, which may or
	 * may not be needed by the caller.
	 */
	p = *ch_procpp;
	*ch_taskp = p->p_task;
	*ch_threadp = p->p_thread;

	/*
	 * Re-assign the new task's exception port to that of the local
	 * server, since the norma_task_create() within pproc_fork() will
	 * have resulted in the remote parent's exception port being inherited.
	 * and this is the exception port of the remote server. 
	 */
	ret = task_set_exception_port(p->p_task, ux_exception_port);
	if (ret != KERN_SUCCESS) {
		panic("rtask_pproc_fork: unable to set exception port");
	}

	/*
	 * Deal with credentials.  The fork mechanism has already set up
	 * a brand new credentials port and data structure.  But if the caller
	 * has provided a pre-existing credentials port receive right then
	 * we have to destroy what was already set up and re-create it anew,
	 * using the provided port.  Our name for the receive right is
	 * left in the cred field of the proc structure in either case.
	 */
	if( cred_portp != NULL ) {
		ret = credentials_deallocate(p->p_cred, &dummy, &dummy);
		if (ret != KERN_SUCCESS) {
			panic("rtask_pproc_fork: credentials_deallocate "
			       "failure, ret = 0x%x", ret);
		}
		p->p_cred = credentials_migrate_in(p->p_pid,
				p->p_pgid, p->p_sid, 
				p->p_rcred, p->p_utask.uu_cmask,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
				p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
				p->p_ruid, p->p_rgid,
				p->p_task,
				*cred_portp, 
				cred_cache_vector, cred_cache_vector_size);
		if (p->p_cred == MACH_PORT_NULL) {
			panic("rtask_pproc_fork: credentials_migrate_in "
			      "failure port=0x%x", *cred_portp);
		}
		*cred_portp = p->p_cred;
	}

	/*
	 * Next we insert a send right to the cred port into the
	 * newly-created task, and give that right the name the caller
	 * specified.
	 */
	ret = mach_port_insert_right(p->p_task,
				     cred_port_name,	/* name to assign */
				     p->p_cred,		/* name for Rcv rt */
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS) {
		panic("rtask_pproc_fork: cred insert_right failed");
	}

out:
	/*
	 * The dummy process no longer needs the memory mapped files
	 * (they have been inherited by the child process).
	 */
	vm_exit(pp);

	/*
	 * The dummy process no longer needs the current and root
	 * directory vnodes/ports (they have been inherited by the
	 * created process)
	 */
	remote_vrele(&pp->p_utask.uu_cdirproxy);
	remote_vrele(&pp->p_utask.uu_rdirproxy);
#ifdef PARACORE
	remote_vrele(&pp->p_utask.uu_exec_utnd.utnd_cdir);
	remote_vrele(&pp->p_utask.uu_exec_utnd.utnd_rdir);
#endif /* PARACORE */

	return(error);
}


/*
 * Unload information about the Parent task from the rf_data structure
 * (plus the ports and other items provided in the RPC to svr_rfork())
 * into the dummy parent structure that exists just during this call.
 */
void
rfork_pproc_unload_msg(
	struct proc	*p,
	struct rf_data	*rf_data,
	task_t		rf_par_task,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,
	char		*logname
#ifdef PARACORE
,	char		*exec_prg_name
#endif /* PARACORE */
	)
{
	register struct ucred	*credp;
	register int		i;

	/*
	 * If no proc structure is specified, use the current context.
	 */
	if (p == NULL)
		p = u.u_procp;

	/*
	 * First assign the ports we were passed to the structure.
	 * At the moment this is only the parent's task port.
	 */

	p->p_task = rf_par_task;
	
	/*
	 * Begin loading client's (parent) info into our proc structure.
	 */

	p->p_nice = rf_data->rf_p_nice;
	p->p_sigmask = rf_data->rf_p_sigmask;
	p->p_sigignore = rf_data->rf_p_sigignore;
	p->p_sigcatch = rf_data->rf_p_sigcatch;
	p->p_flag = rf_data->rf_p_flag;
	p->p_ruid = rf_data->rf_p_ruid;
	p->p_svuid = rf_data->rf_p_svuid;
	p->p_rgid = rf_data->rf_p_rgid;
	p->p_svgid = rf_data->rf_p_svgid;

	/* unload the credentials info */
	credp = p->p_rcred;
	credp->cr_uid = rf_data->rf_p_cr_uid;
#ifdef NX
	credp->cr_acctid = rf_data->rf_p_cr_acctid;
#endif /* NX */
	credp->cr_gid = rf_data->rf_p_cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		credp->cr_groups[i] = rf_data->rf_p_cr_groups[i];
	}
	credp->cr_ngroups = rf_data->rf_p_cr_ngroups;

#if defined(MACH_AFS)
	credp->cr_pag = rf_data->rf_p_cr_pag;
#endif

	/* continue with more proc info */
	p->p_pid = rf_data->rf_p_pid;	/* note this is the parent's pid */
	p->p_pgid = rf_data->rf_p_pgid;
	p->p_sid = rf_data->rf_p_sid;
	p->p_realtimer_coe = rf_data->rf_p_realtimer_coe; /* should fork zero this? */

	/*
	 * Now load info from the user structure.
	 */
	u.u_tsize = rf_data->rf_u_tsize;	/* info held in utask structure */
	u.u_dsize = rf_data->rf_u_dsize;
	u.u_ssize = rf_data->rf_u_ssize;
	u.u_text_start = (caddr_t)rf_data->rf_u_text_start;
	u.u_data_start = (caddr_t)rf_data->rf_u_data_start;
	u.u_stack_start = (caddr_t)rf_data->rf_u_stack_start;
	u.u_stack_end = (caddr_t)rf_data->rf_u_stack_end;
	u.u_stack_grows_up = rf_data->rf_u_stack_grows_up;
	u.u_outime = rf_data->rf_u_outime;	/* ???????? reinitted at fork time? */
	for (i=0; i<NSIG+1; ++i) {
		u.u_signal[i] = (sig_t)rf_data->rf_u_signal[i];
		u.u_sigmask[i] = rf_data->rf_u_sigmask[i];
	}
#ifdef i386
	u.u_sigreturn = (int(*)())rf_data->rf_u_sigreturn;
#endif
#ifdef multimax
	u.u_sigcatch = rf_data->rf_u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	u.u_sigtramp = rf_data->rf_u_sigtramp;
#endif
	u.u_sigonstack = rf_data->rf_u_sigonstack;
	u.u_sigintr = rf_data->rf_u_sigintr;
	u.u_oldmask = rf_data->rf_u_oldmask;
	u.u_sigstack.ss_sp = (char *)rf_data->rf_u_sigstack_ss_sp;
	u.u_sigstack.ss_onstack = rf_data->rf_u_sigstack_ss_onstack;
	u.u_cmask = rf_data->rf_u_cmask;
	u.u_ioch = rf_data->rf_u_ioch;	/* shouldn't this be zeroed on fork? */

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		u.u_timer[i] = rf_data->rf_u_timer[i];
	}

	u.u_prof.pr_lock = NULL;		/* get fresh lock */
	u.u_prof.pr_base = (short *)rf_data->rf_u_prof_base;
	u.u_prof.pr_size = (unsigned)rf_data->rf_u_prof_size;
	u.u_prof.pr_off = (unsigned)rf_data->rf_u_prof_off;
	u.u_prof.pr_scale = (unsigned)rf_data->rf_u_prof_scale;
	u.u_maxuprc = rf_data->rf_u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		u.u_rlimit[i] = rf_data->rf_u_rlimit[i];
	}

	u.u_shmsegs = rf_data->rf_u_shmsegs;
	u.u_argp = (char *)rf_data->rf_u_argp;
	u.u_envp = (char *)rf_data->rf_u_envp;
	u.u_arg_size = (u_short)rf_data->rf_u_arg_size;
	u.u_env_size = (u_short)rf_data->rf_u_env_size;

	/*
	 * Fill in the mmap area.
	 */
	{
		struct mmap_struct	*ms = &mmap_structs[0];
		mach_port_t		*pagerp = &mmap_pagers[0];

		for (i = 0; i < mmap_array_size; i++, ++ms, ++pagerp) {
			vm_mmap_add(p, ms->start, ms->length, *pagerp,
					ms->node, ms->offset, ms->flags);
		}
	}

	/*
	 * Fill in the string parameters
	 */
	bcopy(command_name, u.u_comm, MAXCOMLEN+1);	/* yes, +1 */
	bcopy(logname, u.u_logname, MAXLOGNAME);	/* no +1 here */

#ifdef PARACORE
	strcpy(p->p_utask.uu_exec_prg_name, exec_prg_name);
#endif /* PARACORE */
}


/*
 * Copy information into proc and user area from supplied structures.
 * Note that this routine is also used by checkpoint/restart during
 * exec_restart() and in this case port and mmap parameters will be NULL.
 */
void
migrate_pproc_unload_msg(
	struct proc	*p,
	struct mi_data	*mi_data,
	task_t		mi_old_task,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*command_name,
	char		*logname
#ifdef PARACORE
,	char		*exec_prg_name
#endif /* PARACORE */
	)
{
	register struct ucred	*credp;
	register int		i;

	/*
	 * If no proc structure is specified, use the current context.
	 */
	if (p == NULL)
		p = u.u_procp;

	/*
	 * First assign the ports we were passed to the structure.
	 * At the moment this is only the parent's task port.
	 */
	if (mi_old_task != MACH_PORT_NULL)
		p->p_task = mi_old_task;

	/*
	 * Begin loading client's (parent) info into our proc structure.
	 */
	p->p_nice = mi_data->mi_p_nice;
	p->p_sigmask = mi_data->mi_p_sigmask;
	p->p_sigignore = mi_data->mi_p_sigignore;
	p->p_sigcatch = mi_data->mi_p_sigcatch;
	p->p_flag = mi_data->mi_p_flag;
	p->p_ruid = mi_data->mi_p_ruid;
	p->p_svuid = mi_data->mi_p_svuid;
	p->p_rgid = mi_data->mi_p_rgid;
	p->p_svgid = mi_data->mi_p_svgid;

	/* unload the credentials info */
	credp = p->p_rcred;
	credp->cr_uid = mi_data->mi_p_cr_uid;
#ifdef NX
	credp->cr_acctid = mi_data->mi_p_cr_acctid;
#endif /* NX */
	credp->cr_gid = mi_data->mi_p_cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		credp->cr_groups[i] = mi_data->mi_p_cr_groups[i];
	}
	credp->cr_ngroups = mi_data->mi_p_cr_ngroups;

#if defined(MACH_AFS)
	credp->cr_pag = mi_data->mi_p_cr_pag;
#endif

	/* continue with more proc info */
	if (mi_old_task != MACH_PORT_NULL) {
		p->p_pid = mi_data->mi_p_pid;
		p->p_ppid = mi_data->mi_p_ppid;	/* don't forget parent pid */
		p->p_pgid = mi_data->mi_p_pgid;
		p->p_sid = mi_data->mi_p_sid;
	}
	p->p_realtimer_coe = mi_data->mi_p_realtimer_coe;

	/*
	 * Now load info from the user structure.
	 */
	u.u_tsize = mi_data->mi_u_tsize;
	u.u_dsize = mi_data->mi_u_dsize;
	u.u_ssize = mi_data->mi_u_ssize;
	u.u_text_start = (caddr_t)mi_data->mi_u_text_start;
	u.u_data_start = (caddr_t)mi_data->mi_u_data_start;
	u.u_stack_start = (caddr_t)mi_data->mi_u_stack_start;
	u.u_stack_end = (caddr_t)mi_data->mi_u_stack_end;
	u.u_stack_grows_up = mi_data->mi_u_stack_grows_up;
	u.u_outime = mi_data->mi_u_outime;
	for (i=0; i<NSIG+1; ++i) {
		u.u_signal[i] = (sig_t)mi_data->mi_u_signal[i];
		u.u_sigmask[i] = mi_data->mi_u_sigmask[i];
	}
#ifdef i386
	u.u_sigreturn = (int(*)())mi_data->mi_u_sigreturn;
#endif
#ifdef multimax
	u.u_sigcatch = mi_data->mi_u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	u.u_sigtramp = mi_data->mi_u_sigtramp;
#endif
	u.u_sigonstack = mi_data->mi_u_sigonstack;
	u.u_sigintr = mi_data->mi_u_sigintr;
	u.u_oldmask = mi_data->mi_u_oldmask;
	u.u_sigstack.ss_sp = (char *)mi_data->mi_u_sigstack_ss_sp;
	u.u_sigstack.ss_onstack = mi_data->mi_u_sigstack_ss_onstack;
	u.u_cmask = mi_data->mi_u_cmask;
	u.u_ioch = mi_data->mi_u_ioch;

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		u.u_timer[i] = mi_data->mi_u_timer[i];
	}

	u.u_prof.pr_lock = NULL;		/* get fresh lock */
	u.u_prof.pr_base = (short *)mi_data->mi_u_prof_base;
	u.u_prof.pr_size = (unsigned)mi_data->mi_u_prof_size;
	u.u_prof.pr_off = (unsigned)mi_data->mi_u_prof_off;
	u.u_prof.pr_scale = (unsigned)mi_data->mi_u_prof_scale;
	u.u_maxuprc = mi_data->mi_u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		u.u_rlimit[i] = mi_data->mi_u_rlimit[i];
	}

	u.u_shmsegs = mi_data->mi_u_shmsegs;
	u.u_svipc_flag = (u_char)mi_data->mi_u_svipc_flag;
	u.u_argp = (char *)mi_data->mi_u_argp;
	u.u_envp = (char *)mi_data->mi_u_envp;
	u.u_arg_size = (u_short)mi_data->mi_u_arg_size;
	u.u_env_size = (u_short)mi_data->mi_u_env_size;

	/*
	 * Fill in the mmap area.
	 */
	 if (mmap_structs != NULL) {
		struct mmap_struct	*ms = &mmap_structs[0];
		mach_port_t		*pagerp = &mmap_pagers[0];

		for (i = 0; i < mmap_array_size; i++, ++ms, ++pagerp) {
			vm_mmap_add(p, ms->start, ms->length, *pagerp,
					ms->node, ms->offset, ms->flags);
		}
	}

	/*
	 * Fill in the string parameters
	 */
	bcopy(command_name, u.u_comm, MAXCOMLEN+1);	/* yes, +1 */
	bcopy(logname, u.u_logname, MAXLOGNAME);	/* no +1 here */

#ifdef PARACORE
	if (exec_prg_name != NULL)
		strcpy(p->p_utask.uu_exec_prg_name, exec_prg_name);
#endif /* PARACORE */
}


/*
 * Setup more proc/uarea fields.
 * Note that this routine is also used by checkpoint/restart during
 * exec_restart() and in this case port parameters will be NULL.
 */
void
migrate_pproc_fix(
	struct proc	*procp,
	struct mi_data	*mi_data,
	task_t		old_task,
	mach_port_t	old_task_name,
	mach_port_t	*proc_portp)		/* becomes bootstrap port */
{
	int		ret;
	struct timeval	*timerp;
	extern int	realitexpire();

	/*
	 * Copy over the fields from the mi_data structure that were
	 * zeroed by pproc_fork().
	 */
	procp->p_cursig = mi_data->mi_p_cursig;
	procp->p_sig = mi_data->mi_p_sig;
	procp->p_realtimer_coe = mi_data->mi_p_realtimer_coe;
	procp->p_stopsig = mi_data->mi_p_stopsig;
	procp->p_realtimer = mi_data->mi_p_realtimer;
	procp->p_logdev = mi_data->mi_p_logdev;
	procp->sigwait = mi_data->mi_sigwait;
	procp->p_utask.uu_outime = mi_data->mi_u_outime;
	procp->p_utask.uu_ioch = mi_data->mi_u_ioch;
	procp->p_utask.uu_ru = mi_data->mi_u_ru;
	procp->p_utask.uu_cru = mi_data->mi_u_cru;

	/*
	 * Restart any outstanding realtime timer after converting back
	 * to absolute time from the migrated relative. This assumes that
	 * the migration itself takes no time!
	 */
	timerp = &procp->p_realtimer.it_value;
	untimeout(realitexpire, (caddr_t)procp);
	if (timerisset(timerp)) {
		TIME_READ_LOCK();
		timevaladd(timerp, &time);
		TIME_READ_UNLOCK();
		timeout(realitexpire, (caddr_t)procp, hzto(timerp));
	}

	/*
	 * If called with no old task, we're being used by the checkpoint/
	 * restart code to exec_restart() a process, and we're done.
	 */
	if (old_task == MACH_PORT_NULL)
		return;

	/*
	 * Get rid of newly-created proc port, since we need to use the
	 * receive right passed from the node the process migrated from.
	 */
	ret = mach_port_destroy(mach_task_self(), proc_to_port_lookup(procp));
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: mach_port_destroy failure");
	}

	/*
	 * Rename the passed receive right to match the one we just
	 * destroyed.  Then create a send right to the proc port,
	 * just like task_to_proc_enter() does when it sets up proc
	 * ports.
	 */
	ret = mach_port_rename(mach_task_self(), *proc_portp,
			(mach_port_t) procp);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: mach_port_rename failure "
				"ret=0x%x oldname=0x%x newname=0x%x",
				ret, *proc_portp, procp);
	}
	*proc_portp = proc_to_port_lookup(procp);

	ret = mach_port_insert_right(mach_task_self(),
				     proc_to_port_lookup(procp),
				     proc_to_port_lookup(procp),
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("migrate_pproc_fix: can't acquire send rights");

	ux_server_add_port(proc_to_port_lookup(procp));

	/*
	 * Finally make the proc port available to the newly-created task.
	 * As opposed to other ports, we don't insert the proc port with
	 * a passed name.  Instead we make it the bootstrap port.
	 */
	ret = task_set_bootstrap_port(procp->p_task,proc_to_port_lookup(procp));
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: cannot set new bootstrap port");
	}

	/*
	 * We are thoroughly committed now.  Suspend the old task forever,
	 * as it should never run again.  This must be done on this node,
	 * due to the fact that this node is the one that does the
	 * task_terminate() also.
	 */
	ret = task_suspend(old_task);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: cannot suspend old task");
	}

	/*
	 * Assign a send right to the old task's self_port to the newly-
	 * created task.  This lets the new task extract the port rights
	 * it needs, and then destroy the old task.  We use the passed
	 * name.
	 */
	ret = mach_port_insert_right(procp->p_task, 
				     old_task_name,
				     (mach_port_t) old_task, 
				     MACH_MSG_TYPE_MOVE_SEND);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: self_port insert failure");
	}

	/*
	 * Get rid of the extra send rights to the task and the thread
	 * that were allocated by pproc_fork().  They are not needed to
	 * pass back to the client node.
	 */
	ret = mach_port_deallocate(mach_task_self(),
				   (mach_port_t) procp->p_task);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: mach_port_deallocate(task) failure");
	}
	ret = mach_port_deallocate(mach_task_self(),
				   (mach_port_t) procp->p_thread);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: mach_port_deallocate(th) failure");
	}

	/*
	 * Start up the thread for the migrated process.  The startup
	 * code is expected to extract any port rights it needs from
	 * the old task.
	 */
	ret = thread_resume(procp->p_thread);
	if (ret != KERN_SUCCESS) {
		panic("migrate_pproc_fix: thread_resume failure");
	}
}


void
rexecve_pproc_unload_msg(
	struct proc	*p,
	struct re_data	*re_data,
	task_t		re_old_task,
	struct mmap_struct mmap_structs[],
	mach_port_t	mmap_pagers[],
	unsigned int	mmap_array_size,
	char		*logname)
{
	register struct ucred	*credp;
	register int		i;

	/*
	 * If no proc structure is specified, use the current context.
	 */
	if (p == NULL)
		p = u.u_procp;

	/*
	 * First assign the ports we were passed to the structure.
	 * At the moment this is only the parent's task port.
	 */
	
	p->p_task = re_old_task;

	/*
	 * Begin loading client's (parent) info into our proc structure.
	 */
	p->p_nice = re_data->re_p_nice;
	p->p_sigmask = re_data->re_p_sigmask;
	p->p_sigignore = re_data->re_p_sigignore;
	p->p_sigcatch = re_data->re_p_sigcatch;
	p->p_flag = re_data->re_p_flag;
	p->p_ruid = re_data->re_p_ruid;
	p->p_svuid = re_data->re_p_svuid;
	p->p_rgid = re_data->re_p_rgid;
	p->p_svgid = re_data->re_p_svgid;

	/* unload the credentials info */
	credp = p->p_rcred;
	credp->cr_uid = re_data->re_p_cr_uid;
#ifdef NX
	credp->cr_acctid = re_data->re_p_cr_acctid;
#endif /* NX */
	credp->cr_gid = re_data->re_p_cr_gid;
	for (i=0; i<NGROUPS; ++i) {
		credp->cr_groups[i] = re_data->re_p_cr_groups[i];
	}
	credp->cr_ngroups = re_data->re_p_cr_ngroups;

#if defined(MACH_AFS)
	credp->cr_pag = re_data->re_p_cr_pag;
#endif

	/* continue with more proc info */
	p->p_pid = re_data->re_p_pid;
	p->p_ppid = re_data->re_p_ppid;
	p->p_pgid = re_data->re_p_pgid;
	p->p_sid = re_data->re_p_sid;
	p->p_realtimer_coe = re_data->re_p_realtimer_coe;

	/*
	 * Now load info from the user structure.
	 */
	u.u_outime = re_data->re_u_outime;
	for (i=0; i<NSIG+1; ++i) {
		u.u_signal[i] = (sig_t)re_data->re_u_signal[i];
		u.u_sigmask[i] = re_data->re_u_sigmask[i];
	}
#ifdef i386
	u.u_sigreturn = (int(*)())re_data->re_u_sigreturn;
#endif
#ifdef multimax
	u.u_sigcatch = re_data->re_u_sigcatch;
#endif
#if defined(balance) || defined(mips) || defined(i860)
	u.u_sigtramp = re_data->re_u_sigtramp;
#endif
	u.u_sigintr = re_data->re_u_sigintr;
	u.u_oldmask = re_data->re_u_oldmask;
	u.u_cmask = re_data->re_u_cmask;
	u.u_ioch = re_data->re_u_ioch;

	/* handle timer stuff */
	for (i=0; i<3; ++i) {
		u.u_timer[i] = re_data->re_u_timer[i];
	}

	u.u_prof.pr_lock = NULL;		/* get fresh lock */
	u.u_maxuprc = re_data->re_u_maxuprc;

	/* handle array of rlimits structures */
	for (i=0; i<RLIM_NLIMITS; ++i) {
		u.u_rlimit[i] = re_data->re_u_rlimit[i];
	}

	u.u_shmsegs = re_data->re_u_shmsegs;
	u.u_svipc_flag = (u_char)re_data->re_u_svipc_flag;

	/*
	 * Fill in the mmap area.
	 */
	{
		struct mmap_struct	*ms = &mmap_structs[0];
		mach_port_t		*pagerp = &mmap_pagers[0];

		for (i = 0; i < mmap_array_size; i++, ++ms, ++pagerp) {
			vm_mmap_add(p, ms->start, ms->length, *pagerp,
					ms->node, ms->offset, ms->flags);
		}
	}

	/*
	 * Fill in the string parameter
	 */
	bcopy(logname, u.u_logname, MAXLOGNAME);	/* no +1 here */
}


struct rexecve_save {
	path_name_t	fname;
	int		fname_count;
	vm_address_t	arg_addr;
	vm_size_t	arg_size;
	int		arg_count;
	int		env_count;
	int		char_count;
	char		cfname[8*64];
	char		cfarg[8*64];
	int		entry[16];
	unsigned int	entry_count;
	auxv_mig_t	auxv_structs;
	char		auxv_strings[4096];
};


int
rexecve_pproc_exec(
	struct proc	*procp, 
	char		*fname,
	unsigned int	fname_count,
	vm_address_t	arg_addr,
	vm_size_t	arg_size,
	int		arg_count,
	int		env_count,
	unsigned int	char_count,
	struct re_data	*re_data, 
	task_t		old_task,
	mach_port_t	old_task_name,
	mach_port_t	*proc_portp)
{
	int		ret;
	int		error;
	struct nameidata *ndp = &u.u_nd;
	struct timeval	*timerp;
	extern int	realitexpire();

	/*
	 * Copy over the fields from the re_data structure that were
	 * zeroed by pproc_fork().
	 */
	procp->p_cursig = re_data->re_p_cursig;
	procp->p_sig = re_data->re_p_sig;
	procp->p_realtimer_coe = re_data->re_p_realtimer_coe;
	procp->p_stopsig = re_data->re_p_stopsig;
	procp->p_realtimer = re_data->re_p_realtimer;
	procp->p_logdev = re_data->re_p_logdev;
	procp->sigwait = re_data->re_sigwait;
	procp->p_utask.uu_outime = re_data->re_u_outime;
	procp->p_utask.uu_ioch = re_data->re_u_ioch;
	procp->p_utask.uu_ru = re_data->re_u_ru;
	procp->p_utask.uu_cru = re_data->re_u_cru;

	/*
	 * Restart any outstanding realtime timer after converting back
	 * to absolute time from the migrated relative. This assumes that
	 * the migration itself takes no time!
	 */
	timerp = &procp->p_realtimer.it_value;
	untimeout(realitexpire, (caddr_t)procp);
	if (timerisset(timerp)) {
		TIME_READ_LOCK();
		timevaladd(timerp, &time);
		TIME_READ_UNLOCK();
		timeout(realitexpire, (caddr_t)procp, hzto(timerp));
	}

	/*
	 * Get rid of the extra send rights to the task and the thread
	 * that were allocated by pproc_fork().  They are not needed to
	 * pass back to the client node.
	 */
	ret = mach_port_deallocate(mach_task_self(), 
				   (mach_port_t) procp->p_task);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: mach_port_deallocate(task) failure");
	}
	ret = mach_port_deallocate(mach_task_self(), 
				   (mach_port_t) procp->p_thread);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: mach_port_deallocate(thread) failure"); 
	}

	/*
	 * Perform the actual exec.
	 */
	{
		int arg[3];			/* arguments to execve */
		struct execr {			/* exec return arguments */
			char	*cfname;	/* shell file name */
			char	*cfarg;		/* shell args */
			int	*entry;		/* pointer to pc entry points */
			unsigned int *entry_count; /* number of entries */
		} rtv;
		struct rexecve_save *sv;
		struct proc *proc_save;

		if (procp->p_rexecve_save)
			(void) vm_deallocate(mach_task_self(), 
					     (vm_address_t)procp->p_rexecve_save,
					     sizeof(struct rexecve_save));
		ret = vm_allocate(mach_task_self(),
				  (vm_address_t *) &procp->p_rexecve_save,
				  sizeof(struct rexecve_save),
				  TRUE);
		if (ret != KERN_SUCCESS)
			panic("rexecve_pproc_exec: vm_allocate failure");
		sv = procp->p_rexecve_save;

		arg[0] = (int)fname;		/* file name for exec */
		arg[1] = 0;			/* handled in the emulator */
		arg[2] = 0;			/* handled in the emulator */

		rtv.cfname = sv->cfname;
		rtv.cfarg = sv->cfarg;
		rtv.entry = sv->entry;
		rtv.entry_count = &sv->entry_count;

		procp->p_auxv_structs = &sv->auxv_structs[0];
		procp->p_auxv_strings = sv->auxv_strings;

		strcpy(sv->fname, fname);
		sv->fname_count = fname_count;
		sv->arg_addr = arg_addr;
		sv->arg_size = arg_size;
		sv->arg_count = arg_count;
		sv->env_count = env_count;
		sv->char_count = char_count;

		proc_save = u.u_procp;
		u.u_procp = procp;
		u.u_nd.ni_utnd = &procp->p_utask.uu_utnd;
		unix_master();
		error = execve(procp, arg, &rtv);
		unix_release();
		u.u_nd.ni_utnd = &proc_save->p_utask.uu_utnd;
		u.u_procp = proc_save;

		if (error != ESUCCESS) {
			/*
			 * We must "unestablish" the credentials on this
			 * node and re-establish them on the original node.
			 *
			 * This code makes the assumption that the
			 * credentials cache (including the cache
			 * vector) cannot change between the time that the 
			 * credentials_migrate_in() call was made and 
			 * the credentials_migrate_out() call below.
			 * This may be incorrect for code supporting
			 * multi-threaded rexecve, depending upon
			 * the portential state of all the other
			 * threads.
			 */
			node_t cred_cache_vector[CREDENTIALS_CACHE_SIZE];
			node_t *cred_cache_vectorp;
			unsigned int cred_cache_vector_size;

			cred_cache_vector_size = CREDENTIALS_CACHE_SIZE;
			ret = credentials_migrate_out(procp->p_cred,
						      cred_cache_vector,
						      &cred_cache_vector_size);
			if (ret != KERN_SUCCESS && 
			    cred_cache_vector_size > CREDENTIALS_CACHE_SIZE) {
				while (ret != KERN_SUCCESS) {
					ret = vm_allocate(mach_task_self(),
					    (vm_address_t *)&cred_cache_vectorp,
					    cred_cache_vector_size * 
							sizeof(node_t),
					    TRUE);
					if (ret != KERN_SUCCESS)
						printf("rexecve_pproc_exec: "
						       "cannot allocate cache "
						       "vector, ret=0x%x; "
						       "retrying\n", ret);
				}
				ret = credentials_migrate_out(procp->p_cred,
						      cred_cache_vectorp,
						      &cred_cache_vector_size);
			}
			if (ret != KERN_SUCCESS)
				panic("rexecve_pproc_exex: "
				      "credentials_migrate_out failure");
			return(error);
		}
	}

	/*
	 * Get rid of the process port that the fork mechanism set up
	 * and create it anew, substituting the Rcv rt to the proc port we
	 * had on the old (client) node.  Port renaming is taken care of
	 * right here.
	 */
	ret = mach_port_destroy(mach_task_self(), proc_to_port_lookup(procp));
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: mach_port_destroy failure");
	}

	/*
	 * Put the old receive rights into the places previously
	 * occupied by the rights we just destroyed.  Then create
	 * a send right (of the same name, of course) just like
	 * task_to_proc_enter() does.
	 */
	ret = mach_port_rename(mach_task_self(), *proc_portp,
			(mach_port_t) procp);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: mach_port_rename failure"
				"ret=0x%x oldname=0x%x newname=0x%x",
				ret, *proc_portp, procp);
	}
	*proc_portp = proc_to_port_lookup(procp);

	ret = mach_port_insert_right(mach_task_self(),
				     proc_to_port_lookup(procp),
				     proc_to_port_lookup(procp),
				     MACH_MSG_TYPE_MAKE_SEND);
	if (ret != KERN_SUCCESS)
		panic("rexecve_pproc_exec: can't acquire send rights");

	ux_server_add_port(proc_to_port_lookup(procp));

	ret = task_set_bootstrap_port(procp->p_task,proc_to_port_lookup(procp));
	if (ret != KERN_SUCCESS)
		panic("rexecve_pproc_exec: cannot set new bootstrap port");

	/*
	 * We are thoroughly committed now.  Suspend the old task forever,
	 * as it should never run again.  This must be done on this node,
	 * due to the fact that this node is the one that does the
	 * task_terminate() also.
	 */
	ret = task_suspend(old_task);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: cannot suspend old task");
	}

	/*
	 * Assign the old task's send rights to the new task that is
	 * taking over the process.  This is so the new task can extract
	 * the port rights it needs, and then destroy the old task.
	 */
	ret = mach_port_insert_right(procp->p_task,
				     old_task_name,
				     (mach_port_t) old_task,
				     MACH_MSG_TYPE_MOVE_SEND);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_exec: mach_port_insert_right failure");
	}

	return(error);
}


void
rexecve_pproc_fix(
	struct proc	*procp)
{
	int ret;

	ret = thread_resume(procp->p_thread);
	if (ret != KERN_SUCCESS) {
		panic("rexecve_pproc_fix: thread_resume failure");
	}
}


rexecve_arrival(
	struct proc		*procp,
	char			*fname,			/* OUT */
	unsigned int		*fname_count,		/* OUT */
	vm_address_t		*arg_addr,		/* OUT */
	vm_size_t		*arg_size,		/* OUT */
	int			*arg_count,		/* OUT */
	int			*env_count,		/* OUT */
	unsigned int		*char_count,		/* OUT */
	char			*cfname,		/* OUT */
	char			*cfarg,			/* OUT */
	int			*entry,			/* OUT */
	unsigned int		*entry_count,		/* OUT */
	auxv_type_t		*auxv_structs,		/* OUT */
	unsigned int		*auxv_structs_count,	/* OUT */
	auxv_strings_t		auxv_strings,		/* OUT */
	unsigned int		*auxv_strings_count)	/* OUT */
{
	struct rexecve_save *sv = procp->p_rexecve_save;

	strcpy(fname, sv->fname);
	*fname_count = sv->fname_count;

	*arg_addr = sv->arg_addr;
	*arg_size = sv->arg_size;
	*arg_count = sv->arg_count;
	*env_count = sv->env_count;
	*char_count = sv->char_count;

	strcpy(cfname, sv->cfname);
	strcpy(cfarg, sv->cfarg);

	bcopy(sv->entry, entry, sv->entry_count*sizeof(int));
	*entry_count = sv->entry_count;

	bcopy(sv->auxv_structs, auxv_structs, 
			procp->p_auxv_structs_count*sizeof(auxv_type_t));
	*auxv_structs_count = procp->p_auxv_structs_count;

	bcopy(sv->auxv_strings, auxv_strings, procp->p_auxv_strings_count);
	*auxv_strings_count = procp->p_auxv_strings_count;

	(void) vm_deallocate(mach_task_self(), 
			     (vm_address_t) sv, 
			     sizeof(struct rexecve_save));
	procp->p_rexecve_save = 0;

	return(KERN_SUCCESS);
}
