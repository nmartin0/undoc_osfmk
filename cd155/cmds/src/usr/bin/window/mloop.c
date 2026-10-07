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
static char rcsid[] = "@(#)$RCSfile: mloop.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:48:16 $";
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
 *  	mloop.c	3.13 (Berkeley) 6/29/88
 */


#include "defs.h"
#include <sys/signal.h>

mloop()
{
	kill(getpid(), SIGIO);	/* catch typeahead before ASYNC was set */
	while (!quit) {
		if (incmd) {
			docmd();
		} else if (wwcurwin->ww_state != WWS_HASPROC) {
			if (!wwcurwin->ww_keepopen)
				closewin(wwcurwin);
			setcmd(1);
			if (wwpeekc() == escapec)
				(void) wwgetc();
			error(MSGSTR(PROCDIED, "Process died."));
		} else {
			register struct ww *w = wwcurwin;
			register char *p;
			register n;

			if (wwibp >= wwibq)
				wwiomux();
			for (p = wwibp; p < wwibq && *p != escapec;
			     p++)
				;
			if ((n = p - wwibp) > 0) {
				if (!w->ww_ispty && w->ww_stopped)
					startwin(w);
				(void) write(w->ww_pty, wwibp, n);
				wwibp = p;
			}
			if (wwpeekc() == escapec) {
				(void) wwgetc();
				setcmd(1);
			}
		}
	}
}
