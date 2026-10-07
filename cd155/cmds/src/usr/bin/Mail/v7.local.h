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
 * (c) Copyright 1990, 1991, OPEN SOFTWARE FOUNDATION, INC.
 * ALL RIGHTS RESERVED
 */
/*
 * OSF/1 Release 1.0.3
 */
/* 
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: v7.local.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:18:33 $";
#endif
 */
/* 
 * COMPONENT_NAME: CMDMAILX v7.local.h
 * 
 * FUNCTIONS: 
 *
 * ORIGINS: 10  26  27 
 *
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 * Licensed Materials - Property of IBM
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *      v7.local.h  5.2 (Berkeley) 9/19/85
 */

/*
 * Declarations and constants specific to an installation.
 *
 * Vax/Unix version 7.
 */

#define	GETHOST				/* System has gethostname syscall */
#ifdef	GETHOST
#define	LOCAL		EMPTYID		/* Dynamically determined local host */
#else
#define	LOCAL		'Self'		/* Local host id */
#endif	GETHOST

#define MAILBOX		"/usr/spool/mail"	/* system mailbox dir */
#define	MAIL		"/usr/sbin/sendmail"	/* Name of mail sender */
#define SENDMAIL	"/usr/sbin/sendmail"
					/* Name of classy mail deliverer */
#define	EDITOR		"/usr/bin/ex"	/* Name of text editor */
#define	VISUAL		"/usr/bin/vi"	/* Name of display editor */
#define	PAGE		"/usr/bin/pg"	/* Standard output pager */
#define	SHELL		"/bin/sh"	/* Standard shell */
#define	POSTAGE		"/var/adm/maillog"
					/* Where to audit mail sending */
#define	UIDMASK		0177777		/* Significant uid bits */
#define	MASTER		"/usr/share/lib/Mail.rc"
#define	APPEND				/* New mail goes to end of mailbox */
#define CANLOCK				/* Locking protocol actually works */
#define	UTIME				/* System implements utime(2) */

