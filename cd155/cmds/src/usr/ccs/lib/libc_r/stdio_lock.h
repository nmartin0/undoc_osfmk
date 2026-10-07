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
 * Thread safe locking extenstions for stdio.h
 *
 * OSF/1 Release 1.0
 */

#ifndef	_STDIO_LOCK_H
#define	_STDIO_LOCK_H

#include "rec_mutex.h"

typedef	rec_mutex_t	filelock_t;	/* pointer to rec_mutex struct */
extern	filelock_t	_flockfile();
extern	void		_funlockfile();
extern	filelock_t	_ftestfile();

#define	_funlockfile(filelock)	(((filelock) == (rec_mutex_t) NULL) ? 0 : \
					rec_mutex_unlock(filelock))

#define	_flockfile(iop)	\
		(filelock_t)(((iop)->_lock == (rec_mutex_t) NULL) ? NULL : \
			(rec_mutex_lock((iop)->_lock), (iop)->_lock))

#define	_ftestfile(iop)	\
		(filelock_t)(((iop)->_lock == (rec_mutex_t) NULL) ? NULL : \
			 rec_mutex_trylock((iop)->_lock), (iop)->_lock)

#endif /* _STDIO_LOCK_H */
