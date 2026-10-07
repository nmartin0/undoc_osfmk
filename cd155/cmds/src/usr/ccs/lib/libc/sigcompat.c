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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: sigcompat.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 02:08:12 $";
#endif

/*
 * Copyright (c) 1989 The Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <sys/param.h>
#include <sys/signal.h>

sigvec(signo, sv, osv)
	int signo;
	struct sigvec *sv, *osv;
{
        struct sigaction act, *sa;
	int ret;

	/*
	 * Have to copy the user's structure, since we're going to invert
	 * the SV_INTERRUPT bit, and we may not be able to write into
	 * caller's input argument.
	 */

	if (sv) {
		act.sa_handler = sv->sv_handler;
		act.sa_mask = sv->sv_mask;
		act.sa_flags = sv->sv_flags ^ SV_INTERRUPT; /* !SA_INTERRUPT */
		sa = &act;
	}
	else {
		sa = (struct sigaction *)0;
	}
	ret = sigaction(signo, sa, (struct sigaction *)osv);
	if (ret == 0 && osv)

		osv->sv_flags ^= SV_INTERRUPT;	/* !SA_INTERRUPT */
	return (ret);
}

sigsetmask(mask)
	int mask;
{
	int omask, n;

	n = sigprocmask(SIG_SETMASK, (sigset_t *) &mask, (sigset_t *) &omask);
	if (n)
		return (n);
	return (omask);
}

sigblock(mask)
	int mask;
{
	int omask, n;

	n = sigprocmask(SIG_BLOCK, (sigset_t *) &mask, (sigset_t *) &omask);
	if (n)
		return (n);
	return (omask);
}

sigpause(mask)
	int mask;
{

	return (sigsuspend((sigset_t *)&mask));
}
