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
 * Copyright (c) 1991, Locus Computing Corporation
 * All rights reserved
 */
/*
 * HISTORY
 * $Log: kern_resource.c,v $
 * Revision 1.8  1994/11/18  20:27:11  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/10/25  23:40:40  suri
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
 * Revision 1.6  1994/10/11  18:39:31  suri
 *  Reviewer: cfj
 *  Risk: H
 *  Benefit or PTS #: 10593
 *  Testing: Specific testcase; fileio, message_passing, xtrnl,
 *           PFS and pthreads EATs
 *  Module(s): e_obreak() in emulator/emul_mapped.c
 *             coff_getxfile() in server/bsd/kern_exec.c
 *             obreak() in server/bsd/kern_mman.c
 *             setrlimit() in server/bsd/kern_resource.c
 *
 * Revision 1.5  1994/04/05  15:56:13  jlitvin
 * Add OSF changes for real-time support.
 *
 *  Risk: low
 *  Benefit or PTS #: 8207
 *  Module(s): bsd/kern_resource.c & uxkern/misc.c
 *
 * Revision 1.4  1994/01/13  17:53:29  jlitvin
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
 * Revision 1.3  1993/07/14  17:47:44  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:47:02  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:15:50  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:09  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:06:54  cfj
 * Bump major revision number.
 *
 * Revision 2.15  94/02/28  11:11:16  rabii
 * 	Added real-time support
 * 
 * Revision 2.14  1992/09/28  16:30:18  roman
 * Add physical process system operations (ppsops) to allow getting and
 * 	setting the nice values for all processes in a process set
 * 	on the current node.
 *
 * Revision 2.13  92/07/08  09:05:07  roman
 * Change typo in RCS comment for last submission.
 * 
 * Revision 2.12  92/07/07  15:08:10  roman
 * Change unix_master() and unix_release() during pproc operations to invoke
 * 	TNC_unix_master() and TNC_unix_release() instead.
 * 
 * Revision 2.11  92/07/07  13:07:32  klh
 * 	Revision 2.10  92/06/30  22:45:45  loverso
 * 		Added NULL ruid and rgid parameters to credentials_set_state() 
 * 		(rabii)
 * 
 * Revision 2.10  92/04/06  15:50:28  chrisp
 * Add master locking inside pproc_set_nice().
 * 
 * Revision 2.9  92/03/27  17:33:46  roman
 * Add extra (null) task pointer parameter to credentials_set_state().
 * 
 * Revision 2.8  92/03/15  14:39:17  roy
 * 	Added new credentials interfaces (rabii)
 * 
 * Revision 2.7  91/12/17  13:12:22  roy
 * 	91/10/21  18:47:10  emcmanus
 * 	Include kern/parallel.h for the fake syscall_on_master needed by 
 * 	asserts.    
 * 
 * Revision 2.6  91/12/08  10:56:44  rabii
 * 	Fixed parameters to modify_credentials
 * 
 * Revision 2.5  91/12/08  10:01:07  rabii
 * 	Addec call to modify_credentials
 * 
 * Revision 2.4  91/10/04  14:46:44  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:35:20  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Move system call handling for nice values to vproc directory. Change
 * 	interface to donice() to correspond to vproc rules.
 * 
 * Revision 2.2  91/08/31  13:22:01  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/12  15:35:05  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.11.7.2  91/04/29  15:09:26  garyf
 * 	fix error path in setrlimit()
 * 	[91/04/29  15:04:47  garyf]
 * 
 * Revision 1.11.3.3  91/03/20  13:33:04  seiden
 * 	Added kill access check in donice() for setpriority system call.
 * 	[91/03/12  15:33:19  seiden]
 * 
 * Revision 1.11.3.2  91/03/11  14:52:58  seiden
 * 	Apply same conditions to SEC_BASE side of donice as are enforced under #else clause.
 * 	[91/02/22  15:59:34  seiden]
 * 
 * Revision 1.11  90/10/31  13:49:14  devrcs
 * 	Replace all references of NZERO with PRIZERO.
 * 	Changed the rather confused conversion from bsd's -20..+20 priorities to
 * 	Machs 0-32.
 * 	[90/10/25  12:46:51  sp]
 * 
 * Revision 1.10  90/10/07  13:17:40  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:56:31  gm]
 * 
 * 	Fixes for nice.
 * 	[90/09/27  17:07:57  collins]
 * 
 * 	fixes to setrlimit:
 * 	 don't shrink stack
 * 	 implement code for maximum rlimit
 * 	[90/09/27  15:18:06  garyf]
 * 
 * Revision 1.9  90/08/24  11:16:54  devrcs
 * 	Changes for new system call interface.
 * 	[90/08/17  17:37:20  nags]
 * 
 * Revision 1.8  90/07/17  11:18:58  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:51:56  seiden]
 * 
 * Revision 1.7  90/06/22  20:06:10  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:53:48  seiden]
 * 
 * 	nags merge
 * 	[90/06/12  19:06:18  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:39:58  seiden]
 * 
 * Revision 1.6  90/02/16  16:48:18  devrcs
 * 		Change to use 4.4BSD pgrp structure.
 * 	[90/02/06  14:53:29  coren]
 * 
 * Revision 1.5  90/02/05  15:46:34  robert
 * 	Removed include of <sys/dir.h>
 * 	[90/01/18  19:11:37  gmf]
 * 
 * Revision 1.4  90/01/02  20:00:32  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.3  89/12/26  09:19:13  gm
 * 	Fixed include of fs.h to use ufs path.
 * 	Need to set u.u_error; suser doesn't do it any more.
 * 	Changed to new suser(); inode->vnode include.
 * 	[89/12/25            gmf]
 * 
 * Revision 1.2  89/10/26  07:46:48  gm
 * 	MACH X115 Update
 * 
 * Revision 6.1  89/07/26  15:28:43  alan
 * 	Mach Release 2.5 (preliminary) merged with Encore Multimax
 * 	support and BSD parallelization changes.
 * 
 * Revision 2.10  89/10/11  13:37:52  dlb
 * 	Change priority computation in donice().
 * 	Have donice() call new interface routines.
 * 	[89/05/11            dlb]
 * 
 * Revision 2.9  89/05/21  22:35:15  mrt
 * 	Replaced include of mach/mach.h with mach/mach_interface.h
 * 	[89/05/21            mrt]
 * 
 * Revision 2.8  89/04/18  16:42:07  mwyoung
 * 	Include <mach/vm_param.h>.
 * 	[89/01/28            mwyoung]
 * 
 * Revision 2.7  89/02/25  14:41:56  gm0w
 * 	Changes for cleanup.
 * 
 * Revision 2.6  89/02/09  04:31:19  mwyoung
 * 	Code cleanup cataclysm.
 * 
 * Revision 2.5  89/01/15  16:17:17  rpd
 * 	Updated includes for the new style, new mach/ directory.
 * 	[89/01/15  14:47:20  rpd]
 * 
 * Revision 0.0  88/06/16            mwyoung
 * 	Actually grow or shrink stacks when requested in setrlimit().
 * 	[88/06/16            mwyoung]
 * 
 * Revision 0.0  88/05/04            dlb
 * 	MACH_TIME_NEW is now standard.
 * 	[88/05/04            dlb]
 * 
 * Revision 0.0  88/03/29            mwyoung
 * 	MACH: Removed use of "sys/vm.h".
 * 	[88/03/29            mwyoung]
 * 
 * Revision 0.0  88/03/02            dlb
 * 	Use thread_read_times to get times for self.  This replaces
 * 	and generalized the MACH_TIME_NEW code.
 * 	[88/03/02            dlb]
 * 
 * Revision 0.0  87/11/21            avie
 * 	Reduced conditionals, purged history.
 * 	[87/11/21            avie]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_resource.c	7.1 (Berkeley) 6/5/86
 */

