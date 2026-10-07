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
 *  quit  --  print message and exit
 *
 *  Usage:  quit (status,format [,arg]...);
 *	int status;
 *	(... format and arg[s] make up a printf-arglist)
 *
 *  Quit is a way to easily print an arbitrary message and exit.
 *  It is most useful for error exits from a program:
 *	if (open (...) < 0) then quit (1,"Can't open...",file);
 */

#include <stdio.h>
#if __STDC__
#include <stdarg.h>
#else
#include <varargs.h>
#endif

#if __STDC__
quit (int status, char *fmt, ...)
#else
quit (va_alist)
va_dcl
#endif
{
#if !__STDC__
	int status;
	char *fmt;
#endif
	va_list args;

	fflush(stdout);
#if __STDC__
	va_start(args, fmt);
#else
	va_start(args);
	status = va_arg(args, int);
	fmt = va_arg(args, char *);
#endif
	(void) vfprintf(stderr, fmt, args);
	va_end(args);
	exit(status);
}
