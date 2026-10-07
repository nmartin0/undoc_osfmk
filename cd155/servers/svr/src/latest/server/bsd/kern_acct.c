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
 * $Log: kern_acct.c,v $
 * Revision 1.10  1994/11/18  20:26:48  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/03/14  02:00:21  slk
 * Checkpoint Restart Code Drop
 *  Reviewer: Stefan Tritscher
 *  Risk: Medium
 *  Benefit or PTS #: Enhancement
 *  Testing: Locus VSTNC, EATS TCP-IP, Individual Checkpoint/Restart tests.
 *  Module(s):
 *
 * Revision 1.8  1994/01/13  17:54:20  jlitvin
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
 * Revision 1.7  1993/12/20  19:02:53  dleslie
 *  Reviewer: none
 *  Risk: low
 *  Benefit or PTS #: remove meaningless cast to pass 'lint'
 *  Testing: built
 *  Module(s): kern_acct.c
 *
 * Revision 1.6  1993/07/14  17:46:50  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:45:33  cfj
 * Adding new code from vendor
 *
 * Revision 1.5  1993/06/15  21:58:54  nandy
 * Fixed improper check for PFS file system. - DBM.
 *
 * Revision 1.4  1993/05/06  19:02:05  nandy
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.3  1993/04/03  03:03:41  brad
 * Merge of PFS branch (tagged PFS_End) into CVS trunk (tagged
 * Main_Before_PFS_Merge).  The result is tagged PFS_Merge_Into_Main_April_2.
 *
 * Revision 1.1.2.1.2.2  1993/02/12  17:26:08  dbm
 * Added code to check for PFS file in sysacct and return error.
 *
 * Revision 1.1.2.1.2.1  1993/02/09  21:44:53  brad
 * Added logic to allow a file's I/O mode to be set on a per-file basis,
 * rather than just a per-file system basis.
 *
 * Revision 1.2  1992/11/30  22:15:12  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:05:31  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:06:01  cfj
 * Bump major revision number.
 *
 * Revision 2.12  93/06/19  15:14:29  yazz
 * [ ad1.04 merge ] Take OSF's version of this file.
 * 	Revision 2.11  93/06/02  17:12:59  rabii
 * 		Added support for accounting where the process manager and 
 * 		accounting fileserver are on different nodes. (rabii)
 * 
 * 	Revision 2.10  93/04/08  11:19:25  loverso
 * 		ux server threads are wired by default. (loverso)
 *
 * Revision 2.10  92/06/05  13:54:52  klh
 * 	Revision 2.9  92/05/24  14:13:06  pjg
 * 		92/03/23  18:01:53  condict
 * 		Turn on the acctwatch thread.
 * 		[92/05/18            srl]
 * 
 * Revision 2.9  92/03/27  10:54:41  roman
 * Add extra parameter to VPOP_CTTY_GETATTR().
 * 
 * Revision 2.8  92/03/24  10:18:56  klh
 * For OSF merge, update version # to match LCC#
 * 
 * Revision 2.7  92/03/09  14:01:48  durriya
 * 	Revision 3.2  91/12/18  17:14:35  sp
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.6  92/01/02  18:49:41  roy
 * 	1991/10/14  19:58:46  noemi
 * 	Changed ni_segflg to UIO_SYSSPACE for OSF1_ADFS.
 * 
 * Revision 2.5  91/11/22  14:54:09  rabii
 * 	Locus Merge
 * 	VPOP_CTTY_GETATTR supersedes VPOP_GET_CTTY. (chrisp)
 * 
 * Revision 2.4  91/10/04  14:43:19  chrisp
 * Get rid of extraneous $Log.
 * 
 * Revision 2.3  91/09/16  15:30:20  rabii
 * 	Merge of V2.0 and Locus (locus check-in by chrisp)
 * 	Slight changes due to vproc operations.
 * 
 * Revision 2.2  91/08/31  13:21:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/05  13:54:00  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.12.2.3  90/12/20  12:09:53  devsrc
 * 	Merge 1.01 sandbox to osc1.0
 * 
 * Revision 1.12.2.2  90/12/11  17:17:29  dwm
 * 	SVVS errno fixes in sysacct()
 * 	[90/12/11  12:48:08  dwm]
 * 
 * Revision 1.12  90/10/31  13:48:33  devrcs
 * 	Changed acct to return 0 to syscall.
 * 	[90/10/26  16:38:46  hankin]
 * 
 * 	Add missing splhigh()'s around time lock acquisition in previous change.
 * 	[90/10/19  15:29:59  dlb]
 * 
 * 	Fixed incorrect usage of hzto().
 * 	[90/10/14  19:00:49  ers]
 * 
 * Revision 1.11  90/10/07  13:16:37  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:54:26  gm]
 * 
 * Revision 1.10  90/09/23  15:42:46  devrcs
 * 	sysacct() used namei instead of vn_open and so
 * 	could "open" the accounting file without obeying
 * 	the normal constraints.  In particular, the file
 * 	could be written without having had its quota
 * 	fields attached.  Bug fix from chris@mimsy.umd.edu.
 * 	[90/09/08  19:03:38  nags]
 * 
 * Revision 1.9  90/08/24  11:15:28  devrcs
 * 	New system call interface
 * 	Should look at new 4.3 reno acct function -- some changes!
 * 	[90/08/10  16:01:26  nags]
 * 
 * 	Corrected bug in calculation of ac_mem field.  This bug dates back
 * 	to 1984!
 * 	[90/08/12  12:06:04  ers]
 * 
 * 	Corrected bug in calculation of ac_mem field.  This bug dates back
 * 	to 1984!
 * 	[90/08/12  12:06:04  ers]
 * 
 * 	Corrected bug in calculation of ac_mem field.  This bug dates back
 * 	to 1984!
 * 	[90/08/12  12:06:04  ers]
 * 
 * Revision 1.8  90/07/17  11:18:17  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:51:15  seiden]
 * 
 * Revision 1.7  90/07/05  23:07:27  devrcs
 * 	Fix acctwatch use of VFS_STATFS information.
 * 	[90/07/01  22:19:09  nags]
 * 
 * Revision 1.6  90/06/22  20:05:28  devrcs
 * 	nags merge
 * 
 * 	Condensed history (reverse chronology):
 * 	Use mount's stat info in acctwatch.		nags@encore.com
 * 	Nags merge.					nags@encore.com
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Secureware: least privilege, MAC, DAC, auditing	seiden@osf.org
 * 	Added support for system V accounting		ers@osf.org
 * 	Added controlling tty info			ers@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	BSD4.4 changes.					tu@osf.org
 * 	Merged Mach 2.5 with Encore parallelization	alan@encore.com
 * 	MMAX_MP: Added lock around system time.		boykin@encore.com
 * 	MMAX_MP:  lock accounting against races.	alan@encore.com
 * 	[90/06/12  14:11:56  nags]
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
 * Copyright (c) 1982, 1986, 1989 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)kern_acct.c	7.6 (Berkeley) 9/3/89
 */