#include <sys/secdefines.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <ufs/fs.h>
#include <sys/uio.h>
#include <sys/kernel.h>
#ifndef	OSF1_SERVER
#include <sys/vm.h>

#include <kern/thread.h>
#include <mach/time_value.h>
#include <mach/mach_interface.h>
#include <mach/vm_param.h>
#else	/* OSF1_SERVER */
#include <sys/syscall.h>
#include <uxkern/import_mach.h>
#include <machine/vmparam.h>
#include <kern/parallel.h>
#endif	/* OSF1_SERVER */
#if	SEC_BASE
#include <sys/security.h>
#endif	

extern	int	real_time;

pproc_get_nice(p, n)
	register struct proc *p;
	register int *n;
{
	*n = p->p_nice;
	return(0);
}

pps_procset_get_nice(ps, n)
	struct procset *ps;
	register int *n;
{
	register struct proc *p;
	int low = PRIO_MAX + 1;
	
	TNC_unix_master();
	for (p = allproc; p != NULL; p = p->p_nxt) {
		if (proc_in_procset(p, ps) && p->p_nice < low)
			low = p->p_nice;
	}
	TNC_unix_release();
	*n = low;
	return(0);
}

pproc_set_nice(chgp, cur_uid, cur_ruid, has_priv, n)
	register struct proc *chgp;
	register uid_t cur_uid;
	uid_t cur_ruid;
	int has_priv;
	register int n;
{
	register uid_t chg_uid;
#ifndef	OSF1_SERVER
	register thread_t th;
	int	pri;
#endif	
#ifdef	ibmrt
	short	chg_nice;
#else
	char	chg_nice;
#endif

	BM(PROC_LOCK(chgp));
	chg_uid = chgp->p_rcred->cr_uid;
	chg_nice = chgp->p_nice;
	BM(PROC_UNLOCK(chgp));

#if	SEC_BASE
	if (cur_uid != chg_uid && 
            cur_ruid != chg_uid && 
            !has_priv))
