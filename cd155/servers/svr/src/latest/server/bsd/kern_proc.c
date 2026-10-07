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
 * $Log: kern_proc.c,v $
 * Revision 1.11  1995/02/01  21:27:20  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.10  1994/11/18  20:27:06  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/03/24  20:41:23  yazz
 *  Reviewer: John Litvin
 *  Risk: Lo
 *  Benefit or PTS #: #8334
 *  Testing: boot and full multiuser (only panic messages changed)
 *  Module(s): server/bsd/kern_proc.c
 *
 * Join separage printf + panic calls into one panic call, to better catch
 * the unexpected MACH_RCV_TOO_LARGE returns.
 *
 * Revision 1.8  1994/02/28  15:33:24  cfj
 * Merge revision 1.6.4.1 from R1_2 into the main stem.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.7  1994/01/13  17:53:41  jlitvin
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
 * Revision 1.6.4.1  1994/02/28  15:30:23  cfj
 * Add the taske_by_pid(-17) option which allows us to
 * monitor server memory usage.
 *
 *  Reviewer:
 *  Risk:
 *  Benefit or PTS #:
 *  Testing:
 *  Module(s):
 *
 * Revision 1.6  1993/07/14  17:47:34  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:52  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/05/06  19:03:22  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.4  1993/04/03  03:03:53  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:58:25  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:22  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:15:44  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:04  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.14  92/11/12  14:11:32  chrisp
 * For TNC, update proc port sequence count in bsd_pid_by_task().
 * 
 * Revision 2.13  92/10/01  10:18:50  roman
 *
 * Revision 2.14  1992/11/12  14:11:32  chrisp
 * For TNC, update proc port sequence count in bsd_pid_by_task().
 *
 * Revision 2.13  92/10/01  10:18:50  roman
 * Fix up types for clean compilation under gcc.
 * 
 * Revision 2.12  92/09/28  16:28:57  roman
 * Change bsd_pid_by_task() to use a virtual process system operation (vpsop)
 * 	rather than having explicit TNC code present.
 * 
 * Revision 2.11  92/08/06  16:38:04  roman
 * Fix RCS comments.
 * 
 * Revision 2.10  92/08/06  13:34:43  klh
 * 	Revision 2.8  92/07/28  20:02:31  rabii
 * 		Change bsd_task_by_pid and pproc_get_task_port to return ESRCH
 *		if the mach_port_mod_refs returns KERN_INVALID_NAME, indicating
 *		the task has terminated.  Do not call proc_exit, as the
 *		dead-name notification on the task port should take care of it.
 *		(loverso)
 * 
 * Revision 2.9  92/07/08  09:04:44  roman
 * Change node numbers to type node_t.
 * Remove tnc_mynode variable, and use this_node variable instead
 * 	(this_node is used by the rest of OSF/1 AD).
 * 
 * Revision 2.8  92/07/07  15:07:06  roman
 * Minor format corrections to exactly match OSF source code.
 * 
 * Revision 2.7  92/06/17  09:11:06  roman
 * [Bug 0026] Change code in task_by_pid() and pid_by_task() for TNC so
 * 	that correct results are returned even if the process being
 * 	referenced is running on another node.
 * 
 * Revision 2.6  92/03/09  14:09:38  durriya
 * 	Revision 3.6  92/02/26  13:21:16  sp
 * 	Upgrade to 1.0.4
 * 
 * Revision 2.5  91/12/17  14:07:31  roy
 * 	91/12/13  13:02:43  sp
 * 	Correct calls to suser() and reorganise bsd_task_by_pid to more
 * 	resemble OSF/1
 * 
 * 	91/12/02  16:58:59  sp
 * 	Non functional changes to bsd_task_by_pid to bring the source closer
 * 	to 1.0.3
 * 
 * Revision 2.4  91/10/04  14:45:52  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:34:04  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Move pgrp and session relationship manipulation to the vproc and
 * 	pvproc directories.
 * 
 * Revision 2.2  91/08/31  13:21:48  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/07  17:17:09  jose
 * Adapted from 1.0.2 to OSF/1s environment
 * 
 * Revision 1.12  90/10/07  13:17:29  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:56:10  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:15  jeffc]
 * 
 * Revision 1.11  90/09/23  15:43:05  devrcs
 * 	Overly aggressive deletion of quota information
 * 	changed p_pgrpnxt to p_pgrpnx.
 * 	[90/09/04  08:51:17  nags]
 * 
 * 	No more quota information in proc structure.
 * 	[90/09/03  22:35:23  nags]
 * 
 * Revision 1.10  90/08/24  11:16:34  devrcs
 * 	Special handling of process group 0 in compatibility mode.
 * 	Need to preserve session, so that setpgrp(0,0) ... setpgrp(0,mypgrp)
 * 	will keep the controlling tty.
 * 	[90/08/12  12:06:23  ers]
 * 
 * Revision 1.9  90/07/05  23:07:38  devrcs
 * 	Activated proc_print, adding in the few fields that it was missing
 * 	[90/06/28  12:43:13  coren]
 * 
 * Revision 1.8  90/06/22  20:06:03  devrcs
 * 	nags merge
 * 
 * 	Condensed relevant history, reverse chronolgy:
 * 	Parallelized for OSF/1				nags@encore.com
 * 	Changed pidhash from short to pid_t 		ers@osf.org
 * 	Removed unnecessary include of mbuf.h		tmt@osf.org
 * 	Added proc_print for printing procs from kdb	ers@osf.org
 * 	Merged Robert Coren's and Rich Morris's changes	ers@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.		coren@osf.org
 * 	Mach 2.5 and Encore 0.6 merge			gm@osf.org
 * 	Merge Encore parallelization with Mach 2.5	alan@encore.com
 * 	Added lots of declarations 			rpd@cmu.edu
 * 	[90/06/12  19:06:04  gmf]
 * 
 * $EndLog$
 */