#include <sys/unix_defs.h>
#include <sys/secdefines.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/time.h>
#include <sys/tty.h>
#include <sys/proc.h>
#include <sys/vproc.h>
#include <sys/user.h>
#include <sys/vnode.h>
#include <sys/mount.h>
#include <sys/kernel.h>
#include <sys/file.h>
#include <sys/acct.h>
#include <sys/uio.h>
#include <sys/syslog.h>
#ifdef  OSF1_SERVER
#include <sys/synch.h>
#endif
#include <kern/parallel.h>
#if     SEC_BASE
#include <sys/security.h>
#endif  
#include <uxkern/bsd_types.h>
#include <mach/mig_errors.h>


/*
 * Values associated with enabling and disabling accounting
 */
int	acctsuspend = 2;	/* stop accounting when < 2% free space left */
int	acctresume = 4;		/* resume when free space risen to > 4% */
struct	timeval chk = { 15, 0 };/* frequency to check space for accounting */

/*
 * SHOULD REPLACE THIS WITH A DRIVER THAT CAN BE READ TO SIMPLIFY.
 */
struct vnode	*acctp;
int		acct_suspended = 0;

/*
 * The accounting variables are guarded with a lock.
 *
 * However, the acctwatch routine itself uses too many
 * filesystem locks to be
 * permitted to run at interrupt level.  (Vnode locks,
 * mount locks, possibly others through VFS_STATFS.)
 *
 * We will pay the price of checking accounting every so
 * often even when accounting hasn't been enabled.
 */
