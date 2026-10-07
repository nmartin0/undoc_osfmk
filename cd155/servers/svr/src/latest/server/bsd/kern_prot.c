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
 * $Log: kern_prot.c,v $
 * Revision 1.9  1994/11/18  20:27:10  mtm
 * Copyright additions/changes
 *
 * Revision 1.8  1994/10/25  22:55:19  yazz
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
 * Side-thread changes.  Augment panic message.
 *
 * Revision 1.7  1994/01/13  17:53:34  jlitvin
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
 * Revision 1.6  1993/07/14  17:47:37  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:46:57  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/04/03  03:03:55  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/16  20:02:45  brad
 * Merged trunk (as of the T8_EATS_PASSED tag) into the PFS branch.
 *
 * Revision 1.4  1993/01/18  18:47:54  cfj
 * Removed unnecessary printf in crfree().
 *
 * Revision 1.1.2.1.2.1  1992/12/16  05:58:27  brad
 * Merged trunk (as of the Main_After_Locus_12_1_92_Bugdrop_OK tag)
 * into the PFS branch.
 *
 * Revision 1.3  1992/12/11  02:54:25  cfj
 * Merged 12-1-92 bug drop from Locus.
 *
 * Revision 1.2  1992/11/30  22:15:47  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:06  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 2.15  92/11/23  15:54:44  klh
 * 	Revision 2.15  92/11/13  13:13:01  loverso
 * 		Fix NULL dereference in sameuser().
 * 
 * 	Revision 2.14  92/11/12  22:21:57  loverso
 * 		Intel's change to test for sameuser().
 * 
 * Revision 2.14  92/10/06  12:09:24  roman
 * Fix RCS comments.
 * 
 * Revision 2.13  92/10/05  13:37:28  klh
 * 	Revision 2.13  92/09/11  09:26:38  rabii
 * 		If pruid and/or prgid are given to substitute_real_creds(),
 * 		pass them along to credentials_set_state(). (mmp)
 * 
 * Revision 2.12  92/07/07  13:05:38  klh
 * 	Revision 2.12  92/06/30  22:45:43  loverso
 * 		Added NULL ruid and rgid parameters to credentials_set_state(). 
 *		(rabii)
 * 
 * Revision 2.11  92/03/27  17:32:57  roman
 * Add extra (null) task parameter to credentials_set_state().
 * 
 * Revision 2.10  92/03/15  14:38:57  roy
 * 	Replaced modify_credentials with new interface (rabii)
 * 
 * Revision 2.9  92/01/09  16:17:12  roy
 * 	Last change no longer needed.
 * 
 * Revision 2.8  92/01/05  18:20:03  roy
 * 	92/01/04  15:34:43  roy
 * 	Check special dummy value for OSF1_ADFS (under MACH_ASSERT) in crfree.
 * 
 * Revision 2.7  91/12/17  13:56:33  roy
 * 	91/10/21  18:47:06  emcmanus
 * 	Include kern/parallel.h for the fake syscall_on_master needed 
 * 	by asserts.
 * 
 * Revision 2.6  91/12/08  09:59:59  rabii
 * 	Added call to modify_credentials to update master cred server
 * 
 * Revision 2.5  91/11/22  14:54:25  rabii
 * 	Locus Merge
 * 	pproc_set_pgrp_sid() removed. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:46:29  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:34:47  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	File restructured, with system calls concerning getting or setting
 * 	process group and session ids moved into the vproc pvproc directories.
 * 
 * Revision 2.2  91/08/31  13:21:55  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/12  15:34:43  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13.2.3  90/12/20  12:10:15  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.13.2.2  90/12/06  15:29:06  swallace
 * 	Updated setuid() and setgid() to return EINVAL if the argument is
 * 	less than 0.  Setuid() also returns EINVAL if the argument is larger
 * 	than UID_MAX.  These fixes are needed to pass the SVVS.
 * 	[90/12/04  14:05:47  swallace]
 * 
 * Revision 1.13  90/10/31  13:49:07  devrcs
 * 	Changed setpgid and setpgrp to return the return value of their work
 * 	routine, setpgrp1, to their caller, syscall.
 * 	[90/10/26  16:41:17  hankin]
 * 
 * 	Fixed test in setpgrp1 so that only a child that had done exec()
 * 	would fail, not the calling process itself.
 * 	[90/10/08  09:03:08  coren]
 * 
 * Revision 1.12  90/10/07  13:17:34  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:56:22  gm]
 * 
 * 	Fixed groupmember() to include the effective group.
 * 	[90/09/24  12:48:26  collins]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:24  jeffc]
 * 
 * 	Changed the group management to separate cr_gid from the cr_groups
 * 	 array.
 * 	[90/09/21  11:07:15  collins]
 * 
 * Revision 1.11  90/09/23  15:43:09  devrcs
 * 	Fix getgroups when grouplist size is -ve
 * 	Check invalid uid/gid in setreuid,setuid,setreguid,setgid
 * 	[90/09/13  16:47:11  sp]
 * 
 * 	Fixed conformance bug #777
 * 	[90/09/10  10:49:23  collins]
 * 
 * Revision 1.10  90/08/24  11:16:43  devrcs
 * 	Changes for new system call interface.
 * 	Removed include of syscontext.h
 * 	[90/08/17  17:37:05  nags]
 * 
 * 	Use consistent credentials macros.
 * 	Eliminate old, useless QUOTA code.
 * 	[90/08/19  00:02:13  nags]
 * 
 * 	Something to do with security
 * 	[90/08/15  10:54:28  hosking]
 * 
 * 	fixed a case where privileged() wasn't passed enough arguments
 * 	[90/08/14  10:06:29  hosking]
 * 
 * 	Redid some of the pgrp/controlling tty compatibility code.
 * 	[90/08/12  12:06:29  ers]
 * 
 * Revision 1.9  90/07/27  08:44:01  devrcs
 * 	Fix credentials leak.
 * 	[90/07/20  13:50:04  nags]
 * 
 * 	Return an error in getlogin() if the the logname is a null string
 * 	(meaning that no setlogin() has ever been done).
 * 	[90/07/11  17:33:27  ers]
 * 
 * Revision 1.8  90/07/17  11:18:50  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:51:49  seiden]
 * 
 * Revision 1.7  90/06/22  20:06:06  devrcs
 * 	Post-nags-merge bug fixes
 * 	[90/06/18  09:53:40  seiden]
 * 
 * 	Fix TLB miss caused by calling set_gids() with wrong args in setgid().
 * 	[90/06/13  20:00:02  brezak]
 * 
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Parallelized for OSF/1.					nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing		seiden@osf.org
 * 	Removed pageable flag from zinit argument list		jvs@osf.org
 * 	Added backwards compatiblity code to setsid		ers@osf.org
 * 	Upgraded for POSIX 1003.1 compliance.			coren@osf.org
 * 	Removed include of <sys/dir.h>				gmf@osf.org
 * 	Fixes for first snapshot.				gm@osf.org
 * 	Copy credentials before re-setting, 4.4 stuff.		gmf@osf.org
 * 	Mach 2.5 and Encore 0.6 merge.				gm@osf.org
 * 	Integrated early 2.5 with Encore parallelization.	alan@encore.com
 * 	Use new identity zone instead of kalloc.		jsb@cmu.edu
 * 	More cleanup.						rpd@cmu.edu
 * 	Changes for cleanup.					gm0w@cmu.edu
 * 	Code cleanup cataclysm.					mwyoung@cmu.edu
 * 	Made QUOTA a #if-type option.				bolosky@cmu.edu
 * 	[90/06/12  19:06:11  gmf]
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
 *	@(#)kern_prot.c	7.1 (Berkeley) 6/5/86
 */

#include <sys/secdefines.h>

/*
 * System calls related to processes and protection
 */

#include <machine/reg.h>

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/acct.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/proc.h>
#include <sys/timeb.h>
#include <sys/times.h>
#include <sys/reboot.h>
#include <ufs/fs.h>
#include <sys/acct.h>
#include <sys/mount.h>
#include <kern/zalloc.h>
#if     SEC_BASE
#include <sys/security.h>
#endif  
#if     SEC_ARCH
#include <sys/secpolicy.h>
#endif 
#ifdef OSF1_SERVER
#include <kern/parallel.h>	/* syscall_on_master(). */
#endif

zone_t cred_zone;

/*
 * A reminder about credentials locking.  The system call handler
 * stashes a pointer in the uthread structure to the task's
 * credentials, which normally live in the proc structure.  At
 * the time the syscall handler grabs the pointer, under PROC_LOCK,
 * it also increments the credentials' reference count so that
 * they will not later disappear.  At the end of the system call,
 * the handler disposes of the uthread's reference to the
 * credentials, potentially deallocating the credentials as well.
 *
 * Each invocation of a system call is thereby guaranteed a copy
 * of the credentials that will not change while the system call
 * executes.  Furthermore, in the typical case no locks need to
 * be taken when examining credentials because we never change
 * credentials in place.
 *
 * When changing credentials, however, we generally ignore the
 * credentials supplied by the system call handler (u.u_cred)
 * in favor of using the actual credentials in the proc structure
 * (p_rcred).  The PROC_LOCK guards the p_rcred field and the
 * miscellaneous credentials in the proc structure (p_r{g,u}id
 * and p_sv{g,u}id).  These miscellaneous credentials are a special
 * case and must always be examined and modified under PROC_LOCK.
 */

/*
 * Macros to determine whether POSIX setuid/setgid calls apply to real as
 * well as effective. For now these are just tests for the caller being 
 * super-user, but they could be changed in the future.
 */

#if     SEC_BASE
#define CAN_SET_REAL_UGID()    (privileged(SEC_SETPROCIDENT, 0))
#else
#define CAN_SET_REAL_UGID() (suser(u.u_cred,&u.u_acflag)==0)
#endif

#if     !SEC_BASE
/*
 * Test if the current user is the super user.
 */
suser(cred, ac_flag)
	struct ucred *cred;
	struct flag_field *ac_flag;
{
	if (cred->cr_uid == 0) {
		if (ac_flag) {
			FLAG_LOCK(ac_flag);
			ac_flag->fi_flag |= ASU;
			FLAG_UNLOCK(ac_flag);
		}
		return (0);
	}
	return (EPERM);
}

int
sameuser(
	struct ucred *ccred,
	struct ucred *dcred)
{
	if (ccred && dcred && ccred->cr_uid == dcred->cr_uid)
		return (0);
	return (EPERM);
}
#endif

/* ARGSUSED */
getuid(p, uap, retval)
	struct proc *p;
	void *uap;
	int *retval;
{
	BM(PROC_LOCK(p));
	*retval = p->p_ruid;
	BM(PROC_UNLOCK(p));
	retval[1] = u.u_cred->cr_uid;
	return (0);
}

/* ARGSUSED */
getgid(p, uap, retval)
	struct proc *p;
	void *uap;
	int *retval;
{
	BM(PROC_LOCK(p));
	*retval = p->p_rgid;
	BM(PROC_UNLOCK(p));
	retval[1] = u.u_cred->cr_gid;
	return (0);
}

getgroups(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	gidsetsize;
		int	*gidset;
	} *uap = (struct args *)args;
	register gid_t *gp;
	register int *lp;
	int groups[NGROUPS];
	int error;
	register struct ucred *cr;

	cr = u.u_cred;
	if (uap->gidsetsize == 0) {
		*retval = cr->cr_ngroups;
		return (0);
	}
	if (uap->gidsetsize < cr->cr_ngroups)
		return (EINVAL);
	uap->gidsetsize = cr->cr_ngroups;
	gp = cr->cr_groups;
	for (lp = groups; lp < &groups[uap->gidsetsize]; )
		*lp++ = *gp++;
	if (error = copyout((caddr_t)groups, (caddr_t)uap->gidset,
	    uap->gidsetsize * sizeof (groups[0])))
		return (error);
	*retval = uap->gidsetsize;
	return (0);
}

