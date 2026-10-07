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
 * $Log: lock.h,v $
 * Revision 1.8  1994/11/18  20:32:38  mtm
 * Copyright additions/changes
 *
 * Revision 1.7  1993/07/14  18:01:36  cfj
 * OSF/1 AD 1.0.4 code drop from Locus.
 *
 * Revision 1.1.1.3  1993/07/01  19:20:42  cfj
 * Adding new code from vendor
 *
 * Revision 1.6  1993/05/06  19:16:38  cfj
 * ad103+tnc merged with Intel code.
 *
 * Revision 1.1.1.1  1993/05/03  17:30:38  cfj
 * Initial 1.0.3 code drop
 *
 * Revision 1.5  1993/03/29  23:02:40  nandy
 * Merged from T9 branch.
 *
 * Revision 1.2.8.3  1993/03/29  22:44:28  nandy
 * Locking code from Michael Condict
 *
 * Revision 2.11  1993/04/29  14:00:50  klh
 * 	Revision 2.13  93/03/26  17:34:10  durriya
 * 		change lock_count in the lock struct to be a short rather than a
 * 		     bitfield int - the 860 compiler cannot handle signed bitfields.
 * 		[93/03/26            durriya]
 *
 * 	Revision 2.11  93/03/22  23:56:01  condict
 * 		Extensive changes to the lock struct and the locking functions for
 * 		performance, correctness and readability.  See "1993 Lock Rewrite",
 * 		in kern/lock.c.
 *
 * 	Revision 2.10  92/12/08  10:43:48  durriya
 * 		1.1 unmount sync changes - added lock_try_read_assert (durriya)
 *
 * Revision 2.10  93/03/22  21:11:09  yazz
 * OSF lock changes.  New fields added to make queue of waiting wannabe
 * lock-holders.
 * 
 * Revision 2.9  92/05/24  14:33:19  pjg
 * 	Revision 3.5  92/03/23  18:03:24  condict
 * 	Allow NCPUS == 1 in the server, to compile optimally for a uni-processor.
 * 
 * Revision 2.8  92/05/12  00:08:01  loverso
 * 	Redefined all the blocking lock routines as macros. If MACH_LDEBUG or
 * 	MACH_LTRACKS is defined, they pass as extra parameter the current
 * 	program counter to be stored in the lock structure (pjg).
 * 
 * Revision 2.7  92/04/05  16:56:16  pjg
 * 	Re-enabled option MACH_LDEBUG for OSF1_AD.
 * 
 * Revision 2.6  92/03/15  14:44:07  roy
 * 	Changes for non-gnu case (rabii).
 * 
 * Revision 2.5  92/03/09  14:36:48  durriya
 * 	[Revision 3.4  92/01/07  23:33:57  condict]
 * 	Change def of simple locks from mutexes to spin locks, for performance.
 * 
 * Revision 2.4  91/12/16  20:47:32  roy
 * 	91/10/21  18:47:37  emcmanus
 * 	Fixes to compile with asserts.
 * 
 * Revision 2.3  91/10/14  12:33:52  sjs
 * 	91/09/20  19:16:06  barbou
 * 	Lock declaration always use the same amount of memory (to avoid having 
 * 	different structures).
 * 
 * Revision 2.2  91/08/31  13:37:44  rabii
 * 	Initial V2.0 Checkin
 * 
 * Revision 3.1  91/08/07  17:00:10  sp
 * Upgrade to 1.0.2
 * 
 * Revision 1.6  90/10/07  13:53:57  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  09:55:00  gm]
 * 
 * 	Track lock counts.
 * 	[90/09/28  12:37:23  nags]
 * 
 * Revision 1.5  90/08/24  12:03:36  devrcs
 * 	Allow _simple_unlock and _simple_lock_init to be machine defined.
 * 	[90/08/15  17:09:01  brezak]
 * 
 * 	Add fast inline functions when building with GNU-C.
 * 	[90/08/11  11:11:34  brezak]
 * 
 * Revision 1.4  90/06/22  20:14:22  devrcs
 * 	nags merge
 * 
 * 	Compressed history (reverse chronology):
 * 	Lock debugging, statistics, tracking for OSF/1.	nags@encore.com
 * 	Fixes for first snapshot.			gm@osf.org
 * 	Mach 2.5 and Encore 0.6 merge			gm@osf.org
 * 	Enable tracking of simple lock owner.		sue@encore.com
 * 	Merge Encore parallelization with Release 2.5.	alan@encore.com
 * 	Added lock_nreads field.			boykin@encore.com
 * 	More cleanup.					rpd@cmu.edu
 * 	Changes for cleanup.				gm0w@cmu.edu
 * 	Added lthread field.				boykin@encore.com
 * 	Relocated from sys/lock.h.			mwyoung@cmu.edu
 * 	Added decl_simple_lock_data macros.		rpd@cmu.edu
 * 	Merged multiple lock definitions back to one.	alan@encore.com
 * 	Distinguish recursive locks when debugging.	alan@encore.com
 * 	Adjusted include file references.		mwyoung@cmu.edu
 * 	Check simple lock sanity on uniprocessors.	rpd@cmu.edu
 * 	Simple lock statistics.				alan@encore.com
 * 	Added lock debugging fields.			alan@encore.com
 * 	Use optimized lock structure for multimax also.	dlb@cmu.edu
 * 	Use optimized lock "structure" for balance.	rvb@cmu.edu
 * 	Invert logic of no_sleep to can_sleep.		avie@cmu.edu
 * 	Revamped lock structure.			dbg@cmu.edu
 * 	Added "unsigned" to fields in vax case (lint).	mwyoung@cmu.edu
 * 	Added fields for sleep/recursive locks.		mwyoung@cmu.edu
 * 	Merged Multimax changes.			dlb@cmu.edu
 * 	Removed reference to "caddr_t" from BUSYV/P.	mwyoung@cmu.edu
 * 	Changed to directly import boolean declaration.	mwyoung@cmu.edu
 * 	Added simple_lock_try, sleep locks, recursion.	dbg@cmu.edu
 * 	Removed ';' from locking macros definitions.	bolosky@cmu.edu
 * 	Defined adawi to be add when not on a vax.	bolosky@cmu.edu
 * 	Overhauled from previous version.		mwyoung@cmu.edu
 * 	[90/06/12  21:27:30  gmf]
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
 *	File:	kern/lock.h
 *	Author:	Avadis Tevanian, Jr., Michael Wayne Young
 *	Copyright (C) 1985, Avadis Tevanian, Jr., Michael Wayne Young
 *
 *	Locking primitives definitions
 *
 */

