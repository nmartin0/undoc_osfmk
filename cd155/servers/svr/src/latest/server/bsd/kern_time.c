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
 * $Log: kern_time.c,v $
 * Revision 1.9  1995/02/01  21:28:02  bolsen
 *  Reviewer(s): Jerry Toman
 *  Risk: Medium (lots of files)
 *  Module(s): Too many to list
 *  Configurations built: STD, LITE, & RAMDISK
 *
 *  Added or Updated the Locus Copyright message.
 *
 * Revision 1.8  1994/11/18  20:27:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1994/01/26  12:42:05  paul
 *  Reviewer: none (cfg for changes to 1.2 tree)
 *  Risk: low
 *  Benefit or PTS #: 7698
 *  Testing:
 *  Module(s):
 * 	uxkern/misc.c
 * 	uxkern/rpm_clock.c
 * 	bsd/kern_time.c
 * 	bsd/kern_utctime.c
 * 	afs/afs_osi.c
 *
 *
 * Merge of bug fixes for RPM from 1.2 tree - Moved the actual sending of the
 * RPM offset outside of a TIME lock.
 *
 * Revision 1.6  1994/01/13  17:53:07  jlitvin
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
 * Revision 1.5  1993/12/03  20:24:52  paul
 * Various fixes to RPM support, plus support for global setting of Timezone
 *
 * Added the call to settimezone from settimeofday system call.
 *
 *  Reviewer: John Litvin (jlitvin@ssd.intel.com) Brent Olsen (bolsen@locus.com)
 *  Risk: Moderate
 *  Benefit or PTS #: :Fixes bug #s 3503 5303 6029 7299
 *  Testing: functionality checked on olympus.sd.locus.com w/RPM support
 *  Module(s): server/bsd/kern_time.c
 *
 * Revision 1.4.2.2  1994/01/18  20:27:58  paul
 *  Reviewer: cfj
 *  Risk: M
 *  Benefit or PTS #:7698
 *  Testing:
 *  Module(s):
 *       uxkern/misc.c
 *       uxkern/rpm_clock.c
 *       bsd/kern_time.c
 *       afs/afs_osi.c
 *
 * Changes to fix bug 7698 - Time Lock hang
 *    Moved the RPM's offsett transmittal RPC outside of the TIME lock to
 *    prevent deadlocks.
 *
 * kern_time.c:
 *         Added calls to rpm_transmit_offset in setthetime() and adjtime()
 *
 * Revision 1.4.2.1  1993/12/07  16:27:12  cfj
 * The final timezone fix from Locus.  This is just a merge from
 * the main-stem into the R1_2 branch.
 *
 *  Reviewer:jlitvin,cfj
 *  Risk:M
 *  Benefit or PTS #:
 *  Testing:
 *
 * Revision 1.4  1993/10/29  11:54:37  paul
 * Add support for setting and using the RPM distributed time-of-day clock.
 *
 * Revision 1.3  1993/07/14  17:48:04  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:47:48  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:16:02  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:21  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:07:15  cfj
 * Bump major revision number.
 *
 * Revision 2.4  1992/03/09  14:20:27  durriya
 * 	Revision 3.8  92/02/26  13:21:22  sp
 * 	Upgrade to 1.0.4
 *
 * 	Revision 1.10.12.2  1992/01/23  22:45:57  dwm
 * 		Fix nasty microtime side effect, add raw_microtime. Bug 4317.
 * 		[1992/01/23  22:00:38  dwm]
 *
 * 	Revision 1.10.10.2  1991/06/26  07:32:25  dwm
 * 		Fix #2511, getrusage child, tv_usec > 10**6.
 * 		[91/06/26  07:31:17  dwm]
 *
 * Revision 2.3  91/12/17  13:24:22  roy
 * 	91/11/26  15:32:40  sp
 * 	Upgrade to 1.0.3
 * 
 * 	91/10/23  16:37:48  condict
 * 	Undo most of the changes made for the server, now that the time var 
 * 	works correctly.
 * 
 * 	91/10/17  15:44:35  condict
 * 	Remove OSF1_SERVER ifdefs and various changes to convert the source back
 * 	towards the original 1.0.2 version.  Only minor differences left.
 * 
 * Revision 2.2  91/08/31  13:22:16  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.3  91/08/28  14:00:02  condict
 * Update to OSF 1.0.2.
 * 
 * Revision 1.10.10.2  91/06/26  07:32:25  dwm
 *	Fix #2511, getrusage child, tv_usec > 10**6.
 *	[91/06/26  07:31:17  dwm]
 *
 * Revision 1.10.6.2  91/03/19  08:57:29  bilbo
 * 	Changed nano_timer code to use undocumented ITIMER_REAL_COE/0xDEADBEEF 
 *	hack so that the underlying ITIMER_REAL gets cleared upon successful 
 *	exec.  Bug#0732.
 * 	[91/03/19  08:44:24  bilbo]
 * 
 * Revision 1.10  90/10/07  13:18:02  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  08:57:13  gm]
 * 
 * Revision 1.9  90/08/24  11:17:19  devrcs
 * 	Changes for new system call interface.
 * 	[90/08/17  17:38:00  nags]
 * 
 * Revision 1.8  90/08/09  13:14:09  devrcs
 * 	Revision DECdts modifications 90/06/25  14:14:31  Joe Comuzzi/DEC
 * 	Added external inaccinfinite which indicated current
 * 	inaccuracy is infinite (completely unknown). Changed existing
 * 	time interfaces to set this flag. All modifications can be
 * 	found by searching for 'inaccinfinite'.
 * 	[90/07/31  16:34:20  brezak]
 * 
 * Revision 1.7  90/07/17  11:19:20  devrcs
 * 	Make the calls to privileged() under SEC_BASE, not SEC_PRIV.
 * 	[90/07/10  21:52:12  seiden]
 * 
 * Revision 1.6  90/06/22  20:06:26  devrcs
 * 	nags merge
 * 	[90/06/12  19:06:48  gmf]
 * 
 * 	Changes from SecureWare for least privilege, MAC, DAC, auditing, etc.
 * 	[90/06/09  18:40:15  seiden]
 * 
 * 	Compressed ancient history (reverse chronology):
 * 	Parallelized for OSF/1.				nags@encore.com
 * 	Removed include of <sys/dir.h>.			gmf@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Change to use new suser() interface.		ers@osf.org
 * 	Fix bogus locking by adding curly braces.	sue,boykin@encore.com
 * 	Redundant call to unix_master form hardclock.	alan,boykin@encore.com
 * 	Merge early Mach 2.5, Encore parallelization.	alan@encore.com
 * 	Use u and proc timer locks in {get,set}itimer.	alan@encore.com
 * 	MMAX_MP:  Added locks around time constants.	boykin@encore.com
 * 	Update mach_tz with tz; Mach time device.	jsb@cmu.edu
 * 	Don't resettodr from adjtime for Pmax.		reinhold@mit.edu
 * 	Changes for cleanup.				gm0w@cmu.edu
 * 	Code cleanup cataclysm.				mwyoung@cmu.edu
 * 	Corrected include file references.		mwyoung@cmu.edu
 * 	Improve adjtime() on Sun3.			white@cmu.edu
 * 	[90/06/10  01:55:44  seiden]
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
 *	@(#)kern_time.c	7.4 (Berkeley) 3/23/87
 */

#include <cputypes.h>
#include <sys/secdefines.h>

#include <sys/param.h>
#include <sys/user.h>
#include <sys/kernel.h>
#include <sys/proc.h>

#ifdef	OSF1_SERVER
#include <sys/synch.h>		/* for spl definitions */
#endif

#include <machine/reg.h>
#include <machine/cpu.h>
#include <sys/time.h>
#if	SEC_BASE
#include <sys/security.h>
#endif
/* 
 * Time of day and interval timer support.
 *
 * These routines provide the kernel entry points to get and set
 * the time-of-day and per-process interval timers.  Subroutines
 * here provide support for adding and subtracting timeval structures
 * and decrementing interval timers, optionally reloading the interval
 * timers when they expire.
 */
extern int inaccinfinite;	/* Flag indicating inaccuarcy is infinite */

gettimeofday(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct	timeval *tp;
		struct	timezone *tzp;
	} *uap = (struct args *) args;
	struct timeval atv;
	struct timezone atz;
	int	error, s;

#ifdef	balance
	/*
	 * Return number of users to programs that limit # of users.
	 * (value in R1 on return from gettimeofday()).
	 *
	 * DYNIX login uses this to tell if too many users logged in
	 * for a binary license.
	 */
	{
		extern unsigned sec0eaddr;
		retval[1] = sec0eaddr;
	}
#endif
	if (uap->tp) {
		raw_microtime(&atv);
		if (error = copyout((caddr_t)&atv, (caddr_t)uap->tp,
			sizeof (atv)))
			return (error);
	}
	s = splhigh();
	TIME_READ_LOCK();
	atz = tz;
	TIME_READ_UNLOCK();
	splx(s);
	if (uap->tzp)
		return (copyout((caddr_t)&atz, (caddr_t)uap->tzp,
			sizeof(struct timezone)));
	return (0);
}