/*
 * internal functions to set uids and gids
 */

/*
 * Grab all of our credentials immediately.  Use the
 * current proc's credentials rather than the credentials
 * reserved for this thread by the syscall handler.
 */
fetch_real_creds(p, pruidp, psvuidp, prgidp, psvgidp, crp)
	struct proc *p;
	uid_t *pruidp, *psvuidp;
	gid_t *prgidp, *psvgidp;
	struct ucred **crp;
{
	PROC_LOCK(p);
	if (pruidp)
		*pruidp = p->p_ruid;
	if (psvuidp)
		*psvuidp = p->p_svuid;
	if (prgidp)
		*prgidp = p->p_rgid;
	if (psvgidp)
		*psvgidp = p->p_svgid;
	*crp = p->p_rcred;
	crhold(*crp);
	PROC_UNLOCK(p);
}

substitute_real_creds(p, pruid, psvuid, prgid, psvgid, newcr)
	struct proc *p;
	uid_t pruid, psvuid;
	gid_t prgid, psvgid;
	struct ucred *newcr;
{
	register struct ucred *opcred, *oucred;
	register uid_t *ruidp = NULL;
	register uid_t *rgidp = NULL;

	crhold(newcr);			/* account for current thread */
	PROC_LOCK(p);
	if (pruid != NOUID) {
		p->p_ruid = pruid;
		ruidp = &pruid;
	}
	if (psvuid != NOUID)
		p->p_svuid = psvuid;
	if (prgid != NOGID) {
		p->p_rgid = prgid;
		rgidp = &prgid;
	}
	if (psvgid != NOGID)
		p->p_svgid = psvgid;
	opcred = p->p_rcred;
	oucred = u.u_cred;
	u.u_cred = p->p_rcred = newcr;
	PROC_UNLOCK(p);
	crfree(opcred);
	crfree(oucred);
	credentials_set_state(p->p_cred, (pid_t *) NULL, (pid_t *) NULL,
				p->p_rcred, (int *) NULL,
				(unsigned int *) NULL,
				(unsigned int *) NULL,
				ruidp, rgidp,
				(task_t *) NULL);
}

