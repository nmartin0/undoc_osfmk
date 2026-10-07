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
 * Copyright 1994 , 1995 Intel Corporation
 * All rights reserved
 *
 * $Id: dvp_pvpcore.c,v 1.9 1995/03/13 10:02:10 johannes Exp $
 *
 * HISTORY
 * $Log: dvp_pvpcore.c,v $
 * Revision 1.9  1995/03/13  10:02:10  johannes
 * dpvpop_pgrp_core: Check if pvp_core_data has been initialized.
 * 		  If not, core dumping will be suppressed for that process.
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: 12665
 *  Testing: test case of the PTS report
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 *
 * Revision 1.8  1994/12/19  17:58:09  johannes
 * pgrp_abort(): Lock of pgrp leader vproc isn't needed any more. It was needed
 *               in proc_abort and had to be taken before pgrp list lock.
 * proc_abort(): Use pproc_hold/pproc_release instead of VPROC_LOCK_EXCL/
 *               VPROC_UNLOCK_EXCL.
 *               An improvement to prevent pproc from exiting while accessing
 *               utask in order to ensure deallocating file ports done in
 *               pproc_exit().
 * Make the name conventions for vprocs more consistently.
 *
 *  Reviewer: Bob Yazzi
 *  Risk: High (locking issue)
 *  Benefit or PTS #: 11799
 *  Testing: test case, developer testing, corefile EAT, controlc EAT
 *  Module(s): svr/server/tnc/pvp.ops
 *             svr/server/tnc/dvp_pvpops.c
 *             svr/server/paracore/dvp_pvpcore.c
 *
 * Revision 1.7  1994/12/19  15:41:55  johannes
 * The server informs the allocator that the first faulting process starts
 * dumping core.
 *
 *  Reviewer: Scott Hahn
 *  Risk: High (several components involved)
 *  Benefit or PTS #: 11577
 *  Testing: developer testing, special testing by Simon Tsang,
 * 	  corefile EAT, sched EAT, rmcmd EAT, controlc EAT
 *  Module(s): svr/server/paracore/dvp_pvpcore.c
 * 	    svr/server/nx/nx.c, nx.defs
 * 	    usr/sbin/allocator/alloc.defs, misc_rpcs.c, init_appl.c,
 *                                allocator.c, pspart.c
 *             usr/include/nx/schedule.h
 * 	    usr/include/allocsys.h
 * 	    usr/bin/pspart
 *
 * Revision 1.6  1994/11/18  20:38:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.5  1994/10/25  17:07:37  johannes
 * added warnings (done with uprintf) to the user in case of wrong core actions
 * or problems to create/write core/allocinfo files
 *
 *  Reviewer: Karla (user view)
 *  Risk: low
 *  Benefit or PTS #: 10667 and warning for problems with core files
 *  Testing: developer testing
 *  Module(s): server/paracore/allocinfo.c
 * 	    server/paracore/core.c
 * 	    server/paracore/dump.c
 * 	    server/paracore/dvp_pvpcore.c
 *
 * Revision 1.4  1994/10/21  22:47:26  johannes
 * changed the initialization value of paracore_max_processes from 33 to 1
 *
 *  Reviewer: None
 *  Risk: Low
 *  Benefit or PTS #: The new default will disable a parallel core dump
 *                    of all but 1 application proccesses to avoid slowing
 *                    down a large machine or perturbing accounting
 *                    due to core dumping.
 *  Testing: None
 *  Module(s): server/paracore/dvp_pvpcore.c
 *
 * Revision 1.3  1994/07/27  14:46:14  johannes
 * introduced new boot magic PARACORE_MAX_PROCESSES
 * if limit exceeded change actions that only first faulting process dumps core
 * if limit is zero don't create a core directory
 * negative limit is unlimited
 *
 *  Reviewer: Stefan Tritscher
 *  Risk: Low
 *  Benefit or PTS #: limit the number of processes to dump core
 *  Testing: developer
 *  Module(s): server/uxkern/boot_config.c
 *             server/paracore/dvp_pvpcore.c
 *
 */

#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <kern/kalloc.h>

#ifdef OSF1_SERVER
#include <tnc/dpvproc.h>
#endif

