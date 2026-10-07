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
static char rcsid[] = "@(#)$RCSfile: jobs.c,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:27:21 $";
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
 *  Job control for UNIX Shell
 *
 *   David Korn
 *   AT&T Bell Laboratories
 *   Room 3C-526B
 *   Murray Hill, N. J. 07974
 *   Tel. x7975
 *
 *  Written October, 1982
 *  Rewritten April, 1988
 */

#include	<errno.h>
#include	<sys/ioctl.h>	/*  added for posix  */
#include	"defs.h"
#include	"jobs.h"
#include	"history.h"

				/*  PTM 20569 - fix to sh -c & problem  */
#undef	NTTYDISC		/*  this defines bsd new disciplines  */

#ifdef _sys_wait_
#  include	<sys/wait.h>
#  undef _wait_
#  define _wait_ 1
#else
#  ifdef _wait_
#     include	<wait.h>
#  endif /* _wait_*/
#endif	/* _sys_wait_ */

#ifdef _sys_sigaction_
#  include	<sys/sigaction.h>
#endif /* _sys_sigaction_ */

#ifdef WAIT1ARG
#   undef WNOHANG
#endif /* WAIT1ARG */

char    debug[100];
static struct process	*job_bypid();
static struct process	*job_byjid();
#ifndef	_sys_siglist_
static char		*job_sigmsg();	/*  use signal messages from job_sigmsg */
#else
extern	char		*sys_siglist[];	/*  use signal messages from libc */
#endif
static int		job_alloc();
static struct process	*job_bystring();
static void 		job_free();
static int		job_unpost();
static void		job_unlink();
static struct process	*freelist;
static char		beenhere;
static struct process	dummy;

#ifdef JOBS
   static void			job_set();
   static void			job_reset();
   static struct process	*job_byname();
   static struct termios	my_stty;  /* terminal state for shell */
   static char			*job_string;
#endif /* JOBS */

#ifdef SIGTSTP
    static void		job_unstop();
    static void		job_fgrp();

#ifndef	_termios_

#	ifdef TIOCGPGRP
#ifdef  KSH_88D
           static int _i_;
#endif /* KSH_88D */
#	   define tcgetpgrp(a) (ioctl(a, TIOCGPGRP, &job.mytgid)>=0?job.mytgid:-1)	
#	endif /* TIOCGPGRP */
	int tcsetpgrp(fd,pgrp)
	int fd;
	pid_t pgrp;
	{
#	ifdef TIOCGPGRP
		return(ioctl(fd, TIOCSPGRP, &pgrp));	
#	else
		return(-1);
#	endif /* TIOCGPGRP */
	}

#endif	/* _termios_ */

#else
#   define job_unstop(pw)
#endif /* SIGTSTP */

#ifdef  OTTYDISC
#       undef   NTTYDISC
#endif /* OTTYDISC */

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

#ifdef JOBS
#ifdef SIGTSTP
/*
 * initialize job control
 * if lflag is set the switching driver message will not print
 */

void job_init(int lflag)
{
	register int ntry = 0;

	/* use new line discipline when available */
#   ifdef NTTYDISC
#     ifdef FIOLOOKLD
	if((job.linedisc = ioctl (JOBTTY, FIOLOOKLD, 0)) <0)
#     else
	if(ioctl(JOBTTY,TIOCGETD,&job.linedisc) !=0)
#     endif /* FIOLOOKLD */
		return;
	if(job.linedisc!=NTTYDISC && job.linedisc!=OTTYDISC)
	{
		/* no job control when running with MPX */
#     ifdef VSH
		on_option(VIRAW);
#     endif /* VSH */
		return;
	}
	if(job.linedisc==NTTYDISC)
	{
		job.linedisc = -1;
	}
#   endif /* NTTYDISC */

	if((job.mypgid=getpgid(0))<=0)
		return;

	/* wait until we are in the foreground */
#if     (!defined(KSH_88D) || (!defined(WAIT1ARG) && defined(KSH_88D)))
	while((job.mytgid=tcgetpgrp(JOBTTY)) != job.mypgid)
	{
		if(job.mytgid == -1)
			return;

		/* Stop this shell until continued */
		sh_signal(SIGTTIN,SIG_DFL);
		kill(sh.pid,SIGTTIN);
		/* resumes here after continue tries again */
		if(ntry++ > MAXTRY)
		{
			p_str(MSGSTR(E_NO_START, (char *)e_no_start),NL); /*MSG*/
			return;
		}
	}
#endif /* !WAIT1ARG */


#   ifdef       WAIT1ARG
#       undef   CNSUP
#   else
	/* make sure that we are a process group leader */
	setpgid(0,sh.pid);
	sh_signal(SIGTTOU,SIG_IGN);
	sh_signal(SIGTSTP,SIG_IGN);
	sh_signal(SIGTTIN,SIG_IGN);
	tcsetpgrp(JOBTTY,sh.pid);
#   ifdef CNSUSP
	/* set the switch character */
	tty_get(JOBTTY,&my_stty);
	job.suspend = my_stty.c_cc[VSUSP];
	if(job.suspend == CNSUSP)
	{
		my_stty.c_cc[VSUSP] = CNSUSP;
                tty_set(JOBTTY,TCSAFLUSH,&my_stty);
	}
#   endif /* CNSUSP */
	on_option(MONITOR);
	job.jobcontrol++;
#endif /* WAIT1ARG */
	return;
}