/*
 * Copyright (c) 1982, 1986 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *	@(#)kern_proc.c	7.1 (Berkeley) 6/5/86
 */

#ifndef	OSF1_SERVER
#include <mach_kdb.h>
#include <machine/reg.h>
#if	!defined(ibmrt) && !defined(mips)
#include <machine/psl.h>
#endif
#else	/* OSF1_SERVER */
#include <machine/reg.h>
#if	!defined(ibmrt) && !defined(mips)
#include <machine/psl.h>
#endif
#endif	/* OSF1_SERVER */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/map.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <kern/parallel.h>
#include <sys/vproc.h>
#include <sys/proc.h>
#include <ufs/quota.h>
#include <sys/acct.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/tty.h>
#include <uxkern/proc_to_task.h>

pid_t	pidhash[PIDHSZ];
struct	proc *proc, *procNPROC;	/* the proc table itself */
struct	proc *freeproc, *zombproc, *allproc;
			/* lists of procs in various states */
int	nproc;

struct proc *
pfind(pid)
	int pid;
{
	register struct proc *p;

	for (p = &proc[pidhash[PIDHASH(pid)]]; p != &proc[0]; p = &proc[p->p_idhash]) 
		if (p->p_pid == pid)
			return (p);
	return ((struct proc *)0);
}

/*
 * init the process queues
 */
pqinit()
{
	register struct proc *p;

	/*
	 * most procs are initially on freequeue
	 *	nb: we place them there in their "natural" order.
	 */

	freeproc = NULL;
	for (p = procNPROC; --p > proc; freeproc = p) {
		p->p_nxt = freeproc;
		PROC_LOCK_INIT(p);
	}

	/*
	 * but proc[0] is special ...
	 *
	 * N.B.  proc[0]'s proc lock was initialized in setup_main.
	 */

	allproc = p;
	p->p_nxt = NULL;
	p->p_prev = &allproc;

	zombproc = NULL;
}


#if	MACH_KDB

#define	printf	kdbprintf

extern int indent;

