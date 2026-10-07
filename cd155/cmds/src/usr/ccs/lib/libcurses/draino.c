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
static char sccsid[] = "@(#)draino.c  1.7  com/lib/curses,3.1,9008 12/4/89 21:01:17";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   draino
 *
 * ORIGINS: 3, 10, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1988
 * All Rights Reserved
 * Licensed Material - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 */

/*
 * Code for various kinds of delays.  Most of this is nonportable and
 * requires various enhancements to the operating system, so it won't
 * work on all systems.  It is included in curses to provide a portable
 * interface, and so curses itself can use it for function keys.
 */


#include "cursesext.h"
#include <signal.h>

#define NAPINTERVAL 100

struct _timeval {
	long tv_sec;
	long tv_usec;
};

/*
 * NAME:        draino
 *
 * FUNCTION:
 *
 *      Wait until the output has drained enough that it will only take
 *      ms more milliseconds to drain completely.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Needs Berkeley TIOCOUTQ ioctl.  Returns ERR if impossible.
 */

int
draino(ms)
int ms;
{
	int ncthere;	/* number of chars actually in output queue */
	int ncneeded;	/* number of chars = that many ms */
	int rv;		/* ioctl return value */

#ifdef TIOCOUTQ
# define _DRAINO
	/* 10 bits/char, 1000 ms/sec, baudrate in bits/sec */
	ncneeded = baudrate() * ms / (10 * 1000);
	for (;;) {
		ncthere = 0;
		rv = ioctl(cur_term->Filedes, TIOCOUTQ, &ncthere);
#ifdef DEBUG
		fprintf(outf, "draino: rv %d, ncneeded %d, ncthere %d\n",
			rv, ncneeded, ncthere);
#endif
		if (rv < 0)
			return ERR;	/* ioctl didn't work */
		if (ncthere <= ncneeded) {
			return (0);
		}
		napms(NAPINTERVAL);
	}
#endif

#ifdef TCSETAW
# define _DRAINO
	/*
	 * USG simulation - waits until the entire queue is empty,
	 * then sets the state to what it already is (e.g. no-op).
	 * This only works if ms is zero.
	 */
	if (ms <= 0) {
		ioctl(cur_term->Filedes, TCSETAW, cur_term->Nttyb);
		return (OK);
	}
#endif

#ifndef _DRAINO

	return ERR;
#endif
}