/*
 * see if there are any stopped jobs
 * restore tty driver and pgrp
 */

int job_close(void)
{
	register struct process *pw = job.pwlist;
	register int count = 0;
	if(!job.jobcontrol)
		return(0);
	for(;pw;pw=pw->p_nxtjob)
	{
		if(!(pw->p_flag&P_STOPPED))
			continue;
		if(beenhere)
			killpg(pw->p_pgrp,SIGTERM);
		count++;
	}
	if(beenhere++ == 0 && job.pwlist)
	{
		p_setout(ERRIO);
		if(count)
		{
			p_str(MSGSTR(E_TERMINATE, (char *)e_terminate),NL); /*MSG*/
			return(-1);
		}
		else if(sh.login_sh)
		{
			p_str(MSGSTR(E_RUNNINGJ, (char *)e_runningj),NL); /*MSG*/
			return(-1);
		}
	}
#ifndef WAIT1ARG
	setpgid(0,job.mypgid);
	tcsetpgrp(JOBTTY,job.mypgid);
#endif /* WAIT1ARG */
#   ifdef NTTYDISC
	if(job.linedisc>=0)
	{
		/* restore old line discipline */
#	ifdef FIOPUSHLD
		tty_get(JOBTTY,&my_stty);
		if (ioctl(JOBTTY, FIOPOPLD, 0) < 0)
			return(0);
		if (ioctl(JOBTTY, FIOPUSHLD, &job.linedisc) < 0)
		{
			job.linedisc = NTTYDISC;
			ioctl(JOBTTY, FIOPUSHLD, &job.linedisc);
			return(0);
		}
                tty_set(JOBTTY,TCSAFLUSH,&my_stty);
#	else
		if(ioctl(JOBTTY,TIOCSETD,&job.linedisc) !=0)
			return(0);
#	endif /* FIOPUSHLD */
		p_str(MSGSTR(E_OLDTTY, (char *)e_oldtty),NL); /*MSG*/
	}
#   endif /* NTTYDISC */
#   ifdef CNSUSP
	if(job.suspend==CNSUSP)
	{
		tty_get(JOBTTY,&my_stty);
		my_stty.c_cc[VSUSP] = CNSUSP;
                tty_set(JOBTTY,TCSAFLUSH,&my_stty);
	}
#   endif /* CNSUSP */
	job.jobcontrol = 0;
	return(0);
}


/*
 * Set the ttygroup id to a previously running job
 */
#else
void job_init()
{
}

int job_close()
{
#ifdef  KSH_88D
        if(!(st.states&MONITOR) || (st.states&FORKED))
#else
        if(!(st.states&MONITOR))
#endif /* KSH_88D */
		return(0);
	if(beenhere++ == 0 && job.pwlist && sh.login_sh)
	{
		p_str(MSGSTR(E_RUNNINGJ, (char *)e_runningj),NL); /*MSG*/
		return(-1);
	}
	return(0);
}
#endif	/* SIGTSTP */

static void job_set(pw)
register struct process *pw;
{
	/* save current terminal state */
	tty_get(JOBTTY,&my_stty);
#ifdef TCSAFLUSH
	if(pw->p_flag&P_STTY)
	{
		/* restore terminal state for job */
		tty_set(JOBTTY,TCSAFLUSH,&pw->p_stty);
	}
#endif /* TCSAFLUSH */
#ifdef SIGTSTP
	tcsetpgrp(JOBTTY,pw->p_fgrp);
	/* if job is stopped, resume it in the background */
	job_unstop(pw);
#endif	/* SIGTSTP */
}

