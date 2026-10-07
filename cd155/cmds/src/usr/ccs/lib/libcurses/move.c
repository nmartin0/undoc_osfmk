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
static char sccsid[] = "@(#)move.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:28:21";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   wmove
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

# include	"cursesext.h"

/*
 * NAME:        wmove
 *
 * FUNCTION:
 *
 *      This routine moves the cursor to the given point.
 */

wmove(win, y, x)
register WINDOW	*win;
register int		y, x;
{

# ifdef DEBUG
	if(outf) fprintf(outf, "MOVE to win ");
	if( win == stdscr )
	{
		if(outf) fprintf(outf, "stdscr ");
	}
	else
	{
		if(outf) fprintf(outf, "%o ", win);
	}
	if(outf) fprintf(outf, "(%d, %d)\n", y, x);
# endif
	if( x >= win->_maxx || y >= win->_maxy )
	{
		return ERR;
	}
	win->_curx = x;
	win->_cury = y;
	return OK;
}
