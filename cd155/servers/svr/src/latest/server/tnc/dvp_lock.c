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
 * $Log: dvp_lock.c,v $
 * Revision 1.6  1995/02/01  21:41:09  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.5  1994/11/18  20:43:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/07/14  18:32:31  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  20:44:04  cfj
 * Adding new code from vendor
 *
 * Revision 1.3  1993/05/06  19:21:43  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:44:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.2  1992/11/30  22:46:58  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  22:45:20  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:43:12  cfj
 * Bump major revision number.
 *
 * Revision 3.4  1992/09/29  07:41:34  roman
 * Change naming of routines and flags in the file to refer to
 * 	"movement locks" rather than "migrate locks" because
 * 	this code is used for both migrate and rexec.
 * Also, the movement information is now in its own separate
 * 	structure rather than being in the pvproc explicitly.
 *
 * Revision 3.3  92/07/30  16:05:22  chrisp
 * Major overhaul to extend port sequence counting to vproc free and mach
 * 	port deallocation. This prevents a vproc being destroyed while
 * 	vproc port RPCs are outstanding.
 * 
 * Revision 3.2  92/07/10  08:59:28  chrisp
 * Add proc and vproc port sequence counting and delay migration until
 * 	no operation is outstanding.
 * 
 * Revision 3.1  92/03/18  14:08:50  roman
 * Correct initial submission so RCS log messages are printed.
 * 
 *
 */

#include <sys/param.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <tnc/dpvproc.h>
#include <sys/errno.h>
#include <uxkern/proc_to_task.h>

/*
 * Variable used for VPROC_LOCK_DEBUG macro
 */
int vplocktrace = 0;

/*
 * Routines used for controlling the interaction between pvproc ops and
 * process migration. The way these routines are structured:
 *
 *	vproc_hold_movement is called by code that wants to prevent
 *		process migration. This is undone by vproc_release_movement().
 *	vproc_start_movement() Is used by process migration to prevent
 *		and vproc_hold_movement() calls from succeeding.
 *
 * vproc_hold_movement() is both "recursive" and "shared". "Recursive" means 
 *	that a single thread can call vproc_hold_movement() multiple times 
 *	recursively and all calls will succeed. This is a requirement due to 
 *	the fact that pvproc operations can invoke other pvproc operations, 
 *	and unraveling at what point vproc_hold_movement() was called at
 *	a prior time for this thread can be difficult (made even more difficult
 *	by the fact that a "recursive" invocation may come on a
 *	"remote pvproc op service thread", which is very difficult to
 *	associate with the original invoking thread). "Shared" means that
 *	multiple threads can concurrently call vproc_hold_movement() 
 *	and all will succeed. Migration will be permitted only when
 *	all the vproc_hold_movement() calls have been undone.
 *
 * Note that the algorithms as designed below will allow vproc_hold_movement()
 *	calls to proceed even if there is vproc_start_movement() pending.
 *	This is not optimal, but it is difficult to do in any other way.
 *	This is due to the fact that recursive calls are allowed,
 *	and these calls are hard to identify (see above). This means that
 *	we pretty much must allow all vproc_hold_movement() calls to go
 *	through on a high priority level if we are to allow recursive
 *	calls to go through.
 */

void
vproc_hold_movement_init(
	struct vproc		*vp)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	mutex_init(&mlp->ml_lock);
	condition_init(&mlp->ml_shr_cond);
	condition_init(&mlp->ml_excl_cond);
	condition_init(&mlp->ml_port_cond);
}
	
int
vproc_hold_movement(
	struct vproc 		*vp,
	boolean_t		local_only,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);
	int			error;

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_hold_movement");

	mutex_lock(&mlp->ml_lock);

	mlp->ml_flag |= ML_SHR_LOCK_HELD_OR_PENDING;

	while ((mlp->ml_flag & ML_EXCL_LOCK_HELD) != 0)
		condition_wait(&mlp->ml_shr_cond,
			       &mlp->ml_lock);

	if (local_only && (PVP(vp)->pvp_flag & PV_IS_LOCAL) == 0) {
		mlp->ml_flag &= ~ML_SHR_LOCK_HELD_OR_PENDING;
		error = EREMOTE;
	} else {
		mlp->ml_shr_count++;
		error = ESUCCESS;
	}

	mutex_unlock(&mlp->ml_lock);

	return(error);
}

void
vproc_release_movement(
	struct vproc	 	*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_release_movement");

	mutex_lock(&mlp->ml_lock);

	mlp->ml_shr_count--;
	if(mlp->ml_shr_count == 0) {
		mlp->ml_flag &= ~ML_SHR_LOCK_HELD_OR_PENDING;
		if (mlp->ml_excl_count > 0)
			condition_signal(&mlp->ml_excl_cond);
	}

	mutex_unlock(&mlp->ml_lock);
}

