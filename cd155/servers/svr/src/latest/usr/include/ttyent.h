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
 * $Log: ttyent.h,v $
 * Revision 1.3  1994/11/18  21:11:07  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/11/30  23:17:32  dleslie
 * Copy of NX branch back into main trunk
 *
 * Revision 1.1.2.1  1992/11/06  00:00:23  dleslie
 * Local changes for NX through noon, November 5, 1992.
 *
 * Revision 4.1  1992/11/04  01:19:17  cfj
 * Bump major revision number.
 *
 * Revision 1.1  1992/07/20  16:21:44  nandy
 * Initial revision
 *
 * Revision 2.6  92/04/13  14:39:58  stans
 * "OSF-distribution-ad8.5.2"
 * 
 * Revision 2.6  90/10/07  20:38:26  devrcs
 * 	Added EndLog Marker.
 * 	[90/09/28  21:01:51  gm]
 * 
 * Revision 2.5  90/08/09  14:28:50  devrcs
 * 	Changes for reentrant C library
 * 	[90/08/02  09:58:11  sp]
 * 
 * Revision 2.4  90/03/13  21:23:54  mbrown
 * 	AIX merge first cut.
 * 	[90/02/12  18:03:47  tom]
 * 
 * 	Fixes for first snapshot.
 * 
 * Revision 2.2  89/12/26  10:11:53  gm
 * 	Added function prototype declarations.
 * 	[89/06/30  12:03:21  bww]
 * 
 * $EndLog$
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