#include <paracore/core_types.h>

 /* 
  * max number of processes to dump core
  * set by a boot magic
  * 1 is used as default 
  * i.e. it will be exceeded for any parallel application and
  * at most the first faulting process will dump core
  */
long paracore_max_processes = 1;


/*
 * NAME:	dpvpop_pgrp_core
 *
 * FUNCTION:	Creates or cleans up core directory
 *		Aborts other members of the pgrp if desired
 *		Returns core directory path
 *
 * RETURNS:	ESRCH, if specified vproc is not a process group leader.
 *		ESUCCESS on completion.
 */
int	dpvpop_pgrp_core(
	struct vproc	*vpgrp,		/* group leader vproc of application */
	pid_t		fault_pid,	/* faulting process' pid */
	mach_port_t	cred,		/* credentials port */
	int		*action,	/* OUT: specifies how to proceed */
	mach_port_t	*coredir,	/* OUT: coredir vnode */
	mach_port_t	*rootdir,	/* OUT: rootdir vnode */
	boolean_t	*ports_copied,	/* OUT: for ports deallocation */
	boolean_t	*first_faulting)/* OUT: was first faulting process */
{
	struct pvproc   *pvp;
	int		error;
	mach_port_t	coredir_port;
	boolean_t	was_first;
	int		action_other;
	kern_return_t	ret;


	CORE_DEBUGF("Enter dpvpop_pgrp_core: pgid=%d, fault_pid=%d\n",
		vpgrp->vp_pid, fault_pid);

	/*
	 * Find corresponding pvproc structure.
	 */
	pvp = PVP(vpgrp);
	
	/*
	 * Check if pvp_core_data has been initialized.
	 * If it is not initialized, it is not a valid NX proxy process,
	 * but a NX application process which left the application
	 * with setpgid. In this case core dumping is not supported.
	 * This results in MACH_PORT_NULL as core directory and
	 * CORE_ACTION_KILL as core action. (fix for PTS-12665)
	 */
	if ( pvp->pvp_core_data == NULL ) {
		CORE_DEBUGF("dpvpop_pgrp_core: pvp_core_data=NULL => no core dump\n");
		*action = CORE_ACTION_KILL;
		*coredir = MACH_PORT_NULL;
		*rootdir = MACH_PORT_NULL;
		*ports_copied = FALSE;
		*first_faulting = FALSE;
		
		CORE_DEBUGF("Leave dpvpop_pgrp_core\n");
		return (ESUCCESS);
	}

	ASSERT(pvp->pvp_core_data != NULL);
	
	/*
	 * Initialize OUT parameters to reasonable defaults.
	 */
	*coredir = MACH_PORT_NULL;
	*rootdir = MACH_PORT_NULL;

	/*
	 * Acquire the core lock.
	 */
	VPROC_CORE_LOCK_EXCL(vpgrp, "dpvpop_pgrp_core");

	if ( ! pvp->pvp_core_data->core_dumped ) {

		if (paracore_max_processes == 0 ||
		    (pvp->pvp_core_data->core_action_first == CORE_ACTION_KILL &&
		     pvp->pvp_core_data->core_action_fault == CORE_ACTION_KILL &&
		     (pvp->pvp_core_data->core_action_other == CORE_ACTION_KILL ||
		      pvp->pvp_core_data->core_action_other == CORE_ACTION_CONT)))
		     	/*
		     	 * No core directory has to be created 
		     	 * because no process have to dump core.
		     	 */
			coredir_port = MACH_PORT_NULL;
		
		else {
			boolean_t	too_many_procs = FALSE;
			mach_port_t	cred_orig;
			
			/*
			 * Go through the pgrp list and 
			 * decide if to many processes to dump core.
			 */
			if (paracore_max_processes > 0) {
				long		num_processes = 0;
				struct vproc	*vp;
						
				/*
				 * Lock the process group list.
				 */
				VPROC_LOCK_PGRP_LIST_SHARED(vpgrp, "dpvpop_pgrp_core");
				
				for ( vp = PVP(vpgrp)->pvp_head_pgrpl;
					vp != NULL; 
					vp = PVP(vp)->pvp_pgrpl ) {
					num_processes++;
					if (num_processes >
					    paracore_max_processes) {
						too_many_procs = TRUE;
						break;
					}
				}
				
				/*
				 * Unlock the process group list.
				 */
				VPROC_UNLOCK_PGRP_LIST_SHARED(vpgrp, "dpvpop_pgrp_core");
			}
			
			/*
			 * If to many processes to dump core,
			 * change actions that only first faulting
			 * process will dump core.
			 */
			 if (too_many_procs) {
			 	pvp->pvp_core_data->core_action_fault = CORE_ACTION_KILL;
			 	if (pvp->pvp_core_data->core_action_other != CORE_ACTION_CONT)
			 		pvp->pvp_core_data->core_action_other = CORE_ACTION_KILL;
			 }

			/*
			 * Reset u.uu_nd.ni_utnd to point to values in
			 * pvp_core_data->utnd that have been saves on nx_initve().
			 */

			u.uu_nd.ni_utnd = &pvp->pvp_core_data->utnd;
			
			/*
			 * Set u.uu_procp->p_cred to point to value of
			 * faulting process.
			 */
			cred_orig = u.uu_procp->p_cred;
			u.uu_procp->p_cred = cred;
	
			/*
			 * Try to create or clean up the core directory.
			 */
			coredir_port = setup_core_dir(pvp->pvp_core_data->env_core_path);
	
			if ( coredir_port == MACH_PORT_NULL ) {
				printf("dpvpop_pgrp_core: Set up of core directory failed\n");
			}
		
			/*
			 * Reset u.uu_procp->p_cred to original value.
			 */
			u.uu_procp->p_cred = cred_orig;
		}
		/*
		 * Save core directory port in pvproc structure.
		 */
		pvp->pvp_core_data->core_directory = coredir_port;

		/*
		 * Set pvp_core_dumped flag in pvproc structure to TRUE.
		 */
		pvp->pvp_core_data->core_dumped = TRUE;

		/*
		 * Indicate that this has been the first faulting process.
		 */
		was_first = TRUE;

		/*
		 * Set desired action for other processes in application.
		 */
		action_other = pvp->pvp_core_data->core_action_other;
	}
	else {
		/*
		 * This has not been the first faulting process
		 */
		was_first = FALSE;
	}

	
	/*
	 * Release the core lock.
	 */
	VPROC_CORE_UNLOCK_EXCL(vpgrp, "dpvpop_pgrp_core");

	/*
	 * Inform the allocator about core dumping, 
	 * if this was the first faulting process.
	 */
	if ( was_first ) {
		ret = nx_report_allocator_paracore(vpgrp->vp_pid);
		if ( ret != KERN_SUCCESS )
			printf("nx_report_allocator_paracore failed(0x%x)\n", ret);
	}
	
	/*
	 * Abort the other processes in the process group if this
	 * was the first process to fault and if requested in action_other.
	 */
	if ( was_first && action_other != CORE_ACTION_CONT ) {
		(void) PVPOP_PGRP_ABORT(vpgrp, fault_pid, FALSE, action_other,
					pvp->pvp_core_data->core_directory,
					pvp->pvp_core_data->utnd.utnd_rdir.vpx_port);
	}

	/*
	 * Set return values.
	 */
	*coredir = pvp->pvp_core_data->core_directory;
	*rootdir = pvp->pvp_core_data->utnd.utnd_rdir.vpx_port;
	if ( was_first ) {
		*action = pvp->pvp_core_data->core_action_first;
	}
	else {
		*action = pvp->pvp_core_data->core_action_fault;
	}
	*ports_copied = FALSE;	/* change by SERVER_POST-OP */
	*first_faulting = was_first;
	
	CORE_DEBUGF("Leave dpvpop_pgrp_core\n");
	return (ESUCCESS);
}