set_uids(p, ruid, euid, svuid)
	struct proc *p;
	uid_t ruid;
	uid_t euid;
	uid_t svuid;
{
	struct ucred	*cr, *newcr;
	uid_t		pruid, psvuid;
	int	error;

	fetch_real_creds(p, &pruid, &psvuid, 0, 0, &cr);
	if (ruid == NOUID)
		ruid = pruid;

	if (ruid != pruid &&
#if	COMPAT_43
	    ruid != cr->cr_uid &&			/* XXX */
#endif
#if     SEC_BASE
	    (!privileged(SEC_SETPROCIDENT, EPERM))) {
		crfree(cr);
		return (EPERM);
	}
#else
	    (error = suser(cr, &u.u_acflag))) {
		crfree(cr);
		return (error);
	}
#endif
	if (euid == NOUID)
		euid = cr->cr_uid;
#if     SEC_BASE
	if (euid != cr->cr_uid && euid != pruid && euid != psvuid &&
	    (!privileged(SEC_SETPROCIDENT, EPERM))) {
		crfree(cr);
		return (EPERM);
	}
#else
	if (euid != cr->cr_uid && euid != pruid && euid != psvuid &&
	    (error = suser(cr, &u.u_acflag))) {
		crfree(cr);
		return (error);
	}