#ifndef	_KERN_LOCK_H_
#define _KERN_LOCK_H_

#ifdef  KERNEL
#include <cpus.h>
#include <mach_ldebug.h>
#include <mach_ltracks.h>
#include <lock_stats.h>
#include <slock_stats.h>
#endif  KERNEL

#include <mach/boolean.h>

#if	MACH_LDEBUG
#include <kern/assert.h>
#endif

#include <machine/inline_lock.h>

#define MACH_SLOCKS	((NCPUS > 1) || MACH_LDEBUG)
#ifdef	OSF1_SERVER
#include <uxkern/import_mach.h>
#endif	OSF1_SERVER

/*
 *	A simple spin lock.
 */

struct slock {
#ifndef	OSF1_SERVER
	int		lock_data;	/* in general 1 bit is sufficient */
#else	OSF1_SERVER
	spin_lock_t	lock_data;
#endif	OSF1_SERVER
#if	MACH_LDEBUG
	char		*slthread;	/* thread of locker */
	int		slck_addr;	/* pc where locked */
	int		sunlck_addr;	/* pc of last unlocker */
#endif
#if	SLOCK_STATS
	unsigned long	slock_tries;	/* attempts at taking lock */
	unsigned long	slock_fails;	/* misses on lock */
	unsigned long	slock_min_time;	/* minimum time lock held */
	unsigned long	slock_max_time;	/* longest time lock held */
	unsigned long	slock_avg_time;	/* average time lock held */
#endif
};

typedef struct slock	simple_lock_data_t;
typedef struct slock	*simple_lock_t;

#if	MACH_SLOCKS
extern void		simple_lock_init();
extern void		simple_lock();
extern void		simple_unlock();
extern boolean_t	simple_lock_try();
extern void		_simple_lock_solid();
extern int		check_locks;	/* don't check until current_thread works */