proc_print(p)
register struct proc *p;
{

	extern void cred_print();
	extern void print_simple_lock();

	iprintf("pid= %d p_link= %X p_rlink= %X p_nxt= %X\n",
		p->p_pid, p->p_link, p->p_rlink, p->p_nxt);
	iprintf("p_prev= %X p_usrpri= %x, p_pri= %x p_cpu= %x\n",
		p->p_prev, p->p_usrpri, p->p_pri, p->p_cpu);
	iprintf("p_stat= %x p_time= %x p_nice= %x p_slptime= %x\n",
		p->p_stat, p->p_time, p->p_nice, p->p_slptime);
	iprintf("p_cursig= %x p_sig= %X p_sigmask= %X p_sigignore= %X\n",
		p->p_cursig, p->p_sig, p->p_sigmask, p->p_sigignore);
	iprintf("p_sigcatch= 0x%X, p_flag= %X p_ruid= %d p_svuid= %d\n",
		p->p_sigcatch, p->p_flag, p->p_ruid, p->p_svuid);
	iprintf("p_rgid= %d p_svgid= %d p_ppid= %d p_xstat= %x\n",
		p->p_rgid, p->p_svgid, p->p_ppid, p->p_xstat);
        iprintf("p_pgid= %d, p_rcred= 0x%X, p_session= 0x%X\n",
		p->p_pgid, p->p_rcred, -1);
	iprintf("Credentials (0x%X):\n", p->p_rcred);
	indent += 4;
	if ((p->p_rcred != NOCRED) && (p->p_rcred != 0))
		cred_print(p->p_rcred);
	indent -= 4;
	iprintf("p_ru= %X p_rssize= %X p_maxrss= %X p_swrss= %X\n",
		p->p_ru, p->p_rssize, p->p_maxrss, p->p_swrss);
	iprintf("p_swaddr= %X p_stopsig= %X p_cpticks= %X p_pctcpu= %X p_ndx= %x\n",
		p->p_swaddr, p->p_stopsig, p->p_cpticks, p->p_pctcpu, p->p_ndx);
	/* LTCF? */
	iprintf("p_idhash= %x p_pptr= %X p_cptr= %X p_osptr= %X\n",
		p->p_idhash, -1, -1, -1);
	/* LTCF? */
	iprintf("p_ysptr= %X p_pgrp= %X p_pgrpnxt= %X\n",
		-1, -1, -1);
	iprintf("p_traceflag= %X p_tracep= %X p_logdev= %x task= %X\n",
		p->p_traceflag, p->p_tracep, p->p_logdev, p->task);
	iprintf("utask= %X thread= %X exit_thread= %X\n",
		p->utask, p->thread, p->exit_thread);
#if	MACH_SLOCKS
	indent += 4;
	print_simple_lock(&p->p_lock);
	indent -= 4;
#endif
}

#endif	/* MACH_KDB */

extern mach_port_t privileged_host_port;
extern mach_port_t device_server_port;

/*
 *	Routine:	bsd_task_by_pid
 *	Purpose:
 *		Get the task port for another "process", named by its
 *		process ID on the same host as "target_task".
 *
 *		Only permitted to privileged processes, or processes
 *		with the same user ID.
 */
int
bsd_task_by_pid(proc_port, interrupt, pid, target_task, tType)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	int		pid;
	task_t		*target_task;
	mach_msg_type_name_t *tType;
{
	struct vproc	*vp;
	int		error;

	if (error = start_server_op(proc_port, 1001))
	    return (error);

	if (pid == -17 || pid == -2 || pid == -1 || pid == 0) {

		unix_master();

		if (pid == -17) {
#if	SEC_BASE
			if (privileged(SEC_DEBUG, EPERM))
#else
			if (!suser(u.u_cred, &u.u_acflag))
#endif
				*target_task = mach_task_self();
			else
				error = EACCES;
		} else if (pid == -2) {
#if	SEC_BASE
			if (privileged(SEC_DEBUG, EPERM))
#else
			if (!suser(u.u_cred, &u.u_acflag))
#endif
				*target_task = device_server_port;
			else
				error = EACCES;
		} else if (pid == -1) {
#if     SEC_BASE
			if (privileged(SEC_DEBUG, EPERM))
#else
			if (!suser(u.u_cred, &u.u_acflag))
#endif
				*target_task = privileged_host_port;
			else
				error = EACCES;
		} else {
#if	SEC_BASE
			if (privileged(SEC_DEBUG, EPERM))
#else
			if (!suser(u.u_cred, &u.u_acflag))
#endif
				*target_task = proc[0].p_task;  /* UX server */
			else
				error = EACCES;
		}

		if (!error) {
			kern_return_t kr;

			/*
		 	 * Give ourself another send right for the task port,
			 * and specify that the send right should be moved
			 * into the reply message.  This way there is no problem
			 * if the task port should be destroyed before the
			 * the reply message is sent.
			 */

			kr = mach_port_mod_refs(mach_task_self(), *target_task,
						MACH_PORT_RIGHT_SEND, 1);
			if (kr == KERN_SUCCESS) {
				*tType = MACH_MSG_TYPE_MOVE_SEND;
			} else if (kr == KERN_INVALID_RIGHT
					|| kr == KERN_INVALID_NAME) {
				/* The task has probably terminated */
				error = ESRCH;
			} else {
				panic(
				 "bsd_task_by_pid: pid=%d task=0x%x ret=0x%x\n",
				 pid, *target_task, kr);
			}
		}

		unix_release();
	
	} else {
		vp = LOCATE_VPROC_PID(pid);
		if (vp != NULL) {
			error = VPOP_GET_TASK_PORT(vp, target_task);
			if (error == ESUCCESS)
				*tType = MACH_MSG_TYPE_MOVE_SEND;
			VPROC_RELEASE(vp, "bsd_task_by_pid");
		} else
			error = ESRCH;
	}

	if (error) {
		/*
		 * If we can't produce a real send right,
		 * then give our caller MACH_PORT_NULL instead
		 * of a unix error code which he would probably
		 * mistake for a port.
		 */

		*target_task = MACH_PORT_NULL;
		*tType = MACH_MSG_TYPE_MOVE_SEND;
		error = 0;
	}

	return (end_server_op(error, interrupt));
}

