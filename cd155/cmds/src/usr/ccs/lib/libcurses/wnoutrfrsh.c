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
static char sccsid[] = "@(#)wnoutrfrsh.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:47:38";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   wnoutrefresh
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
 * make the current screen look like "win" over the area covered by
 * win.
 *
 */

#include	"cursesext.h"

extern	WINDOW *lwin;

/*
 * NAME:        wnoutrefresh
 *
 * FUNCTION:
 *
 *      Put out window but don't actually update screen.
 */

wnoutrefresh(win)
register WINDOW	*win;
{
	register int wy, y;
	register chtype	*nsp, *lch;

# ifdef DEBUG
	if( win == stdscr )
	{
		if(outf) fprintf(outf, "REFRESH(stdscr %x)", win);
	}
	else
	{
		if( win == curscr )
		{
			if(outf) fprintf(outf, "REFRESH(curscr %x)", win);
		}
		else
		{
			if(outf) fprintf(outf, "REFRESH(%d)", win);
		}
	}
	if(outf) fprintf(outf,
		" (win == curscr) = %d, maxy %d\n",
		win, (win == curscr), win->_maxy);
	if( win != curscr )
	{
		_dumpwin( win );
	}
	if(outf) fprintf(outf, "REFRESH:\n\tfirstch\tlastch\n");
# endif	/* DEBUG */
	/*
	 * initialize loop parameters
	 */

	if( win->_clear || win == curscr || SP->doclear )
	{
# ifdef DEBUG
		if (outf) fprintf(outf,
			"refresh clears, win->_clear %d, curscr %d\n",
			win->_clear, win == curscr);
# endif	/* DEBUG */
		SP->doclear = 1;
		win->_clear = FALSE;
		if( win != curscr )
		{
			touchwin( win );
		}
	}

	if( win == curscr )
	{
#ifdef	DEBUG
	if(outf) fprintf(outf, "Calling _ll_refresh(FALSE)\n" );
#endif	/* DEBUG */
		_ll_refresh(FALSE);
		return OK;
	}
#ifdef	DEBUG
	if(outf) fprintf(outf, "Didn't do _ll_refresh(FALSE)\n" );
#endif	/* DEBUG */

	for( wy = 0; wy < win->_maxy; wy++ )
	{
		if( win->_firstch[wy] != _NOCHANGE )
		{
			y = wy + win->_begy;
			lch = &win->_y[wy][win->_maxx-1];
			nsp = &win->_y[wy][0];
			_ll_move(y, win->_begx);
			while( nsp <= lch )
			{
				if( SP->virt_x++ < columns )
				{
					*SP->curptr++ = *nsp++;
				}
				else
				{
					break;
				}
			}
			win->_firstch[wy] = _NOCHANGE;
		}
	}
	lwin = win;
	return OK;
}
