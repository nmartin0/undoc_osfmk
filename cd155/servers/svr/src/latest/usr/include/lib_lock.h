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
 * HISTORY
 * $Log: lib_lock.h,v $
 * Revision 1.3  1994/11/18  21:09:57  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:14:48  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:57:48  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:16:15  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:18:57  nandy
 * Initial revision
 *
 * Revision 1.3  92/04/13  14:36:09  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.3  90/10/07  20:28:33  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:47:02  gm]
 * 
 * Revision 1.2  90/09/23  16:42:59  devrcs
 * 	Move to /usr/include
 * 	[90/09/19  15:17:03  lwa]
 * 
 * $EndLog$
 */
/*
 * Library locking functions provided by a threads package usable by a library
 */

#ifndef _LIB_LOCK_H_
#define _LIB_LOCK_H_

typedef	void	*lib_mutex_t;
typedef	void	*lib_spinlock_t;
typedef void	*lib_threadid_t;

typedef	int		(*lib_mutex_func_t)(lib_mutex_t *);
typedef	int		(*lib_spinlock_func_t)(lib_spinlock_t *);
typedef	lib_threadid_t	(*lib_threadid_func_t)(void);

typedef struct lib_lock_functions {
	lib_mutex_func_t	mutex_create;
	lib_mutex_func_t	mutex_delete;
	lib_mutex_func_t	mutex_lock;
	lib_mutex_func_t	mutex_unlock;
	lib_mutex_func_t	mutex_trylock;
	lib_spinlock_func_t	spinlock_create;
	lib_spinlock_func_t	spinlock_delete;
	lib_spinlock_func_t	spinlock_lock;
	lib_spinlock_func_t	spinlock_unlock;
	lib_spinlock_func_t	spinlock_trylock;
	lib_threadid_func_t	thread_id;
} lib_lock_functions_t;

#ifndef ESUCCESS
#define ESUCCESS	0
#endif

#ifndef NO_THREAD
#define NO_THREAD	(lib_threadid_t)0
#endif

#define	LIB_LOCK_FUNCTION(lockstruct, operation, arg) \
	((lockstruct).operation ? (*(lockstruct).operation)(arg) : ESUCCESS)

#define lib_mutex_create(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, mutex_create, lock)

#define lib_mutex_delete(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, mutex_delete, lock)

#define lib_mutex_lock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, mutex_lock, lock)

#define lib_mutex_unlock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, mutex_unlock, lock)

#define lib_mutex_trylock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, mutex_trylock, lock)

#define lib_spinlock_create(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, spinlock_create, lock)

#define lib_spinlock_delete(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, spinlock_delete, lock)

#define lib_spinlock_lock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, spinlock_lock, lock)

#define lib_spinlock_unlock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, spinlock_unlock, lock)

#define lib_spinlock_trylock(lockstruct, lock) \
		LIB_LOCK_FUNCTION(lockstruct, spinlock_trylock, lock)

#define lib_thread_id(lockstruct) \
	((lockstruct).thread_id ? (*(lockstruct).thread_id)() : 0)

#endif /* _LIB_LOCK_H_ */
