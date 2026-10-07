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
 
/*++ nqs_family.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_family.c,v $
 *
 * DESCRIPTION:
 *
 *	Process a "report process family" packet from a shepherd process.
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	August 12, 1985.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.5 $ $Date: 1994/11/19 02:52:56 $ $State: Exp $)
 * $Log: nqs_family.c,v $
 * Revision 1.5  1994/11/19  02:52:56  mtm
 * Copyright additions/changes
 *
 * Revision 1.4  1993/11/02  00:57:12  mwan
 * R1.2 mods
 *
 * Revision 1.2  1992/10/09  22:25:29  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:06  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:06:03  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_family.c	1.2 (nqs_family.c OSF/1 NQS2.0 GJK) 6/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#if	UNICOS
#include <sys/category.h>
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */

extern void udb_reqpgrp();		/* Update request process-group */
#ifdef SDSC
extern int kill_parallel ();
#endif

/*** nqs_family
 *
 *
 *	void nqs_family():
 *	Process a "report process family" packet from a shepherd process.
 */
#ifdef SDSC
void nqs_family (shepherd_pid, process_family, part_id)
#else
void nqs_family (shepherd_pid, process_family)
#endif
int shepherd_pid;			/* Process-id of shepherd process */
int process_family;			/* "Process family" of server */
#ifdef SDSC
int part_id;
#endif
{
	struct queue *serverque;	/* Server was handling this queue */
	register struct queue *queue;	/* Ptr to queue structure */
	register int runcount;		/* Reqs running in a queue */
	register struct request *req;	/* Request structure */
	register short reqindex;	/* Index in Runvars of the running */
					/* structure allocated for the */
					/* request under inspection */

	queue = Nonnet_queueset;		/* First queue in queue set */
	serverque = (struct queue *)0;		/* Request queue not found */
	while (queue != (struct queue *)0 && serverque == (struct queue *)0) {
		runcount = queue->q.runcount;	/* # of requests running */
		req = queue->runset;		/* First run request */
		while (runcount--) {
			reqindex = req->reqindex;
			if (Runvars [reqindex].shepherd_pid == shepherd_pid) {
				/*
				 *  We have found the request!
				 */
				serverque = queue;
				runcount = 0;	/* Exit inner loop */
			}
			else req = req->next;
		}
		queue = queue->next;		/* Get next queue */
	}
	if (serverque == (struct queue *) 0) {
		/*
		 *  The request was not located in the non-network
		 *  queue set.  Scan the network queue set for the
		 *  request.
		 */
		queue = Net_queueset;		/* First queue in queue set */
		serverque = (struct queue *)0;	/* Request queue not found */
		while (queue != (struct queue *) 0 &&
		       serverque == (struct queue *) 0) {
			runcount = queue->q.runcount;
						/* # of requests running */
			req = queue->runset;	/* First run request */
			while (runcount--) {
				reqindex = req->reqindex;
				if (Runvars [reqindex].shepherd_pid
					== shepherd_pid) {
					/*
					 *  We have found the request!
					 */
					serverque = queue;
					runcount = 0;
						/* Exit inner loop */
				}
				else req = req->next;
			}
			queue = queue->next;	/* Get next queue */
		}
	}
	/*
	 *  Determine whether or not we found the request, and if it's
	 *  valid.
	 */
	if (serverque == (struct queue *) 0 ||	/* Request not found */
	    Runvars [reqindex].process_family != 0 || /*Family known already!*/
	    process_family <= 0) {
		/*
		 *  Bad family reported.
		 */
		printf ("E$Bad process family reported.\n");
		fflush (stdout);
		return;
	}
	/*
	 *  Req points to the request structure for the request whose
	 *  "process family" is being reported.
	 *
	 *  Serverque points to the queue structure for the queue
	 *  containing the request.
	 */
	if (req->status & RQF_SIGQUEUED) {
		/*
		 *  We have a queued "kill -<signal> request".
		 *  Kill the server process group/family.
		 */
#ifdef SDSC
		Runvars [reqindex].process_family = process_family;
						/* Store process-family */
		Runvars [reqindex].part_id = part_id;
		/* kill parallel apps first */

		if (Runvars [reqindex].part_id > 0) {
		    if (kill_parallel
		    (Runvars [reqindex].part_id, 
		    Runvars [reqindex].queued_signal) < 0)
			printf ("I$none killed in part %d\n",
			Runvars [reqindex].part_id);
		}
#endif 

#if	UNICOS
		killm(C_JOB, process_family, Runvars [reqindex].queued_signal);
#else
#if	SGI | SYS52 | UTS | OSF
		kill (-process_family, Runvars [reqindex].queued_signal);
#else
#if	BSD42 | BSD43 | ULTRIX
		killpg (process_family, Runvars [reqindex].queued_signal);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
		req->status &= ~RQF_SIGQUEUED;	/* Queued signal sent */
	}
	else {
		Runvars [reqindex].process_family = process_family;
						/* Store process-family */
		Runvars [reqindex].part_id = part_id;
		udb_reqpgrp (serverque, req);	/* Update the proper qentry */
	}					/* to show process group */
}
