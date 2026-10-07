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
 * Copyright (c) 1981 Regents of the University of California.
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
 */

#ifndef lint
static char sccsid[] = "@(#)scanw.c	5.3 (Berkeley) 6/30/88";
#endif /* not lint */

/*
 * scanw and friends
 *
 */

#if __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif
# include	"curses.ext"

/*
 *	This routine implements a scanf on the standard screen.
 */
#if __STDC__
scanw(char *fmt, ...)
#else
scanw(va_alist)
va_dcl
#endif
{
#if !__STDC__
	char	*fmt;
#endif
	va_list	ap;

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
	fmt = va_arg(ap, char *);
#endif
	return _sscans(stdscr, fmt, ap);
	va_end(ap);
}
/*
 *	This routine implements a scanf on the given window.
 */
#if __STDC__
wscanw(WINDOW *win, char *fmt, ...)
#else
wscanw(va_alist)
va_dcl
#endif
{
#if !__STDC__
	WINDOW	*win;
	char	*fmt;
#endif
	va_list	ap;

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
	win = va_arg(ap, WINDOW *);
	fmt = va_arg(ap, char *);
#endif
	return _sscans(win, fmt, ap);
	va_end(ap);
}
/*
 *	This routine actually executes the scanf from the window.
 */
_sscans(win, fmt, ap)
WINDOW	*win;
char	*fmt;
va_list	ap; {

	char	buf[100];

	if (wgetstr(win, buf) == ERR)
		return ERR;
	return vsscanf(buf, fmt, ap);
}
