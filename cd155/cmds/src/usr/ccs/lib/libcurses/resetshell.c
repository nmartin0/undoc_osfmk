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
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 
static char sccsid[] = "@(#)resetshell.c  1.6  com/lib/curses,3.1,8943 10/20/89 11:57:51";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   reset_shell_mode
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

#include "cursesext.h"

extern	struct term *cur_term;

#ifdef DIOCSETT

static struct termcb new, old;
#endif
#ifdef LTILDE
static int newlmode, oldlmode;
#endif

#ifdef USG
#define BR(x) (cur_term->x.c_cflag&CBAUD)
#else
#define BR(x) (cur_term->x.sg_ispeed)
#endif

/*
 * NAME:
 *
 * FUNCTION:
 *
 *      Disable CB/UNIX virtual terminals.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Getting the baud rate is different on the two systems.
 *      In either case, a baud rate of 0 hangs up the phone.
 *      Since things are often initialized to 0, getting the phone
 *      hung up on you is a common result of an error in your program.
 *      This is not very friendly, so if the baud rate is 0, we
 *      assume we're doing a reset_xx_mode with no def_xx_mode, and
 *      just don't do anything.
 */

reset_shell_mode()
{
#ifdef DIOCSETT
	/*
	 * Restore any virtual terminal setting.  This must be done
	 * before the TIOCSETN because DIOCSETT will clobber flags like xtabs.
	 */
	old.st_flgs |= TM_SET;
	ioctl(2, DIOCSETT, &old);
#endif
	if ((cur_term != NULL) && BR(Ottyb)) {
		ioctl(cur_term -> Filedes,
#ifdef USG
			TCSETAW,
#else
			TIOCSETN,
#endif
			&(cur_term->Ottyb));
# ifdef LTILDE
	if (newlmode != oldlmode)
		ioctl(cur_term -> Filedes, TIOCLSET, &oldlmode);
# endif
	}
}