#define decl_simple_lock_data(class,name)	class simple_lock_data_t name;
#define simple_lock_addr(lock)		(&(lock))
#else	/* MACH_SLOCKS */
/*
 *	No multiprocessor locking is necessary.
 */
#define simple_lock_init(l)
#define simple_lock(l)
#define simple_unlock(l)
#define simple_lock_try(l)	(1)	/* always succeeds */

#define decl_simple_lock_data(class,name)	class simple_lock_data_t name;
#define simple_lock_addr(lock)		((simple_lock_t)0)
#endif	/* MACH_SLOCKS */

#include <kern/queue.h>
/*
 *	The general lock structure.  Provides for multiple readers,
 *	upgrading from read to write, and sleeping until the lock
 *	can be gained.
 *
 *	On some (many) architectures, assembly language code in the inline
 *	program fiddles the lock structures.  It must be changed in concert
 *	with the structure layout.
 */

struct lock {
#ifdef	vax
	/*
	 *	Efficient VAX implementation -- see field description below.
	 */
	int		lock_count:16;
	unsigned int	want_upgrade:1,
			waiting_readers:1,
			can_sleep:1,
			:0;
#else	/* vax */
#ifdef	ns32000
	/*
	 *	Efficient ns32000 implementation --
	 *	see field description below.
	 */
	union {
		struct {
			char	l_s_byte;	/* lock byte */
			char	l_s_unused1;
			char	l_s_unused2;
			char	l_s_type;	/* identity */
		} l_s;
		decl_simple_lock_data(,Interlock)
	} l_un;
	int		lock_count:16;
	unsigned int	want_upgrade:1,
			waiting_readers:1,
			can_sleep:1,
			:0;
#else	/* ns32000 */
	/*	Only the "interlock" field is used for hardware exclusion;
	 *	other fields are modified with normal instructions after
	 *	acquiring the interlock bit.
	 */
	boolean_t	want_upgrade:8;	/* Read-to-write upgrade waiting */
	boolean_t	waiting_readers:8;/* Threads sleeping; want read */
	boolean_t	can_sleep:8;	/* Can attempts to lock go to sleep */
	short		lock_count;	/* > 0 : Number of accepted readers;
					   < 0 : Number of recurs write locks */
#endif	/* ns32000 */
#endif	/* vax */
	queue_head_t	waiting_writers;/* Sleeping threads that want write */
	char		*thread;
		/* Thread that has lock, if recursive locking allowed */
		/* (Not thread_t to avoid recursive definitions.) */

#if	!defined(ns32000)
	/*	Put this field last in the structures, so that field
	 *	offsets are constant regardless of whether this is present.
	 *	This makes any assembly language code simpler.
	 */
	decl_simple_lock_data(,interlock)
#endif
#if	MACH_LDEBUG || MACH_LTRACKS
	char		*lthread;	/* Thread of locker */
	int		lck_addr;	/* pc where locked */
	int		unlck_addr;	/* pc of last unlocker */
#endif
#if	LOCK_STATS
	unsigned long	lock_tries;	/* number of attempts at taking lock */
	unsigned long	lock_fails;	/* misses on lock */
	unsigned long	lock_sleeps;	/* actual thread blocks on lock */
	unsigned long	lock_wait_min;	/* shortest time blocked on lock */
	unsigned long	lock_wait_max;	/* longest time blocked on lock */
	unsigned long	lock_wait_sum;	/* total time blocked on lock */
	unsigned long	lock_max_read;	/* maximum active readers */
	unsigned long	lock_nreads;	/* Number of read requests */
#endif
};

#ifdef	ns32000
#define	interlock	l_un.Interlock
#define	lock_type	l_un.l_s.l_s_type
#endif

typedef struct lock	lock_data_t;
typedef struct lock	*lock_t;


#if	MACH_LDEBUG
#define	MAX_LOCK	10