boolean_t
idle_port_ops(
	struct vproc 		*vp,
	boolean_t		is_own_port,
	boolean_t		wait)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);
	mach_port_t		vproc_port = vproc_to_port_lookup(vp);
	mach_port_t		proc_port;
	mach_port_status_t	proc_rstat;
	mach_port_status_t	vproc_rstat;
	kern_return_t		ret;

	/*
	 * Note that we allow there to be no proc port remaining so
	 * we can idle the vproc port prior to its deallocation.
	 */
	proc_port = proc_to_port_lookup(PVP(vp)->pvp_pproc);
	if (proc_port != MACH_PORT_NULL)
		ux_server_remove_port(proc_port);
	ux_server_remove_port(vproc_port);
	while (TRUE) {
		/*
		 * Get mach port status for proc and vproc ports to obtain
		 * the sequence count of received messages.
		 */
		if (proc_port == MACH_PORT_NULL)
			proc_rstat.mps_seqno = mlp->ml_proc_sequence;
		else {
			ret = mach_port_get_receive_status(mach_task_self(),
							   proc_port,
							   &proc_rstat);
			if (ret != KERN_SUCCESS)
				panic("mach_port_get_receive_status for proc port returned 0x%x",ret);
		}
		ret = mach_port_get_receive_status(mach_task_self(),
						   vproc_port,
						   &vproc_rstat);
		if (ret != KERN_SUCCESS)
			panic("mach_port_get_receive_status for vproc port returned 0x%x",ret);

		/*
		 * Check for pending messages by comparing the number
		 * received on proc and vproc ports with the number recorded
		 * as completed. We must exclude the operation in hand, of
		 * course, if we're serving a port op for this vproc.
		 */
		if ((proc_rstat.mps_seqno + vproc_rstat.mps_seqno)
		    != mlp->ml_proc_sequence + mlp->ml_vproc_sequence
			+ is_own_port ? 1 : 0) {
			if (!wait)
				return(FALSE);
			mlp->ml_flag |= ML_EXCL_LOCK_PENDING_COUNT;
			condition_wait(&mlp->ml_port_cond,
				       &mlp->ml_lock);
		} else {
			mlp->ml_flag &= ~ML_EXCL_LOCK_PENDING_COUNT;
			return(TRUE);
		}
	}
}

void
vproc_start_movement(
	struct vproc 		*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_start_movement");

	mutex_lock(&mlp->ml_lock);

	/*
	 * First, we must ensure that there are no
	 * pending proc or vproc port operations in progress.
	 */
	idle_port_ops(vp, TRUE, TRUE);

	while ((mlp->ml_flag & ML_EXCL_LOCK_HELD) != 0 ||
			mlp->ml_shr_count > 0) {
		mlp->ml_excl_count++;
		condition_wait(&mlp->ml_excl_cond,
			       &mlp->ml_lock);
		mlp->ml_excl_count--;
	}

	mlp->ml_flag |= ML_EXCL_LOCK_HELD;

	mutex_unlock(&mlp->ml_lock);
}

boolean_t
vproc_port_idle(
	struct vproc 		*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);
	boolean_t		result = TRUE;

	/*
	 * Called prior to vproc port deallocation to ensure
	 * that no operations are outstanding.
	 * Check first that there is a receive right - nothing to do if not.
	 */
	if ((PVP(vp)->pvp_flag & PV_IS_LOCAL) != 0) {
		VPROC_LOCK_DEBUG(vp, debug_string, "vproc_port_idle");
		mutex_lock(&mlp->ml_lock);
		result = idle_port_ops(vp, 0, FALSE);
		mutex_unlock(&mlp->ml_lock);
	}
	return(result);
}

void
vproc_end_movement(
	struct vproc	 	*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_end_movement");

	mutex_lock(&mlp->ml_lock);

	mlp->ml_flag &= ~ML_EXCL_LOCK_HELD;

	if ((mlp->ml_flag & ML_SHR_LOCK_HELD_OR_PENDING) != 0)
		condition_broadcast(&mlp->ml_shr_cond);
	else if(mlp->ml_excl_count > 0)
		condition_signal(&mlp->ml_excl_cond);

	mutex_unlock(&mlp->ml_lock);
}

void
vproc_end_port_op(
	struct vproc 		*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_end_port_op");

	mutex_lock(&mlp->ml_lock);

	mlp->ml_proc_sequence++;
	if(mlp->ml_flag & ML_EXCL_LOCK_PENDING_COUNT)
		condition_signal(&mlp->ml_port_cond);

	mutex_unlock(&mlp->ml_lock);
}

void
vproc_end_vp_port_op(
	struct vproc 		*vp,
	char			*debug_string)
{
	pvp_movement_lock_t	*mlp = &(PVP(vp)->pvp_movement_lock);

	VPROC_LOCK_DEBUG(vp, debug_string, "vproc_end_vp_port_op");

	mutex_lock(&mlp->ml_lock);

	mlp->ml_vproc_sequence++;
	if(mlp->ml_flag & ML_EXCL_LOCK_PENDING_COUNT)
		condition_signal(&mlp->ml_port_cond);

	mutex_unlock(&mlp->ml_lock);
}