static void	job_reset(pw)
register struct process *pw;
{
	/* save the terminal state for current job */
#ifdef SIGTSTP
	job_fgrp(pw,tcgetpgrp(JOBTTY));
	if(tcsetpgrp(JOBTTY,sh.pid) !=0)
		return;
#endif	/* SIGTSTP */
	if(pw && (pw->p_flag&P_SIGNALLED))
	{
		if(tty_get(JOBTTY,&pw->p_stty) == 0)
			pw->p_flag |= P_STTY;
		/* restore terminal state for job */
#ifdef TCSAFLUSH
		tty_set(JOBTTY,TCSAFLUSH,&my_stty);
#endif
	}
	beenhere = 0;
}
#endif  /* JOBS */

#ifdef  JOBS

/*
 * execute function <fun> for each job
 */

int job_walk(int (*fun)(struct process*,int), int arg, char *jobs[])
{
	register struct process *pw = job.pwlist;
	register int r = 0;
	register char *job;
	register struct process *px;
	job_string = 0;
	if(jobs==0)
	{
		/* do all jobs */
		for(;pw;pw=px)
		{
			px = pw->p_nxtjob;
			if((*fun)(pw,arg))
				r = 2;
		}
	}
	else if(*jobs==0)	/* current job */
	{
		/* skip over non-stop jobs */
		while(pw && pw->p_pgrp==0)
			pw = pw->p_nxtjob;
		if((*fun)(pw,arg))
			r = 2;
	}
	else while(job = *jobs++)
	{
		job_string = job;
		if(*job==0)
			sh_fail(job_string,MSGSTR(E_JOBUSAGE, (char *)e_jobusage)); /*MSG*/
		if(*job == '%')
			pw = job_bystring(job);
		else
		{
			int pid = atoi(job);
			if(pid<0)
				job++;
			while(isdigit(*job))
				job++;
			if(*job)
				sh_fail(job_string,MSGSTR(E_JOBUSAGE, (char *)e_jobusage)); /*MSG*/
			if(!(pw = job_bypid(pid)))
			{
				pw = &dummy;
				pw->p_pid = pid;
				pw->p_pgrp = pid;
			}
			pw->p_flag |= P_BYNUM;
		}
		if((*fun)(pw,arg))
			r = 2;
		if(pw)
			pw->p_flag &= ~P_BYNUM;
	}
	return(r);
}

/*
 * list the given job
 * flag L_FLAG for long listing
 * flag N_FLAG for list only jobs marked for notification
 * flag P_FLAG for process id(s) only
 */

int job_list(struct process *pw, int flag)
{
	register struct process *px = pw;
	register int  n;
	register const char *msg;
	static char m[2];
	int msize;
	if(pw==0)
		return(1);
	if(job.pwlist==NULL)
		return(0);
	if(job.waitsafe)
		job_wait((pid_t)0);
	if((flag&N_FLAG) && (!(px->p_flag&P_NOTIFY)||px->p_pgrp==0))
		return(0);
#ifndef KSH_88D
        p_setout(st.standout);
#endif /* !KSH_88D */
	if((flag&P_FLAG))
	{
		p_num(px->p_pgrp,NL);
		return(0);
	}
	n = px->p_job;
	p_sub(n,' ');
	if(px==job.pwlist)
		m[0] = '+';
	else if(px==job.pwlist->p_nxtjob)
		m[0] = '-';
	else
		m[0] = ' ';
	p_str(m,' ');
	do
	{
		n = 0;
		if(flag&L_FLAG)
			p_num(px->p_pid,'\t');
		if(px->p_flag&P_SIGNALLED)
#ifndef	_sys_siglist_
			msg = job_sigmsg((int)(px->p_exit));
#else
			msg = sys_siglist[(int)(px->p_exit)];
#endif	/* _sys_siglist_ */
		else if(px->p_flag&P_NOTIFY)
		{
			msg = MSGSTR(E_DONE, (char *)e_Done); /*MSG*/
			n = px->p_exit;
		}
		else
			msg = MSGSTR(E_RUNNING, (char *)e_Running); /*MSG*/
		p_str(msg,0);
		msize = strlen(msg);
		if(n)
		{
			p_str(e_nullstr,'(');
			p_num((int)n,')');
			msize += (3+(n>10)+(n>100));
		}
		if(px->p_flag&P_COREDUMP)
		{
			p_str(MSGSTR(E_COREDUMP, (char *)e_coredump),0); /*MSG*/
			msize += strlen(MSGSTR(E_COREDUMP, (char *)e_coredump)); /*MSG*/
		}
		p_nchr(SP,MAXMSG-msize);
		if(flag&L_FLAG)
			px = px->p_nxtproc;
		else
			px = NULL;
		if(px==NULL)
			hist_list(pw->p_name,0,";");
		else
			p_str(e_nlspace,0); /*NOTX*/
	}
	while(px);
	px = pw;
	if(px->p_flag&P_DONE)
	{
		if(!job_unpost(px))
			px->p_flag &= ~P_NOTIFY;
	}
        else if(px->p_flag&P_STOPPED)
                px->p_flag &= ~P_NOTIFY;
	return(0);
}

