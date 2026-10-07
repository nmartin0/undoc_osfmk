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
 * Copyright (c) 1993-1995, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: chkpnt_vproc.c,v $
 * Revision 1.4  1995/02/01  21:39:45  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.3  1994/11/18  20:43:04  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1994/07/27  16:54:22  johannes
 * In chkpnt_get_server_state() the extended migrate_pproc_load_msg() is
 * called with dummy parameters.
 *
 * In restart_set_server_state() the extended migrate_pproc_unload_msg() is
 * called with dummy parameters.
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
 * Revision 1.1  1994/03/14  02:04:22  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 2.3  93/11/10  12:07:20  slk
 * *** empty log message ***
 * 
 * Revision 2.1.1.4  93/08/13  13:07:35  hao
 * 	Fixed code so that if restart fails to reset directory ports, we
 * 	exit instead of continue.
 * 
 * Revision 2.1.1.3  93/08/06  07:25:46  chrisp
 * 	Call chkpnt_check_regions() after writing the .stat file so that
 * 		a failure return of ENOSYS can be easily inferred from the
 * 		contents of the checkpoint directory.
 * 
 * Revision 2.1.1.2  93/06/25  08:02:02  chrisp
 * 	Revision 3.15  93/06/15  15:01:14  hao2
 * 	Make a call to chkpnt_check_regions() to check for extra memory
 * 	regions that can not be chkpnted.
 * 
 * Revision 2.1.1.1  93/06/10  11:49:37  chrisp
 * 	Revision 3.14  93/06/04  11:25:02  chrisp
 * 	".state" becomes ".stat".
 * 	CHKPNT_KILL option added together asynchronous core file writing.
 * 
 * 	Revision 3.13  93/05/25  09:31:23  chrisp
 * 	Use migrate_pproc_{load,unload}_msg() instead of checkpoint/restart
 * 		specific versions.
 * 
 * 	Revision 3.12  93/05/19  10:41:26  chrisp
 * 	Add function-level comments.
 * 
 * 	Revision 3.11  93/04/29  08:22:47  chrisp
 * 	Remove forkfamily() - it's all handled by rforkmulti code.
 * 
 * 	Revision 3.10  93/04/27  14:07:53  hao2
 * 	Root/current directories should have been set already.  No longer need to
 * 	be MACH_PORT_NULL.
 * 
 * 	Revision 3.9  93/04/26  10:40:58  hao2
 * 	Now exec_restart() calls restart_pproc_context() to set the root/current
 * 	directories.
 * 
 * 	Revision 3.8  93/04/22  08:53:19  chrisp
 * 	File naming change: ".image" becomes ".core" and node.pgid.ppid.pid
 * 		becomes pid.ppid.pgid.node so that sorting can be exploited.
 * 
 * 	Revision 3.7  93/04/17  13:17:22  chrisp
 * 	Complete single node, single process case of forkfamily().
 * 
 * 	Revision 3.6  93/04/12  15:19:20  chrisp
 * 	Changes to obtain ctty device and node.
 * 
 * 	Revision 3.5  93/04/08  11:53:16  chrisp
 * 	Add support for exec_restart() and include stub for forkfamily().
 * 
 * 	Revision 3.4  93/03/29  13:29:22  chrisp
 * 	Add ppid field to checkpoint filenames.
 * 
 * 	Revision 3.3  93/03/19  15:38:55  chrisp
 * 	Change checkpoint filename format to <node>.<pgid>.<pid>.{state,image}.
 * 
 * 	Revision 3.2  93/03/19  09:44:14  chrisp
 * 	Replace struct assignment of thread state to a bcopy().
 * 
 * 	Revision 3.1  93/03/16  07:36:27  chrisp
 * 	Extend chkpnt_sync() to include full emulator and server state.
 * 
 * 	Revision 3.0  93/02/17  10:44:42  chrisp
 * 	First draft.
 * 
 * 	$EndLog$
 * 
 */


#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <tnc/chkpnt.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/syscall.h>
#include <uxkern/proc_to_task.h>

/*
 * This file contains server routines implementing checkpoint and restart
 * functions at the VPROC level. 
 */

/*
 * Forward references:
 */
int restart_set_server_state(struct vproc *vp, chkpnt_proc_state_t *ps);
int chkpnt_get_server_state( struct vproc *vp, chkpnt_proc_state_t *ps);

/*
 * chkpnt_sync() is responsible for creating the checkpoint files. The
 * received emulator file state is written into .stat together with
 * internal server state, and core is written to an associated .core file.
 */
