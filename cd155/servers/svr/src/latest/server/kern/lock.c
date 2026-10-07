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
 * $Log: lock.c,v $
 * Revision 1.8  1994/11/18  20:32:35  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:01:32  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:32  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:16:29  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:32  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/03/29  23:02:14  nandy
 * Merged from T9 branch
 *
 * Revision 1.2.8.3  1993/03/29  22:41:10  nandy
 * Locking changes from condict.
 *
 * Revision 2.9  1993/04/29  14:00:42  klh
 * 	Revision 2.10  93/03/23  22:52:48  condict
 * 	Fix assertions and maintainence of debug fields in locks.
 *
 * 	Revision 2.9  93/03/22  23:55:55  condict
 * 		Extensive changes to the lock struct and the locking functions for
 * 		performance, correctness and readability.  See "1993 Lock Rewrite",
 * 		below.
 *
 * 	Revision 2.8  92/12/08  10:42:47  durriya
 * 		1.1 unmount sync changes - added do_lock_try_read_assert (durriya)
 *
 * Revision 2.8  93/03/22  21:10:26  yazz
 * OSF lock changes.  Almost completely rewritten by OSF.
 * 
 * Revision 1.2.8.2  1993/03/19  01:23:41  cfj
 * Fix to new locking code from OSF.
 *
 * Revision 2.7  92/05/24  14:29:21  pjg
 * 	Revision 3.5  92/03/23  18:03:19  condict
 * 	Allow NCPUS == 1 in the server, to compile optimally for a uni-processor.
 * 
 * 	Revision 3.4  92/03/13  15:18:49  condict
 * 	Set and clear u.uu_lock_sleep around each place where we sleep waiting
 * 	for a lock, to prevent Netintr from being called in thread_block (fixes
 * 	deadlock bug).
 * 
 * Revision 2.6  92/05/12  00:07:52  loverso
 * 	Add suport for tracking who acquired the lock for blocking locks.
 * 	Renamed all lock routines to do_*. If MACH_LDEBUG or MACH_LTRACKS is
 * 	defined, these routines receive the program counter of who called them.
 * 	The lock routines were redefined as macros in lock.h (pjg).
 * 
 * Revision 2.5  92/04/05  16:54:21  pjg
 * 	Don't assert the lock_owner in OSF1_ADFS because we may release the
 * 	lock with a different dummy proc.
 * 
 * Revision 2.4  92/03/09  14:36:27  durriya
 * 	[Revision 3.3  92/01/07  23:33:49  condict]
 * 	Optimize complex locks by changing null value of thread field from -1 to
 * 	0 and only comparing it to current thread when it is non-zero.  Also, add
 * 	simple_lock_solid as part of changing simple locks from mutexes to
 * 	spinlocks.
 * 
 * Revision 2.3  91/12/16  20:37:04  roy
 * 	91/10/21  18:47:32  emcmanus
 * 	Fixes to compile with asserts.
 * 
 * Revision 2.2  91/08/31  13:37:39  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/07  17:00:06  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.8  90/10/31  13:57:48  devrcs
 * 	Remove mmax-centric code from LTRACK_DONE.
 * 	[90/10/21  16:50:48  jeffc]
 * 
 * Revision 1.7  90/10/07  13:53:51  devrcs
 * 	Fix lock count code so that the kernel will build without
 * 	MACH_LDEBUG on.
 * 	[90/10/01  17:28:07  jeffc]
 * 
 * 	Added EndLog Marker.
 * 	[90/09/28  09:54:52  gm]
 * 
 * 	Fix invalid declaration of printf_lock.
 * 	[90/09/28  15:09:07  tmt]
 * 
 * 	Added lock count tracking.
 * 	[90/09/28  12:37:12  nags]
 * 
 * Revision 1.6  90/06/29  13:36:25  devrcs
 * 	Don't use current_thread() until we're ready.
 * 	[90/06/28  13:28:41  nags]
 * 
 * 	Added lock statistics code.
 * 
 * 	Compressed history (reverse chronology):
 * 	Count per-thread r/w locks and per-cpu simple locks.	nags@encore.com
 * 	Lock_done uses getfrompc in gcc (multimax only).	nags@encore.com
 * 	Nags merge.						nags@encore.com
 * 	Updated for OSF/1.					nags@encore.com
 * 	Fixes for first snapshot.				gm@osf.org
 * 	MACH X115 Update.					gm@osf.org
 * 	Lock statistics and debugging assertions.
 * 		alan, boykin, shashi, sue at encore.com.
 * 	Debugging checks for lock corruption (-1 read count).	dlb@cmu.edu
 * 	Changes for cleanup.					gm0w@cmu.edu
 * 	Use simple_lock_addr when calling thread_sleep.		rpd@cmu.edu
 * 	Changed panics to assertions for simple locks.		rpd@cmu.edu
 * 	Add simple-locking sanity-checking code.		rpd@cmu.edu
 * 	Set lock_wait_time=0 on a uniprocessor.			sanzi@cmu.edu
 * 	Eliminated previous history.				avie@cmu.edu
 * 	[90/06/26  11:13:28  gmf]
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
 *	File:	kern/lock.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Copyright (C) 1985, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Locking primitives implementation
 */

