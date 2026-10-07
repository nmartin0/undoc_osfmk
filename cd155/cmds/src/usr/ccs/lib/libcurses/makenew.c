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
static char sccsid[] = "@(#)makenew.c  1.6  com/lib/curses,3.1,9008 11/11/89 20:13:30";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   makenew
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
extern	char	*getenv();

extern	WINDOW	*makenew();

/*
 * NAME:        makenew
 *
 * FUNCTION:
 *
 *      This routine sets up a window buffer and returns a pointer to it.
 */

WINDOW *
makenew(num_lines, num_cols, begy, begx)
int	num_lines, num_cols, begy, begx;
{

	register int	i;
	register WINDOW	*win;
	register int	by, bx, nlines, ncols;
	char *calloc();

	by = begy;
	bx = begx;
	nlines = num_lines;
	ncols = num_cols;

	if (nlines <= 0 || ncols <= 0 || by > LINES || bx > COLS)
		return NULL;

# ifdef	DEBUG
	if(outf) fprintf(outf,
	"MAKENEW(%d, %d, %d, %d)\n", nlines, ncols, by, bx);
# endif
	if ((win = (WINDOW *) calloc(1, sizeof (WINDOW))) == NULL)
		return NULL;
# ifdef DEBUG
	if(outf) fprintf(outf, "MAKENEW: nlines = %d\n", nlines);
# endif
	if ((win->_y = (chtype **) calloc(nlines, sizeof(chtype *)))==NULL) {
		cfree((char *)win);
		return (WINDOW *) NULL;
	}
	if ((win->_firstch=(short *) calloc(nlines, sizeof(short)))==NULL) {
		cfree((char *)win);
		cfree((char *)win->_y);
	}
	if ((win->_lastch = (short *) calloc(nlines, sizeof(short)))==NULL) {
		cfree((char *)win);
		cfree((char *)win->_y);
		cfree((char *)win->_firstch);
	}
# ifdef DEBUG
	if(outf) fprintf(outf, "MAKENEW: ncols = %d\n", ncols);
# endif
	win->_cury = win->_curx = 0;
	win->_clear = (nlines == LINES && ncols == COLS);
	win->_maxy = nlines;
	win->_maxx = ncols;
	win->_begy = by;
	win->_begx = bx;
	win->_scroll = win->_leave = win->_use_idl = FALSE;
	win->_tmarg = 0;
	win->_bmarg = nlines - 1;
	for (i = 0; i < nlines; i++)
		win->_firstch[i] = win->_lastch[i] = _NOCHANGE;
	if (bx + ncols == COLS) {
		win->_flags |= _ENDLINE;
		/* Full window: scrolling heuristics (linefeed) work */
		if (nlines == LINES && ncols == COLS &&
		    by == 0 && bx == 0 && scroll_forward)
			win->_flags |= _FULLWIN;
		/* Scrolling window: it might scroll on us by accident */
		if (by + nlines == LINES && auto_right_margin)
			win->_flags |= _SCROLLWIN;
	}
# ifdef DEBUG
	if(outf) fprintf(outf, "MAKENEW: win->_clear = %d\n", win->_clear);
	if(outf) fprintf(outf, "MAKENEW: win->_leave = %d\n", win->_leave);
	if(outf) fprintf(outf, "MAKENEW: win->_scroll = %d\n", win->_scroll);
	if(outf) fprintf(outf, "MAKENEW: win->_flags = %0.2o\n", win->_flags);
	if(outf) fprintf(outf, "MAKENEW: win->_maxy = %d\n", win->_maxy);
	if(outf) fprintf(outf, "MAKENEW: win->_maxx = %d\n", win->_maxx);
	if(outf) fprintf(outf, "MAKENEW: win->_begy = %d\n", win->_begy);
	if(outf) fprintf(outf, "MAKENEW: win->_begx = %d\n", win->_begx);
# endif
	return win;
}
