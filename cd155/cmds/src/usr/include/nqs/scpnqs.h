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
 
/*++ scpnqs.h - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/include/nqs/scpnqs.h,v $
 *
 * DESCRIPTION:
 *
 *
 *	SCP interface structures for NQS.
 *
 *
 *	Author:
 *	-------
 *	Clay Andreasen, Cray Research Incorporated
 *	July, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:44:25 $ $State: Exp $)
 * $Log: scpnqs.h,v $
 * Revision 1.3  1994/11/19  02:44:25  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:18:26  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  17:18:55  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:54:01  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  14:42:30  hender
 * Sterling version 4/22/87
 * 
 *
 */

#define PMAJOR	"/usr/spool/uscp"

/*
 *	Definition of structure returned by qstat for each defined queue.
 *	This is followed by a qstat_j structure for each job in the queue.
 */
struct	qstat_q {
	char	qname[16];		/* queue name */
	int	limit;			/* run limit of queue */
	int	running;		/* number of jobs running */
	int	queued;			/* number of jobs queued */
	int	status;			/* 0=off 1 = on */
};

/*
 *	Definition of structure returned by qstat for each job submitted
 *	throuch USCP.  Qstat compares the first part of the standard output
 *	pathname of the job with the string PMAJOR (defined above) to determine
 *	whether the job came through USCP.
 */
struct	qstat_j {
	char	jobname[8];		/* job name */
	char	user[8];		/* user name */
	char	state[8];		/* job state (in ASCII) */
	int	jobid;			/* UNICOS job id (if executing) */
	int	seqno;			/* NQS sequence number */
	int	nice;			/* nice value at initiation */
	int	fl;			/* memory field length (clicks) */
	int	cpu;			/* cpu used (sec) */
	int	limit;			/* cpu time limit (sec) */
	int	scpflag;		/* job came from SCP flag */
};

/*
 *	Definition of structure returned by qsub when executed with a '-*n'
 *	parameter.
 */
struct	qsub_scp {
	int	qsubpid;		/* Process ID of the qsub program */
	int	seqno;			/* NQS sequence number of the job */
	char	jobname[16];		/* Job name */
};