/*
 * Kill a job or process
 */

int job_kill(struct process *pw, int sig)
{
	register pid_t pid;
        register int r=0;
#ifdef  SIGTSTP
        int     send_sigcont = (sig == SIGTERM || sig == SIGHUP);
#else
        define  send_sigcont    1
#endif /* SIGTSTP */
	const char *msg;
	errno = 0;
	if(pw==0)
		goto error;
	pid = pw->p_pid;
	if(pw->p_flag&P_BYNUM)
	{
#ifdef SIGTSTP
		if(sig==SIGSTOP && pid==sh.pid && sh.ppid==1)
		{
			/* can't stop login shell */
			errno = EPERM;
			r = -1;
		}
		else
		{
				/*	PTM 39983
				*	changed kill to only send SIGCONT
				*	when signal is SIGTERM or SIGHUP
				*/
                        if(pid>=0) {
                                if (((r = kill ( pid, sig)) >= 0) &&
                                    pw->p_flag&P_STOPPED && send_sigcont){
					pw->p_flag &= ~(P_STOPPED|P_SIGNALLED);
					kill(pid,SIGCONT);
				}
			}
			else
                                if (((r = killpg ( -pid, sig)) >= 0) &&
                                    pw->p_flag&P_STOPPED && send_sigcont){
                                        pw->p_flag &= ~(P_STOPPED|P_SIGNALLED);
                                        killpg ( -pid, SIGCONT );
				}
		}
#else
		if(pid>=0)
			r = kill(pid,sig);
		else
			r = killpg(-pid,sig);
#endif	/* SIGTSTP */
	}
	else
	{
		job_unstop(pw);
		if(pid = pw->p_pgrp)
		{
			r = killpg(pid,sig);
                        if ( r >= 0 && send_sigcont )
                                job_unstop(pw);
			if(r>=0)
#ifdef  KSH_88D
#       ifdef   _poll_
                                poll ("", 0, 100);
#       else
                                time((time_t *)0); /* delay a little for SIGCLD
*/
#       endif /* _pool_ */
#else
                                time((time_t *)0); /* delay a little for SIGCLD
*/
#endif /* KSH_88D */
		}
                for (; pw && pw->p_pgrp==0 && r>=0; pw = pw->p_nxtproc)
                        if (((r = kill ( pw->p_pid, sig)) >= 0) &&
                            pw->p_flag&P_STOPPED && send_sigcont) {
                                pw->p_flag &= ~(P_STOPPED|P_SIGNALLED);
                                kill ( pw->p_pid, SIGCONT );
			}

	}
	if(r<0 && job_string)
	{
	error:
		p_setout(ERRIO);
		p_str(MSGSTR(E_KILLCOLON, (char *)e_killcolon),0); /*MSG*/
		if(pw && pw->p_flag&P_BYNUM)
			msg = MSGSTR(E_NO_PROC, (char *)e_no_proc); /*MSG*/
		else
			msg = MSGSTR(E_NO_JOB, (char *)e_no_job); /*MSG*/
		p_str(job_string,0);
		p_str(e_colon,0); /*NOTX*/
		if(errno == EPERM)
			msg = MSGSTR(E_ACCESS, (char *)e_access); /*MSG*/
		p_str(msg,NL);
		r = 2;
	}
	return(r);
}

/*
 * Get process structure from first letters of jobname
 *
 */

static struct process *job_byname(name)
char *name;
{
	register struct process *pw = job.pwlist;
	register struct process *pz = 0;
	register int flag = 0;
	register char *cp = name;
	if(hist_ptr==NULL)
		return(NULL);
	if(*cp=='?')
		(cp++,flag++);
	for(;pw;pw=pw->p_nxtjob)
	{
		if(hist_match(pw->p_name,cp,flag)>=0)
		{
			if(pz)
				sh_fail(name-1,MSGSTR(E_AMBIGUOUS, (char *)e_ambiguous)); /*MSG*/
			pz = pw;
		}
	}
	return(pz);
}

