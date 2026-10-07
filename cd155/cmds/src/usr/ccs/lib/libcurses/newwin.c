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
static char sccsid[] = "@(#)newwin.c  1.7  com/lib/curses,3.1,8943 10/20/89 11:57:20";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   newwin
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

char	*calloc();
char	*malloc();
extern	char	*getenv();

extern	WINDOW	*makenew();

/*
 * NAME:
 *
 * FUNCTION:
 *
 *      Allocate space for and set up defaults for a new _window.
 */

WINDOW *
newwin(nlines, ncols, by, bx)
register int	nlines, ncols, by, bx;
{
	register WINDOW	*win;
	register chtype	*sp;
	register int i;
	char *calloc();

	if (by + nlines > LINES)
		nlines = LINES - by;
	if (bx + ncols > COLS)
		ncols = COLS - bx;

	if (nlines == 0)
		nlines = LINES - by;
	if (ncols == 0)
		ncols = COLS - bx;

	if ((win = makenew(nlines, ncols, by, bx)) == NULL)
		return NULL;
	for (i = 0; i < nlines; i++)
		if ((win->_y[i] = (chtype *) calloc(ncols, sizeof (chtype)))
								 == NULL) {
			register int j;

			for (j = 0; j < i; j++)
				cfree((char *)win->_y[j]);
			cfree((char *)win->_firstch);
			cfree((char *)win->_lastch);
			cfree((char *)win->_y);
			cfree((char *)win);
			return NULL;
		}
		else
			for (sp = win->_y[i]; sp < win->_y[i] + ncols; )
				*sp++ = ' ';
	return win;
}
