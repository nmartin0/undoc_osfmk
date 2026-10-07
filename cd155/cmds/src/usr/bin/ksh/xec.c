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
static char rcsid[] = "@(#)$RCSfile: xec.c,v $ $Revision: 1.3 $ (OSF) $Date: 1994/11/19 01:27:41 $";
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

#include	<errno.h>
#include	"defs.h"
#include	"jobs.h"
#include	"sym.h"
#include	"test.h"
#include	"builtins.h"

#ifdef _sys_timeb_
#   define TIC_SEC		HZ
#   include	<sys/timeb.h>
#endif	/* _sys_timeb_ */

/* These routines are referenced by this module */
#ifdef DEVFD
    extern void	close_pipes();
#endif	/* DEVFD */
#ifdef VFORK
    extern int	vfork_check();
    extern int	vfork_save();
    extern void	vfork_restore();
#endif	/* VFORK */

static int trim_eq();
static char *word_trim();
static char locbuf[TMPSIZ]; 	/* store last argument here if it fits */
#ifdef  KSH_88D
static char pipeflag;
#endif /* KSH_88D */

#ifdef MSG
#include "ksh_msg.h" 
extern nl_catd catd;
#define MSGSTR(n,s) NLcatgets(catd,MS_KSH,n,s) 
#else
#define MSGSTR(n,s) s
#endif

/* ========	command execution	========*/

