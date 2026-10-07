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
/* sigsuspend -- library stub to transform calling sequence */

/*
 * user program calls:
 *     success = sigssuspend(sigset_t *mask);
 *
 * kernel-side interface:
 *     success = sigsuspend(sigset_t mask);
 *
 */

#include <sys/signal.h>
#include <sys/param.h>

extern int _sigsuspend();

int
sigsuspend(mask)
sigset_t	*mask;
{
	return(_sigsuspend(*mask));
}