#define	LOCK_READERS(l)		((l)->lock_count > 0)
#define	LOCK_LOCKED(l)		((l)->lock_count < 0)
#ifndef OSF1_SERVER
#define	LOCK_THREAD(l)		((thread_t) (l)->lthread)
#else
#define	LOCK_THREAD(l)		((struct uthread *) (l)->lthread)
#endif	/* OSF1_SERVER */
#define	LOCK_OWNER(l)		(LOCK_THREAD(l) == current_thread())
#define	LOCK_HOLDER(l)		(!LOCK_READERS(l) && LOCK_LOCKED(l) && \
				 LOCK_OWNER(l))
#ifdef	ns32000
#define	SLOCK_LOCKED(l)		(((l)->lock_data & 0xff) != 0)
#else
#define	SLOCK_LOCKED(l)		((l)->lock_data != 0)
#endif  /* ns32000 */
#ifndef	OSF1_SERVER
#define	SLOCK_THREAD(l)		((thread_t) (l)->slthread)
#else
#define	SLOCK_THREAD(l)		((struct uthread *) (l)->slthread)
#endif	/* OSF1_SERVER */
#define	SLOCK_OWNER(l)		(SLOCK_THREAD(l) == current_thread())
#define	SLOCK_HOLDER(l)		(SLOCK_LOCKED(l) && SLOCK_OWNER(l))
#define	LASSERT(clause)		if (check_locks) ASSERT(clause)

struct slock_debug {
	int count;
	int addr[MAX_LOCK];
};
extern	int	check_locks;
extern	int	check_lock_counts;
extern	struct slock_debug slck_dbg[];

#else	/* MACH_LDEBUG */

#define	LOCK_READERS(l)
#define	LOCK_LOCKED(l)
#define	LOCK_THREAD(l)
#define	LOCK_OWNER(l)
#define	LOCK_HOLDER(l)
#define	SLOCK_LOCKED(l)
#define	SLOCK_THREAD(l)
#define	SLOCK_OWNER(l)
#define	SLOCK_HOLDER(l)
#define	LASSERT(clause)

#endif	/* MACH_LDEBUG */

/* Sleep locks must work even if no multiprocessing */

extern void		do_lock_init();
extern void		do_lock_sleepable();
extern void		do_lock_write();
extern void		do_lock_read();
extern void		do_lock_done();
extern boolean_t	do_lock_read_to_write();
extern void		do_lock_write_to_read();
extern boolean_t	do_lock_try_write();
extern boolean_t	do_lock_try_read();
extern boolean_t	do_lock_try_read_assert();
extern boolean_t	do_lock_try_read_to_write();

#if	MACH_LDEBUG || MACH_LTRACKS
#define lock_init(l, f)		do_lock_init(l, f)
#define lock_speepable(l,f)	do_lock_sleepable(l, f)
#define lock_write(l)		do_lock_write(l, current_pc())
#define lock_read(l)		do_lock_read(l, current_pc())
#define lock_done(l)		do_lock_done(l, current_pc())
#define lock_read_to_write(l)	do_lock_read_to_write(l)
#define lock_write_to_read(l)	do_lock_write_to_read(l)
#define lock_try_write(l)	do_lock_try_write(l, current_pc())
#define lock_try_read(l)	do_lock_try_read(l, current_pc())
#define lock_try_read_assert(l)	do_lock_try_read_assert(l, current_pc())
#define lock_try_read_to_write(l) do_lock_try_read_to_write(l, current_pc())

#else

#define lock_init(l, f)		do_lock_init(l, f)
#define lock_speepable(l,f)	do_lock_sleepable(l, f)
#define lock_write(l)		do_lock_write(l)
#define lock_read(l)		do_lock_read(l)
#define lock_done(l)		do_lock_done(l)
#define lock_read_to_write(l)	do_lock_read_to_write(l)
#define lock_write_to_read(l)	do_lock_write_to_read(l)
#define lock_try_write(l)	do_lock_try_write(l)
#define lock_try_read(l)	do_lock_try_read(l)
#define lock_try_read_assert(l)	do_lock_try_read_assert(l)
#define lock_try_read_to_write(l) do_lock_try_read_to_write(l)
#endif	/* MACH_LDEBUG || MACH_LTRACKS */

#define lock_read_done(l)	lock_done(l)
#define lock_write_done(l)	lock_done(l)

extern void		lock_set_recursive();
extern void		lock_clear_recursive();


