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
/* sigprocmask -- library stub to transform calling sequence */

/*
 * user program calls:
 *     success = sigprocmask(int how, sigset_t *imask, sigset_t *omask);
 *
 * kernel-side interface:
 *     sigset_t omask = sigprocmask(int how, sigset_t imask);
 *
 */

#include <sys/signal.h>
#include <sys/param.h>
#include <sys/errno.h>

extern sigset_t _sigprocmask();

int
sigprocmask(how, imask, omask)
int 		how;
sigset_t	*imask, *omask;
{
	sigset_t new, old;

	if (imask == NULL) {
		how = SIG_BLOCK;
		new = 0;
	}

	else {
		new = *imask;
	}

	old = _sigprocmask(how, new);

	if (old == (sigset_t)-1) {
		return (-1);
	}
	else {
		if (omask != NULL)
			*omask = old;
		return (0);
	}
}
