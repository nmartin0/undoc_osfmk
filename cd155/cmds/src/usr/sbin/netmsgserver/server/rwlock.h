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
/* 
 * Mach Operating System
 * Copyright (c) 1989 Carnegie-Mellon University
 * Copyright (c) 1988 Carnegie-Mellon University
 * Copyright (c) 1987 Carnegie-Mellon University
 * All rights reserved.  The CMU software License Agreement specifies
 * the terms and conditions for use and redistribution.
 */
/*
 * OSF/1 Release 1.0
 */
/*
 * rwlock.h
 *
 *
 * $Header: /afs/ssd/i860/CVS/cmds_libs/src/usr/sbin/netmsgserver/server/rwlock.h,v 1.2 1994/11/19 03:12:06 mtm Exp $
 *
 */

/*
 * Definitions of structures for read/write locks
 */

#include "lock_queue.h"

#ifndef	_RWLOCK_
#define	_RWLOCK_

#ifdef	vax
#define	VAX_FAST_LOCK	1
#else	vax
#define	VAX_FAST_LOCK	0
#endif	vax

#include <pthread.h>

#if	VAX_FAST_LOCK
#include "vax_fast_lock.h"
#endif	VAX_FAST_LOCK

typedef enum {
	PERM_READ,
	PERM_READWRITE,
} rw_perm_t;

typedef enum {
	NOBLOCK = 0,
	BLOCK = 1,
} rw_block_t;

typedef struct lock {
	rw_perm_t		lk_permission;
	pthread_mutex_t		lk_mutex;
	struct condition	lk_condition;
	short			lk_users;
} *lock_t;

#define	X_LOCKS	1

#if	X_LOCKS

#define lk_init(lock) pthread_mutex_init(&(lock)->lk_mutex, pthread_mutexattr_default)

#define	lk_lock(lock, perm, block) pthread_mutex_lock(&(lock)->lk_mutex)

#define lk_unlock(lock) pthread_mutex_unlock(&(lock)->lk_mutex)

#define lk_clear(lock) /* should be noop pthread_mutex_clear(&(lock)->lk_mutex) */

#else	X_LOCKS

extern lock_t	lk_alloc();

extern void	lk_init();
/*
lock_t		lock;
*/

extern void	lk_free();
/*
lock_t		lock;
*/

extern void	lk_clear();
/*
lock_t		lock;
*/


extern int	lk_lock();
/*
lock_t		lock;
rw_perm_t	perm;
rw_block_t	block;
*/

extern void	lk_unlock();
/*
lock_t	lock;
*/

#endif	X_LOCKS

#endif	_RWLOCK_