/*
 *			1993 Lock Rewrite
 *			-----------------
 * 
 * Lock data structure changes:
 * 
 * The "recursion_depth", "want_write" and "read_count" fields are
 * replaced by "lock_count".  Lock_count > 0 indicates num read locks;
 * lock_count < 0 means 1 (or more, if recursive) write locks.  Added
 * "waiting_writers" field, a queue of threads waiting for write lock.
 * Changed "waiting" field to "waiting_readers", to indicate its only
 * remaining function.  The three boolean fields are now declared as
 * 8-bit fields to save space.  Lock structure has shrunk from 8 to 6
 * words on the i386 architecture.
 * 
 * Algorithm changes for performance:
 * 
 * (1) Threads wanting write lock put themselves in waiting_writers
 *     queue and do non-event-based sleep.  This is so we can awaken
 *     just 1 writer, instead of all of them.
 * 
 * (2) The reader waiting for upgrade sleeps on different event
 *     number from other readers, so it can be individually awakened.
 * 
 * (3) We now do minimum wakeups necessary.  For fairness, we prefer
 *     to wake a writer after releasing last read lock, and wake all
 *     readers after releasing last write lock, if any.
 * 
 * (4) simple_lock_solid now calls thread_yield, instead of
 *     cthread_yield, to ensure that that every other Mach thread
 *     gets to run, not just every thread in the C-thread run queue.
 *     This is better because thread that holds the simple lock will
 *     always be running in a different Mach thread and never be in
 *     C-thread run queue.
 * 
 * (5) When NCPUS > 1, hard spin of fixed number of iterations was
 *     performed before each sleep, in hopes that another CPU would
 *     release the lock.  The OSF/1 servers use NCPUS > 1 even on
 *     uni-processor, so changed this to be a spin with thread_yield
 *     in it, and much fewer iterations.
 * 
 * Other changes:
 * 
 * (1) Readability: "want_upgrade" bit now only means that a reader
 *     is waiting for upgrade -- while waiting for upgrade, normal
 *     read lock is held; after successful upgrade, lock is
 *     indistinguishable from any other write lock.
 * 
 * (2) New readers are locked out by a non-empty waiting_writers
 *     queue, taking the place of one of the functions of the
 *     old want_write bit.
 * 
 * (3) Correctness: In recursive locks, read locks are mapped to
 *     write locks.  This is necessary because a recursive read lock
 *     on top of a write lock must not let any other readers in.
 *     Recursive locks are nothing more than multiple write locks
 *     taken by a single thread.
 * 
 * (4) Correctness: can_sleep==0 used to cause the thread to spin
 *     hard with the interlock held.  Now we release the interlock
 *     and yield the thread on each spin.
 */

#include <cpus.h>

#include <kern/lock.h>
#ifdef	OSF1_SERVER
#include <sys/user.h>		/* For current_thread() */
#include <machine/cpu.h>	/* For fake cpu_number() */
#else	/* OSF1_SERVER */
#include <kern/thread.h>
#endif	/* OSF1_SERVER */
#include <kern/sched_prim.h>

#if	MACH_LDEBUG
#include <kern/assert.h>

#define	LDEBUG(clause)		clause

int	check_locks = 0;	/* don't check until current_thread works */
int	check_lock_counts = 0;	
struct slock_debug slck_dbg[NCPUS];

#else	/* MACH_LDEBUG */

#define	LDEBUG(clause)

#endif	/* MACH_LDEBUG */


#if	NCPUS > 1

/*
 *	Module:		lock
 *	Function:
 *		Provide reader/writer sychronization.
 *	Implementation:
 *		Simple interlock on a bit.  Readers first interlock
 *		increment the reader count, then let go.  Writers hold
 *		the interlock (thus preventing further readers), and
 *		wait for already-accepted readers to go away.
 */

/*
 *	The simple-lock routines are the primitives out of which
 *	the lock package is built.  The implementation is left
 *	to the machine-dependent code.
 */
void _simple_lock_solid(l)
	spin_lock_t	*l;
{
	while (spin_lock_locked(l) || !spin_try_lock(l))
		thread_yield();
}

#ifdef	notdef
/*
 *	A sample implementation of simple locks.
 *	assumes:
 *		boolean_t test_and_set(boolean_t *)
 *			indivisibly sets the boolean to TRUE
 *			and returns its old value
 *		and that setting a boolean to FALSE is indivisible.
 */
/*
 *	simple_lock_init initializes a simple lock.  A simple lock
 *	may only be used for exclusive locks.
 */

void simple_lock_init(l)
	simple_lock_t	l;
{
	*(boolean_t *)l = FALSE;
}

void simple_lock(l)
	simple_lock_t	l;
{
	while (test_and_set((boolean_t *)l))
		continue;
}

