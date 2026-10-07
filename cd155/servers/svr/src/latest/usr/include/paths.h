/*
 * 
 * $Copyright
 * Copyright 1993, 1994 , 1995 Intel Corporation
 * INTEL CONFIDENTIAL
 * The technical data and computer software contained herein are subject
 * to the copyright notices; trademarks; and use and disclosure
 * restrictions identified in the file located in /etc/copyright on
 * this system.
 * Copyright$
 * 
 */
 
/*
 * @OSF_COPYRIGHT@
 */
/*
 * HISTORY
 * $Log: paths.h,v $
 * Revision 1.3  1994/11/18  21:10:39  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:16:22  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/05  23:59:18  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:17:53  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:20:27  nandy
 * Initial revision
 *
 * Revision 1.8  92/04/13  14:38:20  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 1.8  90/10/07  20:33:01  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  20:53:41  gm]
 * 
 * Revision 1.7  90/08/09  14:27:32  devrcs
 * 	mods for BSD4.4 reno
 * 	[90/08/02  12:50:43  jrieden]
 * 
 * Revision 1.6  90/07/27  10:42:03  devrcs
 * 	removed DEFPATH, added NOLOGIN
 * 	[90/07/22  16:39:23  jrieden]
 * 
 * Revision 1.5  90/06/22  21:52:10  devrcs
 * 	Corrected various paths.
 * 	[90/06/14  17:38:55  tom]
 * 
 * Revision 1.4  90/05/24  23:08:29  devrcs
 * 	added _PATH_DEFPATH _PATH_PTY _PATH_PTYP _PATH_LOGIN
 * 	[90/05/16  13:18:15  jrieden]
 * 
 * Revision 1.3  90/01/02  19:16:15  gm
 * 	Fixes for first snapshot.
 * 
 * Revision 1.2  89/12/26  10:10:43  gm
 * 	New from UCB.
 * 	[89/12/21            gm]
 * 
 * $EndLog$
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
 *	@(#)paths.h	5.7 (Berkeley) 6/24/90
 */

#define	_PATH_BSHELL	"/bin/sh"
#define	_PATH_CONSOLE	"/dev/console"
#define	_PATH_CSHELL	"/bin/csh"
#define	_PATH_DEV	"/dev/"
#define	_PATH_DEVNULL	"/dev/null"
#define	_PATH_DRUM	"/dev/drum"
#define	_PATH_KMEM	"/dev/kmem"
#define _PATH_MAILDIR	"/var/mail"
#define _PATH_MAN       "/usr/man"
#define	_PATH_MEM	"/dev/mem"
#define _PATH_NOLOGIN   "/etc/nologin"
#define	_PATH_SENDMAIL	"/usr/sbin/sendmail"
#define	_PATH_TMP	"/tmp/"
#define	_PATH_TTY	"/dev/tty"
#define	_PATH_UNIX	"/vmunix"
#define _PATH_VARTMP    "/var/tmp"
#define	_PATH_VI	"/usr/bin/vi"
#define _PATH_PTY	"/dev/pty"
#define _PATH_PTYP	"/dev/ptyp"
#define _PATH_LOGIN	"/usr/bin/login"
