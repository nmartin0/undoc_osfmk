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
static char sccsid[] = "@(#)m_move.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:26:10";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   m_move
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
# include	<signal.h>

/*
 * NAME:        m_move
 *
 * FUNCTION:
 *
 *      Move to given location on stdscr.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      Update curses notion of where we
 *      are (stdscr). This could be a macro
 *      and originally was, but if you do
 *          move(line++, 0)
 *      it will increment line twice, which is a lose.
 *
 *      mini.c contains versions of curses routines for minicurses.
 *      They work just like their non-mini counterparts but draw on
 *      std_body rather than stdscr.  This cuts down on overhead but
 *      restricts what you are allowed to do - you can't get stuff back
 *      from the screen and you can't use multiple windows or things
 *      like insert/delete line (the logical ones that affect the screen).
 */

m_move(row, col)
int row, col;
{
	stdscr->_cury=row;
	stdscr->_curx=col;
	_ll_move(row,col);
}
