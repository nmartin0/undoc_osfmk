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
 * Copyright (c) 1991, Locus Computing Corporation
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
 * OSF/1 Release 1.0
 */
/* 
 * HISTORY
 * $Log: vp_syscalls.c,v $
 * Revision 1.10  1994/11/18  20:51:17  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/03/14  02:10:03  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.8  1993/10/11  13:18:31  nandy
 * All calls to PVPOP_GET_ATTR() and VPOP_GET_ATTR() now have 2 extra parameters: uid and ruid.
 *
 * Revision 1.7  1993/08/17  20:22:54  cfj
 * Modify kill3() so that a VPROC_RELEASE() gets called regardless of the value
 * return by nx_proxy(). (PTS bug #6189)
 *
 * Revision 1.6  1993/07/14  18:47:52  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  21:11:28  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:11:48  stefan
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:13:30  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  06:06:27  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  03:08:00  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:58:55  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:47:33  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.2  1992/11/04  22:39:59  joel
 * Fix PTS 3505.  If a NX application is put in the background, and then killed
 * with "kill <proxy_pid>" we really want the whole application to die, not just
 * the proxy_pid.  This change in kill3() does that.
 *
 * Revision 1.1.1.1  1993/05/03  17:55:40  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 2.19  93/06/16  15:29:37  klh
 * 	Revision 2.14  93/06/04  12:22:05  rabii
 * 		Add node to VPOP_FORK (rabii)
 * 
 * Revision 2.18  93/04/22  16:44:44  roman
 * [Bug 222] Waiting for pgid has problems. Included some variable
 * 	renaming for clarity.
 * Minor fixes to RCS comments.
 *
 * Revision 2.17  92/12/01  11:57:00  chrisp
 * Add cast (pid_t *) for 2nd parameter of VPOP_FORK().
 * 
 * Revision 2.16  92/10/05  14:04:07  klh
 * 	Revision 2.11  92/09/24  16:52:08  rabii
 * 		Fixed ogetpgrp, when pid == 0.  Was doing nothing, returning 
 *		garbage. 
 *		[92/09/17            condict]
 * 
 * Revision 2.15  92/09/29  09:20:30  roman
 * Add code that uses the new virtual process system operations (vpsops)
 * 	to get and set nice values for current uid and to send
 * 	a signal to all permissible processes.
 * 
 * Revision 2.14  92/06/05  15:48:12  roman
 * Change assertions for syscall_on_master() to only happen for non-TNC
 * 	code. The TNC code is parallelized.
 * 
 * Revision 2.13  92/06/05  14:01:25  klh
 * 	Revision 2.9  92/05/31  19:00:19  loverso
 * 		Include user.h (needed by syscall_on_master) (pjg).
 * 
 * Revision 2.12  92/05/29  10:42:56  chrisp
 * Add Faramarz's fixes for POSIX compliance failures in setpgrp().
 * 
 * Revision 2.11  92/01/30  14:21:37  chrisp
 * Add ptrace calling VPOP_PTRACE.
 * 
 * Revision 2.10  92/01/28  16:34:58  roman
 * Include assert.h and parallel.h for ASSERTs.
 * 
 * Revision 2.9  92/01/16  11:48:03  chrisp
 * Remove ignoring of SIGTTIN/SIGTTOU/SIGTSTP because OSF's more does a
 * kill(0,SIGTSTP) in some circumstances.
 * 
 * Revision 2.8  92/01/02  12:36:20  roman
 * Return a value from getpid() and getpgrp() (bug reported by sjs).
 * 
 * Revision 2.11  92/01/30  14:21:37  chrisp
 * Add ptrace calling VPOP_PTRACE.
 * 
 * Revision 2.10  92/01/28  16:34:58  roman
 * Include assert.h and parallel.h for ASSERTs.
 * 
 * Revision 2.9  92/01/16  11:48:03  chrisp
 * Remove ignoring of SIGTTIN/SIGTTOU/SIGTSTP because OSF's more does a
 * kill(0,SIGTSTP) in some circumstances.
 * 
 * Revision 2.8  92/01/02  12:36:20  roman
 * Return a value from getpid() and getpgrp() (bug reported by sjs).
 * 
 * Revision 2.7  91/12/19  11:16:13  chrisp
 * Add kill3() system call.
 * 
 * Revision 2.6  91/11/14  15:02:23  chrisp
 * Extra parameter added to VPOP_GET_ATTR.
 * Extra parameter added to signaling VPOPs.
 * Kill ignores job control signals.
 * 
 * Revision 2.5  91/11/01  17:31:49  roman
 * Change the system calls using VPROC_RELEASE to correctly check
 * whether the release should be done (look at original parameter rather
 * than vproc).
 * 
 * Revision 2.4  91/10/30  12:58:36  roman
 * Fix all the calls to LOCATE_VPROC_PID() to allow a return of a null
 * pointer. Fix numerous bugs with extra VPROC_RELEASE or VPROC_HOLD
 * calls. Fix bugs with bogus return statements prior to calling
 * VPROC_RELEASE.
 * 
 * Revision 2.3  91/10/18  18:40:05  chrisp
 *  	owait() problem fixed - bad #ifdef COMPAT_43s corrected.
 * 	New VPOP ops and argument scheme.
 * 
 * Revision 2.2  91/09/16  16:26:03  rabii
 * 	Initial Checkin (locus check-in by chrisp)
 * 
 *
 */
