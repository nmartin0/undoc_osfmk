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
 * Copyright (c) 1992 Carnegie Mellon University
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
 * $Id: lock.h,v 1.5 1995/03/14 08:03:52 lenb Exp $
 */

#include <mach_mp_debug.h>
#include <mach_lock_mon.h>

/*
 *	Machine-dependent simple locks for the i860. see i860/slock.s
 */
#ifndef	_I860_LOCK_H_
#define	_I860_LOCK_H_

/*
 * Included by kern/lock.h when MACH_SLOCKS is defined --
 * which is either NCPUS > 1, or (NCPUS == 1 and MACH_LDEBUG)
 */

/*
 * These CAPITAL lock primitives are valid for either ASMP or SMP
 */
#define	SIMPLE_LOCK(l)		simple_lock1(l)
#define	SIMPLE_UNLOCK(l)	(l)->lock_data = 0
#define	SIMPLE_LOCK_TRY(l)	simple_lock_try1(l)
#define	SIMPLE_LOCK_TAKEN(lock)		((lock)->lock_data)
#define	SIMPLE_LOCK_NOT_TAKEN(lock)	(!(SIMPLE_LOCK_TAKEN(lock))

extern void             simple_lock1(simple_lock_t);
extern boolean_t        simple_lock_try1(simple_lock_t);

/*
 * If ASMP is defined, then we blow away all
 * but the above capital lock primitives.
 */
#ifdef	ASMP
extern void             simple_lock_init(simple_lock_t);
#define	simple_lock(l)
#define	simple_unlock(l)
#define	simple_lock_try(l)	(1)	/* always succeeds */
#define	simple_lock_taken(l)	(1)	/* always succeeds */
#else	ASMP
extern void             simple_lock_init(simple_lock_t);
extern void             simple_lock(simple_lock_t);
#define	simple_unlock(l)	(l)->lock_data = 0
extern boolean_t        simple_lock_try(simple_lock_t);
#define	simple_lock_taken(lock)		((lock)->lock_data)
#define	simple_lock_not_taken(lock)	(!(simple_lock_taken(lock))
#endif	ASMP

/*
 * in all cases
 */
#define	simple_lock_pause()
#define	check_simple_locks()

#endif	/* _I860_LOCK_H_ */