udecl_simple_lock_data(,accounting_lock)
#define	ACCT_LOCK()		usimple_lock(&accounting_lock)
#define	ACCT_UNLOCK()		usimple_unlock(&accounting_lock)
udecl_simple_lock_data(,pm_accounting_lock)
#define	PM_ACCT_LOCK()		usimple_lock(&pm_accounting_lock)
#define	PM_ACCT_UNLOCK()	usimple_unlock(&pm_accounting_lock)

extern mach_port_t	ux_server_port_set;
extern mach_port_t	root_fs_port;
extern mach_port_t	root_ns_port;
mach_port_t	acct_server_port;
mach_port_t	racct_port = MACH_PORT_NULL;
/*
 * Perform process accounting functions.
 */
/* ARGSUSED */
sysacct(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct vnode *vp;
	register struct args {
		char	*fname;
	} *uap = (struct args *)args;
	register struct nameidata *ndp = &u.u_nd;
	int error;
#if	!UNIX_LOCKS
	extern int acctwatch();
#endif
	struct vnode *oacctp;
	enum vtype type;
#ifdef PFS
	u_long	iomode;
#endif
	kern_return_t kr;
	mach_port_t nacct_port = MACH_PORT_NULL;
	mach_port_t tacct_port = MACH_PORT_NULL;

#if     SEC_BASE
	if (!privileged(SEC_ACCT, EPERM))
		return(EPERM);
#else
	if (error = suser(u.u_cred, &u.u_acflag))
		return(error);
#endif
	/*
	 * We are turning accounting off, destroy port.
	 */
	if (uap->fname == NULL) {
		/* Get send right */
		kr = netname_look_up(root_ns_port, "ACCTSERVER",
			"ACCTSERVER", &tacct_port);
		if (kr != KERN_SUCCESS) {
			return(0);
		}
		/* Attempt to get recieve right */
		kr = ubsd_acct_off(tacct_port, &nacct_port);
		if (nacct_port != MACH_PORT_NULL) {
			kr = netname_check_out(root_ns_port, "ACCTSERVER", 
						nacct_port, 
						MACH_MSG_TYPE_MAKE_SEND);
			kr = mach_port_destroy(mach_task_self(), 
					nacct_port);
			if (kr != KERN_SUCCESS) {
				panic("sysacct off: %s ret=0x%x", 
					"Unable to destroy port:", kr);
			}
		}
		return(0);
	} else {
#ifdef	OSF1_ADFS
		ndp->ni_segflg = UIO_SYSSPACE;
#else
		ndp->ni_segflg = UIO_USERSPACE;
#endif
		ndp->ni_dirp = uap->fname;
#ifdef  PFS
		if (error = vn_open(ndp, FWRITE, 0644, VIO_NONE))
#else
		if (error = vn_open(ndp, FWRITE, 0644))
#endif
			return (error == EISDIR ? EACCES : error);
	
		vp = ndp->ni_vp;
		BM(VN_LOCK(vp));
		type = vp->v_type;
#ifdef PFS
		iomode = vp->v_iomode;
#endif
		BM(VN_UNLOCK(vp));
		if (type != VREG) {
			vrele(vp);
			return (EACCES);
		}
		/* 
		 * We are setting up an accounting file.
		 * Do we need to set up an accounting port, if so make it so!
		 */
		while (TRUE) {
		    mach_port_allocate(mach_task_self(), 
					MACH_PORT_RIGHT_RECEIVE,
					&nacct_port);
		    if ((kr = netname_check_in_poly(root_ns_port, "ACCTSERVER", 
					nacct_port, MACH_MSG_TYPE_MAKE_SEND,
					nacct_port, MACH_MSG_TYPE_MAKE_SEND))
						!= KERN_SUCCESS) {
			kr = mach_port_destroy(mach_task_self(), nacct_port);
			if (kr != KERN_SUCCESS) {
				panic("sysacct on: %s ret=0x%x",
					"Unable to destroy port:", kr);
			}
			kr = netname_look_up(root_ns_port, "ACCTSERVER",
				"ACCTSERVER", &tacct_port);
			if (kr == KERN_SUCCESS) {
				kr = ubsd_acct_off(tacct_port, &nacct_port);
        			if (kr == KERN_SUCCESS) {
					/* Got port from remote site */
					break;
				} else {
				    kr = mach_port_mod_refs(
						mach_task_self(), 
						tacct_port,
						MACH_PORT_RIGHT_SEND, -1);
        			    if (kr != KERN_SUCCESS) {
					panic("sysacct on: %s %s ret=0x%x",
						"Unable to drop ref on remote",
						"accounting server port:", kr);
				    }
				}
			}
		    } else {
		        break;
		    }
		}
	}

#ifdef PFS
        /*
         * Server initiated I/O is not currently supported
         * on PFS files.  This check detects a PFS file type
         * and returns EFSNOTSUPP
         */
        if (iomode == VIO_PFS) {
                vrele(vp);
                return (EFSNOTSUPP);
        }
#endif


	ACCT_LOCK();
	oacctp = acctp;
	acctp = vp;
	acct_suspended = 0;
	acct_server_port = nacct_port;
	kr = mach_port_move_member(mach_task_self(), nacct_port,
				ux_server_port_set);
	if (kr != KERN_SUCCESS)
		panic("sysacct on: %s ret=0x%x",
			"Unable to insert accounting port in port set",kr);
	/* Inform process manager accounting is on */
	kr = ubsd_acct_on(root_fs_port, nacct_port);
	if (kr != KERN_SUCCESS)
		panic("sysacct: %s ret=0x%x",
			"Unable to inform PM of accounting port",kr);
	ACCT_UNLOCK();
#if	UNIX_LOCKS
	acctwatch(); 
#else
	acctwatch(&chk);
#endif
	if (oacctp) {
		vrele(oacctp);
	}
	return (0);
}