#include <machine/reg.h>

#include <sys/unix_defs.h>
#include <sys/vproc.h>
#include <sys/wait.h>
#include <sys/errno.h>
#include <sys/resource.h>
#include <sys/signal.h>
#include <sys/ptrace.h>
#if	UNIX_LOCKS
#include <sys/lock_types.h>
#endif

#ifdef	TNC
#define	ASSERT_IF_NOT_TNC(assertion)
#else	/* TNC */
#include <sys/user.h>
#if	MACH
#include <kern/assert.h>
#include <kern/parallel.h>
#endif
#define	ASSERT_IF_NOT_TNC(assertion) ASSERT(assertion)
#endif	/* TNC */
	
/* ARGSUSED */
getpid(vp, uap, retval)
	struct vproc *vp;
	void *uap;
	int *retval;
{
	pid_t ppid;
	int error;

	/* use the vproc interface */
	error = VPOP_GET_ATTR(vp, &ppid, 0, 0, 0, 0, 0, 0);
	*retval = vp->vp_pid;
	retval[1] = ppid;

	return(error);
}

/* ARGSUSED */
getpgrp(vp, uap, retval)
	struct vproc *vp;
	void *uap;
	int *retval;
{
	/* use the vproc interface */
	return(VPOP_GET_ATTR(vp, 0, (pid_t *) retval, 0, 0, 0, 0, 0));
}

#ifdef	COMPAT_43
ogetpgrp(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	struct args {
		int pid;
	} *uap = (struct args *) args;
	int error;
	pid_t pgrp, sid;

	if (uap->pid == 0) {
		/* use the vproc interface */
		return(VPOP_GET_ATTR(vp, 0, (pid_t *) retval, 0, 0, 0, 0, 0));
	} else {
		vp = LOCATE_VPROC_PID(uap->pid);
		if (vp != 0) {
			error = VPOP_GET_ATTR(vp, 0, (pid_t *) retval, 0, 0, 0, 0, 0);
			VPROC_RELEASE(vp, "ogetpgrp");
		} else
			error = ESRCH;
	}
	return(error);
}
#endif	/* COMPAT_43 */

/* ARGSUSED */
setsid(vp, uap, retval)
	struct vproc *vp;
	void *uap;
	int *retval;
{
	int error;
	if ( (error = VPOP_SETSID(vp)) != 0)
		return( error );
	*retval = vp->vp_pid;

	return(ESUCCESS);
}

/*
 * fork system call.
 */
fork(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	register struct args {
		node_t		node;
		thread_state_t	new_state;
		unsigned int	new_state_count;
	} *uap = (struct args *) args;

	return(VPOP_FORK(vp, (pid_t *) &retval[0], (node_t) uap->node,
			 (caddr_t) uap->new_state, uap->new_state_count));
}

#ifndef	TNC
/*
 * rfork is handled in the emulator and uses the same path as fork
 */
rfork(vp, args, retval)
	struct vproc *vp;
	void *args;
{
	panic("rfork: Should not be invoked in the server");
}
#endif	/* TNC */

vfork(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	register struct args {
		thread_state_t	new_state;
		unsigned int	new_state_count;
	} *uap = (struct args *) args;

	return(VPOP_FORK(vp, (pid_t *) &retval[0], NONODE,
			 (caddr_t) uap->new_state, uap->new_state_count));
}

#ifdef COMPAT_43
/*
 * The compatibility interface wait() is patched through to the
 * general purpose function waitf(). The options and rusage fields are
 * derived from register values that depend on the machine type.
 * N.B. 4.3 reno sets uap->status to 0 unconditionally for compat code;
 *	we're not, so keep an eye on this.
 */