void simple_unlock(l)
	simple_lock_t	l;
{
	*(boolean_t *)l = FALSE;
}

boolean_t simple_lock_try(l)
	simple_lock_t	l;
{
    	return (!test_and_set((boolean_t *)l));
}
#endif	/* notdef */
#endif	/* NCPUS > 1 */

#if	NCPUS > 1
int lock_wait_time = 5;
#else

	/*
	 * 	It is silly to spin on a uni-processor as if we
	 *	thought something magical would happen to the
	 *	lock_count while we are executing.
	 */
int lock_wait_time = 0;
#endif

#if	MACH_SLOCKS && (defined(ibmrt) || (NCPUS == 1))
/* Need simple lock sanity checking code if simple locks are being
   compiled in, and either we are on RT (which doesn't have any special
   locking code of its own) or we are compiling for a uniprocessor. */

void simple_lock_init(l)
	simple_lock_t l;
{
	l->lock_data = 0;
#if	MACH_LDEBUG
	l->slthread = (char *) 0;
	l->slck_addr = (int) -1;
	l->sunlck_addr = (int) -1;
#endif
}

void simple_lock(l)
	simple_lock_t l;
{
	assert(l->lock_data == 0);

	l->lock_data = 1;
#if	MACH_LDEBUG
	l->slthread = (char *) current_thread();
	inc_slock(l);
#endif
}

void simple_unlock(l)
	simple_lock_t l;
{
	assert(l->lock_data != 0);

	l->lock_data = 0;
#if	MACH_LDEBUG
	l->slthread = (char *) 0;
	dec_slock(l);
#endif
}

boolean_t simple_lock_try(l)
	simple_lock_t l;
{
	assert(l->lock_data == 0);

	l->lock_data = 1;
#if	MACH_LDEBUG
	l->slthread = (char *) current_thread();
	inc_slock(l);
#endif

	return TRUE;
}
#endif	/* MACH_SLOCKS && (defined(ibmrt) || (NCPUS == 1)) */

/*
 *	Routine:	lock_init
 *	Function:
 *		Initialize a lock; required before use.
 *		Note that clients declare the "struct lock"
 *		variables and then initialize them, rather
 *		than getting a new one from this module.
 */
void do_lock_init(l, can_sleep)
	lock_t		l;
	boolean_t	can_sleep;
{
	/* Initialize the debug fields: */
	LDEBUG(bzero(l, sizeof(lock_data_t)));

	simple_lock_init(&l->interlock);
	l->want_upgrade = FALSE;
	l->waiting_readers = FALSE;
	l->can_sleep = can_sleep;
	l->lock_count = 0;
	queue_init(&l->waiting_writers);
	l->thread = (char *)0;
#ifdef	ns32000
	l->lock_type = 0;
#endif
	LDEBUG(l->lck_addr = 0xffffffff);	/* Lock is not yet held */
#if	LOCK_STATS
	l->lock_tries = l->lock_fails = l->lock_sleeps = 0;
	l->lock_wait_min = -1;
	l->lock_wait_max = l->lock_wait_sum = 0;
	l->lock_max_read = 0;
	l->lock_nreads = 0;
#endif
}

void lock_init2(l, can_sleep, ltype)
	lock_t		l;
	boolean_t	can_sleep;
	int		ltype;
{
	lock_init(l, can_sleep);
#ifdef	ns32000
	l->lock_type = ltype;
#endif
}

void do_lock_sleepable(l, can_sleep)
	lock_t		l;
	boolean_t	can_sleep;
{
	simple_lock(&l->interlock);
	l->can_sleep = can_sleep;
	simple_unlock(&l->interlock);
}


/*
 *	Sleep locks.  These use the same data structure and algorithm
 *	as the spin locks, but the process sleeps while it is waiting
 *	for the lock.  These work on uniprocessor systems.
 */

#if	MACH_LDEBUG || MACH_LTRACKS
#define	LTRACK(clause)	clause
#define	LTRACK_PARAM(param)	,param

#if	defined(multimax)
#define	LTRACK_DONE(l)	((l)->lthread=(char*)((int)((l)->lthread)|0x80000000))
#else
#define	LTRACK_DONE(l)	((l)->lthread=(char*)0)
#endif
#else	/* MACH_LDEBUG || MACH_LTRACKS */
#define	LTRACK(clause)
#define	LTRACK_PARAM(param)
#define	LTRACK_DONE(l)
#endif

#if	(MACH_LDEBUG || MACH_LTRACKS) && defined(multimax)
#define	MMAX_LTRACK(clause)	clause
#else
#define	MMAX_LTRACK(clause)
#endif

#if	LOCK_STATS && defined(multimax)
#define	LOCK_STATS_ACTIONS	1
#else
#define	LOCK_STATS_ACTIONS	0
#endif


#if	LOCK_STATS_ACTIONS

#define	LSTATS(clause)		clause
#define	LSTATS_TIME(t)		((t) = FRcounter)