/*
 * NAME:	dpvpop_pgrp_abort
 *
 * FUNCTION:	Aborts all members of a process group
 *
 * RETURNS:	ESRCH, if specified vproc is not a process group leader.
 *		ESUCCESS on completion.
 */
int	dpvpop_pgrp_abort(
	struct vproc	*vpgrp,		/* group leader vproc of application */
	pid_t		pid,		/* faulting process' pid */
	boolean_t	abort_self,	/* abort caller also ? */
	int		action_other,	/* desired action for other members */
	mach_port_t	coredir,	/* coredir vnode port */
	mach_port_t	rootdir)	/* rootdir vnode port */
{
	struct vproc	*vp;
	int		error;
	int		nsuccesses;
	int		pid_array_size;
	pid_t		local_pid_array[VP_STACK_ARRAY_SIZE];
	pid_t		*pid_array;
	int		npids;


	CORE_DEBUGF("Enter dpvpop_pgrp_abort\n");
	error = ESRCH;

	/*
	 * Check if process group leader.
	 */
	if ( !(PVP(vpgrp)->pvp_flag&PV_PGRPLEADER) ) {
		printf("dpvpop_pgrp_abort: Not a process group leader\n");
		return(ESRCH);
	}

	/*
	 * Lock the process group list.
	 */
	VPROC_LOCK_PGRP_LIST_SHARED(vpgrp, "dpvpop_pgrp_abort");

	/*
	 * Go through the pgrp list and calculate number of vprocs to be
	 * aborted.
	 */
	pid_array_size = 0;
	for ( vp = PVP(vpgrp)->pvp_head_pgrpl;
		vp != NULL; 
		vp = PVP(vp)->pvp_pgrpl ) {
		pid_array_size++;
	}

	/*
	 * Check if we are to use spanning trees abort all processes,
	 * or if we should just do it sequentially.
	 */
	if ( pid_array_size > SPANNING_TREE_BENEFITS ) {

		/*
		 * If we can use a stack array for the pid list, do so. 
		 * Otherwise, allocate one.
		 */
		if ( pid_array_size > VP_STACK_ARRAY_SIZE ) {
			pid_array = (pid_t *) kalloc(pid_array_size * sizeof(pid_t));
			if ( pid_array == 0 ) {
				/*
				 * Oops got no memory.
				 */
				goto sequential;
			}
		}
		else {
			/*
			 * Use local array.
			 */
			pid_array = local_pid_array;
		}

		/*
		 * Fill in the array, skipping calling pid if requested
		 */
		npids = 0;
		for ( vp = PVP(vpgrp)->pvp_head_pgrpl;
			vp != NULL; 
			vp = PVP(vp)->pvp_pgrpl ) {
                	if ( !abort_self && vp->vp_pid == pid ) {
				/*
				 * Skip faulting pid.
				 */
                        	continue;
			}
			pid_array[npids] = vp->vp_pid;
			npids++;
		}
		
		/*
		 * Abort all the processes in the array.
		 */
		error = PVPOP_PROC_ABORT_MULTI(VPROCPTR(pid_array[0]),
						action_other,
						coredir,
						rootdir,
						0,
						pid_array, 
						npids,
						NULL);

		/*
		 * Free any memory we allocated.
		 */
		if ( pid_array != local_pid_array ) {
			kfree(pid_array, pid_array_size * sizeof(pid_t));
		}

		/*
		 * The EAGAIN error means that we couldn't use a spanning
		 * tree - revert to the slow way of signalling.
		 */
		if ( error == EAGAIN ) {
			goto sequential;
		}

		/*
		 * Unlock the process group list.
		 */
        	VPROC_UNLOCK_PGRP_LIST_SHARED(vpgrp, "dpvpop_pgrp_abort");

		CORE_DEBUGF("Leave dpvpop_pgrp_abort\n");
		return(error);
	}

	/*
	 * Go through the list sequentially, signalling each member.
	 */
sequential:
	CORE_DEBUGF("dpvpop_pgrp_abort: NOT using spanning tree\n");
	nsuccesses = 0;
	for ( vp = PVP(vpgrp)->pvp_head_pgrpl;
		vp != NULL;
		vp = PVP(vp)->pvp_pgrpl ) {

		if ( !abort_self && vp->vp_pid == pid ) {
			/*
			 * Skip calling pid.
			 */
			continue;
		}

		/*
		 * Abort the process.
		 */
		error = PVPOP_PROC_ABORT(vp, action_other, coredir, rootdir);
		if ( error == ESUCCESS ) {
			nsuccesses++;
		}
        }

	/*
	 * Unlock the process group list.
	 */
	VPROC_UNLOCK_PGRP_LIST_SHARED(vpgrp, "dpvpop_pgrp_abort");

	CORE_DEBUGF("Leave dpvpop_pgrp_abort\n");
	return (nsuccesses ? ESUCCESS : error);
}


