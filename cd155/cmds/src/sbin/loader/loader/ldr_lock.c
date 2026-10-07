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
 * ldr_lock.c
 *
 * loader locking functions.
 *
 * OSF/1 Release 1.0
 *
 */

#include <sys/types.h>
#include <sys/signal.h>
#include <loader.h>

#include <loader/ldr_main_types.h>
#include <loader/ldr_main.h>

#include "ldr_types.h"
#include "ldr_lock.h"
#include "ldr_errno.h"

static void ldr_initialize_locks();

static int	initialized = 0;
static sigset_t	sig_disable_mask;	/* mask to disable signals */
static sigset_t sig_mask;		/* saved signal mask */

void
ldr_declare_lock_functions(lib_lock_functions_t *funcs)
{
	if (funcs != NULL) {
		ldr_lock_funcs = *funcs;	/* make a copy of the switch */
		ldr_initialize_locks();
	}
}

/*
 * initialize all locks to be used by the loader.
 */
static void
ldr_initialize_locks()
{
	ldr_lock_init(&ldr_global_lock);
}

/*
 * diasble all asynchronout signals. This is necessary in the non-threads
 * situation because of a fear of longjmp().
 */
int
disable_all_signals()
{
	int	rc;

	if (!initialized) {
		sigemptyset(&sig_disable_mask);
		sigaddset(&sig_disable_mask, SIGINT);
		sigaddset(&sig_disable_mask, SIGTERM);
		sigaddset(&sig_disable_mask, SIGALRM);
		sigaddset(&sig_disable_mask, SIGCHLD);
		sigaddset(&sig_disable_mask, SIGHUP);
		sigaddset(&sig_disable_mask, SIGPIPE);
		sigaddset(&sig_disable_mask, SIGQUIT);
		sigaddset(&sig_disable_mask, SIGTSTP);
		sigaddset(&sig_disable_mask, SIGUSR1);
		sigaddset(&sig_disable_mask, SIGUSR2);
		initialized = 1;
	}

	sigemptyset(&sig_mask);
	if (sigprocmask(SIG_SETMASK, &sig_disable_mask, &sig_mask) != 0) {
		rc = ldr_errno_to_status(errno);
		ldr_log("disable_all_signals: sigprocmask error %E\n", rc);
		return(rc);
	}

	return LDR_SUCCESS;
}

/*
 * reinstates the signal mask to the way it was before 
 * disable_all_signals was called.
 */
int
enable_all_signals()
{
	int	rc;

	if (sigprocmask(SIG_SETMASK, &sig_mask, NULL) != 0) {
		rc = ldr_errno_to_status(errno);
		ldr_log("enable_all_signals: sigprocmask error %E\n", rc);
		return(rc);
	}

	return LDR_SUCCESS;
}
