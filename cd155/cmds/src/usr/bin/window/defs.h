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
/* @(#)$RCSfile: defs.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:48:07 $ */
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
 *	defs.h	3.18 (Berkeley) 6/29/88
 */

#include "ww.h"
#include <sys/time.h>

#define NWINDOW 9

struct timeval starttime;

struct ww *window[NWINDOW];	/* the windows */
struct ww *selwin;		/* the selected window */
struct ww *lastselwin;		/* the last selected window */
struct ww *cmdwin;		/* the command window */
struct ww *framewin;		/* the window for framing */
struct ww *boxwin;		/* the window for the box */
struct ww *fgwin;		/* the last foreground window */

#define isfg(w)		((w)->ww_order <= fgwin->ww_order)

char *shell[128];		/* the shell argv */
char *shellfile;		/* the shell program */
int nbufline;			/* default buffer size for new windows */
char escapec;			/* the escape character */

	/* flags */
char quit;			/* quit command issued */
char terse;			/* terse mode */
char debug;			/* debug mode */
char incmd;			/* in command mode */

struct ww *getwin();
struct ww *openwin();
struct ww *vtowin();
struct ww *openiwin();

#include        <nl_types.h>
#include        "window_msg.h"
nl_catd catd;
#define MSGSTR(num,str) catgets(catd,MS_WINDOW,num,str)

