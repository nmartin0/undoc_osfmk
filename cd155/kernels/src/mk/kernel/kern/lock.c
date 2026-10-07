/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989,1988,1987 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: lock.c,v $
 * Revision 1.10  1994/11/18  20:52:13  mtm
 * Copyright additions/changes
 *
 * Revision 1.9  1994/05/06  21:25:48  lenb
 *         to be consistent with slock.s, always define simple_lock_init().
 *
 * Revision 1.8  1994/03/19  01:21:04  lenb
 * Benefit: simple_lock_init() moved to C; instrumentation needs some cleanup.
 * Testing: SAT
 * Reviewer: sean
 *
 * Revision 1.7  1993/12/14  23:03:03  steved
 * Support for Turbo On Demand and fixed a bug in lock.c which did
 * not store enough bits to record which CPU locked/unlocked the lock
 *
 * Revision 1.6  1993/10/01  00:19:20  stans
 *    MP3: Bulletproof LDEBUG simple_{un}lock() routines by checking for <null>
 * 	lock pointer argument.
 *
 * Revision 1.5  1993/06/30  22:44:59  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.4  1993/04/27  20:35:56  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.2.6.2  1993/04/22  18:36:04  dleslie
 * First R1_0 release
 *
 * 23-Sep-92  Philippe Bernadat (bernadat) at gr.osf.org
 *	Support for MACH_LDEBUG
 *	Moved MACH_MP_DEBUG code from kern/lock_mon.c to here
 *
 * Revision 2.7.2.1  92/03/03  16:20:08  jeffreyh
 * 	Changes from TRUNK
 * 	[92/02/26  11:54:39  jeffreyh]
 * 
 * Revision 2.8  92/01/23  15:20:56  rpd
 * 	Fixed lock_done and lock_read_to_write to only wakeup
 * 	if the number of readers is zero (from Sanjay Nadkarni).
 * 	Fixed db_show_all_slocks.
 * 	[92/01/20            rpd]
 * 
 * Revision 2.7  91/07/01  08:25:03  jsb
 * 	Implemented db_show_all_slocks.
 * 	[91/06/29  16:52:53  jsb]
 * 
 * Revision 2.6  91/05/18  14:32:07  rpd
 * 	Added simple_locks_info, to keep track of which locks are held.
 * 	[91/05/02            rpd]
 * 	Added check_simple_locks.
 * 	[91/03/31            rpd]
 * 
 * Revision 2.5  91/05/14  16:43:40  mrt
 * 	Correcting copyright
 * 
 * Revision 2.4  91/02/05  17:27:31  mrt
 * 	Changed to new Mach copyright
 * 	[91/02/01  16:14:31  mrt]
 * 
 * Revision 2.3  90/06/02  14:54:56  rpd
 * 	Allow the lock to be taken in simple_lock_try.
 * 	[90/03/26  22:09:13  rpd]
 * 
 * Revision 2.2  90/01/11  11:43:18  dbg
 * 	Upgrade to mainline: use simple_lock_addr when calling
 * 	thread_sleep.
 * 	[89/12/11            dbg]
 * 
 * Revision 2.1  89/08/03  15:45:34  rwd
 * Created.
 * 
 * Revision 2.3  88/09/25  22:14:45  rpd
 * 	Changed explicit panics in sanity-checking simple locking calls
 * 	to assertions.
 * 	[88/09/12  23:03:22  rpd]
 * 
 * Revision 2.2  88/07/20  16:35:47  rpd
 * Add simple-locking sanity-checking code.
 * 
 * 23-Jan-88  Richard Sanzi (sanzi) at Carnegie-Mellon University
 *	On a UNIPROCESSOR, set lock_wait_time = 0.  There is no reason
 *	for a uni to spin on a complex lock.
 *
 * 18-Nov-87  Avadis Tevanian (avie) at Carnegie-Mellon University
 *	Eliminated previous history.
 */
/*
 *	File:	kern/lock.c
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Date:	1985
 *
 *	Locking primitives implementation
 */

#include <cpus.h>
#include <mach_kdb.h>

#include <kern/lock.h>
#include <kern/thread.h>
#include <kern/sched_prim.h>
#if	MACH_KDB
#include <ddb/db_sym.h>
#endif

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
#endif	notdef
#endif	NCPUS > 1

