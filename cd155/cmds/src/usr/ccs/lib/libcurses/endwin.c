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
static char sccsid[] = "@(#)endwin.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:15:56";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   endwin
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

#include	"cursesext.h"

extern	int	_endwin;
extern	int	_c_clean();
extern	int	_fixdelay();
extern	int	_outch();
extern	int	_pos();
extern	int	doupdate();
extern	int	reset_shell_mode();
extern	int	tputs();

/*
 * NAME:        endwin
 *
 * FUNCTION:
 *
 *      Clean things up before exiting.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      endwin is TRUE if we have called endwin - this avoids calling it
 *      twice.
 */

int
endwin()
{
	int saveci = SP->check_input;

	if (_endwin)
		return;

	/* Flush out any output not output due to typeahead */
	SP->check_input = 9999;
	doupdate();
	SP->check_input = saveci;	/* in case of another initscr */

	_fixdelay(SP->fl_nodelay, FALSE);
	if (stdscr->_use_meta)
		tputs(meta_off, 1, _outch);
	_pos(lines-1, 0);
	_c_clean();
	_endwin = TRUE;
	reset_shell_mode();
	fflush(stdout);
}
