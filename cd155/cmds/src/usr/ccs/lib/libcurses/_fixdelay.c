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
static char sccsid[] = "@(#)_fixdelay.c	1.7  com/lib/curses,3.1,8943 10/16/89 22:55:52";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _fixdelay
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

/*
 * NAME:        _fixdelay
 *
 * FUNCTION:
 *
 * EXECUTION ENVIRONMENT:
 *
 *      The use has just changed his notion of whether we want nodelay
 *      mode.   Do any system dependent processing.
 *
 */

_fixdelay(old, new)
char old, new;
{
#ifdef USG
# include <fcntl.h>
	int fl, rv, fd;
	extern int errno;
	FILE	*inf;

	inf = SP -> term_file;
	if( inf == stdout )
	{
		inf = stdin;
	}
	fd = fileno( inf );
	fl = fcntl(fd, F_GETFL, 0);
	if (new)
		fl |= O_NDELAY;
	else
		fl &= ~O_NDELAY;
	if (old != new)
	{
		rv = fcntl(fd, F_SETFL, fl);
		SP->fl_nodelay = new;
	}
#else
	/* No system dependent processing on the V7 or Berkeley systems. */
#endif
}
