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
/*
 *  This file contains the declarations of all locks used in libc.
 */

#include "rec_mutex.h"

/* Locks for stdio.  These are the iob locks */
struct rec_mutex _findiop_rmutex;
struct rec_mutex _stdio_buf_rmutex[3];

/* Locks for compat-4.1 */
struct rec_mutex _rand_rmutex;

/* Locks for compat-sys5 */
struct  rec_mutex _clock_rmutex;
struct  rec_mutex _tmpnam_rmutex;
struct 	rec_mutex _tempnam_rmutex;

/* Locks for gen */
struct	rec_mutex _ctime_rmutex;
struct	rec_mutex _fstab_rmutex;
struct	rec_mutex _getttyent_rmutex;
struct	rec_mutex _getusershell_rmutex;
struct  rec_mutex _group_rmutex;
struct	rec_mutex _utmp_rmutex;
struct  rec_mutex _passwd_rmutex;
struct  rec_mutex _malloc_rmutex;
struct  rec_mutex _free_rmutex;
struct	rec_mutex _random_rmutex;
struct	rec_mutex _environ_rmutex;

/* Locks for n16/gen */
struct	rec_mutex _abort_rmutex;

/* locks for internationalization */
struct rec_mutex _locale_rmutex;
struct rec_mutex _catalog_rmutex;

struct rec_mutex _exec_rmutex;
struct rec_mutex _exit_rmutex;
struct rec_mutex _nice_rmutex;
struct rec_mutex _alarm_rmutex;

struct rec_mutex _nanotimer_rmutex;

void
libc_locks_init()
{
	/* Locks for stdio.  These are the iob locks */
	rec_mutex_init(&_findiop_rmutex);
	rec_mutex_init(&_stdio_buf_rmutex[0]);
	rec_mutex_init(&_stdio_buf_rmutex[1]);
	rec_mutex_init(&_stdio_buf_rmutex[2]);

	/* Locks for compat-4.1 */
	rec_mutex_init(&_rand_rmutex);

	/* Locks for compat-sys5 */
	rec_mutex_init(&_clock_rmutex);
	rec_mutex_init(&_tmpnam_rmutex);
	rec_mutex_init(&_tempnam_rmutex);

	/* Locks for gen */
	rec_mutex_init(&_ctime_rmutex);
	rec_mutex_init(&_fstab_rmutex);
	rec_mutex_init(&_getttyent_rmutex);
	rec_mutex_init(&_getusershell_rmutex);
	rec_mutex_init(&_group_rmutex);
	rec_mutex_init(&_utmp_rmutex);
	rec_mutex_init(&_passwd_rmutex);
	rec_mutex_init(&_malloc_rmutex);
	rec_mutex_init(&_free_rmutex);
	rec_mutex_init(&_random_rmutex);
	rec_mutex_init(&_environ_rmutex);

	/* Locks for n16/gen */
	rec_mutex_init(&_abort_rmutex);

	/* lock for interationalization */
	rec_mutex_init(&_locale_rmutex);
	rec_mutex_init(&_catalog_rmutex);

	rec_mutex_init(&_exec_rmutex);
	rec_mutex_init(&_exit_rmutex);
	rec_mutex_init(&_nice_rmutex);
	rec_mutex_init(&_alarm_rmutex);

	rec_mutex_init(&_nanotimer_rmutex);
}
