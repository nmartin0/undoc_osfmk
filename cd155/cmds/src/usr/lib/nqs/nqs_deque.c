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
 
/*++ nqs_deque.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_deque.c,v $
 *
 * DESCRIPTION:
 *
 *	Remove the specified request from its containing queue.
 *	The NQS database description of the queue is NOT updated
 *	by this function.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:52 $ $State: Exp $)
 * $Log: nqs_deque.c,v $
 * Revision 1.3  1994/11/19  02:52:52  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:25:14  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:58  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:05:14  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include <stdio.h>
#include "nqs.h"			/* NQS constants and data types */
#include "nqsxvars.h"			/* NQS global variables */

/*** nqs_deque
 *
 *
 *	void nqs_deque():
 *
 *	Remove the specified request from its containing queue.
 *	The NQS database description of the queue is NOT updated
 *	by this function.  However the QUE_UPDATE flag is SET
 *	to indicate that an update in required.
 */
void nqs_deque (request)
struct request *request;		/* Req structure */
{
	void deqerr();			/* Error display function */

	struct queue *queue;		/* Queue holding request */
	struct request *prevreq;	/* Previous request in queue */

	/*
	 *  Search the departing or staging-out set.
	 */
	queue = request->queue;		/* Containing queue */
	if (!located (queue->departset, request, &prevreq)) {
	    /*
	     *  The specified request is not in the departing or
	     *  staging-out set.  Check the running set (run set).
	     */
	    if (!located (queue->runset, request, &prevreq)) {
		/*
		 *  The specified request is not in the running request
		 *  set for the specified queue.  Check the stage-in set.
		 */
		if (!located (queue->stageset, request, &prevreq)) {
		    /*
		     *  The specified request is not in the stage-in request
		     *  set for the specified queue.  Check eligible to run
		     *  set (queued set).
		     */
		    if (!located (queue->queuedset, request, &prevreq)) {
			/*
			 *  The specified request is not in the eligible
			 *  run set.  Check the waiting set.
			 */
			if (!located (queue->waitset, request, &prevreq)) {
			    /*
			     *  The specified request is not in the
			     *  set of waiting requests.  Check the hold
			     *  set.
			     */
			    if (!located (queue->holdset, request, &prevreq)) {
				/*
				 *  The specified request is not in set
				 *  of holding requests.  Search the
				 *  arriving set.
				 */
				if (!located (queue->arriveset, request,
					      &prevreq)) {
				    /*
				     *  The specified request is
				     *  not in the specified
				     *  queue!
				     */
				    deqerr();
				    return;
				}
				if (prevreq == (struct request *) 0) {
				    queue->arriveset=request->next;
				}
				else prevreq->next = request->next;
				queue->q.arrivecount--;
			    }
			    else {
				/*
				 *  The specified request was located
				 *  in the holding set.
				 */
				if (prevreq == (struct request *) 0) {
				    queue->holdset = request->next;
				}
				else prevreq->next = request->next;
				queue->q.holdcount--;
			    }
			}
			else {
			    /*
			     *  The specified request was located in
			     *  the waiting set.
			     */
			    if (prevreq == (struct request *) 0) {
				queue->waitset = request->next;
			    }
			    else prevreq->next = request->next;
			    queue->q.waitcount--;
			}
		    }
		    else {
			/*
			 *  The specified request was located within
			 *  the eligible queued set.
			 */
			if (prevreq == (struct request *) 0) {
			    queue->queuedset = request->next;
			}
			else prevreq->next = request->next;
			queue->q.queuedcount--;
		    }
		}
		else {
		    /*
		     *  The specified request was located within the
		     *  staging-in request set.
		     */
		    if (prevreq == (struct request *) 0) {
			queue->stageset = request->next;
		    }
		    else prevreq->next = request->next;
		    queue->q.stagecount--;
		}
	    }
	    else {
		/*
		 *  The specified request was located within the running
		 *  request set for the specified queue.
		 */
		if (prevreq == (struct request *) 0) {
			queue->runset = request->next;
		}
		else prevreq->next = request->next;
		queue->q.runcount--;
	    }
	}
	else {
	    /*
	     *  The specified request was located with the departing or
	     *  staging-out set for the specified queue/
	     */
	    if (prevreq == (struct request *) 0) {
		queue->departset = request->next;
	    }
	    else prevreq->next = request->next;
	    queue->q.departcount--;
	}
	queue->q.status |= QUE_UPDATE;	/* Set update required flag */
}


/*** located
 *
 *
 *	int located():
 *
 *	Locate the specified request in the specified request chain.
 *	Returns:
 *
 *		1 if the request is present in the chain;
 *		0 otherwise.
 */
static int located (chain, target, ptrprevreq)
register struct request *chain;		/* Request chain */
register struct request *target;	/* Target request */
struct request **ptrprevreq;		/* Pointer of where to return */
					/* ptr to prev req in chain, if */
					/* successful */
{
	register struct request *prevreq;

	prevreq = (struct request *) 0;
	while (chain != target) {
		if (chain == (struct request *) 0) return (0);
		/*
		 *  Loop to locate request in chain.
		 */
		prevreq = chain;
		chain = chain->next;
	}
	*ptrprevreq = prevreq;
	return (1);
}


/*** deqerr
 *
 *
 *	void deqerr():
 *
 *	Report the error that the specified request is not in the
 *	specified queue, and therefore cannot be removed!
 */
static void deqerr()
{
	printf ("E$NQS internal error.\n");
	printf ("I$Attempt to remove request from wrong queue in ");
	printf ("nqs_deque().\n");
	fflush (stdout);
}
