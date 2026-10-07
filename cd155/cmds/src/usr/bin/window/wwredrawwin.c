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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: wwredrawwin.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:47 $";
#endif
/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 * 
 * wwredrawwin.c	3.13 (Berkeley) 6/29/88
 */


#include "ww.h"

wwredrawwin1(w, row1, row2, offset)
register struct ww *w;
int row1, row2, offset;
{
	int row;
	register col;
	register char *smap;
	register union ww_char *buf;
	register char *win;
	register union ww_char *ns;
	int x;
	int nchanged;

	for (row = row1; row < row2; row++) {
		col = w->ww_i.l;
		ns = wwns[row];
		smap = &wwsmap[row][col];
		buf = w->ww_buf[row + offset];
		win = w->ww_win[row];
		nchanged = 0;
		for (; col < w->ww_i.r; col++)
			if (*smap++ == w->ww_index &&
			    ns[col].c_w !=
			    (x = buf[col].c_w ^ win[col] << WWC_MSHIFT)) {
				nchanged++;
				ns[col].c_w = x;
			}
		if (nchanged > 4)
			wwtouched[row] |= WWU_MAJOR|WWU_TOUCHED;
		else if (nchanged > 0)
			wwtouched[row] |= WWU_TOUCHED;
	}
}
