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
 
/*++ nqs_a2s.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_a2s.c,v $
 *
 * DESCRIPTION:
 *
 *
 *	This module contains 7 externally visible procedures which
 *	add the given request to the appropriate queue/request set:
 *
 *		a2s_a2aset():	Add to arriving set;
 *		a2s_a2dset():	Add to departing/exiting
 *				(staging-out) set.
 *		a2s_a2hset():	Add to holding set;
 *		a2s_a2qset():	Add to queued set;
 *		a2s_a2rset():	Add to running set;
 *		a2s_a2sset():	Add to staging-in set;
 *		a2s_a2wset():	Add to waiting set;
 *
 *
 *	Author:
 *	-------
 *	Brent A. Kingsbury, Sterling Software Incorporated.
 *	March 13, 1986.
 *
 *
 * STANDARDS VIOLATIONS:
 *   None.
 *
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:52:45 $ $State: Exp $)
 * $Log: nqs_a2s.c,v $
 * Revision 1.3  1994/11/19  02:52:45  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:24:51  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:57:45  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:04:18  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"			/* NQS types and definitions */
#include "nqsxvars.h"			/* NQS global vars and dirs */

extern void nqs_vtimer();		/* Set virtual timer */
extern void nqs_wakreq();		/* NQS wakeup request function */


/*** a2s_a2aset
 *
 *
 *	void a2s_a2aset():
 *
 *	Add the given request to the arrive set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2aset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	void a2s_a2pset();		/* Add to set ordered by priority */
					/* and queued time */

	a2s_a2pset (req, queue, &queue->arriveset, &queue->q.arrivecount);
}


/*** a2s_a2dset
 *
 *
 *	void a2s_a2dset():
 *
 *	Add the given request to the depart/exiting set in the
 *	specified queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2dset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	void a2s_a2eset();		/* Add to set ordered by event time */

	a2s_a2eset (req, queue, &queue->departset, &queue->q.departcount);
}


/*** a2s_a2hset
 *
 *
 *	void a2s_a2hset():
 *
 *	Add the given request to the hold set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2hset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	void a2s_a2pset();		/* Add to set ordered by priority */
					/* and queued time */

	a2s_a2pset (req, queue, &queue->holdset, &queue->q.holdcount);
}


/*** a2s_a2qset
 *
 *
 *	void a2s_a2qset():
 *
 *	Add the given request to the queued set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2qset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	void a2s_a2pset();		/* Add to set ordered by priority */
					/* and queued time */

	a2s_a2pset (req, queue, &queue->queuedset, &queue->q.queuedcount);
}


/*** a2s_a2rset
 *
 *
 *	void a2s_a2rset():
 *
 *	Add the given request to the running set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 *
 *	Added Device type as per Intergraph Bill Mar 	TAC
 */
void a2s_a2rset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	register int i;
	struct qcomplex *qcomplex;      /* Queue complex pointer */
	void a2s_a2eset();		/* Add to set ordered by event time */

	if (queue->q.type == QUE_BATCH) {
		for (i = MAX_COMPLXSPERQ; --i >= 0;) {
			qcomplex = queue->v1.batch.qcomplex[i];
			if (qcomplex == (struct qcomplex *)0) continue;
			qcomplex->runcount++;
		}
	}
	else if (queue->q.type == QUE_DEVICE) {
		for (i = MAX_COMPLXSPERQ; --i >= 0;) {
			qcomplex = queue->v1.device.qcomplex[i];
			if (qcomplex == (struct qcomplex *)0) continue;
			qcomplex->runcount++;
		}
	}
	a2s_a2eset (req, queue, &queue->runset, &queue->q.runcount);
}


/*** a2s_a2sset
 *
 *
 *	void a2s_a2sset():
 *
 *	Add the given request to the staging-in set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2sset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	void a2s_a2eset();		/* Add to set ordered by event time */

	a2s_a2eset (req, queue, &queue->stageset, &queue->q.stagecount);
}


/*** a2s_a2wset
 *
 *
 *	void a2s_a2wset():
 *
 *	Add the given request to the waiting set in the specified
 *	queue.  The QUE_UPDATE bit is set for the queue.
 */
void a2s_a2wset (req, queue)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
{
	register struct request *prevreq;
	register struct request *walkreq;
	register short pri;

	/*
	 *  Insert the request by -a time and priority.
	 */
	prevreq = (struct request *) 0;
	walkreq = queue->waitset;
	while (walkreq != (struct request *)0 &&
	       walkreq->start_time < req->start_time) {
	    prevreq = walkreq;
	    walkreq = walkreq->next;
	}
	if (walkreq != (struct request *)0) {
	    if (walkreq->start_time == req->start_time) {
		if (queue->q.type == QUE_NET) {
		    /*
		     *  The request is being placed in a network queue,
		     *  and therefore it MUST be a subrequest.  Its priority
		     *  is inherited from its parent.
		     *
		     *  Search to add the request into the -a time
		     *  set by priority.
		     */
		    pri = req->v1.subreq.parent->v1.req.priority;
		    while (walkreq != (struct request *)0 &&
			   walkreq->start_time == req->start_time &&
			   walkreq->v1.subreq.parent->v1.req.priority >= pri) {
			prevreq = walkreq;
			walkreq = walkreq->next;
		    }
		}
		else {
		    /*
		     *  The request is not being place in a network queue,
		     *  and is therefore NOT a subrequest.
		     *
		     *  Search to add the request into the -a time
		     *  set by priority.
		     */
		    pri = req->v1.req.priority;
		    while (walkreq != (struct request *)0 &&
			   walkreq->start_time == req->start_time &&
			   walkreq->v1.req.priority >= pri) {
			prevreq = walkreq;
			walkreq = walkreq->next;
		    }
		}
	    }
	}
	/*
	 *  Add to set.
	 */
	if (prevreq == (struct request *)0) {
	    req->next = queue->waitset;
	    queue->waitset = req;
	}
	else {
	    req->next = prevreq->next;
	    prevreq->next = req;
	}
	queue->q.waitcount++;		/* One more request in the wait set */
	queue->q.status |= QUE_UPDATE;	/* Database image needs to be */
					/* updated */
	req->queue = queue;		/* Each request always has a pointer */
					/* to the queue in which it resides */
	/*
	 *  Set virtual timer to wakeup request.
	 */
	nqs_vtimer (&req->start_time, nqs_wakreq);
}