#endif

	/*
	 * Everything's okay, do it.
	 */

	/*
	 * Copy credentials so other references do not
	 * see our changes.
	 */
	newcr = crcopy(cr);
	newcr->cr_uid = euid;
	substitute_real_creds(p, ruid, svuid, NOGID, NOGID, newcr);
#if     SEC_ARCH
	SP_CHANGE_SUBJECT();
#endif
	return (0);
}

set_gids(p, rgid, egid, svgid)
	struct proc *p;
	int rgid;
	int egid;
	int svgid;
{
	struct ucred	*newcr, *cr;
	gid_t		prgid, psvgid;
	int	error;

	fetch_real_creds(p, 0, 0, &prgid, &psvgid, &cr);
	if (rgid == NOGID)
		rgid = prgid;
	if (rgid != prgid &&
#if	COMPAT_43_XXX
	    rgid != cr->cr_gid &&		/* XXX */
#endif
#if     SEC_BASE
	    (!privileged(SEC_SETPROCIDENT, EPERM))) {
		crfree(cr);
		return (EPERM);
	}
#else
	    (error = suser(cr, &u.u_acflag))) {
		crfree(cr);
		return (error);
	}
#endif
	if (egid == NOGID)
		egid = cr->cr_gid;
	if (egid != cr->cr_gid && egid != prgid && egid != psvgid &&
#if     SEC_BASE
	   (!privileged(SEC_SETPROCIDENT, EPERM))) {
		crfree(cr);
		return (EPERM);
	}
