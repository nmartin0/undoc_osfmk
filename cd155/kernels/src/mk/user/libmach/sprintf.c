/*
 * 
 * $Copyright
 * Copyright 1991 , 1994, 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/* 
 * Mach Operating System
 * Copyright (c) 1991,1990,1989 Carnegie Mellon University
 * All Rights Reserved.
 * 
 * Permission to use, copy, modify and distribute this software and its
 * documentation is hereby granted, provided that both the copyright
 * notice and this permission notice appear in all copies of the
 * software, derivative works or modified versions, and any portions
 * thereof, and that both notices appear in supporting documentation.
 * 
 * CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
 * CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
 * ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
 * 
 * Carnegie Mellon requests users of this software to return to
 * 
 *  Software Distribution Coordinator  or  Software.Distribution@CS.CMU.EDU
 *  School of Computer Science
 *  Carnegie Mellon University
 *  Pittsburgh PA 15213-3890
 * 
 * any improvements or extensions that they make and grant Carnegie Mellon
 * the rights to redistribute these changes.
 */
/*
 * HISTORY
 * $Log: sprintf.c,v $
 * Revision 1.6  1995/03/07  22:34:20  berg
 *  Reviewer: yazz@locus.com cfleck@ssd.intel.com
 *  Risk: Low
 *  Benefit or PTS #: 12387
 *  Testing: by inspection
 *  Module(s): doprnt.c printf.c sprintf.c
 *
 * added code to libmach functions doprnt.c printf.c sprintf.c to keep track of
 * the number characters output.
 *
 * Revision 1.5  1994/11/18  21:05:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/06/30  23:04:45  dleslie
 * Adding copyright notices required by legal folks
 *
 * Revision 1.3  1993/04/27  20:59:01  dleslie
 * Copy of R1.0 sources onto main trunk
 *
 * Revision 1.1.10.2  1993/04/22  19:05:06  dleslie
 * First R1_0 release
 *
 * Revision 2.2.2.1  92/03/28  10:35:16  jeffreyh
 * 	Changes from MK71
 * 	[92/03/11  18:36:05  jeffreyh]
 * 
 * Revision 2.2  92/02/20  15:57:57  elf
 * 	Created.
 * 	[92/02/11            rpd]
 * 
 */

#include <varargs.h>

static void
savechar(arg, c)
	char *arg;
	int c;
{
	*(*(char **)arg)++ = c;
}

vsprintf(s, fmt, args)
	char *s;
	char *fmt;
	va_list args;
{
	int	rv;
	rv = _doprnt(fmt, args, 0, (void (*)()) savechar, (char *) &s);
	*s = 0;
	return (rv);
}

/*VARARGS2*/
sprintf(s, fmt, va_alist)
	char *s;
	char *fmt;
	va_dcl
{
	va_list	args;
	int	rv;

	va_start(args);
	rv = vsprintf(s, fmt, args);
	va_end(args);
	return (rv);
}