#else	
	if (cur_uid && cur_ruid &&
	    cur_uid != chg_uid && 
            cur_ruid != chg_uid)
#endif
		return (EPERM);
#if SEC_ARCH
	if (!sec_can_kill(chgp))
		return (EACCES);
#endif
	if (n > PRIO_MAX)
		n = PRIO_MAX;
	if (n < PRIO_MIN)
		n = PRIO_MIN;
#ifndef	OSF1_SERVER
	th = chgp->thread;
	pri = BASEPRI_USER + (n - PRIZERO)/2;
#endif
	if ((n < chg_nice) && !has_priv)
		return (EACCES);
#ifndef	OSF1_SERVER
	else if (pri < th->max_priority)
		/*
		 *	Unix superuser gets to raise max priority.
		 */
		thread_max_priority(th, th->processor_set, pri);
	PROC_LOCK(chgp);
	chgp->p_nice = n;
	PROC_UNLOCK(chgp);
	thread_priority(th, pri, TRUE);
#else	/* OSF1_SERVER */
	TNC_unix_master();
	PROC_LOCK(chgp);
	chgp->p_nice = n;
	PROC_UNLOCK(chgp);
        {
            thread_array_t      thread_list;
            unsigned int        thread_count;
            register int        i, newpri;

	    if (real_time) {
            	newpri = 42 + n/2;
	    } else {
            	newpri = 12 + n/2;
	    }

            (void) task_threads(chgp->p_task, &thread_list, &thread_count);
            for (i = 0; i < thread_count; i++)
                if (MACH_PORT_VALID(thread_list[i])) {
                    set_thread_priority(thread_list[i], newpri);
                    (void) mach_port_deallocate(mach_task_self(),
                                                thread_list[i]);
                }
            (void) vm_deallocate(mach_task_self(),
                                 (vm_offset_t)thread_list,
                                 (vm_size_t)thread_count * sizeof(thread_t));
        }
	TNC_unix_release();
#endif	/* OSF1_SERVER */
	return (0);
}

pps_procset_set_nice(ps, cur_uid, cur_ruid, has_priv, n)
	struct procset *ps;
	uid_t cur_uid;
	uid_t cur_ruid;
	int has_priv;
	int n;
{
	register struct proc *p;
	int found = 0;
	int error = 0;
	
	TNC_unix_master();
	for (p = allproc; p != NULL; p = p->p_nxt) {
		if (proc_in_procset(p, ps)) {
			error = pproc_set_nice(p, 
					       cur_uid, 
					       cur_ruid, 
					       has_priv, 
					       n);
			found++;
		}
	}
	TNC_unix_release();
	if (!found)
		return(ESRCH);
	return(error);
}

setrlimit(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		u_int	which;
		struct	rlimit *lim;
	} *uap = (struct args *) args;
	struct rlimit alim;
	register struct rlimit *alimp;
	int error;

	ASSERT(syscall_on_master());
	if (uap->which >= RLIM_NLIMITS)
		return (EINVAL);
	alimp = &u.u_rlimit[uap->which];
	if (error = 
	    copyin((caddr_t)uap->lim, (caddr_t)&alim, sizeof (struct rlimit)))
		return (error);
	if (alim.rlim_cur > alimp->rlim_max || alim.rlim_max > alimp->rlim_max)
#if	SEC_BASE
		if (!privileged(SEC_LIMIT, EPERM))
			return (EPERM);
#else
		if (error = suser(u.u_cred, &u.u_acflag))
			return (error);