#else
#   define job_set(x)
#   define job_reset(x)
#endif /* JOBS */


/*
 * wait built-in command
 */

void job_bwait(char *jobs[])
{
	register char *job;
	register struct process *pw;
	register pid_t pid;
	if(*jobs==0)
		job_wait((pid_t)-1);
	else while(job = *jobs++)
	{
#ifdef  JOBS
		if(*job == '%')
		{
			if(pw = job_bystring(job))
				pid = pw->p_pid;
			else
				return;
		}
		else
#endif /* JOBS */
			pid = atoi(job);
		job_wait(-pid);
	}
}



/*
 * get the process group given the job number
 * This routine returns the process group number or -1
 */

static struct process *job_bystring(ajob)
register char *ajob;
{
	register struct process *pw=job.pwlist;
	register int c;
	if(*ajob++ != '%' || pw==NULL)
		return(NULL);
	c = *ajob;
	if(isdigit(c))
		pw = job_byjid(atoi(ajob));
	else if(c=='+' || c=='%')
		;
	else if(c=='-')
	{
		if(pw)
			pw = job.pwlist->p_nxtjob;
	}
#ifdef	JOBS
	else
		pw = job_byname(ajob);
#endif
	if(pw && pw->p_flag)
		return(pw);
	return(NULL);
}

/*
 * Initialize the process posting array
 */

void	job_clear(void)
{
	register struct process *pw, *px;
	register struct process *pwnext;
	register int j = JBYTES;
	for(pw=job.pwlist; pw; pw=pwnext)
	{
		pwnext = pw->p_nxtjob;
		for(px=pw; px; px=px->p_nxtproc)
			free((void *)px);
	}
	job.pwlist = NULL;
	job.numpost=0;
#ifdef  KSH_88D
#ifdef  JOBS
        job.jobcontrol = 0;
#endif /* JOBS */
#endif /* KSH_88D */
	while(--j >=0)
		job.freejobs[j]  = 0;
}

/*
 * put the process <pid> on the process list and return the job number
 */

int job_post(pid)
pid_t pid;
{
	register struct process *pw;
	register struct history *fp = hist_ptr;
	if(pw=freelist)
		freelist = pw->p_nxtjob;
	else
		pw = new_of(struct process,0);
	job.numpost++;
	if(job.pipeflag && job.pwlist)
	{
		/* join existing current job */
		pw->p_nxtjob = job.pwlist->p_nxtjob;
		pw->p_nxtproc = job.pwlist;
		pw->p_job = job.pwlist->p_job;
	}
	else
	{
		/* create a new job */
		pw->p_nxtjob = job.pwlist;
		pw->p_job = job_alloc();
		pw->p_nxtproc = 0;
	}
	job.pwlist = pw;
	pw->p_pid = pid;
	pw->p_flag = P_RUNNING;
	if(st.states&MONITOR)
		pw->p_fgrp = job.curpgid;
	else
		pw->p_fgrp = 0;
	pw->p_pgrp = pw->p_fgrp;
#ifdef  KSH_88D
#ifdef DBUG
        p_setout(ERRIO);
        p_num(getpid(),':');
        p_str("posting",' ');
        p_num(pw->p_job,' ');
        p_str("pid",'=');
        p_num(pw->p_pid,' ');
        p_str("pgid",'=');
        p_num(pw->p_pgrp,NL);
        p_flush();
#endif /* DBUG */
#endif /* KSH_88D */
#ifdef JOBS
	if(fp!=NULL)
		pw->p_name=hist_position(fp->fixind-1);
	else
		pw->p_name = -1;
#endif /* JOBS */
        /*
         * Root is allowed many background jobs, but users are limited
         * by CHILD_MAX
         */
        if ( job.numpost >= CHILD_MAX-1 ) {
                if ( 0 == geteuid() ) {
                        if ( job.numpost >= MAXJ )
                                job_wait((pid_t)1);
                } else {
                        job_wait((pid_t)1);
                }
        }
	return(pw->p_job);
}

/*
 * Returns a process structure give a process id
 */

