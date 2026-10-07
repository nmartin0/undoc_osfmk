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
static char rcsid[] = "@(#)$RCSfile: wwpty.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:49:41 $";
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
 * wwpty.c	3.15 (Berkeley) 6/29/88
 */


#include "ww.h"
#ifdef POSIX_TTY
#include <sys/ioctl.h>
#endif

wwgetpty(w)
register struct ww *w;
{
	register char c, *p;
	int tty;
	int on = 1;
#define PTY "/dev/XtyXX"
#define _PT	5
#define _PQRS	8
#define _0_9	9

	(void) strcpy(w->ww_ttyname, PTY);
	for (c = 'p'; c <= 'u'; c++) {
		w->ww_ttyname[_PT] = 'p';
		w->ww_ttyname[_PQRS] = c;
		w->ww_ttyname[_0_9] = '0';
		if (access(w->ww_ttyname, 0) < 0)
			break;
		for (p = "0123456789abcdef"; *p; p++) {
			w->ww_ttyname[_PT] = 'p';
			w->ww_ttyname[_0_9] = *p;
			if ((w->ww_pty = open(w->ww_ttyname, 2)) < 0)
				continue;
			w->ww_ttyname[_PT] = 't';
			if ((tty = open(w->ww_ttyname, 2)) < 0) {
				(void) close(w->ww_pty);
				continue;
			}
			(void) close(tty);
			if (ioctl(w->ww_pty, TIOCPKT, (char *)&on) < 0) {
				(void) close(w->ww_pty);
				continue;
			}
			return 0;
		}
	}
	w->ww_pty = -1;
	wwerrno = WWE_NOPTY;
	return -1;
}