/*
 * NAME:	dpvpop_proc_abort
 *
 * FUNCTION:	Aborts a specific process
 *
 * RETURNS:	ESRCH, if specified process not found
 *		ESUCCESS on completion.
 */
int	dpvpop_proc_abort(
	struct vproc	*vp,		/* vproc to abort */
	int		action,		/* desired action */
	mach_port_t	coredir,	/* coredir vnode port */
	mach_port_t	rootdir)	/* rootdir vnode port */
{
	struct pvproc	*pvp = PVP(vp);
	struct proc	*p;
	struct utask    *utask;
	pid_t		tampid;
	int		error;


	CORE_DEBUGF("Enter dpvpop_proc_abort\n");

	/*
	 * Check if process has to be aborted.
	 */
	if ( pvp->pvp_flag & PV_EXITING || pvp->pvp_flag & PV_SZOMB ) {
		/*
		 * Return with doing nothing.
		 */
		CORE_DEBUGF("Leave dpvpop_proc_abort, proc is EXITING or ZOMBIE\n");
		return(ESRCH); 
		/* ports will be deallocated automatically */
	}
	
	/*
	 * Make sure the proc is still alive, and cannot completely die
	 * (be reaped) while we're accessing it.
	 */
	p = pvp->pvp_pproc;
	error = pproc_hold(p, vp->vp_pid);
	if (error != ESUCCESS) {
		/*
		 * Return with doing nothing.
		 */
		CORE_DEBUGF("Leave dpvpop_proc_abort, pproc_hold failed\n");
		return(ESRCH);
		/* ports will be deallocated automatically */
	}

	/*
	 * Look if the application has a TAM assigned to it.
	 */
	tampid = nx_lookup_tam(p->p_pgid);

	/*
	 * If there is a TAM do not kill it.
	 */
	if ( tampid == p->p_pid ) {
		pproc_release(p, vp->vp_pid);
		/*
		 * Return with doing nothing.
		 */
		CORE_DEBUGF("Leaving dpvpop_proc_abort without killing TAM %d\n", tampid);
		return(ESRCH);
		/* ports will be deallocated automatically */
	}

	/*
	 * Find corresponding utask.
	 */
	utask = &p->p_utask;

	/*
	 * Lock proc and test for exiting before writing to utask
	 * to avoid exiting without deallocating ports.
	 */
	simple_lock(&p->p_lock);
	if (p->p_flag & SWEXIT) {
		simple_unlock(&p->p_lock);
		pproc_release(p, vp->vp_pid);
		/*
		 * Return with doing nothing.
		 */
		CORE_DEBUGF("Leave dpvpop_proc_abort, p_flag & SWEXIT\n");
		return(ESRCH); 
		/* ports will be deallocated automatically */
	}
	
	/*
	 *  Store coredir and action in utask.
	 */
	ASSERT(action != CORE_ACTION_CONT);
	utask->uu_core_action = action;
	utask->uu_core_directory = coredir;
	utask->uu_root_directory = rootdir;

	/*
	 * Unlock proc after writing to utask.
	 */
	simple_unlock(&p->p_lock);

	/*
	 * Release the pproc after writing to utask.
	 */
	pproc_release(p, vp->vp_pid);

	/*
	 * Terminate the process using SIGKILL.
	 */
	CORE_DEBUGF("dpvpop_pgrp_abort: Send SIGKILL in dpvpop_proc_abort\n");
	error = VPOP_SIGPROC(vp, SIGKILL, 0, 0);

	CORE_DEBUGF("Leave dpvpop_pgrp_abort\n");
	return (error);
}


