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
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 *
 *      @(#)ttyent.h    5.1 (Berkeley) 5/30/85
 */
#ifndef _TTYENT_H_
#define _TTYENT_H_

struct	ttyent {		/* see getttyent(3) */
    char *ty_name;		/* terminal device name */
    char *ty_getty;		/* command to execute, usually getty */
    char *ty_type;		/* terminal type for termcap (3X) */
    int	ty_status;		/* status flags (see below for defines) */
    char *ty_window;		/* command to start up window manager */
    char *ty_comment;		/* usually the location of the terminal */
};

#define TTY_ON		0x1	/* enable logins (startup getty) */
#define TTY_SECURE	0x2	/* allow root to login */

#ifdef _NO_PROTO
extern struct ttyent *getttyent();
extern struct ttyent *getttynam();
#if defined(_REENTRANT) || (_THREAD_SAFE)
extern int getttyent_r();
extern int getttynam_r();
#endif
#else
extern struct ttyent *getttyent(void);
extern struct ttyent *getttynam(const char*);
extern void setttyent(void);
extern void endttyent(void);
#if defined(_REENTRANT) || (_THREAD_SAFE)
extern int getttyent_r(struct ttyent *, char *);
extern int getttynam_r(const char *, struct ttyent *, char *);
#endif
#endif /* _NO_PROTO */

#endif /* _TTYENT_H_ */
