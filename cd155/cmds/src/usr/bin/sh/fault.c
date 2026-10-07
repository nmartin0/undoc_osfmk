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
#if !defined(lint) && !defined(_NOIDENT)
static char rcsid[] = "@(#)$RCSfile: fault.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:39:05 $";
#endif
/*
 * COMPONENT_NAME: (CMDSH) Bourne shell and related commands
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
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
 */

#include	"defs.h"
#include        "timeout.h"

extern void	done(int);
extern void	fault(int);

static struct sigaction fault_action	= { fault,	0 , 0 }; /* PTM 9555 */
static struct sigaction done_action	= { done,	0 , 0 }; /* PTM 9555 */
static struct sigaction dfl_action	= { SIG_DFL,	0 , 0 }; /* PTM 9555 */

uchar_t	*trapcom[SIGMAX+1];
BOOL	trapflg[SIGMAX+1];	/*  init to 0  */	/* PTM 9555 */


	/*	Initialize signal handling.
	*	Changed all signal handling to use sigaction call
	*	instead of signal call. PTM 9555
	*/
struct sigaction	*sigact [] = {
		&dfl_action ,		/*  0 */
	        &done_action ,		/*  1 - SIGHUP */
	        &fault_action ,		/*  2 - SIGINT */
	        &fault_action ,		/*  3 - SIGQUIT */
	        &done_action ,		/*  4 - SIGILL */
	        &done_action ,		/*  5 - SIGTRAP */
	        &done_action ,		/*  6 - SIGABRT */
	        &done_action ,		/*  7 - SIGEMT */
	        &done_action ,		/*  8 - SIGFPE */
	        &dfl_action ,		/*  9 - SIGKILL */
	        &done_action ,		/* 10 - SIGBUS */
	        &fault_action ,		/* 11 - SIGSEGV */
	        &done_action ,		/* 12 - SIGSYS */
	        &done_action ,		/* 13 - SIGPIPE */
	        &fault_action ,		/* 14 - SIGALRM */
	        &fault_action ,		/* 15 - SIGTERM */
	        &dfl_action ,		/* 16 - SIGURG */
	        &dfl_action ,		/* 17 - SIGSTOP */
	        &dfl_action ,		/* 18 - SIGTSTP */
	        &dfl_action ,		/* 19 - SIGCONT */
	        &done_action ,		/* 20 - SIGCHLD */
	        &dfl_action ,		/* 21 - SIGTTIN */
	        &dfl_action ,		/* 22 - SIGTTOU */
	        &dfl_action ,		/* 23 - SIGIO */
	        &dfl_action ,		/* 24 - SIGXCPU */
	        &dfl_action ,		/* 25 - SIGXFSZ */
	        &dfl_action ,		/* 26 - */
	        &dfl_action ,		/* 27 - SIGMSG */
	        &dfl_action ,		/* 28 - SIGWINCH */
	        &dfl_action ,		/* 29 - SIGPWR */
	        &done_action ,		/* 30 - SIGUSR1 */
	        &done_action ,		/* 31 - SIGUSR2 */
	        &dfl_action ,		/* 32 - SIGPROF */
	        &dfl_action ,		/* 33 - SIGDANGER */
	        &dfl_action ,		/* 34 - SIGVTALRM */
	        &dfl_action ,		/* 35 - SIGMIGRATE */
	        &dfl_action ,		/* 36 - SIGPRE */
	        &dfl_action ,		/* 37 - */
	        &dfl_action ,		/* 38 - */
	        &dfl_action ,		/* 39 - */
	        &dfl_action ,		/* 40 - */
	        &dfl_action ,		/* 41 - */
	        &dfl_action ,		/* 42 - */
	        &dfl_action ,		/* 43 - */
	        &dfl_action ,		/* 44 - */
	        &dfl_action ,		/* 45 - */
	        &dfl_action ,		/* 46 - */
	        &dfl_action ,		/* 47 - */
	        &dfl_action ,		/* 48 - */
	        &dfl_action ,		/* 49 - */
	        &dfl_action ,		/* 50 - */
	        &dfl_action ,		/* 51 - */
	        &dfl_action ,		/* 52 - */
	        &dfl_action ,		/* 53 - */
	        &dfl_action ,		/* 54 - */
	        &dfl_action ,		/* 55 - */
	        &dfl_action ,		/* 56 - */
	        &dfl_action ,		/* 57 - */
	        &dfl_action ,		/* 58 - */
	        &dfl_action ,		/* 59 - */
	        &fault_action ,		/* 60 - SIGGRANT */
	        &fault_action ,		/* 61 - SIGRETRACT */
	        &fault_action ,		/* 62 - SIGSOUND */
	        &done_action } ;	/* 63 - SIGSAK */