settimeofday(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct	timeval *tv;
		struct	timezone *tzp;
	} *uap = (struct args *) args;
	struct timeval atv;
	struct timezone atz;
#ifndef	OSF1_SERVER
	extern struct timezone *mach_tz;
#endif
#ifndef TNC
	register int s;
#endif
	int error = 0;

	if (uap->tv) {
		if (error = copyin((caddr_t)uap->tv, (caddr_t)&atv,
			sizeof (struct timeval)))
			return (error);
#if	!AT386
		error = setthetime(&atv);
#endif
	}
	if (!error && uap->tzp) {
#if     SEC_BASE
		if (!privileged(SEC_SYSATTR, EPERM))
			return (EPERM);
#else
		if ((error = suser(u.u_cred, &u.u_acflag)))
			return (error);
#endif
		error = copyin((caddr_t)uap->tzp, (caddr_t)&atz,
			sizeof (atz));
		if (error == 0) {
#ifdef TNC
			settimezone(&atz);
#else /* !TNC */
			s = splhigh();
			TIME_WRITE_LOCK();
			tz = atz;

#ifndef	OSF1_SERVER
			if (mach_tz) *mach_tz = tz;
#endif
			TIME_WRITE_UNLOCK();
			splx(s);
#endif /* TNC */
		}
	}
