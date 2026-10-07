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
 *  quit  --  print message and exit
 *
 *  Usage:  quit (status,format [,arg]...);
 *	int status;
 *	(... format and arg[s] make up a printf-arglist)
 *
 *  Quit is a way to easily print an arbitrary message and exit.
 *  It is most useful for error exits from a program:
 *	if (open (...) < 0) then quit (1,"Can't open...",file);
 *
 **********************************************************************
 * HISTORY
 * $Log: quit.c,v $
 * Revision 1.2  1994/11/21  16:39:18  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1993/04/19  20:26:47  stans
 * Initial revision
 *
 * Revision 1.2  88/12/13  13:52:41  gm0w
 * 	Rewritten to use varargs.
 * 	[88/12/13            gm0w]
 * 
 **********************************************************************
 */

#include <stdio.h>
#include <varargs.h>

quit (status, fmt, va_alist)
int status;
char *fmt;
va_dcl
{
	va_list args;

	fflush(stdout);
	va_start(args);
	(void) vfprintf(stderr, fmt, args);
	va_end(args);
	exit(status);
}