/*** a2s_a2eset
 *
 *
 *	void a2s_a2eset():
 *
 *	Add the request to the specified set in the given queue.
 *	The requests in the given request set are ordered by
 *	event time (the time at which this procedure is called).
 *
 *	NOTE:	This procedure can only be used for adding requests
 *		to a request set in the DEPARTING/EXITING, RUNNING/ROUTING,
 *		or STAGING states.
 *
 */
static void a2s_a2eset (req, queue, addtoset, setcount)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
struct request **addtoset;		/* Ptr to the chain of requests into */
					/* into which 'req' must be inserted */
short *setcount;			/* Ptr to short integer tallying the */
					/* number of requests in the set into*/
					/* 'req' is being inserted */
{
	register struct request *prevreq;
	register struct request *walkreq;

	/*
	 *  Insert the request by event time.
	 */
	prevreq = (struct request *) 0;
	walkreq = *addtoset;
	while (walkreq != (struct request *) 0) {
	    prevreq = walkreq;
	    walkreq = walkreq->next;
	}
	/*
	 *  Add to set.
	 */
	if (prevreq == (struct request *) 0) {
		req->next = *addtoset;
		*addtoset = req;
	}
	else {
		req->next = prevreq->next;
		prevreq->next = req;
	}
	++*setcount;			/* One more request in this set */
	req->queue = queue;		/* Each request always has a pointer */
					/* to the queue in which it resides */
	queue->q.status |= QUE_UPDATE;	/* Database image needs to be */
}					/* updated */


/*** a2s_a2pset
 *
 *
 *	void a2s_a2pset():
 *
 *	Add the request to the specified set in the given queue.
 *	The requests in the given request set are ordered by:
 *
 *		1. priority	AND
 *		2. queued-time (recorded in state_time).
 *
 *	NOTE:	This procedure can only be used for adding requests
 *		to a request set in the QUEUED, HOLDING or ARRIVING
 *		states.
 *
 */
static void a2s_a2pset (req, queue, addtoset, setcount)
struct request *req;			/* Request */
struct queue *queue;			/* The queue */
struct request **addtoset;		/* Ptr to the chain of requests into */
					/* into which 'req' must be inserted */
short *setcount;			/* Ptr to short integer tallying the */
					/* number of requests in the set into*/
					/* 'req' is being inserted */
{
	register struct request *prevreq;
	register struct request *walkreq;
	register short pri;
	register time_t sta;

	/*
	 *  Insert the request by priority and queued-time (as
	 *  recorded in state_time).
	 */
	prevreq = (struct request *) 0;
	walkreq = *addtoset;
	if (queue->q.type == QUE_NET) {
	    /*
	     *  The request is being placed in a network queue,
	     *  and therefore it MUST be a subrequest.  Its priority
	     *  is inherited from its parent.
	     */
	    pri = req->v1.subreq.parent->v1.req.priority;
	    while (walkreq != (struct request *)0 &&
		   walkreq->v1.subreq.parent->v1.req.priority > pri) {
		prevreq = walkreq;
		walkreq = walkreq->next;
	    }
	    if (walkreq != (struct request *)0) {
		if (walkreq->v1.subreq.parent->v1.req.priority == pri) {
		    /*
		     *  Search to add the request into this
		     *  priority set by the last changed state
		     *  time of its parent.
		     */
		    sta = req->v1.subreq.parent->v1.req.state_time;
		    while (walkreq != (struct request *)0 &&
			   walkreq->v1.subreq.parent->v1.req.priority == pri &&
			   walkreq->v1.subreq.parent->v1.req.state_time <=sta){
			prevreq = walkreq;
			walkreq = walkreq->next;
		    }
		}
	    }
	}
	else {
	    /*
	     *  The request is NOT a subrequest.
	     */
	    pri = req->v1.req.priority;
	    while (walkreq != (struct request *)0 &&
		   walkreq->v1.req.priority > pri) {
		prevreq = walkreq;
		walkreq = walkreq->next;
	    }
	    if (walkreq != (struct request *)0) {
		if (walkreq->v1.req.priority == pri) {
		    /*
		     *  Search to add the request into this
		     *  priority set by its last changed state
		     *  time.
		     */
		    sta = req->v1.req.state_time;
		    while (walkreq != (struct request *)0 &&
			   walkreq->v1.req.priority == pri &&
			   walkreq->v1.req.state_time <= sta) {
			prevreq = walkreq;
			walkreq = walkreq->next;
		    }
		}
	    }
	}
	/*
	 *  Add to set.
	 */
	if (prevreq == (struct request *)0) {
		req->next = *addtoset;
		*addtoset = req;
	}
	else {
		req->next = prevreq->next;
		prevreq->next = req;
	}
	++*setcount;			/* One more request in this set */
	req->queue = queue;		/* Each request always has a pointer */
					/* to the queue in which it resides */
	queue->q.status |= QUE_UPDATE;	/* Database image needs to be */
}					/* updated */