static struct process *job_bypid(pid)
pid_t 	pid;
{
	register struct process  *pw, *px;
	for(pw=job.pwlist; pw; pw=pw->p_nxtjob)
		for(px=pw; px; px=px->p_nxtproc)
		{
			if(px->p_pid==pid)
				return(px);
		}
	return(NULL);
}

/*
 * return a pointer to a job given the job id
 */

static struct process *job_byjid(jobid)
{
	register struct process *pw;
	for(pw=job.pwlist;pw; pw = pw->p_nxtjob)
	{
		if(pw->p_job==jobid)
			break;
	}
	return(pw);
}

#ifndef SIG_NORESTART
static jmp_buf waitintr;
static VOID interrupt()
{
	st.intfn = 0;
	longjmp(waitintr,1);
}
#endif /* SIG_NORESTART */

/*
 * Wait for process pid to complete
 * If pid < -1, then wait can be interrupted, -pid is waited for (wait builtin)
 * pid=0 to wait for any process
 * pid=1 to wait for at least one process to complete
 * pid=-1 to wait for all runing processes
 */

void	job_wait(pid)
register pid_t 	pid;
{
	register struct process *pw = NULL;
#if	( defined JOBS && defined _sgtty_ )
	union wait	wstat;
#else
	int 	wstat;
#endif
	register pid_t	myjob;
	register pid_t 	p;
	int		jobid = 0;
	int		w;
#ifdef WUNTRACED
	int		waitflag = WUNTRACED;
#endif /* WUNTRACED */
	char		intr = 0;
	char		bgjob = 0;
	if(pid==0)
	{
		if(!job.waitsafe)
#ifdef WNOHANG
			waitflag |= WNOHANG;
#else
			return;
#endif /* WNOHANG */
	}
	if(pid < 0)
	{
		pid = -pid;
		intr = 1;
#ifndef SIG_NORESTART
		st.intfn = interrupt;
		if(setjmp(waitintr))
			goto done;
#endif /* SIG_NORESTART */
	}
	if(pid > 1)
	{
		if((pw=job_bypid(pid))==NULL)
			return;
		jobid = pw->p_job;
		if(intr==0 && pw->p_pgrp)
			job_set(job_byjid(jobid));
		if(pw->p_flag&P_DONE)
			goto jobdone;
	}
#ifdef DBUG
	p_setout(ERRIO);
	p_num(getpid(),':');
	p_str(" job_wait job",'=');
	p_num(jobid,' ');
	p_str("pid",'=');
	p_num(pid,NL);
#endif /* DBUG*/
	p_flush();
#ifdef SIGCHLD
	sh_signal(SIGCHLD, SIG_DFL);
	job.waitsafe = 0;
#endif /* SIGCHLD */
	while(1)
	{
                if((p=waitpid((pid_t)-1, &w, waitflag )) == -1)
		{
			if(errno==EINTR &&!intr)
				continue;
			goto done;
		}
		if(p==0)
			goto done;
		if((pw = job_bypid(p))==0)
#ifdef JOBS
			goto tryagain;
#else
			continue;
#endif /* JOBS */
#if	( defined JOBS && defined _sgtty_ )
		wstat.w_status = w;
#else
		wstat = w;
#endif
                myjob= (pw->p_job==jobid);
#ifdef  KSH_88D
                if(!myjob && pw->p_pgrp && pw->p_pgrp != job.curpgid)
#else
                if(!myjob && pw->p_pgrp)
#endif /* KSH_88D */
			bgjob = 1;
		pw->p_flag &= ~P_RUNNING;
#ifdef DBUG
		p_num(getpid(),':');
		p_str(" job with pid",' ');
		p_num(p,' ');
		p_str("complete with status",'=');
		p_num(w,' ');
		p_str("jobid",'=');
		p_num(pw->p_job,NL);
		p_flush();
#endif /* DBUG*/
#ifdef SIGTSTP
		if (WIFSTOPPED(wstat))
		{
			pw->p_exit = WSTOPSIG(wstat);
			if(myjob && (pw->p_exit==SIGTTIN||pw->p_exit==SIGTTOU))
			{
				/* job stopped prematurely, restart it */
				killpg(pw->p_pgrp,SIGCONT);
				continue;
			}
			pw->p_flag |= (P_NOTIFY|P_SIGNALLED|P_STOPPED);
			if(myjob)
			{
#ifdef  KSH_88D
                                if((pw=job_byjid(jobid)) && (pw->p_flag&P_RUNNING))
                                                goto tryagain;
#endif /* KSH_88D */
				job_wait((pid_t)0);
				goto done;
			}
			goto tryagain;
		}
		else
#endif /* SIGTSTP */
		{
			if(p==sh.cpid)
			{
				io_fclose(COTPIPE);
				io_fclose(sh.cpipe[1]);
				sh.cpipe[1] = -1;
			}
			if ( !WIFEXITED (wstat))
			{
				pw->p_flag |= (P_DONE|P_SIGNALLED);
				if ( WIFSIGNALED (wstat) && WCOREDUMP (wstat))
					pw->p_flag |= P_COREDUMP;
				pw->p_exit = WTERMSIG (wstat);
				if(!myjob)
					pw->p_flag |= P_NOTIFY;
				else if(pw->p_exit==SIGINT)
					sh_fault(SIGINT); 
#ifdef JOBS
				if(myjob)
#endif /* JOBS */
				{
					p_setout(ERRIO);
					if(pw->p_exit!=SIGINT &&
						pw->p_exit!=SIGPIPE)
					{
						register char *msg;
						if(pid!=p||!(st.states&PROMPT))
						{
#ifdef  KSH_88D
                                                        char n[20];
                                                        sh_copy(sh_itos(p),n);
                                                        p_prp(n);
#else
                                                        p_prp(sh_itos(p));
#endif /* KSH_88D */
							p_str(e_nullstr,SP); /*NOTX*/
						}
						if(pw->p_flag&P_COREDUMP)
							p = 0;
						else
							p = NL;
#ifndef	_sys_siglist_
						msg = job_sigmsg((int)(pw->p_exit));
#else
						msg = sys_siglist[(int)(pw->p_exit)];
#endif	/* _sys_siglist_ */
						p_str(msg,p);
						if(p==0)
							p_str(MSGSTR(E_COREDUMP, (char *)e_coredump),NL); /*MSG*/
					}
#ifdef  KSH_88D
                                        if(!intr)
                                                pw = job_byjid(jobid);
#endif /* KSH_88D */
				}
			}
			else
			{
				pw->p_flag |= (P_DONE|P_NOTIFY);
				pw->p_exit = WEXITSTATUS(wstat);
			}
		}
		if(myjob)
		{
		jobdone:
#ifdef  KSH_88D
                        /* wstat is true if last process in pipeline */
                        if(wstat = (pw==job_byjid(jobid)))
                        {
                                sh.exitval = pw->p_exit;
                                if(pw->p_flag&P_SIGNALLED)
                                        sh.exitval |= SIGFAIL;
                        }
                        if(job_unpost(pw)) /* job is complete */
                                break;
                        else if(intr || (wstat && (!(st.states&MONITOR))))
                                break;
#else
			sh.exitval = job.pwlist->p_exit;
			if(job.pwlist->p_flag&P_SIGNALLED)
				sh.exitval |= SIGFAIL;
			if(job_unpost(pw)) /* job is complete */
				break;
			else if(intr || (job.pwlist==pw && (!(st.states&MONITOR))))
			{
				sh.exitval = pw->p_exit;
				break;
			}
#endif /* KSH_88D */
			continue;
		}
		else if(pw->p_pgrp==0 && p!=sh.subpid)
			job_unpost(pw);
#ifdef JOBS
	tryagain:
		if(pid==0)
		{
#   ifdef WNOHANG
			waitflag |= WNOHANG;
#   else
			sh_signal(SIGCHLD,sh_fault);
			if(!job.waitsafe)
				goto done;
			job.waitsafe = 0;
#   endif /* WNOHANG */
		}
#endif /* JOBS */
	}
	exitset();
done:
	sh.trapnote &= ~SIGSLOW;
#ifdef  SIGCHLD
	if(bgjob && st.trapcom[SIGCHLD])
	{
		st.trapflg[SIGCHLD] |= TRAPSET;
		sh.trapnote |= TRAPSET;
	}
#endif /* SIGCHLD */
#ifndef SIG_NORESTART
	st.intfn = 0;
#endif /* SIG_NORESTART */
#ifdef SIGCHLD
	sh_signal(SIGCHLD,sh_fault);
#endif /* SIGCHLD */
	if(pid>1 && intr==0 && pw->p_pgrp)
		job_reset(pw);
}

