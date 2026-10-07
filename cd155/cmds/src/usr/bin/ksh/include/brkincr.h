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
/* @(#)brkincr.h	1.8  com/cmd/ksh/sh,3.1,9013 1/23/90 13:30:02 */
/*
 * COMPONENT_NAME: (CMDKSH) Korn shell
 *
 * FUNCTIONS:
 *
 * ORIGINS: 3, 26, 27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * Copyright (c) 1980 Regents of the University of California.
 * All rights reserved.  The Berkeley software License Agreement
 * specifies the terms and conditions for redistribution.
 */

/*

 *      Copyright (c) 1984, 1985, 1986, 1987, 
 *                  1988, 1989   AT&T
 *      All Rights Reserved

 *      THIS IS UNPUBLISHED PROPRIETARY SOURCE 
 *      CODE OF AT&T.
 *      The copyright notice above does not 
 *      evidence any actual or intended
 *      publication of such source code.

 */
/*
 *	UNIX shell
 *	S. R. Bourne
 *	Rewritten by David Korn
 *
 */

#if u370 || uts
#define BRKINCR		4096
#else
#define BRKINCR		((int)(0400*sizeof(char*)))
#endif	/* u370 */
#ifdef INT16
#define BRKMAX		((unsigned)(077777-BRKINCR))
#else
#define BRKMAX		(2*1024*BRKINCR)
#endif /* INT16 */
#define ERRTRAP		(MAXTRAP-2)
#define DEBUGTRAP	(MAXTRAP-1)

#ifndef SIGCHLD
#   ifdef SIGCLD
#	define SIGCHLD SIGCLD
#   endif
#endif

#define SIGBITS		8

#undef SIGIGNORE
#define SIGIGNORE	1	/* signal handler set to ignore */
#define TRAPSET		2	/* signal received and trap is set */
#define SIGSET		4	/* signal received and trap not set */
#define SIGMOD		8	/* modified signal handler */
#undef SIGCAUGHT
#define SIGCAUGHT	16	/* default action is to catch signal */
#undef SIGOFF
#define SIGOFF		32	/* signal ignored by parent */
#undef SIGDONE
#define SIGDONE		64	/* default action is clean up and exit */
#undef SIGFAULT
#define SIGFAULT	128	/* signal handler set to sh_fault */
#undef SIGFAIL
#define SIGSLOW		256	/* slow signal received */
#define SIGBEGIN	512	/* set during signal initialization */
#define SIGFAIL		0600
#undef SIGFLG
#define SIGFLG		0200
#define EXITMASK	0377

#ifdef _NO_PROTO
    extern char 	*sbrk();
    extern void 	sh_fault();
    extern void 	sh_done();
    extern void 	sh_chktrap();
    extern void 	sh_exit();
    extern void 	sh_addmem();
    extern void 	sh_addblok();
    extern void 	sig_clear();
    extern int		sig_ignore();
    extern void 	sig_init();
    extern void 	sig_ontrap();
    extern void 	sig_reset();
    extern void 	sig_funset();
#else /* ~ _NO_PROTO */
    extern char 	*sbrk(int);
    extern void 	sh_fault(int);
    extern void 	sh_done(int);
    extern void 	sh_chktrap(void);
    extern void 	sh_exit(int);
    extern void 	sh_addmem(int);
    extern void 	sh_addblok(int);
    extern void 	sig_clear(int);
    extern int		sig_ignore(int);
    extern void 	sig_init(void);
    extern void 	sig_ontrap(int);
    extern void 	sig_reset(int);
#endif /* _NO_PROTO */

extern const char		e_space[];
#ifndef	_sys_siglist_
extern const struct sysnod	sig_messages[];
#endif
extern const struct sysnod	sig_names[];
