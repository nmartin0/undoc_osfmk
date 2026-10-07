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
 
/*++ nqs_delreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_delreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Delete an NQS request.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:51 $ $State: Exp $)
 * $Log: nqs_delreq.c,v $
 * Revision 1.3  1994/11/19  02:52:51  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:09  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:56  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:06  hender
 * Sterling version 4/22/87
 * 
 * 	Change per Intergraph Bill Mar, allow root (uid=0) to 
 *		delete request 		12/08/89	TAC
 *
 */

#if !defined(lint)
#if !defined SCCS
static char     sccs_id[] = "@(#)nqs_delreq.c	50.2 (nqs_delreq.c OSF/1 NQS2.0 GJK) 9/30/92";
#define SCCS
#endif
static char     module_name[] = __FILE__;
#endif

#if	UNICOS
#include <sys/category.h>
#endif
#include "nqs.h"			/* NQS constants and data types */
#include "transactcc.h"			/* Transaction completion codes */
#include <signal.h>
#include "nqsxvars.h"			/* NQS external vars and dirs  */

extern void nqs_deque();		/* Dequeue the request */
					/* from the queue */
extern void nqs_disreq();		/* Dispose of request */
extern struct request *nqs_fndreq();	/* Find request by sequence# & mid */
extern void udb_qorder();		/* Update request queue ordering */
#ifdef SDSC
extern int kill_parallel ();
#endif

/*** nqs_delreq
 *
 *
 *	long nqs_delreq():
 *	Delete an NQS request.
 *
 *	Returns:
 *		TCML_NOSUCHREQ:	 if the specified request does not
 *				 exist on this machine.
 *		TCML_NOSUCHSIG:  if the specified request was running
 *				 or in transit, and a signal trans-
 *				 mission was specified for a signal
 *				 not recognized by the supporting
 *				 UNIX implementation.
 *		TCML_NOTREQOWN:	 if the mapped user-id does not match
 *				 the current mapped user-id of the
 *				 request owner.
 *		TCML_PEERDEPART: if the specified request is presently
 *				 being routed by a pipe queue.
 *		TCML_REQDELETE:	 if the specified request was deleted.
 *		TCML_REQRUNNING: if the specified request is running,
 *				 and no signal was specified.
 *		TCML_REQSIGNAL:	 if the request was signalled (or will
 *				 be signalled immediately upon receipt
 *				 of the process-family group.
 *
 */
long nqs_delreq (mapped_uid, orig_seqno, orig_mid, sig, state)
uid_t mapped_uid;			/* Mapped owner user-id */
long orig_seqno;			/* Req sequence number */
mid_t orig_mid;				/* Machine-id of request */
int sig;				/* Signal to send to processes */
					/* in a running request.  Sig = 0 */
					/* if no signal should be sent to */
					/* the request processes if running */
int state;				/* Request queue state: RQS_ */
{
	struct request *predecessor;	/* Predecessor in request set in */
					/* queue */
	struct request *req;		/* Ptr to request structure for */
					/* located req, if found */
	short reqindex;			/* Request index in Runvars if */
					/* the request is running, or */
					/* index in Runvars if subrequest */
					/* is transporting parent request */
					/* to its destination via a pipe/ */
					/* network queue pair */

	/*
	 *  Search for the request.
	 */
	if ((req = nqs_fndreq (orig_seqno, orig_mid, &predecessor,
			       &state)) == (struct request *) 0) {
		/*
		 *  The request was not found in any of the local queues.
		 */
		return (TCML_NOSUCHREQ);
	}

	/* Intergraph change: allow root to delete request */
	if (req->v1.req.uid != mapped_uid && mapped_uid != 0) {
		/*
		 *  This request cannot be affected by the client.
		 */
		return (TCML_NOTREQOWN);
	}
	if (state == RQS_DEPARTING) {
		/*
		 *  MOREHERE someday.  The batch request is returning
		 *  output files to the proper locations.  Send SIGTERM.
		 *
		 *	-- OR --
		 *
		 *  A request may be in the very process of transiting
		 *  to another machine.  Send SIGINT.
		 *
		 *  But for now, this ****TRIAGE**** implementation simply
		 *  returns peer-depart.
 		 */
		return (TCML_PEERDEPART);
	}
	if (state == RQS_RUNNING) {	/* The request is running, or */
					/* is being routed by a pipe queue */
		if (req->queue->q.type == QUE_PIPE ||
		   (req->status & RQF_PREDEPART)) {	/* ****TRIAGE**** */
			/*
			 *  The request is presently being routed by a
			 *  pipe queue.
			 *
			 *  This ****TRIAGE**** implementation does not
			 *  allow the deletion of requests presently
			 *  being routed by a pipe queue.
	 		 */
			return (TCML_PEERDEPART);
		}
		if (sig) {
			/*
			 *  The request is presently running, and is NOT
			 *  being routed by a pipe queue.
			 */
#if	BSD42 | BSD43 | ULTRIX
			if (sig < 1 || sig > SIGPROF) {
#else
#if	UNICOS
			if (sig < 1 || sig > SIGCLD) {
#else
#if	SGI | SYS52 | UTS 
			if (sig < 1 || sig > SIGPWR) {
#else
#if OSF
                        if (sig < 1 || sig > SIGCHLD) {
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
#endif
				/*
				 *  No such signal.
				 */
				return (TCML_NOSUCHSIG);
			}
			/*
			 *  Send the signal.  The signal may not necessarily
			 *  kill the request (unless it is of the SIGKILL (9)
			 *  variety).
			 */
			reqindex = req->reqindex;
			if (Runvars [reqindex].process_family == 0) {
				/*
				 *  The process group/family of the server
				 *  has not yet been reported in....  Queue
				 *  the signal so that it can be sent later
				 *  upon receipt of the process group/family
				 *  packet for this request.
				 */
				if ((req->status & RQF_SIGQUEUED) == 0) {
					/*
					 *  We are NOT allowed to overwrite
					 *  any previously pending signal!
					 *  For example, nqs_aboque() may
					 *  have queued a signal for this
					 *  request.  Nqs_aboque() always wins.
					 */
					req->status |= RQF_SIGQUEUED;
					Runvars [reqindex].queued_signal = sig;
				}
			}
			else {
				/*
				 *  The process group/family of the server
				 *  is known.  Send the signal.
				 */
#if	UNICOS
				killm (C_JOB,
					Runvars [reqindex].process_family, sig);
#else
#if	SGI | SYS52 | UTS | OSF
#ifdef SDSC
				/* kill parallel apps first */

				if (Runvars [reqindex].part_id > 0) {
				    if (kill_parallel 
				    (Runvars [reqindex].part_id, sig) < 0)
					printf ("I$none killed in part %d\n",
					Runvars [reqindex].part_id);
				}
#endif
				kill (-Runvars [reqindex].process_family, sig);
#else
#if	BSD42 | BSD43 | ULTRIX
				killpg(Runvars [reqindex].process_family, sig);
#else
BAD SYSTEM TYPE
#endif
#endif
#endif
			}
			return(TCML_REQSIGNAL);	/* Req has been signalled, */
						/* or req will be signalled*/
						/* immediately upon receipt*/
						/* of the process-family */
		}
		return (TCML_REQRUNNING);	/* Request not deleted */
	}
	/*
	 *  The request is not running.
	 *  Simply delete the request.
	 */
	nqs_deque (req);		/* Remove the request from the queue */
					/* setting the QUE_UPDATE bit */
	nqs_disreq (req, 1);		/* Dispose of request */
	udb_qorder (req->queue);	/* Update queue image in database */
	return (TCML_REQDELETE);	/* Request deleted */
}
