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
static char sccsid[] = "@(#)pnoutrfrsh.c  1.5  com/lib/curses,3.1,8943 10/16/89 23:34:29";
/*
 * COMPONENT_NAME: (LIBCURSE) Curses Library
 *
 * FUNCTIONS:   pnoutrefresh
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
 * NAME:        pnoutrefresh
 *
 * FUNCTION:
 *
 *      Put out pad but don't actually update screen.
 */

pnoutrefresh(pad, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol)
register WINDOW	*pad;
int pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol;
{
	register int pr, r, c;
	register chtype	*nsp, *lch;

# ifdef DEBUG
	if(outf) fprintf(outf,
		"PREFRESH(pad %x, pcorner %d,%d, smin %d,%d, smax %d,%d)",
		pad, pminrow, pmincol, sminrow, smincol, smaxrow, smaxcol);
	_dumpwin(pad);
	if(outf) fprintf(outf, "PREFRESH:\n\tfirstch\tlastch\n");
# endif

	/* Make sure everything fits */
	if (pminrow < 0) pminrow = 0;
	if (pmincol < 0) pmincol = 0;
	if (sminrow < 0) sminrow = 0;
	if (smincol < 0) smincol = 0;
	if (smaxrow >= lines) smaxrow = lines-1;
	if (smaxcol >= columns) smaxcol = columns-1;
	if (smaxrow - sminrow > pad->_maxy - pminrow)
		smaxrow = sminrow + (pad->_maxy - pminrow);

	/* Copy it out, like a refresh, but appropriately offset */
	for (pr=pminrow,r=sminrow; r <= smaxrow; r++,pr++) {
		/* No record of what previous loc looked like, so do it all */
		lch = &pad->_y[pr][pad->_maxx-1];
		nsp = &pad->_y[pr][pmincol];
		_ll_move(r, smincol);
		for (c=smincol; nsp<=lch; c++) {
			if (SP->virt_x++ < columns && c <= smaxcol)
				*SP->curptr++ = *nsp++;
			else
				break;
		}
		pad->_firstch[pr] = _NOCHANGE;
	}
	lwin = pad;
	return OK;
}