#define	LSTATS_TIME_SUM(t,s0,s1)					\
MACRO_BEGIN								\
	unsigned int	delta;						\
	if (((int)(delta = (s1) - (s0))) < 0)				\
		delta = 0 - delta;					\
	(t) += delta;							\
MACRO_END

#define	LSTATS_ACCUMULATE(l,total_time)					\
MACRO_BEGIN								\
	(l)->lock_wait_sum += (total_time);				\
	if ((total_time) > 0 && (total_time) < (l)->lock_wait_min)	\
		(l)->lock_wait_min = (int) (total_time);		\
	else if ((total_time) > (l)->lock_wait_max)			\
		(l)->lock_wait_max = (int) (total_time);		\
MACRO_END

#else	/* LOCK_STATS_ACTIONS */

#define	LSTATS(clause)
#define	LSTATS_TIME(t)
#define	LSTATS_TIME_SUM(t,s0,s1)
#define	LSTATS_ACCUMULATE(l,total_time)

#endif	/* LOCK_STATS_ACTIONS */

/* Threads who want to lock for write must, before sleeping, put themselves
 * on a queue of threads that hangs off the lock data.  This is so a
 * thread releasing the lock can use the queue to find and wakeup exactly one
 * writer.
 */
void
lock_write_sleep(l, queue_at_head)
	register lock_t	l;
        boolean_t  queue_at_head;
{
	register uthread_t	thread = current_thread();

	/* Check for failure to initialize lock: */
	LASSERT(queue_next(&l->waiting_writers)!= NULL);

	assert_wait(0, FALSE);
	if (queue_at_head) 
	    queue_enter_first(&l->waiting_writers, 
			      thread, uthread_t, 
			      uu_sleep_link);
	else
	    queue_enter(&l->waiting_writers, thread, uthread_t, uu_sleep_link);
	simple_unlock(&l->interlock);
	thread->uu_lock_sleep = 1;	/* See thread_block() */
	thread_block();
	thread->uu_lock_sleep = 0;
	simple_lock(&l->interlock);
	queue_remove(&l->waiting_writers, thread, uthread_t, uu_sleep_link);
}

/*
 * Dequeue and awaken the first sleeping waiter on the lock:
 */
lock_write_wakeup(l)
	register lock_t	l;
{
	register uthread_t	thread;

	/* Check for failure to initialize lock: */
	LASSERT(queue_next(&l->waiting_writers)!= NULL);

	thread = (uthread_t) queue_first(&l->waiting_writers);
	clear_wait(thread, THREAD_AWAKENED, FALSE);
}



void do_lock_write(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{
	register int		i;
	register boolean_t	queue_at_head;

#if	LOCK_STATS_ACTIONS
	register unsigned int	start_time, stop_time, total_time;
#endif

	/*LASSERT(l>=kernel_map->vm_map_min&&l<=kernel_map->vm_map_max);*/
	simple_lock(&l->interlock);
	LSTATS(l->lock_tries++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  Add one more write lock.
		 */
		LASSERT(l->lock_count < 0);
		l->lock_count--;
		simple_unlock(&l->interlock);
		LDEBUG(inc_lock(l, current_thread()));
		return;
	}

	/*
	 * Check that we do not already hold a read or a write lock:
	 */
	LASSERT(!LOCK_OWNER(l));
	LSTATS(total_time = 0);

	/*
	 *	Try to acquire a write lock.
	 *
	 * The first time we sleep, we go to back of the queue of waiting
	 * writers.  If we are awakened (because we reached the head of the
	 * queue) but can't get lock, it is probably because someone who
	 * didn't sleep at all squeezed in between the lock release and our
	 * wakeup.  So in the 2nd and subsequent sleeps, we queue ourself
	 * at the front of the queue (we are the longest-waiting thread).
	 */
	queue_at_head = FALSE;
	while (l->lock_count != 0) {
		LSTATS(l->lock_fails++);
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->lock_count != 0)
				thread_yield();
			simple_lock(&l->interlock);
		}

		if (l->lock_count != 0) {
			LSTATS(l->lock_sleeps++);
			LSTATS_TIME(start_time);
			if (l->can_sleep)
				lock_write_sleep(l, queue_at_head);
			else {
				simple_unlock(&l->interlock);
				thread_yield();
				simple_lock(&l->interlock);
			}
			LSTATS_TIME(stop_time);
			LSTATS_TIME_SUM(total_time, start_time, stop_time);
		}
		queue_at_head = TRUE;
	}
	l->lock_count = -1;

	LSTATS_ACCUMULATE(l,total_time);
	LASSERT(l->lck_addr & 0x80000000);
	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	MMAX_LTRACK(l->lck_addr = getfrompc());

	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
}


/*
 * Release one read or write lock.
 *
 * We do the minimum amount of wakeups necessary.  We guarantee that either
 * exactly one writer or all readers, but not both, are awakened,
 * and that threads are only awakened if they will be able to take
 * the lock.
 */
