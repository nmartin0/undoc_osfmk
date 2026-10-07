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
static char sccsid[] = "@(#)tgetflag.c  1.6  com/lib/curses,3.1,9008 12/14/89 17:52:54";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   tgetflag
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

#define two(s1, s2) (s1 + 256*s2)
#define twostr(str) two(*str, str[1])

/*
 * NAME:        tgetflag
 *
 * FUNCTION:
 *
 *      Simulation of termcap using terminfo.
 */

int
tgetflag(id)
char *id;
{
	register int rv;
	register char *p;

	switch (twostr(id)) {
	case two('b','w'): rv = auto_left_margin; break;
	case two('a','m'): rv = auto_right_margin; break;
	case two('x','b'): rv = beehive_glitch; break;
	case two('x','s'): rv = ceol_standout_glitch; break;
	case two('x','n'): rv = eat_newline_glitch; break;
	case two('e','o'): rv = erase_overstrike; break;
	case two('g','n'): rv = generic_type; break;
	case two('h','c'): rv = hard_copy; break;
	case two('k','m'): rv = has_meta_key; break;
	case two('h','s'): rv = has_status_line; break;
	case two('i','n'): rv = insert_null_glitch; break;
	case two('d','a'): rv = memory_above; break;
	case two('d','b'): rv = memory_below; break;
	case two('m','i'): rv = move_insert_mode; break;
	case two('m','s'): rv = move_standout_mode; break;
	case two('o','s'): rv = over_strike; break;
	case two('e','s'): rv = status_line_esc_ok; break;
	case two('x','t'): rv = teleray_glitch; break;
	case two('h','z'): rv = tilde_glitch; break;
	case two('u','l'): rv = transparent_underline; break;
	case two('x','o'): rv = xon_xoff; break;
	case two('b','s'):
		p = cursor_left;
		rv = p && *p==8 && p[1] == 0;
		break;
	case two('p','t'):
		p = tab;
		rv = p && *p==9 && p[1] == 0;
		break;
	case two('n','c'):
		p = carriage_return;
		rv = ! (p && *p==13 && p[1] == 0);
		break;
	case two('n','s'):
		p = scroll_forward;
		rv = ! (p && *p==10 && p[1] == 0);
		break;
	default: rv = 0;
	}
	return rv;
}