/*
 * Periodically check the file system to see if accounting
 * should be turned on or off.
 *
 * UNIX_LOCKS:  acctwatch expects to be called in thread context.
 */
#if	UNIX_LOCKS
acctwatch()
#else
acctwatch(resettime)
struct timeval *resettime;
#endif
{
	struct vnode	*vp;
	struct mount	*mp;
	int		suspended, error, avail;
	long		blocks;
#if	!UNIX_LOCKS
	struct timeval	atv;
	int		s;
#endif

	ACCT_LOCK();
	vp = acctp;
	if (vp == NULL) {
		ACCT_UNLOCK();
		return;
	}
	VREF(vp);
	suspended = acct_suspended;
	ACCT_UNLOCK();
	mp = vp->v_mount;
	if (suspended) {
		VFS_STATFS(mp, error);
		BM(MOUNT_LOCK(mp));
		avail = mp->m_stat.f_bavail;
		blocks = mp->m_stat.f_blocks;
		BM(MOUNT_UNLOCK(mp));
		if (avail > acctresume * blocks / 100) {
			ACCT_LOCK();
			if (vp == acctp)
				acct_suspended = 0;
			else
				suspended = 0;
			ACCT_UNLOCK();
			if (suspended)
				log(LOG_NOTICE, "Accounting resumed\n");
		}
	} else {
		VFS_STATFS(mp, error);
		BM(MOUNT_LOCK(mp));
		avail = mp->m_stat.f_bavail;
		blocks = mp->m_stat.f_blocks;
		BM(MOUNT_UNLOCK(mp));
		if (avail <= acctsuspend * blocks / 100) {
			ACCT_LOCK();
			if (vp == acctp)
				suspended = acct_suspended = 1;
			ACCT_UNLOCK();
			if (suspended)
				log(LOG_NOTICE, "Accounting suspended\n");
		}
	}
#if	!UNIX_LOCKS
	if (vp == acctp) {
		atv = *resettime;
		s = splhigh();
		TIME_READ_LOCK();
		timevaladd(&atv, &time);
		TIME_READ_UNLOCK();
		splx(s);
		timeout(acctwatch, (caddr_t)resettime, hzto(&atv));
	}
#endif
	vrele(vp);
}


