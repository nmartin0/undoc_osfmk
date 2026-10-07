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
static char sccsid[] = "@(#)subwin.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:40:40";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   subwin
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
 * NAME:        subwin
 */

WINDOW *
subwin(orig, num_lines, num_cols, begy, begx)
register WINDOW	*orig;
int	num_lines, num_cols, begy, begx;
{

	register int i;
	register WINDOW	*win;
	register int by, bx, nlines, ncols;
	register int j, k;

	by = begy;
	bx = begx;
	nlines = num_lines;
	ncols = num_cols;

	/*
	 * make sure window fits inside the original one
	 */
# ifdef	DEBUG
	if(outf) fprintf(outf,
		"SUBWIN(%0.2o, %d, %d, %d, %d)\n",
		orig, nlines, ncols, by, bx);
# endif
	if (by < orig->_begy || bx < orig->_begx
	    || by + nlines > orig->_begy + orig->_maxy
	    || bx + ncols  > orig->_begx + orig->_maxx)
		return NULL;
	if (nlines == 0)
		nlines = orig->_maxy - orig->_begy - by;
	if (ncols == 0)
		ncols = orig->_maxx - orig->_begx - bx;
	if ((win = makenew(nlines, ncols, by, bx)) == NULL)
		return NULL;
	j = by - orig->_begy;
	k = bx - orig->_begx;
	for (i = 0; i < nlines; i++)
		win->_y[i] = &orig->_y[j++][k];
	win->_flags = _SUBWIN;
	return win;
}
