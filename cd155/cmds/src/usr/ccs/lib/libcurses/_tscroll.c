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
static char sccsid[] = "@(#)_tscroll.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:02:28";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _tscroll
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

# include	"cursesext.h"

/*
 * NAME:        _tscroll
 */

_tscroll( win )
register WINDOW	*win;
{

	register chtype	*sp;
	register int		i;
	register chtype	*temp;
	register int	top, bot;

#ifdef DEBUG
	if( win == stdscr )
	{
		if(outf) fprintf( outf, "scroll( stdscr )\n" );
	}
	else
	{
		if( win == curscr)
		{
			if(outf) fprintf( outf, "scroll( curscr )\n" );
		}
		else
		{
			if(outf) fprintf( outf, "scroll( %x )\n", win );
		}
	}
#endif
	if( !win->_scroll )
	{
		return ERR;
	}
	/* scroll the window lines themselves up */
	top = win->_tmarg;
	bot = win->_bmarg;
	temp = win->_y[top];
#ifdef	DEBUG
	if(outf)
	{
		fprintf( outf, "top = %d, bot = %d\n", top, bot );
	}
#endif	/* DEBUG */
	for (i = top; i < bot; i++)
	{
		win->_y[i] = win->_y[i+1];
	}
	/* Put a blank line in the opened up space */
	for (sp = temp; sp - temp < win->_maxx; )
		*sp++ = ' ';
	win->_y[bot] = temp;
#ifdef	DEBUG
	if(outf)
	{
		fprintf(outf,"SCROLL win [0%o], curscr [0%o], top %d, bot %d\n",
				win, curscr, top, bot);
		fprintf( outf, "Doing --> touchwin( 0%o )\n", win );
	}
#endif	/* DEBUG */
	win->_cury--;
	touchwin(win);
	return OK;
}
