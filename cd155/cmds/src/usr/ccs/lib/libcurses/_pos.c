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
static char sccsid[] = "@(#)_pos.c  1.6  com/lib/curses,3.1,8943 10/16/89 22:59:15";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _pos
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

extern	int	_outch();

/*
 * NAME:        _pos
 *
 * FUNCTION:
 *
 *      Position the SP->curptr to (row, column) which start at 0.
 */

_pos(row, column)
int	row;
int	column;
{
#ifdef DEBUG
    if(outf) fprintf(outf, "_pos from row %d, col %d => row %d, col %d\n",
	    SP->phys_y, SP->phys_x, row, column);
#endif
	if( SP->phys_x == column && SP->phys_y == row )
	{
		return;	/* already there */
	}
	/*
	 * Many terminals can't move the cursor when in standout mode.
	 * We must be careful, however, because HP's and cookie terminals
	 * will drop a cookie when we do this.
	 */
	if( !move_standout_mode && SP->phys_gr && magic_cookie_glitch < 0 )
	{
		if( !ceol_standout_glitch )
		{
			_clearhl ();
		}
	}
	/* some terminals can't move in insert mode */
	if( SP->phys_irm == 1 && !move_insert_mode )
	{
		tputs(exit_insert_mode, 1, _outch);
		SP->phys_irm = 0;
	}
	/* If we try to move outside the scrolling region, widen it */
	if( row<SP->phys_top_mgn || row>SP->phys_bot_mgn )
	{
		_window(0, lines-1, 0, columns-1);
		_setwind();
	}
	mvcur(SP->phys_y, SP->phys_x, row, column);
	SP->phys_x = column;
	SP->phys_y = row;
}