/*
 * NAME:	dpvpop_proc_abort_multi
 *
 * FUNCTION:	Aborts a group of processes, using a spanning tree and
 *		asynchronous techniques.
 *
 *		To call this routine, the "h" parameter should be "NULL" and
 *		the pid_array_index parameter should be "0". These values
 *		are non-zero only for recursive calls to the routine.
 *
 * RETURNS:	EAGAIN, if spanning tree doesn't work.
 *		ESUCCESS on completion.
 */
int	dpvpop_proc_abort_multi(
	struct vproc	*vp,			/* (local) vproc to abort */
	int		action,			/* desired action */
	mach_port_t	coredir,		/* coredir vnode port */
	mach_port_t	rootdir,		/* rootdir vnode port */
	int		pid_array_index,	/* index of vp in pid_array */
	pid_t		pid_array[],		/* full array of pids */
	int		pid_array_count,	/* entries in pid_array */
	msg_handle_t	*h)		/* message handle (unused for dpvp) */
{
	struct vproc	*w;
	int		error, abort_error;
	int		nsuccesses = 0;
	register int	i;
	int		entry_array_count = VP_STACK_ARRAY_SIZE;
	int		entry_array[VP_STACK_ARRAY_SIZE];
	int		error_array[VP_STACK_ARRAY_SIZE];
	msg_handle_t	handle_array[VP_STACK_ARRAY_SIZE];


	CORE_DEBUGF("Enter dpvpop_proc_abort_multi\n");
	ASSERT(vp->vp_pid == pid_array[pid_array_index]);

	/*
	 * Find the entries in the pid array that are below this entry
	 * in the spanning tree.
	 */
	error =  get_spanning_tree(pid_array_count, pid_array_index,
				   entry_array, &entry_array_count);
	if (error) {
		CORE_DEBUGF("Leave dpvpop_proc_abort_multi\n");
		return(EAGAIN);
	}

	/*
	 * Perform the asynchronous "send" of the message to all the
	 * processes that are below this entry in the spanning tree.
	 */
	for ( i = 0; i < entry_array_count; i++ ) {
		w = LOCATE_VPROC_PID(pid_array[entry_array[i]]);
		ASSERT(w);
		error_array[i] = PVPOP_PROC_ABORT_MULTI_SEND(w,
							action,
							coredir,
							rootdir,
							entry_array[i],
							pid_array, 
							pid_array_count,
							&handle_array[i]);
	}

	/*
	 * Abort the local process.
	 */
	abort_error = PVPOP_PROC_ABORT(vp, action, coredir, rootdir);

	/*
	 * Perform the asynchronous "receive" of replies to the message for 
	 * all the processes that are below this entry in the spanning tree.
	 */
	for ( i=0; i < entry_array_count; i++ ) {
		w = VPROCPTR(pid_array[entry_array[i]]);
		ASSERT(w);
		if ( error_array[i] == 0 ) {
			error = PVPOP_PROC_ABORT_MULTI_RECEIVE(w,
							action,
							coredir,
							rootdir,
							entry_array[i],
							pid_array, 
							pid_array_count,
							&handle_array[i]);
			if ( error == ESUCCESS ) {
				nsuccesses++;
			}
		}
		else {
			error = error_array[i];
		}

		VPROC_RELEASE(w, "dpvpop_proc_abort_multi");
	}

	CORE_DEBUGF("Leave dpvpop_proc_abort_multi\n");
	if ( abort_error )
		return (abort_error);
		/* ports will be deallocated automatically */
	else
		return (nsuccesses ? ESUCCESS : error);
}