void do_lock_done(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{
	simple_lock(&l->interlock);

	if (l->lock_count > 0) {
		/*
		 * Releasing a read lock.
		 */
#if	LOCK_STATS_ACTIONS
		if (l->lock_count > l->lock_max_read)
			l->lock_max_read = l->lock_count;
#endif
		l->lock_count--;
		LTRACK((uthread_t)(l->lthread) == current_thread() ?
							LTRACK_DONE(l) : 0);

		if (l->want_upgrade) {
			/* There is a waiting upgrader, which is the only thread
			 * that can make progress, and then only if this was the
			 * last read lock besides the upgrader's:
			 */
			if (l->lock_count == 1)
				thread_wakeup(4 + (int) l);

		} else if (l->lock_count == 0 &&
					    !queue_empty(&l->waiting_writers)) {
			/* We've just released the last read lock and
			 * there are waiting writers.  For fairness, we prefer
			 * to wake a writer, since readers just got their turn.
			 */
			lock_write_wakeup(l);

		} else if (l->waiting_readers &&
					    queue_empty(&l->waiting_writers)) {
			/* This can probably never happen.  Why would
			 * there be any waiting readers if there are no
			 * waiting writers to block them and we just
			 * released a read lock?  But it's cheap and
			 * safe to check.
			 */
			l->waiting_readers = FALSE;
			thread_wakeup((int) l);
		}
	} else {
		/*
		 * Releasing a write lock.
		 */
		LASSERT(l->lock_count < 0);
#ifndef	OSF1_ADFS
		LASSERT(LOCK_OWNER(l));
#endif
	 	l->lock_count++;

		if (l->lock_count == 0) {
			LTRACK_DONE(l);
			/* We've just released the last write lock.  There
			 * may be waiting readers and/or writers.  For fairness,
			 * we prefer to wake the readers, since a writer just
			 * got its turn:
			 */
			if (l->waiting_readers) {
				l->waiting_readers = FALSE;
				thread_wakeup((int) l);

			} else if (!queue_empty(&l->waiting_writers))
				lock_write_wakeup(l);
		} else {
			/* We must be the holder of a recursive lock: */
			LASSERT((uthread_t)l->thread == current_thread());
		}
	}

	LTRACK(l->lck_addr |= 0x80000000);
	LTRACK(l->unlck_addr = frompc);
#if	0
	/*
	 * Gcc puts an enter [] for all functions, so lock_done
	 * can now use the fp to determine where it came from.
	 * (In any case, the stack has changed and getpc_fromld is
	 * incorrect.)
	 */
	MMAX_LTRACK(l->unlck_addr = getpc_fromld());
#else
	MMAX_LTRACK(l->unlck_addr = getfrompc());
#endif
	simple_unlock(&l->interlock);
	LDEBUG(dec_lock(l, current_thread()));
}

void do_lock_read(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{
	register int		i;
	register boolean_t	first_time;
#if	LOCK_STATS_ACTIONS
	register unsigned int	start_time, stop_time, total_time;
#endif

	simple_lock(&l->interlock);
	LSTATS(l->lock_nreads++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  Take another write lock instead of a
		 *	read lock, since there are already write locks and we
		 *	cannot mix lock types.
		 */
		LASSERT(l->lock_count < 0);
		l->lock_count--;
		simple_unlock(&l->interlock);
		LDEBUG(inc_lock(l, current_thread()));
		return;
	}

	/*
	 * Check that we do not already hold a write lock:
	 */
	LASSERT(!LOCK_HOLDER(l));

	/*
	 * Acquire a read lock.  The first time we try, we do not go ahead
	 * of any waiting writers, since they were there first.  When we
	 * are awakened, it was probably because a write lock just finished
	 * and decided it's our turn, so we go ahead in spite of waiting
	 * writers:
	 */
	LSTATS(total_time = 0);
	first_time = TRUE;
	while (l->lock_count < 0 ||
			(first_time && !queue_empty(&l->waiting_writers)) ||
							    l->want_upgrade) {
		LSTATS(l->lock_fails++);
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && (l->lock_count < 0 ||
					   (first_time &&
					   !queue_empty(&l->waiting_writers)) ||
							       l->want_upgrade))
				thread_yield();
			simple_lock(&l->interlock);
		}

		if (l->lock_count < 0 ||
			    (first_time && !queue_empty(&l->waiting_writers)) ||
							      l->want_upgrade) {
			l->waiting_readers = TRUE;
			LSTATS(l->lock_sleeps++);
			LSTATS_TIME(start_time);
			if (l->can_sleep) {
				u.uu_lock_sleep = 1;	/* See thread_block() */
				thread_sleep((int) l,
					simple_lock_addr(l->interlock), FALSE);
				u.uu_lock_sleep = 0;
			} else {
				simple_unlock(&l->interlock);
				thread_yield();
			}
			simple_lock(&l->interlock);
			LSTATS_TIME(stop_time);
			LSTATS_TIME_SUM(total_time, start_time, stop_time);
		}
		first_time = FALSE;
	}
	l->lock_count++;

	LSTATS_ACCUMULATE(l,total_time);
	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	MMAX_LTRACK(l->lck_addr = getfrompc());

	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
}

