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
static char sccsid[] = "@(#)_dumpwin.c	1.7  com/lib/curses,3.1,9008 12/4/89 21:00:51";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   _dumpwin
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

# include       "cursesext.h"

/*
 * NAME:        _dumpwin
 *
 * FUNCTION:
 *
 *      Make the current screen look like "win" over the area covered by
 *      win.
 */

#ifdef DEBUG
_dumpwin(win)
register WINDOW *win;
{
	register int x, y;
	register chtype *nsp;

	if (!outf) {
		return;
	}
	if (win == stdscr)
		fprintf(outf, "_dumpwin(stdscr)--------------\n");
	else if (win == curscr)
		fprintf(outf, "_dumpwin(curscr)--------------\n");
	else
		fprintf(outf, "_dumpwin(%o)----------------\n", win);
	for (y=0; y<win->_maxy; y++) {
		if (y > 76)
			break;
		nsp = &win->_y[y][0];
		fprintf(outf, "%d: ", y);
		for (x=0; x<win->_maxx; x++) {
			_sputc(*nsp, outf);
			nsp++;
		}
		fprintf(outf, "\n");
	}
	fprintf(outf, "end of _dumpwin----------------------\n");
}
#endif