#endif	
	if (uap->which == RLIMIT_STACK) {
		vm_offset_t 	new_stack;
		vm_offset_t	old_stack;
		kern_return_t	ret;

		if (u.u_stack_grows_up) {
                        if (alim.rlim_cur == RLIM_INFINITY)
                           alim.rlim_cur = MAXSSIZ;
			new_stack = trunc_page(u.u_stack_start) +
					round_page(alim.rlim_cur);
			old_stack = (vm_offset_t) u.u_stack_end;
			if (new_stack > old_stack) {
#ifdef	OSF1_SERVER
				ret = vm_allocate(u.u_procp->p_task,
#else
				ret = vm_allocate(current_task()->map,
#endif
					&old_stack,
					(new_stack - old_stack),
					FALSE);
			} else if (new_stack < old_stack) {
		          /* don't deallocate vm space;
                             this could free stack currently in use */
                                ret = KERN_SUCCESS;
			} else
				ret = KERN_SUCCESS;

			if (ret == KERN_SUCCESS)
				u.u_stack_end = (caddr_t) new_stack;
		} else {
                        if (alim.rlim_cur == RLIM_INFINITY)
                           alim.rlim_cur = MAXSSIZ;
			new_stack = round_page(u.u_stack_end) -
					round_page(alim.rlim_cur);
			old_stack = (vm_offset_t) u.u_stack_start;
			if (new_stack < old_stack) {
#ifdef	OSF1_SERVER
				ret = vm_allocate(u.u_procp->p_task,
#else
				ret = vm_allocate(current_task()->map,
#endif
					&new_stack,
					(old_stack - new_stack),
					FALSE);
			} else if (new_stack > old_stack) {
		          /* don't deallocate vm space;
                             this could free stack currently in use */
                                ret = KERN_SUCCESS;
			} else
				ret = KERN_SUCCESS;

			if (ret == KERN_SUCCESS)
				u.u_stack_start = (caddr_t) new_stack;
		}
		if (ret != KERN_SUCCESS) {
			error = EINVAL;
                        return(error);
		}
	}
	*alimp = alim;
#ifndef	OSF1_SERVER
	if (uap->which == RLIMIT_RSS)
		p->p_maxrss = alim.rlim_cur/NBPG;
#endif
	credentials_set_state(p->p_cred, (pid_t *) NULL, (pid_t *) NULL,
				(struct ucred *) NULL, (int *) NULL,
				&p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_cur,
				&p->p_utask.uu_rlimit[RLIMIT_FSIZE].rlim_max,
				(uid_t *) NULL,
				(uid_t *) NULL,
				(task_t *) NULL);
	return (error);
}

getrlimit(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		u_int	which;
		struct	rlimit *rlp;
	} *uap = (struct args *) args;
	ASSERT(syscall_on_master());
	if (uap->which >= RLIM_NLIMITS)
		return (EINVAL);
	return (copyout((caddr_t)&u.u_rlimit[uap->which], (caddr_t)uap->rlp,
	    sizeof (struct rlimit)));
}

#ifdef	OSF1_SERVER
#include <uxkern/proc_to_task.h>
int
bsd_getrusage(proc_port, interrupt, which, rusage)
mach_port_t	proc_port;
boolean_t	*interrupt;
int		which;
struct rusage	*rusage;	/* OUT */
{
	register int		error;
	register struct rusage	*rup;
	mach_port_t		cport;

	error = start_server_op(proc_port, SYS_getrusage);
	if (error)
		return (error);

	cport = u.u_procp->p_cred;
	switch (which) {
	case RUSAGE_SELF:
		rup = &u.u_ru;
		credentials_get_rusage(cport, &rup->ru_inblock, 
					&rup->ru_oublock);
		break;
	case RUSAGE_CHILDREN:
		rup = &u.u_cru;
		break;
	default:
		return (end_server_op(EINVAL, interrupt));
	}
	*rusage = *rup;
	return (end_server_op(0, interrupt));
}

/* The emulator should be using the MIG function bsd_getrusage: */
getrusage()
{
	panic("getrusage - MiG");
}
#else	/* OSF1_SERVER */
getrusage(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int	who;
		struct	rusage *rusage;
	} *uap = (struct args *) args;
	register struct rusage *rup;
	time_value_t		sys_time, user_time;
	register struct timeval	*tvp;

	ASSERT(syscall_on_master());
	switch (uap->who) {

	case RUSAGE_SELF:
		/*
		 *	This is the current_thread.  Don't need to lock it.
		 */
		thread_read_times(current_thread(), &user_time, &sys_time);
		tvp = &u.u_ru.ru_utime;
		tvp->tv_sec = user_time.seconds;
		tvp->tv_usec = user_time.microseconds;
		tvp = &u.u_ru.ru_stime;
		tvp->tv_sec = sys_time.seconds;
		tvp->tv_usec = sys_time.microseconds;
		rup = &u.u_ru;
		break;

	case RUSAGE_CHILDREN:
		rup = &u.u_cru;
		break;

	default:
		return (EINVAL);
	}
	return (copyout((caddr_t)rup, (caddr_t)uap->rusage,
	    sizeof (struct rusage)));
}
#endif	/* OSF1_SERVER */

ruadd(ru, ru2)
	register struct rusage *ru, *ru2;
{
	register long *ip, *ip2;
	register int i;

	timevaladd(&ru->ru_utime, &ru2->ru_utime);
	timevaladd(&ru->ru_stime, &ru2->ru_stime);
	if (ru->ru_maxrss < ru2->ru_maxrss)
		ru->ru_maxrss = ru2->ru_maxrss;
	ip = &ru->ru_first; ip2 = &ru2->ru_first;
	for (i = &ru->ru_last - &ru->ru_first; i > 0; i--)
		*ip++ += *ip2++;
}
