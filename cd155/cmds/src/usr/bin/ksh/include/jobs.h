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
/* @(#)jobs.h	1.7  com/cmd/ksh/sh,3.1,9013 11/6/89 19:36:34 */
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
 *	UNIX shell
 *	S. R. Bourne
 *	rewritten by David Korn
 *
 */

#define LOBYTE	0377
#define MAXMSG	25
#define JOBTTY	2

#include	"sh_config.h"
#ifdef JOBS
#   include	"terminal.h"
#   undef ESCAPE

#   ifdef FIOLOOKLD
	/* Ninth edition */
	extern int tty_ld, ntty_ld;
#	define OTTYDISC	tty_ld
#	define NTTYDISC	ntty_ld
#	define killpg(pgrp,sig)		kill(-(pgrp),sig)
#   endif	/* FIOLOOKLD */
#   define job_nonstop()	(st.states &= ~MONITOR)
#   define job_terminate()	job_walk(job_kill,(int)SIGHUP,(char**)0)
			 /* send a hang-up signal to each of the jobs */
#else
#   undef SIGTSTP
		/*	BMW_ADDITION
#   undef SIGCHLD
#   undef SIGCLD
		*/
#   undef MONITOR
#   define MONITOR	0
#   define job_nonstop()
#   define job_set(x)
#   define job_reset(x)
#endif	/* JOBS */

#define MAXJ 2000	/* root can have many jobs. others limited by 
			   CHILD_MAX */

/* JBYTES is the number of char's needed for MAXJ bits */
#define JBYTES		(1+((MAXJ-1)/(8)))

struct process
{
	struct process *p_nxtjob;	/* next job structure */
	struct process *p_nxtproc;	/* next process in current job */
	pid_t		p_pid;		/* process id */
	pid_t		p_pgrp;		/* process group */
	unsigned char	p_job;		/* job number of process */
	pid_t		p_fgrp;		/* process group when stopped */
	unsigned char	p_flag;		/* flags - see below */
	unsigned short	p_exit;		/* exit value or signal number */
#ifdef JOBS
	off_t		p_name;		/* history file offset for command */
	struct termios	p_stty;		/* terminal state for job */
#endif /* JOBS */
};

/* Process states */

#define P_RUNNING	01
#define P_STOPPED	02
#define P_NOTIFY	04
#define P_SIGNALLED	010
#define P_STTY		020
#define P_DONE		040
#define P_COREDUMP	0100
#define P_BYNUM		0200

struct jobs
{
	struct process	*pwlist;	/* head of process list */
	pid_t		curpgid;	/* current process gid id */
	int		topfd;		/* fd of tty for pipelines  */
	pid_t		parent;		/* set by fork() */
	pid_t		mypid;		/* process id of shell */
	pid_t		mypgid;		/* process group id of shell */
	pid_t		mytgid;		/* terminal group id of shell */
	int		numpost;	/* number of posted jobs */
#ifdef JOBS
	int		suspend;	/* suspend character */
	int		linedisc;	/* line dicipline */
	char		jobcontrol;	/* turned on for real job control */
#endif /* JOBS */
	char		pipeflag;	/* set for pipelines */
	char		waitsafe;	/* wait will not block */
	unsigned char	freejobs[JBYTES];	/* free jobs numbers */
};

extern struct jobs job;

extern const char	e_coredump[];
#ifdef JOBS
extern const char	e_jobusage[];
extern const char	e_kill[];
extern const char	e_Done[];
extern const char	e_Running[];
extern const char	e_killcolon[];
extern const char	e_no_proc[];
extern const char	e_no_job[];
extern const char	e_runningj[];
extern const char	e_nlspace[];
extern const char	e_ambiguous[];
#ifdef SIGTSTP
   extern const char	e_no_start[];
   extern const char	e_terminate[];
   extern const char	e_no_jctl[];
#endif /* SIGTSTP */
#ifdef NTTYDISC
   extern const char	e_newtty[];
   extern const char	e_oldtty[];
#endif /* NTTYDISC */
#endif	/* JOBS */

/*
 * The following are defined in jobs.c
 */

#ifdef _NO_PROTO
    extern void job_clear();
    extern int	job_post();
    extern void job_bwait();
    extern int	job_walk();
    extern int	job_kill();
#else /* ~ _NO_PROTO */
    extern void job_clear(void);
    extern void job_bwait(char*[]);
    extern int	job_walk(int(*)(struct process*,int),int,char*[]);
    extern int	job_kill(struct process*,int);
#endif /* _NO_PROTO */
extern void	job_wait();

#ifdef JOBS
#ifdef _NO_PROTO
	extern void	job_init();
	extern int	job_close();
	extern int	job_list();
#else /* ~ _NO_PROTO */
	extern void	job_init(int);
	extern int	job_close(void);
	extern int	job_list(struct process*,int);
#endif /* _NO_PROTO */
#else
#	define job_init(flag)
#	define job_close()	(0)
#endif	/* JOBS */

#ifdef SIGTSTP
#ifdef _NO_PROTO
	extern int	job_switch();
#else /* ~ _NO_PROTO */
	extern int	job_switch(struct process*,int);
	
#endif /* _NO_PROTO */
#endif /* SIGTSTP */