int
chkpnt_self(
	struct vproc		*vp,
	int			flags,
	node_t			node,
	thread_state_t		cur_state,
	unsigned int		cur_state_count,
	chkpnt_file_state_t	*fsp)
{
	struct pvproc		*pvp = PVP(vp);
	int			error = ESUCCESS;
	kern_return_t		ret;
	path_name_t		image_filename;
	path_name_t		state_filename;
	chkpnt_proc_state_t	*psp = NULL;

	sprintf(&image_filename, "/chkpnt/%s.%d/%d.%d.%d.%d.core",
		flags & CHKPNT_FAMILY ? "pgrp" : "proc",
		flags & CHKPNT_FAMILY ? pvp->pvp_pgid : vp->vp_pid,
		vp->vp_pid, pvp->pvp_ppid, pvp->pvp_pgid, this_node);
	sprintf(&state_filename, "/chkpnt/%s.%d/%d.%d.%d.%d.stat",
		flags & CHKPNT_FAMILY ? "pgrp" : "proc",
		flags & CHKPNT_FAMILY ? pvp->pvp_pgid : vp->vp_pid,
		vp->vp_pid, pvp->pvp_ppid, pvp->pvp_pgid, this_node);

	/*
	 * Bring the checkpointing task into a quiescent and safe state.
	 */
	error = rtask_pproc_quiesce(pvp->pvp_pproc);
	if (error != ESUCCESS)
		return(error);

	/*
	 * Scrape together pertinent server state for this process,
	 * putting it into a vm_allocate'd region.
	 */
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &psp,
			  sizeof(chkpnt_proc_state_t),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	psp->fs_size = CHKPNT_FILE_STATE_SIZE(fsp);
	psp->version = CHKPNT_FILE_VERSION;
	bcopy(cur_state, &psp->thread_state, cur_state_count * sizeof(int));
	error = chkpnt_get_server_state(vp, psp);
	if (error)
		goto out;
	fsp->ctty = psp->ctty;

	/*
	 * Having compiled all the state dope, write it to disk.
	 */
	error = chkpnt_pproc_state(pvp->pvp_pproc, &state_filename, psp, fsp);
	if (error)
		goto out;

	/*
	 * Make sure we don't have any extra memory regions hanging around.
	 */
	error = chkpnt_check_regions(pvp->pvp_pproc);
	if (error)
		goto out;

	/*
	 * Now dump the user task, doing it asynchronously if requested
	 * and the process is not to be killed afterwards.
	 */
	error = chkpnt_pproc_core(pvp->pvp_pproc,
				  &image_filename,
				  cur_state,
				  (flags & CHKPNT_ASYNC) &&
				      !(flags & CHKPNT_KILL));

out:
	if (psp != NULL)
		(void) vm_deallocate(mach_task_self(),
		 		     (vm_address_t) psp,
				     (vm_size_t) sizeof(chkpnt_proc_state_t));

	/*
	 * Finally, either kill ourself after a successful checkpoint, if so
	 * requested, or resume the user process.
	 */
	if ((error == ESUCCESS) && (flags & CHKPNT_KILL))
		(void) VPOP_EXIT(vp, W_EXITCODE(0, SIGKILL));
	else
		(void) rtask_pproc_unquiesce(pvp->pvp_pproc);

	return(error);
}

/*
 * This routine implements the exec_restart() system call. Parameter
 * chkpnt_prefix specifies the path to a pair of checkpoint files containing
 * state and core information. The previously executing program binary is
 * re-exec'ed, the stack and data regions are restored, the current and root
 * direcory context is reset and the file state is returned to the emulator
 * to be re-established.
 */ 