#if	NCPUS > 1
int lock_wait_time = 100;
#else	NCPUS > 1

	/*
	 * 	It is silly to spin on a uni-processor as if we
	 *	thought something magical would happen to the
	 *	want_write bit while we are executing.
	 */
int lock_wait_time = 0;
#endif	NCPUS > 1

#if	MACH_SLOCKS && NCPUS == 1
/*
 *	This code does not protect simple_locks_taken and simple_locks_info.
 *	It works despite the fact that interrupt code does use simple locks.
 *	This is because interrupts use locks in a stack-like manner.
 *	Each interrupt releases all the locks it acquires, so the data
 *	structures end up in the same state after the interrupt as before.
 *	The only precaution necessary is that simple_locks_taken be
 *	incremented first and decremented last, so that interrupt handlers
 *	don't over-write active slots in simple_locks_info.
 */

unsigned int simple_locks_taken = 0;

#define	NSLINFO	1000		/* maximum number of locks held */

struct simple_locks_info {
	simple_lock_t l;
	unsigned int ra;
} simple_locks_info[NSLINFO];

void check_simple_locks()
{
	assert(simple_locks_taken == 0);
}

/* Need simple lock sanity checking code if simple locks are being
   compiled in, and we are compiling for a uniprocessor. */

void simple_lock(l)
	simple_lock_t l;
{
	struct simple_locks_info *info;

	assert(l->lock_data == 0);

	l->lock_data = 1;

	info = &simple_locks_info[simple_locks_taken++];
	info->l = l;
	/* XXX we want our return address, if possible */
#ifdef	i860
	{
		extern int	get_fp();

		info->ra = *((unsigned int *)(get_fp()) + 1);
		l->lock_pc = info->ra;
		l->unlock_pc = 0xdead;
	}
#endif	i860

#ifdef	i386
	info->ra = *((unsigned int *)&l - 1);
#endif	i386
}

boolean_t simple_lock_try(l)
	simple_lock_t l;
{
	struct simple_locks_info *info;

	if (l->lock_data != 0)
		return FALSE;

	l->lock_data = 1;

	info = &simple_locks_info[simple_locks_taken++];
	info->l = l;
	/* XXX we want our return address, if possible */
#ifdef	i860
	{
		extern int	get_fp();

		info->ra = *((unsigned int *)(get_fp()) + 1);
		l->unlock_pc = info->ra;
	}
#endif	i860

#ifdef	i386
	info->ra = *((unsigned int *)&l - 1);
#endif	i386

	return TRUE;
}

void simple_unlock(l)
	simple_lock_t l;
{
	assert(l->lock_data != 0);

	l->lock_data = 0;
#if	i860
	{
		extern int	get_fp();

		l->unlock_pc = *((unsigned int *)(get_fp()) + 1);
		l->lock_pc = 0xdead;
	}
#endif

	if (simple_locks_info[simple_locks_taken-1].l != l) {
		unsigned int i = simple_locks_taken;

		/* out-of-order unlocking */

		do
			if (i == 0)
				panic("simple_unlock");
		while (simple_locks_info[--i].l != l);

		simple_locks_info[i] = simple_locks_info[simple_locks_taken-1];
	}
	simple_locks_taken--;
}

#endif	MACH_SLOCKS && NCPUS == 1

/*
 *	Routine:	lock_init
 *	Function:
 *		Initialize a lock; required before use.
 *		Note that clients declare the "struct lock"
 *		variables and then initialize them, rather
 *		than getting a new one from this module.
 */
void lock_init(l, can_sleep)
	lock_t		l;
	boolean_t	can_sleep;
{
	bzero((char *)l, sizeof(lock_data_t));
	simple_lock_init(&l->interlock);
	l->want_write = FALSE;
	l->want_upgrade = FALSE;
	l->read_count = 0;
	l->can_sleep = can_sleep;
	l->thread = (char *)-1;		/* XXX */
	l->recursion_depth = 0;
}

void lock_sleepable(l, can_sleep)
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

