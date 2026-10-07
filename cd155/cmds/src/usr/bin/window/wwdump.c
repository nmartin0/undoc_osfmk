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
static char rcsid[] = "@(#)$RCSfile: wwdump.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:25 $";
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
 * wwdump.c	3.13 (Berkeley) 6/29/88
 */


#include "ww.h"
#include "tt.h"

static char cmap[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

wwdumpwin(w)
register struct ww *w;
{
	register i, j;

	tt.tt_nmodes = 0;
	(*tt.tt_clear)();
	for (i = w->ww_i.t; i < w->ww_i.b; i++) {
		(*tt.tt_move)(i, w->ww_i.l);
		for (j = w->ww_i.l; j < w->ww_i.r; j++)
			(*tt.tt_putc)(w->ww_win[i][j] & WWM_GLS ? 'G' : ' ');
	}
}

wwdumpnvis(w)
register struct ww *w;
{
	register i;
	char buf[20];

	tt.tt_nmodes = 0;
	(*tt.tt_clear)();
	for (i = w->ww_i.t; i < w->ww_i.b; i++) {
		(*tt.tt_move)(i, w->ww_i.l);
		(void) sprintf(buf, "%d", w->ww_nvis[i]);
		(*tt.tt_write)(buf, strlen(buf));
	}
}

wwdumpsmap()
{
	register i, j;

	tt.tt_nmodes = 0;
	(*tt.tt_clear)();
	for (i = 0; i < wwnrow; i++) {
		(*tt.tt_move)(i, 0);
		for (j = 0; j < wwncol; j++)
			(*tt.tt_putc)(cmap[wwsmap[i][j]]);
	}
}

wwdumpns()
{
	register i, j;

	(*tt.tt_clear)();
	for (i = 0; i < wwnrow; i++) {
		(*tt.tt_move)(i, 0);
		for (j = 0; j < wwncol; j++) {
			tt.tt_nmodes = wwns[i][j].c_m & tt.tt_availmodes;
			(*tt.tt_putc)(wwns[i][j].c_c);
		}
	}
}

wwdumpos()
{
	register i, j;

	(*tt.tt_clear)();
	for (i = 0; i < wwnrow; i++) {
		(*tt.tt_move)(i, 0);
		for (j = 0; j < wwncol; j++) {
			tt.tt_nmodes = wwos[i][j].c_m & tt.tt_availmodes;
			(*tt.tt_putc)(wwns[i][j].c_c);
		}
	}
}