/*
 * NAME:	init_pvp_core_data
 *
 * FUNCTION:	Initializes a pvp_core_data structure
 *
 * RETURNS:	pointer to the initialized pvp_core_data structure.
 */
pvp_core_data_t	*init_pvp_core_data(struct vproc	*vp)
{
	struct utask    *utask;
	pvp_core_data_t	*pvp_core_data;
	kern_return_t	ret;


	CORE_DEBUGF("Enter init_pvp_core_data\n");
	/*
	 * Allocate the memory.
	 */
	pvp_core_data = (pvp_core_data_t *) kalloc(sizeof(pvp_core_data_t));

	/*
	 * Copy utnd from utask.
	 */
	utask = &PVP(vp)->pvp_pproc->p_utask;
	pvp_core_data->utnd = utask->uu_utnd;
	vnode_proxy_init(&pvp_core_data->utnd.utnd_cdir);
	vnode_proxy_init(&pvp_core_data->utnd.utnd_rdir);

	/*
	 * No one faulted yet.
	 */
	pvp_core_data->core_dumped = FALSE;

	/*
	 * No core directory yet.
	 */
	pvp_core_data->core_directory = MACH_PORT_NULL;

	/*
	 * Initialize the lock.
	 */
	pvp_lock_init(&pvp_core_data->core_lock);

	/*
	 * Get the CORE_PATH environment.
	 */
	get_core_path(PVP(vp)->pvp_pproc, 
		      pvp_core_data->env_core_path,
		      sizeof pvp_core_data->env_core_path);

	/*
	 * Get CORE_ACTION_OTHER.
	 * Default is CORE_ACTION_KILL.
	 */
	pvp_core_data->core_action_other = CORE_ACTION_KILL;
	get_core_action(PVP(vp)->pvp_pproc, 
			"CORE_ACTION_OTHER",
			&pvp_core_data->core_action_other);

	if (u.u_rlimit[RLIMIT_CORE].rlim_cur == 0) {
		/*
		 * No core dumping desired.
		 */
		pvp_core_data->core_action_first = CORE_ACTION_KILL;
		pvp_core_data->core_action_fault = CORE_ACTION_KILL;
	 	if (pvp_core_data->core_action_other != CORE_ACTION_CONT)
	 		pvp_core_data->core_action_other = CORE_ACTION_KILL;
	}
	else {
		/*
		 * Get CORE_ACTION_FIRST.
		 * Default is CORE_ACTION_FULL.
		 */
		pvp_core_data->core_action_first = CORE_ACTION_FULL;
		get_core_action(PVP(vp)->pvp_pproc, 
				"CORE_ACTION_FIRST",
				&pvp_core_data->core_action_first);
		if (pvp_core_data->core_action_first == CORE_ACTION_CONT) {
			uprintf("Invalid core action, \"CORE_ACTION_FIRST=CONT\", ignored.\n");
			pvp_core_data->core_action_first = CORE_ACTION_FULL;
		}
		
		/*
		 * Get CORE_ACTION_FAULT.
		 * Default is CORE_ACTION_KILL.
		 */
		pvp_core_data->core_action_fault = CORE_ACTION_KILL;
		get_core_action(PVP(vp)->pvp_pproc, 
				"CORE_ACTION_FAULT",
				&pvp_core_data->core_action_fault);
		if (pvp_core_data->core_action_fault == CORE_ACTION_CONT) {
			uprintf("Invalid core action, \"CORE_ACTION_FAULT=CONT\", ignored.\n");
			pvp_core_data->core_action_fault = CORE_ACTION_KILL;
		}
	}	
	
	/*
	 * We are done.
	 */
	CORE_DEBUGF("Leave init_pvp_core_data\n");
	return (pvp_core_data);
}


/*
 * NAME:	free_pvp_core_data
 *
 * FUNCTION:	Frees a pvp_core_data structure
 *
 * RETURNS:	NONE.
 */
void	free_pvp_core_data(pvp_core_data_t	*pvp_core_data)
{
	kern_return_t	ret;
	
	
	ASSERT(pvp_core_data != NULL );
	CORE_DEBUGF("Enter free_pvp_core_data\n");

	/*
	 * Release the proxy vnodes in pvp_core_data->utnd.
	 */
	remote_vrele(&pvp_core_data->utnd.utnd_cdir);
	remote_vrele(&pvp_core_data->utnd.utnd_rdir);

	/*
	 * Deallocate the core_directory port.
	 */
	if ( pvp_core_data->core_directory != MACH_PORT_NULL ) {
		ret = mach_port_deallocate(mach_task_self(),
			pvp_core_data->core_directory);
		if ( ret != KERN_SUCCESS ) {
			printf("free_pvp_core_data: Could not deallocate core_directory port (error=0x%x)\n", ret);
		}
	}

	/*
	 * Free the memory.
	 */
	kfree(pvp_core_data, sizeof(pvp_core_data_t));
	CORE_DEBUGF("Leave free_pvp_core_data\n");
}