/*
 *	Routine:	do_lock_read_to_write
 *	Function:
 *		Upgrades a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		the upgrade fails and no read lock is held upon return.
 *
 *		Returns TRUE if the upgrade *failed*.
 */
boolean_t do_lock_read_to_write(l)
	register lock_t	l;
{
	register int	i;
#if	LOCK_STATS_ACTIONS
	register unsigned int	start_time, stop_time, total_time;
#endif

	simple_lock(&l->interlock);

	LSTATS(l->lock_tries++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  We already hold write locks, so do
		 *	nothing.
		 */
		LASSERT(l->lock_count < 0);
		simple_unlock(&l->interlock);
		return(FALSE);
	}
	if (l->lock_count <= 0)	panic("lock upgrade w/o read lock");

	LSTATS(total_time = 0);
	if (l->want_upgrade) {
		/*
		 *	Someone else has beat us to it, so give up our
		 *	read lock.  If we've released the last read lock,
		 *	wake him up.
		 */
		LASSERT(l->lock_count > 1);
		l->lock_count--;
		if (l->lock_count == 1)
			thread_wakeup(4 + (int) l);

		LSTATS(l->lock_fails++);
		LTRACK((uthread_t)(l->lthread) == current_thread() ?
							LTRACK_DONE(l) : 0);
		simple_unlock(&l->interlock);
		LDEBUG(dec_lock(l, current_thread()));
		return (TRUE);
	}
	l->want_upgrade = TRUE;

	/*
	 * Wait until all other readers finish and we have the only
	 * read lock:
	 */
	while (l->lock_count > 1) {
		LSTATS(l->lock_fails++);
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->lock_count > 1)
				thread_yield();
			simple_lock(&l->interlock);
		}

		if (l->lock_count > 1) {
			l->waiting_readers = TRUE;
			LSTATS(l->lock_sleeps++);
			LSTATS_TIME(start_time);
			if (l->can_sleep) {
				u.uu_lock_sleep = 1;	/* See thread_block() */
				thread_sleep(4 + (int) l,
					simple_lock_addr(l->interlock), FALSE);
				u.uu_lock_sleep = 0;
			} else {
				simple_unlock(&l->interlock);
				thread_yield();
			}
			simple_lock(&l->interlock);
			LSTATS_TIME(stop_time);
			LSTATS_TIME_SUM(total_time, start_time, stop_time);
		}
	}
	if (l->lock_count != 1)
		panic("lock_read_to_write: lock count not 1");
	l->want_upgrade = FALSE;

	/* Change it to a write lock: */
	l->lock_count = -1;

	LSTATS_ACCUMULATE(l,total_time);
	LTRACK(l->lthread = (char *) current_thread());

	simple_unlock(&l->interlock);
	return (FALSE);
}

void do_lock_write_to_read(l)
	register lock_t	l;
{
	simple_lock(&l->interlock);

	LSTATS(l->lock_nreads++);
	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  We already hold write locks, so do
		 *	nothing.
		 */
		LASSERT(l->lock_count < 0);
		simple_unlock(&l->interlock);
		return;
	}
	LASSERT(l->lock_count == -1);

	/* Change it to a read lock: */
	l->lock_count = 1;

	/* Other readers may now join us: */
	if (l->waiting_readers) {
		l->waiting_readers = FALSE;
		thread_wakeup((int) l);
	}

	simple_unlock(&l->interlock);
}


/*
 *	Routine:	do_lock_try_write
 *	Function:
 *		Tries to get a write lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t do_lock_try_write(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{

	simple_lock(&l->interlock);

	LSTATS(l->lock_tries++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  We already hold write locks, so we
		 *	can always take one more.
		 */
		LASSERT(l->lock_count < 0);
		l->lock_count--;
		simple_unlock(&l->interlock);
		LDEBUG(inc_lock(l, current_thread()));
		return(TRUE);
	}

	if (l->lock_count != 0) {
		/*
		 *	Can't get lock.
		 */
		LSTATS(l->lock_fails++);
		simple_unlock(&l->interlock);
		return(FALSE);
	}
	l->lock_count = -1;

	/*
	 *	Have lock.
	 */
	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	MMAX_LTRACK(l->lck_addr = getfrompc());
	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
	return(TRUE);
}

/*
 *	Routine:	do_lock_try_read
 *	Function:
 *		Tries to get a read lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t do_lock_try_read(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{
	simple_lock(&l->interlock);

	LSTATS(l->lock_tries++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	Recursive lock.  Take another write lock instead of a
		 *	read lock, since there are already write locks and we
		 *	can't mix lock types.
		 */
		LASSERT(l->lock_count < 0);
		l->lock_count--;
		simple_unlock(&l->interlock);
		LDEBUG(inc_lock(l, current_thread()));
		return(TRUE);
	}

	if (l->lock_count < 0 || !queue_empty(&l->waiting_writers) ||
							    l->want_upgrade) {
		simple_unlock(&l->interlock);
		return(FALSE);
	}

	l->lock_count++;
	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
	return(TRUE);
}