sh_exec(union anynode  *argt, int flags)
{
	/* `stakbot' is preserved by this routine */
	register union anynode *t;
	register int 	type = flags;
	STKPTR 	sav=stak_word();
	int errorflg = (type&ERRFLG);
	int execflg = (type&1);
	int lastpipe = (type&LASTPIPE);
	int mainloop = (type&PROMPT);
	unsigned save_states = (st.states&(ERRFLG|MONITOR|LASTPIPE));
#ifdef VFORK
	int v_fork;
#endif	/* VFORK */

	if(sh.trapnote&SIGSET)
		sh_exit(SIGFAIL);
	flags &= ~(LASTPIPE|PROMPT);
	st.states &= ~(ERRFLG|MONITOR);
	st.states |= ((save_states&flags)|lastpipe);
	if((t=argt) && st.execbrk==0 && !is_option(NOEXEC))
	{
		char	**com;
		int	argn;
		char *com0 = NIL;
		pid_t forkpid;
		jmp_buf retbuf;
		jmp_buf *savreturn;
		int save;
		int topfd;
#ifdef  KSH_88D
                struct fileblk *savio;
#else
                struct fileblk *savstandin;
#endif /* KSH_88D */
		if(lastpipe)
		{
			/* save state for last element of a pipe */
			topfd = sh.topfd;
			forkpid = job.parent;
			if(io_ftable[0])
				save = io_ftable[0]->flag;
			savreturn = sh.freturn;
#ifndef KSH_88D
                        savstandin = st.standin;
#endif /* KSH_88D */
			sh.freturn = (jmp_buf*)retbuf;
			save_states |= (st.states&PROMPT);
			st.states &= ~PROMPT;
#ifdef  KSH_88D
                        savio = sh.savio;
                        sh.savio = st.standin;
#endif /* KSH_88D */
			if(setjmp(retbuf))
				goto endpipe;
			if(!execflg)
				io_save(0,topfd);
			io_renumber(sh.inpipe[0],0);
		}
		type = t->tre.tretyp;
		sh.oldexit=sh.exitval;
		sh.exitval=0;
		sh.lastpath = 0;
		switch(type&COMMSK)
		{
			case TCOM:
			{
				register struct ionod	*io;
				register struct argnod	*argp;
#ifdef  KSH_88D
#ifdef POSIX
                                int command = 0;
#endif /* POSIX */
#endif /* KSH_88D */
				struct namnod* np;
				type >>= (COMBITS+1);
				st.cmdline = t->com.comline;
				com = arg_build(&argn,&(t->com));
				if((t->tre.tretyp & COMSCAN))
				{
				  argp = t->com.comarg;
				  if(argp && ((argp->argflag & A_RAW) == 0))
					type = sh_lookup(com[0],tab_builtins);
				  if(sh.trapnote & SIGSET)
					sh_exit(SIGFAIL);
				}
#ifdef  KSH_88D
#ifdef POSIX
                                while(type==SYSCOMMAND)
                                {
                                        command++;
                                        com++;
                                        argn--;
                                        type = sh_lookup(com[0],tab_builtins);
                                }
#endif /* POSIX */
#endif /* KSH_88D */
				com0 = com[0];
				io = t->tre.treio;
				if(argp = t->com.comset)
				{
					if(argn==0 || (!st.subflag && type>0 && type<=SYSSPECIAL))
					{
						env_setlist(argp, 0);
						argp = NULL;
					}
				}
				if((io||argn))
				{
					if(argn==0)
					{
						/* fake a built-in */
						argn=1;
						type = SYSMAX;
					}
					/* set +x doesn't echo */
					else if((type!=SYSSET) && is_option(EXECPR))
#ifdef  KSH_88D
#ifdef POSIX
                                                sh_trace(com-command,1);
#else
                                                sh_trace(com,1);
#endif /* POSIX */
#else
                                                sh_trace(com,1);
#endif /* KSH_88D */
					if(io)
						p_flush();
#ifdef  KSH_88D
#ifdef POSIX
                                        if(!command)
                                                goto dofunction;
                                dobuiltin:
#endif /* POSIX */
#endif /* KSH_88D */
					if(type)
					{
						/* failures on built-ins not fatal */
						jmp_buf retbuf;
						jmp_buf *savreturn = sh.freturn;
#ifdef  KSH_88D
                                                struct fileblk *saveio=sh.savio;
#endif /* KSH_88D */
						int indx = sh.topfd;
						int scope=0;
						if(st.subflag || type>SYSSPECIAL)
							sh.freturn = (jmp_buf*)retbuf;
#ifdef  KSH_88D
                                                sh.savio = st.standin;
#endif /* KSH_88D */
						if(setjmp(retbuf) == 0)
						{
							int flag = execflg;
							if(type<SYSNULL)
							{
								if(type==SYSLOGIN)
									flag=1;
								else if(type==SYSEXEC)
									flag=1+(com[1]==0);
							}
							else if(type>=SYSSPECIAL)
								st.states |= BUILTIN;
							if(io)
								indx = io_redirect(io,flag);
							if(argp)
							{
								nam_scope(argp);
								scope++;
							}
							sh_builtin(type,argn,com,t);
						}
#ifdef  KSH_88D
                                                sh.savio = saveio;
#endif /* KSH_88D */
						st.states &= ~BUILTIN;
						sh.freturn = savreturn;
						if(scope)
							nam_unscope();
						if(io)
							io_restore(indx);
						goto setexit;
					}
#ifdef  KSH_88D
#ifdef POSIX
                                        goto dosearch;
#endif /* POSIX */
#endif /* KSH_88D */
					/* check for functions */
				dofunction:
					if(np=nam_search(com0,sh.fun_tree,0))
					{
						int indx;
						if(np->value.namval.ip==0)
						{
							if(!nam_istype(np,N_FUNCTION))
#ifdef  KSH_88D
#ifdef POSIX
                                                                goto dobuiltin;
#else
                                                                goto dosearch;
#endif /* POSIX */
#else
                                                                goto dosearch;
#endif /* KSH_88D */
							path_search(com0,0);
							if(np->value.namval.ip==0)
								sh_fail(com0,MSGSTR(E_FOUND, (char *)e_found)); /*MSG*/
						}
						indx = io_redirect(io,execflg);

						sh_funct((union anynode*)(funtree(np))
							,com
							,flags|(int)(nam_istype(np,T_FLAG)?EXECPR:0)
							,t->com.comset);
						io_restore(indx);
						goto setexit;
					}
#ifdef  KSH_88D
#ifdef POSIX
                                        goto dobuiltin;
                                dosearch:
#endif /* POSIX */
#endif /* KSH_88D */
					if(strchr(com0,'/')==0)
					{
#ifdef  KSH_88D
#ifndef POSIX
                                        dosearch:
#endif /* POSIX */
#else
                                        dosearch:
#endif /* KSH_88D */
						if(path_search(com0,1))
				 		goto dofunction;
					}
				}
				else if(io==0)
				{
				setexit:
					sh.exitval &= EXITMASK;
					exitset();
					if(sh.trapnote || (sh.exitval && (st.states&ERRFLG)))
						sh_chktrap();
					break;
				}
				type = TCOM;
			}

		    case TFORK:
			{
				register pid_t parent;
				int no_fork;
				int pipes[2];
				io_sync();
				no_fork = (execflg && !(type&(FAMP|FPOU)) &&
					*st.trapcom==0);
				if(no_fork)
					job.parent=parent=0;
				else
				/* FORKLIM is the max period between forks -
					power of 2 usually.  Currently shell tries after
					2,4,8,16, and 32 seconds and then quits */
				{
					register unsigned forkcnt=1;
					if(type&FTMP)
					{
						io_linkdoc(st.iotemp);
						st.linked = 1;
					}
					if(type&FCOOP)
					/* set up pipe for cooperating process */
					{
						if(sh.cpid)
							sh_fail(MSGSTR(E_PEXISTS, (char *)e_pexists),NIL); /*MSG*/
						if(sh.cpipe[0]<=0)
						{
							/* first co-process */
							io_popen(pipes);
							sh.cpipe[0] = io_renumber(pipes[0],CINPIPE);
							sh.cpipe[1] = io_renumber(pipes[1],CINPIPE2);
						}
						sh.outpipe = sh.cpipe;
						io_popen(sh.inpipe=pipes);
						sh.inpipe[1] = io_renumber(sh.inpipe[1],COTPIPE);
					}
					/* async commands allow job control */
					if((type&FAMP) && !(type&(FCOMSUB|FPOU))
						&&!(st.states&PROFILE))
						st.states |= is_option(MONITOR);
#ifdef  KSH_88D
                                        nam_strval(RANDNOD);
#endif /* KSH_88D */
#ifdef VFORK
					if(v_fork=vfork_check(t))
						vfork_save();
					while((job.parent=parent=(v_fork?vfork():fork())) == -1)
#else
					while((job.parent=parent=fork()) == -1)
#endif	/* VFORK */
					{
						if((forkcnt *= 2) > FORKLIM)
						{
							register const char *msg;
							switch(errno)
							{
							case ENOMEM:
								msg = MSGSTR(E_SWAP, (char *)e_swap); /*MSG*/
								break;
							default:
							case EAGAIN:
								msg = MSGSTR(E_FORK, (char *)e_fork); /*MSG*/
								break;
							}
							sh_fail(msg,NIL);
						}
						if(sh.trapnote&SIGSET)
							sh_exit(SIGFAIL);
						alarm(forkcnt);
						pause(); 
						alarm((unsigned)0);
					}
				}
#ifdef SIGTSTP
				if(!job.jobcontrol)
#endif /* SIGTSTP */
				{
					/* disable foreground job control */
					if(!(type&FAMP))
						job_nonstop();
#   ifdef DEVFD
					else if(!(type&FINT))
						job_nonstop();
#   endif /* DEVFD */
				}
#ifdef  KSH_88D_PIPES
                                if(flags&MONITOR)
                                        job.pipeflag = pipeflag;
                                else
                                        job.pipeflag = 0;
#endif /* KSH_88D_PIPES */
				if(parent)
				/* This is the parent branch of fork
				 * It may or may not wait for the child
				 */
				{
					register int pno;
#ifdef VFORK
					if(v_fork)
						vfork_restore();
#endif	/* VFORK */
					forkpid = parent;
					/* first process defines process gid */
#ifdef JOBS
					if(!job.pipeflag)
						job.curpgid = parent;
                                        if (st.states&MONITOR) {
                                                if (setpgid (parent, job.curpgid) < 0 && errno == EPERM) {
                                                        job.curpgid = parent;
                                                        setpgid (parent, parent);
                                                }
                                        }

#endif /* JOBS */
					if(lastpipe && (st.states&MONITOR))
					{
						io_restore(topfd);
						forkpid = 0;
						execflg++;
					}
					if(type&FPCL)
						close(sh.inpipe[0]);
					if(type&FCOOP)
						sh.cpid = parent;
					else if(type&FCOMSUB)
						sh.subpid = parent;
					pno = job_post(parent);
					if(lastpipe)
#ifdef  KSH_88D_PIPES
                                                pipeflag = 0;
#else
                                                job.pipeflag = 0;
#endif /* KSH_88D_PIPES */
					if((type&(FAMP|FPOU))==0)
					{
#ifdef DEVFD
						close_pipes();
#endif	/* DEVFD */
						job_wait(parent);
#ifdef  KSH_88D
                                                /* invalidate tty state */
                                                tty_set(-1);
#endif /* KSH_88D */
					}
					if(type&FAMP)
					{
						sh.bckpid = parent;
#ifdef  KSH_88D
                                                job.curpgid = 0;
#endif /* KSH_88D */
						if(st.states&(PROFILE|PROMPT))
						{
							/* print job number */
							p_setout(ERRIO);
#ifdef JOBS
							p_sub(pno,'\t');
#endif /* JOBS */
							p_num(parent,NL);
						}
					}
#ifndef KSH_88D
					/* invalidate tty state */
					tty_set(-1);
#endif /* KSH_88D */
					sh_chktrap();
					break;
				}
				else
				/*
				 * this is the FORKED branch (child) of execute
				 */
				{
					register pid_t pid = getpid();

#ifdef  MSG
                                                /*  PTM 44953  */
                                        if (catd != (nl_catd)-1)
                                                catclose (catd);
                                        open_msg_catalog ();
#endif /* MSG */

					if(st.standout != 1)
						st.standout = io_renumber(st.standout,1);
#ifdef  KSH_88D
                                        st.states |= (FORKED|NOLOG);
#else
                                        st.states |= FORKED;
#endif /* KSH_88D */
					if(no_fork==0)
					{
#ifdef  KSH_88D
                                                sh.login_sh = 0;
#endif /* KSH_88D */
						st.states &= ~(RM_TMP|IS_TMP);
						io_settemp(pid);
						if(st.linked == 1)
						{
							io_swapdoc(st.iotemp);
							st.linked = 2;
						}
						else
							st.iotemp=0;
						job.waitsafe = 0;
					}
					else if(st.states&RM_TMP)
						rm_files(io_tmpname);
#ifdef ACCT
					suspacct();
#endif	/* ACCT */
					/* child should not unlink the tmpfile */
					/* Turn off INTR and QUIT if `FINT'  */
					/* Reset remaining signals to parent */
					/* except for those `lost' by trap   */
#ifdef VFORK
					sig_reset(1);
#else
					sig_reset(0);
#endif /* VFORK */
#ifdef  JOBS
					if(st.states&MONITOR)
					{
						int pgrp;
						if(job.pipeflag==0)
							pgrp = pid;
						else
							pgrp = job.curpgid;
                                                while(setpgid(0,pgrp) < 0 && pgrp != pid )
                                                       job.curpgid = pgrp = pid;
						setpgid(0,pgrp);
						if(!job.jobcontrol&&(type&FINT))
							goto closein;
#ifdef SIGTSTP
						if(!(type&FAMP))
                                                        if (tcsetpgrp(JOBTTY,pgrp) == -1)
                                                                sh_fail(MSGSTR(E_TCSETPGRP, (char *)e_tcsetpgrp), NIL);
#ifndef KSH_88D
                                                job.jobcontrol = 0;
#endif /* !KSH_88D */
						sh_signal(SIGTTIN,SIG_DFL);
						sh_signal(SIGTTOU,SIG_DFL);
						sh_signal(SIGTSTP,SIG_DFL);
#endif /* SIGTSTP */
					}
					else if(type&FINT)
#else
                                        if(type&FINT)
#endif /* JOBS */
					{
						/* default std input for & */
						sh_signal(SIGINT,SIG_IGN);
						sh_signal(SIGQUIT,SIG_IGN);
					closein:
						if(!st.ioset)
						{
							close(0);
							io_fopen(e_devnull); /*NOTX*/
						}
					}
					/* pipe in or out */
					if((type&FAMP) && is_option(BGNICE))
						nice(4);
					if(type&FPIN)
					{
						io_renumber(sh.inpipe[0],0);
						if((type&FPOU)==0||(type&FCOOP))
							close(sh.inpipe[1]);
					}
					if(type&FPOU)
					{
						io_renumber(sh.outpipe[1],1);
						io_pclose(sh.outpipe);
					}
					st.states &= ~MONITOR;
#ifdef  KSH_88D
#ifdef JOBS
                                        job.jobcontrol = 0;
#endif /* JOBS */
#ifdef  KSH_88D_PIPES
                                        pipeflag = 0;
#else
                                        job.pipeflag = 0;
#endif /* KSH_88D */
#else
                                        job.pipeflag = 0;
#endif /* KSH_88D */
					if(type!=TCOM)
						st.cmdline = t->fork.forkline;
					io_redirect(t->tre.treio,1);
					if(type!=TCOM)
					{
						/* don't clear job table for out
						   pipes so that jobs can be
						   piped
						 */
						if(no_fork==0 && (type&FPOU)==0)
							job_clear();
						sh_exec(t->fork.forktre,flags|1);
					}
					else if(com0!=ENDARGS)
					{
						off_option(ERRFLG);
						io_rmtemp((struct ionod*)0);
						path_exec(com,t->com.comset);
					}
					sh_done(0);
				}
			}

			case TSETIO:
			{
			/*
			 * don't create a new process, just
			 * save and restore io-streams
			 */
				int indx;
				st.cmdline = t->fork.forkline;
				indx = io_redirect(t->fork.forkio,execflg);
				sh_exec(t->fork.forktre,flags);
				io_restore(indx);
				break;
			}

			case TPAR:
				sh_exec(t->par.partre,flags);
				sh_done(0);
	
			case TFIL:
			{
			/*
			 * This code sets up a pipe.
			 * All elements of the pipe are started by the parent.
			 * The last element executes in current environment
			 */
				register union anynode *tf;
				int pvo[2];	/* old pipe for multi-pipeline */
				int pvn[2];	/* set up pipe */
				type = 1;
#ifdef  KSH_88D_PIPES
                                pipeflag = 0;
#else
                                job.pipeflag = 0;
#endif /* KSH_88D_PIPES */
				sh.inpipe = pvo;
				sh.outpipe = pvn;
				do
				{
					/* create the pipe */
					io_popen(pvn);
					tf = t->lst.lstlef;
					/* type==0 on multi-stage pipe */
					if(type==0)
						tf->fork.forktyp |= FPCL|FPIN;
					/* execute out part of pipe no wait */
					type = sh_exec(tf, MONITOR|errorflg);
					tf = t->lst.lstrit;
					t = tf->fork.forktre;
					/* save the pipe stream-ids */
					pvo[0] = pvn[0];
					/* close out-part of pipe */
					close(pvn[1]);
					/* pipeline all in one process group */
					job.topfd = sh.topfd;
#ifdef  KSH_88D_PIPES
                                        pipeflag++;
#else
                                        job.pipeflag++;
#endif /* KSH_88D_PIPES */
				}
				/* repeat until end of pipeline */
				while(!type && !tf->fork.forkio && t->tre.tretyp==TFIL);
				sh.inpipe = pvn;
				sh.outpipe = 0;
				if(type == 0)
				{
					/*
					 * execute last element of pipeline
					 * in the current process
					 */
					sh_exec(tf->fork.forktre,flags|(LASTPIPE));
#if     defined(JOBS) && defined(SIGTSTP)
                                        if(job.jobcontrol && sh.pid != tcgetpgrp(JOBTTY))
                                                tcsetpgrp ( JOBTTY, sh.pid );
#endif
                                }
				else
					/* execution failure, close pipe */
					io_pclose(pvn);
				break;
			}
	
			case TLST:
			{
				/*  a list of commands are executed here */
				do
				{
					sh_exec(t->lst.lstlef,errorflg);
					t = t->lst.lstrit;
				}
				while(t->tre.tretyp == TLST);
				sh_exec(t,flags);
				break;
			}
	
			case TAND:
				if(sh_exec(t->lst.lstlef,0)==0)
					sh_exec(t->lst.lstrit,flags);
				break;
	
			case TORF:
				if(sh_exec(t->lst.lstlef,0)!=0)
					sh_exec(t->lst.lstrit,flags);
				break;
	
			case TFOR:
			case TSELECT:
			{
				register char **args;
				register int nargs;
				struct namnod *n;
				char **arglist;
				struct dolnod	*argsav=NULL;
				struct comnod	*tp;
				char *nullptr = NULL;
#ifdef  KSH_88D
                                int refresh = 1;
                                char *cp;
#endif /* KSH_88D */
				if((tp=t->for_.forlst)==NULL)
				{
					args=st.dolv+1;
					nargs = st.dolc;
					argsav=arg_use();
				}
				else
				{
					args=arg_build(&argn,tp);
					nargs = argn;
				}
#ifndef KSH_88D
				if(type==TSELECT)
				{
					p_setout(ERRIO);
					p_list(nargs,(const char **)(arglist=args));
				}
#endif /* KSH_88D */
				n = env_namset(t->for_.fornam, sh.var_tree,P_FLAG|V_FLAG);
				st.loopcnt++;
				while(*args !=ENDARGS && st.execbrk == 0)
				{
					if(t->tre.tretyp==TSELECT)
					{
#ifdef  KSH_88D
                                                char *val;
#else
                                                char *val,*cp;
#endif /* KSH_88D */
						/* reuse register */
#define c	type
#ifdef  KSH_88D
                                                if(refresh)
                                                {
                                                       p_setout(ERRIO);
                                                       arglist=args;
                                                       p_list(nargs,(char **)arglist);
                                                       refresh = 0;
                                                }
#endif /* KSH_88D */
						sh_prompt(1);
						env_readline(&nullptr,0,R_FLAG);
						if(fiseof(io_ftable[0]))
						{
							sh.exitval = 1;
							ClearErr(io_ftable[0]);
							break;
						}
						if((val=nam_fstrval(REPLYNOD))==NULL)
							continue;
						else
						{
							if(*(cp=val) == 0)
							{
#ifdef  KSH_88D
                                                                refresh++;
                                                                goto checkbrk;
#else
								p_list(nargs,(const char **)args);
								continue;
#endif /* KSH_88D */
							}
#if (defined(NLS) || defined(KJI))
							/* okay, search is
							   ascii-trans, and
							   only valid input
							   is '0'-'9'	    */
#endif	/* NLS|KJI */
							while(c = *cp++)
							if(c < '0' && c > '9')
								break;
							if(c!=0)
								c = nargs;
							else
								c = atoi(val)-1;
							if(c<0 || c >= nargs)
								c = nargs;
							args += c;
						}
					}
#undef c
					nam_putval(n, *args);
#ifdef  KSH_88D
                                        sh_exec(t->for_.fortre,errorflg);
                                        if(t->tre.tretyp == TSELECT)
                                        {
                                                if((cp=nam_fstrval(REPLYNOD)) &&
 *cp==0)
                                                        refresh++;
                                                args = arglist;
                                        }
                                        else
                                                args++;
                                checkbrk:
#else
					if(t->tre.tretyp != TSELECT)
						args++;
					else
						args = arglist;
					sh_exec(t->for_.fortre,errorflg);
#endif /* KSH_88D */
					if(st.breakcnt<0)
						st.execbrk = (++st.breakcnt !=0);
					}
				if(st.breakcnt>0)
					st.execbrk = (--st.breakcnt !=0);
				st.loopcnt--;
				arg_free(argsav,0);
				break;
			}
	
			case TWH:
			{
				register int 	i=0;
				st.loopcnt++;
				while(st.execbrk==0 && (sh_exec(t->wh.whtre,0)==0)==(type==TWH))
				{
					i = sh_exec(t->wh.dotre,errorflg);
					if(st.breakcnt<0)
						st.execbrk = (++st.breakcnt !=0);
				}
				if(st.breakcnt>0)
					st.execbrk = (--st.breakcnt !=0);
				st.loopcnt--;
				sh.exitval= i;
				break;
			}
	
			case TIF:
				if(sh_exec(t->if_.iftre,0)==0)
					sh_exec(t->if_.thtre,flags);
				else if(t->if_.eltre)
					sh_exec(t->if_.eltre, flags);
				else
					sh.exitval=0; /* force zero exit for if-then-fi */
				break;
	
			case TSW:
			{
				char *r = word_trim(t->sw.swarg);
				t= (union anynode*)(t->sw.swlst);
				while(t)
				{
					register struct argnod	*rex=(struct argnod*)t->reg.regptr;
					while(rex)
					{
						register char *s;
						if(rex->argflag&A_MAC)
							s = mac_expand(rex->argval);
						else
							s = rex->argval;
#ifdef  KSH_88D
                                                type = (rex->argflag&A_RAW);
                                                if((type && eq(r,s)) ||
                                                        (!type && (strmatch(r,s)
                                                        || trim_eq(r,s))))
                                                {
                                                        do      sh_exec(t->reg.regcom,flags);
                                                        while(t->reg.regflag &&
                                                                (t=(union anynode*)t->reg.regnxt));
                                                        t=0;
                                                        break;
                                                }
#else
						if(((rex->argflag&A_RAW)==0 && strmatch(r,s))
							|| trim_eq(r,s))
						{
							sh_exec(t->reg.regcom,flags);
							t=0;
							break;
						}
#endif /* KSH_88D */
						else
							rex=rex->argnxt.ap;
					}
					if(t)
						t=(union anynode*)t->reg.regnxt;
				}
				break;
			}

			case TTIME:
#ifdef  KSH_88D
#ifdef POSIX
                                if(type!=TTIME)
                                {
                                        sh_exec(t->par.partre,0);
                                        sh.exitval = !sh.exitval;
                                        break;
                                }
#endif /* POSIX */
#endif /* KSH_88D */
			{
				/* time the command */
				struct tms before,after;
				clock_t at,bt;
#ifdef _sys_timeb_
				struct timeb tb,ta;
				ftime(&tb);
#endif	/* _sys_timeb_ */
				bt = times(&before);
				sh_exec(t->par.partre,0);
				at = times(&after);
#ifdef _sys_timeb_
				ftime(&ta);
				at = TIC_SEC*(ta.time-tb.time);
				at +=  ((TIC_SEC*((clock_t)(ta.millitm-tb.millitm)))/1000);
#else
				at -= bt;
#endif	/* _sys_timeb_ */
				p_setout(ERRIO);
				p_str(MSGSTR(E_REAL, (char *)e_real),'\t'); /*MSG*/
				p_time(at,NL);
				p_str(MSGSTR(E_USER, (char *)e_user),'\t'); /*MSG*/
				at = after.tms_utime - before.tms_utime;
				at += after.tms_cutime - before.tms_cutime;
				p_time(at,NL);
				p_str(MSGSTR(E_SYS, (char *)e_sys),'\t'); /*MSG*/
				at = after.tms_stime - before.tms_stime;
				at += after.tms_cstime - before.tms_cstime;
				p_time(at,NL);
				break;
			}
			case TPROC:
			{
				register struct namnod *np;
				register char *fname = ((struct procnod*)t)->procnam;
#if (defined(NLS) || defined(KJI))
				if(!NLSletter(NCdechr(fname)))
#else
				if(!isalpha(*fname))
#endif	/* NLS|KJI */
					sh_fail(fname, MSGSTR(E_IDENT, (char *)e_ident)); /*MSG*/
				np = env_namset(fname,sh.fun_tree,P_FLAG|V_FLAG);
				if(np->value.namval.rp)
				{
					register struct blk *bp;
					bp = (struct blk*)(np->value.namenv);
					while(bp)
					{
						free((void *)bp);
						bp = bp->word;
					}
				}
				else
					np->value.namval.rp = new_of(struct Ufunction,0);
				if(t->proc.procblk)
				{
					np->value.namenv = (char*)(t->proc.procblk);
					funtree(np) = (int*)(t->proc.proctre);
					np->value.namval.rp->hoffset = t->proc.procloc;
				}
				else
				{
					free((void *)np->value.namval.rp);
					np->value.namval.rp = 0;
				}
				nam_ontype(np,N_FUNCTION);
				break;
			}

#ifdef NEWTEST
			/* new test compound command */
			case TTST:
			{
				register int n;
				int nargs;
				char *left;
				/* we need the following for execution trace */
				static char *arg[6]=  {"[[ !"};
				static char unop[3]= "-?";
				;
				if (type&TPAREN)
				{
					sh_exec(t->lst.lstlef,0);
					n = !sh.exitval;
				}
				else
				{
					n = type>>TSHIFT;
					left = word_trim(&(t->lst.lstlef->arg));
					if(type&TUNARY)
					{
						nargs = 3;
						unop[1] = n;
						arg[1] = unop;
						arg[2] = left;
						n = unop_test(n,left);
					}
					else if(type&TBINARY)
					{
						nargs = 4;
						arg[1] = left;
						arg[2] = (char*)(test_optable+(n&017)-1)->sysnam;
						arg[3] = word_trim(&(t->lst.lstrit->arg));
						n = test_binop(n,left,
							word_trim(&(t->lst.lstrit->arg)));
					}
					arg[nargs] = "]]";
					arg[nargs+1] = 0;
					if(type&TNEGATE)
						arg[0][2] = ' ';
					else
						arg[0][2] = 0;
					sh_trace(arg,1);
				}
				sh.exitval = (!n^((type&TNEGATE)!=0));
				break;
			}
#endif /* NEWTEST */
		}
		/* set $. */
		if(mainloop && com0)
		if(com0)
		{
			if(sh.lastarg != locbuf)
				free((void *)sh.lastarg);
			if(strlen(com[argn-1]) < TMPSIZ)
				sh.lastarg = strcpy(locbuf,com[argn-1]);
			else
				sh.lastarg = sh_heap(com[argn-1]);
		}
		if(lastpipe)
		{
		endpipe:
			if(!execflg)
			{
				io_restore(topfd);
#ifdef  KSH_88D
                                type = sh.exitval;
                                job_wait(forkpid);
                                sh.exitval = type;
#else
                                job_wait(forkpid);
#endif /* KSH_88D */
			}
			if(io_ftable[0])
				io_ftable[0]->flag = save;
#ifdef  KSH_88D
                        sh.savio = savio;
#else
                        io_clear(savstandin);
#endif /* KSH_88D */
			st.states &= ~LASTPIPE;
			sh.freturn = savreturn;
#ifdef  KSH_88D_PIPES
                        pipeflag = 0;
#else
                        job.pipeflag = 0;
#endif /* KSH_88D_PIPES */
		}
		exitset();
	}
	stak_reset(sav);
	st.states |= save_states;
	st.linked = 0;
	if(sh.trapnote&SIGSET)
		sh_exit(SIGFAIL|sh.exitval);
	return(sh.exitval);
}

/*
 * test for equality with second argument trimmed
 * returns 1 if r == trim(s) otherwise 0
 */

static trim_eq(r,s)
register char *r,*s;
{
#if (defined(NLS) || defined(KJI))
	wchar_t c, rc;
	while (s += NCdec(s, &c), c > (wchar_t)0) {
		if (c == ESCAPE)
			s += NCdec(s, &c);
		r += NCdec(r, &rc);
		if (c != rc)
			return 0;
	}
#else
	register char c;
	while(c = *s++)
	{
		if(c==ESCAPE)
			c = *s++;
		if(c && c != *r++)
			return(0);
	}
#endif	/* NLS|KJI */
	return(*r==0);
}

/*
 * print out the command line if set -x is on
 */

int sh_trace(char **com, int nl)
{
	if(is_option(EXECPR))
	{
		p_setout(ERRIO);
                p_str((char *)mac_try(nam_fstrval((struct namnod *)PS4NOD)),(int)0);
		if(com)
			echo_list(1,com);
		if(nl)
			newline();
		return(1);
	}
	return(0);
}


static char *word_trim(arg)
register struct argnod *arg;
{
	register char *r = arg->argval;
	if(!(arg->argflag&A_RAW))
		r = mac_trim(r,0);
	return(r);
}

