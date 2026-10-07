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
 * HISTORY
 * $Log: kern_synch.c,v $
 * Revision 1.4  1994/11/18  20:27:16  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/07/14  17:47:58  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  18:47:40  cfj
 * Adding new code from vendor
 *
 * Revision 1.2  1992/11/30  22:15:59  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:06:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  00:07:10  cfj
 * Bump major revision number.
 *
 * Revision 2.6  93/06/22  17:55:29  yazz
 * Extricate curly braces from ifdefs so that vi's '%' and tags can work.
 * 
 * Revision 2.5  92/05/24  14:14:28  pjg
 * 	92/04/03  14:13:25  condict
 * 	Remove unnecessary OSF1_SERVER ifndefs and uncompiled funcs.  Remove body
 * 	of rqinit, which calls obsolete sleep_queue_init (rewrite of sched_prim.c)
 * 	[92/05/19            srl]
 *
 * Revision 2.4  92/03/09  14:19:57  durriya
 * 	[ 92/03/02  11:33:42  condict ]
 * 	Fix bug in mpsleep; need to fall through the THREAD_SHOULD_TERMINATE
 * 	case, unless process is exiting.
 * 
 * 	[ 91/12/18  17:14:51  sp ]
 * 	Include sys/synch.h to get spl macros
 * 
 * Revision 2.3  92/03/01  18:42:57  pjg
 * 	Allow correct fall-through in THREAD_SHOULD_TERMINATE. (loverso)
 * 
 * Revision 2.2  91/08/31  13:22:12  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.4  91/08/13  15:04:24  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.13  90/10/31  13:49:24  devrcs
 * 	Replace all references of NZERO with PRIZERO
 * 	[90/10/25  12:47:25  sp]
 * 
 * Revision 1.12  90/10/07  13:17:57  devrcs
 * 	Turn off assertion at head of tsleep, not very meaningful and
 * 	sometimes wrong. Check signals in mpsleep even when result is
 * 	not interrupted, in case both an event and a signal received.
 * 	[90/10/03  16:16:20  tmt]
 * 
 * 	Fixed up EndLog Marker.
 * 	[90/09/30  15:51:22  gm]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  08:57:04  gm]
 * 
 * 	Eliminate extraneous dependency on <sys/buf.h>
 * 	[90/09/23  21:33:48  jeffc]
 * 
 * 	Fix up header comments broken during last submit.
 * 	[90/09/20  18:41:42  gmf]
 * 
 * Revision 1.11  90/09/23  15:43:19  devrcs
 * 	Fix setting/clearing of thread->wait_mesg in mpsleep.
 * 	[90/09/20  17:18:57  gmf]
 * 
 * 	Fix mpsleep to lower spl and unlock during panic.
 * 	[90/09/15  14:51:17  tmt]
 * 
 * 	Don't do splhigh in mpsleep.  It's unnecessary and worse,
 * 	it breaks MP synchronization with respect to locking and
 * 	unlocking locks at the proper priority level.
 * 	[90/09/13  14:03:32  gmf]
 * 
 * 	Fix operator precedence in assert in sleep.
 * 	[90/09/06  08:00:52  lwa]
 * 
 * Revision 1.10  90/09/13  11:42:10  devrcs
 * 	Change mpsleep to not assert_wait if chan == 0.
 * 	(Enables network's use of tsleep).
 * 	[90/08/28  11:45:40  tmt]
 * 
 * 	Cleanup changes.  Avoid unnecessary spl's.  Set and clear wait_mesg
 * 	properly.
 * 	[90/08/22  14:34:12  gmf]
 * 
 * Revision 1.9  90/08/24  11:17:14  devrcs
 * 	Added mpsleep call -- a MP-safe generalized sleep to be called
 * 	by tsleep, sleep, and other "educated" callers.
 * 	Added tsleep -- 4.3 reno style.
 * 	Eliminate mach_unix_sleep.
 * 	[90/08/20  06:38:53  gmf]
 * 
 * Revision 1.8  90/07/17  11:19:16  devrcs
 * 	Change arguments for tsleep to include wmesg. Set wait_mesg
 * 	when sleeping.
 * 	[90/07/06  11:02:33  brezak]
 * 
 * Revision 1.7  90/06/22  20:06:23  devrcs
 * 	nags merge
 * 
 * 	Condensed ancient history (reverse chronology):
 * 	Removed CMUCS_RP, references to u.u_modes.	noemi@osf.org
 * 	Removed include of <sys/dir.h>.			gmf@osf.org
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Fixed includes, resourcep ausing to CMUCS_RP.	noemi@osf.org
 * 	Merged Mach 2.5 with Encore parallelization	alan@encore.com
 * 	Added lock around system time.			boykin@encore.com
 * 	Added mach-compatible sleep call		alan@encore.com
 * 	Kernel monitor support; use new priorities	dlb@cmu.edu
 * 	Eliminated unused filter() macro 		rvb@cmu.edu
 * 	Unconditionalized PCATCH flag in priority	gm0w@cmu.edu
 * 	Declaration of should_exit() as boolean_t.	rvb@cmu.edu
 * 	ccpu is no longer used for anyone 		rvb@cmu.edu
 * 	Added changes for Mips:  avoid floating point	af@cmu.edu
 * 	Add wakeup_one.; add PCATCH flag for sleep	jsb@cmu.edu
 * 	Include <sys/callout.h> 			rpd@cmu.edu
 * 	Corrected include file references.		mwyoung@cmu.edu
 * 	Use kernel_vm_space in schedcpu			mwyoung@cmu.edu
 * 	Changed to include <sys/table.h> 		mja@cmu.edu
 * 	Added check for UMODE_NONICE bit in schedcpu()	dbg@cmu.edu
 * 	Cleaned up conditionals in sleep().		dlb@cmu.edu
 * 	Check for use of kernel_pmap 			mwyoung@cmu.edu
 * 	Don't activate pmaps for kernel_only tasks.	dlb@cmu.edu
 * 	Reduced conditionals				avie@cmu.edu
 * 	[90/06/12  19:06:42  gmf]
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
 *	@(#)kern_synch.c	7.1 (Berkeley) 6/5/86
 */

#ifndef	OSF1_SERVER
#include <cputypes.h>
#include <cpus.h>
#include <mach_km.h>
#include <mach_assert.h>
#endif	/* OSF1_SERVER */

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/user.h>
#ifndef	OSF1_SERVER
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/vm.h>
#endif	/* OSF1_SERVER */
#include <sys/kernel.h>
#ifndef	OSF1_SERVER
#include <sys/table.h>

#include <kern/ast.h>
#include <sys/callout.h>
#else	/* OSF1_SERVER */
#include <sys/synch.h>
#include <uxkern/import_mach.h>
#include <sys/signal_macros.h>
#endif	/* OSF1_SERVER */
#include <kern/queue.h>
#ifndef	OSF1_SERVER
#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/sched.h>
#endif	/* OSF1_SERVER */
#include <kern/sched_prim.h>
#ifndef	OSF1_SERVER
#include <mach/machine.h>
#endif	/* OSF1_SERVER */
#include <kern/parallel.h>
#ifndef	OSF1_SERVER
#include <kern/processor.h>

#if     MACH_KM
#include <kern/kern_mon.h>
#endif 

#include <machine/cpu.h>
#include <vm/pmap.h>
#include <vm/vm_kern.h>

#ifdef	vax
#include <vax/mtpr.h>	/* XXX */
#endif

#include <kern/task.h>
#include <mach/time_value.h>
#endif	/* OSF1_SERVER */

#ifndef	OSF1_SERVER
/*
 * Recompute process priorities, once a second
 */
schedcpu()
{
	register thread_t th;
	uid_t uid;

	wakeup((caddr_t)&lbolt);

	/*
	 *	Autonice code moved here from kern_clock.c
	 */
	th = current_thread();
	if (!(th->task->kernel_vm_space)) {
	    register struct proc *p;

	    p = u.u_procp;
	    /*
	     * Important:  uid must be found through the per-thread
	     * credentials; using the proc structure's credentials
	     * would require taking the proc lock, not a good idea
	     * at interrupt level.
	     *
	     * Note sleazoid use of p_nice!		XXX
	     */
	    uid = u.u_uid;
	    if (uid && p->p_nice == PRIZERO) {
		time_value_t	user_time;

		timer_read(&(th->user_timer), &user_time);
		if (user_time.seconds > 10 * 60) {
		    p->p_nice = PRIZERO+4;
		    thread_priority(th, BASEPRI_USER + p->p_nice/2, TRUE);
		}
	    }
	}
	timeout(schedcpu, (caddr_t)0, hz);
}
#endif	/* OSF1_SERVER */



/*
 * Internal MP sleep call.
 *
 * Suspends current thread until a wakeup is made on chan.
 * The process will then be made runnable.
 *
 * The thread sleeps at most timo/hz seconds (0 means no timeout).
 *
 * If pri includes PCATCH flag, signals are checked
 * before and after sleeping; otherwise, signals are not checked.
 *
 * Not setting PCATCH will cause the thread to sleep uninterruptibly.
 * This is different from 4.3 Reno, since pri is not used for scheduling
 * priority in OSF/1.  If PCATCH is not set, the return value will
 * always be zero.
 *
 * mpsleep returns 0 if awakened, and EWOULDBLOCK if the timeout expires.
 *
 * If PCATCH is set and a signal needs to be delivered,
 * ERESTART is returned if the current system call should be restarted
 * (if possible), and EINTR is returned if the system call should
 * be interrupted by the signal (return EINTR).
 * 
 * mpsleep also allows the caller to specify a lock to be unlocked before
 * it blocks.  This takes care of the MP problem with a normal sleep.
 * This lock can be simple, read, or write, as specified by the flags
 * parameter (MS_LOCK_SIMPLE, MS_LOCK_READ, MS_LOCK_WRITE).  The default
 * behavior is to re-lock the lock on success, and leave it unlocked on
 * errors.  If the MS_LOCK_ON_ERROR flag is set, then the lock is
 * re-locked on failure as well.
 *
 * This interface is not intended for general usage.  It is mostly for
 * use by sleep and tsleep, with some use by knowledgeable callers (e.g.
 * sosleep).
 */

mpsleep(chan, pri, wmesg, timo, lockp, flags)
	caddr_t	chan;
        int pri;
        char *wmesg;
        int timo;
	void *lockp;
	int flags;
{
#ifndef	OSF1_SERVER
	register struct proc *p = u.u_procp;
        register thread_t thread = current_thread();
#else	/* OSF1_SERVER */
	register uthread_t	thread = &u;
	register struct proc	*p = thread->u_procp;
#endif	/* OSF1_SERVER */
	int catch = pri & PCATCH;
	int error = 0;
        
	if (chan)
		assert_wait((int)chan, (catch ? TRUE : FALSE));
	/*
	 * We may need to unlock a lock
	 */
	if (lockp) {
		/*
		 * Note:  the lock package macros should deal with this
		 *	  correctly if locking is not configured.
		 */
		if (flags & MS_LOCK_SIMPLE)
			simple_unlock((simple_lock_t) lockp);
		else {
			LASSERT(flags & (MS_LOCK_READ|MS_LOCK_WRITE));
			lock_done((lock_t) lockp);
		}
	}

	if (panicstr) {
		/*
		 * After a panic, just give interrupts a chance,
		 * then just return; don't run any other procs 
		 * or panic below, in case this is the idle process
		 * and already asleep.
		 * The splnet should be spl0 if the network was being used
		 * by the filesystem, but for now avoid network interrupts
		 * that might cause another panic.
		 */
		int s = splnet();
		/* Must clear any assert_wait, here or in caller */
		clear_wait(thread, THREAD_AWAKENED, FALSE);
		splx(s);
		goto out;
	}

	if (catch) {
		/*
		 * CHECK_SIGNALS does not need to be done on
		 * unix_master.  We only do the more expensive operation
		 * of issig after a quick check.
		 */
#ifndef	OSF1_SERVER
		if (CHECK_SIGNALS(p, thread, thread->u_address.uthread))
#else	/* OSF1_SERVER */
		if (CHECK_SIGNALS(p, thread, thread))
#endif	/* OSF1_SERVER */
		{
			unix_master();
			if (ISSIG(p)) {
				clear_wait(thread, THREAD_INTERRUPTED, TRUE);
				if (u.u_sigintr & sigmask(p->p_cursig))
					error = EINTR;
				else
					error = ERESTART;
				unix_release();
				goto out;
			}
			unix_release();
		}
	}
#ifdef	OSF1_SERVER
	if (chan == (caddr_t)&lbolt && timo == 0)
		timo = hz;
#endif	/* OSF1_SERVER */
	if (timo)
		thread_set_timeout(timo);

#ifndef	OSF1_SERVER
        thread->wait_mesg = wmesg;
	u.u_ru.ru_nvcsw++;
#else
	thread->uu_wait_mesg = wmesg;
#endif	/* OSF1_SERVER */

	thread_block();

#ifdef	OSF1_SERVER
        thread->uu_wait_mesg = NULL;
	if (chan == (caddr_t)&lbolt && timo == hz &&
	    thread->uu_wait_result == THREAD_TIMED_OUT)
		thread->uu_wait_result = THREAD_AWAKENED;
	switch (thread->uu_wait_result)
#else
        thread->wait_mesg = NULL;
	switch (thread->wait_result)
#endif	/* OSF1_SERVER */
	{
		case THREAD_TIMED_OUT:
			error = EWOULDBLOCK;
			break;
		case THREAD_AWAKENED:
			/*
			 * Posix implies any signal should be delivered
			 * first, regardless of whether awakened due
			 * to receiving event.
			 */
			if (!catch)
				break;
#ifndef	OSF1_SERVER
			if (!CHECK_SIGNALS(p,thread,thread->u_address.uthread))
#else	/* OSF1_SERVER */
			if (!CHECK_SIGNALS(p,thread,thread))
#endif	/* OSF1_SERVER */
				break;
			/* else fall through */
		case THREAD_SHOULD_TERMINATE:
#ifdef	OSF1_SERVER
			if (EXITING(p)) {
				error = EINTR;
				break;
			}
			/* else fall through */
#endif	/* OSF1_SERVER */
		case THREAD_INTERRUPTED:
			/*
			 * We're pretty sure we'll have to do issig, so
			 * don't bother with CHECK_SIGNALS, and just go
			 * for unix_master.
			 */
			if (catch) {
				unix_master();
				if (ISSIG(p)) {
					if (u.u_sigintr & sigmask(p->p_cursig))
						error = EINTR;
					else
						error = ERESTART;
				}
				unix_release();
			} 
			break;
	}
out:
	/*
	 * re-lock the lock if either no error, or MS_LOCK_ON_ERROR
	 * flag was set.
	 */
	if (lockp && (!error || (flags & MS_LOCK_ON_ERROR))) {
		if (flags & MS_LOCK_SIMPLE)
			simple_lock((simple_lock_t) lockp);
		else if (flags & MS_LOCK_READ) 
			lock_read((lock_t) lockp);
		else
			lock_write((lock_t) lockp);
	}
	return (error);
}

#if	MACH_ASSERT

/*
 * tsleep and sleep are macros (see param.h) if !MACH_ASSERT.
 */
/*
 * tsleep -- 
 * General purpose sleep call.
 * An interface to the mpsleep internal call.
 * Suspends current process until a wakeup is made on chan.
 * The process will then be made runnable.
 * Sleeps at most timo/hz seconds (0 means no timeout).
 * If pri includes PCATCH flag, signals are checked
 * before and after sleeping, else signals are not checked.
 * Returns 0 if awakened, EWOULDBLOCK if the timeout expires.
 * If PCATCH is set and a signal needs to be delivered,
 * ERESTART is returned if the current system call should be restarted
 * if possible, and EINTR is returned if the system call should
 * be interrupted by the signal (return EINTR).
 */

tsleep(chan, pri, wmesg, timo)
	caddr_t chan;
	int pri;
	char *wmesg;
	int timo;
{

	/*
	 * Catch callers who should be using sleep().
	ASSERT((pri & PCATCH) != 0 || (pri & ~PCATCH) > PZERO || timo);
	 */
	return (mpsleep(chan, pri, wmesg, timo, (void *) NULL, 0));
}

/*
 * sleep -- 
 * Give up the processor till a wakeup occurs
 * on chan, at which time the process is rescheduled.
 * This function causes the process to sleep
 * uninterruptibly.
 * As a result, it must be called with pri <= PZERO.
 *
#ifdef	OSF1_SERVER
 * We make a special check for lbolt (the once-per-second timer)
 * to avoid keeping a separate lbolt thread or overloading the
 * timeout queue.
#endif
 */

sleep(chan, pri)
	caddr_t chan;
	int pri;
{

	ASSERT((pri & PCATCH) == 0 && pri <= PZERO);
#ifdef	OSF1_SERVER
	(void) mpsleep(chan, pri, (char *)NULL,
			(chan == (caddr_t)&lbolt) ? hz : 0,
			(void *) NULL, 0);
#else
	(void) mpsleep(chan, pri, (char *)NULL, 0, (void *) NULL, 0);
#endif	/* OSF1_SERVER */
}
#endif	/* MACH_ASSERT */


/*
 * Wake up all processes sleeping on chan.
 */
wakeup(chan)
	register caddr_t chan;
{
	int s;

	s = splhigh();

	thread_wakeup((int) chan);
	splx(s);
}

/*
 * Wake up the first process sleeping on chan.
 *
 * Be very sure that the first process is really
 * the right one to wakeup.
 */
wakeup_one(chan)
	register caddr_t chan;
{
	int s;

	s = splhigh();
	thread_wakeup_one((int) chan);
	splx(s);
}

/*
 * Initialize the (doubly-linked) run queues
 * to be empty.
 */
rqinit()
{
}