#if	UNIX_LOCKS
acctwatch_thread()
{
#ifndef	OSF1_SERVER
	thread_t	thread;
#endif	/* OSF1_SERVER */
	struct timeval	atv;
	int		s;

#ifdef	OSF1_SERVER
	cthread_set_name(cthread_self(), "acctwatch");

	/*
	 * Make this thread slightly higher priority than default.
	 */
	set_thread_priority(mach_thread_self(), 10);
#else	/* OSF1_SERVER */
	thread = current_thread();
	thread->priority = thread->sched_pri = 10;
	thread_swappable(thread, FALSE);
#endif	/* OSF1_SERVER */

	for (;;) {
		assert_wait((int) acctwatch, FALSE);
		atv = chk;
		s = splhigh();
		TIME_READ_LOCK();
		timevaladd(&atv, &time);
		TIME_READ_UNLOCK();
		splx(s);
		thread_set_timeout(hzto(&atv));
		thread_block();
		acctwatch();
	}
}
#endif

/*
 * On exit, write a record on the accounting file.
 * MACH:  assume only one thread in task is active, so no
 * locking required for task state.
 */
acct(status)
	int status;
{
	register struct rusage *ru;
	struct timeval t;
	int i, s, error;
	acct_t acctbuf;
	register struct acct *ap = (struct acct *)&acctbuf;
	register struct proc *p;

	PM_ACCT_LOCK();
	if (racct_port == MACH_PORT_NULL) {
		PM_ACCT_UNLOCK();
		return(0);
	}
	PM_ACCT_UNLOCK();
	bcopy(u.u_comm, ap->ac_comm, sizeof(ap->ac_comm));
	ru = &u.u_ru;
	ap->ac_utime = compress(ru->ru_utime.tv_sec, ru->ru_utime.tv_usec);
	ap->ac_stime = compress(ru->ru_stime.tv_sec, ru->ru_stime.tv_usec);
	s = splhigh();
	TIME_READ_LOCK();
	t = time;
	TIME_READ_UNLOCK();
	splx(s);
	timevalsub(&t, &u.u_start);
	ap->ac_etime = compress(t.tv_sec, t.tv_usec);
	ap->ac_btime = u.u_start.tv_sec;
	p = u.u_procp;
	PROC_LOCK(p);
	ap->ac_uid = p->p_ruid;
	ap->ac_gid = p->p_rgid;
	PROC_UNLOCK(p);
	t = ru->ru_stime;
	timevaladd(&t, &ru->ru_utime);
	if (i = t.tv_sec * hz + t.tv_usec / tick)
		ap->ac_mem = (ru->ru_ixrss+ru->ru_idrss+ru->ru_isrss) / i;
	else
		ap->ac_mem = 0;
	ap->ac_rw = compress(ru->ru_inblock + ru->ru_oublock, (long)0);
	ap->ac_io = compress(u.u_ioch, (long)0);
	unix_master();		/* XXX parallelize sessions, please XXX */
	error = VPOP_CTTY_GETATTR(p->p_vproc,0,0,(dev_t*)&ap->ac_tty,0,0);
	unix_release();
	if (error) {
		if (error == ENOTTY) {
			ap->ac_tty = NODEV;
		} else {
			return(error);
		}
	}
	ap->ac_flag = (char) u.u_acflag.fi_flag; /* XXX */
	ap->ac_stat = status;
	/* Do the remote call to write the data */
	PM_ACCT_LOCK();
	error = ubsd_acct_write(racct_port, acctbuf);
	if (error) {
		/* 
		 * If PM and account server are co-resident it is possible
		 * that the call below will fail but that's OK
		 */
		mach_port_mod_refs(mach_task_self(), racct_port,
					MACH_PORT_RIGHT_SEND, -1);
		racct_port = MACH_PORT_NULL;
	}
	PM_ACCT_UNLOCK();
	return (error);
}