int
pproc_get_task_port(p, uid1, has_priv, target_task)
	struct proc 	*p;
	uid_t		uid1;
	int		has_priv;
	task_t		*target_task;
{
	uid_t		uid, ruid, svuid;
	int		error;
	kern_return_t	kr;
#if   SEC_MAC
	tag_t		*proc_tag;
	tag_t		*proc1_tag;
#endif

	unix_master();

	if (p->p_stat == SZOMB) {
		*target_task = MACH_PORT_NULL;
		error = ESRCH;
		goto out;
	}
	BM(PROC_LOCK(p));
	uid = p->p_rcred->cr_uid;
	ruid = p->p_ruid;
	svuid = p->p_svuid;
	BM(PROC_UNLOCK(p));
#if	SEC_BASE
#if	SEC_MAC
	/* XXX Does not stand a prayer currently XXX */
	/*
	 * make sure communication between these processes
	 * doesn't violate MAC restrictions
 	 */

	proc_tag = secinfo[p - proc].si_tag;
	proc1_tag = secinfo[p1 - proc].si_tag;
        if (((uid == uid1) && (uid == ruid) && (uid == svuid) &&
	     (!SP_ACCESS(proc_tag, proc1_tag, SP_READACC, NULL)) &&
	     (!SP_ACCESS(proc_tag, proc1_tag, SP_WRITEACC, NULL)))
	    || (privileged(SEC_DEBUG, EPERM)
			&& (target_task == current_task())))
#else
        if (((uid == uid1) && (uid == ruid) && (uid == svuid)) ||
	   (privileged(SEC_DEBUG, EPERM)
			&& (target_task == current_task())))
#endif
#else
	if (((uid == uid1) && (uid == ruid) && (uid == svuid)) || has_priv)
#endif
		*target_task = p->p_task;
	else {
		*target_task = MACH_PORT_NULL;
		error = EACCES;
		goto out;
	}

	/*
	 * Give ourself another send right for the task port,
	 * and specify that the send right should be moved
	 * into the reply message.  This way there is no problem
	 * if the task port should be destroyed before the
	 * the reply message is sent.
	 */

	kr = mach_port_mod_refs(mach_task_self(), *target_task,
				MACH_PORT_RIGHT_SEND, 1);
	if (kr == KERN_SUCCESS)
		error = ESUCCESS;
	else if (kr == KERN_INVALID_RIGHT || kr == KERN_INVALID_NAME) {
		*target_task = MACH_PORT_NULL;
		error = ESRCH;
	} else {
		printf("pid %d -> task 0x%x; mod_refs returned %d\n",
			p->p_pid, *target_task, kr);
		panic("pproc_get_task_port");
	}

out:
	unix_release();
	return(error);
}

int
bsd_pid_by_task(proc_port, interrupt, task, pid, comm, commlen, rval)
	mach_port_t	proc_port;
	boolean_t	*interrupt;
	task_t		task;
	pid_t		*pid;
	char		*comm;
	unsigned int	*commlen;
	int		*rval;
{
	int		error;

	/* find the pid using a virtual process system operation */
	error = VPSOP_PID_FROM_TASK(task, pid, comm, commlen);
	if (error == ESUCCESS)
		*rval = 0;
	else
		*rval = -1;

	/* get rid of the send right */
	(void) mach_port_deallocate(mach_task_self(), task);

	*interrupt = FALSE;

#ifdef	TNC
	vproc_end_port_op(port_to_proc_lookup(proc_port)->p_vproc,
			  "start_server_op");
#endif	/* TNC */

	return ESUCCESS;
}


int
pps_pid_from_task(task, pid, comm, commlen)
	task_t			task;
	pid_t			*pid;
	char			*comm;
	unsigned int		*commlen;
{
	register struct proc	*p;
	int			len;
	int			error;

	unix_master();
	if (task == mach_task_self())
		p = &proc[0];
	else
		p = task_to_proc_lookup(task);
	if ((p == 0) || (p->p_flag & SWEXIT)) {
		*pid = 0;
		*commlen = 0;
		error = ESRCH;
	} 
	else {
		*pid = p->p_pid;
		len = strlen(p->p_utask.uu_comm);
		if (*commlen < len)
			len = *commlen;
		bcopy(p->p_utask.uu_comm, comm, len);
		*commlen = len;
		error = ESUCCESS;
	}
	unix_release();

	return (error);
}