#ifdef SIGTSTP


/*
 * move job to foreground if bgflag == 0
 * move job to background if bgflag != 0
 */

job_switch(struct process *pw, int bgflag)
{
	register const char *msg;
	if(!pw || !(pw=job_byjid((int)pw->p_job)))
		return(1);
	p_setout(st.standout);
	if(bgflag)
	{
		p_sub((int)pw->p_job,'\t');
		msg = "&";
	}
	else
	{
		job_unlink(pw);
		pw->p_nxtjob = job.pwlist;
		job.pwlist = pw;
		msg = e_nullstr; /*NOTX*/
	}
	hist_list(pw->p_name,'&',";");
	p_str(msg,NL);
	p_flush();
	if(!bgflag)
		job_wait(pw->p_pid);
	else if(pw->p_flag&P_STOPPED)
		job_unstop(pw);
	return(0);
}


/*
 * Set the foreground group associated with a job
 */

static void job_fgrp(pw,new)
register struct process *pw;
register int new;
{
	for(; pw; pw=pw->p_nxtproc)
		pw->p_fgrp = new;
}

/*
 * turn off STOP state of a process group and send CONT signals
 */

static void job_unstop(px)
register struct process *px;
{
	register struct process *pw;
	register int num = 0;
	for(pw=px ;pw ;pw=pw->p_nxtproc)
	{
		if(pw->p_flag&P_STOPPED)
		{
			num++;
			pw->p_flag &= ~(P_STOPPED|P_SIGNALLED|P_NOTIFY);
#ifdef  KSH_88D
                        pw->p_flag |= P_RUNNING;
#endif /* KSH_88D */
		}
	}
	if(num!=0)
	{
		if(px->p_fgrp != px->p_pgrp)
			killpg(px->p_fgrp,SIGCONT);
		killpg(px->p_pgrp,SIGCONT);
	}
}
#endif	/* SIGTSTP */

