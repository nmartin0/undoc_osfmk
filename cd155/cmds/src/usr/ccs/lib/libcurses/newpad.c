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
static char sccsid[] = "@(#)newpad.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:30:37";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   newpad
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
# include	<signal.h>

char	*calloc();
char	*malloc();
extern	char	*getenv();

extern	WINDOW	*makenew();

/*
 * NAME:        newpad
 *
 * FUNCTION:
 *
 *      Like newwin, but makes a pad instead of a window.
 *
 * EXECUTION ENVIRONMENT:
 *
 *      A pad is not associated with part of the screen, so it can be
 *      bigger.
 */

WINDOW *
newpad(nlines, ncols)
register int	nlines;
{
	register WINDOW	*win;
	register chtype	*sp;
	register int i;
	char *calloc();

	if ((win = makenew(nlines, ncols, 0, 0)) == NULL)
		return NULL;
	win->_flags |= _ISPAD;
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