#if	AT386
	/* Since rtc maintains local time, tz must be set first. */
	if (!error && uap->tv)
		return (setthetime(&atv));
#endif
	return (error);
}

setthetime(tv)
	struct timeval *tv;
{
	int error, s;

#if     SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else  
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif
/* WHAT DO WE DO ABOUT PENDING REAL-TIME TIMEOUTS??? */
	s = splhigh();
	TIME_WRITE_LOCK();
	inaccinfinite = 1;		/* Inaccuracy is infinite */
	boottime.tv_sec += tv->tv_sec - time.tv_sec;
	time = *tv;
#ifdef	OSF1_SERVER
	resettodr();
#endif
	TIME_WRITE_UNLOCK();
	splx(s);
#ifndef	OSF1_SERVER
	resettodr();
#endif
#if defined(__i860__) && defined(TNC)
	rpm_transmit_offset();
#endif /* TNC && i860 */

	return (0);
}

extern	int tickadj;			/* "standard" clock skew, us./tick */
int	tickdelta;			/* current clock skew, us. per tick */
long	timedelta;			/* unapplied time correction, us. */
long	bigadj = 1000000;		/* use 10x skew above bigadj us. */
#ifndef	multimax
int	doresettodr = 0;
#endif

adjtime(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		struct timeval *delta;
		struct timeval *olddelta;
	} *uap = (struct args *) args;
	struct timeval atv, oatv;
	register long ndelta;
	int error, s;

