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
static char rcsid[] = "@(#)$RCSfile: error.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:16 $";
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

#include	<sys/ioctl.h>	/*  PTM 14811 - change to POSIX  */
#include	"defs.h"
#include	"jobs.h"
#include	"history.h"


#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/* These routines are used by this module but defined elsewhere */
extern void	name_unscope();
extern void	rm_files();
#ifdef VFORK
    extern void	vfork_restore();
#endif	/* VFORK */

/* ========	error handling	======== */

	/* Find out if it is time to go away.
	 * `trapnote' is set to SIGSET when fault is seen and
	 * no trap has been set.
	 */

/*
 *  This routine is called when fatal errors are encountered
 *  A message is printed out and the shell tries to exit
 */

void sh_fail(const char *s1,const char *s2)
{
	mac_check();
	p_setout(ERRIO);
	p_prp(s1);
	if(s2)
	{
		p_str(e_colon,0); /*NOTX*/
		p_str(s2,NL);
	}
	else
		newline();
	sh_exit(ERROR);
}

/* Arrive here from `FATAL' errors
 *  a) exit command,
 *  b) default trap,
 *  c) fault with no trap set.
 *
 * Action is to return to command level or exit.
 */

void sh_exit(int xno)
{
	register unsigned state=(st.states&~(ERRFLG|MONITOR));
	sh.exitval=xno;
	if(xno==SIGFAIL)
		sh.exitval |= sh.lastsig;
#ifdef  KSH_88D
        sh.un.com = 0;
        if( !(state&FORKED) && (state&(BUILTIN|LASTPIPE))) {
#if     VSH || ESH
                tty_cooked (-1);
#endif
                io_clear (sh.savio);
                longjmp(*sh.freturn,1);
        }
#else
	if( (!(state&FORKED) && (state&(BUILTIN|LASTPIPE))) )
		longjmp(*sh.freturn,1);
#endif /* KSH_88D */
	state |= is_option(ERRFLG|MONITOR);
	if(!(state&(PROFILE|PROMPT|FUNCTION)) || (state&(ERRFLG|FORKED)))
	{
		st.states = state;
#ifdef  KSH_88D
                st.peekn = 0;
#endif /* KSH_88D */
		sh_done(0);
	}
	else
	{
		if(!(state&FUNCTION))
		{
			p_flush();
			name_unscope();
			arg_clear();
			io_clear((struct fileblk*)0);
			io_restore(0);
			if(st.standin)
			{
				*st.standin->last = 0;
				/* flush out input buffer */
				while(finbuff(st.standin)>0)
					io_readc();
			}
		}
#ifdef VFORK
		vfork_restore();
#endif	/* VFORK */
		st.execbrk = st.breakcnt = 0;
		st.exec_flag = st.subflag = 0;
		st.dot_depth = 0;
		hist_flush();
		state &= ~(FUNCTION|FIXFLG|RWAIT|PROMPT|READPR|MONITOR|BUILTIN|
			LASTPIPE|VFORKED|GRACE);
		state |= is_option(INTFLG|READPR|MONITOR);
		job.pipeflag = 0;
		st.states = state;
		longjmp(*sh.freturn,1);
	}
}

/*
 * This is the exit routine for the shell
 */

void sh_done(int sig)
{
	register char *t;
	register int savxit = sh.exitval;
	if(sh.trapnote&SIGBEGIN)
		return;
	if(t=st.trapcom[0])
	{
		st.trapcom[0]=0; /*should free but not long */
		sh_eval(t);
	}
	else
	{
		/* avoid recursive call for set -e */
		st.states &= ~ERRFLG;
		sh_chktrap();
	}
	io_rmtemp((struct ionod*)0);
#ifdef ACCT
	doacct();
#endif	/* ACCT */
#if VSH || ESH
	if(is_option(EMACS|EDITVI|GMACS) && st.standin && 
		(st.standin->flag&IOEDIT))
		tty_cooked(ERRIO);
#endif
	if(st.states&RM_TMP)
	/* clean up all temp files */
		rm_files(io_tmpname);
	p_flush();
#ifdef JOBS
#ifdef  KSH_88D
        if(sig==SIGHUP || (is_option(INTFLG)&&sh.login_sh))
#else
        if(sig==SIGHUP || (is_option(INTFLG)&&(getppid()==1)))
#endif /* KSH_88D */
		job_terminate();
#endif	/* JOBS */
	if(sig)
	{
		/* generate fault termination code */
		sh_signal(sig,SIG_DFL);
		sigrelease(sig);
		kill(getpid(),sig);
		pause();
	}
#ifdef  KSH_88D
        job_close ();
#endif /* KSH_88D */
	io_sync();


	_exit(savxit&EXITMASK);
}

