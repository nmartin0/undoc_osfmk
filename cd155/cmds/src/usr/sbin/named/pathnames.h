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
 * Copyright (c) 1989 The Regents of the University of California.
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
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *	@(#)pathnames.h	5.4 (Berkeley) 6/1/90
 */


#define	_PATH_BOOT	"/etc/named.boot"

#if defined(BSD) && BSD >= 198810
#include <paths.h>
#define	_PATH_XFER	"/usr/sbin/named-xfer"
#define	_PATH_DEBUG	"/var/tmp/named.run"
#define	_PATH_DUMPFILE	"/var/tmp/named_dump.db"
#define	_PATH_PIDFILE	"/var/run/named.pid"
#define	_PATH_STATS	"/var/tmp/named.stats"
#define	_PATH_TMPXFER	"/var/tmp/xfer.ddt.XXXXXX"
#define	_PATH_TMPDIR	"/var/tmp"

#else /* BSD */
#define	_PATH_DEVNULL	"/dev/null"
#define	_PATH_TTY	"/dev/tty"
#define	_PATH_XFER	"/etc/named-xfer"
#define	_PATH_DEBUG	"/usr/tmp/named.run"
#define	_PATH_DUMPFILE	"/usr/tmp/named_dump.db"
#define	_PATH_PIDFILE	"/etc/named.pid"
#define	_PATH_STATS	"/usr/tmp/named.stats"
#define	_PATH_TMPXFER	"/usr/tmp/xfer.ddt.XXXXXX"
#define	_PATH_TMPDIR	"/usr/tmp"
#endif /* BSD */