#if     SEC_BASE
	if (!privileged(SEC_SYSATTR, EPERM))
		return (EPERM);
#else  
	if (error = suser(u.u_cred, &u.u_acflag))
		return (error);
#endif  
	if (error = copyin((caddr_t)uap->delta, (caddr_t)&atv,
		sizeof (struct timeval)))
		return (error);
	ndelta = atv.tv_sec * 1000000 + atv.tv_usec;

	s = splhigh();
	TIME_WRITE_LOCK();
	inaccinfinite = 1;		/* Inaccuracy is infinite */

#if defined(__i860__) && defined(TNC)
	if(check_for_rpm()){
		rpm_adjust_time(ndelta);
		TIME_WRITE_UNLOCK();
		splx(s);
		if (uap->olddelta){
			oatv.tv_sec = 0;
			oatv.tv_usec = 0;
			(void) copyout((caddr_t)&oatv, (caddr_t)uap->olddelta,
			sizeof (struct timeval));
		}
		return(0);
	}
#endif /* defined(__i860__) && defined(TNC) */

	if (timedelta == 0)
		if (ndelta > bigadj)
			tickdelta = 10 * tickadj;
		else
			tickdelta = tickadj;
	if (ndelta % tickdelta)
		ndelta = ndelta / tickdelta * tickdelta;

	if (uap->olddelta) {
		oatv.tv_sec = timedelta / 1000000;
		oatv.tv_usec = timedelta % 1000000;
	}

#if	defined(PMAX) || defined(multimax)
#else
	if (ndelta)
		doresettodr = 1;
	else if (timedelta && doresettodr) {
		doresettodr = 0;
		resettodr();
	}
#endif
	timedelta = ndelta;
	TIME_WRITE_UNLOCK();
	splx(s);

#if defined(__i860__) && defined(TNC)
	rpm_transmit_offset();
#endif /* TNC && i860 */

#ifdef	multimax
	resettodr();
#endif

	if (uap->olddelta)
		(void) copyout((caddr_t)&oatv, (caddr_t)uap->olddelta,
			sizeof (struct timeval));
	return (0);
}

/*
 * Get value of an interval timer.  The process virtual and
 * profiling virtual time timers are kept in the u. area, since
 * they can be swapped out.  These are kept internally in the
 * way they are specified externally: in time until they expire.
 *
 * The real time interval timer is kept in the process table slot
 * for the process, and its value (it_value) is kept as an
 * absolute time rather than as a delta, so that it is easy to keep
 * periodic real-time signals from drifting.
 *
 * Virtual time timers are processed in the hardclock() routine of
 * kern_clock.c.  The real time timer is processed by a timeout
 * routine, called from the softclock() routine.  Since a callout
 * may be delayed in real time due to interrupt processing in the system,
 * it is possible for the real time timeout routine (realitexpire, given below)
 * to be delayed in real time past when it is supposed to occur.  It
 * does not suffice, therefore, to reload the real timer .it_value from the
 * real time timers .it_interval.  Rather, we compute the next time in
 * absolute time the timer should go off.
 */
getitimer(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		u_int	which;
		struct	itimerval *itv;
	} *uap = (struct args *) args;
	struct itimerval aitv;
	int s;

	if (uap->which > 2)
		return (EINVAL);
	s = splhigh();
	if (uap->which == ITIMER_REAL) {
		/*
		 * Convert from absoulte to relative time in .it_value
		 * part of real time timer.  If time for real time timer
		 * has passed return 0, else return difference between
		 * current time and time for the timer to go off.
		 */
		PROC_TIMER_LOCK(u.u_procp);
		aitv = u.u_procp->p_realtimer;
		PROC_TIMER_UNLOCK(u.u_procp);
		if (timerisset(&aitv.it_value)) {
			TIME_READ_LOCK();
			if (timercmp(&aitv.it_value, &time, <))
				timerclear(&aitv.it_value);
			else
				timevalsub(&aitv.it_value, &time);
			TIME_READ_UNLOCK();
		}
	} else {
		U_TIMER_LOCK();
		aitv = u.u_timer[uap->which];
		U_TIMER_UNLOCK();
	}
	splx(s);
	return (copyout((caddr_t)&aitv, (caddr_t)uap->itv,
	    sizeof (struct itimerval)));
}