void lock_write(l)
	register lock_t	l;
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return;
	}

	/*
	 *	Try to acquire the want_write bit.
	 */
	while (l->want_write) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->want_write)
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && l->want_write) {
			l->waiting = TRUE;
			thread_sleep((int) l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}
	l->want_write = TRUE;

	/* Wait for readers (and upgrades) to finish */

	while ((l->read_count != 0) || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && (l->read_count != 0 ||
					l->want_upgrade))
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && (l->read_count != 0 || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep((int) l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}
	simple_unlock(&l->interlock);
}

void lock_done(l)
	register lock_t	l;
{
	simple_lock(&l->interlock);

	if (l->read_count != 0)
		l->read_count--;
	else
	if (l->recursion_depth != 0)
		l->recursion_depth--;
	else
	if (l->want_upgrade)
	 	l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	/*
	 *	There is no reason to wakeup a waiting thread
	 *	if the read-count is non-zero.  Consider:
	 *		we must be dropping a read lock
	 *		threads are waiting only if one wants a write lock
	 *		if there are still readers, they can't proceed
	 */

	if (l->waiting && (l->read_count == 0)) {
		l->waiting = FALSE;
		thread_wakeup((int) l);
	}

	simple_unlock(&l->interlock);
}

void lock_read(l)
	register lock_t	l;
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->read_count++;
		simple_unlock(&l->interlock);
		return;
	}

	while (l->want_write || l->want_upgrade) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && (l->want_write || l->want_upgrade))
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && (l->want_write || l->want_upgrade)) {
			l->waiting = TRUE;
			thread_sleep((int) l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}

	l->read_count++;
	simple_unlock(&l->interlock);
}

/*
 *	Routine:	lock_read_to_write
 *	Function:
 *		Improves a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		no lock is held upon return.
 *
 *		Returns TRUE if the upgrade *failed*.
 */
boolean_t lock_read_to_write(l)
	register lock_t	l;
{
	register int	i;

	check_simple_locks();
	simple_lock(&l->interlock);

	l->read_count--;

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock.
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return(FALSE);
	}

	if (l->want_upgrade) {
		/*
		 *	Someone else has requested upgrade.
		 *	Since we've released a read lock, wake
		 *	him up.
		 */
		if (l->waiting && (l->read_count == 0)) {
			l->waiting = FALSE;
			thread_wakeup((int) l);
		}

		simple_unlock(&l->interlock);
		return (TRUE);
	}

	l->want_upgrade = TRUE;

	while (l->read_count != 0) {
		if ((i = lock_wait_time) > 0) {
			simple_unlock(&l->interlock);
			while (--i > 0 && l->read_count != 0)
				continue;
			simple_lock(&l->interlock);
		}

		if (l->can_sleep && l->read_count != 0) {
			l->waiting = TRUE;
			thread_sleep((int) l,
				simple_lock_addr(l->interlock), FALSE);
			simple_lock(&l->interlock);
		}
	}

	simple_unlock(&l->interlock);
	return (FALSE);
}

void lock_write_to_read(l)
	register lock_t	l;
{
	simple_lock(&l->interlock);

	l->read_count++;
	if (l->recursion_depth != 0)
		l->recursion_depth--;
	else
	if (l->want_upgrade)
		l->want_upgrade = FALSE;
	else
	 	l->want_write = FALSE;

	if (l->waiting) {
		l->waiting = FALSE;
		thread_wakeup((int) l);
	}

	simple_unlock(&l->interlock);
}


/*
 *	Routine:	lock_try_write
 *	Function:
 *		Tries to get a write lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t lock_try_write(l)
	register lock_t	l;
{

	simple_lock(&l->interlock);

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return(TRUE);
	}

	if (l->want_write || l->want_upgrade || l->read_count) {
		/*
		 *	Can't get lock.
		 */
		simple_unlock(&l->interlock);
		return(FALSE);
	}

	/*
	 *	Have lock.
	 */

	l->want_write = TRUE;
	simple_unlock(&l->interlock);
	return(TRUE);
}

/*
 *	Routine:	lock_try_read
 *	Function:
 *		Tries to get a read lock.
 *
 *		Returns FALSE if the lock is not held on return.
 */

boolean_t lock_try_read(l)
	register lock_t	l;
{
	simple_lock(&l->interlock);

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->read_count++;
		simple_unlock(&l->interlock);
		return(TRUE);
	}

	if (l->want_write || l->want_upgrade) {
		simple_unlock(&l->interlock);
		return(FALSE);
	}

	l->read_count++;
	simple_unlock(&l->interlock);
	return(TRUE);
}

