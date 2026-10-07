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
 * HISTORY
 * $Log: nqsacct.h,v $
 * Revision 1.2  1994/11/19  02:44:03  mtm
 * Copyright additions/changes
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqsacct.h	1.3 (nqsacct.h OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
#endif

#define NQSACCT_FILE	"/usr/adm/nqs"	/* file for NQS account records */
#define NQSACCT_INIT	1		/* job initiation record */
#define NQSACCT_FIN	2		/* job terminiation record */

/*
 *	NQS account file record header.
 */
struct	nqsacct	{
	int	type;			/* type of record */
	int	length;			/* length of record (bytes) */
	int	jobid;			/* UNICOS job id */
};

/*
 *	NQS account file record for job initiation.
 */
struct	nqsacct_init {
	struct	nqsacct h;      /* NQS account record header */
	char	user[16];		/* user name */
	char	queue[16];		/* NQS queue name */
	int	priority;		/* priority of NQS queue */
	long	sub_time;		/* time submitted */
	long 	start_time;		/* requested start time "-a" */
	long	init_time;		/* time initiated */
	int	orig_mid;		/* originating machine ID */
	char	scp_tid[8];		/* station TID */
	char	scp_mf[2];		/* station mainframe ID */
	long	fill;
};

/*
 * 	NQS account file record for job termination.
 */
struct 	nqsacct_fin {
	struct	nqsacct h;		/* NQS account record header */
	char	user[16];		/* user name */
	char 	queue[16];		/* NQS queue name */
	int 	priority;		/* priority of NQS queue */
#if    UNICOS | SGI | SYS52 | UTS | OSF
	long	tms_stime;		/* Time spent by system for this job */
	long	tms_utime;		/* Time spent by user for this job */
#else
#if	BSD42 | BSD43 | ULTRIX
	long	s_sec;			/* System time in seconds */
	long	s_usec;			/* System time in microseconds */
	long	u_sec;			/* User time in seconds */
	long	u_usec;			/* User time in microseconds */
#else
BAD SYSTEM TYPE
#endif
#endif
	int	orig_mid;		/* Originating Machine ID */
};
