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
 
/*++ nqs_wakreq.c - Network Queueing System
 *
 * $Source: /afs/ssd/i860/CVS/cmds_libs/src/usr/lib/nqs/nqs_wakreq.c,v $
 *
 * DESCRIPTION:
 *
 *	Wakeup requests that are being held by -a times which have now
 *	occurred.
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
 * REVISION HISTORY: ($Revision: 1.3 $ $Date: 1994/11/19 02:53:33 $ $State: Exp $)
 * $Log: nqs_wakreq.c,v $
 * Revision 1.3  1994/11/19  02:53:33  mtm
 * Copyright additions/changes
 *
 * Revision 1.2  1992/10/09  22:27:01  mwan
 * T6 freeze
 *
 * Revision 1.1  1992/09/24  18:57:25  rkl
 * Initial revision
 *
 * Revision 3.2  91/02/11  16:58:57  root
 * Version 2.0 Source
 * 
 * Revision 2.2  87/04/22  15:10:44  hender
 * Sterling version 4/22/87
 * 
 *
 */

#include "nqs.h"
#include "nqsxvars.h"			/* Global vars and directories */

extern void a2s_a2qset();		/* Add request to queued set */
extern void bsc_spawn();		/* Maybe spawn some batch reqs */
extern void dsc_spawn();		/* Maybe spawn some device reqs */
extern void nqs_vtimer();		/* Set virtual timer */
extern void psc_spawn();		/* Maybe spawn some pipe reqs */
extern time_t time();			/* GMT time since 0:00:00 Jan 1, 1970 */
extern void udb_qorder();		/* Update NQS database queue image */

/*** nqs_wakreq
 *
 *
 *	void nqs_wakreq():
 *
 *	Move all requests in all queue waiting sets whose -a times have
 *	occurred, to the eligible to run request sets in their respective
 *	queues.
 */
void nqs_wakreq()
{
	void wakeupset();		/* Wakeup requests in a queue set */

	wakeupset (Nonnet_queueset);	/* Wakeup requests in non-network */
					/* queues */
	wakeupset (Net_queueset);	/* Wakeup requests in network queues*/
}


/*** wakeupset
 *
 *
 *	void wakeupset():
 *
 *	Move all requests in all waiting sets whose -a times have occurred,
 *	to the eligible to run request sets in their respective queues, as
 *	present in the given queue set.
 */
static void wakeupset (queueset)
struct queue *queueset;			/* Queue set */
{
	register struct queue *queue;
	register struct request *prevreq;
	register struct request *req;
	time_t timenow;			/* Current GMT time */

	time (&timenow);		/* Get current time */
	queue = queueset;
	while (queue != (struct queue *) 0) {
		prevreq = (struct request *)0;
		req = queue->waitset;
		while (req != (struct request *)0) {
			if (req->start_time <= timenow) {
				/*
				 *  This request is now eligible for execution.
				 *  Remove the request from the waiting set and
				 *  place it in the eligible queue set.
				 */
				if (prevreq == (struct request *)0) {
					queue->waitset = req->next;
				}
				else prevreq->next = req->next;
				queue->q.waitcount--;
				/*
				 *  Add the request to the eligible to run
				 *  set.
				 */
				a2s_a2qset (req, queue);
				/*
				 *  Continue scanning the waiting set of
				 *  requests for the queue.
				 */
				if (prevreq == (struct request *)0) {
					req = queue->waitset;
				}
				else req = prevreq->next;
			}
			else {
				/*
				 *  This request has a start time in the
				 *  future.  Set the virtual alarm to wake
				 *  it up later.
				 */
				nqs_vtimer (&req->start_time, nqs_wakreq);
				prevreq = req;
				req = req->next;
			}
		}
		queue = queue->next;
	}
	/*
	 *  Request(s) may be eligible for spawning.
	 */
	bsc_spawn();			/* Maybe spawn batch reqs */
	dsc_spawn();			/* Maybe spawn device reqs */
	psc_spawn();			/* Maybe spawn pipe reqs */
	/*
	 *  Update queue states whose images are out of date in the NQS
	 *  database.
	 */
	queue = queueset;
	while (queue != (struct queue *)0) {
		if (queue->q.status & QUE_UPDATE) {
			/*
			 *  Update queue image and clear the QUE_UPDATE
			 *  bit.
			 */
			udb_qorder (queue);
		}
		queue = queue->next;
	}
}