#else
	   (error = suser(cr, &u.u_acflag))) {
		crfree(cr);
		return (error);
	}
#endif
	newcr = crcopy(cr);
	newcr->cr_gid = egid;
	substitute_real_creds(p, NOUID, NOUID, rgid, svgid, newcr);
#if     SEC_ARCH
	SP_CHANGE_SUBJECT();
#endif
	return (0);
}

/* ARGSUSED */
setreuid(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		int	ruid;
		int	euid;
	} *uap = (struct args *) args;

	if ((uap->ruid < -1) || (uap->euid < -1))
		return(EINVAL);

#if SEC_BASE
	if(!issetluid())
		return (EPERM);
#endif
	return (set_uids(p, (uid_t) uap->ruid, (uid_t) uap->euid, NOUID));
}

/*
 * The POSIX setuid system call. If privileged user, sets all of real,
 * effective, and saved uids; otherwise sets effective only (provided it
 * matches one of real, effective, or saved).
 */

/* ARGSUSED */
setuid(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		uid_t	uid;
	} *uap = (struct args *)args;
	register uid_t uid;

	if (((int)uap->uid < 0) || (uap->uid > (uid_t)UID_MAX))
		return(EINVAL);

#if     SEC_BASE
	if (!issetluid())
		return (EPERM);
#endif
	uid = (uid_t) uap->uid;
	if (CAN_SET_REAL_UGID())
		return(set_uids(p, uid, uid, uid));
	else
		return(set_uids(p, NOUID, uid, NOUID));
}


/* ARGSUSED */
setregid(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		int	rgid;
		int	egid;
	} *uap = (struct args *)args;

	if ((uap->rgid < -1) || (uap->egid < -1))
		return(EINVAL);

#if SEC_BASE
	if(!issetluid())
		return (EPERM);
#endif
	return (set_gids(p, uap->rgid, uap->egid, NOGID));
}

/*
 * The POSIX setgid system call. If privileged user, sets all of real,
 * effective, and saved group gids; otherwise sets effective only (provided it
 * matches one of real, effective, or saved).
 */

/* ARGSUSED */
setgid(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		gid_t	gid;
	} *uap = (struct args *)args;
	register gid_t gid;

	if ((int)uap->gid < 0)
		return(EINVAL);

#if     SEC_BASE
	if (!issetluid())
		return (EPERM);
#endif
	gid = uap->gid;
	if (CAN_SET_REAL_UGID())
		return(set_gids(p, gid, gid, gid));
	else
		return(set_gids(p, NOGID, gid, NOGID));
}

