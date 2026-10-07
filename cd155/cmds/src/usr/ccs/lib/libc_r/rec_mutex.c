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
 * OSF/1 Release 1.0
 */

#include "lib_lock.h"
#include "rec_mutex.h"

#define	NULL	0

lib_lock_functions_t	libc_lock_funcs;

void
libc_declare_lock_functions(lib_lock_functions_t *f)
{
	libc_lock_funcs = *f;
	libc_locks_init();
}

rec_mutex_alloc(rec_mutex_t *m)
{
	extern char *malloc();

	*m = (rec_mutex_t) malloc(sizeof(struct rec_mutex));
	if (*m == NULL)
		return(-1);
	rec_mutex_init(*m);
	return(0);
}

void
rec_mutex_init(rec_mutex_t m)
{
	m->thread_id = NO_THREAD;
	m->count = 0;
	lib_mutex_create(libc_lock_funcs, &m->mutex);
}

void
rec_mutex_free(rec_mutex_t m)
{
	lib_mutex_delete(libc_lock_funcs, &m->mutex);
	free((char *) m);
}

int
rec_mutex_trylock(register rec_mutex_t m)
{
	register lib_threadid_t self;

	self = lib_thread_id(libc_lock_funcs);

	if (m->thread_id == self) {	/* If already holding lock */
		m->count += 1;
		return(1);
	}
	if (lib_mutex_trylock(libc_lock_funcs, &m->mutex)) {
		/* We got the lock */
		m->count = 1;
		m->thread_id = self;
		return(1);
	}
	return(0);
}

void
rec_mutex_lock(register rec_mutex_t m)
{
	register lib_threadid_t self;

	self = lib_thread_id(libc_lock_funcs);

	if (m->thread_id == self) {	/* If already holding lock */
		m->count += 1;
	} else {
		lib_mutex_lock(libc_lock_funcs, &m->mutex);
		m->count = 1;
		m->thread_id = self;
	}
}

void
rec_mutex_unlock(register rec_mutex_t m)
{
	register lib_threadid_t self;

	self = lib_thread_id(libc_lock_funcs);

	/* Must be holding lock to unlock! */
	if (m->thread_id == self) {
		if (--(m->count) == 0) {
			m->thread_id = NO_THREAD;
			lib_mutex_unlock(libc_lock_funcs, &m->mutex);
		}
	}
}
