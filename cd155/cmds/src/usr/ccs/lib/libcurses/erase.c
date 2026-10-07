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
static char sccsid[] = "@(#)erase.c  1.6  com/lib/curses,3.1,8943 10/16/89 23:16:23";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   werase
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
 * NAME:        werase
 *
 * FUNCTION:
 *
 *      This routine erases everything on the _window.
 */

werase(win)
register WINDOW	*win; {

	register int		y;
	register chtype	*sp, *end, *start, *maxx;
	register int		minx;

# ifdef DEBUG
	if(outf) fprintf(outf, "WERASE(%0.2o), _maxx %d\n", win, win->_maxx);
# endif
	for (y = 0; y < win->_maxy; y++) {
		minx = _NOCHANGE;
		maxx = NULL;
		start = win->_y[y];
		end = &start[win->_maxx];
		for (sp = start; sp < end; sp++) {
#ifdef DEBUG
			if (y == 23) if(outf) fprintf(outf,
				"sp %x, *sp %c %o\n", sp, *sp, *sp);
#endif
			if (*sp != ' ') {
				maxx = sp;
				if (minx == _NOCHANGE)
					minx = sp - start;
				*sp = ' ';
			}
		}
		if (minx != _NOCHANGE) {
			if (win->_firstch[y] > minx
			     || win->_firstch[y] == _NOCHANGE)
				win->_firstch[y] = minx;
			if (win->_lastch[y] < maxx - win->_y[y])
				win->_lastch[y] = maxx - win->_y[y];
		}
# ifdef DEBUG
if(outf) fprintf(outf,
	"WERASE: minx %d maxx %d _firstch[%d] %d, start %x, end %x\n",
		minx, maxx ? maxx-start : NULL, y, win->_firstch[y],
			start, end);
# endif
	}
	win->_curx = win->_cury = 0;
}