owait(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	register struct args {
		int	pid;
		int	*status;
		int	options;
		struct	rusage_dev *rusage;
		int	compat;
	} *uap = (struct args *) args;

	uap->pid = WAIT_ANY;
	uap->compat = 1;
	uap->status = 0;

	return (waitf(vp, uap, retval));
}

wait4(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	struct args {
		int	pid;
		int	*status;
		int	options;
		struct	rusage_dev *rusage;
		int	compat;
	} *uap = (struct args *) args;

	uap->compat = 0;
	return (waitf(vp, uap, retval));
}
#else	/* COMPAT_43 */

#define wait4	waitf		/* now declared the right way round! */

#endif	/* COMPAT_43 */

/*
 * Wait system call, actually the target of waitpid() and wait4(), as
 * well as being the function that actually does the work for the 4.3
 * compatibility interfaces.
 * Search for a terminated (zombie) child,
 * finally lay it to rest, and collect its status.
 * Look also for stopped (traced) children,
 * and pass back status from them.
 *
 * In order to address the "logged-in" terminal problem for window
 * managers, telnet servers, etc. which are not created initially by
 * init but which we want init to clean up after, we add the concept
 * of the "login device" associated with a process.  The login
 * program executes a special ioctl() call to distinguish the top
 * process in the tree and record the controlling device with which
 * it is associated.  The new wait() option, WLOGINDEV, (used by
 * init) indicates that the returned resource usage record should
 * include the device number of the controlling terminal for the
 * top level process (this will be NODEV for any other process).
 * This way, init need not create terminal listeners for all
 * potential login terminals but can still know enough to clean
 * things up after such processes (which it has inherited) do exit.
 */

waitf(vq, args, retval)
	register struct vproc *vq;
	void *args;
	int retval[];
{
	register int error;
	int status;
	struct rusage_dev ru;
	int vproc_options = 0;

	register struct args {
		pid_t	pid;
		int	*status;
		int	options;
		struct	rusage_dev *rusage;
#ifdef	COMPAT_43
		int	compat;
#endif
	} *uap = (struct args *) args;

#ifdef	COMPAT_43
	if (!(uap->compat))
#endif
	if (uap->options &~ (WUNTRACED|WNOHANG|WLOGINDEV))
		return (EINVAL);

	if (uap->options & WUNTRACED)
		vproc_options |= VPROC_WUNTRACED;
	if (uap->options & WNOHANG)
		vproc_options |= VPROC_WNOHANG;

	error = VPOP_WAIT(vq, uap->pid,
			     vproc_options,
			     &status,
			     &ru,
			     (pid_t *) &retval[0]);
	if (error)
		return(error);

	/* copy out status */
	if (uap->status) {
		if (error = copyout((caddr_t)&status,
				(caddr_t)uap->status,
				sizeof(status)))
			return(error);
	}

	/* copy out rusage - taking note of what was requested */
	if (uap->rusage) {
		if (error = copyout((caddr_t)&ru,
				(caddr_t)uap->rusage,
				(uap->options & WLOGINDEV) ?
					sizeof(struct rusage_dev) :
					sizeof(struct rusage)))
			return(error);
	}

#ifdef	COMPAT_43 
	if (uap->compat)
		retval[1] = status;
#endif

	return(error);
}

/*
 * Exit system call: pass back caller's arg
 */
rexit(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	struct args {
		int     rval;
	} *uap = (struct args *) args;
	int error;
	error = VPOP_EXIT(vp, W_EXITCODE(uap->rval, 0));
	return (error);
}

kill(cvp, args, retval)
	register struct vproc *cvp;
	void *args;	
	int *retval;
{
	register struct args {
		int	pid;
		int	signo;
		int	arg;
	} *uap = (struct args *) args;
	int dummy_retval;
	uap->arg = 0;
	return(kill3(cvp, args, &dummy_retval));

}

