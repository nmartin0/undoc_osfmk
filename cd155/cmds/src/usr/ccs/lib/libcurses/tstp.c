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
static char sccsid[] = "@(#)tstp.c  1.6  com/lib/curses,3.1,9008 12/4/89 21:03:16";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _tstp
 *
 * ORIGINS: 3, 10, 26, 27
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

# include	<signal.h>

# ifdef SIGTSTP

# include	"cursesext.h"

/*
 * NAME:        tstp
 *
 * FUNCTION:
 *
 *      Handle stop and start signals
 */
void
_tstp() 

{
	sigset_t mask;

# ifdef DEBUG
	if (outf) fflush(outf);
# endif
	_ll_move(lines-1, 0);
	endwin();
	fflush(stdout);
	sigemptyset(&mask);
	sigaddset(&mask, SIGTSTP);
	signal(SIGTSTP, SIG_DFL);
	sigprocmask(SIG_UNBLOCK, &mask, (sigset_t *)0);
	kill(0, SIGTSTP);
	signal((int)SIGTSTP, ((void (*)(int))(int) _tstp));
	fixterm();
	SP->doclear = 1;
	wrefresh(curscr);
}
# endif
