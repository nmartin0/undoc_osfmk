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
/***
static char rcsid[] = "@(#)$RCSfile: proc.h,v $ $Revision: 1.2 $ (OSF) $Date: 1994/11/19 01:21:42 $";
***/
/*
 * COMPONENT_NAME: CMDCSH  c shell(csh)
 *
 * FUNCTIONS:
 *
 * ORIGINS: 10,26,27
 *
 * This module contains IBM CONFIDENTIAL code. -- (IBM
 * Confidential Restricted when combined with the aggregated
 * modules for this product)
 * OBJECT CODE ONLY SOURCE MATERIALS
 * (C) COPYRIGHT International Business Machines Corp. 1985, 1989
 * All Rights Reserved
 *
 * US Government Users Restricted Rights - Use, duplication or
 * disclosure restricted by GSA ADP Schedule Contract with IBM Corp.
 *
 * (Copyright statements and/or associated legends of other
 * companies whose code appears in any part of this module must
 * be copied here.)
 */

/*
 * IBM CONFIDENTIAL
 * Copyright International Business Machines Corp. 1989
 * Unpublished Work
 * All Rights Reserved
 * Licensed Material - Property of IBM
 */
/*
 * RESTRICTED RIGHTS LEGEND
 * Use, Duplication or Disclosure by the Government is subject to
 * restrictions as set forth in paragraph (b)(3)(B) of the rights in
 * Technical Data and Computer Software clause in DAR 7-104.9(a).
 */ 

/*
 * C shell - process structure declarations
 */

/*
 * Structure for each process the shell knows about:
 *	allocated and filled by pcreate.
 *	flushed by pflush; freeing always happens at top level
 *	    so the interrupt level has less to worry about.
 *	processes are related to "friends" when in a pipeline;
 *	    p_friends links makes a circular list of such jobs
 */
struct process	{
	struct	process *p_next;	/* next in global "proclist" */
	struct	process	*p_friends;	/* next in job list (or self) */
	struct	directory *p_cwd;	/* cwd of the job (only in head) */
	unsigned short p_flags;		/* various job status flags */
	char	p_reason;		/* reason for entering this state */
	char	p_index;		/* shorthand job index */
	pid_t	p_pid;
	pid_t	p_jobid;		/* pid of job leader */
	/* if a job is stopped/background p_jobid gives its pgrp */
	struct timeval	p_btime;	/* begin time */
	struct timeval	p_etime;	/* end time */
	struct rusage p_rusage;
	uchar_t	*p_command;		/* first PMAXLEN uchar_ts of command */
};

/* flag values for p_flags */
#define	PRUNNING	0x0001		/* running */
#define	PSTOPPED	0x0002		/* stopped */
#define	PNEXITED	0x0004		/* normally exited */
#define	PAEXITED	0x0008		/* abnormally exited */
#define	PSIGNALED	0x0010		/* terminated by a signal != SIGINT */

#define	PALLSTATES	(PRUNNING|PSTOPPED|PNEXITED|PAEXITED|PSIGNALED|PINTERRUPTED)
#define	PNOTIFY		0x0020		/* notify async when done */
#define	PTIME		0x0040		/* job times should be printed */
#define	PAWAITED	0x0080		/* top level is waiting for it */
#define	PFOREGND	0x0100		/* started in shells pgrp */
#define	PDUMPED		0x0200		/* process dumped core */
#define	PDIAG		0x0400		/* diagnostic output also piped out */
#define	PPOU		0x0800		/* piped output */
#define	PREPORTED	0x1000		/* status has been reported */
#define	PINTERRUPTED	0x2000		/* job stopped via interrupt signal */
#define	PPTIME		0x4000		/* time individual process */
#define	PNEEDNOTE	0x8000		/* notify as soon as practical */

#define	PNULL		(struct process *)0
#define	PMAXLEN		80

/* defines for arguments to pprint */
#define	NUMBER		01
#define	NAME		02
#define	REASON		04
#define	AMPERSAND	010
#define	FANCY		020
#define	SHELLDIR	040		/* print shell's dir if not the same */
#define	JOBDIR		0100		/* print job's dir if not the same */
#define	AREASON		0200

struct	process	proclist;		/* list head of all processes */
bool	pnoprocesses;			/* pchild found nothing to wait for */

struct	process *pholdjob;		/* one level stack of current jobs */

struct	process *pcurrjob;		/* current job */
struct	process	*pcurrent;		/* current job in table */
struct	process *pprevious;		/* previous job in table */

short	pmaxindex;			/* current maximum job index */

bool	timesdone;			/* shtimes buffer full ? */

int	psigint();
struct	process	*pgetcurr();
struct	process	*plookup();
struct	process *pfind();