/*
 * ITIMER_REAL_COE HACK XXXXX	Undocumented extension.
 * This routines is called to clear an ITIMER_REAL upon exec.  Used
 * only by the user-space nano_timer implementation.
 * *** Remove this when nano_timers migrate into the kernel ***
 */
#define ITIMER_REAL_COE 0xDEADBEEF

int clear_p_realtimer(struct proc *p)
{
	struct itimerval aitv; 

	(void)untimeout(realitexpire, (caddr_t)p);
	timerclear(&aitv.it_interval);
	timerclear(&aitv.it_value);
	PROC_TIMER_LOCK(p);
	p->p_realtimer = aitv;
	p->p_realtimer_coe = 0;
	PROC_TIMER_UNLOCK(p);
	return(0);
}


setitimer(p, args, retval)
	struct proc *p;
	void *args;
	int *retval;
{
	register struct args {
		u_int	which;
		struct	itimerval *itv, *oitv;
	} *uap = (struct args *) args;
	struct itimerval aitv; 
	register struct itimerval *itvp;
	int s, error;
	int is_coe_hack = 0;		/* clear p_realtimer on exec */

	/*
	 * ITIMER_REAL_COE HACK XXXXX	Undocumented extension
	 * supports nano_timer facility, which isn't inherited across exec,
	 * unlike setitimer.  HACK in common with user-space nano_timer code.
	 * *** Remove this when nano_timers migrate into the kernel ***
	 */
	if (uap->which == ITIMER_REAL_COE)
		uap->which = ITIMER_REAL, is_coe_hack = 1;
	if (uap->which > 2)
		return (EINVAL);
	itvp = uap->itv;
	if (itvp && (error = copyin((caddr_t)itvp, (caddr_t)&aitv,
	    sizeof(struct itimerval))))
		return (error);
	if ((uap->itv = uap->oitv) && (error = getitimer(p, uap, retval)))
		return (error);
	if (itvp == 0)
		return (0);
	if (itimerfix(&aitv.it_value) || itimerfix(&aitv.it_interval))
		return (EINVAL);
	s = splhigh();
	if (uap->which == ITIMER_REAL) {
		untimeout(realitexpire, (caddr_t)p);
		if (timerisset(&aitv.it_value)) {
			TIME_READ_LOCK();
			timevaladd(&aitv.it_value, &time);
			TIME_READ_UNLOCK();
			timeout(realitexpire, (caddr_t)p,hzto(&aitv.it_value));
		} else
			is_coe_hack = 0;
		PROC_TIMER_LOCK(p);
		p->p_realtimer = aitv;
		p->p_realtimer_coe = (short)is_coe_hack;
		PROC_TIMER_UNLOCK(p);
	} else {
		U_TIMER_LOCK();
		u.u_timer[uap->which] = aitv;
		U_TIMER_UNLOCK();
	}
	splx(s);
	return (0);
}

/*
 * Real interval timer expired:
 * send process whose timer expired an alarm signal.
 * If time is not set up to reload, then just return.
 * Else compute next time timer should go off which is > current time.
 * This is where delay in processing this timeout causes multiple
 * SIGALRM calls to be compressed into one.
 *
 * Assumes called on the master processor!
 */