kill3(cvp, args, retval)
	register struct vproc *cvp;
	void *args;	
	int *retval;
{
	register struct args {
		int	pid;
		int	signo;
		int	arg;
	} *uap = (struct args *) args;
	register struct vproc *vp;
	int error;
	struct procset ps;

	if ((unsigned) uap->signo > NSIG) {
		return (EINVAL);
	}
 
	if (uap->pid > 0) {
		/* kill single process */
		vp = LOCATE_VPROC_PID(uap->pid);
		if (vp != 0) {
#ifdef NX
		    if (nx_proxy(vp))
                	error = VPOP_SIGPGRP(vp, uap->signo, uap->arg, NULL, 0);
		    else
#endif
			error = VPOP_SIGPROC(vp, uap->signo, uap->arg, 0);

		    VPROC_RELEASE(vp, "kill");
		} else
			error = ESRCH;
		if (error == ESUCCESS)
			*retval = 1;
		return(error);
	}
	switch (uap->pid) {
	case -1:		/* broadcast signal */
		ps.p_op = POP_OR;
		ps.p_lidtype = P_ALL;
		ps.p_ridtype = P_NONE;
		return (VPSOP_SIGPROCSET(&ps, uap->signo, uap->arg, retval));
	case 0:			/* signal own process group */
		return (killpg3(cvp, uap->signo, 0, uap->arg, retval));
	default:		/* negative explicit process group */
		return (killpg3(cvp, uap->signo, -uap->pid, uap->arg, retval));
	}
	/* NOTREACHED */
}

#if COMPAT_43
okillpg(vp, args, retval)
	struct vproc *vp;
	void *args;	
	int *retval;
{
	register struct args {
		int	pgid;
		int	signo;
	} *uap = (struct args *) args;

	if ((unsigned) uap->signo > NSIG)
		return (EINVAL);
	return (killpg3(vp, uap->signo, uap->pgid, 0, NULL));
}
#endif

killpg3(cvp, signo, pgid, arg, retval)
	register struct vproc *cvp;
	int signo, pgid, arg, *retval;
{
	register struct vproc *vp;
	int error;
	
	if (pgid == 0)
		(void) VPOP_GET_ATTR(cvp, 0, (pid_t *) &pgid, 0, 0, 0, 0, 0);

	vp = LOCATE_VPROC_PID((pid_t) pgid);	
	if (vp != 0) {
		error = VPOP_SIGPGRP(vp, signo, arg, retval, 0);
		VPROC_RELEASE(vp, "killpg3");
	} else
		error = ESRCH;
	return(error);
}

#ifdef COMPAT_43
/* ARGSUSED */
setpgrp(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	return(setpgrp1(vp, args, 1));
}
#endif COMPAT_43

/* ARGSUSED */
setpgid(vp, args, retval)
	struct vproc *vp;
	void *args;
	int *retval;
{
	return(setpgrp1(vp, args, 0));
}

setpgrp1(cvp, args, compat)
	struct vproc *cvp;
	void *args;
	int compat;
{
        register struct args {
                int     pid;
                int     pgid;
        } *uap = (struct args *)args;
	register struct vproc *vp = NULL;
	register struct vproc *vg;
	register int error;
	pid_t sid;

	if (uap->pgid < 0)
		return (EINVAL);

	if (uap->pid != 0) {
		vp = LOCATE_VPROC_PID(uap->pid);
		if (vp == 0)
			return(ESRCH);
	} else
		vp = cvp;

	if (uap->pgid == 0) {
#ifdef COMPAT_43
		/*
		 * 4.3 allows a process to set its process
		 * group to zero, but POSIX says that a zero
		 * pgid means use the process pid as the pgid.
		 */
		if (!compat)
#endif COMPAT_43
			uap->pgid = vp->vp_pid;
	}

	(void) VPOP_GET_ATTR(cvp, 0, 0, &sid, 0, 0, 0, 0);
	vg = LOCATE_VPROC_PID(uap->pgid);
	if (vg != 0) {
		error = VPOP_SETPGID(vp, vg, cvp->vp_pid, sid);
		VPROC_RELEASE(vg, "setpgrp1(group)");
	} else
		error = EPERM;
	if (uap->pid != 0)
		VPROC_RELEASE(vp, "setpgrp1(proc)");

	return(error);

}

/*
 * Resource controls and accounting.
 */

getpriority(curvp, args, retval)
	struct vproc *curvp;
	void *args;
	int *retval;
{
	register struct args {
		int	which;
		int	who;
	} *uap = (struct args *) args;
	register int error;
	register struct vproc *v = NULL, *g = NULL;
	struct procset ps;

	ASSERT_IF_NOT_TNC(syscall_on_master());
	switch (uap->which) {

	case PRIO_PROCESS:
		if (uap->who == 0)
			v = curvp;
		else {
			v = LOCATE_VPROC_PID(uap->who);
			if (v == 0)
				return(ESRCH);
		}

		error = VPOP_PROC_NICE(v, retval, VPROC_GET);
		if (uap->who != 0)
			VPROC_RELEASE(v, "getpriority");
		break;

	case PRIO_PGRP:
		if (uap->who == 0)
			(void) VPOP_GET_ATTR(curvp, 0,
						(pid_t *) &uap->who, 0, 0, 0, 0, 0);
		g = LOCATE_VPROC_PID(uap->who);
		if (g != 0) {
			error = VPOP_PGRP_NICE(g, retval, VPROC_GET);
			VPROC_RELEASE(g, "getpriority");
		} else
			error = ESRCH;
		break;

	case PRIO_USER:
		if (uap->who == 0)
			(void) pproc_get_attr(0, 0, 0, 0, 0, 0,
					      (uid_t *) &uap->who, 0, 0, 0);
		ps.p_op = POP_OR;
		ps.p_lidtype = P_UID;
		ps.p_lid = (uid_t) uap->who;
		ps.p_ridtype = P_NONE;
		error = VPSOP_PROCSET_NICE(&ps, retval, VPROC_GET);
		break;

	default:
		return (EINVAL);
	}
	return(error);
}

