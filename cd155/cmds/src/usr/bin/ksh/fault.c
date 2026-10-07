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
static char rcsid[] = "@(#)$RCSfile: fault.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:18 $";
#endif
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
 *
 * Copyright 1976, Bell Telephone Laboratories, Inc.
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
 * UNIX shell
 *
 * S. R. Bourne
 * Rewritten by David Korn
 * AT&T Bell Laboratories
 *
 */

#include	"defs.h"
#include	"jobs.h"
#include	"sym.h"
#include	"timeout.h"

#include	<signal.h>

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

void	(*sh_signal( int sig, void (*func)(int))) (int);
void	sh_fault(int sig);
void	sh_done(int sig);

/* ========	fault handling routines	   ======== */

#ifdef  KSH_88D
#ifndef JOBS
#       undef   SIGCHLD
#endif /* JOBS */
#endif /* KSH_88D */

void
sh_fault(int sig)
{
	register int 	flag;
#ifdef  KSH_88D
#ifdef OLDTERMIO
        /* This .1 sec delay eliminates break key problems on 3b consoles */
#   ifdef _poll_
        if(sig==2)
                poll("",0,100);
#   endif /* _poll_ */
#endif /* OLDTERMIO */
#endif /* KSH_88D */
#ifdef	SIGCHLD
	if(sig==SIGCHLD)
	{
		job.waitsafe++;
		if(st.trapcom[SIGCHLD])
		{
			sh.trapnote |= SIGSLOW;
#ifndef SIG_NORESTART
			if(st.intfn)
			{
				sigrelease(sig);
				(*st.intfn)();
			}
#endif	/* SIG_NORESTART */
		}
		return;
	}
#endif	/* SIGCHLD */
	sh_signal(sig, (void (*)(int))sh_fault);
	if(sig==SIGALRM)
	{
		if((st.states&WAITING) && sh_timeout>0)
		{
			if(st.states&GRACE)
			{
				/* force exit */
					st.states &= ~GRACE;
					st.states |= FORKED;
					sh_fail(MSGSTR(E_TIMEOUT, (char *)e_timeout),NIL); /*MSG*/
			}
			else
			{
				st.states |= GRACE;
				alarm((unsigned)TGRACE);
				p_str(MSGSTR(E_TIMEWARN, (char *)e_timewarn),NL); /*MSG*/
				p_flush();
			}
		}
	}
	else
	{
		if(st.trapcom[sig])
			flag = TRAPSET;
		else
		{
			sh.lastsig = sig;
			flag = SIGSET;
		}
		sh.trapnote |= flag;
		st.trapflg[sig] |= flag;
		if(sig <= SIGQUIT)
			sh.trapnote |= SIGSLOW;
	}
#ifndef SIG_NORESTART
	/* This is needed because interrupted reads automatically restart */
	if(st.intfn)
	{
		sigrelease(sig);
		(*st.intfn)();
	}
#endif	/* SIG_NORESTART */
}

void
sig_init(void)
{
	register int i;
	register int n;
	register const struct sysnod	*syscan = sig_names;
	sig_begin();
	while(*syscan->sysnam)
	{
		n = syscan->sysval;
		i = n&((1<<SIGBITS)-1);
		n >>= SIGBITS;
		st.trapflg[--i] = (n&~SIGIGNORE);
		if(n&SIGFAULT)
			sh_signal(i,(VOID(*)(int))sh_fault);
		else if(n&SIGIGNORE)
			sig_ignore(i);
		else if(n&SIGCAUGHT)
			sig_ontrap(i);
		else if(n&SIGDONE)
		{
			sh.trapnote |= SIGBEGIN;
			if(sh_signal(i,(VOID(*)(int))sh_done)==SIG_IGN)
			{
				sig_ignore(i);
				st.trapflg[i] = SIGOFF;
			}
			else
				st.trapflg[i] = SIGMOD|SIGDONE;
			sh.trapnote &= ~SIGBEGIN;
		}
		syscan++;
	}
#ifndef	_sys_siglist_
        for(syscan = sig_messages; n=syscan->sysval; syscan++)
	{
#ifdef  KSH_88D
                if (n > NSIG+1)
                        continue;
#endif /* KSH_88D */
		if(*syscan->sysnam) {
#ifdef MSG
			char *xm, *xs;

			/* must malloc space as NLcatgets()
			   uses a static buffer for message */
			xm = MSGSTR(n-1, syscan->sysnam);
			xs = malloc((size_t)(strlen(xm)+1));
			strcpy(xs, xm);
			sh.sigmsg[n-1] = xs;
#else
			sh.sigmsg[n-1] = (char *)syscan->sysnam;
#endif /*MSG*/
		}
	}
#endif	/* _sys_siglist_ */
}

/*
 * set signal n to ignore
 * returns 1 if signal was already ignored, 0 otherwise
 */
int
sig_ignore(int n)
{
	if(n < MAXTRAP-1 && !(st.trapflg[n]&SIGIGNORE))
	{
		if(sh_signal(n,SIG_IGN) != SIG_IGN)
		{
			st.trapflg[n] |= SIGIGNORE;
#ifdef  KSH_88D
                        st.trapflg[n] &= ~SIGFAULT;
#endif /* KSH_88D */
			return(0);
		}
		st.trapflg[n] = SIGOFF;
	}
	return(1);
}