realitexpire(p)
	register struct proc *p;
{
	struct timeval	rit_value;
	int s;

#ifndef	OSF1_SERVER
	psignal(p, SIGALRM);
#else
	psignal3(p, SIGALRM, FALSE);
#endif
	s = splhigh();
	PROC_TIMER_LOCK(p);
	if (!timerisset(&p->p_realtimer.it_interval)) {
		timerclear(&p->p_realtimer.it_value);
		p->p_realtimer_coe = 0;	/* clear "clear on exec" */
		PROC_TIMER_UNLOCK(p);
		splx(s);
		return;
	}
	for (;;) {
		timevaladd(&p->p_realtimer.it_value,
		    &p->p_realtimer.it_interval);
		TIME_READ_LOCK();
		if (timercmp(&p->p_realtimer.it_value, &time, >)) {
			TIME_READ_UNLOCK();
			rit_value = p->p_realtimer.it_value;
			PROC_TIMER_UNLOCK(p);
			timeout(realitexpire, (caddr_t)p, hzto(&rit_value));
			splx(s);
			return;
		}
		TIME_READ_UNLOCK();
	}
	/* NOTREACHED */
}

/*
 * Check that a proposed value to load into the .it_value or
 * .it_interval part of an interval timer is acceptable, and
 * fix it to have at least minimal value (i.e. if it is less
 * than the resolution of the clock, round it up.)
 */
itimerfix(tv)
	struct timeval *tv;
{

	if (tv->tv_sec < 0 || tv->tv_sec > 100000000 ||
	    tv->tv_usec < 0 || tv->tv_usec >= 1000000)
		return (EINVAL);
	if (tv->tv_sec == 0 && tv->tv_usec != 0 && tv->tv_usec < tick)
		tv->tv_usec = tick;
	return (0);
}

/*
 * Decrement an interval timer by a specified number
 * of microseconds, which must be less than a second,
 * i.e. < 1000000.  If the timer expires, then reload
 * it.  In this case, carry over (usec - old value) to
 * reducint the value reloaded into the timer so that
 * the timer does not drift.  This routine assumes
 * that it is called in a context where the timers
 * on which it is operating cannot change in value.
 */
itimerdecr(itp, usec)
	register struct itimerval *itp;
	int usec;
{

	if (itp->it_value.tv_usec < usec) {
		if (itp->it_value.tv_sec == 0) {
			/* expired, and already in next interval */
			usec -= itp->it_value.tv_usec;
			goto expire;
		}
		itp->it_value.tv_usec += 1000000;
		itp->it_value.tv_sec--;
	}
	itp->it_value.tv_usec -= usec;
	usec = 0;
	if (timerisset(&itp->it_value))
		return (1);
	/* expired, exactly at end of interval */
expire:
	if (timerisset(&itp->it_interval)) {
		itp->it_value = itp->it_interval;
		itp->it_value.tv_usec -= usec;
		if (itp->it_value.tv_usec < 0) {
			itp->it_value.tv_usec += 1000000;
			itp->it_value.tv_sec--;
		}
	} else
		itp->it_value.tv_usec = 0;		/* sec is already 0 */
	return (0);
}

/*
 * Add and subtract routines for timevals.
 * N.B.: subtract routine doesn't deal with
 * results which are before the beginning,
 * it just gets very confused in this case.
 * Caveat emptor.
 */
timevaladd(t1, t2)
	struct timeval *t1, *t2;
{

	t1->tv_sec += t2->tv_sec;
	t1->tv_usec += t2->tv_usec;
	timevalfix(t1);
}

timevalsub(t1, t2)
	struct timeval *t1, *t2;
{

	t1->tv_sec -= t2->tv_sec;
	t1->tv_usec -= t2->tv_usec;
	timevalfix(t1);
}

timevalfix(t1)
	struct timeval *t1;
{

	if (t1->tv_usec < 0) {
		t1->tv_sec--;
		t1->tv_usec += 1000000;
	}
	if (t1->tv_usec >= 1000000) {
		t1->tv_sec  += t1->tv_usec / 1000000;
		t1->tv_usec %= 1000000;
	}
}