setpriority(curvp, args, retval)
	struct vproc *curvp;
	void *args;
	int *retval;
{
	register struct args {
		int	which;
		int	who;
		int	prio;
	} *uap = (struct args *) args;
	register int error;
	register struct vproc *v = NULL, *g = NULL;
	struct procset ps;

	ASSERT_IF_NOT_TNC(syscall_on_master());
	switch (uap->which) {

	case PRIO_PROCESS:
		if (uap->who == 0)
			v = curvp;
		else {
			v = LOCATE_VPROC_PID(uap->who);
			if (v == 0)
				return(ESRCH);
		}

		error = VPOP_PROC_NICE(v, &uap->prio, VPROC_SET);
		if (uap->who != 0)
			VPROC_RELEASE(v, "setpriority");
		break;

	case PRIO_PGRP:
		if (uap->who == 0)
			(void) VPOP_GET_ATTR(curvp, 0,
						 (pid_t *) &uap->who, 0, 0, 0, 0, 0);
		g = LOCATE_VPROC_PID(uap->who);
		if (g != 0) {
			error = VPOP_PGRP_NICE(g, &uap->prio, VPROC_SET);
			VPROC_RELEASE(g, "setpriority");
		} else
			error = ESRCH;
		break;

	case PRIO_USER:
		if (uap->who == 0)
			(void) pproc_get_attr(0, 0, 0, 0, 0, 0,
					      (uid_t *) &uap->who, 0, 0, 0);
		ps.p_op = POP_OR;
		ps.p_lidtype = P_UID;
		ps.p_lid = (uid_t) uap->who;
		ps.p_ridtype = P_NONE;
		error = VPSOP_PROCSET_NICE(&ps, &uap->prio, VPROC_SET);
		break;

	default:
		return (EINVAL);
	}
	return(error);
}

/*
 * sys-trace system call.
 */
ptrace(curvp, args, retval)
	struct vproc *curvp;
	void *args;
	int *retval;
{
	register struct args {
		int	req;
		int	pid;
		int	*addr;
		int	data;
	} *uap = (struct args *) args;

	register struct vproc *vp;
	register int request;

	switch (uap->req) {
	case PT_TRACE_ME:	/* enable tracing for caller */
		return(VPOP_PTRACE(curvp, curvp, VPROC_PT_TRACE_ME,
				   uap->addr, uap->data, retval));
	case PT_READ_I:		/* read word in child's I space */
		request = VPROC_PT_READ_I; break;
	case PT_READ_D:		/* read word in child's D space */
		request = VPROC_PT_READ_D; break;
	case PT_READ_U:		/* read word in child's user structure */
		request = VPROC_PT_READ_U; break;
	case PT_WRITE_I:	/* write word in child's I space */
		request = VPROC_PT_WRITE_I; break;
	case PT_WRITE_D:	/* write word in child's D space */
		request = VPROC_PT_WRITE_D; break;
	case PT_WRITE_U:	/* write word in child's user structure */
		request = VPROC_PT_WRITE_U; break;
	case PT_CONTINUE:	/* continue the child */
		request = VPROC_PT_CONTINUE; break;
	case PT_KILL:		/* kill the child process */
		request = VPROC_PT_KILL; break;
	case PT_STEP:		/* single step the child */
		request = VPROC_PT_STEP; break;
	default:
		return(EINVAL);
	}

	/*
	 * Locate victim - note it must be a child, so VPROCPTR must find it
	 * and there's no need to use LOCATE_PROC.
	 */
	vp = VPROCPTR(uap->pid);
	if (vp == NULL)
		return (ESRCH);

	return(VPOP_PTRACE(curvp, vp, request, uap->addr, uap->data, retval));

}
