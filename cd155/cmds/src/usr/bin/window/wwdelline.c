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
static char rcsid[] = "@(#)$RCSfile: wwdelline.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:24 $";
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
 * wwdelline.c	3.10 (Berkeley) 6/29/88
 */


#include "ww.h"

wwdelline(w, row)
register struct ww *w;
int row;
{
	register i;
	register union ww_char **cpp, **cqq;
	register union ww_char *cp;
	int row1, row2;
	char deleted;
	int visible;

	/*
	 * Scroll first.
	 */
	if ((row1 = row) < w->ww_i.t) {
		row1 = w->ww_i.t;
	}
	if ((row2 = w->ww_b.b) > w->ww_i.b) {
		row2 = w->ww_i.b;
		visible = 0;
	} else
		visible = 1;
	deleted = wwscroll1(w, row1, row2, 1, visible);

	/*
	 * Fix the buffer.
	 * But leave clearing the last line for wwclreol().
	 */
	cpp = &w->ww_buf[row];
	cqq = cpp + 1;
	cp = *cpp;
	for (i = w->ww_b.b - row; --i > 0;)
		*cpp++ = *cqq++;
	*cpp = cp;

	/*
	 * Now clear the last line.
	 */
	if (visible)
		wwclreol1(w, w->ww_b.b - 1, w->ww_b.l, deleted);
	else {
		cp += w->ww_b.l;
		for (i = w->ww_b.nc; --i >= 0;)
			cp++->c_w = ' ';
	}
}