/* ARGSUSED */
setgroups(p, args, retval)
	register struct proc *p;
	void *args;
	int *retval;
{
	register struct	args {
		u_int	gidsetsize;
		int	*gidset;
	} *uap = (struct args *)args;
	register gid_t *gp;
	register int *lp;
	int error, ngrp, groups[NGROUPS];
	struct ucred *newcr, *cr;

#if     SEC_BASE
	if (!issetluid() || !privileged(SEC_SETPROCIDENT, EPERM))
		return (EPERM);
#else 
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	ngrp = uap->gidsetsize;
	if (ngrp > NGROUPS)
		return (EINVAL);
	error = copyin((caddr_t)uap->gidset, (caddr_t)groups,
	    uap->gidsetsize * sizeof (groups[0]));
	if (error)
		return (error);
	cr = p->p_rcred;
	/*
	 * We need to account for our reference to this cred.  crcopy
	 * will decrement the credentials reference count and
	 * substitute_real_creds will decrement the reference count of
	 * both the u-area and proc creds.  So we bump the reference
	 * count here to account for the three unrefs.
	 */
	crhold(cr);
	newcr = crcopy(cr);
	gp = newcr->cr_groups;
	for (lp = groups; lp < &groups[uap->gidsetsize]; )
		*gp++ = *lp++;
	newcr->cr_ngroups = ngrp;
	substitute_real_creds(p, NOUID, NOUID, NOGID, NOGID, newcr);
#if     SEC_ARCH
	SP_CHANGE_SUBJECT();
#endif
	return (0);
}

/*
 * Check if gid is a member of the group set.
 * No locks needed because credentials are read-only.
 */
groupmember(gid, cred)
	gid_t gid;
	register struct ucred *cred;
{
	register gid_t *gp;
	gid_t *egp;

	if (gid == cred->cr_gid)
		return(1);
	egp = &(cred->cr_groups[cred->cr_ngroups]);
	for (gp = cred->cr_groups; gp < egp; gp++)
		if (*gp == gid)
			return (1);
	return (0);
}


/*
 * Routines to allocate and free credentials structures
 */

int	cractive = 0;
vdecl_simple_lock_data(,cractive_lock)
#define	CRSTAT(clause)		STATS_ACTION(&cractive_lock, (clause))


/*
 * Allocate a zeroed cred structure and reference it.
 */
struct ucred *
crget()
{
	register struct ucred *cr; 

	ZALLOC(cred_zone, cr, struct ucred *);
	LASSERT(cr->cr_lock.slthread == 0);
	bzero((caddr_t)cr, sizeof(*cr));
	CR_LOCK_INIT(cr);
	ASSERT(cr->cr_ref == 0);
	cr->cr_ref = 1;
	CRSTAT(cractive++);
	return(cr);
}

/*
 * Free a cred structure.
 * Throws away space when ref count gets to 0.
 * This can never be done from interrupt level.
 */
crfree(cr)
	struct ucred *cr;
{
	CR_LOCK(cr);

	ASSERT(cr->cr_ref > 0);
	if (--cr->cr_ref != 0) {
		CR_UNLOCK(cr);
		return;
	}

#if	MACH_ASSERT
	ASSERT(cr->cr_dummy == 0);
#endif
	CR_UNLOCK(cr);
#if	MACH_LDEBUG
	cr->cr_lock.slthread = 0;
	cr->cr_lock.slck_addr = 0;
	cr->cr_lock.sunlck_addr = 0;
#endif
	ZFREE(cred_zone, cr);
	CRSTAT(cractive--);
}

/*
 * Copy cred structure to a new one and free the old one.
 * No lock needed as long as we hold cr referenced.
 */
struct ucred *
crcopy(cr)
	struct ucred *cr;
{
	struct ucred *newcr;

	newcr = crget();
	*newcr = *cr;
	CR_LOCK_INIT(newcr);
	newcr->cr_ref = 1;
	crfree(cr);
	return(newcr);
}

/*
 * Dup cred struct to a new held one.
 * No lock needed as long as we hold cr referenced.
 */
