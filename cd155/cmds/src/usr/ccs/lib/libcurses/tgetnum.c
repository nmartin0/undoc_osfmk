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
static char sccsid[] = "@(#)tgetnum.c  1.7  com/lib/curses,3.1,9008 12/14/89 17:53:13";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   tgetnum
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

#include "cursesext.h"

#define	two( s1, s2 )	(s1 + 256 * s2 )
#define	twostr( str )	two( *str, str[ 1 ] )

/*
 * NAME:        tgetnum
 *
 * FUNCTION:
 *
 *      Simulation of termcap using terminfo.
 */

int
tgetnum(id)
char *id;
{
	int rv;

	switch (twostr(id)) {
	case two('c','o'): rv = columns; break;
	case two('i','t'): rv = init_tabs; break;
	case two('l','i'): rv = lines; break;
	case two('l','m'): rv = lines_of_memory; break;
	case two('s','g'): rv = magic_cookie_glitch; break;
	case two('p','b'): rv = padding_baud_rate; break;
	case two('v','t'): rv = virtual_terminal; break;
	case two('w','s'): rv = width_status_line; break;
	default: rv = -1;
	}
	return rv;
}