/*
 * Combine lock_try_read with an assert_wait call to 
 * prepare caller to wait for lock without acquiring it.
 *
 * NOTE: this interface does not work with recursive locks.
 */
boolean_t do_lock_try_read_assert(l LTRACK_PARAM(frompc))
        register lock_t l;
	LTRACK(int	frompc;)
{
	simple_lock(&l->interlock);

	LSTATS(l->lock_tries++);

	if (l->lock_count < 0 || !queue_empty(&l->waiting_writers) ||
							    l->want_upgrade) {
		/*
		 *	Can't get lock.
		 */
		LSTATS(l->lock_fails++);
		l->waiting_readers = TRUE;
		assert_wait((int)l, FALSE);
		simple_unlock(&l->interlock);
		return(FALSE);
	}
	l->lock_count++;

	/*
	 *	Have lock.
	 */

	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
	return(TRUE);
}

/*
 *	Routine:	do_lock_try_read_to_write
 *	Function:
 *		Upgrades a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		the upgrade fails, but the read lock is still
 *		held upon return.
 *
 *		Returns FALSE if the upgrade *failed*.
 */
boolean_t do_lock_try_read_to_write(l LTRACK_PARAM(frompc))
	register lock_t	l;
	LTRACK(int	frompc;)
{
	register int	i;
#if	LOCK_STATS_ACTIONS
	register unsigned int	start_time, stop_time, total_time;
#endif

	simple_lock(&l->interlock);

	LSTATS(l->lock_tries++);

	if ((uthread_t)l->thread && (uthread_t)l->thread == current_thread()) {
		/*
		 *	We already hold recursive write lock, so do nothing.
		 */
		LASSERT(l->lock_count < 0);
		simple_unlock(&l->interlock);
		return(TRUE);
	}
	if (l->lock_count <= 0)	panic("lock upgrade try w/o read lock");

	if (l->want_upgrade) {
		/*
		 *	Someone else has beat us to it.
		 */
		simple_unlock(&l->interlock);
		LDEBUG(dec_lock(l, current_thread()));
		return(FALSE);
	}
	l->want_upgrade = TRUE;

	/*
	 * Wait until all other readers finish and we have the only
	 * read lock:
	 */
	while (l->lock_count > 1) {
		LSTATS(l->lock_fails++);
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->lock_count > 1)
				thread_yield();
			simple_lock(&l->interlock);
		}

		if (l->lock_count > 1) {
			l->waiting_readers = TRUE;
			LSTATS(l->lock_sleeps++);
			LSTATS_TIME(start_time);
			if (l->can_sleep) {
				u.uu_lock_sleep = 1;	/* See thread_block() */
				thread_sleep(4 + (int) l,
					simple_lock_addr(l->interlock), FALSE);
				u.uu_lock_sleep = 0;
			} else {
				simple_unlock(&l->interlock);
				thread_yield();
			}
			simple_lock(&l->interlock);
			LSTATS_TIME(stop_time);
			LSTATS_TIME_SUM(total_time, start_time, stop_time);
		}
	}
	if (l->lock_count != 1)
		panic("lock_read_to_write: lock count not 1");
	l->want_upgrade = FALSE;

	/* Change it to a write lock: */
	l->lock_count = -1;

	LTRACK(l->lthread = (char *) current_thread());
	LTRACK(l->lck_addr = frompc);
	MMAX_LTRACK(l->lck_addr = getfrompc());
	simple_unlock(&l->interlock);
	return(TRUE);
}

/*
 *	Allow a process that has a lock for write to acquire it
 *	recursively (for read, write, or update).
 */
void lock_set_recursive(l)
	lock_t		l;
{
	simple_lock(&l->interlock);
	if (! l->lock_count < 0) {
		panic("lock_set_recursive: don't have write lock");
	}
	l->thread = (char *) current_thread();
	simple_unlock(&l->interlock);
	LDEBUG(inc_lock(l, current_thread()));
}

/*
 *	Prevent multiple locks from being acquired by any thread.
 */
void lock_clear_recursive(l)
	lock_t		l;
{
	simple_lock(&l->interlock);
	if ((uthread_t)l->thread != current_thread()) {
		panic("lock_clear_recursive: wrong thread");
	}
	LASSERT(l->lock_count < 0);
	if (l->lock_count == -1)
		l->thread = (char *)0;
	simple_unlock(&l->interlock);
	LDEBUG(dec_lock(l, current_thread()));
}


/*
 * Test existence of the lock
 */

int
lock_islocked(l)
lock_t	l;
{
	int	ret;

	LASSERT(l != 0);

	if(simple_lock_try(&l->interlock)) {
		ret = l->lock_count != 0;
		simple_unlock(&l->interlock);
		return(ret);
	} else
		return(1);
}