#ifdef OSF1_SERVER
#undef _NO_INLINE_LOCKS
#endif

#if	__GNUC__ && MACH_SLOCKS && !_NO_INLINE_LOCKS
/*
 * Define _simple_lock and _simple_lock_try in the machine
 * dependant inline_lock.h file if they need something more
 * complicated than the simple Uniprocessor locks below. If you
 * redefine these be sure to define __SLOCK so that the default
 * versions are not used.
 *
 * Also to support the debugging lock macros you should define a
 * function, current_pc, that returns the current location to be
 * used in recording the last locker.
 */

#if	!__SLOCKS && (NCPUS == 1)

extern __inline__ _simple_lock_init(l)
        int *l;
{
        *(l) = 0;
}

extern __inline__ _simple_lock(l)
        int *l;
{
        LASSERT(*(l) == 0);
        *(l) = 1;
}

extern __inline__ _simple_lock_try(l)
        int *l;
{
        LASSERT(*(l) == 0);
        *(l) = 1;
        return(1);
}

extern __inline__ _simple_unlock(l)
        int *l;
{
        LASSERT(*(l) != 0);
        *(l) = 0;
}

#endif	/* !__SLOCKS && (NCPUS == 1) */

#define SIMPLE_LOCK_DEBUG(l) \
        l->slthread = (char *) current_thread(); \
	l->slck_addr = (int) current_pc(); \
	l->sunlck_addr = (int) -1; \
        inc_slock(l);

#define SIMPLE_UNLOCK_DEBUG(l) \
        l->slthread = (char *) -1; \
	l->slck_addr = (int) -1; \
	l->sunlck_addr = (int) current_pc(); \
        dec_slock(l);

#ifdef	OSF1_SERVER
#define	_simple_lock_init(l)	spin_lock_init((l))
#define	_simple_lock(l)		spin_lock((l))
#define	_simple_unlock(l)	spin_unlock((l))
#define	_simple_lock_try(l)	spin_try_lock((l))

#ifdef	KERNEL
/*
 * This is called by spin_lock.  Can't use the C-threads version, since it
 * doesn't try to schedule another runnable C-thread, resulting in deadlock:
 */
#define	spin_lock_solid		_simple_lock_solid
#endif

/* Since user.h includes this file before #defining current_thread, we don't
   have the macro in the inline functions below, so we must declare the
   function with the correct type.  */
#ifndef current_thread
extern struct uthread *current_thread();
#endif
#endif	/* OSF1_SERVER */

extern void __inline__ simple_lock_init(l)
	simple_lock_t	l;
{
#if	MACH_LDEBUG
	l->slthread = (char *) -1;
        l->slck_addr = (int) -1;
        l->sunlck_addr = (int) -1;
#endif
        _simple_lock_init(&l->lock_data);
}

extern void __inline__ simple_lock(l)
	simple_lock_t l;
{
        _simple_lock(&l->lock_data);
#if	MACH_LDEBUG
        SIMPLE_LOCK_DEBUG(l);
#endif
}

extern boolean_t __inline__ simple_lock_try(l)
	simple_lock_t l;
{
#if	MACH_LDEBUG
        register int state = _simple_lock_try(&l->lock_data);
        if (state)
                SIMPLE_LOCK_DEBUG(l);
        return(state);
#else
        return(_simple_lock_try(&l->lock_data));
#endif
}

extern void __inline__ simple_unlock(l)
	simple_lock_t l;
{
#if	MACH_LDEBUG
        SIMPLE_UNLOCK_DEBUG(l);
#endif
        _simple_unlock(&l->lock_data);
}

#else  /* __GNUC__ && MACH_SLOCKS && !_NO_INLINE_LOCKS */

#ifdef	OSF1_SERVER
#define	_simple_lock_init(l)	spin_lock_init((l))
#define	_simple_lock(l)		spin_lock((l))
#define	_simple_unlock(l)	spin_unlock((l))
#define	_simple_lock_try(l)	spin_try_lock((l))
#endif	/* OSF1_SERVER */

#endif  /* __GNUC__ && MACH_SLOCKS && !_NO_INLINE_LOCKS */
#endif	/* _KERN_LOCK_H_ */