struct ucred *
crdup(cr)
	struct ucred *cr;
{
	struct ucred *newcr;

	newcr = crget();
	ASSERT(newcr->cr_ref == 1);
	*newcr = *cr;
	CR_LOCK_INIT(newcr);
	newcr->cr_ref = 1;
 	return(newcr);
}

#if	MACH_ASSERT
cr_ref(cr)
	struct ucred *cr;
{
	CR_LOCK(cr);
	if (cr->cr_dummy != 0) {
		panic("cr_ref: attempt to ref non-cred, cred=0x%x dummy=0x%x\n",
				cr, cr->cr_dummy);
	}
	(cr)->cr_ref++;
	CR_UNLOCK(cr);
}
#endif

/*
 * Obtain the first handle on credentials to be used by the
 * current thread.   Anyone else resetting the credentials makes
 * a new copy so the caller of this routine will always have
 * the same credentials it began with.  Used by uarea_init and syscall
 *
 * If the creds passed in are NOCRED, they get assigned to those of
 * the process with which the thread is associated.  If they are valid,
 * they are unreferenced and then reassigned.
 */
cr_threadinit(th)
#ifdef	OSF1_SERVER
	uthread_t th;
#else	/* OSF1_SERVER */
	thread_t th;
#endif	/* OSF1_SERVER */
{
	register struct proc *p;
	register struct uthread *uthread;

#ifdef	OSF1_SERVER
	uthread = th;
#else	/* OSF1_SERVER */
	uthread = th->u_address.uthread;
#endif	/* OSF1_SERVER */
	if (uthread->uu_nd.ni_cred != NOCRED)
		crfree(uthread->uu_nd.ni_cred);
#ifdef	OSF1_SERVER
	p = th->uu_procp;
#else	/* OSF1_SERVER */
	p = th->u_address.utask->uu_procp;
#endif	/* OSF1_SERVER */
	PROC_LOCK(p);
	uthread->uu_nd.ni_cred = p->p_rcred;
	crhold(p->p_rcred);
	ASSERT(p->p_rcred->cr_ref >= 2);
	PROC_UNLOCK(p);
}


/*
 * Get login name, if available.
 */
/* ARGSUSED */
getlogin(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*namebuf;
		u_int	namelen;
	} *uap = (struct args *)args;
	char buf[MAXLOGNAME];
	int error;

	/*
	 * N.B.: 4.3 reno has put u_logname into the proc structure, as
	 *	p_logname.  Do we want to do this?
	 */

	if (uap->namelen > sizeof (u.u_logname))
		uap->namelen = sizeof (u.u_logname);
	U_HANDY_LOCK();
	bcopy(u.u_logname, buf, MAXLOGNAME - 1);
	U_HANDY_UNLOCK();
	/* Error if no setlogin() has been done */
	if (buf[0] == '\0')
		error = EINVAL;
	else
		error = copyout((caddr_t)buf, (caddr_t)uap->namebuf,
				    uap->namelen);
	return (error);
}

/*
 * Set login name.
 */
/* ARGSUSED */
setlogin(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	struct args {
		char	*namebuf;
	} *uap = (struct args *)args;
	char buf[MAXLOGNAME];
	int error;

#if     SEC_BASE
	if (!privileged(SEC_SETPROCIDENT, EPERM))
		return (EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
	error = copyinstr((caddr_t)uap->namebuf, (caddr_t)buf,
	    MAXLOGNAME - 1, (int *) 0);
	if (!error) {
		U_HANDY_LOCK();
		bcopy(buf, u.u_logname, MAXLOGNAME - 1);
		U_HANDY_UNLOCK();
	} else if (error == ENOENT)	/* name too long */
		error = EINVAL;
	return (error);
}

kern_prot_init()
{
	cred_zone = zinit(sizeof(struct ucred), 1024*1024, 0, "ucred");
	VSTATS_LOCK_INIT(&cractive_lock);
}