int
exec_restart(
	struct vproc		*vp,
	char			*chkpnt_prefix,
	thread_state_t		*new_state,
	unsigned int		*new_state_count,
	mach_port_t		*new_rootdir_port,
	mach_port_t		*new_currentdir_port,
	chkpnt_file_state_t	**fs,
	int			*fs_size,
	boolean_t		*traced)
{
	int			error = ESUCCESS;
	int			ret;
	chkpnt_proc_state_t	*ps = NULL;
	path_name_t		image_filename;
	path_name_t		state_filename;
	struct pvproc		*pvp = PVP(vp);
	struct proc		*procp = pvp->pvp_pproc;
	dev_t			ctty_dev;

	/*
	 * Get the current controlling tty device name. It's possible that
	 * there's not one - in that case ENOTTY is returned forthwith.
	 */
	error = VPOP_CTTY_GETATTR(vp, 0, 0, &ctty_dev, 0, 0);
	if (error != ESUCCESS)
		return(error);

	/*
	 * Construct checkpoint filenames from given prefix.
	 */
	sprintf(&image_filename, "%s.core", chkpnt_prefix);
	sprintf(&state_filename, "%s.stat", chkpnt_prefix);

	/*
	 * Read the .stat file into vm_allocate'd memory; the process state
	 * is allocated (and deallocated here) but the file state is allocated
	 * by restart_pproc_state().
	 */
	ret = vm_allocate(mach_task_self(),
			  (vm_address_t *) &ps,
			  sizeof(chkpnt_proc_state_t),
			  TRUE);
	if (ret != KERN_SUCCESS) {
		error = EAGAIN;
		goto out;
	}
	*fs = NULL;
	error = restart_pproc_getstate(procp, state_filename, ps, fs);
	if (error != ESUCCESS)
		goto out;
	*fs_size = ps->fs_size;

	/*
	 * Derive vnode ports for root/current directories to be
	 * used when execing the checkpointed image. 
	 */
	error = restart_pproc_context(procp,
				      fs,
				      new_rootdir_port,
				      new_currentdir_port);
	if (error != ESUCCESS)
		goto out;

	/*
	 * Perform a standard execve() of the image running at the time
	 * of the checkpoint and restore data/stack from the .image file.
	 */
	error = restart_pproc_exec(procp,
				   *new_rootdir_port, *new_currentdir_port,
				   (*fs)->exec_fname, image_filename);
	if (error != ESUCCESS)
		goto out;

	/*
	 * Finally, restore the checkpointed process state.
	 */
	*new_state_count = THREAD_STATE_COUNT;
	bcopy(&ps->thread_state, new_state, sizeof(THREAD_STATE_T));
	(void) restart_set_server_state(vp, ps);

out:
	if (ps != NULL)
		(void) vm_deallocate(mach_task_self(),
		 		     (vm_address_t) ps,
				     (vm_size_t) sizeof(chkpnt_proc_state_t));
	if (error && (*fs != NULL))
		(void) vm_deallocate(mach_task_self(),
		 		     (vm_address_t) *fs,
				     (vm_size_t) *fs_size);
	return(error);
}

/*
 * Collect together all server state to be checkpointed.
 */ 
int
chkpnt_get_server_state(
	struct vproc		*vp,
	chkpnt_proc_state_t	*ps)
{
	int		error = ESUCCESS;
	struct pvproc	*pvp = PVP(vp);

	/*
	 * Get all server dope on the process.
	 */
	ps->pid  = vp->vp_pid;
	ps->ppid = pvp->pvp_ppid;
	ps->pgid = pvp->pvp_pgid;
	ps->sid  = pvp->pvp_sid;
	error = migrate_pproc_load_msg(pvp->pvp_pproc, &ps->mi_data,
			  	       NULL, NULL, NULL, NULL, NULL, 
			  	       NULL, NULL, NULL, NULL, NULL, 
				       (char *) &ps->command_name,
				       (char *) &ps->logname
#ifdef PARACORE
				       , NULL, NULL, NULL
#endif /* PARACORE */
				       );
	if (error)
		return (error);

	/*
	 * Get controlling tty device name. Note that upon restart,
	 * the checkpointed controlling tty is not re-opened but the
	 * restarter's controlling tty must be substituted.
	 */
	error = VPOP_CTTY_GETATTR(vp,
				  0, 0, &ps->ctty.device, &ps->ctty.node, 0);

	return(error);
}

/*
 * Replace server state for current process.
 */
int
restart_set_server_state(
	struct vproc		*vp,
	chkpnt_proc_state_t	*ps)
{
	struct pvproc	*pvp = PVP(vp);

	/*
	 * Simply unload relevant parts of the mi data into the proc struct.
	 * This is performed in 2 stages only because migration needs it
	 * this way.
	 */
	migrate_pproc_unload_msg(pvp->pvp_pproc, &ps->mi_data,
				 MACH_PORT_NULL, NULL, NULL, 0,
				 (char *) &ps->command_name,
				 (char *) &ps->logname
#ifdef PARACORE
	/*
	 * uu_exec_prg_name has already been set via execve()
	 * which was called by restart_pproc_exec().
	 */
			         , NULL
#endif /* PARACORE */
				 );
	migrate_pproc_fix(pvp->pvp_pproc, &ps->mi_data,
			  MACH_PORT_NULL, MACH_PORT_NULL, MACH_PORT_NULL);
	return(ESUCCESS);
}