#if	MACH_LDEBUG

/*
 * Probably this code should go away.  XXX
 */

decl_simple_lock_data(extern, printf_lock)

/*
 * Panic on unlocking an unlocked simple lock.  There's an exception
 * for the printf lock, which we force to be unlocked at panic time.
 */
slpanic(lp, pc)
struct	slock	*lp;
int	pc;
{
	printf("Unlocking unlocked simple lock @ 0x%x from 0x%x\n", lp, pc);
	printf("lckaddr = 0x%x, unlockaddr = 0x%x\n",
	       lp->slck_addr, lp->sunlck_addr);
	if (lp == simple_lock_addr(printf_lock))
		return;
	panic("simple unlock");
}

/*
 * Record current thread and lock addresses.
 */
slhack(lp, pc)
struct	slock	*lp;
int	pc;
{
	lp->slthread = (char *) current_thread();
	lp->slck_addr = pc;
	lp->sunlck_addr |= 0x80000000;
	inc_slock(lp);
}

struct slock *
sunhack(lp, pc)
struct	slock	*lp;
int	pc;
{
	if(lp->slck_addr & 0x80000000)
		slpanic(lp, pc);
	if((int)lp->slthread & 0x80000000)
		slpanic(lp, pc);

	dec_slock(lp);
	if(lp->slthread != (char *) current_thread()) {
		printf("sunhack: lp = 0x%x, thread = 0x%x\n",
			lp, lp->slthread);
		if (lp == simple_lock_addr(printf_lock))
			return;
		panic("I didn't lock this lock");
	}

	lp->sunlck_addr = pc;
	lp->slck_addr |= 0x80000000;
	lp->slthread = (char *)((int)lp->slthread | 0x80000000);
	/*
	 * We return LP here so the in-line assembly
	 *  version doesn't have to save and restore
	 *  the lock address.
	 */
	return(lp);
}

dec_slock(l)
struct slock *l;
{
	int i, j, found;
	int cpu = cpu_number();

	if (!check_locks || !check_lock_counts ||
	    l == simple_lock_addr(printf_lock))
		return;
	slck_dbg[cpu].count--;
	j = 0;
	found = MAX_LOCK;
	for (i = 0; i < MAX_LOCK; i++) {
		if (slck_dbg[cpu].addr[i] == (int)l) {
			slck_dbg[cpu].addr[i] = 0;
			found = i;
		}
		if (slck_dbg[cpu].addr[i] != 0)
			j++;
	}
	if (found == MAX_LOCK)
		panic("dec_slock");
	ASSERT(j == slck_dbg[cpu].count);

}

inc_slock(l)
struct slock *l;
{
	int cpu = cpu_number();
	int i, j, found;

	if (!check_locks  || !check_lock_counts ||
	    l == simple_lock_addr(printf_lock))
		return;
	if (slck_dbg[cpu].count++ >= MAX_LOCK)
		panic("slock debug");
	j = 0;
	found = 0;
	for (i = 0; i < MAX_LOCK; i++) {
		if (!found && slck_dbg[cpu].addr[i] == 0) {
			slck_dbg[cpu].addr[i] = (int)l;
			found = 1;
		} 
		if (slck_dbg[cpu].addr[i] != 0)
			j++;
	}
	ASSERT(j == slck_dbg[cpu].count);
}

#define lock_count uu_lock_count
#define lock_addr uu_lock_addr

inc_lock(l, th)
lock_t l;
uthread_t th;
{
	int i, j, found;

	if (!check_locks || !check_lock_counts)
		return;
	if (th->lock_count++ >= MAX_LOCK)
		panic("lock debug");
	j = 0;
	found = 0;
	for (i = 0; i < MAX_LOCK; i++) {
		if (!found && th->lock_addr[i] == 0) {
			th->lock_addr[i] = (int)l;
			found = 1;
		} 
		if (th->lock_addr[i] != 0)
			j++;
	}
	ASSERT(j == th->lock_count);
}

dec_lock(l, th)
lock_t l;
uthread_t th;
{
	int i, j, found;

	if (!check_locks || !check_lock_counts)
		return;
	th->lock_count--;
	j = 0;
	found = MAX_LOCK;
	for (i = 0; i < MAX_LOCK; i++) {
		if (found == MAX_LOCK && th->lock_addr[i] == (int)l) {
			th->lock_addr[i] = 0;
			found = i;
		}
		if (th->lock_addr[i] != 0)
			j++;
	}
	if (found == MAX_LOCK) {
		printf("Dec_lock: Looking for lock 0x%x\n",l);
		for (i = 0; i < MAX_LOCK; i++) 
			printf("th->lock_addr[%d] = 0x%x\n", i, th->lock_addr[i]);
		panic("dec_lock");
	}
	ASSERT(j == th->lock_count);

}
#endif	/* MACH_LDEBUG */

