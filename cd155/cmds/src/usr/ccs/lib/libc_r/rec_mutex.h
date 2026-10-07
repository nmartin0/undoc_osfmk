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
 * rec_mutex.h
 *
 * Mutex locks which allow recursive calls by the same thread.
 */
/*
 * OSF/1 Release 1.0
 */

#ifndef _REC_MUTEX_H
#define _REC_MUTEX_H 1

/*
 * Recursive mutex definition.
 */

#include "lib_lock.h"

struct rec_mutex {
	lib_threadid_t	thread_id;	/* id of thread holding the lock */
	int		count;		/* Number of outstanding locks */
	lib_mutex_t	mutex;		/* The mutex itself */
};

typedef  struct rec_mutex	*rec_mutex_t;

/*
 * Recursive mutex operations.
 */

extern int		rec_mutex_alloc(rec_mutex_t *);
extern void		rec_mutex_init(rec_mutex_t);
extern void		rec_mutex_free(rec_mutex_t);
extern void		rec_mutex_lock(rec_mutex_t);
extern void		rec_mutex_unlock(rec_mutex_t);
extern int		rec_mutex_trylock(rec_mutex_t);

#endif _REC_MUTEX_H