int	trap_waitrc = 0;		/* PTM 5870 */
int	trap_status = 0;		/* PTM 5870 */

/* ========	fault handling routines	   ======== */

extern long mailtime;
extern int mailchk;

void
fault(int sig)
{
	register int	flag;

	if ( sig == SIGCHLD )		/* PTM 5870  - wait for child here */
		trap_waitrc = wait (&trap_status);

	sigaction ( sig, &fault_action, (struct sigaction *)0 ); /* PTM 9555 */
# if !NOBACKUP
	if (sig == SIGSEGV)
	{
		if (setbrk(brkincr) == -1)
			error(MSGSTR(M_NOSPACE,(char *)nospace));
	}
	else if (sig == SIGALRM)
# else
	if (sig == SIGALRM)
# endif
	{
		long int curtime;

		time(&curtime);
		if(mailchk) {
			chkmail();
			mailalarm = 1;
			alarm(mailchk);
			mailtime = curtime;
		}
		if (timecroak && flags&waiting && (curtime >= timecroak))
			done(0);
	}
	else
	{
		flag = (trapcom[sig] ? TRAPSET : SIGSET);
		trapnote |= flag;
		trapflg[sig] |= flag;
		if (sig == SIGINT)
			wasintr++;
	}
}

stdsigs()
{
	setsig(SIGEMT);
	setsig(SIGHUP);
	setsig(SIGINT);
	ignsig(SIGQUIT);
	setsig(SIGILL);
	setsig(SIGTRAP);
	setsig(SIGABRT);
	setsig(SIGFPE);
	setsig(SIGBUS);
# if NOBACKUP
	setsig(SIGSEGV);
# else
	sigaction (SIGSEGV, &fault_action, (struct sigaction *)0);
# endif
        setsig(SIGSYS);
        setsig(SIGPIPE);
        setsig(SIGALRM);
        setsig(SIGTERM);
        ignsig(SIGXFSZ);
        setsig(SIGUSR1);
        setsig(SIGUSR2);
}

ignsig(i)
register int i;
{
	register int    s;

# if NOBACKUP
	if ((s = (signal(i, SIG_IGN) == SIG_IGN)) == 0)
# else
	if (i == SIGSEGV)
	{
		clrsig(i);
#ifdef MSG
		{
		  uchar_t buf[NL_TEXTMAX];
		  strcpy ((char *)buf, MSGSTR(M_BADTRAP, (char *)badtrap));
		  failed(buf, MSGSTR(M_TRAP,"cannot trap 11"));
		}
#else
		failed(badtrap, "cannot trap 11");
#endif
	}
	else if ((s = (signal(i, SIG_IGN) == SIG_IGN)) == 0)
# endif
	{
		trapflg[i] |= SIGMOD;
	}
	return(s);
}

getsig(n)
{
	register int	i;

	if (trapflg[i = n] & SIGMOD || ignsig(i) == 0)
		sigaction(i,&fault_action,(struct sigaction *)0); /* PTM 9555 */
}


setsig(n)
{
	register int	i;

	if (ignsig(i = n) == 0)
		sigaction ( i, sigact[i], (struct sigaction *)0 );/* PTM 9555 */
}

oldsigs()
{
	register int	i;
	register uchar_t	*t;

	i = SIGMAX;
	while (i--)
	{
		t = trapcom[i];
		if (t == 0 || *t)
			clrsig(i);
		trapflg[i] = 0;
	}
	trapnote = 0;
}

clrsig(i)
int	i;
{
	free(trapcom[i]);
	trapcom[i] = 0;
	if (trapflg[i] & SIGMOD)
	{
		trapflg[i] &= ~SIGMOD;
		sigaction(i, sigact[i], (struct sigaction *)0);	/* PTM 9555 */
	}
}

/*
 * check for traps
 */
chktrap()
{
	register int	i = SIGMAX;
	register uchar_t	*t;

	trapnote &= ~TRAPSET;
	while (--i)
	{
		if (trapflg[i] & TRAPSET)
		{
			trapflg[i] &= ~TRAPSET;
			if (t = trapcom[i])
			{
				int	savxit = exitval;

				execexp(t, 0);
				exitval = savxit;
				exitset();
			}
		}
	}
}