/*
 *	Routine:	lock_try_read_to_write
 *	Function:
 *		Improves a read-only lock to one with
 *		write permission.  If another reader has
 *		already requested an upgrade to a write lock,
 *		the read lock is still held upon return.
 *
 *		Returns FALSE if the upgrade *failed*.
 */
boolean_t lock_try_read_to_write(l)
	register lock_t	l;
{
	check_simple_locks();
	simple_lock(&l->interlock);

	if (((thread_t)l->thread) == current_thread()) {
		/*
		 *	Recursive lock
		 */
		l->read_count--;
		l->recursion_depth++;
		simple_unlock(&l->interlock);
		return(TRUE);
	}

	if (l->want_upgrade) {
		simple_unlock(&l->interlock);
		return(FALSE);
	}
	l->want_upgrade = TRUE;
	l->read_count--;

	while (l->read_count != 0) {
		l->waiting = TRUE;
		thread_sleep((int) l,
			simple_lock_addr(l->interlock), FALSE);
		simple_lock(&l->interlock);
	}

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
	if (!l->want_write) {
		panic("lock_set_recursive: don't have write lock");
	}
	l->thread = (char *) current_thread();
	simple_unlock(&l->interlock);
}

/*
 *	Prevent a lock from being re-acquired.
 */
void lock_clear_recursive(l)
	lock_t		l;
{
	simple_lock(&l->interlock);
	if (((thread_t) l->thread) != current_thread()) {
		panic("lock_clear_recursive: wrong thread");
	}
	if (l->recursion_depth == 0)
		l->thread = (char *)-1;		/* XXX */
	simple_unlock(&l->interlock);
}

#if	MACH_KDB
#if	MACH_SLOCKS && NCPUS == 1
db_show_all_slocks()
{
	int i;
	struct simple_locks_info *info;
	simple_lock_t l;

	for (i = 0; i < simple_locks_taken; i++) {
		info = &simple_locks_info[i];
		db_printf("%d: ", i);
		db_printsym(info->l, DB_STGY_ANY);
#if i386 || i860
		db_printf(" locked by ");
		db_printsym(info->ra, DB_STGY_PROC);
#endif
		db_printf("\n");
	}
}
#else	MACH_SLOCKS && NCPUS == 1
db_show_all_slocks()
{
	db_printf("simple lock info not available\n");
}
#endif	MACH_SLOCKS && NCPUS == 1
#endif	MACH_KDB

#if	MACH_LDEBUG && NCPUS > 1
#include <kern/xpr.h>

int	lock_check;
simple_lock_t traced_lock;
unsigned lock_seq;

#if	i860
#undef	XPR
#define XPR(a,b) printf b
#define DEBUG_MASK 0x00000002
#define CPU_MASK   0x00000003
#define CPU_SHIFT  0
extern int get_fp();
#endif

#if	i386
#define	DEBUG_MASK 0x0f000000
#define CPU_MASK   0xf0000000
#define CPU_SHIFT  28
#endif

void
#ifdef	ASMP
simple_lock1(lock)
#else	ASMP
simple_lock(lock)
#endif	ASMP
	simple_lock_t lock;
{
	if ( lock == (simple_lock_t)0 )
		return;

	_simple_lock(lock);

#ifdef	NOT
	if (lock_check)
		if (lock->thread && lock->thread != (int)current_thread())
			Debugger();
#if	i860
	lock->lock_pc = ((*(((int *)get_fp())+1)) & ~CPU_MASK)
	  | (cpu_number() << CPU_SHIFT);
#endif	/* i860 */

#if	i386
	lock->lock_pc = ((*(((int *)&lock)-1)) & ~CPU_MASK)
	  | (cpu_number() << CPU_SHIFT);
#endif	/* i386 */

	lock->thread = (int)current_thread();
	lock->unlock_pc |= DEBUG_MASK;
#else	NOT
	lock->thread = (int)current_thread();
	lock->lock_pc = my_caller() - 8;
#endif	NOT
	if (traced_lock == lock) {
		XPR(0x10000000, ("seq %d, simple_lock @ %x\n", lock_seq,
				 lock->lock_pc));
		lock_seq++;
	}
}

