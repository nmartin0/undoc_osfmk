/*
 * 
 * $Copyright
 * Copyright 1993, 1994, 1995  Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * (c) Copyright 1990, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/* ldr_lock.h
 * Lock management routines for loader
 *
 * OSF/1 Release 1.0
 */

#ifndef	_H_LDR_LOCK
#define	_H_LDR_LOCK

#include "lib_lock.h"	/* ugly - as this lives in /usr/ccs/lib/libc_r */

/* locking functions for loader's use */
extern lib_lock_functions_t	ldr_lock_funcs;
extern int disable_all_signals __((void));
extern int enable_all_signals __((void));

typedef	univ_t	ldr_lock_t;		/* a lock */

/* the loader uses a single global lock */
extern ldr_lock_t ldr_global_lock;

#define LDR_LOCK_FUNCTION(operation, argument) \
	(ldr_lock_funcs.operation ? \
	 (*ldr_lock_funcs.operation)((lib_mutex_t *)argument) : LDR_SUCCESS)

/* Initialize a lock.  Must be called before the lock is used.
 *	extern void ldr_lock_init(ldr_lock_t *lck);
 */

#define ldr_lock_init(lck) LDR_LOCK_FUNCTION(mutex_create, (lck))


/* Acquire a lock, sleeping until the lock is available.
 *	extern void ldr_lock(ldr_lock_t *lck);
 */

#define ldr_lock(lck) \
	(ldr_lock_funcs.mutex_lock ? \
	 (*ldr_lock_funcs.mutex_lock)((lib_mutex_t *)lck) : disable_all_signals())

/* Release a lock, awakening any thread waiting for the lock.
 *	extern void ldr_unlock(ldr_lock_t *lck);
 */
#define ldr_unlock(lck) \
	(ldr_lock_funcs.mutex_unlock ? \
	 (*ldr_lock_funcs.mutex_unlock)((lib_mutex_t *)lck) : enable_all_signals())

/* Return 0 if lock is currently held, <> 0 otherwise.
 *	extern void ldr_lock_test(ldr_lock_t *lck);
 */

#define	ldr_lock_test(lck) LDR_LOCK_FUNCTION(mutex_trylock, (lck))


/* Terminate use of a lock.  Should be called before any data structure
 * containing the lock is freed.
 *	extern void ldr_lock_term(ldr_lock_t *lck);
 */

#define	ldr_lock_term(lck) LDR_LOCK_FUNCTION(mutex_delete, (lck))

#endif	/* _H_LDR_LOCK */
