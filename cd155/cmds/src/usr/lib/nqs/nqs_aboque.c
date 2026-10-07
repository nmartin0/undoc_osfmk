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
 
/*++ nqs_aboque.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_aboque.c,v $
 *
 * DESCRIPTION:
 *
 *	Abort all running requests in an NQS queue.
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
 * REVISION HISTORY: ($Revision: 1.4 $ $Date: 1994/11/19 02:52:45 $ $State: Exp $)
 * $Log: nqs_aboque.c,v $
 * Revision 1.4  1994/11/19  02:52:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.3  1993/11/02  00:57:06  mwan
 * R1.2 mods
 *
 * Revision 1.2  1992/10/09  22:24:53  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:46  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:23  hender
 * Sterling version 4/22/87
 * 
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_aboque.c	50.3 (nqs_aboque.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#include <stdio.h>
#include <signal.h>
#include <errno.h>
#if	UNICOS
#include <sys/category.h>
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* Global vars */

extern char *asciierrno();		/* Return ASCII errno */
extern int errno;			/* System error number */
extern void nqs_vtimer();		/* Set virtual timer */
extern time_t time();			/* Time in seconds */
#ifdef SDSC
extern int kill_parallel ();
#endif

static char *info = "I$%s.\n";
static char *problem = "E$Problem sending signal in nqs_aboque.c.\n";

/*** nqs_aboque
 *
 *
 *	void nqs_aboque():
 *	Abort all running requests in an NQS queue.
 */
void nqs_aboque (queue, grace_period, requeue_flag)
struct queue *queue;			/* Pointer to queue structure */
int grace_period;			/* Number of seconds to wait */
					/* after sending SIGTERM to send */
					/* SIGKILL. */
