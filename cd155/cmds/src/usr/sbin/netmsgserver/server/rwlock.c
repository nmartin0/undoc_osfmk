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
 * rwlock.c
 *
 *
 */

#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: rwlock.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 03:12:05 $";
#endif

/*
 * Function for read/write lock module.
 */


#include	<stdio.h>
#include	<pthread.h>
#include <sys/errno.h>

#include	"netmsg.h"
#include	"nm_extra.h"
#include	"rwlock.h"

extern char	*malloc();

#if	VAX_FAST_LOCK
int	cthread_critical= 0;
int	swtch_cnt 	= 0;
int *	swtch_cnt_ptr 	= &swtch_cnt;
#endif	VAX_FAST_LOCK

/*
 * lk_alloc()
 *	Allocate a lock and initialize it.
 */

lock_t
lk_alloc()
BEGIN("lk_alloc")
	register lock_t	lock;

	if ((lock = (lock_t)malloc(sizeof(struct lock))) == (lock_t)0) {
	    panic("lk_alloc.malloc");
	}
	pthread_mutex_init(&lock->lk_mutex, pthread_mutexattr_default);
	pthread_cond_init(&lock->lk_condition, pthread_condattr_default);
	lock->lk_users = 0;
	lock->lk_permission = PERM_READ;

	RETURN(lock);
END

#if	X_LOCKS
#else	X_LOCKS

/*
 * lk_init()
 *	Initialize a lock.
 */

void
lk_init(lock)
register lock_t	lock;
BEGIN("lk_init")

	pthread_mutex_init(&lock->lk_mutex, pthread_mutexatr_default);
	pthread_cond_init(&lock->lk_condition, pthread_condattr_default);
	lock->lk_users = 0;
	lock->lk_permission = PERM_READ;
	RET;

END

#endif	X_LOCKS

/*
 * lk_free(lock)
 *	Free the read/write lock.
 */

void
lk_free(lock)
register lock_t	lock;
BEGIN("lk_free")

	pthread_cond_broadcast(&lock->lk_condition);
/*** noop (OSF)
	mutex_clear(&lock->lk_mutex);
**/
	free((char *)lock);
	RET;
END

#if	X_LOCKS
#else	X_LOCKS

/*
 * lk_clear(lock)
 *	Clear the read/write lock.
 */

void
lk_clear(lock)
register lock_t	lock;
BEGIN("lk_clear")

	pthread_cond_broadcast(&lock->lk_condition);
/**** noop (OSF)
	mutex_clear(&lock->lk_mutex);
***/
	RET;
END


/*
 * lk_lock(lock, perm, block)
 *	Get a lock of type lock.  If block is
 *	BLOCK, wait until the lock is gotten; if it's NOBLOCK, return
 *	0 if we can't get it.
 */

int
lk_lock(lock, perm, block)
register lock_t		lock;
register rw_perm_t	perm;
register rw_block_t	block;
BEGIN("lk_lock")
	if (lock == NULL)
		RETURN(0);

	pthread_mutex_lock(&lock->lk_mutex);

	if (lock->lk_permission == PERM_READ) {
		if (perm == PERM_READWRITE) {
			if (lock->lk_users == 0) {
				lock->lk_permission = PERM_READWRITE;
				lock->lk_users = 1;
				pthread_mutex_unlock(&lock->lk_mutex);

				RETURN(1);
			} else if (block == BLOCK) {
				while (lock->lk_users != 0)
					pthread_cond_wait(&lock->lk_condition, &lock->lk_mutex);

				lock->lk_users = 1;
				lock->lk_permission = PERM_READWRITE;
				pthread_mutex_unlock(&lock->lk_mutex);

				RETURN(1);
			}
		} else {
			++lock->lk_users;
			pthread_mutex_unlock(&lock->lk_mutex);

			RETURN(1);
		}
	} else if (block == BLOCK) {
		if (perm == PERM_READ) {
			while (lock->lk_permission == PERM_READWRITE)
				pthread_cond_wait(&lock->lk_condition, &lock->lk_mutex);

			++lock->lk_users;
			lock->lk_permission = PERM_READ;
			pthread_mutex_unlock(&lock->lk_mutex);

			RETURN(1);
		} else {
			while (lock->lk_users != 0)
				pthread_cond_wait(&lock->lk_condition, &lock->lk_mutex);

			lock->lk_users = 1;
			lock->lk_permission = PERM_READWRITE;
			pthread_mutex_unlock(&lock->lk_mutex);

			RETURN(1);
		}
	}

	pthread_mutex_unlock(&lock->lk_mutex);

	RETURN(0);
END

/*
 * lk_unlock(lock)
 *	Release the read or read/write lock.  Signal to the other
 *	threads that this lock has been released.
 */

void
lk_unlock(lock)
register lock_t	lock;
BEGIN("lk_unlock")
	pthread_mutex_lock(&lock->lk_mutex);

	if (lock->lk_permission == PERM_READWRITE) {
		lock->lk_permission = PERM_READ;
		lock->lk_users = 0;
	} else
		--lock->lk_users;

	pthread_cond_signal(&lock->lk_condition);
	pthread_mutex_unlock(&lock->lk_mutex);

	RET;
END

#endif	X_LOCKS
