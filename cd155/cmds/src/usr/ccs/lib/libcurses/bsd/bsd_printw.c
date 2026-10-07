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
static char sccsid[] = "@(#)printw.c	5.4 (Berkeley) 6/30/88";
#endif /* not lint */

/*
 * printw and friends
 *
 */

#if __STDC__
# include <stdarg.h>
#else
# include <varargs.h>
#endif
# include	"curses.ext"

/*
 *	This routine implements a printf on the standard screen.
 */
#if __STDC__
printw(char *fmt, ...)
#else
printw(va_alist)
va_dcl
#endif
{
#if !__STDC__
	char	*fmt;
#endif
	va_list	ap;
	char	buf[512];

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
	fmt = va_arg(ap, char *);
#endif
	(void) vsprintf(buf, fmt, ap);
	va_end(ap);
	return waddstr(stdscr, buf);
}

/*
 *	This routine implements a printf on the given window.
 */
#if __STDC__
wprintw(WINDOW *win, char *fmt, ...)
#else
wprintw(va_alist)
va_dcl
#endif
{
#if !__STDC__
	WINDOW	*win;
	char	*fmt;
#endif
	va_list	ap;
	char	buf[512];

#if __STDC__
	va_start(ap, fmt);
#else
	va_start(ap);
	win = va_arg(ap, WINDOW *);
	fmt = va_arg(ap, char *);
#endif
	(void) vsprintf(buf, fmt, ap);
	va_end(ap);
	return waddstr(win, buf);
}