short requeue_flag;			/* Boolean non-zero if aborted */
					/* requests are to be requeued */
{
	void nqs_kill();		/* Send a SIGKILL to all remaining */
					/* processes for each remaining */
					/* running request in queues which */
					/* are being aborted. */

	short sig;			/* Signal to send */
	time_t timeval;			/* Timer value */
	register short reqindex;	/* Index into the Runvars[] array */
					/* of the running structure for */
					/* the request being aborted */
	register struct request *req;	/* Ptr to request structure for */
					/* the request being aborted */

	if (queue->sigkill) {
	    /*
	     *  Multiple pending ABort Queue commands are NOT allowed
	     *  for the same queue.
	     */
	    return;
	}
	sig = SIGTERM;
	if (grace_period == 0) sig = SIGKILL;
	else {
	    time (&timeval);
	    timeval += grace_period;
printf ("I$nqs_aboque: cur_time=%d, alarm_time=%d\n", time (0), timeval);
	    nqs_vtimer (&timeval, nqs_kill);	/* Set timer for */
	    queue->sigkill = timeval;		/* SIGKILL. */
	}
	req = queue->runset;
	while (req != (struct request *) 0) {
	    /*
	     *  It looks like this request is still running, though
	     *  it could have finished running without our hearing
	     *  about it just yet.  Send sig to all of its processes.
	     */
	    reqindex = req->reqindex;
	    if (Runvars [reqindex].process_family == 0) {
printf ("I$nqs_aboque: Process_family = 0\n");
		/*
		 *  The process group/family of the server
		 *  has not yet been reported in....  Queue
		 *  the signal so that it can be sent later
		 *  upon receipt of the process group/family
		 *  packet for this request.
		 */
		if ((req->status & RQF_SIGQUEUED) == 0 ||
		     Runvars [reqindex].queued_signal != SIGKILL) {
		    /*
		     *  Queue the signal, unless a SIGKILL
		     *  has already been queued.
		     */
		    Runvars [reqindex].queued_signal = sig;
		}
		req->status |= RQF_SIGQUEUED;	/* Queued signal */
		if (requeue_flag) req->status |= RQF_SIGREQUEUE;
		else req->status &= ~RQF_SIGREQUEUE;
	    }
	    else {
		/*
		 *  The process group/family of the server
		 *  is known.  Send the signal.
		 */
#ifdef SDSC
		/* kill parallel apps first */ 
 
		if (Runvars [reqindex].part_id > 0) {
		    if (kill_parallel (Runvars [reqindex].part_id, sig) < 0)
			printf ("I$none killed in part %d\n",
			Runvars [reqindex].part_id);
		}
#endif 

#if	UNICOS
		if (killm(C_JOB, Runvars[reqindex].process_family, sig) == -1) {
#else
#if	SGI | SYS52 | UTS | OSF | OSF
		if (kill (-Runvars [reqindex].process_family, sig) == -1) {
#else
#if	BSD42 | BSD43 | ULTRIX
		if (killpg (Runvars [reqindex].process_family, sig) == -1) {
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
		    if (errno != ESRCH) {
			/*
			 *  Something went really wrong!
			 */
			printf (problem);
			printf (info, asciierrno ());
			fflush (stdout);
		    }
		    /*  Else the request is already done! */
		}
		else if (requeue_flag) req->status |= RQF_SIGREQUEUE;
		else req->status &= ~RQF_SIGREQUEUE;
	    }
	    req = req->next;		/* Get the next request in the */
	}				/* running set */
}


/*** nqs_kill
 *
 *
 *	void nqs_kill():
 *
 *	Terminate ALL remaining request processes for the NQS queues that
 *	have been aborted.
 */
static void nqs_kill()
{
	void nqs_killset();

	nqs_killset (Nonnet_queueset);	/* Non-network queues */
	nqs_killset (Net_queueset);	/* Network queues only */
}


/*** nqs_killset
 *
 *
 *	void nqs_killset():
 *
 *	Zealously murder ALL remaining request processes for a set of queues
 *	that have been aborted, and whose SIGKILL times have arrived.  We
 *	sally forth, bandying our bloody axe with the words "SIGKILL" written
 *	upon it.
 */
static void nqs_killset (queue)
register struct queue *queue;		/* Ptr to a queue structure */
{
	time_t timenow;			/* Time now */
	register short reqindex;	/* Index in the Runvars array of */
					/* running structure allocated for */
					/* the request being aborted */
	register struct request *req;	/* Ptr to request structure for */
					/* the request being aborted */

	time (&timenow);
printf ("I$nqs_killset:cur_time=%d\n", timenow);
	/*
	 *  Walk the queue set looking for aborted queues, whose SIGKILL
	 *  time has arrived.
	 */
	while (queue != (struct queue *) 0) {
	    if (queue->sigkill && queue->sigkill <= timenow) {
printf ("I$nqs_killset: found a queue to abort\n");
		/*
		 *  Swing the axe and cutlass!
		 */
		queue->sigkill = 0;	/* Clear SIGKILL abort flag */
		req = queue->runset;
		while (req != (struct request *) 0) {
		    /*
		     *  It looks like this request is still
		     *  running, though it could have finished
		     *  without our hearing about it just yet.
		     *  Send SIGKILL to all of its processes.
		     */
		    reqindex = req->reqindex;
		    if (Runvars [reqindex].process_family == 0) {
printf ("I$nqs_killset: Process_family = 0\n");
			/*
			 *  The process group/family of the
			 *  server has not yet been reported
			 *  in....  Queue the SIGKILL so that
			 *  it can be sent later upon receipt
			 *  of the process group/family
			 *  packet for this request.
			 */
			req->status |= RQF_SIGQUEUED;
			Runvars [reqindex].queued_signal = SIGKILL;
		    }
		    else {
printf ("I$nqs_killset: killing a req\n");
			/*
			 *  The process group/family of the
			 *  server is known.  Send SIGKILL.
			 */
#ifdef SDSC
			/* kill parallel apps first */ 
 
			if (Runvars [reqindex].part_id > 0) {
			    if (kill_parallel 
			    (Runvars [reqindex].part_id, SIGKILL) < 0)
				printf ("I$none killed in part %d\n",
				Runvars [reqindex].part_id);
			}
#endif 
#if	UNICOS
			if (killm(C_JOB, Runvars [reqindex].process_family,
				  SIGKILL) == -1) {
#else
#if	SGI | SYS52 | UTS | UNICOS | OSF
			if (kill (-Runvars [reqindex].process_family,
				  SIGKILL) == -1) {
#else
#if	BSD42 | BSD43 | ULTRIX
			if (killpg (Runvars [reqindex].process_family,
				    SIGKILL) == -1) {
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
			    if (errno != ESRCH) {
				/*
				 *  Something went really wrong!
				 */
				printf (problem);
				printf (info, asciierrno ());
				fflush (stdout);
			    }
			    /* Else the request is already done! */
			}
		    }
		    req = req->next;	/* Get the next request in the */
		}			/* running set */
	    }
	    else if (queue->sigkill) {
		/*
		 *  This queue has an abort SIGKILL time in the
		 *  future.  Set the virtual timer appropriately.
		 */
		nqs_vtimer (&queue->sigkill, nqs_kill);
	    }
	    queue = queue->next;	/* Get the next queue */
	}
}