void
#ifdef	ASMP
simple_unlock1(lock)
#else	ASMP
simple_unlock(lock)
#endif	ASMP
	simple_lock_t lock;
{
	assert (lock != (simple_lock_t)0);

/*	lock->lock_pc = 0  */
	lock->thread = (int) current_thread();
	lock->unlock_pc = my_caller() - 8;

#ifdef	NOT
	if ( lock == (simple_lock_t)0 )
		return;

	if (lock_check)
		if (lock->thread != (int)current_thread() ||
		    (lock->lock_pc & DEBUG_MASK))
			Debugger();
	lock->lock_pc |= DEBUG_MASK;
#if	i860
	lock->unlock_pc = ((*(((int *)get_fp())+1)) & ~CPU_MASK)
	  | (cpu_number() << CPU_SHIFT);
#endif	/* i860 */

#if	i386
	lock->unlock_pc = ((*(((int *)&lock)-1)) & ~CPU_MASK)
	  | (cpu_number() << CPU_SHIFT);
#endif	/* i386 */
	lock->thread |= DEBUG_MASK;
#endif	NOT
	if (traced_lock == lock) {
		XPR(0x10000000, ("seq %d simple_unlock @ %x\n", lock_seq,
				 lock->unlock_pc));
		lock_seq++;
	}
	_simple_unlock(lock);
}

boolean_t
#ifdef	ASMP
simple_lock_try1(lock)
#else	ASMP
simple_lock_try(lock)
#endif	ASMP
simple_lock_t lock;
{
	assert(lock != (simple_lock_t)0);

	if (_simple_lock_try(lock)) {
		lock->lock_pc = my_caller() - 8;
		lock->thread = (int)current_thread();
/*		lock->unlock_pc = 0; */
#ifdef NOT
		lock->lock_pc = ((*(((int *)&lock)-1)) & ~CPU_MASK)
		  | (cpu_number() << CPU_SHIFT);
		lock->thread = (int)current_thread();
		lock->unlock_pc |= DEBUG_MASK;
#endif NOT
		if (traced_lock == lock) {
			XPR(0x10000000, ("seq %d simple_lock_try @ %x\n",
					 lock_seq, lock->lock_pc));
			lock_seq++;
		}
		return(1);
	}
	return(0);
}

#endif  MACH_LDEBUG && NCPUS > 1


#if	NCPUS > 1 && MACH_MP_DEBUG

decl_simple_lock_data(extern , kdb_lock)
decl_simple_lock_data(extern , printf_lock)

int	max_lock_loops	= 1000000;

/*
 *	Arrange in the lock routines to call the following
 *	routines. This way, when locks are free there is no performance
 *	penalty
 */

void
retry_simple_lock(lock)
decl_simple_lock_data(, *lock)
{
	register count = 0;

	while(!simple_lock_try(lock))
		if (count++ > max_lock_loops && lock != &kdb_lock) {
			if (lock == &printf_lock)
				return;
			XPR(0x10000000, ("looping on %x\n", lock));
			db_printf("cpu %d looping on simple_lock(%x) called by %x\n",
				cpu_number(), lock, *(((int *)&lock) -1));
			Debugger();
			count = 0;
		}
}

void
retry_bit_lock(index, addr)
{
	register count = 0;

	while(!bit_lock_try(index, addr))
		if (count++ > max_lock_loops) {
			db_printf("cpu %d looping on bit_lock(%x, %x) called by %x\n",
				cpu_number(), index, addr, *(((int *)&index) -1));
			Debugger();
			count = 0;
		}
}
#endif	NCPUS > 1 && MACH_MP_DEBUG

#if	i860 || (MACH_SLOCKS && NCPUS == 1)

/*
 * For i860, define this C routine always -- as it was moved from i860/slock.s
 * For other architectures which presumably have this in assembly, define
 * here only for the perverted (MACH_SLOCKS && NCPUS ==1) case.
 */
#ifdef	simple_lock_init
#undef	simple_lock_init
#endif	simple_lock_init

void simple_lock_init( simple_lock_t l)
{
	l->lock_data = 0;
#if	MACH_LDEBUG
        l->lock_pc=0;
        l->unlock_pc=0;
        l->thread=0;
#endif	/* MACH_LDEBUG */
}
#endif	/* i860 || (MACH_SLOCKS && NCPUS == 1) */