/*
 * remove a job from table
 * If all the processes have not completed then unpost returns 0
 * Otherwise the job is removed and unpost returns 1.
 * pwlist is reset if the first job is removed
 */

static int job_unpost(pwtop)
register struct process *pwtop;
{
	register struct process *pw;
	/* make sure all processes are done */
#ifdef DBUG
	p_setout(ERRIO);
	p_num(getpid(),':');
	p_str(" unpost pid",'=');
	p_num(pwtop->p_pid,NL);
	p_flush();
#endif /* DBUG */
	pwtop = pw = job_byjid((int)pwtop->p_job);
	for(; pw && (pw->p_flag&P_DONE); pw=pw->p_nxtproc);
	if(pw)
		return(0);
	/* all processes complete, unpost job */
	job_unlink(pwtop);
	for(pw=pwtop; pw ; pw=pw->p_nxtproc)
	{
		pw->p_flag &= ~P_DONE;
		job.numpost--;
		pw->p_nxtjob = freelist;
		freelist = pw;
	}
#ifdef  KSH_88D
#ifdef DBUG
        p_str("free job",'=');
        p_num(pwtop->p_job,NL);
        p_flush();
#endif /* DBUG */
#endif /* KSH_88D */
	job_free((int)pwtop->p_job);
	return(1);
}

/*
 * unlink a job form the job list
 */

static void job_unlink(pw)
register struct process *pw;
{
	register struct process *px;
	if(pw==job.pwlist)
	{
		job.pwlist = pw->p_nxtjob;
		return;
	}
	for(px=job.pwlist;px;px=px->p_nxtjob)
		if(px->p_nxtjob == pw)
		{
			px->p_nxtjob = pw->p_nxtjob;
			return;
		}
}

/*
 * get an unused job number
 * freejobs is a bit vector, 0 is unused
 */

static int job_alloc()
{
	register int j=0;
	register unsigned mask = 1;
	register unsigned char *freeword;
	/* skip to first word with a free slot */
	while(job.freejobs[j] == 0xff)
		j++;
	freeword = &job.freejobs[j];
	j *= 8;
	for(j++;mask&(*freeword);j++,mask <<=1);
	*freeword  |=mask;
	return(j);
}

/*
 * return a job number
 */

static void job_free(n)
register int n;
{
	register int j = (--n)/8;
	register unsigned mask;
	n -= j*8;
	mask = 1 << n;
	job.freejobs[j]  &= ~mask;
}


#ifndef	_sys_siglist_

static char *job_sigmsg(sig)
int sig;
{
	static char signo[] = "Signal xxx";
	if(sig < (MAXTRAP-1) && sh.sigmsg[sig])
		return(sh.sigmsg[sig]);
	sh_copy(sh_itos(sig),signo+7);
	return(signo);
}
#endif	/* _sys_siglist_ */