/*
 * Produce a pseudo-floating point representation
 * with 3 bits base-8 exponent, 13 bits fraction.
 */
compress(t, ut)
	register long t;
	long ut;
{
	register exp = 0, round = 0;

	t = t * AHZ;  /* compiler will convert only this format to a shift */
	if (ut)
		t += ut / (1000000 / AHZ);
	while (t >= 8192) {
		exp++;
		round = t&04;
		t >>= 3;
	}
	if (round) {
		t++;
		if (t >= 8192) {
			t >>= 3;
			exp++;
		}
	}
	return ((exp<<13) + t);
}

kern_return_t
sbsd_acct_off(acctsp, newacctsp)
mach_port_t	acctsp;
mach_port_t	*newacctsp;
{
	kern_return_t	kr;
	struct vnode 	*vp;

	ACCT_LOCK();
	vp = acctp;
	/*
	 * Someone raced ahead of us and got the accounting port
	 */
	if (vp == NULL) {
		ACCT_UNLOCK();
		return(-1);
	}
	kr = mach_port_move_member(mach_task_self(), acct_server_port, 
					MACH_PORT_NULL);
	if (kr != KERN_SUCCESS) {
		panic("acct_off: %s ret=0x%x",
			"Unable to remove port from set", kr);
	}
	/* 
	 * No need to touch refrences, the recieve right moves to
	 * the caller and there should be no send rights
	 */
	*newacctsp = acct_server_port;
	acct_suspended = 0;
	acctp = NULL;
	acct_server_port = MACH_PORT_NULL;
	ACCT_UNLOCK();
#if	!UNIX_LOCKS
	untimeout(acctwatch, (caddr_t)&chk);
#endif
	vrele(vp);
	return(KERN_SUCCESS);
}

kern_return_t
sbsd_acct_write(acctsp, acctbuf)
mach_port_t	acctsp;
acct_t	        acctbuf;
{
	int	error;
	struct	proc	*p;
	register struct vnode *vp;

	p = &proc[0];
	crhold(p->p_rcred);
	u.u_cred = p->p_rcred;

	ACCT_LOCK();
	if (((vp = acctp) == NULL) || acct_suspended) {
		ACCT_UNLOCK();
		crfree(p->p_rcred);
		return(KERN_SUCCESS);
	}
	VREF(vp);
	ACCT_UNLOCK();
	error = vn_rdwr(UIO_WRITE, vp, (caddr_t)&acctbuf, sizeof (struct acct),
		(off_t)0, UIO_SYSSPACE, IO_UNIT|IO_APPEND, u.u_cred, (int *)0);
	vrele(vp);
	crfree(p->p_rcred);
	if (error) {
		return(error);
	} else {
		return(KERN_SUCCESS);
	}
}
kern_return_t
sbsd_acct_on(rsp, acctsp)
mach_port_t	rsp;
mach_port_t	acctsp;
{
	PM_ACCT_LOCK();
	if (racct_port != MACH_PORT_NULL) {
		/* 
		 * If PM and account server are co-resident it is possible
		 * that the call below will fail but that's OK
		 */
		mach_port_mod_refs(mach_task_self(), racct_port,
				MACH_PORT_RIGHT_SEND, -1);
	}
	racct_port = acctsp;
	PM_ACCT_UNLOCK();
	return(KERN_SUCCESS);
}