/*
 * Turn on trap handler for signal <n>
 */

void
sig_ontrap(int n)
{
	register int flag;
	if(n==DEBUGTRAP)
		sh.trapnote |= TRAPSET;
	/* don't do anything if already set or off by parent */
	else if(!(st.trapflg[n]&(SIGFAULT|SIGOFF)))
	{
		flag = st.trapflg[n];
		if(sh_signal(n,(VOID(*)(int))sh_fault)==SIG_IGN) 
		{
			/* has it been set to ignore by shell */
			if(flag&SIGIGNORE)
				flag |= SIGFAULT;
			else
			{
				/* It ignored already, keep it ignored */ 
				sig_ignore(n);
				flag = SIGOFF;
			}
		}
		else
			flag |= SIGFAULT;
		flag &= ~(SIGSET|TRAPSET|SIGIGNORE|SIGMOD);
		st.trapflg[n] = flag;
	}
}

/*
 * Restore to default signals
 * Do not free the trap strings if flag is non-zero
 */

void
sig_reset(int flag)
{
	register int 	i;
	register char *t;
	i=MAXTRAP;
	while(i--)
	{
		t=st.trapcom[i];
		if(t==0 || *t)
		{
			if(flag)
				st.trapcom[i] = 0; /* don't free the traps */
			sig_clear(i);
		}
		st.trapflg[i] &= ~(TRAPSET|SIGSET);
	}
	sh.trapnote=0;
}

/*
 * reset traps at start of function execution
 * keep track of which traps are caught by caller in case they are modified
 * flag==0 before function, flag==1 after function
 */

void
sig_funset(flag)
{
	register int 	i;
	register char 	*tp;

	i=MAXTRAP;
	while(i--)
	{
                tp = st.trapcom[i];
		if(flag==0)
		{
                        if(tp && *tp==0)
                                st.trapflg[i] = SIGOFF;
                        else
                        {
                                if(tp)
                                        st.trapflg[i] |= SIGCAUGHT;
				st.trapflg[i] &= ~(TRAPSET|SIGSET);
			}
			st.trapcom[i] = 0;
		}
		else if(tp)
			sig_clear(i);
	}
	sh.trapnote = 0;
}

/*
 * free up trap if set and restore signal handler if modified
 */

void
sig_clear(int n)
{
	register int flag = st.trapflg[n];
	register char *t;
	if(t=st.trapcom[n])
	{
		free((void *)t);
		st.trapcom[n]=0;
		flag &= ~(TRAPSET|SIGSET);
	}
	if(flag&(SIGFAULT|SIGMOD|SIGIGNORE))
	{
		if(flag&SIGCAUGHT)
		{
			if(flag&(SIGMOD|SIGIGNORE))
				sh_signal(n, (void (*)(int))sh_fault);
		}
		else if((flag&SIGDONE))
		{
			if(t || (flag&SIGIGNORE))
				sh_signal(n, (void (*)(int))sh_done);
		}
		else
			 sh_signal(n, SIG_DFL);
		flag &= ~(SIGMOD|SIGFAULT|SIGIGNORE);
		if(flag&SIGCAUGHT)
			flag |= SIGFAULT;
		else if(flag&SIGDONE)
			flag |= SIGMOD;
	}
	st.trapflg[n] = flag;
}


/*
 * check for traps
 */

void
sh_chktrap(void)
{
	register int 	i=MAXTRAP;
	register char *t;
#ifdef JOBS
	if(job.waitsafe)
		job_wait(0);
#endif /* JOBS */
#ifdef  KSH_88D
        /* process later if doing command substitution */
        if(st.subflag)
                return;
#endif /* KSH_88D */
	sh.trapnote &= ~(TRAPSET|SIGSLOW);
	if((st.states&ERRFLG) && sh.exitval)
	{
		if(st.trapcom[ERRTRAP])
			st.trapflg[ERRTRAP] = TRAPSET;
		if(is_option(ERRFLG))
			sh_exit(sh.exitval);
	}
	while(--i)
	{
		if(st.trapflg[i]&TRAPSET)
		{
			st.trapflg[i] &= ~TRAPSET;
			if(t=st.trapcom[i])
			{
				int savxit=sh.exitval;
				sh_eval(t);
				p_flush();
				sh.exitval=savxit;
				exitset();
			}
		}
	}
	if(st.trapcom[DEBUGTRAP])
	{
		st.trapflg[DEBUGTRAP] |= TRAPSET;
		sh.trapnote |= TRAPSET;
	}
}

	/*	PTM 21792
	*	change from old style signals (call to signal) to
	*	call sigaction
	*/

void (*sh_signal( int sig, void (*func)(int))) (int)
{
	struct sigaction	act, oact;

	sigaction(sig, (struct sigaction *)NULL, &act);	/* get current signal */
	act.sa_handler = func;			/* set new handler */
	switch (sig) {
	case SIGINT:
		act.sa_flags &= ~SA_RESTART;
		break;
	default:
		break;
	}
	sigaction(sig, &act, &oact);
	return(oact.sa_handler);		/* return old signal handler */
}
